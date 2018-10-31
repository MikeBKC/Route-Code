#include	"uttMachine.h"
#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include	"argcmd.h"

#if (P2P_LIMIT == FYES)
static int aspOutP2PLimitConf(int eid, webs_t wp, int argc, char_t **argv);
static void formP2PLimitConfig(webs_t wp, char_t *path, char_t *query);
void formDefineP2PLimit(void);


/********************************************************************
 * �������ܣ����P2P����ҳ����Ϣ
 * ����ʱ�䣺2013-3-6
 ********************************************************************/ 
static int aspOutP2PLimitConf(int eid, webs_t wp, int argc, char_t **argv) 
{
    P2PLimitProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_P2P_LIMIT;
    unsigned char d_temp;
    char data[16];

    prof = (P2PLimitProfile*)ProfGetInstPointByIndex(mibType, 0);

    if (prof->head.active == TRUE) {
    websWrite(wp, "var P2PLimitEn=\"on\";");
    } else {
    websWrite(wp, "var P2PLimitEn=\"off\";");
    }
    websWrite(wp, "var policy=\"%d\";", prof->limitPolicy);
    websWrite(wp, "var uRate=\"%d\";", prof->uRate);/*�ϴ�����*/
    websWrite(wp, "var dRate=\"%d\";", prof->dRate);/*��������*/
    websWrite(wp, "var p2p_timestart=\"%s\";", prof->timeStart);
    websWrite(wp, "var p2p_timestop=\"%s\";", prof->timeStop);
#if (IP_GRP == FYES)
    websWrite(wp,"var ipGrpNames = \"%s\";",prof->ipGrpName);
#endif
    d_temp = prof->day;
    memset(data, 0, sizeof(data));
    Byte2Days(d_temp, data);
    websWrite(wp, "var p2p_days=\"%s\";", data);

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

/********************************************************************
 * �������ܣ�����ҳ���ύ
 * ����ʱ�䣺 2013-3-6
 ********************************************************************/ 
static void formP2PLimitConfig(webs_t wp, char_t *path, char_t *query) 
{
    MibProfileType mibType = MIB_PROF_P2P_LIMIT;
    P2PLimitProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    char *enable = NULL, *policyStr = NULL, *uRateStr = NULL, *dRateStr = NULL;
    char *weekStr = NULL, *timeStartStr = NULL, *timeStopStr = NULL;
    char *groupName = NULL;

    enable = websGetVar(wp, "P2PLimitEnable", T(""));
#if (IP_GRP == FYES)
    groupName = websGetVar(wp, "except", T(""));
#endif
    policyStr = websGetVar(wp, "policy", T(""));
    dRateStr = websGetVar(wp, T("dRate"), (char *)NULL);/*��������*/
    uRateStr = websGetVar(wp, T("uRate"), (char *)NULL);/*�ϴ�����*/
    weekStr = websGetVar(wp,T("day"), (char *)NULL);/*����*/
    timeStartStr = websGetVar(wp,T("timestart"), (char *)NULL);/*��ʼʱ��*/
    timeStopStr = websGetVar(wp,T("timestop"), (char *)NULL);/*����ʱ��*/

    prof = (P2PLimitProfile *)ProfGetInstPointByIndex(mibType, 0);

    if((prof != NULL) && (ProfInstIsFree(prof) == 0)) 
    {
    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
    if (strcmp(enable, "on") == 0) 
    {
        prof->head.active = TRUE;
    } 
    else 
    {
        prof->head.active = FALSE;
    }
    prof->limitPolicy = strtoul(policyStr, NULL, 0);
    prof->uRate = strtoul(uRateStr, NULL, 0);/*�ϴ�����*/
    prof->dRate = strtoul(dRateStr, NULL, 0);/*��������*/
#if (IP_GRP == FYES)
    strcpy(prof->ipGrpName, groupName);
#endif
    prof->day= days2Byte(weekStr); 
    strcpy(prof->timeStart, timeStartStr); 
    strcpy(prof->timeStop, timeStopStr); 
    
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();/*����flash*/
    }
    websRedirect(wp, "P2PLimit.asp");
    return;
}

/********************************************************************
 * �������ܣ� formDefineP2PLimit
 * ����ʱ�䣺 2013-3-6
 ********************************************************************/ 
void formDefineP2PLimit(void)
{
    websAspDefine(T("aspOutP2PLimitConf"), aspOutP2PLimitConf);	    /*define asp function*/
    websFormDefine(T("formP2PLimitConfig"), formP2PLimitConfig);    /*define form function*/
}
#endif
