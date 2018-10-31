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
#include <linux/netfilter/xt_TCPMSS.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wang bingrong<ff0619@gmail.com>");
MODULE_DESCRIPTION("x_tables TCP WINDOWS modification module");
MODULE_ALIAS("ipt_HTTPCRACK");

static unsigned int
xt_tcpwindows_target4(struct sk_buff *skb,
		  const struct xt_target_param *par)
{
	struct tcphdr *tcph;
        int iph_len; 


	if (!skb_make_writable(skb, skb->len))
		return XT_CONTINUE;

        iph_len = ip_hdrlen(skb);

	tcph = (struct tcphdr *)(skb_network_header(skb) + iph_len);

	inet_proto_csum_replace2(&tcph->check, skb,
				 (tcph->window), (5000), 0);
        tcph->window = 5000;
	return XT_CONTINUE;
}

static int
xt_tcpwindows_checkentry4(const struct xt_tgchk_param *par)
{
        return 0;
}


static struct xt_target xt_tcpwindows_reg[] = {
	{
		.family		= NFPROTO_IPV4,
		.name		= "TCPWINDOWS",
		.checkentry	= xt_tcpwindows_checkentry4,
		.target		= xt_tcpwindows_target4,
		.targetsize	= sizeof(int),
		.proto		= IPPROTO_TCP,
		.me		= THIS_MODULE,
	},
};

static int __init xt_tcpwindows_init(void)
{
	return xt_register_targets(xt_tcpwindows_reg, ARRAY_SIZE(xt_tcpwindows_reg));
}

static void __exit xt_tcpwindows_fini(void)
{
	xt_unregister_targets(xt_tcpwindows_reg, ARRAY_SIZE(xt_tcpwindows_reg));
}

module_init(xt_tcpwindows_init);
module_exit(xt_tcpwindows_fini);
