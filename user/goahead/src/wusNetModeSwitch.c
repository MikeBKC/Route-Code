#include <stdio.h>
#include <unistd.h>
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "mib.h"
#include "profacce.h"
#include "uttfunction.h"
#if (NET_MODE_SWITCH== FYES)

static void formNetModeSwitch(webs_t wp, char_t *path, char_t *query);
static int aspOutPutNetModeSwitch(int eid, webs_t wp, int argc, char **argv);
static int aspOutPutNetModeSwitchForLeft(int eid, webs_t wp, int argc, char **argv);
/********************************************************************
 * 函数： formDefineNetModeSwitch
 * 功能： 页面与交互转换函数
 * 参数： 无
 * 输出： 无
 * 返回： 无
 * 作者： chen.bin
 * 创建： 2013-01-29
 *******************************************************************/
void formDefineNetModeSwitch(void)
{
    websAspDefine(T("aspOutPutNetModeSwitch"), aspOutPutNetModeSwitch);
    websFormDefine(T("formNetModeSwitch"), formNetModeSwitch);
    websAspDefine(T("aspOutPutNetModeSwitchForLeft"), aspOutPutNetModeSwitchForLeft);
}

/*******************************************************************
 *函数： formNetModeSwitch
 *功能： 页面提交函数
 *参数： 
 *返回： 无
 *作者： chen.bin
 *创建： 2013-01-29
 ******************************************************************/
static void formNetModeSwitch(webs_t wp, char_t *path, char_t *query)
{
	char_t *netMode = NULL;
	MibProfileType mibType = MIB_PROF_NET_MODE_SWITCH;
	NetModeSwitchProfile *prof = NULL;
	int mode ;

	netMode = websGetVar(wp,T("netMode"),T(""));
	mode = (int)strtoul(netMode,0,10);
	
	prof = (NetModeSwitchProfile *)ProfGetInstPointByIndex(mibType,0);
	if(prof != NULL)
	{
		if((prof->bridgeEnable == 1)&&(mode==0))
		{
			prof->bridgeEnable = PROF_DISABLE;
			nvramWriteCommit();
			web_reload(wp,"netModeReload.asp");
		}
		else if ((prof->bridgeEnable==0)&&(mode==1))
		{
			prof->bridgeEnable = PROF_ENABLE;
			nvramWriteCommit();
			web_reload(wp,"netModeReload.asp");
		}
		else
		{
			web_reload(wp,"netModeSwitch.asp");
		}
	}
	return;
}


/********************************************************************
 * 函数： aspOutPutNetModeSwitchForLeft
 * 功能： 页面显示函数
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： chen.bin
 * 创建： 2013-01-29
 ********************************************************************/ 
static int aspOutPutNetModeSwitchForLeft(int eid, webs_t wp, int argc, char **argv)
{
	Uint32 mode = 0u;
	MibProfileType mibType = MIB_PROF_NET_MODE_SWITCH;
	NetModeSwitchProfile *prof = NULL;

	prof = (NetModeSwitchProfile *)ProfGetInstPointByIndex(mibType,0);
	if(prof!=NULL)
	{
		mode = prof->bridgeEnable;
	}		
	if(mode == PROF_ENABLE)
	{
		websWrite(wp,"<script src=\"js/br_config.js\" type=\"text/javascript\"></script>");
	}
	else
	{
		websWrite(wp,"<script src=\"js/config.js\" type=\"text/javascript\"></script>");
	}

    return 0;
}

/********************************************************************
 * 函数： aspOutPutNetModeSwitch
 * 功能： 页面显示函数
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： chen.bin
 * 创建： 2013-01-29
 ********************************************************************/ 
static int aspOutPutNetModeSwitch(int eid, webs_t wp, int argc, char **argv)
{
	Uint32 mode = 0u;
	MibProfileType mibType = MIB_PROF_NET_MODE_SWITCH;
	NetModeSwitchProfile *prof = NULL;

	prof = (NetModeSwitchProfile *)ProfGetInstPointByIndex(mibType,0);
	if(prof!=NULL)
	{
		mode = prof->bridgeEnable;
	}
    websWrite(wp,"var netMode=%d;",mode);
    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

#endif
