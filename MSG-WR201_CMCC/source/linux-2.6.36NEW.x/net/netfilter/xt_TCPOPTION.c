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
MODULE_AUTHOR("wang bingrong <ff0619@gmail.com>");
MODULE_DESCRIPTION("x_tables TCP OPTION modification module");
MODULE_ALIAS("ipt_TCPOPTION");

static inline unsigned int
optlen(const u_int8_t *opt, unsigned int offset)
{
	/* Beware zero-length options: make finite progress */
	if (opt[offset] <= TCPOPT_NOP || opt[offset+1] == 0)
		return 1;
	else
		return opt[offset+1];
}

#if 0
static int
tcpmss_mangle_packet(struct sk_buff **pskb,
		     const struct xt_tcpmss_info *info,
		     unsigned int tcphoff,
		     unsigned int minlen)
{
	struct tcphdr *tcph;
	unsigned int tcplen, i;
	u8 *opt;

	if (!skb_make_writable(pskb, (*pskb)->len))
		return -1;

	tcplen = (*pskb)->len - tcphoff;
	tcph = (struct tcphdr *)((*pskb)->nh.raw + tcphoff);

#if 0
	/* Since it passed flags test in tcp match, we know it is is
	   not a fragment, and has data >= tcp header length.  SYN
	   packets should not contain data: if they did, then we risk
	   running over MTU, sending Frag Needed and breaking things
	   badly. --RR */
	if (tcplen != tcph->doff*4) {
#if 0
		if (net_ratelimit())
			printk(KERN_ERR "xt_TCPMSS: bad length (%u bytes)\n",
			       (*pskb)->len);
#endif
		return -1;
	}
#endif

        if(tcplen < 24)
            return 0;

	opt = (u_int8_t *)tcph;
        i = sizeof(struct tcphdr);
        opt[i++] = 0x02;
        opt[i++] = 0x04;
        opt[i++] = 0x05;
        opt[i++] = 0x64;
	for (; i < tcplen; i ++) {
            opt[i] = 0x0;
	}
        /*
         * here need computer checksum
         */
	return 0;
}
#endif

static unsigned int
xt_tcpoption_target4(struct sk_buff *skb, const struct xt_target_param *par)
{
        int iph_len; 
	struct iphdr *iph = ip_hdr(skb);
	struct tcphdr *tcph;
	unsigned int tcplen, i;
	u8 *opt;
        unsigned int tcphoff = 0;


	if (!skb_make_writable(skb, skb->len))
		return -1;

        tcphoff = iph->ihl * 4;

        iph_len = ip_hdrlen(skb);
	tcplen = skb->len - tcphoff;
	tcph = (struct tcphdr *)(skb_network_header(skb) + tcphoff);

	/* Since it passed flags test in tcp match, we know it is is
	   not a fragment, and has data >= tcp header length.  SYN
	   packets should not contain data: if they did, then we risk
	   running over MTU, sending Frag Needed and breaking things
	   badly. --RR */
	if (tcplen != tcph->doff*4) {
	    return XT_CONTINUE;
	}

        if(tcplen < 24)
	    return XT_CONTINUE;

	opt = (u_int8_t *)tcph;
        i = sizeof(struct tcphdr);
        opt[i++] = 0x02;
        opt[i++] = 0x04;
        opt[i++] = 0x05;
        opt[i++] = 0x64;
	for (; i < tcplen; i ++) {
            opt[i] = 0x0;
	}

       tcph->check = 0;
       skb->csum = csum_partial((unsigned char *)tcph, skb->len - tcphoff,0);
       tcph->check = csum_tcpudp_magic(iph->saddr, iph->daddr, 
               ntohs(iph->tot_len) - iph_len,iph->protocol,
               skb->csum);


	return XT_CONTINUE;
}

#define TH_SYN 0x02

/* Must specify -p tcp --syn */
static inline int find_syn_match(const struct xt_entry_match *m)
{
	const struct xt_tcp *tcpinfo = (const struct xt_tcp *)m->data;

	if (strcmp(m->u.kernel.match->name, "tcp") == 0 &&
	    tcpinfo->flg_cmp & TH_SYN &&
	    !(tcpinfo->invflags & XT_TCP_INV_FLAGS))
		return 1;

	return 0;
}

static int
xt_tcpoption_checkentry4(const struct xt_tgchk_param *par)
{
	const struct xt_tcpmss_info *info = par->targinfo;
	const struct ipt_entry *e = par->entryinfo;

	if (info->mss == XT_TCPMSS_CLAMP_PMTU &&
	    (par->hook_mask & ~((1 << NF_INET_FORWARD) |
			   (1 << NF_INET_LOCAL_OUT) |
			   (1 << NF_INET_POST_ROUTING))) != 0) {
		printk("xt_TCPMSS: path-MTU clamping only supported in "
		       "FORWARD, OUTPUT and POSTROUTING hooks\n");
		return -EINVAL;
	}
	if (IPT_MATCH_ITERATE(e, find_syn_match))
		return 0;
	printk("xt_TCPMSS: Only works on TCP SYN packets\n");
	return -EINVAL;
}

static struct xt_target xt_tcpoption_reg[] = {
	{
		.family		= NFPROTO_IPV4,
		.name		= "TCPOPTION",
		.checkentry	= xt_tcpoption_checkentry4,
		.target		= xt_tcpoption_target4,
		.targetsize	= sizeof(int),
		.proto		= IPPROTO_TCP,
		.me		= THIS_MODULE,
	},
};

static int __init xt_tcpoption_init(void)
{
	return xt_register_targets(xt_tcpoption_reg, ARRAY_SIZE(xt_tcpoption_reg));
}

static void __exit xt_tcpoption_fini(void)
{
	xt_unregister_targets(xt_tcpoption_reg, ARRAY_SIZE(xt_tcpoption_reg));
}

module_init(xt_tcpoption_init);
module_exit(xt_tcpoption_fini);
