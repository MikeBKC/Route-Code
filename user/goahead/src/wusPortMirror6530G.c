#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"

#if (PORT_MIRROR_6530G == FYES)

#ifdef  MIRROR_DEBUG
#define MIRROR_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define MIRROR_PRINTF(fmt, args...)
#endif

/***********************
 * aspPortMirrorData_6530G
 * guo.deyuan
 * 2012-05-22 
 */
static int aspPortMirrorData_6530G(int eid, webs_t wp, int argc, char_t **argv)
{
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;

    /*get SystemProfile Pointer*/
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);
    /* get Mirror_6530G_En value 
     * write value to web
     */
    if(profSys->Mirror_6530G_En == FUN_ENABLE)
    {
	websWrite(wp, "var MirrorEnable=\"%s\";", T("on"));
	MIRROR_PRINTF("%s-%d:MirrorEnable = %d\n",__FUNCTION__,__LINE__,profSys->Mirror_6530G_En);
    }
    else
    {
	websWrite(wp, "var MirrorEnable=\"%s\";",T("off"));
	MIRROR_PRINTF("%s-%d:MirrorEnable = %d\n",__FUNCTION__,__LINE__,profSys->Mirror_6530G_En);
    }
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

/**********************
 * formConfigMirrorGlobal
 * 2012-05-21
 * guo.deyuan
 */
static void formConfigMirrorGlobal(webs_t wp, char_t *path, char_t *query)
{
    char *portMirror = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    FunEnableEnum enable = FUN_DISABLE;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;

    /* get web value */
    portMirror = websGetVar(wp, "MirrorEnable", T(""));
    MIRROR_PRINTF("%s-%d:MirrorEnable = %s\n",__FUNCTION__,__LINE__,portMirror);
    
    /* get SystemProfile */
    profAction = PROF_CHANGE_EDIT;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);
    ProfBackupByIndex(mibType, profAction, 0, &profhead);
    
    /* compare web value 
     * if value on
     * set Mirror_6530G_En enable, or set disable
     */
    if(strcmp(portMirror, "on") == 0) {
        enable = FUN_ENABLE;
	MIRROR_PRINTF("%s-%d:enable = %d\n",__FUNCTION__,__LINE__,enable);
    }
    if(profSys->Mirror_6530G_En != enable)
    {
	profSys->Mirror_6530G_En = enable;
	MIRROR_PRINTF("%s-%d:set Mirror_6530G_En = %d\n",__FUNCTION__,__LINE__,profSys->Mirror_6530G_En);
    }
    
    /* update systemProfile
     * and write to flash
     */
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    websRedirect(wp, "PortMirror6530G.asp");
    return;
}

/******************************
 * formDefinePortMirror
 * 2012-05-21
 * guo.deyuan 
 */
void formDefinePortMirror6530G(void)
{
    websAspDefine(T("aspPortMirrorData_6530G"), aspPortMirrorData_6530G);	    /*define asp function*/
    websFormDefine(T("ConfigMirrorGlobal"), formConfigMirrorGlobal);    /*define form function*/
}
#endif
