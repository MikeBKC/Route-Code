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
 * 功能：WAN口都断开情况下为酒店功能添加网关和路由
 * 创建时间：2013-6-7
 */
extern void addGwForETask(void)
{
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;

    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibTypeSys, 0);
    prof = (EasyTaskProfile*)ProfGetInstPointByIndex(mibType, 0);
    /*wanLinkStates--wan口状态，0为全断开，hotelPnpEn--酒店功能*/
    if ((ProfInstIsFree(profSys) == 0) && (profSys->hotelPnpEn == FUN_ENABLE) && (ProfInstIsFree(prof) == 0) && (prof->wanLinkStates == 0U))
    {
	doSystem("ifconfig %s:1 202.160.134.162 netmask 255.255.255.252", getWanIfName(1));
	sleep(1);/*通过睡眠保证路由添加成功，否则会出现路由添加失败情况*/
	doSystem("ip route replace default equalize nexthop via 202.160.134.161 dev %s:1 weight 1", getWanIfName(1));
    }
}

/*
 * 功能：删除为酒店添加的网关和路由，防止出现错误
 * 创建时间：2013-6-7
 */
extern void delRtForETask(void)
{
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;
    MibProfileType mibTypeIf = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;

    prof = (EasyTaskProfile*)ProfGetInstPointByIndex(mibType, 0);
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibTypeIf, 1);
    
    /*左移一位表示WAN1，以此类推*/
    if ((ProfInstIsFree(prof) == 0) && ((prof->wanLinkStates & ((unsigned int)1U << 1U)) == 0U))
    {
	if (ProfInstIsFree(profIf) == 0)
	{
	    doSystem("ifconfig %s:1 0.0.0.0 1>/dev/null 2>&1", getWanIfName(1));/*删掉WAN1虚接口IP地址*/
	}
    }
}
#endif

#if 0
#if (WIRELESS == FYES)
#define MIN_ACTIVE_TIME 1356998400u
/*
 *  wireless_switch_change	-	无线开关改变函数
 *  
 *  参数：
 *	status			-	开关打开或者关闭
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
 *	wireless_date_check	-	判断是否在无线生效时间并做处理
 *
 * 返回值：
 *	status	
 *	0			-	不在生效时段内
 *	1			-	在生效时段内
 *	-1			-	系统时间不对
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
		/*获取星期，时，分*/
		wday = (4 + tmCurrent / 86400) % 7; 
		daytime = tmCurrent % 86400;

		SWORD_PRINTF("%s-%d:wday = %u, daytime = %u\n", __func__, __LINE__, wday, daytime);

		/*星期天处理为星期七*/
		if (wday == 0)
			wday = 7;

		/*判断此天是否在生效时间内*/
		if (((1u << (7u - wday)) & prof->mBaseCf.day)) {
			/*判断是工作日时段还是周末时段*/
			if ((wday >= 1) && (wday <= 5)) {
				sscanf(prof->mBaseCf.weekdayTimeStart, "%u:%u", &hour1, &minute1);/*读入时分*/
				sscanf(prof->mBaseCf.weekdayTimeStop, "%u:%u", &hour2, &minute2);/*读入时分*/
			} else {
				sscanf(prof->mBaseCf.weekendTimeStart, "%u:%u", &hour1, &minute1);/*读入时分*/
				sscanf(prof->mBaseCf.weekendTimeStop, "%u:%u", &hour2, &minute2);/*读入时分*/
			}
			/*判断是否在开始时间到结束时间之间*/
			if ((daytime >= (hour1 * 3600 + minute1 * 60)) && (daytime <= (hour2 * 3600 + minute2 * 60)))
				status = 1;
		}
	}
	SWORD_PRINTF("%s-%d:status = %d\n", __func__, __LINE__, status);
	return status;
}
/*
 *	getWeekdayForCrond	-	获取计划任务所需星期参数
 *
 * 参数：
 *	day			-	星期数据
 * 返回值：
 *	daystr			-	星期组合字符串
 */
static char *getWeekdayForCrond(unsigned int day)
{
	char tmpstr[5];
	static char daystr[14];
	unsigned int i, j = 0u;

	memset(tmpstr, 0, 5);
	memset(daystr, 0, 14);

	/*获取星期几*/
	for (i = 1u; i <= 5u; i++) {
		if ((1u << (7u -i)) & day) {
			sprintf(tmpstr, "%d,", i);
			strcat(daystr, tmpstr);
			j++;
		}
	}
	/*去掉最后一个多余的逗号*/
	if (j != 0)
		daystr[2u * j - 1u] = '\0';
	return daystr;
}
#if 0
/*
 *	editCronRule		-	修改计划任务规则
 *
 * 参数：
 *	action			-	添加还是删除
 *	prof			-	数据存储的profile
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

	/*获取工作日星期*/
	strcpy(weekdaystr, getWeekdayForCrond(prof->day));
	
	/*获取周末星期*/
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
		/*创建工作日计划任务*/
		doSystem("cron-init.sh %s \"%s * * %s snd2speed wireless start\"", action, getTimeForCrond((Uchar*)prof->weekdayTimeStart), weekdaystr);
		doSystem("cron-init.sh %s \"%s * * %s snd2speed wireless stop\"", action, getTimeForCrond((Uchar*)prof->weekdayTimeStop), weekdaystr);
	}

	if (weekendstr[0] != '\0') {
		/*创建周末计划任务*/
		doSystem("cron-init.sh %s \"%s * * %s snd2speed wireless start\"", action, getTimeForCrond((Uchar*)prof->weekendTimeStart), weekendstr);
		doSystem("cron-init.sh %s \"%s * * %s snd2speed wireless stop\"", action, getTimeForCrond((Uchar*)prof->weekendTimeStop), weekendstr);
	}
	return;
}
#endif
/*
 *	WlanTimeEdit		-	无线生效时间修改
 * 
 * 参数：
 *	oldProfileDataPtr	-	原有Profile
 *	newProfileDataPtr	-	新的Profile
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
 * 函数： easyTaskInit
 * 功能： 初始化函数
 * 创建时间：2013-05-13
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
	    editLANRedirectRule('A', lanIp);/*添加DNS跳转规则*/
	}
    }    
#if (HOTEL_PNP == FYES)
    addGwForETask();/*为酒店功能添加网关和路由*/
#endif
}


/********************************************************************
 * 函数： easyTaskProfileChange
 * 功能： profile change 函数
 * 创建时间：2013-05-13
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
	    getIfIp(getLanIfName(), lanIp);/*LAN口IP*/
	    /*开启到关闭*/ 
	    if((oldProfileDataPtr->easyTaskNoticeEn == FUN_ENABLE) && (newProfileDataPtr->easyTaskNoticeEn == FUN_DISABLE))
	    {
		editLANRedirectRule('D', lanIp);
	    }
	    /*关闭到开启*/
	    if((oldProfileDataPtr->easyTaskNoticeEn == FUN_DISABLE) && (newProfileDataPtr->easyTaskNoticeEn == FUN_ENABLE) && (newProfileDataPtr->wanLinkStates == 0U))
	    {
		editLANRedirectRule('D', lanIp);/*添加之前先删除*/
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
 * 函数： funInitEasyTask
 * 功能： 注册函数
 * 创建时间：2013-05-13
 ********************************************************************/ 
extern void funInitEasyTask(void) 
{
    registerForProfileUpdates(MIB_PROF_EASY_TASK, NULL, (ProfileUpdateFnc)easyTaskProfileChange);
    easyTaskInit();
    return;
}
#endif
