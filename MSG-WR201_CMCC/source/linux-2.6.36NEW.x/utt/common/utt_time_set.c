#include <linux/ip.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/jiffies.h>/*������jiffies*/
#include <asm/param.h>/*������HZ*/
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/netfilter_ipv4/time_set.h>
#include <linux/hardirq.h>

#if 0 
#define TIMEGRP_PRINT(fmt, args...)		printk(fmt, ## args)
#else
#define TIMEGRP_PRINT(fmt, args...)
#endif
MODULE_AUTHOR("zhao.chong");
MODULE_DESCRIPTION("time group");
MODULE_LICENSE("GPL");

/****************ȫ�ֱ���*******start********************/
static DEFINE_SPINLOCK(utt_time_set_app_spinlock);
static DEFINE_SPINLOCK(timeWaitListLock);

static struct timer_list timer_set;/*��ʱ���ṹ�������ļ�������*/
struct list_head uttDayChgHookHead;/*һ���췢���˱仯���ͻ����������������ĺ���ִ��һ��*/
struct list_head uttHourChgHookHead;/*һ��Сʱ�����˱仯���ͻ����������������ĺ���ִ��һ��*/
struct list_head uttMinChgHookHead;/*һ�����ӷ����˱仯���ͻ����������������ĺ���ִ��һ��*/
struct list_head timeWaitList;/*����ʱ�����õĵȴ��б�*/
EXPORT_SYMBOL(uttDayChgHookHead);
EXPORT_SYMBOL(uttHourChgHookHead);
EXPORT_SYMBOL(uttMinChgHookHead);
EXPORT_SYMBOL(timeWaitList);
uint16_t uttTimeSetMax = CONFIG_UTT_TIME_SET_MAX;
static int maxTimeSets;

module_param(maxTimeSets, int, 0600);
MODULE_PARM_DESC(maxTimeSets, "maximal number of time sets");
static UttTimeSet **uttTimeSetList;		/* all individual time sets */
/*����һ��ȫ�ֽṹ�壬����Ӧ�ö�����ʹ�������ʾ��ǰϵͳʱ��*/
static UttDateTime uttDTimeNow;
static UttDateTime uttDTimeOld;

/****************ȫ�ֱ���*******end********************/


/****************��������*******start******************/
const UttDateTime* getUttDTimeNow(void)
{
    return &uttDTimeNow;
}
EXPORT_SYMBOL(getUttDTimeNow);

static void setUttDTimeNow(const UttDateTime *uttDateTime)
{
    uttDTimeNow.date = uttDateTime->date;
    uttDTimeNow.week = uttDateTime->week;
    uttDTimeNow.time = uttDateTime->time;
}

const UttDateTime* getUttDTimeOld(void)
{
    return &uttDTimeOld;
}
EXPORT_SYMBOL(getUttDTimeOld);

static void setUttDTimeOld(const UttDateTime *uttDateTime)
{
    uttDTimeOld.date = uttDateTime->date;
    uttDTimeOld.week = uttDateTime->week;
    uttDTimeOld.time = uttDateTime->time;
}

static void timer_task(unsigned long data);
static void set_current_time(void);
static void timer_open(void);
static int uttTimeSetWeekTimeFlush(UttTimeSet * timeSet);
static int uttTimeSetMinChg (const UttDateTime*timeNow, void *conf);
static int uttTimeSetHourChg (const UttDateTime*timeNow, void *conf);
static int uttTimeSetDayChg (const UttDateTime*timeNow, void *conf);
static void uttTimeSetLock(void);
static void uttTimeSetUnlock(void);
static void uttTimeWaitListLock(void);
static void uttTimeWaitListUnlock(void);
static void traverseTimeWaitList(void);
static int __init time_set_init (void);
static void __exit time_set_exit (void);
/****************��������*******end******************/

static void uttTimeSetLock(void)
{
    spin_lock_bh(&utt_time_set_app_spinlock);
}
static void uttTimeSetUnlock(void)
{
    spin_unlock_bh(&utt_time_set_app_spinlock);
}

static void uttTimeWaitListLock(void)
{
    spin_lock_bh(&timeWaitListLock);
}

static void uttTimeWaitListUnlock(void)
{
    spin_unlock_bh(&timeWaitListLock);
}

/********************************************************************
* ������ traverseTimeWaitList
* ���ܣ� �����ȴ������������ϵ�ʱ�����ù��ص�ʱ��仯������
* ������ 2015-01-04
* ������ ��
* ���أ� ��
* ���ߣ� zhao.chong
* ˵��:	 �ú�����ûʮ����ִ��һ�Σ�Ϊ��������ʱ�����õ��û�ģ����ִ�У��˺���
*	 Ӧ���ڱ�����ʱ��仯��������
 ********************************************************************/
static void traverseTimeWaitList(void)
{
    UttTimeWaitNode *pos,*n;
    int count = 0;

    TIMEGRP_PRINT("------file:%s,func:%s,Line %d---\n",__FILE__,__func__,__LINE__);
    uttTimeWaitListLock();
    if (!list_empty_careful(&timeWaitList)) {
	list_for_each_entry_safe(pos,n,&timeWaitList,waitNode){/*����ʱ�䵥Ԫ*/
	    TIMEGRP_PRINT("------file:%s,func:%s,Line %d-the %d conf--\n",__FILE__,__func__,__LINE__,++count);
	    if (pos->fn != NULL) {
		pos->fn(pos->conf);
	    }
	    list_del(&pos->waitNode);
	}
    }
    
    uttTimeWaitListUnlock();
}

/********************************************************************
* ������ timer_task
* ���ܣ� ��ʱ����ִ�к���
* ������ 2014-09-11
* ������ ������ʱ��ʱ��ʱ��ִ�к������ݵĲ���
* ���أ� ��
* ���ߣ� zhao.chong
* ˵��:	 ���û��ֶ�����ϵͳʱ��Ϊ��ȥ��ʱ�䣬�˶�ʱ������ܻ����һ���ӵ���ʱִ��
 ********************************************************************/
static void timer_task(unsigned long data)
{
    const UttDateTime *uttDateTimeNow,*uttDateTimeOld;
    UttDateTimeChgHookOps *pos,*n;

    set_current_time();/*���õ�ǰʱ��*/
    uttDateTimeNow = getUttDTimeNow();/*��ȡ��ǰʱ��*/
    uttDateTimeOld = getUttDTimeOld();/*��ȡ��һ��ʱ��ڵ�*/

    /**
     *
     * ����3��ʱ�����͵ı仯:��仯��ʱ�仯���ֱ仯
     */
    if ((uttDateTimeNow->time != uttDateTimeOld->time) || ((uttDateTimeNow->date != uttDateTimeOld->date))) {/*�ַ����仯*/
	TIMEGRP_PRINT("minute is changed\n");
	/**
	 *���� uttMinChgHookHead
	 */
	if (!list_empty(&uttMinChgHookHead)) {
	    list_for_each_entry_safe(pos,n,&uttMinChgHookHead,list){
		pos->fn(uttDateTimeNow,pos->askerData);
	    }
	}

	/**
	 *Сʱ�����˱仯
	 *
	 * ע�⣺������ֶ�����ϵͳʱ�����ʱ�ӷ����������⣬�������ڳ����⣬ҲҪִ��
	 */
	if ((uttDateTimeNow->time % 60 == 0) || ((uttDateTimeNow->date != uttDateTimeOld->date) || (uttDateTimeNow->time/60 != uttDateTimeOld->time/60))) {
	    TIMEGRP_PRINT("hour is changed\n");
	    /**
	     *���� uttHourChgHookHead
	     */
	    if (!list_empty(&uttHourChgHookHead)) {
		list_for_each_entry_safe(pos,n,&uttHourChgHookHead,list){
		    pos->fn(uttDateTimeNow,pos->askerData);
		}
	    }

	    if (uttDateTimeNow->date != uttDateTimeOld->date) {/*�췢���˱仯*/
		TIMEGRP_PRINT("week/day is changed\n");
		/**
		 *���� uttDayChgHookHead
		 */
		if (!list_empty(&uttDayChgHookHead)) {
		    list_for_each_entry_safe(pos,n,&uttDayChgHookHead,list){
			pos->fn(uttDateTimeNow,pos->askerData);
		    }
		}

	    }
	}

	/*����ǰʱ��ڵ�����ΪoldTime*/
	setUttDTimeOld(uttDateTimeNow);
    }
    traverseTimeWaitList();

    timer_open();/*ÿ��һ��ʱ�����¿�����ʱ��*/
}

/********************************************************************
* ������ set_current_time
* ���ܣ� ��ȡ��ǰϵͳʱ�䣬��������uttDTimeNow������
* ������ 2014-09-11
* ������ ��
* ���أ� ��
* ���ߣ� zhao.chong
 ********************************************************************/
static void set_current_time(void)
{
    unsigned long curseconds;
    int index;
    UttDateTime uttDateTime;
    memset(&uttDateTime,0,sizeof(uttDateTime));

    curseconds = get_seconds();
    uttDateTime.date = curseconds / SECONDS_OF_DAY;/*������*/
    uttDateTime.time = (curseconds % SECONDS_OF_DAY) / SECONDS_OF_MINUTE;/*����ʱ��,һ���д�0�㿪ʼ�����ھ����˷�����*/
    index =  ((curseconds/SECONDS_OF_DAY + 3) % 7u);/*1970-1-1��������*/
    TIMEGRP_PRINT("func:%s---index=%d\n",__func__,index);
    uttDateTime.week = 1<<(6-index);/*��������*/
    TIMEGRP_PRINT("func:%s---curseconds=%lu\n",__func__,curseconds);
    TIMEGRP_PRINT("func:%s---date=%u\n",__func__,uttDateTime.date);
    TIMEGRP_PRINT("func:%s---time=%u\n",__func__,uttDateTime.time);
    TIMEGRP_PRINT("func:%s---week=%u\n",__func__,uttDateTime.week);

    setUttDTimeNow(&uttDateTime);/*���õ�ǰʱ��*/
}
/********************************************************************
* ������ timer_open
* ���ܣ� ����һ����ʱ��
* ������ 2014-09-11
* ������ ��
* ���أ� ��
* ���ߣ� zhao.chong
 ********************************************************************/
static void timer_open(void)
{
    timer_set.function = timer_task;/*���ö�ʱ��������*/
    timer_set.expires = jiffies + HZ*TIME_SET_UP_INTERVAL;/*������TIME_SET_UP_INTERVAL�������*/ 
    add_timer(&timer_set);/*��Ӷ�ʱ��*/
}

/********************************************************************
* ������ uttRegDTimeChgHook
* ���ܣ� ע��ʱ��仯�ҹ�����
* ������ 2014-09-11
* ������ ��
* ���أ� ��
* ˵����һ�������ͬһ��hookHead�����в�����һ�����ϵ�hook�ڵ����һ����askerData(fn��Ϊ��չʹ��)
* ���ߣ� zhao.chong
 ********************************************************************/
int uttRegDTimeChgHook (struct list_head *hookHead, void *askerData, uttDTimeChgFn *fn)
{
    UttDateTimeChgHookOps *ops,*pos,*n;

    TIMEGRP_PRINT("func:%s--line:%d\n",__func__,__LINE__);
    list_for_each_entry_safe(pos,n,hookHead,list){
	if (pos->askerData == askerData) {
	    return 1;
	}
    }

    /**
     *hook��ע���ע���������°벿�н��У�����kmalloc�Ĳ���gfp_flags����ΪGFP_ATOMIC
     */
    ops = kmalloc(sizeof(*ops), GFP_ATOMIC);
    if (ops == NULL) {
	return 1;
    }

    ops->fn = fn;
    ops->askerData = askerData;
    list_add(&ops->list,hookHead);
    /*���Ͼ�ִ��һ��.�����������ʱ��仯�Ĵ���ʱ��*/
    if(fn != NULL) {
	fn(getUttDTimeNow(),askerData);
    }

    return 0;
}

/********************************************************************
* ������ uttUnRegDTimeChgHook
* ���ܣ� ע��ʱ��仯�ҹ�����
* ������ 2014-09-11
* ������ ��
* ���أ� ��
* ˵���� ����ָ����HOOK�ڵ㣬�ҵ����Ƴ�(fn��Ϊ��չʹ��)
* ���ߣ� zhao.chong
 ********************************************************************/
void uttUnRegDTimeChgHook (struct list_head *hookHead, void *askerData, uttDTimeChgFn *fn)
{
    UttDateTimeChgHookOps *pos,*n;

    TIMEGRP_PRINT("func:%s--line:%d\n",__func__,__LINE__);
    if(askerData!=NULL) {
	/**
	 *����askerData����hookHead����
	 */
	if (!list_empty(hookHead)) {
	    list_for_each_entry_safe(pos,n,hookHead,list){
		if (pos->askerData == askerData) {
		    if (pos->fn != NULL) {
			TIMEGRP_PRINT("func:%s--line:%d\n",__func__,__LINE__);
			pos->fn(NULL,pos->askerData);
		    }
		    list_del_init(&pos->list);
		    kfree(pos);
		}
	    }
	}
    }

}

EXPORT_SYMBOL(uttRegDTimeChgHook);
EXPORT_SYMBOL(uttUnRegDTimeChgHook);

static uint16_t _getTimeSetIndex(char *name)
{
    uint16_t index = uttTimeSetMax;

    if (name != NULL) {
	uttTimeSetLock();
	for (index = 0; index < uttTimeSetMax; index++) {
	    if ((uttTimeSetList[index] != NULL) && 
		    (strncmp(uttTimeSetList[index]->name,name,UTT_TIME_SET_MAXNAMELEN) == 0)) {
		break;
	    }
	}
	uttTimeSetUnlock();
    }

    return index;
}
/********************************************************************
* ������ getTimeSetIndex
* ���ܣ� ����ʱ�������Ʋ�������
* ������ 2014-09-12
* �޸ģ� 2015-01-05
* ������ ʱ��������
* ���أ� �ɹ�->����uttTimeSetList��Ӧ��������ʧ��->����uttTimeSetMax
* ���ߣ� zhao.chong
 ********************************************************************/
uint16_t getTimeSetIndex(char *name)
{
    uint16_t index;
#if 0
    UttTimeSet *timeSet;
#endif

    index = _getTimeSetIndex(name);
#if 0
    if (index != uttTimeSetMax) {
	uttTimeSetLock();
	if ((timeSet = uttTimeSetList[index]) != NULL) {
	    write_lock(&uttTimeSet[index]->lock);
	    atomic_inc(&uttTimeSet[index]->ref);
	    write_unlock(&uttTimeSet[index]->lock);
	}
	uttTimeSetUnlock();
    }
#endif

    return index;
}
EXPORT_SYMBOL(getTimeSetIndex);

/********************************************************************
* ������ putTimeSetIndex
* ���ܣ� ȥ����ָ��������ʱ���������
* ������ 2015-01-05
* ������ ʱ��������
* ���أ� ��
* ���ߣ� zhao.chong
 ********************************************************************/
void putTimeSetIndex(uint16_t index)
{
    UttTimeSet *timeSet;

    if (index != uttTimeSetMax) {
	uttTimeSetLock();
	if (((timeSet = uttTimeSetList[index]) != NULL)) {
	    write_lock(&timeSet->lock);
	    atomic_dec(&timeSet->ref);
	    write_unlock(&timeSet->lock);
	}
	uttTimeSetUnlock();
    }
}
EXPORT_SYMBOL(putTimeSetIndex);

/********************************************************************
* ������ uttTimeSetCreat
* ���ܣ� ʱ���鴴������
* ������ 2014-09-12
* �޸ģ� 2015-01-05
* ������ ʱ��������
* ���أ� �����ɹ�->����uttTimeSetList��Ӧ������������ʧ��->����uttTimeSetMax
* ���ߣ� zhao.chong
* ˵���� ��name�������򴴽�����������uttTimeSetList���棬�ɹ�����uttTimeSetList��Ӧ��������
 ********************************************************************/
uint16_t uttTimeSetCreat(const TimeGrpProfile *timeGrp)
{
    uint16_t index,i;
    UttTimeSet *timeSet;
    UttTimeSetWeekTime  *weekTime;

    TIMEGRP_PRINT("func:%s--line:%d-\n",__func__,__LINE__);
    if (timeGrp == NULL) {
	return uttTimeSetMax;
    }

    TIMEGRP_PRINT("func:%s--line:%d-\n",__func__,__LINE__);
    index = _getTimeSetIndex((char *)timeGrp->head.name);
    if (index != uttTimeSetMax) {/*�ҵ�name��Ӧ������*/
	return index;
    }
    TIMEGRP_PRINT("func:%s--line:%d-\n",__func__,__LINE__);

    uttTimeSetLock();
    for (index = 0; index < uttTimeSetMax; index++) {
	if (uttTimeSetList[index] == NULL) {
	    break;
	}
    }
    if (index != uttTimeSetMax) {/*ʱ����δ�ﵽ�����Ŀ���������*/
	timeSet = kzalloc(sizeof(UttTimeSet),GFP_ATOMIC);
	if (timeSet != NULL) {
	    INIT_LIST_HEAD(&timeSet->askerHookHead);
	    INIT_LIST_HEAD(&timeSet->delayExecList);
	    INIT_LIST_HEAD(&timeSet->weekTimeList);
	    strncpy(timeSet->name,timeGrp->head.name,UTT_TIME_SET_MAXNAMELEN);
	    rwlock_init(&timeSet->lock);
	    timeSet->datestart = timeGrp->datestart;
	    timeSet->datestop = timeGrp->datestop;
	    timeSet->isOnTimeChangeList = false;
	    TIMEGRP_PRINT("func:%s--line:%d-\n",__func__,__LINE__);
	    if ((timeSet->datestart == 1) && (timeSet->datestop == 2)) {/*������Ч*/
		timeSet->enable = 0x7;
	    } else {
		for (i = 0; i < TIMEGRP_ENTRYS; i++) {
		    if (timeGrp->timeEntry[i].enable) {/*���ø�ʱ�䵥Ԫ*/
			weekTime = kzalloc(sizeof(UttTimeSetWeekTime),GFP_ATOMIC);
			if (weekTime != NULL) {
			    weekTime->week = timeGrp->timeEntry[i].day;
			    if ((timeGrp->timeEntry[i].timestart == 1) && (timeGrp->timeEntry[i].timestart == 86401)) {/*ȫ����Ч*/
				weekTime->timestart = 0;/*00:00:00*/
				weekTime->timestop = SECONDS_OF_DAY - 1;/*23:59:59*/
			    } else {
				weekTime->timestart = timeGrp->timeEntry[i].timestart;
				weekTime->timestop = timeGrp->timeEntry[i].timestop;
			    }
			    list_add(&weekTime->list,&timeSet->weekTimeList);
			}
		    }
		}
	    }
	    uttTimeSetList[index] = timeSet;
	    TIMEGRP_PRINT("func:%s--line:%d,index=%d\n",__func__,__LINE__,index);
	}
    }
    uttTimeSetUnlock();

    return index; 
}
EXPORT_SYMBOL(uttTimeSetCreat);

/********************************************************************
* ������ uttTimeSetModify
* ���ܣ� ʱ�����޸�(����)����
* ������ 2014-09-16
* �޸ģ� 2015-01-05
* ������ ʱ����Profile
* ���أ� �޸ĳɹ�->����uttTimeSetList��Ӧ���������޸�ʧ��->����uttTimeSetMax
* ���ߣ� zhao.chong
 ********************************************************************/
uint16_t uttTimeSetModify(const TimeGrpProfile * timeGrp)
{
    uint16_t index,i;
    UttTimeSet *timeSet;
    UttTimeSetWeekTime  *weekTime;
    UttTimeSetAskerHookOps *ops,*next;

    TIMEGRP_PRINT("func:%s--line:%d\n",__func__,__LINE__);
    if (timeGrp == NULL) {
	return uttTimeSetMax;
    }

    index = _getTimeSetIndex((char *)timeGrp->head.name);
    TIMEGRP_PRINT("func:%s--line:%d,index=%d\n",__func__,__LINE__,index);
    if (index != uttTimeSetMax) {/*�ҵ�name��Ӧ������*/
	uttTimeSetLock();
	timeSet = uttTimeSetList[index];
	write_lock(&timeSet->lock);
	uttTimeSetWeekTimeFlush(timeSet);/*��ɾ��ʱ�䵥Ԫ*/
	if (atomic_read(&timeSet->ref) == 0) {/*�����ʱ�䵥Ԫû�б��û�ģ�����õĻ�*/
	    /*�����޸�ʱ����timeSet*/
	    timeSet->datestart = timeGrp->datestart;
	    timeSet->datestop = timeGrp->datestop;
	    timeSet->isOnTimeChangeList = false;
	    if ((timeSet->datestart == 1) && (timeSet->datestop == 2)) {/*������Ч*/
		timeSet->enable = 0x7;
	    } else {
		timeSet->enable = 0;
		for (i = 0; i < TIMEGRP_ENTRYS; i++) {/*�����ʱ�䵥Ԫ*/
		    if (timeGrp->timeEntry[i].enable) {/*���ø�ʱ�䵥Ԫ*/
			weekTime = kzalloc(sizeof(UttTimeSetWeekTime),GFP_ATOMIC);
			if (weekTime != NULL) {
			    weekTime->week = timeGrp->timeEntry[i].day;
			    if ((timeGrp->timeEntry[i].timestart == 1) && (timeGrp->timeEntry[i].timestart == 86401)) {/*ȫ����Ч*/
				weekTime->timestart = 0;/*00:00:00*/
				weekTime->timestop = SECONDS_OF_DAY - 1;/*23:59:59*/
			    } else {
				weekTime->timestart = timeGrp->timeEntry[i].timestart;
				weekTime->timestop = timeGrp->timeEntry[i].timestop;
			    }
			    list_add(&weekTime->list,&timeSet->weekTimeList);
			}
		    }
		}
	    }
	    write_unlock(&timeSet->lock);
	    uttTimeSetUnlock();

	    return index;
	} else {
	    /**
	     *�ж�ʱ�����Ƿ��ڵȴ�ʱ���������ǣ���ӵȴ�����ȥ��
	     */
	    delFromWaitList(timeSet);

	    if (timeSet->enable == 0x7) {/*��ʱʱ������Ч*/
		if (!list_empty(&timeSet->askerHookHead)) {
		    list_for_each_entry_safe(ops,next,&timeSet->askerHookHead,list){
			ops->fn(0,ops->askerData);
		    }
		}
	    }
	    if (!list_empty(&timeSet->delayExecList)) {/*δִ�е�����*/
		list_for_each_entry_safe(ops,next,&timeSet->delayExecList,list){
		    list_del(&ops->list);
		    list_add(&ops->list,&timeSet->askerHookHead);
		}
	    }

	    if (timeSet->isOnTimeChangeList) {/*ʱ�����Ѿ���ʱ��仯����*/
		uttUnRegTimeSetFromDTChg(timeSet);/*�ȴ�ʱ��仯������ע����ʱ����*/
	    }
	}

	/*���������޸�ʱ����timeSet*/
	timeSet->datestart = timeGrp->datestart;
	timeSet->datestop = timeGrp->datestop;

	if ((timeSet->datestart == 1) && (timeSet->datestop == 2)) {/*������Ч*/
	    timeSet->enable = 0x7;/*��Ϊ��Ч*/
	    if (!list_empty(&timeSet->askerHookHead)) {
		list_for_each_entry_safe(ops,next,&timeSet->askerHookHead,list){
		    list_del(&ops->list);
		    list_add(&ops->list,&timeSet->delayExecList);
		}
	    }
	} else {
	    timeSet->enable = 0;
	    for (i = 0; i < TIMEGRP_ENTRYS; i++) {/*�����ʱ�䵥Ԫ*/
		if (timeGrp->timeEntry[i].enable) {/*���ø�ʱ�䵥Ԫ*/
		    weekTime = kzalloc(sizeof(UttTimeSetWeekTime),GFP_ATOMIC);
		    if (weekTime != NULL) {
			weekTime->week = timeGrp->timeEntry[i].day;
			if ((timeGrp->timeEntry[i].timestart == 1) && (timeGrp->timeEntry[i].timestart == 86401)) {/*ȫ����Ч*/
			    weekTime->timestart = 0;/*00:00:00*/
			    weekTime->timestop = SECONDS_OF_DAY - 1;/*23:59:59*/
			} else {
			    weekTime->timestart = timeGrp->timeEntry[i].timestart;
			    weekTime->timestop = timeGrp->timeEntry[i].timestop;
			}
			list_add(&weekTime->list,&timeSet->weekTimeList);
		    }
		}
	    }
	}
	write_unlock(&timeSet->lock);
	TIMEGRP_PRINT("func:%s--line:%d,index=%d\n",__func__,__LINE__,index);

	uttRegTimeSetToDTChg(timeSet);/*����ʱ����ע�ᵽʱ��仯������*/
	uttTimeSetUnlock();
    }

    return index; 
}
EXPORT_SYMBOL(uttTimeSetModify);

/********************************************************************
* ������ uttTimeSetDelByIndex
* ���ܣ� ʱ����ɾ������
* ������ 2014-09-18
* ������ ʱ��������
* ���أ� ��
* ���ߣ� zhao.chong
 ********************************************************************/
void uttTimeSetDelByIndex(uint16_t index)
{
    UttTimeSet * timeSet;
    bool flag = false;
    
    if (index < uttTimeSetMax) {
	uttTimeSetLock();
	timeSet = uttTimeSetList[index];
	if (timeSet != NULL) {
	    write_lock(&timeSet->lock);
	    if (atomic_read(&timeSet->ref) == 0) {/*û��ģ�����ø�ʱ����*/
		TIMEGRP_PRINT("func:%s--line:%d,index=%d\n",__func__,__LINE__,index);
		uttTimeSetWeekTimeFlush(timeSet);
		flag = true;
	    }
	    write_unlock(&timeSet->lock);
	    if (flag) {
		kfree(timeSet);
		timeSet = uttTimeSetList[index] = NULL;
	    }
	}
	uttTimeSetUnlock();
    }
}
EXPORT_SYMBOL(uttTimeSetDelByIndex);

/********************************************************************
* ������ uttTimeSetDel
* ���ܣ� ʱ����ɾ������
* ������ 2014-09-18
* ������ ʱ����Profile
* ���أ� ��ɾ����ʱ��������
* ���ߣ� zhao.chong
 ********************************************************************/
uint16_t uttTimeSetDel(const TimeGrpProfile * timeGrp)
{
    uint16_t index;
    
    if (timeGrp == NULL) {
	return uttTimeSetMax;
    }

    index = _getTimeSetIndex((char *)timeGrp->head.name);
    TIMEGRP_PRINT("func:%s--line:%d,index=%d\n",__func__,__LINE__,index);
    if (index != uttTimeSetMax) {
	uttTimeSetDelByIndex(index);
    }

    return index;
}
EXPORT_SYMBOL(uttTimeSetDel);

/********************************************************************
* ������ uttTimeSetDelAll
* ���ܣ� ɾ������ʱ���麯��
* ������ 2014-09-18
* ������ ��
* ���أ� ��
* ���ߣ� zhao.chong
 ********************************************************************/
void uttTimeSetDelAll(void)
{
    uint16_t index;

    for (index = 0; index < uttTimeSetMax; index++) {
	uttTimeSetDelByIndex(index);
    }
}
EXPORT_SYMBOL(uttTimeSetDelAll);

/********************************************************************
 * ������ uttTimeSetWeekTimeFlush
 * ���ܣ� ʱ�����ʱ�䵥Ԫ���ٺ���
 * ������ 2014-09-12
 * ������ ʱ����
 * ���أ� 0->�޸ĳɹ�����0->�޸�ʧ��
 * ���ߣ� zhao.chong
 ********************************************************************/
static int uttTimeSetWeekTimeFlush(UttTimeSet * timeSet)
{
    int ret = -1;
    UttTimeSetWeekTime *pos,*n;

    if (timeSet != NULL) {
	ret = 0;
	if (!list_empty(&timeSet->weekTimeList)) {
	    list_for_each_entry_safe(pos,n,&timeSet->weekTimeList,list){
		list_del_init(&pos->list);
		kfree(pos);
		pos = NULL;
	    }
	}
    }

    return 0;
}

/********************************************************************
* ������ uttTimeSetMinChg
* ���ܣ� ʱ������ӱ仯ִ�к���
* ������ 2014-09-12
* ������ @timeNow-��ǰʱ�䣻@conf-ʱ����
* ���أ� 0->�ɹ�����0->ʧ��
* ���ߣ� zhao.chong
 ********************************************************************/
static int uttTimeSetMinChg (const UttDateTime*timeNow, void *conf)
{
    UttTimeSet *timeSet = (UttTimeSet*)conf;
    UttTimeSetWeekTime *pos,*n;
    UttTimeSetAskerHookOps *ops,*next;
    uint16_t flag = 0U;/*�����Ƿ���Ч��־*/
    uint16_t oldEnable;

    TIMEGRP_PRINT("func:%s--line:%d\n",__func__,__LINE__);
    if ((timeNow == NULL) || (timeSet == NULL)) {
	TIMEGRP_PRINT("func:%s--line:%d\n",__func__,__LINE__);
	return 1;
    }

    TIMEGRP_PRINT("func:%s--line:%d\n",__func__,__LINE__);
    write_lock(&timeSet->lock);
    oldEnable = timeSet->enable;

    if (oldEnable == 0x7) {
	if (!list_empty_careful(&timeSet->delayExecList)) {/*��ʱ������ڴ�����ִ�е�ģ��*/ 
	    list_for_each_entry_safe(ops,next,&timeSet->delayExecList,list){
		if (ops->fn != NULL) {
		    ops->fn(1,ops->askerData);
		}
		list_del(&ops->list);
		list_add(&ops->list,&timeSet->askerHookHead);
	    }
	}
    }

    if (!list_empty_careful(&timeSet->weekTimeList)) {
	list_for_each_entry_safe(pos,n,&timeSet->weekTimeList,list){/*����ʱ�䵥Ԫ*/
	    if (timeNow->week & pos->week) {/*����*/
		if (pos->timestart/SECONDS_OF_MINUTE < pos->timestop/SECONDS_OF_MINUTE) {
		    if ((timeNow->time >= pos->timestart/SECONDS_OF_MINUTE) &&
			(pos->timestop/SECONDS_OF_MINUTE >= timeNow->time )) {/*������Ч*/
			flag = 1U;
			break;
		    }
		} else {
		    if ((timeNow->time >= pos->timestart/SECONDS_OF_MINUTE) || (timeNow->time <= pos->timestop/SECONDS_OF_MINUTE)) {/*������Ч*/
			flag = 1U;
			break;
		    }
		}
	    }
	}
    }
    if (flag == 1U) {
	timeSet->enable |= 0x1;/*����Ч��λ*/
    } else {
	timeSet->enable &= ~0x1;
    }
    TIMEGRP_PRINT("func:%s--line:%d,oldEnable=%d,enableNow=%d\n",__func__,__LINE__,oldEnable,timeSet->enable);
    if ((oldEnable & 0x1) == 0) {/*֮ǰ����Ч*/
	if ((timeSet->enable & 0x1) == 0x1) {/*�ɲ���Ч��Ϊ��Ч*/
	    list_for_each_entry_safe(ops,next,&timeSet->askerHookHead,list){
		if (ops->fn != NULL) {
		    ops->fn(timeSet->enable == 0x7,ops->askerData);
		}
	    }
	}
    } else {/*֮ǰ��Ч*/
	if ((timeSet->enable & 0x1) == 0) {/*����Ч��Ϊ����Ч*/
	    list_for_each_entry_safe(ops,next,&timeSet->askerHookHead,list){
		if (ops->fn != NULL) {
		    ops->fn(timeSet->enable == 0x7,ops->askerData);
		}
	    }
	}
    }
    write_unlock(&timeSet->lock);

    return 0;
}

/********************************************************************
* ������ uttTimeSetHourChg
* ���ܣ� ʱ����Сʱ�仯ִ�к���
* ������ 2014-09-15
* ������ @timeNow-��ǰʱ�䣻@conf-ʱ����
* ���أ� 1->ʧ�ܣ�0->�ɹ�
* ���ߣ� zhao.chong
 ********************************************************************/
static int uttTimeSetHourChg (const UttDateTime*timeNow, void *conf)
{
    UttTimeSet *timeSet = (UttTimeSet*)conf;
    UttTimeSetWeekTime *pos,*n;
    unsigned char flag = 0U;
    uint16_t oldEnable;
    UttTimeSetAskerHookOps *ops,*next;

    TIMEGRP_PRINT("func:%s--line:%d\n",__func__,__LINE__);
    if (timeNow == NULL) {
	TIMEGRP_PRINT("func:%s--line:%d\n",__func__,__LINE__);
	uttUnRegMinChgHook(conf,uttTimeSetMinChg);
	return 0;
    }

    if (timeSet != NULL) {
	write_lock(&timeSet->lock);
	oldEnable = timeSet->enable;
	if (!list_empty(&timeSet->weekTimeList)) {/*������Ч��ʱ�䵥Ԫ*/
	    list_for_each_entry_safe(pos,n,&timeSet->weekTimeList,list){
		if (timeNow->week & pos->week) {/*����*/
		    /*ֻ����ĳһʱ��ε����˵������ע����ӱ仯����*/
		    if ((timeNow->time/MINUTE_OF_HOUR== pos->timestart/SECONDS_OF_HOUR) ||
			    (timeNow->time/MINUTE_OF_HOUR == pos->timestop/SECONDS_OF_HOUR)) {
			flag |= 0x1;/*����ʱ�����ֻҪ��һ����������*/
		    } else {/*������*/
			/*��ʼʱ��С�ڽ���ʱ��*/
			if (pos->timestart/SECONDS_OF_HOUR < pos->timestop/SECONDS_OF_HOUR) {
			    if ((timeNow->time/MINUTE_OF_HOUR >= pos->timestart/SECONDS_OF_HOUR) &&
				(timeNow->time/MINUTE_OF_HOUR <= pos->timestop/SECONDS_OF_HOUR)) {
				flag |= 0x1<<1;/*��ǰʱ����ʱ�����*/
			    } else {
				flag |= 0x1<<2;/*��ǰʱ�䲻��ʱ�����*/
			    }
			} else {
			    if ((timeNow->time/MINUTE_OF_HOUR <= pos->timestop/SECONDS_OF_HOUR) ||
				(timeNow->time/MINUTE_OF_HOUR >= pos->timestart/SECONDS_OF_HOUR)) {
				flag |= 0x1<<1;/*��ǰʱ����ʱ�����*/
			    } else {
				flag |= 0x1<<2;/*��ǰʱ�䲻��ʱ�����*/
			    }
			}
		    }
		}
	    }
	}
	if (flag & 0x1) {/*������һ��ʱ��δ�������λ��*/
	    if ((oldEnable & 0x2) == 0) { /*ԭʱ�仯����Ч*/
		timeSet->enable |= 0x2;/*Сʱ��Ϊ��Ч*/
	    }
	    write_unlock(&timeSet->lock);
	    uttRegMinChgHook(timeSet,uttTimeSetMinChg);/*ע����ӱ仯����*/
	    write_lock(&timeSet->lock);
	} else {
	    if (flag & 0x2) {/*ʱ��ε�����֮��*/
		timeSet->enable |= 0x2;/*ʱ��Ϊ��Ч*/
	    } else {/*��ǰʱ�䲻��ʱ�����ʱ�����*/
		timeSet->enable &= ~0x2;/*ʱ��ΪʧЧ*/
	    }

	    if ((oldEnable & 0x2) != 0) { /*ԭʱ�仯��Ч,�ֱ仯�����ϴ��ڸ�ʱ�����ע��*/
		uttUnRegMinChgHook(timeSet,uttTimeSetMinChg);
		TIMEGRP_PRINT("fun:%s,line:%d,timeSet->enable=%d,(timeSet->enable & 0x2) = %d,(timeSet->enable & 0x1)=%d\n",__func__,__LINE__,timeSet->enable,(timeSet->enable & 0x2),(timeSet->enable & 0x1));
		if (((timeSet->enable & 0x2) == 0) && ((timeSet->enable & 0x1) == 0x1)) {/*Сʱ����Ч��Ϊ����Ч,�ҷ���bitλ����Ч*/
		    timeSet->enable &= ~0x1;/*������Чλ��0������ִ�����ø�ʱ�����ģ��*/
		    list_for_each_entry_safe(ops,next,&timeSet->askerHookHead,list){
			if (ops->fn != NULL) {
			    ops->fn(timeSet->enable == 0x7,ops->askerData);
			}
		    }
		}
	    } else {
		if ((timeSet->enable & 0x2) != 0) {/*ʱ�ɲ���Ч����Ч(��ʱ�����״ι���ʱ)*/
		    write_unlock(&timeSet->lock);
		    uttRegMinChgHook(timeSet,uttTimeSetMinChg);/*ע����ӱ仯����*/
		    write_lock(&timeSet->lock);
		}
	    }
	}
	write_unlock(&timeSet->lock);
    }

    return 0;
}

/********************************************************************
* ������ uttTimeSetDayChg
* ���ܣ� ʱ������仯ִ�к���
* ������ 2014-09-15
* ������ @timeNow-��ǰʱ�䣻@conf-ʱ����
* ���أ� �� 
* ���ߣ� zhao.chong
 ********************************************************************/
static int uttTimeSetDayChg (const UttDateTime*timeNow, void *conf)
{
    UttTimeSet *timeSet = (UttTimeSet*)conf;
    UttTimeSetWeekTime *pos,*n;

    uint16_t oldWeekEnable;
    unsigned char flag = 0U;

    TIMEGRP_PRINT("func:%s--line:%d\n",__func__,__LINE__);
    if (timeNow != NULL) {
	write_lock(&timeSet->lock);
	oldWeekEnable = timeSet->enable & 0x4;
	TIMEGRP_PRINT("func:%s--line:%d\n",__func__,__LINE__);
	if (timeSet->datestart <= timeNow->date && timeNow->date <= timeSet->datestop) {
	    list_for_each_entry_safe(pos,n,&timeSet->weekTimeList,list){
		if (pos->week & timeNow->week) {/*ʱ�䵥Ԫ��ֻҪ��һ�����㣬�����Ϊ��Ч*/
		    flag = 1U;
		    break;
		}
	    }
	}

	if (flag) {
	    timeSet->enable |= 0x4;
	} else {
	    timeSet->enable &= ~0x4;
	}

	if ((oldWeekEnable == 0) && ((timeSet->enable & 0x4) > 0)) {/*���ɲ���Ч��Ϊ��Ч*/
	    write_unlock(&timeSet->lock);
	    uttRegHourChgHook(timeSet,uttTimeSetHourChg);/*ע��Сʱ�仯����*/
	    write_lock(&timeSet->lock);
	} else if(oldWeekEnable > 0 &&  ((timeSet->enable & 0x4) == 0)) {/*������Ч��Ϊ����Ч*/
	    uttUnRegHourChgHook(timeSet,uttTimeSetHourChg);/*ע��Сʱ�仯�Ĺ���*/
	}
	write_unlock(&timeSet->lock);
    } else {/*�˴�Ϊע��ʱ���ã����Сʱ��ص��޳���*/
	TIMEGRP_PRINT("func:%s--line:%d\n",__func__,__LINE__);
	uttUnRegHourChgHook(timeSet,uttTimeSetHourChg);
    }

    return 0;
}

/********************************************************************
* ������ uttRegTimeSetToDTChg
* ���ܣ� ע��ʱ���鵽ʱ��仯�ṹ�ĺ���
* ������ 2014-09-12
* �޸ģ� 2015-01-04
* ������ ʱ����
* ���أ� 0->ע��ɹ�����0->ע��ʧ��
* ���ߣ� zhao.chong
 ********************************************************************/
int uttRegTimeSetToDTChg (void *timeSetConf)
{
    UttTimeSetAskerHookOps *pos, *n;
    unsigned char flag = 0U;
    int ret = 0;
    UttTimeSet *timeSet = (UttTimeSet *)timeSetConf;
    if (timeSet != NULL) {
	write_lock(&timeSet->lock);
	if (!list_empty_careful(&timeSet->delayExecList)) {/*��ʱ������ڴ�����ִ�е�ģ��*/
	    flag = 1U;/*ʱ���黹û�й��ص�ʱ��仯����ʹ��ڴ�����ִ�е����ã�˵����������Ч����*/
	    list_for_each_entry_safe(pos,n,&timeSet->delayExecList,list) {/*������ִ���Ѿ����ص���ʱ������û�ģ��*/
		if (pos->fn != NULL) {
		    pos->fn(1,pos->askerData);
		}
		list_del(&pos->list);
		list_add(&pos->list,&timeSet->askerHookHead);
	    }
	}
	write_unlock(&timeSet->lock);
    }

    if (flag == 0U) {/*��������Ч����*/
	ret = uttRegDayChgHook((void*)timeSet,uttTimeSetDayChg);
	write_lock(&timeSet->lock);
	timeSet->isOnTimeChangeList = true;
	write_unlock(&timeSet->lock);
    }
    return ret;
}
EXPORT_SYMBOL(uttRegTimeSetToDTChg);

/********************************************************************
* ������ uttUnRegTimeSetFromDTChg
* ���ܣ� ע��ʱ������ʱ��仯�ṹ�ĺ���
* ������ 2014-09-15
* ������ ʱ����
* ���أ� ��
* ���ߣ� zhao.chong
 ********************************************************************/
void uttUnRegTimeSetFromDTChg (UttTimeSet *timeSet)
{
    /*dayע��ʱ���Զ�ע����Сʱ�ͷ���*/
    uttUnRegDayChgHook(timeSet,uttTimeSetDayChg);
}
EXPORT_SYMBOL(uttUnRegTimeSetFromDTChg);

/********************************************************************
* ������ addToTimeSetWaitList
* ���ܣ� ��Ӧ��ģ�����ȴ�������ʱ��ִ�к�����ʼִ��ʱ�Ὣ�ȴ������е����ݼ��뵽ʱ��仯������
* ������ 2015-01-04
* ������ @conf-��û�й��ص�ʱ��仯�����ϵ�ʱ���飨ʱ�����Ѿ��������õ��û�ģ�飩��
*	 @fn-�ú�����ʱ����conf���ص�ʱ��仯������
* ���أ� ���ؽ����ʾ�Ƿ�ʧ�ܣ�false->�ɹ���true->ʧ��
* ���ߣ� zhao.chong
* ˵���� ���һ��ģ�������õ�ʱ���鷢���仯�����ڵ��ô˺���֮ǰӦ���ȵ���ģ��ж�غ���
 ********************************************************************/
bool addToTimeSetWaitList(void *conf,uttTimeToDTChg *fn)
{
    bool isErr = false,exist = false;
    UttTimeWaitNode *pos;

    TIMEGRP_PRINT("----file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);
    uttTimeWaitListLock();
    if (list_empty(&timeWaitList)) {/*û��ģ�����ø�ʱ����*/ 
	list_for_each_entry(pos,&timeWaitList,waitNode) {/*�����ȴ�����,�ж��Ƿ��Ѿ�����*/
	    if (pos->conf == conf) {/*�Ѿ�����*/
		exist = true;
		break;
	    }
	}
    }
    if (!exist) {
	pos = kzalloc(sizeof(UttTimeWaitNode), GFP_ATOMIC);
	if (pos == NULL) {
	    isErr = true;
	} else {
	    pos->conf = conf;
	    pos->fn = fn;
	    list_add(&pos->waitNode,&timeWaitList);
	}
    }
    uttTimeWaitListUnlock();

    return isErr;
}
EXPORT_SYMBOL(addToTimeSetWaitList);

/********************************************************************
* ������ delFromWaitList
* ���ܣ� ��ʱ�����ôӵȴ�������ȥ��
* ������ 2015-01-05
* ������ @conf-ʱ�����ã�
* ���أ� ��
* ���ߣ� zhao.chong
* ˵���� ��
 ********************************************************************/
void delFromWaitList(void *conf)
{
    UttTimeWaitNode *pos,*n;

    uttTimeWaitListLock();
    if (!list_empty_careful(&timeWaitList)) {
	list_for_each_entry_safe(pos,n,&timeWaitList,waitNode){/*����ʱ�䵥Ԫ*/
	    if (pos->conf == conf) {
		list_del(&pos->waitNode);
		break;
	    }
	}
    }
    uttTimeWaitListUnlock();
}
EXPORT_SYMBOL(delFromWaitList);

/********************************************************************
 * ������ uttRegToTimeSet
 * ���ܣ� ��Ӧ��ģ��regע�ᵽindexʱ����
 * ������ 2014-09-26
 * ������ @index�����õ�ʱ����������@askerData-����indexʱ�����ģ�飻@fn-ģ��ִ�к���
 * ���أ� true->ע��ɹ���false->ע��ʧ��
 * ���ߣ� zhao.chong
 * ˵���� 1.���һ��ģ�������õ�ʱ���鷢���仯�����ڵ��ô˺���֮ǰӦ���ȵ���ģ��ж�غ���;
 *	  2.�ú�������������ʱ������ص�ʱ��仯����
 ********************************************************************/
bool uttRegToTimeSet (uint16_t index,void *askerData, uttTimeSetAskerFn *fn)
{
    UttTimeSet *timeSet;
    UttTimeSetAskerHookOps *pos, *n, *reg;
    unsigned char flag = 0U;
    bool isErr = false;

    if ((askerData == NULL) || (index == uttTimeSetMax)) {
	return isErr;
    }

    uttTimeSetLock();
    timeSet = uttTimeSetList[index];
    if (timeSet != NULL) {
	write_lock(&timeSet->lock);
	if (!list_empty(&timeSet->askerHookHead)) {/*��ʱ������ڹ��ص�ģ��*/
	    list_for_each_entry_safe(pos,n,&timeSet->askerHookHead,list) {/*�����жϸ�ģ���Ƿ��Ѿ����ص��˸�ʱ����*/
		if (pos->askerData == askerData) {/*���û������Ѿ�����*/
		    flag = 1U;
		    break;
		}
	    }
	}
	if (flag == 0U) {
	    reg = kzalloc(sizeof(UttTimeSetAskerHookOps), GFP_ATOMIC);
	    if (reg == NULL) {
		isErr = true;
	    }
	    if (!isErr) {
		reg->askerData = askerData;
		reg->fn = fn;
		if (timeSet->isOnTimeChangeList) {/*��ʱ�����Ѿ�����ʱ��仯������*/
		    if (timeSet->enable == 0x7) {/*�����ʱ��Ч�����û����ü��뵽�ӳ�ִ��������*/
			list_add(&reg->list,&timeSet->delayExecList);
		    } else {/*�����ʱ����Чֱ�ӹ��ڵ�askerHookHead�ϵȴ�ʱ����Чִ��*/
			list_add(&reg->list,&timeSet->askerHookHead);
		    }
		} else {
		    if (timeSet->enable == 0x7) {/*�������ʱ��仯�����У�ʱ����Ȼ��Ч����˵����������Чʱ��*/
			list_add(&reg->list,&timeSet->delayExecList);
		    } else {
			list_add(&reg->list,&timeSet->askerHookHead);
		    }
		    if ((isErr = addToTimeSetWaitList(timeSet,uttRegTimeSetToDTChg)) == true) {/*����ȴ��б�ʧ��*/
			list_del(&reg->list);
			kfree(reg);
		    }
		}
		if (!isErr) {
		    /*ÿ�����һ��ģ������ʱ��Σ������ü�����1*/
		    atomic_inc(&timeSet->ref);
		}
	    }
	}
	write_unlock(&timeSet->lock);
    }
    uttTimeSetUnlock();

    return (flag == 1U || isErr == true);
}
EXPORT_SYMBOL(uttRegToTimeSet);

/********************************************************************
* ������ uttUnRegFromTimeSet
* ���ܣ� ��Ӧ��ģ��reg��indexʱ����ע��
* ������ 2014-09-26
* �޸ģ� 2015-01-05
* ������ @index�����õ�ʱ����������@askerData-����indexʱ�����ģ��
* ���أ� 0->ע���ɹ�����0->ע��ʧ��
* ���ߣ� zhao.chong
* ˵���� �ڽ��û����ô������õ�ʱ������ж��ǰ�����Ƚ���ʱ��������ü�����1
 ********************************************************************/
int uttUnRegFromTimeSet(uint16_t index, void *askerData)
{
    UttTimeSet *timeSet;
    UttTimeSetAskerHookOps *pos, *n;

    if ((askerData == NULL) || (index == uttTimeSetMax)) {
	return 1;
    }

    uttTimeSetLock();
    timeSet = uttTimeSetList[index];
    if (timeSet != NULL) {
	write_lock(&timeSet->lock);

	if (!list_empty_careful(&timeSet->askerHookHead)) {
	    list_for_each_entry_safe(pos,n,&timeSet->askerHookHead,list) {
		if (pos->askerData == askerData) {
		    list_del_init(&pos->list);
		    if (timeSet->enable == 0x7) {
			pos->fn(0,askerData);
		    }
		    kfree(pos);
		    pos = NULL;
		    break;
		}
	    }
	}
	if (!list_empty(&timeSet->delayExecList)) {
	    list_for_each_entry_safe(pos,n,&timeSet->delayExecList,list) {
		if (pos->askerData == askerData) {
		    list_del_init(&pos->list);
		    kfree(pos);
		    pos = NULL;
		    break;
		}
	    }
	}
	if ((atomic_read(&timeSet->ref) > 0) && (atomic_dec_return(&timeSet->ref) == 0)) {/*��ʱ�����Ѿ�û�б��κ�ģ������*/
	    if (timeSet->isOnTimeChangeList) {/*ʱ������ʱ��仯������*/
		uttUnRegTimeSetFromDTChg(timeSet);
		timeSet->isOnTimeChangeList = false;
		timeSet->enable = 0;
	    } else {
		/*ʱ��������ڵȴ������ϣ���ӵȴ�������ɾ��*/
		delFromWaitList(timeSet);
	    }
	}
	write_unlock(&timeSet->lock);
    }
    uttTimeSetUnlock();

    return 0;
}
EXPORT_SYMBOL(uttUnRegFromTimeSet);

#if 0
/********************************************************************
* ������ uttTimeSetRefCout
* ���ܣ� ʱ�������ü����仯
* ������ 2014-09-26
* ������ @index�����õ�ʱ����������@optType��0-ʱ�������ü�����1,1-ʱ�������ü�����1
* ���أ� 0->�����ɹ�����0->����ʧ��
* ���ߣ� zhao.chong
 ********************************************************************/
int uttTimeSetRefCout (uint16_t index, uint16_t optType)
{
    UttTimeSet *timeSet;

    if (index >= uttTimeSetMax) {
	return 1;
    }
    uttTimeSetLock();
    timeSet = uttTimeSetList[index];
    if (timeSet == NULL) {
	uttTimeSetUnlock();
	return 1;
    }
    if (optType == 1) {
	atomic_inc(&timeSet->ref);
    } else {
	atomic_dec(&timeSet->ref);
    }
    uttTimeSetUnlock();

    return 0;
}
EXPORT_SYMBOL(uttTimeSetRefCout);
#endif
/********************************************************************
* ������ time_set_init
* ���ܣ� ʱ����ģ����س�ʼ������
* ������ 2014-09-11
* ������ ��
* ���أ� 0->�ɹ����أ���0->����ģ��ʧ��
* ���ߣ� zhao.chong
 ********************************************************************/
static int __init time_set_init (void)
{
    TIMEGRP_PRINT("func:%s,Line %d---start\n",__func__,__LINE__);
    /*��ʼ����ʱ��仯��������*/
    INIT_LIST_HEAD(&uttDayChgHookHead);
    INIT_LIST_HEAD(&uttHourChgHookHead);
    INIT_LIST_HEAD(&uttMinChgHookHead);
    INIT_LIST_HEAD(&timeWaitList);

    init_timer(&timer_set);/*��ʼ����ʱ��*/ 
    timer_open();/*������ʱ��*/

    if (maxTimeSets) {/*����ģ��ʱ��ָ̬������*/
	uttTimeSetMax = maxTimeSets;
    }
    uttTimeSetList = kzalloc(sizeof(UttTimeSet *) * uttTimeSetMax,GFP_ATOMIC);

    TIMEGRP_PRINT("func:%s,Line %d---end\n",__func__,__LINE__);
    return 0;
}

/********************************************************************
* ������ time_set_exit
* ���ܣ� ʱ����ģ��ж�غ���
* ������ 2013-09-22
* ������ ��
* ���أ� ��
* ���ߣ� zhao.chong
 ********************************************************************/
static void __exit time_set_exit (void)
{
    TIMEGRP_PRINT("func:%s,Line %d---start\n",__func__,__LINE__);
    del_timer_sync(&timer_set);
    uttTimeSetDelAll();
    TIMEGRP_PRINT("func:%s,Line %d---end\n",__func__,__LINE__);
}

module_init(time_set_init);
module_exit(time_set_exit);
