/*
 * added by baidexiu at 2009/08/18      
 * for GlobalBase config 
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include "nvram.h"
#include "utils.h"
#include "webs.h"
#include "firewall.h"
#include "internet.h"
#include "uttfunction.h"

#define SERVER_IP "202.101.35.232"
#define POLICY_MAX_LENGTH 200
#define NAME_MAX_LENGTH 18
extern int EditOneIptablesGroup(char *GroupList,char action ,char *lan_if_name ,char *wan_if_name ,int mode);
extern int num;


/***********************************************************************
 * 函 数 名：   createPolicyFile 
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void createPolicyFile(char *name)
{
    char_t file_name[NAME_MAX_LENGTH+25]={0};
    FILE *fp;
    sprintf(file_name,"/etc_ro/l7-protocols/%s.pat",name);
    if(!(fp=fopen(file_name,"w+")))
	return;
    else
    {
	strcat(name,"\n");
	fputs(name,fp);
	name[strlen(name)-1]='\0';
	fputs(nvram_bufget(RT2860_NVRAM,name),fp);
	fclose(fp);
    }
}
/***********************************************************************
 * 函 数 名：   initPolicyFile
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
void initPolicyFile(void)
{
    char_t name[NAME_MAX_LENGTH]={0};
    strcpy(name,"qq");
    createPolicyFile(name);
    strcpy(name,"msnmessenger");
    createPolicyFile(name);
    strcpy(name,"xunlei");
    createPolicyFile(name);
    strcpy(name,"bittorrent");
    createPolicyFile(name);
}
/***********************************************************************
 * 函 数 名：   formUpdatePolicy
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void formUpdatePolicy(webs_t wp, char_t *path, char_t *query)
{
    char_t *page=NULL;
    FILE *fp;
    char_t name[NAME_MAX_LENGTH]={0};
    char_t value[POLICY_MAX_LENGTH]={0};
    char_t file_name[NAME_MAX_LENGTH+25]={0};
    //time_t time_begin, time_end;
    char_t *policy_name;
    policy_name=websGetVar(wp,T("policy_name"),T("")); 
    DBGPRINT("policy_name is %s\n",policy_name);
    if(!strcmp(policy_name,"DenyQQ"))
	strcpy(name,"qq");
    else if(!strcmp(policy_name,"DenyMSN"))
	strcpy(name,"msnmessenger");
    else if(!strcmp(policy_name,"DenyBT"))
	strcpy(name,"bittorrent");
    else
	strcpy(name,"xunlei");
    page = websGetVar(wp,T("page"),T("")); 
    if(strcmp(page, T("Group")) == 0)
        setUpPdbFromWhichPage("Group.asp");
    else
        setUpPdbFromWhichPage("GlobalBase.asp");
    websRedirect(wp, "pdbup.asp");
    //time_begin = time(NULL);
    doSystem("cd /etc_ro/l7-protocols/ && tftp -g -r %s.pat -l %s %s",name,name,SERVER_IP); 
    //time_end = time(NULL);
    //DBGPRINT("time space is %f seconds\n",difftime(time_end ,time_begin));
    sprintf(file_name,"/etc_ro/l7-protocols/%s",name);
    if(!(fp=fopen(file_name,"r")))
    {
	setTheErrorMsg(T("更新策略失败，仍采用旧策略!"));
	return;
    }
    else
    {
	fgets(value,POLICY_MAX_LENGTH,fp);	
	if(strlen(value)==0)
	{
	    setTheErrorMsg(T("更新策略失败，仍采用旧策略!"));
	    fclose(fp);
	    return;
	}
	fgets(value,POLICY_MAX_LENGTH,fp);	
	value[strlen(value)-1]='\0';
	DBGPRINT("%s=%s\n",name,value);
	nvram_bufset(RT2860_NVRAM,name,value);
	nvram_commit(RT2860_NVRAM);
        fclose(fp);
	doSystem("rm /etc_ro/l7-protocols/%s",name); 
    }
    createPolicyFile(name);
    sendMsgForSpeed(INIT_GLOBAL_BASE, 0, 0, NULL, 0);
    setTheRightMsg(T("更新策略成功!"));
}
#if 0
void init_GlobalBase(void);
/***********************************************************************
 * 函 数 名：   init_GlobalBase
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
void init_GlobalBase(void)
{
    struct l7pro {
        char *enable;
        char pro_name[16];
    }l7Pro[]={
        {.enable = "", .pro_name = "qq"},
        {.enable = "", .pro_name = "msnmessenger"},
        {.enable = "", .pro_name = "bittorrent"},
        {.enable = "", .pro_name = "xunlei"}
    };
    int i=0;
    //char *denyQQ,*denyMSN,*denyBT,*denyThunder;
    char *day,*timestart,*timestop ; 
    char *PoeSerEn, *poeSerStartIp=NULL, *PoeSerEndIp;
    struct in_addr ipaddr;
    char days[28]={'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
    char_t gateway[16]  ,lanIfMask[16] ;
    strcpy(gateway ,nvram_bufget(RT2860_NVRAM, "lan_ipaddr"));
    strcpy(lanIfMask ,nvram_bufget(RT2860_NVRAM, "lan_netmask"));
    l7Pro[0].enable=nvram_bufget(RT2860_NVRAM,"GLOBALBASE_DENYQQ");
    l7Pro[1].enable=nvram_bufget(RT2860_NVRAM,"GLOBALBASE_DENYMSN");
    l7Pro[2].enable=nvram_bufget(RT2860_NVRAM,"GLOBALBASE_DENYBT");
    l7Pro[3].enable=nvram_bufget(RT2860_NVRAM,"GLOBALBASE_DENYTHUNDER");
    day=nvram_bufget(RT2860_NVRAM,"GLOBALBASE_DAY");
    timestart=nvram_bufget(RT2860_NVRAM,"GLOBALBASE_TIMESTART");
    timestop=nvram_bufget(RT2860_NVRAM,"GLOBALBASE_TIMESTOP");

    if(!strcmp(day,""))
	day="1111111";
    if(!strcmp(timestart,""))
	timestart="All";
    if(!strcmp(timestart,"All"))
    {
	timestart="00:00";
	timestop="23:59";
    }
    getDays(days2Byte(day),days);
    days[27]='\0';

    PoeSerEn = nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_ENABLE);
    if(strcmp(PoeSerEn, "on")==0) {
         unsigned int ipaddrl=0U;
         poeSerStartIp = nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_STARTIP);
         inet_aton(poeSerStartIp, &ipaddr);
         DBGPRINT("ipaddr[%x],", ipaddr.s_addr);
         ipaddrl=ntohl(ipaddr.s_addr);
         DBGPRINT("hl addr[%x],", ipaddrl);
         ipaddrl += UTT_MAX_PPPOE_USER - 1;
         DBGPRINT("hl addr2[%x],", ipaddrl);
         ipaddr.s_addr=htonl(ipaddrl);
         DBGPRINT("ipaddr2[%x]\n", ipaddr.s_addr);
         PoeSerEndIp=inet_ntoa(ipaddr);
     }

    doSystem("iptables -t mangle -F %s" ,GLOBAL_CHAIN);
    for (i=0; i<sizeof(l7Pro)/sizeof(l7Pro[0]); i++) {
        DBGPRINT("iptables config: i[%d], l7Pro[i].enable[%s], l7Pro[i].pro_name[%s]\n", i, l7Pro[i].enable, l7Pro[i].pro_name);
        if(!strcmp(l7Pro[i].enable,"on")) {
	    doSystem("iptables -t mangle -%c %s -s %s/%s -m layer7 --l7proto %s -m time --timestart %s --timestop %s --days %s -j DROP",GLOBAL_ADD ,GLOBAL_CHAIN ,gateway, lanIfMask, l7Pro[i].pro_name, timestart,timestop,days);
            if(strcmp(PoeSerEn, "on")==0) {
	        doSystem("iptables -t mangle -%c %s -m iprange --src-range %s-%s -m layer7 --l7proto %s -m time --timestart %s --timestop %s --days %s -j DROP",GLOBAL_ADD ,GLOBAL_CHAIN ,poeSerStartIp, PoeSerEndIp, l7Pro[i].pro_name, timestart,timestop,days);
            }
        }
    }
#if 0
    if(!strcmp(denyQQ,"on"))
	doSystem("iptables -t mangle -%c %s -s %s/%s -m layer7 --l7proto qq -m time --timestart %s --timestop %s --days %s -j DROP",GLOBAL_ADD ,GLOBAL_CHAIN ,gateway, lanIfMask,timestart,timestop,days);
    if(!strcmp(denyMSN,"on"))
	doSystem("iptables -t mangle -%c %s -s %s/%s -m layer7 --l7proto msnmessenger -m time --timestart %s --timestop %s --days %s -j DROP",GLOBAL_ADD,GLOBAL_CHAIN, gateway, lanIfMask,timestart,timestop,days);
    if(!strcmp(denyBT,"on"))
	doSystem("iptables -t mangle -%c %s -s %s/%s -m layer7 --l7proto bittorrent -m time --timestart %s --timestop %s --days %s -j DROP",GLOBAL_ADD,GLOBAL_CHAIN, gateway, lanIfMask,timestart,timestop,days);
    if(!strcmp(denyThunder,"on"))
	doSystem("iptables -t mangle -%c %s -s %s/%s -m layer7 --l7proto xunlei -m time --timestart %s --timestop %s --days %s -j DROP",GLOBAL_ADD,GLOBAL_CHAIN, gateway, lanIfMask,timestart,timestop,days);
#endif
}
#endif
/***********************************************************************
 * 函 数 名：   formConfigGlobalBase
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/

static void formConfigGlobalBase(webs_t wp, char_t *path, char_t *query)
{
    char *denyQQ,*denyMSN,*denyBT,*denyThunder,*day,*timestart,*timestop; 
    denyQQ=websGetVar(wp,T("DenyQQ"),T(""));
    denyMSN=websGetVar(wp,T("DenyMSN"),T(""));
    denyBT=websGetVar(wp,T("DenyBT"),T(""));
    denyThunder=websGetVar(wp,T("DenyThunder"),T(""));
    day=websGetVar(wp,T("day"),T(""));
    timestart=websGetVar(wp,T("timestart"),T(""));
    timestop=websGetVar(wp,T("timestop"),T(""));
    nvram_bufset(RT2860_NVRAM,"GLOBALBASE_DENYQQ",denyQQ);
    nvram_bufset(RT2860_NVRAM,"GLOBALBASE_DENYMSN",denyMSN);
    nvram_bufset(RT2860_NVRAM,"GLOBALBASE_DENYBT",denyBT);
    nvram_bufset(RT2860_NVRAM,"GLOBALBASE_DENYTHUNDER",denyThunder);
    nvram_bufset(RT2860_NVRAM,"GLOBALBASE_DAY",day);
    nvram_bufset(RT2860_NVRAM,"GLOBALBASE_TIMESTART",timestart);
    nvram_bufset(RT2860_NVRAM,"GLOBALBASE_TIMESTOP",timestop);
    nvram_commit(RT2860_NVRAM);
    sendMsgForSpeed(INIT_GLOBAL_BASE, 0, 0, NULL, 0);
    websRedirect(wp, "GlobalBase.asp");
}
/***********************************************************************
 * 函 数 名：   getGlobalBase
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static int getGlobalBase(int eid, webs_t wp, int argc, char_t **argv)
{
    websWrite(wp,"var DenyQQs=\"%s\";",nvram_bufget(RT2860_NVRAM,"GLOBALBASE_DENYQQ"));
    websWrite(wp,"var DenyMSNs=\"%s\";",nvram_bufget(RT2860_NVRAM,"GLOBALBASE_DENYMSN"));
    websWrite(wp,"var DenyBTs=\"%s\";",nvram_bufget(RT2860_NVRAM,"GLOBALBASE_DENYBT"));
    websWrite(wp,"var DenyThunders=\"%s\";",nvram_bufget(RT2860_NVRAM,"GLOBALBASE_DENYTHUNDER"));
    websWrite(wp,"var days=\"%s\";",nvram_bufget(RT2860_NVRAM,"GLOBALBASE_DAY"));
    websWrite(wp,"var timestarts=\"%s\";",nvram_bufget(RT2860_NVRAM,"GLOBALBASE_TIMESTART"));
    websWrite(wp,"var timestops=\"%s\";",nvram_bufget(RT2860_NVRAM,"GLOBALBASE_TIMESTOP"));
    getTheErrorMsg(eid, wp, argc, argv);  
    getTheRightMsg(eid, wp, argc, argv);  
    return 0;
}

/***********************************************************************
 * 函 数 名：   formDefineGlobalBase
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
void formDefineGlobalBase(void)
{
        websFormDefine(T("ConfigGlobalBase"), formConfigGlobalBase);
	websAspDefine(T("aspOutGlobalBase"), getGlobalBase);
	websFormDefine(T("UpdatePolicy"),formUpdatePolicy);
}
