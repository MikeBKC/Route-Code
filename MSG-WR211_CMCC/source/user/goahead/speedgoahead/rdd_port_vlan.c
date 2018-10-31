
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/autoconf.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"
#include "ifmisc.h"

#if (PVLAN_REDUCED == FYES)

#define SWORD_DEBUG 0
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, argx...) printf(fmt, ##argx)
#else
#define SWORD_PRINTF(fmt, argx...)
#endif
static void add_default_pvlan(void);
void pvlan_dports_set(Uint32 pno, Uint32 dPortsMask);
#if (UTT_NV_840E == FYES) || (UTT_NV_800 == FYES) || (UTT_NV_840D == FYES)
static void init_840E_vlan_reg(void);

#endif

static void pvlan_init(void);

/*********************************************************************
 * 函数： rddPortVlanProfileChange
 * 功能： 配置变化 生效函数
 * 创建： 2012-09-11
 * 参数： 略
 * 返回:  略
 * 输出： 略
 * 作者： Jianqing.Sun
 ********************************************************************/
static void rddPortVlanProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const RddPortVlanProfile *oldProfileDataPtr, const RddPortVlanProfile *newProfileDataPtr )
{
    switch(changeType) {
	case PROF_CHANGE_ADD:
	case PROF_CHANGE_EDIT:
	    pvlan_init();
	    break;
	case PROF_CHANGE_DEL:
	    pvlan_init();
	    break;
	case PROF_CHANGE_DELALL:
	    pvlan_init();
	    break;
	default:/*test*/
	    break;
    }
    return;
}

/*********************************************************************
 * 函数： funInitRddPortVlan
 * 功能： 初始化函数
 * 创建： 2012-09-11
 * 参数： 略
 * 返回:  略
 * 输出： 略
 * 作者： Jianqing.Sun
 ********************************************************************/
void funInitRddPortVlan(void)
{
    registerForProfileUpdates(MIB_PROF_RDD_PORT_VLAN, NULL, (ProfileUpdateFnc)rddPortVlanProfileChange);
#if (UTT_NV_840E == FYES) || (UTT_NV_800 == FYES) || (UTT_NV_840D == FYES)
    /* 可以放到内核中初始化 */
    init_840E_vlan_reg();
#endif
    pvlan_init();
    add_default_pvlan();
}
#if SWORD_DEBUG
/*********************************************************************
 * 函数： print_members
 * 功能： 成员输出函数
 * 创建： 2012-09-11
 * 参数： 略
 * 返回:  略
 * 输出： 略
 * 作者： Jianqing.Sun
 ********************************************************************/
void print_members(unsigned char *member, int  size)
{
    int i;

    for(i=0;i<size;i++)
    {
	printf("member[%d] = %d;\n", i, member[i]);
    }
    return;
}
#endif
/*********************************************************************
 * 函数： add_default_pvlan
 * 功能： 初始化没有vlan设置默认vlan函数
 * 创建： 2012-09-11
 * 参数： 略
 * 返回:  略
 * 输出： 略
 * 作者： Jianqing.Sun
 ********************************************************************/
static void add_default_pvlan(void)
{
    int i, min = 0, max = 0;
    MibProfileType	mibType = MIB_PROF_RDD_PORT_VLAN;
    RddPortVlanProfile  *prof;
    SystemProfile	*profSys = NULL;

    profSys = (SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++)
    {
	prof = (RddPortVlanProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active==TRUE))
	{/*配置了Vlan*/
	    break;
	}
    }
    SWORD_PRINTF("%s------%d, i = %d\n", __func__, __LINE__, i);
    if(i == max)/*i为最大值则表示没有配置任何Vlan*/
    {
	i = 0;
	prof = (RddPortVlanProfile*)ProfGetInstPointByIndex(mibType, i);
	prof->head.active=TRUE;
	/*pvlan做name*/
	ProfSetNameByPoint(prof, "pvlan1");
	memset(prof->description, 0, sizeof(prof->description));
	memset(prof->members, 0x01, sizeof(prof->members));
    }

    return;
}


/*********************************************************************
 * 函数： pvlan_init
 * 功能： 初始化pvlan
 * 创建： 2012-09-11
 * 参数： 略
 * 返回:  略
 * 输出： 略
 * 作者： Jianqing.Sun
 ********************************************************************/
static void pvlan_init()
{
    int pno = 0, min = 0, max = 0, memPort = 0;
    int pvlan_index = 0;
    int flag = 0;
    Uint32 dPortsMask=0U;
    MibProfileType mibType = MIB_PROF_RDD_PORT_VLAN;
    RddPortVlanProfile *pvlanprof = NULL;
    SystemProfile	*profSys = NULL;

    profSys = (SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    /*当前vlan类型*/
    ProfInstLowHigh(MIB_PROF_RDD_PORT_VLAN, &max, &min);
    /*更新所有端口可通信端口*/
    for(pno = 0;pno < profSys->lanPortCount; pno++)
    {
	dPortsMask = 1 << pno;
	/*遍历所有profile。查找包含pno的profile并构造可通信目的端口*/
	for(pvlan_index = min; pvlan_index < max; pvlan_index++)
	{
	    pvlanprof= (RddPortVlanProfile*)ProfGetInstPointByIndex(mibType, pvlan_index);
	    if((pvlanprof != NULL) && (ProfInstIsFree(pvlanprof) == 0) && (pvlanprof->head.active==TRUE))
	    {
		if(flag == 0)/*判断是否有配置存在*/
		{
		    flag = 1;
		}
		SWORD_PRINTF("%s------%d: name = %s\n", __func__, __LINE__, pvlanprof->head.name);
		/*成员包含pno，则pno可与该vlan所有成员通信 */
		if(pvlanprof->members[pno +1] == 1)
		{
		    /*构造可通信的目的端口*/
		    for(memPort = 0; memPort < profSys->lanPortCount; memPort++)
		    {
			if(pvlanprof->members[memPort +1] == 1)
			{
			    dPortsMask |= 1 << memPort ;
			}
		    }/*end for(memPort=0;......*/
		}
	    }/*end if((pvlanprof ......*/
	}/*end for(pvlan_index ......*/
	if(flag == 1)/*有配置存在才初始化，防止出厂配置时没有配置也调用这里设置寄存器*/
	{
	    pvlan_dports_set(pno, dPortsMask);/*配置寄存器使该端口只能和目的端口通信*/
	}

    } 

    return;
}
#ifdef CONFIG_RTL8306_PHY

#if (UTT_NV_840E == FYES) || (UTT_NV_800 == FYES) || (UTT_NV_840D == FYES)
extern int _840E_gpio_write(int phy_id, int page, int reg, int value);
extern int _840E_gpio_read(int phy_id, int page, int reg, int *value);
void pvlan_dports_set(Uint32 pno, Uint32 dPortsMask) {
    Uint32 regVal = 0U, pvlanMask = 0U;
    SystemProfile       *profSys = NULL;

    profSys = (SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    pvlanMask = convertLenToBinMask(profSys->lanPortCount);/*获取设置区域掩码*/

    _840E_gpio_read(pno, 0, 24, &regVal);
    regVal = (regVal & ~ pvlanMask) | (dPortsMask & pvlanMask);
    _840E_gpio_write(pno, 0, 24, regVal);

    return;
}

/*
 * 功能：
 * 初始化840E 8306芯片vlan相关寄存器
 * 包括启用vlan功能、设置4 lan口在同一vlan内
 */
static void init_840E_vlan_reg(void) {
    Uint32 regVal = 0U, pvlanMask = 0U;
    SystemProfile       *profSys = NULL;
    int i;

    /* 启用vlan:phy 0 reg 18 page 0 bit 8 value 0 */
    _840E_gpio_read(0, 0, 18, &regVal);
    regVal &= ~(1 << 8);
    _840E_gpio_write(0, 0, 18, regVal);

    /* 4 lan口在同一vlan内 */
    profSys = (SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    pvlanMask = convertLenToBinMask(profSys->lanPortCount);/*获取设置区域掩码*/

    for (i = 0; i < profSys->lanPortCount; i++) {
        _840E_gpio_read(i, 0, 24, &regVal);
	regVal |= pvlanMask;
	_840E_gpio_write(i, 0, 24, regVal);
    }

    return;
}
#else
/*********************************************************************
 * 函数： pvlan_dports_set
 * 功能： 8306芯片寄存器设置
 * 创建： 2012-09-11
 * 参数： 略
 * 返回:  略
 * 输出： 略
 * 作者： Jianqing.Sun
 ********************************************************************/
void pvlan_dports_set(Uint32 pno, Uint32 dPortsMask)
{
    FILE *fp = NULL;
    char *tmp = NULL;
    char chipRegStr[50], buffer[20];
    Uint32 curDports = 0U, pvlanMask = 0U;
    SystemProfile       *profSys = NULL;

    profSys = (SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    memset(chipRegStr, 0, sizeof(chipRegStr));
    snprintf(chipRegStr, 50, "arswitch reg r %d 0 24", pno);
    fp = popen(chipRegStr, "r"); 
    if(fp != NULL)
    {
	memset(buffer, 0, sizeof(buffer));
	if (fscanf(fp, "%*s%*s%*s%*s%*s%*s%s", buffer) == 1)/*数据*/
	{
	    tmp = (char *)buffer;
	    sprintf(buffer, "%s", tmp + 6);
	    curDports = strtoul(buffer, 0 ,16);
	    SWORD_PRINTF("%s------%d: curDports = %x\n", __func__, __LINE__, curDports);
	    pvlanMask = convertLenToBinMask(profSys->lanPortCount);/*获取设置区域掩码*/

	    /*只修改需要修改的。其他保持不变*/
	    /*这里可以看到 821和 811只要修改lanPortCount 即可达到要设置的效果
	     * 因为821使用了 port号较大的作为wan口*/
	    dPortsMask =  (curDports & ~ pvlanMask) | (dPortsMask & pvlanMask);
	    doSystem("arswitch reg w %u 0 24 %x", pno, dPortsMask);
	    SWORD_PRINTF("%s------%d: arswitch reg w %u 0 24 %x\n", __func__, __LINE__, pno, dPortsMask);
	}
	pclose(fp);/*关闭通道*/
    }
    return;
}
#endif

#elif defined(CONFIG_IT8712_AR8316)

/*********************************************************************
 * 函数： pvlan_dports_set
 * 功能： 8316芯片寄存器设置
 * 创建： 2012-09-11
 * 参数： 略
 * 返回:  略
 * 输出： 略
 * 作者： Jianqing.Sun
 ********************************************************************/
void pvlan_dports_set(Uint32 pno, Uint32 dPortsMask)
{
    SystemProfile	*profSys = NULL;
    char chipRegStr[50], buffer[20];
    Uint32  curDports = 0U, pvlanMask = 0U, chipReg = 0U;
    FILE *fp = NULL;
    char *tmp;

    profSys = (SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    /*获取设置区域掩码*/
    pvlanMask = convertLenToBinMask(profSys->lanPortCount);
    /*lan port0-port3 对应的寄存器依次为 0x208-0x508*/
    chipReg = 0x208 + (pno << 8u);

    /*当前寄存器值*/
    memset(chipRegStr, 0, sizeof(chipRegStr));
    snprintf(chipRegStr, 50, "mii_6530G reg r %x", chipReg);
    fp = popen(chipRegStr, "r"); 
    if(fp != NULL)
    {
	memset(buffer, 0, sizeof(buffer));
	if (fscanf(fp, "%*s%*s%*s%s", buffer) == 1)/*数据*/
	{
	    tmp = (char *)buffer;
	    sprintf(buffer, "%s", tmp + 10);
	    curDports = strtoul(tmp, 0 ,16);
	    SWORD_PRINTF("%s------%d: curDports = %x\n", __func__, __LINE__, curDports);
	    /*本端口对应的位置置0
	     * 因为datasheet上的实例是如此做的
	     * 具体原因尚不明确。应该置1也无问题*/
	    dPortsMask &= (~(1U<< pno)); 
	    /*只设置port vlan区域
	     * 16位表示芯片port 0 即cpu口不用设置，默认即可*/
	    dPortsMask = (dPortsMask & pvlanMask) << 17U; 
	    dPortsMask =  (curDports & (~(pvlanMask <<17U)))  | dPortsMask;
	    doSystem("mii_6530G reg w %x %x", chipReg, dPortsMask);
	    SWORD_PRINTF("%s------%d: mii_6530G reg w %x %x\n", __func__, __LINE__, chipReg, dPortsMask);
	}
	pclose(fp);/*关闭通道*/
    }
    return;
}
#elif defined(CONFIG_UTT_RDD_PVLAN)
void pvlan_dports_set(Uint32 pno, Uint32 dPortsMask)
{
    /*内核实现 并区分芯片*/
    rddPvlanSet(pno,dPortsMask);
    return;
}
#else
#error please add port-based vlan function for this chip
#endif

#endif
