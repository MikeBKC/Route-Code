#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"

#if (PORT_MIRROR == FYES)
//#define PORT_MIRROR_DBG

#ifdef  PORT_MIRROR_DBG
#define PORT_MIRROR_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define PORT_MIRROR_PRINTF(fmt, args...)
#endif

/***********************
 * aspPortMirrorData
 * guo.deyuan
 * 2012-04-11 
 */
static int aspPortMirrorData(int eid, webs_t wp, int argc, char_t **argv)
{
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;

    /*get SystemProfile Pointer*/
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);

#if defined(CONFIG_RTL8367M) || defined(CONFIG_RALINK_RT3883) || defined(CONFIG_RTL8306E) || defined(CONFIG_RTL8367RB)||defined(CONFIG_RALINK_MT7620)||defined(CONFIG_RALINK_MT7621)
    websWrite(wp, "var monitorPort=%u;", profSys->mirror.monitorPort);
    websWrite(wp, "var sourcePort=%u;", profSys->mirror.sourcePort);
    websWrite(wp,"var maxLanPort =%d;",profSys->lanPortCount);
#endif

   /* get portMirrorEn value 
     * write value to web
     */
    if(profSys->portMirrorEn == FUN_ENABLE)
    {
	websWrite(wp, "var portMirror=\"%s\";", T("on"));
	PORT_MIRROR_PRINTF("%s-%d:portMirror = %d\n",__FUNCTION__,__LINE__,profSys->portMirrorEn);
    }
    else
    {
	websWrite(wp, "var portMirror=\"%s\";",T("off"));
	PORT_MIRROR_PRINTF("%s-%d:portMirror = %d\n",__FUNCTION__,__LINE__,profSys->portMirrorEn);
    }

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

/**********************
 * formConfigPortMirror
 * 2012-04-11
 * guo.deyuan
 */
static void formConfigPortMirror(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    FunEnableEnum enable = FUN_DISABLE;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    char *portMirror = NULL;

    /* get web value */
    portMirror = websGetVar(wp, "PortMirrorEn", T(""));
    PORT_MIRROR_PRINTF("%s-%d:portMirrorEn = %s\n",__FUNCTION__,__LINE__,portMirror);
    /* get SystemProfile */
    profAction = PROF_CHANGE_EDIT;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);
    ProfBackupByIndex(mibType, profAction, 0, &profhead);
 
#if defined(CONFIG_RTL8367M) || defined(CONFIG_RTL8306E) || defined(CONFIG_RTL8367RB)
    profSys->mirror.monitorPort = strtoul(websGetVar(wp, "monitorPort",T("")),0, 10);
#if defined(CONFIG_UTT_NV840E) || defined(CONFIG_UTT_NV800) || defined(CONFIG_UTT_NV840D) || defined(CONFIG_UTT_NV840DV3)||defined(CONFIG_UTT_NV840EV3)
    profSys->mirror.sourcePort = strtoul(websGetVar(wp, "hidSourPort",T("")),0, 10);
#else
    profSys->mirror.sourcePort = strtoul(websGetVar(wp, "sourcePort",T("")),0, 10);
#endif
#endif
#if defined(CONFIG_RALINK_MT7620) || defined(CONFIG_RALINK_MT7621)
    profSys->mirror.monitorPort = strtoul(websGetVar(wp, "monitorPort",T("")),0, 10);
#endif
    /* compare web value 
     * if value on
     * set portMirrorEn enable, or set disable
     */
    if(strcmp(portMirror, "on") == 0) {
        enable = FUN_ENABLE;
	PORT_MIRROR_PRINTF("%s-%d:enable = %d\n",__FUNCTION__,__LINE__,enable);
    }
    if(profSys->portMirrorEn != enable)
    {
	profSys->portMirrorEn = enable;
	PORT_MIRROR_PRINTF("%s-%d:set portMirrorEn = %d\n",__FUNCTION__,__LINE__,profSys->portMirrorEn);
    }
    
    /* update systemProfile
     * and write to flash
     */
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
#if defined(CONFIG_RTL8367M) || defined(CONFIG_RALINK_RT3883) || defined(CONFIG_RTL8306E) || defined(CONFIG_RTL8367RB)
#if defined(CONFIG_UTT_NV840E) || defined(CONFIG_UTT_NV800) || defined(CONFIG_UTT_NV840D) || defined(CONFIG_UTT_NV840DV3)||defined(CONFIG_UTT_NV840EV3)
    websRedirect(wp, "PortMirror840E.asp");
#else
    websRedirect(wp, "PortMirror8367M.asp");
#endif
#elif defined(CONFIG_RALINK_MT7620) || defined(CONFIG_RALINK_MT7621)
    websRedirect(wp, "PortMirror7620e.asp");
#else
    websRedirect(wp, "PortMirror.asp");
#endif
    return;
}

/******************************
 * formDefinePortMirror
 * 2012-04-11
 * guo.deyuan 
 */
void formDefinePortMirror(void)
{
    websAspDefine(T("aspPortMirrorData"), aspPortMirrorData);	    /*define asp function*/
    websFormDefine(T("ConfigPortMirror"), formConfigPortMirror);    /*define form function*/
}
#endif
