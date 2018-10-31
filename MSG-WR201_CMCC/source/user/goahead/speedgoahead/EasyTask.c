#include "uttMachine.h"
#if (EASY_TASK == FYES)
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"
#include "spdComFun.h"
#include "comExeFun.h"
#include "ifmisc.h"
#include "typedef.h"

#define SWORD_DEBUG 0
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

extern void LinkChkChg(int wanIndex, const InterfaceProfile *prof);
extern void checkDefaultConf(void);
#if (HOTEL_PNP == FYES)
extern void addGwForETask(void);
extern void delRtForETask(void);
#endif
static void easyTaskInit(void);
static void easyTaskProfileChange(Pointer tag, MibProfileType profileType, ProfChangeType changeType, Uint32 index, const EasyTaskProfile *oldProfileDataPtr, const EasyTaskProfile *newProfileDataPtr);
extern void funInitEasyTask(void); 
#if (WIRELESS == FYES)
extern void WirelessProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr);
#endif

#if (HOTEL_PNP == FYES)
/*
 * ���ܣ�WAN�ڶ��Ͽ������Ϊ�Ƶ깦��������غ�·��
 * ����ʱ�䣺2013-6-7
 */
extern void addGwForETask(void)
{
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;

    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibTypeSys, 0);
    prof = (EasyTaskProfile*)ProfGetInstPointByIndex(mibType, 0);
    /*wanLinkStates--wan��״̬��0Ϊȫ�Ͽ���hotelPnpEn--�Ƶ깦��*/
    if ((ProfInstIsFree(profSys) == 0) && (profSys->hotelPnpEn == FUN_ENABLE) && (ProfInstIsFree(prof) == 0) && (prof->wanLinkStates == 0U))
    {
	doSystem("ifconfig %s:1 202.160.134.162 netmask 255.255.255.252", getWanIfName(1));
	sleep(1);/*ͨ��˯�߱�֤·����ӳɹ�����������·�����ʧ�����*/
	doSystem("ip route replace default equalize nexthop via 202.160.134.161 dev %s:1 weight 1", getWanIfName(1));
    }
}

/*
 * ���ܣ�ɾ��Ϊ�Ƶ���ӵ����غ�·�ɣ���ֹ���ִ���
 * ����ʱ�䣺2013-6-7
 */
extern void delRtForETask(void)
{
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;
    MibProfileType mibTypeIf = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;

    prof = (EasyTaskProfile*)ProfGetInstPointByIndex(mibType, 0);
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibTypeIf, 1);
    
    /*����һλ��ʾWAN1���Դ�����*/
    if ((ProfInstIsFree(prof) == 0) && ((prof->wanLinkStates & ((unsigned int)1U << 1U)) == 0U))
    {
	if (ProfInstIsFree(profIf) == 0)
	{
	    doSystem("ifconfig %s:1 0.0.0.0 1>/dev/null 2>&1", getWanIfName(1));/*ɾ��WAN1��ӿ�IP��ַ*/
	}
    }
}
#endif

#if 0
#if (WIRELESS == FYES)
#define MIN_ACTIVE_TIME 1356998400u
/*
 *  wireless_switch_change	-	���߿��ظı亯��
 *  
 *  ������
 *	status			-	���ش򿪻��߹ر�
 */
void wireless_switch_change(int status)
{
	MibProfileType mibType = MIB_PROF_WIRELESS;
	WirelessProfile *prof = NULL, oldprof;

	prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 0);

	SWORD_PRINTF("%s-%d:status = %d\n", __func__, __LINE__, status);
	if (prof->head.active != status) {
		memcpy(&oldprof, prof, sizeof(oldprof));
		prof->head.active = status;

		WirelessProfileChange((Pointer)NULL, mibType, PROF_CHANGE_EDIT, 0, &oldprof, prof);
	}
	return;
}
/*
 *	wireless_date_check	-	�ж��Ƿ���������Чʱ�䲢������
 *
 * ����ֵ��
 *	status	
 *	0			-	������Чʱ����
 *	1			-	����Чʱ����
 *	-1			-	ϵͳʱ�䲻��
 */
int wireless_date_check(void)
{
	MibProfileType mibType = MIB_PROF_WIRELESS;
	WirelessProfile *prof = NULL;

	unsigned int tmCurrent = 0u;
	unsigned int daytime = 0u, minute1 = 0u, minute2 = 0u, hour1 = 0u, hour2 = 0u, wday = 0u;
	int status = 0;

	prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 0);
	tmCurrent = time(NULL);

	if (tmCurrent < MIN_ACTIVE_TIME)
		status = -1;
	else {
		/*��ȡ���ڣ�ʱ����*/
		wday = (4 + tmCurrent / 86400) % 7; 
		daytime = tmCurrent % 86400;

		SWORD_PRINTF("%s-%d:wday = %u, daytime = %u\n", __func__, __LINE__, wday, daytime);

		/*�����촦��Ϊ������*/
		if (wday == 0)
			wday = 7;

		/*�жϴ����Ƿ�����Чʱ����*/
		if (((1u << (7u - wday)) & prof->mBaseCf.day)) {
			/*�ж��ǹ�����ʱ�λ�����ĩʱ��*/
			if ((wday >= 1) && (wday <= 5)) {
				sscanf(prof->mBaseCf.weekdayTimeStart, "%u:%u", &hour1, &minute1);/*����ʱ��*/
				sscanf(prof->mBaseCf.weekdayTimeStop, "%u:%u", &hour2, &minute2);/*����ʱ��*/
			} else {
				sscanf(prof->mBaseCf.weekendTimeStart, "%u:%u", &hour1, &minute1);/*����ʱ��*/
				sscanf(prof->mBaseCf.weekendTimeStop, "%u:%u", &hour2, &minute2);/*����ʱ��*/
			}
			/*�ж��Ƿ��ڿ�ʼʱ�䵽����ʱ��֮��*/
			if ((daytime >= (hour1 * 3600 + minute1 * 60)) && (daytime <= (hour2 * 3600 + minute2 * 60)))
				status = 1;
		}
	}
	SWORD_PRINTF("%s-%d:status = %d\n", __func__, __LINE__, status);
	return status;
}
/*
 *	getWeekdayForCrond	-	��ȡ�ƻ������������ڲ���
 *
 * ������
 *	day			-	��������
 * ����ֵ��
 *	daystr			-	��������ַ���
 */
static char *getWeekdayForCrond(unsigned int day)
{
	char tmpstr[5];
	static char daystr[14];
	unsigned int i, j = 0u;

	memset(tmpstr, 0, 5);
	memset(daystr, 0, 14);

	/*��ȡ���ڼ�*/
	for (i = 1u; i <= 5u; i++) {
		if ((1u << (7u -i)) & day) {
			sprintf(tmpstr, "%d,", i);
			strcat(daystr, tmpstr);
			j++;
		}
	}
	/*ȥ�����һ������Ķ���*/
	if (j != 0)
		daystr[2u * j - 1u] = '\0';
	return daystr;
}
#if 0
/*
 *	editCronRule		-	�޸ļƻ��������
 *
 * ������
 *	action			-	��ӻ���ɾ��
 *	prof			-	���ݴ洢��profile
 */
static void editCronRule(char *action, const EasyTaskProfile *prof)
{
	char tmpstr[5];
	char weekdaystr[14];
	char weekendstr[14];
	unsigned int j = 0u;

	memset(tmpstr, 0, 5);
	memset(weekdaystr, 0, 14);
	memset(weekendstr, 0, 14);

	/*��ȡ����������*/
	strcpy(weekdaystr, getWeekdayForCrond(prof->day));
	
	/*��ȡ��ĩ����*/
	if ((1u << 0) & prof->day) {
		sprintf(tmpstr, "%d", 0);
		strcat(weekendstr, tmpstr);
		j++;
	}
	if ((1u << 1) & prof->day) {
		if (j == 1)
			strcat(weekendstr, ",");
		sprintf(tmpstr, "%d", 6);
		strcat(weekendstr, tmpstr);
	}

	if (weekdaystr[0] != '\0') {
		/*���������ռƻ�����*/
		doSystem("cron-init.sh %s \"%s * * %s snd2speed wireless start\"", action, getTimeForCrond((Uchar*)prof->weekdayTimeStart), weekdaystr);
		doSystem("cron-init.sh %s \"%s * * %s snd2speed wireless stop\"", action, getTimeForCrond((Uchar*)prof->weekdayTimeStop), weekdaystr);
	}

	if (weekendstr[0] != '\0') {
		/*������ĩ�ƻ�����*/
		doSystem("cron-init.sh %s \"%s * * %s snd2speed wireless start\"", action, getTimeForCrond((Uchar*)prof->weekendTimeStart), weekendstr);
		doSystem("cron-init.sh %s \"%s * * %s snd2speed wireless stop\"", action, getTimeForCrond((Uchar*)prof->weekendTimeStop), weekendstr);
	}
	return;
}
#endif
/*
 *	WlanTimeEdit		-	������Чʱ���޸�
 * 
 * ������
 *	oldProfileDataPtr	-	ԭ��Profile
 *	newProfileDataPtr	-	�µ�Profile
 */
static void WlanTimeEdit(const EasyTaskProfile *oldProfileDataPtr, const EasyTaskProfile *newProfileDataPtr)
{
	int status = 0;
#if 0
	editCronRule("del", oldProfileDataPtr);
	editCronRule("add", newProfileDataPtr);
#endif
	WirelessProfile *prof = NULL;
	MibProfileType mibType = MIB_PROF_WIRELESS;
	prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 0);
#if (WIRELESS_5G == FYES)
	WirelessProfile *prof5G = NULL;
	prof5G = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 1);
#endif
	status = wireless_date_check();
	if (status != -1) {
		wireless_switch_change(status,prof);
#if (WIRELESS_5G == FYES)
		wireless_switch_change(status,prof5G);
#endif
	}
	return;
}
#endif
#endif

/********************************************************************
 * ������ easyTaskInit
 * ���ܣ� ��ʼ������
 * ����ʱ�䣺2013-05-13
 ********************************************************************/ 
static void easyTaskInit(void) 
{
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;
    char lanIp[16];

    prof = (EasyTaskProfile*)ProfGetInstPointByIndex(mibType, 0);

    if (ProfInstIsFree(prof) == 0)
    {
	memset(lanIp, 0, sizeof(lanIp));
	getIfIp(getLanIfName(), lanIp);
	if (prof->easyTaskNoticeEn == FUN_ENABLE)
	{
	    editLANRedirectRule('D', lanIp);
	    editLANRedirectRule('A', lanIp);/*���DNS��ת����*/
	}
    }    
#if (HOTEL_PNP == FYES)
    addGwForETask();/*Ϊ�Ƶ깦��������غ�·��*/
#endif
}


/********************************************************************
 * ������ easyTaskProfileChange
 * ���ܣ� profile change ����
 * ����ʱ�䣺2013-05-13
 ********************************************************************/ 
static void easyTaskProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const EasyTaskProfile *oldProfileDataPtr, const EasyTaskProfile *newProfileDataPtr)
{
    char lanIp[16];
    switch(changeType) 
    {
	case PROF_CHANGE_ADD:
	case PROF_CHANGE_EDIT:
	    memset(lanIp, 0, sizeof(lanIp));
	    getIfIp(getLanIfName(), lanIp);/*LAN��IP*/
	    /*�������ر�*/ 
	    if((oldProfileDataPtr->easyTaskNoticeEn == FUN_ENABLE) && (newProfileDataPtr->easyTaskNoticeEn == FUN_DISABLE))
	    {
		editLANRedirectRule('D', lanIp);
	    }
	    /*�رյ�����*/
	    if((oldProfileDataPtr->easyTaskNoticeEn == FUN_DISABLE) && (newProfileDataPtr->easyTaskNoticeEn == FUN_ENABLE) && (newProfileDataPtr->wanLinkStates == 0U))
	    {
		editLANRedirectRule('D', lanIp);/*���֮ǰ��ɾ��*/
		editLANRedirectRule('A', lanIp);
	    }
#if (HOTEL_PNP ==FYES || EASY_TASK == FYES || UTT_DOMAINACCESS  ==FYES)
	    editDNSRedirectRule();
#endif
#if 0
#if (WIRELESS == FYES)
	    WlanTimeEdit(oldProfileDataPtr, newProfileDataPtr);
#endif
#endif
	    break;
	case PROF_CHANGE_DEL:
	case PROF_CHANGE_DELALL:
	default:
	    /*null*/
	    break;
    }
    return;
}

/********************************************************************
 * ������ funInitEasyTask
 * ���ܣ� ע�ắ��
 * ����ʱ�䣺2013-05-13
 ********************************************************************/ 
extern void funInitEasyTask(void) 
{
    registerForProfileUpdates(MIB_PROF_EASY_TASK, NULL, (ProfileUpdateFnc)easyTaskProfileChange);
    easyTaskInit();
    return;
}
#endif
