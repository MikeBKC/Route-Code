
#ifndef __NET_SCHED_UTT_H
#define __NET_SCHED_UTT_H

/*
 * ut smart qos
 */

enum en_uttQosDirection{
    UTT_SMART_QOS_RX,
    UTT_SMART_QOS_TX
};

struct perUsrIfFlowStatus {
    unsigned int    txByte;
    unsigned int    tmpTxByte;
    unsigned int    rxByte;
    unsigned int    tmpRxByte;
    unsigned int    lastClrJiffies;
};

#define UTT_MAX_IF_NUM 5
#define MAX_HASH_TABLE 256
#define UTT_SMART_QOS_MAX_CHAIN 0x10
#define UTT_SMART_QOS_CHAIN_NULL 0xff
#define UTT_SMART_QOS_CHAIN_LEVEL0 0
#define UTT_SMART_QOS_CHAIN_LEVEL1 1

struct netFlowStatus {
    unsigned int        sip;
    unsigned int    quoteCnt;
    struct netFlowStatus *next;
    struct netFlowStatus *prev;
    struct perUsrIfFlowStatus usrIfFlowSta[UTT_MAX_IF_NUM];
};

struct smart_hash_node {
    struct smart_hash_node *nextHashNode;
    struct smart_hash_node *nextListNode;
    struct smart_hash_node *prevListNode;
    struct sk_buff_head	qs;
    unsigned int        sip;
    unsigned int        prio;
    unsigned int        rxBytes;
    unsigned int        txBytes;
#if 0
    unsigned int        rxBytesO;
    unsigned int        txBytesO;
    unsigned int        perSecTxBytes;
    unsigned int        tmpPerSecTxBytes;
    unsigned int        perSecRxBytes;
    unsigned int        tmpPerSecRxBytes;
    unsigned int        lastClrOneSecTime;
    unsigned char       needChgChain;
    unsigned char       overflowCount;
#endif
    unsigned int        lastUseTime;
    unsigned int        lastTxUseTime;
    unsigned int        lastClrTime;
    int                 chainNo;
    unsigned short      etherProtocol;
    unsigned char       seclude;
    unsigned char       httpPackCount;
    unsigned char       lesslimitCnt;
    struct netFlowStatus    *preSecFlowStatus;
};

struct smart_list {
    struct smart_hash_node* head;
    struct smart_hash_node* curr;
};



struct utt_wan_if_info {
    int     maxRate;
    int     maxRxRate;
    int     maxTxRate;
    int     clear_interval;
    int     onlineCliCount;
    int     avgRate;
    int     avgRxRate;
    int     perSecPerIpRxBytes;
    int     avgTxRate;
    int     perSecPerIpTxBytes;
    int     maxTimeout;
    unsigned char     oneSecCount;
    struct smart_hash_node* smartQosHash[MAX_HASH_TABLE];
    int		qos_pertick_rxbytes;	/* 	*/
    int		qos_pertick_txbytes;	/* 	*/
    int		qos_down_drop;	/* 	*/
    int		qos_up_drop;	/* 	*/
    int		webRxBytes;	/* 	*/
    unsigned int lastRxWeb;
};

#define UTT_SMART_QOS_MAX_QS    0x10
#define UTT_SMART_QOS_BIG_QS    0x0
#define UTT_SMART_QOS_LITTLE_QS    0x1
struct smart_sched_data
{
/* Parameters */
	int		limit;

/* Variables */
	struct timer_list perturb_timer;
        struct sk_buff_head     qs[UTT_SMART_QOS_MAX_QS];

	struct smart_list	bigRateHead;		/* Slot queue */
	struct smart_list	littleRateHead;		/* Slot queue */
	struct smart_list	rateChain[UTT_SMART_QOS_MAX_CHAIN];		/* Slot queue */
        struct smart_hash_node  webPriNode;
        struct smart_hash_node  priNode;
        unsigned int            lastTxUseTime;
};

extern void statisPerSecPerUserFlowSta(struct smart_hash_node *hashNode, 
        int ifIndex, enum en_uttQosDirection dir, unsigned int len);
extern void decUserFlowStatusQuoteCnt(struct smart_hash_node *hashNode);
#endif
