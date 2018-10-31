#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/autoconf.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#if (FAT_FIT_SWITCH == FYES)

static void fatFitSwitchSet();
/*消息处理函数
 **/
void fatFitSwitchProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType aChangeType, Uint32 index,
        const FatFitSwitchProfile* oldProfileDataPtr, const FatFitSwitchProfile* newProfileDataPtr )
{/*对于端口管理所有实例永久存在*/
    switch(aChangeType) {
        case PROF_CHANGE_ADD:
            break;
        case PROF_CHANGE_EDIT:/*编辑*/
	    fatFitSwitchSet();
            break;
        case PROF_CHANGE_DEL:
            break;
        case PROF_CHANGE_DELALL:
            break;
        default:/*默认*/
            break;
    }
}

static void fatFitSwitchSet()
{
}

/***
 * 配置生效函数
 * bhou
 * 2011-08-31
 */
static void fatFitSwitchInit(void)
{
    return;
}

/*初始化函数
 * bhou*/
void funInitFatFitSwitch(void)
{
    registerForProfileUpdates(MIB_PROF_FAT_FIT_SWITCH, (Pointer)NULL, (ProfileUpdateFnc)fatFitSwitchProfileChange);
    fatFitSwitchInit();/*配置初始化生效*/
    return;
}
#endif
