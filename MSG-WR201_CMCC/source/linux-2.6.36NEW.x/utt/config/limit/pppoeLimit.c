#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/netfilter_ipv4/utt_filter_kernel.h>
#include <linux/netfilter_ipv4/utt_module_init.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4/utt_netfilter.h>

#include "mibShare.h"
#include "profacceShare.h"

#if 0
#define LIMIT_GRP_PRINT(fmt, args...)		printk(fmt, ## args)
#else
#define LIMIT_GRP_PRINT(fmt, args...)
#endif
extern void registerForProfileUpdates(MibProfileType mibType,Pointer updateTag,ProfileUpdateFnc	updateFnc);
static LIST_HEAD(uttLimitGrpConfListHead);
static DEFINE_RWLOCK(limit_grp_conf_spinlock);/*限速组配置链表的锁*/
static atomic_t limitGrpConfCount = ATOMIC_INIT(0);
static atomic_t limitGrpConfAliveCount = ATOMIC_INIT(0);

/*限速组操作主函数*/
unsigned int uttNfLimitGrp (UTT_NF_HOOK_FN_ARGS)
{
    int ret = UTT_IPT_CONTINUE;
    struct sk_buff *skb = *pskb;
    UttNfHashBuf *ipHash = (UttNfHashBuf*)(skb->ipHashNode);
    LimitGrpConf *limitGrpConf = NULL;

    if (ipHash == NULL) {
	return ret;
    }
    LIMIT_GRP_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
    limitGrpConf = ipHash->limitGrpConf;
    if (limitGrpConf != NULL) {
	write_lock(&limitGrpConf->lock);
	LIMIT_GRP_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
	/**
	 *真正限速的代码在此处理
	 */
        if (isSkbComeFromLan(skb)){
	    LIMIT_GRP_PRINT("----file:%s,func:%s,line:%d---****upload****--\n",__FILE__,__func__,__LINE__);
	    if (limitGrpConf->uRate != 0) {/*上传限速*/
		/*上传*/
		if(likely(limitGrpConf->limitPolicy == 1)) {/*独享*/
		    ret = utt_up_ratelimit_grp(&(ipHash->runStat), limitGrpConf, skb->len);
		} else {
		    ret = utt_up_ratelimit_grp(&(limitGrpConf->runStat), limitGrpConf, skb->len);
		}
	    }
        } else {
	    LIMIT_GRP_PRINT("----file:%s,func:%s,line:%d---****download****--\n",__FILE__,__func__,__LINE__);
            /*下载*/
	    if (limitGrpConf->dRate != 0) {/*下载限速*/
		if(likely(limitGrpConf->limitPolicy == 1)) {
		    ret = utt_down_ratelimit_grp(&(ipHash->runStat), limitGrpConf, skb->len);
		} else {
		    ret = utt_down_ratelimit_grp(&(limitGrpConf->runStat), limitGrpConf, skb->len);
		}
	    }
        }

	write_unlock(&limitGrpConf->lock);
    }

    return ret;
}

static uttNfHookOps limitGrpHook = {
    .pf =  UTT_PF_INET, /*IPV4*/
    .dir = UTT_NF_IP_FORWARD,
    .table = UTT_NF_IPT_MANGLE,
    .condition = {
	0,0
    },
    .fn = uttNfLimitGrp,/*执行的函数*/
    .priority = UTT_NF_PRI_LIMIT_GRP,/*优先级。越小越先执行*/
};

/*一般ip上线时执行该函数*/
static void uttLimitGrpIpNodeInit (UttNfHashBuf *ipHash)
{
    LimitGrpConf *limitGrpConf = NULL;
    int isIpMatch = 0;
    uint32_t ipHost = ntohl(ipHash->ip);
    uint32_t size = sizeof(ipHost) << 1;

    if (atomic_read(&limitGrpConfCount) > 0) {
	read_lock_bh(&limit_grp_conf_spinlock);
	LIMIT_GRP_PRINT("----file:%s,func:%s,line:%d--,ipHash->ip=%x---\n",__FILE__,__func__,__LINE__,ipHost);
	list_for_each_entry(limitGrpConf,&uttLimitGrpConfListHead,list){
	    read_lock(&limitGrpConf->lock);
	    /*判断该ip是否存在限速组中*/
	    isIpMatch = ip_set_testip_by_index(limitGrpConf->index,(void *)&ipHost,size);
	    LIMIT_GRP_PRINT("----file:%s,func:%s,line:%d--,limitGrpConf->index=%d-,isIpMatch=%d--\n",__FILE__,__func__,__LINE__,limitGrpConf->index,isIpMatch);
	    read_unlock(&limitGrpConf->lock);
	    if (isIpMatch) {
		IPNODE_MODULE_PRI_MATCH_EN(ipHash->conf.uttNfMatch,UTT_NF_PRI_LIMIT_GRP);
		ipHash->limitGrpConf = limitGrpConf;
		if (atomic_inc_return(&limitGrpConfAliveCount) == 1) {/*第一个限速组的ip上线*/
		    uttNfRegisterHook(&limitGrpHook);/*注册限速组模块的钩子节点*/
		}
		break;
	    }
	}
	read_unlock_bh(&limit_grp_conf_spinlock);
    }

    LIMIT_GRP_PRINT("----file:%s,func:%s,line:%d--,ipHash->ip=%x---\n",__FILE__,__func__,__LINE__,ipHost);
}

/**
 *一般IP下线执行
 */
static void uttLimitGrpIpNodeExit (UttNfHashBuf *ipHash)
{
    if (ipHash->limitGrpConf != NULL) { 
	/*生效的配置数-1*/
	if ((atomic_read(&limitGrpConfAliveCount) > 0) && atomic_dec_and_test(&limitGrpConfAliveCount)) {/*当生效配置为0时*/
	    uttNfUnregisterHook(&limitGrpHook);/*注销限速模块的钩子节点*/
	}
    }
    IPNODE_MODULE_PRI_MATCH_DIS(ipHash->conf.uttNfMatch,UTT_NF_PRI_LIMIT_GRP);
    ipHash->limitGrpConf = NULL;
    LIMIT_GRP_PRINT("----file:%s,func:%s,line:%d--,ipHash->ip=%x---\n",__FILE__,__func__,__LINE__,ipHash->ip);
}

static UttIpNodeOps uttLimitGrpIpNodeOps = {
    .init = uttLimitGrpIpNodeInit,
    .exit = uttLimitGrpIpNodeExit
};

static LimitGrpConf* limitGrpCreate(char *limitGrpName) /*生成内核限速组配置*/
{
    LimitGrpConf *limitGrpConf = kzalloc(sizeof(LimitGrpConf),GFP_KERNEL);

    if (limitGrpConf != NULL) {
	INIT_LIST_HEAD(&limitGrpConf->list);
	rwlock_init(&limitGrpConf->lock);
	limitGrpConf->index = ip_set_get_byname(limitGrpName);
	limitGrpConf->limitPolicy = 1;/*限速组暂时没有区分独享和共享，默认为独享*/
    }

    return limitGrpConf;
}

static void uttIpHashTraverse(UttNfHashBuf *ipHash)
{
    if (ipHash != NULL) {
	IPNODE_MODULE_PRI_MATCH_DIS(ipHash->conf.uttNfMatch,UTT_NF_PRI_LIMIT_GRP);
	ipHash->limitGrpConf = NULL;
    }
}

static void delAllLimitGrp(void) /*删除内核配置的同时，释放引用的ipset索引*/
{
    LimitGrpConf *pos,*n;
    ip_set_id_t releaseIndex;

    write_lock_bh(&limit_grp_conf_spinlock);
    LIMIT_GRP_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
    list_for_each_entry_safe(pos,n,&uttLimitGrpConfListHead,list){
	read_lock(&pos->lock);
	releaseIndex = pos->index;
	read_unlock(&pos->lock);
	/*释放ipset索引之前必须解锁，因为ip_set_put_byindex可能会睡眠*/
	write_unlock_bh(&limit_grp_conf_spinlock);
	ip_set_put_byindex(releaseIndex);
	list_del(&pos->list);
	kfree(pos);
	write_lock_bh(&limit_grp_conf_spinlock);
    }
    /*将所有IP hash节点引用的限速组配置指针置为NULL*/
    uttNfHashTraverse(uttIpHashTraverse);
    write_unlock_bh(&limit_grp_conf_spinlock);
}

static void limitGrpAddToList(LimitGrpConf *limitGrpConf) /*添加到全局链表*/
{
    if (limitGrpConf != NULL) {
	write_lock_bh(&limit_grp_conf_spinlock);
	list_add(&limitGrpConf->list,&uttLimitGrpConfListHead);
	write_unlock_bh(&limit_grp_conf_spinlock);
    }
}
/*
 * pppoe serve 账号profile change函数
 */
static void pppoeSrvGloProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const PPPoESrvGloProfile *oldProfileDataPtr, const PPPoESrvGloProfile *newProfileDataPtr)
{
    int i;
    LimitGrpConf *limitGrpConf = NULL;

    /**
     *限速组发生变化时，总是先删除内核的限速组配置
     */
    if (atomic_read(&limitGrpConfCount) > 0) {
	delAllLimitGrp();/*删除内核配置的同时，释放引用的ipset索引*/
	/*注销netfilter hook*/
	uttNfUnregisterHook(&limitGrpHook);/*注销限速模块的钩子节点*/
	atomic_set(&limitGrpConfCount,0);
	atomic_set(&limitGrpConfAliveCount,0);
    }

    /**
     *再重新生成内核限速组
     */
    for (i = 0; i < MAX_PPPOE_LIMIT_IPGRP; i++) {
	if (newProfileDataPtr->limitGroup[i].count > 0) {
	    limitGrpConf = limitGrpCreate((char*)newProfileDataPtr->limitGroup[i].ipGrpName);/*生成内核限速组配置*/
	    if (limitGrpConf != NULL) {
		limitGrpConf->uRate = (newProfileDataPtr->limitGroup[i].uRateLimit << 7)/HZ;
		limitGrpConf->dRate = (newProfileDataPtr->limitGroup[i].dRateLimit << 7)/HZ;
		limitGrpConf->umaxCredit = (newProfileDataPtr->limitGroup[i].uRateLimit << 7)/4; /*上传速率*/
		limitGrpConf->dmaxCredit = (newProfileDataPtr->limitGroup[i].dRateLimit << 7)/4; /*下载速率*/
		limitGrpAddToList(limitGrpConf);/*添加到全局链表*/
		atomic_inc(&limitGrpConfCount);
	    }
	}
    }
    if (atomic_read(&limitGrpConfCount) > 0) {
	/*注册ip node module init list*/
	uttRegIpNodeOps(&uttLimitGrpIpNodeOps);
    }
}

static int __init limit_grp_config_init(void)
{
    LIMIT_GRP_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
    registerForProfileUpdates(MIB_PROF_PPPOE_SRV_GLO, NULL, (ProfileUpdateFnc)pppoeSrvGloProfileChange);
    return 0;
}

static void __exit limit_grp_config_exit(void)
{
    LIMIT_GRP_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
}
module_init(limit_grp_config_init);
module_exit(limit_grp_config_exit);
