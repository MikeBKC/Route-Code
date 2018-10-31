#include	"uttMachine.h"
#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include	"argcmd.h"

#if (IDENTIFY_BIND == FYES)
#define SWORD_DEBUG 0
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

/********************************************************************
 * 函数： aspOutIdentifyBind
 * 功能： 页面输出
 * 创建： 2012-11-09
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int aspOutIdentifyBind(int eid, webs_t wp, int argc, char_t **argv) 
{
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);

    if((profSys != NULL) && (ProfInstIsFree(profSys) == 0)) 
    {
	websWrite(wp, "var IdentifyBindEnables =\"%d\";", profSys->IdentifyBindEn);
    }

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

/********************************************************************
 * 函数： formIdentifyBind
 * 功能： 页面保存
 * 创建： 2012-11-09
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void formIdentifyBind(webs_t wp, char_t *path, char_t *query) 
{
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    struProfAlloc *profhead = NULL;
    char *enable = NULL;

    enable = websGetVar(wp, "IdentifyBindEnable", T(""));
    SWORD_PRINTF("%s------%d, enable = %s\n", __func__, __LINE__, enable);

    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);

    if((profSys != NULL) && (ProfInstIsFree(profSys) == 0)) 
    {
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
	if (strcmp(enable, "on") == 0) 
	{
	    profSys->IdentifyBindEn = FUN_ENABLE;
	} 
	else
	{
	    profSys->IdentifyBindEn = FUN_DISABLE;
	}

	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
    }
    websRedirect(wp, "IdentifyBind.asp");
    return;
}

/********************************************************************
 * 函数： formDefineIdentifyBind
 * 功能： 定义页面接口函数
 * 创建： 2012-11-09
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
void formDefineIdentifyBind(void)
{
    websAspDefine(T("aspOutIdentifyBind"), aspOutIdentifyBind);	    /*define asp function*/
    websFormDefine(T("formIdentifyBind"), formIdentifyBind);    /*define form function*/
}
#endif
