/*** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***
 ***                                                               ***
 ***                  CONFIDENTIAL INFORMATION                     ***
 ***                  ------------------------                     ***
 ***    This Document contains Confidential Information or         ***
 ***    Trade Secrets, or both, which are the property of UTT      ***
 ***    Technologies, Inc. This document may not be  copied,       ***
 ***    reproduced, reduced to any electronic medium or machine    ***
 ***    readable form or otherwise duplicated and the information  ***
 ***    herein may not be used, disseminated or otherwise          ***
 ***    disclosed, except with the prior written consent of UTT    ***
 ***    Technologies, Inc.                                         ***
 ***                                                               ***
 *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***/
#ifndef MIB_NAT_SHARE_H
#define MIB_NAT_SHARE_H

#define UTT_MAX_NATBIND_LEN 12
#define remCtrlName "admin"
#if (STATIC_NAT == FYES)
typedef enum StaticnatProtocol{
    STATIC_NAT_TCP = 1,
    STATIC_NAT_UDP,
    STATIC_NAT_TCP_UDP
}StaticNatProtocol;
typedef struct st_natstatic_profile 
{
    mibHead head;
    StaticNatProtocol protocol;
    UINT32 OutPort;
    IPADDR IP;
    UINT32 InnerPort;
    UINT32 PortNum;
    char NatBind[UTT_MAX_NATBIND_LEN];
}StaticNatProfile;
#endif

#if (NAT_RULE == FYES)
#define NAT_RULE_PASS_THROUGH "nat_rule_pass_through"
typedef enum natruleType{
    UNKOWN = 0,
    EASYIP,
    ONE2ONE,
    PASSTHROUGH
}NatRuleType;

typedef struct st_natrule_profile 
{
    mibHead head;
    NatRuleType NatruleType;
    UINT32 EthId;
    IPADDR OutIp;
    IPADDR InFromIP;
    IPADDR InEndIP;
    char NatBind[UTT_MAX_NATBIND_LEN];
}NatRuleProfile;
#endif

#if (CLOUD_MANAGE == FYES)
typedef struct st_clmanage_profile
{
    mibHead head;
    int  configNeedSyn;/*是否需要配置同步*/
    unsigned int configVer;/*配置版本号*/
}CloudManageProfile;
#endif

#if (DMZ == FYES)
typedef struct st_dmz_profile 
{
    mibHead head;
    IPADDR DMZIP;
}DMZProfile;
#endif
#endif
