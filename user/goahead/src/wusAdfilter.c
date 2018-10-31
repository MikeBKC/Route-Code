#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"

#if (ADFILTER == FYES)
#define DEBUG_ADFILTER 0

extern void formDefineAdfilter(void);
extern void adfilter_Config (int sw, int updateType);

#if DEBUG_ADFILTER
static void formEnableAdfilter(webs_t wp, char_t *path, char_t *query);
static void formDisableAdfilter(webs_t wp, char_t *path, char_t *query);
static void formEnableAdfilter(webs_t wp, char_t *path, char_t *query)
{
    adfilter_Config(1,0);
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/html\nPragma: no-cache\nCache-Control: no-cache\n\n"));
    websWrite(wp, T("Enable Sucess"));
    websDone(wp, 200);
}

static void formDisableAdfilter(webs_t wp, char_t *path, char_t *query)
{
    adfilter_Config(0,0);
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/html\nPragma: no-cache\nCache-Control: no-cache\n\n"));
    websWrite(wp, T("Disable Sucess"));
    websDone(wp, 200);
}
#endif

extern void adfilter_Config (int sw, int updateType)
{
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    AdfilterProfile *prof = NULL;

    prof = (AdfilterProfile *)ProfGetInstPointByIndex(MIB_PROF_ADFILTER, 0);

    if (prof == NULL) {
	return;
    }
    profAction = PROF_CHANGE_EDIT;
    uttSemP(SEM_ADFILTER,0);
    ProfBackupByIndex(MIB_PROF_ADFILTER, profAction, 0, &profhead);
#if (APP_POLICY_REMOTE_UPDATE == FYES)
    prof->updateType = updateType;
#endif
    if (sw == 1) {
	prof->head.active = TRUE;
    } else {
	prof->head.active = FALSE;
    }
    uttSemV(SEM_ADFILTER,0);
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
#if DEBUG_ADFILTER
    nvramWriteCommit();
#endif
}

void formDefineAdfilter(void)
{
#if DEBUG_ADFILTER
    websFormDefine(T("enableAdfilter"), formEnableAdfilter);
    websFormDefine(T("disableAdfilter"), formDisableAdfilter);
#endif
}
#endif
