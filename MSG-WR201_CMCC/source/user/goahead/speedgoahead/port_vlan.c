
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/autoconf.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"

#if (FEATURE_PORT_VLAN == FYES)
#include <linux/switch/swioc_common.h>
#include <sw_ioctl.h>

#define DEBUG_PVLAN 0
#if DEBUG_PVLAN
#define PVLAN_DEBUG(fmt, argx...) printf(fmt, ##argx)
#else
#define PVLAN_DEBUG(fmt, argx...)
#endif



static int pvlan_member_init(int index, int is_del);
static void pvlan_init(int is_del);

static void portVlanProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const PortVlanProfile *oldProfileDataPtr, const PortVlanProfile *newProfileDataPtr )
{
    PVLAN_DEBUG("%s\n", __func__);
    switch(changeType) {
        case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
            pvlan_member_init(index, 0);
	    break;
        case PROF_CHANGE_DEL:
            pvlan_member_init(index, 1);
	    break;
        case PROF_CHANGE_DELALL:
	    pvlan_init(1);
	    break;
        default:
	    break;
    };
}

void funInitPortVlan(void)
{
    PVLAN_DEBUG("%s\n", __func__);
    registerForProfileUpdates(MIB_PROF_PORT_VLAN, NULL, portVlanProfileChange);
    pvlan_init(0);
}
#if DEBUG_PVLAN
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
static void add_default_pvlan(void)
{
    int i, instIndex;
    PortVlanProfile *prof;
    MibProfileType mibType = MIB_PROF_PORT_VLAN;

    PVLAN_DEBUG("%s\n", __func__);
    instIndex = 0;
    prof = (PortVlanProfile*)ProfGetInstPointByIndex(mibType, instIndex);
    prof->head.active=TRUE;
    /*pvlan做name*/
    ProfSetNameByPoint(prof, "pvlan1");
    i = sizeof(prof->description);
    memset(prof->description, 0, i);
    memset(prof->members, 0x01, sizeof(prof->members));

#if 1
    pvlan_member_init(instIndex, 0);
#else
    ProfBackupByIndex(mibType, PROF_CHANGE_ADD, instIndex, &profhead);

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);   
    ProfWiteToFlash();
#endif
    return;
}
/*
 *初始化一条配置
 *
 *KeHuatao
 *2011-08-30
 */
static int pvlan_member_init(int index, int is_del)
{
    int max, min, ret=-1;
    BOOL members[PORT_ARRAY_SIZE];
    PortVlanProfile *prof;
    MibProfileType mibType = MIB_PROF_PORT_VLAN;

    ProfInstLowHigh(mibType, &max, &min);
    if((index>=min) && (index<max))
    {
	if(is_del == 1)/*delete profile*/
	{
	    memset(members, 0, sizeof(members));
	    PVLAN_DEBUG("%s: del index %d\n", __func__, index);
	    /*设置寄存器*/
	    sw_pvlan_port_members_set((index+1), members);
	    ret = 0u;
	}
	else
	{
	    prof = (PortVlanProfile*)ProfGetInstPointByIndex(mibType, index);
	    if(prof != NULL)
	    {
		if((ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
		{
		    PVLAN_DEBUG("%s: index %d\n", __func__, index);
#if DEBUG_PVLAN
		    print_members(prof->members, sizeof(prof->members));
#endif
		    /*设置寄存器*/
		    sw_pvlan_port_members_set((index+1), prof->members);
		    ret = 0u;
		}
	    }
	}
    }

    return ret;
}
/*
 *
 *初始化pvlan
 *KeHuatao
 *2011-08-30
 */
static void pvlan_init(int is_del)
{
    int max, min;
    pvlan_no_t pvlan_no;
    int found, vlan_mode;

    PVLAN_DEBUG("%s\n", __func__);
    found = 0u;
    /*当前vlan类型*/
    vlan_mode = sw_vlan_type_get();
    if(vlan_mode==VLAN_TYPE_PORT)
    {
	ProfInstLowHigh(MIB_PROF_PORT_VLAN, &max, &min);
	for(pvlan_no = min; pvlan_no<max; pvlan_no++)
	{
	    if(pvlan_member_init(pvlan_no, is_del)==0)
	    {
		found = 1u;
	    }
	}
    }

    if( (found == 0u) /*未设置过*/
#ifdef CONFIG_IP1725
        && (vlan_mode==VLAN_TYPE_PORT)
#endif
      )
    {
	/*添加默认pvlan*/
	add_default_pvlan();
    }

    return;
}
#endif
