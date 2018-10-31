
#include <string.h>
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "mib.h"
#include "profacce.h"
#include "uttfunction.h"
#include "wuScript.h"
#include "translate.h"

#if (TAG_VLAN == FYES)

#define DEBUG_TV  0

#if DEBUG_TV
#define debug() printf("debug: %d in %s()\n", __LINE__, __func__)
#else
#define debug()
#endif

static void formTagVlanConfig(webs_t wp, char_t *path, char_t *query)
{
    char *newName, *oldName, *vid, *gw, *netmask, *dhcpEn, *dhcpPool;
    char *mbrMsk = NULL, *untagMsk = NULL;
    Uint32 mbrMsk32 = 0, untagMsk32 = 0;
    int min, max, index, indexTmp;
    Uint32 vidVal;
    struct in_addr addrGw, addrNm;
    struProfAlloc *profhead = NULL;
    MibProfileType mibType = MIB_PROF_TAG_VLAN;
    ProfChangeType profType;
    TagVlanProfile *prof = NULL;
    const char *ErrMsg = NULL;
#if (DHCP_POOL == FYES)
    MibProfileType mibTypeDhcp = MIB_PROF_DHCP_POOL;
    struProfAlloc *profheadDhcp = NULL;
    DhcpPoolProfile *profDhcp = NULL;
#endif

    newName = websGetVar(wp, "vlanName", T(""));
    oldName = websGetVar(wp, "oldName", T(""));
    mbrMsk = websGetVar(wp, "hidmember", T("0"));
    untagMsk = websGetVar(wp, "hidrule", T("0"));
    mbrMsk32 = strtoul(mbrMsk, NULL, 10);
    untagMsk32 = strtoul(untagMsk, NULL, 10);
    vid = websGetVar(wp, "vlanId", T(""));
#if (DHCP_POOL == FYES)
    gw = websGetVar(wp, "ipAddr", T(""));
    netmask = websGetVar(wp, "maskAddr", T(""));
    dhcpEn = websGetVar(wp, "dhcpServer", T(""));
    dhcpPool = websGetVar(wp, "pool", T("default"));
#endif

    if ('\0' == *vid) {
	vidVal = 0;
    } else {
	vidVal = (Uint16)strtoul(vid, NULL, 10);
    }
    if (vidVal > 4094 || vidVal < 6 ) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_VLAN_ID_OUT_OF_RANGE);
	setTheErrorMsg((char *)ErrMsg);
	//setTheErrorMsg(T("VLAN ID超出取值范围"));
	goto out;
    }
    if (strlen(newName) > (UTT_INST_NAME_LEN - 1)) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_TV_NAME_OUT_OF_RANGE);
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
    if (0 == inet_aton(netmask, &addrNm)) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_SUBMASK);
	setTheErrorMsg((char *)ErrMsg);
	//setTheErrorMsg(T("无效子网掩码"));
	goto out;
    }

    ProfInstLowHigh(mibType, &max, &min);

    if ('\0' == *oldName) {
	/* add new an option*/
	debug();
	for (index = min; index < max; index++) {
	    prof = (TagVlanProfile *)ProfGetInstPointByIndex(mibType, index);
	    if (NULL != prof) {
		if (!strcmp(newName, prof->head.name)) {
		    ErrMsg = getTransDataForC(C_LANG_INDEX_TV_NAME_CONFLICT);
		    setTheErrorMsg((char *)ErrMsg);
		    //setTheErrorMsg("操作失败，服务区名称冲突");
		    goto out;
		}
		if (vidVal == prof->vid) {
		    ErrMsg = getTransDataForC(C_LANG_INDEX_VLAN_ID_CONFLICT);
		    setTheErrorMsg((char *)ErrMsg);
		    //setTheErrorMsg("操作失败，VLAN ID冲突");
		    goto out;
		}
	    }
	}

#if (DHCP_POOL == FYES)
	if (0 == strcmp(dhcpEn, "1")) {
	    debug();
	    profDhcp = (DhcpPoolProfile *)ProfGetInstPointByName(mibTypeDhcp, dhcpPool);
	    if (NULL != profDhcp) {
		if (PROF_DISABLE == profDhcp->dhcpEn) {
		    ProfBackupByName(mibTypeDhcp, PROF_CHANGE_EDIT, dhcpPool, &profheadDhcp);
		    profDhcp->dhcpEn = PROF_ENABLE;
		    sprintf(profDhcp->interface, "%s.%d", "eth2", vidVal);
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
    } 
    else {
	/* modify an exist option */
	debug();
	indexTmp = ProfGetInstIndexByName(mibType, oldName);
	if (indexTmp == PROFFAIL) {
	    goto out;
	}

	for (index = min; index < max; index++) {
	    if (index != indexTmp) {
		prof = (TagVlanProfile *)ProfGetInstPointByIndex(mibType, index);
		if (NULL != prof) {
		    if (!strcmp(newName, prof->head.name)) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_TV_NAME_CONFLICT);
			setTheErrorMsg((char *)ErrMsg);
			//setTheErrorMsg("操作失败，服务区名称冲突");
			goto out;
		    }
		    if (vidVal == prof->vid) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_VLAN_ID_CONFLICT);
			setTheErrorMsg((char *)ErrMsg);
			//setTheErrorMsg("操作失败，VLAN ID冲突");
			goto out;
		    }
		}
	    }
	}

	prof = (TagVlanProfile *)ProfGetInstPointByName(mibType, oldName);
	if (NULL == prof) {
	    debug();
	    goto out;
	}

#if (DHCP_POOL == FYES)
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
		    sprintf(profDhcp->interface, "%s.%d", "eth2", vidVal);
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
		    sprintf(profDhcp->interface, "%s.%d", "eth2", vidVal);
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
#endif
	index = indexTmp;
	profType = PROF_CHANGE_EDIT;
    }

    prof = (TagVlanProfile *)ProfGetInstPointByIndex(mibType, index);
    if (NULL == prof) {
	goto out;
    }
    ProfBackupByIndex(mibType, profType, index, &profhead);

    strcpy(prof->head.name, newName);
    prof->head.active = TRUE;
    prof->mbrmsk = mbrMsk32;
    prof->untagmsk = untagMsk32;
    prof->vid = vidVal;
#if (DHCP_POOL == FYES)
    prof->gw = addrGw.s_addr;
    prof->netmask = addrNm.s_addr;

    if (0 == strcmp(dhcpEn, "1")) {
	prof->dhcpEn = PROF_ENABLE;
    } else {
	prof->dhcpEn = PROF_DISABLE;
    }
    strcpy(prof->dhcpPool, dhcpPool);
#endif

    if (NULL != profhead) {
	ProfUpdate(profhead);
	//ProfFreeAllocList(profhead);
    }

    if (NULL != profheadDhcp) {
	ProfUpdate(profheadDhcp);
	//ProfFreeAllocList(profheadDhcp);
    }

    nvramWriteCommit();

out:
    if (NULL != profhead) {
	//ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
    }

    if (NULL != profheadDhcp) {
	//ProfUpdate(profheadDhcp);
	ProfFreeAllocList(profheadDhcp);
    }

    websRedirect(wp, "TagVlanList.asp");
    return;
}

static void formTagVlanListDel(webs_t wp, char_t *path, char_t *query)
{
    char *delStr = NULL, *tmp;
    int min, max, index = 0, change = 0;
#if (DHCP_POOL == FYES)
    MibProfileType mibTypeDhcp = MIB_PROF_DHCP_POOL;
    DhcpPoolProfile *profDhcp = NULL;
    struProfAlloc *profheadDhcp = NULL;
#endif
    MibProfileType mibType = MIB_PROF_TAG_VLAN;
    TagVlanProfile *prof = NULL;

    struProfAlloc *profhead = NULL;

    delStr = websGetVar(wp,T("delstr"),T(""));
    if (NULL == delStr) {
	goto out;
    }

    ProfInstLowHigh(mibType, &max, &min);

    tmp = strtok(delStr, UTT_SPLIT_SIGN_STR);
    while(tmp != NULL) {
	for (index = min; index < max; index++) {
	    prof = (TagVlanProfile *)ProfGetInstPointByIndex(mibType, index);
	    if (!strcmp(prof->head.name, tmp)) {

#if (DHCP_POOL == FYES)
		if (PROF_ENABLE == prof->dhcpEn) {
		    profDhcp = (DhcpPoolProfile *)ProfGetInstPointByName(mibTypeDhcp, prof->dhcpPool);
		    ProfBackupByName(mibTypeDhcp, PROF_CHANGE_EDIT, prof->dhcpPool, &profheadDhcp);
		    profDhcp->dhcpEn = PROF_DISABLE;
		    memset(profDhcp->interface, '\0', sizeof(profDhcp->interface));
		}
#endif
		ProfBackupByIndex(mibType, PROF_CHANGE_DEL, index, &profhead);
		ProfDelInstByIndex(mibType, index);
		change = 1;
	    }
	}

	tmp = strtok(NULL, UTT_SPLIT_SIGN_STR);
    }
    if (1 == change) {
#if (DHCP_POOL == FYES)
	ProfUpdate(profheadDhcp);
	ProfFreeAllocList(profheadDhcp);
#endif
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
	/* vlan间路由通过自定义信号实现，不同过update实现.重新初始化路由表 */
	UttSendMsgForSpeed(MSG_TAG_VLAN_ROUTE_CHG, 0, 0, sizeof(int));
    }

out:
    websRedirect(wp, "TagVlanList.asp");
    return;
}

static void formTagVlanListDelAll(webs_t wp, char_t *path, char_t *query)
{
    int min, max, index = 0;
    MibProfileType mibType = MIB_PROF_TAG_VLAN;
    TagVlanProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
#if (DHCP_POOL == FYES)
    struProfAlloc *profheadDhcp = NULL;
    MibProfileType mibTypeDhcp = MIB_PROF_DHCP_POOL;
    DhcpPoolProfile *profDhcp = NULL;
#endif

    ProfInstLowHigh(mibType, &max, &min);

    for (index = min; index < max; index++) {
	prof = (TagVlanProfile *)ProfGetInstPointByIndex(mibType, index);
	if (NULL != prof) {

#if (DHCP_POOL == FYES)
	    if (PROF_ENABLE == prof->dhcpEn) {
		profDhcp = (DhcpPoolProfile *)ProfGetInstPointByName(mibTypeDhcp, prof->dhcpPool);
		ProfBackupByName(mibTypeDhcp, PROF_CHANGE_EDIT, prof->dhcpPool, &profheadDhcp);
		profDhcp->dhcpEn = PROF_DISABLE;
		memset(profDhcp->interface, '\0', sizeof(profDhcp->interface));
	    }
#endif
	    ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, index, &profhead);
	    ProfDelInstByIndex(mibType, index);
	}
    }

#if (DHCP_POOL == FYES)
    ProfUpdate(profheadDhcp);
    ProfFreeAllocList(profheadDhcp);
#endif
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    /* vlan间路由通过自定义信号实现，不同过update实现.重新初始化路由表 */
    UttSendMsgForSpeed(MSG_TAG_VLAN_ROUTE_CHG, 0, 0, sizeof(int));

    websRedirect(wp, "TagVlanList.asp");
    return;
}

static int getNewDhcpPool(int eid, webs_t wp, int argc, char **argv)
{
#if (DHCP_POOL == FYES)
    int index, min, max, arrayNum = 0;
    DhcpPoolProfile *profDhcp = NULL;
    MibProfileType mibTypeDhcp = MIB_PROF_DHCP_POOL;

    websWrite(wp, "var DhcpPoolSupport = \"yes\";");
    websWrite(wp, "var poolNames = new Array();\n");
    websWrite(wp, "var beginIp = new Array();\n");
    websWrite(wp, "var endIp = new Array();\n");
    websWrite(wp, "var gateWays = new Array();\n");
    websWrite(wp, "var netMasks = new Array();\n");

    websWrite(wp, "gateWays[0] = \"%N\";\n", 0);
    websWrite(wp, "netMasks[0] = \"%N\";\n", 0);

    ProfInstLowHigh(mibTypeDhcp, &max, &min);

    for (index = min + 1; index < max; index++) {
	profDhcp = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibTypeDhcp, index);
	if ((NULL != profDhcp) && (0 == ProfInstIsFree((Pointer)profDhcp)) && (TRUE == profDhcp->head.active)) {
	    websWrite(wp, "poolNames[%d] = \"%s\";\n",arrayNum, profDhcp->head.name);
	    websWrite(wp, "beginIp[%d] = \"%N\";\n", arrayNum, profDhcp->ipStart);
	    websWrite(wp, "endIp[%d] = \"%N\";\n", arrayNum, profDhcp->ipEnd);
	    websWrite(wp, "gateWays[%d] = \"%N\";\n", arrayNum, profDhcp->gateway);
	    websWrite(wp, "netMasks[%d] = \"%N\";\n", arrayNum, profDhcp->netmask);

	    arrayNum++;
	}
    }
#else
    websWrite(wp, "var DhcpPoolSupport = \"no\";");
    websWrite(wp, "var poolNames = new Array();\n");
    websWrite(wp, "var beginIp = new Array();\n");
    websWrite(wp, "var endIp = new Array();\n");
    websWrite(wp, "var gateWays = new Array();\n");
    websWrite(wp, "var netMasks = new Array();\n");
#endif

    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}


static int getTagVlanList(int eid, webs_t wp, int argc, char **argv)
{
    int min, max, index, arrNum = 0;
    MibProfileType mibType = MIB_PROF_TAG_VLAN;
    TagVlanProfile *prof = NULL;

    websWrite(wp, "var vlanName = new Array();\n");
    websWrite(wp, "var vlanId = new Array();\n");
    websWrite(wp, "var ipAddr = new Array();\n");
    websWrite(wp, "var netMask = new Array();\n");
    websWrite(wp, "var dhcp = new Array();\n");
    websWrite(wp, "var mbrmsk = new Array();\n");
    websWrite(wp, "var untagmsk = new Array();\n");

    ProfInstLowHigh(mibType, &max, &min);

    for (index = min; index < max; index++) {
	prof = (TagVlanProfile *)ProfGetInstPointByIndex(mibType, index);
	if ((prof != NULL) && (ProfInstIsFree((Pointer)prof) == 0) && (prof->head.active == TRUE)
	   ) {
	    websWrite(wp, "vlanName[%d] = \"%s\";", arrNum, prof->head.name);
	    if (0 == prof->vid) {
		websWrite(wp, "vlanId[%d] = \"\";", arrNum);
	    } else {
		websWrite(wp, "vlanId[%d] = \"%d\";", arrNum, prof->vid);
	    }
	    websWrite(wp, "mbrmsk[%d] = \"%d\";", arrNum, prof->mbrmsk);
	    websWrite(wp, "untagmsk[%d] = \"%d\";", arrNum, prof->untagmsk);


	    if (prof->dhcpEn == PROF_ENABLE) {
		websWrite(wp, "dhcp[%d] = %s;", arrNum, "true");
	    } else {
		websWrite(wp, "dhcp[%d] = %s;", arrNum, "false");
	    }
	    websWrite(wp, "ipAddr[%d] = \"%N\";", arrNum, prof->gw);
	    websWrite(wp, "netMask[%d] = \"%N\";", arrNum, prof->netmask);

	    arrNum++;
	}
    }

    websWrite(wp, "var totalrecs = %d;", arrNum);
#if (DHCP_POOL == FYES)
    websWrite(wp, "var DhcpPoolSupport = \"yes\";");
#else
    websWrite(wp, "var DhcpPoolSupport = \"no\";");
#endif
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

static int getOneTagVlan(int eid, webs_t wp, int argc, char **argv)
{
    int index, min, max, arrayNum = 0;
    char *edit_Name;
    MibProfileType mibType = MIB_PROF_TAG_VLAN;
    TagVlanProfile *prof = NULL;

#if (DHCP_POOL == FYES)
    DhcpPoolProfile *profDhcp = NULL;
    char tmpName[UTT_DHCP_POOL_LEN];
    MibProfileType mibTypeDhcp = MIB_PROF_DHCP_POOL;
#endif

    memset(tmpName, '\0', sizeof(tmpName));

    ejArgs(argc, argv, T("%s"), &edit_Name);

    prof = ProfGetInstPointByName(mibType, edit_Name);
    if (prof != NULL) {
	websWrite(wp, "var vlanName = \"%s\";", prof->head.name);
	websWrite(wp, "var vlanId = \"%d\";", prof->vid);
	websWrite(wp, "var mbrmsk = \"%d\";", prof->mbrmsk);
	websWrite(wp, "var untagmsk = \"%d\";", prof->untagmsk);
	websWrite(wp, "var ipAddr = \"%N\";", prof->gw);
	websWrite(wp, "var maskAddr = \"%N\";", prof->netmask);

	if (prof->dhcpEn == PROF_ENABLE) {
	    websWrite(wp, "var dhcpServer = %s;", "true");
	} else {
	    websWrite(wp, "var dhcpServer = %s;", "false");
	}
	websWrite(wp, "var pool = \"%s\";", prof->dhcpPool);
    }

    websWrite(wp, "var poolNames = new Array();\n");
    websWrite(wp, "var beginIp = new Array();\n");
    websWrite(wp, "var endIp = new Array();\n");
    websWrite(wp, "var gateWays = new Array();\n");
    websWrite(wp, "var netMasks = new Array();\n");

    websWrite(wp, "gateWays[0] = \"%N\";\n", 0);
    websWrite(wp, "netMasks[0] = \"%N\";\n", 0);

#if (DHCP_POOL == FYES)
    ProfInstLowHigh(mibTypeDhcp, &max, &min);
    for (index = min + 1; index < max; index++) {
	profDhcp = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibTypeDhcp, index);
	if ((NULL != profDhcp) && (0 == ProfInstIsFree((Pointer)profDhcp)) && (TRUE == profDhcp->head.active)) {
	    websWrite(wp, "poolNames[%d] = \"%s\";\n",arrayNum, profDhcp->head.name);
	    websWrite(wp, "beginIp[%d] = \"%N\";\n", arrayNum, profDhcp->ipStart);
	    websWrite(wp, "endIp[%d] = \"%N\";\n", arrayNum, profDhcp->ipEnd);
	    websWrite(wp, "gateWays[%d] = \"%N\";\n", arrayNum, profDhcp->gateway);
	    websWrite(wp, "netMasks[%d] = \"%N\";\n", arrayNum, profDhcp->netmask);

	    arrayNum++;
	}
    }
#endif

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
static int getPvid(int eid, webs_t wp, int argc, char **argv) {
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);

    websWrite(wp, "var pvid1 = %d;", profSys->pvid.pvid1);
    websWrite(wp, "var pvid2 = %d;", profSys->pvid.pvid2);
    websWrite(wp, "var pvid3 = %d;", profSys->pvid.pvid3);
    websWrite(wp, "var pvid4 = %d;", profSys->pvid.pvid4);

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

static void formPvidConfig(webs_t wp, char_t *path, char_t *query) {
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    struProfAlloc *profhead = NULL;
    char *pvid1 = NULL, *pvid2 = NULL, *pvid3 = NULL, *pvid4 = NULL;

    pvid1 = websGetVar(wp, "pvid1", T("0"));
    pvid2 = websGetVar(wp, "pvid2", T("0"));
    pvid3 = websGetVar(wp, "pvid3", T("0"));
    pvid4 = websGetVar(wp, "pvid4", T("0"));

    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);
    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);

    profSys->pvid.pvid1 = strtoul(pvid1, NULL, 10);
    profSys->pvid.pvid2 = strtoul(pvid2, NULL, 10);
    profSys->pvid.pvid3 = strtoul(pvid3, NULL, 10);
    profSys->pvid.pvid4 = strtoul(pvid4, NULL, 10);

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    websRedirect(wp, "vid.asp");

    return;
}

static int getVlanRoute(int eid, webs_t wp, int argc, char **argv) {
    int min, max, i, routeEnIndex = 0, routeDisIndex = 0;
    MibProfileType mibType = MIB_PROF_TAG_VLAN;
    TagVlanProfile *prof = NULL;

    websWrite(wp, "var routeEnArr = new Array();\n");
    websWrite(wp, "var routeDisArr = new Array();\n");

    ProfInstLowHigh(mibType, &max, &min);

    for (i = min; i < max; i++) {
	prof = (TagVlanProfile *)ProfGetInstPointByIndex(mibType, i);
	if ((prof != NULL) && (ProfInstIsFree((Pointer)prof) == 0) && (prof->head.active == TRUE)) {
	    /* 
	     * 可通信vlan
	     */
	    if (prof->cc_vlan == 1) {
		websWrite(wp, "routeEnArr[%d]=\"%s\";", routeEnIndex, prof->head.name);
		routeEnIndex++;
	    } else {
		websWrite(wp, "routeDisArr[%d]=\"%s\";", routeDisIndex, prof->head.name);
		routeDisIndex++;
	    }
	}
    }

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

static void formVlanRouteConfig(webs_t wp, char_t *path, char_t *query) {
    char *routeEnVlan = NULL, *routeDisVlan = NULL;
    MibProfileType mibType = MIB_PROF_TAG_VLAN;
    TagVlanProfile *prof = NULL;
    int i, min, max;

    routeEnVlan = websGetVar(wp, "hidRouteEn", T(""));
    routeDisVlan = websGetVar(wp, "hidRouteDis", T(""));

    ProfInstLowHigh(mibType, &max, &min);

    for (i = min; i < max; i++) {
	prof = (TagVlanProfile *)ProfGetInstPointByIndex(mibType, i);
	if (ProfInstIsFree(prof) == 0) {
	    if (strstr(routeEnVlan, prof->head.name)) {
		prof->cc_vlan = 1;
	    } else {
		prof->cc_vlan = 0;
	    }
	}
    }
    nvramWriteCommit();
    /* vlan间路由通过自定义信号实现，不同过update实现.重新初始化路由表 */
    UttSendMsgForSpeed(MSG_TAG_VLAN_ROUTE_CHG, 0, 0, sizeof(int));

    websRedirect(wp, "vlan_cc.asp");
    return;
}

#if (WAN_TAG_VLAN == FYES)
static int getWanTagVlanData(int eid, webs_t wp, int argc, char **argv) {
    WanTagVlanProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_WAN_TAG_VLAN;
    prof = (WanTagVlanProfile *)ProfGetInstPointByIndex(mibType, 0);

    websWrite(wp, "var WanTagVlanEnable = %d;", prof->head.active);
    websWrite(wp, "var vlanId = %d;", prof->vid);
    websWrite(wp, "var tagged = %d;", prof->tag);

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

/* 功能重启生效 */
static void formWanTagVlanConfig(webs_t wp, char_t *path, char_t *query) {
    MibProfileType mibType = MIB_PROF_WAN_TAG_VLAN;
    WanTagVlanProfile *prof = NULL;
    //    struProfAlloc *profhead = NULL;
    char *enable = NULL, *vlanId = NULL, *rule = NULL;
    const char *errMsg = NULL;
    unsigned int vid = 0;

    enable = websGetVar(wp, "enable", T(""));
    vlanId = websGetVar(wp, "vlanId", T("0"));
    rule = websGetVar(wp, "rule", T("0"));
    vid = strtoul(vlanId, NULL, 10);

    prof = (WanTagVlanProfile *)ProfGetInstPointByIndex(mibType, 0);

    if (strcmp(enable, "on") == 0) {

	if (vid > 4094 || vid < 6) {
	    errMsg = getTransDataForC(C_LANG_INDEX_OPERATION_FAIL);
	    setTheErrorMsg(errMsg);

	    goto out;
	}
	prof->head.active = FUN_ENABLE;
	prof->vid = vid;
	if (strcmp(rule, "tag") == 0) {
	    prof->tag = 1;
	} else {
	    prof->tag = 0;
	}

    } else {
	prof->head.active = FUN_DISABLE;
    }

    ProfWiteToFlash();/*write flash*/
    errMsg = getTransDataForC(C_LANG_INDEX_DYN_WAN_SETCONF_SAVE);
    setTheErrorMsg(errMsg);

out:
    websRedirect(wp, "WanTagVlan.asp");
    return;
}
#endif
void formDefineTagVlan(void)
{
    websAspDefine(T("aspAddNewDhcpPool"), getNewDhcpPool);
    websAspDefine(T("getOneTagVlan"), getOneTagVlan);
    websAspDefine(T("aspOutPutTagVlanList"), getTagVlanList);
    websFormDefine(T("formTagVlanConfig"), formTagVlanConfig);
    websFormDefine(T("formTagVlanListDel"), formTagVlanListDel);
    websFormDefine(T("formTagVlanListDelAll"), formTagVlanListDelAll);
    websAspDefine(T("aspOutputPvid"), getPvid);
    websFormDefine(T("formPvidConfig"), formPvidConfig);
    websAspDefine(T("aspOutputVlanRoute"), getVlanRoute);
    websFormDefine(T("formVlanRouteConfig"), formVlanRouteConfig);
#if (WAN_TAG_VLAN == FYES)
    websAspDefine(T("aspOutputWanTagVlanData"), getWanTagVlanData);
    websFormDefine(T("formWanTagVlanConfig"), formWanTagVlanConfig);
#endif
}

#endif
