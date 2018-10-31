
#ifndef __MIB_SHARE_H
#define __MIB_SHARE_H

#include "typedefShare.h"
#include "uttMachine.h"
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "mibSystemShare.h"
#include "mibInterfaceShare.h"
#include "mibUserShare.h"

#if (IPV6_INTERFACE == FYES) 
#include "mibIpv6InterfaceShare.h"
#endif

#if (IPV6_RADVD == FYES)
#include "mibRadvdShare.h"
#endif

#if(IPV6_NOTLISTPREFIXS == FYES)
#include "mibPrefixListShare.h"

#endif
#if (IPV6_DHCPV6 == FYES)
#include "mibDhcp6Share.h"
#endif

#include "mibUserShare.h"

#if (IPV6_ROUTE == FYES)
#include "mibIpv6RoutConfigShare.h"
#endif

#if (IPV6_TUNNEL == FYES)
#include "mibTunnelShare.h"
#endif

#if (IPV6_ISATAP == FYES)
#include "mibIsatapShare.h"
#endif

#if (IP_GRP == FYES)	
#include "mibIpGroupShare.h"
#endif

#if (PORT_GRP == FYES)	
#include "mibFwServiceShare.h"
#endif

#if (TIME_GRP == FYES)	
#include "mibTimeGrpShare.h"
#endif

#if (DHCP_SERVER == FYES)
#include "mibDhcpServerShare.h"
#endif

#if (DHCP_POOL == FYES)
#include "mibDhcpPoolShare.h"
#endif

#if (VLAN_CONFIG == FYES)
#include "mibVlanConfigShare.h"
#endif

#if (DNS_AGENCY == FYES)
#include "mibDnsAgencyShare.h"
#endif

#if (L2TP_SERVER == FYES)
#include "mibL2TPServerShare.h"
#endif

#if (L2TP_CLIENT == FYES)
#include "mibL2TPClientShare.h"
#endif

#if (PPTP_SERVER == FYES)
#include "mibPPTPServerShare.h"
#endif


#if (AP_PERMISSION == FYES)
#include "mibApPermissionShare.h"
#endif

#if (AP_CONF == FYES)
#include "mibApConfShare.h"
#endif

#if (SERVICE_ZONE == FYES)
#include "mibServiceZoneShare.h"
#endif

#if((FEATURE_AP_MAC_FILTER == FYES)||(FIT_AP_MAC_FILTER == FYES))
#include "mibApMacFilterShare.h"
#endif
#if (TAG_VLAN == FYES)
#include "mibTagVlanShare.h"
#endif

#if (PPTP_CLIENT == FYES)
#include "mibPptpClientShare.h"
#endif

#if (STATIC_ROUTE == FYES)
#include "mibRouteShare.h"
#endif

#if (DDNS == FYES)
#include "mibDdnsShare.h"
#endif

#if (PPPOE_SERVER == FYES)
#include "mibPppoeServerShare.h"
#endif

#if (FEATURE_SW_PORT_CONF == FYES)
#include "mibSwPortConfShare.h"
#endif

#if (FEATURE_PORT_VLAN == FYES)
#include "mibPortVlanShare.h"
#endif
#if (PVLAN_REDUCED == FYES)
#include "mibRddPortVlanShare.h"
#endif
#if (GROUP_BASE == FYES)
#include "mibGroupBaseShare.h" 
#endif

#if (FEATURE_SW_PORT_MIRROR == FYES)
#include "mibMirrorShare.h"
#endif
#if (FEATURE_SW_PORT_TRUNK == FYES)
#include "mibTrunkShare.h"
#endif

#if (FEATURE_SW_SNMP == FYES)
#include "mibSnmpShare.h"
#endif

#if (SNMP == FYES)
#include "mibSNMPShare.h"
#endif
#if (FEATURE_ACL == FYES)
#include "mibACLShare.h"
#endif

#if (UTTPDB == FYES)
#include "mibpdbShare.h"
#endif

#if ((STATIC_NAT == FYES)||(CLOUD_MANAGE == FYES))
#include "mibnatShare.h" 
#endif

#if (ADMIN_CONF == FYES)
#include "mibAdminConfShare.h"
#endif

#if (DNS_FILTER == FYES)
#include "mibDnsFilterShare.h"
#endif

#if (CLI_FOR_ENGINEER_ONLY == FYES)
#include "mibCliForEngineerOnlyShare.h"
#endif

#if (FAT_FIT_SWITCH == FYES)
#include "mibFatFitSwitchShare.h"
#endif

#if (AC_WORK_MODE == FYES)
#include "mibAcModeShare.h"
#endif

#if (AP_LOAD_BALANCE == FYES)
#include "mibApLoadBalanceShare.h"
#endif

#if (WIRELESS == FYES)
#include "mibWirelessShare.h"
#endif

#if (FEATURE_NTP == FYES)
#include "mibNtpShare.h"
#endif
#if (FEATURE_LANG == FYES)
#include "mibLangShare.h"
#endif

#if (TASK_SCHEDULER == FYES)
#include "mibTaskSchedShare.h"
#endif

#if (FIREWALL == FYES)
#include "mibfirewallShare.h"
#endif

#if (NOTICE == FYES)
#include "mibNoticeShare.h"
#endif

#if (CWMP == FYES)
#include "mibCwmpShare.h"
#endif

#if (WEB_AUTH == FYES)
#include "mibWebAuthShare.h"
#endif

#if (EXCEPT_QQ == FYES)
#include "mibExceptQQShare.h"
#endif

#if (EXCEPT_MSN == FYES)
#include "mibExceptMSNShare.h"
#endif

#if (EXCEPT_ALI == FYES)
#include "mibExceptAliShare.h"
#endif

#if (LOG_MANAGE == FYES)
#include "mibLogManageShare.h"
#endif

#if (AP_LOG_MANAGE == FYES)
#include "mibApLogManageShare.h"
#endif

#if (IP_SEC == FYES)
#include "mibipsecShare.h"
#endif
#if (SYSLOG == FYES)
#include "mibSyslogShare.h"
#if (SYSLOG_HEARTBEAT == FYES)
#include "mibLogHelloMsgShare.h"
#endif
#endif
#if (UTT_LICENSE == FYES)
#include "miblicenseShare.h"
#endif
#if (POLICY_ROUTE == FYES)
#include "mibPolicyRouteShare.h"
#endif
#if (PARENTS_CONTROL == FYES)
#include "mibParentsControlShare.h"
#endif

#if (FEATURE_AP_MAIL == FYES)
#include "mibApMailShare.h"
#endif

#if (FEATURE_AP_CONF_TEMP == FYES)
#include "mibApConfTempShare.h"
#endif

#if (FEATURE_AP_SCAN == FYES)
#include "mibApScanShare.h"
#endif

#if (FEATURE_AP_SW_MNG == FYES)
#include "mibApSwMngShare.h"
#endif

#if (FEATURE_AP_BASIC_INFO == FYES)
#include "mibApBasicConfShare.h"
#endif

#if (FEATURE_AP_EFFECT_CONF == FYES)
#include "mibApEffectConfShare.h"
#endif

#if (FIT_AP_SERVICE_ZONE == FYES)
#include "mibFitApSZShare.h"
#endif

#if (AP_BLACK_WHITE_LIST == FYES)
#include "mibApBlackWhiteListShare.h"
#endif

#if(CAPWAP_CONFIG_AP == FYES)
#include "mibCapWapConfigShare.h"
#endif

#if (HTTP_CACHE == FYES)
#include "mibCacheShare.h"
#endif

#if (NET_MODE_SWITCH  == FYES)
#include "mibNetModeSwitchShare.h"
#endif

#if (BRIDGE == FYES)
#include "mibBridgeShare.h"
#endif

#if (P2P_LIMIT == FYES)
#include "mibP2PLimitShare.h"
#endif

#if (SYS_LOG_INFO == FYES)
#include "mibSysLogInfoShare.h"
#endif

#if (EASY_TASK == FYES)
#include "mibEasyTaskShare.h"
#endif

#if (CACHE_SERVER == FYES)
#include "mibCacheServerShare.h"
#endif

#if (MAC_FILTER == FYES)
#include "mibMacFilterShare.h"
#endif

#if (VPN_CONTROL == FYES)
#include "mibVpnControlShare.h"
#endif

#if (NET_SNIPER == FYES)
#include "mibNetSniperShare.h"
#endif

#if (UTT_SMART_UI == FYES)
#if (APP_MENU == FYES)
#include "mibAppMenuShare.h"
#endif
#endif

#if (NET_SHARE_MANAGE == FYES)
#include "mibNetShareManageShare.h"
#endif

#if (FTP_SERVER == FYES)
#include "mibFtpServerShare.h"
#endif

#if (NET_SHARE_USER == FYES)
#include "mibNetShareUserShare.h"
#endif

#if (ADFILTER == FYES)
#include "mibAdfilterShare.h"
#endif

#if (MAKE_MONEY == FYES)
#include "mibMakeMoneyShare.h"
#endif

#if (CLOUD_CONFIG == FYES)
#include "mibCloudConfigShare.h"
#endif

#if (UTT_KNOWIFI == FYES)
#include "mibKnowifiDomainShare.h"
#endif

typedef enum {
    MIB_PROF_SYSTEM = 0,
    MIB_PROF_INTERFACE,
#if (IPV6_INTERFACE == FYES)
    MIB_PROF_IPV6_INTERFACE,
#endif
#if (IPV6_RADVD == FYES)
    MIB_PROF_RADVD,
#endif

#if (IPV6_NOTLISTPREFIXS == FYES)
    MIB_PROF_IPV6_NOTLISTPREFIXS,
#endif
#if (IPV6_ROUTE == FYES)
    MIB_PROF_IPV6_ROUTECONF,
#endif
#if (IPV6_DHCPV6 == FYES)
    MIB_PROF_DHCP6,
#endif
#if (IPV6_TUNNEL == FYES)
    MIB_PROF_TUNNEL,
#endif
#if (IPV6_ISATAP == FYES)
    MIB_PROF_ISATAP,
#endif
#if (IP_GRP == FYES)
    MIB_PROF_IPGRP,
#endif
#if (PORT_GRP == FYES)
    MIB_PROF_PORTGRP,
#endif
#if (TIME_GRP == FYES)
    MIB_PROF_TIMEGRP,
#endif
#if (DHCP_SERVER == FYES)
    MIB_PROF_DHCP_SERVER,
#endif
    MIB_PROF_USER,
#if (FEATURE_SW_PORT_CONF == FYES)
    MIB_PROF_SW_PORT_CONF,
#endif
#if (FEATURE_PORT_VLAN == FYES)
    MIB_PROF_PORT_VLAN,
#endif
#if (FEATURE_SW_PORT_MIRROR == FYES)
	MIB_PROF_MIRROR,
#endif
#if (GROUP_BASE == FYES)
    MIB_PROF_GROUP_BASE,
#endif
#if (L2TP_SERVER == FYES)
    MIB_PROF_L2TP_SRV_GLOBAL,
    MIB_PROF_L2TP_SRV_ACCOUNT,
#endif
#if (L2TP_CLIENT == FYES)
    MIB_PROF_L2TP_CLIENT,
#endif
#if (FEATURE_SW_PORT_TRUNK == FYES)
	MIB_PROF_TRUNK,
#endif
#if (PPTP_SERVER == FYES)
    MIB_PROF_PPTP_SRV_GLOBAL,
    MIB_PROF_PPTP_SRV_ACCOUNT,
#endif
#if (UTTPDB == FYES)
    MIB_PROF_PDB_GLOBAL,
#endif
#if (FEATURE_ACL == FYES)
    MIB_PROF_ACL,
    MIB_PROF_ACL_STATUS,
#endif
#if (STATIC_ROUTE == FYES)
    MIB_PROF_STATIC_ROUTE,
#endif

#if (STATIC_NAT == FYES)
    MIB_PROF_STATIC_NAT,
#endif
#if (NAT_RULE == FYES)
    MIB_PROF_NAT_RULE,
#endif
#if (DMZ == FYES)
    MIB_PROF_DMZ_NAT,
#endif
#if (ADMIN_CONF == FYES)
    MIB_PROF_ADMIN_CONF,
#endif
#if (DNS_FILTER == FYES)
    MIB_PROF_DNS_FILTER,
#endif

#if (CLI_FOR_ENGINEER_ONLY == FYES)
    MIB_PROF_CLI_FOR_ENGINEER_ONLY,
#endif

#if (FEATURE_NTP == FYES)
    MIB_PROF_NTP,
#endif

#if (FEATURE_LANG == FYES)
    MIB_PROF_LANG,
#endif

#if (DDNS == FYES)
    MIB_PROF_DDNS,
#endif

#if (TASK_SCHEDULER == FYES)
    MIB_PROF_TASK_SCHED,
#endif

#if (FIREWALL == FYES)
    MIB_PROF_FIRE_WALL,
#endif

#if (PPPOE_SERVER == FYES)
    MIB_PROF_PPPOE_SRV_GLO,
    MIB_PROF_PPPOE_SRV_ACC,
#endif

#if (PPTP_CLIENT == FYES)
    MIB_PROF_PPTP_CLI,
#endif

#if (WIRELESS == FYES)
    MIB_PROF_WIRELESS,
    MIB_PROF_WIRELESS_MAC_FILTER_GLOBAL,
    MIB_PROF_WIRELESS_MAC_FILTER,
#if (WIRELESS_5G == FYES)
    MIB_PROF_WIRELESS_5G_MAC_FILTER,
#endif
#endif

#if (NOTICE == FYES)
    MIB_PROF_NOTICE,
#endif

#if (CWMP == FYES)
    MIB_PROF_CWMP,
#endif

#if (WEB_AUTH == FYES)
    MIB_PROF_WEB_AUTH_GLOBAL,
    MIB_PROF_WEB_AUTH,
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
    MIB_PROF_WEB_AUTH_CPT,
#endif
#endif
#if (WIFIDOG_AUTH == FYES)
#if (WHITELIST == 1)
    MIB_PROF_WHITE_LIST,
#endif

#endif
#if (EXCEPT_QQ == FYES)
    MIB_PROF_EXCEPT_QQ,
#endif

#if (EXCEPT_MSN == FYES)
    MIB_PROF_EXCEPT_MSN,
#endif

#if (EXCEPT_ALI == FYES)
    MIB_PROF_EXCEPT_ALI,
#endif

#if (LOG_MANAGE == FYES)
    MIB_PROF_LOG_MANAGE,
#endif
#if (PVLAN_REDUCED == FYES)
    MIB_PROF_RDD_PORT_VLAN,
#endif

#if (IP_SEC == FYES)
    MIB_PROF_IPSEC,
    MIB_PROF_TRANSFORMSET,
    MIB_PROF_ISAKMP,
    MIB_PROF_ISAKMP_POLICY,
#endif
#if (SYSLOG == FYES)
    MIB_PROF_SYSLOG,
#endif
#if (UTT_LICENSE == FYES)
    MIB_PROF_LICENSE,
#endif

#if (FAT_FIT_SWITCH == FYES)
    MIB_PROF_FAT_FIT_SWITCH,
#endif

#if (FEATURE_AP_MAIL == FYES)
    MIB_PROF_AP_MAIL,
#endif

#if (FEATURE_AP_CONF_TEMP == FYES)
    MIB_PROF_AP_CONF_TEMP,
#endif

#if (AP_PERMISSION == FYES)
	MIB_PROF_AP_PERMISSION,
#endif

#if (FEATURE_AP_SCAN == FYES)
    MIB_PROF_AP_SCAN,
#endif

#if (SERVICE_ZONE == FYES)
	MIB_PROF_SERVICE_ZONE,
#endif

#if ((FEATURE_AP_MAC_FILTER == FYES)||(FIT_AP_MAC_FILTER == FYES))
    MIB_PROF_AP_MAC_FILTER,
#endif

#if (DHCP_POOL == FYES)
    MIB_PROF_DHCP_POOL,
#endif
#if (VLAN_CONFIG == FYES)
    MIB_PROF_VLAN_CONFIG,
#endif

#if (DNS_AGENCY == FYES)
	MIB_PROF_DNS_AGENCY,
#endif

#if (FEATURE_AP_SW_MNG == FYES)
	MIB_PROF_AP_SW_MNG,
#endif

#if (AP_CONF == FYES)
	MIB_PROF_AP_CONF,
#endif

#if (FEATURE_AP_BASIC_INFO == FYES)
    MIB_PROF_AP_BASIC_CONF,
#endif

#if (FEATURE_AP_EFFECT_CONF == FYES)
    MIB_PROF_AP_EFFECT_CONF,
#endif

#if (FIT_AP_SERVICE_ZONE == FYES)
    MIB_PROF_FIT_AP_SZ,
#if (WIRELESS_5G == FYES)
    MIB_PROF_FIT_AP_SZ_5G,
#endif
#endif


#if (AP_BLACK_WHITE_LIST == FYES)
    MIB_PROF_AP_BLACK_WHITE_LIST,
#endif

#if (POLICY_ROUTE == FYES)
    MIB_PROF_POLICY_ROUTE,
#endif
#if (PARENTS_CONTROL == FYES)
    MIB_PROF_PARENTS_CONTROL,
#endif
#if (P2P_LIMIT == FYES)
    MIB_PROF_P2P_LIMIT,
#endif

#if (SYS_LOG_INFO == FYES)
    MIB_PROF_SYS_LOG_INFO,
#endif

#if (EASY_TASK == FYES)
    MIB_PROF_EASY_TASK,
#endif

#if (HTTP_CACHE == FYES)
	MIB_PROF_CACHE,
#endif

#if (NET_MODE_SWITCH == FYES)
	MIB_PROF_NET_MODE_SWITCH,
#endif

#if (BRIDGE == FYES)
	MIB_PROF_BRIDGE,
#endif

#if (CACHE_SERVER == FYES)
	MIB_PROF_CACHE_SERVER,
#endif

#if (NET_SNIPER == FYES)
	MIB_PROF_NET_SNIPER,
#endif

#if (MAC_FILTER == FYES)
    MIB_PROF_MAC_FILTER_GLOBAL,
    MIB_PROF_MAC_FILTER,
#endif

#if (FEATURE_SW_SNMP == FYES)
    MIB_PROF_SNMP,
#endif
#if (SNMP == FYES)
    MIB_PROF_SNMP_INFO,
#endif

#if (AC_WORK_MODE == FYES)
    MIB_PROF_AC_WORK_MODE,
#endif

#if (AP_LOAD_BALANCE == FYES)
    MIB_PROF_AP_LOAD_BALANCE,
#endif

#if (AP_LOG_MANAGE == FYES)
    MIB_PROF_AP_LOG_MANAGE,
#endif

#if (TAG_VLAN == FYES)
    MIB_PROF_TAG_VLAN,
#if (WAN_TAG_VLAN == FYES)
    MIB_PROF_WAN_TAG_VLAN,
#endif


#endif

#if (UTT_SMART_UI == FYES)
#if (APP_MENU == FYES)
    MIB_PROF_SHORTCUT_MENU,
#endif
#endif

#if (NET_SHARE_MANAGE == FYES)
    MIB_PROF_NET_SHARE_MANAGE,
#endif

#if (FTP_SERVER == FYES)
    MIB_PROF_FTP_SERVER,
    MIB_PROF_FTP_SHARE_DIR,
#endif

#if(NET_SHARE_USER == FYES)
    MIB_PROF_NET_SHARE_USER,
#endif

#if(ADFILTER == FYES)
    MIB_PROF_ADFILTER,
#endif

#if(MAKE_MONEY == FYES)
    MIB_PROF_MAKE_MONEY,
#endif

#if(UTT_KNOWIFI == FYES)
    MIB_PROF_KNOWIFI_DOMAIN,
#endif

#if(CAPWAP_CONFIG_AP == FYES)
    MIB_PROF_AP_CAPWAP_CONFIG,
#endif
#if (CLOUD_MANAGE == FYES)
    MIB_PROF_CLOUD_MANAGE,
#endif

#if (CLOUD_CONFIG == FYES)
    MIB_PROF_CLOUD_CONFIG,
#endif
    MIB_PROF_MAX
}MibProfileType;

typedef union {
    SystemProfile           systemProfile;
    InterfaceProfile        interfaceProfile;
    UserProfile             userProfile;
#if (IPV6_INTERFACE == FYES)
    Ipv6InterfaceProfile    ipv6interfaceProfile;
#endif
#if(IPV6_ROUTE == FYES)
   Ipv6RoutConfProfile     ipv6RoutConfProfile;
#endif
#if (IPV6_RADVD == FYES)
    RadvdProfile            radvdProfile;
#endif
#if(IPV6_NOTLISTPREFIXS == FYES)
    Ipv6PrefixListProfile  ipv6PrefixListProfile;
#endif
#if (IPV6_DHCPV6 == FYES)
    Dhcp6Profile            dhcp6Profile;
#endif
#if (IPV6_TUNNEL == FYES)
    TunnelProfile            tunnelProfile;
#endif
#if (IPV6_ISATAP == FYES)
    IsatapProfile            isatapProfile;
#endif

#if (IP_GRP == FYES)	
    IpGrpProfile            ipGrpProfile;          
#endif
#if (PORT_GRP == FYES)	
    PortGrpProfile          portGrpProfile;          
#endif
#if (TIME_GRP == FYES)	
    TimeGrpProfile           timeGrpProfile;          
#endif
#if (DHCP_SERVER == FYES)
    DhcpServerProfile       dhcpServerProfile;
#endif
#if (DHCP_POOL == FYES)
    DhcpPoolProfile       dhcpPoolProfile;
#endif
#if (VLAN_CONFIG == FYES)
    VlanConfigProfile       vlanConfigProfile;
#endif
#if (DNS_AGENCY == FYES)
	DnsAgencyProfile dnsAgencyProfile;
#endif
#if (FEATURE_SW_PORT_CONF == FYES)
    SwPortConfProfile       swPortConfProfile;
#endif
#if (FEATURE_PORT_VLAN == FYES)
    PortVlanProfile         portVlanProfile;
#endif
#if (PVLAN_REDUCED == FYES)
    RddPortVlanProfile         rddPortVlanProfile;
#endif
#if (L2TP_SERVER == FYES)
    L2TPSrvGlobalProfile    l2tpSrvGlobalProfile;
    L2TPSrvAccProfile	    l2tpSrvAccProfile;
#endif
#if (L2TP_CLIENT == FYES)
    L2TPCliProfile          l2tpCliProfile;
#endif
#if (GROUP_BASE == FYES)
    GroupBaseProfile	    groupBaseProfile;
#endif
#if (FEATURE_SW_PORT_MIRROR == FYES)
	MirrorProfile	mirrorProfile;
#endif
#if (FEATURE_SW_PORT_TRUNK == FYES)
	TrunkProfile	trunkProfile;
#endif

#if (FEATURE_ACL == FYES)
    AclProfile     aclProfile;
    AclStatusProfile     aclStatusProfile;
#endif

#if (PPTP_SERVER == FYES)
    PPTPSrvGlobalProfile    pptpSrvGlobalProfile;
    PPTPSrvAccProfile	    pptpSrvAccProfile;
#endif

#if (STATIC_ROUTE == FYES)
    StaticRouteProfile	    staticRouteProfile;
#endif

#if (DDNS == FYES)
    DDNSProfile	    ddnsProfile;
#endif

#if (PPPOE_SERVER == FYES)
    PPPoESrvGloProfile pppoeSrvGloProfile;
    PPPoESrvAccProfile pppoeSrvAccProfile;
#if 1
    /* 自建实例名 */
    WebInstRec	pppoeInstRec;
#endif
#endif

#if (PPTP_CLIENT == FYES)
    PPTPCliProfile pptpCliProfile;
#endif

#if (UTTPDB == FYES)
    PdbProfile pdbProfile;
#endif

#if (STATIC_NAT == FYES)
   StaticNatProfile staticNatProfile;
#endif
#if (NAT_RULE == FYES)
    NatRuleProfile  natRuleProfile;
#endif
#if (DMZ == FYES)
    DMZProfile dmzProfile;
#endif
#if (ADMIN_CONF == FYES)
    AdminConfProfile	adminConfProfile;
#endif
#if (DNS_FILTER == FYES)
    DnsFilterProfile	dnsFilterProfile;
#endif

#if (CLI_FOR_ENGINEER_ONLY == FYES)
    CliForEngineerOnlyProfile cliForEngineerOnlyProfile;
#endif

#if (WIRELESS == FYES)
    WirelessProfile		wirelessProfile;
    WirelessMacFilterProfile	wirelessMacFilterProfile;
#if (WIRELESS_5G == FYES)
    Wireless5gMacFilterProfile	wireless5gMacFilterProfile;
#endif
#endif
#if (FEATURE_NTP == FYES)
    NtpProfile  ntpProfile;
#endif
#if (FEATURE_LANG == FYES)
    LangProfile  langProfile;
#endif
#if (TASK_SCHEDULER == FYES)
    TaskSchedProfile	taskSchedProfile;
#endif
#if (FIREWALL == FYES)
    FireWallProfile fireWallProfile;
#endif
#if (NOTICE == FYES)
    NoticeProfile noticeProfile;
#endif
#if (CWMP == FYES)
    CwmpProfile cwmpProfile;
#endif
#if (WEB_AUTH == FYES)
    WebAuthProfile webAuthProfile;
    WebAuthGlobalProfile webAuthGlobalProfile;
    /* 自建实例名 */
    WebInstRec	webauthInstRec;
#endif
#if (WIFIDOG_AUTH == FYES)
#if (WHITELIST == 1)
    WhiteListProfile whiteListProfile;
#endif
#endif
#if (EXCEPT_QQ == FYES)
    ExceptQQProfile exceptQQProfile;
    /* 自建实例名 */
    WebInstRec	exceptqqInstRec;
#endif
#if (EXCEPT_MSN == FYES)
    ExceptMSNProfile exceptMSNProfile;
    /* 自建实例名 */
    WebInstRec	exceptmsnInstRec;
#endif
#if (EXCEPT_ALI == FYES)
    ExceptAliProfile exceptAliProfile;
    /* 自建实例名 */
    WebInstRec	exceptaliInstRec;
#endif
#if (LOG_MANAGE == FYES)
    LogManageProfile logManageProfile;
#endif

#if (AP_LOG_MANAGE == FYES)
    ApLogManageProfile apLogManageProfile;
#endif

#if (IP_SEC == FYES)
    ipsecConfProfile ipsecconfProfile;
    WebInstRec	ipsecInstRec;
    ipsecTransformSetProfile ipsectransformsetProfile;
    isakmpConfProfile isakmpconfProfile;
    isakmpPolicyProfile isakmppolicyProfile;
#endif
#if (SYSLOG == FYES)
    SyslogProfile syslogProfile;
#endif
#if (FAT_FIT_SWITCH == FYES)
    FatFitSwitchProfile    fatFitSwitchProfile;
#endif
#if (AC_WORK_MODE == FYES)
    AcWorkModeProfile  acWorkModeProfile;
#endif
#if (AP_LOAD_BALANCE == FYES)
    ApLoadBalanceProfile apLoadBalanceProfile;
#endif
#if (AP_PERMISSION == FYES)
	ApPermissionProfile apPermissionProfile;
#endif
#if (AP_CONF == FYES)
	ApConfProfile apConfProfile;
#endif
#if (SERVICE_ZONE == FYES)
	ServiceZoneProfile serviceZoneProfile;
#endif
#if ((FEATURE_AP_MAC_FILTER == FYES)||(FIT_AP_MAC_FILTER == FYES))
	ApMacFilterProfile apMacFilterProfile;
#endif
#if (TAG_VLAN == FYES)
	TagVlanProfile tagVlanProfile;
#if (WAN_TAG_VLAN == FYES)
	WanTagVlanProfile wanTagVlanProfile;
#endif
#endif

#if (FEATURE_AP_MAIL == FYES)
    ApMailProfile	apMailProfile;
#endif

#if (NET_SHARE_MANAGE == FYES)
    NetShareManageProfile netShareManageProfile;
#endif
#if (FTP_SERVER == FYES)
    FtpServerProfile ftpServerProfile;
    FtpShareDirProfile ftpShareDirProfile;
#endif
#if (NET_SHARE_USER == FYES)
    NetShareUserProfile netShareUserProfile;
#endif
#if(CAPWAP_CONFIG_AP == FYES)
    ApCapwapConfigProfile apCapwapConfigProfile;
#endif
#if (FEATURE_AP_CONF_TEMP == FYES)
    ApConfTempProfile	apConfTempProfile;
#endif
#if (FEATURE_AP_SCAN == FYES)
    ApScanProfile  apScanProfile;
#endif

#if (FEATURE_AP_SW_MNG == FYES)
    ApSwMngProfile apSwMngProfile;
#endif

#if (FEATURE_AP_BASIC_INFO == FYES)
    ApBasicConfProfile  apBasicConfProfile;
#endif

#if (FEATURE_AP_EFFECT_CONF == FYES)
    ApEffectConfProfile  apEffectConfProfile;
#endif

#if (FIT_AP_SERVICE_ZONE == FYES)
    FitApSzProfile  fitApSzProfile;
#if (WIRELESS_5G == FYES)
    FitAp5gSzProfile  fitAp5gSzProfile;
#endif
#endif


#if (AP_BLACK_WHITE_LIST == FYES)
    ApBlackWhiteListProfile apBlackWhiteListProfile;
#endif

#if (UTT_LICENSE == FYES)
    LicenseProfile licenseProfile;
#endif
#if (POLICY_ROUTE == FYES)
    PolicyRouteProfile policyrouteProfile;
#endif
#if (PARENTS_CONTROL == FYES)
    ParentsControlProfile parentscontrolProfile;
#endif
#if (P2P_LIMIT == FYES)
    P2PLimitProfile p2pLimitProfile;
#endif
#if (SYS_LOG_INFO == FYES)
    SysLogInfoProfile sysLogInfoProfile;
#endif
#if (EASY_TASK == FYES)
    EasyTaskProfile easyTaskProfile;
#endif
#if (HTTP_CACHE == FYES)
	CacheProfile cacheProfile;
#endif

#if (NET_MODE_SWITCH == FYES)
	NetModeSwitchProfile netModeSwitchProfile;
#endif
#if (BRIDGE == FYES)
	BridgeProfile bridgeProfile;
#endif
#if (CACHE_SERVER == FYES)
	CacheServerProfile cacheServerProfile;
#endif
#if (NET_SNIPER == FYES)
	NetSniperProfile netSniperProfile;
#endif
#if (MAC_FILTER == FYES)
    MacFilterGlobalProfile macFilterGlobalProfile;
    MacFilterProfile macFilterProfile;
#endif
#if (FEATURE_SW_SNMP == FYES)
    SnmpProfile snmpProfile;
#endif
#if (SNMP == FYES)
    SnmpInfoProfile snmpInfoProfile;
#endif
#if (UTT_SMART_UI == FYES)
#if (APP_MENU == FYES)
    ShortCutMenuProfile shortCutMenuProfile;
#endif
#endif
#if (ADFILTER == FYES)
    AdfilterProfile adfilterProfile;
#endif
#if(MAKE_MONEY == FYES)
    MakeMoneyProfile makeMoneyProfile;
#endif
#if (CLOUD_MANAGE == FYES)
    CloudManageProfile cloudManageProfile;
#endif
#if (CLOUD_CONFIG == FYES)
    CloudConfigProfile cloudConfigProfile;
#endif
#if (UTT_KNOWIFI == FYES)
    KnowifiDomainProfile knowifiDomainProfile;
#endif
}ProfileUnion;

typedef struct st_nvram {
    SystemProfile           systemProfile;          
    InterfaceProfile        interfaceProfile[MAX_INTERFACE_PROFILES];
    UserProfile             userProfile[MAX_USER_PROFILES];
#if (IPV6_INTERFACE == FYES)
    Ipv6InterfaceProfile    ipv6interfaceProfile[MAX_IPV6_INTERFACE_PROFILES];
#endif

#if (IPV6_RADVD == FYES)
    RadvdProfile            radvdProfile[MAX_IPV6_RADVD_PROFILES];
#endif
#if(IPV6_NOTLISTPREFIXS == FYES)
    Ipv6PrefixListProfile      ipv6PrefixListProfile[MAX_IPV6_NOTLISTPREFIXS_PROFILES];
#endif
#if (IPV6_DHCPV6 == FYES)
    Dhcp6Profile            dhcp6Profile[MAX_IPV6_DHCPV6_PROFILES];
#endif

#if(IPV6_ROUTE ==FYES)
    Ipv6RoutConfProfile     ipv6RoutConfProfile[MAX_IPV6_ROUTE_PROFILES];
#endif
#if(IPV6_TUNNEL ==FYES)
    TunnelProfile           tunnelProfile;
#endif
#if(IPV6_ISATAP ==FYES)
    IsatapProfile           isatapProfile;
#endif

#if (IP_GRP == FYES)
    IpGrpProfile            ipGrpProfile[MAX_IPGRP_PROFILES];          
#endif
#if (PORT_GRP == FYES)
    PortGrpProfile          portGrpProfile[MAX_PORTGRP_PROFILES];          
#endif
#if (TIME_GRP == FYES)
    TimeGrpProfile          timeGrpProfile[MAX_TIMEGRP_PROFILES];          
#endif
#if (DHCP_SERVER == FYES)  
    DhcpServerProfile       dhcpServerProfile[MAX_DHCP_SERVER_PROFILES];
#endif
#if (DHCP_POOL == FYES)  
    DhcpPoolProfile       dhcpPoolProfile[MAX_DHCP_POOL_PROFILES];
#endif
#if (VLAN_CONFIG == FYES)  
    VlanConfigProfile       vlanConfigProfile[MAX_VLAN_CONFIG_PROFILES];
#endif
#if (DNS_AGENCY == FYES)
	DnsAgencyProfile       dnsAgencyProfile[MAX_DNS_AGENCY_PROFILES];
#endif

#if (FEATURE_SW_PORT_CONF == FYES)
    SwPortConfProfile       swPortConfProfile[MAX_SW_PORT_CONF_PROFILES];
#endif
#if (FEATURE_PORT_VLAN == FYES)
    PortVlanProfile         portVlanProfile[MAX_PORT_VLAN_PROFILES]; 
#endif
#if (PVLAN_REDUCED == FYES)
    RddPortVlanProfile      rddPortVlanProfile[MAX_LAN_PORT_COUNT];
#endif
#if (L2TP_SERVER == FYES)
    L2TPSrvGlobalProfile    l2tpSrvGlobalProfile;
#if (VPN_ITEM_CONTROL == FYES)
    L2TPSrvAccProfile	    l2tpSrvAccProfile[MAX_VPN_ITEMS_NUM];
#else
    L2TPSrvAccProfile	    l2tpSrvAccProfile[MAX_L2TP_SRV_ACCOUNT_PROFILES];
#endif
#endif
#if (L2TP_CLIENT == FYES)
#if (VPN_ITEM_CONTROL == FYES)
    L2TPCliProfile          l2tpCliProfile[MAX_VPN_ITEMS_NUM];
#else
    L2TPCliProfile          l2tpCliProfile[MAX_L2TP_CLIENT_PROFILES];
#endif
#endif
#if (GROUP_BASE == FYES)
    GroupBaseProfile	    groupBaseProfile[MAX_GROUP_BASE_PROFILES];
#endif
#if (FEATURE_SW_PORT_MIRROR == FYES)
	MirrorProfile   mirrorProfile;
#endif
#if (FEATURE_SW_PORT_TRUNK == FYES)
	TrunkProfile    		trunkProfile[MAX_TRUNK_PROFILES];
#endif

#if (FEATURE_ACL == FYES)
    AclProfile   aclProfile[MAX_ACL_PROFILES];
    AclStatusProfile     aclStatusProfile;
#endif

#if (PPTP_SERVER == FYES)
    PPTPSrvGlobalProfile    pptpSrvGlobalProfile;
#if (VPN_ITEM_CONTROL == FYES)
    PPTPSrvAccProfile	    pptpSrvAccProfile[MAX_VPN_ITEMS_NUM];
#else
    PPTPSrvAccProfile	    pptpSrvAccProfile[MAX_PPTP_SRV_ACCOUNT_PROFILES];
#endif
#endif

#if (PPTP_CLIENT == FYES)
#if (VPN_ITEM_CONTROL == FYES)
    PPTPCliProfile	    pptpCliProfile[MAX_VPN_ITEMS_NUM];
#else
    PPTPCliProfile	    pptpCliProfile[MAX_PPTP_CLI_PROFILES];
#endif
#endif

#if (UTTPDB == FYES)
    PdbProfile pdbProfile[MAX_PDB_PROFILES];
#endif
#if (STATIC_NAT == FYES)
   StaticNatProfile        staticNatProfile[MAX_STATIC_NAT_PROFILES];
#endif

#if (STATIC_ROUTE == FYES)
    StaticRouteProfile	    staticRouteProfile[MAX_STATIC_ROUTE_PROFILES];
#endif

#if (DDNS == FYES)
#if (DDNS_ADVANCE == FYES)
    DDNSProfile		    ddnsProfile[MAX_DDNS_PROFILES];
#else
    DDNSProfile		    ddnsProfile;
#endif
#endif

#if (PPPOE_SERVER == FYES)
    PPPoESrvGloProfile	    pppoeSrvGloProfile;
    PPPoESrvAccProfile	    pppoeSrvAccProfile[MAX_PPPOE_SRV_ACC_PROFILES];
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
    SessionList         Session[MAX_PPPOE_SRV_ACC_PROFILES];
#else
    SessionList		    Session[MAX_PPPOE_SRV_SESSION_PROFILES];
#endif

#if 1
    /* 自建实例名 */
    WebInstRec	pppoeInstRec;
#endif
#endif

#if (NAT_RULE == FYES)
    NatRuleProfile  natRuleProfile[MAX_NAT_RULE_PROFILES];
#endif
#if (DMZ  == FYES)
    DMZProfile	    dmzProfile;
#endif
#if (ADMIN_CONF == FYES)  
	AdminConfProfile	adminConfProfile[MAX_ADMIN_CONF_PROFILES];
#endif
#if (DNS_FILTER == FYES)  
	DnsFilterProfile	dnsFilterProfile[MAX_DNS_FILTER_PROFILES];
#endif

#if (CLI_FOR_ENGINEER_ONLY == FYES)
    CliForEngineerOnlyProfile cliForEngineerOnlyProfile[MAX_CLI_FOR_ENGINEER_ONLY_PROFILES];
#endif

#if (WIRELESS == FYES)  
    WirelessProfile		wirelessProfile[MAX_WIRELESS_PROFILES];
    WirelessMacFilterProfile	wirelessMacFilterProfile[MAX_WIRELESS_MAC_FILTER_PROFILES];
#if (WIRELESS_5G == FYES)  
    Wireless5gMacFilterProfile	wireless5gMacFilterProfile[MAX_WIRELESS_5G_MAC_FILTER_PROFILES];
#endif
#endif
#if (FEATURE_NTP == FYES)
    NtpProfile  ntpProfile;
#endif
#if (FEATURE_LANG == FYES)
    LangProfile  langProfile;
#endif
#if (TASK_SCHEDULER == FYES)
	TaskSchedProfile	taskSchedProfile[MAX_TASK_SCHED_PROFILES];
#endif
#if (FIREWALL == FYES)
	FireWallProfile fireWallProfile[MAX_FIREWALL_FILTER_PROFILES];
#endif
#if (NOTICE == FYES)
	NoticeProfile noticeProfile[MAX_NOTICE_PROFILES];
#endif
#if (CWMP == FYES)
	CwmpProfile cwmpProfile;
#endif
#if (WEB_AUTH == FYES)
	WebAuthProfile webAuthProfile[MAX_WEB_AUTH_PROFILES];
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
	WebAuthProfile webAuthProfile_auto[MAX_WEB_AUTH_PROFILES];
#endif
	WebAuthGlobalProfile webAuthGlobalProfile[MAX_WEB_AUTH_GLOBAL_PROFILES];
	/* 自建实例名 */
	WebInstRec	webauthInstRec;
#if (WEB_AUTH_SHARE == FYES)
	WebAuthSession session[MAX_WEB_AUTH_SESSION];
#endif
#if (WIFIDOG_AUTH == FYES)
#if (SMS_ENABLE ==1 )
	smsList smslist;
#endif
#endif
#endif
#if (WIFIDOG_AUTH == FYES)
#if (WHITELIST == 1)
	WhiteListProfile whiteListProfile[MAX_WHITE_LIST];
#endif
#endif
#if (EXCEPT_QQ == FYES)
	ExceptQQProfile exceptQQProfile[MAX_EXCEPT_QQ_PROFILES];
	/* 自建实例名 */
	WebInstRec	exceptqqInstRec;
#endif
#if (EXCEPT_MSN == FYES)
	ExceptMSNProfile exceptMSNProfile[MAX_EXCEPT_MSN_PROFILES];
	/* 自建实例名 */
	WebInstRec	exceptmsnInstRec;
#endif
#if (EXCEPT_ALI == FYES)
	ExceptAliProfile exceptAliProfile[MAX_EXCEPT_ALI_PROFILES];
	/* 自建实例名 */
	WebInstRec	exceptaliInstRec;
#endif
#if (LOG_MANAGE == FYES)
	LogManageProfile logManageProfile[MAX_LOG_MANAGE_PROFILES];
#endif
#if (AP_LOG_MANAGE == FYES)
	ApLogManageProfile apLogManageProfile[MAX_AP_LOG_MANAGE_PROFILES];
#endif
#if (IP_SEC == FYES)
#if (VPN_ITEM_CONTROL == FYES)
    ipsecConfProfile ipsecconfProfile[MAX_VPN_ITEMS_NUM];
    WebInstRec	ipsecInstRec;
    ipsecTransformSetProfile ipsectransformsetProfile[MAX_TRANSFORMSET_PROFILES];
    isakmpConfProfile isakmpconfProfile[MAX_VPN_ITEMS_NUM];
    isakmpPolicyProfile isakmppolicyProfile[MAX_ISAKMP_POLICY_PROFILES];
#else
    ipsecConfProfile ipsecconfProfile[MAX_IPSEC_PROFILES];
    WebInstRec	ipsecInstRec;
    ipsecTransformSetProfile ipsectransformsetProfile[MAX_TRANSFORMSET_PROFILES];
    isakmpConfProfile isakmpconfProfile[MAX_ISAKMP_PROFILES];
    isakmpPolicyProfile isakmppolicyProfile[MAX_ISAKMP_POLICY_PROFILES];
#endif
#if (VPN_CONTROL == FYES)
#if (VPN_ITEM_CONTROL == FYES)
    saStatus saState[MAX_VPN_ITEMS_NUM];
#else
    saStatus saState[MAX_IPSEC_PROFILES];
#endif
#endif /*VPN_CONTROL*/
#endif
#if (SYSLOG == FYES)
    SyslogProfile syslogProfile;
#endif
#if (FAT_FIT_SWITCH == FYES)  
	FatFitSwitchProfile	fatFitSwitchProfile[MAX_FAT_FIT_SWITCH_PROFILES];
#endif
#if (AC_WORK_MODE == FYES)  
	AcWorkModeProfile acWorkModeProfile;
#endif
#if (AP_LOAD_BALANCE == FYES)
    ApLoadBalanceProfile apLoadBalanceProfile[MAX_AP_LOAD_BL_PROFILES];
#endif
#if (AP_PERMISSION == FYES)
	ApPermissionProfile apPermissionProfile[MAX_AP_PERMISSION_PROFILES];
#endif
#if (AP_CONF == FYES)
	ApConfProfile apConfProfile;
#endif
#if (SERVICE_ZONE == FYES)
	ServiceZoneProfile serviceZoneProfile[MAX_SERVICE_ZONE_PROFILES];
#endif
#if ((FEATURE_AP_MAC_FILTER == FYES)||(FIT_AP_MAC_FILTER == FYES))
	ApMacFilterProfile apMacFilterProfile[MAX_AP_MAC_FILTER_PROFILES];
#endif
#if (TAG_VLAN == FYES)
	TagVlanProfile tagVlanProfile[MAX_TAG_VLAN_PROFILES];
#if (WAN_TAG_VLAN == FYES)
	WanTagVlanProfile wanTagVlanProfile;
#endif
#endif

#if (FEATURE_AP_MAIL == FYES)
	ApMailProfile	apMailProfile[MAX_AP_MAIL_PROFILES];
#endif

#if (NET_SHARE_MANAGE == FYES)
    NetShareManageProfile netShareManageProfile;
#endif
#if (FTP_SERVER == FYES)
    FtpServerProfile ftpServerProfile;
    FtpShareDirProfile ftpShareDirProfile[MAX_FTP_SHARE_DIR];
#endif
#if (NET_SHARE_USER == FYES)
    NetShareUserProfile netShareUserProfile[MAX_NET_SHARE_USER];
#endif
#if(CAPWAP_CONFIG_AP == FYES)
    ApCapwapConfigProfile apCapwapConfigProfile[MAX_AP_CAPWAP_CONFIG];
#endif

#if (FEATURE_AP_CONF_TEMP == FYES)
    ApConfTempProfile	apConfTempProfile[MAX_AP_CONF_TEMP_PROFILES];
#endif
#if (FEATURE_AP_SCAN == FYES)
    ApScanProfile  apScanProfile;
#endif
#if (FEATURE_AP_BASIC_INFO == FYES)
    ApBasicConfProfile  apBasicConfProfile[MAX_AP_BASIC_CONF_PROFILES];
#endif

#if (FEATURE_AP_SW_MNG == FYES)
    ApSwMngProfile	apSwMngProfile[MAX_AP_SW_MNG_PROFILES];
#endif

#if (FEATURE_AP_EFFECT_CONF == FYES)
    ApEffectConfProfile  apEffectConfProfile[MAX_AP_EFFECT_CONF_PROFILES];
#endif

#if (FIT_AP_SERVICE_ZONE == FYES)
    FitApSzProfile  fitApSzProfile[MAX_FIT_AP_SERVICE_ZONE_PROFILES];
#if (WIRELESS_5G == FYES)
    FitAp5gSzProfile  fitAp5gSzProfile[MAX_FIT_AP_SERVICE_ZONE_PROFILES];
#endif
#endif

#if (AP_BLACK_WHITE_LIST == FYES)
    ApBlackWhiteListProfile apBlackWhiteListProfile[MAX_AP_BW_LIST_PROFILES];
#endif

#if (UTT_LICENSE == FYES)
    LicenseProfile licenseProfile[MAX_LICENSE_PROFILES];
#endif
#ifdef CONFIG_UTT_DEBUG
    DebugProfile  uttDebug; 
#endif
#if (POLICY_ROUTE == FYES)
    PolicyRouteProfile policyrouteProfile[MAX_POLICY_ROUTE_PROFILES];
#endif
#if (PARENTS_CONTROL == FYES)
    ParentsControlProfile parentscontrolProfile[MAX_PARENTS_CONTROL_PROFILES];
#endif
#if (P2P_LIMIT == FYES)
    P2PLimitProfile p2pLimitProfile;
#endif
#if (SYS_LOG_INFO == FYES)
    SysLogInfoProfile sysLogInfoProfile;
#endif
#if (EASY_TASK == FYES)
    EasyTaskProfile easyTaskProfile;
#endif
#if (HTTP_CACHE ==FYES)
	 CacheProfile cacheProfile[MAX_CACHE_PROFILES];
#endif

#if (NET_MODE_SWITCH ==FYES)
	NetModeSwitchProfile netModeSwitchProfile[MAX_NET_MODE_PROFILES];
#endif
#if (BRIDGE ==FYES)
	BridgeProfile bridgeProfile[MAX_BRIDGE_PROFILES];
#endif
#if (CACHE_SERVER == FYES)
	CacheServerProfile cacheServerProfile[MAX_CACHE_SERVER_PROFILES];
#endif
#if (NET_SNIPER == FYES)
	NetSniperProfile netSniperProfile;
#endif

#if (MAC_FILTER == FYES)
    MacFilterGlobalProfile macFilterGlobalProfile;
    MacFilterProfile macFilterProfile[MAX_MAC_FILTER_PROFILES];
#endif
#if (FEATURE_SW_SNMP == FYES)
    SnmpProfile snmpProfile;
#endif
#if (SNMP == FYES)
    SnmpInfoProfile snmpInfoProfile;
#endif
#if (VPN_CONTROL == FYES)
    VpnProfile vpnProfile;
#endif

#if (UTT_SMART_UI == FYES)
#if (APP_MENU == FYES)
    ShortCutMenuProfile shortCutMenuProfile[MAX_SHORTCUT_MENU_PROFILES];
#endif
#endif

#if (ADFILTER == FYES)
    AdfilterProfile adfilterProfile;
#endif
#if(MAKE_MONEY == FYES)
    MakeMoneyProfile makeMoneyProfile;
    
    FreeExp freeExpList[MAX_FREE_EXP_LIST];
    int count;
#endif
#if (CLOUD_MANAGE == FYES)
    CloudManageProfile cloudManageProfile;
#endif
#if (CLOUD_CONFIG == FYES)
    CloudConfigProfile cloudConfigProfile;
    NoticeMac noticeMacList[MAX_NOTICE_MAC_LIST] ;/*需要提醒的*/   
    unsigned int needSendCount;/*noticeMacList需要发给云的数量*/
    unsigned int noticemac_list_count;
#endif
#if (UTT_KNOWIFI == FYES)
    KnowifiDomainProfile knowifiDomainProfile[MAX_KNOWIFI_DOMAIN_COUNT];
#endif
}NvramProfiles;

#endif
