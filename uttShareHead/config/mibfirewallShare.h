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

#ifndef MIB_FIREWALL_SHARE_H
#define MIB_FIREWALL_SHARE_H
#if (FIREWALL == FYES)
enum en_FireWallFilterType{
    TYPE_FILTER_IP = 1,
    TYPE_FILTER_URL,
    TYPE_FILTER_WORD,
    TYPE_FILTER_DNS,
#if 0
    TYPE_FILTER_MAC
#endif
};

enum en_FilterIpProtocol{
    Filter_ICMP = 1,
    Filter_TCP,
    Filter_UDP,
    Filter_AH,
    Filter_ALL
};

struct st_filter_ip_profile{
    enum en_FilterIpProtocol Protocol;
    Uint32 dstFromPort;
    Uint32 dstEndPort;
    IPADDR dstFromIp;
    IPADDR dstEndIp;
    Uint32 srcFromPort;
    Uint32 srcEndPort;
};

struct st_filter_WORD_profile{
    Uchar FilterKey[MAX_FILTER_LEN];
};

typedef struct st_firewall_profile 
{
    mibHead head;
    addrObjTypeEn addrTypeEn;/*地址对象类型*/
    IPADDR srcFromIp;
    IPADDR srcEndIp;
#if (IP_GRP == FYES)
    char ipGrpName[UTT_INST_NAME_LEN + 1u];/*地址组名称*/
#endif
#if 0
    UINT32 PRI;
#endif

    int Status;
    enum en_FireWallFilterType FilterType;

    fwObjTypeEn fwTypeEn;/*服务组对象类型*/
#if (PORT_GRP == FYES)
    char fwGrpName[UTT_INST_NAME_LEN + 1u];/*服务组名称*/
#endif

    addrObjTypeEn destAddrTypeEn;/*地址对象类型*/
#if (IP_GRP == FYES)
    char destIpGrpName[UTT_INST_NAME_LEN + 1u];/*地址组名称*/
#endif
    struct st_filter_ip_profile IpFilter;
    struct st_filter_WORD_profile WordFilter;
    UINT32 fw_day;
    Uchar fw_timestart[UTT_TIME_LEN];
    Uchar fw_timestop[UTT_TIME_LEN];
}FireWallProfile;
#endif

#endif
