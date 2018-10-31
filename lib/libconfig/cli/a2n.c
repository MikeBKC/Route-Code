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
 
/* 
    Copyright 1991-2002 UTT Technologies, Inc.
    All rights reserved.
    Use of copyright notice does not imply publication.
 */

#include "typedef.h"
#include "argcmd.h"
#include "lcdmisc.h"

#if RADIUS_COND
# if ( (RADIUS == FYES) || (TACACS == FYES) || (SECURID == FYES) || ( DEFENDER == FYES ) )

const LcdParamEnumValue       radiusRefreshTypeEnum[] = {
     {"NONE", RAD_REFRESH_NONE},
     {"ROUTES", RAD_REFRESH_ROUTES},
     {"POOLS", RAD_REFRESH_POOLS},
     {"DIALOUT", RAD_REFRESH_NAILED},
 # if ( ASYNC == FYES )
     {"TERMSRV", RAD_REFRESH_TERMSRV},
 # endif
 # if ( SOURCE_AUTH == FYES )
     {"SOURCE", RAD_REFRESH_SOURCE},
 # endif
     {"BRIDGE", RAD_REFRESH_BRIDGE},
     {"ANSNUM", RAD_REFRESH_ANS},
     {"ALL", RAD_REFRESH_ALL},
    { 0,                0 }
};

#if 0
    /*
     * The authentication types supported.
     */
const LcdParamEnumValue       authTypeEnum[] = {
    { "None",           AUTH_NONE },
#  if ( TACACS == FYES )
    { "TACACS",         AUTH_TACACS },
#  endif /* TACACS */
#  if ( RADIUS == FYES )
    { "RADIUS",         AUTH_RADIUS },
    { "UTT",	        AUTH_RADIUS_LOGOUT },
#  endif /* RADIUS */
#  if ( SECURID == FYES )
    { "SECURID",        AUTH_SECURID },
#  endif /* SECURID */
#  if ( TACACSPLUS == FYES )
    { "TACACS+",        AUTH_TACACS_PLUS },
#  endif /* TACACSPLUS */
#  if ( DEFENDER == FYES )
    { "DEFENDER",       AUTH_DEFENDER },
#  endif /* DEFENDER */
    { 0,                0 }
};

const LcdParamEnumValue       lpfTypeEnum[] = {
    { "LocalFirst",             AUTH_LPF_YES },
    { "LocalDisabled",          AUTH_LPF_NO },
    { "RemoteFallback",         AUTH_LPF_RNO },
    { 0,                0 }
};

# endif /*( (RADIUS == FYES) || (TACACS == FYES) || (SECURID == FYES) ) */
#endif /* RADIUS_COND */

# if ( RADIUS_SERV == FYES )
const LcdParamEnumValue       radServAttrParams[] = {
    { "Any",            RADIUS_SERV_ATTRIBUTE_ANY },
    { "Session",        RADIUS_SERV_ATTRIBUTE_KEY },
    { "All",            RADIUS_SERV_ATTRIBUTE_ALL },
    { NULL,             0 }
};

#endif /* ( RADIUS_SERV == FYES ) */

#  if (RADIUS == FYES) || ( RADIUS_ACCT == FYES || TACACSPLUS_ACCT == FYES)
    /*
     * The accounting types supported.
     */
const LcdParamEnumValue       acctTypeEnum[] = {
    { "None",           ACCT_NONE },
    { "RADIUS",         ACCT_RADIUS },
#   if (TACACSPLUS_ACCT == FYES)
    { "TACACS+",        ACCT_TACACSPLUS },
#   endif /* TACACSPLUS_ACCT */
    { 0,                0 }
};

    /*
     * The Base to be used for reporting Account ID.
     */
const LcdParamEnumValue       acctIDBaseEnum[] = {
    { "10Base",             ACCT_BASE_10 },
    { "16Base",             ACCT_BASE_16 },
    { 0,                0 }
};

    /*
     * RADIUS compatibility options
     */
const LcdParamEnumValue radCompatEnum[] = {
    { "Livinston",      RAD_COMPAT_OLD },
    { "RFC_VSA",        RAD_COMPAT_VSA },
    { NULL,             0 }
};

    /*
     * RADIUS Keep User Name options
     */
const LcdParamEnumValue radKeepUserNameEnum[] = {
    { "ChgUsrName",     RAD_CHANGE_NAME },
    { "KeepUsrName",    RAD_KEEP_NAME },
    { "KeepRealm",      RAD_KEEP_REALM_NAME },
    { NULL,             0 }
};

# endif /* ( RADIUS_ACCT == FYES || TACACSPLUS_ACCT == FYES) */

const LcdParamEnumValue        languageEnum[] = {
    { "Chinese",	LANGUAGE_CHN},
    { "English",	LANGUAGE_ENG},
    { "Spanish",	LANGUAGE_SPAN},
    { NULL,             0 }
};

const LcdParamEnumValue        sanityCheckEnum[] = {
    { "disabled", SANITY_CHECK_NONE},
    { "image", SANITY_CHECK_IMG},
    { "flash", SANITY_CHECK_FLASH},
    { "nvram", SANITY_CHECK_NVRAM},
    { "data",  SANITY_CHECK_DATA},
    { "task",  SANITY_CHECK_TASK},
    { "full",  SANITY_CHECK_IMG|SANITY_CHECK_FLASH|SANITY_CHECK_NVRAM},
    { "backupImg",  SANITY_CHECK_IMG|SANITY_BACKUP_IMG},
    { "webrom",  SANITY_CHECK_IMG|SANITY_BACKUP_IMG|SANITY_BACKUP_WEB},
    { NULL,             0 }
};

const LcdParamEnumValue        nailedEnum[] = {
    { "Switched",       NAILED_MODE_OFF },
#  if ( NAILED_CHANS == FYES )
    { "Nailed",         NAILED_MODE_FT1 },
    { "Nailed/Mpp",     NAILED_MODE_FT1_MPP },
#   if ( X25B == FYES ) && ( M_M_FRAMING == FYES )
    { "AO/DI",          NAILED_MODE_AODI },
#   endif /* X25B */
#  endif
#  if ( PERM_SWITCHED == FYES )
    { "AO/Switched",    PERM_SW_MODE },
#  endif /* PERM_SWITCHED */
#  if 0
    { "B&O",            NAILED_MODE_FT1_BO },
#  endif
    { NULL,             0 }
};


const LcdParamEnumValue        compressionEnum[] = {
    { "None",           COMP_NONE },
    { "Stac",           COMP_STAC },
    { "Stac-9",         COMP_STAC9 },
    { "MS-Stac",        COMP_MS_STAC },
    { "MS-PPC",         COMP_MS_PPC },
    { "MS-PPE",         COMP_MS_PPE },
    { "MS-PPE40",       COMP_MS_PPE40 },
    { "MS-PPE56",       COMP_MS_PPE56 },
    { "MS-PPE128",      COMP_MS_PPE128 },
    { NULL,             0 }
};

const LcdParamEnumValue        mgmtTypeEnumValues[] = {
/* mod:ast@20010921 from 'static const' to '' for ref */
# if !defined( NO_CHANNELIZED_T1 ) /* can't do MP type calls on 1 channel */
# if ( MPP == FYES ) /* can't do MP type calls on 1 channel */
    { "MPP",            CALL_MGMT_MPP },
    { "MP",             CALL_MGMT_MP },
# endif
# endif
    { "PPP",            CALL_MGMT_PPP },
# if ( COMB == FYES )
    { "COMB",           CALL_MGMT_COMBINET },
# endif /* COMB */
# if 0
    { "SLIP",           CALL_MGMT_SLIP },
    { "CSLIP",          CALL_MGMT_CSLIP },
# endif /* 0 */
# if ( FRAME_RELAY == FYES )
    { FR_OPTION,        CALL_MGMT_FRAME_RELAY },
# endif /* FRAME_RELAY */
# if ( FRAME_RELAY_NNI == FYES )
    { "FR_CIR",         CALL_MGMT_FRAME_RELAY_CIR },
# endif /* FRAME_RELAY_NNI */
# if ( X25PAD == FYES )
    { "X25/PAD",        CALL_MGMT_X25PAD },
# endif /* X25PAD */
# if ( X25_T3POS == FYES )
    { "X25/T3POS",      CALL_MGMT_T3POS },
# endif /* X25_T3POS */
# if ( IPOX25 == FYES)
    { "IPOX",           CALL_MGMT_1356_X25 },
# endif /* IPOX25 */
# if ( X25B == FYES )
    { "X.32",           CALL_MGMT_X32 },
# endif /* X25B */
# if ( EURAW == FYES )
    { "EU-RAW",         CALL_MGMT_EURAW },
# endif /* EURAW */
# if ( EUUI == FYES )
    { "EU-UI",          CALL_MGMT_EUUI },
# endif /* EUUI */
# if ( ASYNC == FYES )
    { "TCP-CLEAR",      CALL_MGMT_TCP_RAW },
# endif /* ASYNC */
# if ( ARAP == FYES)
    { "ARA",            CALL_MGMT_ARA },
# endif
# if ( DTPT == FYES )
    { "DTPT",           CALL_MGMT_DTPT },
# endif
# if ( ATM == FYES )
    { "ATM",            CALL_MGMT_ATM },
    { "ATM-FR_CIR",     CALL_MGMT_ATM_FR_CIR },
# endif /* ( ATM == FYES ) */
    { NULL,             0 }
};

const LcdParamEnumValue       pppLqmEnum[] = {
    { "No", 	PPP_LQM_DISABLED},
    { "Yes",	PPP_LQM_ENABLED},
    { "Echo",	PPP_LQM_ECHO},
    { NULL,	0 }
};

const LcdParamEnumValue       pppAnsAuthEnum[] = {
    { "None",                   PPP_AUTH_NONE },
    { "PAP",                    PPP_AUTH_PAP },
    { "CHAP",                   PPP_AUTH_CHAP },
    { "Either",                 PPP_AUTH_ANY },
    { "MS-CHAP",                PPP_AUTH_MS_CHAP },
    { "MS-CHAPV2",              PPP_AUTH_MS_CHAPV2 },
    { NULL,                     0 }
};

#if (NETKEEPER == FYES)
const LcdParamEnumValue       nkDialModeEnum[] = {
    {"normal",            NK_DIAL_MODE_NORMAL},
    {"special1",          NK_DIAL_MODE_SPECIAL_1},
    {"special2",          NK_DIAL_MODE_SPECIAL_2},
    {"special3",          NK_DIAL_MODE_SPECIAL_3},
    { NULL,             0 }
};
#endif

const LcdParamEnumValue       pppSendAuthEnum[] = {
    { "None",           PPP_AUTH_NONE },
    { "PAP",            PPP_AUTH_PAP },
    { "CHAP",           PPP_AUTH_CHAP },
#if 0
# if ( LCP_CHALLENGE_CLIENT == FYES )
#  if (0)
    { "PAP-DES",        PPP_AUTH_PAP_DES },
#  endif
    { "PAP-TOKEN",      PPP_AUTH_PAP_TOKEN },
    { "PAP-TOKEN-CHAP", PPP_AUTH_PAP_TOKEN_CHAP },
# endif
    { "CACHE-TOKEN",    PPP_AUTH_CHAP_CACHE_TOKEN },
#endif
    /*
     * commented by llliu@080418.
     */
    //{ "MS-CHAP",        PPP_AUTH_MS_CHAP },
    { "Either",         PPP_AUTH_ANY },
    //{ "MS-CHAPV2",      PPP_AUTH_MS_CHAPV2 },
    { NULL,             0 }
};

const LcdParamEnumValue       routingModeEnum[] = {
    { "Off",               ROUTING_OFF },
    { "send-ripv1",        ROUTING_SEND_ONLY },
    { "recv-ripv1",        ROUTING_RECV_ONLY },
    { "ripv1",    	   ROUTING_SEND_AND_RECV },
    { "send-ripv2",        ROUTING_SEND_ONLY_V2 },
    { "recv-ripv2",        ROUTING_RECV_ONLY_V2 },
    { "ripv2",   	   ROUTING_SEND_AND_RECV_V2 },
    { NULL,             0 }
};

    /*
     * Telco sub-menu params
     */

const LcdParamEnumValue	callModeEnum[] = {

    { "Dialin",		CALL_ANS_ONLY },
    { "Dialout",	CALL_ORIG_ONLY },
    { "Both",		CALL_ANS_AND_ORIG },
    { NULL,		0 }
};

const LcdParamEnumValue	natDipFollowEnum[] = {
    { "disable",	nofollow },
    { "tcp",		tcpfollow },
    { "udp",		udpfollow },
    { "all",		bothfollow },
    { NULL,		0 }
};

const LcdParamEnumValue	protoInfo[] = {
    { "NONE",		SM_NONE_PROT },
    { "TCP",		SM_TCP_PROT },
    { "UDP",		SM_UDP_PROT },
    { "GRE",		SM_GRE_PROT },
    { "ESP",		SM_ESP_PROT },
    { "AH",		SM_AH_PROT },
    { "ICMP",		SM_ICMP_PROT },
    { "IGMP",		SM_IGMP_PROT },
    { "IPINIP",		SM_IPINIP_PROT },
    { "OSPF",		SM_OSPF_PROT },
    { NULL,		0 }
};

const LcdParamEnumValue	eqNeqEnum[] = {
    { "Equals",		FALSE },
    { "NotEquals",	TRUE },
    { NULL,		0 }
};

const LcdParamEnumValue	filterTypeEnum[] = {
    { "GENERIC",	FILTER_GENERIC },
    { "IP",		FILTER_IP },
    {"IPSSG",		FILTER_IPSSG},
# if ( IPX_ROUTE == FYES )
    { "IPX",		FILTER_IPX },
# endif
# if ( IPTOS == FYES )
    { "TOS",		FILTER_TOS },
# endif
    { NULL,		0 }
};

const LcdParamEnumValue	testEnum[] = {
    { "None",		COMPARE_NONE },
    { "Less",		COMPARE_LESS },
    { "Eql",		COMPARE_EQUAL },
    { "Gtr",		COMPARE_GREATER },
    { "Neq",		COMPARE_NOT_EQUAL },
    { NULL,		0 }
};

/* Some of these could probably eliminated,
 * How good is your time geography?
 */
const LcdParamEnumValue OffGMTEnum[] = {
     { "UTC+0000", 0 },
     { "UTC+0030", 1800  },
     { "UTC+0100", 3600  },
     { "UTC+0130", 5400  },
     { "UTC+0200", 7200  },
     { "UTC+0230", 9000  },
     { "UTC+0300", 10800 },
     { "UTC+0330", 12600 },
     { "UTC+0400", 14400 },
     { "UTC+0430", 16200 },
     { "UTC+0500", 18000 },
     { "UTC+0530", 19800 },
     { "UTC+0600", 21600 },
     { "UTC+0630", 23400 },
     { "UTC+0700", 25200 },
     { "UTC+0730", 27000 },
     { "UTC+0800", 28800 },
     { "UTC+0830", 30600 },
     { "UTC+0900", 32400 },
     { "UTC+0930", 34200 },
     { "UTC+1000", 36000 },
     { "UTC+1030", 37800 },
     { "UTC+1100", 39600 },
     { "UTC+1130", 41400 },
     { "UTC+1200", 43200 },
 /* Of course timezones go the other way as well. */
     { "UTC-0030",-1800  },
     { "UTC-0100",-3600  },
     { "UTC-0130",-5400  },
     { "UTC-0200",-7200  },
     { "UTC-0230",-9000  },
     { "UTC-0300",-10800 },
     { "UTC-0330",-12600 },
     { "UTC-0400",-14400 },
     { "UTC-0430",-16200 },
     { "UTC-0500",-18000 },
     { "UTC-0530",-19800 },
     { "UTC-0600",-21600 },
     { "UTC-0630",-23400 },
     { "UTC-0700",-25200 },
     { "UTC-0730",-27000 },
     { "UTC-0800",-28800 },
     { "UTC-0830",-30600 },
     { "UTC-0900",-32400 },
     { "UTC-0930",-34200 },
     { "UTC-1000",-36000 },
     { "UTC-1030",-37800 },
     { "UTC-1100",-39600 },
     { "UTC-1130",-41400 },
     { 0,               0 }
};

const LcdParamEnumValue sntpEnabledEnum[] = {
     { "No", SNTP_DISABLED }
    ,{ "Yes", SNTP_ENABLED }
    ,{ NULL,             0 }
};


    /*
     * The range of valid syslog facility numbers.  16 (the offset of
     * LOG_LOCAL0) is added to this value when messages are sent to the
     * syslogd.
     */
const LcdParamEnumValue	facilityEnum[] = {
    { "Local0",		0 },
    { "Local1",		1 },
    { "Local2",		2 },
    { "Local3",		3 },
    { "Local4",		4 },
    { "Local5",		5 },
    { "Local6",		6 },
    { "Local7",		7 },
    { 0,		0 }
};

    /*
     * The range of valid syslog logCallInfo levels.
     */
const LcdParamEnumValue	logCallInfoEnum[] = {
    { "System-Only",	0 },
    { "Call-End",       1 },
    { "NAT-OUT",        2 },
    { "NAT-IN",         3 },
    { "NAT-BOTH",       4 },
    { 0,		0 }
};

    /*
      * The values to use for ripPolicy.
      */
const LcdParamEnumValue	ripPolicyEnum[] = {
     { "SplitHorizon",	RIP_POLICY_SPLIT_HORIZON },
     { "PoisonReverse",	RIP_POLICY_POISON_REVERSE },
     { 0,		0 }
};

const LcdParamEnumValue	dialoutPoisonEnum[] = {
	{ "Always",	    FALSE },
	{ "InterfacesUp",   TRUE  },
	{ NULL,		0 },
};

const LcdParamEnumValue 	ripEnum[] = {
    { "Off",		ROUTING_OFF           },
    { "RIPv1_Listen",	ROUTING_RECV_ONLY     },
#   if PLATFORM & ( MPORT | P50 | PDSL | P25 | M200 )
    { "RIPv1_Advertise",ROUTING_SEND_ONLY },
    { "RIPv1",		ROUTING_SEND_AND_RECV },
    { "RIPv2_Advertise",ROUTING_SEND_ONLY_V2 },
    { "RIPv2_Listen",	ROUTING_RECV_ONLY_V2 },
    { "RIPv2",		ROUTING_SEND_AND_RECV_V2 },
#   else
#    error Unknown PLATFORM
#   endif
    { NULL,	0		      }
};

const LcdParamEnumValue 	proxyEnum[] = {
    { "Disabled",	PROXY_OFF           },
    { "IfUp",		PROXY_CALL_ACTIVE   },
    { "IfDown",		PROXY_CALL_INACTIVE },
    { "Enabled",	PROXY_ALWAYS        },
    { "Nat",		PROXY_NAT           },
    { NULL,		0	            }
};

#  if ( L2TP == FYES )
     /* The L2TP LAC/LNS Mode Enum
     */
const LcdParamEnumValue 	l2tpModeEnum[] = {
    { "None",	L2TP_DISABLED   },
    { "LAC",	L2TP_LAC_MODE  },
    { "LNS",    L2TP_LNS_MODE  },
    { "Both",	L2TP_BOTH_MODE },
    { NULL,	0	       }
};

const LcdParamEnumValue 	l2tpTransportModeEnum[] = {
    { "NORMAL", 	L2TP_TRANS_NORMAL},
    { "FIXED", 		L2TP_TRANS_PRESERVE},
    { "DUP", 		L2TP_TRANS_DUP},
    { "PROBE", 		L2TP_TRANS_DISCOVER},
    {NULL,	0}
};

#  endif /* L2TP == FYES */

    /*
     * The Tunnel Types assigned to each line.
     */
const LcdParamEnumValue 	l2TunnelTypeEnum[] = {
    { "None",	L2_TUNNEL_NONE  },
#  if ( PPTP == FYES )
    { "PPTP",	L2_TUNNEL_PPTP  },
#  endif /* ( PPTP == FYES ) */
#  if ( L2TP == FYES )
    { "L2TP",   L2_TUNNEL_L2TP  },
    { "ANY",   L2_TUNNEL_ANY  },
#  endif /* L2TP == FYES */
    { NULL,	0		}
};

#if (FC == FYES)

#else
const LcdParamEnumValue ipMacRateEnum[] = {
    {"Disabled", IPMAC_RATE_NONE},
# if ( IP_RATE == FYES )
    {"IpAddr", 	 IPMAC_RATE_IPADDR},
# else
    {"MacAddr",  IPMAC_RATE_MACADDR},
# endif
    {"Statistics",  IPMAC_RATE_ACCT},
    {NULL, 0}};

# if ( TC_DRR == FYES )

const LcdParamEnumValue ipRedDurationEnum[] = {
    {"Disabled", RED_DURATION_DISABLED},
    {"15s", RED_DURATION_15s},
    {"30s", RED_DURATION_30s},
    {"45s", RED_DURATION_45s},
    {"60s", RED_DURATION_60s},
    {"75s", RED_DURATION_75s},
    {"90s", RED_DURATION_90s},
    {"120s", RED_DURATION_120s},
    {"Forever", RED_DURATION_8y},
    {NULL, 0}};

const LcdParamEnumValue ipCreditDrrEnum[] = {
    {"Disabled", DRR_DISABLED},
# if 0
    {"Fair(64Kbit\\1.2Mbyte)", DRR_64Kbit},
    {"Ordinary(128Kbit\\1.2Mbyte)", DRR_128Kbit},
    {"Smooth(256Kbit\\1.2Mbyte)", DRR_256Kbit},
    {"Normal(512Kbit\\1.2Mbyte)", DRR_512Kbit},
    {"Fast(1Mbit\\1.2Mbyte)", DRR_1Mbit},
    {"Turbo(2Mbit\\1.2Mbyte)", DRR_2Mbit},
    {"Burst(4Mbit\\1.6Mbyte)", DRR_4Mbit},
    {"Jet(8Mbit\\2.0Mbyte)", DRR_8Mbit},
    {"Optima(16Mbit\\2.4Mbyte)", DRR_16Mbit},
# else
    {"64Kbit", DRR_64Kbit},
    {"128Kbit", DRR_128Kbit},
    {"192Kbit",192*10},// jschen: allow more speed
    {"256Kbit", DRR_256Kbit},
    {"384Kbit", 384*10},// jschen: allow more speed
    {"512Kbit", DRR_512Kbit},
    {"768Kbit", 768*10},// jschen: allow more speed
    {"1Mbit", DRR_1Mbit},
    {"1.5Mbit", 15*1000},// jschen: allow more speed
    {"2Mbit", DRR_2Mbit},
    {"4Mbit", DRR_4Mbit},
    {"8Mbit", DRR_8Mbit},
    {"16Mbit", DRR_16Mbit},
# endif
    {NULL, 0}};

const LcdParamEnumValue ipCreditOverflowEnum[] = {
    {"AUTO", DRR_QUOTA_0Mb},
    {"8Kbyte", DRR_QUOTA_8Kb},
     {"16Kbyte", DRR_QUOTA_16Kb},
     {"32Kbyte", DRR_QUOTA_32Kb},
     {"64Kbyte", DRR_QUOTA_64Kb},
     {"128Kbyte", DRR_QUOTA_128Kb},
     {"256Kbyte", DRR_QUOTA_256Kb},
     {"512Kbyte", DRR_QUOTA_512Kb},
    {"1Mbyte", DRR_QUOTA_1Mb},
    {"2Mbyte", DRR_QUOTA_2Mb},
    {"3Mbyte", DRR_QUOTA_3Mb},
    {"4Mbyte", DRR_QUOTA_4Mb},
    {"5Mbyte", DRR_QUOTA_5Mb},
    {"6Mbyte", DRR_QUOTA_6Mb},
    {"7Mbyte", DRR_QUOTA_7Mb},
    {"8Mbyte", DRR_QUOTA_8Mb},
    {"9Mbyte", DRR_QUOTA_9Mb},
    {"10Mbyte", DRR_QUOTA_10Mb},
    {"20Mbyte", DRR_QUOTA_20Mb},
    {"30Mbyte", DRR_QUOTA_30Mb},
//    {"40Mbyte", DRR_QUOTA_40Mb},
//    {"50Mbyte", DRR_QUOTA_50Mb},
//    {"60Mbyte", DRR_QUOTA_60Mb},
//    {"70Mbyte", DRR_QUOTA_70Mb},
//    {"80Mbyte", DRR_QUOTA_80Mb},
//    {"90Mbyte", DRR_QUOTA_90Mb},
//    {"100Mbyte", DRR_QUOTA_100Mb},
    {NULL, 0}};

const LcdParamEnumValue ipCreditUnderflowEnum[] = {
    {"NoLimit", DRR_QUOTA_0Mb},
    {"1Mbyte", -DRR_QUOTA_1Mb},
    {"2Mbyte", -DRR_QUOTA_2Mb},
    {"3Mbyte", -DRR_QUOTA_3Mb},
    {"4Mbyte", -DRR_QUOTA_4Mb},
    {"5Mbyte", -DRR_QUOTA_5Mb},
    {"6Mbyte", -DRR_QUOTA_6Mb},
    {"7Mbyte", -DRR_QUOTA_7Mb},
    {"8Mbyte", -DRR_QUOTA_8Mb},
    {"9Mbyte", -DRR_QUOTA_9Mb},
    {"10Mbyte", -DRR_QUOTA_10Mb},
    {"20Mbyte", -DRR_QUOTA_20Mb},
    {"30Mbyte", -DRR_QUOTA_30Mb},
    {"40Mbyte", -DRR_QUOTA_40Mb},
    {"50Mbyte", -DRR_QUOTA_50Mb},
    {"60Mbyte", -DRR_QUOTA_60Mb},
    {"70Mbyte", -DRR_QUOTA_70Mb},
    {"80Mbyte", -DRR_QUOTA_80Mb},
    {"90Mbyte", -DRR_QUOTA_90Mb},
    {"100Mbyte", -DRR_QUOTA_100Mb},
    {NULL, 0}};

# endif /* ( TC_DRR == FYES ) */

#endif


const LcdParamEnumValue pdbGroupEnum[] = {
    {"disabled", NO},
    {"enabled", TRUE},
#if ( FIREWALLS == FYES )
    {"Pdb", TRUE+FWALL_NUMBER_RANGE_START},
#endif
    {NULL, 0}};

const LcdParamEnumValue filterAlgTypeEnum[] = {
    { "ALG_NONE", FILTER_ALG_NONE},
    { "DNS_ALG", FILTER_ALG_DNS},
    { "URL_ALG", FILTER_ALG_URL},
    { "STR_ALG", FILTER_ALG_STR},
# if ( XUANLIN_ALG == FYES )
    { "XUANLIN_ALG", FILTER_ALG_FDXL},
# endif
//  { "CUSTOMER_ALG", FILTER_ALG_CUSTOMER},
//    { "FTP_ALG", FILTER_ALG_FTP},
//    { "MSN_ALG", FILTER_ALG_MSN},
      { "QQ_ALG",  FILTER_ALG_QQ},
//    { "BT_ALG",  FILTER_ALG_BT},
//    { "EMULE_ALG", FILTER_ALG_EMULE},
//    { "HTTP_ALG", FILTER_ALG_HTTP},
//    { "POP3_ALG", FILTER_ALG_POP3},
//    { "SMTP_ALG", FILTER_ALG_SMTP},
//    { "RTSP_ALG", FILTER_ALG_RTSP},
//    { "MMS_ALG", FILTER_ALG_MMS},
//    { "REALP_ALG", FILTER_ALG_REALP},
//    { "HTTPS_ALG", FILTER_ALG_HTTPS},
//    { "L2TP_ALG", FILTER_ALG_L2TP},
//    { "PPTP_ALG", FILTER_ALG_PPTP},
//    { "IPSEC_ALG", FILTER_ALG_IPSEC},
//    { "H323_ALG", FILTER_ALG_H323},
    {NULL, 0}};

const LcdParamEnumValue ddnsTzoneTypeEnum[] = {
    { "IPLINK_DDNS", DDNS_TZ_GUAN},
    { "IETELE_DDNS",  DDNS_TZ_NONE},
    {NULL, 0}};

# if ( ENCRYPT_PASSWD == FYES )
const LcdParamEnumValue passwdTypeEnum[] = {
    { "ClearText",  CLEAR_TEXT},
    { "CryptMode1", CRYPT_BASE64},
    {NULL, 0}};
# endif

const LcdParamEnumValue dataIdleEnum[] = {
    { "Outgoing", MARK_TX},
    { "Incoming", MARK_RX},
    { "Both", MARK_RX_TX},
    {NULL, 0}};

const LcdParamEnumValue arpCacheModeEnum[] = {
    { "ANY", ARP_CACHE_ANY},
    { "DYNAMIC", ARP_CACHE_DYNAMIC},
    { "STATIC", ARP_CACHE_STATIC_ONLY},
    { "SPOOF", ARP_CACHE_SPOOF},
    { "BIND", ARP_CACHE_FORCE_BIND},
    {NULL, 0}};

const LcdParamEnumValue hbeatModeEnum[] = {
    {"ARP_REQ",  HBEAT_MODE_L2_ARP},
//    {"LQM_ECHO",  HBEAT_MODE_L2_LQM},
    {"ICMP_ECHO", HBEAT_MODE_L3_ICMP},
    {"DNS_REQ", HBEAT_MODE_L4_DNS},
//    {"HTTPC", HBEAT_MODE_L4_HTTP},
//    {"SMTPC", HBEAT_MODE_L4_SMTP},
//    {"POP3C", HBEAT_MODE_L4_POP3},
//    {"FTPC", HBEAT_MODE_L4_FTP},
//    {"TELNETC", HBEAT_MODE_L4_TELNET},
    {NULL, 0}};

const LcdParamEnumValue hbeatIcmpEnum[] = {
     {"IP", 	ICMP_IP_SEND},
     {"PORT", 	ICMP_IF_OUTPUT},
    {NULL, 0}};

# if (OLDNAT)
const LcdParamEnumValue natPoolEnum[] = {
    {"Disabled", NAT_NONE},
    {"EasyIP", NAT_EASY},
    {"InOut", NAT_TRANS},
    {"Inside", NAT_SRC},
    {"Outside", NAT_DST},
    {NULL, 0}};
# else
const LcdParamEnumValue natMethodEnum[] = {
    {"EasyIP", NAT_EASY},
    {"One2One", NAT_ONE2ONE},
    {"Passthrough", NAT_PASSTHROUGH},
    {NULL, 0}};
# endif

const LcdParamEnumValue rtPolicyEnum[] = {
 { "Disabled",	NO_POLICY},
 { "RoundRobbinPktByPkt", RANDOM_PKT},
 { "RoundRobbinBySrcIP", RANDOM_SRCIP},
 { "RoundRobbinBySrcPort", RANDOM_SPORT},
 { "RoundRobbinByDstIf", RANDOM_DSTIF},
 { "WeightedPktByPkt", BALANCE_PKT},
 { "WeightedBySrcIP", BALANCE_SRCIP},
 { "WeightedBySport", BALANCE_SPORT},
 { "WeightedByDstIf", BALANCE_DSTIF},
 {NULL, 0}};

const LcdParamEnumValue userTypeEnum[] = {
  {"NONE", NPW_NONE},
  {"PASSWD", NPW_PASSWD},
  {"DIALER", NPW_DIALER},
  {"ARP_STATIC", NPW_ARP_STATIC},
  {"DHCP_STATIC", NPW_DHCP_STATIC},
  //{"DHCP_STATIC", NPW_DHCP_STATIC|NPW_ARP_STATIC},
  {"DHCP_AUTOBIND", NPW_DHCP_AUTOBIND},	    /*added by jfgu@90625*/
  {"NBNS_DB", NPW_NBNS_DB},
  {"IPSSG", NPW_IPSSG},
  {"IPSSG+ARP", NPW_IPSSG|NPW_ARP_STATIC},
  {"IPSSG+DHCP", NPW_IPSSG|NPW_DHCP_STATIC},
  {"IKE_PSK", NPW_IKE_PSK},
  {"IKE_FQDN", NPW_IKE_FQDN},
  {"IKE_USERFQDN", NPW_IKE_USER_FQDN},
  {"IKE_KEYID", NPW_IKE_KEYID},
  {"IKE_IPV4ADDR", NPW_IKE_IPV4ADDR},
  {"TRUST_IPHOST", NPW_IP_HOST},
  {"TRUST_MACHOST", NPW_MAC_HOST},
  //add by jxy12@2004-4-8 for WebUI
#if (WEB_UI == FYES)
  {"GROUP_DHCP",NPW_USER_GROUP|NPW_DHCP_STATIC},
  {"GROUP_IPSSG",NPW_USER_GROUP|NPW_IPSSG},
  {"GROUP_TRUSTHOST",NPW_USER_GROUP|NPW_ARP_STATIC},
#endif
# if ( XUANLIN_ALG == FYES )
  {"XL_URL",NPW_XL_URL},
# endif
//add end jxy12
  {"TYPE_NEW",NPW_NEW_TYPE},	//add by jyshen 060904
#if (PPPOE_SRV == FYES)
  {"PPPOE_STATIC", NPW_PPPOE_STATIC},	/* added by llliu */
#endif
 {NULL, 0}};

#if(DHCP_PLUS == FYES)

const LcdParamEnumValue dhcpCastEnum[] = { 
	{"broadcast",DHCPC_BOARDCAST}, 
	{"unicast",DHCPC_UNICAST}, 
	{NULL, 0}
};

const LcdParamEnumValue RawOptPortEnum[] = {
    {"eth1", EXT_ETHER_IF+1},
    {"eth2", EXT_ETHER_IF2+1},
#if ((SWITCHFUN==FYES) && (TAGVlAN==FYES))  /*add by zgd*/
    {"eth3", EXT_ETHER_IF3+1},
    {"eth4", EXT_ETHER_IF4+1},
    {"eth5", EXT_ETHER_IF5+1},
    {"eth6", EXT_ETHER_IF6+1},
    {"eth7", EXT_ETHER_IF7+1},
    {"eth8", EXT_ETHER_IF8+1},
    {"eth9", EXT_ETHER_IF9+1},
    {"eth10", EXT_ETHER_IF10+1},
    {"eth11", EXT_ETHER_IF11+1},
    {"eth12", EXT_ETHER_IF12+1},
    {"eth13", EXT_ETHER_IF13+1},
    {"eth14", EXT_ETHER_IF14+1},
    {"eth15", EXT_ETHER_IF15+1},
    {"eth16", EXT_ETHER_IF16+1},
    {"eth17", EXT_ETHER_IF17+1},
    {"eth18", EXT_ETHER_IF18+1},
    {"eth19", EXT_ETHER_IF19+1},
    {"eth20", EXT_ETHER_IF20+1},
    {"eth21", EXT_ETHER_IF21+1},
    {"eth22", EXT_ETHER_IF22+1},
    {"eth23", EXT_ETHER_IF23+1},
    {"eth24", EXT_ETHER_IF24+1},
    {"eth25", EXT_ETHER_IF25+1},
    {"eth26", EXT_ETHER_IF26+1},
    {"eth27", EXT_ETHER_IF27+1},
    {"eth28", EXT_ETHER_IF28+1},
    {"eth29", EXT_ETHER_IF29+1},
    {"eth30", EXT_ETHER_IF30+1},
    {"eth31", EXT_ETHER_IF31+1},
    {"eth32", EXT_ETHER_IF32+1},
    {"eth33", EXT_ETHER_IF33+1},
    {"eth34", EXT_ETHER_IF34+1},
    {"eth35", EXT_ETHER_IF35+1},
    {"eth36", EXT_ETHER_IF36+1},
    {"eth37", EXT_ETHER_IF37+1},
    {"eth38", EXT_ETHER_IF38+1},
    {"eth39", EXT_ETHER_IF39+1},
    {"eth40", EXT_ETHER_IF40+1},
    {"eth41", EXT_ETHER_IF41+1},
    {"eth42", EXT_ETHER_IF42+1},
    {"eth43", EXT_ETHER_IF43+1},
    {"eth44", EXT_ETHER_IF44+1},
    {"eth45", EXT_ETHER_IF45+1},
    {"eth46", EXT_ETHER_IF46+1},
    {"eth47", EXT_ETHER_IF47+1},
    {"eth48", EXT_ETHER_IF48+1},
    {"eth49", EXT_ETHER_IF49+1}, 
#else
#if ( VIRTUAL_DIALER == FYES)
#else
# if ( HOST_ETH3 == FYES )
    {"eth3", EXT_ETHER_IF3+1},
# if ( HOST_ETH5 == FYES )
    {"eth4", EXT_ETHER_IF4+1},
    {"eth5", EXT_ETHER_IF5+1},
# if ( HOST_ETH9 == FYES )
    {"eth6", EXT_ETHER_IF6+1},
    {"eth7", EXT_ETHER_IF7+1},
    {"eth8", EXT_ETHER_IF8+1},
    {"eth9", EXT_ETHER_IF9+1},
# if ( HOST_ETH25 == FYES )
    {"eth10", EXT_ETHER_IF10+1},
    {"eth11", EXT_ETHER_IF11+1},
    {"eth12", EXT_ETHER_IF12+1},
    {"eth13", EXT_ETHER_IF13+1},
    {"eth14", EXT_ETHER_IF14+1},
    {"eth15", EXT_ETHER_IF15+1},
    {"eth16", EXT_ETHER_IF16+1},
    {"eth17", EXT_ETHER_IF17+1},
    {"eth18", EXT_ETHER_IF18+1},
    {"eth19", EXT_ETHER_IF19+1},
    {"eth20", EXT_ETHER_IF20+1},
    {"eth21", EXT_ETHER_IF21+1},
    {"eth22", EXT_ETHER_IF22+1},
    {"eth23", EXT_ETHER_IF23+1},
    {"eth24", EXT_ETHER_IF24+1},
    {"eth25", EXT_ETHER_IF25+1},
# if ( HOST_ETH49 == FYES )
    {"eth26", EXT_ETHER_IF26+1},
    {"eth27", EXT_ETHER_IF27+1},
    {"eth28", EXT_ETHER_IF28+1},
    {"eth29", EXT_ETHER_IF29+1},
    {"eth30", EXT_ETHER_IF30+1},
    {"eth31", EXT_ETHER_IF31+1},
    {"eth32", EXT_ETHER_IF32+1},
    {"eth33", EXT_ETHER_IF33+1},
    {"eth34", EXT_ETHER_IF34+1},
    {"eth35", EXT_ETHER_IF35+1},
    {"eth36", EXT_ETHER_IF36+1},
    {"eth37", EXT_ETHER_IF37+1},
    {"eth38", EXT_ETHER_IF38+1},
    {"eth39", EXT_ETHER_IF39+1},
    {"eth40", EXT_ETHER_IF40+1},
    {"eth41", EXT_ETHER_IF41+1},
    {"eth42", EXT_ETHER_IF42+1},
    {"eth43", EXT_ETHER_IF43+1},
    {"eth44", EXT_ETHER_IF44+1},
    {"eth45", EXT_ETHER_IF45+1},
    {"eth46", EXT_ETHER_IF46+1},
    {"eth47", EXT_ETHER_IF47+1},
    {"eth48", EXT_ETHER_IF48+1},
    {"eth49", EXT_ETHER_IF49+1},
# endif /* HOST_ETH49 */
# endif /* HOST_ETH25 */
# endif /* HOST_ETH9 */
# endif /* HOST_ETH5 */
# endif /* HOST_ETH3 */
#endif /* VIRTUAL_DIALER */
#endif/*((SWITCHFUN==FYES) && (TAGVlAN==FYES))*/
    {NULL, 0}
};

const LcdParamEnumValue NBTTypeEnum[] = {
	{"Bnode", Bnode},
	{"Pnode", Pnode},
	{"Mnode", Mnode},
	{"Hnode", Hnode},
	{NULL, 0}
};

const LcdParamEnumValue OVERLOAD_FLAGSEnum[] = {
	{"broadcast", broadcast},
	{"unicast", unicast},
	{"clientflag", clientflag},
	{NULL, 0}
};

const LcdParamEnumValue RelayOptEnum[] = {
	{"disabled",0x00},
	{"insert",0x01},
	{NULL, 0}
};

const LcdParamEnumValue RelayPolicyEnum[] = {
        {"keep", keep},
        {"drop", drop},
	{"replace", replace},
        {NULL, 0}
};

#endif

const LcdParamEnumValue userActionEnum[] = {
//  {"NONE", NPW_ACTION_NONE},
  {"PERMIT", NPW_ACTION_PERMIT},
  {"DENY", NPW_ACTION_DENY},
//  {"FORWARD", NPW_ACTION_FORWARD},
//  {"DROP", NPW_ACTION_DROP},
//  {"LOG", NPW_ACTION_LOG},
//  {"ACCT", NPW_ACTION_ACCT},
//  {"TAPE", NPW_ACTION_TAP},
 {NULL, 0}};

const LcdParamEnumValue userSpeedEnum[] = {
  {"NoLimit", NPW_SPEED_NOLIMIT},
  {"Block", NPW_SPEED_ZERO},
  {"64Kbit", NPW_SPEED_64Kbit},
  {"128Kbit", NPW_SPEED_128Kbit},
  {"192Kbit", NPW_SPEED_192Kbit}, 
  {"256Kbit", NPW_SPEED_256Kbit},
  {"384Kbit", NPW_SPEED_384Kbit},
  {"512Kbit", NPW_SPEED_512Kbit},
  {"768Kbit", NPW_SPEED_768Kbit},
  {"1Mbit", NPW_SPEED_1Mbit},
  {"1.5Mbit", NPW_SPEED_1_5Mbit}, // jschen: allow more speed
  {"2Mbit", NPW_SPEED_2Mbit},
  {"3Mbit", NPW_SPEED_3Mbit},
  {"4Mbit", NPW_SPEED_4Mbit},
  {"5Mbit", NPW_SPEED_5Mbit},
  {"6Mbit", NPW_SPEED_6Mbit},
  {"7Mbit", NPW_SPEED_7Mbit},
  {"8Mbit", NPW_SPEED_8Mbit},
  {"9Mbit", NPW_SPEED_9Mbit},
  {"10Mbit", NPW_SPEED_10Mbit},
  {"11Mbit", NPW_SPEED_11Mbit},
  {"12Mbit", NPW_SPEED_12Mbit},
  {"13Mbit", NPW_SPEED_13Mbit},
  {"14Mbit", NPW_SPEED_14Mbit},
  {"15Mbit", NPW_SPEED_15Mbit},
  {"16Mbit", NPW_SPEED_16Mbit},
  {"17Mbit", NPW_SPEED_17Mbit},
  {"18Mbit", NPW_SPEED_18Mbit},
  {"19Mbit", NPW_SPEED_19Mbit},
  {"20Mbit", NPW_SPEED_20Mbit},
  {"21Mbit", NPW_SPEED_21Mbit},
  {"22Mbit", NPW_SPEED_22Mbit},
  {"23Mbit", NPW_SPEED_23Mbit},
  {"24Mbit", NPW_SPEED_24Mbit},
  {"25Mbit", NPW_SPEED_25Mbit},
  {"26Mbit", NPW_SPEED_26Mbit},
  {"27Mbit", NPW_SPEED_27Mbit},
  {"28Mbit", NPW_SPEED_28Mbit},
  {"29Mbit", NPW_SPEED_29Mbit},
  {"30Mbit", NPW_SPEED_30Mbit},
  {"31Mbit", NPW_SPEED_31Mbit},
  {"32Mbit", NPW_SPEED_32Mbit},
  {"33Mbit", NPW_SPEED_33Mbit},
  {"34Mbit", NPW_SPEED_34Mbit},
  {"35Mbit", NPW_SPEED_35Mbit},
  {"36Mbit", NPW_SPEED_36Mbit},
  {"37Mbit", NPW_SPEED_37Mbit},
  {"38Mbit", NPW_SPEED_38Mbit},
  {"39Mbit", NPW_SPEED_39Mbit},
  {"40Mbit", NPW_SPEED_40Mbit},
//  {"41Mbit", NPW_SPEED_41Mbit},
//  {"42Mbit", NPW_SPEED_42Mbit},
//  {"43Mbit", NPW_SPEED_43Mbit},
//  {"44Mbit", NPW_SPEED_44Mbit},
//  {"45Mbit", NPW_SPEED_45Mbit},
//  {"46Mbit", NPW_SPEED_46Mbit},
//  {"47Mbit", NPW_SPEED_47Mbit},
//  {"48Mbit", NPW_SPEED_48Mbit},
//  {"49Mbit", NPW_SPEED_49Mbit},
//  {"50Mbit", NPW_SPEED_50Mbit},
//  {"55Mbit", NPW_SPEED_55Mbit},
//  {"60Mbit", NPW_SPEED_60Mbit},
//  {"65Mbit", NPW_SPEED_65Mbit},
//  {"70Mbit", NPW_SPEED_70Mbit},
//  {"75Mbit", NPW_SPEED_75Mbit},
//  {"80Mbit", NPW_SPEED_80Mbit},
//  {"85Mbit", NPW_SPEED_85Mbit},
//  {"90Mbit", NPW_SPEED_90Mbit},
//  {"95Mbit", NPW_SPEED_95Mbit},
//  {"100Mbit", NPW_SPEED_100Mbit},
 {NULL, 0}};

const LcdParamEnumValue ifName2ifNumEnum[] = {
    {"eth1",            EXT_ETHER_IF},
    {"eth2",            EXT_ETHER_IF2},
#if ((SWITCHFUN==FYES) && (TAGVlAN==FYES))  /*add by zgd*/
    {"eth3",            EXT_ETHER_IF3},
    {"eth4",            EXT_ETHER_IF4},
    {"eth5",            EXT_ETHER_IF5},
    {"eth6",            EXT_ETHER_IF6},
    {"eth7",            EXT_ETHER_IF7},
    {"eth8",            EXT_ETHER_IF8},
    {"eth9",            EXT_ETHER_IF9},
    {"eth10",           EXT_ETHER_IF10},
    {"eth11",           EXT_ETHER_IF11},
    {"eth12",           EXT_ETHER_IF12},
    {"eth13",           EXT_ETHER_IF13},
    {"eth14",           EXT_ETHER_IF14},
    {"eth15",           EXT_ETHER_IF15},
    {"eth16",           EXT_ETHER_IF16},
    {"eth17",           EXT_ETHER_IF17},
    {"eth18",           EXT_ETHER_IF18},
    {"eth19",           EXT_ETHER_IF19},
    {"eth20",           EXT_ETHER_IF20},
    {"eth21",           EXT_ETHER_IF21},
    {"eth22",           EXT_ETHER_IF22},
    {"eth23",           EXT_ETHER_IF23},
    {"eth24",           EXT_ETHER_IF24},
    {"eth25",           EXT_ETHER_IF25},
    {"eth26",           EXT_ETHER_IF26},
    {"eth27",           EXT_ETHER_IF27},
    {"eth28",           EXT_ETHER_IF28},
    {"eth29",           EXT_ETHER_IF29},
    {"eth30",           EXT_ETHER_IF30},
    {"eth31",           EXT_ETHER_IF31},
    {"eth32",           EXT_ETHER_IF32},
    {"eth33",           EXT_ETHER_IF33},
    {"eth34",           EXT_ETHER_IF34},
    {"eth35",           EXT_ETHER_IF35},
    {"eth36",           EXT_ETHER_IF36},
    {"eth37",           EXT_ETHER_IF37},
    {"eth38",           EXT_ETHER_IF38},
    {"eth39",           EXT_ETHER_IF39},
    {"eth40",           EXT_ETHER_IF40},
    {"eth41",           EXT_ETHER_IF41},
    {"eth42",           EXT_ETHER_IF42},
    {"eth43",           EXT_ETHER_IF43},
    {"eth44",           EXT_ETHER_IF44},
    {"eth45",           EXT_ETHER_IF45},
    {"eth46",           EXT_ETHER_IF46},
    {"eth47",           EXT_ETHER_IF47},
    {"eth48",           EXT_ETHER_IF48},
    {"eth49",           EXT_ETHER_IF49},
#else
#if ( VIRTUAL_DIALER == FYES)
#else
# if ( HOST_ETH3 == FYES )
    {"eth3",            EXT_ETHER_IF3},
# if ( HOST_ETH5 == FYES )
    {"eth4",            EXT_ETHER_IF4},
    {"eth5",            EXT_ETHER_IF5},
# if ( HOST_ETH9 == FYES )
    {"eth6",            EXT_ETHER_IF6},
    {"eth7",            EXT_ETHER_IF7},
    {"eth8",            EXT_ETHER_IF8},
    {"eth9",            EXT_ETHER_IF9},
# if ( HOST_ETH25 == FYES )
    {"eth10",            EXT_ETHER_IF10},
    {"eth11",            EXT_ETHER_IF11},
    {"eth12",            EXT_ETHER_IF12},
    {"eth13",            EXT_ETHER_IF13},
    {"eth14",            EXT_ETHER_IF14},
    {"eth15",            EXT_ETHER_IF15},
    {"eth16",            EXT_ETHER_IF16},
    {"eth17",            EXT_ETHER_IF17},
    {"eth18",            EXT_ETHER_IF18},
    {"eth19",            EXT_ETHER_IF19},
    {"eth20",            EXT_ETHER_IF20},
    {"eth21",            EXT_ETHER_IF21},
    {"eth22",            EXT_ETHER_IF22},
    {"eth23",            EXT_ETHER_IF23},
    {"eth24",            EXT_ETHER_IF24},
    {"eth25",            EXT_ETHER_IF25},
# if ( HOST_ETH49 == FYES )
    {"eth26", EXT_ETHER_IF26},
    {"eth27", EXT_ETHER_IF27},
    {"eth28", EXT_ETHER_IF28},
    {"eth29", EXT_ETHER_IF29},
    {"eth30", EXT_ETHER_IF30},
    {"eth31", EXT_ETHER_IF31},
    {"eth32", EXT_ETHER_IF32},
    {"eth33", EXT_ETHER_IF33},
    {"eth34", EXT_ETHER_IF34},
    {"eth35", EXT_ETHER_IF35},
    {"eth36", EXT_ETHER_IF36},
    {"eth37", EXT_ETHER_IF37},
    {"eth38", EXT_ETHER_IF38},
    {"eth39", EXT_ETHER_IF39},
    {"eth40", EXT_ETHER_IF40},
    {"eth41", EXT_ETHER_IF41},
    {"eth42", EXT_ETHER_IF42},
    {"eth43", EXT_ETHER_IF43},
    {"eth44", EXT_ETHER_IF44},
    {"eth45", EXT_ETHER_IF45},
    {"eth46", EXT_ETHER_IF46},
    {"eth47", EXT_ETHER_IF47},
    {"eth48", EXT_ETHER_IF48},
    {"eth49", EXT_ETHER_IF49},
# endif /* HOST_ETH49 */
# endif /* HOST_ETH25 */
# endif /* HOST_ETH9 */
# endif /* HOST_ETH5 */
# endif /* HOST_ETH3 */
#endif /* VIRTUAL_DIALER */
#endif/*((SWITCHFUN==FYES) && (TAGVlAN==FYES))*/
    {"dialer",          INACTIVE_IF},
    {"multicast",       MULTI_IF},
    {"reject",          REJECT_IF},
    {"blockhole",       BLACKHOLE_IF},
    {"local",           LOCAL_IF},
    {"loopback",        LOOPBACK_IF},
    {NULL, 0}};

const LcdParamEnumValue etherIntEnum[] = {
    {"eth1", EXT_ETHER_IF},
    {"eth2", EXT_ETHER_IF2},
#if ((SWITCHFUN==FYES) && (TAGVlAN==FYES))  /*add by zgd*/
    {"eth3", EXT_ETHER_IF3},
    {"eth4", EXT_ETHER_IF4},
    {"eth5", EXT_ETHER_IF5},
    {"eth6", EXT_ETHER_IF6},
    {"eth7", EXT_ETHER_IF7},
    {"eth8", EXT_ETHER_IF8},
    {"eth9", EXT_ETHER_IF9},
    {"eth10", EXT_ETHER_IF10},
    {"eth11", EXT_ETHER_IF11},
    {"eth12", EXT_ETHER_IF12},
    {"eth13", EXT_ETHER_IF13},
    {"eth14", EXT_ETHER_IF14},
    {"eth15", EXT_ETHER_IF15},
    {"eth16", EXT_ETHER_IF16},
    {"eth17", EXT_ETHER_IF17},
    {"eth18", EXT_ETHER_IF18},
    {"eth19", EXT_ETHER_IF19},
    {"eth20", EXT_ETHER_IF20},
    {"eth21", EXT_ETHER_IF21},
    {"eth22", EXT_ETHER_IF22},
    {"eth23", EXT_ETHER_IF23},
    {"eth24", EXT_ETHER_IF24},
    {"eth25", EXT_ETHER_IF25},
    {"eth26", EXT_ETHER_IF26},
    {"eth27", EXT_ETHER_IF27},
    {"eth28", EXT_ETHER_IF28},
    {"eth29", EXT_ETHER_IF29},
    {"eth30", EXT_ETHER_IF30},
    {"eth31", EXT_ETHER_IF31},
    {"eth32", EXT_ETHER_IF32},
    {"eth33", EXT_ETHER_IF33},
    {"eth34", EXT_ETHER_IF34},
    {"eth35", EXT_ETHER_IF35},
    {"eth36", EXT_ETHER_IF36},
    {"eth37", EXT_ETHER_IF37},
    {"eth38", EXT_ETHER_IF38},
    {"eth39", EXT_ETHER_IF39},
    {"eth40", EXT_ETHER_IF40},
    {"eth41", EXT_ETHER_IF41},
    {"eth42", EXT_ETHER_IF42},
    {"eth43", EXT_ETHER_IF43},
    {"eth44", EXT_ETHER_IF44},
    {"eth45", EXT_ETHER_IF45},
    {"eth46", EXT_ETHER_IF46},
    {"eth47", EXT_ETHER_IF47},
    {"eth48", EXT_ETHER_IF48},
    {"eth49", EXT_ETHER_IF49},
#else
#if ( VIRTUAL_DIALER == FYES)
#else
# if ( HOST_ETH3 == FYES )
    {"eth3", EXT_ETHER_IF3},
# if ( HOST_ETH5 == FYES )
    {"eth4", EXT_ETHER_IF4},
    {"eth5", EXT_ETHER_IF5},
# if ( HOST_ETH9 == FYES )
    {"eth6", EXT_ETHER_IF6},
    {"eth7", EXT_ETHER_IF7},
    {"eth8", EXT_ETHER_IF8},
    {"eth9", EXT_ETHER_IF9},
# if ( HOST_ETH25 == FYES )
    {"eth10", EXT_ETHER_IF10},
    {"eth11", EXT_ETHER_IF11},
    {"eth12", EXT_ETHER_IF12},
    {"eth13", EXT_ETHER_IF13},
    {"eth14", EXT_ETHER_IF14},
    {"eth15", EXT_ETHER_IF15},
    {"eth16", EXT_ETHER_IF16},
    {"eth17", EXT_ETHER_IF17},
    {"eth18", EXT_ETHER_IF18},
    {"eth19", EXT_ETHER_IF19},
    {"eth20", EXT_ETHER_IF20},
    {"eth21", EXT_ETHER_IF21},
    {"eth22", EXT_ETHER_IF22},
    {"eth23", EXT_ETHER_IF23},
    {"eth24", EXT_ETHER_IF24},
    {"eth25", EXT_ETHER_IF25},
# if ( HOST_ETH49 == FYES )
    {"eth26", EXT_ETHER_IF26},
    {"eth27", EXT_ETHER_IF27},
    {"eth28", EXT_ETHER_IF28},
    {"eth29", EXT_ETHER_IF29},
    {"eth30", EXT_ETHER_IF30},
    {"eth31", EXT_ETHER_IF31},
    {"eth32", EXT_ETHER_IF32},
    {"eth33", EXT_ETHER_IF33},
    {"eth34", EXT_ETHER_IF34},
    {"eth35", EXT_ETHER_IF35},
    {"eth36", EXT_ETHER_IF36},
    {"eth37", EXT_ETHER_IF37},
    {"eth38", EXT_ETHER_IF38},
    {"eth39", EXT_ETHER_IF39},
    {"eth40", EXT_ETHER_IF40},
    {"eth41", EXT_ETHER_IF41},
    {"eth42", EXT_ETHER_IF42},
    {"eth43", EXT_ETHER_IF43},
    {"eth44", EXT_ETHER_IF44},
    {"eth45", EXT_ETHER_IF45},
    {"eth46", EXT_ETHER_IF46},
    {"eth47", EXT_ETHER_IF47},
    {"eth48", EXT_ETHER_IF48},
    {"eth49", EXT_ETHER_IF49},
# endif /* HOST_ETH49 */
# endif /* HOST_ETH25 */
# endif /* HOST_ETH9 */
# endif /* HOST_ETH5 */
# endif /* HOST_ETH3 */
#endif /* VIRTUAL_DIALER */
#endif/*((SWITCHFUN==FYES) && (TAGVlAN==FYES))*/
{NULL, 0}};

const LcdParamEnumValue enableEnum[] = {
    {"enabled", TRUE},
    {"disabled", NO},
    {NULL, 0}};

const LcdParamEnumValue yesnoEnum[] = {
    {"Yes", TRUE},
    {"No", NO},
    {NULL, 0}};

	//ad by jyshen 060807
	#if (PDB == FYES)
const LcdParamEnumValue policyEnum[] = {
	{"disabled", 0},
	{"auto", 1},
    //{"manual", 2},
    //{"disabled", 3},
    {NULL, 0}};
const LcdParamEnumValue dayEnum[] = {	//ad by jyshen 060817
	{"everyday", 0},
    {"monday", 1},
    {"tuesday", 2},
    {"wednesday", 3},
    {"thursday", 4},
    {"friday", 5},
    {"saturday", 6},
    {"sunday", 7},
    {NULL, 0}};

const LcdParamEnumValue timeEnum[] = {	//ad by jyshen 060817
	{"00:00", 0},
    {"01:00", 1},
    {"02:00", 2},
    {"03:00", 3},
    {"04:00", 4},
    {"05:00", 5},
    {"06:00", 6},
    {"07:00", 7},
    {"08:00", 8},
    {"09:00", 9},
    {"10:00", 10},
    {"11:00", 11},
    {"12:00", 12},
    {"13:00", 13},
    {"14:00", 14},
    {"15:00", 15},
    {"16:00", 16},
    {"17:00", 17},
    {"18:00", 18},
    {"19:00", 19},
    {"20:00", 20},
    {"21:00", 21},
    {"22:00", 22},
    {"23:00", 23},
    {NULL, 0}};
	
	#endif //pdb
//end by jyshen

# if ( L2TP == FYES )
const LcdParamEnumValue tunnelProtocolEnum[] = {
    {"Disabled", TUNNEL_PROTOCOL_NONE},
    {"PPTP", TUNNEL_PROTOCOL_PPTP},
    {"L2TP", TUNNEL_PROTOCOL_L2TP},
    {NULL, 0}};

const LcdParamEnumValue pptpModeEnum[] = {
    {"Disabled", PPTP_DISABLED},
    {"PNS", PPTP_PNS_MODE},
    {"PAC", PPTP_PAC_MODE},
    {"Both", PPTP_BOTH_MODE},
    {NULL, 0}};
#endif

const LcdParamEnumValue tunnelTypeEnum[] = {
    {"disabled", 0},
    {"client", 1},
    {"server", 2},      /* added by llliu@080422 */
    {NULL, 0}};

const LcdParamEnumValue pppoeTypeEnum[] = {
    {"disabled", 0},
    {"client", 1},
#if (PPPOE_SRV == FYES)	/* added by llliu */
    {"server", 2},
#endif 
    {NULL, 0}};

#if (PPPOE_SRV == FYES) 
const LcdParamEnumValue macFilterEnum[] = {
    {"allow", 0},
    {"deny", 1},
    {NULL, 0}};

const LcdParamEnumValue bindPortEnum[] = {
    {"eth1", EXT_ETHER_IF},
    {"eth2", EXT_ETHER_IF2},
#if ( VIRTUAL_DIALER == FYES)
#else
# if ( HOST_ETH3 == FYES )
    {"eth3", EXT_ETHER_IF3},
# if ( HOST_ETH5 == FYES )
    {"eth4", EXT_ETHER_IF4},
    {"eth5", EXT_ETHER_IF5},
# if ( HOST_ETH9 == FYES )
    {"eth6", EXT_ETHER_IF6},
    {"eth7", EXT_ETHER_IF7},
    {"eth8", EXT_ETHER_IF8},
    {"eth9", EXT_ETHER_IF9},
# if ( HOST_ETH25 == FYES )
    {"eth10", EXT_ETHER_IF10},
    {"eth11", EXT_ETHER_IF11},
    {"eth12", EXT_ETHER_IF12},
    {"eth13", EXT_ETHER_IF13},
    {"eth14", EXT_ETHER_IF14},
    {"eth15", EXT_ETHER_IF15},
    {"eth16", EXT_ETHER_IF16},
    {"eth17", EXT_ETHER_IF17},
    {"eth18", EXT_ETHER_IF18},
    {"eth19", EXT_ETHER_IF19},
    {"eth20", EXT_ETHER_IF20},
    {"eth21", EXT_ETHER_IF21},
    {"eth22", EXT_ETHER_IF22},
    {"eth23", EXT_ETHER_IF23},
    {"eth24", EXT_ETHER_IF24},
    {"eth25", EXT_ETHER_IF25},
# if ( HOST_ETH49 == FYES )
    {"eth26", EXT_ETHER_IF26},
    {"eth27", EXT_ETHER_IF27},
    {"eth28", EXT_ETHER_IF28},
    {"eth29", EXT_ETHER_IF29},
    {"eth30", EXT_ETHER_IF30},
    {"eth31", EXT_ETHER_IF31},
    {"eth32", EXT_ETHER_IF32},
    {"eth33", EXT_ETHER_IF33},
    {"eth34", EXT_ETHER_IF34},
    {"eth35", EXT_ETHER_IF35},
    {"eth36", EXT_ETHER_IF36},
    {"eth37", EXT_ETHER_IF37},
    {"eth38", EXT_ETHER_IF38},
    {"eth39", EXT_ETHER_IF39},
    {"eth40", EXT_ETHER_IF40},
    {"eth41", EXT_ETHER_IF41},
    {"eth42", EXT_ETHER_IF42},
    {"eth43", EXT_ETHER_IF43},
    {"eth44", EXT_ETHER_IF44},
    {"eth45", EXT_ETHER_IF45},
    {"eth46", EXT_ETHER_IF46},
    {"eth47", EXT_ETHER_IF47},
    {"eth48", EXT_ETHER_IF48},
    {"eth49", EXT_ETHER_IF49},
# endif /* HOST_ETH49 */
# endif /* HOST_ETH25 */
# endif /* HOST_ETH9 */
# endif /* HOST_ETH5 */
# endif /* HOST_ETH3 */
#endif /* VIRTUAL_DIALER */
    {NULL, 0}
};

const LcdParamEnumValue pppoeAccModeEnum[] = {
    {"NONE", PPPOE_ACC_MODE_NONE},
    {"DATE", PPPOE_ACC_MODE_DATE},
    {"HOUR", PPPOE_ACC_MODE_HOUR},
    {"TRAFFIC", PPPOE_ACC_MODE_TRAFFIC},
    {NULL, 0}
};


#endif
const LcdParamEnumValue tqdiscEnum[] = {
    {"FIFO", 0},
    {"CBQ", 1},
    {"SFQ", 2},
    {NULL, 0}};

const LcdParamEnumValue matchtypeEnum[] = {
    {"MATCH", 0},
    {NULL, 0}};

const LcdParamEnumValue prototypeEnum[] = {
    {"ETHER", 0},
    {"IP", 1},
    {"UDP", 2},
    {"TCP", 3},
    {NULL, 0}};

const LcdParamEnumValue wirespeedtypeEnum[] = {
    {"Auto", 0},
    {"100M-FD", 1},
    {"100M-HD", 2},
    {"10M-FD", 3},
    {"10M-HD", 4},
#if NEW_ADDR
    {"100M-FD-MDI", 5},
    {"100M-HD-MDI", 6},
    {"10M-FD-MDI", 7},
    {"10M-HD-MDI", 8},
#endif
#if (LANPORT5 == FYES)
    {"1000M-FD", 9},
#endif
    {NULL, 0}};

/*add by chenyunhua*/
const LcdParamEnumValue tmWindowEnum[] = {
    { "NONE",           tmNone},
    { "EVERYDAY",       tmEveryday },
    { "MONDAY",         tmMonday },
    { "TUESDAY",        tmTuesday },
    { "WEDNESDAY",      tmWednesday },
    { "THURSDAY",       tmThursday},
    { "FRIDAY",         tmFriday },
    { "SATURDAY",       tmSaturday },
    { "SUNDAY",         tmSunday },
    { "WEEKDAY",        tmWeekday },
    { "WEEKEND",        tmWeekend },
    { NULL,             0 }
};

//add by jxy8@2003-12-26 for WebUserRight
#if ( WEB_SERVER == FYES )
const LcdParamEnumValue webPageColorEnum[] = {
#if (CHANGESKIN == FYES)
    {"Blue",0},
#endif	//end CHANGESKIN
    {"Green",1},
    {NULL,1}
};
const LcdParamEnumValue webAccessMethEnum[] = {
    {"NULL", 0},
    {"FULL", 1},
    {"BASIC", 2},
    {"DIGEST", 3},
    {NULL, 0},
};
#endif
//add end jxy8

const LcdParamEnumValue ethFlowCtrlTypeEnum[] = {
    {"Auto", 0},
    {"802.3x", 1},
    {"backPressure", 2},
    {NULL, 0}};

const LcdParamEnumValue etherTypeEnum[] = {
	{"ip", 		ETHER_IP},
	{"arp",		ETHER_ARP},
	{"rarp",	ETHER_RARP},
	{"addp",	ETHER_ADDP},
	{"aarp",	ETHER_AARP},
	{"ipx",		ETHER_IPX},
	{"pppoed",	ETHER_PPPOE_DISC},
	{"pppoes",	ETHER_PPPOE_SESS},
	{"dot1x",	ETHER_DOT1X_AHTH},
	{"dot1q",	ETHER_DOT1Q_TYPE},
    	{NULL, 0},
};

const LcdParamEnumValue ipProtocolEnum[] = {
	{"any", 	0},
	{"icmp", 	PROT_ICMP},
	{"igmp", 	PROT_IGMP},
	{"ipip", 	PROT_IPINIP},
	{"tcp", 	PROT_TCP},
	{"udp", 	PROT_UDP},
	{"gre",		PROT_GRE},
	{"esp",		PROT_ESP},
	{"ah",		PROT_AH},
	{"ospf",	PROT_OSPF},
    	{NULL, 0},
};

const LcdParamEnumValue udpPortEnum[] = {
//	{"any", 		0},
	{"echo(7)", 		IPPORT_ECHOS},
	{"dns(53)", 		IPPORT_DOMAIN},
	{"dhcps(67)", 		IPPORT_BOOTPS},
	{"dhcpc(68)", 		IPPORT_BOOTPC},
	{"tftp(69)", 		IPPORT_TFTP},
	{"nbns(137)", 		IPPORT_NETBIOS_UNSP},
	{"nbdgram(138)", 	IPPORT_NETBIOS_UDSP},
	{"radius(1645)", 	IPPORT_RADIUS_AUTH_OLD},
	{"radacct(1646)", 	IPPORT_RADIUS_ACCT_OLD},
	{"ietf_radius(1812)", 	IPPORT_RADIUS_AUTH},
	{"ietf_radacct(1813)", 	IPPORT_RADIUS_ACCT},
    	{NULL, 0},
};

const LcdParamEnumValue ipAddressEnum[] = {
	{"any", 	0},
	{"host", 	(UINT32)0xFFFFFFFF},
    	{NULL, 0},
};

const LcdParamEnumValue ipNetmaskEnum[] = {
	{"any", 	0},
	{"host", 	(UINT32)0xFFFFFFFF},
    	{NULL, 0},
};

const LcdParamEnumValue tcpPortEnum[] = {
//	{"any", 		0,},
	{"echo(7)", 		IPPORT_ECHOS},
	{"dns(53)", 		IPPORT_DOMAIN},
	{"http(80)", 		IPPORT_HTTP},
	{"ident(113)",		IPPORT_IDENT},
    	{NULL, 0},
};

const LcdParamEnumValue       pktFieldOffsetEnum[] = {
    { "dmac",          offsetof(ETHER_HDR, eh_dst)},   	 		/* 6 */
    { "smac",          offsetof(ETHER_HDR, eh_src)},   	 		/* 6 */
    { "etype",         offsetof(ETHER_HDR, eh_type)},	 		/* 2 */
    { "ip_vlen",       offsetof(IP_PKT, ipp_ip)},	 		/* 1 */
    { "ip_tos",        offsetof(IP_PKT, ipp_ip.ip_tos)}, 		/* 1 */
    { "ip_len",        offsetof(IP_PKT, ipp_ip.ip_len)}, 		/* 2 */
    { "ip_id",         offsetof(IP_PKT, ipp_ip.ip_id)},	 		/* 2 */
    { "ip_frag",       offsetof(IP_PKT, ipp_ip.ip_frag)},		/* 2 */
    { "ip_ttl",        offsetof(IP_PKT, ipp_ip.ip_ttl)}, 		/* 1 */
    { "ip_proto",      offsetof(IP_PKT, ipp_ip.ip_protocol)},		/* 1 */
    { "ip_cksum",      offsetof(IP_PKT, ipp_ip.ip_chsum)},		/* 2 */
    { "ip_src",        offsetof(IP_PKT, ipp_ip.ip_src)},		/* 4 */
    { "ip_dst",        offsetof(IP_PKT, ipp_ip.ip_dst)},		/* 4 */
    { "ip_data",       offsetof(IP_PKT, ipp_data)},			/* 4 */
//
    { "ip_sport",      offsetof(UDP_PKT, udpp_udp.udp_src_port)}, 	/* 2 */
    { "ip_dport",      offsetof(UDP_PKT, udpp_udp.udp_dst_port)}, 	/* 2 */
//
 // { "udp_sport",     offsetof(UDP_PKT, udpp_udp.udp_src_port)}, 	/* 2 */
 // { "udp_dport",     offsetof(UDP_PKT, udpp_udp.udp_dst_port)}, 	/* 2 */
    { "udp_len",       offsetof(UDP_PKT, udpp_udp.udp_len)}, 	  	/* 2 */
    { "udp_cksum",     offsetof(UDP_PKT, udpp_udp.udp_chsum)},	 	/* 2 */
    { "udp_data",      offsetof(UDP_PKT, udpp_data)},		 	/* 2 */
//
 // { "tcp_sport",     offsetof(TCP_PKT, tcpp_tcp.tcp_sport)}, 		/* 2 */
 // { "tcp_dport",     offsetof(TCP_PKT, tcpp_tcp.tcp_dport)}, 		/* 2 */
    { "tcp_seq",       offsetof(TCP_PKT, tcpp_tcp.tcp_seq)}, 		/* 4 */
    { "tcp_ack",       offsetof(TCP_PKT, tcpp_tcp.tcp_ack)}, 		/* 4 */
    { "tcp_offset",    offsetof(TCP_PKT, tcpp_tcp.tcp_offset)}, 	/* 1 */
    { "tcp_code",      offsetof(TCP_PKT, tcpp_tcp.tcp_code)}, 		/* 1 */
    { "tcp_win",       offsetof(TCP_PKT, tcpp_tcp.tcp_window)}, 	/* 2 */
    { "tcp_cksum",     offsetof(TCP_PKT, tcpp_tcp.tcp_cksum)}, 		/* 2 */
    { "tcp_urgent",    offsetof(TCP_PKT, tcpp_tcp.tcp_urgptr)}, 	/* 2 */
    { "tcp_data",      offsetof(TCP_PKT, tcpp_tcp.tcp_data)}, 		/* 1 */
//
    { "icmp_type",     offsetof(ICMP_PKT, icmpp_icmp.icmp_type)}, 	/* 1 */
    { "icmp_code",     offsetof(ICMP_PKT, icmpp_icmp.icmp_code)}, 	/* 1 */
    { "icmp_chsum",    offsetof(ICMP_PKT, icmpp_icmp.icmp_chsum)}, 	/* 2 */
    { "icmp_part1",    offsetof(ICMP_PKT, icmpp_icmp.icmp_part1)}, 	/* 2 */
    { "icmp_part2",    offsetof(ICMP_PKT, icmpp_icmp.icmp_part2)}, 	/* 2 */
    { "icmp_data",     offsetof(ICMP_PKT, icmpp_data)}, 		/* 2 */
    { 0,                0 }
};

const LcdParamEnumValue       pktFieldLengthEnum[] = {
# if 0
    { "dmac",          sizeof(ETHER_HDR.eh_dst)},   	 		/* 6 */
    { "smac",          sizeof(ETHER_HDR.eh_src)},   	 		/* 6 */
    { "etype",         sizeof(ETHER_HDR.eh_type)},	 		/* 2 */
    { "ip_vlen",       sizeof(IP_PKT, ipp_ip)},	 		/* 1 */
    { "ip_tos",        sizeof(IP_PKT, ipp_ip.ip_tos)}, 		/* 1 */
    { "ip_len",        sizeof(IP_PKT, ipp_ip.ip_len)}, 		/* 2 */
    { "ip_id",         sizeof(IP_PKT, ipp_ip.ip_id)},	 		/* 2 */
    { "ip_frag",       sizeof(IP_PKT, ipp_ip.ip_frag)},		/* 2 */
    { "ip_ttl",        sizeof(IP_PKT, ipp_ip.ip_ttl)}, 		/* 1 */
    { "ip_proto",      sizeof(IP_PKT, ipp_ip.ip_protocol)},		/* 1 */
    { "ip_cksum",      sizeof(IP_PKT, ipp_ip.ip_chsum)},		/* 2 */
    { "ip_src",        sizeof(IP_PKT, ipp_ip.ip_src)},		/* 4 */
    { "ip_dst",        sizeof(IP_PKT, ipp_ip.ip_dst)},		/* 4 */
//
    { "ip_sport",      sizeof(UDP_PKT, udpp_udp.udp_src_port)}, 	/* 2 */
    { "ip_dport",      sizeof(UDP_PKT, udpp_udp.udp_dst_port)}, 	/* 2 */
//
 // { "udp_sport",     sizeof(UDP_PKT, udpp_udp.udp_src_port)}, 	/* 2 */
 // { "udp_dport",     sizeof(UDP_PKT, udpp_udp.udp_dst_port)}, 	/* 2 */
    { "udp_len",       sizeof(UDP_PKT, udpp_udp.udp_len)}, 	  	/* 2 */
    { "udp_cksum",     sizeof(UDP_PKT, udpp_udp.udp_chsum)},	 	/* 2 */
//
 // { "tcp_sport",     sizeof(TCP_PKT, tcpp_tcp.tcp_sport)}, 		/* 2 */
 // { "tcp_dport",     sizeof(TCP_PKT, tcpp_tcp.tcp_dport)}, 		/* 2 */
    { "tcp_seq",       sizeof(TCP_PKT, tcpp_tcp.tcp_seq)}, 		/* 4 */
    { "tcp_ack",       sizeof(TCP_PKT, tcpp_tcp.tcp_ack)}, 		/* 4 */
    { "tcp_offset",    sizeof(TCP_PKT, tcpp_tcp.tcp_offset)}, 	/* 1 */
    { "tcp_code",      sizeof(TCP_PKT, tcpp_tcp.tcp_code)}, 		/* 1 */
    { "tcp_win",       sizeof(TCP_PKT, tcpp_tcp.tcp_window)}, 	/* 2 */
    { "tcp_chsum",     sizeof(TCP_PKT, tcpp_tcp.tcp_chsum)}, 		/* 2 */
    { "tcp_urgent",    sizeof(TCP_PKT, tcpp_tcp.tcp_urgptr)}, 	/* 2 */
//
    { "icmp_type",     sizeof(ICMP_PKT, icmpp_icmp.icmp_type)}, 	/* 1 */
    { "icmp_code",     sizeof(ICMP_PKT, icmpp_icmp.icmp_code)}, 	/* 1 */
    { "icmp_chsum",    sizeof(ICMP_PKT, icmpp_icmp.icmp_chsum)}, 	/* 2 */
    { "icmp_part1",    sizeof(ICMP_PKT, icmpp_icmp.icmp_part1)}, 	/* 2 */
    { "icmp_part2",    sizeof(ICMP_PKT, icmpp_icmp.icmp_part2)}, 	/* 2 */
# endif
    { 0,                0 }
};

#if(DOT1X == FYES)
const LcdParamEnumValue Dot1xAuthSchemeEnum[] = {
    {"local", SCHEME_LOCAL},
    {"radius", SCHEME_RADIUS},
    {"radius-local", SCHEME_RADIUS_LOCAL},
    {NULL, 0}};

const LcdParamEnumValue Dot1xPortMethodEnum[] = {
    {"port-based", METHOD_PORT_BASED},
    {"mac-based", METHOD_MAC_BASED},
    {NULL, 0}};

const LcdParamEnumValue Dot1xPortControlEnum[] = {
    {"authorized-force", CTL_FORCE_AUTHORIZED},
    {"auto", CTL_AUTO},
    {"unauthorized-force", CTL_FORCE_UNAUTHORIZED},
    {NULL, 0}};

const LcdParamEnumValue Dot1xPortStatusEnum[] = {
    {"authorized", STATUS_AUTHORIZED},
    {"unauthorized", STATUS_UNAUTHORIZED},
    {"undefined",STATUS_UNDEFINED},
    {NULL, 0}};

const LcdParamEnumValue Dot1xSuppPAEStateEnum[] = {
    {"disconnected",SUPP_PAE_DISCONNECTED},
    {"logoff",SUPP_PAE_LOGOFF},
    {"connecting",SUPP_PAE_CONNECTING},
    {"authenticating",SUPP_PAE_AUTHENTICATING},
    {"authenticated",SUPP_PAE_AUTHENTICATED},
    {"acquired",SUPP_PAE_ACQUIRED},
    {"held",SUPP_PAE_HELD},
    {"restart",SUPP_PAE_RESTART},
    {"force-auth",SUPP_PAE_sFORCE_AUTH},
    {"force-unauth",SUPP_PAE_sFORCE_UNAUTH},
    {NULL, 0}};

const LcdParamEnumValue Dot1xSuppBackendStateEnum[] = {
    {"initialize", BE_SUPP_INITIALIZE},
    {"idle", BE_SUPP_IDLE},
    {"request", BE_SUPP_REQUEST},
    {"response", BE_SUPP_RESPONSE},
    {"receive", BE_SUPP_RECEIVE},
    {"fail", BE_SUPP_FAIL},
    {"success", BE_SUPP_SUCCESS},
    {"timeout", BE_SUPP_TIMEOUT},
    {NULL, 0}};

const LcdParamEnumValue Dot1xAuthBackendAuthStateEnum[]={
    {"initialize", BE_AUTH_INITIALIZE},
    {"idle", BE_SUPP_IDLE},
    {"request", BE_SUPP_REQUEST},
    {"response", BE_SUPP_RESPONSE},
    {"receive", BE_SUPP_RECEIVE},
    {"fail", BE_SUPP_FAIL},
    {"success", BE_SUPP_SUCCESS},
    {"timeout", BE_SUPP_TIMEOUT},
    {NULL, 0}};

const LcdParamEnumValue Dot1xAuthenticMethod[] = {
    {"remote", REMOTE_AUTH_SERVER},
    {"local", LOCAL_AUTH_SERVER},
    {NULL, 0}};

const LcdParamEnumValue Dot1xSessionTerminateCause[] = {
    {"supp-logoff", CAUSE_SUPPLICANT_LOGOFF},
    {"port-failure", CAUSE_PORT_FAILURE},
    {"supp-restart", CAUSE_SUPPLICANT_RESTART},
    {"reauth-failed", CAUSE_REAUTH_FAILED},
    {"force-unauth", CAUSE_AUTH_CONTROL_FORCE_UNAUTH},
    {"port-reinit", CAUSE_PORT_REINIT},
    {"admin-disabled",CAUSE_PORT_ADMIN_DISABLED,},
    {"not-terminate-yet", CAUSE_NOT_TERMINATE_YET},
    {NULL, 0}};

#endif

#if (SWITCHFUN == FYES)   /*add by zgd*/
const LcdParamEnumValue MirrFilEnum[] = {
  {"all", NPW_MON_ALL},
  {"samac", NPW_MON_SAMAC},
  {"damac", NPW_MON_DAMAC},
  {NULL, 0},
};

const LcdParamEnumValue MirrAttEnum[] = {
  {"normal", NPW_MIRR_NORMAL},
  {"mirrored", NPW_MIRR_MONED},
  {"mirror", NPW_MIRR_MON},
  {NULL, 0},
};

const LcdParamEnumValue MirrDirEnum[] = {
  {"mirrorall", NPW_MIRR_INOUT},
  {"mirrorin", NPW_MIRR_IN},
  {"mirrorout", NPW_MIRR_OUT},
  {NULL, 0},
};

const LcdParamEnumValue TrunkNumEnum[] = {
  {"normal", NPW_TRUNK_NOM},
  {"trunk1", NPW_TRUNK_TK1},
  {"trunk2", NPW_TRUNK_TK2},
  {NULL, 0},
};

const LcdParamEnumValue TrunkModEnum[] = {
  {"SA", NPW_TRUNK_SA},
  {"DA", NPW_TRUNK_DA},
  {"SADA", NPW_TRUNK_SADA},
  {NULL, 0},
};

const LcdParamEnumValue InspeedModEnum[] = {
  {"none", NPW_INSPEED_NON},
  {"all", NPW_INSPEED_ALL},
  {"broad", NPW_INSPEED_BRO},
  {"bromul", NPW_INSPEED_BRML},
  {"dlf", NPW_INSPEED_DLF},
  {NULL, 0},
};

#if (TAGVlAN == FYES)
const LcdParamEnumValue VlanModEnum[] = {
  {"none", NPW_VLAN_NONE},
  {"portvlan", NPW_VLAN_PORT},
  {"tagvlan", NPW_VLAN_TAG},
  {NULL, 0},
};

const LcdParamEnumValue VlanComModEnum[] = {
  {"normal", NPW_VLAN_COMNONE},
  {"super", NPW_VLAN_ARGENT},
  {"l3vlan", NPW_VLAN_ROUTE},
  {NULL, 0},
};

const LcdParamEnumValue VlanPhyNumEnum[] = {
  {"lan", NPW_VLAN_LAN},
  {"wan1", NPW_VLAN_WAN1},
  {"wan2", NPW_VLAN_WAN2},
  {"wan3", NPW_VLAN_WAN3},
  {"wan4", NPW_VLAN_WAN4},
  {NULL, 0},
};

const LcdParamEnumValue VlanTypeEnum[] = {
  {"common", NPW_VLAN_TYPNONE},
  {"super", NPW_VLAN_SUPER},
  {"sub", NPW_VLAN_SUB},
  {NULL, 0},
};

const LcdParamEnumValue TagVlanInEnum[] = {
  {"exclude", NPW_VLAN_EX},
  {"include", NPW_VLAN_INC},
  {NULL, 0},
};

const LcdParamEnumValue TagVlanOutEnum[] = {
  {"tagged", NPW_VLAN_TAGGED},
  {"untag", NPW_VLAN_UNTAG},
  {NULL, 0},
};
#endif

const LcdParamEnumValue PriModEnum[] = {
  {"none", NPW_PRI_NONE},
  {"port", NPW_PRI_PORT},
  {"8021p", NPW_PRI_8021P},
  {"dscp", NPW_PRI_DSCP},
  {NULL, 0},
};

const LcdParamEnumValue PriRuleEnum[] = {
  {"scale", NPW_PRI_SCALE},
  {"fix", NPW_PRI_FIX},
  {NULL, 0},
};

const LcdParamEnumValue PriValueEnum[] = {
  {"lowest", NPW_PRI_LOWEST},
  {"lower", NPW_PRI_LOWER},
  {"higher", NPW_PRI_HIGHER},
  {"highest", NPW_PRI_HIGHEST},
  {NULL, 0},
};

const LcdParamEnumValue PortMacTypeEnum[] = {
  {"none", NPW_PORTMAC_NONE},
  {"static", NPW_PORTMAC_STATIC},
  {"filter", NPW_PORTMAC_FILTER},
  {"multi", NPW_PORTMAC_MULTI},
  {NULL, 0},
};

const LcdParamEnumValue PortMacPortInEnum[] = {
  {"exclude", NPW_PORTMAC_EX},
  {"include", NPW_PORTMAC_INC},
  {NULL, 0},
};

# endif

/* add by brwang */
const LcdParamEnumValue etherFilterTypeEnum[] = {
//  {"b&m",ETHERFILTER_BROADANDMULTI},
  {"b",ETHERFILTER_BROAD},
  {"m",ETHERFILTER_MULTI},
  {NULL, 0},
};

const LcdParamEnumValue pdbCategoryEnum[] = {
  {"IM",PDB_IM},
  {"P2P",PDB_P2P},
  {"DNS",PDB_DNS},
  {"URL",PDB_URL},
  {"GAME", PDB_GAME},
  {"OTHER",PDB_OTHER},
  {NULL, 0},
};


/*
 * added by llliu@080428.
 */
/*
 * for filter policy.
 */
const LcdParamEnumValue filterPolicyEnum[] = {
    {"allow", FP_ALLOW},
    {"deny", FP_DENY},
    
    /*
     * add new type add above.
     */
    {NULL, 0},
};


/* ----------------------------- ENUMINT defines -------------------- */

const LcdParamEnumIntValue ipNatPortRangeEnumInt = { 
	0, 20, NULL 
};

const LcdParamEnumIntValue idleLogoutEnumInt = { 
	0, 1440, NULL 
};

const LcdParamEnumIntValue percentEnumInt = { 
	0, 100, NULL 
};

const LcdParamEnumIntValue ipClassCEnumInt = { 
	1, 254, NULL 
};

const LcdParamEnumIntValue ipClassBEnumInt = { 
	1, UINT16_MAX, NULL 
};

const LcdParamEnumIntValue rtKeepAliveEnumInt = { 
	1000, 60000, NULL 
};

const LcdParamEnumIntValue rtKeepRetryEnumInt = { 
	3, 10000, NULL 
};

const LcdParamEnumIntValue etherSpeedEnumInt = { 
	NPW_SPEED_NOLIMIT, NPW_SPEED_100Mbit, userSpeedEnum
};

const LcdParamEnumIntValue rtMetricEnumInt = { 
	1, RT_METRIC_INF, NULL
};

const LcdParamEnumIntValue rtPrefEnumInt = { 
	RT_PREF_PREFERED, RT_PREF_INFINITE, NULL
};

const LcdParamEnumIntValue pppMruEnumInt = { 
	128, PPP_MTU, NULL
};

const LcdParamEnumIntValue pppLqmEnumInt = { 
	300, 60000, NULL
};

const LcdParamEnumIntValue ipProtocolEnumInt = { 
	0, UCHAR_MAX, ipProtocolEnum
};

const LcdParamEnumIntValue ipPortEnumInt = { 
	0, UINT16_MAX, NULL
};

const LcdParamEnumIntValue tcpPortEnumInt = { 
	1, UINT16_MAX, tcpPortEnum
};

const LcdParamEnumIntValue udpPortEnumInt = { 
	1, UINT16_MAX, udpPortEnum
};

const LcdParamEnumIntValue etherMtuEnumInt = { 
	IP_DEFAULT_MTU, ETHER_MTU, NULL
};

# if ( DOT1Q_VLAN == FYES )
const LcdParamEnumIntValue etherVlanTagEnumInt = { 
	0, USER_VLANID_MASK, NULL
};

const LcdParamEnumIntValue etherVlanGroupEnumInt = { 
	0, MAX_ETHER_MAC_PORTS-1, NULL
};
# endif

const LcdParamEnumIntValue tunnelMtuEnumInt = { 
	TUNNEL_DEFAULT_MTU, TUNNEL_MTU, NULL
};

const LcdParamEnumIntValue etherTypeEnumInt = { 
	0, UINT16_MAX, etherTypeEnum
};

const LcdParamEnumIntValue dnsListEnumInt = {
	6, MAX_IP_LIST, NULL
};

const LcdParamEnumIntValue filterGenOffsetEnumInt = {
//  0, 1510, NULL 
    0, 1510, pktFieldOffsetEnum
};

const LcdParamEnumIntValue filterGenLengthEnumInt = {
    0, 8, NULL
};

const LcdParamEnumIntValue tcpTimeoutEnumInt = {
    0, 200, NULL
};

const LcdParamEnumIntValue sysReloadEnumInt = {
    0, INT32_MAX>>10, NULL	/* microSec -> second */
};

const LcdParamEnumIntValue trunkGroupDigsEnumInt = {
    1, 2, NULL
};
#endif

const char pat_bit_torrent[] = { '\x13', 'b', 'i', 't', 't', 'o', 'r',\
		      'r', 'e', 'n', 't', ' ', 'p', 'r', 'o', 't', 'o', 'c', 'o', 'l'};

#if (FC == FYES)
#if (RED == FYES)
const LcdParamEnumValue fcRedShiftEnum[] = {
    {"0",	    0},
    {"1",	    1},
    {"2",	    2},
    {"3",	    3},
    {"4",	    4},
    {"5",	    5},
    {"6",	    6},
    {"7",	    7},
    {"8",	    8},
    {"9",	    9},
    {"10",	    10},
    {"11",	    11},
    {"12",	    12},
    {"13",	    13},
    {"14",	    14},
    {"15",	    15},
    {"16",	    16},
    {"17",	    17},
    {"18",	    18},
    {"19",	    19},
    {"20",	    20},
    {"21",	    21},
    {"22",	    22},
    {"23",	    23},
    {"24",	    24},
    {"25",	    25},
    {"26",	    26},
    {"27",	    27},
    {"28",	    28},
    {"29",	    29},
    {"30",	    30},
    {NULL,	    0}
};
#endif

const LcdParamEnumValue fbmPriorityEnum[] = {
    {"low",	    RL_PRIO_LOW},
    {"mid",	    RL_PRIO_MID},
    {"high",	    RL_PRIO_HIGH},
    {NULL,	    0}
};



#endif

//added by dhyu
#if (NOTICE == FYES)
const LcdParamEnumValue UrlOrHtmlEnum[] = {
    {"html",	HTML},
    {"url",	URL},
    {NULL,	0}
};
#endif

/* ------------------------ END OF ENUMINT defines ----------------- */
