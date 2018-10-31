#include "uttMachine.h"
#include	<stdlib.h>
#include    <stdio.h>
#include    <fcntl.h>
#include    <unistd.h>
#include	<sys/ioctl.h>
#include	<net/route.h>
#include    <string.h>
#include    <dirent.h>
#include    <sys/time.h>
#include	"internet.h"
#include	"webs.h"
#include	"utils.h"
#include 	"firewall.h"
#include	"management.h"
#include	"station.h"
#include	"uttfunction.h"
#include    "linux/mii.h"
#include    "linux/sockios.h"
#include	<sys/sysinfo.h>
#include	"translate.h"

/*
 * kernel config
 */
#include	"linux/autoconf.h"
#ifndef CONFIG_RALINK_RT2880
#include	<net/if.h>
#endif
/*
 * user config
 */
#include	"config/autoconf.h" 

#include	"utt_pppoeClient.h" 

#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"argcmd.h"
#include	"wuScript.h"
#include	"comExeFun.h"

#define EXPORT_IMPORT_NOT_CONFIGURED_STRING "*"
#define MAX_PPPOE_ACCOUNT_INFO_LEN 32

#if (PPPOE_SERVER == FYES)
#if (PPPOE_SELF_HELP == FYES)
#define SELF_HELP_MSG_LEN   60
#endif

#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)	
#else
#define SWORD_PRINTF(fmt, args...)
#endif

#ifdef PPPOE_SRV_DBG
#define PPPOE_SRV_PRINTF(fmt, args...)		printf(fmt, ## args)	
#else
#define PPPOE_SRV_PRINTF(fmt, args...)
#endif

#ifdef CONFIG_UTT_NETFILTER
extern int uttNfNlSock;
#endif

#if (IP_GRP == FYES)
extern IpGrpProfile* userRefByIpGrp(char *username, ipGrpEntryType userType);
#endif

extern const LcdParamEnumValue pppAuthEnum[];
extern char* converMac6To12Byte(MacAddr macAddr);
extern int converMac12To6Byte(char *macStr, MacAddr* macAddr);
static int getAddreGrpList(int eid, webs_t wp, int argc, char_t **argv);
static int editShelfSleep(int index, webs_t wp);

#if (IP_GRP == FYES)
static int UserGroupConfigAdd(char_t *UserGroupName, char_t *user);
static int getUserGrpList(int eid, webs_t wp, int argc, char_t **argv);
#endif


#if (IP_GRP == FYES)
/********************************************************************
 * ������ pppoeAccGrpIsFull
 * ���ܣ� �ж�PPPOE�˺����Ƿ�����
 * ������ 2013-09-23
 * ������ Ҫ�жϵ�PPPOE�˺�����
 * ���أ� 1->��������2->��δ��
 * ����� ��
 * ���ߣ� zhao.chong 
 ********************************************************************/ 
static int pppoeAccGrpIsFull(char_t *UserGroupName)
{
    IpGrpProfile *prof;
    int i = 0, ret = 0, flag = 0;

    prof = (IpGrpProfile *)ProfGetInstPointByName(MIB_PROF_IPGRP, UserGroupName);
    if((ProfInstIsFree(prof) == 0) && (prof->grpType == IPGRP_USER)) {
        for(i = 0; i < IPGRP_USER_ENTRYS; i++) {
            if(prof->ipEntry[i].entryType == IPENT_NONE) {/*����ֻҪ�и���Ŀû��ʹ�ã���ʾ��δ��*/
                flag = 1;/*falg==1��ʾ��δ��*/
                break;
            }
        }
    }
    if (flag == 0) {
        ret = 1;
    }

    return ret;
}

/********************************************************************
 * ������ UserGroupConfigAdd
 * ���ܣ� ���û�������û�
 * ������ 2012-07-30
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int UserGroupConfigAdd(char_t *UserGroupName, char_t *user)
{
    IpGrpProfile *prof;
    int i = 0, ret = 1, flag = 0;
    prof = (IpGrpProfile *)ProfGetInstPointByName(MIB_PROF_IPGRP, UserGroupName);
    const char *ErrMsg = NULL;

    /*�˺���*/
    if((ProfInstIsFree(prof) == 0) && (prof->grpType == IPGRP_USER))
    {
        for(i = 0; i < IPGRP_USER_ENTRYS; i++)
        {
            if((prof->ipEntry[i].entryType==IPENT_POE_ACC) && ((strcmp(prof->ipEntry[i].entryVal.name, user)) == 0))
            {
                ret = 0;
                ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_EXIT_IN_GROUP);
                setTheErrorMsgOnFree(ErrMsg);
                break;
            }
        }
        if(i == IPGRP_USER_ENTRYS)
        {
            for(i = 0; i < IPGRP_USER_ENTRYS; i++)
            {
                if(prof->ipEntry[i].entryType == IPENT_NONE)
                {
                    flag = 1;
                    prof->ipEntry[i].entryType = IPENT_POE_ACC;
                    strcpy(prof->ipEntry[i].entryVal.name, user);
                    break;
                }
            }
            SWORD_PRINTF("%s - %d, flag = %d\n",__FUNCTION__, __LINE__, flag);
            if(flag == 0)
            {
                ret = 0;
                ErrMsg = getTransDataForC(C_LANG_INDEX_USER_GROUP_LIST_MAX);
                setTheErrorMsgOnFree(ErrMsg);
            }
        }
    }
    return ret;
}
/********************************************************************
 * ������ getIpGrpList
 * ���ܣ� ��ҳ������û���
 * ������ 2012-05-10
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int getUserGrpList(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_IPGRP;
    IpGrpProfile *prof = NULL;

    int min = 0, max = 0;
    int i = 0;
    int num = 0;

    websWrite(wp,"var UserGroupNames = new Array();");

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) 
    {
        prof = (IpGrpProfile*)ProfGetInstPointByIndex(mibType, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0)) 
        {
            if(prof->grpType == IPGRP_USER)
            {
                websWrite(wp,"UserGroupNames[%d] = \"%s\";",num,prof->head.name);
                num++;
            }
        }
    }
    return 0;
}
#endif

#if 0
/*
 * ɾ��ָ���û�pppoe ����
 */
static void killOnePppoeClient(char *user)
{
    PPPOE_SRV_PRINTF("%s: user=%s\n", __FUNCTION__, user);
    int fd = -1;
    struct st_uttPppdClient info;
    int st_size = sizeof(struct st_uttPppdClient);
    fd = open(UTT_PPPOE_CLIENT_INFO_FILE, O_RDONLY);
    if(fd != -1) {
        while(read(fd, &info, st_size) > 0) {
            PPPOE_SRV_PRINTF("%s: user=%s,pid=%d,STime=%d,LTime=%d,ConTime=%d,rx=%lld,tx=%lld\n",
                    __FUNCTION__, info.user, info.pid, info.STime,info.LTime,info.ConTime,
                    info.rxByte,info.txByte);
            PPPOE_SRV_PRINTF("mac=%s, ip=%s, ifname=%s, devie=%s\n", info.mac, info.ip,
                    info.ifname,info.device);
            if(strncmp(user, info.user, UTT_PPP_CLIENT_NAME_LEN) == 0) {
                doSystem("kill -INT %d", info.pid);
                break;
            }
        }
        close(fd);
    }
    return;
}
#endif
#if 0
/********************************************************************
 * ������ killOnePppoeClient 
 * ���ܣ� ɾ��ָ���û�pppoe ����
 * ������ 2012-03-28
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
#if 0
static void killOnePppoeClient(char *user)
#else
static void killOnePppoeClient(PPPoESrvAccProfile* info)
#endif
{
#if 0
    int max = 0;
    int min = 0;
    int i = 0;

    MibProfileType mibType = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *info = NULL;

    PPPOE_SRV_PRINTF("%s: user=%s\n", __FUNCTION__, user);
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
            SWORD_PRINTF("%s-%d: user=%s,pid=%d,STime=%d,LTime=%d,ConTime=%d,rx=%lld,tx=%lld\n",
                    __FUNCTION__, __LINE__, info->pppoename, info->pid, info->STime,info->LTime,info->ConTime,
                    info->rxByte,info->txByte);
            SWORD_PRINTF("mac=%s, ip=%s, ifname=%s, devie=%s\n", info->mac, info->ip,
                    info->ifname,info->device);
            if(strncmp(user, info->pppoename, UTT_PPPOE_NAME_LEN) == 0) 
            {
                doSystem("kill -INT %d", info->pid);
                break;
            }
        }
    }
#else
    if(info->ConTime == 0)
    {/*����*/
#if (PPPOE_ACCOUNT == FYES)
        /*����Ӧע����Ϊ ɾ���������� �ͻ������ߵĻ�
          �� pppd�����������Ҳ�����Ӧ��profile��
          �����ڴ˴���
          �༭����Ҳ�д˿��ܡ�
          */
        if(info->chargeType !=  NONE_CHARG_MODE)
        {/*�����˼Ʒ�*/
            upPppAccRule(info, info->expired, 0);/*ɾ��ԭ���Ĺ���*/
        }
#endif
#ifdef CONFIG_UTT_NETFILTER
        uttNfNlHashDelIpStr(uttNfNlSock, info->ip);/*ɾ��hash���Ӧ��ip*/
#endif
        doSystem("kill -INT %d", info->pid);
    }
#endif

#if 0
    /*
     *�˴�Ϊ���źŽ���
     */
    uttSemV(SEM_PPPOE_NO,0);
#endif

    return;
}
#else
/********************************************************************
 * ������ killOnePppoeClient
 * ���ܣ� ѭ�������˺Ŷ�Ӧ�����лỰ������������
 * ������ 2012-08-24
 * ������ 
 *	  info     -    �˺Ŷ�Ӧprofile
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/
static void killOnePppoeClient(PPPoESrvAccProfile* info)
{
    MibProfileType mibType = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *prof = NULL;
    int i = 0, j = 0, k = 0;
#if (PPPOE_LIMIT == FYES)
    unsigned int index = 0U;
#endif

    prof = (PPPoESrvGloProfile*)ProfGetInstPointByIndex(mibType, 0);
    if(prof != NULL)
    {
        j = info->sesIndex;
        SWORD_PRINTF("%s - %d, j = %d\n",__FUNCTION__, __LINE__, j);
        if(SesIndexIsOk(j) == 1)
        {
            for(i = 0; i < info->actualSession; i++)
            {
                SWORD_PRINTF("%s - %d, j = %d, nvramProfile->Session[j].ip = %s\n",__FUNCTION__, __LINE__, j, nvramProfile->Session[j].ip);
                if(nvramProfile->Session[j].ConTime == 0U)
                {/*����*/
#if (IP_GRP == FYES)
                    userUpToIpGrp(info->pppoename, nvramProfile->Session[j].ip, IPENT_POE_ACC, IPSET_MEM_DEL);
#endif
#if (PPPOE_ACCOUNT == FYES)
                    if(info->chargeType !=  NONE_CHARG_MODE)
                    {/*�����˼Ʒ�*/
                        upPppAccRule(nvramProfile->Session[j], info->expired, 0);/*ɾ��ԭ���Ĺ���*/
                    }
#endif
#if (PPPOE_LIMIT == FYES)
                    if ((info->uRateLimit != 0U) || (info->dRateLimit != 0U))
                    {
                        for (index=0U;index<MAX_PPPOE_LIMIT_IPGRP;index++)
                        {
                            if ((prof->limitGroup[index].uRateLimit == info->uRateLimit) && (prof->limitGroup[index].dRateLimit == info->dRateLimit))/*ͨ���Ƚ������ҵ�����*/
                            {
                                ipsetAOrDIpStr(prof->limitGroup[index].ipGrpName, nvramProfile->Session[j].ip, IPSET_MEM_DEL);/*����PPPoE������Ӧ��ipset��*/
                                ipsetAOrDIpStr(POE_SKIP_FIXED_LIMIT, nvramProfile->Session[j].ip, IPSET_MEM_DEL);/*����������ϸ������ipset��*/
                            }
                        }
                    }
#endif
#ifdef CONFIG_UTT_NETFILTER
                    uttNfNlHashDelIpStr(uttNfNlSock, nvramProfile->Session[j].ip);/*ɾ��hash���Ӧ��ip*/
#endif
                    nvramProfile->Session[j].ConTime = 1U;
                    nvramProfile->Session[j].accIndex = -1;
                    doSystem("kill -INT %d", nvramProfile->Session[j].pid);
                }
                k = j;
                j = nvramProfile->Session[j].nextIndex;
                nvramProfile->Session[k].nextIndex = -1;
                if(SesIndexIsOk(j) == 0)
                {
                    break;
                }
            }
        }
    }
    return;
}
#endif

#if 0
/*
 * ɾ������pppoe �������˺Ų����˺�
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
            if (strcmp(lanDev, info.device) == 0) {
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
#endif
/********************************************************************
 * ���ܣ� ѭ�������˺Ŷ�Ӧ�����лỰ�������� ����*
 * ������ 2013-08-16
 * ������ 
 *	  info     -    �˺Ŷ�Ӧprofile
 * ���أ� ��
 * ����� ��
 * ���ߣ� bhou
 ********************************************************************/
static void delAllSesRule(PPPoESrvAccProfile* info)
{
    int i = 0, j = 0;

    j = info->sesIndex;
    if(SesIndexIsOk(j) == 1)
    {
        for(i = 0; i < info->actualSession; i++)
        {
            if(nvramProfile->Session[j].ConTime == 0U)
            {/*����*/
                upPppAccRule(nvramProfile->Session[j], info->expired, 0);/*ɾ��ԭ���Ĺ���*/
                j = nvramProfile->Session[j].nextIndex;
                if(SesIndexIsOk(j) == 0)
                {
                    break;
                }
            }
        }
    }
    return;
}

/*
 * �ж��Ƿ���ڲ��ڵ�ַ���ڵĹ̶�IP�����ַ
 * TRUE  - ����
 * FALSE - ������
 * ipS ipEndΪ�����ֽ���
 */
static int existNonPoolAssignIp(Uint32 ipS, Uint32 ipE) {
    MibProfileType mibTypeA = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *profA = NULL;
    int min = 0, max = 0, i = 0;
    Uint32 ip;

    ProfInstLowHigh(mibTypeA, &max, &min);

    /*
     *�˴�Ϊ�����ź��������˲���
     */
    uttSemP(SEM_PPPOE_NO,0);

    for (i = min; i < max; i++) {
        profA = (PPPoESrvAccProfile *)ProfGetInstPointByIndex(mibTypeA, i);
        if((profA != NULL) && (ProfInstIsFree(profA) == 0) && (profA->bindIp != 0)) {
            ip = ntohl(profA->bindIp);
            if ((ip < ipS) || (ip >= ipE)) {
                /*
                 * �˴�Ϊ���źŽ���
                 */
                uttSemV(SEM_PPPOE_NO,0);
                return TRUE;
            }
        }
    }

    /*
     *�˴�Ϊ���źŽ���
     */
    uttSemV(SEM_PPPOE_NO,0);

    return FALSE;
}

/*
 * ȫ������ҳ�汣��
 */
static void formConfigPppoeServGlobale(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;

    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;

    struct in_addr ipS, pDns, sDns;
    Uint32 cnt;
    ProfPppAuthEnum mode;
    IpPoolEnum cfPoolType = IP_POOL_NONE;
    Uint32 lanIp;
#if (MULTI_LAN == FYES)
    Uint32 lanIp2, lanIp3, lanIp4;
#endif

    ProfChangeType profAction;
    struProfAlloc *profhead = NULL;
    const char *ErrMsg = NULL;

    char *pppoeEn, *startIp, *priDns, *secDns, *authType, *superveneCount, *onlyPppoe, *pexceptIpGroup;
#if (PPPOE_SELF_HELP == FYES)
    char *selfhelpEn=NULL;
    FunEnableEnum enable = FUN_DISABLE;
#endif

#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
    Uint32 ip_count;
    char *ipcount;
#endif
    pppoeEn = websGetVar(wp, "lanPPPoETypew", T(""));
    onlyPppoe = websGetVar(wp, "PPPoEOnly", T(""));
    pexceptIpGroup = websGetVar(wp, "exceptIpGroup", T(""));
    startIp = websGetVar(wp, "pppoeStart", T(""));
    priDns = websGetVar(wp, "pppoePriDns", T(""));
    secDns = websGetVarNull(wp, "pppoeSecDns", T("0.0.0.0"), T("0.0.0.0"));
    authType = websGetVar(wp, "vailtype", T(""));
    superveneCount = websGetVar(wp, "smaxconv", T(""));
#if (PPPOE_SELF_HELP == FYES)
    selfhelpEn = websGetVar(wp, "selfHelpEnable", T(""));
#endif

#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
    ipcount = websGetVar(wp, "ipcount", T(""));
#endif

    PPPOE_SRV_PRINTF("%s:enabled=%s,ipS=%s,pDns=%s,sDns=%s,authType=%s,maxSess=%s,exceptIpGroup=%s\n", 
            __FUNCTION__, pppoeEn, startIp, priDns, secDns, authType, superveneCount,pexceptIpGroup);

    if ((startIp == NULL) || (priDns == NULL) || (secDns == NULL) || (authType == NULL) 
            || (superveneCount == NULL)) {
        ErrMsg = getTransDataForC(C_LANG_INDEX_CINFIG_PARAMETER_WRONG);
        setTheErrorMsg(ErrMsg);
        goto out;
    }

    if (0 == inet_aton(startIp, &ipS)) {
        ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_START_IP_ADDR);
        setTheErrorMsg(ErrMsg);
        goto out;
    }
   
    if (0 == inet_aton(priDns, &pDns)) {
        ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_MAIN_DNS_SERVER);
        setTheErrorMsg(ErrMsg);
        goto out;
    }

    if (strlen(secDns) != 0) {
        if (0 == inet_aton(secDns, &sDns)) {
            ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_SEC_DNS_SERVER);
            setTheErrorMsg(ErrMsg);
            goto out;
        }
    } else {
        sDns.s_addr = 0;
    }

    mode = getEnumValue(pppAuthEnum, authType);
    if (mode == -1) {
        ErrMsg = getTransDataForC(C_LANG_INDEX_USELESS_PASSWD_CHECK_ERR);
        setTheErrorMsg(ErrMsg);
        goto out;
    }

    cnt = (Uint32)strtol(superveneCount, NULL, 10);
    if (cnt == 0) {
        ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_SYS_MAX_TALK);
        setTheErrorMsg(ErrMsg);
        goto out;
    }
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
    ip_count = (Uint32)strtol(ipcount, NULL, 10);
    if (0 == ip_count){
        setTheErrorMsg("��Ч��ַ����");
        goto out;
    }
    if (ip_count > MAX_PPPOE_SRV_ACC_PROFILES || ip_count < cnt) {
        setTheErrorMsg("��ַ�������벻�����ʺ���������С��ϵͳ���Ự��");
        goto out;
    }
#endif
     
    /*
     * mantis 0008715
     */
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
    if (existNonPoolAssignIp(ntohl(ipS.s_addr), ntohl(ipS.s_addr) + ip_count) == TRUE) {
#else
	if (existNonPoolAssignIp(ntohl(ipS.s_addr), ntohl(ipS.s_addr) + cnt) == TRUE) {
#endif
        ErrMsg = getTransDataForC(C_LANG_INDEX_EXIT_FIX_IP_IN_POOL);
        setTheErrorMsg(ErrMsg);
        goto out;
    }

    /*
     * mantis 8438:lan��ip���ܰ����ڵ�ַ����
     */
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
    if (profIF != NULL) {
        lanIp = ntohl(profIF->ether.sta.ip);
#if (MULTI_LAN == FYES)
        lanIp2 = ntohl(profIF->ether.sta.ip2);
        lanIp3 = ntohl(profIF->ether.sta.ip3);
        lanIp4 = ntohl(profIF->ether.sta.ip4);
#endif

#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
        if (((ntohl(ipS.s_addr) <= lanIp) && ((ntohl(ipS.s_addr) + ip_count) >= lanIp)) 
#else
        if (((ntohl(ipS.s_addr) <= lanIp) && ((ntohl(ipS.s_addr) + cnt) >= lanIp)) 
#endif

#if (MULTI_LAN == FYES)

#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
                || ((ntohl(ipS.s_addr) <= lanIp2) && ((ntohl(ipS.s_addr) + ip_count) >= lanIp2)) 
                || ((ntohl(ipS.s_addr) <= lanIp3) && ((ntohl(ipS.s_addr) + ip_count) >= lanIp3)) 
                || ((ntohl(ipS.s_addr) <= lanIp4) && ((ntohl(ipS.s_addr) + ip_count) >= lanIp4)) )
#else
                || ((ntohl(ipS.s_addr) <= lanIp2) && ((ntohl(ipS.s_addr) + cnt) >= lanIp2)) 
                || ((ntohl(ipS.s_addr) <= lanIp3) && ((ntohl(ipS.s_addr) + cnt) >= lanIp3)) 
                || ((ntohl(ipS.s_addr) <= lanIp4) && ((ntohl(ipS.s_addr) + cnt) >= lanIp4)) )
#endif

#endif
        {
            ErrMsg = getTransDataForC(C_LANG_INDEX_LAN_IP_CANT_IN_PPPoE_POOL);
            setTheErrorMsg(ErrMsg);
            goto out;
        }
    }

    /*
     * �жϵ�ַ��
     */
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
    cfPoolType = ipPoolConflict(IP_POOL_PPPOE, ipS.s_addr, htonl(ntohl(ipS.s_addr) + ip_count));
#else
    cfPoolType = ipPoolConflict(IP_POOL_PPPOE, ipS.s_addr, htonl(ntohl(ipS.s_addr) + cnt));
#endif

    if (cfPoolType != IP_POOL_NONE) {
        setPoolConflictMsg(cfPoolType);
        goto out;
    }
    profAction = PROF_CHANGE_EDIT;
    ProfBackupByIndex(mibTypeS, profAction, 0, &profhead);

    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS, 0);
    if (profS == NULL) {
        PPPOE_SRV_PRINTF("%s: get pppoe srv prof error!\n", __FUNCTION__);
        goto out;
    }

    if(strcmp(pppoeEn,"on") == 0) {
        profS->head.active = 1;
    } else {
        profS->head.active = 0;
#if 0
        /* ��profile change�е��� */
        killallPppoeClient();
#endif
    }

    profS->onlyPppoe = (Uint32)strtol(onlyPppoe,NULL,10);
    /*��ַ�����ĳ��Ȳ��ܴ���IP_GROUP_NAME_LEN*/
    memset( profS->exceptIpGroup, 0, IP_GROUP_NAME_LEN );
    PPPOE_SRV_PRINTF("==== %s, %d.\n ====exceptIpGroup:%s\n", __FUNCTION__, __LINE__, pexceptIpGroup);
    if( 1 == profS->onlyPppoe )
    {
        strncpy(profS->exceptIpGroup,pexceptIpGroup, IP_GROUP_NAME_LEN);
        PPPOE_SRV_PRINTF("==== %s, %d.\n ====profS->exceptIpGroup_save:%s\n", __FUNCTION__, __LINE__, profS->exceptIpGroup);
    }
    profS->ipStart = ipS.s_addr;
    profS->pDns = pDns.s_addr;
    profS->sDns = sDns.s_addr;
    profS->pppAuthMode = mode;
    profS->maxSessCnt = cnt;
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
    profS->ipcount = ip_count;
#endif

#if (PPPOE_SELF_HELP == FYES)
    if(strcmp(selfhelpEn, "on")==0)
    {
        enable=FUN_ENABLE;
    }
    if(profS->self_help != enable)
    {
        profS->self_help=enable;
        PPPOE_SRV_PRINTF("%s-%d: set profS->self_help = %d\n",__FUNCTION__,__LINE__,profS->self_help);
    }
#endif
#if 0
    nvram_bufset(RT2860_NVRAM, _CONF_PPPOE_S_ENABLE, pppoeEn);
    nvram_bufset(RT2860_NVRAM, _CONF_PPPOE_S_STARTIP, startIp);
    nvram_bufset(RT2860_NVRAM, _CONF_PPPOE_S_CLIENTCOUNT, clientCount);
    nvram_bufset(RT2860_NVRAM, _CONF_PPPOE_S_PRIDNS, priDns);
    nvram_bufset(RT2860_NVRAM, _CONF_PPPOE_S_SECDNS, secDns);
    nvram_bufset(RT2860_NVRAM, _CONF_PPPOE_S_AUTHTYPE, authType);
    nvram_bufset(RT2860_NVRAM, _CONF_PPPOE_S_ONLINECOUNT, superveneCount);
#endif

#if 0
    nvram_commit(RT2860_NVRAM);
    sendMsgForSpeed(INIT_PPPOE_SERVER, 0, 0, NULL, 0);
#endif
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
    websRedirect(wp, "PPPoEGlobal.asp");
    return ;
}

/*
 * ȫ������ҳ�����
 */
static int aspGetPppoeServGlobale(int eid, webs_t wp, int argc, char_t **argv)
{
#if 0
    websWrite(wp, "var lanPPPoETypes=\"%s\";", nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_ENABLE));
    websWrite(wp, "var pppoeStarts=\"%s\";", nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_STARTIP));
    websWrite(wp, "var ipcounts=\"%s\";", nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_CLIENTCOUNT));
    websWrite(wp, "var pppoePriDnss=\"%s\";", nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_PRIDNS));
    websWrite(wp, "var pppoeSecDnss=\"%s\";", nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_SECDNS));
    websWrite(wp, "var vailtypes=\"%s\";", nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_AUTHTYPE));
    websWrite(wp, "var smaxconvs=\"%s\";", nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_ONLINECOUNT));
#endif
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;

    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS, 0);
    if (profS == NULL) {
        PPPOE_SRV_PRINTF("%s: get profile error\n", __FUNCTION__);
        return 0;
    }

    websWrite(wp, "var lanPPPoETypes= %d;", profS->head.active);
    websWrite(wp, "var onlyPppoes= %d;", profS->onlyPppoe);
    PPPOE_SRV_PRINTF("==== %s, %d.\n ====profS->exceptIpGroup_read:%s\n", __FUNCTION__, __LINE__, profS->exceptIpGroup);
    websWrite(wp, "var exceptIpGroups=\"%s\";", profS->exceptIpGroup);
    websWrite(wp, "var pppoeStarts=\"%N\";", profS->ipStart);
    websWrite(wp, "var pppoePriDnss=\"%N\";", profS->pDns);
    websWrite(wp, "var pppoeSecDnss=\"%N\";", profS->sDns);
    websWrite(wp, "var vailtypes=\"%s\";", getEnumString(pppAuthEnum, profS->pppAuthMode));
    websWrite(wp, "var smaxconvs=\"%d\";", profS->maxSessCnt);
    websWrite(wp, "var maxconv=\"%d\";", MAX_PPPOE_SRV_SESSION_PROFILES);
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
    websWrite(wp, "var showIpcount=\"%s\";", T("yes"));
    websWrite(wp, "var ipcounts=\"%d\";", profS->ipcount);
#else
    websWrite(wp, "var showIpcount=\"%s\";", T("no"));
#endif
#if (PPPOE_SELF_HELP == FYES)
    if(profS->self_help == FUN_ENABLE)
    {
        websWrite(wp, "var selfHelpEnable=\"%s\";", T("on"));
    }
    else
    {
        websWrite(wp, "var selfHelpEnable=\"%s\";", T("off"));
    }
#else
    websWrite(wp, "var selfHelpEnable=\"null\";");
#endif
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

#if 0
static int _pppoeUserCount;
static void _addPppoeUser(char *user, char *passwd, char *aspip, char *list, int tempLen)
{
    char *newCList;

    if(strnmatch(list, user, 0) != NULL) {
        setTheErrorMsg(T("�û��������ظ�!"));
    }else if(strnmatch(list, aspip, 2) != NULL){
        setTheErrorMsg(T("�̶�IP��ַ�����ظ���"));
    }else {
        newCList = (char*)malloc(sizeof(char) * tempLen);
        if( newCList != NULL) {
            memset(newCList, 0, tempLen);
            strcpy(newCList, list);
            if(strlen(newCList) != 0) {
                strcat(newCList, UTT_SPLIT_SIGN_RECORDS);
            } else {
                ;
            }
            strcat(newCList,user);
            strcat(newCList,UTT_SPLIT_SIGN_STR);
            strcat(newCList,passwd);
            if(strlen(aspip)<6){
                strcpy(aspip,"*");
            }
            strcat(newCList,UTT_SPLIT_SIGN_STR);
            strcat(newCList,aspip);
            printf("bufset newClist[%s]\n", newCList);
            nvram_bufset(RT2860_NVRAM, _CONF_PPPOE_S_USER, newCList);
            free(newCList);
        } else {
            ;
        }
    }
}

static void _modifyPppoeUser(char *user, char *passwd, char *aspip, char *list, char *userOld, int tempLen)
{
    char *p, *q, c;
    char *newCList;
    p = strnmatch(list,userOld, 0);
    if(p == NULL) {
        setTheErrorMsg(T("�Ҳ������û��� !"));
    } else {
        q = p;
        while( (*q != UTT_SPLIT_SIGN_RECORD) && *q) {
            q++;
        }
        c=*p;
        *p = 0;
        if( strnmatch(list, user, 0) || strnmatch(q, user, 0)) {
            *p=c;
            setTheErrorMsg(T("�û��������ظ�!"));
        }else if( strnmatch(list, aspip, 2) || strnmatch(q, user, 0)) {
            *p=c;
            setTheErrorMsg(T("�̶�IP��ַ�����ظ�!"));
        } else {
            newCList = (char*)malloc(sizeof(char) * tempLen);
            if(newCList != NULL) {
                memset(newCList, 0, tempLen);
                strcpy(newCList, list);            //copy the string before UserNameOld
                strcat(newCList, user);
                strcat(newCList, UTT_SPLIT_SIGN_STR);
                strcat(newCList, passwd);
                if(strlen(aspip)<6){
                    strcpy(aspip,"*");
                }

                strcat(newCList, UTT_SPLIT_SIGN_STR);
                strcat(newCList, aspip);
                strcat(newCList,q);                             //copy the string after UserNameOld record
                nvram_bufset(RT2860_NVRAM, _CONF_PPPOE_S_USER, newCList);
                killOnePppoeClient(userOld);
                free(newCList);
            } else {
                ;
            }
        }
    }
}
#endif

#if 0
/*
 * �ж��Ƿ������˺ţ�IP��
 * ���������
 *  user    -	ҳ���ύ���˺�
 *  ip	    -	ҳ���ύ�İ�IP��ַ
 *
 * ����ֵ��
 *  1	-   ����
 *  0	-   ������
 */
static int allowIpUserBind(char *user, char *ip) {
    int fd = -1;
    int fileLen;
    struct st_uttPppdClient info;
    int rst = 1;
    int st_size = sizeof(struct st_uttPppdClient);
    off_t   start, end;

    if (strcmp(ip, "") == 0) {
        return rst;
    }

    fd = open(UTT_PPPOE_CLIENT_INFO_FILE, O_RDONLY);
    if (fd != -1) {
        start = lseek(fd, 0, SEEK_SET);
        end = lseek(fd, 0, SEEK_END);
        fileLen = end - start;
        lseek(fd, 0, SEEK_SET);

        while (fileLen >= st_size) {
            fcntl(fd, F_SETLKW, file_lock(F_WRLCK|F_RDLCK, SEEK_SET));
            read(fd, &info, st_size);
            fcntl(fd, F_SETLKW, file_lock(F_UNLCK, SEEK_SET));

            /* �ҵ�IP��ַ��Ӧ�Ĳ�����Ϣ */
            if ((info.ConTime == 0) && (strcmp(info.device, (char *)getLanIfName()) == 0)
                    && (strcmp(info.ip, ip) == 0)) {
                if (strcmp(user, info.user) != 0) {
                    rst = 0;
                }
                break;
            }
            fileLen -= st_size;
        }
        close(fd);
    }

    return rst;
}
#endif
#if 0
/********************************************************************
 * ������ allowIpUserBind 
 * ���ܣ� �ж��Ƿ������˺ţ�IP��
 * ������ 2012-03-28
 * ������ 
 *  user    -	ҳ���ύ���˺�
 *  ip	    -	ҳ���ύ�İ�IP��ַ
 * ���أ� 
 *  1	    -   ����
 *  0	    -   ������
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int allowIpUserBind(char *user, char *ip) 
{
    int max = 0;
    int min = 0;
    int i = 0;

    MibProfileType mibType = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *info = NULL;

    int rst = 1;

    if (strcmp(ip, "") == 0) {
        rst = 1;
    }

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

            /* �ҵ�IP��ַ��Ӧ�Ĳ�����Ϣ */
            if ((info->ConTime == 0U) && (strcmp(info->device, (char *)getLanIfName()) == 0)
                    && (strcmp(info->ip, ip) == 0)) 
            {
                if (strcmp(user, info->pppoename) != 0) 
                {
                    rst = 0;
                }
                break;
            }
        }
    }

    /*
     *�˴�Ϊ���źŽ���
     */
    uttSemV(SEM_PPPOE_NO,0);

    return rst;
}
#else
/********************************************************************
 * ������ allowIpUserBind 
 * ���ܣ� �ж��Ƿ������˺ţ�IP��
 * ������ 2012-08-24
 * ������ 
 *  user    -	ҳ���ύ���˺�
 *  ip	    -	ҳ���ύ�İ�IP��ַ
 * ���أ� 
 *  1	    -   ����
 *  0	    -   ������
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int allowIpUserBind(char *user, char *ip) 
{
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *prof = NULL;

    MibProfileType mibType = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *info = NULL;

    int i = 0;
    int rst = 1;
    struct in_addr pppoeIp;


    /*
     *�˴�Ϊ�����ź��������˲���
     */
    uttSemP(SEM_PPPOE_NO,0);
    prof = (PPPoESrvGloProfile*)ProfGetInstPointByIndex(mibTypeS, 0);

    if(prof != NULL)
    {
        if (strcmp(ip, "") != 0) 
        {
            inet_aton(ip, &pppoeIp);
            i = ntohl(pppoeIp.s_addr) - ntohl(prof->ipStart);
            if(SesIndexIsOk(i) == 1)
            {
                /* �ҵ�IP��ַ��Ӧ�Ĳ�����Ϣ */
                if ((nvramProfile->Session[i].ConTime == 0U) && (strcmp(nvramProfile->Session[i].device, (char *)getLanIfName()) == 0))
                {
                    info = (PPPoESrvAccProfile*)ProfGetInstPointByIndex(mibType, nvramProfile->Session[i].accIndex);
                    if (strcmp(user, info->pppoename) != 0) 
                    {
                        rst = 0;
                    }
                }
            }
        }

    }
    /*
     *�˴�Ϊ���źŽ���
     */
    uttSemV(SEM_PPPOE_NO,0);

    return rst;
}
#endif

#if 0
static void printWeb(void) {
    int i;
    printf("portMap:\n");
#if 0
    for (i = 0; i <= 2000; i++) {
        if (isPortMapSet(&(nvramProfile->pppoeInstRec.instRecMap), i)) {
            printf("1 ");
        } else {
            printf("0 ");
        }
        if ((i + 1)%8 == 0) {
            printf("\n");
        }
    }
#endif
    for (i = 0; i < PORTMAP_MAX; i++) {
        printf("0x%x ", nvramProfile->pppoeInstRec.instRecMap.content[i]);
        if ((i + 1)%8 == 0) {
            printf("\n");
        }
    }
    printf("instNameIndex = %d\n", nvramProfile->pppoeInstRec.instNameIndex);
}
#endif

#if (PPPOE_LIMIT == FYES)
/*
 * ����������������������ж��Ƿ��ܹ����
 */
static int checkPPPOEAccLimitNum(int uRate, int dRate, char *indexStr)
{
    MibProfileType mibTypeA = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *profA = NULL;
    int min = 0, max = 0;
    int i = 0, j = 0;
    int ret = 0;
    int index = 0;
    int uRates[MAX_PPPOE_LIMIT_IPGRP];/*�ϴ���������*/
    int dRates[MAX_PPPOE_LIMIT_IPGRP];/*������������*/

    if ((indexStr == NULL) || (indexStr[0] == '\0'))
    {
        index = -1;/*�û�����ȡʧ������Ÿ�Ϊ�����ܵ�����*/
    }
    else
    {
        index = ProfGetInstIndexByName(mibTypeA, indexStr);
    }

    /*����Ϊ0��������*/
    if ((uRate != 0) || (dRate != 0))
    {
        memset(uRates, 0, sizeof(uRates));
        memset(dRates, 0, sizeof(dRates));
        uRates[0] = uRate;
        dRates[0] = dRate;
        ProfInstLowHigh(mibTypeA, &max, &min);

        /*ͨ����������Ϸ��������������������*/
        for(i=min;i<max;i++) 
        {
            /*��ǰ�ṹ�岻�������*/
            if (i != index)
            {
                profA = (PPPoESrvAccProfile*)ProfGetInstPointByIndex(mibTypeA, i);
                if(ProfInstIsFree(profA) == 0) 
                {
                    /*����Ϊ0���������*/
                    if ((profA->uRateLimit != 0) || (profA->dRateLimit != 0))
                    {
                        /*���Ҽ���������Ϣ������*/
                        for (j=0;j<MAX_PPPOE_LIMIT_IPGRP;j++)
                        {
                            /*�ҵ���Ҫ�ı��������ͬ���飬�ɹ����˳�*/
                            if ((uRates[j] == profA->uRateLimit) && (dRates[j] == profA->dRateLimit))
                            {
                                break;
                            }
                            /*�ҵ����飬�ɹ����˳�*/
                            if ((uRates[j] == 0) && (dRates[j] == 0))
                            {
                                uRates[j] = profA->uRateLimit;
                                dRates[j] = profA->dRateLimit;
                                break;
                            }
                        }
                        /*�Ȳ���ͬ��Ҳû�п�λ�ã�ʧ��*/
                        if (j == MAX_PPPOE_LIMIT_IPGRP)
                        {
                            ret = 1;
                        }
                    }
                }
            }
        }
    }
    return ret;
}
#endif

/*
 * �û��˺�����ҳ��
 */
static void formConfigPppoeUserEdit(webs_t wp, char_t *path, char_t *query)
{

#if 0
    char *user, *passwd1, *passwd2, *action, *userOld, *list, *startip, *userIp, aspip[16]={0};
    unsigned int naspip,nstartip,maxip;
    int tempLen = 0;
    user = websGetVar(wp,T("pppoename"), T(""));
    passwd1 = websGetVar(wp,T("passwd1"), T(""));
    passwd2 = websGetVar(wp,T("passwd2"), T(""));
    userIp = websGetVar(wp,T("aspip"), T(""));
    strcpy(aspip, userIp);
    action = websGetVar(wp,T("Action"), T(""));
    list = nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_USER);
    startip = nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_STARTIP);
    naspip = ip2int(aspip);
    nstartip = ip2int(startip);
    maxip = nstartip + UTT_MAX_PPPOE_USER -1;
    if ((naspip != 0) && ((naspip < nstartip) || (naspip > maxip))) {
        setTheErrorMsg(T("�̶�IP��ַ����PPPoE��ַ����!"));
    }
    else{
        if(strncmp(passwd1, passwd2, MAX_UTT_PASSWD_LEN) == 0){
            tempLen=strlen(list)+strlen(aspip)+strlen(user)+strlen(passwd1)+4+16;
            if(!strncmp(action, "add", 3)) {
                /*
                 * add a new pppoe user
                 */
                if(_pppoeUserCount >= UTT_MAX_PPPOE_USER) {
                    setTheErrorMsg(T("�Ѵ������Ŀ��"));
                } else {
                    _addPppoeUser(user, passwd1, aspip, list, tempLen);
                }
            } else {
                /*
                 * edit one pppoe user
                 */
                userOld = websGetVar(wp, T("pppoenameold"), T("")); 
#if 0
                p = strmatch(CList,userOld);
                if( p == NULL) {
                    setTheErrorMsg(T("�Ҳ������û��� !"));
                } else {
                    _modifyPppoeUser(user, passwd1, aspip, list, userOld);
                }
#else
                _modifyPppoeUser(user, passwd1, aspip, list, userOld, tempLen);
#endif
            }
        } else {
            setTheErrorMsg(T("�������벻��ͬ"));
        }
    }
    nvram_commit(RT2860_NVRAM);
    sendMsgForSpeed(INIT_PPPOE_SERVER_USER_FILE, 0, 0, NULL, 0);
#endif
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;

    MibProfileType mibTypeA = MIB_PROF_PPPOE_SRV_ACC;
    struProfAlloc *profList  = NULL;
#if (IP_GRP == FYES)
    IpGrpProfile* referedProf = NULL;
    char referedMsg[50];
#endif
    PPPoESrvAccProfile *profA = NULL, profBak;
    ProfChangeType profAction;
    int instIndex = 0;
    int spFlag = 0;
    char *s_instIndex = NULL;
#if 0
    Boolean isUserChg = FALSE;
#endif

    char *user = NULL, *passwd1 = NULL, *action = NULL, *userOld = NULL, *userIp = NULL;
    char *remark = NULL, *remarkOld = NULL;
    struct in_addr bindIp;
#if (PPPOE_LIMIT == FYES)
    char *uRateLimitStr = NULL, *dRateLimitStr = NULL;
#endif
#if 0
    char_t *bindmacs = NULL, *accmaxconvs = NULL;
#else
    char_t *AccMACBinds = NULL, *bindmacs = NULL, *accmaxconvs = NULL;
    char_t *bindmac2s = NULL, *bindmac3s = NULL, *bindmac4s = NULL;
#endif
    Uint32 h_bindIp, h_ipStart;
    char_t *enable = NULL;
    char_t *timestartStr = NULL, *timeendStr = NULL;
#if (IP_GRP == FYES)
    char_t *UserGroupName = NULL;
#endif
    Uint32 starttm = 0U;
    Uint32 endtm = 0U;
    char instId[UTT_INST_NAME_LEN + 1] = {0};
    Uint16 webIndexInst = 0;
    const char * ErrMsg = NULL;
    const char * ErrMsg2 = NULL;

#if (IP_GRP == FYES)
    memset(referedMsg, 0, sizeof(referedMsg));
#endif
    memset(&profBak, 0, sizeof(profBak));
    /* * ���ҳ����� */
    user = websGetVar(wp,T("pppoename"), T(""));
    passwd1 = websGetVar(wp,T("passwd1"), T(""));
    userIp = websGetVar(wp,T("aspip"), T(""));
    action = websGetVar(wp,T("Action"), T(""));
    userOld = websGetVar(wp, T("pppoenameold"), T("")); 
#if (IP_GRP == FYES)
    UserGroupName = websGetVar(wp, T("UserGroupName"), T(""));
    SWORD_PRINTF("%s - %d, UserGroupName = %s\n",__func__, __LINE__, UserGroupName);
    if (strcmp(UserGroupName,"") != 0) {
        if (pppoeAccGrpIsFull(UserGroupName)) {
            ErrMsg = getTransDataForC(C_LANG_INDEX_USER_GROUP_LIST_MAX);
            setTheErrorMsg(ErrMsg);
            goto out;
        }
    }
#endif
#if (PPPOE_ACCOUNT == FYES) 
    enable = websGetVar(wp, T("enabled"),NULL);
    timestartStr = websGetVar(wp, T("BeginDate"), T(""));
    timeendStr = websGetVar(wp, T("EndDate"), T(""));
    SWORD_PRINTF("%s - %d, get BeginDate = %s, get EndDate = %s\n",__FUNCTION__, __LINE__, timestartStr, timeendStr);
    starttm = strtoul(timestartStr, NULL, 10);
    endtm = strtoul(timeendStr, NULL, 10);
    SWORD_PRINTF("%s - %d, starttm = %d, endtm = %d\n",__FUNCTION__, __LINE__, starttm, endtm);
#endif
    AccMACBinds = websGetVar(wp, T("AccMACBind"), T(""));
    bindmacs = websGetVar(wp, T("bindmac"), T("")); 
    bindmac2s = websGetVar(wp, T("bindmac2"), T("")); 
    bindmac3s = websGetVar(wp, T("bindmac3"), T("")); 
    bindmac4s = websGetVar(wp, T("bindmac4"), T("")); 
    accmaxconvs = websGetVar(wp, T("accmaxconv"), T("")); 
    s_instIndex = websGetVar(wp, T("instIndex"), T("")); 
    remark = websGetVar(wp, T("remark"), T(""));
    remarkOld =websGetVar(wp, T("remarkold"), T(""));
#if 0
    instIndex = s_instIndex[2] - '0';
#endif

    PPPOE_SRV_PRINTF("%s:user=%s,passwd1=%s,userIp=%s,action=%s,userOld=%s\n", __FUNCTION__, user, passwd1, userIp, action, userOld);
    PPPOE_SRV_PRINTF("s_instIndex[%s] instIndex[%d]\n",s_instIndex,instIndex);
    if ((user == NULL) || (passwd1 == NULL) || (userIp == NULL) || (action == NULL) || (userOld == NULL)) {
        ErrMsg = getTransDataForC(C_LANG_INDEX_CINFIG_PARAMETER_WRONG);
        setTheErrorMsg(ErrMsg);
        goto out;
    }
#if (PPPOE_LIMIT == FYES)
    uRateLimitStr = websGetVar(wp,T("uRate"), T("0"));/*�ϴ�����*/
    dRateLimitStr = websGetVar(wp,T("dRate"), T("0"));/*��������*/
#endif
#if 0
    /*
     * �ж�����
     */
    if (strncmp(passwd1, passwd2, UTT_PASSWD_LEN) != 0) {
        setTheErrorMsg(T("ǰ�������������벻��"));
        goto out;
    }
#endif

#if 0
    printWeb();
#endif
    /*
     * �̶�IP��ַ
     */
    if (strlen(userIp) == 0) {
        bindIp.s_addr = 0;
    } else {
        if (0 == inet_aton(userIp, &bindIp)) {
            ErrMsg = getTransDataForC(C_LANG_INDEX_STATIC_IP_ADDR_CIN_ERR);
            setTheErrorMsg(ErrMsg);
            goto out;
        }
    }

    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS, 0);
    if ((profS == NULL) || (ProfInstIsFree(profS) == 1)) {
        PPPOE_SRV_PRINTF("%s: get profile error\n", __FUNCTION__);
        ErrMsg = getTransDataForC(C_LANG_INDEX_GET_ARGS_ERR);
        setTheErrorMsg(ErrMsg);
        goto out;
    }

    h_bindIp = ntohl(bindIp.s_addr);
    h_ipStart = ntohl(profS->ipStart);
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
    //�̶�ip��ַ�ط����޸�Ϊ��ϵͳ���Ự���޹�
    if ((h_bindIp != 0) && (h_bindIp < h_ipStart || h_bindIp >= (h_ipStart + profS->ipcount))) { 
#else
    if ((h_bindIp != 0) && (h_bindIp < h_ipStart || h_bindIp >= (h_ipStart + profS->maxSessCnt))){
#endif
        ErrMsg = getTransDataForC(C_LANG_INDEX_STATIC_IP_ADDR_CANT_IN_PPPoE_POOL);
        setTheErrorMsg(ErrMsg);
        goto out;
    }

    /*
     * ����������˺�-IP��
     */
    if (allowIpUserBind(user, userIp) == 0) {
        ErrMsg = getTransDataForC(C_LANG_INDEX_BANNED_BOUND_ALREADY_ASSIGN_IP);
        setTheErrorMsg(ErrMsg);
        goto out;
    }

#if (PPPOE_LIMIT == FYES)
    /*�Ƿ�ﵽpppoe���������ֵ*/
    if (checkPPPOEAccLimitNum(strtoul(uRateLimitStr, NULL, 0), strtoul(dRateLimitStr, NULL, 0), s_instIndex) == 1)
    {
        ErrMsg = getTransDataForC(C_LANG_INDEX_PPPOE_LIMIT_CANT_MORE);
        setTheErrorMsg(ErrMsg);
        goto out;
    }
#endif

    if (strcmp(action, "add") == 0) {
        /*
         * �û����Ƿ��ظ�
         */
        if (user != NULL && (ProfGetInstPointByValue(mibTypeA, OFFANDSIZE(PPPoESrvAccProfile, pppoename), TSTR, user, -1) != PROFFAIL)) {
            ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_ALREADY_EXITS);
            setTheErrorMsg(ErrMsg);
            goto out;
        }
        /*
         * �жϹ̶�IP��ַ�Ƿ��ظ�
         */
        if ((bindIp.s_addr != 0) && (ProfGetInstPointByValue(mibTypeA, OFFANDSIZE(PPPoESrvAccProfile, bindIp), TIPADDR, userIp, -1) != PROFFAIL)) {
            ErrMsg = getTransDataForC(C_LANG_INDEX_STATIC_IP_ADDR_ALREADY_EXITS);
            setTheErrorMsg(ErrMsg);
            goto out;
        }
        webIndexInst = getWebInstName(&(nvramProfile->pppoeInstRec.instRecMap), &(nvramProfile->pppoeInstRec.instNameIndex));
        /* portmap���� */
        if (webIndexInst == 0) {
            ErrMsg = getTransDataForC(C_LANG_INDEX_OPERATION_FAIL);
            setTheErrorMsg(ErrMsg);
            goto out;
        }
        snprintf(instId, UTT_INST_NAME_LEN, "ID%d", webIndexInst);
        /* �Ѵ���ʵ���� */
        if (ProfGetInstIndexByName(mibTypeA, instId) != PROFFAIL) {
            ErrMsg = getTransDataForC(C_LANG_INDEX_CASE_ALREADY_EXITS);
            setTheErrorMsg(ErrMsg);
            goto out;
        }
        instIndex = ProfNewInst(mibTypeA, instId, FALSE);

        if(instIndex == PROFFAIL) {
            clearPortMap(&(nvramProfile->pppoeInstRec.instRecMap), webIndexInst);
            ErrMsg = getTransDataForC(C_LANG_INDEX_ALREADY_THE_MAX_TITLE);
            setTheErrorMsg(ErrMsg);
            goto out;
        }
#if 0
        sprintf(s_instIndex,"ID%d",instIndex);
#endif
        profAction = PROF_CHANGE_ADD;
    } else {
        strncpy(instId, s_instIndex, UTT_INST_NAME_LEN);
        instIndex = ProfGetInstIndexByName(mibTypeA, instId);
        if (instIndex == PROFFAIL) {
            ErrMsg = getTransDataForC(C_LANG_INDEX_CHANGED_CASE_NOT_EXITS);
            setTheErrorMsg(ErrMsg);
            goto out;
        }
        /*
         * �û����ж�
         */
        if (strcmp(user, userOld) != 0) { /* �û����޸��� */
#if 0
            isUserChg = TRUE;
#endif
#if (IP_GRP == FYES)
            referedProf = userRefByIpGrp(userOld, IPENT_POE_ACC);
            if(referedProf != NULL)
            {
                ErrMsg = getTransDataForC(C_LANG_INDEX_BE_USER_GROUP);
                ErrMsg2 = getTransDataForC(C_LANG_INDEX_USE_CANT_BE_CHANGE);
                sprintf(referedMsg, "%s%s%s%s", userOld, ErrMsg, referedProf->head.name, ErrMsg2);
                setTheErrorMsgOnFree(referedMsg);
                goto out;
            }
#endif
            if (user != NULL && (ProfGetInstPointByValue(mibTypeA, OFFANDSIZE(PPPoESrvAccProfile, pppoename), TSTR, user, instIndex) != PROFFAIL)) {
                ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_ALREADY_EXITS);
                setTheErrorMsg(ErrMsg);
                goto out;
            }
        }
        /*
         * �жϹ̶�IP��ַ
         */
        if ((bindIp.s_addr != 0) && (ProfGetInstPointByValue(mibTypeA, OFFANDSIZE(PPPoESrvAccProfile, bindIp), TIPADDR, userIp, instIndex) != PROFFAIL)) {
            ErrMsg = getTransDataForC(C_LANG_INDEX_STATIC_IP_ADDR_ALREADY_EXITS);
            setTheErrorMsg(ErrMsg);
            goto out;
        }

        profAction = PROF_CHANGE_EDIT;
#if 0
        killOnePppoeClient(userOld);
#endif
    }

#if (IP_GRP == FYES)
    if(strlen(UserGroupName) != 0)
    {
        if (UserGroupConfigAdd(UserGroupName, user) == 0)
        {
            goto out;
        }
    }
#endif
    /*
     *�˴�Ϊ�����ź��������˲���
     */
    uttSemP(SEM_PPPOE_NO,0);
    ProfBackupByIndex(mibTypeA, profAction, instIndex, &profList);

    profA = (PPPoESrvAccProfile *)ProfGetInstPointByIndex(mibTypeA, instIndex);
    memcpy(&profBak, profA, sizeof(profBak));/*�ȱ���ԭ����*/

    /* ����Ϊʵ����  */
#if 0
    ProfSetNameByPoint(profA, s_instIndex);
#endif
    ProfSetNameByPoint(profA, instId);
    strncpy(profA->pppoename, user, UTT_PPPOE_NAME_LEN);
    strncpy(profA->passwd, passwd1, UTT_PPPOE_CLI_PASS_LEN);
    profA->bindIp = bindIp.s_addr;

    profA->accmaxconv = strtoul(accmaxconvs, NULL, 0);
    strcpy(profA->AccMACBind, AccMACBinds);
    if(strlen(bindmacs) < 12) 
    {
        memset(bindmacs, 0, sizeof(bindmacs));
        strcpy(bindmacs, "000000000000");
    }
    converMac12To6Byte(bindmacs, &(profA->bindmac));
    if(strlen(bindmac2s) < 12) 
    {
        memset(bindmac2s, 0, sizeof(bindmac2s));
        strcpy(bindmac2s, "000000000000");
    }
    converMac12To6Byte(bindmac2s, &(profA->bindmac2));
    if(strlen(bindmac3s) < 12) 
    {
        memset(bindmac3s, 0, sizeof(bindmac3s));
        strcpy(bindmac3s, "000000000000");
    }
    converMac12To6Byte(bindmac3s, &(profA->bindmac3));
    if(strlen(bindmac4s) < 12) 
    {
        memset(bindmac4s, 0, sizeof(bindmac4s));
        strcpy(bindmac4s, "000000000000");
    }
    converMac12To6Byte(bindmac4s, &(profA->bindmac4));

    if (memcmp(&profBak, profA, sizeof(profBak)) != 0)
    {
        killOnePppoeClient(&profBak);
        /*mantis:11317,���Ҷ���������£�Ҳ��������Ự�������ֻ�޸�ʱ�䲻�Ҷϣ����޸ĺ��ʱ�����ǹ��ڵģ���Ҫ�Ự�����жϹ��ڣ����Բ������*/
        profA->sesIndex = -1;
        profA->actualSession = 0;
    }

#if (PPPOE_ACCOUNT == FYES)
    if(enable != NULL)
    {
        SWORD_PRINTF("%s - %d, chargeType = date\n", __FUNCTION__, __LINE__);
        /*mantis:11317,�ɹرռƷ�ģʽ״̬����ʵ�����������ǹ��ڵģ�ֻ��û���Ʒ�ģʽ������Ч��
          �޸�Ϊ�򿪼Ʒ�ģʽʱ����Ҫ��״̬�޸�Ϊ���������жϹ���״̬ʱ���ܽ�ip�ٴμ��������*/
        if (profA->chargeType == NONE_CHARG_MODE)
        {
            profA->expired = PPPOE_ACC_NORMAL;
        }
        profA->chargeType = DATE_CHARG_MODE;
    }
    else
    {
        SWORD_PRINTF("%s - %d, chargeType = none\n", __FUNCTION__, __LINE__);
        /*mantis:11317,�����ڹ���״̬�޸�Ϊ�رռƷ�ģʽ״̬����Ҫɾ��pppoe�������е�ip�������ڲ��Ҷ�������޷�����*/
        if (profA->chargeType == DATE_CHARG_MODE)
        {
            delAllSesRule(profA);/*ɾ��ԭ�й���*/
        }
        profA->chargeType = NONE_CHARG_MODE;
    }

    /*mantis11317,�޸�ʱ�䲻�Ҷ�����*/
    profA->tmStart = starttm;
    profA->tmStop = endtm;
    SWORD_PRINTF("%s - %d, profA->tmStart = %d, profA->tmStop = %d\n", __FUNCTION__, __LINE__,profA->tmStart, profA->tmStop);
#endif

#if (PPPOE_LIMIT == FYES)
    profA->uRateLimit= strtoul(uRateLimitStr, NULL, 0);
    profA->dRateLimit= strtoul(dRateLimitStr, NULL, 0);
#endif

    if (strcmp(remark, remarkOld) != 0)
    {
        strcpy(profA->remark, remark);
    }

    SWORD_PRINTF("%s - %d, profA->pppoename = %s,profA->actualSession = %d, profA->accmaxconv= %d\n",__FUNCTION__, __LINE__, profA->pppoename, profA->actualSession, profA->accmaxconv);


    /*
     *�˴�Ϊ���źŽ���
     */
    uttSemV(SEM_PPPOE_NO,0);

    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
out:
#if 0
    if((profBak.ConTime == 0) &&
            (strcmp(profBak.ip, wp->ipaddr) == 0))
    {/*bug 0009804:�޸��Լ�����tcp�쳣�Ͽ�*/
        sleep(3u);
    }
#else
    spFlag = editShelfSleep(instIndex, wp);
    if(spFlag == 1)
    {
        sleep(3u);
    }
#endif
    websRedirect(wp, "PPPoEServer.asp");
    return ;
}
/********************************************************************
 * ������ editShelfSleep
 * ���ܣ� �ж��޸ĵ��˺��Ƿ����Լ�����ʹ�õ�
 * ������ 2012-08-21
 * ������ index  -    �û������˺ŵ�����
 *         wp    -    ҳ��ṹ��
 * ���أ�  1     -    �����˺����Լ�����ʹ�õ�
 *         0     -    �����˺Ų����Լ�����ʹ�õ�
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int editShelfSleep(int index, webs_t wp)
{
    MibProfileType mibType = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *prof = NULL;
    int i = 0, flag = 0;
    struct in_addr pppoeIp;

    prof = (PPPoESrvGloProfile*)ProfGetInstPointByIndex(mibType, 0);
    if(prof != NULL)
    {
        inet_aton(wp->ipaddr, &pppoeIp);
        i = ntohl(pppoeIp.s_addr) - ntohl(prof->ipStart);
        if(SesIndexIsOk(i) == 1)
        {
            if((nvramProfile->Session[i].ConTime == 0U) && (nvramProfile->Session[i].accIndex == index))
            {/*bug 0009804:�޸��Լ�����tcp�쳣�Ͽ�*/
                flag = 1;
            }
        }
    }
    return flag;
}

#if (PPPOE_LIMIT == FYES)
/*
 * ���������ٲ������������������м��
 */
static int pppoeAccLimitBatchCheck(PPPoESrvAccProfile* indexArray[], int uRate, int dRate)
{
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;
    int isEmpty = 0;/*�п�λ��*/
    int i = 0, j = 0;
    PppoeAccLimit limitGroupTmp[MAX_PPPOE_LIMIT_IPGRP];

    int retVal = 1;/*Ĭ��ʧ��*/

    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS, 0);
    if (ProfInstIsFree(profS) == 0)
    {
        /*�������������Ϣ���鿽��һ����������*/
        memcpy(limitGroupTmp, profS->limitGroup, sizeof(profS->limitGroup));
        /*ѭ�����ҿ�λ��*/
        for (j=0;j<MAX_PPPOE_LIMIT_IPGRP;j++)
        {
            /*���ԭ���пյĻ���Ҫ�޸ĵĺ�ԭ������ͬ*/
            if ((limitGroupTmp[j].count == 0) || ((limitGroupTmp[j].uRateLimit == uRate) && (limitGroupTmp[j].dRateLimit == dRate)))
            {
                isEmpty = 1;/*���ҵ���ʹ�õ�λ��*/
                break;
            }
        }
        if (isEmpty == 1)
        {
            return 0;/*���سɹ�*/
        }
        else/*ȫ����ʹ��*/
        {
            /*�ҵ�NULL����*/
            for (i=0;indexArray[i] != NULL;i++)
            {
                for (j=0;j<MAX_PPPOE_LIMIT_IPGRP;j++)
                {
                    /*�ҵ���¼��Ӧ������Ϣ����*/
                    if ((indexArray[i]->uRateLimit == limitGroupTmp[j].uRateLimit) && (indexArray[i]->dRateLimit == limitGroupTmp[j].dRateLimit))
                    {
                        limitGroupTmp[j].count--;/*���ü�����1*/
                        if (limitGroupTmp[j].count <= 0)/*���û�б����ã���ɹ�*/
                        {
                            return 0;/*�ɹ�*/
                        } 
                    }
                }
            }
        }
    }
    return retVal;
}

/*
 * �������ٲ���
 */
static void formConfigPppoeRateLimit(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibTypeA = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *profA = NULL;
    struProfAlloc *profList  = NULL;
    const char *ErrMsg = NULL;
    PPPoESrvAccProfile* indexArray[MAX_PPPOE_SRV_ACC_PROFILES + 1];
    char *uRateLimitStr = NULL, *dRateLimitStr = NULL;
    char *UserName = NULL, *tmp = NULL;
    unsigned long uRate = 0, dRate = 0;

    int i = 0;

    uRateLimitStr = websGetVar(wp,T("MaxUpRatetxt"), NULL);/*�ϴ�����*/
    dRateLimitStr = websGetVar(wp,T("MaxDownRatetxt"), NULL);/*��������*/
    UserName = websGetVar(wp, T("delstr"), NULL); 
    uRate = strtoul(uRateLimitStr, NULL, 0);
    dRate = strtoul(dRateLimitStr, NULL, 0);

    if (UserName == NULL) {
        ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_USER_NAME_NOG);
        setTheErrorMsg(ErrMsg);
        goto out;
    }

    tmp = strtok(UserName, UTT_SPLIT_SIGN_STR);
    while(tmp != NULL)/*������ѡ�нṹ��ָ�뱣�浽����*/
    {
        profA = ProfGetInstPointByName(mibTypeA, tmp);
        if (ProfInstIsFree(profA) == 0)
        {
            indexArray[i] = profA;
            i++;
        }
        tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
    }
    indexArray[i] = NULL;/*NULL��Ϊ������־*/

    /*�������Ƿ�ﵽ�������*/
    if (pppoeAccLimitBatchCheck(indexArray, uRate, dRate))
    {
        ErrMsg = getTransDataForC(C_LANG_INDEX_PPPOE_LIMIT_CANT_MORE);
        setTheErrorMsg(ErrMsg);
        goto out;
    }

    /*
     *�˴�Ϊ�����ź��������˲���
     */
    uttSemP(SEM_PPPOE_NO,0);

    for (i=0;indexArray[i]!=NULL;i++)
    {
        if (i == 0)/*ֻ֪ͨһ��*/
        {
            ProfBackupByName(mibTypeA, PROF_CHANGE_ADD_ALL, indexArray[i]->head.name, &profList);/*֪ͨspeedweb*/
        }
        indexArray[i]->uRateLimit = uRate;
        indexArray[i]->dRateLimit = dRate;
    }

    /*
     *�˴�Ϊ���źŽ���
     */
    uttSemV(SEM_PPPOE_NO,0);

    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();

out:
    websRedirect(wp, "PPPoEServer.asp");
}
#endif
/*
 * �б�ɾ������
 */
static void formConfigPppoeUserDel(webs_t wp, char_t *path, char_t *query)
{
#if 0
    char_t *UserName=NULL,*CList,*p1,*p2,*tmp;
#endif
    char_t *UserName = NULL, *tmp = NULL;
#if 0
    unsigned int     kill_pid;
    int instIndexs = 0;
#endif
    struProfAlloc *profList  = NULL;
    MibProfileType mibTypeA = MIB_PROF_PPPOE_SRV_ACC;
#if (IP_GRP == FYES)
    IpGrpProfile* referedProf = NULL;
    char referedMsg[50];
#endif
    PPPoESrvAccProfile *profA = NULL, profBak;
    int back = 0, spFlag = 0, Index = 0;
    Uint16 webIndexInst = 0;
    const char *ErrMsg = NULL, *ErrMsg2 = NULL;
    a_assert(wp);
#if (IP_GRP == FYES)
    memset(referedMsg, 0, sizeof(referedMsg));
#endif
    memset(&profBak, 0, sizeof(profBak));
#if 0
    CList = nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_USER);
#endif
    UserName = websGetVar(wp,T("delstr"),T("")); 

    PPPOE_SRV_PRINTF("%s: instIndexs = %s\n", __FUNCTION__, UserName);

    if (UserName == NULL) {
        ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_USER_NAME_NOG);
        setTheErrorMsg(ErrMsg);
        goto out;
    }

    tmp = strtok(UserName, UTT_SPLIT_SIGN_STR);

    /*
     *�˴�Ϊ�����ź��������˲���
     */
    uttSemP(SEM_PPPOE_NO,0);

    while(tmp != NULL) {

        profA =  (PPPoESrvAccProfile*)ProfGetInstPointByName(mibTypeA, tmp);
        if(ProfInstIsFree(profA) == 0)
        {/*ʹ����*/
#if (IP_GRP == FYES)
            referedProf = userRefByIpGrp(profA->pppoename, IPENT_POE_ACC);
            if(referedProf != NULL)
            {
                ErrMsg = getTransDataForC(C_LANG_INDEX_BE_USER_GROUP);
                ErrMsg2 = getTransDataForC(C_LANG_INDEX_USE_CANT_BE_DELETE);
                sprintf(referedMsg, "%s%s%s%s", profA->pppoename, ErrMsg, referedProf->head.name, ErrMsg2);
                setTheErrorMsgOnFree(referedMsg);
            }
            else
            {
#endif

                memcpy(&profBak, profA, sizeof(profBak));/*�ȱ���ԭ����*/
                if(back == 0){
                    ProfBackupByName(mibTypeA, PROF_CHANGE_SPECIAL, tmp, &profList);
                    back = 1;
                }
                else{
                    ProfBackupByName(mibTypeA, PROF_CHANGE_DEL, tmp, &profList);
                }
                ProfDelInstByName(mibTypeA, tmp);
                killOnePppoeClient(&profBak);
                profA->sesIndex = -1;
                profA->actualSession = 0;
                SWORD_PRINTF("%s - %d, profA->pppoename = %s,profA->actualSession = %d, profA->accmaxconv= %d\n",__FUNCTION__, __LINE__, profA->pppoename, profA->actualSession, profA->accmaxconv);
#if 0
                if((profBak.ConTime == 0) &&
                        (strcmp(profBak.ip, wp->ipaddr) == 0))
                {/*bug 0009804:�޸��Լ�����tcp�쳣�Ͽ�*/

                    spFlag = 1;
                }
#else
                Index = ProfGetInstIndexByName(mibTypeA, tmp);
                spFlag = editShelfSleep(Index, wp);
#endif

                /* �����¼map */
                if (strncmp(tmp, "ID", 2) == 0) {
                    webIndexInst = (Uint16)strtol(&(tmp[2]), NULL, 10);
                    clearPortMap(&(nvramProfile->pppoeInstRec.instRecMap), webIndexInst);
                }

#if (IP_GRP == FYES)
            }
#endif
        }


        tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
    }

    /*
     *�˴�Ϊ���źŽ���
     */
    uttSemV(SEM_PPPOE_NO,0);

    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
#if 0
    if(!UserName) {   
        setTheErrorMsg(T("�Ҳ������û���!"));
    } else {
        tmp=strtok(UserName,UTT_SPLIT_SIGN_STR);
        while(tmp != NULL) {
            p1=strnmatch(CList, tmp, 0);
            if(p1 == 0) {
                setTheErrorMsg(T("�Ҳ������û���!"));
            } else {
                killOnePppoeClient(tmp);
                if(_pppoeUserCount == 1) {
                    _pppoeUserCount = 0;
                    nvram_bufset(RT2860_NVRAM, _CONF_PPPOE_S_USER, "");
                    tmp = NULL;
                } else {
                    p2=p1;
                    while( (*p2!=UTT_SPLIT_SIGN_RECORD) && *p2) {
                        p2++;
                    }               
                    if(p1 != CList) {
                        /*
                         * before p1 there is the char';'
                         */
                        p1--;   
                    } else if(*p2 != 0) { 
                        /*
                         * if del the first record ,jump over the char ';'
                         */
                        p2++;    
                    }
                    *p1=0;
                    /*
                     * copy the string after UserNameOld record
                     */
                    strcat(CList,p2);                             
                    _pppoeUserCount--;
                    tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
                }
            }
        }
    }
    nvram_bufset(RT2860_NVRAM, _CONF_PPPOE_S_USER, CList);
    nvram_commit(RT2860_NVRAM);
    sendMsgForSpeed(INIT_PPPOE_SERVER_USER_FILE, 0, 0, NULL, 0);
#endif
out:
    if(spFlag == 1)
    {
        sleep(3u);/*�ȴ���ɱ��*/
    }
    websRedirect(wp, "PPPoEServer.asp");
}

/*
 * �б�ȫ��ɾ��
 */
static void formPppoeUserDelAll(webs_t wp, char_t *path, char_t *query)
{
    int min = 0, max = 0;
    int i = 0;
    int back = 0, spFlag = 0;
    Uint16 webIndexInst = 0;
#if 0
    char	     kill_device[UTT_PPP_CLIENT_DEVICE_LEN + 1];
    unsigned int     kill_pid;
#endif
    MibProfileType mibTypeA = MIB_PROF_PPPOE_SRV_ACC;
#if (IP_GRP == FYES)
    IpGrpProfile* referedProf = NULL;
    char referedMsg[50];
#endif
    PPPoESrvAccProfile *profA = NULL, profBak;
    struProfAlloc *profList  = NULL;
    const char *ErrMsg = NULL, *ErrMsg2 = NULL;

    a_assert(wp);
#if (IP_GRP == FYES)
    memset(referedMsg, 0, sizeof(referedMsg));
#endif
    memset(&profBak, 0, sizeof(profBak));
    ProfInstLowHigh(mibTypeA, &max, &min);

    /*
     *�˴�Ϊ�����ź��������˲���
     */
    uttSemP(SEM_PPPOE_NO,0);


    for(i = min; i < max; i++) {
        profA = (PPPoESrvAccProfile*)ProfGetInstPointByIndex(mibTypeA, i);
        if(ProfInstIsFree(profA) == 0) {
#if (IP_GRP == FYES)
            referedProf = userRefByIpGrp(profA->pppoename, IPENT_POE_ACC);
            if(referedProf != NULL)
            {
                ErrMsg = getTransDataForC(C_LANG_INDEX_BE_USER_GROUP);
                ErrMsg2 = getTransDataForC(C_LANG_INDEX_USE_CANT_BE_DELETE);
                sprintf(referedMsg, "%s%s%s%s", profA->pppoename, ErrMsg, referedProf->head.name, ErrMsg2);
                setTheErrorMsgOnFree(referedMsg);
            }
            else
            {
#endif
                memcpy(&profBak, profA, sizeof(profBak));/*�ȱ���ԭ����*/
                if(back == 0) {
                    ProfBackupByIndex(mibTypeA, PROF_CHANGE_DELALL, i, &profList);
                    back = 1;
                }
                ProfDelInstByIndex(mibTypeA, i);
#if 0
                if (strcmp((char *)getLanIfName(), profBak.device) == 0) 
                {/*��ɾ�˺š���ɱ�����̡���ֹɱ�����û����²��ųɹ�*/
#endif
                    killOnePppoeClient(&profBak);
                    profA->sesIndex = -1;
                    profA->actualSession = 0;
                    SWORD_PRINTF("%s - %d, profA->pppoename = %s,profA->actualSession = %d, profA->accmaxconv= %d\n",__FUNCTION__, __LINE__, profA->pppoename, profA->actualSession, profA->accmaxconv);
#if 0
                    if((profBak.ConTime == 0) &&
                            (strcmp(profBak.ip, wp->ipaddr) == 0))
                    {/*bug 0009804:�޸��Լ�����tcp�쳣�Ͽ�*/

                        spFlag = 1;
                    }
#else
                    spFlag = editShelfSleep(i, wp);
#endif
                    /* �����¼map */
                    if (strncmp(profBak.head.name, "ID", 2) == 0) {
                        webIndexInst = (Uint16)strtol(&(profBak.head.name[2]), NULL, 10);
                        clearPortMap(&(nvramProfile->pppoeInstRec.instRecMap), webIndexInst);
                    }
#if 0
                }
#endif
#if (IP_GRP == FYES)
            }
#endif
        }
    }

    /*
     *�˴�Ϊ���źŽ���
     */
    uttSemV(SEM_PPPOE_NO,0);

#if 0
    /* ����map��¼ */
    clearAllPortMap(&(nvramProfile->pppoeInstRec.instRecMap));
    nvramProfile->pppoeInstRec.instNameIndex = 0;
#endif
#if 0
    killallPppoeClient();
#endif
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
#if 0
    nvram_bufset(RT2860_NVRAM, _CONF_PPPOE_S_USER, "");
    _pppoeUserCount = 0;
    nvram_commit(RT2860_NVRAM);
    sendMsgForSpeed(INIT_PPPOE_SERVER, 0, 0, NULL, 0);
#endif
    if(spFlag == 1)
    {
        sleep(5u);/*�ȴ���ɱ��*/
    }
    websRedirect(wp, "PPPoEServer.asp");
}



/*
 * �˺�����ҳ���б��������
 */
static int aspGetPppoeUser(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibTypeA = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *profA = NULL;
    int min = 0, max = 0, i = 0, totalrecs = 0;
    const char *MsgStr = NULL;
   
    MibProfileType mibTypeG = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profG = NULL;
    /*global profile inst*/
    profG = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeG, 0);
    websWrite(wp, "var smaxconvs=\"%d\";", profG->maxSessCnt);/*����û������õ�ϵͳ���Ự��*/
    websWrite(wp, "var pppoeStarts=\"%N\";", profG->ipStart);
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
    websWrite(wp, "var ipcounts=\"%d\";", MAX_PPPOE_SRV_ACC_PROFILES);
#endif
    char_t *names=T("PPPoENames[%d] = \"%s\";");
    char_t *aspips=T("aspips[%d] = \"%N\";");
    char_t *uRateFmt = T("uRates[%d] = \"%d kbit/s\"; ");
    char_t *dRateFmt = T("dRates[%d] = \"%d kbit/s\"; ");
    char_t *Allow = T("Allows[%d] = \"%s\"; ");
    char_t *accModes= T("accModes[%d] = \"%s\"; ");
    char_t *expireds = T("expireds[%d] = \"%s\"; ");
    char_t *starttms = T("starttms[%d] = \"%d\"; ");
    char_t *endtms = T("endtms[%d] = \"%d\"; ");
    char_t *accmaxconvs = T("accmaxconvs[%d] = \"%d\"; ");
    char_t *pppoe_remark= T("pppoe_remark[%d] = \"%s\"; ");
    char_t *instIndexs =  T("instIndexs[%d] = \"%s\"; ");
    websWrite(wp,"var PPPoENames=new Array();");
    websWrite(wp,"var aspips=new Array();");
    websWrite(wp,"var bindmacs=new Array();");
    websWrite(wp,"var dRates=new Array();");
    websWrite(wp,"var uRates=new Array();");
    websWrite(wp,"var Allows=new Array();");
    websWrite(wp,"var accModes=new Array();");
    websWrite(wp,"var expireds=new Array();");
    websWrite(wp,"var starttms=new Array();");
    websWrite(wp,"var endtms=new Array();");
    websWrite(wp,"var startdates =new Array();");
    websWrite(wp,"var enddates =new Array();");
    websWrite(wp,"var accmaxconvs=new Array();");
    websWrite(wp,"var pppoe_remark=new Array();");
    websWrite(wp,"var instIndexs =new Array();");

    ProfInstLowHigh(mibTypeA, &max, &min);

    /*
     *�˴�Ϊ�����ź��������˲���
     */
    uttSemP(SEM_PPPOE_NO,0);

    for (i = min; i < max; i++) {
        profA = (PPPoESrvAccProfile *)ProfGetInstPointByIndex(mibTypeA, i);
        if((profA != NULL) && (ProfInstIsFree(profA) == 0)) {
            websWrite(wp, names, totalrecs, profA->pppoename);
            websWrite(wp, instIndexs , totalrecs, profA->head.name);
#if (PPPOE_LIMIT == FYES)
            websWrite(wp, uRateFmt, totalrecs, profA->uRateLimit);/*�ϴ�����*/
            websWrite(wp, dRateFmt, totalrecs, profA->dRateLimit);/*��������*/
#endif
            websWrite(wp, Allow, totalrecs, (profA->head.active ? "yes" : "no"));
#if (PPPOE_ACCOUNT == FYES)
            if (profA->chargeType == DATE_CHARG_MODE)
            {
                MsgStr = getTransDataForC(C_LANG_INDEX_BY_DATE);
                websWrite(wp, accModes, totalrecs, MsgStr);

                SWORD_PRINTF("%s - %d, profA->expired = %d\n",__FUNCTION__,__LINE__, profA->expired );
#if 0
                if(profA->ConTime != 0)
                {
                    upPppAccStatus(profA);
                }
#else
                pppoeSesCheck(profA);
#endif
                if (profA->expired == PPPOE_ACC_NORMAL)
                {
                    MsgStr = getTransDataForC(C_LANG_INDEX_NORMAL);
                    websWrite(wp, expireds, totalrecs, MsgStr);
                }
                else if (profA->expired == PPPOE_ACC_EXPIRING)
                {
                    MsgStr = getTransDataForC(C_LANG_INDEX_WILL_EXPIRE);
                    websWrite(wp, expireds, totalrecs, MsgStr);
                }
                else
                {
                    MsgStr = getTransDataForC(C_LANG_INDEX_EXPIRE);
                    websWrite(wp, expireds, totalrecs, MsgStr);
                }
                websWrite(wp, starttms, totalrecs, profA->tmStart);
                websWrite(wp, endtms, totalrecs, profA->tmStop);
            }
            else
            {
                websWrite(wp, accModes, totalrecs, "");
                websWrite(wp, expireds, totalrecs, "");
                websWrite(wp, starttms, totalrecs, 0);
                websWrite(wp, endtms, totalrecs, 0);
            }
#endif
            websWrite(wp, accmaxconvs, totalrecs, profA->accmaxconv);
            websWrite(wp, pppoe_remark, totalrecs, profA->remark);

            if (profA->bindIp == 0) {
                websWrite(wp, T("aspips[%d] = \"\";"), totalrecs);
            } else {
                websWrite(wp, aspips, totalrecs, profA->bindIp);
            }
            websWrite(wp, "bindmacs[%d]=\"", totalrecs);
            if(strcmp(converMac6To12Byte(profA->bindmac), "000000000000") != 0)
            {
                websWrite(wp, "%s ", converMac6To12Byte(profA->bindmac));
            }
            if(strcmp(converMac6To12Byte(profA->bindmac2), "000000000000") != 0)
            {
                websWrite(wp, "%s ", converMac6To12Byte(profA->bindmac2));
            }
            if(strcmp(converMac6To12Byte(profA->bindmac3), "000000000000") != 0)
            {
                websWrite(wp, "%s ", converMac6To12Byte(profA->bindmac3));
            }
            if(strcmp(converMac6To12Byte(profA->bindmac4), "000000000000") != 0)
            {
                websWrite(wp, "%s ", converMac6To12Byte(profA->bindmac4));
            }
            websWrite(wp, "\";\n");
            totalrecs++;
        }
    }

    /*
     *�˴�Ϊ���źŽ���
     */
    uttSemV(SEM_PPPOE_NO,0);

#if 0
    cList = nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_USER);
    while(*cList) {
        if(sscanf(cList,"%[^,],%[^,],%[^;]", user, passwd, aspip) == 3) {
            websWrite(wp, names, num, user);
            if(strcmp(aspip, "*") == 0)
            {
                strcpy(aspip,"");
            }
            websWrite(wp, aspips, num, aspip);
            num++;
        } else {
            ;
        }
        cList = NextRecordStart(cList);
    }
    _pppoeUserCount = num;
#endif
    websWrite(wp,"var totalrecs=%d;", totalrecs);
    websWrite(wp,"var max_totalrecs=%d;", MAX_PPPOE_SRV_ACC_PROFILES);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

/*
 * �û��˺�����ҳ�����һ��������Ϣ
 */
static int aspPppoeOneUserInfo(int eid, webs_t wp, int argc, char_t **argv)
{
#if 0
    char_t *edit_name,*CList,*tmp2,data[MAX_PPP_NAME_LEN]={0};
    char user[80], pwd[80];
    char *tmp1;
    int ct=0;
    int tempct = 3;
    if(ejArgs(argc,argv,T("%s"), &edit_name) <1) {
        setTheErrorMsg(T("��ȡedit_Name����ʧ��!"));
    } else {
        if(!edit_name) {
            setTheErrorMsg(T("�û�������Ϊ��!"));
        } else {
            CList = nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_USER);
            tmp1=strnmatch(CList,edit_name, 0);
            if(!tmp1) {
                setTheErrorMsg(T("�Ҳ������û�!"));
            } else {
                tmp2 = data;

                while(ct < tempct) {
                    if((*tmp1!=UTT_SPLIT_SIGN_CHAR) && (*tmp1!=UTT_SPLIT_SIGN_RECORD) && *tmp1) {
                        *tmp2++ = *tmp1++;
                    } else {
                        *tmp2 = 0;
                        ct++;
                        tmp1++;  
                        if(ct == 1) {
                            websWrite(wp, "var usernames=\"%s\";",data);
                        } else if(ct == 2){
                            websWrite(wp, "var passwd1s=\"%s\";",data);
                        } else if(ct == 3){
                            if(strcmp(data, "*") == 0)
                            {
                                strcpy(data,"");
                            }
                            websWrite(wp, "var aspips=\"%s\";",data);
                        }
                        tmp2=data;
                        memset(tmp2,0,sizeof(data));
                    }
                }
            }
        }
    }
#endif
    MibProfileType mibTypeA = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *profA = NULL;
    int min = 0, max = 0, i = 0;

    char *editName = NULL;
    const char *ErrMsg = NULL;

    websWrite(wp, "var usernames = \"\";");
    websWrite(wp, "var passwd1s = \"\";");
    websWrite(wp, "var aspips = \"\";");
    websWrite(wp, "var bindmacs=\"\";");
    websWrite(wp, "var bindmac2s=\"\";");
    websWrite(wp, "var bindmac3s=\"\";");
    websWrite(wp, "var bindmac4s=\"\";");
    websWrite(wp, "var accmaxconvs=\"\";");
    websWrite(wp, "var remarks=\"\";");
    websWrite(wp, "var instIndexs =\"\";");
#if (PPPOE_ACCOUNT == FYES)
    websWrite(wp, "var lively;");
    websWrite(wp, "var accMode=\"\";");
    websWrite(wp, "var starttm;");
    websWrite(wp, "var endtm;");
#endif
#if (PPPOE_LIMIT == FYES)
    websWrite(wp, "var uRate=\"\";");/*�ϴ�����*/
    websWrite(wp, "var dRate=\"\";");/*��������*/
#endif

    if ((ejArgs(argc, argv, T("%s"), &editName) < 1) || (editName == NULL)) {
        ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_READ_DATA_ERR);
        setTheErrorMsg(ErrMsg);
    } else {
        ProfInstLowHigh(mibTypeA, &max, &min);

        /*
         *�˴�Ϊ�����ź��������˲���
         */
        uttSemP(SEM_PPPOE_NO,0);

        for (i = min; i < max; i++) {
            profA = (PPPoESrvAccProfile *)ProfGetInstPointByIndex(mibTypeA, i);
            if((profA != NULL) && (ProfInstIsFree(profA) == 0) &&
                    (strcmp(editName, profA->head.name) == 0)) {
                websWrite(wp, "usernames = \"%s\";", profA->pppoename);
                websWrite(wp, "instIndexs = \"%s\";", profA->head.name);
                websWrite(wp, "remarks = \"%s\";", profA->remark);
                websWrite(wp, "passwd1s = \"%s\";", profA->passwd);
#if (PPPOE_LIMIT == FYES)
                websWrite(wp, "uRate=\"%d\";", profA->uRateLimit);/*�ϴ�����*/
                websWrite(wp, "dRate=\"%d\";", profA->dRateLimit);/*��������*/
#endif
                websWrite(wp, "AccMACBinds=\"%s\";", profA->AccMACBind);
                if(strcmp(profA->AccMACBind, "handbind") == 0)
                {
                    websWrite(wp, "bindmacs=\"%s\";", converMac6To12Byte(profA->bindmac));
                    websWrite(wp, "bindmac2s=\"%s\";", converMac6To12Byte(profA->bindmac2));
                    websWrite(wp, "bindmac3s=\"%s\";", converMac6To12Byte(profA->bindmac3));
                    websWrite(wp, "bindmac4s=\"%s\";", converMac6To12Byte(profA->bindmac4));
                }
                websWrite(wp, "accmaxconvs=\"%d\";", profA->accmaxconv);
                if (profA->bindIp != 0) {
                    websWrite(wp, "aspips = \"%N\";", profA->bindIp);
                }
#if (PPPOE_ACCOUNT == FYES)
                websWrite(wp, "lively=\"%d\";",1);
                if (profA->chargeType == DATE_CHARG_MODE)
                {
                    SWORD_PRINTF("%s - %d,accMode = date\n",__FUNCTION__,__LINE__);
                    websWrite(wp, "accMode=\"%s\";", "date");
                }
                else
                {
                    SWORD_PRINTF("%s - %d, accMode = none\n",__FUNCTION__,__LINE__);
                    websWrite(wp, "accMode=\"%s\";", "none");
                }
                websWrite(wp, "starttm=\"%d\";", profA->tmStart);
                websWrite(wp, "endtm=\"%d\";", profA->tmStop);
                SWORD_PRINTF("%s - %d, tmStart = %d, tmStop = %d\n",__FUNCTION__,__LINE__,profA->tmStart,profA->tmStop);
#endif
                break;
            }
        }

        /*
         *�˴�Ϊ���źŽ���
         */
        uttSemV(SEM_PPPOE_NO,0);

    }

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

#if 0
/*
 * pppoe serverͳ��ҳ��
 */
static int aspPppoeClientStatus(int eid, webs_t wp, int argc, char_t **argv)
{
    int fd = -1;
    int totalrecs = 0;
    off_t   start, end;
    int     timeL;
    int     fileLen;
    long long rx, tx, rxrate, txrate;
    char    txratebuf[0x20], rxratebuf[0x20];
    int readNum = 0;

    memset(txratebuf, 0x00, sizeof(txratebuf));
    memset(rxratebuf, 0x00, sizeof(rxratebuf));
    struct st_uttPppdClient info;
#if 0
    struct timeval now;
#endif
    /* ��ȡ����ʱ�䣬����ϵͳʱ�� */
    struct sysinfo sysInfo;
    int st_size = sizeof(struct st_uttPppdClient);

    char_t *names=T("PPPoENames[%d] = \"%s\";");
    char_t *ips=T("PPPoEIpaddrs[%d] = \"%s\";");
    char_t *macs=T("PPPoEMACaddrs[%d] = \"%s\";");
    char_t *times=T("PPPoEtimes[%d] = \"%d\";");
    char_t *uprates=T("PPPoEUprates[%d] = \"%s\";");
    char_t *downrates=T("PPPoEDownrates[%d] = \"%s\";");

    fd = open(UTT_PPPOE_CLIENT_INFO_FILE, O_RDWR);

#if 0
    gettimeofday(&now, NULL);
#endif
    sysinfo(&sysInfo);

    if(fd != -1) {
        start = lseek(fd, 0, SEEK_SET);
        end = lseek(fd, 0, SEEK_END);
        fileLen = end - start;
        lseek(fd, 0, SEEK_SET);
        /* mantis 8923: ��ȡ�ļ�ֱ��д�ļ���Ҫ���ļ���ס */
        fcntl(fd, F_SETLKW, file_lock(F_WRLCK|F_RDLCK, SEEK_SET));
        while(fileLen >= st_size) {
#if 0
            fcntl(fd, F_SETLKW, file_lock(F_WRLCK|F_RDLCK, SEEK_SET));
#endif
            read(fd, &info, st_size);
#if 0
            fcntl(fd, F_SETLKW, file_lock(F_UNLCK, SEEK_SET));
#endif
            readNum++;
            if((info.ConTime == 0) && (strcmp(info.ip, "")) && (strcmp(info.device, (char *)getLanIfName()) == 0)) {
                websWrite(wp, names, totalrecs, info.user);
                websWrite(wp, ips, totalrecs, info.ip);
                websWrite(wp, macs, totalrecs, info.mac);
                //                printf("uttfffffffff %s \n",info.ifname);

#if 0
                websWrite(wp, times, totalrecs, now.tv_sec - info.STime);
#endif
                websWrite(wp, times, totalrecs, sysInfo.uptime - info.STime);

                tx = getIfStatistic(info.ifname, TXBYTE);
                rx = getIfStatistic(info.ifname, RXBYTE);
#if 0
                timeL = now.tv_sec - info.LTime;
#endif
                timeL = sysInfo.uptime - info.LTime;
                if(timeL <= 0) {
                    timeL = 1;
                }

                if(tx == 0 || tx == -1) {
                    strcpy(txratebuf, "0");
                } else {
                    txrate = (tx - info.txByte)/timeL;
                    sprintf(txratebuf, "%lld", (txrate << 3));
                    info.txByte = tx;
                }

                if(rx == 0 || rx == -1) {
                    strcpy(rxratebuf, "0");
                } else {
                    rxrate = (rx - info.rxByte)/timeL;
                    sprintf(rxratebuf, "%lld", (rxrate << 3));
                    info.rxByte = rx;
                }
#if 0
                info.LTime = now.tv_sec;
#endif
                info.LTime = sysInfo.uptime;

                websWrite(wp, downrates, totalrecs, txratebuf);
                websWrite(wp, uprates, totalrecs, rxratebuf);
#if 0
                fcntl(fd, F_SETLKW, file_lock(F_WRLCK|F_RDLCK, SEEK_SET));
#endif
                lseek(fd, (readNum -1) * st_size, SEEK_SET);
                write(fd, &info, st_size);
#if 0
                fcntl(fd, F_SETLKW, file_lock(F_UNLCK, SEEK_SET));
#endif
                totalrecs++;
            }
            fileLen -= st_size;
        }
        fcntl(fd, F_SETLKW, file_lock(F_UNLCK, SEEK_SET));
        close(fd);
    }

    websWrite(wp,"var totalrecs = %d;", totalrecs);
    return 0;
}
#endif
#if 0
/********************************************************************
 * ������ aspPppoeClientStatus 
 * ���ܣ� pppoe serverͳ��ҳ��
 * ������ 2012-03-28
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int aspPppoeClientStatus(int eid, webs_t wp, int argc, char_t **argv)
{
    int max = 0;
    int min = 0;
    int i = 0;

    MibProfileType mibType = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *info = NULL;

    int totalrecs = 0;
    int     timeL;
    long long rx, tx, rxrate, txrate;
    char    txratebuf[0x20], rxratebuf[0x20];
    const char *ErrMsg = NULL;

    memset(txratebuf, 0x00, sizeof(txratebuf));
    memset(rxratebuf, 0x00, sizeof(rxratebuf));

    /* ��ȡ����ʱ�䣬����ϵͳʱ�� */
    struct sysinfo sysInfo;

    char_t *names=T("PPPoENames[%d] = \"%s\";");
    char_t *ips=T("PPPoEIpaddrs[%d] = \"%s\";");
    char_t *macs=T("PPPoEMACaddrs[%d] = \"%s\";");
    char_t *times=T("PPPoEtimes[%d] = \"%d\";");
    char_t *uprates=T("PPPoEUprates[%d] = \"%s\";");
    char_t *downrates=T("PPPoEDownrates[%d] = \"%s\";");
    char_t *expireds = T("expireds[%d] = \"%s\"; ");

    sysinfo(&sysInfo);

    websWrite(wp,"var expireds=new Array();");
    /* mantis 8923: ��ȡ�ļ�ֱ��д�ļ���Ҫ���ļ���ס */
    /*
     * �˴�Ϊ�����ź��������˲���
     */
    /*fcntl(fd, F_SETLKW, file_lock(F_WRLCK|F_RDLCK, SEEK_SET));*/

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min;i < max;i++)
    {
        info = (PPPoESrvAccProfile*)ProfGetInstPointByIndex(mibType, i);
        if(info != NULL)
        {
            if((info->ConTime == 0) && (strcmp(info->ip, "")) && (strcmp(info->device, (char *)getLanIfName()) == 0)) 
            {
                websWrite(wp, names, totalrecs, info->pppoename);
                websWrite(wp, ips, totalrecs, info->ip);
                websWrite(wp, macs, totalrecs, info->mac);

                websWrite(wp, times, totalrecs, sysInfo.uptime - info->STime);
                PPPOE_SRV_PRINTF("%s-%d: sysInfo->uptime=%u, info->STime=%u\n",__FUNCTION__,__LINE__,sysInfo.uptime,info->STime);
                tx = getIfStatistic(info->ifname, TXBYTE);
                rx = getIfStatistic(info->ifname, RXBYTE);
                timeL = sysInfo.uptime - info->LTime;
                if(timeL <= 0) {
                    timeL = 1;
                }

                if(tx == 0 || tx == -1) {
                    strcpy(txratebuf, "0");
                } else {
                    txrate = (tx - info->txByte)/timeL;
                    sprintf(txratebuf, "%lld", (txrate << 3));
                    info->txByte = tx;
                }

                if(rx == 0 || rx == -1) {
                    strcpy(rxratebuf, "0");
                } else {
                    rxrate = (rx - info->rxByte)/timeL;
                    sprintf(rxratebuf, "%lld", (rxrate << 3));
                    info->rxByte = rx;
                }
                info->LTime = sysInfo.uptime;

                websWrite(wp, downrates, totalrecs, txratebuf);
                websWrite(wp, uprates, totalrecs, rxratebuf);

#if (PPPOE_ACCOUNT == FYES)
                if (info->chargeType == NONE_CHARG_MODE)
                {
                    ErrMsg = getTransDataForC(C_LANG_INDEX_NORMAL);
                    websWrite(wp, expireds, totalrecs, ErrMsg);
                    SWORD_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
                }
                else if(info->chargeType == DATE_CHARG_MODE)
                {
                    if (info->expired == PPPOE_ACC_NORMAL)
                    {
                        ErrMsg = getTransDataForC(C_LANG_INDEX_NORMAL);
                        websWrite(wp, expireds, totalrecs, ErrMsg);
                        SWORD_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
                    }
                    else if (info->expired == PPPOE_ACC_EXPIRING)
                    {
                        ErrMsg = getTransDataForC(C_LANG_INDEX_WILL_EXPIRE);
                        websWrite(wp, expireds, totalrecs, ErrMsg);
                    }
                    else if (info->expired == PPPOE_ACC_EXPIRED)
                    {
                        ErrMsg = getTransDataForC(C_LANG_INDEX_EXPIRE);
                        websWrite(wp, expireds, totalrecs, ErrMsg);
                        SWORD_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
                    }
                    else
                    {
                        websWrite(wp, expireds, totalrecs, "");
                    }
                }
#endif
                totalrecs++;
            }
        }
    }
    /*
     *�˴�Ϊ���źŽ���
     */
    /*fcntl(fd, F_SETLKW, file_lock(F_UNLCK, SEEK_SET));*/

    websWrite(wp,"var totalrecs = %d;", totalrecs);
    return 0;
}
#else
/********************************************************************
 * ������ aspPppoeClientStatus 
 * ���ܣ� pppoe serverͳ��ҳ��
 * ������ 2012-08-24
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int aspPppoeClientStatus(int eid, webs_t wp, int argc, char_t **argv)
{
    int i = 0;

    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *prof = NULL;

    MibProfileType mibType = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *info = NULL;

    int totalrecs = 0;
    int     timeL;
    long long rx, tx, rxrate, txrate;
    char    txratebuf[0x20], rxratebuf[0x20];
    const char *ErrMsg=NULL;

    memset(txratebuf, 0x00, sizeof(txratebuf));
    memset(rxratebuf, 0x00, sizeof(rxratebuf));

    /* ��ȡ����ʱ�䣬����ϵͳʱ�� */
    struct sysinfo sysInfo;

    char_t *names=T("PPPoENames[%d] = \"%s\";");
    char_t *ips=T("PPPoEIpaddrs[%d] = \"%s\";");
    char_t *macs=T("PPPoEMACaddrs[%d] = \"%s\";");
    char_t *times=T("PPPoEtimes[%d] = \"%d\";");
    char_t *uprates=T("PPPoEUprates[%d] = \"%s\";");
    char_t *downrates=T("PPPoEDownrates[%d] = \"%s\";");
    char_t *expireds = T("expireds[%d] = \"%s\"; ");
    char_t *sessionId = T("PPPoESessionId[%d] = \"%d\";");
    char_t *pppoe_remarks=T("pppoe_remark[%d] = \"%s\";");


    sysinfo(&sysInfo);

    websWrite(wp,"var expireds=new Array();");
    websWrite(wp,"var PPPoESessionId=new Array();");
    /*
     * �˴�Ϊ�����ź��������˲���
     */
    uttSemP(SEM_PPPOE_NO,0);

    prof = (PPPoESrvGloProfile*)ProfGetInstPointByIndex(mibTypeS, 0);
    if(prof != NULL)
    {
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
        for(i = 0;i < prof->ipcount;i++)
#else 
        for(i = 0;i < prof->maxSessCnt;i++)
#endif
        {
            if((nvramProfile->Session[i].ConTime == 0U) && (strcmp(nvramProfile->Session[i].ip, "")) && (strcmp(nvramProfile->Session[i].device, (char *)getLanIfName()) == 0)) 
            {
                SWORD_PRINTF("%s-%d, i = %d, nvramProfile->Session[i].accIndex = %d\n",__func__,__LINE__, i, nvramProfile->Session[i].accIndex);
                info = (PPPoESrvAccProfile*)ProfGetInstPointByIndex(mibType, nvramProfile->Session[i].accIndex);
               
                websWrite(wp, names, totalrecs, info->pppoename);
                websWrite(wp, ips, totalrecs, nvramProfile->Session[i].ip);
                websWrite(wp, macs, totalrecs, nvramProfile->Session[i].mac);

                websWrite(wp, times, totalrecs, sysInfo.uptime - nvramProfile->Session[i].STime);
                PPPOE_SRV_PRINTF("%s-%d: sysInfo->uptime=%u, nvramProfile->Session[i].STime=%u\n",__func__,__LINE__,sysInfo.uptime,nvramProfile->Session[i].STime);
                tx = getIfStatistic(nvramProfile->Session[i].ifname, TXBYTE);
                rx = getIfStatistic(nvramProfile->Session[i].ifname, RXBYTE);
                timeL = sysInfo.uptime - nvramProfile->Session[i].LTime;
                if(timeL <= 0) {
                    timeL = 1;
                }

                if(tx == 0 || tx == -1) {
                    strcpy(txratebuf, "0");
                } else {
                    if (tx < nvramProfile->Session[i].txByte) {
                        strcpy(txratebuf, "0");
                    } else {
                        txrate = (tx - nvramProfile->Session[i].txByte)/timeL;
                        sprintf(txratebuf, "%lld", (txrate << 3));
                    }
                    nvramProfile->Session[i].txByte = tx;
                }

                if(rx == 0 || rx == -1) {
                    strcpy(rxratebuf, "0");
                } else {
                    if (rx < nvramProfile->Session[i].rxByte) {
                        strcpy(rxratebuf, "0");
                    } else {
                        rxrate = (rx - nvramProfile->Session[i].rxByte)/timeL;
                        sprintf(rxratebuf, "%lld", (rxrate << 3));
                    }
                    nvramProfile->Session[i].rxByte = rx;
                }
                nvramProfile->Session[i].LTime = sysInfo.uptime;


#if (PPPOE_ACCOUNT == FYES)
                if (info->chargeType == NONE_CHARG_MODE)
                {
                    ErrMsg = getTransDataForC(C_LANG_INDEX_PPPOE_ACC_NORMAL);
                    websWrite(wp, expireds, totalrecs, ErrMsg);
                    SWORD_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
                }
                else if(info->chargeType == DATE_CHARG_MODE)
                {
                    if (info->expired == PPPOE_ACC_NORMAL)
                    {
                        ErrMsg = getTransDataForC(C_LANG_INDEX_PPPOE_ACC_NORMAL);
                        websWrite(wp, expireds, totalrecs, ErrMsg);
                        SWORD_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
                    }
                    else if (info->expired == PPPOE_ACC_EXPIRING)
                    {
                        ErrMsg = getTransDataForC(C_LANG_INDEX_PPPOE_ACC_EXPIRING);
                        websWrite(wp, expireds, totalrecs, ErrMsg);
                    }
                    else if (info->expired == PPPOE_ACC_EXPIRED)
                    {
                        ErrMsg = getTransDataForC(C_LANG_INDEX_PPPOE_ACC_EXPIRED);
                        websWrite(wp, expireds, totalrecs, ErrMsg);
                        SWORD_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
                        sprintf(rxratebuf, "%d", 0);
                        sprintf(txratebuf, "%d", 0);
                    }
                    else
                    {
                        websWrite(wp, expireds, totalrecs, "");
                    }
                }
#endif
                websWrite(wp, downrates, totalrecs, txratebuf);
                websWrite(wp, uprates, totalrecs, rxratebuf);
                websWrite(wp, pppoe_remarks, totalrecs, info->remark);
                websWrite(wp, sessionId, totalrecs, i);
                totalrecs++;
            }
        }
    }
    /*
     *�˴�Ϊ���źŽ���
     */
    uttSemV(SEM_PPPOE_NO,0);


    websWrite(wp,"var totalrecs = %d;", totalrecs);
    return 0;
}
#endif
/********************************************************************
 * ������ formPPPoEAccAllow
 * ���ܣ� �б��е�����
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� yqyang    
 * ������ 2011-11-17
 *******************************************************************/
static void formConfigPPPoEAccAllow(webs_t wp, char *path, char *query)
{
    UINT32 Allow;
    char_t *AllowID = NULL;
    struProfAlloc *profhead = NULL;
    MibProfileType mibType = MIB_PROF_PPPOE_SRV_ACC;
    ProfChangeType profAction;
    PPPoESrvAccProfile *profA = NULL;
    int instIndex;
    const char *ErrMsg = NULL;

    Allow = (UINT32)strtoul(websGetVar(wp, T("Allow"), NULL),NULL,0);
    AllowID = websGetVar(wp, T("AllowID"), NULL);

    if (strlen(AllowID) == 0)
    {
        /* check variable */
        ErrMsg = getTransDataForC(C_LANG_INDEX_PLEASESELECT_USER);
        setTheErrorMsg(ErrMsg);
        goto out;
    }

    uttSemP(SEM_PPPOE_NO,0);/*����*/

    profAction = PROF_CHANGE_EDIT;
    instIndex = ProfGetInstPointByValue(mibType, OFFANDSIZE(PPPoESrvAccProfile, pppoename), TSTR, AllowID, -1);
    if(instIndex == PROFFAIL)
    {
        ErrMsg = getTransDataForC(C_LANG_INDEX_DID_NOT_FIND_CASE);
        setTheErrorMsg(ErrMsg);
        goto out;
    }
    ProfBackupByIndex(mibType, profAction, instIndex, &profhead);
    profA = (PPPoESrvAccProfile*)ProfGetInstPointByIndex(mibType, instIndex);
    if(0 == Allow)/*��ֹ*/
    {
        killOnePppoeClient(profA);	/*ɱ���ͻ��˽���*/
        profA->sesIndex = -1;		/*�ỰID��Ϊ-1��ʼֵ*/
        profA->actualSession = 0;	/*�ܵĻỰ����Ϊ0*/
    }
    profA->head.active = Allow;/*������ֹ*/

    uttSemV(SEM_PPPOE_NO,0);/*����*/

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
    websRedirect(wp, "PPPoEServer.asp");/*ҳ����ת*/
}

/***********************************************************************
 *   �� �� ����formConfigPppoeImport
 *   ������������������ʵ��
 *   �����������ҳ���ȡ����
 *   �����������
 *   �������ڣ�12-05-09
 *   �޸����ڣ�
 *   ���ߣ�  zkh 
 *   ����˵������
 **********************************************************************/
#if 1
#define MAX_REMARK_LENGTH	31
#define Maxlen			MAX_PPPOE_SRV_ACC_PROFILES*(MAX_REMARK_LENGTH + UTT_PPPOE_CLI_NAME_LEN + UTT_PPPOE_CLI_PASS_LEN+6)

extern void formConfigPppoeImport(webs_t wp, char *path, char *query)
{
    char *data, *username, *passwd, *remark;
    char *AccMACBinds,*bindmac, *bindmac2, *bindmac3, *bindmac4, *accmaxconv, *bindIp;
    char StrReplace[MAX_PPPOE_ACCOUNT_INFO_LEN];
    struct in_addr ip;
#if (PPPOE_ACCOUNT == FYES)
    char *chargeType, *tmStart, *tmStop;
    struct tm time;
#endif

#if (PPPOE_LIMIT == FYES)
    char *uRateLimit, *dRateLimit, *s_instIndex = NULL;
#endif
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;
    MibProfileType mibTypeA = MIB_PROF_PPPOE_SRV_ACC;
    char *pUser[MAX_PPPOE_SRV_ACC_PROFILES] = {0};
    char user[Maxlen];  /*MAX_PPPOE_SRV_ACC_PROFILES*31+10=23260*/
    int j=0, k=0, i=0;
    Uint16 webIndexInst = 0;
    char instId[UTT_INST_NAME_LEN + 1] = {0};
    int instIndex = 0;
    ProfChangeType profAction;
    struProfAlloc *profList  = NULL;
    PPPoESrvAccProfile *profA = NULL, profBak;
    const char *ErrMsg = NULL;
    data = websGetVar(wp, T("data"), NULL);
    memset(user, 0, sizeof(user));
    int len = strlen(data);
    for(j = 0;j<len;j++)               // * ���ű��еõ������ݿ����������� */
    {
        user[j] = data[j];
    }
    pUser[0] = strtok(user,";");
    if(!pUser[0])				// * ʵ����ϢΪ�� */
    {
        //SetWebLastMsg(T("������Ϣ����Ϊ�գ�"));
        websRedirect(wp,T("PPPoEImport.asp"));
        return ;
    }

    for(k = 1;k<MAX_PPPOE_SRV_ACC_PROFILES;k++)                        // * ��data�������ȡ��ʵ������ */
    {
        pUser[k] = strtok(NULL,";");
        if(!pUser[k])
        {
            break;
        }
    }
    for(i = 0;i<MAX_PPPOE_SRV_ACC_PROFILES;i++)
    {
        if(!pUser[i])
        {
            break;
        }
        username = strtok(pUser[i], " ");	// * �õ��û��� */
        passwd = strtok(NULL, " ");		// * �õ����� */ 
        AccMACBinds = strtok(NULL, " ");//�󶨷�ʽ
        bindmac = strtok(NULL, " "); //�õ���mac
        bindmac2 = strtok(NULL, " ");
        bindmac3 = strtok(NULL, " ");
        bindmac4 = strtok(NULL, " ");
        accmaxconv = strtok(NULL, " ");//���Ự��
        bindIp = strtok(NULL, " ");
#if (PPPOE_ACCOUNT == FYES)
        chargeType = strtok(NULL, " ");//�Ʒ�ģʽ
        tmStart = strtok(NULL, " ");//�˺ſ�ͨ����
        tmStop = strtok(NULL, " ");//��������
#endif
#if (PPPOE_LIMIT == FYES)
        uRateLimit = strtok(NULL, " ");//�ϴ�����
        dRateLimit = strtok(NULL, " ");//����
        Uint16 txRate;
        Uint16 rxRate;
#endif
        remark = strtok(NULL, " ");		// * �õ�������Ϣ */
        if ((username == NULL) || (passwd == NULL) || (AccMACBinds == NULL) || (bindIp == NULL) || (accmaxconv == NULL)
                || (bindmac == NULL) || (bindmac2 == NULL) || (bindmac3 == NULL) || (bindmac4 == NULL) || (remark == NULL)
#if (PPPOE_ACCOUNT == FYES)
                || (chargeType == NULL) || (tmStop == NULL) || (tmStart == NULL)
#endif
#if (PPPOE_LIMIT == FYES)
                ||(uRateLimit == NULL) || (dRateLimit == NULL)
#endif
                ){
            setTheErrorMsg("������Ϣ����");
            goto out;
        }
/*    printf("%s----%d---------username---%s\n", __func__, __LINE__, username);
    printf("%s----%d---------passwd---%s\n", __func__, __LINE__, passwd);
    printf("%s----%d---------AccMACBinds---%s\n", __func__, __LINE__, AccMACBinds);
    printf("%s----%d---------bindmac---%s\n", __func__, __LINE__, bindmac);
    printf("%s----%d---------bindmac2---%s\n", __func__, __LINE__, bindmac2);
    printf("%s----%d---------bindmac3---%s\n", __func__, __LINE__, bindmac3);
    printf("%s----%d---------bindmac4---%s\n", __func__, __LINE__, bindmac4);
    printf("%s----%d---------accmaxconv---%s\n", __func__, __LINE__, accmaxconv);
    printf("%s----%d---------bindIp---%s\n", __func__, __LINE__, bindIp);
    printf("%s----%d---------chargeType---%s\n", __func__, __LINE__, chargeType);
    printf("%s----%d---------tmStart---%s\n", __func__, __LINE__, tmStart);
    printf("%s----%d---------tmStop---%s\n", __func__, __LINE__, tmStop);
    printf("%s----%d---------TXrate---%s\n", __func__, __LINE__, dRateLimit);
    printf("%s----%d---------RXrate---%s\n", __func__, __LINE__, uRateLimit);
    printf("%s----%d---------remark---%s\n", __func__, __LINE__,remark );
*/
/*
        PPPOE_SRV_PRINTF("username = %s\n",username);
        PPPOE_SRV_PRINTF("passwd = %s\n", passwd);
        PPPOE_SRV_PRINTF("remark = %s\n",remark);
        if ((username == NULL) || (passwd == NULL)) {
            ErrMsg = getTransDataForC(C_LANG_INDEX_CONFIG_ARGS_ERR);
            setTheErrorMsg(ErrMsg);
            goto out;
        }
*/

        /* * �û����Ƿ��ظ� */
        if (username != NULL && (ProfGetInstPointByValue(mibTypeA, OFFANDSIZE(PPPoESrvAccProfile, pppoename), TSTR, username, -1) != PROFFAIL)) {
            ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_ALREADY_EXITS);
            setTheErrorMsg(ErrMsg);
            goto out;
        }

        webIndexInst = getWebInstName(&(nvramProfile->pppoeInstRec.instRecMap), &(nvramProfile->pppoeInstRec.instNameIndex));
        /* portmap���� */
        if (webIndexInst == 0) {
            ErrMsg = getTransDataForC(C_LANG_INDEX_OPERATION_FAIL);
            setTheErrorMsg(ErrMsg);
            goto out;
        }
        snprintf(instId, UTT_INST_NAME_LEN, "ID%d", webIndexInst);
        /* �Ѵ���ʵ���� */
        if (ProfGetInstIndexByName(mibTypeA, instId) != PROFFAIL) {
            ErrMsg = getTransDataForC(C_LANG_INDEX_CASE_ALREADY_EXITS);
            setTheErrorMsg(ErrMsg);
            goto out;
        }

       //�жϹ̶�ip�Ϸ��� 
        profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS, 0);
        if(strncmp(bindIp, EXPORT_IMPORT_NOT_CONFIGURED_STRING, 1) == 0) {
            ip.s_addr = 0;
        } else {
            if(0 == inet_aton(bindIp, &ip)) {
                ErrMsg = getTransDataForC(C_LANG_INDEX_STATIC_IP_ADDR_CIN_ERR);
                setTheErrorMsg(ErrMsg);                                        
                goto out; 
            }
            //if ((ProfGetInstPointByValue(mibTypeA, OFFANDSIZE(PPPoESrvAccProfile, bindIp), TIPADDR, bindIp, instIndex) != PROFFAIL)) {
            if ((ProfGetInstPointByValue(mibTypeA, OFFANDSIZE(PPPoESrvAccProfile, bindIp), TIPADDR, bindIp, -1) != PROFFAIL)) {
                ErrMsg = getTransDataForC(C_LANG_INDEX_STATIC_IP_ADDR_ALREADY_EXITS);
                setTheErrorMsg(ErrMsg);
                goto out;
            }
            if(profS->head.active == TRUE){
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
                if(ntohl(ip.s_addr) < ntohl(profS->ipStart) || ntohl(ip.s_addr) >=  ntohl(profS->ipStart) + profS->ipcount){
#else
                if(ntohl(ip.s_addr) < ntohl(profS->ipStart) || ntohl(ip.s_addr) >=  ntohl(profS->ipStart) + profS->maxSessCnt){
#endif
                    ErrMsg = getTransDataForC(C_LANG_INDEX_STATIC_IP_ADDR_CANT_IN_PPPoE_POOL);
                    setTheErrorMsg(ErrMsg);
                    goto out;
                }
            }
        }

        s_instIndex = websGetVar(wp, T("instIndex"), T("")); 
        if(strncmp(uRateLimit, EXPORT_IMPORT_NOT_CONFIGURED_STRING, 1) == 0) {
            txRate = 0;
        } else {
            txRate = strtoul(uRateLimit, NULL, 0);
        }
        if(strncmp(dRateLimit, EXPORT_IMPORT_NOT_CONFIGURED_STRING, 1) == 0) {
            rxRate = 0;
        } else {
            rxRate = strtoul(dRateLimit, NULL, 0);
        }
        if (checkPPPOEAccLimitNum(txRate, rxRate, s_instIndex) == 1) {
            ErrMsg = getTransDataForC(C_LANG_INDEX_PPPOE_LIMIT_CANT_MORE);
            setTheErrorMsg(ErrMsg);
            goto out;
        }

        instIndex = ProfNewInst(mibTypeA, instId, FALSE);
        if(instIndex == PROFFAIL) {
            clearPortMap(&(nvramProfile->pppoeInstRec.instRecMap), webIndexInst);
            ErrMsg = getTransDataForC(C_LANG_INDEX_MAXIMUM_NUM_HAS_REACHED);
            setTheErrorMsg(ErrMsg);
            goto out;
        }
                profAction = PROF_CHANGE_ADD;
        /*
         *�˴�Ϊ�����ź��������˲���
         */
        uttSemP(SEM_PPPOE_NO,0);
        ProfBackupByIndex(mibTypeA, profAction, instIndex, &profList);
        profA = (PPPoESrvAccProfile *)ProfGetInstPointByIndex(mibTypeA, instIndex);
        memcpy(&profBak, profA, sizeof(profBak));/*�ȱ���ԭ����*/
 
        /* ����Ϊʵ����  */
        ProfSetNameByPoint(profA, instId);
        strncpy(profA->pppoename, username, UTT_PPPOE_NAME_LEN);
        strncpy(profA->passwd, passwd, UTT_PPPOE_CLI_PASS_LEN);
        strcpy(profA->AccMACBind, AccMACBinds);
        profA->bindIp = ip.s_addr;

        if(strncmp(bindmac, EXPORT_IMPORT_NOT_CONFIGURED_STRING, 1) == 0) {
            memset(StrReplace, 0, sizeof(StrReplace));
            strcpy(StrReplace, "000000000000");
            converMac12To6Byte(StrReplace, &(profA->bindmac));
        } else {
            converMac12To6Byte(bindmac, &(profA->bindmac));
        }
        if(strncmp(bindmac2, EXPORT_IMPORT_NOT_CONFIGURED_STRING, 1) == 0) {
            memset(StrReplace, 0, sizeof(StrReplace));
            strcpy(StrReplace, "000000000000");
            converMac12To6Byte(StrReplace, &(profA->bindmac2));
        } else {
            converMac12To6Byte(bindmac2, &(profA->bindmac2));
        }
        if(strncmp(bindmac3, EXPORT_IMPORT_NOT_CONFIGURED_STRING, 1) == 0) {
            memset(StrReplace, 0, sizeof(StrReplace));
            strcpy(StrReplace, "000000000000");
            converMac12To6Byte(StrReplace, &(profA->bindmac3));
        } else {
            converMac12To6Byte(bindmac3, &(profA->bindmac3));
        }
        if(strncmp(bindmac4, EXPORT_IMPORT_NOT_CONFIGURED_STRING, 1) == 0) {
            memset(StrReplace, 0, sizeof(StrReplace));
            strcpy(StrReplace, "000000000000");
            converMac12To6Byte(StrReplace, &(profA->bindmac4));
        } else {
            converMac12To6Byte(bindmac4, &(profA->bindmac4));
        }
        profA->accmaxconv = strtoul(accmaxconv, NULL, 0);

#if (PPPOE_ACCOUNT == FYES)
        if(strncmp(chargeType, EXPORT_IMPORT_NOT_CONFIGURED_STRING, 1) == 0) {
            profA->chargeType = NONE_CHARG_MODE;
        } else {
            profA->chargeType = DATE_CHARG_MODE;
            memset(&time, 0, sizeof(struct tm));
            strptime(tmStart, "%Y-%m-%d", &time);
            profA->tmStart = mktime(&time);
            memset(&time, 0, sizeof(struct tm));
            strptime(tmStop, "%Y-%m-%d", &time);
            profA->tmStop= mktime(&time) + 86339; //86339 == 23:59:59
        }
#endif

#if (PPPOE_LIMIT == FYES)
        profA->uRateLimit = txRate;
        profA->dRateLimit = rxRate;
#endif
        if(strncmp(remark, EXPORT_IMPORT_NOT_CONFIGURED_STRING, 1) == 0)
        {
            strcpy(profA->remark, "");
        }
        else
        {
            strcpy(profA->remark, remark);
        }

        killOnePppoeClient(&profBak);
        profA->sesIndex = -1;
        profA->actualSession = 0;
        SWORD_PRINTF("%s - %d, profA->pppoename = %s,profA->actualSession = %d, profA->accmaxconv= %d\n",__FUNCTION__, __LINE__, profA->pppoename, profA->actualSession, profA->accmaxconv);
        /*
         *�˴�Ϊ���źŽ���
         */
        uttSemV(SEM_PPPOE_NO,0);
    } 

    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
out:
    websRedirect(wp, "PPPoEServer.asp");
    return ;
}
/**************************************
 * ��PPPoEû�����õ�ĳ������Ϊ�ض��ķ���
 * �Ա㵼��ʱ�������֣�
 * ************************************/
void checkToSetUnconfigured(void *toString, int opt)
{
    int flag = 0;
    switch(opt)
    {
        case 1:
            if (strncmp((unsigned char *)toString, "000000000000", 12) == 0) //��mac���
                flag = 1; 
            break;
        case 2:
            if (strncmp(toString, "0.0.0.0", 7) == 0)//�̶�ip���
                flag = 1; 
            break;
        case 3:
            if (strncmp(toString, "0", 1) == 0) //���ؼ��, �������ü��
                flag = 1;
            break;
        case 4:
            if (strncmp(toString, "1970-01-01", 8) == 0) //������� 
                flag = 1;
            break;
        case 5:
            flag = 1;
            break;
        default:
            break;
    } 
    if(flag == 1)
    {
        strcpy(toString, EXPORT_IMPORT_NOT_CONFIGURED_STRING);
    }
}

/***************************
 *��PPPoE�ʺ���Ϣת�����ַ���
 * ************************/
void convertToString(void *src, char*toString, int opt)
{
    memset(toString, 0, MAX_PPPOE_ACCOUNT_INFO_LEN);
    switch(opt)
    {
        case 1: 
            {
                unsigned char *mac = (unsigned char *)src;//careful, unsigned char 
                sprintf(toString, "%02x%02x%02x%02x%02x%02x",mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                checkToSetUnconfigured(toString, 1);
                break;
            }
        case 2: 
            {
                unsigned int *pInt = (unsigned int *)src;
                sprintf(toString, "%d", *pInt);
                checkToSetUnconfigured(toString, 3);//�ϴ���������
                break;
            }
        case 3: 
            {
                unsigned int *ip = (unsigned int *)src;
                struct in_addr ipAddr;
                ipAddr.s_addr = *ip;
                sprintf(toString, "%s", inet_ntoa(ipAddr)); 
                checkToSetUnconfigured(toString, 2);//����Ƿ�Ϊ0.0.0.0
                break;
            }
        case 4:
            {
                unsigned int *chargeType = (unsigned int *)src;
                sprintf(toString, "%d", *(chargeType) == NONE_CHARG_MODE ? NONE_CHARG_MODE : DATE_CHARG_MODE);
                checkToSetUnconfigured(toString, 3);//����Ƿ�Ϊ0
                break;
            }
        case 5:
            {
                unsigned int *time = (unsigned int *)src;
                struct tm *date = localtime((time_t *)time);
                sprintf(toString, "%d-%02d-%02d", 1900+date->tm_year, 1+date->tm_mon, date->tm_mday);
                checkToSetUnconfigured(toString, 4);//����Ƿ���1970-01-01
                break;
            }
        case 6:
            {
                if (strlen((char *)src) == 0) //remarkδ��д
                {
                    checkToSetUnconfigured(toString, 5);
                }
                else
                {
                    sprintf(toString, "%s", (char *)src); 
                }
                break;
            }
        default:
            break;
    }
}

/***********************************
 *formConfigPppoeExport(webs_t wp, char *path, char *query)
 *
 *action= /goform/formPppoeExport
 *
 *write PPPoE information to assigned file setted by webpage
 *
 *add by zkh
 ************************************/
static Boolean ReadPppoeUserFromProfile(char *buf)
{
    MibProfileType mibTypeA = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *profA = NULL;
    int min = 0, max = 0, i = 0, totalrecs = 0;
    ProfInstLowHigh(mibTypeA, &max, &min);
    /* �˴�Ϊ�����ź��������˲��� */
    uttSemP(SEM_PPPOE_NO,0);
    char toString[MAX_PPPOE_ACCOUNT_INFO_LEN];
    for (i = min; i < max; i++)
    {
        profA = (PPPoESrvAccProfile *)ProfGetInstPointByIndex(mibTypeA, i);
        if((profA != NULL) && (ProfInstIsFree(profA) == 0)) {
            if( Maxlen > ( strlen(buf) + strlen(profA->pppoename) ) ) {
                strcat(buf,profA->pppoename);//�û���
            }
            if( Maxlen > ( strlen(buf) + strlen(" ") ) ) { 
                strcat(buf," ");
            }
            if( Maxlen > ( strlen(buf) + strlen(profA->passwd) ) ) {
                strcat(buf,profA->passwd);//����
            }
            if( Maxlen > ( strlen(buf) + strlen(" ") ) ) {
                strcat(buf," ");
            }
            if (Maxlen > (strlen(buf) + strlen(profA->AccMACBind))) //�󶨷�ʽ 
            {
               strcat(buf, profA->AccMACBind); // handbind, autobind, nobind
            }
            if( Maxlen > ( strlen(buf) + strlen(" ") ) ) {
                strcat(buf," ");
            }
            if(Maxlen > (strlen(buf) + strlen("0022aa112233"))) {  //һ��mac���ĳ���
                convertToString(&(profA->bindmac), toString, 1);
                strcat(buf, toString);
            }
            if(Maxlen > (strlen(buf) + strlen(" "))){
                strcat(buf," ");
            } 
            if(Maxlen > (strlen(buf) + strlen("0022aa112233"))) { 
                convertToString(&(profA->bindmac2), toString, 1);
                strcat(buf, toString);
            }
            if(Maxlen > (strlen(buf) + strlen(" "))){
                strcat(buf," ");
            } 
            if(Maxlen > (strlen(buf) + strlen("0022aa112233"))) {
                convertToString(&(profA->bindmac3), toString, 1);
                strcat(buf, toString);
            }
            if(Maxlen > (strlen(buf) + strlen(" "))){
                strcat(buf," ");
            } 
            if(Maxlen > (strlen(buf) + strlen("0022aa112233"))) {
                convertToString(&(profA->bindmac4), toString, 1);
                strcat(buf, toString);
            }
            if(Maxlen > (strlen(buf) + strlen(" "))){
                strcat(buf," ");
            } 
            if(Maxlen > (strlen(buf) + strlen("100"))) {//���Ự��,MAX_PPPOE_SRV_SESSION_PROFILES 
                convertToString(&(profA->accmaxconv), toString, 2);
                strcat(buf, toString);
            }
            if(Maxlen > (strlen(buf) + strlen(" "))){
                strcat(buf," ");
            }           
            if(Maxlen > (strlen(buf) + strlen("255.255.255.255"))) { //�̶�IP��ַ,ȡ����ip����
                convertToString(&(profA->bindIp), toString, 3);
                strcat(buf, toString);
            }
            if(Maxlen > (strlen(buf) + strlen(" "))){
                strcat(buf," ");
            } 
#if (PPPOE_ACCOUNT == FYES)
            if(Maxlen > (strlen(buf) + strlen("0"))) { //�Ʒ�ģʽ 0,1��ͬģʽ
                convertToString(&(profA->chargeType), toString, 4);
                strcat(buf, toString);
            }
            if(Maxlen > (strlen(buf) + strlen(""))){
                strcat(buf," ");
            }
            if(Maxlen > (strlen(buf) + strlen("20150101"))) { //��ͨ����
                convertToString(&(profA->tmStart), toString, 5); 
                strcat(buf, toString);
            }
            if(Maxlen > (strlen(buf) + strlen(" "))){
                strcat(buf," ");
            }
            if(Maxlen > (strlen(buf) + strlen("20151231"))) {
                convertToString(&(profA->tmStop), toString, 5);
                strcat(buf, toString);//ͣ������
            }
            if(Maxlen > (strlen(buf) + strlen(" "))){
                strcat(buf," ");
            }
#endif
#if (PPPOE_LIMIT == FYES)
            if(Maxlen > (strlen(buf) + strlen("102400"))) {//�ϴ�
                convertToString(&(profA->uRateLimit), toString, 2);
                strcat(buf, toString);
            }
            if(Maxlen > (strlen(buf) + strlen(" "))){
                strcat(buf," ");
            }
            if(Maxlen > (strlen(buf) + strlen("102400"))) {//����
                convertToString(&(profA->dRateLimit), toString, 2);
                strcat(buf, toString);
            }
            if(Maxlen > (strlen(buf) + strlen(" "))){
                strcat(buf," ");
            }
#endif
            if( Maxlen > ( strlen(buf) + strlen(profA->remark))) {//��ע
                convertToString(profA->remark, toString, 6);
                strcat(buf, toString);
            }
            if( Maxlen > ( strlen(buf) + strlen("\r\n") ) ) {
                strcat(buf,"\r\n");
            }
            totalrecs++;
        }
    }

    /* *�˴�Ϊ���źŽ��� */
    uttSemV(SEM_PPPOE_NO,0);
    return TRUE;
}

extern void formConfigPppoeExport(webs_t wp, char *path, char *query)
{
    char filename[20] = {0};
    char buf[Maxlen];
    int len = 0;

    PPPOE_SRV_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
    memset( buf, 0, Maxlen );
    memset( filename, 0, 20);
    strcpy(filename, "PPPoEserverinfo.txt");

    ReadPppoeUserFromProfile(buf);
    len = strlen(buf);
    wimDownloadFile(wp,filename,buf,len);
    //websRedirect(wp, "PPPoEServer.asp");
    PPPOE_SRV_PRINTF("%s-%d:%s\n",__FUNCTION__,__LINE__,buf);
    return;
}
#endif
#if (PPPOE_SELF_HELP == FYES)
static char selfhelpMsg[SELF_HELP_MSG_LEN+1]={0};

static void setSelfHelpMsg(char *buf)
{
    if(buf == NULL)
    {
        memset(selfhelpMsg, 0, SELF_HELP_MSG_LEN+1);	/*make selfhelpMsg NULL*/
    }
    else
    {
        if(strlen(buf) <= SELF_HELP_MSG_LEN)
        {
            strcpy(selfhelpMsg, buf);	/*selfhelp msg*/
        }
    }
    return;
}
/***********************************
 * isIpInPool(IPADDR ip)
 * �ж��û���ip�Ƿ���PPPoE��ַ����
 * input:   
 *	ip: �û�IP�������ֽ���
 * output: 
 *	1:  ��PPPoE��ַ����
 *	0:  ����PPPoE��ַ����
 * 2012-06-07
 ************************************/
static int isIpInPool(IPADDR ip)
{
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;
    int ret=0;
    /*global profile inst*/
    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS, 0);
    PPPOE_SRV_PRINTF("%s-%d:ipStart = %x, ntohl(ipStart)=%x, ntohl(ipStart)+maxSessionCnt=%x, maxSession = %d\n", __FUNCTION__,__LINE__,profS->ipStart,ntohl(profS->ipStart),ntohl(profS->ipStart)+profS->maxSessCnt,profS->maxSessCnt);
    if((ip >= ntohl(profS->ipStart)) && (ip <= (ntohl(profS->ipStart)+profS->maxSessCnt-1)))	/*find*/
    {
        PPPOE_SRV_PRINTF("%s-%d: \n",__FUNCTION__,__LINE__);
        ret = 1;
    }
    else
    {
        ret = 0;
    }
    PPPOE_SRV_PRINTF("%s-%d: ret=%d\n",__FUNCTION__,__LINE__,ret);
    return ret;
}

#if 0
/***********************************
 * PPPoESrvGetUserNameByIp(IPADDR ip)
 * �����û�IP����PPPoE�˻���
 * input:   
 *	ip:	�û�IP�������ֽ���
 * output:  
 *	inst	index
 *	or	-1
 * 2012-06-07
 ************************************/
static int PPPoESrvGetUserNameByIp(IPADDR ip)
{
    MibProfileType mibType = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *prof=NULL;
    int i=0,min=0,max=0,instIndex=-1;

    ProfInstLowHigh(mibType, &max, &min);
    for(i=min;i<max;i++)
    {
        prof = (PPPoESrvAccProfile *)ProfGetInstPointByIndex(mibType, i);
        if((prof != NULL)&&(prof->ConTime==0)&&(ProfInstIsFree(prof) == 0))
        {
            if(ntohl(inet_addr(prof->ip)) == ip)    /*find*/
            {
                PPPOE_SRV_PRINTF("%s-%d: prof->ip=%s,after change=%x\n",__FUNCTION__,__LINE__,prof->ip,inet_addr(prof->ip));
                PPPOE_SRV_PRINTF("%s-%d: pppoename=%s\n",__FUNCTION__,__LINE__,prof->pppoename);
                instIndex = i;	/*return inst index*/
                break;
            }
        }
    }
    return instIndex;
}
#else
/***********************************
 * PPPoESrvGetUserNameByIp(IPADDR ip)
 * �����û�IP����PPPoE�˻���
 * input:   
 *	ip:	�û�IP�������ֽ���
 * output:  
 *	inst	index
 *	or	-1
 * 2012-08-24
 ************************************/
static int PPPoESrvGetUserNameByIp(IPADDR ip)
{
    MibProfileType mibType = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *prof = NULL;

    int i=0, instIndex=-1;

    prof = (PPPoESrvGloProfile*)ProfGetInstPointByIndex(mibType, 0);

    if(prof != NULL)
    {
        i = ip - ntohl(prof->ipStart);
        SWORD_PRINTF("%s-%d: ntohl(ip)=%x, ntohl(prof->ipStart)=%x,i = %d\n",__FUNCTION__,__LINE__,ip,ntohl(prof->ipStart), i);
        if(SesIndexIsOk(i) == 1)
        {
            if(nvramProfile->Session[i].ConTime == 0U)
            {
                instIndex = nvramProfile->Session[i].accIndex;
            }
        }
    }
    return instIndex;
}
#endif

/***********************************
 *formPppoeSelfHelpConfig(webs_t wp, char *path, char *query)
 *
 *action= /goform/formPppoeSelfHelpConfig
 *
 * 2012-06-07
 ************************************/
static int aspGetPppoeSelfHelpData(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;
    MibProfileType mibType = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *prof=NULL;
    IPADDR userip=0U;
    int getIndex=-1;

    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS, 0);
    if((profS->head.active != 1) || (profS->self_help !=  FUN_ENABLE))
    {
        websWrite(wp, "var isPPPoEIp=%s;\n", T("false"));
        return 0;
    }
    /*��ȡ�û�ip�������ֽ���*/
    userip = inet_addr((const char *)wp->ipaddr);
    PPPOE_SRV_PRINTF("%s-%d: get userip=%x, ntohl(userip)=%x\n",__FUNCTION__,__LINE__,userip,ntohl(userip));
    /*�ж��û�ip�Ƿ���PPPoE��ַ����*/
    if(isIpInPool(ntohl(userip)) == 1)
    {
        /*�ж��û�IP�Ƿ���ʹ��pppoe�˺Ų�������*/
        if((getIndex=PPPoESrvGetUserNameByIp(ntohl(userip))) != -1)
        {
            PPPOE_SRV_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
            websWrite(wp, "var isPPPoEIp=%s;",T("true"));
        }
        else
        {
            PPPOE_SRV_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
            websWrite(wp, "var isPPPoEIp=%s;",T("false"));
        }
    }
    else
    {
        PPPOE_SRV_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
        websWrite(wp, "var isPPPoEIp=%s;",T("false"));
    }
    PPPOE_SRV_PRINTF("%s-%d: getIndex=%d\n",__FUNCTION__,__LINE__,getIndex);
    if(getIndex != -1)
    {	/*write user modify count value*/
        prof = (PPPoESrvAccProfile *)ProfGetInstPointByIndex(mibType, getIndex);
        PPPOE_SRV_PRINTF("%s-%d:pppoeName=%s,optCnt=%u\n",__FUNCTION__,__LINE__,prof->pppoename,prof->optCnt);
        if(prof!=NULL)
        {
            PPPOE_SRV_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
            websWrite(wp, "var userOptCnt=%d;", MAX_USER_OPT_CNT-prof->optCnt);
        }
        else
        {
            PPPOE_SRV_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
            websWrite(wp, "var userOptCnt=%d;", MAX_USER_OPT_CNT);
        }
    }
    else
    {
        PPPOE_SRV_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
        websWrite(wp, "var userOptCnt=%d;", MAX_USER_OPT_CNT);
    }
    PPPOE_SRV_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
    websWrite(wp, "var optResult=\"%s\";", selfhelpMsg);    /*selfhelp msg*/
    setSelfHelpMsg((char *)NULL);
    return 0;
}
#if 0
/***********************************
 * pppoeSrvIpUseName(IPADDR ip, char *name)
 * �жϴ�ip�Ƿ����ô��û�����������
 * input:
 *	ip:	���жϵ�ip�������ֽ���
 *	name:	���жϵ��û���
 * output:
 *	-1:	��ipΪʹ�ô��û���
 *	instIndex
 *	2012-06-07
 ************************************/
static int pppoeSrvIpUseName(IPADDR ip, char *name)
{
    MibProfileType mibType = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *prof = NULL;
    int i = 0, min = 0, max = 0, ret=-1;
    /*get inst min value and max value*/
    ProfInstLowHigh(mibType, &max, &min);
    PPPOE_SRV_PRINTF("%s-%d: param ip=%x, name=%s, min=%d, max=%d\n",__FUNCTION__,__LINE__,ip,name,min,max);
    /*search form min to max*/
    for(i=min;i<max;i++)
    {
        /*get inst pointer*/
        prof = (PPPoESrvAccProfile *)ProfGetInstPointByIndex(mibType, i);
        PPPOE_SRV_PRINTF("%s-%d: prof->ip=%x\n",__FUNCTION__,__LINE__,inet_addr(prof->ip));
        /*judge ip and name*/
        if((prof!=NULL)&&(prof->ConTime==0)&&(ip==ntohl(inet_addr(prof->ip)))&&(strcmp(name, prof->pppoename)==0))
        {
            PPPOE_SRV_PRINTF("%s-%d;prof->ip=%x\n",__FUNCTION__,__LINE__,ntohl(inet_addr(prof->ip)));
            ret = i;
            break;
        }
    }
    PPPOE_SRV_PRINTF("%s-%d: ret=%d\n", __FUNCTION__,__LINE__,ret);
    return ret;
}
#else
/***********************************
 * pppoeSrvIpUseName(IPADDR ip, char *name)
 * �жϴ�ip�Ƿ����ô��û�����������
 * input:
 *	ip:	���жϵ�ip�������ֽ���
 *	name:	���жϵ��û���
 * output:
 *	-1:	��ipΪʹ�ô��û���
 *	instIndex
 *	2012-08-24
 ************************************/
static int pppoeSrvIpUseName(IPADDR ip, char *name)
{
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *prof = NULL;

    MibProfileType mibType = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *info = NULL;
    int i = 0, ret=-1;

    prof = (PPPoESrvGloProfile*)ProfGetInstPointByIndex(mibTypeS, 0);

    if(prof != NULL)
    {
        i = ip - ntohl(prof->ipStart);
        SWORD_PRINTF("%s-%d: ntohl(ip)=%x, ntohl(prof->ipStart)=%x,i = %d\n",__FUNCTION__,__LINE__,ip,ntohl(prof->ipStart), i);
        if(SesIndexIsOk(i) == 1)
        {
            info = (PPPoESrvAccProfile*)ProfGetInstPointByIndex(mibType, nvramProfile->Session[i].accIndex);
            if((nvramProfile->Session[i].ConTime == 0U) && (strcmp(name, info->pppoename)==0))
            {
                ret = nvramProfile->Session[i].accIndex;
            }
        }
    }
    PPPOE_SRV_PRINTF("%s-%d: ret=%d\n", __FUNCTION__,__LINE__,ret);
    return ret;
}
#endif

/***********************************
 * checkData(IPADDR ip, char *name, char *oldpwd, char *newpwd, char *verifypwd)
 * ���ҳ���ύ�����ĺϷ���
 * input��
 *	    ip:	    �û�IP�������ֽ���
 *	  name:	    pppoename
 *	oldpwd:     ������
 *	newpwd:	    ������
 *   verifypwd:     ȷ������
 * ouiput:
 *	     1:	    �Ϸ�
 *	     0:	    ���Ϸ�
 * 2012-06-07
 ************************************/
static int checkData(IPADDR ip, char *name, char *oldpwd, char *newpwd, char *verifypwd)
{
    PPPOE_SRV_PRINTF("%s-%d: param ip=%x, name=%s, oldpwd=%s, newpwd=%s, verifypwd=%s\n",__FUNCTION__,__LINE__,ip,name,oldpwd,newpwd,verifypwd);
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;
    int ret=1;
    const char *ErrMsg = NULL;

    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS, 0);
    PPPOE_SRV_PRINTF("%s-%d: \n",__FUNCTION__,__LINE__);
    /*���ܿ����Ƿ���*/
    if((profS->head.active != 1) || (profS->self_help !=  FUN_ENABLE))
    {
        PPPOE_SRV_PRINTF("%s-%d: \n",__FUNCTION__,__LINE__);
        ErrMsg = getTransDataForC(C_LANG_INDEX_NO_JURISDICTION_FOR_O_PERATION);
        setSelfHelpMsg((char *)ErrMsg);
        ret=0;
    }
    else
    {
        /*�ж���������ĳ��ȼ��*/
        if((strlen(name)==0)||(strlen(oldpwd)==0)||(strlen(newpwd)==0)||(strlen(name)>31)||(strlen(oldpwd)>31)||(strlen(newpwd)>31))
        {
            PPPOE_SRV_PRINTF("%s-%d: \n",__FUNCTION__,__LINE__);
            ErrMsg = getTransDataForC(C_LANG_INDEX_SUBMIT_WRONG_PARMETERS_RECIN);
            setSelfHelpMsg((char *)ErrMsg);
            ret=0;
        }
        else
        {
            /*�ж���������ȷ�������Ƿ���ͬ*/
            if(strcmp(newpwd, verifypwd)!=0)
            {
                PPPOE_SRV_PRINTF("%s-%d: \n",__FUNCTION__,__LINE__);
                ErrMsg = getTransDataForC(C_LANG_INDEX_SUBMIT_WRONG_PARMETERS_PASSWD_DIFF);
                setSelfHelpMsg((char *)ErrMsg);
                ret=0;
            }
#if 0
            else
            {
                if(pppoeSrvIpUseName(ip, name)==0)
                {
                    PPPOE_SRV_PRINTF("%s-%d: \n",__FUNCTION__,__LINE__);
                    setSelfHelpMsg(T("��û��Ȩ�޽��д˲�����"));
                    ret=0;
                }
            }
#endif
        }
    }
    PPPOE_SRV_PRINTF("%s-%d: \n",__FUNCTION__,__LINE__);
    return ret;
}
/***********************************
 *formPppoeSelfHelpConfig(webs_t wp, char *path, char *query)
 *
 *action= /goform/formPppoeSelfHelpConfig
 *
 *2012-06-07
 ************************************/
static void formPppoeSelfHelpConfig(webs_t wp, char *path, char *query)
{
    MibProfileType mibType = MIB_PROF_PPPOE_SRV_ACC;
#if 0
    PPPoESrvAccProfile *prof = NULL, *profBuf=NULL;
#else
    PPPoESrvAccProfile *prof = NULL;
#endif
    struProfAlloc *profList  = NULL;

#if 0
    int i = 0, min = 0, max = 0, instIndex=-1;
#else
    int instIndex=-1;
#endif
    IPADDR userip = 0U;
    char pwdBuf[UTT_PPPOE_CLI_PASS_LEN+ 1];
    char *username = NULL, *oldpwd = NULL, *newpwd = NULL, *verifypwd = NULL;
    const char *ErrMsg = NULL;

    memset(pwdBuf, 0, UTT_PPPOE_CLI_PASS_LEN+ 1);
    /*ת��Ϊ��·�ֽ����ip*/
    userip = inet_addr((const char *)wp->ipaddr);
    username = websGetVar(wp, T("userName"), T(""));
    oldpwd = websGetVar(wp, T("oldPasswd"), T(""));
    newpwd = websGetVar(wp, T("newPasswd"), T(""));
    verifypwd = websGetVar(wp, T("verifyPasswd"), T(""));
    PPPOE_SRV_PRINTF("%s-%d: userip=%x, username=%s,oldpwd=%s,newpwd=%s,verifypwd=%s\n",__FUNCTION__,__LINE__,userip,username,oldpwd,newpwd,verifypwd);
    if(checkData(ntohl(userip), username, oldpwd, newpwd, verifypwd)==0)
    {
        PPPOE_SRV_PRINTF("%s-%d: \n",__FUNCTION__,__LINE__);
        websRedirect(wp, "poeUsers.asp");
        return;
    }
#if 0
    ProfInstLowHigh(mibType, &max, &min);
    for(i=min;i<max;i++)
    {
        profBuf = (PPPoESrvAccProfile *)ProfGetInstPointByIndex(mibType, i);
        if(profBuf != NULL)
        {
            if(strcmp(profBuf->pppoename, username)==0)
            {
                prof = profBuf;
                instIndex=i;
                break;
            }
        }
    }
#endif
    instIndex=pppoeSrvIpUseName(ntohl(userip), username);
    PPPOE_SRV_PRINTF("%s-%d: isntIndex=%d\n",__FUNCTION__,__LINE__,instIndex);
    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, instIndex, &profList);
    if(instIndex != -1)
    {
        prof = (PPPoESrvAccProfile *)ProfGetInstPointByIndex(mibType, instIndex);
#if 0
        if((prof!=NULL)&&(prof->ConTime==0)&&(prof->optCnt<MAX_USER_OPT_CNT))	/*check*/
#else
            if((prof!=NULL)&&(prof->optCnt<MAX_USER_OPT_CNT))	/*check*/
#endif
            {
                strcpy(pwdBuf, prof->passwd);
                if(strcmp(pwdBuf, oldpwd) == 0) /*save*/
                {
                    memset(prof->passwd, 0, UTT_PPPOE_CLI_PASS_LEN+ 1);
                    strcpy(prof->passwd, newpwd);
                    PPPOE_SRV_PRINTF("%s-%d: instIndex=%d, username=%s, set passwd =%s\n",__FUNCTION__,__LINE__,instIndex,prof->pppoename,prof->passwd);
                    prof->optCnt++;
                    ErrMsg = getTransDataForC(C_LANG_INDEX_SUCCESSFUL_OPERATION);
                    setSelfHelpMsg((char *)ErrMsg);
                }
                else
                {
                    ErrMsg = getTransDataForC(C_LANG_INDEX_OLD_PASSWD_CIN_ERR);
                    setSelfHelpMsg((char *)ErrMsg);
                }
            }
            else
            {
                ErrMsg = getTransDataForC(C_LANG_INDEX_HAVE_CHANGED_5_TIMES_CANT_TODAY);
                setSelfHelpMsg((char *)ErrMsg);
            }
    }
    else
    {

        ErrMsg = getTransDataForC(C_LANG_INDEX_NO_JURISDICTION_FOR_O_PERATION);
        setSelfHelpMsg((char *)ErrMsg);
    }
    ProfUpdate(profList);   /*update*/
    ProfFreeAllocList(profList);
    nvramWriteCommit();	    /*save*/
    PPPOE_SRV_PRINTF("%s-%d: \n",__FUNCTION__,__LINE__);
    websRedirect(wp, "poeUsers.asp");
    return;
}
#endif
/********************************************************************
 * ������ getAddreGrpList
 * ���ܣ� ���������е�ַ����� 
 * ������ 2012-08-15
 * ������ ��
 * ���أ� ��
 * ����� �������еĵ�ַ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
#if (IP_GRP == FYES)
static int getAddreGrpList(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_IPGRP;
    IpGrpProfile *prof = NULL;

    int min = 0, max = 0;
    int i = 0;
    int num = 0;

    websWrite(wp,"var addrGroupName = new Array();");

    ProfInstLowHigh(mibType, &max, &min);
    /*�������е�ַ��*/
    for(i = min; i < max; i++) 
    {
        prof = (IpGrpProfile*)ProfGetInstPointByIndex(mibType, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0)) 
        {
            if(prof->grpType == IPGRP_IP)
            {/*�������ǵ�ַ�����*/
                websWrite(wp,"addrGroupName[%d] = \"%s\";", num, prof->head.name);
                num++;
            }
        }
    }
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
#endif
/********************************************************************
 * ������ aspPppoeMaxSess
 * ���ܣ� PPPoEϵͳ���Ự�����
 * ������ 2012-08-16
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int aspPppoeMaxSess(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;

    /*global profile inst*/
    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS, 0);
    websWrite(wp, "var smaxconvs=\"%d\";", profS->maxSessCnt);/*����û������õ�ϵͳ���Ự��*/

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

/**********************************************
 *������formPppoeHangUp
 *���ܣ��ڷ���˹Ҷ���ѡ�Ự
 *�����������ṹ��
 *����ֵ����
 *�������
 **********************************************/
static int formPppoeHangUp(webs_t wp, char_t *path, char_t *query)
{
    char *tmp,*hangstr;
    Uint32 id;
    const char *ErrMsg = NULL;
    int ret=0;
    a_assert(wp);
    hangstr = websGetVar(wp,T("hangstr"),T(""));

    if((strcmp(hangstr,T(""))==0)){
        ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_CANT_TUNNEL_ERR);
        setTheErrorMsg(ErrMsg);
        websRedirect(wp,"PPPoEConSta.asp");
        ret=0;
    }else{
        /*ͨ���ָ���ȡ���Ựid*/
        tmp = strtok(hangstr,UTT_SPLIT_SIGN_STR);
        while (tmp!=NULL) {
            id= (Uint32)strtoul(tmp, NULL, 10);
            /*�жϴ˻Ựid�Ƿ�Ϸ�*/ 
            if (SesIndexIsOk(id) == 1) {
                /*�жϽ��̺��Ƿ�Ϊ0*/
                uttSemP(SEM_PPPOE_NO,0);
                if ( (nvramProfile->Session[id].ConTime == 0u) && 
                        (nvramProfile->Session[id].pid != 0)) 
                {
                    /*ɱ���˻Ự��Ӧ����*/
                    doSystem("kill -INT %d", nvramProfile->Session[id].pid);
                }
                uttSemV(SEM_PPPOE_NO,0);
            }
            tmp=strtok(NULL,UTT_SPLIT_SIGN_STR);
        }
        websRedirect(wp,"PPPoEConSta.asp");

        ret=1;
    }
    return ret;
}

/*
 * fun for multi path
 * define asp and form fun
 */
void formDefinePppoeServer(void) {
    websAspDefine(T("aspPppoeClientStatus"), aspPppoeClientStatus);
    websAspDefine(T("aspPppoeServGlobale"), aspGetPppoeServGlobale);
    websAspDefine(T("aspPppoeOneUserInfo"), aspPppoeOneUserInfo);
    websAspDefine(T("aspGetPppoeUser"), aspGetPppoeUser);
#if (IP_GRP == FYES)
    websAspDefine(T("getUserGrpList"), getUserGrpList);
#endif
    websFormDefine(T("formPPPoEAccAllow"),formConfigPPPoEAccAllow);
    websFormDefine(T("forPppoeServGlobale"), formConfigPppoeServGlobale);
    websFormDefine(T("formPppoeUserDelAll"), formPppoeUserDelAll);
#if (PPPOE_LIMIT == FYES)
    websFormDefine(T("formConfigPppoeRateLimit"), formConfigPppoeRateLimit);
#endif
    websFormDefine(T("formPppoeUserDel"), formConfigPppoeUserDel);
    websFormDefine(T("formPppoeUserEdit"), formConfigPppoeUserEdit);
    websFormDefine(T("formPppoeImport"), formConfigPppoeImport);
    websFormDefine(T("formPppoeExport"), formConfigPppoeExport);
    websFormDefine(T("formPppoeHang"), formPppoeHangUp);
#if (PPPOE_SELF_HELP == FYES)
    websFormDefine(T("formPppoeSelfHelpConfig"), formPppoeSelfHelpConfig);
    websAspDefine(T("aspGetPppoeSelfHelpData"), aspGetPppoeSelfHelpData);
#endif
#if (IP_GRP == FYES)
    websAspDefine(T("aspOutAddreGrpList"), getAddreGrpList);
#endif
    websAspDefine(T("aspPppoeMaxSess"), aspPppoeMaxSess);
}

#else
static int aspGetPppoeUser(int eid, webs_t wp, int argc, char_t **argv)
{
    return 0;
}
void formDefinePppoeServer(void) {
    websAspDefine(T("aspGetPppoeUser"), aspGetPppoeUser);
}
#endif
