#pragma once

#include "mibunionShare.h"
#include "mibtypeShare.h"

#if (SERVICE_ZONE == FYES)

#define MAX_VID_BITS_LEN  128
#define MAX_AP_VID_STR_LEN 32

typedef struct key_num {
	ProfileEnableEnum key_state_action;
	char              passwd[UTT_PAS_KEY_VALUE_LEN+1];
	KeyFormat         kFmt;
} KeyNum;

typedef struct wep_security {
	AuthMethod auMethod;
	KeyType    kType;
	KeyNum     kNum1;
	KeyNum     kNum2;
	KeyNum     kNum3;
	KeyNum     kNum4;
} auth_wep_t;

struct wep_security_web_get {
	char *auMethod;
	char *kType;
	char *keyCh;
	char *key1_passwd, *key1_kFmt;
	char *key2_passwd, *key2_kFmt;
	char *key3_passwd, *key3_kFmt;
	char *key4_passwd, *key4_kFmt;
};

typedef struct wpa_security {
	WpaVersion  wVersion;
	SecurityAlg sAlg;
	IPADDR      radiusIp;
	Uint16      radiusPort;
	char        radiusPasswd[UTT_RADIUS_KEY_LEN];
	Uint32      updataTime;
} auth_wpa_t;

struct wpa_security_web_get {
	char *wVersion;
	char *sAlg;
	char *radiusIp;
	char *radiusPort;
	char *radiusPasswd;
	char *updataTime;
};

typedef struct wpa_psk_security {
	WpaVersion  wVersion;
	SecurityAlg sAlg;
	char        sharePasswd[UTT_PSK_PASSWD_LEN];
	Uint32      updataTime;
} auth_wpa_psk_t;

struct wpa_psk_security_web_get {
	char *wVersion;
	char *sAlg;
	char *sharePasswd;
	char *updataTime;
};

typedef struct st_service_zone_profile {
	mibHead head;
	ProfileEnableEnum serviceEn;
	char              ssid[UTT_SSID_LENTH + 1];
	ProfileEnableEnum ssidBcast;
	ProfileEnableEnum ssidIsolate;
	Uint32	          apVidBits[MAX_VID_BITS_LEN];
	char              strApVid[MAX_AP_VID_STR_LEN];
	Uint16            vlanType; // 0为无线VLAN， 1为管理vlan
	Uint16            vid;
	Uint16            client;
	IPADDR            gw;
	IPADDR            netmask;
	IPADDR            virif;
	ProfileEnableEnum dhcpEn;
	char              dhcpPool[UTT_DHCP_POOL_LEN];
	uth_mode          securMode;
	auth_wep_t        wepSecurity;
	auth_wpa_t        wpaSecurity;
	auth_wpa_psk_t    wpaPskSecurity;
#if (SSID_ENCODE_TYPE == FYES)
        Uint32            ssid_encode_type;//0为gb2312，电脑优先，1为utf-8，手机优先
#endif
#if (SZ_SHARE_LIMIT_AC == FYES)
	Uint32 limit_type;	//0不限制. 1共享 2独享
	Uint32 limit_down;	//下行限制 kbit/s
	Uint32 limit_up;
#endif
} ServiceZoneProfile;

#endif
