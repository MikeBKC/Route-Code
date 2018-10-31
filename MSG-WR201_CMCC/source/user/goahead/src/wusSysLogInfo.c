

/*
 * �ļ���:
 *     wusSysLogInfo.c
 *
 * ����:
 *    ϵͳ��־�����ļ�
 *    
 * ���ߣ�
 *     �｣��
 *
 * ά���ˣ�
 *     �｣��
 *
 * �����
 *     1. ��ʼ�汾:2012-05-24
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
#if (SYS_LOG_INFO == FYES)

#define SWORD_DEBUG 0
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

static int getSysLogInfoConfig(int eid, webs_t wp, int argc, char **argv); 
static void formSysLogInfoConfig(webs_t wp, char_t *path, char_t *query);
static void SysLogMess(webs_t wp, char_t * path, char_t * query);
static void DelHistoryLog(webs_t wp, char_t * path, char_t * query);
/********************************************************************
 * ������ websDefineWebAuth
 * ���ܣ� ҳ���뽻��ת������
 * ������ 2013-05-03
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
extern void websDefineSysLogInfo(void)
{
	websFormDefine(T("SysLogInfoConfig"), formSysLogInfoConfig);
	websFormDefine(T("SysLogMess"), SysLogMess);
	websFormDefine(T("DelHistorySysLog"), DelHistoryLog);
	websAspDefine(T("aspOutputSysLogInfoConfig"), getSysLogInfoConfig);     
}
/********************************************************************
 * ������ formSysLogInfoConfig
 * ���ܣ� ϵͳ��־ҳ����뽻��
 * ������ 2013-05-03
 * ������ �����̶��������
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static void formSysLogInfoConfig(webs_t wp, char_t *path, char_t *query)
{
	MibProfileType mibType = MIB_PROF_SYS_LOG_INFO;
	SysLogInfoProfile *prof = NULL;
	struProfAlloc *profhead = NULL;

#ifdef CONFIG_UTT_NOTICE_LOG
	char_t *NoticeLogEnable = NULL;
#endif
#ifdef CONFIG_UTT_ARP_COLLISION
	char_t *ArpLogEnable = NULL;
#endif
#ifdef CONFIG_UTT_DHCP_COLLISION
	char_t *DhcpLogEnable = NULL;
#endif
	char_t *PppoeLogEnable = NULL;

	/*��ҳ��ռ��ȡֵ*/
#ifdef CONFIG_UTT_NOTICE_LOG
	NoticeLogEnable = websGetVar(wp, T("NoticeLog"), T(""));
#endif
#ifdef CONFIG_UTT_ARP_COLLISION
	ArpLogEnable = websGetVar(wp, T("ArpLog"), T(""));
#endif
#ifdef CONFIG_UTT_DHCP_COLLISION
	DhcpLogEnable = websGetVar(wp, T("DhcpLog"), T(""));
#endif
	PppoeLogEnable = websGetVar(wp, T("PppoeLog"), T(""));

	/*ȡ��profile*/
	prof = (SysLogInfoProfile *)ProfGetInstPointByIndex(mibType, 0);
	SWORD_PRINTF("%s-%d, NoticeLogEnable = %s, ArpLogEnable = %s, DhcpLogEnable = %s, PppoeLogEnable = %s\n",
			__func__, __LINE__, NoticeLogEnable, ArpLogEnable, DhcpLogEnable, PppoeLogEnable);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)){
		/*����ʹ��*/
		SWORD_PRINTF("%s-%d\n", __func__, __LINE__);
		ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
#ifdef CONFIG_UTT_NOTICE_LOG
		if(strcmp(NoticeLogEnable, "on") == 0) {
			/*���ش�*/
			prof->NoticeLogEnable = LOG_ENABLE;
		} else {
			prof->NoticeLogEnable = LOG_DISABLE;
		}
#endif
#ifdef CONFIG_UTT_ARP_COLLISION
		if(strcmp(ArpLogEnable, "on") == 0) {
			prof->ArpLogEnable = LOG_ENABLE;
		} else {
			prof->ArpLogEnable = LOG_DISABLE;
		}
#endif
#ifdef CONFIG_UTT_DHCP_COLLISION
		if(strcmp(DhcpLogEnable, "on") == 0) {
			prof->DhcpLogEnable = LOG_ENABLE;
		} else {
			prof->DhcpLogEnable = LOG_DISABLE;
		}
#endif
		if(strcmp(PppoeLogEnable, "on") == 0) {
			prof->PppoeLogEnable = LOG_ENABLE;
		} else {
			prof->PppoeLogEnable = LOG_DISABLE;
		}
		prof->head.active = TRUE;
		ProfUpdate(profhead);/*����*/
		ProfFreeAllocList(profhead);
		nvramWriteCommit(); /*save*/
	}

	websRedirect(wp, "SysLogInfo_edit.asp");
	return;	   
}
/********************************************************************
 * ������ getSysLogInfoConfig
 * ���ܣ� ϵͳ��־д��ҳ�潻��
 * ������ 2013-05-03
 * ������ �����̶��������
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int getSysLogInfoConfig(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_SYS_LOG_INFO;
    SysLogInfoProfile *prof = NULL;

    websWrite(wp, "var NoticeLogEnable = \"\";");
    websWrite(wp, "var ArpLogEnable = \"\";");
    websWrite(wp, "var DhcpLogEnable = \"\";");
    websWrite(wp, "var PppoeLogEnable = \"\";");

    /*ȡ��profile*/
    prof = (SysLogInfoProfile *)ProfGetInstPointByIndex(mibType, 0);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
#ifdef CONFIG_UTT_NOTICE_LOG
	websWrite(wp, "NoticeLogEnable = \"%d\";", prof->NoticeLogEnable);
#endif
#ifdef CONFIG_UTT_ARP_COLLISION
	websWrite(wp, "ArpLogEnable = \"%d\";", prof->ArpLogEnable);
#endif
#ifdef CONFIG_UTT_DHCP_COLLISION
	websWrite(wp, "DhcpLogEnable = \"%d\";", prof->DhcpLogEnable);
#endif
	websWrite(wp, "PppoeLogEnable = \"%d\";", prof->PppoeLogEnable);
    }
    getTheErrorMsg(eid, wp, argc, argv); 
    return 0;
}

#define LOG_MAX (16384)
/********************************************************************
 * ������ SysLogMess
 * ���ܣ� ϵͳ��־��Ϣ���
 * ������ 2013-05-03
 * ������ �����̶��������
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static void SysLogMess(webs_t wp, char_t * path, char_t * query)
{
	FILE *fp = NULL;
	char *log;

	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));

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
	fread(log, 1, LOG_MAX, fp);

	websLongWrite(wp, log);

	free(log);
error:
	if (fp)
		pclose(fp);
	websDone(wp, 200);
}

/********************************************************************
 * ������ DelHistoryLog
 * ���ܣ� ���ϵͳ��־��Ϣ
 * ������ 2013-05-03
 * ������ �����̶��������
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static void DelHistoryLog(webs_t wp, char_t * path, char_t * query)
{
	doSystem("syslog.sh");
	websRedirect(wp, "SysLogInfo.asp");
}
#endif
