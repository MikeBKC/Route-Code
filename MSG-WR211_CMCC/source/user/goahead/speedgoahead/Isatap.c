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
#include "Radvd.h"
#include "Isatap.h"
#if (IPV6_ISATAP == FYES)


#define SWORD_DEBUG 1
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)              printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif
static void Ipv6IsatapChange(Pointer tag, MibProfileType profileType,
	ProfChangeType changeType, Uint32 index,
	const IsatapProfile *oldProfileDataPtr, const IsatapProfile *newProfileDataPtr );

extern void Isatapinit(void);
static void IsatapRuleAdd(IsatapProfile *oldProfileDataPtr,IsatapProfile *newProfileDataPtr);



/********************************************************************
 * * 函数： IsatapRuleAdd
 * * 功能： ISATAP服务器配置命令添加函数
 * * 创建： 2014-10-24
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/
static void IsatapRuleAdd(IsatapProfile *oldProfileDataPtr,IsatapProfile *newProfileDataPtr)
{
    
    doSystem("/bin/ipv6/ip tunnel del name is0");    //使用另外一个ip进程，加路径执行命令
   if(newProfileDataPtr->ISATAPEnabled == 1){
	doSystem("/bin/ipv6/ip tunnel add name is0 mode isatap local %N",newProfileDataPtr->ISATAP_IPv4Gat);
	doSystem("/bin/ipv6/ip link set is0 up");
	doSystem("ip -6 addr add %s/64 dev is0",newProfileDataPtr->ISATAP_IPv6Pre);
    }
    Radvdinit();     //重写radvd配置文件
}

/********************************************************************
 * * 函数： Isatapinit
 * * 功能： Isatap初始化函数
 * * 创建： 2014-10-24
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/
extern void Isatapinit()
{
    MibProfileType mibType=MIB_PROF_ISATAP;
    IsatapProfile   *prof=NULL;
    prof=(IsatapProfile *)ProfGetInstPointByIndex(mibType, 0);
    if((prof != NULL) && (prof->ISATAPEnabled == 1)){ 
	doSystem("/bin/ipv6/ip tunnel add name is0 mode isatap local %N",prof->ISATAP_IPv4Gat);
	doSystem("/bin/ipv6/ip link set is0 up");
	doSystem("ip -6 addr add %s/64 dev is0",prof->ISATAP_IPv6Pre);
    }    
}



/********************************************************************
 * * 函数： Ipv6IsatapChange
 * * 功能： Isatap Change函数
 * * 创建： 2014-10-24
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/
static void Ipv6IsatapChange(Pointer tag, MibProfileType profileType,
	ProfChangeType changeType, Uint32 index,
	const IsatapProfile *oldProfileDataPtr, const IsatapProfile *newProfileDataPtr )
{

    switch(changeType) {
	case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
            printf("***%s---%d\n",__func__,__LINE__);
	    IsatapRuleAdd((IsatapProfile *)oldProfileDataPtr,(IsatapProfile *)newProfileDataPtr);
            break;

	case PROF_CHANGE_DEL:
	case PROF_CHANGE_DELALL:
        default:/*default*/
            break;
        }
        return;
}



/********************************************************************
 * * 函数： funInitIsatap
 * * 功能： Isatap 初始化入口函数
 * * 创建： 2014-10-24
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang
 * ********************************************************************/
void funInitIsatap(void)
{
    
    registerForProfileUpdates(MIB_PROF_ISATAP, (Pointer)NULL, (ProfileUpdateFnc)Ipv6IsatapChange);/*ISATAP触发函数*/
    doSystem("cd bin;mkdir ipv6;cd ..;mv ip /bin/ipv6/");
    Isatapinit();/*初始化*/
    printf("--%s--%d\n",__func__,__LINE__);
    return;
}

#endif
