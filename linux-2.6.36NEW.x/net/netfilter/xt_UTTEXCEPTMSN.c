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
 * MSN白名单hash功能模块
 */
#define DEBUG_UTT_MSN 0
#if DEBUG_UTT_MSN
#define UTT_MSN_DEBUG(fmt, argx...) printk_msn(fmt, ##argx)
#else
#define UTT_MSN_DEBUG(fmt, argx...)
#endif

#define MAX_MSN_NAME_LEN 32u

asmlinkage int printk_msn(const char *fmt, ...)
{
	va_list args;
	int r;

	va_start(args, fmt);
	r = vprintk(fmt, args);
	va_end(args);

	return r;
}

#define MSN_FILTER_HASH_TABLE_SIZE              211

typedef struct st_MsnFilterEntry {
    ExceptMsnEntryVal  entryVal;
    struct  st_MsnFilterEntry *next;
} MsnFilterEntry;

static MsnFilterEntry *msnHashTable[MSN_FILTER_HASH_TABLE_SIZE + 1];

static int msnHashPrint(void);
static int except_msn_allow(struct sk_buff *skb);
static __u32 unpack_msn(struct sk_buff *skb, struct nf_conn *conntrack);
static __u32 searchmsnNumber(struct sk_buff *skb, struct nf_conn *conntrack, __u8 *msnData);
static __u16 msnHashKey(const char *msn);
static int msnHashInsert(ExceptMsnEntryVal* newEntryVal);
static int msnHashDelAll(void);
static __u16 msnHashLookupExact(char *msn);


void uttNlMsnFilter(struct sk_buff *skb);
/*
 *
 * key生成算法
 *
 * msn: 用于做hash的字符串
 * len:
 *    length of the msn (must less than MAX_MSN_NAME_LEN).
 *
 */
static __u16 msnHashKey(const char *msn)
{
    const char *end;
    char lowerDomain[MAX_MSN_NAME_LEN + 1];
    char *curseDomain;
    __u16 key;
    __u16 i;
    __u16 len = strlen(msn);

    memset(lowerDomain, 0, sizeof(lowerDomain));
    for (i = 0u; i < len; i++) 
    {
        lowerDomain[i] = tolower(msn[i]);/*转换为小写字符*/
    }
    curseDomain = lowerDomain;

    end = curseDomain + len;
    /*hash key生成*/
    for (key = 0u; curseDomain < end; curseDomain++) 
    {
        key *= 16777619u;
        key ^= (__u16) *(char*) curseDomain;
    }

    return (key % MSN_FILTER_HASH_TABLE_SIZE);/*使key小于hash表长度*/
}

/**
 * 往MSN白名单中插入号码
 */
static int msnHashInsert(ExceptMsnEntryVal* newEntryVal)
{
    int flag = 0;
    __u16              hashIndex;
    MsnFilterEntry      *newEntry;
    char *msnNumber = (char *)newEntryVal->msnNumber;


    hashIndex = msnHashLookupExact(msnNumber);/*完全匹配查找*/
    UTT_MSN_DEBUG("%s-%d name %s key %d.\n", __FUNCTION__, __LINE__, msnNumber, hashIndex);

    if (hashIndex != 0U) 
    { 
	newEntry = (MsnFilterEntry *)kmalloc(sizeof(MsnFilterEntry), GFP_KERNEL);

	if (newEntry != NULL) 
	{
	    memcpy(&(newEntry->entryVal), newEntryVal, sizeof(MsnFilterEntry));
	    newEntry->next = msnHashTable[hashIndex];
	    msnHashTable[hashIndex] = newEntry; 
	    UTT_MSN_DEBUG("%s-%d msn %s.\n", __FUNCTION__, __LINE__, msnNumber);
	    flag = 1;
	}
	else
	{
	    UTT_MSN_DEBUG("%s-%d GETBUFF error.\n", __FUNCTION__, __LINE__);
	}
    }
    else
    {
	UTT_MSN_DEBUG("Fail: msnNumber %s already inserted! @%s-%d.\n", msnNumber, __FUNCTION__, __LINE__);
    }
    msnHashPrint();
    return flag;
}
/*
 * 删除对应的MSN项
 * */
static int msnHashDelAll(void)
{
    int i = 0;
    MsnFilterEntry **hashBufPP = NULL;/*二级指针*/
    MsnFilterEntry *hashBufP = NULL;


    /*遍历整个hash表*/
    for(i = 0; i < MSN_FILTER_HASH_TABLE_SIZE; i++)
    {
	hashBufPP = &(msnHashTable[i]);/*定位到表头*/

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
static __u16 msnHashLookupExact(char *msn)
{
    MsnFilterEntry  *entry;
    __u16	    hashIndex = 0U;

    hashIndex = msnHashKey(msn);/*hash后取得序号*/
    UTT_MSN_DEBUG("%s-%d name %s key %d.\n", __FUNCTION__, __LINE__, msn, hashIndex);

    for (entry = msnHashTable[hashIndex]; entry; entry = entry->next) 
    {
	if (strcasecmp(entry->entryVal.msnNumber, msn) == 0)/*查看是否已存在*/ 
	{
	    hashIndex = 0U;
        }
    }
    return hashIndex;
}

/**
 * netlink通信
 */
void uttNlMsnFilter(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    UttNlNfMsnConf  *nlData;
    nlh = nlmsg_hdr(skb);
    nlData = (UttNlNfMsnConf*)NLMSG_DATA(nlh);/*取值*/

    UTT_MSN_DEBUG("%s %d msn %s nlData->confType %d\n", __func__, __LINE__, nlData->entryVal.msnNumber, nlData->confType);/*MSN*/
    switch(nlData->confType)/*类别*/
    {
	case UTT_NFCONF_ADD:
	    msnHashInsert(&(nlData->entryVal));
	    break;
	case UTT_NFCONF_DEL_ALL:
	    msnHashDelAll();
	    break;
	default:/*test*/
	    break;

    }
    return;
}

/********************************************************************
 * 函数： except_msn_allow
 * 功能： 允许例外Msn数据包处理函数
 * 创建： 2012-06-18
 * 参数： skb包
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int except_msn_allow(struct sk_buff *skb)
{
    struct nf_conn *conntrack;
    /* 数据包必须有对应的连接*/
    conntrack = (struct nf_conn *)skb->nfct;
    if (conntrack != NULL) 
    {
	if ((conntrack->utt_app.priFlag == 1U) && (conntrack->utt_app.secFlag == 2U) && (conntrack->utt_app.uttIdFlag == NONE_FLAG))
	{
	    unpack_msn(skb, conntrack);
	}
    }
    return 0;
}
/********************************************************************
 * 函数： unpack_msn
 * 功能： 分析protocol类型找出msn数据
 * 创建： 2012-06-18
 * 参数： skb包
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static __u32 unpack_msn(struct sk_buff *skb, struct nf_conn *conntrack)
{
    __u32 flag = 0U;
    __u32 tcpHeadLen = 0U;
    TCP_HDR *tcp;
    __u8 *data;

    __u16 iph_len = 0U; 
    iph_len = ip_hdrlen(skb);

    tcp = (TCP_HDR *)(skb_network_header(skb) + iph_len); 
    tcpHeadLen = tcp->doff * 4U;
    data = (__u8 *)((void *)tcp + tcpHeadLen);/*tcp包前会多2位*/
    flag = searchmsnNumber(skb, conntrack, data);
    return flag;
}
/********************************************************************
 * 函数： searchmsnNumber
 * 功能： 查找hash表中是否有此例外Msn
 * 创建： 2012-06-18
 * 参数： msn数据包
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static __u32 searchmsnNumber(struct sk_buff *skb, struct nf_conn *conntrack, __u8 *msnData)
{
    __u32 flag = 0U;
    int i = 0;
    char msnID[MSN_ID_LEN + 1];
    static char msnCvr[] = "CVR";
    int len = 0;
    __u8 *tmpData = NULL;
    
    memset(msnID, 0, sizeof(msnID));
    if(strstr(msnData, msnCvr) != NULL)
    {
	tmpData = strstr(msnData + 4, "msmsgs");
	if (tmpData == NULL) {
	    tmpData = strstr(msnData + 4, "MSNMSGR");
	    if (tmpData != NULL) {/*2011版本第二个MSNMSGR后面才是账号*/
		tmpData = strstr(tmpData + 7, "MSNMSGR");
	    }
	    UTT_MSN_DEBUG("%s-%u, *tmpData = %x\n", __func__, __LINE__, *tmpData);
	    len = 1;
	}
	if (tmpData != NULL) 
	{ 
	    msnData = tmpData;
	    UTT_MSN_DEBUG("%s-%u, *msnData = %x\n", __func__, __LINE__, *msnData);
	    msnData += (7 + len);
	    UTT_MSN_DEBUG("%s-%u, *msnData = %x\n", __func__, __LINE__, *msnData);
	    for (i = 0; *msnData != 0x0d; msnData++, i++) 
	    {
		if (i >= MSN_ID_LEN)
		{
		    break;
		} 
		if (_checkData(*msnData) == 1)
		{
		    msnID[i] = *msnData;
		}
	    }
	    UTT_MSN_DEBUG("%s-%u, msnID = %s\n", __func__, __LINE__, msnID);
	    msnHashPrint();
	    flag = msnHashLookupExact(msnID);/*查例外Msn的hash表*/
	    if(flag == 0U)
	    {
		UTT_MSN_DEBUG("%s-%u\n", __func__, __LINE__);
		/*会话设置*/
		conntrack->utt_app.uttIdFlag = MSN_FLAG;
		/* 当前包设置 */
		skb->utt_app.uttIdFlag = MSN_FLAG;
	    }
	}
    }
    return flag;
}

static unsigned int
xt_uttexceptmsn_target4(struct sk_buff *skb,
		  const struct xt_target_param *par)
{
    except_msn_allow(skb);
    return XT_CONTINUE;
}

static int
xt_uttexceptmsn_checkentry4(const struct xt_tgchk_param *par)
{
        return 0;
}


static struct xt_target xt_uttexceptmsn_reg[] = {
	{
		.family		= NFPROTO_IPV4,
		.name		= "UTTEXCEPTMSN",
		.checkentry	= xt_uttexceptmsn_checkentry4,
		.target		= xt_uttexceptmsn_target4,
		.targetsize	= sizeof(int),
		.me		= THIS_MODULE,
	},
};

static int __init xt_uttexceptmsn_init(void)
{
    return xt_register_targets(xt_uttexceptmsn_reg, ARRAY_SIZE(xt_uttexceptmsn_reg));
}

static void __exit xt_uttexceptmsn_fini(void)
{
    xt_unregister_targets(xt_uttexceptmsn_reg, ARRAY_SIZE(xt_uttexceptmsn_reg));
}

module_init(xt_uttexceptmsn_init);
module_exit(xt_uttexceptmsn_fini);
#if 1
/*
 * 打印MSN项
 * */
static int msnHashPrint(void)
{
#if 1
    int i = 0;
    MsnFilterEntry *hashBufP = NULL;


    UTT_MSN_DEBUG("%s------%d\n", __func__, __LINE__);
    /*遍历整个hash表*/
    for(i = 0; i < MSN_FILTER_HASH_TABLE_SIZE; i++)
    {
	hashBufP = msnHashTable[i];/*定位到表头*/

	/*遍历冲突项*/
	while(hashBufP != NULL)
	{
	    UTT_MSN_DEBUG("i = %d msn = %s\n", i, hashBufP->entryVal.msnNumber);
	    hashBufP = hashBufP->next;/*指向当前节点的指针，指向当前节点的next*/
	}
    }
    UTT_MSN_DEBUG("%s------%d\n", __func__, __LINE__);

#endif
    return 1;
}
#endif
