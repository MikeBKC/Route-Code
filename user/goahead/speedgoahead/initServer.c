#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>
#include    <dirent.h>
#include    <unistd.h>
#include    <sys/ioctl.h>
#include    <linux/autoconf.h>
#ifndef CONFIG_RALINK_RT2880
#include    <net/if.h>
#endif
#include    <sys/socket.h>
#include    <netinet/in.h>
#include    <arpa/inet.h>
#include    "internet.h"
#include    <sys/types.h>
#include    <sys/ipc.h>
#include    <sys/msg.h>
#include    <sys/stat.h>
#include    <signal.h>
#include    <sys/shm.h>
#include    <linux/sockios.h>
#include    <linux/mii.h>
#include    <syslog.h>
#include    "utt_conf_define.h"
#include    "pppoeServer.h"
//#include    "ldra.h"

#include "uttMachine.h"
#if (UTT_NV_1800N != FYES)
#include    <linux/netfilter/nf_conntrack_common.h>
#endif
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "ifmisc.h"

#if (STATIC_ROUTE == FYES)
#include "StaticRoute.h"
#endif

#if (GROUP_BASE == FYES)
#include    "pdb.h"
#endif
//#define GROUP_CHAIN             "group"

#define USER_ALL_ADDRS_FOR_8_0

/*
 * 原来的firewall_init函数
 */
extern void iptablesFilterInit(void);

int WanLinkUp(int wanNo);
int WanLinkDown(int wanNo);
void EditStaticRoute(char *RouteRec, int flag);

/* initiate network time synchronization using ntpclient programe */
void ntp_sh(void)
{
    /* cmdstr used to storate commands */
    char cmdstr[256];
    int n;
    struct in_addr inaddr;
    MibProfileType mibType = MIB_PROF_NTP;
    NtpProfile *prof; 

#if (UTT_NV_1800N == FYES)
    FatFitSwitchProfile *profFft = NULL;
    profFft = (FatFitSwitchProfile*)ProfGetInstPointByIndex(MIB_PROF_FAT_FIT_SWITCH,0);
    if( profFft->fatEnable == 0 ){
        return;
    }
#endif

    memset(cmdstr, 0x00, sizeof(cmdstr));
    /* get NTP configurations */
    prof = (NtpProfile*)ProfGetInstPointByIndex(mibType, 0);
    if ((prof!=NULL) && (ProfInstIsFree(prof) == 0) && (prof->enable!=0)) {
        /* if it is enabled, start the ntpclient */
        /* restart the process */
        doSystem("killall -q ntpclient");
#if (DST == FYES)
	if (prof->dstEnable != 0) {
	    sprintf(cmdstr, "ntpclient -a -s -f -c 1 -h ");
	} else {
	    sprintf(cmdstr, "ntpclient -a -s -c 1 -h ");
	}
#else
        sprintf(cmdstr, "ntpclient -a -s -c 1 -h ");
#endif
	n = strlen(cmdstr);
        if(prof->NTPServerIP1!=0U){
	    inaddr.s_addr = prof->NTPServerIP1;
            sprintf(cmdstr+strlen(cmdstr), "%s,", inet_ntoa(inaddr));
        }
        if(prof->NTPServerIP2!=0U){
	    inaddr.s_addr = prof->NTPServerIP2;
            sprintf(cmdstr+strlen(cmdstr), "%s,", inet_ntoa(inaddr));
        }
        if(prof->NTPServerIP3!=0U){
	    inaddr.s_addr = prof->NTPServerIP3;
            sprintf(cmdstr+strlen(cmdstr), "%s", inet_ntoa(inaddr));
        }
        
	/*尾部可能为','*/
	if(cmdstr[strlen(cmdstr)-1]==',')
	{
	     cmdstr[strlen(cmdstr)-1]='\0';
	}
	if(strlen(cmdstr)!=n)
	{
	    sprintf(cmdstr+strlen(cmdstr), " -i 60 -z %d &", prof->time_zone);
	    doSystem(cmdstr);
	    sleep(1U);
	}
    }
}


#if 0
/* initiate wlan */
static void initWlanSec(void)
{
    char_t *apworkmode;
    apworkmode=nvram_bufget(RT2860_NVRAM, "apworkmode");
    doSystem("wlan.sh");
    /* enable apclient mode */
    if(strcmp(apworkmode,"5")==0) {
        initWan(4);
    }
    restart8021XDaemon(RT2860_NVRAM);
}
#endif

/*
 * hide the possible "error/warn" message when deleting a non-exist chain.
 */
#if 0
static void iptablesSecDefenseClear(void)
{
    doSystem("iptables -t mangle -D PREROUTING -j %s  1>/dev/null 2>&1", DEFENSE_FILTER_CHAIN);
    doSystem("iptables -F %s  1>/dev/null 2>&1", DEFENSE_FILTER_CHAIN);
}
#endif



static void iptablesAllMangleClear(void)
{
    int i=0;
    doSystem("iptables -t mangle -F %s 1>/dev/null 2>&1", DEFENSE_FILTER_CHAIN);
    
    doSystem("iptables -t mangle -F %s 1>/dev/null 2>&1", PRE_DROP_CHAIN);
    for(i=1; i<=getWanSlotCount(); i++) {
        doSystem("iptables -t mangle -F %s%d 1>/dev/null 2>&1", 
            CONNMARK_WAN_CHAIN, i);
    }
    doSystem("iptables -t mangle -F PREROUTING 1>/dev/null 2>&1");
    doSystem("iptables -t mangle -F OUTPUT 1>/dev/null 2>&1");
}

#if 0
/* add the ddos defense fuction*/
void iptablesSecDefenseRun(void)
{
    char *DDosEn=NULL, *WormEn, *PingEn, *LanIP=NULL, *LanNm=NULL;
    /* PoeCliCnt is the PPPoE Client Number that user configure */
    char *PoeSerEn=NULL, *PoeSerStart=NULL, PoeSerEnd[32], *PoeCliCnt=PoeSerEnd;
    unsigned int nlanIp=0U, nlanNm=0U, i=0U;
    char cmdstr[128];
    char *lanIf=getLanIfName();

    /* clear all the local variable */
    memset(cmdstr, 0, sizeof(cmdstr));
    memset(PoeSerEnd, 0, sizeof(PoeSerEnd));
    /* flush the rules in chain DEFENSE_FILTER_CHAIN */
    doSystem("iptables -t mangle -F %s  1>/dev/null 2>&1", DEFENSE_FILTER_CHAIN);
    DDosEn=nvram_bufget(RT2860_NVRAM ,"DDosDf");
    if (strcmp(DDosEn, "on") == 0) {
        /* do only when DDos is enabled */
        LanIP=nvram_bufget(RT2860_NVRAM, _CONF_LAN_IP);
        LanNm=nvram_bufget(RT2860_NVRAM, _CONF_LAN_NETMASK);
        nlanNm=ip2int(LanNm);
        /* enable the broadcast users */
        sprintf(cmdstr, "iptables -t mangle -A %s -i %s -d 255.255.255.255 -j ACCEPT", DEFENSE_FILTER_CHAIN,lanIf);
        doSystem(cmdstr);
        /* enable the lan ip range users */
        sprintf(cmdstr, "iptables -t mangle -A %s -i %s -s %s/%d -j ACCEPT", DEFENSE_FILTER_CHAIN,lanIf, LanIP, headbits(nlanNm));
        doSystem(cmdstr);
#if 0
        /* get the pppoe configurations */
        PoeSerEn=nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_ENABLE);
        if(strcmp(PoeSerEn, "on") == 0) {
            /* if pppoe is enable, make it be accpeted */
            PoeSerStart=nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_STARTIP);
            PoeCliCnt=nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_ONLINECOUNT);
            nlanIp=ip2int(PoeSerStart);
#if 0
            nlanIp+=atoi(PoeCliCnt);
            nlanIp = nlanIp + (unsigned int )(strtol(PoeCliCnt, NULL, 10));
#endif
            /* get the number of clients */
            unsigned int icnt=0U, j=0U;
            for(; PoeCliCnt[j]!='\0'; j++){
                icnt = icnt *10U + (unsigned int)PoeCliCnt[j];
                icnt -= 0x30U;
            }
            DBGPRINT("PoeCliCnt[%d]\n", icnt);
            /* nlanIp now is the end ip address 
             * of the pppoe clients */
            nlanIp += icnt -1U;
            int2ip(nlanIp, PoeSerEnd);
            DBGPRINT("PoeSerStart[%s],PoeSerEnd[%s]\n", PoeSerStart,PoeSerEnd);
            /* add the rule now, accpet this clients */
            sprintf(cmdstr, "iptables -t mangle -A %s -i %s -m iprange --src-range %s-%s -j ACCEPT", DEFENSE_FILTER_CHAIN, lanIf, PoeSerStart, PoeSerEnd);
            doSystem(cmdstr);
        }
#endif
        /* other users should be forbidden */
        sprintf(cmdstr, "iptables -t mangle -A %s -i %s -j DROP", DEFENSE_FILTER_CHAIN, lanIf);
        doSystem(cmdstr);
    }
    WormEn=nvram_bufget(RT2860_NVRAM ,"WormDf");
    if (strcmp(WormEn, "on") == 0) {
        for(i=1U; i<=getWanSlotCount(); i++){
            if ( _getWanLinkState(i)==1 ) {
                doSystem("iptables -t mangle -A %s -i %s --fragment -p ICMP -j DROP", DEFENSE_FILTER_CHAIN, getWanIfNamePPP(i));
            }
        }
    }
    PingEn=nvram_bufget(RT2860_NVRAM ,"PingDf");
    if (strcmp(PingEn, "on") == 0) {
        for(i=1U; i<=getWanSlotCount(); i++){
            if ( _getWanLinkState(i)==1 ) {
                doSystem("iptables -t mangle -A %s -i %s -p icmp --icmp-type 8 -j DROP", DEFENSE_FILTER_CHAIN, getWanIfNamePPP(i));
            }
	}
    }
}
#endif
#if (MULTI_LAN == FYES)
/*增加或删除过期通告规则链到iptables规则
 * action: Iptables_INS or Iptables_DEL*/
static void preDropChainAOrD(char action)
{
#if (IP_GRP == FYES)
    doSystem("iptables -t mangle -%c PREROUTING -m set ! --set %s dst -j %s 1>/dev/null 2>&1",action, SYS_LAN_GROUP, PRE_DROP_CHAIN);
#else
#error ipset has been moved
#endif
    return;
}
#else
/*增加或删除过期通告规则链到iptables规则
 * ipstr : lan口ip
 * action: Iptables_INS or Iptables_DEL*/
static void preDropChainAOrD(char* ipstr, char action)
{
    doSystem("iptables -t mangle -%c PREROUTING -d ! %s -j %s 1>/dev/null 2>&1",action, ipstr, PRE_DROP_CHAIN);
    return;
}
#endif

#if (WEB_AUTH == FYES)
#if (WIFIDOG_AUTH == FYES)
extern Uint32 getWebauthType();
#endif
/*
 * 判断web认证是否开启
 * 返回值：
 *  1	-   web认证开启
 *  0	-   web认证关闭
 */
static int isWAOn(void) {
    int rst = 0;
    WebAuthGlobalProfile *profWA = NULL;
    profWA = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(MIB_PROF_WEB_AUTH_GLOBAL, 0);
    if ((ProfInstIsFree(profWA) == 0) && (profWA->enabled == TRUE)
#if (WIFIDOG_AUTH == FYES)
	    && (getWebauthType() ==1)
#endif
	    ) {
	rst = 1;
    }

    return rst;
}
#endif

#if (PPPOE_ACCOUNT == FYES)
/*
 * 判断pppoe计费是否开启
 * 因为计费没有统一开关。故用 server的开启做判断
 */
static int isPoeAccOn(void) {
    int rst = 0;
    PPPoESrvGloProfile *prof = NULL;

    prof = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(MIB_PROF_PPPOE_SRV_GLO, 0);
    if ((ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE)) {
	/*开启*/
	rst = 1;
    }

    return rst;
}

#endif

extern int initRuleToPreDrop(char * oldip, char *newip)
{
    int isAdd = 0;
#if (MULTI_LAN == FYES)
    preDropChainAOrD(Iptables_DEL);/*先删除老的*/
#else
    preDropChainAOrD(oldip, Iptables_DEL);/*先删除老的*/
#endif

#if (WEB_AUTH == FYES)
    isAdd |= isWAOn();/*无认证账号的丢包用predrop*/
#endif

    if(isAdd)
    {/*如果需要则添加新的规则*/
#if (MULTI_LAN == FYES)
	preDropChainAOrD(Iptables_INS);
#else
	preDropChainAOrD(newip, Iptables_INS);
#endif
    }
    return 1;
}
#if (PPPOE_SERVER == FYES)
/* 新建只允许PPPoE上网链, 需要在加规则之前调用，mantis 9547 */
static void  iptablesPppoeGloInit(void)
{
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;

    /*
     * 初始化强制PPPoE 链表
     */
    doSystem("iptables -t filter -D FORWARD -j %s 1>/dev/null 2>&1", ONLYPPPOE_FORWARD);
    doSystem("iptables -t filter -F %s  1>/dev/null 2>&1; iptables -t filter -X %s  1>/dev/null 2>&1", ONLYPPPOE_FORWARD, ONLYPPPOE_FORWARD);
    doSystem("iptables -t filter -N %s 1>/dev/null 2>&1", ONLYPPPOE_FORWARD); 

    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS, 0);
    if ((ProfInstIsFree(profS) == 0) && (profS->head.active == 1) && (profS->onlyPppoe == 1)) {
	doSystem("iptables -t filter -D FORWARD  -j %s 1>/dev/null 2>&1", ONLYPPPOE_FORWARD);
	doSystem("iptables -t filter -I FORWARD  -j %s 1>/dev/null 2>&1", ONLYPPPOE_FORWARD);
    }
}
#endif

#if (NET_SECURITY == FYES)
extern int isAnyDefenseOn(void);
extern void addDefenseJumpRule(void);
extern void delDefenseJumpRule(void);
#endif

#if (UTT_NV_1800N != FYES)
/*
 * 旧的防火墙初始化函数,包括访问控制策略初始化，攻击防御初始化等,原函数名为firewall_init,因为函数功能不仅包含
 * 防火墙初始化，所以更名为iptablesFilterInit
 * 
 * 新防火墙(访问控制策略)初始化函数为fw_init，在原来基础上将关于IP过滤，URL过滤和关键字过滤的内容移到fw_init中
 * fw_init被iptablesFilterInit调用
 *
 */
extern void iptablesFilterInit(void)    
{
    int i;
#if 0
    /* 移到fw_init函数中,firewall.c中 */
	// make a new chain
	doSystem("iptables -t filter -N %s 1>/dev/null 2>&1", IPPORT_FILTER_CHAIN);
	doSystem("iptables -t filter -A FORWARD -j %s 1>/dev/null 2>&1", IPPORT_FILTER_CHAIN);
	doSystem("iptables -A FORWARD -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu 1>/dev/null 2>&1");
	iptablesAllFilterRun();
#endif
	char ip[16] = {0};
	/* 获得lan口IP */
	getIfIp(getLanIfName(), ip);


    /*
     * filter表初始化
     */
    doSystem("iptables -F -t filter 1>/dev/null 2>&1");
    doSystem("iptables -P INPUT ACCEPT");
    doSystem("iptables -P OUTPUT ACCEPT");
    doSystem("iptables -P FORWARD ACCEPT");
//    doSystem("iptables -A FORWARD -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu 1>/dev/null 2>&1");
#if (PPPOE_SERVER == FYES)
    iptablesPppoeGloInit();
#endif

	// init NAT(DMZ)
	// We use -I instead of -A here to prevent from default MASQUERADE NAT rule 
	// being in front of us.
	// So "port forward chain" has the highest priority in the system, and "DMZ chain" is the second one.
#if 0
	iptablesAllNATClear();
	doSystem("iptables -t nat -N %s 1>/dev/null 2>&1; iptables -t nat -I POSTROUTING 1 -j %s 1>/dev/null 2>&1", PORT_FORWARD_RULE_CHAIN, PORT_FORWARD_RULE_CHAIN);
	doSystem("iptables -t nat -N %s 1>/dev/null 2>&1; iptables -t nat -I PREROUTING 1 -j %s 1>/dev/null 2>&1", PORT_FORWARD_CHAIN, PORT_FORWARD_CHAIN);
	doSystem("iptables -t nat -N %s 1>/dev/null 2>&1; iptables -t nat -I POSTROUTING 2 -j %s 1>/dev/null 2>&1", PORT_FORWARD_CHAIN1, PORT_FORWARD_CHAIN1);
	doSystem("iptables -t nat -N %s 1>/dev/null 2>&1; iptables -t nat -I PREROUTING 2 -j %s 1>/dev/null 2>&1", DMZ_CHAIN, DMZ_CHAIN);
	doSystem("iptables -t nat -N %s 1>/dev/null 2>&1; iptables -t nat -I POSTROUTING 3 -j %s 1>/dev/null 2>&1", DMZ_CHAIN1, DMZ_CHAIN1);
        for(i=1; i<=MAX_UTT_WAN_IF; i++) {
            doSystem("iptables -t nat -N %s%d 1>/dev/null 2>&1; iptables -t nat -I POSTROUTING %d -j %s%d 1>/dev/null 2>&1", 
                    PORT_FORWARD_WAN_CHAIN, i, 3+i, PORT_FORWARD_WAN_CHAIN, i);
            doSystem("iptables -t nat -N %s%d 1>/dev/null 2>&1; iptables -t nat -I PREROUTING %d -j %s%d 1>/dev/null 2>&1", 
                    UPNP_CHAIN_WAN, i, i+2, UPNP_CHAIN_WAN, i);
        }
#endif
    //    mangleTableInit();
	iptablesAllMangleClear();

	doSystem("iptables -t mangle -N %s 1>/dev/null 2>&1", PRE_DROP_CHAIN);

	/*该链表放于所有规则之前，且除了初始化处，不应被随意清空*/
	initRuleToPreDrop(ip, ip);

	doSystem("iptables -t mangle -N %s 1>/dev/null 2>&1", DEFENSE_FILTER_CHAIN);
#if (NET_SECURITY == FYES)
	/* 攻击防御开启才建链, defense和preDrop链优先顺序，后配置先生效 */
	if (isAnyDefenseOn()) {
	    /* 先删再建，防止speedweb重启 */
	    delDefenseJumpRule();
	    addDefenseJumpRule();
	}
#endif
#if 0
        for(i=1; i<=getWanSlotCount(); i++) {
            doSystem("iptables -t mangle -N %s%d 1>/dev/null 2>&1; iptables -t mangle -A PREROUTING -j %s%d 1>/dev/null 2>&1", 
                    CONNMARK_WAN_CHAIN, i, CONNMARK_WAN_CHAIN, i);
        }
#endif
#if 1
#if 0
/*移到内核ct_to_sk_mark函数实现*/
        /*这里除了restore 规则添加的标记。还包括内核添加的。如qos内核模块*/
	doSystem("iptables -t mangle -I PREROUTING -j CONNMARK --restore-mark");
#endif

#if (UTT_NV_1800N != FYES)
#if (HOTEL_PNP != FYES )
#if 0
/*移至内核ipt_pre_routing_hook函数实现*/
        /*wan口进来的包 不走线路打标记对应的路由
	 * 否则wan口进来的包 直接从wan口出去了*/
	doSystem("iptables -t mangle -A PREROUTING ! -i %s -j MARK --and-mark 0x%x", getLanIfName(), (~IPRULE_CONNMARK_MSK));
#endif
#endif
#endif
#else
        doSystem("iptables -t mangle -A PREROUTING -i %s -j CONNMARK --restore-mark", getLanIfName());
#endif

#if 0
	/*移到内核ct_to_sk_mark函数实现*/
	doSystem("iptables -t mangle -A OUTPUT -j CONNMARK --restore-mark");
#endif

	//iptablesAllNATRun();
	//init_NatRule(Iptables_ADD);  /* when wanConnMode changed the interface(-o) should be change, so it is run then */
}
#endif

/* return the number of bit "1" in the head of nm *
 * headbits(0x8FFFFFFF) is 1 */
int headbits(unsigned int nm)
{
    int i;
    /* it is nothing, only a bit operation */
    for(i=0; ((nm&0x80000000U) != 0U) && (i<=32) ; i++){
        /* the first bit of the nm is 1 */
        nm<<=1U;
    }
    return i;
}

#if (HOTEL_PNP == FYES)
extern int isHotelPnpEn(void);
#endif
#if (DHCP_SERVER == FYES)
void init_dnsmasq(int wanNo)
{
    MibProfileType mibType = MIB_PROF_DHCP_SERVER;
    DhcpServerProfile *profDhcpS = NULL;
#if (GROUP_BASE == FYES)
    SystemProfile *profSys = NULL;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
#endif
    profDhcpS = (DhcpServerProfile*)ProfGetInstPointByIndex(mibType, 0);
    doSystem("killall -q dnsmasq");
#if (GROUP_BASE == FYES)
    doSystem("iptables -t %s -D INPUT -p udp --dport 53 -j UTTPOLICY >/dev/null 2>&1", PDB_NF_TABLE);
#endif
    if((profDhcpS->DnsProxyEn == FUN_ENABLE)
#if (HOTEL_PNP == FYES)
	    /*酒店功能开启。则dns必须开启*/
	    || (isHotelPnpEn() != 0)
#endif
      )
    {
	/* restart the dnsmasq process, if needed */
#ifdef DNSMASQ_MAX_QUERY
	doSystem("sleep 1 && dnsmasq -0 %d -u admin -r /etc/resolv.conf -i %s&", DNSMASQ_MAX_QUERY,getLanIfName());
#else
	doSystem("sleep 1 && dnsmasq -u admin -r /etc/resolv.conf -i %s&",getLanIfName());
#endif
#if (GROUP_BASE == FYES)
	if (profSys->UttNfRuleEn == FUN_ENABLE)
	{
	    doSystem("iptables -t %s -A INPUT -p udp --dport 53 -j UTTPOLICY", PDB_NF_TABLE);
	}
#endif
    }
}
#endif

#if (DHCP_POOL == FYES)
void init_dnsmasq(int wanNo)
{
#if (DNS_AGENCY == FYES)
    MibProfileType mibType = MIB_PROF_DNS_AGENCY;
    DnsAgencyProfile *prof = NULL;
#if (GROUP_BASE == FYES)
    SystemProfile *profSys = NULL;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
#endif
    prof = (DnsAgencyProfile *)ProfGetInstPointByIndex(mibType, 0);
    doSystem("killall -q dnsmasq");
#if (GROUP_BASE == FYES)
    doSystem("iptables -t %s -D INPUT -p udp --dport 53 -j UTTPOLICY >/dev/null 2>&1", PDB_NF_TABLE);
#endif
    if((prof->DnsProxyEn == FUN_ENABLE) 
#if (HOTEL_PNP == FYES)
	/*酒店功能开启。则dns必须开启*/
	|| (isHotelPnpEn() != 0)
#endif
     )
    {
        /* restart the dnsmasq process, if needed */
#ifdef DNSMASQ_MAX_QUERY
	doSystem("sleep 1 && dnsmasq -0 %d -u admin -r /etc/resolv.conf -i %s&", DNSMASQ_MAX_QUERY,getLanIfName());
#else
	doSystem("sleep 1 && dnsmasq -u admin -r /etc/resolv.conf -i %s&",getLanIfName());
#endif
#if (GROUP_BASE == FYES)
	if (profSys->UttNfRuleEn == FUN_ENABLE)
	{
	    doSystem("iptables -t %s -A INPUT -p udp --dport 53 -j UTTPOLICY", PDB_NF_TABLE);
	}
#endif
    }
#endif
}
#endif

#if 0
char* nvram_shmInit()
{
     int shm_id;
     char *tmp;
     char filenm[] = UTT_SHM_NVRAM_FILE;
     key_t key;
        key = ftok(filenm, 0);
    if(key == -1){
        perror("ftok error: ");
        return  NULL;
    }
    shm_id = shmget(key, UTT_MAX_NVRAM_SEG, IPC_CREAT | IPC_EXCL | 0666);
    if(shm_id == -1) {
        perror("speedweb shmget error: ");
        return  NULL;
    }
    tmp = (char*)shmat(shm_id,NULL,0);
    return tmp; 
}

int mqid;
void terminate_handler(int signo)
{
     msgctl(mqid,IPC_RMID,NULL);
     exit(0);
}

int main(int argc,char** argv)
{
    static char* shmData;   
    char filenm[] = UTT_MSG_NVRAM_FILE;
    int edgeFlag;
    key_t mqkey;
    static speed_msg msg;
#if 0
    mqkey = ftok(filenm, 0);
    if(mqkey == -1){
        perror("ftok error: ");
        return  -1;
    }
    mqid = msgget(mqkey, IPC_CREAT | IPC_EXCL | 0666);
#else 
    SlotNumber slot;

    slot.slotNumber = SLOT_NUMBER_ANY;
    
    ProfInit();
    makeFactoryProfiles();
    ProfReadFromFlash();
    funAllInit();
    mqid = UttMsgInit();
#endif
    if(mqid == -1) {
        perror("speedweb msgget error: ");
        return  -1;
    }

    signal(SIGINT, terminate_handler);
    signal(SIGTERM, terminate_handler);

    shmData = nvram_shmInit();

    openlog("SYS_CHANGE:", LOG_PID | LOG_NDELAY, LOG_LOCAL2);
    setlogmask(LOG_UPTO(LOG_INFO));
    while(1){
        msgrcv(mqid,&msg,sizeof(msg.speedPara),UTT_SPEED_WEB,0);
        if(msg.speedPara.initType == NVRAM_WRITE){
            flash_write(shmData, msg.speedPara.offset, msg.speedPara.len);
        }else{
            nvram_close(RT2860_NVRAM);
            nvram_init(RT2860_NVRAM);
            switch(msg.speedPara.initType){
                case INIT_LAN:
                    if (msg.speedPara.action == LAN_AC_DEL_LAN_RULE) {
                        _modifyLanIpRuleAndRoute(LAN_AC_DEL_LAN_RULE);
                    } else {
                        initLan();
                    }
                    syslog(LOG_INFO, "LAN initiate OK.");
                    break;
                case INIT_WAN:
                    initWan(msg.speedPara.wanIndex);
                    syslog(LOG_INFO, "WAN%d initiate OK.",msg.speedPara.wanIndex);
                    break;
                case INIT_INTERNET:
                    initInternet();
                    break;
                case INIT_DHCP:
                    init_dhcpserver();
                    syslog(LOG_INFO, "DHCP Server initiate OK.");
                    break;
                case WAN_LINK_UP:
                    doSystem("echo 1 >/var/run/Wan%dlink.status", msg.speedPara.wanIndex);
                    edgeFlag=WanLinkUp(msg.speedPara.wanIndex);
                    syslog(LOG_INFO, "WAN%d is already alive.", msg.speedPara.wanIndex);
                    if (edgeFlag == 1) {
                        syslog(LOG_INFO, "make all backup line sleep.");
                        ChangeBackLines(0);
                    }
                    break;
                case WAN_LINK_DOWN:
                    doSystem("echo 0 >/var/run/Wan%dlink.status", msg.speedPara.wanIndex);
                    edgeFlag=WanLinkDown(msg.speedPara.wanIndex);
                    syslog(LOG_INFO, "WAN%d is already down.", msg.speedPara.wanIndex);
                    if (edgeFlag == 1) {
                        ChangeBackLines(1);
                        syslog(LOG_INFO, "make all backup line alive.");
                    }
                    break;
                case DOUBLE_WAN_CONF_CHG:
                    if (msg.speedPara.action == UTT_SPEED_WEB_SEND_MSG_ACT_WGHT) {
                        ModifyDefaultRoute(msg.speedPara.wanIndex, 0);
                        syslog(LOG_INFO, "initate WAN%d link weigth OK.", msg.speedPara.wanIndex);
                    }else if(msg.speedPara.action == UTT_SPEED_WEB_SEND_MSG_ACT_CHK) {
                        LinkChkChg(msg.speedPara.wanIndex);
                        syslog(LOG_INFO, "initate WAN%d linkcheck OK.", msg.speedPara.wanIndex);
                    }else {
                        wanIpRuleFun(msg.speedPara.wanIndex, msg.speedPara.action);
                        syslog(LOG_INFO, "initate WAN%d SrcIP route policy OK.", msg.speedPara.wanIndex);
                    }
                    break;
                case ISP_PDB_CHANGE:
                    pdbISPRouteChange(msg.speedPara.data);
                    syslog(LOG_INFO, "Update ISP[%s] Policy Route OK.", msg.speedPara.data);
                    break;
                case MP_COMB_CHANGE:
                    MpCombChange((int*)msg.speedPara.data);
                    syslog(LOG_INFO, "Mutipule path line combine initiate OK.");
                    break;
                case EDIT_STATIC_ROUTE:
                    FlushUserRoute(msg.speedPara.wanIndex);
                    if ( (msg.speedPara.wanIndex==0) || _getWanLinkState(msg.speedPara.wanIndex)) {
                        /*if (msg.speedPara.action == 2) { */
                            AddUserRoute(msg.speedPara.wanIndex);
                        /*} else {
                            EditStaticRoute(msg.speedPara.data, msg.speedPara.action);
                        }*/
                    }
                    syslog(LOG_INFO, "edit static route[%s] OK.\n", msg.speedPara.data);
                    break;
                case INIT_STATIC_NAT:
                    iptablesPortForwardRun();
                    syslog(LOG_INFO, "initiate static DNAT OK.\n");
                    break;
                case EDIT_NAT_RULE:
                    //EditOneNatRule(msg.speedPara.data, 0, msg.speedPara.action);
                    init_NatRule(0, msg.speedPara.action);
                    syslog(LOG_INFO, "edit[%c] SNAT RULE[%s] OK.\n", msg.speedPara.action, msg.speedPara.data);
                    break;
                case INIT_DMZ:
                    iptablesDMZRun();
                    syslog(LOG_INFO, "initiate DMZ DNAT OK.\n");
                    break;
                case INIT_PPPOE_SERVER:
                    initPppoeServer();
                    syslog(LOG_INFO, "initiate PPPoE Server OK.\n");
                    break;
                case INIT_PPPOE_SERVER_USER_FILE:
                    creatPppoeUserFile();
                    syslog(LOG_INFO, "edit PPPoE user file OK.\n");
                    break;
                case INIT_SEC_DEFENSE:
                    iptablesSecDefenseRun();
                    syslog(LOG_INFO, "initiate security defense  OK.\n");
                    break;
                case INIT_WLAN:
                    initWlanSec();
                    syslog(LOG_INFO, "initiate WLAN OK.\n");
                    break;
                case WLAN_SEC_INIT:
                    initWlanSec();
                    break;
                case INIT_DDNS:
                    init_DDNS();
                    syslog(LOG_INFO, "initiate DDNS OK.\n");
                    break;
                case INIT_GLOBAL_BASE:
                    //init_GlobalBase();
                    syslog(LOG_INFO, "initiate Global Policy OK.\n");
                    break;
                case INIT_GROUP:
                 //   initGroup();
                    syslog(LOG_INFO, "initiate Group Configuration OK.\n");
                    break;
                default:
                    mainSec((struct st_uttMsg*)msg);
                    break;
            }
        }
    }
    closelog();
    exit(0);
}
#endif
