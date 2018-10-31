/*
 * This is a module which is used for setting the MSS option in TCP packets.
 *
 * Copyright (C) 2000 Marc Boucher <marc@mbsi.ca>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/tcp.h>
#include <net/ipv6.h>
#include <net/tcp.h>

#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv6/ip6_tables.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_tcpudp.h>
#include <net/netfilter/nf_conntrack.h>

#ifdef CONFIG_UTT_NETFILTER
#include <linux/netfilter_ipv4/utt_netfilter.h>
#endif
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wang bingrong<ff0619@gmail.com>");
MODULE_DESCRIPTION("x_tables UTT POLICY modification module");
MODULE_ALIAS("ipt_UTTPOLICY");

#if CONFIG_NETFILTER_XT_TARGET_UTTLOG
#include <linux/netfilter_ipv4/utt_netfilter.h>
#include <linux/netfilter/xt_UTTLOG.h>
extern void recordErrLog(char *log, __u32 sec); 
#define MAX_EIM_LOG_MSG_LENGTH 120
#define EIM_DBG 0
#if EIM_DBG
#define UTT_EIM_DEBUG(fmt, args...)		printk(fmt, ## args)
#else
#define UTT_EIM_DEBUG(fmt, args...)
#endif
int eimFlag = 0;
#if CONFIG_XPORT_LOG
unsigned int xport_nat=0U;
#endif
/**
 * netlink通信
 */
void uttNlEim(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    UttNlNfEimConf *nlData;
    nlh = nlmsg_hdr(skb);
    nlData = (UttNlNfEimConf*)NLMSG_DATA(nlh);/*取值*/
    eimFlag = nlData->entryVal.button;
#if CONFIG_XPORT_LOG
    xport_nat = nlData->entryVal.natLogEnable;
#endif
    UTT_EIM_DEBUG("%s %d eim %d\n", __func__, __LINE__, nlData->entryVal.button);
    return;
}
#endif

extern bool uttNfPolicyHashMatch(struct sk_buff* skb);
static unsigned int
xt_uttpolicy_target4(struct sk_buff *skb,
	const struct xt_target_param *par)
{
    struct nf_conn *conntrack;
#if CONFIG_NETFILTER_XT_TARGET_UTTLOG
    char msg[MAX_EIM_LOG_MSG_LENGTH + 1];
    struct timex  txc;
    IP_HDR *iph = ip_hdr(skb);
    __u32    myCurrentTime;
#endif
    /* 数据包必须有对应的连接*/
    conntrack = (struct nf_conn *)skb->nfct;
    if ((conntrack != NULL) && ((conntrack->utt_app.uttIdFlag == QQ_FLAG) || (conntrack->utt_app.uttIdFlag == MSN_FLAG) || (conntrack->utt_app.uttIdFlag == ALI_FLAG))) 
    {/*匹配QQ白名单或者MSN白名单或阿里旺旺白名单则通过*/
	return XT_CONTINUE;
    }
    else if(skb->utt_app.uttIdFlag == QQ_FLAG)
    {/*十一位QQ的QQ号码位置为0的包通过*/
	return XT_CONTINUE;
    }
#if 1
    else if(skb->utt_app.uttIdFlag == ALI_FLAG)
    {/*阿里旺旺前两个包通过*/
        return XT_CONTINUE;
    }
#endif
    else if(uttNfPolicyHashMatch(skb))
    {/*匹配禁止规则。则丢包*/
#if CONFIG_NETFILTER_XT_TARGET_UTTLOG
	if (eimFlag == 1) {
	    do_gettimeofday(&(txc.time));
	    myCurrentTime = txc.time.tv_sec;
	    snprintf(msg, MAX_EIM_LOG_MSG_LENGTH, "%d is forbidden, srcip = %u.%u.%u.%u, dstip = %u.%u.%u.%u\n", 
		    skb->utt_app.secFlag, NIPQUAD(iph->saddr), NIPQUAD(iph->daddr));
	    recordErrLog(msg, myCurrentTime);
	}
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTDNSREDIRECT
	if (skb->utt_app.priFlag == 4 && skb->utt_app.secFlag == 143) {
	    send_dns_redirect(skb,(__u32)2130706433);
	}
#endif
	return NF_DROP;
    }
    return XT_CONTINUE;
}

static int
xt_uttpolicy_checkentry4(const struct xt_tgchk_param *par)
{
        return 0;
}


static struct xt_target xt_uttpolicy_reg[] = {
	{
		.family		= NFPROTO_IPV4,
		.name		= "UTTPOLICY",
		.checkentry	= xt_uttpolicy_checkentry4,
		.target		= xt_uttpolicy_target4,
		.targetsize	= sizeof(int),
		.me		= THIS_MODULE,
	},
};

static int __init xt_uttpolicy_init(void)
{
	return xt_register_targets(xt_uttpolicy_reg, ARRAY_SIZE(xt_uttpolicy_reg));
}

static void __exit xt_uttpolicy_fini(void)
{
	xt_unregister_targets(xt_uttpolicy_reg, ARRAY_SIZE(xt_uttpolicy_reg));
}

module_init(xt_uttpolicy_init);
module_exit(xt_uttpolicy_fini);
