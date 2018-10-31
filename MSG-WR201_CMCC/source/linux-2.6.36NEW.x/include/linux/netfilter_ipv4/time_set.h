#ifndef _TIME_SET_H
#define _TIME_SET_H

#include <linux/list.h>
#include "mibShare.h"

#define TIME_SET_UP_INTERVAL 10 /*时间组更新间隔，单位为s*/
#define SECONDS_OF_DAY (24*60*60)
#define SECONDS_OF_HOUR  (60*60)
#define SECONDS_OF_MINUTE (60)
#define MINUTE_OF_HOUR (60)

extern struct list_head uttDayChgHookHead;/*一旦天发生了变化，就会遍历该链表，把上面的函数执行一遍*/
extern struct list_head uttHourChgHookHead;/*一旦小时发生了变化，就会遍历该链表，把上面的函数执行一遍*/
extern struct list_head uttMinChgHookHead;/*一旦分钟发生了变化，就会遍历该链表，把上面的函数执行一遍*/
extern uint16_t uttTimeSetMax;

/*Utt定义时间*/
typedef struct{
    uint32_t date;/*这里为时间精确到天。注意linux时间戳从1970-01-01 开始*/
    uint32_t week;/*周*/
    uint32_t time;/*时分*/
} UttDateTime;

typedef int uttDTimeChgFn(const UttDateTime*timeNow, void *askerData);


extern const UttDateTime* getUttDTimeNow(void);
extern const UttDateTime* getUttDTimeOld(void);
/*时间变化钩子函数结构体*/
typedef struct{ 
    struct list_head list;
    void *askerData; /*注册钩子节点时间组的配置*/
    uttDTimeChgFn  *fn;/*钩子函数*/
}UttDateTimeChgHookOps;

/**
 *简化的API接口
 *
 */
/*日变化*/
extern int uttRegDTimeChgHook (struct list_head *hookHead, void *askerData, uttDTimeChgFn *fn);
extern void uttUnRegDTimeChgHook (struct list_head *hookHead, void *askerData, uttDTimeChgFn *fn);
extern void uttDTimeChgHookRefresh (struct list_head *hookHead,void *askerData);

#define  uttRegDayChgHook(askerData,fn) \
    uttRegDTimeChgHook(&uttDayChgHookHead,askerData,fn)

#define  uttUnRegDayChgHook(askerData,fn) \
    uttUnRegDTimeChgHook(&uttDayChgHookHead,askerData,fn)

#define  uttDayChgHookRefresh(askerData) \
    uttDTimeChgHookRefresh(&uttDayChgHookHead,askerData)

/*小时变化*/
#define  uttRegHourChgHook(askerData,fn) \
    uttRegDTimeChgHook(&uttHourChgHookHead,askerData, fn)

#define  uttUnRegHourChgHook(askerData,fn) \
    uttUnRegDTimeChgHook(&uttHourChgHookHead,askerData,fn)

#define  uttHourChgHookRefresh(askerData) \
    uttDTimeChgHookRefresh(&uttHourChgHookHead,askerData)

/*分钟变化*/
#define  uttRegMinChgHook(askerData,fn) \
    uttRegDTimeChgHook(&uttMinChgHookHead,askerData,fn)

#define  uttUnRegMinChgHook(askerData,fn) \
    uttUnRegDTimeChgHook(&uttMinChgHookHead,askerData,fn)

#define  uttMinrChgHookRefresh(askerData) \
    uttDTimeChgHookRefresh(&uttMinChgHookHead,askerData)
#define UTT_TIME_SET_MAXNAMELEN 16	/*定义为跟时间组profile name一样的长度*/

/**
 *该结构对应于页面中配置的时间单元
 */
typedef struct uttTimeSetWeekTime{
    uint32_t week;/*周几*/
    uint32_t timestart;/*时分（这里精确到秒。以方便扩展）24*60*60.一天最多86400秒*/
    uint32_t timestop;
    struct list_head list;/*挂接到UttTimeSet->weekTimeList*/
} UttTimeSetWeekTime;

typedef int uttTimeToDTChg (void *conf);

typedef struct uttTimeWaitNode {
    struct list_head waitNode; /*用于挂载到时间组等待链表timeSetWaitList上*/
    void *conf;
    uttTimeToDTChg *fn;
} UttTimeWaitNode;

extern bool addToTimeSetWaitList(void *conf,uttTimeToDTChg *fn);
extern void delFromWaitList(void *conf);

typedef struct uttTimeSet {
    char name[UTT_TIME_SET_MAXNAMELEN]; /* the name of the time set */
    rwlock_t lock;	    /*这个还需考虑下用自旋锁或读写锁*/
    atomic_t ref;	    /*该时间组被其他功能模块的引用数*/
    bool isOnTimeChangeList;/*标志该时间组是否挂载到了时间变化链表上*/
    uint32_t datestart;/*开始日期。这里为时间精确到天。注意linux时间戳从1970-01-01 开始*/
    uint32_t datestop;/*结束日期*/
    struct list_head weekTimeList; /*周信息相关。若profile中对应的时间单元都未启用，或者为所有时间,则此为NULL*/
    struct list_head askerHookHead;/*引用该时间组的功能模块钩子链表.*/
    struct list_head delayExecList;/*挂载该时间组上待立即执行的用户配置链表*/
    uint16_t enable;/*取低3位，从低到高分别表示分、时、天是否生效*/
}UttTimeSet;


/**
 *enable: 0 该时间组失效， >0 该时间组生效
 *askerData： 申请者数据配置
 */
typedef int uttTimeSetAskerFn(uint32_t enable, void *askerData);
/**
 *引用时间组的钩子结构
 */
typedef struct{
    struct list_head list;/*挂接到UttTimeSet的askerHookHead链上*/
    void *askerData; /*注册钩子节点模块的配置*/
    uttTimeSetAskerFn  * fn;/*钩子函数*/
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
