
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
 * ������ rddPortVlanProfileChange
 * ���ܣ� ���ñ仯 ��Ч����
 * ������ 2012-09-11
 * ������ ��
 * ����:  ��
 * ����� ��
 * ���ߣ� Jianqing.Sun
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
 * ������ funInitRddPortVlan
 * ���ܣ� ��ʼ������
 * ������ 2012-09-11
 * ������ ��
 * ����:  ��
 * ����� ��
 * ���ߣ� Jianqing.Sun
 ********************************************************************/
void funInitRddPortVlan(void)
{
    registerForProfileUpdates(MIB_PROF_RDD_PORT_VLAN, NULL, (ProfileUpdateFnc)rddPortVlanProfileChange);
#if (UTT_NV_840E == FYES) || (UTT_NV_800 == FYES) || (UTT_NV_840D == FYES)
    /* ���Էŵ��ں��г�ʼ�� */
    init_840E_vlan_reg();
#endif
    pvlan_init();
    add_default_pvlan();
}
#if SWORD_DEBUG
/*********************************************************************
 * ������ print_members
 * ���ܣ� ��Ա�������
 * ������ 2012-09-11
 * ������ ��
 * ����:  ��
 * ����� ��
 * ���ߣ� Jianqing.Sun
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
 * ������ add_default_pvlan
 * ���ܣ� ��ʼ��û��vlan����Ĭ��vlan����
 * ������ 2012-09-11
 * ������ ��
 * ����:  ��
 * ����� ��
 * ���ߣ� Jianqing.Sun
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
	{/*������Vlan*/
	    break;
	}
    }
    SWORD_PRINTF("%s------%d, i = %d\n", __func__, __LINE__, i);
    if(i == max)/*iΪ���ֵ���ʾû�������κ�Vlan*/
    {
	i = 0;
	prof = (RddPortVlanProfile*)ProfGetInstPointByIndex(mibType, i);
	prof->head.active=TRUE;
	/*pvlan��name*/
	ProfSetNameByPoint(prof, "pvlan1");
	memset(prof->description, 0, sizeof(prof->description));
	memset(prof->members, 0x01, sizeof(prof->members));
    }

    return;
}


/*********************************************************************
 * ������ pvlan_init
 * ���ܣ� ��ʼ��pvlan
 * ������ 2012-09-11
 * ������ ��
 * ����:  ��
 * ����� ��
 * ���ߣ� Jianqing.Sun
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
    /*��ǰvlan����*/
    ProfInstLowHigh(MIB_PROF_RDD_PORT_VLAN, &max, &min);
    /*�������ж˿ڿ�ͨ�Ŷ˿�*/
    for(pno = 0;pno < profSys->lanPortCount; pno++)
    {
	dPortsMask = 1 << pno;
	/*��������profile�����Ұ���pno��profile�������ͨ��Ŀ�Ķ˿�*/
	for(pvlan_index = min; pvlan_index < max; pvlan_index++)
	{
	    pvlanprof= (RddPortVlanProfile*)ProfGetInstPointByIndex(mibType, pvlan_index);
	    if((pvlanprof != NULL) && (ProfInstIsFree(pvlanprof) == 0) && (pvlanprof->head.active==TRUE))
	    {
		if(flag == 0)/*�ж��Ƿ������ô���*/
		{
		    flag = 1;
		}
		SWORD_PRINTF("%s------%d: name = %s\n", __func__, __LINE__, pvlanprof->head.name);
		/*��Ա����pno����pno�����vlan���г�Աͨ�� */
		if(pvlanprof->members[pno +1] == 1)
		{
		    /*�����ͨ�ŵ�Ŀ�Ķ˿�*/
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
	if(flag == 1)/*�����ô��ڲų�ʼ������ֹ��������ʱû������Ҳ�����������üĴ���*/
	{
	    pvlan_dports_set(pno, dPortsMask);/*���üĴ���ʹ�ö˿�ֻ�ܺ�Ŀ�Ķ˿�ͨ��*/
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
    pvlanMask = convertLenToBinMask(profSys->lanPortCount);/*��ȡ������������*/

    _840E_gpio_read(pno, 0, 24, &regVal);
    regVal = (regVal & ~ pvlanMask) | (dPortsMask & pvlanMask);
    _840E_gpio_write(pno, 0, 24, regVal);

    return;
}

/*
 * ���ܣ�
 * ��ʼ��840E 8306оƬvlan��ؼĴ���
 * ��������vlan���ܡ�����4 lan����ͬһvlan��
 */
static void init_840E_vlan_reg(void) {
    Uint32 regVal = 0U, pvlanMask = 0U;
    SystemProfile       *profSys = NULL;
    int i;

    /* ����vlan:phy 0 reg 18 page 0 bit 8 value 0 */
    _840E_gpio_read(0, 0, 18, &regVal);
    regVal &= ~(1 << 8);
    _840E_gpio_write(0, 0, 18, regVal);

    /* 4 lan����ͬһvlan�� */
    profSys = (SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    pvlanMask = convertLenToBinMask(profSys->lanPortCount);/*��ȡ������������*/

    for (i = 0; i < profSys->lanPortCount; i++) {
        _840E_gpio_read(i, 0, 24, &regVal);
	regVal |= pvlanMask;
	_840E_gpio_write(i, 0, 24, regVal);
    }

    return;
}
#else
/*********************************************************************
 * ������ pvlan_dports_set
 * ���ܣ� 8306оƬ�Ĵ�������
 * ������ 2012-09-11
 * ������ ��
 * ����:  ��
 * ����� ��
 * ���ߣ� Jianqing.Sun
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
	if (fscanf(fp, "%*s%*s%*s%*s%*s%*s%s", buffer) == 1)/*����*/
	{
	    tmp = (char *)buffer;
	    sprintf(buffer, "%s", tmp + 6);
	    curDports = strtoul(buffer, 0 ,16);
	    SWORD_PRINTF("%s------%d: curDports = %x\n", __func__, __LINE__, curDports);
	    pvlanMask = convertLenToBinMask(profSys->lanPortCount);/*��ȡ������������*/

	    /*ֻ�޸���Ҫ�޸ĵġ��������ֲ���*/
	    /*������Կ��� 821�� 811ֻҪ�޸�lanPortCount ���ɴﵽҪ���õ�Ч��
	     * ��Ϊ821ʹ���� port�Žϴ����Ϊwan��*/
	    dPortsMask =  (curDports & ~ pvlanMask) | (dPortsMask & pvlanMask);
	    doSystem("arswitch reg w %u 0 24 %x", pno, dPortsMask);
	    SWORD_PRINTF("%s------%d: arswitch reg w %u 0 24 %x\n", __func__, __LINE__, pno, dPortsMask);
	}
	pclose(fp);/*�ر�ͨ��*/
    }
    return;
}
#endif

#elif defined(CONFIG_IT8712_AR8316)

/*********************************************************************
 * ������ pvlan_dports_set
 * ���ܣ� 8316оƬ�Ĵ�������
 * ������ 2012-09-11
 * ������ ��
 * ����:  ��
 * ����� ��
 * ���ߣ� Jianqing.Sun
 ********************************************************************/
void pvlan_dports_set(Uint32 pno, Uint32 dPortsMask)
{
    SystemProfile	*profSys = NULL;
    char chipRegStr[50], buffer[20];
    Uint32  curDports = 0U, pvlanMask = 0U, chipReg = 0U;
    FILE *fp = NULL;
    char *tmp;

    profSys = (SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    /*��ȡ������������*/
    pvlanMask = convertLenToBinMask(profSys->lanPortCount);
    /*lan port0-port3 ��Ӧ�ļĴ�������Ϊ 0x208-0x508*/
    chipReg = 0x208 + (pno << 8u);

    /*��ǰ�Ĵ���ֵ*/
    memset(chipRegStr, 0, sizeof(chipRegStr));
    snprintf(chipRegStr, 50, "mii_6530G reg r %x", chipReg);
    fp = popen(chipRegStr, "r"); 
    if(fp != NULL)
    {
	memset(buffer, 0, sizeof(buffer));
	if (fscanf(fp, "%*s%*s%*s%s", buffer) == 1)/*����*/
	{
	    tmp = (char *)buffer;
	    sprintf(buffer, "%s", tmp + 10);
	    curDports = strtoul(tmp, 0 ,16);
	    SWORD_PRINTF("%s------%d: curDports = %x\n", __func__, __LINE__, curDports);
	    /*���˿ڶ�Ӧ��λ����0
	     * ��Ϊdatasheet�ϵ�ʵ�����������
	     * ����ԭ���в���ȷ��Ӧ����1Ҳ������*/
	    dPortsMask &= (~(1U<< pno)); 
	    /*ֻ����port vlan����
	     * 16λ��ʾоƬport 0 ��cpu�ڲ������ã�Ĭ�ϼ���*/
	    dPortsMask = (dPortsMask & pvlanMask) << 17U; 
	    dPortsMask =  (curDports & (~(pvlanMask <<17U)))  | dPortsMask;
	    doSystem("mii_6530G reg w %x %x", chipReg, dPortsMask);
	    SWORD_PRINTF("%s------%d: mii_6530G reg w %x %x\n", __func__, __LINE__, chipReg, dPortsMask);
	}
	pclose(fp);/*�ر�ͨ��*/
    }
    return;
}
#elif defined(CONFIG_UTT_RDD_PVLAN)
void pvlan_dports_set(Uint32 pno, Uint32 dPortsMask)
{
    /*�ں�ʵ�� ������оƬ*/
    rddPvlanSet(pno,dPortsMask);
    return;
}
#else
#error please add port-based vlan function for this chip
#endif

#endif
