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
 *��������ģ������hashͰ��С
 */
#define DNS_FILTER_HASH_TABLE_SIZE              211

/**
 *��������hash keyʱ�Ĵ���key
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
    unsigned char name[MAXDNAME+1]; /*����*/
    __u16 hashKey;/*hashKey*/
} UttNfDnsAid; /*�����ṹ�����ڿ�����Ч*/

typedef struct{
    atomic_t ref;/*�Ƿ�ip�ڵ�����*/
    IpGrpEntry ipSrc;  /*ip��Ϣ������ע��ipset��index*/
    uint32_t timeSetIndex;/*ʱ���*/
    uint32_t matchDrop;/*0->��ֹ�б��е�������������; 1->�����б��е�����������ֹ*/
    rwlock_t lock;
    bool isNotice; /*��ֹ���Ƿ����ͨ��*/
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
 *����hash�������
 */
typedef enum {
    DNS_FILTER_MODULE = 0, /*��������ģ���Ӧ������hash������*/
    BEHAVIOR_MANAGEMENT_MODULE /*��Ϊ����ģ���Ӧ������hash������*/
} DNS_HASHTABLE_TYPE;

/**
 * find_dns_filter_entry_value - ������������Ӧhash������Ӧ����Ŀ 
 * @domainName: ��dns���л�ȡ������.
 * @type:       ����hash�������
 */
extern dns_filter_entry *find_dns_filter_entry_value(const char *domainName);

/**
 * utt_netlink_dns_filter - ��netlink��Ӧ�ò���յ����������������ͽ������/ɾ������
 * @skb: netlink��Ӧ�ò���յ������ݰ�
 */
extern void utt_netlink_dns_filter(struct sk_buff *skb);

/**
 *��ȡ������Ӧ��hash key
 */
extern __u16 dns_hash_key(const char *dns, __u16 len);

/**
 * isDnsMatch - �Ƚ�domainName�Ƿ���hashKey��Ӧ�ı���Ŀ������ƥ�� 
 * @hashKey: hash������.
 * @domainName: skb�е�����
 *
 * ���أ�1-ƥ��ɹ���0-ƥ��ʧ��
 */
extern int isDnsMatch(__u16 hashKey, const char *domainName);

/**
 *���һ��hash����
 */
extern bool dns_hash_insert(const char *domainName);
/**
 *ɾ��hash���¼
 */
extern bool dns_hash_delete(const char *domainName);
/**
 *ɾ��hash�����м�¼
 */
extern bool dns_hash_delete_all (void);

extern int getDomainName(DNS_HDR *dns_hdr, int dns_len, unsigned char *buffer);

/*hash�����������*/
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
