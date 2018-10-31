#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/autoconf.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#if (FEATURE_AP_EFFECT_CONF == FYES)

static void apEffectConfSet();
/*消息处理函数
 **/
void apEffectConfProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType aChangeType, Uint32 index,
        const ApEffectConfProfile* oldProfileDataPtr, const ApEffectConfProfile* newProfileDataPtr )
{/*对于端口管理所有实例永久存在*/
    switch(aChangeType) {
        case PROF_CHANGE_ADD:
            break;
        case PROF_CHANGE_EDIT:/*编辑*/
	    apEffectConfSet();
            break;
        case PROF_CHANGE_DEL:
            break;
        case PROF_CHANGE_DELALL:
            break;
        default:/*默认*/
            break;
    }
}

static void apEffectConfSet()
{
}

/***
 * 配置生效函数
 * xhhu
 * 2012-07-10
 */
static void apEffectConfInit(void)
{
    return;
}

/*初始化函数
 * xhhu*/
void funInitApEffectConf(void)
{
    registerForProfileUpdates(MIB_PROF_AP_EFFECT_CONF, (Pointer)NULL, (ProfileUpdateFnc)apEffectConfProfileChange);
    apEffectConfInit();/*配置初始化生效*/
    return;
}
#endif
