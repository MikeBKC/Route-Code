#include "uttMachine.h"
#if (SYSLOG == FYES)
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"

/********************************************************************
 * 函数： SyslogInit
 * 功能： 初始化函数
 * 创建： 2012-10-16
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void SyslogInit(void) 
{
    SyslogProfile *profSyslog = NULL;
    MibProfileType mibType = MIB_PROF_SYSLOG;
    profSyslog = (SyslogProfile*)ProfGetInstPointByIndex(mibType, 0);

    if((profSyslog != NULL) && (ProfInstIsFree(profSyslog) == 0)) 
    {
	doSystem("syslog.sh");/*调用脚本*/
    }
    return;
}

/********************************************************************
 * 函数： SyslogProfileChange
 * 功能： profile change 函数
 * 创建： 2012-10-16
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void SyslogProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const SyslogProfile *oldProfileDataPtr, const SyslogProfile *newProfileDataPtr)
{
    switch(changeType) 
    {
	case PROF_CHANGE_ADD:
	case PROF_CHANGE_EDIT:
	    if(memcmp(oldProfileDataPtr, newProfileDataPtr, sizeof(SyslogProfile)) != 0)
	    {
		doSystem("syslog.sh");
	    }
	    break;
	case PROF_CHANGE_DEL:
	case PROF_CHANGE_DELALL:
	default:/*test*/
	    break;
    }
    return;
}

/********************************************************************
 * 函数： funInitSyslog
 * 功能： 初始化函数
 * 创建： 2012-10-16
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
extern void funInitSyslog(void) 
{
    registerForProfileUpdates(MIB_PROF_SYSLOG, NULL, (ProfileUpdateFnc)SyslogProfileChange);
    SyslogInit();
    return;
}
#endif
