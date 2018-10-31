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
#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

#define UTT_USER_LEN   31
#define web_len        sizeof(struct UTT_WEB_AUTH)

typedef enum utt_em_action{
    UTT_ACTION_UNKNOWN,
    UTT_ACTION_ADD,
    UTT_ACTION_DELETE,
    UTT_ACTION_MODIFY,
    UTT_ACTION_UPDATE,
};
typedef enum utt_em_open_net{
    UTT_OPEN_UNKNOWN,
    UTT_OPEN_START,
    UTT_OPEN_STOP,
};

struct UTT_WEB_AUTH
{
    unsigned short action;
    unsigned short retaction;
    unsigned short origin;
    unsigned short opennet;
    unsigned short opentime;
    unsigned short timetotal;
    unsigned short maxsession;
    char num[UTT_USER_LEN + 1];
    char psw[UTT_USER_LEN + 1];
    char user[UTT_USER_LEN + 1];
    char idcode[UTT_USER_LEN + 1];
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
#define MAX_LEN 1024*60
#define INDEX_LEN 400
#define MY_OFFSET(m,y) (&((m*)0)->y)

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

//返回同步的配置数
int  WebAuthConfigUpdate(char* packet,int web_max,int startindex)
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH;
    WebAuthProfile *prof = NULL;
    struct UTT_WEB_AUTH* utt_web_auth = NULL;

    utt_web_auth = (struct UTT_WEB_AUTH*)packet;

    int i = 0, min = 0, max = 0;
    int k = 0;
    Uint32 current  = getClock();

#if (WEBAUTH_ACCOUNT == FYES)
    uttSemP(SEM_WEBAUTH_NO, 0);//与speedweb冲突
#endif

    ProfInstLowHigh(mibType, &max, &min);
    if(startindex > max)
    {
        k = 0;
        goto out;
    }

    SWORD_PRINTF("%s-%d   startindex=%d;min = %d; max=%d; k=%d\n", __func__, __LINE__,startindex,min,max,k);
    for(i = startindex -1;i < max;i++)
    {    
        if(k == INDEX_LEN)
            goto out;
        prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);

        if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
        {    
            utt_web_auth->timetotal = 0;
            if(prof->timeingPatternEn == FUN_ENABLE)
            {
                SWORD_PRINTF("%s-%d timeStart : %d ;timeStop : %d\n", __func__, __LINE__,prof->timeStart,prof->timeStop);
                utt_web_auth->opentime  = htons(UTT_OPEN_START);
                utt_web_auth->timetotal = htons((prof->timeStop - prof->timeStart + 1)/(60*60*24) - 1);
            }else{
                utt_web_auth->opentime  = htons(UTT_OPEN_STOP);
                utt_web_auth->timetotal = 0;
            }
            utt_web_auth->maxsession    = htons(prof->maxSession);
            utt_web_auth->opennet       = htons(UTT_OPEN_START);
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
            utt_web_auth->origin        = htons(prof->autoAuthMethod); 
            strcpy(utt_web_auth->idcode,prof->idcode);
#endif
            memmove(utt_web_auth->num,prof->user,UTT_USER_LEN);
            memmove(utt_web_auth->psw,prof->passwd,UTT_USER_LEN);
            memmove(utt_web_auth->user,prof->remark,UTT_USER_LEN);
            utt_web_auth = utt_web_auth + 1;
            k++;
        }    
    }    
out:
#if (WEBAUTH_ACCOUNT == FYES)
    uttSemV(SEM_WEBAUTH_NO, 0);//与speedweb冲突
#endif
    return k;
}

int* unpack(char *packet,int *checksum)//0表示失败,1表示成功
{
    struct UTT_WEB_HEAD  *utt_web_head = NULL;

    utt_web_head = (struct UTT_WEB_HEAD *)packet;
#ifdef SWORD_DEBUG
    printf("start : %x\t version : %x\t checksum : %x ;\n",utt_web_head->start,utt_web_head->version,utt_web_head->checksum);
#endif
    if(utt_web_head->start != 0x4242 || utt_web_head->version != 0x0101)
        return 0;
    return ntohs(utt_web_head->index);
}

int  pack(char * sendMsg,char *buffer,int checksum,int sizeed,int startindex)
{
    struct UTT_WEB_HEAD *utt_web_head = NULL;
    utt_web_head = (struct UTT_WEB_HEAD*)sendMsg;
    int size =0;
    size = sizeof(struct UTT_WEB_HEAD);
    utt_web_head->start    = htons(0x4242);
    utt_web_head->version  = htons(0x0101);
    utt_web_head->checksum = htons(checksum);
    utt_web_head->index    = htons(sizeed + startindex);
    if(sizeed == 0 || sizeed != INDEX_LEN)
        utt_web_head->index    = 0;
    memmove(utt_web_head->data,buffer,sizeed*web_len);
    return size + web_len*sizeed;
}

int main(void)
{
    ProfInit();
    int sockfd,size;
    int sock_size = sizeof(struct sockaddr);
    char recvmsg[MAX_LEN];
    char sendmsg[MAX_LEN];
    char buffer[MAX_LEN];
    struct sockaddr_in servaddr;
    struct sockaddr_in clientaddr;
    int checksum = 0;
    unsigned short startindex = 0;
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0))<0)
    {
        printf("Socket created failed.\n");
        return -1;	
    }
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(2182u);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int mem = MAX_LEN;
    setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &mem, sizeof(mem));

    if(bind(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
    {
        printf("bind failed.\n");	
        return -1;
    }
    while(1) 
    {
#ifdef SWORD_DEBUG
        printf("***************************************************************************************************\nreceiving package ...\n");	
#endif
        size = -1;
        if((size=recvfrom(sockfd, recvmsg, MAX_LEN, 0,(struct sockaddr *)&clientaddr,&sock_size))<0)//接收客户端数据包
        {
            printf("recv from server failed. \n");
            return -1;
        }
        startindex = 0;
        checksum   = 0;
        memset(sendmsg,0,sizeof(sendmsg));
        memset(buffer,0,sizeof(buffer));

        if((startindex = unpack(recvmsg,&checksum)) >= 1)
        {
            size = WebAuthConfigUpdate(buffer,sizeof(buffer),startindex);
            size = pack(sendmsg,buffer,0,size , startindex);
            sendto(sockfd,sendmsg,size,0,(struct sockaddr *)&clientaddr,sizeof(struct sockaddr));//接收客户端数据包*/
        }
#ifdef SWORD_DEBUG
        printf("sizeof(UTT_WEB_AUTH)=%d;sendsize = %d \n",sizeof(struct UTT_WEB_AUTH),size);
#endif
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
#endif
