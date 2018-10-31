/* netfilter.c: look after the filters for various protocols.
 * Heavily influenced by the old firewall.c by David Bonn and Alan Cox.
 *
 * Thanks to Rob `CmdrTaco' Malda for not influencing this code in any
 * way.
 *
 * Rusty Russell (C)2000 -- This code is GPL.
 */
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <net/protocol.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/wait.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/if.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include <linux/proc_fs.h>
#include <linux/mutex.h>
#include <net/sock.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <linux/netfilter_ipv4/utt_filter_kernel.h>

#include "nf_internals.h"

#if 0
#define MODULE_HOOK_PRINT(fmt, args...)		printk(fmt, ## args)
#else
#define MODULE_HOOK_PRINT(fmt, args...)
#endif

/*性能优化自定义hook结构及操作*/
static struct list_head uttNfHookHead[UTT_PF_MAX][UTT_NF_IP_DIRECTION_MAX][UTT_NF_IPT_MAX] __read_mostly;
static DEFINE_SPINLOCK(uttNfHookSpinLock);

static void uttNfHookLock(void)
{
    spin_lock_bh(&uttNfHookSpinLock);/*软中断之间(跟内核定时器)可能会有竞争*/
}

static void uttNfHookUnlock(void)
{
    spin_unlock_bh(&uttNfHookSpinLock);
}

int uttNfRegisterHook(uttNfHookOps *reg)
{
    uttNfHookOps *elem;

    MODULE_HOOK_PRINT("===file:%s,func:%s,line:%d======\n",__FILE__,__func__,__LINE__);
    uttNfHookLock();
    list_for_each_entry_rcu(elem, &uttNfHookHead[reg->pf][reg->dir][reg->table], list) {
	if (reg == elem) {
	    MODULE_HOOK_PRINT("already in the hook\n");
	    goto unlock;
	}
	if (reg->priority < elem->priority) {
	    break;
	}
    }   
    list_add_rcu(&reg->list, elem->list.prev);

unlock:
    uttNfHookUnlock();

    return 0;
}
EXPORT_SYMBOL(uttNfRegisterHook);

void uttNfHookOpsFree(struct rcu_head *rh)
{
    MODULE_HOOK_PRINT("===file:%s,func:%s,line:%d======\n",__FILE__,__func__,__LINE__);
}
EXPORT_SYMBOL(uttNfHookOpsFree);

void uttNfUnregisterHook(uttNfHookOps *reg)
{
    uttNfHookOps *elem;
    bool flag = false;

    MODULE_HOOK_PRINT("===file:%s,func:%s,line:%d======\n",__FILE__,__func__,__LINE__);

    uttNfHookLock();
    list_for_each_entry_rcu(elem, &uttNfHookHead[reg->pf][reg->dir][reg->table], list) {
	if (reg == elem) {
	    flag = true;
	    list_del_rcu(&reg->list);
	    break;
	}
    }   
    uttNfHookUnlock();

    if (flag) {
	call_rcu_bh(&reg->rcu,uttNfHookOpsFree);
    }
}
EXPORT_SYMBOL(uttNfUnregisterHook);

int uttNfRegisterHooks(uttNfHookOps *reg, unsigned int n)
{
    unsigned int i;

    if (reg != NULL) {
	for (i = 0; i < n; i++) {
	    uttNfRegisterHook(&reg[i]);
	}
    }

    return 0;
}
EXPORT_SYMBOL(uttNfRegisterHooks);

void uttNfUnregisterHooks(uttNfHookOps *reg, unsigned int n)
{
    unsigned int i;

    if (reg != NULL) {
	for (i = 0; i < n; i++) {
	    uttNfUnregisterHook(&reg[i]);
	}
    }
}
EXPORT_SYMBOL(uttNfUnregisterHooks);

static unsigned int uttNfIterate(struct list_head *head,
			UTT_NF_HOOK_FN_ARGS,
			struct list_head **i)
{
	unsigned int verdict;
	struct iphdr *iph;
	struct udphdr *udph;
	struct tcphdr *tcph;
	__u8 proto;
	__be16 dport;
	UttNfHashBuf *ipHashNode;

	iph = ip_hdr(*pskb);

	/* 非ip包 */
	if (iph != NULL) {
	    proto = iph->protocol;
	    /*非UDP协议*/
	    if (proto != IPPROTO_UDP) {
		udph = (struct udphdr *) ((void *)iph + iph->ihl * 4);
		dport = ntohs(udph->dest);
	    } else if (proto != IPPROTO_TCP) {
		tcph = (struct tcphdr *) ((void *)iph + iph->ihl * 4);
		dport = ntohs(tcph->dest);
	    } else {
		return UTT_NF_ACCEPT;
	    }
	} else {
	    return UTT_NF_ACCEPT;
	}

	/**
	 * 1. 若 对应的iphash表节点 需要匹配该hook（skb->uttHashConf[uttNfMatch]的bit位
	 * priority是否为1 ）并且 匹配uttNlPrecondition    
	 * 则执行相关的ret = uttNfHookFn 函数
	 * 2.ret为UTT_IPT_CONTINUE则继续遍历mglFwdHookHead执行
	 * 3. ret 不为UTT_IPT_CONTINUE 则break
	 */

	ipHashNode = (UttNfHashBuf *)((*pskb)->ipHashNode);
	if (ipHashNode != NULL) {
	    /*
	     * The caller must not block between calls to this
	     * function because of risk of continuing from deleted element.
	     */
	    list_for_each_continue_rcu(*i, head) {
		    uttNfHookOps *elem = (uttNfHookOps *)*i;
		    if (IPNODE_MODULE_PRI_MATCH_TEST(ipHashNode->conf.uttNfMatch,elem->priority)) {
			if (((elem->condition.dport == 0) && (elem->condition.ipproto == 0)) ||
				((elem->condition.dport == dport) && (elem->condition.ipproto == proto))) {
			    /* Optimization: we don't need to hold module
			       reference here, since function can't sleep. --RR */
			    MODULE_HOOK_PRINT("============func:%s,priority Match!!!!=============\n",__func__);
			    verdict = elem->fn(pskb);
			    if (verdict != UTT_IPT_CONTINUE) {
				    return verdict;
			    }
			}
		    }
	    }
	}
	return UTT_NF_ACCEPT;
}


int uttNfHookSlow(uttProtocolFamily pf, uttNfIpDirection dir, uttNfIpTable table, UTT_NF_HOOK_FN_ARGS)
{
	struct list_head *elem;
	unsigned int verdict;
	int ret = UTT_NF_ACCEPT;

	/* We may already have this, but read-locks nest anyway */
	rcu_read_lock_bh();

	elem = &uttNfHookHead[pf][dir][table];
	if (!list_empty(elem)) {
	    verdict = uttNfIterate(&uttNfHookHead[pf][dir][table], pskb, &elem);
	    if (verdict == UTT_NF_ACCEPT) {
		    ret = 1;
		    goto unlock;
	    } else if (verdict == UTT_NF_DROP) {
		    kfree_skb(*pskb);
		    ret = -EPERM;
	    }
	}
unlock:
	rcu_read_unlock_bh();
	return ret;
}
EXPORT_SYMBOL(uttNfHookSlow);

void __init uttNetfilterInit(void)
{
    int i, h, t;
    for (i = 0; i < UTT_PF_MAX; i++) {
	for (h = 0; h < UTT_NF_IP_DIRECTION_MAX; h++) {
	    for (t = 0; t < UTT_NF_IPT_MAX; t++) {
		INIT_LIST_HEAD(&uttNfHookHead[i][h][t]);
	    }
	}
    }
}
