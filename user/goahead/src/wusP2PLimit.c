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
 * 函数功能：输出P2P限速页面信息
 * 创建时间：2013-3-6
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
    websWrite(wp, "var uRate=\"%d\";", prof->uRate);/*上传速率*/
    websWrite(wp, "var dRate=\"%d\";", prof->dRate);/*下载速率*/
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
 * 函数功能：处理页面提交
 * 创建时间： 2013-3-6
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
    dRateStr = websGetVar(wp, T("dRate"), (char *)NULL);/*下载速率*/
    uRateStr = websGetVar(wp, T("uRate"), (char *)NULL);/*上传速率*/
    weekStr = websGetVar(wp,T("day"), (char *)NULL);/*星期*/
    timeStartStr = websGetVar(wp,T("timestart"), (char *)NULL);/*开始时间*/
    timeStopStr = websGetVar(wp,T("timestop"), (char *)NULL);/*结束时间*/

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
    prof->uRate = strtoul(uRateStr, NULL, 0);/*上传速率*/
    prof->dRate = strtoul(dRateStr, NULL, 0);/*下载速率*/
#if (IP_GRP == FYES)
    strcpy(prof->ipGrpName, groupName);
#endif
    prof->day= days2Byte(weekStr); 
    strcpy(prof->timeStart, timeStartStr); 
    strcpy(prof->timeStop, timeStopStr); 
    
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();/*保存flash*/
    }
    websRedirect(wp, "P2PLimit.asp");
    return;
}

/********************************************************************
 * 函数功能： formDefineP2PLimit
 * 创建时间： 2013-3-6
 ********************************************************************/ 
void formDefineP2PLimit(void)
{
    websAspDefine(T("aspOutP2PLimitConf"), aspOutP2PLimitConf);	    /*define asp function*/
    websFormDefine(T("formP2PLimitConfig"), formP2PLimitConfig);    /*define form function*/
}
#endif
