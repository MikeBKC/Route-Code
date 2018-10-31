
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include "utils.h"
#include "webs.h"
#include "firewall.h"
#include "internet.h"
#include "uttfunction.h"


//存放配置参数的查找序号
#define INDEX_GroupName    0 
//#define COEFFICIENT	   / 11
#define PACKET_SIZE     32	
#define	COEFFICIENT	*(1024/(PACKET_SIZE*8)) 
#define	WEB_COEFFICIENT	    11

//记录组管理条目
static int GroupConfigCounter=0;
int num;
/***********************************************************************
 * 函 数 名：   div8
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void div8(char * src, int division)
{
    int i=0;
    char unit[2]="m";
    while(src[i]!='\0')
    {
	if(src[i]=='k')
	{
	    unit[0]=src[i];
	    src[i]='\0';
	    sprintf(src,"%.0f",atof(src)  *(1024/(division*8)) );
	    //strcat(src,unit);
	    break;
	}
        else if(src[i]=='m')
	{
	    unit[0]='k';
	    src[i]='\0';
	    sprintf(src,"%.0f",atof(src)*1024  *(1024/(division*8)) );
	    //strcat(src,unit);
	    break;
	}
	i++;
    }
}
#if 0
/***********************************************************************
 * 函 数 名：   only
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void only(int policy ,int begin_ip ,int ip_num ,char *if_name,char *rate ,char *ceil ,char *timestart,char *timestop,char *days,char *parm ,char *parm1 ,char *parm2)
{
    int i=0;
    char ip[16]={0};
    char ip1[16]={0};
    char ip2[16]={0};
    if(policy==1)
    {
#if 0
	for(i=0;i<ip_num;i++) 
	{
	    memset(ip,0,16);
	    int2ip(begin_ip,ip);
	    doSystem("iptables -t mangle -%c %s %s %s -m time --timestart %s --timestop %s --days %s -m limit --limit %s/s --limit-burst %s -j ACCEPT", GROUP_ADD ,parm, parm1 ,ip,timestart,timestop,days ,rate ,rate);
	    doSystem("iptables -t mangle -%c %s %s %s -m time --timestart %s --timestop %s --days %s -j DROP",GROUP_ADD ,parm, parm1 ,ip,timestart,timestop,days);
	    begin_ip++;
	}
#else
        unsigned int irate=atoi(rate)*PACKET_SIZE;
        char aRate[32]={0};
        sprintf(aRate, "%d", irate);
	doSystem("iptables -t mangle -%c %s -m iprange --%s-range  %s-%s -m time --timestart %s --timestop %s --days %s -m hashlimit --hashlimit-name %s%s --hashlimit %s/sec --hashlimit-burst %s --hashlimit-mode %sip -j ACCEPT",GROUP_ADD ,parm, parm2 ,int2ip(begin_ip,ip1),int2ip(begin_ip+ip_num-1,ip2) ,timestart,timestop,days ,parm1, parm2, aRate ,aRate, parm2);
	doSystem("iptables -t mangle -%c %s -m iprange --%s-range  %s-%s -m time --timestart %s --timestop %s --days %s -j DROP",GROUP_ADD ,parm ,parm2 ,ip1,ip2,timestart,timestop,days);
#endif
    }
    else
    {
	doSystem("iptables -t mangle -%c %s -m iprange --%s-range  %s-%s -m time --timestart %s --timestop %s --days %s -m limit --limit %s/s --limit-burst %s -j ACCEPT",GROUP_ADD ,parm, parm2 ,int2ip(begin_ip,ip1),int2ip(begin_ip+ip_num-1,ip2) ,timestart,timestop,days ,rate ,rate);
	doSystem("iptables -t mangle -%c %s -m iprange --%s-range  %s-%s -m time --timestart %s --timestop %s --days %s -j DROP",GROUP_ADD ,parm ,parm2 ,ip1,ip2,timestart,timestop,days);
    }
}
/***********************************************************************
 * 函 数 名：   EditOneIptablesGroup
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
int EditOneIptablesGroup(char *GroupList,char action ,char *lan_if_name, char *wan_if_name ,int mode)
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
    char_t *target=NULL;
    char_t days[28]={0},group_timestart[6]={0},group_timestop[6]={0},ip1[16]={0},ip2[16]={0};
    char_t GroupName[16]={0}, policy[2]={0} ,dRate[7]={0} ,dCeil[7]={0} ,uRate[7]={0} ,uCeil[7]={0},*Rate=NULL ; 
    unsigned int FromIp,EndIp, FromIp2=0U, EndIp2=0U;
    unsigned int group_day;
    char *timestart=T("All"),*group_starttmp=T("00:00"),*group_stoptmp=T("23:59");
    if(sscanf(GroupList,"%*[^,],%x,%x",&FromIp,&EndIp)==2) {
        sscanf(GroupList,"%[^,],%*[^,],%*[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%x,%[^,],%[^;]",
            GroupName, policy,dRate, dCeil,uRate,uCeil,l7Pro[0].enable,l7Pro[1].enable,
            l7Pro[2].enable,l7Pro[3].enable,&group_day,group_timestart,group_timestop);
        getDays(group_day,days);
        if(!strcmp(group_timestart,timestart)){
    	    strcpy(group_timestart,group_starttmp);
    	    strcpy(group_timestop,group_stoptmp);
        }
    
        if ( (EndIp | FromIp) == 0U) {
            char net_area[19];
            FromIp=getInNet(net_area, "0");
            EndIp=getInNet(net_area, "255");

            char *PoeSerEn, *poeSerStartIp=NULL;
            PoeSerEn = nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_ENABLE);
            if(strcmp(PoeSerEn, "on")==0) {
                 poeSerStartIp = nvram_bufget(RT2860_NVRAM, _CONF_PPPOE_S_STARTIP);
                 inet_aton(poeSerStartIp, &ipaddr);
                 FromIp2=ntohl(ipaddr.s_addr);
                 EndIp2 = FromIp2 + UTT_MAX_PPPOE_USER - 1;
             }
        }
        for (i=0; i<sizeof(l7Pro)/sizeof(l7Pro[0]); i++) {
            if(!strcmp(l7Pro[i].enable,"1")) {
                target="DROP";
            } else {
                target="ACCPET";
            }
    	    doSystem("iptables -t mangle -%c %s -m iprange --src-range %s-%s \
                -m layer7 --l7proto %s -m time --timestart %s --timestop %s --days %s -j %s",
                action, GROUP_CHAIN ,int2ip(FromIp,ip1),int2ip(EndIp,ip2), 
                l7Pro[i].pro_name, group_timestart,group_timestop,days,target);
            /* for PPPoE Server Users */
            if( (FromIp2|EndIp2) == 0U) {
    	        doSystem("iptables -t mangle -%c %s -m iprange --src-range %s-%s \
                    -m layer7 --l7proto %s -m time --timestart %s --timestop %s --days %s -j %s",
                    action, GROUP_CHAIN ,int2ip(FromIp2,ip1),int2ip(EndIp2,ip2), 
                    l7Pro[i].pro_name, group_timestart,group_timestop,days,target);
            }
        }

        if(!strcmp(dRate,"nolimit") && !strcmp(uRate,"nolimit")) {
    	    return 0;
        } else if(strcmp(dRate,"nolimit")) {
            limittype="dst";
            Rate=dRate;
        } else if(strcmp(uRate,"nolimit")){
            limittype="src";
            Rate=uRate;
        }

        int j;
        for(j=0; j<2; j++ ) {
    	    if(!strcmp(policy,"1")) {
    	        div8(Rate,1);
	        doSystem("iptables -t mangle -%c %s -m iprange --%s-range  %s-%s -m time --timestart %s --timestop %s --days %s \
                        -m hashlimit --hashlimit-name %s%s --hashlimit %s/sec --hashlimit-burst %s --hashlimit-mode %sip -j ACCEPT",
                        GROUP_ADD ,GROUP_LIMIT_CHAIN, limittype,int2ip(FromIp,ip1),int2ip(EndIp,ip2),
                        timestart,timestop,days ,GroupName, limittype, Rate ,Rate, limittype);
	        doSystem("iptables -t mangle -%c %s -m iprange --%s-range  %s-%s -m time --timestart %s \
                        --timestop %s --days %s -j DROP",
                        GROUP_ADD ,GROUP_LIMIT_CHAIN, limittype ,ip1,ip2,timestart,timestop,days);
            } else {
    	        div8(Rate,8);
	        doSystem("iptables -t mangle -%c %s -m iprange --%s-range  %s-%s -m time --timestart %s --timestop %s --days %s \
                        -m limit --limit %s/s --limit-burst %s -j ACCEPT",
                        GROUP_ADD ,GROUP_LIMIT_CHAIN,limittype ,int2ip(FromIp,ip1),int2ip(EndIp,ip2) ,
                        timestart,timestop,days ,Rate ,Rate);
	        doSystem("iptables -t mangle -%c %s -m iprange --%s-range  %s-%s -m time --timestart %s \
                        --timestop %s --days %s -j DROP",
                        GROUP_ADD ,GROUP_LIMIT_CHAIN ,limittype ,ip1,ip2,timestart,timestop,days);
            }
            if( (FromIp2|EndIp2) == 0U) {
                break;
            }else {
                /* for PPPoE Users */
                FromIp=FromIp2;
                EndIp=EndIp2;
            }
        }
    }
    return 0;
}
#endif
/***********************************************************************
 * 函 数 名：   getOneGroupConfig
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static int getOneGroupConfig(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t *edit_name,*GroupList,*tmp2,data[16]={0};
	int ct=0;
	char *tmp1;
    unsigned int d;

	char_t *GroupName=T("GroupNames = \"%s\";");
    char_t *FromIP =T("FromIPs = \"%s\";");
    char_t *EndIP = T("EndIPs = \"%s\"; ");
    char_t *policy = T("policy = \"%s\"; ");
    char_t *dRate = T("dRate = \"%s\"; ");
    char_t *dCeil = T("dCeil = \"%s\"; ");
    char_t *uRate = T("uRate = \"%s\"; ");
    char_t *uCeil = T("uCeil = \"%s\"; ");

	char_t *DenyQQ = T("DenyQQs = \"%s\"; ");
	char_t *DenyMSN=T("DenyMSNs = \"%s\";");
    char_t *DenyBT =T("DenyBTs = \"%s\";");
    char_t *DenyThunder = T("DenyThunders= \"%s\"; ");
	char_t *group_day = T("group_days = \"%s\"; ");
	char_t *group_timestart = T("group_timestarts = \"%s\"; ");
	char_t *group_timestop = T("group_timestops = \"%s\"; ");
    if(ejArgs(argc,argv,T("%s"), &edit_name) <1) 
    {
	setTheErrorMsg(T("读取edit_Name变量失败!"));
        goto out;
    }
    GroupList = nvram_bufget(RT2860_NVRAM,"GroupConfigList");
    tmp1=strnmatch(GroupList,edit_name,INDEX_GroupName);
    if(!tmp1)
    {
        setTheErrorMsg(T("找不到该组管理配置!"));
        goto out;
    }
    tmp2=data;
    while(1)
    {
        if( (*tmp1!=UTT_SPLIT_SIGN_CHAR) && (*tmp1!=UTT_SPLIT_SIGN_RECORD) && *tmp1 ) 
            *tmp2++=*tmp1++;
        else
        {
            ct++;
			tmp1++;
			if(ct==1) websWrite(wp, GroupName, data);
            else if(ct==2) {
			    sscanf(data,"%x",&d);
                int2ip(d,data);
			    websWrite(wp, FromIP, data);
			}
			else if(ct==3){
				sscanf(data,"%x",&d);
                   int2ip(d,data);
				websWrite(wp, EndIP,  data);
			}
			else if(ct==4) websWrite(wp, policy,  data);
			else if(ct==5) websWrite(wp, dRate,  data);
			else if(ct==6) websWrite(wp, dCeil,  data);
			else if(ct==7) websWrite(wp, uRate,  data);
			else if(ct==8) websWrite(wp, uCeil,  data);
			else if(ct==9) websWrite(wp, DenyQQ,  data);
			else if(ct==10) websWrite(wp, DenyMSN,  data);
			else if(ct==11) websWrite(wp, DenyBT, data);
			else if(ct==12) websWrite(wp, DenyThunder,  data);
			else if(ct==13){
					sscanf(data,"%x",&d);
					Byte2Days(d,data);
					websWrite(wp, group_day, data);
			}
			else if(ct==14) websWrite(wp, group_timestart, data);
			else
			{
				ct=0;
				websWrite(wp, group_timestop,  data);
			}
			tmp2=data;
			memset(tmp2,0,sizeof(data));
			if(ct==0) break;
	   }
    }
out:
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
/***********************************************************************
 * 函 数 名：   coefficient
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static int coefficient(int eid, webs_t wp, int argc, char_t **argv)
{
    websWrite(wp,"var coefficient = %d;",WEB_COEFFICIENT);
    websWrite(wp,"var ip_num = %d;",UTT_MAX_GROUP_IP);
    return 0;
}
/***********************************************************************
 * 函 数 名：   getGroupConfigList
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static int getGroupConfigList(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t data[16]={0},*GroupList = NULL,*newList,*tmpRecord,*tmp,*tmp1,*ptr;
    unsigned int d,num=0;
	int ct=0;
	char_t *GroupName=T("GroupNames[%d] = \"%s\";");
    char_t *FromIP =T("FromIPs[%d] = \"%s\";");
    char_t *EndIP = T("EndIPs[%d] = \"%s\"; ");
    char_t *policy = T("policy[%d] = \"%s\"; ");
    char_t *dRate = T("dRate[%d] = \"%s bit/s\"; ");
    char_t *dCeil = T("dCeil[%d] = \"%s bit/s\"; ");
    char_t *uRate = T("uRate[%d] = \"%s bit/s\"; ");
    char_t *uCeil = T("uCeil[%d] = \"%s bit/s\"; ");
	char_t *DenyQQ = T("DenyQQs[%d] = \"%s\"; ");
	char_t *DenyMSN=T("DenyMSNs[%d] = \"%s\";");
    char_t *DenyBT =T("DenyBTs[%d] = \"%s\";");
    char_t *DenyThunder = T("DenyThunders[%d] = \"%s\"; ");
	char_t *group_day = T("group_days[%d] = \"%s\"; ");
	char_t *group_timestart = T("group_timestarts[%d] = \"%s\"; ");
	char_t *group_timestop = T("group_timestops[%d] = \"%s\"; ");

    websWrite(wp,"var GroupNames=new Array();");
    websWrite(wp,"var FromIPs = new Array();");
    websWrite(wp,"var EndIPs=new Array();");
    websWrite(wp,"var policy=new Array();");
    websWrite(wp,"var dRate=new Array();");
    websWrite(wp,"var dCeil=new Array();");
    websWrite(wp,"var uRate=new Array();");
    websWrite(wp,"var uCeil=new Array();");
    websWrite(wp,"var DenyQQs=new Array();");
    websWrite(wp,"var DenyMSNs=new Array();");
    websWrite(wp,"var DenyBTs=new Array();");
    websWrite(wp,"var DenyThunders = new Array();");
    websWrite(wp,"var group_days=new Array();");
    websWrite(wp,"var group_timestarts=new Array();");
    websWrite(wp,"var group_timestops = new Array();");

    GroupList = nvram_bufget(RT2860_NVRAM,"GroupConfigList");
    DBGPRINT("GroupList=%s\n",GroupList);
#if 1 
    newList=(char*)malloc(strlen(GroupList)+1);
    strcpy(newList, GroupList);
    if(*GroupList)
    {
	tmpRecord = strtok_r(newList, UTT_SPLIT_SIGN_RECORDS,&ptr);
	tmp=data;
	while(tmpRecord!=NULL)
	{
		    tmp1=tmpRecord;
		    //DBGPRINT("record:%s\n",tmpRecord);
		    while(1) //output Username,ip ,mac
		    {
			     if( ( *tmp1!=UTT_SPLIT_SIGN_CHAR ) &&(*tmp1!=UTT_SPLIT_SIGN_RECORD)&& (*tmp1) ) 
			    {
				    *tmp++=*tmp1++;
			    }
			    else
			    {
				    ct++;
				    tmp1++; // jump over the ","
				    if(ct==1) websWrite(wp, GroupName, num, data);//DBGPRINT("2data=%s\n",data);
				    else if(ct==2) {
					    sscanf(data,"%x",&d);
					    int2ip(d,data);
					    websWrite(wp, FromIP, num, data);
				    }
				    else if(ct==3){
					    sscanf(data,"%x",&d);
					    int2ip(d,data);
					    websWrite(wp, EndIP, num, data);
				    }
				    else if(ct==4) websWrite(wp, policy, num, data);
				    else if(ct==5){
					if(!strcmp(data,"nolimit"))
					    strcpy(data,"0");
					websWrite(wp, dRate, num, data);
				    }
				    else if(ct==6){
					if(!strcmp(data,"nolimit"))
					    strcpy(data,"0");
					websWrite(wp, dCeil, num, data);
				    }
				    else if(ct==7){
					if(!strcmp(data,"nolimit"))
					    strcpy(data,"0");
					websWrite(wp, uRate, num, data);
				    }
				    else if(ct==8){
					if(!strcmp(data,"nolimit"))
					    strcpy(data,"0");
					websWrite(wp, uCeil, num, data);
				    }
				    else if(ct==9) websWrite(wp, DenyQQ, num, data);
				    else if(ct==10) websWrite(wp, DenyMSN, num, data);
				    else if(ct==11) websWrite(wp, DenyBT, num, data);
				    else if(ct==12) websWrite(wp, DenyThunder, num, data);
				    else if(ct==13){
					    sscanf(data,"%x",&d);
					    Byte2Days(d,data);
					    websWrite(wp, group_day, num, data);
				    }
				    else if(ct==14) websWrite(wp, group_timestart, num, data);
				    else
				    {
					    ct=0;
					    websWrite(wp, group_timestop, num, data);
				    }
				    tmp=data;
				    memset(tmp,0,sizeof(data));
				    if(ct==0) break;
			    }
		}
	    num++;
	    tmpRecord = strtok_r(NULL, UTT_SPLIT_SIGN_RECORDS,&ptr);
	}
    }
    free(newList);
#endif
    GroupConfigCounter=num;
    websWrite(wp,"var totalrecs=%d;",num);
    websWrite(wp,"var max_totalrecs=%d;",UTT_MAX_GROUP_CONFIG);
    getTheErrorMsg(eid, wp, argc, argv);
    getTheRightMsg(eid, wp, argc, argv);
    return 0;
}
/***********************************************************************
 * 函 数 名：   formConfigGroupConfig
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/

static void formConfigGroupConfig(webs_t wp, char_t *path, char_t *query)
{
    char_t *GroupList,*newList,*GroupName,*GroupNameOld,*FromIp,*EndIp,*policy ,*dRate ,*dCeil ,*uRate ,*uCeil ;
    char_t *DenyQQ,*DenyMSN,*DenyBT,*DenyThunder,*group_day,*group_timestart,*group_timestop;
    char_t *action,*p1,*p2 = 0,c;
	char ip[18]={0};
    unsigned int tempLen,nFromIp,nEndIp;
    char error_msg[25] = {0};
    GroupName=websGetVar(wp,T("GroupName"), T(""));
    FromIp=websGetVar(wp,T("FromIP"), T(""));
    EndIp=websGetVar(wp,T("EndIP"), T(""));
    policy=websGetVar(wp,T("policy"), T(""));
    dRate=websGetVar(wp,T("dRate"), T(""));
    dCeil=websGetVar(wp,T("dCeil"), T(""));
    uRate=websGetVar(wp,T("uRate"), T(""));
    uCeil=websGetVar(wp,T("uCeil"), T(""));
    DenyQQ=websGetVar(wp,T("DenyQQ"), T(""));
    DenyMSN=websGetVar(wp,T("DenyMSN"), T(""));
    DenyBT=websGetVar(wp,T("DenyBT"), T(""));
    DenyThunder=websGetVar(wp,T("DenyThunder"), T(""));
    group_day=websGetVar(wp,T("day"), T(""));
    group_timestart=websGetVar(wp,T("timestart"), T(""));
    group_timestop=websGetVar(wp,T("timestop"), T(""));
    nFromIp=ip2int(FromIp);
    nEndIp=ip2int(EndIp);
    sprintf(ip,"%x,%x",nFromIp,nEndIp);
    action = websGetVar(wp, T("Action"), T("")); 
    GroupList = nvram_bufget(RT2860_NVRAM,"GroupConfigList");
    tempLen=strlen(GroupList)+strlen(GroupName)+64+30;
    if((!strcmp(policy , "1")) && (nEndIp - nFromIp + 1 > UTT_MAX_GROUP_IP))
    {
	sprintf(error_msg ,"组ip个数不能大于%d！",UTT_MAX_GROUP_IP);
	setTheErrorMsg(error_msg);
	goto out;
    }
    if(!strcmp(action,"add"))
    {
        if(GroupConfigCounter >= UTT_MAX_GROUP_CONFIG)
		{
            setTheErrorMsg(T("已达最大数目！"));
			goto out;
        }
		if(strnmatch(GroupList,GroupName,INDEX_GroupName))//没有判断IP地址重复
		{
            setTheErrorMsg(T("组名已经存在!"));
			goto out;
        }
	newList = (char*)malloc(sizeof(char) * tempLen);

        strcpy(newList, GroupList);
	p1=newList+strlen(GroupList);
        if(strlen(newList) != 0){
            strcat(newList, UTT_SPLIT_SIGN_RECORDS);
	    p1++;
	}
    }
    else //modify
    {
        GroupNameOld= websGetVar(wp, T("GroupNameold"), T("")); 
	p1=strnmatch(GroupList,GroupNameOld,INDEX_GroupName);
	if(p1==0)
	{
		 setTheErrorMsg(T("找不到该条组管理配置!"));
		 goto out;
	}
	p2=p1;
	while( (*p2!=UTT_SPLIT_SIGN_RECORD) && *p2)
	{
	  p2++;
	}
	c=*p1;
	*p1=0;
	if(strnmatch(GroupList,GroupName,INDEX_GroupName)||( *p2 &&  strnmatch(p2,GroupName,INDEX_GroupName)))
	{
		setTheErrorMsg(T("组名已经存在！"));
		*p1=c;
		goto out;
	}
	 *p1=c;
         *p1=0;
        newList = (char*)malloc(sizeof(char) * tempLen);
	strcpy(newList,GroupList); 
	p1=newList+strlen(GroupList);
    }
    sprintf(p1,"%s,%x,%x,%s,%s,%s,%s,%s,%s,%s,%s,%s,%x,%s,%s",GroupName,nFromIp,nEndIp,policy ,dRate ,dCeil ,uRate ,uCeil,DenyQQ,DenyMSN,DenyBT,DenyThunder,days2Byte(group_day),group_timestart,group_timestop);
    if(strcmp(action,"add")!=0) strcat(newList,p2);
    nvram_bufset(RT2860_NVRAM,"GroupConfigList",newList);
    nvram_commit(RT2860_NVRAM);
    sendMsgForSpeed(INIT_GROUP, 0, 0, NULL, 0);
    free(newList);
out:
    websRedirect(wp, "Group.asp");
}
/***********************************************************************
 * 函 数 名：   formGroupConfigDel
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void formGroupConfigDel(webs_t wp, char_t *path, char_t *query)
{
    char_t *GroupName=NULL,*GroupList,*p1,*p2,*tmp,*ptr;
    GroupList = nvram_bufget(RT2860_NVRAM,"GroupConfigList");
    GroupName = websGetVar(wp,T("delstr"),T("")); 
	if(!strcmp(GroupName,T("")))
	{   
         goto out;
	}
    tmp=strtok_r(GroupName,UTT_SPLIT_SIGN_STR,&ptr);//可能删除多条映射
	while(tmp!=NULL)
	{
	    p1=strnmatch(GroupList,tmp,INDEX_GroupName);
        if(!p1)
        {
	        setTheErrorMsg(T("找不到该组管理配置!"));
	        goto out;
        }
        
        p2=p1;
        while( (*p2!=UTT_SPLIT_SIGN_RECORD) && *p2)
            {
	        p2++;
            }
	    if(p1!=GroupList) p1--;   //before p1 there is the char';'
	    else if(*p2) p2++;    //if del the first record ,jump over the char ';'
            *p1=0;
	    strcat(GroupList,p2);                            
        tmp=strtok_r(NULL,UTT_SPLIT_SIGN_STR,&ptr);
	}
    nvram_bufset(RT2860_NVRAM,"GroupConfigList",GroupList);
    nvram_commit(RT2860_NVRAM);
    sendMsgForSpeed(INIT_GROUP, 0, 0, NULL, 0);
out:
        websRedirect(wp,"Group.asp");
}
/***********************************************************************
 * 函 数 名：   formGroupConfigDelAll
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/


static void formGroupConfigDelAll(webs_t wp, char_t *path, char_t *query)
{
        nvram_bufset(RT2860_NVRAM, "GroupConfigList", "");
        nvram_commit(RT2860_NVRAM);
        sendMsgForSpeed(INIT_GROUP, 0, 0, NULL, 0);
        GroupConfigCounter = 0;
        websRedirect(wp, "Group.asp");
}
#if 0
/***********************************************************************
 * 函 数 名：   initGroup
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
void initGroup(void);
void initGroup(void)
{
    char_t * GroupList,*tmp,*ptr,*newList;
    //time_t time_begin, time_end;
    GroupList = nvram_bufget(RT2860_NVRAM,"GroupConfigList");
    newList=(char*)malloc(strlen(GroupList)+1);
    strcpy(newList, GroupList);
    //time_begin = time(NULL);
    doSystem("iptables -t mangle -F %s" ,GROUP_LIMIT_CHAIN);
    doSystem("iptables -t mangle -F %s" ,GROUP_CHAIN);
    if(*GroupList)
    {
	tmp=strtok_r(newList,UTT_SPLIT_SIGN_RECORDS,&ptr);
	while(tmp!=NULL)
	{
	    EditOneIptablesGroup(tmp,GROUP_ADD,"","",1);
	    tmp=strtok_r(NULL,UTT_SPLIT_SIGN_RECORDS,&ptr);
	}
    }	
    //time_end = time(NULL);
    //DBGPRINT("time space is %f seconds\n",difftime(time_end ,time_begin));
    free(newList);
}
#endif

/***********************************************************************
 * 函 数 名：   formDefineGroupConfig
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
void formDefineGroupConfig(void)
{
	websFormDefine(T("formGroupConfigDelAll"), formGroupConfigDelAll);
	websFormDefine(T("formGroupConfigDel"), formGroupConfigDel);
	websFormDefine(T("formGroupConfig"),formConfigGroupConfig );
	websAspDefine(T("aspOutOneGroupConfig"), getOneGroupConfig);
	websAspDefine(T("aspOutGroupConfigList"), getGroupConfigList);
	websAspDefine(T("coefficient"), coefficient);
}
