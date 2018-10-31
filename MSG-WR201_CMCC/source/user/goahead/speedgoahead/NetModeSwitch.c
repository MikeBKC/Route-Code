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
/*消息处理函数
 *chen.bin
 *2013-01-29
 *
 */
void netModeSwitchProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType aChangeType, Uint32 index,
        const NetModeSwitchProfile* oldProfileDataPtr, const NetModeSwitchProfile* newProfileDataPtr )
{/*对于端口管理所有实例永久存在*/
    switch(aChangeType) {
        case PROF_CHANGE_ADD:
            break;
        case PROF_CHANGE_EDIT:/*编辑*/
	    netModeSwitchSet();
            break;
        case PROF_CHANGE_DEL:
            break;
        case PROF_CHANGE_DELALL:
            break;
        default:/*默认*/
            break;
    }
}

static void netModeSwitchSet()
{
}

/***
 * 配置生效函数
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
		//根据工作模式设置桥的状态，状态从启动到重启一直保持不变，用于判断当前运行的状态
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
 * 初始化函数
 *chen.bin
 *2012-01-29
 */
void funInitNetModeSwitch(void)
{
    registerForProfileUpdates(MIB_PROF_NET_MODE_SWITCH, (Pointer)NULL, (ProfileUpdateFnc)netModeSwitchProfileChange);
    netModeSwitchInit();//配置初始化生效
    return;
}
#endif
