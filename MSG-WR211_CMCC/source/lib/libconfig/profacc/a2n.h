#ifndef __A2N_H__
#define __A2N_H__

#include "conver.h"
#include "uttMachine.h"
/*
 * this file for enum to int
 */

extern const LcdParamEnumValue yesnoEnum[];
extern const LcdParamEnumValue addrObjTypeEnum[];
extern const LcdParamEnumValue dnsFilterTypeEnum[];
extern const LcdParamEnumValue pppAuthEnum[];
extern const LcdParamEnumValue vpnPeerTypeEnum[];
extern const LcdParamEnumValue portModeEnum[];
extern const LcdParamEnumValue enDisableEnum[];
extern const LcdParamEnumValue ethConnModeEnum[];
extern const LcdParamEnumValue pppConnModeEnum[];
extern const LcdParamEnumValue pppSpDailEnum[];
extern const LcdParamEnumValue ifBalanceModeEnum[];
extern const LcdParamEnumValue lineGroupModeEnum[];
extern const LcdParamEnumValue ispMode3G[];
extern const LcdParamEnumValue authType3G[];
#if (DHCP_AUTO_BIND == FYES)
extern const LcdParamEnumValue endhcpBindType[];
#endif
#if (IP_GRP == FYES)
extern const LcdParamEnumValue ipGrpTypeEnum[];
extern const LcdParamEnumValue ipGrpEntryTypeEnum[];
#endif
#if (PORT_GRP == FYES)
extern const LcdParamEnumValue portGrpTypeEnum[];
extern const LcdParamEnumValue portGrpEntryTypeEnum[];
#endif
#if (STATIC_NAT == FYES)
extern LcdParamEnumValue nat_protocol[];
#endif
#if (FEATURE_ACL == FYES)
extern LcdParamEnumValue acl_actions[];
#endif

#if (FIREWALL == FYES)
extern const LcdParamEnumValue FilterTypeEnum[];
extern const LcdParamEnumValue FilterIpProtocolEnum[];
extern const LcdParamEnumValue fwObjTypeEnum[];
#endif

#if (GROUP_BASE == FYES)
extern const LcdParamEnumValue limitPolicyEnum[];
extern const LcdParamEnumValue groupBaseTypeEnum[];
#endif

#if (P2P_LIMIT == FYES)
extern const LcdParamEnumValue p2pLimitEnum[];
#endif

#if (EASY_TASK == FYES)
extern const LcdParamEnumValue easyTaskEnum[];
#endif

#if (WIRELESS == FYES)
extern const LcdParamEnumValue apWorkModeEnum[];
extern const LcdParamEnumValue wirelessModeEnum[];
extern const LcdParamEnumValue apMacFilterActionEnum[];
#endif

#if (MAC_FILTER == FYES)
extern const LcdParamEnumValue MacFilterActionEnum[];
#endif

#if (NOTICE == FYES)
extern const LcdParamEnumValue webSubmitEnum[];
#endif

#if (PPPOE_SERVER == FYES)
extern const LcdParamEnumValue chargeTypeEnum[];
#endif

#if(IP_SEC == FYES)
extern const LcdParamEnumValue encryptEnum[];
extern const LcdParamEnumValue authWayEnum[];
extern const LcdParamEnumValue groupTypeEnum[];
extern const LcdParamEnumValue UserTypeEnum[];
extern const LcdParamEnumValue negModeEnum[];
extern const LcdParamEnumValue ipsecConnTypeEnum[];
#endif

#if (POLICY_ROUTE == FYES)
extern const  LcdParamEnumValue pr_ipGrpTypeEnum[];
#endif

#if ((SERVICE_ZONE == FYES) ||(FIT_AP_SERVICE_ZONE == FYES))
extern const LcdParamEnumValue authMethodEnum[];
extern const LcdParamEnumValue keyTypeEnum[];
extern const LcdParamEnumValue kFmtEnum[];
extern const LcdParamEnumValue wVersionEnum[];
extern const LcdParamEnumValue sAlgEnum[];
extern const LcdParamEnumValue uthModeEnum[];
#endif

extern const LcdParamEnumValue wlanRateEnum[];
extern const LcdParamEnumValue wlanManualPowerEnum[];
#if (NET_SHARE_USER == FYES)
extern const LcdParamEnumValue netShareJurisdictionEnum[];
#endif
#endif
