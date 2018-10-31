
#include <stdlib.h>
#include <stdio.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "ifmisc.h"

#if (PORT_MIRROR == FYES)
#if (CONFIG_BOARD_MTK7620_E == FYES || CONFIG_RALINK_MT7621 == FYES) || (CONFIG_BOARD_MTK7620_EN == FYES)

#if (CONFIG_RALINK_MT7621 == FYES)
/* 7621 接口对应关系： port0 - lan1, port1 - lan2 ... */
static int mirror7620eGetLanId(int portnumber, int lanCount)
{
    int wanType = -1;
    int lanId = wanType;

    if (lanCount < portnumber + 1) {
	lanId = wanType;
    } else {
	lanId = portnumber + 1;
    }

    return lanId;
}
#else
static int mirror7620eGetLanId(int portnumber, int lanCount)
{
    int wanType = 0;
    int lanId = wanType;

    if (portnumber == 0)
    {
	if (lanCount <= 1)
	    lanId = wanType;
	else
	    lanId = 2;
    }
    else if (portnumber == 1)
    {
	if (lanCount <= 2)
	    lanId = wanType;
	else
	    lanId = 3;
    }
    else if (portnumber == 2)
    {	
	if (lanCount <= 3)
	    lanId = wanType;
	else
	    lanId = 4;
    }
    else if (portnumber == 3)
    {
	
	lanId = wanType;
    }
    else if (portnumber == 4)
    {
	lanId = 1;
    }

    return lanId;
}
#endif

void spd7620eMirrorSet(SystemProfile *profSys)
{
    FILE *fp;
    char buf[50];
    unsigned int regValue = 0;
    int portnumber, lanId;

    if (profSys == NULL)
    {
	printf("profSys is null.\n");
	return;
    }

    if ((profSys->portMirrorEn == FUN_ENABLE) && (profSys->lanPortCount != 1))
    {
	for (portnumber=0; portnumber<5; portnumber++)
	{
	    lanId = mirror7620eGetLanId(portnumber, profSys->lanPortCount);
	    if (lanId == profSys->mirror.monitorPort)
	    {	
		/* 设置监控lan口 */
		doSystem("switch mirror monitor %d", portnumber);
		printf("switch mirror monitor %d\n", portnumber);
	    }
#if (CONFIG_RALINK_MT7621 == FYES)
	    else if (lanId == -1)
#else
	    else if (lanId == 0)
#endif
	    {
		/* 设置wan口 */
		doSystem("switch mirror target %d 0", portnumber);
		printf("switch mirror target %d 0\n", portnumber);
	    }
	    else
	    {
		/* 设置被监控lan口*/
		doSystem("switch mirror target %d 3", portnumber);
		printf("switch mirror target %d 3\n", portnumber);
	    }
	}
    }
    else
    {
	fp = popen("switch reg r 10", "r");
	if (fp == NULL)
	{
	    printf("%s %d: command error.\n", __func__, __LINE__);
	    return;
	}
	memset(buf, 0, sizeof(buf));
	fgets(buf, 45, fp);
	printf("%s %d.........get buf: %s\n", __func__, __LINE__, buf);
	pclose(fp);
	sscanf(buf, "switch reg read offset=10, value=%8x\n", &regValue);
	regValue &= 0xfffffff0;
	doSystem("switch reg w 10 %x", regValue);
	printf("switch reg w 10 %x", regValue);
    }
    return;
}

/***************
 * mirror821Init()
 * 2012-04-11
 */
void mirror7620eInit(void)
{
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;

    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);
    spd7620eMirrorSet(profSys);   

    return;
}

/****************
 * mirror821ProfileChanged()
 * 2010-04-11
 */
void mirror7620eProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr)
{
    spd7620eMirrorSet((SystemProfile*)newProfileDataPtr);

    return;
}

void funInitMirror7620e(void)
{
    registerForProfileUpdates(MIB_PROF_SYSTEM, NULL, mirror7620eProfileChange); /*注册ProfileChange函数*/
    mirror7620eInit(); /*初始化*/
    return;
}
#endif
#endif
