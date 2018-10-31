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
#ifndef CONFIG_RALINK_RT2880
#include    <linux/netfilter/nf_conntrack_common.h>
#endif
#include    <syslog.h>
#include    "utt_conf_define.h"
#include    "pppoeServer.h"
//#include    "ldra.h"
#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#if (IPV6_INTERFACE == FYES)
#include "Ipv6Internet.h"
#endif
#if(IPV6_ROUTE == FYES)
#include "Ipv6Route.h"
#endif
#if(IPV6_TUNNEL == FYES)
#include "Tunnel.h"
#endif
#if(IPV6_ISATAP == FYES)
#include "Isatap.h"
#endif
#include "spdComFun.h"
#include "comExeFun.h"
#include "mibunion.h"
#include "profacce.h"
#include "ifmisc.h"
#ifdef CONFIG_UTT_ROUTER_CHIP_FUNC
#include "uttRtChipFun.h"
#endif
#ifdef CONFIG_PPC_85xx
#include <fcntl.h>
#include <sys/stat.h>
#include "uttRtChipApi.h"
#define RTL8367M_DEVICE "/dev/utt_rtl8367m"
#endif
//#include "interface.h"
#if (STATIC_ROUTE == FYES)
#include "StaticRoute.h"
extern void ISPModifyOldTable(int wanIndex, int flag);
extern void ISPRouteOldModifyTable(InterfaceProfile *profIF,int ispNo);
#endif
#if (HOTEL_PNP == FYES)
extern int isHotelPnpEn(void);
#endif
#if (DDNS == FYES)
#include "ddns.h"
#endif
# if(UTT_DOMAINACCESS == FYES)
extern void DomainAccess(char action,char *ip_addr)
{  
    doSystem("iptables -t filter -%c FORWARD  -m uttdev --is-lan-in -m domain --name %s -j REJECT --reject-with dns-redirect:%s 1>/dev/null 2>&1",action,DOMAIN_NAME,ip_addr);
    doSystem("iptables -t filter -%c INPUT  -m uttdev --is-lan-in -m domain --name %s -j REJECT --reject-with dns-redirect:%s 1>/dev/null 2>&1",action,DOMAIN_NAME,ip_addr);
    doSystem("iptables -t filter -%c FORWARD  -m uttdev --is-lan-in -m domain --name %s -j REJECT --reject-with dns-redirect:%s 1>/dev/null 2>&1",action,DOMAIN_NAME_ADD,ip_addr);
    doSystem("iptables -t filter -%c INPUT  -m uttdev --is-lan-in -m domain --name %s -j REJECT --reject-with dns-redirect:%s 1>/dev/null 2>&1",action,DOMAIN_NAME_ADD,ip_addr);
}
#endif
#if(IP_SEC == FYES)
extern void init_IPSEC(int wanIndex);
extern void delete_ipsec_route(int wanIndex);
extern void changeAggresModetoSecLine(int wanIndex);
#endif
extern void ntp_sh(void);
extern void init_dnsmasq(int wanNo);
#if (DDNS == FYES)
extern void delete_dns_route(int wanNo);
#endif
#if(UTT_NV_1800N != FYES)
extern void wanClearSmartQosSniff(int wanIndex);
extern void webRateLimitInit(int wanIndex);
extern void wanClearAntiSniff(int wanIndex);
extern void iptablesAllNATchangeRun(int wanNo, char action);
extern void wanInitSmartQosSniff(int wanIndex);
#if (ANTI_SNIFFER == FYES)
extern void wanInitAntiSniff(int wanIndex);
extern void wanClearAntiSniff(int wanIndex);
#endif
#if (UPNP_GLOBAL_ENABLE == FYES)
extern void initUpnpServerS(void);
extern void initUpnpServerC(int wanIndex, char action);
#endif
extern void iptablesFilterInit(void);
#if (FIREWALL == FYES)
extern void fw_init(void);
#endif
#if (GROUP_BASE == FYES)
extern void groupiptablesInit(void);
#endif
extern void updataStaticNat(void);
extern int headbits(unsigned int nm);
#if (NET_SECURITY == FYES)
extern int LanArpBroadcast(void);
extern void iptablesSecDefenseRun(void);
#endif
#endif
#if((MULTI_LAN == FYES) && (WEB_BIND_ANY_SRC == FNO))
void initMultiLan(void);
#endif
#if (MULTI_LAN == FYES)
static void MultiLanArp(void);
#endif

extern int initRuleToPreDrop(char * oldip, char *newip);
#if (DHCP_SERVER == FYES)
extern void dhcpServerInit();
#endif

#if (IPV6_INTERFACE == FYES)
void initIpv6Lan(void);
#endif
#if (IPV6_RADVD == FYES)
void reStartRadvd(void);
#endif

#if(PORT_CONF_REDUCED == FYES)
static void initLanPortSpeed();
static void initWanPortSpeed(int index);
#endif
#if ((EASY_TASK == FYES) && (HOTEL_PNP == FYES))
extern void addGwForETask(void);
#endif
extern phyStaCounterSet(int wanIndex, int workState);

#if (PPPOE_ACCOUNT == FYES)
extern void poeAccExpiredInit(void);
#if (NOTICE == FYES)
extern void poeAccExpiringInit(void);
extern void poeNtCapChainAOrD(char* ipstr, char action);
extern void webauthChainAOrD(char* ipstr, char action);
extern void DnsNoticeChange(int action);
#endif
#endif
extern int isWAOrPoeNTOn(void);

#if (UTT_NV_840E == FYES) || (UTT_NV_800 == FYES) || (UTT_NV_840D == FYES) || (UTT_NV_840DV3 == FYES)||(UTT_NV_517W == FYES)||(UTT_NV_840EV3 == FYES)

#ifndef CONFIG_RTL8306E 
extern int _840E_mii_read(int phy_id, int page, int reg, int *value);
extern int _840E_mii_write(int phy_id, int page, int reg, int value);
#endif
extern int _840E_gpio_read(int phy_id, int page, int reg, int *value);
extern int _840E_gpio_write(int phy_id, int page, int reg, int value);
#endif

#if(WIFIDOG_MAKE_MONEY == FYES)
extern void makeMoneyOnLan();
#endif
#if (WIFIDOG_AUTH == FYES)
extern void wifidogExit();
extern void wifidogInit();
#endif

#if ((VLAN_CONFIG == FYES)  && (IP_GRP == FYES))
extern void add_vlan_sys_lan_group();
#endif
#if (AC_WORK_MODE == FYES)
extern void add_route_and_dns(AcWorkModeProfile *prof);
#endif

#if (DNS_AGENCY== FYES)
extern void DnsAgencyInit();
#endif
void checkLink();
#if (WLAN_WORK_TIME == FYES)
extern int wireless_switch_change(int status,WirelessProfile *prof);
#endif
#if (MULTI_LAN == FNO)
static void iptablesOnLan(IPADDR oldIp, IPADDR newIp);
/**
 * 根据lan口地址变化进行变化的iptables规则
 */
static void iptablesOnLan(IPADDR oldIp, IPADDR newIp)
{
#if (NOTICE == FYES)
    MibProfileType mibType = MIB_PROF_NOTICE;
    NoticeProfile *prof = NULL;
#endif
#if (WEB_AUTH == FYES)
    MibProfileType mibTypeWA = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profWA = NULL;
#endif

    char oldIpStr[16];
    char newIpStr[16];
    struct in_addr sAddr;

    memset(oldIpStr, 0, sizeof(oldIpStr));
    memset(newIpStr, 0, sizeof(newIpStr));

    /*转化为字串*/
    sAddr.s_addr = oldIp;
    strcpy(oldIpStr,inet_ntoa(sAddr));

    sAddr.s_addr = newIp;
    strcpy(newIpStr,inet_ntoa(sAddr));

    /*drop链初始化*/
    initRuleToPreDrop(oldIpStr, newIpStr);

#if (WEB_AUTH == FYES)
    profWA = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeWA, 0);
    /*网页认证链初始化*/
    if ((profWA) && (profWA->head.active == TRUE)) {
	webauthChainAOrD(oldIpStr, Iptables_DEL);
	webauthChainAOrD(newIpStr, Iptables_ADD);
    }
#endif
#if (NOTICE == FYES)
#if (PPPOE_ACCOUNT == FYES)
    prof = (NoticeProfile*)ProfGetInstPointByIndex(mibType, 1);
    if ((prof) && (prof->head.active == TRUE)) {

    /*将过期通告链初始化*/
	poeNtCapChainAOrD(oldIpStr, Iptables_DEL);
	poeNtCapChainAOrD(newIpStr, Iptables_ADD);

    }
#endif
    prof = (NoticeProfile*)ProfGetInstPointByIndex(mibType, 0);
    if ((prof) && (prof->head.active == TRUE)) {
	/*日常事务通告初始化*/
	doSystem("iptables -t mangle -D PREROUTING ! -d %s -j %s",oldIpStr, PRE_NOTICE);
	doSystem("iptables -t mangle -A PREROUTING ! -d %s -j %s", newIpStr, PRE_NOTICE);
    }

#endif
    return;
}
#endif

#if (HARDWARE_NAT == FYES)
//#define NAT_LOAD_BALANCE
#if defined(NAT_LOAD_BALANCE)
extern void delNlbJumpRule(char *ip);
extern void createNlbJumpRule(char *ip);
#endif
#endif

/*
 * description: setup internet according to nvram configurations
 *              (assume that nvram_init has already been called)
 *              return value: 0 = successful, -1 = failed
 */
int initInternet(void)
{
#if (MULTI_LAN == FNO)
    char if_addr[16] = {0}; 
    char *lanIfName=NULL;
#endif
    int i = 0;
    Uint32 wanCount = 0;
    InterfaceProfile *profIf = NULL;
    SystemProfile *profSys = NULL;
#if (WLAN_WORK_TIME == FYES)
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof = NULL;
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 0);
#if (WIRELESS_5G == FYES)
    WirelessProfile *prof5G = NULL;
    prof5G = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 1);
#endif
#endif
    /* nvram_name and cmdstr is just a temporary variable */

#ifdef CONFIG_PPC_85xx
    mknod(RTL8367M_DEVICE, S_IRWXU|S_IFCHR, makedev(255, 0));
#endif
#ifdef CONFIG_UTT_WANPORT_SET
    printf("%s-%d, set wanport\n", __func__, __LINE__);
    wanPortSet();
#endif

    doSystem("internet.sh");
    /* init the lan functions */
#if (TAG_VLAN == FYES)
#if (WAN_TAG_VLAN == FYES)
    extern void wanTagVlanInit(void);
    wanTagVlanInit();
#endif
#endif
#if (UTT_BRIDEG_MODE_FLAG == FYES)
    SystemProfile *profBridge = NULL;  
    int brideg_mode_flag = 0;

    profBridge = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0); 
    if(profBridge != NULL)
    { 
	brideg_mode_flag = profBridge->brideg_mode_flag;
    } 
#endif
#if(UTT_NV_1800N != FYES)
#if (UTT_BRIDEG_MODE_FLAG == FYES)
    if(brideg_mode_flag != 1)
#endif
    {
	profIf = (InterfaceProfile *)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);
	if (profIf) {
	    doSystem("lan.sh %u", profIf->ether.sta.ip);
	}
    }
#if (PORT_CONF_REDUCED == FYES)
    initLanPortSpeed();
#endif
#endif
#if (WIRELESS == FYES)
#if (WLAN_WORK_TIME == FYES)
    wireless_switch_change(1,prof);
#if (WIRELESS_5G == FYES)
    wireless_switch_change(1,prof5G);
#endif
#endif
    doSystem("wlan.sh");
#if (WIRELESS_5G == FYES)
    doSystem("wlan_5g.sh");
#endif
#endif

#if(UTT_NV_1800N != FYES)
    _modifyLanIpRuleAndRoute(LAN_AC_ADD_LAN_RULE, 0);
#endif
    /* add the rule table */
    doSystem("ip rule add from all table %d prio %d", LAN_ROUTE_TABLE_BASE, LAN_ROUTE_RULE_PRIO_BASE);
    doSystem("ip rule add iif lo table %d prio %d", SRCIP_ROUTE_TABLE_BASE, SRCIP_ROUTE_RULE_PRIO_BASE);
    doSystem("ip rule add from all table %d prio %d", USR_ROUTE_TABLE_BASE, USR_ROUTE_RULE_PRIO_BASE);
    doSystem("ip rule add from all table %d prio %d", USR_ROUTE_LAN_TABLE_BASE, USR_ROUTE_LAN_RULE_PRIO_BASE);
#if (TAG_VLAN == FYES)
    doSystem("ip rule add from all table %d prio %d", TAG_VLAN_ROUTE_TABLE, TAG_VLAN_ROUTE_RULE_PRIO);
#endif
    wanCount = getWanSlotCount();
    if(wanCount > 1)
    {
	for(i = 1; i <= ISP_DEFAULT_COUNT; i++) 
	{
	    doSystem("ip rule add from all table %d prio %d", ISP_ROUTE_TABLE_BASE + i, ISP_ROUTE_RULE_PRIO_BASE + i);
	    doSystem("ip rule add from all table %d prio %d", ISP_BACKUP_ROUTE_TABLE_BASE + i, ISP_BACKUP_ROUTE_RULE_PRIO_BASE + i);
	}
    }

    profSys = (SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    for(i = 1; i <= wanCount; i++) {

#ifndef CONFIG_IT8712_AR8316 
#if (PORT_CONF_REDUCED == FYES)
	    initWanPortSpeed(i);
#endif
#endif

#if(UTT_NV_1800N != FYES)
        doSystem("ip rule add iif lo table %d prio %d", WANCHK_ROUTE_TABLE_BASE + i, WANCHK_ROUTE_RULE_PRIO_BASE + i);
#if 0
	/* mantis 8175, 静态路由表由原来分接口的表合并成一个表 */
        doSystem("ip rule add from all table %d prio %d", USR_ROUTE_TABLE_BASE + i, USR_ROUTE_RULE_PRIO_BASE + i);
#endif
        doSystem("ip rule add fwmark 0x%d0/0x%x table %d prio %d", i, POLICYROUTE_CONNMARK_MSK, SRCIP_ROUTE_TABLE_BASE+i, FWMARK_ROUTE_RULE_PRIO_BASE);
#endif
        profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, i);
#if (STATIC_ROUTE == FYES)
        /* enable src rule table, prepared for route */
#if 0
        wanIpRuleFun(i, UTT_SPEED_WEB_SEND_MSG_ACT_ADD, ntohl(profIf->srcRouteIpS), ntohl(profIf->srcRouteIpE));
#endif
#endif

        if(profSys->lineGroupMode == GROUP_LINE_BALANCE) {
           profIf->runningLineMode = 1;
        } else {
            if(profIf->lineBalMode == IF_MIAN_LINE) {
                profIf->runningLineMode = 1;
            } else {
                profIf->runningLineMode = 0;
            }
        }
        /* init wan interface */
        if(profIf->ether.connMode != ETH_CONN_PPPOE ||
                profIf->ether.pppoe.pppConnMode != PPP_CONN_MANUAL) {
            doSystem("wan.sh %d &", i);
        }
    }

    /* do some system initiation */
#if 0
    doSystem("killall -q klogd");
#endif
    doSystem("killall -q syslogd");
    doSystem("syslogd -C8 1>/dev/null 2>&1");
#if 0
    doSystem("klogd 1>/dev/null 2>&1");
#endif

#if(UTT_NV_1800N != FYES)
    /*1800n dhcp server在 wan口状态变化时初始化*/
#if (DHCP_SERVER == FYES)
    /* enable lan functions */
    dhcpServerInit();
#endif
#if (MULTI_LAN == FYES)
    MultiLanArp();
#else
    lanIfName=getLanIfName();
    getIfIp(lanIfName,if_addr);
    doSystem("arping -q -c 50 -w 20 -I %s %s&", lanIfName, if_addr);
#endif
#endif
#if (WIRELESS == FYES)
    restart8021XDaemon();
#if (WIRELESS_5G == FYES)
#ifdef CONFIG_SECOND_IF_MT7610
    restart8021XDaemon_5g();
#endif
#endif
#endif
#if 0
    /* firewall funcions */
    firewall_init();
#endif

#if(UTT_NV_1800N != FYES)
    /*
     * 与原来firewall_init函数功能相同
     */

    iptablesFilterInit();

#if defined(NAT_LOAD_BALANCE)
    doSystem("iptables -t mangle -N nlb_chain");
#endif
#if (PPPOE_ACCOUNT == FYES)
    /*过期状态相关ipset组初始化需放在pppoeServer初始化之前
     * 防止组没初始化之前。pppoe客户端状态更新，无法更新到相应ipset组*/
#if (NOTICE == FYES)
    poeAccExpiringInit();/*将过期初始化*/
#endif
    /*过期账户规则初始化*/
    poeAccExpiredInit();/*放在pppoe初始化之前。防止本函数初始化之前， pppoe初始化之后有客户端连接到服务器*/
#endif

#if (PPPOE_SERVER == FYES)
    initPppoeServer();
#endif
    /* other local applications */
#if (UPNP_GLOBAL_ENABLE == FYES)
    initUpnpServerS();
#endif
    doSystem("pptp-init.sh");
#endif
    doSystem("uttlog 2>/dev/null &");
    return 0;
}
#if (IPV6_INTERFACE == FYES)
void initIpv6Lan(void)
{   char *port=getLanIfName();
    MibProfileType mibType = MIB_PROF_IPV6_INTERFACE;
    Ipv6InterfaceProfile *prof = NULL;
    prof = (Ipv6InterfaceProfile *)ProfGetInstPointByIndex(mibType , 0);
    if(strcmp(prof->ipv6LAN_addr,"") != 0){
	doSystem("ifconfig %s del %s/%d",port,prof->ipv6LAN_addr,prof->LANprefixLength);
	doSystem("ifconfig %s add %s/%d",port,prof->ipv6LAN_addr,prof->LANprefixLength);
	printf("ifconfig %s add %s/%d\n",port,prof->ipv6LAN_addr,prof->LANprefixLength);
    }
}
#endif
#if (IPV6_RADVD == FYES)
void reStartRadvd(void)
{
    MibProfileType mibType=MIB_PROF_RADVD;
    RadvdProfile   *prof=NULL;
    prof = (RadvdProfile *)ProfGetInstPointByIndex(mibType, 0);
    if(prof->head.active == 1){
        doSystem("killall radvd");
        doSystem("radvd -C /etc/radvd.conf -d 1&");
    }
}
#endif

#if(UTT_NV_1800N != FYES)
int initLan(void)
{
#if (MULTI_LAN == FNO)
    char *lanIfName=NULL;
    char if_addr[16] = {0};
#endif
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
#if (UTT_BRIDEG_MODE_FLAG == FYES)
    SystemProfile *profBridge = NULL;  
    int brideg_mode_flag = 0;

    profBridge = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0); 
    if(profBridge != NULL)
    { 
	brideg_mode_flag = profBridge->brideg_mode_flag;
    } 
#endif
#if (UTT_BRIDEG_MODE_FLAG == FYES)
    if(brideg_mode_flag != 1)
#endif
    {
	profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
	if (profIF) {
	    doSystem("lan.sh %u", profIF->ether.sta.ip);
	}
    }
#if (MULTI_LAN == FYES)
    MultiLanArp();
#else
    lanIfName=getLanIfName();
    getIfIp(lanIfName,if_addr);
    doSystem("arping -q -c 10 -w 5 -I %s %s&",lanIfName,if_addr);
#endif

    _modifyLanIpRuleAndRoute(LAN_AC_ADD_LAN_RULE, 0);

#if (DHCP_SERVER == FYES)
    dhcpServerInit();
#endif

#if (DHCP_POOL== FYES)
    DhcpPoolInit();
#endif

#if (FIREWALL == FYES)
    /*  刷新防火墙 */
    fw_init();
#endif

#if (GROUP_BASE == FYES)
    /*  刷新上网行为管理 */
    groupiptablesInit();
#endif

#if (UPNP_GLOBAL_ENABLE == FYES)
    initUpnpServerS();
#endif

#if (PPPOE_SERVER == FYES)
    initPppoeServer();
#endif

    /* 刷新static nat  */
    updataStaticNat();  

#if (NET_SECURITY == FYES)
    iptablesSecDefenseRun();/*攻击防御初始化*/
    LanArpBroadcast();
#endif

#if (HOTEL_PNP == FYES)
    if(isHotelPnpEn() != 0)
    {/*酒店即插即用默认路由*/
	doSystem("ip route add table %d dev %s", HOTEL_PNP_ROUTE_TABLE, getLanIfName());
    }
#endif
#if (L2TP_SERVER == FYES)
    l2tpServerInit();
#endif
    return 0;
}
#endif
/* run wan.sh */
int initWan(int wanIndex)
{
    /* just all the script */
    doSystem("wan.sh %d &", wanIndex);
    return 0;
}


/*
 * do more processes after wan port is up 
 */
void wanUpDelay(int wanNo)
{
    char *wanIfName=NULL;
    char ip[32] = {0}, limit[32] = {0};
#if (DDNS_ADVANCE == FYES)
    char interface[5];
//    char action[4];
#endif
    /* get wan interface name */
    wanIfName=getWanIfNamePPP(wanNo);
    if (getIfIp(wanIfName, ip) == 0) {
        /* construct the control message for kernel */
//        sprintf(limit, "%x%s", inet_addr(ip), wanIfName);
	 sprintf(limit, "%08x%d", inet_addr(ip), wanNo);  
  }
    sleep(1U);

#if(UTT_NV_1800N != FYES)
	    /* del nat rule */
    /* clear the nat sessions */
    doSystem("echo %s >/proc/nf_conntrack_wan ", limit);
#if (UPNP_GLOBAL_ENABLE == FYES)
    /* initiate the upnp server */
    initUpnpServerC(wanNo, 'A');
#endif
#endif
#if (DDNS == FYES)
    /* initiate the ddns server */
#if (DDNS_ADVANCE == FYES)
    sprintf(interface,"WAN%d",wanNo);
    Do_DDNS_By_Interface(interface,"add");
#else 
    init_DDNS();
#endif
#endif
    /* initiate the NTP server */
    ntp_sh();
    /* restart dns proxy */
    init_dnsmasq(wanNo);

#if(UTT_NV_1800N != FYES)
	    /* del nat rule */
    /* security defense */
    iptablesSecDefenseRun();
#endif
#if(IP_SEC == FYES)
    init_IPSEC(wanNo);
#endif
    return;
}

/* wanDownDelay do some clear such route cache */
void wanDownDelay(int wanNo)
{
    char *wanIfName;
    char ip[32] = {0}, limit[32] = {0};
#if (DDNS_ADVANCE == FYES)
    char interface[5];
//    char action[4];
#endif

#if 0
    memset(ip, 0x00, sizeof(ip));
    memset(limit, 0x00, sizeof(limit));
#endif
    wanIfName=getWanIfNamePPP(wanNo);
    /* del vpn route if any */
    doSystem("delvpnroute.sh %s  2>/dev/null", wanIfName);
    if (getIfIp(wanIfName, ip) == 0) {
//        sprintf(limit, "%x%s", ip2int(ip), wanIfName);
          sprintf(limit, "%08x%d", ip2int(ip), wanNo); 
   }
    sleep(2U);

#if(IP_SEC==FYES)
	delete_ipsec_route(wanNo);
	changeAggresModetoSecLine(wanNo);
#endif
#if(UTT_NV_1800N != FYES)
	/* del nat rule */
    /* clear nat sessions */
    doSystem("echo %s >/proc/nf_conntrack_wan", limit);
#if (UPNP_GLOBAL_ENABLE == FYES)
    /* initiate the upnp server */
    initUpnpServerC(wanNo,'D');
#endif
#endif
#if (DDNS == FYES)
    /* initiate the ddns server */
#if (DDNS_ADVANCE == FYES)
    sprintf(interface,"WAN%d",wanNo);
    Do_DDNS_By_Interface(interface,"del");
#else 

    init_DDNS();
#endif
#endif
    /* initiate the NTP server */
    ntp_sh();
    /* restart dns proxy */
    init_dnsmasq(wanNo);

#if(UTT_NV_1800N != FYES)
	/* del nat rule */
    /* security defense */
    iptablesSecDefenseRun();
#endif
} 

#if 0
/* if wanNo's link status is alive
 * On success: alive return 1
 *        sleep return 0
 * if fail return a negtive number */
int _getWanLinkState(int wanNo)
{
    FILE *fp;
    int retval=0;
    char filename[64];
    char status[2]={0,0};
    unsigned int rc=0U;

    memset(filename, 0x00, sizeof(filename));
    /* open link status file */
    sprintf(filename, "/var/run/Wan%dlink.status", wanNo);
    fp=fopen(filename, "r");
    if (fp == NULL ) {
        retval = -1;
    }else {
        /* the content of the file is a byte, which indicant 
         * the link status of the wanNo port. 
         * while '1' for up, and '0' for down */
        rc = fread((void *)status, 2U, 1U, fp);
        if (rc != 1U) {
            retval = -1;
        } else {
            retval = ((int)status[0]-0x30);
        }
        fclose(fp);
    }
    return retval;
}
#endif


/* get the number of alive-main-lines except wanNo 
 * if the status of the wanNo is main line and the
 *      link status is up, then it is a alive-main-line.
 * you can treat the balance mode as all the lines are main,
 *      none are backup in backup mode
 * if wanNo is 0, return the number of all alive main lines */
int HaveOtherMainAlive(int wanNo)
{
    int i, flag=0;
    Uint32 wanCount = 0;
    InterfaceProfile *profIf = NULL;
    wanCount = getWanSlotCount();


    for(i=1; i <= wanCount; i++) {
        if (i != wanNo) {
            profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, i);
            if (profIf->runningLineMode == 1) {
                /* it is a main line */
                if (_getWanLinkState(i) == 1) {
                    flag++;
                }
            }
        }
    }
    return flag;
}

#if (EASY_TASK == FYES)
/*
 * 功能：添加或删除DNS重定向和HTTP重定向规则
 * 创建时间：2013-6-7
 * 输入：wanNo -- WAN口序号，1对应第一个WAN口
 *	 action -- 添加或删除动作
 * 输出：0 -- 成功 1 -- 失败
 */
static int checkWanLinkStatus(unsigned int wanNo, char action)
{
    MibProfileType mibTypeEt = MIB_PROF_EASY_TASK;
    EasyTaskProfile *profEt = NULL;
    unsigned int wanLinkStatesTmp = 0U;
    char lanIp[16];
    int retVal = 0;

    profEt = (EasyTaskProfile *)ProfGetInstPointByIndex(mibTypeEt, 0);
    wanLinkStatesTmp = profEt->wanLinkStates;
    if (action == 'D')
    {
	profEt->wanLinkStates = profEt->wanLinkStates | (1U << wanNo);/*wanNo对应位置1*/
	if (wanLinkStatesTmp == 0U)/*由全部未连接到有WAN口已连接*/
	{
	    memset(lanIp, 0, sizeof(lanIp));
	    getIfIp(getLanIfName(), lanIp);/*获得LAN口IP*/
	    editLANRedirectRule(action, lanIp);/*删除LAN口重定向规则*/

	    wanConnStatsChange(CONNECTED);/*WanLinkUp表示WAN口连接*/
	}
    }
    else/*action == 'A'*/
    {
	/*wanNo对应位置置0*/
	profEt->wanLinkStates = profEt->wanLinkStates & (~(1U << wanNo));
	if ((wanLinkStatesTmp > 0U) && (profEt->wanLinkStates == 0U))/*由已连接到未连接*/
	{
	    if (profEt->easyTaskNoticeEn == FUN_ENABLE)/*easyTaskNoticeEn -- 易管理通告提示开关*/
	    {
		memset(lanIp, 0, sizeof(lanIp));
		getIfIp(getLanIfName(), lanIp);/*LAN口IP*/
		editLANRedirectRule('D', lanIp);/*防止重复添加*/
		editLANRedirectRule(action, lanIp);
	    }
	    wanConnStatsChange(CONNECT_ERROR);/*WanLinkDown连接异常*/
	}
    }/*end -- (action == 'D')*/
    return retVal;
}

/*
 * 功能：LAN口IP修改时修改重定向规则
 * 创建时间：2013-6-7
 * 输入：oldProf -- 包含原来的LAN口IP
 *	 newProf -- 包含修改后的LAN口IP
 * 输出: 0 -- 成功   1 -- 失败
 */
static int changeLANRedirectIp(const InterfaceProfile *oldProf, const InterfaceProfile *newProf)
{
    char lanip[16];
    int retVal = 0;
    struct in_addr addr;
    MibProfileType mibTypeEt = MIB_PROF_EASY_TASK;
    EasyTaskProfile *profEt = NULL;
    if(oldProf->ether.sta.ip != newProf->ether.sta.ip)/*源LANip与修改后LANip不同*/
    {
	profEt = (EasyTaskProfile *)ProfGetInstPointByIndex(mibTypeEt, 0);
	if (profEt->wanLinkStates == 0U)/*WAN口已连接则不需要规则*/
	{
	    memset(lanip, 0, sizeof(lanip));
	    addr.s_addr = oldProf->ether.sta.ip;
	    if (inet_ntoa(addr) != NULL)/*转换IP*/
	    {
		strcpy(lanip, inet_ntoa(addr));
	    }
	    editLANRedirectRule('D', lanip);/*删除以原LAN口地址建立的重定向规则*/
	    memset(lanip, 0, sizeof(lanip));
	    addr.s_addr = newProf->ether.sta.ip;
	    if (inet_ntoa(addr) != NULL)
	    {
		strcpy(lanip, inet_ntoa(addr));
	    }
	    editLANRedirectRule('A', lanip);/*添加新LAN口地址的重定向规则*/
	}
    }
    return retVal;
}
#endif

/* WanLinkUp changed the NO:$wanNo port to active mode.
 * when the linkcheck process found the line is reconnected, or
 * this line is changed to primary by user, this funcions is executed.
 * On success, it return 0; if this is the last primary line that
 *      changed up. it reutrn 1;
 * if fail, it returns a negative number.
 */
int WanLinkUp(int wanNo)
{
    char *wanIfName=NULL;
    int AliveMainLines,retval=0;
    InterfaceProfile *profIf = NULL;
#if(UTT_NV_1800N == FYES)
#if (DHCP_SERVER == FYES)
    FatFitSwitchProfile *prof1= NULL;
#endif
#endif

    /* parameter check */
    if ((wanNo < 1) || (wanNo > getWanSlotCount())){
        DBGPRINT("wanNo[%d] is illegal\n", wanNo);
        retval = -1;
    } else {
        profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, wanNo);

        DBGPRINT("WanLinkUp: Wan%d, Status[%d]\n", wanNo, profIf->runningLineMode);
        AliveMainLines = HaveOtherMainAlive(wanNo);
        {
#if 1
            wanIfName=getWanIfNamePPP(wanNo);
            doSystem("echo %d >/proc/sys/net/ipv4/conf/%s/if_index", wanNo, wanIfName);
#if(UTT_NV_1800N != FYES)
	    /* del nat rule */
            /* reform NAT */
            iptablesAllNATchangeRun(wanNo, 'A');
#else
#if (DHCP_SERVER == FYES)
	    /* enable lan functions */
    prof1 = (FatFitSwitchProfile*)ProfGetInstPointByIndex(MIB_PROF_FAT_FIT_SWITCH,0);
    if((prof1 != NULL) && (prof1->fatEnable==1))
    {
	    dhcpServerInit();
    }
#endif
#endif
            /* edit the route that directly linked with metric value */
            doSystem("edit_scope_route.sh  %d 1 1", wanNo);
            /* add srcip route */
            addOneWanDefaultRoute(wanNo);
            /* add ISP dest-ip route */
//    printf("*********\n************\n%s----%d\n",__func__,__LINE__);
            ISPRouteTableAdd(wanNo);
#if 0
            /* add user defined route */
            AddUserRoute(wanNo);
#else
	    /* for mantis 8175 */
	    TIRouteChg();
#endif
            /* add Dns */
            add_dns_route(wanNo);

            /* reform the default route */
            ModifyDefaultRoute(wanNo, 1);
#if(UTT_NV_1800N != FYES)
	    /* del nat rule */
            wanInitSmartQosSniff(wanNo);
		    webRateLimitInit(wanNo);
#if (ANTI_SNIFFER == FYES)
			wanClearAntiSniff(wanNo);
            wanInitAntiSniff(wanNo);
#endif
#endif
            /* other process */
            wanUpDelay(wanNo);
#endif
       if (profIf->runningLineMode == 1 && (AliveMainLines == 0) ) { 
                /* shutdown all the backup lines */
                retval = 1;
            }
#if (EASY_TASK == FYES)
	    checkWanLinkStatus(wanNo, 'D');
#endif

        }
    }
    return retval;
}

/* WanLinkDown changed the NO:$wanNo port to sleep mode.
 * when the linkcheck process found the line is disconnected, or
 * this line is changed to backup by user, this funcions is executed.
 * On success, it return 0; if this is the last primary line that
 *      changed down. it reutrn 1;
 * if fail, it returns a negative number.
 */
int WanLinkDown(int wanNo)
{
#if 0
    char *wanIfName=NULL;
#endif
    int AliveMainLines, retval=0;
    InterfaceProfile *profIf = NULL;

    /* parameter checks */
    if ((wanNo < 1) || (wanNo > getWanSlotCount())) {
        DBGPRINT("wanNo[%d] is illegal\n", wanNo);
        retval = -1;
    } else {
        profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, wanNo);
        /* wanStatus identify this is a primary line or backup line */
        AliveMainLines = HaveOtherMainAlive(wanNo);
    
        /* delete srcip default route */
        delOneWanDefaultRoute(wanNo);
        /* flush the ISP destip Route */
//    printf("%s----%d\n",__func__,__LINE__);
        ISPRouteTableFlush(wanNo);
#if 0
        wanIfName=getWanIfNamePPP(wanNo);
        doSystem("echo 0 >/proc/sys/net/ipv4/conf/%s/if_index", wanIfName);
        /* flush the route that user config */
        FlushUserRoute(wanNo);
#else
	/* for mantis 8175 */
	TIRouteChg();
#endif
#if (DDNS == FYES)
        /* delete dns route */
        delete_dns_route(wanNo);
#endif
        /* edit the route that directly linked with metric value */
        doSystem("edit_scope_route.sh  %d 2 %d", wanNo, get_phylink_status(wanNo));
        
#if(UTT_NV_1800N != FYES)
	/* del nat rule */
        iptablesAllNATchangeRun(wanNo, 'D');
#endif
#if (EASY_TASK == FYES)
	checkWanLinkStatus(wanNo, 'A');
#endif
        if ( profIf->runningLineMode == 1 && (AliveMainLines == 0) ) {
            /* link up all the bakcup lines */
            doSystem("ip route del default");
#if ((EASY_TASK == FYES) && (HOTEL_PNP == FYES))
	    addGwForETask();
#endif
           retval = 1;
 #if(UTT_DOMAINACCESS ==FYES)
	   isWanLinkStatus=0;
#endif
#if (HOTEL_PNP ==FYES || EASY_TASK == FYES || UTT_DOMAINACCESS ==FYES)
	   editDNSRedirectRule();
#endif
        } else {
            /* reform the default route */
            ModifyDefaultRoute(wanNo, 0);
        }

#if(UTT_NV_1800N != FYES)
	/* del nat rule */
        wanClearSmartQosSniff(wanNo);
#if (ANTI_SNIFFER == FYES)
        wanClearAntiSniff(wanNo);
#endif
#endif
        /* delete Nat,dns,and dns-appointed route */
        wanDownDelay(wanNo);
    }
    return retval;
}

/* ChangeBackLines change all the backup line to the status Up or Down
 * by calling the WanLinkUp or WanLinkDown depanding the value of $action.
 * while 1 means up, 0 means down.
 * in our design idea, there is no distinct between backup mode and balance mode.
 * balance mode is the particular mode of backup when all lines are primary.
 * we make sure it by using the wanxStatus name distinct from the wanxStatusW
 *      that user config.
 */
void ChangeBackLines(int action)
{
    int i;
#if 0
    char buf[16];
#endif
    Uint32 wanCount = 0;
    InterfaceProfile *profIf = NULL;

    wanCount = getWanSlotCount();

    for(i = 1; i <= wanCount; i++) {
        profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, i);
        /* is a backup line */
        if (profIf->runningLineMode == 0 ) {
            /* no matter what is the link status, delete default route */
            ModifyDefaultRoute(i, action);
            /* change to alive */
            if (action == 1) {
                wanUpDelay(i);
            /* change to sleep */
            }else if(action == 0) {
                wanDownDelay(i);
            }else {
                DBGPRINT("action[%d] is invalid value\n", action);
            }
        }
    }
}

#if(UTT_NV_1800N != FYES)
/* MpCombChange process the change of the status of wan.
 * for example, user change the wan1 from primary line to backup line.
 * $wanStatuses is the change flags from wan1 to wan8(only use first 2 elements for
 *      wan1 and wan2 now).
 * if the flag is minus, it means the wan is changed from primary to backup; if 
 *      the flag is zero, it means no change happend; if flag is postive number, it
 *      means the wan is changed from backup to primary.
 */
void MpCombChange(int wanStatuses[MAX_WAN_IF_COUNT])
{
    unsigned int i;
    int chgflag, linkStatus, AliveMainLines;
    Uint32 wanCount = 0;

    wanCount = getWanSlotCount();

    DBGPRINT("MpCombChange:begin\n");
    for (i=1U; i <= wanCount; i++) {
        /* get current line link status */
        linkStatus =_getWanLinkState((int)i);
        AliveMainLines=HaveOtherMainAlive((int)i);
        chgflag = wanStatuses[i-1U];
        DBGPRINT("MpCombChange:wan%d,change flag[%d]\n", i, chgflag);
        DBGPRINT("MpCombChange:wan%d,linkStatus[%d],AliveMainLines[%d]\n", i, linkStatus, AliveMainLines);
        /* only the alive lines should be processed */
        if (linkStatus == 1) {
            if (chgflag > 0 ) {
                /* backup to primary change */
                DBGPRINT("MpCombChange:backup to primary change\n");
                syslog(LOG_INFO, "WAN%d: backup to primary change.", i);
		ISPModifyOldTable(i, 1);
                if (AliveMainLines >= 1) {
                    /* there are alive main lines, just wake up this wan port */
//	    printf("%s--%d \n",__func__,__LINE__);
                    WanLinkUp((int)i);
                }else{
                    /* there is not any alive-main-lines, this is the first main lines.
                     * so, other backup lines should be sleeped 
                     */
//    printf("%s----%d\n",__func__,__LINE__);
		    ISPRouteTableFlush(i);
                    ChangeBackLines(0);
                }
            }else if (chgflag < 0 ) {
                /* primary to backup change */
                DBGPRINT("MpCombChange:primary to backup change\n");
                syslog(LOG_INFO, "WAN%d: primary to backup change.", i);
                /* anyway, this line should be sleeped first */
		ISPModifyOldTable(i, -1);
//    printf("%s----%d\n",__func__,__LINE__);
		ISPRouteTableFlush(i);
                ModifyDefaultRoute((int)i, 0);
                if (AliveMainLines == 0) {
                    /* this is the last alive-main-line that changed,
                     * so wake up all the other backup lines
                     */
                    ChangeBackLines(1);
                }
            }else{
                /* no change, do nothing */
            }
        }
    }
}
#endif

void LinkChkChg(int wanIndex, const InterfaceProfile *prof)
{
    char wanDhcpGw[32], *wanIfName;
    IPADDR detectIp = 0u;
    struct in_addr addr;
    
    wanIfName = getWanIfNamePPP(wanIndex);

    doSystem("ip route flush table  %d", WANCHK_ROUTE_TABLE_BASE+wanIndex);

    if(prof->detectCount != 0){
	if (prof->ether.connMode == ETH_CONN_DHCP) {
	    getWanDhcpDefaultRoute(wanIndex, wanDhcpGw);
	}
	    if (prof->detectIp == 0u) {
		    if (prof->ether.connMode == ETH_CONN_DHCP) {
			    memset(&addr, 0, sizeof(addr));
			    inet_aton(wanDhcpGw, &addr);
			    detectIp = addr.s_addr;
		    } else {
			    detectIp = prof->ether.sta.gw;
		    }
	    } else {
	    detectIp = prof->detectIp;
	}
        switch(prof->ether.connMode) {
            case ETH_CONN_PPPOE:
                doSystem("ip route replace table %d %N/32 dev %s",
                        WANCHK_ROUTE_TABLE_BASE + wanIndex, detectIp, wanIfName);
                break;
            case ETH_CONN_DHCP:
                doSystem("ip route replace table %d %N/32 via %s dev %s",
                        WANCHK_ROUTE_TABLE_BASE + wanIndex, detectIp, wanDhcpGw, wanIfName);
                break;
            case ETH_CONN_STATIC:
                doSystem("ip route replace table %d %N/32 via %N dev %s",
                        WANCHK_ROUTE_TABLE_BASE + wanIndex, detectIp, prof->ether.sta.gw, wanIfName);
                break;
            default:
                break;
        }
    }
    //doSystem("kill -9 `cat /var/run/linkcheck%d.pid`", wanIndex);
    //doSystem("linkcheck.sh %d >/dev/null 2>&1 &", wanIndex);
    checkLink();    
}

void checkLink()
{
    doSystem("killall  uttLineDetec");                               
    InterfaceProfile *profIf = NULL;
    MibProfileType mibType = MIB_PROF_INTERFACE;
    int j=0,count=getWanSlotCount(),i=0;
    char tmp1[10]="",tmp2[100]="",tmp[50]="";
    IPADDR detectIp = 0u;
    char wanDhcpGw[RT_IP_LEN];
    struct in_addr addr;
    
    for(j=1;j<=count;j++)
    {
        if(!strlen(getWanIfNamePPP(j)))
            continue;
        profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, j);
        if((ProfInstIsFree(profIf)==0) && (profIf->head.active == TRUE ) &&(profIf->detectInterval!=0))
        {
            i++;
            if (profIf->detectIp == 0u) {
		    if (profIf->ether.connMode == ETH_CONN_DHCP) {
			    getWanDhcpDefaultRoute(j, wanDhcpGw);
			    memset(&addr, 0, sizeof(addr));
			    inet_aton(wanDhcpGw, &addr);
			    detectIp = addr.s_addr;
		    } else {
			    detectIp = profIf->ether.sta.gw;
		    }
            } else {
                detectIp = profIf->detectIp;
            }
            sprintf(tmp," %d ",j);
            strcat(tmp1,tmp);
            sprintf(tmp," %s %d %d %u ",getWanIfNamePPP(j),profIf->detectInterval,profIf->detectCount,detectIp);
            strcat(tmp2,tmp);
        }
    }     
    if (i > 0)
	    doSystem("uttLineDetec %d %s %s &",i,tmp1,tmp2);
}

#if 1
#if (PORT_CONF_REDUCED == FYES)
static void  converModeTospdSetMode(SwPortConfMode speedMode, PortSpdSetSt *spdSetMode)
{
    if(speedMode == PORT_MODE_AUTO)
    {
	spdSetMode->autoneg = 1;
	spdSetMode->speed = 0;
	spdSetMode->fdx = 0;
    }else if(speedMode == PORT_MODE_10FDX){
	spdSetMode->autoneg = 0;
	spdSetMode->speed = 10;
	spdSetMode->fdx = 1;
    } else if(speedMode == PORT_MODE_100FDX) {
	spdSetMode->autoneg = 0;
	spdSetMode->speed = 100;
	spdSetMode->fdx = 1;
    
    } else if(speedMode == PORT_MODE_1GFDX) {
	spdSetMode->autoneg = 0;
	spdSetMode->speed = 1000;
	spdSetMode->fdx = 1;
    
    } else if(speedMode == PORT_MODE_10HDX) {
	spdSetMode->autoneg = 0;
	spdSetMode->speed = 10;
	spdSetMode->fdx = 0;
    
    } else if(speedMode == PORT_MODE_100HDX) {
	spdSetMode->autoneg = 0;
	spdSetMode->speed = 100;
	spdSetMode->fdx = 0;
    
    } else {
	spdSetMode->autoneg = 0;
	spdSetMode->speed = 0;
	spdSetMode->fdx = 0;
    }
    DBGPRINT("%s-%d: converMode autoneg=%d, speed=%d, fdx=%d\n",__func__,__LINE__,spdSetMode->autoneg,spdSetMode->speed,spdSetMode->fdx);
    return;
}

/*ethtool 设置*/
void ethtoolSet(char* ifName, PortSpdSetSt mode)
{
    if(mode.autoneg == 1)
    {
	/*自动协商*/
#if defined CONFIG_IT8712_AR8316
	doSystem("ethtool -s %s autoneg on speed 1000 ", ifName);
#else
	doSystem("ethtool -s %s autoneg on", ifName);
#endif
    }
    else
    {	/*强制设置*/
	doSystem("ethtool -s %s autoneg off speed %d  duplex %s", ifName, mode.speed,
	    (mode. fdx == 1?"full":"half"));
    }
}
#if 1
#ifdef CONFIG_RALINK_RT3052

/*
 * chipPort: >=0
 * */
void SpdModeSet3052(Uint32 chiPort, PortSpdSetSt mode)
{  
    /* 可参考goahead/src/utils.c写文件
     * 这里也可以echo chiPort >  /proc/rt2880/gmac portnum
     * 但是鉴于少用system函数的原因。我们不用它 
     *
     * 目前对于510v2，510w，520v2型号，设置有所不同
     * "/proc/rt3052/gmac" 三种型号都应为rt3052，但是目前只有510v2是rt3052
     * 而510W，520v2 还是原来rt2880，这里同一修改为rt3052*/
        FILE *proc_file = fopen("/proc/rt3052/gmac", "w");
	if(!proc_file){
	return; 
	}  
	fprintf(proc_file, "%u", chiPort);
	fclose(proc_file);

	ethtoolSet("eth2", mode);
	DBGPRINT("%s-%d: chiPort=%u\\n",__func__,__LINE__,chiPort);

	      
}
#elif  defined(CONFIG_RTL8306_PHY)

#if (UTT_NV_840E == FYES) || (UTT_NV_800 == FYES) || (UTT_NV_840D == FYES)
/*
 * lanPort: 1 - 设置lan口speed， 0 - 设置wan口speed
 */
static void  SpdModeSet8306(Uint32 phyId, PortSpdSetSt mode, int lanPort) {
    Uint32 regVal;

    /* 读值 */
    if (lanPort) {
	_840E_gpio_read(phyId, 0, 0, &regVal);
    } else {
	_840E_mii_read(phyId, 0, 0, &regVal);
    }

    /* 设置值 */
    if(mode.autoneg == 1)
    {
	regVal = BIT_SET(regVal, 12, 1);/*自动协商*/
    }
    else
    {
        regVal = BIT_SET(regVal, 12, 0);/*关闭自动协商*/
        regVal = BIT_SET(regVal, 8, mode.fdx);/*duplex */
        if(mode.speed == 100)
        {
	    regVal = BIT_SET(regVal, 13, 1);/*100m */
	}  
	else
	{ 
	    regVal = BIT_SET(regVal, 13,0);/*10m*/
	}
    }


#if 0
    /*由于该芯片端口模式设置后。某些模式 不会自动重启。故要手动重启下*/
    regVal = BIT_SET(regVal, 11, 1);/*power down */
    printf("%s-%d: regVal=0x%x\n",__func__,__LINE__,regVal);
    /* 写值 */
    if (lanPort) {
	_840E_gpio_write(phyId, 0, 0, regVal);
    } else {
	_840E_mii_write(phyId, 0, 0, regVal);
    }
    regVal = BIT_SET(regVal, 11, 0);/*power up */
    printf("%s-%d: regVal=0x%x\n",__func__,__LINE__,regVal);
#endif
    if (lanPort) {
	_840E_gpio_write(phyId, 0, 0, regVal);
    } else {
	_840E_mii_write(phyId, 0, 0, regVal);
    }

    return;
}
#else
/*
 * phyId: >=0
 */
void SpdModeSet8306(Uint32 phyId, PortSpdSetSt mode)
{  
    FILE *fp;
    Uint32 regVal=0U;
    char pStr[30];
    char buff[256];

    memset(pStr, 0, sizeof(pStr));
    memset(buff,0,sizeof(buff));
    snprintf(pStr, 30, "arswitch reg r %x 0 0",phyId);
    DBGPRINT("%s-%d: pStr=%s\n",__func__,__LINE__,pStr);
    fp = popen(pStr, "r");/*读取当前phy寄存器值*/
    if(fp!=NULL)
    {
	fscanf(fp, "%*s%*s%*s%*s%*s%*s%s",buff);
    
	DBGPRINT("%s-%d: buff=%s,buff+6=%s\n",__func__,__LINE__,buff,buff+strlen("value="));
	regVal = strtoul(buff+strlen("value="), 0, 16);
	DBGPRINT("%s-%d: regVal=0x%x\n",__func__,__LINE__,regVal);
	if(mode.autoneg == 1)
	{
	    regVal = BIT_SET(regVal, 12, 1);/*自动协商*/
	}
	else
	{
	    regVal = BIT_SET(regVal, 12, 0);/*关闭自动协商*/
	    regVal = BIT_SET(regVal, 8, mode.fdx);/*duplex */
	    if(mode.speed == 100)
	    {
		regVal = BIT_SET(regVal, 13, 1);/*100m */
	    }  
	    else
	    { 
		regVal = BIT_SET(regVal, 13,0);/*10m*/
	    }
	}
	DBGPRINT("%s-%d: regVal=%u,regVal=0x%x\n",__func__,__LINE__,regVal,regVal);
	/*由于该芯片端口模式设置后。某些模式 不会自动重启。故要手动重启下*/
	regVal = BIT_SET(regVal, 11, 1);/*power down */
	DBGPRINT("%s-%d: regVal=%u,regVal=0x%x\n",__func__,__LINE__,regVal,regVal);
	doSystem("arswitch reg w %x 0 0 %x",phyId,regVal);
	regVal = BIT_SET(regVal, 11, 0);/*power up*/
	DBGPRINT("%s-%d: regVal=%u,regVal=0x%x\n",__func__,__LINE__,regVal,regVal);
	doSystem("arswitch reg w %x 0 0 %x",phyId,regVal);
	pclose(fp);
	return;
    }
}
#endif

#elif defined(CONFIG_IT8712_AR8316) 

/*
 * phyId: >=0
 */
void SpdModeSet8316(Uint32 phyId, PortSpdSetSt mode)
{  
    FILE *fp;
    Uint32 regVal=0U;
    char pStr[30];
    char buff[256];

    memset(pStr, 0, sizeof(pStr));
    memset(buff,0,sizeof(buff));
    snprintf(pStr, 30, "mii_6530G phy r %x 0",phyId);
    DBGPRINT("%s-%d: pStr=%s\n",__func__,__LINE__,pStr);
    fp = popen(pStr, "r");/*读取当前phy寄存器值*/
    
    if(fp!=NULL)
    {
	fscanf(fp, "%*s%*s%*s%*s%s",buff);
    
	DBGPRINT("%s-%d: buff=%s,buff+10=%s\n",__func__,__LINE__,buff,buff+strlen("data.data="));
	regVal = strtoul(buff+strlen("data.data="), 0, 16);
	DBGPRINT("%s-%d: regVal=0x%x\n",__func__,__LINE__,regVal);

	if((mode.autoneg == 1) || (mode.speed == 1000))
	{  
	    /*1g手动模式，芯片有问题。用自动协商暂时代替*/
	    regVal = BIT_SET(regVal, 12, 1);/*自动协商*/
	}
	else
	{
	    regVal = BIT_SET(regVal, 12, 0);/*关闭自动协商*/
	    regVal = BIT_SET(regVal, 8, mode.fdx);/*duplex */
	    if(mode.speed == 100)	/*01 100m*/
	    { 
		regVal = BIT_SET(regVal, 6,0);
		regVal = BIT_SET(regVal, 13,1);
	    }
	    else{			/*00 10m*/
		regVal = BIT_SET(regVal, 6,0);
		regVal = BIT_SET(regVal, 13,0);
	    }
	}
	DBGPRINT("%s-%d:regVal=%u, regVal=0x%x\n",__func__,__LINE__,regVal,regVal);

	doSystem("mii_6530G phy w %x 0 %x",phyId,regVal);
	pclose(fp);
    }

}
#elif defined(CONFIG_IT82583)
void SpdModeSet82583(Uint32 chiPort, PortSpdSetSt mode){
	ethtoolSet("eth0",mode);
}
#elif defined(CONFIG_RALINK_RT3883)
/*
 * chipPort: >=0
 * */
void SpdModeSet3883(Uint32 chiPort, PortSpdSetSt mode)
{  
    /* 可参考goahead/src/utils.c写文件
     * 这里也可以echo chiPort >  /proc/rt3883/gmac portnum
     * 但是鉴于少用system函数的原因。我们不用它 */
        FILE *proc_file = fopen("/proc/rt3883/gmac", "w");
	if(!proc_file){
	return; 
	}  
	DBGPRINT("%s-%d: \n",__func__,__LINE__);
	fprintf(proc_file, "%u", chiPort);
	fclose(proc_file);
	if(mode.speed == 1000)
	{
	DBGPRINT("%s-%d: \n",__func__,__LINE__);
	    mode.autoneg = 1;
	    mode.speed = 0;
	    mode.fdx = 0;
	}
	ethtoolSet("eth2", mode);
	DBGPRINT("%s-%d: chiPort=%u\\n",__func__,__LINE__,chiPort);

}
#elif defined(CONFIG_RALINK_MT7620) || defined(CONFIG_RALINK_MT7621)|| defined(CONFIG_RALINK_MT7628)
#ifdef CONFIG_RTL8306E 
/*
 * 设置lan口speed,840Dv3 lan口通过8306控制，4个wan口通过7620控制
 */
static void  SpdModeSet8306(Uint32 phyId, PortSpdSetSt mode) {
    Uint32 regVal;

    /* 读值 */
    _840E_gpio_read(phyId, 0, 0, &regVal);

    /* 设置值 */
    if(mode.autoneg == 1)
    {
	regVal = BIT_SET(regVal, 12, 1);/*自动协商*/
    }
    else
    {
        regVal = BIT_SET(regVal, 12, 0);/*关闭自动协商*/
        regVal = BIT_SET(regVal, 8, mode.fdx);/*duplex */
        if(mode.speed == 100)
        {
	    regVal = BIT_SET(regVal, 13, 1);/*100m */
	}  
	else
	{ 
	    regVal = BIT_SET(regVal, 13,0);/*10m*/
	}
    }

    _840E_gpio_write(phyId, 0, 0, regVal);

    return;
}
#endif

/*
 * chipPort: >=0
 * */
void SpdModeSet7620(Uint32 chiPort, PortSpdSetSt mode)
{
	Uint32 regVal = 0U;

#ifdef CONFIG_RTL8367RB
	if (mode.autoneg == 1 || mode.speed == 1000) {
		regVal = 0x1200;
	} else {
		if (mode.speed == 1000){
			regVal |= 1u<<6u;
		} else if (mode.speed == 100){
			regVal |= 1u<<13u;
		} else {
			regVal &= ~((1u<<6u) | (1u<<13u));
		}

		if(mode.fdx == 1) {
			regVal |= 1u<<8u;
		}
	}
#else
	if (mode.autoneg == 1) {
		regVal = 0x3300;
	} else {
		if(mode.fdx == 1) {
			if (mode.speed == 10) {
				regVal = 0x100;
			} else if (mode.speed == 100){
				regVal = 0x2100;
			} else {
				regVal = 0x3300;
			}
		} else {
			if (mode.speed == 10) {
				regVal = 0x0;
			} else if (mode.speed == 100) {
				regVal = 0x2000;
			} else {
				regVal = 0x3300;
			}
		}
	}
#endif
#if(UTT_NV_1220V2 == FYES)
	doSystem("mii_mgr -s -p %d -r 9 -v 0", chiPort);
#endif
	doSystem("mii_mgr -s -p %d -r 0 -v %x", chiPort, regVal);
}

#elif defined(CONFIG_PPC_85xx)
/*
 *	SpdModeSetP1010		-	设置端口的速率模式
 * 参数：
 *	chiPort			-	端口号
 *	mode			-	端口模式
 */
void SpdModeSetP1010(Uint32 chiPort, PortSpdSetSt mode)
{
	rtl8367m_speed_data data;
	int fp;

	memset(&data, 0, sizeof(data));
	data.port = chiPort;
	if (mode.autoneg == 1) {
		data.speed.AutoNegotiation = 1;
		data.speed.Half_10 = 1;
		data.speed.Full_10 = 1;
		data.speed.Half_100 = 1;
		data.speed.Full_100 = 1;
		data.speed.Full_1000 = 1;
	} else {
		if(mode.fdx == 1) {
			if (mode.speed == 10) {
				data.speed.Full_10 = 1;
			} else if (mode.speed == 100){
				data.speed.Full_100 = 1;
			} else {
				data.speed.Full_1000 = 1;
			}
		} else {
			if (mode.speed == 10) {
				data.speed.Half_10 = 1;
			} else if (mode.speed == 100) {
				data.speed.Half_100 = 1;
			} else {
				printf("speed is error");
			}
		}
	}
	/*打开字符设备*/
	fp = open(RTL8367M_DEVICE, O_RDWR, S_IRUSR|S_IWUSR);
	if (fp < 0) {
		perror("Open");
		return;
	}
	if(ioctl(fp, RTL8367M_MII_SPEED_WRITE, &data) < 0) {
		perror("ioctl");
		close(fp);
		return;
	}
	close(fp);
	return;
}
#ifdef CONFIG_BOARD_P1010_C
void SpdModeSetP1010_C_WAN(Uint32 chiPort, PortSpdSetSt mode)
{
#if defined(CONFIG_AR8337N)
	rtl8367m_ioctl_data data;
	int fp;
	Uint32 regVal = 0U;

	if (mode.autoneg == 1) {
		regVal = 0x3300;
	} else {
		if(mode.fdx == 1) {
			if (mode.speed == 10) {
				regVal = 0x100;
			} else if (mode.speed == 100){
				regVal = 0x2100;
			} else {
				regVal = 0x3300;
			}
		} else {
			if (mode.speed == 10) {
				regVal = 0x0;
			} else if (mode.speed == 100) {
				regVal = 0x2000;
			} else {
				regVal = 0x3300;
			}
		}
	}

    memset(&data,0,sizeof(data));
    data.phy_id = chiPort;
    data.reg_num = 0;
    data.val_in = regVal;

	/*打开字符设备*/
	fp = open(RTL8367M_DEVICE, O_RDWR, S_IRUSR|S_IWUSR);
	if (fp < 0) {
		perror("Open");
		return;
	}
	if(ioctl(fp, AR8337N_MII_WRITE, &data) < 0) {
		perror("ioctl");
		close(fp);
		return;
	}
	close(fp);
#endif
	return;
}
#endif
#else
#error please add port speed mode set function for this chip
#endif
#endif
static void rddPortSpeedModeSet(int index, SwPortConfMode speedMode)
{
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    PortSpdSetSt spdSetMode;
    int i=0;
#if (WIRELESS == FYES)
    if((index == get3GSlotIndex()) || (index == getAPCliSlotIndex()))
    {
	DBGPRINT("%s-%d: \n",__func__,__LINE__);
	return;
    }
#endif
    converModeTospdSetMode(speedMode, &spdSetMode);
    DBGPRINT("%s-%d: converMode autoneg=%d, speed=%d, fdx=%d\n",__func__,__LINE__,spdSetMode.autoneg,spdSetMode.speed,spdSetMode.fdx);
#ifndef CONFIG_SWITCH_EXT
#if(UTT_NV_1800N != FYES)
    /*lan 口*/
    if(0==index)
    {
	profSys = (SystemProfile*)ProfGetInstPointByIndex(mibTypeSys, 0);
	DBGPRINT("%s-%d: lanPortCount=%d\n",__func__,__LINE__,profSys->lanPortCount);
	/*需支持动态wan口下lan口减少的情况*/
	for(i=0;i<profSys->lanPortCount;i++)
	{
#ifdef CONFIG_RALINK_RT3052
	    /*这里还要根据软件型号控制
	     * 如下提供的为从右开始为lan port1的型号
	     */
#if((UTT_NV_510V2 == FYES) || (UTT_NV_510V4 == FYES) || (UTT_NV_510W == FYES) || (UTT_NV_540 == FYES) || (UTT_NV_588 == FYES)  || (UTT_NV_512W == FYES)  || (UTT_NV_518W == FYES) || (UTT_NV_518WLITE == FYES)|| (UTT_NV_518WVPN == FYES)|| (UTT_NV_518WPLUS == FYES)|| (UTT_NV_518 == FYES))
	    SpdModeSet3052(4-i,spdSetMode);/*设置所有lan口模式。3052 lan从芯片4口开始*/
#else
	    SpdModeSet3052(i,spdSetMode);/*设置所有lan口模式。3052 lan从芯片1口开始*/
#endif
#elif defined(CONFIG_RTL8306_PHY)
#if (UTT_NV_840E == FYES) || (UTT_NV_800 == FYES) || (UTT_NV_840D == FYES)
	    SpdModeSet8306(i,spdSetMode, 1);
#else
	    SpdModeSet8306(i,spdSetMode);
#endif
#elif defined(CONFIG_IT8712_AR8316)
	    SpdModeSet8316(i,spdSetMode);
#elif defined(CONFIG_IT82583)
		SpdModeSet82583(i,spdSetMode);
#elif defined(CONFIG_RALINK_RT3883)
	    SpdModeSet3883(i,spdSetMode);/*设置所有lan口模式。3883 lan从芯片1口开始*/
#elif defined(CONFIG_RALINK_MT7620) || defined(CONFIG_RALINK_MT7621)  || defined(CONFIG_RALINK_MT7628)
#if ((UTT_NV_750W == FYES) || (UTT_NV_842WV3 == FYES) || (UTT_NV_810V4 == FYES)|| (UTT_NV_830V3 == FYES) || (UTT_NV_811V4 == FYES) || (UTT_NV_821V2 == FYES) || (UTT_NV_841V4 == FYES) || (UTT_NV_840V4 == FYES) || (UTT_NV_520WV3 == FYES) || (UTT_NV_D910 == FYES) || (UTT_NV_WX20S == FYES) || (UTT_U_1100 == FYES) || (UTT_U_1900 == FYES))
		/* 750W 硬件上接口顺序是错的*/
		if (i == 0) {
			SpdModeSet7620(4,spdSetMode);/*设置所有lan口模式。3883 lan从芯片1口开始*/
		} else if (i == 1) {
			SpdModeSet7620(0,spdSetMode);/*设置所有lan口模式。3883 lan从芯片1口开始*/
		} else if (i == 2) {
			SpdModeSet7620(1,spdSetMode);/*设置所有lan口模式。3883 lan从芯片1口开始*/
		} else if (i == 3) {
			SpdModeSet7620(2,spdSetMode);/*设置所有lan口模式。3883 lan从芯片1口开始*/
		} else if (i == 4) {
			SpdModeSet7620(3,spdSetMode);/*设置所有lan口模式。3883 lan从芯片1口开始*/
		} else {
		}
#elif (UTT_NV_840DV3 == FYES)||(UTT_NV_517W == FYES)||(UTT_NV_840EV3 == FYES)
		if (i < 4) {
			SpdModeSet8306(i,spdSetMode);/*设置所有lan口模式*/
		} else {
			SpdModeSet7620(i -3 ,spdSetMode);/*设置所有wan口模式,chipPort顺序与wan口顺序相反*/
		}
#elif ((UTT_NV_512WV3 == FYES) || (UTT_NV_518WV3 == FYES) || (UTT_NV_515W == FYES) || (UTT_NV_D908W == FYES))
		if (i == 0) {
			SpdModeSet7620(3,spdSetMode);
		} else if (i == 1) {
			SpdModeSet7620(2,spdSetMode);
		} else if (i == 2) {
			SpdModeSet7620(1,spdSetMode);
		} else if (i == 3) {
			SpdModeSet7620(0,spdSetMode);
		} else {
		}
#elif ((UTT_NV_810V3 == FYES) || (UTT_NV_840V3 == FYES) || (UTT_NV_520V3 == FYES) || (UTT_NV_1220GW == FYES) ||(UTT_NV_1220V2 == FYES))
	    SpdModeSet7620(i,spdSetMode);/*设置所有lan口模式。3883 lan从芯片1口开始*/
#elif defined(UTT_NV_521GW)
	    SpdModeSet7620(i, spdSetMode);/*设置所有lan口模式。3883 lan从芯片1口开始*/
#elif ((UTT_NV_750GW == FYES) || (UTT_NV_521GV2 == FYES) || (UTT_NV_840GV2 == FYES) )
	    SpdModeSet7620(i, spdSetMode);/*设置所有lan口模式*/
#else
	    SpdModeSet7620(4 - i,spdSetMode);/*设置所有lan口模式。3883 lan从芯片1口开始*/
#endif
#elif defined(CONFIG_PPC_85xx)
#if defined(CONFIG_BOARD_P1010_B) || defined(CONFIG_BOARD_P1010_C)
	    SpdModeSetP1010(i,spdSetMode);/*设置所有lan口模式。8367m lan从芯片0口开始*/ 
#endif
#else
#error please add port speed mode set function for this chip
#endif

	}
    }
#endif
#endif

    if(index > 0)
    {
	/*wan口配置*/
#ifdef CONFIG_RALINK_RT3052
	/* 这里还要根据软件型号控制
	 * 如下提供的为从右开始为lan port1的型号
	 */
#if((UTT_NV_510V2 == FYES) || (UTT_NV_510V4 == FYES) || (UTT_NV_510W == FYES) || (UTT_NV_540 == FYES) || (UTT_NV_588 == FYES)  || (UTT_NV_512W == FYES)  || (UTT_NV_518W == FYES) || (UTT_NV_518WLITE == FYES)|| (UTT_NV_518WVPN == FYES)|| (UTT_NV_518WPLUS == FYES)|| (UTT_NV_518 == FYES))
	SpdModeSet3052(index-1,spdSetMode);/*wan口模式。3052 wan口从0开始递增*/
#elif (UTT_NV_3520G == FYES)
	SpdModeSet3052(index*2,spdSetMode);/*设置所有lan口模式。3052 lan从芯片4口开始 3520G wan1--2 wan2--4*/
#else
	SpdModeSet3052(5-index,spdSetMode);/*wan口模式。3052 wan口从4开始递减*/
#endif
#elif  defined(CONFIG_RTL8306_PHY)
	/*以下行代码也可以不加。加上只是为了使ethtool显示内容与寄存器一致*/
	if(index ==  1) ethtoolSet(getWanIfName(index),spdSetMode);
#if (UTT_NV_840E == FYES) || (UTT_NV_800 == FYES) || (UTT_NV_840D == FYES)
	SpdModeSet8306(index - 1,spdSetMode, 0);/* wan1对应port0 wan2对应port1 以此类推 */
#else
	SpdModeSet8306(5-index,spdSetMode);/*wan口从4开始递减*/
#endif
#elif defined(CONFIG_IT8712_AR8316) 
	ethtoolSet(getWanIfName(index),spdSetMode);
#elif defined(CONFIG_IT82583)
	ethtoolSet(getWanIfName(index),spdSetMode);
#elif defined(CONFIG_RALINK_RT3883)
	SpdModeSet3883(5-index,spdSetMode);/*设置所有wan口模式。3883 wan从芯片4口开始*/
#elif defined(CONFIG_RALINK_MT7620) || defined(CONFIG_RALINK_MT7621) || defined(CONFIG_RALINK_MT7628) 
#if ((UTT_NV_750W == FYES) || (UTT_NV_842WV3 == FYES)|| (UTT_NV_811V4 == FYES)|| (UTT_NV_821V2 == FYES)|| (UTT_NV_841V4 == FYES) || (UTT_NV_810V4 == FYES)|| (UTT_NV_830V3 == FYES) || (UTT_NV_840V4 == FYES) || (UTT_NV_520WV3 == FYES) || (UTT_NV_D910 == FYES) || (UTT_NV_WX20S == FYES) || (UTT_U_1100 == FYES) || (UTT_U_1900 == FYES))
		/* 750W 硬件上接口顺序是错的*/
		if (index == 1) {
			SpdModeSet7620(3,spdSetMode);/*设置所有lan口模式。3883 lan从芯片1口开始*/
		} else if (index == 2) {
			SpdModeSet7620(2,spdSetMode);/*设置所有lan口模式。3883 lan从芯片1口开始*/
		} else if (index == 3) {
			SpdModeSet7620(1,spdSetMode);/*设置所有lan口模式。3883 lan从芯片1口开始*/
		} else if (index == 4) {
			SpdModeSet7620(0,spdSetMode);/*设置所有lan口模式。3883 lan从芯片1口开始*/
		} else {
		}
#elif (UTT_NV_840DV3 == FYES)||(UTT_NV_840EV3 == FYES)||(UTT_NV_517W == FYES)
	SpdModeSet7620(5 - index,spdSetMode);/*设置所有wan口模式,chipPort顺序与wan口顺序相反*/
#elif ((UTT_NV_512WV3 == FYES) || (UTT_NV_518WV3 == FYES) || (UTT_NV_515W == FYES) || (UTT_NV_D908W == FYES))
		if (index == 1) {
			SpdModeSet7620(4,spdSetMode);
		} else if (index == 2) {
			SpdModeSet7620(0,spdSetMode);
		} else if (index == 3) {
			SpdModeSet7620(1,spdSetMode);
		} else if (index == 4) {
			SpdModeSet7620(2,spdSetMode);
		} else {
		}
#elif ((UTT_NV_810V3 == FYES) || (UTT_NV_840V3 == FYES) || (UTT_NV_520V3 == FYES) || (UTT_NV_1220GW == FYES) ||(UTT_NV_1220V2 == FYES))
	SpdModeSet7620(5 - index,spdSetMode);/*设置所有wan口模式。3883 wan从芯片4口开始*/
#elif defined(UTT_NV_521GW)
	    SpdModeSet7620(5 - index, spdSetMode);/*设置所有lan口模式。3883 lan从芯片1口开始*/
#elif ((UTT_NV_750GW == FYES) || (UTT_NV_521GV2 == FYES)  || (UTT_NV_840GV2 == FYES))
	SpdModeSet7620(5 - index, spdSetMode);/*设置所有WAN口模式*/
#else
	SpdModeSet7620(index - 1,spdSetMode);/*设置所有wan口模式。3883 wan从芯片4口开始*/
#endif
#elif defined(CONFIG_PPC_85xx)
#if defined(CONFIG_BOARD_P1010_B)
	SpdModeSetP1010(5 - index,spdSetMode);/*设置所有wan口模式。8367m wan从芯片4口开始*/
#elif defined(CONFIG_BOARD_P1010_C)
	/* wan1~wan3设置8336，从wan4开始设置8367M */
	if (index <= P1010C_8336_IF_COUNT) {
	    SpdModeSetP1010_C_WAN(5 - index,spdSetMode);/* 设置8336 wan口, wan1 port4, wan2 port3.. */
	} else {
            SpdModeSetP1010((P1010C_8367M_IF_COUNT + P1010C_8336_IF_COUNT) - index,spdSetMode); /* 8367: wan4 - port4, wan5 - port3...*/
	}
#endif
#else
#error please add port speed mode set function for this chip
#endif

    }

    return;
}
    
static void initLanPortSpeed()
{
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
    
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
    if ((profIF != NULL) && (profIF->head.active == 1)) 
    {
	rddPortSpeedModeSet(0, profIF->ether.speedMode);
    }
}

static void initWanPortSpeed(int index)
{
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
    
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, index);
    if ((profIF != NULL) && (profIF->head.active == 1)) 
    {
	rddPortSpeedModeSet(index, profIF->ether.speedMode);
    }
}
#endif
#endif
extern  void interfaceProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const InterfaceProfile *oldProf, const InterfaceProfile *newProf )
{ 
	char oldIpStr[16];
	char newIpStr[16];
	struct in_addr sAddr;
       
#if (WEB_AUTH == FYES)
	char ip[16];
	memset(ip, 0, sizeof(ip));
	getIfIp(getLanIfName(), ip);
#endif

#if(UTT_NV_1800N != FYES)
    int edgeFlag;
    if(changeType == PROF_CHANGE_DEL) {
	if(oldProf->ether.connMode != ETH_CONN_PPPOE) {
	    doSystem("echo 0 >/var/run/Wan%dlink.status", index);
	    edgeFlag = WanLinkDown(index);
	    if (edgeFlag == 1) {
		ChangeBackLines(1);
	    }
	}
	LinkChkChg(index, newProf);
	initWan(index);
	return;
    }
#endif
//    printf("***************\n*****%s---%d***\n",__func__,__LINE__);
#if 1
#if (PORT_CONF_REDUCED == FYES)
    if(oldProf->ether.speedMode!=newProf->ether.speedMode)
    {
	DBGPRINT("%s-%d: index=%d, speedMode=%d\n",__func__,__LINE__,index, newProf->ether.speedMode);

	rddPortSpeedModeSet(index, newProf->ether.speedMode);

    }
#endif
#endif

//        printf("%s--%d\n",__func__,__LINE__);
#if 0
    if(oldProf->ether.connMode!=newProf->ether.connMode)
    {
        checkLink();
    }
#endif
    if(index != 0) {
	if(oldProf->detectInterval != newProf->detectInterval ||
		oldProf->detectCount != newProf->detectCount ||
		oldProf->detectIp != newProf->detectIp) {
	    LinkChkChg(index, newProf);
	}

#if(UTT_NV_1800N != FYES)
	if(oldProf->rxRate != newProf->rxRate) {
	    ModifyDefaultRoute(index, 0);
	}
#endif

#if (STATIC_ROUTE == FYES)
#if 0
	if(oldProf->srcRouteIpS != newProf->srcRouteIpS||
		oldProf->srcRouteIpE != newProf->srcRouteIpE) {
	    wanIpRuleFun(index, UTT_SPEED_WEB_SEND_MSG_ACT_DEL, 
		    ntohl(oldProf->srcRouteIpS), ntohl(oldProf->srcRouteIpE));
	    wanIpRuleFun(index, UTT_SPEED_WEB_SEND_MSG_ACT_ADD,
		    ntohl(newProf->srcRouteIpS), ntohl(newProf->srcRouteIpE));
	}
#endif
#endif
#if 0
	if(newProf->NatActive != oldProf->NatActive)
	{
	    extern void upnpupdata(void);
	    upnpupdata();
	}
#endif
//        printf("%s--%d\n",__func__,__LINE__);
	if((oldProf->isp != 0) && (oldProf->isp != newProf->isp))
	{
	    ISPRouteOldModifyTable((InterfaceProfile *)oldProf,0);
	}
	if((oldProf->isp1 != 0) && (oldProf->isp1 != newProf->isp1))
	{
	    ISPRouteOldModifyTable((InterfaceProfile *)oldProf,1);
	}
	if((oldProf->isp2 != 0) && (oldProf->isp2 != newProf->isp2))
	{
	    ISPRouteOldModifyTable((InterfaceProfile *)oldProf,2);
	}

    }
#if(UTT_NV_1800N != FYES)
    else if (index == 0) { /* lan口profile Change */
	/* 与lan口相关的配置未修改 */
	if (memcmp(&(oldProf->ether), &(newProf->ether), sizeof(struct st_ethernet_if)) != 0) {


	    /*
	     * 删除lan ip rule
	     */
	    _modifyLanIpRuleAndRoute(LAN_AC_DEL_LAN_RULE, oldProf->ether.sta.ip);
#if (MULTI_LAN == FYES)
	    _modifyLanIpRuleAndRoute(LAN_AC_DEL_LAN_RULE, oldProf->ether.sta.ip2);
	    _modifyLanIpRuleAndRoute(LAN_AC_DEL_LAN_RULE, oldProf->ether.sta.ip3);
	    _modifyLanIpRuleAndRoute(LAN_AC_DEL_LAN_RULE, oldProf->ether.sta.ip4);
#endif

	    /*
	     * init lan
	     */
	    initLan();
#if (HARDWARE_NAT == FYES)
#if defined(NAT_LOAD_BALANCE)
	    /*lan口地址变化则改变规则*/
	    if(oldProf->ether.sta.ip != newProf->ether.sta.ip)
	    {
		SystemProfile *profSys = NULL;
		MibProfileType mibType = MIB_PROF_SYSTEM;
		profSys = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);
		
		if ((ProfInstIsFree(profSys) == 0) && (profSys->hardwareNATEn == FUN_ENABLE))
		{
		   
		    /*转化为字串*/
		    sAddr.s_addr = oldProf->ether.sta.ip;
		    strcpy(oldIpStr,inet_ntoa(sAddr));

		    sAddr.s_addr = newProf->ether.sta.ip;
		    strcpy(newIpStr,inet_ntoa(sAddr));
    
		    delNlbJumpRule(oldIpStr);
		    createNlbJumpRule(newIpStr);
		}
	    }
#endif
#endif
#if (IPV6_INTERFACE == FYES)
          initIpv6Lan();
#endif
#if (MULTI_LAN == FYES)
	    initMultiLan();/*放到initLan之后，因为initLan调用lan.sh脚本会down掉lan口接口，接口相关的路由就会没了*/
#else
	    /*lan口地址变化则改变规则*/
	    if(oldProf->ether.sta.ip != newProf->ether.sta.ip)
	    {
#if (NOTICE == FYES)
		iptablesOnLan(oldProf->ether.sta.ip, newProf->ether.sta.ip);
#endif
	    }
#endif
#if ((VLAN_CONFIG == FYES)  && (IP_GRP == FYES))
	    add_vlan_sys_lan_group();
#endif
#if (EASY_TASK == FYES)
	    changeLANRedirectIp(oldProf, newProf);
#endif

#if (HOTEL_PNP ==FYES || EASY_TASK == FYES || UTT_DOMAINACCESS ==FYES)
	 editDNSRedirectRule();
#endif
#if (WEB_AUTH == FYES)
	doSystem("iptables -t nat -A %s -p tcp --dport 80 -j DNAT --to %s", WEB_AUTH_REDIRECT, ip);
#endif

if(oldProf->ether.sta.ip != newProf->ether.sta.ip){
#if(UTT_DOMAINACCESS ==FYES)
	sAddr.s_addr = oldProf->ether.sta.ip;
	strcpy(oldIpStr,inet_ntoa(sAddr));
	sAddr.s_addr = newProf->ether.sta.ip;
	strcpy(newIpStr,inet_ntoa(sAddr));

	DomainAccess(Iptables_DEL,oldIpStr);    
	DomainAccess(Iptables_INS,newIpStr);    
#endif
}
#if(WIFIDOG_MAKE_MONEY == FYES)
    /*赚钱wifi跟web认证重启wifidog可能冲突
     * 若两者要同时开启。这边需要更改*/
    makeMoneyOnLan();
#else
#if (WIFIDOG_AUTH == FYES) 
	sleep(2);
	wifidogExit();
	sleep(1);
	wifidogInit();
#endif
#endif

#if 0
#if (PORT_CONF_REDUCED == FYES)
#if defined CONFIG_IT8712_AR8316
	    /*
	     * for mantis 10412
	     * 6530G 模式改变后ethtool显示与寄存器不一致
	     * 在Lan口初始化后显示用 ethtool 设置Lan口模式
	     */
	    PortSpdSetSt spdSetMode;
	    converModeTospdSetMode(newProf->ether.speedMode, &spdSetMode);
	    if(spdSetMode.speed == 1000){
		spdSetMode.autoneg=1;
	    }
	    ethtoolSet(getLanIfName(), spdSetMode);	    /*为了使ethtool与寄存器显示一致*/
	    DBGPRINT("%s-%d: ifName=%s\n",__func__,__LINE__,getLanIfName());
#endif
#endif
#endif
	}
#if (FTP_SERVER == FYES)
	if (memcmp(&(oldProf->ether.sta), &(newProf->ether.sta), sizeof(struct st_ethernet_static)) != 0) {
        ftpServerInit();
    }
#endif
    } else {

    }
#endif
    SystemProfile *profBridge = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    if(profBridge != NULL && profBridge->brideg_mode_flag == 1) {
        doSystem("lan.sh bridge");
    }
    return;
}

#if(UTT_NV_1800N != FYES)
void InterfaceChange(Uint32 ifIndex)
{
    Uint32 wanCount = 0;
    wanCount = getWanSlotCount();
    if(ifIndex >= 0 && ifIndex <= wanCount) {
        if(ifIndex != 0) {
            initWan(ifIndex);
        } else {
            initLan();
        }
    }
    return;
}
#endif

/*
 * LanLinkUp		-		lan口起来后进行相关的初始化
 */
static void LanLinkUp(void)
{
    TIRouteChg();/*初始化静态路由*/
    return;
}
/*
 *wan口绑定网关单个接口添加函数
 */
static void ArpBindAdd(int number,InterfaceProfile *profIf)
{
    struct in_addr addrS;
    char *ipStr;
    addrS.s_addr = profIf->ether.sta.gw;
    ipStr = inet_ntoa(addrS);
    doSystem("arptables -N arp_wan%d_chain 1>/dev/null 2>&1",number); 
    doSystem("arptables -I INPUT -j arp_wan%d_chain -i %s+ 1>/dev/null 2>&1",number,getWanIfNamePPP(number)); 

    doSystem("arptables -I arp_wan%d_chain -s %s --source-mac %s -i %s -j RETURN",number,ipStr,converMac6To17Byte(profIf->GateWayMac),getWanIfNamePPP(number));
    doSystem("arptables -A arp_wan%d_chain -s %s -i %s -j DROP",number,ipStr,getWanIfNamePPP(number));
    doSystem("arptables -A arp_wan%d_chain --source-mac %s -i %s -j DROP",number,converMac6To17Byte(profIf->GateWayMac),getWanIfNamePPP(number));
    doSystem("ip neigh flush dev %s 2>/dev/null",getWanIfNamePPP(number));
}
/*
 *wan口绑定网关生效函数
 */
static void ArpBindWan(void)
{
    int index = 1,flag =0,i=0;
    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;
    for(i=0;i<getWanSlotCount();i++){
	doSystem("arptables -F arp_wan%d_chain",i+1); 
	doSystem("arptables -X arp_wan%d_chain 1>/dev/null 2>&1",i+1); 
	doSystem("arptables -D INPUT -j arp_wan%d_chain -i %s+ 1>/dev/null 2>&1",i+1,getWanIfNamePPP(i+1)); 
    }
    for(index=1;index<=getWanSlotCount();index++){
	profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, index);
     
	if((_getWanLinkState(index)!=0)&&(profIf != NULL) &&  (ProfInstIsFree(profIf) == 0) && (strcmp(MibGetStrByEnum(profIf->ether.connMode, ethConnModeEnum),"STATIC") == 0) && (profIf->BindWanEn == FUN_ENABLE)){
	    ArpBindAdd(index,profIf);
	    flag = 1;
	}
    }
    if(flag == 1){
	doSystem("ip neigh flush all"); 
    }

   return;
}
/*
 * 由于此函数会改变/var/run/Wan%dlink.status以及物理线路检测所需的其他值，
 * 因此调用此函数需加SEM_PHYDETECT_NO锁
 */
void InterfaceStatusChange(Uint32 wanIndex, Uint32 status)
{
    int edgeFlag = 0;

//	    printf("%s--%d \n",__func__,__LINE__);
    if(wanIndex > 0 && wanIndex <= getWanSlotCount()) {
	if(status == 0) {
	    printf("down \n");
	    doSystem("echo 0 >/var/run/Wan%dlink.status", wanIndex);
	    edgeFlag=WanLinkDown(wanIndex);
	    if (edgeFlag == 1) {
		ChangeBackLines(1);
	    }
	} else if(status == 1){
#if 0
	    if (!get_phylink_status(wanIndex))
		return;
#else
            /*线路起来了 则把相应的 物理线路检测计数器状态置为1
	     * 防止物理线路断开时 物理检测不到
	     * 但若线路断开了 无需置0
	     *  因为 1. 若是物理检测down的线路 则 物理检测会在适当时机up
	     *       2. 若不是物理检测down的，却up反而会异常*/
	    phyStaCounterSet(wanIndex, 1);
#endif
	    printf("up \n");
	    doSystem("echo 1 >/var/run/Wan%dlink.status", wanIndex);
	    edgeFlag=WanLinkUp(wanIndex);
	    if (edgeFlag == 1) {
		ChangeBackLines(0);
	    }
	}
	checkLink();
    } else {
	LanLinkUp();
    }

#if (DNS_AGENCY== FYES)
	 DnsAgencyInit();
#endif
#if (AC_WORK_MODE == FYES)
	 sleep(1);
	 MibProfileType mibType = MIB_PROF_AC_WORK_MODE;
	 AcWorkModeProfile *prof = NULL;
	 prof = (AcWorkModeProfile*)ProfGetInstPointByIndex(mibType, 0);
	 if(prof->workMode == PROF_ENABLE)
	 {
	     add_route_and_dns(prof);
	 }
#if((UTT_NV_WX100 == FYES) && (MULTI_LAN == FYES) && (WEB_BIND_ANY_SRC == FNO))
	 void LanRouteChg(unsigned ip, unsigned int netmask);
	 MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
	 InterfaceProfile *profIF = NULL;
	 unsigned int iIp = 0U;  
	 unsigned int iIpStart = 0U;
	 char ip[16]="";
	 profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
	 if ((profIF != NULL) && (profIF->head.active == 1))
	 {    
	     iIp = htonl(profIF->ether.sta.ip);
	     iIpStart = profIF->ether.sta.ip & profIF->ether.sta.nm;
	     doSystem("ip route flush table %d", LAN_ROUTE_TABLE_BASE);
	     if(iIp != 0U)
	     {/*接口启用了*/
		 int2ip(iIp, ip); 
		 LanRouteChg(iIpStart, profIF->ether.sta.nm);
	     }    
	 }
#endif
#endif
	ArpBindWan();
#if (IPV6_INTERFACE == FYES)   //接口重启时添加ipv6相关接口配置
        initIpv6Lan();
        Ipv6WanInit();
#endif
#if(IPV6_ROUTE == FYES)
        Ipv6RouteInit();
#endif
#if(IPV6_RADVD == FYES)
        reStartRadvd();
#endif
#if(IPV6_TUNNEL == FYES)       
        doSystem("ip tunnel del he-ipv6");
        Tunnelinit();
#endif
#if(IPV6_ISATAP == FYES)
        doSystem("/bin/ipv6/ip tunnel del name is0");
        Isatapinit();
#endif
#if (UTT_BRIDEG_MODE_FLAG == FYES)
        SystemProfile *profBridge = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
        if(profBridge != NULL && profBridge->brideg_mode_flag== 1) {
            doSystem("bridge_vif.sh add");
        }
#endif
    return ;
}

#if 0
void sysProfChangeForMP(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr )
{
    int i = 0;
    int wanCount = 0;
    InterfaceProfile *profIf = NULL;
    if(oldProfileDataPtr->lineGroupMode != newProfileDataPtr->lineGroupMode) {
        if(newProfileDataPtr->lineGroupMode == GROUP_LINE_BALANCE) {
            wanCount = getWanSlotCount();
            for(i = 0; i <= wanCount; i++) {
                profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, i);
                if(profIf->head.active == 1) {
                    
                }
            }
        }
    }
    return;
}
#endif

/********************************************************************
 * 函数： LanRouteChg
 * 功能： LAN口路由表变化
 * 创建： 2013-03-15
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static  void LanRouteChg(unsigned ip, unsigned int netmask)
{
    char landev[11];
    strncpy(landev, getLanIfName(), 10);
    doSystem("ip route add %N/%d table %d dev %s", ip, headbits(htonl(netmask))%100, LAN_ROUTE_TABLE_BASE, landev);
    return;
}
#if((MULTI_LAN == FYES) && (WEB_BIND_ANY_SRC == FNO))
/********************************************************************
 * 函数： initMultiLan
 * 功能： 多LAN口初始化SYS_LAN_GROUP组
 * 创建： 2012-09-04
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
void initMultiLan(void)
{
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
#if (NOTICE == FYES)
    NoticeProfile *profNotice = NULL;
#endif

    unsigned int iIp = 0U, iIp2 = 0U, iIp3 = 0U, iIp4 = 0U;
    unsigned int iIpStart = 0U, iIpStart2 = 0U, iIpStart3 = 0U, iIpStart4 = 0U;
    char ip[16], ip2[16], ip3[16], ip4[16];

    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
    if ((profIF != NULL) && (profIF->head.active == 1)) 
    {/*判断lan口profile是否正常*/
#if(IP_GRP == FYES)
	doSystem("ipset -F %s", SYS_LAN_GROUP);/*清空组*/
	doSystem("ipset -F %s", SYS_LAN_RANGE_GROUP);/*清空组*/
#endif
	iIp = htonl(profIF->ether.sta.ip);
	iIp2 = htonl(profIF->ether.sta.ip2);
	iIp3 = htonl(profIF->ether.sta.ip3);
	iIp4 = htonl(profIF->ether.sta.ip4);
	iIpStart = profIF->ether.sta.ip & profIF->ether.sta.nm;
	iIpStart2 = profIF->ether.sta.ip2 & profIF->ether.sta.nm2;
	iIpStart3 = profIF->ether.sta.ip3 & profIF->ether.sta.nm3;
	iIpStart4 = profIF->ether.sta.ip4 & profIF->ether.sta.nm4;
	doSystem("ip route flush table %d", LAN_ROUTE_TABLE_BASE);
	if(iIp != 0U)
	{/*接口启用了*/
	    int2ip(iIp, ip);
	    LanRouteChg(iIpStart, profIF->ether.sta.nm);
#if(IP_GRP == FYES)
	    ipsetAOrDIpStr(SYS_LAN_GROUP, ip, IPSET_MEM_ADD);/*加入组*/
	    ipsetAOrDNet(SYS_LAN_RANGE_GROUP, iIpStart, headbits(htonl(profIF->ether.sta.nm))%100, IPSET_MEM_ADD);
#endif
	}
	if(iIp2 != 0U)
	{/*接口启用了*/
	    int2ip(iIp2, ip2);
	    LanRouteChg(iIpStart2, profIF->ether.sta.nm2);
#if(IP_GRP == FYES)
	    ipsetAOrDIpStr(SYS_LAN_GROUP, ip2, IPSET_MEM_ADD);
	    ipsetAOrDNet(SYS_LAN_RANGE_GROUP, iIpStart2, headbits(htonl(profIF->ether.sta.nm2))%100, IPSET_MEM_ADD);
#endif
	}
	if(iIp3 != 0U)
	{/*接口启用了*/
	    int2ip(iIp3, ip3);
	    LanRouteChg(iIpStart3, profIF->ether.sta.nm3);
#if(IP_GRP == FYES)
	    ipsetAOrDIpStr(SYS_LAN_GROUP, ip3, IPSET_MEM_ADD);
	    ipsetAOrDNet(SYS_LAN_RANGE_GROUP, iIpStart3, headbits(htonl(profIF->ether.sta.nm3))%100, IPSET_MEM_ADD);
#endif
	}
	if(iIp4 != 0U)
	{/*接口启用了*/
	    int2ip(iIp4, ip4);
	    LanRouteChg(iIpStart4, profIF->ether.sta.nm4);
#if(IP_GRP == FYES)
	    ipsetAOrDIpStr(SYS_LAN_GROUP, ip4, IPSET_MEM_ADD);
	    ipsetAOrDNet(SYS_LAN_RANGE_GROUP, iIpStart4, headbits(htonl(profIF->ether.sta.nm4))%100, IPSET_MEM_ADD);
#endif
	}
#ifndef CONFIG_PERFORMANCE_OPTIMIZATION
#if (NOTICE == FYES)
	profNotice = (NoticeProfile*)ProfGetInstPointByIndex(MIB_PROF_NOTICE, 2);
	if((profNotice != NULL) && (ProfInstIsFree(profNotice) == 0)) {
	    if (profNotice->head.active == 1) {
		DnsNoticeChange(1);
	    }
	}
#endif
#endif
    }
    return;
}
#endif
#if (MULTI_LAN == FYES)
/********************************************************************
 * 函数： MultiLanArp
 * 功能： 多LAN口发送arp
 * 创建： 2012-09-07
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void MultiLanArp()
{
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;

    unsigned int iIp = 0U, iIp2 = 0U, iIp3 = 0U, iIp4 = 0U;
    char ip[16], ip2[16], ip3[16], ip4[16];

    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
    if ((profIF != NULL) && (profIF->head.active == 1)) 
    {/*判断lan口profile是否正常*/
	iIp = htonl(profIF->ether.sta.ip);
	iIp2 = htonl(profIF->ether.sta.ip2);
	iIp3 = htonl(profIF->ether.sta.ip3);
	iIp4 = htonl(profIF->ether.sta.ip4);
	if(iIp != 0U)
	{/*接口启用了*/
	    int2ip(iIp, ip);
	    doSystem("arping -q -c 50 -w 20 -I %s %s&", getLanIfName(), ip);
	}
	if(iIp2 != 0U)
	{/*接口启用了*/
	    int2ip(iIp2, ip2);
	    doSystem("arping -q -c 50 -w 20 -I %s %s&", getLanIfName(), ip2);
	}
	if(iIp3 != 0U)
	{/*接口启用了*/
	    int2ip(iIp3, ip3);
	    doSystem("arping -q -c 50 -w 20 -I %s %s&", getLanIfName(), ip3);
	}
	if(iIp4 != 0U)
	{/*接口启用了*/
	    int2ip(iIp4, ip4);
	    doSystem("arping -q -c 50 -w 20 -I %s %s&", getLanIfName(), ip4);
	}
    }
    return;
}
#endif
void funInitInterface()
{
#if (IPV6 == FYES)
    FILE *fp = NULL;
    if((fp = fopen("/tmp/ipv6resolv.conf","w+")) != NULL){
        fclose(fp);
    }
    if((fp = fopen("/tmp/ipv4resolv.conf","w+")) != NULL){
        fclose(fp);
    }
#endif
    registerForProfileUpdates(MIB_PROF_INTERFACE, NULL, (ProfileUpdateFnc)interfaceProfileChange);
#if 0
    registerForProfileUpdates(MIB_PROF_SYSTEM, NULL, sysProfChangeForMP);
#endif

    initInternet();
#if((MULTI_LAN == FYES) && (WEB_BIND_ANY_SRC == FNO))
#if(IP_GRP == FYES)
#ifdef CONFIG_RALINK_MT7620
    doSystem("iptables -t mangle -I INPUT -p tcp --dport 80 -m set ! --match-set %s dst -j DROP", SYS_LAN_GROUP);
#else
    doSystem("iptables -t mangle -I INPUT -p tcp --dport 80 -m set ! --set %s dst -j DROP", SYS_LAN_GROUP);
#endif
#else
#error ipset has been removed
#endif
    initMultiLan();
#endif
    checkLink();
# if(UTT_DOMAINACCESS == FYES)
    char IpStr[16];
    struct in_addr sAddr;

    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
    if(profIF!=NULL){
       sAddr.s_addr = profIF->ether.sta.ip;
       strcpy(IpStr,inet_ntoa(sAddr));
    }
    DomainAccess(Iptables_INS,IpStr);
#endif
}

#if(UTT_NV_1800N != FYES)
void upnpupdata(void)
{
     doSystem("upnp.sh");
}


#if (UPNP_GLOBAL_ENABLE == FYES)
/* initiate upnp client process  */
void initUpnpServerC(int wanIndex, char action)
{
    doSystem("kill `cat /var/run/upnp_wan%d.pid 2>/dev/null`  2>/dev/null", wanIndex);
    if (action == Iptables_ADD ) {
        doSystem("uttUpnpDemo %d %s %d&", wanIndex, getWanIfNamePPP(wanIndex), 
            UPNP_DEMO_PORT_BASE+wanIndex );
    }
}

void initUpnpServerS(void)
{
    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;
    int back = 0,i;
    for(i = 1; i <= getWanSlotCount(); i++) {
	profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, i);
	if((profIf->head.active == 1) &&(profIf->UpnpActive == 1)) {
	    if(back == 0)
	    {
		upnpupdata();
		back = 1;
	    }
	}
    }
}
#endif
#endif
