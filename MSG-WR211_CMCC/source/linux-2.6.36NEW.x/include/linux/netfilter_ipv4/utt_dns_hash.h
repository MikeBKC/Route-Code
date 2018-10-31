#ifndef __UTT_DNS_HASH_H__
#define __UTT_DNS_HASH_H__

#include <linux/types.h>
#include <linux/list.h>
#include <linux/netfilter_ipv4/utt_filter_kernel.h>
#include <linux/netfilter_ipv4/utt_netfilter.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/udp.h>
#include <linux/netfilter_ipv4/ipt_domain.h>

/**
 *域名过滤模块域名hash桶大小
 */
#define DNS_FILTER_HASH_TABLE_SIZE              211

/**
 *生成域名hash key时的错误key
 */
#define DNS_HASH_ERR_KEY                 DNS_FILTER_HASH_TABLE_SIZE + 100

typedef  struct {
	__u16	id;		/* query identification number */
# ifdef BIG_ENDIAN
	/* fields in third byte */
	__u8	qr:1;		/* response flag */
	__u8	opcode:4;	/* purpose of message */
	__u8	aa:1;		/* authoritive answer */
	__u8	tc:1;		/* truncated message */
	__u8	rd:1;		/* recursion desired */
	/* fields in fourth byte */
	__u8	ra:1;		/* recursion available */
	__u8	pr:1;		/* primary server required (non standard) */
	__u8	unused:2;	/* unused bits */
	__u8	rcode:4;	/* response code */
# else
	/* fields in third byte */
	__u8	rd:1;		/* recursion desired */
	__u8	tc:1;		/* truncated message */
	__u8	aa:1;		/* authoritive answer */
	__u8	opcode:4;	/* purpose of message */
	__u8	qr:1;		/* response flag */
	/* fields in fourth byte */
	__u8	rcode:4;	/* response code */
	__u8	unused:2;	/* unused bits */
	__u8	pr:1;		/* primary server required (non standard) */
	__u8	ra:1;		/* recursion available */
# endif
	/* remaining bytes */
	__u16	qdcount;	/* number of question entries */
	__u16	ancount;	/* number of answer entries */
	__u16	nscount;	/* number of authority entries */
	__u16	arcount;	/* number of resource entries */
}DNS_HDR;

#ifndef MAXDNAME 
#define MAXDNAME 256 
#endif
typedef struct st_uttNfDnsAid{
    unsigned char name[MAXDNAME+1]; /*域名*/
    __u16 hashKey;/*hashKey*/
} UttNfDnsAid; /*辅助结构体用于快速生效*/

typedef struct{
    atomic_t ref;/*是否被ip节点引用*/
    IpGrpEntry ipSrc;  /*ip信息。这里注意ipset用index*/
    uint32_t timeSetIndex;/*时间段*/
    uint32_t matchDrop;/*0->禁止列表中的域名其他允许; 1->允许列表中的域名其他禁止*/
    rwlock_t lock;
    bool isNotice; /*禁止的是否进行通告*/
} UttDomaindfCtl;

typedef struct {
    struct hlist_node node;
#if 0
    DnsFilterEntryVal  entryVal;
#else
    char    domainName[MAX_DOMAIN_NAME_LENGTH + 1]; 
#endif
} dns_filter_entry;
/**
 *域名hash表的类型
 */
typedef enum {
    DNS_FILTER_MODULE = 0, /*域名过滤模块对应的域名hash表类型*/
    BEHAVIOR_MANAGEMENT_MODULE /*行为管理模块对应的域名hash表类型*/
} DNS_HASHTABLE_TYPE;

/**
 * find_dns_filter_entry_value - 查找域名在相应hash表的项对应的条目 
 * @domainName: 从dns包中获取的域名.
 * @type:       域名hash表的类型
 */
extern dns_filter_entry *find_dns_filter_entry_value(const char *domainName);

/**
 * utt_netlink_dns_filter - 将netlink从应用层接收到的域名，根据类型进行添加/删除操作
 * @skb: netlink从应用层接收到的数据包
 */
extern void utt_netlink_dns_filter(struct sk_buff *skb);

/**
 *获取域名对应的hash key
 */
extern __u16 dns_hash_key(const char *dns, __u16 len);

/**
 * isDnsMatch - 比较domainName是否与hashKey对应的表条目中域名匹配 
 * @hashKey: hash表索引.
 * @domainName: skb中的域名
 *
 * 返回：1-匹配成功，0-匹配失败
 */
extern int isDnsMatch(__u16 hashKey, const char *domainName);

/**
 *添加一条hash表项
 */
extern bool dns_hash_insert(const char *domainName);
/**
 *删除hash表记录
 */
extern bool dns_hash_delete(const char *domainName);
/**
 *删除hash表所有记录
 */
extern bool dns_hash_delete_all (void);

extern int getDomainName(DNS_HDR *dns_hdr, int dns_len, unsigned char *buffer);

/*hash表遍历处理函数*/
extern void utt_dns_traverse(void (*processFun)(dns_filter_entry* entry));

static inline UttNfDnsAid *uttNfDnsAidBuild(struct sk_buff *skb,UttNfDnsAid *aid)
{
    struct iphdr *iph;
    struct udphdr *udph;
    DNS_HDR *dnsPkt;
    int pktlen;
    int domainNameLen;
    unsigned char name[MAXDNAME + 1] = {0};

    if (aid != NULL) {
	iph = ip_hdr(skb);
	if ((iph == NULL) || (iph->protocol != IPPROTO_UDP)) {/* UDP */
	    return NULL;
	}
	udph = (struct udphdr *)((void *)iph + iph->ihl * 4);
	if ((udph == NULL) || (ntohs(udph->dest) != 53) || (ntohs(udph->len) < 20)) {
	    return NULL;
	}
	dnsPkt = (DNS_HDR *)(udph + 1);
	pktlen = ntohs(udph->len) - sizeof(struct udphdr);

	if (getDomainName(dnsPkt,pktlen,name)) {
	    domainNameLen = strlen(name);
	    strncpy(aid->name,name,MAXDNAME);
	    if (domainNameLen > MAX_DOMAIN_NAME_LENGTH) {
		aid->hashKey = DNS_FILTER_HASH_TABLE_SIZE;
	    } else {
		aid->hashKey = dns_hash_key(aid->name, strlen(aid->name));
	    }
	}
    }

    return aid;
}
#endif
