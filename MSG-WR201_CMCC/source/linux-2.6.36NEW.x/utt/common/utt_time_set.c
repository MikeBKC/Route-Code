#include <linux/ip.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/jiffies.h>/*定义了jiffies*/
#include <asm/param.h>/*定义了HZ*/
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

/****************全局变量*******start********************/
static DEFINE_SPINLOCK(utt_time_set_app_spinlock);
static DEFINE_SPINLOCK(timeWaitListLock);

static struct timer_list timer_set;/*定时器结构变量，文件作用域*/
struct list_head uttDayChgHookHead;/*一旦天发生了变化，就会遍历该链表，把上面的函数执行一遍*/
struct list_head uttHourChgHookHead;/*一旦小时发生了变化，就会遍历该链表，把上面的函数执行一遍*/
struct list_head uttMinChgHookHead;/*一旦分钟发生了变化，就会遍历该链表，把上面的函数执行一遍*/
struct list_head timeWaitList;/*引用时间配置的等待列表*/
EXPORT_SYMBOL(uttDayChgHookHead);
EXPORT_SYMBOL(uttHourChgHookHead);
EXPORT_SYMBOL(uttMinChgHookHead);
EXPORT_SYMBOL(timeWaitList);
uint16_t uttTimeSetMax = CONFIG_UTT_TIME_SET_MAX;
static int maxTimeSets;

module_param(maxTimeSets, int, 0600);
MODULE_PARM_DESC(maxTimeSets, "maximal number of time sets");
static UttTimeSet **uttTimeSetList;		/* all individual time sets */
/*定义一个全局结构体，所有应用都可以使用这个表示当前系统时间*/
static UttDateTime uttDTimeNow;
static UttDateTime uttDTimeOld;

/****************全局变量*******end********************/


/****************函数声明*******start******************/
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
/****************函数声明*******end******************/

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
* 函数： traverseTimeWaitList
* 功能： 遍历等待链表，将链表上的时间配置挂载到时间变化链表中
* 创建： 2015-01-04
* 参数： 无
* 返回： 无
* 作者： zhao.chong
* 说明:	 该函数会没十秒钟执行一次，为避免引用时间配置的用户模块多次执行，此函数
*	 应该在遍历完时间变化链表后调用
 ********************************************************************/
static void traverseTimeWaitList(void)
{
    UttTimeWaitNode *pos,*n;
    int count = 0;

    TIMEGRP_PRINT("------file:%s,func:%s,Line %d---\n",__FILE__,__func__,__LINE__);
    uttTimeWaitListLock();
    if (!list_empty_careful(&timeWaitList)) {
	list_for_each_entry_safe(pos,n,&timeWaitList,waitNode){/*遍历时间单元*/
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
* 函数： timer_task
* 功能： 定时任务执行函数
* 创建： 2014-09-11
* 参数： 开启定时器时向定时器执行函数传递的参数
* 返回： 无
* 作者： zhao.chong
* 说明:	 当用户手动设置系统时间为过去的时间，此定时任务可能会有最长一分钟的延时执行
 ********************************************************************/
static void timer_task(unsigned long data)
{
    const UttDateTime *uttDateTimeNow,*uttDateTimeOld;
    UttDateTimeChgHookOps *pos,*n;

    set_current_time();/*设置当前时间*/
    uttDateTimeNow = getUttDTimeNow();/*获取当前时间*/
    uttDateTimeOld = getUttDTimeOld();/*获取上一个时间节点*/

    /**
     *
     * 分析3种时间类型的变化:天变化，时变化，分变化
     */
    if ((uttDateTimeNow->time != uttDateTimeOld->time) || ((uttDateTimeNow->date != uttDateTimeOld->date))) {/*分发生变化*/
	TIMEGRP_PRINT("minute is changed\n");
	/**
	 *遍历 uttMinChgHookHead
	 */
	if (!list_empty(&uttMinChgHookHead)) {
	    list_for_each_entry_safe(pos,n,&uttMinChgHookHead,list){
		pos->fn(uttDateTimeNow,pos->askerData);
	    }
	}

	/**
	 *小时发生了变化
	 *
	 * 注意：如果是手动更改系统时间或者时钟服务器出问题，导致日期出问题，也要执行
	 */
	if ((uttDateTimeNow->time % 60 == 0) || ((uttDateTimeNow->date != uttDateTimeOld->date) || (uttDateTimeNow->time/60 != uttDateTimeOld->time/60))) {
	    TIMEGRP_PRINT("hour is changed\n");
	    /**
	     *遍历 uttHourChgHookHead
	     */
	    if (!list_empty(&uttHourChgHookHead)) {
		list_for_each_entry_safe(pos,n,&uttHourChgHookHead,list){
		    pos->fn(uttDateTimeNow,pos->askerData);
		}
	    }

	    if (uttDateTimeNow->date != uttDateTimeOld->date) {/*天发生了变化*/
		TIMEGRP_PRINT("week/day is changed\n");
		/**
		 *遍历 uttDayChgHookHead
		 */
		if (!list_empty(&uttDayChgHookHead)) {
		    list_for_each_entry_safe(pos,n,&uttDayChgHookHead,list){
			pos->fn(uttDateTimeNow,pos->askerData);
		    }
		}

	    }
	}

	/*将当前时间节点设置为oldTime*/
	setUttDTimeOld(uttDateTimeNow);
    }
    traverseTimeWaitList();

    timer_open();/*每隔一定时间重新开启定时器*/
}

/********************************************************************
* 函数： set_current_time
* 功能： 获取当前系统时间，结果存放在uttDTimeNow变量中
* 创建： 2014-09-11
* 参数： 无
* 返回： 无
* 作者： zhao.chong
 ********************************************************************/
static void set_current_time(void)
{
    unsigned long curseconds;
    int index;
    UttDateTime uttDateTime;
    memset(&uttDateTime,0,sizeof(uttDateTime));

    curseconds = get_seconds();
    uttDateTime.date = curseconds / SECONDS_OF_DAY;/*设置天*/
    uttDateTime.time = (curseconds % SECONDS_OF_DAY) / SECONDS_OF_MINUTE;/*设置时分,一天中从0点开始到现在经历了分钟数*/
    index =  ((curseconds/SECONDS_OF_DAY + 3) % 7u);/*1970-1-1是星期四*/
    TIMEGRP_PRINT("func:%s---index=%d\n",__func__,index);
    uttDateTime.week = 1<<(6-index);/*设置星期*/
    TIMEGRP_PRINT("func:%s---curseconds=%lu\n",__func__,curseconds);
    TIMEGRP_PRINT("func:%s---date=%u\n",__func__,uttDateTime.date);
    TIMEGRP_PRINT("func:%s---time=%u\n",__func__,uttDateTime.time);
    TIMEGRP_PRINT("func:%s---week=%u\n",__func__,uttDateTime.week);

    setUttDTimeNow(&uttDateTime);/*设置当前时间*/
}
/********************************************************************
* 函数： timer_open
* 功能： 开启一个定时器
* 创建： 2014-09-11
* 参数： 无
* 返回： 无
* 作者： zhao.chong
 ********************************************************************/
static void timer_open(void)
{
    timer_set.function = timer_task;/*设置定时器处理函数*/
    timer_set.expires = jiffies + HZ*TIME_SET_UP_INTERVAL;/*处理函数TIME_SET_UP_INTERVAL秒后运行*/ 
    add_timer(&timer_set);/*添加定时器*/
}

/********************************************************************
* 函数： uttRegDTimeChgHook
* 功能： 注册时间变化挂钩函数
* 创建： 2014-09-11
* 参数： 无
* 返回： 无
* 说明：一般情况下同一个hookHead链表中不能有一个以上的hook节点包含一样的askerData(fn做为扩展使用)
* 作者： zhao.chong
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
     *hook的注册和注销操作在下半部中进行，所以kmalloc的参数gfp_flags必须为GFP_ATOMIC
     */
    ops = kmalloc(sizeof(*ops), GFP_ATOMIC);
    if (ops == NULL) {
	return 1;
    }

    ops->fn = fn;
    ops->askerData = askerData;
    list_add(&ops->list,hookHead);
    /*挂上就执行一次.否则可能跳过时间变化的触发时机*/
    if(fn != NULL) {
	fn(getUttDTimeNow(),askerData);
    }

    return 0;
}

/********************************************************************
* 函数： uttUnRegDTimeChgHook
* 功能： 注销时间变化挂钩函数
* 创建： 2014-09-11
* 参数： 无
* 返回： 无
* 说明： 查找指定的HOOK节点，找到后移除(fn做为扩展使用)
* 作者： zhao.chong
 ********************************************************************/
void uttUnRegDTimeChgHook (struct list_head *hookHead, void *askerData, uttDTimeChgFn *fn)
{
    UttDateTimeChgHookOps *pos,*n;

    TIMEGRP_PRINT("func:%s--line:%d\n",__func__,__LINE__);
    if(askerData!=NULL) {
	/**
	 *根据askerData查找hookHead链表；
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
* 函数： getTimeSetIndex
* 功能： 根据时间组名称查找索引
* 创建： 2014-09-12
* 修改： 2015-01-05
* 参数： 时间组名称
* 返回： 成功->返回uttTimeSetList对应的索引；失败->返回uttTimeSetMax
* 作者： zhao.chong
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
* 函数： putTimeSetIndex
* 功能： 去掉对指定索引的时间组的引用
* 创建： 2015-01-05
* 参数： 时间组索引
* 返回： 无
* 作者： zhao.chong
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
* 函数： uttTimeSetCreat
* 功能： 时间组创建函数
* 创建： 2014-09-12
* 修改： 2015-01-05
* 参数： 时间组名称
* 返回： 创建成功->返回uttTimeSetList对应的索引；创建失败->返回uttTimeSetMax
* 作者： zhao.chong
* 说明： 若name不存在则创建，并挂载在uttTimeSetList上面，成功返回uttTimeSetList对应的索引。
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
    if (index != uttTimeSetMax) {/*找到name对应的索引*/
	return index;
    }
    TIMEGRP_PRINT("func:%s--line:%d-\n",__func__,__LINE__);

    uttTimeSetLock();
    for (index = 0; index < uttTimeSetMax; index++) {
	if (uttTimeSetList[index] == NULL) {
	    break;
	}
    }
    if (index != uttTimeSetMax) {/*时间组未达到最大条目，可以添加*/
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
	    if ((timeSet->datestart == 1) && (timeSet->datestop == 2)) {/*永久生效*/
		timeSet->enable = 0x7;
	    } else {
		for (i = 0; i < TIMEGRP_ENTRYS; i++) {
		    if (timeGrp->timeEntry[i].enable) {/*启用该时间单元*/
			weekTime = kzalloc(sizeof(UttTimeSetWeekTime),GFP_ATOMIC);
			if (weekTime != NULL) {
			    weekTime->week = timeGrp->timeEntry[i].day;
			    if ((timeGrp->timeEntry[i].timestart == 1) && (timeGrp->timeEntry[i].timestart == 86401)) {/*全天生效*/
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
* 函数： uttTimeSetModify
* 功能： 时间组修改(更新)函数
* 创建： 2014-09-16
* 修改： 2015-01-05
* 参数： 时间组Profile
* 返回： 修改成功->返回uttTimeSetList对应的索引；修改失败->返回uttTimeSetMax
* 作者： zhao.chong
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
    if (index != uttTimeSetMax) {/*找到name对应的索引*/
	uttTimeSetLock();
	timeSet = uttTimeSetList[index];
	write_lock(&timeSet->lock);
	uttTimeSetWeekTimeFlush(timeSet);/*先删除时间单元*/
	if (atomic_read(&timeSet->ref) == 0) {/*如果该时间单元没有被用户模块引用的话*/
	    /*构造修改时间组timeSet*/
	    timeSet->datestart = timeGrp->datestart;
	    timeSet->datestop = timeGrp->datestop;
	    timeSet->isOnTimeChangeList = false;
	    if ((timeSet->datestart == 1) && (timeSet->datestop == 2)) {/*永久生效*/
		timeSet->enable = 0x7;
	    } else {
		timeSet->enable = 0;
		for (i = 0; i < TIMEGRP_ENTRYS; i++) {/*再添加时间单元*/
		    if (timeGrp->timeEntry[i].enable) {/*启用该时间单元*/
			weekTime = kzalloc(sizeof(UttTimeSetWeekTime),GFP_ATOMIC);
			if (weekTime != NULL) {
			    weekTime->week = timeGrp->timeEntry[i].day;
			    if ((timeGrp->timeEntry[i].timestart == 1) && (timeGrp->timeEntry[i].timestart == 86401)) {/*全天生效*/
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
	     *判断时间组是否在等待时间链表，若是，则从等待链表去除
	     */
	    delFromWaitList(timeSet);

	    if (timeSet->enable == 0x7) {/*此时时间组生效*/
		if (!list_empty(&timeSet->askerHookHead)) {
		    list_for_each_entry_safe(ops,next,&timeSet->askerHookHead,list){
			ops->fn(0,ops->askerData);
		    }
		}
	    }
	    if (!list_empty(&timeSet->delayExecList)) {/*未执行的链表*/
		list_for_each_entry_safe(ops,next,&timeSet->delayExecList,list){
		    list_del(&ops->list);
		    list_add(&ops->list,&timeSet->askerHookHead);
		}
	    }

	    if (timeSet->isOnTimeChangeList) {/*时间组已经在时间变化链表*/
		uttUnRegTimeSetFromDTChg(timeSet);/*先从时间变化链表上注销该时间组*/
	    }
	}

	/*开启构造修改时间组timeSet*/
	timeSet->datestart = timeGrp->datestart;
	timeSet->datestop = timeGrp->datestop;

	if ((timeSet->datestart == 1) && (timeSet->datestop == 2)) {/*永久生效*/
	    timeSet->enable = 0x7;/*置为生效*/
	    if (!list_empty(&timeSet->askerHookHead)) {
		list_for_each_entry_safe(ops,next,&timeSet->askerHookHead,list){
		    list_del(&ops->list);
		    list_add(&ops->list,&timeSet->delayExecList);
		}
	    }
	} else {
	    timeSet->enable = 0;
	    for (i = 0; i < TIMEGRP_ENTRYS; i++) {/*再添加时间单元*/
		if (timeGrp->timeEntry[i].enable) {/*启用该时间单元*/
		    weekTime = kzalloc(sizeof(UttTimeSetWeekTime),GFP_ATOMIC);
		    if (weekTime != NULL) {
			weekTime->week = timeGrp->timeEntry[i].day;
			if ((timeGrp->timeEntry[i].timestart == 1) && (timeGrp->timeEntry[i].timestart == 86401)) {/*全天生效*/
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

	uttRegTimeSetToDTChg(timeSet);/*将该时间组注册到时间变化链表上*/
	uttTimeSetUnlock();
    }

    return index; 
}
EXPORT_SYMBOL(uttTimeSetModify);

/********************************************************************
* 函数： uttTimeSetDelByIndex
* 功能： 时间组删除函数
* 创建： 2014-09-18
* 参数： 时间组索引
* 返回： 无
* 作者： zhao.chong
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
	    if (atomic_read(&timeSet->ref) == 0) {/*没有模块引用该时间组*/
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
* 函数： uttTimeSetDel
* 功能： 时间组删除函数
* 创建： 2014-09-18
* 参数： 时间组Profile
* 返回： 被删除的时间组索引
* 作者： zhao.chong
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
* 函数： uttTimeSetDelAll
* 功能： 删除所有时间组函数
* 创建： 2014-09-18
* 参数： 无
* 返回： 无
* 作者： zhao.chong
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
 * 函数： uttTimeSetWeekTimeFlush
 * 功能： 时间组的时间单元销毁函数
 * 创建： 2014-09-12
 * 参数： 时间组
 * 返回： 0->修改成功；非0->修改失败
 * 作者： zhao.chong
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
* 函数： uttTimeSetMinChg
* 功能： 时间组分钟变化执行函数
* 创建： 2014-09-12
* 参数： @timeNow-当前时间；@conf-时间组
* 返回： 0->成功；非0->失败
* 作者： zhao.chong
 ********************************************************************/
static int uttTimeSetMinChg (const UttDateTime*timeNow, void *conf)
{
    UttTimeSet *timeSet = (UttTimeSet*)conf;
    UttTimeSetWeekTime *pos,*n;
    UttTimeSetAskerHookOps *ops,*next;
    uint16_t flag = 0U;/*分钟是否生效标志*/
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
	if (!list_empty_careful(&timeSet->delayExecList)) {/*该时间组存在待马上执行的模块*/ 
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
	list_for_each_entry_safe(pos,n,&timeSet->weekTimeList,list){/*遍历时间单元*/
	    if (timeNow->week & pos->week) {/*当天*/
		if (pos->timestart/SECONDS_OF_MINUTE < pos->timestop/SECONDS_OF_MINUTE) {
		    if ((timeNow->time >= pos->timestart/SECONDS_OF_MINUTE) &&
			(pos->timestop/SECONDS_OF_MINUTE >= timeNow->time )) {/*分钟生效*/
			flag = 1U;
			break;
		    }
		} else {
		    if ((timeNow->time >= pos->timestart/SECONDS_OF_MINUTE) || (timeNow->time <= pos->timestop/SECONDS_OF_MINUTE)) {/*分钟生效*/
			flag = 1U;
			break;
		    }
		}
	    }
	}
    }
    if (flag == 1U) {
	timeSet->enable |= 0x1;/*分生效置位*/
    } else {
	timeSet->enable &= ~0x1;
    }
    TIMEGRP_PRINT("func:%s--line:%d,oldEnable=%d,enableNow=%d\n",__func__,__LINE__,oldEnable,timeSet->enable);
    if ((oldEnable & 0x1) == 0) {/*之前不生效*/
	if ((timeSet->enable & 0x1) == 0x1) {/*由不生效变为生效*/
	    list_for_each_entry_safe(ops,next,&timeSet->askerHookHead,list){
		if (ops->fn != NULL) {
		    ops->fn(timeSet->enable == 0x7,ops->askerData);
		}
	    }
	}
    } else {/*之前生效*/
	if ((timeSet->enable & 0x1) == 0) {/*由生效变为不生效*/
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
* 函数： uttTimeSetHourChg
* 功能： 时间组小时变化执行函数
* 创建： 2014-09-15
* 参数： @timeNow-当前时间；@conf-时间组
* 返回： 1->失败；0->成功
* 作者： zhao.chong
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
	if (!list_empty(&timeSet->weekTimeList)) {/*存在生效的时间单元*/
	    list_for_each_entry_safe(pos,n,&timeSet->weekTimeList,list){
		if (timeNow->week & pos->week) {/*当天*/
		    /*只有在某一时间段的两端的整点才注册分钟变化钩子*/
		    if ((timeNow->time/MINUTE_OF_HOUR== pos->timestart/SECONDS_OF_HOUR) ||
			    (timeNow->time/MINUTE_OF_HOUR == pos->timestop/SECONDS_OF_HOUR)) {
			flag |= 0x1;/*三个时间段中只要有一个存在整点*/
		    } else {/*非整点*/
			/*起始时间小于结束时间*/
			if (pos->timestart/SECONDS_OF_HOUR < pos->timestop/SECONDS_OF_HOUR) {
			    if ((timeNow->time/MINUTE_OF_HOUR >= pos->timestart/SECONDS_OF_HOUR) &&
				(timeNow->time/MINUTE_OF_HOUR <= pos->timestop/SECONDS_OF_HOUR)) {
				flag |= 0x1<<1;/*当前时间在时间段内*/
			    } else {
				flag |= 0x1<<2;/*当前时间不在时间段内*/
			    }
			} else {
			    if ((timeNow->time/MINUTE_OF_HOUR <= pos->timestop/SECONDS_OF_HOUR) ||
				(timeNow->time/MINUTE_OF_HOUR >= pos->timestart/SECONDS_OF_HOUR)) {
				flag |= 0x1<<1;/*当前时间在时间段内*/
			    } else {
				flag |= 0x1<<2;/*当前时间不在时间段内*/
			    }
			}
		    }
		}
	    }
	}
	if (flag & 0x1) {/*至少有一个时间段处在整点位置*/
	    if ((oldEnable & 0x2) == 0) { /*原时变化不生效*/
		timeSet->enable |= 0x2;/*小时置为生效*/
	    }
	    write_unlock(&timeSet->lock);
	    uttRegMinChgHook(timeSet,uttTimeSetMinChg);/*注册分钟变化钩子*/
	    write_lock(&timeSet->lock);
	} else {
	    if (flag & 0x2) {/*时间段的整点之间*/
		timeSet->enable |= 0x2;/*时置为生效*/
	    } else {/*当前时间不在时间组的时间段内*/
		timeSet->enable &= ~0x2;/*时置为失效*/
	    }

	    if ((oldEnable & 0x2) != 0) { /*原时变化生效,分变化链表上存在该时间组会注销*/
		uttUnRegMinChgHook(timeSet,uttTimeSetMinChg);
		TIMEGRP_PRINT("fun:%s,line:%d,timeSet->enable=%d,(timeSet->enable & 0x2) = %d,(timeSet->enable & 0x1)=%d\n",__func__,__LINE__,timeSet->enable,(timeSet->enable & 0x2),(timeSet->enable & 0x1));
		if (((timeSet->enable & 0x2) == 0) && ((timeSet->enable & 0x1) == 0x1)) {/*小时由生效变为不生效,且分钟bit位还生效*/
		    timeSet->enable &= ~0x1;/*分钟有效位置0，并且执行引用该时间组的模块*/
		    list_for_each_entry_safe(ops,next,&timeSet->askerHookHead,list){
			if (ops->fn != NULL) {
			    ops->fn(timeSet->enable == 0x7,ops->askerData);
			}
		    }
		}
	    } else {
		if ((timeSet->enable & 0x2) != 0) {/*时由不生效到生效(该时间组首次挂载时)*/
		    write_unlock(&timeSet->lock);
		    uttRegMinChgHook(timeSet,uttTimeSetMinChg);/*注册分钟变化钩子*/
		    write_lock(&timeSet->lock);
		}
	    }
	}
	write_unlock(&timeSet->lock);
    }

    return 0;
}

/********************************************************************
* 函数： uttTimeSetDayChg
* 功能： 时间组天变化执行函数
* 创建： 2014-09-15
* 参数： @timeNow-当前时间；@conf-时间组
* 返回： 略 
* 作者： zhao.chong
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
		if (pos->week & timeNow->week) {/*时间单元中只要有一个满足，天均置为生效*/
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

	if ((oldWeekEnable == 0) && ((timeSet->enable & 0x4) > 0)) {/*天由不生效变为生效*/
	    write_unlock(&timeSet->lock);
	    uttRegHourChgHook(timeSet,uttTimeSetHourChg);/*注册小时变化钩子*/
	    write_lock(&timeSet->lock);
	} else if(oldWeekEnable > 0 &&  ((timeSet->enable & 0x4) == 0)) {/*天由生效变为不生效*/
	    uttUnRegHourChgHook(timeSet,uttTimeSetHourChg);/*注销小时变化的钩子*/
	}
	write_unlock(&timeSet->lock);
    } else {/*此处为注销时调用，需把小时相关的剔除掉*/
	TIMEGRP_PRINT("func:%s--line:%d\n",__func__,__LINE__);
	uttUnRegHourChgHook(timeSet,uttTimeSetHourChg);
    }

    return 0;
}

/********************************************************************
* 函数： uttRegTimeSetToDTChg
* 功能： 注册时间组到时间变化结构的函数
* 创建： 2014-09-12
* 修改： 2015-01-04
* 参数： 时间组
* 返回： 0->注册成功；非0->注册失败
* 作者： zhao.chong
 ********************************************************************/
int uttRegTimeSetToDTChg (void *timeSetConf)
{
    UttTimeSetAskerHookOps *pos, *n;
    unsigned char flag = 0U;
    int ret = 0;
    UttTimeSet *timeSet = (UttTimeSet *)timeSetConf;
    if (timeSet != NULL) {
	write_lock(&timeSet->lock);
	if (!list_empty_careful(&timeSet->delayExecList)) {/*该时间组存在待马上执行的模块*/
	    flag = 1U;/*时间组还没有挂载到时间变化链表就存在待马上执行的配置，说明是永久生效配置*/
	    list_for_each_entry_safe(pos,n,&timeSet->delayExecList,list) {/*遍历并执行已经挂载到该时间组的用户模块*/
		if (pos->fn != NULL) {
		    pos->fn(1,pos->askerData);
		}
		list_del(&pos->list);
		list_add(&pos->list,&timeSet->askerHookHead);
	    }
	}
	write_unlock(&timeSet->lock);
    }

    if (flag == 0U) {/*非永久生效配置*/
	ret = uttRegDayChgHook((void*)timeSet,uttTimeSetDayChg);
	write_lock(&timeSet->lock);
	timeSet->isOnTimeChangeList = true;
	write_unlock(&timeSet->lock);
    }
    return ret;
}
EXPORT_SYMBOL(uttRegTimeSetToDTChg);

/********************************************************************
* 函数： uttUnRegTimeSetFromDTChg
* 功能： 注销时间组在时间变化结构的函数
* 创建： 2014-09-15
* 参数： 时间组
* 返回： 无
* 作者： zhao.chong
 ********************************************************************/
void uttUnRegTimeSetFromDTChg (UttTimeSet *timeSet)
{
    /*day注销时会自动注销掉小时和分钟*/
    uttUnRegDayChgHook(timeSet,uttTimeSetDayChg);
}
EXPORT_SYMBOL(uttUnRegTimeSetFromDTChg);

/********************************************************************
* 函数： addToTimeSetWaitList
* 功能： 将应用模块加入等待链表，定时器执行函数开始执行时会将等待链表中的数据加入到时间变化链表中
* 创建： 2015-01-04
* 参数： @conf-还没有挂载到时间变化链表上的时间组（时间组已经存在引用的用户模块）；
*	 @fn-该函数将时间组conf挂载到时间变化链表中
* 返回： 返回结果表示是否失败；false->成功；true->失败
* 作者： zhao.chong
* 说明： 如果一个模块所引用的时间组发生变化，则在调用此函数之前应该先调用模块卸载函数
 ********************************************************************/
bool addToTimeSetWaitList(void *conf,uttTimeToDTChg *fn)
{
    bool isErr = false,exist = false;
    UttTimeWaitNode *pos;

    TIMEGRP_PRINT("----file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);
    uttTimeWaitListLock();
    if (list_empty(&timeWaitList)) {/*没有模块引用该时间组*/ 
	list_for_each_entry(pos,&timeWaitList,waitNode) {/*遍历等待链表,判断是否已经存在*/
	    if (pos->conf == conf) {/*已经存在*/
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
* 函数： delFromWaitList
* 功能： 将时间配置从等待链表中去除
* 创建： 2015-01-05
* 参数： @conf-时间配置；
* 返回： 无
* 作者： zhao.chong
* 说明： 略
 ********************************************************************/
void delFromWaitList(void *conf)
{
    UttTimeWaitNode *pos,*n;

    uttTimeWaitListLock();
    if (!list_empty_careful(&timeWaitList)) {
	list_for_each_entry_safe(pos,n,&timeWaitList,waitNode){/*遍历时间单元*/
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
 * 函数： uttRegToTimeSet
 * 功能： 将应用模块reg注册到index时间组
 * 创建： 2014-09-26
 * 参数： @index被引用的时间组索引；@askerData-引用index时间组的模块；@fn-模块执行函数
 * 返回： true->注册成功；false->注册失败
 * 作者： zhao.chong
 * 说明： 1.如果一个模块所引用的时间组发生变化，则在调用此函数之前应该先调用模块卸载函数;
 *	  2.该函数不会立即将时间组挂载到时间变化链表
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
	if (!list_empty(&timeSet->askerHookHead)) {/*该时间组存在挂载的模块*/
	    list_for_each_entry_safe(pos,n,&timeSet->askerHookHead,list) {/*遍历判断该模块是否已经挂载到了该时间组*/
		if (pos->askerData == askerData) {/*该用户配置已经存在*/
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
		if (timeSet->isOnTimeChangeList) {/*该时间组已经存在时间变化链表中*/
		    if (timeSet->enable == 0x7) {/*如果此时生效，将用户配置加入到延迟执行链表中*/
			list_add(&reg->list,&timeSet->delayExecList);
		    } else {/*如果此时不生效直接挂在到askerHookHead上等待时间生效执行*/
			list_add(&reg->list,&timeSet->askerHookHead);
		    }
		} else {
		    if (timeSet->enable == 0x7) {/*如果不在时间变化链表中，时间仍然生效，则说明是永久生效时间*/
			list_add(&reg->list,&timeSet->delayExecList);
		    } else {
			list_add(&reg->list,&timeSet->askerHookHead);
		    }
		    if ((isErr = addToTimeSetWaitList(timeSet,uttRegTimeSetToDTChg)) == true) {/*加入等待列表失败*/
			list_del(&reg->list);
			kfree(reg);
		    }
		}
		if (!isErr) {
		    /*每新添加一个模块引用时间段，则引用计数加1*/
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
* 函数： uttUnRegFromTimeSet
* 功能： 将应用模块reg从index时间组注销
* 创建： 2014-09-26
* 修改： 2015-01-05
* 参数： @index被引用的时间组索引；@askerData-引用index时间组的模块
* 返回： 0->注销成功；非0->注销失败
* 作者： zhao.chong
* 说明： 在将用户配置从其引用的时间组上卸载前，需先将该时间组的引用计数减1
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
	if ((atomic_read(&timeSet->ref) > 0) && (atomic_dec_return(&timeSet->ref) == 0)) {/*该时间组已经没有被任何模块引用*/
	    if (timeSet->isOnTimeChangeList) {/*时间组在时间变化链表上*/
		uttUnRegTimeSetFromDTChg(timeSet);
		timeSet->isOnTimeChangeList = false;
		timeSet->enable = 0;
	    } else {
		/*时间组如果在等待链表上，则从等待链表中删除*/
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
* 函数： uttTimeSetRefCout
* 功能： 时间组引用计数变化
* 创建： 2014-09-26
* 参数： @index被引用的时间组索引；@optType：0-时间组引用计数减1,1-时间组引用计数加1
* 返回： 0->操作成功；非0->操作失败
* 作者： zhao.chong
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
* 函数： time_set_init
* 功能： 时间组模块加载初始化函数
* 创建： 2014-09-11
* 参数： 无
* 返回： 0->成功加载；非0->加载模块失败
* 作者： zhao.chong
 ********************************************************************/
static int __init time_set_init (void)
{
    TIMEGRP_PRINT("func:%s,Line %d---start\n",__func__,__LINE__);
    /*初始化各时间变化钩子链表*/
    INIT_LIST_HEAD(&uttDayChgHookHead);
    INIT_LIST_HEAD(&uttHourChgHookHead);
    INIT_LIST_HEAD(&uttMinChgHookHead);
    INIT_LIST_HEAD(&timeWaitList);

    init_timer(&timer_set);/*初始化定时器*/ 
    timer_open();/*开启定时器*/

    if (maxTimeSets) {/*加载模块时动态指定个数*/
	uttTimeSetMax = maxTimeSets;
    }
    uttTimeSetList = kzalloc(sizeof(UttTimeSet *) * uttTimeSetMax,GFP_ATOMIC);

    TIMEGRP_PRINT("func:%s,Line %d---end\n",__func__,__LINE__);
    return 0;
}

/********************************************************************
* 函数： time_set_exit
* 功能： 时间组模块卸载函数
* 创建： 2013-09-22
* 参数： 无
* 返回： 无
* 作者： zhao.chong
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
