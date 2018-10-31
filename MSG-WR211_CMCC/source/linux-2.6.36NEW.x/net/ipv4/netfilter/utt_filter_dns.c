#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/types.h>
#include <net/sock.h>
#include <net/netlink.h>
#include <linux/netfilter_ipv4/utt_filter_kernel.h>

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


#define DNS_FILTER_HASH_TABLE_SIZE              211
#define DNS_FILTER_HASH_ERR_KEY                 DNS_FILTER_HASH_TABLE_SIZE + 100

typedef struct st_DnsFilterEntry {
    DnsFilterEntryVal  entryVal;
    struct  st_DnsFilterEntry *next;
} DnsFilterEntry;

static DnsFilterEntry *dnsHashTable[DNS_FILTER_HASH_TABLE_SIZE + 1];

static __u16 dnsHashKey(const char *dns, __u16 len);
static bool dnsHashInsert(DnsFilterEntryVal* newEntryVal);
static bool dnsHashDelete(DnsFilterEntryVal* oldEntryVal);
static bool  cmpTwoStrMana(char *str1,char *str2);
static __u16 dnsHashKeyMana(char *domainName);
static __u16 dnsHashLookupExact(char *domainName);


void uttNlDnsFilter(struct sk_buff *skb);
/**
 *  忽略大小写子窜匹配
 *  bhou
 *  注：该函数移植于 2.6内核的 strcasestr函数。如果系统本身含有strcasestr函数。可不用改函数
 */
char *utt_strcasestr(const char *s1, const char *s2)
{
    register const char *s = s1;
    register const char *p = s2;

#if 1
    do {
	if (!*p) {
	    return (char *) s1;;
	}
	if ((*p == *s)
		|| (tolower(*((unsigned char *)p)) == tolower(*((unsigned char *)s)))
	   ) {
	    ++p;
	    ++s;
	} else {
	    p = s2;
	    if (!*s) {
		return NULL;
	    }
	    s = ++s1;
	}
    } while (1);
#else
    while (*p && *s) {
	if ((*p == *s)
		|| (tolower(*((unsigned char *)p)) == tolower(*((unsigned char *)s)))
	   ) {
	    ++p;
	    ++s;
	} else {
	    p = s2;
	    s = ++s1;
	}
    }

    return (*p) ? NULL : (char *) s1;
#endif
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
static __u16 dnsHashKey(const char *dns, __u16 len)
{
    const char *end;
    char lowerDomain[MAX_DOMAIN_NAME_LENGTH + 1];
    char *curseDomain;
    __u16 key;
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

    return (key % DNS_FILTER_HASH_TABLE_SIZE);/*使key小于hash表长度*/
}

/**
 * 往hash表中插入域名
 */
static bool dnsHashInsert(DnsFilterEntryVal* newEntryVal)
{
    __u16              hashIndex;
    DnsFilterEntry      *newEntry;
    char *domainName  = newEntryVal->domainName;

#if 0
    if (domainName == NULL || strlen(domainName) == 0) {
       return false;
    }
#endif

    hashIndex = dnsHashLookupExact(domainName);/*完全匹配查找*/
    UTT_DNS_DEBUG("%s-%d name %s key %d.\n", __FUNCTION__, __LINE__, domainName, hashIndex);

    if (hashIndex == DNS_FILTER_HASH_ERR_KEY) {
        UTT_DNS_DEBUG("Fail: domainName %s already inserted! @%s-%d.\n",
                domainName, __FUNCTION__, __LINE__);
        return false;
    }


    newEntry = (DnsFilterEntry *)kmalloc(sizeof(DnsFilterEntry), GFP_KERNEL);

    if (newEntry == NULL) {
        UTT_DNS_DEBUG("%s-%d GETBUFF error.\n", __FUNCTION__, __LINE__);
        return false;
    }
    memcpy(&(newEntry->entryVal), newEntryVal, sizeof(DnsFilterEntry));
    newEntry->next = dnsHashTable[hashIndex];
    dnsHashTable[hashIndex] = newEntry;


    UTT_DNS_DEBUG("%s-%d domain %s.\n", __FUNCTION__, __LINE__, domainName);

    return true;
}


/*
 * 删除对应的域名项
 * */
static bool dnsHashDelete(DnsFilterEntryVal* oldEntryVal)
{
    __u16              hashIndex;
    DnsFilterEntry      *toDel;
    DnsFilterEntry      *prevDel;


    char *domainName  = oldEntryVal->domainName;
 
    prevDel = NULL;

    hashIndex = dnsHashKeyMana(domainName);

    UTT_DNS_DEBUG("%s-%d key %d.\n", __FUNCTION__, __LINE__, hashIndex);

    for (toDel = dnsHashTable[hashIndex];
            toDel;
            prevDel = toDel, toDel = toDel->next) {
        if (strcmp(domainName, toDel->entryVal.domainName) == 0) {
            if (toDel == dnsHashTable[hashIndex]) {
                dnsHashTable[hashIndex] = toDel->next;
		UTT_DNS_DEBUG("%s-%d domain %s.\n", __FUNCTION__, __LINE__, domainName);
		kfree(toDel);
                return true;
            }
            else {
                prevDel->next = toDel->next;
		UTT_DNS_DEBUG("%s-%d domain %s.\n", __FUNCTION__, __LINE__, domainName);
                kfree(toDel);
                return true;
            }
        }
    }

    return false;
}

/*
 * 函 数 名: cmpTwoStrMana
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

static bool  cmpTwoStrMana(char *str1,char *str2)
{
    char temp[MAX_DOMAIN_NAME_LENGTH + 1];
    char *pointer = str2;
    char *pointer1 = str1;
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
DnsFilterEntryVal *dnsHashLookupData(char *domainName)
{
    DnsFilterEntry  *entry;

    __u16          hashIndex;
    int             domainNameLen;

    domainNameLen = strlen(domainName);

    UTT_DNS_DEBUG("%s-%d start serach name:%s.\n", __FUNCTION__, __LINE__, domainName);
    if (domainNameLen > MAX_DOMAIN_NAME_LENGTH) {
	goto wildcardMatch;
    }


    hashIndex = dnsHashKey((const char *)domainName, domainNameLen);

    /*
     * first go through the exact header,
     */
    for (entry = dnsHashTable[hashIndex];
	    entry;
	    entry = entry->next) {
	if (strcasecmp(entry->entryVal.domainName, domainName) == 0) {
	    return &(entry->entryVal);
	}
    }

wildcardMatch:

    /*
     * ok, let it fall through to this.
     * go through the wildcard header.
     */
    for (entry = dnsHashTable[DNS_FILTER_HASH_TABLE_SIZE];
	    entry;
	    entry = entry->next) {

	if (cmpTwoStrMana(entry->entryVal.domainName, domainName) == 1) {
	    return &(entry->entryVal);
	}
    }

    UTT_DNS_DEBUG("%s-%d can't find name:%s.\n", __FUNCTION__, __LINE__, domainName);
    return NULL;
}


/*
 * 函 数 名: dnsHashKeyMana
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
static __u16 dnsHashKeyMana(char *domainName)
{
    if (strchr(domainName, '*')) {
        return DNS_FILTER_HASH_TABLE_SIZE;
    }
    else {
        return dnsHashKey((const char *)domainName, strlen(domainName));
    }
}


/**
 * 精确匹配查找
 */
static __u16 dnsHashLookupExact(char *domainName)
{
    DnsFilterEntry  *entry;

    __u16          hashIndex;

    hashIndex = dnsHashKeyMana(domainName);

    UTT_DNS_DEBUG("\n ---------%s %d name %s key %d \n", __func__, __LINE__, domainName, hashIndex);

    for (entry = dnsHashTable[hashIndex];
            entry;
            entry = entry->next) {
        if (strcasecmp(entry->entryVal.domainName, domainName) == 0) {
            return DNS_FILTER_HASH_ERR_KEY;
        }
    }
    return hashIndex;
}

/**
 * netlink通信
 */
void uttNlDnsFilter(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    UttNlNfDnsConf  *nlData;
    nlh = nlmsg_hdr(skb);
    nlData = (UttNlNfDnsConf*)NLMSG_DATA(nlh);

    UTT_DNS_DEBUG("%s %d domainName %s priType %d secType %d \n", __func__, __LINE__,
	    nlData->entryVal.domainName, 
	    nlData->entryVal.priType,
	    nlData->entryVal.secType);/*域名*/
    switch(nlData->confType)
    {
	case UTT_NFCONF_ADD:
	    dnsHashInsert(&(nlData->entryVal));
	    break;
	case UTT_NFCONF_DEL:
	    dnsHashDelete(&(nlData->entryVal));
	    break;
	default:
	    break;

    }
    return;
}

#if DEBUG_UTT_DNS
/**
 * 遍历hash表
 * processFun : 处理函数
 */
void uttNfDnsTraverse(void (*processFun)(DnsFilterEntry* entry))
{
    int i = 0;
    DnsFilterEntry *hashBufP = NULL;

    for(i = 0; i < DNS_FILTER_HASH_TABLE_SIZE + 1; i++)
    {
	hashBufP = dnsHashTable[i];/*定位到表项头部*/
	/*遍历该key对应的链表*/
	while(hashBufP != NULL)
	{
	    processFun(hashBufP);
	    hashBufP =hashBufP->next;/*查找下一个项*/
	}
    }
    return;
}
static void uttDnsBufPrint(DnsFilterEntry *buf)
{
    static int i = 0;
    if(i%5 == 0)
    {
	printk("\n");
    }

    printk("%s %d %d | ", buf->entryVal.domainName, buf->entryVal.priType, buf->entryVal.secType);
    i++;
}
static void uttDnsPrint(void)
{
    printk("_______________________________%s start\n\n", __func__);
    uttNfDnsTraverse(uttDnsBufPrint);
    printk("_______________________________%s end\n\n", __func__);

    return;
}
static DnsFilterEntryVal* dnsHashBuildVal(char *domainName)
{
    static DnsFilterEntryVal entryVal;
    strcpy(entryVal.domainName, domainName);
    return &entryVal;
}
void UttDnsDebug(void)
{


#if 0
    dnsHashInsert(dnsHashBuildVal("www.aibdu.com"));
    dnsHashInsert(dnsHashBuildVal("www.baidu.com"));
    dnsHashInsert(dnsHashBuildVal("www.dubai.com"));
    dnsHashInsert(dnsHashBuildVal("www.iabdu.com"));
    dnsHashInsert(dnsHashBuildVal("www.sina.com"));
    dnsHashInsert(dnsHashBuildVal("www.163.com"));
    dnsHashInsert(dnsHashBuildVal("www.google.com.hk"));
   
    uttDnsBufPrint ( dnsHashLookupData("www.baidu.com"));
    uttDnsBufPrint ( dnsHashLookupData("www.aibdu.com"));
    uttDnsBufPrint ( dnsHashLookupData("www.163.com"));
    uttDnsBufPrint ( dnsHashLookupData("www.iabdu.com"));
    uttDnsPrint();

    dnsHashDelete(dnsHashBuildVal("www.baidu.com"));
    
    uttDnsPrint();

    dnsHashDelete(dnsHashBuildVal("www.aibdu.com"));

    uttDnsPrint();


    dnsHashDelete(dnsHashBuildVal("www.iabdu.com"));
    uttDnsPrint();

    dnsHashDelete(dnsHashBuildVal("www.aibdu.com"));
    dnsHashDelete(dnsHashBuildVal("www.baidu.com"));
    dnsHashDelete(dnsHashBuildVal("www.dubai.com"));
    dnsHashDelete(dnsHashBuildVal("www.iabdu.com"));
    dnsHashDelete(dnsHashBuildVal("www.sina.com"));
    dnsHashDelete(dnsHashBuildVal("www.163.com"));
    dnsHashDelete(dnsHashBuildVal("www.google.com.hk"));
   
#endif
    uttDnsPrint();
    return;
}
#endif

