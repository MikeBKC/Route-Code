#include	"uttMachine.h"
#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include	"argcmd.h"

#if (SYSLOG == FYES)
#define SWORD_DEBUG 0
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

/********************************************************************
 * ������ aspSyslogConf
 * ���ܣ� ҳ�����
 * ������ 2012-10-16
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int aspSyslogConf(int eid, webs_t wp, int argc, char_t **argv) 
{
    SyslogProfile *profSyslog = NULL;
    MibProfileType mibType = MIB_PROF_SYSLOG;
    profSyslog = (SyslogProfile*)ProfGetInstPointByIndex(mibType, 0);

    if (profSyslog->head.active == TRUE) {
	websWrite(wp, "var SyslogEn=\"on\";");
    } else {
	websWrite(wp, "var SyslogEn=\"off\";");
    }
    websWrite(wp, "var ServerIp=\"%s\";", profSyslog->SyslogIp);
    websWrite(wp, "var SyslogPort=%d;", profSyslog->SyslogPort);
    websWrite(wp, "var SyslogType=\"%s\";", profSyslog->SyslogType);
#if (SYSLOG_HEARTBEAT == FYES)
    websWrite(wp, "var SyslogInterval=\"%d\";", profSyslog->SyslogInterval);
#else
    websWrite(wp, "var SyslogInterval=\"\";");
#endif

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

/********************************************************************
 * ������ formSyslogConf
 * ���ܣ� ҳ�汣��
 * ������ 2012-10-16
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static void formSyslogConf(webs_t wp, char_t *path, char_t *query) 
{
    MibProfileType mibType = MIB_PROF_SYSLOG;
    SyslogProfile *profSyslog = NULL;
    struProfAlloc *profhead = NULL;
    char *enable = NULL, *SyslogIp = NULL, *SyslogPort = NULL, *SyslogType = NULL,*SyslogInterval;

    enable = websGetVar(wp, "SyslogEnable", T(""));
    SyslogIp = websGetVar(wp, "ServerIp", T(""));
    SyslogPort = websGetVar(wp, "ServerPort", T(""));
    SyslogType = websGetVar(wp, "SyslogType", T(""));
#if (SYSLOG_HEARTBEAT == FYES)
    SyslogInterval = websGetVar(wp, "SyslogInterval", T(""));
#endif
    SWORD_PRINTF("%s------%d, enable = %s, SyslogIp = %s, SyslogPort = %s, SyslogType = %s\n", __func__, __LINE__, enable, SyslogIp, SyslogPort, SyslogType);

    profSyslog = (SyslogProfile *)ProfGetInstPointByIndex(mibType, 0);

    if((profSyslog != NULL) && (ProfInstIsFree(profSyslog) == 0)) 
    {
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
	if (strcmp(enable, "on") == 0) 
	{/*����Syslog�ű���ip����Ϣ*/
	    profSyslog->head.active = TRUE;
	    strcpy(profSyslog->SyslogIp, SyslogIp);
	    profSyslog->SyslogPort = (Uint32)strtoul(SyslogPort, NULL, 10);
#if (SYSLOG_HEARTBEAT == FYES)
	    profSyslog->SyslogInterval = (Uint32)strtoul(SyslogInterval, NULL, 10);
#endif
	    strcpy(profSyslog->SyslogType, SyslogType);
	} 
	else 
	{
	    profSyslog->head.active = FALSE;
	}

	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
    }
    websRedirect(wp, "Syslog.asp");
    return;
}

/********************************************************************
 * ������ formDefineSyslog
 * ���ܣ� ����ҳ��ӿں���
 * ������ 2012-10-16
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
void formDefineSyslog(void)
{
    websAspDefine(T("aspSyslogConf"), aspSyslogConf);	    /*define asp function*/
    websFormDefine(T("formSyslogConf"), formSyslogConf);    /*define form function*/
}
#endif
