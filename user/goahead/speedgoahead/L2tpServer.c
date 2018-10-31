#include "uttMachine.h"
#if (L2TP_SERVER == FYES)
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "argcmd.h"
#include "a2n.h"
#include "internet.h"
#include "utt_conf_define.h"
#include "utt_pppoeClient.h" 

//#define L2TP_DEBUG

#ifdef L2TP_DEBUG
#define L2TP_PRINT(fmt, args...)		printf(fmt, ## args)
#else
#define L2TP_PRINT(fmt, args...)
#endif

/*
 * /etc/ppp/chap-secrets�е�����
 */
#define L2TP_SRV_ASS_IP	"\\*"

#define IP_SEG_LEN  40
#define IP_LEN	    20

#if (L2TP_CLIENT == FYES)
extern int l2tpClientInit(void);
#endif
/*
 * ɱ����l2tp�ͻ��˽���
 */
static void killAllL2tpClient(void) {
    int fd = -1;
    struct st_uttPppdClient info;
    int st_size = sizeof(struct st_uttPppdClient);
    fd = open(UTT_L2TP_CLIENT_INFO_FILE, O_RDONLY);

    if(fd != -1) {
        while (read(fd, &info, st_size) > 0) {
	    if (info.ConTime == 0) {
		doSystem("kill -INT %d", info.pid);
#if 0 //(VPN_CONTROL == FYES)
		/*����˿�����Ŀ��pppd�д���*/
		doSystem("vpnHandle plus ");
#endif
	    }
        }
	close(fd);
	doSystem("rm -f %s 2>/dev/null", UTT_L2TP_CLIENT_INFO_FILE);
    }
    return;
}

/*
 * ɱָ��l2tp�ͻ��˽���
 */
static void killOneL2tpClient(char *user) 
{
    int fd = -1;
    struct st_uttPppdClient info;
    int st_size = sizeof(struct st_uttPppdClient);
    if(user != NULL) {
    fd = open(UTT_L2TP_CLIENT_INFO_FILE, O_RDONLY);

    if(fd != -1) {
        while (read(fd, &info, st_size) > 0) {
	    L2TP_PRINT("%s: user=%s,pid=%d,STime=%d,LTime=%d,ConTime=%d,rx=%lld,tx=%lld\n",__FUNCTION__,info.user,info.pid,info.STime,info.LTime,info.ConTime,info.rxByte,info.txByte);
	    if (info.ConTime == 0) {
            if(strncmp(user, info.user, UTT_PPP_CLIENT_NAME_LEN) == 0) {
                doSystem("kill -INT %d", info.pid);
#if 0 //(VPN_CONTROL == FYES)
		/*����˿�����Ŀ��pppd�д���*/
		doSystem("vpnHandle plus ");
#endif
		break;
            }
	    }
        }
	close(fd);
    }
    }
    return;
}

#if (L2TP_CLIENT == FYES)
static void killL2tpClient()
{
    MibProfileType mibType = MIB_PROF_L2TP_CLIENT;
    L2TPCliProfile *prof = NULL;
    int min = 0, max = 0, i = 0;
    
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) {
	prof = (L2TPCliProfile *)ProfGetInstPointByIndex(mibType, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
	    if (prof->head.active == 1) {
		doSystem("l2tp-down.sh l2tp_%s ",prof->head.name);
		doSystem("echo 'd %s' > %s",prof->head.name, L2TP_CONTROL_PIPE);
		usleep(100000); /*0.1s*/
	    }
	}
    }
}
#endif

static void l2tpSrvGloConfChg(L2TPSrvGlobalProfile *prof) {
    struct in_addr fromAddr, endAddr, addrL, listenAddr,addrPriDns, addrSecDns;
    /* 255.255.255.255-255.255.255.255 */
    char remoteIp[IP_SEG_LEN] = {0};
    /* 255.255.255.255 */
    char localIp[IP_LEN] = {0};
    /* 255.255.255.255 */
    char listenIp[IP_LEN] = {0};
    char priDns[IP_LEN] = {0};
    char secDns[IP_LEN] = {0};
    char lanIp[16];
 
    fromAddr.s_addr = prof->poolIpStart;
    if (prof->poolIpCnt <= 1) {
	endAddr = fromAddr;
    } else {
	endAddr.s_addr = htonl(ntohl(prof->poolIpStart) + prof->poolIpCnt - 1);
    }
    sprintf(remoteIp, "%s-", inet_ntoa(fromAddr));
    strcat(remoteIp, inet_ntoa(endAddr));
    addrL.s_addr = prof->localIp;
    sprintf(localIp, "%s", inet_ntoa(addrL));
    listenAddr.s_addr = prof->listenIp;
    sprintf(listenIp, "%s", inet_ntoa(listenAddr));
    addrPriDns.s_addr = prof->priDns;
    sprintf(priDns, "%s", inet_ntoa(addrPriDns));
    addrSecDns.s_addr = prof->secDns;
    sprintf(secDns, "%s", inet_ntoa(addrSecDns));

    memset(lanIp, 0, sizeof(lanIp));
    getIfIp(getLanIfName(), lanIp);/*���LAN��IP*/
    L2TP_PRINT("%s: remoteIp=%s,localIp=%s,authtype=%s,listenIp=%s\n", 
		    __FUNCTION__, remoteIp, localIp, getEnumString(pppAuthEnum, prof->pppAuthMode), listenIp);
    /*
     * �޸�����
     */
    if(strcmp(lanIp, listenIp) ==0) {
    doSystem("l2tpSrvGloConf.sh %s %s %s %s %s %s ", remoteIp, localIp, getEnumString(pppAuthEnum, prof->pppAuthMode), listenIp,priDns,secDns);
    doSystem("l2tpSrvGloConf_update.sh %s %s %s %s %s %s ", remoteIp, localIp, getEnumString(pppAuthEnum, prof->pppAuthMode), listenIp,priDns,secDns);
    L2TP_PRINT("l2tpSrvGloConf.sh %s %s %s %s %s %s ", remoteIp, localIp, getEnumString(pppAuthEnum, prof->pppAuthMode), listenIp,priDns,secDns);
    } else {
    doSystem("l2tpSrvGloConf.sh %s %s %s %s %s %s ", remoteIp, localIp, getEnumString(pppAuthEnum, prof->pppAuthMode), lanIp,priDns,secDns);
    doSystem("l2tpSrvGloConf_update.sh %s %s %s %s %s %s ", remoteIp, localIp, getEnumString(pppAuthEnum, prof->pppAuthMode), lanIp,priDns,secDns);
    L2TP_PRINT("l2tpSrvGloConf.sh %s %s %s %s %s %s ", remoteIp, localIp, getEnumString(pppAuthEnum, prof->pppAuthMode), lanIp,priDns,secDns);
    }

    return;
}

#if (L2TP_CLIENT == FYES)
static void l2tpServerConfUpdata()
{
    char cmd[80];
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd,80,"xl2tpd-control update /etc/xl2tpd/xl2tpd_bak.conf "); /*xl2tpd��laclist�пͻ���������ɾ������*/
    L2TP_PRINT("%s-%d: cmd =%s\n",__func__,__LINE__,cmd);
    doSystem(cmd);
    sleep(3);

}
#endif
/*
 * ȫ�������޸�profile change����
 */
extern void l2tpSrvGlobalProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr )
{
    L2TPSrvGlobalProfile *newProf = (L2TPSrvGlobalProfile *)newProfileDataPtr;
    L2TPSrvGlobalProfile *oldProf = (L2TPSrvGlobalProfile *)oldProfileDataPtr;
#if (L2TP_CLIENT == FYES)
    int ret = 0;
#endif

    L2TP_PRINT("%s!\n", __FUNCTION__);

    switch(changeType) {
        case PROF_CHANGE_ADD:
            break;
        case PROF_CHANGE_EDIT:
#if 0
	    fromAddr.s_addr = newProf->poolIpStart;
	    if (newProf->poolIpCnt <= 1) {
		endAddr = fromAddr;
	    } else {
		endAddr.s_addr = htonl(ntohl(newProf->poolIpStart) + newProf->poolIpCnt - 1);
	    }
	    sprintf(remoteIp, "%s-", inet_ntoa(fromAddr));
	    strcat(remoteIp, inet_ntoa(endAddr));
	    addrL.s_addr = newProf->localIp;
	    sprintf(localIp, "%s", inet_ntoa(addrL));
	    L2TP_PRINT("%s: remoteIp=%s,localIp=%s,authtype=%s\n", 
		    __FUNCTION__, remoteIp, localIp, getEnumString(pppAuthEnum, newProf->pppAuthMode));
	    /*
	     * �޸�����
	     */
	    doSystem("l2tpSrvGloConf.sh %s %s %s", remoteIp, localIp, getEnumString(pppAuthEnum, newProf->pppAuthMode));
#endif
	    l2tpSrvGloConfChg(newProf);
	    sleep(1);

	    /*
	     * ֮ǰ������on���ر�server
	     */
#if 1
	    /* mantis 15358
	     * �޸�Ϊ���������ʱ
	     * ֻ�Ǹ��·��������
	     * ����������xl2tpd����
	     * */
	    if (oldProf->head.active) {
		if (newProf->head.active) {
		    /* ɱ�û����� */
		    killAllL2tpClient();
#if (L2TP_CLIENT == FYES)
		    l2tpServerConfUpdata();
#endif
		} else {
#if (L2TP_CLIENT == FYES)
		    if(checkl2tpCliExistOrNum()>0) {
			/* ɱ�û����� */
			killAllL2tpClient();
		    } else 
#endif
		    {
			/* ɱ�û����� */
			killAllL2tpClient();
			doSystem("l2tpSrvDown.sh > /dev/null 2>&1");
		    }
		}
	    } else {
		/*
		 * ��������on,��ʼserver
		 */
		if (newProf->head.active) {
#if (L2TP_CLIENT == FYES)
		    if(checkl2tpCliExistOrNum()>0) {
		    l2tpServerConfUpdata();
		    } else 
#endif
		    {
		    doSystem("l2tpSrvUp.sh");
		    }
		} else {
		}
	    }
#else
	    if (oldProf->head.active) {
		if (newProf->head.active) {
		    /* ɱ�û����� */
		    killAllL2tpClient();
#if (L2TP_CLIENT == FYES)
		    killL2tpClient();
		    sleep(1);
#endif
		    doSystem("l2tpSrvDown.sh > /dev/null 2>&1");
		    sleep(4);
#if (L2TP_CLIENT == FYES)
		    l2tpClientInit();
#endif
		    doSystem("l2tpSrvUp.sh");
		}else{
#if (L2TP_CLIENT == FYES)
		    if(checkl2tpCliExistOrNum()>0) {
			/* ɱ�û����� */
			killAllL2tpClient();
		    } else 
#endif
		    {
			/* ɱ�û����� */
			killAllL2tpClient();
			doSystem("l2tpSrvDown.sh > /dev/null 2>&1");
		    }
		}
	    }
	    else {
#if (L2TP_CLIENT == FYES)
	    ret =l2tpClientInit();
#endif
	    /*
	     * ��������on,��ʼserver
	     */
	    if (newProf->head.active) {
#if (L2TP_CLIENT == FYES)
		if(ret == 1) {
		    killL2tpClient();
		    sleep(1);
		    doSystem("l2tpSrvDown.sh > /dev/null 2>&1");
		    sleep(4);
		}
#endif
		doSystem("l2tpSrvUp.sh");
	    }
#if (L2TP_CLIENT == FYES)
	    else if(ret == 1) {
		//doSystem("l2tpSrvUp.sh");
	    }
#endif
	    }
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

/*
 * l2tp server��ʼ��
 */
void l2tpServerInit(void) {
    L2TPSrvGlobalProfile *profL2tpS;
    MibProfileType mibTypeGlobal = MIB_PROF_L2TP_SRV_GLOBAL;
    L2TPSrvAccProfile *profL2tpA;
    MibProfileType mibTypeAccount = MIB_PROF_L2TP_SRV_ACCOUNT;
    int min = 0, max = 0, i = 0;
#if (L2TP_CLIENT == FYES)
    int ret = 0;
#endif
#if 1
    /*�ȹҶ����е�L2TP�ͻ��˺ͷ�������ӣ�Ȼ����������*/
    /* ɱ�û����� */
    killAllL2tpClient();
#if (L2TP_CLIENT == FYES)
    killL2tpClient();
    sleep(1);
#endif
    doSystem("l2tpSrvDown.sh > /dev/null 2>&1");
    sleep(2);
#endif

    L2TP_PRINT("%s\n", __FUNCTION__);

    /*
     * ���ó�ʼ���ű�
     */
    doSystem("l2tpSrvInit.sh");

    /*
     * ��ʼ���˺�����
     */
    ProfInstLowHigh(mibTypeAccount, &max, &min);
    for (i = min; i < max; i++) {
        profL2tpA = (L2TPSrvAccProfile *)ProfGetInstPointByIndex(mibTypeAccount, i);
	if((profL2tpA != NULL) && (ProfInstIsFree(profL2tpA) == 0)) {
	    doSystem("l2tpSrvAccAdd.sh %s %s %s %s", 
		    profL2tpA->userName, L2TP_SRV_NAME, profL2tpA->passwd, L2TP_SRV_ASS_IP);
	}
    }

    /*
     * ��ʼ��ȫ������
     */
    profL2tpS = (L2TPSrvGlobalProfile *)ProfGetInstPointByIndex(mibTypeGlobal, 0);
    l2tpSrvGloConfChg(profL2tpS);
#if (L2TP_CLIENT == FYES)
    ret =l2tpClientInit();
#endif
    if (profL2tpS->head.active == 1) {
	doSystem("l2tpSrvUp.sh");
    }
#if (L2TP_CLIENT == FYES)
    else if(ret == 1) {
	doSystem("l2tpSrvUp.sh");
    }
#endif
    return;
}

/*
 * l2tp serve �˺�profile change����
 */
static void l2tpSrvAccountProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr )
{
    L2TPSrvAccProfile *newProf = (L2TPSrvAccProfile *)newProfileDataPtr;
    L2TPSrvAccProfile *oldProf = (L2TPSrvAccProfile *)oldProfileDataPtr;

    switch(changeType) {
        case PROF_CHANGE_ADD:
	    /*
	     * �����ļ���һ���˺�
	     */
	    L2TP_PRINT("%s PROF_CHANGE_ADD: user=%s, srvName=%s, passwd=%s, assip=%s\n", 
		    __FUNCTION__, newProf->userName, L2TP_SRV_NAME, newProf->passwd, L2TP_SRV_ASS_IP);
	    doSystem("l2tpSrvAccAdd.sh %s %s %s %s", newProf->userName, L2TP_SRV_NAME, newProf->passwd, L2TP_SRV_ASS_IP);

	    break;
        case PROF_CHANGE_EDIT:
	    /*
	     * ��ɾ����Ӧ�˺ţ�����Ӷ�Ӧ�˺�
	     */	    
	    L2TP_PRINT("%s PROF_CHANGE_EDIT-oldProf: user=%s\n", 
		    __FUNCTION__, oldProf->userName);
	    L2TP_PRINT("%s PROF_CHANGE_EDIT-newProf: user=%s, srvName=%s, passwd=%s, assip=%s\n", 
		    __FUNCTION__, newProf->userName, L2TP_SRV_NAME, newProf->passwd, L2TP_SRV_ASS_IP);

	    killOneL2tpClient(oldProf->userName);
	    doSystem("l2tpSrvAccDel.sh %s %s", oldProf->userName, L2TP_SRV_NAME);
	    doSystem("l2tpSrvAccAdd.sh %s %s %s %s", newProf->userName, L2TP_SRV_NAME, newProf->passwd, L2TP_SRV_ASS_IP);

	    break;
        case PROF_CHANGE_DEL:
	    /*
	     * �����ļ�ɾ����Ӧ����
	     */
	    L2TP_PRINT("%s PROF_CHANGE_DEL-oldProf: user=%s\n", 
		    __FUNCTION__, oldProf->userName);

	    killOneL2tpClient(oldProf->userName);
	    doSystem("l2tpSrvAccDel.sh %s %s", oldProf->userName, L2TP_SRV_NAME);

	    break;
        case PROF_CHANGE_DELALL:
	    /*
	     * �����ļ�ɾ�������˺�
	     */
	    L2TP_PRINT("%s PROF_CHANGE_DELALL\n", __FUNCTION__);
	    killAllL2tpClient();
	    sleep(1);
	    doSystem("l2tpSrvAccDelAll.sh %s", L2TP_SRV_NAME);

	    break;
        default:
	    break;
    };
}

/*
 * ��ʼ������
 */
extern void funInitL2tpServer(void) {
    registerForProfileUpdates(MIB_PROF_L2TP_SRV_GLOBAL, NULL, l2tpSrvGlobalProfileChange);
    registerForProfileUpdates(MIB_PROF_L2TP_SRV_ACCOUNT, NULL, l2tpSrvAccountProfileChange);
    l2tpServerInit();

    return;
}
#endif
