
#ifndef __MIB_H
#define __MIB_H

#include <linux/autoconf.h>
#include "typedef.h"

#include "uttMachine.h"

#include "mibunion.h"
#include "mibtype.h"

#include "mibSystem.h"
#include "mibInterface.h"
#include "mibUser.h"

#include "uttSem.h"
#include "uttNfNetlink.h"
#include "ipsetApi.h"

#if (IPV6_INTERFACE == FYES)
#include "mibIpv6Interface.h"
#endif

#include "mibInterface.h"

#if (IPV6_RADVD == FYES)
#include "mibRadvd.h"
#endif

#if(IPV6_NOTLISTPREFIXS == FYES)
#include "mibPrefixList.h"
#endif

#if(IPV6_ROUT == FYES)
#include "mibIpv6RoutConfig.h"
#endif

#if (IPV6_DHCPV6 == FYES)
#include "mibDhcp6.h"
#endif

#if (IPV6_TUNNEL == FYES)
#include "mibTunnel.h"
#endif

#if (IPV6_ISATAP == FYES)
#include "mibIsatap.h"
#endif
#if (IP_GRP == FYES)	
#include "mibIpGroup.h"
#endif

#if (TIME_GRP == FYES)	
#include "mibTimeGrp.h"
#endif

#if (DHCP_SERVER == FYES)
#include "mibDhcpServer.h"
#endif

#if (DHCP_POOL == FYES)
#include "mibDhcpPool.h"
#endif
#if (VLAN_CONFIG == FYES)
#include "mibVlanConfig.h"
#endif
#if (DNS_AGENCY == FYES)
#include "mibDnsAgency.h"
#endif

#if (L2TP_SERVER == FYES)
#include "mibL2TPServer.h"
#endif

#if (L2TP_CLIENT == FYES)
#include "mibL2TPClient.h"                                                                                                            
#endif                                                                                                                                

#if (PPTP_SERVER == FYES)
#include "mibPPTPServer.h"
#endif


#if (AP_PERMISSION == FYES)
#include "mibApPermission.h"
#endif

#if (AP_CONF == FYES)
#include "mibApConf.h"
#endif

#if (SERVICE_ZONE == FYES)
#include "mibServiceZone.h"
#endif

#if((FEATURE_AP_MAC_FILTER == FYES)||(FIT_AP_MAC_FILTER == FYES))
#include "mibApMacFilter.h"
#endif

#if (TAG_VLAN == FYES)
#include "mibTagVlan.h"
#endif

#if (PPTP_CLIENT == FYES)
#include "mibPptpClient.h"
#endif

#if (STATIC_ROUTE == FYES)
#include "mibRoute.h"
#endif

#if (DDNS == FYES)
#include "mibDdns.h"
#endif

#if (PPPOE_SERVER == FYES)
#include "mibPppoeServer.h"
#endif

#if (FEATURE_SW_PORT_CONF == FYES)
#include "mibSwPortConf.h"
#endif

#if (FEATURE_PORT_VLAN == FYES)
#include "mibPortVlan.h"
#endif
#if (PVLAN_REDUCED == FYES)
#include "mibRddPortVlan.h"
#endif
#if (GROUP_BASE == FYES)
#include "mibGroupBase.h" 
#endif

#if (FEATURE_SW_PORT_MIRROR == FYES)
#include "mibMirror.h"
#endif
#if (FEATURE_SW_PORT_TRUNK == FYES)
#include "mibTrunk.h"
#endif

#if (FEATURE_SW_SNMP == FYES)
#include "mibSnmp.h"
#endif

#if (FEATURE_ACL == FYES)
#include "mibACL.h"
#endif

#if (UTTPDB == FYES)
#include "mibpdb.h"
#endif

#if (STATIC_NAT == FYES)
#include "mibnat.h" 
#endif

#if (ADMIN_CONF == FYES)
#include "mibAdminConf.h"
#endif

#if (DNS_FILTER == FYES)
#include "mibDnsFilter.h"
#endif

#if (CLI_FOR_ENGINEER_ONLY == FYES)
#include "mibCliForEngineerOnly.h"
#endif

#if (FAT_FIT_SWITCH == FYES)
#include "mibFatFitSwitch.h"
#endif

#if (AC_WORK_MODE == FYES)
#include "mibAcMode.h"
#endif

#if (AP_LOAD_BALANCE == FYES)
#include "mibApLoadBalance.h"
#endif

#if (WIRELESS == FYES)
#include "mibWireless.h"
#endif

#if (FEATURE_NTP == FYES)
#include "mibNtp.h"
#endif
#if (FEATURE_LANG == FYES)
#include "mibLang.h"
#endif

#if (TASK_SCHEDULER == FYES)
#include "mibTaskSched.h"
#endif

#if (FIREWALL == FYES)
#include "mibfirewall.h"
#endif

#if (NOTICE == FYES)
#include "mibNotice.h"
#endif

#if (WEB_AUTH == FYES)
#include "mibWebAuth.h"
#endif

#if (EXCEPT_QQ == FYES)
#include "mibExceptQQ.h"
#endif

#if (EXCEPT_MSN == FYES)
#include "mibExceptMSN.h"
#endif

#if (EXCEPT_ALI == FYES)
#include "mibExceptAli.h"
#endif

#if (LOG_MANAGE == FYES)
#include "mibLogManage.h"
#endif

#if (AP_LOG_MANAGE == FYES)
#include "mibApLogManage.h"
#endif

#if (IP_SEC == FYES)
#include "mibipsec.h"
#endif
#if (SYSLOG == FYES)
#include "mibSyslog.h"
#endif
#if (UTT_LICENSE == FYES)
#include "miblicense.h"
#endif
#if (POLICY_ROUTE == FYES)
#include "mibPolicyRoute.h"
#endif
#if (PARENTS_CONTROL == FYES)
#include "mibParentsControl.h"
#endif

#if (FEATURE_AP_MAIL == FYES)
#include "mibApMail.h"
#endif

#if (FEATURE_AP_CONF_TEMP == FYES)
#include "mibApConfTemp.h"
#endif

#if (FEATURE_AP_SCAN == FYES)
#include "mibApScan.h"
#endif

#if (FEATURE_AP_SW_MNG == FYES)
#include "mibApSwMng.h"
#endif

#if (FEATURE_AP_BASIC_INFO == FYES)
#include "mibApBasicConf.h"
#endif

#if (FEATURE_AP_EFFECT_CONF == FYES)
#include "mibApEffectConf.h"
#endif

#if (FIT_AP_SERVICE_ZONE == FYES)
#include "mibFitApSZ.h"
#endif

#if (AP_BLACK_WHITE_LIST == FYES)
#include "mibApBlackWhiteList.h"
#endif

#if (HTTP_CACHE == FYES)
#include "mibCache.h"
#endif

#if (NET_MODE_SWITCH  == FYES)
#include "mibNetModeSwitch.h"
#endif

#if (BRIDGE == FYES)
#include "mibBridge.h"
#endif

#include "mibDebug.h"

#if (P2P_LIMIT == FYES)
#include "mibP2PLimit.h"
#endif

#if (SYS_LOG_INFO == FYES)
#include "mibSysLogInfo.h"
#endif

#if (EASY_TASK == FYES)
#include "mibEasyTask.h"
#endif

#if (CACHE_SERVER == FYES)
#include "mibCacheServer.h"
#endif

#if (MAC_FILTER == FYES)
#include "mibMacFilter.h"
#endif

#if (VPN_CONTROL == FYES)
#include "mibVpnControl.h"
#endif

#if (NET_SNIPER == FYES)
#include "mibNetSniper.h"
#endif

#if (UTT_SMART_UI == FYES)
#if (APP_MENU == FYES)
#include "mibAppMenu.h"
#endif
#endif

#if (NET_SHARE_MANAGE == FYES)
#include "mibNetShareManage.h"
#endif

#if (FTP_SERVER == FYES)
#include "mibFtpServer.h"
#endif

#if (NET_SHARE_USER == FYES)
#include "mibNetShareUser.h"
#endif

#if (ADFILTER == FYES)
#include "mibAdfilter.h"
#endif

#if(MAKE_MONEY == FYES)
#include "mibMakeMoney.h"
#endif

#if (UTT_KNOWIFI == FYES)
#include "mibKnowifiDomain.h"
#endif

#include "mibShare.h"

#endif
