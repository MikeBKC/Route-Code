
/*
 * �ļ���:
 *     wusWebAUth.c
 *
 * ����:
 *    ��ҳ��֤�����ļ�
 *    
 * ���ߣ�
 *     �｣��
 *
 * ά���ˣ�
 *     �｣��
 *
 * �����
 *     1. ��ʼ�汾:2012-04-13
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
 * ��̬������ʾ��Ϣ
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
 * ������ websDefineWebAuth
 * ���ܣ� ҳ���뽻��ת������
 * ������ 2012-04-13
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
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
 * �� �� ����   getWhiteList
 * �������ڣ�	2014-7-15
 * �޸����ڣ�
 * ���ߣ�       hww
 * ����˵����	��
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
 * �� �� ����   websWhiteList
 * �������ڣ�	2014-7-16
 * �޸����ڣ�
 * ���ߣ�       hww
 * ����˵����	��
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
 * �� �� ����   formWhiteListDel
 * �������ڣ�	2014-7-16
 * �޸����ڣ�
 * ���ߣ�       hww
 * ����˵����	��
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
    if(!strcmp(PolicyName,"")) {				    /*�մ�����*/
        websRedirect(wp, "WebAuthGlobal.asp");
        return;
    }
    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
	prof = (WhiteListProfile *)ProfGetInstPointByIndex(mibType, index);
	if(!strcmp(prof->domain, PolicyName))			    /*������������λ��*/
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
* �� �� ����   formWhiteListDelAll
* �������ڣ�	2014-7-16
* �޸����ڣ�
* ���ߣ�	hww
* ����˵����	��
***********************************************************************/
static void formWhiteListDelAll(webs_t wp, char_t *path, char_t *query)
{
   int min, max, index;
   MibProfileType mibType = MIB_PROF_WHITE_LIST;
   WhiteListProfile *prof = NULL;
   ProfChangeType profAction = PROF_CHANGE_DELALL;
   struProfAlloc *profhead = NULL;

   ProfBackupByIndex(mibType, profAction, 0, &profhead);/*����delall��Ϣ*/
   ProfInstLowHigh(mibType, &max, &min);
   for (index = min; index < max; index++)
   {
	   ProfDelInstByIndex(mibType, index);/*ɾ������*/
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
 * wifidog ����ͨ������
 * ����Ƿ����wifidogͨ��ҳ��
 * �ǣ��������ͨ�����ͨ���¼����ɾ��
 * ������Ʒ����
 */
int isWifiNoticeWeb(webs_t wp) {
    int ret = 0;
    char_t *relIp = NULL;
    /*
     * ��ȡ����ͨ��ҳ��Ŀͻ���ip
     **/
    relIp = wp->ipaddr;
    if (wp->path != NULL)
    {
	if(strncmp(wp->path, "/"WIFI_NOTICE_HTML, (sizeof(WIFI_NOTICE_HTML)+1)) == 0) { /* wifidog����ͨ�� */
		/*���record��
		 *��ո������ʹ�������汾��ͨ����û��ӳ�ͨ��
		 *�����Լ����ڴ����ġ�ͬʱ����webserverѹ��*/
		//ipsetFlushGrp(NOTICE_R_GRP);
		ipsetAOrDIpStr(WIFI_IPSET_NOTICE_RECORD_GROUP, relIp, IPSET_MEM_DEL); /*������ͨ�棬��ͨ���¼����ɾ������ֳ�´��ظ�ͨ��*/
		ipsetAOrDIpStr(WIFI_IPSET_NOTICE_GROUP, relIp, IPSET_MEM_DEL);   /*��ͨ������ɾ������ֹ�´��ظ�ƥ��http�����ظ�ͨ��*/
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
 *  *������_formPictureLoad
 *   *���ܣ����ϴ���ͼƬд��flash��
 *    *������2014-01-13
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

    postData[fnlen] = '\0';/*ȥ���������õ��ַ�*/

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
	setTheErrorMsg(T("δ��⵽���ô洢�豸"));
	return -1;
    }
#endif

    if(strcmp(picName, bn) != 0)
	strcpy(picName, bn);

    return  0;
}

#if (WEBAUTH_AUTO == FYES)
/*****************************************
 *  *������formPictureLoad
 *   *���ܣ����ϴ���ͼƬд��flash��
 *    *������2013-01-07
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
*���ܣ��ж�ҳ���ļ��Ƿ����
*�������ļ��ľ���·��
*���أ�0---������
*      1---����
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
*���ܣ��ж�ͼƬ�ļ��Ƿ����
*�������ļ��ľ���·��
*���أ�0---������
*      1---����
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
 *  *������formPictureLoad
 *   *���ܣ����ϴ���ͼƬд��flash��
 *    *������2013-01-07
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
    uttSemP(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
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
	    prof->enpicture = 1;/*ѡ�м�������ѡ���еĵ���ͼƬ*/
	}else{
	    prof->enpicture = 3;/*ѡ�и߼�����ѡ���еĵ���ͼƬ*/
	}
    }
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemV(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
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
 *������getWebAuthTips
 *���ܣ����ͼƬ·������ʾ��Ϣ
 *������2013-01-07
 *�����������̶��������
 *���أ���
 *�������
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
 *����:�ӷ����ж����û��ϴ���web��֤ҳ��
 *����:outPutUserPage
 *����:��
 *���:��
 *����:ʵ�ʶ������ֽ���
 *����:20130618
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
 *������formDownloadPage
 *���ܣ�������֤ҳ��
 *�����������̶��������
 *���أ���
 *�������
 *������2013-06-08
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
	/*����*/
	wimDownloadFile(wp,webpagename,pageBuf,strlen(pageBuf));
	return;
}
/*******************************************************
 *������formActiveDefPage
 *���ܣ�����Ĭ��web��֤ҳ��
 *�����������̶��������
 *���أ���
 *�������
 *������20130608
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
        uttSemP(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
	prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0))
	{
		ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
		prof->enDefPage = 1;/*����Ĭ����֤ҳ��*/
		if(strlen(prof->pageName) > 4)
		{
			strcat(webPath,prof->pageName);
			doSystem("rm -f %s",webPath);
		}
		memset(prof->pageName,'\0',sizeof(prof->pageName));
	}
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemV(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit(); /*save*/

	doSystem("cp -rf /etc_ro/web/WebAuth_auth_def.asp /etc_ro/web/WebAuth_auth.asp");
	websRedirect(wp, "WebAuthGlobal.asp");
	return;
}
/*******************************************************
 *������effectUserPage
 *���ܣ�����ff�е��û��ϴ�����֤ҳ�濽�����ڴ���
 *��������
 *���أ���
 *�������
 *������20130608
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
 *������formImportPage
 *���ܣ����û��ϴ�����֤ҳ��д�뵽���������滻��ǰ����֤ҳ��
 *�����������̶��������
 *�������
 *���أ���
 *������20130608
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

    postData[fnlen] = '\0';/*ȥ���������õ��ַ�*/

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
        uttSemP(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
	prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0))
	{
		ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
		if (prof->enpicture == 1 || prof->enpicture == 3)
		{
			prof->enpicture = 3;/*ѡ�и߼�����ѡ���еĵ���ͼƬ*/
		}else{
			prof->enpicture = 2;/*ѡ�и߼�����ѡ���е�ͼƬ����*/
		}
		prof->enDefPage = 0;/*������Ĭ��web��֤ҳ��*/
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
        uttSemV(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit(); /*save*/

	effectUserPage();/*���û��ϴ���ҳ�濽�����ڴ���*/
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
 * ������ IsWebAuthOpen
 * ���ܣ� �ж�WEB��֤�Ƿ���
 * ������ 2013-03-07
 * ������ ��
 * ���أ� 
 * 1	-   ����
 * 0	-   δ����
 * ����� ��
 * ���ߣ� Jianqing.Sun 
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
 * ������ IsDnsNoticeOpen
 * ���ܣ� �ж��������˺���������ͨ���Ƿ���
 * ������ 2013-03-06
 * ������ ��
 * ���أ� 
 * 1	-   ������
 * 0	-   ��������һ��δ����
 * ����� ��
 * ���ߣ� Jianqing.Sun 
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
 * ������ isDDNSDomainName
 * ���ܣ� �ж�hostֵ�ǲ���·����֧�ֵ�ddns����
 * ������ 2013-03-06
 * ������ 
	host	-	��Ҫ�жϵ�hostֵ
 * ���أ� 
 *	1	-	��֧�ֵ�ddns����
 *	0	-	����֧�ֵ�ddns����
 * ����� ��
 * ���ߣ� Jianqing.Sun 
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
 * ������ redirectWAPage
 * ���ܣ��жϱ��η����Ƿ�����ת����ҳ��֤ҳ��
 * ������ 2012-04-13
 * ������ 
 * wp	-   ����goahead�û���Ϣ�ṹ��
 * ���أ� 
 * 1	-   ������ҳ��֤ҳ��
 * 0	-   ���ʷ���ҳ��֤ҳ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
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

    //��ģʽ�²���Ҫ��֤ 

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
    /*����web��֤Ҫ�ж� host �Ƿ�Ϊ lan��ip��ַ
     * ����Ϊ ����web��֤ʱ pc��Դip����¼����SYS_WEB_AUTH_WEB ��
     * ���� �������ֺ�  ���ֱ��� http����Ƿ�����ΪѸ�׷���post����
     * Ȼ��Ͳ��ᴥ����֤ ҳ�����Ӧ
     * ��ô��ʱ �� pc����webҳ�� �Ϳ����޷����� �������� ��֤ҳ��*/
    if ((isIp == 0) || (ipsetTestIpStr(SYS_LAN_GROUP, wp->httpHeadHost) == 0))
#else
    if ((isIp == 0) || (strcmp(wp->httpHeadHost, lanIp) != 0))
#endif
    {
#if (WEB_AUTH == FYES)
	/*ֻ��WEB��֤�����ҵ�ַ�ڼ�¼���в���Ҫ������֤ҳ��*/
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
	    if (IsDnsNoticeOpen() == 0) {/*�ж��������˺���������ͨ���Ƿ�ͬʱ����*/
		return ret;
	    } 

#if (DDNS == FYES)
	    /*
	     * ���host��·����֧�ֵ�ddns�������򷵻�
	     */
	    if (isDDNSDomainName(wp->httpHeadHost) == 1) {
		    return ret;
	    }
#endif
	    isIp = inet_aton(wp->httpHeadHost, &addr);

	    if(isIp == 0)
	    {/*��������ͨ�� host�϶���������ip
	       ��Ӧע�����ǰ� ���ʵ�web�� ����host ��ip ���͵� http����
	       ������ ͨ��
	       ���� �� ������һ������  ��Ӧ���ǵ��豸ip��ַ
	       ���ڷ��� ���������ʱ��ͻᴥ����������ͨ��*/
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
 * ������ isWebAuthWeb
 * ���ܣ� �жϷ��ʵ�ҳ���Ƿ���ҳ��֤ҳ�棬�����ݲ�ͬҳ������ش���
 * ������ 2012-04-13
 * ������ 
 * wp	-   ����goahead�û���Ϣ�ṹ��
 * ���أ� 
 * 1	-   ������ҳ��֤ҳ��
 * 0	-   ���ʷ���ҳ��֤ҳ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
extern int isWebAuthWeb(webs_t wp) 
{
	int ret = 1;
	if (wp->path != NULL)
	{
		/*��ҳ��֤ҳ��*/
		if(strcmp(wp->path, "/WebAuth_auth.asp") == 0) 
		{ 
#if 0
			if (ipsetTestIpStr(SYS_WEB_AUTH_WEB, wp->ipaddr) == 1)
			{
				/*ɾ������֤��¼ */
				ipsetAOrDIpStr(SYS_WEB_AUTH_WEB, wp->ipaddr, IPSET_MEM_DEL);
			}
#endif
			ret = 1;
		}
		else 
		    if(strcmp(wp->path, "/WebAuth_message.asp") == 0) 
		{
			/*��ҳ��֤ע��ҳ��*/
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
 * ������ _waCancelByIp
 * ���ܣ� �û�ע�������������û�IP��ַע���˺�
 * ������ 2014-01-16
 * ������ ip - 	�û�IP��ַ
 * ���أ� ��
 * ����� ��
 * ���ߣ� ���Ĳ� 
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
			/*����IP���Ҷ�Ӧ�˺�*/
			if(prof->ip == ip)
			{
				/*����IP��OK�������*/
				ipsetAOrDIp(SYS_WEB_AUTH_OK,ip,IPSET_MEM_DEL); 
#if (IP_GRP == FYES)
				addr.s_addr = prof->ip;
				SWORD_PRINTF("%s-----%d, inet_ntoa(addr) is %s\n", __func__, __LINE__,  inet_ntoa(addr));
				userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
				uttNfNlHashDelIp(uttNfNlSock, prof->ip);/*ɾ��hash���Ӧ��ip*/
#endif
				break;
			}
#endif /*WEB_AUTH_SHARE*/
		}
		
	}
#if (WEB_AUTH_SHARE == FYES)
#else
	/*�ҵ�*/
	if(i != max) 
	{
		/*���IP*/
		prof->ip = 0U;
	}
#endif
}
/********************************************************************
 * ������ waCancelByIp
 * ���ܣ� �û�ע�������������û�IP��ַע���˺�
 * ������ 2012-04-17
 * ������ ip - 	�û�IP��ַ
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
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
 * ������ searchDelSessionByIp
 * ���ܣ� �����û�IP��ַ�����˺ŻỰ��Ϣ�����ҵ���ע���Ự
 * ������ 2013-09-24
 * ������ ip - 	�û�IP��ַ
 * ���أ� 
 *	  1  -  �鵽�Ự��ע��
 *	  0  -  δ���ҵ�
 * ����� ��
 * ���ߣ� guo.deyuan
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
		/*����IP���Ҷ�Ӧ�˺�*/
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
	/*����IP��OK�������*/
	ipsetAOrDIp(SYS_WEB_AUTH_OK,ip,IPSET_MEM_DEL); 
#if (IP_GRP == FYES)
	addr.s_addr = ip;
#if DEBUG
	printf("%s-%d: prof->user =%s find and del inet_ntoa(addr) =%s\n",__func__,__LINE__,prof->user,inet_ntoa(addr));
#endif
	userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
	uttNfNlHashDelIp(uttNfNlSock, ip);/*ɾ��hash���Ӧ��ip*/
#endif
	}
    }
    return flag;
}
/********************************************************************
 * ������ searchSessionByIp
 * ���ܣ� �����û�IP��ַ�����˺ŻỰ��Ϣ
 * ������ 2013-09-24
 * ������ ip - 	�û�IP��ַ
 * ���أ� 
 *	  1  -  �鵽�Ự
 *	  0  -  δ���ҵ�
 * ����� ��
 * ���ߣ� guo.deyuan
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
 * ������ getActualSession
 * ���ܣ� ��ȡ�����˺ŵ�ʵ�ʻỰ����
 * ������ 2013-09-24
 * ������ null
 * ���أ� �ܻỰ�� 
 * ����� ��
 * ���ߣ� guo.deyuan
 ********************************************************************/ 
static int getActualSession()
{
	MibProfileType mibType = MIB_PROF_WEB_AUTH;
	WebAuthProfile *prof = NULL;
	int min = 0, max = 0;
	int i = 0;
	int count = 0;
	
	/* ����˺�profile*/
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
 * ������ checkSessionNum
 * ���ܣ� ����ʱ����˺ŻỰ���Ƿ�Ϸ�
 * ������ 2013-09-24
 * ������ 
 * ���أ� 
 * 1		�Ƿ�
 * 0		�Ϸ�
 * ����� ��
 * ���ߣ� guo.deyuan
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
 *  * ������ _getWebAuthConnListData
 *   * ���ܣ� ��ҳ��֤����״̬ҳ������˺������Ϣ
 *    * ������ 2014-01-08
 *     * ������ ��
 *      * ���أ� ��
 *       * ����� ��
 *        * ���ߣ� ���Ĳ�
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
		    websWrite(wp,"WebAuthNames[%d] = \"΢���û�\";",*num);
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
 * ������ getWebAuthConnListData
 * ���ܣ� ��ҳ��֤����״̬ҳ����������Ϣ
 * ������ 2013-09-24
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� guo.deyuan
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
    uttSemP(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
    _getWebAuthConnListData(wp, mibType, &num);
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
    _getWebAuthConnListData(wp, mibTypeC, &num);
#endif
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemV(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
    /*�Ḳ�Ǵ�����Ϣ����Ϊ��ҳ��������
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
 * ������ formWebAuthHangUpByIP
 * ���ܣ� �Ҷ�ָ���˺�IP
 * ������ 2013-11-28
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� guo.deyuan
 ********************************************************************/ 
static void formWebAuthHangUpByIP(webs_t wp, char_t *path, char_t *query)
{
	char_t *ipStr = NULL; 
	const char *ErrMsg = NULL;

	a_assert(wp);

	/*
	 * ɾ��ָ��IP
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
	     *  �����֤
	     */
	    waCancelByIp(ipAddress);
	}
#endif

	websRedirect(wp, "WebAuthStatus.asp");
	return;
}

/********************************************************************
 * ������ formWebAuthHangUpListByIP
 * ���ܣ� ҳ��Ҷϰ�ť���ú���
 * ������ 2013-11-28
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� guo.deyuan
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
	 * ɾ������ѡ���PROFILE
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
	    uttSemP(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
	    while(tmp != NULL) 
	    {
#if DEBUG
		printf("%s-%d, tmp = %s\n", __func__, __LINE__, tmp);
#endif
#if 1
		ipAddress = inet_addr((const char *)tmp);
		/*
		 *  �����֤
		 */
		waCancelByIp(ipAddress);
#endif

		tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
	    }

#if (WEBAUTH_ACCOUNT == FYES)
	    uttSemV(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
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
		    websWrite(wp,"WebAuthNames[%d] = \"΢���û�\";",*num);
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
 * ������ waUserAuth
 * ���ܣ� �û���֤����
 * ������ 2012-04-17
 * ������ ������������
 * ���أ� 
 * 1		��֤�ɹ�
 * 0		��֤ʧ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
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
	/* ��ȡ����ʱ�䣬����ϵͳʱ�� */
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
	 *  ����û���������
	 */  
	userName = websGetVar(wp, T("userName"), T(""));
	userPasswd = websGetVar(wp, T("userPasswd"), T(""));
	SWORD_PRINTF("%s %d, userName = %s, userPasswd = %s\n",
			__FUNCTION__, __LINE__, userName, userPasswd);

	/* �˺ţ����벻�Ϸ�*/
	if ((strlen(userName) > UTT_USER_LEN) || (strlen(userPasswd) > UTT_PASSWD_LEN)) 
	{
		flag = 0;
	}
	/* ����˺�profile*/
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
				/*��������֤����ֱ�ӷ�����֤�ɹ���Ϣ*/
				MsgStr = getTransDataForC(C_LANG_INDEX_SUCCESS_IDENTITY);
#if (WEBAUTH_AUTO == FYES)
				websWrite(wp,"var errorstr=\"%s\";", MsgStr);
				websDone(wp, 200);/*ajaxβ*/
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
				websDone(wp, 200);/*ajaxβ*/
#else
				webAuthSetMsg(MsgStr);
				websRedirect(wp, "WebAuth_auth.asp");
#endif
				return 0;    
			    } else {
				/*����˺����Ự��Ϊ1
				 * ��δ�����˺Ź���
				 * �����������滻��ǰ�Ự*/
				if(prof->maxSession == 1U){
#if DEBUG
				    printf("%s-%d: prof->maxSession =%d, prof->actualSession =%d,prof->sesIndex =%d\n",
					    __func__,__LINE__,prof->maxSession,prof->actualSession,prof->sesIndex);
#endif
				    /*�˺��ѱ�ʹ��*/
				    if( prof->actualSession == 1U) {
#if DEBUG
					printf("%s-%d: profile account used.\n",__func__,__LINE__);
#endif
					/*��prof->ip��OK��ַ����ɾ��*/
					if(checkSessionIndex(prof->sesIndex) == 1) {
					webAuthLog(prof, nvramProfile->session[prof->sesIndex].ip, nvramProfile->session[prof->sesIndex].mac);
					ipsetAOrDIp(SYS_WEB_AUTH_OK, nvramProfile->session[prof->sesIndex].ip, IPSET_MEM_DEL);
#if (IP_GRP == FYES)
					addr.s_addr = nvramProfile->session[prof->sesIndex].ip;
					SWORD_PRINTF("%s-----%d, inet_ntoa(addr) is %s\n", __func__, __LINE__,  inet_ntoa(addr));
					userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif

#ifdef CONFIG_UTT_NETFILTER
					uttNfNlHashDelIp(uttNfNlSock, nvramProfile->session[prof->sesIndex].ip);/*ɾ��hash���Ӧ��ip*/
#endif
					sesindex = prof->sesIndex;
					}
					//free(prof->sesHead); /*�ͷŻỰָ��*/
				    } else { /*�˺�δ��ʹ�ã����½��Ự*/
					if(sesflag == 2) {
					    MsgStr = getTransDataForC(C_LANG_INDEX_MAX_SYS_SESSION);
#if (WEBAUTH_AUTO == FYES)
					    websWrite(wp,"var errorstr=\"%s\";", MsgStr);
					    websDone(wp, 200);/*ajaxβ*/
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
				} else { /*�˺����Ự������1���������˺Ź���*/
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
				    websDone(wp, 200);/*ajaxβ*/
#else
				    webAuthSetMsg(MsgStr);
				    websRedirect(wp, "WebAuth_message.asp");
#endif
				    return 0;    
				}
			    }
#else /*WEB_AUTH_SHARE*/
				/*�˺��ѱ�ʹ��*/
				if( prof->ip != 0U) 
				{
					webAuthLog(prof, prof->ip, prof->mac);
					/*��prof->ip��OK��ַ����ɾ��*/
					ipsetAOrDIp(SYS_WEB_AUTH_OK, prof->ip, IPSET_MEM_DEL);
#if (IP_GRP == FYES)
					addr.s_addr = prof->ip;
					SWORD_PRINTF("%s-----%d, inet_ntoa(addr) is %s\n", __func__, __LINE__,  inet_ntoa(addr));
					userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif

#ifdef CONFIG_UTT_NETFILTER
					uttNfNlHashDelIp(uttNfNlSock, prof->ip);/*ɾ��hash���Ӧ��ip*/
#endif
				}
#endif /*WEB_AUTH_SHARE*/
				break;
			}
		}
	}
	SWORD_PRINTF("%s-%d, i = %d\n", __func__, __LINE__, i);
	if(i != max)/*�ҵ���Ӧ�û���������*/
	{
#if (WEBAUTH_ACCOUNT == FYES)
	    tmCurrent = time(NULL);
	    if(((prof->timeStart > tmCurrent) || (tmCurrent > prof->timeStop)) && (prof->timeingPatternEn == FUN_ENABLE) && ((tmCurrent < DELIVERY_START_TIME) || (tmCurrent > DELIVERY_END_TIME)))
	    {
		webauth_acc_debug_print("%s--%d:This account has expired\n", __func__, __LINE__);
		MsgStr = getTransDataForC(C_LANG_INDEX_WEBAUTH_ACC_EXPIRED);
#if (WEBAUTH_AUTO == FYES)
				    websWrite(wp,"var errorstr=\"%s\";", MsgStr);
				    websDone(wp, 200);/*ajaxβ*/
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
		websDone(wp, 200);/*ajaxβ*/
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
			/*���ӵ�����֤*/
			ipsetAOrDIpStr(SYS_WEB_AUTH_OK, wp->ipaddr, IPSET_MEM_ADD); 
		}
#if (IP_GRP == FYES)
		SWORD_PRINTF("%s-----%d, wp->ipaddr is %s\n", __func__, __LINE__,  wp->ipaddr);
		userUpToIpGrp(userName, wp->ipaddr, IPENT_WEB_AUTH, IPSET_MEM_ADD);
#endif
		ip = inet_addr((const char *)(wp->ipaddr));/*�ַ���ip��ַת��Ϊ����*/
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
		/*����ʱ���ܱ��˻�dhcp�����Ӧ�Ĳ��ԵȻ���
		 * ��Ҫɾ��һ��*/
		uttNfNlHashDelIp(uttNfNlSock, ip);/*ɾ��hash���Ӧ��ip*/
#endif
#if (WEBAUTH_ACCOUNT == FYES)
		((struct sockaddr_in*)&arpLan.arp_pa)->sin_addr.s_addr = prof->ip;
		memset(arpLan.arp_ha.sa_data, 0, sizeof(arpLan.arp_ha.sa_data));
		mac = getMacByIpIoctl(&arpLan);/*ͨ��arp���ѯ��ǰip�Ķ�Ӧmac��ַ*/
		if (mac != NULL)/*�鵽�ˣ����浽�ṹ����*/
		{
#if (WEB_AUTH_SHARE == FYES)
#if DEBUG
		    printf("%s-%d: mac addr find.\n",__func__,__LINE__);
#endif
		    memcpy(nvramProfile->session[sesindex].mac, mac, 6);
		   // nvramProfile->session[sesindex].compareMacEn = FUN_ENABLE;/*��mac��ַ�Ƚϱ�־����ѭ���Ʒ�ʱ��Ҫ�ж�mac��ַ*/
#else /*WEB_AUTH_SHARE*/
		    memcpy(prof->mac, mac, 6);
		    //prof->compareMacEn = FUN_ENABLE;/*��mac��ַ�Ƚϱ�־����ѭ���Ʒ�ʱ��Ҫ�ж�mac��ַ*/
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
		prof->timeLast = (unsigned long)sysInfo.uptime;/*��ȡ��������ǰ��ʱ�䣬������֤�ɹ�ʱ��*/
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
		websDone(wp, 200);/*ajaxβ*/
#else
		webAuthSetMsg(MsgStr);
		websRedirect(wp, "WebAuth_auth.asp");
#endif
		return 0;
	}
#if (WEBAUTH_AUTO == FYES)
	websWrite(wp,"var errorstr=\"%s\";", MsgStr);
	websDone(wp, 200);/*ajaxβ*/
#else
	webAuthSetMsg(MsgStr);
	websRedirect(wp, "WebAuth_message.asp");
#endif
	return flag;
}

#if (WEBAUTH_AUTO == FYES)

/********************************************************************
 *  * ������ waCaptchaAuth
 *   * ���ܣ� ��֤����֤����
 *    * ������ 2014-01-02
 *     * ������ ������������
 *      * ���أ� 
 *       * 1	    ��֤�ɹ�
 *        * 0	    ��֤ʧ��
 *         * ����� ��
 *          * ���ߣ� li.wenchang 
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
    /* ��ȡ����ʱ�䣬����ϵͳʱ�� */
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
     *	 *  �����֤��
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

    /* ��֤�벻�Ϸ�(���Ȳ�Ϊ6)*/
    if (strlen(captcha) != 4) 
    {
	SWORD_PRINTF("%s %d, strlen not 4\n", __func__, __LINE__);
	return C_LANG_INDEX_AUTHENTICATION_FAILURE;
    }

    SWORD_PRINTF("%s %d, check...\n", __func__, __LINE__);
    /* ����˺�profile*/
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
		    /*��������֤����ֱ�ӷ�����֤�ɹ���Ϣ*/
		    return C_LANG_INDEX_SUCCESS_IDENTITY;    
		}
		if(((sesflag = checkSessionNum(prof))>0) && (prof->maxSession != 1)) {
		    if ((sesflag == 1) || (sesflag == 2))
			return C_LANG_INDEX_MAX_ACC_SESSION;
		} else {
		    /*����˺����Ự��Ϊ1
		     *��δ�����˺Ź���
		     *�����������滻��ǰ�Ự*/
		    if(prof->maxSession == 1U){
			/*�˺��ѱ�ʹ��*/
			if( prof->actualSession == 1U) {
			    /*��prof->ip��OK��ַ����ɾ��*/
			    if(checkSessionIndex(prof->sesIndex) == 1) {
				webAuthLog(prof, nvramProfile->session[prof->sesIndex].ip, nvramProfile->session[prof->sesIndex].mac);
				ipsetAOrDIp(SYS_WEB_AUTH_OK, nvramProfile->session[prof->sesIndex].ip, IPSET_MEM_DEL);
#if (IP_GRP == FYES)
				addr.s_addr = nvramProfile->session[prof->sesIndex].ip;
				userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif

#ifdef CONFIG_UTT_NETFILTER
				uttNfNlHashDelIp(uttNfNlSock, nvramProfile->session[prof->sesIndex].ip);/*ɾ��hash���Ӧ��ip*/
#endif
				sesindex = prof->sesIndex;
			    }
			} else { /*�˺�δ��ʹ�ã����½��Ự*/
			    sesindex = searchFreeSessionIndex();
			}
		    } else { /*�˺����Ự������1���������˺Ź���*/
			sesindex = searchFreeSessionIndex();
		    }
		    if(checkSessionIndex(sesindex) != 1) 
			return C_LANG_INDEX_APPLY_SES_ERR;    
		}
#else /*WEB_AUTH_SHARE*/
		/*�˺��ѱ�ʹ��*/
		if( prof->ip != 0U) 
		{
		    webAuthLog(prof, prof->ip, prof->mac);
		    /*��prof->ip��OK��ַ����ɾ��*/
		    ipsetAOrDIp(SYS_WEB_AUTH_OK, prof->ip, IPSET_MEM_DEL);
#if (IP_GRP == FYES)
		    addr.s_addr = prof->ip;
		    userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif

#ifdef CONFIG_UTT_NETFILTER
		    uttNfNlHashDelIp(uttNfNlSock, prof->ip);/*ɾ��hash���Ӧ��ip*/
#endif
		}
#endif /*WEB_AUTH_SHARE*/
		break;
	    }
	}
    }

    if(i != max)/*�ҵ���Ӧ�û���������*/
    {
#if (WEBAUTH_ACCOUNT == FYES)
	tmCurrent = time(NULL);
	if(((prof->timeStart > tmCurrent) || (tmCurrent > prof->timeStop)) && (prof->timeingPatternEn == FUN_ENABLE) && ((tmCurrent < DELIVERY_START_TIME) || (tmCurrent > DELIVERY_END_TIME)))
	{
	    /* ���˺Ų�����Ч��ʱ�䷶Χ�� */
	    return C_LANG_INDEX_WEBAUTH_ACC_EXPIRED;    
	}
	else if ((prof->timeTotal != 0U) && (prof->timeUsed >= prof->timeTotal) && (prof->timeingPatternEn == FUN_ENABLE))
	{
	    /* ���˺ſ���ʱ�������� */
	    return C_LANG_INDEX_WEBAUTH_NO_TIME;
	}
	else
#endif
	{
	    if (ipsetTestIpStr(SYS_WEB_AUTH_OK, wp->ipaddr) == 0)
	    { 
		/*���ӵ�����֤*/
		ipsetAOrDIpStr(SYS_WEB_AUTH_OK, wp->ipaddr, IPSET_MEM_ADD); 
	    }
#if (IP_GRP == FYES)
	    userUpToIpGrp(prof->user, wp->ipaddr, IPENT_WEB_AUTH, IPSET_MEM_ADD);
#endif
	    ip = inet_addr((const char *)(wp->ipaddr));/*�ַ���ip��ַת��Ϊ����*/
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
	    /*����ʱ���ܱ��˻�dhcp�����Ӧ�Ĳ��ԵȻ���
	     *		 * ��Ҫɾ��һ��*/
	    uttNfNlHashDelIp(uttNfNlSock, ip);/*ɾ��hash���Ӧ��ip*/
#endif
#if (WEBAUTH_ACCOUNT == FYES)
	    ((struct sockaddr_in*)&arpLan.arp_pa)->sin_addr.s_addr = prof->ip;
	    memset(arpLan.arp_ha.sa_data, 0, sizeof(arpLan.arp_ha.sa_data));
	    mac = getMacByIpIoctl(&arpLan);/*ͨ��arp���ѯ��ǰip�Ķ�Ӧmac��ַ*/
	    if (mac != NULL)/*�鵽�ˣ����浽�ṹ����*/
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
	    prof->timeLast = (unsigned long)sysInfo.uptime;/*��ȡ��������ǰ��ʱ�䣬������֤�ɹ�ʱ��*/
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

	    /* ��֤�ɹ� */
	    return C_LANG_INDEX_SUCCESS_IDENTITY;
	}
    }
    else
    {
	/* δ�ҵ���Ӧ���û������� */
	return C_LANG_INDEX_AUTHENTICATION_FAILURE;
    }

    /* ��ʵ�����߲������� */
    return C_LANG_INDEX_SUCCESS_IDENTITY;
}
#endif
/********************************************************************
 * ������ _waCancelByUser
 * ���ܣ� �Ҷϣ�ɾ�����ú���������ʵ����ע��web��֤
 * ������ 2014-01-16
 * ������ user  -  ʵ����
 * ���أ� ��
 * ����� ��
 * ���ߣ� ���Ĳ� 
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
			/*����user��ö�Ӧprofile*/
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
					/*����IP��OK��ַ��ɾ��*/
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
					uttNfNlHashDelIp(uttNfNlSock, nvramProfile->session[j].ip);/*ɾ��hash���Ӧ��ip*/
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
				    /*����IP��OK��ַ��ɾ��*/
				    printf("%s-%d: i =%d, session->ip =0x%x\n",__func__,__LINE__,i,session->ip);
				    ipsetAOrDIp(SYS_WEB_AUTH_OK, session->ip, IPSET_MEM_DEL);
#if (IP_GRP == FYES)
				    addr.s_addr = session->ip;
				    SWORD_PRINTF("%s-%d: inet_ntoa(addr) =%s\n",__func__,__LINE__,inet_ntoa(addr));
				    userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
				    uttNfNlHashDelIp(uttNfNlSock, session->ip);/*ɾ��hash���Ӧ��ip*/
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
				/*����IP��OK��ַ��ɾ��*/
				SWORD_PRINTF("%s-%d, i = %d, prof->ip is %x\n", __func__, __LINE__, i,  prof->ip);
				webAuthLog(prof, prof->ip, prof->mac);
				ipsetAOrDIp(SYS_WEB_AUTH_OK, prof->ip, IPSET_MEM_DEL);
#if (IP_GRP == FYES)
				addr.s_addr = prof->ip;
				SWORD_PRINTF("%s-----%d, inet_ntoa(addr) is %s\n", __func__, __LINE__,  inet_ntoa(addr));
				userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
				uttNfNlHashDelIp(uttNfNlSock, prof->ip);/*ɾ��hash���Ӧ��ip*/
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
 * ������ waCancelByUser
 * ���ܣ� �Ҷϣ�ɾ�����ú���������ʵ����ע��web��֤
 * ������ 2012-04-17
 * ������ user  -  ʵ����
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
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
  * ��ʱ���ַ���(2.5)ת����int������������ʱ��
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
 * ������ WebAuthConfigAdd
 * ���ܣ� ��ҳ��֤����Ա�����˺�
 * ������ 2012-04-13
 * ������ �����̶��������
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
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
	char_t *timingModeEn = NULL;/*��ʱģʽ*/
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

	/*��ҳ��ռ��ȡֵ*/
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
        uttSemP(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
	ProfInstLowHigh(mibType, &max, &min);
	for(i = min;i < max;i++)
	{
		prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
		if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
		{
			if(strncmp(prof->user,userName,UTT_USER_LEN) == 0)/*�鿴�û����Ƿ��Ѵ���*/
			{
				ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_ALREADY_EXITS);
				setTheErrorMsgOnFree(ErrMsg);
				break;
			}
		}
	}
	SWORD_PRINTF("%s-%d, i = %d\n", __func__, __LINE__, i);
	if(i == max)/*û��ͬ�����û���*/
	{
		webIndexInst = getWebInstName(&(nvramProfile->webauthInstRec.instRecMap), &(nvramProfile->webauthInstRec.instNameIndex));
		SWORD_PRINTF("%s-%d, webIndexInst = %d\n", __func__, __LINE__, webIndexInst);
		/* portmap���� */
		if (webIndexInst == 0) 
		{
			ErrMsg = getTransDataForC(C_LANG_INDEX_OPERATION_FAIL);
			setTheErrorMsg(ErrMsg);
			goto out;
		}
		memset(instId, 0, sizeof(instId));
		snprintf(instId, UTT_INST_NAME_LEN, "ID%d", webIndexInst);
		SWORD_PRINTF("%s-%d, instId = %s\n", __func__, __LINE__, instId);
		/* �Ѵ���ʵ���� */
		if (ProfGetInstIndexByName(mibType, instId) != PROFFAIL) 
		{
			ErrMsg = getTransDataForC(C_LANG_INDEX_CASE_ALREADY_EXITS);
			setTheErrorMsg(ErrMsg);
			goto out;
		}
		instIndex = ProfNewInst(mibType, instId, FALSE);/*��ȡʵ���Ų�д��ʵ����*/
		SWORD_PRINTF("%s-%d, instIndex = %d\n", __func__, __LINE__, instIndex);
		if(instIndex == PROFFAIL)/*��ȡʧ��������Ѵ������Ŀ*/ 
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
				    /*��ʱ���ַ���(2.5)ת����int������������ʱ��*/
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

	ProfUpdate(profhead);/*����*/
	ProfFreeAllocList(profhead);
	nvramWriteCommit(); /*save*/
out:
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemV(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
	websRedirect(wp, "WebAuthServer.asp");
	return;
}
/********************************************************************
 * ������ WebAuthConfigModify
 * ���ܣ� ��ҳ��֤����Ա�޸��˺�
 * ������ 2012-04-13
 * ������ �����̶��������
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
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
	char_t *timingModeEn = NULL;/*��ʱģʽ*/
	char_t *timestartStr = NULL, *timeendStr = NULL;
	Uint32 starttm = 0U;
	Uint32 endtm = 0U;
	int profChange = 0;/*�Ƿ�ֻ����ʱ��ı䣬ֻ����ʱ��ı䲻�Ҷ�*/
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
        uttSemP(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
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
	{ /* �û����޸��� */
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
				    /*��ʱ���ַ���(2.5)ת����int������������ʱ��*/
#ifdef CONFIG_PPC_85xx
				    prof->timeTotal = timeStrToSecond(timeTotal);
#else
				    prof->timeTotal = ((unsigned long)(strtod(timeTotal, NULL) * 10) * 360U);
#endif
				    if (prof->timeTotal == 0U)/*��ʱ���޸�Ϊ0�������ʹ��ʱ��*/
				    {
					prof->timeUsed = 0U;/*��ʹ��ʱ��*/
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
				    prof->timeUsed = 0U;/*��ʹ��ʱ��*/

				}
#endif
				SWORD_PRINTF("%s-%d,user is %s,passwd is %s, remark is %s\n", __func__, __LINE__, prof->user, prof->passwd,remark);
#if (WEBAUTH_ACCOUNT == FYES)
				if (profChange == 1)/*������ʱ���������޸ģ��͹Ҷϵ�ǰ����*/
#endif
				{
				    waCancelByUser(prof->head.name);/*����ʵ����ע��*/
				}
			}
		}
	}

	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit(); /*save*/
out:
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemV(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
	websRedirect(wp, "WebAuthServer.asp");
	return;
}
/********************************************************************
 * ������ formPictureUrl
 * ���ܣ������û��ύ������ͼƬ����
 * ����ʱ�䣺20130607
 * �����������̶��������
 * ���أ���
 * �������
 * *****************************************************************/
static void formPictureUrl(webs_t wp, char_t *path, char_t *query)
{
	MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
	WebAuthGlobalProfile *prof = NULL;
	struProfAlloc *profhead = NULL;
	int i = 0;
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemP(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
	char *pictureUrls = NULL;
	pictureUrls = websGetVar(wp,T("importpictureurl"),T(""));
	prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0))
	{
		ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, i, &profhead);
		if(prof->enpicture < 2)
		{
			prof->enpicture = 0;/*ѡ�м�������ѡ���е���������*/
		}else{
			prof->enpicture = 2;/*ѡ�и߼�����ѡ���е���������*/
		}
		strcpy(prof->pictureUrl,pictureUrls);
		saveContactToFile(pictureUrls, WEB_AUTH_PICURL_PATH);
	}
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemV(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit(); /*save*/
	websRedirect(wp, "WebAuthGlobal.asp");
	return;
}

#if (WEBAUTH_AUTO == FYES)
/*������֤�ɹ�����Ҫ��ת������*/
static void formJumpUrl(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    int i = 0;
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemP(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
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
    uttSemV(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit(); /*save*/
    websRedirect(wp, "WebAuthGlobal.asp");
    return;
}
#endif

/********************************************************************
 * ������ formWebAuthConfig
 * ���ܣ� ��ҳ��֤����Ա�����û��������뽻��
 * ������ 2012-04-13
 * ������ �����̶��������
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
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
 *  * ������ webAuthCaptchaListDel 
 *   * ���ܣ� ��ҳ��֤����Աɾ������
 *    * ������ 2014-01-14
 *     * ������ �û��б�
 *      * ���أ� ��
 *       * ����� ��
 *        * ���ߣ� ���Ĳ�
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
	    waCancelByUser(tmp);/*����ʵ����ע��*/
	    ProfDelInstByName(mibType, tmp); 
	}
#else
	waCancelByUser(tmp);/*����ʵ����ע��*/
	ProfDelInstByName(mibType, tmp); 
#endif

	tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
    }

    return;
}

/********************************************************************
 *  * ������ formWebAuthCaptureListDelAll 
 *   * ���ܣ� ��ҳ��֤����Աɾ����������
 *    * ������ 2014-01-14
 *     * ������ �����̶��������
 *      * ���أ� ��
 *       * ����� ��
 *        * ���ߣ� ���Ĳ�
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
     *	 * ɾ������ѡ���PROFILE
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
		waCancelByUser(prof->head.name);/*����ʵ����ע��*/
		ProfDelInstByIndex(mibType, i);
	    }
#else
	    waCancelByUser(prof->head.name);/*����ʵ����ע��*/
	    ProfDelInstByIndex(mibType, i);
#endif
	}
    }

    /* ����map��¼ */
    clearAllPortMap(&(nvramProfile->webauthInstRec.instRecMap));
    nvramProfile->webauthInstRec.instNameIndex = 0;
}
#endif

/********************************************************************
 * ������ formWebAuthListDel 
 * ���ܣ� ��ҳ��֤����Աɾ������
 * ������ 2012-04-13
 * ������ �����̶��������
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
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
     * ɾ������ѡ���PROFILE
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
        uttSemP(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
		while(tmp != NULL) 
		{
			if(back == 0)/*���ڿ���ɾ�������ֻ��һ�α��ݲ���*/
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
				waCancelByUser(tmp);/*����ʵ����ע��*/
				ProfDelInstByName(mibType, tmp); 
			}
#else
			waCancelByUser(tmp);/*����ʵ����ע��*/
			ProfDelInstByName(mibType, tmp); 
#endif
			/* �����¼map */
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
	uttSemV(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
		nvramWriteCommit(); /*save*/
	}
	websRedirect(wp, "WebAuthServer.asp");

	return;	   
}
/********************************************************************
 * ������ formWebAuthListDelAll 
 * ���ܣ� ��ҳ��֤����Աɾ����������
 * ������ 2012-04-13
 * ������ �����̶��������
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
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
        uttSemP(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
#if (IP_GRP == FYES)
	memset(referedMsg, 0, sizeof(referedMsg));
#endif
	/*
	 * ɾ������ѡ���PROFILE
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
				waCancelByUser(prof->head.name);/*����ʵ����ע��*/
				ProfDelInstByIndex(mibType, i);
			}
#else
			waCancelByUser(prof->head.name);/*����ʵ����ע��*/
			ProfDelInstByIndex(mibType, i);
#endif
		}
	}

	/* ����map��¼ */
	clearAllPortMap(&(nvramProfile->webauthInstRec.instRecMap));
	nvramProfile->webauthInstRec.instNameIndex = 0;

#if (WEBAUTH_ACCOUNT == FYES)
        uttSemV(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit(); /*save*/
	websRedirect(wp, "WebAuthServer.asp");
}
/********************************************************************
 * ������ formWebAuthHangUp
 * ���ܣ� ҳ��Ҷ�ͨ����֤���û�������Աʹ��
 * ������ 2012-04-13
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static void formWebAuthHangUp(webs_t wp, char_t *path, char_t *query)
{
	char_t *UserName = NULL; 
	const char *ErrMsg = NULL;

	a_assert(wp);

	/*
	 * ɾ������ѡ���PROFILE
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
		waCancelByUser(UserName);/*����ʵ����ע��*/
	}

	websRedirect(wp, "WebAuthServer.asp");
	return;
}
/********************************************************************
 * ������ formSubmitCheck
 * ���ܣ� ����û��ύ������ע��������֤
 * ������ 2012-04-13
 * ������ 
 * - command            ����
 * ���أ� 
 * - TRUE               ע��
 * - FALSE              ��֤
 * ����� ��
 * ���ߣ� Jianqing.Sun 
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
 * ������ formWebAuthUserSubmit
 * ���ܣ� �����û��ύ
 * ������ 2012-04-13
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static void formWebAuthUserSubmit(webs_t wp, char *path, char *query) 
{
	char *command = NULL;

	/*
	 * ͨ��command����û���ע��������֤
	 */
	command = websGetVar(wp, T("userCommand"), T(""));
	SWORD_PRINTF("%s %d, command = %s\n", __FUNCTION__, __LINE__, command);

#if (WEBAUTH_ACCOUNT == FYES)
        uttSemP(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
	/*
	 * ע��
	 */
	if (formSubmitCheck(command) == TRUE)
	{
		IPADDR ipAddress;
		ipAddress = inet_addr((const char *)(wp->ipaddr));

		SWORD_PRINTF("%s %d, wp->ipaddr = %s, ipAddress = %x\n", __FUNCTION__, __LINE__, wp->ipaddr, ipAddress);

		/*
		 *  �����֤
		 */
		waCancelByIp(ipAddress);
		websRedirect(wp, "WebAuth_auth.asp");
	} 
	/*
	 * ��֤
	 */
	else 
	{
		waUserAuth(wp, path, query);
	}
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemV(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
	return;
}

#if (WEBAUTH_AUTO == FYES)
/********************************************************************
 *  * ������ formWebAuthSmsSubmit
 *   * ���ܣ� �����û��ύ
 *    * ������ 2014-01-02
 *     * ������ ��
 *      * ���أ� ��
 *       * ����� ��
 *        * ���ߣ� li.wenchang 
 *         ********************************************************************/ 
static void formWebAuthCaptchaSubmit(webs_t wp, char *path, char *query) 
{
    char *command = NULL;
    const char *MsgStr = NULL;
    int ret;

    /*
     *	 * ͨ��command����û���ע��������֤
     *	     */
    command = websGetVar(wp, T("userCommand"), T(""));
    SWORD_PRINTF("%s %d, command = %s\n", __FUNCTION__, __LINE__, command);

#if (WEBAUTH_ACCOUNT == FYES)
    uttSemP(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
    /*
     *	 * ע��
     *	     */
    if (formSubmitCheck(command) == TRUE)
    {
	IPADDR ipAddress;
	ipAddress = inet_addr((const char *)(wp->ipaddr));

	SWORD_PRINTF("%s %d, wp->ipaddr = %s, ipAddress = %x\n", __FUNCTION__, __LINE__, wp->ipaddr, ipAddress);

	/*
	 *	 *  �����֤
	 *		 */
	waCancelByIp(ipAddress);
	websRedirect(wp, "WebAuth_auth.asp");
    } 
    /*
     *   * ��֤
     *	 */
    else 
    {
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain;charset=GB2312\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	ret = waCaptchaAuth(wp, path, query);
	MsgStr = getTransDataForC(ret);
	websWrite(wp,"var errorstr=\"%s\";", MsgStr);
	websDone(wp, 200);/*ajaxβ*/
    }
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemV(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
    return;
}

/* ��������֤����֤ */
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
 * ������ formWebAuthGlobalConfig
 * ���ܣ� �ύ��ҳ��֤ȫ�ֲ���
 * ������ 2012-04-13
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static void formWebAuthGlobalConfig(webs_t wp, char_t *path, char_t *query)
{
	MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
	WebAuthGlobalProfile *prof = NULL;
	struProfAlloc *profhead = NULL;
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemP(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
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
	 * ȡֵ
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
		    sprintf(prof->sms.contents, "��֤����##��������Ч��Ϊ**����%s��", smsname);
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
		if(prof->enpicture<2)/*���Ǽ������ã���ʹ��Ĭ��ҳ��*/
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
		/*��ַ�����ĳ��Ȳ��ܴ���IP_GROUP_NAME_LEN*/
		strcpy(prof->exceptIpGroup,pexceptIpGroup);
		strcpy( prof->tipstitle, ptipstitle);
		strcpy( prof->tipsinfo, ptipsinfo);
		prof->head.active = TRUE;

	}

#if (WEBAUTH_ACCOUNT == FYES)
        uttSemV(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit(); /*save*/
	websRedirect(wp, "WebAuthGlobal.asp");
	return;
}
/********************************************************************
 * ������ getAddreGrpList
 * ���ܣ� ���������е�ַ����� 
 * ������ 2012-08-15
 * ������ ��
 * ���أ� ��
 * ����� �������еĵ�ַ��
 * ���ߣ� Jianqing.Sun 
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
    /*�������е�ַ��*/
    for(i = min; i < max; i++) 
    {
	prof = (IpGrpProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) 
	{
	    if(prof->grpType == IPGRP_IP)
	    {/*�������ǵ�ַ�����*/
		websWrite(wp,"addrGroupName[%d] = \"%s\";", num, prof->head.name);
		num++;
	    }
	}
    }
    return 0;
}
#endif


/********************************************************************
 *  * ������ _getWebAuthListData
 *   * ���ܣ� ��ҳ��֤����ҳ������˺������Ϣ
 *    * ������ 2014-01-08
 *     * ������ ��
 *      * ���أ� ��
 *       * ����� ��
 *        * ���ߣ� ���Ĳ� 
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
		websWrite(wp,"WebAuthNames[%d] = \"΢���û�\";", *num);
	    else
#endif
		websWrite(wp,"WebAuthNames[%d] = \"%s\";", *num, prof->user);

#if (WEBAUTH_ACCOUNT == FYES)
	    tmCurrent = time(NULL);/*��ȡ��ǰʱ��*/
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
		websWrite(wp,"authMethods[%d] = \"%s\";", *num, "�ֶ�");
	    else if (prof->autoAuthMethod == IDCARD_METHOD)
		websWrite(wp,"authMethods[%d] = \"%s\";", *num, "���֤");
	    else if (prof->autoAuthMethod == SMS_METHOD)
		websWrite(wp,"authMethods[%d] = \"%s\";", *num, "����");
	    else if (prof->autoAuthMethod == WECHAT_METHOD)
		websWrite(wp,"authMethods[%d] = \"%s\";", *num, "΢��");

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
 * ������ getWebAuthListData
 * ���ܣ� ��ҳ��֤����ҳ����������Ϣ
 * ������ 2012-04-13
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
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
        uttSemP(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
	_getWebAuthListData(wp, mibType, &num);
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
	_getWebAuthListData(wp, mibTypeC, &num);
#endif
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemV(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
/*�Ḳ�Ǵ�����Ϣ����Ϊ��ҳ��������
	getTheErrorMsg(eid, wp, argc, argv);
*/
	websWrite(wp,"var totalrecs=%d;",num);
	websWrite(wp,"var max_totalrecs =%d;",MAX_WEB_AUTH_PROFILES);
	return 0;
}
/********************************************************************
 * ������ _getWebAuthOneInfo
 * ���ܣ� ��ҳ��֤����ҳ�浥���û���������Ϣ
 * ������ 2014-01-27
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� ���Ĳ�
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
		    websWrite(wp,"usernames = \"΢���û�\";");
		else
#endif
		    websWrite(wp,"usernames = \"%s\";", prof->user);
		websWrite(wp, "authpasswds = \"%s\";", prof->passwd);
		websWrite(wp, "descriptions = \"%s\";", prof->remark);
		websWrite(wp, "sessionNum = \"%d\";", prof->maxSession);
#if (WEBAUTH_ACCOUNT == FYES)
		sprintf(timeTotal, "%.1f", prof->timeTotal / 3600.0);/*��ת����Сʱ*/
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
 * ������ getWebAuthOneInfo
 * ���ܣ� ��ҳ��֤����ҳ�浥���û���������Ϣ
 * ������ 2012-04-13
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
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
	if ((ejArgs(argc, argv, T("%s"), &editName) < 1) || (editName == NULL))/*�鿴ʵ�����Ƿ����*/ 
	{
		ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_READ_DATA_ERR);
		setTheErrorMsg(ErrMsg);
	}
	else
	{
#if (WEBAUTH_ACCOUNT == FYES)
		uttSemP(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
		ret = _getWebAuthOneInfo(wp, mibType, editName);
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
		if (ret == 0) {
		    mibType = MIB_PROF_WEB_AUTH_CPT;
		    ret = _getWebAuthOneInfo(wp, mibType, editName);
		}

#endif
#if (WEBAUTH_ACCOUNT == FYES)
		uttSemV(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
	}
	getTheErrorMsg(eid, wp, argc, argv);
	return 0;
}
/********************************************************************
 * ������ getWebAuthGlobal
 * ���ܣ� ��ҳ��֤ȫ�ֿ�����Ϣ
 * ������ 2012-04-13
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
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
        uttSemP(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
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
        uttSemV(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
	return 0;
}


/***************************************************************
 * ���ܣ��滻�ַ���content�е�Դ�ַ���srcstrΪĿ���ַ�destchar
 * ����ʱ�䣺2013-04-23
 * ������content--Դ�ַ�����
 *      srcstr--Ҫ�滻���ַ���
 *      destchar--�滻�ɵ��ַ�
 * ����ֵ��0--�ɹ� 
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
	    /*���滻�ַ���������ַ�������ǰ�ƶ�*/
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
 * �������ܣ���web��֤ҳ���������ת���Ķ��Żظ�����
 * ����ʱ�䣺2014-01-16
 * ��ע����һ������Ϊhtmlʱ����ת��ֱ�������ҳ����
 *********************************************************************/
static int getSmsContentData(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *prof = NULL;
    char content[WEB_AUTH_CONTACT_LEN + 1];
    memset(content, 0, sizeof(content));
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemP(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
    prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
    if((ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
    {
    strncpy(content, prof->sms.contents, WEB_AUTH_CONTACT_LEN + 1);
    /*�ж��Ƿ���Ҫ��html����ת��Ϊjsp����*/
    if((argv[0]== NULL) || (strcmp(argv[0], "html") != 0))
    {
        replaceStr(content, "<br>", '\n');/*ת��html����Ϊjsp����*/
        replaceStr(content, "&nbsp;", ' ');
    }
    websWrite(wp, "%s", content);
    }
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemV(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
    return 0;
}

/********************************************************************
 * �������ܣ���web��֤ҳ���������ת����΢�Żظ�����
 * ����ʱ�䣺2014-01-16
 * ��ע����һ������Ϊhtmlʱ����ת��ֱ�������ҳ����
 *********************************************************************/
static int getWechatContentData(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *prof = NULL;
    char content[WEB_AUTH_CONTACT_LEN + 1];
    memset(content, 0, sizeof(content));
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemP(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
    prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0))
    {
	strncpy(content, prof->wechat.contents, WEB_AUTH_CONTACT_LEN + 1);
    /*�ж��Ƿ���Ҫ��html����ת��Ϊjsp����*/
    if((argv[0]== NULL) || (strcmp(argv[0], "html") != 0))
    {
        replaceStr(content, "<br>", '\n');/*ת��html����Ϊjsp����*/
        replaceStr(content, "&nbsp;", ' ');
    }
    websWrite(wp, "%s", content);
    }
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemV(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
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
 * �������ܣ���web��֤ҳ���������ת������ϵ��ʽ��Ϣ
 * ����ʱ�䣺2013-03-08
 * ��ע����һ������Ϊhtmlʱ����ת��ֱ�������ҳ����
 *********************************************************************/
static int getContactData(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *prof = NULL;
    char content[WEB_AUTH_CONTACT_LEN + 1];
    memset(content, 0, sizeof(content));
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemP(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
    prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
    if((ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
    {
    strncpy(content, prof->contact, WEB_AUTH_CONTACT_LEN + 1);
    /*�ж��Ƿ���Ҫ��html����ת��Ϊjsp����*/
    if((argv[0]== NULL) || (strcmp(argv[0], "html") != 0))
    {
        replaceStr(content, "<br>", '\n');/*ת��html����Ϊjsp����*/
        replaceStr(content, "&nbsp;", ' ');
    }
    websWrite(wp, "%s", content);
    }
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemV(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
    return 0;
}


/********************************************************************
 * ������ webAuthSetMsg
 * ���ܣ� �޸ľ�̬������ʾ��Ϣ
 * ������ 2012-04-13
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
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
 * ������ getWebAuthMessage
 * ���ܣ� ���û���֤ҳ�������Ϣ����֤ͨ�� or ��֤ʧ��
 * ������ 2012-04-13
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
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
    /* ���lan��IP */
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
 * ������ formWebAuthPOk
 * ���ܣ� ��֤�ɹ�����ַ����OK��
 * ������ 2012-04-26
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static void formWebAuthOk(webs_t wp, char_t *path, char_t *query)
{
	char *result = NULL;

	/*
	 * ͨ��result����û��Ƿ���֤�ɹ�
	 */
	result = websGetVar(wp, T("result"), T(""));
	SWORD_PRINTF("%s %d, result = %s\n", __FUNCTION__, __LINE__, result);
	/*
	 * �ɹ�
	 */
	if(strcmp(result, "OK") == 0)
	{
		if (ipsetTestIpStr(SYS_WEB_AUTH_OK, wp->ipaddr) == 0)
		{ 
			/*���ӵ�����֤*/
			ipsetAOrDIpStr(SYS_WEB_AUTH_OK, wp->ipaddr, IPSET_MEM_ADD); 
		}
	}
	return;
}
/********************************************************************
 * ������ aspGetWebAuthSelfHelpEnabled
 * ���ܣ� output selfhelp is enable or disable
 * ������ 2012-06-18
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ�  
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
 * ������ _WebAuthUserNameByIp
 * ���ܣ� 
 * ������ 2014-01-18
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� ���Ĳ� 
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
		/*����ip�����˺����лỰ*/
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
 * ������ WebAuthUserNameByIp
 * ���ܣ� 
 * ������ 2012-06-15
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ�  
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
 * ������ aspGetWebAuthSelfHelpData
 * ���ܣ� 
 * ������ 2012-06-15
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ�  
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
        uttSemP(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
	profS = (WebAuthGlobalProfile *)ProfGetInstPointByIndex(mibTypeS, 0);
	/*check is correct or not*/
	if((profS->enabled != 1) || (profS->self_help != FUN_ENABLE))
	{
		SWORD_PRINTF("%s-%d; \n",__FUNCTION__,__LINE__);
		websWrite(wp, T("var isWebAuthIp=false;"));
#if (WEBAUTH_ACCOUNT == FYES)
                uttSemV(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
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
        uttSemV(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
	return 0;
}

/********************************************************************
 *  WebAuthIpUserName
 *  �ж��û�ip�Ƿ�ʹ�ô��˺�����֤
 *  2012-06-15
 *  input:
 *	ip	    -   user ip 
 *	name	    -   webAuth name
 * output:
 *	-1	    -   ip δʹ�ô��˺ţ�����
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
		/*����ip�����˺����лỰ*/
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
 * ������ checkData
 * ���ܣ� 
 * ������ 2012-06-15
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ�  
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
 * ������ formWebAuthSelfHelpConfig
 * ���ܣ� 
 * ������ 2012-06-15
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ�  
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
        uttSemP(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
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
        uttSemV(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
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
        uttSemP(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
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
        uttSemV(SEM_WEBAUTH_NO, 0);/*��speedweb��ͻ*/
#endif
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit(); /*save*/
	setTheRightMsg("saveok");
	websRedirect(wp, "learnConfig.asp");
	return ;
}
#if 1 // �ֶ����domian�ܿ��豸���û����ԡ�
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
