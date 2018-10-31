/*
 * This is a module which is used for UTT LOG.
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
#include <linux/netdevice.h>
#include <linux/if.h>
#include <linux/inetdevice.h>

#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv6/ip6_tables.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>

#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_tcpudp.h>
#include <linux/netfilter/xt_UTTINDENTIFY.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wang bingrong<ff0619@gmail.com>");
MODULE_DESCRIPTION("x_tables UTT INDENTIFY modification module");
MODULE_ALIAS("ipt_UTTINDENTIFY");

static unsigned int
xt_uttindentify_target4(struct sk_buff *skb,
		  const struct xt_target_param *par)
{
    struct sk_buff *skbin = (struct sk_buff *)skb;
    struct xt_uttindentify_info *info = (struct xt_uttindentify_info *)par->targinfo;
    struct nf_conn *conntrack;
    enum ip_conntrack_info ctinfo;

    if (!(conntrack = nf_ct_get(skbin, &ctinfo))) {
	return XT_CONTINUE;
    }

    if (conntrack->utt_app.priFlag && conntrack->utt_app.secFlag) {
#if 0
	printk("indentified conntrack, priFlag = %d, secFlag = %d\n",
		conntrack->utt_app.priFlag, conntrack->utt_app.secFlag);
#endif
	return XT_CONTINUE;
    }
    conntrack->utt_app.priFlag = info->priFlag;
    conntrack->utt_app.secFlag = info->secFlag;

    /* 当前包设置 */
    skb->utt_app.priFlag = info->priFlag;
    skb->utt_app.secFlag = info->secFlag;

    return XT_CONTINUE;
}

static int
xt_uttindentify_checkentry4(const struct xt_tgchk_param *par)
{
    return 0;
}


static struct xt_target xt_uttindentify_reg[] = {
	{
		.family		= AF_INET,
		.name		= "UTTINDENTIFY",
		.checkentry	= xt_uttindentify_checkentry4,
		.target		= xt_uttindentify_target4,
		.targetsize	= sizeof(struct xt_uttindentify_info),
		.me		= THIS_MODULE,
	},
};

static int __init xt_uttindentify_init(void)
{
	return xt_register_targets(xt_uttindentify_reg, ARRAY_SIZE(xt_uttindentify_reg));
}

static void __exit xt_uttindentify_fini(void)
{
	xt_unregister_targets(xt_uttindentify_reg, ARRAY_SIZE(xt_uttindentify_reg));
}

module_init(xt_uttindentify_init);
module_exit(xt_uttindentify_fini);
