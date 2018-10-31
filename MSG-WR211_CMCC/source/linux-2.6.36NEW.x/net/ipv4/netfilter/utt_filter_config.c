#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/ktime.h>
#include <linux/skbuff.h>
#include <linux/types.h>
#include <net/sock.h>
#include <linux/autoconf.h>
#include <linux/ip.h>
#include <net/netlink.h>
#include <linux/netfilter_ipv4/utt_filter_kernel.h>

#define DEBUG_UTT_POLICY 0
#if DEBUG_UTT_POLICY
#define UTT_POLICY_DEBUG(fmt, argx...) printk(fmt, ##argx)
#else
#define UTT_POLICY_DEBUG(fmt, argx...)
#endif


static u_int uttNfUserPid = 0;
static u_int uttNfWifidogPid = 0;
static int uttNlPolicyAddRequest(u_long userIp);
static bool uttNfPolicyListMatch(UttNfCurTime *curTime, UttNfPolicyList *list, u_int appFlag);
static void uttNfPolicyDelAll(void);


extern bool uttNfPolicyHashMatch(struct sk_buff* skb);
extern void uttNlPolicyConf(struct sk_buff *skb);
#if DEBUG_UTT_POLICY
static void print_members(unsigned char *member, int  size)
{
    int i;

    for(i=0;i<size;i++)
    {
        printk("%02x", member[i]);
    }
    return;
}
static void uttPolicyListPrint(UttNfPolicyList *list)
{
    while(list != NULL) 
    {
	printk("\n");
	print_members(list->value.nfApps.appFlags, sizeof(UttNfApps));
	printk(" day:%u", list->value.nfTime.wday);
	printk(" timestart:%u", list->value.nfTime.timestart);
	printk(" timestop:%u", list->value.nfTime.timestop);
	printk("\n");

	list = list->next;/*指向下一个*/
    }
    return;
}
static void uttHashBufPolicyPrint(UttNfHashBuf *buf)
{
    printk("ip %pI4:status %u \n", &(buf->ip), buf->status);
    uttPolicyListPrint(buf->conf.policyConf.policyHead);
    printk("\n");
}
static void uttHashPolicyPrint(void)
{
    printk("\n_______________________________%s start\n\n", __func__);
    uttNfHashTraverse(uttHashBufPolicyPrint);
    printk("\n_______________________________%s end\n\n", __func__);

    return;
}

#if 0
static struct timer_list nfDebugTimer;
void UttPolicyDebugDelay(u_long param)
{
    if(param == 0u)
    {
	uttNfPolicyHashMatch(0x00001000u, 1);
	uttNfPolicyHashMatch(0x10001000u, 1);
	uttNfPolicyHashMatch(0x20001000u, 1);
	uttHashPolicyPrint();
	uttNfPolicyHashMatch(0x20001000u, 1);
	nfDebugTimer.expires = jiffies + 30*HZ;   /* 定时器超时时的节拍数*/
	nfDebugTimer.data     = 1;                /* 给定时器处理函数传入参数0*/
	nfDebugTimer.function = UttPolicyDebugDelay;      /* 定时器超时时调用的函数*/
	add_timer(&nfDebugTimer);/*激活定时器*/

    }
    else
    {
	uttNfPolicyHashMatch(0x00001000u, 1);
	uttNfPolicyHashMatch(0x00001000u, 2);
	uttNfPolicyHashMatch(0x00001000u, 9);
	uttHashPolicyPrint();
	nfDebugTimer.expires = jiffies + 10*HZ;   /* 定时器超时时的节拍数*/
	nfDebugTimer.data     = 1;                /* 给定时器处理函数传入参数0*/
	nfDebugTimer.function = UttPolicyDebugDelay;      /* 定时器超时时调用的函数*/
	add_timer(&nfDebugTimer);/*激活定时器*/


    }

    return;
}
void UttPolicyDebug(void)
{

    init_timer(&nfDebugTimer);
    nfDebugTimer.expires = jiffies + 30*HZ;   /* 定时器超时时的节拍数*/
    nfDebugTimer.data     = 0;                /* 给定时器处理函数传入参数0*/
    nfDebugTimer.function = UttPolicyDebugDelay;      /* 定时器超时时调用的函数*/
    add_timer(&nfDebugTimer);/*激活定时器*/
    return;
}
#endif
#endif

/**
 * 从hash表中进行策略匹配
 * ip :对应的 skb ip 
 * appFlag  包被识别时添加的标签
 *
 * return ：true 策略中有该ip对应的标签
 *          false 策略中无该ip对应的标签
 */
bool uttNfPolicyHashMatch(struct sk_buff *skb)
{
    UttNfHashConf* hashConf = NULL;
    struct iphdr *ipH = NULL;
    UttNfHashPolicy *hashConfPolicy = NULL;
    UttNfCurTime pakTime;
    bool ret = false;

    if(((skb->utt_app.secFlag) > 0)
	    && ((skb->utt_app.secFlag) <= UTT_MAX_KPDB_APPFLAGS))
    {
	if(isSkbComeFromLan(skb))
	{/*来自lan口的包才进行匹配*/
	    UTT_POLICY_DEBUG("%s %d %u\n", __func__, __LINE__, skb->utt_app.secFlag);
	    ipH = ip_hdr(skb);
	    /*这里hashConf的读取也可以放到公用函数中。不用每次都hash*/
	    hashConf = uttNfHashConfGet(ipH->saddr, 1);/*从hash表读取配置*/
	    if(hashConf != NULL)
	    {
		hashConfPolicy = &(hashConf->policyConf);
		if(hashConfPolicy->policyHead != NULL)/*规则链表存在*/
		{/*注意若只有规则链条全部构造完毕才能进行匹配。则需在此判断*/
		    /*从规则链条中进行匹配*/
		    UTT_POLICY_DEBUG("%s %d %pI4 match from list\n", __func__, __LINE__, &(ipH->saddr));
		    uttNfPolicyGetTime(skb, &pakTime);
		    ret = uttNfPolicyListMatch(&pakTime, hashConfPolicy->policyHead, skb->utt_app.secFlag);
		}
		else
		{
		    if(hashConfPolicy->buildStatus == UTT_NFNL_DONE_NONE)
		    {/*未进行过规则建立请求*/
			if(uttNlPolicyAddRequest(ipH->saddr) >= 0)/*给用户程序发送消息。建立规则*/
			{/*发送成功*/
			    /*该ip对应规则改为接受中。即在规则建立前。无需重发请求*/
			    hashConfPolicy->buildStatus |= UTT_NFNL_RCV_ING;
			}
#if DEBUG_UTT_POLICY
			else
			{
			    UTT_POLICY_DEBUG("%s %d %pI4 request erro\n", __func__, __LINE__, &(ipH->saddr));
			}
#endif
		    }
#if DEBUG_UTT_POLICY
		    else
		    {
			UTT_POLICY_DEBUG("%s %d %pI4 in buiding\n", __func__, __LINE__, &(ipH->saddr));
		    }
#endif
		}
	    }
	}
    }
    return ret;

}

/**
 * 从策略链表中进行策略匹配
 */
static bool uttNfPolicyListMatch(UttNfCurTime *curTime, UttNfPolicyList *list, u_int appFlag)
{
    register u_int flagsIndex;/*寄存器变量加快运行速度*/
    register u_char flagsBit;
    bool ret = false;

#if 0
    appFlag = appFlag - 1u;/*策略集合从下标0开始。但标志从1开始*/
#endif
    flagsIndex = appFlag >> 3u ;/*策略数组索引 即appFlag /8 */
    flagsBit = 1u << (appFlag  & 0x7u);/*策略元素值 即appFlag%8*/

    UTT_POLICY_DEBUG("%s %d  flagsIndex:%u flagsBit:%u flag:%u\n", __func__, __LINE__, flagsIndex, flagsBit, appFlag);
    while(list != NULL)
    {
	if ((list->value.nfTime.wday&curTime->week))
	{/*星期内*/
	    if (uttTimeIn(curTime->time , list->value.nfTime.timestart, list->value.nfTime.timestop))
	    {/*当前时间内*/
		if((u_char)list->value.nfApps.appFlags[flagsIndex] & (u_char)flagsBit)
		{
		    UTT_POLICY_DEBUG("%s %d  %u match ok\n", __func__, __LINE__, appFlag);
		    ret = true;/*找到一个匹配元素*/
		    break;
		}
	    }
	}

	list = list->next;
    }
    return ret;
}

#ifdef CONFIG_UTT_CLOUD_CONFIG
/**
 *发送消息给用户态
 *告知上线提醒的mac和类型
 * xrg
 ***/
extern int uttNlOnRemindToUser(unsigned char mac[],unsigned int noticeBanMask)
{
    int err = -1;
    UttNlNfOnRemind conf;
    conf.msgHead.nlType = UTT_NLNF_ONLIN_REMIND;
    conf.noticeBanMask = noticeBanMask;
    memcpy(conf.newMac,mac,ETH_ALEN);
    if(uttNfUserPid > 0)
    {
	err = uttNlSend((char *)&conf, sizeof(conf), uttNfUserPid, 0);
    }
    return err;
}
#endif
/**
 * 发消息给用户态
 * 让应用程序把相应 userIp的 过滤配置发送过来
 */
extern int uttNlPolicyAddRequest(u_long userIp)
{
    int err = -1;
    UttNlNfPolicy conf;
    conf.msgHead.nlType = UTT_NLNF_POLICY;
    conf.confType = UTT_NFCONF_ADD;
    conf.ip = userIp;/*用户ip*/
    if(uttNfUserPid > 0)
    {
	err = uttNlSend((char *)&conf, sizeof(conf), uttNfUserPid, 0);
    }
    return err;
}
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
/**
 * 发消息给用户态
 * 告知应用层ip下线
 */
extern int uttNlWebAuthAddRequest(u_long userIp)
{
    int err = -1;
    UttNlNfHashConf conf;
    conf.msgHead.nlType = UTT_NLNF_HASH;
    conf.confType = UTT_NFCONF_DEL;
    conf.ip = userIp;/*用户ip*/
    if(uttNfUserPid > 0)
    {
	UTT_POLICY_DEBUG("%s %d:-----request-----\n", __func__, __LINE__);
	err = uttNlSend((char *)&conf, sizeof(conf), uttNfUserPid, 0);
    }
    return err;
}
#endif
/**
 * 判断策略链表中是否已存在 指定的val
 * return : true 存在
 *          false 不存在
 */
static bool isUttNfPolicyContain( UttNfPolicyList *list,UttNfPolicyVal *policyVal)
{
   while(list != NULL)
    {
	if (memcmp(&(list->value), policyVal, sizeof(UttNfPolicyVal)) == 0)
	{/*存在value完全相同的节点*/
	    UTT_POLICY_DEBUG("%s %d\n", __func__, __LINE__);
	    return true;
	}
	list = list->next;
    }
    return false;
}
/**
 * 增加一条策略
 */
static void uttNfPolicyAdd(u_long ip, UttNfPolicyVal *policyVal)
{
    UttNfHashConf* hashConf = NULL;
    UttNfPolicyList *newPolicy = NULL;
    hashConf = uttNfHashConfGet(ip, 0);/*从hash表读取配置*/
    if(hashConf != NULL)
    {
	if(!isUttNfPolicyContain(hashConf->policyConf.policyHead, policyVal))
	{/*不存在此策略。则新建
	  *这里必须加此判断。否则 
	  * 1 可能用户本身配置了相同的策略。这个影响不是太大
	  * 2 用户层可能重复发送了某些策略。如 由于 信号量控制不好。导致的重发。这个影响比较大
	  */
	    newPolicy = (UttNfPolicyList*)kmalloc(sizeof(UttNfPolicyList), GFP_KERNEL);
	    if(newPolicy != NULL)
	    {/*内存分配成功*/
		memcpy(&(newPolicy->value), policyVal, sizeof(UttNfPolicyVal));
		/*放在链首*/
		newPolicy->next = hashConf->policyConf.policyHead;
		hashConf->policyConf.policyHead = newPolicy;
	    }
	}

    }
#if DEBUG_UTT_POLICY
    uttHashPolicyPrint();
#endif
    return;
}
/**
 * 删除一条规则链表
 */
void uttPolicyListDel(UttNfPolicyList *list)
{
    UttNfPolicyList *bakList = NULL;

    while(list != NULL) 
    {
	bakList = list;/*备份当前节点的指针*/
	list = list->next;/*指向下一个待删节点*/
	kfree(bakList);/*释放当前节点空间*/
    }
    return;
}

/**
 * 删除一个ip hash 对应的策略
 */
static void uttHashBufDelPolicy(UttNfHashBuf *buf)
{
    UttNfHashPolicy *hashPolicy = &(buf->conf.policyConf);/*指向策略配置结构*/
    uttPolicyListDel(hashPolicy->policyHead);/*删除该条配置链条*/
    memset(hashPolicy, 0, sizeof(UttNfHashPolicy));/*清空内存*/
    return;

}
/**
 * 删除所有规则
 */
static void uttNfPolicyDelAll(void)
{
    UTT_POLICY_DEBUG("%s %d\n", __func__, __LINE__);
    /*遍历hash表。并删除每个ip对应的规则*/
    uttNfHashTraverse(uttHashBufDelPolicy);
#if DEBUG_UTT_POLICY
    uttHashPolicyPrint();
#endif
    return;
}
/*行为管理规则设置函数*/
void uttNlPolicyConf(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    UttNlNfPolicy  *nlData;
    u_int dpid = 0;
    nlh = nlmsg_hdr(skb);
    dpid = nlh->nlmsg_pid;
    nlData = (UttNlNfPolicy*)NLMSG_DATA(nlh);
    switch(nlData->confType)
    {
	case UTT_NFCONF_INIT:
	    UTT_POLICY_DEBUG("\n=========================%s %d init dpid %d\n", __func__, __LINE__, dpid) ;
	    uttNfUserPid = dpid;
	    break;
	case UTT_NFCONF_ADD:
	    uttNfPolicyAdd(nlData->ip, &(nlData->policyVal));
	    break;
	case UTT_NFCONF_DEL_ALL:
	    uttNfPolicyDelAll();
	    break;

    }
    return;
}

#ifdef CONFIG_WEBAUTH_MAC_VALIDATE
void uttNlWifidogSend(int type, unsigned int ip, unsigned char *mac)
{
    int err = -1;
    struct timex  txc;
    unsigned int now;
    static unsigned int count = 0;
    static unsigned int last_time = 0;

    if (count > 1000)
    {
	do_gettimeofday(&(txc.time));
	now = txc.time.tv_sec;
	if (last_time == 0 || now - last_time > 60)
	{
	    last_time = now;
	    count = 0;
	}
	else
	{
	    return;
	}
    }

    UttNlNfWifidogConf conf;
    conf.msgHead.nlType = UTT_NLNF_WIFIDOG;
    conf.confType = type;
    //memcpy(&conf.ip, ip, 4);
    conf.ip = ip;
    memset(conf.mac, 0, 6);
    if (mac)
	memcpy(conf.mac, mac, 6);
    if(uttNfWifidogPid > 0)
    {
	err = uttNlSend((char *)&conf, sizeof(conf), uttNfWifidogPid, 0);
	count++;
    }
    return err;
}

void uttNlWifidogConf(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    UttNlNfPolicy  *nlData;
    u_int dpid = 0;
    nlh = nlmsg_hdr(skb);
    dpid = nlh->nlmsg_pid;
    nlData = (UttNlNfPolicy*)NLMSG_DATA(nlh);
    printk("%s %d:  \n", __func__, __LINE__);
    switch(nlData->confType)
    {
	case UTT_NF_WIFIDOG_INIT:
	    printk("%s %d:  \n", __func__, __LINE__);
	    uttNfWifidogPid = dpid;
	    break;

    }
    return;
}
#endif

