#include "uttMachine.h"
#if (HTTPS == FYES)
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
 * 函数： httpsInit
 * 功能： 初始化函数
 ********************************************************************/ 
static void httpsInit(void) 
{
    char lanIP[16]={0};
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);

    if((profSys != NULL) && (ProfInstIsFree(profSys) == 0)) 
    {
	/*没有开启https就过滤掉443端口的包*/
	if(profSys->sysHttps.httpsEn == FUN_DISABLE)
	{
	    getIfIp(getLanIfName(),lanIP);
	    doSystem("iptables -t filter -A INPUT -p tcp -d %s --dport 443 -j DROP", lanIP);
	}
    }
    return;
}

/********************************************************************
 * 函数： httpsProfileChange
 * 功能： profile change 函数
 ********************************************************************/ 
static void httpsProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr)
{
    char lanIP[16]={0};
    switch(changeType) 
    {
	case PROF_CHANGE_ADD:
	case PROF_CHANGE_EDIT:
	    /*开启到关闭*/ 
	    if((oldProfileDataPtr->sysHttps.httpsEn == FUN_ENABLE) && (newProfileDataPtr->sysHttps.httpsEn == FUN_DISABLE))
	    {
	    	getIfIp(getLanIfName(),lanIP);
	    	doSystem("iptables -t filter -A INPUT -p tcp -d %s --dport 443 -j DROP", lanIP);
	    }
	    /*关闭到开启*/
	    if((oldProfileDataPtr->sysHttps.httpsEn == FUN_DISABLE) && (newProfileDataPtr->sysHttps.httpsEn == FUN_ENABLE))
	    {
	    	getIfIp(getLanIfName(),lanIP);
		doSystem("iptables -t filter -D INPUT -p tcp -d %s --dport 443 -j DROP", lanIP);
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
 * 函数： funInitHttps
 * 功能： 初始化函数
 ********************************************************************/ 
extern void funInitHttps(void) 
{
    registerForProfileUpdates(MIB_PROF_SYSTEM, NULL, (ProfileUpdateFnc)httpsProfileChange);
    httpsInit();
    return;
}
#endif
