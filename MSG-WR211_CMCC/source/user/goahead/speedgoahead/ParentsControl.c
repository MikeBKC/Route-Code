#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "uttMachine.h"
#include "typedef.h"
#include "ifmisc.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"
#include "spdComFun.h"
#include "pdb.h"

#if (PARENTS_CONTROL == FYES)
#define DBGPRINT_PARENTS(fmt, args...)
#define MAC_NULL_STR "00:00:00:00:00:00"

#define PARENTS_FILTER_INPUT_CHAIN "parents_control_input_chain"
#define PRE_PARENTS_FILTER_INPUT_CHAIN "pre_parents_input_chain"
#define PARENTS_FILTER_CHAIN "parents_control_chain"
#define PRE_PARENTS_FILTER_CHAIN "pre_parents_control_chain"

static void parentsControlProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const ParentsControlProfile *oldProfileDataPtr, const ParentsControlProfile *newProfileDataPtr );
static void parentsControlglobalProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr );
static unsigned int isParentsControlEnable(void);
static void parentsControlChange(ParentsControlProfile *prof, char action);
#if (TIME_GRP == FYES)
static void parentsControlforTime(TimeGrpProfile *timeGrpProf, char *macStr, char action);
static void parentsControlUrlforTime(TimeGrpProfile *timeGrpProf, char *macStr, char *url, char action);
#else
static void parentsControlforTime(char *macStr, char action);
static void parentsControlUrlforTime(char *macStr, char *url char action);
#endif
static void parentsControlDelAll(void);
static void parentscontrolInit(void);


#if (UTT_SMART_UI == FYES)
/*新UI 不使用时间段，直接设置时间*/
static void parentsControlUrlforTimeSui(ParentsControlProfile *prof, char *macStr, char *url, char action)
{
    char *daystr = NULL;
    char *timestr = NULL;

    if(prof != NULL) 
    {
	daystr = getDayForIptables(prof->day);
	timestr = getTimeForIptables(prof->timeStart, prof->timeStop);
	/*mantis:13493,时间和日期都是全选，则不加入时间和日期到iptables规则中*/
	if (prof->day == 127U && !strcmp(prof->timeStart, "All"))
	{
	    strcpy(daystr, "");
	    strcpy(timestr, "");
	}
	doSystem("iptables -%c %s -m mac --mac-source %s %s %s -m domain --name \"%s\" -j ACCEPT >/dev/null 2>&1",action,PARENTS_FILTER_INPUT_CHAIN,macStr,timestr,daystr,url); /*maybe need change*/
	DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m mac --mac-source %s %s %s -m domain --name \"%s\" -j ACCEPT\n",__func__,__LINE__,action,PARENTS_FILTER_INPUT_CHAIN,macStr,timestr,daystr,url); /*maybe need change*/
	doSystem("iptables -%c %s -m mac --mac-source %s %s %s -m domain --name \"%s\" -j ACCEPT >/dev/null 2>&1",action,PARENTS_FILTER_CHAIN,macStr,timestr,daystr,url); /*maybe need change*/
	DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m mac --mac-source %s %s %s -m domain --name \"%s\" -j ACCEPT\n",__func__,__LINE__,action,PARENTS_FILTER_CHAIN,macStr,timestr,daystr,url); /*maybe need change*/
    }
}
#endif


#if (TIME_GRP == FYES)
/********************************************************************
* 函数： parentsControlOnTimeGrp
* 功能： 随时间组变化的家长控制规则变化
* 创建： 2013-05-10
* 参数： 略
* 返回： 略
* 输出： 略
* 作者：  
********************************************************************/
extern void parentsControlOnTimeGrp(TimeGrpProfile *oldProf, TimeGrpProfile *newProf)
{
    MibProfileType mibType = MIB_PROF_PARENTS_CONTROL;
    ParentsControlProfile *prof = NULL;
    int min = 0, max = 0, index = 0, j = 0;
    char_t macStr[18];
    char action = 'A';
    if(isParentsControlEnable() == 1U) {
	
	ProfInstLowHigh(mibType, &max, &min);
	for (index = min; index < max; index++) 
	{
	    prof = (ParentsControlProfile*)ProfGetInstPointByIndex(mibType, index); /*get prof pointer*/
	    if((prof != NULL) && (ProfInstIsFree(prof) == 0) 
		    && (prof->head.active == TRUE))
	    {
		if(strcmp(prof->timeGrp, newProf->head.name) == 0U) {	/*时间组被引用*/
			memset(macStr, 0U, sizeof(macStr));
			strncpy(macStr, converMac6To17Byte(prof->mac), 17U);
			DBGPRINT_PARENTS("%s-%d: macStr(17Byte) %s\n",__func__, __LINE__,macStr);
			if((memcmp(macStr, MAC_NULL_STR, 17U) != 0)) /*check mac is null or not*/
			{
			    for(j=0;j<MAX_ALLOW_URL_ITEM;j++){
			    if(prof->allowURL[j].url[0] != '\0'){
				parentsControlUrlforTime(oldProf,macStr,prof->allowURL[j].url,Iptables_DEL);
				parentsControlUrlforTime(newProf,macStr,prof->allowURL[j].url,Iptables_INS);
#if 0
				parentsControlforTime(oldProf, macStr, Iptables_DEL);
				parentsControlforTime(newProf, macStr, Iptables_INS);
#endif
			    }
			    }
			}
		}
	    }
	}
    }
}
#endif
/********************************************************************
* 函数： isParentsControlEnable(void)
* 功能： 判断家长控制总开关是否打开
* 创建： 2013-05-10
* 参数： 略
* 返回： 略
* 输出： 略
* 作者：  
********************************************************************/
static unsigned int isParentsControlEnable(void)
{
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    unsigned int ret = 0U;

    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibTypeSys, 0);
    if(profSys->parentControlEn.parentsEn == FUN_ENABLE) 
    {
	ret = 1U;
    }
    DBGPRINT_PARENTS("%s-%d: ret =%d\n",__func__, __LINE__,ret);
    return ret;
}
/********************************************************************
 * 函数： parentsControlChange()
 * 功能： 配置变化时,生效函数
 * 创建： 2013-05-10
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/
static void parentsControlChange(ParentsControlProfile *prof, char action)
{
#if 1
#if (TIME_GRP == FYES)
    TimeGrpProfile *timeGrpProf = NULL;
#endif
    char_t macStr[18];
    int j = 0;

    if(isParentsControlEnable() == 1U) {
	memset(macStr, 0U, sizeof(macStr));
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
	{
	    strncpy(macStr, converMac6To17Byte(prof->mac), 17U);
	    DBGPRINT_PARENTS("%s-%d: macStr(17Byte) %s\n",__func__, __LINE__,macStr);
	    if((memcmp(macStr, MAC_NULL_STR, 17U) != 0)) /*check mac is null or not*/
	    //if((prof->mac.addr[0] != '\0')) /*need modify check mac is null or not*/
	    {
#if (TIME_GRP == FYES)
		if(strlen(prof->timeGrp) != 0U)
		{
		    timeGrpProf = (TimeGrpProfile *)ProfGetInstPointByName(MIB_PROF_TIMEGRP, prof->timeGrp);
		} else {
		    timeGrpProf = NULL;
		}
		parentsControlforTime((TimeGrpProfile *)NULL, macStr, action);
#else
		parentsControlforTime(macStr, action);
#endif
		for(j=0;j<MAX_ALLOW_URL_ITEM;j++){
		    if(prof->allowURL[j].url[0] != '\0'){
#if 0
		    doSystem("iptables -%c %s -m mac --mac-source %s -m domain --name \"%s\" -j ACCEPT >/dev/null 2>&1",action,PARENTS_FILTER_INPUT_CHAIN,macStr,prof->allowURL[j].url); /*maybe need change*/
		    doSystem("iptables -%c %s -m mac --mac-source %s -m domain --name \"%s\" -j ACCEPT >/dev/null 2>&1",action,PARENTS_FILTER_CHAIN,macStr,prof->allowURL[j].url); /*maybe need change*/
#endif
#if (UTT_SMART_UI == FYES)
		    parentsControlUrlforTimeSui(prof, macStr, prof->allowURL[j].url, action);
#else
#if (TIME_GRP == FYES)
		    parentsControlUrlforTime(timeGrpProf,macStr,prof->allowURL[j].url,action);
#else
		    parentsControlUrlforTime(macStr,prof->allowURL[j].url,action);
#endif
#endif
		    }
		}
	    }
	}
    }
#endif
    return;
}
/********************************************************************
 * 函数： parentsControlDelAll(void)
 * 功能： 删除所有家长控制规则
 * 创建： 2013-05-10
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/
static void parentsControlDelAll(void)
{
#if 0
    doSystem("iptables -F %s ", PARENTS_FILTER_CHAIN);
    doSystem("iptables -F %s ",PRE_PARENTS_FILTER_CHAIN);
#endif
    return;
}
/********************************************************************
 * 函数： parentscontrolProfileChange()
 * 功能： 家长控制配置变化 生效函数
 * 创建： 2013-05-10
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/
static void parentscontrolProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const ParentsControlProfile *oldProfileDataPtr, const ParentsControlProfile *newProfileDataPtr )
{
    ParentsControlProfile *newProf = NULL, *oldProf = NULL;
    newProf = (ParentsControlProfile *)newProfileDataPtr;
    oldProf = (ParentsControlProfile *)oldProfileDataPtr;
    DBGPRINT_PARENTS("%s-%d: new prof username =%s, active %d\n",__func__,__LINE__,newProf->head.name,newProf->head.active);
    DBGPRINT_PARENTS("%s-%d: old prof username =%s, active %d\n",__func__,__LINE__,oldProf->head.name,oldProf->head.active);

    switch(changeType) {
        case PROF_CHANGE_ADD:
	    DBGPRINT_PARENTS("%s-%d: \n",__func__,__LINE__);
	    parentsControlChange(newProf, Iptables_INS);
            break;
        case PROF_CHANGE_EDIT:
	    DBGPRINT_PARENTS("%s-%d: \n",__func__,__LINE__);
	    parentsControlChange(oldProf, Iptables_DEL);
	    parentsControlChange(newProf, Iptables_INS);
            break;
        case PROF_CHANGE_DEL:
	    DBGPRINT_PARENTS("%s-%d: \n",__func__,__LINE__);
	    parentsControlChange(oldProf, Iptables_DEL);
            break;
        case PROF_CHANGE_DELALL:
	    DBGPRINT_PARENTS("%s-%d: \n",__func__,__LINE__);
	    parentscontrolInit();/*初始化*/
	    break;
        default:/*default*/
            break;
    }
    return;
}
/********************************************************************
 * 函数： parentscontrolglobalProfileChange()
 * 功能： 家长控制全局开关变化 生效函数
 * 创建： 2013-05-10
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/
static void parentscontrolglobalProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
	    break;
        case PROF_CHANGE_EDIT:
	    DBGPRINT_PARENTS("%s-%d: old global enable %d, new global enable %d\n",__func__,__LINE__,
		    oldProfileDataPtr->parentControlEn.parentsEn,newProfileDataPtr->parentControlEn.parentsEn);
	    if((oldProfileDataPtr->parentControlEn.parentsEn == FUN_ENABLE) && (newProfileDataPtr->parentControlEn.parentsEn == FUN_DISABLE))
	    {
		/*关闭变为开启*/
		DBGPRINT_PARENTS("%s-%d: \n",__func__,__LINE__);
		doSystem("iptables -F %s ",PARENTS_FILTER_CHAIN);
		doSystem("iptables -F %s ",PRE_PARENTS_FILTER_CHAIN);
		doSystem("iptables -D FORWARD -m uttdev --lan-to-wan -j %s ",PRE_PARENTS_FILTER_CHAIN);
		doSystem("iptables -X %s ",PARENTS_FILTER_CHAIN);
		doSystem("iptables -X %s ",PRE_PARENTS_FILTER_CHAIN);
		doSystem("iptables -F %s ",PARENTS_FILTER_INPUT_CHAIN);
		doSystem("iptables -F %s ",PRE_PARENTS_FILTER_INPUT_CHAIN);
		doSystem("iptables -D INPUT -j %s ",PRE_PARENTS_FILTER_INPUT_CHAIN);
		doSystem("iptables -X %s ",PARENTS_FILTER_INPUT_CHAIN);
		doSystem("iptables -X %s ",PRE_PARENTS_FILTER_INPUT_CHAIN);
	    }
	    else if((oldProfileDataPtr->parentControlEn.parentsEn == FUN_DISABLE) && (newProfileDataPtr->parentControlEn.parentsEn == FUN_ENABLE))
	    {
		/*开启变为关闭*/
		DBGPRINT_PARENTS("%s-%d: \n",__func__,__LINE__);
		parentscontrolInit();
	    }
	    else
	    {
		/*家长PC的MAC地址是否修改*/
		DBGPRINT_PARENTS("%s-%d: \n",__func__,__LINE__);
		parentscontrolInit();
	    }
	    break;
        case PROF_CHANGE_DEL:
	    break;
        case PROF_CHANGE_DELALL:
	    break;
        default:/*default*/
            break;
    }
    return;
}

/********************************************************************
 * 函数： parentsControlforTime();
 * 功能： 基于时间段的家长控制规则生效函数
 * 创建： 2013-05-10
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/
#if (TIME_GRP == FYES)
static void parentsControlforTime(TimeGrpProfile *timeGrpProf, char *macStr, char action)
#else
static void parentsControlforTime(char *macStr, char action)
#endif
{
#if (TIME_GRP == FYES)

    int i = 0;
    char *daystr = NULL;
    char *timestr = NULL;
    char *datestr = NULL;
    unsigned int isdateForever = 1U;
    unsigned int istimeRange = 0U;

    if((timeGrpProf != NULL)) {
	if(strcmp(timeGrpProf->datestart,"Forever")!=0U)
	{
	    datestr = getDateForIptables(timeGrpProf->datestart, timeGrpProf->datestop);/*iptables 日期字段*/
	    isdateForever = 0U;
	}


	if(isdateForever == 0U)
	{

	    for(i=0;i < TIMEGRP_ENTRYS; i++)
	    {
		/*有一个时间段符合的则 跳转到过滤链表*/
		daystr = getDayForIptables(timeGrpProf->timeEntry[i].day);
		timestr = getTimeForIptables(timeGrpProf->timeEntry[i].timestart,timeGrpProf->timeEntry[i].timestop);
		if(timeGrpProf->timeEntry[i].enable == FUN_ENABLE)
		{
		    doSystem("iptables -%c %s -m uttdev --is-lan-in -m mac --mac-source %s %s %s %s -p udp --dport 53 -j %s ",action,PRE_PARENTS_FILTER_INPUT_CHAIN,macStr,timestr,daystr,datestr,PARENTS_FILTER_INPUT_CHAIN);
		    DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m uttdev --is-lan-in -m mac --mac-source %s %s %s %s -p udp --dport 53 -j %s \n",__func__,__LINE__,action,PRE_PARENTS_FILTER_INPUT_CHAIN,macStr,timestr,daystr,datestr,PARENTS_FILTER_INPUT_CHAIN);
		    doSystem("iptables -%c %s -m mac --mac-source %s -p udp --dport 53 %s %s %s -j %s ",action,PRE_PARENTS_FILTER_CHAIN,macStr,timestr,daystr,datestr,PARENTS_FILTER_CHAIN);
		    DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m mac --mac-source %s -p udp --dport 53 %s %s %s -j %s \n",__func__,__LINE__,action,PRE_PARENTS_FILTER_CHAIN,macStr,timestr,daystr,datestr,PARENTS_FILTER_CHAIN);
		    istimeRange = 1U;
		}
	    }
	    if(istimeRange == 0U) {
		/*开始到结束日期的数据包包才匹配*/
		doSystem("iptables -%c %s -m uttdev --is-lan-in -m mac --mac-source %s -m time %s -p udp --dport 53 -j %s ",action,PRE_PARENTS_FILTER_INPUT_CHAIN,macStr,datestr,PARENTS_FILTER_INPUT_CHAIN);
		DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m uttdev --is-lan-in -m mac --mac-source %s -m time %s -p udp --dport 53 -j %s \n",__func__,__LINE__,action,PRE_PARENTS_FILTER_INPUT_CHAIN,macStr,datestr,PARENTS_FILTER_INPUT_CHAIN);
		doSystem("iptables -%c %s -m mac --mac-source %s -p udp --dport 53 -m time %s -j %s ",action,PRE_PARENTS_FILTER_CHAIN,macStr,datestr,PARENTS_FILTER_CHAIN);
		DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m mac --mac-source %s -p udp --dport 53 -m time %s -j %s \n",__func__,__LINE__,action,PRE_PARENTS_FILTER_CHAIN,macStr,datestr,PARENTS_FILTER_CHAIN);
	    }
	}
	else
	{
	    /*时间组名为空代表所有时段*/
	    doSystem("iptables -%c %s -m uttdev --is-lan-in -m mac --mac-source %s -p udp --dport 53 -j %s ", action, PRE_PARENTS_FILTER_INPUT_CHAIN,macStr,PARENTS_FILTER_INPUT_CHAIN);
	    DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m uttdev --dport 53 -m mac --mac-source %s -p udp --dport 53 -j %s \n", __func__,__LINE__,action, PRE_PARENTS_FILTER_INPUT_CHAIN,macStr,PARENTS_FILTER_INPUT_CHAIN);
	    doSystem("iptables -%c %s -m mac --mac-source %s -p udp --dport 53 -j %s ", action, PRE_PARENTS_FILTER_CHAIN,macStr,PARENTS_FILTER_CHAIN);
	    DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m mac --mac-source %s -p udp --dport 53 -j %s \n", __func__,__LINE__,action, PRE_PARENTS_FILTER_CHAIN,macStr,PARENTS_FILTER_CHAIN);
	}
    }
    else
    {
	/*时间组名为空代表所有时段*/
	doSystem("iptables -%c %s -m uttdev --is-lan-in -m mac --mac-source %s -p udp --dport 53 -j %s ", action, PRE_PARENTS_FILTER_INPUT_CHAIN, macStr, PARENTS_FILTER_INPUT_CHAIN);
	DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m uttdev --is-lan-in -m mac --mac-source %s -p udp --dport 53 -j %s \n", __func__,__LINE__,action, PRE_PARENTS_FILTER_INPUT_CHAIN, macStr, PARENTS_FILTER_INPUT_CHAIN);
	doSystem("iptables -%c %s -m mac --mac-source %s -p udp --dport 53 -j %s ", action, PRE_PARENTS_FILTER_CHAIN, macStr, PARENTS_FILTER_CHAIN);
	DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m mac --mac-source %s -p udp --dport 53 -j %s \n", __func__,__LINE__,action, PRE_PARENTS_FILTER_CHAIN, macStr, PARENTS_FILTER_CHAIN);
    }
#else
	/*无时间组，代表所有时段*/
	doSystem("iptables -%c %s -m uttdev --is-lan-in -m mac --mac-source %s -p udp --dport 53 -j %s ", action, PRE_PARENTS_FILTER_INPUT_CHAIN, macStr PARENTS_FILTER_INPUT_CHAIN);
	DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m uttdev --dport 53 -m mac --mac-source %s -p udp --dport 53 -j %s \n", __func__,__LINE__,action, PRE_PARENTS_FILTER_INPUT_CHAIN, macStr PARENTS_FILTER_INPUT_CHAIN);
	doSystem("iptables -%c %s -m mac --mac-source %s -p udp --dport 53 -j %s ", action, PRE_PARENTS_FILTER_CHAIN, macStr PARENTS_FILTER_CHAIN);
	DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m mac --mac-source %s -p udp --dport 53 -j %s \n", __func__,__LINE__,action, PRE_PARENTS_FILTER_CHAIN, macStr PARENTS_FILTER_CHAIN);
#endif
}

/********************************************************************
 * 函数： parentsControlforTime();
 * 功能： 基于时间段的家长控制规则生效函数
 * 创建： 2013-05-10
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/
#if (TIME_GRP == FYES)
static void parentsControlUrlforTime(TimeGrpProfile *timeGrpProf, char *macStr, char *url, char action)
#else
static void parentsControlUrlforTime(char *macStr, char *url char action)
#endif
{
#if (TIME_GRP == FYES)

    int i = 0;
    char *daystr = NULL;
    char *timestr = NULL;
    char *datestr = NULL;
    unsigned int isdateForever = 1U;
    unsigned int istimeRange = 0U;

    if((timeGrpProf != NULL)) {
	if(strcmp(timeGrpProf->datestart,"Forever")!=0U)
	{
	    datestr = getDateForIptables(timeGrpProf->datestart, timeGrpProf->datestop);/*iptables 日期字段*/
	    isdateForever = 0U;
	}


	if(isdateForever == 0U)
	{

	    for(i=0;i < TIMEGRP_ENTRYS; i++)
	    {
		/*有一个时间段符合的则 跳转到过滤链表*/
		daystr = getDayForIptables(timeGrpProf->timeEntry[i].day);
		timestr = getTimeForIptables(timeGrpProf->timeEntry[i].timestart,timeGrpProf->timeEntry[i].timestop);
		if(timeGrpProf->timeEntry[i].enable == FUN_ENABLE)
		{
		    doSystem("iptables -%c %s -m mac --mac-source %s %s %s %s -m domain --name \"%s\" -j ACCEPT >/dev/null 2>&1",action,PARENTS_FILTER_INPUT_CHAIN,macStr,timestr,daystr,datestr,url); /*maybe need change*/
		    DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m mac --mac-source %s %s %s %s -m domain --name \"%s\" -j ACCEPT\n",__func__,__LINE__,action,PARENTS_FILTER_INPUT_CHAIN,macStr,timestr,daystr,datestr,url); /*maybe need change*/
		    doSystem("iptables -%c %s -m mac --mac-source %s %s %s %s -m domain --name \"%s\" -j ACCEPT >/dev/null 2>&1",action,PARENTS_FILTER_CHAIN,macStr,timestr,daystr,datestr,url); /*maybe need change*/
		    DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m mac --mac-source %s %s %s %s -m domain --name \"%s\" -j ACCEPT\n",__func__,__LINE__,action,PARENTS_FILTER_CHAIN,macStr,timestr,daystr,datestr,url); /*maybe need change*/
		    istimeRange = 1U;
		}
	    }
	    if(istimeRange == 0U) {
		/*开始到结束日期的数据包包才匹配*/
		doSystem("iptables -%c %s -m mac --mac-source %s -m time %s -m domain --name \"%s\" -j ACCEPT >/dev/null 2>&1",action,PARENTS_FILTER_INPUT_CHAIN,macStr,datestr,url); /*maybe need change*/
		DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m mac --mac-source %s -m time %s -m domain --name \"%s\" -j ACCEPT\n",__func__,__LINE__,action,PARENTS_FILTER_INPUT_CHAIN,macStr,datestr,url); /*maybe need change*/
		doSystem("iptables -%c %s -m mac --mac-source %s -m time %s -m domain --name \"%s\" -j ACCEPT >/dev/null 2>&1",action,PARENTS_FILTER_CHAIN,macStr,datestr,url); /*maybe need change*/
		DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m mac --mac-source %s -m time %s -m domain --name \"%s\" -j ACCEPT\n",__func__,__LINE__,action,PARENTS_FILTER_CHAIN,macStr,datestr,url); /*maybe need change*/
	    }
	}
	else
	{
	    /*时间组名为空代表所有时段*/
	    doSystem("iptables -%c %s -m mac --mac-source %s -m domain --name \"%s\" -j ACCEPT >/dev/null 2>&1",action,PARENTS_FILTER_INPUT_CHAIN,macStr,url); /*maybe need change*/
	    DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m mac --mac-source %s -m domain --name \"%s\" -j ACCEPT\n",__func__,__LINE__,action,PARENTS_FILTER_INPUT_CHAIN,macStr,url); /*maybe need change*/
	    doSystem("iptables -%c %s -m mac --mac-source %s -m domain --name \"%s\" -j ACCEPT >/dev/null 2>&1",action,PARENTS_FILTER_CHAIN,macStr,url); /*maybe need change*/
	    DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m mac --mac-source %s -m domain --name \"%s\" -j ACCEPT\n",__func__,__LINE__,action,PARENTS_FILTER_CHAIN,macStr,url); /*maybe need change*/
	}
    }
    else
    {
	/*时间组名为空代表所有时段*/
	doSystem("iptables -%c %s -m mac --mac-source %s -m domain --name \"%s\" -j ACCEPT >/dev/null 2>&1",action,PARENTS_FILTER_INPUT_CHAIN,macStr,url); /*maybe need change*/
	DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m mac --mac-source %s -m domain --name \"%s\" -j ACCEPT\n",__func__,__LINE__,action,PARENTS_FILTER_INPUT_CHAIN,macStr,url); /*maybe need change*/
	doSystem("iptables -%c %s -m mac --mac-source %s -m domain --name \"%s\" -j ACCEPT >/dev/null 2>&1",action,PARENTS_FILTER_CHAIN,macStr,url); /*maybe need change*/
	DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m mac --mac-source %s -m domain --name \"%s\" -j ACCEPT\n",__func__,__LINE__,action,PARENTS_FILTER_CHAIN,macStr,url); /*maybe need change*/
    }
#else
	/*无时间组，代表所有时段*/
	doSystem("iptables -%c %s -m mac --mac-source %s -m domain --name \"%s\" -j ACCEPT >/dev/null 2>&1",action,PARENTS_FILTER_INPUT_CHAIN,macStr,url); /*maybe need change*/
	DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m mac --mac-source %s -m domain --name \"%s\" -j ACCEPT\n",__func__,__LINE__,action,PARENTS_FILTER_INPUT_CHAIN,macStr,url); /*maybe need change*/
	doSystem("iptables -%c %s -m mac --mac-source %s -m domain --name \"%s\" -j ACCEPT >/dev/null 2>&1",action,PARENTS_FILTER_CHAIN,macStr,url); /*maybe need change*/
	DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m mac --mac-source %s -m domain --name \"%s\" -j ACCEPT\n",__func__,__LINE__,action,PARENTS_FILTER_CHAIN,macStr,url); /*maybe need change*/
#endif
}
static void parentRuleBuild(char action,MacAddr macaddr)
{
    char_t macStr[18];
    memset(macStr, 0U, sizeof(macStr));
    strncpy(macStr, converMac6To17Byte(macaddr), 17U);
    DBGPRINT_PARENTS("%s-%d: macStr(17Byte) %s\n",__func__, __LINE__,macStr);
    //if((profSys->parentControlEn.parentMac1.addr[0] != '\0')) /*check mac is null or not*/
    if((memcmp(macStr, MAC_NULL_STR, 17U) != 0)) /*check mac is null or not*/
    {
	doSystem("iptables -%c %s -m mac --mac-source %s -j RETURN ",action,PRE_PARENTS_FILTER_CHAIN,macStr);
	DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m mac --mac-source %s -j RETURN \n",__func__,__LINE__,action,PRE_PARENTS_FILTER_CHAIN,macStr);
	doSystem("iptables -%c %s -m uttdev --is-lan-in -m mac --mac-source %s -j RETURN ",action,PRE_PARENTS_FILTER_INPUT_CHAIN,macStr);
    }

}
/********************************************************************
 * 函数： parentscontrolInit(void)
 * 功能： 家长控制配置初始化函数
 * 创建： 2013-05-10
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/
static void parentscontrolInit(void)
{
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_PARENTS_CONTROL;
    ParentsControlProfile *prof = NULL;
    int min = 0, max = 0, index = 0, j = 0;
    char_t macStr[18];
    char action = 'A';
#if (TIME_GRP == FYES)
    TimeGrpProfile *timeGrpProf = NULL;
#endif
    
    if(isParentsControlEnable() == 1U) {
#if 1
	doSystem("iptables -N %s ",PARENTS_FILTER_CHAIN);
	doSystem("iptables -N %s ",PRE_PARENTS_FILTER_CHAIN);
	doSystem("iptables -F %s ",PARENTS_FILTER_CHAIN);
	doSystem("iptables -F %s ",PRE_PARENTS_FILTER_CHAIN);
	doSystem("iptables -D FORWARD -m uttdev --lan-to-wan -j %s ",PRE_PARENTS_FILTER_CHAIN);
	doSystem("iptables -A FORWARD -m uttdev --lan-to-wan -j %s ",PRE_PARENTS_FILTER_CHAIN);
	doSystem("iptables -N %s ",PARENTS_FILTER_INPUT_CHAIN);
	doSystem("iptables -N %s ",PRE_PARENTS_FILTER_INPUT_CHAIN);
	doSystem("iptables -F %s ",PARENTS_FILTER_INPUT_CHAIN);
	doSystem("iptables -F %s ",PRE_PARENTS_FILTER_INPUT_CHAIN);
	doSystem("iptables -D INPUT -j %s ",PRE_PARENTS_FILTER_INPUT_CHAIN);
	doSystem("iptables -A INPUT -j %s ",PRE_PARENTS_FILTER_INPUT_CHAIN);
	/*家长 MAC*/
	profSys = (SystemProfile *)ProfGetInstPointByIndex(mibTypeSys, 0);
	if((profSys != NULL) && (ProfInstIsFree(profSys) == 0))
	{
	    parentRuleBuild('A',profSys->parentControlEn.parentMac1);
	    parentRuleBuild('A',profSys->parentControlEn.parentMac2);
	    parentRuleBuild('A',profSys->parentControlEn.parentMac3);
	    parentRuleBuild('A',profSys->parentControlEn.parentMac4);
	    parentRuleBuild('A',profSys->parentControlEn.parentMac5);
	    parentRuleBuild('A',profSys->parentControlEn.parentMac6);
	    parentRuleBuild('A',profSys->parentControlEn.parentMac7);
	    parentRuleBuild('A',profSys->parentControlEn.parentMac8);
	    parentRuleBuild('A',profSys->parentControlEn.parentMac9);
	}
#endif
	/*dns包需要允许通过，否则无法访问网站*/

	ProfInstLowHigh(mibType, &max, &min);
	for (index = min; index < max; index++) 
	{
	    memset(macStr, 0U, sizeof(macStr));
	    prof = (ParentsControlProfile*)ProfGetInstPointByIndex(mibType, index); /*get prof pointer*/
	    if((prof != NULL) && (ProfInstIsFree(prof) == 0) 
		    && (prof->head.active == TRUE))
	    {
		strncpy(macStr, converMac6To17Byte(prof->mac), 17U);
		DBGPRINT_PARENTS("%s-%d: macStr(17Byte) %s\n",__func__, __LINE__,macStr);
		//if((prof->mac.addr[0] != '\0')) /*need modify check mac is null or not*/
		if((memcmp(macStr, MAC_NULL_STR, 17U) != 0)) /*check mac is null or not*/
		{
#if (TIME_GRP == FYES)
		if(strlen(prof->timeGrp) != 0U)
		{
		    timeGrpProf = (TimeGrpProfile *)ProfGetInstPointByName(MIB_PROF_TIMEGRP, prof->timeGrp);
		} else {
		    timeGrpProf = NULL;
		}
		parentsControlforTime((TimeGrpProfile *)NULL, macStr, action);
#else
		parentsControlforTime(macStr, action);
#endif
		for(j=0;j<MAX_ALLOW_URL_ITEM;j++){
		    if(prof->allowURL[j].url[0] != '\0'){
#if 0
			doSystem("iptables -%c %s -m mac --mac-source %s -m domain --name \"%s\" -j ACCEPT >/dev/null 2>&1",action,PARENTS_FILTER_INPUT_CHAIN,macStr,prof->allowURL[j].url); /*maybe need change*/
			DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m mac --mac-source %s -m domain --name \"%s\" -j ACCEPT\n",__func__,__LINE__,action,PARENTS_FILTER_INPUT_CHAIN,macStr,prof->allowURL[j].url); /*maybe need change*/
			doSystem("iptables -%c %s -m mac --mac-source %s -m domain --name \"%s\" -j ACCEPT >/dev/null 2>&1",action,PARENTS_FILTER_CHAIN,macStr,prof->allowURL[j].url); /*maybe need change*/
			DBGPRINT_PARENTS("%s-%d: iptables -%c %s -m mac --mac-source %s -m domain --name \"%s\" -j ACCEPT\n",__func__,__LINE__,action,PARENTS_FILTER_CHAIN,macStr,prof->allowURL[j].url); /*maybe need change*/
#endif
#if (UTT_SMART_UI == FYES)
			parentsControlUrlforTimeSui(prof, macStr, prof->allowURL[j].url, action);
#else
#if (TIME_GRP == FYES)
			parentsControlUrlforTime(timeGrpProf,macStr,prof->allowURL[j].url,action);
#else
			parentsControlUrlforTime(macStr,prof->allowURL[j].url,action);
#endif
#endif
		    }
		}
		}
	    }
	}
#ifdef CONFIG_NETFILTER_XT_MATCH_HTTPQQ
	doSystem("iptables -%c %s -p tcp --dport 80 -m httpQQ ! --is-match -j ACCEPT ",action,PRE_PARENTS_FILTER_CHAIN);
#else
	doSystem("iptables -%c %s -p tcp --dport 80 -j ACCEPT ",action,PRE_PARENTS_FILTER_CHAIN);
#endif
	doSystem("iptables -%c %s -p tcp --dport 443 -j ACCEPT ",action,PRE_PARENTS_FILTER_CHAIN);
	doSystem("iptables -A %s -j DROP ",PRE_PARENTS_FILTER_CHAIN);
	doSystem("iptables -%c %s -j REJECT --reject-with dns-redirect:127.0.0.1",action,PARENTS_FILTER_CHAIN);
	doSystem("iptables -%c %s -m uttdev --is-lan-in -p udp --dport 53 -j DROP",action,PRE_PARENTS_FILTER_INPUT_CHAIN);
	doSystem("iptables -%c %s -j REJECT --reject-with dns-redirect:127.0.0.1 ",action,PARENTS_FILTER_INPUT_CHAIN);
#if 0
	doSystem("iptables -%c %s -p tcp --dport 80 -j ACCEPT ",action,PARENTS_FILTER_INPUT_CHAIN);
	doSystem("iptables -A %s -j DROP ",PRE_PARENTS_FILTER_INPUT_CHAIN);
#endif
    }
    return;
}

#if (UTT_SMART_UI == FYES)
/*原来的prof没有时间段，再此设置初始值*/
void changeProfToSui()
{
    MibProfileType mibType = MIB_PROF_PARENTS_CONTROL;
    ParentsControlProfile *prof = NULL;
    int min = 0, max = 0, index = 0;
    
    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++) 
    {
	prof = (ParentsControlProfile*)ProfGetInstPointByIndex(mibType, index); /*get prof pointer*/
	if (ProfInstIsFree(prof) == PROFOK)
	{
	    if (!strcmp(prof->timeStart, ""))
	    {
		prof->day = 127U;
		strcpy(prof->timeStart, "All");
	    }
	    if (!strcmp(prof->timeStop, ""))
	    {
		strcpy(prof->timeStop, "0");
	    }
	}
    }
}
#endif

/********************************************************************
 * 函数： funInitParentsControl(void)
 * 功能： 家长控制功能 初始化入口函数
 * 创建： 2013-05-10
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/
void funInitParentsControl(void)
{

#if 1
    registerForProfileUpdates(MIB_PROF_SYSTEM, (Pointer)NULL, (ProfileUpdateFnc)parentscontrolglobalProfileChange);/*触发函数*/
    registerForProfileUpdates(MIB_PROF_PARENTS_CONTROL, (Pointer)NULL, (ProfileUpdateFnc)parentscontrolProfileChange);/*触发函数*/
#endif
#if (UTT_SMART_UI == FYES)
    changeProfToSui();/*原来的prof没有时间段，再此设置初始值*/
#endif

    DBGPRINT_PARENTS("%s-%d: \n",__func__, __LINE__);
    parentscontrolInit();/*初始化*/
    return;
}
#endif
