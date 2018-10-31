#include "uttMachine.h"
#if (IDENTIFY_BIND == FYES)
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"
#include "ifmisc.h"

#define SWORD_DEBUG 0
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

/********************************************************************
 * 函数： IdentifyBindAOrD
 * 功能： 添加或删除身份绑定链
 * 创建： 2012-11-09
 * 参数： action:       
 *	  Iptables_ADD   -        添加
 *	  Iptables_DEL   -        删除
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void IdentifyBindAOrD(char action)
{
    int i = 0;
    for(i = 1; i <= getWanSlotCount(); i++) 
    {
	if(_getWanLinkState(i) == 1)
	{
	    doSystem("iptables -t mangle -%c connmark_wan%d -o %s -p tcp --dport 443 -m conntrack  --ctstate NEW  -j CONNMARK --set-mark 0x%d0/0xff", action, i, getWanIfNamePPP(i), i);
	    doSystem("iptables -t mangle -%c connmark_wan%d -o %s -p udp -m multiport --dport 4000,8000,8001 -m conntrack  --ctstate NEW  -j CONNMARK --set-mark 0x%d0/0xff", action, i, getWanIfNamePPP(i), i);
	    SWORD_PRINTF("%s------%d, iptables -t mangle -%c connmark_wan%d -o %s -p tcp --dport 443 -m conntrack  --ctstate NEW  -j CONNMARK --set-mark 0x%d0/0xff\n", __func__, __LINE__, action, i, getWanIfNamePPP(i), i);
	}
    }
    return;
}
/********************************************************************
 * 函数： IdentifyBindProfileChange
 * 功能： profile change 函数
 * 创建： 2012-10-16
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void IdentifyBindProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr)
{
    switch(changeType) 
    {
	case PROF_CHANGE_ADD:
	case PROF_CHANGE_EDIT:
	    if((oldProfileDataPtr->IdentifyBindEn == FUN_ENABLE) && (newProfileDataPtr->IdentifyBindEn == FUN_DISABLE))
	    {
		IdentifyBindAOrD(Iptables_DEL);
	    }
	    if((oldProfileDataPtr->IdentifyBindEn == FUN_DISABLE) && (newProfileDataPtr->IdentifyBindEn == FUN_ENABLE)) 
	    {
		IdentifyBindAOrD(Iptables_ADD);
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
 * 函数： funInitIdentifyBind
 * 功能： 初始化函数
 * 创建： 2012-10-16
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
extern void funInitIdentifyBind(void) 
{
    registerForProfileUpdates(MIB_PROF_SYSTEM, NULL, (ProfileUpdateFnc)IdentifyBindProfileChange);
    /*在线路起来的时候会初始化，因此此处不用初始化*/
    return;
}
#endif
