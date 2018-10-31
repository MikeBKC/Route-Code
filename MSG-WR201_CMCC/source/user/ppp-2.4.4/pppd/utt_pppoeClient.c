

/*
 * added by brwang
 */


#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "utt_pppoeClient.h"
#include "utt_conf_define.h"
#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "comExeFun.h"
#include "ifmisc.h"
#include <linux/autoconf.h>

/* pppoe server ip �̶����� */
#include "pppoe-server.h"
#include "utt_ip_assign.h"

extern sigset_t signals_handled;
#ifdef CONFIG_UTT_NETFILTER
extern int uttNfNlSock;
#endif

#ifdef SWORD_DEBUG
#define SWORD_SYSLOG(pri, fmt, args...)		syslog(pri, fmt, ## args)	
#else
#define SWORD_SYSLOG(pri, fmt, args...)
#endif
struct st_uttPppdClient pppoeClientInfo;

#if 0
static void _utt_writeInfoToFile(void);
#endif
#if (PPPOE_SERVER == FYES)
static void _utt_writeInfoToProfile(void);
static int uttPoeOffLine(PPPoESrvAccProfile* info, int i);
static void uttPoeOnLine(PPPoESrvAccProfile* info, int index, int i);
extern int inet_aton (const char *cp, struct in_addr *inaddr);
#endif

struct flock* file_lock(short type, short whence)
{
    static struct flock ret;
    ret.l_type = type ;
    ret.l_start = 0;
    ret.l_whence = whence;
    ret.l_len = 0;
    ret.l_pid = getpid();
    return &ret;
}

#if 0
static int fd = -1;
#endif
void _utt_setevn(char *name, char *value)
{
    SWORD_SYSLOG(LOG_NOTICE, "%s-----%d, name = %s\n",  __func__, __LINE__,name);
    if(strncmp(name, "PEERNAME", 8) == 0) {
        strncpy(pppoeClientInfo.user, value, UTT_PPP_CLIENT_NAME_LEN);
    } else if((strncmp(name, "CONNECT_TIME", 12) == 0) && (strcmp(pppoeClientInfo.ourName, PPTP_SRV_NAME) != 0) 
	    && (strcmp(pppoeClientInfo.ourName, L2TP_SRV_NAME) != 0)) {
        if(pppoeClientInfo.ConTime == 0) 
        {
            pppoeClientInfo.ConTime = strtol(value, NULL, 10);
#if 0
            if(pppoeClientInfo.ConTime == 0)
                pppoeClientInfo.ConTime = 0xffffffff;
#endif
            if(pppoeClientInfo.ConTime != 0){
#if 0
		_utt_writeInfoToFile();
#endif
#if (PPPOE_SERVER == FYES)
		_utt_writeInfoToProfile();
#endif
#if 0
                close(fd);
                fd = -1;
#endif
	    }
        }
    } else if((strncmp(name, "PPPD_PID", 8) == 0) && (strcmp(pppoeClientInfo.ourName, PPTP_SRV_NAME) != 0)
	    && (strcmp(pppoeClientInfo.ourName, L2TP_SRV_NAME) != 0)) {
        pppoeClientInfo.pid = strtol(value, NULL, 10);
#if 0
	_utt_writeInfoToFile();
#if (PPPOE_SERVER == FYES)
	_utt_writeInfoToProfile();
#endif
#endif
    } else if(strncmp(name, "IFNAME", 6) == 0) {
        strncpy(pppoeClientInfo.ifname, value, UTT_PPP_CLIENT_IFNAME_LEN);
    } else if(strncmp(name, "DEVICE", 6) == 0) {
        strncpy(pppoeClientInfo.device, value, UTT_PPP_CLIENT_DEVICE_LEN);
    } else if((strncmp(name, "IPREMOTE", 8) == 0) && (strcmp(pppoeClientInfo.ourName, PPTP_SRV_NAME) != 0)
	    && (strcmp(pppoeClientInfo.ourName, L2TP_SRV_NAME) != 0)) {
        pppoeClientInfo.ConTime = 0;
        strncpy(pppoeClientInfo.ip, value, UTT_PPP_CLIENT_IP_LEN);
#if 0
	_utt_writeInfoToFile();
#endif

#if (PPPOE_SERVER == FYES)
        _utt_writeInfoToProfile();
#endif
    } else if(strncmp(name, "MACREMOTE", 8) == 0) {
        strncpy(pppoeClientInfo.mac, value, UTT_PPP_CLIENT_MAC_LEN);
    } else if(strncmp(name, "START_TIME", 8) == 0) {
	/* pppoeClientInfo��ʼ�� */
        memset(&pppoeClientInfo, 0, sizeof(struct st_uttPppdClient));

        pppoeClientInfo.STime = strtol(value, NULL, 10);
        pppoeClientInfo.LTime = strtol(value, NULL, 10);
    } else if (strncmp(name, "OURNAME", 7) == 0) {
	strncpy(pppoeClientInfo.ourName, value, UTT_PPP_CLIENT_OURNAME_LEN);
    } 
    /*
     * mantis 8436
     */
    else if (strncmp(name, "TERMINATED", 10) == 0) {
	if (strncmp("YES", value, 3) == 0) {
	    if(pppoeClientInfo.ConTime == 0) 
	    {
		pppoeClientInfo.ConTime = 1;
#if 0
		_utt_writeInfoToFile();
#endif

#if (PPPOE_SERVER == FYES)
		_utt_writeInfoToProfile();
#endif
#if 0
	        close(fd);
	        fd = -1;
#endif
	    }
	}
    }
}

#if 0
static void _utt_writeInfoToFile()
{
    static int     st_size;
    off_t   start, end;
    int     fileLen;
    int readNum = 0,freeNum = 0;
    struct st_uttPppdClient info;
    int found = 0;

    if(fd == -1) {
        fd = open(UTT_PPPOE_CLIENT_INFO_FILE, O_RDWR|O_CREAT, 0666);
    }
    st_size = sizeof(struct st_uttPppdClient);
    start = lseek(fd, 0, SEEK_SET);
    end = lseek(fd, 0, SEEK_END);
    fileLen = end - start;
    lseek(fd, 0, SEEK_SET);
    fcntl(fd, F_SETLKW, file_lock(F_WRLCK|F_RDLCK, SEEK_SET));
    while(fileLen >= st_size) {
        read(fd, &info, st_size);
        readNum++;
#if 0
        if(info.ConTime != 0 || info.pid == pppoeClientInfo.pid ||
                (strncmp(info.user, pppoeClientInfo.user, UTT_PPP_CLIENT_NAME_LEN) == 0) ||
                (strncmp(info.ifname, pppoeClientInfo.ifname, UTT_PPP_CLIENT_IFNAME_LEN) == 0)) {
            found = 1;
            break;
        }
#else
        if(info.ConTime != 0) {
            if(freeNum == 0) {
                freeNum = readNum;
            }
        }
        if((info.pid == pppoeClientInfo.pid )
#if 0
                || ((strncmp(info.user, pppoeClientInfo.user, UTT_PPP_CLIENT_NAME_LEN) == 0) &&
                (strncmp(info.ifname, pppoeClientInfo.ifname, UTT_PPP_CLIENT_IFNAME_LEN) == 0))
#endif
                )
        {
            
            found = 1;
            break;
        }
#endif
        fileLen -= st_size;
    }

    if(found == 1) {
#if 0
        lseek(fd, 0-st_size, SEEK_CUR);
#endif
        lseek(fd, (readNum - 1) * st_size, SEEK_SET);
    } else {
        if(freeNum != 0) {
            lseek(fd, (freeNum - 1) * st_size, SEEK_SET);
        }
    }

    write(fd, &pppoeClientInfo, st_size);
    fcntl(fd, F_SETLKW, file_lock(F_UNLCK, SEEK_SET));
    return;
}
#endif

#if 0
#if (PPPOE_SERVER == FYES)
/********************************************************************
 * ������ _utt_writeInfoToProfile 
 * ���ܣ� PPPoE��Ϣ����profile��
 * ������ 2012-03-28
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static void _utt_writeInfoToProfile()
{
    int max = 0;
    int min = 0;
    int i = 0;

    MibProfileType mibType = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *info = NULL;

    /*
     *�˴�Ϊ����profile
     */
    ProfInstLowHigh(mibType, &max, &min);

    /*
     *�˴�Ϊ�����ź��������˲���
     */
    uttSemP(SEM_PPPOE_NO,0);

    for(i = min;i < max;i++)
    {
	info = (PPPoESrvAccProfile*)ProfGetInstPointByIndex(mibType, i);
	if(ProfInstIsFree(info) == 0)
	{
	    if((strcmp(info->pppoename, pppoeClientInfo.user) == 0)||((info->ConTime == 0) && (info->pid == pppoeClientInfo.pid)))
	    { 
		/*
		 * �ҵ�����˺��޸��˺���Ϣ��ֹͣ
		 */
		if(pppoeClientInfo.ConTime != 0)
		{/*����*/
		    if(info->ConTime == 0)
		    {/*״̬�仯*/
#if (IP_GRP == FYES)
			userUpToIpGrp(info->pppoename, info->ip, IPENT_POE_ACC, IPSET_MEM_DEL);
#endif
#if (PPPOE_ACCOUNT == FYES)	
			if(info->chargeType !=  NONE_CHARG_MODE)
			{
			    upPppAccRule(info, info->expired, 0);/*ɾ��ԭ���Ĺ���*/

			}
#endif
#ifdef CONFIG_UTT_NETFILTER
			uttNfNlHashDelIpStr(uttNfNlSock, info->ip);/*ɾ��hash���Ӧ��ip*/
#endif
		    }
		}
		info->pid = pppoeClientInfo.pid;
		info->STime = pppoeClientInfo.STime;
		info->LTime = pppoeClientInfo.LTime;
		info->rxByte = pppoeClientInfo.rxByte;
		info->txByte = pppoeClientInfo.txByte;
		strcpy(info->mac, pppoeClientInfo.mac);
		strcpy(info->ip, pppoeClientInfo.ip);
		strcpy(info->ifname, pppoeClientInfo.ifname);
		strcpy(info->device, pppoeClientInfo.device);
		strcpy(info->ourName, pppoeClientInfo.ourName);

		if(pppoeClientInfo.ConTime == 0)
		{/*����*/
		    if(info->ConTime != 0)
		    {/*״̬�仯*/
#if (IP_GRP == FYES)
			userUpToIpGrp(info->pppoename, info->ip, IPENT_POE_ACC, IPSET_MEM_ADD);
#endif
#if (PPPOE_ACCOUNT == FYES)	
			if(info->chargeType !=  NONE_CHARG_MODE)
			{
			    upPppAccStatus(info);/*ͳ��һ���˻�״̬*/
			    upPppAccRule(info, info->expired, 1);/*��ӹ���*/
			}
#endif
#ifdef CONFIG_UTT_NETFILTER
			/*ɾ��hash���Ӧ��ip
			 * �ں˻��Զ��ؽ���ip����
			 * ����ɾ����Ϊ���¸�ip���������δ�ͷ�*/
			uttNfNlHashDelIpStr(uttNfNlSock, info->ip);
#endif
		    }
		}

		info->ConTime = pppoeClientInfo.ConTime;
		break;
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
#else
#if (PPPOE_SERVER == FYES)
#if (PPPOE_LIMIT == FYES)
static int pppoeAccLimitUpToGrp(PPPoESrvAccProfile *profA, PPPoESrvGloProfile *profS, char *ip, char addOrDel)
{
    int i = 0;

    if ((profA->uRateLimit != 0) || (profA->dRateLimit != 0))
    {
	for (i=0;i<MAX_PPPOE_LIMIT_IPGRP;i++)
	{
	    if ((profS->limitGroup[i].uRateLimit == profA->uRateLimit) && (profS->limitGroup[i].dRateLimit == profA->dRateLimit))
	    {
		ipsetAOrDIpStr(profS->limitGroup[i].ipGrpName, ip, addOrDel);/*������Ӧ��ipset��*/
		ipsetAOrDIpStr(POE_SKIP_FIXED_LIMIT, ip, addOrDel);/*������Ӧ��ipset��*/
	    }
	}
    }
    return 0;
}
#endif

/********************************************************************
 * ������ _utt_writeInfoToProfile 
 * ���ܣ� PPPoE��Ϣ����profile��
 * ������ 2012-08-24
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static void _utt_writeInfoToProfile()
{
    int i = 0, index = -1;
    struct in_addr pppoeIp;

    MibProfileType mibType = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *prof = NULL;

    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *info = NULL;
    char pppoeMac[UTT_PPP_CLIENT_MAC_LEN + 1];

    sigset_t oldsignals_handled;

    SWORD_SYSLOG(LOG_NOTICE, "%s-----%d, pppoeClientInfo.device = \"%s\"\n",  __func__, __LINE__, pppoeClientInfo.device);

    if( strcmp(pppoeClientInfo.device, ( char *)getLanIfName()) != 0)
    {/*��lan�� pppoe����*/
	return;
    }
    /*�����źš���ֹpv�����м䱻signal��ϡ���signal������
     * ��term �����¼����������Լ��Ͱ��Լ�������
     * ������Է���ʹ����Ϊ�ú���ֻ�ı䵱ǰ���ź������֣������ı��źŵ���Ϊ
     * ��pppd�źŵ���ΪΪ sa_flags=0 ���źű�����ʱ���źŻᱻ�Ƴ�ִ��һ�Ρ������᲻ִ��*/
    sigprocmask(SIG_BLOCK, &signals_handled, &oldsignals_handled);
    /*
     *�˴�Ϊ�����ź��������˲���
     */
    uttSemP(SEM_PPPOE_NO,0);

    prof = (PPPoESrvGloProfile*)ProfGetInstPointByIndex(mibType, 0);
    if(prof != NULL)
    {
	if(strlen(pppoeClientInfo.ip) != 0)
	{
	    inet_aton(pppoeClientInfo.ip, &pppoeIp);
	    i = ntohl(pppoeIp.s_addr) - ntohl(prof->ipStart);/*ȡ�ûỰ�����±�*/
	    SWORD_SYSLOG(LOG_NOTICE, "%s-----%d, i = %d\n",  __func__, __LINE__, i);
	    SWORD_SYSLOG(LOG_NOTICE, "%s-----%d, ip = %s, ntohl(pppoeIp.s_addr) = %x, ntohl(prof->ipStart) = %x, ConTime = %d\n",  __func__, __LINE__, pppoeClientInfo.ip, ntohl(pppoeIp.s_addr), ntohl(prof->ipStart), pppoeClientInfo.ConTime);
	    if(SesIndexIsOk(i) == 1)
	    {
		nvramProfile->Session[i].pid = pppoeClientInfo.pid;
		nvramProfile->Session[i].STime = pppoeClientInfo.STime;
		nvramProfile->Session[i].LTime = pppoeClientInfo.LTime;
		nvramProfile->Session[i].rxByte = pppoeClientInfo.rxByte;
		nvramProfile->Session[i].txByte = pppoeClientInfo.txByte;
		strcpy(nvramProfile->Session[i].mac, pppoeClientInfo.mac);
		strcpy(nvramProfile->Session[i].ip, pppoeClientInfo.ip);
		strcpy(nvramProfile->Session[i].ifname, pppoeClientInfo.ifname);
		strcpy(nvramProfile->Session[i].device, pppoeClientInfo.device);
		strcpy(nvramProfile->Session[i].ourName, pppoeClientInfo.ourName);
		SWORD_SYSLOG(LOG_NOTICE, "%s-----%d, pppoeClientInfo.user = %s\n",  __func__, __LINE__, pppoeClientInfo.user);
		index = ProfGetInstPointByValue(mibTypeS, OFFANDSIZE(PPPoESrvAccProfile, pppoename), TSTR, pppoeClientInfo.user, -1); 
		info = (PPPoESrvAccProfile*)ProfGetInstPointByIndex(mibTypeS, index);
		if(ProfInstIsFree(info) == 0)
		{
		    /*
		     * �ҵ�����˺��޸��˺���Ϣ��ֹͣ
		     */
		    if(pppoeClientInfo.ConTime != 0U)
		    {/*����*/
			if(nvramProfile->Session[i].ConTime == 0U)
			{/*״̬�仯*/

#if (IP_GRP == FYES)
			    userUpToIpGrp(info->pppoename, nvramProfile->Session[i].ip, IPENT_POE_ACC, IPSET_MEM_DEL);
#endif
#if (PPPOE_ACCOUNT == FYES)	
			    if(info->chargeType !=  NONE_CHARG_MODE)
			    {
				upPppAccRule(nvramProfile->Session[i], info->expired, 0);/*ɾ��ԭ���Ĺ���*/

			    }
#endif
#if (PPPOE_LIMIT == FYES)
			    pppoeAccLimitUpToGrp(info, prof, nvramProfile->Session[i].ip, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
			    uttNfNlHashDelIpStr(uttNfNlSock, nvramProfile->Session[i].ip);/*ɾ��hash���Ӧ��ip*/
#endif
			    uttPoeOffLine(info, i);
			}
		    }

		    if(pppoeClientInfo.ConTime == 0U)
		    {/*����*/
			SWORD_SYSLOG(LOG_NOTICE, "%s-----%d, ip = %s, ntohl(pppoeIp.s_addr) = %x, ntohl(prof->ipStart) = %x, nvramProfile->Session[i].ConTime = %d, pppoeClientInfo.ConTime = %d\n",  __func__, __LINE__, pppoeClientInfo.ip, ntohl(pppoeIp.s_addr), ntohl(prof->ipStart), nvramProfile->Session[i].ConTime, pppoeClientInfo.ConTime);
			if(nvramProfile->Session[i].ConTime != 0U)
			{/*״̬�仯*/
#if (IP_GRP == FYES)
			    userUpToIpGrp(info->pppoename, nvramProfile->Session[i].ip, IPENT_POE_ACC, IPSET_MEM_ADD);
#endif

#if (PPPOE_LIMIT == FYES)
			    pppoeAccLimitUpToGrp(info, prof, nvramProfile->Session[i].ip, IPSET_MEM_ADD);
#endif
#ifdef CONFIG_UTT_NETFILTER
			    /*ɾ��hash���Ӧ��ip
			     * �ں˻��Զ��ؽ���ip����
			     * ����ɾ����Ϊ���¸�ip���������δ�ͷ�*/
			    uttNfNlHashDelIpStr(uttNfNlSock, nvramProfile->Session[i].ip);
#endif
			    SWORD_SYSLOG(LOG_NOTICE, "%s--------%d, info->actualSession = %d\n", __func__, __LINE__, info->actualSession);
			    uttPoeOnLine(info, index, i);
			    /* edit by guo.deyuan
			     * mantis bug 0012986 ���ųɹ������mac��
			     * */
			    if(strcmp(info->AccMACBind, "autobind") == 0)
			    {
				strcpy(pppoeMac, pppoeClientInfo.mac);
				strRemoveChar(pppoeMac, ':');/*ȥ��MAC��ַ�е�ð��*/
				converMac12To6Byte(pppoeMac, &(info->bindmac));
				nvramWriteCommit(); /*��macд��flash*/
			    }
			}
		    }

		    nvramProfile->Session[i].ConTime = pppoeClientInfo.ConTime;
		    SWORD_SYSLOG(LOG_NOTICE, "%s-----%d, ip = %s, ntohl(pppoeIp.s_addr) = %x, ntohl(prof->ipStart) = %x, nvramProfile->Session[i].ConTime = %d, pppoeClientInfo.ConTime = %d\n",  __func__, __LINE__, pppoeClientInfo.ip, ntohl(pppoeIp.s_addr), ntohl(prof->ipStart), nvramProfile->Session[i].ConTime, pppoeClientInfo.ConTime);
		}
	    }
	}
    }

    /*
     *�˴�Ϊ���źŽ���
     */
    uttSemV(SEM_PPPOE_NO,0);
    /*signal�����ָֻ�*/
    sigprocmask(SIG_SETMASK, &oldsignals_handled, NULL);

    return;
}
/********************************************************************
 * ������ uttPoeOffLine
 * ���ܣ� �˺�����ʱ�Ự�±�仯
 * ������ 2012-08-23
 * ������ info   -    �����˺ŵ�profile
 *         i     -    ���߻Ự�±�
 * ���أ� 0     -      ʧ��
 *        1     -      �ɹ� 
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int uttPoeOffLine(PPPoESrvAccProfile* info, int i)
{
    SWORD_SYSLOG(LOG_NOTICE, "%s--------%d, info->actualSession = %d\n", __func__, __LINE__, info->actualSession);
    int j = 0, m = 0, n = 0, flag = 0;
    j = info->sesIndex;
    if(SesIndexIsOk(j) == 1)
    {/*�ж��±��Ƿ���*/
	nvramProfile->Session[i].accIndex = -1;
	if(j == i)
	{/*��һ������Ҫɾ���ĻỰ*/
	    info->sesIndex = nvramProfile->Session[j].nextIndex;
	    nvramProfile->Session[j].nextIndex = -1;
	    info->actualSession -= 1;
	    flag = 1;
	    SWORD_SYSLOG(LOG_NOTICE, "%s--------%d, info->actualSession = %d\n", __func__, __LINE__, info->actualSession);
	}
	else
	{
	    for(m = 0; m < (info->actualSession - 1); m++)
	    {/*ѭ��ʵ�ʻỰ����һ��*/
		n = j;
		j = nvramProfile->Session[j].nextIndex;
		if(j == i)
		{/*jΪ��ɾ���ĻỰ�±�*/
		    nvramProfile->Session[n].nextIndex = nvramProfile->Session[j].nextIndex;
		    nvramProfile->Session[j].nextIndex = -1;
		    info->actualSession -= 1;
		    flag = 1;
		    SWORD_SYSLOG(LOG_NOTICE, "%s--------%d, info->actualSession = %d\n", __func__, __LINE__, info->actualSession);
		}
		if((SesIndexIsOk(j) == 0) || (flag == 1))
		{/*�ж��±��Ƿ��޻��Ƿ����ҵ���ɾ��*/
		    break;
		}
	    }
	}
    }
    SWORD_SYSLOG(LOG_NOTICE, "%s--------%d, info->actualSession = %d\n", __func__, __LINE__, info->actualSession);
    return flag;
}
/********************************************************************
 * ������ uttPoeOnLine
 * ���ܣ� �˺�����ʱ�Ự�±�仯
 * ������ 2012-08-23
 * ������ info   -    �����˺ŵ�profile
 *        index  -    �����˺ŵ�����
 *         i     -    ���߻Ự�±�
 * ���أ� �� 
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static void uttPoeOnLine(PPPoESrvAccProfile* info, int index, int i)
{
    SWORD_SYSLOG(LOG_NOTICE, "%s--------%d, info->actualSession = %d\n", __func__, __LINE__, info->actualSession);
    nvramProfile->Session[i].nextIndex = info->sesIndex;
    nvramProfile->Session[i].accIndex = index;
    info->sesIndex = i;
#if (PPPOE_ACCOUNT == FYES)	
    if(info->chargeType !=  NONE_CHARG_MODE)
    {
	/*mantis 12732 
	 * ��ֻ�Ǹ��¸�session �� �����˺ŻỰ����Ϊ 1 ���������˺ŵĹ���״̬�޷��ٸ���,��Ϊ�˺�״̬���ٱ仯��*/
	if (pppoeSesCheck(info) == 0)
	{/*��״̬�ޱ仯���򲻻���ӹ��򡣹��������һ��*/
	    upPppAccRule(nvramProfile->Session[i], info->expired, 1);
	}
    }
#endif
#if 0
    if(info->actualSession < 0)
    {
	info->actualSession = 0;
    }
    info->actualSession += 1;
#endif
    SWORD_SYSLOG(LOG_NOTICE, "%s--------%d, info->actualSession = %d\n", __func__, __LINE__, info->actualSession);
    return;
}
#endif
#endif


#if 0
/********************************************************************
 * ������ utt_controlOneUserConnCount 
 * ���ܣ� ����һ���û�����
 * ������ 2012-03-28
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
int utt_controlOneUserConnCount(char *user)
{
#if (PPPOE_SERVER == FYES)
    int max = 0;
    int min = 0;
    int i = 0;

    int val = 0;

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
	    if((info->ConTime == 0 && (strncmp(info->pppoename, user, UTT_PPP_CLIENT_NAME_LEN) == 0)
			&& (strncmp(pppoeClientInfo.device, info->device, UTT_PPP_CLIENT_DEVICE_LEN) == 0)
			)
		    /* mantis 8436: ���ڽ��̲��ȣ�start time��� */
		    || 
		    ((info->pid != pppoeClientInfo.pid) && (info->STime == pppoeClientInfo.STime))) 
	    {
		val = 1;
		break;
	    }
	}
    }

    /*
     *�˴�Ϊ���źŽ���
     */
    uttSemV(SEM_PPPOE_NO,0);

    return val;
#else
    return 0;
#endif
}
#else
#if (PPPOE_SERVER == FYES)
/*
 * utt_actualSeeionPlus   -    �˺�ʵ�ʻỰ����1
 *
 * ���Ự�������ޣ����ڴ˴�����ʵ�ʻỰ����1��
 * ��������жϵ��²���ʧ������
 * user/rp-pppoe-3.8/src/pppoe-server.c
 * ��childHandler����(�˺�����pppd�������������)
 * �е���utt_minusMaxUserConnCount������ʵ�ʻỰ����1
 *
 * ������
 *	info		-		�˺Žṹ��
 *	index		-		�˺�����
 *	ip		-		�����Ự��ip
 *
 * ����ֵ��
 *	0		-		�Ự���ӳɹ�
 *	1		-		�Ự����ʧ��
 *
 */
static int utt_actualSeeionPlus(PPPoESrvAccProfile *info, int index, unsigned int ip)
{
	MibProfileType mibType = MIB_PROF_PPPOE_SRV_GLO;
	PPPoESrvGloProfile *prof = NULL;
	int i = 0, ret = 1;

	prof = (PPPoESrvGloProfile*)ProfGetInstPointByIndex(mibType, 0);
	if (ProfInstIsFree(prof) == 0) {
		i = ntohl(ip) - ntohl(prof->ipStart);
		if (i >= 0) {
			/*�Ự����1*/
			info->actualSession += 1;
			nvramProfile->Session[i].accIndex = index;
			ret = 0;
		}
		SWORD_SYSLOG(LOG_NOTICE, "%s-%d: i = %d, ntohl(ip) = %x, ntohl(prof->ipStart) = %x, ret = %d\n",  
					__func__, __LINE__, i, ntohl(ip), 
					ntohl(prof->ipStart), ret); 
	}
	return ret;
}
#endif
/********************************************************************
 * ������ utt_controlMaxUserConnCount
 * ���ܣ� �����û��Ự��
 * ������ 2012-08-20
 * ������ user   -    �û������˺�
 * ���أ�  1     -    �Ự������
 *         0     -    �Ự��û�г��� 
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
int utt_controlMaxUserConnCount(char *user, unsigned int ip)
{
#if (PPPOE_SERVER == FYES)
    int max = 0;
    int min = 0;
    int i = 0;

    int val = 0;

    MibProfileType mibType = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *info = NULL;

    sigset_t oldsignals_handled;

    SWORD_SYSLOG(LOG_NOTICE, "%s-----%d, pppoeClientInfo.device = \"%s\"\n",  __func__, __LINE__, pppoeClientInfo.device);

    if( strcmp(pppoeClientInfo.device, ( char *)getLanIfName()) != 0)
    {/*��lan�� pppoe����*/
	return val;
    }
    ProfInstLowHigh(mibType, &max, &min);

    /*�����źš���ֹpv�����м䱻signal��ϡ���signal������
     * ��term �����¼����������Լ��Ͱ��Լ�������
     * ������Է���ʹ����Ϊ�ú���ֻ�ı䵱ǰ���ź������֣������ı��źŵ���Ϊ
     * ��pppd�źŵ���ΪΪ sa_flags=0 ���źű�����ʱ���źŻᱻ�Ƴ�ִ��һ�Ρ������᲻ִ��*/
    sigprocmask(SIG_BLOCK, &signals_handled, &oldsignals_handled);

    /*
     *�˴�Ϊ�����ź��������˲���
     */
    uttSemP(SEM_PPPOE_NO,0);

    for(i = min;i < max;i++)
    {
	info = (PPPoESrvAccProfile*)ProfGetInstPointByIndex(mibType, i);
	if((info != NULL) && (ProfInstIsFree(info) == 0))
	{
	    if(strcmp(info->pppoename, user) == 0)
	    {
		SWORD_SYSLOG(LOG_NOTICE, "%s-----%d, info->pppoename = %s, info->actualSession = %d, info->accmaxconv = %d\n",  __func__, __LINE__, info->pppoename, info->actualSession, info->accmaxconv);
		if(info->actualSession + 1 > info->accmaxconv)
		{
		    val = 1;
		    break;
		} else {
			val = utt_actualSeeionPlus(info, i, ip);
		}
	    }
	}
    }

    /*
     *�˴�Ϊ���źŽ���
     */
    uttSemV(SEM_PPPOE_NO,0);
    /*signal�����ָֻ�*/
    sigprocmask(SIG_SETMASK, &oldsignals_handled, NULL);
    SWORD_SYSLOG(LOG_NOTICE, "%s-----%d, val = %d\n",  __func__, __LINE__, val);

    return val;
#else
    return 0;
#endif
}
#endif
/********************************************************************
 * ������ utt_UserMacBind
 * ���ܣ� PPPoE�˺�MAC��ַ���ж�
 * ������ 2012-08-16
 * ������ user  -   �û���
 * ���أ� 
 *         1    -   �жϰ󶨲�ͨ��,����ʧ��
 *         0    -   �жϰ�ͨ�������������ж�
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
int utt_UserMacBind(char *user)
{
#if (PPPOE_SERVER == FYES)
    int max = 0;
    int min = 0;
    int i = 0;
    int flag = 0;
    char pppoeMac[UTT_PPP_CLIENT_MAC_LEN + 1];

    MibProfileType mibType = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *info = NULL;

    sigset_t oldsignals_handled;
    ProfInstLowHigh(mibType, &max, &min);

    /*�����źš���ֹpv�����м䱻signal��ϡ���signal������
     * ��term �����¼����������Լ��Ͱ��Լ�������
     * ������Է���ʹ����Ϊ�ú���ֻ�ı䵱ǰ���ź������֣������ı��źŵ���Ϊ
     * ��pppd�źŵ���ΪΪ sa_flags=0 ���źű�����ʱ���źŻᱻ�Ƴ�ִ��һ�Ρ������᲻ִ��*/
    sigprocmask(SIG_BLOCK, &signals_handled, &oldsignals_handled);

    /*
     *�˴�Ϊ�����ź��������˲���
     */
    uttSemP(SEM_PPPOE_NO,0);

    for(i = min;i < max;i++)
    {
	info = (PPPoESrvAccProfile*)ProfGetInstPointByIndex(mibType, i);
	if((info != NULL) && (ProfInstIsFree(info) == 0))
	{
	    if(strcmp(info->pppoename, user) == 0)
	    {
		strcpy(pppoeMac, pppoeClientInfo.mac);
		strRemoveChar(pppoeMac, ':');/*ȥ��MAC��ַ�е�ð��*/
		SWORD_SYSLOG(LOG_NOTICE, "%s-----%d, pppoeMac = %s, converMac6To12Byte(info->bindmac) = %s\n",  __func__, __LINE__, pppoeMac, converMac6To12Byte(info->bindmac));
		if((strcmp(converMac6To12Byte(info->bindmac), "000000000000") != 0)
			|| (strcmp(converMac6To12Byte(info->bindmac2), "000000000000") != 0)
			|| (strcmp(converMac6To12Byte(info->bindmac3), "000000000000") != 0)
			|| (strcmp(converMac6To12Byte(info->bindmac4), "000000000000") != 0))
		{
		    if((strcmp(converMac6To12Byte(info->bindmac), my_strlwr(pppoeMac)) != 0)
			    && (strcmp(converMac6To12Byte(info->bindmac2), my_strlwr(pppoeMac)) != 0)
			    && (strcmp(converMac6To12Byte(info->bindmac3), my_strlwr(pppoeMac)) != 0)
			    && (strcmp(converMac6To12Byte(info->bindmac4), my_strlwr(pppoeMac)) != 0))
		    {
			flag = 1;/*�ж���ͨ��������ʧ��*/
		    }
		}
		else 
		{
#if 0
		    /* edit by guo.deyuan
		     * mantis bug 0012986
		     * ɾ���˴���mac��
		     * ���ųɹ��������mac��
		     * */
		    if(strcmp(info->AccMACBind, "autobind") == 0)
		    {
			converMac12To6Byte(pppoeMac, &(info->bindmac));
			nvramWriteCommit(); /*��macд��flash*/
		    }
#endif
		}
	    }/*end if(strcmp(info->pppoename, user) == 0)*/
	}/*end if((prof != NULL) && (ProfInstIsFree(prof) == 0))*/
    }/*end for(i = min;i < max;i++)*/

    /*
     *�˴�Ϊ���źŽ���
     */
    uttSemV(SEM_PPPOE_NO,0);
    /*signal�����ָֻ�*/
    sigprocmask(SIG_SETMASK, &oldsignals_handled, NULL);

    return flag;
#else
    return 0;
#endif
}
#if 0
int utt_controlOneUserConnCount(char *user)
{
    int val = 0;
    static int     st_size;
    off_t   start, end;
    int     fileLen;
    struct st_uttPppdClient info;
   // FILE *fp;
   // fp = fopen("/dev/ttyS1", "w+");

    if(fd == -1) {
        fd = open(UTT_PPPOE_CLIENT_INFO_FILE, O_RDWR|O_CREAT, 0666);
    }
    st_size = sizeof(struct st_uttPppdClient);
    start = lseek(fd, 0, SEEK_SET);
    end = lseek(fd, 0, SEEK_END);
    fileLen = end - start;
    lseek(fd, 0, SEEK_SET);
    fcntl(fd, F_SETLKW, file_lock(F_WRLCK|F_RDLCK, SEEK_SET));
    while(fileLen >= st_size) {
        read(fd, &info, st_size);
//	fprintf(fp, "info.user[%s] user[%s]\n",info.user,user);
        if((info.ConTime == 0 &&
                (strncmp(info.user, user, UTT_PPP_CLIENT_NAME_LEN) == 0)
#if 1
                && (strncmp(pppoeClientInfo.device, info.device, UTT_PPP_CLIENT_DEVICE_LEN) == 0)
#endif
	   )
		/* mantis 8436: ���ڽ��̲��ȣ�start time��� */
		|| 
		((info.pid != pppoeClientInfo.pid) && (info.STime == pppoeClientInfo.STime))) {
            val = 1;
            break;
        }
        fileLen -= st_size;
    }
    fcntl(fd, F_SETLKW, file_lock(F_UNLCK, SEEK_SET));
    return val;
}
#endif

#if 0
/* PPPOE�Ŀ����˺����Ự�� 
 * user: PPPOE�˺�
 * ����ֵ 1 �� �ﵽ���Ự�� ���ܲ���
 *        0 �� û�дﵽ���Ự�� ���Բ���
 * yqyang
 * */
int utt_controlUserMaxConnCount(char *user)
{
    int val = 0;
    static int     st_size;
    off_t   start, end;
    int     fileLen;
    struct st_uttPppdClient info;
    int connectSession = 0;  /*PPPOE��ǰ���ӻỰ��  */
    int accmaxconv = 0; /* ��ȡUSER�ĵ��˺����Ự��  */
    int min = 0, max = 0, i = 0;
  //  FILE *fp;
   // fp = fopen("/dev/ttyS1", "w+");

    MibProfileType mibTypeA = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *profA = NULL;

    ProfInstLowHigh(mibTypeA, &max, &min);

    for (i = min; i < max; i++) {
	profA = (PPPoESrvAccProfile *)ProfGetInstPointByIndex(mibTypeA, i);
        if((profA != NULL) && (ProfInstIsFree(profA) == 0)) {
	    if(strcmp(profA->head.name,user) == 0)
		    accmaxconv =  profA->accmaxconv;
	}
    }

    if(accmaxconv != 0){
	if(fd == -1) {
	    fd = open(UTT_PPPOE_CLIENT_INFO_FILE, O_RDWR|O_CREAT, 0666);
	}
	st_size = sizeof(struct st_uttPppdClient);
	start = lseek(fd, 0, SEEK_SET);
	end = lseek(fd, 0, SEEK_END);
	fileLen = end - start;
	lseek(fd, 0, SEEK_SET);
	fcntl(fd, F_SETLKW, file_lock(F_WRLCK|F_RDLCK, SEEK_SET));
	while(fileLen >= st_size) {
	    read(fd, &info, st_size);
	    if(info.ConTime == 0 &&
		    (strncmp(info.user, user, UTT_PPP_CLIENT_NAME_LEN) == 0)
		    && (strncmp(pppoeClientInfo.device, info.device, UTT_PPP_CLIENT_DEVICE_LEN) == 0)
	      ) {
		/* ͳ������˺ŵ�PPPOE������*/
		++connectSession;
	    }
	    fileLen -= st_size;
	}
	/* PPPOE����������������Ự���� ����1 */
//	fprintf(fp, "connectSession[%d] accmaxconv[%d]\n", connectSession, accmaxconv);
	if((accmaxconv != 0 && connectSession >= accmaxconv)){
	  //  fprintf(fp, "over mac pppoe max connect session !\n");
	    val = 1;
	}
	fcntl(fd, F_SETLKW, file_lock(F_UNLCK, SEEK_SET));
    }
    return val;
}
/*
 *MAC ȥ������ ����ת����Сд
 * ���� 00:33:AA:BB:CC:DD ת����0033aabbccdd
 */
static void macRemoveSemicolon(char *src, char *dst)
{
    int i = 0, j = 0;

    for(j = 0, i = 0; j < strlen(src)+1;)
    {   
	if(src[j] != ':')
	{   
	    dst[i] = tolower(src[j]);
	    i++;
	    j++;
	}   
	else
	{   
	    j++;
	}   
    }   
}

/* 
 * ����������mac�󶨵Ĳ���
 * user ���mac�󶨵�PPPOE���û���
 * ����ֵ��1����ʾ��MAC�Ͳ���MAC��һ�£��޷���֤
 *         0��������֤
 */
int utt_controlUserMacBind(char *user)
{
    int val = 0;
    unsigned char userbindmac[13] = {0};
    unsigned char infoMac[13] = {0};
    int min = 0, max = 0, i = 0;
  //  FILE *fp;
 //   fp = fopen("/dev/ttyS1", "w+");

    MibProfileType mibTypeA = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *profA = NULL;

    sigset_t oldsignals_handled;
    ProfInstLowHigh(mibTypeA, &max, &min);

    /*�����źš���ֹpv�����м䱻signal��ϡ���signal������
     * ��term �����¼����������Լ��Ͱ��Լ�������
     * ������Է���ʹ����Ϊ�ú���ֻ�ı䵱ǰ���ź������֣������ı��źŵ���Ϊ
     * ��pppd�źŵ���ΪΪ sa_flags=0 ���źű�����ʱ���źŻᱻ�Ƴ�ִ��һ�Ρ������᲻ִ��*/
    sigprocmask(SIG_BLOCK, &signals_handled, &oldsignals_handled);
    /*
     *�˴�Ϊ�����ź��������˲���
     */
    uttSemP(SEM_PPPOE_NO,0);

    for (i = min; i < max; i++) {
	profA = (PPPoESrvAccProfile *)ProfGetInstPointByIndex(mibTypeA, i);
	if(profA != NULL){
	    if(strcmp(profA->head.name,user) == 0)
	    {
		strcpy(userbindmac, converMac6To12Byte(profA->bindmac));
	    }
	}
    }

  //  fprintf(fp, "userbindmac[%s] mac[%s]\n",userbindmac,pppoeClientInfo.mac);
    if(strlen(userbindmac) >= 12  && strcmp(userbindmac, "000000000000") != 0){
	macRemoveSemicolon(pppoeClientInfo.mac,infoMac);
	/* ��MAC��ַ������ ����PPPOE��ַ���ǰ󶨵�MAC��ַ �����쳣*/
	if((strlen(userbindmac) == 12) && (strcmp(infoMac,userbindmac))){
	    	 //   fprintf(fp,"pppoe mac band is not match! \n");
	    val = 1;
	}
    }

    /*
     *�˴�Ϊ���źŽ���
     */
    uttSemV(SEM_PPPOE_NO,0);
    /*signal�����ָֻ�*/
    sigprocmask(SIG_SETMASK, &oldsignals_handled, NULL);

    return val;
}


#endif

#ifdef UTT_IP_ASSIGN
/*
 * Used for storing a sequence of words.  Usually malloced. 
 * same define in pppd.h
 */
struct wordlist {
    struct wordlist	*next;
    char		*word;
};

/*
 * ���wordlist���Ƿ����ָ��IP��������ͨ���
 * �����ж�һ���˺��Ƿ��ǹ̶�IP�����˺�
 * ����ֵ:
 *  0	-   wordlist�в�����ָ��IP
 *  1	-   wordlist�а���ָ��IP
 */
extern int utt_poesrv_bindip_ok(struct wordlist *addrs) {
    while (addrs != NULL) {
	if (addrs->word[0] == '-') {
	    break;
	}
	if (addrs->word[0] == '*') {
	    break;
	}
	if (addrs->word[0] != '!') {
	    return 1;
	}
	addrs = addrs->next;
    }
    return 0;
}

#define UTT_MAXLINE 256
/*
 * isBindIp - �ж�һ��IP��ַ�Ƿ��ǰ�IP��ַ
 * ���������
 *  ip	-   �жϵ�IP��ַ�������ֽ���
 * ����ֵ��
 *  0	-   �ǰ�IP
 *  1	-   ��IP
 */
extern int isBindIp(u_int32_t ip) {
    FILE *fp = NULL;
    char line[UTT_MAXLINE];
    unsigned char bufIp[4];
    unsigned int a, b, c, d;
    int rst = 0;

    fp = fopen(PPPOE_SERVER_BIND_IP_FILE, "r");
    if (fp == NULL) {
	return rst;
    }

    while (feof(fp) == 0) {
	if (fgets(line, UTT_MAXLINE, fp) == NULL) {
	    break;
	}
	if ((sscanf(line, "%u.%u.%u.%u", &a, &b, &c, &d) == 4) && 
		(a < 256) && (b < 256) && (c < 256) && (d < 256)) {
	    bufIp[0] = (unsigned char)a;
	    bufIp[1] = (unsigned char)b;
	    bufIp[2] = (unsigned char)c;
	    bufIp[3] = (unsigned char)d;
	    if (ip == *((unsigned int *)bufIp)) {
		rst = 1;
		break;
	    }
	}
    }
    fclose(fp);
    return rst;

}
#endif
#if (SYS_LOG_INFO == FYES)
int isPppoeLogEn(void)
{
	MibProfileType mibType = MIB_PROF_SYS_LOG_INFO;
	SysLogInfoProfile *prof = NULL;
	int ret = 0;

	prof = (SysLogInfoProfile *)ProfGetInstPointByIndex(mibType, 0);
	if (ProfInstIsFree(prof) == 0) 
		if (prof->PppoeLogEnable == LOG_ENABLE) 
			ret = 1;

	return ret;
}
#endif
