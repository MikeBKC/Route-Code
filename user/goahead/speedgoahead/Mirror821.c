
#include <stdlib.h>
#include <stdio.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "ifmisc.h"

#if (PORT_MIRROR == FYES)
#if defined(CONFIG_RTL8306_PHY) || defined(CONFIG_RTL8306E)
#if (UTT_NV_840E == FYES) || (UTT_NV_800 == FYES) || (UTT_NV_840D == FYES) || (UTT_NV_840DV3 == FYES)||(UTT_NV_840EV3 == FYES)
extern int _840E_gpio_write(int phy_id, int page, int reg, int value);

static void mirror840ESet(SystemProfile *profSys)
{
    Uint32 regV = 0;
    if(profSys->portMirrorEn == FUN_ENABLE) 
    {
	/* 设置镜像端口 */
	_840E_gpio_write(2, 3, 22, (profSys->mirror.monitorPort - 1) << 11);

	/* 设置被镜像端口 */
	regV = 0x80 | profSys->mirror.sourcePort | (profSys->mirror.sourcePort << 8);
	_840E_gpio_write(6, 3, 21, regV);
    }
    else
    {
	_840E_gpio_write(6, 3, 21, 0x0);
    }

    return;

}
#else
/**
 * 生效代码
 * */
void mirror821Set(SystemProfile *profSys)
{
    if(profSys->portMirrorEn == FUN_ENABLE) 
    {
	doSystem("arswitch reg w 2 3 22 0x0");
#if (UTT_MV_821 == FYES)
	doSystem("arswitch reg w 6 3 21 0x686");
#elif (UTT_MV_811 == FYES)
	doSystem("arswitch reg w 6 3 21 0xe8e");
#endif
    }
    else
    {
	doSystem("arswitch reg w 6 3 21 0x0");
    }
    return;

}
#endif
#endif
void spdMirrorSet(SystemProfile *profSys)
{
#if defined(CONFIG_RTL8306_PHY) || defined(CONFIG_RTL8306E)
#if (UTT_NV_840E == FYES) || (UTT_NV_800 == FYES) || (UTT_NV_840D == FYES) || (UTT_NV_840DV3 == FYES)||(UTT_NV_840EV3 == FYES)
    mirror840ESet(profSys);
#else
    mirror821Set(profSys);
#endif
#elif defined(CONFIG_UTT_RDD_MIRROR)
    rddMirrorSet(profSys);
#endif
    return;
}

/***************
 * mirror821Init()
 * 2012-04-11
 */
void mirror821Init(void)
{
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;

    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);
    spdMirrorSet(profSys);   
    return;
}

/****************
 * mirror821ProfileChanged()
 * 2010-04-11
 */
void mirror821ProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr)
{
    SystemProfile *newProf = (SystemProfile *)newProfileDataPtr;
    SystemProfile *oldProf = (SystemProfile *)oldProfileDataPtr;
    
#if defined(CONFIG_RTL8367M) || defined(CONFIG_RALINK_RT3883) || defined(CONFIG_RTL8306E)|| defined(CONFIG_RTL8367RB) 
    if(memcmp(&(oldProf->mirror), &(newProf->mirror), sizeof(PortMirror)) 
	    || (oldProf->portMirrorEn != newProf->portMirrorEn))
#else
    /*portmirror enable changed*/
    if(oldProf->portMirrorEn != newProf->portMirrorEn) 
#endif
    {
	spdMirrorSet(newProf);
    }
    return;
}

/****************
 * funInitMirror821()
 * 2012-04-11
 */
void funInitMirror821(void)
{
    registerForProfileUpdates(MIB_PROF_SYSTEM, NULL, mirror821ProfileChange); /*注册ProfileChange函数*/
    mirror821Init(); /*初始化*/
    return;
}
#endif
