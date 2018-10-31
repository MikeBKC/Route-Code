/* 
 * match the dev that the skb's iif redirected
 * add by bhou 2012-11-6
 * */

#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <net/dst.h>
#include <net/route.h>
#include <linux/netfilter/xt_wifiMatch.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_ipv4/utt_filter_kernel.h>

MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_wifiMatch");
MODULE_ALIAS("ip6t_wifiMatch");

extern  int wifi_match_debug_print(const char* fmt, ...);
extern DnsEntryVal *wifidog_dnsHashLookupData(char *domainName);
#if 1
bool ipset_ip_add_del(struct sk_buff *skb, const char *setname, DnsEntryVal* dnsEntry,__u32 netip, char operate);
#else
bool ipEntryIpAddOrDel(DnsEntryVal* dnsEntry, __u32 netip, char *oprate);
#endif
extern int getDomainName(struct dnshdr *dns_hdr, int dns_len, unsigned char *buffer);
//extern bool isSkbComeFromLan(struct sk_buff *skb);
int addDnsIpFilter(const struct sk_buff *skb,const struct dnshdr *dnsh, int dnslen, DnsEntryVal *dnsEntry, char *domainName)
{
    int ret = 1;
    int domainNameLen = 0;
    __u8 *dnsAnswer = NULL;
    int dnsAnswer_len = 0;
    int question_num = 0;
    int answer_num = 0;
    __u16 type = 0U;
    __u16 cname_len = 0U;
    __u32 *address = NULL;
#if 0
    uint32_t flag = IPSET_SRC;
    ip_set_id_t ipsetindex = IP_SET_INVALID_ID;
    struct sk_buff *nskb=NULL;
    struct iphdr *iph=NULL;
    nskb = skb_copy(skb,GFP_ATOMIC);
    if(nskb) {
	iph = ip_hdr(nskb);
#endif

	question_num = ntohs(dnsh->qdcount);
	answer_num = ntohs(dnsh->ancount);
	domainNameLen = strlen(domainName);
	dnsAnswer_len = dnslen - 12 - (domainNameLen + 2 + 2 + 2); /*dnslen - dnsheadlen - dnsquerylen*/
	wifi_match_debug_print("%s-%d: Domain Name: %s, len %u, answer_num %d, dnsAnswer_len %d, dnslen %d, find in wifidog dns hash table.\n", __func__,__LINE__,domainName,domainNameLen,answer_num,dnsAnswer_len,dnslen);

	dnsAnswer = (((__u8 *)dnsh) + 12 +(domainNameLen + 2 + 2 + 2));
	    int i=0;
	    __u8 *tmp=dnsAnswer;
	    for(i=0;i<dnsAnswer_len;i++) {
	    wifi_match_debug_print("%02x ", *tmp);
	    tmp++;
	    }
	while((dnsAnswer !=NULL)&&(dnsAnswer_len>0)&&(answer_num>0)) {
	    if(*dnsAnswer == 0xc0) {
		type = ntohs(*((__u16*)(dnsAnswer + 2)));
		if(type == 0x05) {
		    cname_len = ntohs(*((__u16 *)(dnsAnswer + 2 + 2 + 2 + 4)));
		    dnsAnswer += (2 + 2 + 2 + 4 + 2 + cname_len);
		    dnsAnswer_len -=(2 + 2 + 2 + 4 + 2 + cname_len);
wifi_match_debug_print("\n%s-%d: type %x, cname_len %d, next dnsAnswer %02x%02x%02x%02x,dnsAnswer_len %d\n", __func__,__LINE__,type,cname_len,*(dnsAnswer),*(dnsAnswer+1),*(dnsAnswer+2),*(dnsAnswer+3),dnsAnswer_len);
		} else if(type == 0x01) {
		    address = (__u32 *)(dnsAnswer + (2 + 2 + 2 + 4 + 2));
		    wifi_match_debug_print("\n%s-%d: type %x,address %x\n", __func__,__LINE__,type,*address);
#if 1
		    ipset_ip_add_del(skb, WHITE_LIST, dnsEntry, *address, 'A');
		    answer_num--;
		    break;
#else
		    if((iph!=NULL) && (*address > 0)) {
			iph->saddr = *address;
			ipsetindex = ip_set_get_byname(WHITE_LIST);
			if(ipsetindex != IP_SET_INVALID_ID) {
			    if(ip_set_testip_kernel(ipsetindex, skb, &flag)==0){ /*检测IP*/
				ip_set_addip_kernel(ipsetindex, skb, &flag);
				ipEntryIpAddOrDel(dnsEntry, *address, 'A');
				wifi_match_debug_print("%s-%d: add address %x, in set name %s\n", __func__,__LINE__,ntohl(iph->saddr),WHITE_LIST);
				answer_num--;
				break;
			    }
			    ip_set_put_byindex(ipsetindex);
			}
		    }
#endif
		    dnsAnswer += 16;
		    dnsAnswer_len -= 16;
wifi_match_debug_print("%s-%d: type %x, address %x, next dnsAnswer %02x%0x%02x%02x, dnsAnswer_len %d\n", __func__,__LINE__,type,*address,*(dnsAnswer),*(dnsAnswer+1),*(dnsAnswer+2),*(dnsAnswer+3),dnsAnswer_len);
		}else{
		    break;
		}
	    } else {
		break;
	    }
	}
#if 0
	kfree_skb(nskb);
    }
#endif
#if 0
    struct sk_buff *nskb=NULL;
    struct iphdr *iph=NULL;
    nskb = skb_copy(skb,GFP_ATOMIC);
    if(nskb) {
	iph = ip_hdr(skb);
	if((iph!=NULL) && (address > 0)) {
	    iph->saddr = address;
	    ipsetindex = ip_set_get_byname(WHITE_LIST);
	    if(ipsetindex != IP_SET_INVALID_ID) {
		if(ip_set_testip_kernel(ipsetindex, skb, &flag)==0){ /*检测IP*/
		    ip_set_addip_kernel(ipsetindex, skb, &flag);
		    wifi_match_debug_print("%s-%d: add address %x, in set name %s\n", __func__,__LINE__,ntohl(iph->saddr),WHITE_LIST);
		}
		ip_set_put_byindex(ipsetindex);
	    }
	}
	kfree_skb(nskb);
    }
#endif
    return ret;
}
#if 0
int
ip_set_testip_kernel_user_define(char *name,
		     ip_set_ip_t ip,
		     char *settype)
{
    struct ip_set *set;
    int res = 0;
    ip_set_id_t index = IP_SET_INVALID_ID;

    index = ip_set_get_byname(name);
    if(ipsetindex != IP_SET_INVALID_ID) {
	read_lock_bh(&ip_set_lock);
	set = ip_set_list[index];
	IP_SET_ASSERT(set);
	DP("set %s, index %u, settype %s", set->name, index,set->type->typename);

	read_lock_bh(&set->lock);
	if(strcmp(settype, "iphash") == 0) {
	    res = FNAME(settype,"_","test")(set, ip);			\
	}
	//res = set->type->testip_kernel(set, skb, flags);
	read_unlock_bh(&set->lock);

	read_unlock_bh(&ip_set_lock);
	ip_set_put_byindex(index);
    }

    return (res < 0 ? 0 : res);
}
#endif
#define MAX_WIFI_DNS_NAME 256
static bool wifiMatch_mt(const struct sk_buff *skb, const struct xt_match_param *par)
{
#if 1
    bool rst = true;
    const struct xt_wifiMatch_info *info = par->matchinfo;
#endif
    struct iphdr *iph;
    struct udphdr *udph;
    struct dnshdr *dnsH;
    int found = 0;
    unsigned char domainName[MAX_WIFI_DNS_NAME + 1];
    int dnsResponsePktlen = 0;
    DnsEntryVal *dnsentry = NULL;
    
    iph = ip_hdr(skb);
    /* 非ip包 */
    if (!iph) {
	wifi_match_debug_print("iph == NULL\n");
	return true;
    }

//wifi_match_debug_print("%s-%d: info->wififlag %d\n", __func__,__LINE__,info->wififlag);
    if (info->wififlag & XT_WIFIMATCH_OP_DNS_WHITE_LIST_ADD_IP) {
	/*域名白名单*/
	if (iph->protocol == IPPROTO_UDP) {/* UDP */
	    udph = (void *)iph + iph->ihl * 4;
	    if ((udph != NULL) && (ntohs(udph->source) == 53)) {
		/* 源端口53, dns应答包 */
		/* 长度错误, 小于udp头长度加上dns头长度 */
		if (ntohs(udph->len) < 20) {
		    wifi_match_debug_print("%s-%d: skb udp source %d, udp len < 20\n", __func__,__LINE__,ntohs(udph->source));
		    return true;
		}
		memset(domainName, 0, sizeof(domainName));
		dnsH = (struct dnshdr *)(udph + 1);
		dnsResponsePktlen = ntohs(udph->len) - sizeof(struct udphdr);
		found = getDomainName(dnsH, dnsResponsePktlen, domainName);
		domainName[MAX_WIFI_DNS_NAME] = '\0';
		wifi_match_debug_print("%s-%d: udp source port %d, udp len %d,udp head len %d, dnsResponsePktlen %d,found %d\n", __func__,__LINE__,ntohs(udph->source),ntohs(udph->len), sizeof(struct udphdr), dnsResponsePktlen, found);
		if(found) {
		    wifi_match_debug_print("%s-%d: Get Domain Name: %s, len %u,\n", __func__,__LINE__,domainName,strlen(domainName));
		    dnsentry = wifidog_dnsHashLookupData(domainName);
		    if(dnsentry!=NULL) {
			addDnsIpFilter(skb, dnsH, dnsResponsePktlen, dnsentry, domainName);
		    }
		}
		wifi_match_debug_print("%s-%d: end\n", __func__,__LINE__,domainName,strlen(domainName));
	    }
	}
    } 
#if 0
    if ((rst == false) &&(info->wififlag & )) {
	/* wifidog anopther match flag */
	    rst = ;
    } 
#endif

    return rst;
}

static int wifiMatch_mt_check(const struct xt_mtchk_param *par)
{

#if 1
    const struct xt_wifiMatch_info *info = par->matchinfo;

    /* 
     * dns whilte not support '!'
     */
    if (info->invert & XT_WIFIMATCH_OP_DNS_WHITE_LIST_ADD_IP) {
	wifi_match_debug_print(KERN_WARNING "wifiMatch match: add-dns-ip not support '!'\n");

	return -EINVAL;
    }
#endif
	return 0;
}

static struct xt_match wifiMatch_mt_reg __read_mostly = {
	.name       = "wifiMatch",
	.revision   = 0,
	.family     = NFPROTO_UNSPEC,
	.checkentry = wifiMatch_mt_check,
	.match      = wifiMatch_mt,
	.matchsize  = sizeof(struct xt_wifiMatch_info),
	.me         = THIS_MODULE,
};

static int __init wifiMatch_mt_init(void)
{
	return xt_register_match(&wifiMatch_mt_reg);
}

static void __exit wifiMatch_mt_exit(void)
{
	xt_unregister_match(&wifiMatch_mt_reg);
}

module_init(wifiMatch_mt_init);
module_exit(wifiMatch_mt_exit);
