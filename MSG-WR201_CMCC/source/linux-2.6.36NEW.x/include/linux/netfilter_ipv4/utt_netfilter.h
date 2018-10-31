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
/*UTT_MAX_KPDB_POLICY * 8 - 1��-1 ����Ϊҳ���Ǵ�1��ʼ�� ��0λԤ��) �֡�Ϊ��Ч������ֱ��д����*/     
#define UTT_MAX_KPDB_APPFLAGS   255 
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICYROUTE
#define UTT_ROUTE_NAME_LEN 31
#endif
#define ETH_ALEN 6
/*utt netfilter netlink type*/
typedef enum{
    UTT_NLNF_HASH = 0,  /*����hash������*/

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICY
    UTT_NLNF_POLICY,/*���˹�������*/
#endif

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTWEBIND
    UTT_NLNF_DNS,   /*������������*/
#endif

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTHOSTSTA
    UTT_NLNF_STATS, /*ͳ��*/
#endif

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTEXCEPTQQ
    UTT_NLNF_QQ,/*QQ*/
    UTT_NLNF_ENTERPRISE_QQ,/*��ҵQQ*/
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
    UTT_NLNF_POLICY_ROUTE,/*���˹�������*/
#endif
#ifdef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
    UTT_NLNF_WIFIDOG_DNS_WHITE,           /*wifidog����������*/
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
    UTT_NF_HASH_DES_NOTIFY = 0,/*����ʱ����֪ͨ*/
    UTT_NF_HASH_WEB_AUTH_DES_NOTIFY, /*web��֤����֪ͨ*/
    UTT_NF_HASH_WIFIDOG_DES_NOTIFY, /*wifidog��֤����֪ͨ*/
}UttNfHashTrigger;
#endif

#ifdef CONFIG_UTT_CLOUD_CONFIG
/*�����������ѷ��͵����ݽṹ*/
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
    /*����hash���û�ֻ��Ҫɾ������
     * ���������Ͳ��������ô�����*/
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
/*�����б���Ŀֵ*/
typedef struct st_dnsEntryVal{
    char    domainName[MAX_DOMAIN_NAME_LENGTH + 1]; 
    IpEntry *ipEntry;
} DnsEntryVal;
/*utt netfilter dns tables config*/
typedef struct st_uttNlNfwifiDnsConf{
    UttNlNfMsgHead msgHead;/*ͷ*/
    UttNfConfType  confType;/*��������*/
    DnsEntryVal  entryVal;/*ֵ*/
}UttNlNfwifiDnsConf;
#endif


#ifdef CONFIG_WEBAUTH_MAC_VALIDATE
void uttNlWifidogSend(int type, unsigned int ip, unsigned char *mac);
#endif

/* move from utt_filter_config.c (ReOS SE 1.4 for policy route)*/
typedef struct st_uttNfCurTime{
    u_int time;/*ת��Ϊ���� ʱ���뼯��*/
    u_char week;/*����*/
}UttNfCurTime;
/*������Чʱ��*/
typedef struct st_uttNfTime{
    u_long wday;/*���ڼ�*/
    u_long timestart;/*24*60*60.һ�����86400��*/
    u_long timestop;
}UttNfTime;
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICY
/*utt netfilter application flags set*/
typedef struct st_uttNfApps{
   u_char appFlags[UTT_MAX_KPDB_POLICY];/*ÿλһ������*/
}UttNfApps;
/*����ֵ*/
typedef struct st_uttNfPolicyVal{
    UttNfTime nfTime;/*ʱ��*/
    UttNfApps nfApps;/*������ЩӦ��*/
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
/*�����б���Ŀֵ*/
typedef struct st_dnsFilterEntryVal{
    char    domainName[MAX_DOMAIN_NAME_LENGTH + 1]; 
    int     priType;/*�����͡���ͳ��ʱ���͵�����*/
    int     secType;/*������ Ϊ appFlags��ֵ*/
} DnsFilterEntryVal;

/*utt netfilter dns tables config*/
typedef struct st_uttNlNfDnsConf{
    UttNlNfMsgHead msgHead;/*ͷ*/
    UttNfConfType  confType;/*��������*/
    DnsFilterEntryVal  entryVal;/*ֵ*/
}UttNlNfDnsConf;
#endif

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTEXCEPTQQ
/*QQ����*/
typedef struct st_ExceptQQEntryVal{
    char qqNumber[20 + 1]; 
} ExceptQQEntryVal;

/*utt netfilter qq config*/
typedef struct st_uttNlNfQQConf{
    UttNlNfMsgHead msgHead;/*ͷ*/
    UttNfConfType  confType;/*��������*/
    ExceptQQEntryVal entryVal;/*ֵ*/
}UttNlNfQQConf;

/*��ҵQQ����*/
typedef struct st_enterpriseQQEntryVal{
    __u16  qqValue;
    struct st_enterpriseQQEntryVal *next;
} enterpriseQQEntryVal;

/*utt netfilter qq config*/
typedef struct st_uttNlNfEnterpriseQQConf{
    UttNlNfMsgHead msgHead;/*ͷ*/
    UttNfConfType  confType;/*��������*/
    enterpriseQQEntryVal entryVal;/*ֵ*/
}UttNlNfEnterpriseQQConf;
#endif


#ifdef CONFIG_NETFILTER_XT_TARGET_UTTEXCEPTMSN
/*MSN����*/
typedef struct st_ExceptMsnEntryVal{
    char msnNumber[32 + 1]; 
} ExceptMsnEntryVal;

/*utt netfilter msn config*/
typedef struct st_uttNlNfMsnConf{
    UttNlNfMsgHead msgHead;/*ͷ*/
    UttNfConfType  confType;/*��������*/
    ExceptMsnEntryVal entryVal;/*ֵ*/
}UttNlNfMsnConf;
#endif

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTEXCEPTALI
/*aliww����*/
typedef struct st_ExceptAliEntryVal{
    char aliNumber[25 + 1]; 
} ExceptAliEntryVal;

/*utt netfilter aliww config*/
typedef struct st_uttNlNfAliConf{
    UttNlNfMsgHead msgHead;/*ͷ*/
    UttNfConfType  confType;/*��������*/
    ExceptAliEntryVal entryVal;/*ֵ*/
}UttNlNfAliConf;
#endif

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTHOSTSTA
/* stats msg value */
typedef struct st_StatsEntryVal {
    u_long  ip;/* ������ */
} StatsEntryVal;
/* stats update type */
typedef enum {
    UTT_STATS_CLEAR = 0, /* �����ͳ�� */
    UTT_STATS_SEP_CLEAR, /* �������IPͳ�� */
    UTT_BV_STATS_UPDATE,    /* ������Ϊͳ�� */
} UttNlNfStatsType;
/* utt stats msg */
typedef struct st_uttNlNfStatsMsg {
    UttNlNfMsgHead msgHead;/*ͷ*/
    UttNlNfStatsType msgType;
    StatsEntryVal entryVal;
} UttNlNfStatsMsg;
#endif
/*����ip*/
typedef struct st_EimEntryVal{
    unsigned int button;
#ifdef CONFIG_XPORT_LOG
    unsigned int natLogEnable;
#endif
} EimEntryVal;

/*Employee Internet Management*/
typedef struct st_uttNlNfEimConf{
    UttNlNfMsgHead msgHead;/*ͷ*/
    EimEntryVal entryVal;/*ֵ*/
}UttNlNfEimConf;

#ifdef	CONFIG_UTT_DEBUG
/*Debug type*/
typedef enum{
    UTT_DEBUG_DEFAULT = 0,
#if CONFIG_NETFILTER_XT_TARGET_UTTDNSREDIRECT
    UTT_DEBUG_DNS_NOTICE,  /*����hash������*/
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICYROUTE
#ifdef CONFIG_UTT_DEB_POLICY_ROUTE
    UTT_DEBUG_POLICY_ROUTE,  /*����·������*/
#endif
#endif
#ifdef CONFIG_UTT_DHCP_COLLISION
    UTT_DEBUG_DHCP_COLLISION,
#endif
}UttDebugType;
/*���е�������flag*/
typedef struct st_DebugEntryVal{
    UttDebugType debugFlag;
} DebugEntryVal;

/*��������ṹ��*/
typedef struct st_uttNlNfDebugConf{
    UttNlNfMsgHead msgHead;/*ͷ*/
    UttNfConfType  confType;/*����*/
    DebugEntryVal entryVal;/*ֵ*/
}UttNlNfDebugConf;

#endif

/*IP GRP*/
#define UTT_IPGRP_NAME_LEN 31
/* ip type */
typedef enum{
    IP_RANGE,
    IP_GROUP
}IpGrpType;
/* ip ��ַ�� */
typedef struct st_IpRange {
    u_long ipFrom;	/*��ʼip*/
    u_long ipTo;	/*����ip*/
} IpRange;
/*ip����Ŀ����*/
typedef union{
    unsigned char macAddr[6];
    IpRange range;/*ip��*/
    unsigned int index;
    char name[UTT_IPGRP_NAME_LEN + 1];/*���� ����Ϊ������ �û���*/
} IpGrpVal;

/*ip����Ŀ*/
typedef struct st_IpGrpEntry{
    IpGrpType entryType;/*����*/
    IpGrpVal  entryVal;/*ֵ*/
} IpGrpEntry;

/*����·�ɻ���IP��ַ*/
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

/*����·�ɻ���Ӧ��*/
typedef struct st_uttNfApply {
    unsigned int protocol;//Э��
    unsigned short dstFromPort; //Ŀ����ʼ�˿�
    unsigned short dstEndPort;	//Ŀ�Ľ����˿�
} UttNfApply;

/* policy route value */
typedef struct st_PolicyRouteEntryVal {
    int bindMark;			/* �󶨽ӿ�mark */
    UttNfIpGrp ip;			/* ����IP */
    UttNfApply apply;			/* ����Ӧ�� */
    UttNfTime prTime;			/* ��Чʱ�� */
    struct st_PolicyRouteEntryVal *next;
} PolicyRouteEntryVal;
/* utt netlink policy route config */
typedef struct st_UttNlNfPolicyRouteConf {
    UttNlNfMsgHead msgHead;/*ͷ*/
    UttNfConfType  confType;/*��������*/
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
	UttNlNfMsgHead msgHead;/*ͷ*/
	UttNfConfType  confType;/*��������*/
	NetSniperEntryVal netSniperEntryVal;
	char delstr[200];
}UttNlNfNetSniperConf;
#endif
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTDNSREDIRECT
extern void send_dns_redirect(struct sk_buff *oldskb, __u32 redirectIp);
#endif
#endif
