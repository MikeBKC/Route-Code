#ifndef __UTT_FILTER_KERNEL_H__
#define __UTT_FILTER_KERNEL_H__
#include <linux/autoconf.h>
#include <linux/if_ether.h>
#include <linux/netfilter_ipv4/utt_netfilter.h>
#include <linux/netfilter/xt_UTTLOG.h>
#include <linux/netfilter/xt_UTTHOSTSTA.h> /* �û���Ϊͳ�� */
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4/utt_limit.h>
#endif

#define MAX_UTTNF_HASH_TABLE 256u /*ֱ��hash�ı������*/

#define UTT_NFNL_DONE_NONE  0u  /*δ����netlinkͨ��*/
#define UTT_NFNL_RCV_ING    1u  /*1u<<0   netlink���ݽ�����*/
#define UTT_NFNL_SEND_DIS   2u  /*1u<<2   netlink send disable ������ �����Ѿ�������ϵ����*/

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
/*��������*/
typedef struct st_uttNfPolicylist{
    UttNfPolicyVal value;
    struct st_uttNfPolicylist *next;
}UttNfPolicyList;
typedef struct st_uttNfHashPolicy{
    unsigned int    buildStatus;/*����״̬*/
    UttNfPolicyList *policyHead;
}UttNfHashPolicy;
/*end ������*/
#endif

#ifdef CONFIG_UTT_SKB_IIFTYPE
typedef enum {
    DEFAULT_IFTYPE = 0,/*������Ҫָ���ӿ�����ֵʱ���ɸ�ֵΪĬ��ֵ0*/

    IS_WIRELESS,/*��ʾ���ݰ��������߽ӿ�*/

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
/*����ÿ��ip��ͳ��*/
typedef struct st_uttStatsPerIp{
    u_long stTime; /*����ʱ��*/
    u_char mac[ETH_ALEN]; /*��ip��Ӧ��mac��ַ*/
    __u64  txBytes;/*��������������*/
    __u64  rxBytes;/*��������������*/
#if 1
    /* �û���Ϊͳ�Ƶ�ǰ״̬, ����ͳ�������� */
    __u8   netBehavior[UTT_APP_MAX_ARR - 1];
    /* ����IP��Ϊͳ����Ϣ */
    __u32   netBVStats[UTT_APP_MAX_ARR - 1];
#endif
#ifdef CONFIG_UTT_SKB_IIFTYPE
    IFTYPE iftype;
#endif
    RealtimeFlowStatus realtimeflowS;/*ʵʱ����ͳ��*/
}UttStatsPerIp;
#endif
/*end ����ÿ��ip��ͳ��*/

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
/*hash�����Ӧ������*/
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
    u_int                 count; /*ios/WP ����ssid֮���������Ƿ����۵ļ�����*/
    u_int                 second; /*��⵽֮�������ʱ��*/
} wifiRedirectInfo;
#endif
/*hash buffer���ڻ���hash����*/
typedef struct st_uttNfHashBuf{
    u_long                  ip;/*ip����hashchongtuʱ�ıȽ�*/
    u_int                   status;/*��¼�Ƿ�ʱ��״̬*/
    UttNfHashConf           conf;/*hash���Ӧ������*/
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
    unsigned int	    trigger;/*������*/
#endif
#ifdef CONFIG_UTT_WIFIDOG_REDIRECT
    wifiRedirectInfo      wifiInfo;
#endif
    struct st_uttNfHashBuf         *next;/*hash��ͻʱָ�����һ������*/
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    atomic_t ref;/*���ü���������Ϊ0���޷�ɾ�����ñ���Ŀǰû�á�Ϊ��ӿ���ʱ����Ԥ��*/
    UttLimitConf *limitConf;/*��ǰIP���õľ�ϸ�����ٵ�����ָ��*/
    LimitGrpConf *limitGrpConf;/*������������(Ŀǰֻ��PPPoE�û�ʹ��������)*/
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
