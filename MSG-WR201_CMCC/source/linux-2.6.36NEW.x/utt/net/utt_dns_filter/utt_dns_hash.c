#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/types.h>
#include <net/sock.h>
#include <net/netlink.h>
#include <linux/netfilter_ipv4/utt_dns_hash.h>

/**
 * ����hash����ģ��
 * 
 * �������Ժ���ͨ���
 * ͨ�������ֹ��hash������
 *
 * �����а���mana��׺��ǰ׺�ġ���ʾ����ʱ ����ͨ���
 *
 */
#define DEBUG_UTT_DNS 0
#if DEBUG_UTT_DNS
#define UTT_DNS_DEBUG(fmt, argx...) printk(fmt, ##argx)
#else
#define UTT_DNS_DEBUG(fmt, argx...)
#endif

/**
 * ��������ģ������hash��
 */
static struct hlist_head dnsFilterHashTable[DNS_FILTER_HASH_TABLE_SIZE + 1];
/**
 *��Ϊ����ģ������hash��
 */
static struct hlist_head *dns_hash_table(__u16 hashIndex);
static bool  cmp_two_str_mana(const char *str1, const char *str2);
static __u16 dns_hash_key_mana(const char *domainName);
static __u16 dns_hash_lookup_exact(const char *domainName);

static struct hlist_head *dns_hash_table(__u16 hashIndex)
{
    struct hlist_head *head = NULL;

    if (hashIndex != DNS_HASH_ERR_KEY) {
	head = &dnsFilterHashTable[hashIndex];
    }

    return head;
}
/*
 *
 * ����key�����㷨
 *
 * dns: �����еĲ��ֻ�ȫ���ִ���������hash
 * len:
 *    length of the dns (must less than MAX_DOMAIN_NAME_LENGTH).
 *
 */
__u16 dns_hash_key(const char *dns, __u16 len)
{
    const char *end;
    char lowerDomain[MAX_DOMAIN_NAME_LENGTH + 1];
    char *curseDomain;
    __u16 key,hashIndex;
    __u16 i;

    memset(lowerDomain, 0, sizeof(lowerDomain));
    for (i = 0u; i < len; i++) {
        lowerDomain[i] = tolower(dns[i]);/*ת��ΪСд�ַ�*/
    }
    curseDomain = lowerDomain;

    end = curseDomain + len;
    /*hash key����*/
    for (key = 0u; curseDomain < end; curseDomain++) {
        key *= 16777619u;
        key ^= (__u16) *(char*) curseDomain;
    }

    hashIndex = key % DNS_FILTER_HASH_TABLE_SIZE;

    return hashIndex;/*ʹkeyС��hash����*/
}
EXPORT_SYMBOL(dns_hash_key);

/**
 * ��hash���в�������
 */
bool dns_hash_insert(const char * domainName)
{
    __u16              hashIndex;
    dns_filter_entry      *newEntry;
    struct hlist_head *head;

    hashIndex = dns_hash_lookup_exact(domainName);/*��ȫƥ�����*/
    UTT_DNS_DEBUG("%s-%d name %s key %d.\n", __FUNCTION__, __LINE__, domainName, hashIndex);

    if (hashIndex == DNS_HASH_ERR_KEY) {
        UTT_DNS_DEBUG("Fail: domainName %s already inserted! @%s-%d.\n",
                domainName, __FUNCTION__, __LINE__);
        return false;
    }


    newEntry = (dns_filter_entry*)kzalloc(sizeof(dns_filter_entry), GFP_KERNEL);

    if (newEntry == NULL) {
        UTT_DNS_DEBUG("%s-%d GETBUFF error.\n", __FUNCTION__, __LINE__);
        return false;
    }
    strcpy(newEntry->domainName, domainName);


    head = dns_hash_table(hashIndex);
    if (head == NULL) {
	kfree(newEntry);
        UTT_DNS_DEBUG("%s-%d GET hashTable error.\n", __FUNCTION__, __LINE__);
        return false;
    }
    hlist_add_head(&newEntry->node,head);

    UTT_DNS_DEBUG("%s-%d domain %s.\n", __FUNCTION__, __LINE__, domainName);

    return true;
}
EXPORT_SYMBOL(dns_hash_insert);


/**
 * ɾ����Ӧ��������
 */
bool dns_hash_delete (const char *domainName)
{
    __u16              hashIndex;
    dns_filter_entry      *toDel;
    struct hlist_node *pos,*n;
    struct hlist_head *head;
 
    hashIndex = dns_hash_key_mana(domainName);

    if ((head = dns_hash_table(hashIndex)) == NULL) {
	return false;
    }

    UTT_DNS_DEBUG("%s-%d key %d.\n", __FUNCTION__, __LINE__, hashIndex);

    if (!hlist_empty(head)) {
	hlist_for_each_entry_safe(toDel,pos,n,head,node) {
	    if (strcmp(domainName, toDel->domainName) == 0) {
		hlist_del_init(pos);
		kfree(toDel);
                return true;
	    }
	}
    }

    return false;
}
EXPORT_SYMBOL(dns_hash_delete);

/**
 * ɾ�����е�������
 */
bool dns_hash_delete_all (void)
{
    __u16              hashIndex;
    dns_filter_entry      *toDel;
    struct hlist_node *pos,*n;
    struct hlist_head *head;
 
    for (hashIndex = 0; hashIndex <= DNS_FILTER_HASH_TABLE_SIZE; hashIndex++) {
	if ((head = dns_hash_table(hashIndex)) != NULL) {
	    if (!hlist_empty(head)) {
		hlist_for_each_entry_safe(toDel,pos,n,head,node) {
		    hlist_del_init(pos);
		    kfree(toDel);
		    toDel = NULL;
		}
	    }
	}
    }

    return true;
}
EXPORT_SYMBOL(dns_hash_delete_all);
/*
 * �� �� ��: cmp_two_str_mana
 * ���������������ַ����ȽϺ�������ͨ���*�ıȽϣ�
 * ���������
 *          - str1:���ܺ�ͨ������ַ���ָ��
 *          - str2:��dns����ȡ��������ָ�� 
 * ����ֵ��
 *          �����ַ����ȽϵĽ�������Ϊtrue�������Ϊfalse
 * �������ڣ�08-06-26
 * ���ߣ�   ������
 * ����˵����
 */

static bool  cmp_two_str_mana(const char *str1, const char *str2)
{
    char temp[MAX_DOMAIN_NAME_LENGTH + 1];
    const char *pointer = str2;
    const char *pointer1 = str1;
    int length1 = strlen(str1);
    int num =0;
    int i;



    for(i = 0; i<= length1; i++, str1++) {
        if((*str1 =='*') || (*str1 =='\0')) {
            temp[num] = '\0';               /* mod by llliu@080804 */
            num = 0;

            if(strlen(temp) == 0)
                continue;

            pointer = utt_strcasestr(pointer,temp);
            if(pointer == NULL) {
                return false;
            }

           pointer += strlen(temp);
        } else {
            temp[num++] = *str1;
        }
    }

    /*
     * compare the last char 
     */
    if(pointer1[length1 - 1] != '*') {
	if(*pointer == '\0') {
	    return true;
	}
	else {
	    return false;
	}
    }
    else {
	return true;
    }
}

/*
 * �� �� ��: dnsHashKeyData
 * ��������������ƽ���hash���Һ�����
 * ���������
 *          - domainName: ��ȡ����������
 * �����������
 * ����ֵ��  ���ز��ҵ�hash��㡣
 *
 * �������ڣ�08-06-26
 * ���ߣ�    ������
 * ����˵�������Ƚ��о�ȷƥ����ң����û���ҵ��������ͨ���ƥ�䡣
 */
dns_filter_entry *find_dns_filter_entry_value(const char *domainName)
{
    dns_filter_entry *entry;
    struct hlist_head *head;
    struct hlist_node *pos;
    __u16          hashIndex;
    int             domainNameLen;

    if (domainName == NULL) {
	return NULL;
    }
    hashIndex = dns_hash_key_mana(domainName);

    if ((head = dns_hash_table(hashIndex)) == NULL) {
	return NULL;
    }

    domainNameLen = strlen(domainName);
    UTT_DNS_DEBUG("%s-%d start serach name:%s.\n", __FUNCTION__, __LINE__, domainName);
    if (domainNameLen > MAX_DOMAIN_NAME_LENGTH) {/*������ݰ��е��������Ƚϳ�������ͨ�������*/
	goto wildcardMatch;
    }

    /*
     * ���Ƚ��о�ȷƥ�����,���hash���д��ڣ���ֱ�ӷ���
     */
    hlist_for_each_entry(entry,pos,head,node) {
	if (strnicmp(entry->domainName, domainName, MAX_DOMAIN_NAME_LENGTH + 1) == 0) {
	    return entry;
	}
    }

    /**
     * ͨ�������
     */
wildcardMatch:
    /*
     * ok, let it fall through to this.
     * go through the wildcard header.
     */
    if ((head = dns_hash_table(DNS_FILTER_HASH_TABLE_SIZE)) == NULL) {
	hlist_for_each_entry(entry,pos,head,node) {
	    if (cmp_two_str_mana(entry->domainName, domainName)) {
		return entry;
	    }
	}
    }
    UTT_DNS_DEBUG("%s-%d can't find name:%s.\n", __FUNCTION__, __LINE__, domainName);

    return NULL;
}
EXPORT_SYMBOL(find_dns_filter_entry_value);

int isDnsMatch(__u16 hashIndex, const char *domainName)
{
    dns_filter_entry *entry;
    struct hlist_head *head;
    struct hlist_node *pos;
    int domainNameLen;

    if ((domainName == NULL) || (*domainName == '\0')) {
	return 0;
    }

    if ((head = dns_hash_table(hashIndex)) == NULL) {
	return 0;
    }
    domainNameLen = strlen(domainName);
    UTT_DNS_DEBUG("%s-%d start serach name:%s,domainNameLen=%d.\n", __func__, __LINE__, domainName,domainNameLen);
    if (domainNameLen > MAX_DOMAIN_NAME_LENGTH) {/*������ݰ��е��������Ƚϳ�������ͨ�������*/
	goto wildcardMatch;
    }
    UTT_DNS_DEBUG("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);

    /*
     * ���Ƚ��о�ȷƥ�����,���hash���д��ڣ���ֱ�ӷ���
     */
    hlist_for_each_entry(entry,pos,head,node) {
	UTT_DNS_DEBUG("----file:%s,func:%s,line:%d,entry->domainName=%s-----\n",__FILE__,__func__,__LINE__,entry->domainName);
	if (strnicmp(entry->domainName, domainName, MAX_DOMAIN_NAME_LENGTH + 1) == 0) {
	    return 1;
	}
    }

    /**
     * ͨ�������
     */
wildcardMatch:
    /*
     * ok, let it fall through to this.
     * go through the wildcard header.
     */
    if ((head = dns_hash_table(DNS_FILTER_HASH_TABLE_SIZE)) != NULL) {
	hlist_for_each_entry(entry,pos,head,node) {
	    UTT_DNS_DEBUG("----file:%s,func:%s,line:%d,entry->domainName=%s-----\n",__FILE__,__func__,__LINE__,entry->domainName);
	    if (cmp_two_str_mana(entry->domainName, domainName)) {
		return 1;
	    }
	}
    }

    return 0;
}
EXPORT_SYMBOL(isDnsMatch);
/*
 * �� �� ��: dns_hash_key_mana
 * ��������������ƽ���hash����, �������ͨ�������ֱ�ӷ���
 *           DNS_FILTER_HASH_TABLE_SIZE;
 * ���������
 *          - domainName:  hash����
 * ���������
 *          ��
 * ����ֵ��  ����
 *
 * �������ڣ�08-06-26
 * ���ߣ�    ������
 * ����˵������
 *
 */
static __u16 dns_hash_key_mana(const char *domainName)
{
    if (strchr(domainName, '*')) {
        return DNS_FILTER_HASH_TABLE_SIZE;
    } else {
	return dns_hash_key(domainName,strlen(domainName));
    }

}


/**
 * ��ȷƥ�����
 */
static __u16 dns_hash_lookup_exact(const char *domainName)
{
    dns_filter_entry *entry;
    struct hlist_node *pos, *n;
    __u16          hashIndex;
    struct hlist_head *head;

    hashIndex = dns_hash_key_mana(domainName);
    UTT_DNS_DEBUG("\n --------%s %d name %s key %d \n", __func__, __LINE__, domainName, hashIndex);

    if ((head = dns_hash_table(hashIndex)) != NULL) {
	if (!hlist_empty(head)) {
	    hlist_for_each_entry_safe(entry,pos,n,head,node) {
		if (strnicmp(entry->domainName,domainName,MAX_DOMAIN_NAME_LENGTH) == 0) {
		    hashIndex = DNS_HASH_ERR_KEY;
		    break;
		}
	    }
	}
    } else {
	hashIndex = DNS_HASH_ERR_KEY;
    }


    return hashIndex;
}

/**
 * ����hash��
 * processFun : ������
 */
void utt_dns_traverse(void (*processFun)(dns_filter_entry* entry))
{
    int i = 0;
    dns_filter_entry *hashBufP = NULL;
    struct hlist_node *pos,*n;

    for(i = 0; i < DNS_FILTER_HASH_TABLE_SIZE + 1; i++) {
	if (!hlist_empty(&dnsFilterHashTable[i])) {
	    hlist_for_each_entry_safe(hashBufP,pos,n,&dnsFilterHashTable[i],node) {
		processFun(hashBufP);
	    }
	}
    }
}
EXPORT_SYMBOL(utt_dns_traverse);
