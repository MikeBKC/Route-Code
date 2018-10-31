#include	"uttMachine.h"
#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include	"argcmd.h"

#if (CACHE_SERVER == FYES)
#define SWORD_DEBUG 0
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

/********************************************************************
 * 函数： aspCacheServerConf
 * 功能： 页面输出
 * 创建： 2013-04-02
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： chen.bin 
 ********************************************************************/ 
static int aspCacheServerConf(int eid, webs_t wp, int argc, char_t **argv) 
{
    CacheServerProfile *profCacheServer = NULL;
    MibProfileType mibType = MIB_PROF_CACHE_SERVER;
    profCacheServer = (CacheServerProfile*)ProfGetInstPointByIndex(mibType, 0);

    if (profCacheServer->cacheServerEnable ) {
	websWrite(wp, "var cacheServerEnable=\"on\";");
    } else {
	websWrite(wp, "var cacheServerEnable=\"off\";");
    }
    websWrite(wp, "var cacheServerIp=\"%s\";", profCacheServer->cacheServerIp);
    websWrite(wp, "var cacheServerPort=%d;", profCacheServer->cacheServerPort);

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

/********************************************************************
 * 函数： formCacheServerConf
 * 功能： 页面保存
 * 创建： 2013-04-03
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： chen.bin
 ********************************************************************/ 
static void formCacheServerConf(webs_t wp, char_t *path, char_t *query) 
{
    MibProfileType mibType = MIB_PROF_CACHE_SERVER;
    CacheServerProfile *profCacheServer = NULL;
    struProfAlloc *profhead = NULL;
    char *enable = NULL, *cacheServerIp = NULL, *cacheServerPort = NULL ;

    enable = websGetVar(wp, "cacheServerEnable", T(""));
    cacheServerIp = websGetVar(wp, "cacheServerIp", T(""));
    cacheServerPort = websGetVar(wp, "cacheServerPort", T(""));
    SWORD_PRINTF("%s------%d, enable = %s, cacheServerIp = %s, cacheServerPort = %s\n", __func__, __LINE__, enable, cacheServerIP, cacheServerPort);

    profCacheServer = (CacheServerProfile *)ProfGetInstPointByIndex(mibType, 0);

    if((profCacheServer != NULL) && (ProfInstIsFree(profCacheServer) == 0)) 
    {
		ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
		if (strcmp(enable, "on") == 0) 
		{/*开启CacheServer才保存ip等信息*/
		 profCacheServer->cacheServerEnable = TRUE;
		 strcpy(profCacheServer->cacheServerIp, cacheServerIp);
		 profCacheServer->cacheServerPort = (Uint32)strtoul(cacheServerPort, NULL, 10);
		} 
		else 
		{
		 profCacheServer->cacheServerEnable = FALSE;
		}

		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
		nvramWriteCommit();
    }
    websRedirect(wp, "CacheServerConfig.asp");
    return;
}

/********************************************************************
 * 函数： formDefineCacheServer
 * 功能： 定义页面接口函数
 * 创建： 2013-04-03
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： chen.bin
 ********************************************************************/ 
void formDefineCacheServer(void)
{
    websAspDefine(T("aspCacheServerConf"), aspCacheServerConf);	    /*define asp function*/
    websFormDefine(T("formCacheServerConf"), formCacheServerConf);    /*define form function*/
}
#endif
