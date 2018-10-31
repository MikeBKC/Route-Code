#include "uttMachine.h"
#if (SYSLOG == FYES)
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"

/********************************************************************
 * ������ SyslogInit
 * ���ܣ� ��ʼ������
 * ������ 2012-10-16
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static void SyslogInit(void) 
{
    SyslogProfile *profSyslog = NULL;
    MibProfileType mibType = MIB_PROF_SYSLOG;
    profSyslog = (SyslogProfile*)ProfGetInstPointByIndex(mibType, 0);

    if((profSyslog != NULL) && (ProfInstIsFree(profSyslog) == 0)) 
    {
	doSystem("syslog.sh");/*���ýű�*/
    }
    return;
}

/********************************************************************
 * ������ SyslogProfileChange
 * ���ܣ� profile change ����
 * ������ 2012-10-16
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static void SyslogProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const SyslogProfile *oldProfileDataPtr, const SyslogProfile *newProfileDataPtr)
{
    switch(changeType) 
    {
	case PROF_CHANGE_ADD:
	case PROF_CHANGE_EDIT:
	    if(memcmp(oldProfileDataPtr, newProfileDataPtr, sizeof(SyslogProfile)) != 0)
	    {
		doSystem("syslog.sh");
	    }
	    break;
	case PROF_CHANGE_DEL:
	case PROF_CHANGE_DELALL:
	default:/*test*/
	    break;
    }
    return;
}

/********************************************************************
 * ������ funInitSyslog
 * ���ܣ� ��ʼ������
 * ������ 2012-10-16
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
extern void funInitSyslog(void) 
{
    registerForProfileUpdates(MIB_PROF_SYSLOG, NULL, (ProfileUpdateFnc)SyslogProfileChange);
    SyslogInit();
    return;
}
#endif
