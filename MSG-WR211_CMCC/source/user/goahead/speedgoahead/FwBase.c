#include "uttMachine.h"
#if (NET_SECURITY == FYES)
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <syslog.h>
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "argcmd.h"
#include "a2n.h"
#include "internet.h"
#include "utt_conf_define.h"

#ifdef  NET_SE_DBG
#define NET_SE_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define NET_SE_PRINTF(fmt, args...)
#endif

#if (SSH_CLI == FYES)
extern void SSHRuleAdd(char *ip_from, char *ip_end);
#endif

#if (HOTEL_PNP == FYES)
extern int isHotelPnpEn(void);
#endif

extern int headbits(unsigned int nm);
extern int _getWanLinkState(int wanNo);
#if 0
/* 移到DnsFilter.c中 */
extern void dnsFilterInit(SystemProfile *oldProf);
#endif


static void add_access_ctrl(void);
/*
 * 删除prerouting中defense链
 */
extern void delDefenseJumpRule(void) {
    doSystem("iptables -t mangle -D PREROUTING -j %s 1>/dev/null 2>&1", DEFENSE_FILTER_CHAIN);
}

/*
 * 增加prerouting中defense链
 */
extern void addDefenseJumpRule(void) {
    doSystem("iptables -t mangle -I PREROUTING -j %s 1>/dev/null 2>&1", DEFENSE_FILTER_CHAIN);
}

/*
 * 判断攻击防御是否有功能开启
 * 返回值：
 *  1	-   安全配置中有开启的功能
 *  2	-   安全配置中所有功能都关闭
 */
extern int isAnyDefenseOn(void) {
    int rst = 0;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;

    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);

    if (!profSys) {
	return rst;
    }

    /* 访问控制策略 */
    if ((!rst) && (profSys->sysNetSe.accessCtrlFromIP !=0u) && (profSys->sysNetSe.accessCtrlEndIP != 0u)) {
	rst = 1;
    }

    /* ddos */
    if((!rst) && (profSys->sysNetSe.antiDDoSEn == FUN_ENABLE)
#if (HOTEL_PNP == FYES)
	    /*酒店功能开启。则防御关闭。否则非lan口网段的ip无法上网*/
	    && (isHotelPnpEn() == 0)
#endif
      )
    {
	rst = 1;
    }

    /* 冲击波 */
    if ((!rst) && (profSys->sysNetSe.antiBlasterEn == FUN_ENABLE)) {
	rst = 1;
    }
	
    /* 远程ping */
    if ((!rst) && (profSys->sysNetSe.antiPingReplyEn == FUN_ENABLE)) {
	rst = 1;
    }

    return rst;
}
/*函数中 多次调用相同命令 只是参数不同*/
void LanArpDoSystem(long long BroadcastInterval, char *IP)
{
	char lanArpBroadcastCMD[MAX_COMMAND_LEN];

	snprintf(lanArpBroadcastCMD, MAX_COMMAND_LEN, "arping -q -I %s -t %lld %s &", getLanIfName(), BroadcastInterval, IP);
	doSystem(lanArpBroadcastCMD);
	
	return;
}
/******************************************************************
 * 函数:  ARP广播
 * 日期:  2012-9-11
 * 备注:  LAN口ARP广播功能
 *****************************************************************/
int LanArpBroadcast(void)
{
	struct in_addr addrS;
	InterfaceProfile *profIF = NULL;
	char lanArpBroadcastCmd[MAX_COMMAND_LEN];
    SystemProfile *profSys = NULL;
	long long lanArpBroadcastIntervalus;

    memset(&addrS, 0, sizeof(addrS));
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
	lanArpBroadcastIntervalus = 1000 * profSys->sysNetSe.lanArpBroadcastInterval;/* arping -t 为us，web获取的arp广播间隔和profile中的为ms，所以这里 乘以1000*/

    profIF = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);/*获取lan口配置*/
    addrS.s_addr = profIF->ether.sta.ip;
	snprintf(lanArpBroadcastCmd, MAX_COMMAND_LEN, "kill -9 `ps | grep \"arping -q -I %s -t\" | sed \"s/^ *//g\"| sed \"s/ .*$//g\"` > /dev/null 2>&1", getLanIfName());/*杀掉原有的ARP广播进程，后两个变量（时间，和IP）在grep的时候如果要加上必须是old profile的 这里因为是最后两个参数 全是变量，判断没什么意义，且要做4次判断，所以只查找前面一半字符串更好。*/
	doSystem(lanArpBroadcastCmd);
	if (profSys->sysNetSe.lanArpBroadcastEn == FUN_ENABLE) {
	    addrS.s_addr = profIF->ether.sta.ip;
		LanArpDoSystem(lanArpBroadcastIntervalus, inet_ntoa(addrS));
#if (MULTI_LAN == FYES)
		if (profIF->ether.sta.ip2 != 0) { 
	    	addrS.s_addr = profIF->ether.sta.ip2;
			LanArpDoSystem(lanArpBroadcastIntervalus, inet_ntoa(addrS));
		}
		if (profIF->ether.sta.ip3 != 0) {
	    	addrS.s_addr = profIF->ether.sta.ip3;
			LanArpDoSystem(lanArpBroadcastIntervalus, inet_ntoa(addrS));
		}
		if (profIF->ether.sta.ip4 != 0) {
	    	addrS.s_addr = profIF->ether.sta.ip4;
			LanArpDoSystem(lanArpBroadcastIntervalus, inet_ntoa(addrS));
		}
#endif
	}
	return 0;
}

/* add the defense fuction */
void iptablesSecDefenseRun(void)
{
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;

#if 0
    char *DDosEn=NULL, *WormEn, *PingEn, *LanIP=NULL, *LanNm=NULL;
#endif
#if 0
    /* PoeCliCnt is the PPPoE Client Number that user configure */
    char *PoeSerEn=NULL, *PoeSerStart=NULL, PoeSerEnd[32], *PoeCliCnt=PoeSerEnd;
#endif
    unsigned int i = 0U;
    char *lanIf;
    Uint32 wanCount = 0U;

    NET_SE_PRINTF("%s has been called\n", __FUNCTION__);
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);
    if (profSys == NULL) {
	return;
    }

    lanIf = (char *)getLanIfName();

    /* flush the rules in chain DEFENSE_FILTER_CHAIN */
    doSystem("iptables -t mangle -F %s  1>/dev/null 2>&1", DEFENSE_FILTER_CHAIN);
    add_access_ctrl();/*增加lan口设备访问控制。该规则必须位于该链条的最顶端*/

    if ((profSys->sysNetSe.antiDDoSEn == FUN_ENABLE)
#if (HOTEL_PNP == FYES)
	    /*酒店功能开启。则防御关闭。否则非lan口网段的ip无法上网*/
	    && (isHotelPnpEn() == 0)
#endif
       ) 
    {
	MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
	InterfaceProfile *profIF = NULL;
	struct in_addr lanIp;
	profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);

	if (profIF != NULL) {
	    /* do only when DDos is enabled */
	    lanIp.s_addr = profIF->ether.sta.ip;
	    /* enable the broadcast users */
	    doSystem("iptables -t mangle -A %s -i %s -d 255.255.255.255 -j RETURN", DEFENSE_FILTER_CHAIN, lanIf);
	    /* enable the lan ip range users */
#if (MULTI_LAN == FYES)
#if (IP_GRP == FYES)
	    doSystem("iptables -t mangle -A %s -i %s -m set --set %s src -j RETURN", DEFENSE_FILTER_CHAIN, 
	    lanIf, SYS_LAN_RANGE_GROUP);
#else
#error ipset has been moved
#endif
#else
	    doSystem( "iptables -t mangle -A %s -i %s -s %s/%d -j RETURN", DEFENSE_FILTER_CHAIN, lanIf, 
		    inet_ntoa(lanIp), headbits(ntohl(profIF->ether.sta.nm)));
#endif
	    /* other users should be forbidden */
	    doSystem("iptables -t mangle -A %s -i %s -j DROP", DEFENSE_FILTER_CHAIN, lanIf);
	}
    }

	
    wanCount = getWanSlotCount();
    if (profSys->sysNetSe.antiBlasterEn == FUN_ENABLE) {
        for(i = 1U; i <= wanCount; i++){
            if (_getWanLinkState(i) == 1) {
                doSystem("iptables -t mangle -A %s -i %s --fragment -p ICMP -j DROP", DEFENSE_FILTER_CHAIN, getWanIfNamePPP(i));
            }
        }
    }

    if (profSys->sysNetSe.antiPingReplyEn == FUN_ENABLE) {
        for(i = 1U; i <= wanCount; i++){
            if (_getWanLinkState(i) == 1) {
                doSystem("iptables -t mangle -A %s -i %s -p icmp --icmp-type 8 -j DROP", DEFENSE_FILTER_CHAIN, getWanIfNamePPP(i));
            }
        }
    }
	
	return;
}
/******************************************************************
 * 函数： 增加设备访问控制相应iptables规则
 * 参数： ifname:设备接口 
 * 作者： bhou
 * 日期： 2011-12-29
 * 备注： 该设备访问规则针对lan口
 *****************************************************************/
static void add_access_ctrl(void)
{
    InterfaceProfile *profIF = NULL;
    SystemProfile *profSys = NULL;
    char ip_dest[32];
    char ip_from[32];
    char ip_end[32];
#if 0
    IPADDR netMask = 0u;
#if (MULTI_LAN == FYES)
    IPADDR netMask2 = 0U, netMask3 = 0U, netMask4 = 0U;
#endif
#endif
    struct in_addr addrS;

    /*初始化*/
    memset(ip_dest,0,32);
    memset(ip_from, 0, 32);
    memset(ip_end, 0, 32);
    memset(&addrS, 0, sizeof(addrS));


    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    if( (profSys->sysNetSe.accessCtrlFromIP !=0u) && (profSys->sysNetSe.accessCtrlEndIP != 0u))
    {/*为0表示不控制*/
	profIF = (InterfaceProfile *)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);/*lan口配置*/

	/*为了防止。因配置导入失误等原因造成 添加设备访问后，用户再也无法访问设备
	 * 这里添加规则前。依然进行网段的判断*/

#if 0
	/*这里网段的判断按照网络字节序进行*/
	netMask = profIF->ether.sta.ip & profIF->ether.sta.nm;/*lan口网段*/
#if (MULTI_LAN == FYES)
	netMask2 = profIF->ether.sta.ip2 & profIF->ether.sta.nm2;/*lan口网段*/
	netMask3 = profIF->ether.sta.ip3 & profIF->ether.sta.nm3;/*lan口网段*/
	netMask4 = profIF->ether.sta.ip4 & profIF->ether.sta.nm4;/*lan口网段*/
#endif
	
	if( (((IPADDR)(profSys->sysNetSe.accessCtrlFromIP & profIF->ether.sta.nm) == netMask)
		&& ((IPADDR)(profSys->sysNetSe.accessCtrlEndIP & profIF->ether.sta.nm) == netMask))
#if (MULTI_LAN == FYES)
	|| (((IPADDR)(profSys->sysNetSe.accessCtrlFromIP & profIF->ether.sta.nm2) == netMask2)
		&& ((IPADDR)(profSys->sysNetSe.accessCtrlEndIP & profIF->ether.sta.nm2) == netMask2))
	|| (((IPADDR)(profSys->sysNetSe.accessCtrlFromIP & profIF->ether.sta.nm3) == netMask3)
		&& ((IPADDR)(profSys->sysNetSe.accessCtrlEndIP & profIF->ether.sta.nm3) == netMask3))
	|| (((IPADDR)(profSys->sysNetSe.accessCtrlFromIP & profIF->ether.sta.nm4) == netMask4)
		&& ((IPADDR)(profSys->sysNetSe.accessCtrlEndIP & profIF->ether.sta.nm4) == netMask4))
#endif
	    )
	{/*必须与ifname同网段。否则可能导致再也无法访问设备*/
#endif
	    addrS.s_addr = profSys->sysNetSe.accessCtrlFromIP;
	    strcpy(ip_from, inet_ntoa(addrS));/*起始ip字串*/
	    addrS.s_addr = profSys->sysNetSe.accessCtrlEndIP;
	    strcpy(ip_end, inet_ntoa(addrS));/*结束ip字串*/
	    addrS.s_addr = profIF->ether.sta.ip;
	    strcpy(ip_dest, inet_ntoa(addrS));/*lan口ip字串*/

#if (MULTI_LAN == FYES)
#if (IP_GRP == FYES)
#if (NET_SECURITY_WEB_ABS == FYES)
	    /*添加相应规则*/
	    doSystem("iptables -t mangle -A %s -m uttdev --is-lan-in -p tcp --dport 80 -m iprange ! --src-range %s-%s -m set --set %s dst -j DROP", DEFENSE_FILTER_CHAIN, ip_from, ip_end, SYS_LAN_GROUP);/*增加http规则*/
#endif
	    doSystem("iptables -t mangle -A %s -m uttdev --is-lan-in -p tcp -m multiport --destination-port 161,443,60023 -m iprange ! --src-range %s-%s -m set --set %s dst -j DROP", DEFENSE_FILTER_CHAIN, ip_from, ip_end, SYS_LAN_GROUP);/*增加规则*/
#if (SSH_CLI == FYES)
	    SSHRuleAdd(ip_from, ip_end);
#endif
#else
#error ipset has been removed
#endif
#else
#if (NET_SECURITY_WEB_ABS == FYES)
	    /*添加相应规则*/
	    doSystem("iptables -t mangle -A %s -i %s -p tcp --dport 80 -m iprange ! --src-range %s-%s -d %s -j DROP", DEFENSE_FILTER_CHAIN, getLanIfName(),ip_from,ip_end,ip_dest);/*增加http规则*/
#endif
/*设备都关闭了23端口。所以不需要控制*/
#if 0
	    doSystem("iptables -t mangle -A %s -i %s -p tcp --dport 23 -m iprange ! --src-range %s-%s -d %s -j DROP", DEFENSE_FILTER_CHAIN,ifname,ip_from,ip_end,ip_dest);/*增加telnet规则*/
#endif
	    doSystem("iptables -t mangle -A %s -i %s -p tcp --dport 161 -m iprange ! --src-range %s-%s -d %s -j DROP", DEFENSE_FILTER_CHAIN,getLanIfName(),ip_from,ip_end,ip_dest);/*增加snmp规则*/
#endif
#if 0
	}
	else
	{
	    /*不同网段。只是清空现有内存配置。但不写flash*/
	    /*这是为了防止若设备访问配置导入不正确。则可能开机就写flash。这可能导致与goahead写flash的死等*/
	    profSys->sysNetSe.accessCtrlFromIP = 0u;
	    profSys->sysNetSe.accessCtrlEndIP = 0u;
	}
#endif
    }
    return;
}
extern void netSecuritySet(void)
{
    /* 建立或者删除prerouting defense链 */
    if (isAnyDefenseOn()) {
	delDefenseJumpRule();
	addDefenseJumpRule();
    } else {
	delDefenseJumpRule();
    }

    iptablesSecDefenseRun();	
}
/* profile change */
static void netSeProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr ) {
   
    SystemProfile *newProf = (SystemProfile *)newProfileDataPtr;
    SystemProfile *oldProf = (SystemProfile *)oldProfileDataPtr;
    
#if 0
    /* 移到DnsFilter.c中 */
    if ((newProf->dnsFilterEn != oldProf->dnsFilterEn) ||
	(memcmp(&newProf->dnsFilterControl, &oldProf->dnsFilterControl, sizeof(DnsFilterControl))!=0))

    {
	    dnsFilterInit(oldProf);
	    return;
    }
#endif

    NET_SE_PRINTF("%s has been called!\n", __FUNCTION__);
    
    /* 如果SysNetSecurity结构体没有变化, 退出 */
    if (memcmp(&(newProf->sysNetSe), &(oldProf->sysNetSe), sizeof(SysNetSecurity)) == 0) {
	return;
    }
    switch(changeType) {
        case PROF_CHANGE_ADD:
            break;
        case PROF_CHANGE_EDIT:
	    NET_SE_PRINTF("%s: prof_change_edit!\n", __FUNCTION__);
	    netSecuritySet();
/* 	
 *  	LanArpBroadcast()此函数只是初始化攻击防御中的APR防御,
 *  	 以后修改和上面的iptablesSecDefenseRun()放在一起，现
 *  	 在放在一起开机时，除此功能外的其它功能会被初始化俩次.
 */
		LanArpBroadcast();
	    break;
        case PROF_CHANGE_DEL:
            break;
        case PROF_CHANGE_DELALL:
            break;
        default:
            break;
    };
    return;
}

/*
 * 初始化
 */
extern void funInitNetSecurity(void) {
    registerForProfileUpdates(MIB_PROF_SYSTEM, NULL, netSeProfileChange);
	LanArpBroadcast();
    return;
}
#endif
