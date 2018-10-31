
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>
#include <net/if.h>
#include <linux/sockios.h>
#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "uttfunction.h"
#include "nat.h"
#include "ifmisc.h"
#if (MAKE_MONEY == FYES)
#include "MakeMoney.h"
#endif
#if (WLAN_WORK_TIME == FYES)
#include <time.h>
#define SWORD_DEBUG 0
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif
#define MIN_ACTIVE_TIME 1356998400u
/*
 *  wireless_switch_change	-	���߿��ظı亯��
 *  
 *  ������
 *	status			-	���ش򿪻��߹ر�
 *	prof			-	2.4G����5G����profʵ��
 *  return:
 *     ret                     -  ���߿���״̬�Ƿ�仯
 */
int wireless_switch_change(int status, WirelessProfile *prof)
{
        int ret = 0;
#if 0
	MibProfileType mibType = MIB_PROF_WIRELESS;
	WirelessProfile *prof = NULL;

	prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 0);
#endif

	uttSemP(SEM_WIFI_NO,0);
	SWORD_PRINTF("%s-%d:prof->time_active = %d, status = %d\n", __func__, __LINE__, prof->time_active, status);
	if (prof->time_active != status) 
	{
	    prof->time_active = status;
            ret = 1;
	}
	uttSemV(SEM_WIFI_NO,0);
	return ret;
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
        unsigned int timestart = 0u, timestop = 0u;
	int status = 0;

	prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 0);
	uttSemP(SEM_WIFI_NO,0);
	tmCurrent = time(NULL);

	if (ProfInstIsFree(prof) != 0)
	{
	    status = -1;
	}
#if (UTT_SMART_UI == FYES)
	else if (prof->mBaseCf.WlanTimeEn == FUN_DISABLE)
	{
	    status = 1;
	}
#endif
	else if (tmCurrent < MIN_ACTIVE_TIME)
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
                        timestart = hour1 * 3600 + minute1 * 60;
                        timestop = hour2 * 3600 + minute2 * 60 + 59;
			/*�ж��Ƿ��ڿ�ʼʱ�䵽����ʱ��֮��*/
			if(timestart <= timestop)
			{
			    if ((daytime >= timestart ) && (daytime <= timestop))
			    {
				status = 1;
			    }
			}
			else{/*��һ�쿪ʼ���ڶ������*/
			    if((daytime >= timestart) || (daytime <= timestop))
			    {
				status = 1;
			    }
			}
		}
	}
	SWORD_PRINTF("%s-%d:status = %d\n", __func__, __LINE__, status);
	uttSemV(SEM_WIFI_NO,0);
	return status;
}
#if 0
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
#endif
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
	MibProfileType mibType = MIB_PROF_WIRELESS;
	WirelessProfile *prof = NULL;
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

#if (MULTI_SSID_DHCP_POOL == FYES)
extern void dhcpServerInit(void);
#endif
#ifdef CONFIG_WLAN_LAN_ISOLATION
int set_br_isolation(int enable);
/*���ߺ������û����빦�ܿ���*/
int set_br_isolation(int enable)
{
    struct ifreq temp;
    int skfd, ret;

    memset(&temp, 0, sizeof(struct ifreq));
    strcpy(temp.ifr_name, "br0");

    if((skfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	    return -1;
    }

    temp.ifr_flags = enable;
    ret = ioctl(skfd, SIOCBRISOLATION, &temp);
    if(ret < 0)
    {
	return -1;
    }
    close(skfd);

    return 0;
}
#endif

#if (WIRELESS == FYES)
extern int doSystem(char_t *fmt, ...);
extern int initWan(int wanIndex);
extern int getAPCliSlotIndex(void);
extern void restart8021XDaemon(void);
#if (WIRELESS_5G == FYES)
extern void restart8021XDaemon_5g(void);
#endif
#if (WIRELESS_5G == FYES)
#if (WLAN_WORK_TIME == FYES)
static int isTimeModified(const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr);
static int isTimeModified(const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr)
{
    WirelessProfile *profNew = (WirelessProfile *)newProfileDataPtr;
    WirelessProfile *profOld = (WirelessProfile *)oldProfileDataPtr;
    int timeModified = 0;

    if ((profOld->mBaseCf.WlanTimeEn != profNew->mBaseCf.WlanTimeEn) ||
	(profOld->mBaseCf.day != profNew->mBaseCf.day) ||
	(strcmp(profOld->mBaseCf.weekdayTimeStart,profNew->mBaseCf.weekdayTimeStart) != 0) ||
	(strcmp(profOld->mBaseCf.weekdayTimeStop,profNew->mBaseCf.weekdayTimeStop) != 0) ||
	(strcmp(profOld->mBaseCf.weekendTimeStart,profNew->mBaseCf.weekendTimeStart) != 0) ||
	(strcmp(profOld->mBaseCf.weekendTimeStop,profNew->mBaseCf.weekendTimeStop) != 0)) {
	timeModified = 1;	
    }

    return timeModified;
}
#endif
#endif
#if (FEATURE_AP == FYES)
#define WANNO 1
#endif
#if (UTT_SSID_ISOLATION == FYES)
static void ssid_separate(void)
{
    WirelessProfile *prof[2];
    prof[0] = (WirelessProfile *)ProfGetInstPointByIndex(MIB_PROF_WIRELESS, 0);
#if (WIRELESS_5G == FYES) 
    prof[1] = (WirelessProfile *)ProfGetInstPointByIndex(MIB_PROF_WIRELESS, 1);
#endif
		    
    doSystem("ebtables -F FORWARD");

	    doSystem("iptables -t mangle -D INPUT -p tcp -m uttdev --uttdev-in ra0 -m multiport ! --dport 53,2060 -j DROP");   //��ֹ�����ssid�豸���ʹ���ҳ�� 
	    doSystem("iptables -t mangle -D INPUT -p tcp -m uttdev --uttdev-in ra1 -m multiport ! --dport 53,2060 -j DROP");   //��ֹ�����ssid�豸���ʹ���ҳ�� 
	    doSystem("iptables -t mangle -D INPUT -p tcp -m uttdev --uttdev-in rai0 -m multiport ! --dport 53,2060 -j DROP");   //��ֹ�����ssid�豸���ʹ���ҳ�� 
	    doSystem("iptables -t mangle -D INPUT -p tcp -m uttdev --uttdev-in rai1 -m multiport ! --dport 53,2060 -j DROP");   //��ֹ�����ssid�豸���ʹ���ҳ�� 


	if (prof[0]->mBaseCf.ssidSeparate == 0){
#if (WIRELESS_5G == FYES) 
		if (prof[1]->mBaseCf.ssidSeparate == 1) {/*����rai0*/
			doSystem("ebtables -A FORWARD -i rai0 -o ra0 -j DROP");
			doSystem("ebtables -A FORWARD -i ra0 -o rai0 -j DROP");
			doSystem("ebtables -A FORWARD -i rai0 -o ra1 -j DROP"); 
			doSystem("ebtables -A FORWARD -i ra1 -o rai0 -j DROP"); 
			doSystem("ebtables -A FORWARD -i rai0 -o rai1 -j DROP");
			doSystem("ebtables -A FORWARD -i rai1 -o rai0 -j DROP");
			doSystem("ebtables -A FORWARD -i rai0 -o eth2.1 -j DROP");
			doSystem("ebtables -A FORWARD -i eth2.1 -o rai0 -j DROP");
	    doSystem("iptables -t mangle -A INPUT -p tcp -m uttdev --uttdev-in rai0 -m multiport ! --dport 53,2060 -j DROP");   //�����ssid�豸���ɷ��ʹ���ҳ�� 
		} else if (prof[1]->mBaseCf.ssidSeparate == 2) {/*����rai1*/
			doSystem("ebtables -A FORWARD -i rai1 -o ra0 -j DROP");
			doSystem("ebtables -A FORWARD -i ra0 -o rai1 -j DROP");
			doSystem("ebtables -A FORWARD -i rai1 -o ra1 -j DROP");
			doSystem("ebtables -A FORWARD -i ra1 -o rai1 -j DROP");
			doSystem("ebtables -A FORWARD -i rai1 -o rai0 -j DROP");
			doSystem("ebtables -A FORWARD -i rai0 -o rai1 -j DROP");
			doSystem("ebtables -A FORWARD -i rai1 -o eth2.1 -j DROP");
			doSystem("ebtables -A FORWARD -i eth2.1 -o rai1 -j DROP");
	    doSystem("iptables -t mangle -A INPUT -p tcp -m uttdev --uttdev-in rai1 -m multiport ! --dport 53,2060 -j DROP");   //��ֹ�����ssid�豸���ʹ���ҳ�� 
		}
#endif
	}else if (prof[0]->mBaseCf.ssidSeparate == 1) {/*����ra0*/
		doSystem("ebtables -A FORWARD -i ra0 -o ra1 -j DROP");
		doSystem("ebtables -A FORWARD -i ra1 -o ra0 -j DROP");
		doSystem("ebtables -A FORWARD -i ra0 -o eth2.1 -j DROP");
		doSystem("ebtables -A FORWARD -i eth2.1 -o ra0 -j DROP");
	    doSystem("iptables -t mangle -A INPUT -p tcp -m uttdev --uttdev-in ra0 -m multiport ! --dport 53,2060 -j DROP");   //��ֹ�����ssid�豸���ʹ���ҳ�� 
#if (WIRELESS_5G == FYES) 
		if (prof[1]->mBaseCf.ssidSeparate == 0){
			doSystem("ebtables -A FORWARD -i ra0 -o rai0 -j DROP");
			doSystem("ebtables -A FORWARD -i rai0 -o ra0 -j DROP");
			doSystem("ebtables -A FORWARD -i ra0 -o rai1 -j DROP");
			doSystem("ebtables -A FORWARD -i rai1 -o ra0 -j DROP");
		}else if (prof[1]->mBaseCf.ssidSeparate == 1) {/*����rai0*/
			/*����ra0��rai0ʱ��ra0��rai0���Ի�ͨ,ra0��rai1����ͨ*/
			doSystem("ebtables -A FORWARD -i ra0 -o rai1 -j DROP");
			doSystem("ebtables -A FORWARD -i rai1 -o ra0 -j DROP");
			doSystem("ebtables -A FORWARD -i rai0 -o rai1 -j DROP");
			doSystem("ebtables -A FORWARD -i rai1 -o rai0 -j DROP");
			doSystem("ebtables -A FORWARD -i rai0 -o ra1 -j DROP");
            doSystem("ebtables -A FORWARD -i ra1 -o rai0 -j DROP");
			doSystem("ebtables -A FORWARD -i rai0 -o eth2.1 -j DROP");
			doSystem("ebtables -A FORWARD -i eth2.1 -o rai0 -j DROP");
	    doSystem("iptables -t mangle -A INPUT -p tcp -m uttdev --uttdev-in rai0 -m multiport ! --dport 53,2060 -j DROP");   //��ֹ�����ssid�豸���ʹ���ҳ�� 
		} else if (prof[1]->mBaseCf.ssidSeparate == 2) {/*����rai1*/
			/*����ra0��rai1ʱ��ra0��rai1���Ի�ͨ,ra0��rai0����ͨ*/
			doSystem("ebtables -A FORWARD -i ra0 -o rai0 -j DROP");
			doSystem("ebtables -A FORWARD -i rai0 -o ra0 -j DROP");
			doSystem("ebtables -A FORWARD -i rai1 -o rai0 -j DROP");
			doSystem("ebtables -A FORWARD -i rai0 -o rai1 -j DROP");
			doSystem("ebtables -A FORWARD -i rai1 -o ra1 -j DROP");
			doSystem("ebtables -A FORWARD -i ra1 -o rai1 -j DROP");
			doSystem("ebtables -A FORWARD -i rai1 -o eth2.1 -j DROP");
			doSystem("ebtables -A FORWARD -i eth2.1 -o rai1 -j DROP");
	    doSystem("iptables -t mangle -A INPUT -p tcp -m uttdev --uttdev-in rai1 -m multiport ! --dport 53,2060 -j DROP");   //��ֹ�����ssid�豸���ʹ���ҳ�� 
		}
#endif
    } else if (prof[0]->mBaseCf.ssidSeparate == 2) {/*����ra1*/
		doSystem("ebtables -A FORWARD -i ra1 -o ra0 -j DROP");
		doSystem("ebtables -A FORWARD -i ra0 -o ra1 -j DROP");
		doSystem("ebtables -A FORWARD -i ra1 -o eth2.1 -j DROP");
		doSystem("ebtables -A FORWARD -i eth2.1 -o ra1 -j DROP");
	    doSystem("iptables -t mangle -A INPUT -p tcp -m uttdev --uttdev-in ra1 -m multiport ! --dport 53,2060 -j DROP");   //��ֹ�����ssid�豸���ʹ���ҳ�� 
#if (WIRELESS_5G == FYES)
		if(prof[1]->mBaseCf.ssidSeparate == 0){
			doSystem("ebtables -A FORWARD -i ra1 -o rai0 -j DROP");
			doSystem("ebtables -A FORWARD -i rai0 -o ra1 -j DROP");
			doSystem("ebtables -A FORWARD -i ra1 -o rai1 -j DROP");
			doSystem("ebtables -A FORWARD -i rai1 -o ra1 -j DROP");
		}else if (prof[1]->mBaseCf.ssidSeparate == 1) {/*����rai0*/
			/*����ra1��rai0ʱ��ra1��rai0���Ի�ͨ,ra1��rai1����ͨ*/
			doSystem("ebtables -A FORWARD -i ra1 -o rai1 -j DROP");			
			doSystem("ebtables -A FORWARD -i rai1 -o ra1 -j DROP");
			doSystem("ebtables -A FORWARD -i rai0 -o rai1 -j DROP");
			doSystem("ebtables -A FORWARD -i rai1 -o rai0 -j DROP");
			doSystem("ebtables -A FORWARD -i rai0 -o ra0 -j DROP");
			doSystem("ebtables -A FORWARD -i ra0 -o rai0 -j DROP");         
			doSystem("ebtables -A FORWARD -i rai0 -o eth2.1 -j DROP");
			doSystem("ebtables -A FORWARD -i eth2.1 -o rai0 -j DROP");
	    doSystem("iptables -t mangle -A INPUT -p tcp -m uttdev --uttdev-in rai0 -m multiport ! --dport 53,2060 -j DROP");   //��ֹ�����ssid�豸���ʹ���ҳ�� 
		} else if (prof[1]->mBaseCf.ssidSeparate == 2) {/*����rai1*/
			/*����ra1��rai1ʱ��ra1��rai1���Ի�ͨ,ra1��rai0����ͨ*/
			doSystem("ebtables -A FORWARD -i ra1 -o rai0 -j DROP");
			doSystem("ebtables -A FORWARD -i rai0 -o ra1 -j DROP");
			doSystem("ebtables -A FORWARD -i rai1 -o rai0 -j DROP");
			doSystem("ebtables -A FORWARD -i rai0 -o rai1 -j DROP");
			doSystem("ebtables -A FORWARD -i rai1 -o ra0 -j DROP");         
			doSystem("ebtables -A FORWARD -i ra0 -o rai1 -j DROP");
			doSystem("ebtables -A FORWARD -i rai1 -o eth2.1 -j DROP");
			doSystem("ebtables -A FORWARD -i eth2.1 -o rai1 -j DROP");
	    doSystem("iptables -t mangle -A INPUT -p tcp -m uttdev --uttdev-in rai1 -m multiport ! --dport 53,2060 -j DROP");   //��ֹ�����ssid�豸���ʹ���ҳ�� 
		}
#endif
    }
}
#endif
void WirelessProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr )
{
    WirelessProfile *prof = (WirelessProfile *)newProfileDataPtr;
#if (FEATURE_AP == FYES)
    int i, j=0;
    FatFitSwitchProfile *profS = NULL;
    WirelessProfile *fitWirelessProf = NULL;
    MibProfileType mibTypeS = MIB_PROF_FAT_FIT_SWITCH;
    MibProfileType mibType = MIB_PROF_WIRELESS;
#endif
#if((UTT_NV_1800N != FYES) && (FEATURE_AP != FYES))
    int apCliIndex;
#endif
    switch(changeType) {
	case PROF_CHANGE_ADD:
	    break;
	case PROF_CHANGE_EDIT:
#if (FEATURE_AP == FYES)
#if (WIRELESS_5G == FYES)
//		if (strcmp("wireless5g", prof->head.name) == 0) {
//			doSystem("wlan_5g.sh");
//		} else
		{
#endif
	    /* ��ִ֤����wlan�ű���ִ����������, ������ܻ���Ϊrt2860ģ��û�м��ض�����bug */
	    profS = (FatFitSwitchProfile *)ProfGetInstPointByIndex(mibTypeS, 0);
	    if (profS->fatEnable == 0)	/* fit */
	    {
                for (i=FIT_AP_WL_24G_PF_START; i<(FIT_AP_WL_24G_PF_START + MAX_FIT_AP_SERVICE_ZONE_PROFILES); i++)
		{
		    fitWirelessProf = (WirelessProfile *)ProfGetInstPointByIndex(mibType, i);
		    if (fitWirelessProf->head.active == TRUE)
		    {
			j++;
			break;
		    }
		}
		if (j==0)   /* no sz */
		{
		    doSystem("wlan.sh");
		}
		else	/* has sz */
		{
#if (WIRELESS_5G == FYES)
		    //doSystem("fit_ap_wlan.sh 2");
		    doSystem("fit_ap_wlan.sh 6");
#else
		    doSystem("fit_ap_wlan.sh");
#endif
		}

#if (WIRELESS_5G == FYES)
		if (strcmp("wireless5g", prof->head.name) == 0) 
		{
		    j = 0;
		    for (i = FIT_AP_WL_5G_PF_START; i < (FIT_AP_WL_5G_PF_START + MAX_FIT_AP_SERVICE_ZONE_PROFILES); i++)
		    {
			fitWirelessProf = (WirelessProfile *)ProfGetInstPointByIndex(mibType, i);
			if (fitWirelessProf->head.active == TRUE)
			{
			    j++;
			    break;
			}
		    }
		    if (j==0)   /* no sz */
		    {
			doSystem("wlan_5g.sh");
		    }
		    else	/* has sz */
		    {
			doSystem("fit_ap_wlan_5g.sh 2");
		    }
		}
#endif
	    }
	    else    /* fat */
	    {
		doSystem("wlan.sh");
	    }
#if (WIRELESS_5G == FYES)
		}
#endif
#else
#if (WLAN_WORK_TIME == FYES)
	    WlanTimeEdit(oldProfileDataPtr, newProfileDataPtr);
#endif
#if (WIRELESS_5G == FYES)
		if (strcmp("wireless5g", prof->head.name) == 0) {
			doSystem("wlan_5g.sh");
		} else
#endif
		{
			doSystem("wlan.sh");
#if (WIRELESS_5G == FYES)
#if (WLAN_WORK_TIME == FYES)
			/*�����2.4G�����prof�޸ģ�����Ҫ���ж��ǲ������߶�ʱ����ģ�����ǵĻ���
			 *����Ҫ���ж��Ƿ����5G���磬������ڣ���5G����Ҳ��Ҫ������������Ϊ2.4G��
			 *5G�����߶�ʱ����һ��profile���Ƶġ�
			 **/
			if (isTimeModified(oldProfileDataPtr,newProfileDataPtr)) {
			    doSystem("wlan_5g.sh");
			}
#endif
#endif
		}
#endif
#if((UTT_NV_1800N != FYES) && (FEATURE_AP != FYES))
	    if (prof->mBaseCf.apWorkMode == AP_CLIENT_MODE) {
		apCliIndex = getAPCliSlotIndex();
		initWan(apCliIndex);
	    }
#endif
#if (WIRELESS_5G == FYES)
		if (strcmp("wireless5g", prof->head.name) == 0) {
			restart8021XDaemon_5g();
		} else
#endif
		{
			restart8021XDaemon();
		}
#if (FIT_AP_LOCAL_CONF_TEMP == FYES)
	    doSystem("iwpriv ra0 set Channel=%d", prof->mBaseCf.Channel);
//	    doSystem("iwpriv ra0 set NoForwarding=%d", prof->mBaseCf.NoForwarding);
#ifdef CONFIG_WLAN_LAN_ISOLATION
		if(prof->mBaseCf.NoForwarding == 1)
		{
			set_br_isolation(1);
		}else{
			set_br_isolation(0);
		}
#endif

		FitApSzProfile *profFAS = NULL;
		MibProfileType mibTypeFAS = MIB_PROF_FIT_AP_SZ;
		ApBasicConfProfile *profBase = NULL;
		char str[200];

		for (i=0; i<4; i++)
		{
			profFAS = (FitApSzProfile *)ProfGetInstPointByIndex(mibTypeFAS, i);
			if((profFAS != NULL) && (profFAS->head.active == TRUE))
			{
				sprintf(str, "iwpriv ra%d set VLANID=%u; iwpriv ra%d set NoForwarding=%u", i, profFAS->vid, i, profFAS->ssidIsolate);
                system(str);
                if(profFAS->ssidIsolate){
                    sprintf(str, "iwpriv ra%d set NoForwardingBTNBSSID=%u", i, profFAS->ssidIsolate);
                    system(str);
                }
			}
		    
		    if ((profFAS != NULL) && (profFAS->head.active == TRUE) && (profFAS->head.name[0] != '\0') && (profFAS->client >= 0))
		    {
			memset(str, 0, sizeof(str));
			sprintf(str, "iwpriv ra%d set MaxStaNum=%u", i, profFAS->client );
			system(str);
		    }
#if (SZ_SHARE_LIMIT_AP == FYES)
		    /*��ʼ�����������ٲ���*/
		    if ((profFAS != NULL) && (profFAS->head.active == TRUE) && (profFAS->limit_type != 0))
		    {
			memset(str, 0, sizeof(str));
			sprintf(str, "iwpriv ra%d set StaRate=%u:%u:%u", i, profFAS->limit_type , profFAS->limit_down, profFAS->limit_up);	
			system(str);
		    }
#endif
		}

		profBase = (ApBasicConfProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_BASIC_CONF, 0);
		if (profBase != NULL)
		{
		    if(profBase->dbm != 0) {
			sprintf(str, "iwpriv ra0 set KickStaRssiLow=-%u;iwpriv ra1 set KickStaRssiLow=-%u;iwpriv ra2 set KickStaRssiLow=-%u;iwpriv ra3 set KickStaRssiLow=-%u", profBase->dbm, profBase->dbm, profBase->dbm, profBase->dbm);
			system(str);
			sprintf(str, "iwpriv ra0 set AssocReqRssiThres=-%u;iwpriv ra1 set AssocReqRssiThres=-%u;iwpriv ra2 set AssocReqRssiThres=-%u;iwpriv ra3 set AssocReqRssiThres=-%u", profBase->dbm, profBase->dbm, profBase->dbm, profBase->dbm);
			system(str);
		    }
#if (WIRELESS_5G == FYES)
		    if(profBase->dbm_5g != 0) {
			sprintf(str, "iwpriv rai0 set KickStaRssiLow=-%u;iwpriv rai1 set KickStaRssiLow=-%u;iwpriv rai2 set KickStaRssiLow=-%u;iwpriv rai3 set KickStaRssiLow=-%u", profBase->dbm_5g, profBase->dbm_5g, profBase->dbm_5g, profBase->dbm_5g);
			system(str);
			sprintf(str, "iwpriv rai0 set AssocReqRssiThres=-%u;iwpriv rai1 set AssocReqRssiThres=-%u;iwpriv rai2 set AssocReqRssiThres=-%u;iwpriv rai3 set AssocReqRssiThres=-%u", profBase->dbm_5g, profBase->dbm_5g, profBase->dbm_5g, profBase->dbm_5g);
			system(str);
		    }
#endif
		}
#endif

#if (WIRELESS_TURBO == FYES)
#if (FEATURE_AP != FYES)
		system("killall -q -USR2 turbo&");
#endif
#endif
#if (FEATURE_AP == FYES)
        addOneWanDefaultRoute(WANNO);
        ModifyDefaultRoute(WANNO, 1);
#endif
#if (UTT_SSID_ISOLATION == FYES)
		ssid_separate();
#endif
#if (MAKE_MONEY == FYES)
        moneywifi_separate();
#endif
#if (MULTI_SSID_DHCP_POOL == FYES)
		dhcpServerInit();
#endif
	    break;
	case PROF_CHANGE_DEL:
	    break;
	case PROF_CHANGE_DELALL:
	    break;
	default:
	    break;
    };
    return;
}

//#if ((UTT_NV_1800N == FYES) || (UTT_NV_510V2 == FYES))
#if (FEATURE_AP == FYES)
void funInitFitApWireless(void)
{
		int i;
		char str[200];
		FitApSzProfile *prof = NULL;
		MibProfileType mibType = MIB_PROF_FIT_AP_SZ;
		int sz_num = 0, min, max;
		WirelessProfile *profWL = NULL;
		MibProfileType mibTypeWL = MIB_PROF_WIRELESS;
		ApBasicConfProfile *profBase = NULL;
#if (WIRELESS_5G == FYES)
		FitAp5gSzProfile *prof5G = NULL;
		MibProfileType mibType5G = MIB_PROF_FIT_AP_SZ_5G;
		int sz_num_5g = 0;
#endif
		ProfInstLowHigh(mibTypeWL, &max, &min);
		for (i = min + FIT_AP_WL_24G_PF_START; i < max; i++) {
		    profWL = (WirelessProfile *)ProfGetInstPointByIndex(mibTypeWL, i);
		    if ((NULL != profWL) && (TRUE == profWL->head.active)) {
			sz_num++;
		    }
		}

#if (WIRELESS_5G == FYES)
		for (i = min + FIT_AP_WL_5G_PF_START; i < FIT_AP_WL_5G_PF_START + 4; i++) {
		    profWL = (WirelessProfile *)ProfGetInstPointByIndex(mibTypeWL, i);
		    if ((NULL != profWL) && (TRUE == profWL->head.active)) {
			sz_num_5g++;
		    }
		}

#endif
		if (sz_num > 0) {
#if (WIRELESS_5G == FYES)
			system("fit_ap_wlan.sh 6");
#else
			system("fit_ap_wlan.sh");
#endif
			system("sleep 3");
//			FitApRestart8021XDaemon();
			system("killall -9 rt2860apd 1>/dev/null 2>&1");
			system("sleep 1");
			system("rt2860apd");

			for (i=0; i < sz_num; i++)
			{
					prof = (FitApSzProfile *)ProfGetInstPointByIndex(mibType, i);
					sprintf(str, "iwpriv ra%d set VLANID=%u; iwpriv ra%d set NoForwarding=%u;", i, prof->vid, i, prof->ssidIsolate);
					system(str);
                    if(prof->ssidIsolate){
                        sprintf(str, "iwpriv ra%d set NoForwardingBTNBSSID=%u", i, prof->ssidIsolate);
                        system(str);
                    }
		    /*��ʼ����������ͻ�����*/
		    if ((prof != NULL) && (prof->head.active == TRUE) && (prof->head.name[0] != '\0') && (prof->client >= 0))
		    {
			memset(str, 0, sizeof(str));
			sprintf(str, "iwpriv ra%d set MaxStaNum=%u", i, prof->client );
			system(str);
		    }
#if (SZ_SHARE_LIMIT_AP == FYES)
		    /*��ʼ�����ٲ���*/
		    if ((prof != NULL) && (prof->head.active == TRUE) && (prof->limit_type != 0))
		    {
			memset(str, 0, sizeof(str));
			sprintf(str, "iwpriv ra%d set StaRate=%u:%u:%u", i, prof->limit_type , prof->limit_down, prof->limit_up);	
			system(str);
		    }
#endif

			}
		}

#if (WIRELESS_5G == FYES)
    if (sz_num_5g > 0)
    { 
	system("fit_ap_wlan_5g.sh 2");
	system("sleep 3");
	for (i=0; i < sz_num_5g; i++)
	{
	    prof5G = (FitAp5gSzProfile *)ProfGetInstPointByIndex(mibType5G, i);
	    sprintf(str, "iwpriv rai%d set VLANID=%u; iwpriv rai%d set NoForwarding=%u;", i, prof5G->vid, i, prof5G->ssidIsolate);
	    system(str);
	    if(prof5G->ssidIsolate)
	    {
		sprintf(str, "iwpriv rai%d set NoForwardingBTNBSSID=%u", i, prof5G->ssidIsolate);
		system(str);
	    }
#if (SZ_SHARE_LIMIT_AP == FYES)
	    /*��ʼ�����ٲ���*/
	    if ((prof5G != NULL) && (prof5G->head.active == TRUE) && (prof5G->limit_type != 0))
	    {
		memset(str, 0, sizeof(str));
		sprintf(str, "iwpriv rai%d set StaRate=%u:%u:%u", i, prof5G->limit_type , prof5G->limit_down, prof5G->limit_up);	
		system(str);
	    }
#endif
	}
    }
#endif
		profBase = (ApBasicConfProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_BASIC_CONF, 0);
		if (profBase != NULL)
		{
                    if(profBase->dbm != 0)
		    {
			sprintf(str, "iwpriv ra0 set KickStaRssiLow=-%u;iwpriv ra1 set KickStaRssiLow=-%u;iwpriv ra2 set KickStaRssiLow=-%u;iwpriv ra3 set KickStaRssiLow=-%u", profBase->dbm, profBase->dbm, profBase->dbm, profBase->dbm);
			system(str);
			sprintf(str, "iwpriv ra0 set AssocReqRssiThres=-%u;iwpriv ra1 set AssocReqRssiThres=-%u;iwpriv ra2 set AssocReqRssiThres=-%u;iwpriv ra3 set AssocReqRssiThres=-%u", profBase->dbm, profBase->dbm, profBase->dbm, profBase->dbm);
			system(str);
		    }
#if (WIRELESS_5G == FYES)
		    if(profBase->dbm_5g != 0) {
			sprintf(str, "iwpriv rai0 set KickStaRssiLow=-%u;iwpriv rai1 set KickStaRssiLow=-%u;iwpriv rai2 set KickStaRssiLow=-%u;iwpriv rai3 set KickStaRssiLow=-%u", profBase->dbm_5g, profBase->dbm_5g, profBase->dbm_5g, profBase->dbm_5g);
			system(str);
			sprintf(str, "iwpriv rai0 set AssocReqRssiThres=-%u;iwpriv rai1 set AssocReqRssiThres=-%u;iwpriv rai2 set AssocReqRssiThres=-%u;iwpriv rai3 set AssocReqRssiThres=-%u", profBase->dbm_5g, profBase->dbm_5g, profBase->dbm_5g, profBase->dbm_5g);
			system(str);
		    }
#endif
		}


		return;
}
#endif

void funInitWireless(void)
{
	registerForProfileUpdates(MIB_PROF_WIRELESS, (Pointer)NULL, WirelessProfileChange);
#if ((WIRELESS_5G == FYES) || defined (CONFIG_WLAN_LAN_ISOLATION))
	MibProfileType mibType = MIB_PROF_WIRELESS;
	WirelessProfile *prof = NULL;
#endif
#if (WIRELESS_5G == FYES)
	prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 1);
	doSystem("iwpriv rai0 set Channel=%d", prof->mBaseCf.Channel);
	if ((prof->head.active == FALSE)
#if (WLAN_WORK_TIME == FYES)
		|| (prof->time_active != 1)
#endif
		) 
	{
	    doSystem("insmod /lib/modules/2.6.36/kernel/drivers/net/wireless/MT7610_ap/MT7610_ap.ko;\
		    ifconfig rai0 up;ifconfig rai0 down;rmmod MT7610_ap");
	}
#endif

#if (UTT_SSID_ISOLATION == FYES)
	ssid_separate();
#endif
#ifdef CONFIG_WLAN_LAN_ISOLATION
	MibProfileType mibType = MIB_PROF_WIRELESS;
	WirelessProfile *prof = NULL;

	prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 0);
	if(prof->mBaseCf.NoForwarding == 1)
	{
		set_br_isolation(1);
	}else{
		set_br_isolation(0);
	}

#endif
#if (MAKE_MONEY == FYES)
        moneywifi_separate();
#endif

/* ebtables Netfilter��ʱ����ֹebtables����iptables���˹��� --by cao.yongxiang */
#ifdef CONFIG_BRIDGE_NETFILTER
	doSystem("echo 0 > /proc/sys/net/bridge/bridge-nf-call-iptables");
#endif

}
#endif

