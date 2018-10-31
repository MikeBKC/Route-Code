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
#ifndef MIBWIRELESS_SHARE_H
#define MIBWIRELESS_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"


/*
 * mac filterȫ������
 */
typedef struct st_wireless_mac_filter_global_profile
{
    FunEnableEnum filterEn;
    apMacFilterActionEn action;
}WirelessMacFilterGlobalProfile;

typedef struct st_wireless_mac_filter_profile
{
    mibHead head;
    MacAddr mac;
}WirelessMacFilterProfile;

typedef struct st_wireless_5g_mac_filter_profile
{
    mibHead head;
    MacAddr mac;
}Wireless5gMacFilterProfile;
/*
 *  ��ȫģʽ�ṹ��
 *  pskPsswd			-   WPA_WPA2��Ԥ������Կ
 *  EncrypType			-   WPA_WPA2�ļ����㷨
 *  wepKeyID			-   wep��Կ������Ч��Կ
 *  wepKeyType			-   wep��Կ��ʽ
 *  wepkey1			-   wep��Կ��ֵ
 *  wepkeyLength1		-   wep��Կ�ĳ���
 *
 */
struct st_AuthMode_prof{
    Uchar	        pskPsswd[UTT_PSK_PASSWD_LEN+1];
    Uchar		EncrypType[UTT_SEL_AUTHMODE_LEN+1];
    UINT32		wepKeyID;
    UINT32		wepKeyType;
    Uchar		wepkey1[UTT_PAS_KEY_VALUE_LEN+1];
    Uchar		wepkey2[UTT_PAS_KEY_VALUE_LEN+1];
    Uchar		wepkey3[UTT_PAS_KEY_VALUE_LEN+1];
    Uchar		wepkey4[UTT_PAS_KEY_VALUE_LEN+1];
    UINT32		wepkeyLength1;
    UINT32		wepkeyLength2;
    UINT32		wepkeyLength3;
    UINT32		wepkeyLength4;
    IPADDR		RADIUS_Server;
    UINT32		RADIUS_Port;
    Uchar               RADIUS_Key[UTT_RADIUS_KEY_LEN+ 1];
    UINT32		IEEE8021X;
    UINT32		RekeyInterval;
};

/* 
 * ap mode profile
 * st_AuthMode_prof	-   AuthMode  
 * ע�⣺WPS�İ�ȫģʽ�����߰�ȫ���õ�AP��ȫģʽ����ͬһ���ṹ��洢
 */
struct st_ap_prof {
    struct st_AuthMode_prof AuthMode;	
#if (UTT_SSID2_ENCRYPTION == FYES)
    struct st_AuthMode_prof AuthMode_2;	
#endif
};

/* 
 * ap client profile
 * apclientActive	-   apclient�Ƿ񼤻�
 * APCliMacs		-   AP��MAC��ַ
 * ApCliSsid		-   AP��SSID
 * AuthMode		-   ��ȫģʽ
 */
struct st_ap_cli_prof {
    FunEnableEnum	    apclientEnable;	
    MacAddr		    APCliMacs;
    Uchar		    ApCliSsid[UTT_SSID_LENTH+1];
    struct st_AuthMode_prof AuthMode;	
    Uchar		    wdsSwitch[4];
};

/*
 * wds profile ����repeater, bridge, lazy ����ģʽ
 * wdsEnable		-   WDSģʽ
 * WPSMacs0		-   AP��MAC��ַ 
 * AuthMode		-   ��ȫģʽ
 */
struct st_wds_prof {
    apWorkModeEn	    wdsEnable;
    char		    wdsEncrypType[UTT_WDS_MODE_LEN + 1];
    MacAddr		    WPSMacs0;
    MacAddr		    WPSMacs1;
    MacAddr		    WPSMacs2;
    MacAddr		    WPSMacs3;
    Uchar		    WdsList[UTT_WDS_MACLIST_LEN+1];
    UINT32		    WdsPhyMode;
    Uchar		    bridgeSSID1[UTT_SSID_LENTH+1];
    struct st_AuthMode_prof AuthMode;
};

typedef enum Wire_lessMode
{
	ONLY_11A = 2,
    ONLY_11G = 4,
    ONLY_11N = 6,
    AND_11A_11N = 8,
    AND_11G_11N = 7,
    AND_11B_11G_11N = 9,
    AND_11AC_11AN_11A = 14,
    AND_11AC_11AN = 15
}WirelessModeEn;

/* ���߰�ȫ���ýṹ��
 * ap			- ap�İ�ȫģʽ
 */
typedef struct st_wireless_safeconfig_profile{
    struct st_ap_prof ap;
    Uchar		    SelAuthMode[UTT_SEL_AUTHMODE_LEN+1];   
#if (UTT_SSID2_ENCRYPTION == FYES)
    Uchar		    SelAuthMode_2[UTT_SEL_AUTHMODE_LEN+1];   
#endif
} WirelessAPSafeConfigProfile;

/* 
 * apAdvanceProfile		-   ap Advance profile
 */
typedef struct st_advance_prof
{
    Uint32 rts;
    Uint32 section;
    Uint32 beacon;
    Uint32 dtim;
    Uint32 TxBurst;
    FunEnableEnum preamble;
    FunEnableEnum wmm;
} WirelessAdvanceProfile;



/* ���߻������ýṹ��
 * apWorkMode		-   ap����ģʽ
 * SSID			-   SSID
 * apworkmode		-   AP�Ĺ���ģʽ
 * WirelessMode		-   ����ģʽ 4 6 9 
 * BasicRate		-   BasicRate: bg,bgn,n:15, b:3; g,gn:351
 * AutoChannelSelect	-   �ŵ��Ƿ��Զ�ѡ��
 * Channel		-   Channel �ŵ�ֵ
 * chanWidth		-   Ƶ������
 * SSIDBroadCastEn	-   �Ƿ�����SSID�㲥
 * OperationMode	-   change the opmode to apcli
 * SelAuthMode		-   ��ȫģʽѡ��
 * ap			-   ap����ģʽ�ṹ��
 * apClient		-   ap�ͻ��˹���ģʽ�ṹ��
 * apWds		-   wds����ģʽ�ṹ�壬����repeater, bridge��lazy����ģʽ
*/
typedef struct st_wireless_baseconfig_profile
{ 
    apWorkModeEn	    apWorkMode;
    Uchar		    SSID1[UTT_SSID_LENTH+1];
#if (WLAN_MULTI_SSID == FYES)
    Uchar		    SSID2[UTT_SSID_LENTH+1];
#if (MIDDLE_SOFT == FYES)
    Uchar		    SSID3[UTT_SSID_LENTH+1];
    Uchar		    SSID4[UTT_SSID_LENTH+1];
#endif
#if (UTT_SSID_ISOLATION == FYES)
    UINT32                  ssidSeparate;
#endif
#endif
    WirelessModeEn	    WirelessMode;
    UINT32		    BasicRate;
    UINT32		    AutoChannelSelect;
    UINT32		    Channel; 
#if (FEATURE_AP == FYES)
	UINT32			BYPASS;
#endif
    UINT32		    ChanWidth;
    UINT32		    VChanWidth;
#if ((UTT_SMART_UI == FYES) && (WIRELESS_5G == FYES))
    UINT32                  Country;
#endif
    FunEnableEnum	    SSIDBroadCastEn;
    Uchar		    SelAuthMode[UTT_SEL_AUTHMODE_LEN+1];   
    UINT32		    OperationMode;
    IPADDR		    own_ip_addr;
    Uchar		    EAPifname[UTT_AP_IFNAME_LEN+1];
    Uchar		    PreAuthifname[UTT_AP_IFNAME_LEN+1];
    /*  Ralink��Ҫ��һЩ�����������ڴ����в�û�б���ֵ  */
    UINT32		    CountryRegion;
    UINT32		    CountryRegionABand;
    UINT32		    BssidNum;
    UINT32		    TxPower;
    UINT32		    PktAggregate;
    UINT32		    AutoProvisionEn;
    Uchar		    APAifsn[UTT_AP_BS_LEN];
    Uchar		    APCwmin[UTT_AP_BS_LEN];
    Uchar		    APCwmax[UTT_AP_BS_LEN];
    Uchar		    APTxop[UTT_AP_BS_LEN];
    Uchar		    APACM[UTT_AP_BS_LEN];
    Uchar		    BSSAifsn[UTT_AP_BS_LEN];
    Uchar		    BSSCwmin[UTT_AP_BS_LEN];
    Uchar		    BSSCwmax[UTT_AP_BS_LEN];
    Uchar		    BSSTxop[UTT_AP_BS_LEN];
    Uchar		    BSSACM[UTT_AP_BS_LEN];
    Uchar		    AckPolicy[UTT_AP_BS_LEN];
    UINT32		    APSDCapable;
    UINT32		    DLSCapable;
    UINT32		    NoForwarding;
    UINT32		    ShortSlot;
    UINT32		    CSPeriod;
    UINT32		    PreAuth;
    Uchar		    RekeyMethod[UTT_SEL_AUTHMODE_LEN];
    UINT32		    PMKCachePeriod;
    UINT32		    HT_HTC;
    UINT32		    HT_RDG;
    UINT32		    HT_MpduDensity;
    UINT32		    HT_EXTCHA;
    UINT32		    HT_AutoBA;
    UINT32		    HT_BAWinSize;
    UINT32		    HT_GI;
    UINT32		    HT_STBC;
    UINT32		    HT_MCS;
    UINT32		    HT_TxStream;
    UINT32		    HT_RxStream;
    UINT32		    HT_PROTECT;
    UINT32		    RADIUS_Acct_Port;
    UINT32		    HT_BSSCoexistence;
    UINT32		    FreqDelta;
    UINT32		    ITxBfEn;
    UINT32		    PreAntSwitch;
    UINT32		    PhyRateLimit;
    UINT32		    DebugFlags;
    UINT32		    ETxBfEnCond;
    UINT32		    ITxBfTimeout;
    UINT32		    ETxBfTimeout;
    UINT32		    ETxBfNoncompress ;
    UINT32		    ETxBfIncapable;
    UINT32		    FineAGC;
    UINT32		    StreamMode;
    UINT32		    DfsOutdoor;
    UINT32		    SymRoundFromCfg;
    UINT32		    BusyIdleFromCfg;
    UINT32		    DfsRssiHighFromCfg;
    UINT32		    DfsRssiLowFromCfg;
    UINT32		    DFSParamFromConfig;
    UINT32		    HT_DisallowTKIP;
    UINT32		    WCNTest;
    UINT32		    GreenAP;
    struct st_ap_cli_prof apClient;
    struct st_wds_prof apWds;
#if (WLAN_WORK_TIME == FYES)
#if (UTT_SMART_UI == FYES)
    FunEnableEnum WlanTimeEn;
#endif
    UINT32 day;
    char weekdayTimeStart[UTT_TIME_LEN];
    char weekdayTimeStop[UTT_TIME_LEN];
    char weekendTimeStart[UTT_TIME_LEN];
    char weekendTimeStop[UTT_TIME_LEN];
#endif
#if (UTT_SMART_UI == FYES || WLAN_MULTI_SSID == FYES)
    char encodeType[7];/*ssid���뷽ʽ*/
#if (WLAN_MULTI_SSID == FYES)
    char encodeType2[7];/*ssid���뷽ʽ,��Ӧssid2*/
#if (MIDDLE_SOFT == FYES)
    char encodeType3[7];/*ssid���뷽ʽ,��Ӧssid2*/
    char encodeType4[7];/*ssid���뷽ʽ,��Ӧssid2*/
#endif
#endif
#endif
    int                 roamthre;
}WirelessBaseConfigProfile;
#if (FEATURE_AP == FYES)
#if 0
/* �������� */
typedef enum wlan_Rate
{
    autoRate = 0,
    M11 = 11,
    M54 = 54,
    M150 = 150,
    M300 = 300
}WlanRate;

/* ���߹����ֶ�ģʽ */
typedef enum wlan_Power_manual
{
    LOW = 1,
    MIDDLE = 2,
    HIGH = 3
}WlanPowerManual;
#endif
typedef struct st_fit_ap_profile
{
#if 0
    ProfileEnableEnum cltIsolate;   /* ���߿ͻ��˸��� */
#endif
    WlanRate rate;  /* �������� */
    Uint32 autoPower;   /* ���ߴ��书�� */
    WlanPowerManual manPower;	/* ���ߴ��书�� �ֶ����� */
} FitApProfile;
#endif
/*
 * wireless profile
 * head.active		-   �������߹��ܿ���
 * mBaseCf		-   ���߻������ýṹ��
 * advance		-   ap advance structure
 * mSafeCf		-   ���߰�ȫ���ýṹ��
 *
 */
typedef struct st_wireless_profile 
{
    mibHead			    head;
#if (WLAN_WORK_TIME == FYES)
    int				    time_active;
#endif
    WirelessBaseConfigProfile	    mBaseCf;
#if (FEATURE_AP == FYES)
    FitApProfile		    fitAp;
#endif
    WirelessAPSafeConfigProfile	    mSafeCf;
    WirelessAdvanceProfile	    advance;
    WirelessMacFilterGlobalProfile  mFltGlb;
#if (WIRELESS_TURBO == FYES)
    int				    turbo;
#endif
    int                 profflag;
    int                 elinkc_flag;
} WirelessProfile;
#endif
