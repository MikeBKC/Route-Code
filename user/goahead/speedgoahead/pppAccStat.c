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

/*每个轮询内，最多处理的pppoe计费账号数,太大会增加CPU瞬间负担*/
#define MAX_PPPACC_ONE_POLL   ((MAX_PPPOE_SRV_ACC_PROFILES+15) >> 4u)  /*除以16(小数部分非0则加1)*/

/*每个轮询内，最少处理的PPPOE计费账号，太少则处理一遍所有账号，需要的时间会过长*/
#define MIN_PPPACC_ONE_POLL   ((MAX_PPPOE_SRV_ACC_PROFILES+63) >> 6u)  /*除以64(小数部分非0则加1)*/

/* 每个轮询内，最多处理的PPPOE会话。
 * 注：若账号有会话，就会消耗比较多的CPU资源*/
#define MAX_PPPSESS_ONE_POLL  MIN_PPPACC_ONE_POLL
#endif

#if (WEBAUTH_ACCOUNT == FYES)
static int webAuthAccTimes = 0;/*统计次数 80秒分十次*/
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
	if ((adfilterProfile->head.active == TRUE)/*功能开启*/
		&& (adfilterProfile->updateType == 0)/*且更新方式为自动*/
		&& (adfilterProfile->oldVersion == adfilterProfile->newVersion)) {/*服务器端反馈新版本仍可支持自动升级*/
	    if (policyUpdateCheck(adfilterProfile,flag) == 0) {/*执行检测程序*/
		flag = 1U;
	    }
	}
	uttSemV(SEM_ADFILTER,0);
    }
    if (flag) {/*执行完检测程序*/
	uttSemP(SEM_ADFILTER,0);
	if ((adfilterProfile->head.active == TRUE)/*功能开启*/
		&& (adfilterProfile->updateType == 0)/*且更新方式为自动*/
		&& (adfilterProfile->oldVersion == adfilterProfile->newVersion)) {/*服务器端反馈新版本仍可支持自动升级*/
	    policyUpdateCheck(adfilterProfile,flag);/*执行更新程序*/
	    if (flag++ % 10 == 0) {
		flag = 0U;
	    }
	} else {
	    flag = 0U;
	}
	uttSemV(SEM_ADFILTER,0);
    }
    /*每检测完一次，需重新计算出下次检测的时间点*/
    if ((policyCounter == oldCheckCycle) && (oldCheckCycle != 0)) {
	uttSemP(SEM_ADFILTER,0);
	if ((adfilterProfile->head.active == TRUE)/*功能开启*/
		&& (adfilterProfile->updateType == 0)/*且更新方式为自动*/
		&& (adfilterProfile->oldVersion == adfilterProfile->newVersion)) {/*服务器端反馈新版本仍可支持自动升级*/
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
 * 统计单个pppoe用户的状态，并更新相关规则
 */
void upPppoeAcc(PPPoESrvAccProfile*prof)
{
    emPppoeAccStatus oldSta = prof->expired;/*备份老的状态*/
    upPppAccStatus(prof);/*更新现有状态*/
    if(oldSta != prof->expired)
    {
	upPppAccRule(prof, oldSta, 0);/*删除原有规则*/
	upPppAccRule(prof, prof->expired, 1);/*添加新规则*/
    }

    return;
}
#else
#endif
/**
 * 统计所有pppoe用户的状态，并更新规则
 */
void pppoeAccCheck(void)
{
    static int nextEnd = 0;/*下一个轮询，需处理的起始账号索引*/
    int sessCheckNum = 0;
    int max = nextEnd + MAX_PPPACC_ONE_POLL;/*本函数最多处理的账号结束索引(多1)*/
    int min = nextEnd + MIN_PPPACC_ONE_POLL - 1;/*本函数最少处理的账号结束索引*/
    int i = 0;

    MibProfileType mibType = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *info = NULL;

    if (max > MAX_PPPOE_SRV_ACC_PROFILES)
    {/*不能超过实际的账号上限*/
	max = MAX_PPPOE_SRV_ACC_PROFILES;
    }
    uttGlTime = time(NULL);
    /*
     *此处为加上信号量锁定此操作
     */
    uttSemP(SEM_PPPOE_NO,0);
    for(i = nextEnd;i < max;i++)
    {
	info = (PPPoESrvAccProfile*)ProfGetInstPointByIndex(mibType, i);
	if(ProfInstIsFree(info) == 0)
	{
#if 0
	    if( (info->ConTime == 0u) && (info->chargeType !=  NONE_CHARG_MODE))/*在线并且开启了计费*/
	    {		   
		upPppoeAcc(info);
	    }
#else
	    if((info->actualSession > 0) && (info->chargeType !=  NONE_CHARG_MODE))
	    {/*该账号有会话数。即有人使用该账号拨号。才有统计计费的必要*/
		pppoeSesCheck(info);
		sessCheckNum += info->actualSession;
		if((sessCheckNum >= MAX_PPPSESS_ONE_POLL) 
			&& (i>=min))
		{/*处理的会话数和账号数都达到要求*/
                    i++;/*指向下一个索引*/
		    break;
		}

	    }
#endif
	}
    }
   

    /*
     *此处为发信号解锁
     */
    uttSemV(SEM_PPPOE_NO,0);

    uttGlTime = 0;
    nextEnd = i;
    if (nextEnd >= MAX_PPPOE_SRV_ACC_PROFILES)
    {/*处理完了，从头开始*/
	nextEnd = 0;
    }

    return;
}

/**
 * pppoe 账号计费统计函数
 * **/
static void pppoeAccPoll(void)
{
    static Uint32 pollCount = 0u;

    /*这里如果精度不够。则可考虑使用计时器*/
    if(pollCount < PPPOE_POLL_INTERVAL)
    {
	pollCount++;/*记录主循环 循环多少次。执行pppoe统计*/
    }
    else
    {/*执行一次统计*/
	pppoeAccCheck();
	pollCount = 0u;
    }
    return;

}
#endif

#if (WEBAUTH_ACCOUNT == FYES)
/**************************************************************
 * 函数功能：根据ip挂断到时间用户或非法用户
 * 创建时间：2013-02-18
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
	/*将此IP从OK规则清除*/ 
	webAuthLog(prof, nvramProfile->session[j].ip, nvramProfile->session[j].mac);
	ipsetAOrDIp(SYS_WEB_AUTH_OK,nvramProfile->session[j].ip,IPSET_MEM_DEL);
#if (IP_GRP == FYES)
	addr.s_addr = nvramProfile->session[j].ip;
	userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
	uttNfNlHashDelIp(spdNfNlSock, nvramProfile->session[j].ip);/*删除hash表对应的ip*/
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

    /*将此IP从OK规则清除*/ 
    webAuthLog(prof, prof->ip, prof->mac);
    ipsetAOrDIp(SYS_WEB_AUTH_OK,prof->ip,IPSET_MEM_DEL);
#if (IP_GRP == FYES)
    addr.s_addr = prof->ip;
    userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
    uttNfNlHashDelIp(spdNfNlSock, prof->ip);/*删除hash表对应的ip*/
#endif
    prof->ip = 0U;/*清空ip*/
    memset(prof->mac, 0, sizeof(prof->mac));/*清空mac*/
#endif /*WEB_AUTH_SHARE*/
}

#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
/********************************************************************
 * 函数： searchDelSessionByIp
 * 功能： 根据用户IP地址查找账号会话信息，查找到则注销会话
 * 创建： 2013-09-24
 * 参数： ip - 	用户IP地址
 * 返回： 
 *	  1  -  查到会话且注销
 *	  0  -  未查找到
 * 输出： 略
 * 作者： guo.deyuan
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
		/*根据IP查找对应账号*/
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
	/*将此IP从OK规则清除*/
	ipsetAOrDIp(SYS_WEB_AUTH_OK,ip,IPSET_MEM_DEL); 
#if (IP_GRP == FYES)
	addr.s_addr = ip;
#if DEBUG
	printf("%s-%d: prof->user =%s find and del inet_ntoa(addr) =%s\n",__func__,__LINE__,prof->user,inet_ntoa(addr));
#endif
	userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
	uttNfNlHashDelIp(spdNfNlSock, ip);/*删除hash表对应的ip*/
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
	    /*根据IP查找对应账号*/
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
 * 函数功能：web认证日期检查
 * 创建时间：2013-02-28
 * 函数参数：timeCurrent--当前时间
 * 返回值：0--合格 1--过期
 ************************************************************/
static int webAuthDateCheck(WebAuthProfile *prof, int tmCurrent)
{
    int ret = 1;
    /*当前时间在用户可用日期范围内，或者在出厂时间2小时内，不当做过期*/
    if (((prof->timeStart <= tmCurrent) && (tmCurrent <= prof->timeStop)) || ((DELIVERY_START_TIME <= tmCurrent) && (tmCurrent <= DELIVERY_END_TIME)))
    {
	ret = 0;/*正常*/
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
 * 函数功能：修改用户已使用时间，判断剩余时间
 * 创建时间：2013-02-28
 * 返回值：0--合格 1--不合格
 ************************************************************/
static int webAuthTimeCheck(SystemProfile *profSys, WebAuthProfile *prof, struct sysinfo *psysInfo)
{
    int ret = 0;
    if (prof->timeTotal != 0U)
    {
	prof->timeUsed += (unsigned long)psysInfo->uptime - prof->timeLast;/*当前时间减去上一次保存时间，得出使用时间*/
	if (prof->timeUsed >= prof->timeTotal)/*已使用时间超过总时间*/
	{
	    ret = 1;
#if (DEBUG_WEBAUTH_ACC == 1)
	    webauth_acc_debug_print("%s--%d:This account don't have time remaining, will be hang up\n", __func__, __LINE__);	
#endif
	}
	prof->timeLast = (unsigned long)psysInfo->uptime;
	profSys->flashWt |= WEB_AUTH_FLASH_W;/*打开web认证计费写flash标志*/
    }
    return ret;
}

#if (WEBAUTH_AUTO == FYES)
/*************************************************************
 *  * 函数功能：检查mac地址是否与认证时相同
 *   * 创建时间：2013-09-25
 *    * 返回值：0--与认证时mac地址相同 1--与认证时mac地址不同 
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
 * 函数功能：检查mac地址是否与认证时相同
 * 创建时间：2013-09-25
 * 返回值：0--与认证时mac地址相同 1--与认证时mac地址不同 
 *********************************************************/
static int checkMacSame(int index)
{
    char *mac = NULL;
    int ret = 0;/*函数返回值*/

    if(checkSessionIndex(index) == 1)
    {
    if (nvramProfile->session[index].compareMacEn == FUN_ENABLE)
    {
	((struct sockaddr_in*)&arpLan.arp_pa)->sin_addr.s_addr = nvramProfile->session[index].ip;
	memset(arpLan.arp_ha.sa_data, 0, sizeof(arpLan.arp_ha.sa_data));
	mac = getMacByIpIoctl(&arpLan);/*通过arp表查询当前ip的对应mac地址*/
	if (mac == NULL)/*查不到当前mac地址，用户已经下线包括异常下线*/
	{
	    memset(nvramProfile->session[index].mac, 0, sizeof(nvramProfile->session[index].mac));/*goahead中只清空ip，在这清空mac*/
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
	    ret = ((*(unsigned short*)mac) ^ (*((unsigned short*)(nvramProfile->session[index].mac)))) || ((*((UINT32*)(mac + 2))) ^ (*((UINT32*)(nvramProfile->session[index].mac + 2))));/*比较当前mac与认证时mac地址*/
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
	/* 如果上次获取到mac，则本次不用再获取 */
	if (isMacZero(nvramProfile->session[index].mac))
	{
	    ((struct sockaddr_in*)&arpLan.arp_pa)->sin_addr.s_addr = nvramProfile->session[index].ip;
	    memset(arpLan.arp_ha.sa_data, 0, sizeof(arpLan.arp_ha.sa_data));
	    mac = getMacByIpIoctl(&arpLan);/*通过arp表查询当前ip的对应mac地址*/
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
 * 函数功能：检查mac地址，判断用户在线或下线,挂断非法用户
 * 创建时间：2013-02-28
 * 返回值：
 *********************************************************/
static int webAuthMacCheckAndDel(SystemProfile *profSys, WebAuthProfile *prof)
{    
    int ret = 1;/*函数返回值*/
    int prev = -1;
    int i = 0, j = -1;
    int max = 0;
#if (IP_GRP == FYES)
    struct in_addr addr;
#endif
    /*compareMacEn打开表示认证时查到了mac地址，需要使用mac地址判断时间*/
#if DEBUG
    printf("%s-%d: prof->user =%s,prof->maxSession =%d, prof->actualSession =%d, prof->sesIndex =%d\n",__func__,__LINE__,
	    prof->user,prof->maxSession,prof->actualSession,prof->sesIndex);
#endif
    j = prof->sesIndex;
    while(checkMacSame(j) == 1) /*与认证时mac地址不同*/
    {
#if DEBUG
	printf("%s-%d: mac check del session, session ip =0x%x\n",__func__,__LINE__,nvramProfile->session[j].ip);
#endif
	/*将此IP从OK规则清除*/ 
	webAuthLog(prof, nvramProfile->session[j].ip, nvramProfile->session[j].mac);
	ipsetAOrDIp(SYS_WEB_AUTH_OK,nvramProfile->session[j].ip,IPSET_MEM_DEL);
#if (IP_GRP == FYES)
	addr.s_addr = nvramProfile->session[j].ip;
	userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
	uttNfNlHashDelIp(spdNfNlSock, nvramProfile->session[j].ip);/*删除hash表对应的ip*/
#endif
	/*注销会话*/
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
	if(checkMacSame(j) == 1) /*与认证时mac地址不同*/
	{
#if DEBUG
	    printf("%s-%d: mac check del session, session ip =0x%x\n",__func__,__LINE__,nvramProfile->session[j].ip);
#endif
	    /*将此IP从OK规则清除*/ 
	    webAuthLog(prof, nvramProfile->session[j].ip, nvramProfile->session[j].mac);
	    ipsetAOrDIp(SYS_WEB_AUTH_OK,nvramProfile->session[j].ip,IPSET_MEM_DEL);
#if (IP_GRP == FYES)
	    addr.s_addr = nvramProfile->session[j].ip;
	    userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
	    uttNfNlHashDelIp(spdNfNlSock, nvramProfile->session[j].ip);/*删除hash表对应的ip*/
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
 * 函数功能：检查mac地址，判断用户在线或下线,挂断非法用户
 * 创建时间：2013-02-28
 * 返回值：0--与认证时mac地址相同 1--与认证时mac地址不同 
 *********************************************************/
static int webAuthMacCheck(SystemProfile *profSys, WebAuthProfile *prof)
{    
    char *mac = NULL;
    int ret = 1;/*函数返回值*/
    /*compareMacEn打开表示认证时查到了mac地址，需要使用mac地址判断时间*/
    if (prof->compareMacEn == FUN_ENABLE)
    {
	((struct sockaddr_in*)&arpLan.arp_pa)->sin_addr.s_addr = prof->ip;
	memset(arpLan.arp_ha.sa_data, 0, sizeof(arpLan.arp_ha.sa_data));
	mac = getMacByIpIoctl(&arpLan);/*通过arp表查询当前ip的对应mac地址*/
	if (mac == NULL)/*查不到当前mac地址，用户已经下线包括异常下线*/
	{
	    memset(prof->mac, 0, sizeof(prof->mac));/*goahead中只清空ip，在这清空mac*/
	    ret = 1;
#if (DEBUG_WEBAUTH_ACC == 1)
	    webauth_acc_debug_print("%s--%d:The user has quit\n", __func__, __LINE__);	
#endif
	}
	else
	{
	    ret = ((*(unsigned short*)mac) ^ (*((unsigned short*)(prof->mac)))) || ((*((UINT32*)(mac + 2))) ^ (*((UINT32*)(prof->mac + 2))));/*比较当前mac与认证时mac地址*/
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
	    /* 采集设备mac */
	    ((struct sockaddr_in*)&arpLan.arp_pa)->sin_addr.s_addr = prof->ip;  
	    memset(arpLan.arp_ha.sa_data, 0, sizeof(arpLan.arp_ha.sa_data));    
	    mac = getMacByIpIoctl(&arpLan);/*通过arp表查询当前ip的对应mac地址*/ 
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
 *  * 函数功能：统计每个验证码认证用户(短信、微信认证)使用时间，到时间用户挂断并删除账号
 *   * 创建时间：2014-01-08
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

    /* 删除到期的短信微信认证账号 */
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min;i < max;i++)
    {
	prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
	if ((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE)) {
	    if (webAuthDateCheck(prof, tmCurrent) == 1)
	    {
		printf("headname %s user %s time up, delete it.\n", 
			prof->head.name, prof->user);
		waCancelByProf(prof);/*根据ip挂断到时用户*/                                
		if (strncmp(prof->head.name, "CPT", 3) == 0)
		{
		    webIndexInst = (Uint16)strtol(&(prof->head.name[3]), NULL, 10);
		    clearPortMap(&(nvramProfile->webauthInstRec.instRecMap), webIndexInst);
		}
		ProfDelInstByIndex(mibType, i);/*删除账号*/
	    }
#if (WEB_AUTH_SHARE == FYES)
	    webAuthMacCheckAndDel(profSys, prof);
#else
	    if (webAuthMacCheck(profSys, prof) == 1)
		waCancelByProf(prof);/*挂断MAC超时用户*/
#endif
	}
    }

    /* 删除到期的身份证阅读器生成的账号 */
    mibType = MIB_PROF_WEB_AUTH;
    ProfInstLowHigh(mibType, &max, &min);                                                  
    for(i = min;i < max;i++)                                                               
    {                                                                                      
	prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);                       
	if ((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE)) {
	    if ((prof->autoAuthMethod == IDCARD_METHOD) && (webAuthDateCheck(prof, tmCurrent) == 1))                                    
	    {                                                                              
		waCancelByProf(prof);/*根据ip挂断到时用户*/                                
		if (strncmp(prof->head.name, "ID", 2) == 0)
		{
		        webIndexInst = (Uint16)strtol(&(prof->head.name[2]), NULL, 10);
			    clearPortMap(&(nvramProfile->webauthInstRec.instRecMap), webIndexInst);
		}
		ProfDelInstByIndex(mibType, i);/*删除账号*/
	    }                                                                              
	}                                                                                  
    }                                                                                      
}
#endif

/********************************************************
 * 函数功能：统计每个web认证用户使用时间，把非法用户或到时间用户挂断
 * 创建时间：2013-02-17
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
     *此处为加上信号量锁定此操作
     */
    uttSemP(SEM_WEBAUTH_NO, 0);
    if (webAuthAccNum != 0)/*webAuthAccNum表示开启计费功能的用户个数，为0时不需要判断时间*/
    {   
        sysinfo(&sysInfo);/*开机时间*/
        tmCurrent = time(NULL);/*系统时间*/
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
	    if (prof->actualSession > 0)/*ip为0表示未使用*/
	    {
		if (prof->timeingPatternEn == FUN_ENABLE)/*计费功能开启*/
		{
		    if ((webAuthDateCheck(prof, tmCurrent) == 1) || (webAuthTimeCheck(profSys, prof, &sysInfo) == 1))
		    {
			waCancelByProf(prof);/*根据ip挂断到时用户*/
		    }
		}
		webAuthMacCheckAndDel(profSys, prof);
#if 0
		if (webAuthMacCheck(profSys, prof) == 1)
		{
		    waCancelByProf(prof);/*挂断MAC超时用户*/
		}
#endif
	    }
#else /*WEB_AUTH_SHARE*/
	    if (prof->ip != 0)/*ip为0表示未使用*/
	    {
		if (prof->timeingPatternEn == FUN_ENABLE)/*计费功能开启*/
		{
		    if ((webAuthDateCheck(prof, tmCurrent) == 1) || (webAuthTimeCheck(profSys, prof, &sysInfo) == 1))
		    {
			waCancelByProf(prof);/*根据ip挂断到时用户*/
		    }
		}
		if (webAuthMacCheck(profSys, prof) == 1)
		{
		    waCancelByProf(prof);/*挂断MAC超时用户*/
		}
	    }
#endif /*WEB_AUTH_SHARE*/
	}
    }

    /*
     *此处为发信号解锁
     */
    uttSemV(SEM_WEBAUTH_NO, 0);

    return;

}

/*************************************
 * 函数功能：web认证时间计费功能，每80s调用一次统计函数
 * 创建时间：2013-02-16
 *************************************/
static void webAuthPoll(void)
{
    int step = 0;/*保存性能指标的十分之一，即每次统计数量*/
    int min = 0;/*每次统计中最小一个*/
    int max = 0;/*每次统计中最大一个*/
    step = MAX_WEB_AUTH_PROFILES/10;/*每次统计性能指标的十分之一*/
    min = step * webAuthAccTimes;
    max = step * (webAuthAccTimes + 1);
    SWORD_PRINTF("%s-%d:times=%d,min=%d,max=%d\n", __func__, __LINE__, webAuthAccTimes + 1, min, max);
    if (webAuthAccTimes == 9)/*第10次把剩余的都统计一遍*/
    {
	max = MAX_WEB_AUTH_PROFILES;
	webAuthAccTimes = 0;/*下次重新统计前十分之一*/
    }
    else
    {
	webAuthAccTimes++;
    }
    webAuthCheck(min, max);/*对min与max之间的用户统计*/
#if (WEBAUTH_AUTO == FYES)
    webAuthCaptchaCheck();/*对短信认证和微信认证的用户统计*/
#endif
    return;
}
#endif


#if (USER_STATUS == FYES)
#define UTT_STATS_INTERVAL_INIT 300 /* 统计函数调用间隔初始300s */
int utt_stats_interval;

/*
 * 用户上网行为统计更新消息函数
 * 发送消息到内核, 定时累加单个用户上网行为到总上网行为统计中
 */
static void uttBehaviorStatsPoll(void) {
    UttNlNfStatsMsg nlMsg;

    memset(&nlMsg, 0, sizeof(nlMsg));

    nlMsg.msgHead.nlType = UTT_NLNF_STATS;
    nlMsg.msgType = UTT_BV_STATS_UPDATE;
    uttNfNlSend(spdNfNlSock, (char*)&nlMsg, sizeof(nlMsg), 0 ,0 );/*发送消息给内核*/

    return;
}
#endif
#if CONFIG_XPORT_LOG
static void speedConfigTask(void)
{
    int flag = 0;
    unsigned int sn = 0U;

    sn = getSystemSn();
    /*开机启动后，等待5分钟发送告警信息*/
    //sleep(30U);
    flag = is_limit();
    if(flag <= 0) /*没有限速配置，发送告警日志*/
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
    pthread_detach(pthread_self());/*线程退出后立即释放资源.若指定pthread_join可不用此举*/
    while(1)
    {
	if(pollTaskStatus == 0)
	{
	    pthread_exit(NULL);/*放在循环开头退出。保证锁资源被正常释放*/
	}
	else
	{

#if (PPPOE_ACCOUNT == FYES)
	    pppoeAccPoll();
#endif

#if (WEBAUTH_ACCOUNT == FYES)
    if (pollCount < WEB_AUTH_POLL_INTERVAL)
    {
	pollCount++;/*每隔8秒统计一次,统计性能指标的十分之一*/
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
		    /*时间大于2013年1月1日*/
		    if (wireless_now_status == -1) {
			wireless_now_status = 1;/*时间有问题。则一直开启*/
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
                /*时间大于2013年1月1日*/
                if (money_now_status == -1) {
                    money_now_status = 0;/*时间有问题。则一直关闭*/
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
	    sleep(POLL_INTERVAL);/*挂起等待*/
	}
    }
    return;
}
/**
 * 创建一个线程 来 统计pppoe用户状态
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
