#ifndef __MIB_TASK_SCHEDULER_SHARE_H
#define __MIB_TASK_SCHEDULER_SHARE_H

#define SCHEDULER_LENGTH    16

typedef struct _task_sched_profile {
    mibHead head;
    char pMin[4];
    char pHour[4];
    char pWeek[4];
    char pType[4];
    char pCmd[SCHEDULER_LENGTH];
} TaskSchedProfile;

#endif
