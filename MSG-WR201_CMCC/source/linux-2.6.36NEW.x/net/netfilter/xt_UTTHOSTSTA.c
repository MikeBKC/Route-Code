/*
 * This is a module which is used for setting the MSS option in TCP packets.
 *
 * Copyright (C) 2000 Marc Boucher <marc@mbsi.ca>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
/* TODO: 
 * 统计是否需要加信号量?
 *
 */
#include <linux/types.h>
#include <net/sock.h>
#include <linux/autoconf.h>
#include <net/netlink.h>

#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/tcp.h>
#include <net/ipv6.h>
#include <net/tcp.h>
#include <net/netfilter/nf_conntrack.h>

#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv6/ip6_tables.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_tcpudp.h>
#include <linux/netfilter/xt_UTTHOSTSTA.h>
#include <linux/netfilter_ipv4/utt_netfilter.h>
#include <linux/netfilter_ipv4/utt_filter_kernel.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wang bingrong<ff0619@gmail.com>");
MODULE_DESCRIPTION("x_tables UTT HOST STATISTICS modification module");
MODULE_ALIAS("ipt_UTTHOSTSTA");

/* 清空统计信息 */
static void clearUttStat(void); 
#if 0
extern void clearUttStatsPerIp(void);
#endif
extern void uttNfStatsPerIp(struct sk_buff *skb);
#if 0
static void updateStatsByTrav(UttNfHashBuf *buf);
#endif
static void updateAndClearStatsByTrav(UttNfHashBuf *buf);
static void ipHashBVStatsClearByTrav(UttNfHashBuf *buf);
extern void clearIpBvStats(u_long ip);
extern void addIpHashBvStats(UttAppSta *uttTotalStat);
static UttAppSta utt_app_sta[UTT_APP_MAX_ARR];
#if 0
static char *utt_app_name[UTT_APP_MAX_ARR] = {"IM", "P2P", "STOCK", "VIDEO", "GAME", "SHOP", "SNS", "WEBGAME", "MAIL", 
    "FORUM", "OTHER"};
#endif

/*
 * 处理stats相关消息
 */
void uttNlStats(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    UttNlNfStatsMsg *nlData;
    nlh = nlmsg_hdr(skb);
    nlData = (UttNlNfStatsMsg *)NLMSG_DATA(nlh);
    switch(nlData->msgType)
    {
	/* 清除统计信息 */
	case UTT_STATS_CLEAR:
	    clearUttStat(); 
#if 0
	    uttNfHashTraverse(updateStatsByTrav);
#endif
	    break;
	/* 清除单个IP统计信息 */
	case UTT_STATS_SEP_CLEAR:
	    clearIpBvStats(nlData->entryVal.ip);
	    break;
	/* 更新行为统计信息 */
	case UTT_BV_STATS_UPDATE:
	    uttNfHashTraverse(updateAndClearStatsByTrav);
	    break;
	default:
	    break;
    }
    return;
}



#if 0
/*
 * 响应用户空间发送的更新用户行为netlink消息函数
 */
extern void uttNlUpdateBehavior(struct sk_buff *skb) {
    struct nlmsghdr *nlh;
    UttNlNfBvStatsMsg *nlData;
    nlh = nlmsg_hdr(skb);
    nlData = (UttNlNfBvStatsMsg *)NLMSG_DATA(nlh);

    switch(nlData->msgType)
    {
	case UTT_BV_STATS_UPDATE_CLEAR_ALL:
	    uttNfHashTraverse(updateAndClearStatsByTrav);
	    break;
	case UTT_BV_STATS_UPDATE_SEP:
	    break;
	default:
	    break;
    }
    return;
}
#endif

/*
 * 更新行为统计信息
 * clear - 1 清空主机表中netBehavior项
 * clear - 0 不清空
 */
extern void updateStatsBehavior(UttStatsPerIp *stats, int clear) {
    int i;

    /* 行为统计 */
    for (i = 0; i < (UTT_APP_MAX_ARR - 1); i++) {
	if ((stats->netBehavior)[i] != 0) {
	    /* 总行为统计 */
	    utt_app_sta[i].conns++;
	    /* 单个IP行为统计 */
	    (stats->netBVStats)[i]++;
	}
    }

    if (clear) {
	memset(stats->netBehavior, 0, UTT_APP_MAX_ARR - 1);
    }
    return;
}

#if 0
/*
 * uttNfHashTraverse回调函数，更新stats信息
 */
static void updateStatsByTrav(UttNfHashBuf *buf) {
    updateStatsBehavior(&(buf->conf.userStats), 0);
    return;
}
#endif

/*
 * uttNfHashTraverse回调函数，更新stats信息并清除
 */
static void updateAndClearStatsByTrav(UttNfHashBuf *buf) {
    updateStatsBehavior(&(buf->conf.userStats), 1);
    return;
}

/*
 * /proc/uttAppStat读取函数
 */
int uttAppStatRead( char *page, char **start, off_t off,
                           int count, int *eof, void *data) {
    int i, len = 0;
    UttAppSta utt_app_sta_tmp[UTT_APP_MAX_ARR];

    memcpy(utt_app_sta_tmp, utt_app_sta, sizeof(utt_app_sta));
    addIpHashBvStats(utt_app_sta_tmp);

    for (i = 0; i < UTT_APP_MAX_ARR; i++) {
	len += sprintf(page + len, "%llu %u\n", utt_app_sta_tmp[i].bytes, utt_app_sta_tmp[i].conns);
    }
    return len;
}


/* 清空统计信息 */
static void clearUttStat(void) {
    /* 总统计信息清除 */
    memset(utt_app_sta, 0, sizeof(utt_app_sta));
    /* 单个IP行为统计信息清除 */
    uttNfHashTraverse(ipHashBVStatsClearByTrav);
}

/*
 * 清除ip hash主机表中的行为统计信息
 */
static void ipHashBVStatsClearByTrav(UttNfHashBuf *buf) {
    memset(buf->conf.userStats.netBVStats, 0, sizeof(buf->conf.userStats.netBVStats));
    /* 清除当前行为状态 */
    memset(buf->conf.userStats.netBehavior, 0, sizeof(buf->conf.userStats.netBehavior));
    return;
}

void utt_fast_hostState(struct sk_buff *skb)
{

    uttNfStatsPerIp(skb);/*统计单个ip的信息*/
    /* 统计流量 */
    if (skb->utt_app.priFlag == 0) {
	utt_app_sta[UTT_APP_OTHER_ARR].bytes += skb->len;
    } else if ((skb->utt_app.priFlag > 0) && (skb->utt_app.priFlag <= UTT_APP_OTHER_ARR)) {
	utt_app_sta[skb->utt_app.priFlag - 1].bytes += skb->len;
    }
//    return XT_CONTINUE;
}

static unsigned int
xt_utthoststa_target4(struct sk_buff *skb,
		  const struct xt_target_param *par)
{
#if 0
    struct nf_conn *conntrack = NULL;
#endif

    uttNfStatsPerIp(skb);/*统计单个ip的信息*/
    /* 统计流量 */
    if (skb->utt_app.priFlag == 0) {
	utt_app_sta[UTT_APP_OTHER_ARR].bytes += skb->len;
    } else if ((skb->utt_app.priFlag > 0) && (skb->utt_app.priFlag <= UTT_APP_OTHER_ARR)) {
	utt_app_sta[skb->utt_app.priFlag - 1].bytes += skb->len;
    }

#if 0
    /* 统计会话 */
    if (!(conntrack = (struct nf_conn *)skb->nfct)) {
	return XT_CONTINUE;
    }

    if (!conntrack->utt_stat.counted) {
	if (conntrack->utt_app.priFlag == 0) {
	    utt_app_sta[UTT_APP_OTHER_ARR].conns += 1;
	    conntrack->utt_stat.counted = 1;
	} else if (conntrack->utt_app.priFlag > 0) {
	    utt_app_sta[conntrack->utt_app.priFlag - 1].conns += 1;
	    conntrack->utt_stat.counted = 1;
	} else {
	    ;
	}
    }
#endif

    return XT_CONTINUE;
}

static int
xt_utthoststa_checkentry4(const struct xt_tgchk_param *par)
{
        return 0;
}

static void xt_utthoststa_destroy(const struct xt_tgdtor_param *par) {

    return;
}


static struct xt_target xt_utthoststa_reg[] = {
	{
		.family		= NFPROTO_IPV4,
		.name		= "UTTHOSTSTA",
		.checkentry	= xt_utthoststa_checkentry4,
		.destroy	= xt_utthoststa_destroy,
		.target		= xt_utthoststa_target4,
		.targetsize	= sizeof(int),
		.me		= THIS_MODULE,
	},
};

static int __init xt_utthoststa_init(void)
{
    static struct proc_dir_entry *entry;
    memset(utt_app_sta, 0, sizeof(utt_app_sta));

    entry = create_proc_entry( "uttAppStat", 0644, NULL );
    if (!entry) {
	printk("uttAppStat: create failed\n");
    } else {
	entry->read_proc = uttAppStatRead;
    }

    return xt_register_targets(xt_utthoststa_reg, ARRAY_SIZE(xt_utthoststa_reg));
}

static void __exit xt_utthoststa_fini(void)
{
    remove_proc_entry("uttAppStat", NULL);
    xt_unregister_targets(xt_utthoststa_reg, ARRAY_SIZE(xt_utthoststa_reg));
}

module_init(xt_utthoststa_init);
module_exit(xt_utthoststa_fini);
