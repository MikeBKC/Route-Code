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
 * QQ白名单hash功能模块
 */
#define DEBUG_UTT_QQ 0
#if DEBUG_UTT_QQ
#define UTT_QQ_DEBUG(fmt, argx...) printk_qq(fmt, ##argx)
#else
#define UTT_QQ_DEBUG(fmt, argx...)
#endif

#define ENTERPRISE_QQ 0x2b1e
#define ENTERPRISE_QQ2 0x2b24
#define ENTERPRISE_QQ3 0x2b2a
#define ENTERPRISE_QQ4 0x2b30
#define ENTERPRISE_QQ5 0x3112
#define UTT_NORMAL_QQ_ENABLE   (1 << 0)
#define UTT_ENTERPRISE_QQ_ENABLE   (1 << 1)
#define MAX_QQ_NO_LEN 11u
asmlinkage int printk_qq(const char *fmt, ...)
{
	va_list args;
	int r;

	va_start(args, fmt);
	r = vprintk(fmt, args);
	va_end(args);

	return r;
}

#define QQ_FILTER_HASH_TABLE_SIZE              211
#define ENTERPRISE_QQ_FILTER_LINK_TABLE_SIZE   10

typedef struct st_QQFilterEntry {
    ExceptQQEntryVal  entryVal;
    struct  st_QQFilterEntry *next;
} QQFilterEntry;

static QQFilterEntry *qqHashTable[QQ_FILTER_HASH_TABLE_SIZE + 1];
static enterpriseQQEntryVal *enterpriseQQLink_Head = NULL;

static int qqHashPrint(void);
static int except_qq_allow(struct sk_buff *skb, struct xt_uttlog_info *info);
static __u32 unpack_qq(struct sk_buff *skb, struct nf_conn *conntrack, struct xt_uttlog_info *info);
static __u32 searchqqNumber(struct sk_buff *skb, struct nf_conn *conntrack, QQ_HDR *qqData);
static __u32 EnterpriseQQ(struct sk_buff *skb, struct nf_conn *conntrack, QQ_HDR *qqData);
static __u16 qqHashKey(const char *qq);
static int qqHashInsert(ExceptQQEntryVal* newEntryVal);
static int qqHashDelAll(void);
static int uttNlexceptEnterpriseQQAdd(enterpriseQQEntryVal *entery);
static void uttNlexceptEnterpriseQQDelAll(enterpriseQQEntryVal *entery);
static __u16 qqHashLookupExact(char *qq);
static int enterpriseQQLinkPrint(void);


void uttNlQQFilter(struct sk_buff *skb);
/*
 *
 * key生成算法
 *
 * qq: 用于做hash的字符串
 * len:
 *    length of the qq (must less than MAX_QQ_NO_LEN).
 *
 */
static __u16 qqHashKey(const char *qq)
{
    const char *end;
    char lowerDomain[MAX_QQ_NO_LEN + 1];
    char *curseDomain;
    __u16 key;
    __u16 i;
    __u16 len = strlen(qq);

    memset(lowerDomain, 0, sizeof(lowerDomain));
    for (i = 0u; i < len; i++) 
    {
        lowerDomain[i] = tolower(qq[i]);/*转换为小写字符*/
    }
    curseDomain = lowerDomain;

    end = curseDomain + len;
    /*hash key生成*/
    for (key = 0u; curseDomain < end; curseDomain++) 
    {
        key *= 16777619u;
        key ^= (__u16) *(char*) curseDomain;
    }

    return (key % QQ_FILTER_HASH_TABLE_SIZE);/*使key小于hash表长度*/
}

/**
 * 往QQ白名单中插入号码
 */
static int qqHashInsert(ExceptQQEntryVal* newEntryVal)
{
    int flag = 0;
    __u16              hashIndex;
    QQFilterEntry      *newEntry;
    char *qqNumber = (char *)newEntryVal->qqNumber;


    hashIndex = qqHashLookupExact(qqNumber);/*完全匹配查找*/
    UTT_QQ_DEBUG("%s-%d name %s key %d.\n", __FUNCTION__, __LINE__, qqNumber, hashIndex);

    if (hashIndex != 0U) 
    { 
	newEntry = (QQFilterEntry *)kmalloc(sizeof(QQFilterEntry), GFP_KERNEL);

	if (newEntry != NULL) 
	{
	    memcpy(&(newEntry->entryVal), newEntryVal, sizeof(QQFilterEntry));
	    newEntry->next = qqHashTable[hashIndex];
	    qqHashTable[hashIndex] = newEntry; 
	    UTT_QQ_DEBUG("%s-%d qq %s.\n", __FUNCTION__, __LINE__, qqNumber);
	    flag = 1;
	}
	else
	{
	    UTT_QQ_DEBUG("%s-%d GETBUFF error.\n", __FUNCTION__, __LINE__);
	}
    }
    else
    {
	UTT_QQ_DEBUG("Fail: qqNumber %s already inserted! @%s-%d.\n", qqNumber, __FUNCTION__, __LINE__);
    }
    qqHashPrint();
    return flag;
}
/*
 * 删除对应的QQ项
 * */
static int qqHashDelAll(void)
{
    int i = 0;
    QQFilterEntry **hashBufPP = NULL;/*二级指针*/
    QQFilterEntry *hashBufP = NULL;


    /*遍历整个hash表*/
    for(i = 0; i < QQ_FILTER_HASH_TABLE_SIZE; i++)
    {
	hashBufPP = &(qqHashTable[i]);/*定位到表头*/

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
static int uttNlexceptEnterpriseQQAdd(enterpriseQQEntryVal *entery)
{
    /*保存企业QQ策略,保存为链表*/
    int flag = 0;
    enterpriseQQEntryVal      *newEntry;
    enterpriseQQEntryVal      *now = enterpriseQQLink_Head ;

    enterpriseQQLinkPrint();
    UTT_QQ_DEBUG("\n%s-%d qqValue : %x\n", __FUNCTION__, __LINE__,entery->qqValue);

    while(now != NULL)
    {
        if(now->qqValue == entery->qqValue)   
           return flag; 
        now = now->next;
    }

    UTT_QQ_DEBUG("%s-%d \n", __FUNCTION__, __LINE__);
    newEntry = (enterpriseQQEntryVal *)kmalloc(sizeof(enterpriseQQEntryVal), GFP_KERNEL);
    if (newEntry != NULL) 
    {
        newEntry->qqValue = entery->qqValue;
        newEntry->next    = enterpriseQQLink_Head;
        enterpriseQQLink_Head = newEntry;
        UTT_QQ_DEBUG("%s-%d qq %x.\n", __FUNCTION__, __LINE__, newEntry->qqValue);
        flag = 1;
    }else{
        UTT_QQ_DEBUG("%s-%d GETBUFF error.\n", __FUNCTION__, __LINE__);
    }
    UTT_QQ_DEBUG("%s-%d \n", __FUNCTION__, __LINE__);
    enterpriseQQLinkPrint();
    return flag;
}
static void uttNlexceptEnterpriseQQDelAll(enterpriseQQEntryVal *entery)
{
   /*保存企业QQ策略,保存为链表*/
    enterpriseQQEntryVal *now     = enterpriseQQLink_Head;
    int i = 0;

    while( enterpriseQQLink_Head != NULL)
    {
        UTT_QQ_DEBUG("%s------%d EnterpriseQQValue[%d] : =%x=\n", __func__, __LINE__, i++, now->qqValue);
        now = enterpriseQQLink_Head;
        enterpriseQQLink_Head = enterpriseQQLink_Head->next; 
        kfree(now);
    }
    return 1;
}

/********************************************************************
* 函数： qqHashLookupExact
* 功能： 精确匹配查找
* 创建： 2012-06-19
* 参数： 略
* 返回： 略
* 输出： 略
* 作者： Jianqing.Sun 
********************************************************************/
static __u16 qqHashLookupExact(char *qq)
{
    QQFilterEntry  *entry;
    __u16	    hashIndex = 0U;

    hashIndex = qqHashKey(qq);/*hash后取得序号*/
    UTT_QQ_DEBUG("%s-%d name %s key %d.\n", __FUNCTION__, __LINE__, qq, hashIndex);

    for (entry = qqHashTable[hashIndex]; entry; entry = entry->next) 
    {
	if (strcasecmp((char *)entry->entryVal.qqNumber, qq) == 0)/*查看是否已存在*/ 
	{
	    hashIndex = 0U;
        }
    }
    return hashIndex;
}

/**
 * netlink通信
 */
void uttNlQQFilter(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    UttNlNfQQConf  *nlData;
    nlh = nlmsg_hdr(skb);
    nlData = (UttNlNfQQConf*)NLMSG_DATA(nlh);/*取值*/

    UTT_QQ_DEBUG("%s %d qq %s nlData->confType %d\n", __func__, __LINE__, nlData->entryVal.qqNumber, nlData->confType);/*QQ*/
    switch(nlData->confType)/*类别*/
    {
	case UTT_NFCONF_ADD:
	    qqHashInsert(&(nlData->entryVal));
	    break;
	case UTT_NFCONF_DEL_ALL:
	    qqHashDelAll();
	    break;
	default:/*test*/
	    break;

    }
    return;
}
/**
 * netlink通信
 */
void uttNlEnterpriseQQFilter(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    UttNlNfEnterpriseQQConf  *nlData;
    nlh = nlmsg_hdr(skb);
    nlData = (UttNlNfEnterpriseQQConf*)NLMSG_DATA(nlh);/*取值*/

    UTT_QQ_DEBUG("%s %d qqValue : %x nlData->confType %d\n", __func__, __LINE__, nlData->entryVal.qqValue, nlData->confType);/*QQ*/
    switch(nlData->confType)/*类别*/
    {
	case UTT_NFCONF_ADD:
	    uttNlexceptEnterpriseQQAdd(&(nlData->entryVal));
	    break;
	case UTT_NFCONF_DEL_ALL:
	    uttNlexceptEnterpriseQQDelAll(&(nlData->entryVal));
	    break;
	default:/*test*/
	    break;

    }
    return;
}

/********************************************************************
 * 函数： except_qq_allow
 * 功能： 允许例外QQ数据包处理函数
 * 创建： 2012-06-18
 * 参数： skb包
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int except_qq_allow(struct sk_buff *skb, struct xt_uttlog_info *info)
{
    struct nf_conn *conntrack;
    /* 数据包必须有对应的连接*/
    conntrack = (struct nf_conn *)skb->nfct;
    if (conntrack != NULL) 
    {
	if ((conntrack->utt_app.priFlag == 1U) && (conntrack->utt_app.secFlag == 1U) && (conntrack->utt_app.uttIdFlag == NONE_FLAG))
	{
	    unpack_qq(skb, conntrack, info);
	}
    }
    return 0;
}
/********************************************************************
 * 函数： unpack_qq
 * 功能： 分析protocol类型找出qq数据
 * 创建： 2012-06-18
 * 参数： skb包
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static __u32 unpack_qq(struct sk_buff *skb, struct nf_conn *conntrack, struct xt_uttlog_info *info)
{
    __u32 flag = 0U;
    __u32 tcpHeadLen = 0U;
    TCP_HDR *tcp;
    UDP_HDR *udp;
    IP_HDR *iph = ip_hdr(skb);
    QQ_HDR *data;

    __u16 iph_len = 0U; 
    iph_len = ip_hdrlen(skb);

    switch (iph->protocol) 
    {
	case PROT_TCP:/*以tcp登录的QQ*/
	    tcp = (TCP_HDR *)(skb_network_header(skb) + iph_len); 
	    tcpHeadLen = tcp->doff * 4U;
	    data = (QQ_HDR *)((void *)tcp + tcpHeadLen + 2U);/*tcp包前会多2位*/
	    UTT_QQ_DEBUG("%s------%d, info->sw = %d\n", __func__, __LINE__, info->sw);
	    if((flag == 0U) && ((info->sw & UTT_ENTERPRISE_QQ_ENABLE) != 0))
	    {
		flag = EnterpriseQQ(skb, conntrack, data);
	    }
	    if((flag == 0U) && ((info->sw & UTT_NORMAL_QQ_ENABLE) != 0))
	    {
		flag = searchqqNumber(skb, conntrack, data);
	    }
	    break;

	case PROT_UDP:/*以udp登录的QQ*/
	    udp = (UDP_HDR *)(skb_network_header(skb) + iph_len);
	    data = (QQ_HDR *)((void *)udp + sizeof(UDP_HDR));
	    UTT_QQ_DEBUG("%s------%d, info->sw = %d\n", __func__, __LINE__, info->sw);
	    if((flag == 0U) && ((info->sw & UTT_ENTERPRISE_QQ_ENABLE) != 0))
	    {
		flag = EnterpriseQQ(skb, conntrack, data);
	    }
	    if((flag == 0U) && ((info->sw & UTT_NORMAL_QQ_ENABLE) != 0))
	    {
		flag = searchqqNumber(skb, conntrack, data);
	    }
	    break;

	default:/*test*/
	    break;
    }
    return flag;
}
/********************************************************************
 * 函数： searchqqNumber
 * 功能： 查找hash表中是否有此例外QQ
 * 创建： 2012-06-18
 * 参数： qq数据包
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static __u32 searchqqNumber(struct sk_buff *skb, struct nf_conn *conntrack, QQ_HDR *qqData)
{
    __u32 flag = 0U;
    char QQNumber[20 + 1];
    UTT_QQ_DEBUG("%s------%d\n", __func__, __LINE__);
    memset(QQNumber, 0, sizeof(QQNumber));
    if(qqData->qq_number == 0U)/*11位QQ会发号码为0的包*/
    {
	/* 当前包设置 */
	skb->utt_app.uttIdFlag = QQ_FLAG;
    }
    else
    {
	sprintf(QQNumber, "%u", ntohl(qqData->qq_number));
	UTT_QQ_DEBUG("%s------%d, QQNumber = %s\n", __func__, __LINE__, QQNumber);
	flag = qqHashLookupExact(QQNumber);/*查例外QQhash表*/
	if(flag == 0U)
	{
	    UTT_QQ_DEBUG("%s------%d\n", __func__, __LINE__);
	    /*会话设置*/
	    conntrack->utt_app.uttIdFlag = QQ_FLAG;
	    /* 当前包设置 */
	    skb->utt_app.uttIdFlag = QQ_FLAG;
	}
    }
    return flag;
}
/********************************************************************
 * 函数： EnterpriseQQ
 * 功能： 查找hash表中是否有此例外QQ
 * 创建： 2012-06-18
 * 参数： qq数据包
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static __u32 EnterpriseQQ(struct sk_buff *skb, struct nf_conn *conntrack, QQ_HDR *qqData)
{
    __u32 flag = 0U;
    __u16 qVersion = ntohs(qqData->version);
    UTT_QQ_DEBUG("%s------%d\n", __func__, __LINE__);
#if 0
    if( (qVersion== ENTERPRISE_QQ) || (qVersion == ENTERPRISE_QQ2) || (qVersion == ENTERPRISE_QQ3)
	    || (qVersion == ENTERPRISE_QQ4) || (qVersion == ENTERPRISE_QQ5) )/*企业QQ*/
    {
	flag = 1U;
	/*会话设置*/
	conntrack->utt_app.uttIdFlag = QQ_FLAG;
	/* 当前包设置 */
	skb->utt_app.uttIdFlag = QQ_FLAG;
    }
#else
    enterpriseQQEntryVal *now = enterpriseQQLink_Head;
    int i = 0;

    while( now != NULL)
    {
        UTT_QQ_DEBUG("%s------%d EnterpriseQQValue[%d] : =%x=, qVersion :=%x= \n", __func__, __LINE__, i++, now->qqValue, qVersion);
        if(qVersion == now->qqValue)
         {
             flag = 1U;

             /*会话设置*/
             conntrack->utt_app.uttIdFlag = QQ_FLAG;
             /* 当前包设置 */
             skb->utt_app.uttIdFlag = QQ_FLAG;

             break;
         }
        now = now->next; 
    }
#endif
    return flag;
}

static unsigned int
xt_uttexceptqq_target4(struct sk_buff *skb,
		  const struct xt_target_param *par)
{
    struct xt_uttlog_info *info = (struct xt_uttlog_info *)par->targinfo;
    except_qq_allow(skb, info);
    return XT_CONTINUE;
}

static int
xt_uttexceptqq_checkentry4(const struct xt_tgchk_param *par)
{
        return 0;
}


static struct xt_target xt_uttexceptqq_reg[] = {
	{
		.family		= NFPROTO_IPV4,
		.name		= "UTTEXCEPTQQ",
		.checkentry	= xt_uttexceptqq_checkentry4,
		.target		= xt_uttexceptqq_target4,
		.targetsize	= sizeof(int),
		.me		= THIS_MODULE,
	},
};

static int __init xt_uttexceptqq_init(void)
{
    return xt_register_targets(xt_uttexceptqq_reg, ARRAY_SIZE(xt_uttexceptqq_reg));
}

static void __exit xt_uttexceptqq_fini(void)
{
    xt_unregister_targets(xt_uttexceptqq_reg, ARRAY_SIZE(xt_uttexceptqq_reg));
}

module_init(xt_uttexceptqq_init);
module_exit(xt_uttexceptqq_fini);
/*
 * 打印QQ项
 * */
static int qqHashPrint(void)
{
#if 1
    int i = 0;
    QQFilterEntry *hashBufP = NULL;


    UTT_QQ_DEBUG("%s------%d\n", __func__, __LINE__);
    /*遍历整个hash表*/
    for(i = 0; i < QQ_FILTER_HASH_TABLE_SIZE; i++)
    {
	hashBufP = qqHashTable[i];/*定位到表头*/

	/*遍历冲突项*/
	while(hashBufP != NULL)
	{
	    UTT_QQ_DEBUG("i = %d qq = %s\n", i, hashBufP->entryVal.qqNumber);
	    hashBufP = hashBufP->next;/*指向当前节点的指针，指向当前节点的next*/
	}
    }
    UTT_QQ_DEBUG("%s------%d\n", __func__, __LINE__);

#endif
    return 1;
}
/*
 * 打印企业QQ项
 * */
static int enterpriseQQLinkPrint(void)
{
    enterpriseQQEntryVal *now = enterpriseQQLink_Head;
    int i = 0;

    while( now != NULL)
    {
        UTT_QQ_DEBUG("%s------%d EnterpriseQQValue[%d] : =%x=\n", __func__, __LINE__, i++, now->qqValue);
        now = now->next; 
    }
    return 1;
}
