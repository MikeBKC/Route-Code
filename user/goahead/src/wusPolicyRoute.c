/*
 *	firewall.c -- Firewall Settings 
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: wusPolicyRoute.c,v 1.6 2013/03/26 11:21:00 guo.deyuan Exp $
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "mib.h"
#include "profacce.h"
#include "uttfunction.h"
#include "translate.h"



#if (POLICY_ROUTE == FYES)
#if (IP_GRP== FYES)
extern int getIpGrpList(int eid, webs_t wp, int argc, char_t **argv);
#endif
#define DBGPRINT policy_route_debug_print
extern int moveInstToFirst(char *sID, char * tID, MibProfileType proftype);
/***********************************************************************
 * 函 数 名：formPolicyRouteMove_Config
 * 功能描述：插入函数
 * 输入参数：null
 * 输出参数：null
 * 创建日期：2013-03-06
 * 修改日期：
 * 作者：   
 * 附加说明：
 **********************************************************************/

void formPolicyRouteMove_Config(webs_t wp, char *path, char *query)
{
    char *sID,*locationID;
    MibProfileType proftype;

    sID = websGetVar(wp, T("sID"), NULL);
    locationID= websGetVar(wp, T("tID"),NULL);				/* 是什么位置之前*/
    proftype= MIB_PROF_POLICY_ROUTE;

    DBGPRINT("%s-%d: sID =%s, tID =%s\n",__func__,__LINE__,sID,locationID);
    if(strcmp(sID,locationID) == 0)
    {
	goto out;
    }
    if(moveInstToFirst(sID, locationID, proftype)) {
	nvramWriteCommit();
    }
out:
	websRedirect(wp, "PolicyRoute.asp");
}
 /**
  * 219  * 把时间转换为 秒
  * 220  */
static Uint32 converTimeToSec(char *time)
{
    Uint32 hour = 0u, mintue = 0u, second = 0u;
    sscanf(time, "%u:%u", &hour, &mintue);/*读入时分*/
    second =  (hour*60 + mintue) *60;
    return second;

}
static char *converSecToTime(Uint32 time)
{
    Uint32 min = 0U, hour = 0U;
    char strTime[UTT_TIME_LEN+1];

    memset(strTime, 0, sizeof(strTime));
    if(time == 0U) {
	snprintf(strTime,UTT_TIME_LEN,"%u:%u",00U,00U);
    }else{
	min = time/60;
	hour = min/60;
	snprintf(strTime,UTT_TIME_LEN,"%u:%u",hour,min-hour*60);
    }
    return strTime;
}
static void formPolicyRouteGlobalConfig(webs_t wp, char_t *path, char_t *query)
{
    char *prEnable = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    FunEnableEnum enable = FUN_DISABLE;
    struProfAlloc *profhead = NULL;

    /* get web value */
    prEnable = websGetVar(wp, "GlobalEnable", T(""));
    DBGPRINT("%s-%d: prEnable =%s\n", __func__, __LINE__,prEnable);
    
    /* get SystemProfile */
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);
    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
    
    if(strcmp(prEnable, "on") == 0) {
        enable = FUN_ENABLE;
    }
    profSys->policyrouteEn = enable;
    DBGPRINT("%s-%d: profSys->enable =%d\n", __func__, __LINE__,profSys->policyrouteEn);
    
    /* update systemProfile
     * and write to flash
     */
#if 1
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
#endif
    websRedirect(wp, "PolicyRoute.asp");/*页面跳转*/
    return;

}

/**
 * 判断ip组是否被策略路由引用了
 * return :
 *	1 被引用了
 *	0 未被引用
 **/
int isIpGrpReredByPolicyRoute(IpGrpProfile *ipProf)
{
    MibProfileType mibType = MIB_PROF_POLICY_ROUTE;
    PolicyRouteProfile *prof = NULL;
    int ret = 0;
    int max, min, i;
    
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
    prof = (PolicyRouteProfile*)ProfGetInstPointByIndex(mibType, i);
    if(ProfInstIsFree(prof) == 0) {
	if(prof->ip.srcIP.entryType == PR_IP_GROUP)
	{/*源地址*/
	    if(strcmp(ipProf->head.name, prof->ip.srcIP.entryVal.name) == 0)
	    {/*被引用了*/
		ret = 1;
		break;
	    }
	}
	if(prof->ip.dstIP.entryType == PR_IP_GROUP)
	{/*目的地址*/
	    if(strcmp(ipProf->head.name, prof->ip.dstIP.entryVal.name) == 0)
	    {/*被引用了*/
		ret = 1;
		break;
	    }
	}
    }
    }
    return ret;
}
/***********************************************************************
 * 函 数 名：   formPolicyRouteDelAll
 * 创建日期：	2011-11-02
 * 修改日期：
 * 作者：       yqyang
 * 附加说明：	无
 ***********************************************************************/
static void formPolicyRouteDelAll(webs_t wp, char_t *path, char_t *query)
{ 
    int min = 0, max = 0;
    int i = 0;
    int back = 0;
    MibProfileType mibType = MIB_PROF_POLICY_ROUTE;
    Pointer prof;
    struProfAlloc *profList  = NULL;

    a_assert(wp);
    ProfInstLowHigh(mibType, &max, &min);

    for(i = min; i < max; i++) {
	prof = (PolicyRouteProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
	    if(back == 0) {
		ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, i, &profList);
		back = 1;
	    }
	    ProfDelInstByIndex(mibType, i);
	}
    }
#if 1
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
#endif
    websRedirect(wp, "PolicyRoute.asp");
}

/***********************************************************************
 * 函 数 名：   formPolicyRouteDel
 * 创建日期：	2013-02-27
 * 修改日期：
 * 作者：       
 * 附加说明：	无
 ***********************************************************************/
static void formPolicyRouteDel(webs_t wp, char_t *path, char_t *query)
{
    char_t *PolicyName=NULL,*tmp,*ptr;
    a_assert(wp);
    MibProfileType mibType = MIB_PROF_POLICY_ROUTE;
    struProfAlloc *profList  = NULL;
    int back = 0;
	const char* ErrMsg = NULL;

    PolicyName = websGetVar(wp,T("delstr"),T("")); 
    if(!strcmp(PolicyName,T(""))) {   
		ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_STRA_NAME_GN);
		setTheErrorMsg(ErrMsg);
		goto out;
    }
    tmp=strtok_r(PolicyName,UTT_SPLIT_SIGN_STR,&ptr);
    DBGPRINT("%s-%d: policyname =%s\n", __func__, __LINE__,PolicyName);
    while(tmp!=NULL) {
	if(back == 0){
	    ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profList);
	    back = 1;
	}
	ProfDelInstByName(mibType, tmp); 
	tmp=strtok_r(NULL,UTT_SPLIT_SIGN_STR,&ptr);
    }
#if 1
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
#endif
out:
    websRedirect(wp,"PolicyRoute.asp");
}
/***********************************************************************
 * 函 数 名：   getOnePolicyRouteInfo
 * 创建日期：	2013-02-27
 * 修改日期：
 * 作者：       
 * 附加说明：	无
 ***********************************************************************/
static int getOnePolicyRouteInfo(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t *edit_name; 
    PolicyRouteProfile *prof;
    UINT32 d_temp;
    char_t data[16];
    MibProfileType mibType = MIB_PROF_POLICY_ROUTE;
	const char *ErrMsg = NULL;

    if(ejArgs(argc,argv,T("%s"), &edit_name) <1) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_READ_EDIT_NAME_VAL_ERR);
		setTheErrorMsg(ErrMsg);
		goto out;
    }
    if(edit_name == NULL) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_STRATEGY_NAME_CANT_NULL);
		setTheErrorMsg(ErrMsg);
		goto out;
    }

    prof = (PolicyRouteProfile*)ProfGetInstPointByName(mibType, edit_name);
    if(prof == NULL) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_THE_GROP_CONF);
		setTheErrorMsg(ErrMsg);
    } else {
	websWrite(wp,"var routeNames = \"%s\";",prof->head.name);
	websWrite(wp,"var PolicyEnables = \"%d\";",prof->head.active);
	websWrite(wp,"var Profile = \"%s\";",prof->interfaceName);
	websWrite(wp,"var bind = %d;",prof->bindMark);
	if(prof->ip.srcIP.entryType == PR_IP_RANGE)
	{
	    websWrite(wp,"var srcIPs = \"%s\";",T("ipRange"));
	    websWrite(wp,"var srcipFrom = \"%N\";",prof->ip.srcIP.entryVal.range.ipFrom);
	    websWrite(wp,"var srcipEnd = \"%N\";",prof->ip.srcIP.entryVal.range.ipTo);
	    //websWrite(wp,"var srcipGrps = \"\";",T(""));
	}
	else
	{
#if (IP_GRP == FYES)
	    websWrite(wp,"var srcIPs = \"%s\";",T("groupSel"));
	    websWrite(wp,"var srcipGrps = \"%s\";",prof->ip.srcIP.entryVal.name);

#endif
	}
	if(prof->ip.dstIP.entryType == PR_IP_RANGE)
	{
	    websWrite(wp,"var dstIPs = \"%s\";",T("ipRange"));
	    websWrite(wp,"var dstipFrom = \"%N\";",prof->ip.dstIP.entryVal.range.ipFrom);
	    websWrite(wp,"var dstipEnd = \"%N\";",prof->ip.dstIP.entryVal.range.ipTo);
	    //websWrite(wp,"var dstipGrps = \"\";",T(""));
	}
	else
	{
#if (IP_GRP == FYES)
	    websWrite(wp,"var dstIPs = \"%s\";",T("groupSel"));
	    websWrite(wp,"var dstipGrps = \"%s\";",prof->ip.dstIP.entryVal.name);

#endif
	}
	
	websWrite(wp,"var protocols = \"%d\";",prof->apply.protocol);
	websWrite(wp,"var fromport = \"%d\";",prof->apply.dstFromPort);
	websWrite(wp,"var toport = \"%d\"; ",prof->apply.dstEndPort);

	d_temp = prof->prtime.wday;
	Byte2Days(d_temp,data);
	websWrite(wp,"var pr_days = \"%s\";",data);
	websWrite(wp,"var pr_timestarts = \"%s\";",prof->prtime.timestart_str);
	websWrite(wp,"var pr_timestops = \"%s\";",prof->prtime.timestop_str);
    }
out:
    websWrite(wp,"var max_totalrecs =%d;",MAX_POLICY_ROUTE_PROFILES);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
/***********************************************************************
 * 函 数 名：   getPolicyRouteConf
 * 创建日期：	2013-02-27
 * 修改日期：
 * 作者：       
 * 附加说明：	无
 ***********************************************************************/
static int getPolicyRouteConf(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t data[28]={0};
    int min = 0, max = 0;
    int i = 0;
    int num = 0;
    UINT32 d_temp;
    MibProfileType mibType = MIB_PROF_POLICY_ROUTE;
    PolicyRouteProfile *prof = NULL;
    SystemProfile *profSys = NULL;
    const char* ErrMsg = NULL;

    websWrite(wp,"var routeNames=new Array();");
    websWrite(wp,"var bind=new Array();");
    websWrite(wp,"var Profile=new Array();");
    websWrite(wp,"var Allows = new Array();");
    websWrite(wp,"var srcIPs = new Array();");
    websWrite(wp,"var srcipFrom = new Array();");
    websWrite(wp,"var srcipEnd = new Array();");
    websWrite(wp,"var dstIPs=new Array();");
    websWrite(wp,"var dstipFrom = new Array();");
    websWrite(wp,"var dstipEnd = new Array();");
#if (IP_GRP == FYES)
    websWrite(wp,"var srcipGrps = new Array();");
    websWrite(wp,"var dstipGrps = new Array();");
#endif
    websWrite(wp,"var protocols = new Array();");
    websWrite(wp,"var fromport = new Array();");
    websWrite(wp,"var toport = new Array();");
    websWrite(wp,"var pr_days = new Array();");
    websWrite(wp,"var pr_timestarts=new Array();");
    websWrite(wp,"var pr_timestops=new Array();");
    websWrite(wp,"\r\n");

    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    if(profSys->policyrouteEn == FUN_ENABLE) {
	websWrite(wp,"var prEnable = \"%s\";",T("on"));
    } else {
	websWrite(wp,"var prEnable = \"%s\";",T("off"));
    }

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = (PolicyRouteProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && (ProfInstIsFree(prof) == 0)) {

	websWrite(wp,"routeNames[%d] = \"%s\";",num,prof->head.name);
	if(prof->head.active == FUN_ENABLE) {
	    websWrite(wp,"Allows[%d] = \"%s\";",num,T("yes"));
	} else {
	    websWrite(wp,"Allows[%d] = \"%s\";",num,T("no"));
	}
	websWrite(wp,"Profile[%d] = \"%s\";",num,prof->interfaceName);
	websWrite(wp,"bind[%d] = %d;",num,prof->bindMark);
	if(prof->ip.srcIP.entryType == PR_IP_RANGE)
	{
	    websWrite(wp,"srcIPs[%d] = \"%s\";",num,T("ipRange"));
	    websWrite(wp,"srcipFrom[%d] = \"%N\";",num,prof->ip.srcIP.entryVal.range.ipFrom);
	    websWrite(wp,"srcipEnd[%d] = \"%N\";",num,prof->ip.srcIP.entryVal.range.ipTo);
	    websWrite(wp,"srcipGrps[%d] = \"\";",num,T(""));
	}
	else
	{
#if (IP_GRP == FYES)
	    websWrite(wp,"srcIPs[%d] = \"%s\";",num,T("groupSel"));
	    if(strlen(prof->ip.srcIP.entryVal.name) == 0)
	    {
		ErrMsg = getTransDataForC(C_LANG_INDEX_ALL_USERS);
		websWrite(wp,"srcipGrps[%d] = \"%s\";",num, ErrMsg);
	    }
	    else
	    {
		websWrite(wp,"srcipGrps[%d] = \"%s\";",num,prof->ip.srcIP.entryVal.name);
	    }

#endif
	}
	if(prof->ip.dstIP.entryType == PR_IP_RANGE)
	{
	    websWrite(wp,"dstIPs[%d] = \"%s\";",num,T("ipRange"));
	    websWrite(wp,"dstipFrom[%d] = \"%N\";",num,prof->ip.dstIP.entryVal.range.ipFrom);
	    websWrite(wp,"dstipEnd[%d] = \"%N\";",num,prof->ip.dstIP.entryVal.range.ipTo);
	    websWrite(wp,"dstipGrps[%d] = \"\";",num,T(""));
	}
	else
	{
#if (IP_GRP == FYES)
	    websWrite(wp,"dstIPs[%d] = \"%s\";",num,T("groupSel"));
	    if(strlen(prof->ip.dstIP.entryVal.name) == 0)
	    {
		ErrMsg = getTransDataForC(C_LANG_INDEX_ALL_USERS);
		websWrite(wp,"dstipGrps[%d] = \"%s\";",num, ErrMsg);
	    }
	    else
	    {
		websWrite(wp,"dstipGrps[%d] = \"%s\";",num,prof->ip.dstIP.entryVal.name);
	    }

#endif
	}
	
	websWrite(wp,"protocols[%d] = \"%d\";",num,prof->apply.protocol);
	websWrite(wp,"fromport[%d] = \"%d\";",num,prof->apply.dstFromPort);
	websWrite(wp,"toport[%d] = \"%d\"; ",num,prof->apply.dstEndPort);

	d_temp = prof->prtime.wday;
	Byte2Days(d_temp,data);
	websWrite(wp,"pr_days[%d] = \"%s\";",num,data);
	websWrite(wp,"pr_timestarts[%d] = \"%s\";",num,prof->prtime.timestart_str);
	websWrite(wp,"pr_timestops[%d] = \"%s\";",num,prof->prtime.timestop_str);
	num++;
	}
    }
    getTheErrorMsg(eid, wp, argc, argv);
    websWrite(wp,"var totalrecs=%d;",num);
    websWrite(wp,"var max_totalrecs =%d;",MAX_POLICY_ROUTE_PROFILES);
    return 0;
}
/*
 * formConfigIPSecAllow
 * /goform/formIPSecAllow
 * set profile allow or forbidden
 */
static void  formConfigPolicyRouteAllow(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibtype = MIB_PROF_POLICY_ROUTE;
    PolicyRouteProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    FunEnableEnum allowEn = FUN_ENABLE;
    int instIndex = -1;

    char *routeName = NULL;
    char *allow = NULL;
    const char *errmsg = NULL;

    a_assert(wp);
    routeName=websGetVar(wp,T("AllowID"), T(""));/*用户名*/
    allow=websGetVar(wp,T("Allow"), T(""));/*允许*/
    instIndex = ProfGetInstIndexByName(mibtype, routeName);
    if(instIndex == PROFFAIL) 
    {/*欲修改的配置不存在*/
	errmsg = getTransDataForC(C_LANG_INDEX_THE_CONFIG_NOT_FIND);
	setTheErrorMsgOnFree(errmsg);
    }
    else
    {	/*要操作的配置存在*/
	DBGPRINT("%s-%d: instIndex=%d\n",__FUNCTION__,__LINE__,instIndex);
	prof = (PolicyRouteProfile *)ProfGetInstPointByIndex(mibtype, instIndex);
	if((prof!=NULL) && (ProfInstIsFree(prof) == 0))
	{
	    if (0 == strcmp("no",allow))/*禁止该绑定用户*/
	    {
		allowEn = FUN_DISABLE;
	    }
	    DBGPRINT("%s-%d: allow=%s,allowEn=%d\n",__FUNCTION__,__LINE__,allow,allowEn);
	    /* modify  */
	    profAction = PROF_CHANGE_EDIT;
	    if(prof->head.active != allowEn)
	    {
		ProfBackupByIndex(mibtype, profAction, instIndex, &profhead);
		prof->head.active = allowEn;
		DBGPRINT("%s-%d: active=%d\n",__FUNCTION__,__LINE__,prof->head.active);
		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
		nvramWriteCommit();		/*save*/
	    }
	}
    }

#if 1
#endif
    websRedirect(wp, "PolicyRoute.asp");
    return;
}

/***********************************************************************
 * 函 数 名：   formFilterList
 * 创建日期：	2011-11-02
 * 修改日期：
 * 作者：       yqyang
 * 附加说明：	无
 ***********************************************************************/
static void formPolicyRouteConf(webs_t wp, char_t *path, char_t *query)
{
    char_t *PolicyName,*PolicyNameOld,*Enable,
	   *srcFromIp,*srcEndIp,*GroupName,*sourceIP,*Status,*dstFromIp,*dstEndIp,*dstGroupName,*dstIP,*profile;
    char_t *Protocol = 0,*dstFromPort = 0,*dstEndPort = 0, *day,*timestart,*timestop;
    char_t *action;
    struct in_addr addr_SrcFromIp,addr_SrcEndIp,addr_DstFromIp,addr_DstEndIp;
    unsigned int nSrcFromIp,nSrcEndIp,nDstFromIp = 0,nDstEndIp = 0;
    int instIndex;
	const char *ErrMsg = NULL;
    a_assert(wp);

    MibProfileType mibType = MIB_PROF_POLICY_ROUTE;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    PolicyRouteProfile *prof= NULL;

    PolicyName = websGetVar(wp,T("PolicyName"), T(""));
    PolicyNameOld = websGetVar(wp, T("PolicyNameold"), T("")); 
    Enable = websGetVar(wp,T("PolicyEnable"), T(""));
    profile = websGetVar(wp,T("Profile"), T(""));
    DBGPRINT("%s-%d: policyname =%s,policynameold =%s, enable =%s\n", __func__, __LINE__,PolicyName,PolicyNameOld,Enable);
    sourceIP = websGetVar(wp,T("sourceIP"), T(""));
    srcFromIp=websGetVar(wp,T("FromIP"), T(""));
    srcEndIp=websGetVar(wp,T("EndIP"), T(""));
    GroupName = websGetVar(wp,T("GroupName"), T("")); 
    DBGPRINT("%s-%d: sourceIP =%s,srcfromip =%s,srcendip =%s,groupname =%s\n", __func__, __LINE__,sourceIP,srcFromIp,srcEndIp,GroupName);
    dstIP = websGetVar(wp,T("dstIP"), T(""));
    dstFromIp=websGetVar(wp,T("dstFromIP"), T(""));
    dstEndIp=websGetVar(wp,T("dstEndIP"), T(""));
    dstGroupName = websGetVar(wp,T("dstGroupName"), T("")); 
    DBGPRINT("%s-%d: dstIP =%s,dstfromip =%s,dstendip =%s,dstgroupname =%s\n", __func__, __LINE__,dstIP,dstFromIp,dstEndIp,dstGroupName);
    Protocol=websGetVar(wp,T("Protocol"), T(""));
    dstFromPort=websGetVar(wp,T("DesFromPort"), T(""));
    dstEndPort=websGetVar(wp,T("DesEndPort"), T(""));
    DBGPRINT("%s-%d:  protocol =%s,dstfromport =%s,dstendport =%s\n", __func__, __LINE__,Protocol,dstFromPort,dstEndPort);

    nSrcFromIp=ip2int(srcFromIp);
    nSrcEndIp=ip2int(srcEndIp);

    if((strcmp(sourceIP, "ipRange") == 0) &&((inet_aton(srcFromIp, &addr_SrcFromIp) == 0) || (inet_aton(srcEndIp, &addr_SrcEndIp) == 0)))
    {
		ErrMsg = getTransDataForC(C_LANG_INDEX_IP_PLAN_ADDR_ERR);
		setTheErrorMsg(ErrMsg);
		goto out;
    }
    DBGPRINT("%s-%d: nSrcFromIp =0x%x,nSrcEndIp =0x%x,srcfromip =0x%x,srcendip =0x%x\n", __func__, __LINE__,nSrcFromIp,nSrcEndIp,addr_SrcFromIp.s_addr,addr_SrcEndIp.s_addr);

    nDstFromIp=ip2int(dstFromIp);
    nDstEndIp=ip2int(dstEndIp);
    if((strcmp(dstIP, "ipRange") == 0) &&((inet_aton(dstFromIp, &addr_DstFromIp) == 0) || (inet_aton(dstEndIp, &addr_DstEndIp) == 0)) ){  
	ErrMsg = getTransDataForC(C_LANG_INDEX_IP_PLAN_ADDR_ERR);
	setTheErrorMsg(ErrMsg);
	goto out;
    }
    DBGPRINT("%s-%d: nDstFromIp =0x%x,nDstEndIp =0x%x,dstfromip =0x%x,dstendip =0x%x\n", __func__, __LINE__,nDstFromIp,nDstEndIp,addr_DstFromIp.s_addr,addr_DstEndIp.s_addr);

    day=websGetVar(wp,T("day"),T(""));
    timestart=websGetVar(wp,T("timestart"), T(""));
    timestop=websGetVar(wp,T("timestop"), T(""));
    action = websGetVar(wp, T("Action"), T("")); 
    DBGPRINT("%s-%d:  day =%s,timestart =%s,timestop =%s,action =%s\n", __func__, __LINE__,day,timestart,timestop,action);

    instIndex = ProfGetInstIndexByName(mibType, PolicyNameOld);
    if(strcmp(action,"add") == 0) {
	if(instIndex != PROFFAIL)
	{
		ErrMsg = getTransDataForC(C_LANG_INDEX_STRATEGY_NAME_REPEATED);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	instIndex = ProfNewInst(mibType, PolicyName, FALSE);
	if(instIndex == PROFFAIL) 
	{
		ErrMsg = getTransDataForC(C_LANG_INDEX_MAXIMUM_NUM_HAS_REACHED);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	profAction = PROF_CHANGE_ADD;

    } else {//modify

	if(instIndex == PROFFAIL) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_STRA_NAME);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	if (strcmp(PolicyName ,PolicyNameOld) != 0){
	    int newIndex;
	    newIndex = ProfGetInstIndexByName(mibType, PolicyName);
	    if(newIndex != PROFFAIL)
	    {
			ErrMsg = getTransDataForC(C_LANG_INDEX_STRATEGY_NAME_EXISTS);
			setTheErrorMsg(ErrMsg);
			goto out;
	    }
	}
	profAction = PROF_CHANGE_EDIT;
    }
    ProfBackupByIndex(mibType, profAction, instIndex, &profhead);
    prof = (PolicyRouteProfile*)ProfGetInstPointByIndex(mibType, instIndex);
    ProfSetNameByPoint(prof, PolicyName);
    prof->head.active = strtoul(Enable,0,10);
    if(strlen(profile) != 0U) {
	strncpy(prof->interfaceName, profile,UTT_INTERFACE_LEN);
    } else {
	ErrMsg = getTransDataForC(C_LANG_INDEX_CINFIG_PARAMETER_WRONG);
	setTheErrorMsg(ErrMsg);
	goto out;
    }
    prof->bindMark = strtoul(profile+3,0,10);  //need modify
    DBGPRINT("%s-%d: prof->name =%s,prof->enable =%d,instindex =%d\n",__func__, __LINE__,prof->head.name,prof->head.active,instIndex);
    if(strcmp(sourceIP, "ipRange") == 0)
    {
	prof->ip.srcIP.entryType = PR_IP_RANGE; 
	prof->ip.srcIP.entryVal.range.ipFrom = addr_SrcFromIp.s_addr; 
	prof->ip.srcIP.entryVal.range.ipTo = addr_SrcEndIp.s_addr; 
	DBGPRINT("%s-%d: prof->srcip.ipfrom =0x%x,prof->srcip.ipto =0x%x\n",__func__, __LINE__,prof->ip.srcIP.entryVal.range.ipFrom,prof->ip.srcIP.entryVal.range.ipTo);
    }
    else
    {
#if (IP_GRP == FYES)
	prof->ip.srcIP.entryType = PR_IP_GROUP; 
	strcpy(prof->ip.srcIP.entryVal.name, GroupName);
#endif
    }
    if(strcmp(dstIP, "ipRange") == 0)
    {
	prof->ip.dstIP.entryType = PR_IP_RANGE; 
	prof->ip.dstIP.entryVal.range.ipFrom = addr_DstFromIp.s_addr; 
	prof->ip.dstIP.entryVal.range.ipTo = addr_DstEndIp.s_addr; 
	DBGPRINT("%s-%d: prof->dstip.ipfrom =0x%x,prof->dstip.ipto =0x%x\n",__func__, __LINE__,prof->ip.dstIP.entryVal.range.ipFrom,prof->ip.dstIP.entryVal.range.ipTo);
    }
    else
    {
#if (IP_GRP == FYES)
	prof->ip.dstIP.entryType = PR_IP_GROUP; 
	strcpy(prof->ip.dstIP.entryVal.name, dstGroupName);
#endif
    }

#if (IP_GRP == FYES)
    DBGPRINT("%s-----%d,prof->srcGrpName = %s, srcGroupName = %s,prof->dstGrpName = %s, dstGroupName = %s\n", __func__, __LINE__, prof->ip.srcIP.entryVal.name, GroupName,prof->ip.dstIP.entryVal.name,dstGroupName);
#endif

    prof->apply.protocol = strtoul(Protocol,0,10); 
    prof->apply.dstFromPort = strtoul(dstFromPort,0,10);
    prof->apply.dstEndPort = strtoul(dstEndPort,0,10);
    DBGPRINT("%s-%d: prof->protocol =%d,prof->dstfromport =%d,prof->dstendport =%d\n", __func__, __LINE__, prof->apply.protocol,prof->apply.dstFromPort,prof->apply.dstEndPort);

    prof->prtime.wday = days2Byte(day); 
    strcpy(prof->prtime.timestart_str, timestart); 
    strcpy(prof->prtime.timestop_str, timestop); 
    if(strcmp(prof->prtime.timestart_str, "All") == 0)
    {
	prof->prtime.timestart = 0U;/*从零秒开始*/ 
	prof->prtime.timestop = converTimeToSec("24:00"); /*最后一秒结束*/
    }
    else
    {
	/*转换为秒*/
	prof->prtime.timestart = converTimeToSec(prof->prtime.timestart_str); 
	prof->prtime.timestop = converTimeToSec(prof->prtime.timestop_str); 
    }
    DBGPRINT("%s-%d: prof->wday =%d,,prof->timestart =%d,prof->timestop =%d\n", __func__, __LINE__, prof->prtime.wday,prof->prtime.timestart,prof->prtime.timestop);
#if 1
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
#endif
out:
    websRedirect(wp, "PolicyRoute.asp");
}
/***********************************************************************
 * 函 数 名：   formDefineNatRuleConfig
 * 创建日期：	2011-11-02
 * 修改日期：
 * 作者：       yqyang
 * 附加说明：	无
 ***********************************************************************/
void formDefinePolicyRouteConfig(void)
{
    websFormDefine(T("formPolicyRouteGlobalConfig"),formPolicyRouteGlobalConfig);
    websFormDefine(T("formPolicyRouteConf"),formPolicyRouteConf);
    websFormDefine(T("formPolicyRouteDel"),formPolicyRouteDel);
    websFormDefine(T("formPolicyRouteDelAll"),formPolicyRouteDelAll);
    websFormDefine(T("formPolicyRouteAllow"),formConfigPolicyRouteAllow);
    websAspDefine(T("aspOutPolicyRouteConf"),getPolicyRouteConf);
    websAspDefine(T("getOnePolicyRouteInfo"),getOnePolicyRouteInfo);
    websFormDefine(T("formPolicyRouteMove_Config"), formPolicyRouteMove_Config);
#if (IP_GRP == FYES)
    websAspDefine(T("aspOutIpGrpList"),getIpGrpList);
#endif
#if 0
    websFormDefine(T("Move_Config"),formMove_Config);
#endif
}
#endif
