#include "uttMachine.h"
#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include	"argcmd.h"

#if (CONNECT_LIMIT == FYES)
/* 页面输出 */
static int aspConnLimitData(int eid, webs_t wp, int argc, char_t **argv) {
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);

    if (profSys->connLimit.connLimitEn == FUN_ENABLE) {
	websWrite(wp, "var connLimitSw=\"on\";");
    } else {
	websWrite(wp, "var connLimitSw=\"off\";");
    }
    websWrite(wp, "var totalLimit=%d;", profSys->connLimit.totalConnCount);
    websWrite(wp, "var tcpLimit=%d;", profSys->connLimit.TCPConnCount);
    websWrite(wp, "var udpLimit=%d;", profSys->connLimit.UDPConnCount);
    websWrite(wp, "var icmpLimit=%d;", profSys->connLimit.ICMPConnCount);
    websWrite(wp, "var maxSess=%d;", MAX_NAT_SESSIONS);

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

/* 页面保存 */
static void formConnLimit(webs_t wp, char_t *path, char_t *query) {
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    struProfAlloc *profhead = NULL;
    char *enable = NULL, *totalCnt = NULL, *tcpCnt = NULL, *udpCnt = NULL, *icmpCnt = NULL;

    enable = websGetVar(wp, "enable", T(""));
    totalCnt = websGetVar(wp, "totalCnt", T(""));
    tcpCnt = websGetVar(wp, "tcp", T(""));
    udpCnt = websGetVar(wp, "udp", T(""));
    icmpCnt = websGetVar(wp, "icmp", T(""));

    if ((!enable) || (!totalCnt) || (!tcpCnt) || (!udpCnt) || (!icmpCnt)) {
	setTheErrorMsg(T("操作失败！"));
	websRedirect(wp, "ConnectionLimit.asp");
	return;
    }

    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);
    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);

    if (strcmp(enable, "on") == 0) {
	profSys->connLimit.connLimitEn = FUN_ENABLE;
	profSys->connLimit.totalConnCount = strtoul(totalCnt, NULL, 10);
	profSys->connLimit.TCPConnCount = strtoul(tcpCnt, NULL, 10);
	profSys->connLimit.UDPConnCount = strtoul(udpCnt, NULL, 10);
	profSys->connLimit.ICMPConnCount = strtoul(icmpCnt, NULL, 10);
    } else {
	profSys->connLimit.connLimitEn = FUN_DISABLE;
    }

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    websRedirect(wp, "ConnectionLimit.asp");
    return;
}

/* 定义页面接口函数 */
void formDefineConnLimit(void)
{
    websAspDefine(T("aspConnLimitData"), aspConnLimitData);	    /*define asp function*/
    websFormDefine(T("formConnLimit"), formConnLimit);    /*define form function*/
}
#endif
