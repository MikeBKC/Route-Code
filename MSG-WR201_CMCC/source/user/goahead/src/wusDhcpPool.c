
#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include	"wuScript.h"
#include    <string.h>
#include	"translate.h"
#include    <uttMachine.h>

#if (DHCP_POOL == FYES)

#define DEBUG_CK 0

#if DEBUG_CK
	#define debug() do { \
						printf("debug: %d in %s()\n", __LINE__, __func__); \
						} while (0);
#else
	#define debug()
#endif

#define DHCP_LEASE_FILE "/var/udhcpd.leases"
extern void webArpBindDelAll(webs_t wp);
extern void webArpBindDel(webs_t wp);
extern void webConfigArpBindConfig(webs_t wp);

#if (DHCP_SERVER_OPTION_43 == FYES)
static void generateOption43TLV(int index, DhcpPoolProfile *prof, char *showBuf);
#endif


static void formDhcpServer(webs_t wp, char_t *path, char_t *query)
{
	char_t *dhcp_server_en;
	ProfChangeType profType = PROF_CHANGE_EDIT;
    MibProfileType mibType = MIB_PROF_DHCP_POOL;
    DhcpPoolProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
#if (SERVICE_ZONE == FYES)
	ServiceZoneProfile *profSZ = NULL;
    MibProfileType mibTypeSZ = MIB_PROF_SERVICE_ZONE;
#endif

	dhcp_server_en = websGetVar(wp, T("lanDhcpType"), T(""));

	prof = (DhcpPoolProfile *)ProfGetInstPointByName(mibType, "default");
	if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)) && (TRUE == prof->head.active)) {
		ProfBackupByName(mibType, profType, "default", &profhead);
		if (!strcmp("SERVER", dhcp_server_en)) {
			prof->dhcpEn = PROF_ENABLE;
		} else {
			prof->dhcpEn = PROF_DISABLE;
		}
	}

#if (SERVICE_ZONE == FYES)
	profSZ = (ServiceZoneProfile *)ProfGetInstPointByName(mibTypeSZ, "default");
	if ((NULL != profSZ) && (0 == ProfInstIsFree((Pointer)profSZ)) && (TRUE == profSZ->head.active)) {
		if (!strcmp("SERVER", dhcp_server_en)) {
			profSZ->dhcpEn = PROF_ENABLE;
		} else {
			profSZ->dhcpEn = PROF_DISABLE;
		}
	}
#endif

	ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();

    websRedirect(wp,"DhcpServerForAc.asp");
	return;
}

static int aspDhcpServerConfig(int eid, webs_t wp, int argc, char_t **argv)
{
	int arrayNum = 0;
    DhcpPoolProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_DHCP_POOL;

	websWrite(wp, "var poolNames = new Array();\n");
	websWrite(wp, "var beginIp = new Array();\n");
	websWrite(wp, "var endIp = new Array();\n");

	prof = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibType, 0);
	if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)) && (TRUE == prof->head.active)) {
		websWrite(wp, "poolNames[%d] = \"%s\";\n",arrayNum, prof->head.name);
		websWrite(wp, "beginIp[%d] = \"%N\";\n", arrayNum, prof->ipStart);
		websWrite(wp, "endIp[%d] = \"%N\";\n", arrayNum, prof->ipEnd);
	}

	websWrite(wp, "var setPoolName = \"%s\";\n", "default");

	if (PROF_ENABLE == prof->dhcpEn) {
		websWrite(wp, "var lanDhcpTypes = \"Enable\";\n");
	} else {
		websWrite(wp, "var lanDhcpTypes = \"Disable\";\n");
	}

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

static char * splitInterface(char *interface, char split)
{
	int count = 0;
	char *p = interface;

	while (*p != '\0') {
		p++;
	}

	while (*p != split) {
		p--;
	}

	if (*p == split) {
		return p;
	} else {
		return NULL;
	}
}

static void formDhcpPoolConfig(webs_t wp, char_t *path, char_t *query)
{
    char_t  *new_name, *old_name, *dhcp_s, *dhcp_e, *dhcp_m,
			*dhcp_pd, *dhcp_sd, *dhcp_g, *dhcp_l, *dhcp_vid,*dhcp_en;
    Uint32 tmpNum = 0, lanIp = 0;

#if (DHCP_SERVER_OPTION_43 == FYES)
    char_t *option43Type, *option43Addr;
#endif

	char tmp[32];
#if (MULTI_LAN == FYES)
	Uint32 lanIp1 = 0, lanIp2 = 0, lanIp3 = 0;
#endif
	int max, min, index, indexTmp;
	ProfChangeType profType = PROF_CHANGE_ADD;
    unsigned int nMaxIp, vid = 0;
    MibProfileType mibType = MIB_PROF_DHCP_POOL;
#if (SERVICE_ZONE == FYES)
	MibProfileType mibTypeSz = MIB_PROF_SERVICE_ZONE;
	ServiceZoneProfile *profSz = NULL;
	struProfAlloc *profheadSz = NULL;
#endif
#if (TAG_VLAN == FYES)
	MibProfileType mibTypeTv = MIB_PROF_TAG_VLAN;
	TagVlanProfile *profTv = NULL;
	struProfAlloc *profheadTv = NULL;
#endif

#if (VLAN_CONFIG == FYES)
	int maxV, minV, indexV;
    MibProfileType mibTypeV = MIB_PROF_VLAN_CONFIG;
    VlanConfigProfile *profV = NULL;
#endif

    DhcpPoolProfile *prof = NULL;
    struct in_addr addrS, addrE, addrNm, addrGw;
    InterfaceProfile *profIf = NULL;
    struProfAlloc *profhead = NULL;
    IpPoolEnum cfPoolType = IP_POOL_NONE;
    const char *ErrMsg = NULL;
    UserProfile *profUser;

	new_name = websGetVar(wp, T("poolName"), T(""));
	old_name = websGetVar(wp, T("oldName"), T(""));
	dhcp_en = websGetVar(wp, T("DhcpEnable"), T(""));
	dhcp_vid = websGetVar(wp, T("poolVid"), T(""));
    dhcp_s = websGetVar(wp, T("dhcpStart"), T(""));
    dhcp_e = websGetVar(wp, T("dhcpEnd"), T(""));
    dhcp_m = websGetVar(wp, T("dhcpMask"), T(""));
    dhcp_pd = websGetVar(wp, T("dhcpPriDns"), T(""));
    dhcp_sd = websGetVarNull(wp, T("dhcpSecDns"), T("0.0.0.0"), T("0.0.0.0"));
    dhcp_g = websGetVar(wp, T("dhcpGateway"), T(""));
    dhcp_l = websGetVar(wp, T("dhcpLease"), T("1440"));

#if (DHCP_SERVER_OPTION_43 == FYES)
    option43Type = websGetVar(wp, T("option43Type"), T(""));
    option43Addr = websGetVar(wp, T("option43Addr"), T(""));
#endif

	if (strncmp(dhcp_vid, "VIF", 3) == 0) {
        vid = strtol(dhcp_vid + 3, NULL, 10);
	}

	debug();
    if (0 == inet_aton(dhcp_s, &addrS)) {
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_START_IP);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("无效起始地址"));
		goto out;
    }
    if (0 == inet_aton(dhcp_e, &addrE)) {
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_END_IP);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("无效结束地址"));
		goto out;
    }
    if (0 == inet_aton(dhcp_m, &addrNm)) {
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_SUBMASK);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("无效子网掩码"));
		goto out;
    }
    if (0 == inet_aton(dhcp_g, &addrGw)) {
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_GATEWAY);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("无效网关"));
		goto out;
    }
    /*
     * 判断地址池
     */
	debug();
    cfPoolType = ipPoolConflict(IP_POOL_DHCP, addrS.s_addr, addrE.s_addr);
    if (cfPoolType != IP_POOL_NONE) {
	debug();
	setPoolConflictMsg(cfPoolType);
	goto out;
    }

    /*
     * 是否与dhcp server地址池冲突
     */
	MibProfileType mibTypeDhcp = MIB_PROF_DHCP_POOL;
	DhcpPoolProfile *profDhcp = NULL;
	ProfInstLowHigh(mibTypeDhcp, &max, &min);

	for (index = min; index < max; index++) {
		profDhcp = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibTypeDhcp, index);
        if ((NULL != profDhcp) ) {
            if ( !strcmp(old_name, profDhcp->head.name) && ('\0' != *old_name) ){
                continue;
            }else{
                if((ntohl(addrS.s_addr) & ntohl(addrNm.s_addr)) >= (ntohl(profDhcp->ipStart) & ntohl(addrNm.s_addr) )
                       && (ntohl(addrS.s_addr) & ntohl(addrNm.s_addr)) <= (ntohl(profDhcp->ipEnd) & ntohl(addrNm.s_addr))
                    ){
					ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_POOL_ADDCONFLICK);
					setTheErrorMsg(ErrMsg);
		            goto out;
                }
                if((ntohl(addrE.s_addr)&ntohl(addrNm.s_addr)) >= (ntohl(profDhcp->ipStart) & ntohl(addrNm.s_addr))
                       && (ntohl(addrE.s_addr) & ntohl(addrNm.s_addr)) <= (ntohl(profDhcp->ipEnd) & ntohl(addrNm.s_addr))
                    ){
					ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_POOL_ADDCONFLICK);
	                setTheErrorMsg(ErrMsg);
		            goto out;
                }
            }
		}
	}


#if(UTT_NV_1800N != FYES)
	debug();
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);
    lanIp = ntohl(profIf->ether.sta.ip);
    if(lanIp <= ntohl(addrE.s_addr) && lanIp >= ntohl(addrS.s_addr)){
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("DHCP地址池包含了LAN口地址"));
		goto out;
    }
#if (MULTI_LAN == FYES)
	debug();
    lanIp1 = ntohl(profIf->ether.sta.ip2);
    if((lanIp1 <= ntohl(addrE.s_addr) && lanIp1 >= ntohl(addrS.s_addr))) {
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("DHCP地址池包含了LAN口地址"));
		goto out;
    }

    lanIp2 = ntohl(profIf->ether.sta.ip3);
    if((lanIp2 <= ntohl(addrE.s_addr) && lanIp2 >= ntohl(addrS.s_addr))) {
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("DHCP地址池包含了LAN口地址"));
		goto out;
    }

    lanIp3 = ntohl(profIf->ether.sta.ip4);
    if((lanIp3 <= ntohl(addrE.s_addr) && lanIp3 >= ntohl(addrS.s_addr))) {
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("DHCP地址池包含了LAN口地址"));
		goto out;
    }
#endif
#else
	debug();
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 1);
    if( profIf->ether.connMode == ETH_CONN_STATIC)
    {/*只有静态ip类型判断。因动态类型无法判断*/
	lanIp = ntohl(profIf->ether.sta.ip);
	if(lanIp <= ntohl(addrE.s_addr) && lanIp >= ntohl(addrS.s_addr)){
	debug();
	    ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN);
	    setTheErrorMsg((char *)ErrMsg);
	    //setTheErrorMsg(T("DHCP地址池包含了LAN口地址"));
	    goto out;
	}
    }
#endif

	if( !strncmp(new_name,"default",7) ){
	debug();
		if((addrS.s_addr & addrNm.s_addr) != (htonl(lanIp) & addrNm.s_addr)){
			ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_IP_LAN_IP_NOT_SAME_NETWORK);
			setTheErrorMsg((char *)ErrMsg);
			//setTheErrorMsg(T("DHCPPOOL地址和LANIP地址不在同一网络!"));1
			goto out;
		}
	}

    if((addrS.s_addr & addrNm.s_addr) != (addrGw.s_addr & addrNm.s_addr)){
	debug();
	ErrMsg = getTransDataForC(C_LANG_INDEX_START_IP_GATEWAY_NOT_SAME_NETWORK);
	setTheErrorMsg((char *)ErrMsg);
        //setTheErrorMsg(T("起始地址和网关地址不在同一网络!"));
		goto out;
    }

    if((addrS.s_addr & addrNm.s_addr) != (addrE.s_addr & addrNm.s_addr)){
	debug();
	ErrMsg = getTransDataForC(C_LANG_INDEX_START_IP_END_IP_NOT_SAME_NETWORK);
	setTheErrorMsg((char *)ErrMsg);
        //setTheErrorMsg(T("起始地址和结束地址不在同一网络"));
		goto out;
    }

    nMaxIp = (addrS.s_addr & addrNm.s_addr) | (~(addrNm.s_addr));     //the max ip
	debug();
    if(addrE.s_addr >= nMaxIp) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_END_IP_TOO_BIG);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg (T("结束地址过大! "));
        goto out;
    }


    ProfInstLowHigh(MIB_PROF_USER, &max, &min);
    for(index = min; index < max; index++) {
        profUser = (UserProfile*)ProfGetInstPointByIndex(MIB_PROF_USER, index);
        if(profUser != NULL && ProfInstIsFree(profUser) == 0) {
            if ( !strcmp(old_name, profUser->dhcpPoolName) ){
                if ((ntohl(addrS.s_addr) > ntohl(profUser->ip)) || (ntohl(addrE.s_addr) < ntohl(profUser->ip))){
                    ErrMsg = getTransDataForC(C_LANG_INDEX_STATIC_IP_NOT_IN_DHCP_POOL);
                    setTheErrorMsg((char *)ErrMsg);
                    goto out;
                }
				/*如果地址池VID被修改，做判断*/
                if (profUser->vid != vid){
					setTheErrorMsg(T("操作失败，请先删除DHCP静态绑定成员"));
                    goto out;
                }
            }
        }
    }

	debug();
	ProfInstLowHigh(mibType, &max, &min);
	ProfInstLowHigh(mibTypeV, &maxV, &minV);
	if ('\0' == *old_name) {
	debug();
		for (index = min; index < max; index++) {
			prof = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibType, index);
			if (NULL != prof) {
				if (!strcmp(new_name, prof->head.name)) {
					ErrMsg = getTransDataForC(C_LANG_INDEX_SAME_ACCOUNT_EXIST);
					setTheErrorMsg((char *)ErrMsg);
					//setTheErrorMsg(T("操作失败，系统中存在同名帐号"));
					goto out;
				}

			}
		}
		/*vid是否冲突*/
		for (index = min; index < max; index++) {
			prof = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibType, index);
			if (NULL != prof) {
				if (prof->vid == vid) {
					//ErrMsg = getTransDataForC(C_LANG_INDEX_SAME_VID_EXIST);
					//setTheErrorMsg((char *)ErrMsg);
					setTheErrorMsg(T("操作失败，系统中存在相同VID的地址池"));
					goto out;
				}

			}
		}

#if (VLAN_CONFIG == FYES)
		profV = (VlanConfigProfile *)ProfGetInstPointByName(mibTypeV, dhcp_vid);
		if (profV != NULL) {
			if (profV->netmask != addrNm.s_addr) {
					setTheErrorMsg(T("操作失败，网关地址与虚接口地址不符！"));
					goto out;
			}

			if((addrS.s_addr & addrNm.s_addr) != (profV->ip & addrNm.s_addr)){
			debug();
//			ErrMsg = getTransDataForC(C_LANG_INDEX_START_IP_GATEWAY_NOT_SAME_NETWORK);
//			setTheErrorMsg((char *)ErrMsg);
					setTheErrorMsg(T("操作失败，地址池与虚接口地址不在同一网络！"));
			    //setTheErrorMsg(T("起始地址和网关地址不在同一网络!"));
			   goto out;
			}

			if((addrS.s_addr & addrNm.s_addr) != (profV->ip & addrNm.s_addr)){
			debug();
			    //setTheErrorMsg(T("起始地址和结束地址不在同一网络"));
					setTheErrorMsg(T("操作失败，地址池与虚接口地址不在同一网络！"));
			   goto out;
			}

			 if(ntohl(profV->ip) <= ntohl(addrE.s_addr) && ntohl(profV->ip) >= ntohl(addrS.s_addr)){
			 debug();
//				ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN);
//				setTheErrorMsg((char *)ErrMsg);
				setTheErrorMsg(T("DHCP地址池包含了虚接口地址"));
				goto out;
			 }
		}
#endif

		index = ProfNewInst(mibType, new_name, FALSE);
		profType = PROF_CHANGE_ADD;
	} else if (0 == strcmp("default", old_name)) {
//		setTheErrorMsg(T("默认地址池不能修改！"));
//		goto out;
	debug();
#if 1
#if (SERVICE_ZONE == FYES)
		profSz = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibTypeSz, 0);
		ProfBackupByIndex(mibTypeSz, PROF_CHANGE_EDIT , 0, &profheadSz);
        if(profIf->ether.sta.nm != addrNm.s_addr){
		ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_POOL_NOTMODIFY);
		setTheErrorMsg((char *)ErrMsg);
					
		//setTheErrorMsg(T("默认地址池的子网掩码不能在这修改，需要到LAN口配置去修改！"));
		goto out;
        }else{
		profSz->netmask = addrNm.s_addr;
        }
		profSz->gw = addrGw.s_addr;
#endif
		strcpy(new_name, "default");
		index = 0;
		profType = PROF_CHANGE_EDIT;
#endif
	} else {
	debug();
		indexTmp = ProfGetInstIndexByName(mibType, old_name);
		if (indexTmp == PROFFAIL) {
			goto out;
		}

		for (index = min; index < max; index++) {
	debug();
			if (index != indexTmp) {
				prof = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibType, index);
				if (NULL != prof) {
					if (!strcmp(new_name, prof->head.name)) {
						ErrMsg = getTransDataForC(C_LANG_INDEX_SAME_ACCOUNT_EXIST);
						setTheErrorMsg((char *)ErrMsg);
						//setTheErrorMsg(T("操作失败，系统中存在同名帐号"));
						goto out;
					}
				}
			}
		}
	debug();
		/*vid是否冲突*/
		for (index = min; index < max; index++) {
			if (index != indexTmp) {
				prof = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibType, index);
				if (NULL != prof) {
					if (prof->vid == vid) {
						//ErrMsg = getTransDataForC(C_LANG_INDEX_SAME_VID_EXIST);
						//setTheErrorMsg((char *)ErrMsg);
						setTheErrorMsg(T("操作失败，系统中存在相同VID的地址池"));
						goto out;
					}

				}
			}
		}
#if 0

		prof = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibType, indexTmp);
	debug();
		if (NULL != prof) {
			if (PROF_ENABLE == prof->dhcpEn) {
				tmpNum = (Uint32)strtoul(splitInterface(prof->interface, '.'), NULL, 10);
			}
		}
#endif
#if (SERVICE_ZONE == FYES)
		ProfInstLowHigh(mibTypeSz, &max, &min);
		for (index = min + 1; index < max; index++) {
			profSz = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibTypeSz, index);
			if (NULL != profSz) {
//				if (profSz->vid == tmpNum)
                if ( !strcmp(old_name,profSz->dhcpPool) )
                {
					ProfBackupByIndex(mibTypeSz, PROF_CHANGE_EDIT , index, &profheadSz);
					profSz->gw = addrGw.s_addr;
					profSz->netmask = addrNm.s_addr;
					break;
				}
			}
		}
#endif
#if (VLAN_CONFIG == FYES)
		profV = (VlanConfigProfile *)ProfGetInstPointByName(mibTypeV, dhcp_vid);
		if (profV != NULL) {
			if (profV->netmask != addrNm.s_addr) {
					setTheErrorMsg(T("操作失败，网关地址与虚接口地址不符！"));
					goto out;
			}

			if((addrS.s_addr & addrNm.s_addr) != (profV->ip & addrNm.s_addr)){
			debug();
//			ErrMsg = getTransDataForC(C_LANG_INDEX_START_IP_GATEWAY_NOT_SAME_NETWORK);
//			setTheErrorMsg((char *)ErrMsg);
					setTheErrorMsg(T("操作失败，地址池与虚接口地址不在同一网络！"));
			    //setTheErrorMsg(T("起始地址和网关地址不在同一网络!"));
			   goto out;
			}

			if((addrS.s_addr & addrNm.s_addr) != (profV->ip & addrNm.s_addr)){
			debug();
			    //setTheErrorMsg(T("起始地址和结束地址不在同一网络"));
					setTheErrorMsg(T("操作失败，地址池与虚接口地址不在同一网络！"));
			   goto out;
			}

			 if(ntohl(profV->ip) <= ntohl(addrE.s_addr) && ntohl(profV->ip) >= ntohl(addrS.s_addr)){
			 debug();
//				ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN);
//				setTheErrorMsg((char *)ErrMsg);
				setTheErrorMsg(T("DHCP地址池包含了虚接口地址"));
				goto out;
			 }
		}
#endif

#if (TAG_VLAN == FYES)
		ProfInstLowHigh(mibTypeTv, &max, &min);
		for (index = min; index < max; index++) {
		    profTv = (TagVlanProfile *)ProfGetInstPointByIndex(mibTypeTv, index);
		    if (NULL != profTv) {
			if ( !strcmp(old_name,profTv->dhcpPool) )
			{
			    ProfBackupByIndex(mibTypeTv, PROF_CHANGE_EDIT , index, &profheadTv);
			    profTv->gw = addrGw.s_addr;
			    profTv->netmask = addrNm.s_addr;
			    break;
			}
		    }
		}
#endif

	debug();
		index = indexTmp;
		profType = PROF_CHANGE_EDIT;
	}
	debug();
		prof = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibType, index);
		ProfBackupByIndex(mibType, PROF_CHANGE_EDIT , index, &profhead);
		if (NULL != prof) {
	debug();
			strcpy(prof->head.name, new_name);
			if(strcmp(dhcp_en,"on") == 0)
			{
				prof->dhcpEn = PROF_ENABLE;
			}else{
				prof->dhcpEn = PROF_DISABLE;
			}
			prof->vid = vid;
//			memset(prof->interface, '\0', sizeof(prof->interface));
//			sprintf(prof->interface, "%s", (char *)getLanIfName());
			if (prof->vid == 0) {
				strcpy(prof->interface,"LAN");
			} else {
				memset(tmp, 0, sizeof(tmp));
				sprintf(tmp, "VIF%d", prof->vid);
				strcpy(prof->interface, tmp);
			}
			prof->ipStart = addrS.s_addr;
			prof->ipEnd = addrE.s_addr;
			prof->netmask = addrNm.s_addr;
			prof->gateway = addrGw.s_addr;
			prof->lease = strtol(dhcp_l, NULL, 10); /*minutes*/
			prof->lease = prof->lease * 60; /*seconds*/
			prof->pDns = inet_addr(dhcp_pd);
			prof->sDns = inet_addr(dhcp_sd);
#if (DHCP_SERVER_OPTION_43 == FYES)
            prof->option43Type = option43Type[0] - '0';
            memset(prof->option43Addr, 0, sizeof(prof->option43Addr));
            memset(prof->option43Addr2, 0, sizeof(prof->option43Addr2));
            if (option43Type != DHCP_OPTION_43_TYPE_DISABLE)
                strncpy(prof->option43Addr, option43Addr, DHCP_OPTION_43_LEN);
            if (strlen(option43Addr) > DHCP_OPTION_43_LEN)
                strncpy(prof->option43Addr2, option43Addr+DHCP_OPTION_43_LEN, DHCP_OPTION_43_LEN);
#endif
		}

	debug();
#if (SERVICE_ZONE == FYES)
    ProfUpdate(profheadSz);
    ProfFreeAllocList(profheadSz);
#endif
#if (TAG_VLAN == FYES)
    ProfUpdate(profheadTv);
    ProfFreeAllocList(profheadTv);
#endif
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
	debug();
out:
    websRedirect(wp,"DhcpPoolForAc.asp");
	return;
}

#if (DHCP_SERVER_OPTION_43 == FYES)
void generateOption43TLV(int index, DhcpPoolProfile *prof, char *showBuf)
{
    int i;
    struct in_addr addr;
    if (prof->option43Type == DHCP_OPTION_43_TYPE_HEX)
    {
        inet_aton(prof->option43Addr, &addr);
        sprintf(showBuf,"%02x%02x%02x%02x%02x%02x%02x%02x",
                0x2b, 0x06, 0x01, 0x04, 
#if defined(CONFIG_BOARD_P1010_C)
                (addr.s_addr>>24)&0xff, (addr.s_addr>>16)&0xff,
                (addr.s_addr>>8)&0xff, (addr.s_addr)&0xff);
#else
                (addr.s_addr)&0xff, (addr.s_addr>>8)&0xff,
                (addr.s_addr>>16)&0xff, (addr.s_addr>>24)&0xff);
#endif
    }
    else if (prof->option43Type == DHCP_OPTION_43_TYPE_ASCII)
    {
        sprintf(showBuf, "%02x%02x%02x%02x", 
                0x2b, strlen(prof->option43Addr)+2, 
                0x03, strlen(prof->option43Addr)); 
        for (i=0; i<strlen(prof->option43Addr); i++)
            sprintf(showBuf, "%s%02x", showBuf, prof->option43Addr[i]);
    }
    else if (prof->option43Type == DHCP_OPTION_43_TYPE_COSTUM)
    {
        strcpy(showBuf, prof->option43Addr);
        if (prof->option43Addr2[0] != '\0')
            strcat(showBuf, prof->option43Addr2);
    }

    if (strlen(showBuf) > 40)
    {
        showBuf[40] = '.';
        showBuf[41] = '.';
        showBuf[42] = '.';
        showBuf[43] = '\0';
    }
    return ;
}
#endif

static int aspDhcpPoolConfig(int eid, webs_t wp, int argc, char_t **argv)
{
	int min, max, index, arrayNum = 0;
	int min1, max1, index1, exist = 0;
    DhcpPoolProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_DHCP_POOL;
    VlanConfigProfile *profv = NULL;
    MibProfileType mibTypev = MIB_PROF_VLAN_CONFIG;
#if (DHCP_SERVER_OPTION_43 == FYES)
    char addrBuf[DHCP_OPTION_43_LEN*2+1];
    char showBuf[DHCP_OPTION_43_LEN*2+1];
#endif
	websWrite(wp, "var poolNames = new Array();\n");
	websWrite(wp, "var poolSts = new Array();\n");
	websWrite(wp, "var poolVids = new Array();\n");
	websWrite(wp, "var used = new Array();\n");
	websWrite(wp, "var beginIp = new Array();\n");
	websWrite(wp, "var endIp = new Array();\n");
	websWrite(wp, "var netMask = new Array();\n");
	websWrite(wp, "var gateway = new Array();\n");
	websWrite(wp, "var lease = new Array();\n");
	websWrite(wp, "var primaryDns = new Array();\n");
	websWrite(wp, "var backupDns = new Array();\n");
	websWrite(wp, "var vlanConfig = new Array();\n");
	websWrite(wp, "var edit_Names = new Array();\n");
	websWrite(wp, "var vlanNetMask = new Array();\n");
	websWrite(wp, "var vlanIp = new Array();\n");
#if (DHCP_SERVER_OPTION_43 == FYES)
    websWrite(wp, "var dhcpOption43Types = new Array();\n");
    websWrite(wp, "var dhcpOption43Addrs = new Array();\n");
    websWrite(wp, "var dhcpOption43TLV = new Array();\n");
#endif

	ProfInstLowHigh(mibTypev, &max, &min); 
	ProfInstLowHigh(mibType, &max1, &min1);
	for (index = min; index < max; index++)//vlan
    {
        profv = (VlanConfigProfile *)ProfGetInstPointByIndex(mibTypev, index);
        if ((NULL != profv) && (0 == ProfInstIsFree((Pointer)profv)) && (TRUE == profv->head.active)) {
            if (profv->vlanEn == PROF_ENABLE) {
                exist = 0;

                for (index1 = min1 + 1; index1 < max1; index1++)//dhcp
                {
                    prof = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibType, index1);
                    if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)) && (TRUE == prof->head.active)) {
                        if (0 == strcmp(prof->interface, profv->head.name)) {
                            exist = 1;
                            break;
                        }
                    }
                }

                if (exist == 0) {
                    websWrite(wp, "vlanConfig[%d] = \"%s\";\n",arrayNum, profv->head.name);
                    websWrite(wp, "vlanNetMask[%d] = \"%N\";\n", arrayNum, profv->netmask);
		    websWrite(wp, "vlanIp[%d] = \"%N\";\n", arrayNum, profv->ip);
                    arrayNum++;
                }

            }
        }
    }

	ProfInstLowHigh(mibType, &max, &min);

	arrayNum = 0;
	for (index = min; index < max; index++) {
		prof = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibType, index);
		if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)) && (TRUE == prof->head.active)) {
			websWrite(wp, "poolNames[%d] = \"%s\";\n",arrayNum, prof->head.name);
			if (PROF_ENABLE == prof->dhcpEn) {
				websWrite(wp, "used[%d] = \"%d\";\n", arrayNum, 1);
				websWrite(wp, "poolSts[%d] = \"%d\";\n",arrayNum, 1);
			} else {
				websWrite(wp, "used[%d] = \"%d\";\n", arrayNum, 0);
				websWrite(wp, "poolSts[%d] = \"%d\";\n",arrayNum, 0);
			}

			websWrite(wp, "poolVids[%d] = \"%s\";\n",arrayNum, prof->interface);
			websWrite(wp, "beginIp[%d] = \"%N\";\n", arrayNum, prof->ipStart);
			websWrite(wp, "endIp[%d] = \"%N\";\n", arrayNum, prof->ipEnd);
			websWrite(wp, "netMask[%d] = \"%N\";\n", arrayNum, prof->netmask);
			websWrite(wp, "gateway[%d] = \"%N\";\n", arrayNum, prof->gateway);
			websWrite(wp, "lease[%d] = \"%d\";\n", arrayNum, (prof->lease/60));
			websWrite(wp, "primaryDns[%d] = \"%N\";\n", arrayNum, prof->pDns);
			websWrite(wp, "backupDns[%d] = \"%N\";\n", arrayNum, prof->sDns);
#if (DHCP_SERVER_OPTION_43 == FYES)
            memset(addrBuf, 0, sizeof(addrBuf));
            memset(showBuf, 0, sizeof(showBuf));
           
            strcpy(addrBuf, prof->option43Addr);
            if (prof->option43Addr2[0] != '\0')
                strcat(addrBuf, prof->option43Addr2);
            generateOption43TLV(arrayNum, prof, showBuf);
           
            websWrite(wp, "dhcpOption43=\"%s\";\n", "on");
            websWrite(wp, "dhcpOption43Types[%d] =\"%d\";\n",arrayNum, prof->option43Type);
            websWrite(wp, "dhcpOption43Addrs[%d] =\"%s\";\n",arrayNum, addrBuf);
            websWrite(wp, "dhcpOption43TLV[%d] =\"%s\";\n",arrayNum, showBuf);
#else
            websWrite(wp, "dhcpOption43=\"%s\";\n", "off");
#endif
			arrayNum++;
		}
	}

	websWrite(wp, "var totalrecs = %d;\n", arrayNum);
	websWrite(wp, "var max_totalrecs = %d;\n", MAX_DHCP_POOL_PROFILES);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

static void formDhcpPoolDelAll(webs_t wp, char_t *path, char_t *query)
{
	int min, max, index = 0,change = 0;
	MibProfileType mibType = MIB_PROF_DHCP_POOL;
	DhcpPoolProfile *prof = NULL;
	const char *ErrMsg = NULL;

	ProfInstLowHigh(mibType, &max, &min);

	doSystem("echo -n >/etc/udhcpd_vlan.conf");
	for (index = (min + 1); index < max; index++) {
		prof = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibType, index);
		if (NULL != prof) {
		#if 0
			if (PROF_ENABLE == prof->dhcpEn) {
				ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_DEL_USEING_ENTRY);
				setTheErrorMsg((char *)ErrMsg);
				//setTheErrorMsg(T("不能删除使用中的选项"));
            } else
		#endif
			if(prof->head.name[0] != '\0')
			{
                UserProfile *profUser = NULL;
                int minUser = 0, maxUser = 0, indexUser = 0;
                int del_it = 1;

                ProfInstLowHigh(MIB_PROF_USER, &maxUser, &minUser);

                for (indexUser = minUser; indexUser < maxUser; indexUser++) {
                    profUser = (UserProfile*)ProfGetInstPointByIndex(MIB_PROF_USER, indexUser);
                    if(profUser != NULL && ProfInstIsFree(profUser) == 0) {
                        if ( !strcmp(prof->head.name,profUser->dhcpPoolName) ){
                            del_it = 0;
                            break;
                        }
                    }
                }

                if ( !del_it ){
                    //ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_DEL_USEING_ENTRY);
                    //setTheErrorMsg((char *)ErrMsg);
                    setTheErrorMsg(T("不能删除含有静态绑定成员的选项"));
                }else{
					doSystem("echo 'del_vlan %d' >>/etc/udhcpd_vlan.conf",prof->vid);
                    ProfDelInstByIndex(mibType, index);
					change = 1;
                }
            }
	   }
	}
	if(change == 1)
	{
		doSystem("killall -q -USR2 udhcpd");
		nvramWriteCommit();
	}
	websRedirect(wp, "DhcpPoolForAc.asp");
	return;
}

static void formDhcpPoolDel(webs_t wp, char_t *path, char_t *query)
{
	char *delStr = NULL, *tmp;
	int min, max, index = 0, change = 0;
	MibProfileType mibType = MIB_PROF_DHCP_POOL;
	DhcpPoolProfile *prof = NULL;
	const char *ErrMsg = NULL;

	delStr = websGetVar(wp,T("delstr"),T(""));
	if (NULL == delStr || '\0' == *delStr) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_GROUP);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("找不到删除的组"));
		goto out;
	}

	ProfInstLowHigh(mibType, &max, &min);

	tmp = strtok(delStr, UTT_SPLIT_SIGN_STR);
	doSystem("echo -n >/etc/udhcpd_vlan.conf");
	while(tmp != NULL) {
		for (index = min; index < max; index++) {
			prof = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibType, index);
			if (NULL != prof) {
				if (!strcmp(tmp, "default")) {
					ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_DEL_DEFAULT);
					setTheErrorMsg((char *)ErrMsg);
					//setTheErrorMsg(T("不能删除默认选项"));
                } else if (!strcmp(prof->head.name, tmp)) {
				#if 0
                    if (PROF_ENABLE == prof->dhcpEn) {
                        ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_DEL_USEING_ENTRY);
                        setTheErrorMsg((char *)ErrMsg);
                        //setTheErrorMsg(T("不能删除使用中的选项"));
                    } else
				#endif
					{
                        UserProfile *profUser = NULL;
                        int minUser = 0, maxUser = 0, indexUser = 0;
                        int del_it = 1;

                        ProfInstLowHigh(MIB_PROF_USER, &maxUser, &minUser);

                        for (indexUser = minUser; indexUser < maxUser; indexUser++) {
                            profUser = (UserProfile*)ProfGetInstPointByIndex(MIB_PROF_USER, indexUser);
                            if(profUser != NULL && ProfInstIsFree(profUser) == 0) {
                                if ( !strcmp(prof->head.name,profUser->dhcpPoolName) ){
                                    del_it = 0;
                                    break;
                                }
                            }
                        }

                        if ( !del_it ){
                            //ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_DEL_USEING_ENTRY);
                            //setTheErrorMsg((char *)ErrMsg);
                             setTheErrorMsg(T("不能删除含有静态绑定成员的选项"));
                        }else{
							doSystem("echo 'del_vlan %d' >>/etc/udhcpd_vlan.conf",prof->vid);
                            ProfDelInstByIndex(mibType, index);
                            change = 1;
                        }
                    }
				}
			}
		}

		tmp = strtok(NULL, UTT_SPLIT_SIGN_STR);
	}

	if (1 == change) {
		doSystem("killall -q -USR2 udhcpd");
		nvramWriteCommit();
	}

out:
	websRedirect(wp, "DhcpPoolForAc.asp");
	return;
}

static int aspNewDhcpCli(int eid, webs_t wp, int argc, char_t **argv)
{
	int arrayNum = 0;
    DhcpPoolProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_DHCP_POOL;
	int max, min, index;

	websWrite(wp, "var poolNames = new Array();\n");
	websWrite(wp, "var poolVids = new Array();\n");
	websWrite(wp, "var beginIp = new Array();\n");
	websWrite(wp, "var endIp = new Array();\n");

	ProfInstLowHigh(mibType, &max, &min);

	for (index = min; index < max; index++) {
		prof = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibType, index);
		if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)) && (TRUE == prof->head.active)) {
			websWrite(wp, "poolNames[%d] = \"%s\";\n", arrayNum, prof->head.name);
			websWrite(wp, "poolVids[%d] = \"%d\";\n", arrayNum, prof->vid);
			websWrite(wp, "beginIp[%d] = \"%N\";\n", arrayNum, prof->ipStart);
			websWrite(wp, "endIp[%d] = \"%N\";\n", arrayNum, prof->ipEnd);

			arrayNum++;
		}
	}

    getTheErrorMsg(eid, wp, argc, argv);

	return 0;
}
static int aspDhcpCliList(int eid, webs_t wp, int argc, char_t **argv)
{
    FILE *fp;
    int ct=0;
    char tmpmac[256];
    DhcpPoolProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_DHCP_POOL;
    int i, min = 0, max = 0, index;
    struct in_addr addr;
    unsigned long expires;
    const char *day=NULL, *hour = NULL, *minN = NULL, *sec = NULL;
    strTime_t * TimeData = NULL;

    day = getTransDataForC(C_LANG_INDEX_DAY);
    hour = getTransDataForC(C_LANG_INDEX_HOUR);
    minN = getTransDataForC(C_LANG_INDEX_MINUTE);
    sec = getTransDataForC(C_LANG_INDEX_SECOND);

    char_t *vids=T("UserVids[%d]=\"%d\";");
    char_t *ips=T("UserIPs[%d]=\"%s\";");
    char_t *macs=T("UserMacs[%d]=\"%s\";");
    char_t *mask=T("UserMasks[%d]=\"%N\";");
  //  char_t *lefttime=T("UserLeftTimes[%d]=\"%d时%d分%d秒\";");

    websWrite(wp,"var UserVids=new Array();");
    websWrite(wp,"var UserIPs=new Array();");
    websWrite(wp,"var UserMacs=new Array();");
    websWrite(wp,"var UserMasks=new Array();");
    websWrite(wp,"var UserLeftTimes=new Array();");
    prof = (DhcpPoolProfile*)ProfGetInstPointByIndex(mibType, 0);

    struct dhcpOfferedAddr {
		unsigned char hostname[16];
		unsigned char mac[16];
		unsigned long ip;
		unsigned long expires;
		unsigned int  vid;
    } lease;

    doSystem("killall -q -USR1 udhcpd");
#if 0
	ProfInstLowHigh(mibType, &max, &min);

	for (index = min; index < max; index++) {
		prof = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibType, index);
		if ((prof != NULL) && (ProfInstIsFree((Pointer)prof) == 0) &&
			(prof->head.active == TRUE) && (prof->dhcpEn == PROF_ENABLE)) {
			memset(tmpmac, '\0', sizeof(tmpmac));
			memset(file_name, '\0', sizeof(file_name));
			memset(&lease, '\0', sizeof(lease));

			sprintf(file_name, "/var/udhcpd.%s.leases", prof->interface);
//			printf("file_name = %s\n", file_name);

			fp = fopen(file_name, "r");
			if (fp != NULL) {
				while (fread(&lease, 1, sizeof(lease), fp) == sizeof(lease)) {
					for (i = 0; i < 6; i++){
						sprintf(tmpmac,"%s%02X",tmpmac, lease.mac[i]);
					}

					Conversion2StdMac(tmpmac);
					websWrite(wp, macs,ct, tmpmac);
					addr.s_addr = lease.ip;
					websWrite(wp,ips ,ct, inet_ntoa(addr));
					websWrite(wp,mask,ct,prof->netmask);
					expires = ntohl(lease.expires);
					websWrite(wp,lefttime ,ct, expires);
					memset(tmpmac,0,strlen(tmpmac));
					ct++;
				}

				fclose(fp);
			}

		}
	}
#else

	fp = fopen(DHCP_LEASE_FILE, "r");
	if (fp != NULL) {
		while (fread(&lease, 1, sizeof(lease), fp) == sizeof(lease)) {
		memset(tmpmac,0,strlen(tmpmac));
			for (i = 0; i < 6; i++){
				sprintf(tmpmac,"%s%02X",tmpmac, lease.mac[i]);
			}

		websWrite(wp,vids ,ct, ntohl(lease.vid));
		Conversion2StdMac(tmpmac);
		websWrite(wp, macs,ct, tmpmac);
		addr.s_addr = lease.ip;
		websWrite(wp,ips ,ct, inet_ntoa(addr));
		websWrite(wp,mask,ct,prof->netmask);
		expires = ntohl(lease.expires);
		TimeData = convertS2HMS(expires);
		websWrite(wp,"UserLeftTimes[%d]=\"%d%s%d%s%d%s%d%s\";\n",ct,TimeData->day,day,TimeData->hour,hour,TimeData->min,minN,TimeData->sec,sec);
		ct++;
		}

		fclose(fp);
	}
#endif
    websWrite(wp,"var totalrecs=%d;",ct);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

static int getDhcpList_static(int eid, webs_t wp, int argc, char_t **argv)
{
    int min = 0, max = 0;
    int i = 0;
    int totalrecs = 0;
    MibProfileType mibType = MIB_PROF_USER;
    UserProfile *prof;
	char_t *poolName = T("pool[%d] = \"%s\";");
    char_t *UserNames=T("FixUserNames[%d] = \"%s\";");
    char_t *vids=T("FixVids[%d] = \"%d\";");
    char_t *ips =T("FixIPs[%d] = \"%N\";");
    char_t *macs = T("FixMacs[%d] = \"%s\"; ");

	websWrite(wp, "var pool = new Array();");
    websWrite(wp,"var FixUserNames=new Array();");
    websWrite(wp,"var FixVids = new Array();");
    websWrite(wp,"var FixIPs = new Array();");
    websWrite(wp,"var FixMacs = new Array();");

    ProfInstLowHigh(mibType, &max, &min);

    for(i = min; i < max; i++) {
        prof = (UserProfile*)ProfGetInstPointByIndex(mibType, i);
        if(prof != NULL && ProfInstIsFree(prof) == 0) {
			websWrite(wp, poolName, totalrecs, prof->dhcpPoolName);
            websWrite(wp, UserNames, totalrecs, prof->head.name);
            websWrite(wp, vids, totalrecs, prof->vid);
            websWrite(wp, ips, totalrecs, prof->ip);
#if 0
	    Conversion2StdMac(prof->userMac);
#endif
            websWrite(wp, macs, totalrecs, converMac6To12Byte(prof->mac));
            totalrecs++;
        }
    }

    websWrite(wp,"var totalrecs=%d;", totalrecs);
    websWrite(wp,"var max_totalrecs=%d;", max);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

static int getDhcpOneClientInfo(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t *edit_name = NULL;
    UserProfile *prof;
    MibProfileType mibType = MIB_PROF_USER;
    const char *ErrMsg = NULL;

    if(ejArgs(argc,argv,T("%s"), &edit_name) < 1) {
	//setTheErrorMsg(T("读取edit_Name变量失败!"));
        goto out;
    }

    if(edit_name == NULL) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_NOT_NULL);
	setTheErrorMsg((char *)ErrMsg);
        //setTheErrorMsg(T("用户名不能为空!"));
        goto out;
    }

    prof = (UserProfile*)ProfGetInstPointByName(mibType, edit_name);
    if(prof == NULL) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_NOT_FIND);
	setTheErrorMsg((char *)ErrMsg);
        //setTheErrorMsg(T("找不到该用户!"));
    } else {
		websWrite(wp, "var pool = \"%s\";", prof->dhcpPoolName);
        websWrite(wp, "var FixUserNames=\"%s\";", prof->head.name);
        websWrite(wp, "var FixVids=\"%d\";", prof->vid);
        websWrite(wp, "var FixIPs=\"%N\";", prof->ip);
        websWrite(wp, "var FixMacs=\"%s\";", converMac6To12Byte(prof->mac));
    }

out:
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

static void formDhcpListStaticDel(webs_t wp, char_t *path, char_t *query)
{
#if 0
    char_t *UserName=NULL, *tmp = NULL;
    MibProfileType mibType = MIB_PROF_USER;
    struProfAlloc *profList  = NULL;
    int back = 0;
    a_assert(wp);

    UserName = websGetVar(wp,T("delstr"),T(""));
    if(UserName == NULL) {
        setTheErrorMsg(T("找不到该用户名!"));
        goto out;
    }

    tmp=strtok(UserName, UTT_SPLIT_SIGN_STR);
    while(tmp != NULL) {
        if(back == 0) {
            ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profList);
            back = 1;
        }
        ProfDelInstByName(mibType, tmp);
        tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
    }

    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();

#endif
    webArpBindDel(wp);

    websRedirect(wp,"DhcpFixForAc.asp");
}

static void formDhcpListStaticDelAll(webs_t wp, char_t *path, char_t *query)
{
#if 0
    int min = 0, max = 0;
    int i = 0;
    int back = 0;
    MibProfileType mibType = MIB_PROF_USER;
    Pointer prof;
    struProfAlloc *profList  = NULL;

    a_assert(wp);
    ProfInstLowHigh(mibType, &max, &min);

    for(i = min; i < max; i++) {
        prof = ProfGetInstPointByIndex(mibType, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
            if(back == 0) {
                ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, i, &profList);
                back = 1;
            }
            ProfDelInstByIndex(mibType, i);
        }
    }
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
#endif
    webArpBindDelAll(wp);
    websRedirect(wp, "DhcpFixForAc.asp");
}

static void formConfigDhcpList_static(webs_t wp, char_t *path, char_t *query)
{
    char_t *ip, *pools,*vid;

    struct in_addr addr;
    DhcpPoolProfile *prof = NULL;
	const char *ErrMsg = NULL;

    a_assert(wp);

    ip = websGetVar(wp,T("IP"), T(""));
	pools = websGetVar(wp, T("pools"), T(""));
	if (strcmp(pools, "")) {
		prof = (DhcpPoolProfile *)ProfGetInstPointByName(MIB_PROF_DHCP_POOL, pools);
	    if((inet_aton(ip, &addr) == 0) ||
	            ((addr.s_addr & prof->netmask) != (prof->ipStart & prof->netmask))) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_IP_PLAN_ADDR_ERR);
			setTheErrorMsg((char *)ErrMsg);

	        //setTheErrorMsg(T("IP 地址错误!"));
			goto out;
	    }

		if((ntohl(addr.s_addr) > ntohl(prof->ipEnd)) || (ntohl(addr.s_addr) < ntohl(prof->ipStart))) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_OVERFLOW_IP_ADDR);
			setTheErrorMsg((char *)ErrMsg);
//	setTheErrorMsg(T("IP 地址超出范围!"));
			goto out;
			}
	} else {
			ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_POOL_NOTEDIT);
			setTheErrorMsg((char *)ErrMsg);

	   // setTheErrorMsg(T("此选项不可编辑"));
		goto out;
	}


    webConfigArpBindConfig(wp);
out:
    websRedirect(wp, "DhcpFixForAc.asp");
}

void formDefineDhcpPool(void)
{
	websAspDefine(T("aspDhcpServerConfig"), aspDhcpServerConfig);
	websFormDefine(T("formDhcpServer"), formDhcpServer);
    websAspDefine(T("aspDhcpPoolConfig"), aspDhcpPoolConfig);
    websFormDefine(T("formDhcpPoolConfig"), formDhcpPoolConfig);
	websFormDefine(T("formDhcpPoolDel"), formDhcpPoolDel);
	websFormDefine(T("formDhcpPoolDelAll"), formDhcpPoolDelAll);

    websAspDefine(T("aspNewDhcpCli"), aspNewDhcpCli);
    websAspDefine(T("aspOutDhcpOneClientInfo"), getDhcpOneClientInfo);
    websAspDefine(T("outputDhcpStaticList"), getDhcpList_static);
    websFormDefine(T("formDhcpListStaticDel"),formDhcpListStaticDel);
    websFormDefine(T("formDhcpListStaticDelAll"),formDhcpListStaticDelAll);
    websFormDefine(T("formDhcpListStatic"), formConfigDhcpList_static);

    websAspDefine(T("aspDhcpCliList"), aspDhcpCliList);
}
#endif
