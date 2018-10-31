#ifndef _TIME_SET_H
#define _TIME_SET_H

#include <linux/list.h>
#include "mibShare.h"

#define TIME_SET_UP_INTERVAL 10 /*ʱ������¼������λΪs*/
#define SECONDS_OF_DAY (24*60*60)
#define SECONDS_OF_HOUR  (60*60)
#define SECONDS_OF_MINUTE (60)
#define MINUTE_OF_HOUR (60)

extern struct list_head uttDayChgHookHead;/*һ���췢���˱仯���ͻ����������������ĺ���ִ��һ��*/
extern struct list_head uttHourChgHookHead;/*һ��Сʱ�����˱仯���ͻ����������������ĺ���ִ��һ��*/
extern struct list_head uttMinChgHookHead;/*һ�����ӷ����˱仯���ͻ����������������ĺ���ִ��һ��*/
extern uint16_t uttTimeSetMax;

/*Utt����ʱ��*/
typedef struct{
    uint32_t date;/*����Ϊʱ�侫ȷ���졣ע��linuxʱ�����1970-01-01 ��ʼ*/
    uint32_t week;/*��*/
    uint32_t time;/*ʱ��*/
} UttDateTime;

typedef int uttDTimeChgFn(const UttDateTime*timeNow, void *askerData);


extern const UttDateTime* getUttDTimeNow(void);
extern const UttDateTime* getUttDTimeOld(void);
/*ʱ��仯���Ӻ����ṹ��*/
typedef struct{ 
    struct list_head list;
    void *askerData; /*ע�ṳ�ӽڵ�ʱ���������*/
    uttDTimeChgFn  *fn;/*���Ӻ���*/
}UttDateTimeChgHookOps;

/**
 *�򻯵�API�ӿ�
 *
 */
/*�ձ仯*/
extern int uttRegDTimeChgHook (struct list_head *hookHead, void *askerData, uttDTimeChgFn *fn);
extern void uttUnRegDTimeChgHook (struct list_head *hookHead, void *askerData, uttDTimeChgFn *fn);
extern void uttDTimeChgHookRefresh (struct list_head *hookHead,void *askerData);

#define  uttRegDayChgHook(askerData,fn) \
    uttRegDTimeChgHook(&uttDayChgHookHead,askerData,fn)

#define  uttUnRegDayChgHook(askerData,fn) \
    uttUnRegDTimeChgHook(&uttDayChgHookHead,askerData,fn)

#define  uttDayChgHookRefresh(askerData) \
    uttDTimeChgHookRefresh(&uttDayChgHookHead,askerData)

/*Сʱ�仯*/
#define  uttRegHourChgHook(askerData,fn) \
    uttRegDTimeChgHook(&uttHourChgHookHead,askerData, fn)

#define  uttUnRegHourChgHook(askerData,fn) \
    uttUnRegDTimeChgHook(&uttHourChgHookHead,askerData,fn)

#define  uttHourChgHookRefresh(askerData) \
    uttDTimeChgHookRefresh(&uttHourChgHookHead,askerData)

/*���ӱ仯*/
#define  uttRegMinChgHook(askerData,fn) \
    uttRegDTimeChgHook(&uttMinChgHookHead,askerData,fn)

#define  uttUnRegMinChgHook(askerData,fn) \
    uttUnRegDTimeChgHook(&uttMinChgHookHead,askerData,fn)

#define  uttMinrChgHookRefresh(askerData) \
    uttDTimeChgHookRefresh(&uttMinChgHookHead,askerData)
#define UTT_TIME_SET_MAXNAMELEN 16	/*����Ϊ��ʱ����profile nameһ���ĳ���*/

/**
 *�ýṹ��Ӧ��ҳ�������õ�ʱ�䵥Ԫ
 */
typedef struct uttTimeSetWeekTime{
    uint32_t week;/*�ܼ�*/
    uint32_t timestart;/*ʱ�֣����ﾫȷ���롣�Է�����չ��24*60*60.һ�����86400��*/
    uint32_t timestop;
    struct list_head list;/*�ҽӵ�UttTimeSet->weekTimeList*/
} UttTimeSetWeekTime;

typedef int uttTimeToDTChg (void *conf);

typedef struct uttTimeWaitNode {
    struct list_head waitNode; /*���ڹ��ص�ʱ����ȴ�����timeSetWaitList��*/
    void *conf;
    uttTimeToDTChg *fn;
} UttTimeWaitNode;

extern bool addToTimeSetWaitList(void *conf,uttTimeToDTChg *fn);
extern void delFromWaitList(void *conf);

typedef struct uttTimeSet {
    char name[UTT_TIME_SET_MAXNAMELEN]; /* the name of the time set */
    rwlock_t lock;	    /*������迼���������������д��*/
    atomic_t ref;	    /*��ʱ���鱻��������ģ���������*/
    bool isOnTimeChangeList;/*��־��ʱ�����Ƿ���ص���ʱ��仯������*/
    uint32_t datestart;/*��ʼ���ڡ�����Ϊʱ�侫ȷ���졣ע��linuxʱ�����1970-01-01 ��ʼ*/
    uint32_t datestop;/*��������*/
    struct list_head weekTimeList; /*����Ϣ��ء���profile�ж�Ӧ��ʱ�䵥Ԫ��δ���ã�����Ϊ����ʱ��,���ΪNULL*/
    struct list_head askerHookHead;/*���ø�ʱ����Ĺ���ģ�鹳������.*/
    struct list_head delayExecList;/*���ظ�ʱ�����ϴ�����ִ�е��û���������*/
    uint16_t enable;/*ȡ��3λ���ӵ͵��߷ֱ��ʾ�֡�ʱ�����Ƿ���Ч*/
}UttTimeSet;


/**
 *enable: 0 ��ʱ����ʧЧ�� >0 ��ʱ������Ч
 *askerData�� ��������������
 */
typedef int uttTimeSetAskerFn(uint32_t enable, void *askerData);
/**
 *����ʱ����Ĺ��ӽṹ
 */
typedef struct{
    struct list_head list;/*�ҽӵ�UttTimeSet��askerHookHead����*/
    void *askerData; /*ע�ṳ�ӽڵ�ģ�������*/
    uttTimeSetAskerFn  * fn;/*���Ӻ���*/
}UttTimeSetAskerHookOps, UttTimeAskerHookOps;

extern uint16_t getTimeSetIndex(char *name);
extern uint16_t uttTimeSetCreat(const TimeGrpProfile * timeGrp);
extern uint16_t uttTimeSetModify(const TimeGrpProfile * timeGrp);
extern uint16_t uttTimeSetDel(const TimeGrpProfile * timeGrp);
extern void uttTimeSetDelByIndex(uint16_t index);
extern void uttTimeSetDelAll(void);


extern int uttRegTimeSetToDTChg (void *timeSetConf);
extern void uttUnRegTimeSetFromDTChg (UttTimeSet *timeSet);

extern bool uttRegToTimeSet (uint16_t index,void *askerData, uttTimeSetAskerFn *fn);
extern int uttUnRegFromTimeSet (uint16_t index,void *askerData);
#endif
