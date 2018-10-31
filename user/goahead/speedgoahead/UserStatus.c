#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"
#include "spdComFun.h"
#include "comExeFun.h"
#if 0
/* 函数移动到speedweb/pdb.c中*/
static void UserStatusProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr );
static void UserStatusChainDel(void);
static void UserStatusChainAdd(void);
static void UserStatusInit(void);
/********************************************************************
* 函数： UserStatusProfileChange
* 功能： UserStatus开关变化 生效函数
* 创建： 2012-06-10
* 参数： 略
* 返回： 略
* 输出： 略
* 作者： Jianqing.Sun 
********************************************************************/
static void UserStatusProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
	    break;
        case PROF_CHANGE_EDIT:
	    if((oldProfileDataPtr->UserStatusEn == FUN_ENABLE) && (newProfileDataPtr->UserStatusEn == FUN_DISABLE))
	    {
		UserStatusChainDel();
	    }
	    else if((oldProfileDataPtr->UserStatusEn == FUN_DISABLE) && (newProfileDataPtr->UserStatusEn == FUN_ENABLE))
	    {
		UserStatusChainAdd();
	    }
	    else
	    {
		/*test*/
	    }
	    break;
        case PROF_CHANGE_DEL:
	    break;
        case PROF_CHANGE_DELALL:
	    break;
        default:/*default*/
            break;
    }
    return;
}

/********************************************************************
* 函数： UserStatusChainDel
* 功能： 删除UserStatus规则
* 创建： 2012-06-10
* 参数： 略
* 返回： 略
* 输出： 略
* 作者： Jianqing.Sun 
********************************************************************/
static void UserStatusChainDel(void)
{
#if 0
    /*删除链*/
    doSystem("iptables -t mangle -D POSTROUTING -j UTTHOSTSTA");
#endif
    return;
}

/********************************************************************
* 函数： UserStatusChainAdd
* 功能： 添加UserStatus规则
* 创建： 2012-06-10
* 参数： 略
* 返回： 略
* 输出： 略
* 作者： Jianqing.Sun 
********************************************************************/
static void UserStatusChainAdd(void)
{
#if 0
    /*添加链*/
    doSystem("iptables -t mangle -I POSTROUTING -j UTTHOSTSTA 1>/dev/null 2>&1");
#endif
    return;
}
/********************************************************************
* 函数： UserStatusInit
* 功能： 用户状态初始化函数 - 先清空现有规则链表，再重新添加
* 创建： 2012-06-10
* 参数： 略
* 返回： 略
* 输出： 略
* 作者： Jianqing.Sun 
********************************************************************/
static void UserStatusInit(void)
{
#if 0
    UserStatusChainDel();
    UserStatusChainAdd();
#endif
    return;
}

/********************************************************************
* 函数： funInitUserStatus
* 功能： 用户状态 初始化入口函数
* 创建： 2012-06-10
* 参数： 略
* 返回： 略
* 输出： 略
* 作者： Jianqing.Sun 
********************************************************************/
void funInitUserStatus(void)
{
    registerForProfileUpdates(MIB_PROF_SYSTEM, (Pointer)NULL, UserStatusProfileChange);/*触发函数*/

    UserStatusInit();/*初始化*/
    return;
}
#endif
