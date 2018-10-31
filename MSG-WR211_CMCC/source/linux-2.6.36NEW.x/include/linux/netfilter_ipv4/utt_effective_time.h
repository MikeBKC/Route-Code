#ifndef __UTT_EFFECTIVE_TIME_H__
#define __UTT_EFFECTIVE_TIME_H__

#include <linux/types.h>
#include <linux/list.h>
#include <linux/netfilter_ipv4/time_set.h>

/*规则生效时间*/
typedef struct st_uttEffectiveTime{
    u_long wday;/*星期几*/
    u_long timestart;/*24*60*60.一天最多86400秒*/
    u_long timestop;
} UttEffectiveTime;

/*生效时间钩子结构体定义如下*/
typedef struct{
    struct list_head list; /*将该生效时间挂载到全局生效时间链表的钩子*/
    rwlock_t lock;		
    atomic_t ref;	    /* 配置引用计数 */
    atomic_t onTimeRef;     /* 表示该时间hook被实际引用的计数，大于0，则说明不能将该hook从时间变化链表中卸载*/
    bool isOnTimeChangeList;/*标志该时间配置是否挂载到了时间变化链表上*/
    UttEffectiveTime effectiveTime;      /*生效时间*/
    struct list_head askerHookHead;/*引用生效时间的功能模块钩子链表.跟时间组保持一致*/
    struct list_head delayExecList;/*挂载该时间组上待立即执行的用户配置链表*/
    uint16_t enable;/*表示该时间是否生效*/
} UttTimeHookOps;

/*根据effectiveTime生成timeHookOps指针*/
extern UttTimeHookOps *uttGetTimeHookOpsRef (UttEffectiveTime *effectiveTime);

/*将对UttTimeHookOps指针的引用释放*/
extern void uttPutTimeHookOpsRef (UttTimeHookOps *timeHookOps); 

/*将应用模块配置添加到timeHook生效时间上*/
extern bool uttRegToEffTimeHook (UttTimeHookOps *timeHook, void *conf, uttTimeSetAskerFn *fn);

/*将用户模块从相应的生效时间链表上删除*/
extern int uttUnRegFromTimeHook (UttTimeHookOps *timeHook , void *conf);
#endif
