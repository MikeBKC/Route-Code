/* 
 * match the dev that the skb's iif redirected
 * add by bhou 2012-11-6
 * */

#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/module.h>
#include <net/dst.h>
#include <net/tcp.h>
#include <net/netfilter/nf_conntrack.h>
#include <linux/netfilter/xt_httpQQ.h>
#include <linux/netfilter/x_tables.h>

MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_httpQQ");
MODULE_ALIAS("ip6t_httpQQ");

#define DEBUG 0

static int proto_hdr_http[] = {
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


static bool match(const struct sk_buff *skb, const struct xt_match_param *par)
{
    bool rst = false;
    const struct xt_httpQQ_info *info = par->matchinfo;
#if 1
    const struct iphdr *iph = NULL;
    struct tcphdr *tcph = NULL;
    unsigned int tcph_len = 0;
    char * tcpData = NULL;
    struct nf_conn *ct;
    enum ip_conntrack_info ctinfo;
    unsigned int iph_len = 0; 
    
    if(info->invert && XT_HTTPQQ_MATCH_CHILD) {
	rst = true;
    }
    if (info->flag & XT_HTTPQQ_MATCH_CHILD) {
#if DEBUG
	printk("%s-%d: http QQ match begin.\n",__func__,__LINE__);
#endif
	ct = nf_ct_get(skb, &ctinfo);
	if(ct != NULL)
	{
	    if((ct->httpQQ == UTT_HTTP_QQ_MATCH_FLAG)) {
		rst = true;
		if(info->invert && XT_HTTPQQ_MATCH_CHILD) {
		    rst = !rst;
		}
	    } else if(ct->httpQQ == UTT_HTTP_QQ_NOT_MATCH_FLAG) {
		rst = false;
		if(info->invert && XT_HTTPQQ_MATCH_CHILD) {
		    rst = !rst;
		}
	    } else {
		    switch (skb->protocol) {
			case htons(ETH_P_IP):
			{
			    iph = ip_hdr(skb);
			    if(iph->protocol == IPPROTO_TCP) {
#if 0
				tcph = (struct tcphdr *)((char*)iph + iph->ihl * 4);
#endif
				iph_len = ip_hdrlen(skb);
				tcph = (struct tcphdr *)(skb_network_header(skb) + iph_len); 
				tcph_len = (tcph->doff) * 4;;
#if DEBUG
				printk("%s-%d: tcp source port %d, dest port %d.\n",__func__,__LINE__,ntohs(tcph->source),ntohs(tcph->dest));
#endif
#if 0
				if(ntohs(tcph->dest) == (80)) 
#endif
				{
				    tcpData = (char*)((char*)tcph + tcph_len);
				    if(tcpData + 4 < skb->tail) {
#if DEBUG
					printk("%s-%d: tcp dport 80, ct->httpQQ =%d\n",__func__,__LINE__,ct->httpQQ);
#endif
#if 1
					ct->httpQQ  = UTT_HTTP_QQ_NOT_MATCH_FLAG;
					if(*((int*)tcpData) != proto_hdr_http[0] && 
						*((int*)tcpData) != proto_hdr_http[1]) {
					    /* conn is recognized as a http QQ login conn*/
					    ct->httpQQ  = UTT_HTTP_QQ_MATCH_FLAG;
					    rst = true;
					    if(info->invert && XT_HTTPQQ_MATCH_CHILD) {
						rst = !rst;
#if DEBUG
						printk("%s-%d: info->invert =%d\n",__func__,__LINE__,info->invert);
#endif
					    }
#if DEBUG
					    printk("%s-%d: conn is matched by http QQ login.\n",__func__,__LINE__);
#endif

					}
#endif
				    }
				}
			    }
			}
			    break;
			default:
			    break;
		    }
	    }
	}
    }
#endif
#if DEBUG
    printk("%s-%d: rst = %d\n", __func__,__LINE__,rst);
#endif
    return rst;
}

static int match_check(const struct xt_mtchk_param *par)
{
#if 0
    const struct xt_httpQQ_info *info = par->matchinfo;

#endif
	return 0;
}

static struct xt_match httpQQ_mt_reg __read_mostly = {
	.name       = "httpQQ",
	.revision   = 0,
	.family     = NFPROTO_UNSPEC,
	.checkentry = match_check,
	.match      = match,
	.matchsize  = sizeof(struct xt_httpQQ_info),
	.me         = THIS_MODULE,
};

static int __init httpQQ_mt_init(void)
{
	return xt_register_match(&httpQQ_mt_reg);
}

static void __exit httpQQ_mt_exit(void)
{
	xt_unregister_match(&httpQQ_mt_reg);
}

module_init(httpQQ_mt_init);
module_exit(httpQQ_mt_exit);
