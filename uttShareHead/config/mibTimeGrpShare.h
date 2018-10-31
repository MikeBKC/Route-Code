#ifndef MIB_TIME_GROUP_SHARE_H
#define MIB_TIME_GROUP_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"
#if (TIME_GRP == FYES)

#define TIMEGRP_ENTRYS 3


/*����Ŀ*/
typedef struct st_timeGrpEntry{
    FunEnableEnum  enable;/*�Ƿ����ø�ʱ���*/
    UINT32 day;/*�ܼ�*/
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    UINT32  timestart;/*��ʼʱ��*/
    UINT32  timestop;/*����ʱ��*/
#else
    char timestart[UTT_TIME_LEN];/*��ʼʱ��*/
    char timestop[UTT_TIME_LEN];/*����ʱ��*/
#endif
} timeGrpEntry;

typedef struct st_timeGrpProfile{
    mibHead head;
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    UINT32 datestart;/*��ʼ����*/
    UINT32 datestop;/*��������*/
#else
    char datestart[UTT_DATE_LEN + 1];/*��ʼ����*/
    char datestop[UTT_DATE_LEN + 1];/*��������*/
#endif

    timeGrpEntry timeEntry[TIMEGRP_ENTRYS];
}TimeGrpProfile;

#endif
#endif
