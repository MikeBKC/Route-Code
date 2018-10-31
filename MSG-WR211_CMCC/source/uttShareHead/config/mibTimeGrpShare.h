#ifndef MIB_TIME_GROUP_SHARE_H
#define MIB_TIME_GROUP_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"
#if (TIME_GRP == FYES)

#define TIMEGRP_ENTRYS 3


/*组条目*/
typedef struct st_timeGrpEntry{
    FunEnableEnum  enable;/*是否启用该时间段*/
    UINT32 day;/*周几*/
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    UINT32  timestart;/*开始时间*/
    UINT32  timestop;/*结束时间*/
#else
    char timestart[UTT_TIME_LEN];/*开始时间*/
    char timestop[UTT_TIME_LEN];/*结束时间*/
#endif
} timeGrpEntry;

typedef struct st_timeGrpProfile{
    mibHead head;
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    UINT32 datestart;/*开始日期*/
    UINT32 datestop;/*结束日期*/
#else
    char datestart[UTT_DATE_LEN + 1];/*开始日期*/
    char datestop[UTT_DATE_LEN + 1];/*结束日期*/
#endif

    timeGrpEntry timeEntry[TIMEGRP_ENTRYS];
}TimeGrpProfile;

#endif
#endif
