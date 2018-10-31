#ifndef MIBFITAPSZ_SHARE_H
#define MIBFITAPSZ_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

typedef struct key_num_t {
	ProfileEnableEnum key_state_action;
	char              passwd[UTT_PAS_KEY_VALUE_LEN+1];
	KeyFormat         kFmt;
} KeyNum_t;

typedef struct wep_security_t {
	AuthMethod auMethod;
	KeyType    kType;
	KeyNum_t   kNum1;
	KeyNum_t   kNum2;
	KeyNum_t   kNum3;
	KeyNum_t   kNum4;
} auth_wep;

typedef struct wpa_security_t {
	WpaVersion  wVersion;
	SecurityAlg sAlg;
	Uint32      radiusIp;
	Uint32      radiusPort;
	char        radiusPasswd[UTT_RADIUS_KEY_LEN];
	Uint32      updataTime;
} auth_wpa;

typedef struct wpa_psk_security_t {
	WpaVersion  wVersion;
	SecurityAlg sAlg;
	char        sharePasswd[UTT_RADIUS_KEY_LEN];
	Uint32      updataTime;
} auth_wpa_psk;

typedef struct st_fit_ap_sz_profile {
	mibHead head;
	//ProfileEnableEnum serviceEn;
	char              ssid[UTT_SSID_LENTH + 1];
	ProfileEnableEnum ssidBcast;
	ProfileEnableEnum ssidIsolate;
	Uint16            vid;
	Uint16            client;
//	Uint16            dbm;
	uth_mode          securMode;
	auth_wep        wepSecurity;
	auth_wpa        wpaSecurity;
	auth_wpa_psk    wpaPskSecurity;
#if (SZ_SHARE_LIMIT_AP == FYES)
	Uint32 limit_type;
	Uint32 limit_down;
	Uint32 limit_up;
#endif
} FitApSzProfile
#if defined(WIRELESS_5G) && (WIRELESS_5G==FYES)
, FitAp5gSzProfile
#endif
;
#endif
