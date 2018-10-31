/*
 * this file for enum to int
 */
#include "uttMachine.h"
#include "typedef.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "conver.h"
#include "mxml.h"
#include "uttMachine.h"
#ifdef CONFIG_SWITCH_EXT
#include <linux/switch/swioc_common.h>
#endif

const LcdParamEnumValue enDisableEnum[] = {
	{"Enable", FUN_ENABLE},
	{"Disable", FUN_DISABLE},
	{NULL, 0}
};

#if (DHCP_AUTO_BIND == FYES)
/*
 *DHCP°ó¶¨·½Ê½
 */
const LcdParamEnumValue endhcpBindType[] = {
	{"manualBind", MANUAL_BIND},
	{"autoBind", AUTO_BIND},
	{NULL, 0}
};
#endif
const LcdParamEnumValue yesnoEnum[] = {
	{"Yes", PROF_ENABLE},
	{"No", PROF_DISABLE},
	{NULL, 0}
};

const LcdParamEnumValue ifBalanceModeEnum[] = {
	{"main", IF_MIAN_LINE},
	{"backup", IF_BACKUP_LINE},
	{NULL, 0}
};

const LcdParamEnumValue lineGroupModeEnum[] = {
	{"balance", GROUP_LINE_BALANCE},
	{"backup", GROUP_LINE_BACKUP},
	{NULL, 0}
};

const LcdParamEnumValue ethConnModeEnum[] = {
	{"DHCP", ETH_CONN_DHCP},
	{"STATIC", ETH_CONN_STATIC},
	{"PPPOE", ETH_CONN_PPPOE},
	{NULL, 0}
};

const LcdParamEnumValue pppConnModeEnum[] = {
	{"KEEPALIVE", PPP_CONN_KEEPLIVE},
	{"DEMAND", PPP_CONN_DEMAND},
	{"MANUAL", PPP_CONN_MANUAL},
	{NULL, 0}
};

const LcdParamEnumValue pppSpDailEnum[] = {
	{"NORMAL", PPP_DAIL_NORMAL},
	{"SP1", PPP_DAIL_SP1},
	{"SP2", PPP_DAIL_SP2},
	{"SP3", PPP_DAIL_SP3},
	{NULL, 0}
};

const LcdParamEnumValue addrObjTypeEnum[] = {
	{"IPRANGE", ADDR_IPRANGE},
#if (IP_GRP == FYES)
	{"IPGRP", ADDR_IPGRP},
#endif
	{NULL, 0}
};

const LcdParamEnumValue fwObjTypeEnum[] = {
	{"FWDIY", FW_DIY},
#if (PORT_GRP == FYES)
	{"FWGRP", FW_GRP},
#endif
	{NULL, 0}
};

const LcdParamEnumValue dnsFilterTypeEnum[] = {
	{"DOMAIN_DIS", DNSFILTER_DOMAIN_DIS},
	{"DOMAIN_EN", DNSFILTER_DOMAIN_EN},
	{NULL, 0}
};

const LcdParamEnumValue pppAuthEnum[] = {
	{"MS-CHAPV2", PPP_AUTH_MS_CHAPV2},
	{"EITHER", PPP_AUTH_ANY},
	{"THRIN", PPP_AUTH_THREE_ANY},
	{"PAP", PPP_AUTH_PAP},
	{"CHAP", PPP_AUTH_CHAP},
	{"MS-CHAP", PPP_AUTH_MS_CHAP},
	{"NONE", PPP_AUTH_NONE},
	{NULL, 0}
};

#if (IP_GRP == FYES)
const LcdParamEnumValue ipGrpTypeEnum[] = {
	{"ip",IPGRP_IP},
	{"user",IPGRP_USER},
	{NULL,0}
};

const LcdParamEnumValue ipGrpEntryTypeEnum[] = {
	{"NONE",IPENT_NONE},
	{"RANGE",IPENT_RANGE},
#if (PPPOE_SERVER == FYES)
	{"POE_ACC",IPENT_POE_ACC},
#endif
#if (WEB_AUTH == FYES)
	{"WEB_AUTH",IPENT_WEB_AUTH},
#endif
	{"GRP",IPENT_GRP},
	{NULL,0}
};
#endif

#if (PORT_GRP == FYES)
const LcdParamEnumValue portGrpTypeEnum[] = {
	{"port",PORTGRP_PORT},
	{"url",PORTGRP_URL},
	{"dns",PORTGRP_DNS},
	{"key",PORTGRP_KEY},
	{"mac",PORTGRP_MAC},
	{NULL,0}
};

const LcdParamEnumValue portGrpEntryTypeEnum[] = {
	{"NONE",PORTENT_NONE},
	{"NORMAL",PORTENT_NORMAL},
	{"TCP",PORTENT_TCP},
	{"UDP",PORTENT_UDP},
	{"OTHER",PORTENT_OTHER},
	{"SYS",PORTENT_SYS},
	{"GRP",PORTENT_GRP},
	{NULL,0}
};
#endif

#if (PPPOE_ACCOUNT == FYES)
const LcdParamEnumValue chargeTypeEnum[] = {
	{"DATE",DATE_CHARG_MODE},
	{"NONE", NONE_CHARG_MODE},
	{NULL, 0}
};
#endif

#if (NOTICE == FYES)
const LcdParamEnumValue webSubmitEnum[] = {
	{"EDIT",NOTICE_WEB_EDIT},
	{"IMPORT",NOTICE_WEB_IMPORT},
	{"NONE", NOTICE_WEB_NONE},
	{NULL, 0}
};
#endif

#if ((L2TP_SERVER == FYES) || (PPTP_SERVER == FYES))
const LcdParamEnumValue vpnPeerTypeEnum[] = {
	{"mobile", VPN_MOBILE},
	{"lantolan", VPN_LANTOLAN},
	{NULL, 0}
};
#endif

#if ((FEATURE_SW_PORT_CONF == FYES) || (PORT_CONF_REDUCED == FYES))
const LcdParamEnumValue  portModeEnum[] = {
    {"10FDX",          PORT_MODE_10FDX}  ,/*10Mbit/s full duplex*/
    {"100FDX",         PORT_MODE_100FDX},/*100Mbit/s full duplex*/
    {"1GFDX",          PORT_MODE_1GFDX},/*1000Mbit/s full duplex*/
    {"autoNegotiate",  PORT_MODE_AUTO},/*auto negotiate*/
    {"10HDX",          PORT_MODE_10HDX},/*10Mbit/s half duplex*/
    {"100HDX",         PORT_MODE_100HDX},/*100Mbit/s half duplex*/
    {"Disabled",       PORT_MODE_DISABLED},/*link disabled*/
    {NULL,             0                 }
};
#endif

#if (STATIC_NAT == FYES)
LcdParamEnumValue nat_protocol[]={
    {"tcp", STATIC_NAT_TCP},
    {"udp", STATIC_NAT_UDP},
    {"", STATIC_NAT_TCP_UDP},
    {NULL, 0},
};

#endif

#if (FEATURE_ACL == FYES)
LcdParamEnumValue acl_actions[MAX_ACL_ACTION+2]={
    {"NONE", ACL_NONE},
    {"forward", ACL_FORWARD},
    {"cpu", ACL_CPU},
    {"drop", ACL_DROP},
    {"queue0", ACL_QUEUE0},
    {"queue1", ACL_QUEUE1},
    {"queue2", ACL_QUEUE2},
    {"queue3", ACL_QUEUE3},
    {"QinQTag1", ACL_QINQ1},
    {"QinQTag2", ACL_QINQ2},
    {"QinQTag3", ACL_QINQ3},
    {"QinQTag4", ACL_QINQ4},
    {NULL, 0},
};
#endif

#if (FIREWALL == FYES)
const LcdParamEnumValue FilterTypeEnum[] = {
	{"IpFilter", TYPE_FILTER_IP},
	{"UrlFilter", TYPE_FILTER_URL},
	{"WordFilter", TYPE_FILTER_WORD},
	{"DnsFilter", TYPE_FILTER_DNS},
#if 0
	{"MacFilter", TYPE_FILTER_MAC},
#endif
	{NULL, 0}
};

const LcdParamEnumValue FilterIpProtocolEnum[] = {
	{"ICMP", Filter_ICMP},
	{"TCP", Filter_TCP},
	{"UDP", Filter_UDP},
	{"AH", Filter_AH},
	{"ALL", Filter_ALL},
	{NULL, 0}
};
#endif
#if (GROUP_BASE == FYES)
const LcdParamEnumValue limitPolicyEnum[] = {
#if (MACADDR_ADD_LIMIT == FYES)
	{"MAC_LIMIT_RATE", MAC_LIMIT_RATE},
#endif
	{"SHARE_RATE", SHARE_RATE},
	{"EXCLUSIVE_RATE",  EXCLUSIVE_RATE},
	{NULL, 0}
};
const LcdParamEnumValue groupBaseTypeEnum[] = {
	{"FINE_LIMIT", GBT_FINE_LIMIT},
	{"ACT_CONTROL",  GBT_ACT_CONTROL},
	{NULL, 0}
};
#endif

#if (P2P_LIMIT == FYES)
const LcdParamEnumValue p2pLimitEnum[] = {
	{"SHARE_RATE_P2P", SHARE_RATE_P2P},
	{"EXCLUSIVE_RATE_P2P",  EXCLUSIVE_RATE_P2P},
	{NULL, 0}
};
#endif

#if (EASY_TASK == FYES)
const LcdParamEnumValue easyTaskEnum[] = {
	{"DISCONNECT", DISCONNECT},
	{"CONNECTED", CONNECTED},
	{"CONNECTING", CONNECTING},
	{"CHECKING", CHECKING},
	{"CHECK_INTERRUPT", CHECK_INTERRUPT},
	{"CHECK_COMPLETE", CHECK_COMPLETE},
	{"USER_PASS_ERROR", USER_PASS_ERROR},
	{"CONNECT_ERROR", CONNECT_ERROR},
	{NULL, 0}
};
#endif

#if (WIRELESS == FYES)
const LcdParamEnumValue ispMode3G[] = {
	{"CMC", ISP_CMC},
	{"CUC", ISP_CUC},
	{"CDC", ISP_CDC},
	{NULL, 0}
};
const LcdParamEnumValue authType3G[] = {
	{"SIM", AUTH_SIM},
	{"PassAuth", AUTH_PASSAUTH},
	{NULL, 0}
};

const LcdParamEnumValue apWorkModeEnum[] = {
	{"apMode", AP_MODE},
	{"bridgeMode",  BRIDGE_MODE},
	{"repeaterMode", REPEATER_MODE},
	{"lazyMode",  LAZY_MODE},
	{"apClientMode",  AP_CLIENT_MODE},
	{NULL, 0}
};

const LcdParamEnumValue wirelessModeEnum[] = {
	{"only11a", ONLY_11A},
	{"only11g", ONLY_11G},
	{"only11n",  ONLY_11N},
	{"and11a11n",  AND_11A_11N},
	{"and11g11n",  AND_11G_11N},
	{"and11b11g11n",  AND_11B_11G_11N},
	{"and11ac11an11a",  AND_11AC_11AN_11A},
	{"and11ac11an",  AND_11AC_11AN},
	{NULL, 0}
};

const LcdParamEnumValue apMacFilterActionEnum[] = {
	{"allow",  AP_MAC_ALLOW},
	{"deny",  AP_MAC_DENY},
	{NULL, 0}
};

#endif

#if (MAC_FILTER == FYES)
const LcdParamEnumValue MacFilterActionEnum[] = {
	{"allow",  MAC_ALLOW},
	{"deny",  MAC_DENY},
	{NULL, 0}
};
#endif

#if(IP_SEC == FYES)
const LcdParamEnumValue encryptEnum[] = {
	{"des",Des},
	{"3des",Tri_Des},
	{"aes",Aes},
	{"aes192",Aes192},
	{"aes256",Aes256},
	{"rc4",Rc4},
	{"Null",0},
};
const LcdParamEnumValue authWayEnum[] = {
	{"md5",Md5},
	{"sha",Sha},
	{"Null",0},
};
const LcdParamEnumValue groupTypeEnum[] = {
	{"group1",Group1},
	{"group2",Group2},
	{"group5",Group5},
	{"Null",0},
};
const LcdParamEnumValue UserTypeEnum[] = {
	{"domain",IKE_FQDN},
	{"email",IKE_USERFQDN},
	{"key",IKE_KEYID},
	{"ipaddr",IKE_IPV4ADDR},
	{"Null",0},
};
const LcdParamEnumValue negModeEnum[] = {
	{"main",Mainmode},
	{"aggres",Aggresmode},
	{"Null",0},
};
const LcdParamEnumValue ipsecConnTypeEnum[] = {
	{"gatetogate",GATEWAY_TO_GATEWAY},
	{"dyntogate",DYNAMIC_TO_GATEWAY},
	{"gatfromdyn",GATEWAY_FROM_DYNAMIC},
	{"Null",0},
};
#endif

#if (POLICY_ROUTE == FYES)
const  LcdParamEnumValue pr_ipGrpTypeEnum[] = {
	{"pr_iprange", PR_IP_RANGE},
	{"pr_ipgroup", PR_IP_GROUP},
};
#endif

#if ((SERVICE_ZONE == FYES) || (FIT_AP_SERVICE_ZONE == FYES))
const LcdParamEnumValue authMethodEnum[] = {
	{"0", AUTO_AUTH},
	{"1", OPEN_SYS},
	{"2", SHARE_KEY},
	{NULL, 0}
};

const LcdParamEnumValue keyTypeEnum[] = {
	{"1", HEX},
	{"2", ASCII},
	{NULL, 0}
};

const LcdParamEnumValue kFmtEnum[] = {
	{"0", FORBID},
	{"1", BIT_64},
	{"2", BIT_128},
	{NULL, 0}
};

const LcdParamEnumValue wVersionEnum[] = {
	{"0", AUTO_WPA},
	{"1", WPA},
	{"2", WPA2},
	{NULL, 0}
};

const LcdParamEnumValue sAlgEnum[] = {
	{"0", AUTO_SEC},
	{"1", TKIP},
	{"2", AES},
	{NULL, 0}
};

const LcdParamEnumValue uthModeEnum[] = {
	{"0", NONE},
	{"1", WEP},
	{"2", WPA1_AND_WPA2},
	{"3", WPA1_PSK_AND_WPA2_PSK},
	{NULL, 0}
};
#endif

const LcdParamEnumValue wlanRateEnum[] = {
	{"0", AUTO_RATE},
	{"11", M11},
	{"54", M54},
	{"150", M150},
	{"300", M300},
	{NULL, 0}
};
const LcdParamEnumValue wlanManualPowerEnum[] = {
	{"1", LOW},
	{"2", MIDDLE},
	{"3", HIGH},
	{NULL, 0}
};

#if (NET_SHARE_USER == FYES)
const LcdParamEnumValue netShareJurisdictionEnum[] = {
	{"0", SHARE_READ_ONLY},
	{"1", SHARE_READ_WRITE},
	{NULL, 0}
};
#endif

int MibGetEnumByStr(char *str, const LcdParamEnumValue *enumPara)
{
    while(enumPara->enumName != NULL) {
        if(strcmp(str, enumPara->enumName) == 0) {
            return enumPara->enumValue;
        }
        enumPara++;
    }
    return 0;
}

char* MibGetStrByEnum(int value, const LcdParamEnumValue *enumPara)
{
    while(enumPara->enumName != NULL) {
        if(value == enumPara->enumValue) {
            return (char*)enumPara->enumName;
        }
        enumPara++;
    }
    return "";
}
