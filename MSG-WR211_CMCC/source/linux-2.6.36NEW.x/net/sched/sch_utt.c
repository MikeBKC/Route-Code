/*
 * net/sched/sch_sfq.c	Stochastic Fairness Queueing discipline.
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 *
 * Authors:	Alexey Kuznetsov, <kuznet@ms2.inr.ac.ru>
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/string.h>
#include <linux/in.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/ipv6.h>
#include <linux/skbuff.h>
#include <linux/jhash.h>
#include <net/ip.h>
#include <net/netlink.h>
#include <net/pkt_sched.h>
#include <net/sch_utt.h>


/*	Stochastic Fairness Queuing algorithm.
	=======================================

	Source:
	Paul E. McKenney "Stochastic Fairness Queuing",
	IEEE INFOCOMM'90 Proceedings, San Francisco, 1990.

	Paul E. McKenney "Stochastic Fairness Queuing",
	"Interworking: Research and Experience", v.2, 1991, p.113-131.


	See also:
	M. Shreedhar and George Varghese "Efficient Fair
	Queuing using Deficit Round Robin", Proc. SIGCOMM 95.


	This is not the thing that is usually called (W)FQ nowadays.
	It does not use any timestamp mechanism, but instead
	processes queues in round-robin order.

	ADVANTAGE:

	- It is very cheap. Both CPU and memory requirements are minimal.

	DRAWBACKS:

	- "Stochastic" -> It is not 100% fair.
	When hash collisions occur, several flows are considered as one.

	- "Round-robin" -> It introduces larger delays than virtual clock
	based schemes, and should not be used for isolating interactive
	traffic	from non-interactive. It means, that this scheduler
	should be used as leaf of CBQ or P3, which put interactive traffic
	to higher priority band.

	We still need true WFQ for top level CSZ, but using WFQ
	for the best effort traffic is absolutely pointless:
	SFQ is superior for this purpose.

	IMPLEMENTATION:
	This implementation limits maximal queue length to 128;
	maximal mtu to 2^15-1; number of hash buckets to 1024.
	The only goal of this restrictions was that all data
	fit into one 4K page :-). Struct sfq_sched_data is
	organized in anti-cache manner: all the data for a bucket
	are scattered over different locations. This is not good,
	but it allowed me to put it into 4K.

	It is easy to increase these values, but not in flight.  */


static unsigned int sfq_drop(struct Qdisc *sch)
{
#if 0
	struct sfq_sched_data *q = qdisc_priv(sch);
	sfq_index d = q->max_depth;
	struct sk_buff *skb;
	unsigned int len;

	/* Queue is full! Find the longest slot and
	   drop a packet from it */

	if (d > 1) {
		sfq_index x = q->dep[d + SFQ_DEPTH].next;
		skb = q->qs[x].prev;
		len = qdisc_pkt_len(skb);
		__skb_unlink(skb, &q->qs[x]);
		kfree_skb(skb);
		sfq_dec(q, x);
		sch->q.qlen--;
		sch->qstats.drops++;
		sch->qstats.backlog -= len;
		return len;
	}

	if (d == 1) {
		/* It is difficult to believe, but ALL THE SLOTS HAVE LENGTH 1. */
		d = q->next[q->tail];
		q->next[q->tail] = q->next[d];
		q->allot[q->next[d]] += q->quantum;
		skb = q->qs[d].prev;
		len = qdisc_pkt_len(skb);
		__skb_unlink(skb, &q->qs[d]);
		kfree_skb(skb);
		sfq_dec(q, d);
		sch->q.qlen--;
		q->ht[q->hash[d]] = SFQ_DEPTH;
		sch->qstats.drops++;
		sch->qstats.backlog -= len;
		return len;
	}

#endif
	return 0;
}

struct sk_buff_head uttqs;
extern struct utt_wan_if_info   uttEtherIfInfo[UTT_MAX_IF_NUM];
static int sfq_enqueue(struct sk_buff *skb, struct Qdisc *sch)
{
    struct smart_sched_data *q = qdisc_priv(sch);
#if 0
    unsigned int hash;
    unsigned int sIP;
#endif
    struct iphdr *iph = NULL;
    struct smart_hash_node *hashNode = NULL;
    struct smart_list *list = NULL, *tmpList = NULL;
    struct net_device  *dev = sch->dev_queue->dev;
    /*
     * calculate the hash for find hashtable head
     */
#if 0
    switch (skb->protocol) {
        case htons(ETH_P_IP):
        {
    	    iph = ip_hdr(skb);
            if(skb->old_ip == 0) {
                sIP = ntohl(iph->saddr);
            } else {
                sIP = ntohl(skb->old_ip);
            }
            break;
        }
        default:
            sIP = skb->protocol;
            break;
    }
    /*
     * notice little endian big endian
     */
    hash = sIP & 0xff;

    /*
     * find match this ip's hash node
     */
    head = uttEtherIfInfo[dev->utt_ifindex].smartQosHash[hash];
    while(head != NULL) {
        if(head->sip == sIP) {
            hashNode = head;

#if 0
            printk("hash sd %x %x\n",hashNode->chainNo, head->sip);
#endif
            break;
        } else {
            head = head->nextHashNode;
        }
    }

#if 0
    printk("ip %x\n",sIP);
#endif
    if(hashNode == NULL) {
	    return qdisc_drop(skb, sch);
#if 0
        printk("new hash %x\n", sIP);
#endif
        /*
         * cread a hash node and insert to hashtable
         */
        hashNode = kzalloc(sizeof(struct smart_hash_node), GFP_KERNEL);
        if(hashNode == NULL) {
	    return qdisc_drop(skb, sch);
        }
        hashNode->chainNo = UTT_SMART_QOS_CHAIN_NULL;
        hashNode->sip = sIP;
        hashNode->nextHashNode = uttEtherIfInfo[dev->utt_ifindex].smartQosHash[hash];
        uttEtherIfInfo[dev->utt_ifindex].smartQosHash[hash] = hashNode;

        skb_queue_head_init(&hashNode->qs);
        /*
         * change online num
         */
        if(skb->protocol == htons(ETH_P_IP)) {
            uttEtherIfInfo[dev->utt_ifindex].onlineCliCount++;
            hashNode->etherProtocol = htons(ETH_P_IP);
        }

        hashNode->rxBytes = 0;
        hashNode->txBytes = 0;
        hashNode->lastClrTime = jiffies;
        /*
         * here need change pre rate for every one
         */
        if(uttEtherIfInfo[dev->utt_ifindex].onlineCliCount > 0) {
        uttEtherIfInfo[dev->utt_ifindex].avgRxRate = 
            uttEtherIfInfo[dev->utt_ifindex].maxRxRate/uttEtherIfInfo[dev->utt_ifindex].onlineCliCount;

        uttEtherIfInfo[dev->utt_ifindex].avgTxRate = 
            uttEtherIfInfo[dev->utt_ifindex].maxTxRate/uttEtherIfInfo[dev->utt_ifindex].onlineCliCount;

        uttEtherIfInfo[dev->utt_ifindex].avgTxRate = 
            (uttEtherIfInfo[dev->utt_ifindex].avgTxRate/HZ) * uttEtherIfInfo[dev->utt_ifindex].clear_interval;
        uttEtherIfInfo[dev->utt_ifindex].avgRxRate = 
            (uttEtherIfInfo[dev->utt_ifindex].avgRxRate/HZ) * uttEtherIfInfo[dev->utt_ifindex].clear_interval;
        } else {
        uttEtherIfInfo[dev->utt_ifindex].avgRxRate = 
            (uttEtherIfInfo[dev->utt_ifindex].maxRxRate/HZ) * uttEtherIfInfo[dev->utt_ifindex].clear_interval;

        uttEtherIfInfo[dev->utt_ifindex].avgTxRate = 
            (uttEtherIfInfo[dev->utt_ifindex].maxTxRate/HZ) * uttEtherIfInfo[dev->utt_ifindex].clear_interval;
        }
        hashNode->seclude = 1;
    }
#else
    hashNode = skb->qosHash;
    if(hashNode == NULL) {
        return qdisc_drop(skb, sch);
    }
    if(skb->protocol == htons(ETH_P_IP)) {
        iph = ip_hdr(skb);
    }
#endif

    if((unsigned int)jiffies - hashNode->lastClrTime >= dev->qos_clear_interval) {
        hashNode->rxBytes = 0;
        hashNode->txBytes = 0;
        hashNode->lastClrTime = jiffies;
#if 0
        printk("en clr %x %x %x\n",jiffies,hashNode->rxBytes,hashNode->txBytes);
#endif
    }

#if 0
    if((unsigned int)jiffies - hashNode->lastClrOneSecTime >= HZ) {
        hashNode->perSecRxBytes = hashNode->tmpPerSecRxBytes;
        hashNode->perSecTxBytes = hashNode->tmpPerSecTxBytes;
        hashNode->tmpPerSecTxBytes = skb->len;
        hashNode->tmpPerSecRxBytes = 0;
        hashNode->lastClrOneSecTime = jiffies;
    } else {
        hashNode->tmpPerSecTxBytes += skb->len;
    }
#endif

    hashNode->lastUseTime = jiffies;
    /*
     * insert the skb to the qs
     */

#if 0
#if 0
    if(hashNode->chainNo == UTT_SMART_QOS_CHAIN_LEVEL1) {
        if(hashNode->qs.qlen >= 1) {
	    return qdisc_drop(skb, sch);
        }
    } else {
#endif
    if(hashNode->qs.qlen >= dev->qos_max_queue_len) {
#if 0
        if(iph != NULL && iph->protocol == IPPROTO_TCP) {
            struct tcphdr *tcph = (struct tcphdr *)(iph+1);
            if(tcph->dest != ntohs(80) || hashNode->qs.qlen > 10) {
	        return qdisc_drop(skb, sch);
            } else {
            }
        } else {
	    return qdisc_drop(skb, sch);
        }
#else
	    return qdisc_drop(skb, sch);
#endif
    }
#if 0
    }
#endif
#else
#if 0
//    if(hashNode->chainNo == UTT_SMART_QOS_CHAIN_LEVEL1) 
    {
        if(hashNode->qs.qlen >= dev->qos_max_queue_len) {
            if(iph != NULL && iph->protocol == IPPROTO_TCP) {
                struct tcphdr *tcph = (struct tcphdr *)(iph+1);
                if(tcph->dest != ntohs(80) || hashNode->qs.qlen >= 10) {
	            return qdisc_drop(skb, sch);
                }
            } else {
	        return qdisc_drop(skb, sch);
            }
        }
    }
#else
#if 0
        if(iph != NULL) {
            if(iph->protocol == IPPROTO_TCP) {
             struct tcphdr *tcph = (struct tcphdr *)(iph+1);
             if(tcph->dest == ntohs(80) && hashNode->httpPackCount < 5) {
//                 printk(" %pI4 80\n", &hashNode->sip);
                 (hashNode->httpPackCount)++;
#if 0
                 if(hashNode->lesslimitCnt >= 2) {
                    hashNode = &(q->webPriNode);
//                    dev->qos_pertick_rxbytes -= (HZ/2 * dev->qos_pertick_rxbytes);
                 } else 
#endif
                 {
                    hashNode = &(q->priNode);
                 }
                 goto out;
             }
            }
#if 0
            else {
                 hashNode = &(q->webPriNode);
                 goto out;
            }
#endif
        }
#endif

    if(hashNode->qs.qlen >= dev->qos_max_queue_len) {
            return qdisc_drop(skb, sch);
    }
#endif
#endif

#if 0
    if(hashNode->overflowCount == uttEtherIfInfo[dev->utt_ifindex].oneSecCount) {
        /* big chain */
        if(hashNode->chainNo != UTT_SMART_QOS_CHAIN_LEVEL1) {
            list = &(q->rateChain[UTT_SMART_QOS_CHAIN_LEVEL1]);
            hashNode->chainNo = UTT_SMART_QOS_CHAIN_LEVEL1;
            tmpList = &(q->rateChain[UTT_SMART_QOS_CHAIN_LEVEL0]);
        }
    } else if(hashNode->overflowCount == 0){
        /* little chain */
        if(hashNode->chainNo != UTT_SMART_QOS_CHAIN_LEVEL0) {
            list = &(q->rateChain[UTT_SMART_QOS_CHAIN_LEVEL0]);
            tmpList = &(q->rateChain[UTT_SMART_QOS_CHAIN_LEVEL1]);
            hashNode->chainNo = UTT_SMART_QOS_CHAIN_LEVEL0;
        }
    }
    
    if(list != NULL) {
        if(hashNode->seclude == 0) {
            if(hashNode->nextListNode == hashNode) {
                tmpList->curr = NULL;
            } else {
                hashNode->prevListNode->nextListNode = hashNode->nextListNode;
                hashNode->nextListNode->prevListNode = hashNode->prevListNode;
                if(tmpList->curr == hashNode) {
                    tmpList->curr = hashNode->nextListNode;
                }
            }
        } else {
            hashNode->seclude = 0;
        }
        if(list->curr != NULL) {
            list->curr->prevListNode->nextListNode = hashNode;
            hashNode->prevListNode = list->curr->prevListNode;
            list->curr->prevListNode = hashNode;
            hashNode->nextListNode = list->curr;
        } else {
            list->curr = hashNode;
            list->curr->prevListNode = hashNode;
            list->curr->nextListNode = hashNode;
        }   
    }
#else

    /*
     * use node's rx and tx byte verdict which chain(qs) should use
     */

#if 0
    if((hashNode->rxBytes > (uttEtherIfInfo[dev->utt_ifindex].avgRxRate/2))
            || (hashNode->txBytes > (uttEtherIfInfo[dev->utt_ifindex].avgTxRate/2))) {
        /* big chain */
        if(hashNode->chainNo != UTT_SMART_QOS_CHAIN_LEVEL1) {
            list = &(q->rateChain[UTT_SMART_QOS_CHAIN_LEVEL1]);
            hashNode->chainNo = UTT_SMART_QOS_CHAIN_LEVEL1;
            tmpList = &(q->rateChain[UTT_SMART_QOS_CHAIN_LEVEL0]);
        }
    } else {
        /* little chain */
        if(hashNode->chainNo != UTT_SMART_QOS_CHAIN_LEVEL0) {
            list = &(q->rateChain[UTT_SMART_QOS_CHAIN_LEVEL0]);
            tmpList = &(q->rateChain[UTT_SMART_QOS_CHAIN_LEVEL1]);
            hashNode->chainNo = UTT_SMART_QOS_CHAIN_LEVEL0;
        }
    }
#else

    if(hashNode->preSecFlowStatus->usrIfFlowSta[dev->utt_ifindex].tmpTxByte < dev->qos_persec_min_txbytes
         && hashNode->preSecFlowStatus->usrIfFlowSta[dev->utt_ifindex].tmpRxByte < dev->qos_persec_min_rxbytes) {
            if(hashNode->chainNo != UTT_SMART_QOS_CHAIN_LEVEL0) {
                list = &(q->rateChain[UTT_SMART_QOS_CHAIN_LEVEL0]);
                tmpList = &(q->rateChain[UTT_SMART_QOS_CHAIN_LEVEL1]);
                hashNode->chainNo = UTT_SMART_QOS_CHAIN_LEVEL0;
            }
    } else if((hashNode->preSecFlowStatus->usrIfFlowSta[dev->utt_ifindex].rxByte 
                > (uttEtherIfInfo[dev->utt_ifindex].perSecPerIpRxBytes/2))
            || (hashNode->preSecFlowStatus->usrIfFlowSta[dev->utt_ifindex].txByte 
                > (uttEtherIfInfo[dev->utt_ifindex].perSecPerIpTxBytes/2))
            || (hashNode->rxBytes > (uttEtherIfInfo[dev->utt_ifindex].avgRxRate))
            || (hashNode->txBytes > (uttEtherIfInfo[dev->utt_ifindex].avgTxRate))) {
        if(hashNode->chainNo != UTT_SMART_QOS_CHAIN_LEVEL1) {
            list = &(q->rateChain[UTT_SMART_QOS_CHAIN_LEVEL1]);
            tmpList = &(q->rateChain[UTT_SMART_QOS_CHAIN_LEVEL0]);
            hashNode->chainNo = UTT_SMART_QOS_CHAIN_LEVEL1;
        }
    } else 
#if 0
        if((hashNode->preSecFlowStatus->usrIfFlowSta[dev->utt_ifindex].rxByte 
                <= (uttEtherIfInfo[dev->utt_ifindex].perSecPerIpRxBytes/2))
            && (hashNode->preSecFlowStatus->usrIfFlowSta[dev->utt_ifindex].txByte
                <= (uttEtherIfInfo[dev->utt_ifindex].perSecPerIpTxBytes)/2)) 
#endif
        {
        if(hashNode->chainNo != UTT_SMART_QOS_CHAIN_LEVEL0) {
            list = &(q->rateChain[UTT_SMART_QOS_CHAIN_LEVEL0]);
            tmpList = &(q->rateChain[UTT_SMART_QOS_CHAIN_LEVEL1]);
            hashNode->chainNo = UTT_SMART_QOS_CHAIN_LEVEL0;
        }
    }
#endif


    
    if(list != NULL) {
        if(hashNode->seclude == 0) {
            if(hashNode->nextListNode == hashNode) {
                tmpList->curr = NULL;
            } else {
                hashNode->prevListNode->nextListNode = hashNode->nextListNode;
                hashNode->nextListNode->prevListNode = hashNode->prevListNode;
                if(tmpList->curr == hashNode) {
                    tmpList->curr = hashNode->nextListNode;
                }
            }
        } else {
            hashNode->seclude = 0;
        }
        if(list->curr != NULL) {
            list->curr->prevListNode->nextListNode = hashNode;
            hashNode->prevListNode = list->curr->prevListNode;
            list->curr->prevListNode = hashNode;
            hashNode->nextListNode = list->curr;
        } else {
            list->curr = hashNode;
            list->curr->prevListNode = hashNode;
            list->curr->nextListNode = hashNode;
        }   
    }
#endif

out:
    sch->q.qlen++;
    sch->bstats.bytes += qdisc_pkt_len(skb);
    sch->bstats.packets++;
    sch->qstats.backlog += qdisc_pkt_len(skb);

    __skb_queue_tail(&(hashNode->qs), skb);
    return 0;

}

static struct sk_buff *
sfq_peek(struct Qdisc *sch)
{
    return NULL;
}

static struct sk_buff *
sfq_dequeue(struct Qdisc *sch)
{
    struct smart_sched_data *q = qdisc_priv(sch);
    struct sk_buff *skb;
    struct smart_list *list = NULL;
    struct net_device  *dev = sch->dev_queue->dev;
    struct smart_hash_node *hashNode = NULL;
    char chainNo = UTT_SMART_QOS_CHAIN_NULL;
    char isPriNode = 0, isUdpNode = 0, isNormalNode = 0;

    if(q->rateChain[UTT_SMART_QOS_CHAIN_LEVEL0].curr != NULL) {
        /*
         * sent little rate queue
         */
        list = &(q->rateChain[UTT_SMART_QOS_CHAIN_LEVEL0]);
        chainNo = UTT_SMART_QOS_CHAIN_LEVEL0;
        isNormalNode = 1;
    }
    else {
        /*
         * sent big rate queue
         */
        if(dev->qos_pertick_rxbytes <= 0 || dev->qos_pertick_txbytes <= 0) {
            return NULL;
        }
        else {
#if 1
            if(q->priNode.qs.qlen != 0){
                isPriNode = 1;
                hashNode = &(q->priNode);
                goto getskb;
            } else 
#endif
            {
#if 0
                if((unsigned int)jiffies - q->lastTxUseTime < 1) {
                    return NULL;
                } 
#endif
                list = &(q->rateChain[UTT_SMART_QOS_CHAIN_LEVEL1]);
                isNormalNode = 1;
            }
            if(list->curr == NULL) {
                if(q->priNode.qs.qlen != 0){
                isUdpNode = 1;
                hashNode = &(q->webPriNode);
                goto getskb;
                }
            }
        }
    }

    if(list->curr == NULL) {
        return NULL;
    }

    hashNode = list->curr;
getskb:
    if(hashNode->chainNo == UTT_SMART_QOS_CHAIN_LEVEL1 &&
            jiffies - hashNode->lastTxUseTime < dev->qos_delay) {
        if(isNormalNode == 1) {
            list->curr = hashNode->nextListNode;
        }
        return NULL;
    }
    skb = __skb_dequeue(&(hashNode->qs));
    if(skb != NULL) {
        hashNode = skb->qosHash;
        if(isPriNode == 1){
            (hashNode->httpPackCount)--;
        }
    } 
    hashNode->lastTxUseTime = jiffies;
    if((unsigned int)jiffies - hashNode->lastClrTime >= dev->qos_clear_interval) {
        hashNode->rxBytes = 0;
        hashNode->txBytes = 0;
        hashNode->lastClrTime = jiffies;
    }

    if(skb != NULL) {
        hashNode->txBytes += skb->len;
    }

    hashNode->lastUseTime = jiffies;
#if 1
    if(isNormalNode == 1) {
    if(hashNode->qs.qlen == 0) {
        /*
         * this node have 0 skb,so we del it form list
         */
        hashNode->chainNo = UTT_SMART_QOS_CHAIN_NULL;
        if(hashNode->nextListNode == hashNode) {

            /*
             * only have one node
             */
            list->curr = NULL;
        } else {
            hashNode->prevListNode->nextListNode = hashNode->nextListNode;
            hashNode->nextListNode->prevListNode = hashNode->prevListNode;
            list->curr = hashNode->nextListNode;
        }
        hashNode->seclude = 1;
    } else 
#endif
    {
        /*
         * current point it's next
         */
        list->curr = hashNode->nextListNode;
    }
    }
    (sch->q.qlen)--;
    if(skb != NULL) {
        sch->qstats.backlog -= qdisc_pkt_len(skb);
    }
    return skb;
}

static void
sfq_reset(struct Qdisc *sch)
{
#if 0
	struct sk_buff *skb;

	while ((skb = sfq_dequeue(sch)) != NULL)
		kfree_skb(skb);
#endif
}

static int sfq_change(struct Qdisc *sch, struct nlattr *opt)
{
#if 0
	struct sfq_sched_data *q = qdisc_priv(sch);
	struct tc_sfq_qopt *ctl = nla_data(opt);
	unsigned int qlen;

	if (opt->nla_len < nla_attr_size(sizeof(*ctl)))
		return -EINVAL;

	sch_tree_lock(sch);
	q->quantum = ctl->quantum ? : psched_mtu(qdisc_dev(sch));
	q->perturb_period = ctl->perturb_period * HZ;
	if (ctl->limit)
		q->limit = min_t(u32, ctl->limit, SFQ_DEPTH - 1);

	qlen = sch->q.qlen;
	while (sch->q.qlen > q->limit)
		sfq_drop(sch);
	qdisc_tree_decrease_qlen(sch, qlen - sch->q.qlen);

	del_timer(&q->perturb_timer);
	if (q->perturb_period) {
		mod_timer(&q->perturb_timer, jiffies + q->perturb_period);
		q->perturbation = net_random();
	}
	sch_tree_unlock(sch);
#endif
	return 0;
}

static struct timer_list utt_smartQos_statistics;
static void uttSmartQosStaTask(void)
{
    int i, j;
    struct smart_hash_node *tmpNode, *prevNode, *node;

    /*
     * del some timeout hashnode and do some compute
     */
    for(i = 0; i < UTT_MAX_IF_NUM; i++) {
        for(j = 0; j < MAX_HASH_TABLE; j++) {
            prevNode = NULL;
            tmpNode = uttEtherIfInfo[i].smartQosHash[j];
            while(tmpNode != NULL) {
//                printk("have a not null node %x %x\n", j, ntohl(tmpNode->sip));
                if(jiffies < tmpNode->lastUseTime) {
                    tmpNode->lastUseTime = jiffies;
                    tmpNode = tmpNode->nextHashNode;
                } else {
                    if(((unsigned int)jiffies - tmpNode->lastUseTime > uttEtherIfInfo[i].maxTimeout) && (tmpNode->seclude == 1)) {
#if 0
                        printk("have a node need free  %d %d %d\n",jiffies, tmpNode->lastUseTime,uttEtherIfInfo[i].maxTimeout);
#endif
                        /*
                         * dec online num, only dec when > 0
                         */

                        if(tmpNode->etherProtocol == htons(ETH_P_IP) 
                                && uttEtherIfInfo[i].onlineCliCount > 0) {
                            uttEtherIfInfo[i].onlineCliCount--;
                        }

                        /*judge prevNode is NULL*/
                        if(prevNode == NULL)
                            uttEtherIfInfo[i].smartQosHash[j] = tmpNode->nextHashNode;
                        else 
                            prevNode->nextHashNode = tmpNode->nextHashNode;
                        node = tmpNode;
                        tmpNode = tmpNode->nextHashNode;
                        /*
                         *if qs's len isn't 0 need free skb
                         */
#if 0
                        if(tmpNode->qs.qlen != 0) {
                        }
#endif
                        /*
                         * free node
                         */
                        decUserFlowStatusQuoteCnt(node);
                        kfree(node);
                    } else {
                        /*only do next node*/
                        prevNode = tmpNode;
                        tmpNode = tmpNode->nextHashNode;
                    }
                }
            }
        }

        /*
         * computer per rate
         */
        if(uttEtherIfInfo[i].onlineCliCount != 0) {
            uttEtherIfInfo[i].perSecPerIpTxBytes = uttEtherIfInfo[i].avgTxRate 
             = uttEtherIfInfo[i].maxTxRate/uttEtherIfInfo[i].onlineCliCount;
            uttEtherIfInfo[i].avgTxRate = (uttEtherIfInfo[i].avgTxRate/HZ) * uttEtherIfInfo[i].clear_interval;
            uttEtherIfInfo[i].perSecPerIpRxBytes = uttEtherIfInfo[i].avgRxRate 
             = uttEtherIfInfo[i].maxRxRate/uttEtherIfInfo[i].onlineCliCount;
            uttEtherIfInfo[i].avgRxRate = (uttEtherIfInfo[i].avgRxRate/HZ) * uttEtherIfInfo[i].clear_interval;
        } else {
            uttEtherIfInfo[i].perSecPerIpTxBytes = uttEtherIfInfo[i].avgTxRate 
                = (uttEtherIfInfo[i].maxTxRate/HZ) * uttEtherIfInfo[i].clear_interval;
            uttEtherIfInfo[i].perSecPerIpRxBytes = uttEtherIfInfo[i].avgRxRate 
                = (uttEtherIfInfo[i].maxRxRate/HZ) * uttEtherIfInfo[i].clear_interval;
        }

        if(uttEtherIfInfo[i].clear_interval != 0 && uttEtherIfInfo[i].clear_interval <= HZ) {
            uttEtherIfInfo[i].oneSecCount =  HZ /uttEtherIfInfo[i].clear_interval;
        } else {
            uttEtherIfInfo[i].oneSecCount = 1;
        }
#if 0
        printk("%d person, tx %d rx %d\n",uttEtherIfInfo[i].onlineCliCount,
               uttEtherIfInfo[i].avgTxRate ,
               uttEtherIfInfo[i].avgRxRate 
               );
#endif
    }

    /*
     * add time for next task
     */
    mod_timer(&utt_smartQos_statistics, jiffies + 10*HZ);
}

static int sfq_init(struct Qdisc *sch, struct nlattr *opt)
{
    struct smart_sched_data *q = qdisc_priv(sch);
#if 0
    int j = 0;
#endif
    struct net_device  *dev = sch->dev_queue->dev;
    if(dev->utt_ifindex < UTT_MAX_IF_NUM && dev->utt_ifindex > 0){
#if 0
        for(j = 0; j < MAX_HASH_TABLE; j++) {
            uttEtherIfInfo[dev->utt_ifindex].smartQosHash[j] = NULL;
        }
        uttEtherIfInfo[dev->utt_ifindex].onlineCliCount = 0;
#endif
        uttEtherIfInfo[dev->utt_ifindex].maxTimeout = HZ * 60;
    }
    q->rateChain[UTT_SMART_QOS_CHAIN_LEVEL0].curr = NULL;
    q->rateChain[UTT_SMART_QOS_CHAIN_LEVEL1].curr = NULL;
    skb_queue_head_init(&((q->priNode).qs));
    skb_queue_head_init(&((q->webPriNode).qs));
    q->priNode.chainNo = UTT_SMART_QOS_CHAIN_NULL;
    return 0;
}

static void sfq_destroy(struct Qdisc *sch)
{
    struct smart_sched_data *q = qdisc_priv(sch);
#if 0
    int j = 0;
    struct smart_hash_node *tmpNode, *nextNode;
    struct net_device  *dev = sch->dev_queue->dev;
    if(dev->utt_ifindex < UTT_MAX_IF_NUM && dev->utt_ifindex > 0) {
        for(j = 0; j < MAX_HASH_TABLE; j++) {
            tmpNode = uttEtherIfInfo[dev->utt_ifindex].smartQosHash[j];
            while(tmpNode != NULL) {
               nextNode = tmpNode->nextHashNode;
               kfree(tmpNode);
               tmpNode = nextNode;
            }
            uttEtherIfInfo[dev->utt_ifindex].smartQosHash[j] = NULL;
        }
    }
#endif
    q->rateChain[UTT_SMART_QOS_CHAIN_LEVEL0].curr = NULL;
    q->rateChain[UTT_SMART_QOS_CHAIN_LEVEL1].curr = NULL;
}

static int sfq_dump(struct Qdisc *sch, struct sk_buff *skb)
{
	return 1;
}

#if 0
static unsigned long sfq_get(struct Qdisc *sch, u32 classid)
{
	return 0;
}

static struct tcf_proto **sfq_find_tcf(struct Qdisc *sch, unsigned long cl)
{
	struct sfq_sched_data *q = qdisc_priv(sch);

	if (cl)
		return NULL;
	return &q->filter_list;
}

static int sfq_dump_class(struct Qdisc *sch, unsigned long cl,
			  struct sk_buff *skb, struct tcmsg *tcm)
{
	tcm->tcm_handle |= TC_H_MIN(cl);
	return 0;
}

static int sfq_dump_class_stats(struct Qdisc *sch, unsigned long cl,
				struct gnet_dump *d)
{
	struct sfq_sched_data *q = qdisc_priv(sch);
	sfq_index idx = q->ht[cl-1];
	struct gnet_stats_queue qs = { .qlen = q->qs[idx].qlen };
	struct tc_sfq_xstats xstats = { .allot = q->allot[idx] };

	if (gnet_stats_copy_queue(d, &qs) < 0)
		return -1;
	return gnet_stats_copy_app(d, &xstats, sizeof(xstats));
}

static void sfq_walk(struct Qdisc *sch, struct qdisc_walker *arg)
{
	struct sfq_sched_data *q = qdisc_priv(sch);
	unsigned int i;

	if (arg->stop)
		return;

	for (i = 0; i < SFQ_HASH_DIVISOR; i++) {
		if (q->ht[i] == SFQ_DEPTH ||
		    arg->count < arg->skip) {
			arg->count++;
			continue;
		}
		if (arg->fn(sch, i + 1, arg) < 0) {
			arg->stop = 1;
			break;
		}
		arg->count++;
	}
}

static const struct Qdisc_class_ops sfq_class_ops = {
	.get		=	sfq_get,
	.tcf_chain	=	sfq_find_tcf,
	.dump		=	sfq_dump_class,
	.dump_stats	=	sfq_dump_class_stats,
	.walk		=	sfq_walk,
};
#endif


static struct Qdisc_ops sfq_qdisc_ops __read_mostly = {
	.id		=	"smartQos",
	.priv_size	=	sizeof(struct smart_sched_data),
	.enqueue	=	sfq_enqueue,
	.dequeue	=	sfq_dequeue,
	.peek		=	sfq_peek,
	.drop		=	sfq_drop,
	.init		=	sfq_init,
	.reset		=	sfq_reset,
	.destroy	=	sfq_destroy,
	.change		=	NULL,
	.dump		=	sfq_dump,
	.owner		=	THIS_MODULE,
};

static int __init sfq_module_init(void)
{
    int rc = 0;
    init_timer(&utt_smartQos_statistics);
    utt_smartQos_statistics.function = uttSmartQosStaTask;
    utt_smartQos_statistics.expires = jiffies + 10*HZ;
    add_timer(&utt_smartQos_statistics);
    rc = register_qdisc(&sfq_qdisc_ops);

    if(rc != 0) {
        del_timer(&utt_smartQos_statistics);
    }
    return rc;
}
static void __exit sfq_module_exit(void)
{
    int i = 0, j = 0;
    struct smart_hash_node *tmpNode, *nextNode;
    for(i = 0; i < UTT_MAX_IF_NUM; i++) {
        for(j = 0; j < MAX_HASH_TABLE; j++) {
            tmpNode = uttEtherIfInfo[i].smartQosHash[j];
            while(tmpNode != NULL) {
               nextNode = tmpNode->nextHashNode;
               kfree(tmpNode);
               tmpNode = nextNode;
            }
        }
    }
    del_timer(&utt_smartQos_statistics);
    unregister_qdisc(&sfq_qdisc_ops);
}
module_init(sfq_module_init)
module_exit(sfq_module_exit)
MODULE_LICENSE("GPL");
