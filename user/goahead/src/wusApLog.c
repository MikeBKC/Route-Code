
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

#if defined CONFIG_SWITCH_EXT || (FEATURE_AC == FYES)
#include    <switch.h>
#include    <sw_cfg.h>
#include    <urcp_ipc.h>
#endif

#if (AP_LOG_MANAGE == FYES)

#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

static int getApLogManageConfig(int eid, webs_t wp, int argc, char **argv); 
static void formApLogManageConfig(webs_t wp, char_t *path, char_t *query);
static void apSyslog_audit(webs_t wp, char_t * path, char_t * query);
static void apDelHistoryLog(webs_t wp, char_t * path, char_t * query);
/********************************************************************
 * 函数：
 * 功能：
 * 创建：
 * 参数：
 * 返回：
 * 输出：
 * 作者：
 ********************************************************************/
extern void websDefineApLogManage(void)
{
	websFormDefine(T("apLogManageConfig"), formApLogManageConfig);
	websFormDefine(T("apSyslog_audit"), apSyslog_audit);
	websFormDefine(T("apDelHistoryLog"), apDelHistoryLog);
	websAspDefine(T("aspOutputApLogManageConfig"), getApLogManageConfig);
}
/********************************************************************
 * 函数：
 * 功能：
 * 创建：
 * 参数：
 * 返回：
 * 输出：
 * 作者：
 ********************************************************************/
static void formApLogManageConfig(webs_t wp, char_t *path, char_t *query)
{
	MibProfileType mibType = MIB_PROF_AP_LOG_MANAGE;
	ApLogManageProfile *prof = NULL;

	char_t *apLinkLogEnable = NULL;
	char_t *apUserLogEnable = NULL;
	char_t *apUserNumEnable = NULL;

	/*从页面空间获取值*/
	apLinkLogEnable = websGetVar(wp, T("apLinkLog"), T(""));
	apUserLogEnable = websGetVar(wp, T("apUserLog"), T(""));
	apUserNumEnable = websGetVar(wp, T("apUserNum"), T(""));

	/*取得profile*/
	prof = (ApLogManageProfile *)ProfGetInstPointByIndex(mibType, 0);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0))/*正在使用*/ {
		if(strcmp(apLinkLogEnable, "on") == 0)/*开关打开*/
		{
			prof->apLogType[APLINKLOG] = LOG_ENABLE;
		}
		else
		{
			prof->apLogType[APLINKLOG] = LOG_DISABLE;
		}
		if(strcmp(apUserLogEnable, "on") == 0)
		{
			prof->apLogType[APUSERLOG] = LOG_ENABLE;
		}
		else
		{
			prof->apLogType[APUSERLOG] = LOG_DISABLE;
		}
		if(strcmp(apUserNumEnable, "on") == 0)
		{
			prof->apLogType[APUSERNUM] = LOG_ENABLE;
		}
		else
		{
			prof->apLogType[APUSERNUM] = LOG_DISABLE;
		}
	}


	nvramWriteCommit(); /*save*/
	websRedirect(wp, "apLogManage.asp");

	return;
}

static int getApLogManageConfig(int eid, webs_t wp, int argc, char **argv)
{
	MibProfileType mibType = MIB_PROF_AP_LOG_MANAGE;
	ApLogManageProfile *prof = NULL;

	websWrite(wp, "var ApLinkLogEnable = \"\";");
	websWrite(wp, "var ApUserLogEnable = \"\";");
	websWrite(wp, "var ApUserNumEnable = \"\";");

	/*取得profile*/
	prof = (ApLogManageProfile *)ProfGetInstPointByIndex(mibType, 0);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
		websWrite(wp, "ApLinkLogEnable = \"%d\";", prof->apLogType[APLINKLOG]);
		websWrite(wp, "ApUserLogEnable = \"%d\";", prof->apLogType[APUSERLOG]);
		websWrite(wp, "ApUserNumEnable = \"%d\";", prof->apLogType[APUSERNUM]);
	}

	getTheErrorMsg(eid, wp, argc, argv);
	return 0;
}

static void apSyslog_audit(webs_t wp, char_t * path, char_t * query)
{
	char linestr[MAX_LOG_LENGTH + 1];
	char logstr[MAX_LOG_LENGTH + 1];
	urcp_shm_t *mem_shadow = NULL;
	ap_log *tmp = NULL;
	unsigned long sec;
	struct tm *date;
	char *strp;
	int i = 0;

	websWrite(wp,
			T
			("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));

	mem_shadow = get_shm_ptr();
	if (NULL != mem_shadow) {
		tmp = &(mem_shadow->ap_msg);

		for (i = tmp->current_msg_index; i < NUM_OF_AP_LOG; i++) {
			if (0 == strncmp(&(tmp->msg[i]), "\0", MAX_LOG_LENGTH)) {
				break;
			}

			memset(linestr, 0, sizeof(linestr));
			memset(logstr, 0, sizeof(logstr));

			memcpy(linestr, &(tmp->msg[i]), MAX_LOG_LENGTH);
			strp = strchr(linestr, ' ');
		    sec = strtoul(linestr, NULL, 10);

			if (sec) {
				date = localtime((time_t*)&sec);
				snprintf(logstr, MAX_LOG_LENGTH, "%d-%02d-%02d %02d:%02d:%02d %s",
						(1900 + date->tm_year), (1 + date->tm_mon), date->tm_mday,
						date->tm_hour, date->tm_min, date->tm_sec, strp+1);
				websWrite(wp, "%s", logstr);
			}
		}

		for (i = 0; i <= tmp->current_msg_index; i++) {
			if (0 == strncmp(&(tmp->msg[i]), "\0", MAX_LOG_LENGTH)) {
				break;
			}

			memset(linestr, 0, sizeof(linestr));
			memset(logstr, 0, sizeof(logstr));

			memcpy(linestr, &(tmp->msg[i]), MAX_LOG_LENGTH);
			strp = strchr(linestr, ' ');
		    sec = strtoul(linestr, NULL, 10);

			if (sec) {
				date = localtime((time_t*)&sec);
				snprintf(logstr, MAX_LOG_LENGTH, "%d-%02d-%02d %02d:%02d:%02d %s",
						(1900 + date->tm_year), (1 + date->tm_mon), date->tm_mday,
						date->tm_hour, date->tm_min, date->tm_sec, strp+1);
				websWrite(wp, "%s", logstr);
			}
		}
	}

	websDone(wp, 200);
}

/*
 * 清除
 */
static void apDelHistoryLog(webs_t wp, char_t * path, char_t * query)
{
    urcp_shm_t *mem_shadow=NULL;
	ap_log *tmp = NULL;

    mem_shadow = get_shm_ptr();

	if (NULL != mem_shadow) {
        uttSemP(SEM_AP_LOG, 0);/*与urcp冲突*/
		tmp = &(mem_shadow->ap_msg);

		memset(tmp, 0, sizeof(ap_log));
        uttSemV(SEM_AP_LOG, 0);/*与urcp冲突*/
	}

    websRedirect(wp, "apLog.asp");
	return ;
}
#endif
