
/*
 * 文件名:
 *     wusWebAUth.c
 *
 * 描述:
 *    网页认证交互文件
 *    
 * 作者：
 *     孙剑卿
 *
 * 维护人：
 *     孙剑卿
 *
 * 变更：
 *     1. 初始版本:2012-04-13
 */
#include <stdio.h>
#include <string.h>
#include <mib.h>
#include <sys/sysinfo.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <net/if_arp.h>
#include "typedef.h"
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "uttfunction.h"
#include "profacce.h"
#include "uttMachine.h"
#include "comExeFun.h"
#include "translate.h"
#include "flash_api.h"
#include "mib.h"
#include <../../../lib/libusb-user/usb-user.h>
//#define SWORD_DEBUG
#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif
#if (WEB_AUTH_SHARE == FYES)
#define DEBUG 0
static int searchDelSessionByIp(WebAuthProfile *prof,Uint32 ip);
#endif

#if (WEB_AUTH == FYES)
#ifdef CONFIG_UTT_NETFILTER
extern int uttNfNlSock;
#endif

#if (IP_GRP == FYES)
extern IpGrpProfile* userRefByIpGrp(char *username, ipGrpEntryType userType);
#endif
extern int getIfIp(char *ifname, char *if_addr);
static int getAddreGrpList(int eid, webs_t wp, int argc, char_t **argv);
#if (WEBAUTH_ACCOUNT == FYES)
static struct arpreq arpLan = {
.arp_pa.sa_family = AF_INET,
.arp_ha.sa_family = AF_INET
};
#endif
/*
 * 静态变量提示信息
 */
static char webAuthMsg[MAX_WEB_AUTH_PROFILES] = {0};
static int billing=0 ;
static Uint32 stopData=0u;
static unsigned long leftTime=0u;
#define WEB_FILE_PATH "/etc_ro/web/WebAuth_auth.asp"
#define USER_WEB_FILE_PATH "/ff/WebAuth_auth.asp"
#define WEB_SIZE 1024*1024
static WebAuthProfile* WebAuthUserNameByIp(Uint32 ip);
static void waCancelByIp(Uint32 ip); 
static int waUserAuth(webs_t wp, char *path, char *query); 
static void waCancelByUser(char *user); 
static void WebAuthConfigAdd(webs_t wp);
static void WebAuthConfigModify(webs_t wp);
static void formWebAuthListDel(webs_t wp, char_t *path, char_t *query);
static void formWebAuthListDelAll(webs_t wp, char_t *path, char_t *query);
static void formWebAuthConfig(webs_t wp, char_t *path, char_t *query);
static void formWebAuthGlobalConfig(webs_t wp, char_t *path, char_t *query);
static void formPictureUrl(webs_t wp, char_t *path, char_t *query);
static void formWebAuthHangUp(webs_t wp, char_t *path, char_t *query);
static void formWebAuthUserSubmit(webs_t wp, char *path, char *query); 
#if (WEBAUTH_AUTO == FYES)
static void formWebAuthCaptchaSubmit(webs_t wp, char *path, char *query);
static void formWebAuthFastSubmit(webs_t wp, char *path, char *query);
#endif
static int getWebAuthListData(int eid, webs_t wp, int argc, char **argv);
static int getWebAuthOneInfo(int eid, webs_t wp, int argc, char **argv);
static int getWebAuthGlobal(int eid, webs_t wp, int argc, char **argv);
static int getWebAuthMessage(int eid, webs_t wp, int argc, char **argv); 
static void webAuthSetMsg(const char *pMsg);
static void formWebAuthOk(webs_t wp, char_t *path, char_t *query);
static int aspGetWebAuthSelfHelpEnabled(int eid, webs_t wp, int argc, char **argv);
static int getWebAuthTips(int eid,webs_t wp,int argc, char **argv);
#if (WEBAUTH_ADVANCE == FYES)
//static char picname[64];
static char pageBuf[WEB_SIZE];
static void formDownloadPage(webs_t wp, char_t * path, char_t * query);
static void formImportPage(webs_t wp, char_t * path, char_t * query);
static void formActiveDefPage(webs_t wp, char_t * path, char_t * query);
static void formPictureLoad (webs_t wp, char_t * path, char_t * query);
static void OutputPicture();
#endif
static int getSmsContentData(int eid, webs_t wp, int argc, char **argv);
static int getWechatContentData(int eid, webs_t wp, int argc, char **argv);
#if (WEBAUTH_AUTO == FYES)
static void formWechatPicLoad (webs_t wp, char_t * path, char_t * query);
static void formJumpUrl(webs_t wp, char_t *path, char_t *query);
void smsListDelAll(void);
void wechatListDelAll(void);
#endif
#if (WEBAUTH_SELF_HELP == FYES)
static int aspGetWebAuthSelfHelpData(int eid, webs_t wp, int argc, char **argv);
static void formWebAuthSelfHelpConfig(webs_t wp, char *path, char *query);
#endif
static int replaceStr(char *content, char *srcstr, char destchar);
static int getContactData(int eid, webs_t wp, int argc, char **argv);
extern int saveContactToFile(char *content, const char *path);
#if 1 //(WEB_AUTH_SHARE == FYES)
static int getWebAuthConnListData(int eid, webs_t wp, int argc, char **argv);
static int getWebAuthmaxSessions(int eid, webs_t wp, int argc, char **argv);
#endif
#if (WEB_AUTH_SHARE == FYES)
static void formWebAuthHangUpByIP(webs_t wp, char_t *path, char_t *query);
static void formWebAuthHangUpListByIP(webs_t wp, char_t *path, char_t *query);
#endif
static int getWifiClientList(int eid, webs_t wp, int argc, char **argv);
#if(WIFIDOG_AUTH == FYES)
#if (WHITELIST == 1)
static int getWhiteList(int eid, webs_t wp, int argc, char_t **argv);
static void websWhiteList(webs_t wp, char_t *path, char_t *query);
static void formWhiteListDelAll(webs_t wp, char_t *path, char_t *query);
static void formWhiteListDel(webs_t wp, char_t *path, char_t *query);
#endif
#endif
#if (UTT_KNOWIFI == FYES)
static int aspGetKnowifiConfig(int eid, webs_t wp, int argc, char_t **argv);
static void formKnowifiConfig(webs_t wp, char_t *path, char_t *query);
#if 1
static void formKnowifiAdd(webs_t wp, char_t *path, char_t *query);
static void formKnowifiDel(webs_t wp, char_t *path, char_t *query);
static void formKnowifiDelAll(webs_t wp, char_t *path, char_t *query);
static void formOpenLearn(webs_t wp, char_t *path, char_t *query);
#endif
#endif
/********************************************************************
 * 函数： websDefineWebAuth
 * 功能： 页面与交互转换函数
 * 创建： 2012-04-13
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
extern void websDefineWebAuth(void)
{
	websFormDefine(T("formWebAuthListDel"), formWebAuthListDel);
	websFormDefine(T("formWebAuthListDelAll"), formWebAuthListDelAll);
	websFormDefine(T("formWebAuthConfig"), formWebAuthConfig);
	websFormDefine(T("formWebAuthGlobalConfig"), formWebAuthGlobalConfig);
	websFormDefine(T("formPictureUrl"), formPictureUrl);
#if (WEBAUTH_ADVANCE == FYES)
	websFormDefine(T("formDownloadPage"), formDownloadPage);
	websFormDefine(T("formImportPage"), formImportPage);
	websFormDefine(T("formPictureLoad"), formPictureLoad);
	websFormDefine(T("formActiveDefPage"), formActiveDefPage);
#endif
	websFormDefine(T("formWebAuthHangUp"), formWebAuthHangUp);
	websFormDefine(T("formWebAuthUserSubmit"), formWebAuthUserSubmit);
#if (WEBAUTH_AUTO == FYES)
	websFormDefine(T("formWechatPicLoad"), formWechatPicLoad);
	websFormDefine(T("formWebAuthCaptchaSubmit"), formWebAuthCaptchaSubmit);
	websFormDefine(T("formWebAuthFastSubmit"), formWebAuthFastSubmit);
	websFormDefine(T("formJumpUrl"), formJumpUrl);
#endif
	websFormDefine(T("formWebAuthOk"), formWebAuthOk);
	websAspDefine(T("aspOutputWebAuthListData"), getWebAuthListData);     
	websAspDefine(T("aspOutputTips"), getWebAuthTips);     
	websAspDefine(T("aspOutputWebAuthOneInfo"), getWebAuthOneInfo);     
	websAspDefine(T("aspOutputWebAuthGlobal"), getWebAuthGlobal);     
	websAspDefine(T("aspOutputWebAuthMessage"), getWebAuthMessage);     
#if (IP_GRP == FYES)
    websAspDefine(T("aspOutputAddrexIPGrpList"), getAddreGrpList);
#endif
	websAspDefine(T("aspGetWebAuthSelfHelpEnabled"), aspGetWebAuthSelfHelpEnabled);
#if (WEBAUTH_ACCOUNT == FYES)
	strcpy(arpLan.arp_dev, getLanIfName());
#endif
#if (WEBAUTH_SELF_HELP == FYES)
	websAspDefine(T("aspGetWebAuthSelfHelpData"), aspGetWebAuthSelfHelpData);
	websFormDefine(T("formWebAuthSelfHelpConfig"), formWebAuthSelfHelpConfig);
#endif
	websAspDefine(T("aspOutputSmsContentData"), getSmsContentData);
	websAspDefine(T("aspOutputWechatContentData"), getWechatContentData);
	websAspDefine(T("aspOutputContactData"), getContactData);
#if 1 //(WEB_AUTH_SHARE == FYES)
	websAspDefine(T("aspOutputWebAuthConnListData"), getWebAuthConnListData);
	websAspDefine(T("aspOutputWebAuthmaxSessions"), getWebAuthmaxSessions);
#endif
#if (WEB_AUTH_SHARE == FYES)
	websFormDefine(T("formWebAuthHangUp_IP"), formWebAuthHangUpByIP);
	websFormDefine(T("formWebAuthListHangUp"), formWebAuthHangUpListByIP);
#endif
	websAspDefine(T("outputWifiClient"), getWifiClientList);
#if(WIFIDOG_AUTH == FYES)
#if(WHITELIST == 1)
	websFormDefine(T("websWhiteList"), websWhiteList);
	websFormDefine(T("formWhiteListDel"), formWhiteListDel);
	websFormDefine(T("formWhiteListDelAll"), formWhiteListDelAll);
#endif
#endif
#if (UTT_KNOWIFI == FYES)
	websAspDefine(T("aspOutKnowifiConfig"), aspGetKnowifiConfig);
	websFormDefine(T("formKnowifiConfig"), formKnowifiConfig);
#if 1
	websFormDefine(T("formKnowifiAdd"), formKnowifiAdd);
	websFormDefine(T("formKnowifiDel"), formKnowifiDel);
	websFormDefine(T("formKnowifiDelAll"), formKnowifiDelAll);
	websFormDefine(T("formOpenLearn"), formOpenLearn);

#endif
#endif
}
#if(WIFIDOG_AUTH == FYES)
struct flock* file_lock_wifi(short type, short whence)
{
    static struct flock ret;
    ret.l_type = type ;
    ret.l_start = 0;
    ret.l_whence = whence;
    ret.l_len = 0;
    ret.l_pid = getpid();
    return &ret;
} 
#if(WHITELIST == 1)
/***********************************************************************
 * 函 数 名：   getWhiteList
 * 创建日期：	2014-7-15
 * 修改日期：
 * 作者：       hww
 * 附加说明：	无
***********************************************************************/
static int getWhiteList(int eid, webs_t wp, int argc, char_t **argv)
{
    int min, max, index, num =0;
    MibProfileType mibType = MIB_PROF_WHITE_LIST;
    WhiteListProfile *prof = NULL;
    static int old_num =  0;   
    websWrite(wp,"var DnsLists=new Array();");
    ProfInstLowHigh(mibType, &max, &min);
    num = min;
    for (index = min; index < max; index++)
    {
	prof = (WhiteListProfile *)ProfGetInstPointByIndex(mibType, index);
	if (strcmp(prof->domain, ""))
	{
	    websWrite(wp, "DnsLists[%d] = \"%s\";", num, prof->domain);
	    num++;
	}
	else
	{
	    continue;
	}
    }

    websWrite(wp,"var old_totalrecs =%d;",old_num);
    if(old_num != num){
	old_num = num ;
    }
    websWrite(wp,"var totalrecs=%d;",num);
    websWrite(wp,"var max_totalrecs=%d;",max);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
/***********************************************************************
 * 函 数 名：   websWhiteList
 * 创建日期：	2014-7-16
 * 修改日期：
 * 作者：       hww
 * 附加说明：	无
***********************************************************************/
static void websWhiteList(webs_t wp, char_t *path, char_t *query)
{
    int min, max, index;
    char tmp[100];
    char *rule = websGetVar(wp, T("addHostFilter"), T(""));
    MibProfileType mibType = MIB_PROF_WHITE_LIST;
    WhiteListProfile *prof = NULL;
    ProfChangeType profAction = PROF_CHANGE_ADD;
    struProfAlloc *profhead = NULL;

    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
	/* Blank */
	prof = (WhiteListProfile *)ProfGetInstPointByIndex(mibType, index);
	if (0 == strcmp(prof->domain, ""))
	{
	    break;
	}
    }
    sprintf(tmp, "%d", index);
    index = ProfNewInst(mibType, tmp, FALSE);
    /* Backup */
    ProfBackupByIndex(mibType, profAction, index, &profhead);
    /* Get pointer */
    prof = (WhiteListProfile *)ProfGetInstPointByName(mibType, tmp);
    /* Change value */
    strcpy(prof->domain, rule);
        /* Effect immediately */
    ProfUpdate(profhead);
    /* Release */
    ProfFreeAllocList(profhead);
    /* Write config */
    nvramWriteCommit();
    websRedirect(wp, "WebAuthGlobal.asp");
    return;
}
/***********************************************************************
 * 函 数 名：   formWhiteListDel
 * 创建日期：	2014-7-16
 * 修改日期：
 * 作者：       hww
 * 附加说明：	无
***********************************************************************/
static void formWhiteListDel(webs_t wp, char_t *path, char_t *query)
{
    int min, max, index;
    char *ptr, *PolicyName=NULL;
    MibProfileType mibType = MIB_PROF_WHITE_LIST;
    WhiteListProfile *prof = NULL;
    ProfChangeType profAction = PROF_CHANGE_DEL;
    struProfAlloc *profhead = NULL;

    PolicyName = websGetVar(wp, T("delstr"), T("")); 
    PolicyName = strtok_r(PolicyName,UTT_SPLIT_SIGN_STR,&ptr);
    if(!strcmp(PolicyName,"")) {				    /*空串返回*/
        websRedirect(wp, "WebAuthGlobal.asp");
        return;
    }
    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
	prof = (WhiteListProfile *)ProfGetInstPointByIndex(mibType, index);
	if(!strcmp(prof->domain, PolicyName))			    /*查找域名所在位置*/
	{

	    ProfBackupByIndex(mibType, profAction, index, &profhead);
	    ProfDelInstByIndex(mibType, index);
	    ProfUpdate(profhead);
	    ProfFreeAllocList(profhead);
	    nvramWriteCommit();
	    break;
	}
    }
    websRedirect(wp, "WebAuthGlobal.asp");
}
/***********************************************************************
* 函 数 名：   formWhiteListDelAll
* 创建日期：	2014-7-16
* 修改日期：
* 作者：	hww
* 附加说明：	无
***********************************************************************/
static void formWhiteListDelAll(webs_t wp, char_t *path, char_t *query)
{
   int min, max, index;
   MibProfileType mibType = MIB_PROF_WHITE_LIST;
   WhiteListProfile *prof = NULL;
   ProfChangeType profAction = PROF_CHANGE_DELALL;
   struProfAlloc *profhead = NULL;

   ProfBackupByIndex(mibType, profAction, 0, &profhead);/*发送delall消息*/
   ProfInstLowHigh(mibType, &max, &min);
   for (index = min; index < max; index++)
   {
	   ProfDelInstByIndex(mibType, index);/*删除域名*/
   }
   ProfUpdate(profhead);
   ProfFreeAllocList(profhead);
   nvramWriteCommit();
   websRedirect(wp, "WebAuthGlobal.asp");
}
#endif
#define WDCTL_SOCK    "/tmp/wdctl.sock"
static void getClientFromWifiDog(webs_t wp)
{
#if 1
    FILE *fp = NULL;
    char buf[1025];

    memset(buf, 0, sizeof(buf));
    fp = fopen(WIFI_CLIENT_LIST_FILE, "r");
    if(fp != NULL) {
	//fcntl(fp, F_SETLKW, file_lock_wifi(F_WRLCK|F_RDLCK, SEEK_SET));
	while(fread(buf, sizeof(char), 1024, fp)>0)
	{
	    websWrite(wp,"%s",buf);
	    memset(buf, 0, sizeof(buf));
	}
	//fcntl(fp, F_SETLKW, file_lock_wifi(F_UNLCK, SEEK_SET));
	fclose(fp);
	fp = NULL;
    }
#else
	int	sock;
	struct sockaddr_un	sa_un;
	char	request[16];
	char	buffer[4096];
	int len = 0, written = 0;
	
	/* Connect to socket */
	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	memset(&sa_un, 0, sizeof(sa_un));
	sa_un.sun_family = AF_UNIX;
	strncpy(sa_un.sun_path, WDCTL_SOCK, (sizeof(sa_un.sun_path) - 1));

	if (connect(sock, (struct sockaddr *)&sa_un, 
			strlen(sa_un.sun_path) + sizeof(sa_un.sun_family))) {
		fprintf(stderr, "wdctl: wifidog probably not started (Error: %s)\n", strerror(errno));
		close(sock);
		return ;
	}

	strncpy(request, "client\r\n\r\n", 15);
	len = 0;
	while (len != strlen(request)) {
		written = write(sock, (request + len), strlen(request) - len);
		if (written == -1) {
			fprintf(stderr, "Write to wifidog failed: %s\n",
					strerror(errno));
			close(sock);
			return;
		}
		len += written;
	}

	len = 0;
	while ((len = read(sock, buffer, sizeof(buffer))) > 0) {
		buffer[len] = '\0';
		websWrite(wp,"%s",buffer);
	}

	close(sock);
#endif
	return ;
}
#if (WIFIDOG_NOTICE == FYES) 
/* isWifiNoticeWeb()
 * wifidog 到期通告推送
 * 检测是否访问wifidog通告页面
 * 是，则将其从需通告组和通告记录组中删除
 * 否，则部作品处理
 */
int isWifiNoticeWeb(webs_t wp) {
    int ret = 0;
    char_t *relIp = NULL;
    /*
     * 获取范文通告页面的客户端ip
     **/
    relIp = wp->ipaddr;
    if (wp->path != NULL)
    {
	if(strncmp(wp->path, "/"WIFI_NOTICE_HTML, (sizeof(WIFI_NOTICE_HTML)+1)) == 0) { /* wifidog到期通告 */
		/*清空record组
		 *清空该组可能使该组里面本该通告的用户延迟通告
		 *但可以减少内存消耗。同时减少webserver压力*/
		//ipsetFlushGrp(NOTICE_R_GRP);
		ipsetAOrDIpStr(WIFI_IPSET_NOTICE_RECORD_GROUP, relIp, IPSET_MEM_DEL); /*本次已通告，从通告记录组中删除，繁殖下次重复通告*/
		ipsetAOrDIpStr(WIFI_IPSET_NOTICE_GROUP, relIp, IPSET_MEM_DEL);   /*从通告组中删除，防止下次重复匹配http规则重复通告*/
		ret = 1;
	}
    }

    return ret;
}
#endif /*WIFIDOG_NOTICE*/
#endif
static int getWifiClientList(int eid, webs_t wp, int argc, char **argv)
{
#if(WIFIDOG_AUTH == FYES)
    char *buffer = NULL;
    MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *prof = NULL;
    
    prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
    {
	if(prof->authType == 2) {
	    websWrite(wp, "var uttWifiAuth = 1;");
	} else {
	    websWrite(wp, "var uttWifiAuth = 0;");
	}
    }

    //getClientFromWifiDog(buffer);
    getClientFromWifiDog(wp);
    if(buffer != NULL) {
	websWrite(wp,"%s",buffer);
	free(buffer);
    }
#endif
    return 0;
}

/*****************************************
 *  *函数：_formPictureLoad
 *   *功能：将上传的图片写到flash中
 *    *创建：2014-01-13
 *     **************************************/
static int _formPictureLoad (webs_t wp, char *picName)
{
    char *fn = NULL;
    char *bn = NULL;
    char *chk = NULL;
    char *postData = NULL;
    int  fnlen = 0;
    const char *ErrMsg=NULL;

    a_assert(websValid(wp));

    fn = websGetVar(wp, T("filename"), T(""));
    if (fn != NULL && *fn != '\0') {
	if ((int)(bn = gstrrchr(fn, '/') + 1) == 1) {
	    if ((int)(bn = gstrrchr(fn, '\\') + 1) == 1) {
		bn = fn;
	    }
	}
    }
    fnlen = wp->lenPostData;
    postData = wp->postData;

    wp->postData = NULL;
    wp->lenPostData = 0;


    if(fnlen <= 0  )
    {
	ErrMsg=getTransDataForC(C_LANG_INDEX_INVALID_FILE);
	setTheErrorMsg(ErrMsg);
	return -1;
    }
    if(strlen(bn) >= 32)
    {
	ErrMsg=getTransDataForC(C_LANG_INDEX_PICTURE_NAME_LENGTH);
	setTheErrorMsg(ErrMsg);
	return -1;
    }
    chk = strrchr(bn,'.');
    if(chk == NULL)
    {
	ErrMsg=getTransDataForC(C_LANG_INDEX_PICTURE_FORMAT);
	setTheErrorMsg(ErrMsg);
	return -1;
    }

    if((strcasecmp(chk,".bmp") != 0)&&(strcasecmp(chk,".gif") != 0)&&(strcasecmp(chk,".png") != 0)&&(strcasecmp(chk,".jpg") != 0)&&(strcasecmp(chk,".jpeg") != 0))
    {
	ErrMsg=getTransDataForC(C_LANG_INDEX_PICTURE_FORMAT);
	setTheErrorMsg(ErrMsg);
	return -1;
    }

    if((fnlen > ( 10*1024 * 1024 - 40)) || (NULL == postData) || (fnlen <= 14 ) )
    {
	ErrMsg=getTransDataForC(C_LANG_INDEX_PICTURE_SIZE);
	setTheErrorMsg(ErrMsg);
	return -1;
    }

    postData[fnlen] = '\0';/*去除其他无用的字符*/

    printf("old pic name: %s\nnew pic name: %s\n", picName, bn);

#if (CONFIG_X86_CPU == 1)
    FILE *fp = NULL;
    int size = 0;
    char picPath[40];
    memset(picPath,'\0',sizeof(picPath));
    strcat(picPath,"/ff/");
    strcat(picPath,bn);

    fp = fopen(picPath,"w+");
    if(fp == NULL)
    {
	setTheErrorMsg(T("write picture content error"));
	return -1;
    }else{
	size = fwrite(postData,fnlen+1,1,fp);
	if(0>= size)
	{
	    fclose(fp);
	    setTheErrorMsg(T("write picture content error"));
	    return -1;
	}
	fclose(fp);
    }
    if(picName[0] != 0 && strcmp(picName, bn) != 0)
    {
	doSystem("rm -rf /ff/%s", picName);
    }
    doSystem("cp /ff/%s /etc_ro/web/images/", bn);
#else
    FILE *fp = NULL;
    int size = 0;
    char picPath[40], picPathName[40];
    int i, count, flag = 0;

    flag |= USB_EXIST;
    flag |= SD_EXIST;
    memset(picPath, '\0', sizeof(picPath));
    memset(picPathName, '\0', sizeof(picPathName));
    if (isStorageExist(picPath, &count, flag)) {
	printf("%s %d: picPath: %s, count: %d\n", __func__, __LINE__, picPath, count);
	for (i=0; i<count; i++) {
	    sprintf(picPathName, "%s%s", picPath, bn);
	    printf("picPathName: %s\n", picPathName);
	    fp = fopen(picPathName, "w+");
	    if (fp == NULL) {
		setTheErrorMsg(T("write picture content error"));
		return -1;
	    } else {
		size = fwrite(postData,fnlen+1,1,fp);
		if(0>= size)
		{
		    fclose(fp);
		    setTheErrorMsg(T("write picture content error"));
		    return -1;
		}
		fclose(fp);
	    }
	    if(picName[0] != 0 && strcmp(picName, bn) != 0)
	    {
		doSystem("rm -rf /ff/%s", picName);
	    }
	    doSystem("cp %s /etc_ro/web/images/", picPathName);
	    break;
	}
    } else {
	printf("no usb device found\n");
	setTheErrorMsg(T("未检测到外置存储设备"));
	return -1;
    }
#endif

    if(strcmp(picName, bn) != 0)
	strcpy(picName, bn);

    return  0;
}

#if (WEBAUTH_AUTO == FYES)
/*****************************************
 *  *函数：formPictureLoad
 *   *功能：将上传的图片写到flash中
 *    *创建：2013-01-07
 *     **************************************/
static void formWechatPicLoad (webs_t wp, char_t * path, char_t * query)
{
    char bn[50];
    char *postData = NULL;
    int  fnlen = 0;
    const char *ErrMsg=NULL;

    MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *prof = NULL;
    struProfAlloc *profhead = NULL;

    prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0))
    {
#if (WEBAUTH_AUTO == FYES)
	if(_formPictureLoad(wp, prof->wechat.wechatPicName) < 0) {
	    websRedirect(wp, "WebAuthGlobal.asp");
	    return;
	}	    
#endif

	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
    }

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit(); /*save*/

#if (WEBAUTH_AUTO == FYES)
    doSystem("cp /etc_ro/web/images/%s /etc_ro/web/images/weixinpic.jpg", prof->wechat.wechatPicName);
#endif

    ErrMsg=getTransDataForC(C_LANG_INDEX_PICTURE_IMPORT_SUCCEED);
    setTheRightMsg(ErrMsg);
    websRedirect(wp, "WebAuthGlobal.asp");
    return ;
}
#endif
#if (WEBAUTH_ADVANCE == FYES)
/****************************************
*功能：判断页面文件是否存在
*参数：文件的绝对路径
*返回：0---不存在
*      1---存在
* *************************************/
static int isWebFileExits(char* Path)
{
	int ret = 0;
#if (CONFIG_X86_CPU == 1)
	FILE *fp = NULL;
	fp = fopen(Path,"r");
	if(fp == NULL)
	{
		ret=0;
		doSystem("rm -rf /ff/*.asp");
	}else{
		fclose(fp);
		ret=1;
	}
#else
#endif
	return ret;
}
/****************************************
*功能：判断图片文件是否存在
*参数：文件的绝对路径
*返回：0---不存在
*      1---存在
* *************************************/
static int isPicFileExits(char* Path)
{
	int ret = 0;
#if (CONFIG_X86_CPU == 1)
	FILE *fp = NULL;
	fp = fopen(Path,"r");
	if(fp == NULL)
	{
		ret=0;
		doSystem("rm -f /ff/*.png /ff/*.PNG /ff/*.jpg /ff/*.JPG /ff/*.gif /ff/*.GIF /ff/*.jpeg /ff/*.JPEG");
	}else{
		fclose(fp);
		ret=1;
	}
#else
#endif
	return ret;
}

/*****************************************
 *  *函数：formPictureLoad
 *   *功能：将上传的图片写到flash中
 *    *创建：2013-01-07
 *     **************************************/
static void formPictureLoad (webs_t wp, char_t * path, char_t * query)
{
    char *fn = NULL;
    char bn[50];
    char *chk = NULL;
    char *postData = NULL;
    int  fnlen = 0;
    const char *ErrMsg=NULL;


    MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
    prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0))
    {
	if (_formPictureLoad(wp, prof->picName) < 0) {
	    websRedirect(wp, "WebAuthGlobal.asp");
	    return;
	}

	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
	if (prof->enpicture < 2)
	{
	    prof->enpicture = 1;/*选中简易配置选项中的导入图片*/
	}else{
	    prof->enpicture = 3;/*选中高级配置选项中的导入图片*/
	}
    }
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit(); /*save*/

    ErrMsg=getTransDataForC(C_LANG_INDEX_PICTURE_IMPORT_SUCCEED);
    setTheRightMsg(ErrMsg);
    websRedirect(wp, "WebAuthGlobal.asp");
    return ;
}
#endif
/******************************************************************
 *函数：getWebAuthTips
 *功能：输出图片路径及提示信息
 *创建：2013-01-07
 *参数：交互固定输入参数
 *返回：略
 *输出：略
 * ***************************************************************/
static int getWebAuthTips(int eid,webs_t wp,int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *prof = NULL;

    prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);

#if (WEBAUTH_ADVANCE == FYES)
    char picname[64];
    char picPath[40];
    memset(picPath,'\0',sizeof(picPath));
    strcat(picPath,"/ff/");
    strcat(picPath,prof->picName);
    memset(picname,0,sizeof(picname));
    if(((prof->enpicture == 1)||(prof->enpicture == 3))&&(prof->active_pic == 1)&&(isPicFileExits(picPath)) == 1){
	sprintf(picname,"images/%s",prof->picName);
    }
    websWrite(wp, "var picturedir=\"%s\";", picname);
#else
    websWrite(wp, "var picturedir=\"\";");
#endif
    websWrite(wp, "var tipstitles=\"%s\";", prof->tipstitle);
    websWrite(wp, "var tipsinfos=\"%s\";", prof->tipsinfo);
    return 0;
}
#if (WEBAUTH_ADVANCE == FYES)
/*******************************************************************
 *功能:从分区中读出用户上传的web认证页面
 *函数:outPutUserPage
 *参数:空
 *输出:略
 *返回:实际读到的字节数
 *创建:20130618
 * *****************************************************************/
static int outPutUserPage()
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
	WebAuthGlobalProfile *prof = NULL;
	prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
	int ret=0;
	memset(pageBuf,0,WEB_SIZE);
#if (CONFIG_X86_CPU == 1)
	int fd=0;
	char webPath[40];
	memset(webPath,0,sizeof(webPath));
	if(strlen(prof->pageName) > 4){
		sprintf(webPath,"/ff/%s",prof->pageName);
		fd = open(webPath,O_RDONLY);
		if(0 >= fd)
		{
			ret = 0;
		}else{
			ret = read(fd,pageBuf,sizeof(pageBuf));
			close(fd);
		}
	}else{
		ret = 0;
	}
#else
#endif
	return ret;
}
  
/*******************************************************************
 *函数：formDownloadPage
 *功能：下载认证页面
 *参数：交互固定输入参数
 *返回：略
 *输出：略
 *创建：2013-06-08
 *******************************************************************/
static void formDownloadPage(webs_t wp, char_t * path, char_t * query)
{
	int ret = 0;
	int fd;
	char webpagename[32];
	const char *ErrMsg=NULL;
	memset(pageBuf,'\0',WEB_SIZE);
	fd = open(WEB_FILE_PATH,O_RDONLY);
	if(0 >= fd)
	{
		ErrMsg=getTransDataForC(C_LANG_INDEX_READ_WEBAUTH_PAGE_ERROR);
		setTheErrorMsg(ErrMsg);
	}
	else
	{
	    ret = read(fd,pageBuf,WEB_SIZE);
		if(0 >= ret)
		{
		ErrMsg=getTransDataForC(C_LANG_INDEX_WRITE_FILE_FAILED);
		setTheErrorMsg(ErrMsg);
		}
		close(fd);
	}
	memset(webpagename,0,sizeof(webpagename));
	MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
	WebAuthGlobalProfile *prof = NULL;
	prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
	if((strlen(prof->pageName) >4)&&(prof->enDefPage == 0)){
	strcpy(webpagename,prof->pageName);
	}else{
	strcpy(webpagename,"WebAuth_auth.asp");
	}
	/*下载*/
	wimDownloadFile(wp,webpagename,pageBuf,strlen(pageBuf));
	return;
}
/*******************************************************
 *函数：formActiveDefPage
 *功能：启用默认web认证页面
 *参数：交互固定输入参数
 *返回：略
 *输出：略
 *创建：20130608
 * *****************************************************/
static void formActiveDefPage(webs_t wp, char_t * path, char_t * query)
{
	MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
	WebAuthGlobalProfile *prof = NULL;
	struProfAlloc *profhead = NULL;
	char webPath[40];
	memset(webPath,'\0',sizeof(webPath));
	strcat(webPath,"/ff/");
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0))
	{
		ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
		prof->enDefPage = 1;/*启用默认认证页面*/
		if(strlen(prof->pageName) > 4)
		{
			strcat(webPath,prof->pageName);
			doSystem("rm -f %s",webPath);
		}
		memset(prof->pageName,'\0',sizeof(prof->pageName));
	}
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit(); /*save*/

	doSystem("cp -rf /etc_ro/web/WebAuth_auth_def.asp /etc_ro/web/WebAuth_auth.asp");
	websRedirect(wp, "WebAuthGlobal.asp");
	return;
}
/*******************************************************
 *函数：effectUserPage
 *功能：将在ff中的用户上传的认证页面拷贝到内存中
 *参数：空
 *返回：略
 *输出：略
 *创建：20130608
 * *****************************************************/
static void effectUserPage()
{

	MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
	WebAuthGlobalProfile *prof = NULL;
	prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
	if(strlen(prof->pageName) >4)
	{
#if (CONFIG_X86_CPU == 1)
		doSystem("cp /ff/%s /etc_ro/web/WebAuth_auth.asp",prof->pageName);
#else
#endif
	}
}

/*************************************************************
 *函数：formImportPage
 *功能：将用户上传的认证页面写入到分区，并替换当前的认证页面
 *参数：交互固定输入参数
 *输出：略
 *返回：略
 *创建：20130608
 * ***********************************************************/
static void formImportPage(webs_t wp, char_t * path, char_t * query)
{
	char *fn = NULL;
	char *bn = NULL;
	char *chk = NULL;
	char *postData = NULL;
	int  fnlen = 0;
	FILE *fp=NULL;
	int size = 0;
	const char *ErrMsg=NULL;
	a_assert(websValid(wp));

    fn = websGetVar(wp, T("filename"), T(""));
     if (fn != NULL && *fn != '\0') {
     if ((int)(bn = gstrrchr(fn, '/') + 1) == 1) {
         if ((int)(bn = gstrrchr(fn, '\\') + 1) == 1) {
         bn = fn;
         }
	 }
	 }
     fnlen = wp->lenPostData;
     postData = wp->postData;

     wp->postData = NULL;
     wp->lenPostData = 0;
	if(fnlen <= 0  )
    {
		ErrMsg=getTransDataForC(C_LANG_INDEX_INVALID_FILE);
		setTheErrorMsg(ErrMsg);
		websRedirect(wp, "WebAuthGlobal.asp");
		return;
	}
	if(strlen(bn) >= 32)
	{
		ErrMsg=getTransDataForC(C_LANG_INDEX_PICTURE_NAME_LENGTH);
		setTheErrorMsg(ErrMsg);
		websRedirect(wp, "WebAuthGlobal.asp");
		return;
	}

	chk = strrchr(bn,'.');
	if(chk == NULL)
	{
		ErrMsg=getTransDataForC(C_LANG_INDEX_INVALED_FILE_FORMAT);
		setTheErrorMsg(ErrMsg);
		websRedirect(wp, "WebAuthGlobal.asp");
		return;
	}
	if(strcmp(chk,".asp") == 0)
	{

		if(fnlen > WEB_SIZE || (NULL == postData))
		{
			ErrMsg=getTransDataForC(C_LANG_INDEX_WEBAUTH_PAGE_SIZE_ERROR);
			setTheErrorMsg(ErrMsg);
			websRedirect(wp, "WebAuthGlobal.asp");
			return;
		}

    postData[fnlen] = '\0';/*去除其他无用的字符*/

#if (CONFIG_X86_CPU == 1)
	char webPath[40];
	memset(webPath,'\0',sizeof(webPath));
	strcat(webPath,"/ff/");
	strcat(webPath,bn);
	fp = fopen(webPath,"w+");
	if(fp == NULL)
	{
		printf("%s,%d\n",__func__,__LINE__);
		perror(webPath);
        setTheErrorMsg(T("write WebAuth Page content error"));
		websRedirect(wp, "WebAuthGlobal.asp");
        return;
	}else{
		size =	fwrite(postData,fnlen+1,1,fp);
		if(0>= size)
		{
		printf("%s,%d\n",__func__,__LINE__);
			fclose(fp);
			setTheErrorMsg(T("write WebAuth Page content error"));
			websRedirect(wp, "WebAuthGlobal.asp");
		    return;
		}
		fclose(fp);
		fp = NULL;
	}
#else
#endif
	MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
	WebAuthGlobalProfile *prof = NULL;
	struProfAlloc *profhead = NULL;
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0))
	{
		ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
		if (prof->enpicture == 1 || prof->enpicture == 3)
		{
			prof->enpicture = 3;/*选中高级配置选项中的导入图片*/
		}else{
			prof->enpicture = 2;/*选中高级配置选项中的图片链接*/
		}
		prof->enDefPage = 0;/*不启用默认web认证页面*/
		if(strcmp(prof->pageName,bn) != 0)
		{
			if(prof->pageName[0] != '\0')
			{
#if (CONFIG_X86_CPU == 1)
			doSystem("rm -f /ff/%s",prof->pageName);
#else
#endif
			}
		strcpy(prof->pageName, bn);
		}
	}
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit(); /*save*/

	effectUserPage();/*将用户上传的页面拷贝到内存中*/
    ErrMsg=getTransDataForC(C_LANG_INDEX_WEBAUTH_PAGE_IMPORT_SUCCEED);
    setTheRightMsg(ErrMsg);
    websRedirect(wp, "WebAuthGlobal.asp");
	return;
   }
   else
   {
		ErrMsg=getTransDataForC(C_LANG_INDEX_INVALED_FILE_FORMAT);
		setTheErrorMsg(ErrMsg);
		websRedirect(wp, "WebAuthGlobal.asp");
		return;
   }
}
#endif
#if 0

/********************************************************************
 * 函数： IsWebAuthOpen
 * 功能： 判断WEB认证是否开启
 * 创建： 2013-03-07
 * 参数： 空
 * 返回： 
 * 1	-   开启
 * 0	-   未开启
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int IsWebAuthOpen(void)
{
    int ret = 0;
    WebAuthGlobalProfile *prof = NULL;

    prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(MIB_PROF_WEB_AUTH_GLOBAL, 0);
    if((ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE)) {
	ret = 1;
    }
    return ret;
}
#endif

#endif
#if (NOTICE == FYES)
/********************************************************************
 * 函数： IsDnsNoticeOpen
 * 功能： 判断域名过滤和域名过滤通告是否开启
 * 创建： 2013-03-06
 * 参数： 空
 * 返回： 
 * 1	-   都开启
 * 0	-   二者至少一个未开启
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int IsDnsNoticeOpen(void)
{
    int ret = 0;
    SystemProfile *profSys = NULL;
    NoticeProfile *prof = NULL;

    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0); 
    if (profSys->dnsFilterEn == FUN_ENABLE)  {
	prof = (NoticeProfile*)ProfGetInstPointByIndex(MIB_PROF_NOTICE, 2);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
	    if (prof->head.active == 1)
	    ret = 1;
	}
    }
    return ret;
}
#endif

#if (DDNS == FYES)
/********************************************************************
 * 函数： isDDNSDomainName
 * 功能： 判断host值是不是路由器支持的ddns域名
 * 创建： 2013-03-06
 * 参数： 
	host	-	需要判断的host值
 * 返回： 
 *	1	-	是支持的ddns域名
 *	0	-	不是支持的ddns域名
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int isDDNSDomainName(char *host) 
{
	int ret = 0;

	if ((strstr(host, ".iplink.com.cn") != NULL) || (strstr(host, ".3322.org") != NULL)) {
		ret = 1;
	} 

	return ret;
}
#endif
/********************************************************************
 * 函数： redirectWAPage
 * 功能：判断本次访问是否需跳转到网页认证页面
 * 创建： 2012-04-13
 * 参数： 
 * wp	-   访问goahead用户信息结构体
 * 返回： 
 * 1	-   访问网页认证页面
 * 0	-   访问非网页认证页面
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
int redirectWAPage(webs_t wp)
{
#if 0
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
#endif

#if (BRIDGE == FYES)
    MibProfileType mibTypeBR= MIB_PROF_BRIDGE;
    BridgeProfile *profBR= NULL;

    profBR = (BridgeProfile *)ProfGetInstPointByIndex(mibTypeBR,0);

    //桥模式下不需要认证 

    if((profBR!= NULL) && (profBR->head.active== 1))
    {
	return 0;
    }
#endif


    struct in_addr addr;
    int ret = 0;
    char lanIp[16];
    char url[40];
    char *colon = NULL;
#if 0
    int i = 0 ;
#endif
    int isIp = 0;

    memset(url, 0, sizeof(url));
    memset(lanIp, 0, sizeof(lanIp));
    if (wp->httpHeadHost == NULL) {
	return ret;
    } else {
	colon = strstr(wp->httpHeadHost, ":");
	if (colon != NULL) {
	    *colon = '\0';
	}
    }
    SWORD_PRINTF("inet_aton(wp->httpHeadHost, &addr) = %d\n", inet_aton(wp->httpHeadHost, &addr));
#if 0
    getIfIp(getLanIfName(), lanIp);
    isIp = inet_aton(wp->httpHeadHost, &addr);
#if (MULTI_LAN == FYES)
    /*这里web认证要判断 host 是否为 lan口ip地址
     * 是因为 可能web认证时 pc的源ip被记录到了SYS_WEB_AUTH_WEB 组
     * 但是 三次握手后  发现本次 http请求非法（如为迅雷发的post包）
     * 然后就不会触发认证 页面的响应
     * 那么这时 若 pc访问web页面 就可能无法访问 而是跳出 认证页面*/
    if ((isIp == 0) || (ipsetTestIpStr(SYS_LAN_GROUP, wp->httpHeadHost) == 0))
#else
    if ((isIp == 0) || (strcmp(wp->httpHeadHost, lanIp) != 0))
#endif
    {
#if (WEB_AUTH == FYES)
	/*只有WEB认证开启且地址在记录组中才需要弹出认证页面*/
	if((IsWebAuthOpen() == 1) && (ipsetTestIpStr(SYS_WEB_AUTH_WEB, wp->ipaddr) == 1))
	{
	    websRedirect(wp, "WebAuth_auth.asp");
	    ret = 1;
	} else 
#endif
#endif
#if (NOTICE == FYES)
	{
#if 0
	    for(i = 1; i <= getWanSlotCount(); i++) {
		SWORD_PRINTF("wp->httpHeadHost = %s,i = %d\n", wp->httpHeadHost, i);
		profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, i);
		if ((profIF != NULL) && (profIF->head.active == 1)) 
		{
		    SWORD_PRINTF("addr.s_addr is %x, profIF->ether.sta.ip is %x\n", addr.s_addr, profIF->ether.sta.ip);
		    if (addr.s_addr == profIF->ether.sta.ip)
		    {
			return ret;
		    }
		}
	    }
#endif
	    if (IsDnsNoticeOpen() == 0) {/*判断域名过滤和域名过滤通告是否同时开启*/
		return ret;
	    } 

#if (DDNS == FYES)
	    /*
	     * 如果host是路由器支持的ddns域名，则返回
	     */
	    if (isDDNSDomainName(wp->httpHeadHost) == 1) {
		    return ret;
	    }
#endif
	    isIp = inet_aton(wp->httpHeadHost, &addr);

	    if(isIp == 0)
	    {/*域名过滤通告 host肯定不可能是ip
	       但应注意我们把 访问到web的 所有host 非ip 类型的 http请求
	       当成了 通告
	       这样 若 真的添加一个域名  对应我们的设备ip地址
	       则在访问 这个域名的时候就会触发域名过滤通告*/
		getIfIp(getLanIfName(), lanIp);
		sprintf(url, "http://%s/Notice3.asp", lanIp);
		websRedirect(wp, url);
		ret = 1;
	    }
	}
#endif
#if 0
    }
#endif

    return ret;
}
#if (WEB_AUTH == FYES) 
/********************************************************************
 * 函数： isWebAuthWeb
 * 功能： 判断访问的页面是否网页认证页面，并根据不同页面做相关处理
 * 创建： 2012-04-13
 * 参数： 
 * wp	-   访问goahead用户信息结构体
 * 返回： 
 * 1	-   访问网页认证页面
 * 0	-   访问非网页认证页面
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
extern int isWebAuthWeb(webs_t wp) 
{
	int ret = 1;
	if (wp->path != NULL)
	{
		/*网页认证页面*/
		if(strcmp(wp->path, "/WebAuth_auth.asp") == 0) 
		{ 
#if 0
			if (ipsetTestIpStr(SYS_WEB_AUTH_WEB, wp->ipaddr) == 1)
			{
				/*删除需认证记录 */
				ipsetAOrDIpStr(SYS_WEB_AUTH_WEB, wp->ipaddr, IPSET_MEM_DEL);
			}
#endif
			ret = 1;
		}
		else 
		    if(strcmp(wp->path, "/WebAuth_message.asp") == 0) 
		{
			/*网页认证注销页面*/
			ret = 1;
		}
		else
		{
			ret = 0;
		}
	}
	else
	{
		ret = 0;
	}
	return ret;
}
/********************************************************************
 * 函数： _waCancelByIp
 * 功能： 用户注销函数。根据用户IP地址注销账号
 * 创建： 2014-01-16
 * 参数： ip - 	用户IP地址
 * 返回： 略
 * 输出： 略
 * 作者： 李文昌 
 ********************************************************************/ 
static void _waCancelByIp(MibProfileType mibType, Uint32 ip) 
{
	WebAuthProfile *prof = NULL;
	int min = 0, max = 0;
	int i = 0;
#if (IP_GRP == FYES)
	struct in_addr addr;
#endif
#if (WEB_AUTH_SHARE == FYES)
	int flag = 0;
#endif

	ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++) 
	{
		
		prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
		if((prof != NULL) && (ProfInstIsFree(prof) == 0) 
				&& (prof->head.active == TRUE))
		{
#if (WEB_AUTH_SHARE == FYES)
		    if(prof->actualSession > 0) {
		    flag = searchDelSessionByIp(prof,ip);
		    if(flag == 1)
		    {
			prof->actualSession -= 1;
#if DEBUG
			printf("%s-%d: prof->user =%s, prof->maxSession =%d,prof->actualSession =%d\n",__func__,__LINE__,
				prof->user,prof->maxSession,prof->actualSession);
#endif
			break;
		    }
		    }
#if DEBUG
	    printf("%s-%d: prof->user =%s,prof->maxSession =%d, prof->actualSession =%d, prof->sesIndex =%d\n",__func__,__LINE__,
		    prof->user,prof->maxSession,prof->actualSession,prof->sesIndex);
#endif
#else /*WEB_AUTH_SHARE*/
			/*根据IP查找对应账号*/
			if(prof->ip == ip)
			{
				/*将此IP从OK规则清除*/
				ipsetAOrDIp(SYS_WEB_AUTH_OK,ip,IPSET_MEM_DEL); 
#if (IP_GRP == FYES)
				addr.s_addr = prof->ip;
				SWORD_PRINTF("%s-----%d, inet_ntoa(addr) is %s\n", __func__, __LINE__,  inet_ntoa(addr));
				userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
				uttNfNlHashDelIp(uttNfNlSock, prof->ip);/*删除hash表对应的ip*/
#endif
				break;
			}
#endif /*WEB_AUTH_SHARE*/
		}
		
	}
#if (WEB_AUTH_SHARE == FYES)
#else
	/*找到*/
	if(i != max) 
	{
		/*清除IP*/
		prof->ip = 0U;
	}
#endif
}
/********************************************************************
 * 函数： waCancelByIp
 * 功能： 用户注销函数。根据用户IP地址注销账号
 * 创建： 2012-04-17
 * 参数： ip - 	用户IP地址
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void waCancelByIp(Uint32 ip) 
{
	MibProfileType mibType = MIB_PROF_WEB_AUTH;

	_waCancelByIp(mibType, ip);

#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
	mibType = MIB_PROF_WEB_AUTH_CPT;
	_waCancelByIp(mibType, ip);
#endif
	return;
}
#if (WEB_AUTH_SHARE == FYES)
/********************************************************************
 * 函数： searchDelSessionByIp
 * 功能： 根据用户IP地址查找账号会话信息，查找到则注销会话
 * 创建： 2013-09-24
 * 参数： ip - 	用户IP地址
 * 返回： 
 *	  1  -  查到会话且注销
 *	  0  -  未查找到
 * 输出： 略
 * 作者： guo.deyuan
 ********************************************************************/ 
static int searchDelSessionByIp(WebAuthProfile *prof,Uint32 ip)
{
    int flag = 0;
    int prev =-1;
    int i=0, j=-1;
#if (IP_GRP == FYES)
	struct in_addr addr;
#endif
#if DEBUG
	    printf("%s-%d: prof->user =%s,prof->maxSession =%d, prof->actualSession =%d, prof->sesIndex =%d\n",__func__,__LINE__,
		    prof->user,prof->maxSession,prof->actualSession,prof->sesIndex);
#endif
    if(checkSessionIndex(prof->sesIndex) == 1)
    {
	j = prof->sesIndex;
	if(nvramProfile->session[j].ip == ip) {
	    webAuthLog(prof, ip, nvramProfile->session[j].mac);
	    prof->sesIndex = nvramProfile->session[j].nextIndex;
	    memset(&(nvramProfile->session[j]), 0U, sizeof(WebAuthSession));
	    nvramProfile->session[j].nextIndex = -1;
	    flag = 1;
	} else {
	    for(i=0;i<prof->actualSession;i++)
	    {
		prev = j;
		j = nvramProfile->session[j].nextIndex;
		/*根据IP查找对应账号*/
		if(checkSessionIndex(j) != 1)
		{
		    break;
		}
		if(nvramProfile->session[j].ip == ip) {
		    webAuthLog(prof, ip, nvramProfile->session[j].mac);
		    nvramProfile->session[prev].nextIndex = nvramProfile->session[j].nextIndex;
		    memset(&(nvramProfile->session[j]), 0U, sizeof(WebAuthSession));
		    nvramProfile->session[j].nextIndex = -1;
		    flag = 1;
		    break;
		}
	    }
	}
	if(flag == 1)
	{
	/*将此IP从OK规则清除*/
	ipsetAOrDIp(SYS_WEB_AUTH_OK,ip,IPSET_MEM_DEL); 
#if (IP_GRP == FYES)
	addr.s_addr = ip;
#if DEBUG
	printf("%s-%d: prof->user =%s find and del inet_ntoa(addr) =%s\n",__func__,__LINE__,prof->user,inet_ntoa(addr));
#endif
	userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
	uttNfNlHashDelIp(uttNfNlSock, ip);/*删除hash表对应的ip*/
#endif
	}
    }
    return flag;
}
/********************************************************************
 * 函数： searchSessionByIp
 * 功能： 根据用户IP地址查找账号会话信息
 * 创建： 2013-09-24
 * 参数： ip - 	用户IP地址
 * 返回： 
 *	  1  -  查到会话
 *	  0  -  未查找到
 * 输出： 略
 * 作者： guo.deyuan
 ********************************************************************/ 
extern int searchSessionByIp(WebAuthProfile *prof,Uint32 ip)
{
    int flag = 0;
    int i=0,j=-1;

    j = prof->sesIndex;
    for(i=0;i<prof->actualSession;i++)
    {
	if(checkSessionIndex(j) != 1)
	{
	    break;
	}
	if(nvramProfile->session[j].ip == ip)
	{
	    flag = 1;
	    break;
	}
	j = nvramProfile->session[j].nextIndex;
    }
#if DEBUG
    if(flag == 1) {
	printf("%s-%d: find session, ip =0x%x, prof user =%s\n",__func__,__LINE__,ip,prof->user);
    }
#endif
    return flag;
}
/********************************************************************
 * 函数： getActualSession
 * 功能： 获取所有账号的实际会话总数
 * 创建： 2013-09-24
 * 参数： null
 * 返回： 总会话数 
 * 输出： 略
 * 作者： guo.deyuan
 ********************************************************************/ 
static int getActualSession()
{
	MibProfileType mibType = MIB_PROF_WEB_AUTH;
	WebAuthProfile *prof = NULL;
	int min = 0, max = 0;
	int i = 0;
	int count = 0;
	
	/* 获得账号profile*/
	ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++) 
	{
		prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
		if((prof != NULL) && (ProfInstIsFree(prof) == 0) 
				&& (prof->head.active == TRUE))
		{
		    count += prof->actualSession;
		}
	}
#if DEBUG
	printf("%s-%d: web auth all actual sessions =%d\n",__func__,__LINE__,count);
#endif
	return count;
}
/********************************************************************
 * 函数： checkSessionNum
 * 功能： 连接时检测账号会话数是否合法
 * 创建： 2013-09-24
 * 参数： 
 * 返回： 
 * 1		非法
 * 0		合法
 * 输出： 略
 * 作者： guo.deyuan
 ********************************************************************/ 
static int checkSessionNum(WebAuthProfile *prof) {
    int flag = 1;
#if DEBUG
    printf("%s-%d: prof->actualSession =%d,prof->maxSession =%d,MAX_WEB_AUTH_SESSION =%d\n",__func__,__LINE__,
	    prof->actualSession,prof->maxSession,MAX_WEB_AUTH_SESSION);
#endif
    if((prof->actualSession >= prof->maxSession)) {
#if DEBUG
	printf("%s-%d: session is greater than prof max session ",__func__,__LINE__);
#endif
	flag = 1;
    } else if(getActualSession()>=MAX_WEB_AUTH_SESSION) {
#if DEBUG
	printf("%s-%d: session is greater than web auth max session ",__func__,__LINE__);
#endif
	flag = 2;
    } else {
#if DEBUG
	printf("%s-%d: profile account session is valid.\n",__func__,__LINE__);
#endif
	flag = 0;
    }
    return flag ;
}
#if 0
static WebAuthSession* webAuthSessionInit()
{
    WebAuthSession *session  = NULL;
    
    session = (WebAuthSession *)malloc(sizeof(WebAuthSession));
    if(session != NULL)
    {
	memset(session, 0U, sizeof(WebAuthSession));
	session->next = NULL;
	session->compareMacEn = FUN_DISABLE;
    }
    return session;
}
#endif
static int searchFreeSessionIndex()
{
    int index = -1;
    int i=0;
    for(i=0;i<MAX_WEB_AUTH_SESSION;i++)
    {
	if(nvramProfile->session[i].ip == 0U)
	{
	    nvramProfile->session[i].nextIndex = -1;
	    index = i;
	    break;
	}
    }

    return index;
}
#if 0
static int checkSessionIndex(int index)
{
    int flag = 0;
    if((index >= 0)&&(index < MAX_WEB_AUTH_SESSION))
    {
	flag = 1;
    }
    return flag;
}
#endif

/********************************************************************
 *  * 函数： _getWebAuthConnListData
 *   * 功能： 网页认证连接状态页面输出账号相关信息
 *    * 创建： 2014-01-08
 *     * 参数： 略
 *      * 返回： 略
 *       * 输出： 略
 *        * 作者： 李文昌
 *         ********************************************************************/ 
static int _getWebAuthConnListData(webs_t wp, MibProfileType mibType, int *num)
{
    WebAuthProfile *prof = NULL;
    int min = 0, max = 0;
    int i = 0, j = -1, k = 0;

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) 
    {
	prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
	{
#if 0
	    printf("%s %d:........user: %s sesIndex: %d actualSession: %d\n", 
		    __func__, __LINE__, prof->user, prof->sesIndex, prof->actualSession);
#endif
	    j = prof->sesIndex;
	    for(k=0;k<prof->actualSession;k++)
	    {
		if(checkSessionIndex(j) != 1)
		{
		    break;
		}
		websWrite(wp,"sesIndexs[%d] = \"%d\";",*num,*num);
#if (WEBAUTH_AUTO == FYES)
		if (prof->autoAuthMethod == WECHAT_METHOD)
		    websWrite(wp,"WebAuthNames[%d] = \"微信用户\";",*num);
		else
#endif
		    websWrite(wp,"WebAuthNames[%d] = \"%s\";",*num,prof->user);
		websWrite(wp,"IPAddress[%d] = \"%N\";",*num,nvramProfile->session[j].ip);
		websWrite(wp,"maxSessions[%d] = %d;",*num,prof->maxSession);
		websWrite(wp,"actualSessions[%d] = %d;",*num,prof->actualSession);
		j = nvramProfile->session[j].nextIndex;
		(*num)++;
	    }
	}
    }

}
/********************************************************************
 * 函数： getWebAuthConnListData
 * 功能： 网页认证连接状态页面输出相关信息
 * 创建： 2013-09-24
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： guo.deyuan
 ********************************************************************/ 
static int getWebAuthConnListData(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH;
    WebAuthProfile *prof = NULL;
    int min = 0, max = 0;
    int i = 0, j = -1, k = 0;
    int num = 0;
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
    MibProfileType mibTypeC = MIB_PROF_WEB_AUTH_CPT;
#endif

    int tmCurrent = time(0);
    websWrite(wp,"var sesIndexs =new Array();");
    websWrite(wp,"var WebAuthNames = new Array();");
    websWrite(wp,"var IPAddress = new Array();");
    websWrite(wp,"var maxSessions = new Array();");
    websWrite(wp,"var actualSessions = new Array();");
    websWrite(wp, T("\n"));
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
    _getWebAuthConnListData(wp, mibType, &num);
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
    _getWebAuthConnListData(wp, mibTypeC, &num);
#endif
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
    /*会覆盖错误信息，改为在页面上声明
      getTheErrorMsg(eid, wp, argc, argv);
      */
    websWrite(wp, "var totalrecs = %d;",num);
    websWrite(wp,"var maxWebAuthSes = %d;\n",MAX_WEB_AUTH_SESSION);
    return 0;
}
static int getWebAuthmaxSessions(int eid, webs_t wp, int argc, char **argv)
{
	websWrite(wp,"var maxWebAuthSes = %d;\n",MAX_WEB_AUTH_SESSION);
	return 0;
}

/********************************************************************
 * 函数： formWebAuthHangUpByIP
 * 功能： 挂断指定账号IP
 * 创建： 2013-11-28
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： guo.deyuan
 ********************************************************************/ 
static void formWebAuthHangUpByIP(webs_t wp, char_t *path, char_t *query)
{
	char_t *ipStr = NULL; 
	const char *ErrMsg = NULL;

	a_assert(wp);

	/*
	 * 删除指定IP
	 */
	ipStr = websGetVar(wp,T("hangup"),T("")); 
#if DEBUG
	printf("%s-%d: ipStr= %s\n", __func__, __LINE__, ipStr);
#endif
#if 1
	if (ipStr != NULL) 
	{
	    IPADDR ipAddress;
	    ipAddress = inet_addr((const char *)ipStr);
	    /*
	     *  清除认证
	     */
	    waCancelByIp(ipAddress);
	}
#endif

	websRedirect(wp, "WebAuthStatus.asp");
	return;
}

/********************************************************************
 * 函数： formWebAuthHangUpListByIP
 * 功能： 页面挂断按钮调用函数
 * 创建： 2013-11-28
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： guo.deyuan
 ********************************************************************/ 
static void formWebAuthHangUpListByIP(webs_t wp, char_t *path, char_t *query)
{
	MibProfileType mibType = MIB_PROF_WEB_AUTH;
	WebAuthProfile *prof = NULL;

	char_t *ipStr = NULL; 
	char_t *tmp = NULL;
	const char *ErrMsg = NULL;

	a_assert(wp);
	/*
	 * 删除所有选择的PROFILE
	 */
	ipStr = websGetVar(wp,T("delstr"),T("")); 
#if DEBUG
	printf("%s-%d, ipStr = %s\n", __func__, __LINE__, ipStr);
#endif
	if (ipStr != NULL) 
	{
	    IPADDR ipAddress;
	    tmp = strtok(ipStr, UTT_SPLIT_SIGN_STR);
#if (WEBAUTH_ACCOUNT == FYES)
	    uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	    while(tmp != NULL) 
	    {
#if DEBUG
		printf("%s-%d, tmp = %s\n", __func__, __LINE__, tmp);
#endif
#if 1
		ipAddress = inet_addr((const char *)tmp);
		/*
		 *  清除认证
		 */
		waCancelByIp(ipAddress);
#endif

		tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
	    }

#if (WEBAUTH_ACCOUNT == FYES)
	    uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	}
	websRedirect(wp, "WebAuthStatus.asp");
	return;	   
}
#else /*WEB_AUTH_SHARE*/

static int getWebAuthConnListData(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH;
    WebAuthProfile *prof = NULL;
    int min = 0, max = 0;
    int i = 0, num = 0;
    const char *MsgStr = NULL;

    websWrite(wp,"var sesIndexs =new Array();");
    websWrite(wp,"var WebAuthNames = new Array();");
    websWrite(wp,"var IPAddress = new Array();");
    websWrite(wp,"var maxSessions = new Array();");
    websWrite(wp,"var actualSessions = new Array();");
    websWrite(wp, T("var totalrecs;"));

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) 
    {
	prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
	{
	    if (prof->ip != 0)
	    {
		websWrite(wp,"sesIndexs[%d] = \"%d\";", num, num);
#if (WEBAUTH_AUTO == FYES)
		if (prof->autoAuthMethod == WECHAT_METHOD)
		    websWrite(wp,"WebAuthNames[%d] = \"微信用户\";",*num);
		else
#endif
		    websWrite(wp,"WebAuthNames[%d] = \"%s\";", num, prof->user);
		websWrite(wp,"IPAddress[%d] = \"%N\";", num, prof->ip);
		websWrite(wp,"maxSessions[%d] = %d;", num, 1);
		websWrite(wp,"actualSessions[%d] = %d;", num, 1);
		num++;
	    }
	}
    }

    websWrite(wp, "var totalrecs = %d;",num);
    websWrite(wp,"var maxWebAuthSes = %d;", MAX_WEB_AUTH_PROFILES);
    websWrite(wp, T("\n"));
    return 0;
}
static int getWebAuthmaxSessions(int eid, webs_t wp, int argc, char **argv)
{
	websWrite(wp,"var maxWebAuthSes = 1;\n");
	return 0;
}
#endif /*WEB_AUTH_SHARE*/
/********************************************************************
 * 函数： waUserAuth
 * 功能： 用户认证函数
 * 创建： 2012-04-17
 * 参数： 交互基本参数
 * 返回： 
 * 1		认证成功
 * 0		认证失败
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int waUserAuth(webs_t wp, char *path, char *query) 
{
	MibProfileType mibType = MIB_PROF_WEB_AUTH;
	WebAuthProfile *prof = NULL;
	int min = 0, max = 0;
	int i = 0;
	int flag = 1;
	char *userName = NULL, *userPasswd = NULL;
#if (IP_GRP == FYES)
	struct in_addr addr;
#endif
	IPADDR ip = 0U;
	const char *MsgStr = NULL;
#if (WEBAUTH_ACCOUNT == FYES)
	char *mac = NULL;
	/* 读取开机时间，代替系统时间 */
	struct sysinfo sysInfo;
	int tmCurrent = 0;
#endif
#if (WEB_AUTH_SHARE == FYES)
	int sesindex = -1;
	int sesflag = 0;
#endif

#if (WEBAUTH_AUTO == FYES)
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain;charset=GB2312\nPragma: no-cache\nCache-Control: no-cache\n\n"));
#endif
	/*
	 *  获得用户名和密码
	 */  
	userName = websGetVar(wp, T("userName"), T(""));
	userPasswd = websGetVar(wp, T("userPasswd"), T(""));
	SWORD_PRINTF("%s %d, userName = %s, userPasswd = %s\n",
			__FUNCTION__, __LINE__, userName, userPasswd);

	/* 账号，密码不合法*/
	if ((strlen(userName) > UTT_USER_LEN) || (strlen(userPasswd) > UTT_PASSWD_LEN)) 
	{
		flag = 0;
	}
	/* 获得账号profile*/
	ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++) 
	{
		prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
		if((prof != NULL) && (ProfInstIsFree(prof) == 0) 
				&& (prof->head.active == TRUE))
		{
			if((strcmp(userName, prof->user) == 0) && (strcmp(userPasswd, prof->passwd) == 0))
			{
#if (WEB_AUTH_SHARE == FYES)
			    /*add 'if(){ ... }'
			     * for bug 0013332
			     * */
			    if (ipsetTestIpStr(SYS_WEB_AUTH_OK, wp->ipaddr) == 1)
			    { 
				/*如果多次认证，则直接返回认证成功信息*/
				MsgStr = getTransDataForC(C_LANG_INDEX_SUCCESS_IDENTITY);
#if (WEBAUTH_AUTO == FYES)
				websWrite(wp,"var errorstr=\"%s\";", MsgStr);
				websDone(wp, 200);/*ajax尾*/
#else
				webAuthSetMsg(MsgStr);
				websRedirect(wp, "WebAuth_message.asp");
#endif
				return 0;    
			    }
			    if(((sesflag = checkSessionNum(prof))>0) && (prof->maxSession != 1)) {
#if DEBUG
				printf("%s--%d: session util the max number.\n", __func__, __LINE__);
				printf("%s--%d:  prof->maxSession =%d, prof->actualSession =%d.\n", __func__, __LINE__,
					prof->maxSession,prof->actualSession);
#endif
				if(sesflag == 1) {
				    MsgStr = getTransDataForC(C_LANG_INDEX_MAX_ACC_SESSION);
				}
				if(sesflag == 2) {
				    MsgStr = getTransDataForC(C_LANG_INDEX_MAX_SYS_SESSION);
				}
#if (WEBAUTH_AUTO == FYES)
				websWrite(wp,"var errorstr=\"%s\";", MsgStr);
				websDone(wp, 200);/*ajax尾*/
#else
				webAuthSetMsg(MsgStr);
				websRedirect(wp, "WebAuth_auth.asp");
#endif
				return 0;    
			    } else {
				/*如果账号最大会话数为1
				 * 即未开启账号共享
				 * 若已连接则替换当前会话*/
				if(prof->maxSession == 1U){
#if DEBUG
				    printf("%s-%d: prof->maxSession =%d, prof->actualSession =%d,prof->sesIndex =%d\n",
					    __func__,__LINE__,prof->maxSession,prof->actualSession,prof->sesIndex);
#endif
				    /*账号已被使用*/
				    if( prof->actualSession == 1U) {
#if DEBUG
					printf("%s-%d: profile account used.\n",__func__,__LINE__);
#endif
					/*将prof->ip从OK地址组中删除*/
					if(checkSessionIndex(prof->sesIndex) == 1) {
					webAuthLog(prof, nvramProfile->session[prof->sesIndex].ip, nvramProfile->session[prof->sesIndex].mac);
					ipsetAOrDIp(SYS_WEB_AUTH_OK, nvramProfile->session[prof->sesIndex].ip, IPSET_MEM_DEL);
#if (IP_GRP == FYES)
					addr.s_addr = nvramProfile->session[prof->sesIndex].ip;
					SWORD_PRINTF("%s-----%d, inet_ntoa(addr) is %s\n", __func__, __LINE__,  inet_ntoa(addr));
					userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif

#ifdef CONFIG_UTT_NETFILTER
					uttNfNlHashDelIp(uttNfNlSock, nvramProfile->session[prof->sesIndex].ip);/*删除hash表对应的ip*/
#endif
					sesindex = prof->sesIndex;
					}
					//free(prof->sesHead); /*释放会话指针*/
				    } else { /*账号未被使用，则新建会话*/
					if(sesflag == 2) {
					    MsgStr = getTransDataForC(C_LANG_INDEX_MAX_SYS_SESSION);
#if (WEBAUTH_AUTO == FYES)
					    websWrite(wp,"var errorstr=\"%s\";", MsgStr);
					    websDone(wp, 200);/*ajax尾*/
#else
					    webAuthSetMsg(MsgStr);
					    websRedirect(wp, "WebAuth_message.asp");
#endif
					    return 0;
					}
#if DEBUG
					printf("%s-%d: profile account not used.\n",__func__,__LINE__);
#endif
					sesindex = searchFreeSessionIndex();
					//prof->sesHead = newses;
				    }
				} else { /*账号最大会话数大于1，即开启账号共享*/
#if DEBUG
				    printf("%s-%d: prof->maxSession =%d, prof->actualSession =%d\n",
					    __func__,__LINE__,prof->maxSession,prof->actualSession);
#endif
				    sesindex = searchFreeSessionIndex();
				}
				if(checkSessionIndex(sesindex) != 1) 
				{
#if DEBUG
				    printf("%s--%d: search new session error.\n", __func__, __LINE__);
#endif
				    MsgStr = getTransDataForC(C_LANG_INDEX_APPLY_SES_ERR);
#if (WEBAUTH_AUTO == FYES)
				    websWrite(wp,"var errorstr=\"%s\";", MsgStr);
				    websDone(wp, 200);/*ajax尾*/
#else
				    webAuthSetMsg(MsgStr);
				    websRedirect(wp, "WebAuth_message.asp");
#endif
				    return 0;    
				}
			    }
#else /*WEB_AUTH_SHARE*/
				/*账号已被使用*/
				if( prof->ip != 0U) 
				{
					webAuthLog(prof, prof->ip, prof->mac);
					/*将prof->ip从OK地址组中删除*/
					ipsetAOrDIp(SYS_WEB_AUTH_OK, prof->ip, IPSET_MEM_DEL);
#if (IP_GRP == FYES)
					addr.s_addr = prof->ip;
					SWORD_PRINTF("%s-----%d, inet_ntoa(addr) is %s\n", __func__, __LINE__,  inet_ntoa(addr));
					userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif

#ifdef CONFIG_UTT_NETFILTER
					uttNfNlHashDelIp(uttNfNlSock, prof->ip);/*删除hash表对应的ip*/
#endif
				}
#endif /*WEB_AUTH_SHARE*/
				break;
			}
		}
	}
	SWORD_PRINTF("%s-%d, i = %d\n", __func__, __LINE__, i);
	if(i != max)/*找到对应用户名和密码*/
	{
#if (WEBAUTH_ACCOUNT == FYES)
	    tmCurrent = time(NULL);
	    if(((prof->timeStart > tmCurrent) || (tmCurrent > prof->timeStop)) && (prof->timeingPatternEn == FUN_ENABLE) && ((tmCurrent < DELIVERY_START_TIME) || (tmCurrent > DELIVERY_END_TIME)))
	    {
		webauth_acc_debug_print("%s--%d:This account has expired\n", __func__, __LINE__);
		MsgStr = getTransDataForC(C_LANG_INDEX_WEBAUTH_ACC_EXPIRED);
#if (WEBAUTH_AUTO == FYES)
				    websWrite(wp,"var errorstr=\"%s\";", MsgStr);
				    websDone(wp, 200);/*ajax尾*/
#else
				    webAuthSetMsg(MsgStr);
				    websRedirect(wp, "WebAuth_auth.asp");
#endif
		flag = 0;    
		return flag;
	    }
	    else if ((prof->timeTotal != 0U) && (prof->timeUsed >= prof->timeTotal) && (prof->timeingPatternEn == FUN_ENABLE))
	    {
		webauth_acc_debug_print("%s--%d:This account don't have time remaining\n", __func__, __LINE__);
		MsgStr = getTransDataForC(C_LANG_INDEX_WEBAUTH_NO_TIME);
#if (WEBAUTH_AUTO == FYES)
		websWrite(wp,"var errorstr=\"%s\";", MsgStr);
		websDone(wp, 200);/*ajax尾*/
#else
		webAuthSetMsg(MsgStr);
		websRedirect(wp, "WebAuth_auth.asp");
#endif
		flag = 0;
                return flag;
	    }
	    else
#endif
	    {
		MsgStr = getTransDataForC(C_LANG_INDEX_SUCCESS_IDENTITY);
		webAuthSetMsg(MsgStr);
		if (ipsetTestIpStr(SYS_WEB_AUTH_OK, wp->ipaddr) == 0)
		{ 
			SWORD_PRINTF("%s-----%d, wp->ipaddr is %s\n", __func__, __LINE__,  wp->ipaddr);
			/*增加到已认证*/
			ipsetAOrDIpStr(SYS_WEB_AUTH_OK, wp->ipaddr, IPSET_MEM_ADD); 
		}
#if (IP_GRP == FYES)
		SWORD_PRINTF("%s-----%d, wp->ipaddr is %s\n", __func__, __LINE__,  wp->ipaddr);
		userUpToIpGrp(userName, wp->ipaddr, IPENT_WEB_AUTH, IPSET_MEM_ADD);
#endif
		ip = inet_addr((const char *)(wp->ipaddr));/*字符型ip地址转换为整型*/
#if (WEB_AUTH_SHARE == FYES)
		if((prof->maxSession == 1U)&&(prof->actualSession == 1U)){
			memset(&(nvramProfile->session[sesindex]),0U,sizeof(WebAuthSession));
			nvramProfile->session[sesindex].nextIndex = -1;
			prof->sesIndex = -1;
			prof->actualSession -=1;
#if DEBUG
			printf("%s-%d: session was substituted.\n",__func__,__LINE__);
#endif
		}
		nvramProfile->session[sesindex].nextIndex = prof->sesIndex;
		prof->sesIndex = sesindex;
		prof->actualSession +=1;
		nvramProfile->session[sesindex].ip = ip;
#if DEBUG
		printf("%s-%d: prof->maxSession =%d, prof->actualSession =%d\n",__func__,__LINE__,prof->maxSession,prof->actualSession);
#endif
#else /*WEB_AUTH_SHARE*/
		prof->ip = ip;
#endif /*WEB_AUTH_SHARE*/
#ifdef CONFIG_UTT_NETFILTER
		/*上线时可能别人或dhcp分配对应的策略等还在
		 * 故要删除一次*/
		uttNfNlHashDelIp(uttNfNlSock, ip);/*删除hash表对应的ip*/
#endif
#if (WEBAUTH_ACCOUNT == FYES)
		((struct sockaddr_in*)&arpLan.arp_pa)->sin_addr.s_addr = prof->ip;
		memset(arpLan.arp_ha.sa_data, 0, sizeof(arpLan.arp_ha.sa_data));
		mac = getMacByIpIoctl(&arpLan);/*通过arp表查询当前ip的对应mac地址*/
		if (mac != NULL)/*查到了，保存到结构体中*/
		{
#if (WEB_AUTH_SHARE == FYES)
#if DEBUG
		    printf("%s-%d: mac addr find.\n",__func__,__LINE__);
#endif
		    memcpy(nvramProfile->session[sesindex].mac, mac, 6);
		   // nvramProfile->session[sesindex].compareMacEn = FUN_ENABLE;/*打开mac地址比较标志，在循环计费时需要判断mac地址*/
#else /*WEB_AUTH_SHARE*/
		    memcpy(prof->mac, mac, 6);
		    //prof->compareMacEn = FUN_ENABLE;/*打开mac地址比较标志，在循环计费时需要判断mac地址*/
#endif /*WEB_AUTH_SHARE*/
		}
#ifdef CONFIG_UTT_NETFILTER
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
		    uttNfNlHashEditTrigger(uttNfNlSock, ip);
#endif
#endif
#if (WEB_AUTH_SHARE == FYES)
		    nvramProfile->session[sesindex].compareMacEn = FUN_DISABLE;
#else /*WEB_AUTH_SHARE*/
		    prof->compareMacEn = FUN_DISABLE;
#endif /*WEB_AUTH_SHARE*/
#if (WEB_AUTH_SHARE == FYES)
#if DEBUG
		printf("%s-%d: prof->maxSession =%d, prof->actualSession =%d,prof->sesIndex =%d\n",__func__,__LINE__,
			prof->maxSession,prof->actualSession,prof->sesIndex);
		printf("%s-%d: sesindex =%d,nvramProfile->session[sesindex].ip =0x%x, mac = %s, compareMacEn =%d\n",__func__,__LINE__,
			sesindex,nvramProfile->session[sesindex].ip,nvramProfile->session[sesindex].mac,nvramProfile->session[sesindex].compareMacEn);
#endif
#endif /*WEB_AUTH_SHARE*/
		sysinfo(&sysInfo);
		prof->timeLast = (unsigned long)sysInfo.uptime;/*读取开机到当前的时间，当做认证成功时间*/
#endif
#if (WEBAUTH_ACCOUNT == FYES)
		if(prof->timeingPatternEn == FUN_ENABLE)
		{
			billing  = 1;
			stopData = prof->timeStop;
			if(prof->timeTotal == 0u){
			leftTime = 0u;
			}else{
			leftTime = prof->timeTotal - prof->timeUsed - ((unsigned long)sysInfo.uptime - prof->timeLast);
			}
		}else
#endif
		{
			billing  = 0;
			stopData = 0u;
			leftTime = 0u;
		}
		//websRedirect(wp, "WebAuth_message.asp");
	    }

	}
	else
	{
		MsgStr = getTransDataForC(C_LANG_INDEX_AUTHENTICATION_FAILURE);
#if (WEBAUTH_AUTO == FYES)
		websWrite(wp,"var errorstr=\"%s\";", MsgStr);
		websDone(wp, 200);/*ajax尾*/
#else
		webAuthSetMsg(MsgStr);
		websRedirect(wp, "WebAuth_auth.asp");
#endif
		return 0;
	}
#if (WEBAUTH_AUTO == FYES)
	websWrite(wp,"var errorstr=\"%s\";", MsgStr);
	websDone(wp, 200);/*ajax尾*/
#else
	webAuthSetMsg(MsgStr);
	websRedirect(wp, "WebAuth_message.asp");
#endif
	return flag;
}

#if (WEBAUTH_AUTO == FYES)

/********************************************************************
 *  * 函数： waCaptchaAuth
 *   * 功能： 验证码认证函数
 *    * 创建： 2014-01-02
 *     * 参数： 交互基本参数
 *      * 返回： 
 *       * 1	    认证成功
 *        * 0	    认证失败
 *         * 输出： 略
 *          * 作者： li.wenchang 
 *           ********************************************************************/ 

static int waCaptchaAuth(webs_t wp, char *path, char *query) 
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH_CPT;
    WebAuthProfile *prof = NULL;
    int min = 0, max = 0;
    int i = 0;
    int flag = 1;
    char *captcha = NULL;
#if (IP_GRP == FYES)
    struct in_addr addr;
#endif
    IPADDR ip = 0U;
    const char *MsgStr = NULL;
#if (WEBAUTH_ACCOUNT == FYES)
    char *mac = NULL;
    /* 读取开机时间，代替系统时间 */
    struct sysinfo sysInfo;
    int tmCurrent = 0;
#endif
#if (WEB_AUTH_SHARE == FYES)
    int sesindex = -1;
    int sesflag = 0;
#endif
    unsigned long tm = time(0);
    int ret = 0;

    /*
     *	 *  获得验证码
     *		 */  
    captcha = websGetVar(wp, T("phonecode"), T(""));
    if (captcha != NULL && captcha[0] != 0)
	ret = SMS_METHOD;
    else 
    {
	captcha = websGetVar(wp, T("weixincode"), T(""));
	if (captcha != NULL && captcha[0] != 0)
	    ret = WECHAT_METHOD;
	else
	    return C_LANG_INDEX_AUTHENTICATION_FAILURE;
    }

    SWORD_PRINTF("%s %d, ret = %d, captcha = %s\n",
	    __FUNCTION__, __LINE__, ret, captcha);

    /* 验证码不合法(长度不为6)*/
    if (strlen(captcha) != 4) 
    {
	SWORD_PRINTF("%s %d, strlen not 4\n", __func__, __LINE__);
	return C_LANG_INDEX_AUTHENTICATION_FAILURE;
    }

    SWORD_PRINTF("%s %d, check...\n", __func__, __LINE__);
    /* 获得账号profile*/
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) 
    {
	prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) 
		&& (prof->head.active == TRUE))
	{
	    if((ret == prof->autoAuthMethod) && (strcmp(captcha, prof->passwd) == 0))
	    {
#if (WEB_AUTH_SHARE == FYES)
		if (ipsetTestIpStr(SYS_WEB_AUTH_OK, wp->ipaddr) == 1)
		{ 
		    /*如果多次认证，则直接返回认证成功信息*/
		    return C_LANG_INDEX_SUCCESS_IDENTITY;    
		}
		if(((sesflag = checkSessionNum(prof))>0) && (prof->maxSession != 1)) {
		    if ((sesflag == 1) || (sesflag == 2))
			return C_LANG_INDEX_MAX_ACC_SESSION;
		} else {
		    /*如果账号最大会话数为1
		     *即未开启账号共享
		     *若已连接则替换当前会话*/
		    if(prof->maxSession == 1U){
			/*账号已被使用*/
			if( prof->actualSession == 1U) {
			    /*将prof->ip从OK地址组中删除*/
			    if(checkSessionIndex(prof->sesIndex) == 1) {
				webAuthLog(prof, nvramProfile->session[prof->sesIndex].ip, nvramProfile->session[prof->sesIndex].mac);
				ipsetAOrDIp(SYS_WEB_AUTH_OK, nvramProfile->session[prof->sesIndex].ip, IPSET_MEM_DEL);
#if (IP_GRP == FYES)
				addr.s_addr = nvramProfile->session[prof->sesIndex].ip;
				userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif

#ifdef CONFIG_UTT_NETFILTER
				uttNfNlHashDelIp(uttNfNlSock, nvramProfile->session[prof->sesIndex].ip);/*删除hash表对应的ip*/
#endif
				sesindex = prof->sesIndex;
			    }
			} else { /*账号未被使用，则新建会话*/
			    sesindex = searchFreeSessionIndex();
			}
		    } else { /*账号最大会话数大于1，即开启账号共享*/
			sesindex = searchFreeSessionIndex();
		    }
		    if(checkSessionIndex(sesindex) != 1) 
			return C_LANG_INDEX_APPLY_SES_ERR;    
		}
#else /*WEB_AUTH_SHARE*/
		/*账号已被使用*/
		if( prof->ip != 0U) 
		{
		    webAuthLog(prof, prof->ip, prof->mac);
		    /*将prof->ip从OK地址组中删除*/
		    ipsetAOrDIp(SYS_WEB_AUTH_OK, prof->ip, IPSET_MEM_DEL);
#if (IP_GRP == FYES)
		    addr.s_addr = prof->ip;
		    userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif

#ifdef CONFIG_UTT_NETFILTER
		    uttNfNlHashDelIp(uttNfNlSock, prof->ip);/*删除hash表对应的ip*/
#endif
		}
#endif /*WEB_AUTH_SHARE*/
		break;
	    }
	}
    }

    if(i != max)/*找到对应用户名和密码*/
    {
#if (WEBAUTH_ACCOUNT == FYES)
	tmCurrent = time(NULL);
	if(((prof->timeStart > tmCurrent) || (tmCurrent > prof->timeStop)) && (prof->timeingPatternEn == FUN_ENABLE) && ((tmCurrent < DELIVERY_START_TIME) || (tmCurrent > DELIVERY_END_TIME)))
	{
	    /* 该账号不在有效期时间范围内 */
	    return C_LANG_INDEX_WEBAUTH_ACC_EXPIRED;    
	}
	else if ((prof->timeTotal != 0U) && (prof->timeUsed >= prof->timeTotal) && (prof->timeingPatternEn == FUN_ENABLE))
	{
	    /* 该账号可用时间已用完 */
	    return C_LANG_INDEX_WEBAUTH_NO_TIME;
	}
	else
#endif
	{
	    if (ipsetTestIpStr(SYS_WEB_AUTH_OK, wp->ipaddr) == 0)
	    { 
		/*增加到已认证*/
		ipsetAOrDIpStr(SYS_WEB_AUTH_OK, wp->ipaddr, IPSET_MEM_ADD); 
	    }
#if (IP_GRP == FYES)
	    userUpToIpGrp(prof->user, wp->ipaddr, IPENT_WEB_AUTH, IPSET_MEM_ADD);
#endif
	    ip = inet_addr((const char *)(wp->ipaddr));/*字符型ip地址转换为整型*/
#if (WEB_AUTH_SHARE == FYES)
	    if((prof->maxSession == 1U)&&(prof->actualSession == 1U)){
		memset(&(nvramProfile->session[sesindex]),0U,sizeof(WebAuthSession));
		nvramProfile->session[sesindex].nextIndex = -1;
		prof->sesIndex = -1;
		prof->actualSession -=1;
	    }
	    nvramProfile->session[sesindex].nextIndex = prof->sesIndex;
	    prof->sesIndex = sesindex;
	    prof->actualSession +=1;
	    nvramProfile->session[sesindex].ip = ip;
#else /*WEB_AUTH_SHARE*/
	    prof->ip = ip;
#endif /*WEB_AUTH_SHARE*/
#ifdef CONFIG_UTT_NETFILTER
	    /*上线时可能别人或dhcp分配对应的策略等还在
	     *		 * 故要删除一次*/
	    uttNfNlHashDelIp(uttNfNlSock, ip);/*删除hash表对应的ip*/
#endif
#if (WEBAUTH_ACCOUNT == FYES)
	    ((struct sockaddr_in*)&arpLan.arp_pa)->sin_addr.s_addr = prof->ip;
	    memset(arpLan.arp_ha.sa_data, 0, sizeof(arpLan.arp_ha.sa_data));
	    mac = getMacByIpIoctl(&arpLan);/*通过arp表查询当前ip的对应mac地址*/
	    if (mac != NULL)/*查到了，保存到结构体中*/
	    {
#if (WEB_AUTH_SHARE == FYES)
		memcpy(nvramProfile->session[sesindex].mac, mac, 6);
#else /*WEB_AUTH_SHARE*/
		memcpy(prof->mac, mac, 6);
#endif /*WEB_AUTH_SHARE*/
	    }
#ifdef CONFIG_UTT_NETFILTER
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
	    uttNfNlHashEditTrigger(uttNfNlSock, ip);
#endif
#endif
#if (WEB_AUTH_SHARE == FYES)
	    nvramProfile->session[sesindex].compareMacEn = FUN_DISABLE;
#else /*WEB_AUTH_SHARE*/
	    prof->compareMacEn = FUN_DISABLE;
#endif /*WEB_AUTH_SHARE*/
	    sysinfo(&sysInfo);
	    prof->timeLast = (unsigned long)sysInfo.uptime;/*读取开机到当前的时间，当做认证成功时间*/
#endif
#if (WEBAUTH_ACCOUNT == FYES)
	    if(prof->timeingPatternEn == FUN_ENABLE)
	    {
		billing  = 1;
		stopData = prof->timeStop;
		if(prof->timeTotal == 0u){
		    leftTime = 0u;
		}else{
		    leftTime = prof->timeTotal - prof->timeUsed - ((unsigned long)sysInfo.uptime - prof->timeLast);
		}
	    }else
#endif
	    {
		billing  = 0;
		stopData = 0u;
		leftTime = 0u;
	    }

	    /* 认证成功 */
	    return C_LANG_INDEX_SUCCESS_IDENTITY;
	}
    }
    else
    {
	/* 未找到对应的用户名密码 */
	return C_LANG_INDEX_AUTHENTICATION_FAILURE;
    }

    /* 其实程序走不到这里 */
    return C_LANG_INDEX_SUCCESS_IDENTITY;
}
#endif
/********************************************************************
 * 函数： _waCancelByUser
 * 功能： 挂断，删除调用函数。根据实例名注销web认证
 * 创建： 2014-01-16
 * 参数： user  -  实例名
 * 返回： 略
 * 输出： 略
 * 作者： 李文昌 
 ********************************************************************/ 
void _waCancelByUser(MibProfileType mibType, char *user) 
{
	WebAuthProfile *prof = NULL;
	int min = 0, max = 0;
	int i = 0;
#if (IP_GRP == FYES)
	struct in_addr addr;
#endif
#if (WEB_AUTH_SHARE == FYES)
	int j=-1,k=0;
#endif

	ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++) 
	{
		prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
		if((prof != NULL) && (ProfInstIsFree(prof) == 0) 
				&& (prof->head.active == TRUE))
		{
			/*根据user获得对应profile*/
			if(strcmp(user,prof->head.name) == 0)
			{
#if (WEB_AUTH_SHARE == FYES)
			    j = prof->sesIndex;
			    for(k=0;k<prof->actualSession;k++)
			    {
				if(checkSessionIndex(prof->sesIndex) == 1) 
				{
				    if(nvramProfile->session[j].ip != 0)
				    {
					webAuthLog(prof, nvramProfile->session[j].ip, nvramProfile->session[j].mac);
					/*将此IP从OK地址组删除*/
#if DEBUG
					printf("%s-%d: session index =%d, session ip =0x%x\n",__func__,__LINE__,j,nvramProfile->session[j].ip);
#endif
					ipsetAOrDIp(SYS_WEB_AUTH_OK, nvramProfile->session[j].ip, IPSET_MEM_DEL);
#if (IP_GRP == FYES)
					addr.s_addr = nvramProfile->session[j].ip;
					SWORD_PRINTF("%s-%d: inet_ntoa(addr) =%s\n",__func__,__LINE__,inet_ntoa(addr));
					userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
					uttNfNlHashDelIp(uttNfNlSock, nvramProfile->session[j].ip);/*删除hash表对应的ip*/
#endif
					prof->sesIndex = nvramProfile->session[j].nextIndex;
					memset(&(nvramProfile->session[j]), 0U, sizeof(WebAuthSession));
					nvramProfile->session[j].nextIndex = -1;
					j = prof->sesIndex;
				    }
				}
			    }
			    prof->sesIndex = -1;
			    prof->actualSession = 0U;
#if DEBUG
			    printf("%s-%d: prof->maxSession =%d, prof->actualSession =%d,prof->sesIndex =%d\n",__func__,__LINE__,
				    prof->maxSession,prof->actualSession,prof->sesIndex);
#endif
#if 0
			    if(prof->sesHead != NULL) 
			    {
				printf("%s-%d: cacel user all session,prof->maxSession =%d,prof->actualSession =%d.\n",
					__func__,__LINE__, prof->maxSession,prof->actualSession);
				session = prof->sesHead;
				for(;session != NULL;)
				{
				    /*将此IP从OK地址组删除*/
				    printf("%s-%d: i =%d, session->ip =0x%x\n",__func__,__LINE__,i,session->ip);
				    ipsetAOrDIp(SYS_WEB_AUTH_OK, session->ip, IPSET_MEM_DEL);
#if (IP_GRP == FYES)
				    addr.s_addr = session->ip;
				    SWORD_PRINTF("%s-%d: inet_ntoa(addr) =%s\n",__func__,__LINE__,inet_ntoa(addr));
				    userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
				    uttNfNlHashDelIp(uttNfNlSock, session->ip);/*删除hash表对应的ip*/
#endif
				    prof->sesHead = session->next;
				    free(session);
				    session = prof->sesHead;
				}
			    }
			    prof->sesHead = NULL;
			    prof->actualSession = 0;
#endif
#else /*WEB_AUTH_SHARE*/
				/*将此IP从OK地址组删除*/
				SWORD_PRINTF("%s-%d, i = %d, prof->ip is %x\n", __func__, __LINE__, i,  prof->ip);
				webAuthLog(prof, prof->ip, prof->mac);
				ipsetAOrDIp(SYS_WEB_AUTH_OK, prof->ip, IPSET_MEM_DEL);
#if (IP_GRP == FYES)
				addr.s_addr = prof->ip;
				SWORD_PRINTF("%s-----%d, inet_ntoa(addr) is %s\n", __func__, __LINE__,  inet_ntoa(addr));
				userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
				uttNfNlHashDelIp(uttNfNlSock, prof->ip);/*删除hash表对应的ip*/
#endif
				prof->ip = 0U;
				SWORD_PRINTF("%s-%d, user is %s,ip = %d\n", __func__, __LINE__, prof->user, prof->ip);
#endif /*WEB_AUTH_SHARE*/
			}
		}
	}
	return;
}

/********************************************************************
 * 函数： waCancelByUser
 * 功能： 挂断，删除调用函数。根据实例名注销web认证
 * 创建： 2012-04-17
 * 参数： user  -  实例名
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void waCancelByUser(char *user) 
{
	MibProfileType mibType = MIB_PROF_WEB_AUTH;

	_waCancelByUser(mibType, user);

#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
	mibType = MIB_PROF_WEB_AUTH_CPT;
	_waCancelByUser(mibType, user);
#endif

	return;
}

#ifdef CONFIG_PPC_85xx
 /*
  * 将时间字符串(2.5)转换成int型秒数，做总时间
  *
  */
#define UTT_TIME_SPLIT_CHAR "."
static unsigned long timeStrToSecond(char_t *time)
{
    char *intergerPtr = NULL;
    char *decimalPtr = NULL;
    char *splitPtr = NULL;
    unsigned long interger = 0U;
    unsigned long decimal = 0U;
    unsigned long total = 0U;

    if(time != NULL) {
        intergerPtr = time;
        splitPtr = strstr(intergerPtr, UTT_TIME_SPLIT_CHAR);
        if(splitPtr == NULL) {
            interger = strtoul(intergerPtr, NULL, 10);
        } else {
            decimalPtr = splitPtr + 1;
            *splitPtr = '\0';
            interger = strtoul(intergerPtr, NULL, 10);
            decimal = strtoul(decimalPtr, NULL, 10);
        }
        total = (interger*3600U) + (decimal*360U);
    }
    return total;
}
#endif
/********************************************************************
 * 函数： WebAuthConfigAdd
 * 功能： 网页认证管理员增加账号
 * 创建： 2012-04-13
 * 参数： 交互固定输入参数
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void WebAuthConfigAdd(webs_t wp)
{
	MibProfileType mibType = MIB_PROF_WEB_AUTH;
	WebAuthProfile *prof = NULL;
	struProfAlloc *profhead = NULL;

	char_t *userName = NULL, *userPasswd = NULL, *remark = NULL;
	char_t *sessionStr = NULL;
#if (WEBAUTH_ACCOUNT == FYES) 
	char_t *timeTotal = NULL;
	char_t *timingModeEn = NULL;/*计时模式*/
	char_t *timestartStr = NULL, *timeendStr = NULL;
	Uint32 starttm = 0U;
	Uint32 endtm = 0U;
#endif
	Uint16 webIndexInst = 0;
	char_t instId[UTT_INST_NAME_LEN + 1];

	int i = 0;
	int min = 0, max = 0;
	int instIndex = 0;
	const char *ErrMsg = NULL;

	/*从页面空间获取值*/
	userName = websGetVar(wp, T("authname"), T(""));
	userPasswd = websGetVar(wp, T("authpasswd"), T(""));
	remark = websGetVar(wp, T("remark"), T(""));
	sessionStr = websGetVar(wp, T("maxnumber"), T(""));
#if (WEBAUTH_ACCOUNT == FYES)
	timeTotal = websGetVar(wp, T("authtimetotal"), T(""));
	timingModeEn = websGetVar(wp, T("authtimemode"), T(""));
	timestartStr = websGetVar(wp, T("BeginDate"), T(""));
	timeendStr = websGetVar(wp, T("EndDate"), T(""));
	starttm = strtoul(timestartStr, NULL, 10);
	endtm = strtoul(timeendStr, NULL, 10);
        uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	ProfInstLowHigh(mibType, &max, &min);
	for(i = min;i < max;i++)
	{
		prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
		if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
		{
			if(strncmp(prof->user,userName,UTT_USER_LEN) == 0)/*查看用户名是否已存在*/
			{
				ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_ALREADY_EXITS);
				setTheErrorMsgOnFree(ErrMsg);
				break;
			}
		}
	}
	SWORD_PRINTF("%s-%d, i = %d\n", __func__, __LINE__, i);
	if(i == max)/*没有同名的用户名*/
	{
		webIndexInst = getWebInstName(&(nvramProfile->webauthInstRec.instRecMap), &(nvramProfile->webauthInstRec.instNameIndex));
		SWORD_PRINTF("%s-%d, webIndexInst = %d\n", __func__, __LINE__, webIndexInst);
		/* portmap已满 */
		if (webIndexInst == 0) 
		{
			ErrMsg = getTransDataForC(C_LANG_INDEX_OPERATION_FAIL);
			setTheErrorMsg(ErrMsg);
			goto out;
		}
		memset(instId, 0, sizeof(instId));
		snprintf(instId, UTT_INST_NAME_LEN, "ID%d", webIndexInst);
		SWORD_PRINTF("%s-%d, instId = %s\n", __func__, __LINE__, instId);
		/* 已存在实例名 */
		if (ProfGetInstIndexByName(mibType, instId) != PROFFAIL) 
		{
			ErrMsg = getTransDataForC(C_LANG_INDEX_CASE_ALREADY_EXITS);
			setTheErrorMsg(ErrMsg);
			goto out;
		}
		instIndex = ProfNewInst(mibType, instId, FALSE);/*获取实例号并写上实例名*/
		SWORD_PRINTF("%s-%d, instIndex = %d\n", __func__, __LINE__, instIndex);
		if(instIndex == PROFFAIL)/*获取失败则表明已达最大条目*/ 
		{
			clearPortMap(&(nvramProfile->webauthInstRec.instRecMap), webIndexInst);
			ErrMsg = getTransDataForC(C_LANG_INDEX_ALREADY_THE_MAX_TITLE);
			setTheErrorMsgOnFree(ErrMsg);
		}
		else
		{
			prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, instIndex);
			SWORD_PRINTF("%s-%d, name = %s, ProfInstIsFree(prof) = %d\n", __func__, __LINE__, prof->head.name, ProfInstIsFree(prof));
			if((prof != NULL) && (ProfInstIsFree(prof) == 0))
			{
				ProfBackupByIndex(mibType, PROF_CHANGE_ADD, instIndex, &profhead);
				strcpy(prof->user,userName);
				strcpy(prof->passwd,userPasswd);
				strcpy(prof->remark,remark);
#if (WEB_AUTH_SHARE == FYES)
				if(strtoul(sessionStr, NULL, 10) > MAX_WEB_AUTH_SESSION)
				{
				    ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_ACC_SESSION);
				    setTheErrorMsg(ErrMsg);
				    goto out;
				}
				prof->maxSession = strtoul(sessionStr, NULL, 10);
#if DEBUG
				printf("%s-%d: maxSession =%d\n",__func__,__LINE__,prof->maxSession);
#endif
#endif
#if (WEBAUTH_ACCOUNT == FYES)
				if (strcmp(timingModeEn, "on") == 0)
				{
				    prof->timeingPatternEn = FUN_ENABLE;

				    prof->timeStart = starttm;
				    prof->timeStop = endtm;
				    /*将时间字符串(2.5)转换成int型秒数，做总时间*/
#ifdef CONFIG_PPC_85xx
                                    prof->timeTotal = timeStrToSecond(timeTotal);
#else
				    prof->timeTotal = ((unsigned long)(strtod(timeTotal, NULL) * 10) * 360U);
#endif
				    prof->timeUsed = 0U;
				    SWORD_PRINTF("%s-%d timeStart : %d ;timeStop : %d\n", __func__, __LINE__,starttm,endtm);
				}
				else
				{
				    prof->timeingPatternEn = FUN_DISABLE;
				    prof->timeStart = 0U;
				    prof->timeStop = 0U;
				    prof->timeTotal = 0U;
				    prof->timeUsed = 0U;
				}
#endif
				prof->head.active = TRUE;
				SWORD_PRINTF("%s-%d,user is %s,passwd is %s\n", __func__, __LINE__, prof->user, prof->passwd);
			}
		}
	}

	ProfUpdate(profhead);/*更新*/
	ProfFreeAllocList(profhead);
	nvramWriteCommit(); /*save*/
out:
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	websRedirect(wp, "WebAuthServer.asp");
	return;
}
/********************************************************************
 * 函数： WebAuthConfigModify
 * 功能： 网页认证管理员修改账号
 * 创建： 2012-04-13
 * 参数： 交互固定输入参数
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void WebAuthConfigModify(webs_t wp)
{
	MibProfileType mibType = MIB_PROF_WEB_AUTH;
	WebAuthProfile *prof = NULL;
	struProfAlloc *profhead = NULL;
#if (IP_GRP == FYES)
	IpGrpProfile* referedProf = NULL;
	char referedMsg[50];
#endif

	char_t *userName = NULL, *userOld = NULL, *userPasswd = NULL, *remark = NULL, *instId = NULL;
	char_t *sessionStr = NULL;
#if (WEBAUTH_ACCOUNT == FYES)
	char_t *timeTotal = NULL;
	char_t *timingModeEn = NULL;/*计时模式*/
	char_t *timestartStr = NULL, *timeendStr = NULL;
	Uint32 starttm = 0U;
	Uint32 endtm = 0U;
	int profChange = 0;/*是否只有总时间改变，只有总时间改变不挂断*/
#endif

	int i = 0;
	int min = 0, max = 0;
	int instIndex = 0;
	const char *ErrMsg = NULL, *ErrMsg2 = NULL;

#if (IP_GRP == FYES)
	memset(referedMsg, 0, sizeof(referedMsg));
#endif
	userName = websGetVar(wp, T("authname"), T(""));
	userOld = websGetVar(wp, T("authnameold"), T("")); 
	userPasswd = websGetVar(wp, T("authpasswd"), T(""));
	remark = websGetVar(wp, T("remark"), T(""));
	sessionStr = websGetVar(wp, T("maxnumber"), T(""));
#if (WEBAUTH_ACCOUNT == FYES)
	timeTotal = websGetVar(wp, T("authtimetotal"), T(""));
	timingModeEn = websGetVar(wp, T("authtimemode"), T(""));
	timestartStr = websGetVar(wp, T("BeginDate"), T(""));
	timeendStr = websGetVar(wp, T("EndDate"), T(""));
	starttm = strtoul(timestartStr, NULL, 10);
	endtm = strtoul(timeendStr, NULL, 10);
        uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	instId = websGetVar(wp, T("instIndex"), T("")); 


	instIndex = ProfGetInstIndexByName(mibType, instId);
	if (instIndex == PROFFAIL) 
	{
		ErrMsg = getTransDataForC(C_LANG_INDEX_CHANGED_CASE_NOT_EXITS);
		setTheErrorMsg(ErrMsg);
		goto out;
	}
	if (strcmp(userName, userOld) != 0) 
	{ /* 用户名修改了 */
#if (IP_GRP == FYES)
		referedProf = userRefByIpGrp(userOld, IPENT_WEB_AUTH);
		if(referedProf != NULL)
		{
			ErrMsg = getTransDataForC(C_LANG_INDEX_BE_USER_GROUP);
			ErrMsg2 = getTransDataForC(C_LANG_INDEX_USE_CANT_BE_CHANGE);
			sprintf(referedMsg, "%s%s%s%s", userOld, ErrMsg, referedProf->head.name, ErrMsg2);
			setTheErrorMsgOnFree(referedMsg);
			goto out;
		}
#endif
		if (userName != NULL && (ProfGetInstPointByValue(mibType, OFFANDSIZE(WebAuthProfile, user), TSTR, userName, instIndex) != PROFFAIL)) 
		{
			ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_ALREADY_EXITS);
			setTheErrorMsg(ErrMsg);
			goto out;
		}
	}
	ProfInstLowHigh(mibType, &max, &min);
	for(i = min;i < max;i++)
	{
		prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
		if((prof != NULL) && (ProfInstIsFree(prof) == 0))
		{
			if(strcmp(prof->user,userOld) == 0)
			{
				ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, i, &profhead);
#if (WEBAUTH_ACCOUNT == FYES)
				if ((strcmp(prof->user, userName) != 0) || (strcmp(prof->passwd, userPasswd) != 0))
				{
				    profChange = 1;
				}
#endif
				strcpy(prof->user,userName);
				strcpy(prof->passwd,userPasswd);
				strcpy(prof->remark,remark);
#if (WEB_AUTH_SHARE == FYES)
				if(strtoul(sessionStr, NULL, 10) > MAX_WEB_AUTH_SESSION)
				{
				    ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_ACC_SESSION);
				    setTheErrorMsg(ErrMsg);
				    goto out;
				}
				prof->maxSession = strtoul(sessionStr, NULL, 10);
#if DEBUG
				printf("%s-%d: maxSession =%d\n",__func__,__LINE__,prof->maxSession);
#endif
#endif
#if (WEBAUTH_ACCOUNT == FYES)
				if (strcmp(timingModeEn, "on") == 0)
				{
				    if (prof->timeingPatternEn == FUN_DISABLE)
				    {
					profChange = 1;
				    }
				    prof->timeingPatternEn = FUN_ENABLE;

				    prof->timeStart = starttm;
				    prof->timeStop = endtm;
				    SWORD_PRINTF("********************************************************************************************************\n%s-%d starttm : %d ;endtm : %d\n", __func__, __LINE__,starttm,endtm);
				    /*将时间字符串(2.5)转换成int型秒数，做总时间*/
#ifdef CONFIG_PPC_85xx
				    prof->timeTotal = timeStrToSecond(timeTotal);
#else
				    prof->timeTotal = ((unsigned long)(strtod(timeTotal, NULL) * 10) * 360U);
#endif
				    if (prof->timeTotal == 0U)/*总时间修改为0后，清空已使用时间*/
				    {
					prof->timeUsed = 0U;/*已使用时间*/
				    }
				}
				else
				{
				    if (prof->timeingPatternEn == FUN_ENABLE)
				    {
					profChange = 1;
				    }
				    prof->timeingPatternEn = FUN_DISABLE;

				    prof->timeStart = 0U;
				    prof->timeStop = 0U;
				    prof->timeTotal = 0U;
				    prof->timeUsed = 0U;/*已使用时间*/

				}
#endif
				SWORD_PRINTF("%s-%d,user is %s,passwd is %s, remark is %s\n", __func__, __LINE__, prof->user, prof->passwd,remark);
#if (WEBAUTH_ACCOUNT == FYES)
				if (profChange == 1)/*除了总时间有其他修改，就挂断当前连接*/
#endif
				{
				    waCancelByUser(prof->head.name);/*根据实例名注销*/
				}
			}
		}
	}

	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit(); /*save*/
out:
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	websRedirect(wp, "WebAuthServer.asp");
	return;
}
/********************************************************************
 * 函数： formPictureUrl
 * 功能：保存用户提交的网络图片链接
 * 创建时间：20130607
 * 参数：交互固定输入参数
 * 返回：略
 * 输出：略
 * *****************************************************************/
static void formPictureUrl(webs_t wp, char_t *path, char_t *query)
{
	MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
	WebAuthGlobalProfile *prof = NULL;
	struProfAlloc *profhead = NULL;
	int i = 0;
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	char *pictureUrls = NULL;
	pictureUrls = websGetVar(wp,T("importpictureurl"),T(""));
	prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0))
	{
		ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, i, &profhead);
		if(prof->enpicture < 2)
		{
			prof->enpicture = 0;/*选中简易配置选项中的网络链接*/
		}else{
			prof->enpicture = 2;/*选中高级配置选项中的网络链接*/
		}
		strcpy(prof->pictureUrl,pictureUrls);
		saveContactToFile(pictureUrls, WEB_AUTH_PICURL_PATH);
	}
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit(); /*save*/
	websRedirect(wp, "WebAuthGlobal.asp");
	return;
}

#if (WEBAUTH_AUTO == FYES)
/*保存认证成功后需要跳转的链接*/
static void formJumpUrl(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    int i = 0;
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
#if (WEBAUTH_AUTO == FYES)
    char *jumpUrls = NULL;
    jumpUrls = websGetVar(wp,T("jumpurl"),T(""));
    prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, i);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0))
    {
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, i, &profhead);
	strcpy(prof->jumpUrl,jumpUrls);
	saveContactToFile(jumpUrls, WEB_AUTH_JUMPURL_PATH);
    }
#endif
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit(); /*save*/
    websRedirect(wp, "WebAuthGlobal.asp");
    return;
}
#endif

/********************************************************************
 * 函数： formWebAuthConfig
 * 功能： 网页认证管理员配置用户名和密码交互
 * 创建： 2012-04-13
 * 参数： 交互固定输入参数
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void formWebAuthConfig(webs_t wp, char_t *path, char_t *query)
{

	char_t *action = NULL;


	action = websGetVar(wp,T("Action"), T(""));

	SWORD_PRINTF("%s-%d, action = %s\n", __func__, __LINE__, action);

	if (strcmp(action, "add") == 0) 
	{
		WebAuthConfigAdd(wp);
	}
	else
	{
		WebAuthConfigModify(wp);
	}
	return;
}

#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
/********************************************************************
 *  * 函数： webAuthCaptchaListDel 
 *   * 功能： 网页认证管理员删除配置
 *    * 创建： 2014-01-14
 *     * 参数： 用户列表
 *      * 返回： 略
 *       * 输出： 略
 *        * 作者： 李文昌
 *         ********************************************************************/ 
static void webAuthCaptchaListDel(const char *UserName)
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH_CPT;
    char_t *tmp = strtok(UserName, UTT_SPLIT_SIGN_STR);
    const char *ErrMsg = NULL, *ErrMsg2 = NULL;
#if (IP_GRP == FYES)
    IpGrpProfile* referedProf = NULL;
    WebAuthProfile *prof = NULL;
    char referedMsg[100];
#endif
    Uint16 webIndexInst = 0;

    while(tmp != NULL) 
    {
#if (IP_GRP == FYES)
	SWORD_PRINTF("%s-%d, tmp = %s\n", __func__, __LINE__, tmp);
	prof = ProfGetInstPointByName(mibType, tmp);
	if (prof == NULL) 
	{
	    tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
	    continue;
	}

	referedProf = userRefByIpGrp(prof->user, IPENT_WEB_AUTH);
	if(referedProf != NULL)
	{
	    ErrMsg = getTransDataForC(C_LANG_INDEX_BE_USER_GROUP);
	    ErrMsg2 = getTransDataForC(C_LANG_INDEX_USE_CANT_BE_DELETE);
	    sprintf(referedMsg, "%s%s%s%s", prof->user, ErrMsg, referedProf->head.name, ErrMsg2);
	    setTheErrorMsgOnFree(referedMsg);
	}
	else
	{
	    waCancelByUser(tmp);/*根据实例名注销*/
	    ProfDelInstByName(mibType, tmp); 
	}
#else
	waCancelByUser(tmp);/*根据实例名注销*/
	ProfDelInstByName(mibType, tmp); 
#endif

	tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
    }

    return;
}

/********************************************************************
 *  * 函数： formWebAuthCaptureListDelAll 
 *   * 功能： 网页认证管理员删除所有配置
 *    * 创建： 2014-01-14
 *     * 参数： 交互固定输入参数
 *      * 返回： 略
 *       * 输出： 略
 *        * 作者： 李文昌
 *         ********************************************************************/ 
static void webAuthCaptchaListDelAll(void)
{
    int min = 0, max = 0;
    int i = 0;
#if (IP_GRP == FYES)
    IpGrpProfile* referedProf = NULL;
    char referedMsg[50];
#endif

    MibProfileType mibType = MIB_PROF_WEB_AUTH_CPT;
    WebAuthProfile *prof = NULL;
    struProfAlloc *profhead  = NULL;
    const char *ErrMsg = NULL, *ErrMsg2 = NULL;

#if (IP_GRP == FYES)
    memset(referedMsg, 0, sizeof(referedMsg));
#endif
    /*
     *	 * 删除所有选择的PROFILE
     *	     */
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) 
    {
	prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL)  && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
	{
#if (IP_GRP == FYES)
	    referedProf = userRefByIpGrp(prof->user, IPENT_WEB_AUTH);
	    if(referedProf != NULL)
	    {
		ErrMsg = getTransDataForC(C_LANG_INDEX_BE_USER_GROUP);
		ErrMsg2 = getTransDataForC(C_LANG_INDEX_USE_CANT_BE_DELETE);
		sprintf(referedMsg, "%s%s%s%s", prof->user, ErrMsg, referedProf->head.name, ErrMsg2);
		setTheErrorMsgOnFree(referedMsg);
	    }
	    else
	    {
		waCancelByUser(prof->head.name);/*根据实例名注销*/
		ProfDelInstByIndex(mibType, i);
	    }
#else
	    waCancelByUser(prof->head.name);/*根据实例名注销*/
	    ProfDelInstByIndex(mibType, i);
#endif
	}
    }

    /* 更新map记录 */
    clearAllPortMap(&(nvramProfile->webauthInstRec.instRecMap));
    nvramProfile->webauthInstRec.instNameIndex = 0;
}
#endif

/********************************************************************
 * 函数： formWebAuthListDel 
 * 功能： 网页认证管理员删除配置
 * 创建： 2012-04-13
 * 参数： 交互固定输入参数
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void formWebAuthListDel(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH;
    struProfAlloc *profhead = NULL;

    char_t *UserName = NULL; 
    char_t  UserName_bak[1000] = {0};
    char_t *tmp = NULL;
    const char *ErrMsg = NULL, *ErrMsg2 = NULL;

#if (IP_GRP == FYES)
    IpGrpProfile* referedProf = NULL;
    WebAuthProfile *prof = NULL;
    char referedMsg[100];
#endif
    int back = 0;
    Uint16 webIndexInst = 0;

    a_assert(wp);
#if (IP_GRP == FYES)
    memset(referedMsg, 0, sizeof(referedMsg));
#endif
    /*
     * 删除所有选择的PROFILE
     */
    UserName = websGetVar(wp,T("delstr"),T("")); 
    SWORD_PRINTF("%s-%d, instIndexs = %s\n", __func__, __LINE__, UserName);
    if (UserName == NULL) 
    {
	ErrMsg = getTransDataForC(C_LANG_INDEX_BE_USER_GROUP);
	setTheErrorMsg(ErrMsg);
    }
	else
	{
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
        memcpy(UserName_bak,UserName,strlen(UserName));
	    webAuthCaptchaListDel(UserName_bak);
#endif
		tmp = strtok(UserName, UTT_SPLIT_SIGN_STR);
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
		while(tmp != NULL) 
		{
			if(back == 0)/*由于可能删除多个，只做一次备份操作*/
			{
				ProfBackupByName(mibType, PROF_CHANGE_SPECIAL, tmp, &profhead);
				back = 1;
			}
			else
			{
				ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profhead);
			}
#if (IP_GRP == FYES)
			SWORD_PRINTF("%s-%d, tmp = %s\n", __func__, __LINE__, tmp);
			prof = ProfGetInstPointByName(MIB_PROF_WEB_AUTH, tmp);
			referedProf = userRefByIpGrp(prof->user, IPENT_WEB_AUTH);
			if(referedProf != NULL)
			{
				ErrMsg = getTransDataForC(C_LANG_INDEX_BE_USER_GROUP);
				ErrMsg2 = getTransDataForC(C_LANG_INDEX_USE_CANT_BE_DELETE);
				sprintf(referedMsg, "%s%s%s%s", prof->user, ErrMsg, referedProf->head.name, ErrMsg2);
				setTheErrorMsgOnFree(referedMsg);
			}
			else
			{
				waCancelByUser(tmp);/*根据实例名注销*/
				ProfDelInstByName(mibType, tmp); 
			}
#else
			waCancelByUser(tmp);/*根据实例名注销*/
			ProfDelInstByName(mibType, tmp); 
#endif
			/* 清除记录map */
			if (strncmp(tmp, "ID", 2) == 0) 
			{ 
				webIndexInst = (Uint16)strtol(&(tmp[2]), NULL, 10);
				clearPortMap(&(nvramProfile->webauthInstRec.instRecMap), webIndexInst);
			}
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
			else if (strncmp(tmp, "CPT", 3) == 0) 
			{ 
				webIndexInst = (Uint16)strtol(&(tmp[3]), NULL, 10);
				clearPortMap(&(nvramProfile->webauthInstRec.instRecMap), webIndexInst);
			}
#endif

			tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
		}

#if (WEBAUTH_ACCOUNT == FYES)
	uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
		nvramWriteCommit(); /*save*/
	}
	websRedirect(wp, "WebAuthServer.asp");

	return;	   
}
/********************************************************************
 * 函数： formWebAuthListDelAll 
 * 功能： 网页认证管理员删除所有配置
 * 创建： 2012-04-13
 * 参数： 交互固定输入参数
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void formWebAuthListDelAll(webs_t wp, char_t *path, char_t *query)
{
	int min = 0, max = 0;
	int i = 0;
	int back = 0;
#if (IP_GRP == FYES)
	IpGrpProfile* referedProf = NULL;
	char referedMsg[50];
#endif

	MibProfileType mibType = MIB_PROF_WEB_AUTH;
	WebAuthProfile *prof = NULL;
	struProfAlloc *profhead  = NULL;
	const char *ErrMsg = NULL, *ErrMsg2 = NULL;

#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
	webAuthCaptchaListDelAll();
#endif

	a_assert(wp);
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
#if (IP_GRP == FYES)
	memset(referedMsg, 0, sizeof(referedMsg));
#endif
	/*
	 * 删除所有选择的PROFILE
	 */
	ProfInstLowHigh(mibType, &max, &min);
	for(i = min; i < max; i++) 
	{
		prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
		if((prof != NULL)  && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
		{
			if(back == 0)
			{
				ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, i, &profhead);
				back = 1;
			}
#if (IP_GRP == FYES)
			referedProf = userRefByIpGrp(prof->user, IPENT_WEB_AUTH);
			if(referedProf != NULL)
			{
				ErrMsg = getTransDataForC(C_LANG_INDEX_BE_USER_GROUP);
				ErrMsg2 = getTransDataForC(C_LANG_INDEX_USE_CANT_BE_DELETE);
				sprintf(referedMsg, "%s%s%s%s", prof->user, ErrMsg, referedProf->head.name, ErrMsg2);
				setTheErrorMsgOnFree(referedMsg);
			}
			else
			{
				waCancelByUser(prof->head.name);/*根据实例名注销*/
				ProfDelInstByIndex(mibType, i);
			}
#else
			waCancelByUser(prof->head.name);/*根据实例名注销*/
			ProfDelInstByIndex(mibType, i);
#endif
		}
	}

	/* 更新map记录 */
	clearAllPortMap(&(nvramProfile->webauthInstRec.instRecMap));
	nvramProfile->webauthInstRec.instNameIndex = 0;

#if (WEBAUTH_ACCOUNT == FYES)
        uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit(); /*save*/
	websRedirect(wp, "WebAuthServer.asp");
}
/********************************************************************
 * 函数： formWebAuthHangUp
 * 功能： 页面挂断通过认证的用户，管理员使用
 * 创建： 2012-04-13
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void formWebAuthHangUp(webs_t wp, char_t *path, char_t *query)
{
	char_t *UserName = NULL; 
	const char *ErrMsg = NULL;

	a_assert(wp);

	/*
	 * 删除所有选择的PROFILE
	 */
	UserName = websGetVar(wp,T("hangup"),T("")); 
	SWORD_PRINTF("%s-%d, instIndexs = %s\n", __func__, __LINE__, UserName);
	if (UserName == NULL) 
	{
		ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_USER_NAME);
		setTheErrorMsg(ErrMsg);
	}
	else
	{ 
		waCancelByUser(UserName);/*根据实例名注销*/
	}

	websRedirect(wp, "WebAuthServer.asp");
	return;
}
/********************************************************************
 * 函数： formSubmitCheck
 * 功能： 检查用户提交命令是注销还是认证
 * 创建： 2012-04-13
 * 参数： 
 * - command            命令
 * 返回： 
 * - TRUE               注销
 * - FALSE              认证
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static Boolean formSubmitCheck(char *command) 
{
	Boolean flag = FALSE;

	if ((command != NULL) && (strcmp(command, "logoff") == 0)) 
	{
		flag = TRUE;
	}
	return flag;
}
/********************************************************************
 * 函数： formWebAuthUserSubmit
 * 功能： 处理用户提交
 * 创建： 2012-04-13
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void formWebAuthUserSubmit(webs_t wp, char *path, char *query) 
{
	char *command = NULL;

	/*
	 * 通过command获得用户是注销还是认证
	 */
	command = websGetVar(wp, T("userCommand"), T(""));
	SWORD_PRINTF("%s %d, command = %s\n", __FUNCTION__, __LINE__, command);

#if (WEBAUTH_ACCOUNT == FYES)
        uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	/*
	 * 注销
	 */
	if (formSubmitCheck(command) == TRUE)
	{
		IPADDR ipAddress;
		ipAddress = inet_addr((const char *)(wp->ipaddr));

		SWORD_PRINTF("%s %d, wp->ipaddr = %s, ipAddress = %x\n", __FUNCTION__, __LINE__, wp->ipaddr, ipAddress);

		/*
		 *  清除认证
		 */
		waCancelByIp(ipAddress);
		websRedirect(wp, "WebAuth_auth.asp");
	} 
	/*
	 * 认证
	 */
	else 
	{
		waUserAuth(wp, path, query);
	}
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	return;
}

#if (WEBAUTH_AUTO == FYES)
/********************************************************************
 *  * 函数： formWebAuthSmsSubmit
 *   * 功能： 处理用户提交
 *    * 创建： 2014-01-02
 *     * 参数： 略
 *      * 返回： 略
 *       * 输出： 略
 *        * 作者： li.wenchang 
 *         ********************************************************************/ 
static void formWebAuthCaptchaSubmit(webs_t wp, char *path, char *query) 
{
    char *command = NULL;
    const char *MsgStr = NULL;
    int ret;

    /*
     *	 * 通过command获得用户是注销还是认证
     *	     */
    command = websGetVar(wp, T("userCommand"), T(""));
    SWORD_PRINTF("%s %d, command = %s\n", __FUNCTION__, __LINE__, command);

#if (WEBAUTH_ACCOUNT == FYES)
    uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
    /*
     *	 * 注销
     *	     */
    if (formSubmitCheck(command) == TRUE)
    {
	IPADDR ipAddress;
	ipAddress = inet_addr((const char *)(wp->ipaddr));

	SWORD_PRINTF("%s %d, wp->ipaddr = %s, ipAddress = %x\n", __FUNCTION__, __LINE__, wp->ipaddr, ipAddress);

	/*
	 *	 *  清除认证
	 *		 */
	waCancelByIp(ipAddress);
	websRedirect(wp, "WebAuth_auth.asp");
    } 
    /*
     *   * 认证
     *	 */
    else 
    {
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain;charset=GB2312\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	ret = waCaptchaAuth(wp, path, query);
	MsgStr = getTransDataForC(ret);
	websWrite(wp,"var errorstr=\"%s\";", MsgStr);
	websDone(wp, 200);/*ajax尾*/
    }
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
    return;
}

/* 免输入验证码认证 */
static void formWebAuthFastSubmit(webs_t wp, char *path, char *query) 
{
    int ret;
    const char *ErrMsg = NULL;

    ret = waCaptchaAuth(wp, path, query); 
    if (ret == C_LANG_INDEX_SUCCESS_IDENTITY)
    {
	websRedirect(wp, "WebAuth_message.asp");
    } 
    else 
    {
	printf("ret:%d ok:%d fail:%d\n", ret, 
		C_LANG_INDEX_SUCCESS_IDENTITY, 
		C_LANG_INDEX_AUTHENTICATION_FAILURE);
	//ErrMsg = getTransDataForC(ret);
	//setTheErrorMsg(ErrMsg);
	websRedirect(wp, "WebAuth_auth.asp");
    }

    return;
}
#endif

/********************************************************************
 * 函数： formWebAuthGlobalConfig
 * 功能： 提交网页认证全局参数
 * 创建： 2012-04-13
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void formWebAuthGlobalConfig(webs_t wp, char_t *path, char_t *query)
{
	MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
	WebAuthGlobalProfile *prof = NULL;
	struProfAlloc *profhead = NULL;
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	char_t *enabled = NULL;
#if (WIFIDOG_AUTH == FYES)
	char_t *authtype = NULL;
#if (SMS_ENABLE ==1)
	char_t *smsEnable = NULL;
	char_t *cdmaType = NULL;
	char_t *province = NULL;
	char_t *city = NULL;
	char_t *sms_number = NULL;
#endif
#endif
	char_t *enpicture = NULL;
#if (WEBAUTH_SELF_HELP == FYES)
	char_t *selfenabled = NULL;
#endif
#if(WEB_AUTH_STALE == FYES)
	char_t *stale = NULL;
#endif
	char_t *activePic = NULL;
	char *pexceptIpGroup = NULL;	
	char *ptipstitle = NULL;
	char *ptipsinfo = NULL;
	char_t *contact = NULL;

#if 0
	char buf[1024*1024];
	memset(buf,0,1024*1024);
	flash_write_mtdname("/dev/sda3", buf, 0x0, 1024*1024);
	flash_write_mtdname("/dev/sda3", buf, 0x100000, 1024*1024);
	flash_write_mtdname("/dev/sda3", buf, 0x200000, 1024*1024);
	flash_write_mtdname("/dev/sda3", buf, 0x300000, 1024*1024);
	flash_write_mtdname("/dev/sda3", buf, 0x400000, 1024*1024);
#endif


#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
    int  totalTimes;
    char *idcardEnabled = NULL;
    char *smsEnabled = NULL;
    char *wechatEnabled = NULL;
    char *idcardTimeTotal = NULL;
    char *idcardDay = NULL;
    char *smsTimeTotal = NULL;
    char *smsDay = NULL;
    char *wechatTimeTotal = NULL;
    char *wechatDay = NULL;
    char *idcardMaxSessions = NULL;
    char *smsMaxSessions = NULL;
    char *wechatMaxSessions = NULL;
    char *smsTimes = NULL;
    char *wechatTimes = NULL;
    char *smsContents = NULL;
    char *wechatContents = NULL;
    char *wechatName = NULL;
    char *wechatToken = NULL;
    char *wechatKeyContents = NULL;
    char *smsServer = NULL;
#endif

	int i = 0;
	/*
	 * 取值
	 */
	enabled = websGetVar(wp, T("enabled"), T(""));
#if (WIFIDOG_AUTH == FYES)
	authtype = websGetVar(wp, T("authType"), T(""));
#if (SMS_ENABLE ==1)
	smsEnable = websGetVar(wp, T("smsEnable"), T(""));
	cdmaType = websGetVar(wp, T("cdmaType"), T(""));
	province = websGetVar(wp, T("province"), T(""));
	city = websGetVar(wp, T("city"), T(""));
	sms_number = websGetVar(wp, T("sms_number"), T(""));
#endif
#endif
	enpicture = websGetVar(wp, T("hidenpicture"), T(""));
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
	idcardEnabled = websGetVar(wp, T("idcardEnabled"), T(""));
	smsEnabled = websGetVar(wp, T("smsEnabled"), T(""));
	wechatEnabled = websGetVar(wp, T("wechatEnabled"), T(""));

	idcardTimeTotal = websGetVar(wp, T("idcardTimeTotal"), T(""));	
	smsTimeTotal = websGetVar(wp, T("smsTimeTotal"), T(""));
	wechatTimeTotal = websGetVar(wp, T("wechatTimeTotal"), T(""));

	idcardDay = websGetVar(wp, T("idcardday"), T(""));	
	smsDay = websGetVar(wp, T("smsday"), T(""));
	wechatDay = websGetVar(wp, T("day"), T(""));

	idcardMaxSessions = websGetVar(wp, T("idcardMaxSessions"), T(""));
	smsMaxSessions = websGetVar(wp, T("smsMaxSessions"), T(""));
	wechatMaxSessions = websGetVar(wp, T("wechatMaxSessions"), T(""));

	smsTimes = websGetVar(wp, T("smsTimes"), T(""));
	wechatTimes = websGetVar(wp, T("wechatTimes"), T(""));
	smsContents = websGetVar(wp, T("smsContents"), T(""));
	wechatContents = websGetVar(wp, T("wechatContents"), T(""));
	wechatName = websGetVar(wp, T("wechatName"), T(""));
	wechatToken = websGetVar(wp, T("wechatToken"), T(""));
	wechatKeyContents = websGetVar(wp, T("wechatKeyContents"), T(""));
	smsServer = websGetVar(wp, T("smsServer"), T(""));
#if 0
	printf("enabled:     %s,%s,%s\n", idcardEnabled, smsEnabled, wechatEnabled);
	printf("timetotal:   %s,%s,%s\n", idcardTimeTotal, smsTimeTotal, wechatTimeTotal);
	printf("day:         %s,%s,%s\n", idcardDay, smsDay, wechatDay);
	printf("massessions: %s,%s,%s\n", idcardMaxSessions, smsMaxSessions, wechatMaxSessions);
	printf("wechatname:  %s\n", wechatName);
	printf("token:       %s\n", wechatToken);
	printf("contents: \n----\n%s\n----\n%s\n----\n", smsContents, wechatContents);
#endif
#endif
#if (WEBAUTH_SELF_HELP == FYES)
	selfenabled = websGetVar(wp, T("selfenabled"), T(""));
#endif
#if(WEB_AUTH_STALE == FYES)
	stale = websGetVar(wp, T("staletime"), T(""));
#endif
	activePic = websGetVar(wp, T("ActivePic"), T(""));
	pexceptIpGroup = websGetVar(wp,T("exceptIpGroup"),T(""));
	ptipstitle = websGetVar(wp,T("tipstitle"),T(""));
	ptipsinfo = websGetVar(wp,T("tipsinfo"),T(""));
	contact = websGetVar(wp, T("hidcontact"), T(""));

	SWORD_PRINTF("%s %d: enabled : %s \n", __func__, __LINE__,  enabled);
	SWORD_PRINTF("%s %d: enpicture : %s \n", __func__, __LINE__,  enpicture);
	prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0))
	{
		ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, i, &profhead);
		if(strcmp(enabled,"on") == 0)
		{
			prof->enabled = 1;
		}
		else
		{
			prof->enabled = 0;
		}
#if (WIFIDOG_AUTH == FYES)
		if(strcmp(authtype, T("localAuth")) == 0) {
		    prof->authType = 1;
		} else if(strcmp(authtype, T("remoteAuth")) == 0) {
		    prof->authType = 2;
		}
#if (SMS_ENABLE ==1)
		if(strcmp(smsEnable,"on") == 0)
		{
			prof->smsEnable = 1;
		}
		else
		{
			prof->smsEnable = 0;
		}
		if(strcmp(cdmaType, T("TD-CDMA")) == 0) {
		    prof->cdmaType = 1;
		} else if(strcmp(cdmaType, T("WCDMA")) == 0) {
		    prof->cdmaType = 2;
		}else{
		    prof->cdmaType = 3;
		}
		strcpy(prof->province,province);
		strcpy(prof->city,city);
		strcpy(prof->sms_number,sms_number);
#endif
#endif
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
	    if (strcmp(idcardEnabled, "on") == 0) {
		prof->idcard.enabled = 1;
	    } else {
		prof->idcard.enabled = 0;
	    }
#endif
	    
#if (WEBAUTH_AUTO == FYES)
	    if (strcmp(smsEnabled, "on") == 0) {
		prof->sms.enabled = 1;
		prof->sms.timeTotal = (Uint32)strtol(smsTimeTotal, NULL, 10);
		if (strcmp(smsDay, "day") == 0)
		    prof->sms.timeTotal *= 24;
		prof->sms.maxSessions = (Uint32)strtol(smsMaxSessions, NULL, 10);
//		strcpy(prof->sms.contents, smsContents);
		totalTimes = (Uint32)strtol(smsTimes, NULL, 10);
		if ((prof->sms.totalTimes != 0) && (totalTimes == 0))
		        smsListDelAll();                                 
		prof->sms.totalTimes = totalTimes;                   
		strcpy(prof->sms.smsServer, smsServer);
#if 1
		FILE *fp = fopen("/utt/smsname", "r");
		char smsname[10];
		if (fp) {
		    memset(smsname, 0, sizeof(smsname));
		    fscanf(fp, "%s", smsname);
		    printf("read name: %s\n", smsname);
		    sprintf(prof->sms.contents, "验证码是##，上网有效期为**。【%s】", smsname);
		    fclose(fp);
		}
#endif
#if 1
		FILE *fpS = fopen("/etc/smsserver", "w");
		char logserver[141];
		if (fpS) {
		    fprintf(fpS, "%s", smsServer);
		    fclose(fpS);
		}
#endif
	    } else {
		prof->sms.enabled = 0;
	    }
	    
	    if (strcmp(wechatEnabled, "on") == 0) {
		prof->wechat.enabled = 1;
		prof->wechat.timeTotal = (Uint32)strtol(wechatTimeTotal, NULL, 10);
		if (strcmp(wechatDay, "day") == 0)
		    prof->wechat.timeTotal *= 24;
		prof->wechat.maxSessions = (Uint32)strtol(wechatMaxSessions, NULL, 10);
		totalTimes = (Uint32)strtol(wechatTimes, NULL, 10);     
		if ((prof->wechat.totalTimes != 0) && (totalTimes == 0))
		        wechatListDelAll();                                 
		prof->wechat.totalTimes = totalTimes;                   
		strcpy(prof->wechat.contents, wechatContents);
		strcpy(prof->wechat.wechatName, wechatName);
		strcpy(prof->wechat.wechatToken, wechatToken);
		strcpy(prof->wechat.wechatKeyContents, wechatKeyContents);
	    } else {
		prof->wechat.enabled = 0;
	    }
#endif
		prof->enpicture = (Uint32)strtol(enpicture,NULL,10);
#if (WEBAUTH_ADVANCE == FYES)
		if(prof->enpicture<2)/*若是简易配置，则使用默认页面*/
		{
			prof->enDefPage = 1;
		}else{
			prof->enDefPage = 0;
		}
#endif
#if (WEBAUTH_SELF_HELP == FYES)
		if(strcmp(selfenabled,"on") == 0)
		{
			prof->self_help = FUN_ENABLE;
		}
		else
		{
			prof->self_help = FUN_DISABLE;
		}
		SWORD_PRINTF("%s %d: selfenabled : %s, self_help = %d\n", __func__, __LINE__,  selfenabled, prof->self_help);
#endif
#if(WEB_AUTH_STALE == FYES)
		prof->staleT = (Uint32)strtol(stale,NULL,10);
#endif
		if(strcmp(activePic,"on") == 0)
		{
			prof->active_pic = 1;
		}
		else
		{
			prof->active_pic = 0;
		}
		strcpy(prof->contact, contact);
		saveContactToFile(contact, WEB_AUTH_CONTACT_PATH);
		/*地址组名的长度不能大于IP_GROUP_NAME_LEN*/
		strcpy(prof->exceptIpGroup,pexceptIpGroup);
		strcpy( prof->tipstitle, ptipstitle);
		strcpy( prof->tipsinfo, ptipsinfo);
		prof->head.active = TRUE;

	}

#if (WEBAUTH_ACCOUNT == FYES)
        uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit(); /*save*/
	websRedirect(wp, "WebAuthGlobal.asp");
	return;
}
/********************************************************************
 * 函数： getAddreGrpList
 * 功能： 将所有组中地址组输出 
 * 创建： 2012-08-15
 * 参数： 无
 * 返回： 略
 * 输出： 多有组中的地址组
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
#if (IP_GRP == FYES)
static int getAddreGrpList(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_IPGRP;
    IpGrpProfile *prof = NULL;

    int min = 0, max = 0;
    int i = 0;
    int num = 0;

    websWrite(wp,"var addrGroupName = new Array();");

    ProfInstLowHigh(mibType, &max, &min);
    /*遍历所有地址组*/
    for(i = min; i < max; i++) 
    {
	prof = (IpGrpProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) 
	{
	    if(prof->grpType == IPGRP_IP)
	    {/*组类型是地址组的组*/
		websWrite(wp,"addrGroupName[%d] = \"%s\";", num, prof->head.name);
		num++;
	    }
	}
    }
    return 0;
}
#endif


/********************************************************************
 *  * 函数： _getWebAuthListData
 *   * 功能： 网页认证配置页面输出账号相关信息
 *    * 创建： 2014-01-08
 *     * 参数： 略
 *      * 返回： 略
 *       * 输出： 略
 *        * 作者： 李文昌 
 *         ********************************************************************/ 
static void _getWebAuthListData(webs_t wp, MibProfileType mibType, int *num)
{
    WebAuthProfile *prof = NULL;
    int min = 0, max = 0;
    int i = 0;
    int tmCurrent = 0;
    const char *MsgStr = NULL;

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) 
    {
	prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
	{
	    websWrite(wp,"instIndexs[%d] = \"%s\";", *num, prof->head.name);
#if (WEBAUTH_AUTO == FYES)
	    if (prof->autoAuthMethod == WECHAT_METHOD)
		websWrite(wp,"WebAuthNames[%d] = \"微信用户\";", *num);
	    else
#endif
		websWrite(wp,"WebAuthNames[%d] = \"%s\";", *num, prof->user);

#if (WEBAUTH_ACCOUNT == FYES)
	    tmCurrent = time(NULL);/*获取当前时间*/
	    if ((prof->timeingPatternEn == FUN_ENABLE) && ((prof->timeStart > tmCurrent) || (tmCurrent > prof->timeStop) || ((prof->timeTotal != 0U) && (prof->timeUsed >= prof->timeTotal))) && ((tmCurrent < DELIVERY_START_TIME) || (tmCurrent > DELIVERY_END_TIME)))
	    {
		MsgStr = getTransDataForC(C_LANG_INDEX_PPPOE_ACC_EXPIRED);
	    }
	    else
#endif
#if (WEB_AUTH_SHARE == FYES)
		if(prof->actualSession <= 0)
#else /*WEB_AUTH_SHARE*/
		    if(prof->ip == 0)
#endif /*WEB_AUTH_SHARE*/
		    {
			MsgStr = getTransDataForC(C_LANG_INDEX_NOT_IN_USE);
		    }
		    else
		    {
			MsgStr = getTransDataForC(C_LANG_INDEX_IN_USE);
		    }
	    websWrite(wp,"Status[%d] = \"%s\";", *num, MsgStr);
	    websWrite(wp,"IPAddress[%d] = \"%N\";", *num, prof->ip);
#if (WEBAUTH_ACCOUNT == FYES)
	    if (prof->timeingPatternEn == FUN_ENABLE)
	    {
		websWrite(wp,"timeStarts[%d] = \"%d\";", *num, prof->timeStart);
		websWrite(wp,"timeStops[%d] = \"%d\";", *num, prof->timeStop);
#if (WEBAUTH_AUTO == FYES)
		if (prof->autoAuthMethod == SMS_METHOD || prof->autoAuthMethod == WECHAT_METHOD) {
		    websWrite(wp,"timeTotalSecs[%d] = \"%lu\";", *num, prof->timeStop - prof->timeStart);
		    websWrite(wp,"timeUsedSecs[%d] = \"%lu\";", *num, tmCurrent - prof->timeStart);
		} else 
#endif
		{
		    websWrite(wp,"timeTotalSecs[%d] = \"%lu\";", *num, prof->timeTotal);
		    websWrite(wp,"timeUsedSecs[%d] = \"%lu\";", *num, prof->timeUsed);
		}

		MsgStr = getTransDataForC(C_LANG_INDEX_ENABLED);
	    }
	    else
#endif
	    {
		MsgStr = getTransDataForC(C_LANG_INDEX_DISABLED);
	    }

#if (WEBAUTH_ACCOUNT == FYES)
	    websWrite(wp,"timeingPatternEns[%d] = \"%s\";", *num, MsgStr);
#endif
	    websWrite(wp,"Description[%d] = \"%s\";", *num, prof->remark);
	    websWrite(wp,"sessionNum[%d] = \"%d\";", *num, prof->maxSession);
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
	    if (prof->autoAuthMethod == MANUAL_METHOD)
		websWrite(wp,"authMethods[%d] = \"%s\";", *num, "手动");
	    else if (prof->autoAuthMethod == IDCARD_METHOD)
		websWrite(wp,"authMethods[%d] = \"%s\";", *num, "身份证");
	    else if (prof->autoAuthMethod == SMS_METHOD)
		websWrite(wp,"authMethods[%d] = \"%s\";", *num, "短信");
	    else if (prof->autoAuthMethod == WECHAT_METHOD)
		websWrite(wp,"authMethods[%d] = \"%s\";", *num, "微信");

#endif
#if DEBUG
	    printf("%s-%d: maxSession =%d\n",__func__,__LINE__,prof->maxSession);
#endif
	    (*num)++;
	}
    }

    return;
}

/********************************************************************
 * 函数： getWebAuthListData
 * 功能： 网页认证配置页面输出相关信息
 * 创建： 2012-04-13
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int getWebAuthListData(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH;
    WebAuthProfile *prof = NULL;
    int min = 0, max = 0;
    int i = 0;
	int num = 0;
	const char *MsgStr = NULL;

	int tmCurrent = time(0);
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
	MibProfileType mibTypeC = MIB_PROF_WEB_AUTH_CPT;	
#endif
	websWrite(wp,"var instIndexs =new Array();");
	websWrite(wp,"var WebAuthNames = new Array();");
	websWrite(wp,"var Status = new Array();");
	websWrite(wp,"var IPAddress = new Array();");
	websWrite(wp,"var timeingPatternEns = new Array();");
	websWrite(wp,"var timeStarts = new Array();");
	websWrite(wp,"var timeStops = new Array();");
	websWrite(wp,"var dateStarts = new Array();");
	websWrite(wp,"var dateStops = new Array();");
	websWrite(wp,"var timeTotalSecs = new Array();");
	websWrite(wp,"var timeUsedSecs = new Array();");
	websWrite(wp,"var timeTotals = new Array();");
	websWrite(wp,"var timeUseds = new Array();");
	websWrite(wp,"var Description = new Array();");
	websWrite(wp,"var sessionNum = new Array();");
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
	websWrite(wp, "var authMethods = new Array();");
#endif
	websWrite(wp, T("var totalrecs;"));
	websWrite(wp, T("\n"));
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	_getWebAuthListData(wp, mibType, &num);
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
	_getWebAuthListData(wp, mibTypeC, &num);
#endif
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
/*会覆盖错误信息，改为在页面上声明
	getTheErrorMsg(eid, wp, argc, argv);
*/
	websWrite(wp,"var totalrecs=%d;",num);
	websWrite(wp,"var max_totalrecs =%d;",MAX_WEB_AUTH_PROFILES);
	return 0;
}
/********************************************************************
 * 函数： _getWebAuthOneInfo
 * 功能： 网页认证配置页面单个用户输出相关信息
 * 创建： 2014-01-27
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： 李文昌
 ********************************************************************/ 
static int _getWebAuthOneInfo(webs_t wp, MibProfileType mibType, char *editName)
{
	WebAuthProfile *prof = NULL;
	int min = 0, max = 0;
	int i = 0;
#if (WEBAUTH_ACCOUNT == FYES)
	char timeTotal[12];
#endif

	ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++) 
	{
	    prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
	    if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE) && (strcmp(editName, prof->head.name) == 0))
	    {
#if (WEBAUTH_AUTO == FYES)
		if (prof->autoAuthMethod == WECHAT_METHOD)
		    websWrite(wp,"usernames = \"微信用户\";");
		else
#endif
		    websWrite(wp,"usernames = \"%s\";", prof->user);
		websWrite(wp, "authpasswds = \"%s\";", prof->passwd);
		websWrite(wp, "descriptions = \"%s\";", prof->remark);
		websWrite(wp, "sessionNum = \"%d\";", prof->maxSession);
#if (WEBAUTH_ACCOUNT == FYES)
		sprintf(timeTotal, "%.1f", prof->timeTotal / 3600.0);/*秒转换成小时*/
		websWrite(wp, "timetotal = %lu;", prof->timeTotal);
		websWrite(wp, "timingmode = \"%d\";", prof->timeingPatternEn);
		websWrite(wp, "datestart = \"%d\";", prof->timeStart);
		websWrite(wp, "dateend = \"%d\";", prof->timeStop);
#endif
		websWrite(wp, "instIndexs = \"%s\";", prof->head.name);
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
		int type;
		if (prof->autoAuthMethod == SMS_METHOD)
		    type = 1;
		else if (prof->autoAuthMethod == WECHAT_METHOD)
		    type = 2;
		else
		    type = 0;
		websWrite(wp, "var currentType = \"%d\";", type);
#endif
		return 1;
	    }
	}
	return 0;
}
/********************************************************************
 * 函数： getWebAuthOneInfo
 * 功能： 网页认证配置页面单个用户输出相关信息
 * 创建： 2012-04-13
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int getWebAuthOneInfo(int eid, webs_t wp, int argc, char **argv)
{
	MibProfileType mibType = MIB_PROF_WEB_AUTH;
	WebAuthProfile *prof = NULL;
	int ret;

	char *editName = NULL;
	const char *ErrMsg = NULL;

	websWrite(wp, "var usernames = \"\";");
	websWrite(wp, "var authpasswds = \"\";");
	websWrite(wp, "var descriptions = \"\";");
	websWrite(wp, "var sessionNum = \"\";");
	websWrite(wp, "var instIndexs =\"\";");
#if (WEBAUTH_ACCOUNT == FYES)
	websWrite(wp, "var timetotal = \"\";");
	websWrite(wp, "var timingmode = \"\";");
	websWrite(wp, "var datestart = \"\";");
	websWrite(wp, "var dateend = \"\";");
#endif
	if ((ejArgs(argc, argv, T("%s"), &editName) < 1) || (editName == NULL))/*查看实例名是否存在*/ 
	{
		ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_READ_DATA_ERR);
		setTheErrorMsg(ErrMsg);
	}
	else
	{
#if (WEBAUTH_ACCOUNT == FYES)
		uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
		ret = _getWebAuthOneInfo(wp, mibType, editName);
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
		if (ret == 0) {
		    mibType = MIB_PROF_WEB_AUTH_CPT;
		    ret = _getWebAuthOneInfo(wp, mibType, editName);
		}

#endif
#if (WEBAUTH_ACCOUNT == FYES)
		uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	}
	getTheErrorMsg(eid, wp, argc, argv);
	return 0;
}
/********************************************************************
 * 函数： getWebAuthGlobal
 * 功能： 网页认证全局开关信息
 * 创建： 2012-04-13
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int getWebAuthGlobal(int eid, webs_t wp, int argc, char **argv)
{
	MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
	WebAuthGlobalProfile *prof = NULL;
	websWrite(wp, "var lively = \"\";");
	websWrite(wp, "var authtype = \"\";");
	websWrite(wp, "var smsShow = \"\";");
	websWrite(wp, "var whiteListShow = \"\";");
#if (SMS_ENABLE ==1)
	websWrite(wp, "var smsEnable = \"\";");
	websWrite(wp, "var cdmaTupe = \"\";");
	websWrite(wp, "var province = \"\";");
	websWrite(wp, "var city = \"\";");
	websWrite(wp, "var sms_number = \"\";");
	websWrite(wp, "smsShow = \"yes\";");
#endif
#if (WHITELIST ==1)
	websWrite(wp, "whiteListShow = \"yes\";");
#ifdef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
	websWrite(wp, "var wildchar = 1;");
#else
	websWrite(wp, "var wildchar = 0;");
#endif
#endif
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
	{
		websWrite(wp, "lively = \"%d\";", prof->enabled);
	}
#if (WIFIDOG_AUTH == FYES)
		if(prof->authType == 2) {
		    websWrite(wp, "authtype = \"%s\";", T("remoteAuth"));
		} else {
		    websWrite(wp, "authtype = \"%s\";", T("localAuth"));
		}
#if (SMS_ENABLE ==1)
		websWrite(wp, "smsEnable = \"%d\";", prof->smsEnable);	
		websWrite(wp, "province = \"%s\";", prof->province);	
		websWrite(wp, "city = \"%s\";", prof->city);	
		websWrite(wp, "sms_number = \"%s\";", prof->sms_number);	
		if(prof->cdmaType == 1) {
		    websWrite(wp, "cdmaType = \"%s\";", T("TD-CDMA"));
		} else if(prof->cdmaType == 2){
		    websWrite(wp, "cdmaType = \"%s\";", T("WCDMA"));
		}else{
		    websWrite(wp, "cdmaType = \"%s\";", T("CDMA2000"));
		}
#endif
#if (WHITELIST ==1)
	getWhiteList(eid, wp, argc,argv);
#endif
#endif
	websWrite(wp, "var webAuthAutoFunc = \"\";");
	websWrite(wp, "var webAuthIdcode = \"\";");
#if (WEBAUTH_AUTO == FYES)
#if 0
	printf("var idcardEnableds=\"%d\";\n", prof->idcard.enabled);
	printf("var smsEnableds=\"%d\";\n", prof->sms.enabled);
	printf("var wechatEnableds=\"%d\";\n", prof->wechat.enabled);

	printf("var idcardTimeTotals=\"%d\";\n", prof->idcard.timeTotal);
	printf("var smsTimeTotals=\"%d\";\n", prof->sms.timeTotal);
	printf("var wechatTimeTotals=\"%d\";\n", prof->wechat.timeTotal);

	printf("var idcardMaxSessionss=\"%d\";\n", prof->idcard.maxSessions);
	printf("var smsMaxSessionss=\"%d\";\n", prof->sms.maxSessions);
	printf("var wechatMaxSessionss=\"%d\";\n", prof->wechat.maxSessions);

	printf("var smsContentss=\"%s\";\n", prof->sms.contents);
	printf("var wechatContentss=\"%s\";\n", prof->wechat.contents);
	printf("var wechatNames=\"%s\";\n", prof->wechat.wechatName);
	printf("var wechatTokens=\"%s\";\n", prof->wechat.wechatToken);
#endif
	websWrite(wp, "webAuthAutoFunc = \"1\";");
	websWrite(wp, "var idcardEnableds=\"%d\";", prof->idcard.enabled);
	websWrite(wp, "var smsEnableds=\"%d\";", prof->sms.enabled);
	websWrite(wp, "var wechatEnableds=\"%d\";", prof->wechat.enabled);

	websWrite(wp, "var idcardTimeTotals=\"%d\";", prof->idcard.timeTotal);
	websWrite(wp, "var smsTimeTotals=\"%d\";", prof->sms.timeTotal);
	websWrite(wp, "var wechatTimeTotals=\"%d\";", prof->wechat.timeTotal);

	websWrite(wp, "var idcardMaxSessionss=\"%d\";", prof->idcard.maxSessions);
	websWrite(wp, "var smsMaxSessionss=\"%d\";", prof->sms.maxSessions);
	websWrite(wp, "var wechatMaxSessionss=\"%d\";", prof->wechat.maxSessions);

	websWrite(wp, "var smsTimess=\"\";");
	websWrite(wp, "var wechatTimess=\"\";");
	websWrite(wp, "smsTimess=\"%d\";", prof->sms.totalTimes);
	websWrite(wp, "wechatTimess=\"%d\";", prof->wechat.totalTimes);
	websWrite(wp, "var smsContentss=\"%s\";", prof->sms.contents);
//	websWrite(wp, "var wechatContentss=\"%s\";", prof->wechat.contents);
	websWrite(wp, "var wechatNames=\"%s\";", prof->wechat.wechatName);
	websWrite(wp, "var wechatTokens=\"%s\";", prof->wechat.wechatToken);
	websWrite(wp, "var wechatKeyContentss=\"%s\";", prof->wechat.wechatKeyContents);
	websWrite(wp, "var smsServers=\"%s\";", prof->sms.smsServer);
#endif
#if (WEBAUTH_IDCODE == FYES)
	websWrite(wp, "webAuthIdcode = \"1\";");
	websWrite(wp, "var idcardEnableds=\"%d\";", prof->idcard.enabled);
	websWrite(wp, "var idcardTimeTotals=\"%d\";", prof->idcard.timeTotal);
	websWrite(wp, "var idcardMaxSessionss=\"%d\";", prof->idcard.maxSessions);
#endif
#if (WEBAUTH_SELF_HELP == FYES)
	if(prof->self_help == FUN_ENABLE)
	{
		websWrite(wp, "var selfEnable=\"%s\";", T("on"));
	}
	else
	{
		websWrite(wp, "var selfEnable=\"%s\";", T("off"));
	}
#else
	websWrite(wp, "var selfEnable=\"%s\";", T("null"));
#endif
#if(WEB_AUTH_STALE == FYES)
	websWrite(wp, "var staleTime=%d;", prof->staleT);
#endif
	websWrite(wp, "var exceptIpGroups=\"%s\";", prof->exceptIpGroup);
#if (WEBAUTH_ADVANCE == FYES)
	char* chk = NULL;
	char webPath[40];
	memset(webPath,'\0',sizeof(webPath));
	strcat(webPath,"/ff/");
	strcat(webPath,prof->pageName);
	chk = strrchr(prof->pageName,'.');
	if ((chk != NULL)&&(strcmp(chk,".asp") == 0)&&(isWebFileExits(webPath) == 1)){
	websWrite(wp, "var webFileNames=\"%s\";", prof->pageName);
	}else{
	websWrite(wp, "var webFileNames=\"\";");
	}
	websWrite(wp, "var WebAuth_Advance =\"%s\";",T("on"));
#else
	websWrite(wp, "var webFileNames=\"\";");
	websWrite(wp, "var WebAuth_Advance =\"%s\";",T("off"));
#endif
	getTheRightMsg(eid, wp, argc, argv);
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	return 0;
}


/***************************************************************
 * 功能：替换字符串content中的源字符串srcstr为目标字符destchar
 * 创建时间：2013-04-23
 * 参数：content--源字符数组
 *      srcstr--要替换的字符串
 *      destchar--替换成的字符
 * 返回值：0--成功 
 *************************************************************/
static int replaceStr(char *content, char *srcstr, char destchar)
{
    char *write_content = NULL;
    unsigned int i = 0U, j = 0U, k = 0U;
    unsigned int len = 0U;
    
    len = strlen(srcstr) - 1U;
    for(i = 0U; content[i] != '\0'; i=k+1U)
    {
	write_content = strstr(&content[i], srcstr);
	if(write_content != NULL)
	{
	    k = write_content - content;
	    content[k] = destchar;
	    /*将替换字符串后面的字符依次向前移动*/
	    for(j = k + 1U; content[j + len] != '\0';j++)
	    {
	    content[j] = content[j + len];
	    }
	    content[j] = '\0';
	}
	else
	{
	    k = i;
	}
        }	    
    return 0;
}
#if (WEBAUTH_AUTO == FYES)
/********************************************************************
 * 函数功能：向web认证页面输出经过转换的短信回复内容
 * 创建时间：2014-01-16
 * 备注：第一个参数为html时，不转换直接输出到页面上
 *********************************************************************/
static int getSmsContentData(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *prof = NULL;
    char content[WEB_AUTH_CONTACT_LEN + 1];
    memset(content, 0, sizeof(content));
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
    prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
    if((ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
    {
    strncpy(content, prof->sms.contents, WEB_AUTH_CONTACT_LEN + 1);
    /*判断是否需要将html代码转换为jsp代码*/
    if((argv[0]== NULL) || (strcmp(argv[0], "html") != 0))
    {
        replaceStr(content, "<br>", '\n');/*转换html代码为jsp代码*/
        replaceStr(content, "&nbsp;", ' ');
    }
    websWrite(wp, "%s", content);
    }
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
    return 0;
}

/********************************************************************
 * 函数功能：向web认证页面输出经过转换的微信回复内容
 * 创建时间：2014-01-16
 * 备注：第一个参数为html时，不转换直接输出到页面上
 *********************************************************************/
static int getWechatContentData(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *prof = NULL;
    char content[WEB_AUTH_CONTACT_LEN + 1];
    memset(content, 0, sizeof(content));
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
    prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0))
    {
	strncpy(content, prof->wechat.contents, WEB_AUTH_CONTACT_LEN + 1);
    /*判断是否需要将html代码转换为jsp代码*/
    if((argv[0]== NULL) || (strcmp(argv[0], "html") != 0))
    {
        replaceStr(content, "<br>", '\n');/*转换html代码为jsp代码*/
        replaceStr(content, "&nbsp;", ' ');
    }
    websWrite(wp, "%s", content);
    }
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
    return 0;
}
#else
static int getSmsContentData(int eid, webs_t wp, int argc, char **argv)
{
    return 0;
}
static int getWechatContentData(int eid, webs_t wp, int argc, char **argv)
{
    return 0;
}
#endif
/********************************************************************
 * 函数功能：向web认证页面输出经过转换的联系方式信息
 * 创建时间：2013-03-08
 * 备注：第一个参数为html时，不转换直接输出到页面上
 *********************************************************************/
static int getContactData(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *prof = NULL;
    char content[WEB_AUTH_CONTACT_LEN + 1];
    memset(content, 0, sizeof(content));
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
    prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
    if((ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
    {
    strncpy(content, prof->contact, WEB_AUTH_CONTACT_LEN + 1);
    /*判断是否需要将html代码转换为jsp代码*/
    if((argv[0]== NULL) || (strcmp(argv[0], "html") != 0))
    {
        replaceStr(content, "<br>", '\n');/*转换html代码为jsp代码*/
        replaceStr(content, "&nbsp;", ' ');
    }
    websWrite(wp, "%s", content);
    }
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
    return 0;
}


/********************************************************************
 * 函数： webAuthSetMsg
 * 功能： 修改静态变量提示信息
 * 创建： 2012-04-13
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void webAuthSetMsg(const char *pMsg)
{
	if (pMsg == NULL)
	{
		//SWORD_PRINTF("%s %d, pMsg == NULL\n", __FUNCTION__, __LINE__);
		memset(webAuthMsg, 0, sizeof(webAuthMsg));
	}
	else
	{
		SWORD_PRINTF("%s %d,  pMsg  = %s\n", __FUNCTION__, __LINE__, pMsg);

		if (strlen(pMsg) < MAX_WEB_AUTH_PROFILES)
		{	
			SWORD_PRINTF("%s %d, pMsg  = %s\n", __FUNCTION__, __LINE__, pMsg);
			strcpy(webAuthMsg, pMsg);
		}
	}
	return;
}
/********************************************************************
 * 函数： getWebAuthMessage
 * 功能： 向用户认证页面输出信息：认证通过 or 认证失败
 * 创建： 2012-04-13
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int getWebAuthMessage(int eid, webs_t wp, int argc, char **argv) 
{
	MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
	WebAuthGlobalProfile *prof = NULL;
	MibProfileType mibType2 = MIB_PROF_WEB_AUTH;
	WebAuthProfile *userprof = NULL;
	prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
	int instIndex = -1;
	Uint32 userip=0U;
	char ip[16] ;
	struct sysinfo sysInfo;
	int currentType = 0;
    /* 获得lan口IP */
    memset(ip, 0, sizeof(ip));
    getIfIp(getLanIfName(), ip);
	sysinfo(&sysInfo);
	userip = inet_addr((const char *)wp->ipaddr);
	//SWORD_PRINTF("%s-%d: user ip = %x\n",__FUNCTION__,__LINE__,userip);
	if((userprof = WebAuthUserNameByIp(userip)) != NULL)
	{
		SWORD_PRINTF("%s-%d: prof->ip =%x\n",__FUNCTION__,__LINE__,userprof->ip);
#if (WEBAUTH_ACCOUNT == FYES)
		if(userprof->timeingPatternEn == FUN_ENABLE)
		{
			billing  = 1;
			stopData = userprof->timeStop;
			if(userprof->timeTotal == 0u){
			leftTime = 0u;
			}else{
#if (WEBAUTH_AUTO == FYES)
			    if (userprof->autoAuthMethod == SMS_METHOD || userprof->autoAuthMethod == WECHAT_METHOD)
				leftTime = userprof->timeStop - time(NULL);
			    else
#endif
				leftTime = userprof->timeTotal - userprof->timeUsed - ((unsigned long)sysInfo.uptime - userprof->timeLast);
			}
		}else
#endif
		{
			billing  = 0;
			stopData = 0u;
			leftTime = 0u;
		}
	}
	websWrite(wp, T("var ip = \"%s\";"), ip);
	websWrite(wp, T("var authResult = \"%s\";"), (char *)webAuthMsg);
	websWrite(wp, T("var en_picture = %d;"), prof->enpicture);
	websWrite(wp, T("var activePic = %d;"), prof->active_pic);
	websWrite(wp, T("var pictureUrl = \"%s\";"), prof->pictureUrl);
	websWrite(wp, T("var billing = \"%d\";"), billing);
	websWrite(wp, T("var leftTime = \"%lu\";"), leftTime);
	websWrite(wp, T("var stopData = \"%d\";"), stopData);
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
#if 0
	printf("var idcardauth = \"%d\";\n", prof->idcard.enabled?1:0);
	printf("var smsauth    = \"%d\";\n", prof->sms.enabled?1:0);
	printf("var weixinauth = \"%d\";\n", prof->wechat.enabled?1:0);
	printf("var wechatNames = \"%s\";\n", prof->wechat.wechatName);
// websWrite(wp, "var picturedir=\"%s\";", picname);
#endif
	websWrite(wp, T("var idcardauth = \"%d\";"), prof->idcard.enabled?1:0);
#endif
#if (WEBAUTH_AUTO == FYES)
	websWrite(wp, T("var smsauth    = \"%d\";"), prof->sms.enabled?1:0);
	websWrite(wp, T("var weixinauth = \"%d\";"), prof->wechat.enabled?1:0);
	websWrite(wp, T("var wechatNames = \"%s\";"), prof->wechat.wechatName);
	if (prof->sms.enabled)
	    currentType = 1;
	else if (prof->wechat.enabled)
	    currentType = 2;
	websWrite(wp, T("var currentType = \"%d\";"), currentType);
	websWrite(wp, T("var jumpUrl = \"%s\";"), prof->jumpUrl);
	if (prof->idcard.enabled || prof->sms.enabled || prof->wechat.enabled)
	{
	    websWrite(wp, T("var needjump = 1;"));
	}
	websWrite(wp, T("var weixinPicExist = \"%d\";"), (access("/etc_ro/web/images/weixinpic.jpg", 0) == 0)?1:0);
#endif
	//SWORD_PRINTF("%s %d, webAuth = %s\n", __FUNCTION__, __LINE__, webAuthMsg);
	webAuthSetMsg((const char *)NULL);
	return 0;
}
/********************************************************************
 * 函数： formWebAuthPOk
 * 功能： 认证成功，地址加入OK组
 * 创建： 2012-04-26
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void formWebAuthOk(webs_t wp, char_t *path, char_t *query)
{
	char *result = NULL;

	/*
	 * 通过result获得用户是否认证成功
	 */
	result = websGetVar(wp, T("result"), T(""));
	SWORD_PRINTF("%s %d, result = %s\n", __FUNCTION__, __LINE__, result);
	/*
	 * 成功
	 */
	if(strcmp(result, "OK") == 0)
	{
		if (ipsetTestIpStr(SYS_WEB_AUTH_OK, wp->ipaddr) == 0)
		{ 
			/*增加到已认证*/
			ipsetAOrDIpStr(SYS_WEB_AUTH_OK, wp->ipaddr, IPSET_MEM_ADD); 
		}
	}
	return;
}
/********************************************************************
 * 函数： aspGetWebAuthSelfHelpEnabled
 * 功能： output selfhelp is enable or disable
 * 创建： 2012-06-18
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/ 
static int aspGetWebAuthSelfHelpEnabled(int eid, webs_t wp, int argc, char **argv)
{
#if (WEBAUTH_SELF_HELP == FYES)
	websWrite(wp, "var selfhelpEnable=true;");
#else
	websWrite(wp, "var selfhelpEnable=false;");
#endif
	return 0;
}
#if (WEBAUTH_SELF_HELP == FYES)
/********************************************************************
 * 函数： _WebAuthUserNameByIp
 * 功能： 
 * 创建： 2014-01-18
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： 李文昌 
 ********************************************************************/ 
static WebAuthProfile* _WebAuthUserNameByIp(MibProfileType mibType, Uint32 ip)
{
	WebAuthProfile *prof = NULL;
	int i=0;
	int min=0, max=0;

	ProfInstLowHigh(mibType, &max, &min);
	for(i=min;i<max;i++)
	{
		prof = (WebAuthProfile *)ProfGetInstPointByIndex(mibType, i);
		if ((prof == NULL) || (ProfInstIsFree(prof)))
		    continue;
#if (WEB_AUTH_SHARE == FYES)
		/*根据ip查找账号所有会话*/
		if((prof!=NULL)&&(searchSessionByIp(prof,ip) == 1))  /*find inst*/
#else /*WEB_AUTH_SHARE*/
		if((prof!=NULL)&&(prof->ip == ip))  /*find inst*/
#endif /*WEB_AUTH_SHARE*/
		{
			break;
		}
	}

	//SWORD_PRINTF("%s-%d: i =%d\n",__FUNCTION__,__LINE__,i);
	if (i == max)
	    prof = NULL;

	return prof;
}
/********************************************************************
 * 函数： WebAuthUserNameByIp
 * 功能： 
 * 创建： 2012-06-15
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/ 
static WebAuthProfile* WebAuthUserNameByIp(Uint32 ip)
{
	int ret;
	MibProfileType mibType = MIB_PROF_WEB_AUTH;
	WebAuthProfile *prof = NULL;

	prof = _WebAuthUserNameByIp(mibType, ip);

#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
	if (prof == NULL)
	{
	    mibType = MIB_PROF_WEB_AUTH_CPT;
	    prof = _WebAuthUserNameByIp(mibType, ip);
	}
#endif

	return prof;
}
/********************************************************************
 * 函数： aspGetWebAuthSelfHelpData
 * 功能： 
 * 创建： 2012-06-15
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/ 
static int aspGetWebAuthSelfHelpData(int eid, webs_t wp, int argc, char **argv)
{
	MibProfileType mibTypeS = MIB_PROF_WEB_AUTH_GLOBAL;
	WebAuthGlobalProfile *profS = NULL;
	MibProfileType mibType = MIB_PROF_WEB_AUTH;
	WebAuthProfile *prof = NULL;
	Uint32 ip=0U;
	int instIndex=-1;

	ip = inet_addr((const char *)wp->ipaddr);
	SWORD_PRINTF("%s-%d; user ip = %x\n",__FUNCTION__,__LINE__,ip);
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	profS = (WebAuthGlobalProfile *)ProfGetInstPointByIndex(mibTypeS, 0);
	/*check is correct or not*/
	if((profS->enabled != 1) || (profS->self_help != FUN_ENABLE))
	{
		SWORD_PRINTF("%s-%d; \n",__FUNCTION__,__LINE__);
		websWrite(wp, T("var isWebAuthIp=false;"));
#if (WEBAUTH_ACCOUNT == FYES)
                uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
		return 0;
	}
	if((prof = WebAuthUserNameByIp(ip)) != NULL)
	{
		SWORD_PRINTF("%s-%d; \n",__FUNCTION__,__LINE__);
		websWrite(wp, T("var isWebAuthIp=true;"));
	}
	else
	{
		SWORD_PRINTF("%s-%d; \n",__FUNCTION__,__LINE__);
		websWrite(wp, T("var isWebAuthIp=false;"));
	}
	if(prof != NULL)
	{
		websWrite(wp, T("var userOptCnt=%d;"), MAX_USER_OPT_MOD_CNT-prof->optCnt);
		SWORD_PRINTF("%s-%d; instIndex=%d, name=%s, passwd=%s ,optCnt=%d\n",__FUNCTION__,__LINE__,instIndex,prof->user,prof->passwd,prof->optCnt);
	}
	else
	{
		SWORD_PRINTF("%s-%d; \n",__FUNCTION__,__LINE__);
		websWrite(wp, T("var userOptCnt=%d;"), MAX_USER_OPT_MOD_CNT);
	}
	SWORD_PRINTF("%s-%d; \n",__FUNCTION__,__LINE__);
	websWrite(wp, T("var optResult=\"%s\";"), webAuthMsg);  /*write msg*/
	webAuthSetMsg((const char *)NULL);
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	return 0;
}

/********************************************************************
 *  WebAuthIpUserName
 *  判断用户ip是否使用此账号名认证
 *  2012-06-15
 *  input:
 *	ip	    -   user ip 
 *	name	    -   webAuth name
 * output:
 *	-1	    -   ip 未使用此账号，错误
 *	instindex   -	instindex
 ********************************************************************/ 
static int WebAuthIpUserName(Uint32 ip, char *name)
{
	SWORD_PRINTF("%s-%d: ip=%x, name=%s\n",__FUNCTION__,__LINE__,ip,name);
	MibProfileType mibType = MIB_PROF_WEB_AUTH;
	WebAuthProfile *prof = NULL;
	int ret = -1;
	int i=0;
	int min=0, max=0;
	/*get inst min val;ue and max value*/
	ProfInstLowHigh(mibType, &max, &min);
	/*search from min to max*/
	for(i=min;i<max;i++)
	{
		prof = (WebAuthProfile *)ProfGetInstPointByIndex(mibType, i);
		/*compare with ip and username*/
#if (WEB_AUTH_SHARE == FYES)
		/*根据ip查找账号所有会话*/
		SWORD_PRINTF("%s-%d: ip=0x%x\n",__FUNCTION__,__LINE__,ip);
		if((prof!=NULL)&&(strcmp(prof->user, name)==0)&&(searchSessionByIp(prof,ip) == 1))  /*find inst*/
#else /*WEB_AUTH_SHARE*/
		SWORD_PRINTF("%s-%d: prof->ip=%x\n",__FUNCTION__,__LINE__,prof->ip);
		if((prof!=NULL)&&(prof->ip == ip)&&(strcmp(prof->user, name)==0))
#endif /*WEB_AUTH_SHARE*/
		{
#if DEBUG
		    printf("%s-%d: find,prof->user =%s\n",__func__,__LINE__,prof->user);
#endif
			ret = i;
			break;
		}
	}
	SWORD_PRINTF("%s-%d: ret=%d\n",__FUNCTION__,__LINE__,ret);
	/*return inst index*/
	return ret;
}

/********************************************************************
 * 函数： checkData
 * 功能： 
 * 创建： 2012-06-15
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/ 
static int checkData(Uint32 ip, char *name, char *oldpwd, char *newpwd, char *verifypwd)
{
	int ret=1;
	const char *ErrMsg = NULL;
	SWORD_PRINTF("%s-%d: param ip=%x, name=%s, oldpwd=%s,newpwd=%s, verifypwd=%s\n",__FUNCTION__,__LINE__,ip,name,oldpwd,newpwd,verifypwd);
	if((strlen(name)==0)||(strlen(oldpwd)==0)||(strlen(newpwd)==0)||(strlen(verifypwd)==0)||(strlen(name)>31)||(strlen(oldpwd)>31)||(strlen(newpwd)>31))
	{	/*check error*/
		ErrMsg = getTransDataForC(C_LANG_INDEX_YOUR_ARGS_WAS_WRONG);
		webAuthSetMsg(ErrMsg);  
		ret = 0;
	}
	else
	{
		if(strcmp(newpwd, verifypwd)!=0)
		{   /*check passwd*/
			ErrMsg = getTransDataForC(C_LANG_INDEX_PASSWD_NEW_PASSWD_DIFF);
			webAuthSetMsg(ErrMsg);
			ret = 0;
		}
	}
	return ret;
}
/********************************************************************
 * 函数： formWebAuthSelfHelpConfig
 * 功能： 
 * 创建： 2012-06-15
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/ 
static void formWebAuthSelfHelpConfig(webs_t wp, char *path, char *query)
{
	MibProfileType mibTypeS = MIB_PROF_WEB_AUTH_GLOBAL;
	WebAuthGlobalProfile *profS = NULL;
	MibProfileType mibType = MIB_PROF_WEB_AUTH;
	WebAuthProfile *prof = NULL;
	struProfAlloc *profList  = NULL;
	int instIndex = -1;
	Uint32 comIP=0U;
	Uint32 userip=0U;
	char passWd[UTT_PASSWD_LEN  + 1];
	char *name, *oldpwd, *newpwd, *verifypwd;
	const char *ErrMsg = NULL;
	/*get web value*/
	name = websGetVar(wp, T("userName"), T(""));
	oldpwd = websGetVar(wp, T("oldPasswd"), T(""));
	newpwd = websGetVar(wp, T("newPasswd"), T(""));
	verifypwd = websGetVar(wp, T("verifyPasswd"), T(""));
	SWORD_PRINTF("%s-%d: get web value userName=%s, oldpwd=%s, newpwd=%s,verifypwd=%s\n",__FUNCTION__,__LINE__,name,oldpwd,newpwd,verifypwd);
	memset(passWd, 0, UTT_PASSWD_LEN  + 1);
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	profS = (WebAuthGlobalProfile *)ProfGetInstPointByIndex(mibTypeS, 0);
	if((profS->enabled != 1) || (profS->self_help != FUN_ENABLE))
	{
		SWORD_PRINTF("%s-%d: \n",__FUNCTION__,__LINE__);
		ErrMsg = getTransDataForC(C_LANG_INDEX_YOU_HAVE_NO_LEGAL_POWER);
		webAuthSetMsg(ErrMsg);
		websRedirect(wp, T("waUsers.asp"));
		return;
	}
	userip = inet_addr((const char *)wp->ipaddr);
	SWORD_PRINTF("%s-%d: user ip = %x\n",__FUNCTION__,__LINE__,userip);
	/*check web value is correct or not*/
	if(checkData(userip, name, oldpwd, newpwd, verifypwd) == 0)
	{
		websRedirect(wp, T("waUsers.asp"));
		return;
	}
	instIndex = WebAuthIpUserName(userip, name);
	SWORD_PRINTF("%s-%d: instIndex = %d\n",__FUNCTION__,__LINE__,instIndex);
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, instIndex, &profList);
	if(instIndex != -1)
	{
		prof = (WebAuthProfile *)ProfGetInstPointByIndex(mibType, instIndex);
		SWORD_PRINTF("%s-%d: Prof->ip=%x\n",__FUNCTION__,__LINE__,prof->ip);
#if (WEB_AUTH_SHARE == FYES)
		if((prof!=NULL)&&(0U < prof->actualSession))	/*find inst*/
#else /*WEB_AUTH_SHARE*/
		if((prof!=NULL)&&(comIP != prof->ip))	/*find inst*/
#endif /*WEB_AUTH_SHARE*/
		{
			if(prof->optCnt<MAX_USER_OPT_MOD_CNT)
			{
				strcpy(passWd, prof->passwd);
				if(strcmp(passWd, oldpwd) == 0)
				{	/*save*/
					memset(prof->passwd, 0, UTT_PASSWD_LEN  + 1);
					strcpy(prof->passwd, newpwd);
					prof->optCnt++;
					SWORD_PRINTF("%s-%d: set passwd=%s\n",__FUNCTION__,__LINE__,prof->passwd);
					ErrMsg = getTransDataForC(C_LANG_INDEX_SUCCESSFUL_OPERATION);
					webAuthSetMsg(ErrMsg);
				}
				else
				{
					ErrMsg = getTransDataForC(C_LANG_INDEX_OLD_PASSWD_CIN_ERR);
					webAuthSetMsg(ErrMsg);
				}
			}
			else
			{
				ErrMsg = getTransDataForC(C_LANG_INDEX_YOU_MODIFY_5_TIMES_MORE);
				webAuthSetMsg(ErrMsg);
			}
		}
		else
		{
			ErrMsg = getTransDataForC(C_LANG_INDEX_THIS_ACCOUNT_NOT_CONNECT);
			webAuthSetMsg(ErrMsg);
		}
	}
	else
	{
		ErrMsg = getTransDataForC(C_LANG_INDEX_NO_LEGAL_POWER_FOR_OPERATION);
		webAuthSetMsg(ErrMsg);
	}
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	SWORD_PRINTF("%s-%d: \n",__FUNCTION__,__LINE__);
	ProfUpdate(profList);   /*update*/
	ProfFreeAllocList(profList);
	nvramWriteCommit();	    /*save*/
	websRedirect(wp, T("waUsers.asp"));
	return;
}
#endif
#else
static int getWebAuthListData(int eid, webs_t wp, int argc, char **argv)
{
    return 0;
}
extern void websDefineWebAuth(void)
{
    websAspDefine(T("aspOutputWebAuthListData"), getWebAuthListData);     
}
#endif
#if (UTT_KNOWIFI == FYES)
static int aspGetKnowifiConfig(int eid, webs_t wp, int argc, char_t **argv)
{
   	MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
	WebAuthGlobalProfile *profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);

	websWrite(wp, "var wifidogEn = \"\";");
	websWrite(wp, "var knowifiEn = \"\";");
	websWrite(wp, "var uttSn = \"\";");
	websWrite(wp, "var routerID = \"\";");

	if((profG != NULL) && (ProfInstIsFree(profG) == 0) && (profG->head.active == TRUE))
	{
	    websWrite(wp, "var wifidogEn = \"%d\";", profG->enabled);
	    websWrite(wp, "var knowifiEn = \"%d\";", profG->knowifi_enable);
	    websWrite(wp, "var uttSn = \"%u\";", getSystemSn());
	    //websWrite(wp, "var routerID = \"%s\";", profG->knowifi_routeId);
			    
	}

#if 0
	MibProfileType mibType = MIB_PROF_KNOWIFI_DOMAIN;
	KnowifiDomainProfile *prof = NULL;
	int i, max, min;
	int total = 0;
	ProfInstLowHigh(mibType, &max, &min);

	websWrite(wp, "var deviceName = new Array();");
	websWrite(wp, "var userName = new Array();");
	websWrite(wp, "var deviceIp = new Array();");
	websWrite(wp, "var deviceMac = new Array();");
	websWrite(wp, "var deviceStatus = new Array();");
	websWrite(wp, "var deviceConntype = new Array();");
	websWrite(wp, "var deviceDoamin = new Array();");

	for (i = min; i < max; i++)
	{
	    prof = (KnowifiDomainProfile*)ProfGetInstPointByIndex(mibType, i);
	    //if (prof != NULL && prof->head.active == TRUE)
	    if (prof != NULL && ProfInstIsFree(prof) == 0)
	    {
		websWrite(wp, "deviceName[%d] = \"%s\";", total, prof->deviceName);
		websWrite(wp, "userName[%d] = \"%s\";", total, prof->username);
		websWrite(wp, "deviceIp[%d] = \"%s\";", total, prof->ip);
		websWrite(wp, "deviceMac[%d] = \"%s\";", total, prof->mac);
		websWrite(wp, "deviceStatus[%d] = \"%d\";", total, prof->status);
		websWrite(wp, "deviceConntype[%d] = \"%d\";", total, prof->conntype);
		websWrite(wp, "deviceDoamin[%d] = \"%s\";", total, prof->domain);
		total ++;	
	    }
	}
	websWrite(wp, "var totalDevice = \"%d\";", total);
#endif
	

	getTheErrorMsg(eid, wp, argc, argv); 
	getTheRightMsg(eid, wp, argc, argv);
}
static void formOpenLearn(webs_t wp, char_t *path, char_t *query){
    FILE *fp;
    char cl[200];
    int routerTimes = 3;
    char_t weburl[1024];
    char_t rtoken[255];
    int i = 0, j = 0, k = 0;;
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);

    memset(weburl, 0, sizeof(weburl));

    if (strcmp(profG->knowifi_routeId, "unknown") && strcmp(profG->knowifi_routeId, ""))
    {
	int flagOk = 0;
	doSystem("wget -O filerouterID http://api.greenwifi.com.cn/open/knowifi/v1.0/getRtoken/%s &", profG->knowifi_routeId);
	while(routerTimes--)
	{
	    sleep(1);
	    if((fp=fopen("filerouterID","r"))==NULL)
		continue;
	    while(fscanf(fp,"%s",cl)==1){
		if(i==6){
		    memset(rtoken, 0, sizeof(rtoken));
		    for(j = 0, k = 0; cl[j] != '\0'; j++)
		    {
			if (cl[j] == '\"') continue;
			rtoken[k] = cl[j];
			k++;
		    }

		    flagOk = 1;
		    sprintf(weburl,"http://service.qingxueba.com/lm/studysetting?rtoken=%s",rtoken);
		    break;
		}
		i++;
	    }
	    if(fclose(fp)!=0)
		exit(1);
	    else
		doSystem("rm filerouterID");
	    if (flagOk)
		break;
	}
	websRedirect(wp, weburl);
    }else{
	websRedirect(wp, "transition.asp");
    }

}
static void formKnowifiConfig(webs_t wp, char_t *path, char_t *query)
{
	MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
	WebAuthGlobalProfile *prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
	struProfAlloc *profhead = NULL;

#if (WEBAUTH_ACCOUNT == FYES)
        uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	char *wifidogEn = NULL;
	char *authType = NULL;
	char *knowifiEn = NULL;
	char *knowifiModel = NULL;

	knowifiEn = websGetVar(wp, T("knowifiEnable"), T("0"));

	if((prof != NULL) && (ProfInstIsFree(prof) == 0))
	{
	    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
	    if(strcmp(knowifiEn,"on") == 0)
	    {
		prof->enabled = 1;
		prof->authType = 2;
		prof->knowifi_enable = 1;
	    }
	    else
	    {
		prof->enabled = 0;
		prof->knowifi_enable = 0;
	    }
	    prof->head.active = TRUE;
	}
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit(); /*save*/
	setTheRightMsg("saveok");
	websRedirect(wp, "learnConfig.asp");
	return ;
}
#if 1 // 手动添加domian受控设备，用户调试。
static void formKnowifiAdd(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_KNOWIFI_DOMAIN;
    KnowifiDomainProfile *prof = NULL;
    char *devname = NULL;
    char *devip = NULL;
    char *devmac = NULL;
    char *devstatus = NULL;
    char *devconntype = NULL;
    char *devdomain = NULL;

    int i, max, min;
    int instIndex;
    ProfInstLowHigh(mibType, &max, &min);
    int saveOk = 0;

    devname = websGetVar(wp, T("devname"), T(""));
    devip = websGetVar(wp, T("devip"), T(""));
    devmac = websGetVar(wp, T("devmac"), T(""));
    devstatus = websGetVar(wp, T("devstatus"), T(""));
    devconntype = websGetVar(wp, T("devconntype"), T(""));
    devdomain = websGetVar(wp, T("devdomain"), T(""));
    
    instIndex = ProfGetInstIndexByName(mibType, devmac);

    if(instIndex == PROFFAIL)
    {
	instIndex = ProfNewInst(mibType, devmac, FALSE);
	if(instIndex == PROFFAIL) {
	    goto out;
	}
    }
    
    prof = (KnowifiDomainProfile*)ProfGetInstPointByIndex(mibType, instIndex);

    ProfSetNameByPoint(prof, devmac);

    strncpy(prof->deviceName, devname, 128);
    strncpy(prof->ip, devip, 16);
    strncpy(prof->mac, devmac, 18);
    prof->status = strtol(devstatus, NULL, 10);
    prof->conntype = strtol(devconntype, NULL, 10);
    strncpy(prof->domain, devdomain, 128);
    prof->head.active = TRUE;

    saveOk = 1;

    doSystem("ipset -A knowifi_redirect_client  %s,%s",prof->ip, prof->mac);

    nvramWriteCommit(); /*save*/

out:
    if (saveOk == 1)
	setTheRightMsg("saveok");
    websRedirect(wp, "learnConfig.asp");

}
static void formKnowifiDel(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_KNOWIFI_DOMAIN;
    KnowifiDomainProfile *prof = NULL;
    char *mactmp = NULL;
    mactmp = websGetVar(wp, T("devmac"), T(""));

    prof = (KnowifiDomainProfile *)ProfGetInstPointByName(mibType, mactmp);
    if (prof != NULL)
    {
	printf("del mac : %s\n", mactmp);
	doSystem("ipset -D knowifi_redirect_client  %s",prof->ip);
	ProfDelInstByName(mibType, mactmp);
    }

    nvramWriteCommit(); /*save*/
    websRedirect(wp, "learnConfig.asp");
}
static void formKnowifiDelAll(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_KNOWIFI_DOMAIN;
    KnowifiDomainProfile *prof = NULL;
    char *mactmp = NULL;
    mactmp = websGetVar(wp, T("devmac"), T(""));
    int i, max, min;
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++)
    {
	printf("-test : %d\n", i);
	prof = (KnowifiDomainProfile*)ProfGetInstPointByIndex(mibType, i);
	//if (prof != NULL && prof->head.active == TRUE)
	if (prof != NULL && ProfInstIsFree(prof) == 0)
	{
	    printf("--cyx--\n-prof : %X\n-active : %d\n-devname : %s\n-ip : %s\n-mac : %s\n-status : %d\n-conntype : %d\n-domain : %s\n",
		    prof, prof->head.active ,prof->deviceName, prof->ip, prof->mac, prof->status, prof->conntype, prof->domain);
	    //if (prof->mac && strncmp(prof->mac, mactmp, 18) == 0)
	    ProfDelInstByIndex(mibType, i);
	}

    }
    doSystem("ipset -F knowifi_redirect_client");

    nvramWriteCommit(); /*save*/
    websRedirect(wp, "learnConfig.asp");
}
#endif
#endif
