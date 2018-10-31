#ifndef MIB_PPPOESERVER_SHARE_H
#define MIB_PPPOESERVER_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"

#define ONLYPPPOE_FORWARD "onlypppoe_forward"
#define UTT_PPP_CLIENT_MAC_LEN      20
#define UTT_PPP_CLIENT_IP_LEN       20
#define UTT_PPP_CLIENT_IFNAME_LEN   20
#define UTT_PPP_CLIENT_DEVICE_LEN   20
#define UTT_PPP_CLIENT_OURNAME_LEN  20
#define UTT_PPPOE_ACCMODE_LEN       20
#define UTT_PPPOE_MAC_TYPE          8
#if (PPPOE_SELF_HELP == FYES)
#define MAX_USER_OPT_CNT	    5
#endif
#if (PPPOE_LIMIT == FYES)
#define PPPOEACC_LIMIT_CHAIN "pppoeacc_limit"
#define POE_SKIP_FIXED_LIMIT IPSET_SYS_GNAME_PRE"poe_skip_fixed_limit"
#endif

#if (PPPOE_ACCOUNT == FYES)
#define POE_DROP_CHAIN    "poe_drop_chain"   /*过期账号丢包链表*/
#if (NOTICE == FYES)
/*expiring account*/
#define POE_EXPIRING_ACC  IPSET_SYS_GNAME_PRE"poe_expiring_acc" /*将过期账号集合*/
#define POE_NT_OK         IPSET_SYS_GNAME_PRE"poe_nt_ok"       /*已经通告过的用户*/
#define POE_NT_R          IPSET_SYS_GNAME_PRE"poe_nt_r"         /*需通告用户记录，http回包时改包*/
#define POE_NT_CAP_CHAIN  "poe_nt_cap_chain"  /*将到期通告规则链表*/
#define POE_NT_R_CHAIN    "poe_nt_r_chain"    /*将到期通告回包，改包链表*/

/*expired account*/
#define POE_EXPIRED_WEB IPSET_SYS_GNAME_PRE"poe_expired_web"
#define POE_NT_DROP_CHAIN "poe_nt_drop_chain" /*到期用户drop链表*/
#define POE_NT_DROP_R_CHAIN "poe_nt_drop_r_chain" /*到期用户drop链表*/
#endif


#define POE_EXPIRED_ACC IPSET_SYS_GNAME_PRE"poe_expired_acc"
typedef enum{
    PPPOE_ACC_NORMAL =  0,
    PPPOE_ACC_EXPIRING,
    PPPOE_ACC_EXPIRED
}emPppoeAccStatus;
#endif
typedef struct st_SessionList{
    unsigned int        pid; 
    unsigned int        STime;
    unsigned int        LTime;
    unsigned int        ConTime;/*0表示在线，1表示离线*/
    unsigned long long  rxByte;
    unsigned long long  txByte;
    char                mac[UTT_PPP_CLIENT_MAC_LEN + 1]; 
    char		ip[UTT_PPP_CLIENT_IP_LEN + 1];
    char                ifname[UTT_PPP_CLIENT_IFNAME_LEN + 1]; 
    char                device[UTT_PPP_CLIENT_DEVICE_LEN + 1]; 
    char                ourName[UTT_PPP_CLIENT_OURNAME_LEN + 1];
    short		nextIndex;/*前一个会话存储位置下标(初始化为-1)*/
    short		accIndex;/*此profile对应的账号索引*/
} SessionList;

#if (PPPOE_LIMIT == FYES)
/*
 * ipGrpName	-   用户组组名
 * count	-   当前组被使用次数
 * uRateLimit	-   上传限制速率
 * dRateLimit	-   下载限制速率
 */
typedef struct st_pppoeAccLimit {
    char    ipGrpName[IP_GROUP_NAME_LEN + 1u];
    int	    count;
    Uint32  uRateLimit;
    Uint32  dRateLimit; 
} PppoeAccLimit;
#endif

/*
 * pppoe server全局配置结构体
 * ipStart	-   起始IP地址
 * self_help	-   启用用户自助服务  
 * pDns		-   主DNS
 * sDns		-   备DNS 
 * pppAuthMode	-   密码验证方式
 * maxSessCnt	-   系统最大会话数
 * exceptIpGroup-   例外地址组
 * limitGroup	-   pppoe账号限速组信息
 */
typedef struct st_pppoeSrvGloProfile {
    mibHead		head;
    int			onlyPppoe;
    IPADDR		ipStart;
#if (PPPOE_SELF_HELP == FYES)
    FunEnableEnum	self_help;
#endif
    IPADDR		pDns;
    IPADDR		sDns;
    ProfPppAuthEnum	pppAuthMode;
    int			maxSessCnt;
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
    Uint32      ipcount;//可分配最大地址池数目
#endif
    char		exceptIpGroup[IP_GROUP_NAME_LEN + 1];
#if (PPPOE_LIMIT == FYES)
    PppoeAccLimit	limitGroup[MAX_PPPOE_LIMIT_IPGRP];
#endif
} PPPoESrvGloProfile;


/*
 * pppoe server账号配置
 * pppoename	-   账号
 * passwd	-   密码
 * bindIp	-   固定IP分配 
 * accmaxconv   -   账号最大会话数
 * AccMACBind   -   账号绑定方式
 * bindmac      -   账号MAC绑定
 * chargeType;	-   计费方式
 * tmStart;	-   起始日期
 * tmStop;	-   结束日期
 * expired;	-   记录过期状态，过期、将过期、正常
 * pid;		-   ppp进程号
 * STime;	-   ppp连接上时，系统启动时间
 * LTime;	-   上一次保存时，系统启动时间，在交互中会修改
 * ConTime;	-   ppp是否连接，为0表示连接，非0表示断开
 * optCnt	-   用户自助服务修改次数 
 * rxByte;	-   ppp进程rx统计，在交互中设置
 * txByte;	-   tx统计，在交换中设置
 * mac		-   pppoe连接用户的mac地址
 * ip;		-   分配的IP地址
 * ifname;	-   接口名称，如ppp0等
 * device	-   物理接口名称，如br0,eth2.1等
 * ourName	-   服务名称，PPPoE Server对应的服务名称是pppoeSrv
 * uRateLimit	-   上传限制速率
 * dRateLimit	-   下载限制速率
 */
typedef struct st_pppoeSrvAccProfile {
    mibHead		head;
    char		pppoename[UTT_PPPOE_NAME_LEN+1];
    char		passwd[UTT_PPPOE_CLI_PASS_LEN+ 1];
    char                remark[UTT_REMARK_LEN + 1];
    IPADDR		bindIp;
    Uint32		uRate;
    Uint32		dRate;
    int		        accmaxconv;/*账号最大会话数（prof初始化时初始化为1）*/
    char                AccMACBind[UTT_PPPOE_MAC_TYPE + 1];
    MacAddr             bindmac;
    MacAddr             bindmac2;
    MacAddr             bindmac3;
    MacAddr             bindmac4;
#if (PPPOE_ACCOUNT == FYES)
    ProfchargeTypeEnum  chargeType;	
    Uint32		tmStart;			
    Uint32		tmStop;		
    emPppoeAccStatus	expired;
#endif
    int actualSession;/*账号实际会话数*/
    short sesIndex;/*此账号最后一个会话实际存储位置索引*/
#if (PPPOE_LIMIT == FYES)
    Uint32		uRateLimit;
    Uint32		dRateLimit;
#endif
#if 0
    unsigned int	pid; 
    unsigned int	STime;
    unsigned int	LTime;
    unsigned int	ConTime;
#if (PPPOE_SELF_HELP == FYES)
    Uint32		optCnt;
#endif
    unsigned long long	rxByte;
    unsigned long long	txByte;
    char		mac[UTT_PPP_CLIENT_MAC_LEN + 1];
    char		ip[UTT_PPP_CLIENT_IP_LEN + 1];
    char		ifname[UTT_PPP_CLIENT_IFNAME_LEN + 1];
    char		device[UTT_PPP_CLIENT_DEVICE_LEN + 1];
    char		ourName[UTT_PPP_CLIENT_OURNAME_LEN + 1];
#else
#if (PPPOE_SELF_HELP == FYES)
    Uint32		optCnt;
#endif
#endif
} PPPoESrvAccProfile;
#endif
