/* 
 * match the dev that the skb's iif redirected
 * add by bhou 2012-11-6
 * */

#include <linux/ip.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <net/dst.h>
#include <linux/netfilter/xt_uttsniper.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_ipv4/utt_filter_kernel.h>

#define NTSR_IP_TTL_FLAG    (1U)

/* 网络尖兵ip id识别标识 */
#define NTSR_IP_ID_FLAG     (1U << 1U)

/* 主机被识别为网络尖兵标识 */
#define NTSR_FLAG       (NTSR_IP_TTL_FLAG | NTSR_IP_ID_FLAG)

MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_uttsniper");
MODULE_ALIAS("ip6t_uttsniper");

#define DEBUG_UTT_NETSNIPER 1
#if DEBUG_UTT_NETSNIPER
#define UTT_NETSNIPER_DEBUG(fmt,argx...) printk_sniper(fmt,##argx)
#else
#define UTT_NETSNIPER_DEBUG(fmt,argx...)
#endif

asmlinkage int printk_sniper(const char *fmt,...)
{
	va_list args;
	int r;

	va_start(args,fmt);
	r = vprintk(fmt,args);
	va_end(args);

	return r;
}

static bool uttsniper_mt(const struct sk_buff *skb, const struct xt_match_param *par)
{
	bool rst = false;
	const struct xt_uttsniper_info *info = par->matchinfo;
	UttNfHashConf* hashConf = NULL;
	struct iphdr *ipH = NULL;
	UttNetSniper *netSniper = NULL;

	if(info->bitmask & XT_UTTSNIPER_OP_NEEDNOTICE)
	{
		ipH = ip_hdr(skb);
		hashConf = uttNfHashConfGet(ipH->saddr,0);
		if(hashConf!=NULL){
			netSniper = &(hashConf->netSniper);
			if(netSniper->ntsr_flag == NTSR_FLAG){
				UTT_NETSNIPER_DEBUG("netsniper need notice\n");
				rst = true;
			}
		}

	}
    return rst;
}

static bool uttsniper_mt_check(const struct xt_mtchk_param *par)
{
	return true;
}

static struct xt_match uttsniper_mt_reg __read_mostly = {
	.name       = "uttsniper",
	.revision   = 0,
	.family     = NFPROTO_UNSPEC,
	.checkentry = uttsniper_mt_check,
	.match      = uttsniper_mt,
	.matchsize  = sizeof(struct xt_uttsniper_info),
	.me         = THIS_MODULE,
};

static int __init uttsniper_mt_init(void)
{
	return xt_register_match(&uttsniper_mt_reg);
}

static void __exit uttsniper_mt_exit(void)
{
	xt_unregister_match(&uttsniper_mt_reg);
}

module_init(uttsniper_mt_init);
module_exit(uttsniper_mt_exit);
