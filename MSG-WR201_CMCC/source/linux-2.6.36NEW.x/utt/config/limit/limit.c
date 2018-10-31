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
/*��ϸ�����ٵ�ÿ��ʵ������Ӧ�ڹ�����uttLimitConfListHead�����ϵ�һ��UttLimitConf����*/
static LIST_HEAD(uttLimitConfListHead);
static DEFINE_RWLOCK(limit_conf_lock);/*���������������*/
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
 *���徫ϸ�����ٹ���ģ��netfilter����
 */
static uttNfHookOps limitHook = {
    .pf =  UTT_PF_INET, /*IPV4*/
    .dir = UTT_NF_IP_FORWARD,
    .table = UTT_NF_IPT_MANGLE,
    .condition = {
	0,0
    },
    .fn = uttNfLimit,/*ִ�еĺ���*/
    .priority = UTT_NF_PRI_LIMIT,/*���ȼ���ԽСԽ��ִ��*/
};

/**
 *���徫ϸ�����ٹ��ܿ���ʱ��ģ���ʼ��(utt_module_init.c�ļ�)����Ľṹ
 */
static UttIpNodeOps uttLimitIpNodeOps = {
    .init = uttLimitIpNodeInit,
    .exit = uttLimitIpNodeExit
};

/*��ϸ������������*/
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
    if (ipHash->limitGrpConf != NULL) {/*���pppoe�û��Ѿ����������٣���������ģ��*/
	return ret;
    }
#endif
    limitConf = ipHash->limitConf;

    if (limitConf != NULL) {
	write_lock(&limitConf->lock);
	LIMIT_PRINT("----file:%s,func:%s,line:%d-,limitConf->name=%s----\n",__FILE__,__func__,__LINE__, limitConf->name);
	/**
	 *�������ٵĴ����ڴ˴���
	 */
	if (isSkbComeFromLan(skb)){
	    LIMIT_PRINT("----file:%s,func:%s,line:%d---****upload****--\n",__FILE__,__func__,__LINE__);
	    /*�ϴ�*/
	    if (limitConf->uRate != 0) {/*�ϴ�����*/
		if(limitConf->limitPolicy == 1) {/*����*/
		    ret = utt_up_ratelimit(&(ipHash->runStat), limitConf, skb->len);
		} else {
		    ret = utt_up_ratelimit(&(limitConf->runStat), limitConf, skb->len);
		}
	    } 
	} else {
	    LIMIT_PRINT("----file:%s,func:%s,line:%d---****download****--\n",__FILE__,__func__,__LINE__);
	    /*����*/
	    if (limitConf->dRate != 0) {/*��������*/
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
 * ��ʱ��ת��Ϊ ��
 */
static uint32_t convertTimeToSec(char *time)
{
    uint32_t hour = 0u, mintue = 0u, second = 0u;
    sscanf(time, "%u:%u", &hour, &mintue);/*����ʱ��*/
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
	/*ת��Ϊ��*/
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
 *��������ɾ����ϸ�������ں�����ʵ��
 */
static void uttLimitConfDel(UttLimitConf *limitConf)
{
    LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
    if (limitConf != NULL) {
	write_lock_bh(&limit_conf_lock);
	list_del_init(&limitConf->list);
	/*�ͷŶ�timeHookOps������*/
	uttPutTimeHookOpsRef(limitConf->timeHookOps);
	kfree(limitConf);
	limitConf = NULL;
	atomic_dec(&limitConfCount);
	write_unlock_bh(&limit_conf_lock);
    }
}

/**
 * ɾ�����������ϵ�����
 */
static void limitConfDelAll(void)
{
    UttLimitConf *limitConf = NULL, *n;

    LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
    write_lock_bh(&limit_conf_lock);
    if (!list_empty_careful(&uttLimitConfListHead)) {
	list_for_each_entry_safe (limitConf,n,&uttLimitConfListHead,list) {
	    list_del_init(&limitConf->list);
	    /*�ͷŶ�timeHookOps������*/
	    uttPutTimeHookOpsRef(limitConf->timeHookOps);
	    kfree(limitConf);
	    limitConf = NULL;
	}
    }
    atomic_set(&limitConfCount,0);/*���ü�¼������*/
    write_unlock_bh(&limit_conf_lock);
}

/**
 *�������Ʋ��Ҿ�ϸ�������ں�����ʵ��
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
     *���ȱ���ʱ����Ч���õ������ͷ��������õ���ipset
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
		    put_byindex(ipsetIndex);/*�ú������ܻ�˯�ߣ����Ե��øú���ǰ���������������*/
		}
		read_lock_bh(&limit_conf_lock);
		read_lock(&limitConf->lock);
	    }
	    read_unlock(&limitConf->lock);
	}
    }
    read_unlock_bh(&limit_conf_lock);
}

/*����ʱ����Ч����*/
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
	    if(limitConf->limitPolicy == 3)/*mac����*/
	    {
		printk("xrg kernel get mac limit limitConf->ipSrcGrpEntry.entryVal.macAddr = %x,ipHash->conf.userStats.mac = %x,%d\n",__LINE__);
		if ((atomic_read(&limitConf->ref) > 0) /*�����ô���ip hash�ڵ�����*/
			&& (memcmp(&(limitConf->ipSrcGrpEntry.entryVal.macAddr),&(ipHash->conf.userStats.mac),ETH_ALEN)==0)/*��skb�е�macƥ��*/
			&& limitConf->isAvailable) {/*�����ÿɱ�hash�ڵ�����*/
		    /*ƥ�䵽��һ����¼����ipHash�ڵ��limitָ��ָ��ü�¼
		     *ipHash->conf.uttMatch��λ
		     */
		    IPNODE_MODULE_PRI_MATCH_EN(ipHash->conf.uttNfMatch,UTT_NF_PRI_LIMIT);
		    ipHash->limitConf = limitConf;
		    /*����ѭ��ǰ��Ҫ�ͷ���*/
		    read_unlock(&limitConf->lock);
		    break;
		}
	    }
	    else
#endif
	    {
		if ((atomic_read(&limitConf->ref) > 0) /*�����ô���ip hash�ڵ�����*/
			&& uttIpMatch(&(limitConf->ipSrcGrpEntry),ipHash->ip)/*��skb�е�ipƥ��*/
			&& limitConf->isAvailable) {/*�����ÿɱ�hash�ڵ�����*/
		    /*ƥ�䵽��һ����¼����ipHash�ڵ��limitָ��ָ��ü�¼
		     *ipHash->conf.uttMatch��λ
		     */
		    IPNODE_MODULE_PRI_MATCH_EN(ipHash->conf.uttNfMatch,UTT_NF_PRI_LIMIT);
		    ipHash->limitConf = limitConf;
		    LIMIT_PRINT("----file:%s,func:%s,line:%d--limitConf->name=%s,ip=%lx---\n",__FILE__,__func__,__LINE__,limitConf->name,ipHash->ip);
		    /*����ѭ��ǰ��Ҫ�ͷ���*/
		    read_unlock(&limitConf->lock);
		    break;
		}
	    }
	    read_unlock(&limitConf->lock);
	}
	read_unlock_bh(&limit_conf_lock);
    }
}

/*һ��ip����ʱִ�иú���*/
static void uttLimitIpNodeInit (UttNfHashBuf *ipHash)
{
    UttLimitConf *limitConf = NULL;

#ifdef CONFIG_UTT_NF_PPPOE_LIMIT
    if (ipHash->limitGrpConf != NULL) {/*���pppoe�û��Ѿ����������٣���������ģ��*/
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
	    if(limitConf->limitPolicy == 3)/*mac����*/
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
		if (uttIpMatch(&(limitConf->ipSrcGrpEntry),ipHash->ip)){/*skb�е�ipƥ�����õ�IP��Χ*/
		    LIMIT_PRINT("----file:%s,func:%s,line:%d--limitConf->name=%s-ip is match!--\n",__FILE__,__func__,__LINE__,limitConf->name);
		    /*����IPƥ���������Ŀ�����ü�������1,���ڵ�һ�α�����ʱע�ᵽtimeHook��*/
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
 *һ��IP����ִ��
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
	    if(limitConf->limitPolicy ==3)/*mac����*/
	    {
		if ((atomic_read(&limitConf->ref) > 0) && (memcmp(&(limitConf->ipSrcGrpEntry.entryVal.macAddr),&(ipHash->conf.userStats.mac),ETH_ALEN)==0)) {/*skb�е�macƥ�����õ�mac*/
		    if (atomic_dec_and_test(&limitConf->ref)) {/*����macƥ���������Ŀ�����ü�������1*/
			LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
			uttUnRegFromTimeHook(limitConf->timeHookOps, limitConf); 
		    }
		}
	    }
	    else
#endif
	    {

		if ((atomic_read(&limitConf->ref) > 0) && uttIpMatch(&(limitConf->ipSrcGrpEntry),ipHash->ip)) {/*skb�е�ipƥ�����õ�IP��Χ*/
		    if (atomic_dec_and_test(&limitConf->ref)) {/*����IPƥ���������Ŀ�����ü�������1*/
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
 *��ϸ������ģ�鶨ʱ��������
 */
static int uttLimitOnEffectiveTime (uint32_t enable, void *limitModuleData)
{
    UttLimitConf *limitConf;
    limitConf = (UttLimitConf *)limitModuleData;

    LIMIT_PRINT("----file:%s,func:%s,line:%d-name=%s- enable=%u,atomic_read(&limitConfOnTimeCount)=%d----\n",__FILE__,__func__,__LINE__,limitConf->name,enable,atomic_read(&limitConfOnTimeCount));

    if (enable) {/*ʱ����Ч*/
	limitConf->isAvailable = true;
	/*��Ч��������+1*/
	if (atomic_inc_return(&limitConfOnTimeCount) == 1) {/*�״�ע��*/
	    uttNfRegisterHook(&limitHook);/*ע������ģ��Ĺ��ӽڵ�*/
	}

    } else {/*ʱ�䲻��Ч*/
	limitConf->isAvailable = false;
	/*��Ч��������-1*/
	if (atomic_read(&limitConfOnTimeCount) > 0 && atomic_dec_and_test(&limitConfOnTimeCount)) {/*����Ч����Ϊ0ʱ*/
	    uttNfUnregisterHook(&limitHook);/*ע������ģ��Ĺ��ӽڵ�*/
	}
    }
    LIMIT_PRINT("----file:%s,func:%s,line:%d-name=%s- enable=%u,atomic_read(&limitConfOnTimeCount)=%d----\n",__FILE__,__func__,__LINE__,limitConf->name,enable,atomic_read(&limitConfOnTimeCount));
    /**
     * һ���������仯������ʹ�����ø������õ�hash�ڵ��ܵ�Ӱ��
     * �����ڴ˴�����IP hash��
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
    if (newProfileDataPtr->groupType != GBT_FINE_LIMIT) {/*������Ǿ�ϸ�����٣���������*/
	return;
    }
    LIMIT_PRINT("----file:%s,func:%s,line:%d-,changeType=%d----\n",__FILE__,__func__,__LINE__,changeType);

    /**
     * �����޸ġ�ɾ����������Ҫ�Ƚ�֮ǰ���õ�ipsetindex�����ͷ�
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

	    ip_set_put_byindex(oldIpsetIndex);/*�ͷŶԸ�ipset������*/
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
 * ������Ӻ��޸Ĳ�������Ҫ��Ӷ��µ�ipsetindex������
 * �������µ���Чʱ��ṹ
 */
if (changeType == PROF_CHANGE_ADD) {
    if (newProfileDataPtr->addrTypeEn == IP_GROUP) {
	if (*(newProfileDataPtr->ipGrpName) != '\0') {
	    ipsetindex = ip_set_get_byname((char *)newProfileDataPtr->ipGrpName);
	    LIMIT_PRINT("----file:%s,func:%s,line:%d--,ipsetName=%s,ipsetIndex=%d---\n",__FILE__,__func__,__LINE__,newProfileDataPtr->ipGrpName,ipsetindex);
	} else {
	    ipsetindex = CONFIG_ALL_USER;/*�����������û���Чʱ����65534��Ȩ��֮��*/
	}
    }
}
switch(changeType) {
    case PROF_CHANGE_ADD:
	LIMIT_PRINT("----file:%s,func:%s,line:%d--name=%s,instIndex=%d\n",__FILE__,__func__,__LINE__,newProfileDataPtr->head.name,newProfileDataPtr->ProfIndex);
#if 1
	/*Ϊ�˱������������ظ��������ã��������õ����ü�����׼ȷ��������ע��IP Node�ڵ�*/
	if (atomic_read(&limitConfCount) > 0) {
	    uttUnRegIpNodeOps(&uttLimitIpNodeOps);/*ע��IP �ڵ�*/
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
	if(limitConf->limitPolicy == 3)/*mac����*/
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
		/*����ַΪ��ַ��ʱ���ں˱�������
		 * ����ַ��������Ϊipset����ֵ*/
		strcpy(limitConf->ipSrcGrpEntry.entryVal.name,newProfileDataPtr->ipGrpName);
		limitConf->ipSrcGrpEntry.entryVal.index = ipsetindex;
#endif
	    }
	} 
	getEffectTiveTime(&effectiveTime,newProfileDataPtr);
	if ((limitConf->timeHookOps = uttGetTimeHookOpsRef(&effectiveTime)) != NULL) {
	    addToList(limitConf);
	    uttRegIpNodeOps(&uttLimitIpNodeOps);/*ע��IP �ڵ�*/
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
	/*Ҫɾ�������ÿ����Ѿ���IP hash�ڵ����ã�����ÿ��ɾ����¼ʱ������ע��IP Node�ڵ�*/
	uttUnRegIpNodeOps(&uttLimitIpNodeOps);/*ע��IP �ڵ�*/
#endif
	/*�������ô�����������ж�أ����ͷ�*/
	uttLimitConfDel(oldLimitConf);
#if 0
	if (atomic_read(&limitConfCount) == 0) {/*���ɾ��ָ�����ú󣬻�������������*/
	    uttUnRegIpNodeOps(&uttLimitIpNodeOps);/*ע��IP �ڵ�*/
	}
#endif
#if 1
	if (atomic_read(&limitConfCount) > 0) {/*���ɾ��ָ�����ú󣬻�������������*/
	    uttRegIpNodeOps(&uttLimitIpNodeOps);/*����ע��IP �ڵ�*/
	}
#endif
	LIMIT_PRINT("----file:%s,func:%s,line:%d,atomic_read(limitConfCount)=%d-----\n",__FILE__,__func__,__LINE__,atomic_read(&limitConfCount));
	break;
    case PROF_CHANGE_DELALL:
	LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
	if (atomic_read(&limitConfCount) > 0) {/*�����д�������*/
	    LIMIT_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
	    uttUnRegIpNodeOps(&uttLimitIpNodeOps);/*ע��IP �ڵ�*/
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
