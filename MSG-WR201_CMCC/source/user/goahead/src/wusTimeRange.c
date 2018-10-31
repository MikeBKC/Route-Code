/*
 * 文件名:
 *     wusTimeRange.c
 *
 * 描述:
 *    时间组交互文件
 *    
 * 作者：
 * 维护人：
 * 变更：
 *     1. 初始版本:2012-05-15
 *
 */
#include <stdio.h>
#include <string.h>
#include <mib.h>
#include "typedef.h"
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "uttfunction.h"
#include "profacce.h"
#include "uttMachine.h"
#include "comExeFun.h"
#include "ipsetApi.h"
#include "translate.h"
#if (TIME_GRP == FYES)
#define TR_CHECKED  "on"
#ifdef  TIME_GRP_DEBUG
#define TIME_GRP_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define TIME_GRP_PRINTF(fmt, args...)
#endif

#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
#define ONEDAYSEC   (24*60*60)
#define MAXDATA     24854
static  UINT32 transDate(char *dateTime)
{
    if(dateTime==NULL){
	return 0;
    }
    char *Date=dateTime;
    int i=0;
    int num=0;
    int time[3];
    time_t t;
    struct tm stm;
    const char *errMsg = NULL;

    while(*Date!='\0'){
	while(*Date!='-'&&*Date!='\0')
	{
	    num=num*10+(*Date-'0');
	    Date++;
	}
	time[i++]=num;
	if(*Date=='-') Date++;
	num=0;
    }
    stm.tm_year=time[0]-1900;
    stm.tm_mon=time[1]-1;
    stm.tm_mday=time[2];
    stm.tm_hour=0;
    stm.tm_min=0;
    stm.tm_sec=0;
    t=mktime(&stm); 
    if(t!=-1){
	t=(t/ONEDAYSEC);
    }
    else 
    {
	errMsg = getTransDataForC(C_LANG_INDEX_TIMESET_TOO_LARGE);
	setTheErrorMsgOnFree(errMsg);
	t=MAXDATA;
    }

    return t;
}
static  void FormatTime(UINT32 time, char *_time)
{
    int i;
    time_t t=time*ONEDAYSEC;
    struct tm *tm_ptr;
    tm_ptr=localtime(&t);
    i=0;
    if(tm_ptr!=NULL){
	sprintf(_time,"%04d",(tm_ptr->tm_year+1900));
	_time[4]='-';
	sprintf(_time+5,"%02d",(tm_ptr->tm_mon+1));
	_time[7]='-';
	sprintf(_time+8,"%02d",tm_ptr->tm_mday);
	_time[10]='\0';
	return ;
    }
}

static  int  ChangeTimeToSec(char *dateTime)
{
    int time_sec=0;
    if(dateTime==NULL){
	return -1;
    }
    time_sec=(((dateTime[0]-'0')*10+(dateTime[1]-'0'))*60+((dateTime[3]-'0')*10+(dateTime[4]-'0')))*60;
    if(time_sec==0)
    {
	time_sec=59;
    }
    return time_sec;

}
static void formatTime(int time_sec,char *time)
{
    char time_c[6];
    int i=0;
    if(time_sec==59)
    {
    time[0]='0';
    time[1]='0';
    time[2]=':';
    time[3]='0';
    time[4]='0';
    time[5]='\0';
    }
    if(time_sec>=60){
    time[i++]=(time_sec/3600)/10+'0';
    time[i++]=(time_sec/3600)%10+'0';
    time[i++]=':';
    time[i++]=((time_sec%3600)/60)/10+'0';
    time[i++]=((time_sec%3600)/60)%10+'0';
    time[i]='\0';
    }
    return ;
}
#endif

#if (PARENTS_CONTROL == FYES)
extern int isTimeGrpReredByParentsControl(char *timeGrpName);
#endif
/********************************************************************
* 函数： getIpGroupListData
* 功能： 
* 创建： 2012-05-15
* 参数： 
* 返回： 略
* 输出： 1	被引用
*	 0	违背引用
* 作者：  
 ********************************************************************/
static int isReredByRule(TimeGrpProfile *prof)
{
    int flag=0;
    MibProfileType mibType=MIB_PROF_SYSTEM;
    SystemProfile *sys=NULL;	/*get systemprofile*/
    sys=(SystemProfile *)ProfGetInstPointByIndex(mibType,0);
    /* judge ipgroup Rered or not */
    if(sys && strcmp(sys->dnsFilterControl.timeGrpName, prof->head.name)==0)
    {
	flag = 1;
    }
#if (PARENTS_CONTROL == FYES)
    else if(isTimeGrpReredByParentsControl(prof->head.name) == 1)
    {
	flag = 1;
    }
#endif
    return flag;
}
/********************************************************************
* 函数： getTimeGroupListData
* 功能： 
* 创建： 2012-05-15
* 参数： 
* 返回： 略
* 输出： 略
* 作者：  
 ********************************************************************/
static int getTimeGroupListData(int eid, webs_t wp, int argc, char **argv)
{
#if 1
    MibProfileType mibType = MIB_PROF_TIMEGRP;
    TimeGrpProfile *prof = NULL;
    int min = 0, max = 0;
    int i = 0;
    int num = 0;
#if CONFIG_PERFORMANCE_OPTIMIZATION
    char the_time[11];
    char startall[8]="Forever";
    char zero[2]="0";
#endif
    /*define web varliable*/
    websWrite(wp,"var timeRangeNames=new Array();");
    websWrite(wp,"var group_dateStart=new Array();");
    websWrite(wp,"var group_dateStop=new Array();");
    websWrite(wp, T("\n"));
    /*get inst maxnum and minnum*/
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) 
    {
	/*get TImeGroup inst*/
	prof = (TimeGrpProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
	{
	    /*write TimeGroProfile inst infor*/
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    websWrite(wp,"timeRangeNames[%d] = \"%s\";", num, prof->head.name);
	    if(prof->datestart==1)
	    {
		websWrite(wp, "group_dateStart[%d]=\"Forever\";", num);
	    }
	    
	    else{
		FormatTime(prof->datestart,the_time);
		websWrite(wp, "group_dateStart[%d]=\"%s\";", num,the_time);
	    }
	    if(prof->datestop==2)
	    {
		websWrite(wp,"group_dateStop[%d]=\"0\";", num);
	    }
	    else{
		FormatTime(prof->datestop,the_time);
		websWrite(wp,"group_dateStop[%d]=\"%s\";", num,the_time);
	    }
#else
	    websWrite(wp,"timeRangeNames[%d] = \"%s\";", num, prof->head.name);
	    websWrite(wp, "group_dateStart[%d]=\"%s\";", num, prof->datestart);
	    websWrite(wp,"group_dateStop[%d]=\"%s\";", num, prof->datestop);
	    TIME_GRP_PRINTF("%s-%d:timegroupName=%s,datestart=%s,datestop=%s\n",__FUNCTION__,__LINE__,prof->head.name,prof->datestart,prof->datestop);
#endif
	    num++;
	}
    }
    TIME_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
    /*get system error msg*/
    getTheErrorMsg(eid, wp, argc, argv);
    /*write inst num and max inst num*/
    websWrite(wp,"var totalrecs=%d;",num);
    websWrite(wp,"var max_totalrecs =%d;",max);
#endif
    return 0;
}
/*******************************************************************
* 函数： getTimeGroupOneInfo
* 功能： 
* 创建： 2012-05-15
* 参数： 
* 返回： 略
* 输出： 略
* 作者：  
 ********************************************************************/
static int getTimeGroupOneInfo(int eid, webs_t wp, int argc, char **argv)
{
#if 1
    MibProfileType mibType = MIB_PROF_TIMEGRP;
    TimeGrpProfile *prof = NULL;
    UINT32 d_temp=0U;
    char_t data[8]={0};
    int min=0, max=0;
    int i=0, j=0;
    const char *errMsg=NULL;

    char *editName = NULL;
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    char the_time[11];
    char startall[8]="Forever";
    char zero[2]="0"; 
#endif
    /*define web varliable*/
    websWrite(wp, "var timeRangeName=\"\";");
    websWrite(wp, "var dateStart=\"\";");
    websWrite(wp, "var dateStop=\"\";");

    websWrite(wp, "var enable=new Array();");
    websWrite(wp, "var day=new Array();");
    websWrite(wp, "var timeStart=new Array();");
    websWrite(wp, "var timeStop=new Array();");
    /*get inst name*/
    if ((ejArgs(argc, argv, T("%s"), &editName) < 1) || (editName == NULL))/*查看实例名是否存在*/ 
    {
	errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_READ_DATA_ERR);
	setTheErrorMsgOnFree(errMsg);
    }
    else
    {
	TIME_GRP_PRINTF("%s-%d:editName=%s\n",__FUNCTION__,__LINE__,editName);
	ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++) 
	{
	    prof = (TimeGrpProfile*)ProfGetInstPointByIndex(mibType, i);
	    if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE) && (strcmp(editName, prof->head.name) == 0))
	    {
		/*find inst
		 * write inst information
		 */
		websWrite(wp, "timeRangeName=\"%s\";", prof->head.name);
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
		if(prof->datestart==1){
		    websWrite(wp, "dateStart=\"Forever\";");
		}
		else{
		    FormatTime(prof->datestart,the_time);
		    websWrite(wp, "dateStart=\"%s\";", the_time);
		}
		if(prof->datestop==2)
		{
		    websWrite(wp, "timeStop[%d]=\"0\";",j);
		}
		else{
		    FormatTime(prof->datestop,the_time);
		    websWrite(wp, "dateStop=\"%s\";", the_time);
		}
#else
		websWrite(wp, "dateStart=\"%s\";", prof->datestart);
		websWrite(wp, "dateStop=\"%s\";", prof->datestop);
		TIME_GRP_PRINTF("%s-%d:timeRangeName=%s, datestart=%s, datestop=%s\n",__FUNCTION__,__LINE__,prof->head.name,prof->datestart,prof->datestop);
#endif
		for(j=0;j<TIMEGRP_ENTRYS;j++)
		{
		    if(prof->timeEntry[j].enable==FUN_ENABLE)
		    {
			d_temp=0U;
			websWrite(wp, "enable[%d]=\"%s\";", j, T("on"));
			d_temp = prof->timeEntry[j].day;
			Byte2Days(d_temp, data);
			websWrite(wp, "day[%d]=\"%s\";", j, data);
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
			if(prof->timeEntry[j].timestart==1){
			    websWrite(wp, "timeStart[%d]=\"All\";",j);
			}
			else{
			    formatTime(prof->timeEntry[j].timestart,the_time);
			    websWrite(wp, "timeStart[%d]=\"%s\";",j,the_time);
			}
			if(prof->timeEntry[j].timestop==86401)
			{
			    websWrite(wp, "timeStop[%d]=\"0\";",j);
			}
			else{
			    formatTime(prof->timeEntry[j].timestop,the_time);
			    websWrite(wp, "timeStop[%d]=\"%s\";",j,the_time);
			}
#else
			websWrite(wp, "timeStart[%d]=\"%s\";", j, prof->timeEntry[j].timestart);
			websWrite(wp, "timeStop[%d]=\"%s\";", j, prof->timeEntry[j].timestop);
#endif
			TIME_GRP_PRINTF("%s-%d:timeEntry%d day=%s, timestart=%s, timestop=%s\n",__FUNCTION__,__LINE__,j,data,prof->timeEntry[j].timestart,prof->timeEntry[j].timestop);
			TIME_GRP_PRINTF("--------------------------------------------\n");
		    }
		    else
		    {
			websWrite(wp, "enable[%d]=\"%s\";", j, T("off"));
			TIME_GRP_PRINTF("--------------------------------------------\n");
		    }
		}
	    }
	}
    }
    /*get system error msg*/
    getTheErrorMsg(eid, wp, argc, argv);
#endif
    return 0;
}
/********************************************************************
 * 函数： getTimeGrpList
 * 功能： 向页面输出时间段
 * 创建： 2012-05-10
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
extern int getTimeGrpList(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_TIMEGRP;
    TimeGrpProfile *prof = NULL;

    int min = 0, max = 0;
    int i = 0;
    int num = 0;

    websWrite(wp,"var addTimeGroupNames = new Array();");

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) 
    {
	prof = (TimeGrpProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) 
	{
	    websWrite(wp,"addTimeGroupNames[%d] = \"%s\";",num,prof->head.name);
	    num++;
	}
    }
    return 0;
}
/*******************************************************************
* 函数： formTimeGroupListDelAll
* 功能： 
* 创建： 2012-05-15
* 参数： 
* 返回： 略
* 输出： 略
* 作者：  
 ********************************************************************/
static void formTimeGroupListDelAll(webs_t wp, char_t *path, char_t *query)
{
#if 1
    MibProfileType mibType=MIB_PROF_TIMEGRP;
    TimeGrpProfile *prof=NULL;
    struProfAlloc *profhead = NULL;
    char err_Msg[60]={0};
    const char *timeStr=NULL;
    const char *quotStr=NULL;

    int i=0;
    int min,max;
    memset(err_Msg, 0, 60);
    ProfInstLowHigh(mibType,&max,&min);
    for(i=min;i<max;i++)/*del from min to max*/
    {
	prof=(TimeGrpProfile *)ProfGetInstPointByIndex(mibType,i);
	if((prof!=NULL)&&(ProfInstIsFree(prof)==0))
	{
	    TIME_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
	    if(isReredByRule(prof)==0) /*没有被其他规则引用*/
	    {
		ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, i, &profhead);
		TIME_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
		ProfDelInstByIndex(mibType, i);
	    }
	    else
	    {
		timeStr = getTransDataForC(C_LANG_INDEX_TIME_GROUP);
		quotStr = getTransDataForC(C_LANG_INDEX_BE_QUOTED);
		sprintf(err_Msg, "%s%s%s", timeStr, prof->head.name, quotStr);
		setTheErrorMsgOnFree(err_Msg);
	    }

	}
    }
    TIME_GRP_PRINTF("%s-%d:-----\n",__FUNCTION__,__LINE__);
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit(); /*save*/

    websRedirect(wp, "TimeRange.asp");
#endif
    return;
}
/*******************************************************************
* 函数： formTimeGroupListDel
* 功能： 
* 创建： 2012-05-15
* 参数： 
* 返回： 略
* 输出： 略
* 作者：  
 ********************************************************************/
static void formTimeGroupListDel(webs_t wp, char_t *path, char_t *query)
{
#if 1
    MibProfileType mibType = MIB_PROF_TIMEGRP;
    TimeGrpProfile *prof = NULL;
    struProfAlloc *profhead = NULL;

    char_t *timeGroupName = NULL; 
    char_t *tmp = NULL;
    const char *errMsg=NULL;
    const char *timeStr=NULL;
    const char *quotStr=NULL;
    char err_Msg[60]={0};
    memset(err_Msg, 0, 60);
    a_assert(wp);
    /*
     * 删除所有选择的PROFILE
     */
    timeGroupName = websGetVar(wp,T("delstr"),T("")); 
    TIME_GRP_PRINTF("%s-%d, get delstr = %s\n", __FUNCTION__, __LINE__, timeGroupName);
    if (strcmp(timeGroupName,"")==0) 
    {
	errMsg = getTransDataForC(C_LANG_INDEX_GROUP_NAME_NOT_FIND);
	setTheErrorMsgOnFree(errMsg);
    }
    else
    {
	tmp = strtok(timeGroupName, UTT_SPLIT_SIGN_STR);
	while(tmp != NULL) 
	{
	    prof=(TimeGrpProfile *)ProfGetInstPointByName(mibType, tmp);
	    if(prof)
	    {
		if(isReredByRule(prof)==0) /*没有被其他规则引用*/
		{
		    TIME_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
		    ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profhead);
		    ProfDelInstByName(mibType, tmp); 
		}
		else
		{
		    timeStr = getTransDataForC(C_LANG_INDEX_TIME_GROUP);
		    quotStr = getTransDataForC(C_LANG_INDEX_BE_QUOTED);
		    sprintf(err_Msg, "%s%s%s", timeStr, prof->head.name, quotStr);
		    setTheErrorMsgOnFree(err_Msg);
		}
	    }
	    TIME_GRP_PRINTF("%s-%d:del str=%s\n",__FUNCTION__,__LINE__,tmp);
	    tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
	}
    TIME_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
    ProfUpdate(profhead);
    TIME_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
    ProfFreeAllocList(profhead);
    TIME_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
    nvramWriteCommit(); /*save*/
    TIME_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
    }
    websRedirect(wp, "TimeRange.asp");/*页面跳转*/
#endif
    return;	   
}
/*******************************************************************
* 函数： formTimeRangeConfig
* 功能： 
* 创建： 2012-05-15
* 参数： 
* 返回： 略
* 输出： 略
* 作者：  
 ********************************************************************/
static void formTimeGroupConfig(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_TIMEGRP;
    TimeGrpProfile *prof = NULL;
    struProfAlloc *profList  = NULL;
    ProfChangeType profAction;
    int instIndex=0;
    const char *errMsg=NULL;
    /*define varliable*/
    char_t *timeRangeName = NULL;
    char_t *timeRangeNameOld = NULL;
    char_t *datestart = NULL;
    char_t *datestop = NULL;
    char_t *action = NULL;
    char_t *timeRange1 = NULL;
    char_t *day1 = NULL;
    char_t *timestart1 = NULL;
    char_t *timestop1 = NULL;
    char_t *timeRange2 = NULL;
    char_t *day2 = NULL;
    char_t *timestart2 = NULL;
    char_t *timestop2 = NULL;
    char_t *timeRange3 = NULL;
    char_t *day3 = NULL;
    char_t *timestart3 = NULL;
    char_t *timestop3 = NULL;
    char_t *effectyear =NULL;
    /*get value from web*/
    timeRangeName = websGetVar(wp, T("TimeRangeName"),T(""));
    timeRangeNameOld = websGetVar(wp, T("TimeRangeNameOld"),T(""));
    datestart = websGetVar(wp, T("year1"),T(""));
    datestop = websGetVar(wp, T("year2"),T(""));
    effectyear=websGetVar(wp, T("effectyear"),T(""));

    action = websGetVar(wp,T("Action"), T(""));
    timeRange1 = websGetVar(wp, T("timeRange1"),T(""));
    day1 = websGetVar(wp, T("day"),T(""));
    timestart1 = websGetVar(wp, T("timestart"),T(""));
    timestop1 = websGetVar(wp, T("timestop"),T(""));
    timeRange2 = websGetVar(wp, T("timeRange2"),T(""));
    day2 = websGetVar(wp, T("day2"),T(""));
    timestart2 = websGetVar(wp, T("timestart2"),T(""));
    timestop2 = websGetVar(wp, T("timestop2"),T(""));
    timeRange3 = websGetVar(wp, T("timeRange3"),T(""));
    day3 = websGetVar(wp, T("day3"),T(""));
    timestart3 = websGetVar(wp, T("timestart3"),T(""));
    timestop3 = websGetVar(wp, T("timestop3"),T(""));
    TIME_GRP_PRINTF("%s-%d:timeRangeName=%s,datestart=%s,datestop=%s\n",__FUNCTION__,__LINE__,timeRangeName,datestart,datestop);
    TIME_GRP_PRINTF("%s-%d:timeRange1 checked=%s,day=%s,timestart=%s,timestop=%s\n",__FUNCTION__,__LINE__,timeRange1,day1,timestart1,timestop1);
    TIME_GRP_PRINTF("%s-%d:timeRange2 checked=%s,day=%s,timestart=%s,timestop=%s\n",__FUNCTION__,__LINE__,timeRange2,day2,timestart2,timestop2);
    TIME_GRP_PRINTF("%s-%d:timeRange3 checked=%s,day=%s,timestart=%s,timestop=%s\n",__FUNCTION__,__LINE__,timeRange3,day3,timestart3,timestop3);
    TIME_GRP_PRINTF("%s-%d, action = %s\n", __func__, __LINE__, action);

    if (strcmp(action, "add") == 0) 
    {/*add inst*/
	TIME_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
	if(ProfGetInstIndexByName(mibType,timeRangeName) == PROFFAIL)
	{
	    instIndex = ProfNewInst(mibType, timeRangeName, FALSE);
	    TIME_GRP_PRINTF("%s-%d:instIndex=%d\n",__FUNCTION__,__LINE__,instIndex);
	    if(instIndex == PROFFAIL) 
	    {
		errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_MAX_TOTAL_ERR);
		setTheErrorMsgOnFree(errMsg);
		websRedirect(wp,T("TimeRange.asp"));
		return;
	    }
	}
	else
	{
	    errMsg = getTransDataForC(C_LANG_INDEX_INST_EXIST);
	    setTheErrorMsgOnFree(errMsg);
	    websRedirect(wp,T("TimeRange.asp"));
	    return;
	}

        profAction = PROF_CHANGE_ADD;
    }
    else
    {
	/*modify inst*/
	if(strcmp(timeRangeName,timeRangeNameOld)==0)
	{
	    instIndex = ProfGetInstIndexByName(mibType, timeRangeNameOld);
	    TIME_GRP_PRINTF("%s-%d:instIndex=%d\n",__FUNCTION__,__LINE__,instIndex);
	    if (instIndex == PROFFAIL) {
		errMsg = getTransDataForC(C_LANG_INDEX_MOD_INST_NOT_EXIST);
		setTheErrorMsgOnFree(errMsg);
		return;
	    }
	}
	else
	{
	    errMsg = getTransDataForC(C_LANG_INDEX_TIMEGRP_NOT_MOD);
	    setTheErrorMsgOnFree(errMsg);
	    websRedirect(wp,T("TimeRange.asp"));
	    return;
	}
        profAction = PROF_CHANGE_EDIT;
    }

    ProfBackupByIndex(mibType, profAction, instIndex, &profList);
    prof = (TimeGrpProfile *)ProfGetInstPointByIndex(mibType, instIndex);
    if((prof!=NULL))
    {
    /*set inst value*/
    prof->head.active=1;
    if(strcmp(effectyear, T("Forever")) == 0)
    {
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	prof->datestart=1;
	prof->datestop=2;
#else
	strcpy(prof->datestart, T("Forever"));
	strcpy(prof->datestop, T("0"));
#endif
    }
    else
    {
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	prof->datestart=transDate(datestart);
	prof->datestop=transDate(datestop);
#else
    	strcpy(prof->datestart, datestart);
    	strcpy(prof->datestop, datestop);
#endif
    if(strcmp(timeRange1, TR_CHECKED) == 0)
    {
	TIME_GRP_PRINTF("%s-%d:timeRange1 checked\n",__FUNCTION__,__LINE__);
	prof->timeEntry[0].enable = FUN_ENABLE;
	prof->timeEntry[0].day = days2Byte(day1); 
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	if(strcmp(timestart1,"All")==0){
	    prof->timeEntry[0].timestart=1;
	    prof->timeEntry[0].timestop=86401;
	} else {
	    prof->timeEntry[0].timestart=ChangeTimeToSec(timestart1);
	    prof->timeEntry[0].timestop=ChangeTimeToSec(timestop1);
	}
#else
	strcpy(prof->timeEntry[0].timestart, timestart1);
	strcpy(prof->timeEntry[0].timestop, timestop1);
	TIME_GRP_PRINTF("%s-%d:enable=%d,day=%d,timestart=%s,timestop=%s\n",__FUNCTION__,__LINE__,prof->timeEntry[0].enable,prof->timeEntry[0].day,prof->timeEntry[0].timestart,prof->timeEntry[0].timestop);
#endif
    }
    else 
    {
	prof->timeEntry[0].enable = FUN_DISABLE;
	TIME_GRP_PRINTF("%s-%d:timeRange1 not checked,enable=%d\n",__FUNCTION__,__LINE__,prof->timeEntry[0].enable);
    }
    TIME_GRP_PRINTF("-------------------------------------------------------\n");
    if(strcmp(timeRange2, TR_CHECKED) == 0)
    {
	TIME_GRP_PRINTF("%s-%d:timeRange2 checked\n",__FUNCTION__,__LINE__);
	prof->timeEntry[1].enable = FUN_ENABLE;
	prof->timeEntry[1].day = days2Byte(day2); 
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	if(strcmp(timestart2,"All")==0){
	    prof->timeEntry[1].timestart=1;
	    prof->timeEntry[1].timestop=86401;
	}
	else{
	    prof->timeEntry[1].timestart=ChangeTimeToSec(timestart2); 
	    prof->timeEntry[1].timestop=ChangeTimeToSec(timestop2);
	}
#else
	strcpy(prof->timeEntry[1].timestart, timestart2);
	strcpy(prof->timeEntry[1].timestop, timestop2);
	TIME_GRP_PRINTF("%s-%d:enable=%d,day=%d,timestart=%s,timestop=%s\n",__FUNCTION__,__LINE__,prof->timeEntry[1].enable,prof->timeEntry[1].day,prof->timeEntry[1].timestart,prof->timeEntry[1].timestop);
#endif
    }
    else 
    {
	prof->timeEntry[1].enable = FUN_DISABLE;
	TIME_GRP_PRINTF("%s-%d:timeRange2 not checked,enable=%d\n",__FUNCTION__,__LINE__,prof->timeEntry[1].enable);
    }
    TIME_GRP_PRINTF("-------------------------------------------------------\n");
    if(strcmp(timeRange3, TR_CHECKED) == 0)
    {
	TIME_GRP_PRINTF("%s-%d:timeRange3 checked\n",__FUNCTION__,__LINE__);
	prof->timeEntry[2].enable = FUN_ENABLE;
	prof->timeEntry[2].day = days2Byte(day3); 
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	if(strcmp(timestart3,"All")==0){//全天
	    prof->timeEntry[2].timestart=1;
	    prof->timeEntry[2].timestop=86401;
	} else {
	    prof->timeEntry[2].timestart=ChangeTimeToSec(timestart3);
	    prof->timeEntry[2].timestop=ChangeTimeToSec(timestop3);
	}
#else
	strcpy(prof->timeEntry[2].timestart, timestart3);
	strcpy(prof->timeEntry[2].timestop, timestop3);
	TIME_GRP_PRINTF("%s-%d:enable=%d,day=%d,timestart=%s,timestop=%s\n",__FUNCTION__,__LINE__,prof->timeEntry[2].enable,prof->timeEntry[2].day,prof->timeEntry[2].timestart,prof->timeEntry[2].timestop);
#endif
    }
    else
    {
	prof->timeEntry[2].enable = FUN_DISABLE;
	TIME_GRP_PRINTF("%s-%d:timeRange3 not checked,enable=%d\n",__FUNCTION__,__LINE__,prof->timeEntry[2].enable);
    }
    }
    TIME_GRP_PRINTF("-------------------------------------------------------\n");
    }

    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();/*save*/
#if 0
out:
#endif
    websRedirect(wp, "TimeRange.asp");
    return;
}
/********************************************************************
 * 函数： websDefineTimeGrp
 * 功能： 页面与交互转换函数
 * 创建： 2012-05-15
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： 
 ********************************************************************/ 
extern void websDefineTimeGrp(void)
{	/*form function*/
	websFormDefine(T("formTimeGroupConfig"), formTimeGroupConfig);
	websFormDefine(T("formTimeGroupListDel"), formTimeGroupListDel);
	websFormDefine(T("formTimeGroupListDelAll"), formTimeGroupListDelAll);
	/*asp function*/
	websAspDefine(T("aspOutputTimeGroupListData"), getTimeGroupListData);
	websAspDefine(T("aspOutputTimeGroupOneInfo"), getTimeGroupOneInfo);
	websAspDefine(T("aspOutTimeGrpList"),getTimeGrpList);
}
#endif
