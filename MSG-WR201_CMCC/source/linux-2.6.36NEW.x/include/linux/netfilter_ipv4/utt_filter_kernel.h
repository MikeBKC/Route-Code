#ifndef __UTT_FILTER_KERNEL_H__
#define __UTT_FILTER_KERNEL_H__
#include <linux/autoconf.h>
#include <linux/if_ether.h>
#include <linux/netfilter_ipv4/utt_netfilter.h>
#include <linux/netfilter/xt_UTTLOG.h>
#include <linux/netfilter/xt_UTTHOSTSTA.h> /* 用户行为统计 */
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4/utt_limit.h>
#endif

#define MAX_UTTNF_HASH_TABLE 256u /*直接hash的表项个数*/

#define UTT_NFNL_DONE_NONE  0u  /*未进行netlink通信*/
#define UTT_NFNL_RCV_ING    1u  /*1u<<0   netlink数据接收中*/
#define UTT_NFNL_SEND_DIS   2u  /*1u<<2   netlink send disable 如用于 策略已经建立完毕的情况*/

struct dnshdr {
	__u16	id;		/* query identification number */
# ifdef BIG_ENDIAN
	/* fields in third byte */
	__u8	qr:1;		/* response flag */
	__u8	opcode:4;	/* purpose of message */
	__u8	aa:1;		/* authoritive answer */
	__u8	tc:1;		/* truncated message */
	__u8	rd:1;		/* recursion desired */
	/* fields in fourth byte */
	__u8	ra:1;		/* recursion available */
	__u8	pr:1;		/* primary server required (non standard) */
	__u8	unused:2;	/* unused bits */
	__u8	rcode:4;	/* response code */
# else
	/* fields in third byte */
	__u8	rd:1;		/* recursion desired */
	__u8	tc:1;		/* truncated message */
	__u8	aa:1;		/* authoritive answer */
	__u8	opcode:4;	/* purpose of message */
	__u8	qr:1;		/* response flag */
	/* fields in fourth byte */
	__u8	rcode:4;	/* response code */
	__u8	unused:2;	/* unused bits */
	__u8	pr:1;		/* primary server required (non standard) */
	__u8	ra:1;		/* recursion available */
# endif
	/* remaining bytes */
	__u16	qdcount;	/* number of question entries */
	__u16	ancount;	/* number of answer entries */
	__u16	nscount;	/* number of authority entries */
	__u16	arcount;	/* number of resource entries */
};
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICY
/*规则链表*/
typedef struct st_uttNfPolicylist{
    UttNfPolicyVal value;
    struct st_uttNfPolicylist *next;
}UttNfPolicyList;
typedef struct st_uttNfHashPolicy{
    unsigned int    buildStatus;/*生成状态*/
    UttNfPolicyList *policyHead;
}UttNfHashPolicy;
/*end 规则链*/
#endif

#ifdef CONFIG_UTT_SKB_IIFTYPE
typedef enum {
    DEFAULT_IFTYPE = 0,/*当不需要指定接口类型值时，可赋值为默认值0*/

    IS_WIRELESS,/*表示数据包来自无线接口*/

    MAX_IFTYPE

} IFTYPE;
#endif

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTHOSTSTA
typedef struct {
    unsigned int  rtTxBytes;
    unsigned int  tmpTxBytes;
    unsigned int  rtRxBytes;
    unsigned int  tmpRxBytes;
    unsigned long lastClrJiffies;
} RealtimeFlowStatus;
/*基于每个ip的统计*/
typedef struct st_uttStatsPerIp{
    u_long stTime; /*启动时间*/
    u_char mac[ETH_ALEN]; /*该ip对应的mac地址*/
    __u64  txBytes;/*发送数据总流量*/
    __u64  rxBytes;/*接受数据总流量*/
#if 1
    /* 用户行为统计当前状态, 不用统计其他类 */
    __u8   netBehavior[UTT_APP_MAX_ARR - 1];
    /* 单个IP行为统计信息 */
    __u32   netBVStats[UTT_APP_MAX_ARR - 1];
#endif
#ifdef CONFIG_UTT_SKB_IIFTYPE
    IFTYPE iftype;
#endif
    RealtimeFlowStatus realtimeflowS;/*实时流量统计*/
}UttStatsPerIp;
#endif
/*end 基于每个ip的统计*/

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTNETSNIPER
typedef struct st_uttNtsr{
	__u8    ntsr_flag;
	__u32   ntsr_identify_time;
	__u16   ntsr_ip_id;
	__u8   ntsr_ip_id_cnt;
	__u32  ntsr_ip_id_identify_time;
	__u8   ntsr_anti_flag;
}UttNetSniper;
#endif
 /*************start ip hash**/
/*hash表项对应的配置*/
typedef struct st_uttNfHashConf{

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICY
    UttNfHashPolicy policyConf;
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTHOSTSTA
    UttStatsPerIp   userStats;
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTLOG
    log_host        logNode;
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTNETSNIPER
	UttNetSniper  netSniper;
#endif
#ifdef CONFIG_UTT_SKB_IIFTYPE
    int iif;
#endif
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    unsigned char uttNfMatch[MAX_APP_MATCH_LEN];
#endif
}UttNfHashConf;

#ifdef CONFIG_UTT_WIFIDOG_REDIRECT
typedef struct st_wifiRedirectInfo {
    u_int                 count; /*ios/WP 连接ssid之后检测网络是否廉价的检测次数*/
    u_int                 second; /*检测到之后的运行时间*/
} wifiRedirectInfo;
#endif
/*hash buffer用于缓存hash表项*/
typedef struct st_uttNfHashBuf{
    u_long                  ip;/*ip用于hashchongtu时的比较*/
    u_int                   status;/*记录是否超时等状态*/
    UttNfHashConf           conf;/*hash表对应的配置*/
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
    unsigned int	    trigger;/*触发器*/
#endif
#ifdef CONFIG_UTT_WIFIDOG_REDIRECT
    wifiRedirectInfo      wifiInfo;
#endif
    struct st_uttNfHashBuf         *next;/*hash冲突时指向的下一个表项*/
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    atomic_t ref;/*引用计数，若不为0则无法删除。该变量目前没用。为添加可延时函数预留*/
    UttLimitConf *limitConf;/*当前IP引用的精细化限速的配置指针*/
    LimitGrpConf *limitGrpConf;/*保存限速配置(目前只有PPPoE用户使用限速组)*/
    UttLimitRunStat         runStat;
#endif
}UttNfHashBuf;
/****** end ip hash**/

extern UttNfHashBuf *uttNfHashBufGet(u_long ipaddr, int *isCreat);
extern UttNfHashBuf *uttNfHashBufGetBySkb(struct sk_buff*skb, int isCreat);
extern UttNfHashConf *uttNfHashConfGet(u_long ipaddr, int isCreat);
extern UttNfHashConf *uttNfHashConfGetBySkb(struct sk_buff* skb, int isCreat);
extern void uttNfHashTraverse(void (*processFun)(UttNfHashBuf* buf));
extern int uttNlSend(char *data, u_int dataLen, u_int dpid, u_int dgrp);
extern char *utt_strcasestr(const char *s1, const char *s2);
//extern bool isSkbComeFromLan(struct sk_buff *skb);
extern void uttNfPolicyGetTime(struct sk_buff* skb, UttNfCurTime* curTime);
extern bool uttTimeIn(u_int curTime, u_int timeStart, u_int timeStop);
#ifdef CONFIG_UTT_SKB_IIFTYPE
extern bool isWirelessIn(struct sk_buff *skb);
#endif
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
extern int uttIpMatch(IpGrpEntry *ipGrp, uint32_t ip);
extern void uttNfHashTraverseUserConf(void (*processFun)(UttNfHashBuf* buf, void *userConf), void *userConf);
extern void uttNfHashBufDel(u_long ipaddr);
#endif
#ifdef CONFIG_UTT_CLOUD_CONFIG
extern int uttNlOnRemindToUser(unsigned char mac[],unsigned int noticeBanMask);
#endif
#endif
