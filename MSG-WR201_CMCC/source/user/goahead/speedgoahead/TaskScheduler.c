
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <wsIntrn.h>
#include <um.h>
#include <utils.h>
#include <uttMachine.h>
#include "typedef.h"
#include "mibunion.h"
#include "mib.h"
#include "profacce.h"
#include "uttfunction.h"
#include "mibTaskSched.h"


#if (TASK_SCHEDULER == FYES)
/***************************************************************************
 * Function: task_sched_prof_change
 *
 * Description: exec cron-init.sh 
 * 
 * History: added by Chen.Shiyu, 2011.10.28
****************************************************************************/
#if (MIDDLE_SOFT == FYES)
#define CHECKVAR "checkreboot &"
#endif
static void task_sched_prof_change(void)
{
    char schedRecord[512];
    char *list = schedRecord;
    MibProfileType mibType = MIB_PROF_TASK_SCHED;
    TaskSchedProfile *prof = NULL;
    int i, min = 0, max = 0;
 
    memset(schedRecord, 0, sizeof(schedRecord)); /*init schedRecord[]*/
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++)
    {
	prof = (TaskSchedProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0))
	{
	    if(strcmp(prof->pType,"04")== 0)
	    {
		strcat(list, "*/1");  /*每分钟*/
	    }
	    else
	    { 
		strcat(list, prof->pMin);   /*每小时 每天*/
	    }
	    strcat(list, UTT_SPLIT_SIGN_SPACES);
	    if((strcmp(prof->pType,"02") == 0) || (strcmp(prof->pType,"01") == 0))
	    {
		strcat(list, prof->pHour);  /*每天*/
	    }
	    else
	    {
		strcat(list, UTT_SPLIT_SIGN_STATS);  
	    }
	    strcat(list, UTT_SPLIT_SIGN_SPACES); 
	    strcat(list, UTT_SPLIT_SIGN_STATS);  
	    strcat(list, UTT_SPLIT_SIGN_SPACES);
	    strcat(list, UTT_SPLIT_SIGN_STATS);  
	    strcat(list, UTT_SPLIT_SIGN_SPACES);

	    strcat(list, prof->pWeek);  
	    strcat(list, UTT_SPLIT_SIGN_SPACES);
#if (MIDDLE_SOFT == FYES)
        strcat(list, CHECKVAR);
#else
        strcat(list, prof->pCmd);
#endif
        /*use double-semicolon to split each task*/
	    strcat(list, UTT_SPLIT_DOUBLE_RECORDS); 
	}
    }
#if (WEBAUTH_ACCOUNT == FYES)
    /*如果开启web认证时间计费则调用自定义重启函数，先将数据保存然后重启*/
#if (MIDDLE_SOFT == FYES)
    doSystem("cron-init.sh del \"%s\" 1;cron-init.sh add \"%s\"", CHECKVAR,schedRecord); /*call script*/
#else
    doSystem("cron-init.sh del rebootS 1;cron-init.sh add \"%s\"", schedRecord); /*call script*/
#endif
#else
#if (MIDDLE_SOFT == FYES)
    doSystem("cron-init.sh del \"%s\" 1;cron-init.sh add \"%s\"", CHECKVAR,schedRecord); /*call script*/ 
#else
    doSystem("cron-init.sh del reboot 1;cron-init.sh add \"%s\"", schedRecord); /*call script*/ 
#endif
#endif
    return;
}

/***************************************************************************
 * Function: taskSchedInit
 *
 * Description: init plan scheduler.
 * 
 * History: added by Chen.Shiyu, 2011.10.28
****************************************************************************/
static void taskSchedInit(void)
{
    task_sched_prof_change();

    return;
}

/***************************************************************************
 * Function: taskSchedProfileChange
 *
 * Description: profile change
 * 
 * History: added by Chen.Shiyu, 2011.10.28
****************************************************************************/
void taskSchedProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType pChangeType, Uint32 index,
	const TaskSchedProfile *oldProfileDataPtr, const TaskSchedProfile *newProfileDataPtr)
{
    switch(pChangeType) {
	case PROF_CHANGE_ADD:
	case PROF_CHANGE_EDIT:
	case PROF_CHANGE_DEL:
	    task_sched_prof_change();			
	    break;
	case PROF_CHANGE_DELALL:
#if (WEBAUTH_ACCOUNT == FYES)
	    doSystem("cron-init.sh del rebootS");
#else
	    doSystem("cron-init.sh del reboot");
#endif
	    break;
	default: /* default operation */
	    break;
    }
}

/***************************************************************************
 * Function: funInitTaskSched
 *
 * Description: register taskSched profile change function and init task.
 * 
 * History: added by Chen.Shiyu, 2011.10.28
****************************************************************************/
void funInitTaskSched(void)
{
    registerForProfileUpdates(MIB_PROF_TASK_SCHED, (Pointer)NULL, (ProfileUpdateFnc)taskSchedProfileChange);
    taskSchedInit();
}

#endif
