#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"

#if (ANTI_SNIFFER == FYES)
static int aspAntiNetSniffer(int eid, webs_t wp, int argc, char_t **argv)
{
    InterfaceProfile *profIf = NULL;
    MibProfileType mibType = MIB_PROF_INTERFACE;

    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, 1);
    websWrite(wp, "var antiSniffer=%d;", profIf->antiSnifEn);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

static void formConfigAntiNetSniffer(webs_t wp, char_t *path, char_t *query)
{
    int max = 0, i = 0;
    char *antiSniffer = NULL;
    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *prof = NULL;
    FunEnableEnum antiSniffEn = FUN_DISABLE;
    struProfAlloc *profhead = NULL;

    antiSniffer = websGetVar(wp, "AntiNetSnifer", T(""));

    max = getWanSlotCount();

    if(strcmp(antiSniffer, "on") == 0) {
        antiSniffEn = FUN_ENABLE;
    }

    for(i = 1; i <= max; i++) {
        prof = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, i);
        if(prof->antiSnifEn != antiSniffEn) {
            ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, i, &profhead);
            prof->antiSnifEn = antiSniffEn;
        }
    }

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    websRedirect(wp, "AntiNetSnifer.asp");
    return;
}

void formDefineAntiNetSniffer(void)
{
    websAspDefine(T("aspAntiNetSniffer"), aspAntiNetSniffer);
    websFormDefine(T("ConfigAntiNetSniffer"), formConfigAntiNetSniffer);
}
#endif
