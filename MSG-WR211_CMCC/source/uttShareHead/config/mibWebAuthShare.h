#ifndef MIBWEBAUTH_SHARE_H
#define MIBWEBAUTH_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"

#define WEB_AUTH_REDIRECT		"web_auth_redirect"
#define SYS_WEB_AUTH_OK			IPSET_SYS_GNAME_PRE"web_auth_ok" 
#define SYS_WEB_AUTH_WEB		IPSET_SYS_GNAME_PRE"web_auth_web"
#define WEB_AUTH_DROP			"web_auth_drop"
#define WEB_AUTH_AUTH_ASP		"WebAuth_auth.asp"
#define WEB_AUTH_CONTACT_LEN	255
#define WEB_AUTH_CONTENT_LEN	140
#define WEB_AUTH_PICTURE_URL_LEN 511
#define WEB_AUTH_TITLE_LEN	32
#define WEB_AUTH_INFO_LEN	64
#if (WEBAUTH_SELF_HELP == FYES)
#define MAX_USER_OPT_MOD_CNT		5
#endif

#if (WIFIDOG_AUTH == FYES) || (WIFIDOG_MAKE_MONEY == FYES)
#ifdef CONFIG_UTT_WIFIDOG_REDIRECT
/*编译wifidog是使用，定义wifidog的认证条状方式
 * 采用内核构造http应答包实现认证跳转
 * 目前仅支持内核版本大于2.6.30的设备
 * */
#define WIFI_PRE_REDIRECT
#ifdef WIFI_PRE_REDIRECT
#define TABLE_WIFI_PRE_REDIRECT "wifi_pre_redirect"
#endif
#else
#error "please defind CONFIG_UTT_WIFIDOG_REDIRECT in file 'linux-config' while linux version is greater than 2.6.30"
#endif
/* wifidog标准接口控制宏，用于客户定制标准wifidog接口时使用，默认关闭
* 我们自己的wifidog远端认证是适用于我们自己认证服务器的wifidog接口,非标准接口
* */
#define WIFIDOG_STDIF FNO
/*
 * 本地短信发送控制宏
 * */
#define SMS_ENABLE 0

#if (CWMP == FYES)
#if (UTT_KNOWIFI == FYES)
#define WIFIDOG_MAC_AUTH FYES			/*mac 认证控制宏*/
#define WIFIDOG_USER_SUCCESS_URL FYES		/*认证成功URL设置控制宏*/
#else
#define WIFIDOG_MAC_AUTH FNO			/*mac 认证控制宏*/
#define WIFIDOG_USER_SUCCESS_URL FNO		/*认证成功URL设置控制宏*/
#endif
#define WIFIDOG_REMOTE_LOGOUT FNO		/*远程挂断*/
#endif
#ifndef WIFIDOG_USER_SECRET_KEY
#define WIFIDOG_USER_SECRET_KEY FYES		/*认证秘钥修改控制宏*/
#endif
#if (WIFIDOG_MAC_AUTH  == FYES)
#define WIFI_GROUP_BLACK_MAC "wifi_black_mac_grp"  /*MAC 黑名单组*/
#endif
#ifndef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
/*wifidog域名白名单改到内核实现后，次定义移到内核文件include/linux/netfilter_ipv4/utt_netlink.h中*/
#define  WHITE_LIST  "wifi_whitelist"
#endif
#define WIFI_CLIENT_LIST_FILE "/etc/wifiClientList"
#define WEIXIN_IPSET_GRP "weixin_free_group"
/* 添加其他的微信域名时，需要在文件
 * user/wifidog/src/fw_iptables.c和user/uttTool/uttUpDomain.c中
 * 添加处理*/
#define WEIXIN_DOMAIN_SHORT "short.weixin.qq.com"
#define WEIXIN_DOMAIN_LONG "long.weixin.qq.com"
#define WEIXIN_DOMAIN_SZLONG "szlong.weixin.qq.com" 
#define WEIXIN_DOMAIN_SZSHORT "szshort.weixin.qq.com"

#define PARAM_SIGN /*wifidog 参数数字签名*/

/* 支持三层交换机下的客户端认证 
 * 无法获取三层交换下主机mac
 * 暂时取LAN口mac作为认证IP的mac*/
#define SWITCH_AUTH_MODE  FYES

#define  SERVER_VERSION FYES  /*服务器区分新功能*/
#if (SERVER_VERSION == FYES)
#if (WIFIDOG_MAKE_MONEY == FYES)
#define SERV_VERSION "2.0"
#else
#define SERV_VERSION "1.0"
#endif
#define AUTH_SERV_VER "&v="SERV_VERSION
#else
#define	 SERV_VERSION ""
#define  AUTH_SERV_VER ""
#endif
#if (WIFIDOG_STDIF == FYES)
#undef PARAM_SIGN		/*标准wifidog接口不需要，验证加密信息*/
#undef WIFI_PRE_REDIRECT	/*标准wifidog接口，目前不支持内核对302跳转跳转优化*/
#endif
#define SMS_DES_CRYPTO 0

#if (SMS_ENABLE ==1 )
#define SMS_LIST_LEN 10U
#define MAX_PHONE_LEN (11+1)
#define MAX_SMS_DATA_LEN 300 
typedef struct sms_info_t {
    char    phone[MAX_PHONE_LEN];
    int	    flag;
    int	    len;
    char    data[MAX_SMS_DATA_LEN];
} smsinfo;
#define MAX_DEV_NAME 20
typedef struct smslist_t
{
    char dev[MAX_DEV_NAME];
    smsinfo smsnode[SMS_LIST_LEN];
} smsList;
#endif
#if (WHITELIST ==1 )
typedef struct st_whiteListProfile
{
    mibHead head;
    char domain[50];
} WhiteListProfile;
#endif
#if (WIFIDOG_NOTICE == FYES)
/*到期通告*/
#define WIFI_NOTICE_PRE_CHAIN "wifi_pre_notice"
#define WIFI_NOTICR_REDIRECT_CHAIN "wifi_notice_redirect"
#define WIFI_IPSET_NOTICE_GROUP "wifi_notice_group"
#define WIFI_IPSET_NOTICE_RECORD_GROUP "wifi_notice_record_group"
#define WIFI_NOTICE_HTML "NoticeWifi.htm"
#endif /*WIFIDOG_NOTICE*/
#if (WIFI_RATE_LIMIT == FYES)
/*带宽控制*/
#define WIFI_IPSET_RATELIMIT_GROUP_PREFIX "wifi_ratelimit_group"
#define WIFI_RATE_LIMIT_CHAIN "wifi_rate_limit_chain"
#endif /*WIFI_RATE_LIMIT*/
#endif /*WIFIDOG_AUTH*/
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
enum{
    MANUAL_METHOD = 0,
    IDCARD_METHOD,
    SMS_METHOD,
    WECHAT_METHOD
};
#endif

#if (UTT_KNOWIFI == FYES)
enum{
    KNOWIFI_MODE_MISSION = 0, //任务模式
    KNOWIFI_MODE_TIME, //时间模式
    KNOWIFI_MODE_FORBBIDEN, //禁网模式
};
#endif

/*
 * web 认证会话信息
 * */
typedef struct st_webAuthSession {
    Uint32  ip;
    char    mac[6];
    FunEnableEnum   compareMacEn;
    int nextIndex;
    int accIndex;
    //struct st_webAuthSession *next;
} WebAuthSession;

#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
typedef struct st_webAuthAutoGlobalProfile
{
    Uint32 enabled;             /* 开关，默认关 */
    Uint32 timeTotal;       /* 全局有效期，单位小时，默认刷卡机48小时，短信和微信2小时 */
    Uint32 maxSessions; /* 最大会话数， 默认1 */
    Uint32 totalTimes;	/* 每日最大允许申请次数，0表示不限制 */

    char   contents[WEB_AUTH_CONTENT_LEN +1];       /* 自定义短信、微信消息回复内容 */
    char   smsServer[WEB_AUTH_INFO_LEN+1];          /* 短信发送服务器地址 */ 
    char   wechatName[WEB_AUTH_INFO_LEN +1];    /* 微信名称 */
    char   wechatPicName[WEB_AUTH_TITLE_LEN + 1];/* 微信二维码图片名 */
    char   wechatToken[WEB_AUTH_TITLE_LEN +1];  /* 微信Token码 */
    char   wechatKeyContents[WEB_AUTH_TITLE_LEN +1];  /* 微信Token码 */
}WebAuthAutoGlobalProfile;
#endif

#ifdef CONFIG_USER_WIFIDOG
#if (WIFIDOG_AUTH == FYES)
#define MAX_WIFIDOG_URL_LEN 120U
typedef struct st_wifidogData {
#if (WIFIDOG_USER_SUCCESS_URL == FYES)
    char		successUrl[MAX_WIFIDOG_URL_LEN+1];	    /*认证成功跳转URL,远程配置使用(TR069)*/
#endif
#if (WIFIDOG_NOTICE == FYES)
    unsigned int	noticeTime;				    /*到期推送时间 单位:分钟,远程配置使用(TR069)*/
    char		noticeUrl[MAX_WIFIDOG_URL_LEN+1];	    /*到期通告URL,远程配置使用(TR069)*/
#endif
#if (WIFIDOG_USER_SECRET_KEY == FYES)
    char		secret_key[MAX_WIFIDOG_URL_LEN+1];	    /*签名秘钥*/
#endif
#if (WIFIDOG_MAC_AUTH == FYES)
    unsigned int	macAuthEn ;				/*MAC认证开启*/
#endif
} WifidogData;
#endif
#endif
typedef struct st_webAuthGlobalProfile
{
    mibHead head;
    Uint32  enabled;
#ifdef CONFIG_USER_WIFIDOG
#if (WIFIDOG_AUTH == FYES)
    Uint32 authType;	/*值为1表示为本地认证，值为2表示为远程认证；默认为本地认证*/
    char testDomain[50];
    char login_url[50];
    char auth_url[50];
    char success_url[50];
    char ping_url[50];
#if (SMS_ENABLE ==1)
    Uint32 smsEnable;
    Uint32 cdmaType;		/*值为1代表移动，2代表联通，3代表电信*/
    char   province[20];
    char   city[20];
    char   sms_number[20];
    char   addrPhone[15];
#endif
    /* wifidog 的相关信息都保存在此结构体中
     * 上面的变量也需要移到结构体中*/
    WifidogData wifiData;
#endif
#endif
    int     enpicture;
#if (WEBAUTH_ADVANCE == FYES)
    int     enDefPage;/*是否启用默认页面，0 否，1 是*/
	char	pageName[WEB_AUTH_TITLE_LEN +1];
	char	picName[WEB_AUTH_TITLE_LEN +1];
#endif
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
	WebAuthAutoGlobalProfile idcard;
#endif
#if (WEBAUTH_AUTO == FYES)
	WebAuthAutoGlobalProfile sms;
	WebAuthAutoGlobalProfile wechat;
	char    logServer[WEB_AUTH_INFO_LEN + 1];
	char	jumpUrl[WEB_AUTH_PICTURE_URL_LEN +1];/*认证成功后跳转url*/
#endif
#if (WEBAUTH_SELF_HELP == FYES)
    FunEnableEnum		self_help;
#endif
    Uint32		active_pic;			/*启用背景图片*/
    char    exceptIpGroup[IP_GROUP_NAME_LEN + 1];/*例外地址组名*/ 
    char    tipstitle[WEB_AUTH_TITLE_LEN+1]; /*窗口提示标题*/
    char    tipsinfo[WEB_AUTH_INFO_LEN+1]; /*窗口提示信息*/
	char    pictureUrl[WEB_AUTH_PICTURE_URL_LEN + 1];/*图片网络链接URL*/
    char    contact[WEB_AUTH_CONTACT_LEN + 1];
#if(WEB_AUTH_STALE == FYES)
    Uint32 staleT;
#endif
#if (UTT_KNOWIFI == FYES)
    Uint32 knowifi_enable;
    char knowifi_routeId[128];
#endif
} WebAuthGlobalProfile;
/*
 *
 * 网页认证结构体
 * user   -   网页认证用户名
 * passwd -   网页认证密码
 * remark -   网页认证描述
 * ip	  -   使用此账号的IP，0.0.0.0认为此账号未使用
 * mac	  -   mac地址
 * optCnt -   用户自助服务修改次数
 * timeingPatternEn - 计时模式
 * timeStart - 账号开通时间
 * timeStop - 账号过期时间
 * timeTotal - 总时间
 * timeUsed - 已使用时间，单位秒
 * timeLast - 上一次统计时间，单位秒，不保存flash
 * compareMacEn - 记录认证是否查到mac地址，计费时需不需要判断mac
 */
typedef struct st_webAuthProfile 
{ 
    mibHead head;
    char    user[UTT_USER_LEN + 1];
    char    passwd[UTT_PASSWD_LEN  + 1];
    char    remark[UTT_USER_LEN + 1];
    char    status[UTT_USER_LEN + 1];
    Uint32  ip;
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
    char    idcode[UTT_USER_LEN + 1];
    int	    autoAuthMethod;
#endif
#if (WEBAUTH_ACCOUNT == FYES)
    char    mac[6];
    FunEnableEnum   timeingPatternEn;
    Uint32  timeStart;
    Uint32  timeStop;
    unsigned long   timeTotal;
    unsigned long   timeUsed;
    unsigned long   timeLast;
    FunEnableEnum   compareMacEn;
#endif
#if(WEBAUTH_SELF_HELP == FYES)
    Uint32  optCnt;
#endif
    int  maxSession;
    Uint32  actualSession;
    int	    sesIndex;
    //WebAuthSession *sesHead;
} WebAuthProfile;
#endif
