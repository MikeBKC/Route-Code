#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/types.h>
#include <net/sock.h>
#include <net/netlink.h>
#include <linux/ip.h>
#include <linux/netfilter_ipv4/utt_filter_kernel.h>
#include <linux/netfilter_ipv4/ip_set.h>

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
void wifiDnsHashPrint(void);
#endif

unsigned int wifi_match_debug = 0;
extern  int wifi_match_debug_print(const char* fmt, ...)
{
#ifdef CONFIG_UTT_DEBUG
    va_list   args; 
    if (wifi_match_debug) {             
	va_start(args, fmt); 
	debug_kprint(fmt, args);			
	va_end(args);
    }                                   
#endif
    return 0;
}

#define WIFIDOG_DNS_HASH_TABLE_SIZE              211
#define WIFIDOG_DNS_FILTER_HASH_ERR_KEY                 WIFIDOG_DNS_HASH_TABLE_SIZE + 100

typedef struct st_DnsEntryNode {
    DnsEntryVal  entryVal;
    struct  st_DnsEntryNode *next;
} DnsEntryNode;

static DnsEntryNode *dnsHashTable[WIFIDOG_DNS_HASH_TABLE_SIZE + 1];
struct sk_buff *wifi_skb=NULL;

static __u16 dnsHashKey(const char *dns, __u16 len);
static bool dnsHashInsert(DnsEntryVal* newEntryVal);
static bool dnsHashDelete(struct sk_buff *skb, DnsEntryVal* oldEntryVal);
static bool  cmpTwoStrMana(char *str1,char *str2);
static __u16 dnsHashKeyMana(char *domainName);
static __u16 dnsHashLookupExact(char *domainName);

#if 0
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
#endif

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

    return (key % WIFIDOG_DNS_HASH_TABLE_SIZE);/*使key小于hash表长度*/
}

/*
 * ipEntryIpAddOrDel
 * 将域名ip添加到域名对应的ip链表中
 *	dnsEntry	- 域名节点，需要添加的域名ip节点
 *	netip		- 域名ip 需要添加的ip
 *	operate		- 操作类型 添加或者删除 目前只有添加
 *			    取值 'A' / 'D'
 * guo.deyuan
 * */
bool ipEntryIpAddOrDel(DnsEntryVal* dnsEntry, __u32 netip, char operate) {
    IpEntry *newEntry=NULL;
    bool rst = false;

    if(operate == 'A') {
	if((dnsEntry!=NULL) && (netip>0U)) {
	    newEntry = (IpEntry *)kmalloc(sizeof(IpEntry), GFP_ATOMIC);
	    if(newEntry!=NULL) {
		newEntry->ip=netip;
		newEntry->next=dnsEntry->ipEntry;
		dnsEntry->ipEntry = newEntry;
		rst = true;
	    }
	}

    }else if(operate == 'D') {
	rst = true;
    }
    return rst;
}

/*
 * ipset_ip_add_del
 * setname	- 需要添加的ipset组
 * dnsEntry	- 将ip添加到此链表中
 * netip	- 需要添加的ip(网络字节序)
 * operate	- 操作类型 添加或者删除
 *		    取值 'A' / 'D'
 *
 * guo.deyuan
 * */
bool ipset_ip_add_del(struct sk_buff *skb, const char *setname, DnsEntryVal *dnsEntry, __u32 netip, char operate) 
{
    struct iphdr *iph=NULL;
    __u32 srcip = 0U;
    uint32_t flag = IPSET_SRC;
    ip_set_id_t ipsetindex = IP_SET_INVALID_ID;

#if 1
    if(operate == 'A') {
	if((skb!=NULL)&&(setname!=NULL)&&(dnsEntry!=NULL)) {
	    iph = ip_hdr(skb);
	    if((iph!=NULL) && (netip > 0)) {
		srcip = iph->saddr;
		iph->saddr = netip;
		ipsetindex = ip_set_get_byname(setname);
		if(ipsetindex != IP_SET_INVALID_ID) {
		    if(ip_set_testip_kernel(ipsetindex, skb, &flag)==0){ /*未检测IP*/

			ip_set_addip_kernel(ipsetindex, skb, &flag); /*添加ip到ipset组*/
			ipEntryIpAddOrDel(dnsEntry, netip, 'A');
			wifi_match_debug_print("%s-%d: add domain %s, address %x, in set name %s\n", __func__,__LINE__,dnsEntry->domainName,ntohl(iph->saddr),setname);
			if(wifi_skb == NULL) {
			    wifi_skb = skb_copy(skb,GFP_ATOMIC);
			    if(wifi_skb != NULL) {
				wifi_match_debug_print("%s-%d: wifi skb back malloc success.\n",__func__,__LINE__);
			    } else {
				wifi_match_debug_print("%s-%d: wifi skb back malloc error.\n",__func__,__LINE__);
			    }
			}
		    }
		    ip_set_put_byindex(ipsetindex);
		}
		iph->saddr = srcip;
	    } else {
		wifi_match_debug_print("%s-%d: maybe skb ip head is null.\n",__func__,__LINE__);
	    }
	} else {
	    wifi_match_debug_print("%s-%d: maybe skb is null.\n",__func__,__LINE__);
	}
    } else if(operate == 'D') {
	if((wifi_skb!=NULL)&&(setname!=NULL)&&(dnsEntry!=NULL)) {
	    iph = ip_hdr(wifi_skb);
	    if((iph!=NULL) && (netip > 0)) {
		srcip = iph->saddr;
		iph->saddr = netip;
		ipsetindex = ip_set_get_byname(setname);
		if(ipsetindex != IP_SET_INVALID_ID) {
		    if(ip_set_testip_kernel(ipsetindex, wifi_skb, &flag)!=0)
		    { /*检测IP*/

			ip_set_delip_kernel(ipsetindex, wifi_skb, &flag); /*从ipset组删除ip*/
			wifi_match_debug_print("%s-%d: del domain %s, address %x, in set name %s\n", __func__,__LINE__,dnsEntry->domainName,ntohl(iph->saddr),setname);
			//ipEntryIpAddOrDel(dnsEntry, netip, 'D');
		    }
		    ip_set_put_byindex(ipsetindex);
		}
	    } else {
		wifi_match_debug_print("%s-%d: maybe skb ip head is null.\n",__func__,__LINE__);
	    }
	} else {
	    wifi_match_debug_print("%s-%d: maybe skb is null.\n",__func__,__LINE__);
	}
    }
#else 
    if((skb!=NULL)&&(setname!=NULL)&&(dnsEntry!=NULL)) {
	iph = ip_hdr(skb);
	if((iph!=NULL) && (netip > 0)) {
wifi_match_debug_print("%s-%d: setname %s\n",__func__,__LINE__,setname);
	    srcip = iph->saddr;
	    iph->saddr = netip;
	    ipsetindex = ip_set_get_byname(setname);
	    if(ipsetindex != IP_SET_INVALID_ID) {
		if(ip_set_testip_kernel(ipsetindex, skb, &flag)==0){ /*未检测IP*/
		    if(operate == 'A') {
			ip_set_addip_kernel(ipsetindex, skb, &flag); /*添加ip到ipset组*/
			ipEntryIpAddOrDel(dnsEntry, netip, 'A');
			wifi_match_debug_print("%s-%d: add domain %s, address %x, in set name %s\n", __func__,__LINE__,dnsEntry->domainName,ntohl(iph->saddr),setname);
			wifi_skb = skb_copy(skb,GFP_ATOMIC);
			if(wifi_skb != NULL) {
			    wifi_match_debug_print("%s-%d: wifi skb back malloc success.\n",__func__,__LINE__);
			} else {
			    wifi_match_debug_print("%s-%d: wifi skb back malloc error.\n",__func__,__LINE__);
			}
		    }
		} else {
		    if(operate == 'D') {
			ip_set_delip_kernel(ipsetindex, skb, &flag); /*从ipset组删除ip*/
			wifi_match_debug_print("%s-%d: del domain %s, address %x, in set name %s\n", __func__,__LINE__,dnsEntry->domainName,ntohl(iph->saddr),setname);
			//ipEntryIpAddOrDel(dnsEntry, netip, 'D');
		    }
		}
		ip_set_put_byindex(ipsetindex);
	    }
	    iph->saddr = srcip;
	} else {
	    wifi_match_debug_print("%s-%d: maybe skb ip head is null.\n",__func__,__LINE__);
	}
    } else {
	wifi_match_debug_print("%s-%d: maybe skb is null.\n",__func__,__LINE__);
    }
#endif
#if DEBUG_UTT_DNS
    wifiDnsHashPrint();
#endif
    return true;
}

/*
 * ipEntryDestory
 * 删除域名时删除域名对应的ip
 *	dnsEntry	- 域名节点，需要删除域名节点
 *
 * guo.deyuan
 * */
static bool ipEntryDestory(struct sk_buff *skb, DnsEntryVal *dnsEntry) {
    IpEntry *Del=NULL;
    IpEntry *tmp=NULL;

    if(dnsEntry != NULL) {
	for(tmp = dnsEntry->ipEntry;tmp;) {
	    Del=tmp;
	    tmp=tmp->next;
	    ipset_ip_add_del(skb, WHITE_LIST, dnsEntry, Del->ip, 'D');
	    kfree(Del);
	}
    }
    return true;
}
/**
 * 往hash表中插入域名
 */
static bool dnsHashInsert(DnsEntryVal* newEntryVal)
{
    __u16              hashIndex;
    DnsEntryNode      *newEntry;
    char *domainName  = newEntryVal->domainName;

#if 0
    if (domainName == NULL || strlen(domainName) == 0) {
	return false;
    }
#endif

    hashIndex = dnsHashLookupExact(domainName);/*完全匹配查找*/

    if (hashIndex == WIFIDOG_DNS_FILTER_HASH_ERR_KEY) {
	wifi_match_debug_print("Fail: domainName %s already inserted! @%s-%d.\n",
		domainName, __FUNCTION__, __LINE__);
	return false;
    }


    newEntry = (DnsEntryNode *)kmalloc(sizeof(DnsEntryNode), GFP_ATOMIC);

    if (newEntry == NULL) {
	wifi_match_debug_print("%s-%d GETBUFF error.\n", __FUNCTION__, __LINE__);
	return false;
    }
    memcpy(&(newEntry->entryVal), newEntryVal, sizeof(DnsEntryNode));
    newEntry->next = dnsHashTable[hashIndex];
    dnsHashTable[hashIndex] = newEntry;
    newEntry->entryVal.ipEntry = NULL;


    wifi_match_debug_print("%s-%d: add name: %s, hash key: %d \n", __func__, __LINE__, domainName, hashIndex);

    return true;
}


/*
 * dnsHashDelete
 * 删除对应的域名项
 *
 * */
static bool dnsHashDelete(struct sk_buff *skb, DnsEntryVal* oldEntryVal)
{
    __u16              hashIndex;
    DnsEntryNode      *toDel;
    DnsEntryNode      *prevDel;


    char *domainName  = oldEntryVal->domainName;

    prevDel = NULL;
    if(domainName != NULL) {
	hashIndex = dnsHashKeyMana(domainName);

	wifi_match_debug_print("%s-%d key %d.\n", __FUNCTION__, __LINE__, hashIndex);

	for (toDel = dnsHashTable[hashIndex];
		toDel;
		prevDel = toDel, toDel = toDel->next) {
	    if (strcmp(domainName, toDel->entryVal.domainName) == 0) {
		if (toDel == dnsHashTable[hashIndex]) {
		    dnsHashTable[hashIndex] = toDel->next;
		    wifi_match_debug_print("%s-%d domain %s.\n", __FUNCTION__, __LINE__, domainName);
		    ipEntryDestory(skb, &(toDel->entryVal));
		    kfree(toDel);
		    return true;
		}
		else {
		    prevDel->next = toDel->next;
		    wifi_match_debug_print("%s-%d domain %s.\n", __FUNCTION__, __LINE__, domainName);
		    ipEntryDestory(skb, &(toDel->entryVal));
		    kfree(toDel);
		    return true;
		}
	    }
	}
    }
    return false;
}

/*
 * dnsHashDeleteAll
 * 删除所有的域名项
 * 包括域名对应的ip链表也要释放
 *
 * */
static bool dnsHashDeleteAll(struct sk_buff *skb) {
    __u16              hashIndex;
    DnsEntryNode      *toDel;


    wifi_match_debug_print("%s-%d key %d.\n", __FUNCTION__, __LINE__, hashIndex);
    for(hashIndex=0U;hashIndex<=WIFIDOG_DNS_HASH_TABLE_SIZE;hashIndex++) {
	for (toDel = dnsHashTable[hashIndex]; toDel; ) {
#if 1
	    wifi_match_debug_print("%s-%d domain %s.\n", __FUNCTION__, __LINE__, toDel->entryVal.domainName);
	    dnsHashTable[hashIndex] = toDel->next;
	    ipEntryDestory(skb, &(toDel->entryVal));
	    kfree(toDel);
	    toDel = dnsHashTable[hashIndex];
#endif
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
DnsEntryVal *wifidog_dnsHashLookupData(char *domainName)
{
    DnsEntryNode  *entry;
    __u16          hashIndex;
    int             domainNameLen;

    domainNameLen = strlen(domainName);
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
    for (entry = dnsHashTable[WIFIDOG_DNS_HASH_TABLE_SIZE];
	    entry;
	    entry = entry->next) {

	if (cmpTwoStrMana(entry->entryVal.domainName, domainName) == 1) {
	    return &(entry->entryVal);
	}
    }
    wifi_match_debug_print("%s-%d can't find name:%s.\n", __FUNCTION__, __LINE__, domainName);
    return NULL;
}


/*
 * 函 数 名: dnsHashKeyMana
 * 功能描述：管理平面的hash函数, 如果含有通配符，则直接返回
 *           WIFIDOG_DNS_HASH_TABLE_SIZE;
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
        return WIFIDOG_DNS_HASH_TABLE_SIZE;
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
    DnsEntryNode  *entry;

    __u16          hashIndex;

    hashIndex = dnsHashKeyMana(domainName);

    wifi_match_debug_print("%s-%d: name: %s, hash key: %d \n", __func__, __LINE__, domainName, hashIndex);

    for (entry = dnsHashTable[hashIndex];
            entry;
            entry = entry->next) {
        if (strcasecmp(entry->entryVal.domainName, domainName) == 0) {
            return WIFIDOG_DNS_FILTER_HASH_ERR_KEY;
        }
    }
    return hashIndex;
}

/**
 * netlink通信
 * wifidog 配置接口函数(utt_filter_netlink.c中调用)
 * wifidog 所有配置都使用此接口实现
 * guo.deyuan
 */
void uttNlwifidogOperation(struct sk_buff *skb, UttNlNfType type)
{
    struct nlmsghdr *nlh;
    void *nlData;
    nlh = nlmsg_hdr(skb);
    nlData = NLMSG_DATA(nlh);
#if 0
    if(wifi_skb == NULL) {
	wifi_skb = skb_copy(skb,GFP_ATOMIC);
    }
#endif
    if(type == UTT_NLNF_WIFIDOG_DNS_WHITE) {
	switch(((UttNlNfwifiDnsConf*)nlData)->confType)
	{
	    case UTT_NFCONF_ADD:
		dnsHashInsert(&(((UttNlNfwifiDnsConf*)nlData)->entryVal));
		break;
	    case UTT_NFCONF_DEL:
		dnsHashDelete(skb,&(((UttNlNfwifiDnsConf*)nlData)->entryVal));
		break;
	    case UTT_NFCONF_DEL_ALL: 
		dnsHashDeleteAll(skb);
		break;
	    default:
		break;

	}
#if DEBUG_UTT_DNS
	wifiDnsHashPrint();
#endif
    }
}

#if DEBUG_UTT_DNS
/**
 * 遍历hash表
 */
void wifiDnsHashPrint(void)
{
    int i = 0;
    DnsEntryNode *hashBufP = NULL;
    IpEntry *ipEntry =NULL;
    struct in_addr ina;
    unsigned char *ucp = NULL;

    wifi_match_debug_print("%s: wifidog dns hash table print start.\n\n", __func__);
    for(i = 0; i < WIFIDOG_DNS_HASH_TABLE_SIZE + 1; i++)
    {
	hashBufP = dnsHashTable[i];/*定位到表项头部*/
	/*遍历该key对应的链表*/
	while(hashBufP != NULL)
	{

	    wifi_match_debug_print("hash key: %d, %s, ip list: \n", i, hashBufP->entryVal.domainName);
	    for(ipEntry = hashBufP->entryVal.ipEntry;ipEntry;ipEntry = ipEntry->next) {
		ina.s_addr = ipEntry->ip;
		ucp = (unsigned char *)&ina;
		wifi_match_debug_print("%d.%d.%d.%d ", ucp[0] & 0xff, ucp[1] & 0xff, ucp[2] & 0xff, ucp[3] & 0xff);
	    }
	    wifi_match_debug_print("\n");
	    hashBufP =hashBufP->next;/*查找下一个项*/
	}
    }
    wifi_match_debug_print("%s: wifidog dns hash table print end.\n\n", __func__);
    return;
}
static DnsFilterEntryVal* dnsHashBuildVal(char *domainName)
{
    static DnsEntryVal entryVal;
    strcpy(entryVal.domainName, domainName);
    return &entryVal;
}
void dnsHash_debug_init(void) {
    dnsHashInsert(dnsHashBuildVal("*.baidu.com"));
    dnsHashInsert(dnsHashBuildVal("*.163.com"));
}

#endif

