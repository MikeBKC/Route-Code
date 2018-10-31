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
/*����wifidog��ʹ�ã�����wifidog����֤��״��ʽ
 * �����ں˹���httpӦ���ʵ����֤��ת
 * Ŀǰ��֧���ں˰汾����2.6.30���豸
 * */
#define WIFI_PRE_REDIRECT
#ifdef WIFI_PRE_REDIRECT
#define TABLE_WIFI_PRE_REDIRECT "wifi_pre_redirect"
#endif
#else
#error "please defind CONFIG_UTT_WIFIDOG_REDIRECT in file 'linux-config' while linux version is greater than 2.6.30"
#endif
/* wifidog��׼�ӿڿ��ƺ꣬���ڿͻ����Ʊ�׼wifidog�ӿ�ʱʹ�ã�Ĭ�Ϲر�
* �����Լ���wifidogԶ����֤�������������Լ���֤��������wifidog�ӿ�,�Ǳ�׼�ӿ�
* */
#define WIFIDOG_STDIF FNO
/*
 * ���ض��ŷ��Ϳ��ƺ�
 * */
#define SMS_ENABLE 0

#if (CWMP == FYES)
#if (UTT_KNOWIFI == FYES)
#define WIFIDOG_MAC_AUTH FYES			/*mac ��֤���ƺ�*/
#define WIFIDOG_USER_SUCCESS_URL FYES		/*��֤�ɹ�URL���ÿ��ƺ�*/
#else
#define WIFIDOG_MAC_AUTH FNO			/*mac ��֤���ƺ�*/
#define WIFIDOG_USER_SUCCESS_URL FNO		/*��֤�ɹ�URL���ÿ��ƺ�*/
#endif
#define WIFIDOG_REMOTE_LOGOUT FNO		/*Զ�̹Ҷ�*/
#endif
#ifndef WIFIDOG_USER_SECRET_KEY
#define WIFIDOG_USER_SECRET_KEY FYES		/*��֤��Կ�޸Ŀ��ƺ�*/
#endif
#if (WIFIDOG_MAC_AUTH  == FYES)
#define WIFI_GROUP_BLACK_MAC "wifi_black_mac_grp"  /*MAC ��������*/
#endif
#ifndef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
/*wifidog�����������ĵ��ں�ʵ�ֺ󣬴ζ����Ƶ��ں��ļ�include/linux/netfilter_ipv4/utt_netlink.h��*/
#define  WHITE_LIST  "wifi_whitelist"
#endif
#define WIFI_CLIENT_LIST_FILE "/etc/wifiClientList"
#define WEIXIN_IPSET_GRP "weixin_free_group"
/* ���������΢������ʱ����Ҫ���ļ�
 * user/wifidog/src/fw_iptables.c��user/uttTool/uttUpDomain.c��
 * ��Ӵ���*/
#define WEIXIN_DOMAIN_SHORT "short.weixin.qq.com"
#define WEIXIN_DOMAIN_LONG "long.weixin.qq.com"
#define WEIXIN_DOMAIN_SZLONG "szlong.weixin.qq.com" 
#define WEIXIN_DOMAIN_SZSHORT "szshort.weixin.qq.com"

#define PARAM_SIGN /*wifidog ��������ǩ��*/

/* ֧�����㽻�����µĿͻ�����֤ 
 * �޷���ȡ���㽻��������mac
 * ��ʱȡLAN��mac��Ϊ��֤IP��mac*/
#define SWITCH_AUTH_MODE  FYES

#define  SERVER_VERSION FYES  /*�����������¹���*/
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
#undef PARAM_SIGN		/*��׼wifidog�ӿڲ���Ҫ����֤������Ϣ*/
#undef WIFI_PRE_REDIRECT	/*��׼wifidog�ӿڣ�Ŀǰ��֧���ں˶�302��ת��ת�Ż�*/
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
/*����ͨ��*/
#define WIFI_NOTICE_PRE_CHAIN "wifi_pre_notice"
#define WIFI_NOTICR_REDIRECT_CHAIN "wifi_notice_redirect"
#define WIFI_IPSET_NOTICE_GROUP "wifi_notice_group"
#define WIFI_IPSET_NOTICE_RECORD_GROUP "wifi_notice_record_group"
#define WIFI_NOTICE_HTML "NoticeWifi.htm"
#endif /*WIFIDOG_NOTICE*/
#if (WIFI_RATE_LIMIT == FYES)
/*�������*/
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
    KNOWIFI_MODE_MISSION = 0, //����ģʽ
    KNOWIFI_MODE_TIME, //ʱ��ģʽ
    KNOWIFI_MODE_FORBBIDEN, //����ģʽ
};
#endif

/*
 * web ��֤�Ự��Ϣ
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
    Uint32 enabled;             /* ���أ�Ĭ�Ϲ� */
    Uint32 timeTotal;       /* ȫ����Ч�ڣ���λСʱ��Ĭ��ˢ����48Сʱ�����ź�΢��2Сʱ */
    Uint32 maxSessions; /* ���Ự���� Ĭ��1 */
    Uint32 totalTimes;	/* ÿ������������������0��ʾ������ */

    char   contents[WEB_AUTH_CONTENT_LEN +1];       /* �Զ�����š�΢����Ϣ�ظ����� */
    char   smsServer[WEB_AUTH_INFO_LEN+1];          /* ���ŷ��ͷ�������ַ */ 
    char   wechatName[WEB_AUTH_INFO_LEN +1];    /* ΢������ */
    char   wechatPicName[WEB_AUTH_TITLE_LEN + 1];/* ΢�Ŷ�ά��ͼƬ�� */
    char   wechatToken[WEB_AUTH_TITLE_LEN +1];  /* ΢��Token�� */
    char   wechatKeyContents[WEB_AUTH_TITLE_LEN +1];  /* ΢��Token�� */
}WebAuthAutoGlobalProfile;
#endif

#ifdef CONFIG_USER_WIFIDOG
#if (WIFIDOG_AUTH == FYES)
#define MAX_WIFIDOG_URL_LEN 120U
typedef struct st_wifidogData {
#if (WIFIDOG_USER_SUCCESS_URL == FYES)
    char		successUrl[MAX_WIFIDOG_URL_LEN+1];	    /*��֤�ɹ���תURL,Զ������ʹ��(TR069)*/
#endif
#if (WIFIDOG_NOTICE == FYES)
    unsigned int	noticeTime;				    /*��������ʱ�� ��λ:����,Զ������ʹ��(TR069)*/
    char		noticeUrl[MAX_WIFIDOG_URL_LEN+1];	    /*����ͨ��URL,Զ������ʹ��(TR069)*/
#endif
#if (WIFIDOG_USER_SECRET_KEY == FYES)
    char		secret_key[MAX_WIFIDOG_URL_LEN+1];	    /*ǩ����Կ*/
#endif
#if (WIFIDOG_MAC_AUTH == FYES)
    unsigned int	macAuthEn ;				/*MAC��֤����*/
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
    Uint32 authType;	/*ֵΪ1��ʾΪ������֤��ֵΪ2��ʾΪԶ����֤��Ĭ��Ϊ������֤*/
    char testDomain[50];
    char login_url[50];
    char auth_url[50];
    char success_url[50];
    char ping_url[50];
#if (SMS_ENABLE ==1)
    Uint32 smsEnable;
    Uint32 cdmaType;		/*ֵΪ1�����ƶ���2������ͨ��3�������*/
    char   province[20];
    char   city[20];
    char   sms_number[20];
    char   addrPhone[15];
#endif
    /* wifidog �������Ϣ�������ڴ˽ṹ����
     * ����ı���Ҳ��Ҫ�Ƶ��ṹ����*/
    WifidogData wifiData;
#endif
#endif
    int     enpicture;
#if (WEBAUTH_ADVANCE == FYES)
    int     enDefPage;/*�Ƿ�����Ĭ��ҳ�棬0 ��1 ��*/
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
	char	jumpUrl[WEB_AUTH_PICTURE_URL_LEN +1];/*��֤�ɹ�����תurl*/
#endif
#if (WEBAUTH_SELF_HELP == FYES)
    FunEnableEnum		self_help;
#endif
    Uint32		active_pic;			/*���ñ���ͼƬ*/
    char    exceptIpGroup[IP_GROUP_NAME_LEN + 1];/*�����ַ����*/ 
    char    tipstitle[WEB_AUTH_TITLE_LEN+1]; /*������ʾ����*/
    char    tipsinfo[WEB_AUTH_INFO_LEN+1]; /*������ʾ��Ϣ*/
	char    pictureUrl[WEB_AUTH_PICTURE_URL_LEN + 1];/*ͼƬ��������URL*/
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
 * ��ҳ��֤�ṹ��
 * user   -   ��ҳ��֤�û���
 * passwd -   ��ҳ��֤����
 * remark -   ��ҳ��֤����
 * ip	  -   ʹ�ô��˺ŵ�IP��0.0.0.0��Ϊ���˺�δʹ��
 * mac	  -   mac��ַ
 * optCnt -   �û����������޸Ĵ���
 * timeingPatternEn - ��ʱģʽ
 * timeStart - �˺ſ�ͨʱ��
 * timeStop - �˺Ź���ʱ��
 * timeTotal - ��ʱ��
 * timeUsed - ��ʹ��ʱ�䣬��λ��
 * timeLast - ��һ��ͳ��ʱ�䣬��λ�룬������flash
 * compareMacEn - ��¼��֤�Ƿ�鵽mac��ַ���Ʒ�ʱ�費��Ҫ�ж�mac
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
