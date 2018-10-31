/* 
 * match the dev that the skb's iif redirected
 * add by bhou 2012-11-6
 * */

#include <linux/ip.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <net/dst.h>
#include <net/route.h>
#include <linux/netfilter/xt_uttdev.h>
#include <linux/netfilter/x_tables.h>

MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_uttdev");
MODULE_ALIAS("ip6t_uttdev");
static int isSkbComeFromSaleWifi(struct sk_buff *skb)
{
    struct net_device *device = NULL;
    int ret = 0;

    device = dev_get_by_index(sock_net(skb->sk),skb->skb_iif);
    if (device != NULL)
    {
        printk("device name =%s\n", device->name);
        if ((strcmp(device->name, "ra2") == 0) || (strcmp(device->name, "ra3") == 0) || (strcmp(device->name, "rai2") == 0) || (strcmp(device->name, "rai3") == 0))
        {
            ret = 1;
        }else{
            ret = 0;
        }
        dev_put(device);/*释放设备引用*/
    }
    else
        printk("device is null.\n");

    return ret;
}

static int isSkbComeFromDevIn(struct sk_buff *skb, const struct xt_uttdev_info *info)
{
    struct net_device *device = NULL;
    int ret = 0;
    
    device = dev_get_by_index(sock_net(skb->sk),skb->skb_iif);
    if (device != NULL)
    {
        printk("device name =%s\n", device->name);
	ret = !ifname_compare_aligned(device->name, info->uttindev, info->in_mask);
	ret = ret ^ !!(info->invert & XT_UTTDEV_OP_IN);
        dev_put(device);/*释放设备引用*/
    }
    else
        printk("device is null.\n");

    return ret;
}

//extern bool isSkbComeFromLan(struct sk_buff *skb);
static bool uttdev_mt(const struct sk_buff *skb, const struct xt_match_param *par)
{
	struct rtable *rt;	/* Route we use */
#if 1
    bool rst = false;
    const struct xt_uttdev_info *info = par->matchinfo;
#endif

	rt = skb_rtable(skb);
#if 1
    if (info->bitmask & XT_UTTDEV_OP_ISLANIN) {
#endif
	bool lanIn = isSkbComeFromLan(skb);
#if 0
    const struct xt_uttdev_info *info = par->matchinfo;
#endif
#if 0
	return ((info->bitmask & XT_UTTDEV_OP_ISLANIN) &&
	    (lanIn ^ !!(info->invert & XT_UTTDEV_OP_ISLANIN)) );
#else
	rst = lanIn ^ !!(info->invert & XT_UTTDEV_OP_ISLANIN);
#endif
#if 1
    } else if (info->bitmask & XT_UTTDEV_OP_LANTOWAN) {
	/* from lan to wan */
	if ((skb->utt_iif == 0) && (&rt->dst != NULL) && (rt->dst.dev != NULL) &&  
	    (rt->dst.dev->utt_ifindex > 0) 
	    && (rt->dst.dev->utt_ifindex < UTT_MAX_IF_NUM)) {
	    rst = true;
	} else {
	    rst = false;
	}
    } else if (info->bitmask & XT_UTTDEV_OP_WANTOLAN) {
	/* from wan to lan */
	if ((&rt->dst != NULL) && (rt->dst.dev != NULL) && 
	    (rt->dst.dev->utt_ifindex == 0) && (skb->utt_iif > 0) 
	    && (skb->utt_iif < UTT_MAX_IF_NUM)) {
	    rst = true;
	} else {
	    rst = false;
	}
    }else if(info->bitmask & XT_UTTDEV_OP_WIFISALEIN) {
	if (isSkbComeFromSaleWifi(skb) == 1){ 
	    rst = true;
	} else{
            rst = false; 
	}   
    }else if(info->bitmask & XT_UTTDEV_OP_IN) {
	if (isSkbComeFromDevIn(skb, info) == 1){ 
	    rst = true;
	} else{
            rst = false; 
	}   
    }

    return rst;
#endif
}

static int uttdev_mt_check(const struct xt_mtchk_param *par)
{
#if 0
	const struct xt_uttdev_info *info = par->matchinfo;
	if (info->bitmask & XT_UTTDEV_OP_OUT &&
	    (!(info->bitmask & XT_UTTDEV_OP_BRIDGED) ||
	     info->invert & XT_UTTDEV_OP_BRIDGED) &&
	    par->hook_mask & ((1 << NF_INET_LOCAL_OUT) |
	    (1 << NF_INET_FORWARD) | (1 << NF_INET_POST_ROUTING))) {
		printk(KERN_WARNING "uttdev match: using --uttdev-out in the "
		       "OUTPUT, FORWARD and POSTROUTING chains for non-bridged "
		       "traffic is not supported anymore.\n");
		if (par->hook_mask & (1 << NF_INET_LOCAL_OUT))
			return false;
	}
#endif

#if 1
    const struct xt_uttdev_info *info = par->matchinfo;

    /* 
     * lan-to-wan and wan-to-lan should be in forward
     * i do not check if it is correct in other chain
     */
    if (((info->bitmask & XT_UTTDEV_OP_LANTOWAN) || (info->bitmask & XT_UTTDEV_OP_WANTOLAN))
	&& ((par->hook_mask & ~(1 << NF_INET_FORWARD)) != 0)) {
	printk(KERN_WARNING "uttdev match: only permit use lan-to-wan or wan-to-lan in forward chain ");

	return -EINVAL;
    }
#endif
	return 0;
}

static struct xt_match uttdev_mt_reg __read_mostly = {
	.name       = "uttdev",
	.revision   = 0,
	.family     = NFPROTO_UNSPEC,
	.checkentry = uttdev_mt_check,
	.match      = uttdev_mt,
	.matchsize  = sizeof(struct xt_uttdev_info),
	.me         = THIS_MODULE,
};

static int __init uttdev_mt_init(void)
{
	return xt_register_match(&uttdev_mt_reg);
}

static void __exit uttdev_mt_exit(void)
{
	xt_unregister_match(&uttdev_mt_reg);
}

module_init(uttdev_mt_init);
module_exit(uttdev_mt_exit);
