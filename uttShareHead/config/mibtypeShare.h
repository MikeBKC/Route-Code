
#ifndef _MIBTYPE_SHARE_H
#define _MIBTYPE_SHARE_H
#include "uttMachine.h"

typedef enum en_functionEnable {
    FUN_DISABLE,
    FUN_ENABLE
}FunEnableEnum;

#if (DHCP_AUTO_BIND == FYES)
/*
 *DHCP�󶨷�ʽ
 */
typedef enum en_dhcpBindType{
    MANUAL_BIND,
    AUTO_BIND
}DhcpBindType;
#endif

typedef enum en_profEnable {
    PROF_DISABLE,
    PROF_ENABLE
}ProfileEnableEnum;

/*
 * �Ʒ�ģʽ
 */
typedef enum en_profChargeTypeEnum
{
    NONE_CHARG_MODE = 0,
    DATE_CHARG_MODE
}ProfchargeTypeEnum;

/*
 * ͨ���������÷�ʽ
 */
typedef enum en_profWebSubmitEnum
{
    NOTICE_WEB_EDIT,
    NOTICE_WEB_IMPORT,
    NOTICE_WEB_NONE
}ProfWebSubmitEnum;

/*��Ϊ������Ƿ���*/
typedef enum en_logEnableEnum
{
    LOG_DISABLE,
    LOG_ENABLE
}LogEnableEnum;


/*
 * PPP������֤��ʽ
 */
typedef enum en_profPppAuthMode {
    PPP_AUTH_ANY,
    PPP_AUTH_THREE_ANY,
    PPP_AUTH_PAP,
    PPP_AUTH_CHAP,
    PPP_AUTH_MS_CHAP,
    PPP_AUTH_MS_CHAPV2,
    PPP_AUTH_NONE
}ProfPppAuthEnum;

/*
 * L2TP,PPTP�������Զ��û�����
 * VPN_MOBILE	-   �ƶ��û�
 * VPN_LANTOLAN	-   lan to lan �û�
 */
typedef enum en_profVPNPeerType {
    VPN_MOBILE,
    VPN_LANTOLAN
}ProfVPNPeerType;

/*
 * ap����ģʽ
 * ���� ap mode, ap client mode, repeater mode, bridge mode��lazy mode����ģʽ
 */
typedef enum en_apWorkMode {
    AP_MODE = 1,
    BRIDGE_MODE,
    REPEATER_MODE,
    LAZY_MODE,
    AP_CLIENT_MODE
} apWorkModeEn;

/*
 * ap mac��ַ���˹���
 * AP_MAC_ALLOW		-   ֻ������˵�mac��ַ
 * AP_MAC_DENY		-   ֻ��ֹ���˵�mac��ַ
 */
typedef enum en_apMacFilterAction {
    AP_MAC_ALLOW,
    AP_MAC_DENY
} apMacFilterActionEn;

/*
 * mac��ַ���˹���
 * MAC_ALLOW		-   ֻ������˵�mac��ַ
 * MAC_DENY		-   ֻ��ֹ���˵�mac��ַ
 */
typedef enum en_MacFilterAction {
    MAC_ALLOW,
    MAC_DENY
} MacFilterActionEn;

/*��ַ��������*/
typedef enum en_addrObjType{
    ADDR_IPRANGE = 0,
#if (IP_GRP == FYES)
    ADDR_IPGRP
#endif
} addrObjTypeEn;

/*�������������*/
typedef enum en_fwObjType{
    FW_DIY = 0,
#if (PORT_GRP == FYES)
    FW_GRP
#endif
} fwObjTypeEn;

/*dns��������*/
typedef enum en_dnsFilterType{
    DNSFILTER_DOMAIN_DIS = 0,/*��ֹ�б��е�������������*/
    DNSFILTER_DOMAIN_EN /*�����б��е�����������ֹ*/
} dnsFilterTypeEn;

#if ((FEATURE_SW_PORT_CONF == FYES) || (PORT_CONF_REDUCED == FYES))
typedef enum{
    PORT_MODE_10FDX = 0,/*10Mbit/s full duplex*/
    PORT_MODE_100FDX = 1,/*100Mbit/s full duplex*/
    PORT_MODE_1GFDX = 2,/*1000Mbit/s full duplex*/
    PORT_MODE_AUTO = 3,/*auto negotiate*/
    PORT_MODE_10HDX = 4,/*10Mbit/s half duplex*/
    PORT_MODE_100HDX = 5,/*100Mbit/s half duplex*/
    PORT_MODE_DISABLED =6/*link disabled*/
}SwPortConfMode;
#endif

/*
 * security mode
 * include WEP, WPA, WPA_PSK
 */
typedef enum uth_mode_t {
	NONE,
	WEP,
	WPA1_AND_WPA2,
	WPA1_PSK_AND_WPA2_PSK,
} uth_mode;

/*
 * WPA version
 * include AUTO, WPA, WPA2
 */
typedef enum en_wpaVersion {
	AUTO_WPA,
	WPA,
	WPA2,
} WpaVersion;

/*
 * Security Algorithm
 */
typedef enum en_securityAlgorithm {
	AUTO_SEC,
	TKIP,
	AES,
} SecurityAlg;

/*
 * Authentication method
 * include AUTO, OPEN_SYS, SHARE_KEY
 */
typedef enum en_authMethod {
	AUTO_AUTH,
	OPEN_SYS,
	SHARE_KEY,
} AuthMethod;

/*
 * Key type
 * include HEX, ASCII
 */
typedef enum en_keyType {
	HEX,
	ASCII,
} KeyType;

/*
 * key format
 * include FORBID, BIT_64, BIT_128
 */
typedef enum en_keyFormat {
	FORBID,
	BIT_64,
	BIT_128,
} KeyFormat;

/* �������� */
typedef enum wlan_Rate
{
    AUTO_RATE=0,
    M11,
    M54,
    M150,
    M300
}WlanRate;

/* ���߹����ֶ�ģʽ */
typedef enum wlan_Power_manual
{
    LOW=1,
    MIDDLE,
    HIGH
}WlanPowerManual;

/*��������*/
typedef enum limit_type
{
    FIXED_RATE_LIMIT,
    PPPOE_ACC_LIMIT
} LimitType;
#endif
