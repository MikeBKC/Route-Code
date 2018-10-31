#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/autoconf.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#if (NET_MODE_SWITCH == FYES)

static void netModeSwitchSet();
/*��Ϣ������
 *chen.bin
 *2013-01-29
 *
 */
void netModeSwitchProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType aChangeType, Uint32 index,
        const NetModeSwitchProfile* oldProfileDataPtr, const NetModeSwitchProfile* newProfileDataPtr )
{/*���ڶ˿ڹ�������ʵ�����ô���*/
    switch(aChangeType) {
        case PROF_CHANGE_ADD:
            break;
        case PROF_CHANGE_EDIT:/*�༭*/
	    netModeSwitchSet();
            break;
        case PROF_CHANGE_DEL:
            break;
        case PROF_CHANGE_DELALL:
            break;
        default:/*Ĭ��*/
            break;
    }
}

static void netModeSwitchSet()
{
}

/***
 * ������Ч����
 * chen.bin
 * 2013-01-29
 */
static void netModeSwitchInit(void)
{
	NetModeSwitchProfile *profNMS = NULL;
	BridgeProfile *profBR = NULL;
	profBR = (BridgeProfile *)ProfGetInstPointByIndex(MIB_PROF_BRIDGE,0);
	profNMS = (NetModeSwitchProfile *)ProfGetInstPointByIndex(MIB_PROF_NET_MODE_SWITCH,0);
	if(profNMS!=NULL)
	{
		//���ݹ���ģʽ�����ŵ�״̬��״̬������������һֱ���ֲ��䣬�����жϵ�ǰ���е�״̬
		if(profNMS->bridgeEnable==FUN_ENABLE)
		{
			if(profBR!=NULL)
			{
				profBR->head.active = FUN_ENABLE;
				nvramWriteCommit();
			}
		}
		else
		{
			if(profBR!=NULL)
			{
				profBR->head.active = FUN_DISABLE;
				nvramWriteCommit();
			}
		}
	}
	
    return;
}

/*
 * ��ʼ������
 *chen.bin
 *2012-01-29
 */
void funInitNetModeSwitch(void)
{
    registerForProfileUpdates(MIB_PROF_NET_MODE_SWITCH, (Pointer)NULL, (ProfileUpdateFnc)netModeSwitchProfileChange);
    netModeSwitchInit();//���ó�ʼ����Ч
    return;
}
#endif
