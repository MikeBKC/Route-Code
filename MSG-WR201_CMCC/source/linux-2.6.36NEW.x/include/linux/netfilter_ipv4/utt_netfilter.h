#ifndef __UTT_NETFILTER_H__
#define __UTT_NETFILTER_H__

#include <linux/types.h>
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
#define UTT_MAX_NLMSGSIZE       4096
#else
#define UTT_MAX_NLMSGSIZE       1024
#endif

#define MAX_DOMAIN_NAME_LENGTH  63


#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICY
#define UTT_MAX_KPDB_POLICY     32
/*UTT_MAX_KPDB_POLICY * 8 - 1（-1 是因为页面是从1开始的 即0位预留) 种。为了效率这里直接写数字*/     
#define UTT_MAX_KPDB_APPFLAGS   255 
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICYROUTE
#define UTT_ROUTE_NAME_LEN 31
#endif
#define ETH_ALEN 6
/*utt netfilter netlink type*/
typedef enum{
    UTT_NLNF_HASH = 0,  /*公用hash表配置*/

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICY
    UTT_NLNF_POLICY,/*过滤规则设置*/
#endif

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTWEBIND
    UTT_NLNF_DNS,   /*域名过滤配置*/
#endif

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTHOSTSTA
    UTT_NLNF_STATS, /*统计*/
#endif

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTEXCEPTQQ
    UTT_NLNF_QQ,/*QQ*/
    UTT_NLNF_ENTERPRISE_QQ,/*企业QQ*/
#endif

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTEXCEPTMSN
    UTT_NLNF_MSN,/*MSN*/
#endif

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTNETSNIPER
	    UTT_NLNF_NET_SNIPER,/*NETSNIPER*/
#endif

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTEXCEPTALI
    UTT_NLNF_ALI,/*ALIWW*/
#endif

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTLOG
    UTT_NLNF_LOG, /* utt err log msg */
    UTT_NLNF_EIM,
#endif
#ifdef	CONFIG_UTT_DEBUG
    UTT_NLNF_DEBUG,
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICYROUTE
    UTT_NLNF_POLICY_ROUTE,/*过滤规则设置*/
#endif
#ifdef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
    UTT_NLNF_WIFIDOG_DNS_WHITE,           /*wifidog域名白名单*/
#endif
    UTT_NLNF_WIFIDOG,
#ifdef CONFIG_MULTI_SSID_DHCP_POOL
        UTT_MULTI_SSID_DHCP_POOL,
#endif
#ifdef CONFIG_UTT_CLOUD_CONFIG
	UTT_NLNF_ONLIN_REMIND,
#endif
}UttNlNfType;


/*utt netfilter rule config*/
typedef enum{
    UTT_NFCONF_INIT = 0,
    UTT_NFCONF_ADD ,/*add one rule*/
    UTT_NFCONF_DEL,/*del one rule*/
    UTT_NFCONF_DEL_ALL, /*del all rule*/
    UTT_NFCONF_EDIT,
	UTT_NFCONF_FINI
}UttNfConfType;
typedef enum{
    UTT_NF_WIFIDOG_INIT = 0,
    UTT_NF_WIFIDOG_WECHAT_ATTENTION,
    UTT_NF_WIFIDOG_ARP_ADD,
}UttNfWifidogConfType;

/*utt netfilter netlink message head*/
typedef struct st_uttNlNfMsgHead{
    UttNlNfType nlType;/*head type*/
} UttNlNfMsgHead;

/*utt netfilter netlink general config*/
typedef struct st_uttNlNfGeneral{
    UttNlNfMsgHead msgHead;
    UttNfConfType  confType;
}UttNlNfGeneral;

#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
typedef enum{
    UTT_NF_HASH_DES_NOTIFY = 0,/*销毁时发出通知*/
    UTT_NF_HASH_WEB_AUTH_DES_NOTIFY, /*web认证下线通知*/
    UTT_NF_HASH_WIFIDOG_DES_NOTIFY, /*wifidog认证下线通知*/
}UttNfHashTrigger;
#endif

#ifdef CONFIG_UTT_CLOUD_CONFIG
/*增加上线提醒发送的数据结构*/
typedef struct st_uttNlNfOnRemind{
    UttNlNfMsgHead msgHead;
    unsigned int noticeBanMask;
    unsigned char newMac[ETH_ALEN];
}UttNlNfOnRemind;
#endif


/*utt netfilter hash tables config*/
typedef struct st_uttNlNfHashConf{
    UttNlNfMsgHead msgHead;
#if 1
    /*现在hash表用户只需要删除操作
     * 若更多类型操作需配置此类型*/
    UttNfConfType confType;
#endif
    u_long ip;
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
    unsigned int trigger;
#endif
    int staleEn;
    unsigned int staleTime;
}UttNlNfHashConf;
typedef struct st_uttNlNfWifidogConf{
    UttNlNfMsgHead msgHead;
    UttNfWifidogConfType confType;
    u_long ip;
    unsigned char mac[6];
}UttNlNfWifidogConf;
#ifdef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
#define  WHITE_LIST  "wifi_whitelist"
typedef struct st_dnsIplist {
    __u32 ip;
    struct st_dnsIplist *next;
} IpEntry;
/*域名列表条目值*/
typedef struct st_dnsEntryVal{
    char    domainName[MAX_DOMAIN_NAME_LENGTH + 1]; 
    IpEntry *ipEntry;
} DnsEntryVal;
/*utt netfilter dns tables config*/
typedef struct st_uttNlNfwifiDnsConf{
    UttNlNfMsgHead msgHead;/*头*/
    UttNfConfType  confType;/*操作类型*/
    DnsEntryVal  entryVal;/*值*/
}UttNlNfwifiDnsConf;
#endif


#ifdef CONFIG_WEBAUTH_MAC_VALIDATE
void uttNlWifidogSend(int type, unsigned int ip, unsigned char *mac);
#endif

/* move from utt_filter_config.c (ReOS SE 1.4 for policy route)*/
typedef struct st_uttNfCurTime{
    u_int time;/*转换为秒后的 时分秒集合*/
    u_char week;/*星期*/
}UttNfCurTime;
/*规则生效时间*/
typedef struct st_uttNfTime{
    u_long wday;/*星期几*/
    u_long timestart;/*24*60*60.一天最多86400秒*/
    u_long timestop;
}UttNfTime;
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICY
/*utt netfilter application flags set*/
typedef struct st_uttNfApps{
   u_char appFlags[UTT_MAX_KPDB_POLICY];/*每位一个策略*/
}UttNfApps;
/*规则值*/
typedef struct st_uttNfPolicyVal{
    UttNfTime nfTime;/*时间*/
    UttNfApps nfApps;/*过滤哪些应用*/
} UttNfPolicyVal;
/*utt netfilter config netlink message struct*/
typedef struct st_uttNlNfPolicy{
    UttNlNfMsgHead msgHead;
    UttNfConfType  confType;
    u_long ip;
    UttNfPolicyVal  policyVal;

}UttNlNfPolicy;
#endif

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTWEBIND
/*域名列表条目值*/
typedef struct st_dnsFilterEntryVal{
    char    domainName[MAX_DOMAIN_NAME_LENGTH + 1]; 
    int     priType;/*主类型。如统计时类型的区分*/
    int     secType;/*次类型 为 appFlags的值*/
} DnsFilterEntryVal;

/*utt netfilter dns tables config*/
typedef struct st_uttNlNfDnsConf{
    UttNlNfMsgHead msgHead;/*头*/
    UttNfConfType  confType;/*操作类型*/
    DnsFilterEntryVal  entryVal;/*值*/
}UttNlNfDnsConf;
#endif

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTEXCEPTQQ
/*QQ号码*/
typedef struct st_ExceptQQEntryVal{
    char qqNumber[20 + 1]; 
} ExceptQQEntryVal;

/*utt netfilter qq config*/
typedef struct st_uttNlNfQQConf{
    UttNlNfMsgHead msgHead;/*头*/
    UttNfConfType  confType;/*操作类型*/
    ExceptQQEntryVal entryVal;/*值*/
}UttNlNfQQConf;

/*企业QQ号码*/
typedef struct st_enterpriseQQEntryVal{
    __u16  qqValue;
    struct st_enterpriseQQEntryVal *next;
} enterpriseQQEntryVal;

/*utt netfilter qq config*/
typedef struct st_uttNlNfEnterpriseQQConf{
    UttNlNfMsgHead msgHead;/*头*/
    UttNfConfType  confType;/*操作类型*/
    enterpriseQQEntryVal entryVal;/*值*/
}UttNlNfEnterpriseQQConf;
#endif


#ifdef CONFIG_NETFILTER_XT_TARGET_UTTEXCEPTMSN
/*MSN号码*/
typedef struct st_ExceptMsnEntryVal{
    char msnNumber[32 + 1]; 
} ExceptMsnEntryVal;

/*utt netfilter msn config*/
typedef struct st_uttNlNfMsnConf{
    UttNlNfMsgHead msgHead;/*头*/
    UttNfConfType  confType;/*操作类型*/
    ExceptMsnEntryVal entryVal;/*值*/
}UttNlNfMsnConf;
#endif

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTEXCEPTALI
/*aliww号码*/
typedef struct st_ExceptAliEntryVal{
    char aliNumber[25 + 1]; 
} ExceptAliEntryVal;

/*utt netfilter aliww config*/
typedef struct st_uttNlNfAliConf{
    UttNlNfMsgHead msgHead;/*头*/
    UttNfConfType  confType;/*操作类型*/
    ExceptAliEntryVal entryVal;/*值*/
}UttNlNfAliConf;
#endif

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTHOSTSTA
/* stats msg value */
typedef struct st_StatsEntryVal {
    u_long  ip;/* 网络序 */
} StatsEntryVal;
/* stats update type */
typedef enum {
    UTT_STATS_CLEAR = 0, /* 清除总统计 */
    UTT_STATS_SEP_CLEAR, /* 清除单个IP统计 */
    UTT_BV_STATS_UPDATE,    /* 更新行为统计 */
} UttNlNfStatsType;
/* utt stats msg */
typedef struct st_uttNlNfStatsMsg {
    UttNlNfMsgHead msgHead;/*头*/
    UttNlNfStatsType msgType;
    StatsEntryVal entryVal;
} UttNlNfStatsMsg;
#endif
/*调试ip*/
typedef struct st_EimEntryVal{
    unsigned int button;
#ifdef CONFIG_XPORT_LOG
    unsigned int natLogEnable;
#endif
} EimEntryVal;

/*Employee Internet Management*/
typedef struct st_uttNlNfEimConf{
    UttNlNfMsgHead msgHead;/*头*/
    EimEntryVal entryVal;/*值*/
}UttNlNfEimConf;

#ifdef	CONFIG_UTT_DEBUG
/*Debug type*/
typedef enum{
    UTT_DEBUG_DEFAULT = 0,
#if CONFIG_NETFILTER_XT_TARGET_UTTDNSREDIRECT
    UTT_DEBUG_DNS_NOTICE,  /*公用hash表配置*/
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICYROUTE
#ifdef CONFIG_UTT_DEB_POLICY_ROUTE
    UTT_DEBUG_POLICY_ROUTE,  /*策略路由配置*/
#endif
#endif
#ifdef CONFIG_UTT_DHCP_COLLISION
    UTT_DEBUG_DHCP_COLLISION,
#endif
}UttDebugType;
/*所有调试命令flag*/
typedef struct st_DebugEntryVal{
    UttDebugType debugFlag;
} DebugEntryVal;

/*调试命令结构体*/
typedef struct st_uttNlNfDebugConf{
    UttNlNfMsgHead msgHead;/*头*/
    UttNfConfType  confType;/*类型*/
    DebugEntryVal entryVal;/*值*/
}UttNlNfDebugConf;

#endif

/*IP GRP*/
#define UTT_IPGRP_NAME_LEN 31
/* ip type */
typedef enum{
    IP_RANGE,
    IP_GROUP
}IpGrpType;
/* ip 地址段 */
typedef struct st_IpRange {
    u_long ipFrom;	/*起始ip*/
    u_long ipTo;	/*结束ip*/
} IpRange;
/*ip组条目内容*/
typedef union{
    unsigned char macAddr[6];
    IpRange range;/*ip段*/
    unsigned int index;
    char name[UTT_IPGRP_NAME_LEN + 1];/*名称 可能为组名或 用户名*/
} IpGrpVal;

/*ip组条目*/
typedef struct st_IpGrpEntry{
    IpGrpType entryType;/*类型*/
    IpGrpVal  entryVal;/*值*/
} IpGrpEntry;

/*策略路由基于IP地址*/
typedef struct st_uttNfIpRange {
	IpGrpEntry srcIP;
	IpGrpEntry dstIP;
} UttNfIpGrp;

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICYROUTE
#ifdef CONFIG_UTT_DEBUG
#ifdef CONFIG_UTT_DEB_POLICY_ROUTE
typedef enum {
    UTT_DEBUG_INIT = 0, /*  */
    UTT_DEBUG_LIST_ALL = 1, /*  */
} UttNlNfDebugType;
#endif
#endif

/*策略路由基于应用*/
typedef struct st_uttNfApply {
    unsigned int protocol;//协议
    unsigned short dstFromPort; //目的起始端口
    unsigned short dstEndPort;	//目的结束端口
} UttNfApply;

/* policy route value */
typedef struct st_PolicyRouteEntryVal {
    int bindMark;			/* 绑定接口mark */
    UttNfIpGrp ip;			/* 基于IP */
    UttNfApply apply;			/* 基于应用 */
    UttNfTime prTime;			/* 生效时间 */
    struct st_PolicyRouteEntryVal *next;
} PolicyRouteEntryVal;
/* utt netlink policy route config */
typedef struct st_UttNlNfPolicyRouteConf {
    UttNlNfMsgHead msgHead;/*头*/
    UttNfConfType  confType;/*操作类型*/
#ifdef CONFIG_UTT_DEBUG
#ifdef CONFIG_UTT_DEB_POLICY_ROUTE
    UttNlNfDebugType debugType;
#endif
#endif
    PolicyRouteEntryVal entryVal;
} UttNlNfPolicyRouteConf;
#endif

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTNETSNIPER
typedef struct st_NetSniperEntryVal {
	__u32 ctlTime;
	__u32 linearIntvl;
	__u32 chkIntvl;
	__u32 chkCnt;
}NetSniperEntryVal;
typedef struct st_UttNlNfNetSniperConf {
	UttNlNfMsgHead msgHead;/*头*/
	UttNfConfType  confType;/*操作类型*/
	NetSniperEntryVal netSniperEntryVal;
	char delstr[200];
}UttNlNfNetSniperConf;
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTDNSREDIRECT
extern void send_dns_redirect(struct sk_buff *oldskb, __u32 redirectIp);
#endif
#endif
