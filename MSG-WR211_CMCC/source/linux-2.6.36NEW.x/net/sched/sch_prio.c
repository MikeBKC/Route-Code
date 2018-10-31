/*
 * net/sched/sch_prio.c	Simple 3-band priority "scheduler".
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 *
 * Authors:	Alexey Kuznetsov, <kuznet@ms2.inr.ac.ru>
 * Fixes:       19990609: J Hadi Salim <hadi@nortelnetworks.com>:
 *              Init --  EINVAL when opt undefined
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/skbuff.h>
#include <net/netlink.h>
#include <net/pkt_sched.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <net/sch_utt.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_CONNMARK.h>
#include <net/netfilter/nf_conntrack_ecache.h>


struct prio_sched_data
{
	int bands;
	struct tcf_proto *filter_list;
	u8  prio2band[TC_PRIO_MAX+1];
	struct Qdisc *queues[TCQ_PRIO_BANDS];
};


static struct Qdisc *
prio_classify(struct sk_buff *skb, struct Qdisc *sch, int *qerr)
{
	struct prio_sched_data *q = qdisc_priv(sch);
	u32 band = skb->priority;
	struct tcf_result res;
	int err;

	*qerr = NET_XMIT_SUCCESS | __NET_XMIT_BYPASS;
	if (TC_H_MAJ(skb->priority) != sch->handle) {
		err = tc_classify(skb, q->filter_list, &res);
#ifdef CONFIG_NET_CLS_ACT
		switch (err) {
		case TC_ACT_STOLEN:
		case TC_ACT_QUEUED:
			*qerr = NET_XMIT_SUCCESS | __NET_XMIT_STOLEN;
		case TC_ACT_SHOT:
			return NULL;
		}
#endif
		if (!q->filter_list || err < 0) {
			if (TC_H_MAJ(band))
				band = 0;
			return q->queues[q->prio2band[band&TC_PRIO_MAX]];
		}
		band = res.classid;
	}
	band = TC_H_MIN(band) - 1;
	if (band >= q->bands)
		return q->queues[q->prio2band[0]];

	return q->queues[band];
}

enum HTTP_HEAD_INFO{
    HTTP_HEAD_GET,
    HTTP_HEAD_POST
};

int proto_hdr_http[] = {
#if defined(__LITTLE_ENDIAN_BITFIELD)
    0x20544547,  /* GET */
    0x54534F50  /* POST */
#elif defined(__BIG_ENDIAN_BITFIELD)
    0x47455420,  /* GET */
    0x504F5354  /* POST */
#else
#error	"Adjust your <asm/byteorder.h> defines"
#endif	
};

static void skb_distinguish(struct iphdr *iph, struct sk_buff *skb)
{
    struct tcphdr *tcph = NULL;
    unsigned int tcph_len = 0;
    char * tcpData = NULL;
    struct nf_conn *ct;
    enum ip_conntrack_info ctinfo;

    if(iph->protocol == IPPROTO_TCP) {
	tcph = (struct tcphdr *)((char*)iph + iph->ihl * 4);
        tcph_len = (tcph->doff) * 4;;
        if(tcph->syn == 1) {
            skb->mark = QOS_WEB_CONNMARK;
        } else {
            tcpData = (char*)((char*)tcph + tcph_len);
            if(tcpData + 4 < skb->tail) {
                if(*((int*)tcpData) == proto_hdr_http[HTTP_HEAD_GET] ||
                   *((int*)tcpData) == proto_hdr_http[HTTP_HEAD_POST]) {
		    skb->mark = QOS_WEB_CONNMARK;
		    ct = nf_ct_get(skb, &ctinfo);
		    if(ct != NULL)
		    {/*one connect may be have multi get or post 
		       and if much more post packets after one html get quest these packets will be speed up
		       so we clear qos connet mark after one get or post
		       add by bhou*/
			ct->mark  = ((ct->mark) &(~QOS_CONNMARK_MSK));
		    }

		}
	    }
        }
    }
    return;
}

extern struct utt_wan_if_info   uttEtherIfInfo[UTT_MAX_IF_NUM];
extern struct netFlowStatus* uttPerIpPerSecFlowSta[MAX_HASH_TABLE];
static int
prio_enqueue(struct sk_buff *skb, struct Qdisc *sch)
{
	struct Qdisc *qdisc;
	int ret;
#if 1
    unsigned int hash;
    unsigned int sIP;
    const struct iphdr *iph = NULL;
    struct smart_hash_node *hashNode = NULL, *head = NULL;
    struct netFlowStatus *headFlowStatus;

    struct net_device *dev;

    dev = sch->dev_queue->dev;
#endif
/*
 * begin edit for status for flow
 */
#if 1
    switch (skb->protocol) {
        case htons(ETH_P_IP):
        {
            iph = ip_hdr(skb);
            if(skb->old_ip == 0 || skb->utt_iif > 0) {
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

    hash = sIP & 0xff;

    head = uttEtherIfInfo[dev->utt_ifindex].smartQosHash[hash];
    while(head != NULL) {
        if(head->sip == sIP) {
            hashNode = head;
            break;
        } else {
            head = head->nextHashNode;
        }
    }

    if(hashNode == NULL && skb->utt_iif == 0) {
        /*
         * cread a hash node and insert to hashtable
         */
        hashNode = kzalloc(sizeof(struct smart_hash_node), GFP_ATOMIC);
        if(hashNode == NULL) {
	    return qdisc_drop(skb, sch);
        }
        /*get flow Status Node*/
        headFlowStatus = uttPerIpPerSecFlowSta[hash];

        while(headFlowStatus != NULL) {
            if(headFlowStatus->sip == sIP) {
                break;
            } else {
                headFlowStatus = headFlowStatus->next;
            }
        }

        if(headFlowStatus == NULL) {
            headFlowStatus = kzalloc(sizeof(struct netFlowStatus), GFP_ATOMIC);
            if(headFlowStatus != NULL) {
                headFlowStatus->next = uttPerIpPerSecFlowSta[hash];
                if(uttPerIpPerSecFlowSta[hash] != NULL) {
                    uttPerIpPerSecFlowSta[hash]->prev = headFlowStatus;
                }
                uttPerIpPerSecFlowSta[hash] = headFlowStatus;
                headFlowStatus->sip = sIP;
#if 0
                printk("new hash %pI4\n", &sIP);
#endif
            }
        }

        if(headFlowStatus == NULL) {
            kfree(hashNode);
	    return qdisc_drop(skb, sch);
        }

        (headFlowStatus->quoteCnt)++;
        hashNode->preSecFlowStatus = headFlowStatus;

        hashNode->chainNo = UTT_SMART_QOS_CHAIN_NULL;
        hashNode->sip = sIP;

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

        hashNode->nextHashNode = uttEtherIfInfo[dev->utt_ifindex].smartQosHash[hash];
        uttEtherIfInfo[dev->utt_ifindex].smartQosHash[hash] = hashNode;
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

#if 0
    if((unsigned int)jiffies - hashNode->lastClrOneSecTime >= HZ) {
        hashNode->perSecTxBytes = hashNode->tmpPerSecTxBytes;
        hashNode->tmpPerSecTxBytes = skb->len;
        hashNode->lastClrOneSecTime = jiffies;
    } else {
        hashNode->tmpPerSecTxBytes += skb->len;
    }
        hashNode->tmpPerSecTxBytes += skb->len;
 
    if(skb->protocol == htons(ETH_P_IP)) {
        iph = ip_hdr(skb);
    }
#endif
    if(hashNode != NULL) {
        hashNode->lastUseTime = jiffies;

        if(iph != NULL) {
            if(iph->protocol == IPPROTO_TCP) {
                skb_distinguish(iph, skb);
#if 0
                struct tcphdr *tcph = (struct tcphdr *)(iph+1);
 
                /*edit by brwang for web vedio*/
                if(tcph->dest == ntohs(80) 
                        && hashNode->lesslimitCnt >= 5) {
                   skb->mark = 10;
                }
#endif
            } else if(iph->protocol == IPPROTO_UDP){
                struct udphdr *udph = (struct udphdr *)(iph+1);
                if(udph->dest == ntohs(53)) {
                   skb->mark = QOS_WEB_CONNMARK;
                }
            } else if(iph->protocol == IPPROTO_ICMP){
                   skb->mark = QOS_WEB_CONNMARK;
            }
        }
    }
    skb->qosHash = hashNode;

#endif
	qdisc = prio_classify(skb, sch, &ret);
#ifdef CONFIG_NET_CLS_ACT
	if (qdisc == NULL) {

		if (ret & __NET_XMIT_BYPASS)
			sch->qstats.drops++;
		kfree_skb(skb);
		return ret;
	}
#endif

	ret = qdisc_enqueue(skb, qdisc);
	if (ret == NET_XMIT_SUCCESS) {
		sch->bstats.bytes += qdisc_pkt_len(skb);
		sch->bstats.packets++;
		sch->q.qlen++;
		return NET_XMIT_SUCCESS;
	}
	if (net_xmit_drop_count(ret))
		sch->qstats.drops++;
	return ret;
}

static struct sk_buff *prio_peek(struct Qdisc *sch)
{
	struct prio_sched_data *q = qdisc_priv(sch);
	int prio;

	for (prio = 0; prio < q->bands; prio++) {
		struct Qdisc *qdisc = q->queues[prio];
		struct sk_buff *skb = qdisc->ops->peek(qdisc);
		if (skb)
			return skb;
	}
	return NULL;
}

#if 0
static struct sk_buff *prio_dequeue(struct Qdisc* sch)
{
	struct prio_sched_data *q = qdisc_priv(sch);
	int prio;

	for (prio = 0; prio < q->bands; prio++) {
		struct Qdisc *qdisc = q->queues[prio];
		struct sk_buff *skb = qdisc->dequeue(qdisc);
		if (skb) {
			sch->q.qlen--;
			return skb;
		}
	}
	return NULL;

}
#else
static struct sk_buff *prio_dequeue(struct Qdisc* sch)
{
	struct prio_sched_data *q = qdisc_priv(sch);
	int prio;

	unsigned long jiffieslong = 1;
	int tmpBands;

	struct in_device *in_dev;
	struct net_device *dev;
        static jiffiesover = HZ/2;
        struct smart_hash_node *hashNode = NULL;


        dev = sch->dev_queue->dev;
	tmpBands = q->bands;

        if(dev == NULL) {
            return NULL;
        }

	if((dev->utt_ifindex != 0) && (dev->uttQosEnable == 1)) {

		if(dev->qos_jiffies != jiffies) {
			if(dev->qos_jiffies < jiffies) {
				jiffieslong = jiffies - dev->qos_jiffies;
				if(jiffieslong > jiffiesover) {
					jiffieslong = jiffiesover;
				}
				dev->qos_pertick_rxbytes += (int)(dev->qos_pertick_max_rxbytes * ((int)jiffieslong));
				dev->qos_pertick_txbytes += (int)(dev->qos_pertick_max_txbytes * ((int)jiffieslong));
			} else {
				dev->qos_pertick_rxbytes += (int)(dev->qos_pertick_max_rxbytes);
				dev->qos_pertick_txbytes += (int)(dev->qos_pertick_max_txbytes);
			}

			if(dev->qos_pertick_rxbytes > dev->qos_max_credit_rx) {
				dev->qos_pertick_rxbytes = dev->qos_max_credit_rx;
			}

			if(dev->qos_pertick_txbytes > dev->qos_max_credit_tx) {
				dev->qos_pertick_txbytes = dev->qos_max_credit_tx;
			}
			dev->qos_jiffies = jiffies;
		}

		if(dev->qos_pertick_rxbytes < 0 || dev->qos_pertick_txbytes < 0) {
			tmpBands = 2;
		} else {
			tmpBands = q->bands;
		}
                if(uttEtherIfInfo[dev->utt_ifindex].qos_up_drop != 0) {
                if(dev->qos_pertick_txbytes < 0)
                    return NULL;
                }
	}
	
	for (prio = 0; prio < q->bands; prio++) {

#if 0
                if(skb->mark == 0xb) {
                    if((unsigned int)jiffies - uttEtherIfInfo[skb->utt_iif].lastRxWeb >= HZ) {
                         uttEtherIfInfo[skb->utt_iif].webRxBytes = 0;
                         uttEtherIfInfo[skb->utt_iif].lastRxWeb = jiffies;
                    }
                }
                if(prio == 1 && uttEtherIfInfo[dev->utt_iif].webRxBytes > dev->qos_persec_max_rxbytes/2) {
                    continue;
                }
#endif
		struct Qdisc *qdisc = q->queues[prio];
		struct sk_buff *skb = qdisc->dequeue(qdisc);
		if (skb) {
                        hashNode = skb->qosHash;
                        if(hashNode != NULL && dev->utt_ifindex != 0) {
#if 1
                            if((unsigned int)jiffies - 
                                    hashNode->preSecFlowStatus->usrIfFlowSta[dev->utt_ifindex].lastClrJiffies >= HZ) {
                                statisPerSecPerUserFlowSta(hashNode, dev->utt_ifindex, UTT_SMART_QOS_TX, skb->len);
                            } else {
                                hashNode->preSecFlowStatus->usrIfFlowSta[dev->utt_ifindex].tmpTxByte += skb->len;
                            }
#endif
                            hashNode->lastUseTime = jiffies;
                        }
                        if(dev->uttQosEnable == 1) {
                            if(dev->qos_pertick_txbytes > 0)
                            dev->qos_pertick_txbytes -= skb->len;
                        }
			sch->q.qlen--;
			return skb;
		}
	}
	return NULL;

}

#endif


static unsigned int prio_drop(struct Qdisc* sch)
{
	struct prio_sched_data *q = qdisc_priv(sch);
	int prio;
	unsigned int len;
	struct Qdisc *qdisc;

	for (prio = q->bands-1; prio >= 0; prio--) {
		qdisc = q->queues[prio];
		if (qdisc->ops->drop && (len = qdisc->ops->drop(qdisc)) != 0) {
			sch->q.qlen--;
			return len;
		}
	}
	return 0;
}


static void
prio_reset(struct Qdisc* sch)
{
	int prio;
	struct prio_sched_data *q = qdisc_priv(sch);

	for (prio=0; prio<q->bands; prio++)
		qdisc_reset(q->queues[prio]);
	sch->q.qlen = 0;
}

static void
prio_destroy(struct Qdisc* sch)
{
	int prio, j;
	struct prio_sched_data *q = qdisc_priv(sch);
        struct net_device  *dev = sch->dev_queue->dev;

        struct smart_hash_node *tmpNode, *nextNode;


	tcf_destroy_chain(&q->filter_list);
	for (prio=0; prio<q->bands; prio++)
		qdisc_destroy(q->queues[prio]);

    if(dev->utt_ifindex < UTT_MAX_IF_NUM && dev->utt_ifindex > 0) {
        for(j = 0; j < MAX_HASH_TABLE; j++) {
            tmpNode = uttEtherIfInfo[dev->utt_ifindex].smartQosHash[j];
            while(tmpNode != NULL) {
                decUserFlowStatusQuoteCnt(tmpNode);
                nextNode = tmpNode->nextHashNode;

#if 0
                while(tmpNode->qs.qlen > 0) {
                    skb = __skb_dequeue(&(tmpNode->qs));
                    if(skb != NULL) {
                        kfree_skb(skb);
                    }
                }
#endif

                kfree(tmpNode);
                tmpNode = nextNode;
            }
            uttEtherIfInfo[dev->utt_ifindex].smartQosHash[j] = NULL;
        }
        uttEtherIfInfo[dev->utt_ifindex].onlineCliCount = 0;
    }

#if 0
    local_irq_enable();
#endif

}

static int prio_tune(struct Qdisc *sch, struct nlattr *opt)
{
	struct prio_sched_data *q = qdisc_priv(sch);
	struct tc_prio_qopt *qopt;
	int i;

	if (nla_len(opt) < sizeof(*qopt))
		return -EINVAL;
	qopt = nla_data(opt);

	if (qopt->bands > TCQ_PRIO_BANDS || qopt->bands < 2)
		return -EINVAL;

	for (i=0; i<=TC_PRIO_MAX; i++) {
		if (qopt->priomap[i] >= qopt->bands)
			return -EINVAL;
	}

	sch_tree_lock(sch);
	q->bands = qopt->bands;
	memcpy(q->prio2band, qopt->priomap, TC_PRIO_MAX+1);

	for (i=q->bands; i<TCQ_PRIO_BANDS; i++) {
		struct Qdisc *child = q->queues[i];
		q->queues[i] = &noop_qdisc;
		if (child != &noop_qdisc) {
			qdisc_tree_decrease_qlen(child, child->q.qlen);
			qdisc_destroy(child);
		}
	}
	sch_tree_unlock(sch);

	for (i=0; i<q->bands; i++) {
		if (q->queues[i] == &noop_qdisc) {
			struct Qdisc *child, *old;
			child = qdisc_create_dflt(qdisc_dev(sch), sch->dev_queue,
						  &pfifo_qdisc_ops,
						  TC_H_MAKE(sch->handle, i + 1));
			if (child) {
				sch_tree_lock(sch);
				old = q->queues[i];
				q->queues[i] = child;

				if (old != &noop_qdisc) {
					qdisc_tree_decrease_qlen(old,
								 old->q.qlen);
					qdisc_destroy(old);
				}
				sch_tree_unlock(sch);
			}
		}
	}
	return 0;
}

static int prio_init(struct Qdisc *sch, struct nlattr *opt)
{
	struct prio_sched_data *q = qdisc_priv(sch);
	int i, j;
        struct net_device  *dev = sch->dev_queue->dev;

	for (i=0; i<TCQ_PRIO_BANDS; i++)
		q->queues[i] = &noop_qdisc;

    if(dev->utt_ifindex < UTT_MAX_IF_NUM && dev->utt_ifindex > 0){
        for(j = 0; j < MAX_HASH_TABLE; j++) {
            uttEtherIfInfo[dev->utt_ifindex].smartQosHash[j] = NULL;
        }
    }
    uttEtherIfInfo[dev->utt_ifindex].onlineCliCount = 0;

	if (opt == NULL) {
		return -EINVAL;
	} else {
		int err;

		if ((err= prio_tune(sch, opt)) != 0)
			return err;
	}
	return 0;
}

static int prio_dump(struct Qdisc *sch, struct sk_buff *skb)
{
	struct prio_sched_data *q = qdisc_priv(sch);
	unsigned char *b = skb_tail_pointer(skb);
	struct tc_prio_qopt opt;

	opt.bands = q->bands;
	memcpy(&opt.priomap, q->prio2band, TC_PRIO_MAX+1);

	NLA_PUT(skb, TCA_OPTIONS, sizeof(opt), &opt);

	return skb->len;

nla_put_failure:
	nlmsg_trim(skb, b);
	return -1;
}

static int prio_graft(struct Qdisc *sch, unsigned long arg, struct Qdisc *new,
		      struct Qdisc **old)
{
	struct prio_sched_data *q = qdisc_priv(sch);
	unsigned long band = arg - 1;

	if (new == NULL)
		new = &noop_qdisc;

	sch_tree_lock(sch);
	*old = q->queues[band];
	q->queues[band] = new;
	qdisc_tree_decrease_qlen(*old, (*old)->q.qlen);
	qdisc_reset(*old);
	sch_tree_unlock(sch);

	return 0;
}

static struct Qdisc *
prio_leaf(struct Qdisc *sch, unsigned long arg)
{
	struct prio_sched_data *q = qdisc_priv(sch);
	unsigned long band = arg - 1;

	return q->queues[band];
}

static unsigned long prio_get(struct Qdisc *sch, u32 classid)
{
	struct prio_sched_data *q = qdisc_priv(sch);
	unsigned long band = TC_H_MIN(classid);

	if (band - 1 >= q->bands)
		return 0;
	return band;
}

static unsigned long prio_bind(struct Qdisc *sch, unsigned long parent, u32 classid)
{
	return prio_get(sch, classid);
}


static void prio_put(struct Qdisc *q, unsigned long cl)
{
}

static int prio_dump_class(struct Qdisc *sch, unsigned long cl, struct sk_buff *skb,
			   struct tcmsg *tcm)
{
	struct prio_sched_data *q = qdisc_priv(sch);

	tcm->tcm_handle |= TC_H_MIN(cl);
	tcm->tcm_info = q->queues[cl-1]->handle;
	return 0;
}

static int prio_dump_class_stats(struct Qdisc *sch, unsigned long cl,
				 struct gnet_dump *d)
{
	struct prio_sched_data *q = qdisc_priv(sch);
	struct Qdisc *cl_q;

	cl_q = q->queues[cl - 1];
	cl_q->qstats.qlen = cl_q->q.qlen;
	if (gnet_stats_copy_basic(d, &cl_q->bstats) < 0 ||
	    gnet_stats_copy_queue(d, &cl_q->qstats) < 0)
		return -1;

	return 0;
}

static void prio_walk(struct Qdisc *sch, struct qdisc_walker *arg)
{
	struct prio_sched_data *q = qdisc_priv(sch);
	int prio;

	if (arg->stop)
		return;

	for (prio = 0; prio < q->bands; prio++) {
		if (arg->count < arg->skip) {
			arg->count++;
			continue;
		}
		if (arg->fn(sch, prio+1, arg) < 0) {
			arg->stop = 1;
			break;
		}
		arg->count++;
	}
}

static struct tcf_proto ** prio_find_tcf(struct Qdisc *sch, unsigned long cl)
{
	struct prio_sched_data *q = qdisc_priv(sch);

	if (cl)
		return NULL;
	return &q->filter_list;
}

static const struct Qdisc_class_ops prio_class_ops = {
	.graft		=	prio_graft,
	.leaf		=	prio_leaf,
	.get		=	prio_get,
	.put		=	prio_put,
	.walk		=	prio_walk,
	.tcf_chain	=	prio_find_tcf,
	.bind_tcf	=	prio_bind,
	.unbind_tcf	=	prio_put,
	.dump		=	prio_dump_class,
	.dump_stats	=	prio_dump_class_stats,
};

struct Qdisc_ops prio_qdisc_ops __read_mostly = {
	.next		=	NULL,
	.cl_ops		=	&prio_class_ops,
	.id		=	"prio",
	.priv_size	=	sizeof(struct prio_sched_data),
	.enqueue	=	prio_enqueue,
	.dequeue	=	prio_dequeue,
	.peek		=	prio_peek,
	.drop		=	prio_drop,
	.init		=	prio_init,
	.reset		=	prio_reset,
	.destroy	=	prio_destroy,
	.change		=	prio_tune,
	.dump		=	prio_dump,
	.owner		=	THIS_MODULE,
};


#if 0
int flowStatus_read( char *page, char **start, off_t off,
                           int count, int *eof, void *data )
{
    int len = 0, j;
    struct smart_hash_node *tmpNode;
    for(j = 0; j < MAX_HASH_TABLE; j++) {
        tmpNode = uttEtherIfInfo[1].smartQosHash[j];
        while(tmpNode != NULL) {
            len += sprintf(page, "%x   %d   %x\n", tmpNode->sip, tmpNode->perSecTxBytes, tmpNode->perSecRxBytes);
            tmpNode = tmpNode->nextHashNode;
        }
    }

    return len;
}

static int flowSIndex = 0;
static void *flowS_seq_start(struct seq_file *s, loff_t *pos)
{
    struct smart_hash_node *tmpNode;
    if(*pos == 0) {
        for(flowSIndex = 0; flowSIndex < MAX_HASH_TABLE; flowSIndex++) {
            tmpNode = uttEtherIfInfo[1].smartQosHash[flowSIndex];
            while(tmpNode != NULL) {
                return tmpNode;
            }
        }
        return NULL;
    } else {
        *pos = 0;
        return NULL;
    }
}

void *flowS_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
    struct smart_hash_node *tmpNode;
    int find = 0;
    tmpNode = (struct smart_hash_node *)(v);
    if(tmpNode->nextHashNode != NULL) {
        v = tmpNode->nextHashNode;
        find = 1;
    } else {
        for(flowSIndex = (flowSIndex + 1); (flowSIndex < MAX_HASH_TABLE) && (find == 0); flowSIndex++) {
            tmpNode = uttEtherIfInfo[1].smartQosHash[flowSIndex];
            while(tmpNode != NULL) {
                v = tmpNode;
                find = 1;
                break;
            }
        }
    }
    (*pos)++;
    if(find == 1)
        return v;
    else 
        return NULL;
}

static int flowS_seq_show(struct seq_file *seq, void *v)
{
    struct smart_hash_node *tmpNode;
    tmpNode = (struct smart_hash_node *)(v);
    seq_printf(seq, "%pI4 %d %d\n",&tmpNode->sip, tmpNode->perSecTxBytes, tmpNode->perSecRxBytes);
}

static void flowS_seq_stop(struct seq_file *s, void *v)
{
    flowSIndex = 0;
}
#else
#if 1
int onlinecount_read( char *page, char **start, off_t off,
                           int count, int *eof, void *data )
{
    int len = 0;
    len += sprintf(page, "%d:%d:%d\n", uttEtherIfInfo[1].onlineCliCount,
            uttEtherIfInfo[2].onlineCliCount,uttEtherIfInfo[3].onlineCliCount);

    return len;
}
#endif

struct flowS_priv{
    int index;
    struct netFlowStatus *node;
};

//static int flowSIndex = 0;

struct netFlowStatus* findNextFlowSNode(struct seq_file *seq)
{
    struct netFlowStatus *tmpNode;
    struct flowS_priv *state = seq->private;
    int flowSIndex = state->index;
    tmpNode = state->node;
    tmpNode = rcu_dereference(tmpNode->next);
    if(tmpNode != NULL) {
        state->node = tmpNode;
        state->index = flowSIndex;
        return tmpNode;
    } else {
        for(flowSIndex = (flowSIndex + 1); flowSIndex < MAX_HASH_TABLE; flowSIndex++) {
            tmpNode = rcu_dereference(uttPerIpPerSecFlowSta[flowSIndex]);
            if(tmpNode != NULL) {
                state->node = tmpNode;
                state->index = flowSIndex;
                return tmpNode;
            }
        }
    }
    return NULL;
}

static void *flowS_seq_start(struct seq_file *seq, loff_t *pos)
    __acquires(RCU)
{
    struct netFlowStatus *tmpNode;
    struct flowS_priv *state = seq->private;
    int flowSIndex = 0;
    rcu_read_lock();
    flowSIndex = 0;
    if(*pos == 0) {
        for(flowSIndex = 0; flowSIndex < MAX_HASH_TABLE; flowSIndex++) {
            tmpNode = rcu_dereference(uttPerIpPerSecFlowSta[flowSIndex]);
            while(tmpNode != NULL) {
                state->node = tmpNode;
                state->index = flowSIndex;
                return tmpNode;
            }
        }
        return NULL;
    } else {
        return findNextFlowSNode(seq);
    }
}

void *flowS_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
    (*pos)++;
    return findNextFlowSNode(seq);
#if 0
    struct netFlowStatus *tmpNode;
#if 0
    int find = 0;
   if(v == NULL)
        return NULL;
#endif
    tmpNode = rcu_dereference(((struct netFlowStatus *)(v))->next);
    if(tmpNode != NULL) {
        return tmpNode;
    } else {
                printk("w %x %x\n",flowSIndex, MAX_HASH_TABLE);
        for(flowSIndex = (flowSIndex + 1); flowSIndex < MAX_HASH_TABLE; flowSIndex++) {
                printk("o %x %x\n",flowSIndex, MAX_HASH_TABLE);
            tmpNode = rcu_dereference(uttPerIpPerSecFlowSta[flowSIndex]);
            if(tmpNode != NULL) {
                return tmpNode;
            }
        }
    }
#if 0
    if(find == 1)
        return v;
    else 
#endif

        printk("n %x %x\n",flowSIndex, MAX_HASH_TABLE);
        return NULL;
#endif
}

static int flowS_seq_show(struct seq_file *seq, void *v)
{
    struct netFlowStatus *tmpNode;
    int i, tx = 0, rx = 0;
    unsigned int tmpIp = 0;
    tmpNode = (struct netFlowStatus *)(v);
    if(tmpNode == 0) {
        return 0;
    }
    for(i = 1; i < UTT_MAX_IF_NUM; i++) {
        tx += tmpNode->usrIfFlowSta[i].txByte;
        rx += tmpNode->usrIfFlowSta[i].rxByte;
    }
    tmpIp = ntohl(tmpNode->sip);
    seq_printf(seq, "%pI4 %d %d\n",&tmpIp, tx, rx);
//    seq_printf(seq, "%pI4\n",&tmpIp);
    return 0;
}

static void flowS_seq_stop(struct seq_file *s, void *v)
    __releases(RCU)
{
//    flowSIndex = 0;
    rcu_read_unlock();
}
#endif

static const struct seq_operations flowS_seq_ops = {
	.start  = flowS_seq_start,
	.next   = flowS_seq_next,
	.stop   = flowS_seq_stop,
	.show   = flowS_seq_show,
};

static int flowS_seq_open(struct inode *inode, struct file *file)
{
	return seq_open_net(inode, file, &flowS_seq_ops, sizeof(struct flowS_priv));
#if 0
	return seq_open(file, &flowS_seq_ops);
#endif
}

static const struct file_operations flowS_seq_fops = {
	.owner		= THIS_MODULE,
	.open           = flowS_seq_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.release	= seq_release_net,
#if 0
	.release	= seq_release
#endif
};

static int __net_init flowS_net_init(struct net *net)
{
	if (!proc_net_fops_create(net, "flowS", S_IRUGO, &flowS_seq_fops))
		return -ENOMEM;
	return 0;
}

static void __net_exit flowS_net_exit(struct net *net)
{
	proc_net_remove(net, "flowS");
}

static struct pernet_operations flowS_net_ops = {
	.init = flowS_net_init,
	.exit = flowS_net_exit,
};

static int __init prio_module_init(void)
{
    int i = 0;
#if 1
    static struct proc_dir_entry *entry;
    static struct proc_dir_entry *proc_entry;
    proc_entry = create_proc_entry( "onlinecount", 0644, NULL );
    if (proc_entry == NULL) {
        printk("onlinecount: Couldn't create proc entry\n");
    } else {
        proc_entry->read_proc = onlinecount_read;
    }
#endif

    for(i = 1; i < UTT_MAX_IF_NUM; i++) {
        uttEtherIfInfo[i].maxTimeout = HZ * 60;
    }
    register_pernet_subsys(&flowS_net_ops);
#if 0
    entry = create_proc_entry("flowS", 0644, NULL);
    if (entry) {
        entry->proc_fops = &flowS_seq_fops;
    }
#else
#endif

	return register_qdisc(&prio_qdisc_ops);
}

static void __exit prio_module_exit(void)
{
	unregister_qdisc(&prio_qdisc_ops);
}

module_init(prio_module_init)
module_exit(prio_module_exit)

MODULE_LICENSE("GPL");
