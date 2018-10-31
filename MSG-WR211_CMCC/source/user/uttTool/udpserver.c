#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "mib.h"
#include "profacce.h"
#include "argcmd.h"
#include <sys/sysinfo.h>

#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
#include <linux/netfilter_ipv4/utt_netfilter.h>

#define SWORD_PRINTF(fmt, args...)        \
        if(flag == 1)                     \
               printf(fmt, ## args)
        
#if 0
#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif
#endif

#define UTT_SPLIT_SIGN_STR      ","
#define UTT_USER_LEN 31
#define UTT_ONE_DAY             (24*60*60)

int uttNfNlSock;
int flag = 0;
typedef enum utt_em_action{
    UTT_ACTION_UNKNOWN,
    UTT_ACTION_ADD,
    UTT_ACTION_DELETE,
    UTT_ACTION_MODIFY,
    UTT_ACTION_UPDATE,
    UTT_ACTION_REPEAT,
    UTT_ACTION_FULL,
    UTT_ACTION_FIBDDEN,
};
typedef enum utt_em_open_net{
    UTT_OPEN_STOP,
    UTT_OPEN_START,
};

struct UTT_WEB_AUTH
{
    unsigned   short   action;
    unsigned   short   retaction;
    unsigned   short   origin; 
    unsigned   short   opennet; 
    unsigned   short   opentime;
    unsigned   short   timetotal;
    unsigned   short   maxsession;
    char       user[UTT_USER_LEN + 1];
    char       psw[UTT_USER_LEN + 1];
    char       remark[UTT_USER_LEN + 1];
    char       idcode[UTT_USER_LEN + 1];
};

struct UTT_WEB_HEAD
{
    unsigned short start ;
    unsigned short version;
    unsigned short checksum;
    unsigned short index;
    char     data[0];
};

#if 0
SWORD_PRINTF("%s-%d\n", __func__, __LINE__);
#endif
#define MAX_LEN 1024
#define MY_OFFSET(m,y) (&((m*)0)->y)

static struct UTT_WEB_AUTH* WebAuthConfigModify(struct UTT_WEB_AUTH * utt_web);
static struct UTT_WEB_AUTH* WebAuthConfigAdd(struct UTT_WEB_AUTH * utt_web);
static struct UTT_WEB_AUTH* WebAuthListDel(struct UTT_WEB_AUTH * utt_web);

static Uint32 getClock()
{

    char buf[64], *tmp;
    struct in_addr inaddr;
    Uint32 time;
    FILE *fp = popen("date '+%s'", "r");
    if (!fp) {
        return 0;
    }
    fgets(buf, 64, fp);
    pclose(fp);
    tmp = strtok(buf, "\n");
    time = strtoul(tmp, NULL, 10);
    return time;
}

extern IpGrpProfile* userRefByIpGrp(char *username, ipGrpEntryType userType)
{
    MibProfileType mibType = MIB_PROF_IPGRP;
    IpGrpProfile* retProf = NULL;
    IpGrpProfile* prof = NULL;

    int i = 0, min = 0, max = 0;
    int searched[MAX_IPGRP_PROFILES];/*辅助空间。保证每个组只被查找一次。提高效率**/
    memset(searched, 0,sizeof(searched));/*初始化为 每个组都未查找过*/

    SWORD_PRINTF("%s------%d, username = %s\n", __func__, __LINE__, username);
    ProfInstLowHigh(mibType, &max, &min);
    /*遍历整个profile*/
    for(i = min; i < max; i++) 
    {
        prof = (IpGrpProfile*)ProfGetInstPointByIndex(mibType, i);
        if((ProfInstIsFree(prof) == 0) && (prof->grpType == IPGRP_USER))/*只查找用户性组**/
        {
            if(searched[i] == 0)
            {
                ipGrpSearchUser(i, username, userType, searched);
            }
            SWORD_PRINTF("%s------%d, searched[%d] = %d\n", __func__, __LINE__, i, searched[i]);
            if(searched[i] == 1)
            {
                SWORD_PRINTF("%s------%d, searched[%d] = %d\n", __func__, __LINE__, i, searched[i]);
                retProf = prof; 
            }
        } 
    }

    return retProf;
}

static void waCancelByUser(char *user) 
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH;
    WebAuthProfile *prof = NULL;
    int min = 0, max = 0;
    int i = 0;
#if (IP_GRP == FYES)
    struct in_addr addr;
#endif
#if (WEB_AUTH_SHARE == FYES)
    int j=-1,k=0;
#endif

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) 
    {
        prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0) 
                && (prof->head.active == TRUE))
        {
            if(strcmp(user,prof->head.name) == 0)
            {
#if (WEB_AUTH_SHARE == FYES)
                j = prof->sesIndex;
                for(k=0;k<prof->actualSession;k++)
                {
                    if(checkSessionIndex(prof->sesIndex) == 1) 
                    {
                        if(nvramProfile->session[j].ip != 0)
                        {
#if DEBUG
                            printf("%s-%d: session index =%d, session ip =0x%x\n",__func__,__LINE__,j,nvramProfile->session[j].ip);
#endif
                            ipsetAOrDIp(SYS_WEB_AUTH_OK, nvramProfile->session[j].ip, IPSET_MEM_DEL);
#if (IP_GRP == FYES)
                            addr.s_addr = nvramProfile->session[j].ip;
                            SWORD_PRINTF("%s-%d: inet_ntoa(addr) =%s\n",__func__,__LINE__,inet_ntoa(addr));
                            userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
                            uttNfNlHashDelIp(uttNfNlSock, nvramProfile->session[j].ip);
#endif
                            prof->sesIndex = nvramProfile->session[j].nextIndex;
                            memset(&(nvramProfile->session[j]), 0U, sizeof(WebAuthSession));
                            nvramProfile->session[j].nextIndex = -1;
                            j = prof->sesIndex;
                        }
                    }
                }
                prof->sesIndex = -1;
                prof->actualSession = 0U;
#if DEBUG
                printf("%s-%d: prof->maxSession =%d, prof->actualSession =%d,prof->sesIndex =%d\n",__func__,__LINE__,
                        prof->maxSession,prof->actualSession,prof->sesIndex);
#endif
#if 0
                if(prof->sesHead != NULL) 
                {
                    printf("%s-%d: cacel user all session,prof->maxSession =%d,prof->actualSession =%d.\n",
                            __func__,__LINE__, prof->maxSession,prof->actualSession);
                    session = prof->sesHead;
                    for(;session != NULL;)
                    {

                        printf("%s-%d: i =%d, session->ip =0x%x\n",__func__,__LINE__,i,session->ip);
                        ipsetAOrDIp(SYS_WEB_AUTH_OK, session->ip, IPSET_MEM_DEL);
#if (IP_GRP == FYES)
                        addr.s_addr = session->ip;
                        SWORD_PRINTF("%s-%d: inet_ntoa(addr) =%s\n",__func__,__LINE__,inet_ntoa(addr));
                        userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
                        uttNfNlHashDelIp(uttNfNlSock, session->ip);/*删除hash表对应的ip*/
#endif
                        prof->sesHead = session->next;
                        free(session);
                        session = prof->sesHead;
                    }
                }
                prof->sesHead = NULL;
                prof->actualSession = 0;
#endif
#else /*WEB_AUTH_SHARE*/

                SWORD_PRINTF("%s-%d, i = %d, prof->ip is %x\n", __func__, __LINE__, i,  prof->ip);
                ipsetAOrDIp(SYS_WEB_AUTH_OK, prof->ip, IPSET_MEM_DEL);
#if (IP_GRP == FYES)
                addr.s_addr = prof->ip;
                SWORD_PRINTF("%s-----%d, inet_ntoa(addr) is %s\n", __func__, __LINE__,  inet_ntoa(addr));
                userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
                uttNfNlHashDelIp(uttNfNlSock, prof->ip);
#endif
                prof->ip = 0U;
                SWORD_PRINTF("%s-%d, user is %s,ip = %d\n", __func__, __LINE__, prof->user, prof->ip);
#endif /*WEB_AUTH_SHARE*/
            }
        }
    }
    return;
}

static struct UTT_WEB_AUTH* WebAuthConfigAdd(struct UTT_WEB_AUTH* utt_web)
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH;
    WebAuthProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    char userName[UTT_USER_LEN];
    char userPasswd[UTT_USER_LEN];
    char remark[UTT_USER_LEN];
    char idcode[UTT_USER_LEN];

    memmove(userName,utt_web->user,sizeof(userName));
    memmove(userPasswd,utt_web->psw,sizeof(userPasswd));
    memmove(remark,utt_web->remark,sizeof(remark));
    memmove(idcode,utt_web->idcode,sizeof(idcode));

#if (WEBAUTH_ACCOUNT == FYES) 
    Uint32 starttm  = 0U;
    Uint32 endtm    = 0U;
    Uint32 current  = getClock();
#endif
    Uint16 webIndexInst = 0;
    char instId[UTT_INST_NAME_LEN + 1];
    int min = 0, max = 0;
    int i = 0,instIndex = 0;
#if (WEBAUTH_ACCOUNT == FYES)
    starttm = (current/UTT_ONE_DAY)*UTT_ONE_DAY;
    endtm   = starttm + (utt_web->timetotal + 1)*UTT_ONE_DAY  - 1;
    uttSemP(SEM_WEBAUTH_NO, 0);
#endif
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min;i < max;i++)
    {
        prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
        {
            if(strncmp(prof->user,userName,UTT_USER_LEN) == 0)
            {
                if(utt_web->origin == MANUAL_METHOD)
                {
                    utt_web->retaction = UTT_ACTION_REPEAT;
                    goto out;
                }else if(utt_web->origin == IDCARD_METHOD){
#if (WEBAUTH_ACCOUNT == FYES)
                    uttSemV(SEM_WEBAUTH_NO, 0);
#endif
                    utt_web = WebAuthConfigModify(utt_web);
                    break;
                }
            }
        }
    }
    SWORD_PRINTF("%s-%d,authname is =%s,authpasswd is =%s,remark is =%s\n", __func__, __LINE__,userName,userPasswd,remark); 
    if(i == max)
    {
        webIndexInst = getWebInstName(&(nvramProfile->webauthInstRec.instRecMap), &(nvramProfile->webauthInstRec.instNameIndex));
        /* portmap����*/
        if (webIndexInst == 0) 
        {
            utt_web->retaction = UTT_ACTION_FULL;
            goto out;
        }
        memset(instId, 0, sizeof(instId));
        snprintf(instId, UTT_INST_NAME_LEN, "ID%d", webIndexInst);
        /* 已存在实例名 */
        if (ProfGetInstIndexByName(mibType, instId) != PROFFAIL) 
        {
            utt_web->retaction = UTT_ACTION_REPEAT;
            goto out;
        }
        instIndex = ProfNewInst(mibType, instId, FALSE);/*获取实例号并写上实例名**/
        if(instIndex == PROFFAIL)/*获取失败则表明已达最大条目 */
        {
            clearPortMap(&(nvramProfile->webauthInstRec.instRecMap), webIndexInst);
            utt_web->retaction = UTT_ACTION_FULL;
            goto out;
        }else{
            prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, instIndex);
            if((prof != NULL) && (ProfInstIsFree(prof) == 0))
            {
                ProfBackupByIndex(mibType, PROF_CHANGE_ADD, instIndex, &profhead);
                strcpy(prof->user,userName);
                strcpy(prof->passwd,userPasswd);
                strcpy(prof->remark,remark);
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
                strcpy(prof->idcode,idcode);
                prof->autoAuthMethod = utt_web->origin;
#endif
#if (WEBAUTH_ACCOUNT == FYES)
                if(utt_web->opentime == UTT_OPEN_START)
                {
                    prof->timeingPatternEn = FUN_ENABLE;
                    prof->timeStart   = starttm;
                    prof->timeStop    = endtm;
                }else if(utt_web->opentime == UTT_OPEN_STOP){
                    prof->timeingPatternEn = FUN_DISABLE;
                    prof->timeStart = 0U;
                    prof->timeStop = 0U;
                    prof->timeTotal = 0U;
                }
                SWORD_PRINTF("%s-%d timeStart : %d ;timeStop : %d\n", __func__, __LINE__,starttm,endtm);
#endif
                prof->maxSession  = utt_web->maxsession;
                prof->head.active = TRUE;
            }
        }
    }

    ProfUpdate(profhead);/*更新*/
    ProfFreeAllocList(profhead);
    nvramWriteCommit(); /*save*/
out:
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
    return utt_web;
}

static struct UTT_WEB_AUTH* WebAuthConfigModify(struct UTT_WEB_AUTH * utt_web)
{
    MibProfileType mibType  = MIB_PROF_WEB_AUTH;
    WebAuthProfile *prof    = NULL;
    struProfAlloc  *profhead = NULL;

    char userName[UTT_USER_LEN];
    char userPasswd[UTT_USER_LEN];
    char remark[UTT_USER_LEN];

    memmove(userName,utt_web->user,sizeof(userName));
    memmove(userPasswd,utt_web->psw,sizeof(userPasswd));
    memmove(remark,utt_web->remark,sizeof(remark));

#if (WEBAUTH_ACCOUNT == FYES)
    int profChange = 0;//是否只有总时间改变，只有总时间改变不挂断
    Uint32 starttm  = 0U;
    Uint32 endtm    = 0U;
    Uint32 current  = getClock();
#endif
    int  min = 0, max = 0, i = 0;
    int  instIndex = 0;
    char instId[ UTT_INST_NAME_LEN + 1];
#if (WEBAUTH_ACCOUNT == FYES)
    starttm = (current/UTT_ONE_DAY)*UTT_ONE_DAY;
    endtm   = starttm + (utt_web->timetotal + 1)*UTT_ONE_DAY - 1;
    uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min;i < max;i++)
    {    
        prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
        {    
            if(strncmp(prof->user,userName,UTT_USER_LEN) == 0)/*查看用户名是否已存在*/
            {    
                if(utt_web->opennet == UTT_OPEN_STOP)
                {
#if (WEBAUTH_ACCOUNT == FYES)
                    uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
                    utt_web = WebAuthListDel(utt_web);
                    goto out;
                }
                strcpy(instId,prof->head.name);
                break;
            }    
        }    
    }    
    if(i == max )/*可能有问*/
    {
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
        utt_web = WebAuthConfigAdd(utt_web);
        goto  out;
    }
    instIndex = ProfGetInstIndexByName(mibType, instId);
    if (instIndex == PROFFAIL) 
    {
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
        utt_web = WebAuthConfigAdd(utt_web);
        goto  out;
    }
    for(i = min;i < max;i++)
    {
        prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0))
        {
            ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, i, &profhead);

            if(strcmp(prof->user,userName) == 0)
            {
                strcpy(prof->user,userName);
                strcpy(prof->user,userName);
                strcpy(prof->passwd,userPasswd);
                strcpy(prof->remark,remark);
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
                prof->autoAuthMethod =   utt_web->origin;
#endif
#if (WEBAUTH_ACCOUNT == FYES)
                if(prof->timeingPatternEn == FUN_DISABLE)//ԭʼ״̬û�п����Ʒ�
                {
                    if(utt_web->opentime == UTT_OPEN_START)//�����Ʒ�ģʽ
                    {
                        prof->timeingPatternEn = FUN_ENABLE;
                        prof->timeStart        = starttm;
                        prof->timeStop         = endtm;
                    }
                }else{//ԭʼ״̬�����Ʒ�
                    if(utt_web->opentime == UTT_OPEN_STOP)//�رռƷ�ģʽ
                    {
                        prof->timeingPatternEn = FUN_DISABLE;
                        prof->timeStart = 0U;
                        prof->timeStop = 0U;
                        prof->timeTotal = 0U;
                    }else{//�����Ʒ�ģʽ
                        Uint32 timetotal = (prof->timeStop - prof->timeStart + 1)/(60*60*24) - 1;//����ԭ������
                        if(utt_web->timetotal > timetotal) //ʱ������
                        {
                            prof->timeStop     = prof->timeStart + (utt_web->timetotal + 1)*UTT_ONE_DAY - 1;
                        }else{//ʱ������
#if 0
                            if(utt_web->timetotal == 0)
                            {
                                prof->timeingPatternEn = FUN_DISABLE;
                                prof->timeStart   = 0U;
                                prof->timeStop    = 0U;
                            }else
#endif
                            {
                                //�Ѿ�����
                                if(current >= (prof->timeStart + (utt_web->timetotal + 1)*UTT_ONE_DAY)) 
                                {
                                    profChange = 1;
                                }else{
                                    prof->timeingPatternEn = FUN_ENABLE;
                                    prof->timeStop     = prof->timeStart + (utt_web->timetotal + 1)*UTT_ONE_DAY - 1;
                                }
                            }
                        }//ʱ������
                    }
                }
#endif
                prof->maxSession  = utt_web->maxsession;
#if (WEBAUTH_ACCOUNT == FYES)
                if (profChange == 1)//除了总时间有其他修改，就挂断当前连接
#endif
                {
                    waCancelByUser(prof->head.name);//根据实例名注销
                }
            }
        }
    }

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit(); //save
out:
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemV(SEM_WEBAUTH_NO, 0);//与speedweb冲突
#endif
    return utt_web;
}

static struct UTT_WEB_AUTH* WebAuthListDel(struct UTT_WEB_AUTH * utt_web)
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH;
    struProfAlloc *profhead = NULL;

    int min = 0, max = 0;
    char tmp[ UTT_INST_NAME_LEN + 1];
    int i = 0;
    char UserName[30];
    char userPasswd[30];
    char remark[30];

    memmove(UserName,utt_web->user,sizeof(UserName));
    memmove(userPasswd,utt_web->psw,sizeof(userPasswd));
    memmove(remark,utt_web->remark,sizeof(remark));

#if (IP_GRP == FYES)
    IpGrpProfile* referedProf = NULL;
    WebAuthProfile *prof = NULL;
    char referedMsg[50];
#endif
    int back = 0;
    Uint16 webIndexInst = 0;

#if (IP_GRP == FYES)
    memset(referedMsg, 0, sizeof(referedMsg));
#endif
    /*
     * 删除所有选择的PROFILE
     */
    SWORD_PRINTF("%s-%d, instIndexs = %s\n", __func__, __LINE__, UserName);
    if (UserName != NULL) 
    {
        ProfInstLowHigh(mibType, &max, &min);
        for(i = min;i < max;i++)
        {    
            prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
            if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
            {    
                if(strncmp(prof->user,UserName,UTT_USER_LEN) == 0)/*查看用户名是否已存在*/
                {    
                    strcpy(tmp,prof->head.name);
                    break;
                }    
            }    
        }    
        if(i == max )
        {
            return utt_web;
        }
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemP(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
        if(back == 0)/*由于可能删除多个，只做一次备份操作**/
        {
            ProfBackupByName(mibType, PROF_CHANGE_SPECIAL, tmp, &profhead);
            back = 1;
        }
        else
        {
            ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profhead);
        }
#if (IP_GRP == FYES)
        prof = ProfGetInstPointByName(MIB_PROF_WEB_AUTH, tmp);
        referedProf = userRefByIpGrp(prof->user, IPENT_WEB_AUTH);
        if(referedProf == NULL)
        {
            waCancelByUser(tmp);/*����ʵ����ע��*/
            ProfDelInstByName(mibType, tmp); 
        }
#else
        waCancelByUser(tmp);/*����ʵ����ע��*/
        ProfDelInstByName(mibType, tmp); 
#endif
        /*�����¼MAP*/
        if (strncmp(tmp, "ID", 2) == 0) 
        { 
            webIndexInst = (Uint16)strtol(&(tmp[2]), NULL, 10);
            clearPortMap(&(nvramProfile->webauthInstRec.instRecMap), webIndexInst);
        }
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemV(SEM_WEBAUTH_NO, 0);/*与speedweb冲突*/
#endif
        ProfUpdate(profhead);
        ProfFreeAllocList(profhead);
        nvramWriteCommit(); /*save*/
    }
    return utt_web;
}

struct UTT_WEB_AUTH* unpack(char *pack ,int size,int *checksum)//0表示失败,1表示成功
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *prof = NULL;
    struct UTT_WEB_HEAD  *utt_web_head = NULL;
    struct UTT_WEB_AUTH  *utt_web      = NULL;

    utt_web_head = (struct UTT_WEB_HEAD *)pack;
    if(size != (sizeof(struct UTT_WEB_HEAD) + sizeof(struct UTT_WEB_AUTH)) ||utt_web_head->start != 0x4242 || utt_web_head->version != 0x0101)
        return 0;
    utt_web = (struct UTT_WEB_AUTH *)utt_web_head->data;

    SWORD_PRINTF("%s-%d user : %s ; psw : %s ;remark : %s\n", __func__, __LINE__,utt_web->user,utt_web->psw,utt_web->remark);
    utt_web->retaction = utt_web->action;

    prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->idcard.enabled == 0))
    {

        utt_web->action    = UTT_ACTION_UNKNOWN;
        utt_web->retaction = UTT_ACTION_FIBDDEN;
    }
    utt_web->action = ntohs(utt_web->action); 
    utt_web->retaction = ntohs(utt_web->retaction); 
    utt_web->origin = ntohs(utt_web->origin); 
    utt_web->opennet = ntohs(utt_web->opennet); 
    utt_web->opentime = ntohs(utt_web->opentime); 
    utt_web->timetotal = ntohs(utt_web->timetotal); 
    utt_web->maxsession = ntohs(utt_web->maxsession); 

    switch(utt_web->action)
    {
        case  UTT_ACTION_ADD:
            WebAuthConfigAdd(utt_web);
            break;
        case  UTT_ACTION_DELETE:
            WebAuthListDel(utt_web);
            break;
        case  UTT_ACTION_MODIFY:
            WebAuthConfigModify(utt_web);
            break;
        default:
            break;
    }
    return utt_web;
}

int  pack(char * sendMsg,struct UTT_WEB_AUTH * utt_web_auth,int checksum)
{
    struct UTT_WEB_HEAD *utt_web_head = NULL;
    utt_web_head = (struct UTT_WEB_HEAD*)sendMsg;
    int size =0;
    size = sizeof(struct UTT_WEB_HEAD);
    utt_web_head->start   = 0x4242;
    utt_web_head->version = 0x0101;
    utt_web_head->checksum = checksum;
    utt_web_auth->action = ntohs(utt_web_auth->action); 
    utt_web_auth->retaction = ntohs(utt_web_auth->retaction); 
    utt_web_auth->origin = ntohs(utt_web_auth->origin); 
    utt_web_auth->opennet = ntohs(utt_web_auth->opennet); 
    utt_web_auth->opentime = ntohs(utt_web_auth->opentime); 
    utt_web_auth->timetotal = ntohs(utt_web_auth->timetotal); 
    utt_web_auth->maxsession = ntohs(utt_web_auth->maxsession); 
    memmove(utt_web_head->data,utt_web_auth,sizeof(struct UTT_WEB_AUTH));
    return size + sizeof(struct UTT_WEB_AUTH);
}
int main(int argc,char** argv)
{
    ProfInit();
    int sockfd,size;
    int sock_size = sizeof(struct sockaddr);
    char recvmsg[MAX_LEN];
    char sendmsg[MAX_LEN];
    struct sockaddr_in servaddr;
    struct sockaddr_in clientaddr;
    int checksum = 0;
    struct UTT_WEB_AUTH * utt_web_auth = NULL;

    if((argv[1] != NULL) && (strcmp(argv[1] , "debug") == 0))
    {
        flag = 1; 
    }

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0))<0)
    {
        SWORD_PRINTF("%s-%d socket create failed.\n", __func__, __LINE__);
        return -1;	
    }
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(2181u);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
    {
        SWORD_PRINTF("%s-%d bind failed.\n", __func__, __LINE__);
        return -1;
    }
    while(1) 
    {

        SWORD_PRINTF("%s-%d=======================================================================\nreceiving package ...\n", __func__, __LINE__);
        if((size=recvfrom(sockfd, recvmsg, MAX_LEN, 0,(struct sockaddr *)&clientaddr,&sock_size))<0)//接收客户端数据包
        {
            SWORD_PRINTF("%s-%drecvfrom client failed. \n", __func__, __LINE__);
            return -1;
        }
        checksum = 0;
        utt_web_auth = NULL;
        memset(sendmsg,0,sizeof(sendmsg));

        utt_web_auth = unpack(recvmsg,size,&checksum);
        size = pack(sendmsg,utt_web_auth,checksum);
        sendto(sockfd,sendmsg,size,0,(struct sockaddr *)&clientaddr,sizeof(struct sockaddr));//接收客户端数据包
    }
    close(sockfd);
    ProfDetach();
    return 0;
}
#else
int main(void)
{
    return 0;
}
#endif /* WEBAUTH_AUTO == FYES */
