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
/* �Ƶ�DnsFilter.c�� */
extern void dnsFilterInit(SystemProfile *oldProf);
#endif


static void add_access_ctrl(void);
/*
 * ɾ��prerouting��defense��
 */
extern void delDefenseJumpRule(void) {
    doSystem("iptables -t mangle -D PREROUTING -j %s 1>/dev/null 2>&1", DEFENSE_FILTER_CHAIN);
}

/*
 * ����prerouting��defense��
 */
extern void addDefenseJumpRule(void) {
    doSystem("iptables -t mangle -I PREROUTING -j %s 1>/dev/null 2>&1", DEFENSE_FILTER_CHAIN);
}

/*
 * �жϹ��������Ƿ��й��ܿ���
 * ����ֵ��
 *  1	-   ��ȫ�������п����Ĺ���
 *  2	-   ��ȫ���������й��ܶ��ر�
 */
extern int isAnyDefenseOn(void) {
    int rst = 0;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;

    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);

    if (!profSys) {
	return rst;
    }

    /* ���ʿ��Ʋ��� */
    if ((!rst) && (profSys->sysNetSe.accessCtrlFromIP !=0u) && (profSys->sysNetSe.accessCtrlEndIP != 0u)) {
	rst = 1;
    }

    /* ddos */
    if((!rst) && (profSys->sysNetSe.antiDDoSEn == FUN_ENABLE)
#if (HOTEL_PNP == FYES)
	    /*�Ƶ깦�ܿ�����������رա������lan�����ε�ip�޷�����*/
	    && (isHotelPnpEn() == 0)
#endif
      )
    {
	rst = 1;
    }

    /* ����� */
    if ((!rst) && (profSys->sysNetSe.antiBlasterEn == FUN_ENABLE)) {
	rst = 1;
    }
	
    /* Զ��ping */
    if ((!rst) && (profSys->sysNetSe.antiPingReplyEn == FUN_ENABLE)) {
	rst = 1;
    }

    return rst;
}
/*������ ��ε�����ͬ���� ֻ�ǲ�����ͬ*/
void LanArpDoSystem(long long BroadcastInterval, char *IP)
{
	char lanArpBroadcastCMD[MAX_COMMAND_LEN];

	snprintf(lanArpBroadcastCMD, MAX_COMMAND_LEN, "arping -q -I %s -t %lld %s &", getLanIfName(), BroadcastInterval, IP);
	doSystem(lanArpBroadcastCMD);
	
	return;
}
/******************************************************************
 * ����:  ARP�㲥
 * ����:  2012-9-11
 * ��ע:  LAN��ARP�㲥����
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
	lanArpBroadcastIntervalus = 1000 * profSys->sysNetSe.lanArpBroadcastInterval;/* arping -t Ϊus��web��ȡ��arp�㲥�����profile�е�Ϊms���������� ����1000*/

    profIF = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);/*��ȡlan������*/
    addrS.s_addr = profIF->ether.sta.ip;
	snprintf(lanArpBroadcastCmd, MAX_COMMAND_LEN, "kill -9 `ps | grep \"arping -q -I %s -t\" | sed \"s/^ *//g\"| sed \"s/ .*$//g\"` > /dev/null 2>&1", getLanIfName());/*ɱ��ԭ�е�ARP�㲥���̣�������������ʱ�䣬��IP����grep��ʱ�����Ҫ���ϱ�����old profile�� ������Ϊ������������� ȫ�Ǳ������ж�ûʲô���壬��Ҫ��4���жϣ�����ֻ����ǰ��һ���ַ������á�*/
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
    add_access_ctrl();/*����lan���豸���ʿ��ơ��ù������λ�ڸ����������*/

    if ((profSys->sysNetSe.antiDDoSEn == FUN_ENABLE)
#if (HOTEL_PNP == FYES)
	    /*�Ƶ깦�ܿ�����������رա������lan�����ε�ip�޷�����*/
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
 * ������ �����豸���ʿ�����Ӧiptables����
 * ������ ifname:�豸�ӿ� 
 * ���ߣ� bhou
 * ���ڣ� 2011-12-29
 * ��ע�� ���豸���ʹ������lan��
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

    /*��ʼ��*/
    memset(ip_dest,0,32);
    memset(ip_from, 0, 32);
    memset(ip_end, 0, 32);
    memset(&addrS, 0, sizeof(addrS));


    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    if( (profSys->sysNetSe.accessCtrlFromIP !=0u) && (profSys->sysNetSe.accessCtrlEndIP != 0u))
    {/*Ϊ0��ʾ������*/
	profIF = (InterfaceProfile *)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);/*lan������*/

	/*Ϊ�˷�ֹ�������õ���ʧ���ԭ����� ����豸���ʺ��û���Ҳ�޷������豸
	 * ������ӹ���ǰ����Ȼ�������ε��ж�*/

#if 0
	/*�������ε��жϰ��������ֽ������*/
	netMask = profIF->ether.sta.ip & profIF->ether.sta.nm;/*lan������*/
#if (MULTI_LAN == FYES)
	netMask2 = profIF->ether.sta.ip2 & profIF->ether.sta.nm2;/*lan������*/
	netMask3 = profIF->ether.sta.ip3 & profIF->ether.sta.nm3;/*lan������*/
	netMask4 = profIF->ether.sta.ip4 & profIF->ether.sta.nm4;/*lan������*/
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
	{/*������ifnameͬ���Ρ�������ܵ�����Ҳ�޷������豸*/
#endif
	    addrS.s_addr = profSys->sysNetSe.accessCtrlFromIP;
	    strcpy(ip_from, inet_ntoa(addrS));/*��ʼip�ִ�*/
	    addrS.s_addr = profSys->sysNetSe.accessCtrlEndIP;
	    strcpy(ip_end, inet_ntoa(addrS));/*����ip�ִ�*/
	    addrS.s_addr = profIF->ether.sta.ip;
	    strcpy(ip_dest, inet_ntoa(addrS));/*lan��ip�ִ�*/

#if (MULTI_LAN == FYES)
#if (IP_GRP == FYES)
#if (NET_SECURITY_WEB_ABS == FYES)
	    /*�����Ӧ����*/
	    doSystem("iptables -t mangle -A %s -m uttdev --is-lan-in -p tcp --dport 80 -m iprange ! --src-range %s-%s -m set --set %s dst -j DROP", DEFENSE_FILTER_CHAIN, ip_from, ip_end, SYS_LAN_GROUP);/*����http����*/
#endif
	    doSystem("iptables -t mangle -A %s -m uttdev --is-lan-in -p tcp -m multiport --destination-port 161,443,60023 -m iprange ! --src-range %s-%s -m set --set %s dst -j DROP", DEFENSE_FILTER_CHAIN, ip_from, ip_end, SYS_LAN_GROUP);/*���ӹ���*/
#if (SSH_CLI == FYES)
	    SSHRuleAdd(ip_from, ip_end);
#endif
#else
#error ipset has been removed
#endif
#else
#if (NET_SECURITY_WEB_ABS == FYES)
	    /*�����Ӧ����*/
	    doSystem("iptables -t mangle -A %s -i %s -p tcp --dport 80 -m iprange ! --src-range %s-%s -d %s -j DROP", DEFENSE_FILTER_CHAIN, getLanIfName(),ip_from,ip_end,ip_dest);/*����http����*/
#endif
/*�豸���ر���23�˿ڡ����Բ���Ҫ����*/
#if 0
	    doSystem("iptables -t mangle -A %s -i %s -p tcp --dport 23 -m iprange ! --src-range %s-%s -d %s -j DROP", DEFENSE_FILTER_CHAIN,ifname,ip_from,ip_end,ip_dest);/*����telnet����*/
#endif
	    doSystem("iptables -t mangle -A %s -i %s -p tcp --dport 161 -m iprange ! --src-range %s-%s -d %s -j DROP", DEFENSE_FILTER_CHAIN,getLanIfName(),ip_from,ip_end,ip_dest);/*����snmp����*/
#endif
#if 0
	}
	else
	{
	    /*��ͬ���Ρ�ֻ����������ڴ����á�����дflash*/
	    /*����Ϊ�˷�ֹ���豸�������õ��벻��ȷ������ܿ�����дflash������ܵ�����goaheadдflash������*/
	    profSys->sysNetSe.accessCtrlFromIP = 0u;
	    profSys->sysNetSe.accessCtrlEndIP = 0u;
	}
#endif
    }
    return;
}
extern void netSecuritySet(void)
{
    /* ��������ɾ��prerouting defense�� */
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
    /* �Ƶ�DnsFilter.c�� */
    if ((newProf->dnsFilterEn != oldProf->dnsFilterEn) ||
	(memcmp(&newProf->dnsFilterControl, &oldProf->dnsFilterControl, sizeof(DnsFilterControl))!=0))

    {
	    dnsFilterInit(oldProf);
	    return;
    }
#endif

    NET_SE_PRINTF("%s has been called!\n", __FUNCTION__);
    
    /* ���SysNetSecurity�ṹ��û�б仯, �˳� */
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
 *  	LanArpBroadcast()�˺���ֻ�ǳ�ʼ�����������е�APR����,
 *  	 �Ժ��޸ĺ������iptablesSecDefenseRun()����һ����
 *  	 �ڷ���һ�𿪻�ʱ�����˹�������������ܻᱻ��ʼ������.
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
 * ��ʼ��
 */
extern void funInitNetSecurity(void) {
    registerForProfileUpdates(MIB_PROF_SYSTEM, NULL, netSeProfileChange);
	LanArpBroadcast();
    return;
}
#endif
