#include <linux/ip.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/jiffies.h>/*定义了jiffies*/
#include <asm/param.h>/*定义了HZ*/
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/netfilter_ipv4/time_set.h>
#include <linux/netfilter_ipv4/utt_effective_time.h>
#include <linux/hardirq.h>

#if 0 
#define EFFECTIVE_TIME_PRINT(fmt, args...)		printk(fmt, ## args)
#else
#define EFFECTIVE_TIME_PRINT(fmt, args...)
#endif
MODULE_AUTHOR("zhao.chong");
MODULE_DESCRIPTION("utt effective time");
MODULE_LICENSE("GPL");

/****************全局变量*******start********************/
static DEFINE_SPINLOCK(utt_effective_time_spinlock);
static LIST_HEAD(uttTimeHookHead);/*所有的生效时间挂载在这上面*/
/****************全局变量*******end********************/


/****************函数声明*******start******************/
static void uttEffectiveTimeLock(void);
static void uttEffectiveTimeUnlock(void);
static UttTimeHookOps* isEffectiveTimeExist(UttEffectiveTime  *effectiveTime);
static bool isUserConfExist(UttTimeHookOps *timeHookOps, void *conf);
static int uttRegTimeHook (UttTimeHookOps  *timeHook);
static int uttUnRegTimeHook (UttTimeHookOps  *timeHook);
static int uttRegEffectiveTimeToDTChg (void *efTimeHookOpsConf);
static void uttUnRegEffectiveTimeFromDTChg (UttTimeHookOps *efTimeHookOps);
/****************函数声明*******end******************/

static void uttEffectiveTimeLock(void)
{
    spin_lock_bh(&utt_effective_time_spinlock);
}

static void uttEffectiveTimeUnlock(void)
{
    spin_unlock_bh(&utt_effective_time_spinlock);
}

/********************************************************************
* 函数： uttEffectiveTimeMinuteChg
* 功能： 时间组分钟变化执行函数
* 创建： 2014-10-15
* 参数： @timeNow-当前时间；@conf-生效时间配置
* 返回： 0->成功；非0->失败
* 作者： zhao.chong
 ********************************************************************/
static int uttEffectiveTimeMinuteChg (const UttDateTime *timeNow, void *conf)
{
    UttTimeHookOps *efTimeHookOps = (UttTimeHookOps*)conf;
    uint16_t flag = 1U;/*分钟生效标志，默认生效*/
    uint16_t oldEnable;
    UttTimeAskerHookOps *ops,*next;

    EFFECTIVE_TIME_PRINT("func:%s--line:%d\n",__func__,__LINE__);
    if ((timeNow == NULL) || (efTimeHookOps == NULL)) {/*将该时间从分钟变化函数中卸载时*/
	EFFECTIVE_TIME_PRINT("func:%s--line:%d\n",__func__,__LINE__);
	return 1;
    }
    write_lock(&efTimeHookOps->lock);
    oldEnable = efTimeHookOps->enable & 0x1;
    if (oldEnable == 0x1) {
	if (!list_empty_careful(&efTimeHookOps->delayExecList)) {/*该时间组存在待马上执行的模块*/ 
	    list_for_each_entry_safe(ops,next,&efTimeHookOps->delayExecList,list){
		if (ops->fn != NULL) {
		    ops->fn(1,ops->askerData);
		}
		list_del(&ops->list);
		list_add_tail(&ops->list,&efTimeHookOps->askerHookHead);
	    }
	}
    }

    if (timeNow->week & efTimeHookOps->effectiveTime.wday) {/*当天*/
	/**
	 *需判断时间是否发生了跨天的情况
	 */
	if (efTimeHookOps->effectiveTime.timestart < efTimeHookOps->effectiveTime.timestop) {/*不跨天*/
	    if ((timeNow->time < efTimeHookOps->effectiveTime.timestart/SECONDS_OF_MINUTE) || (timeNow->time > efTimeHookOps->effectiveTime.timestop/SECONDS_OF_MINUTE)) {
		flag = 0U;/*分钟不生效*/
	    }
	} else {
	    if ((timeNow->time < efTimeHookOps->effectiveTime.timestart/SECONDS_OF_MINUTE) && (timeNow->time > efTimeHookOps->effectiveTime.timestop/SECONDS_OF_MINUTE)) {
		flag = 0U;/*分钟不生效*/
	    }
	}
    } else {
	flag = 0U;
    }

    if (flag == 1U) {
	efTimeHookOps->enable |= 0x1;/*分生效置位*/
    } else {
	efTimeHookOps->enable &= ~0x1;/*分生效清零*/
    }

    EFFECTIVE_TIME_PRINT("func:%s--line:%d,oldEnable=%d,efTimeHookOps->enable & 0x1=%d\n",__func__,__LINE__,oldEnable,(efTimeHookOps->enable & 0x1));
    if (oldEnable == 0) {
	if ((efTimeHookOps->enable & 0x1) == 0x1) {/*由不生效变为生效*/
	    list_for_each_entry_safe(ops,next,&efTimeHookOps->askerHookHead,list){
		if (ops->fn != NULL) {
		    ops->fn(efTimeHookOps->enable == 0x7,ops->askerData);
		}
	    }
	}
    } else {
	if ((efTimeHookOps->enable & 0x1) == 0) {/*由生效变为不生效*/
	    list_for_each_entry_safe(ops,next,&efTimeHookOps->askerHookHead,list){
		if (ops->fn != NULL) {
		    ops->fn(efTimeHookOps->enable == 0x7,ops->askerData);
		}
	    }
	}
    }
    write_unlock(&efTimeHookOps->lock);
    return 0;
}

/********************************************************************
* 函数： uttEffectiveTimeHourChg
* 功能： 生效时间配置小时变化执行函数
* 创建： 2014-10-15
* 参数： @timeNow-当前时间；@conf-生效时间配置
* 返回： 1->失败；0->成功
* 作者： zhao.chong
 ********************************************************************/
static int uttEffectiveTimeHourChg (const UttDateTime *timeNow, void *conf)
{
    UttTimeHookOps *efTimeHookOps = (UttTimeHookOps*)conf;
    uint16_t oldEnable;
    unsigned char flag = 0U;
    UttTimeAskerHookOps *ops,*next;

    EFFECTIVE_TIME_PRINT("func:%s--line:%d\n",__func__,__LINE__);
    if (timeNow == NULL) {
	EFFECTIVE_TIME_PRINT("func:%s--line:%d\n",__func__,__LINE__);
	uttUnRegMinChgHook(conf,uttEffectiveTimeMinuteChg);
	return 0;
    }
    if (efTimeHookOps != NULL) {
	write_lock(&efTimeHookOps->lock);
	oldEnable = efTimeHookOps->enable;
	if (timeNow->week & efTimeHookOps->effectiveTime.wday) {/*当天*/
	    /*只有在某一时间段的两端的整点才注册分钟变化钩子*/
	    if ((timeNow->time/MINUTE_OF_HOUR == efTimeHookOps->effectiveTime.timestart/SECONDS_OF_HOUR) ||
		    (timeNow->time/MINUTE_OF_HOUR == efTimeHookOps->effectiveTime.timestop/SECONDS_OF_HOUR)) {
		flag |= 0x1;/*当前时间的小时处在生效时间配置中的整点*/
	    } else {
		/**
		 *如果起始时间小于结束时间，那么时间计算不会跨天
		 */
		if ((efTimeHookOps->effectiveTime.timestart < efTimeHookOps->effectiveTime.timestop)) {
		    if ((timeNow->time/MINUTE_OF_HOUR < efTimeHookOps->effectiveTime.timestart/SECONDS_OF_HOUR) ||
			(timeNow->time/MINUTE_OF_HOUR > efTimeHookOps->effectiveTime.timestop/SECONDS_OF_HOUR)) {
			flag |= 0x1<<2;/*当前时间不在生效时间配置内*/
		    } else {
			flag |= 0x1<<1;/*当前时间处在配置的生效时间内*/
		    }
		} else {/*起始时间不小于结束时间,例如从12:00-7:00,出现跨天情况*/
		    if ((timeNow->time/MINUTE_OF_HOUR < efTimeHookOps->effectiveTime.timestart/SECONDS_OF_HOUR) && (timeNow->time/MINUTE_OF_HOUR > efTimeHookOps->effectiveTime.timestop/SECONDS_OF_HOUR)) {
			flag |= 0x1<<2;/*当前时间不在生效时间配置内*/
		    } else {
			flag |= 0x1<<1;/*当前时间处在配置的生效时间内*/
		    }
		}
	    }
	}

	if (flag & 0x1) {/*当前时间的小时处在生效时间配置中的整点*/
	    if ((oldEnable & 0x2) == 0) { /*原时变化不生效*/
		efTimeHookOps->enable |= 0x2;/*小时置为生效*/
	    }
	    write_unlock(&efTimeHookOps->lock);
	    uttRegMinChgHook(efTimeHookOps,uttEffectiveTimeMinuteChg);/*注册分钟变化钩子*/
	    write_lock(&efTimeHookOps->lock);
	} else {
	    if (flag & 0x2) {/*生效时间配置的整点之间*/
		efTimeHookOps->enable |= 0x2;/*时置为生效*/
	    } else {/*当前时间不在时间组的时间段内*/
		efTimeHookOps->enable &= ~0x2;/*时置为失效*/
	    }

	    if ((oldEnable & 0x2) != 0) { /*原时变化生效,分变化链表上存在该时间组会注销*/
		/*不管当前时间是在配置的生效时间的整点间，还是时间范围之外,均应注销*/
		uttUnRegMinChgHook(efTimeHookOps,uttEffectiveTimeMinuteChg);

		if (((efTimeHookOps->enable & 0x2) == 0) && ((efTimeHookOps->enable & 0x1) == 0x1)) {/*小时由生效变为不生效,且分钟bit位还生效*/
		    efTimeHookOps->enable &= ~0x1;/*分钟有效位置0，并且执行引用该时间组的模块*/
		    list_for_each_entry_safe(ops,next,&efTimeHookOps->askerHookHead,list){
			if (ops->fn != NULL) {
			    ops->fn(efTimeHookOps->enable == 0x7,ops->askerData);
			}
		    }
		}
	    } else {
		if ((efTimeHookOps->enable & 0x2) != 0) {/*时由不生效到生效(首次挂载时)*/
		    write_unlock(&efTimeHookOps->lock);
		    uttRegMinChgHook(efTimeHookOps,uttEffectiveTimeMinuteChg);/*注册分钟变化钩子*/
		    write_lock(&efTimeHookOps->lock);
		}
	    }
	}
	write_unlock(&efTimeHookOps->lock);
    }

    return 0;
}

/********************************************************************
* 函数： uttEffectiveTimeDayChg
* 功能： 生效时间配置天变化执行函数
* 创建： 2014-10-15
* 参数： @timeNow-当前时间；@conf-时间组
* 返回： 1->失败；0->成功
* 作者： zhao.chong
 ********************************************************************/
static int uttEffectiveTimeDayChg (const UttDateTime *timeNow, void *conf)
{
    UttTimeHookOps *efTimeHookOps = (UttTimeHookOps*)conf;
    uint16_t oldWeekEnable; 

    EFFECTIVE_TIME_PRINT("func:%s--line:%d\n",__func__,__LINE__);
    if (timeNow != NULL) {
	write_lock(&efTimeHookOps->lock);
	oldWeekEnable = efTimeHookOps->enable & 0x4;

	EFFECTIVE_TIME_PRINT("func:%s--line:%d--efTimeHookOps->effectiveTime.wday=%lx,timeNow->week=%x---\n",__func__,__LINE__,efTimeHookOps->effectiveTime.wday,timeNow->week);
	if (efTimeHookOps->effectiveTime.wday & timeNow->week) {
	    efTimeHookOps->enable |= 0x4;/*天置为生效*/
	} else {
	    efTimeHookOps->enable &= ~0x4;
	}

	EFFECTIVE_TIME_PRINT("func:%s--line:%d,oldWeekEnable=%d,efTimeHookOps->enable=%x---\n",__func__,__LINE__,oldWeekEnable,efTimeHookOps->enable);
	if ((oldWeekEnable == 0) && ((efTimeHookOps->enable & 0x4) > 0)) {/*天由不生效变为生效*/
	    write_unlock(&efTimeHookOps->lock);
	    uttRegHourChgHook(efTimeHookOps,uttEffectiveTimeHourChg);/*注册小时变化钩子*/
	    write_lock(&efTimeHookOps->lock);
	} else if(oldWeekEnable > 0 &&  ((efTimeHookOps->enable & 0x4) == 0)) {/*天由生效变为不生效*/
	    uttUnRegHourChgHook(efTimeHookOps,uttEffectiveTimeHourChg);/*注销小时变化的钩子*/
	}
	write_unlock(&efTimeHookOps->lock);
    } else {/*此处为注销时调用，需把小时相关的剔除掉*/
	EFFECTIVE_TIME_PRINT("func:%s--line:%d\n",__func__,__LINE__);
	uttUnRegHourChgHook(efTimeHookOps,uttEffectiveTimeHourChg);/*注销小时变化的钩子*/
    }

    return 0;
}

/********************************************************************
* 函数： uttRegEffectiveTimeToDTChg
* 功能： 注册生效时间配置到时间变化结构的函数
* 创建： 2014-10-15
* 参数： 生效时间配置
* 返回： 0->注册成功；非0->注册失败
* 作者： zhao.chong
 ********************************************************************/
static int uttRegEffectiveTimeToDTChg (void *efTimeHookOpsConf)
{
    UttTimeHookOps *efTimeHookOps = (UttTimeHookOps *)efTimeHookOpsConf;
    int ret = 1;

    EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);
    if (efTimeHookOps != NULL) {
	ret = uttRegDayChgHook(efTimeHookOps,uttEffectiveTimeDayChg);
	if (ret == 0) {
	    write_lock(&efTimeHookOps->lock);
	    efTimeHookOps->isOnTimeChangeList = true;
	    write_unlock(&efTimeHookOps->lock);
	    EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d,%p reg to time chang list ok\n",__FILE__,__func__,__LINE__,efTimeHookOps);
	}
    }
    return ret;
}

/********************************************************************
* 函数： uttUnRegEffectiveTimeFromDTChg
* 功能： 注销生效时间配置在时间变化结构的函数
* 创建： 2014-10-15
* 参数： 时间组
* 返回： 无
* 作者： zhao.chong
 ********************************************************************/
static void uttUnRegEffectiveTimeFromDTChg (UttTimeHookOps *efTimeHookOps)
{
    /*day注销时会自动注销掉小时和分钟*/
    uttUnRegDayChgHook(efTimeHookOps,uttEffectiveTimeDayChg);
}
/**
 *
 *判断给定的生效时间是否已经存在
 *
 */
static UttTimeHookOps* isEffectiveTimeExist(UttEffectiveTime  *effectiveTime)
{
    UttTimeHookOps *pos = NULL,*n;
    bool exist = false;

    if (effectiveTime != NULL) {
	if (!list_empty_careful(&uttTimeHookHead)) {
	    list_for_each_entry_safe(pos,n,&uttTimeHookHead,list){
		EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d,wday=%lu,timestart=%lu,timestop=%lu-----\n",__FILE__,__func__,__LINE__,pos->effectiveTime.wday,pos->effectiveTime.timestart,pos->effectiveTime.timestop);
		if (memcmp(&pos->effectiveTime,effectiveTime,sizeof(UttEffectiveTime)) == 0) {/*该生效时间已经存在*/
		    exist = true;
		    break;
		}
	    }
	}
    }

    return exist ? pos : NULL;
}

/**
 *将给定的生效时间结构挂载到全局链表上
 *
 * return：1-失败；0-成功
 */
static int uttRegTimeHook (UttTimeHookOps  *timeHook)
{
    if ((timeHook == NULL) || isEffectiveTimeExist(&timeHook->effectiveTime)) {
	return 1;
    }
    list_add_tail(&timeHook->list,&uttTimeHookHead);

    return 0;
}

/**
 *将给定的生效时间结构挂载到全局链表上
 *return: 1-失败；0-成功
 */
static int uttUnRegTimeHook (UttTimeHookOps  *timeHook)
{
    int ret = 1;

    if ((timeHook != NULL) && isEffectiveTimeExist(&timeHook->effectiveTime)) {
	list_del_init(&timeHook->list);
	ret = 0;
    }

    return ret;
}

/**
 *判断该用户模块是否已经挂载到该生效时间的用户模块链表中了
 *
 */
static bool isUserConfExist(UttTimeHookOps *timeHookOps, void *conf)
{
    bool exist = false;
    UttTimeAskerHookOps *pos;

    EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);
    if (timeHookOps != NULL) {
	write_lock_bh(&timeHookOps->lock);
	if (!list_empty(&timeHookOps->askerHookHead)) { 
	    list_for_each_entry(pos,&timeHookOps->askerHookHead,list) {
		if (pos->askerData == conf) {
		    exist = true;
		    break;
		}
	    }
	}
	if ((!exist) && (!list_empty(&timeHookOps->delayExecList))) {
	    list_for_each_entry(pos,&timeHookOps->delayExecList,list) {
		if (pos->askerData == conf) {
		    exist = true;
		    break;
		}
	    }
	}
	write_unlock_bh(&timeHookOps->lock);
    }

    EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d, exist=%d\n",__FILE__,__func__,__LINE__,exist);
    return exist;
}

/********************************************************************
 * 函数：uttGetTimeHookOpsRef
 * 功能：根据effectiveTime获取相应的UttTimeHookOps的指针
 * 创建：2015-01-04
 * 参数：@effectiveTime-生效时间配置
 * 返回：成功-返回相应的指针；失败-返回NULL
 * 作者：zhao.chong
 ********************************************************************/
UttTimeHookOps *uttGetTimeHookOpsRef (UttEffectiveTime *effectiveTime)
{
    UttTimeHookOps *timeHookOps = NULL;

    EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);
    if (effectiveTime != NULL) {
	EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d,wday=%lu,timestart=%lu,timestop=%lu-----\n",__FILE__,__func__,__LINE__,effectiveTime->wday,effectiveTime->timestart,effectiveTime->timestop);
	uttEffectiveTimeLock();
	if ((timeHookOps = isEffectiveTimeExist(effectiveTime)) == NULL) {/*该生效时间不存在全局链表中*/
	    EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);
	    timeHookOps = kzalloc(sizeof(UttTimeHookOps),GFP_ATOMIC);
	    if (timeHookOps != NULL) {
		rwlock_init(&timeHookOps->lock);
		INIT_LIST_HEAD(&timeHookOps->askerHookHead);
		INIT_LIST_HEAD(&timeHookOps->delayExecList);
		memcpy(&timeHookOps->effectiveTime,effectiveTime,sizeof(UttEffectiveTime));
		EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);
		atomic_set(&timeHookOps->ref,1);/*该生效时间hook第一次被引用*/
		atomic_set(&timeHookOps->onTimeRef,0);
		if (uttRegTimeHook(timeHookOps) == 1) {/*将当前生效时间挂载到全局链表失败*/
		    kfree(timeHookOps);
		    EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d,uttRegTimeHook failed\n",__FILE__,__func__,__LINE__);
		    timeHookOps = NULL;
		}
	    }

	} else {
	    EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);
	    atomic_inc(&timeHookOps->ref);
	}
	if (timeHookOps != NULL) {
	    EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d--timeHook is refereced by %d times--------\n",__FILE__,__func__,__LINE__,atomic_read(&timeHookOps->ref));
	}
	uttEffectiveTimeUnlock();
    }
	    
    return timeHookOps;
}
EXPORT_SYMBOL(uttGetTimeHookOpsRef);

/********************************************************************
 * 函数：uttPutTimeHookOpsRef
 * 功能：将对UttTimeHookOps指针的引用释放
 * 创建：2015-01-04
 * 参数：@effectiveTime-生效时间配置
 * 返回：成功-返回相应的指针；失败-返回NULL
 * 作者：zhao.chong
 ********************************************************************/
void uttPutTimeHookOpsRef (UttTimeHookOps *timeHookOps)
{
    EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);
    if ((timeHookOps != NULL)) {
	EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d--timeHook is refereced by %d times--------\n",__FILE__,__func__,__LINE__,atomic_read(&timeHookOps->ref));
	if ((atomic_dec_and_test(&timeHookOps->ref))) {/*该生效时间Hook已经没有被任何模块引用*/
	    uttEffectiveTimeLock();
	    uttUnRegTimeHook(timeHookOps);
	    kfree(timeHookOps);
	    timeHookOps = NULL;
	    uttEffectiveTimeUnlock();
	}
    }
}
EXPORT_SYMBOL(uttPutTimeHookOpsRef);

/********************************************************************
 * 函数：uttRegToEffTimeHook
 * 功能：将应用模块配置添加到timeHook生效时间上
 * 创建：2015-01-08
 * 参数：@timeHook-生效时间配置Hook
 *	 @conf-引用该生效时间的配置
 *	 @fn-时间生效或失效时执行的函数
 * 返回：@isErr-表示是否注册失败，返回true表示注册失败；false表示注册成功
 * 作者：zhao.chong
 ********************************************************************/
bool uttRegToEffTimeHook (UttTimeHookOps *timeHook, void *conf, uttTimeSetAskerFn *fn)
{
    UttTimeAskerHookOps *timeAskerHookOps;
    bool isErr = false;

    EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);
    if (!isUserConfExist(timeHook,conf)) {/*判断该用户模块不存在timeHook上*/
	/*创建新的用户模块hook*/
	timeAskerHookOps = kzalloc(sizeof(UttTimeAskerHookOps),GFP_ATOMIC);
	if (timeAskerHookOps == NULL) {
	    EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d,kzalloc failed\n",__FILE__,__func__,__LINE__);
	    isErr = true;
	    return isErr;
	}
	timeAskerHookOps->askerData = conf;
	timeAskerHookOps->fn = fn;
	write_lock_bh(&timeHook->lock);
	/*判断该timeHook是否已经在*/
	if (timeHook->isOnTimeChangeList) {/*该生效时间timeHook已经存在时间变化链表中*/
	    if (timeHook->enable == 0x7) {/*如果此时生效，将用户配置加入到延迟执行链表中*/
		EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);
		list_add_tail(&timeAskerHookOps->list,&timeHook->delayExecList);
	    } else {/*如果此时不生效直接挂在到askerHookHead上等待时间生效执行*/
		EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);
		list_add_tail(&timeAskerHookOps->list,&timeHook->askerHookHead);
	    }
	} else {
	    EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);
	    list_add_tail(&timeAskerHookOps->list,&timeHook->askerHookHead);
	    if ((isErr = addToTimeSetWaitList(timeHook,uttRegEffectiveTimeToDTChg)) == true) {/*加入等待列表失败*/
		EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);
		list_del(&timeAskerHookOps->list);
		kfree(timeAskerHookOps);
	    }
	}
#if 1
	if (!isErr) {
	    atomic_inc(&timeHook->onTimeRef);
	    EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d,timeHook->onTimeRef = %d\n",__FILE__,__func__,__LINE__,atomic_read(&timeHook->onTimeRef));
	    //atomic_inc(&timeHook->ref);
	}
#endif
	write_unlock_bh(&timeHook->lock);
    }

    return isErr;
}

/**
 *
 *将用户配置从给定的时间配置的链表中删除
 *
 *return 1-失败；0-成功
 */
int uttUnRegFromTimeHook (UttTimeHookOps *timeHookOps, void *conf)
{
    UttTimeAskerHookOps *pos,*n;

    if (timeHookOps == NULL) {
	return 1;
    }
    uttEffectiveTimeLock();
    EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
    write_lock(&timeHookOps->lock);
    if (!list_empty_careful(&timeHookOps->askerHookHead)) {
	EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
	list_for_each_entry_safe(pos,n,&timeHookOps->askerHookHead,list) {
	    if (pos->askerData == conf) {
		EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
		list_del_init(&pos->list);
		if (timeHookOps->enable == 0x7) {
		    pos->fn(0,conf);
		}
		kfree(pos);
		pos = NULL;
		break;
	    }
	}
    }
    EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
    if (!list_empty(&timeHookOps->delayExecList)) {
	list_for_each_entry_safe(pos,n,&timeHookOps->delayExecList,list) {
	    if (pos->askerData == conf) {
		list_del_init(&pos->list);
		kfree(pos);
		pos = NULL;
		break;
	    }
	}
    }
    if ((atomic_dec_and_test(&timeHookOps->onTimeRef))) {/*该生效时间Hook已经没有被任何模块引用*/
	if (timeHookOps->isOnTimeChangeList) {/*生效时间Hook在时间变化链表上*/
	    /*将此时间配置结构从时间变化链表卸载*/
	    uttUnRegEffectiveTimeFromDTChg(timeHookOps);
	    timeHookOps->isOnTimeChangeList = false;
	    timeHookOps->enable = 0;
	} else {
	    /*时间Hook如果在等待链表上，则从等待链表中删除*/
	    delFromWaitList(timeHookOps);
	}
    }
    write_unlock(&timeHookOps->lock);
    uttEffectiveTimeUnlock();
    return 0;
}
EXPORT_SYMBOL(uttUnRegFromTimeHook);
