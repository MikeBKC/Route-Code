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
 * 域名hash功能模块
 * 
 * 域名可以含有通配符
 * 通配符被防止于hash表的最后
 *
 * 函数中包含mana后缀或前缀的。表示操作时 考虑通配符
 *
 */
#define DEBUG_UTT_DNS 0
#if DEBUG_UTT_DNS
#define UTT_DNS_DEBUG(fmt, argx...) printk(fmt, ##argx)
#else
#define UTT_DNS_DEBUG(fmt, argx...)
#endif

/**
 * 域名过滤模块域名hash表
 */
static struct hlist_head dnsFilterHashTable[DNS_FILTER_HASH_TABLE_SIZE + 1];
/**
 *行为管理模块域名hash表
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
 * 域名key生成算法
 *
 * dns: 域名中的部分或全部字串。用于做hash
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
        lowerDomain[i] = tolower(dns[i]);/*转换为小写字符*/
    }
    curseDomain = lowerDomain;

    end = curseDomain + len;
    /*hash key生成*/
    for (key = 0u; curseDomain < end; curseDomain++) {
        key *= 16777619u;
        key ^= (__u16) *(char*) curseDomain;
    }

    hashIndex = key % DNS_FILTER_HASH_TABLE_SIZE;

    return hashIndex;/*使key小于hash表长度*/
}
EXPORT_SYMBOL(dns_hash_key);

/**
 * 往hash表中插入域名
 */
bool dns_hash_insert(const char * domainName)
{
    __u16              hashIndex;
    dns_filter_entry      *newEntry;
    struct hlist_head *head;

    hashIndex = dns_hash_lookup_exact(domainName);/*完全匹配查找*/
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
 * 删除对应的域名项
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
 * 删除所有的域名项
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
 * 函 数 名: cmp_two_str_mana
 * 功能描述：两个字符串比较函数（含通配符*的比较）
 * 输入参数：
 *          - str1:可能含通配符的字符串指针
 *          - str2:从dns包中取到的域名指针 
 * 返回值：
 *          两个字符串比较的结果，相等为true，不相等为false
 * 创建日期：08-06-26
 * 作者：   王炳荣
 * 附加说明：
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
 * 函 数 名: dnsHashKeyData
 * 功能描述：数据平面的hash查找函数。
 * 输入参数：
 *          - domainName: 获取到的域名。
 * 输出参数：无
 * 返回值：  返回查找到hash结点。
 *
 * 创建日期：08-06-26
 * 作者：    刘亮亮
 * 附加说明：首先进行精确匹配查找，如果没有找到，则进行通配符匹配。
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
    if (domainNameLen > MAX_DOMAIN_NAME_LENGTH) {/*如果数据包中的域名长度较长，进行通配符查找*/
	goto wildcardMatch;
    }

    /*
     * 首先进行精确匹配查找,如果hash表中存在，则直接返回
     */
    hlist_for_each_entry(entry,pos,head,node) {
	if (strnicmp(entry->domainName, domainName, MAX_DOMAIN_NAME_LENGTH + 1) == 0) {
	    return entry;
	}
    }

    /**
     * 通配符查找
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
    if (domainNameLen > MAX_DOMAIN_NAME_LENGTH) {/*如果数据包中的域名长度较长，进行通配符查找*/
	goto wildcardMatch;
    }
    UTT_DNS_DEBUG("----file:%s,func:%s,line:%d-----\n",__FILE__,__func__,__LINE__);

    /*
     * 首先进行精确匹配查找,如果hash表中存在，则直接返回
     */
    hlist_for_each_entry(entry,pos,head,node) {
	UTT_DNS_DEBUG("----file:%s,func:%s,line:%d,entry->domainName=%s-----\n",__FILE__,__func__,__LINE__,entry->domainName);
	if (strnicmp(entry->domainName, domainName, MAX_DOMAIN_NAME_LENGTH + 1) == 0) {
	    return 1;
	}
    }

    /**
     * 通配符查找
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
 * 函 数 名: dns_hash_key_mana
 * 功能描述：管理平面的hash函数, 如果含有通配符，则直接返回
 *           DNS_FILTER_HASH_TABLE_SIZE;
 * 输入参数：
 *          - domainName:  hash对象
 * 输出参数：
 *          无
 * 返回值：  返回
 *
 * 创建日期：08-06-26
 * 作者：    刘亮亮
 * 附加说明：无
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
 * 精确匹配查找
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
 * 遍历hash表
 * processFun : 处理函数
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
