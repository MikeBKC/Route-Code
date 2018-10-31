#include "uttMachine.h"
#if (STATIC_ROUTE == FYES)
#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include	"argcmd.h"
#include	"wuScript.h"
#include	"translate.h"
//#define ROUTE_DEBUG

#ifdef ROUTE_DEBUG
#define ROUTE_PRINT(fmt, args...)		printf(fmt, ## args)
#else
#define ROUTE_PRINT(fmt, args...)
#endif

#if(STATIC_ROUTE_BIND_PPTP == FYES)
static int aspOutputPPTPAccInfo(int eid, webs_t wp, int argc, char_t **argv);
static void PPTPServerAccInfo(webs_t wp);
static void PPTPClientAccInfo(webs_t wp);
#endif
/*
 * 列表页面相关配置输出
 */
static int aspOutputStaticRouteList(int eid, webs_t wp, int argc, char_t **argv) {
    MibProfileType mibType = MIB_PROF_STATIC_ROUTE;
    StaticRouteProfile *profRoute = NULL;
    int min = 0, max = 0, i = 0, totalrecs = 0;

    char_t *routeNames = T("RouteNames[%d] = \"%s\";");
    char_t *enables = T("RouteEnables[%d] = \"%d\";");
    char_t *dstIps = T("DesIPs[%d] = \"%N\";");
    char_t *netmasks = T("DesMasks[%d] = \"%N\";");
    char_t *gateways = T("GateWays[%d] = \"%N\";");
    char_t *metrics = T("Metrics[%d] = \"%d\";");
    char_t *interfaces = T("Profiles[%d] = \"%s\";");

    websWrite(wp,"var RouteNames=new Array();");
    websWrite(wp,"var RouteEnables= new Array();");
    websWrite(wp,"var DesIPs=new Array();");
    websWrite(wp,"var DesMasks=new Array();");
    websWrite(wp,"var GateWays=new Array();");
    websWrite(wp,"var Metrics=new Array();");
    websWrite(wp,"var Profiles=new Array();");

    ProfInstLowHigh(mibType, &max, &min);

    for (i = min; i < max; i++) {
	profRoute = (StaticRouteProfile *)ProfGetInstPointByIndex(mibType, i);
	if ((profRoute != NULL) && (ProfInstIsFree(profRoute) == 0)) {
	    websWrite(wp, routeNames, totalrecs, profRoute->head.name);
	    websWrite(wp, enables, totalrecs, profRoute->head.active);
	    websWrite(wp, dstIps, totalrecs, profRoute->dstAddr);
	    websWrite(wp, netmasks, totalrecs, profRoute->netmask);
	    websWrite(wp, gateways, totalrecs, profRoute->gwAddr);
	    websWrite(wp, metrics, totalrecs, profRoute->preference);
	    websWrite(wp, interfaces, totalrecs, profRoute->interfaceName);

	    totalrecs++;
	}
    }

    websWrite(wp, "var totalrecs=%d;", totalrecs);
    websWrite(wp, "var max_totalrecs=%d;", max);
    getTheErrorMsg(eid, wp, argc, argv);
#if (UTT_SMART_UI == FYES) 
    getTheRightMsg(eid,wp,argc,argv);
#endif
    return 0;
}

#if (UTT_SMART_UI != FYES)
/*
 * 路由配置页面相关信息输出
 */
static int aspOutputStaticRouteOneInfo(int eid, webs_t wp, int argc, char_t **argv) {
    MibProfileType mibType = MIB_PROF_STATIC_ROUTE;
    StaticRouteProfile *profRoute = NULL;
    int min = 0, max = 0, i = 0;

    char *editName;
    const char *errMsg=NULL;

    if (ejArgs(argc, argv, T("%s"), &editName) < 1) {
	errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_READ_DATA_ERR);
	setTheErrorMsg(errMsg);
    } else {
	ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++) {
	    profRoute = (StaticRouteProfile *)ProfGetInstPointByIndex(mibType, i);
	    if((profRoute != NULL) && (ProfInstIsFree(profRoute) == 0) &&
		    (editName != NULL) && (strcmp(editName, profRoute->head.name) == 0)) {
		websWrite(wp, "RouteNames = \"%s\";", profRoute->head.name);
		websWrite(wp, "RouteEnables = \"%d\";", profRoute->head.active);
		websWrite(wp, "DesIPs = \"%N\";", profRoute->dstAddr);
		websWrite(wp, "DesMasks = \"%N\";", profRoute->netmask);
		websWrite(wp, "GateWays = \"%N\";", profRoute->gwAddr);
		websWrite(wp, "Metrics = \"%d\";", profRoute->preference);
		websWrite(wp, "Profiles = \"%s\";", profRoute->interfaceName);
		break;
	    }
	}
    }
    
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
#endif

#if(STATIC_ROUTE_BIND_PPTP == FYES)
/********************************************************************
 * 函数： aspOutputPPTPAccInfo
 * 功能： 输出PPTP服务端和客户端账号
 * 创建： 2012-10-23
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int aspOutputPPTPAccInfo(int eid, webs_t wp, int argc, char_t **argv) 
{
    PPTPServerAccInfo(wp);
    PPTPClientAccInfo(wp);
    return 0;
}
/********************************************************************
 * 函数： PPTPServerAccInfo
 * 功能： 输出PPTP服务端账号
 * 创建： 2012-10-23
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void PPTPServerAccInfo(webs_t wp)
{
    MibProfileType mibTypeA = MIB_PROF_PPTP_SRV_ACCOUNT;
    PPTPSrvAccProfile *profPptpA = NULL;
    int i = 0, min = 0, max = 0, num = 0;

    websWrite(wp, "var PptpServerNames = new Array();");
    ProfInstLowHigh(mibTypeA, &max, &min);
    for (i = min; i < max; i++) 
    {/*输出账号*/
	profPptpA = (PPTPSrvAccProfile *)ProfGetInstPointByIndex(mibTypeA, i);
	if((profPptpA != NULL) && (ProfInstIsFree(profPptpA) == 0)) 
	{
	    websWrite(wp, "PptpServerNames[%d] = \"(PPTPS)%s\";", num, profPptpA->head.name);
	    num ++;/*注意：num不能而用i来代替*/
	}
    }
    return;
}
/********************************************************************
 * 函数： PPTPClientAccInfo
 * 功能： 输出PPTP客户端账号
 * 创建： 2012-10-23
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void PPTPClientAccInfo(webs_t wp)
{
    MibProfileType mibType = MIB_PROF_PPTP_CLI;
    PPTPCliProfile *profPptp = NULL;
    int i = 0, min = 0, max = 0, num = 0;

    websWrite(wp, "var PptpClientNames = new Array();");
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) 
    {
	profPptp = (PPTPCliProfile*)ProfGetInstPointByIndex(mibType, i);
	if((profPptp != NULL) && (ProfInstIsFree(profPptp) == 0)) 
	{/*输出账号*/
	    websWrite(wp, "PptpClientNames[%d] = \"(PPTPC)%s\";", num, profPptp->head.name);
	    num ++;/*注意：num不能而用i来代替*/
	}
    }
    return;
}
#endif
/*
 * 静态路由配置
 */
static void formStaticRoute(webs_t wp, char_t *path, char_t *query) {
    MibProfileType mibType = MIB_PROF_STATIC_ROUTE;
    struProfAlloc *profhead = NULL;
    StaticRouteProfile *profRoute = NULL;
    int instIndex;
    ProfChangeType profAction;
    struct in_addr addr_ip, addr_mask, addr_gw;

    char *routeName, *enable, *dstIp, *netmask, *gateway, *metric, *interface, *action;
    const char *errMsg=NULL;

    routeName = websGetVar(wp, T("RouteName"), T(""));
    enable = websGetVar(wp, T("RouteEnable"), T("")); /* 启用 1， 不启用 0 */
    dstIp = websGetVar(wp, T("DesIP"), T(""));
    netmask = websGetVar(wp, T("DesMask"), T(""));
    gateway = websGetVar(wp, T("GateWay"), T(""));
    metric = websGetVar(wp, T("Metric"), T(""));
    interface = websGetVar(wp, T("Profile"), T(""));
    action = websGetVar(wp, T("Action"), T(""));

    ROUTE_PRINT("%s: name=%s,enable=%s,dstIp=%s,mask=%s,gw=%s,metric=%s,interface=%s,action=%s\n", 
	    __FUNCTION__, routeName, enable, dstIp, netmask, gateway, metric, interface, action);

    if ((routeName == NULL) || (enable == NULL) || (dstIp == NULL) || (netmask == NULL) || 
	    (gateway == NULL) || (metric == NULL) || (interface == NULL) || (action == NULL)) {
	errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_PARAMETER_ERR);
	setTheErrorMsg(errMsg);
	goto out;
    }

    /*
     * 判断目的地址，掩码，网关是否合法
     */
    if (0 == inet_aton(dstIp, &addr_ip)) {
	errMsg = getTransDataForC(C_LANG_INDEX_DEST_IP_ERR);
    	setTheErrorMsg(errMsg);
    	goto out;
    }
    inet_aton(netmask, &addr_mask);
    if (0 == inet_aton(gateway, &addr_gw)) {
	errMsg = getTransDataForC(C_LANG_INDEX_GATEWAY_ERR);
    	setTheErrorMsg(errMsg);
    	goto out;
    }

    /* 
     * mantis 12545
     * mantis 8197的问题已解决，不再需要该临时处理方法
     */
#if 0
    /*
     * mantis 8197 暂时处理方法
     * 目的网络不能与lan口网段和pppoe地址池重叠
     */
    if (isOverlapInnerIp(ntohl(addr_ip.s_addr), ntohl(addr_mask.s_addr)) == 1) {
	errMsg = getTransDataForC(C_LANG_INDEX_DEST_NET_NOT_INCLUDE);
	setTheErrorMsg(errMsg);
	goto out;
    }
#endif

    /*
     * 判断优先级
     */
    if (uttIsNum(metric) == 0) {
	errMsg = getTransDataForC(C_LANG_INDEX_PRIORITY_ERR);
    	setTheErrorMsg(errMsg);
    	goto out;
    }

    instIndex = ProfGetInstIndexByName(mibType, routeName);

    if (strcmp(action, "add") == 0) {
        if(instIndex != PROFFAIL) {
	    errMsg = getTransDataForC(C_LANG_INDEX_ROUTE_NAME_REPEAT);
    	    setTheErrorMsg(errMsg);
    	    goto out;
        }

        instIndex = ProfNewInst(mibType, routeName, FALSE);

        if(instIndex == PROFFAIL) {
	    errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_MAX_TOTAL_ERR);
    	    setTheErrorMsg(errMsg);
    	    goto out;
        }
        profAction = PROF_CHANGE_ADD;
    } else {
	if(instIndex == PROFFAIL) {
	    errMsg = getTransDataForC(C_LANG_INDEX_ROUTE_NAME_NOT_FIND);
    	    setTheErrorMsg(errMsg);
    	    goto out;
        }
        profAction = PROF_CHANGE_EDIT;
    }

    ProfBackupByIndex(mibType, profAction, instIndex, &profhead);

    profRoute = (StaticRouteProfile *)ProfGetInstPointByIndex(mibType, instIndex);

    ProfSetNameByPoint(profRoute, routeName);
    /* 启用开关 */
    if (strcmp(enable, "1") == 0) {
	profRoute->head.active = 1;
    } else {
	profRoute->head.active = 0;
    }
    profRoute->dstAddr = ((addr_ip.s_addr) & addr_mask.s_addr);
    ROUTE_PRINT("%s-----%d: addr_ip.s_addr = %x, profRoute->dstAddr = %x\n", __func__, __LINE__, addr_ip.s_addr, profRoute->dstAddr);
    profRoute->netmask = addr_mask.s_addr;
    profRoute->gwAddr = addr_gw.s_addr;
    profRoute->preference = strtol(metric, NULL, 10);
    strncpy(profRoute->interfaceName, interface, UTT_INTERFACE_LEN);
    
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, "RouteSui.asp");
#else
    websRedirect(wp, "Route.asp");
#endif
    return;
}

/* 
 * 
 * 备份实例的绑定接口的存储结构
 * interface - 路由配置接口
 */
typedef struct st_BPNode {
    char interface[UTT_INTERFACE_LEN + 1];
    struct st_BPNode *next;
} BPNode;

/*
 * 对应接口的节点是否在链表中已存在
 * 返回值：
 * 1	-   存在，即已备份
 * 0	-   不存在，未备份
 */
static int lookupBPNode(BPNode *pList, char *iif) {
    if (iif == NULL) {
	return 0;
    }
    while (pList != NULL) {
	if (strcmp(pList->interface, iif) == 0) {
	    ROUTE_PRINT("%s: interface=%s,iif[%s] exist\n", __FUNCTION__, pList->interface, iif);
	    return 1;
	}
	pList = pList->next;
    }
    return 0;
}

/*
 * 将备份实例的接口保存到*ppList表中
 * 输入：
 *  *ppList - 保存接口的链表
 *  iif   - 需要保存的绑定接口
 */
static void insertBPNode(BPNode **ppList, char *iif) {
    BPNode *new = NULL;

    if (iif == NULL) {
	return;
    }

    new = (BPNode *)malloc(sizeof(BPNode));
    if (new == NULL) {
	return;
    }
    memset(new, 0, sizeof(BPNode));
    strncpy(new->interface, iif, UTT_INTERFACE_LEN);
    new->next = *ppList;
    *ppList = new;
#ifdef ROUTE_DEBUG 
    BPNode *ptmp;
    ROUTE_PRINT("%s: now pList:\n", __FUNCTION__);
    for (ptmp = *ppList; ptmp != NULL; ptmp = ptmp->next) {
	printf("%s: interface=%s\n", __FUNCTION__, ptmp->interface);
    }
#endif
    return;
}

/*
 * 删除BPNodeList表
 */
static void freeBPNodeList(BPNode *pList) {
    BPNode *tmp;

    while(pList != NULL) {
	tmp = pList;
	pList = pList->next;
	free(tmp);
    }
    return;
}

/*
 * 列表删除
 */
static void formStaticRouteListDel(webs_t wp, char_t *path, char_t *query) {
    char *routeName = NULL, *tmp = NULL;
    const char *errMsg=NULL;
    struProfAlloc *profList  = NULL;
    MibProfileType mibType = MIB_PROF_STATIC_ROUTE;
    /* 对应接口实例已经备份 */
    BPNode *pExistList = NULL;
    StaticRouteProfile *prof = NULL;

    routeName = websGetVar(wp, T("delstr"), T(""));

    ROUTE_PRINT("%s: routeName = %s\n", __FUNCTION__, routeName);

    if (routeName == NULL) {
        errMsg = getTransDataForC(C_LANG_INDEX_ROUTE_NAME_NOT_FIND);
	setTheErrorMsg(errMsg);
        goto out;
    }

    if (strlen(routeName) != 0) {
	tmp = strtok(routeName, UTT_SPLIT_SIGN_STR);
	while(tmp != NULL) {
	    ROUTE_PRINT("%s: tmp=%s\n", __FUNCTION__, tmp);
	    /*
	     * profile change是按接口操作路由，所以删除相同接口的路由配置，只需要备份一次
	     * 例如：删除两条绑定在wan1口的路由配置，profile change只需要对wan1路由表操作一次
	     * 即可。所以，在此也只需要备份一次。否则，profile change就需要重复操作了。
	     */
	    prof = (StaticRouteProfile *)ProfGetInstPointByName(mibType, tmp);

	    if ((prof != NULL) && (lookupBPNode(pExistList, prof->interfaceName) == 0)) {
		ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profList);
		insertBPNode(&pExistList, prof->interfaceName);
		ROUTE_PRINT("%s: backup instName=%s\n", __FUNCTION__, tmp);
	    }
	    ProfDelInstByName(mibType, tmp); 
	    tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
	}
    }

    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    freeBPNodeList(pExistList);
    nvramWriteCommit();
out:
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp,"RouteSui.asp");
#else
    websRedirect(wp,"Route.asp");
#endif
    return;
}

/*
 * 列表全部删除
 */
static void formStaticRouteListDelAll(webs_t wp, char_t *path, char_t *query) {
    int min = 0, max = 0;
    int i = 0;
    int back = 0;
    MibProfileType mibType = MIB_PROF_STATIC_ROUTE;
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
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp,"RouteSui.asp");
#else
    websRedirect(wp,"Route.asp");
#endif
    return;
}

#if(STATIC_ROUTE_BIND_PPTP == FNO)
/********************************************************************
 * 函数： aspOutputDefault
 * 功能： 当宏STATIC_ROUTE_BIND_PPTP没有开启时默认用此函数以解决
 *	  页面错误问题
 * 创建： 2012-10-29
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int aspOutputDefault(int eid, webs_t wp, int argc, char_t **argv) 
{
    websWrite(wp, "var PptpServerNames = new Array();");
    websWrite(wp, "var PptpClientNames = new Array();");
    return 0;
}
#endif
/*
 * 定义页面句柄
 */
extern void websDefineStaticRoute(void) {
    websAspDefine(T("aspOutStaticRouteList"), aspOutputStaticRouteList);
    websFormDefine(T("formStaticRoute"), formStaticRoute);
    websFormDefine(T("formStaticRouteDelAll"), formStaticRouteListDelAll);
    websFormDefine(T("formStaticRouteDel"), formStaticRouteListDel);
#if (UTT_SMART_UI != FYES)
    websAspDefine(T("aspOutStaticRouteOneInfo"), aspOutputStaticRouteOneInfo);
#endif
#if(STATIC_ROUTE_BIND_PPTP == FYES)
    websAspDefine(T("aspOutputPPTPAccInfo"), aspOutputPPTPAccInfo);
#else
    websAspDefine(T("aspOutputPPTPAccInfo"), aspOutputDefault);
#endif

    return;
}

#endif
