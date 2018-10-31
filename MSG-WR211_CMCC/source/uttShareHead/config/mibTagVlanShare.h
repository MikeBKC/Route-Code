#ifndef MIBTAGVLAN_SHARE_H
#define MIBTAGVLAN_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

#if (TAG_VLAN == FYES)

/*
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
*/

typedef struct st_tag_vlan_profile {
	mibHead head;
//	ProfileEnableEnum serviceEn;
//	char              ssid[UTT_SSID_LENTH + 1];
//	ProfileEnableEnum ssidBcast;
	Uint16            vid;
//	Uint16            client;
	IPADDR            gw;
	IPADDR            netmask;
//	IPADDR            virif;
	ProfileEnableEnum dhcpEn;
	char              dhcpPool[UTT_DHCP_POOL_LEN];
//	uth_mode          securMode;
//	auth_wep_t        wepSecurity;
//	auth_wpa_t        wpaSecurity;
//	auth_wpa_psk_t    wpaPskSecurity;
	Uint32		  mbrmsk;
	Uint32		  untagmsk;
	Uint8		  cc_vlan; /* 是否是可通信vlan, 1 - 是. 0 - 否 */
} TagVlanProfile;
#if (WAN_TAG_VLAN == FYES)
typedef struct st_wan_tag_vlan_profile {
	mibHead head;
	Uint16  vid;
	Uint8	tag; /* 1 - tag, 0 - untag */
} WanTagVlanProfile;
#endif

#endif
#endif
