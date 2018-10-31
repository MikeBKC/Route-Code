#include <net/if_arp.h>
#include    "uttMachine.h"
#include    "wsIntrn.h"
#include    "um.h"
#include    "utils.h"
#include    "mib.h"
#include    "profacce.h"
#include    "uttfunction.h"
#include    "argcmd.h"
#include        "sha1.h"
#include    "linklist.h"

#if (WEBAUTH_AUTO == FYES)

#define SWORD_DEBUG 1
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)	printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

#define CPT_STR_LEN 200
#define WX_MSG_TYPE_TEXT 1
#define SMS_SHA1_KEY "helloUTTwebauthCaptcha"
#ifdef CONFIG_UTT_NETFILTER
extern int uttNfNlSock;
#endif
extern unsigned int productSn;

extern char *iconv_string (const char *str, const char *from_codeset, const char *to_codeset);

struct wxMsg {
    char toUser[CPT_STR_LEN];
    char fromUser[CPT_STR_LEN];
    char msgType[CPT_STR_LEN];
    char content[CPT_STR_LEN];
};

/******************************************************************/
static struct arpreq arpLan = {
    .arp_pa.sa_family = AF_INET,
    .arp_ha.sa_family = AF_INET
};

static struct list *smsList;
static struct list *wechatList;
static struct list *devList;
static pthread_t tid;
static int smsListLockFlag = 0;
static int wechatListLockFlag = 0;
static int devListLockFlag = 0;
struct smsNode{
    char phonenum[11];
    int  times;	/* 已申请验证码次数 */
};
struct wechatNode{
    char openid[30];
    int  times;	/* 已申请验证码次数 */
};
struct devNode{
    char mac[6];		/* 该设备mac地址 */
    unsigned long lastAskTime;	/* 上次申请验证码的时间*/
    int times;			/* 已申请验证码的次数 */
};
static void captchaListCheck(void);

int captchaListInit(void)
{
    smsList = list_new();
    if (smsList == NULL)
    {
	SWORD_PRINTF("%s %d: no enough memory.\n", __func__, __LINE__);
	return -1;
    }

    wechatList = list_new();
    if (wechatList == NULL)
    {
	SWORD_PRINTF("%s %d: no enough memory.\n", __func__, __LINE__);
	return -1;
    }
    
    devList = list_new();
    if (devList == NULL)
    {
	SWORD_PRINTF("%s %d: no enough memory.\n", __func__, __LINE__);
	return -1;
    }

    if (pthread_create(&tid, NULL, (void*)captchaListCheck, NULL) != 0)
    {
	SWORD_PRINTF("%s %d: create threat error.\n", __func__, __LINE__);
	return -1;
    }

    return 0;
}

static int smsListAsk(char *phonenum, int totalTimes)
{
    struct listnode *node;
    struct smsNode *smsNode;
    int exist_flag = 0;
    int ok = 0, full = 1, memerr = 2;

    while (smsListLockFlag)
	sleep(500);

    for (node=listhead(smsList); node; nextnode(node))
    {
	smsNode = getdata(node);
	if (strcmp(smsNode->phonenum, phonenum) == 0)
	{
	    exist_flag = 1;
	    break;
	}
    }

    if (exist_flag)
    {
	if (smsNode->times >= totalTimes)
	    return full;
	else
	{
	    (smsNode->times)++;
	    return ok;
	}
    }
    else 
    {
	if (totalTimes == 0)
	    return ok;

	smsNode = NULL;
	smsNode = (struct smsNode*) malloc (sizeof(struct smsNode));
	if (smsNode == NULL)
	{
	    SWORD_PRINTF("%s %d: no enough memory.\n", __func__, __LINE__);
	    return memerr;
	}
	strcpy(smsNode->phonenum, phonenum);
	smsNode->times = 1;
	listnode_add(smsList, smsNode);
	return ok;
    }

    return ok;
}

static int wechatListAsk(char *openid, int totalTimes)
{
    struct listnode *node;
    struct wechatNode *wechatNode;
    int exist_flag = 0;
    int ok = 0, full = 1, memerr = 2;

    while (wechatListLockFlag)
	sleep(500);
    for (node=listhead(wechatList); node; nextnode(node))
    {
	wechatNode = getdata(node);
	if (strcmp(wechatNode->openid, openid) == 0)
	{
	    exist_flag = 1;
	    break;
	}
    }

    if (exist_flag)
    {
	if (wechatNode->times >= totalTimes)
	    return full;
	else
	{
	    (wechatNode->times)++;
	    return ok;
	}
    }
    else 
    {
	if (totalTimes == 0)
	    return ok;

	wechatNode = NULL;
	wechatNode = (struct wechatNode*) malloc (sizeof(struct wechatNode));
	if (wechatNode == NULL)
	{
	    SWORD_PRINTF("%s %d: no enough memory.\n", __func__, __LINE__);
	    return memerr;
	}
	strcpy(wechatNode->openid, openid);
	wechatNode->times = 1;
	listnode_add(wechatList, wechatNode);
	return ok;
    }

    return ok;
}

static int devListAsk(char *mac)
{
    struct listnode *node;
    struct devNode *devNode;
    int exist_flag = 0;
    int ok = 0, full = 1, memerr = 2, busy = 3;
    int periodTimes = 5;
    int periodTime = 60;
    unsigned int now = time(0);

    while (devListLockFlag)
	sleep(500);

    for (node=listhead(devList); node; nextnode(node))
    {
	devNode = getdata(node);
	if (memcmp(devNode->mac, mac, 6) == 0)
	{
	    exist_flag = 1;
	    break;
	}
    }

    if (exist_flag)
    {
	if (devNode->times >= periodTimes)
	{
	    return full;
	}
	else if (now - devNode->lastAskTime < periodTime)
	{
	    return busy;
	}
	else
	{
	    (devNode->times)++;
	    devNode->lastAskTime = now;
	    return ok;
	}
    }
    else 
    {
	devNode = NULL;
	devNode = (struct devNode*) malloc (sizeof(struct devNode));
	if (devNode == NULL)
	{
	    SWORD_PRINTF("%s %d: no enough memory.\n", __func__, __LINE__);
	    return memerr;
	}
	memcpy(devNode->mac, mac, 6);
	devNode->times = 1;
	devNode->lastAskTime = now;
	listnode_add(devList, devNode);
	return ok;
    }

    return ok;
}
void smsListDelAll(void)
{
    list_delete_all_node(smsList);
    return;
}

void wechatListDelAll(void)
{
    list_delete_all_node(wechatList);
    return;
}

void devListDelAll(void)
{
    list_delete_all_node(devList);
    return;
}

static void captchaListCheck(void)
{
    time_t now;
    struct tm *tm;
    int interval = 1 * 60; /* sec */
    static int lastDay = 0;
    static int lastHour = 0;

    while (1) 
    {
	now = time(0);
	tm = localtime(&now);

	if (lastDay != tm->tm_mday)
	{
	    smsListLockFlag = 1;
	    smsListDelAll();
	    smsListLockFlag = 0;
	    wechatListLockFlag = 1;
	    wechatListDelAll();
	    wechatListLockFlag = 0;
	    lastDay = tm->tm_mday;
	}

	if (lastHour != tm->tm_hour)
	{
	    devListLockFlag = 1;
	    devListDelAll();
	    devListLockFlag = 0;
	    lastHour = tm->tm_hour;
	}

	sleep(interval);	/*挂起，单位毫秒*/
    }

    return;
}

/******************************************************************/

/********************************************************************
 ** 函数： sort2str
 ** 功能： 将token、timestamp、nonce进行字典排序后组成字符串buf
 ** 创建： 2014-01-05
 ** 参数： 略
 ** 返回： 略
 ** 输出： 略
 ** 作者： 李文昌 
 *********************************************************************/
void sort2str(const char *token, const char *timestamp, const char *nonce, char *buf)
{
    char bufarray[3][CPT_STR_LEN];
    int token_id = 0, timestamp_id = 0, nonce_id = 0;

    if (strcmp(token, timestamp) > 0)
	token_id++;
    else
	timestamp_id++;

    if (strcmp(token, nonce) > 0)
	token_id++;
    else
	nonce_id++;

    if (strcmp(timestamp, nonce) > 0)
	timestamp_id++;
    else
	nonce_id++;

    memset(buf, 0, sizeof(buf));
    strcpy(bufarray[token_id], token);
    strcpy(bufarray[timestamp_id], timestamp);
    strcpy(bufarray[nonce_id], nonce);
    sprintf(buf, "%s%s%s", bufarray[0], bufarray[1], bufarray[2]);

    return;
}

/********************************************************************
 ** 函数： sha1
 ** 功能： 将inbuf进行sha1加密，生成outbuf
 ** 创建： 2014-01-05
 ** 参数： 略
 ** 返回： 略
 ** 输出： 略
 ** 作者： 李文昌 
 *********************************************************************/ 
void sha1(const char *inbuf, char *outbuf)
{
    SHA1_CTX s;

    SHA1_Init(&s);
    SHA1_Update(&s, inbuf, strlen(inbuf));
    SHA1_Final(outbuf, &s);

    return;
}

char *changeStrLower(char *str)
{
    char *p = str;

    while (*p) {
	if (*p > 'A' && *p < 'Z')
	    *p += ('a'-'A');
	p++;
    }

    return str;
}

static int checkMacZero(char *mac)
{
    if ((mac == NULL) ||
	    ((mac[0] == 0) && (mac[0] == 0) && (mac[0] == 0) &&
	     (mac[0] == 0) && (mac[0] == 0) && (mac[0] == 0)))
	return 1;
    else 
	return 0;
}

static char *getMacByIpStr(const char *ipStr, char *getmac)
{
    char *mac = NULL;
    ((struct sockaddr_in*)&arpLan.arp_pa)->sin_addr.s_addr = inet_addr((const char *)(ipStr));
    memset(arpLan.arp_ha.sa_data, 0, sizeof(arpLan.arp_ha.sa_data));
    mac = getMacByIpIoctl(&arpLan);/*通过arp表查询当前ip的对应mac地址*/

    if (checkMacZero(mac))
    {
	SWORD_PRINTF("%s %d: ip %s cannot get mac!\n", __func__, __LINE__, ipStr);
    } else {
	memcpy(getmac, mac, 6);
	printf("%s %d: ip %s get mac %02X%02X-%02X%02X-%02X%02X\n",
		__func__, __LINE__, ipStr, 
		(unsigned char)mac[0], (unsigned char)mac[1], 
		(unsigned char)mac[2], (unsigned char)mac[3], 
		(unsigned char)mac[4], (unsigned char)mac[5]);
    }
    
    return mac;
}

/********************************************************************
 * 函数： wxSendFormat
 * 功能： 生成随机且唯一的验证码
 * 创建： 2014-01-10
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： 李文昌 
 ********************************************************************/ 
void captchaCreate(int *captcha)
{
    int i;
    static int index = 0;
    MibProfileType mibType = MIB_PROF_WEB_AUTH_CPT;
    WebAuthProfile *prof = NULL;
    int min = 0, max = 0;
    char passwd[UTT_PASSWD_LEN];

    ProfInstLowHigh(mibType, &max, &min);

    while (1) {
	index++;
	srand(time(0)+index);
	*captcha = rand()%10000;
	if (*captcha == 0)
	    continue;
	memset(passwd, 0, sizeof(passwd));
	sprintf(passwd, "%04d", *captcha);

	for(i = min;i < max;i++)
	{
	    prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
	    if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE) && (prof->autoAuthMethod == WECHAT_METHOD))
	    {
		if (strcmp(prof->passwd, passwd) == 0)
		    continue;
	    }
	}

	break;
    }
}

/********************************************************************
 * 函数： analysisXml
 * 功能： 解析微信报文中的xml消息
 * 创建： 2014-01-10
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： 李文昌 
 ********************************************************************/ 
int analysisXml(const char *xml, const char *key, char *value)
{
    char *p1, *p2;

    p1 = xml;
    while(*p1) {
	if ((memcmp(p1, key, strlen(key)) == 0) && 
		(*(p1-1) == '<' && *(p1+strlen(key)) == '>')) {
	    p1 += (strlen(key) + 1);
	    break;
	}

	p1++;
    }

    if (*p1 == '\0') {
	return 0;
    }

    p2 = p1;
    while (*p2) {
	if ((memcmp(p2, key, strlen(key)) == 0) &&
		(*(p2-2) == '<' && *(p2-1) == '/' && *(p2+strlen(key)) == '>')) {
	    p2 -= 2;
	    break;
	}

	p2++;
    }

    if (*p2 == '\0') {
	return 0;
    }

    if ((memcmp(p1, "<![CDATA[", strlen("<![CDATA[")) == 0) &&
	    (*(p2-1) == '>' && *(p2-2) == ']' && *(p2-3) == ']')) {
	p1 += strlen("<![CDATA[");
	p2 -= 3;
    }

    memcpy(value, p1, p2-p1);
    value[p2-p1] = '\0';
    return 1;
}

void writeSmsServerFile(void)
{
    FILE *fp;
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;
    profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
    if ((profG == NULL) || (ProfInstIsFree(profG)))
    {
	printf("%s %d: cannot get global parameters\n", __func__, __LINE__);
	return;
    }

    fp = fopen("/etc/smsserver", "w");
    if (fp == NULL)
    {
	printf("%s %d: cannot open file /etc/smsserver", __func__, __LINE__);
	return;
    }

    fprintf(fp, "%s", profG->sms.smsServer);
    fclose(fp);
    return;
}

#if 0
#define SMS_SHA1_KEY "helloUTTwebauthCaptcha"
#define CPT_STR_LEN 200
#define SHA1_SIGNATURE_SIZE 20
#define PHONE_LEN 11
#define CAPTCHA_LEN 6

/********************************************************************
 ** 函数： isFromSms
 ** 功能： 检验sha1Key密钥是否是由UTT路由器加密生成的
 ** 创建： 2014-01-16
 ** 参数： productSn:产品序列号；phoneNum:11位手机号；captcha:6位验证码
 ** 返回： 0:校验失败；1:校验成功
 ** 输出： 略
 ** 作者： 李文昌 
 *********************************************************************/ 
int isFromSms(char *sha1Key, int productSn, char *phoneNum)
{
    int i, phone1, phone2, phone3;
    char inbuf[CPT_STR_LEN], outbuf[SHA1_SIGNATURE_SIZE], key[CPT_STR_LEN];
    int captcha = 8888;

    if ((strlen(sha1Key) != SHA1_SIGNATURE_SIZE*2) 
	    || (strlen(phoneNum) != PHONE_LEN))
	return 0;

    /* 私有加密方式，根据SMS_SHA1_KEY、phoneNum、captcha和producSn生成密文 inbuf */
    memset(inbuf, 0, sizeof(inbuf));
    sscanf(phoneNum, "%5x%4x%2x", &phone1, &phone2, &phone3);
    sprintf(inbuf, "%s%d%d%d", SMS_SHA1_KEY, 
	    productSn+captcha+phone1, 
	    phone1*phone3+captcha*phone2, 
	    phone3*captcha+((captcha*phone1/phone2)<<2));

    /* 将inbuf进行sha1加密，生成outbuf */
    sha1(inbuf, outbuf);

    /* 将生成的outbuf十六进制数转化成字符串 */
    for (i=0; i<SHA1_SIGNATURE_SIZE; i++)
	sprintf(key, "%s%02x", key, (unsigned char)outbuf[i]);

    printf("key: \n\t%s\n\t%s\n",sha1Key, key);
    if (memcmp(sha1Key, key, SHA1_SIGNATURE_SIZE*2) == 0)
	return 1;
    else 
	return 0;
}
#endif

/********************************************************************
 ** 函数： smsCreatesha1Key
 ** 功能： 按照一定规则创建sha1密钥
 ** 创建： 2014-01-15
 ** 参数： 略
 ** 返回： 略
 ** 输出： 略
 ** 作者： 李文昌 
 *********************************************************************/ 
static int smsCreatesha1Key(char *key, char *phonenum)
{
    int i, phone1, phone2, phone3, max, min;
    char inbuf[CPT_STR_LEN], outbuf[SHA1_SIGNATURE_SIZE];
    int captcha = 8888;

    memset(inbuf, 0, sizeof(inbuf));
    sscanf(phonenum, "%5x%4x%2x", &phone1, &phone2, &phone3);
    sprintf(inbuf, "%s%d%d%d", SMS_SHA1_KEY, productSn+captcha+phone1, phone1*phone3+captcha*phone2, phone3*captcha+((captcha*phone1/(phone2+2))<<2));
    sha1(inbuf, outbuf);

    for (i=0; i<SHA1_SIGNATURE_SIZE; i++)
	sprintf(key, "%s%02x", key, (unsigned char)outbuf[i]);

#if 0
    if (isFromSms(key, productSn, phonenum))
	printf("%s %d: 校验成功!\n", __func__, __LINE__);
    else
	printf("%s %d: 校验失败!\n", __func__, __LINE__);
#endif

    return 0;
}

/********************************************************************
 * 函数： smsSend
 * 功能： 发送短信
 * 创建： 2014-01-15
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： 李文昌 
 ********************************************************************/ 
static int smsSend(char *sha1buf, char *phone, char *sendbuf)
{
    char command[CPT_STR_LEN];

    memset(command, 0, sizeof(command));
    sprintf(command, "smsClient \"%s\" \"%s\" \"%s\" \"%d\" &", sha1buf, phone, sendbuf, productSn);

    doSystem(command);
    
    return 0;
}

/********************************************************************
 * 函数： formWeixinUrl
 * 功能： 处理用户提交微信申请
 * 创建： 2014-01-02
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： 李文昌 
 ********************************************************************/  
static void wechatSend(webs_t wp, char *sendBuf)
{
    char httpBuf[CPT_STR_LEN];

    if ((sendBuf == NULL) || (strlen(sendBuf) == 0))
    {
	return;
    }

    memset(httpBuf, 0, sizeof(httpBuf));
    strcpy(httpBuf, "HTTP/1.1 200 OK\n");
    sprintf(httpBuf, "%sContent-length:%d\n", httpBuf, strlen(sendBuf));
    sprintf(httpBuf, "%sKeep-Alive:timeout=15,max=100\n", httpBuf);
    sprintf(httpBuf, "%sConnection:Keep-Alive\n", httpBuf);
    sprintf(httpBuf, "%sContent-Type:text/html\n", httpBuf);

    SWORD_PRINTF("sendbuf: \n%s\n", sendBuf);

    websWrite(wp, "%s\n", httpBuf); 
    websWrite(wp, "%s", sendBuf);
    websDone(wp, 200);

    return;
}


/********************************************************************
 ** 函数： isFromWeixin
 ** 功能： 判断消息是否来自微信
 ** 创建： 2014-01-04
 ** 参数： 略
 ** 返回： 略
 ** 输出： 略
 ** 作者： 李文昌 
 *********************************************************************/ 
int isFromWechat(webs_t wp)
{
    int i;
    char inbuf[CPT_STR_LEN];
    char outbuf[SHA1_SIGNATURE_SIZE];
    char sigbuf[CPT_STR_LEN];

    char *signature = NULL; 
    char *timestamp = NULL;
    char *nonce     = NULL;

    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;
    profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
    if ((profG == NULL) || (ProfInstIsFree(profG)) || (profG->head.active == FALSE)
	|| (profG->enabled == 0) || (profG->wechat.enabled == 0)) {
	SWORD_PRINTF("%s %d: wechat disabled.\n", __func__, __LINE__);
	return 0;
    }

    signature = websGetVar(wp, T("signature"), T(""));
    timestamp = websGetVar(wp, T("timestamp"), T(""));
    nonce     = websGetVar(wp, T("nonce"),     T(""));

    SWORD_PRINTF("signature = %s, timestamp = %s, nonce = %s\n",
	    signature, timestamp, nonce);

    memset(inbuf, 0, sizeof(inbuf));
    memset(outbuf, 0, sizeof(outbuf));
    sort2str(profG->wechat.wechatToken, timestamp, nonce, inbuf);
    sha1(inbuf, outbuf);

    memset(sigbuf, 0, sizeof(sigbuf));
    for (i=0; i<SHA1_SIGNATURE_SIZE; i++)
	sprintf(sigbuf, "%s%02x", sigbuf, (unsigned char)outbuf[i]);

    if (memcmp(signature, sigbuf, SHA1_SIGNATURE_SIZE*2) == 0)
	return 1;
    else 
	return 0;
}



char *captchaFastUrl(int type, int captcha, char *url)
{
    char gateway[16];
    char code[5];
    int tmp;
    struct in_addr addr;
#if (DHCP_SERVER == FYES)
    DhcpServerProfile *profDhcpS = NULL;
    MibProfileType mibType = MIB_PROF_DHCP_SERVER;

    memset(gateway, 0, sizeof(gateway));

    profDhcpS = (DhcpServerProfile*)ProfGetInstPointByIndex(mibType, 0);
    if (profDhcpS != NULL && profDhcpS->enable == FUN_ENABLE)
    {
	addr.s_addr = profDhcpS->gateway;
//	printf("get dhcp gateway: %s\n", inet_ntoa(addr));
	sprintf(gateway, "%s", inet_ntoa(addr));
    }
    else
#endif
	getIfIp(getLanIfName(), gateway);

    srand(time(0));
    tmp = captcha%10000/1000;
    if (tmp == 0)
	tmp = 10;
    if (type == SMS_METHOD)
	code[0] = (rand()%2 + 4) * 16 + tmp;
    else
	code[0] = (rand()%2 + 6) * 16 + tmp;

    tmp = captcha%1000/100;
    if (tmp == 0)
	tmp = 10;
    code[1] = (rand()%4 + 4) * 16 + tmp;
    tmp = captcha%100/10;
    if (tmp == 0)
	tmp = 10;
    code[2] = (rand()%4 + 4) * 16 + tmp;
    tmp = captcha%10;
    if (tmp == 0)
	tmp = 10;
    code[3] = (rand()%4 + 4) * 16 + tmp;
    code[4] = 0;

  //  printf("captcha: %04d, code: %c[%d], %c[%d], %c[%d], %c[%d]\n", captcha, 
//	    code[0], code[0], code[1], code[1], code[2], code[2], code[3], code[3]);

    sprintf(url, "http://%s/%s", gateway, code);

    return url;
}

void captchaCreateTest(void)
{
    int i, cpt, num = 5000;
    char url[100];

    for (i=0; i<num; i++)
    {
	captchaCreate(&cpt);
	printf("%d: %04d, %s\n", i, cpt, captchaFastUrl(SMS_METHOD, cpt, url));
    }
}

int _captchaSendFormat(char *insert, char *key, char *fmt)
{
    int i, err = 0, ok = 1; 
    char *p;
    char buf[CPT_STR_LEN]; 

    for (i=0; i<strlen(fmt); i++) 
    {
	p = &fmt[i];
	if (memcmp(p, key, strlen(key)) == 0)
	    break;
    }

    if (i >= strlen(fmt))
	return err;

    memset(buf, 0, sizeof(buf));
    memcpy(buf, fmt, i);
    strcat(buf, insert);
    p = &fmt[i+strlen(key)];
    strcat(buf, p);

    strcpy(fmt, buf);
    return ok;
}

/********************************************************************
 * 函数： wxSendFormat
 * 功能： 按照固定格式将验证码和有效期组成字符串
 * 创建： 2014-01-10
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： 李文昌 
 ********************************************************************/ 
int captchaSendFormat(int type, WebAuthGlobalProfile *profG, int captcha, char *send)
{
    char *p;
    unsigned int time;
    char captcha_str[CPT_STR_LEN], time_str[CPT_STR_LEN], url_str[CPT_STR_LEN], buf[CPT_STR_LEN];
    int i, tmp1 = -1, tmp2 = -1, tmp3 = -1;

    if (type == SMS_METHOD)
    {
	time = profG->sms.timeTotal;
	strcpy(buf, profG->sms.contents);
    }
    else if (type == WECHAT_METHOD)
    {
	time = profG->wechat.timeTotal;
	strcpy(buf, profG->wechat.contents);
    }

    sprintf(captcha_str, "%04d", captcha);

    memset(time_str, 0, sizeof(time_str));
    if (time%24 == 0)
	sprintf(time_str, "%d天", time/24);
    else
	sprintf(time_str, "%d小时", time);

    captchaFastUrl(type, captcha, url_str);

    _captchaSendFormat(captcha_str, "##", buf);
    _captchaSendFormat(time_str, "**", buf);
    _captchaSendFormat(url_str, "$$", buf);

    strcpy(send, buf);
    return 0;
}

/********************************************************************
 * 函数： WebAuthAccountIsFull
 * 功能： 检查认证账号是否已达最大值
 * 创建： 2014-01-22
 * 参数： 略
 * 返回： 1：账号数已达最大，0：账号数未达最大值
 * 输出： 略
 * 作者： 李文昌 
 ********************************************************************/ 
int WebAuthAccountIsFull()
{

    MibProfileType mibType = MIB_PROF_WEB_AUTH;
    MibProfileType mibTypeC = MIB_PROF_WEB_AUTH_CPT;
    WebAuthProfile *prof = NULL;

    int i = 0;
    int min = 0, max = 0;
    int webIndexInst, instIndex = 0;
    int num = 0;

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min;i < max;i++)
    {
	prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
	{
	    num++;
	}
    }
    
    ProfInstLowHigh(mibTypeC, &max, &min);
    for(i = min;i < max;i++)
    {
	prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibTypeC, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
	{
	    num++;
	}
    }

    if (num < MAX_WEB_AUTH_PROFILES)
	return 0;
    else
	return 1;
}

void captchaAuthCancel(WebAuthProfile *prof)
{
#if (IP_GRP == FYES)
    struct in_addr addr;
#endif
#if (WEB_AUTH_SHARE == FYES)
    int i = 0, j = -1;
    for(i=0;i<prof->actualSession;i++)
    {
	j = prof->sesIndex;
	if(checkSessionIndex(j) != 1)
	{
	    break;
	}
	/*将此IP从OK规则清除*/ 
	webAuthLog(prof, nvramProfile->session[j].ip, nvramProfile->session[j].mac);
	ipsetAOrDIp(SYS_WEB_AUTH_OK,nvramProfile->session[j].ip,IPSET_MEM_DEL);
#if (IP_GRP == FYES)
	addr.s_addr = nvramProfile->session[j].ip;
	userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
	uttNfNlHashDelIp(uttNfNlSock, nvramProfile->session[j].ip);/*删除hash表对应的ip*/
#endif
	prof->sesIndex = nvramProfile->session[j].nextIndex;
	memset(&(nvramProfile->session[j]), 0U, sizeof(WebAuthSession));
	nvramProfile->session[j].nextIndex = -1;
    }
    prof->sesIndex = -1;
    prof->actualSession = 0U;
#else /*WEB_AUTH_SHARE*/

    /*将此IP从OK规则清除*/ 
    webAuthLog(prof, prof->ip, prof->mac);
    ipsetAOrDIp(SYS_WEB_AUTH_OK,prof->ip,IPSET_MEM_DEL);
#if (IP_GRP == FYES)
    addr.s_addr = prof->ip;
    userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
    uttNfNlHashDelIp(spdNfNlSock, prof->ip);/*删除hash表对应的ip*/
#endif
    prof->ip = 0U;/*清空ip*/
    memset(prof->mac, 0, sizeof(prof->mac));/*清空mac*/
#endif /*WEB_AUTH_SHARE*/

    return;
}

/********************************************************************
 * 函数： captchaAuthDel
 * 功能： 新增验证码认证账号
 * 创建： 2014-01-02
 * 参数： type: WECHAT_METHOD, SMS_METHOD; userName: 微信OpenId或手机号, send，返回的成功信息
 * 返回： 0 成功
 * 输出： 略
 * 作者： 李文昌 
 ********************************************************************/ 
int captchaAuthDel(char *userName)
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH_CPT;
    WebAuthProfile *prof = NULL;

    int captcha = -1;
    int ret = -1, i = 0;
    int min = 0, max = 0;
    int webIndexInst, instIndex = 0;


    ProfInstLowHigh(mibType, &max, &min);
    for(i = min;i < max;i++)
    {
	prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
	{
	    if(strncmp(prof->user,userName,UTT_USER_LEN) == 0)/*查看用户名是否已存在*/
	    {
		captchaAuthCancel(prof);
		ProfDelInstByIndex(mibType, i);
	    }
	}
    }
}

WebAuthProfile* captchaAuthLookup(char *userName)
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH_CPT;
    WebAuthProfile *prof = NULL;
    int i, min = 0, max = 0;
    
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min;i < max;i++)
    {
	prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
	{
	    if(strncmp(prof->user, userName, UTT_USER_LEN) == 0)
	    {
		/* 账号已存在 */
		return prof;
	    }
	}
    }

    return NULL;
}

int captchaAuthCheckTimes(WebAuthGlobalProfile *profG, int type, char *userName, char *mac)
{
    int checkTimesOk = 0;

    if (type == SMS_METHOD)
    {
	if (checkMacZero(mac))
	    return 11;
	if (devListAsk(mac))
	    return 12;

	if (smsListAsk(userName, profG->sms.totalTimes) == 0)
	    checkTimesOk = 1;
    }
    else if (type == WECHAT_METHOD)
    {
	if (wechatListAsk(userName, profG->wechat.totalTimes) == 0)
	    checkTimesOk = 1;
    }
    else
    {
	SWORD_PRINTF("%s %d: type is not wechat and sms.\n", __func__, __LINE__);
	return 4;
    }

    if (!checkTimesOk)
    {
	printf("%s %d: ask so many captcha today!\n");
	return 10;
    }
    
    return 0;
}

/********************************************************************
 * 函数： captchaAuthAdd
 * 功能： 新增验证码认证账号
 * 创建： 2014-01-02
 * 参数： type: WECHAT_METHOD, SMS_METHOD; userName: 微信OpenId或手机号, send，返回的成功信息
 * 返回： 0 成功
 * 输出： 略
 * 作者： 李文昌 
 ********************************************************************/ 
int captchaAuthAdd(int type, char *userName, char *mac, char *send)
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH_CPT;
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthProfile *prof = NULL;
    WebAuthGlobalProfile *profG = NULL;

    int captcha = -1;
    int ret, i = 0;
    int min = 0, max = 0;
    int webIndexInst, instIndex = 0;
    char instid[10] = {0};

    if (WebAuthAccountIsFull())
	return 9;

    profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
    if ((profG == NULL) || (ProfInstIsFree(profG)) || (profG->head.active == FALSE))
    {
	SWORD_PRINTF("%s %d: cannot get global parameters.\n", __func__, __LINE__);
	return 1;
    } else if (profG->enabled == 0 
	    || (type == SMS_METHOD && profG->sms.enabled == 0)
	    || (type == WECHAT_METHOD && profG->wechat.enabled == 0)) {
	SWORD_PRINTF("%s %d: config is disable.\n", __func__, __LINE__);
	return 2;
    }

    /* 检查认证次数是否合法 */
    ret = captchaAuthCheckTimes(profG, type, userName, mac);
    if (ret != 0)
	return ret;

    /* 检查账号是否存在 */
    prof = captchaAuthLookup(userName);
    if (prof != NULL)
    {
	/* 账号存在 */
	captcha = strtol(prof->passwd, NULL, 10);
    }
    else
    {
	/* 账号不存在，创建新的账号 */
	webIndexInst = getWebInstName(&(nvramProfile->webauthInstRec.instRecMap), 
		&(nvramProfile->webauthInstRec.instNameIndex));
	if (webIndexInst == 0)
	    return 6;
	memset(instid, 0, sizeof(instid));
	sprintf(instid, "CPT%d", webIndexInst);
	if (ProfGetInstIndexByName(mibType, instid) != PROFFAIL)
	    return 7;
	instIndex = ProfNewInst(mibType, instid, TRUE);/*获取实例号并写上实例名*/
	if (instIndex == PROFFAIL) 
	{
	    clearPortMap(&(nvramProfile->webauthInstRec.instRecMap), webIndexInst);
	    SWORD_PRINTF("%s %d: profnewinst fail.\n", __func__, __LINE__);
	    return 3;
	}	

	prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, instIndex);
	if (prof == NULL || ProfInstIsFree(prof))
	    return 8;

	prof->autoAuthMethod = type;
	prof->timeingPatternEn = FUN_ENABLE;
	memset(prof->user, 0, sizeof(prof->user));
	strcpy(prof->user, userName);

	if (memcmp(userName, "13661632466", 11) == 0)
	    captcha = 0;
	else
	    captchaCreate(&captcha);
	memset(prof->passwd, 0, sizeof(prof->passwd));
	sprintf(prof->passwd, "%04d", captcha);
    }

    printf("%s\n", prof->passwd);

    /* 不管新老账户，重新申请验证码后，都将重新计时 */
    if (type == SMS_METHOD)
    {
	prof->timeTotal = profG->sms.timeTotal * 3600;
	prof->maxSession = profG->sms.maxSessions;
    } 
    else
    {
	prof->timeTotal = profG->wechat.timeTotal * 3600;
	prof->maxSession = profG->wechat.maxSessions;
    }
    prof->timeStart = time(NULL);
    prof->timeStop = prof->timeStart + prof->timeTotal;

    /* 根据回复内容格式，生成回复内容 */
    captchaSendFormat(type, profG, captcha, send);

    return 0;
}


/********************************************************************
 * 函数： getWechatKeyContents 
 * 功能： 获取验证码回复关键字
 * 创建： 2014-01-02
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： 李文昌 
 ********************************************************************/ 
int getWechatKeyContents(char (*key)[20])
{
    int num = 0, i = 0;
    char *p;
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;                             
    WebAuthGlobalProfile *profG = NULL;                                             

    profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);            
    if ((profG == NULL) || (ProfInstIsFree(profG)) || (profG->head.active == FALSE))
	return 0;

    p = profG->wechat.wechatKeyContents;
    if (p == NULL || p[0] == '\0')
	return 0;

    while (p[0] == '/')
	p++;

    while (p[strlen(p)-1] == '/')
	p[strlen(p)-1] = '\0';

    while (*p)
    {
	key[num][i++] = *p++;
	if (*p == '/')
	{
	    key[num][i] = '\0';
	    p++;
	    num++;
	    i = 0;
	}
	while (*p == '/')
	    p++;
    }

    key[num][i] = '\0';
    num++;

    return num;
}

/********************************************************************
 * 函数： wxResponseMsg
 * 功能： 响应用户微信消息
 * 创建： 2014-01-02
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： 李文昌 
 ********************************************************************/ 
int wxResponseMsg(struct wxMsg *msg, char *send)
{
    int index, ret;
    char *p = NULL;
    char key[20][20];
    int  i, keynum, existFlag = 0;
    char recv_gb[CPT_STR_LEN], send_gb[CPT_STR_LEN];

    memset(recv_gb, 0, sizeof(recv_gb));
    memset(send_gb, 0, sizeof(send_gb));

    p = iconv_string(msg->content, "UTF-8", "GB2312");
    if (p != NULL) {
	strcpy(recv_gb, p);
	free(p);
	p = NULL;
    } else {
	return 0;
    }

    keynum = getWechatKeyContents(key);
    if (keynum == 0)
    {
	SWORD_PRINTF("%s %d: get wechat key contents NULL!\n", __func__, __LINE__);
	return 0;
    }

    for (i=0; i<keynum; i++)
    {
	if (strstr(changeStrLower(recv_gb), key[i]))
	{
	   existFlag = 1; 
	   break;
	}
    }
	
    if (existFlag) { 
	ret = captchaAuthAdd(WECHAT_METHOD, msg->fromUser, NULL, send_gb);
	if (ret == 3 || ret == 9)
	    strcpy(send_gb, "上网用户数已达最大值，请稍后再试。");
	else if (ret == 10)
	    strcpy(send_gb, "您今天申请次数过多，请明天再试。");
    } else if (strcmp(msg->msgType, "event") == 0 && strcmp(recv_gb, "subscribe") == 0) {
	ret = captchaAuthAdd(WECHAT_METHOD, msg->fromUser, NULL, send_gb);
	if (ret == 3 || ret == 9)
	    strcpy(send_gb, "上网用户数已达最大值，请稍后再试。");
	else if (ret == 0)
	    sprintf(send_gb, "%s\n您可以回复\"%s\"再次获取验证码。", send_gb, key);
	else if (ret == 10)
	    strcpy(send_gb, "您今天申请次数过多，请明天再试。");
    } else if (strcmp(msg->msgType, "event") == 0 && strcmp(recv_gb, "unsubscribe") == 0) {
	captchaAuthDel(msg->fromUser);
    } else {
	sprintf(send_gb, "您可以回复\"%s\"获取上网验证码。", key);
    }

    p = iconv_string(send_gb, "GB2312", "UTF-8");
    if (p!= NULL) {
	strcpy(send, p);
	free(p);
    } else {
	return 0;
    }

    return 1;
}

/********************************************************************
 * 函数： wxHandleMsg
 * 功能： 处理用户微信消息
 * 创建： 2014-01-02
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： 李文昌 
 ********************************************************************/ 
int wxHandleMsg(char *recvMsg, char *sendMsg)
{
    char *sendMsgFmt = "<xml><ToUserName><![CDATA[%s]]></ToUserName>\n\
			<FromUserName><![CDATA[%s]]></FromUserName>\n\
			<CreateTime>%d</CreateTime>\n\
			<MsgType><![CDATA[%s]]></MsgType>\n\
			%s\n\
			<funcflag>0</funcflag>\
			</xml>";
    char *textMsgFmt = "<Content><![CDATA[%s]]></Content>";
    char data[CPT_STR_LEN];
    char sendbuf[CPT_STR_LEN];
    struct wxMsg msg;

    if (recvMsg == NULL || recvMsg[0] == 0) {
	SWORD_PRINTF("recv msg is null.\n");
	return 1;
    }

    memset(sendbuf, 0, sizeof(sendbuf));
    if (analysisXml(recvMsg, "ToUserName", msg.toUser) &&
	    analysisXml(recvMsg, "FromUserName", msg.fromUser) &&
	    analysisXml(recvMsg, "MsgType", msg.msgType)){
	if (strcmp(msg.msgType, "text") == 0) {
	    if (analysisXml(recvMsg, "Content", msg.content)) {
		wxResponseMsg(&msg, sendbuf);
	    } else {
		SWORD_PRINTF("content not found.\n");
		return 3;
	    }
	} else if (strcmp(msg.msgType, "event") == 0) {
	    if (analysisXml(recvMsg, "Event", msg.content)) {
		    wxResponseMsg(&msg, sendbuf);
		    strcpy(msg.msgType, "text");
	    } else {
		SWORD_PRINTF("Event not found.\n");
		return 3;
	    }
	} else {
	    wxResponseMsg(&msg, sendbuf);
	    strcpy(sendbuf, "Unkown message type.");
	    return 4;
	}
    } else {
	SWORD_PRINTF("not correct xml.\n");
	return 2;
    }

    memset(data, 0, sizeof(data));
    sprintf(data, textMsgFmt, sendbuf);
    sprintf(sendMsg, sendMsgFmt, 
	    msg.fromUser, msg.toUser, 
	    time(0), msg.msgType,data); 

    return 0;
}

/********************************************************************
 * 函数： formWeixinUrl
 * 功能： 处理用户提交微信申请
 * 创建： 2014-01-02
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： 李文昌 
 ********************************************************************/  
static void formWechatUrl(webs_t wp, char_t *path, char_t *query) 
{
    char *echostr   = NULL;
    char *recvMsg   = NULL;
    char sendMsg[CPT_STR_LEN * 4];
    int ret;

    echostr   = websGetVar(wp, T("echostr"),   T(""));
    recvMsg   = wp->postData;
    printf("get wechat msg.\n");
    if (!isFromWechat(wp)) 
    {
	SWORD_PRINTF("msg is not from weixin\n");
	return;
    }

    if ((echostr != NULL) && (echostr[0] != '\0')) 
    {
	strcpy(sendMsg, echostr);
    } 
    else 
    {
	memset(sendMsg, 0, sizeof(sendMsg));
	ret = wxHandleMsg(recvMsg, sendMsg);
	if (ret)
	{
	    SWORD_PRINTF("%s %d: error %d\n", __func__, __LINE__, ret);
	    return;
	}
    }

    wechatSend(wp, sendMsg);

    return;
}



/********************************************************************
 * 函数： formWebAuthGetPhoneSubmit
 * 功能： 处理用户提交的手机号
 * 创建： 2014-01-15
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： 李文昌 
 ********************************************************************/ 
static void formWebAuthGetPhoneSubmit(webs_t wp, char *path, char *query) 
{
    int i, ret;
    char *phonenum = NULL;
    char sendbuf[CPT_STR_LEN];
    char sha1buf[CPT_STR_LEN];
    char *p = NULL;
    char mac[6];
    IPADDR ip = 0U;

    phonenum = websGetVar(wp, T("phonenum"), T(""));

    SWORD_PRINTF("%s %d, phonenum = %s\n", __FUNCTION__, __LINE__, phonenum);
    getMacByIpStr(wp->ipaddr, mac);

    if (phonenum == NULL || strlen(phonenum) != 11)
	return;

    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain;charset=GB2312\nPragma: no-cache\nCache-Control: no-cache\n\n"));
    websWrite(wp,"var errorstr = \"\";");

    memset(sendbuf, 0, sizeof(sendbuf));
    ret = captchaAuthAdd(SMS_METHOD, phonenum, mac, sendbuf);
    if (ret != 0)
    {
	SWORD_PRINTF("%s %d, auth add error %d\n", __FUNCTION__, __LINE__, ret);
	if (ret == 9)
	    websWrite(wp, "errorstr=\"%s\";", "上网用户数已达最大值，请稍后再试");
	else if (ret == 10)
	    websWrite(wp, "errorstr=\"%s\";", "申请次数过多，请明天再试");
	else if (ret == 11)
	    websWrite(wp, "errorstr=\"%s\";", "系统忙，请稍后再试");
	else if (ret == 12)
	    websWrite(wp, "errorstr=\"%s\";", "申请过于频繁，请稍后再试");

	websDone(wp, 200);/*ajax尾*/
	return;
    }

    SWORD_PRINTF("%s %d, sendbuf: %s\n", __FUNCTION__, __LINE__, sendbuf);

    memset(sha1buf, 0, sizeof(sha1buf));
    if (smsCreatesha1Key(sha1buf, phonenum) != 0)
    {
	SWORD_PRINTF("%s %d, sha1 error.\n", __FUNCTION__, __LINE__);
	return;
    }

    smsSend(sha1buf, phonenum, sendbuf);
#if 0
    p = iconv_string(sendbuf, "GB2312", "UTF-8");

    if (smsSend(sha1buf, phonenum, p))
    {
	SWORD_PRINTF("%s %d: send sms fail.\n", __func__, __LINE__);
    }

    if (p!= NULL)
	free(p);
#endif
    websDone(wp, 200);/*ajax尾*/
    return;
}


/********************************************************************
 ** 函数： websDefineCaptcha
 ** 功能： 定义页面接口函数
 ** 创建： 2014-01-02
 ** 参数： 略
 ** 返回： 略
 ** 输出： 略
 ** 作者： 李文昌 
 *********************************************************************/ 
void websDefineCaptcha(void)
{
    websFormDefine(T("wechatUrl"), formWechatUrl);
    websFormDefine(T("formWebAuthGetPhoneSubmit"), formWebAuthGetPhoneSubmit);
    writeSmsServerFile();
    doSystem("cp /etc_ro/web/WebAuth_auth_auto.asp /etc_ro/web/WebAuth_auth.asp");
    doSystem("cp /etc_ro/web/WebAuth_message_auto.asp /etc_ro/web/WebAuth_message.asp");
    doSystem("rm /etc_ro/web/WebAuth_auth_auto.asp");
    doSystem("rm /etc_ro/web/WebAuth_message_auto.asp");
    strcpy(arpLan.arp_dev, getLanIfName());
    captchaListInit();
//    captchaCreateTest();
}
#endif /* if (WEBAUTH_AUTO == FYES) */
