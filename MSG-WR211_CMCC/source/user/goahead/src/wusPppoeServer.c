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
 * 函数： pppoeAccGrpIsFull
 * 功能： 判断PPPOE账号组是否已满
 * 创建： 2013-09-23
 * 参数： 要判断的PPPOE账号组名
 * 返回： 1->组已满；2->组未满
 * 输出： 略
 * 作者： zhao.chong 
 ********************************************************************/ 
static int pppoeAccGrpIsFull(char_t *UserGroupName)
{
    IpGrpProfile *prof;
    int i = 0, ret = 0, flag = 0;

    prof = (IpGrpProfile *)ProfGetInstPointByName(MIB_PROF_IPGRP, UserGroupName);
    if((ProfInstIsFree(prof) == 0) && (prof->grpType == IPGRP_USER)) {
        for(i = 0; i < IPGRP_USER_ENTRYS; i++) {
            if(prof->ipEntry[i].entryType == IPENT_NONE) {/*组中只要有个条目没有使用，表示组未满*/
                flag = 1;/*falg==1表示组未满*/
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
 * 函数： UserGroupConfigAdd
 * 功能： 向用户组添加用户
 * 创建： 2012-07-30
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int UserGroupConfigAdd(char_t *UserGroupName, char_t *user)
{
    IpGrpProfile *prof;
    int i = 0, ret = 1, flag = 0;
    prof = (IpGrpProfile *)ProfGetInstPointByName(MIB_PROF_IPGRP, UserGroupName);
    const char *ErrMsg = NULL;

    /*账号组*/
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
 * 函数： getIpGrpList
 * 功能： 向页面输出用户组
 * 创建： 2012-05-10
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
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
 * 删除指定用户pppoe 拨号
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
 * 函数： killOnePppoeClient 
 * 功能： 删除指定用户pppoe 拨号
 * 创建： 2012-03-28
 * 参数： 无
 * 返回： 无
 * 输出： 略
 * 作者： Jianqing.Sun 
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
     *此处为加上信号量锁定此操作
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
    {/*在线*/
#if (PPPOE_ACCOUNT == FYES)
        /*这里应注意因为 删除操作导致 客户端下线的话
          在 pppd程序里面是找不到对应的profile的
          故需在此处理。
          编辑操作也有此可能。
          */
        if(info->chargeType !=  NONE_CHARG_MODE)
        {/*开启了计费*/
            upPppAccRule(info, info->expired, 0);/*删除原来的规则*/
        }
#endif
#ifdef CONFIG_UTT_NETFILTER
        uttNfNlHashDelIpStr(uttNfNlSock, info->ip);/*删除hash表对应的ip*/
#endif
        doSystem("kill -INT %d", info->pid);
    }
#endif

#if 0
    /*
     *此处为发信号解锁
     */
    uttSemV(SEM_PPPOE_NO,0);
#endif

    return;
}
#else
/********************************************************************
 * 函数： killOnePppoeClient
 * 功能： 循环遍历账号对应的所有会话，清除相关内容
 * 创建： 2012-08-24
 * 参数： 
 *	  info     -    账号对应profile
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
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
                {/*在线*/
#if (IP_GRP == FYES)
                    userUpToIpGrp(info->pppoename, nvramProfile->Session[j].ip, IPENT_POE_ACC, IPSET_MEM_DEL);
#endif
#if (PPPOE_ACCOUNT == FYES)
                    if(info->chargeType !=  NONE_CHARG_MODE)
                    {/*开启了计费*/
                        upPppAccRule(nvramProfile->Session[j], info->expired, 0);/*删除原来的规则*/
                    }
#endif
#if (PPPOE_LIMIT == FYES)
                    if ((info->uRateLimit != 0U) || (info->dRateLimit != 0U))
                    {
                        for (index=0U;index<MAX_PPPOE_LIMIT_IPGRP;index++)
                        {
                            if ((prof->limitGroup[index].uRateLimit == info->uRateLimit) && (prof->limitGroup[index].dRateLimit == info->dRateLimit))/*通过比较速率找到组名*/
                            {
                                ipsetAOrDIpStr(prof->limitGroup[index].ipGrpName, nvramProfile->Session[j].ip, IPSET_MEM_DEL);/*更新PPPoE限速相应的ipset组*/
                                ipsetAOrDIpStr(POE_SKIP_FIXED_LIMIT, nvramProfile->Session[j].ip, IPSET_MEM_DEL);/*更新跳过精细化限速ipset组*/
                            }
                        }
                    }
#endif
#ifdef CONFIG_UTT_NETFILTER
                    uttNfNlHashDelIpStr(uttNfNlSock, nvramProfile->Session[j].ip);/*删除hash表对应的ip*/
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
 * 删除所有pppoe 服务器账号拨号账号
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
 * 功能： 循环遍历账号对应的所有会话，清除相关 规则*
 * 创建： 2013-08-16
 * 参数： 
 *	  info     -    账号对应profile
 * 返回： 略
 * 输出： 略
 * 作者： bhou
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
            {/*在线*/
                upPppAccRule(nvramProfile->Session[j], info->expired, 0);/*删除原来的规则*/
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
 * 判断是否存在不在地址池内的固定IP分配地址
 * TRUE  - 存在
 * FALSE - 不存在
 * ipS ipEnd为主机字节序
 */
static int existNonPoolAssignIp(Uint32 ipS, Uint32 ipE) {
    MibProfileType mibTypeA = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *profA = NULL;
    int min = 0, max = 0, i = 0;
    Uint32 ip;

    ProfInstLowHigh(mibTypeA, &max, &min);

    /*
     *此处为加上信号量锁定此操作
     */
    uttSemP(SEM_PPPOE_NO,0);

    for (i = min; i < max; i++) {
        profA = (PPPoESrvAccProfile *)ProfGetInstPointByIndex(mibTypeA, i);
        if((profA != NULL) && (ProfInstIsFree(profA) == 0) && (profA->bindIp != 0)) {
            ip = ntohl(profA->bindIp);
            if ((ip < ipS) || (ip >= ipE)) {
                /*
                 * 此处为发信号解锁
                 */
                uttSemV(SEM_PPPOE_NO,0);
                return TRUE;
            }
        }
    }

    /*
     *此处为发信号解锁
     */
    uttSemV(SEM_PPPOE_NO,0);

    return FALSE;
}

/*
 * 全局配置页面保存
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
        setTheErrorMsg("无效地址池数");
        goto out;
    }
    if (ip_count > MAX_PPPOE_SRV_ACC_PROFILES || ip_count < cnt) {
        setTheErrorMsg("地址池数必须不大于帐号总数并不小于系统最大会话数");
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
     * mantis 8438:lan口ip不能包含在地址池内
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
     * 判断地址池
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
        /* 在profile change中调用 */
        killallPppoeClient();
#endif
    }

    profS->onlyPppoe = (Uint32)strtol(onlyPppoe,NULL,10);
    /*地址组名的长度不能大于IP_GROUP_NAME_LEN*/
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
 * 全局配置页面输出
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
        setTheErrorMsg(T("用户名输入重复!"));
    }else if(strnmatch(list, aspip, 2) != NULL){
        setTheErrorMsg(T("固定IP地址输入重复！"));
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
        setTheErrorMsg(T("找不到该用户名 !"));
    } else {
        q = p;
        while( (*q != UTT_SPLIT_SIGN_RECORD) && *q) {
            q++;
        }
        c=*p;
        *p = 0;
        if( strnmatch(list, user, 0) || strnmatch(q, user, 0)) {
            *p=c;
            setTheErrorMsg(T("用户名输入重复!"));
        }else if( strnmatch(list, aspip, 2) || strnmatch(q, user, 0)) {
            *p=c;
            setTheErrorMsg(T("固定IP地址输入重复!"));
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
 * 判断是否允许账号，IP绑定
 * 输入参数：
 *  user    -	页面提交的账号
 *  ip	    -	页面提交的绑定IP地址
 *
 * 返回值：
 *  1	-   允许
 *  0	-   不允许
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

            /* 找到IP地址对应的拨号信息 */
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
 * 函数： allowIpUserBind 
 * 功能： 判断是否允许账号，IP绑定
 * 创建： 2012-03-28
 * 参数： 
 *  user    -	页面提交的账号
 *  ip	    -	页面提交的绑定IP地址
 * 返回： 
 *  1	    -   允许
 *  0	    -   不允许
 * 输出： 略
 * 作者： Jianqing.Sun 
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
     *此处为加上信号量锁定此操作
     */
    uttSemP(SEM_PPPOE_NO,0);

    for(i = min;i < max;i++)
    {
        info = (PPPoESrvAccProfile*)ProfGetInstPointByIndex(mibType, i);
        if(info != NULL)
        {

            /* 找到IP地址对应的拨号信息 */
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
     *此处为发信号解锁
     */
    uttSemV(SEM_PPPOE_NO,0);

    return rst;
}
#else
/********************************************************************
 * 函数： allowIpUserBind 
 * 功能： 判断是否允许账号，IP绑定
 * 创建： 2012-08-24
 * 参数： 
 *  user    -	页面提交的账号
 *  ip	    -	页面提交的绑定IP地址
 * 返回： 
 *  1	    -   允许
 *  0	    -   不允许
 * 输出： 略
 * 作者： Jianqing.Sun 
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
     *此处为加上信号量锁定此操作
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
                /* 找到IP地址对应的拨号信息 */
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
     *此处为发信号解锁
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
 * 检查已配置限速组数量，判断是否能够添加
 */
static int checkPPPOEAccLimitNum(int uRate, int dRate, char *indexStr)
{
    MibProfileType mibTypeA = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *profA = NULL;
    int min = 0, max = 0;
    int i = 0, j = 0;
    int ret = 0;
    int index = 0;
    int uRates[MAX_PPPOE_LIMIT_IPGRP];/*上传速率数组*/
    int dRates[MAX_PPPOE_LIMIT_IPGRP];/*下载速率数组*/

    if ((indexStr == NULL) || (indexStr[0] == '\0'))
    {
        index = -1;/*用户名获取失败则将序号改为不可能的数字*/
    }
    else
    {
        index = ProfGetInstIndexByName(mibTypeA, indexStr);
    }

    /*速率为0不需限速*/
    if ((uRate != 0) || (dRate != 0))
    {
        memset(uRates, 0, sizeof(uRates));
        memset(dRates, 0, sizeof(dRates));
        uRates[0] = uRate;
        dRates[0] = dRate;
        ProfInstLowHigh(mibTypeA, &max, &min);

        /*通过将速率组合放入数组计算限速组数量*/
        for(i=min;i<max;i++) 
        {
            /*当前结构体不参与计算*/
            if (i != index)
            {
                profA = (PPPoESrvAccProfile*)ProfGetInstPointByIndex(mibTypeA, i);
                if(ProfInstIsFree(profA) == 0) 
                {
                    /*速率为0不产与计算*/
                    if ((profA->uRateLimit != 0) || (profA->dRateLimit != 0))
                    {
                        /*查找计算速率信息的数组*/
                        for (j=0;j<MAX_PPPOE_LIMIT_IPGRP;j++)
                        {
                            /*找到和要改变的速率相同的组，成功，退出*/
                            if ((uRates[j] == profA->uRateLimit) && (dRates[j] == profA->dRateLimit))
                            {
                                break;
                            }
                            /*找到空组，成功，退出*/
                            if ((uRates[j] == 0) && (dRates[j] == 0))
                            {
                                uRates[j] = profA->uRateLimit;
                                dRates[j] = profA->dRateLimit;
                                break;
                            }
                        }
                        /*既不相同，也没有空位置，失败*/
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
 * 用户账号配置页面
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
        setTheErrorMsg(T("固定IP地址不在PPPoE地址池内!"));
    }
    else{
        if(strncmp(passwd1, passwd2, MAX_UTT_PASSWD_LEN) == 0){
            tempLen=strlen(list)+strlen(aspip)+strlen(user)+strlen(passwd1)+4+16;
            if(!strncmp(action, "add", 3)) {
                /*
                 * add a new pppoe user
                 */
                if(_pppoeUserCount >= UTT_MAX_PPPOE_USER) {
                    setTheErrorMsg(T("已达最大条目！"));
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
                    setTheErrorMsg(T("找不到该用户名 !"));
                } else {
                    _modifyPppoeUser(user, passwd1, aspip, list, userOld);
                }
#else
                _modifyPppoeUser(user, passwd1, aspip, list, userOld, tempLen);
#endif
            }
        } else {
            setTheErrorMsg(T("两次密码不相同"));
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
    /* * 获得页面参数 */
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
    uRateLimitStr = websGetVar(wp,T("uRate"), T("0"));/*上传速率*/
    dRateLimitStr = websGetVar(wp,T("dRate"), T("0"));/*下载速率*/
#endif
#if 0
    /*
     * 判断密码
     */
    if (strncmp(passwd1, passwd2, UTT_PASSWD_LEN) != 0) {
        setTheErrorMsg(T("前后两次输入密码不等"));
        goto out;
    }
#endif

#if 0
    printWeb();
#endif
    /*
     * 固定IP地址
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
    //固定ip地址池分配修改为与系统最大会话数无关
    if ((h_bindIp != 0) && (h_bindIp < h_ipStart || h_bindIp >= (h_ipStart + profS->ipcount))) { 
#else
    if ((h_bindIp != 0) && (h_bindIp < h_ipStart || h_bindIp >= (h_ipStart + profS->maxSessCnt))){
#endif
        ErrMsg = getTransDataForC(C_LANG_INDEX_STATIC_IP_ADDR_CANT_IN_PPPoE_POOL);
        setTheErrorMsg(ErrMsg);
        goto out;
    }

    /*
     * 不允许这对账号-IP绑定
     */
    if (allowIpUserBind(user, userIp) == 0) {
        ErrMsg = getTransDataForC(C_LANG_INDEX_BANNED_BOUND_ALREADY_ASSIGN_IP);
        setTheErrorMsg(ErrMsg);
        goto out;
    }

#if (PPPOE_LIMIT == FYES)
    /*是否达到pppoe限速组最大值*/
    if (checkPPPOEAccLimitNum(strtoul(uRateLimitStr, NULL, 0), strtoul(dRateLimitStr, NULL, 0), s_instIndex) == 1)
    {
        ErrMsg = getTransDataForC(C_LANG_INDEX_PPPOE_LIMIT_CANT_MORE);
        setTheErrorMsg(ErrMsg);
        goto out;
    }
#endif

    if (strcmp(action, "add") == 0) {
        /*
         * 用户名是否重复
         */
        if (user != NULL && (ProfGetInstPointByValue(mibTypeA, OFFANDSIZE(PPPoESrvAccProfile, pppoename), TSTR, user, -1) != PROFFAIL)) {
            ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_ALREADY_EXITS);
            setTheErrorMsg(ErrMsg);
            goto out;
        }
        /*
         * 判断固定IP地址是否重复
         */
        if ((bindIp.s_addr != 0) && (ProfGetInstPointByValue(mibTypeA, OFFANDSIZE(PPPoESrvAccProfile, bindIp), TIPADDR, userIp, -1) != PROFFAIL)) {
            ErrMsg = getTransDataForC(C_LANG_INDEX_STATIC_IP_ADDR_ALREADY_EXITS);
            setTheErrorMsg(ErrMsg);
            goto out;
        }
        webIndexInst = getWebInstName(&(nvramProfile->pppoeInstRec.instRecMap), &(nvramProfile->pppoeInstRec.instNameIndex));
        /* portmap已满 */
        if (webIndexInst == 0) {
            ErrMsg = getTransDataForC(C_LANG_INDEX_OPERATION_FAIL);
            setTheErrorMsg(ErrMsg);
            goto out;
        }
        snprintf(instId, UTT_INST_NAME_LEN, "ID%d", webIndexInst);
        /* 已存在实例名 */
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
         * 用户名判断
         */
        if (strcmp(user, userOld) != 0) { /* 用户名修改了 */
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
         * 判断固定IP地址
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
     *此处为加上信号量锁定此操作
     */
    uttSemP(SEM_PPPOE_NO,0);
    ProfBackupByIndex(mibTypeA, profAction, instIndex, &profList);

    profA = (PPPoESrvAccProfile *)ProfGetInstPointByIndex(mibTypeA, instIndex);
    memcpy(&profBak, profA, sizeof(profBak));/*先备份原来的*/

    /* 索引为实例名  */
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
        /*mantis:11317,不挂断连接情况下，也不能清除会话数，如果只修改时间不挂断，但修改后的时间又是过期的，需要会话数来判断过期，所以不能清空*/
        profA->sesIndex = -1;
        profA->actualSession = 0;
    }

#if (PPPOE_ACCOUNT == FYES)
    if(enable != NULL)
    {
        SWORD_PRINTF("%s - %d, chargeType = date\n", __FUNCTION__, __LINE__);
        /*mantis:11317,由关闭计费模式状态（但实际设置日期是过期的，只是没开计费模式，不生效）
          修改为打开计费模式时，需要将状态修改为正常，在判断过期状态时才能将ip再次加入过期组*/
        if (profA->chargeType == NONE_CHARG_MODE)
        {
            profA->expired = PPPOE_ACC_NORMAL;
        }
        profA->chargeType = DATE_CHARG_MODE;
    }
    else
    {
        SWORD_PRINTF("%s - %d, chargeType = none\n", __FUNCTION__, __LINE__);
        /*mantis:11317,由日期过期状态修改为关闭计费模式状态，需要删除pppoe过期组中的ip，否则在不挂断情况下无法上网*/
        if (profA->chargeType == DATE_CHARG_MODE)
        {
            delAllSesRule(profA);/*删除原有规则*/
        }
        profA->chargeType = NONE_CHARG_MODE;
    }

    /*mantis11317,修改时间不挂断连接*/
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
     *此处为发信号解锁
     */
    uttSemV(SEM_PPPOE_NO,0);

    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
out:
#if 0
    if((profBak.ConTime == 0) &&
            (strcmp(profBak.ip, wp->ipaddr) == 0))
    {/*bug 0009804:修改自己导致tcp异常断开*/
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
 * 函数： editShelfSleep
 * 功能： 判断修改的账号是否是自己正在使用的
 * 创建： 2012-08-21
 * 参数： index  -    用户所拨账号的索引
 *         wp    -    页面结构体
 * 返回：  1     -    所拨账号是自己正在使用的
 *         0     -    所拨账号不是自己正在使用的
 * 输出： 略
 * 作者： Jianqing.Sun 
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
            {/*bug 0009804:修改自己导致tcp异常断开*/
                flag = 1;
            }
        }
    }
    return flag;
}

#if (PPPOE_LIMIT == FYES)
/*
 * 对批量限速操作中限速组数量进行检查
 */
static int pppoeAccLimitBatchCheck(PPPoESrvAccProfile* indexArray[], int uRate, int dRate)
{
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;
    int isEmpty = 0;/*有空位置*/
    int i = 0, j = 0;
    PppoeAccLimit limitGroupTmp[MAX_PPPOE_LIMIT_IPGRP];

    int retVal = 1;/*默认失败*/

    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS, 0);
    if (ProfInstIsFree(profS) == 0)
    {
        /*将保存的限速信息数组拷贝一份用来计算*/
        memcpy(limitGroupTmp, profS->limitGroup, sizeof(profS->limitGroup));
        /*循环查找空位置*/
        for (j=0;j<MAX_PPPOE_LIMIT_IPGRP;j++)
        {
            /*如果原来有空的或者要修改的和原来的相同*/
            if ((limitGroupTmp[j].count == 0) || ((limitGroupTmp[j].uRateLimit == uRate) && (limitGroupTmp[j].dRateLimit == dRate)))
            {
                isEmpty = 1;/*查找到可使用的位置*/
                break;
            }
        }
        if (isEmpty == 1)
        {
            return 0;/*返回成功*/
        }
        else/*全都被使用*/
        {
            /*找到NULL结束*/
            for (i=0;indexArray[i] != NULL;i++)
            {
                for (j=0;j<MAX_PPPOE_LIMIT_IPGRP;j++)
                {
                    /*找到记录对应速率信息的组*/
                    if ((indexArray[i]->uRateLimit == limitGroupTmp[j].uRateLimit) && (indexArray[i]->dRateLimit == limitGroupTmp[j].dRateLimit))
                    {
                        limitGroupTmp[j].count--;/*引用计数减1*/
                        if (limitGroupTmp[j].count <= 0)/*如果没有被引用，则成功*/
                        {
                            return 0;/*成功*/
                        } 
                    }
                }
            }
        }
    }
    return retVal;
}

/*
 * 批量限速操作
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

    uRateLimitStr = websGetVar(wp,T("MaxUpRatetxt"), NULL);/*上传速率*/
    dRateLimitStr = websGetVar(wp,T("MaxDownRatetxt"), NULL);/*下载速率*/
    UserName = websGetVar(wp, T("delstr"), NULL); 
    uRate = strtoul(uRateLimitStr, NULL, 0);
    dRate = strtoul(dRateLimitStr, NULL, 0);

    if (UserName == NULL) {
        ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_USER_NAME_NOG);
        setTheErrorMsg(ErrMsg);
        goto out;
    }

    tmp = strtok(UserName, UTT_SPLIT_SIGN_STR);
    while(tmp != NULL)/*将所有选中结构体指针保存到数组*/
    {
        profA = ProfGetInstPointByName(mibTypeA, tmp);
        if (ProfInstIsFree(profA) == 0)
        {
            indexArray[i] = profA;
            i++;
        }
        tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
    }
    indexArray[i] = NULL;/*NULL作为结束标志*/

    /*检查规则是否达到最大数量*/
    if (pppoeAccLimitBatchCheck(indexArray, uRate, dRate))
    {
        ErrMsg = getTransDataForC(C_LANG_INDEX_PPPOE_LIMIT_CANT_MORE);
        setTheErrorMsg(ErrMsg);
        goto out;
    }

    /*
     *此处为加上信号量锁定此操作
     */
    uttSemP(SEM_PPPOE_NO,0);

    for (i=0;indexArray[i]!=NULL;i++)
    {
        if (i == 0)/*只通知一次*/
        {
            ProfBackupByName(mibTypeA, PROF_CHANGE_ADD_ALL, indexArray[i]->head.name, &profList);/*通知speedweb*/
        }
        indexArray[i]->uRateLimit = uRate;
        indexArray[i]->dRateLimit = dRate;
    }

    /*
     *此处为发信号解锁
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
 * 列表删除函数
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
     *此处为加上信号量锁定此操作
     */
    uttSemP(SEM_PPPOE_NO,0);

    while(tmp != NULL) {

        profA =  (PPPoESrvAccProfile*)ProfGetInstPointByName(mibTypeA, tmp);
        if(ProfInstIsFree(profA) == 0)
        {/*使用了*/
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

                memcpy(&profBak, profA, sizeof(profBak));/*先备份原来的*/
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
                {/*bug 0009804:修改自己导致tcp异常断开*/

                    spFlag = 1;
                }
#else
                Index = ProfGetInstIndexByName(mibTypeA, tmp);
                spFlag = editShelfSleep(Index, wp);
#endif

                /* 清除记录map */
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
     *此处为发信号解锁
     */
    uttSemV(SEM_PPPOE_NO,0);

    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
#if 0
    if(!UserName) {   
        setTheErrorMsg(T("找不到该用户名!"));
    } else {
        tmp=strtok(UserName,UTT_SPLIT_SIGN_STR);
        while(tmp != NULL) {
            p1=strnmatch(CList, tmp, 0);
            if(p1 == 0) {
                setTheErrorMsg(T("找不到该用户名!"));
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
        sleep(3u);/*等待被杀死*/
    }
    websRedirect(wp, "PPPoEServer.asp");
}

/*
 * 列表全部删除
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
     *此处为加上信号量锁定此操作
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
                memcpy(&profBak, profA, sizeof(profBak));/*先备份原来的*/
                if(back == 0) {
                    ProfBackupByIndex(mibTypeA, PROF_CHANGE_DELALL, i, &profList);
                    back = 1;
                }
                ProfDelInstByIndex(mibTypeA, i);
#if 0
                if (strcmp((char *)getLanIfName(), profBak.device) == 0) 
                {/*先删账号。在杀死进程。防止杀死后。用户重新拨号成功*/
#endif
                    killOnePppoeClient(&profBak);
                    profA->sesIndex = -1;
                    profA->actualSession = 0;
                    SWORD_PRINTF("%s - %d, profA->pppoename = %s,profA->actualSession = %d, profA->accmaxconv= %d\n",__FUNCTION__, __LINE__, profA->pppoename, profA->actualSession, profA->accmaxconv);
#if 0
                    if((profBak.ConTime == 0) &&
                            (strcmp(profBak.ip, wp->ipaddr) == 0))
                    {/*bug 0009804:修改自己导致tcp异常断开*/

                        spFlag = 1;
                    }
#else
                    spFlag = editShelfSleep(i, wp);
#endif
                    /* 清除记录map */
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
     *此处为发信号解锁
     */
    uttSemV(SEM_PPPOE_NO,0);

#if 0
    /* 更新map记录 */
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
        sleep(5u);/*等待被杀死*/
    }
    websRedirect(wp, "PPPoEServer.asp");
}



/*
 * 账号配置页面列表输出函数
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
    websWrite(wp, "var smaxconvs=\"%d\";", profG->maxSessCnt);/*输出用户所设置的系统最大会话数*/
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
     *此处为加上信号量锁定此操作
     */
    uttSemP(SEM_PPPOE_NO,0);

    for (i = min; i < max; i++) {
        profA = (PPPoESrvAccProfile *)ProfGetInstPointByIndex(mibTypeA, i);
        if((profA != NULL) && (ProfInstIsFree(profA) == 0)) {
            websWrite(wp, names, totalrecs, profA->pppoename);
            websWrite(wp, instIndexs , totalrecs, profA->head.name);
#if (PPPOE_LIMIT == FYES)
            websWrite(wp, uRateFmt, totalrecs, profA->uRateLimit);/*上传速率*/
            websWrite(wp, dRateFmt, totalrecs, profA->dRateLimit);/*下载速率*/
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
     *此处为发信号解锁
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
 * 用户账号配置页面输出一条配置信息
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
        setTheErrorMsg(T("读取edit_Name变量失败!"));
    } else {
        if(!edit_name) {
            setTheErrorMsg(T("用户名不能为空!"));
        } else {
            CList = nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_USER);
            tmp1=strnmatch(CList,edit_name, 0);
            if(!tmp1) {
                setTheErrorMsg(T("找不到该用户!"));
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
    websWrite(wp, "var uRate=\"\";");/*上传速率*/
    websWrite(wp, "var dRate=\"\";");/*下载速率*/
#endif

    if ((ejArgs(argc, argv, T("%s"), &editName) < 1) || (editName == NULL)) {
        ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_READ_DATA_ERR);
        setTheErrorMsg(ErrMsg);
    } else {
        ProfInstLowHigh(mibTypeA, &max, &min);

        /*
         *此处为加上信号量锁定此操作
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
                websWrite(wp, "uRate=\"%d\";", profA->uRateLimit);/*上传速率*/
                websWrite(wp, "dRate=\"%d\";", profA->dRateLimit);/*下载速率*/
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
         *此处为发信号解锁
         */
        uttSemV(SEM_PPPOE_NO,0);

    }

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

#if 0
/*
 * pppoe server统计页面
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
    /* 读取开机时间，代替系统时间 */
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
        /* mantis 8923: 读取文件直到写文件都要将文件锁住 */
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
 * 函数： aspPppoeClientStatus 
 * 功能： pppoe server统计页面
 * 创建： 2012-03-28
 * 参数： 无
 * 返回： 无
 * 输出： 略
 * 作者： Jianqing.Sun 
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

    /* 读取开机时间，代替系统时间 */
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
    /* mantis 8923: 读取文件直到写文件都要将文件锁住 */
    /*
     * 此处为加上信号量锁定此操作
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
     *此处为发信号解锁
     */
    /*fcntl(fd, F_SETLKW, file_lock(F_UNLCK, SEEK_SET));*/

    websWrite(wp,"var totalrecs = %d;", totalrecs);
    return 0;
}
#else
/********************************************************************
 * 函数： aspPppoeClientStatus 
 * 功能： pppoe server统计页面
 * 创建： 2012-08-24
 * 参数： 无
 * 返回： 无
 * 输出： 略
 * 作者： Jianqing.Sun 
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

    /* 读取开机时间，代替系统时间 */
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
     * 此处为加上信号量锁定此操作
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
     *此处为发信号解锁
     */
    uttSemV(SEM_PPPOE_NO,0);


    websWrite(wp,"var totalrecs = %d;", totalrecs);
    return 0;
}
#endif
/********************************************************************
 * 函数： formPPPoEAccAllow
 * 功能： 列表中的允许
 * 参数： 无
 * 输出： 无
 * 返回： 无
 * 作者： yqyang    
 * 创建： 2011-11-17
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

    uttSemP(SEM_PPPOE_NO,0);/*加锁*/

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
    if(0 == Allow)/*禁止*/
    {
        killOnePppoeClient(profA);	/*杀掉客户端进程*/
        profA->sesIndex = -1;		/*会话ID置为-1初始值*/
        profA->actualSession = 0;	/*总的会话数置为0*/
    }
    profA->head.active = Allow;/*允许或禁止*/

    uttSemV(SEM_PPPOE_NO,0);/*解锁*/

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
    websRedirect(wp, "PPPoEServer.asp");/*页面跳转*/
}

/***********************************************************************
 *   函 数 名：formConfigPppoeImport
 *   功能描述：批量生成实例
 *   输入参数：从页面读取数据
 *   输出参数：无
 *   创建日期：12-05-09
 *   修改日期：
 *   作者：  zkh 
 *   附加说明：无
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
    for(j = 0;j<len;j++)               // * 将脚本中得到的数据拷贝到数组中 */
    {
        user[j] = data[j];
    }
    pUser[0] = strtok(user,";");
    if(!pUser[0])				// * 实例信息为空 */
    {
        //SetWebLastMsg(T("导入信息不能为空！"));
        websRedirect(wp,T("PPPoEImport.asp"));
        return ;
    }

    for(k = 1;k<MAX_PPPOE_SRV_ACC_PROFILES;k++)                        // * 从data中逐个读取出实例变量 */
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
        username = strtok(pUser[i], " ");	// * 得到用户名 */
        passwd = strtok(NULL, " ");		// * 得到密码 */ 
        AccMACBinds = strtok(NULL, " ");//绑定方式
        bindmac = strtok(NULL, " "); //得到绑定mac
        bindmac2 = strtok(NULL, " ");
        bindmac3 = strtok(NULL, " ");
        bindmac4 = strtok(NULL, " ");
        accmaxconv = strtok(NULL, " ");//最大会话数
        bindIp = strtok(NULL, " ");
#if (PPPOE_ACCOUNT == FYES)
        chargeType = strtok(NULL, " ");//计费模式
        tmStart = strtok(NULL, " ");//账号开通日期
        tmStop = strtok(NULL, " ");//截至日期
#endif
#if (PPPOE_LIMIT == FYES)
        uRateLimit = strtok(NULL, " ");//上传限速
        dRateLimit = strtok(NULL, " ");//下载
        Uint16 txRate;
        Uint16 rxRate;
#endif
        remark = strtok(NULL, " ");		// * 得到描述信息 */
        if ((username == NULL) || (passwd == NULL) || (AccMACBinds == NULL) || (bindIp == NULL) || (accmaxconv == NULL)
                || (bindmac == NULL) || (bindmac2 == NULL) || (bindmac3 == NULL) || (bindmac4 == NULL) || (remark == NULL)
#if (PPPOE_ACCOUNT == FYES)
                || (chargeType == NULL) || (tmStop == NULL) || (tmStart == NULL)
#endif
#if (PPPOE_LIMIT == FYES)
                ||(uRateLimit == NULL) || (dRateLimit == NULL)
#endif
                ){
            setTheErrorMsg("导入信息有误");
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

        /* * 用户名是否重复 */
        if (username != NULL && (ProfGetInstPointByValue(mibTypeA, OFFANDSIZE(PPPoESrvAccProfile, pppoename), TSTR, username, -1) != PROFFAIL)) {
            ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_ALREADY_EXITS);
            setTheErrorMsg(ErrMsg);
            goto out;
        }

        webIndexInst = getWebInstName(&(nvramProfile->pppoeInstRec.instRecMap), &(nvramProfile->pppoeInstRec.instNameIndex));
        /* portmap已满 */
        if (webIndexInst == 0) {
            ErrMsg = getTransDataForC(C_LANG_INDEX_OPERATION_FAIL);
            setTheErrorMsg(ErrMsg);
            goto out;
        }
        snprintf(instId, UTT_INST_NAME_LEN, "ID%d", webIndexInst);
        /* 已存在实例名 */
        if (ProfGetInstIndexByName(mibTypeA, instId) != PROFFAIL) {
            ErrMsg = getTransDataForC(C_LANG_INDEX_CASE_ALREADY_EXITS);
            setTheErrorMsg(ErrMsg);
            goto out;
        }

       //判断固定ip合法性 
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
         *此处为加上信号量锁定此操作
         */
        uttSemP(SEM_PPPOE_NO,0);
        ProfBackupByIndex(mibTypeA, profAction, instIndex, &profList);
        profA = (PPPoESrvAccProfile *)ProfGetInstPointByIndex(mibTypeA, instIndex);
        memcpy(&profBak, profA, sizeof(profBak));/*先备份原来的*/
 
        /* 索引为实例名  */
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
         *此处为发信号解锁
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
 * 给PPPoE没有配置的某项设置为特定的符号
 * 以便导入时便以区分！
 * ************************************/
void checkToSetUnconfigured(void *toString, int opt)
{
    int flag = 0;
    switch(opt)
    {
        case 1:
            if (strncmp((unsigned char *)toString, "000000000000", 12) == 0) //绑定mac检测
                flag = 1; 
            break;
        case 2:
            if (strncmp(toString, "0.0.0.0", 7) == 0)//固定ip检测
                flag = 1; 
            break;
        case 3:
            if (strncmp(toString, "0", 1) == 0) //开关检测, 速率设置检测
                flag = 1;
            break;
        case 4:
            if (strncmp(toString, "1970-01-01", 8) == 0) //检测日期 
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
 *将PPPoE帐号信息转换成字符串
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
                checkToSetUnconfigured(toString, 3);//上传下载速率
                break;
            }
        case 3: 
            {
                unsigned int *ip = (unsigned int *)src;
                struct in_addr ipAddr;
                ipAddr.s_addr = *ip;
                sprintf(toString, "%s", inet_ntoa(ipAddr)); 
                checkToSetUnconfigured(toString, 2);//检查是否为0.0.0.0
                break;
            }
        case 4:
            {
                unsigned int *chargeType = (unsigned int *)src;
                sprintf(toString, "%d", *(chargeType) == NONE_CHARG_MODE ? NONE_CHARG_MODE : DATE_CHARG_MODE);
                checkToSetUnconfigured(toString, 3);//检查是否为0
                break;
            }
        case 5:
            {
                unsigned int *time = (unsigned int *)src;
                struct tm *date = localtime((time_t *)time);
                sprintf(toString, "%d-%02d-%02d", 1900+date->tm_year, 1+date->tm_mon, date->tm_mday);
                checkToSetUnconfigured(toString, 4);//检查是否是1970-01-01
                break;
            }
        case 6:
            {
                if (strlen((char *)src) == 0) //remark未填写
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
    /* 此处为加上信号量锁定此操作 */
    uttSemP(SEM_PPPOE_NO,0);
    char toString[MAX_PPPOE_ACCOUNT_INFO_LEN];
    for (i = min; i < max; i++)
    {
        profA = (PPPoESrvAccProfile *)ProfGetInstPointByIndex(mibTypeA, i);
        if((profA != NULL) && (ProfInstIsFree(profA) == 0)) {
            if( Maxlen > ( strlen(buf) + strlen(profA->pppoename) ) ) {
                strcat(buf,profA->pppoename);//用户名
            }
            if( Maxlen > ( strlen(buf) + strlen(" ") ) ) { 
                strcat(buf," ");
            }
            if( Maxlen > ( strlen(buf) + strlen(profA->passwd) ) ) {
                strcat(buf,profA->passwd);//密码
            }
            if( Maxlen > ( strlen(buf) + strlen(" ") ) ) {
                strcat(buf," ");
            }
            if (Maxlen > (strlen(buf) + strlen(profA->AccMACBind))) //绑定方式 
            {
               strcat(buf, profA->AccMACBind); // handbind, autobind, nobind
            }
            if( Maxlen > ( strlen(buf) + strlen(" ") ) ) {
                strcat(buf," ");
            }
            if(Maxlen > (strlen(buf) + strlen("0022aa112233"))) {  //一个mac串的长度
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
            if(Maxlen > (strlen(buf) + strlen("100"))) {//最大会话数,MAX_PPPOE_SRV_SESSION_PROFILES 
                convertToString(&(profA->accmaxconv), toString, 2);
                strcat(buf, toString);
            }
            if(Maxlen > (strlen(buf) + strlen(" "))){
                strcat(buf," ");
            }           
            if(Maxlen > (strlen(buf) + strlen("255.255.255.255"))) { //固定IP地址,取最大的ip长度
                convertToString(&(profA->bindIp), toString, 3);
                strcat(buf, toString);
            }
            if(Maxlen > (strlen(buf) + strlen(" "))){
                strcat(buf," ");
            } 
#if (PPPOE_ACCOUNT == FYES)
            if(Maxlen > (strlen(buf) + strlen("0"))) { //计费模式 0,1表不同模式
                convertToString(&(profA->chargeType), toString, 4);
                strcat(buf, toString);
            }
            if(Maxlen > (strlen(buf) + strlen(""))){
                strcat(buf," ");
            }
            if(Maxlen > (strlen(buf) + strlen("20150101"))) { //开通日期
                convertToString(&(profA->tmStart), toString, 5); 
                strcat(buf, toString);
            }
            if(Maxlen > (strlen(buf) + strlen(" "))){
                strcat(buf," ");
            }
            if(Maxlen > (strlen(buf) + strlen("20151231"))) {
                convertToString(&(profA->tmStop), toString, 5);
                strcat(buf, toString);//停用日期
            }
            if(Maxlen > (strlen(buf) + strlen(" "))){
                strcat(buf," ");
            }
#endif
#if (PPPOE_LIMIT == FYES)
            if(Maxlen > (strlen(buf) + strlen("102400"))) {//上传
                convertToString(&(profA->uRateLimit), toString, 2);
                strcat(buf, toString);
            }
            if(Maxlen > (strlen(buf) + strlen(" "))){
                strcat(buf," ");
            }
            if(Maxlen > (strlen(buf) + strlen("102400"))) {//下载
                convertToString(&(profA->dRateLimit), toString, 2);
                strcat(buf, toString);
            }
            if(Maxlen > (strlen(buf) + strlen(" "))){
                strcat(buf," ");
            }
#endif
            if( Maxlen > ( strlen(buf) + strlen(profA->remark))) {//备注
                convertToString(profA->remark, toString, 6);
                strcat(buf, toString);
            }
            if( Maxlen > ( strlen(buf) + strlen("\r\n") ) ) {
                strcat(buf,"\r\n");
            }
            totalrecs++;
        }
    }

    /* *此处为发信号解锁 */
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
 * 判断用户的ip是否在PPPoE地址池内
 * input:   
 *	ip: 用户IP（主机字节序）
 * output: 
 *	1:  在PPPoE地址池内
 *	0:  不在PPPoE地址池内
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
 * 根据用户IP查找PPPoE账户名
 * input:   
 *	ip:	用户IP（主机字节序）
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
 * 根据用户IP查找PPPoE账户名
 * input:   
 *	ip:	用户IP（主机字节序）
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
    /*获取用户ip（网络字节序）*/
    userip = inet_addr((const char *)wp->ipaddr);
    PPPOE_SRV_PRINTF("%s-%d: get userip=%x, ntohl(userip)=%x\n",__FUNCTION__,__LINE__,userip,ntohl(userip));
    /*判断用户ip是否在PPPoE地址池内*/
    if(isIpInPool(ntohl(userip)) == 1)
    {
        /*判断用户IP是否在使用pppoe账号拨号上网*/
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
 * 判断此ip是否正用此用户名拨号上网
 * input:
 *	ip:	需判断的ip，主机字节序
 *	name:	需判断的用户名
 * output:
 *	-1:	此ip为使用此用户名
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
 * 判断此ip是否正用此用户名拨号上网
 * input:
 *	ip:	需判断的ip，主机字节序
 *	name:	需判断的用户名
 * output:
 *	-1:	此ip为使用此用户名
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
 * 检测页面提交参数的合法性
 * input：
 *	    ip:	    用户IP，主机字节序
 *	  name:	    pppoename
 *	oldpwd:     旧密码
 *	newpwd:	    新密码
 *   verifypwd:     确认密码
 * ouiput:
 *	     1:	    合法
 *	     0:	    不合法
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
    /*功能开关是否开启*/
    if((profS->head.active != 1) || (profS->self_help !=  FUN_ENABLE))
    {
        PPPOE_SRV_PRINTF("%s-%d: \n",__FUNCTION__,__LINE__);
        ErrMsg = getTransDataForC(C_LANG_INDEX_NO_JURISDICTION_FOR_O_PERATION);
        setSelfHelpMsg((char *)ErrMsg);
        ret=0;
    }
    else
    {
        /*判断输入参数的长度检查*/
        if((strlen(name)==0)||(strlen(oldpwd)==0)||(strlen(newpwd)==0)||(strlen(name)>31)||(strlen(oldpwd)>31)||(strlen(newpwd)>31))
        {
            PPPOE_SRV_PRINTF("%s-%d: \n",__FUNCTION__,__LINE__);
            ErrMsg = getTransDataForC(C_LANG_INDEX_SUBMIT_WRONG_PARMETERS_RECIN);
            setSelfHelpMsg((char *)ErrMsg);
            ret=0;
        }
        else
        {
            /*判断新密码与确认密码是否相同*/
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
                    setSelfHelpMsg(T("您没有权限进行此操作！"));
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
    /*转换为网路字节序的ip*/
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
 * 函数： getAddreGrpList
 * 功能： 将所有组中地址组输出 
 * 创建： 2012-08-15
 * 参数： 无
 * 返回： 略
 * 输出： 多有组中的地址组
 * 作者： Jianqing.Sun 
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
    /*遍历所有地址组*/
    for(i = min; i < max; i++) 
    {
        prof = (IpGrpProfile*)ProfGetInstPointByIndex(mibType, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0)) 
        {
            if(prof->grpType == IPGRP_IP)
            {/*组类型是地址组的组*/
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
 * 函数： aspPppoeMaxSess
 * 功能： PPPoE系统最大会话数输出
 * 创建： 2012-08-16
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int aspPppoeMaxSess(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;

    /*global profile inst*/
    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS, 0);
    websWrite(wp, "var smaxconvs=\"%d\";", profS->maxSessCnt);/*输出用户所设置的系统最大会话数*/

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

/**********************************************
 *函数：formPppoeHangUp
 *功能：在服务端挂断所选会话
 *参数：交互结构体
 *返回值：无
 *输出：略
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
        /*通过分隔号取出会话id*/
        tmp = strtok(hangstr,UTT_SPLIT_SIGN_STR);
        while (tmp!=NULL) {
            id= (Uint32)strtoul(tmp, NULL, 10);
            /*判断此会话id是否合法*/ 
            if (SesIndexIsOk(id) == 1) {
                /*判断进程号是否为0*/
                uttSemP(SEM_PPPOE_NO,0);
                if ( (nvramProfile->Session[id].ConTime == 0u) && 
                        (nvramProfile->Session[id].pid != 0)) 
                {
                    /*杀掉此会话对应进程*/
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
