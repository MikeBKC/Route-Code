#include "uttMachine.h"
#if (SSH_CLI == FYES)
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"

extern void SSHRuleAdd(char *ip_from, char *ip_end)
{
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);

    if((profSys != NULL) && (ProfInstIsFree(profSys) == 0)) 
    {
	if(profSys->SSH.SSHEn == FUN_ENABLE)
	{
	    doSystem("iptables -t mangle -D %s -m uttdev --is-lan-in -p tcp -m multiport --destination-port 161,443,60023 -m iprange ! --src-range %s-%s -m set --set %s dst -j DROP", DEFENSE_FILTER_CHAIN, ip_from, ip_end, SYS_LAN_GROUP);/*删除规则*/
	    doSystem("iptables -t mangle -A %s -m uttdev --is-lan-in -p tcp -m multiport --destination-port %d,161,443,60023 -m iprange ! --src-range %s-%s -m set --set %s dst -j DROP", DEFENSE_FILTER_CHAIN, profSys->SSH.Port, ip_from, ip_end, SYS_LAN_GROUP);/*增加规则*/
	}
    }
    return;
}
#if (NET_SECURITY == FYES)
/********************************************************************
 * 函数： AccessCtr
 * 功能： 当端口改变时改变设备访问控制的
 * 创建： 2012-11-07
 * 参数： prof      -        端口
 *	  action    -        动作（添加还是删除）
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void AccessCtr(Uint32 Port, char action, int flag)
{
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);

    char ip_from[32];
    char ip_end[32];
    struct in_addr addrS;

    memset(ip_from, 0, 32);
    memset(ip_end, 0, 32);
    memset(&addrS, 0, sizeof(addrS));
    if((profSys != NULL) && (ProfInstIsFree(profSys) == 0)) 
    {
	if((profSys->sysNetSe.accessCtrlFromIP != 0U) && (profSys->sysNetSe.accessCtrlEndIP != 0U))
	{/*设备访问控制开启才改变规则*/
	    addrS.s_addr = profSys->sysNetSe.accessCtrlFromIP;
	    strcpy(ip_from, inet_ntoa(addrS));/*起始ip字串*/
	    addrS.s_addr = profSys->sysNetSe.accessCtrlEndIP;
	    strcpy(ip_end, inet_ntoa(addrS));/*结束ip字串*/
	    doSystem("iptables -t mangle -%c %s -m uttdev --is-lan-in -p tcp -m multiport --destination-port %d,161,443,60023 -m iprange ! --src-range %s-%s -m set --set %s dst -j DROP", action, DEFENSE_FILTER_CHAIN, Port, ip_from, ip_end, SYS_LAN_GROUP);/*修改访问控制规则*/
	    if((action == Iptables_DEL) && (flag == 1))
	    {
		doSystem("iptables -t mangle -A %s -m uttdev --is-lan-in -p tcp -m multiport --destination-port 161,443,60023 -m iprange ! --src-range %s-%s -m set --set %s dst -j DROP", DEFENSE_FILTER_CHAIN, ip_from, ip_end, SYS_LAN_GROUP);/*修改访问控制规则*/
	    }
	    else if((action == Iptables_DEL) && (flag == 2))
	    {
		doSystem("iptables -t mangle -D %s -m uttdev --is-lan-in -p tcp -m multiport --destination-port 161,443,60023 -m iprange ! --src-range %s-%s -m set --set %s dst -j DROP", DEFENSE_FILTER_CHAIN, ip_from, ip_end, SYS_LAN_GROUP);/*修改访问控制规则*/
	    }
	    else
	    {
		/*test*/
	    }
	}
    }
    return;
}
#endif
/********************************************************************
 * 函数： SSHInit
 * 功能： 初始化函数
 * 创建： 2012-10-16
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void SSHInit(void) 
{
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);

    if((profSys != NULL) && (ProfInstIsFree(profSys) == 0)) 
    {
	doSystem("dropbear_init.sh %d %d", profSys->SSH.SSHEn, profSys->SSH.Port);/*调用脚本*/
    }
    return;
}

/********************************************************************
 * 函数： SSHProfileChange
 * 功能： profile change 函数
 * 创建： 2012-10-16
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void SSHProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr)
{
    switch(changeType) 
    {
	case PROF_CHANGE_ADD:
	case PROF_CHANGE_EDIT:
	    if((oldProfileDataPtr->SSH.SSHEn == FUN_ENABLE) && (newProfileDataPtr->SSH.SSHEn == FUN_DISABLE))
	    {
		AccessCtr(oldProfileDataPtr->SSH.Port, Iptables_DEL, 1);
		doSystem("killall dropbear");
	    }
	    if(((oldProfileDataPtr->SSH.SSHEn == FUN_DISABLE) && (newProfileDataPtr->SSH.SSHEn == FUN_ENABLE)) || (oldProfileDataPtr->SSH.Port != newProfileDataPtr->SSH.Port))
	    {
		doSystem("dropbear.sh %d", newProfileDataPtr->SSH.Port);
#if (NET_SECURITY == FYES)
		if((oldProfileDataPtr->SSH.SSHEn == FUN_DISABLE) && (newProfileDataPtr->SSH.SSHEn == FUN_ENABLE))
		{
		    AccessCtr(oldProfileDataPtr->SSH.Port, Iptables_DEL, 2);
		}
		else
		{
		    AccessCtr(oldProfileDataPtr->SSH.Port, Iptables_DEL, 0);
		}
		AccessCtr(newProfileDataPtr->SSH.Port, Iptables_ADD, 0);
#endif
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
 * 函数： funInitSSH
 * 功能： 初始化函数
 * 创建： 2012-10-16
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
extern void funInitSSH(void) 
{
    registerForProfileUpdates(MIB_PROF_SYSTEM, NULL, (ProfileUpdateFnc)SSHProfileChange);
    SSHInit();
    return;
}
#endif
