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
/*��Ϣ������
 **/
void apEffectConfProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType aChangeType, Uint32 index,
        const ApEffectConfProfile* oldProfileDataPtr, const ApEffectConfProfile* newProfileDataPtr )
{/*���ڶ˿ڹ�������ʵ�����ô���*/
    switch(aChangeType) {
        case PROF_CHANGE_ADD:
            break;
        case PROF_CHANGE_EDIT:/*�༭*/
	    apEffectConfSet();
            break;
        case PROF_CHANGE_DEL:
            break;
        case PROF_CHANGE_DELALL:
            break;
        default:/*Ĭ��*/
            break;
    }
}

static void apEffectConfSet()
{
}

/***
 * ������Ч����
 * xhhu
 * 2012-07-10
 */
static void apEffectConfInit(void)
{
    return;
}

/*��ʼ������
 * xhhu*/
void funInitApEffectConf(void)
{
    registerForProfileUpdates(MIB_PROF_AP_EFFECT_CONF, (Pointer)NULL, (ProfileUpdateFnc)apEffectConfProfileChange);
    apEffectConfInit();/*���ó�ʼ����Ч*/
    return;
}
#endif
