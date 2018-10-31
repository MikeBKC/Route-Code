#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/tcp.h>
#include <net/ipv6.h>
#include <net/tcp.h>
#include <linux/netdevice.h>
#include <linux/if.h>
#include <linux/inetdevice.h>

#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv6/ip6_tables.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_tcpudp.h>
#include <linux/netfilter/xt_UTTLOG.h>
#include <linux/netfilter_ipv4/utt_netfilter.h>
#include <net/netfilter/nf_conntrack.h>



/*
 * Ali白名单hash功能模块
 */
//#define DEBUG_UTT_ALI 1
#if DEBUG_UTT_ALI
#define UTT_MSN_DEBUG(fmt, argx...) printk_ali(fmt, ##argx)
#else
#define UTT_MSN_DEBUG(fmt, argx...)
#endif

#define MAX_ALI_NAME_LEN 32u

asmlinkage int printk_ali(const char *fmt, ...)
{
    va_list args;
    int r;

    va_start(args, fmt);
    r = vprintk(fmt, args);
    va_end(args);

    return r;
}

#define ALI_FILTER_HASH_TABLE_SIZE              211

typedef struct st_AliFilterEntry {
    ExceptAliEntryVal  entryVal;
    struct  st_AliFilterEntry *next;
} AliFilterEntry;

static AliFilterEntry *aliHashTable[ALI_FILTER_HASH_TABLE_SIZE + 1];

static int aliHashPrint(void);
static int except_ali_allow(struct sk_buff *skb);
static __u32 unpack_ali(struct sk_buff *skb, struct nf_conn *conntrack);
static __u32 searchaliNumber(struct sk_buff *skb, struct nf_conn *conntrack, __u8 *aliData,__u32 datalen);
static __u16 aliHashKey(const char *ali);
static int aliHashInsert(ExceptAliEntryVal* newEntryVal);
static int aliHashDelAll(void);
static __u16 aliHashLookupExact(char *ali);


void uttNlAliFilter(struct sk_buff *skb);
/*
 *
 * key生成算法
 *
 * ali: 用于做hash的字符串
 * len:
 *    length of the ali (must less than MAX_ALI_NAME_LEN).
 *
 */
static __u16 aliHashKey(const char *ali)
{
    const char *end;
    char lowerDomain[MAX_ALI_NAME_LEN + 1];
    char *curseDomain;
    __u16 key;
    __u16 i;
    __u16 len = strlen(ali);

#if 0
    /*取前五个字符做hash*/
    if(len > 5){
        len = 5;
    }
#endif

    memset(lowerDomain, 0, sizeof(lowerDomain));
    for (i = 0u; i < len; i++) 
    {
        lowerDomain[i] = tolower(ali[i]);/*转换为小写字符*/
    }
    curseDomain = lowerDomain;

    end = curseDomain + len;
    /*hash key生成*/
    for (key = 0u; curseDomain < end; curseDomain++) 
    {
        key *= 16777619u;
        key ^= (__u16) *(char*) curseDomain;
    }

    return (key % ALI_FILTER_HASH_TABLE_SIZE);/*使key小于hash表长度*/
}

/**
 * 往Ali白名单中插入号码
 */
static int aliHashInsert(ExceptAliEntryVal* newEntryVal)
{
    int flag = 0;
    __u16              hashIndex;
    AliFilterEntry      *newEntry;
    char *aliNumber = (char *)newEntryVal->aliNumber;


    hashIndex = aliHashLookupExact(aliNumber);/*完全匹配查找*/
    UTT_MSN_DEBUG("%s-%d name %s key %d.\n", __FUNCTION__, __LINE__, aliNumber, hashIndex);

    if (hashIndex != 0U) 
    { 
        newEntry = (AliFilterEntry *)kmalloc(sizeof(AliFilterEntry), GFP_KERNEL);

        if (newEntry != NULL) 
        {
            memcpy(&(newEntry->entryVal), newEntryVal, sizeof(AliFilterEntry));
            newEntry->next = aliHashTable[hashIndex];
            aliHashTable[hashIndex] = newEntry; 
            UTT_MSN_DEBUG("%s-%d ali %s.\n", __FUNCTION__, __LINE__, aliNumber);
            flag = 1;
        }
        else
        {
            UTT_MSN_DEBUG("%s-%d GETBUFF error.\n", __FUNCTION__, __LINE__);
        }
    }
    else
    {
        UTT_MSN_DEBUG("Fail: aliNumber %s already inserted! @%s-%d.\n", aliNumber, __FUNCTION__, __LINE__);
    }
    //aliHashPrint();
    return flag;
}
/*
 * 删除对应的Aliww项
 * */
static int aliHashDelAll(void)
{
    int i = 0;
    AliFilterEntry **hashBufPP = NULL;/*二级指针*/
    AliFilterEntry *hashBufP = NULL;


    /*遍历整个hash表*/
    for(i = 0; i < ALI_FILTER_HASH_TABLE_SIZE; i++)
    {
        hashBufPP = &(aliHashTable[i]);/*定位到表头*/

        /*遍历冲突项*/
        while(*hashBufPP != NULL)
        {
            hashBufP = *hashBufPP;/*记录当前节点位置*/
            *hashBufPP = hashBufP->next;/*指向当前节点的指针，指向当前节点的next*/
            kfree(hashBufP);/*释放当前节点的内容*/
        }
        hashBufPP = &((*hashBufPP)->next);/*查找下一个冲突项*/
    }

    return 1;
}



/**
 * 精确匹配查找
 */
static __u16 aliHashLookupExact(char *ali)
{
    AliFilterEntry  *entry;
    __u16	    hashIndex = 0U;
    char        aliHead[5 + 1];

#if 0
    memset(aliHead,0,sizeof(aliHead));
    /*取出阿里旺旺帐号前5位*/
    memcpy(aliHead,ali,5);
#endif

    hashIndex = aliHashKey(ali);/*hash后取得序号*/

    UTT_MSN_DEBUG("\n%s-%d name :%s key :%d Head: %s .\n", __FUNCTION__, __LINE__, ali, hashIndex,aliHead);

    for (entry = aliHashTable[hashIndex]; entry; entry = entry->next) 
    {
        if (memcmp(entry->entryVal.aliNumber, ali, strlen(ali)) == 0)/*查看是否已存在*/ 
        {
            hashIndex = 0U;
        }
    }
    return hashIndex;
}

/**
 * netlink通信
 */
void uttNlAliFilter(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    UttNlNfAliConf  *nlData;
    nlh = (struct nlmsghdr *)skb->data;
    nlData = (UttNlNfAliConf*)NLMSG_DATA(nlh);/*取值*/

    UTT_MSN_DEBUG("%s %d ali %s nlData->confType %d\n", __func__, __LINE__, nlData->entryVal.aliNumber, nlData->confType);/*Aliww*/
    switch(nlData->confType)/*类别*/
    {
        case UTT_NFCONF_ADD:
            aliHashInsert(&(nlData->entryVal));
            break;
        case UTT_NFCONF_DEL_ALL:
            aliHashDelAll();
            break;
        default:/*test*/
            break;

    }
    return;
}

/********************************************************************
 * 函数： except_ali_allow
 * 功能： 允许例外Aliww数据包处理函数
 * 创建： 2012-06-18
 * 参数： skb包
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int except_ali_allow(struct sk_buff *skb)
{
    struct nf_conn *conntrack;
    /* 数据包必须有对应的连接*/
    conntrack = (struct nf_conn *)skb->nfct;
    if (conntrack != NULL) 
    {
        if ((conntrack->utt_app.priFlag == 1U) && (conntrack->utt_app.secFlag == 3U) && (conntrack->utt_app.uttIdFlag == NONE_FLAG))
        {
            unpack_ali(skb, conntrack);
        }
    }
    return 0;
}
/********************************************************************
 * 函数： unpack_ali
 * 功能： 分析protocol类型找出ali数据
 * 创建： 2012-06-18
 * 参数： skb包
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static __u32 unpack_ali(struct sk_buff *skb, struct nf_conn *conntrack)
{
    __u32 flag = 0U;
    __u32 tcpHeadLen = 0U;
    TCP_HDR *tcp;
    __u8 *data;

    __u16 iph_len = 0U; 
    iph_len = ip_hdrlen(skb);

    tcp = (TCP_HDR *)(skb_network_header(skb) + iph_len); 
    tcpHeadLen = tcp->doff * 4U;
    data = (__u8 *)((void *)tcp + tcpHeadLen);/**/

    int datalen = skb->len-iph_len-tcpHeadLen; 

    flag = searchaliNumber(skb, conntrack, data,datalen);
    return flag;
}
/********************************************************************
 * 函数： Mstrstr
 * 功能： 查找s1中是否有此s2
 * 创建： 2012-06-18
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
char *Mstrstr(const char *s1, const char *s2,int datalen)
{
    int i = 0;
    int len = strlen(s2);

    if(datalen < len)
        return NULL;

    if (*s2)
    {   
        while( i < datalen)
        {   
            if((*(s1 + i) == 'c' ) && (memcmp(s1 + i, s2,len) == 0))                                                                                                   
            {   
                return (char *)(s1 + i); 
            }   
            i++;
        }   
    }
    return NULL;
}

/********************************************************************
 * 函数： searchaliNumber
 * 功能： 查找hash表中是否有此例外Ali
 * 创建： 2012-06-18
 * 参数： ali数据包
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static __u32 searchaliNumber(struct sk_buff *skb, struct nf_conn *conntrack, __u8 *aliData,__u32 datalen)
{
    __u32 flag = 0U;
    char aliID[ALI_ID_LEN + 1];
    __u32 len = 0U;
    __u8 *temp = NULL;
    static char matchStr1[] = "POST /httpaccess";
    static char matchStr2[] = "AliNetwork";

    memset(aliID, 0, sizeof(aliID));
    if(datalen > 14) {
	/* 添加 if(){...}
	 * 当以http方式登录时，直接禁止登陆
	 * 如若想使得白名单生效，阿里旺旺应以TCP方式登录
	 * */
	if(memcmp(aliData,(__u8 *)matchStr1,16) == 0) {
	    if(strstr(aliData, matchStr2) != NULL) {
		UTT_MSN_DEBUG("%s-%d: printk_ali =%s\n",__func__,__LINE__,aliData);
		return flag;
	    }
	}

        if( (temp = Mstrstr(aliData, "cntaobao",datalen)) == NULL)
        {
            temp = Mstrstr(aliData, "cnalichn",datalen);              
        }

        UTT_MSN_DEBUG("\n%s-%u temp ; =%s=\n", __func__, __LINE__,temp);
        if(temp != NULL && strlen(temp) > 12){
	    len = ntohl(*((__u32*)(temp-4)));
	    if((len >= 13)&&(len <= 33)) {

            /*越过匹配的字符串去复制帐号信息*/
            memmove(aliID, temp + 8, len-8);

            UTT_MSN_DEBUG("\n%s-%u aliID ; =%s=, len =%u\n", __func__, __LINE__,aliID,len-8);

            flag = aliHashLookupExact(aliID);/*查例外Ali的hash表*/

            if(flag == 0U)
            {
                /*会话设置*/
                conntrack->utt_app.uttIdFlag = ALI_FLAG;
                /* 当前包设置 */
                skb->utt_app.uttIdFlag = ALI_FLAG;
            }
	    }
        }
#if 1
        else{
            skb->utt_app.uttIdFlag = ALI_FLAG;
        }
#endif
    }
    return flag;
}

    static unsigned int
xt_uttexceptali_target4(struct sk_buff *skb,
        const struct xt_target_param *par)
{
    except_ali_allow(skb);
    return XT_CONTINUE;
}

    static bool
xt_uttexceptali_checkentry4(const struct xt_tgchk_param *par)
{
    return 0;
}


static struct xt_target xt_uttexceptali_reg[] = {
    {
        .family		= NFPROTO_IPV4,
        .name		= "UTTEXCEPTALIIM",
        .checkentry	= xt_uttexceptali_checkentry4,
        .target		= xt_uttexceptali_target4,
        .targetsize	= sizeof(int),
        .me		= THIS_MODULE,
    },
};

static int __init xt_uttexceptali_init(void)
{
    return xt_register_targets(xt_uttexceptali_reg, ARRAY_SIZE(xt_uttexceptali_reg));
}

static void __exit xt_uttexceptali_fini(void)
{
    xt_unregister_targets(xt_uttexceptali_reg, ARRAY_SIZE(xt_uttexceptali_reg));
}

module_init(xt_uttexceptali_init);
module_exit(xt_uttexceptali_fini);
#if 1
/*
 * 打印MSN项
 * */
static int aliHashPrint(void)
{
#if 1
    int i = 0;
    AliFilterEntry *hashBufP = NULL;


    UTT_MSN_DEBUG("%s------%d\n", __func__, __LINE__);
    /*遍历整个hash表*/
    for(i = 0; i < ALI_FILTER_HASH_TABLE_SIZE; i++)
    {
        hashBufP = aliHashTable[i];/*定位到表头*/

        /*遍历冲突项*/
        while(hashBufP != NULL)
        {
            UTT_MSN_DEBUG("i = %d ali = %s\n", i, hashBufP->entryVal.aliNumber);
            hashBufP = hashBufP->next;/*指向当前节点的指针，指向当前节点的next*/
        }
    }
    UTT_MSN_DEBUG("%s------%d\n", __func__, __LINE__);

#endif
    return 1;
}
#endif
