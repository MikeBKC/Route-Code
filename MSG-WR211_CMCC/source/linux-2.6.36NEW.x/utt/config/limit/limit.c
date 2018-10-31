#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/netfilter_ipv4/utt_filter_kernel.h>
#include <linux/netfilter_ipv4/utt_module_init.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4/utt_netfilter.h>
#include <linux/ip.h>
#include <linux/netfilter_ipv4/ip_set.h>
#include <linux/netfilter_ipv4/utt_effective_time.h>

#include "mibShare.h"
#include "profacceShare.h"

#if 0
#define LIMIT_PRINT(fmt, args...)		printk(fmt, ## args)
#else
#define LIMIT_PRINT(fmt, args...)
#endif
extern void registerForProfileUpdates(MibProfileType mibType,Pointer updateTag,ProfileUpdateFnc	updateFnc);
/*精细化限速的每个实例都对应于挂载在uttLimitConfListHead链表上的一个UttLimitConf对象*/
static LIST_HEAD(uttLimitConfListHead);
static DEFINE_RWLOCK(limit_conf_lock);/*限速配置链表的锁*/
static atomic_t limitConfCount = ATOMIC_INIT(0);
static atomic_t limitConfOnTimeCount = ATOMIC_INIT(0);

static int uttLimitOnEffectiveTime (uint32_t enable, void *limitModuleData);
static void uttTraverTimeValidList(UttNfHashBuf *ipHash);
static unsigned int uttNfLimit (UTT_NF_HOOK_FN_ARGS);
static uint32_t convertTimeToSec(char *time);
static void getEffectTiveTime (UttEffectiveTime *effectiveTime, const GroupBaseProfile* prof);
static UttLimitConf* uttLimitConfCreate(const char *instName);
static int isLimitConfExist(UttLimitConf *limitConf, struct list_head *listHead);
static void addToList(UttLimitConf *limitConf);
static void uttLimitConfDel(UttLimitConf *limitConf);
static void limitConfDelAll(void);
static UttLimitConf* getLimitConf (const char instName[]);
static void put_all_ipset_index(void (*put_byindex)(ip_set_id_t index));
static void uttTraverTimeValidList(UttNfHashBuf *ipHash);
static void uttLimitIpNodeInit (UttNfHashBuf *ipHash);
static void uttLimitIpNodeExit (UttNfHashBuf *ipHash);

/**
 *定义精细化限速功能模块netfilter钩子
 */
static uttNfHookOps limitHook = {
    .pf =  UTT_PF_INET, /*IPV4*/
    .dir = UTT_NF_IP_FORWARD,
    .table = UTT_NF_IPT_MANGLE,
    .condition = {
	0,0
    },
    .fn = uttNfLimit,/*执行的函数*/
    .priority = UTT_NF_PRI_LIMIT,/*优先级。越小越先执行*/
};

/**
 *定义精细化限速功能开启时，模块初始化(utt_module_init.c文件)所需的结构
 */
static UttIpNodeOps uttLimitIpNodeOps = {
    .init = uttLimitIpNodeInit,
    .exit = uttLimitIpNodeExit
};

/*精细化限速主函数*/
static unsigned int uttNfLimit (UTT_NF_HOOK_FN_ARGS)
{
    int ret = UTT_IPT_CONTINUE;
    UttLimitConf *limitConf;
    UttNfHashBuf *ipHash = (UttNfHashBuf*)((*pskb)->ipHashNode);
    struct sk_buff *skb = *pskb;

    if (ipHash == NULL) {
	return ret;
    }
#ifdef CONFIG_UTT_NF_PPPOE_LIMIT
    if (ipHash->limitGrpConf != NULL) {/*如果pppoe用户已经配置了限速，则跳过该模块*/
	return ret;
    }
#endif
    limitConf = ipHash->limitConf;

    if (limitConf != NULL) {
	write_lock(&limitConf->lock);
	LIMIT_PRINT("----file:%s,func:%s,line:%d-,limitConf->name=%s----\n",__FILE__,__func__,__LINE__, limitConf->name);
	/**
	 *真正限速的代码在此处理
	 */
	if (isSkbComeFromLan(skb)){
	    LIMIT_PRINT("----file:%s,func:%s,line:%d---****upload****--\n",__FILE__,__func__,__LINE__);
	    /*上传*/
	    if (limitConf->uRate != 0) {/*上传限速*/
		if(limitConf->limitPolicy == 1) {/*独享*/
		    ret = utt_up_ratelimit(&(ipHash->runStat), limitConf, skb->len);
		} else {
		    ret = utt_up_ratelimit(&(limitConf->runStat), limitConf, skb->len);
		}
	    } 
	} else {
	    LIMIT_PRINT("----file:%s,func:%s,line:%d---****download****--\n",__FILE__,__func__,__LINE__);
	    /*下载*/
	    if (limitConf->dRate != 0) {/*下载限速*/
		if(limitConf->limitPolicy == 1) {
		    ret = utt_down_ratelimit(&(ipHash->runStat), limitConf, skb->len);
		} else {
		    ret = utt_down_ratelimit(&(limitConf->runStat), limitConf, skb->len);
		}
	    }
	}

	write_unlock(&limitConf->lock);
    } else {
	LIMIT_PRINT("----file:%s,func:%s,line:%d-,limitConf is null----\n",__FILE__,__func__,__LINE__);
    }

    return ret;
}

/**
 * 把时间转换为 秒
 */
static uint32_t convertTimeToSec(char *time)
{
    uint32_t hour = 0u, mintue = 0u, second = 0u;
    sscanf(time, "%u:%u", &hour, &mintue);/*读入时分*/
    second =  (hour*60 + mintue) *60;
    return second;

}
static void getEffectTiveTime (UttEffectiveTime *effectiveTime, const GroupBaseProfile* prof)
{

    LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
    effectiveTime->wday = prof->day;
    if(strcmp(prof->timeStart, "All") == 0) {
	effectiveTime->timestart = 0U;
	effectiveTime->timestop = convertTimeToSec("23:59");
    } else {
	/*转换为秒*/
	effectiveTime->timestart = convertTimeToSec((char*)(prof->timeStart));
	effectiveTime->timestop = convertTimeToSec((char*)(prof->timeStop));
    }
}

static UttLimitConf* uttLimitConfCreate(const char *instName)
{
    UttLimitConf *limitConf = (UttLimitConf *)kzalloc(sizeof(UttLimitConf),GFP_KERNEL);

    LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
    if (limitConf != NULL) {
	INIT_LIST_HEAD(&limitConf->list);
	rwlock_init(&limitConf->lock);
	strncpy(limitConf->name,instName,UTT_INST_NAME_LEN);
    }

    return limitConf;
}

static int isLimitConfExist(UttLimitConf *limitConf, struct list_head *listHead) 
{
    UttLimitConf *pos;
    int exist = 0;

    LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
    read_lock_bh(&limit_conf_lock);
    if (!list_empty(listHead)) {
	list_for_each_entry(pos,listHead,list){
	    if (pos == limitConf) {
		exist = 1;
		break;
	    }
	}
    }
    read_unlock_bh(&limit_conf_lock);
    LIMIT_PRINT("----file:%s,func:%s,line:%d---,exist=%d--\n",__FILE__,__func__,__LINE__,exist);

    return exist;
}

static void addToList(UttLimitConf *limitConf)
{
    UttLimitConf *pos;

    LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
    if (!isLimitConfExist(limitConf,&uttLimitConfListHead)) {
	LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
	write_lock_bh(&limit_conf_lock);
	list_for_each_entry(pos,&uttLimitConfListHead,list){
	    read_lock(&pos->lock);
	    if (limitConf->instIndex < pos->instIndex) {
		read_unlock(&pos->lock);
		break;
	    }
	    read_unlock(&pos->lock);
	}
	list_add(&limitConf->list,pos->list.prev);
	atomic_inc(&limitConfCount);
	write_unlock_bh(&limit_conf_lock);
    }
}

/**
 *根据名称删除精细化限速内核配置实例
 */
static void uttLimitConfDel(UttLimitConf *limitConf)
{
    LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
    if (limitConf != NULL) {
	write_lock_bh(&limit_conf_lock);
	list_del_init(&limitConf->list);
	/*释放对timeHookOps的引用*/
	uttPutTimeHookOpsRef(limitConf->timeHookOps);
	kfree(limitConf);
	limitConf = NULL;
	atomic_dec(&limitConfCount);
	write_unlock_bh(&limit_conf_lock);
    }
}

/**
 * 删除所有链表上的配置
 */
static void limitConfDelAll(void)
{
    UttLimitConf *limitConf = NULL, *n;

    LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
    write_lock_bh(&limit_conf_lock);
    if (!list_empty_careful(&uttLimitConfListHead)) {
	list_for_each_entry_safe (limitConf,n,&uttLimitConfListHead,list) {
	    list_del_init(&limitConf->list);
	    /*释放对timeHookOps的引用*/
	    uttPutTimeHookOpsRef(limitConf->timeHookOps);
	    kfree(limitConf);
	    limitConf = NULL;
	}
    }
    atomic_set(&limitConfCount,0);/*配置记录数清零*/
    write_unlock_bh(&limit_conf_lock);
}

/**
 *根据名称查找精细化限速内核配置实例
 */
static UttLimitConf* getLimitConf (const char instName[]) 
{
    UttLimitConf *limitConf = NULL;
    unsigned int found = 0U;

    LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
    read_lock_bh(&limit_conf_lock);
    if (!list_empty(&uttLimitConfListHead)) {
	list_for_each_entry(limitConf,&uttLimitConfListHead,list){
	    read_lock(&limitConf->lock);
	    if (strcmp(limitConf->name,instName) == 0) {
		found = 1U;
		read_unlock(&limitConf->lock);
		break;
	    }
	    read_unlock(&limitConf->lock);
	}
    }
    read_unlock_bh(&limit_conf_lock);

    return (found ? limitConf : NULL);
}

static void put_all_ipset_index(void (*put_byindex)(ip_set_id_t index))
{
    ip_set_id_t ipsetIndex; 
    UttLimitConf *limitConf = NULL;

    LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
    /**
     *首先遍历时间生效配置的链表，释放所有引用到的ipset
     */
    read_lock_bh(&limit_conf_lock);
    if (!list_empty(&uttLimitConfListHead)) {
	list_for_each_entry(limitConf,&uttLimitConfListHead,list){
	    read_lock(&limitConf->lock);
	    if (limitConf->ipSrcGrpEntry.entryType == IP_GROUP) {
		read_unlock(&limitConf->lock);
		read_unlock_bh(&limit_conf_lock);
		ipsetIndex = limitConf->ipSrcGrpEntry.entryVal.index;
		if (ipsetIndex != CONFIG_ALL_USER) {
		    put_byindex(ipsetIndex);/*该函数可能会睡眠，所以调用该函数前必须打开所有自旋锁*/
		}
		read_lock_bh(&limit_conf_lock);
		read_lock(&limitConf->lock);
	    }
	    read_unlock(&limitConf->lock);
	}
    }
    read_unlock_bh(&limit_conf_lock);
}

/*遍历时间生效链表*/
static void uttTraverTimeValidList(UttNfHashBuf *ipHash)
{
    UttLimitConf *limitConf = NULL;

    IPNODE_MODULE_PRI_MATCH_DIS(ipHash->conf.uttNfMatch,UTT_NF_PRI_LIMIT);
    ipHash->limitConf = NULL;

    if (atomic_read(&limitConfOnTimeCount) > 0) {
	read_lock_bh(&limit_conf_lock);
	LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
	list_for_each_entry(limitConf,&uttLimitConfListHead,list){
	    read_lock(&limitConf->lock);
	    LIMIT_PRINT("----file:%s,func:%s,line:%d--limitConf->name=%s,ip=%lx---\n",__FILE__,__func__,__LINE__,limitConf->name,ipHash->ip);

#if (CONFIG_UTT_MAC_LIMIT == FYES)
	    if(limitConf->limitPolicy == 3)/*mac限速*/
	    {
		printk("xrg kernel get mac limit limitConf->ipSrcGrpEntry.entryVal.macAddr = %x,ipHash->conf.userStats.mac = %x,%d\n",__LINE__);
		if ((atomic_read(&limitConf->ref) > 0) /*该配置存在ip hash节点引用*/
			&& (memcmp(&(limitConf->ipSrcGrpEntry.entryVal.macAddr),&(ipHash->conf.userStats.mac),ETH_ALEN)==0)/*且skb中的mac匹配*/
			&& limitConf->isAvailable) {/*该配置可被hash节点引用*/
		    /*匹配到第一条记录，则ipHash节点的limit指针指向该记录
		     *ipHash->conf.uttMatch置位
		     */
		    IPNODE_MODULE_PRI_MATCH_EN(ipHash->conf.uttNfMatch,UTT_NF_PRI_LIMIT);
		    ipHash->limitConf = limitConf;
		    /*跳出循环前需要释放锁*/
		    read_unlock(&limitConf->lock);
		    break;
		}
	    }
	    else
#endif
	    {
		if ((atomic_read(&limitConf->ref) > 0) /*该配置存在ip hash节点引用*/
			&& uttIpMatch(&(limitConf->ipSrcGrpEntry),ipHash->ip)/*且skb中的ip匹配*/
			&& limitConf->isAvailable) {/*该配置可被hash节点引用*/
		    /*匹配到第一条记录，则ipHash节点的limit指针指向该记录
		     *ipHash->conf.uttMatch置位
		     */
		    IPNODE_MODULE_PRI_MATCH_EN(ipHash->conf.uttNfMatch,UTT_NF_PRI_LIMIT);
		    ipHash->limitConf = limitConf;
		    LIMIT_PRINT("----file:%s,func:%s,line:%d--limitConf->name=%s,ip=%lx---\n",__FILE__,__func__,__LINE__,limitConf->name,ipHash->ip);
		    /*跳出循环前需要释放锁*/
		    read_unlock(&limitConf->lock);
		    break;
		}
	    }
	    read_unlock(&limitConf->lock);
	}
	read_unlock_bh(&limit_conf_lock);
    }
}

/*一般ip上线时执行该函数*/
static void uttLimitIpNodeInit (UttNfHashBuf *ipHash)
{
    UttLimitConf *limitConf = NULL;

#ifdef CONFIG_UTT_NF_PPPOE_LIMIT
    if (ipHash->limitGrpConf != NULL) {/*如果pppoe用户已经配置了限速，则跳过该模块*/
	return ;
    }
#endif
    LIMIT_PRINT("----file:%s,func:%s,line:%d--,ipHash->ip=%lx---\n",__FILE__,__func__,__LINE__,ipHash->ip);
    if (atomic_read(&limitConfCount) > 0) {
	read_lock_bh(&limit_conf_lock);
	LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
	list_for_each_entry(limitConf,&uttLimitConfListHead,list){
	    write_lock(&limitConf->lock);
	    LIMIT_PRINT("----file:%s,func:%s,line:%d,atomic_read(&limitConf->ref)=%d--limitConf->name=%s---\n",__FILE__,__func__,__LINE__,atomic_read(&limitConf->ref),limitConf->name);
#if (CONFIG_UTT_MAC_LIMIT == FYES)	
	    if(limitConf->limitPolicy == 3)/*mac限速*/
	    {
		if(memcmp(&(limitConf->ipSrcGrpEntry.entryVal.macAddr),&(ipHash->conf.userStats.mac),ETH_ALEN)==0)
		{
		    if ((atomic_inc_return(&limitConf->ref) == 1)) {
			uttRegToEffTimeHook(limitConf->timeHookOps,limitConf,uttLimitOnEffectiveTime);
		    }
		}
	    }
	    else
#endif
	    {
		if (uttIpMatch(&(limitConf->ipSrcGrpEntry),ipHash->ip)){/*skb中的ip匹配配置的IP范围*/
		    LIMIT_PRINT("----file:%s,func:%s,line:%d--limitConf->name=%s-ip is match!--\n",__FILE__,__func__,__LINE__,limitConf->name);
		    /*所有IP匹配的配置条目的引用计数均加1,且在第一次被引用时注册到timeHook上*/
		    if ((atomic_inc_return(&limitConf->ref) == 1)) {
			uttRegToEffTimeHook(limitConf->timeHookOps,limitConf,uttLimitOnEffectiveTime);
		    }
		}
	    }
	    LIMIT_PRINT("----file:%s,func:%s,line:%d,atomic_read(&limitConf->ref)=%d--limitConf->name=%s---\n",__FILE__,__func__,__LINE__,atomic_read(&limitConf->ref),limitConf->name);
	    write_unlock(&limitConf->lock);
	}
	read_unlock_bh(&limit_conf_lock);
    }
    uttTraverTimeValidList(ipHash);
}

/**
 *一般IP下线执行
 */
static void uttLimitIpNodeExit (UttNfHashBuf *ipHash)
{
    UttLimitConf *limitConf = NULL,*n;

    LIMIT_PRINT("----file:%s,func:%s,line:%d--,ipHash->ip=%lx---\n",__FILE__,__func__,__LINE__,ipHash->ip);

    if (atomic_read(&limitConfCount) > 0) {
	read_lock_bh(&limit_conf_lock);
	list_for_each_entry_safe(limitConf,n,&uttLimitConfListHead,list){
	    LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
	    write_lock(&limitConf->lock);

#if (CONFIG_UTT_MAC_LIMIT == FYES)
	    if(limitConf->limitPolicy ==3)/*mac限速*/
	    {
		if ((atomic_read(&limitConf->ref) > 0) && (memcmp(&(limitConf->ipSrcGrpEntry.entryVal.macAddr),&(ipHash->conf.userStats.mac),ETH_ALEN)==0)) {/*skb中的mac匹配配置的mac*/
		    if (atomic_dec_and_test(&limitConf->ref)) {/*所有mac匹配的配置条目的引用计数均减1*/
			LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
			uttUnRegFromTimeHook(limitConf->timeHookOps, limitConf); 
		    }
		}
	    }
	    else
#endif
	    {

		if ((atomic_read(&limitConf->ref) > 0) && uttIpMatch(&(limitConf->ipSrcGrpEntry),ipHash->ip)) {/*skb中的ip匹配配置的IP范围*/
		    if (atomic_dec_and_test(&limitConf->ref)) {/*所有IP匹配的配置条目的引用计数均减1*/
			LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
			uttUnRegFromTimeHook(limitConf->timeHookOps, limitConf); 
		    }
		}
	    }
	    write_unlock(&limitConf->lock);
	    LIMIT_PRINT("----file:%s,func:%s,line:%d,atomic_read(&limitConf->ref)=%d--limitConf->name=%s---\n",__FILE__,__func__,__LINE__,atomic_read(&limitConf->ref),limitConf->name);
	}
	read_unlock_bh(&limit_conf_lock);
    }

}

/**
 *精细化限速模块定时器处理函数
 */
static int uttLimitOnEffectiveTime (uint32_t enable, void *limitModuleData)
{
    UttLimitConf *limitConf;
    limitConf = (UttLimitConf *)limitModuleData;

    LIMIT_PRINT("----file:%s,func:%s,line:%d-name=%s- enable=%u,atomic_read(&limitConfOnTimeCount)=%d----\n",__FILE__,__func__,__LINE__,limitConf->name,enable,atomic_read(&limitConfOnTimeCount));

    if (enable) {/*时间生效*/
	limitConf->isAvailable = true;
	/*生效的配置数+1*/
	if (atomic_inc_return(&limitConfOnTimeCount) == 1) {/*首次注册*/
	    uttNfRegisterHook(&limitHook);/*注册限速模块的钩子节点*/
	}

    } else {/*时间不生效*/
	limitConf->isAvailable = false;
	/*生效的配置数-1*/
	if (atomic_read(&limitConfOnTimeCount) > 0 && atomic_dec_and_test(&limitConfOnTimeCount)) {/*当生效配置为0时*/
	    uttNfUnregisterHook(&limitHook);/*注销限速模块的钩子节点*/
	}
    }
    LIMIT_PRINT("----file:%s,func:%s,line:%d-name=%s- enable=%u,atomic_read(&limitConfOnTimeCount)=%d----\n",__FILE__,__func__,__LINE__,limitConf->name,enable,atomic_read(&limitConfOnTimeCount));
    /**
     * 一旦链表发生变化，都会使得引用该条配置的hash节点受到影响
     * 所以在此处遍历IP hash表
     */
    uttNfHashTraverse(uttTraverTimeValidList);

    return 0;
}

static void GroupBaseProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const GroupBaseProfile* oldProfileDataPtr, const GroupBaseProfile* newProfileDataPtr)
{
    UttLimitConf *limitConf = NULL, *oldLimitConf = NULL;
    UttEffectiveTime effectiveTime;
    ip_set_id_t ipsetindex = IP_SET_INVALID_ID;
    ip_set_id_t oldIpsetIndex = IP_SET_INVALID_ID; 

    LIMIT_PRINT("----file:%s,func:%s,line:%d-,changeType=%d----\n",__FILE__,__func__,__LINE__,changeType);
    if (newProfileDataPtr->groupType != GBT_FINE_LIMIT) {/*如果不是精细化限速，立即返回*/
	return;
    }
    LIMIT_PRINT("----file:%s,func:%s,line:%d-,changeType=%d----\n",__FILE__,__func__,__LINE__,changeType);

    /**
     * 对于修改、删除操作，需要先将之前引用的ipsetindex进行释放
     */
    switch (changeType) {
	case PROF_CHANGE_DEL:
	    LIMIT_PRINT("----file:%s,func:%s,line:%d-,changeType=%d----\n",__FILE__,__func__,__LINE__,changeType);
	    oldLimitConf = getLimitConf(oldProfileDataPtr->head.name);
	if ((oldProfileDataPtr->addrTypeEn == IP_GROUP) && 
		(*(oldProfileDataPtr->ipGrpName) != '\0')) {
	    if (oldLimitConf != NULL) {
		read_lock_bh(&oldLimitConf->lock);
		LIMIT_PRINT("----file:%s,func:%s,line:%d-,oldLimitConf->name=%s----\n",__FILE__,__func__,__LINE__,oldLimitConf->name);
		oldIpsetIndex = oldLimitConf->ipSrcGrpEntry.entryVal.index;
		read_unlock_bh(&oldLimitConf->lock);
	    }

	    ip_set_put_byindex(oldIpsetIndex);/*释放对该ipset的引用*/
	}
    break;
    case PROF_CHANGE_DELALL:
    LIMIT_PRINT("----file:%s,func:%s,line:%d-,changeType=%d----\n",__FILE__,__func__,__LINE__,changeType);
    put_all_ipset_index(ip_set_put_byindex);
    break;
    default:
    break;
}
/**
 * 对于添加和修改操作，需要添加对新的ipsetindex的引用
 * 并生成新的生效时间结构
 */
if (changeType == PROF_CHANGE_ADD) {
    if (newProfileDataPtr->addrTypeEn == IP_GROUP) {
	if (*(newProfileDataPtr->ipGrpName) != '\0') {
	    ipsetindex = ip_set_get_byname((char *)newProfileDataPtr->ipGrpName);
	    LIMIT_PRINT("----file:%s,func:%s,line:%d--,ipsetName=%s,ipsetIndex=%d---\n",__FILE__,__func__,__LINE__,newProfileDataPtr->ipGrpName,ipsetindex);
	} else {
	    ipsetindex = CONFIG_ALL_USER;/*当配置所有用户生效时，用65534做权宜之计*/
	}
    }
}
switch(changeType) {
    case PROF_CHANGE_ADD:
	LIMIT_PRINT("----file:%s,func:%s,line:%d--name=%s,instIndex=%d\n",__FILE__,__func__,__LINE__,newProfileDataPtr->head.name,newProfileDataPtr->ProfIndex);
#if 1
	/*为了避免已有配置重复引用配置，导致配置的引用计数不准确，所以先注销IP Node节点*/
	if (atomic_read(&limitConfCount) > 0) {
	    uttUnRegIpNodeOps(&uttLimitIpNodeOps);/*注销IP 节点*/
	}
#endif
	limitConf = uttLimitConfCreate((const char *)newProfileDataPtr->head.name);
	limitConf->uRate = (newProfileDataPtr->uRate << 7)/HZ;
	limitConf->umaxCredit = (newProfileDataPtr->uRate << 7)/4;
	limitConf->dRate = (newProfileDataPtr->dRate << 7)/HZ;
	limitConf->dmaxCredit = (newProfileDataPtr->dRate << 7)/4;
	limitConf->limitPolicy = newProfileDataPtr->limitPolicy;
	limitConf->ipSrcGrpEntry.entryType = newProfileDataPtr->addrTypeEn;
	limitConf->instIndex = newProfileDataPtr->ProfIndex;
#if (CONFIG_UTT_MAC_LIMIT == FYES)
	if(limitConf->limitPolicy == 3)/*mac限速*/
	{	
	     memcpy(&(limitConf->ipSrcGrpEntry.entryVal.macAddr),&(newProfileDataPtr->mac_addr.addr),ETH_ALEN);
	}
	else
#endif
	{
	    if (limitConf->ipSrcGrpEntry.entryType == IP_RANGE) {
		limitConf->ipSrcGrpEntry.entryVal.range.ipFrom = ntohl(newProfileDataPtr->ipStart);
		limitConf->ipSrcGrpEntry.entryVal.range.ipTo = ntohl(newProfileDataPtr->ipEnd);
#if (IP_GRP == FYES) 
	    } else {
		/*当地址为地址组时，内核保存配置
		 * 将地址组名保存为ipset索引值*/
		strcpy(limitConf->ipSrcGrpEntry.entryVal.name,newProfileDataPtr->ipGrpName);
		limitConf->ipSrcGrpEntry.entryVal.index = ipsetindex;
#endif
	    }
	} 
	getEffectTiveTime(&effectiveTime,newProfileDataPtr);
	if ((limitConf->timeHookOps = uttGetTimeHookOpsRef(&effectiveTime)) != NULL) {
	    addToList(limitConf);
	    uttRegIpNodeOps(&uttLimitIpNodeOps);/*注册IP 节点*/
	} else {
#if (IP_GRP == FYES) 
	    if (newProfileDataPtr->addrTypeEn == IP_GROUP) {
		if (*(newProfileDataPtr->ipGrpName) != '\0') {
		    ip_set_put_byindex(ipsetindex);
		}
	    }
#endif
	    kfree(limitConf);
	    limitConf = NULL;
	}
	break;
    case PROF_CHANGE_DEL:
	LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
#if 1
	/*要删除的配置可能已经被IP hash节点引用，所以每次删除记录时，都先注销IP Node节点*/
	uttUnRegIpNodeOps(&uttLimitIpNodeOps);/*注销IP 节点*/
#endif
	/*将该配置从所在链表中卸载，并释放*/
	uttLimitConfDel(oldLimitConf);
#if 0
	if (atomic_read(&limitConfCount) == 0) {/*如果删除指定配置后，还存在其他配置*/
	    uttUnRegIpNodeOps(&uttLimitIpNodeOps);/*注销IP 节点*/
	}
#endif
#if 1
	if (atomic_read(&limitConfCount) > 0) {/*如果删除指定配置后，还存在其他配置*/
	    uttRegIpNodeOps(&uttLimitIpNodeOps);/*重新注册IP 节点*/
	}
#endif
	LIMIT_PRINT("----file:%s,func:%s,line:%d,atomic_read(limitConfCount)=%d-----\n",__FILE__,__func__,__LINE__,atomic_read(&limitConfCount));
	break;
    case PROF_CHANGE_DELALL:
	LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
	if (atomic_read(&limitConfCount) > 0) {/*链表中存在配置*/
	    LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
	    uttUnRegIpNodeOps(&uttLimitIpNodeOps);/*注销IP 节点*/
	    limitConfDelAll();
	}
	LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
	break;
    default:
	break;
}
LIMIT_PRINT("----file:%s,func:%s,line:%d-,atomic_read(&limitConfCount)=%d----\n",__FILE__,__func__,__LINE__,atomic_read(&limitConfCount));
}
static int __init limit_config_init(void)
{
    LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
    registerForProfileUpdates(MIB_PROF_GROUP_BASE, NULL, (ProfileUpdateFnc)GroupBaseProfileChange);
    return 0;
}

static void __exit limit_config_exit(void)
{
    LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
}
module_init(limit_config_init);
module_exit(limit_config_exit);
