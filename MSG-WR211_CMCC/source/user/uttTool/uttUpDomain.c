#include <stdlib.h>
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
#include "netdb.h"

#if (IP_SEC == FYES) || (WIFIDOG_AUTH == FYES )


#ifdef IPSEC_DEBUG
#define IPSEC_PRINT(fmt, args...)		printf(fmt, ## args)
#else
#define IPSEC_PRINT(fmt, args...)
#endif

static void usage()
{
#if (IP_SEC == FYES)
    printf("    %s ipsec\n",__func__);
#elif(WIFIDOG_AUTH == FYES )
    printf("    %s wifi  -- update weixin and except url domain\n",__func__);
#endif
}

#if (IP_SEC == FYES)
/*
 * getDomainNameIP(char *peer,char *retIP)
 * input: 
 *	peer -- 域名
 * output:
 *	retIP -- ip地址
 * 获取域名IP地址
 */
static void getDomainNameIP(char *peer,char *retIP)
{
    struct hostent *host;
    struct in_addr inaddr;
    host = gethostbyname(peer);
    //inaddr.s_addr=*(unsigned long *)host->h_addr;
    if(host!=NULL)
    {
	memcpy((char *) &inaddr, (char *) host->h_addr_list[0],(size_t) host->h_length);
	strcpy(retIP,inet_ntoa(inaddr));
	IPSEC_PRINT("%s-%d: hostname=%s,ip[0]=%s,retIP=%s\n",__func__,__LINE__,host->h_name,inet_ntoa(inaddr),retIP);
    }
    return;
}
#endif
#if(WIFIDOG_AUTH == FYES )
static void getDomainNameIP_wifi(char *domain,char *ipset_grp)
{
    struct hostent *host;
    struct in_addr inaddr;
    char **ptr = NULL;
    char ip[20];

    host = gethostbyname(domain);
    //inaddr.s_addr=*(unsigned long *)host->h_addr;
    if(host!=NULL)
    {
	for(ptr=host->h_addr_list;*ptr != NULL;ptr++) {
	memcpy((char *) &inaddr, *ptr,(size_t) host->h_length);
	memset(ip, 0, sizeof(ip));
	strcpy(ip,inet_ntoa(inaddr));
	ipsetAOrDIpStr(ipset_grp, ip, IPSET_MEM_ADD);                                   
	}
    }
    return;
}
/*
 * WechatDomainAdd
 * 微信域名放通
 * 通过配置查找放通域名
 *
 * */
static void WechatDomainAdd_wifi(void){
    MibProfileType mibType = MIB_PROF_PDB_GLOBAL;
    PdbProfile *prof= NULL;
    char *ptr = NULL, *start = NULL;
    int i,j;
#if 0
    int max, min;
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) 
#endif
    {
	i = 168;
	prof = (PdbProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) {
	    if((prof->type == wifidogUp) && (i == 168)) {
		for(j=0;j<MAX_PDB_LINE_COUNT;j++)
		{
		    if((strlen(prof->line[j])!=0) && (strncmp(prof->line[j], "DNS|",4) == 0))
		    {
			start = (prof->line[j])+4;
			ptr = strtok(start, ";");
			while(ptr != NULL) {
			    getDomainNameIP_wifi(ptr,WEIXIN_IPSET_GRP);
			    ptr = strtok(NULL, ";");
			}
		    }
		}
	    } else {
		fprintf(stderr, "[ERROR] please check wechat policy config instindex.\n");
	    }
	}
    }
}
#endif
/*
 * updateDomainIP()
 *
 * 更新域名 IP地址
 *
 * 如果IP变化，则重新连接ipsec
 */
int main(int argc, char** argv)
{
#if (IP_SEC == FYES)
    MibProfileType mibipsec = MIB_PROF_IPSEC;
    ipsecConfProfile *ipsecProf = NULL;
    struProfAlloc *profList = NULL;
    int max=0, min=0,index=-1;
    ProfChangeType profAction = PROF_CHANGE_EDIT;
    char domainIP[MAX_IPSEC_IP_LEN+1]={0};
    int flag=1;
#endif

    ProfInit();
    if(argc < 2)    /*argument cannot less than 2*/
    {
	IPSEC_PRINT("%s-%d: exit.\n",__func__,__LINE__);
	usage();
	exit(0);
    }
    if(strcmp(argv[1], "ipsec")==0)
    {
#if (IP_SEC == FYES)
	while(1)
	{
	    sleep(60);	    /* updae every 60 seconds */
	    ProfInstLowHigh(mibipsec, &max, &min);
	    for(index=min;index < max;index++)
	    {
		memset(domainIP, 0, sizeof(domainIP));
		ipsecProf = (ipsecConfProfile *)ProfGetInstPointByIndex(mibipsec, index);
		if((ipsecProf!=NULL)&&(ProfInstIsFree(ipsecProf) == 0))
		{
		    getDomainNameIP(ipsecProf->peer, domainIP);		/* get domain ip */
		    if(strlen(domainIP)!=0) {
			if(strcmp(domainIP, ipsecProf->peerIP)!=0)	/* compare ip same or not */
			{
			    ProfBackupByIndex(mibipsec, profAction, index, &profList);
			    strcpy(ipsecProf->peerIP, domainIP);	/* save new domain ip */
			    IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
			}
		    }
		}
	    }
	    if(profList!=NULL)
	    {
		ProfUpdate(profList);		/* update */
		ProfFreeAllocList(profList);    /* erlease */
		IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
		nvramWriteCommit();		/* save */
		profList=NULL;			/* set null for next circle*/
	    }
	}
#endif
    }
#if(WIFIDOG_AUTH == FYES )
    else if(strcmp(argv[1], "wifi")==0)
    {
    while(1) {
	sleep(5*60);
	ipsetFlushGrp(WEIXIN_IPSET_GRP);
	getDomainNameIP_wifi(WEIXIN_DOMAIN_SHORT,WEIXIN_IPSET_GRP);
	getDomainNameIP_wifi(WEIXIN_DOMAIN_LONG,WEIXIN_IPSET_GRP);
	getDomainNameIP_wifi(WEIXIN_DOMAIN_SZLONG,WEIXIN_IPSET_GRP);
	getDomainNameIP_wifi(WEIXIN_DOMAIN_SZSHORT,WEIXIN_IPSET_GRP);
	WechatDomainAdd_wifi();
#if(WHITELIST ==1)
#ifndef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
/*移到内核实现，通过iptables的WiFiMatch匹配模块实现*/
	{
	    int min_i, max_i, index_i;
	    MibProfileType mibType = MIB_PROF_WHITE_LIST;
	    WhiteListProfile *prof = NULL;

	    ipsetFlushGrp(WHITE_LIST);
	    ProfInstLowHigh(mibType, &max_i, &min_i);
	    for (index_i = min_i; index_i < max_i; index_i++)
	    {
		prof = (WhiteListProfile *)ProfGetInstPointByIndex(mibType, index_i);
		if((prof->domain != NULL) && (strlen(prof->domain) > 0U)) 
		{
		    getDomainNameIP_wifi(prof->domain,WHITE_LIST);			/*加入WHITE_LIST*/ 
		}
	    }
	}
#endif
#endif
    }
    }
#endif
    else {
	usage();
    }
	
    ProfDetach();
    exit(0);
}
#else
int main(int argc, char** argv)
{
    exit(0);
}
#endif
