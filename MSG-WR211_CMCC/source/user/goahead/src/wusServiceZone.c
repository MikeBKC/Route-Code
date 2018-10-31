
#include <string.h>
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "mib.h"
#include "profacce.h"
#include "uttfunction.h"
#include "wuScript.h"
#include "translate.h"

#if (SERVICE_ZONE == FYES)

#define DEBUG_SZ 0

#if DEBUG_SZ
#define debug() printf("debug: %d in %s()\n", __LINE__, __func__)
#else
#define debug()
#endif

/* 判断某个vid是否在ap的vid bit位中，不在返回0，在返回-1*/
int judgeVidBits(unsigned int vid, unsigned int *ap_vid_bits)
{
	unsigned int i, j;
	int ret = 0;

	i = vid >> 5;
	j = vid & 0x1fu;
	j = 1u << j;

	if ((ap_vid_bits[i] & j) != 0) {
		ret = -1;
	}

	return ret;
}

/* 判断两个ap的vid bit位是否重复，不重复返回0，重复返回-1*/
int judgeApBits(unsigned int *vid_bits, unsigned int *ap_vid_bits, int len)
{
	int ret = 0;
	int i;

	for (i = 0; i < len; i++) {
		if (vid_bits[i] != 0) {
			if ((vid_bits[i] & ap_vid_bits[i]) != 0) {
				ret = -1;
				break;
			}
		}
	}

	return ret;
}

/* end 值一定要大于等于start，由调用函数保证*/
int set_vid_bits(unsigned int vid_bits[128], unsigned int start, unsigned int end)
{
	int i, j, k, m, n;

	i = start >> 5;
	j = end >> 5;

	if (i == j) {
		m = start & 0x1fu;
		n = end & 0x1fu;

		m = (1u << m) - 1u;
		if (n == 31) {
			n = 0xffffffff;
		} else {
			n = (1u << (n + 1)) - 1u;
		}

		n &= ~m;
		vid_bits[i] |= n;
	} else {
		m = start & 0x1fu;
		n = end & 0x1fu;

		m = (1u << m) - 1u;
		if (n == 31) {
			n = 0xffffffff;
		} else {
			n = (1u << (n + 1)) - 1u;
		}

		vid_bits[i] |= ~m;
		vid_bits[j] |= n;

		for (k = i + 1; k < j; k++) {
			vid_bits[k] = 0xffffffffu;
		}
	}

	return 0;
}

int numInStr(char a, char *str)
{
	int i, n = 0, num = 0;

	n = strlen(str);

	for (i = 0; i < n; i++) {
		if (a == str[i]) {
			num++;
		}
	}

	return num;
}

/* 解析页面传下来的VLANID字符串，返回0代表正常，返回-1代表字符串错误 */
int parseVidStr(unsigned int vid_bits[128] , char str[128])
{
	int ret = 0;
	char *tmp = NULL, *ptr = NULL;
	char a;
	int n = 0;
	char str1[128];
	unsigned int vid, startVid, endVid;

	memcpy(str1, str, 128);

	tmp = strtok_r(str1, ",", &ptr);
	while (tmp != NULL) {
		n = numInStr('-', tmp);

		if (n == 0) {
			vid = strtol(tmp, NULL, 10);
			set_vid_bits(vid_bits, vid, vid);

		} else if (n == 1) {
			n = sscanf(tmp, "%u-%u", &startVid, &endVid);
			if (n == 2) {
				if ((startVid > 4094) || (endVid > 4094) || (startVid > endVid)) {
					printf("%d in %s, error: %s\n", __LINE__, __func__, tmp);
					ret = -1;
				} else {
					set_vid_bits(vid_bits, startVid, endVid);
				}
			} else {
				printf("%d in %s, error: %s\n", __LINE__, __func__, tmp);
				ret = -1;
			}

		} else {
			printf("%d in %s, error: %s\n", __LINE__, __func__, tmp);
			ret = -1;
		}

		tmp = strtok_r(NULL, ",", &ptr);
	}


	return ret;
}

static void formServiceZoneConfig(webs_t wp, char_t *path, char_t *query)
{
	char *tmp, *newName, *oldName, *serviceEn, *ssid, *vif, *ssidBcast, *ssidIsolate,
		 *vid, *maxClt, *gw, *netmask, *dhcpEn, *dhcpPool, *securMode,*ssidEncodeType;
	char *vlanType, *apVlan;
	Uint32 apVidBits[128];
	int vlanT;
	int ret;
	struct wep_security_web_get wepSecurityWeb;
	struct wpa_security_web_get	wpaSecurityWeb;
	struct wpa_psk_security_web_get wpaPskSecurityWeb;
	int min, max, index, indexTmp;
	Uint32 vidVal = 0, maxCltVal, radiusPortVal = 0, wpaupVal = 0, wpaPskUpVal,encodeTypeVal;
	struct in_addr addrVif, addrGw, addrNm, addrRadius;
	struProfAlloc *profhead = NULL;
	struProfAlloc *profheadDhcp = NULL;
	MibProfileType mibType = MIB_PROF_SERVICE_ZONE;
#if(DHCP_POOL == FYES)
	MibProfileType mibTypeDhcp = MIB_PROF_DHCP_POOL;
	DhcpPoolProfile *profDhcp = NULL;
#endif
	ProfChangeType profType;
	ServiceZoneProfile *prof = NULL;
	const char *ErrMsg = NULL;
#if (SZ_SHARE_LIMIT_AC == FYES)
	char *limit_type, *limit_down, *limit_up;
#endif
#if(FEATURE_AP_MAC_FILTER == FYES)
    ApMacFilterProfile *mfProf=NULL;
    int mf_min,mf_max,mf_index,mf_i=0;
    ProfInstLowHigh(MIB_PROF_AP_MAC_FILTER, &mf_max, &mf_min);
#endif

#if (SZ_SHARE_LIMIT_AC == FYES)
	limit_type = websGetVar(wp, "share_select", T(""));
	limit_down = websGetVar(wp, "rxBand", T(""));
	limit_up = websGetVar(wp, "txBand", T(""));
#endif
	newName = websGetVar(wp, "zoneName", T(""));
	oldName = websGetVar(wp, "oldName", T(""));
	serviceEn = websGetVar(wp, "szEn", T(""));
//	vif = websGetVar(wp, "ifAddr", T(""));
	ssid = websGetVar(wp, "ssid", T(""));
	ssidEncodeType = websGetVar(wp, "encodeType", T(""));
	ssidBcast = websGetVar(wp, "broadcastEn", T(""));
	ssidIsolate = websGetVar(wp, "isolateEn", T(""));
	vid = websGetVar(wp, "vlanId", T(""));
	maxClt = websGetVar(wp, "max_user", T(""));
	gw = websGetVar(wp, "ipAddr", T(""));
//	netmask = websGetVar(wp, "maskAddr", T(""));
	dhcpEn = websGetVar(wp, "dhcpServer", T(""));
	dhcpPool = websGetVar(wp, "pool", T("default"));
	securMode = websGetVar(wp, "encryType", T(""));
	vlanType = websGetVar(wp, "vlanType", T(""));
	apVlan = websGetVar(wp, "apvlan", T(""));

	if('\0' == ssidEncodeType)
	{
        encodeTypeVal = 0;
	}else{
        encodeTypeVal =(Uint32)strtoul(ssidEncodeType, NULL, 10);
	}

#if(SSID_ENCODE_TYPE == FYES)
	if((encodeTypeVal == 1) && (ssid!=NULL)) /*手机优先*/
	{
	    tmp = iconv_string(ssid,"GBK","UTF-8");
	    if(strlen(tmp)>UTT_SSID_LENTH){
               setTheErrorMsg(T("SSID名称过长!"));
	       goto out;
	    }
	}
#endif

	vlanT = (Uint16)strtoul(vlanType, NULL, 10);
	memset(apVidBits, 0, sizeof(apVidBits));

	if (strlen(newName) > (UTT_INST_NAME_LEN - 1)) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_SZ_NAME_OUT_OF_RANGE);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("服务区名称不能超过11个字节"));
		goto out;
	}
	if (0 == inet_aton(gw, &addrGw)) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_GATEWAY);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("无效网关"));
		goto out;
	}
	wepSecurityWeb.auMethod = websGetVar(wp, "wepAuthType", T(""));
	wepSecurityWeb.kType = websGetVar(wp, "wepFormat", T(""));
	wepSecurityWeb.keyCh = websGetVar(wp, "wepRadio", T(""));
	wepSecurityWeb.key1_passwd = websGetVar(wp, "wepKey1Text", T(""));
	wepSecurityWeb.key1_kFmt = websGetVar(wp, "wepKey1Type", T(""));
	wepSecurityWeb.key2_passwd = websGetVar(wp, "wepKey2Text", T(""));
	wepSecurityWeb.key2_kFmt = websGetVar(wp, "wepKey2Type", T(""));
	wepSecurityWeb.key3_passwd = websGetVar(wp, "wepKey3Text", T(""));
	wepSecurityWeb.key3_kFmt = websGetVar(wp, "wepKey3Type", T(""));
	wepSecurityWeb.key4_passwd = websGetVar(wp, "wepKey4Text", T(""));
	wepSecurityWeb.key4_kFmt = websGetVar(wp, "wepKey4Type", T(""));

	wpaSecurityWeb.wVersion = websGetVar(wp, "wpaVersion", T(""));
	wpaSecurityWeb.sAlg = websGetVar(wp, "wpaEncrp", T(""));
	wpaSecurityWeb.radiusIp = websGetVar(wp, "radiusIP", T(""));
	wpaSecurityWeb.radiusPort = websGetVar(wp, "radiusPort", T(""));
	wpaSecurityWeb.radiusPasswd = websGetVar(wp, "radiusPsswd", T(""));
	wpaSecurityWeb.updataTime = websGetVar(wp, "wpaTime", T(""));

	wpaPskSecurityWeb.wVersion = websGetVar(wp, "pskVersion", T(""));
	wpaPskSecurityWeb.sAlg = websGetVar(wp, "pskEncry", T(""));
	wpaPskSecurityWeb.sharePasswd = websGetVar(wp, "pskPsswd", T(""));
	wpaPskSecurityWeb.updataTime = websGetVar(wp, "pskTime", T(""));
	wpaPskUpVal = (Uint32)strtoul(wpaPskSecurityWeb.updataTime, NULL, 10);

	if (!strcmp(securMode, "1")) {
/* WEP的密码检查交由页面来做控制 */
#if 0
		if (strlen(wepSecurityWeb.key1_passwd) > (UTT_PASSWD_LEN - 1) ||
			strlen(wepSecurityWeb.key2_passwd) > (UTT_PASSWD_LEN - 1) ||
			strlen(wepSecurityWeb.key3_passwd) > (UTT_PASSWD_LEN - 1) ||
			strlen(wepSecurityWeb.key4_passwd) > (UTT_PASSWD_LEN - 1)) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_PASSWORD_OUT_OF_RANGE);
			setTheErrorMsg((char *)ErrMsg);
			//setTheErrorMsg(T("密码长度不能超过11个字节"));
			goto out;
		}
#endif
	} else if (!strcmp(securMode, "2")) {
		radiusPortVal = (Uint16)strtoul(wpaSecurityWeb.radiusPort, NULL, 10);
		if (radiusPortVal > 65536 || radiusPortVal < 0) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_SERVICE_PORT_OUT_OF_RANGE);
			setTheErrorMsg((char *)ErrMsg);
			//setTheErrorMsg(T("端口应在0~65536之间"));
			goto out;
		}
		if (0 == inet_aton(wpaSecurityWeb.radiusIp, &addrRadius)) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_RADIUS_IP);
			setTheErrorMsg((char *)ErrMsg);
			//setTheErrorMsg(T("无效Radius服务器IP"));
			goto out;
		}
		if (strlen(wpaSecurityWeb.radiusPasswd) > 31) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_RADIUS_PASSWORD_OUT_OF_RANGE);
			setTheErrorMsg((char *)ErrMsg);
			//setTheErrorMsg(T("Radius密码长度不能超过31个字节"));
			goto out;
		}
		wpaupVal = (Uint32)strtoul(wpaSecurityWeb.updataTime, NULL, 10);
		if (wpaupVal > 86400 || (wpaupVal < 60 && wpaupVal != 0)) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_PASSWORD_PERIOD_OUT_OF_RANGE);
			setTheErrorMsg((char *)ErrMsg);
			//setTheErrorMsg(T("密码更新周期超出取值范围"));
			goto out;
		}
	} else if (!strcmp(securMode, "3")) {
		if (wpaPskUpVal> 86400 || (wpaPskUpVal< 60 && wpaPskUpVal!= 0)) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_PASSWORD_PERIOD_OUT_OF_RANGE);
			setTheErrorMsg((char *)ErrMsg);
			//setTheErrorMsg(T("密码更新周期超出取值范围"));
			goto out;
		}
	} else {
	}

	ProfInstLowHigh(mibType, &max, &min);

	// 无线vlan
	if (vlanT == 0) {
		if ('\0' == *vid) {
			vidVal = 0;
		} else {
			vidVal = (Uint16)strtoul(vid, NULL, 10);
		}
#if 0
		if (vidVal > 4094 || vidVal < 0 || ((0 == vidVal) && (strcmp(newName, "default")))) {
#else
		if (vidVal > 4094 || vidVal < 0 ) {
#endif
			ErrMsg = getTransDataForC(C_LANG_INDEX_VLAN_ID_OUT_OF_RANGE);
			setTheErrorMsg((char *)ErrMsg);
			//setTheErrorMsg(T("VLAN ID超出取值范围"));
			goto out;
		}

#if 0
		if (0 == inet_aton(netmask, &addrNm)) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_SUBMASK);
			setTheErrorMsg((char *)ErrMsg);
			//setTheErrorMsg(T("无效子网掩码"));
			goto out;
		}
#endif

#if 0
		if ( strcmp(oldName,"default") ){
			if (0 == inet_aton(vif, &addrVif)) {
				setTheErrorMsg(T("无效接口地址"));
				goto out;
			}
			if ((0 == strcmp(dhcpEn, "1"))) {
				if ( (ntohl(addrGw.s_addr) & ntohl(addrNm.s_addr)) != (ntohl(addrVif.s_addr) & ntohl(addrNm.s_addr)) ){
					setTheErrorMsg(T("接口地址跟网关地址不在同一网段！"));
					goto out;
				}
			}
		}
#endif

		// AP vlan
	} else if (vlanT == 1) {
		vidVal = 0;
		vif = 0;
		ret = parseVidStr(apVidBits , apVlan);

		if (ret != 0) {
			ErrMsg = getTransDataForC("AP管理vlan值错误");
			setTheErrorMsg((char *)ErrMsg);
		}
	}

	if ('\0' == *oldName) {
	/* add new an option*/
		debug();
		for (index = min; index < max; index++) {
			prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, index);
			if (NULL != prof) {
				if (!strcmp(newName, prof->head.name)) {
					ErrMsg = getTransDataForC(C_LANG_INDEX_SZ_NAME_CONFLICT);
					setTheErrorMsg((char *)ErrMsg);
					//setTheErrorMsg("操作失败，服务区名称冲突");
					goto out;
				}
				if (vlanT == 0) {
					if ((vidVal != 0) && (prof->vlanType == 0) && (vidVal == prof->vid)) {
						ErrMsg = getTransDataForC(C_LANG_INDEX_VLAN_ID_CONFLICT);
						setTheErrorMsg((char *)ErrMsg);
						//setTheErrorMsg("操作失败，VLAN ID冲突");
						goto out;
					}
				} else {
					if (prof->vlanType == 1) {
						ret = judgeApBits(apVidBits, prof->apVidBits, MAX_VID_BITS_LEN);
						if (ret == -1) {
							ErrMsg = getTransDataForC(C_LANG_INDEX_VLAN_ID_CONFLICT);
							setTheErrorMsg((char *)ErrMsg);
							//setTheErrorMsg("操作失败，VLAN ID冲突");
							goto out;
						}
					}

				}
			}
		}

#if 0
		if ((0 == strcmp(serviceEn, "1")) && (0 == strcmp(dhcpEn, "1"))) {
		debug();
			profDhcp = (DhcpPoolProfile *)ProfGetInstPointByName(mibTypeDhcp, dhcpPool);
			if (NULL != profDhcp) {
				if (PROF_DISABLE == profDhcp->dhcpEn) {
					ProfBackupByName(mibTypeDhcp, PROF_CHANGE_EDIT, dhcpPool, &profheadDhcp);
					profDhcp->dhcpEn = PROF_ENABLE;
					sprintf(profDhcp->interface, "%s.%d", (char *)getLanIfName(), vidVal);
				} else {
					ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_POOL_USED);
					setTheErrorMsg((char *)ErrMsg);
					//setTheErrorMsg(T("操作失败,地址池已被使用"));
					goto out;
				}
			}
		}
#endif

		index = ProfNewInst(mibType, newName, FALSE);
		profType = PROF_CHANGE_ADD;
	} else if (0 == strcmp("default", oldName)){
		debug();

		for (index = min + 1; index < max; index++) {
			prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, index);
			if (prof->vlanType == 1) {
				ret = judgeApBits(apVidBits, prof->apVidBits, MAX_VID_BITS_LEN);
				if (ret == -1) {
					ErrMsg = getTransDataForC(C_LANG_INDEX_VLAN_ID_CONFLICT);
					setTheErrorMsg((char *)ErrMsg);
					//setTheErrorMsg("操作失败，VLAN ID冲突");
					goto out;
				}
			}
		}

		prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, 0);
		if (NULL == prof) {
			debug();
			goto out;
		}

#if(FEATURE_AP_MAC_FILTER == FYES)
        for(mf_index = mf_min; mf_index < mf_max; mf_index++)
        {
           mfProf = (ApMacFilterProfile*)ProfGetInstPointByIndex(MIB_PROF_AP_MAC_FILTER,mf_index);
           mf_i = 0;
           if((mfProf != NULL) && (ProfInstIsFree(mfProf) == 0))
           {
               while(mfProf->sz[mf_i][0] != '\0')
               {
                   if(strcmp(prof->head.name,mfProf->sz[mf_i]) == 0)
                   {
                        setTheErrorMsg("有MAC地址过滤引用该服务区，请先删除MAC地址过滤，再修改该服务区！");
                        goto out;
                   }
                   mf_i++;
               }
           }
        }
#endif
#if 0
		if ((PROF_ENABLE == prof->serviceEn) && (0 == strcmp(serviceEn, "1"))) {
			profDhcp = (DhcpPoolProfile *)ProfGetInstPointByName(mibTypeDhcp, "default");
			if (NULL != profDhcp) {
				ProfBackupByName(mibTypeDhcp, PROF_CHANGE_EDIT, "default", &profheadDhcp);
				if (0 == strcmp(dhcpEn, "1")) {
					profDhcp->dhcpEn = PROF_ENABLE;
				} else {
					profDhcp->dhcpEn = PROF_DISABLE;
				}
			}
		} else if ((PROF_ENABLE == prof->serviceEn) && (0 != strcmp(serviceEn, "1"))) {
			debug();
		} else if ((PROF_DISABLE == prof->serviceEn) && (0 == strcmp(serviceEn, "1"))) {
			profDhcp = (DhcpPoolProfile *)ProfGetInstPointByName(mibTypeDhcp, "default");
			if (NULL != profDhcp) {
				ProfBackupByName(mibTypeDhcp, PROF_CHANGE_EDIT, "default", &profheadDhcp);
				if (0 == strcmp(dhcpEn, "1")) {
					profDhcp->dhcpEn = PROF_ENABLE;
				} else {
					profDhcp->dhcpEn = PROF_DISABLE;
				}
			}
		} else if ((PROF_DISABLE == prof->serviceEn) && (0 != strcmp(serviceEn, "1"))) {
			debug();
		} else {
			debug();
		}
#endif

		strcpy(newName, "default");
		index = 0;
		profType = PROF_CHANGE_EDIT;
	} else {
		/* modify an exist option */
		debug();
		indexTmp = ProfGetInstIndexByName(mibType, oldName);
		if (indexTmp == PROFFAIL) {
			goto out;
		}

		for (index = min; index < max; index++) {
			if (index != indexTmp) {
				prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, index);
				if (NULL != prof) {
					if (!strcmp(newName, prof->head.name)) {
						ErrMsg = getTransDataForC(C_LANG_INDEX_SZ_NAME_CONFLICT);
						setTheErrorMsg((char *)ErrMsg);
						//setTheErrorMsg("操作失败，服务区名称冲突");
						goto out;
					}
					if (vlanT == 0) {
						if ((vidVal != 0) && (prof->vlanType == 0) && (vidVal == prof->vid)) {
							ErrMsg = getTransDataForC(C_LANG_INDEX_VLAN_ID_CONFLICT);
							setTheErrorMsg((char *)ErrMsg);
							//setTheErrorMsg("操作失败，VLAN ID冲突");
							goto out;
						}
					} else {
						if (prof->vlanType == 1) {
							ret = judgeApBits(apVidBits, prof->apVidBits, MAX_VID_BITS_LEN);
							if (ret == -1) {
								ErrMsg = getTransDataForC(C_LANG_INDEX_VLAN_ID_CONFLICT);
								setTheErrorMsg((char *)ErrMsg);
								//setTheErrorMsg("操作失败，VLAN ID冲突");
								goto out;
							}
						}

					}
				}
			}
		}

		prof = (ServiceZoneProfile *)ProfGetInstPointByName(mibType, oldName);
		if (NULL == prof) {
            debug();
            goto out;
        }
#if(FEATURE_AP_MAC_FILTER == FYES)
        for(mf_index = mf_min; mf_index < mf_max; mf_index++)
        {
           mfProf = (ApMacFilterProfile*)ProfGetInstPointByIndex(MIB_PROF_AP_MAC_FILTER,mf_index);
           mf_i = 0;
           if((mfProf != NULL) && (ProfInstIsFree(mfProf) == 0))
           {
               while(mfProf->sz[mf_i][0] != '\0')
               {
                   if(strcmp(prof->head.name,mfProf->sz[mf_i]) == 0)
                   {
                        setTheErrorMsg("有MAC地址过滤引用该服务区，请先删除MAC地址过滤，再修改该服务区！");
                        goto out;
                   }
                   mf_i++;
               }
           }
        }
#endif
        if((0 != strcmp(serviceEn, "1")) ){
            UserProfile *profUser;
            ProfInstLowHigh(MIB_PROF_USER, &max, &min);
            for(index = min; index < max; index++) {
                profUser = (UserProfile*)ProfGetInstPointByIndex(MIB_PROF_USER, index);
                if(profUser != NULL && ProfInstIsFree(profUser) == 0) {
                    if ( (!strcmp(prof->dhcpPool, profUser->dhcpPoolName))  && ('\0' != *(profUser->dhcpPoolName))){
                        setTheErrorMsg("有DHCP静态绑定在该服务区，请先删除该绑定，再关闭该服务区！");
                        goto out;
                    }
                }
            }
        }

        if ( (PROF_ENABLE == prof->serviceEn)  && ( strcmp(prof->dhcpPool,dhcpPool)) ){
            UserProfile *profUser;
            ProfInstLowHigh(MIB_PROF_USER, &max, &min);
            for(index = min; index < max; index++) {
                profUser = (UserProfile*)ProfGetInstPointByIndex(MIB_PROF_USER, index);
                if(profUser != NULL && ProfInstIsFree(profUser) == 0) {
                    if ( (!strcmp(prof->dhcpPool, profUser->dhcpPoolName))  && ('\0' != *(profUser->dhcpPoolName))){
                        setTheErrorMsg("有DHCP静态绑定在该地址池，请先删除该绑定，再更换地址池！");
                        goto out;
                    }
                }
            }
        }

#if 0
        if ((PROF_ENABLE == prof->serviceEn) && (0 == strcmp(serviceEn, "1"))) {
            if ((prof->dhcpEn == PROF_ENABLE) && (0 == strcmp(dhcpEn, "1"))) {
                debug();
                profDhcp = (DhcpPoolProfile *)ProfGetInstPointByName(mibTypeDhcp, prof->dhcpPool);
                if (NULL != profDhcp) {
                    ProfBackupByName(mibTypeDhcp, PROF_CHANGE_EDIT, prof->dhcpPool, &profheadDhcp);
                    profDhcp->dhcpEn = PROF_DISABLE;
                    memset(profDhcp->interface, '\0', sizeof(profDhcp->interface));
				}

				profDhcp = (DhcpPoolProfile *)ProfGetInstPointByName(mibTypeDhcp, dhcpPool);
				if (NULL != profDhcp) {
					if (PROF_DISABLE == profDhcp->dhcpEn) {
						ProfBackupByName(mibTypeDhcp, PROF_CHANGE_EDIT, dhcpPool, &profheadDhcp);
						profDhcp->dhcpEn = PROF_ENABLE;
						sprintf(profDhcp->interface, "%s.%d", (char *)getLanIfName(), vidVal);
					} else {
						ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_POOL_USED);
						setTheErrorMsg((char *)ErrMsg);
						//setTheErrorMsg(T("操作失败,地址池已被使用"));
						goto out;
					}
				}
			} else if ((prof->dhcpEn == PROF_ENABLE) && (0 != strcmp(dhcpEn, "1"))) {
				debug();
				profDhcp = (DhcpPoolProfile *)ProfGetInstPointByName(mibTypeDhcp, prof->dhcpPool);
				if (NULL != profDhcp) {
					ProfBackupByName(mibTypeDhcp, PROF_CHANGE_EDIT, prof->dhcpPool, &profheadDhcp);
					profDhcp->dhcpEn = PROF_DISABLE;
					memset(profDhcp->interface, '\0', sizeof(profDhcp->interface));
				}
			} else if ((prof->dhcpEn == PROF_DISABLE) && (0 == strcmp(dhcpEn, "1"))) {
				debug();
				profDhcp = (DhcpPoolProfile *)ProfGetInstPointByName(mibTypeDhcp, dhcpPool);
				if (NULL != profDhcp) {
					if (PROF_DISABLE == profDhcp->dhcpEn) {
						ProfBackupByName(mibTypeDhcp, PROF_CHANGE_EDIT, dhcpPool, &profheadDhcp);
						profDhcp->dhcpEn = PROF_ENABLE;
						sprintf(profDhcp->interface, "%s.%d", (char *)getLanIfName(), vidVal);
					} else {
						ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_POOL_USED);
						setTheErrorMsg((char *)ErrMsg);
						//setTheErrorMsg(T("操作失败,地址池已被使用"));
						goto out;
					}
				}
			} else if ((prof->dhcpEn == PROF_DISABLE) && (0 != strcmp(dhcpEn, "1"))) {
				debug();
			} else {
				debug();
			}
		} else if ((PROF_ENABLE == prof->serviceEn) && (0 != strcmp(serviceEn, "1"))) {
			debug();
			profDhcp = (DhcpPoolProfile *)ProfGetInstPointByName(mibTypeDhcp, prof->dhcpPool);
			if (NULL != profDhcp) {
				ProfBackupByName(mibTypeDhcp, PROF_CHANGE_EDIT, prof->dhcpPool, &profheadDhcp);
				profDhcp->dhcpEn = PROF_DISABLE;
				memset(profDhcp->interface, '\0', sizeof(profDhcp->interface));
			}
		} else if ((PROF_DISABLE == prof->serviceEn) && (0 == strcmp(serviceEn, "1"))) {
			debug();
			if (0 == strcmp(dhcpEn, "1")) {
				profDhcp = (DhcpPoolProfile *)ProfGetInstPointByName(mibTypeDhcp, dhcpPool);
				if (NULL != profDhcp) {
					if (PROF_DISABLE == profDhcp->dhcpEn) {
						ProfBackupByName(mibTypeDhcp, PROF_CHANGE_EDIT, dhcpPool, &profheadDhcp);
						profDhcp->dhcpEn = PROF_ENABLE;
						sprintf(profDhcp->interface, "%s.%d", (char *)getLanIfName(), vidVal);
					} else {
						ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_POOL_USED);
						setTheErrorMsg((char *)ErrMsg);
						//setTheErrorMsg(T("操作失败,地址池已被使用"));
						goto out;
					}
				}
			}
		} else if ((PROF_DISABLE == prof->serviceEn) && (0 != strcmp(serviceEn, "1"))) {
			debug();
		} else {
			debug();
		}
#endif

		index = indexTmp;
		profType = PROF_CHANGE_EDIT;
	}

	prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, index);
	if (NULL == prof) {
		goto out;
	}
	ProfBackupByIndex(mibType, profType, index, &profhead);

	strcpy(prof->head.name, newName);
	prof->head.active = TRUE;
	if (0 == strcmp(serviceEn, "1")) {
		prof->serviceEn = PROF_ENABLE;
	} else {
		prof->serviceEn = PROF_DISABLE;
	}
	strcpy(prof->ssid, ssid);
#if(SSID_ENCODE_TYPE == FYES)
    prof->ssid_encode_type = encodeTypeVal;
#endif
	if (0 == strcmp(ssidBcast, "1")) {
		prof->ssidBcast = PROF_ENABLE;
	} else {
		prof->ssidBcast = PROF_DISABLE;
	}
	if (0 == strcmp(ssidIsolate, "1")) {
		prof->ssidIsolate = PROF_ENABLE;
	} else {
		prof->ssidIsolate = PROF_DISABLE;
	}
	prof->vlanType = vlanT;
	prof->vid = vidVal;
	memcpy(prof->apVidBits, apVidBits, sizeof(apVidBits));
	if (vlanT == 0) {
		memset(prof->strApVid, 0, MAX_AP_VID_STR_LEN);
#if 0
		prof->virif = addrVif.s_addr;
		prof->netmask = addrNm.s_addr;
#endif
	} else {
		memcpy(prof->strApVid, apVlan, MAX_AP_VID_STR_LEN);
		prof->virif = 0u;
		if (index != 0) {
			prof->netmask = 0u;
		}
	}
	maxCltVal = (Uint16)strtoul(maxClt, NULL, 10);
	prof->client = maxCltVal;
	prof->gw = addrGw.s_addr;
	if (0 == strcmp(securMode, "1")) {
		prof->securMode = WEP;

		if (0 == strcmp(wepSecurityWeb.auMethod, "0")) {
			prof->wepSecurity.auMethod = AUTO_AUTH;
		} else if (0 == strcmp(wepSecurityWeb.auMethod, "1")) {
			prof->wepSecurity.auMethod = OPEN_SYS;
		} else {
			prof->wepSecurity.auMethod = SHARE_KEY;
		}

		if (0 == strcmp(wepSecurityWeb.kType, "1")) {
			prof->wepSecurity.kType = HEX;
		} else {
			prof->wepSecurity.kType = ASCII;
		}

		if (0 == strcmp(wepSecurityWeb.keyCh, "3")) {
			prof->wepSecurity.kNum1.key_state_action = PROF_DISABLE;
			prof->wepSecurity.kNum2.key_state_action = PROF_DISABLE;
			prof->wepSecurity.kNum3.key_state_action = PROF_DISABLE;
			prof->wepSecurity.kNum4.key_state_action = PROF_ENABLE;
		} else if (0 == strcmp(wepSecurityWeb.keyCh, "2")) {
			prof->wepSecurity.kNum1.key_state_action = PROF_DISABLE;
			prof->wepSecurity.kNum2.key_state_action = PROF_DISABLE;
			prof->wepSecurity.kNum3.key_state_action = PROF_ENABLE;
			prof->wepSecurity.kNum4.key_state_action = PROF_DISABLE;
		} else if (0 == strcmp(wepSecurityWeb.keyCh, "1")) {
			prof->wepSecurity.kNum1.key_state_action = PROF_DISABLE;
			prof->wepSecurity.kNum2.key_state_action = PROF_ENABLE;
			prof->wepSecurity.kNum3.key_state_action = PROF_DISABLE;
			prof->wepSecurity.kNum4.key_state_action = PROF_DISABLE;
		} else {
			prof->wepSecurity.kNum1.key_state_action = PROF_ENABLE;
			prof->wepSecurity.kNum2.key_state_action = PROF_DISABLE;
			prof->wepSecurity.kNum3.key_state_action = PROF_DISABLE;
			prof->wepSecurity.kNum4.key_state_action = PROF_DISABLE;
		}

		strcpy(prof->wepSecurity.kNum1.passwd, wepSecurityWeb.key1_passwd);
		if (0 == strcmp(wepSecurityWeb.key1_kFmt, "0")) {
			prof->wepSecurity.kNum1.kFmt = FORBID;
		} else if (0 == strcmp(wepSecurityWeb.key1_kFmt, "1")) {
			prof->wepSecurity.kNum1.kFmt = BIT_64;
		} else {
			prof->wepSecurity.kNum1.kFmt = BIT_128;
		}
		strcpy(prof->wepSecurity.kNum2.passwd, wepSecurityWeb.key2_passwd);
		if (0 == strcmp(wepSecurityWeb.key2_kFmt, "0")) {
			prof->wepSecurity.kNum2.kFmt = FORBID;
		} else if (0 == strcmp(wepSecurityWeb.key2_kFmt, "1")) {
			prof->wepSecurity.kNum2.kFmt = BIT_64;
		} else {
			prof->wepSecurity.kNum2.kFmt = BIT_128;
		}
		strcpy(prof->wepSecurity.kNum3.passwd, wepSecurityWeb.key3_passwd);
		if (0 == strcmp(wepSecurityWeb.key3_kFmt, "0")) {
			prof->wepSecurity.kNum3.kFmt = FORBID;
		} else if (0 == strcmp(wepSecurityWeb.key3_kFmt, "1")) {
			prof->wepSecurity.kNum3.kFmt = BIT_64;
		} else {
			prof->wepSecurity.kNum3.kFmt = BIT_128;
		}
		strcpy(prof->wepSecurity.kNum4.passwd, wepSecurityWeb.key4_passwd);
		if (0 == strcmp(wepSecurityWeb.key4_kFmt, "0")) {
			prof->wepSecurity.kNum4.kFmt = FORBID;
		} else if (0 == strcmp(wepSecurityWeb.key4_kFmt, "1")) {
			prof->wepSecurity.kNum4.kFmt = BIT_64;
		} else {
			prof->wepSecurity.kNum4.kFmt = BIT_128;
		}
	} else if (0 == strcmp(securMode, "2")) {
		prof->securMode = WPA1_AND_WPA2;

		if (0 == strcmp(wpaSecurityWeb.wVersion, "0")) {
			prof->wpaSecurity.wVersion = AUTO_WPA;
		} else if (0 == strcmp(wpaSecurityWeb.wVersion, "1")) {
			prof->wpaSecurity.wVersion = WPA;
		} else {
			prof->wpaSecurity.wVersion = WPA2;
		}
		if (0 == strcmp(wpaSecurityWeb.sAlg, "0")) {
			prof->wpaSecurity.sAlg = AUTO_SEC;
		} else if (0 == strcmp(wpaSecurityWeb.sAlg, "1")) {
			prof->wpaSecurity.sAlg = TKIP;
		} else {
			prof->wpaSecurity.sAlg = AES;
		}
		prof->wpaSecurity.radiusIp = addrRadius.s_addr;
		prof->wpaSecurity.radiusPort = radiusPortVal;
		strcpy(prof->wpaSecurity.radiusPasswd,wpaSecurityWeb.radiusPasswd);
		prof->wpaSecurity.updataTime = wpaupVal;
	} else if (0 == strcmp(securMode, "3")) {
		prof->securMode = WPA1_PSK_AND_WPA2_PSK;

		if (0 == strcmp(wpaPskSecurityWeb.wVersion, "0")) {
			prof->wpaPskSecurity.wVersion = AUTO_WPA;
		} else if (0 == strcmp(wpaPskSecurityWeb.wVersion, "1")) {
			prof->wpaPskSecurity.wVersion = WPA;
		} else {
			prof->wpaPskSecurity.wVersion = WPA2;
		}
		if (0 == strcmp(wpaPskSecurityWeb.sAlg, "0")) {
			prof->wpaPskSecurity.sAlg = AUTO_SEC;
		} else if (0 == strcmp(wpaPskSecurityWeb.sAlg, "1")) {
			prof->wpaPskSecurity.sAlg = TKIP;
		} else {
			prof->wpaPskSecurity.sAlg = AES;
		}
		strcpy(prof->wpaPskSecurity.sharePasswd, wpaPskSecurityWeb.sharePasswd);
		prof->wpaPskSecurity.updataTime = wpaPskUpVal;
	} else {
		prof->securMode = NONE;
	}

	if (0 == strcmp(dhcpEn, "1")) {
		prof->dhcpEn = PROF_ENABLE;
	} else {
		prof->dhcpEn = PROF_DISABLE;
	}
	strcpy(prof->dhcpPool, dhcpPool);

#if (SZ_SHARE_LIMIT_AC == FYES)
	prof->limit_down = atoi(limit_down);
	prof->limit_up = atoi(limit_up);
	prof->limit_type = atoi(limit_type);
#endif

	if (NULL != profhead) {
		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
	}

	if (NULL != profheadDhcp) {
		ProfUpdate(profheadDhcp);
		ProfFreeAllocList(profheadDhcp);
	}
	nvramWriteCommit();

out:
	websRedirect(wp, "serviceZone.asp");
	return;
}

static void formServiceZoneListDel(webs_t wp, char_t *path, char_t *query)
{
	char *delStr = NULL, *tmp;
	int min, max, index = 0, change = 0;
	MibProfileType mibType = MIB_PROF_SERVICE_ZONE;
#if(DHCP_POOL == FYES)
	MibProfileType mibTypeDhcp = MIB_PROF_DHCP_POOL;
	DhcpPoolProfile *profDhcp = NULL;
#endif
	ServiceZoneProfile *prof = NULL;
	struProfAlloc *profhead = NULL;
	struProfAlloc *profheadDhcp = NULL;
	const char *ErrMsg = NULL;

	delStr = websGetVar(wp,T("delstr"),T(""));
	if (NULL == delStr) {
		goto out;
	}

	ProfInstLowHigh(mibType, &max, &min);

	tmp = strtok(delStr, UTT_SPLIT_SIGN_STR);
	while(tmp != NULL) {
		for (index = min; index < max; index++) {
			prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, index);
			if (NULL != prof) {
				if (!strcmp(tmp, "default")) {
					ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_DEL_DEFAULT);
					setTheErrorMsg((char *)ErrMsg);
					//setTheErrorMsg(T("不能删除默认选项"));
                } else if (!strcmp(prof->head.name, tmp)) {


                    UserProfile *profUser;
                    int minUser,maxUser,indexUser;
                    ProfInstLowHigh(MIB_PROF_USER, &maxUser, &minUser);
                    for(indexUser = minUser; indexUser < maxUser; indexUser++) {
                        profUser = (UserProfile*)ProfGetInstPointByIndex(MIB_PROF_USER, indexUser);
                        if(profUser != NULL && ProfInstIsFree(profUser) == 0) {
                            if ( (!strcmp(prof->dhcpPool,profUser->dhcpPoolName))  && ('\0' != *(profUser->dhcpPoolName))){
                                setTheErrorMsg("有DHCP静态绑定在该服务区，请先删除该绑定，再删除该服务区！");
                                goto out;
                            }
                        }
                    }
#if(FEATURE_AP_MAC_FILTER == FYES)
					ApMacFilterProfile *mfProf=NULL;
					int mf_min,mf_max,mf_index,mf_i=0;
                    ProfInstLowHigh(MIB_PROF_AP_MAC_FILTER, &mf_max, &mf_min);
					for(mf_index = mf_min; mf_index < mf_max; mf_index++)
					{
						mfProf = (ApMacFilterProfile*)ProfGetInstPointByIndex(MIB_PROF_AP_MAC_FILTER,mf_index);
						mf_i = 0;
						if((mfProf != NULL) && (ProfInstIsFree(mfProf) == 0))
						{
							while(mfProf->sz[mf_i][0] != '\0')
							{
								if(strcmp(prof->head.name,mfProf->sz[mf_i]) == 0)
								{
									setTheErrorMsg("有MAC地址过滤引用该服务区，请先删除MAC地址过滤，再删除该服务区！");
									goto out;
								}
								mf_i++;
							}
						}
					}
#endif
#if (DHCP_POOL == FYES)
                    if (PROF_ENABLE == prof->dhcpEn) {
                        profDhcp = (DhcpPoolProfile *)ProfGetInstPointByName(mibTypeDhcp, prof->dhcpPool);
                        if(profDhcp != NULL){
                        ProfBackupByName(mibTypeDhcp, PROF_CHANGE_EDIT, prof->dhcpPool, &profheadDhcp);
                        profDhcp->dhcpEn = PROF_DISABLE;
                        memset(profDhcp->interface, '\0', sizeof(profDhcp->interface));
                        }
                    }
#endif
                    ProfBackupByIndex(mibType, PROF_CHANGE_DEL, index, &profhead);
                    ProfDelInstByIndex(mibType, index);
                    change = 1;
                }
			}
		}

		tmp = strtok(NULL, UTT_SPLIT_SIGN_STR);
	}
	if (1 == change) {
		ProfUpdate(profheadDhcp);
		ProfFreeAllocList(profheadDhcp);
		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
		nvramWriteCommit();
	}

out:
    websRedirect(wp, "serviceZone.asp");
	return;
}

static void formServiceZoneListDelAll(webs_t wp, char_t *path, char_t *query)
{
	int min, max, index = 0;
	MibProfileType mibType = MIB_PROF_SERVICE_ZONE;
	ServiceZoneProfile *prof = NULL;
	struProfAlloc *profhead = NULL;
	struProfAlloc *profheadDhcp = NULL;
#if(DHCP_POOL == FYES)
	MibProfileType mibTypeDhcp = MIB_PROF_DHCP_POOL;
	DhcpPoolProfile *profDhcp = NULL;
#endif

	ProfInstLowHigh(mibType, &max, &min);

	for (index = (min + 1); index < max; index++) {
		prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, index);
        if (NULL != prof) {
            UserProfile *profUser;
            int minUser,maxUser,indexUser;
            ProfInstLowHigh(MIB_PROF_USER, &maxUser, &minUser);
            for(indexUser = minUser; indexUser < maxUser; indexUser++) {
                profUser = (UserProfile*)ProfGetInstPointByIndex(MIB_PROF_USER, indexUser);
                if(profUser != NULL && ProfInstIsFree(profUser) == 0) {
                    if ( (!strcmp(prof->dhcpPool,profUser->dhcpPoolName))  && ('\0' != *(profUser->dhcpPoolName))){
                        setTheErrorMsg("有DHCP静态绑定在该服务区，请先删除该绑定，再删除该服务区！");
                        goto out;
                    }
                }
            }
#if(FEATURE_AP_MAC_FILTER == FYES)
			ApMacFilterProfile *mfProf=NULL;
			int mf_min,mf_max,mf_index,mf_i=0;
            ProfInstLowHigh(MIB_PROF_AP_MAC_FILTER, &mf_max, &mf_min);
			for(mf_index = mf_min; mf_index < mf_max; mf_index++)
			{
				mfProf = (ApMacFilterProfile*)ProfGetInstPointByIndex(MIB_PROF_AP_MAC_FILTER,mf_index);
				mf_i = 0;
				if((mfProf != NULL) && (ProfInstIsFree(mfProf) == 0))
				{
					while(mfProf->sz[mf_i][0] != '\0')
					{
						if(strcmp(prof->head.name,mfProf->sz[mf_i]) == 0)
						{
							setTheErrorMsg("有MAC地址过滤引用该服务区，请先删除MAC地址过滤，再删除该服务区！");
							goto out;
						}
						mf_i++;
					}
				}
			}
#endif
#if(DHCP_POOL == FYES)
            if (PROF_ENABLE == prof->dhcpEn) {
                profDhcp = (DhcpPoolProfile *)ProfGetInstPointByName(mibTypeDhcp, prof->dhcpPool);
                if(profDhcp != NULL){
                ProfBackupByName(mibTypeDhcp, PROF_CHANGE_EDIT, prof->dhcpPool, &profheadDhcp);
				profDhcp->dhcpEn = PROF_DISABLE;
				memset(profDhcp->interface, '\0', sizeof(profDhcp->interface));
                }
			}
#endif
			ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, index, &profhead);
			ProfDelInstByIndex(mibType, index);
	   }
	}

	ProfUpdate(profheadDhcp);
	ProfFreeAllocList(profheadDhcp);
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();

out:
	websRedirect(wp, "serviceZone.asp");
	return;
}

#if(DHCP_POOL == FYES)
static int getNewServiceZone(int eid, webs_t wp, int argc, char **argv)
{
	int index, min, max, arrayNum = 0;
    DhcpPoolProfile *profDhcp = NULL;
    MibProfileType mibTypeDhcp = MIB_PROF_DHCP_POOL;

	websWrite(wp, "var poolNames = new Array();\n");
	websWrite(wp, "var poolVid = new Array();\n");
	websWrite(wp, "var beginIp = new Array();\n");
	websWrite(wp, "var endIp = new Array();\n");
	websWrite(wp, "var gateWays = new Array();\n");
	websWrite(wp, "var netMasks = new Array();\n");

	websWrite(wp, "gateWays[0] = \"%N\";\n", 0);
	websWrite(wp, "netMasks[0] = \"%N\";\n", 0);

	ProfInstLowHigh(mibTypeDhcp, &max, &min);

	for (index = min + 0; index < max; index++) {
		profDhcp = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibTypeDhcp, index);
		if ((NULL != profDhcp) && (0 == ProfInstIsFree((Pointer)profDhcp)) && (TRUE == profDhcp->head.active)) {
			websWrite(wp, "poolNames[%d] = \"%s\";\n",arrayNum, profDhcp->head.name);
			websWrite(wp, "poolVid[%d] = \"%d\";\n", arrayNum, profDhcp->vid);
			websWrite(wp, "beginIp[%d] = \"%N\";\n", arrayNum, profDhcp->ipStart);
			websWrite(wp, "endIp[%d] = \"%N\";\n", arrayNum, profDhcp->ipEnd);
			websWrite(wp, "gateWays[%d] = \"%N\";\n", arrayNum, profDhcp->gateway);
			websWrite(wp, "netMasks[%d] = \"%N\";\n", arrayNum, profDhcp->netmask);

			arrayNum++;
		}
	}

	getTheErrorMsg(eid, wp, argc, argv);

	return 0;
}
#endif


static int getServiceZoneGroup(int eid, webs_t wp, int argc, char **argv)
{
	int min, max, index, arrNum = 0;
	MibProfileType mibType = MIB_PROF_SERVICE_ZONE;
	ServiceZoneProfile *prof = NULL;

	websWrite(wp, "var statusSZ = new Array();\n");
	websWrite(wp, "var zoneName = new Array();\n");
	websWrite(wp, "var ssid = new Array();\n");
	websWrite(wp, "var broadcast = new Array();\n");
	websWrite(wp, "var encrytype = new Array();\n");
	websWrite(wp, "var isolate = new Array();\n");
	websWrite(wp, "var vlanId = new Array();\n");
	websWrite(wp, "var ipAddr = new Array();\n");
	websWrite(wp, "var netMask = new Array();\n");
	websWrite(wp, "var dhcp = new Array();\n");
	websWrite(wp, "autoIssued = new Array();\n");
#if (SZ_SHARE_LIMIT_AC == FYES)
	websWrite(wp, "var limit_type = new Array();\n");
	websWrite(wp, "var limit_down = new Array();\n");
	websWrite(wp, "var limit_up = new Array();\n");
#endif
	ProfInstLowHigh(mibType, &max, &min);

	for (index = min; index < max; index++) {
		prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, index);
		if ((prof != NULL) && (ProfInstIsFree((Pointer)prof) == 0) && (prof->head.active == TRUE)) {
			websWrite(wp, "zoneName[%d] = \"%s\";", arrNum, prof->head.name);
			if (prof->serviceEn == PROF_ENABLE) {
				websWrite(wp, "statusSZ[%d] = %s;", arrNum, "true");
			} else {
				websWrite(wp, "statusSZ[%d] = %s;", arrNum, "false");
			}
			websWrite(wp, "ssid[%d] = \"%s\";", arrNum, prof->ssid);
			if (prof->ssidBcast == PROF_ENABLE) {
				websWrite(wp, "broadcast[%d] = %s;", arrNum, "true");
			} else {
				websWrite(wp, "broadcast[%d] = %s;", arrNum, "false");
			}
		    websWrite(wp, "encrytype[%d] = \"%s\";", arrNum, MibGetStrByEnum(prof->securMode, uthModeEnum));
			if (prof->ssidIsolate == PROF_ENABLE) {
				websWrite(wp, "isolate[%d] = %s;", arrNum, "true");
			} else {
				websWrite(wp, "isolate[%d] = %s;", arrNum, "false");
			}
			if (0 == prof->vid) {
				websWrite(wp, "vlanId[%d] = \"\";", arrNum);
			} else {
				websWrite(wp, "vlanId[%d] = \"%d\";", arrNum, prof->vid);
			}

#if 0
			if (0 == index) {
				profD = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibTypeD, index);
				if ((prof != NULL) && (ProfInstIsFree((Pointer)prof) == 0) && (prof->head.active == TRUE)) {
					prof->gw = profD->gateway;
					prof->netmask = profD->netmask;
					prof->dhcpEn = profD->isUsed;
				}
			}
#endif

			if (prof->dhcpEn == PROF_ENABLE) {
				websWrite(wp, "dhcp[%d] = %s;", arrNum, "true");
			} else {
				websWrite(wp, "dhcp[%d] = %s;", arrNum, "false");
			}
			if (prof->vlanType == 0) {
				websWrite(wp, "autoIssued[%d] = %d;", arrNum, 0);
			} else {
				websWrite(wp, "autoIssued[%d] = %d;", arrNum, 1);
			}
			websWrite(wp, "ipAddr[%d] = \"%N\";", arrNum, prof->gw);
			websWrite(wp, "netMask[%d] = \"%N\";", arrNum, prof->netmask);
#if (SZ_SHARE_LIMIT_AC == FYES)
			if (prof->limit_type == 0u)
			{
			    websWrite(wp, "limit_type[%d] = \"2\";", arrNum);
			}
			else
			{
			    websWrite(wp, "limit_type[%d] = \"%d\";", arrNum, prof->limit_type);
			}
			websWrite(wp, "limit_down[%d] = \"%d\";", arrNum, prof->limit_down);
			websWrite(wp, "limit_up[%d] = \"%d\";", arrNum, prof->limit_up);
#endif
			arrNum++;
		}
	}

	websWrite(wp, "var totalrecs = %d;", arrNum);
	getTheErrorMsg(eid, wp, argc, argv);
	return 0;
}

#if 1
static int getServiceZoneGroupUrcp(int eid, webs_t wp, int argc, char **argv)
{
	int min, max, index, arrNum = 0;
	MibProfileType mibType = MIB_PROF_SERVICE_ZONE;
	ServiceZoneProfile *prof = NULL;

	websWrite(wp, "var statusSZ = new Array();\n");
	websWrite(wp, "var zoneName = new Array();\n");
	websWrite(wp, "var ssid = new Array();\n");
	websWrite(wp, "var broadcast = new Array();\n");
	websWrite(wp, "var encrytype = new Array();\n");
	websWrite(wp, "var isolate = new Array();\n");
	websWrite(wp, "var vlanId = new Array();\n");
	websWrite(wp, "var ipAddr = new Array();\n");
	websWrite(wp, "var netMask = new Array();\n");
	websWrite(wp, "var dhcp = new Array();\n");
	websWrite(wp, "autoIssued = new Array();\n");
#if (SZ_SHARE_LIMIT_AC == FYES)
	websWrite(wp, "var limit_type = new Array();\n");
	websWrite(wp, "var limit_down = new Array();\n");
	websWrite(wp, "var limit_up = new Array();\n");
#endif

	ProfInstLowHigh(mibType, &max, &min);

	for (index = min; index < max; index++) {
		prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, index);
		if ((prof != NULL) && (ProfInstIsFree((Pointer)prof) == 0) && (prof->head.active == TRUE) && (prof->serviceEn == PROF_ENABLE)) {
			websWrite(wp, "zoneName[%d] = \"%s\";", arrNum, prof->head.name);
			if (prof->serviceEn == PROF_ENABLE) {
				websWrite(wp, "statusSZ[%d] = %s;", arrNum, "true");
			} else {
				websWrite(wp, "statusSZ[%d] = %s;", arrNum, "false");
			}
			websWrite(wp, "ssid[%d] = \"%s\";", arrNum, prof->ssid);
			if (prof->ssidBcast == PROF_ENABLE) {
				websWrite(wp, "broadcast[%d] = %s;", arrNum, "true");
			} else {
				websWrite(wp, "broadcast[%d] = %s;", arrNum, "false");
			}
		    websWrite(wp, "encrytype[%d] = \"%s\";", arrNum, MibGetStrByEnum(prof->securMode, uthModeEnum));
			if (prof->ssidIsolate == PROF_ENABLE) {
				websWrite(wp, "isolate[%d] = %s;", arrNum, "true");
			} else {
				websWrite(wp, "isolate[%d] = %s;", arrNum, "false");
			}
			if (0 == prof->vid) {
				websWrite(wp, "vlanId[%d] = \"\";", arrNum);
			} else {
				websWrite(wp, "vlanId[%d] = \"%d\";", arrNum, prof->vid);
			}

			if (prof->dhcpEn == PROF_ENABLE) {
				websWrite(wp, "dhcp[%d] = %s;", arrNum, "true");
			} else {
				websWrite(wp, "dhcp[%d] = %s;", arrNum, "false");
			}
			if (prof->vlanType == 0) {
				websWrite(wp, "autoIssued[%d] = %d;", arrNum, 0);
			} else {
				websWrite(wp, "autoIssued[%d] = %d;", arrNum, 1);
			}
			websWrite(wp, "ipAddr[%d] = \"%N\";", arrNum, prof->gw);
			websWrite(wp, "netMask[%d] = \"%N\";", arrNum, prof->netmask);
#if (SZ_SHARE_LIMIT_AC == FYES)
			if (prof->limit_type == 0u)
			{
			    websWrite(wp, "limit_type[%d] = \"2\";", arrNum);
			}
			else
			{
			    websWrite(wp, "limit_type[%d] = \"%d\";", arrNum, prof->limit_type);
			}
			websWrite(wp, "limit_down[%d] = \"%d\";", arrNum, prof->limit_down);
			websWrite(wp, "limit_up[%d] = \"%d\";", arrNum, prof->limit_up);
#endif
			arrNum++;
		}
	}

	websWrite(wp, "var totalrecs = %d;", arrNum);
	getTheErrorMsg(eid, wp, argc, argv);
	return 0;
}
#else
static int getServiceZoneGroupUrcp(int eid, webs_t wp, int argc, char **argv)
{
	int min, max, index, arrNum = 0, found = 0;
	unsigned char *vidStr;
	unsigned int vid;
	MibProfileType mibType = MIB_PROF_SERVICE_ZONE;
	ServiceZoneProfile *prof = NULL;

	vidStr = websGetVar(wp, "vidStr", T(""));
	if ('\0' == vidStr) {
		vid = 0;
	} else {
		vid = (unsigned int)strtoul(vidStr, NULL, 10);
	}

	websWrite(wp, "var statusSZ = new Array();\n");
	websWrite(wp, "var zoneName = new Array();\n");
	websWrite(wp, "var ssid = new Array();\n");
	websWrite(wp, "var broadcast = new Array();\n");
	websWrite(wp, "var encrytype = new Array();\n");
	websWrite(wp, "var isolate = new Array();\n");
	websWrite(wp, "var vlanId = new Array();\n");
	websWrite(wp, "var ipAddr = new Array();\n");
	websWrite(wp, "var netMask = new Array();\n");
	websWrite(wp, "var dhcp = new Array();\n");

	ProfInstLowHigh(mibType, &max, &min);

	for (index = min; index < max; index++) {
		prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, index);
		if ((prof != NULL) && (ProfInstIsFree((Pointer)prof) == 0) && (prof->head.active == TRUE) && (prof->serviceEn == PROF_ENABLE)) {
			if ((prof->vlanType == 0) || (vid == 0)) {
				websWrite(wp, "zoneName[%d] = \"%s\";", arrNum, prof->head.name);
				if (prof->serviceEn == PROF_ENABLE) {
					websWrite(wp, "statusSZ[%d] = %s;", arrNum, "true");
				} else {
					websWrite(wp, "statusSZ[%d] = %s;", arrNum, "false");
				}
				websWrite(wp, "ssid[%d] = \"%s\";", arrNum, prof->ssid);
				if (prof->ssidBcast == PROF_ENABLE) {
					websWrite(wp, "broadcast[%d] = %s;", arrNum, "true");
				} else {
					websWrite(wp, "broadcast[%d] = %s;", arrNum, "false");
				}
		        websWrite(wp, "encrytype[%d] = \"%s\";", arrNum, MibGetStrByEnum(prof->securMode, uthModeEnum));
				if (prof->ssidIsolate == PROF_ENABLE) {
					websWrite(wp, "isolate[%d] = %s;", arrNum, "true");
				} else {
					websWrite(wp, "isolate[%d] = %s;", arrNum, "false");
				}
				if (0 == prof->vid) {
					websWrite(wp, "vlanId[%d] = \"\";", arrNum);
				} else {
					websWrite(wp, "vlanId[%d] = \"%d\";", arrNum, prof->vid);
				}

				if (prof->dhcpEn == PROF_ENABLE) {
					websWrite(wp, "dhcp[%d] = %s;", arrNum, "true");
				} else {
					websWrite(wp, "dhcp[%d] = %s;", arrNum, "false");
				}
				websWrite(wp, "ipAddr[%d] = \"%N\";", arrNum, prof->gw);
				websWrite(wp, "netMask[%d] = \"%N\";", arrNum, prof->netmask);

				arrNum++;
			} else {
				if ((found == 0) && (judgeVidBits(vid, prof->apVidBits) == 0)) {
					websWrite(wp, "zoneName[%d] = \"%s\";", arrNum, prof->head.name);
					if (prof->serviceEn == PROF_ENABLE) {
						websWrite(wp, "statusSZ[%d] = %s;", arrNum, "true");
					} else {
						websWrite(wp, "statusSZ[%d] = %s;", arrNum, "false");
					}
					websWrite(wp, "ssid[%d] = \"%s\";", arrNum, prof->ssid);
					if (prof->ssidBcast == PROF_ENABLE) {
						websWrite(wp, "broadcast[%d] = %s;", arrNum, "true");
					} else {
						websWrite(wp, "broadcast[%d] = %s;", arrNum, "false");
					}
		            websWrite(wp, "encrytype[%d] = \"%s\";", arrNum, MibGetStrByEnum(prof->securMode, uthModeEnum));
					if (prof->ssidIsolate == PROF_ENABLE) {
						websWrite(wp, "isolate[%d] = %s;", arrNum, "true");
					} else {
						websWrite(wp, "isolate[%d] = %s;", arrNum, "false");
					}
					if (0 == prof->vid) {
						websWrite(wp, "vlanId[%d] = \"\";", arrNum);
					} else {
						websWrite(wp, "vlanId[%d] = \"%d\";", arrNum, prof->vid);
					}

					if (prof->dhcpEn == PROF_ENABLE) {
						websWrite(wp, "dhcp[%d] = %s;", arrNum, "true");
					} else {
						websWrite(wp, "dhcp[%d] = %s;", arrNum, "false");
					}
					websWrite(wp, "ipAddr[%d] = \"%N\";", arrNum, prof->gw);
					websWrite(wp, "netMask[%d] = \"%N\";", arrNum, prof->netmask);

					arrNum++;
					found = 1;
				}
			}

		}
	}

	websWrite(wp, "var totalrecs = %d;", arrNum);
	getTheErrorMsg(eid, wp, argc, argv);
	return 0;
}
#endif

static int getOneServiceZone(int eid, webs_t wp, int argc, char **argv)
{
	int index, min, max, arrayNum = 0;
	char *edit_Name;
	MibProfileType mibType = MIB_PROF_SERVICE_ZONE;
	ServiceZoneProfile *prof = NULL;
#if(DHCP_POOL == FYES)
    DhcpPoolProfile *profDhcp = NULL;
	char tmpName[UTT_DHCP_POOL_LEN];
    MibProfileType mibTypeDhcp = MIB_PROF_DHCP_POOL;
	memset(tmpName, '\0', sizeof(tmpName));
#endif

	ejArgs(argc, argv, T("%s"), &edit_Name);

	prof = ProfGetInstPointByName(mibType, edit_Name);
	if (prof != NULL) {
		websWrite(wp, "var zoneName = \"%s\";", prof->head.name);
		if (prof->serviceEn == PROF_ENABLE) {
			websWrite(wp, "var enSZ = %s;", "true");
		} else {
			websWrite(wp, "var enSZ = %s;", "false");
		}
		websWrite(wp, "var ssid = \"%s\";", prof->ssid);
#if(SSID_ENCODE_TYPE == FYES)
		websWrite(wp, "var encodeType = \"%d\";", prof->ssid_encode_type);
#endif
		if (prof->ssidBcast == PROF_ENABLE) {
			websWrite(wp, "var enBroadcast = %s;", "true");
		} else {
			websWrite(wp, "var enBroadcast = %s;", "false");
		}
		if (prof->ssidIsolate == PROF_ENABLE) {
			websWrite(wp, "var enIsolate = %s;", "true");
		} else {
			websWrite(wp, "var enIsolate = %s;", "false");
		}
		if (0 == prof->vid) {
			websWrite(wp, "vlanId = \"\";");
		} else {
			websWrite(wp, "vlanId = \"%d\";", prof->vid);
		}
		websWrite(wp, "var client = \"%d\";", prof->client);
		websWrite(wp, "var encryType = \"%s\";",
				MibGetStrByEnum(prof->securMode, uthModeEnum));
		websWrite(wp, "var wepAuthType = \"%s\";",
				MibGetStrByEnum(prof->wepSecurity.auMethod, authMethodEnum));
		websWrite(wp, "var wepFormat = \"%s\";",
				MibGetStrByEnum(prof->wepSecurity.kType, keyTypeEnum));

		if (prof->wepSecurity.kNum4.key_state_action == PROF_ENABLE) {
			websWrite(wp, "var wepKeyRadio = \"%s\";", "3");
		} else if (prof->wepSecurity.kNum3.key_state_action == PROF_ENABLE) {
			websWrite(wp, "var wepKeyRadio = \"%s\";", "2");
		} else if (prof->wepSecurity.kNum2.key_state_action == PROF_ENABLE) {
			websWrite(wp, "var wepKeyRadio = \"%s\";", "1");
		} else {
			websWrite(wp, "var wepKeyRadio = \"%s\";", "0");
		}
		websWrite(wp, "var wepKey1Text = \"%s\";", prof->wepSecurity.kNum1.passwd);
		websWrite(wp, "var wepKey1Type = \"%s\";",
				MibGetStrByEnum(prof->wepSecurity.kNum1.kFmt, kFmtEnum));
		websWrite(wp, "var wepKey2Text = \"%s\";", prof->wepSecurity.kNum2.passwd);
		websWrite(wp, "var wepKey2Type = \"%s\";",
				MibGetStrByEnum(prof->wepSecurity.kNum2.kFmt, kFmtEnum));
		websWrite(wp, "var wepKey3Text = \"%s\";", prof->wepSecurity.kNum3.passwd);
		websWrite(wp, "var wepKey3Type = \"%s\";",
				MibGetStrByEnum(prof->wepSecurity.kNum3.kFmt, kFmtEnum));
		websWrite(wp, "var wepKey4Text = \"%s\";", prof->wepSecurity.kNum4.passwd);
		websWrite(wp, "var wepKey4Type = \"%s\";",
				MibGetStrByEnum(prof->wepSecurity.kNum4.kFmt, kFmtEnum));
		websWrite(wp, "var wpaVersion = \"%s\";",
				MibGetStrByEnum(prof->wpaSecurity.wVersion, wVersionEnum));
		websWrite(wp, "var wpaEncrp = \"%s\";",
				MibGetStrByEnum(prof->wpaSecurity.sAlg, sAlgEnum));
		websWrite(wp, "var radiusIP =\"%N\";", prof->wpaSecurity.radiusIp);
		if(prof->wpaSecurity.radiusPort)
		{
		websWrite(wp, "var radiusPort = \"%d\";", prof->wpaSecurity.radiusPort);
		}
		else
		{
		websWrite(wp, "var radiusPort = \"1812\";");
		}
		websWrite(wp, "var radiusPsswd = \"%s\";", prof->wpaSecurity.radiusPasswd);
		websWrite(wp, "var wpaTime = \"%d\";", prof->wpaSecurity.updataTime);
		websWrite(wp, "var pskVersion = \"%s\";",
				MibGetStrByEnum(prof->wpaPskSecurity.wVersion, wVersionEnum));
		websWrite(wp, "var pskEncry = \"%s\";",
				MibGetStrByEnum(prof->wpaPskSecurity.sAlg, sAlgEnum));
		websWrite(wp, "var pskPsswd = \"%s\";", prof->wpaPskSecurity.sharePasswd);
		websWrite(wp, "var pskTime = \"%d\";", prof->wpaPskSecurity.updataTime);

#if 0
		if (prof->virif != 0) {
			websWrite(wp, "var ifAddr = \"%N\";", prof->virif);
		} else {
			websWrite(wp, "var ifAddr = \"\";");
		}
#endif

		websWrite(wp, "var ipAddr = \"%N\";", prof->gw);
#if 0
		if (prof->netmask != 0) {
			websWrite(wp, "var maskAddr = \"%N\";", prof->netmask);
		} else {
			websWrite(wp, "var maskAddr = \"\";");
		}
#endif

		if (prof->dhcpEn == PROF_ENABLE) {
			websWrite(wp, "var dhcpServer = %s;", "true");
		} else {
			websWrite(wp, "var dhcpServer = %s;", "false");
		}
	websWrite(wp, "var pool = \"%s\";", prof->dhcpPool);
	websWrite(wp, "var vlanType = \"%d\";", prof->vlanType);
	websWrite(wp, "var apvlan = \"%s\";", prof->strApVid);

#if (SZ_SHARE_LIMIT_AC == FYES)
	websWrite(wp, "var limit_type = \"%u\";\n", prof->limit_type);
	websWrite(wp, "var limit_down = \"%u\";\n", prof->limit_down);
	websWrite(wp, "var limit_up = \"%u\";\n", prof->limit_up);
#endif
	}

	websWrite(wp, "var poolNames = new Array();\n");
	websWrite(wp, "var poolVid = new Array();\n");
	websWrite(wp, "var beginIp = new Array();\n");
	websWrite(wp, "var endIp = new Array();\n");
	websWrite(wp, "var gateWays = new Array();\n");
	websWrite(wp, "var netMasks = new Array();\n");

	websWrite(wp, "gateWays[0] = \"%N\";\n", 0);
	websWrite(wp, "netMasks[0] = \"%N\";\n", 0);

#if 0
	if (!strcmp(edit_Name, "default")) {
		profDhcp = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibTypeDhcp, 0);
		if ((NULL != profDhcp) && (0 == ProfInstIsFree((Pointer)profDhcp)) && (TRUE == profDhcp->head.active)) {
			websWrite(wp, "poolNames[%d] = \"%s\";\n",arrayNum, profDhcp->head.name);
			websWrite(wp, "poolVid[%d] = \"%d\";\n", arrayNum, profDhcp->vid);
			websWrite(wp, "beginIp[%d] = \"%N\";\n", arrayNum, profDhcp->ipStart);
			websWrite(wp, "endIp[%d] = \"%N\";\n", arrayNum, profDhcp->ipEnd);
			websWrite(wp, "gateWays[%d] = \"%N\";\n", arrayNum, profDhcp->gateway);
			websWrite(wp, "netMasks[%d] = \"%N\";\n", arrayNum, profDhcp->netmask);
		}
	} else {
#endif

#if(DHCP_POOL == FYES)
		ProfInstLowHigh(mibTypeDhcp, &max, &min);

		for (index = min + 0; index < max; index++) {
			profDhcp = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibTypeDhcp, index);
			if ((NULL != profDhcp) && (0 == ProfInstIsFree((Pointer)profDhcp)) && (TRUE == profDhcp->head.active)) {
				websWrite(wp, "poolNames[%d] = \"%s\";\n",arrayNum, profDhcp->head.name);
			    websWrite(wp, "poolVid[%d] = \"%d\";\n", arrayNum, profDhcp->vid);
				websWrite(wp, "beginIp[%d] = \"%N\";\n", arrayNum, profDhcp->ipStart);
				websWrite(wp, "endIp[%d] = \"%N\";\n", arrayNum, profDhcp->ipEnd);
				websWrite(wp, "gateWays[%d] = \"%N\";\n", arrayNum, profDhcp->gateway);
				websWrite(wp, "netMasks[%d] = \"%N\";\n", arrayNum, profDhcp->netmask);

				arrayNum++;
			}
		}
#endif
#if 0
	}
#endif

	getTheErrorMsg(eid, wp, argc, argv);
	return 0;
}

#if (SZONE_AUTO_UPDATA == FYES)

static int getSzoneAutoUpGlobalConfig(int eid, webs_t wp, int argc, char_t **argv)
{

    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;

    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibTypeSys, 0);

    if(profSys->szoneAutoUpdata == FUN_ENABLE) /**/
    {
        websWrite(wp,"var SzoneAutoUpdata =\"on\";");
    }
    else
    {
        websWrite(wp,"var SzoneAutoUpdata =\"\";");
    }

    getTheErrorMsg(eid, wp, argc, argv);  
    return 0;
}
/***********************************************************************
 * 函 数 名：   formConfigSzoneAutoUpGlobalConfig
 * 功能描述：   保存全局配置
 * 创建日期：	2010-3-17
 * 修改日期：   2011-10-24 bhou
 * 作者：       柏德秀
 * 附加说明：	无
 ***********************************************************************/
static void formConfigSzoneAutoUpGlobalConfig(webs_t wp, char_t *path, char_t *query)
{
    char *Enable = NULL;
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    FunEnableEnum enable = FUN_DISABLE;

    Enable = websGetVar(wp, "SzoneAutoUpdata", T(""));
    if( !strcmp(Enable, "on"))/**/
    {
	    enable = FUN_ENABLE;/**/
    }
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibTypeSys, 0);
    if(profSys != NULL)
    {
        if(profSys->szoneAutoUpdata != enable){

            profSys->szoneAutoUpdata = enable;
            nvramWriteCommit();
        }
    }
    websRedirect(wp, "SzoneAutoUpdata.asp");/*页面跳转*/
    return;
}
#endif
void formDefineServiceZone(void)
{
#if (DHCP_POOL == FYES)
	websAspDefine(T("aspAddNewServiceZone"), getNewServiceZone);
#endif
	websAspDefine(T("getOneServiceZone"), getOneServiceZone);
	websAspDefine(T("aspOutPutServiceZone"), getServiceZoneGroup);
	websAspDefine(T("aspOutPutServiceZoneUrcp"), getServiceZoneGroupUrcp);
	websFormDefine(T("formServiceZoneConfig"), formServiceZoneConfig);
	websFormDefine(T("formServiceZoneListDel"), formServiceZoneListDel);
	websFormDefine(T("formServiceZoneListDelAll"), formServiceZoneListDelAll);
#if (SZONE_AUTO_UPDATA == FYES)
    websAspDefine(T("aspOutSzoneAutoUpGlobalConfig"), getSzoneAutoUpGlobalConfig);
	websFormDefine(T("formSzoneAutoUpGlobalConfig"), formConfigSzoneAutoUpGlobalConfig);
#endif
}

#endif
