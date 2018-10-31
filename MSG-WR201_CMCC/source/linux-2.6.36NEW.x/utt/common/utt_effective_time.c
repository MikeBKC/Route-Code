#include <linux/ip.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/jiffies.h>/*������jiffies*/
#include <asm/param.h>/*������HZ*/
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

/****************ȫ�ֱ���*******start********************/
static DEFINE_SPINLOCK(utt_effective_time_spinlock);
static LIST_HEAD(uttTimeHookHead);/*���е���Чʱ�������������*/
/****************ȫ�ֱ���*******end********************/


/****************��������*******start******************/
static void uttEffectiveTimeLock(void);
static void uttEffectiveTimeUnlock(void);
static UttTimeHookOps* isEffectiveTimeExist(UttEffectiveTime  *effectiveTime);
static bool isUserConfExist(UttTimeHookOps *timeHookOps, void *conf);
static int uttRegTimeHook (UttTimeHookOps  *timeHook);
static int uttUnRegTimeHook (UttTimeHookOps  *timeHook);
static int uttRegEffectiveTimeToDTChg (void *efTimeHookOpsConf);
static void uttUnRegEffectiveTimeFromDTChg (UttTimeHookOps *efTimeHookOps);
/****************��������*******end******************/

static void uttEffectiveTimeLock(void)
{
    spin_lock_bh(&utt_effective_time_spinlock);
}

static void uttEffectiveTimeUnlock(void)
{
    spin_unlock_bh(&utt_effective_time_spinlock);
}

/********************************************************************
* ������ uttEffectiveTimeMinuteChg
* ���ܣ� ʱ������ӱ仯ִ�к���
* ������ 2014-10-15
* ������ @timeNow-��ǰʱ�䣻@conf-��Чʱ������
* ���أ� 0->�ɹ�����0->ʧ��
* ���ߣ� zhao.chong
 ********************************************************************/
static int uttEffectiveTimeMinuteChg (const UttDateTime *timeNow, void *conf)
{
    UttTimeHookOps *efTimeHookOps = (UttTimeHookOps*)conf;
    uint16_t flag = 1U;/*������Ч��־��Ĭ����Ч*/
    uint16_t oldEnable;
    UttTimeAskerHookOps *ops,*next;

    EFFECTIVE_TIME_PRINT("func:%s--line:%d\n",__func__,__LINE__);
    if ((timeNow == NULL) || (efTimeHookOps == NULL)) {/*����ʱ��ӷ��ӱ仯������ж��ʱ*/
	EFFECTIVE_TIME_PRINT("func:%s--line:%d\n",__func__,__LINE__);
	return 1;
    }
    write_lock(&efTimeHookOps->lock);
    oldEnable = efTimeHookOps->enable & 0x1;
    if (oldEnable == 0x1) {
	if (!list_empty_careful(&efTimeHookOps->delayExecList)) {/*��ʱ������ڴ�����ִ�е�ģ��*/ 
	    list_for_each_entry_safe(ops,next,&efTimeHookOps->delayExecList,list){
		if (ops->fn != NULL) {
		    ops->fn(1,ops->askerData);
		}
		list_del(&ops->list);
		list_add_tail(&ops->list,&efTimeHookOps->askerHookHead);
	    }
	}
    }

    if (timeNow->week & efTimeHookOps->effectiveTime.wday) {/*����*/
	/**
	 *���ж�ʱ���Ƿ����˿�������
	 */
	if (efTimeHookOps->effectiveTime.timestart < efTimeHookOps->effectiveTime.timestop) {/*������*/
	    if ((timeNow->time < efTimeHookOps->effectiveTime.timestart/SECONDS_OF_MINUTE) || (timeNow->time > efTimeHookOps->effectiveTime.timestop/SECONDS_OF_MINUTE)) {
		flag = 0U;/*���Ӳ���Ч*/
	    }
	} else {
	    if ((timeNow->time < efTimeHookOps->effectiveTime.timestart/SECONDS_OF_MINUTE) && (timeNow->time > efTimeHookOps->effectiveTime.timestop/SECONDS_OF_MINUTE)) {
		flag = 0U;/*���Ӳ���Ч*/
	    }
	}
    } else {
	flag = 0U;
    }

    if (flag == 1U) {
	efTimeHookOps->enable |= 0x1;/*����Ч��λ*/
    } else {
	efTimeHookOps->enable &= ~0x1;/*����Ч����*/
    }

    EFFECTIVE_TIME_PRINT("func:%s--line:%d,oldEnable=%d,efTimeHookOps->enable & 0x1=%d\n",__func__,__LINE__,oldEnable,(efTimeHookOps->enable & 0x1));
    if (oldEnable == 0) {
	if ((efTimeHookOps->enable & 0x1) == 0x1) {/*�ɲ���Ч��Ϊ��Ч*/
	    list_for_each_entry_safe(ops,next,&efTimeHookOps->askerHookHead,list){
		if (ops->fn != NULL) {
		    ops->fn(efTimeHookOps->enable == 0x7,ops->askerData);
		}
	    }
	}
    } else {
	if ((efTimeHookOps->enable & 0x1) == 0) {/*����Ч��Ϊ����Ч*/
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
* ������ uttEffectiveTimeHourChg
* ���ܣ� ��Чʱ������Сʱ�仯ִ�к���
* ������ 2014-10-15
* ������ @timeNow-��ǰʱ�䣻@conf-��Чʱ������
* ���أ� 1->ʧ�ܣ�0->�ɹ�
* ���ߣ� zhao.chong
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
	if (timeNow->week & efTimeHookOps->effectiveTime.wday) {/*����*/
	    /*ֻ����ĳһʱ��ε����˵������ע����ӱ仯����*/
	    if ((timeNow->time/MINUTE_OF_HOUR == efTimeHookOps->effectiveTime.timestart/SECONDS_OF_HOUR) ||
		    (timeNow->time/MINUTE_OF_HOUR == efTimeHookOps->effectiveTime.timestop/SECONDS_OF_HOUR)) {
		flag |= 0x1;/*��ǰʱ���Сʱ������Чʱ�������е�����*/
	    } else {
		/**
		 *�����ʼʱ��С�ڽ���ʱ�䣬��ôʱ����㲻�����
		 */
		if ((efTimeHookOps->effectiveTime.timestart < efTimeHookOps->effectiveTime.timestop)) {
		    if ((timeNow->time/MINUTE_OF_HOUR < efTimeHookOps->effectiveTime.timestart/SECONDS_OF_HOUR) ||
			(timeNow->time/MINUTE_OF_HOUR > efTimeHookOps->effectiveTime.timestop/SECONDS_OF_HOUR)) {
			flag |= 0x1<<2;/*��ǰʱ�䲻����Чʱ��������*/
		    } else {
			flag |= 0x1<<1;/*��ǰʱ�䴦�����õ���Чʱ����*/
		    }
		} else {/*��ʼʱ�䲻С�ڽ���ʱ��,�����12:00-7:00,���ֿ������*/
		    if ((timeNow->time/MINUTE_OF_HOUR < efTimeHookOps->effectiveTime.timestart/SECONDS_OF_HOUR) && (timeNow->time/MINUTE_OF_HOUR > efTimeHookOps->effectiveTime.timestop/SECONDS_OF_HOUR)) {
			flag |= 0x1<<2;/*��ǰʱ�䲻����Чʱ��������*/
		    } else {
			flag |= 0x1<<1;/*��ǰʱ�䴦�����õ���Чʱ����*/
		    }
		}
	    }
	}

	if (flag & 0x1) {/*��ǰʱ���Сʱ������Чʱ�������е�����*/
	    if ((oldEnable & 0x2) == 0) { /*ԭʱ�仯����Ч*/
		efTimeHookOps->enable |= 0x2;/*Сʱ��Ϊ��Ч*/
	    }
	    write_unlock(&efTimeHookOps->lock);
	    uttRegMinChgHook(efTimeHookOps,uttEffectiveTimeMinuteChg);/*ע����ӱ仯����*/
	    write_lock(&efTimeHookOps->lock);
	} else {
	    if (flag & 0x2) {/*��Чʱ�����õ�����֮��*/
		efTimeHookOps->enable |= 0x2;/*ʱ��Ϊ��Ч*/
	    } else {/*��ǰʱ�䲻��ʱ�����ʱ�����*/
		efTimeHookOps->enable &= ~0x2;/*ʱ��ΪʧЧ*/
	    }

	    if ((oldEnable & 0x2) != 0) { /*ԭʱ�仯��Ч,�ֱ仯�����ϴ��ڸ�ʱ�����ע��*/
		/*���ܵ�ǰʱ���������õ���Чʱ�������䣬����ʱ�䷶Χ֮��,��Ӧע��*/
		uttUnRegMinChgHook(efTimeHookOps,uttEffectiveTimeMinuteChg);

		if (((efTimeHookOps->enable & 0x2) == 0) && ((efTimeHookOps->enable & 0x1) == 0x1)) {/*Сʱ����Ч��Ϊ����Ч,�ҷ���bitλ����Ч*/
		    efTimeHookOps->enable &= ~0x1;/*������Чλ��0������ִ�����ø�ʱ�����ģ��*/
		    list_for_each_entry_safe(ops,next,&efTimeHookOps->askerHookHead,list){
			if (ops->fn != NULL) {
			    ops->fn(efTimeHookOps->enable == 0x7,ops->askerData);
			}
		    }
		}
	    } else {
		if ((efTimeHookOps->enable & 0x2) != 0) {/*ʱ�ɲ���Ч����Ч(�״ι���ʱ)*/
		    write_unlock(&efTimeHookOps->lock);
		    uttRegMinChgHook(efTimeHookOps,uttEffectiveTimeMinuteChg);/*ע����ӱ仯����*/
		    write_lock(&efTimeHookOps->lock);
		}
	    }
	}
	write_unlock(&efTimeHookOps->lock);
    }

    return 0;
}

/********************************************************************
* ������ uttEffectiveTimeDayChg
* ���ܣ� ��Чʱ��������仯ִ�к���
* ������ 2014-10-15
* ������ @timeNow-��ǰʱ�䣻@conf-ʱ����
* ���أ� 1->ʧ�ܣ�0->�ɹ�
* ���ߣ� zhao.chong
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
	    efTimeHookOps->enable |= 0x4;/*����Ϊ��Ч*/
	} else {
	    efTimeHookOps->enable &= ~0x4;
	}

	EFFECTIVE_TIME_PRINT("func:%s--line:%d,oldWeekEnable=%d,efTimeHookOps->enable=%x---\n",__func__,__LINE__,oldWeekEnable,efTimeHookOps->enable);
	if ((oldWeekEnable == 0) && ((efTimeHookOps->enable & 0x4) > 0)) {/*���ɲ���Ч��Ϊ��Ч*/
	    write_unlock(&efTimeHookOps->lock);
	    uttRegHourChgHook(efTimeHookOps,uttEffectiveTimeHourChg);/*ע��Сʱ�仯����*/
	    write_lock(&efTimeHookOps->lock);
	} else if(oldWeekEnable > 0 &&  ((efTimeHookOps->enable & 0x4) == 0)) {/*������Ч��Ϊ����Ч*/
	    uttUnRegHourChgHook(efTimeHookOps,uttEffectiveTimeHourChg);/*ע��Сʱ�仯�Ĺ���*/
	}
	write_unlock(&efTimeHookOps->lock);
    } else {/*�˴�Ϊע��ʱ���ã����Сʱ��ص��޳���*/
	EFFECTIVE_TIME_PRINT("func:%s--line:%d\n",__func__,__LINE__);
	uttUnRegHourChgHook(efTimeHookOps,uttEffectiveTimeHourChg);/*ע��Сʱ�仯�Ĺ���*/
    }

    return 0;
}

/********************************************************************
* ������ uttRegEffectiveTimeToDTChg
* ���ܣ� ע����Чʱ�����õ�ʱ��仯�ṹ�ĺ���
* ������ 2014-10-15
* ������ ��Чʱ������
* ���أ� 0->ע��ɹ�����0->ע��ʧ��
* ���ߣ� zhao.chong
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
* ������ uttUnRegEffectiveTimeFromDTChg
* ���ܣ� ע����Чʱ��������ʱ��仯�ṹ�ĺ���
* ������ 2014-10-15
* ������ ʱ����
* ���أ� ��
* ���ߣ� zhao.chong
 ********************************************************************/
static void uttUnRegEffectiveTimeFromDTChg (UttTimeHookOps *efTimeHookOps)
{
    /*dayע��ʱ���Զ�ע����Сʱ�ͷ���*/
    uttUnRegDayChgHook(efTimeHookOps,uttEffectiveTimeDayChg);
}
/**
 *
 *�жϸ�������Чʱ���Ƿ��Ѿ�����
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
		if (memcmp(&pos->effectiveTime,effectiveTime,sizeof(UttEffectiveTime)) == 0) {/*����Чʱ���Ѿ�����*/
		    exist = true;
		    break;
		}
	    }
	}
    }

    return exist ? pos : NULL;
}

/**
 *����������Чʱ��ṹ���ص�ȫ��������
 *
 * return��1-ʧ�ܣ�0-�ɹ�
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
 *����������Чʱ��ṹ���ص�ȫ��������
 *return: 1-ʧ�ܣ�0-�ɹ�
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
 *�жϸ��û�ģ���Ƿ��Ѿ����ص�����Чʱ����û�ģ����������
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
 * ������uttGetTimeHookOpsRef
 * ���ܣ�����effectiveTime��ȡ��Ӧ��UttTimeHookOps��ָ��
 * ������2015-01-04
 * ������@effectiveTime-��Чʱ������
 * ���أ��ɹ�-������Ӧ��ָ�룻ʧ��-����NULL
 * ���ߣ�zhao.chong
 ********************************************************************/
UttTimeHookOps *uttGetTimeHookOpsRef (UttEffectiveTime *effectiveTime)
{
    UttTimeHookOps *timeHookOps = NULL;

    EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);
    if (effectiveTime != NULL) {
	EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d,wday=%lu,timestart=%lu,timestop=%lu-----\n",__FILE__,__func__,__LINE__,effectiveTime->wday,effectiveTime->timestart,effectiveTime->timestop);
	uttEffectiveTimeLock();
	if ((timeHookOps = isEffectiveTimeExist(effectiveTime)) == NULL) {/*����Чʱ�䲻����ȫ��������*/
	    EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);
	    timeHookOps = kzalloc(sizeof(UttTimeHookOps),GFP_ATOMIC);
	    if (timeHookOps != NULL) {
		rwlock_init(&timeHookOps->lock);
		INIT_LIST_HEAD(&timeHookOps->askerHookHead);
		INIT_LIST_HEAD(&timeHookOps->delayExecList);
		memcpy(&timeHookOps->effectiveTime,effectiveTime,sizeof(UttEffectiveTime));
		EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);
		atomic_set(&timeHookOps->ref,1);/*����Чʱ��hook��һ�α�����*/
		atomic_set(&timeHookOps->onTimeRef,0);
		if (uttRegTimeHook(timeHookOps) == 1) {/*����ǰ��Чʱ����ص�ȫ������ʧ��*/
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
 * ������uttPutTimeHookOpsRef
 * ���ܣ�����UttTimeHookOpsָ��������ͷ�
 * ������2015-01-04
 * ������@effectiveTime-��Чʱ������
 * ���أ��ɹ�-������Ӧ��ָ�룻ʧ��-����NULL
 * ���ߣ�zhao.chong
 ********************************************************************/
void uttPutTimeHookOpsRef (UttTimeHookOps *timeHookOps)
{
    EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);
    if ((timeHookOps != NULL)) {
	EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d--timeHook is refereced by %d times--------\n",__FILE__,__func__,__LINE__,atomic_read(&timeHookOps->ref));
	if ((atomic_dec_and_test(&timeHookOps->ref))) {/*����Чʱ��Hook�Ѿ�û�б��κ�ģ������*/
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
 * ������uttRegToEffTimeHook
 * ���ܣ���Ӧ��ģ��������ӵ�timeHook��Чʱ����
 * ������2015-01-08
 * ������@timeHook-��Чʱ������Hook
 *	 @conf-���ø���Чʱ�������
 *	 @fn-ʱ����Ч��ʧЧʱִ�еĺ���
 * ���أ�@isErr-��ʾ�Ƿ�ע��ʧ�ܣ�����true��ʾע��ʧ�ܣ�false��ʾע��ɹ�
 * ���ߣ�zhao.chong
 ********************************************************************/
bool uttRegToEffTimeHook (UttTimeHookOps *timeHook, void *conf, uttTimeSetAskerFn *fn)
{
    UttTimeAskerHookOps *timeAskerHookOps;
    bool isErr = false;

    EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);
    if (!isUserConfExist(timeHook,conf)) {/*�жϸ��û�ģ�鲻����timeHook��*/
	/*�����µ��û�ģ��hook*/
	timeAskerHookOps = kzalloc(sizeof(UttTimeAskerHookOps),GFP_ATOMIC);
	if (timeAskerHookOps == NULL) {
	    EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d,kzalloc failed\n",__FILE__,__func__,__LINE__);
	    isErr = true;
	    return isErr;
	}
	timeAskerHookOps->askerData = conf;
	timeAskerHookOps->fn = fn;
	write_lock_bh(&timeHook->lock);
	/*�жϸ�timeHook�Ƿ��Ѿ���*/
	if (timeHook->isOnTimeChangeList) {/*����Чʱ��timeHook�Ѿ�����ʱ��仯������*/
	    if (timeHook->enable == 0x7) {/*�����ʱ��Ч�����û����ü��뵽�ӳ�ִ��������*/
		EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);
		list_add_tail(&timeAskerHookOps->list,&timeHook->delayExecList);
	    } else {/*�����ʱ����Чֱ�ӹ��ڵ�askerHookHead�ϵȴ�ʱ����Чִ��*/
		EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);
		list_add_tail(&timeAskerHookOps->list,&timeHook->askerHookHead);
	    }
	} else {
	    EFFECTIVE_TIME_PRINT("----file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);
	    list_add_tail(&timeAskerHookOps->list,&timeHook->askerHookHead);
	    if ((isErr = addToTimeSetWaitList(timeHook,uttRegEffectiveTimeToDTChg)) == true) {/*����ȴ��б�ʧ��*/
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
 *���û����ôӸ�����ʱ�����õ�������ɾ��
 *
 *return 1-ʧ�ܣ�0-�ɹ�
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
    if ((atomic_dec_and_test(&timeHookOps->onTimeRef))) {/*����Чʱ��Hook�Ѿ�û�б��κ�ģ������*/
	if (timeHookOps->isOnTimeChangeList) {/*��Чʱ��Hook��ʱ��仯������*/
	    /*����ʱ�����ýṹ��ʱ��仯����ж��*/
	    uttUnRegEffectiveTimeFromDTChg(timeHookOps);
	    timeHookOps->isOnTimeChangeList = false;
	    timeHookOps->enable = 0;
	} else {
	    /*ʱ��Hook����ڵȴ������ϣ���ӵȴ�������ɾ��*/
	    delFromWaitList(timeHookOps);
	}
    }
    write_unlock(&timeHookOps->lock);
    uttEffectiveTimeUnlock();
    return 0;
}
EXPORT_SYMBOL(uttUnRegFromTimeHook);
