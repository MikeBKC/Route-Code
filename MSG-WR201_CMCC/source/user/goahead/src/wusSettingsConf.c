/**
 *配置管理相关
 *bhou
 *2011-10-14
 * */
#include        <linux/autoconf.h>
#include	<string.h>
#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include	"translate.h"

#if (SETTINGS_CONF == FYES)

extern void makeFactoryProfiles(void);
static void formResetSettings(webs_t wp, char_t * path, char_t * query);
static void formUploadSettings(webs_t wp, char_t * path, char_t * query);
static void formExportSettings(webs_t wp, char_t * path, char_t * query);

void formDefineSettingsConf(void);

/********************************************************************
 * 函数： formDefineDeviceConfigure
 * 功能： 页面与交互转换函数
 * 作者： bhou
 *******************************************************************/
void formDefineSettingsConf(void)
{
    websFormDefine(T("formResetSettings"), formResetSettings);
    websFormDefine(T("formUploadSettings"), formUploadSettings);
    websFormDefine(T("formExportSettings"), formExportSettings);
}

/*
 * 恢复出厂配置
 * bhou
 * 2011-10-14
 * */
static void formResetSettings(webs_t wp, char_t * path, char_t * query)
{
#if (NOTICE == FYES)
    char title[2], content[2];
    memset(title, 0, sizeof(title));
    memset(content, 0, sizeof(content));
#endif
#if (UTT_LICENSE == FYES)
    LicenseProfile uttlicense;
    LicenseProfile *licensePtr;
    memset(&uttlicense, 0, sizeof(LicenseProfile));
    licensePtr =(LicenseProfile *)ProfGetInstPointByIndex(MIB_PROF_LICENSE, 0);
    if((licensePtr != NULL)){
	memcpy(&uttlicense, licensePtr,sizeof(LicenseProfile));
    }
#endif
#if (NET_MODE_SWITCH == FYES)
	BridgeProfile *profBR = NULL;
	int brEnable = 0;
	profBR = (BridgeProfile *)ProfGetInstPointByIndex(MIB_PROF_BRIDGE,0);
	if(profBR!=NULL){
		brEnable = profBR->head.active;
	}   
#endif
#if (CACHE_SERVER == FYES)
    CacheServerProfile *profCacheServer = NULL;
	int cacheEnable = 0;
    profCacheServer = (CacheServerProfile*)ProfGetInstPointByIndex(MIB_PROF_CACHE_SERVER, 0);
	if(profCacheServer!=NULL){
		cacheEnable = profCacheServer->head.active;
	}
#endif
    makeFactoryProfiles();/*恢复*/
#if (NOTICE == FYES)
    saveConfigToFile(title, content, NOTIFY_HTM, NOTICE_HTML_1);
    saveConfigToFile(title, content, NOTIFY_HTM, NOTICE_HTML_2);
    saveConfigToFile(title, content, DNS_NOTIFY_HTM, NOTICE_HTML_3);
#endif
#if (NET_SNIPER == FYES)
    saveConfigToFile(title, content, NOTIFY_HTM, NOTICE_HTML_4);
#endif
#if (WEB_AUTH == FYES)
	char contact[WEB_AUTH_CONTACT_LEN + 1];
	char pictureUrls[511];
	memset(contact, 0, sizeof(contact));
	memset(pictureUrls,0,sizeof(pictureUrls));
	saveContactToFile(contact, WEB_AUTH_CONTACT_PATH);
	saveContactToFile(pictureUrls, WEB_AUTH_PICURL_PATH);
#if (WEBAUTH_AUTO == FYES)
	char jumpUrls[511];
	memset(jumpUrls,0,sizeof(jumpUrls));
	saveContactToFile(jumpUrls, WEB_AUTH_JUMPURL_PATH);
#endif
#if (WEBAUTH_ADVANCE == FYES)
#if (CONFIG_X86_CPU == 1)
    doSystem("rm -f /ff/*.png /ff/*.jpg /ff/*.gif /ff/*.jpeg /ff/*.PNG /ff/*.JPG /ff/*.GIF /ff/*.JPEG /ff/*.asp");
#else
#endif
#endif
#endif
#if (UTT_LICENSE == FYES)
    licensePtr =(LicenseProfile *)ProfGetInstPointByIndex(MIB_PROF_LICENSE, 0);
    if((licensePtr != NULL)){
	memcpy(licensePtr,&uttlicense,sizeof(LicenseProfile));
    }
    DBGPRINT("license->runtime=%d, impowerTime=%d, authResult=%d, taskRunning=%d, len=%d, timeflag=%d, controltype=%d\n",
	    licensePtr->runtime,licensePtr->impowerTime,licensePtr->authResult, licensePtr->taskRunning,licensePtr->len,licensePtr->timeflag,licensePtr->controltype);
#endif
#if (NET_MODE_SWITCH == FYES)
	if(profBR!=NULL){
		profBR->head.active = brEnable;
	}
#endif
#if (UTT_LICENSE == FYES)
    licensePtr =(LicenseProfile *)ProfGetInstPointByIndex(MIB_PROF_LICENSE, 0);
    if((licensePtr != NULL)){
	memcpy(licensePtr,&uttlicense,sizeof(LicenseProfile));
    }
    DBGPRINT("license->runtime=%d, impowerTime=%d, authResult=%d, taskRunning=%d, len=%d, timeflag=%d, controltype=%d\n",
	    licensePtr->runtime,licensePtr->impowerTime,licensePtr->authResult, licensePtr->taskRunning,licensePtr->len,licensePtr->timeflag,licensePtr->controltype);
#endif
#if (NET_MODE_SWITCH == FYES)
	if(profBR!=NULL){
		profBR->head.active = brEnable;
	}   
#endif
#if (CACHE_SERVER == FYES)
		profCacheServer->head.active = cacheEnable;
#endif
#if 0
    ProfWiteToFlash();/*写flash*/
#else
    ProfWiteToFlashNoSyn();/*恢复出厂写flash时不去修改版本号*/
#endif
#if (UTT_SMART_UI == FYES)
    websRedirect(wp, "BackupRecovery.asp");
#else
    websRedirect(wp, "DeviceConfig.asp");
#endif
}
/**
 * 配置导入
 * bhou
 * 2011-10-14
 */
static void formUploadSettings(webs_t wp, char_t * path, char_t * query)
{
    char *     fn;
    char *     bn = NULL;
    char *     chk;
    bool  reset = 0;
    char *postData;
    int fnlen; 
#if(MAKE_MONEY == FYES)
    char phone[12];
    MakeMoneyProfile *prof =NULL;
#endif
#if (CLOUD_MANAGE == FYES)
    CloudManageProfile *prof_cm= NULL;
#endif
#if (UTT_LICENSE == FYES)
    LicenseProfile uttlicense;
    LicenseProfile *licensePtr;
#endif
	const char *ErrMsg=NULL;

    a_assert(websValid(wp));

    fn = websGetVar(wp, T("filename"), T(""));

    chk = websGetVar(wp,T("chkset"),  T(""));

    if (strcmp(chk,"on") ==0) {
	reset = 1;
    }

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


    if( fnlen <= 0 ){
		ErrMsg=getTransDataForC(C_LANG_INDEX_CONFIG_FILE_FORMAT);
	setTheErrorMsg(ErrMsg);
    }
    else
    {
	if(postData) {
#if (UTT_LICENSE == FYES)
	    memset(&uttlicense, 0, sizeof(LicenseProfile));
	    licensePtr =(LicenseProfile *)ProfGetInstPointByIndex(MIB_PROF_LICENSE, 0);
	    if((licensePtr != NULL)){
		memcpy(&uttlicense, licensePtr,sizeof(LicenseProfile));
	    }
#endif
	    postData[fnlen] = '\0';/*去除其他无用的字符*/
#if(MAKE_MONEY == FYES)
            prof = (MakeMoneyProfile *)ProfGetInstPointByIndex(MIB_PROF_MAKE_MONEY,0);
            if(prof != NULL)
            {   
                memset(phone,0,sizeof(phone));
                strcpy(phone,prof->phoneNumber);
            }   
#endif
#if (CLOUD_MANAGE == FYES)
	    prof_cm = (CloudManageProfile*)ProfGetInstPointByIndex(MIB_PROF_CLOUD_MANAGE,0);
	    if(prof_cm!=NULL)
	    {	
		prof_cm->configVer = 0;
	    }
#endif
	    if(parseProfileFromStr(postData, reset) != 0 )/*合并配置到内存*/
	    {
		ErrMsg=getTransDataForC(C_LANG_INDEX_CONFIG_FILE_FORMAT);
		setTheErrorMsg(ErrMsg);
	    }
	    else
	    {
#if(MAKE_MONEY == FYES)
		prof = (MakeMoneyProfile *)ProfGetInstPointByIndex(MIB_PROF_MAKE_MONEY,0);
                if(prof != NULL)
                {   
                    if(strcmp(phone,prof->phoneNumber)!=0)
                    {   
			prof->isBind = 0;
			prof->MakeMoneyEn = 0;
//                        ErrMsg="新导入的配置将导致赚钱wifi手机号改变";
//                        setTheErrorMsg(ErrMsg);
                    }
		}
#endif
		ProfWiteToFlash();/*写flash*/
	    }

	    bfree(B_L, postData);
#if (UTT_LICENSE == FYES)
	    licensePtr =(LicenseProfile *)ProfGetInstPointByIndex(MIB_PROF_LICENSE, 0);
	    if((licensePtr != NULL)){
		memcpy(licensePtr,&uttlicense,sizeof(LicenseProfile));
	    }
	    DBGPRINT("license->runtime=%d, impowerTime=%d, authResult=%d, taskRunning=%d, len=%d, timeflag=%d, controltype=%d\n",
		licensePtr->runtime,licensePtr->impowerTime,licensePtr->authResult, licensePtr->taskRunning,licensePtr->len,licensePtr->timeflag,licensePtr->controltype);
#endif
	}

    }

#if (UTT_SMART_UI == FYES)
    websRedirect(wp, "BackupRecovery.asp");
#else
    websRedirect(wp, "DeviceConfig.asp");
#endif
}

/**
 * 配置导出
 * bhou
 * 2011-10-14
 */
static void formExportSettings(webs_t wp, char_t * path, char_t * query)
{
	char fileName[30];
	time_t now;
	struct tm *timenow;
	const char *ErrMsg = NULL;
#if 0
    char buf[CONFIG_BUFF_SIZE];
#endif
    char *buf = NULL;

	time(&now);
	timenow = localtime(&now);
	sprintf(fileName,"config_%d%02d%02d%02d%02d.xml",(1900+timenow->tm_year),(1+timenow->tm_mon),timenow->tm_mday,timenow->tm_hour,timenow->tm_min);

    buf = (char *)malloc(CONFIG_BUFF_SIZE);
    if (!buf) {
	return;
    }
    memset(buf, 0, CONFIG_BUFF_SIZE);
#if 0
    memset(buf, 0, sizeof(buf));
#endif

    if( parseProfileToStr(buf, NULL) == -1)
    {
	/*解析配置为字符串失败*/
		ErrMsg=getTransDataForC(C_LANG_INDEX_CONFIG_FILE_READ_ERROR);
		strcpy(buf, ErrMsg);
    }

    /*下载*/
    wimDownloadFile(wp,fileName,buf,strlen(buf));
    free(buf);
}
#endif
