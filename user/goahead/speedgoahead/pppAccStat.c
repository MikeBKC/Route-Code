#include <stdlib.h>
#include <sys/sysinfo.h>
#include <stdio.h>
#include <string.h>
#include "uttMachine.h"
#include "typedef.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ifmisc.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "conver.h"
#include "uttfunction.h"
#include "comExeFun.h"
#include <pthread.h>
#include <net/if_arp.h>
#include <syslog.h>
#ifndef CONFIG_RALINK_RT2880
#include <linux/netfilter_ipv4/utt_netfilter.h>
#include <unistd.h>
#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif
#if (WEB_AUTH_SHARE == FYES)
#define DEBUG 0
#endif

//#if (PPPOE_ACCOUNT == FYES) /* not only for pppoe accoutn */
#define POLL_INTERVAL 1u

#if (PPPOE_ACCOUNT == FYES)
#define PPPOE_POLL_INTERVAL 20u

/*ÿ����ѯ�ڣ���ദ���pppoe�Ʒ��˺���,̫�������CPU˲�为��*/
#define MAX_PPPACC_ONE_POLL   ((MAX_PPPOE_SRV_ACC_PROFILES+15) >> 4u)  /*����16(С�����ַ�0���1)*/

/*ÿ����ѯ�ڣ����ٴ����PPPOE�Ʒ��˺ţ�̫������һ�������˺ţ���Ҫ��ʱ������*/
#define MIN_PPPACC_ONE_POLL   ((MAX_PPPOE_SRV_ACC_PROFILES+63) >> 6u)  /*����64(С�����ַ�0���1)*/

/* ÿ����ѯ�ڣ���ദ���PPPOE�Ự��
 * ע�����˺��лỰ���ͻ����ıȽ϶��CPU��Դ*/
#define MAX_PPPSESS_ONE_POLL  MIN_PPPACC_ONE_POLL
#endif

#if (WEBAUTH_ACCOUNT == FYES)
static int webAuthAccTimes = 0;/*ͳ�ƴ��� 80���ʮ��*/
static int webAuthAccNum = 0;
extern void setWebAuthAccNum(int num)
{
    webAuthAccNum = num;
}
extern int getWebAuthAccNum()
{
    return webAuthAccNum;
}
#define WEB_AUTH_POLL_INTERVAL 8u
struct arpreq arpLan = {
    .arp_pa.sa_family = AF_INET,
    .arp_ha.sa_family = AF_INET
};
#endif

#if (WLAN_WORK_TIME == FYES)
extern int  wireless_switch_change(int status,WirelessProfile *prof);
#endif
#if (FEATURE_AP_MAIL == FYES)
extern void ap_send_mail(void);
extern Uint32 get_ap_mail_time(void);
extern int get_ap_mail_enable(void);
#endif
#if (WIFIDOG_AUTH == FYES)
extern void check_wifidog_by_name( char* pidName);
#endif
#if (CWMP == FYES)
extern void check_cwmpd_by_name( char* pidName);
#endif
static pthread_t poll_task_thread;
static int pollTaskStatus;
extern int spdNfNlSock;

static void pollTask(void);

#if (UTT_SMART_UI == FYES)
#if (APP_POLICY_REMOTE_UPDATE == FYES)
#if (ADFILTER == FYES)
extern unsigned int oldCheckCycle;
extern unsigned int policyCounter;
extern unsigned int timeCheckUpdatePoint;
extern int policyUpdateCheck(AdfilterProfile *adfilterProfile,unsigned char flag);
static inline void policyUpdateCheckTask(AdfilterProfile *adfilterProfile)
{
    static unsigned char flag;

    if ((flag == 0U) && (oldCheckCycle != 0) && (policyCounter % oldCheckCycle == timeCheckUpdatePoint)) {
	uttSemP(SEM_ADFILTER,0);
	if ((adfilterProfile->head.active == TRUE)/*���ܿ���*/
		&& (adfilterProfile->updateType == 0)/*�Ҹ��·�ʽΪ�Զ�*/
		&& (adfilterProfile->oldVersion == adfilterProfile->newVersion)) {/*�������˷����°汾�Կ�֧���Զ�����*/
	    if (policyUpdateCheck(adfilterProfile,flag) == 0) {/*ִ�м�����*/
		flag = 1U;
	    }
	}
	uttSemV(SEM_ADFILTER,0);
    }
    if (flag) {/*ִ���������*/
	uttSemP(SEM_ADFILTER,0);
	if ((adfilterProfile->head.active == TRUE)/*���ܿ���*/
		&& (adfilterProfile->updateType == 0)/*�Ҹ��·�ʽΪ�Զ�*/
		&& (adfilterProfile->oldVersion == adfilterProfile->newVersion)) {/*�������˷����°汾�Կ�֧���Զ�����*/
	    policyUpdateCheck(adfilterProfile,flag);/*ִ�и��³���*/
	    if (flag++ % 10 == 0) {
		flag = 0U;
	    }
	} else {
	    flag = 0U;
	}
	uttSemV(SEM_ADFILTER,0);
    }
    /*ÿ�����һ�Σ������¼�����´μ���ʱ���*/
    if ((policyCounter == oldCheckCycle) && (oldCheckCycle != 0)) {
	uttSemP(SEM_ADFILTER,0);
	if ((adfilterProfile->head.active == TRUE)/*���ܿ���*/
		&& (adfilterProfile->updateType == 0)/*�Ҹ��·�ʽΪ�Զ�*/
		&& (adfilterProfile->oldVersion == adfilterProfile->newVersion)) {/*�������˷����°汾�Կ�֧���Զ�����*/
	    srand(time(NULL));
	    timeCheckUpdatePoint = rand() % (SECONDS_OF_DAY * adfilterProfile->checkCycle);
	    policyCounter = 0;
	    oldCheckCycle = (SECONDS_OF_DAY * adfilterProfile->checkCycle);
	}
	uttSemV(SEM_ADFILTER,0);
    }
    policyCounter++;
}
#endif
#endif
#endif

#if (PPPOE_ACCOUNT == FYES)
static void pppoeAccPoll(void);
#if 0
/**
 * ͳ�Ƶ���pppoe�û���״̬����������ع���
 */
void upPppoeAcc(PPPoESrvAccProfile*prof)
{
    emPppoeAccStatus oldSta = prof->expired;/*�����ϵ�״̬*/
    upPppAccStatus(prof);/*��������״̬*/
    if(oldSta != prof->expired)
    {
	upPppAccRule(prof, oldSta, 0);/*ɾ��ԭ�й���*/
	upPppAccRule(prof, prof->expired, 1);/*����¹���*/
    }

    return;
}
#else
#endif
/**
 * ͳ������pppoe�û���״̬�������¹���
 */
void pppoeAccCheck(void)
{
    static int nextEnd = 0;/*��һ����ѯ���账�����ʼ�˺�����*/
    int sessCheckNum = 0;
    int max = nextEnd + MAX_PPPACC_ONE_POLL;/*��������ദ����˺Ž�������(��1)*/
    int min = nextEnd + MIN_PPPACC_ONE_POLL - 1;/*���������ٴ�����˺Ž�������*/
    int i = 0;

    MibProfileType mibType = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *info = NULL;

    if (max > MAX_PPPOE_SRV_ACC_PROFILES)
    {/*���ܳ���ʵ�ʵ��˺�����*/
	max = MAX_PPPOE_SRV_ACC_PROFILES;
    }
    uttGlTime = time(NULL);
    /*
     *�˴�Ϊ�����ź��������˲���
     */
    uttSemP(SEM_PPPOE_NO,0);
    for(i = nextEnd;i < max;i++)
    {
	info = (PPPoESrvAccProfile*)ProfGetInstPointByIndex(mibType, i);
	if(ProfInstIsFree(info) == 0)
	{
#if 0
	    if( (info->ConTime == 0u) && (info->chargeType !=  NONE_CHARG_MODE))/*���߲��ҿ����˼Ʒ�*/
	    {		   
		upPppoeAcc(info);
	    }
#else
	    if((info->actualSession > 0) && (info->chargeType !=  NONE_CHARG_MODE))
	    {/*���˺��лỰ����������ʹ�ø��˺Ų��š�����ͳ�ƼƷѵı�Ҫ*/
		pppoeSesCheck(info);
		sessCheckNum += info->actualSession;
		if((sessCheckNum >= MAX_PPPSESS_ONE_POLL) 
			&& (i>=min))
		{/*����ĻỰ�����˺������ﵽҪ��*/
                    i++;/*ָ����һ������*/
		    break;
		}

	    }
#endif
	}
    }
   

    /*
     *�˴�Ϊ���źŽ���
     */
    uttSemV(SEM_PPPOE_NO,0);

    uttGlTime = 0;
    nextEnd = i;
    if (nextEnd >= MAX_PPPOE_SRV_ACC_PROFILES)
    {/*�������ˣ���ͷ��ʼ*/
	nextEnd = 0;
    }

    return;
}

/**
 * pppoe �˺żƷ�ͳ�ƺ���
 * **/
static void pppoeAccPoll(void)
{
    static Uint32 pollCount = 0u;

    /*����������Ȳ�������ɿ���ʹ�ü�ʱ��*/
    if(pollCount < PPPOE_POLL_INTERVAL)
    {
	pollCount++;/*��¼��ѭ�� ѭ�����ٴΡ�ִ��pppoeͳ��*/
    }
    else
    {/*ִ��һ��ͳ��*/
	pppoeAccCheck();
	pollCount = 0u;
    }
    return;

}
#endif

#if (WEBAUTH_ACCOUNT == FYES)
/**************************************************************
 * �������ܣ�����ip�Ҷϵ�ʱ���û���Ƿ��û�
 * ����ʱ�䣺2013-02-18
 *************************************************************/
static void waCancelByProf(WebAuthProfile *prof)
{
#if (IP_GRP == FYES)
    struct in_addr addr;
#endif
#if (WEB_AUTH_SHARE == FYES)
    int i = 0, j = -1;
#if DEBUG
    printf("%s-%d: prof->user =%s,prof->maxSession =%d,prof->actualSession =%d\n",__func__,__LINE__,
	    prof->user,prof->maxSession,prof->actualSession);
#endif
    printf("%s %d:....\n", __func__, __LINE__);
    for(i=0;i<prof->actualSession;i++)
    {
	printf("%s %d:....\n", __func__, __LINE__);
	j = prof->sesIndex;
	if(checkSessionIndex(j) != 1)
	{
	    printf("%s %d:....\n", __func__, __LINE__);
	    break;
	}
#if DEBUG
	printf("%s-%d: session ip =0x%x\n",__func__,__LINE__,nvramProfile->session[j].ip);
#endif
	/*����IP��OK�������*/ 
	webAuthLog(prof, nvramProfile->session[j].ip, nvramProfile->session[j].mac);
	ipsetAOrDIp(SYS_WEB_AUTH_OK,nvramProfile->session[j].ip,IPSET_MEM_DEL);
#if (IP_GRP == FYES)
	addr.s_addr = nvramProfile->session[j].ip;
	userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
	uttNfNlHashDelIp(spdNfNlSock, nvramProfile->session[j].ip);/*ɾ��hash���Ӧ��ip*/
#endif
	prof->sesIndex = nvramProfile->session[j].nextIndex;
	memset(&(nvramProfile->session[j]), 0U, sizeof(WebAuthSession));
	nvramProfile->session[j].nextIndex = -1;
    }
    prof->sesIndex = -1;
    prof->actualSession = 0U;
#if DEBUG
    printf("%s-%d: prof->user =%s,prof->maxSession =%d, prof->actualSession =%d, prof->sesIndex =%d\n",__func__,__LINE__,
	    prof->user,prof->maxSession,prof->actualSession,prof->sesIndex);
#endif
#else /*WEB_AUTH_SHARE*/

    /*����IP��OK�������*/ 
    webAuthLog(prof, prof->ip, prof->mac);
    ipsetAOrDIp(SYS_WEB_AUTH_OK,prof->ip,IPSET_MEM_DEL);
#if (IP_GRP == FYES)
    addr.s_addr = prof->ip;
    userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
    uttNfNlHashDelIp(spdNfNlSock, prof->ip);/*ɾ��hash���Ӧ��ip*/
#endif
    prof->ip = 0U;/*���ip*/
    memset(prof->mac, 0, sizeof(prof->mac));/*���mac*/
#endif /*WEB_AUTH_SHARE*/
}

#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
/********************************************************************
 * ������ searchDelSessionByIp
 * ���ܣ� �����û�IP��ַ�����˺ŻỰ��Ϣ�����ҵ���ע���Ự
 * ������ 2013-09-24
 * ������ ip - 	�û�IP��ַ
 * ���أ� 
 *	  1  -  �鵽�Ự��ע��
 *	  0  -  δ���ҵ�
 * ����� ��
 * ���ߣ� guo.deyuan
 ********************************************************************/ 
#if (WEB_AUTH_SHARE == FYES)
static int searchDelSessionByIp(WebAuthProfile *prof,Uint32 ip)
{
    int flag = 0;
    int prev =-1;
    int i=0, j=-1;
#if (IP_GRP == FYES)
	struct in_addr addr;
#endif
#if DEBUG
	    printf("%s-%d: prof->user =%s,prof->maxSession =%d, prof->actualSession =%d, prof->sesIndex =%d\n",__func__,__LINE__,
		    prof->user,prof->maxSession,prof->actualSession,prof->sesIndex);
#endif
    if(checkSessionIndex(prof->sesIndex) == 1)
    {
	j = prof->sesIndex;
	if(nvramProfile->session[j].ip == ip) {
	    webAuthLog(prof, ip, nvramProfile->session[j].mac);
	    prof->sesIndex = nvramProfile->session[j].nextIndex;
	    memset(&(nvramProfile->session[j]), 0U, sizeof(WebAuthSession));
	    nvramProfile->session[j].nextIndex = -1;
	    flag = 1;
	} else {
	    for(i=0;i<prof->actualSession;i++)
	    {
		prev = j;
		j = nvramProfile->session[j].nextIndex;
		/*����IP���Ҷ�Ӧ�˺�*/
		if(checkSessionIndex(j) != 1)
		{
		    break;
		}
		if(nvramProfile->session[j].ip == ip) {
		    webAuthLog(prof, ip, nvramProfile->session[j].mac);
		    nvramProfile->session[prev].nextIndex = nvramProfile->session[j].nextIndex;
		    memset(&(nvramProfile->session[j]), 0U, sizeof(WebAuthSession));
		    nvramProfile->session[j].nextIndex = -1;
		    flag = 1;
		    break;
		}
	    }
	}
	if(flag == 1)
	{
	/*����IP��OK�������*/
	ipsetAOrDIp(SYS_WEB_AUTH_OK,ip,IPSET_MEM_DEL); 
#if (IP_GRP == FYES)
	addr.s_addr = ip;
#if DEBUG
	printf("%s-%d: prof->user =%s find and del inet_ntoa(addr) =%s\n",__func__,__LINE__,prof->user,inet_ntoa(addr));
#endif
	userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
	uttNfNlHashDelIp(spdNfNlSock, ip);/*ɾ��hash���Ӧ��ip*/
#endif
	}
    }
    return flag;
}
#endif
void uttNfWebAuthCancel_byIp(unsigned long ip)
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH;
    WebAuthProfile *prof = NULL;
    int min = 0, max = 0;
    int i = 0;

    SWORD_PRINTF("%s %d : del prof ip 0x%x\n",__func__, __LINE__,ip) ;
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) 
    {
	
	prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
	{
#if (WEB_AUTH_SHARE == FYES)
#if DEBUG
	    printf("%s-%d: check session ip =0x%x\n",__func__,__LINE__,(Uint32)ip);
#endif
	    if((prof->actualSession > 0) && (searchDelSessionByIp(prof, ip) ==1))
	    {
#if DEBUG
		printf("%s-%d: find and del session,prof->actualSession =%d\n",__func__,__LINE__,prof->actualSession);
#endif
		prof->actualSession -= 1;
		break;
	    }
#else /*WEB_AUTH_SHARE*/
	    /*����IP���Ҷ�Ӧ�˺�*/
	    if(prof->ip == ip)
	    {
		waCancelByProf(prof);
		SWORD_PRINTF("%s %d : Cancel By prof\n",__func__, __LINE__) ;
	    }
#endif /*WEB_AUTH_SHARE*/
	}
    }
}
#endif

/**************************************************************
 * �������ܣ�web��֤���ڼ��
 * ����ʱ�䣺2013-02-28
 * ����������timeCurrent--��ǰʱ��
 * ����ֵ��0--�ϸ� 1--����
 ************************************************************/
static int webAuthDateCheck(WebAuthProfile *prof, int tmCurrent)
{
    int ret = 1;
    /*��ǰʱ�����û��������ڷ�Χ�ڣ������ڳ���ʱ��2Сʱ�ڣ�����������*/
    if (((prof->timeStart <= tmCurrent) && (tmCurrent <= prof->timeStop)) || ((DELIVERY_START_TIME <= tmCurrent) && (tmCurrent <= DELIVERY_END_TIME)))
    {
	ret = 0;/*����*/
    }
#if (DEBUG_WEBAUTH_ACC == 1)
    else
    {
	 webauth_acc_debug_print("%s--%d:This account has expired, will be hang up\n", __func__, __LINE__);	
    }
#endif

    return ret;
} 


/*************************************************************
 * �������ܣ��޸��û���ʹ��ʱ�䣬�ж�ʣ��ʱ��
 * ����ʱ�䣺2013-02-28
 * ����ֵ��0--�ϸ� 1--���ϸ�
 ************************************************************/
static int webAuthTimeCheck(SystemProfile *profSys, WebAuthProfile *prof, struct sysinfo *psysInfo)
{
    int ret = 0;
    if (prof->timeTotal != 0U)
    {
	prof->timeUsed += (unsigned long)psysInfo->uptime - prof->timeLast;/*��ǰʱ���ȥ��һ�α���ʱ�䣬�ó�ʹ��ʱ��*/
	if (prof->timeUsed >= prof->timeTotal)/*��ʹ��ʱ�䳬����ʱ��*/
	{
	    ret = 1;
#if (DEBUG_WEBAUTH_ACC == 1)
	    webauth_acc_debug_print("%s--%d:This account don't have time remaining, will be hang up\n", __func__, __LINE__);	
#endif
	}
	prof->timeLast = (unsigned long)psysInfo->uptime;
	profSys->flashWt |= WEB_AUTH_FLASH_W;/*��web��֤�Ʒ�дflash��־*/
    }
    return ret;
}

#if (WEBAUTH_AUTO == FYES)
/*************************************************************
 *  * �������ܣ����mac��ַ�Ƿ�����֤ʱ��ͬ
 *   * ����ʱ�䣺2013-09-25
 *    * ����ֵ��0--����֤ʱmac��ַ��ͬ 1--����֤ʱmac��ַ��ͬ 
 *     *********************************************************/
static int isMacZero(char *mac)
{
    if ((mac[0] == 0)
	    && (mac[1] == 0)
	    && (mac[2] == 0)
	    && (mac[3] == 0)
	    && (mac[4] == 0)
	    && (mac[5] == 0))
	return 1;
    else
	return 0;
}
#endif

#if (WEB_AUTH_SHARE == FYES)
/*************************************************************
 * �������ܣ����mac��ַ�Ƿ�����֤ʱ��ͬ
 * ����ʱ�䣺2013-09-25
 * ����ֵ��0--����֤ʱmac��ַ��ͬ 1--����֤ʱmac��ַ��ͬ 
 *********************************************************/
static int checkMacSame(int index)
{
    char *mac = NULL;
    int ret = 0;/*��������ֵ*/

    if(checkSessionIndex(index) == 1)
    {
    if (nvramProfile->session[index].compareMacEn == FUN_ENABLE)
    {
	((struct sockaddr_in*)&arpLan.arp_pa)->sin_addr.s_addr = nvramProfile->session[index].ip;
	memset(arpLan.arp_ha.sa_data, 0, sizeof(arpLan.arp_ha.sa_data));
	mac = getMacByIpIoctl(&arpLan);/*ͨ��arp���ѯ��ǰip�Ķ�Ӧmac��ַ*/
	if (mac == NULL)/*�鲻����ǰmac��ַ���û��Ѿ����߰����쳣����*/
	{
	    memset(nvramProfile->session[index].mac, 0, sizeof(nvramProfile->session[index].mac));/*goahead��ֻ���ip���������mac*/
	    ret = 1;
#if DEBUG
	    printf("%s-%d: mac is null.\n",__func__,__LINE__);
#endif
#if (DEBUG_WEBAUTH_ACC == 1)
	    webauth_acc_debug_print("%s--%d:The user has quit\n", __func__, __LINE__);	
#endif
	}
	else
	{
	    ret = ((*(unsigned short*)mac) ^ (*((unsigned short*)(nvramProfile->session[index].mac)))) || ((*((UINT32*)(mac + 2))) ^ (*((UINT32*)(nvramProfile->session[index].mac + 2))));/*�Ƚϵ�ǰmac����֤ʱmac��ַ*/
#if DEBUG
	    printf("%s-%d: mac =%s, session mac =%s\n",__func__,__LINE__,mac,nvramProfile->session[index].mac);
#endif
#if DEBUG
	    if(ret == 1)
		printf("%s-%d: mac is not same with the auth mac.\n",__func__,__LINE__);
#endif
#if (DEBUG_WEBAUTH_ACC == 1)
	    if (ret == 1)
	    {
		webauth_acc_debug_print("%s--%d:Others use this account now, will be hang up\n", __func__, __LINE__);	
	    }
#endif
	}
    }
    else{
#if (WEBAUTH_AUTO == FYES)
	/* ����ϴλ�ȡ��mac���򱾴β����ٻ�ȡ */
	if (isMacZero(nvramProfile->session[index].mac))
	{
	    ((struct sockaddr_in*)&arpLan.arp_pa)->sin_addr.s_addr = nvramProfile->session[index].ip;
	    memset(arpLan.arp_ha.sa_data, 0, sizeof(arpLan.arp_ha.sa_data));
	    mac = getMacByIpIoctl(&arpLan);/*ͨ��arp���ѯ��ǰip�Ķ�Ӧmac��ַ*/
	    if (mac)
	    {
		printf("%s %d: get index %d ip %#X mac %02X%02X-%02X%02X-%02X%02X\n", 
			__func__, __LINE__, index,
			nvramProfile->session[index].ip, 
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		memcpy(nvramProfile->session[index].mac, mac, 6);
		//nvramProfile->session[index].compareMacEn = FUN_ENABLE;
	    } else 
		printf("%s %d:.....cannot get mac by ip %#X\n", __func__, __LINE__, nvramProfile->session[index].ip);
	}
#endif
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
	ret = 0;
	SWORD_PRINTF("%s-%d:---flow check---\n",__func__,__LINE__);
#endif
    }
    }
    return ret;
}
/*************************************************************
 * �������ܣ����mac��ַ���ж��û����߻�����,�ҶϷǷ��û�
 * ����ʱ�䣺2013-02-28
 * ����ֵ��
 *********************************************************/
static int webAuthMacCheckAndDel(SystemProfile *profSys, WebAuthProfile *prof)
{    
    int ret = 1;/*��������ֵ*/
    int prev = -1;
    int i = 0, j = -1;
    int max = 0;
#if (IP_GRP == FYES)
    struct in_addr addr;
#endif
    /*compareMacEn�򿪱�ʾ��֤ʱ�鵽��mac��ַ����Ҫʹ��mac��ַ�ж�ʱ��*/
#if DEBUG
    printf("%s-%d: prof->user =%s,prof->maxSession =%d, prof->actualSession =%d, prof->sesIndex =%d\n",__func__,__LINE__,
	    prof->user,prof->maxSession,prof->actualSession,prof->sesIndex);
#endif
    j = prof->sesIndex;
    while(checkMacSame(j) == 1) /*����֤ʱmac��ַ��ͬ*/
    {
#if DEBUG
	printf("%s-%d: mac check del session, session ip =0x%x\n",__func__,__LINE__,nvramProfile->session[j].ip);
#endif
	/*����IP��OK�������*/ 
	webAuthLog(prof, nvramProfile->session[j].ip, nvramProfile->session[j].mac);
	ipsetAOrDIp(SYS_WEB_AUTH_OK,nvramProfile->session[j].ip,IPSET_MEM_DEL);
#if (IP_GRP == FYES)
	addr.s_addr = nvramProfile->session[j].ip;
	userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
	uttNfNlHashDelIp(spdNfNlSock, nvramProfile->session[j].ip);/*ɾ��hash���Ӧ��ip*/
#endif
	/*ע���Ự*/
	prof->sesIndex = nvramProfile->session[j].nextIndex;
	memset(&(nvramProfile->session), 0U, sizeof(WebAuthSession));
	nvramProfile->session[j].nextIndex = -1;
	j = prof->sesIndex;
	prof->actualSession -= 1;
    }
    if(checkSessionIndex(prof->sesIndex) == 1)
    {
    prev = prof->sesIndex;
    j = nvramProfile->session[prev].nextIndex;
    max = prof->actualSession;
    for(i=0;i<max;i++)
    {
	if(checkMacSame(j) == 1) /*����֤ʱmac��ַ��ͬ*/
	{
#if DEBUG
	    printf("%s-%d: mac check del session, session ip =0x%x\n",__func__,__LINE__,nvramProfile->session[j].ip);
#endif
	    /*����IP��OK�������*/ 
	    webAuthLog(prof, nvramProfile->session[j].ip, nvramProfile->session[j].mac);
	    ipsetAOrDIp(SYS_WEB_AUTH_OK,nvramProfile->session[j].ip,IPSET_MEM_DEL);
#if (IP_GRP == FYES)
	    addr.s_addr = nvramProfile->session[j].ip;
	    userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
	    uttNfNlHashDelIp(spdNfNlSock, nvramProfile->session[j].ip);/*ɾ��hash���Ӧ��ip*/
#endif
	    nvramProfile->session[prev].nextIndex = nvramProfile->session[j].nextIndex;
	    memset(&(nvramProfile->session[j]), 0U, sizeof(WebAuthSession));
	    nvramProfile->session[j].nextIndex = -1;
	    j = nvramProfile->session[prev].nextIndex;
	    prof->actualSession -= 1;
	    continue;
	}
	prev = j;
	j = nvramProfile->session[j].nextIndex;

    }
    }
#if DEBUG
    printf("%s-%d: prof->user =%s,prof->maxSession =%d, prof->actualSession =%d, prof->sesIndex =%d\n",__func__,__LINE__,
	    prof->user,prof->maxSession,prof->actualSession,prof->sesIndex);
#endif
   
    return ret;
}
#endif

/*************************************************************
 * �������ܣ����mac��ַ���ж��û����߻�����,�ҶϷǷ��û�
 * ����ʱ�䣺2013-02-28
 * ����ֵ��0--����֤ʱmac��ַ��ͬ 1--����֤ʱmac��ַ��ͬ 
 *********************************************************/
static int webAuthMacCheck(SystemProfile *profSys, WebAuthProfile *prof)
{    
    char *mac = NULL;
    int ret = 1;/*��������ֵ*/
    /*compareMacEn�򿪱�ʾ��֤ʱ�鵽��mac��ַ����Ҫʹ��mac��ַ�ж�ʱ��*/
    if (prof->compareMacEn == FUN_ENABLE)
    {
	((struct sockaddr_in*)&arpLan.arp_pa)->sin_addr.s_addr = prof->ip;
	memset(arpLan.arp_ha.sa_data, 0, sizeof(arpLan.arp_ha.sa_data));
	mac = getMacByIpIoctl(&arpLan);/*ͨ��arp���ѯ��ǰip�Ķ�Ӧmac��ַ*/
	if (mac == NULL)/*�鲻����ǰmac��ַ���û��Ѿ����߰����쳣����*/
	{
	    memset(prof->mac, 0, sizeof(prof->mac));/*goahead��ֻ���ip���������mac*/
	    ret = 1;
#if (DEBUG_WEBAUTH_ACC == 1)
	    webauth_acc_debug_print("%s--%d:The user has quit\n", __func__, __LINE__);	
#endif
	}
	else
	{
	    ret = ((*(unsigned short*)mac) ^ (*((unsigned short*)(prof->mac)))) || ((*((UINT32*)(mac + 2))) ^ (*((UINT32*)(prof->mac + 2))));/*�Ƚϵ�ǰmac����֤ʱmac��ַ*/
#if (DEBUG_WEBAUTH_ACC == 1)
	    if (ret == 1)
	    {
		webauth_acc_debug_print("%s--%d:Others use this account now, will be hang up\n", __func__, __LINE__);	
	    }
#endif
	}
    }
    else{
#if (WEBAUTH_AUTO == FYES)
	if (isMacZero(prof->mac))
	{
	    /* �ɼ��豸mac */
	    ((struct sockaddr_in*)&arpLan.arp_pa)->sin_addr.s_addr = prof->ip;  
	    memset(arpLan.arp_ha.sa_data, 0, sizeof(arpLan.arp_ha.sa_data));    
	    mac = getMacByIpIoctl(&arpLan);/*ͨ��arp���ѯ��ǰip�Ķ�Ӧmac��ַ*/ 
	    if (mac)
	    {
		memcpy(prof->mac, mac, 6);
		//prof->compareMacEn = FUN_ENABLE;
	    }
	}
#endif
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
	ret = 0;
	SWORD_PRINTF("%s-%d:---flow check---\n",__func__,__LINE__);
#endif
    }
   
    return ret;
}

#if (WEBAUTH_AUTO == FYES)
void printAuthMac(void)
{
   int i;

   printf("%s %d:.............\n", __func__, __LINE__);
   for (i=0; i<MAX_WEB_AUTH_SESSION; i++)
   {
       if (nvramProfile->session[i].ip)
	   printf("%d %#08X %02X%02X-%02X%02X-%02X%02X\n",
		   (unsigned char)nvramProfile->session[i].compareMacEn,
		   (unsigned char)nvramProfile->session[i].ip,
		   (unsigned char)nvramProfile->session[i].mac[0],
		   (unsigned char)nvramProfile->session[i].mac[1],
		   (unsigned char)nvramProfile->session[i].mac[2],
		   (unsigned char)nvramProfile->session[i].mac[3],
		   (unsigned char)nvramProfile->session[i].mac[4],
		   (unsigned char)nvramProfile->session[i].mac[5]);
   }
   printf("\n");
   return;
}
/********************************************************
 *  * �������ܣ�ͳ��ÿ����֤����֤�û�(���š�΢����֤)ʹ��ʱ�䣬��ʱ���û��Ҷϲ�ɾ���˺�
 *   * ����ʱ�䣺2014-01-08
 *    ********************************************************/
void webAuthCaptchaCheck()
{
    int i = 0, webIndexInst;
    MibProfileType mibType = MIB_PROF_WEB_AUTH_CPT;
    WebAuthProfile *prof = NULL;
    int min, max;
    int tmCurrent = time(NULL);
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibTypeSys, 0);

    printAuthMac();

    /* ɾ�����ڵĶ���΢����֤�˺� */
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min;i < max;i++)
    {
	prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
	if ((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE)) {
	    if (webAuthDateCheck(prof, tmCurrent) == 1)
	    {
		printf("headname %s user %s time up, delete it.\n", 
			prof->head.name, prof->user);
		waCancelByProf(prof);/*����ip�Ҷϵ�ʱ�û�*/                                
		if (strncmp(prof->head.name, "CPT", 3) == 0)
		{
		    webIndexInst = (Uint16)strtol(&(prof->head.name[3]), NULL, 10);
		    clearPortMap(&(nvramProfile->webauthInstRec.instRecMap), webIndexInst);
		}
		ProfDelInstByIndex(mibType, i);/*ɾ���˺�*/
	    }
#if (WEB_AUTH_SHARE == FYES)
	    webAuthMacCheckAndDel(profSys, prof);
#else
	    if (webAuthMacCheck(profSys, prof) == 1)
		waCancelByProf(prof);/*�Ҷ�MAC��ʱ�û�*/
#endif
	}
    }

    /* ɾ�����ڵ����֤�Ķ������ɵ��˺� */
    mibType = MIB_PROF_WEB_AUTH;
    ProfInstLowHigh(mibType, &max, &min);                                                  
    for(i = min;i < max;i++)                                                               
    {                                                                                      
	prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);                       
	if ((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE)) {
	    if ((prof->autoAuthMethod == IDCARD_METHOD) && (webAuthDateCheck(prof, tmCurrent) == 1))                                    
	    {                                                                              
		waCancelByProf(prof);/*����ip�Ҷϵ�ʱ�û�*/                                
		if (strncmp(prof->head.name, "ID", 2) == 0)
		{
		        webIndexInst = (Uint16)strtol(&(prof->head.name[2]), NULL, 10);
			    clearPortMap(&(nvramProfile->webauthInstRec.instRecMap), webIndexInst);
		}
		ProfDelInstByIndex(mibType, i);/*ɾ���˺�*/
	    }                                                                              
	}                                                                                  
    }                                                                                      
}
#endif

/********************************************************
 * �������ܣ�ͳ��ÿ��web��֤�û�ʹ��ʱ�䣬�ѷǷ��û���ʱ���û��Ҷ�
 * ����ʱ�䣺2013-02-17
 ********************************************************/
void webAuthCheck(int min, int max)
{
    int i = 0;
    MibProfileType mibType = MIB_PROF_WEB_AUTH;
    WebAuthProfile *prof = NULL;
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibTypeSys, 0);
    struct sysinfo sysInfo;
    int tmCurrent = 0;

    /*
     *�˴�Ϊ�����ź��������˲���
     */
    uttSemP(SEM_WEBAUTH_NO, 0);
    if (webAuthAccNum != 0)/*webAuthAccNum��ʾ�����Ʒѹ��ܵ��û�������Ϊ0ʱ����Ҫ�ж�ʱ��*/
    {   
        sysinfo(&sysInfo);/*����ʱ��*/
        tmCurrent = time(NULL);/*ϵͳʱ��*/
    }   
    for(i = min;i < max;i++)
    {
	prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
	if (ProfInstIsFree(prof) == 0)
	{
#if (WEB_AUTH_SHARE == FYES)
#if DEBUG
	    printf("%s-%d: prof->maxSession =%d,prof->actualSession =%d\n",__func__,__LINE__,prof->maxSession,prof->actualSession);
#endif
	    if (prof->actualSession > 0)/*ipΪ0��ʾδʹ��*/
	    {
		if (prof->timeingPatternEn == FUN_ENABLE)/*�Ʒѹ��ܿ���*/
		{
		    if ((webAuthDateCheck(prof, tmCurrent) == 1) || (webAuthTimeCheck(profSys, prof, &sysInfo) == 1))
		    {
			waCancelByProf(prof);/*����ip�Ҷϵ�ʱ�û�*/
		    }
		}
		webAuthMacCheckAndDel(profSys, prof);
#if 0
		if (webAuthMacCheck(profSys, prof) == 1)
		{
		    waCancelByProf(prof);/*�Ҷ�MAC��ʱ�û�*/
		}
#endif
	    }
#else /*WEB_AUTH_SHARE*/
	    if (prof->ip != 0)/*ipΪ0��ʾδʹ��*/
	    {
		if (prof->timeingPatternEn == FUN_ENABLE)/*�Ʒѹ��ܿ���*/
		{
		    if ((webAuthDateCheck(prof, tmCurrent) == 1) || (webAuthTimeCheck(profSys, prof, &sysInfo) == 1))
		    {
			waCancelByProf(prof);/*����ip�Ҷϵ�ʱ�û�*/
		    }
		}
		if (webAuthMacCheck(profSys, prof) == 1)
		{
		    waCancelByProf(prof);/*�Ҷ�MAC��ʱ�û�*/
		}
	    }
#endif /*WEB_AUTH_SHARE*/
	}
    }

    /*
     *�˴�Ϊ���źŽ���
     */
    uttSemV(SEM_WEBAUTH_NO, 0);

    return;

}

/*************************************
 * �������ܣ�web��֤ʱ��Ʒѹ��ܣ�ÿ80s����һ��ͳ�ƺ���
 * ����ʱ�䣺2013-02-16
 *************************************/
static void webAuthPoll(void)
{
    int step = 0;/*��������ָ���ʮ��֮һ����ÿ��ͳ������*/
    int min = 0;/*ÿ��ͳ������Сһ��*/
    int max = 0;/*ÿ��ͳ�������һ��*/
    step = MAX_WEB_AUTH_PROFILES/10;/*ÿ��ͳ������ָ���ʮ��֮һ*/
    min = step * webAuthAccTimes;
    max = step * (webAuthAccTimes + 1);
    SWORD_PRINTF("%s-%d:times=%d,min=%d,max=%d\n", __func__, __LINE__, webAuthAccTimes + 1, min, max);
    if (webAuthAccTimes == 9)/*��10�ΰ�ʣ��Ķ�ͳ��һ��*/
    {
	max = MAX_WEB_AUTH_PROFILES;
	webAuthAccTimes = 0;/*�´�����ͳ��ǰʮ��֮һ*/
    }
    else
    {
	webAuthAccTimes++;
    }
    webAuthCheck(min, max);/*��min��max֮����û�ͳ��*/
#if (WEBAUTH_AUTO == FYES)
    webAuthCaptchaCheck();/*�Զ�����֤��΢����֤���û�ͳ��*/
#endif
    return;
}
#endif


#if (USER_STATUS == FYES)
#define UTT_STATS_INTERVAL_INIT 300 /* ͳ�ƺ������ü����ʼ300s */
int utt_stats_interval;

/*
 * �û�������Ϊͳ�Ƹ�����Ϣ����
 * ������Ϣ���ں�, ��ʱ�ۼӵ����û�������Ϊ����������Ϊͳ����
 */
static void uttBehaviorStatsPoll(void) {
    UttNlNfStatsMsg nlMsg;

    memset(&nlMsg, 0, sizeof(nlMsg));

    nlMsg.msgHead.nlType = UTT_NLNF_STATS;
    nlMsg.msgType = UTT_BV_STATS_UPDATE;
    uttNfNlSend(spdNfNlSock, (char*)&nlMsg, sizeof(nlMsg), 0 ,0 );/*������Ϣ���ں�*/

    return;
}
#endif
#if CONFIG_XPORT_LOG
static void speedConfigTask(void)
{
    int flag = 0;
    unsigned int sn = 0U;

    sn = getSystemSn();
    /*���������󣬵ȴ�5���ӷ��͸澯��Ϣ*/
    //sleep(30U);
    flag = is_limit();
    if(flag <= 0) /*û���������ã����͸澯��־*/
    {
	syslog(LOG_ERR, "<132>%u@msglog: SPEEDLIMITLACK:speed limit config lack",sn);
    }

    return ;
}
#endif
#if 0
int main(void)
#endif
#if (WLAN_WORK_TIME == FYES)
#define UTT_WIRELESS_SEC_INTERVAL 30
#endif
#if (MAKE_MONEY == FYES)
#define UTT_MONEY_SEC_INTERVAL 30
#endif
static void pollTask(void)
{
#if 0
    ProfInit();
#endif
#if (USER_STATUS == FYES) 
    static int stats_sec;
    stats_sec = 0;
    utt_stats_interval = UTT_STATS_INTERVAL_INIT;
#endif
#if (WEBAUTH_ACCOUNT == FYES)
    static Uint32 pollCount = 0u;
#endif
#if (WLAN_WORK_TIME == FYES)
    static int wireless_sec = 0;
    static int wireless_check_num = 0u;
    int wireless_now_status = 0;
    int wireless_old_status = 0;
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof = NULL;
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 0);
#if (WIRELESS_5G == FYES)
    WirelessProfile *prof5G = NULL;
    prof5G = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 1);
#endif
#endif
#if (MAKE_MONEY == FYES)
    static int money_sec = 0;
    static int money_check_num = 0u;
    int money_now_status = 0;
    int money_old_status = 0;
    MakeMoneyProfile *profM = NULL;
    profM = (MakeMoneyProfile*)ProfGetInstPointByIndex(MIB_PROF_MAKE_MONEY, 0);
#endif
#if (FEATURE_AP_MAIL == FYES)
    static Uint32 mail_sec = 0u;
#endif
#if (WIFIDOG_AUTH == FYES)
    static int wifidog_cnt = 0;
#endif
#if (CWMP == FYES)
    static int cwmpd_cnt = 0;
#endif
#if CONFIG_XPORT_LOG
    static int xport_cnt = 0;
#endif
#if (UTT_SMART_UI == FYES)
#if (APP_POLICY_REMOTE_UPDATE == FYES)
#if (ADFILTER == FYES)
    AdfilterProfile *adfilterProfile;
    unsigned char flag = 0U;

    adfilterProfile = (AdfilterProfile*)ProfGetInstPointByIndex(MIB_PROF_ADFILTER, 0);
#endif
#endif
#endif
    pthread_detach(pthread_self());/*�߳��˳��������ͷ���Դ.��ָ��pthread_join�ɲ��ô˾�*/
    while(1)
    {
	if(pollTaskStatus == 0)
	{
	    pthread_exit(NULL);/*����ѭ����ͷ�˳�����֤����Դ�������ͷ�*/
	}
	else
	{

#if (PPPOE_ACCOUNT == FYES)
	    pppoeAccPoll();
#endif

#if (WEBAUTH_ACCOUNT == FYES)
    if (pollCount < WEB_AUTH_POLL_INTERVAL)
    {
	pollCount++;/*ÿ��8��ͳ��һ��,ͳ������ָ���ʮ��֮һ*/
    }
    else
    {
	webAuthPoll();
	pollCount = 0U;
    }
#endif

#if (USER_STATUS == FYES)
	    stats_sec++;
	    if (stats_sec > utt_stats_interval) {
		uttBehaviorStatsPoll();
		stats_sec = 0;
	    }
#endif
#if (WLAN_WORK_TIME == FYES)
	    wireless_sec++;
	    if (wireless_sec > UTT_WIRELESS_SEC_INTERVAL) {
		    wireless_sec = 0;
		    wireless_now_status = wireless_date_check();
		    /*ʱ�����2013��1��1��*/
		    if (wireless_now_status == -1) {
			wireless_now_status = 1;/*ʱ�������⡣��һֱ����*/
		    }
		    if (wireless_now_status == wireless_old_status) {
			wireless_check_num++;
		    } else {
			wireless_check_num = 0;
			wireless_old_status = wireless_now_status;
		    }
		    if (wireless_check_num == 3) {
			wireless_check_num = 0;
			if( wireless_switch_change(wireless_now_status,prof))
			{
			    doSystem("wlan.sh");
			}
#if (WIRELESS_5G == FYES)
			if( wireless_switch_change(wireless_now_status,prof5G))
			{
			    doSystem("wlan_5g.sh");
			}
#endif
		    }
	    }
#endif
#if (MAKE_MONEY == FYES)
	    uttSemP(SEM_SALE_WIFI,0);
            money_sec++;
            if (money_sec > UTT_MONEY_SEC_INTERVAL) {
                money_sec = 0;
                money_now_status = moneywifi_timeCheck();
                /*ʱ�����2013��1��1��*/
                if (money_now_status == -1) {
                    money_now_status = 0;/*ʱ�������⡣��һֱ�ر�*/
                }
                if (money_now_status == money_old_status) {
                    money_check_num++;
                } else {
                    money_check_num = 0;
                    money_old_status = money_now_status;
                }
                if (money_check_num == 3) {
                    money_check_num = 0;
                    if( moneywifi_switch_change(money_now_status,profM))
                    {
			MakeMoneyInit(profM,0);
		    }
		}
	    }
	    uttSemV(SEM_SALE_WIFI,0);
#endif
#if (FEATURE_AP_MAIL == FYES)
		mail_sec++;
		if(mail_sec > get_ap_mail_time()){
			mail_sec = 0u;
			if(get_ap_mail_enable()){
				ap_send_mail();
			}
		}
#endif
#if (WIFIDOG_AUTH == FYES)
    wifidog_cnt++;
    if(wifidog_cnt > 30)
    {
    wifidog_cnt = 0;
    check_wifidog_by_name("wifidog");
    }
#endif
#if (CWMP == FYES)
     cwmpd_cnt++;
    if(cwmpd_cnt > 30)
    {
	cwmpd_cnt = 0;
	check_cwmpd_by_name("cwmpd");
    }
#endif
#if CONFIG_XPORT_LOG
    if(xport_cnt <=300) {
	xport_cnt++;
	if(xport_cnt == 300) { /*300s*/
	    speedConfigTask();
	}
    }
#endif
#if (UTT_SMART_UI == FYES)
#if (APP_POLICY_REMOTE_UPDATE == FYES)
#if (ADFILTER == FYES)
	    if (adfilterProfile != NULL) {
		policyUpdateCheckTask(adfilterProfile);
	    }
#endif
#endif
#endif
	    sleep(POLL_INTERVAL);/*����ȴ�*/
	}
    }
    return;
}
/**
 * ����һ���߳� �� ͳ��pppoe�û�״̬
 * crOrRe : 1 creat a thread for pppoe  poll statistic
 *          0 remove the thread
 */
void pollTaskInit(int crOrRe)
{
    pollTaskStatus = crOrRe;
    if(crOrRe == 1)
    {
	if( pthread_create(&poll_task_thread, NULL, (void*)pollTask, NULL) != 0) 
	{
	    printf("Create threat for poll task error!\n");
	}
    }
    
    return;
}
#endif
