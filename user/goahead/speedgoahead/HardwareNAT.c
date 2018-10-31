#include "uttMachine.h"
#if (HARDWARE_NAT == FYES)
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"

//#define NAT_LOAD_BALANCE
#if defined(NAT_LOAD_BALANCE)
extern void delNlbJumpRule(char *ip) {

	doSystem("iptables -t mangle -D PREROUTING -m conntrack --ctstate NEW -m uttdev --is-lan-in -m connmark --mark 0 ! -d %s -j nlb_chain", ip);
}

extern void createNlbJumpRule(char *ip) {

	doSystem("iptables -t mangle -A PREROUTING -m conntrack --ctstate NEW -m uttdev --is-lan-in -m connmark --mark 0 ! -d %s -j nlb_chain", ip);
}
#endif

/********************************************************************
 * 函数： hardwareNATInit
 * 功能： 初始化函数
 * 创建时间：2012-11-19
 ********************************************************************/ 
static void hardwareNATInit(void) 
{
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);

#if defined(NAT_LOAD_BALANCE)
    char ip[16] = {0};

    getIfIp(getLanIfName(), ip);
#endif

    if((profSys != NULL) && (ProfInstIsFree(profSys) == 0)) 
    {
	if(profSys->hardwareNATEn == FUN_ENABLE)
	{
#if (KERNEL_2_6_36 == FYES) 
	    doSystem("insmod /lib/modules/2.6.36/kernel/net/nat/hw_nat/hw_nat.ko");
#else
	    doSystem("insmod /lib/modules/2.6.21/kernel/net/nat/hw_nat/hw_nat.ko");
#endif
#if defined(NAT_LOAD_BALANCE)
	    delNlbJumpRule(ip);
	    createNlbJumpRule(ip);
#endif
	}
    }
    return;
}

/********************************************************************
 * 函数： hardwareNATProfileChange
 * 功能： profile change 函数
 * 创建时间：2012-11-19
 ********************************************************************/ 
static void hardwareNATProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr)
{

#if defined(NAT_LOAD_BALANCE)
    char ip[16] = {0};

    getIfIp(getLanIfName(), ip);
#endif

    switch(changeType) 
    {
	case PROF_CHANGE_ADD:
	case PROF_CHANGE_EDIT:
	    /*开启到关闭*/ 
	    if((oldProfileDataPtr->hardwareNATEn == FUN_ENABLE) && (newProfileDataPtr->hardwareNATEn == FUN_DISABLE))
	    {
#if (KERNEL_2_6_36 == FYES) 
		doSystem("rmmod /lib/modules/2.6.36/kernel/net/nat/hw_nat/hw_nat.ko");
#else
		doSystem("rmmod /lib/modules/2.6.21/kernel/net/nat/hw_nat/hw_nat.ko");
#endif
#if defined(NAT_LOAD_BALANCE)
		delNlbJumpRule(ip);
#endif
	    }
	    /*关闭到开启*/
	    if((oldProfileDataPtr->hardwareNATEn == FUN_DISABLE) && (newProfileDataPtr->hardwareNATEn == FUN_ENABLE))
	    {
#if (KERNEL_2_6_36 == FYES) 
		doSystem("insmod /lib/modules/2.6.36/kernel/net/nat/hw_nat/hw_nat.ko");
#else
		doSystem("insmod /lib/modules/2.6.21/kernel/net/nat/hw_nat/hw_nat.ko");
#endif
#if defined(NAT_LOAD_BALANCE)
		delNlbJumpRule(ip);
		createNlbJumpRule(ip);
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
 * 函数： funInitHardwareNAT
 * 功能： 注册函数
 * 创建时间：2101-11-19
 ********************************************************************/ 
extern void funInitHardwareNAT(void) 
{
    registerForProfileUpdates(MIB_PROF_SYSTEM, NULL, (ProfileUpdateFnc)hardwareNATProfileChange);
    hardwareNATInit();
    return;
}
#endif
