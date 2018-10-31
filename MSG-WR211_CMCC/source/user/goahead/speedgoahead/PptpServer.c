#include "uttMachine.h"
#if (PPTP_SERVER == FYES)
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

//#define PPTP_DEBUG
#include	"utt_pppoeClient.h"

#ifdef PPTP_DEBUG
#define PPTP_PRINT(fmt, args...)		printf(fmt, ## args)
#else
#define PPTP_PRINT(fmt, args...)
#endif

/*
 * /etc/ppp/chap-secrets�е�����
 */
#define PPTP_SRV_ASS_IP	"\\*"

#define IP_SEG_LEN  40
#define IP_LEN	    20

#if (PPTP_IP_BIND == FYES)
static void creatPptpBindFile(void);
#endif
/*
 * ɾ��ָ���û�pptp����
 */
static void killOnePptpClient(char *user)
{
    PPTP_PRINT("%s: user=%s\n", __FUNCTION__, user);
    int fd = -1;
    struct st_uttPppdClient info;
    int st_size = sizeof(struct st_uttPppdClient);
    fd = open(UTT_PPTP_CLIENT_INFO_FILE, O_RDONLY);
    if(fd != -1) {
        while(read(fd, &info, st_size) > 0) {
	    PPTP_PRINT("%s: user=%s,pid=%d,STime=%d,LTime=%d,ConTime=%d,rx=%lld,tx=%lld\n",
		    __FUNCTION__, info.user, info.pid, info.STime,info.LTime,info.ConTime,
		    info.rxByte,info.txByte);
	    PPTP_PRINT("mac=%s, ip=%s, ifname=%s, devie=%s\n", info.mac, info.ip,
		    info.ifname,info.device);
            if(strncmp(user, info.user, UTT_PPP_CLIENT_NAME_LEN) == 0) {
                doSystem("kill -INT %d", info.pid);
#if (VPN_CONTROL == FYES)
		doSystem("vpnHandle plus ");
#endif
		break;
            }
        }
	close(fd);
    }
    return;
}

/*
 * ɾ������pptp�������˺Ų����˺�
 */
static void killallPptpClient(void)
{
    int fd = -1;
    struct st_uttPppdClient info;
    int st_size = sizeof(struct st_uttPppdClient);
    fd = open(UTT_PPTP_CLIENT_INFO_FILE, O_RDONLY);
    if(fd != -1) {
        while(read(fd, &info, st_size) > 0) {
	    PPTP_PRINT("%s: user=%s,pid=%d,STime=%d,LTime=%d,ConTime=%d,rx=%lld,tx=%lld\n",
		    __FUNCTION__, info.user, info.pid, info.STime,info.LTime,info.ConTime,
		    info.rxByte,info.txByte);
	    PPTP_PRINT("mac=%s, ip=%s, ifname=%s, devie=%s\n", info.mac, info.ip,
		    info.ifname,info.device);

	    if ((info.ConTime == 0) && (strcmp(info.ip, "") != 0)) {
		doSystem("kill -INT %d", info.pid);
#if (VPN_CONTROL == FYES)
		doSystem("vpnHandle plus ");
#endif
	    }
        }
	close(fd);
    }
#if 1
    /* mantis 8923: ɾ�������˺���ͬʱɾ��ͳ���ļ� */
    doSystem("rm /var/run/uttPptpClientInfo 2>/dev/null");
#endif
    return;
}

/*
 * ȫ�����ñ仯���޸���������ļ�
 */
static void pptpSrvGloConfChg(PPTPSrvGlobalProfile *prof) {
    struct in_addr fromAddr, addrL, listenAddr, addrPriDns, addrSecDns;
    /* 
     * 255.255.255.254-255,255.255.255.255-255 
     * Ŀǰpptp��ַ��ֻ֧�ֿ�һ������
     */
    char remoteIp[IP_SEG_LEN] = {0};
    /* 255.255.255.255 */
    char localIp[IP_LEN] = {0};
    char priDns[IP_LEN] = {0};
    char secDns[IP_LEN] = {0};
    char listenIp[IP_LEN] = {0};
    /* numpȡip��ַ���λ���� */
    unsigned char *nump = NULL;
    unsigned char num;
    /* ��λ��ֵ */
    int over;
 
    fromAddr.s_addr = prof->poolIpStart;

    /* ȡip��ַ����ֽ����� */
    nump = (unsigned char *)(&(prof->poolIpStart));
    num = *(nump + 3);

    /* ��λ��ֵ */
    over = 256 - num;
    /* ������ num + prof->poolIpCnt -1 > 255 */
    if (over < prof->poolIpCnt) { /* ����ĵ�ַ������ */
	char secondSeg[IP_LEN] = {0};
	struct in_addr secFromAddr;
	unsigned char secNum;
	sprintf(remoteIp, "%s-%d,",inet_ntoa(fromAddr), 255);

	/* �ڶ�����ʼIP, ��λ֮���IP */
	secFromAddr.s_addr = htonl(ntohl(fromAddr.s_addr) + over);
	/* ����IP, ����ǰһ��������IP����ʼIP */
	secNum = prof->poolIpCnt - over - 1;
	sprintf(secondSeg, "%s-%d", inet_ntoa(secFromAddr), secNum);
	strcat(remoteIp, secondSeg);
    } else {
	if (prof->poolIpCnt >= 1) {
	    num += prof->poolIpCnt - 1;
	}
	sprintf(remoteIp, "%s-%d", inet_ntoa(fromAddr), num);
    }
#if 0
    if (prof->poolIpCnt > 1) {
    } else {
	endAddr.s_addr = htonl(ntohl(prof->poolIpStart) + prof->poolIpCnt - 1);
    }
#endif
    addrL.s_addr = prof->localIp;
    sprintf(localIp, "%s", inet_ntoa(addrL));
    addrPriDns.s_addr = prof->priDns;
    sprintf(priDns, "%s", inet_ntoa(addrPriDns));
    addrSecDns.s_addr = prof->secDns;
    sprintf(secDns, "%s", inet_ntoa(addrSecDns));
    listenAddr.s_addr = prof->listenIp;
    sprintf(listenIp, "%s", inet_ntoa(listenAddr));

#if (PPTP_ENCRYPT_MODE == FYES)
    PPTP_PRINT("%s: remoteIp=%s,localIp=%s,authtype=%s,listenIp=%s\n", 
		    __FUNCTION__, remoteIp, localIp, getEnumString(pppAuthEnum, prof->pppAuthMode), listenIp);
#else
    PPTP_PRINT("%s: remoteIp=%s,localIp=%s,authtype=%s,listenIp=%s, EncryptionMode = %s\n", 
		    __FUNCTION__, remoteIp, localIp, getEnumString(pppAuthEnum, prof->pppAuthMode), listenIp, prof->EncryptionMode);
#endif
    /*
     * �޸�����
     */
#if (PPTP_ENCRYPT_MODE == FYES)
    doSystem("pptpSrvGloConf.sh %u %s %s %s %s %s %s %s", prof->mtu, remoteIp, localIp, priDns, secDns, getEnumString(pppAuthEnum, prof->pppAuthMode), listenIp, prof->EncryptionMode);
    if(strcmp(prof->EncryptionMode,"MPPE") == 0){ 
        doSystem("iptables -I FORWARD -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --set-mss 1200");
    } else {
        doSystem("iptables -D FORWARD -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --set-mss 1200");
    }   
#else
    doSystem("iptables -D FORWARD -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --set-mss 1200");
    doSystem("pptpSrvGloConf.sh %u %s %s %s %s %s %s", prof->mtu, remoteIp, localIp, priDns, secDns, getEnumString(pppAuthEnum, prof->pppAuthMode), listenIp);
#endif

    return;
}

/*
 * ȫ�������޸�profile change����
 */
extern void pptpSrvGlobalProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr )
{
    PPTPSrvGlobalProfile *newProf = (PPTPSrvGlobalProfile *)newProfileDataPtr;
    PPTPSrvGlobalProfile *oldProf = (PPTPSrvGlobalProfile *)oldProfileDataPtr;

    PPTP_PRINT("%s!\n", __FUNCTION__);

    switch(changeType) {
        case PROF_CHANGE_ADD:
            break;
        case PROF_CHANGE_EDIT:
	    pptpSrvGloConfChg(newProf);

	    /*
	     * ֮ǰ������on���ر�server
	     */
	    if (oldProf->head.active) {
		doSystem("pptpSrvDown.sh");
	    }
	    /* mantis 8923: �ر�pptp������ʱͬʱɾ��ͳ���ļ� */
	    doSystem("rm /var/run/uttPptpClientInfo 2>/dev/null");

	    /*
	     * ��������on,��ʼserver
	     */
	    if (newProf->head.active) {
		doSystem("pptpSrvUp.sh");
	    }
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

/*
 * pptp server��ʼ��
 */
static void pptpServerInit(void) {
    PPTPSrvGlobalProfile *profPptpS;
    MibProfileType mibTypeGlobal = MIB_PROF_PPTP_SRV_GLOBAL;
    PPTPSrvAccProfile *profPptpA;
    MibProfileType mibTypeAccount = MIB_PROF_PPTP_SRV_ACCOUNT;
    int min = 0, max = 0, i = 0;
#if (PPTP_IP_BIND == FYES)
    struct in_addr addr;
#endif

    PPTP_PRINT("%s\n", __FUNCTION__);

#if 0
    /*
     * ���ó�ʼ���ű�
     */
    doSystem("pptpSrvInit.sh");
#endif

    /*
     * ��ʼ���˺�����
     */
    ProfInstLowHigh(mibTypeAccount, &max, &min);
    for (i = min; i < max; i++) {
        profPptpA = (PPTPSrvAccProfile *)ProfGetInstPointByIndex(mibTypeAccount, i);
	if((profPptpA != NULL) && (ProfInstIsFree(profPptpA) == 0)) {
#if (PPTP_IP_BIND == FYES)
	    if(profPptpA->bindIp == 0)
	    {
#if (HARD_FEATURE == FYES)
		if(strcmp(converMac6To12Byte(profPptpA->hardF), "000000000000")!=0) {
		doSystem("pptpSrvAccAdd.sh %s %s %s%s %s", 
		    profPptpA->userName, PPTP_SRV_NAME, converMac6To12Byte(profPptpA->hardF), profPptpA->passwd, PPTP_SRV_ASS_IP);
		} else {
		doSystem("pptpSrvAccAdd.sh %s %s %s %s", 
		    profPptpA->userName, PPTP_SRV_NAME, profPptpA->passwd, PPTP_SRV_ASS_IP);
		}
#else
		doSystem("pptpSrvAccAdd.sh %s %s %s %s", 
		    profPptpA->userName, PPTP_SRV_NAME, profPptpA->passwd, PPTP_SRV_ASS_IP);
#endif /*HARD_FEATURE*/
	    }
	    else
	    {
		addr.s_addr = profPptpA->bindIp;
		doSystem("echo %s >> %s &", inet_ntoa(addr), PPPOE_SERVER_BIND_IP_FILE);
#if (HARD_FEATURE == FYES)
		if(strcmp(converMac6To12Byte(profPptpA->hardF), "000000000000")!=0) {
		doSystem("pptpSrvAccAdd.sh %s %s %s%s %s", 
			profPptpA->userName, PPTP_SRV_NAME, converMac6To12Byte(profPptpA->hardF), profPptpA->passwd, inet_ntoa(addr));
		} else {
		doSystem("pptpSrvAccAdd.sh %s %s %s %s", 
			profPptpA->userName, PPTP_SRV_NAME, profPptpA->passwd, inet_ntoa(addr));
		}
#else
		doSystem("pptpSrvAccAdd.sh %s %s %s %s", 
			profPptpA->userName, PPTP_SRV_NAME, profPptpA->passwd, inet_ntoa(addr));
#endif /*HARD_FEATURE*/
	    }
#else
#if (HARD_FEATURE == FYES)
	    if(strcmp(converMac6To12Byte(profPptpA->hardF), "000000000000")!=0) {
	    doSystem("pptpSrvAccAdd.sh %s %s %s%s %s", 
		    profPptpA->userName, PPTP_SRV_NAME, converMac6To12Byte(profPptpA->hardF), profPptpA->passwd, PPTP_SRV_ASS_IP);
	    } else {
	    doSystem("pptpSrvAccAdd.sh %s %s %s %s", 
		    profPptpA->userName, PPTP_SRV_NAME, profPptpA->passwd, PPTP_SRV_ASS_IP);
	    }
#else
	    doSystem("pptpSrvAccAdd.sh %s %s %s %s", 
		    profPptpA->userName, PPTP_SRV_NAME, profPptpA->passwd, PPTP_SRV_ASS_IP);
#endif /*HARD_FEATURE*/
#endif
	}
    }

    /*
     * ��ʼ��ȫ������
     */
    profPptpS = (PPTPSrvGlobalProfile *)ProfGetInstPointByIndex(mibTypeGlobal, 0);
    pptpSrvGloConfChg(profPptpS);
    if (profPptpS->head.active == 1) {
	doSystem("pptpSrvUp.sh");
    }
    return;
}

/*
 * pptp serve �˺�profile change����
 */
static void pptpSrvAccountProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr )
{
    PPTPSrvAccProfile *newProf = (PPTPSrvAccProfile *)newProfileDataPtr;
    PPTPSrvAccProfile *oldProf = (PPTPSrvAccProfile *)oldProfileDataPtr;
#if (PPTP_IP_BIND == FYES)
struct in_addr addr;
#endif
    switch(changeType) {
        case PROF_CHANGE_ADD:
	    /*
	     * �����ļ���һ���˺�
	     */
	    PPTP_PRINT("%s PROF_CHANGE_ADD: user=%s, srvName=%s, passwd=%s, assip=%s\n", 
		    __FUNCTION__, newProf->userName, PPTP_SRV_NAME, newProf->passwd, PPTP_SRV_ASS_IP);
#if (PPTP_IP_BIND == FYES)
	    if(newProf->bindIp == 0)
	    {
#if (HARD_FEATURE == FYES)
		if(strcmp(converMac6To12Byte(newProf->hardF), "000000000000")!=0) {
		doSystem("pptpSrvAccAdd.sh %s %s %s%s %s", newProf->userName, PPTP_SRV_NAME, converMac6To12Byte(newProf->hardF), newProf->passwd, PPTP_SRV_ASS_IP);
		} else {
		doSystem("pptpSrvAccAdd.sh %s %s %s %s", newProf->userName, PPTP_SRV_NAME, newProf->passwd, PPTP_SRV_ASS_IP);
		}
#else
		doSystem("pptpSrvAccAdd.sh %s %s %s %s", newProf->userName, PPTP_SRV_NAME, newProf->passwd, PPTP_SRV_ASS_IP);
#endif /*HARD_FEATURE*/
	    }
	    else
	    {
		creatPptpBindFile();
		addr.s_addr = newProf->bindIp;
#if (HARD_FEATURE == FYES)
		if(strcmp(converMac6To12Byte(newProf->hardF), "000000000000")!=0) {
		doSystem("pptpSrvAccAdd.sh %s %s %s%s %s", newProf->userName, PPTP_SRV_NAME, converMac6To12Byte(newProf->hardF), newProf->passwd, inet_ntoa(addr));
		} else {
		doSystem("pptpSrvAccAdd.sh %s %s %s %s", newProf->userName, PPTP_SRV_NAME, newProf->passwd, inet_ntoa(addr));
		}
#else
		doSystem("pptpSrvAccAdd.sh %s %s %s %s", newProf->userName, PPTP_SRV_NAME, newProf->passwd, inet_ntoa(addr));
#endif /*HARD_FEATURE*/
	    }
#else
#if (HARD_FEATURE == FYES)
	    if(strcmp(converMac6To12Byte(newProf->hardF), "000000000000")!=0) {
	    doSystem("pptpSrvAccAdd.sh %s %s %s%s %s", newProf->userName, PPTP_SRV_NAME, converMac6To12Byte(newProf->hardF), newProf->passwd, PPTP_SRV_ASS_IP);
	    } else {
	    doSystem("pptpSrvAccAdd.sh %s %s %s %s", newProf->userName, PPTP_SRV_NAME, newProf->passwd, PPTP_SRV_ASS_IP);
	    }
#else
	    doSystem("pptpSrvAccAdd.sh %s %s %s %s", newProf->userName, PPTP_SRV_NAME, newProf->passwd, PPTP_SRV_ASS_IP);
#endif /*HARD_FEATURE*/
#endif

	    break;
        case PROF_CHANGE_EDIT:
	    /*
	     * ��ɾ����Ӧ�˺ţ�����Ӷ�Ӧ�˺�
	     */	    
	    PPTP_PRINT("%s PROF_CHANGE_EDIT-oldProf: user=%s\n", 
		    __FUNCTION__, oldProf->userName);
	    PPTP_PRINT("%s PROF_CHANGE_EDIT-newProf: user=%s, srvName=%s, passwd=%s, assip=%s\n", 
		    __FUNCTION__, newProf->userName, PPTP_SRV_NAME, newProf->passwd, PPTP_SRV_ASS_IP);

	    killOnePptpClient(oldProf->userName);
	    doSystem("pptpSrvAccDel.sh %s %s", oldProf->userName, PPTP_SRV_NAME);
	    if (newProf->head.active == 1) {
#if (PPTP_IP_BIND == FYES)
	    creatPptpBindFile();
	    if(newProf->bindIp == 0)
	    {
#if (HARD_FEATURE == FYES)
		if(strcmp(converMac6To12Byte(newProf->hardF), "000000000000")!=0) {
		doSystem("pptpSrvAccAdd.sh %s %s %s%s %s", newProf->userName, PPTP_SRV_NAME, converMac6To12Byte(newProf->hardF), newProf->passwd, PPTP_SRV_ASS_IP);
		} else {
		doSystem("pptpSrvAccAdd.sh %s %s %s %s", newProf->userName, PPTP_SRV_NAME, newProf->passwd, PPTP_SRV_ASS_IP);
		}
#else
		doSystem("pptpSrvAccAdd.sh %s %s %s %s", newProf->userName, PPTP_SRV_NAME, newProf->passwd, PPTP_SRV_ASS_IP);
#endif /*HARD_FEATURE*/
	    }
	    else
	    {
		addr.s_addr = newProf->bindIp;
#if (HARD_FEATURE == FYES)
		if(strcmp(converMac6To12Byte(newProf->hardF), "000000000000")!=0) {
		doSystem("pptpSrvAccAdd.sh %s %s %s%s %s", newProf->userName, PPTP_SRV_NAME, converMac6To12Byte(newProf->hardF), newProf->passwd, inet_ntoa(addr));
		} else {
		doSystem("pptpSrvAccAdd.sh %s %s %s %s", newProf->userName, PPTP_SRV_NAME, newProf->passwd, inet_ntoa(addr));
		}
#else
		doSystem("pptpSrvAccAdd.sh %s %s %s %s", newProf->userName, PPTP_SRV_NAME, newProf->passwd, inet_ntoa(addr));
#endif /*HARD_FEATURE*/
	    }
#else
#if (HARD_FEATURE == FYES)
	    if(strcmp(converMac6To12Byte(newProf->hardF), "000000000000")!=0) {
	    doSystem("pptpSrvAccAdd.sh %s %s %s%s %s", newProf->userName, PPTP_SRV_NAME, converMac6To12Byte(newProf->hardF), newProf->passwd, PPTP_SRV_ASS_IP);
	    } else {
	    doSystem("pptpSrvAccAdd.sh %s %s %s %s", newProf->userName, PPTP_SRV_NAME, newProf->passwd, PPTP_SRV_ASS_IP);
	    }
#else
	    doSystem("pptpSrvAccAdd.sh %s %s %s %s", newProf->userName, PPTP_SRV_NAME, newProf->passwd, PPTP_SRV_ASS_IP);
#endif /*HARD_FEATURE*/
#endif
	    }

	    break;
        case PROF_CHANGE_DEL:
	    /*
	     * �����ļ�ɾ����Ӧ����
	     */
	    PPTP_PRINT("%s PROF_CHANGE_DEL-oldProf: user=%s\n", 
		    __FUNCTION__, oldProf->userName);

	    killOnePptpClient(oldProf->userName);
	    doSystem("pptpSrvAccDel.sh %s %s", oldProf->userName, PPTP_SRV_NAME);
#if (PPTP_IP_BIND == FYES)
	    creatPptpBindFile();
#endif

	    break;
        case PROF_CHANGE_DELALL:
	    /*
	     * �����ļ�ɾ�������˺�
	     */
	    PPTP_PRINT("%s PROF_CHANGE_DELALL\n", __FUNCTION__);
	    killallPptpClient();
	    doSystem("pptpSrvAccDelAll.sh %s", PPTP_SRV_NAME);
#if (PPTP_IP_BIND == FYES)
	    creatPptpBindFile();
#endif

	    break;
        default:
	    break;
    }
}

#if (PPTP_IP_BIND == FYES)
static void creatPptpBindFile(void)
{
    MibProfileType mibType = MIB_PROF_PPTP_SRV_ACCOUNT;
    PPTPSrvAccProfile *profPptp = NULL;
    struct in_addr addr;
    int i = 0, min = 0, max = 0;
    /* ���pppoe��IP��¼�ļ� */
    doSystem("cp /dev/null %s -f", PPTP_SERVER_BIND_IP_FILE);
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) 
    {
	profPptp = (PPTPSrvAccProfile *)ProfGetInstPointByIndex(mibType, i);
	if((profPptp != NULL) && (ProfInstIsFree(profPptp) == 0)) 
	{
	    if(profPptp->bindIp != 0)
	    {
		addr.s_addr = profPptp->bindIp;
		doSystem("echo %s >> %s &", inet_ntoa(addr), PPTP_SERVER_BIND_IP_FILE);
	    }
	}
    }
    return;
}
#endif
/*
 * ��ʼ������
 */
extern void funInitPptpServer(void) {
    registerForProfileUpdates(MIB_PROF_PPTP_SRV_GLOBAL, NULL, pptpSrvGlobalProfileChange);
    registerForProfileUpdates(MIB_PROF_PPTP_SRV_ACCOUNT, NULL, pptpSrvAccountProfileChange);
    pptpServerInit();

    return;
}
#endif
