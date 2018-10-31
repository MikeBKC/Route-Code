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

#define SWORD_DEBUG 1
#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

#if (CWMP == FYES)
extern unsigned int productSn;


static int getCwmpConfig(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_CWMP;
    CwmpProfile    *prof   = NULL;

    websWrite(wp, "var cwmpEnable=\"\";");
    websWrite(wp, "var cwmpType=\"\";");
    websWrite(wp, "var cwmpServerIp=\"\";");
	websWrite(wp, "var cwmpAcsAuthEnable=\"\";");
    websWrite(wp, "var cwmpUsername=\"\";");
    websWrite(wp, "var cwmpPassword=\"\";\n");

	websWrite(wp, "var cwmpCpeAuthEnable=\"\";");
    websWrite(wp, "var cwmpCpeUsername=\"\";");
    websWrite(wp, "var cwmpCpePassword=\"\";\n");

	websWrite(wp, "var cwmpStunEnable=\"\";");
    websWrite(wp, "var cwmpStunIp=\"\";");
	websWrite(wp, "var cwmpStunAuthEnable=\"\";");
    websWrite(wp, "var cwmpStunUsername=\"\";");
    websWrite(wp, "var cwmpStunPassword=\"\";\n");

    prof = (CwmpProfile*)ProfGetInstPointByIndex(mibType, 0);
    if (prof)
    {
    SWORD_PRINTF("%s --- %d \n",__func__,__LINE__);
	websWrite(wp, "cwmpEnable=\"%d\";",     prof->head.active);
	websWrite(wp, "cwmpType=\"%d\";",       prof->type);
	websWrite(wp, "cwmpServerIp=\"%s\";",   prof->acsUrl);

    SWORD_PRINTF("%s --- %d \n",__func__,__LINE__);
	websWrite(wp, "cwmpAcsAuthEnable=\"%d\";",   prof->acsAuthEnable);
	websWrite(wp, "cwmpUsername=\"%s\";",   prof->acsUsername);
	websWrite(wp, "cwmpPassword=\"%s\";",   prof->acsPassword);

    SWORD_PRINTF("%s --- %d \n",__func__,__LINE__);
	websWrite(wp, "cwmpCpeAuthEnable=\"%d\";",   prof->cpeAuthEnable);
	websWrite(wp, "cwmpCpeUsername=\"%s\";",   prof->cpeUsername);
	websWrite(wp, "cwmpCpePassword=\"%s\";",   prof->cpePassword);

    SWORD_PRINTF("%s --- %d \n",__func__,__LINE__);
	websWrite(wp, "cwmpStunEnable=\"%d\";",   prof->stunEnable);
	websWrite(wp, "cwmpStunIp=\"%s\";",   prof->stunUrl);
    SWORD_PRINTF("%s --- %d \n",__func__,__LINE__);
	websWrite(wp, "cwmpStunAuthEnable=\"%d\";",   prof->stunAuthEnable);
	websWrite(wp, "cwmpStunUsername=\"%s\";",   prof->stunUsername);
	websWrite(wp, "cwmpStunPassword=\"%s\";",   prof->stunPassword);
    SWORD_PRINTF("%s --- %d \n",__func__,__LINE__);
    }

    SWORD_PRINTF("%s --- %d \n",__func__,__LINE__);
    return 0;
}

static void formCwmpConfig(webs_t wp, char_t *path, char_t query)
{
    MibProfileType mibType   = MIB_PROF_CWMP;
    CwmpProfile    *prof     = NULL;
    struProfAlloc  *profhead = NULL;
    int i = 0;

    char_t *cwmpEnable     = NULL;
    char_t *cwmpType       = NULL;
    char_t *cwmpServerIp   = NULL;
    char_t *cwmpUsername   = NULL;
    char_t *cwmpPassword   = NULL;

    cwmpEnable     = websGetVar(wp, T("cwmpEnable"),     T(""));
    cwmpType       = websGetVar(wp, T("cwmpType"),       T(""));
    cwmpServerIp   = websGetVar(wp, T("cwmpServerIp"),   T(""));

	char_t *cwmpAcsAuthEnable = websGetVar(wp, T("cwmpAcsAuthEnable"),T(""));
    cwmpUsername   = websGetVar(wp, T("cwmpUsername"),   T(""));
    cwmpPassword   = websGetVar(wp, T("cwmpPassword"),   T(""));

	char_t *cwmpCpeAuthEnable = websGetVar(wp, T("cwmpCpeAuthEnable"),T(""));
    char_t *cwmpCpeUsername   = websGetVar(wp, T("cwmpCpeUsername"),   T(""));
    char_t *cwmpCpePassword   = websGetVar(wp, T("cwmpCpePassword"),   T(""));

	char_t *cwmpStunEnable = websGetVar(wp, T("cwmpStunEnable"),T(""));
    char_t *cwmpStunIp   = websGetVar(wp, T("cwmpStunIp"),   T(""));
	char_t *cwmpStunAuthEnable = websGetVar(wp, T("cwmpStunAuthEnable"),T(""));
    char_t *cwmpStunUsername  = websGetVar(wp, T("cwmpStunUsername"),   T(""));
    char_t *cwmpStunPassword  = websGetVar(wp, T("cwmpStunPassword"),   T(""));

    prof = (CwmpProfile*)ProfGetInstPointByIndex(mibType, i);
    if (prof)
    {
	SWORD_PRINTF("goahead cwmp set cwmpEnable: '%s'; cwmpType: '%s'; cwmpServerIp: '%s';\n", 
		cwmpEnable, cwmpType, cwmpServerIp);
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, i, &profhead);

	if (strcmp(cwmpEnable, "on") == 0)
	{
        SWORD_PRINTF("%s --- %d \n",__func__,__LINE__);
	    prof->head.active = 1;
	    prof->type        = strtol(cwmpType,       NULL, 10);
        if(strlen(cwmpServerIp) < sizeof(prof->acsUrl))
        {
            memset(prof->acsUrl,0,sizeof(prof->acsUrl));
            memcpy(prof->acsUrl, cwmpServerIp,strlen(cwmpServerIp));
        }

        SWORD_PRINTF("%s --- %d \n",__func__,__LINE__);
	}
	else
    {
	    prof->head.active = 0;
    }

	if (strcmp(cwmpAcsAuthEnable, "on") == 0)
    {
        SWORD_PRINTF("%s --- %d \n",__func__,__LINE__);
        prof->acsAuthEnable = 1;
        if(strlen(cwmpUsername) < sizeof(prof->acsUsername))
        {
            memset(prof->acsUsername,0,sizeof(prof->acsUsername));
            memcpy(prof->acsUsername, cwmpUsername,strlen(cwmpUsername));
        }
        if(strlen(cwmpPassword) < sizeof(prof->acsPassword))
        {
            memset(prof->acsPassword,0,sizeof(prof->acsPassword));
            memcpy(prof->acsPassword, cwmpPassword,strlen(cwmpPassword));
        }
    }else{
        prof->acsAuthEnable = 0;
    }

    SWORD_PRINTF("%s --- %d \n",__func__,__LINE__);
    if (strcmp(cwmpCpeAuthEnable, "on") == 0)
    {
        prof->cpeAuthEnable = 1;
        if(strlen(cwmpCpeUsername) < sizeof(prof->cpeUsername))
        {
            memset(prof->cpeUsername,0,sizeof(prof->cpeUsername));
            memcpy(prof->cpeUsername, cwmpCpeUsername,strlen(cwmpCpeUsername));
        }
        if(strlen(cwmpCpePassword) < sizeof(prof->cpePassword))
        {
            memset(prof->cpePassword,0,sizeof(prof->cpePassword));
            memcpy(prof->cpePassword, cwmpCpePassword,strlen(cwmpCpePassword));
        }
    }else{
        prof->cpeAuthEnable = 0;
    }

    if (strcmp(cwmpStunEnable, "on") == 0)
    {
        SWORD_PRINTF("%s --- %d \n",__func__,__LINE__);
        prof->stunEnable = 1;
        if(strlen(cwmpStunIp) < sizeof(prof->stunUrl))
        {
            memset(prof->stunUrl,0,sizeof(prof->stunUrl));
            memcpy(prof->stunUrl, cwmpStunIp,strlen(cwmpStunIp));
        }
    }else{
        prof->stunEnable = 0;
    }

    if (strcmp(cwmpStunAuthEnable, "on") == 0)
    {
        SWORD_PRINTF("%s --- %d cwmpStunEnable : %s\n",__func__,__LINE__,cwmpStunEnable);
        SWORD_PRINTF("%s --- %d prof->stunEnable : %d\n",__func__,__LINE__,prof->stunEnable);
        prof->stunAuthEnable = 1;
        if(strlen(cwmpStunUsername) < sizeof(prof->stunUsername))
        {
            memset(prof->stunUsername,0,sizeof(prof->stunUsername));
            memcpy(prof->stunUsername, cwmpStunUsername,strlen(cwmpStunUsername));
        }
        if(strlen(cwmpStunPassword) < sizeof(prof->stunPassword))
        {
            memset(prof->stunPassword,0,sizeof(prof->stunPassword));
            memcpy(prof->stunPassword, cwmpStunPassword,strlen(cwmpStunPassword));
        }
    }else{
        prof->stunAuthEnable = 0;
    }

	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit(); /*save*/
    }
    else
    {
	SWORD_PRINTF("cannot get cwmp profile.\n");
    }

    SWORD_PRINTF("%s --- %d \n",__func__,__LINE__);
    websRedirect(wp, "rac.asp");

    return;
}

extern void websDefineCwmp(void)
{
    websAspDefine(T("aspOutputCwmpConfig"), getCwmpConfig);
    websFormDefine(T("formCwmpConfig"), formCwmpConfig);
}

#endif

