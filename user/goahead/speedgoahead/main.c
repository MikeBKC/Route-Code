
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/signal.h>
#include <linux/autoconf.h>
#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibUrcp.h"
#include "mibunion.h"
#include "profacce.h"
#include "sys/msg.h"

#if(UTT_NV_1800N != FYES)
extern void MpCombChange(int wanStatuses[MAX_WAN_IF_COUNT]);
extern void InterfaceChange(Uint32 ifIndex);
extern void InterfaceStatusChange(Uint32 wanIndex, Uint32 status);
#endif
#ifdef CONFIG_UTT_NETFILTER
int spdNfNlSock;
#endif
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
int profChangeNfNlSock;
#endif
#if 0
#if (EASY_TASK == FYES)
#if (WIRELESS == FYES)
extern void wireless_date_check();
extern int wireless_switch_change(int status);
#endif
#endif
#endif
#if (USER_STATUS == FYES)
extern int utt_stats_interval;
#endif

#if (IPV6_INTERFACE == FYES)
extern void funInitIpv6Interface();
#endif

#if (IPV6_ROUTE == FYES)
extern void funInitIpv6Route();
#endif

#if (IPV6_RADVD == FYES)
extern void funInitRadvd();
#endif

#if (IPV6_TUNNEL == FYES)
extern void funInitTunnel();
#endif

#if (IPV6_ISATAP == FYES)
extern void funInitIsatap();
#endif

#if (IPV6_DHCPV6 == FYES)
extern void funInitDhcp6();
#endif

#if (DHCP_AUTO_BIND == FYES)
extern int AutoDelCheck(int interval);
#endif
extern void funInitInterface();
#if (FEATURE_ACL == FYES)
extern void funInitAcl(void);
#endif
#if (ANTI_SNIFFER == FYES)
extern void funInitAntiSniffer(void);
#endif
#if (SMART_QOS_FUN == FYES)
extern void funInitSmartQos(void);
#endif
#if (DHCP_SERVER == FYES)
extern void funInitDhcpServer(void);
#endif
#if(MAKE_MONEY == FYES)
extern void funInitMakeMoney(void);
#endif
#if (FEATURE_SW_SNMP == FYES)
extern void funcInitSnmpConf(void);
#endif
#if (SNMP == FYES)
extern void funcInitSNMP(void);
#endif

#if (AP_PERMISSION == FYES)
extern void funInitApPermission(void);
#endif

#if (FEATURE_AP_MAIL == FYES)
extern void funInitApMailConf(void);
#endif

#if (AC_WORK_MODE == FYES)
extern void funInitAcWorkMode(void);
#endif

#if (AP_CONF == FYES)
extern void funInitApConf(void);
#endif

#if (AP_LOAD_BALANCE == FYES)
extern void funInitApLoadBalance(void);
#endif

#if (SERVICE_ZONE == FYES)
extern void funInitServiceZone(void);
#endif
#if (TAG_VLAN == FYES)
extern void funInitTagVlan(void);
#endif

#if (DNS_AGENCY == FYES)
extern void funInitDnsAgency(void);
#endif

#if (DHCP_POOL == FYES)
extern void funInitDhcpPool(void);
#endif

#if (VLAN_CONFIG == FYES)
extern void funInitVlanConfig(void);
#endif

#if (L2TP_SERVER == FYES)
extern void funInitL2tpServer(void);
#endif

#if (L2TP_CLIENT == FYES)
extern void funInitL2tpClient(void);
#endif

#if (PPTP_SERVER == FYES)
extern void funInitPptpServer(void);
#endif

#if (PPTP_CLIENT == FYES)
extern void funInitPptpCli(void);
#endif

#if (STATIC_ROUTE == FYES)
extern void funInitStaticRoute(void);
#endif

#if (NET_SECURITY == FYES)
extern void funInitNetSecurity(void);
#endif

#if (ARP_BINDS == FYES)
extern void funInitArpBind(void);
#endif

#if (PPPOE_SERVER == FYES)
extern void poeNProfShmInit(void);
extern void funInitPppoeServer(void);
#endif

#if (PVLAN_REDUCED == FYES)
extern void funInitRddPortVlan(void);
#endif

#if (UTTPDB == FYES)
extern void funInitPDB(void);
#endif
#if(UTT_NV_1800N != FYES)
extern void iptablesNatInit(void);
#if (STATIC_NAT == FYES)
extern void funInitStaticNat(void);
#endif
#if (NAT_RULE == FYES)
extern void funInitNatRule(void);
#endif
#if (DMZ == FYES)
extern void funInitDMZ(void);
#endif
#endif
#if (FEATURE_NTP == FYES)
extern void funInitNtp(void);
#endif

#if (DDNS == FYES)
extern void funInitDdns(void);
#endif

#if (TASK_SCHEDULER == FYES)
void funInitTaskSched(void);
#endif

#if (DNS_FILTER == FYES)
void funInitDnsFilter(void);
#endif
#if (CONNECT_LIMIT == FYES)
extern void funInitConnLimit(void);
#endif

#if (WIRELESS == FYES)
void funInitWirelessMacFilter(void);
void funInitWirelessMacFilterGlobal(void);
#if (WIRELESS_5G == FYES)
void funInitWireless5gMacFilter(void);
#endif
#endif

#if (FIREWALL == FYES)
extern void funInitFirewall(void);
#endif

#if (GROUP_BASE == FYES)
extern void funInitGroupBase(void);
#endif
#if (NOTICE == FYES)
extern void funInitNotice(void);
#endif
#if (CWMP == FYES)
extern void funInitCwmp(void);
#endif

#if (WEB_AUTH == FYES)
extern void funInitWebAuth(void);
#endif

#if (SYSLOG == FYES)
extern void funInitSyslog(void);
#endif

#ifdef CONFIG_UTT_NETFILTER
#if (EXCEPT_QQ== FYES)
extern void funInitExceptQQ(void);
#endif

#if (EXCEPT_MSN== FYES)
extern void funInitExceptMSN(void);
#endif
#if (EXCEPT_ALI== FYES)
extern void funInitExceptAli(void);
#endif
#endif
#if (IP_GRP == FYES)
extern void funInitIpGroup(void);
#endif
#if (PORT_GRP == FYES)
extern void funInitPortGroup(void);
#endif
#if (PORT_MIRROR_6530G == FYES)
extern void funInitMirror6530G(void);
#endif
#if (TIME_GRP == FYES)
extern void funInitTimeGrp(void);
#endif
#if (LOG_MANAGE == FYES)
extern void funInitLogManage(void);
#endif
#if (SYS_LOG_INFO == FYES)
extern void funInitSysLogInfo(void);
#endif
#if (CONFIG_RALINK_MT7620 == FYES || CONFIG_RALINK_MT7621 == FYES)
extern void funInitMirror7620e(void);
#endif
#if (IP_SEC == FYES)
extern void funInitIPSec(void);
#if ((PPTP_SERVER == FYES) || (PPTP_CLIENT == FYES))
extern void ipsecAutoupOverpptp(char *);
#endif
#endif
#if (HOTEL_PNP == FYES)
extern void funInitHotel(void);
#endif
#if (SSH_CLI == FYES)
extern void funInitSSH(void);
#endif
#if (HTTPS == FYES)
extern void funInitHttps(void);
#endif
#if (HARDWARE_NAT == FYES)
extern void funInitHardwareNAT(void);
#endif
#if (IDENTIFY_BIND == FYES)
extern void funInitIdentifyBind(void);
#endif
#if (UTT_LICENSE == FYES)
extern void licenseAuthMain(void);
#endif
#if (POLICY_ROUTE == FYES)
extern void funInitPolicyRoute(void);
#endif
#if (PARENTS_CONTROL == FYES)
extern void funInitParentsControl(void);
#endif
#if 0
extern void funInitUserStatus(void);
#endif
#if (FAT_FIT_SWITCH == FYES)
extern void funInitFatFitSwitch(void);
#endif

#if (FEATURE_AP_EFFECT_CONF == FYES)
extern void funInitApEffectConf(void);
#endif

#if (EASY_TASK == FYES)
extern void funInitEasyTask(void);
#endif
#if (MAC_FILTER == FYES)
extern void funInitMacFilter(void);
#endif
#if (NET_SNIPER == FYES)
extern void funInitNetSniper(void);
#endif
#if (NET_SHARE_USER == FYES)
extern void funInitNetShareUser(void);
#endif
#if (NET_SHARE_MANAGE == FYES)
extern void funInitNetShareManage(void);
#endif
#if (FTP_SERVER == FYES)
extern void funInitFtpServer(void);
extern void funInitFtpShareDir(void);
#endif

#if (CLI_FOR_ENGINEER_ONLY == FYES)
extern void funInitCliForEngineerOnly(void);
#endif

#if 0
#if (UTT_SMART_UI == FYES)
#if (NET_SHARE_MANAGE == FYES)
extern void netShareStatusCheck(void);
#endif
#endif
#endif

#if 0
#if ((UTT_NV_520W == FYES)||(UTT_U_1000N == FYES)||(UTT_NV_512W == FYES)|| (UTT_NV_518W == FYES)|| (UTT_NV_842W == FYES))
/********************************************************************
* 函数： DownUpApClient
* 功能： 520W动态WAN口时改变WAN口数量后重启要up出厂时的索引3接口，
*	    down现在的ap接口
* 创建： 2012-12-03
* 参数： 略
* 返回： 略
* 输出： 略
* 作者： Jianqing.Sun 
********************************************************************/
static void DownUpApClient(void)
{
    MibProfileType	mibType = MIB_PROF_INTERFACE;
    InterfaceProfile	*profIf = NULL;
    SystemProfile	*profSys = NULL;

    profSys = (SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    if (profSys->wanIfCount != 3) {/*一个系统正常运行必须有profSys结构体，故此处不作是否存在判断*/
	profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, 3);
	profIf->head.active = PROF_ENABLE;/*将出厂配置时的无线接口改为默认up*/
	profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, profSys->wanIfCount);
	profIf->head.active = PROF_DISABLE;/*新的无线接口默认down*/
    }
}
#endif
#endif

#if (HTTP_CACHE == FYES)
extern void funInitCache(void);
#endif
#if (NET_MODE_SWITCH == FYES)
#if 0
extern void funInitNetModeSwitch(void);
#endif
#endif
#if (BRIDGE == FYES)
#if 0
extern void funInitBridge(void);
#endif
extern void funInitNetConfig(void);
#endif
#if (CACHE_SERVER == FYES)
extern void funInitCacheServer(void);
extern void cacheNatRuleAOrD(int sta);
#endif

#if (NET_SHARE_MANAGE == FYES)
extern void netShareReset(struct st_mdevMsg mdevMsg);
#endif
#if (AP_SET_HOST_NAME == FYES)
extern void ap_set_hostname(void);
#endif
static void funAllInit()
{
#if 0
#if ((UTT_NV_520W == FYES)||(UTT_U_1000N == FYES)||(UTT_NV_512W == FYES)|| (UTT_NV_518W == FYES)|| (UTT_NV_842W == FYES))
    DownUpApClient();
#endif
#endif
#if (IP_GRP == FYES)
    funInitIpGroup();
#endif
#if (PORT_GRP == FYES)
    funInitPortGroup();
#endif
#if (TIME_GRP == FYES)
    funInitTimeGrp();
#endif
	funInitInterface();

#if (ANTI_SNIFFER == FYES)
    funInitAntiSniffer();
#endif
#if (SMART_QOS_FUN == FYES)
    funInitSmartQos();
#endif
#if (DHCP_SERVER == FYES)
    funInitDhcpServer();
#endif
#if (FEATURE_SW_SNMP == FYES)
    funcInitSnmpConf();
#endif
#if (SNMP == FYES)
    funcInitSNMP();
#endif

#if (AP_PERMISSION == FYES)
	funInitApPermission();
#endif

#if (AP_LOAD_BALANCE == FYES)
	funInitApLoadBalance();
#endif

#if (AP_CONF == FYES)
	funInitApConf();
#endif

#if (SERVICE_ZONE == FYES)
	funInitServiceZone();
#endif
#if (TAG_VLAN == FYES)
	funInitTagVlan();
#endif

#if (DNS_AGENCY == FYES)
	funInitDnsAgency();
#endif

#if (DHCP_POOL == FYES)
	funInitDhcpPool();
#endif
#if (VLAN_CONFIG == FYES)
	funInitVlanConfig();
#endif

#if (ARP_BINDS == FYES)
    funInitArpBind();
#endif

#if (L2TP_SERVER == FYES)
    /*
     * l2tp server
     */
    funInitL2tpServer();
#endif
#if (L2TP_CLIENT == FYES)
    /*l2tp client*/
    funInitL2tpClient();
#endif

#if (PPTP_SERVER == FYES)
    funInitPptpServer();
#endif

#if (PPTP_CLIENT == FYES)
    funInitPptpCli();
#endif

#if (STATIC_ROUTE == FYES)
    funInitStaticRoute();
#endif

#if (NET_SECURITY == FYES)
    funInitNetSecurity();
#endif

#if (PPPOE_SERVER == FYES)
    funInitPppoeServer();
#endif

#if (DDNS == FYES)
    funInitDdns();
#endif

#if (FEATURE_SW_PORT_CONF == FYES)
    funInitSwPortConf();
#endif
#if (FEATURE_PORT_VLAN == FYES)
    funInitPortVlan();
#endif

#if (PVLAN_REDUCED == FYES)
    funInitRddPortVlan();
#endif

#if (FEATURE_SW_PORT_MIRROR == FYES)
	funInitPortMirror();
#endif

#if (FEATURE_ACL == FYES)
    funInitAcl();
#endif

#if (FEATURE_SW_PORT_TRUNK == FYES)
	funInitPortTrunk();
#endif
#if (UTTPDB == FYES)
    funInitPDB();
#endif
#if(UTT_NV_1800N != FYES)
     iptablesNatInit();
#if (STATIC_NAT == FYES)
     funInitStaticNat();
#endif
#if (NAT_RULE == FYES)
     funInitNatRule();
#endif
#if (DMZ == FYES)
     funInitDMZ();
#endif
#endif
#if (FEATURE_NTP == FYES)
    funInitNtp();
#endif

#if (FEATURE_AP_MAIL == FYES)
    funInitApMailConf();
#endif

#if (TASK_SCHEDULER == FYES)
    funInitTaskSched();
#endif
#if (WIRELESS == FYES)
    funInitWireless();
#endif
#if (WIRELESS == FYES)
    funInitWirelessMacFilter();
    funInitWirelessMacFilterGlobal();
#if (WIRELESS_5G == FYES)
	funInitWireless5gMacFilter();
#endif
#endif

#if(MAKE_MONEY == FYES)
    funInitMakeMoney();
#endif

#if (FIREWALL == FYES)
    funInitFirewall();
#endif
#if (DNS_FILTER == FYES)
    funInitDnsFilter();
#endif
#ifdef CONFIG_UTT_NETFILTER
#if (EXCEPT_QQ == FYES)
    funInitExceptQQ();
#endif
#if (EXCEPT_MSN == FYES)
    funInitExceptMSN();
#endif
#if (EXCEPT_ALI == FYES)
    funInitExceptAli();
#endif
#endif
#if (GROUP_BASE == FYES)
     funInitGroupBase();
#endif

#if (FAT_FIT_SWITCH == FYES)
    funInitFatFitSwitch();
#endif
#if (FEATURE_AP_EFFECT_CONF == FYES)
    funInitApEffectConf();
#endif
#if (NOTICE == FYES)
    funInitNotice();
#endif
#if (IPV6_INTERFACE == FYES)
        funInitIpv6Interface();
#endif
#if (IPV6_ROUTE == FYES)
       funInitIpv6Route();
#endif

#if (IPV6_ISATAP == FYES)
       funInitIsatap();
#endif

#if (IPV6_RADVD == FYES)
       funInitRadvd();
#endif

#if (IPV6_TUNNEL == FYES)
       funInitTunnel();
#endif


#if (IPV6_DHCPV6 == FYES)
       funInitDhcp6();
#endif
#if (WEB_AUTH == FYES)
    funInitWebAuth();
#endif
#if (PORT_MIRROR == FYES)
#if (CONFIG_BOARD_MTK7620_E == FYES || CONFIG_RALINK_MT7621 == FYES || CONFIG_BOARD_MTK7620_EN == FYES)
    funInitMirror7620e();
#else
    funInitMirror821();
#endif
#endif
#if (PORT_MIRROR_6530G == FYES)
    funInitMirror6530G();
#endif
#if (LOG_MANAGE == FYES)
    funInitLogManage();
#endif
#if (IP_SEC == FYES)
    funInitIPSec();
#endif
#if (CONNECT_LIMIT == FYES)
    funInitConnLimit();
#endif
#if (SYSLOG == FYES)
    funInitSyslog();
#endif
#if (HOTEL_PNP == FYES)
    funInitHotel();
#endif
#if (SSH_CLI == FYES)
    funInitSSH();
#endif
#if (HTTPS == FYES)
    funInitHttps();
#endif
#if (HARDWARE_NAT == FYES)
    funInitHardwareNAT();
#endif
#if (IDENTIFY_BIND == FYES)
    funInitIdentifyBind();
#endif
#if (UTT_LICENSE == FYES)
    licenseAuthMain();
#endif
#if (POLICY_ROUTE == FYES)
    funInitPolicyRoute();
#endif
#if 0
    funInitUserStatus();
#endif
#if (HTTP_CACHE == FYES)
	 funInitCache();
#endif
#if (NET_MODE_SWITCH== FYES)
#if 0
	funInitNetModeSwitch();
#endif
#endif
#if (CACHE_SERVER == FYES)
	funInitCacheServer();
#endif
#if (SYS_LOG_INFO == FYES)
    funInitSysLogInfo();
#endif
#if (PARENTS_CONTROL == FYES)
    funInitParentsControl();
#endif
#if (EASY_TASK == FYES)
    funInitEasyTask();
#endif
#if (MAC_FILTER == FYES)
    funInitMacFilter();
#endif
#if (NET_SNIPER == FYES)
	funInitNetSniper();
#endif
#if (NET_SHARE_USER == FYES)
    funInitNetShareUser();
#endif
#if (FTP_SERVER == FYES)
    funInitFtpShareDir();
    funInitFtpServer();
#endif
#if (NET_SHARE_MANAGE == FYES)
    funInitNetShareManage();
#endif
#if (CWMP == FYES)
    funInitCwmp();
#endif
#if (AC_WORK_MODE == FYES)
	funInitAcWorkMode();
#endif

#if (AP_SET_HOST_NAME == FYES)
    ap_set_hostname();
#endif
#if (CAPWAP_CONFIG_AP == FYES)
    funInitCapwapConfig();
#endif
#if (CLI_FOR_ENGINEER_ONLY == FYES)
    funInitCliForEngineerOnly();
#endif
}

#if (FEATURE_AP == FYES)
static void FitApFunAllInit()
{
#if (AP_CONF == FYES)
    funInitApConf();
#endif

#if (WIRELESS == FYES)
    funInitWireless();
#endif

#if (WIRELESS == FYES)
    funInitWirelessMacFilter();
    funInitWirelessMacFilterGlobal();
#endif

#if (FAT_FIT_SWITCH == FYES)
    funInitFatFitSwitch();
#endif
#if (FEATURE_AP_EFFECT_CONF == FYES)
    funInitApEffectConf();
#endif

#if (AP_SET_HOST_NAME == FYES)
    ap_set_hostname();
#endif
}
#endif



//#if (UTT_NV_1800N == FYES)
#if (FEATURE_AP == FYES)
static void funcInit(void)
{
    FatFitSwitchProfile *prof1= NULL;
    ApEffectConfProfile *prof2= NULL;

    prof1 = (FatFitSwitchProfile*)ProfGetInstPointByIndex(MIB_PROF_FAT_FIT_SWITCH,0);

    if(prof1->fatEnable==1)
    {
	/*fat ap*/
	funAllInit();
#if (WIRELESS_TURBO == FYES)
        system("killall turbo; turbo&");
#endif
    }
    else
    {
	/*fit ap*/
	prof2 = (ApEffectConfProfile*)ProfGetInstPointByIndex(MIB_PROF_AP_EFFECT_CONF,0);
	if(prof2->config == AP_CONF_LOCAL)
	{
	    /*user local config*/
	    FitApFunAllInit();
#if (WIRELESS_TURBO == FYES)
        system("killall turbo; turbo&");
#endif
	}else{
        funInitApConf();
#if (CAPWAP_CONFIG_AP == FYES)
	funInitCapwapConfig(); 
#endif
    }

    }

    return;
}
#elif (NET_MODE_SWITCH == FYES)
/*
 *桥模式初始化
 *chen.bin
 *2013-01-29
 */
static void BridgeFunAllInit()
{
#if 0
	funInitBridge();
	funInitNetModeSwitch();
#endif
	funInitNetConfig();
#if (SYSLOG == FYES)
    funInitSyslog();
#endif
#if (TASK_SCHEDULER == FYES)
    funInitTaskSched();
#endif
#if (FEATURE_NTP == FYES)
    funInitNtp();
#endif
#if (HTTP_CACHE == FYES)
	 funInitCache();
#endif
#if (UTT_LICENSE == FYES)
	 licenseAuthMain();
#endif
}
static void funcInit(void)
{
	NetModeSwitchProfile *prof = NULL;
	prof = (NetModeSwitchProfile*)ProfGetInstPointByIndex(MIB_PROF_NET_MODE_SWITCH,0);

	if(prof!=NULL)
	{
		if(prof->bridgeEnable==PROF_DISABLE)
		{
			/*路由模式*/
			funAllInit();
		}
		else
		{
			/*桥模式*/
			BridgeFunAllInit();
		}
	}
	else
	{
		funAllInit();
	}
	
}
#endif

#if ((EASY_TASK == FYES) && (HOTEL_PNP == FYES))
static void editHotelByDefConf(void)
{
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;

    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibTypeSys, 0);
    if (ProfInstIsFree(profSys) == 0)
    {
	if (profSys->isDefaultConf == 0)
	{
	    profSys->hotelPnpEn = FUN_ENABLE;
	}
    }
}
#endif

//#if ((UTT_NV_1800N == FYES) || (UTT_NV_510V2 == FYES))
#if (FEATURE_AP == FYES)
static void funcReInit(void)
{
    FatFitSwitchProfile *prof1= NULL;

    prof1 = (FatFitSwitchProfile*)ProfGetInstPointByIndex(MIB_PROF_FAT_FIT_SWITCH,0);

    if(prof1->fatEnable==1)
    {
	/*fat ap*/
	funAllInit();
    }
    else
    {
		funInitInterface();
		funInitFitApWireless();
	    /*fit ap*/
	    FitApFunAllInit();
    }
#if (WIRELESS_TURBO == FYES)
        system("killall turbo; turbo&");
#endif

    return;

}
#endif

#if (EASY_TASK == FYES)
/*
 * isDefaultConf	-	判断是否为出厂配置
 *
 * 若由1.5以前版本升级到1.5，则isDefaultConf成员肯定为0，此时
 * 需要判断配置来确定是否为出厂配置（1.5中改了WAN口或者自己手
 * 动在页面点击无须配置则认为此后不再是出厂配置）
 */
static void isDefaultConf(void)
{
	SystemProfile	*profSys = NULL;
	profSys = (SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
	if (profSys->isDefaultConf == 0) {
		profSys->isDefaultConf = IsDefaultConf();
	}
	return;
}
#endif
#if (TAG_VLAN == FYES)
extern void tagVlanRouteChg(void);
#endif

#if 1
//#if (PPPOE_ACCOUNT == FYES) /* not only for pppoe account */
extern void pollTaskInit(int crOrRe);
extern void wanLinkCheckTaskInit(int crOrRe);
extern void makeFactoryProfiles(void);
//#endif

/**
 * 非profile的共享内存初始化
 * 必须放在ProfInit之后，因为ProfInit初始化共享内存其他相关
 * 必须放在功能初始化之前，因为功能可能用到改函数
 *
 * 注意：改函数可能导致speedweb的重启出现问题（不包括断电重启和系统重启）
 *
 *       如 speedweb被杀死时 不会杀死pppoer-server
 *       但重启时 会重新初始化 会话。从而导致会话异常
 *
 *       若后期应用要speedweb必须重启。需添加信号量支持
 *       或把该函数移植到库或其他地方。但需保证开机后只能运行一次，否则同样可能发生异常
 * 
 * 2013-08-06
 * bhou
 *
 */
void nProfShmInit(void)
{
#if (PPPOE_SERVER == FYES)
    poeNProfShmInit();
#endif
#if (VPN_CONTROL == FYES)
    int i = 0;
    nvramProfile->vpnProfile.count = MAX_VPN_SESSION_NUM;
#if (IP_SEC == FYES)
#if (VPN_ITEM_CONTROL == FYES)
    for(i = 0;i<MAX_VPN_ITEMS_NUM;i++)
#else
    for(i = 0;i<MAX_IPSEC_PROFILES;i++)
#endif
    {
	memset(&(nvramProfile->saState[i]), 0U, sizeof(saStatus));
    }
#endif
#endif
}
#ifdef CONFIG_UTT_MT7628_FLASH_SOFTEN  

static char IDDATA[4]={0x2d, 0x4b, 0x68, 0x3a};
int uttSoftEncodeFlash()
{
    FILE *flash_fp;
    unsigned char flash_line[64],flash_name[16],flash_factory[9],flash_tmp[8],flash_id[8],buf[8],flash_id_l[4],flash_id_r[4];
    unsigned char flash_sn_hmd5[4],flash_sn_tmp[4],flash_sosn_tmp[4];
    unsigned int flash_sosn,flash_sn;
    int i;

    flash_fp = fopen("/proc/uttFlashId","r");

    if(flash_fp != NULL)
    {    
	fgets(flash_line, sizeof(flash_line), flash_fp);
	sscanf(flash_line,"%s%s",flash_name,flash_factory);
	fgets(flash_line, sizeof(flash_line), flash_fp);
	sscanf(flash_line,"%s%x%x",flash_name,flash_id_l,flash_id_r);
    }
    fclose(flash_fp);

    if(memcmp(flash_factory,"ef",2)!=0)
    {
	/*printf("it is not windbond-------------\n");*/
	return -1;
    }

    for( i = 0 ; i < 4 ; i++ )
    {
	memcpy(&flash_id[i],&flash_id_l[3-i],1);
	memcpy(&flash_id[i+4],&flash_id_r[3-i],1);
    }

    flash_read_mtdname("Bootloader", flash_tmp, 0x8, 8);

    hmac_md5((char*)(IDDATA), 4, (unsigned char*)flash_id, 8, buf);
    if(memcmp(flash_tmp,buf,8)==0)
    {
	/* printf("------------flash --------------------ok!-----------\n");*/
    }
    else
    {
	return -1;
    }
    flash_read_mtdname("Bootloader", &flash_sn, 0x2c, 4); 
    flash_read_mtdname("Bootloader", &flash_sosn, 0x10, 4); 

    printf("flash_sn ====%d------------flash_sosn =========%x======\n",flash_sn,flash_sosn);
    if((flash_sn == 0xffffffff) && (flash_sosn == 0xffffffff))
    {
	/* printf("---------------sn = -1--------------\n");*/
    }
    else
    {
	memcpy(flash_sn_tmp,&flash_sn,4);
	memcpy(flash_sosn_tmp,&flash_sosn,4);
	hmac_md5((char*)(IDDATA), 4, (unsigned char*)flash_sn_tmp, 4, flash_sn_hmd5);
	if(memcmp(&flash_sosn,flash_sn_hmd5,4)==0)
	{
	    /*printf("--------------------sn-----------------ok!-------------\n");*/
	}
	else
	{
	    return -1;
	}
    }
    return 0;
}
#endif
#if(UTT_SPEEDWEB_CORE_DUMP == FYES)
struct sigaction sa_usr,oact;
void sig_usr(int a )
{
    struct rlimit coredump;
    memset(&coredump, 0, sizeof(struct rlimit));
    coredump.rlim_cur = RLIM_INFINITY;
    coredump.rlim_max = RLIM_INFINITY;
    setrlimit(RLIMIT_CORE, &coredump);
    //system("killall goaheadCore.sh");
    //system("goaheadCore.sh  &");
    printf("CoreDump !\n");
    sigaction(SIGSEGV, &oact, NULL);
}
#endif
int main(int argc, char** argv)
{

#ifdef CONFIG_UTT_MT7628_FLASH_SOFTEN  
    if(uttSoftEncodeFlash()<0)
    {
	/*如果加密检测失败，则杀掉speed*/
	char *speed=NULL;
	printf("-----------------------------------kill speedweb--------------------------\n");
	strcpy(speed,"killall speed");
    }  
#endif
    int msgid;
    struct st_uttMsg msg;
    SlotNumber slot;

    slot.slotNumber = SLOT_NUMBER_ANY; 
#if(UTT_SPEEDWEB_CORE_DUMP == FYES)
    system("config-debug.sh &");
    sa_usr.sa_flags |= SA_RESTART;
    sa_usr.sa_handler = sig_usr;
    sigaction(SIGSEGV, &sa_usr, &oact);
#endif
#ifdef CONFIG_UTT_NETFILTER
#if 1
    spdNfNlSock = uttNfNlSkBind(pthread_self() << 16 | getpid(), 0);/*单播到本进程*/
#else
    spdNfNlSock = uttNfNlSkBind(getpid(), 0);/*单播到本进程*/
#endif
    if(spdNfNlSock <  0)
    {
	return -1;/*bind异常直接结束*/
    }
#endif

#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    profChangeNfNlSock = uttNfNlProfChangeSkBind(pthread_self() << 16 | getpid(), 0);
    if(profChangeNfNlSock <  0)
    {
	return -1;/*bind异常直接结束*/
    }
#endif
    ProfInit();
#if SW_FEATURE_URCP || (FEATURE_AC == FYES)
	urcp_ipc_init();
#endif
    makeFactoryProfiles();
    ProfReadFromFlash();
#if (UTT_NV_D908W == FYES)
    /*
     * AC功能开启，把FALSH中的AP配置还原到/tmp/urcp/ap_config/
     * */
    apConfigReadFromFlash();
    /*AP分布图，从FLASH中读取*/
    apDrawingReadFromFlash();
#endif
    /*非profile架构的共享内存变量初始化
      必须放在所有功能初始化之前* */
    nProfShmInit();
#if (EASY_TASK == FYES)
    isDefaultConf();
#if (UTT_SMART_UI != FYES)
#if (HOTEL_PNP == FYES)
    editHotelByDefConf();
#endif
#endif
#endif
#if (CAPWAP_CONFIG_AP == FYES)
    initCapwap(); 
#endif
#if (FEATURE_AP == FYES)
    funInitInterface();
#endif
//#if ((UTT_NV_1800N != FYES) || (UTT_NV_510V2 != FYES))
#if (FEATURE_AP == FYES)
	funcInit();
#elif (NET_MODE_SWITCH == FYES)
funcInit();
#else
	funAllInit();
#endif

#if ((GROUP_BASE == FYES) || (WEB_AUTH == FYES))
    extern void uttNfNlTaskInit(void);
    uttNfNlTaskInit();/*在共享内存等初始化完后。初始化netlink任务。因为此任务可能需要读配置*/
#endif
    msgid = UttMsgInit();

#ifndef CONFIG_RALINK_RT2880
//#if (PPPOE_ACCOUNT == FYES)
    pollTaskInit(1);
//#endif
#endif
    wanLinkCheckTaskInit(1);
#if (UTT_SMART_UI == FYES)
#if (NET_SHARE_MANAGE == FYES)
#if 0
    netShareStatusCheck();
#endif
#endif
#endif

#if (AC_WORK_MODE == FYES)
	funInitAcWorkMode();
#endif

#if (UDP_PORT_SCAN_DEFENSE == FYES)
	/*丢掉由外网建立到路由udp端口的新连接，防止恶意攻击,snmp,l2tp,IPSec连接例外*/
	doSystem("iptables -A INPUT -m uttdev ! --is-lan-in -p udp -m multiport ! --dport 161,500,1701,4500,34951 -m conntrack --ctstate NEW -j DROP");
#endif
#if (ADFILTER == FYES)
    funInitAdfilter();
#endif

#if (CAPWAP_CONFIG_AC == FYES)
    initCapwap(); 
#endif

#if (RESTART_GOAHEAD == FYES)
    doSystem("RestartGoahead &");
#endif
#if (UTT_BRIDEG_MODE_FLAG == FYES)
    SystemProfile *profBridge = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    if(profBridge != NULL && profBridge->brideg_mode_flag == 1) {
        doSystem("killall -9 goahead;sleep 1;goahead &");
    }
#endif
#if (APP_ELINKC == FYES)
        doSystem("elinkc.sh &");
#endif
    while(1) {
        msgrcv(msgid, &msg, sizeof(msg.msgPara), UTT_MSG_TYPE_1, 0);
	switch(msg.msgPara.mainType) {
            case MSG_PROF:
                reportProfileUpdate(slot, msg.msgPara.secType, msg.msgPara.data.profChangeData.profType,
                        msg.msgPara.data.profChangeData.instIndex,
                        &(msg.msgPara.data.profChangeData.oldDataPtr),
                        msg.msgPara.data.profChangeData.flag);
                break;
            case MSG_NVRAM:
                ProfWiteToFlash();
                break;
#if(UTT_NV_1800N != FYES)
            case MSG_MPGLOBALE_CHANGE:
                MpCombChange(msg.msgPara.data.wanStatuses);
                break;
            case MSG_INTERFACE_CHANGE:
		InterfaceChange(msg.msgPara.data.ifIndex);
		break;
#endif
	    case MSG_LINECHK_STA_CHANGE:
		/*
		 *此处为加上信号量锁定此操作
		 */
		uttSemP(SEM_PHYDETECT_NO, 0);
		InterfaceStatusChange(msg.msgPara.data.linkCheckMsg.wanIndex,
			msg.msgPara.data.linkCheckMsg.status);
		 /*
		 *此处为加上信号量锁定此操作
		 */
		uttSemV(SEM_PHYDETECT_NO, 0);

		break;

#if (USER_STATUS == FYES)
	    case MSG_NL_UPDATE_STATS_INTERVAL_CHANGE:
		utt_stats_interval = msg.msgPara.data.utt_stats_update_interval;
		break;
#endif
#if (DHCP_AUTO_BIND == FYES)
	    case MSG_NL_UPDATE_AUTODEL_INTERVAL_CHANGE:
		AutoDelCheck(msg.msgPara.data.utt_autodel_update_interval);
		break;
#endif
//#if ((UTT_NV_1800N == FYES) || (UTT_NV_510V2 == FYES))
#if (FEATURE_AP == FYES)
	    case MSG_PROF_REINIT:
				funcReInit();
		break;
#endif
#if (IP_SEC == FYES)
#if ((PPTP_SERVER == FYES) || (PPTP_CLIENT == FYES))
	    case MSG_IPSEC_AUTOUP_OVER_PPTP_CHANGE:
		ipsecAutoupOverpptp(msg.msgPara.data.utt_ipsec_autoup);
		break;
#endif
#endif
#if (CACHE_SERVER == FYES)
		case MSG_CACHE_STA_CHANGE:
		cacheNatRuleAOrD(msg.msgPara.data.utt_cache_sta);
		break;
#endif
#if (EASY_TASK == FYES)
		case MSG_LINK_STA_CHANGE:
		wanConnStatsChange(msg.msgPara.data.utt_link_status);
		break;
#if 0
#if (WIRELESS == FYES)
		case MSG_WIRELESS_SWITCH_CHANGE:
		if (msg.msgPara.data.utt_wireless_status == 2)
			wireless_date_check();
		else
			wireless_switch_change(msg.msgPara.data.utt_wireless_status);
		break;
#endif
#endif
#endif
#if (TAG_VLAN == FYES)
	    case MSG_TAG_VLAN_ROUTE_CHG:
		tagVlanRouteChg();
		break;
#endif
#if (NET_SHARE_MANAGE == FYES)
	    case MSG_MDEV_CHG:
		netShareReset(msg.msgPara.data.mdevMsg);
		break;
#endif

            default:
                break;
        }
    }
    exit(0);
}
#else

void mainSec(struct st_uttMsg *msg)
{
        switch(msg.msgPara.mainType) {
            case MSG_PROF:
                reportProfileUpdate(slot, msg.msgPara.secType, msg.msgPara.data.profChangeData.profType,
                        msg.msgPara.data.profChangeData.instIndex,
                        &(msg.msgPara.data.profChangeData.oldDataPtr),
                        msg.msgPara.data.profChangeData.flag);
                break;
            case MSG_NVRAM:
                ProfWiteToFlash();
                break;

            default:
                break;
        }
}
#endif
