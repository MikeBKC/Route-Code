#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/netfilter_ipv4/time_set.h>
#include <linux/netfilter_ipv4/utt_filter_kernel.h>
#include <linux/netfilter_ipv4/utt_module_init.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4/utt_dns_hash.h>
#include <linux/netfilter_ipv4/ipt_REJECT.h>
#include <linux/netfilter_ipv4/ip_set.h>

#include "mibShare.h"
#include "profacceShare.h"

#if 0
#define DNS_PRINT(fmt, args...)		printk(fmt, ## args)
#else
#define DNS_PRINT(fmt, args...)
#endif
static UttDomaindfCtl *uttDnsCtl;

static int dnsModuleData;
extern __be32 utt_local_address __read_mostly;
extern void registerForProfileUpdates(MibProfileType mibType,Pointer updateTag,ProfileUpdateFnc	updateFnc);
static unsigned int uttNfdf (UTT_NF_HOOK_FN_ARGS);
static int uttDfOnTimeSet (uint32_t enable, void *dnsFilterModuleData);
static void uttDFIpNodeInit (UttNfHashBuf *ipHash);
static void uttDFIpNodeExit (UttNfHashBuf *ipHash);

/**
 *定义域名过滤模块的netfilter钩子
 */
static uttNfHookOps dfHooks[] = {
    {
	.pf =  UTT_PF_INET, /*IPV4*/
	.dir = UTT_NF_IP_FORWARD,
	.table = UTT_NF_IPT_MANGLE,
	.condition = {
	    .ipproto =  IPPROTO_UDP,
	    .dport  =  53,
	}, /*只有dns包才执行dns过滤功能*/
	.fn = uttNfdf,/*执行的函数*/
	.priority = UTT_NF_PRI_DOMAIN_NAME_FILTER,/*优先级。越小越先执行*/
    },
    {
	.pf = UTT_PF_INET, /*IPV4*/
	.dir = UTT_NF_IP_LOCAL_IN,
	.table = UTT_NF_IPT_MANGLE,
	.condition = {
	    .ipproto =  IPPROTO_UDP,
	    .dport  =  53,
	}, /*只有dns包才执行dns过滤功能*/
	.fn = uttNfdf,/*执行的函数*/
	.priority = UTT_NF_PRI_DOMAIN_NAME_FILTER,/*优先级。越小越先执行*/
    }
};

/**
 * 域名过滤功能开启时，模块初始化(utt_module_init.c文件)所需的结构
 */
static UttIpNodeOps uttDfIpNodeOps  = {
    .init = uttDFIpNodeInit,
    .exit = uttDFIpNodeExit
};

/*域名过滤主函数*/
static unsigned int uttNfdf (UTT_NF_HOOK_FN_ARGS)
{
    int match;
    int ret = UTT_IPT_CONTINUE;
    UttNfDnsAid aid;

    memset(&aid,0,sizeof(aid));
    uttNfDnsAidBuild(*pskb,&aid);/*获取域名以及生成hashkey*/
    if (*(aid.name) != '\0') {
	match = isDnsMatch(aid.hashKey,aid.name);

	read_lock(&uttDnsCtl->lock);
	if (((match == 1) && (uttDnsCtl->matchDrop == 0)) || /*匹配到的禁止*/
		((match == 0) && (uttDnsCtl->matchDrop == 1))) {/*或者没匹配到的禁止*/
	    if (uttDnsCtl->isNotice) {/*需要重定向进行通告*/
		DNS_PRINT("----file:%s,func:%s,line:%d-need redirect---\n",__FILE__,__func__,__LINE__);
		send_dns_redirect(*pskb,ntohl(utt_local_address));
	    }
	    ret = UTT_NF_DROP;
	}
	read_unlock(&uttDnsCtl->lock);
    }
    DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);

    return ret;
}

static int uttDfOnTimeSet (uint32_t enable, void *dnsFilterModuleData)
{
    DNS_PRINT("----file:%s,func:%s,line:%d-- enable=%u---\n",__FILE__,__func__,__LINE__,enable);
    if(enable) {/*时间组生效*/
	uttNfRegisterHooks(dfHooks,sizeof(dfHooks)/sizeof(dfHooks[0]));/*注册dfHook钩子节点*/
    } else {/*时间组失效，注销dfHook钩子节点*/
	uttNfUnregisterHooks(dfHooks,sizeof(dfHooks)/sizeof(dfHooks[0]));
    }

    return 0;
}
/*注册到Hash Node变化*/
static void uttDFIpNodeInit (UttNfHashBuf *ipHash)
{
    DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
    write_lock_bh(&uttDnsCtl->lock);
    if (uttIpMatch(&(uttDnsCtl->ipSrc),ipHash->ip)) {/*skb 中的ip匹配域名过滤中配置的IP范围*/
	DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
	/*优先级置位*/
	IPNODE_MODULE_PRI_MATCH_EN(ipHash->conf.uttNfMatch,UTT_NF_PRI_DOMAIN_NAME_FILTER);
	if(atomic_inc_return(&uttDnsCtl->ref) == 1) {/*原子加1并判断是否第一次执行*/
	    DNS_PRINT("----file:%s,func:%s,line:%d--uttDnsCtl->timeSetIndex=%d---\n",__FILE__,__func__,__LINE__,uttDnsCtl->timeSetIndex);
	    if (uttDnsCtl->timeSetIndex == uttTimeSetMax) {/*配置所有时段*/
		uttNfRegisterHooks(dfHooks,sizeof(dfHooks)/sizeof(dfHooks[0]));/*注册dfHook钩子节点*/
	    } else {
		/*注册到时间组，时间回调函数会延时最长10秒后执行*/
		uttRegToTimeSet(uttDnsCtl->timeSetIndex,&dnsModuleData,uttDfOnTimeSet);	
	    }
	}
    }
    write_unlock_bh(&uttDnsCtl->lock);
}

static void uttDFIpNodeExit (UttNfHashBuf *ipHash)
{
    DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
    if(IPNODE_MODULE_PRI_MATCH_TEST(ipHash->conf.uttNfMatch,UTT_NF_PRI_DOMAIN_NAME_FILTER)) {
	IPNODE_MODULE_PRI_MATCH_DIS(ipHash->conf.uttNfMatch,UTT_NF_PRI_DOMAIN_NAME_FILTER);
	write_lock_bh(&uttDnsCtl->lock);
	if ((atomic_read(&uttDnsCtl->ref) > 0) && (atomic_dec_return(&uttDnsCtl->ref) == 0)) {
	    /*原子减1并判断是否无ip hash节点引用*/
	    if (uttDnsCtl->timeSetIndex == uttTimeSetMax) {/*配置所有时段*/
		uttNfUnregisterHooks(dfHooks,sizeof(dfHooks)/sizeof(dfHooks[0]));
	    } else {
		uttUnRegFromTimeSet(uttDnsCtl->timeSetIndex,&dnsModuleData);	
	    }
	}
	write_unlock_bh(&uttDnsCtl->lock);
    }
}

#if (IP_GRP == FYES) 
static void ipGroupProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const IpGrpProfile *oldProfileDataPtr, const IpGrpProfile *newProfileDataPtr )
{
	uttUnRegIpNodeOps(&uttDfIpNodeOps);
	uttRegIpNodeOps(&uttDfIpNodeOps);
}
#endif
#if (NOTICE == FYES)
/***********************************************************************
 * 函 数 名：noticeProfileChange
 * 创建日期：2014-9-26
 * 修改日期：
 * 作者：    zhao.chong
 * 附加说明：通告配置变化需修改内核UttDomaindfCtl配置
***********************************************************************/
static void noticeProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const NoticeProfile *oldProfileDataPtr, const NoticeProfile *newProfileDataPtr )
{
    if (index == 2) {/*域名过滤通告*/
	DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
	write_lock_bh(&uttDnsCtl->lock);
	uttDnsCtl->isNotice = newProfileDataPtr->head.active;
	write_unlock_bh(&uttDnsCtl->lock);
    }
}
#endif
/***********************************************************************
 * 函 数 名：dnsFilterGlobalChange
 * 创建日期：2014-9-26
 * 修改日期：
 * 作者：    zhao.chong
 * 附加说明：域名过滤启用与否，通过SystemProfile决定
***********************************************************************/
void dnsFilterGlobalChange (Pointer tag, MibProfileType profileType,
        ProfChangeType changeType, Uint32 index,
        const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr )
{
    uint16_t  oldTimeSetIndex, newTimeSetIndex;
    ip_set_id_t ipsetindex = IP_SET_INVALID_ID;
    ip_set_id_t oldIpsetIndex; 

    DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
    if ((changeType != PROF_CHANGE_ADD) && 
	(oldProfileDataPtr->dnsFilterControl.addrTypeEn == IP_GROUP) && 
	    (*(oldProfileDataPtr->dnsFilterControl.ipGrpName) != '\0')) {
	write_lock_bh(&uttDnsCtl->lock);
	oldIpsetIndex = uttDnsCtl->ipSrc.entryVal.index;
	write_unlock_bh(&uttDnsCtl->lock);
	
	ip_set_put_byindex(oldIpsetIndex);
    }
    if (newProfileDataPtr->dnsFilterControl.addrTypeEn == IP_GROUP) {
	if (*(newProfileDataPtr->dnsFilterControl.ipGrpName) != '\0') {
	    ipsetindex = ip_set_get_byname((char *)newProfileDataPtr->dnsFilterControl.ipGrpName);
	    DNS_PRINT("----file:%s,func:%s,line:%d--,ipsetName=%s,ipsetIndex=%d---\n",__FILE__,__func__,__LINE__,newProfileDataPtr->dnsFilterControl.ipGrpName,ipsetindex);
	}
    }

    write_lock_bh(&uttDnsCtl->lock);
    /*地址类型:ip地址范围或是用户组*/
    uttDnsCtl->ipSrc.entryType = newProfileDataPtr->dnsFilterControl.addrTypeEn;
    if (uttDnsCtl->ipSrc.entryType == IP_RANGE) {
	uttDnsCtl->ipSrc.entryVal.range.ipFrom = ntohl(newProfileDataPtr->dnsFilterControl.ipstart);
	uttDnsCtl->ipSrc.entryVal.range.ipTo = ntohl(newProfileDataPtr->dnsFilterControl.ipend);
#if (IP_GRP == FYES) 
    } else {
	/*当地址为地址组时，内核保存配置
	 * 将地址组名保存为ipset索引值*/
	strcpy(uttDnsCtl->ipSrc.entryVal.name,newProfileDataPtr->dnsFilterControl.ipGrpName);
	if (*(uttDnsCtl->ipSrc.entryVal.name) != '\0') {
	    uttDnsCtl->ipSrc.entryVal.index = ipsetindex;
	} else {
	    uttDnsCtl->ipSrc.entryVal.index = CONFIG_ALL_USER;/*当配置所有用户生效时，用65534做权宜之计*/
	}
#endif
    }
    uttDnsCtl->matchDrop = newProfileDataPtr->dnsFilterControl.filterType;
    write_unlock_bh(&uttDnsCtl->lock);

    switch(changeType) {
        case PROF_CHANGE_ADD:
		DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);

		if (newProfileDataPtr->dnsFilterEn == TRUE) {
		    if (*(newProfileDataPtr->dnsFilterControl.timeGrpName) == '\0') {/*配置所有时段*/
			newTimeSetIndex = uttTimeSetMax;
		    } else {
			newTimeSetIndex = getTimeSetIndex((char *)newProfileDataPtr->dnsFilterControl.timeGrpName);
			DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
		    }
		    write_lock_bh(&uttDnsCtl->lock);
		    uttDnsCtl->timeSetIndex = newTimeSetIndex;
		    write_unlock_bh(&uttDnsCtl->lock);

		    uttRegIpNodeOps(&uttDfIpNodeOps);
		}
		break;
        case PROF_CHANGE_EDIT:
	    DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
	    if ((oldProfileDataPtr->dnsFilterEn == FALSE) && (newProfileDataPtr->dnsFilterEn == TRUE)) {/*域名过滤由关闭到开启*/
		DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
		if (*(newProfileDataPtr->dnsFilterControl.timeGrpName) == '\0') {/*配置所有时段*/
		    newTimeSetIndex = uttTimeSetMax;
		} else {
		    newTimeSetIndex = getTimeSetIndex((char *)newProfileDataPtr->dnsFilterControl.timeGrpName);
		}
		write_lock_bh(&uttDnsCtl->lock);
		uttDnsCtl->timeSetIndex = newTimeSetIndex;
		write_unlock_bh(&uttDnsCtl->lock);
		uttRegIpNodeOps(&uttDfIpNodeOps);
	    } else if ((oldProfileDataPtr->dnsFilterEn == TRUE) && (newProfileDataPtr->dnsFilterEn == FALSE)){/*域名过滤由开启到关闭*/
		//atomic_set(&uttDnsCtl->ref,0);
		DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
		uttUnRegIpNodeOps(&uttDfIpNodeOps);
	    } else {
		if ((oldProfileDataPtr->dnsFilterEn == TRUE) && (newProfileDataPtr->dnsFilterEn == TRUE)) {/*一直开启*/
		    DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
		    uttUnRegIpNodeOps(&uttDfIpNodeOps);
		    if ((*(oldProfileDataPtr->dnsFilterControl.timeGrpName) == '\0') && ((*(newProfileDataPtr->dnsFilterControl.timeGrpName) != '\0'))) {/*引用的时间段从所有时段到特定时间段*/
			DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
			newTimeSetIndex = getTimeSetIndex((char *)newProfileDataPtr->dnsFilterControl.timeGrpName);
			write_lock_bh(&uttDnsCtl->lock);
			uttDnsCtl->timeSetIndex = newTimeSetIndex;
			write_unlock_bh(&uttDnsCtl->lock);
		    } else if ((*(oldProfileDataPtr->dnsFilterControl.timeGrpName) != '\0') && ((*(newProfileDataPtr->dnsFilterControl.timeGrpName) != '\0'))) {/*引用的时间段一直是特定时间段*/
			DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
			oldTimeSetIndex = getTimeSetIndex((char *)oldProfileDataPtr->dnsFilterControl.timeGrpName);
			newTimeSetIndex = getTimeSetIndex((char *)newProfileDataPtr->dnsFilterControl.timeGrpName);
			if (oldTimeSetIndex != newTimeSetIndex) {/*引用的时间组有变化*/
			    write_lock_bh(&uttDnsCtl->lock);
			    uttDnsCtl->timeSetIndex = newTimeSetIndex;
			    write_unlock_bh(&uttDnsCtl->lock);
			}
		    } else {/*引用的时间段是所有时间段*/
			    write_lock_bh(&uttDnsCtl->lock);
			    uttDnsCtl->timeSetIndex = uttTimeSetMax;
			    write_unlock_bh(&uttDnsCtl->lock);
		    }
		    uttRegIpNodeOps(&uttDfIpNodeOps);
		}
	    }
	    break;
        case PROF_CHANGE_DEL:
	    break;
        case PROF_CHANGE_DELALL:
	    break;
        default:
	    break;
    };
    DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
}

/***********************************************************************
 * 函 数 名：dnsFilterProfileChange
 * 创建日期：2014-9-18
 * 修改日期：
 * 作者：    zhao.chong
 * 附加说明：profile变化内核触发函数
***********************************************************************/
void dnsFilterProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const DnsFilterProfile *oldProfileDataPtr, const DnsFilterProfile *newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
	    DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
	    dns_hash_insert(newProfileDataPtr->domainName);
	    break;
        case PROF_CHANGE_EDIT:
	    DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
	    dns_hash_delete(oldProfileDataPtr->domainName);
	    dns_hash_insert(newProfileDataPtr->domainName);
	    break;
        case PROF_CHANGE_DEL:
	    DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
	    dns_hash_delete(oldProfileDataPtr->domainName);
	    break;
        case PROF_CHANGE_DELALL:
	    DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
	    dns_hash_delete_all();
	    break;
        default:
	    break;
    };
}
static int __init dns_filter_config_init(void)
{
    DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
    uttDnsCtl = (UttDomaindfCtl*)kzalloc(sizeof(UttDomaindfCtl),GFP_KERNEL);
    if (uttDnsCtl == NULL) {
	return 1;
    }
    rwlock_init(&uttDnsCtl->lock);
    registerForProfileUpdates(MIB_PROF_SYSTEM, (Pointer)NULL, (ProfileUpdateFnc)dnsFilterGlobalChange);/*域名过滤全局配置触发函数*/
    registerForProfileUpdates(MIB_PROF_DNS_FILTER, (Pointer)NULL, (ProfileUpdateFnc)dnsFilterProfileChange);/*域名配置变化触发函数*/
#if (NOTICE == FYES)
    registerForProfileUpdates(MIB_PROF_NOTICE, (Pointer)NULL, (ProfileUpdateFnc)noticeProfileChange);/*域名过滤通告触发函数*/
#endif
#if (IP_GRP == FYES) 
    registerForProfileUpdates(MIB_PROF_IPGRP, (Pointer)NULL, (ProfileUpdateFnc)ipGroupProfileChange);/*触发函数*/
#endif
    return 0;
}

static void __exit dns_filter_config_exit(void)
{
    DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
    if (uttDnsCtl != NULL) {
	uttUnRegIpNodeOps(&uttDfIpNodeOps);
	kfree(uttDnsCtl);
	uttDnsCtl = NULL;
    }
}
module_init(dns_filter_config_init);
module_exit(dns_filter_config_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhao.chong (zhao.chong@utt.com.cn)");
