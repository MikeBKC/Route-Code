

/*
 * this file is contain all conf array
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
#include "a2n.h"

#if (UTT_KNOWIFI == FYES)
static struct st_xmlConfig knowifiDomainConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(KnowifiDomainProfile, head.active), (Pointer)yesnoEnum, 0},
    {"devivename", NULL, VALUE, TSTR, OFFANDSIZE(KnowifiDomainProfile, deviceName), NULL, 0},
    {"username", NULL, VALUE, TSTR, OFFANDSIZE(KnowifiDomainProfile, username), NULL, 0},
    {"userIp", NULL, VALUE, TSTR, OFFANDSIZE(KnowifiDomainProfile, ip), NULL, 0},
    {"userMac", NULL, VALUE, TSTR, OFFANDSIZE(KnowifiDomainProfile, mac), NULL, 0},
    {"status", NULL, VALUE, TINT, OFFANDSIZE(KnowifiDomainProfile, status), NULL, 0},
    {"conntype", NULL, VALUE, TINT, OFFANDSIZE(KnowifiDomainProfile, conntype), NULL, 0},
    {"domain", NULL, VALUE, TSTR, OFFANDSIZE(KnowifiDomainProfile, domain), NULL, 0},
    CONFIGEND,
};
#endif

#if (STATIC_ROUTE == FYES)
static struct st_xmlConfig staticRouteConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(StaticRouteProfile, head.active), (Pointer)yesnoEnum, 0},
    {"dstIp", NULL, VALUE, TIPADDR, OFFANDSIZE(StaticRouteProfile, dstAddr), NULL, 0},
    {"netmask", NULL, VALUE, TIPADDR, OFFANDSIZE(StaticRouteProfile, netmask), NULL, 0},
    {"gwIp", NULL, VALUE, TIPADDR, OFFANDSIZE(StaticRouteProfile, gwAddr), NULL, 0},
    {"metric", NULL, VALUE, TINT, OFFANDSIZE(StaticRouteProfile, preference), NULL, 0},
    {"interface", NULL, VALUE, TSTR, OFFANDSIZE(StaticRouteProfile, interfaceName), NULL, 0},
    CONFIGEND,
};
#endif

#if (FIREWALL ==  FYES)
static struct st_xmlConfig IpFilterConfig[] = {
    {"protocol", NULL, VALUE, TENUM, OFFANDSIZE(FireWallProfile, IpFilter.Protocol), (Pointer)FilterIpProtocolEnum, 0},
    {"dstFromPort", NULL, VALUE, TINT, OFFANDSIZE(FireWallProfile, IpFilter.dstFromPort), NULL, 0},
    {"dstEndPort", NULL, VALUE, TINT, OFFANDSIZE(FireWallProfile, IpFilter.dstEndPort), NULL, 0},
    {"dstFromIp", NULL, VALUE, TIPADDR, OFFANDSIZE(FireWallProfile, IpFilter.dstFromIp), NULL, 0},
    {"dstEndIp", NULL, VALUE, TIPADDR, OFFANDSIZE(FireWallProfile, IpFilter.dstEndIp), NULL, 0},
    {"srcFromPort", NULL, VALUE, TINT, OFFANDSIZE(FireWallProfile, IpFilter.srcFromPort), NULL, 0},
    {"srcEndPort", NULL, VALUE, TINT, OFFANDSIZE(FireWallProfile, IpFilter.srcEndPort), NULL, 0},
    CONFIGEND,
};

#if (IPV6_INTERFACE == FYES)
static struct st_xmlConfig ipv6interfaceConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(Ipv6InterfaceProfile, head.active), (Pointer)yesnoEnum, 0},
    {"ipv6LAN_addr", NULL, VALUE, TSTR, OFFANDSIZE(Ipv6InterfaceProfile, ipv6LAN_addr), NULL, 0},
    {"LANprefixLength", NULL, VALUE, TINT, OFFANDSIZE(Ipv6InterfaceProfile,LANprefixLength ), NULL, 0},
    {"LanAutoEnabled", NULL, VALUE, TINT, OFFANDSIZE(Ipv6InterfaceProfile,LanAutoEnabled ), NULL, 0},
    {"port", NULL, VALUE, TINT, OFFANDSIZE(Ipv6InterfaceProfile, port), NULL, 0},
    {"ipv6WAN_addr", NULL, VALUE, TSTR, OFFANDSIZE(Ipv6InterfaceProfile,ipv6WAN_addr), NULL, 0},
    {"WANprefixLength", NULL, VALUE, TINT, OFFANDSIZE(Ipv6InterfaceProfile,WANprefixLength ), NULL, 0},
    {"ipv6Gateway", NULL, VALUE, TSTR, OFFANDSIZE(Ipv6InterfaceProfile,ipv6Gateway), NULL, 0},
    {"ipv6Dns1", NULL, VALUE, TSTR, OFFANDSIZE(Ipv6InterfaceProfile,ipv6Dns1), NULL, 0},
    {"ipv6Dns2", NULL, VALUE, TSTR, OFFANDSIZE(Ipv6InterfaceProfile,ipv6Dns2), NULL, 0},
    CONFIGEND,
};
#endif

#if (IPV6_RADVD == FYES)
static struct st_xmlConfig radvdConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(Ipv6InterfaceProfile, head.active), (Pointer)yesnoEnum, 0},
    {"DesPrefix", NULL, VALUE, TSTR, OFFANDSIZE(RadvdProfile,DesPrefix), NULL, 0},
    {"manageEnabled", NULL, VALUE, TINT, OFFANDSIZE(RadvdProfile,manageEnabled), NULL, 0},
    {"otherEnabled", NULL, VALUE, TINT, OFFANDSIZE(RadvdProfile,otherEnabled), NULL, 0},
    {"DesPrefixLength", NULL, VALUE, TINT, OFFANDSIZE(RadvdProfile,DesPrefixLength), NULL, 0},
    {"PrefixstaleTime", NULL, VALUE, TINT, OFFANDSIZE(RadvdProfile,PrefixstaleTime), NULL, 0},
    {"noticeInterval", NULL, VALUE, TINT, OFFANDSIZE(RadvdProfile,noticeInterval), NULL, 0},
    {"routerPreferences", NULL, VALUE, TINT, OFFANDSIZE(RadvdProfile,routerPreferences), NULL, 0},
    {"MTU_config", NULL, VALUE, TINT, OFFANDSIZE(RadvdProfile,MTU_config), NULL, 0},
    {"routerLifetime", NULL, VALUE, TINT, OFFANDSIZE(RadvdProfile,routerLifetime), NULL, 0},
    CONFIGEND,
};
#endif
#if(IPV6_ROUTE == FYES)
static  struct  st_xmlConfig ipv6RoutConfigConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(Ipv6RoutConfProfile, head.active), (Pointer)yesnoEnum, 0},
    {"routeName", NULL, VALUE,TSTR, OFFANDSIZE(Ipv6RoutConfProfile,routeName), NULL, 0},
    {"desIP", NULL, VALUE,TSTR, OFFANDSIZE(Ipv6RoutConfProfile,desIP), NULL, 0},
    {"desNetPrefix", NULL, VALUE,TINT, OFFANDSIZE(Ipv6RoutConfProfile,desNetPrefix), NULL, 0},
    {"GateWay", NULL, VALUE,TSTR, OFFANDSIZE(Ipv6RoutConfProfile,GateWay), NULL, 0},
    {"priority", NULL, VALUE,TINT, OFFANDSIZE(Ipv6RoutConfProfile,priority), NULL, 0},
    {"port", NULL, VALUE,TSTR, OFFANDSIZE(Ipv6RoutConfProfile,port), NULL, 0},
CONFIGEND,
};
#endif

#if (IPV6_DHCPV6 == FYES)
static struct st_xmlConfig dhcp6Config[] = {
    {"dhcpEnabled", NULL, VALUE, TINT, OFFANDSIZE(Dhcp6Profile,dhcpEnabled), NULL, 0},
    {"sta_ipv6_addr", NULL, VALUE, TSTR, OFFANDSIZE(Dhcp6Profile,sta_ipv6_addr), NULL, 0},
    {"end_ipv6_addr", NULL, VALUE, TSTR, OFFANDSIZE(Dhcp6Profile,end_ipv6_addr), NULL, 0},
    {"dhcpPrefixLength", NULL, VALUE, TINT, OFFANDSIZE(Dhcp6Profile,dhcpPrefixLength), NULL, 0},
    {"ipv6dhcpGateway", NULL, VALUE, TSTR, OFFANDSIZE(Dhcp6Profile,ipv6dhcpGateway), NULL, 0},
    {"staleTime", NULL, VALUE, TINT, OFFANDSIZE(Dhcp6Profile,staleTime), NULL, 0},
    {"ipv6DnsZ", NULL, VALUE, TSTR, OFFANDSIZE(Dhcp6Profile,ipv6DnsZ), NULL, 0},
    {"ipv6DnsB", NULL, VALUE, TSTR, OFFANDSIZE(Dhcp6Profile,ipv6DnsB), NULL, 0},
    CONFIGEND,
};
#endif

#if(IPV6_NOTLISTPREFIXS == FYES)
static struct st_xmlConfig ipv6PrefixListConfig[]={
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(Ipv6PrefixListProfile, head.active), (Pointer)yesnoEnum, 0},
    {"DesPrefix", NULL, VALUE,TSTR, OFFANDSIZE(Ipv6PrefixListProfile,DesPrefix), NULL, 0},
    {"DesPrefixLength", NULL, VALUE,TINT, OFFANDSIZE(Ipv6PrefixListProfile,DesPrefixLength), NULL, 0},
    {"PrefixstaleTime", NULL, VALUE,TINT, OFFANDSIZE(Ipv6PrefixListProfile,PrefixstaleTime), NULL, 0},
    CONFIGEND,
};
#endif

#if(IPV6_TUNNEL == FYES)
static struct st_xmlConfig tunnelConfig[]={
    {"tunnelEnabled", NULL, VALUE,TINT, OFFANDSIZE(TunnelProfile,tunnelEnabled), NULL, 0},
    {"local_ip4_addr", NULL, VALUE,TIPADDR, OFFANDSIZE(TunnelProfile,local_ip4_addr), NULL, 0},
    {"remote_ip4_addr", NULL, VALUE,TIPADDR, OFFANDSIZE(TunnelProfile,remote_ip4_addr), NULL, 0},
    {"remote_ip6_addr", NULL, VALUE,TSTR, OFFANDSIZE(TunnelProfile,remote_ip6_addr), NULL, 0},
    CONFIGEND,
};
#endif
#if(IPV6_ISATAP == FYES)
static struct st_xmlConfig isatapConfig[]={
    {"ISATAPEnabled", NULL, VALUE,TINT, OFFANDSIZE(IsatapProfile,ISATAPEnabled), NULL, 0},
    {"ISATAP_IPv6Pre", NULL, VALUE,TSTR, OFFANDSIZE(IsatapProfile,ISATAP_IPv6Pre), NULL, 0},
    {"ISATAP_IPv4Gat", NULL, VALUE,TIPADDR, OFFANDSIZE(IsatapProfile,ISATAP_IPv4Gat), NULL, 0},
    CONFIGEND,
};
#endif

static struct st_xmlConfig WordFilterConfig[] = {
    {"filterKey", NULL, VALUE, TSTR, OFFANDSIZE(FireWallProfile, WordFilter.FilterKey), NULL, 0},
    CONFIGEND,
};

static struct st_xmlConfig firewallConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(FireWallProfile, head.active), (Pointer)yesnoEnum, 0},
    {"addrTypeEn", NULL, VALUE, TENUM, OFFANDSIZE(FireWallProfile, addrTypeEn), (Pointer)addrObjTypeEnum, 0},
    {"srcFromIp", NULL, VALUE, TIPADDR, OFFANDSIZE(FireWallProfile, srcFromIp), NULL, 0},
    {"srcEndIp", NULL, VALUE, TIPADDR, OFFANDSIZE(FireWallProfile, srcEndIp), NULL, 0},
#if (IP_GRP == FYES)
    {"ipGrpName", NULL, VALUE, TSTR, OFFANDSIZE(FireWallProfile, ipGrpName), NULL, 0},
#endif
    {"status", NULL, VALUE, TENUM, OFFANDSIZE(FireWallProfile, Status), (Pointer)enDisableEnum, 0},
#if 0
    {"priority", NULL, VALUE, TINT, OFFANDSIZE(FireWallProfile, PRI), 0, 0},
#endif
    {"filterType", NULL, VALUE, TENUM, OFFANDSIZE(FireWallProfile, FilterType), (Pointer)FilterTypeEnum, 0},
    {"fwTypeEn", NULL, VALUE, TENUM, OFFANDSIZE(FireWallProfile, fwTypeEn), (Pointer)fwObjTypeEnum, 0},
#if (PORT_GRP == FYES)
    {"fwGrpName", NULL, VALUE, TSTR, OFFANDSIZE(FireWallProfile, fwGrpName), NULL, 0},
#endif
    {"destAddrTypeEn", NULL, VALUE, TENUM, OFFANDSIZE(FireWallProfile, destAddrTypeEn), (Pointer)addrObjTypeEnum, 0},
#if (IP_GRP == FYES)
    {"destIpGrpName", NULL, VALUE, TSTR, OFFANDSIZE(FireWallProfile, destIpGrpName), NULL, 0},
#endif
    {"IpFilter", IpFilterConfig, 0, 0, 0, 0, 0},
    {"wordFilter", WordFilterConfig, 0, 0, 0, 0, 0},
    {"day", NULL, VALUE, TINT, OFFANDSIZE(FireWallProfile, fw_day), NULL, 0},
    {"timestart", NULL, VALUE, TSTR, OFFANDSIZE(FireWallProfile, fw_timestart), NULL, 0},
    {"timestop", NULL, VALUE, TSTR, OFFANDSIZE(FireWallProfile, fw_timestop), NULL, 0},
    CONFIGEND,
};
#endif
    
#if (NOTICE == FYES)
static struct st_xmlConfig noticeConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(NoticeProfile, head.active), (Pointer)yesnoEnum, 0},
    {"webSubmitType", NULL, VALUE, TENUM, OFFANDSIZE(NoticeProfile, webSubmitType), (Pointer)webSubmitEnum, 0},
    {"ipstart", NULL, VALUE, TIPADDR, OFFANDSIZE(NoticeProfile, ipstart), NULL, 0},
    {"ipend", NULL, VALUE, TIPADDR, OFFANDSIZE(NoticeProfile, ipend), NULL, 0},
    {"day", NULL, VALUE, TINT, OFFANDSIZE(NoticeProfile, day), NULL, 0},
    {"timestart", NULL, VALUE, TSTR, OFFANDSIZE(NoticeProfile, timestart), NULL, 0},
    {"timestop", NULL, VALUE, TSTR, OFFANDSIZE(NoticeProfile, timestop), NULL, 0},
    {"remainDays", NULL, VALUE, TINT, OFFANDSIZE(NoticeProfile, remainDays), NULL, 0},
    {"datestart", NULL, VALUE, TSTR, OFFANDSIZE(NoticeProfile, datestart), NULL, 0},
    {"datestop", NULL, VALUE, TSTR, OFFANDSIZE(NoticeProfile, datestop), NULL, 0},
    CONFIGEND,
};
#endif

#if (CWMP == FYES)
static struct st_xmlConfig cwmpConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(CwmpProfile, head.active), (Pointer)yesnoEnum, 0},
    {"type", NULL, VALUE, TINT, OFFANDSIZE(CwmpProfile, type), NULL, 0},
    {"acsUrl", NULL, VALUE, TSTR, OFFANDSIZE(CwmpProfile, acsUrl), NULL, 0},
    {"acsAuthEnabled", NULL, VALUE, TINT, OFFANDSIZE(CwmpProfile, acsAuthEnable), NULL, 0},
    {"acsUsername", NULL, VALUE, TSTR, OFFANDSIZE(CwmpProfile, acsUsername), NULL, 0},
    {"acsPassword", NULL, VALUE, TSTR, OFFANDSIZE(CwmpProfile, acsPassword), NULL, 0},
    {"periodicInformInterval", NULL, VALUE, TINT, OFFANDSIZE(CwmpProfile, periodicInformInterval), NULL, 0},
    {"cpeAuthEnable", NULL, VALUE, TINT, OFFANDSIZE(CwmpProfile, cpeAuthEnable), NULL, 0},
    {"cpeUsername", NULL, VALUE, TSTR, OFFANDSIZE(CwmpProfile, cpeUsername), NULL, 0},
    {"cpePassword", NULL, VALUE, TSTR, OFFANDSIZE(CwmpProfile, cpePassword), NULL, 0},
    {"stunEnable", NULL, VALUE, TINT, OFFANDSIZE(CwmpProfile, stunEnable), NULL, 0},
    {"stunUrl", NULL, VALUE, TSTR, OFFANDSIZE(CwmpProfile, stunUrl), NULL, 0},
    {"stunPeriodicInterval", NULL, VALUE, TINT, OFFANDSIZE(CwmpProfile, stunPeriodicInterval), NULL, 0},
    {"stunAuthEnable", NULL, VALUE, TINT, OFFANDSIZE(CwmpProfile, stunAuthEnable), NULL, 0},
    {"stunUsername", NULL, VALUE, TSTR, OFFANDSIZE(CwmpProfile, stunUsername), NULL, 0},
    {"stunPassword", NULL, VALUE, TSTR, OFFANDSIZE(CwmpProfile, stunPassword), NULL, 0},
    CONFIGEND,
};
#endif

#if (WEB_AUTH == FYES)
static struct st_xmlConfig webauthglobalConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(WebAuthGlobalProfile, head.active), (Pointer)yesnoEnum, 0},
    {"enabled", NULL, VALUE, TINT, OFFANDSIZE(WebAuthGlobalProfile, enabled), NULL, 0},
#if (WIFIDOG_AUTH == FYES)
    {"webauthtype", NULL, VALUE, TINT, OFFANDSIZE(WebAuthGlobalProfile, authType), NULL, 0},
    {"testDomain", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, testDomain), NULL, 0},
    {"login_url", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, login_url), NULL, 0},
    {"auth_url", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, auth_url), NULL, 0},
    {"success_url", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, success_url), NULL, 0},
    {"ping_url", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, ping_url), NULL, 0},
#if (WIFIDOG_USER_SUCCESS_URL == FYES)
    {"successUrl", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, wifiData.successUrl), NULL, 0},
#endif
#if (WIFIDOG_NOTICE == FYES)
    {"noticeTime", NULL, VALUE, TINT, OFFANDSIZE(WebAuthGlobalProfile, wifiData.noticeTime), NULL, 0},
    {"noticeUrl", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, wifiData.noticeUrl), NULL, 0},
#endif
#if (WIFIDOG_MAC_AUTH == FYES)
    {"macAuthEn", NULL, VALUE, TINT, OFFANDSIZE(WebAuthGlobalProfile, wifiData.macAuthEn), NULL, 0},
#endif
#if (WIFIDOG_USER_SECRET_KEY == FYES)
    {"secret_key", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, wifiData.secret_key), NULL, 0},
#endif
#if (SMS_ENABLE ==1)
    {"smsEnable", NULL, VALUE, TINT, OFFANDSIZE(WebAuthGlobalProfile, smsEnable), NULL, 0},
    {"cdmaType", NULL, VALUE, TINT, OFFANDSIZE(WebAuthGlobalProfile, cdmaType), NULL, 0},
    {"province", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, province), NULL, 0},
    {"city", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, city), NULL, 0},
    {"sms_number", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, sms_number), NULL, 0},
#endif
#endif
    {"enpicture", NULL, VALUE, TINT, OFFANDSIZE(WebAuthGlobalProfile, enpicture), NULL, 0},
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
    {"idcardEnabled", NULL, VALUE, TINT, OFFANDSIZE(WebAuthGlobalProfile, idcard.enabled), NULL, 0},
    {"idcardTimeTotal", NULL, VALUE, TINT, OFFANDSIZE(WebAuthGlobalProfile, idcard.timeTotal), NULL, 0},
    {"idcardMaxSessions", NULL, VALUE, TINT, OFFANDSIZE(WebAuthGlobalProfile, idcard.maxSessions), NULL, 0},
#endif
#if (WEBAUTH_AUTO == FYES)
    {"smsEnabled", NULL, VALUE, TINT, OFFANDSIZE(WebAuthGlobalProfile, sms.enabled), NULL, 0},
    {"smsTimeTotal", NULL, VALUE, TINT, OFFANDSIZE(WebAuthGlobalProfile, sms.timeTotal), NULL, 0},
    {"smsMaxSessions", NULL, VALUE, TINT, OFFANDSIZE(WebAuthGlobalProfile, sms.maxSessions), NULL, 0},
    {"smsMaxAskTimes", NULL, VALUE, TINT, OFFANDSIZE(WebAuthGlobalProfile, sms.totalTimes), NULL, 0},
    {"smsserver", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, sms.smsServer), NULL, 0},
    {"smsserver2", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, logServer), NULL, 0},
    {"smsContents", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, sms.contents), NULL, 0},
    {"wechatEnabled", NULL, VALUE, TINT, OFFANDSIZE(WebAuthGlobalProfile, wechat.enabled), NULL, 0},
    {"wechatTimeTotal", NULL, VALUE, TINT, OFFANDSIZE(WebAuthGlobalProfile, wechat.timeTotal), NULL, 0},
    {"wechatMaxSessions", NULL, VALUE, TINT, OFFANDSIZE(WebAuthGlobalProfile, wechat.maxSessions), NULL, 0},
    {"wechatMaxAskTimes", NULL, VALUE, TINT, OFFANDSIZE(WebAuthGlobalProfile, wechat.totalTimes), NULL, 0},
    {"wechatContents", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, wechat.contents), NULL, 0},
    {"wechatName", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, wechat.wechatName), NULL, 0},
    {"wechatPicName", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, wechat.wechatPicName), NULL, 0},
    {"wechatToken", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, wechat.wechatToken), NULL, 0},
    {"wechatKeyContents", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, wechat.wechatKeyContents), NULL, 0},
#endif
#if (WEBAUTH_SELF_HELP == FYES)
    {"selfHelpEnable", NULL, VALUE, TENUM, OFFANDSIZE(WebAuthGlobalProfile, self_help), (Pointer)enDisableEnum, 0},
#endif
    {"active_pic", NULL, VALUE, TINT, OFFANDSIZE(WebAuthGlobalProfile, active_pic), NULL, 0},
    {"exceptIpGroup", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, exceptIpGroup), NULL, 0},
    {"tipstitle", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, tipstitle), NULL, 0},
    {"tipsinfo", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, tipsinfo), NULL, 0},
    {"contact", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, contact), NULL, 0},
#if (WEBAUTH_ADVANCE == FYES)
    {"enDefPage", NULL, VALUE, TINT, OFFANDSIZE(WebAuthGlobalProfile, enDefPage), NULL, 0},
    {"picName", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, picName), NULL, 0},
    {"pageName", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, pageName), NULL, 0},
#endif
#if(WEB_AUTH_STALE == FYES)
    {"staleTime", NULL, VALUE, TINT, OFFANDSIZE(WebAuthGlobalProfile, staleT), NULL, 0},
#endif
#if (UTT_KNOWIFI == FYES)
    {"knowifiEnable", NULL, VALUE, TINT, OFFANDSIZE(WebAuthGlobalProfile, knowifi_enable), NULL, 0},
    {"knowifiRouteId", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthGlobalProfile, knowifi_routeId), NULL, 0},
#endif
    CONFIGEND,
};
static struct st_xmlConfig webauthConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(WebAuthProfile, head.active), (Pointer)yesnoEnum, 0},
    {"user", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthProfile, user), NULL, 0},
    {"passwd", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthProfile, passwd), NULL, 0},
    {"remark", NULL, VALUE, TSTR, OFFANDSIZE(WebAuthProfile, remark), NULL, 0},
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
    {"idcode", NULL, TSTR, OFFANDSIZE(WebAuthProfile, idcode), NULL, 0},
    {"autoAuthMethod", NULL, VALUE, TINT, OFFANDSIZE(WebAuthProfile, autoAuthMethod), NULL, 0},
#endif
#if (WEBAUTH_ACCOUNT == FYES)
    {"timeingPatternEn", NULL, VALUE, TENUM, OFFANDSIZE(WebAuthProfile, timeingPatternEn), (Pointer)enDisableEnum, 0},
    {"timeStart", NULL, VALUE, TINT, OFFANDSIZE(WebAuthProfile, timeStart), NULL, 0},
    {"timeStop", NULL, VALUE, TINT, OFFANDSIZE(WebAuthProfile, timeStop), NULL, 0},
    {"timeTotal", NULL, VALUE, TINT, OFFANDSIZE(WebAuthProfile, timeTotal), NULL, 0},
    {"timeUsed", NULL, VALUE, TINT, OFFANDSIZE(WebAuthProfile, timeUsed), NULL, 0},
#endif
#if (WEBAUTH_SELF_HELP == FYES)
    {"webauthoptCnt", NULL, VALUE, TINT, OFFANDSIZE(WebAuthProfile, optCnt), NULL, 0},
#endif
    {"maxsession", NULL, VALUE, TINT, OFFANDSIZE(WebAuthProfile, maxSession), NULL, 0},
    CONFIGEND,
};
#endif

#if (LOG_MANAGE == FYES)
static struct st_xmlConfig logmanageConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(LogManageProfile, head.active), (Pointer)yesnoEnum, 0},
    {"webLogEnable", NULL, VALUE, TENUM, OFFANDSIZE(LogManageProfile, webLogEnable), (Pointer)enDisableEnum, 0},
    {"QQLogEnable", NULL, VALUE, TENUM, OFFANDSIZE(LogManageProfile, QQLogEnable), (Pointer)enDisableEnum, 0},
    {"MSNLogEnable", NULL, VALUE, TENUM, OFFANDSIZE(LogManageProfile, MSNLogEnable), (Pointer)enDisableEnum, 0},
    {"mailLogEnable", NULL, VALUE, TENUM, OFFANDSIZE(LogManageProfile, mailLogEnable), (Pointer)enDisableEnum, 0},
    {"eimLogEnable", NULL, VALUE, TENUM, OFFANDSIZE(LogManageProfile, eimLogEnable), (Pointer)enDisableEnum, 0},
#if CONFIG_XPORT_LOG 
    {"luntanLogEnable", NULL, VALUE, TENUM, OFFANDSIZE(LogManageProfile, luntanLogEnable), (Pointer)enDisableEnum, 0},
    {"weiboLogEnable", NULL, VALUE, TENUM, OFFANDSIZE(LogManageProfile, weiboLogEnable), (Pointer)enDisableEnum, 0},
    {"NatLogEnable", NULL, VALUE, TENUM, OFFANDSIZE(LogManageProfile, NatLogEnable), (Pointer)enDisableEnum, 0},
#endif
    CONFIGEND,
};
#endif

#if (AP_LOG_MANAGE == FYES)
static struct st_xmlConfig aplogmanageConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(ApLogManageProfile, head.active), (Pointer)yesnoEnum, 0},
    {"apLogType0", NULL, VALUE, TENUM, OFFANDSIZE(ApLogManageProfile, apLogType[0]), (Pointer)enDisableEnum, 0},
    {"apLogType1", NULL, VALUE, TENUM, OFFANDSIZE(ApLogManageProfile, apLogType[1]), (Pointer)enDisableEnum, 0},
    {"apLogType2", NULL, VALUE, TENUM, OFFANDSIZE(ApLogManageProfile, apLogType[2]), (Pointer)enDisableEnum, 0},
    {"apLogType3", NULL, VALUE, TENUM, OFFANDSIZE(ApLogManageProfile, apLogType[3]), (Pointer)enDisableEnum, 0},
    {"apLogType4", NULL, VALUE, TENUM, OFFANDSIZE(ApLogManageProfile, apLogType[4]), (Pointer)enDisableEnum, 0},
    {"apLogType5", NULL, VALUE, TENUM, OFFANDSIZE(ApLogManageProfile, apLogType[5]), (Pointer)enDisableEnum, 0},
    CONFIGEND,
};
#endif

#if (DDNS == FYES)
static struct st_xmlConfig ddnsConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(DDNSProfile, head.active), (Pointer)yesnoEnum, 0},
    {"provider", NULL, VALUE, TSTR, OFFANDSIZE(DDNSProfile, ddnsProvider), NULL, 0},
    {"host", NULL, VALUE, TSTR, OFFANDSIZE(DDNSProfile, ddnsHost), NULL, 0},
    {"account", NULL, VALUE, TSTR, OFFANDSIZE(DDNSProfile, ddnsAccount), NULL, 0},
    {"passwd", NULL, VALUE, TSTR, OFFANDSIZE(DDNSProfile, ddnsPasswd), NULL, 0},
    {"if", NULL, VALUE, TSTR, OFFANDSIZE(DDNSProfile, ddnsIf), NULL, 0},
    CONFIGEND,
};
#endif
#if (WIFIDOG_AUTH == FYES)
#if (WHITELIST == 1)
static struct st_xmlConfig whitelistConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(WhiteListProfile, head.active), (Pointer)yesnoEnum, 0},
    {"domain", NULL, VALUE, TSTR, OFFANDSIZE(WhiteListProfile, domain), NULL, 0},
    CONFIGEND,
};
#endif
#endif

#if (PPPOE_SERVER == FYES)
static struct st_xmlConfig pppoeSrvGloConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(PPPoESrvGloProfile, head.active), (Pointer)yesnoEnum, 0},
    {"onlyPppoe", NULL, VALUE, TENUM, OFFANDSIZE(PPPoESrvGloProfile, onlyPppoe), (Pointer)yesnoEnum, 0},
    {"ipStart", NULL, VALUE, TIPADDR, OFFANDSIZE(PPPoESrvGloProfile, ipStart), NULL, 0},
    {"priDns", NULL, VALUE, TIPADDR, OFFANDSIZE(PPPoESrvGloProfile, pDns), NULL, 0},
    {"secDns", NULL, VALUE, TIPADDR, OFFANDSIZE(PPPoESrvGloProfile, sDns), NULL, 0},
#if (PPPOE_SELF_HELP == FYES)
    {"selfHelpEnable", NULL, VALUE, TENUM, OFFANDSIZE(PPPoESrvGloProfile, self_help), (Pointer)enDisableEnum, 0},
#endif
    {"authMode", NULL, VALUE, TENUM, OFFANDSIZE(PPPoESrvGloProfile, pppAuthMode), (Pointer)pppAuthEnum, 0},
    {"maxSessCnt", NULL, VALUE, TINT, OFFANDSIZE(PPPoESrvGloProfile, maxSessCnt), NULL, 0},
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
    {"ipcount", NULL, VALUE, TINT, OFFANDSIZE(PPPoESrvGloProfile, ipcount), NULL, 0},
#endif
    {"exceptIpGroup", NULL, VALUE, TSTR, OFFANDSIZE(PPPoESrvGloProfile, exceptIpGroup), NULL, 0},
    CONFIGEND,
};

static struct st_xmlConfig pppoeSrvAccConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(PPPoESrvAccProfile, head.active), (Pointer)yesnoEnum, 0},
    {"pppoename", NULL, VALUE, TSTR, OFFANDSIZE(PPPoESrvAccProfile, pppoename), NULL, 0},
    {"passwd", NULL, VALUE, TSTR, OFFANDSIZE(PPPoESrvAccProfile, passwd), NULL, 0},
    {"remark", NULL, VALUE, TSTR, OFFANDSIZE(PPPoESrvAccProfile, remark), NULL, 0},
#if 0
    {"pppoeuploadRate", NULL, VALUE, TINT, OFFANDSIZE(PPPoESrvAccProfile, uRate), NULL, 0},
    {"pppoedownloadRate", NULL, VALUE, TINT, OFFANDSIZE(PPPoESrvAccProfile, dRate), NULL, 0},
#endif
    {"bindIp", NULL, VALUE, TIPADDR, OFFANDSIZE(PPPoESrvAccProfile, bindIp), NULL, 0},
#if (PPPOE_ACCOUNT == FYES)
    {"chargeType", NULL, VALUE, TENUM, OFFANDSIZE(PPPoESrvAccProfile, chargeType), (Pointer)chargeTypeEnum, 0},
    {"tmStart", NULL, VALUE, TINT, OFFANDSIZE(PPPoESrvAccProfile, tmStart), NULL, 0},
    {"tmStop", NULL, VALUE, TINT, OFFANDSIZE(PPPoESrvAccProfile, tmStop), NULL, 0},
#endif
#if (PPPOE_SELF_HELP == FYES)
    {"pppoeoptCnt", NULL, VALUE, TINT, OFFANDSIZE(PPPoESrvAccProfile, optCnt), NULL, 0},
#endif
    {"accMaxConv", NULL, VALUE, TINT, OFFANDSIZE(PPPoESrvAccProfile, accmaxconv), NULL, 0},
    {"AccMACBind", NULL, VALUE, TSTR, OFFANDSIZE(PPPoESrvAccProfile, AccMACBind), NULL, 0},
    {"bindmac", NULL, VALUE, TMACADDR, OFFANDSIZE(PPPoESrvAccProfile, bindmac), NULL, 0},
    {"bindmac2", NULL, VALUE, TMACADDR, OFFANDSIZE(PPPoESrvAccProfile, bindmac2), NULL, 0},
    {"bindmac3", NULL, VALUE, TMACADDR, OFFANDSIZE(PPPoESrvAccProfile, bindmac3), NULL, 0},
    {"bindmac4", NULL, VALUE, TMACADDR, OFFANDSIZE(PPPoESrvAccProfile, bindmac4), NULL, 0},
#if (PPPOE_LIMIT == FYES)
    {"uRateLimit", NULL, VALUE, TINT, OFFANDSIZE(PPPoESrvAccProfile, uRateLimit), NULL, 0},
    {"dRateLimit", NULL, VALUE, TINT, OFFANDSIZE(PPPoESrvAccProfile, dRateLimit), NULL, 0},
#endif
    CONFIGEND,
};
#endif

#if (DHCP_SERVER == FYES)
static struct st_xmlConfig  dhcpServerConfig[] = {
//  {"active", NULL, VALUE, TENUM, OFFANDSIZE(DhcpBindProfile, head.active), (Pointer)yesnoEnum, 0},
    {"DhcpSEn", NULL, VALUE, TENUM, OFFANDSIZE(DhcpServerProfile, enable), (Pointer)enDisableEnum, 0},
    {"ipStart", NULL, VALUE, TIPADDR, OFFANDSIZE(DhcpServerProfile, ipStart), NULL, 0},
    {"ipEnd", NULL, VALUE, TIPADDR, OFFANDSIZE(DhcpServerProfile, ipEnd), NULL, 0},
#if (MULTI_SSID_DHCP_POOL == FYES)
    {"ssid1_dhcpE", NULL, VALUE, TENUM, OFFANDSIZE(DhcpServerProfile, ssid1_dhcp_enable), (Pointer)enDisableEnum, 0},
    {"ssid1_ipStart", NULL, VALUE, TIPADDR, OFFANDSIZE(DhcpServerProfile, ssid1_ipStart), NULL, 0},
    {"ssid1_ipEnd", NULL, VALUE, TIPADDR, OFFANDSIZE(DhcpServerProfile, ssid1_ipEnd), NULL, 0},
    {"ssid2_dhcpE", NULL, VALUE, TENUM, OFFANDSIZE(DhcpServerProfile, ssid2_dhcp_enable), (Pointer)enDisableEnum, 0},
    {"ssid2_ipStart", NULL, VALUE, TIPADDR, OFFANDSIZE(DhcpServerProfile, ssid2_ipStart), NULL, 0},
    {"ssid2_ipEnd", NULL, VALUE, TIPADDR, OFFANDSIZE(DhcpServerProfile, ssid2_ipEnd), NULL, 0},
#if (WIRELESS_5G == FYES)
    {"ssid1_dhcpE_5g", NULL, VALUE, TENUM, OFFANDSIZE(DhcpServerProfile, ssid1_dhcp_enable_5g), (Pointer)enDisableEnum, 0},
    {"ssid1_ipStart_5g", NULL, VALUE, TIPADDR, OFFANDSIZE(DhcpServerProfile, ssid1_ipStart_5g), NULL, 0},
    {"ssid1_ipEnd_5g", NULL, VALUE, TIPADDR, OFFANDSIZE(DhcpServerProfile, ssid1_ipEnd_5g), NULL, 0},
    {"ssid2_dhcpE_5g", NULL, VALUE, TENUM, OFFANDSIZE(DhcpServerProfile, ssid2_dhcp_enable_5g), (Pointer)enDisableEnum, 0},
    {"ssid2_ipStart_5g", NULL, VALUE, TIPADDR, OFFANDSIZE(DhcpServerProfile, ssid2_ipStart_5g), NULL, 0},
    {"ssid2_ipEnd_5g", NULL, VALUE, TIPADDR, OFFANDSIZE(DhcpServerProfile, ssid2_ipEnd_5g), NULL, 0},
#endif
#endif
    {"nm", NULL, VALUE, TIPADDR, OFFANDSIZE(DhcpServerProfile, netmask), NULL, 0},
    {"gw", NULL, VALUE, TIPADDR, OFFANDSIZE(DhcpServerProfile, gateway), NULL, 0},
    {"lease", NULL, VALUE, TINT, OFFANDSIZE(DhcpServerProfile, lease), NULL, 0},
    {"pDns", NULL, VALUE, TIPADDR, OFFANDSIZE(DhcpServerProfile, pDns), NULL, 0},
    {"sDns", NULL, VALUE, TIPADDR, OFFANDSIZE(DhcpServerProfile, sDns), NULL, 0},
    {"dnsProxyEn", NULL, VALUE, TENUM, OFFANDSIZE(DhcpServerProfile, DnsProxyEn), (Pointer)enDisableEnum, 0},
    {"dnsPpDns", NULL, VALUE, TIPADDR, OFFANDSIZE(DhcpServerProfile, pPDns), NULL, 0},
    {"dnsPsDns", NULL, VALUE, TIPADDR, OFFANDSIZE(DhcpServerProfile, sPDns), NULL, 0},
#if (DHCP_SERVER_OPTION_43 == FYES)
    {"option43Type", NULL, VALUE, TINT, OFFANDSIZE(DhcpServerProfile, option43Type), NULL, 0},
    {"option43Addr", NULL, VALUE, TSTR, OFFANDSIZE(DhcpServerProfile, option43Addr), NULL, 0},
    {"option43Addr2", NULL, VALUE, TSTR, OFFANDSIZE(DhcpServerProfile, option43Addr2), NULL, 0},
#endif
#if (DHCP_AUTO_BIND == FYES)
    {"DhcpNewAutobindEn", NULL, VALUE, TENUM, OFFANDSIZE(DhcpServerProfile, DhcpNewAutobind), (Pointer)enDisableEnum, 0},
    {"DhcpDelAutobindEn", NULL, VALUE, TENUM, OFFANDSIZE(DhcpServerProfile, DhcpDelAutobind), (Pointer)enDisableEnum, 0},
#endif
    CONFIGEND
};
#endif

#if (DHCP_POOL == FYES)
static struct st_xmlConfig  dhcpPoolConfig[] = {
	{"active", NULL, VALUE, TENUM, OFFANDSIZE(DhcpPoolProfile, head.active), (Pointer)yesnoEnum, 0},
    {"dhcpEn", NULL, VALUE, TENUM, OFFANDSIZE(DhcpPoolProfile, dhcpEn), (Pointer)enDisableEnum, 0},
    {"interface", NULL, VALUE, TSTR, OFFANDSIZE(DhcpPoolProfile, interface), NULL, 0},
    {"vid", NULL, VALUE, TINT, OFFANDSIZE(DhcpPoolProfile, vid), NULL, 0},
    {"ipStart", NULL, VALUE, TIPADDR, OFFANDSIZE(DhcpPoolProfile, ipStart), NULL, 0},
    {"ipEnd", NULL, VALUE, TIPADDR, OFFANDSIZE(DhcpPoolProfile, ipEnd), NULL, 0},
    {"nm", NULL, VALUE, TIPADDR, OFFANDSIZE(DhcpPoolProfile, netmask), NULL, 0},
    {"gw", NULL, VALUE, TIPADDR, OFFANDSIZE(DhcpPoolProfile, gateway), NULL, 0},
    {"lease", NULL, VALUE, TINT, OFFANDSIZE(DhcpPoolProfile, lease), NULL, 0},
    {"pDns", NULL, VALUE, TIPADDR, OFFANDSIZE(DhcpPoolProfile, pDns), NULL, 0},
    {"sDns", NULL, VALUE, TIPADDR, OFFANDSIZE(DhcpPoolProfile, sDns), NULL, 0},
#if (DHCP_SERVER_OPTION_43 == FYES)
    {"option43Type", NULL, VALUE, TINT, OFFANDSIZE(DhcpPoolProfile, option43Type), NULL, 0},
    {"option43Addr", NULL, VALUE, TSTR, OFFANDSIZE(DhcpPoolProfile, option43Addr), NULL, 0},
    {"option43Addr2", NULL, VALUE, TSTR, OFFANDSIZE(DhcpPoolProfile, option43Addr2), NULL, 0},
#endif
    CONFIGEND
};
#endif

#if (VLAN_CONFIG == FYES)
static struct st_xmlConfig  vlanConfig[] = {
	{"active", NULL, VALUE, TENUM, OFFANDSIZE(VlanConfigProfile, head.active), (Pointer)yesnoEnum, 0},
    {"vlanEn", NULL, VALUE, TENUM, OFFANDSIZE(VlanConfigProfile, vlanEn), (Pointer)enDisableEnum, 0},
//    {"interface", NULL, VALUE, TSTR, OFFANDSIZE(VlanConfigProfile, interface), NULL, 0},
    {"vid", NULL, VALUE, TINT, OFFANDSIZE(VlanConfigProfile, vid), NULL, 0},
    {"ip", NULL, VALUE, TIPADDR, OFFANDSIZE(VlanConfigProfile, ip), NULL, 0},
    {"nm", NULL, VALUE, TIPADDR, OFFANDSIZE(VlanConfigProfile, netmask), NULL, 0},
    CONFIGEND
};
#endif

static struct st_xmlConfig  userConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(UserProfile, head.active), (Pointer)yesnoEnum, 0},
    {"dhcpPoolName", NULL, VALUE, TSTR, OFFANDSIZE(UserProfile, dhcpPoolName), NULL, 0},
    {"bindEn", NULL, VALUE, TENUM, OFFANDSIZE(UserProfile, bindEn), (Pointer)enDisableEnum, 0},
#if(DHCP_POOL == FYES)
    {"vid", NULL, VALUE, TINT, OFFANDSIZE(UserProfile, vid), NULL, 0},
#endif
    {"ip", NULL, VALUE, TIPADDR, OFFANDSIZE(UserProfile, ip), NULL, 0},
    {"mac", NULL, VALUE, TMACADDR, OFFANDSIZE(UserProfile, mac), NULL, 0},
#if (DHCP_AUTO_BIND == FYES)
    {"bindType", NULL, VALUE, TENUM, OFFANDSIZE(UserProfile, bindType), (Pointer)endhcpBindType, 0},
#endif
    CONFIGEND,
};

#if (DNS_AGENCY == FYES)
static struct st_xmlConfig dnsAgencyConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(DnsAgencyProfile, head.active), (Pointer)yesnoEnum, 0},
    {"dnsProxyEn", NULL, VALUE, TENUM, OFFANDSIZE(DnsAgencyProfile, DnsProxyEn), (Pointer)yesnoEnum, 0},
    {"dnsPpDns", NULL, VALUE, TIPADDR, OFFANDSIZE(DnsAgencyProfile, pDns), NULL, 0},
    {"dnsPsDns", NULL, VALUE, TIPADDR, OFFANDSIZE(DnsAgencyProfile, sDns), NULL, 0},
	CONFIGEND,
};
#endif

#if (FEATURE_SW_PORT_CONF == FYES)
static struct st_xmlConfig swPortConfConfig[] = {
    {"active",      NULL, VALUE, TENUM, OFFANDSIZE(SwPortConfProfile, head.active),  (Pointer)yesnoEnum, 0},
    {"portName",    NULL, VALUE, TSTR,  OFFANDSIZE(SwPortConfProfile, name),         NULL, 0},
#ifdef CONFIG_VSC7390
    {"maxFrame",    NULL, VALUE, TINT,  OFFANDSIZE(SwPortConfProfile, max_frame),    NULL, 0},
#endif
    {"portMode",    NULL, VALUE, TENUM, OFFANDSIZE(SwPortConfProfile, port_mode),    (Pointer)portModeEnum, 0},
    {"flowControl", NULL, VALUE, TENUM, OFFANDSIZE(SwPortConfProfile, flow_control), (Pointer)yesnoEnum, 0},
    CONFIGEND
};
#endif

#if (FEATURE_PORT_VLAN == FYES)
static struct st_xmlConfig  portVlanConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(PortVlanProfile, head.active), (Pointer)yesnoEnum, 0},
    {"description", NULL, VALUE, TSTR, OFFANDSIZE(PortVlanProfile, description), 0, 0},
    {"members", NULL, VALUE, TMEMBERS, OFFANDSIZE(PortVlanProfile, members), 0, 0},
    CONFIGEND,
};
#endif

#if (PVLAN_REDUCED == FYES)
static struct st_xmlConfig  rddPortVlanConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(RddPortVlanProfile, head.active), (Pointer)yesnoEnum, 0},
    {"description", NULL, VALUE, TSTR, OFFANDSIZE(RddPortVlanProfile, description), 0, 0},
    {"members", NULL, VALUE, TMEMBERS, OFFANDSIZE(RddPortVlanProfile, members), 0, 0},
    CONFIGEND,
};
#endif

#if (L2TP_SERVER == FYES)
static struct st_xmlConfig  l2tpSrvConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(L2TPSrvGlobalProfile, head.active), (Pointer)yesnoEnum, 0},
    {"authMode", NULL, VALUE, TENUM, OFFANDSIZE(L2TPSrvGlobalProfile, pppAuthMode), (Pointer)pppAuthEnum, 0},
    {"poolIpStart", NULL, VALUE, TIPADDR, OFFANDSIZE(L2TPSrvGlobalProfile, poolIpStart), NULL, 0},
    {"poolIpCnt", NULL, VALUE, TINT, OFFANDSIZE(L2TPSrvGlobalProfile, poolIpCnt), NULL, 0},
    {"localIp", NULL, VALUE, TIPADDR, OFFANDSIZE(L2TPSrvGlobalProfile, localIp), NULL, 0},
    {"listenIp", NULL, VALUE, TIPADDR, OFFANDSIZE(L2TPSrvGlobalProfile, listenIp), NULL, 0},
    {"priDns", NULL, VALUE, TIPADDR, OFFANDSIZE(L2TPSrvGlobalProfile, priDns), NULL, 0},
    {"secDns", NULL, VALUE, TIPADDR, OFFANDSIZE(L2TPSrvGlobalProfile, secDns), NULL, 0},
    CONFIGEND
};

static struct st_xmlConfig  l2tpSrvAccConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(L2TPSrvAccProfile, head.active), (Pointer)yesnoEnum, 0},
    {"peerType", NULL, VALUE, TENUM, OFFANDSIZE(L2TPSrvAccProfile, peerType), (Pointer)vpnPeerTypeEnum, 0},
    {"userName", NULL, VALUE, TSTR, OFFANDSIZE(L2TPSrvAccProfile, userName), NULL, 0},
    {"passwd", NULL, VALUE, TSTR, OFFANDSIZE(L2TPSrvAccProfile, passwd), NULL, 0},
    {"remoteLanIpDis", NULL, VALUE, TIPADDR, OFFANDSIZE(L2TPSrvAccProfile, remoteLanIpDis), NULL, 0},
    {"remoteLanIp", NULL, VALUE, TIPADDR, OFFANDSIZE(L2TPSrvAccProfile, remoteLanIp), NULL, 0},
    {"remoteLanNetmask", NULL, VALUE, TIPADDR, OFFANDSIZE(L2TPSrvAccProfile, remoteLanNetmask), NULL, 0},
    CONFIGEND,
};
#endif

#if (L2TP_CLIENT == FYES)                                                                                                            
static struct st_xmlConfig  l2tpCliConfig[] = {                                                                                      
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(L2TPCliProfile, head.active), (Pointer)yesnoEnum, 0},                                  
    {"userName", NULL, VALUE, TSTR, OFFANDSIZE(L2TPCliProfile, userName), NULL, 0},                                                  
    {"passwd", NULL, VALUE, TSTR, OFFANDSIZE(L2TPCliProfile, passwd), NULL, 0},                                                      
    {"authMode", NULL, VALUE, TENUM, OFFANDSIZE(L2TPCliProfile, pppAuthMode), (Pointer)pppAuthEnum, 0},                              
    {"remoteLanIpDis", NULL, VALUE, TIPADDR, OFFANDSIZE(L2TPCliProfile, remoteLanIpDis), NULL, 0},                                   
    {"remoteLanIp", NULL, VALUE, TIPADDR, OFFANDSIZE(L2TPCliProfile, remoteLanIp), NULL, 0},                                         
    {"remoteLanNetmask", NULL, VALUE, TIPADDR, OFFANDSIZE(L2TPCliProfile, remoteLanNetmask), NULL, 0},                               
    {"serverHost", NULL, VALUE, TSTR, OFFANDSIZE(L2TPCliProfile, serverHost), NULL, 0},                                              
    {"mtu", NULL, VALUE, TINT, OFFANDSIZE(L2TPCliProfile, mtu), NULL, 0},                                                            
#if (VPN_PROXY == FYES)
    {"vpnMode", NULL, VALUE, TINT, OFFANDSIZE(L2TPCliProfile, vpnMode), NULL, 0},                                                            
#endif
    CONFIGEND                                                                                                                        
};                                                                                                                                   
#endif                                                                                                                               

#if (PPTP_SERVER == FYES)
static struct st_xmlConfig  pptpSrvConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(PPTPSrvGlobalProfile, head.active), (Pointer)yesnoEnum, 0},
    {"authMode", NULL, VALUE, TENUM, OFFANDSIZE(PPTPSrvGlobalProfile, pppAuthMode), (Pointer)pppAuthEnum, 0},
    {"poolIpStart", NULL, VALUE, TIPADDR, OFFANDSIZE(PPTPSrvGlobalProfile, poolIpStart), NULL, 0},
    {"poolIpCnt", NULL, VALUE, TINT, OFFANDSIZE(PPTPSrvGlobalProfile, poolIpCnt), NULL, 0},
    {"localIp", NULL, VALUE, TIPADDR, OFFANDSIZE(PPTPSrvGlobalProfile, localIp), NULL, 0},
    {"listenIp", NULL, VALUE, TIPADDR, OFFANDSIZE(PPTPSrvGlobalProfile, listenIp), NULL, 0},
    {"priDns", NULL, VALUE, TIPADDR, OFFANDSIZE(PPTPSrvGlobalProfile, priDns), NULL, 0},
    {"secDns", NULL, VALUE, TIPADDR, OFFANDSIZE(PPTPSrvGlobalProfile, secDns), NULL, 0},
#if (PPTP_ENCRYPT_MODE == FYES)
    {"EncryptionMode", NULL, VALUE, TSTR, OFFANDSIZE(PPTPSrvGlobalProfile, EncryptionMode), NULL, 0},
#endif
    {"mtu", NULL, VALUE, TINT, OFFANDSIZE(PPTPSrvGlobalProfile, mtu), NULL, 0},
    CONFIGEND
};

static struct st_xmlConfig  pptpSrvAccConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(PPTPSrvAccProfile, head.active), (Pointer)yesnoEnum, 0},
    {"peerType", NULL, VALUE, TENUM, OFFANDSIZE(PPTPSrvAccProfile, peerType), (Pointer)vpnPeerTypeEnum, 0},
    {"userName", NULL, VALUE, TSTR, OFFANDSIZE(PPTPSrvAccProfile, userName), NULL, 0},
    {"passwd", NULL, VALUE, TSTR, OFFANDSIZE(PPTPSrvAccProfile, passwd), NULL, 0},
#if (PPTP_IP_BIND == FYES)
    {"bindIp", NULL, VALUE, TIPADDR, OFFANDSIZE(PPTPSrvAccProfile, bindIp), NULL, 0},
#endif
#if(HARD_FEATURE == FYES)
    {"hardfeature", NULL, VALUE, TMACADDR, OFFANDSIZE(PPTPSrvAccProfile, hardF), NULL, 0},
#endif
    {"remoteLanIpDis", NULL, VALUE, TIPADDR, OFFANDSIZE(PPTPSrvAccProfile, remoteLanIpDis), NULL, 0},
    {"remoteLanIp", NULL, VALUE, TIPADDR, OFFANDSIZE(PPTPSrvAccProfile, remoteLanIp), NULL, 0},
    {"remoteLanNetmask", NULL, VALUE, TIPADDR, OFFANDSIZE(PPTPSrvAccProfile, remoteLanNetmask), NULL, 0},
    CONFIGEND,
};
#endif

#if (PPTP_CLIENT == FYES)
static struct st_xmlConfig pptpCliConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(PPTPCliProfile, head.active), (Pointer)yesnoEnum, 0},
    {"Natenable", NULL, VALUE, TENUM, OFFANDSIZE(PPTPCliProfile, Natenable), (Pointer)yesnoEnum, 0},
    {"userName", NULL, VALUE, TSTR, OFFANDSIZE(PPTPCliProfile, userName), NULL, 0},
    {"passwd", NULL, VALUE, TSTR, OFFANDSIZE(PPTPCliProfile, passwd), NULL, 0},
    {"authMode", NULL, VALUE, TENUM, OFFANDSIZE(PPTPCliProfile, pppAuthMode), (Pointer)pppAuthEnum, 0},
#if (PPTP_ENCRYPT_MODE == FYES)
    {"EncryptionMode", NULL, VALUE, TSTR, OFFANDSIZE(PPTPCliProfile, EncryptionMode), NULL, 0},
#endif
    {"remoteLanIpDis", NULL, VALUE, TIPADDR, OFFANDSIZE(PPTPCliProfile, remoteLanIpDis), NULL, 0},
    {"remoteLanIp", NULL, VALUE, TIPADDR, OFFANDSIZE(PPTPCliProfile, remoteLanIp), NULL, 0},
    {"remoteLanNetmask", NULL, VALUE, TIPADDR, OFFANDSIZE(PPTPCliProfile, remoteLanNetmask), NULL, 0},
    {"serverHost", NULL, VALUE, TSTR, OFFANDSIZE(PPTPCliProfile, serverHost), NULL, 0},
    {"mtu", NULL, VALUE, TINT, OFFANDSIZE(PPTPCliProfile, mtu), NULL, 0},
#if (VPN_PROXY == FYES)
    {"vpnMode", NULL, VALUE, TINT, OFFANDSIZE(PPTPCliProfile, vpnMode), NULL, 0},                                                            
#endif
    CONFIGEND
};
#endif

#if (FEATURE_SW_PORT_MIRROR == FYES)
static struct st_xmlConfig  portMirrorConfig[] = {
	{"active", NULL, VALUE, TENUM, OFFANDSIZE(MirrorProfile, head.active), (Pointer)yesnoEnum, 0},
	{"mirrorMode", NULL, VALUE, TINT, OFFANDSIZE(MirrorProfile, mirrorMode), NULL, 0},
	{"monitoredPort", NULL, VALUE, TMEMBERS, OFFANDSIZE(MirrorProfile, monitoredPort), NULL, 0},
	{"mirrorPort", NULL, VALUE, TINT, OFFANDSIZE(MirrorProfile, mirrorPort), NULL, 0},
	CONFIGEND,
};
#endif
#if (FEATURE_SW_PORT_TRUNK == FYES)
static struct st_xmlConfig portTrunkConfig[] = {
	{"active", NULL, VALUE, TENUM, OFFANDSIZE(TrunkProfile, head.active), (Pointer)yesnoEnum, 0},
	{"tMode", NULL, VALUE, TINT, OFFANDSIZE(TrunkProfile, tMode), NULL, 0},
	{"tGroup", NULL, VALUE, TINT, OFFANDSIZE(TrunkProfile, tGroup), NULL, 0},
	{"tName", NULL, VALUE, TSTR, OFFANDSIZE(TrunkProfile, tName), NULL, 0},
	{"tMembers", NULL, VALUE, TMEMBERS, OFFANDSIZE(TrunkProfile, tMembers), NULL, 0},
	CONFIGEND,
};
#endif

#if (FEATURE_SW_SNMP == FYES)
static struct st_xmlConfig snmpConfig[] = {
	{"active", NULL, VALUE, TENUM, OFFANDSIZE(SnmpProfile, head.active), (Pointer)yesnoEnum, 0},
	{"snmp_enable", NULL, VALUE, TINT, OFFANDSIZE(SnmpProfile, snmp_enable), NULL, 0},
	{"trap_ip", NULL, VALUE, TSTR, OFFANDSIZE(SnmpProfile, trap_ip), NULL, 0},
	{"r_comunity", NULL, VALUE, TSTR, OFFANDSIZE(SnmpProfile, r_community), NULL, 0},
	{"w_community", NULL, VALUE, TSTR, OFFANDSIZE(SnmpProfile, w_community), NULL, 0},
	{"trap_community", NULL, VALUE, TSTR, OFFANDSIZE(SnmpProfile, trap_community), NULL, 0},
	CONFIGEND,
};
#endif
#if (SNMP == FYES)
static struct st_xmlConfig snmpInfoConfig[] = {
	{"active", NULL, VALUE, TENUM, OFFANDSIZE(SnmpInfoProfile, head.active), (Pointer)yesnoEnum, 0},
	{"snmp_enable", NULL, VALUE, TINT, OFFANDSIZE(SnmpInfoProfile, snmp_enable), NULL, 0},
	{"comunity", NULL, VALUE, TSTR, OFFANDSIZE(SnmpInfoProfile, community), NULL, 0},
	{"dev_name", NULL, VALUE, TSTR, OFFANDSIZE(SnmpInfoProfile,dev_name), NULL, 0},
	{"contactor", NULL, VALUE, TSTR, OFFANDSIZE(SnmpInfoProfile,contactor), NULL, 0},
	{"position", NULL, VALUE, TSTR, OFFANDSIZE(SnmpInfoProfile,position), NULL, 0},
	{"host_manage_enable", NULL, VALUE, TINT, OFFANDSIZE(SnmpInfoProfile, host_manage_enable), NULL, 0},
	{"host1", NULL, VALUE, TSTR, OFFANDSIZE(SnmpInfoProfile, host1), NULL, 0},
	{"host2", NULL, VALUE, TSTR, OFFANDSIZE(SnmpInfoProfile, host2), NULL, 0},
	{"host3", NULL, VALUE, TSTR, OFFANDSIZE(SnmpInfoProfile, host3), NULL, 0},
	CONFIGEND,
};
#endif

#if (AP_PERMISSION == FYES)
static struct st_xmlConfig ApPermissionConfig[] = {
	{"active", NULL, VALUE, TENUM, OFFANDSIZE(ApPermissionProfile, head.active), (Pointer)yesnoEnum, 0},
    {"ap_name", NULL, VALUE, TSTR, OFFANDSIZE(ApPermissionProfile, ap_name), NULL, 0},
	{"num_start", NULL, VALUE, TINT, OFFANDSIZE(ApPermissionProfile, num_start), NULL, 0},
	{"num_end", NULL, VALUE, TINT, OFFANDSIZE(ApPermissionProfile, num_end), NULL, 0},
	{"action", NULL, VALUE, TENUM, OFFANDSIZE(ApPermissionProfile, action), (Pointer)enDisableEnum, 0},
	CONFIGEND,
};
#endif

#if (AP_CONF == FYES)
static struct st_xmlConfig apConfConfig[] = {
	{"active", NULL, VALUE, TENUM, OFFANDSIZE(ApConfProfile, head.active), (Pointer)yesnoEnum, 0},
    {"apTypes", NULL, VALUE, TSTR, OFFANDSIZE(ApConfProfile, types), NULL, 0},
    {"softVersion", NULL, VALUE, TSTR, OFFANDSIZE(ApConfProfile, softVersion), NULL, 0},
    {"firmVersion", NULL, VALUE, TSTR, OFFANDSIZE(ApConfProfile, firmVersion), NULL, 0},
	{"serial", NULL, VALUE, TINT, OFFANDSIZE(ApConfProfile, serial), NULL, 0},
	CONFIGEND,
};
#endif

#if (SERVICE_ZONE == FYES)
static struct st_xmlConfig authWpaPskConfig[] = {
	{"wVersion", NULL, VALUE, TENUM, OFFANDSIZE(ServiceZoneProfile, wpaPskSecurity.wVersion), (Pointer)wVersionEnum, 0},
	{"sAlg", NULL, VALUE, TENUM, OFFANDSIZE(ServiceZoneProfile, wpaPskSecurity.sAlg), (Pointer)sAlgEnum, 0},
	{"sharePasswd", NULL, VALUE, TSTR, OFFANDSIZE(ServiceZoneProfile, wpaPskSecurity.sharePasswd), NULL, 0},
	{"updataTime", NULL, VALUE, TINT, OFFANDSIZE(ServiceZoneProfile, wpaPskSecurity.updataTime), NULL, 0},
	CONFIGEND,
};

static struct st_xmlConfig authWpaConfig[] = {
	{"wVersion", NULL, VALUE, TENUM, OFFANDSIZE(ServiceZoneProfile, wpaSecurity.wVersion), (Pointer)wVersionEnum, 0},
	{"sAlg", NULL, VALUE, TENUM, OFFANDSIZE(ServiceZoneProfile, wpaSecurity.sAlg), (Pointer)sAlgEnum, 0},
	{"radiusIp", NULL, VALUE, TIPADDR, OFFANDSIZE(ServiceZoneProfile, wpaSecurity.radiusIp), NULL, 0},
	{"radiusPort", NULL, VALUE, TINT, OFFANDSIZE(ServiceZoneProfile, wpaSecurity.radiusPort), NULL, 0},
	{"radiusPasswd", NULL, VALUE, TSTR, OFFANDSIZE(ServiceZoneProfile, wpaSecurity.radiusPasswd), NULL, 0},
	{"updataTime", NULL, VALUE, TINT, OFFANDSIZE(ServiceZoneProfile, wpaSecurity.updataTime), NULL, 0},
	CONFIGEND,
};

static struct st_xmlConfig keyNumConfig[] = {
	{"key_state_action", NULL, VALUE, TENUM, OFFANDSIZE(ServiceZoneProfile, wepSecurity.kNum1.key_state_action), (Pointer)yesnoEnum, 0},
	{"passwd", NULL, VALUE, TSTR, OFFANDSIZE(ServiceZoneProfile, wepSecurity.kNum1.passwd), NULL, 0},
	{"kFmt", NULL, VALUE, TENUM, OFFANDSIZE(ServiceZoneProfile, wepSecurity.kNum1.kFmt), (Pointer)kFmtEnum, 0},
	CONFIGEND,
};

static struct st_xmlConfig authWepConfig[] = {
	{"auMethod", NULL, VALUE, TENUM, OFFANDSIZE(ServiceZoneProfile, wepSecurity.auMethod), (Pointer)authMethodEnum, 0},
	{"kType", NULL, VALUE, TENUM, OFFANDSIZE(ServiceZoneProfile, wepSecurity.kType), (Pointer)keyTypeEnum, 0},
	{"kNum1", keyNumConfig, 0, 0, 0, 0, 0},
	{"kNum2", keyNumConfig, 0, 0, 0, 0, 0},
	{"kNum3", keyNumConfig, 0, 0, 0, 0, 0},
	{"kNum4", keyNumConfig, 0, 0, 0, 0, 0},
	CONFIGEND,
};

static struct st_xmlConfig ServiceZoneConfig[] = {
	{"active", NULL, VALUE, TENUM, OFFANDSIZE(ServiceZoneProfile, head.active), (Pointer)yesnoEnum, 0},
	{"serviceEn", NULL, VALUE, TENUM, OFFANDSIZE(ServiceZoneProfile, serviceEn), (Pointer)yesnoEnum, 0},
	{"ssid", NULL, VALUE, TSTR, OFFANDSIZE(ServiceZoneProfile, ssid), NULL, 0},
#if(SSID_ENCODE_TYPE == FYES)
	{"ssidEncodeType", NULL, VALUE, TINT, OFFANDSIZE(ServiceZoneProfile, ssid_encode_type), NULL, 0},
#endif
#if (SZ_SHARE_LIMIT_AC == FYES)
	{"limitType", NULL, VALUE, TINT, OFFANDSIZE(ServiceZoneProfile, limit_type), NULL, 0},
	{"limitDown", NULL, VALUE, TINT, OFFANDSIZE(ServiceZoneProfile, limit_down), NULL, 0},
	{"limitUp", NULL, VALUE, TINT, OFFANDSIZE(ServiceZoneProfile, limit_up), NULL, 0},
#endif
	{"ssidBcast", NULL, VALUE, TENUM, OFFANDSIZE(ServiceZoneProfile, ssidBcast), (Pointer)yesnoEnum, 0},
	{"ssidIsolate", NULL, VALUE, TENUM, OFFANDSIZE(ServiceZoneProfile, ssidIsolate), (Pointer)yesnoEnum, 0},
	{"strApVid", NULL, VALUE, TSTR, OFFANDSIZE(ServiceZoneProfile, strApVid), NULL, 0},
	{"vlanType", NULL, VALUE, TINT, OFFANDSIZE(ServiceZoneProfile, vlanType), NULL, 0},
	{"vid", NULL, VALUE, TINT, OFFANDSIZE(ServiceZoneProfile, vid), NULL, 0},
	{"client", NULL, VALUE, TINT, OFFANDSIZE(ServiceZoneProfile, client), NULL, 0},
	{"gw", NULL, VALUE, TIPADDR, OFFANDSIZE(ServiceZoneProfile, gw), NULL, 0},
	{"netmask", NULL, VALUE, TIPADDR, OFFANDSIZE(ServiceZoneProfile, netmask), NULL, 0},
	{"virif", NULL, VALUE, TIPADDR, OFFANDSIZE(ServiceZoneProfile, virif), NULL, 0},
	{"dhcpEn", NULL, VALUE, TENUM, OFFANDSIZE(ServiceZoneProfile, dhcpEn), (Pointer)yesnoEnum, 0},
	{"dhcpPool", NULL, VALUE, TSTR, OFFANDSIZE(ServiceZoneProfile, dhcpPool), NULL, 0},
	{"securMode", NULL, VALUE, TENUM, OFFANDSIZE(ServiceZoneProfile, securMode), (Pointer)uthModeEnum, 0},
	{"wepSecurity", authWepConfig, 0, 0, 0, 0, 0},
	{"wpaSecurity", authWpaConfig, 0, 0, 0, 0, 0},
	{"wpaPskSecurity", authWpaPskConfig, 0, 0, 0, 0, 0},
	CONFIGEND,
};
#endif
#if((FEATURE_AP_MAC_FILTER == FYES)||(FIT_AP_MAC_FILTER == FYES))
static struct st_xmlConfig ApMacFilterConfig[] = {
	{"active", NULL, VALUE, TENUM, OFFANDSIZE(ApMacFilterProfile, head.active), (Pointer)yesnoEnum, 0},
	{"name", NULL, VALUE, TSTR, OFFANDSIZE(ApMacFilterProfile, head.name), NULL, 0},
	{"sz1", NULL, VALUE, TSTR, OFFANDSIZE(ApMacFilterProfile, sz[0]), NULL, 0},
	{"sz2", NULL, VALUE, TSTR, OFFANDSIZE(ApMacFilterProfile, sz[1]), NULL, 0},
	{"sz3", NULL, VALUE, TSTR, OFFANDSIZE(ApMacFilterProfile, sz[2]), NULL, 0},
	{"sz4", NULL, VALUE, TSTR, OFFANDSIZE(ApMacFilterProfile, sz[3]), NULL, 0},
	{"sz5", NULL, VALUE, TSTR, OFFANDSIZE(ApMacFilterProfile, sz[4]), NULL, 0},
	{"sz6", NULL, VALUE, TSTR, OFFANDSIZE(ApMacFilterProfile, sz[5]), NULL, 0},
	{"sz7", NULL, VALUE, TSTR, OFFANDSIZE(ApMacFilterProfile, sz[6]), NULL, 0},
	{"sz8", NULL, VALUE, TSTR, OFFANDSIZE(ApMacFilterProfile, sz[7]), NULL, 0},
	{"sz9", NULL, VALUE, TSTR, OFFANDSIZE(ApMacFilterProfile, sz[8]), NULL, 0},
	{"sz10", NULL, VALUE, TSTR, OFFANDSIZE(ApMacFilterProfile, sz[9]), NULL, 0},
	{"mode", NULL, VALUE, TINT, OFFANDSIZE(ApMacFilterProfile, mode), NULL, 0},
	{"macnum", NULL, VALUE, TINT, OFFANDSIZE(ApMacFilterProfile, num), NULL, 0},
	{"mac1", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[0]), 0, 0},
	{"mac2", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[1]), 0, 0},
	{"mac3", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[2]), 0, 0},
	{"mac4", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[3]), 0, 0},
	{"mac5", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[4]), 0, 0},
	{"mac6", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[5]), 0, 0},
	{"mac7", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[6]), 0, 0},
	{"mac8", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[7]), 0, 0},
	{"mac9", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[8]), 0, 0},
	{"mac10", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile,mac[9]), 0, 0},
	{"mac11", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[10]), 0, 0},
	{"mac12", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[11]), 0, 0},
	{"mac13", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[12]), 0, 0},
	{"mac14", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[13]), 0, 0},
	{"mac15", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[14]), 0, 0},
	{"mac16", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[15]), 0, 0},
	{"mac17", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[16]), 0, 0},
	{"mac18", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[17]), 0, 0},
	{"mac19", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[18]), 0, 0},
	{"mac20", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[19]), 0, 0},
	{"mac21", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[20]), 0, 0},
	{"mac22", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[21]), 0, 0},
	{"mac23", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[22]), 0, 0},
	{"mac24", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[23]), 0, 0},
	{"mac25", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[24]), 0, 0},
	{"mac26", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[25]), 0, 0},
	{"mac27", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[26]), 0, 0},
	{"mac28", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[27]), 0, 0},
	{"mac29", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[28]), 0, 0},
	{"mac30", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[29]), 0, 0},
	{"mac31", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[30]), 0, 0},
	{"mac32", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[31]), 0, 0},
#if 0
	{"mac33", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[32]), 0, 0},
	{"mac34", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[33]), 0, 0},
	{"mac35", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[34]), 0, 0},
	{"mac36", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[35]), 0, 0},
	{"mac37", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[36]), 0, 0},
	{"mac38", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[37]), 0, 0},
	{"mac39", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[38]), 0, 0},
	{"mac40", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[39]), 0, 0},
	{"mac41", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[40]), 0, 0},
	{"mac42", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[41]), 0, 0},
	{"mac43", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[42]), 0, 0},
	{"mac44", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[43]), 0, 0},
	{"mac45", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[44]), 0, 0},
	{"mac46", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[45]), 0, 0},
	{"mac47", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[46]), 0, 0},
	{"mac48", NULL, VALUE, TMACADDR, OFFANDSIZE(ApMacFilterProfile, mac[47]), 0, 0},
#endif
	CONFIGEND,
};
#endif

#if (TAG_VLAN == FYES)
static struct st_xmlConfig TagVlanConfig[] = {
	{"active", NULL, VALUE, TENUM, OFFANDSIZE(TagVlanProfile, head.active), (Pointer)yesnoEnum, 0},
	{"vid", NULL, VALUE, TINT, OFFANDSIZE(TagVlanProfile, vid), NULL, 0},
	{"gw", NULL, VALUE, TIPADDR, OFFANDSIZE(TagVlanProfile, gw), NULL, 0},
	{"netmask", NULL, VALUE, TIPADDR, OFFANDSIZE(TagVlanProfile, netmask), NULL, 0},
	{"dhcpEn", NULL, VALUE, TENUM, OFFANDSIZE(TagVlanProfile, dhcpEn), (Pointer)yesnoEnum, 0},
	{"dhcpPool", NULL, VALUE, TSTR, OFFANDSIZE(TagVlanProfile, dhcpPool), NULL, 0},
	{"mbrmsk", NULL, VALUE, TINT, OFFANDSIZE(TagVlanProfile, mbrmsk), NULL, 0},
	{"untagmsk", NULL, VALUE, TINT, OFFANDSIZE(TagVlanProfile, untagmsk), NULL, 0},
	{"cc_vlan", NULL, VALUE, TINT, OFFANDSIZE(TagVlanProfile, cc_vlan), NULL, 0},
	CONFIGEND,
};
#if (WAN_TAG_VLAN == FYES)
static struct st_xmlConfig WanTagVlanConfig[] = {
	{"active", NULL, VALUE, TENUM, OFFANDSIZE(WanTagVlanProfile, head.active), (Pointer)yesnoEnum, 0},
	{"vid", NULL, VALUE, TINT, OFFANDSIZE(WanTagVlanProfile, vid), NULL, 0},
	{"tag", NULL, VALUE, TINT, OFFANDSIZE(WanTagVlanProfile, tag), NULL, 0},
	CONFIGEND,
};
#endif
#endif

#if (TASK_SCHEDULER == FYES)
static struct st_xmlConfig taskSchedConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(TaskSchedProfile, head.active), (Pointer)yesnoEnum, 0},
    {"pMin", NULL, VALUE, TSTR, OFFANDSIZE(TaskSchedProfile, pMin), NULL, 0},
    {"pHour", NULL, VALUE, TSTR, OFFANDSIZE(TaskSchedProfile, pHour), NULL, 0},
    {"pWeek", NULL, VALUE, TSTR, OFFANDSIZE(TaskSchedProfile, pWeek), NULL, 0},
    {"pType", NULL, VALUE, TSTR, OFFANDSIZE(TaskSchedProfile, pType), NULL, 0},
    {"pCmd", NULL, VALUE, TSTR, OFFANDSIZE(TaskSchedProfile, pCmd), NULL, 0},
    CONFIGEND,
};
#endif

#if (GROUP_BASE == FYES)
static struct st_xmlConfig  NetManageConfig[] = {
	{"active", NULL, VALUE, TENUM, OFFANDSIZE(GroupBaseProfile, head.active), (Pointer)yesnoEnum, 0},
	{"groupType", NULL, VALUE, TENUM, OFFANDSIZE(GroupBaseProfile, groupType), (Pointer)groupBaseTypeEnum, 0},
	{"addrTypeEn", NULL, VALUE, TENUM, OFFANDSIZE(GroupBaseProfile, addrTypeEn), (Pointer)addrObjTypeEnum, 0},
	{"dst_addrTypeEn", NULL, VALUE, TENUM, OFFANDSIZE(GroupBaseProfile, dst_addrTypeEn), (Pointer)addrObjTypeEnum, 0},
	{"ipStart", NULL, VALUE, TIPADDR, OFFANDSIZE(GroupBaseProfile, ipStart), NULL, 0},
	{"ipEnd", NULL, VALUE, TIPADDR, OFFANDSIZE(GroupBaseProfile, ipEnd), NULL, 0},
    {"dst_ipStart", NULL, VALUE, TIPADDR, OFFANDSIZE(GroupBaseProfile, dst_ipStart), NULL, 0},
	{"dst_ipEnd", NULL, VALUE, TIPADDR, OFFANDSIZE(GroupBaseProfile, dst_ipEnd), NULL, 0},
#if (MACADDR_ADD_LIMIT == FYES)
	{"mac_addr", NULL, VALUE, TMACADDR, OFFANDSIZE(GroupBaseProfile, mac_addr), NULL, 0},
#endif

#if (IP_GRP == FYES)
	{"ipGrpName", NULL, VALUE, TSTR, OFFANDSIZE(GroupBaseProfile, ipGrpName), NULL, 0},
	{"dst_ipGrpName", NULL, VALUE, TSTR, OFFANDSIZE(GroupBaseProfile, dst_ipGrpName), NULL, 0},
#endif
	{"day", NULL, VALUE, TINT, OFFANDSIZE(GroupBaseProfile, day), NULL, 0},
	{"timeStart", NULL, VALUE, TSTR, OFFANDSIZE(GroupBaseProfile, timeStart), NULL, 0},
	{"timeStop", NULL, VALUE, TSTR, OFFANDSIZE(GroupBaseProfile, timeStop), NULL, 0},
	{"pdbPolicy", NULL, VALUE, TSTR, OFFANDSIZE(GroupBaseProfile, pdbPolicy), NULL, 0},
	{"pdbPolicy1", NULL, VALUE, THEX, OFFANDSIZE(GroupBaseProfile, pdbPolicy1), NULL, 0},

	{"limitPolicy", NULL, VALUE, TENUM, OFFANDSIZE(GroupBaseProfile, limitPolicy), (Pointer)limitPolicyEnum, 0},
	{"uploadRate", NULL, VALUE, TINT, OFFANDSIZE(GroupBaseProfile, uRate), NULL, 0},
	{"downloadRate", NULL, VALUE, TINT, OFFANDSIZE(GroupBaseProfile, dRate), NULL, 0},
#if (APP_POLICY_REMOTE_UPDATE == FYES)
	{"appVersion", NULL, VALUE, TINT, OFFANDSIZE(GroupBaseProfile,appVersion), NULL, 0},
#endif
	CONFIGEND,
};
#endif

#if (P2P_LIMIT == FYES)
static struct st_xmlConfig p2pLimitConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(P2PLimitProfile, head.active), (Pointer)yesnoEnum, 0},
    {"limitPolicy", NULL, VALUE, TENUM, OFFANDSIZE(P2PLimitProfile, limitPolicy), (Pointer)p2pLimitEnum, 0},
    {"uploadRate", NULL, VALUE, TINT, OFFANDSIZE(P2PLimitProfile, uRate), NULL, 0},
    {"downloadRate", NULL, VALUE, TINT, OFFANDSIZE(P2PLimitProfile, dRate), NULL, 0},
#if (IP_GRP == FYES)
    {"ipGrpName", NULL, VALUE, TSTR, OFFANDSIZE(P2PLimitProfile, ipGrpName), NULL, 0},
#endif
    {"day", NULL, VALUE, TINT, OFFANDSIZE(P2PLimitProfile, day), NULL, 0},
    {"timeStart", NULL, VALUE, TSTR, OFFANDSIZE(P2PLimitProfile, timeStart), NULL, 0},
    {"timeStop", NULL, VALUE, TSTR, OFFANDSIZE(P2PLimitProfile, timeStop), NULL, 0},
    CONFIGEND,
};
#endif

#if (SYS_LOG_INFO == FYES)
static struct st_xmlConfig SysLogInfoConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(SysLogInfoProfile, head.active), (Pointer)yesnoEnum, 0},
#ifdef CONFIG_UTT_DHCP_COLLISION
    {"DhcpLogEnable", NULL, VALUE, TENUM, OFFANDSIZE(SysLogInfoProfile, DhcpLogEnable), (Pointer)enDisableEnum, 0},
#endif
#ifdef CONFIG_UTT_NOTICE_LOG
    {"NoticeLogEnable", NULL, VALUE, TENUM, OFFANDSIZE(SysLogInfoProfile, NoticeLogEnable), (Pointer)enDisableEnum, 0},
#endif
#ifdef CONFIG_UTT_ARP_COLLISION
    {"ArpLogEnable", NULL, VALUE, TENUM, OFFANDSIZE(SysLogInfoProfile, ArpLogEnable), (Pointer)enDisableEnum, 0},
#endif
    {"PppoeLogEnable", NULL, VALUE, TENUM, OFFANDSIZE(SysLogInfoProfile, PppoeLogEnable), (Pointer)enDisableEnum, 0},
    CONFIGEND,
};
#endif
#if (MAC_FILTER == FYES)
static struct st_xmlConfig macFilterGlobalConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(MacFilterGlobalProfile, head.active), (Pointer)yesnoEnum, 0},
    {"action", NULL, VALUE, TENUM, OFFANDSIZE(MacFilterGlobalProfile, action), (Pointer)MacFilterActionEnum, 0},
    CONFIGEND,
};
static struct st_xmlConfig macFilterConfig[] = {
    {"mac", NULL, VALUE, TMACADDR, OFFANDSIZE(MacFilterProfile, mac), NULL, 0},
    CONFIGEND,
};
#endif
#if (EASY_TASK == FYES)
static struct st_xmlConfig easyTaskConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(EasyTaskProfile, head.active), (Pointer)yesnoEnum, 0},
#if 0
#if (WLAN_WORK_TIME == FYES)
    {"safeToNet", NULL, VALUE, TENUM, OFFANDSIZE(EasyTaskProfile, safeToNet), (Pointer)enDisableEnum, 0},
#endif
#endif
    {"easyTaskNoticeEn", NULL, VALUE, TENUM, OFFANDSIZE(EasyTaskProfile, easyTaskNoticeEn), (Pointer)enDisableEnum, 0},
    CONFIGEND,
};
#endif
#if (UTTPDB == FYES)
static struct st_xmlConfig  PDB[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(PdbProfile, head.active), (Pointer)yesnoEnum, 0},
#if (APP_POLICY_REMOTE_UPDATE == FYES)
    {"policyVersion", NULL, VALUE, TINT, OFFANDSIZE(PdbProfile,policyVersion), NULL, 0},
#endif
	{"line1", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[0]), NULL, 0},
	{"line2", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[1]), NULL, 0},
	{"line3", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[2]), NULL, 0},
	{"line4", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[3]), NULL, 0},
	{"line5", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[4]), NULL, 0},
	{"line6", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[5]), NULL, 0},
	{"line7", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[6]), NULL, 0},
	{"line8", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[7]), NULL, 0},
	{"line9", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[8]), NULL, 0},
	{"line10", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[9]), NULL, 0},
	{"line11", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[10]), NULL, 0},
	{"line12", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[11]), NULL, 0},
	{"line13", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[12]), NULL, 0},
	{"line14", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[13]), NULL, 0},
	{"line15", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[14]), NULL, 0},
	{"line16", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[15]), NULL, 0},
	{"line17", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[16]), NULL, 0},
	{"line18", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[17]), NULL, 0},
	{"line19", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[18]), NULL, 0},
	{"line20", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[19]), NULL, 0},
	{"line21", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[20]), NULL, 0},
	{"line22", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[21]), NULL, 0},
	{"line23", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[22]), NULL, 0},
	{"line24", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[23]), NULL, 0},
	{"line25", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[24]), NULL, 0},
	{"line26", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[25]), NULL, 0},
	{"line27", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[26]), NULL, 0},
	{"line28", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[27]), NULL, 0},
	{"line29", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[28]), NULL, 0},
	{"line30", NULL, VALUE, TSTR, OFFANDSIZE(PdbProfile, line[29]), NULL, 0},
	CONFIGEND,
};
#endif
#if (STATIC_NAT == FYES)
static struct st_xmlConfig  StaticNatConfig[] = {
	{"active", NULL, VALUE, TENUM, OFFANDSIZE(StaticNatProfile, head.active), (Pointer)yesnoEnum, 0},
	{"protocol", NULL, VALUE, TINT, OFFANDSIZE(StaticNatProfile, protocol), NULL, 0},
	{"outPort", NULL, VALUE, TINT, OFFANDSIZE(StaticNatProfile, OutPort), NULL, 0},
	{"IP", NULL, VALUE, TIPADDR, OFFANDSIZE(StaticNatProfile, IP), NULL, 0},
	{"innerPort", NULL, VALUE, TINT, OFFANDSIZE(StaticNatProfile, InnerPort), NULL, 0},
	{"portNum", NULL, VALUE, TINT, OFFANDSIZE(StaticNatProfile, PortNum), NULL, 0},
	{"natBind", NULL, VALUE, TSTR, OFFANDSIZE(StaticNatProfile, NatBind), NULL, 0},
	CONFIGEND,
};
#endif
#if (NAT_RULE == FYES)
static struct st_xmlConfig  NatRuleConfig[] = {
	{"active", NULL, VALUE, TENUM, OFFANDSIZE(NatRuleProfile, head.active), (Pointer)yesnoEnum, 0},
	{"natruleType", NULL, VALUE, TINT, OFFANDSIZE(NatRuleProfile, NatruleType), NULL, 0},
	{"ethId", NULL, VALUE, TINT, OFFANDSIZE(NatRuleProfile, EthId), NULL, 0},
	{"outIp", NULL, VALUE, TIPADDR, OFFANDSIZE(NatRuleProfile, OutIp), NULL, 0},
	{"inFromIP", NULL, VALUE, TIPADDR, OFFANDSIZE(NatRuleProfile, InFromIP), NULL, 0},
	{"inEndIP", NULL, VALUE, TIPADDR, OFFANDSIZE(NatRuleProfile, InEndIP), NULL, 0},
	{"natBind", NULL, VALUE, TSTR, OFFANDSIZE(NatRuleProfile, NatBind), NULL, 0},
	CONFIGEND,
};
#endif
#if (DMZ == FYES)
static struct st_xmlConfig  DMZConfig[] = {
	{"active", NULL, VALUE, TENUM, OFFANDSIZE(DMZProfile, head.active), (Pointer)yesnoEnum, 0},
	{"dmzIP", NULL, VALUE, TIPADDR, OFFANDSIZE(DMZProfile, DMZIP), NULL, 0},
	CONFIGEND,
};
#endif
#if (FEATURE_ACL == FYES)
static struct st_xmlConfig aclConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(AclProfile, head.active), (Pointer)yesnoEnum, 0},
    {"action", NULL, VALUE, TENUM, OFFANDSIZE(AclProfile, action_type), (Pointer)acl_actions, 0},
    {"port", NULL, VALUE, TINT, OFFANDSIZE(AclProfile, source), NULL, 0},
    {"srcIP", NULL, VALUE, TIPADDR, OFFANDSIZE(AclProfile, sip), NULL, 0},
    {"srcIPMask", NULL, VALUE, TINT, OFFANDSIZE(AclProfile, sip_mask), NULL, 0},
    {"dstIP", NULL, VALUE, TIPADDR, OFFANDSIZE(AclProfile, dip), NULL, 0},
    {"dstIPMask", NULL, VALUE, TINT, OFFANDSIZE(AclProfile, dip_mask), NULL, 0},
    {"srcPort", NULL, VALUE, TINT, OFFANDSIZE(AclProfile, sport), NULL, 0},
    {"dstPort", NULL, VALUE, TINT, OFFANDSIZE(AclProfile, dport), NULL, 0},
    CONFIGEND,
};

static struct st_xmlConfig aclStatusConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(AclStatusProfile, head.active), (Pointer)yesnoEnum, 0},
    CONFIGEND,
};
#endif
#if (ADMIN_CONF == FYES)
static struct st_xmlConfig  adminConfConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(AdminConfProfile, head.active), (Pointer)yesnoEnum, 0},
    {"password", NULL, VALUE, TSTR, OFFANDSIZE(AdminConfProfile, password), NULL, 0},
	CONFIGEND
};
#endif
#if (DNS_FILTER == FYES)
static struct st_xmlConfig  dnsFilterConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(DnsFilterProfile, head.active), (Pointer)yesnoEnum, 0},
    {"domainName", NULL, VALUE, TSTR, OFFANDSIZE(DnsFilterProfile, domainName), NULL, 0},
	CONFIGEND
};
static struct st_xmlConfig  dnsFilterGlobalConfig[] = {
    {"addrTypeEn", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, dnsFilterControl.addrTypeEn), (Pointer)addrObjTypeEnum, 0},
    {"ipstart", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, dnsFilterControl.ipstart), NULL, 0},
    {"ipend", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, dnsFilterControl.ipend), NULL, 0},
#if (IP_GRP == FYES)
    {"ipGrpName", NULL, VALUE, TSTR, OFFANDSIZE(SystemProfile, dnsFilterControl.ipGrpName), NULL, 0},
#endif
#if (TIME_GRP == FYES)
    {"timeGrpName", NULL, VALUE, TSTR, OFFANDSIZE(SystemProfile, dnsFilterControl.timeGrpName), NULL, 0},
#endif
    {"filterType", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, dnsFilterControl.filterType), (Pointer)dnsFilterTypeEnum, 0},
	CONFIGEND
};
#endif

#if (CLI_FOR_ENGINEER_ONLY == FYES)
static struct st_xmlConfig cliForEngineerOnlyConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(CliForEngineerOnlyProfile, head.active), (Pointer)yesnoEnum, 0},
    {"command", NULL, VALUE, TSTR, OFFANDSIZE(CliForEngineerOnlyProfile, command), NULL, 0},
    CONFIGEND
};
#endif

#if (CONNECT_LIMIT == FYES)
static struct st_xmlConfig  connLimitConfig[] = {
    {"enable", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, connLimit.connLimitEn), (Pointer)enDisableEnum, 0},
    {"totalConnCnt", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, connLimit.totalConnCount), NULL, 0},
    {"TCPConnCnt", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, connLimit.TCPConnCount), NULL, 0},
    {"UDPConnCnt", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, connLimit.UDPConnCount), NULL, 0},
    {"ICMPConnCnt", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, connLimit.ICMPConnCount), NULL, 0},
    CONFIGEND
};
#endif
#if (TAG_VLAN == FYES)
static struct st_xmlConfig  pvidConfig[] = {
    {"pvid1", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, pvid.pvid1), NULL, 0},
    {"pvid2", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, pvid.pvid2), NULL, 0},
    {"pvid3", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, pvid.pvid3), NULL, 0},
    {"pvid4", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, pvid.pvid4), NULL, 0},
    CONFIGEND
};
#endif
#if (SSH_CLI == FYES)
static struct st_xmlConfig  SSHConfig[] = {
    {"enable", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, SSH.SSHEn), (Pointer)enDisableEnum, 0},
    {"Port", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, SSH.Port), NULL, 0},
    CONFIGEND
};
#endif
#if (HTTPS == FYES)
static struct st_xmlConfig  httpsConfig[] = {
    {"enable", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, sysHttps.httpsEn), (Pointer)enDisableEnum, 0},
    CONFIGEND
};
#endif
#if (EXCEPT_QQ == FYES)
static struct st_xmlConfig exceptQQConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(ExceptQQProfile, head.active), (Pointer)yesnoEnum, 0},
    {"qqHead", NULL, VALUE, TUINT, OFFANDSIZE(ExceptQQProfile, phoneHead), NULL, 0},
    {"qqNumber", NULL, VALUE, TUINT, OFFANDSIZE(ExceptQQProfile, qqNumber), NULL, 0},
    {"remark", NULL, VALUE, TSTR, OFFANDSIZE(ExceptQQProfile, remark), NULL, 0},
	CONFIGEND
};
#endif
#if (EXCEPT_MSN == FYES)
static struct st_xmlConfig exceptMSNConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(ExceptMSNProfile, head.active), (Pointer)yesnoEnum, 0},
    {"msnNumber", NULL, VALUE, TSTR, OFFANDSIZE(ExceptMSNProfile, msnNumber), NULL, 0},
    {"remark", NULL, VALUE, TSTR, OFFANDSIZE(ExceptMSNProfile, remark), NULL, 0},
	CONFIGEND
};
#endif
#if (EXCEPT_ALI == FYES)
static struct st_xmlConfig exceptAliConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(ExceptAliProfile, head.active), (Pointer)yesnoEnum, 0},
    {"aliNumber", NULL, VALUE, TSTR, OFFANDSIZE(ExceptAliProfile, aliNumber), NULL, 0},
    {"remark", NULL, VALUE, TSTR, OFFANDSIZE(ExceptAliProfile, remark), NULL, 0},
	CONFIGEND
};
#endif
#if (IP_GRP==FYES)
static struct st_xmlConfig ipGrpEntryConfig[] = {
    {"entry1", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[0]), NULL, 0},
    {"entry2", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[1]), NULL, 0},
    {"entry3", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[2]), NULL, 0},
    {"entry4", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[3]), NULL, 0},
    {"entry5", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[4]), NULL, 0},
    {"entry6", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[5]), NULL, 0},
    {"entry7", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[6]), NULL, 0},
    {"entry8", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[7]), NULL, 0},
    {"entry9", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[8]), NULL, 0},
    {"entry10", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[9]), NULL, 0},
    {"entry11", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[10]), NULL, 0},
    {"entry12", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[11]), NULL, 0},
    {"entry13", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[12]), NULL, 0},
    {"entry14", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[13]), NULL, 0},
    {"entry15", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[14]), NULL, 0},
    {"entry16", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[15]), NULL, 0},
    {"entry17", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[16]), NULL, 0},
    {"entry18", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[17]), NULL, 0},
    {"entry19", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[18]), NULL, 0},
    {"entry20", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[19]), NULL, 0},
    {"entry21", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[20]), NULL, 0},
    {"entry22", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[21]), NULL, 0},
    {"entry23", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[22]), NULL, 0},
    {"entry24", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[23]), NULL, 0},
    {"entry25", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[24]), NULL, 0},
    {"entry26", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[25]), NULL, 0},
    {"entry27", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[26]), NULL, 0},
    {"entry28", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[27]), NULL, 0},
    {"entry29", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[28]), NULL, 0},
    {"entry30", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[29]), NULL, 0},
    {"entry31", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[30]), NULL, 0},
    {"entry32", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[31]), NULL, 0},
    {"entry33", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[32]), NULL, 0},
    {"entry34", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[33]), NULL, 0},
    {"entry35", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[34]), NULL, 0},
    {"entry36", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[35]), NULL, 0},
    {"entry37", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[36]), NULL, 0},
    {"entry38", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[37]), NULL, 0},
    {"entry39", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[38]), NULL, 0},
    {"entry40", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[39]), NULL, 0},
    {"entry41", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[40]), NULL, 0},
    {"entry42", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[41]), NULL, 0},
    {"entry43", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[42]), NULL, 0},
    {"entry44", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[43]), NULL, 0},
    {"entry45", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[44]), NULL, 0},
    {"entry46", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[45]), NULL, 0},
    {"entry47", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[46]), NULL, 0},
    {"entry48", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[47]), NULL, 0},
    {"entry49", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[48]), NULL, 0},
    {"entry50", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[49]), NULL, 0},
    {"entry51", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[50]), NULL, 0},
    {"entry52", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[51]), NULL, 0},
    {"entry53", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[52]), NULL, 0},
    {"entry54", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[53]), NULL, 0},
    {"entry55", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[54]), NULL, 0},
    {"entry56", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[55]), NULL, 0},
    {"entry57", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[56]), NULL, 0},
    {"entry58", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[57]), NULL, 0},
    {"entry59", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[58]), NULL, 0},
    {"entry60", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[59]), NULL, 0},
    {"entry61", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[60]), NULL, 0},
    {"entry62", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[61]), NULL, 0},
    {"entry63", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[62]), NULL, 0},
    {"entry64", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[63]), NULL, 0},
    {"entry65", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[64]), NULL, 0},
    {"entry66", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[65]), NULL, 0},
    {"entry67", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[66]), NULL, 0},
    {"entry68", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[67]), NULL, 0},
    {"entry69", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[68]), NULL, 0},
    {"entry70", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[69]), NULL, 0},
    {"entry71", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[70]), NULL, 0},
    {"entry72", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[71]), NULL, 0},
    {"entry73", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[72]), NULL, 0},
    {"entry74", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[73]), NULL, 0},
    {"entry75", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[74]), NULL, 0},
    {"entry76", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[75]), NULL, 0},
    {"entry77", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[76]), NULL, 0},
    {"entry78", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[77]), NULL, 0},
    {"entry79", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[78]), NULL, 0},
    {"entry80", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[79]), NULL, 0},
    {"entry81", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[80]), NULL, 0},
    {"entry82", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[81]), NULL, 0},
    {"entry83", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[82]), NULL, 0},
    {"entry84", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[83]), NULL, 0},
    {"entry85", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[84]), NULL, 0},
    {"entry86", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[85]), NULL, 0},
    {"entry87", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[86]), NULL, 0},
    {"entry88", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[87]), NULL, 0},
    {"entry89", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[88]), NULL, 0},
    {"entry90", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[89]), NULL, 0},
    {"entry91", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[90]), NULL, 0},
    {"entry92", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[91]), NULL, 0},
    {"entry93", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[92]), NULL, 0},
    {"entry94", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[93]), NULL, 0},
    {"entry95", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[94]), NULL, 0},
    {"entry96", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[95]), NULL, 0},
    {"entry97", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[96]), NULL, 0},
    {"entry98", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[97]), NULL, 0},
    {"entry99", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[98]), NULL, 0},
    {"entry100", NULL, VALUE, TIPENTRY, OFFANDSIZE(IpGrpProfile, ipEntry[99]), NULL, 0},
    CONFIGEND
};

static struct st_xmlConfig ipGrpConfig[] = {
    {"active", NULL,VALUE, TENUM, OFFANDSIZE(IpGrpProfile, head.active), (Pointer)yesnoEnum, 0},
    {"groupType", NULL, VALUE, TENUM, OFFANDSIZE(IpGrpProfile, grpType), (Pointer)ipGrpTypeEnum, 0},
    {"entrys", ipGrpEntryConfig, 0, 0, 0, 0, 0},
	CONFIGEND
};
#endif
#if (PORT_GRP==FYES)
static struct st_xmlConfig portGrpEntryConfig[] = {
    {"entry1", NULL, VALUE, TPORTENTRY, OFFANDSIZE(PortGrpProfile, portEntry[0]), NULL, 0},
    {"entry2", NULL, VALUE, TPORTENTRY, OFFANDSIZE(PortGrpProfile, portEntry[1]), NULL, 0},
    {"entry3", NULL, VALUE, TPORTENTRY, OFFANDSIZE(PortGrpProfile, portEntry[2]), NULL, 0},
    {"entry4", NULL, VALUE, TPORTENTRY, OFFANDSIZE(PortGrpProfile, portEntry[3]), NULL, 0},
    {"entry5", NULL, VALUE, TPORTENTRY, OFFANDSIZE(PortGrpProfile, portEntry[4]), NULL, 0},
    {"entry6", NULL, VALUE, TPORTENTRY, OFFANDSIZE(PortGrpProfile, portEntry[5]), NULL, 0},
    {"entry7", NULL, VALUE, TPORTENTRY, OFFANDSIZE(PortGrpProfile, portEntry[6]), NULL, 0},
    {"entry8", NULL, VALUE, TPORTENTRY, OFFANDSIZE(PortGrpProfile, portEntry[7]), NULL, 0},
    {"entry9", NULL, VALUE, TPORTENTRY, OFFANDSIZE(PortGrpProfile, portEntry[8]), NULL, 0},
    {"entry10", NULL, VALUE, TPORTENTRY, OFFANDSIZE(PortGrpProfile, portEntry[9]), NULL, 0},
    CONFIGEND
};

static struct st_xmlConfig portGrpConfig[] = {
    {"active", NULL,VALUE, TENUM, OFFANDSIZE(PortGrpProfile, head.active), (Pointer)yesnoEnum, 0},
    {"groupType", NULL, VALUE, TENUM, OFFANDSIZE(PortGrpProfile, grpType), (Pointer)portGrpTypeEnum, 0},
    {"entrys", portGrpEntryConfig, 0, 0, 0, 0, 0},
	CONFIGEND
};
#endif
#if(TIME_GRP == FYES)
static struct st_xmlConfig timeGrpEntryConfig[] = {
    {"entry1En", NULL, VALUE, TENUM, OFFANDSIZE(TimeGrpProfile, timeEntry[0].enable), (Pointer)enDisableEnum, 0},
    {"entry1day", NULL, VALUE, TINT, OFFANDSIZE(TimeGrpProfile, timeEntry[0].day), NULL, 0},
#ifdef  CONFIG_PERFORMANCE_OPTIMIZATION
    {"entry1timestart", NULL, VALUE, TTIMESEC, OFFANDSIZE(TimeGrpProfile, timeEntry[0].timestart), NULL, 0},
    {"entry1timestop", NULL, VALUE, TTIMESEC, OFFANDSIZE(TimeGrpProfile, timeEntry[0].timestop), NULL, 0},
#else
    {"entry1timestart", NULL, VALUE, TSTR, OFFANDSIZE(TimeGrpProfile, timeEntry[0].timestart), NULL, 0},
    {"entry1timestop", NULL, VALUE, TSTR, OFFANDSIZE(TimeGrpProfile, timeEntry[0].timestop), NULL, 0},
#endif
    {"entry2En", NULL, VALUE, TENUM, OFFANDSIZE(TimeGrpProfile, timeEntry[1].enable), (Pointer)enDisableEnum, 0},
    {"entry2day", NULL, VALUE, TINT, OFFANDSIZE(TimeGrpProfile, timeEntry[1].day), NULL, 0},
#ifdef  CONFIG_PERFORMANCE_OPTIMIZATION
    {"entry2timestart", NULL, VALUE, TTIMESEC, OFFANDSIZE(TimeGrpProfile, timeEntry[1].timestart), NULL, 0},
    {"entry2timestop", NULL, VALUE, TTIMESEC, OFFANDSIZE(TimeGrpProfile, timeEntry[1].timestop), NULL, 0},
#else
    {"entry2timestart", NULL, VALUE, TSTR, OFFANDSIZE(TimeGrpProfile, timeEntry[1].timestart), NULL, 0},
    {"entry2timestop", NULL, VALUE, TSTR, OFFANDSIZE(TimeGrpProfile, timeEntry[1].timestop), NULL, 0},
#endif
    {"entry3En", NULL, VALUE, TENUM, OFFANDSIZE(TimeGrpProfile, timeEntry[2].enable), (Pointer)enDisableEnum, 0},
    {"entry3day", NULL, VALUE, TINT, OFFANDSIZE(TimeGrpProfile, timeEntry[2].day), NULL, 0},
#ifdef  CONFIG_PERFORMANCE_OPTIMIZATION
    {"entry3timestart", NULL, VALUE, TTIMESEC, OFFANDSIZE(TimeGrpProfile, timeEntry[2].timestart), NULL, 0},
    {"entry3timestop", NULL, VALUE, TTIMESEC, OFFANDSIZE(TimeGrpProfile, timeEntry[2].timestop), NULL, 0},
#else
    {"entry3timestart", NULL, VALUE, TSTR, OFFANDSIZE(TimeGrpProfile, timeEntry[2].timestart), NULL, 0},
    {"entry3timestop", NULL, VALUE, TSTR, OFFANDSIZE(TimeGrpProfile, timeEntry[2].timestop), NULL, 0},
#endif
	CONFIGEND
};
static struct st_xmlConfig timeGrpConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(TimeGrpProfile, head.active), (Pointer)yesnoEnum, 0},
#ifdef  CONFIG_PERFORMANCE_OPTIMIZATION
    {"datestart", NULL, VALUE, TTIMEDATE, OFFANDSIZE(TimeGrpProfile, datestart), NULL, 0},
    {"datestop", NULL, VALUE, TTIMEDATE, OFFANDSIZE(TimeGrpProfile, datestop), NULL, 0},
#else
    {"datestart", NULL, VALUE, TSTR, OFFANDSIZE(TimeGrpProfile, datestart), NULL, 0},
    {"datestop", NULL, VALUE, TSTR, OFFANDSIZE(TimeGrpProfile, datestop), NULL, 0},
#endif
    {"timentrys", timeGrpEntryConfig, 0, 0, 0, 0, 0},
	CONFIGEND
};
#endif
#if (SYSLOG == FYES)
static struct st_xmlConfig SyslogConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(SyslogProfile, head.active), (Pointer)yesnoEnum, 0},
    {"SyslogIp", NULL, VALUE, TSTR, OFFANDSIZE(SyslogProfile, SyslogIp), NULL, 0},
    {"SyslogPort", NULL, VALUE, TINT, OFFANDSIZE(SyslogProfile, SyslogPort), NULL, 0},
    {"SyslogType", NULL, VALUE, TSTR, OFFANDSIZE(SyslogProfile, SyslogType), NULL, 0},
#if (SYSLOG_HEARTBEAT == FYES)
    {"SyslogInterval", NULL, VALUE, TINT, OFFANDSIZE(SyslogProfile, SyslogInterval), NULL, 0},
#endif
	CONFIGEND
};
#endif
#if(IP_SEC == FYES)
static struct st_xmlConfig isakmpPolicyConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(isakmpPolicyProfile, head.active), (Pointer)yesnoEnum, 0},
    {"encrypt", NULL, VALUE, TENUM, OFFANDSIZE(isakmpPolicyProfile, encrypt), (Pointer)encryptEnum, 0},
    {"hash", NULL, VALUE, TENUM, OFFANDSIZE(isakmpPolicyProfile, hash), (Pointer)authWayEnum, 0},
    {"group", NULL, VALUE, TENUM, OFFANDSIZE(isakmpPolicyProfile, group), (Pointer)groupTypeEnum, 0},
	CONFIGEND
};
static struct st_xmlConfig isakmpConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(isakmpConfProfile, head.active), (Pointer)yesnoEnum, 0},
    {"remoteUser", NULL, VALUE, TSTR, OFFANDSIZE(isakmpConfProfile, remoteuser), NULL, 0},
    {"remoteUserTYpe", NULL, VALUE, TENUM, OFFANDSIZE(isakmpConfProfile, remoteType), (Pointer)UserTypeEnum, 0},
    {"localUser", NULL, VALUE, TSTR, OFFANDSIZE(isakmpConfProfile, localuser), NULL, 0},
    {"localUserType", NULL, VALUE, TENUM, OFFANDSIZE(isakmpConfProfile, localType), (Pointer)UserTypeEnum, 0},
    {"preshareKey", NULL, VALUE, TSTR, OFFANDSIZE(isakmpConfProfile, preshareKey), NULL, 0},
    {"mode", NULL, VALUE, TENUM, OFFANDSIZE(isakmpConfProfile, mode), (Pointer)negModeEnum, 0},
    {"aggersEncrypt", NULL, VALUE, TENUM, OFFANDSIZE(isakmpConfProfile, aggresEncrypt), (Pointer)enDisableEnum, 0},
    {"lifetime", NULL, VALUE, TINT, OFFANDSIZE(isakmpConfProfile, lifetime), NULL, 0},
    {"policy", NULL, VALUE, TSTR, OFFANDSIZE(isakmpConfProfile, policy), NULL, 0},
	CONFIGEND
};
static struct st_xmlConfig transformSetConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(ipsecTransformSetProfile, head.active), (Pointer)yesnoEnum, 0},
    {"encrypt", NULL, VALUE, TENUM, OFFANDSIZE(ipsecTransformSetProfile, encrypt), (Pointer)encryptEnum, 0},
    {"espAuth", NULL, VALUE, TENUM, OFFANDSIZE(ipsecTransformSetProfile, espAuth), (Pointer)authWayEnum, 0},
    {"auth", NULL, VALUE, TENUM, OFFANDSIZE(ipsecTransformSetProfile, auth), (Pointer)authWayEnum, 0},
	CONFIGEND
};
static struct st_xmlConfig ipsecConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(ipsecConfProfile, head.active), (Pointer)yesnoEnum, 0},
    {"allow", NULL, VALUE, TENUM, OFFANDSIZE(ipsecConfProfile, active), (Pointer)enDisableEnum, 0},
    {"connType", NULL, VALUE, TENUM, OFFANDSIZE(ipsecConfProfile, connType), (Pointer)ipsecConnTypeEnum, 0},
    {"peer", NULL, VALUE, TSTR, OFFANDSIZE(ipsecConfProfile, peer), NULL, 0},
    {"peerIP", NULL, VALUE, TSTR, OFFANDSIZE(ipsecConfProfile, peerIP), NULL, 0},
    {"destAddr", NULL, VALUE, TSTR, OFFANDSIZE(ipsecConfProfile, destAddr), NULL, 0},
    {"destMask", NULL, VALUE, TSTR, OFFANDSIZE(ipsecConfProfile, destMask), NULL, 0},
    {"profile", NULL, VALUE, TSTR, OFFANDSIZE(ipsecConfProfile, profile), NULL, 0},
    {"srcAddr", NULL, VALUE, TSTR, OFFANDSIZE(ipsecConfProfile, srcAddr), NULL, 0},
    {"srcMask", NULL, VALUE, TSTR, OFFANDSIZE(ipsecConfProfile, srcMask), NULL, 0},
    {"protocol", NULL, VALUE, TINT, OFFANDSIZE(ipsecConfProfile, protocol), NULL, 0},
    {"srcPort", NULL, VALUE, TINT, OFFANDSIZE(ipsecConfProfile, srcPort), NULL, 0},
    {"destPort", NULL, VALUE, TINT, OFFANDSIZE(ipsecConfProfile, destPort), NULL, 0},
    {"lifetime_sec", NULL, VALUE, TINT, OFFANDSIZE(ipsecConfProfile, lifetime_sec), NULL, 0},
    {"lifetime_kbytes", NULL, VALUE, TINT, OFFANDSIZE(ipsecConfProfile, lifetime_kbytes), NULL, 0},
    {"anReplay", NULL, VALUE, TENUM, OFFANDSIZE(ipsecConfProfile, anti_replay), (Pointer)enDisableEnum, 0},
    {"dpdEnable", NULL, VALUE, TENUM, OFFANDSIZE(ipsecConfProfile, dpd_enable), (Pointer)enDisableEnum, 0},
    {"dpdheartbeat", NULL, VALUE, TINT, OFFANDSIZE(ipsecConfProfile, dpd_heartbeat), NULL, 0},
    {"natEnable", NULL, VALUE, TENUM, OFFANDSIZE(ipsecConfProfile, nat_enable), (Pointer)enDisableEnum, 0},
    {"natPort", NULL, VALUE, TINT, OFFANDSIZE(ipsecConfProfile, nat_port), NULL, 0},
    {"natKeepalive", NULL, VALUE, TINT, OFFANDSIZE(ipsecConfProfile, nat_keepalive), NULL, 0},
    {"isakmpProfile", NULL, VALUE, TSTR, OFFANDSIZE(ipsecConfProfile, isakmpProfile), NULL, 0},
    {"transformSet", NULL, VALUE, TSTR, OFFANDSIZE(ipsecConfProfile, transformSet), NULL, 0},
    {"flag", NULL, VALUE, TINT, OFFANDSIZE(ipsecConfProfile, flag), NULL, 0},
	CONFIGEND
};
#endif
#if (POLICY_ROUTE == FYES)
static struct st_xmlConfig policyRouteConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(PolicyRouteProfile, head.active), (Pointer)yesnoEnum, 0},
    {"interface", NULL, VALUE, TSTR, OFFANDSIZE(PolicyRouteProfile, interfaceName), NULL, 0},
    {"bindMark", NULL, VALUE, TINT, OFFANDSIZE(PolicyRouteProfile, bindMark), NULL, 0},
    {"srcipType", NULL, VALUE, TENUM, OFFANDSIZE(PolicyRouteProfile, ip.srcIP.entryType), (Pointer)pr_ipGrpTypeEnum, 0},
    {"srcipFrom", NULL, VALUE, TIPADDR, OFFANDSIZE(PolicyRouteProfile, ip.srcIP.entryVal.range.ipFrom), NULL, 0},
    {"srcipTo", NULL, VALUE, TIPADDR, OFFANDSIZE(PolicyRouteProfile, ip.srcIP.entryVal.range.ipTo), NULL, 0},
    {"srcipGrp", NULL, VALUE, TSTR, OFFANDSIZE(PolicyRouteProfile, ip.srcIP.entryVal.name), NULL, 0},
    {"dstipType", NULL, VALUE, TENUM, OFFANDSIZE(PolicyRouteProfile, ip.dstIP.entryType), (Pointer)pr_ipGrpTypeEnum, 0},
    {"dstipFrom", NULL, VALUE, TIPADDR, OFFANDSIZE(PolicyRouteProfile, ip.dstIP.entryVal.range.ipFrom), NULL, 0},
    {"dstipTo", NULL, VALUE, TIPADDR, OFFANDSIZE(PolicyRouteProfile, ip.dstIP.entryVal.range.ipTo), NULL, 0},
    {"dstipGrp", NULL, VALUE, TSTR, OFFANDSIZE(PolicyRouteProfile, ip.dstIP.entryVal.name), NULL, 0},
    {"protocol", NULL, VALUE, TUINT, OFFANDSIZE(PolicyRouteProfile, apply.protocol), NULL, 0},
    {"dstfromPort", NULL, VALUE, TUINT, OFFANDSIZE(PolicyRouteProfile, apply.dstFromPort), NULL, 0},
    {"dstendPort", NULL, VALUE, TUINT, OFFANDSIZE(PolicyRouteProfile, apply.dstEndPort), NULL, 0},
    {"week", NULL, VALUE, TUINT, OFFANDSIZE(PolicyRouteProfile, prtime.wday), NULL, 0},
    {"timestart_str", NULL, VALUE, TSTR, OFFANDSIZE(PolicyRouteProfile, prtime.timestart_str), NULL, 0},
    {"timestop_str", NULL, VALUE, TSTR, OFFANDSIZE(PolicyRouteProfile, prtime.timestop_str), NULL, 0},
    {"timestart", NULL, VALUE, TUINT, OFFANDSIZE(PolicyRouteProfile, prtime.timestart), NULL, 0},
    {"timestop", NULL, VALUE, TUINT, OFFANDSIZE(PolicyRouteProfile, prtime.timestop), NULL, 0},
	CONFIGEND
};
#endif
#if (PARENTS_CONTROL == FYES)
static struct st_xmlConfig parentsControlGlobalConfig[] = {
    {"parentEn", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, parentControlEn.parentsEn), (Pointer)yesnoEnum, 0},
    {"mac1", NULL, VALUE, TMACADDR, OFFANDSIZE(SystemProfile, parentControlEn.parentMac1), NULL, 0},
    {"mac2", NULL, VALUE, TMACADDR, OFFANDSIZE(SystemProfile, parentControlEn.parentMac2), NULL, 0},
    {"mac3", NULL, VALUE, TMACADDR, OFFANDSIZE(SystemProfile, parentControlEn.parentMac3), NULL, 0},
    {"mac4", NULL, VALUE, TMACADDR, OFFANDSIZE(SystemProfile, parentControlEn.parentMac4), NULL, 0},
    {"mac5", NULL, VALUE, TMACADDR, OFFANDSIZE(SystemProfile, parentControlEn.parentMac5), NULL, 0},
    {"mac6", NULL, VALUE, TMACADDR, OFFANDSIZE(SystemProfile, parentControlEn.parentMac6), NULL, 0},
    {"mac7", NULL, VALUE, TMACADDR, OFFANDSIZE(SystemProfile, parentControlEn.parentMac7), NULL, 0},
    {"mac8", NULL, VALUE, TMACADDR, OFFANDSIZE(SystemProfile, parentControlEn.parentMac8), NULL, 0},
    {"mac9", NULL, VALUE, TMACADDR, OFFANDSIZE(SystemProfile, parentControlEn.parentMac9), NULL, 0},
	CONFIGEND
};
static struct st_xmlConfig parentsControlConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(ParentsControlProfile, head.active), (Pointer)yesnoEnum, 0},
    {"mac", NULL, VALUE, TMACADDR, OFFANDSIZE(ParentsControlProfile, mac), NULL, 0},
#if (TIME_GRP == FYES)
    {"timegrp", NULL, VALUE, TSTR, OFFANDSIZE(ParentsControlProfile, timeGrp), NULL, 0},
#endif
#if (UTT_SMART_UI == FYES)
    {"day", NULL, VALUE, TINT, OFFANDSIZE(ParentsControlProfile, day), NULL, 0},
    {"timeStart", NULL, VALUE, TSTR, OFFANDSIZE(ParentsControlProfile, timeStart), NULL, 0},
    {"timeStop", NULL, VALUE, TSTR, OFFANDSIZE(ParentsControlProfile, timeStop), NULL, 0},
#endif
    {"url1", NULL, VALUE, TSTR, OFFANDSIZE(ParentsControlProfile, allowURL[0].url), NULL, 0},
    {"url2", NULL, VALUE, TSTR, OFFANDSIZE(ParentsControlProfile, allowURL[1].url), NULL, 0},
    {"url3", NULL, VALUE, TSTR, OFFANDSIZE(ParentsControlProfile, allowURL[2].url), NULL, 0},
    {"url4", NULL, VALUE, TSTR, OFFANDSIZE(ParentsControlProfile, allowURL[3].url), NULL, 0},
    {"url5", NULL, VALUE, TSTR, OFFANDSIZE(ParentsControlProfile, allowURL[4].url), NULL, 0},
    {"url6", NULL, VALUE, TSTR, OFFANDSIZE(ParentsControlProfile, allowURL[5].url), NULL, 0},
    {"url7", NULL, VALUE, TSTR, OFFANDSIZE(ParentsControlProfile, allowURL[6].url), NULL, 0},
    {"url8", NULL, VALUE, TSTR, OFFANDSIZE(ParentsControlProfile, allowURL[7].url), NULL, 0},
    {"url9", NULL, VALUE, TSTR, OFFANDSIZE(ParentsControlProfile, allowURL[8].url), NULL, 0},
    {"url10", NULL, VALUE, TSTR, OFFANDSIZE(ParentsControlProfile, allowURL[9].url), NULL, 0},
	CONFIGEND
};
#endif
#if (FAT_FIT_SWITCH == FYES)
static struct st_xmlConfig  fatFitSwitchConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(FatFitSwitchProfile, head.active), (Pointer)yesnoEnum, 0},
    {"fatEnable", NULL, VALUE, TENUM, OFFANDSIZE(FatFitSwitchProfile, fatEnable), (Pointer)yesnoEnum, 0},
	CONFIGEND
};
#endif
#if (AC_WORK_MODE == FYES)
static struct st_xmlConfig  acWorkModeConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(AcWorkModeProfile, head.active), (Pointer)yesnoEnum, 0},
    {"workMode", NULL, VALUE, TENUM, OFFANDSIZE(AcWorkModeProfile, workMode), (Pointer)yesnoEnum, 0},
    {"gw", NULL, VALUE, TIPADDR, OFFANDSIZE(AcWorkModeProfile, gateWay), NULL, 0},
    {"pdns", NULL, VALUE, TIPADDR, OFFANDSIZE(AcWorkModeProfile, pDns), NULL, 0},
	CONFIGEND
};
#endif

#if (AP_LOAD_BALANCE == FYES)
static struct st_xmlConfig bl0GroupConfig[] = {
    {"ap0Mac", NULL, VALUE, TMACADDR, OFFANDSIZE(ApLoadBalanceProfile, ap[0].macAddr), 0, 0},
    {"ap0DevName", NULL, VALUE, TSTR, OFFANDSIZE(ApLoadBalanceProfile, ap[0].dev_name), 0, 0},
    {"ap0Sn", NULL, VALUE, TINT, OFFANDSIZE(ApLoadBalanceProfile, ap[0].sn), 0, 0},
    {"ap0Alive", NULL, VALUE, TINT, OFFANDSIZE(ApLoadBalanceProfile, ap[0].isAlive), 0, 0},
    {"ap0Client", NULL, VALUE, TINT, OFFANDSIZE(ApLoadBalanceProfile, ap[0].client), 0, 0},
    {"ap0Full", NULL, VALUE, TINT, OFFANDSIZE(ApLoadBalanceProfile, ap[0].isFull), 0, 0},
	CONFIGEND,
};
static struct st_xmlConfig bl1GroupConfig[] = {
    {"ap1Mac", NULL, VALUE, TMACADDR, OFFANDSIZE(ApLoadBalanceProfile, ap[1].macAddr), 0, 0},
    {"ap1DevName", NULL, VALUE, TSTR, OFFANDSIZE(ApLoadBalanceProfile, ap[1].dev_name), 0, 0},
    {"ap1Sn", NULL, VALUE, TINT, OFFANDSIZE(ApLoadBalanceProfile, ap[1].sn), 0, 0},
    {"ap1Alive", NULL, VALUE, TINT, OFFANDSIZE(ApLoadBalanceProfile, ap[1].isAlive), 0, 0},
    {"ap1Client", NULL, VALUE, TINT, OFFANDSIZE(ApLoadBalanceProfile, ap[1].client), 0, 0},
    {"ap1Full", NULL, VALUE, TINT, OFFANDSIZE(ApLoadBalanceProfile, ap[1].isFull), 0, 0},
	CONFIGEND,
};
static struct st_xmlConfig bl2GroupConfig[] = {
    {"ap2Mac", NULL, VALUE, TMACADDR, OFFANDSIZE(ApLoadBalanceProfile, ap[2].macAddr), 0, 0},
    {"ap2DevName", NULL, VALUE, TSTR, OFFANDSIZE(ApLoadBalanceProfile, ap[2].dev_name), 0, 0},
    {"ap2Sn", NULL, VALUE, TINT, OFFANDSIZE(ApLoadBalanceProfile, ap[2].sn), 0, 0},
    {"ap2Alive", NULL, VALUE, TINT, OFFANDSIZE(ApLoadBalanceProfile, ap[2].isAlive), 0, 0},
    {"ap2Client", NULL, VALUE, TINT, OFFANDSIZE(ApLoadBalanceProfile, ap[2].client), 0, 0},
    {"ap2Full", NULL, VALUE, TINT, OFFANDSIZE(ApLoadBalanceProfile, ap[2].isFull), 0, 0},
	CONFIGEND,
};
static struct st_xmlConfig bl3GroupConfig[] = {
    {"ap3Mac", NULL, VALUE, TMACADDR, OFFANDSIZE(ApLoadBalanceProfile, ap[3].macAddr), 0, 0},
    {"ap3DevName", NULL, VALUE, TSTR, OFFANDSIZE(ApLoadBalanceProfile, ap[3].dev_name), 0, 0},
    {"ap3Sn", NULL, VALUE, TINT, OFFANDSIZE(ApLoadBalanceProfile, ap[3].sn), 0, 0},
    {"ap3Alive", NULL, VALUE, TINT, OFFANDSIZE(ApLoadBalanceProfile, ap[3].isAlive), 0, 0},
    {"ap3Client", NULL, VALUE, TINT, OFFANDSIZE(ApLoadBalanceProfile, ap[3].client), 0, 0},
    {"ap3Full", NULL, VALUE, TINT, OFFANDSIZE(ApLoadBalanceProfile, ap[3].isFull), 0, 0},
	CONFIGEND,
};
static struct st_xmlConfig bl4GroupConfig[] = {
    {"ap4Mac", NULL, VALUE, TMACADDR, OFFANDSIZE(ApLoadBalanceProfile, ap[4].macAddr), 0, 0},
    {"ap4DevName", NULL, VALUE, TSTR, OFFANDSIZE(ApLoadBalanceProfile, ap[4].dev_name), 0, 0},
    {"ap4Sn", NULL, VALUE, TINT, OFFANDSIZE(ApLoadBalanceProfile, ap[4].sn), 0, 0},
    {"ap4Alive", NULL, VALUE, TINT, OFFANDSIZE(ApLoadBalanceProfile, ap[4].isAlive), 0, 0},
    {"ap4Client", NULL, VALUE, TINT, OFFANDSIZE(ApLoadBalanceProfile, ap[4].client), 0, 0},
    {"ap4Full", NULL, VALUE, TINT, OFFANDSIZE(ApLoadBalanceProfile, ap[4].isFull), 0, 0},
	CONFIGEND,
};

static struct st_xmlConfig  apLoadBalanceConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(ApLoadBalanceProfile, head.active), (Pointer)yesnoEnum, 0},
    {"lbEn", NULL, VALUE, TENUM, OFFANDSIZE(ApLoadBalanceProfile, lbEn), (Pointer)yesnoEnum, 0},
    {"num", NULL, VALUE, TINT, OFFANDSIZE(ApLoadBalanceProfile, num), NULL, 0},
    {"threshold", NULL, VALUE, TINT, OFFANDSIZE(ApLoadBalanceProfile, threshold), NULL, 0},
    {"source_sum", NULL, VALUE, TINT, OFFANDSIZE(ApLoadBalanceProfile, source_sum), NULL, 0},
	{"bl0Group", bl0GroupConfig, 0, 0, 0, 0, 0},
	{"bl1Group", bl1GroupConfig, 0, 0, 0, 0, 0},
	{"bl2Group", bl2GroupConfig, 0, 0, 0, 0, 0},
	{"bl3Group", bl3GroupConfig, 0, 0, 0, 0, 0},
	{"bl4Group", bl4GroupConfig, 0, 0, 0, 0, 0},
	CONFIGEND
};

#endif

#if (WIRELESS == FYES)
static struct st_xmlConfig wireSafeConfig[] = {
    {"authMode", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mSafeCf.SelAuthMode), NULL, 0},
    {"SafepskPsswd", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode.pskPsswd), NULL, 0},
    {"SafeEncrypType", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode.EncrypType), NULL, 0},
    {"SafewepKeyID", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode.wepKeyID), NULL, 0},
    {"SafewepKeyType", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode.wepKeyType), NULL, 0},
    {"SafewepKey1", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode.wepkey1), NULL, 0},
    {"SafewepKey2", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode.wepkey2), NULL, 0},
    {"SafewepKey3", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode.wepkey3), NULL, 0},
    {"SafewepKey4", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode.wepkey4), NULL, 0},
    {"SafewepkeyLen1", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode.wepkeyLength1), NULL, 0},
    {"SafewepkeyLen2", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode.wepkeyLength2), NULL, 0},
    {"SafewepkeyLen3", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode.wepkeyLength3), NULL, 0},
    {"SafewepkeyLen4", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode.wepkeyLength4), NULL, 0},
    {"SaferadiusIp", NULL, VALUE, TIPADDR, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode.RADIUS_Server), NULL, 0},
    {"SaferadiusPort", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode.RADIUS_Port), NULL, 0},
    {"Saferieee8021x", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode.IEEE8021X), NULL, 0},
    {"Saferekeyinterval", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode.RekeyInterval), NULL, 0},
    {"SaferadiusKey", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode.RADIUS_Key), NULL, 0},
#if (UTT_SSID2_ENCRYPTION == FYES)
    {"authMode_2", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mSafeCf.SelAuthMode_2), NULL, 0},
    {"SafepskPsswd_2", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode_2.pskPsswd), NULL, 0},
    {"SafeEncrypType_2", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode_2.EncrypType), NULL, 0},
    {"SafewepKeyID_2", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode_2.wepKeyID), NULL, 0},
    {"SafewepKeyType_2", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode_2.wepKeyType), NULL, 0},
    {"SafewepKey1_2", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode_2.wepkey1), NULL, 0},
    {"SafewepKey2_2", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode_2.wepkey2), NULL, 0},
    {"SafewepKey3_2", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode_2.wepkey3), NULL, 0},
    {"SafewepKey4_2", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode_2.wepkey4), NULL, 0},
    {"SafewepkeyLen1_2", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode_2.wepkeyLength1), NULL, 0},
    {"SafewepkeyLen2_2", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode_2.wepkeyLength2), NULL, 0},
    {"SafewepkeyLen3_2", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode_2.wepkeyLength3), NULL, 0},
    {"SafewepkeyLen4_2", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode_2.wepkeyLength4), NULL, 0},
    {"SaferadiusIp_2", NULL, VALUE, TIPADDR, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode_2.RADIUS_Server), NULL, 0},
    {"SaferadiusPort_2", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode_2.RADIUS_Port), NULL, 0},
    {"Saferieee8021x_2", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode_2.IEEE8021X), NULL, 0},
    {"Saferekeyinterval_2", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode_2.RekeyInterval), NULL, 0},
    {"SaferadiusKey_2", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mSafeCf.ap.AuthMode_2.RADIUS_Key), NULL, 0},
#endif
    CONFIGEND
};

static struct st_xmlConfig apClientConfig[] = {
    {"apcliEnable", NULL, VALUE, TENUM, OFFANDSIZE(WirelessProfile, mBaseCf.apClient.apclientEnable), (Pointer)enDisableEnum, 0},
    {"apcliMac", NULL, VALUE, TMACADDR, OFFANDSIZE(WirelessProfile, mBaseCf.apClient.APCliMacs), 0, 0},
    {"apcliSsid", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.apClient.ApCliSsid), NULL, 0},
    {"apclipskPsswd", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.apClient.AuthMode.pskPsswd), NULL, 0},
    {"apcliEncrypType", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.apClient.AuthMode.EncrypType), NULL, 0},
    {"apcliwepKeyID", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.apClient.AuthMode.wepKeyID), NULL, 0},
    {"apcliwepKeyType", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.apClient.AuthMode.wepKeyType), NULL, 0},
    {"apcliwepKey1", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.apClient.AuthMode.wepkey1), NULL, 0},
    {"apcliwepKey2", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.apClient.AuthMode.wepkey2), NULL, 0},
    {"apcliwepKey3", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.apClient.AuthMode.wepkey3), NULL, 0},
    {"apcliwepKey4", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.apClient.AuthMode.wepkey4), NULL, 0},
    {"apcliwepkeyLen1", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.apClient.AuthMode.wepkeyLength1), NULL, 0},
    {"apcliwepkeyLen2", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.apClient.AuthMode.wepkeyLength2), NULL, 0},
    {"apcliwepkeyLen3", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.apClient.AuthMode.wepkeyLength3), NULL, 0},
    {"apcliwepkeyLen4", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.apClient.AuthMode.wepkeyLength4), NULL, 0},
    {"wdsSwitch", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.apClient.wdsSwitch), NULL, 0},
    CONFIGEND
};

static struct st_xmlConfig apWdsConfig[] = {
    {"wdsmode", NULL, VALUE, TENUM, OFFANDSIZE(WirelessProfile, mBaseCf.apWds.wdsEnable), (Pointer)apWorkModeEnum, 0},
    {"wdsMac0", NULL, VALUE, TMACADDR, OFFANDSIZE(WirelessProfile, mBaseCf.apWds.WPSMacs0), 0, 0},
    {"wdsMac1", NULL, VALUE, TMACADDR, OFFANDSIZE(WirelessProfile, mBaseCf.apWds.WPSMacs1), 0, 0},
    {"wdsMac2", NULL, VALUE, TMACADDR, OFFANDSIZE(WirelessProfile, mBaseCf.apWds.WPSMacs2), 0, 0},
    {"wdsMac3", NULL, VALUE, TMACADDR, OFFANDSIZE(WirelessProfile, mBaseCf.apWds.WPSMacs3), 0, 0},
    {"wdsList", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.apWds.WdsList), NULL, 0},
    {"wdsPhyMode", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.apWds.WdsPhyMode), NULL, 0},
    {"bridgeSSID1", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.apWds.bridgeSSID1), NULL, 0},
    {"wdsEncrypType", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.apWds.wdsEncrypType), NULL, 0},
    {"wdspskPsswd", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.apWds.AuthMode.pskPsswd), NULL, 0},
    {"wdswepKeyID", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.apWds.AuthMode.wepKeyID), NULL, 0},
    {"wdswepKeyType", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.apWds.AuthMode.wepKeyType), NULL, 0},
    {"wdswepKey1", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.apWds.AuthMode.wepkey1), NULL, 0},
    {"wdswepKey2", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.apWds.AuthMode.wepkey2), NULL, 0},
    {"wdswepKey3", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.apWds.AuthMode.wepkey3), NULL, 0},
    {"wdswepKey4", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.apWds.AuthMode.wepkey4), NULL, 0},
    {"wdswepkeyLen1", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.apWds.AuthMode.wepkeyLength1), NULL, 0},
    {"wdswepkeyLen2", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.apWds.AuthMode.wepkeyLength2), NULL, 0},
    {"wdswepkeyLen3", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.apWds.AuthMode.wepkeyLength3), NULL, 0},
    {"wdswepkeyLen4", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.apWds.AuthMode.wepkeyLength4), NULL, 0},
    CONFIGEND
};

static struct st_xmlConfig wireAdvanceConfig[] = {
    {"rts", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, advance.rts), NULL, 0},
    {"section", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, advance.section), NULL, 0},
    {"beacon", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, advance.beacon), NULL, 0},
    {"dtim", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, advance.dtim), NULL, 0},
    {"txBurst", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, advance.TxBurst), NULL, 0},
    {"preamble", NULL, VALUE, TENUM, OFFANDSIZE(WirelessProfile, advance.preamble), (Pointer)enDisableEnum, 0},
    {"wmm", NULL, VALUE, TENUM, OFFANDSIZE(WirelessProfile, advance.wmm), (Pointer)enDisableEnum, 0},
    CONFIGEND
};

static struct st_xmlConfig apAdvanceConfig[] = {
    {"rts", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, advance.rts), NULL, 0},
    {"section", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, advance.section), NULL, 0},
    {"beacon", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, advance.beacon), NULL, 0},
    {"dtim", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, advance.dtim), NULL, 0},
    {"preamble", NULL, VALUE, TENUM, OFFANDSIZE(WirelessProfile, advance.preamble), (Pointer)enDisableEnum, 0},
    {"wmm", NULL, VALUE, TENUM, OFFANDSIZE(WirelessProfile, advance.wmm), (Pointer)enDisableEnum, 0},
    CONFIGEND
};

static struct st_xmlConfig filterGlobalConfig[] = {
    {"filterEn", NULL, VALUE, TENUM, OFFANDSIZE(WirelessProfile, mFltGlb.filterEn), (Pointer)enDisableEnum, 0},
    {"action", NULL, VALUE, TENUM, OFFANDSIZE(WirelessProfile, mFltGlb.action), (Pointer)apMacFilterActionEnum, 0},
    CONFIGEND
};

static struct st_xmlConfig wireBaseConfig[] = {
    {"apWorkMode", NULL, VALUE, TENUM, OFFANDSIZE(WirelessProfile, mBaseCf.apWorkMode), (Pointer)apWorkModeEnum, 0},
    {"ssid", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.SSID1), NULL, 0},
#if (WLAN_MULTI_SSID == FYES)
    {"ssid2", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.SSID2), NULL, 0},
#if (MIDDLE_SOFT == FYES)
    {"ssid3", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.SSID3), NULL, 0},
    {"ssid4", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.SSID4), NULL, 0},
#endif
    {"BssidNum", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.BssidNum), NULL, 0},
#endif
    {"wirelessMode", NULL, VALUE, TENUM, OFFANDSIZE(WirelessProfile, mBaseCf.WirelessMode), (Pointer)wirelessModeEnum, 0},
    {"basicRate", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.BasicRate), NULL, 0},
    {"AutoChannelSelect", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.AutoChannelSelect), NULL, 0},
    {"channel", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.Channel), NULL, 0},
    {"cltIsolate", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.NoForwarding), NULL, 0},
    {"txPower", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.TxPower), NULL, 0},
#if (FEATURE_AP == FYES)
    {"byPass", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.BYPASS), NULL, 0},
#endif
    {"ChanWidth", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.ChanWidth), NULL, 0},
    {"VChanWidth", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.VChanWidth), NULL, 0},
#if ((UTT_SMART_UI == FYES) && (WIRELESS_5G == FYES))
    {"Country", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.Country), NULL, 0},
    {"CountryRegionABand", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.CountryRegionABand), NULL, 0},
#endif
    {"ssidBroadCastEn", NULL, VALUE, TENUM, OFFANDSIZE(WirelessProfile, mBaseCf.SSIDBroadCastEn), (Pointer)enDisableEnum, 0},
    {"operationMode", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.OperationMode), NULL, 0},
    {"authMode", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.SelAuthMode), NULL, 0},
    {"ownip", NULL, VALUE, TIPADDR, OFFANDSIZE(WirelessProfile, mBaseCf.own_ip_addr), NULL, 0},
    {"eapifname", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.EAPifname), NULL, 0},
    {"preAuthifname", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.PreAuthifname), NULL, 0},
    {"HT_BSSCoexistence", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.HT_BSSCoexistence), NULL, 0},
    {"apClient", apClientConfig, 0, 0, 0, 0, 0},
    {"apWds", apWdsConfig, 0, 0, 0, 0, 0},
    {"sgi", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.HT_GI), NULL, 0},
#if (UTT_SMART_UI == FYES || WLAN_MULTI_SSID == FYES)
    {"encodeType", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.encodeType), NULL, 0},
#if (WLAN_MULTI_SSID == FYES)
    {"encodeType2", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.encodeType2), NULL, 0},
#if (MIDDLE_SOFT == FYES)
    {"encodeType3", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.encodeType3), NULL, 0},
    {"encodeType4", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.encodeType4), NULL, 0},
#endif
#if (UTT_SSID_ISOLATION == FYES)
    {"ssidSeparate", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.ssidSeparate),NULL, 0},
#endif
#endif
#endif
#if (WLAN_WORK_TIME == FYES)
#if (UTT_SMART_UI == FYES)
    {"WlanTimeEn", NULL, VALUE, TENUM, OFFANDSIZE(WirelessProfile, mBaseCf.WlanTimeEn), (Pointer)enDisableEnum, 0},
#endif
    {"day", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.day), NULL, 0},
    {"weekdayTimeStart", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.weekdayTimeStart), NULL, 0},
    {"weekdayTimeStop", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.weekdayTimeStop), NULL, 0},
    {"weekendTimeStart", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.weekendTimeStart), NULL, 0},
    {"weekendTimeStop", NULL, VALUE, TSTR, OFFANDSIZE(WirelessProfile, mBaseCf.weekendTimeStop), NULL, 0},
#endif
    {"CountryRegion", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, mBaseCf.CountryRegion), NULL, 0},
    CONFIGEND
};

#if (FEATURE_AP == FYES)
static struct st_xmlConfig wireFitApConfig[] = {
#if 0
    {"cltIsolate", NULL, VALUE, TENUM, OFFANDSIZE(WirelessProfile, fitAp.cltIsolate), (Pointer)enDisableEnum, 0},
#endif
    {"rate", NULL, VALUE, TENUM, OFFANDSIZE(WirelessProfile, fitAp.rate), (Pointer)wlanRateEnum, 0},
    {"autoPower", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, fitAp.autoPower), NULL, 0},
    {"manPower", NULL, VALUE, TENUM, OFFANDSIZE(WirelessProfile, fitAp.manPower), (Pointer)wlanManualPowerEnum, 0},
    CONFIGEND
};
#endif

static struct st_xmlConfig wirelessConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(WirelessProfile, head.active), (Pointer)yesnoEnum, 0},
#if (WLAN_WORK_TIME == FYES)
    {"time_active", NULL, VALUE, TENUM, OFFANDSIZE(WirelessProfile, time_active), (Pointer)yesnoEnum, 0},
#endif
    {"mbasecfg", wireBaseConfig, 0, 0, 0, 0, 0},
#if (FEATURE_AP == FYES)
    {"fitAp", wireFitApConfig, 0, 0, 0, 0, 0},
#endif
    {"msafecfg", wireSafeConfig, 0, 0, 0, 0, 0},
    {"advance", wireAdvanceConfig, 0, 0, 0, 0, 0},
    {"mFltGlb", filterGlobalConfig, 0, 0, 0, 0, 0},
#if (WIRELESS_TURBO == FYES)
    {"turbo", NULL, VALUE, TINT, OFFANDSIZE(WirelessProfile, turbo), NULL, 0},
#endif
    CONFIGEND
};

static struct st_xmlConfig  wirelessMacFilterConfig[] = {
    {"mac", NULL, VALUE, TMACADDR, OFFANDSIZE(WirelessMacFilterProfile, mac), NULL, 0},
    //{"mac", NULL, VALUE, TSTR, OFFANDSIZE(WirelessMacFilterProfile, mac), NULL, 0},
    CONFIGEND,
};

#if (WIRELESS_5G == FYES)
static struct st_xmlConfig  wireless5gMacFilterConfig[] = {
    {"mac", NULL, VALUE, TMACADDR, OFFANDSIZE(Wireless5gMacFilterProfile, mac), NULL, 0},
    //{"mac", NULL, VALUE, TSTR, OFFANDSIZE(Wireless5gMacFilterProfile, mac), NULL, 0},
    CONFIGEND,
};
#endif
#endif

static struct st_xmlConfig ifEtherStaticConfig[] = {
    {"ip", NULL, VALUE, TIPADDR, OFFANDSIZE(InterfaceProfile, ether.sta.ip), NULL, 0},
    {"nm", NULL, VALUE, TIPADDR, OFFANDSIZE(InterfaceProfile, ether.sta.nm), NULL, 0},
#if (MULTI_LAN == FYES)
    {"ip2", NULL, VALUE, TIPADDR, OFFANDSIZE(InterfaceProfile, ether.sta.ip2), NULL, 0},
    {"nm2", NULL, VALUE, TIPADDR, OFFANDSIZE(InterfaceProfile, ether.sta.nm2), NULL, 0},
    {"ip3", NULL, VALUE, TIPADDR, OFFANDSIZE(InterfaceProfile, ether.sta.ip3), NULL, 0},
    {"nm3", NULL, VALUE, TIPADDR, OFFANDSIZE(InterfaceProfile, ether.sta.nm3), NULL, 0},
    {"ip4", NULL, VALUE, TIPADDR, OFFANDSIZE(InterfaceProfile, ether.sta.ip4), NULL, 0},
    {"nm4", NULL, VALUE, TIPADDR, OFFANDSIZE(InterfaceProfile, ether.sta.nm4), NULL, 0},
#endif
    {"gw", NULL, VALUE, TIPADDR, OFFANDSIZE(InterfaceProfile, ether.sta.gw), NULL, 0},
    {"pdns", NULL, VALUE, TIPADDR, OFFANDSIZE(InterfaceProfile, ether.sta.pd), NULL, 0},
    {"sdns", NULL, VALUE, TIPADDR, OFFANDSIZE(InterfaceProfile, ether.sta.sd), NULL, 0},
    CONFIGEND,
};

static struct st_xmlConfig ifEtherPppoeConfig[] = {
    {"user", NULL, VALUE, TSTR, OFFANDSIZE(InterfaceProfile, ether.pppoe.user), NULL, 0},
    {"passwd", NULL, VALUE, TSTR, OFFANDSIZE(InterfaceProfile, ether.pppoe.passwd), NULL, 0},
    {"pppConnMode", NULL, VALUE, TENUM, OFFANDSIZE(InterfaceProfile, ether.pppoe.pppConnMode), (Pointer)pppConnModeEnum, 0},
    {"spDialMode", NULL, VALUE, TENUM, OFFANDSIZE(InterfaceProfile, ether.pppoe.spDial), (Pointer)pppSpDailEnum, 0},
    {"authType", NULL, VALUE, TENUM, OFFANDSIZE(InterfaceProfile, ether.pppoe.authType), (Pointer)pppAuthEnum, 0},
    {"mtu", NULL, VALUE, TINT, OFFANDSIZE(InterfaceProfile, ether.pppoe.mtu), NULL, 0},
    {"idleTime", NULL, VALUE, TINT, OFFANDSIZE(InterfaceProfile, ether.pppoe.idleTime), NULL, 0},
    CONFIGEND,
};

#if (WIRELESS == FYES)
static struct st_xmlConfig ifEtherWan3gConfig[] = {
    {"dev", NULL, VALUE, TSTR, OFFANDSIZE(InterfaceProfile, ether.wan3g.wan_3g_dev), NULL, 0},
    {"isp", NULL, VALUE, TENUM, OFFANDSIZE(InterfaceProfile, ether.wan3g.wan_3g_isp), (Pointer)ispMode3G, 0},
    {"authtype", NULL, VALUE, TENUM, OFFANDSIZE(InterfaceProfile, ether.wan3g.wan_3g_authtype), (Pointer)authType3G, 0},
    {"pincode", NULL, VALUE, TSTR, OFFANDSIZE(InterfaceProfile, ether.wan3g.wan_3g_pincode), NULL, 0},
    {"authname", NULL, VALUE, TSTR, OFFANDSIZE(InterfaceProfile, ether.wan3g.wan_3g_authname), NULL, 0},
    {"authpasswd", NULL, VALUE, TSTR, OFFANDSIZE(InterfaceProfile, ether.wan3g.wan_3g_authpasswd), NULL, 0},
    {"connname", NULL, VALUE, TSTR, OFFANDSIZE(InterfaceProfile, ether.wan3g.wan_3g_connname), NULL, 0},
    {"dialnum", NULL, VALUE, TSTR, OFFANDSIZE(InterfaceProfile, ether.wan3g.wan_3g_dialnum), NULL, 0},
    {"pppname", NULL, VALUE, TSTR, OFFANDSIZE(InterfaceProfile, ether.wan3g.wan_3g_pppname), NULL, 0},
    {"ppppasswd", NULL, VALUE, TSTR, OFFANDSIZE(InterfaceProfile, ether.wan3g.wan_3g_ppppasswd), NULL, 0},
    CONFIGEND,
};
#endif
static struct st_xmlConfig ifEtherConfig[] = {
    {"connMode", NULL, VALUE, TENUM, OFFANDSIZE(InterfaceProfile, ether.connMode), (Pointer)ethConnModeEnum, 0},
    {"mac", NULL, VALUE, TMACADDR, OFFANDSIZE(InterfaceProfile, ether.mac), 0, 0},
#if (PORT_CONF_REDUCED == FYES)
    {"speedMode",    NULL, VALUE, TENUM, OFFANDSIZE(InterfaceProfile, ether.speedMode),    (Pointer)portModeEnum, 0},
#endif
    {"static", ifEtherStaticConfig, 0, 0, 0, 0, 0},
    {"pppoe", ifEtherPppoeConfig, 0, 0, 0, 0, 0},
#if (WIRELESS == FYES)
    {"wan3g", ifEtherWan3gConfig, 0, 0, 0, 0, 0},
#endif
    CONFIGEND,
};

static struct st_xmlConfig interfaceConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(InterfaceProfile, head.active), (Pointer)yesnoEnum, 0},
#if (SMART_QOS_FUN == FYES)
    {"smartQosEn", NULL, VALUE, TENUM, OFFANDSIZE(InterfaceProfile, smartQosEn), (Pointer)enDisableEnum, 0},
#endif
#if (ANTI_SNIFFER == FYES)
    {"antiSniffEn", NULL, VALUE, TENUM, OFFANDSIZE(InterfaceProfile, antiSnifEn), (Pointer)enDisableEnum, 0},
#endif
#if (ARP_BINDS == FYES)
    {"arpAllowOtherEn", NULL, VALUE, TENUM, OFFANDSIZE(InterfaceProfile, arpAllowOtherEn), (Pointer)enDisableEnum, 0},
#endif
    {"ethernet", ifEtherConfig, 0, 0, 0, 0, 0},
    {"rxBand", NULL, VALUE, TINT, OFFANDSIZE(InterfaceProfile, txRate), 0, 0},
    {"txBand", NULL, VALUE, TINT, OFFANDSIZE(InterfaceProfile, rxRate), 0, 0},
    {"isp", NULL, VALUE, TINT, OFFANDSIZE(InterfaceProfile, isp), 0, 0},
    {"isp1", NULL, VALUE, TINT, OFFANDSIZE(InterfaceProfile, isp1), 0, 0},
    {"isp2", NULL, VALUE, TINT, OFFANDSIZE(InterfaceProfile, isp2), 0, 0},
    {"detectIp", NULL, VALUE, TIPADDR, OFFANDSIZE(InterfaceProfile, detectIp), 0, 0},
    {"detectInterval", NULL, VALUE, TINT, OFFANDSIZE(InterfaceProfile, detectInterval), 0, 0},
    {"detectCount", NULL, VALUE, TINT, OFFANDSIZE(InterfaceProfile, detectCount), 0, 0},
#if 0
    {"interIPS", NULL, VALUE, TIPADDR, OFFANDSIZE(InterfaceProfile, srcRouteIpS), 0, 0},
    {"interIPE", NULL, VALUE, TIPADDR, OFFANDSIZE(InterfaceProfile, srcRouteIpE), 0, 0},
#endif
    {"lineBalMode", NULL, VALUE, TENUM, OFFANDSIZE(InterfaceProfile, lineBalMode), (Pointer)ifBalanceModeEnum, 0},
#if (NAT_GLOBAL_ENABLE == FYES)
    {"NatActive", NULL, VALUE, TINT, OFFANDSIZE(InterfaceProfile, NatActive), (Pointer)enDisableEnum, 0},
#endif
#if (UPNP_GLOBAL_ENABLE == FYES)
    {"UpnpActive", NULL, VALUE, TINT, OFFANDSIZE(InterfaceProfile, UpnpActive), (Pointer)enDisableEnum, 0},
#endif
    {"BindWanEn", NULL, VALUE, TENUM, OFFANDSIZE(InterfaceProfile, BindWanEn), (Pointer)enDisableEnum, 0},
    {"GateWayMac", NULL, VALUE, TMACADDR, OFFANDSIZE(InterfaceProfile, GateWayMac), NULL, 0},
    CONFIGEND,
};

static struct st_xmlConfig systemConfig[] = {
    {"lineGroupMode", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, lineGroupMode), (Pointer)lineGroupModeEnum, 0},
    {"popWelcome", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, popWelcome), (Pointer)enDisableEnum, 0},
    {"wanIfCount", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, wanIfCount), NULL, 0},
    {"lanPortCount", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, lanPortCount), NULL, 0},
#if (ANTI_SNIFFER == FYES)
    {"antiSniffer", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, antiSnifferEn), (Pointer)enDisableEnum, 0},
#endif
#if (NET_SECURITY == FYES)
    {"antiBlaster", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, sysNetSe.antiBlasterEn), (Pointer)enDisableEnum, 0},
    {"antiDDoS", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, sysNetSe.antiDDoSEn), (Pointer)enDisableEnum, 0},
    {"antiIPCheat", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, sysNetSe.antiIPCheatEn), (Pointer)enDisableEnum, 0},
    {"antiUDPFlood", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, sysNetSe.antiUDPFloodEn), (Pointer)enDisableEnum, 0},
    {"max_udp_rxpp", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, sysNetSe.max_udp_rxpp), NULL, 0},
    {"antiICMPFlood", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, sysNetSe.antiICMPFloodEn), (Pointer)enDisableEnum, 0},
    {"max_icmp_rxpp", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, sysNetSe.max_icmp_rxpp), NULL, 0},
    {"antiSYNFlood", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, sysNetSe.antiSYNFloodEn), (Pointer)enDisableEnum, 0},
    {"max_syn_rxpp", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, sysNetSe.max_syn_rxpp), NULL, 0},
    {"antiArpBroadcast", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, sysNetSe.lanArpBroadcastEn), (Pointer)enDisableEnum, 0},
	{"lanArpBroadcastInterval", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, sysNetSe.lanArpBroadcastInterval), NULL, 0},
    {"antiPingReply", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, sysNetSe.antiPingReplyEn), (Pointer)enDisableEnum, 0},
    {"accessCtrlFromIP", NULL, VALUE, TIPADDR, OFFANDSIZE(SystemProfile, sysNetSe.accessCtrlFromIP), NULL, 0},
    {"accessCtrlEndIP", NULL, VALUE, TIPADDR, OFFANDSIZE(SystemProfile, sysNetSe.accessCtrlEndIP), NULL, 0},
    {"antiportScanf", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, sysNetSe.antiportScanfEn), (Pointer)enDisableEnum, 0},
    {"scanfCnt", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, sysNetSe.scanfCnt), NULL, 0},
#endif
#if (DNS_FILTER == FYES)
    {"dnsFilter", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, dnsFilterEn), (Pointer)enDisableEnum, 0},
    {"dnsFilterGlobal", dnsFilterGlobalConfig, 0, 0, 0, 0, 0},
#endif
#if (CONNECT_LIMIT == FYES)
    {"connectLimit", connLimitConfig, 0, 0, 0, 0, 0},
#endif
#if (TAG_VLAN == FYES)
    {"pvid", pvidConfig, 0, 0, 0, 0, 0},
#endif
#if (SSH_CLI == FYES)
    {"SSH", SSHConfig, 0, 0, 0, 0, 0},
#endif
#if (HTTPS == FYES)
    {"https", httpsConfig, 0, 0, 0, 0, 0},
#endif
#if (PORT_MIRROR == FYES)
    {"portMirror", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, portMirrorEn), (Pointer)enDisableEnum, 0},
#if defined(CONFIG_RTL8367M) || defined(CONFIG_RTL8306_PHY) || defined(CONFIG_RALINK_MT7620) || defined(CONFIG_RTL8367RB)|| defined(CONFIG_RALINK_MT7621)|| defined(CONFIG_RALINK_MT7628)
    {"monitorPort", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, mirror.monitorPort), NULL, 0},
    {"sourcePort", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, mirror.sourcePort), NULL, 0},
#endif
#endif
#if (EXCEPT_QQ == FYES)
    {"exceptQQEn", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, exceptQQEn), (Pointer)enDisableEnum, 0},
    {"exceptEnterpriseQQEn", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, exceptEnterpriseQQEn), (Pointer)enDisableEnum, 0},
#endif
#if (EXCEPT_MSN == FYES)
    {"exceptMSNEn", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, exceptMSNEn), (Pointer)enDisableEnum, 0},
#endif
#if (EXCEPT_ALI == FYES)
    {"exceptAliEn", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, exceptAliEn), (Pointer)enDisableEnum, 0},
#endif
#if (GROUP_BASE == FYES) 
    {"uttNetfilterEn", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, UttNfRuleEn), (Pointer)enDisableEnum, 0},
#endif

#if (PORT_MIRROR_6530G == FYES)
    {"Mirror_6530G", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, Mirror_6530G_En), (Pointer)enDisableEnum, 0},
#endif

#if (UTTPDB == FYES)
    {"UserStatusEn", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, UttNfIndentEn), (Pointer)enDisableEnum, 0},
#endif
#if (HOTEL_PNP == FYES)
    {"hotelPnpEn", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, hotelPnpEn), (Pointer)enDisableEnum, 0},
#endif
#if (IDENTIFY_BIND == FYES)
    {"IdentifyBindEn", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, IdentifyBindEn), (Pointer)enDisableEnum, 0},
#endif
#if (HARDWARE_NAT == FYES)
    {"hardwareNATEn", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, hardwareNATEn), (Pointer)enDisableEnum, 0},
#endif
#if (FIREWALL == FYES)
    {"firewallEn", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, firewallEn), (Pointer)enDisableEnum, 0},
#endif
#if (POLICY_ROUTE == FYES)
    {"policyrouteEn", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, policyrouteEn), (Pointer)enDisableEnum, 0},
#endif
#if (PARENTS_CONTROL == FYES)
    {"parentsControlGlobal", parentsControlGlobalConfig, 0, 0, 0, 0, 0},
#endif
#if (EASY_TASK == FYES)
    {"isDefaultConf", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, isDefaultConf), NULL, 0},
#endif
#if (AP_LOAD_BALANCE == FYES)
    {"loadBalanceEn", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, loadBalanceEn), (Pointer)enDisableEnum, 0},
#endif
#if(UTT_IPAUTOREP==  FYES)
    {"CheckIPEn", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, CheckIPEn), (Pointer)enDisableEnum, 0},  
#endif
#if (SZONE_AUTO_UPDATA == FYES)
    {"szoneAutoUpdata", NULL, VALUE, TENUM, OFFANDSIZE(SystemProfile, szoneAutoUpdata), (Pointer)enDisableEnum, 0},
#endif
#if (UTT_BRIDEG_MODE_FLAG == FYES)
    {"brideg_mode_flag", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, brideg_mode_flag), NULL, 0},
    {"eth2_mac", NULL, VALUE, TSTR, OFFANDSIZE(SystemProfile, eth2_mac), NULL, 0},
    {"Elink_flag", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, Elink_flag), NULL, 0},
#endif
#if (APP_ELINKC == FYES)
    {"elinkc_flag", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, elinkc_flag), NULL, 0},
#endif
#if (WIFI_SLEEP_FLAG == FYES)
    {"wifisleep", NULL, VALUE, TINT, OFFANDSIZE(SystemProfile, wifisleep), NULL, 0},
#endif
    CONFIGEND,
};
#if FEATURE_NTP
static struct st_xmlConfig ntpConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(NtpProfile, head.active), (Pointer)yesnoEnum, 0},
    {"enable", NULL, VALUE, TENUM, OFFANDSIZE(NtpProfile, enable), (Pointer)enDisableEnum, 0},
#if (DST == FYES)
    {"dstEnable", NULL, VALUE, TENUM, OFFANDSIZE(NtpProfile, dstEnable), (Pointer)enDisableEnum, 0},
#endif
    {"timeZone", NULL, VALUE, TINT, OFFANDSIZE(NtpProfile, time_zone), NULL, 0},
    {"NTPServer1", NULL, VALUE, TIPADDR, OFFANDSIZE(NtpProfile, NTPServerIP1), NULL, 0},
    {"NTPServer2", NULL, VALUE, TIPADDR, OFFANDSIZE(NtpProfile, NTPServerIP2), NULL, 0},
    {"NTPServer3", NULL, VALUE, TIPADDR, OFFANDSIZE(NtpProfile, NTPServerIP3), NULL, 0},
    CONFIGEND
};
#endif
#if FEATURE_LANG
static struct st_xmlConfig langConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(LangProfile, head.active), (Pointer)yesnoEnum, 0},
    {"lang", NULL, VALUE, TSTR, OFFANDSIZE(LangProfile, lang), NULL, 0},
    CONFIGEND
};
#endif
#if (FEATURE_AP_MAIL == FYES)
static struct st_xmlConfig apMailConfig[] = {
	{"active", NULL, VALUE, TENUM, OFFANDSIZE(ApMailProfile, head.active), (Pointer)yesnoEnum, 0},
    {"spaceTime", NULL, VALUE, TINT, OFFANDSIZE(ApMailProfile, spaceTime), NULL, 0},
	{"senderEmail", NULL, VALUE, TSTR, OFFANDSIZE(ApMailProfile,senderEmail), NULL, 0},
	{"senderEmailPasswd", NULL, VALUE, TSTR, OFFANDSIZE(ApMailProfile,senderEmailPasswd), NULL, 0},
	{"receiverEmail", NULL, VALUE, TSTR, OFFANDSIZE(ApMailProfile,receiverEmail), NULL, 0},
	{"smtpServer", NULL, VALUE, TSTR, OFFANDSIZE(ApMailProfile,smtpServer), NULL, 0},
	{"emailTitle", NULL, VALUE, TSTR, OFFANDSIZE(ApMailProfile,emailTitle), NULL, 0},
	CONFIGEND
};
#endif
#if (FEATURE_AP_CONF_TEMP == FYES)
static struct st_xmlConfig  apConfTempConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(ApConfTempProfile, head.active), (Pointer)yesnoEnum, 0},
    {"tempName", NULL, VALUE, TSTR, OFFANDSIZE(ApConfTempProfile, tempName), NULL, 0},
    {"wireless", NULL, VALUE, TENUM, OFFANDSIZE(ApConfTempProfile, wireless), (Pointer)enDisableEnum, 0},
    {"sgi", NULL, VALUE, TENUM, OFFANDSIZE(ApConfTempProfile, sgi), (Pointer)enDisableEnum, 0},
    {"preamble", NULL, VALUE, TENUM, OFFANDSIZE(ApConfTempProfile, preamble), (Pointer)enDisableEnum, 0},
    {"wmm", NULL, VALUE, TENUM, OFFANDSIZE(ApConfTempProfile, wmm), (Pointer)enDisableEnum, 0},
    {"rate", NULL, VALUE, TENUM, OFFANDSIZE(ApConfTempProfile, rate), (Pointer)wlanRateEnum, 0},
    {"wlFre", NULL, VALUE, TINT, OFFANDSIZE(ApConfTempProfile, wlFre), NULL, 0},
    {"bw", NULL, VALUE, TINT, OFFANDSIZE(ApConfTempProfile, bw), NULL, 0},
    {"channel", NULL, VALUE, TINT, OFFANDSIZE(ApConfTempProfile, channel), NULL, 0},
    {"power", NULL, VALUE, TINT, OFFANDSIZE(ApConfTempProfile, power), NULL, 0},
    {"mode", NULL, VALUE, TINT, OFFANDSIZE(ApConfTempProfile, mode), NULL, 0},
    {"manPower", NULL, VALUE, TENUM, OFFANDSIZE(ApConfTempProfile, manPower), (Pointer)wlanManualPowerEnum, 0},
    {"beaconPeriod", NULL, VALUE, TINT, OFFANDSIZE(ApConfTempProfile, BeaconPeriod), NULL, 0},
    {"VChanWidth", NULL, VALUE, TINT, OFFANDSIZE(ApConfTempProfile, VChanWidth), NULL, 0},
    CONFIGEND
};
#endif
#if (FEATURE_AP_SCAN == FYES)
static struct st_xmlConfig  apScanConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(ApScanProfile, head.active), (Pointer)yesnoEnum, 0},
    {"maxAp", NULL, VALUE, TINT, OFFANDSIZE(ApScanProfile, maxAp), NULL, 0},
    {"autoScan", NULL, VALUE, TENUM, OFFANDSIZE(ApScanProfile, autoScan), (Pointer)yesnoEnum, 0},
    {"scanAll", NULL, VALUE, TENUM, OFFANDSIZE(ApScanProfile, scanAll), (Pointer)enDisableEnum, 0},
    CONFIGEND
};
#endif
#if (FEATURE_AP_SW_MNG == FYES)
static struct st_xmlConfig  apSwMngConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(ApSwMngProfile, head.active), (Pointer)yesnoEnum, 0},
    {"sw_name", NULL, VALUE, TSTR, OFFANDSIZE(ApSwMngProfile, sw_name), NULL, 0},
    {"sw_version", NULL, VALUE, TSTR, OFFANDSIZE(ApSwMngProfile, sw_version), NULL, 0},
    {"sw_model", NULL, VALUE, TSTR, OFFANDSIZE(ApSwMngProfile, sw_model), NULL, 0},
    {"hw_version", NULL, VALUE, TSTR, OFFANDSIZE(ApSwMngProfile, hw_version), NULL, 0},
    {"defaulted", NULL, VALUE, TINT, OFFANDSIZE(ApSwMngProfile, defaulted), NULL, 0},
    CONFIGEND
};
#endif
#if (FEATURE_AP_BASIC_INFO == FYES)
static struct st_xmlConfig  apBasicConfConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(ApBasicConfProfile, head.active), (Pointer)yesnoEnum, 0},
    {"devName", NULL, VALUE, TSTR, OFFANDSIZE(ApBasicConfProfile, dev_name), NULL, 0},
    {"dhcpEn", NULL, VALUE, TINT, OFFANDSIZE(ApBasicConfProfile, dhcp_en), NULL, 0},
    {"ip", NULL, VALUE, TINT, OFFANDSIZE(ApBasicConfProfile, ip_addr), NULL, 0},
    {"mask", NULL, VALUE, TINT, OFFANDSIZE(ApBasicConfProfile, net_mask), NULL, 0},
    {"pswd", NULL, VALUE, TSTR, OFFANDSIZE(ApBasicConfProfile, password), NULL, 0},
    //{"client", NULL, VALUE, TINT, OFFANDSIZE(ApBasicConfProfile, max_clt), NULL, 0},
    {"channel", NULL, VALUE, TINT, OFFANDSIZE(ApBasicConfProfile, channel), NULL, 0},
	{"dbm", NULL, VALUE, TINT, OFFANDSIZE(ApBasicConfProfile, dbm), NULL, 0},
#if (WIRELESS_5G == FYES)
    {"dbm5g", NULL, VALUE, TINT, OFFANDSIZE(ApBasicConfProfile, dbm_5g), NULL, 0},
#endif
    CONFIGEND
};
#endif
#if (CAPWAP_CONFIG_AP == FYES)
static struct st_xmlConfig apCapwapConfig[] = {
     {"active", NULL, VALUE, TENUM, OFFANDSIZE(ApCapwapConfigProfile, head.active), (Pointer)yesnoEnum, 0},
     {"name", NULL, VALUE, TSTR, OFFANDSIZE(ApCapwapConfigProfile, head.name), NULL, 0},
     {"dns1",NULL,VALUE,TIPADDR,OFFANDSIZE(ApCapwapConfigProfile, pd),NULL,0},
     {"dns2",NULL,VALUE,TIPADDR,OFFANDSIZE(ApCapwapConfigProfile, sd),NULL,0},
 //    {"acHost", NULL, VALUE, TSTR, OFFANDSIZE(ApCapwapConfigProfile, acHostInfo), NULL, 0},
     {"acHost0", NULL, VALUE, TSTR, OFFANDSIZE(ApCapwapConfigProfile, acHostInfo[0]), NULL, 0},
     {"acHost1", NULL, VALUE, TSTR, OFFANDSIZE(ApCapwapConfigProfile, acHostInfo[1]), NULL, 0},
     {"acHost2", NULL, VALUE, TSTR, OFFANDSIZE(ApCapwapConfigProfile, acHostInfo[2]), NULL, 0},
    CONFIGEND
};
#endif
#if (FEATURE_AP_EFFECT_CONF == FYES)
static struct st_xmlConfig  apEffectConfConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(ApEffectConfProfile, head.active), (Pointer)yesnoEnum, 0},
    //{"apEffectConf", NULL, VALUE, TENUM, OFFANDSIZE(ApEffectConfProfile, config), (Pointer)yesnoEnum, 0},
    CONFIGEND
};
#endif
#if (FIT_AP_SERVICE_ZONE == FYES)
static struct st_xmlConfig fitApAuthWpaPskConfig[] = {
	{"wVersion", NULL, VALUE, TENUM, OFFANDSIZE(FitApSzProfile, wpaPskSecurity.wVersion), (Pointer)wVersionEnum, 0},
	{"sAlg", NULL, VALUE, TENUM, OFFANDSIZE(FitApSzProfile, wpaPskSecurity.sAlg), (Pointer)sAlgEnum, 0},
	{"sharePasswd", NULL, VALUE, TSTR, OFFANDSIZE(FitApSzProfile, wpaPskSecurity.sharePasswd), NULL, 0},
	{"updataTime", NULL, VALUE, TINT, OFFANDSIZE(FitApSzProfile, wpaPskSecurity.updataTime), NULL, 0},
	CONFIGEND,
};

static struct st_xmlConfig fitApAuthWpaConfig[] = {
	{"wVersion", NULL, VALUE, TENUM, OFFANDSIZE(FitApSzProfile, wpaSecurity.wVersion), (Pointer)wVersionEnum, 0},
	{"sAlg", NULL, VALUE, TENUM, OFFANDSIZE(FitApSzProfile, wpaSecurity.sAlg), (Pointer)sAlgEnum, 0},
	{"radiusIp", NULL, VALUE, TIPADDR, OFFANDSIZE(FitApSzProfile, wpaSecurity.radiusIp), NULL, 0},
	{"radiusPort", NULL, VALUE, TINT, OFFANDSIZE(FitApSzProfile, wpaSecurity.radiusPort), NULL, 0},
	{"radiusPasswd", NULL, VALUE, TSTR, OFFANDSIZE(FitApSzProfile, wpaSecurity.radiusPasswd), NULL, 0},
	{"updataTime", NULL, VALUE, TINT, OFFANDSIZE(FitApSzProfile, wpaSecurity.updataTime), NULL, 0},
	CONFIGEND,
};

static struct st_xmlConfig fitApKeyNumConfig[] = {
	{"key_state_action", NULL, VALUE, TENUM, OFFANDSIZE(FitApSzProfile, wepSecurity.kNum1.key_state_action), (Pointer)yesnoEnum, 0},
	{"passwd", NULL, VALUE, TSTR, OFFANDSIZE(FitApSzProfile, wepSecurity.kNum1.passwd), NULL, 0},
	{"kFmt", NULL, VALUE, TENUM, OFFANDSIZE(FitApSzProfile, wepSecurity.kNum1.kFmt), (Pointer)kFmtEnum, 0},
	CONFIGEND,
};

static struct st_xmlConfig fitApAuthWepConfig[] = {
	{"auMethod", NULL, VALUE, TENUM, OFFANDSIZE(FitApSzProfile, wepSecurity.auMethod), (Pointer)authMethodEnum, 0},
	{"kType", NULL, VALUE, TENUM, OFFANDSIZE(FitApSzProfile, wepSecurity.kType), (Pointer)keyTypeEnum, 0},
	{"kNum1", fitApKeyNumConfig, 0, 0, 0, 0, 0},
	{"kNum2", fitApKeyNumConfig, 0, 0, 0, 0, 0},
	{"kNum3", fitApKeyNumConfig, 0, 0, 0, 0, 0},
	{"kNum4", fitApKeyNumConfig, 0, 0, 0, 0, 0},
	CONFIGEND,
};

static struct st_xmlConfig FitApSZConfig[] = {
	{"active", NULL, VALUE, TENUM, OFFANDSIZE(FitApSzProfile, head.active), (Pointer)yesnoEnum, 0},
	//{"serviceEn", NULL, VALUE, TENUM, OFFANDSIZE(FitApSzProfile, serviceEn), (Pointer)yesnoEnum, 0},
	{"ssid", NULL, VALUE, TSTR, OFFANDSIZE(FitApSzProfile, ssid), NULL, 0},
	{"ssidBcast", NULL, VALUE, TENUM, OFFANDSIZE(FitApSzProfile, ssidBcast), (Pointer)yesnoEnum, 0},
	{"ssidIsolate", NULL, VALUE, TENUM, OFFANDSIZE(FitApSzProfile, ssidIsolate), (Pointer)yesnoEnum, 0},
	{"vid", NULL, VALUE, TINT, OFFANDSIZE(FitApSzProfile, vid), NULL, 0},
	{"client", NULL, VALUE, TINT, OFFANDSIZE(FitApSzProfile, client), NULL, 0},
#if (SZ_SHARE_LIMIT_AP == FYES)
	{"limitType", NULL, VALUE, TINT, OFFANDSIZE(FitApSzProfile, limit_type), NULL, 0},
 	{"limitDown", NULL, VALUE, TINT, OFFANDSIZE(FitApSzProfile, limit_down), NULL, 0},
 	{"limitUp", NULL, VALUE, TINT, OFFANDSIZE(FitApSzProfile, limit_up), NULL, 0},
#endif
	//{"dbm", NULL, VALUE, TINT, OFFANDSIZE(FitApSzProfile, dbm), NULL, 0},
	{"securMode", NULL, VALUE, TENUM, OFFANDSIZE(FitApSzProfile, securMode), (Pointer)uthModeEnum, 0},
	{"wepSecurity", fitApAuthWepConfig, 0, 0, 0, 0, 0},
	{"wpaSecurity", fitApAuthWpaConfig, 0, 0, 0, 0, 0},
	{"wpaPskSecurity", fitApAuthWpaPskConfig, 0, 0, 0, 0, 0},
	CONFIGEND,
};
#if (WIRELESS_5G == FYES)
static struct st_xmlConfig FitAp5gSZConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(FitAp5gSzProfile, head.active), (Pointer)yesnoEnum, 0},
    {"ssid", NULL, VALUE, TSTR, OFFANDSIZE(FitAp5gSzProfile, ssid), NULL, 0},
    {"ssidBcast", NULL, VALUE, TENUM, OFFANDSIZE(FitAp5gSzProfile, ssidBcast), (Pointer)yesnoEnum, 0},
    {"ssidIsolate", NULL, VALUE, TENUM, OFFANDSIZE(FitAp5gSzProfile, ssidIsolate), (Pointer)yesnoEnum, 0},
    {"vid", NULL, VALUE, TINT, OFFANDSIZE(FitAp5gSzProfile, vid), NULL, 0},
    {"client", NULL, VALUE, TINT, OFFANDSIZE(FitAp5gSzProfile, client), NULL, 0},
#if (SZ_SHARE_LIMIT_AP == FYES)
    {"limitType", NULL, VALUE, TINT, OFFANDSIZE(FitAp5gSzProfile, limit_type), NULL, 0},
    {"limitDown", NULL, VALUE, TINT, OFFANDSIZE(FitAp5gSzProfile, limit_down), NULL, 0},
    {"limitUp", NULL, VALUE, TINT, OFFANDSIZE(FitAp5gSzProfile, limit_up), NULL, 0},
#endif
    // {"dbm", NULL, VALUE, TINT, OFFANDSIZE(FitAp5gSzProfile, dbm), NULL, 0},
    {"securMode", NULL, VALUE, TENUM, OFFANDSIZE(FitAp5gSzProfile, securMode), (Pointer)uthModeEnum, 0},
    {"wepSecurity", fitApAuthWepConfig, 0, 0, 0, 0, 0},
    {"wpaSecurity", fitApAuthWpaConfig, 0, 0, 0, 0, 0},
    {"wpaPskSecurity", fitApAuthWpaPskConfig, 0, 0, 0, 0, 0},
    CONFIGEND,
};
#endif
#endif
#if (AP_BLACK_WHITE_LIST == FYES)
static struct st_xmlConfig apBlackWhiteListConfig[] = {
	{"active", NULL, VALUE, TENUM, OFFANDSIZE(ApBlackWhiteListProfile, head.active), (Pointer)yesnoEnum, 0},
	{"mode", NULL, VALUE, TINT, OFFANDSIZE(ApBlackWhiteListProfile, mode), NULL, 0},
	{"num", NULL, VALUE, TINT, OFFANDSIZE(ApBlackWhiteListProfile, num), NULL, 0},
	{"sz", NULL, VALUE, TSTR, OFFANDSIZE(ApBlackWhiteListProfile, sz), NULL, 0},
	{"mac1", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[0].addr), 0, 0},
	{"mac2", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[1]), 0, 0},
	{"mac3", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[2]), 0, 0},
	{"mac4", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[3]), 0, 0},
	{"mac5", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[4]), 0, 0},
	{"mac6", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[5]), 0, 0},
	{"mac7", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[6]), 0, 0},
	{"mac8", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[7]), 0, 0},
	{"mac9", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[8]), 0, 0},
	{"mac10", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[9]), 0, 0},
	{"mac11", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[10]), 0, 0},
	{"mac12", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[11]), 0, 0},
	{"mac13", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[12]), 0, 0},
	{"mac14", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[13]), 0, 0},
	{"mac15", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[14]), 0, 0},
	{"mac16", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[15]), 0, 0},
#if 0
	{"mac17", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[16]), 0, 0},
	{"mac18", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[17]), 0, 0},
	{"mac19", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[18]), 0, 0},
	{"mac20", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[19]), 0, 0},
	{"mac21", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[20]), 0, 0},
	{"mac22", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[21]), 0, 0},
	{"mac23", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[22]), 0, 0},
	{"mac24", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[23]), 0, 0},
	{"mac25", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[24]), 0, 0},
	{"mac26", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[25]), 0, 0},
	{"mac27", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[26]), 0, 0},
	{"mac28", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[27]), 0, 0},
	{"mac29", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[28]), 0, 0},
	{"mac30", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[29]), 0, 0},
	{"mac31", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[30]), 0, 0},
	{"mac32", NULL, VALUE, TMACADDR, OFFANDSIZE(ApBlackWhiteListProfile, mac[31]), 0, 0},
#endif
	CONFIGEND,
};
#endif
#if (HTTP_CACHE == FYES)
static struct st_xmlConfig cacheConfig[] = {
	{"active",NULL,VALUE,TENUM,OFFANDSIZE(CacheProfile,head.active),(Pointer)yesnoEnum,0},
	{"size", NULL, VALUE, TINT, OFFANDSIZE(CacheProfile,size), NULL, 0},
	{"cacheEnable",NULL,VALUE,TENUM,OFFANDSIZE(CacheProfile,cacheEnable),(Pointer)enDisableEnum,0},
	{"youkuVideoEnable",NULL,VALUE,TENUM,OFFANDSIZE(CacheProfile,youkuVideoEnable),(Pointer)enDisableEnum,0},
	{"tudouVideoEnable",NULL,VALUE,TENUM,OFFANDSIZE(CacheProfile,tudouVideoEnable),(Pointer)enDisableEnum,0},
	{"tcVideoEnable",NULL,VALUE,TENUM,OFFANDSIZE(CacheProfile,tcVideoEnable),(Pointer)enDisableEnum,0},
	{"sinaVideoEnable",NULL,VALUE,TENUM,OFFANDSIZE(CacheProfile,sinaVideoEnable),(Pointer)enDisableEnum,0},
	{"ifengVideoEnable",NULL,VALUE,TENUM,OFFANDSIZE(CacheProfile,ifengVideoEnable),(Pointer)enDisableEnum,0},
	{"letvVideoEnable",NULL,VALUE,TENUM,OFFANDSIZE(CacheProfile,letvVideoEnable),(Pointer)enDisableEnum,0},
	{"v56VideoEnable",NULL,VALUE,TENUM,OFFANDSIZE(CacheProfile,v56VideoEnable),(Pointer)enDisableEnum,0},
	{"funshionVideoEnable",NULL,VALUE,TENUM,OFFANDSIZE(CacheProfile,funshionVideoEnable),(Pointer)enDisableEnum,0},
	{"baiduMp3Enable",NULL,VALUE,TENUM,OFFANDSIZE(CacheProfile,baiduMp3Enable),(Pointer)enDisableEnum,0},
	{"sinaPageEnable",NULL,VALUE,TENUM,OFFANDSIZE(CacheProfile,sinaPageEnable),(Pointer)enDisableEnum,0},
	CONFIGEND,
};
#endif

#if (NET_MODE_SWITCH == FYES)
static struct st_xmlConfig netModeConfig[] = {
	{"active",NULL,VALUE,TENUM,OFFANDSIZE(NetModeSwitchProfile,head.active),(Pointer)yesnoEnum,0},
	{"bridgeEnable",NULL,VALUE,TENUM,OFFANDSIZE(NetModeSwitchProfile,bridgeEnable),(Pointer)enDisableEnum,0},
	CONFIGEND,
};
#endif

#if (BRIDGE == FYES)
static struct st_xmlConfig bridgeInterfaceConfig[] = {
	{"if0Enable",NULL,VALUE,TENUM,OFFANDSIZE(BridgeProfile,interface[0].enable),(Pointer)yesnoEnum,0},
	{"if0Name",NULL,VALUE,TSTR,OFFANDSIZE(BridgeProfile,interface[0].ifName),NULL,0},
	{"if0Mac", NULL, VALUE, TMACADDR, OFFANDSIZE(BridgeProfile,interface[0].mac), NULL, 0},
	{"if1Enable",NULL,VALUE,TENUM,OFFANDSIZE(BridgeProfile,interface[1].enable),(Pointer)yesnoEnum,0},
	{"if1Name",NULL,VALUE,TSTR,OFFANDSIZE(BridgeProfile,interface[1].ifName),NULL,0},
	{"if1Mac", NULL, VALUE, TMACADDR, OFFANDSIZE(BridgeProfile,interface[1].mac), NULL, 0},
	{"if2Enable",NULL,VALUE,TENUM,OFFANDSIZE(BridgeProfile,interface[2].enable),(Pointer)yesnoEnum,0},
	{"if2Name",NULL,VALUE,TSTR,OFFANDSIZE(BridgeProfile,interface[2].ifName),NULL,0},
	{"if2Mac", NULL, VALUE, TMACADDR, OFFANDSIZE(BridgeProfile,interface[2].mac), NULL, 0},
	{"if3Enable",NULL,VALUE,TENUM,OFFANDSIZE(BridgeProfile,interface[3].enable),(Pointer)yesnoEnum,0},
	{"if3Name",NULL,VALUE,TSTR,OFFANDSIZE(BridgeProfile,interface[3].ifName),NULL,0},
	{"if3Mac", NULL, VALUE, TMACADDR, OFFANDSIZE(BridgeProfile,interface[3].mac), NULL, 0},
	CONFIGEND,
};
static struct st_xmlConfig bridgeConfig[] = {
	{"active",NULL,VALUE,TENUM,OFFANDSIZE(BridgeProfile,head.active),(Pointer)yesnoEnum,0},
	{"ip",NULL,VALUE,TIPADDR,OFFANDSIZE(BridgeProfile,ip),NULL,0},
	{"nm",NULL,VALUE,TIPADDR,OFFANDSIZE(BridgeProfile,nm),NULL,0},
	{"gw",NULL,VALUE,TIPADDR,OFFANDSIZE(BridgeProfile,gw),NULL,0},
	{"dns1",NULL,VALUE,TIPADDR,OFFANDSIZE(BridgeProfile,dns1),NULL,0},
	{"dns2",NULL,VALUE,TIPADDR,OFFANDSIZE(BridgeProfile,dns2),NULL,0},
	{"interface",bridgeInterfaceConfig,0,0,0,0,0},
	CONFIGEND,
};
#endif
#if (CACHE_SERVER == FYES)
static struct st_xmlConfig cacheServerConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(CacheServerProfile, head.active), (Pointer)yesnoEnum, 0},
	{"cacheServerEnable",NULL,VALUE,TENUM,OFFANDSIZE(CacheServerProfile,cacheServerEnable),(Pointer)enDisableEnum,0},
    {"cacheServerIp", NULL, VALUE, TSTR, OFFANDSIZE(CacheServerProfile, cacheServerIp), NULL, 0},
    {"cacheServerPort", NULL, VALUE, TINT, OFFANDSIZE(CacheServerProfile, cacheServerPort), NULL, 0},
	CONFIGEND
};
#endif

#if (NET_SHARE_MANAGE == FYES)
static struct st_xmlConfig netShareManageConfig[] = {
	{"active",NULL,VALUE,TENUM,OFFANDSIZE(NetShareManageProfile,head.active),(Pointer)yesnoEnum,0},
	{"GlbEn",NULL,VALUE,TENUM,OFFANDSIZE(NetShareManageProfile,GlbEn),(Pointer)enDisableEnum,0},
	{"need_passwd",NULL,VALUE,TENUM,OFFANDSIZE(NetShareManageProfile,need_passwd),(Pointer)enDisableEnum,0},
	{"smbEnable", NULL, VALUE, TSTR, OFFANDSIZE(NetShareManageProfile, smbEnable), NULL, 0},
#if (UTT_SMART_UI == FYES)
	{"netShareEn",NULL,VALUE,TENUM,OFFANDSIZE(NetShareManageProfile,netShareEn),(Pointer)enDisableEnum,0},
	{"sambaEn",NULL,VALUE,TENUM,OFFANDSIZE(NetShareManageProfile,sambaEn),(Pointer)enDisableEnum,0},
	{"ftpEn",NULL,VALUE,TENUM,OFFANDSIZE(NetShareManageProfile,ftpEn),(Pointer)enDisableEnum,0},
	{"allow_wan_visit",NULL,VALUE,TENUM,OFFANDSIZE(NetShareManageProfile,allow_wan_visit),(Pointer)enDisableEnum,0},
	{"change",NULL,VALUE,TINT,OFFANDSIZE(NetShareManageProfile,change),NULL,0},
	{"dirName", NULL, VALUE, TSTR, OFFANDSIZE(NetShareManageProfile, dirName), NULL, 0},
	{"net_share_file_path", NULL, VALUE, TSTR, OFFANDSIZE(NetShareManageProfile, net_share_file_path), NULL, 0},
#endif
	CONFIGEND
};
#endif

#if (FTP_SERVER == FYES)
static struct st_xmlConfig ftpServerConfig[] = {
	{"active",NULL,VALUE,TENUM,OFFANDSIZE(FtpServerProfile,head.active),(Pointer)yesnoEnum,0},
	{"ftpEnable",NULL,VALUE,TENUM,OFFANDSIZE(FtpServerProfile,ftpEnable),(Pointer)enDisableEnum,0},
	{"allow_wan_visit",NULL,VALUE,TENUM,OFFANDSIZE(FtpServerProfile,allow_wan_visit),(Pointer)enDisableEnum,0},
	{"ftp_port",NULL,VALUE,TINT,OFFANDSIZE(FtpServerProfile,ftp_port),NULL,0},
#if (UTT_SMART_UI == FYES)
	{"need_passwd",NULL,VALUE,TENUM,OFFANDSIZE(FtpServerProfile,need_passwd),(Pointer)enDisableEnum,0},
	{"change",NULL,VALUE,TINT,OFFANDSIZE(FtpServerProfile,change),NULL,0},
#endif
	CONFIGEND
};

static struct st_xmlConfig ftpShareDirConfig[] = {
	{"active",NULL,VALUE,TENUM,OFFANDSIZE(FtpShareDirProfile,head.active),(Pointer)yesnoEnum,0},
    {"net_share_volume_name", NULL, VALUE, TSTR, OFFANDSIZE(FtpShareDirProfile, net_share_volume_name), NULL, 0},
    {"net_share_volume_path", NULL, VALUE, TSTR, OFFANDSIZE(FtpShareDirProfile, net_share_volume_path), NULL, 0},
	{"select_all",NULL,VALUE,TENUM,OFFANDSIZE(FtpShareDirProfile,select_all),(Pointer)enDisableEnum,0},
    {"net_share_file_path", NULL, VALUE, TSTR, OFFANDSIZE(FtpShareDirProfile, net_share_file_path), NULL, 0},
	CONFIGEND
};
#endif

#if (NET_SHARE_USER == FYES)
static struct st_xmlConfig netShareUserConfig[] = {
	{"active",NULL,VALUE,TENUM,OFFANDSIZE(NetShareUserProfile,head.active),(Pointer)yesnoEnum,0},
    {"password", NULL, VALUE, TSTR, OFFANDSIZE(NetShareUserProfile, password), NULL, 0},
	{"jurisdiction",NULL,VALUE,TENUM,OFFANDSIZE(NetShareUserProfile,jurisdiction),(Pointer)netShareJurisdictionEnum,0},
	{"allow_ftp",NULL,VALUE,TENUM,OFFANDSIZE(NetShareUserProfile,allow_ftp),(Pointer)enDisableEnum,0},
    CONFIGEND
};
#endif


#if (NET_SNIPER == FYES)
static struct st_xmlConfig netSniperConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(NetSniperProfile, head.active), (Pointer)yesnoEnum, 0},
    {"ipGrpName", NULL, VALUE, TSTR, OFFANDSIZE(NetSniperProfile,ipGrpName), NULL, 0},
    {"ctlTime", NULL, VALUE, TINT, OFFANDSIZE(NetSniperProfile,ctlTime), NULL, 0},
    {"linearIntvl", NULL, VALUE, TINT, OFFANDSIZE(NetSniperProfile,linearIntvl), NULL, 0},
    {"chkIntvl", NULL, VALUE, TINT, OFFANDSIZE(NetSniperProfile,chkIntvl), NULL, 0},
    {"chkCnt", NULL, VALUE, TINT, OFFANDSIZE(NetSniperProfile,chkCnt), NULL, 0},
	CONFIGEND,
};
#endif

#if (UTT_SMART_UI == FYES)
#if (APP_MENU == FYES)
static struct st_xmlConfig shortCutMenuConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(ShortCutMenuProfile, head.active), (Pointer)yesnoEnum, 0},
    {"item1", NULL, VALUE, TSTR, OFFANDSIZE(ShortCutMenuProfile, appName[0]), NULL, 0},
    {"item2", NULL, VALUE, TSTR, OFFANDSIZE(ShortCutMenuProfile, appName[1]), NULL, 0},
    {"item3", NULL, VALUE, TSTR, OFFANDSIZE(ShortCutMenuProfile, appName[2]), NULL, 0},
    {"item4", NULL, VALUE, TSTR, OFFANDSIZE(ShortCutMenuProfile, appName[3]), NULL, 0},
    {"item5", NULL, VALUE, TSTR, OFFANDSIZE(ShortCutMenuProfile, appName[4]), NULL, 0},
    CONFIGEND
};
#endif
#endif

#if (ADFILTER == FYES)
static struct st_xmlConfig  adfilterConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(AdfilterProfile, head.active), (Pointer)yesnoEnum, 0},
#if (APP_POLICY_REMOTE_UPDATE == FYES)
    {"policyVersion", NULL, VALUE, TINT, OFFANDSIZE(AdfilterProfile,policyVersion), NULL, 0},
    {"oldVersion", NULL, VALUE, TINT, OFFANDSIZE(AdfilterProfile,oldVersion), NULL, 0},
    {"newVersion", NULL, VALUE, TINT, OFFANDSIZE(AdfilterProfile,newVersion), NULL, 0},
#endif
    {"originalHost0", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile,originalHost[0]), NULL, 0},
    {"originalUri0", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, originalUri[0]), NULL, 0},
    {"host0", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, host[0]), NULL, 0},
    {"uri0", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, uri[0]), NULL, 0},
    {"normalHostIp0", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, normalHostIp[0]), NULL, 0},
    {"originalHost1", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile,originalHost[1]), NULL, 0},
    {"originalUri1", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, originalUri[1]), NULL, 0},
    {"host1", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, host[1]), NULL, 0},
    {"uri1", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, uri[1]), NULL, 0},
    {"normalHostIp1", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, normalHostIp[1]), NULL, 0},
    {"originalHost2", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile,originalHost[2]), NULL, 0},
    {"originalUri2", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, originalUri[2]), NULL, 0},
    {"host2", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, host[2]), NULL, 0},
    {"uri2", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, uri[2]), NULL, 0},
    {"normalHostIp2", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, normalHostIp[2]), NULL, 0},
    {"originalHost3", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile,originalHost[3]), NULL, 0},
    {"originalUri3", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, originalUri[3]), NULL, 0},
    {"host3", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, host[3]), NULL, 0},
    {"uri3", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, uri[3]), NULL, 0},
    {"normalHostIp3", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, normalHostIp[3]), NULL, 0},
    {"originalHost4", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile,originalHost[4]), NULL, 0},
    {"originalUri4", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, originalUri[4]), NULL, 0},
    {"host4", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, host[4]), NULL, 0},
    {"uri4", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, uri[4]), NULL, 0},
    {"normalHostIp4", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, normalHostIp[4]), NULL, 0},
    {"originalHost5", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile,originalHost[5]), NULL, 0},
    {"originalUri5", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, originalUri[5]), NULL, 0},
    {"host5", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, host[5]), NULL, 0},
    {"uri5", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, uri[5]), NULL, 0},
    {"normalHostIp5", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, normalHostIp[5]), NULL, 0},
    {"originalHost6", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile,originalHost[6]), NULL, 0},
    {"originalUri6", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, originalUri[6]), NULL, 0},
    {"host6", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, host[6]), NULL, 0},
    {"uri6", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, uri[6]), NULL, 0},
    {"normalHostIp6", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, normalHostIp[6]), NULL, 0},
    {"originalHost7", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile,originalHost[7]), NULL, 0},
    {"originalUri7", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, originalUri[7]), NULL, 0},
    {"host7", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, host[7]), NULL, 0},
    {"uri7", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, uri[7]), NULL, 0},
    {"normalHostIp7", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, normalHostIp[7]), NULL, 0},
    {"originalHost8", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile,originalHost[8]), NULL, 0},
    {"originalUri8", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, originalUri[8]), NULL, 0},
    {"host8", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, host[8]), NULL, 0},
    {"uri8", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, uri[8]), NULL, 0},
    {"normalHostIp8", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, normalHostIp[8]), NULL, 0},
    {"originalHost9", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile,originalHost[9]), NULL, 0},
    {"originalUri9", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, originalUri[9]), NULL, 0},
    {"host9", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, host[9]), NULL, 0},
    {"uri9", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, uri[9]), NULL, 0},
    {"normalHostIp9", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, normalHostIp[9]), NULL, 0},
    {"originalHost10", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile,originalHost[10]), NULL, 0},
    {"originalUri10", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, originalUri[10]), NULL, 0},
    {"host10", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, host[10]), NULL, 0},
    {"uri10", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, uri[10]), NULL, 0},
    {"normalHostIp10", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, normalHostIp[10]), NULL, 0},
    {"originalHost11", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile,originalHost[11]), NULL, 0},
    {"originalUri11", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, originalUri[11]), NULL, 0},
    {"host11", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, host[11]), NULL, 0},
    {"uri11", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, uri[11]), NULL, 0},
    {"normalHostIp11", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, normalHostIp[11]), NULL, 0},
    {"originalHost12", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile,originalHost[12]), NULL, 0},
    {"originalUri12", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, originalUri[12]), NULL, 0},
    {"host12", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, host[12]), NULL, 0},
    {"uri12", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, uri[12]), NULL, 0},
    {"normalHostIp12", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, normalHostIp[12]), NULL, 0},
    {"originalHost13", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile,originalHost[13]), NULL, 0},
    {"originalUri13", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, originalUri[13]), NULL, 0},
    {"host13", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, host[13]), NULL, 0},
    {"uri13", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, uri[13]), NULL, 0},
    {"normalHostIp13", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, normalHostIp[13]), NULL, 0},
    {"originalHost14", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile,originalHost[14]), NULL, 0},
    {"originalUri14", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, originalUri[14]), NULL, 0},
    {"host14", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, host[14]), NULL, 0},
    {"uri14", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, uri[14]), NULL, 0},
    {"normalHostIp14", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, normalHostIp[14]), NULL, 0},
    {"originalHost15", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile,originalHost[15]), NULL, 0},
    {"originalUri15", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, originalUri[15]), NULL, 0},
    {"host15", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, host[15]), NULL, 0},
    {"uri15", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, uri[15]), NULL, 0},
    {"normalHostIp15", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, normalHostIp[15]), NULL, 0},
    {"originalHost16", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile,originalHost[16]), NULL, 0},
    {"originalUri16", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, originalUri[16]), NULL, 0},
    {"host16", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, host[16]), NULL, 0},
    {"uri16", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, uri[16]), NULL, 0},
    {"normalHostIp16", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, normalHostIp[16]), NULL, 0},
    {"originalHost17", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile,originalHost[17]), NULL, 0},
    {"originalUri17", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, originalUri[17]), NULL, 0},
    {"host17", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, host[17]), NULL, 0},
    {"uri17", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, uri[17]), NULL, 0},
    {"normalHostIp17", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, normalHostIp[17]), NULL, 0},
    {"originalHost18", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile,originalHost[18]), NULL, 0},
    {"originalUri18", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, originalUri[18]), NULL, 0},
    {"host18", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, host[18]), NULL, 0},
    {"uri18", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, uri[18]), NULL, 0},
    {"normalHostIp18", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, normalHostIp[18]), NULL, 0},
    {"originalHost19", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile,originalHost[19]), NULL, 0},
    {"originalUri19", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, originalUri[19]), NULL, 0},
    {"host19", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, host[19]), NULL, 0},
    {"uri19", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, uri[19]), NULL, 0},
    {"normalHostIp19", NULL, VALUE, TSTR, OFFANDSIZE(AdfilterProfile, normalHostIp[19]), NULL, 0},
    CONFIGEND
};
#endif

#if (MAKE_MONEY == FYES)
static struct st_xmlConfig makeMoneyConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(MakeMoneyProfile, head.active), (Pointer)yesnoEnum, 0},
    {"MakeMoneyEn", NULL, VALUE, TENUM, OFFANDSIZE(MakeMoneyProfile, MakeMoneyEn), (Pointer)enDisableEnum, 0},
    {"SaftyEn", NULL, VALUE, TENUM, OFFANDSIZE(MakeMoneyProfile, SaftyEn), (Pointer)enDisableEnum, 0},
//    {"account", NULL, VALUE, TSTR, OFFANDSIZE(MakeMoneyProfile, account), NULL, 0},
    {"phone", NULL, VALUE, TSTR, OFFANDSIZE(MakeMoneyProfile, phoneNumber), NULL, 0},
    {"isBind", NULL, VALUE, TINT, OFFANDSIZE(MakeMoneyProfile, isBind), NULL, 0},
    {"ssid", NULL, VALUE, TSTR, OFFANDSIZE(MakeMoneyProfile, ssid), NULL, 0},
    {"ssid2", NULL, VALUE, TSTR, OFFANDSIZE(MakeMoneyProfile, ssid2), NULL, 0},
    {"ssid_5g", NULL, VALUE, TSTR, OFFANDSIZE(MakeMoneyProfile, ssid_5g), NULL, 0},
    {"ssid2_5g", NULL, VALUE, TSTR, OFFANDSIZE(MakeMoneyProfile, ssid2_5g), NULL, 0},
#if (WLAN_MULTI_SSID == FYES)
    {"encodeType", NULL, VALUE, TSTR, OFFANDSIZE(MakeMoneyProfile, encodeType), NULL, 0},
    {"encodeType_5g", NULL, VALUE, TSTR, OFFANDSIZE(MakeMoneyProfile, encodeType_5g), NULL, 0},
    {"encodeType2", NULL, VALUE, TSTR, OFFANDSIZE(MakeMoneyProfile, encodeType2), NULL, 0},
    {"encodeType2_5g", NULL, VALUE, TSTR, OFFANDSIZE(MakeMoneyProfile, encodeType2_5g), NULL, 0},
#endif
    {"TimeStart", NULL, VALUE, TSTR, OFFANDSIZE(MakeMoneyProfile,TimeStart), NULL, 0},
    {"TimeStop", NULL, VALUE, TSTR, OFFANDSIZE(MakeMoneyProfile, TimeStop), NULL, 0},
    {"charge", NULL, VALUE, TSTR, OFFANDSIZE(MakeMoneyProfile, charge), NULL, 0},
    {"FreeTime", NULL, VALUE, TINT, OFFANDSIZE(MakeMoneyProfile, FreeTime), NULL, 0},
    {"dRate", NULL, VALUE, TINT, OFFANDSIZE(MakeMoneyProfile, dRate), NULL, 0},
    {"uRate", NULL, VALUE, TINT, OFFANDSIZE(MakeMoneyProfile, uRate), NULL, 0},
    {"wifitime_active", NULL, VALUE, TENUM, OFFANDSIZE(MakeMoneyProfile, wifitime_active), (Pointer)yesnoEnum, 0},
    CONFIGEND
};
#endif
#if (CLOUD_MANAGE == FYES)
static struct st_xmlConfig cloudmanageConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(CloudManageProfile, head.active), (Pointer)yesnoEnum, 0}, 
/*profile中有，但不需要写flash，保留
    {"configNeedSyn", NULL, VALUE, TINT, OFFANDSIZE(CloudManageProfile, configNeedSyn), NULL, 0},
*/
    {"configVer", NULL, VALUE, TINT, OFFANDSIZE(CloudManageProfile, configVer), NULL, 0},
    CONFIGEND
};
#endif

#if (CLOUD_CONFIG == FYES)
static struct st_xmlConfig cloudconfigConfig[] = {
    {"active", NULL, VALUE, TENUM, OFFANDSIZE(CloudConfigProfile, head.active), (Pointer)yesnoEnum, 0}, 
    {"noticeBanMask", NULL, VALUE, TINT, OFFANDSIZE(CloudConfigProfile, noticeBanMask), NULL, 0},
/*64个mac*/
    {"mac1", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[0]), 0, 0}, 
    {"mac2", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[1]), 0, 0}, 
    {"mac3", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[2]), 0, 0}, 
    {"mac4", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[3]), 0, 0}, 
    {"mac5", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[4]), 0, 0}, 
    {"mac6", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[5]), 0, 0}, 
    {"mac7", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[6]), 0, 0}, 
    {"mac8", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[7]), 0, 0}, 
    {"mac9", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[8]), 0, 0}, 
    {"mac10", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[9]), 0, 0}, 
    {"mac11", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[10]), 0, 0}, 
    {"mac12", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[11]), 0, 0}, 
    {"mac13", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[12]), 0, 0}, 
    {"mac14", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[13]), 0, 0}, 
    {"mac15", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[14]), 0, 0}, 
    {"mac16", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[15]), 0, 0}, 
    {"mac17", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[16]), 0, 0}, 
    {"mac18", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[17]), 0, 0}, 
    {"mac19", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[18]), 0, 0}, 
    {"mac20", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[19]), 0, 0}, 
    {"mac21", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[20]), 0, 0}, 
    {"mac22", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[21]), 0, 0}, 
    {"mac23", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[22]), 0, 0}, 
    {"mac24", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[23]), 0, 0}, 
    {"mac25", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[24]), 0, 0}, 
    {"mac26", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[25]), 0, 0}, 
    {"mac27", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[26]), 0, 0}, 
    {"mac28", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[27]), 0, 0}, 
    {"mac29", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[28]), 0, 0}, 
    {"mac30", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[29]), 0, 0}, 
    {"mac31", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[30]), 0, 0}, 
    {"mac32", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[31]), 0, 0}, 
    {"mac33", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[32]), 0, 0}, 
    {"mac34", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[33]), 0, 0}, 
    {"mac35", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[34]), 0, 0}, 
    {"mac36", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[35]), 0, 0}, 
    {"mac37", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[36]), 0, 0}, 
    {"mac38", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[37]), 0, 0}, 
    {"mac39", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[38]), 0, 0}, 
    {"mac40", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[39]), 0, 0}, 
    {"mac41", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[40]), 0, 0}, 
    {"mac42", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[41]), 0, 0}, 
    {"mac43", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[42]), 0, 0}, 
    {"mac44", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[43]), 0, 0}, 
    {"mac45", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[44]), 0, 0}, 
    {"mac46", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[45]), 0, 0}, 
    {"mac47", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[46]), 0, 0}, 
    {"mac48", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[47]), 0, 0}, 
    {"mac49", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[48]), 0, 0}, 
    {"mac50", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[49]), 0, 0}, 
    {"mac51", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[50]), 0, 0}, 
    {"mac52", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[51]), 0, 0}, 
    {"mac53", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[52]), 0, 0}, 
    {"mac54", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[53]), 0, 0}, 
    {"mac55", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[54]), 0, 0}, 
    {"mac56", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[55]), 0, 0}, 
    {"mac57", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[56]), 0, 0}, 
    {"mac58", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[57]), 0, 0}, 
    {"mac59", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[58]), 0, 0}, 
    {"mac60", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[59]), 0, 0}, 
    {"mac61", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[60]), 0, 0}, 
    {"mac62", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[61]), 0, 0}, 
    {"mac63", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[62]), 0, 0}, 
    {"mac64", NULL, VALUE, TMACADDR, OFFANDSIZE(CloudConfigProfile, friendMac[63]), 0, 0},
/*64个*/ 
    {"friendName1", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[0]), NULL, 0},
    {"friendName2", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[1]), NULL, 0},
    {"friendName3", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[2]), NULL, 0},
    {"friendName4", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[3]), NULL, 0},
    {"friendName5", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[4]), NULL, 0},
    {"friendName6", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[5]), NULL, 0},
    {"friendName7", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[6]), NULL, 0},
    {"friendName8", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[7]), NULL, 0},
    {"friendName9", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[8]), NULL, 0},
    {"friendName10", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[9]), NULL, 0},
    {"friendName11", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[10]), NULL, 0},
    {"friendName12", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[11]), NULL, 0},
    {"friendName13", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[12]), NULL, 0},
    {"friendName14", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[13]), NULL, 0},
    {"friendName15", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[14]), NULL, 0},
    {"friendName16", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[15]), NULL, 0},
    {"friendName17", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[16]), NULL, 0},
    {"friendName18", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[17]), NULL, 0},
    {"friendName19", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[18]), NULL, 0},
    {"friendName20", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[19]), NULL, 0},
    {"friendName21", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[20]), NULL, 0},
    {"friendName22", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[21]), NULL, 0},
    {"friendName23", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[22]), NULL, 0},
    {"friendName24", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[23]), NULL, 0},
    {"friendName25", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[24]), NULL, 0},
    {"friendName26", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[25]), NULL, 0},
    {"friendName27", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[26]), NULL, 0},
    {"friendName28", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[27]), NULL, 0},
    {"friendName29", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[28]), NULL, 0},
    {"friendName30", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[29]), NULL, 0},
    {"friendName31", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[30]), NULL, 0},
    {"friendName32", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[31]), NULL, 0},
    {"friendName33", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[32]), NULL, 0},
    {"friendName34", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[33]), NULL, 0},
    {"friendName35", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[34]), NULL, 0},
    {"friendName36", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[35]), NULL, 0},
    {"friendName37", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[36]), NULL, 0},
    {"friendName38", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[37]), NULL, 0},
    {"friendName39", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[38]), NULL, 0},
    {"friendName40", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[39]), NULL, 0},
    {"friendName41", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[40]), NULL, 0},
    {"friendName42", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[41]), NULL, 0},
    {"friendName43", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[42]), NULL, 0},
    {"friendName44", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[43]), NULL, 0},
    {"friendName45", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[44]), NULL, 0},
    {"friendName46", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[45]), NULL, 0},
    {"friendName47", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[46]), NULL, 0},
    {"friendName48", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[47]), NULL, 0},
    {"friendName49", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[48]), NULL, 0},
    {"friendName50", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[49]), NULL, 0},
    {"friendName51", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[50]), NULL, 0},
    {"friendName52", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[51]), NULL, 0},
    {"friendName53", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[52]), NULL, 0},
    {"friendName54", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[53]), NULL, 0},
    {"friendName55", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[54]), NULL, 0},
    {"friendName56", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[55]), NULL, 0},
    {"friendName57", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[56]), NULL, 0},
    {"friendName58", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[57]), NULL, 0},
    {"friendName59", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[58]), NULL, 0},
    {"friendName60", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[59]), NULL, 0},
    {"friendName61", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[60]), NULL, 0},
    {"friendName62", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[61]), NULL, 0},
    {"friendName63", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[62]), NULL, 0},
    {"friendName64", NULL, VALUE, TSTR, OFFANDSIZE(CloudConfigProfile, friendName[63]), NULL, 0},
    CONFIGEND
};
#endif

struct st_xmlConfig  rootConfig[] = {
#if (AP_CONF == FYES)
	{"apConf", apConfConfig, INST, 0, 0, 0, 0, MIB_PROF_AP_CONF},
#endif
    {"sysConf", systemConfig, INST, 0, 0, 0, 0, MIB_PROF_SYSTEM},
    {"interface", interfaceConfig, INST, 0, 0, 0, 0, MIB_PROF_INTERFACE},
    {"user", userConfig, INST, 0, 0, 0, 0, MIB_PROF_USER},
#if (IPV6_INTERFACE == FYES)
    {"ipv6interface", ipv6interfaceConfig, INST, 0, 0, 0, 0, MIB_PROF_IPV6_INTERFACE},
#endif
#if (IPV6_RADVD == FYES)
    {"radvd", radvdConfig, INST, 0, 0, 0, 0, MIB_PROF_RADVD},
#endif
 #if(IPV6_ROUTE == FYES)
 {"ipv6RoutConfig",ipv6RoutConfigConfig,INST, 0, 0, 0, 0, MIB_PROF_IPV6_ROUTECONF},
#endif
#if (IPV6_DHCPV6 == FYES)
    {"dhcp6", dhcp6Config, INST, 0, 0, 0, 0, MIB_PROF_DHCP6},
#endif
#if(IPV6_NOTLISTPREFIXS == FYES)
    {"ipv6PrefixList",ipv6PrefixListConfig,INST, 0, 0, 0, 0, MIB_PROF_IPV6_NOTLISTPREFIXS},
#endif
#if(IPV6_TUNNEL == FYES)
    {"tunnel",tunnelConfig,INST, 0, 0, 0, 0, MIB_PROF_TUNNEL},
#endif
#if(IPV6_ISATAP == FYES)
    {"isatap",isatapConfig,INST, 0, 0, 0, 0, MIB_PROF_ISATAP},
#endif
#if (DHCP_SERVER == FYES)
    {"dhcpServer", dhcpServerConfig, INST, 0, 0, 0, 0, MIB_PROF_DHCP_SERVER},
#endif
#if (DHCP_POOL == FYES)
    {"dhcpPool", dhcpPoolConfig, INST, 0, 0, 0, 0, MIB_PROF_DHCP_POOL},
#endif
#if (VLAN_CONFIG == FYES)
    {"vlanConfig", vlanConfig, INST, 0, 0, 0, 0, MIB_PROF_VLAN_CONFIG},
#endif
#if (DNS_AGENCY == FYES)
	{"dnsAgency", dnsAgencyConfig, INST, 0, 0, 0, 0, MIB_PROF_DNS_AGENCY},
#endif
#if (FEATURE_SW_PORT_CONF == FYES)
    {"portConfig", swPortConfConfig, INST, 0, 0, 0, 0, MIB_PROF_SW_PORT_CONF},
#endif
#if (FEATURE_PORT_VLAN == FYES)
    {"portVlan", portVlanConfig, INST, 0, 0, 0, 0, MIB_PROF_PORT_VLAN},
#endif
#if (PVLAN_REDUCED == FYES)
    {"rddPortVlan", rddPortVlanConfig, INST, 0, 0, 0, 0, MIB_PROF_RDD_PORT_VLAN},
#endif
#if (L2TP_SERVER == FYES)
    {"l2tpServer", l2tpSrvConfig, INST, 0, 0, 0, 0, MIB_PROF_L2TP_SRV_GLOBAL},
    {"l2tpSrvAccount", l2tpSrvAccConfig, INST, 0, 0, 0, 0, MIB_PROF_L2TP_SRV_ACCOUNT},
#endif
#if (L2TP_CLIENT == FYES)
    {"l2tpClient", l2tpCliConfig, INST, 0, 0, 0, 0, MIB_PROF_L2TP_CLIENT},
#endif
#if (PPTP_SERVER == FYES)
    {"pptpServer", pptpSrvConfig, INST, 0, 0, 0, 0, MIB_PROF_PPTP_SRV_GLOBAL},
    {"pptpSrvAccount", pptpSrvAccConfig, INST, 0, 0, 0, 0, MIB_PROF_PPTP_SRV_ACCOUNT},
#endif
#if (PPTP_CLIENT)
    {"pptpClient", pptpCliConfig, INST, 0, 0, 0, 0, MIB_PROF_PPTP_CLI},
#endif
#if (FEATURE_SW_PORT_MIRROR == FYES)
	{"portMirror", portMirrorConfig, INST, 0, 0, 0, 0, MIB_PROF_MIRROR},
#endif
#if (GROUP_BASE == FYES)
    {"NetManage", NetManageConfig, INST, 0, 0, 0, 0, MIB_PROF_GROUP_BASE},
#endif
#if (FEATURE_SW_PORT_TRUNK == FYES)
	{"portTrunk", portTrunkConfig, INST, 0, 0, 0, 0, MIB_PROF_TRUNK},
#endif
#if (FEATURE_SW_SNMP == FYES)
    {"snmp", snmpConfig, INST, 0, 0, 0, 0, MIB_PROF_SNMP},
#endif
#if (SNMP == FYES)
    {"SNMP", snmpInfoConfig, INST, 0, 0, 0, 0, MIB_PROF_SNMP_INFO},
#endif
#if (AP_PERMISSION == FYES)
	{"ApPermission", ApPermissionConfig, INST, 0, 0, 0, 0, MIB_PROF_AP_PERMISSION},
#endif

#if (SERVICE_ZONE == FYES)
	{"ServiceZone", ServiceZoneConfig, INST, 0, 0, 0, 0, MIB_PROF_SERVICE_ZONE},
#endif

#if( (FEATURE_AP_MAC_FILTER == FYES)||(FIT_AP_MAC_FILTER == FYES))
	{"ApMacFilter", ApMacFilterConfig, INST, 0, 0, 0, 0, MIB_PROF_AP_MAC_FILTER},
#endif

#if (TAG_VLAN == FYES)
	{"TagVlan", TagVlanConfig, INST, 0, 0, 0, 0, MIB_PROF_TAG_VLAN},
#if (WAN_TAG_VLAN == FYES)
	{"WanTagVlan", WanTagVlanConfig, INST, 0, 0, 0, 0, MIB_PROF_WAN_TAG_VLAN},
#endif
#endif

#if (TASK_SCHEDULER == FYES)
    {"TaskSched", taskSchedConfig, INST, 0, 0, 0, 0, MIB_PROF_TASK_SCHED},
#endif
#if (FEATURE_ACL == FYES)
    {"aclEnable", aclStatusConfig, INST, 0, 0, 0, 0, MIB_PROF_ACL_STATUS},
    {"acl", aclConfig, INST, 0, 0, 0, 0, MIB_PROF_ACL},
#endif
#if (UTTPDB == FYES)
    {"pdb", PDB, INST, 0, 0, 0, 0, MIB_PROF_PDB_GLOBAL},
#endif
#if (STATIC_NAT == FYES)
    {"staticNat", StaticNatConfig, INST, 0, 0, 0, 0, MIB_PROF_STATIC_NAT},
#endif
#if (NAT_RULE == FYES)
    {"natRule", NatRuleConfig, INST, 0, 0, 0, 0, MIB_PROF_NAT_RULE},
#endif
#if (DMZ == FYES)
    {"dmz", DMZConfig, INST, 0, 0, 0, 0, MIB_PROF_DMZ_NAT},
#endif
#if (ADMIN_CONF == FYES)
    {"adminConf", adminConfConfig, INST, 0, 0, 0, 0, MIB_PROF_ADMIN_CONF},
#endif
#if (DNS_FILTER == FYES)
    {"dnsFilter", dnsFilterConfig, INST, 0, 0, 0, 0, MIB_PROF_DNS_FILTER},
#endif
#if (CLI_FOR_ENGINEER_ONLY == FYES)
    {"cliForEngineerOnly", cliForEngineerOnlyConfig, INST, 0, 0, 0, 0, MIB_PROF_CLI_FOR_ENGINEER_ONLY},
#endif
#if (WIFIDOG_AUTH == FYES)
#if (WHITELIST == 1)
    {"whiteList", whitelistConfig, INST, 0, 0, 0, 0, MIB_PROF_WHITE_LIST},
#endif
#endif
#if (EXCEPT_QQ == FYES)
    {"exceptQQ", exceptQQConfig, INST, 0, 0, 0, 0, MIB_PROF_EXCEPT_QQ},
#endif
#if (EXCEPT_MSN == FYES)
    {"exceptMSN", exceptMSNConfig, INST, 0, 0, 0, 0, MIB_PROF_EXCEPT_MSN},
#endif
#if (EXCEPT_ALI == FYES)
    {"exceptAli", exceptAliConfig, INST, 0, 0, 0, 0, MIB_PROF_EXCEPT_ALI},
#endif
#if (IP_GRP == FYES)
    {"ipGroup", ipGrpConfig, INST, 0, 0, 0, 0, MIB_PROF_IPGRP},
#endif
#if (PORT_GRP == FYES)
    {"portGroup", portGrpConfig, INST, 0, 0, 0, 0, MIB_PROF_PORTGRP},
#endif
#if (TIME_GRP == FYES)
    {"timeGroup", timeGrpConfig, INST, 0, 0, 0, 0, MIB_PROF_TIMEGRP},
#endif
#if (IP_SEC == FYES)
    {"ipsecConf", ipsecConfig, INST, 0, 0, 0, 0, MIB_PROF_IPSEC},
    {"transformsetConf", transformSetConfig, INST, 0, 0, 0, 0, MIB_PROF_TRANSFORMSET},
    {"isakmpConf", isakmpConfig, INST, 0, 0, 0, 0, MIB_PROF_ISAKMP},
    {"policyConf", isakmpPolicyConfig, INST, 0, 0, 0, 0, MIB_PROF_ISAKMP_POLICY},
#endif
#if (WIRELESS == FYES)
    {"wireless", wirelessConfig, INST, 0, 0, 0, 0, MIB_PROF_WIRELESS},
    {"wirelessMacFilter", wirelessMacFilterConfig, INST, 0, 0, 0, 0, MIB_PROF_WIRELESS_MAC_FILTER},
#if (WIRELESS_5G == FYES)
    {"wireless5gMacFilter", wireless5gMacFilterConfig, INST, 0, 0, 0, 0, MIB_PROF_WIRELESS_5G_MAC_FILTER},
#endif
#endif
#if (STATIC_ROUTE == FYES)
    {"staticRoute", staticRouteConfig, INST, 0, 0, 0, 0, MIB_PROF_STATIC_ROUTE},
#endif
#if (POLICY_ROUTE == FYES)
    {"policyRoute", policyRouteConfig, INST, 0, 0, 0, 0, MIB_PROF_POLICY_ROUTE},
#endif
#if (PARENTS_CONTROL == FYES)
    {"parentscontrol", parentsControlConfig, INST, 0, 0, 0, 0, MIB_PROF_PARENTS_CONTROL},
#endif

#if (FEATURE_NTP == FYES)
    {"ntp", ntpConfig, INST, 0, 0, 0, 0, MIB_PROF_NTP},
#endif
#if (FEATURE_LANG == FYES)
    {"Language", langConfig, INST, 0, 0, 0, 0, MIB_PROF_LANG},
#endif
#if (DDNS == FYES)
    {"ddns", ddnsConfig, INST, 0, 0, 0, 0, MIB_PROF_DDNS},
#endif
#if (FIREWALL == FYES)
    {"firewall", firewallConfig, INST, 0, 0, 0, 0, MIB_PROF_FIRE_WALL},
#endif
#if (PPPOE_SERVER == FYES)
    {"pppoeSrvGlo", pppoeSrvGloConfig, INST, 0, 0, 0, 0, MIB_PROF_PPPOE_SRV_GLO},
    {"pppoeSrvAcc", pppoeSrvAccConfig, INST, 0, 0, 0, 0, MIB_PROF_PPPOE_SRV_ACC},
#endif
#if (NOTICE == FYES)
    {"notice", noticeConfig, INST, 0, 0, 0, 0, MIB_PROF_NOTICE},
#endif
#if (CWMP == FYES)
    {"cwmp", cwmpConfig, INST, 0, 0, 0, 0, MIB_PROF_CWMP},
#endif
#if (WEB_AUTH == FYES)
    {"webauth", webauthConfig, INST, 0, 0, 0, 0, MIB_PROF_WEB_AUTH},
    {"webauthglobal", webauthglobalConfig, INST, 0, 0, 0, 0, MIB_PROF_WEB_AUTH_GLOBAL},
#endif
#if (LOG_MANAGE == FYES)
    {"logmanage", logmanageConfig, INST, 0, 0, 0, 0, MIB_PROF_LOG_MANAGE},
#endif
#if (AP_LOG_MANAGE == FYES)
    {"aplogmanage", aplogmanageConfig, INST, 0, 0, 0, 0, MIB_PROF_AP_LOG_MANAGE},
#endif
#if (SYSLOG == FYES)
    {"syslog", SyslogConfig, INST, 0, 0, 0, 0, MIB_PROF_SYSLOG},
#endif
#if (FEATURE_AP_SCAN == FYES)
    {"apScan", apScanConfig, INST, 0, 0, 0, 0, MIB_PROF_AP_SCAN},
#endif
#if (FEATURE_AP_SW_MNG == FYES)
    {"apSwMng", apSwMngConfig, INST, 0, 0, 0, 0, MIB_PROF_AP_SW_MNG},
#endif
#if (FEATURE_AP_BASIC_INFO == FYES)
    {"apBasicConf", apBasicConfConfig, INST, 0, 0, 0, 0, MIB_PROF_AP_BASIC_CONF},
#endif
#if (CAPWAP_CONFIG_AP == FYES)
    {"apCapwapConf",apCapwapConfig, INST, 0, 0, 0, 0, MIB_PROF_AP_CAPWAP_CONFIG},
#endif
#if (FAT_FIT_SWITCH == FYES)
    {"fatFitSwitch", fatFitSwitchConfig, INST, 0, 0, 0, 0, MIB_PROF_FAT_FIT_SWITCH},
#endif
#if (AC_WORK_MODE == FYES)
    {"acWorkMode", acWorkModeConfig, INST, 0, 0, 0, 0, MIB_PROF_AC_WORK_MODE},
#endif
#if (AP_LOAD_BALANCE == FYES)
    {"apLoadBalance",apLoadBalanceConfig,INST,0, 0, 0, 0, MIB_PROF_AP_LOAD_BALANCE },
#endif
#if 1	/* 瘦AP本地配置不需要在cliaaa.c 中的rootConfig数组里添加配置结构。 */
#if (FEATURE_AP_EFFECT_CONF == FYES)
    {"apEffectConf", apEffectConfConfig, INST, 0, 0, 0, 0, MIB_PROF_AP_EFFECT_CONF},
#endif
#if (FEATURE_AP_MAIL == FYES)
    {"apMail", apMailConfig, INST, 0, 0, 0, 0, MIB_PROF_AP_MAIL},
#endif
#if (FEATURE_AP_CONF_TEMP == FYES)
    {"apConfTemp", apConfTempConfig, INST, 0, 0, 0, 0, MIB_PROF_AP_CONF_TEMP},
#endif
#if (FIT_AP_SERVICE_ZONE == FYES)
    {"fitApSZ", FitApSZConfig, INST, 0, 0, 0, 0, MIB_PROF_FIT_AP_SZ},
#if (WIRELESS_5G == FYES)
    {"fitApSZ_5G", FitAp5gSZConfig, INST, 0, 0, 0, 0, MIB_PROF_FIT_AP_SZ_5G},
#endif
#endif
#if (P2P_LIMIT == FYES)
    {"p2pLimit", p2pLimitConfig, INST, 0, 0, 0, 0, MIB_PROF_P2P_LIMIT},
#endif
#endif
#if (SYS_LOG_INFO == FYES)
	{"SysLogInfo", SysLogInfoConfig, INST, 0, 0, 0, 0, MIB_PROF_SYS_LOG_INFO},
#endif
#if (EASY_TASK == FYES)
    {"easyTask", easyTaskConfig, INST, 0, 0, 0, 0, MIB_PROF_EASY_TASK},
#endif
#if (HTTP_CACHE == FYES)
	 {"cache",cacheConfig,INST,0,0,0,0,MIB_PROF_CACHE},
#endif
#if (NET_MODE_SWITCH == FYES)
	{"netMode",netModeConfig,INST,0,0,0,0,MIB_PROF_NET_MODE_SWITCH},
#endif
#if (BRIDGE== FYES)
	{"bridge",bridgeConfig,INST,0,0,0,0,MIB_PROF_BRIDGE},
#endif
#if (CACHE_SERVER == FYES)
	{"cacheServer",cacheServerConfig,INST,0,0,0,0,MIB_PROF_CACHE_SERVER},
#endif
#if (NET_SNIPER == FYES)
	{"netSniper",netSniperConfig,INST,0,0,0,0,MIB_PROF_NET_SNIPER},
#endif
#if (MAC_FILTER == FYES)
	{"macFilterGlobal",macFilterGlobalConfig,INST,0,0,0,0,MIB_PROF_MAC_FILTER_GLOBAL},
	{"macFilter", macFilterConfig, INST, 0, 0, 0, 0, MIB_PROF_MAC_FILTER},
#endif
#if (UTT_SMART_UI == FYES)
#if (APP_MENU == FYES)
    {"shortCutMenu",shortCutMenuConfig,INST,0,0,0,0, MIB_PROF_SHORTCUT_MENU},
#endif
#endif
#if (AP_BLACK_WHITE_LIST == FYES)
    {"apBlackWhiteList", apBlackWhiteListConfig, INST, 0, 0, 0, 0, MIB_PROF_AP_BLACK_WHITE_LIST},
#endif
#if (NET_SHARE_MANAGE == FYES)
	{"netShareManage", netShareManageConfig, INST, 0, 0, 0, 0, MIB_PROF_NET_SHARE_MANAGE},
#endif
#if (FTP_SERVER == FYES)
	{"ftpServer", ftpServerConfig, INST, 0, 0, 0, 0, MIB_PROF_FTP_SERVER},
	{"ftpSharedir", ftpShareDirConfig, INST, 0, 0, 0, 0, MIB_PROF_FTP_SHARE_DIR},
#endif
#if (NET_SHARE_USER == FYES)
	{"netShareUser", netShareUserConfig, INST, 0, 0, 0, 0, MIB_PROF_NET_SHARE_USER},
#endif
#if (ADFILTER == FYES)
	{"adfilter", adfilterConfig, INST, 0, 0, 0, 0, MIB_PROF_ADFILTER},
#endif
#if (MAKE_MONEY == FYES)
        {"makemoney", makeMoneyConfig, INST, 0, 0, 0, 0, MIB_PROF_MAKE_MONEY},
#endif
#if (CLOUD_MANAGE == FYES)
        {"cloudmanage", cloudmanageConfig, INST, 0, 0, 0, 0, MIB_PROF_CLOUD_MANAGE},
#endif
#if (CLOUD_CONFIG == FYES)
        {"cloudconfig", cloudconfigConfig, INST, 0, 0, 0, 0, MIB_PROF_CLOUD_CONFIG},
#endif
#if (UTT_KNOWIFI == FYES)
        {"knowifiDomain", knowifiDomainConfig, INST, 0, 0, 0, 0, MIB_PROF_KNOWIFI_DOMAIN},
#endif
    CONFIGEND
};

