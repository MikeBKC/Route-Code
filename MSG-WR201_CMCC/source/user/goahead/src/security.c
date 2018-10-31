/*
 * security.c -- Security handler
 *
 * Copyright (c) GoAhead Software Inc., 1995-2000. All Rights Reserved.
 *
 * See the file "license.txt" for usage and redistribution license requirements
 *
 * $Id: security.c,v 1.28.14.9.2.1 2017/05/25 16:42:50 wu.xiaoyu Exp $
 */

/******************************** Description *********************************/

/*
 *	This module provides a basic security policy.
 */

/********************************* Includes ***********************************/

#include        <linux/autoconf.h>
#include	"wsIntrn.h"
#include	"um.h"
#ifdef DIGEST_ACCESS_SUPPORT
#include	"websda.h"
#endif

#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"

#if (NOTICE == FYES)
extern int isNoticeWeb(webs_t wp); 
#endif

#if (WEB_AUTH == FYES)
extern int redirectWAPage(webs_t wp);
extern int isWebAuthWeb(webs_t wp); 
#endif
#if(WIFIDOG_AUTH == FYES)
#if (WIFIDOG_NOTICE == FYES)
extern int isWifiNoticeWeb(webs_t wp);
#endif
#endif

/********************************** Defines ***********************************/
/*
 *	The following #defines change the behaviour of security in the absence 
 *	of User Management.
 *	Note that use of User management functions require prior calling of
 *	umInit() to behave correctly
 */

#ifndef USER_MANAGEMENT_SUPPORT
#define umGetAccessMethodForURL(url) AM_FULL
#define umUserExists(userid) 0
#define umUserCanAccessURL(userid, url) 1
#define umGetUserPassword(userid) websGetPassword()
#define umGetAccessLimitSecure(accessLimit) 0
#define umGetAccessLimit(url) NULL
#endif

#if (UTT_LOGIN_LIMIT == FYES)
#define LOGIN_ERR_NUM_MAX 5
#define LOGIN_FORBID_TIME 30
#define LOGIN_ERR_RESET_TIME 60
extern int pwdErrNum = 0;
extern int allowLoginFlag = 1;
extern int firstLoginFlag = 0;
extern int restartForbidLoginFlag = 1;
extern unsigned long forbidLoginTimeEnd = 0;
extern unsigned long lastLoginFailTime = 0;
extern char_t lastLoginInput[100] = {0};
extern char* read_file_sui(char *filename, char *buf);
#endif

/******************************** Local Data **********************************/

static char_t	websPassword[WEBS_MAX_PASS];	/* Access password (decoded) */
#ifdef _DEBUG
static int		debugSecurity = 1;
#else
static int		debugSecurity = 0;
#endif

#if ((NET_SECURITY == FYES) && (!(NET_SECURITY_WEB_ABS == FYES)))
/**
 * 判断设备访问控制是否允许 ip地址进行访问
 * ip：需判断的ip字串
 *  return :
 *         TRUE 允许
 *         FLASE 不允许
 */
static bool accessCtrlAllow(char* ip)
{
#if (PPPOE_SERVER == FYES)
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;
#endif
    struct in_addr addrS;
    InterfaceProfile *profIF = NULL;
    SystemProfile *profSys = NULL;
    bool ret = TRUE;
#if (PPPOE_SERVER == FYES)
    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS, 0);
#endif

    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    /*开启了设备访问控制*/
    if( (profSys->sysNetSe.accessCtrlFromIP !=0u) && (profSys->sysNetSe.accessCtrlEndIP != 0u))
    {
	if(inet_aton(ip, &addrS) != 0 )
	{
	    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);/*lan口配置*/
	    /*与lan口同网段*/
	    if( ((profIF->ether.sta.ip & profIF->ether.sta.nm)
		    == (addrS.s_addr & profIF->ether.sta.nm )) 
#if (MULTI_LAN == FYES)
	     || ((profIF->ether.sta.ip2 & profIF->ether.sta.nm2) == (addrS.s_addr & profIF->ether.sta.nm2)) 
	     || ((profIF->ether.sta.ip3 & profIF->ether.sta.nm3) == (addrS.s_addr & profIF->ether.sta.nm3)) 
	     || ((profIF->ether.sta.ip4 & profIF->ether.sta.nm4) == (addrS.s_addr & profIF->ether.sta.nm4)) 
#endif
#if (PPPOE_SERVER == FYES)
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
	     || ((ntohl(profS->ipStart) <= ntohl(addrS.s_addr)) && (ntohl(profS->ipStart) + profS->ipcount > addrS.s_addr))
#else
	     || ((ntohl(profS->ipStart) <= ntohl(addrS.s_addr)) && (ntohl(profS->ipStart) + profS->maxSessCnt > addrS.s_addr))
#endif
#endif
		    )
	    {
		/*不在设备访问控制网段内*/
		if(( ntohl(addrS.s_addr) < ntohl(profSys->sysNetSe.accessCtrlFromIP))
			|| (ntohl(addrS.s_addr) > ntohl(profSys->sysNetSe.accessCtrlEndIP)))
		{
		    ret = FALSE;
		}
	    }
	}
    }

    return ret;
}
#endif

#if (EASY_TASK == FYES)
/*
 * 功能：判断并跳转到易管理通告页面
 */
static int isEasyTask(webs_t wp)
{
    int retVal = 0;
    char lanip[16];/*LAN口IP*/
    struct in_addr addr;
    char url[100];/*跳转地址*/
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    MibProfileType mibTypeEt = MIB_PROF_EASY_TASK;
    EasyTaskProfile *profEt = NULL;

    memset(lanip, 0, sizeof(lanip));
    getIfIp(getLanIfName(), lanip);

    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibTypeSys, 0);
    profEt = (EasyTaskProfile*)ProfGetInstPointByIndex(mibTypeEt, 0);
    if ((wp->httpHeadHost != NULL) && (profEt->wanLinkStates == 0U) && ((inet_aton(wp->httpHeadHost, &addr) == 0) || (ipsetTestIpStr(SYS_LAN_GROUP, wp->httpHeadHost) == 0)))
    {
	memset(url, 0, sizeof(url));
	/*判断出厂设置*/
	if (profSys->isDefaultConf == 0)
	{
#if (UTT_SMART_UI == FYES)
	    snprintf(url, sizeof(url), "http://%s/EasyManageWelcome_Sui.asp", lanip);
#else
	    snprintf(url, sizeof(url), "http://%s/EasyManageWelcome.asp", lanip);
#endif
	    websRedirect(wp, url);
	    retVal = 1;
	}
	else/*非出厂设置*/
	{
	    /*开启易管理提示功能且wan口都断开*/
	    if (profEt->easyTaskNoticeEn == FUN_ENABLE)
	    {
#if (UTT_SMART_UI == FYES)
		snprintf(url, sizeof(url), "http://%s/NoticEasyConfig_Sui.asp", lanip);
#else
		snprintf(url, sizeof(url), "http://%s/NoticEasyConfig.asp", lanip);
#endif
		websRedirect(wp, url);
		retVal = 1;
	    }
	}
    }
    return retVal;
}
#endif


/*********************************** Code *************************************/
/*
 *	Determine if this request should be honored
 */
int websSecurityHandler(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg, 
						char_t *url, char_t *path, char_t *query)
{
	char_t			*type, *userid, *password, *accessLimit;
	int				flags, nRet;
	accessMeth_t	am;
#if (NOTICE == FYES)
#if 0
	char * ntUrl = NULL;
	int ntStatus = -1;
#endif
#endif

	a_assert(websValid(wp));
	a_assert(url && *url);
	a_assert(path && *path);
	



#if (LOGIN_PAGE_DIY == FYES)
	if (strstr(wp->path, "/login.asp") != NULL)
	{
	    return websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
	else if (strstr(wp->path, "/images_Sui") != NULL)
	{
	    return websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
	else if (strstr(wp->path, "/css/style_new.css") != NULL)
	{
	    return websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
	else if (strstr(wp->path, "/script/curvycorners.js") != NULL)
	{
	    return websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
	else if (strstr(wp->path, "/script/jquery-1.8.1.min.js") != NULL)
	{
	    return websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
#endif

#if (EASY_TASK == FYES)
#if (UTT_SMART_UI == FYES)
	if (strstr(wp->path, "/EasyManageWelcome_Sui.asp") != NULL)
#else
	if (strstr(wp->path, "/EasyManageWelcome.asp") != NULL)
#endif
	{
	    return websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
#if (UTT_SMART_UI == FYES)
	else if (strstr(wp->path, "/NoticEasyConfig_Sui.asp") != NULL)
#else
	else if (strstr(wp->path, "/NoticEasyConfig.asp") != NULL)
#endif
	{
	    return websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
	else if (strstr(wp->path, "/js/commonSkipAuth.js") != NULL)
	{
	    return websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
	else if (strstr(wp->path, "/goform/formCloseDefConf") != NULL)
	{
	    return websFormHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
#if (UTT_SMART_UI == FYES)
	else if (strstr(wp->path, "/goform/formEasyTaskNoticeEn_Sui") != NULL)
	{
	    return websFormHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
	else if (strstr(wp->path, "/EasyManageSuccess_Sui.asp") != NULL)
#else
	else if (strstr(wp->path, "/EasyManageSuccess.asp") != NULL)
#endif
	{
	    return websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
#if (UTT_SMART_UI == FYES)
	else if (strstr(wp->path, "/script/jquery-1.8.1.min.js") != NULL)
	{
	    return websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
	else if (strstr(wp->path, "/script/jquery.corner.js") != NULL)
	{
	    return websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
	else if (strstr(wp->path, "/css/style_new.css") != NULL)
	{
	    return websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
#endif
#if (MAKE_MONEY == FYES)
	else if(strstr(wp->path, "/getParam.asp") != NULL)
	{
	    return websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
//	else if(strstr(wp->path, "/savePhone.asp") != NULL)
//	{
//	    return websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query);
//	}

#endif

#if (UTT_DOMAINACCESS == FYES)
	else if ((wp->httpHeadHost != NULL) && ((strstr(wp->httpHeadHost,DOMAIN_NAME) != NULL)||(strstr(wp->httpHeadHost,DOMAIN_NAME_ADD) != NULL))) {
	    goto auth;
	}
#endif
	else if (isEasyTask(wp) == 1)
	{
	    return 1;
	}
#endif
#if (WEB_AUTH == FYES) 
#if (WEBAUTH_AUTO == FYES)
	if (strstr(wp->path, "/goform/wechatUrl") != NULL)
	{
	        return websFormHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
#endif
	/*访问网页认证页面不做安全认证*/ 
	if(isWebAuthWeb(wp) == 1) 
	{
	    return websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
	else if(strstr(wp->path, "/goform/formWebAuthUserSubmit") != NULL)
	{
	    return websFormHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
#if (WEBAUTH_AUTO == FYES)
	else if(strstr(wp->path, "/goform/formWebAuthGetPhoneSubmit") != NULL)
	{
	    return websFormHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
	else if(strstr(wp->path, "/goform/formWebAuthCaptchaSubmit") != NULL)
	{
	    return websFormHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
	else if(strcmp(wp->path, "/goform/formWebAuthFastSubmit") == 0)
	{
	    return websFormHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
#endif
	else if(strstr(wp->path, "/goform/formWebAuthOk") != NULL)
	{
	    return websFormHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
	else if((strstr(wp->path, "/images") != NULL) || (strstr(wp->path, "gif") != NULL) 
			|| (strstr(wp->path, "favicon.ico") != NULL)/*UC浏览器不知为何经常get这个页面*/
			)
	{/*webAuth图片*/
	    return websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
	else if(strstr(wp->path, "/WebAuth_message.asp") != NULL)/*用户安全下线页面不做认证*/
	{
	    return websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
	else if ((strstr(wp->path, "/js") != NULL) || (strstr(wp->path, "/lang") != NULL))
	{
	    return websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
	else
	{
#endif
#if ((WEB_AUTH == FYES) || (NOTICE == FYES))
	    if (redirectWAPage(wp) == 1)/*跳转到网页认证页面或域名过滤通告页面*/
	    {
		return 1; 
	    }
#endif
#if (WEB_AUTH == FYES) 
	}
#if (WEBAUTH_SELF_HELP == FYES)
	if(strstr(wp->path, "/waUsers.asp") != NULL)
	{
	    return websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
	if(strstr(wp->path, "/goform/formWebAuthSelfHelpConfig") != NULL)
	{
	    return websFormHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
#endif

#endif
#if (PPPOE_SELF_HELP == FYES)
	if(strstr(wp->path, "/poeUsers.asp") != NULL)
	{
	    return websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
	if(strstr(wp->path, "/goform/formPppoeSelfHelpConfig") != NULL)
	{
	    return websFormHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
#endif


#if (NOTICE == FYES)
	/* 
	 * 访问通告页面不做安全认证 
	 */
	if (isNoticeWeb(wp) == 1) {
	    return websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query);
	} else if(strstr(wp->path, "/"NOTICE_HTML_3_NAME) != NULL) { /*域名过滤通告页面*/
	    return websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
#endif
#if(WIFIDOG_AUTH == FYES)
#if (WIFIDOG_NOTICE == FYES)
	/*wifidog 到期通告推送*/
	/* 
	 * 访问wifidog通告页面不做安全认证 
	 */
	if (isWifiNoticeWeb(wp) == 1) {
	    return websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
#endif
#endif

#if (FEATURE_AC == FYES)
	if(strstr(wp->path, "/api/") != NULL) {
	    return websApiHandler(wp, urlPrefix, webDir, arg, url, path, query);
	}
#endif

 auth:
#if ((NET_SECURITY == FYES) && (!(NET_SECURITY_WEB_ABS == FYES)))
        /*设备访问控制不允许本次访问*/
	if(!accessCtrlAllow(wp->ipaddr))
	{
#if 0
	    websError(wp, 405, T("Access Denied\nSecure access is required."));
#endif
	    return 1;
	}
#endif


#if (UTT_LOGIN_LIMIT == FYES)
/* 判断用户登录信息输入错误次数及跳转提示页面 */
	char buf[128]={0},*p;
	unsigned long sysTimeNow = 0;
	static unsigned long  lastLoginTime = 0;
	read_file_sui("/proc/uptime", buf);
	p=strchr(buf,'.');
	*p='\0';
	sysTimeNow=strtol(buf, NULL, 10);
	firstLoginFlag = 0;
	if (!allowLoginFlag)
	{
	    if (sysTimeNow > forbidLoginTimeEnd)
	    {
		allowLoginFlag = 1;
		firstLoginFlag = 1;
		pwdErrNum = 0;
		lastLoginInput[0] = "\0";
	    }
	    else
	    {
		if (strstr(wp->path, "/forbidlogin.asp") != NULL)
		{
		    return websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query);
		}
		else
		{
		    websRedirect(wp, "forbidlogin.asp");
		    return 1;
		}
	    }
	}
	else if (restartForbidLoginFlag && (sysTimeNow - lastLoginFailTime > LOGIN_ERR_RESET_TIME)) // 最后一次登录信息错误 60 秒后错误次数清零
	{
		restartForbidLoginFlag = 0;
		pwdErrNum = 0;
	}
#endif



/*
 *	Get the critical request details
 */
	type = websGetRequestType(wp);
	password = websGetRequestPassword(wp);
	userid = websGetRequestUserName(wp);
	flags = websGetRequestFlags(wp);


/*
 *	Get the access limit for the URL.  Exit if none found.
 */
	accessLimit = umGetAccessLimit(path);
	if (accessLimit == NULL) {
		return 0;
	}
/*
 *	Check to see if URL must be encrypted
 */
#ifdef WEBS_SSL_SUPPORT
	nRet = umGetAccessLimitSecure(accessLimit);
	if (nRet && ((flags & WEBS_SECURE) == 0)) {
		websStats.access++;
		websError(wp, 405, T("Access Denied\nSecure access is required."));
		trace(3, T("SEC: Non-secure access attempted on <%s>\n"), path);
      /* bugfix 5/24/02 -- we were leaking the memory pointed to by
       * 'accessLimit'. Thanks to Simon Byholm.
       */
      bfree(B_L, accessLimit);
		return 1;
	}
#endif

/*
 *	Get the access limit for the URL
 */
	am = umGetAccessMethodForURL(accessLimit);

	nRet = 0;
	if ((flags & WEBS_LOCAL_REQUEST) && (debugSecurity == 0)) {
/*
 *		Local access is always allowed (defeat when debugging)
 */
	} else if (am == AM_NONE) {
/*
 *		URL is supposed to be hidden!  Make like it wasn't found.
 */
		websStats.access++;
		websError(wp, 404, T("Page Not Found"));
		nRet = 1;
#if (UTT_LOGIN_LIMIT == FYES)
	} else 	if ((!firstLoginFlag) && (userid && *userid)) {
#else
	} else 	if (userid && *userid) {
#endif
		if (!umUserExists(userid)) {
/* 登录信息输入错误次数，5次后跳转至提示页面 */
#if (UTT_LOGIN_LIMIT == FYES)
			
			if (allowLoginFlag && strcmp(lastLoginInput,userid))
			{
			    char buf[128]={0},*p;
			    read_file_sui("/proc/uptime", buf);
			    p=strchr(buf,'.');
			    *p='\0';
			    lastLoginFailTime = strtol(buf, NULL, 10);
			    strcpy(lastLoginInput,userid);
			    restartForbidLoginFlag = 1;
			    pwdErrNum++;
			    if (pwdErrNum >= LOGIN_ERR_NUM_MAX) 
			    {
				forbidLoginTimeEnd = lastLoginFailTime + LOGIN_FORBID_TIME;
				allowLoginFlag = 0;
				pwdErrNum = 0;
				websRedirect(wp, "forbidlogin.asp");
				return 1;
			    }
			}
#endif
/********************************************/
			websStats.access++;
			websError(wp, 401, T("Access Denied\nUnknown User"));
			trace(3, T("SEC: Unknown user <%s> attempted to access <%s>\n"), 
				userid, path);
			nRet = 1;
		} else if (!umUserCanAccessURL(userid, accessLimit)) {
			websStats.access++;
			websError(wp, 403, T("Access Denied\nProhibited User"));
			nRet = 1;
		} else if (password && * password) {
			char_t * userpass = umGetUserPassword(userid);
			if (userpass) {
			    if ((gstrcmp(password, userpass) != 0 ) || (sysTimeNow-lastLoginTime > 900)) {

				/* 登录信息输入错误次数，5次后跳转至提示页面 */
#if (UTT_LOGIN_LIMIT == FYES)
				if (allowLoginFlag && strcmp(lastLoginInput,password))
				{
				    char buf[128]={0},*p;
				    read_file_sui("/proc/uptime", buf);
				    p=strchr(buf,'.');
				    *p='\0';
				    lastLoginFailTime = strtol(buf, NULL, 10);
				    strcpy(lastLoginInput,password);
				    restartForbidLoginFlag = 1;
				    if(gstrcmp(password, userpass) != 0)
				    {
					pwdErrNum++;
				    }
				    if (pwdErrNum >= LOGIN_ERR_NUM_MAX) 
				    {
					forbidLoginTimeEnd = lastLoginFailTime + LOGIN_FORBID_TIME;
					allowLoginFlag = 0;
					pwdErrNum = 0;
					websRedirect(wp, "forbidlogin.asp");
					return 1;
				    }
				}
#endif
				/********************************************/
				if(gstrcmp(password, userpass) != 0)
				{
				    websStats.access++;
				    websError(wp, 401, T("Access Denied\nWrong Password"));
				    trace(3, T("SEC: Password fail for user <%s>")
					    T("attempt to access <%s>\n"), userid, path);
				    nRet = 1;
				}
				else
				{
				    /*
				     *		This will cause the browser to display a password / username
				     *		dialog
				     */
#ifdef DIGEST_ACCESS_SUPPORT
				    if (am == AM_DIGEST) {
					wp->flags |= WEBS_AUTH_DIGEST;
				    }
#endif
				    websStats.errors++;
				    {
					websError(wp, 401, T("Access to this document requires a User ID"));
					nRet = 1;
				    }

				}
			    }else {
				/*
				 *					User and password check out.
				 *
				 */
			    }
			    bfree (B_L, userpass);
			}
#ifdef DIGEST_ACCESS_SUPPORT
		} else if (flags & WEBS_AUTH_DIGEST) {

			char_t *digestCalc;

/*
 *			Check digest for equivalence
 */
			wp->password = umGetUserPassword(userid);

			a_assert(wp->digest);
			a_assert(wp->nonce);
			a_assert(wp->password);
							 
			digestCalc = websCalcDigest(wp);
			a_assert(digestCalc);

			if (wp->digest == NULL || gstrcmp(wp->digest, digestCalc) != 0) {
				websStats.access++;
            /* 16 Jun 03 -- error code changed from 405 to 401 -- thanks to
             * Jay Chalfant.
             */
				websError(wp, 401, T("Access Denied\nWrong Password"));
				nRet = 1;
			}

			bfree (B_L, digestCalc);
#endif
		} else {
/*
 *			No password has been specified
 */
#ifdef DIGEST_ACCESS_SUPPORT
			if (am == AM_DIGEST) {
				wp->flags |= WEBS_AUTH_DIGEST;
			}
#endif
			websStats.errors++;
			websError(wp, 401, 
				T("Access to this document requires a password"));
			nRet = 1;
		}
	} else if (am != AM_FULL) {
/*
 *		This will cause the browser to display a password / username
 *		dialog
 */
#ifdef DIGEST_ACCESS_SUPPORT
		if (am == AM_DIGEST) {
			wp->flags |= WEBS_AUTH_DIGEST;
		}
#endif
		websStats.errors++;
		websError(wp, 401, T("Access to this document requires a User ID"));
		nRet = 1;
		lastLoginTime = sysTimeNow;
	}
	lastLoginTime = sysTimeNow;
	bfree(B_L, accessLimit);
	    

	return nRet;
}

/******************************************************************************/
/*
 *	Delete the default security handler
 */

void websSecurityDelete()
{
	websUrlHandlerDelete(websSecurityHandler);
}

/******************************************************************************/
/*
 *	Store the new password, expect a decoded password. Store in websPassword in 
 *	the decoded form.
 */

void websSetPassword(char_t *password)
{
	a_assert(password);

	gstrncpy(websPassword, password, TSZ(websPassword));
}

/******************************************************************************/
/*
 *	Get password, return the decoded form
 */

char_t *websGetPassword()
{
	return bstrdup(B_L, websPassword);
}

/******************************************************************************/


