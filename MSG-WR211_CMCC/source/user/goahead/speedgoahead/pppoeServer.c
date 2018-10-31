#include "uttMachine.h"
#if (PPPOE_SERVER == FYES)
/*
 *added by brwang for speed goahead
 */
#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>
#include    <dirent.h>
#include    <unistd.h>
#include    <sys/ioctl.h>
#include    <net/if.h>
#include    <sys/socket.h>
#include    <netinet/in.h>
#include    <arpa/inet.h>
#include    "internet.h"
#include    <sys/types.h>
#include    <sys/ipc.h>
#include    <sys/msg.h>
#include    <sys/stat.h>
#include    <signal.h>
#include    <sys/shm.h>
#include    "utt_conf_define.h"
#include    "utt_pppoeClient.h" 
#include    "pppoeServer.h"
#include    "fcntl.h"

#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "spdComFun.h"
#include "argcmd.h"
#include "a2n.h"
#include "comExeFun.h"
#if (FIREWALL == FYES)
#include "firewall.h"
#if 0
extern void iptablesIPPortFilterRun(void);
#endif
extern void iptablesFilterRun(void);
#endif
static void killallPppoeClient(void);
#ifdef CONFIG_UTT_NETFILTER
extern int spdNfNlSock;
#endif
#if (PPPOE_ACCOUNT == FYES)
extern int initRuleToPreDrop(char * oldip, char *newip);
#endif
#if (PPPOE_LIMIT == FYES)
extern char* getTimeForIptables(char* timeStart, char* timeEnd);
extern char* getDayForIptables(Uint32 day);
static void  iptablesPppoeAccInit(void);
static void utt_init_PPPOEACC_LIMIT_CHAIN(char action);
#ifndef CONFIG_PERFORMANCE_OPTIMIZATION
static void EditOnePPPOELimitGroup(const PppoeAccLimit *pppoeAccLimit, char action);
#endif
static void pppAccLimIptRuleInit();
static int pppoeServerInit(void);
static int pppoeAccLimitAdd(PPPoESrvAccProfile *prof, PPPoESrvGloProfile *profS);
static int pppoeAccLimitDel(PPPoESrvAccProfile *prof, PPPoESrvGloProfile *profS);
static int pppoeAccLimitDelAll(void);
static int pppoeAccLimitChange(PPPoESrvAccProfile *prof, ProfChangeType changeType);
#if (GROUP_BASE == FYES)
extern void utt_init_GROUP_LIMIT_CHAIN(LimitType type, char action);
#endif
#endif
#ifdef PPPOE_SRV_DBG
#define PPPOE_SRV_PRINTF(fmt, args...)		printf(fmt, ## args)	
#else
#define PPPOE_SRV_PRINTF(fmt, args...)
#endif

#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
extern int profChangeNfNlSock;
extern void sendMsgToKernel(int socket, MibProfileType proftype, int instIndex, Pointer oldDataPtr, int profsize, ProfChangeType changeType);
#endif

/**
 * 以fromIP,toIp的形式获取pppoe地址池
 * return:静态局部变量
 */
extern void getPoePoolFT(PPPoESrvGloProfile *prof, char** ipFrom, char** ipTo)
{
    static char fIp_a[16];
    static char tIp_a[16];
    struct in_addr fIp, tIp;

    memset(fIp_a, 0, sizeof(fIp_a));
    memset(tIp_a, 0, sizeof(tIp_a));

    fIp.s_addr = prof->ipStart;
    tIp.s_addr = htonl(ntohl(prof->ipStart) + prof->maxSessCnt - 1);
    strncpy(fIp_a, inet_ntoa(fIp), FW_IP_LEN);
    strncpy(tIp_a, inet_ntoa(tIp), FW_IP_LEN);

    *ipFrom = fIp_a;
    *ipTo = tIp_a;
    return;
}
#if 0
extern void iptablesSecDefenseRun(void);
#endif
/**
 * 根据lan口地址变化进行变化的iptables规则
 */
static void iptablesOnPoePool(PPPoESrvGloProfile *oldProf, PPPoESrvGloProfile *newProf)
{
    char *ipFrom = NULL, *ipTo = NULL;
#if (PPPOE_ACCOUNT == FYES)
    char ip[16];
    if(oldProf->head.active != newProf->head.active)
    {
	memset(ip, 0, sizeof(ip));
	/* 获得lan口IP */
	getIfIp(getLanIfName(), ip);
	/*过期账号丢包要用到pre_drop*/
	initRuleToPreDrop(ip, ip);
    }
#endif
#if (NOTICE == FYES)
    if(newProf->head.active == TRUE)
    {
	getPoePoolFT(newProf, &ipFrom, &ipTo);
#if (PPPOE_ACCOUNT == FYES)
	noticeRGrpEdit(POE_NT_R, ipFrom, ipTo);/*修改通告记录组的范围*/
#endif

    }
#endif
    return;
}

#if 0
/*
 * 获取指定账号IP地址
 * 参数：user  PPPOE账号
 * 返回值：char*  PPPOE账号USER对应的IP地址
 */
static char* getPppoeIpAddress(unsigned char *user)
{
    static int fd = -1;
    struct st_uttPppdClient info;
    int st_size = sizeof(struct st_uttPppdClient);
    fd = open(UTT_PPPOE_CLIENT_INFO_FILE, O_RDONLY);
    if(fd != -1) {
        while(read(fd, &info, st_size) > 0) {
            if(strncmp(user, info.user, UTT_PPP_CLIENT_NAME_LEN) == 0) {
		return info.ip;
            }
        }
    }
    return NULL;
}
/********************************************************************
 * 函数： getPppoeIpAddress 
 * 功能： 获取指定账号IP地址
 * 创建： 2012-03-28
 * 参数： user  PPPOE账号
 * 返回： char*  PPPOE账号USER对应的IP地址
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static char* getPppoeIpAddress(char *user)
{
    int max = 0;
    int min = 0;
    int i = 0;

    MibProfileType mibType = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *info = NULL;
    ProfInstLowHigh(mibType, &max, &min);

    /*
     *此处为加上信号量锁定此操作
     */
    uttSemP(SEM_PPPOE_NO,0);

    for(i = min;i < max;i++)
    {
	info = (PPPoESrvAccProfile*)ProfGetInstPointByIndex(mibType, i);
	if(info != NULL)
	{
	    if(strncmp(user, info->pppoename, UTT_PPP_CLIENT_NAME_LEN) == 0) 
	    {
		return info->ip;
            }
        }
    }

    /*
     *此处为发信号解锁
     */
    uttSemV(SEM_PPPOE_NO,0);

    return NULL;
}
#endif


/*
 * 根据pppoe账号配置生成文件
 * 同时生成绑定pppoe server绑定IP列表，提供给pppoe server分配IP使用
 */
void creatPppoeUserFile(void)
{
    MibProfileType mibTypeA = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *profA = NULL;
    int i = 0, min = 0, max = 0;
    FILE *fp;
    struct in_addr addr;

#if 0
    /* pppoe账号限速：在PPPOE拨号之前 清除之前的PPPOE限速规则*/

    doSystem("iptables -t mangle -F %s",PPPOEACC_LIMIT_CHAIN);
#endif

#if 0
    char *userList;
    char user[80], pwd[80], aspip[80];
    userList = nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_USER);
#endif
    /* 删除所有pppoe server账号 */
    doSystem("sed -i \"/\\t%s\\t/d\" %s", PPPOE_SRV_NAME, PPPOE_SERVER_USER_PAP_FILE);
    doSystem("sed -i \"/\\t%s\\t/d\" %s", PPPOE_SRV_NAME, PPPOE_SERVER_USER_CHAP_FILE);
    /* 清空pppoe绑定IP记录文件 */
    doSystem("cp /dev/null %s -f", PPPOE_SERVER_BIND_IP_FILE);

    fp = fopen(PPPOE_SERVER_USER_ORI_FILE, "w+");
    if(fp != NULL) {
	/*
	 * 将账号,密码,固定IP输出到配置文件
	 */
	ProfInstLowHigh(mibTypeA, &max, &min);

	/*
	 *此处为加上信号量锁定此操作
	 */
	uttSemP(SEM_PPPOE_NO,0);

        for (i = min; i < max; i++) {
	    profA = (PPPoESrvAccProfile *)ProfGetInstPointByIndex(mibTypeA, i);
	    /* mantis 8174: 禁用账号生效 */
	    if((profA != NULL) && (ProfInstIsFree(profA) == 0) && (profA->head.active == 1)) {
		addr.s_addr = profA->bindIp;
		if (profA->bindIp == 0) {
		    fprintf(fp, "%s\t%s\t%s\t%s\n", profA->pppoename, PPPOE_SRV_NAME, profA->passwd, "*");
		} else {
		    fprintf(fp, "%s\t%s\t%s\t%s\n", profA->pppoename, PPPOE_SRV_NAME, profA->passwd, inet_ntoa(addr));
		    /* 将固定IP输出到指定文件 */
		    doSystem("echo %s >> %s &", inet_ntoa(addr), PPPOE_SERVER_BIND_IP_FILE);
		}
	    }
	}

	/*
	 *此处为发信号解锁
	 */
	uttSemV(SEM_PPPOE_NO,0);

#if 0
        while(1) {
            if(sscanf(userList,"%[^,],%[^,],%[^;]", user, pwd, aspip)==3) {
        	userList += strlen(user) + strlen(pwd) + strlen(aspip) + 2;
        	fprintf(fp, "%s\t*\t%s\t%s\n", user, pwd, aspip);
		userList=NextRecordStart(userList);
            } else {
        	break;
            }
        }
#endif
        fclose(fp);
#if 0
        doSystem("cp %s %s -f", PPPOE_SERVER_USER_ORI_FILE, PPPOE_SERVER_USER_PAP_FILE);
        doSystem("cp %s %s -f", PPPOE_SERVER_USER_ORI_FILE, PPPOE_SERVER_USER_CHAP_FILE);
#endif
	doSystem("cat %s >> %s", PPPOE_SERVER_USER_ORI_FILE, PPPOE_SERVER_USER_PAP_FILE);
        doSystem("cat %s >> %s", PPPOE_SERVER_USER_ORI_FILE, PPPOE_SERVER_USER_CHAP_FILE);
    }
    return;
}

#if 0
/*
 * 杀掉所有pppoe server拨号进程
 */
static void killallPppoeClient(void)
{
    int fd = -1;
    struct st_uttPppdClient info;
    char *lanDev = NULL;
    int st_size = sizeof(struct st_uttPppdClient);
    lanDev = (char *)getLanIfName();
    fd = open(UTT_PPPOE_CLIENT_INFO_FILE, O_RDONLY);
    if(fd != -1) {
        while(read(fd, &info, st_size) > 0) {
	    /* mantis 8164:全部删除时删除路由器pppoe拨号连接 */
	    /* STime为0，即连接上的，杀进程 */
	    if ((strcmp(lanDev, info.device) == 0) && (info.ConTime == 0)) {
                doSystem("kill -INT %d", info.pid);
	    }
        }
	close(fd);
    }
#if 0
        doSystem("rm /var/run/uttPppoeClientInfo");
#endif
    return;
}
/********************************************************************
 * 函数： killallPppoeClient 
 * 功能： 杀掉所有pppoe server拨号进程
 * 创建： 2012-03-28
 * 参数： 无
 * 返回： 无
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void killallPppoeClient(void)
{
    int max = 0;
    int min = 0;
    int i = 0;

    MibProfileType mibType = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *info = NULL;

    char *lanDev = NULL;
    lanDev = (char *)getLanIfName();

    ProfInstLowHigh(mibType, &max, &min);

    /*
     *此处为加上信号量锁定此操作
     */
    uttSemP(SEM_PPPOE_NO,0);

    for(i = min;i < max;i++)
    {
	info = (PPPoESrvAccProfile*)ProfGetInstPointByIndex(mibType, i);
	if(info != NULL)
	{
	    /* mantis 8164:全部删除时删除路由器pppoe拨号连接 */
	    /* STime为0，即连接上的，杀进程 */
	    if ((strcmp(lanDev, info->device) == 0) && (info->ConTime == 0)) 
	    {
                doSystem("kill -INT %d", info->pid);
	    }
        }
    }

    /*
     *此处为发信号解锁
     */
    uttSemV(SEM_PPPOE_NO,0);

    return;
}
#else
/********************************************************************
 * 函数： killallPppoeClient 
 * 功能： 杀掉所有pppoe server拨号进程
 * 创建： 2012-08-28
 * 参数： 无
 * 返回： 无
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void killallPppoeClient(void)
{
    int i = 0;

    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *prof = NULL;

    MibProfileType mibType = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *info = NULL;

    /*
     *此处为加上信号量锁定此操作
     */
    uttSemP(SEM_PPPOE_NO,0);

    prof = (PPPoESrvGloProfile*)ProfGetInstPointByIndex(mibTypeS, 0);
    if(prof != NULL)
    {
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
	for(i = 0;i < MAX_PPPOE_SRV_ACC_PROFILES; i++)
#else
	for(i = 0;i < MAX_PPPOE_SRV_SESSION_PROFILES;i++)
#endif
	{
	    if((nvramProfile->Session[i].ConTime == 0U) && (strcmp(nvramProfile->Session[i].device, (char *)getLanIfName()) == 0)) 
	    {
		info = (PPPoESrvAccProfile*)ProfGetInstPointByIndex(mibType, nvramProfile->Session[i].accIndex);
		if(info != NULL)
		{
#if (IP_GRP == FYES)
		    userUpToIpGrp(info->pppoename, nvramProfile->Session[i].ip, IPENT_POE_ACC, IPSET_MEM_DEL);
#endif
#if (PPPOE_ACCOUNT == FYES)
		    if(info->chargeType !=  NONE_CHARG_MODE)
		    {/*开启了计费*/
			upPppAccRule(nvramProfile->Session[i], info->expired, 0);/*删除原来的规则*/
		    }
#endif
#ifdef CONFIG_UTT_NETFILTER
		    uttNfNlHashDelIpStr(spdNfNlSock, nvramProfile->Session[i].ip);/*删除hash表对应的ip*/
#endif
		    nvramProfile->Session[i].ConTime = 1U;
		    nvramProfile->Session[i].nextIndex = -1;
		    nvramProfile->Session[i].accIndex = -1;
		    info->actualSession = 0;
		    info->sesIndex = -1;
		}
	    }
	}
    }

    /*
     *此处为发信号解锁
     */
    uttSemV(SEM_PPPOE_NO,0);

    return;
}
#endif
/*
 * 初始化pppoe server
 */
void initPppoeServer()
{
    char ip[20];
#if 0
    char *startIp, *onlineCount, *pppoeSEn;
#endif
    char *lanif;
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;
    struct in_addr addr;
    PPPOE_SRV_PRINTF("%s has been called\n", __FUNCTION__);
    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS, 0);
    if ((profS == NULL) || (ProfInstIsFree(profS) == 1)) {
	PPPOE_SRV_PRINTF("%s: get profile error\n", __FUNCTION__);
	return;
    }

#if 0
    pppoeSEn = nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_ENABLE);
#endif
    doSystem("pppoeserver.sh");
#if 0
    if(strncmp(pppoeSEn, "on", 2) == 0) {
#endif


/*
*  删除原来的强制PPPOE规则
*/
	doSystem("iptables -t filter -F %s  1>/dev/null 2>&1", ONLYPPPOE_FORWARD);

    if (profS->head.active == 1) {
#if 0
        onlineCount = nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_ONLINECOUNT);
        startIp = nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_STARTIP);
#endif
        creatPppoeUserFile();
        lanif = (char *)getLanIfName();
        getIfIp(lanif, ip);
	addr.s_addr = profS->ipStart;
#if 0
        doSystem("pppoe-server -k -I %s -L %s -R %s -N %s",
               lanif, ip, startIp, onlineCount);
#endif
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
    doSystem("pppoe-server -k -I %s -L %s -R %s -N %d", lanif, ip, inet_ntoa(addr), profS->ipcount);
#else
	doSystem("pppoe-server -k -I %s -L %s -R %s -N %d", lanif, ip, inet_ntoa(addr), profS->maxSessCnt);
#endif
	/*
	 *  初始化强制PPPOE
	 */

#if 0
	  int_startip = profS->ipStart;
	  int_start_endip = ntohl(int_startip) - 1;
	  int_end_startip = ntohl(int_startip) + MAX_PPPOE_SRV_SESSION_PROFILES;
#endif
	  if(profS->onlyPppoe == 1){
	      if(*(profS->exceptIpGroup) != '\0')
	      {
		  doSystem("iptables -A %s -i %s -m set --set %s src -j RETURN 1>/dev/null 2>&1", ONLYPPPOE_FORWARD, (char*)getLanIfName(), profS->exceptIpGroup);
	      }
	      doSystem("iptables -A %s -i %s -j DROP  1>/dev/null 2>&1", ONLYPPPOE_FORWARD,(char*)getLanIfName());
	  }
    }
#if 0
    /* not need */
    iptablesSecDefenseRun();
#endif
    return;
}

/*
 * pppoe serve 账号profile change函数
 */
static void pppoeSrvGloProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr )
{

    switch(changeType) {
	case PROF_CHANGE_ADD:
	    break;
	case PROF_CHANGE_EDIT:
	    PPPOE_SRV_PRINTF("%s: edit!\n", __FUNCTION__);
#if 0
	    /* 
	     * 杀所有pppoe server客户端拨号
	     * 没有放到initPppoeServer函数中，由于项目后期，小改动
	     */
	    killallPppoeClient();
#else
	    /*由于现在pppd杀进程写profile是按ip来的，故这里的杀掉服务器进程并不会进行写profile中的下线操作，故这里要执行所有在线账号下线操作*/
	    killallPppoeClient();
#endif

	    /* 正常杀掉pppoe-server,杀掉又拨进来的pppd进程 */
	    doSystem("killall -q pppoe-server");

	    initPppoeServer();
	    PPPoESrvGloProfile *newProf = (PPPoESrvGloProfile *)newProfileDataPtr;
	    PPPoESrvGloProfile *oldProf = (PPPoESrvGloProfile *)oldProfileDataPtr;
	    /* 如果地址池变化,更新防火墙IP过滤策略 */
	    if ((newProf != NULL) && (oldProf != NULL)) 
	    {

		iptablesOnPoePool(oldProf, newProf);

#if (FIREWALL == FYES)
		if (((newProf->ipStart != oldProf->ipStart) 
			    || (newProf->maxSessCnt != oldProf->maxSessCnt))) {
#if 0
		    doSystem("iptables -F %s 1>/dev/null 2>&1", IPPORT_FILTER_CHAIN);
		    iptablesIPPortFilterRun();
#endif
		    /* ip 链和 uk链合并成一条链 , mantis 8519 */
		    doSystem("iptables -F %s 1>/dev/null 2>&1", ACL_FILTER_CHAIN);
		    iptablesFilterRun();
		}
#endif
	    }

	    /* pppoe srv开启到关闭 */
	    if ((oldProf->head.active == TRUE) && (newProf->head.active == FALSE)) {
		doSystem("iptables -t filter -D FORWARD  -j %s 1>/dev/null 2>&1", ONLYPPPOE_FORWARD);
		doSystem("iptables -t mangle -D PREROUTING -m set --set %s src -m set ! --set %s dst -j %s 1>/dev/null 2>&1", POE_EXPIRED_ACC, SYS_LAN_GROUP, POE_DROP_CHAIN);
		pppoeAccLimitDelAll();
	    } else if (newProf->head.active == TRUE) { /* poe srv开启状态 */
		if (oldProf->head.active == FALSE) {/*原来pppoe srv是关闭的*/
		    doSystem("iptables -t mangle -I PREROUTING -m set --set %s src -m set ! --set %s dst -j %s 1>/dev/null 2>&1", POE_EXPIRED_ACC, SYS_LAN_GROUP, POE_DROP_CHAIN);
		    pppAccLimIptRuleInit();
		}
		if ((oldProf->onlyPppoe == FALSE) && (newProf->onlyPppoe == TRUE)) { /* 只允许pppoe 关闭->开启 */
		    doSystem("iptables -t filter -D FORWARD  -j %s 1>/dev/null 2>&1", ONLYPPPOE_FORWARD);
		    doSystem("iptables -t filter -I FORWARD  -j %s 1>/dev/null 2>&1", ONLYPPPOE_FORWARD);
		} else if ((oldProf->onlyPppoe == TRUE) && (newProf->onlyPppoe == FALSE)) { /* 只允许pppoe 开启->关闭 */
		    doSystem("iptables -t filter -D FORWARD  -j %s 1>/dev/null 2>&1", ONLYPPPOE_FORWARD);
		} else if ((oldProf->head.active == FALSE) && (newProf->onlyPppoe == TRUE)) { /* 服务器关闭->开启,且只允许pppoe开启 */
		    doSystem("iptables -t filter -D FORWARD  -j %s 1>/dev/null 2>&1", ONLYPPPOE_FORWARD);
		    doSystem("iptables -t filter -I FORWARD  -j %s 1>/dev/null 2>&1", ONLYPPPOE_FORWARD);
		}
	    }
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
#if (PPPOE_LIMIT == FYES)
	    sendMsgToKernel(profChangeNfNlSock, MIB_PROF_PPPOE_SRV_GLO,0, (Pointer)NULL,0,PROF_CHANGE_SPECIAL);
#endif
#endif
	    break;
	case PROF_CHANGE_DEL:
	    break;
	case PROF_CHANGE_DELALL:
	    break;
	default:
	    break;
    }
    return;
}

#if (PPPOE_LIMIT == FYES)
/* 
 * 功能：pppoe账号限速初始化,建立新链
 */
static void  iptablesPppoeAccInit(void)
{
    doSystem("iptables -t mangle -D FORWARD -j %s 1>/dev/null 2>&1", PPPOEACC_LIMIT_CHAIN);
    doSystem("iptables -t mangle -F %s  1>/dev/null 2>&1; iptables -t mangle -X %s  1>/dev/null 2>&1", PPPOEACC_LIMIT_CHAIN, PPPOEACC_LIMIT_CHAIN);
    doSystem("iptables -t mangle -N %s 1>/dev/null 2>&1", PPPOEACC_LIMIT_CHAIN);
}

/*
 * 功能：建立或删除跳转到pppoe账号限速链的规则
 * 输入: action - 删除或新建
 */
static void utt_init_PPPOEACC_LIMIT_CHAIN(char action)
{
    static char pppAccLimitCount = 0;/*PPPoE限速规则条数*/
    char pppAccLimitCountBak = pppAccLimitCount;/*备份之前的*/

    if (action == Iptables_ADD)
    {
	pppAccLimitCount += 1;
    }
    else if (action == Iptables_DEL)
    {
	pppAccLimitCount -= 1;
    }
    else if (action == Iptables_FLUSH)
    {
	pppAccLimitCount = 0;
    }

    if (pppAccLimitCountBak <= 0)
    {
	if (pppAccLimitCount > 0)/*由没有到有规则*/
	{
	    doSystem("iptables -t mangle -A FORWARD -j %s 1>/dev/null 2>&1", PPPOEACC_LIMIT_CHAIN);
	}
    }
    else
    {
	if (pppAccLimitCount <= 0)/*所有规则都被删除*/
	{
	    doSystem("iptables -t mangle -D FORWARD -j %s 1>/dev/null 2>&1", PPPOEACC_LIMIT_CHAIN);/*删除跳转规则*/
	}
    }

    return;
}

#ifndef CONFIG_PERFORMANCE_OPTIMIZATION
#define IF_DIR_LEN  32
/*
 * 功能：根据上传下载速率建立或删除限速规则
 * 输入：pppAccLimit - 保存上传下载速率的结构体
 *	 action - 新建或删除
 */
static void EditOnePPPOELimitGroup(const PppoeAccLimit *pppoeAccLimit, char action)
{
    char ipstr[128];
#if 0
    char *timestr, *daystr;
#endif
    char ratestr[32];
    char limittype[32];
    char cmdstr[512];
    int i = 0;
    Uint32 rateTmp = 0u;
    utt_init_PPPOEACC_LIMIT_CHAIN(action);
#if (GROUP_BASE == FYES)
    utt_init_GROUP_LIMIT_CHAIN(PPPOE_ACC_LIMIT, action);
#endif

    char if_dir[IF_DIR_LEN + 1];
    memset(if_dir, 0, sizeof(if_dir));

    /*添加上传以及下载限速*/
    for(i = 0 ;i < 2; i++)
    {
	memset(ipstr, 0, sizeof(ipstr));
	memset(if_dir, 0, IF_DIR_LEN + 1);
	if( i == 0)
	{/*上传限速*/
	    sprintf(ipstr, " -m set --set %s src ", pppoeAccLimit->ipGrpName);
	    strcpy(limittype, "src");/*针对源ip进行限速*/
	    rateTmp = pppoeAccLimit->uRateLimit;
	    strncpy(if_dir, " -m uttdev --lan-to-wan ", IF_DIR_LEN);
	}
	else/*( i == 0)*/
	{/*下载限速*/
	    sprintf(ipstr, " -m set --set %s dst ", pppoeAccLimit->ipGrpName);
	    strcpy(limittype, "dst");/*针对目的ip进行限速*/
	    rateTmp = pppoeAccLimit->dRateLimit;
	    strncpy(if_dir, " -m uttdev --wan-to-lan ", IF_DIR_LEN);
	}/*end -- ( i == 0)*/
#if 0
	timestr = getTimeForIptables("All", 0);/*iptables时间参数*/
	daystr = getDayForIptables(127u);/*iptables天参数*/
#endif

	if (rateTmp != 0u)
	{
	    sprintf(ratestr , "%.0u", rateTmp*1024U/8U);
	    memset(cmdstr, 0, sizeof(cmdstr));
#if 0
	    sprintf(cmdstr, "iptables -t mangle -%c %s%s%s%s%s -m hashlimit --hashlimit-name %d%s --hashlimit %s/sec --hashlimit-burst %s --hashlimit-mode %sip -j RETURN", action ,PPPOEACC_LIMIT_CHAIN, if_dir, ipstr, timestr,daystr , pppoeAccLimit->ipGrpName, limittype, ratestr ,ratestr, limittype);
#endif
	    sprintf(cmdstr, "iptables -t mangle -%c %s%s%s -m hashlimit --hashlimit-name poe%ld%s --hashlimit %s/sec --hashlimit-burst %s --hashlimit-mode %sip -j RETURN", action ,PPPOEACC_LIMIT_CHAIN, if_dir, ipstr, strtol(pppoeAccLimit->ipGrpName + 22, NULL, 10), limittype, ratestr ,ratestr, limittype);
	    doSystem(cmdstr);
	    memset(cmdstr, 0, sizeof(cmdstr));
	    /* 如果已经限速，当速度达到限速值后将数据包丢弃 -j DROP */
#if 0
	    sprintf(cmdstr, "iptables -t mangle -%c %s%s%s%s%s -j DROP", action ,PPPOEACC_LIMIT_CHAIN ,if_dir, ipstr,timestr,daystr);
#endif
	    sprintf(cmdstr, "iptables -t mangle -%c %s%s%s -j DROP", action ,PPPOEACC_LIMIT_CHAIN ,if_dir, ipstr);
	    doSystem(cmdstr);
	}/*end -- (rateTmp != 0u)*/
    }/*end -- for(i = 0 ;i < 2; i++)*/
    return;
}
#endif

/*
 * 创建pppoe限速规则
 */
static void pppAccLimIptRuleInit()
{
    int	j = 0;
    int min = 0, max = 0;
    MibProfileType mibTypeA = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *profA = NULL;
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;

    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS, 0); 
    if (ProfInstIsFree(profS) == 0)
    {
	ProfInstLowHigh(mibTypeA, &max, &min);
	/*遍历pppoe账户结构体，对每个结构体创建规则*/
	for(j = min; j < max; j++) 
	{
	    profA = (PPPoESrvAccProfile*)ProfGetInstPointByIndex(mibTypeA, j);
	    if(ProfInstIsFree(profA) == 0)
	    {
		/*速率为0不需要限制*/
		if ((profA->uRateLimit != 0U) || (profA->dRateLimit != 0U))
		{
		    pppoeAccLimitAdd(profA, profS);
		}
	    }
	}
    }
}

/*
 * 功能：开机时初始化记录速率信息数组和根据配置初始化规则
 */
static int pppoeServerInit(void)
{
    int ret = 0;
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;
    unsigned int i = 0U; 
    char buffer[IP_GROUP_NAME_LEN + 1u];

    uttSemP(SEM_PPPOE_NO,0);/*加信号量。防止其他进程访问*/
    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS, 0); 

    if (ProfInstIsFree(profS) == 0)
    {
	/*循环为记录速率的数组赋组名，组名在建立用户组时使用*/
	for (i=0U;i<MAX_PPPOE_LIMIT_IPGRP;i++)
	{
	    sprintf(buffer, "sys_pppoe_limit_ipgrp_%u", i);/*根据i创建不同的组名*/
	    strncpy(profS->limitGroup[i].ipGrpName, buffer, IP_GROUP_NAME_LEN);
	    PPPOE_SRV_PRINTF("%s--%d:profS->limitGroup[i].ipGrpName=%s\n", __func__, __LINE__, profS->limitGroup[i].ipGrpName);
	}
    }
    pppAccLimIptRuleInit();/*创建规则*/
    uttSemV(SEM_PPPOE_NO,0);/*释放信号量*/
    return ret;
}

/*
 * 功能：添加一组pppoe账号中的上传下载速率到保存速率信息的数组中，并根据需要建立用户组和规则
 * 输入：prof -- pppoe账号结构体，包含一组上传下载速率
 *	 profS -- 包含记录速率信息的数组
 */
static int pppoeAccLimitAdd(PPPoESrvAccProfile *prof, PPPoESrvGloProfile *profS)
{
    unsigned int i = 0U;
    int ret = 0;
    unsigned int index = MAX_PPPOE_LIMIT_IPGRP;/*记录第一个未使用组的下标*/

    for (i=0U;i<MAX_PPPOE_LIMIT_IPGRP;i++)
    {
	PPPOE_SRV_PRINTF("%s--%d:limitGroup[%d].uRateLimit=%d, prof->uRateLimit=%d, limitGroup[%d].dRateLimit=%d, prof->dRateLimit=%d\n", __func__, __LINE__, i, profS->limitGroup[i].uRateLimit, prof->uRateLimit, i, profS->limitGroup[i].dRateLimit, prof->dRateLimit);
	if ((profS->limitGroup[i].uRateLimit == prof->uRateLimit) && (profS->limitGroup[i].dRateLimit == prof->dRateLimit))/*已有相同速率的组和规则，直接添加*/
	{
	    PPPOE_SRV_PRINTF("%s--%d:limitGroup[%d].uRateLimit=%d, prof->uRateLimit=%d, limitGroup[%d].dRateLimit=%d, prof->dRateLimit=%d\n", __func__, __LINE__, i, profS->limitGroup[i].uRateLimit, prof->uRateLimit, i, profS->limitGroup[i].dRateLimit, prof->dRateLimit);
	    profS->limitGroup[i].count += 1;
	    /*如果当前IP在线，将它加入到对应组中*/
	    AccAddIpStr(prof, profS->limitGroup[i].ipGrpName, IPSET_MEM_ADD);
	    AccAddIpStr(prof, POE_SKIP_FIXED_LIMIT, IPSET_MEM_ADD);
	    break;
	}/*end -- ((profS->limitGroup[i].uRateLimit == prof->uRateLimit) && (profS->limitGroup[i].dRateLimit == prof->dRateLimit))*/
	/*记录第一个空的位置*/
	if ((index == MAX_PPPOE_LIMIT_IPGRP) && (profS->limitGroup[i].count == 0))
	{
	    index = i; 
	    PPPOE_SRV_PRINTF("%s--%d:index=%u\n", __func__, __LINE__, index);
	}
    }/*end -- (i=0U;i<MAX_PPPOE_LIMIT_IPGRP;i++)*/
    if ((i == MAX_PPPOE_LIMIT_IPGRP) && (index != MAX_PPPOE_LIMIT_IPGRP))
    {
	profS->limitGroup[index].uRateLimit = prof->uRateLimit;
	profS->limitGroup[index].dRateLimit = prof->dRateLimit;
	profS->limitGroup[index].count ++;
	/*生成用户组*/
	PPPOE_SRV_PRINTF("%s--%d:ipGrpName=%s\n", __func__, __LINE__, profS->limitGroup[index].ipGrpName);
	ipGrpUserAddSet(profS->limitGroup[index].ipGrpName);
	/*如果当前IP在线，将它加入到对应组中*/
	AccAddIpStr(prof, profS->limitGroup[index].ipGrpName, IPSET_MEM_ADD);
	AccAddIpStr(prof, POE_SKIP_FIXED_LIMIT, IPSET_MEM_ADD);
#ifndef CONFIG_PERFORMANCE_OPTIMIZATION
	/*建立规则*/
	EditOnePPPOELimitGroup(&(profS->limitGroup[index]), 'A');
#endif
    }

    return ret;
}

/*
 * 功能：根据pppoe账号结构体中的速率改变数组中引用计数，根据计算删除用户组和规则
 * 输入：prof -- pppoe账号结构体
 *	 profS -- 保存有记录速率信息数组
 */
static int pppoeAccLimitDel(PPPoESrvAccProfile *prof, PPPoESrvGloProfile *profS)
{
    PPPOE_SRV_PRINTF("%s--%d:pppoeAccLimitDel start\n", __func__, __LINE__);
    int ret = 0;
    unsigned int i = 0U;

    for (i=0U;i<MAX_PPPOE_LIMIT_IPGRP;i++)
    {
	if ((profS->limitGroup[i].uRateLimit == prof->uRateLimit) && (profS->limitGroup[i].dRateLimit == prof->dRateLimit))/*已有相同速率的组和规则，直接将引用数减1*/
	{
	    profS->limitGroup[i].count --;
	    AccAddIpStr(prof, profS->limitGroup[i].ipGrpName, IPSET_MEM_DEL);
	    AccAddIpStr(prof, POE_SKIP_FIXED_LIMIT, IPSET_MEM_DEL);
	    break;
	}
    }/*end -- for (i=0U;i<MAX_PPPOE_LIMIT_IPGRP;i++)*/
    PPPOE_SRV_PRINTF("%s--%d:count=%d\n", __func__, __LINE__, profS->limitGroup[i].count);
    if ((i != MAX_PPPOE_LIMIT_IPGRP) && (profS->limitGroup[i].count <= 0))
    {
#ifndef CONFIG_PERFORMANCE_OPTIMIZATION
	/*删除规则*/
	EditOnePPPOELimitGroup(&(profS->limitGroup[i]), 'D');
#endif
	PPPOE_SRV_PRINTF("%s--%d:ipGrpName=%s\n", __func__, __LINE__, profS->limitGroup[i].ipGrpName);
	/*删除组*/
	ipsetDelGrp(profS->limitGroup[i].ipGrpName, TRUE);
	/*将记录信息情况，以备下次加入新速率*/
	profS->limitGroup[i].uRateLimit = 0U;
	profS->limitGroup[i].dRateLimit = 0U;
    }/*end -- (profS->limitGroup[i].count <= 0)*/
    return ret;
}

/*
 * 功能：删除所有pppoe账户限速用到的规则和用户组，将记录速率的数组初始化
 */
static int pppoeAccLimitDelAll(void)
{
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;
    int ret = 0;/*返回值*/
    unsigned int i = 0U;

    uttSemP(SEM_PPPOE_NO,0);/*加信号量防止冲突*/
    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS, 0);
    if (ProfInstIsFree(profS) == 0)
    {
	/*清空限速链*/
	doSystem("iptables -t mangle -F %s 1>/dev/null 2>&1",PPPOEACC_LIMIT_CHAIN);
	/*清除跳转规则*/
	utt_init_PPPOEACC_LIMIT_CHAIN(Iptables_FLUSH);
	/*将记录速率信息数组初始化*/
	for (i=0U;i<MAX_PPPOE_LIMIT_IPGRP;i++)
	{
	    /*删除组*/
	    ipsetDelGrp(profS->limitGroup[i].ipGrpName, TRUE);
	    profS->limitGroup[i].uRateLimit = 0U;/*上传*/
	    profS->limitGroup[i].dRateLimit = 0U;/*下载*/
	    profS->limitGroup[i].count = 0;/*引用计数*/
	}
    }
#if (GROUP_BASE == FYES)
    utt_init_GROUP_LIMIT_CHAIN(PPPOE_ACC_LIMIT, Iptables_FLUSH);
#endif
    ipsetFlushGrp(POE_SKIP_FIXED_LIMIT);/*mantis:11302,清除跳过精细化限速组*/
    uttSemV(SEM_PPPOE_NO,0);/*如果加了锁。则应该释放该锁*/
    return ret;
}

/*
 * 功能：对prof进行检查，调用添加或删除函数
 * 输入：prof -- pppoe账户结构体
 *	 changeType -- 添加或删除标志
 * 返回：0--成功
 */
static int pppoeAccLimitChange(PPPoESrvAccProfile *prof, ProfChangeType changeType)
{
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;
    int ret = 0;/*返回值*/

    uttSemP(SEM_PPPOE_NO,0);/*加信号量。防止其他进程访问*/
    /*没有设置速率，不需要处理*/
    if ((prof->uRateLimit != 0U) || (prof->dRateLimit != 0U))
    {
	PPPOE_SRV_PRINTF("%s--%d:uRateLimit=%d, dRateLimit=%d\n", __func__, __LINE__, prof->uRateLimit, prof->dRateLimit);
	profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS, 0);
	if ((ProfInstIsFree(profS) == 0) && (profS->head.active == TRUE))
	{
	    if (changeType == PROF_CHANGE_ADD)
	    {
		PPPOE_SRV_PRINTF("%s--%d:changeType = PROF_CHANGE_ADD\n", __func__, __LINE__);
		pppoeAccLimitAdd(prof, profS);/*添加*/
	    }
	    else/*PROF_CHANGE_DEL*/
	    {
		pppoeAccLimitDel(prof, profS);/*删除*/
	    }
	}
    }/*end -- if ((prof->uRateLimit != 0) || (prof->dRateLimit != 0))*/

    uttSemV(SEM_PPPOE_NO,0);/*释放信号量*/
    return ret;
}
#endif


/*
 * pppoe serve 账号profile change函数
 */
static void pppoeSrvAccProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr )
{
    switch(changeType) {
	case PROF_CHANGE_ADD:
	    creatPppoeUserFile();
#if (PPPOE_LIMIT == FYES)
	    pppoeAccLimitChange(newProfileDataPtr, PROF_CHANGE_ADD);
#endif
	    break;
	case PROF_CHANGE_ADD_ALL:
#if (PPPOE_LIMIT == FYES)
	    pppoeAccLimitDelAll();
	    pppoeServerInit();
#endif
	    break;
        case PROF_CHANGE_EDIT:
	    creatPppoeUserFile();
#if (PPPOE_LIMIT == FYES)
	    pppoeAccLimitChange(oldProfileDataPtr, PROF_CHANGE_DEL);
	    pppoeAccLimitChange(newProfileDataPtr, PROF_CHANGE_ADD);
#endif
	    break;
        case PROF_CHANGE_DEL:
#if (PPPOE_LIMIT == FYES)
	    pppoeAccLimitChange(oldProfileDataPtr, PROF_CHANGE_DEL);
#endif
	    break;
        case PROF_CHANGE_SPECIAL:   
	    /* 目的是为了只出发一次createPppoeUserFile */
	    creatPppoeUserFile();
#if (PPPOE_LIMIT == FYES)
	    pppoeAccLimitChange(oldProfileDataPtr, PROF_CHANGE_DEL);
#endif
            break;
        case PROF_CHANGE_DELALL:
	    initPppoeServer();
#if (PPPOE_LIMIT == FYES)
	    pppoeAccLimitDelAll();
	    pppoeServerInit();
#endif
            break;
        default:
            break;
    }
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
#if (PPPOE_LIMIT == FYES)
    PPPoESrvGloProfile *profServer = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(MIB_PROF_PPPOE_SRV_GLO,0);
    if (profServer->head.active) {
	sendMsgToKernel(profChangeNfNlSock, MIB_PROF_PPPOE_SRV_GLO,0, (Pointer)NULL,0,PROF_CHANGE_SPECIAL);
    }
#endif
#endif
    return;
}

#if 0
static void  iptablesPppoeGloInit(void)
{
    /*
     * 初始化强制PPPoE 链表
     */
   doSystem("iptables -t filter -D FORWARD -j %s 1>/dev/null 2>&1", ONLYPPPOE_FORWARD);
    doSystem("iptables -t filter -F %s  1>/dev/null 2>&1; iptables -t filter -X %s  1>/dev/null 2>&1", ONLYPPPOE_FORWARD, ONLYPPPOE_FORWARD);
    doSystem("iptables -t filter -N %s 1>/dev/null 2>&1; iptables -t filter -I FORWARD  -j %s 1>/dev/null 2>&1", ONLYPPPOE_FORWARD, ONLYPPPOE_FORWARD);

}
#endif



#if (PPPOE_ACCOUNT == FYES)
#if (NOTICE == FYES)
#if (MULTI_LAN == FYES)
/*增加或删除过期通告规则链到iptables规则
 * action: Iptables_ADD or Iptables_DEL*/
extern void poeNtCapChainAOrD(char action)
{
#if (IP_GRP == FYES)
    doSystem("iptables -t mangle -%c PREROUTING -p tcp --dport 80 -m set --set %s src -m set ! --set %s src -m set ! --set %s dst -j %s 1>/dev/null 2>&1",action, POE_EXPIRING_ACC, POE_NT_OK, SYS_LAN_GROUP, POE_NT_CAP_CHAIN);
#else
#error ipset has been removed
#endif
    return;
}
#else
/*增加或删除过期通告规则链到iptables规则
 * ipstr : lan口ip
 * action: Iptables_ADD or Iptables_DEL*/
extern void poeNtCapChainAOrD(char* ipstr, char action)
{
    doSystem("iptables -t mangle -%c PREROUTING -p tcp --dport 80 ! -d %s -j %s",action, ipstr, POE_NT_CAP_CHAIN);
    return;
}
#endif
/**
 * pppoe 账号过期规则初始化
 *
 */
extern void poeAccExpiringInit(void)
{

	ipsetDelGrp(SYS_HTTP_PORT,TRUE);
	httpPortGrpAdd(SYS_HTTP_PORT);
	httpPortAdd(SYS_HTTP_PORT,80u);
    PPPoESrvGloProfile *profS = NULL;
    char *ipFrom = NULL, *ipTo = NULL;
    char ip[16];
    MibProfileType mibTypeNT = MIB_PROF_NOTICE;
    NoticeProfile *profNT = NULL;

    memset(ip, 0, sizeof(ip));
    getIfIp(getLanIfName(), ip);

#if 0
    doSystem("cron-init.sh del \"ipset -F %s\"", POE_NT_R);/*删除清记录组的任务*/
#endif
    doSystem("cron-init.sh del \"ipset -F %s\"", POE_NT_OK);/*清空定时请ok组的任务*/
    /*cap链用于匹配需通告数据包*/
#if 0
    /* 初始化mangle PREROUTING中跳转poe_nt_cap_chain规则在noticeInit中实现 */
    /*清除规则*/
    poeNtCapChainAOrD(ip, Iptables_DEL);
#endif
    /*清空链表内容*/
    doSystem("iptables -t mangle -F %s 1>/dev/null 2>&1", POE_NT_CAP_CHAIN);
#if 0
    /* no need */
    /*删除链*/
    doSystem("iptables -t mangle -X %s  1>/dev/null 2>&1" ,POE_NT_CAP_CHAIN);
#endif

    /*reply链表。用于修改http回包*/
#if 0
    /*清除规则*/
    doSystem("iptables -t mangle -D POSTROUTING -j %s 1>/dev/null 2>&1", POE_NT_R_CHAIN);
#endif
    /*清空链表内容*/
    doSystem("iptables -t mangle -F %s 1>/dev/null 2>&1", POE_NT_R_CHAIN);
#if 0
    /*删除链*/
    doSystem("iptables -t mangle -X %s  1>/dev/null 2>&1" ,POE_NT_R_CHAIN);
#endif
   
 

    /*再添加ipset组*/
    /*此部分必须在iptables规则清除的情况下进行。否则被引用的ipset组无法删除*/
    ipsetDelGrp(POE_EXPIRING_ACC, TRUE);/*将过期账号组*/
    ipsetDelGrp(POE_NT_OK, TRUE);/*已通告用户*/
    ipsetDelGrp(POE_NT_R, TRUE);/*record组。用于记录http回包，需改包的数据包*/

    poeAccGrpAdd(POE_EXPIRING_ACC);/*添加过期账户组*/
    poeAccGrpAdd(POE_NT_OK);/*添加已通告账户组*/
    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(MIB_PROF_PPPOE_SRV_GLO, 0);
    if(ProfInstIsFree(profS) == 0)
    {
	if(profS->head.active == 1)
	{/*pppoe服务器开启*/
	    getPoePoolFT(profS, &ipFrom, &ipTo);
	}
	else
	{/*未启用pppoe服务器则随便找一个ip作为组的范围。这样做的目的是为了 使引用该组的规则不用变化
	  *从而增强代码独立性*/
	    ipFrom = ip;
	    ipTo = ip;
	}

	noticeRGrpAdd(POE_NT_R, ipFrom, ipTo);/*record组*/
    }

    /*增加通告规则链*/
    doSystem("iptables -t mangle -N %s >/dev/null 2>&1" , POE_NT_CAP_CHAIN);
#if 0
    /* 初始化mangle PREROUTING中跳转poe_nt_cap_chain规则在noticeInit中实现 */
    /*增加规则链*/
    poeNtCapChainAOrD(ip, Iptables_ADD);
#endif

    /*增加http reply相关规则*/
    doSystem("iptables -t mangle -N %s >/dev/null 2>&1" , POE_NT_R_CHAIN);
#if 0
    doSystem("iptables -t mangle -A POSTROUTING -j %s 1>/dev/null 2>&1", POE_NT_R_CHAIN);
#endif
    /*先判断是否将过期用户，因该规则能过滤掉大部分用户*/

    /*每天0点0分清ok组，即过期通告频率为1天1次*/
    doSystem("cron-init.sh add \"00 00 * * * ipset -F %s\"", POE_NT_OK);
#if 0
    doSystem("cron-init.sh add \"00 00 * * * ipset -F %s\"", POE_NT_R);
#endif
    profNT = (NoticeProfile*)ProfGetInstPointByIndex(mibTypeNT, 1);
    /* 根据开关决定是否建规则 */
    if ((profNT) && (profNT->head.active == TRUE)) {
#if (CACHE_SERVER==FYES)
	doSystem("iptables -t mangle -D POSTROUTING -p tcp -m set --set %s src -m set --set %s dst -m set ! --set %s dst -j %s 1>/dev/null 2>&1",SYS_HTTP_PORT, POE_EXPIRING_ACC, POE_NT_OK, POE_NT_R_CHAIN);
	doSystem("iptables -t mangle -I POSTROUTING -p tcp -m set --set %s src -m set --set %s dst -m set ! --set %s dst -j %s",SYS_HTTP_PORT, POE_EXPIRING_ACC, POE_NT_OK, POE_NT_R_CHAIN);
#else
	doSystem("iptables -t mangle -D POSTROUTING -p tcp --sport 80 -m set --set %s dst -m set ! --set %s dst -j %s 1>/dev/null 2>&1", POE_EXPIRING_ACC, POE_NT_OK, POE_NT_R_CHAIN);
	doSystem("iptables -t mangle -I POSTROUTING -p tcp --sport 80 -m set --set %s dst -m set ! --set %s dst -j %s", POE_EXPIRING_ACC, POE_NT_OK, POE_NT_R_CHAIN);
#endif
    }

    return;
}
#endif
/**
 * pppoe 账号过期规则初始化
 *
 */
extern void poeAccExpiredInit(void)
{

    PPPoESrvGloProfile *profS = NULL;
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;

#if (NOTICE == FYES)
    char ip[16];
    MibProfileType mibTypeNT = MIB_PROF_NOTICE;
    NoticeProfile *profNT = NULL;

    memset(ip, 0, sizeof(ip));
    getIfIp(getLanIfName(), ip);

    /*删除计费丢包链表*/
    doSystem("iptables -t mangle -F %s 1>/dev/null 2>&1", POE_DROP_CHAIN);
    doSystem("iptables -t mangle -X %s 1>/dev/null 2>&1", POE_DROP_CHAIN);

    /*再添加ipset组*/
    /*此部分必须在iptables规则清除的情况下进行。否则被引用的ipset组无法删除*/
#endif
    ipsetDelGrp(POE_EXPIRED_ACC, TRUE);

    poeAccGrpAdd(POE_EXPIRED_ACC);/*添加过期账户组*/
#if (NOTICE == FYES)
    /*增加通告规则链*/
    doSystem("iptables -t mangle -F %s 1>/dev/null 2>&1", POE_NT_DROP_R_CHAIN);
    doSystem("iptables -t mangle -N %s >/dev/null 2>&1" , POE_NT_DROP_R_CHAIN);
    
    /*过期账户丢包处理*/
    doSystem("iptables -t mangle -N %s", POE_DROP_CHAIN);
    profNT = (NoticeProfile*)ProfGetInstPointByIndex(mibTypeNT, 1);
    /* 根据开关建规则 */
    if ((profNT) && (profNT->head.active == TRUE)) {
#if(CACHE_SERVER==FYES)
	doSystem("iptables -t mangle -D POSTROUTING -p tcp -m set --set %s src -m set --set %s dst -j %s 1>/dev/null 2>&1", SYS_HTTP_PORT,POE_EXPIRED_ACC, POE_NT_DROP_R_CHAIN);
	doSystem("iptables -t mangle -I POSTROUTING -p tcp -m set --set %s src -m set --set %s dst -j %s",SYS_HTTP_PORT, POE_EXPIRED_ACC, POE_NT_DROP_R_CHAIN);
#else
	doSystem("iptables -t mangle -D POSTROUTING -p tcp --sport 80 -m set --set %s dst -j %s 1>/dev/null 2>&1", POE_EXPIRED_ACC, POE_NT_DROP_R_CHAIN);
	doSystem("iptables -t mangle -I POSTROUTING -p tcp --sport 80 -m set --set %s dst -j %s", POE_EXPIRED_ACC, POE_NT_DROP_R_CHAIN);
#endif
#if 0
	doSystem("iptables -t mangle -D PREROUTING -m set --set %s src -m set ! --set %s dst -j %s 1>/dev/null 2>&1", POE_EXPIRED_ACC, SYS_LAN_GROUP, POE_DROP_CHAIN);
	doSystem("iptables -t mangle -I PREROUTING -m set --set %s src -m set ! --set %s dst -j %s 1>/dev/null 2>&1", POE_EXPIRED_ACC, SYS_LAN_GROUP, POE_DROP_CHAIN);
#endif
    }
#endif
    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS, 0);

    if ((ProfInstIsFree(profS) == 0) && (profS->head.active == TRUE)) {
	doSystem("iptables -t mangle -D PREROUTING -m set --set %s src -m set ! --set %s dst -j %s 1>/dev/null 2>&1", POE_EXPIRED_ACC, SYS_LAN_GROUP, POE_DROP_CHAIN);
	doSystem("iptables -t mangle -I PREROUTING -m set --set %s src -m set ! --set %s dst -j %s 1>/dev/null 2>&1", POE_EXPIRED_ACC, SYS_LAN_GROUP, POE_DROP_CHAIN);
    }

    /*加到丢包链中*/
    doSystem("iptables -t mangle -A %s -j DROP", POE_DROP_CHAIN);/*默认情况下丢弃所有的包*/
    return;
}
#endif

/*
 * 更新pppoe账号记录map
 */
static void poeUpdatePortMap(void) {
    MibProfileType mibTypeA = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *profA = NULL;
    int i = 0, min = 0, max = 0;
    Uint16 webInstIndex;

    ProfInstLowHigh(mibTypeA, &max, &min);
    for (i = min; i < max; i++) {
        profA = (PPPoESrvAccProfile *)ProfGetInstPointByIndex(mibTypeA, i);
	if ((ProfInstIsFree(profA) == 0) && (strncmp(profA->head.name, "ID", 2) == 0)) {
	    webInstIndex = (Uint16)strtol(&(profA->head.name[2]), NULL, 10);
#if 0
	    printf("%s:webInstIndex = %d\n", __FUNCTION__, webInstIndex);
#endif
	    if (webInstIndex != 0) {
		setPortMap(&(nvramProfile->pppoeInstRec.instRecMap), webInstIndex);
	    }
	}
    }

    return;
}
/********************************************************************
 * 功能： 将会话数组初始化
 * 创建： 2012-08-22
 * 参数： 无
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
extern void poeNProfShmInit(void)
{
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *prof = NULL;

    int i = 0;

    prof = (PPPoESrvGloProfile*)ProfGetInstPointByIndex(mibTypeS, 0);
    if(prof != NULL)
    {
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
    for(i = 0; i < MAX_PPPOE_SRV_ACC_PROFILES; i++)
#else
	for(i = 0; i < MAX_PPPOE_SRV_SESSION_PROFILES; i++)
#endif
	{
	    nvramProfile->Session[i].ConTime = 1U;
	    nvramProfile->Session[i].nextIndex = -1;
	    nvramProfile->Session[i].accIndex = -1;
	}
    }
    return;
}

/*
 * 初始化函数
 */
extern void funInitPppoeServer(void) {
    registerForProfileUpdates(MIB_PROF_PPPOE_SRV_GLO, NULL, pppoeSrvGloProfileChange);
    registerForProfileUpdates(MIB_PROF_PPPOE_SRV_ACC, NULL, pppoeSrvAccProfileChange);
#if 0
    /* 只允许pppoe上网filter表链初始化应该在加规则之前, mantis 9547 */
    iptablesPppoeGloInit();
#endif
#if (PPPOE_LIMIT == FYES)
    iptablesPppoeAccInit();
    ipGrpUserAddSet(POE_SKIP_FIXED_LIMIT);/*跳过精细化限速用户组*/
    pppoeServerInit();
#endif
    /* 初始化记录map */
    poeUpdatePortMap();
#if (PPPOE_SELF_HELP == FYES)
    doSystem("cron-init.sh del \"uttclearOptCnt pppoe\";cron-init.sh add \"00 01 * * * uttclearOptCnt pppoe\"");
#endif
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
#if (PPPOE_LIMIT == FYES)
    PPPoESrvGloProfile *profServer = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(MIB_PROF_PPPOE_SRV_GLO,0);
    if (profServer->head.active) {
	sendMsgToKernel(profChangeNfNlSock, MIB_PROF_PPPOE_SRV_GLO,0, (Pointer)NULL,0,PROF_CHANGE_SPECIAL);
    }
#endif
#endif
    return;
}
#endif
