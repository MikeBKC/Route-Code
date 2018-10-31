
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

#if (VLAN_CONFIG == FYES)
#define DEBUG_CK 0

#if DEBUG_CK
	#define debug() do { \
						printf("debug: %d in %s()\n", __LINE__, __func__); \
						} while (0);
#else
	#define debug()
#endif
static int aspVlanConfig(int eid, webs_t wp, int argc, char_t **argv)
{
	debug();
	int min, max, index, arrayNum = 0;
    VlanConfigProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_VLAN_CONFIG;

	websWrite(wp, "var poolNames = new Array();\n");
	websWrite(wp, "var poolSts = new Array();\n");
	websWrite(wp, "var poolVids = new Array();\n");
	websWrite(wp, "var used = new Array();\n");
	websWrite(wp, "var beginIp = new Array();\n");
	websWrite(wp, "var netMask = new Array();\n");

	ProfInstLowHigh(mibType, &max, &min);

	for (index = min; index < max; index++) {
		prof = (VlanConfigProfile *)ProfGetInstPointByIndex(mibType, index);
		if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)) && (TRUE == prof->head.active)) {
			websWrite(wp, "poolNames[%d] = \"%s\";\n",arrayNum, prof->head.name);
			if (PROF_ENABLE == prof->vlanEn) {
				websWrite(wp, "used[%d] = \"%d\";\n", arrayNum, 1);
				websWrite(wp, "poolSts[%d] = \"%d\";\n",arrayNum, 1);
			} else {
				websWrite(wp, "used[%d] = \"%d\";\n", arrayNum, 0);
				websWrite(wp, "poolSts[%d] = \"%d\";\n",arrayNum, 0);
			}

			websWrite(wp, "poolVids[%d] = \"%d\";\n",arrayNum, prof->vid);
			websWrite(wp, "beginIp[%d] = \"%N\";\n", arrayNum, prof->ip);
			websWrite(wp, "netMask[%d] = \"%N\";\n", arrayNum, prof->netmask);

			arrayNum++;
		}
	}

	websWrite(wp, "var totalrecs = %d;\n", arrayNum);
	websWrite(wp, "var max_totalrecs = %d;\n", MAX_VLAN_CONFIG_PROFILES);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

static void formVlanConfig(webs_t wp, char_t *path, char_t *query)
{
	debug();
    char_t  *new_name, *old_name, *dhcp_s, *dhcp_m,
			*dhcp_vid,*dhcp_en;
    Uint32 tmpNum = 0, lanIp = 0;
	int max, min, index, indexTmp;
	ProfChangeType profType = PROF_CHANGE_ADD;
    unsigned int nMaxIp;
    MibProfileType mibType = MIB_PROF_VLAN_CONFIG;
#if (MULTI_LAN == FYES)
	Uint32 lanIp1 = 0, lanIp2 = 0, lanIp3 = 0;
#endif

#if 0//(DHCP_POOL == FYES)
    unsigned int nIp, eIp, uNet, nIp1, eIp1;
	int index1 = 0;
    struProfAlloc *profheadD = NULL;
    MibProfileType mibTypeDhcpP = MIB_PROF_DHCP_POOL;
	DhcpPoolProfile *dhcpPool = NULL;
#endif

    VlanConfigProfile *prof = NULL;
    struct in_addr addrS, addrE, addrNm, addrGw;
    InterfaceProfile *profIf = NULL;
    struProfAlloc *profhead = NULL;
    IpPoolEnum cfPoolType = IP_POOL_NONE;
    const char *ErrMsg = NULL;

	new_name = websGetVar(wp, T("poolName"), T(""));
	old_name = websGetVar(wp, T("oldName"), T(""));
	dhcp_en = websGetVar(wp, T("DhcpEnable"), T(""));
	dhcp_vid = websGetVar(wp, T("dhcpVid"), T(""));
    dhcp_s = websGetVar(wp, T("dhcpStart"), T(""));
    dhcp_m = websGetVar(wp, T("dhcpMask"), T(""));
#ifdef  CONFIG_BOARD_P1010_C
#ifndef CONFIG_VLAN_BOARD_P1010_C
#error "please open CONFIG_VLAN_BOARD_P1010_C in linux-config for router-on-a-stick !"
#endif
    Uint32 vid=0;
    vid = strtol(dhcp_vid, NULL, 10);
    if(vid == 1 || vid == 5){
	setTheErrorMsg(T("VID 1和5 已被系统使用,不支持配置！"));
	goto out;
    }
#endif

    if (0 == inet_aton(dhcp_s, &addrS)) {
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_START_IP);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("无效起始地址"));
		goto out;
    }
    if (0 == inet_aton(dhcp_m, &addrNm)) {
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_SUBMASK);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("无效子网掩码"));
		goto out;
    }

	debug();
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);
    lanIp = ntohl(profIf->ether.sta.ip);
    if(lanIp == ntohl(addrS.s_addr)){
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_VIF_INCLUDE_LAN);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("DHCP地址池包含了LAN口地址"));
		goto out;
    }
#if (MULTI_LAN == FYES)
	debug();
    lanIp1 = ntohl(profIf->ether.sta.ip2);
    if((lanIp1 == ntohl(addrS.s_addr))) {
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_VIF_INCLUDE_LAN);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("DHCP地址池包含了LAN口地址"));
		goto out;
    }

    lanIp2 = ntohl(profIf->ether.sta.ip3);
    if((lanIp2 == ntohl(addrS.s_addr))) {
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_VIF_INCLUDE_LAN);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("DHCP地址池包含了LAN口地址"));
		goto out;
    }

    lanIp3 = ntohl(profIf->ether.sta.ip4);
    if((lanIp3 == ntohl(addrS.s_addr))) {
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_VIF_INCLUDE_LAN);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("DHCP地址池包含了LAN口地址"));
		goto out;
    }
#endif

	debug();
	ProfInstLowHigh(mibType, &max, &min);
	if ('\0' == *old_name) {
	debug();
		/*vid是否冲突*/
		for (index = min; index < max; index++) {
			prof = (VlanConfigProfile *)ProfGetInstPointByIndex(mibType, index);
			if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)) && (TRUE == prof->head.active)) {
				if (prof->vid == strtol(dhcp_vid, NULL, 10)) {
					//ErrMsg = getTransDataForC(C_LANG_INDEX_SAME_VID_EXIST);
					//setTheErrorMsg((char *)ErrMsg);
					//setTheErrorMsg(T("操作失败，系统中存在相同VID的地址池"));
					setTheErrorMsg(T("Failed, Same VID Exist!"));
					goto out;
				}

				if ((prof->ip & prof->netmask) == (addrS.s_addr & prof->netmask)) {
					//setTheErrorMsg(T("操作失败，虚接口地址网段重复！"));
					setTheErrorMsg(T("Failed, Duplicate Entry!"));
					goto out;
				}

				if ((prof->ip & addrNm.s_addr) == (addrS.s_addr & addrNm.s_addr)) {
				//	setTheErrorMsg(T("操作失败，虚接口地址网段重复！"));
					setTheErrorMsg(T("Failed, Duplicate Entry!"));
					goto out;
				}
			}
		}

		index = ProfNewInst(mibType, new_name, FALSE);
		profType = PROF_CHANGE_ADD;
	} else {
	debug();
		indexTmp = ProfGetInstIndexByName(mibType, old_name);
		if (indexTmp == PROFFAIL) {
			goto out;
		}

		/*vid是否冲突*/
		for (index = min; index < max; index++) {
			if (index != indexTmp) {
				prof = (VlanConfigProfile *)ProfGetInstPointByIndex(mibType, index);
				if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)) && (TRUE == prof->head.active)) {
					if (prof->vid == strtol(dhcp_vid, NULL, 10)) {
						//ErrMsg = getTransDataForC(C_LANG_INDEX_SAME_VID_EXIST);
						//setTheErrorMsg((char *)ErrMsg);
						setTheErrorMsg(T("操作失败，系统中存在相同VID的地址池"));
						goto out;
					}

					if ((prof->ip & prof->netmask) == (addrS.s_addr & prof->netmask)) {
						debug();
						setTheErrorMsg(T("操作失败，虚接口地址网段重复！"));
						goto out;
					}

					if ((prof->ip & addrNm.s_addr) == (addrS.s_addr & addrNm.s_addr)) {
						debug();
						setTheErrorMsg(T("操作失败，虚接口地址网段重复！"));
						goto out;
					}
				}

			}
		}

	debug();
		index = indexTmp;
		profType = PROF_CHANGE_EDIT;

	}

	debug();
		prof = (VlanConfigProfile *)ProfGetInstPointByIndex(mibType, index);
		if (NULL != prof) {
	debug();
#if (DHCP_POOL == FYES)
#if 0
		debug();
		if (profType == PROF_CHANGE_EDIT) {
			ProfInstLowHigh(mibTypeDhcpP, &max, &min);
			for (index1 = min + 1; index1 < max; index1++) {
				dhcpPool = (DhcpPoolProfile*)ProfGetInstPointByIndex(mibTypeDhcpP, index1);
				if ((NULL != dhcpPool) && (0 == ProfInstIsFree((Pointer)dhcpPool)) && (TRUE == dhcpPool->head.active)) {
					if (0 == strcmp(dhcpPool->interface, prof->head.name)) {
					debug();
						uNet = ntohl(addrS.s_addr) & 0xffffff00;
						nIp = ntohl(dhcpPool->ipStart);
						eIp = ntohl(dhcpPool->ipEnd);
						nIp1 = uNet + (nIp & 0x000000ff);
						eIp1 = uNet + (eIp & 0x000000ff);

						dhcpPool->ipStart = htonl(nIp1);
						dhcpPool->ipEnd = htonl(eIp1);
						dhcpPool->netmask = addrNm.s_addr;
						dhcpPool->gateway = addrS.s_addr;
						strcpy(dhcpPool->interface, new_name);

						ProfBackupByIndex(mibTypeDhcpP, profType, index1, &profheadD);
						ProfUpdate(profheadD);
						ProfFreeAllocList(profheadD);
						break;
					}
				}
			}
		}
#endif
#endif

			ProfBackupByIndex(mibType, profType, index, &profhead);
			strcpy(prof->head.name, new_name);
			if(strcmp(dhcp_en,"on") == 0)
			{
				prof->vlanEn = PROF_ENABLE;
			}else{
				prof->vlanEn = PROF_DISABLE;
			}
			prof->vid = strtol(dhcp_vid, NULL, 10);
			prof->ip = addrS.s_addr;
			prof->netmask = addrNm.s_addr;
		}



	debug();
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
	debug();
out:
    websRedirect(wp,"vlanConfig.asp");
	return;
}

static void formVlanConfigDel(webs_t wp, char_t *path, char_t *query)
{
	debug();
	char *delStr = NULL, *tmp;
	int min, max, index = 0, change = 0, isUsed = 0;
	MibProfileType mibType = MIB_PROF_VLAN_CONFIG;
	VlanConfigProfile *prof = NULL;
	struProfAlloc *profhead = NULL;
	const char *ErrMsg = NULL;

    MibProfileType mibTypeD = MIB_PROF_DHCP_POOL;
    DhcpPoolProfile *profD = NULL;
	int minD, maxD, indexD = 0;

	delStr = websGetVar(wp,T("delstr"),T(""));
	if (NULL == delStr || '\0' == *delStr) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_GROUP);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("找不到删除的组"));
		goto out;
	}

	ProfInstLowHigh(mibType, &max, &min);
	ProfInstLowHigh(mibTypeD, &maxD, &minD);

	tmp = strtok(delStr, UTT_SPLIT_SIGN_STR);
	while(tmp != NULL) {
		for (index = min; index < max; index++) {
			prof = (VlanConfigProfile *)ProfGetInstPointByIndex(mibType, index);
			if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)) && (TRUE == prof->head.active)) {
				if (!strcmp(prof->head.name, tmp)) {
					{
						isUsed = 0;
						for (indexD = minD; indexD < maxD; indexD++) {
							profD = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibTypeD, indexD);
							if (profD != NULL) {
								if (strcmp(prof->head.name, profD->interface) == 0) {
									setTheErrorMsg(T("已被引用的虚接口无法删除！"));
//									ErrMsg = getTransDataForC(C_LANG_INDEX_CAN_NOT_DEL_VIF_USED);
//									setTheErrorMsg((char *)ErrMsg);
									isUsed = 1;
									break;
								}
							}
						}

						if (isUsed == 0) {
							debug();
							ProfBackupByIndex(mibType, PROF_CHANGE_DEL, index, &profhead);
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
		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
		nvramWriteCommit();
	}

out:
	websRedirect(wp, "vlanConfig.asp");
	return;
}

static void formVlanConfigDelAll(webs_t wp, char_t *path, char_t *query)
{
	int min, max, index = 0,change = 0, isUsed = 0;
	MibProfileType mibType = MIB_PROF_VLAN_CONFIG;
	VlanConfigProfile *prof = NULL;
	struProfAlloc *profhead = NULL;
	const char *ErrMsg = NULL;

    MibProfileType mibTypeD = MIB_PROF_DHCP_POOL;
    DhcpPoolProfile *profD = NULL;
	int minD, maxD, indexD = 0;

	ProfInstLowHigh(mibType, &max, &min);
	ProfInstLowHigh(mibTypeD, &maxD, &minD);

	for (index = min; index < max; index++) {
		prof = (VlanConfigProfile *)ProfGetInstPointByIndex(mibType, index);
		if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)) && (TRUE == prof->head.active)) {
			isUsed = 0;
			for (indexD = minD; indexD < maxD; indexD++) {
				profD = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibTypeD, indexD);
				if (profD != NULL) {
					if (strcmp(prof->head.name, profD->interface) == 0) {
#if 0
						ErrMsg = getTransDataForC(C_LANG_INDEX_CAN_NOT_DEL_VIF_USED);
						setTheErrorMsg((char *)ErrMsg);
#endif
						setTheErrorMsg(T("已被引用的虚接口无法删除！"));
						isUsed = 1;
						break;
					}
				}
			}

			if (isUsed == 0) {
				ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, index, &profhead);
				ProfDelInstByIndex(mibType, index);

				change = 1;
			}
		}
	}

	if(change == 1)
	{
		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
		nvramWriteCommit();
	}
	websRedirect(wp, "vlanConfig.asp");
	return;
}

static int getOneVlanConfig(int eid, webs_t wp, int argc, char **argv)
{
	int index, min, max, arrayNum = 0;
	char *edit_Name;
	MibProfileType mibType = MIB_PROF_VLAN_CONFIG;
	VlanConfigProfile *prof = NULL;

	ejArgs(argc, argv, T("%s"), &edit_Name);
	websWrite(wp, "var poolNames = \"\";\n");
	websWrite(wp, "var poolSts = \"\";\n");
	websWrite(wp, "var poolVids = \"\";\n");
	websWrite(wp, "var used = \"\";\n");
	websWrite(wp, "var beginIp = \"\";\n");
	websWrite(wp, "var netMask = \"\";\n");
	websWrite(wp, "var errorstr = \"\";\n");


	prof = ProfGetInstPointByName(mibType, edit_Name);
	if (prof != NULL) {
			websWrite(wp, "poolNames = \"%s\";\n", prof->head.name);
			if (PROF_ENABLE == prof->vlanEn) {
				websWrite(wp, "used = \"%d\";\n", 1);
				websWrite(wp, "poolSts = %d;\n", 1);
			} else {
				websWrite(wp, "used = \"%d\";\n", 0);
				websWrite(wp, "poolSts = %d;\n", 0);
			}

			websWrite(wp, "poolVids = \"%d\";\n", prof->vid);
			websWrite(wp, "beginIp = \"%N\";\n",  prof->ip);
			websWrite(wp, "netMask = \"%N\";\n",  prof->netmask);
	}

	getTheErrorMsg(eid, wp, argc, argv);
	return 0;
}

void formDefineVlanConfig(void)
{
    websAspDefine(T("aspVlanConfig"), aspVlanConfig);
    websAspDefine(T("getOneVlanConfig"), getOneVlanConfig);
    websFormDefine(T("formVlanConfig"), formVlanConfig);
	websFormDefine(T("formVlanConfigDel"), formVlanConfigDel);
	websFormDefine(T("formVlanConfigDelAll"), formVlanConfigDelAll);
}
#endif
