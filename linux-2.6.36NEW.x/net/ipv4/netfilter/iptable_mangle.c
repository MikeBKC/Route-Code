/*
 * This is the 1999 rewrite of IP Firewalling, aiming for kernel 2.3.x.
 *
 * Copyright (C) 1999 Paul `Rusty' Russell & Michael J. Neuling
 * Copyright (C) 2000-2004 Netfilter Core Team <coreteam@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/module.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/slab.h>
#include <net/sock.h>
#include <net/route.h>
#include <linux/ip.h>
#include <net/ip.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_CONNMARK.h>
#include <net/netfilter/nf_conntrack_ecache.h>
#include <linux/netfilter_ipv4/utt_filter_kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Netfilter Core Team <coreteam@netfilter.org>");
MODULE_DESCRIPTION("iptables mangle table");

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICYROUTE
extern int uttNfPolicyRouteListMatch( struct sk_buff *skb );
#endif
#define MANGLE_VALID_HOOKS ((1 << NF_INET_PRE_ROUTING) | \
			    (1 << NF_INET_LOCAL_IN) | \
			    (1 << NF_INET_FORWARD) | \
			    (1 << NF_INET_LOCAL_OUT) | \
			    (1 << NF_INET_POST_ROUTING))

static const struct xt_table packet_mangler = {
	.name		= "mangle",
	.valid_hooks	= MANGLE_VALID_HOOKS,
	.me		= THIS_MODULE,
	.af		= NFPROTO_IPV4,
	.priority	= NF_IP_PRI_MANGLE,
};

/**
 *  copy connect mark to skb
 *  2012-12-03
 *  bhou
 */
static void ct_to_sk_mark(struct sk_buff *skb)
{
    struct nf_conn *ct;
    enum ip_conntrack_info ctinfo;

    ct = nf_ct_get(skb, &ctinfo);
    if(ct != NULL)
    {
	skb->mark = ct->mark;
    }
    return;
}

static unsigned int
ipt_mangle_out(struct sk_buff *skb, const struct net_device *out)
{
	unsigned int ret;
	const struct iphdr *iph;
	u_int8_t tos;
	__be32 saddr, daddr;
	u_int32_t mark;

	/* root is playing with raw sockets. */
	if (skb->len < sizeof(struct iphdr) ||
	    ip_hdrlen(skb) < sizeof(struct iphdr))
		return NF_ACCEPT;

	/* Save things which could affect route */
	mark = skb->mark;
	iph = ip_hdr(skb);
	saddr = iph->saddr;
	daddr = iph->daddr;
	tos = iph->tos;

	/* replace iptables rule :
	   doSystem("iptables -t mangle -A OUTPUT -j CONNMARK --restore-mark");

           and we must perform function at this point
           because all iptables'rule in OUTPUT chain have performed after route 
           but ip_route_me_harder funtion will performing repeat route when skb's mark change 

	 * restore ct mark to skb for hotel pnp and qos tc
	 * edit by bhou*/
	ct_to_sk_mark(skb);

	ret = ipt_do_table(skb, NF_INET_LOCAL_OUT, NULL, out,
			   dev_net(out)->ipv4.iptable_mangle);
	/* Reroute for ANY change. */
	if (ret != NF_DROP && ret != NF_STOLEN && ret != NF_QUEUE) {
		iph = ip_hdr(skb);

		if (iph->saddr != saddr ||
		    iph->daddr != daddr ||
		    skb->mark != mark ||
		    iph->tos != tos)
			if (ip_route_me_harder(skb, RTN_UNSPEC))
				ret = NF_DROP;
	}

	return ret;
}

/* The work comes in here from netfilter.c. */
static unsigned int
iptable_mangle_hook(unsigned int hook,
		     struct sk_buff *skb,
		     const struct net_device *in,
		     const struct net_device *out,
		     int (*okfn)(struct sk_buff *))
{
	unsigned int ret = 0;
	struct nf_conn *ct;
	enum ip_conntrack_info ctinfo;
	if (hook == NF_INET_PRE_ROUTING) {
    /* replace iptables rule :
       doSystem("iptables -t mangle -I PREROUTING -j CONNMARK --restore-mark");

     * restore ct mark to skb for hotel pnp and qos tc
     * edit by bhou*/
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICYROUTE
		uttNfPolicyRouteListMatch(skb);
		ct_to_sk_mark(skb);
#else
		ct_to_sk_mark(skb);
#endif
	}
	if (hook == NF_INET_LOCAL_OUT)
		return ipt_mangle_out(skb, out);
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICYROUTE
	if (hook == NF_INET_LOCAL_IN) {
		/* 在INPUT中清除策略路由的mark标记，
		 * 防止从Lan口进来走向Lan口的包从WAN口出去
		 * 主要有以下几种情况：
		 * 1、Lan口网段的PC访问路由器时
		 * 2、Lan口PPPoE拨号用户访问路由器时*/
		ct = nf_ct_get(skb, &ctinfo);
		if((ct != NULL))
		{
			if((ctinfo == IP_CT_NEW) && 
					isSkbComeFromLan(skb)) {
				(ct->mark) &= (~POLICYROUTE_CONNMARK_MSK);
			}
		}
	}
#endif
	if (hook == NF_INET_POST_ROUTING) {
		ret = ipt_do_table(skb, hook, in, out,
				    dev_net(out)->ipv4.iptable_mangle);
		skb->mark &= QOS_CONNMARK_MSK;
		ct = nf_ct_get(skb, &ctinfo);
		if((ct != NULL) && !(ct->mark & POLICYROUTE_CONNMARK_MSK) && skb->dev != NULL && skb->dev->utt_ifindex != 0) {
		    ct->mark |= (skb->dev->utt_ifindex)<<4;
		} 

		return ret;
	}
	/* INPUT/FORWARD: */
	ret = ipt_do_table(skb, hook, in, out,
			    dev_net(in)->ipv4.iptable_mangle);
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	if (ret != NF_DROP) {
	    if (hook == NF_INET_LOCAL_IN) {
		ret = uttNfHookSlow(UTT_PF_INET,UTT_NF_IP_LOCAL_IN,UTT_NF_IPT_MANGLE,&skb);
	    } else if (hook == NF_INET_FORWARD) {
		ret = uttNfHookSlow(UTT_PF_INET,UTT_NF_IP_FORWARD,UTT_NF_IPT_MANGLE,&skb);
	    } 
	} 
#endif
	if(isSkbComeFromLan(skb)) {
#if 0
	    (skb->mark) &= (~HOTEL_CONNNARK_MSK);
#endif
	} else {
	    (skb->mark) &= (~POLICYROUTE_CONNMARK_MSK);
	}
	return ret;
}

static struct nf_hook_ops *mangle_ops __read_mostly;

static int __net_init iptable_mangle_net_init(struct net *net)
{
	struct ipt_replace *repl;

	repl = ipt_alloc_initial_table(&packet_mangler);
	if (repl == NULL)
		return -ENOMEM;
	net->ipv4.iptable_mangle =
		ipt_register_table(net, &packet_mangler, repl);
	kfree(repl);
	if (IS_ERR(net->ipv4.iptable_mangle))
		return PTR_ERR(net->ipv4.iptable_mangle);
	return 0;
}

static void __net_exit iptable_mangle_net_exit(struct net *net)
{
	ipt_unregister_table(net, net->ipv4.iptable_mangle);
}

static struct pernet_operations iptable_mangle_net_ops = {
	.init = iptable_mangle_net_init,
	.exit = iptable_mangle_net_exit,
};

static int __init iptable_mangle_init(void)
{
	int ret;

	ret = register_pernet_subsys(&iptable_mangle_net_ops);
	if (ret < 0)
		return ret;

	/* Register hooks */
	mangle_ops = xt_hook_link(&packet_mangler, iptable_mangle_hook);
	if (IS_ERR(mangle_ops)) {
		ret = PTR_ERR(mangle_ops);
		goto cleanup_table;
	}

	return ret;

 cleanup_table:
	unregister_pernet_subsys(&iptable_mangle_net_ops);
	return ret;
}

static void __exit iptable_mangle_fini(void)
{
	xt_hook_unlink(&packet_mangler, mangle_ops);
	unregister_pernet_subsys(&iptable_mangle_net_ops);
}

module_init(iptable_mangle_init);
module_exit(iptable_mangle_fini);
