#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "webs.h"
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "uttfunction.h"
#include "mib.h"
#include "profacce.h"
#include "mibTaskSched.h"
#include "translate.h"


#if (TASK_SCHEDULER == FYES)
/*******************************************************************
 * Function: aspGetScheduler
 *
 * Description: To get scheduler values to web
 *
 * History: 2011.10.28 added by Chen.Shiyu
********************************************************************/
static int aspGetScheduler(int eid, webs_t wp, int argc, char_t **argv)
{
#if (MIDDLE_SOFT == FYES)
    MibProfileType mibType = MIB_PROF_TASK_SCHED;
    TaskSchedProfile *prof;
    int index = 0;
    char *pweek = NULL;
    int flag = 0;
    prof = (TaskSchedProfile*)ProfGetInstPointByIndex(mibType, index);
    if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0)&& (prof->head.active == TRUE))
    {
        flag = 1;
        websWrite(wp , "var flag = \"%d\";\n",flag );
        if(strcmp(prof->pWeek,"*") == 0)
        {
            pweek = " ";
        }else{

            pweek = prof->pWeek;
        }
        websWrite(wp , "var selDay = \"%s\";\n", pweek);
        websWrite(wp , "var txtHour1 = \"%s\";\n", prof->pHour);
        websWrite(wp , "var txtMin1 = \"%s\";\n",prof->pMin );
        websWrite(wp , "var ID = \"%s\";\n",prof->head.name );
        websWrite(wp , "var selDateType = \"%s\";\n",prof->pType );
    }
    else{
        websWrite(wp , "var flag = \"%d\";\n",flag );
    }
    return 0;
#endif
#if 0
    MibProfileType mibType = MIB_PROF_TASK_SCHED;
    TaskSchedProfile *prof;
    int index = 0, min = 0, max = 0,  num = 0;
    char *pweek = NULL;
    /*define the page vars*/
    char_t *names=T("names[%d] = \"%s\";");
    char_t *times=T("times[%d] = \"%s %s:%s:00\";");
    char_t *types=T("types[%d] = \"%s\";");
    char_t *cmds=T("cmds[%d] = \"%s\";");
    char_t *totalrecs=T("var totalrecs=%d;");
    char_t *max_totalrecs=T("var max_totalrecs=%d;");
    ProfInstLowHigh(mibType, &max, &min);
    for(index = min; index < max; index++) /*access all profile by index*/
    {
    prof = (TaskSchedProfile*)ProfGetInstPointByIndex(mibType, index);
	if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0)&& (prof->head.active == TRUE))
	{
        websWrite(wp, names, , prof->head.name);
		websWrite(wp, times, num, prof->pWeek, prof->pHour, prof->pMin);
		websWrite(wp, types, num, prof->pType);
		websWrite(wp, cmds, num, prof->pCmd);
		num++;
    websWrite(wp , "var selDateType = \"%s\";\n",prof->pType );
    if(strcmp(prof->pWeek,"*") == 0)
    {
        pweek = " ";
    }else{

        pweek = prof->pWeek;
    }
    websWrite(wp , "var selDay = \"%s\";\n", pweek);
    websWrite(wp , "var txtHour1 = \"%s\";\n", prof->pHour);
    websWrite(wp , "var txtMin1 = \"%s\";\n",prof->pMin );
    websWrite(wp , "var ID = \"%s\";\n",prof->head.name );
    
    }
    
    
    }
    websWrite(wp, totalrecs, num);
    websWrite(wp, max_totalrecs, max);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;

#endif

}

/*******************************************************************
 * Function: aspGetTaskOne
 *
 * Description: To get some a task scheduler
 *
 * History: 2011.10.28 added by Chen.Shiyu
********************************************************************/
static int aspGetTaskOne(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t *edit_name = NULL;
    MibProfileType mibType = MIB_PROF_TASK_SCHED;
    TaskSchedProfile *prof = NULL;
    int profile_num = 0;
	const char *ErrMsg = NULL;

    /*define the page vars*/
    char_t *names=T("var names=\"%s\";");
    char_t *times=T("var times=\"%s %s:%s:00\";");
    char_t *types=T("var types=\"%s\";");
    char_t *cmds=T("var cmds=\"%s\";");

    if(ejArgs(argc,argv,T("%s"), &edit_name) <1) 
    {
		ErrMsg = getTransDataForC(C_LANG_INDEX_READ_EDIT_NAME_VAL_ERR);
		setTheErrorMsg(ErrMsg);
    } 
    else 
    {
	if(edit_name == NULL) 
	{
		ErrMsg = getTransDataForC(C_LANG_INDEX_TASK_NAME_CANT_NULL);
	    setTheErrorMsg(ErrMsg);
	} 
	else 
	{
	    /* find the profile no by name */
	    profile_num = ProfGetInstIndexByName(mibType, edit_name);
	    if(profile_num == PROFFAIL)
	    {
			ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_USER_NAME);
			setTheErrorMsg(ErrMsg);
	    }
	    else
	    {
			prof = (TaskSchedProfile*)ProfGetInstPointByIndex(mibType, profile_num); /* get profile by index */
			if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0))
			{
		    /* define vars to web page*/
				websWrite(wp, names, prof->head.name);
				 websWrite(wp, times, prof->pWeek, prof->pHour, prof->pMin);
				websWrite(wp, types, prof->pType);
			    websWrite(wp, cmds, prof->pCmd);
			}
	    }
	}
    }

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}


/*******************************************************************
 * Function: formTaskDelAll
 *
 * Description: To del all task scheduler
 *
 * History: 2011.10.28 added by Chen.Shiyu
********************************************************************/
static void formTaskDelAll(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_TASK_SCHED;
    TaskSchedProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction = PROF_CHANGE_DELALL;
    int i, min = 0, max = 0, back = 0;

    a_assert(wp);
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) /*del all*/
    {
	prof = (TaskSchedProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0))
	{
	    if(back == 0)/*backup one time is enough*/
	    {
		ProfBackupByIndex(mibType, profAction, i, &profhead);
		back = 1;
	    }

	    ProfDelInstByIndex(mibType, i);
	}
    }

    ProfUpdate(profhead); /* update profile */
    ProfFreeAllocList(profhead); /* free alloc profile list*/
    nvramWriteCommit();
    websRedirect(wp, "TaskScheduler.asp"); 
}


/*******************************************************************
 * Function: formConfigTaskDel
 *
 * Description: To delete the configurations about scheduler.
 *
 * History: 2011.10.28 added by Chen.Shiyu
********************************************************************/
static void formConfigTaskDel(webs_t wp, char_t *path, char_t *query)
{
    char_t *UserName=NULL, *tmp=NULL;
    MibProfileType mibType = MIB_PROF_TASK_SCHED;
    TaskSchedProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction = PROF_CHANGE_DEL;
    int profile_num = 0, prof_change = 0;
	const char * ErrMsg = NULL;

    a_assert(wp);

    UserName = websGetVar(wp,T("delstr"),T(""));
    if(UserName == NULL) 
    {
		ErrMsg = getTransDataForC(C_LANG_INDEX_TASK_NAME_CANT_NULL);
		setTheErrorMsg(ErrMsg);
    }
    else 
    {
	tmp=strtok(UserName, UTT_SPLIT_SIGN_STR);
	while(tmp != NULL)
	{
	    profile_num = ProfGetInstIndexByName(mibType, tmp);
	    if(profile_num == PROFFAIL)
	    {
			ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_USER_NAME);
			setTheErrorMsg(ErrMsg);
	    }
	    else
	    {
		prof = (TaskSchedProfile*)ProfGetInstPointByIndex(mibType, profile_num);
		if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0))
		{
		    ProfBackupByIndex(mibType, profAction, profile_num, &profhead);
		    ProfDelInstByIndex(mibType, profile_num); /*delete the profile by index*/
		    ProfUpdate(profhead); /* update profile */
		    prof_change = 1;
		}
	    }
	    tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
	}
    }

    if(prof_change == 1) /* Free the prohead and write profile to flash if the profile changed */
    {
	ProfFreeAllocList(profhead); /* free alloc profile list*/
	nvramWriteCommit(); /*save to flash*/
    }

    websRedirect(wp, "TaskScheduler.asp");
}

/*************************************************************************
 * Function: formConfigTaskUserEdit
 *
 * Description: To config plan scheduler, action:add/modify scheduler
 *
 * History: 2011.10.28 added by Chen.Shiyu
**************************************************************************/
static void formConfigTaskUserEdit(webs_t wp, char_t *path, char_t *query)
{
#if (MIDDLE_SOFT == FYES)
    char *pID = NULL, *pDatetype =  NULL, *p_week = NULL, *p_cmd = NULL;
    char *Hour1 = NULL ,*Min1 = NULL , *Sec1 = NULL, *Hour2 = NULL, *Min2 = NULL, *Sec2 =NULL;
    MibProfileType mibType = MIB_PROF_TASK_SCHED;
    TaskSchedProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    const char *ErrMsg = NULL;
    ProfChangeType profAction = PROF_CHANGE_EDIT;
    int instIndex = 0;
    pID = websGetVar(wp,T("ID"), T(""));
    pDatetype = websGetVar(wp,T("selDateType"), T(""));
    p_week = websGetVar(wp,T("selDay"), T(""));
    p_cmd = websGetVar(wp,T("selContent"), T(""));
    Hour1 = websGetVar(wp,T("txtHour1"), T(""));
    Min1 = websGetVar(wp,T("txtMin1"), T(""));
    Sec1 = websGetVar(wp,T("txtSec1"), T(""));
    Hour2 = websGetVar(wp,T("txtHour2"), T(""));
    Min2 = websGetVar(wp,T("txtMin2"), T(""));
    Sec2 = websGetVar(wp,T("txtSec2"), T(""));

        ProfBackupByIndex(mibType, profAction, instIndex, &profhead);/* Backup */
        prof = (TaskSchedProfile*)ProfGetInstPointByIndex(mibType, instIndex);
        if(prof != NULL)
        {
            prof->head.active = 1;
        }
         
        if(strcmp(pDatetype,"01") == 0)
        {
            sprintf(prof->pMin, "%s", Min1);
            sprintf(prof->pHour, "%s", Hour1);
            sprintf(prof->pWeek, "%s", p_week);
        }
        else
        {
            sprintf(prof->pMin, "%s", Min2);
            sprintf(prof->pHour, "%s", Hour2);
            sprintf(prof->pWeek, "*");
        }
        sprintf(prof->pCmd,"%s", p_cmd);
        sprintf(prof->pType, "%s", pDatetype);
        ProfSetNameByPoint(((Pointer)(prof)), pID);
        ProfUpdate(profhead); /* update profile */
        ProfFreeAllocList(profhead); /* free alloc profile list*/
        nvramWriteCommit(); /* write to flash */
        websRedirect(wp, "Timeplan.asp");
#endif

#if 0

    char *pID = NULL, *pAct = NULL, *pDatetype =  NULL, *p_week = NULL, *p_cmd = NULL, *pIDold = NULL;
    char *Hour1 = NULL ,*Min1 = NULL , *Sec1 = NULL, *Hour2 = NULL, *Min2 = NULL, *Sec2 =NULL;
    MibProfileType mibType = MIB_PROF_TASK_SCHED;
    TaskSchedProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    const char *ErrMsg = NULL;
    ProfChangeType profAction = PROF_CHANGE_EDIT;
    int instIndex = 0;
    int change = 1, instIndex = 0;

    /* To get vars from web */
       pAct = websGetVar(wp,T("Action"), T(""));
    pID = websGetVar(wp,T("ID"), T(""));
    pDatetype = websGetVar(wp,T("selDateType"), T(""));
    p_week = websGetVar(wp,T("selDay"), T(""));
    p_cmd = websGetVar(wp,T("selContent"), T(""));
    Hour1 = websGetVar(wp,T("txtHour1"), T(""));
    Min1 = websGetVar(wp,T("txtMin1"), T(""));
    Sec1 = websGetVar(wp,T("txtSec1"), T(""));
    Hour2 = websGetVar(wp,T("txtHour2"), T(""));
    Min2 = websGetVar(wp,T("txtMin2"), T(""));
    Sec2 = websGetVar(wp,T("txtSec2"), T(""));
    pIDold = websGetVar(wp, T("IDold"), T(""));
    if(strlen(pID) < (unsigned int)UTT_INST_NAME_LEN) /*The user name cannot over 11 bytes*/
    {
        if(strncmp(pAct, "add", 3) == 0) /*add a plan scheduler*/
        {
            instIndex = ProfGetInstIndexByName(mibType, pID);
            if(instIndex != PROFFAIL) /*find the name*/
            {
                ErrMsg = getTransDataForC(C_LANG_INDEX_TASK_NAME_EXITS);
                setTheErrorMsg(ErrMsg);
                change = 0;/*Redirect to TaskScheduler.asp*/
            }
            else
            {
                instIndex = ProfNewInst(mibType, pID, FALSE); /* creat a new profile */
                if((instIndex == PROFFAIL) || (instIndex == MAX_TASK_SCHED_PROFILES))
                {
                    ErrMsg = getTransDataForC(C_LANG_INDEX_IS_MAX_CANT_ADD_MORE);
                    setTheErrorMsg(ErrMsg);
                    change = 0;/*Redirect to TaskScheduler.asp*/
                }
            }
            profAction = PROF_CHANGE_ADD;
        }
        else
        {
            /* add if for bug 0010914
             * 当修改任务名, 任务名改动时
             * 检测编辑的任务名是否存在
             */
            if(strcmp(pID, pIDold) != 0){
                if(ProfGetInstIndexByName(mibType, pID) != PROFFAIL){
                    ErrMsg = getTransDataForC(C_LANG_INDEX_TASK_NAME_EXITS);
                    setTheErrorMsg(ErrMsg);
                    change = 0;/*Redirect to TaskScheduler.asp*/
                }
            }

            instIndex = ProfGetInstIndexByName(mibType, pIDold);
            if(instIndex == PROFFAIL)
            {
                ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_THE_TASK_NAME);
                setTheErrorMsg(ErrMsg);
                change = 0;/*Redirect to TaskScheduler.asp*/
            }
            profAction = PROF_CHANGE_EDIT;
        }
    }
    else
    {
        ErrMsg = getTransDataForC(C_LANG_INDEX_TASK_NAME_SHOULD_LESS_11);
        setTheErrorMsg(ErrMsg);
        change = 0; /*Redirect to TaskScheduler.asp*/
    }
    if (change == 1) /* change the profile configuration */
    {
        ProfBackupByIndex(mibType, profAction, instIndex, &profhead);/* Backup */
        prof = (TaskSchedProfile*)ProfGetInstPointByIndex(mibType, instIndex);
        if(prof != NULL)
        {
            prof->head.active = 1;
        }
         
        if(strcmp(pDatetype,"01") == 0)
        {
            sprintf(prof->pMin, "%s", Min1);
            sprintf(prof->pHour, "%s", Hour1);
            sprintf(prof->pWeek, "%s", p_week);
        }
        else
        {
            sprintf(prof->pMin, "%s", Min2);
            sprintf(prof->pHour, "%s", Hour2);
            sprintf(prof->pWeek, "*");
        }
        sprintf(prof->pCmd,"%s", p_cmd);
        sprintf(prof->pType, "%s", pDatetype);
        ProfSetNameByPoint(((Pointer)(prof)), pID);
        ProfUpdate(profhead); /* update profile */
        ProfFreeAllocList(profhead); /* free alloc profile list*/
        nvramWriteCommit(); /* write to flash */
        //   }
        websRedirect(wp, "Timeplan.asp");

#endif
}

/*******************************************************************
 * Function: formDefineTaskScheduler
 *
 * Description: 页面与交互转换函数
 *
 * History: 2011.10.28 added by Chen.Shiyu
********************************************************************/
void formDefineTaskScheduler(void) 
{
    websAspDefine(T("aspGetTaskOne"), aspGetTaskOne);
    websAspDefine(T("aspGetScheduler"), aspGetScheduler);
    websFormDefine(T("formTaskDelAll"), formTaskDelAll);
    websFormDefine(T("formTaskDel"), formConfigTaskDel);
    websFormDefine(T("formTaskEdit"), formConfigTaskUserEdit);
}
#endif
