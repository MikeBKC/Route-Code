

/*
 * 文件名:
 *     wusLogManage.c
 *
 * 描述:
 *    行为审计交互文件
 *    
 * 作者：
 *     孙剑卿
 *
 * 维护人：
 *     孙剑卿
 *
 * 变更：
 *     1. 初始版本:2012-05-24
 *
 */
#include <stdio.h>
#include <string.h>
#include <mib.h>
#include "typedef.h"
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "uttfunction.h"
#include "profacce.h"
#include "uttMachine.h"
#if (LOG_MANAGE == FYES)
#include <include/linux/netfilter_ipv4/utt_netfilter.h>
#include <include/linux/netfilter/xt_UTTLOG.h>

#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif
#if 0
static char lastline[200];
#endif

static int getLogManageConfig(int eid, webs_t wp, int argc, char **argv); 
static void formLogManageConfig(webs_t wp, char_t *path, char_t *query);
static void syslog_audit(webs_t wp, char_t * path, char_t * query);
static void DelHistoryLog(webs_t wp, char_t * path, char_t * query);
/********************************************************************
 * 函数： websDefineWebAuth
 * 功能： 页面与交互转换函数
 * 创建： 2012-04-13
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
extern void websDefineLogManage(void)
{
	websFormDefine(T("LogManageConfig"), formLogManageConfig);
	websFormDefine(T("syslog_audit"), syslog_audit);
	websFormDefine(T("DelHistoryLog"), DelHistoryLog);
	websAspDefine(T("aspOutputLogManageConfig"), getLogManageConfig);     
}
/********************************************************************
 * 函数： formLogManageConfig
 * 功能： 行为审计页面读入交互
 * 创建： 2012-05-24
 * 参数： 交互固定输入参数
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void formLogManageConfig(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_LOG_MANAGE;
    LogManageProfile *prof = NULL;
    struProfAlloc *profhead = NULL;

    char_t *webLogEnable = NULL;
    char_t *QQLogEnable = NULL;
    char_t *MSNLogEnable = NULL;
    char_t *mailLogEnable = NULL;
    char_t *eimLogEnable = NULL;
#if CONFIG_XPORT_LOG 
    char_t *luntanLogEnable = NULL;
    char_t *weiboLogEnable = NULL;
    char_t *NatLogEnable = NULL;
#endif
    /*从页面空间获取值*/
    webLogEnable = websGetVar(wp, T("weblog"), T(""));
    QQLogEnable = websGetVar(wp, T("QQlog"), T(""));
    MSNLogEnable = websGetVar(wp, T("MSNlog"), T(""));
    mailLogEnable = websGetVar(wp, T("maillog"), T(""));
    eimLogEnable = websGetVar(wp, T("eimlog"), T(""));
#if CONFIG_XPORT_LOG 
    luntanLogEnable = websGetVar(wp, T("luntanlog"), T(""));
    weiboLogEnable = websGetVar(wp, T("weibolog"), T(""));
    NatLogEnable = websGetVar(wp, T("Natlog"), T(""));
    SWORD_PRINTF("%s-%d,luntanLogEnable = %s,weiboLogEnable = %s,NatLogEnable = %s\n", __func__, __LINE__,luntanLogEnable,weiboLogEnable,NatLogEnable);
#endif

    /*取得profile*/
    prof = (LogManageProfile *)ProfGetInstPointByIndex(mibType, 0);
    SWORD_PRINTF("%s-%d, webLogEnable = %s, QQLogEnable = %s, MSNLogEnable = %s, mailLogEnable = %s, eimLogEnable = %s\n", __func__, __LINE__, webLogEnable, QQLogEnable, MSNLogEnable, mailLogEnable, eimLogEnable);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0))/*正在使用*/
    {
	SWORD_PRINTF("%s-%d\n", __func__, __LINE__);
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
	if(strcmp(webLogEnable, "on") == 0)/*开关打开*/
	{
	    prof->webLogEnable = LOG_ENABLE;
	}
	else
	{
	    prof->webLogEnable = LOG_DISABLE;
	}
	if(strcmp(QQLogEnable, "on") == 0)
	{
	    prof->QQLogEnable = LOG_ENABLE;
	}
	else
	{
	    prof->QQLogEnable = LOG_DISABLE;
	}
	if(strcmp(MSNLogEnable, "on") == 0)
	{
	    prof->MSNLogEnable = LOG_ENABLE;
	}
	else
	{
	    prof->MSNLogEnable = LOG_DISABLE;
	}
	if(strcmp(mailLogEnable, "on") == 0)
	{
	    prof->mailLogEnable = LOG_ENABLE;
	}
	else
	{
	    prof->mailLogEnable = LOG_DISABLE;
	}
	if (strcmp(eimLogEnable, "on") == 0) {
	    prof->eimLogEnable = LOG_ENABLE;
	} else {
	    prof->eimLogEnable = LOG_DISABLE;
	}
#if CONFIG_XPORT_LOG 
	if (strcmp(luntanLogEnable, "on") == 0) {
	    prof->luntanLogEnable = LOG_ENABLE;
	} else {
	    prof->luntanLogEnable = LOG_DISABLE;
	}
	if (strcmp(weiboLogEnable, "on") == 0) {
	    prof->weiboLogEnable = LOG_ENABLE;
	} else {
	    prof->weiboLogEnable = LOG_DISABLE;
	}
	if (strcmp(NatLogEnable, "on") == 0) {
	    prof->NatLogEnable = LOG_ENABLE;
	} else {
	    prof->NatLogEnable = LOG_DISABLE;
	}
#endif 
	prof->head.active = TRUE;
    }



    ProfUpdate(profhead);/*更新*/
    ProfFreeAllocList(profhead);
    nvramWriteCommit(); /*save*/
    websRedirect(wp, "LogManage.asp");

    return;	   
}
/********************************************************************
 * 函数： getLogManageConfig
 * 功能： 行为审计写入页面交互
 * 创建： 2012-05-24
 * 参数： 交互固定输入参数
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int getLogManageConfig(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_LOG_MANAGE;
    LogManageProfile *prof = NULL;

    websWrite(wp, "var webLogEnable = \"\";");
    websWrite(wp, "var QQLogEnable = \"\";");
    websWrite(wp, "var MSNLogEnable = \"\";");
    websWrite(wp, "var mailLogEnable =\"\";");
    websWrite(wp, "var eimLogEnable =\"\";");
    websWrite(wp, "var xportEnable = \"no\";");

    websWrite(wp, "var luntanLogEnable =\"\";");
    websWrite(wp, "var weiboLogEnable =\"\";");
    websWrite(wp, "var NatLogEnable =\"\";");
#if CONFIG_XPORT_LOG 
    websWrite(wp, "xportEnable = \"yes\";");
#endif
    /*取得profile*/
    prof = (LogManageProfile *)ProfGetInstPointByIndex(mibType, 0);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0))
    {
	websWrite(wp, "webLogEnable = \"%d\";", prof->webLogEnable);
	websWrite(wp, "QQLogEnable = \"%d\";", prof->QQLogEnable);
	websWrite(wp, "MSNLogEnable = \"%d\";", prof->MSNLogEnable);
	websWrite(wp, "mailLogEnable = \"%d\";", prof->mailLogEnable);
	websWrite(wp, "eimLogEnable = \"%d\";", prof->eimLogEnable);
#if CONFIG_XPORT_LOG 
	websWrite(wp, "luntanLogEnable = \"%d\";", prof->luntanLogEnable);
	websWrite(wp, "weiboLogEnable = \"%d\";", prof->weiboLogEnable);
	websWrite(wp, "NatLogEnable = \"%d\";", prof->NatLogEnable);
#endif
    }
    getTheErrorMsg(eid, wp, argc, argv); 
    return 0;
}
#define LOG_MAX (16384)
#if 0
static void syslog_audit(webs_t wp, char_t * path, char_t * query)
{
	FILE *fp = NULL;
	char linestr[200];
	char *log;

	websWrite(wp,
		  T
		  ("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));

	fp = popen("logread", "r");
	if (!fp) {
		websWrite(wp, "-1");
		goto error;
	}

	log = malloc(LOG_MAX * sizeof(char));
	if (!log) {
		websWrite(wp, "-1");
		goto error;
	}
	memset(log, 0, LOG_MAX);
#if 0
	fread(log, 1, LOG_MAX, fp);
#endif
	while(!feof(fp))
	{
	    memset(linestr, 0, sizeof(linestr));
	    if(fgets(linestr, sizeof(linestr), fp) != NULL)
	    {
		if((strstr(linestr,"kernel") != NULL) && (strstr(linestr,"klogd") == NULL) &&(strlen(linestr) + strlen(log) < LOG_MAX))
		{
		    strcat(log, linestr);
		    memset(lastline, 0, sizeof(lastline));
		    strcpy(lastline, linestr);
		}
	    }
	}
	if(strstr)
	websLongWrite(wp, log);

	free(log);
      error:
	if (fp)
		pclose(fp);
	websDone(wp, 200);
}
#else
#define LOG_PROC_FILE "/proc/uttErrLog"
#define UTT_APP_NAME_LEN 30
static char *getAppName(__u32 flag)
{
    PdbProfile *prof= NULL;
    static char name[UTT_APP_NAME_LEN + 1];

    /*QQ和QQ邮箱互换了索引*/
    if (flag == 1) {
	flag = 128;
    } else if (flag == 128) {
	flag = 1;
    }
    prof = (PdbProfile*)ProfGetInstPointByIndex(MIB_PROF_PDB_GLOBAL, flag - 1);
    if (ProfInstIsFree(prof) == 0) {
	memset(name, 0, sizeof(name));
	strncpy(name, prof->head.name, UTT_APP_NAME_LEN);
	SWORD_PRINTF("%d, name = %s\n", __LINE__, name);
    }
    return name;
}
static void syslog_audit(webs_t wp, char_t * path, char_t * query)
{
	FILE *fp = NULL;
	char linestr[MAX_ERROR_DATELOG_LENGTH + 1];
	char logstr[MAX_ERROR_DATELOG_LENGTH + 1];
	char *strp;
	char *strs;
	__u32 secFlag;
	char appName[UTT_APP_NAME_LEN + 1]; 
	__u32 sec;
	struct tm *date;

	websWrite(wp,
		  T
		  ("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));

	fp = fopen(LOG_PROC_FILE, "r");
	if (!fp) {
		websWrite(wp, "-1");
		goto error;
	}
#if 0
	fread(log, 1, LOG_MAX, fp);
#endif
	while(!feof(fp))
	{
	    memset(linestr, 0, sizeof(linestr));
	    memset(logstr, 0, sizeof(logstr));
	    if(fgets(linestr, sizeof(linestr), fp) != NULL)
	    {
		strp = strchr(linestr, ' ');
		if (strp) {
		    /* 分解时间 */
		    sec = strtoul(linestr, NULL, 10);
		    strs = strchr(strp + 1, ' ');
		    *strp = '\0';
		    if (strs) {
			*strs = '\0'; 
			if (strstr(strs + 1, "forbidden") != NULL) {
			    secFlag = strtoul(strp + 1, NULL, 10);
			    SWORD_PRINTF("%d, secFlag = %d\n", __LINE__, secFlag);
			    if (!secFlag) {
				return;
			    }
			    strncpy(appName, getAppName(secFlag), UTT_APP_NAME_LEN);
			} else {
			    strncpy(appName, strp + 1, UTT_APP_NAME_LEN);
			}
		    }
		    if (sec) {
			date = localtime((time_t*)&sec);
			if (strs) {
			    snprintf(logstr, MAX_ERROR_DATELOG_LENGTH, "%d-%02d-%02d %02d:%02d:%02d %s %s", 
				    (1900 + date->tm_year), (1 + date->tm_mon), date->tm_mday, 
				    date->tm_hour, date->tm_min, date->tm_sec, appName, strs+1);
			} else {
			    snprintf(logstr, MAX_ERROR_DATELOG_LENGTH, "%d-%02d-%02d %02d:%02d:%02d %s", 
				    (1900 + date->tm_year), (1 + date->tm_mon), date->tm_mday, 
				    date->tm_hour, date->tm_min, date->tm_sec, strp+1);
			}
			websWrite(wp, "%s", logstr);
		    }
		}
#if 0
		websWrite(wp, "%s\n", linestr);
#endif
	    }
	}
error:
	if (fp) {
	    fclose(fp);
	}
	websDone(wp, 200);
}
#endif
#if 0
static void DelHistoryLog(webs_t wp, char_t * path, char_t * query)
{
    doSystem("killall syslogd");
    SWORD_PRINTF("%s-%d, killall syslogd\n", __func__, __LINE__);
    doSystem("syslogd -C8");
    SWORD_PRINTF("%s-%d, syslogd -C8\n", __func__, __LINE__);
    websRedirect(wp, "NetActionAudit.asp");
}
#else
extern int uttNfNlSock;
/*
 * 清除
 */
static void DelHistoryLog(webs_t wp, char_t * path, char_t * query)
{
    int ret = 0;
    UttNlNfGeneral nlMsg;
    memset(&nlMsg, 0, sizeof(nlMsg));
    nlMsg.msgHead.nlType = UTT_NLNF_LOG;
    /*增加*/
    nlMsg.confType = UTT_NFCONF_DEL;
    ret = uttNfNlSend(uttNfNlSock,(char*)&nlMsg, sizeof(nlMsg), 0 ,0 );/*发送消息给内核*/
    websRedirect(wp, "NetActionAudit.asp");
    return;
}
#endif
#endif
