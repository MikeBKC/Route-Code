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
 * ��fromIP,toIp����ʽ��ȡpppoe��ַ��
 * return:��̬�ֲ�����
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
 * ����lan�ڵ�ַ�仯���б仯��iptables����
 */
static void iptablesOnPoePool(PPPoESrvGloProfile *oldProf, PPPoESrvGloProfile *newProf)
{
    char *ipFrom = NULL, *ipTo = NULL;
#if (PPPOE_ACCOUNT == FYES)
    char ip[16];
    if(oldProf->head.active != newProf->head.active)
    {
	memset(ip, 0, sizeof(ip));
	/* ���lan��IP */
	getIfIp(getLanIfName(), ip);
	/*�����˺Ŷ���Ҫ�õ�pre_drop*/
	initRuleToPreDrop(ip, ip);
    }
#endif
#if (NOTICE == FYES)
    if(newProf->head.active == TRUE)
    {
	getPoePoolFT(newProf, &ipFrom, &ipTo);
#if (PPPOE_ACCOUNT == FYES)
	noticeRGrpEdit(POE_NT_R, ipFrom, ipTo);/*�޸�ͨ���¼��ķ�Χ*/
#endif

    }
#endif
    return;
}

#if 0
/*
 * ��ȡָ���˺�IP��ַ
 * ������user  PPPOE�˺�
 * ����ֵ��char*  PPPOE�˺�USER��Ӧ��IP��ַ
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
 * ������ getPppoeIpAddress 
 * ���ܣ� ��ȡָ���˺�IP��ַ
 * ������ 2012-03-28
 * ������ user  PPPOE�˺�
 * ���أ� char*  PPPOE�˺�USER��Ӧ��IP��ַ
 * ����� ��
 * ���ߣ� Jianqing.Sun 
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
     *�˴�Ϊ�����ź��������˲���
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
     *�˴�Ϊ���źŽ���
     */
    uttSemV(SEM_PPPOE_NO,0);

    return NULL;
}
#endif


/*
 * ����pppoe�˺����������ļ�
 * ͬʱ���ɰ�pppoe server��IP�б��ṩ��pppoe server����IPʹ��
 */
void creatPppoeUserFile(void)
{
    MibProfileType mibTypeA = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *profA = NULL;
    int i = 0, min = 0, max = 0;
    FILE *fp;
    struct in_addr addr;

#if 0
    /* pppoe�˺����٣���PPPOE����֮ǰ ���֮ǰ��PPPOE���ٹ���*/

    doSystem("iptables -t mangle -F %s",PPPOEACC_LIMIT_CHAIN);
#endif

#if 0
    char *userList;
    char user[80], pwd[80], aspip[80];
    userList = nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_USER);
#endif
    /* ɾ������pppoe server�˺� */
    doSystem("sed -i \"/\\t%s\\t/d\" %s", PPPOE_SRV_NAME, PPPOE_SERVER_USER_PAP_FILE);
    doSystem("sed -i \"/\\t%s\\t/d\" %s", PPPOE_SRV_NAME, PPPOE_SERVER_USER_CHAP_FILE);
    /* ���pppoe��IP��¼�ļ� */
    doSystem("cp /dev/null %s -f", PPPOE_SERVER_BIND_IP_FILE);

    fp = fopen(PPPOE_SERVER_USER_ORI_FILE, "w+");
    if(fp != NULL) {
	/*
	 * ���˺�,����,�̶�IP����������ļ�
	 */
	ProfInstLowHigh(mibTypeA, &max, &min);

	/*
	 *�˴�Ϊ�����ź��������˲���
	 */
	uttSemP(SEM_PPPOE_NO,0);

        for (i = min; i < max; i++) {
	    profA = (PPPoESrvAccProfile *)ProfGetInstPointByIndex(mibTypeA, i);
	    /* mantis 8174: �����˺���Ч */
	    if((profA != NULL) && (ProfInstIsFree(profA) == 0) && (profA->head.active == 1)) {
		addr.s_addr = profA->bindIp;
		if (profA->bindIp == 0) {
		    fprintf(fp, "%s\t%s\t%s\t%s\n", profA->pppoename, PPPOE_SRV_NAME, profA->passwd, "*");
		} else {
		    fprintf(fp, "%s\t%s\t%s\t%s\n", profA->pppoename, PPPOE_SRV_NAME, profA->passwd, inet_ntoa(addr));
		    /* ���̶�IP�����ָ���ļ� */
		    doSystem("echo %s >> %s &", inet_ntoa(addr), PPPOE_SERVER_BIND_IP_FILE);
		}
	    }
	}

	/*
	 *�˴�Ϊ���źŽ���
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
 * ɱ������pppoe server���Ž���
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
	    /* mantis 8164:ȫ��ɾ��ʱɾ��·����pppoe�������� */
	    /* STimeΪ0���������ϵģ�ɱ���� */
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
 * ������ killallPppoeClient 
 * ���ܣ� ɱ������pppoe server���Ž���
 * ������ 2012-03-28
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
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
     *�˴�Ϊ�����ź��������˲���
     */
    uttSemP(SEM_PPPOE_NO,0);

    for(i = min;i < max;i++)
    {
	info = (PPPoESrvAccProfile*)ProfGetInstPointByIndex(mibType, i);
	if(info != NULL)
	{
	    /* mantis 8164:ȫ��ɾ��ʱɾ��·����pppoe�������� */
	    /* STimeΪ0���������ϵģ�ɱ���� */
	    if ((strcmp(lanDev, info->device) == 0) && (info->ConTime == 0)) 
	    {
                doSystem("kill -INT %d", info->pid);
	    }
        }
    }

    /*
     *�˴�Ϊ���źŽ���
     */
    uttSemV(SEM_PPPOE_NO,0);

    return;
}
#else
/********************************************************************
 * ������ killallPppoeClient 
 * ���ܣ� ɱ������pppoe server���Ž���
 * ������ 2012-08-28
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static void killallPppoeClient(void)
{
    int i = 0;

    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *prof = NULL;

    MibProfileType mibType = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *info = NULL;

    /*
     *�˴�Ϊ�����ź��������˲���
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
		    {/*�����˼Ʒ�*/
			upPppAccRule(nvramProfile->Session[i], info->expired, 0);/*ɾ��ԭ���Ĺ���*/
		    }
#endif
#ifdef CONFIG_UTT_NETFILTER
		    uttNfNlHashDelIpStr(spdNfNlSock, nvramProfile->Session[i].ip);/*ɾ��hash���Ӧ��ip*/
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
     *�˴�Ϊ���źŽ���
     */
    uttSemV(SEM_PPPOE_NO,0);

    return;
}
#endif
/*
 * ��ʼ��pppoe server
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
*  ɾ��ԭ����ǿ��PPPOE����
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
	 *  ��ʼ��ǿ��PPPOE
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
 * pppoe serve �˺�profile change����
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
	     * ɱ����pppoe server�ͻ��˲���
	     * û�зŵ�initPppoeServer�����У�������Ŀ���ڣ�С�Ķ�
	     */
	    killallPppoeClient();
#else
	    /*��������pppdɱ����дprofile�ǰ�ip���ģ��������ɱ�����������̲��������дprofile�е����߲�����������Ҫִ�����������˺����߲���*/
	    killallPppoeClient();
#endif

	    /* ����ɱ��pppoe-server,ɱ���ֲ�������pppd���� */
	    doSystem("killall -q pppoe-server");

	    initPppoeServer();
	    PPPoESrvGloProfile *newProf = (PPPoESrvGloProfile *)newProfileDataPtr;
	    PPPoESrvGloProfile *oldProf = (PPPoESrvGloProfile *)oldProfileDataPtr;
	    /* �����ַ�ر仯,���·���ǽIP���˲��� */
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
		    /* ip ���� uk���ϲ���һ���� , mantis 8519 */
		    doSystem("iptables -F %s 1>/dev/null 2>&1", ACL_FILTER_CHAIN);
		    iptablesFilterRun();
		}
#endif
	    }

	    /* pppoe srv�������ر� */
	    if ((oldProf->head.active == TRUE) && (newProf->head.active == FALSE)) {
		doSystem("iptables -t filter -D FORWARD  -j %s 1>/dev/null 2>&1", ONLYPPPOE_FORWARD);
		doSystem("iptables -t mangle -D PREROUTING -m set --set %s src -m set ! --set %s dst -j %s 1>/dev/null 2>&1", POE_EXPIRED_ACC, SYS_LAN_GROUP, POE_DROP_CHAIN);
		pppoeAccLimitDelAll();
	    } else if (newProf->head.active == TRUE) { /* poe srv����״̬ */
		if (oldProf->head.active == FALSE) {/*ԭ��pppoe srv�ǹرյ�*/
		    doSystem("iptables -t mangle -I PREROUTING -m set --set %s src -m set ! --set %s dst -j %s 1>/dev/null 2>&1", POE_EXPIRED_ACC, SYS_LAN_GROUP, POE_DROP_CHAIN);
		    pppAccLimIptRuleInit();
		}
		if ((oldProf->onlyPppoe == FALSE) && (newProf->onlyPppoe == TRUE)) { /* ֻ����pppoe �ر�->���� */
		    doSystem("iptables -t filter -D FORWARD  -j %s 1>/dev/null 2>&1", ONLYPPPOE_FORWARD);
		    doSystem("iptables -t filter -I FORWARD  -j %s 1>/dev/null 2>&1", ONLYPPPOE_FORWARD);
		} else if ((oldProf->onlyPppoe == TRUE) && (newProf->onlyPppoe == FALSE)) { /* ֻ����pppoe ����->�ر� */
		    doSystem("iptables -t filter -D FORWARD  -j %s 1>/dev/null 2>&1", ONLYPPPOE_FORWARD);
		} else if ((oldProf->head.active == FALSE) && (newProf->onlyPppoe == TRUE)) { /* �������ر�->����,��ֻ����pppoe���� */
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
 * ���ܣ�pppoe�˺����ٳ�ʼ��,��������
 */
static void  iptablesPppoeAccInit(void)
{
    doSystem("iptables -t mangle -D FORWARD -j %s 1>/dev/null 2>&1", PPPOEACC_LIMIT_CHAIN);
    doSystem("iptables -t mangle -F %s  1>/dev/null 2>&1; iptables -t mangle -X %s  1>/dev/null 2>&1", PPPOEACC_LIMIT_CHAIN, PPPOEACC_LIMIT_CHAIN);
    doSystem("iptables -t mangle -N %s 1>/dev/null 2>&1", PPPOEACC_LIMIT_CHAIN);
}

/*
 * ���ܣ�������ɾ����ת��pppoe�˺��������Ĺ���
 * ����: action - ɾ�����½�
 */
static void utt_init_PPPOEACC_LIMIT_CHAIN(char action)
{
    static char pppAccLimitCount = 0;/*PPPoE���ٹ�������*/
    char pppAccLimitCountBak = pppAccLimitCount;/*����֮ǰ��*/

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
	if (pppAccLimitCount > 0)/*��û�е��й���*/
	{
	    doSystem("iptables -t mangle -A FORWARD -j %s 1>/dev/null 2>&1", PPPOEACC_LIMIT_CHAIN);
	}
    }
    else
    {
	if (pppAccLimitCount <= 0)/*���й��򶼱�ɾ��*/
	{
	    doSystem("iptables -t mangle -D FORWARD -j %s 1>/dev/null 2>&1", PPPOEACC_LIMIT_CHAIN);/*ɾ����ת����*/
	}
    }

    return;
}

#ifndef CONFIG_PERFORMANCE_OPTIMIZATION
#define IF_DIR_LEN  32
/*
 * ���ܣ������ϴ��������ʽ�����ɾ�����ٹ���
 * ���룺pppAccLimit - �����ϴ��������ʵĽṹ��
 *	 action - �½���ɾ��
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

    /*����ϴ��Լ���������*/
    for(i = 0 ;i < 2; i++)
    {
	memset(ipstr, 0, sizeof(ipstr));
	memset(if_dir, 0, IF_DIR_LEN + 1);
	if( i == 0)
	{/*�ϴ�����*/
	    sprintf(ipstr, " -m set --set %s src ", pppoeAccLimit->ipGrpName);
	    strcpy(limittype, "src");/*���Դip��������*/
	    rateTmp = pppoeAccLimit->uRateLimit;
	    strncpy(if_dir, " -m uttdev --lan-to-wan ", IF_DIR_LEN);
	}
	else/*( i == 0)*/
	{/*��������*/
	    sprintf(ipstr, " -m set --set %s dst ", pppoeAccLimit->ipGrpName);
	    strcpy(limittype, "dst");/*���Ŀ��ip��������*/
	    rateTmp = pppoeAccLimit->dRateLimit;
	    strncpy(if_dir, " -m uttdev --wan-to-lan ", IF_DIR_LEN);
	}/*end -- ( i == 0)*/
#if 0
	timestr = getTimeForIptables("All", 0);/*iptablesʱ�����*/
	daystr = getDayForIptables(127u);/*iptables�����*/
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
	    /* ����Ѿ����٣����ٶȴﵽ����ֵ�����ݰ����� -j DROP */
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
 * ����pppoe���ٹ���
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
	/*����pppoe�˻��ṹ�壬��ÿ���ṹ�崴������*/
	for(j = min; j < max; j++) 
	{
	    profA = (PPPoESrvAccProfile*)ProfGetInstPointByIndex(mibTypeA, j);
	    if(ProfInstIsFree(profA) == 0)
	    {
		/*����Ϊ0����Ҫ����*/
		if ((profA->uRateLimit != 0U) || (profA->dRateLimit != 0U))
		{
		    pppoeAccLimitAdd(profA, profS);
		}
	    }
	}
    }
}

/*
 * ���ܣ�����ʱ��ʼ����¼������Ϣ����͸������ó�ʼ������
 */
static int pppoeServerInit(void)
{
    int ret = 0;
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;
    unsigned int i = 0U; 
    char buffer[IP_GROUP_NAME_LEN + 1u];

    uttSemP(SEM_PPPOE_NO,0);/*���ź�������ֹ�������̷���*/
    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS, 0); 

    if (ProfInstIsFree(profS) == 0)
    {
	/*ѭ��Ϊ��¼���ʵ����鸳�����������ڽ����û���ʱʹ��*/
	for (i=0U;i<MAX_PPPOE_LIMIT_IPGRP;i++)
	{
	    sprintf(buffer, "sys_pppoe_limit_ipgrp_%u", i);/*����i������ͬ������*/
	    strncpy(profS->limitGroup[i].ipGrpName, buffer, IP_GROUP_NAME_LEN);
	    PPPOE_SRV_PRINTF("%s--%d:profS->limitGroup[i].ipGrpName=%s\n", __func__, __LINE__, profS->limitGroup[i].ipGrpName);
	}
    }
    pppAccLimIptRuleInit();/*��������*/
    uttSemV(SEM_PPPOE_NO,0);/*�ͷ��ź���*/
    return ret;
}

/*
 * ���ܣ����һ��pppoe�˺��е��ϴ��������ʵ�����������Ϣ�������У���������Ҫ�����û���͹���
 * ���룺prof -- pppoe�˺Žṹ�壬����һ���ϴ���������
 *	 profS -- ������¼������Ϣ������
 */
static int pppoeAccLimitAdd(PPPoESrvAccProfile *prof, PPPoESrvGloProfile *profS)
{
    unsigned int i = 0U;
    int ret = 0;
    unsigned int index = MAX_PPPOE_LIMIT_IPGRP;/*��¼��һ��δʹ������±�*/

    for (i=0U;i<MAX_PPPOE_LIMIT_IPGRP;i++)
    {
	PPPOE_SRV_PRINTF("%s--%d:limitGroup[%d].uRateLimit=%d, prof->uRateLimit=%d, limitGroup[%d].dRateLimit=%d, prof->dRateLimit=%d\n", __func__, __LINE__, i, profS->limitGroup[i].uRateLimit, prof->uRateLimit, i, profS->limitGroup[i].dRateLimit, prof->dRateLimit);
	if ((profS->limitGroup[i].uRateLimit == prof->uRateLimit) && (profS->limitGroup[i].dRateLimit == prof->dRateLimit))/*������ͬ���ʵ���͹���ֱ�����*/
	{
	    PPPOE_SRV_PRINTF("%s--%d:limitGroup[%d].uRateLimit=%d, prof->uRateLimit=%d, limitGroup[%d].dRateLimit=%d, prof->dRateLimit=%d\n", __func__, __LINE__, i, profS->limitGroup[i].uRateLimit, prof->uRateLimit, i, profS->limitGroup[i].dRateLimit, prof->dRateLimit);
	    profS->limitGroup[i].count += 1;
	    /*�����ǰIP���ߣ��������뵽��Ӧ����*/
	    AccAddIpStr(prof, profS->limitGroup[i].ipGrpName, IPSET_MEM_ADD);
	    AccAddIpStr(prof, POE_SKIP_FIXED_LIMIT, IPSET_MEM_ADD);
	    break;
	}/*end -- ((profS->limitGroup[i].uRateLimit == prof->uRateLimit) && (profS->limitGroup[i].dRateLimit == prof->dRateLimit))*/
	/*��¼��һ���յ�λ��*/
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
	/*�����û���*/
	PPPOE_SRV_PRINTF("%s--%d:ipGrpName=%s\n", __func__, __LINE__, profS->limitGroup[index].ipGrpName);
	ipGrpUserAddSet(profS->limitGroup[index].ipGrpName);
	/*�����ǰIP���ߣ��������뵽��Ӧ����*/
	AccAddIpStr(prof, profS->limitGroup[index].ipGrpName, IPSET_MEM_ADD);
	AccAddIpStr(prof, POE_SKIP_FIXED_LIMIT, IPSET_MEM_ADD);
#ifndef CONFIG_PERFORMANCE_OPTIMIZATION
	/*��������*/
	EditOnePPPOELimitGroup(&(profS->limitGroup[index]), 'A');
#endif
    }

    return ret;
}

/*
 * ���ܣ�����pppoe�˺Žṹ���е����ʸı����������ü��������ݼ���ɾ���û���͹���
 * ���룺prof -- pppoe�˺Žṹ��
 *	 profS -- �����м�¼������Ϣ����
 */
static int pppoeAccLimitDel(PPPoESrvAccProfile *prof, PPPoESrvGloProfile *profS)
{
    PPPOE_SRV_PRINTF("%s--%d:pppoeAccLimitDel start\n", __func__, __LINE__);
    int ret = 0;
    unsigned int i = 0U;

    for (i=0U;i<MAX_PPPOE_LIMIT_IPGRP;i++)
    {
	if ((profS->limitGroup[i].uRateLimit == prof->uRateLimit) && (profS->limitGroup[i].dRateLimit == prof->dRateLimit))/*������ͬ���ʵ���͹���ֱ�ӽ���������1*/
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
	/*ɾ������*/
	EditOnePPPOELimitGroup(&(profS->limitGroup[i]), 'D');
#endif
	PPPOE_SRV_PRINTF("%s--%d:ipGrpName=%s\n", __func__, __LINE__, profS->limitGroup[i].ipGrpName);
	/*ɾ����*/
	ipsetDelGrp(profS->limitGroup[i].ipGrpName, TRUE);
	/*����¼��Ϣ������Ա��´μ���������*/
	profS->limitGroup[i].uRateLimit = 0U;
	profS->limitGroup[i].dRateLimit = 0U;
    }/*end -- (profS->limitGroup[i].count <= 0)*/
    return ret;
}

/*
 * ���ܣ�ɾ������pppoe�˻������õ��Ĺ�����û��飬����¼���ʵ������ʼ��
 */
static int pppoeAccLimitDelAll(void)
{
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;
    int ret = 0;/*����ֵ*/
    unsigned int i = 0U;

    uttSemP(SEM_PPPOE_NO,0);/*���ź�����ֹ��ͻ*/
    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS, 0);
    if (ProfInstIsFree(profS) == 0)
    {
	/*���������*/
	doSystem("iptables -t mangle -F %s 1>/dev/null 2>&1",PPPOEACC_LIMIT_CHAIN);
	/*�����ת����*/
	utt_init_PPPOEACC_LIMIT_CHAIN(Iptables_FLUSH);
	/*����¼������Ϣ�����ʼ��*/
	for (i=0U;i<MAX_PPPOE_LIMIT_IPGRP;i++)
	{
	    /*ɾ����*/
	    ipsetDelGrp(profS->limitGroup[i].ipGrpName, TRUE);
	    profS->limitGroup[i].uRateLimit = 0U;/*�ϴ�*/
	    profS->limitGroup[i].dRateLimit = 0U;/*����*/
	    profS->limitGroup[i].count = 0;/*���ü���*/
	}
    }
#if (GROUP_BASE == FYES)
    utt_init_GROUP_LIMIT_CHAIN(PPPOE_ACC_LIMIT, Iptables_FLUSH);
#endif
    ipsetFlushGrp(POE_SKIP_FIXED_LIMIT);/*mantis:11302,���������ϸ��������*/
    uttSemV(SEM_PPPOE_NO,0);/*�������������Ӧ���ͷŸ���*/
    return ret;
}

/*
 * ���ܣ���prof���м�飬������ӻ�ɾ������
 * ���룺prof -- pppoe�˻��ṹ��
 *	 changeType -- ��ӻ�ɾ����־
 * ���أ�0--�ɹ�
 */
static int pppoeAccLimitChange(PPPoESrvAccProfile *prof, ProfChangeType changeType)
{
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;
    int ret = 0;/*����ֵ*/

    uttSemP(SEM_PPPOE_NO,0);/*���ź�������ֹ�������̷���*/
    /*û���������ʣ�����Ҫ����*/
    if ((prof->uRateLimit != 0U) || (prof->dRateLimit != 0U))
    {
	PPPOE_SRV_PRINTF("%s--%d:uRateLimit=%d, dRateLimit=%d\n", __func__, __LINE__, prof->uRateLimit, prof->dRateLimit);
	profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS, 0);
	if ((ProfInstIsFree(profS) == 0) && (profS->head.active == TRUE))
	{
	    if (changeType == PROF_CHANGE_ADD)
	    {
		PPPOE_SRV_PRINTF("%s--%d:changeType = PROF_CHANGE_ADD\n", __func__, __LINE__);
		pppoeAccLimitAdd(prof, profS);/*���*/
	    }
	    else/*PROF_CHANGE_DEL*/
	    {
		pppoeAccLimitDel(prof, profS);/*ɾ��*/
	    }
	}
    }/*end -- if ((prof->uRateLimit != 0) || (prof->dRateLimit != 0))*/

    uttSemV(SEM_PPPOE_NO,0);/*�ͷ��ź���*/
    return ret;
}
#endif


/*
 * pppoe serve �˺�profile change����
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
	    /* Ŀ����Ϊ��ֻ����һ��createPppoeUserFile */
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
     * ��ʼ��ǿ��PPPoE ����
     */
   doSystem("iptables -t filter -D FORWARD -j %s 1>/dev/null 2>&1", ONLYPPPOE_FORWARD);
    doSystem("iptables -t filter -F %s  1>/dev/null 2>&1; iptables -t filter -X %s  1>/dev/null 2>&1", ONLYPPPOE_FORWARD, ONLYPPPOE_FORWARD);
    doSystem("iptables -t filter -N %s 1>/dev/null 2>&1; iptables -t filter -I FORWARD  -j %s 1>/dev/null 2>&1", ONLYPPPOE_FORWARD, ONLYPPPOE_FORWARD);

}
#endif



#if (PPPOE_ACCOUNT == FYES)
#if (NOTICE == FYES)
#if (MULTI_LAN == FYES)
/*���ӻ�ɾ������ͨ���������iptables����
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
/*���ӻ�ɾ������ͨ���������iptables����
 * ipstr : lan��ip
 * action: Iptables_ADD or Iptables_DEL*/
extern void poeNtCapChainAOrD(char* ipstr, char action)
{
    doSystem("iptables -t mangle -%c PREROUTING -p tcp --dport 80 ! -d %s -j %s",action, ipstr, POE_NT_CAP_CHAIN);
    return;
}
#endif
/**
 * pppoe �˺Ź��ڹ����ʼ��
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
    doSystem("cron-init.sh del \"ipset -F %s\"", POE_NT_R);/*ɾ�����¼�������*/
#endif
    doSystem("cron-init.sh del \"ipset -F %s\"", POE_NT_OK);/*��ն�ʱ��ok�������*/
    /*cap������ƥ����ͨ�����ݰ�*/
#if 0
    /* ��ʼ��mangle PREROUTING����תpoe_nt_cap_chain������noticeInit��ʵ�� */
    /*�������*/
    poeNtCapChainAOrD(ip, Iptables_DEL);
#endif
    /*�����������*/
    doSystem("iptables -t mangle -F %s 1>/dev/null 2>&1", POE_NT_CAP_CHAIN);
#if 0
    /* no need */
    /*ɾ����*/
    doSystem("iptables -t mangle -X %s  1>/dev/null 2>&1" ,POE_NT_CAP_CHAIN);
#endif

    /*reply���������޸�http�ذ�*/
#if 0
    /*�������*/
    doSystem("iptables -t mangle -D POSTROUTING -j %s 1>/dev/null 2>&1", POE_NT_R_CHAIN);
#endif
    /*�����������*/
    doSystem("iptables -t mangle -F %s 1>/dev/null 2>&1", POE_NT_R_CHAIN);
#if 0
    /*ɾ����*/
    doSystem("iptables -t mangle -X %s  1>/dev/null 2>&1" ,POE_NT_R_CHAIN);
#endif
   
 

    /*�����ipset��*/
    /*�˲��ֱ�����iptables�������������½��С��������õ�ipset���޷�ɾ��*/
    ipsetDelGrp(POE_EXPIRING_ACC, TRUE);/*�������˺���*/
    ipsetDelGrp(POE_NT_OK, TRUE);/*��ͨ���û�*/
    ipsetDelGrp(POE_NT_R, TRUE);/*record�顣���ڼ�¼http�ذ�����İ������ݰ�*/

    poeAccGrpAdd(POE_EXPIRING_ACC);/*��ӹ����˻���*/
    poeAccGrpAdd(POE_NT_OK);/*�����ͨ���˻���*/
    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(MIB_PROF_PPPOE_SRV_GLO, 0);
    if(ProfInstIsFree(profS) == 0)
    {
	if(profS->head.active == 1)
	{/*pppoe����������*/
	    getPoePoolFT(profS, &ipFrom, &ipTo);
	}
	else
	{/*δ����pppoe�������������һ��ip��Ϊ��ķ�Χ����������Ŀ����Ϊ�� ʹ���ø���Ĺ����ñ仯
	  *�Ӷ���ǿ���������*/
	    ipFrom = ip;
	    ipTo = ip;
	}

	noticeRGrpAdd(POE_NT_R, ipFrom, ipTo);/*record��*/
    }

    /*����ͨ�������*/
    doSystem("iptables -t mangle -N %s >/dev/null 2>&1" , POE_NT_CAP_CHAIN);
#if 0
    /* ��ʼ��mangle PREROUTING����תpoe_nt_cap_chain������noticeInit��ʵ�� */
    /*���ӹ�����*/
    poeNtCapChainAOrD(ip, Iptables_ADD);
#endif

    /*����http reply��ع���*/
    doSystem("iptables -t mangle -N %s >/dev/null 2>&1" , POE_NT_R_CHAIN);
#if 0
    doSystem("iptables -t mangle -A POSTROUTING -j %s 1>/dev/null 2>&1", POE_NT_R_CHAIN);
#endif
    /*���ж��Ƿ񽫹����û�����ù����ܹ��˵��󲿷��û�*/

    /*ÿ��0��0����ok�飬������ͨ��Ƶ��Ϊ1��1��*/
    doSystem("cron-init.sh add \"00 00 * * * ipset -F %s\"", POE_NT_OK);
#if 0
    doSystem("cron-init.sh add \"00 00 * * * ipset -F %s\"", POE_NT_R);
#endif
    profNT = (NoticeProfile*)ProfGetInstPointByIndex(mibTypeNT, 1);
    /* ���ݿ��ؾ����Ƿ񽨹��� */
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
 * pppoe �˺Ź��ڹ����ʼ��
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

    /*ɾ���ƷѶ�������*/
    doSystem("iptables -t mangle -F %s 1>/dev/null 2>&1", POE_DROP_CHAIN);
    doSystem("iptables -t mangle -X %s 1>/dev/null 2>&1", POE_DROP_CHAIN);

    /*�����ipset��*/
    /*�˲��ֱ�����iptables�������������½��С��������õ�ipset���޷�ɾ��*/
#endif
    ipsetDelGrp(POE_EXPIRED_ACC, TRUE);

    poeAccGrpAdd(POE_EXPIRED_ACC);/*��ӹ����˻���*/
#if (NOTICE == FYES)
    /*����ͨ�������*/
    doSystem("iptables -t mangle -F %s 1>/dev/null 2>&1", POE_NT_DROP_R_CHAIN);
    doSystem("iptables -t mangle -N %s >/dev/null 2>&1" , POE_NT_DROP_R_CHAIN);
    
    /*�����˻���������*/
    doSystem("iptables -t mangle -N %s", POE_DROP_CHAIN);
    profNT = (NoticeProfile*)ProfGetInstPointByIndex(mibTypeNT, 1);
    /* ���ݿ��ؽ����� */
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

    /*�ӵ���������*/
    doSystem("iptables -t mangle -A %s -j DROP", POE_DROP_CHAIN);/*Ĭ������¶������еİ�*/
    return;
}
#endif

/*
 * ����pppoe�˺ż�¼map
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
 * ���ܣ� ���Ự�����ʼ��
 * ������ 2012-08-22
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
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
 * ��ʼ������
 */
extern void funInitPppoeServer(void) {
    registerForProfileUpdates(MIB_PROF_PPPOE_SRV_GLO, NULL, pppoeSrvGloProfileChange);
    registerForProfileUpdates(MIB_PROF_PPPOE_SRV_ACC, NULL, pppoeSrvAccProfileChange);
#if 0
    /* ֻ����pppoe����filter������ʼ��Ӧ���ڼӹ���֮ǰ, mantis 9547 */
    iptablesPppoeGloInit();
#endif
#if (PPPOE_LIMIT == FYES)
    iptablesPppoeAccInit();
    ipGrpUserAddSet(POE_SKIP_FIXED_LIMIT);/*������ϸ�������û���*/
    pppoeServerInit();
#endif
    /* ��ʼ����¼map */
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
