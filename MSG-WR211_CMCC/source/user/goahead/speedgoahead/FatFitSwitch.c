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
/*��Ϣ������
 **/
void fatFitSwitchProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType aChangeType, Uint32 index,
        const FatFitSwitchProfile* oldProfileDataPtr, const FatFitSwitchProfile* newProfileDataPtr )
{/*���ڶ˿ڹ�������ʵ�����ô���*/
    switch(aChangeType) {
        case PROF_CHANGE_ADD:
            break;
        case PROF_CHANGE_EDIT:/*�༭*/
	    fatFitSwitchSet();
            break;
        case PROF_CHANGE_DEL:
            break;
        case PROF_CHANGE_DELALL:
            break;
        default:/*Ĭ��*/
            break;
    }
}

static void fatFitSwitchSet()
{
}

/***
 * ������Ч����
 * bhou
 * 2011-08-31
 */
static void fatFitSwitchInit(void)
{
    return;
}

/*��ʼ������
 * bhou*/
void funInitFatFitSwitch(void)
{
    registerForProfileUpdates(MIB_PROF_FAT_FIT_SWITCH, (Pointer)NULL, (ProfileUpdateFnc)fatFitSwitchProfileChange);
    fatFitSwitchInit();/*���ó�ʼ����Ч*/
    return;
}
#endif
