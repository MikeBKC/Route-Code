

#include	<stdlib.h>
#include	<sys/ioctl.h>
#include	<net/route.h>
#include        <string.h>
#include        <dirent.h>
#include	"internet.h"
#include	"webs.h"
#include	"utils.h"
#include 	"firewall.h"
#include	"management.h"
#include	"station.h"
#include	"uttfunction.h"
#include    "linux/mii.h"
#include    "linux/sockios.h"
#include	"translate.h"
/*
 * kernel config
 */
#include	"linux/autoconf.h"
#ifndef CONFIG_RALINK_RT2880
#include	<net/if.h>
#endif
/*
 * user config
 */
#include	"config/autoconf.h" 
#include	"mib.h"
#include	"profacce.h"
#include        "ifmisc.h"

static void formConfigMultiPathConfig(webs_t wp, char_t *path, char_t *query)
{
    char *wanName, *detectTime, *detectCount, *weight, *ipStart, *ipEnd, *detectIp;
    int wanIndex = 0;
    InterfaceProfile *profIf = NULL;
    MibProfileType mibType = MIB_PROF_INTERFACE;
    struProfAlloc *profhead = NULL;
    IPADDR decIp = 0, iIpS = 0, iIpE = 0;
    InterfaceProfile *profIF = NULL;
    Uint32 i;
    Uint32 wanCount;
    char if_addr[16] = {0};
    const char *errMsg=NULL;

    wanName = websGetVar(wp, "PortName", T(""));
    detectTime = websGetVar(wp, "KeepLive", T(""));
    detectCount = websGetVar(wp, "RetryTimes", T(""));
    detectIp = websGetVar(wp, "DestIP", T(""));
    weight = websGetVar(wp, "MaxRatetxt", T(""));
    ipStart = websGetVarNull(wp, "InnerFIP", T("0.0.0.0"), T("0.0.0.0"));
    ipEnd = websGetVarNull(wp, "InnerTIP", T("0.0.0.0"), T("0.0.0.0"));
    wanIndex = strtol(wanName, NULL, 10);
    decIp = inet_addr(detectIp);
#if 0
    iIpS = inet_addr(ipStart);
    iIpE = inet_addr(ipEnd);
    if(iIpS > iIpE) {
        errMsg = getTransDataForC(C_LANG_INDEX_CMP_START_END_ERR);
	setTheErrorMsg(errMsg);
        goto out;
    }
#endif
    if(decIp == 0xffffffff || iIpS == 0xffffffff || iIpE == 0xffffffff) {
        goto out;
    }

    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);
    /* mantis 8915: 检测ip不能是lan网段ip */
    if(((decIp & profIF->ether.sta.nm) == (profIF->ether.sta.ip & profIF->ether.sta.nm)) 
#if (MULTI_LAN == FYES)
    || ((decIp & profIF->ether.sta.nm2) == (profIF->ether.sta.ip2 & profIF->ether.sta.nm2)) 
    || ((decIp & profIF->ether.sta.nm3) == (profIF->ether.sta.ip3 & profIF->ether.sta.nm3)) 
    || ((decIp & profIF->ether.sta.nm4) == (profIF->ether.sta.ip4 & profIF->ether.sta.nm4)) 
#endif
	)
    {
	if (decIp != 0) {
	    errMsg = getTransDataForC(C_LANG_INDEX_CHECK_IP_NOT_LAN);
	    setTheErrorMsg(errMsg);
	    goto out;
	}
    }

    /* mantis 8913: 检测ip不能是wan口ip地址 */
    wanCount = getWanSlotCount();
    for (i = 1U; i <= wanCount; i++) {
	if (getIfIp(getWanIfNamePPP(i), if_addr) == 0) {
	    if (decIp == inet_addr(if_addr)) {
		errMsg = getTransDataForC(C_LANG_INDEX_CHECK_IP_NOT_WAN);
		setTheErrorMsg(errMsg);
		goto out;
	    }
	}
    }

    if(wanIndex > 0 && wanIndex <= getWanSlotCount()) {
        profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, wanIndex);
        ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, wanIndex, &profhead);
        profIf->detectInterval = strtol(detectTime, NULL, 10);
        profIf->detectCount = strtol(detectCount, NULL, 10);
        profIf->detectIp = decIp;
        profIf->rxRate = strtol(weight, NULL, 10);
#if 0
        profIf->srcRouteIpS = iIpS;
        profIf->srcRouteIpE = iIpE;
#endif
        ProfUpdate(profhead);
        ProfFreeAllocList(profhead);
        nvramWriteCommit();
    }
out:
    websRedirect(wp, "DoubleRoadInfo.asp");
    return;
}

#if (WIRELESS == FYES)
extern int get3GSlotIndex(void);
extern int getAPCliSlotIndex(void);
#endif
static int aspGetMultiPathConfig(int eid, webs_t wp, int argc, char_t **argv)
{
    int arrayIndex = 0;
    int i = 0;
    InterfaceProfile *profIf = NULL;
    MibProfileType mibType = MIB_PROF_INTERFACE;
    char_t *PortNames=T("PortNames[%d]=\"%d\";\n");
    char_t *ConnectTypes=T("ConnTypes[%d]=\"%s\";\n");
    char_t *Weights=T("Weights[%d]=\"%dk bit/s\";\n");
    char_t *KeepLives=T("KeepLives[%d]=\"%d\";\n");
    char_t *RetryTimess=T("RetryTimess[%d]=\"%d\";\n");
    char_t *DestIPs=T("DestIPs[%d]=\"%N\";\n");
#if 0
    char_t *InnerFIPs=T("InnerFIPs[%d]=\"%N\";\n");
    char_t *InnerTIPs=T("InnerTIPs[%d]=\"%N\";\n");
#endif
    char_t *ConnCheck=T("ConnCheck[%d]=\"%d\";\n");
    char_t *PortNamesw=T("PortNamesw[%d]=\"WAN%d\";\n");
    websWrite(wp, "var PortNames = new Array();");
    websWrite(wp, "var ConnTypes = new Array();");
    websWrite(wp, "var Weights = new Array();");
    websWrite(wp, "var KeepLives = new Array();");
    websWrite(wp, "var RetryTimess = new Array();");
    websWrite(wp, "var DestIPs=new Array();");
#if 0
    websWrite(wp, "var InnerFIPs = new Array();");
    websWrite(wp, "var InnerTIPs = new Array();");
#endif
    websWrite(wp, "var ConnCheck=new Array();");
    websWrite(wp, "var PortNamesw=new Array();");

    for(i = 1; i <= getWanSlotCount(); i++) {
        arrayIndex = i - 1;
        profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, i);
        websWrite(wp, PortNames, arrayIndex, i);
	if(profIf->head.active == PROF_ENABLE) {
            websWrite(wp, ConnectTypes, arrayIndex, 
                MibGetStrByEnum(profIf->ether.connMode, ethConnModeEnum));
        } else {
            websWrite(wp, ConnectTypes, arrayIndex, "");
        }
        websWrite(wp, Weights, arrayIndex, profIf->rxRate);
        websWrite(wp, KeepLives, arrayIndex, profIf->detectInterval);
        websWrite(wp, RetryTimess, arrayIndex, profIf->detectCount);
        websWrite(wp, DestIPs, arrayIndex, profIf->detectIp);
#if 0
        websWrite(wp, InnerFIPs, arrayIndex, profIf->srcRouteIpS);
        websWrite(wp, InnerTIPs, arrayIndex, profIf->srcRouteIpE);
#endif
        websWrite(wp, ConnCheck, arrayIndex, _getWanLinkState(i));
#if (WIRELESS == FYES)
	if (i == get3GSlotIndex()) {
	    websWrite(wp, T("PortNamesw[%d]=\"%s\";"), arrayIndex, "3G");
	} else if (i == getAPCliSlotIndex()) {
	    websWrite(wp, T("PortNamesw[%d]=\"%s\";"), arrayIndex, "APClient");
	} else 
#endif
	{
	    websWrite(wp, PortNamesw, arrayIndex, i);
	}
    }

    websWrite(wp, "var totalrecs=%d;", i-1);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

static int aspGetMultiPathOneConfig(int eid, webs_t wp, int argc, char_t **argv)
{
    int wanIndex = 0;
    InterfaceProfile *profIf = NULL;
    MibProfileType mibType = MIB_PROF_INTERFACE;
    const char *errMsg=NULL;

    if(ejArgs(argc,argv,T("%d"), &wanIndex) <1) {
	errMsg = getTransDataForC(C_LANG_INDEX_READ_EDIT_NAME_ERR);
	setTheErrorMsg(errMsg);
    } else {
        profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, wanIndex);
        if(ProfInstIsFree(profIf) == 0) {
            websWrite(wp, "var PortNames=\"%d\";", wanIndex);
            websWrite(wp, "var KeepLives=\"%d\";", profIf->detectInterval);
            websWrite(wp, "var RetryTimess=\"%d\";", profIf->detectCount);
            websWrite(wp, "var Rate=\"%dk\";", profIf->rxRate);
#if 0
            websWrite(wp, "var InnerFIPs=\"%N\";", profIf->srcRouteIpS);
            websWrite(wp, "var InnerTIPs=\"%N\";", profIf->srcRouteIpE);
#endif
            websWrite(wp, "var DestIPs=\"%N\";", profIf->detectIp);
        }
    }
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

static void formConfigMultiPathGlobale(webs_t wp, char_t *path, char_t *query)
{
    int wanStatuses[MAX_WAN_IF_COUNT] = {0};
    int i = 0;
    char wanName[8] = {0};
    InterfaceProfile *profIf = NULL;
    MibProfileType mibType = MIB_PROF_INTERFACE;
    char *idBind, *multiPathType, *mainPath, *backupPath;
    Uint32 wanCount = 0;
#if 0
    struProfAlloc *profhead = NULL;
#endif
    SystemProfile *profSys = NULL;

    idBind = websGetVar(wp, "IDBinding", T(""));
    multiPathType = websGetVar(wp, "DualType", T(""));
    mainPath = websGetVar(wp, "MainRoad", T(""));
    backupPath = websGetVar(wp, "BranchRoad", T(""));

    profSys = (SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    profSys->lineGroupMode = MibGetEnumByStr(multiPathType, lineGroupModeEnum);
    wanCount = getWanSlotCount();
    for(i = 1; i <= wanCount; i++) {
        profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, i);
        if(ProfInstIsFree(profIf) == 1) {
            continue;
        }
        sprintf(wanName,"WAN%d", i);
        if (strstr(mainPath, wanName)) {
            profIf->lineBalMode = IF_MIAN_LINE;
        } else {
            profIf->lineBalMode = IF_BACKUP_LINE;
        }
        if(profSys->lineGroupMode == GROUP_LINE_BALANCE) {
            wanStatuses[i-1] = 1 - profIf->runningLineMode;
            profIf->runningLineMode = 1;
        } else {
            if(profIf->lineBalMode == IF_MIAN_LINE) {
                wanStatuses[i-1] = 1 - profIf->runningLineMode;
                profIf->runningLineMode = 1;
            } else {
                wanStatuses[i-1] = 0 - profIf->runningLineMode;
                profIf->runningLineMode = 0;
            }
        }

#if 0
        ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, instIndex, &profhead);
        ProfUpdate(profhead);
        ProfFreeAllocList(profhead);
#endif
    }
    /*
     * here need active porf change
     */
    UttSendMsgForSpeed(MSG_MPGLOBALE_CHANGE, 0, wanStatuses, sizeof(wanStatuses));
    nvramWriteCommit();
    websRedirect(wp, "DoubleRoad.asp");
    return ;
}

static int aspGetMultiPathGlobale(int eid, webs_t wp, int argc, char_t **argv)
{
    InterfaceProfile *profIf = NULL;
    MibProfileType mibType = MIB_PROF_INTERFACE;
    SystemProfile *profSys = NULL;
    Uint32 wanCount = 0;
    int i, mainIndex = 0, backIndex = 0;

    profSys = (SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    websWrite(wp, "var DualTypes=\"%s\";",
                MibGetStrByEnum(profSys->lineGroupMode, lineGroupModeEnum));
    wanCount = getWanSlotCount();
    websWrite(wp, "var MAX_LEN=%d;", wanCount);

    websWrite(wp, "var mainRoadArr = new Array();");
    websWrite(wp, "var backupRoadArr = new Array();");
    for(i = 1; i <= wanCount; i++) {
        profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, i);
        if(ProfInstIsFree(profIf) == 1) {
            continue;
        }

        if(profIf->lineBalMode == IF_MIAN_LINE) {
            websWrite(wp, "mainRoadArr[%d]=\"WAN%d\";\n", mainIndex, i);
            mainIndex += 1;
        } else {
            websWrite(wp, "backupRoadArr[%d]=\"WAN%d\";\n", backIndex, i);
            backIndex += 1;
        }
    }
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

/*
 * getRateArr1		-	获取千兆设备的速率数组
 *
 * 若设备是千兆设备则将重定义速率数组，否则什么也不做，
 * 直接使用common.js中的速率数组
 */
static int getRateArr1(int eid, webs_t wp, int argc, char **argv)
{
#if (MAX_SPDMODE_RDD == 1000)
	websWrite(wp, "RateArr1 = new Array(\"512K\", \"1M\", \"1.5M\", \"2M\", \"3M\", \"4M\", \"5M\", \"6M\", \"7M\", \"8M\", \"9M\", \"10M\", \"11M\", \"12M\", \"13M\", \"14M\", \"15M\", \"16M\", \"17M\", \"18M\", \"19M\", \"20M\", \"25M\", \"30M\", \"35M\", \"40M\", \"45M\", \"50M\", \"90M\", \"100M\", \"1000M\");\n");
	websWrite(wp, "RateArrValue1 = new Array(\"512\", \"1000\", \"1500\", \"2000\", \"3000\", \"4000\", \"5000\", \"6000\", \"7000\", \"8000\", \"9000\", \"10000\", \"11000\", \"12000\", \"13000\", \"14000\", \"15000\", \"16000\", \"17000\", \"18000\", \"19000\", \"20000\", \"25000\", \"30000\", \"35000\", \"40000\", \"45000\", \"50000\", \"90000\", \"100000\", \"1000000\");\n");
#endif
	return 0;
}


/*
 * fun for multi path
 * define asp and form fun
 */
void formDefineMultiPath(void) {
    websAspDefine(T("aspMultiPathGlobale"), aspGetMultiPathGlobale);
    websAspDefine(T("aspMultiPathConfig"), aspGetMultiPathConfig);
    websAspDefine(T("aspMultiPathOneConfig"), aspGetMultiPathOneConfig);
    websFormDefine(T("formMultiPathGlobale"), formConfigMultiPathGlobale);
    websFormDefine(T("formMultiPathConfig"), formConfigMultiPathConfig);
    websAspDefine(T("aspOutputRateArr1"), getRateArr1);
}


