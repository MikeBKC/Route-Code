#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#ifdef CONFIG_DEFAULTS_KERNEL_2_6_21
#include  <linux/types.h>
#include  <linux/socket.h>
#include  <linux/if.h>
#endif

#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "management.h"
#include "wireless.h"
#include "oid.h"
#include "linux/autoconf.h"
#include "uttfunction.h"
#include "mib.h"
#include "profacce.h"
#include "translate.h"

#if(IPV6_ROUTE == FYES)
static int ConverCharToInt(char *src){
    int length=0;
    while(*src!='\0'){
	length=length*10+(*src-'0');
	src++;
    }
    return length;
}

//添加或者修改单条信息执行函数.
static void formConfigIpv6RouteConfig(webs_t wp, char_t *path, char_t *query)
{
   MibProfileType mibType = MIB_PROF_IPV6_ROUTECONF;
    struProfAlloc *profhead = NULL;
    Ipv6RoutConfProfile *profRoute = NULL;
    int instIndex;
    ProfChangeType profAction;
    //struct in_addr addr_ip, addr_mask, addr_gw;
    const char *errMsg=NULL;
    char *RouteName=NULL,*RouteEnablew=NULL,*DesIP=NULL,*DesPrefix=NULL,*GateWay=NULL,*Metric=NULL,*Profile=NULL,*action=NULL;

    action = websGetVar(wp, T("Action"), "");
    RouteName = websGetVar(wp, T("RouteName"), "");
    RouteEnablew = websGetVar(wp, T("RouteEnablew"), "");
    DesIP = websGetVar(wp, T("DesIP"), "");
    DesPrefix=websGetVar(wp, T("PNPL"), "");
    GateWay = websGetVar (wp, T("GateWay"), "");
    Metric = websGetVar (wp, T("Metric"), "");
    Profile = websGetVar (wp, T("Profile"), "");

    if((action==NULL )||(RouteName==NULL) ||(RouteEnablew==NULL) ||( DesIP==NULL) || (DesPrefix==NULL) || (GateWay==NULL) ||(Metric==NULL) || (Profile==NULL))//检查新添加的prof是否合法
    {
	errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_PARAMETER_ERR);
	setTheErrorMsg(errMsg);
	goto out;
    }

    /*
     * 判断目的地址，目标地址前缀，网关是否合法
     */


    /*
     * 判断优先级
     */
   //printf("%s %s %s %s %s %s %s %s\n",action,RouteName,RouteEnablew,DesIP,DesPrefix,GateWay,Metric,Profile);
    instIndex = ProfGetInstIndexByName(mibType, RouteName);

    if (strcmp(action, "add") == 0) {
	if(instIndex != PROFFAIL) {
	    errMsg = getTransDataForC(C_LANG_INDEX_ROUTE_NAME_REPEAT);
	    setTheErrorMsg(errMsg);
	    goto out;
	}

	instIndex = ProfNewInst(mibType, RouteName, FALSE);

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

    profRoute = (Ipv6RoutConfProfile *)ProfGetInstPointByIndex(mibType, instIndex);

    ProfSetNameByPoint(profRoute, RouteName);
    /* 启用开关 */
    if (strcmp(RouteEnablew, "on") == 0) {
	profRoute->head.active = 1;
    } else {
	profRoute->head.active = 0;
    }
    memcpy(profRoute->routeName,RouteName,sizeof(profRoute->routeName));
    memcpy(profRoute->desIP,DesIP,sizeof(profRoute->desIP));
    //memcpy(profRoute->desNetPrefix,DesPrefix,sizeof(profRoute->desNetPrefix));
    profRoute->desNetPrefix = ConverCharToInt(DesPrefix);   
    memcpy(profRoute->GateWay,GateWay,sizeof(profRoute->GateWay));
    profRoute->priority=ConverCharToInt(Metric);
    memcpy(profRoute->port,Profile,sizeof(profRoute->port));

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();

out:
    websRedirect(wp, "ipv6Route.asp");
}

static void formConfigIpv6RouteConfigDel(webs_t wp, char_t *path, char_t *query){
    int num=0;
    int min, max, index = 0;
    char *ptr;
    MibProfileType mibType= MIB_PROF_IPV6_ROUTECONF;
    Ipv6RoutConfProfile *prof=NULL;
    struProfAlloc *profhead = NULL;
    const char *errMsg=NULL;
    char *routeName = NULL, *tmp = NULL;
    routeName = websGetVar(wp, T("delstr"), T(""));
    if (routeName == NULL) {
	errMsg = getTransDataForC(C_LANG_INDEX_ROUTE_NAME_NOT_FIND);
	setTheErrorMsg(errMsg);
	goto out;
    } 
    if (strlen(routeName) != 0) {
	tmp=strtok_r(routeName, UTT_SPLIT_SIGN_STR,&ptr);
	while(tmp!=NULL){
	    prof= (Ipv6RoutConfProfile *)ProfGetInstPointByName(mibType, tmp);
	    if(prof!=NULL && ProfInstIsFree(prof) == PROFOK){	
		ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profhead);
		ProfDelInstByName(mibType, tmp);/*删除已有实例*/	
	    }	
	    else
	    {
		errMsg = getTransDataForC(C_LANG_INDEX_SUBMIT_FOR_NO_LOGIN);
		setTheErrorMsgOnFree(errMsg);
	    }
	    tmp=strtok_r(NULL, ",",&ptr);
	}
    }
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();/*保存flash*/
out:
    websRedirect(wp, "ipv6Route.asp");
}

static void formConfigIpv6RouteConfigDelAll(webs_t wp, char_t *path, char_t *query)
{
    int min = 0, max = 0;
    int i = 0;
    int back = 0;
    MibProfileType mibType = MIB_PROF_IPV6_ROUTECONF;
    Pointer  *prof=NULL;
    struProfAlloc *profList = NULL;

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = ProfGetInstPointByIndex(mibType, i);
	if(prof!=NULL && (ProfInstIsFree(prof) == 0)){
	    if(back == 0) {
		ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, i, &profList);
		back=1;
	    }
	    ProfDelInstByIndex(mibType, i);
	}
    }
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
    websRedirect(wp, "ipv6Route.asp");
}
static int getIpv6RouteConfigList(int eid, webs_t wp, int argc, char_t **argv)
{
    int num=0;
    int min, max, index = 0;
    MibProfileType mibType= MIB_PROF_IPV6_ROUTECONF;
    Ipv6RoutConfProfile *prof=NULL;

    websWrite(wp,"var RouteNames=new Array();");
    websWrite(wp,"var RouteEnablews=new Array();");
    websWrite(wp,"var DesIPs=new Array();");
    websWrite(wp,"var DesPrefixs=new Array();");
    websWrite(wp,"var GateWays=new Array();");
    websWrite(wp,"var Metrics=new Array();");
    websWrite(wp,"var Profiles=new Array();");

    ProfInstLowHigh(mibType, &max, &min);
    num=min;
    for(index=min;index<max;index++){
	prof = (Ipv6RoutConfProfile *)ProfGetInstPointByIndex(mibType, index);
	if(prof!=NULL &&  (ProfInstIsFree(prof) == 0)){
	    websWrite(wp,"RouteNames[%d]=\"%s\";",num,prof->routeName);
	    if(prof->head.active==1){
		websWrite(wp,"RouteEnablews[%d]=\"%s\";",num,"1");
	    }
	    else
	    {
		websWrite(wp,"RouteEnablews[%d]=\"%s\";",num,"0");
	    }
	    websWrite(wp,"DesIPs[%d]=\"%s\";",num,prof->desIP);
	    websWrite(wp,"DesPrefixs[%d]=\"%d\";",num,prof->desNetPrefix);
	    websWrite(wp,"GateWays[%d]=\"%s\";",num,prof->GateWay);
	    websWrite(wp,"Metrics[%d]=\"%d\";",num,prof->priority);
	    websWrite(wp,"Profiles[%d]=\"%s\";",num,prof->port);
	    num++;
	}
    }
    websWrite(wp,"var totalrecs=%d;", num);
    websWrite(wp,"var max_totalrecs=%d;", MAX_IPV6_ROUTE_PROFILES);/*最大条目数，即性能指标*/
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
static int aspOutputIpv6RouteConfigOneInfo(int eid, webs_t wp, int argc, char_t **argv) {
    MibProfileType mibType = MIB_PROF_IPV6_ROUTECONF;
    Ipv6RoutConfProfile *profRoute = NULL;
    int min = 0, max = 0, i = 0;

    char *editName;
    const char *errMsg=NULL;

    if (ejArgs(argc, argv, T("%s"), &editName) < 1) {
	errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_READ_DATA_ERR);
	setTheErrorMsg(errMsg);
    } else {
	ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++) {
	    profRoute = (Ipv6RoutConfProfile *)ProfGetInstPointByIndex(mibType, i);
	    if((profRoute != NULL) && (ProfInstIsFree(profRoute) == 0) &&
		    (editName != NULL) && (strcmp(editName, profRoute->head.name) == 0)) {
		websWrite(wp, "RouteNames = \"%s\";", profRoute->routeName);
		websWrite(wp, "RouteEnables = \"%d\";", profRoute->head.active);
		websWrite(wp, "DesIPs = \"%s\";", profRoute->desIP);
		websWrite(wp, "PNPL = \"%d\";", profRoute->desNetPrefix);
		websWrite(wp, "GateWays = \"%s\";", profRoute->GateWay);
		websWrite(wp, "Metrics = \"%d\";", profRoute->priority);
		websWrite(wp, "Profiles = \"%s\";", profRoute->port);
		break;
	    }
	}
    }

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

/***********************************************************************
 * 函 数 名：   formDefineIpv6RouteConfig
 * 创建日期：	2014-9-24
 * 修改日期：
 * 附加说明：	无
 ***********************************************************************/
extern  void formDefineIpv6RouteConfig(void)
{
    websFormDefine(T("ConfigIpv6RouteConfig"), formConfigIpv6RouteConfig);
    websFormDefine(T("ConfigIpv6RouteConfigDel"), formConfigIpv6RouteConfigDel);
    websFormDefine(T("ConfigIpv6RouteConfigDelAll"), formConfigIpv6RouteConfigDelAll);
    websAspDefine(T("outputIpv6RouteConfigConfig"), getIpv6RouteConfigList);
    websAspDefine(T("aspOutputIpv6RouteConfigOneInfo"), aspOutputIpv6RouteConfigOneInfo);
}
#endif
