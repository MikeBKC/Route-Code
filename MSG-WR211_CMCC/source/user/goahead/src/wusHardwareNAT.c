#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"

#if (HARDWARE_NAT == FYES)
static int aspOutHardwareNAT(int eid, webs_t wp, int argc, char_t **argv)
{
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    profSys = ProfGetInstPointByIndex(mibType, 0);

    if((profSys != NULL) && (ProfInstIsFree((Pointer)profSys) == 0))
    {
	websWrite(wp, "var hardwareNAT=%d;", profSys->hardwareNATEn);
    }
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

static void formConfigHardwareNAT(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    struProfAlloc *profhead = NULL;
    char_t *hardwareNATEn = NULL;
    hardwareNATEn = websGetVar(wp, "HardwareNAT", T(""));

    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);

    if((profSys != NULL) && (ProfInstIsFree((Pointer)profSys) == 0)) 
    {
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
	if (strcmp(hardwareNATEn, "on") == 0) 
	{
	    profSys->hardwareNATEn = FUN_ENABLE;
	} 
	else 
	{
	    profSys->hardwareNATEn = FUN_DISABLE;
	}
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
    }
    websRedirect(wp, "HardwareNAT.asp");
    return;
}

void formDefineHardwareNAT(void)
{
    websAspDefine(T("aspOutHardwareNAT"), aspOutHardwareNAT);
    websFormDefine(T("ConfigHardwareNAT"), formConfigHardwareNAT);
}
#endif
