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
 *������������ģ���netfilter����
 */
static uttNfHookOps dfHooks[] = {
    {
	.pf =  UTT_PF_INET, /*IPV4*/
	.dir = UTT_NF_IP_FORWARD,
	.table = UTT_NF_IPT_MANGLE,
	.condition = {
	    .ipproto =  IPPROTO_UDP,
	    .dport  =  53,
	}, /*ֻ��dns����ִ��dns���˹���*/
	.fn = uttNfdf,/*ִ�еĺ���*/
	.priority = UTT_NF_PRI_DOMAIN_NAME_FILTER,/*���ȼ���ԽСԽ��ִ��*/
    },
    {
	.pf = UTT_PF_INET, /*IPV4*/
	.dir = UTT_NF_IP_LOCAL_IN,
	.table = UTT_NF_IPT_MANGLE,
	.condition = {
	    .ipproto =  IPPROTO_UDP,
	    .dport  =  53,
	}, /*ֻ��dns����ִ��dns���˹���*/
	.fn = uttNfdf,/*ִ�еĺ���*/
	.priority = UTT_NF_PRI_DOMAIN_NAME_FILTER,/*���ȼ���ԽСԽ��ִ��*/
    }
};

/**
 * �������˹��ܿ���ʱ��ģ���ʼ��(utt_module_init.c�ļ�)����Ľṹ
 */
static UttIpNodeOps uttDfIpNodeOps  = {
    .init = uttDFIpNodeInit,
    .exit = uttDFIpNodeExit
};

/*��������������*/
static unsigned int uttNfdf (UTT_NF_HOOK_FN_ARGS)
{
    int match;
    int ret = UTT_IPT_CONTINUE;
    UttNfDnsAid aid;

    memset(&aid,0,sizeof(aid));
    uttNfDnsAidBuild(*pskb,&aid);/*��ȡ�����Լ�����hashkey*/
    if (*(aid.name) != '\0') {
	match = isDnsMatch(aid.hashKey,aid.name);

	read_lock(&uttDnsCtl->lock);
	if (((match == 1) && (uttDnsCtl->matchDrop == 0)) || /*ƥ�䵽�Ľ�ֹ*/
		((match == 0) && (uttDnsCtl->matchDrop == 1))) {/*����ûƥ�䵽�Ľ�ֹ*/
	    if (uttDnsCtl->isNotice) {/*��Ҫ�ض������ͨ��*/
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
    if(enable) {/*ʱ������Ч*/
	uttNfRegisterHooks(dfHooks,sizeof(dfHooks)/sizeof(dfHooks[0]));/*ע��dfHook���ӽڵ�*/
    } else {/*ʱ����ʧЧ��ע��dfHook���ӽڵ�*/
	uttNfUnregisterHooks(dfHooks,sizeof(dfHooks)/sizeof(dfHooks[0]));
    }

    return 0;
}
/*ע�ᵽHash Node�仯*/
static void uttDFIpNodeInit (UttNfHashBuf *ipHash)
{
    DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
    write_lock_bh(&uttDnsCtl->lock);
    if (uttIpMatch(&(uttDnsCtl->ipSrc),ipHash->ip)) {/*skb �е�ipƥ���������������õ�IP��Χ*/
	DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
	/*���ȼ���λ*/
	IPNODE_MODULE_PRI_MATCH_EN(ipHash->conf.uttNfMatch,UTT_NF_PRI_DOMAIN_NAME_FILTER);
	if(atomic_inc_return(&uttDnsCtl->ref) == 1) {/*ԭ�Ӽ�1���ж��Ƿ��һ��ִ��*/
	    DNS_PRINT("----file:%s,func:%s,line:%d--uttDnsCtl->timeSetIndex=%d---\n",__FILE__,__func__,__LINE__,uttDnsCtl->timeSetIndex);
	    if (uttDnsCtl->timeSetIndex == uttTimeSetMax) {/*��������ʱ��*/
		uttNfRegisterHooks(dfHooks,sizeof(dfHooks)/sizeof(dfHooks[0]));/*ע��dfHook���ӽڵ�*/
	    } else {
		/*ע�ᵽʱ���飬ʱ��ص���������ʱ�10���ִ��*/
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
	    /*ԭ�Ӽ�1���ж��Ƿ���ip hash�ڵ�����*/
	    if (uttDnsCtl->timeSetIndex == uttTimeSetMax) {/*��������ʱ��*/
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
 * �� �� ����noticeProfileChange
 * �������ڣ�2014-9-26
 * �޸����ڣ�
 * ���ߣ�    zhao.chong
 * ����˵����ͨ�����ñ仯���޸��ں�UttDomaindfCtl����
***********************************************************************/
static void noticeProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const NoticeProfile *oldProfileDataPtr, const NoticeProfile *newProfileDataPtr )
{
    if (index == 2) {/*��������ͨ��*/
	DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
	write_lock_bh(&uttDnsCtl->lock);
	uttDnsCtl->isNotice = newProfileDataPtr->head.active;
	write_unlock_bh(&uttDnsCtl->lock);
    }
}
#endif
/***********************************************************************
 * �� �� ����dnsFilterGlobalChange
 * �������ڣ�2014-9-26
 * �޸����ڣ�
 * ���ߣ�    zhao.chong
 * ����˵�������������������ͨ��SystemProfile����
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
    /*��ַ����:ip��ַ��Χ�����û���*/
    uttDnsCtl->ipSrc.entryType = newProfileDataPtr->dnsFilterControl.addrTypeEn;
    if (uttDnsCtl->ipSrc.entryType == IP_RANGE) {
	uttDnsCtl->ipSrc.entryVal.range.ipFrom = ntohl(newProfileDataPtr->dnsFilterControl.ipstart);
	uttDnsCtl->ipSrc.entryVal.range.ipTo = ntohl(newProfileDataPtr->dnsFilterControl.ipend);
#if (IP_GRP == FYES) 
    } else {
	/*����ַΪ��ַ��ʱ���ں˱�������
	 * ����ַ��������Ϊipset����ֵ*/
	strcpy(uttDnsCtl->ipSrc.entryVal.name,newProfileDataPtr->dnsFilterControl.ipGrpName);
	if (*(uttDnsCtl->ipSrc.entryVal.name) != '\0') {
	    uttDnsCtl->ipSrc.entryVal.index = ipsetindex;
	} else {
	    uttDnsCtl->ipSrc.entryVal.index = CONFIG_ALL_USER;/*�����������û���Чʱ����65534��Ȩ��֮��*/
	}
#endif
    }
    uttDnsCtl->matchDrop = newProfileDataPtr->dnsFilterControl.filterType;
    write_unlock_bh(&uttDnsCtl->lock);

    switch(changeType) {
        case PROF_CHANGE_ADD:
		DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);

		if (newProfileDataPtr->dnsFilterEn == TRUE) {
		    if (*(newProfileDataPtr->dnsFilterControl.timeGrpName) == '\0') {/*��������ʱ��*/
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
	    if ((oldProfileDataPtr->dnsFilterEn == FALSE) && (newProfileDataPtr->dnsFilterEn == TRUE)) {/*���������ɹرյ�����*/
		DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
		if (*(newProfileDataPtr->dnsFilterControl.timeGrpName) == '\0') {/*��������ʱ��*/
		    newTimeSetIndex = uttTimeSetMax;
		} else {
		    newTimeSetIndex = getTimeSetIndex((char *)newProfileDataPtr->dnsFilterControl.timeGrpName);
		}
		write_lock_bh(&uttDnsCtl->lock);
		uttDnsCtl->timeSetIndex = newTimeSetIndex;
		write_unlock_bh(&uttDnsCtl->lock);
		uttRegIpNodeOps(&uttDfIpNodeOps);
	    } else if ((oldProfileDataPtr->dnsFilterEn == TRUE) && (newProfileDataPtr->dnsFilterEn == FALSE)){/*���������ɿ������ر�*/
		//atomic_set(&uttDnsCtl->ref,0);
		DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
		uttUnRegIpNodeOps(&uttDfIpNodeOps);
	    } else {
		if ((oldProfileDataPtr->dnsFilterEn == TRUE) && (newProfileDataPtr->dnsFilterEn == TRUE)) {/*һֱ����*/
		    DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
		    uttUnRegIpNodeOps(&uttDfIpNodeOps);
		    if ((*(oldProfileDataPtr->dnsFilterControl.timeGrpName) == '\0') && ((*(newProfileDataPtr->dnsFilterControl.timeGrpName) != '\0'))) {/*���õ�ʱ��δ�����ʱ�ε��ض�ʱ���*/
			DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
			newTimeSetIndex = getTimeSetIndex((char *)newProfileDataPtr->dnsFilterControl.timeGrpName);
			write_lock_bh(&uttDnsCtl->lock);
			uttDnsCtl->timeSetIndex = newTimeSetIndex;
			write_unlock_bh(&uttDnsCtl->lock);
		    } else if ((*(oldProfileDataPtr->dnsFilterControl.timeGrpName) != '\0') && ((*(newProfileDataPtr->dnsFilterControl.timeGrpName) != '\0'))) {/*���õ�ʱ���һֱ���ض�ʱ���*/
			DNS_PRINT("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);
			oldTimeSetIndex = getTimeSetIndex((char *)oldProfileDataPtr->dnsFilterControl.timeGrpName);
			newTimeSetIndex = getTimeSetIndex((char *)newProfileDataPtr->dnsFilterControl.timeGrpName);
			if (oldTimeSetIndex != newTimeSetIndex) {/*���õ�ʱ�����б仯*/
			    write_lock_bh(&uttDnsCtl->lock);
			    uttDnsCtl->timeSetIndex = newTimeSetIndex;
			    write_unlock_bh(&uttDnsCtl->lock);
			}
		    } else {/*���õ�ʱ���������ʱ���*/
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
 * �� �� ����dnsFilterProfileChange
 * �������ڣ�2014-9-18
 * �޸����ڣ�
 * ���ߣ�    zhao.chong
 * ����˵����profile�仯�ں˴�������
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
    registerForProfileUpdates(MIB_PROF_SYSTEM, (Pointer)NULL, (ProfileUpdateFnc)dnsFilterGlobalChange);/*��������ȫ�����ô�������*/
    registerForProfileUpdates(MIB_PROF_DNS_FILTER, (Pointer)NULL, (ProfileUpdateFnc)dnsFilterProfileChange);/*�������ñ仯��������*/
#if (NOTICE == FYES)
    registerForProfileUpdates(MIB_PROF_NOTICE, (Pointer)NULL, (ProfileUpdateFnc)noticeProfileChange);/*��������ͨ�津������*/
#endif
#if (IP_GRP == FYES) 
    registerForProfileUpdates(MIB_PROF_IPGRP, (Pointer)NULL, (ProfileUpdateFnc)ipGroupProfileChange);/*��������*/
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
