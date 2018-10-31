#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"

#if (SYS_LOG_INFO == FYES)

#define SWORD_DEBUG 0
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

static void SysLogInfoKlog(const SysLogInfoProfile *oldProf, const SysLogInfoProfile *newProf);

/********************************************************************
* ������ SysLogInfoProfileChange
* ���ܣ� ϵͳ��־���ñ仯 ��Ч����
* ������ 2013-05-03
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void SysLogInfoProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const SysLogInfoProfile *oldProfileDataPtr, const SysLogInfoProfile *newProfileDataPtr )
{
    switch(changeType) 
    {
        case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
	    SysLogInfoKlog(oldProfileDataPtr, newProfileDataPtr);
	    break;
        case PROF_CHANGE_DEL:
        case PROF_CHANGE_DELALL:
        default:/*default*/
            break;
    }
    return;
}

/********************************************************************
* ������ SysLogInfoKlog
* ���ܣ� ϵͳ��־���ñ仯 ��Ч����
* ������ 2013-05-03
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void SysLogInfoKlog(const SysLogInfoProfile *oldProf, const SysLogInfoProfile *newProf)
{
	int logSwitchOld = 0;
	int logSwitchNew = 0;
	if (newProf != NULL) {
		/*�µĿ��ؽ��*/
		logSwitchNew = 
			(0 
#ifdef CONFIG_UTT_DHCP_COLLISION
			 | (newProf->DhcpLogEnable)
#endif
#ifdef CONFIG_UTT_NOTICE_LOG
			 | (newProf->NoticeLogEnable << 1) 
#endif
#ifdef CONFIG_UTT_ARP_COLLISION
			 | (newProf->ArpLogEnable << 2)
#endif
			);
		/*�ɵĿ��ؽ��*/
		if(oldProf != NULL) {
			logSwitchOld = 
				(0 
#ifdef CONFIG_UTT_DHCP_COLLISION
				 | (oldProf->DhcpLogEnable)
#endif
#ifdef CONFIG_UTT_NOTICE_LOG
				 | (oldProf->NoticeLogEnable << 1) 
#endif
#ifdef CONFIG_UTT_ARP_COLLISION
				 | (oldProf->ArpLogEnable << 2)
#endif
				);

			SWORD_PRINTF("%s------%d, logSwitchOld = %x, logSwitchNew= %x\n",__func__, __LINE__, logSwitchOld, logSwitchNew);
			if((logSwitchOld == 0) && (logSwitchNew != 0)) {
				/*�رյ�����*/
				doSystem("klogd &");
			} else if((logSwitchOld != 0) && (logSwitchNew == 0)) {
				/*�������ر�*/
				doSystem("killall klogd");
			} else {
				/*test*/
			}
		} else {
			/*������ʼ��*/
			if(logSwitchNew != 0) {
				doSystem("klogd &");
			}
		} 
		doSystem("echo %d > /proc/sys/net/ipv4/netfilter/utt_collision_detection", logSwitchNew);
	}
	return;
}

/********************************************************************
* ������ SysLogInfoInit
* ���ܣ� ϵͳ��־��ʼ������ - ��������й����������������
* ������ 2013-05-03
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void SysLogInfoInit(void)
{
    MibProfileType mibType = MIB_PROF_SYS_LOG_INFO;
    SysLogInfoProfile *prof = NULL;

    prof = (SysLogInfoProfile *)ProfGetInstPointByIndex(mibType, 0);
    SysLogInfoKlog((const SysLogInfoProfile *)NULL, (const SysLogInfoProfile *)prof);

    return;
}

/********************************************************************
* ������ funInitSysLogInfo
* ���ܣ� ϵͳ��־���� ��ʼ����ں���
* ������ 2013-05-03
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
void funInitSysLogInfo(void)
{
    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
    registerForProfileUpdates(MIB_PROF_SYS_LOG_INFO, (Pointer)NULL, (ProfileUpdateFnc)SysLogInfoProfileChange);/*��������*/

    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
    SysLogInfoInit();/*��ʼ��*/
    return;
}
#endif
