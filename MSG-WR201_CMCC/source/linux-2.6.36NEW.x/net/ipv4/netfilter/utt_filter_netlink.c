#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/types.h>
#include <net/sock.h>
#include <net/netlink.h>
#include <linux/netfilter_ipv4/utt_filter_kernel.h>
#ifdef CONFIG_UTT_DHCP_COLLISION
#include <linux/netfilter_ipv4/utt_collision_detection.h>
#endif
#include <linux/semaphore.h>
static struct semaphore netlink_mutex;	/* serializes user access */

#define DEBUG_UTT_NETLINK 0
#if DEBUG_UTT_NETLINK
#define UTT_NETLINK_DEBUG(fmt, argx...) printk(fmt, ##argx)
#else
#define UTT_NETLINK_DEBUG(fmt, argx...)
#endif
static struct sock *nl_sk = NULL;
static void uttNlRcv(struct sk_buff *__skb);
static int uttNlFilterInit(void);
static void uttNlFilterExit(void);

extern void uttNfHashInit(void);
#ifdef CONFIG_UTT_DHCP_COLLISION
extern void uttNfColliDetecInit(void);
#endif
extern void uttNlHashConf(struct sk_buff *skb);
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICY
extern void uttNlPolicyConf(struct sk_buff *skb);
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTWEBIND
extern void uttNlDnsFilter(struct sk_buff *skb);
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTHOSTSTA
extern void uttNlStats(struct sk_buff *skb);
extern void uttStatsPerIpInit(void);
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTEXCEPTQQ
extern void uttNlQQFilter(struct sk_buff *skb);
extern void uttNlEnterpriseQQFilter(struct sk_buff *skb);
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTEXCEPTMSN
extern void uttNlMsnFilter(struct sk_buff *skb);
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTEXCEPTALI
extern void uttNlAliFilter(struct sk_buff *skb);
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTLOG
extern void uttNlLog(struct sk_buff *skb);
extern void uttNlEim(struct sk_buff *skb);
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICYROUTE
extern void uttNlPolicyRouteConf(struct sk_buff *skb);
#endif

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTNETSNIPER
extern void uttNlNetSniper(struct sk_buff *skb);
extern void uttStatSniperInit(void);
#endif
#ifdef CONFIG_WEBAUTH_MAC_VALIDATE
extern void uttNlWifidogConf(struct sk_buff *skb);
#endif
#ifdef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
extern void uttNlwifidogOperation(struct sk_buff *skb, UttNlNfType type);
#endif

#ifdef CONFIG_UTT_DEBUG
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTDNSREDIRECT
extern void utt_dns_notice_debug(UttNfConfType type);
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICYROUTE
#ifdef CONFIG_UTT_DEB_POLICY_ROUTE
extern void utt_policy_route_debug(UttNfConfType type);
#endif
#endif
#ifdef CONFIG_UTT_WIFIDOG_REDIRECT
extern void utt_wifi_debug(UttNfConfType type);
#endif
#endif
#ifdef CONFIG_MULTI_SSID_DHCP_POOL
extern void uttNldMultiSsidDhcpPool(struct sk_buff *skb);
#endif
/**
 * netlink 发送数据
 *
 * data:指向要发送的数据
 * dataLen:发送数据的长度
 * dpid:目的进程id
 * dgrp:目的组.非0则组播。否则单播
 *
 * return: < 0 失败
 */
int uttNlSend(char *data, u_int dataLen, u_int dpid, u_int dgrp)
{
    struct sk_buff *skb_1;
    struct nlmsghdr *nlh;
    int nlLen = NLMSG_SPACE(dataLen);/*包含netlink消息头的长度*/
    int err = 0;
#if 0
    if(!data || !nl_sk)
    {
	return ;
    }
#endif

    skb_1 = alloc_skb(nlLen,GFP_KERNEL);
    if(skb_1)
    {/*分配成功*/
	nlh = nlmsg_put(skb_1,0,0,0,dataLen,0);

	NETLINK_CB(skb_1).pid = 0;/*源代pid*/
	NETLINK_CB(skb_1).dst_group = dgrp;/*目的组*/

	memcpy(NLMSG_DATA(nlh), data, dataLen);
	UTT_NETLINK_DEBUG("%s %d .\n", __func__, __LINE__);

	if(dgrp != 0)
	{/*组播*/
	    netlink_broadcast(nl_sk, skb_1, 0, dgrp, GFP_KERNEL);
	}
	else
	{/*单播*/
	   err =  netlink_unicast(nl_sk,skb_1,dpid,MSG_DONTWAIT);
	}
    }
    else
    {
	err = -1;
	UTT_NETLINK_DEBUG(KERN_ERR "utt_netfilter_nelink:alloc_skb_1 error\n");
    }
    return err; 

}

#ifdef CONFIG_UTT_DEBUG
static void uttNlDebug(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    UttNlNfDebugConf *nlData;

    nlh = nlmsg_hdr(skb);
    nlData = (UttNlNfDebugConf*)NLMSG_DATA(nlh);/*取值*/
    UTT_NETLINK_DEBUG("%s-%d, nlData->entryVal.debugFlag = %d, nlData->confType = %d\n", __func__, __LINE__, nlData->entryVal.debugFlag, nlData->confType) ;
    switch (nlData->entryVal.debugFlag) {
#if CONFIG_NETFILTER_XT_TARGET_UTTDNSREDIRECT
	case UTT_DEBUG_DNS_NOTICE:
	    utt_dns_notice_debug(nlData->confType);
#ifdef CONFIG_UTT_WIFIDOG_REDIRECT
	    utt_wifi_debug(nlData->confType);
#endif
	    break;
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICYROUTE
#ifdef CONFIG_UTT_DEB_POLICY_ROUTE
	case UTT_DEBUG_POLICY_ROUTE:
	    utt_policy_route_debug(nlData->confType);
	    break;
#endif
#endif
#ifdef CONFIG_UTT_DHCP_COLLISION
	case UTT_DEBUG_DHCP_COLLISION:
	    utt_dhcp_collision_debug(nlData->confType);
	    break;
#endif
	default:/*default*/
	    break;
    }
    return;
}
#endif

/*
 * netlink 数据接收函数
 * */
static void uttNlRcv(struct sk_buff *__skb)
{
    struct sk_buff *skb;
    struct nlmsghdr *nlh;
    UttNlNfMsgHead *msgHead = NULL;

    skb = skb_get (__skb);
    if(skb->len >= NLMSG_SPACE(0))
    {
	nlh = nlmsg_hdr(skb);
	msgHead = (UttNlNfMsgHead*)NLMSG_DATA(nlh);
	UTT_NETLINK_DEBUG("=========================Message received type:%d\n", msgHead->nlType) ;
	down(&netlink_mutex);
	switch(msgHead->nlType)
	{
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICY
	    case UTT_NLNF_POLICY:/*行为管理规则设置*/
		uttNlPolicyConf(skb);
		break;
#endif
	    case UTT_NLNF_HASH:
		uttNlHashConf(skb);
		break;
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTWEBIND
	    case UTT_NLNF_DNS:
		uttNlDnsFilter(skb);
		break;
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTHOSTSTA
	    case UTT_NLNF_STATS:
		uttNlStats(skb);
		break;
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTEXCEPTQQ
	    case UTT_NLNF_QQ:
		uttNlQQFilter(skb);
		break;
        case UTT_NLNF_ENTERPRISE_QQ:
            uttNlEnterpriseQQFilter(skb);
            break;
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTEXCEPTMSN
	    case UTT_NLNF_MSN:
		uttNlMsnFilter(skb);
		break;
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTEXCEPTALI
	    case UTT_NLNF_ALI:
		uttNlAliFilter(skb);
		break;
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTLOG
	    case UTT_NLNF_LOG:
		uttNlLog(skb);
		break;
	    case UTT_NLNF_EIM:
		uttNlEim(skb);
		break;
#endif
#ifdef CONFIG_UTT_DEBUG
	    case UTT_NLNF_DEBUG:
		uttNlDebug(skb);
		break;
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICYROUTE
	    case UTT_NLNF_POLICY_ROUTE:
		uttNlPolicyRouteConf(skb);
		break;
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTNETSNIPER
		case UTT_NLNF_NET_SNIPER:
		uttNlNetSniper(skb);
		break;
#endif
#ifdef CONFIG_WEBAUTH_MAC_VALIDATE
	    case UTT_NLNF_WIFIDOG:
		uttNlWifidogConf(skb);
		break;
#endif
#ifdef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
	    /*wifidog的所有配置都在此处实现，使用相同的接口函数*/
	    case UTT_NLNF_WIFIDOG_DNS_WHITE:
		uttNlwifidogOperation(skb, msgHead->nlType);
		break;
#endif
#ifdef CONFIG_MULTI_SSID_DHCP_POOL
	    case UTT_MULTI_SSID_DHCP_POOL:
		uttNldMultiSsidDhcpPool(skb);
		break;
#endif
	    default:
		break;
	}
	up(&netlink_mutex);
	kfree_skb(skb);
    }

}

/**
 * 初始化入口
 */
static int uttNlFilterInit(void)
{
    UTT_NETLINK_DEBUG("======================my_net_link: create netlink socket start.\n");

    nl_sk = netlink_kernel_create(&init_net, NETLINK_UTT_NETFILTER, 0,
	    uttNlRcv, NULL, THIS_MODULE);

    if(!nl_sk){
	UTT_NETLINK_DEBUG(KERN_ERR "create netlink socket error.\n");
	return 1;
    }

    sema_init(&netlink_mutex, 1);/*初始化*/

    UTT_NETLINK_DEBUG("create netlink socket ok.\n");
    uttNfHashInit();/*hash表初始化*/
#ifdef CONFIG_UTT_DHCP_COLLISION
    uttNfColliDetecInit();
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTHOSTSTA
    uttStatsPerIpInit();
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTNETSNIPER
	uttStatSniperInit();
#endif
    return 0;
}

static void uttNlFilterExit(void)
{
    if(nl_sk != NULL){
	sock_release(nl_sk->sk_socket);
    }

    UTT_NETLINK_DEBUG("my_net_link: self module exited\n");
}

module_init(uttNlFilterInit);
module_exit(uttNlFilterExit);

MODULE_AUTHOR("UTT");
MODULE_LICENSE("GPL");

