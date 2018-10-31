#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <net/sock.h>
#include <linux/skbuff.h>
#include <linux/autoconf.h>
#include <linux/ip.h>
#include <net/netlink.h>
#include <linux/netfilter_ipv4/utt_filter_kernel.h>
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
#include <linux/netfilter_ipv4/utt_module_init.h>
#endif

#define DEBUG_UTT_HASH 0
#if DEBUG_UTT_HASH
#define UTT_HASH_DEBUG(fmt, argx...) printk(fmt, ##argx)
#else
#define UTT_HASH_DEBUG(fmt, argx...)
#endif

#define ip_hash_key(ipaddr) ((ipaddr & 0xffu) ^ ((ipaddr >> 8u) & 0xffu))
#define UTT_HASH_POLL_INTERVAL 300*HZ /*hash表每隔300s自动超时不在线*/

unsigned int staleTimeUser = 0;

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICY
extern void uttPolicyListDel(UttNfHashPolicy *list);
#endif
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
unsigned int triggerUser=0U;
extern int uttNlWebAuthAddRequest(u_long userIp);
#endif

/*直接hash表头*/
UttNfHashBuf *uttNfIpHashTable[MAX_UTTNF_HASH_TABLE];
static struct timer_list nfHashTimer;
#ifdef CONFIG_UTT_WIFIDOG_REDIRECT
#define UTT_HASH_WIFI_POLL_INTERVAL 1*HZ /*远端认证hash表每1s自动超时不在线*/
static struct timer_list nfHashWifiTimer;
#endif

static void nfHashTimeOut(u_long param);
static void nfHashPollInit(void);
static void uttNfHashConfDel(UttNfHashConf *hashConf);

void uttNfHashInit(void);
void uttNlHashConf(struct sk_buff *skb);

#if DEBUG_UTT_HASH
static void uttHashBufPrint(UttNfHashBuf *buf)
{
    static int i = 0;
    printk("%08x:%u ", buf->ip, buf->status);
    i++;
    if(i%8 == 0)
    {
	printk("\n");
    }
}
static void uttHashPrint(void)
{
    printk("\n_______________________________%s start\n\n", __func__);
    uttNfHashTraverse(uttHashBufPrint);
    printk("\n_______________________________%s end\n\n", __func__);

    return;
}
void UttHashDebug(void)
{
    int num = 0;
    uttNfHashBufGet(0x00001000u,&num);
    uttNfHashBufGet(0x10001000u,&num);
    uttNfHashBufGet(0x20001000u,&num);
    uttNfHashBufGet(0x30001000u,&num);
    uttNfHashBufGet(0x40001000u,&num);
    uttNfHashBufGet(0x00001500u,&num);

#if 0


    uttHashPrint();
    uttNfHashBufDel(0x00001000u);

    uttHashPrint();

    uttNfHashBufDel(0x30001000u);
    uttNfHashBufGet(0x00001000u,1);
    uttHashPrint();
    uttNfHashBufDel(0x40001000u);

    uttHashPrint();
    uttNfHashBufDel(0x00001000u);
    uttNfHashBufDel(0x10001000u);
    uttNfHashBufDel(0x20001000u);
    uttNfHashBufDel(0x30001000u);
    uttNfHashBufDel(0x40001000u);

    uttNfHashBufDel(0x00001500u);
#endif

   
    uttHashPrint();
    return;
}

#endif

#if 0
#define  TIMER_CALLBACK_TIMES 2
extern void updateStatsBehavior(UttStatsPerIp *stats, int clear);
#endif
/**
 * 超时清理不在线的ip地址表项
 */
static void nfHashTimeOut(u_long param)
{
    int i = 0;
    UttNfHashBuf **hashBufPP = NULL;/*二级指针*/
    UttNfHashBuf *hashBufP = NULL;
#if 0 /* 接收用户空间消息更新统计信息, 此处不再需要 */
    /* 每调用2次timeout,需要调用一次用户行为统计 */
    static int flag = 0;
    flag++;
#endif

#if DEBUG_UTT_HASH
    UTT_HASH_DEBUG("%s %d:timeout hash start\n", __func__, __LINE__);
    uttHashPrint();
#endif
    /*遍历整个hash表*/
    for(i = 0; i < MAX_UTTNF_HASH_TABLE; i++)
    {
	hashBufPP = &(uttNfIpHashTable[i]);/*定位到表头*/

	/*遍历冲突项*/
	while(*hashBufPP != NULL)
	{
	    if((*hashBufPP)->status == 0)
	    {/*已超时的直接删除*/
		UTT_HASH_DEBUG("%s %08x:timeout hash %08x\n", __func__, __LINE__, (*hashBufPP)->ip);
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
		UTT_HASH_DEBUG("%s %08x:timeout hash ip %08x, trigger %d\n", __func__, __LINE__,(*hashBufPP)->ip, (*hashBufPP)->trigger);
		/* 上线时已经设置触发器
		 * 超时会发请求消息，应用层挂断此ip对应的web认证账号
		 * */
		if((((*hashBufPP)->trigger)&(1U<< UTT_NF_HASH_WEB_AUTH_DES_NOTIFY)) ||
			(((*hashBufPP)->trigger)&(1U<< UTT_NF_HASH_WIFIDOG_DES_NOTIFY)))
		{
		    UTT_HASH_DEBUG("%s %d: send request for web auth account\n", __func__, __LINE__);
		    /*通知应用层*/
		    uttNlWebAuthAddRequest((*hashBufPP)->ip);
		}
#endif
		hashBufP = *hashBufPP;/*记录当前节点位置*/
		*hashBufPP = hashBufP->next;/*指向当前节点的指针，指向当前节点的next*/
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
		/**
		 *遍历各模块，调用各模块析构函数
		 */
		uttTraverseIpNodeExit(hashBufP);
#endif
		uttNfHashConfDel(&(hashBufP->conf));/*删除当前节点对应的配置*/
		kfree(hashBufP);/*释放当前节点的内容*/
		/*这删除节点后已经跳到了下一个冲突节点。故continue*/
		continue;
	    }
	    else
	    {/*未超时的。超时标志置为0，等下次轮询时超时*/
		(*hashBufPP)->status = 0;
#if 0
		/* 用户统计, 未删除的节点才统计 */
		if (flag >= TIMER_CALLBACK_TIMES) {
		    updateStatsBehavior(&((*hashBufPP)->conf.userStats), 1);
		}
#endif
	    }
	    hashBufPP = &((*hashBufPP)->next);/*查找下一个冲突项*/
	}
    }
#if 0
    if (flag >= TIMER_CALLBACK_TIMES) {
	flag = 0;
    }
#endif

#if DEBUG_UTT_HASH
    UTT_HASH_DEBUG("%s %d:timeout hash end\n", __func__, __LINE__);
    uttHashPrint();
#endif
    if(staleTimeUser > UTT_HASH_POLL_INTERVAL) {
    UTT_HASH_DEBUG("%s-%d: UTT_HASH_POLL_INTERVAL =%u, staleTimeUser =%u\n",__func__,__LINE__,UTT_HASH_POLL_INTERVAL,staleTimeUser);
    mod_timer(&nfHashTimer, jiffies + staleTimeUser);/*重新激活定时器*/
    } else {
    UTT_HASH_DEBUG("%s-%d: UTT_HASH_POLL_INTERVAL =%u, staleTimeUser =%u\n",__func__,__LINE__,UTT_HASH_POLL_INTERVAL,staleTimeUser);
    mod_timer(&nfHashTimer, jiffies + UTT_HASH_POLL_INTERVAL);/*重新激活定时器*/
    }
    return;
}
#ifdef CONFIG_UTT_WIFIDOG_REDIRECT
/*
 * 远端认证调用的超时函数
 * 去掉Apple, Wp系统的url认证次数
 * */
static void nfHashWifiTimeOut(u_long param)
{
    int i = 0;
    UttNfHashBuf **hashBufPP = NULL;/*二级指针*/
    UttNfHashBuf *hashBufP = NULL;

#if 0
    printk("%s %d:timeout hash start\n", __func__, __LINE__);
#endif
    /*遍历整个hash表*/
    for(i = 0; i < MAX_UTTNF_HASH_TABLE; i++)
    {
	hashBufPP = &(uttNfIpHashTable[i]);/*定位到表头*/

	/*遍历冲突项*/
	while(*hashBufPP != NULL)
	{
#if 0
	    printk("%s %d: wifi timeout hash ip %08x, count %d, second %d\n", __func__, __LINE__,(*hashBufPP)->ip, (*hashBufPP)->wifiInfo.count,(*hashBufPP)->wifiInfo.second);
#endif
	    if((*hashBufPP)->wifiInfo.count > 0U)
	    {/*已超时的直接删除*/
		if((*hashBufPP)->wifiInfo.second >= 30U) {
		    (*hashBufPP)->wifiInfo.count = 0U;
		    (*hashBufPP)->wifiInfo.second = 0U;
		} else {
		    (*hashBufPP)->wifiInfo.second += 1U;
		}
	    }
	    hashBufPP = &((*hashBufPP)->next);/*查找下一个冲突项*/
	}
    }

#if 0
    printk("%s-%d: UTT_HASH_WIFI_POLL_INTERVAL =%u\n",__func__,__LINE__,UTT_HASH_WIFI_POLL_INTERVAL);
#endif
    mod_timer(&nfHashWifiTimer, jiffies + UTT_HASH_WIFI_POLL_INTERVAL);/*重新激活定时器*/
    return;
}
#endif
/**
 * 轮询初始化函数入口
 */
static void nfHashPollInit(void)
{
    init_timer(&nfHashTimer);
    if(staleTimeUser > UTT_HASH_POLL_INTERVAL) {
    nfHashTimer.expires = jiffies + staleTimeUser;   /* 定时器超时时的节拍数*/
    } else {
    nfHashTimer.expires = jiffies + UTT_HASH_POLL_INTERVAL;   /* 定时器超时时的节拍数*/
    }
    nfHashTimer.data     = 0;                /* 给定时器处理函数传入参数0*/
    nfHashTimer.function = nfHashTimeOut;      /* 定时器超时时调用的函数*/
    add_timer(&nfHashTimer);/*激活定时器*/
#ifdef CONFIG_UTT_WIFIDOG_REDIRECT
    init_timer(&nfHashWifiTimer);
    nfHashWifiTimer.expires = jiffies + UTT_HASH_POLL_INTERVAL;   /* 定时器超时时的节拍数*/
    nfHashWifiTimer.data     = 0;                /* 给定时器处理函数传入参数0*/
    nfHashWifiTimer.function = nfHashWifiTimeOut;      /* 定时器超时时调用的函数*/
    add_timer(&nfHashWifiTimer);/*激活定时器*/
#endif
    return;
}

/**
 * 在hash表中查找指定的ip地址,对应的conf
 * isCreat: 1 若找不到相应的表项，则建立
 *          0 若找不到相应的表项，不建立
 *
 * return:  NULL 查找失败 若isCreat为1，则建立失败
 *          非NULL 找到后新建的hash结构配置指针
 *
 */
UttNfHashConf *uttNfHashConfGet(u_long ipaddr, int isCreat)
{
    UttNfHashBuf * buf = uttNfHashBufGet(ipaddr, &isCreat);
    if(buf != NULL)
    {
	return &(buf->conf);
    }
    return NULL;
}
#ifdef CONFIG_UTT_SKB_IIFTYPE
bool isWirelessIn(struct sk_buff *skb)
{
    struct net_device *device = NULL;
    bool ret = false;
    /*获取入口设备*/
    device = dev_get_by_index(sock_net(skb->sk),skb->skb_iif);
    if(device != NULL)
    {
	UTT_HASH_DEBUG("%s %d device %s,CONFIG_UTT_LAN2_IFNAME=%s: \n", __func__, __LINE__, device->name,CONFIG_UTT_LAN2_IFNAME);
	if(strcmp(device->name, CONFIG_UTT_LAN_IFNAME) != 0)
	{/*入口为无线口*/
	    ret = true;
	}
	dev_put(device);/*释放设备引用*/
    }
    return ret;
}
#endif
#if 0
bool isSkbComeFromLan(struct sk_buff *skb)
{
    struct net_device *device = NULL;
    bool ret = false;
    /*获取入口设备*/
    device = dev_get_by_index(sock_net(skb->sk),skb->iif);
    if(device != NULL)
    {
	UTT_HASH_DEBUG("%s %d device %s: \n", __func__, __LINE__, device->name);
	if(strcmp(device->name, CONFIG_UTT_LAN_IFNAME) == 0)
	{/*入口为lan口*/
	    ret = true;
	}
	dev_put(device);/*释放设备引用*/
    }
    return ret;
}
#endif
/**
 * 通过当前skb的内容 获取hash buf
 * isCreat：找不到是否创建
 */
UttNfHashBuf *uttNfHashBufGetBySkb(struct sk_buff*skb, int isCreat)
{
    struct iphdr *ipH = NULL;
    
    UTT_HASH_DEBUG("%s %d device:%s\n", __func__, __LINE__, skb->dev->name);

    ipH = ip_hdr(skb);
    if(isSkbComeFromLan(skb))
    {/*入口为lan口*/
	return uttNfHashBufGet(ipH->saddr, &isCreat);
    }
    else 
    {/*iif指向的设备不为lan口。
      *这里出口物理设备比较难以获得。要涉及到pppoe，vlan以及br等虚接口
      *所以lan口进入时，记录ip。否则只是查表。不记录ip*/
	UTT_HASH_DEBUG("%s %d out device:%s dip:%pI4\n", __func__, __LINE__, skb->dev->name, &(ipH->daddr));
	int xrg = 0 ;
	return uttNfHashBufGet(ipH->daddr, &xrg);
    }
    return NULL;
}

/**
 * 在hash表中查找 lan口进来的包对应的源ip对应的 hasn表 config
 * isCreat: 1 若找不到相应的表项，则建立
 *          0 若找不到相应的表项，不建立
 *
 * return:  NULL 查找失败 若isCreat为1，则建立失败
 *          非NULL 找到后新建的hash结构配置指针
 *
 */
UttNfHashConf *uttNfHashConfGetBySkb(struct sk_buff* skb, int isCreat)
{
    UttNfHashBuf * buf = uttNfHashBufGetBySkb(skb, isCreat);
    if(buf != NULL)
    {
	return &(buf->conf);
    }
    return NULL;
}
/**
 * 在hash表中查找指定的ip地址,对应的buffer
 * isCreat: 1 若找不到相应的表项，则建立
 *          0 若找不到相应的表项，不建立
 * 修改为：*isCreat ：NULL，等同于0.
 *		    非NULL，等同于1，传参实例，int isCreat = 1. 然后传入取地址
 *
 * return:  NULL 查找失败 若isCreat为1，则建立失败
 *          非NULL 找到后新建的hash结构指针
 *
 */
UttNfHashBuf *uttNfHashBufGet(u_long ipaddr, int *isCreat)
{
    u_int key = ip_hash_key(ntohl(ipaddr));/*获取key*/
    struct timespec uptime;
    UttNfHashBuf **hashBufPP = &(uttNfIpHashTable[key]);/*二级指针，定位到表项头部*/

    UTT_HASH_DEBUG("%s %d:hash key %d\n", __func__, __LINE__, key);
    /*遍历该key对应的链表*/
    while(*hashBufPP != NULL)
    {
	if((*hashBufPP)->ip == ipaddr)
	{/*找到*/
	    if(*isCreat == 1)
	    {
		*isCreat  = 0;/*找到则不再新建*/
		(*hashBufPP)->status = 1;/*状态位置为在线*/
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
		if(triggerUser == (1U<<UTT_NF_HASH_WIFIDOG_DES_NOTIFY)) {
		    (*hashBufPP)->trigger |= triggerUser;
		}
#endif
	    }
	    UTT_HASH_DEBUG("%s %d:find ok hash %pI4, key %d\n", __func__, __LINE__, &ipaddr,key);
	    break;
	}
	hashBufPP = &((*hashBufPP)->next);/*查找下一个项*/
    }
    if(*isCreat == 1)
    {/*需要新建表项*/
	*hashBufPP = (UttNfHashBuf*) kmalloc(sizeof(UttNfHashBuf), GFP_ATOMIC);/*分配一个表项*/
	if(*hashBufPP != NULL)
	{/*内存分配成功*/
	    memset(*hashBufPP, 0 ,sizeof(UttNfHashBuf));
	    (*hashBufPP)->status = 1;/*状态位置为在线*/
	    (*hashBufPP)->ip = ipaddr;
#ifdef CONFIG_UTT_SKB_IIFTYPE
	    (*hashBufPP)->conf.iif = -1;/*初始化赋值为-1*/
#endif
	    (*hashBufPP)->conf.userStats.realtimeflowS.lastClrJiffies = jiffies;

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTHOSTSTA
	    /*记录当前离开机的运行时间
	     * 这个在不同的内核下实现方法不同*/
	    do_posix_clock_monotonic_gettime(&uptime);
	    monotonic_to_bootbased(&uptime);
	    (*hashBufPP)->conf.userStats.stTime = (u_long)( uptime.tv_sec);
#endif
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
	    if(triggerUser == (1U<<UTT_NF_HASH_WIFIDOG_DES_NOTIFY)) {
		(*hashBufPP)->trigger |= triggerUser;
	    }
#endif

#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    /**
	     *遍历各模块，初始化ip hash节点配置中的优先级字段
	     * 修改到ip_input.c中去初始化。by xrg.
	    uttTraverseIpNodeInit(*hashBufPP);*/
#endif

	    UTT_HASH_DEBUG("%s %d:new hash %pI4, key %d\n", __func__, __LINE__, &ipaddr,key);
	}
#if DEBUG_UTT_HASH 
	else
	{
	    UTT_HASH_DEBUG("%s %d: kernel memory alloced error\n", __func__, __LINE__);
	}
#endif
    }

    return *hashBufPP;

}
/*
 * 删除hash表项对应的配置
 * 以及配置相关数据
 * */
static void uttNfHashConfDel(UttNfHashConf *hashConf)
{
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICY
    /*删除该ip对应的规则链条*/
    uttPolicyListDel(hashConf->policyConf.policyHead);/*删除该条配置链条*/
#endif
}
/**
 * 删除ipaddr对应的 hash表项
 */
void uttNfHashBufDel(u_long ipaddr)
{
    u_int key = ip_hash_key(ntohl(ipaddr));/*获取key*/
    UttNfHashBuf **hashBufPP = &(uttNfIpHashTable[key]);/*二级指针，定位到表项头部*/
    UttNfHashBuf *hashBufP = NULL;
    /*遍历该key对应的链表*/
UTT_HASH_DEBUG("%s %08x:del hash %08x hash key %u\n", __func__, __LINE__, ipaddr,key);
    while(*hashBufPP != NULL)
    {
	if((*hashBufPP)->ip == ipaddr)
	{/*找到*/
	    UTT_HASH_DEBUG("%s %08x:del hash %08x\n", __func__, __LINE__, ipaddr);
	    hashBufP = *hashBufPP;/*记录当前节点位置*/
	    *hashBufPP = hashBufP->next;/*指向当前节点的指针，指向当前节点的next*/
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    /**
	     *遍历各模块，调用各模块析构函数
	     */
	    uttTraverseIpNodeExit(hashBufP);
#endif
	    uttNfHashConfDel(&(hashBufP->conf));/*删除当前节点对应的配置*/
	    kfree(hashBufP);/*释放当前节点的内容*/
	    break;
	}
	hashBufPP = &((*hashBufPP)->next);/*查找下一个项*/
    }

    return;

}

#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
/**
 * 修改ipaddr对应的 hash表项的变量trigger
 * 当流量检测超时后，通知应用层web认证账号下线
 */
void uttNfHashBufEditTrigger(UttNlNfHashConf *nfconf)
{
#if 1
    UttNfHashBuf *tmpNode = NULL;

    UTT_HASH_DEBUG("%s %08x:edit hash %08x,trigger %u\n", __func__, __LINE__, nfconf->ip,nfconf->trigger);
    int isCreate = 0;
    tmpNode = uttNfHashBufGet(nfconf->ip, &isCreate);
    if(tmpNode != NULL)
    {
	tmpNode->trigger = nfconf->trigger;
	UTT_HASH_DEBUG("%s %08x:edit hash trigger %d\n", __func__, __LINE__, tmpNode->trigger);
    }
#endif
#if 0
    u_int key = ip_hash_key(ntohl(nfconf->ip));/*获取key*/
    UttNfHashBuf **hashBufPP = &(uttNfIpHashTable[key]);/*二级指针，定位到表项头部*/
    UttNfHashBuf *hashBufP = NULL;
UTT_HASH_DEBUG("%s %d:edit hash %08x hash key %d\n", __func__, __LINE__, nfconf->ip,key);
    /*遍历该key对应的链表*/
    while(*hashBufPP != NULL)
    {
	if((*hashBufPP)->ip == nfconf->ip)
	{/*找到*/
	    UTT_HASH_DEBUG("%s %d:edit hash %08x,trigger %u\n", __func__, __LINE__, nfconf->ip,nfconf->trigger);
	    (*hashBufPP)->trigger = nfconf->trigger;/*修改当前节点对应的trigger*/
	    UTT_HASH_DEBUG("%s %d:edit hash trigger %d\n", __func__, __LINE__, (*hashBufPP)->trigger);
	    hashBufP = *hashBufPP;/*记录当前节点位置*/
	    *hashBufPP = hashBufP->next;/*指向当前节点的指针，指向当前节点的next*/
	    break;
	}
	hashBufPP = &((*hashBufPP)->next);/*查找下一个项*/
    }
#endif
UTT_HASH_DEBUG("%s %08x: end edit hash %08x\n", __func__, __LINE__, nfconf->ip);

    return;

}
#endif
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
/**
 * 遍历hash表
 * processFun : 处理函数
 */
void uttNfHashTraverseUserConf(void (*processFun)(UttNfHashBuf* buf, void *userConf), void *userConf)
{
    int i = 0;
    UttNfHashBuf *hashBufP = NULL;

    for(i = 0; i < MAX_UTTNF_HASH_TABLE; i++)
    {
	hashBufP = uttNfIpHashTable[i];/*定位到表项头部*/
	/*遍历该key对应的链表*/
	while(hashBufP != NULL)
	{
	    processFun(hashBufP,userConf);
	    hashBufP =hashBufP->next;/*查找下一个项*/
	}
    }
    return;
}
EXPORT_SYMBOL(uttNfHashTraverseUserConf);
#endif
/**
 * 遍历hash表
 * processFun : 处理函数
 */
void uttNfHashTraverse(void (*processFun)(UttNfHashBuf* buf))
{
    int i = 0;
    UttNfHashBuf *hashBufP = NULL;

    for(i = 0; i < MAX_UTTNF_HASH_TABLE; i++)
    {
	hashBufP = uttNfIpHashTable[i];/*定位到表项头部*/
	/*遍历该key对应的链表*/
	while(hashBufP != NULL)
	{
	    processFun(hashBufP);
	    hashBufP =hashBufP->next;/*查找下一个项*/
	}
    }
    return;
}

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTHOSTSTA
/*
 * 清除ip hash表中单个IP的行为统计信息
 */
extern void clearIpBvStats(u_long ip) {
    UttNfHashBuf *hashBufP = NULL;
    u_int key;

    /* 非法ip */
    if (ip == 0) {
	return;
    }
    key = ip_hash_key(ntohl(ip));
    hashBufP = uttNfIpHashTable[key]; 

    while (hashBufP) {
	if (hashBufP->ip == ip) {
	    memset(hashBufP->conf.userStats.netBVStats, 0, sizeof(hashBufP->conf.userStats.netBVStats));
	    /* 清除当前行为状态 */
	    memset(hashBufP->conf.userStats.netBehavior, 0, sizeof(hashBufP->conf.userStats.netBehavior));
	    break;
	}
	hashBufP = hashBufP->next;
    }
    return;
}
#endif

/**
 * netlink hash表配置
 */
void uttNlHashConf(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    UttNlNfHashConf  *nlData;
    nlh = nlmsg_hdr(skb);
    nlData = (UttNlNfHashConf*)NLMSG_DATA(nlh);/*定位到数据区域*/

    if(nlData->confType == UTT_NFCONF_DEL) {
    UTT_HASH_DEBUG("%s %08x:netlinke down ip %08x\n", __func__, __LINE__, nlData->ip);
    /*目前只有删除一个大类型。若需小类型，则在此区分*/
    uttNfHashBufDel(nlData->ip);/*删除对应的表项*/   
    } else if(nlData->confType == UTT_NFCONF_EDIT) {
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
    /*修改trigger*/
    UTT_HASH_DEBUG("%s %08x:netlinke edit ip %08x\n", __func__, __LINE__, nlData->ip);
    uttNfHashBufEditTrigger(nlData);
#endif
    } else if(nlData->confType == UTT_NFCONF_ADD) {
	if(nlData->staleEn == 1) {
	    staleTimeUser = (nlData->staleTime)*60*HZ;
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
	    triggerUser = nlData->trigger;
	    UTT_HASH_DEBUG("%s-%d: staleTime = %u, staleTimeUser =%u,triggerUser =%u\n",__func__,__LINE__,nlData->staleTime,staleTimeUser,triggerUser);
#endif
	    UTT_HASH_DEBUG("%s-%d: staleTime = %u, staleTimeUser =%u\n",__func__,__LINE__,nlData->staleTime,staleTimeUser);
	}
    }
    return;
}
void uttNfHashInit(void)
{
    nfHashPollInit();/*轮询初始化*/
#if 0
#if DEBUG_UTT_HASH
    UttHashDebug();
#endif
#endif
}

#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
/********************************************************************
 * 函数： uttIpMatch
 * 功能： 数据包IP地址的匹配函数
 * 创建： 2013-02-20
 * 参数：  ipGrp: 要匹配的IP组
 *	  ip:	 数据包ip地址
 * 返回： 1  匹配成功
 *	  0  匹配失败
 * 输出： 略
 * 作者：  
 ********************************************************************/ 
int uttIpMatch(IpGrpEntry *ipGrp, uint32_t ip)
{
    uint32_t ret = 0;
    uint32_t ipHost = ntohl(ip);
    uint32_t size = sizeof(ipHost) << 1;

    if(ipGrp != NULL) {
	if(ipGrp->entryType == IP_RANGE){ 
	    /* ip 地址段匹配
	     * 当起始和结束ip都为0x0时，默认为是所有地址
	     * */
	    /*所有地址*/
	    if((ipGrp->entryVal.range.ipFrom == 0U)&&(ipGrp->entryVal.range.ipTo == 0U)) {
		ret = 1;
	    }else if((ipHost >= ipGrp->entryVal.range.ipFrom)&&(ipHost <= ipGrp->entryVal.range.ipTo)) {	/*地址段比较*/
		ret = 1;
	    }
	}else if(ipGrp->entryType == IP_GROUP){
	    if (ipGrp->entryVal.index == CONFIG_ALL_USER) {/*用户组为所有用户时*/
		ret = 1;
	    } else {
		/* ip 组匹配
		 * 由组名，获取ipset的索引值
		 * 然后根据索引值检测ip是否在此ipset组中*/
		if(ipGrp->entryVal.index != IP_SET_INVALID_ID) {
		    ret = ip_set_testip_by_index(ipGrp->entryVal.index,(void *)&ipHost,size);
		}
	    }
	}
    }
    return ret;
}
EXPORT_SYMBOL(uttIpMatch);
#endif
