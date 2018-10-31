/* TTL modification target for IP tables
 * (C) 2000,2005 by Harald Welte <laforge@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <net/checksum.h>

#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_ipv4/ipt_IPCRACK.h>

MODULE_AUTHOR("wang bingrong <ff0619@gmail.com>");
MODULE_DESCRIPTION("IP head crack module");
MODULE_LICENSE("GPL");
#define MAX_UTT_WAN_IF 2

static unsigned int
ipt_ipcrack_target(struct sk_buff *skb, const struct xt_target_param *par)
{
	struct iphdr *iph;
	struct ipt_IPCRACK_info *info = (struct ipt_IPCRACK_info *)(par->targinfo);
        static unsigned short id[MAX_UTT_WAN_IF] = {0};
        unsigned int ifindex = 0U; 

	if (!skb_make_writable(skb, skb->len))
		return NF_DROP;

	iph = ip_hdr(skb);

	if (info->ttl != iph->ttl) {
		csum_replace2(&iph->check, htons(iph->ttl << 8),
					      htons(info->ttl << 8));
		iph->ttl = info->ttl;
	}
        
        ifindex = skb->dev->utt_ifindex;
        id[ifindex]++;
	csum_replace2(&iph->check, iph->id, htons(id[ifindex]));
	iph->id = htons(id[ifindex]);

	return XT_CONTINUE;
}

static int ipt_ipcrack_checkentry(const struct xt_tgchk_param *par)
{
	struct ipt_IPCRACK_info *info = par->targinfo;

	if (info->ttl == 0) {
		return -EINVAL;
        } else {
	    return 0;
        }
}

static struct xt_target ipt_IPCRACK = {
	.name 		= "IPCRACK",
	.family		= NFPROTO_IPV4,
	.target 	= ipt_ipcrack_target,
	.targetsize	= sizeof(struct ipt_IPCRACK_info),
	.table		= "mangle",
	.checkentry 	= ipt_ipcrack_checkentry,
	.me 		= THIS_MODULE,
};

static int __init ipt_ipcrack_init(void)
{
	return xt_register_target(&ipt_IPCRACK);
}

static void __exit ipt_ipcrack_fini(void)
{
	xt_unregister_target(&ipt_IPCRACK);
}

module_init(ipt_ipcrack_init);
module_exit(ipt_ipcrack_fini);
