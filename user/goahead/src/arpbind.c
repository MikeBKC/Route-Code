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
#include "profacce.h"


//存放配置参数的查找序号
#define     INDEX_USERNAME    0 
#define     INDEX_IP    1 
#define     INDEX_MAC  2

static void addArpBind(char *ip, char *mac,char *allow);
static void delArpBind(char *ip, char *mac,char *allow);
static void enableArpBindGlobal(char *enable);
static void modifyIptables(char *ip, char *mac, char *allow);

//记录ARPBIND条目
static int ArpBindCounter=0; 
static int ArpReadBindCounter=0; 


/***********************************************************************
 * 函 数 名：   read_arp
 * 功能描述：	根据传入的ip地址查找arp表中的相应的条目，并返回
 * 输入参数:    webIp - 待查找的ip地址
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static char ret[80];
static char * read_arp(char *webIp)
{
    int i=0;
    FILE *fp;
    fp=fopen("/proc/net/arp","r");
    while(1)
    {
	i=0;
	fgets(ret,80,fp);
	while(ret[i]!=' ')
	    i++;
	ret[i]='\0';
	if(feof(fp))
       	    break;
        DBGPRINT("read_arp:ret[%s]\n", ret);
	if(!strcmp(webIp,ret))
	{
	    ret[i]=' ';
	    fclose(fp);
	    return ret;
	}
    }
    fclose(fp);
    return NULL;
}

/***********************************************************************
 * 函 数 名：   isSave
 * 功能描述：	判断进行ARP绑定的IP是否将自己的IP阻挡了，即IP相同MAC不同，
 *		或者IP不同MAC相同，用于警告WEB用户将自己阻挡不能访问路由器。
 * 输入参数：	allowOthers - 是否允许非IP/MAC绑定的用户，"on"允许，其他
 *		                为不允许。
 *		newIpStr    - WEBUI上用户试图增加的ARP绑定的IP
 *		newMacStr   - WEBUI上用户试图增加的ARP绑定的MAC
 *              webIp	    - 用户操作该系统所使用的ip
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
int isSave(const char* allowOthers,const char* newIpStr,const char* newMacStr,char* errMsg,char *webIp)
{
    char *ptr,*ptmp,Mactmp[18]={0},UserMactmp[18]={0},buf[64]={0};
    char *tmptok1,*webmac = 0;
    if (ptmp = read_arp(webIp))
        strncpy(buf, ptmp, 63);
    else /* pppoe-user is allowed all the time */
        return 1;
    char *split=" ";
    tmptok1 = strtok_r(buf,split,&ptr);
    int ct=0;
    while(tmptok1!=NULL)
      {
		  ct++;
		  if(ct==4) {
			  webmac=tmptok1;
			  break;
		  }
	      tmptok1 = strtok_r(NULL, split,&ptr);
	   }
	strcpy(UserMactmp,webmac);
	strcpy(Mactmp,newMacStr);
        Conversion2StdMac(Mactmp);
       my_strlwr(Mactmp);
       my_strlwr(UserMactmp);
    if((!strcmp(newIpStr,webIp)) && (strcmp(UserMactmp ,Mactmp)))
    {
	if(errMsg)
	{
	    strcpy(errMsg,T("被操作的MAC地址与所使用的MAC地址不一致"));
	}
	return 0;
    }
    if((strcmp(newIpStr,webIp)) && (!strcmp(UserMactmp ,Mactmp)))
    {
	if(errMsg)
	{
	    strcpy(errMsg,T("被操作的IP地址与所使用的IP地址不一致"));
	}
	return 0;
    }
    return 1;
}

/***********************************************************************
 * 函 数 名：   IsYourselfOK 
 * 功能描述：	判断进行ARP绑定的IP是否将自己的IP阻挡了，即IP相同MAC不同，
 *		用于警告WEB用户将自己阻挡不能访问路由器。
 * 输入参数：	allowOthers   - 是否允许非IP/MAC绑定的用户，"on"允许，其他
 *		                为不允许。
 *		newIp、newMac - WEBUI上用户试图增加的ARP绑定的IP和MAC,没有
 *		                则输入NULL
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
extern int IsYourselfOK(const char* allowOthers,char* newIpStr,const char* newMacStr,char* errMsg,char *webIp)
{
    unsigned int UserIptmp,ipNew;
    int bAllowOthers=1,bIpFind=0;
    char *ptr,*ptmp,*CList,Mactmp[18]={0},UserMactmp[18]={0},buf[64]={0};
    char *p1,*p2,conMac[18]={0};
    if(errMsg)
    {
	    strcpy(errMsg,""); 
    }
    if(!allowOthers || 0 != strcmp("on",allowOthers))
    {
	    bAllowOthers = 0;
    }
    UserIptmp=ip2int(webIp);//WEB进行操作的IP地址


    DBGPRINT("webIp[%s]\n", webIp);
    char *tmptok1,*webmac = 0;
    if (ptmp = read_arp(webIp))
        strncpy(buf, ptmp, 63);
    else /* pppoe-user is allowed all the time */
        return 1;

    char *split=" ";
    tmptok1 = strtok_r(buf,split,&ptr);
    int ct=0;
    while(tmptok1!=NULL)
      {
		  ct++;
		  if(ct==4) {
			  webmac=tmptok1;
			  break;
		  }
	      tmptok1 = strtok_r(NULL, split,&ptr);
    }
	    strcpy(UserMactmp,webmac);

	if(newIpStr)
			ipNew=ip2int(newIpStr);
	else
			ipNew = 0;

	CList = nvram_bufget(RT2860_NVRAM,"ArpBindList");
	p1=strnmatch(CList,webIp,INDEX_IP);
	p2=p1;
	/*查找已经配置的ARP中是否有和WEBip一样的*/
	if(p1)
	{
		sscanf(p2,"%*[^,],%[^,]",conMac);
		strcpy(Mactmp,conMac);
		Conversion2StdMac(Mactmp);
		bIpFind = 1;
	}
	
	if(0 != ipNew && ipNew == UserIptmp)
	{
		strcpy(Mactmp,newMacStr);
        Conversion2StdMac(Mactmp);
		bIpFind = 1;
	}
	   my_strlwr(Mactmp);
	   my_strlwr(UserMactmp);
    if(!bAllowOthers) //only arp-bind user is allowed
    { 
    		if(!bIpFind)
    		{
		    if(errMsg)
		    {
			strcpy(errMsg,T("无法提交：只允许IP/MAC绑定用户，但该操作将使您不在绑定用户列表里，无法登录设备"));
		    }
		    return 0;
    		}
    		else if(0 != strcmp(Mactmp,UserMactmp))
    		{
		    if(errMsg)
		    {
			strcpy(errMsg,T("无法提交：该操作将使您登录的MAC与绑定用户列表里的MAC不一致，无法登录设备"));
		    }
		    return 0;
    		}
    }
    else
    {
    		if(bIpFind && 0 != strcmp(Mactmp,UserMactmp))
    		{
		    if(errMsg)
		    {
			strcpy(errMsg,T("无法提交：该操作将使您登录的MAC与绑定用户列表里的MAC不一致，无法登录设备"));
		    }
		    return 0;
    		}
			else if(!bIpFind && 0 == strcmp(Mactmp,UserMactmp)){
				if(errMsg)
					{
					strcpy(errMsg,T("无法提交：该操作将使您登录的IP与绑定用户列表里的IP不一致，无法登录设备"));
					}
    			return 0;
			}
    }
  	return 1;
}

/***********************************************************************
 * 函 数 名：   getArpBindGlobalConfig
 * 功能描述：   向页面输出全局配置中的参数信息（允许非ARP绑定用户）
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static int getArpBindGlobalConfig(int eid, webs_t wp, int argc, char_t **argv)
{
    websWrite(wp,"var AllowOtherEnables=\"%s\";",nvram_bufget(RT2860_NVRAM,"ARPBIND_ALLOWENABLE"));
    getTheErrorMsg(eid, wp, argc, argv);  
    return 0;
}

/***********************************************************************
 * 函 数 名：   getArpBindList
 * 功能描述：   向页面输出ARP绑定配置信息
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static int getArpBindList(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t *tmp1 = NULL, *cList = NULL ,*newList ,*tmp2,data[18]={0},*tmp3;
    int  num = 0;
    char ct = 0;
	char *ptr;
 
    char_t *UserNames=T("UserNames[%d] = \"%s\";\n");
    char_t *ips =T("arp_IPs[%d] = \"%s\";\n");
    char_t *macs = T("arp_Macs[%d] = \"%s\"; \n");
	char_t *allows = T("Allows[%d] = \"%s\"; \n");
    websWrite(wp,"var UserNames=new Array();\n");
    websWrite(wp,"var arp_IPs = new Array();\n");
    websWrite(wp,"var arp_Macs = new Array();\n");
	websWrite(wp,"var Allows = new Array();\n");
    cList = nvram_bufget(RT2860_NVRAM,"ArpBindList");
#if 0
    DBGPRINT("cList=%s\n",cList);
#endif
    if(!cList||!strlen(cList))
	goto out;
    newList=(char*)malloc(strlen(cList)+1);
    strcpy(newList, cList);
    tmp1 = strtok_r(newList, UTT_SPLIT_SIGN_RECORDS,&ptr);//strtok_r();
    tmp3=data;
    while(tmp1!=NULL)
    {
	tmp2=tmp1;
        while(1) //output Username,ip ,mac
	{
		 if( ( *tmp2!=UTT_SPLIT_SIGN_CHAR ) && (*tmp2) ) 
		{
			*tmp3++=*tmp2++;
		}
		else
		{
			ct++;
			tmp2++; // jump over the ","
			if(ct==1) websWrite(wp, UserNames, num, data);
                        else if(ct==2) websWrite(wp, ips, num, data);
			else if(ct==3) 
			{
			    Conversion2StdMac(data);
			    websWrite(wp, macs, num, data);
			}
			else
			{
				ct=0;
				websWrite(wp, allows, num, data);
			}
			tmp3=data;
			memset(tmp3,0,sizeof(data));
			if(ct==0) break;
		}
	}
        num++;
        if(num >=UTT_MAX_ARP_BIND)
            break;
        tmp1 = strtok_r(NULL, UTT_SPLIT_SIGN_RECORDS,&ptr);
    }
    free(newList);
out:
    ArpBindCounter = num;
    websWrite(wp,"var totalrecs=%d;",num);
    return 0;
}

/***********************************************************************
 * 函 数 名：   getOneArpBindConfig
 * 功能描述：   用于修改时向页面输出所修改的ARP绑定配置信息
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static int getOneArpBindConfig(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t *edit_name,*CList,*tmp2,data[16]={0};
    char *tmp1;
    int ct=0;
    if(ejArgs(argc,argv,T("%s"), &edit_name) <1) 
    {
	setTheErrorMsg(T("读取配置实例名失败!"));
        goto out;
    }
    if(!edit_name)
    {
        setTheErrorMsg(T("用户名不能为空!"));
        goto out;
    }
    CList = nvram_bufget(RT2860_NVRAM,"ArpBindList");
    tmp1=strnmatch(CList,edit_name,INDEX_USERNAME);
    if(!tmp1)
    {
        setTheErrorMsg(T("找不到该用户!"));
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
	     tmp1++;  //jump over the ","
	     if(ct==1) websWrite(wp, "var UserNames=\"%s\";",data);
             else if(ct==2)  websWrite(wp, "var arp_IPs=\"%s\";",data);
	     else if(ct==3)  websWrite(wp, "var arp_Macs=\"%s\";",data);
	     else
             {
		websWrite(wp, "var Allows=\"%s\";",data);
	     }
	     tmp2=data;
	     memset(tmp2,0,sizeof(data));
	     if(ct>3) break;
	}
    }
out:
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
char old_en[3];

/***********************************************************************
 * 函 数 名：   formConfigArpBindGlobalConfig
 * 功能描述：   保存全局配置
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void formConfigArpBindGlobalConfig(webs_t wp, char_t *path, char_t *query)
{
    char_t *allowEnable;
	char errMsg[128];
    a_assert(wp);
	
    allowEnable = websGetVar(wp, T("AllowOtherEnable"), T(""));
	if(!IsYourselfOK(allowEnable,NULL,NULL,errMsg,wp->ipaddr))
	{
		setTheErrorMsg(errMsg);
		goto out;
	}
    nvram_bufset(RT2860_NVRAM,"ARPBIND_ALLOWENABLE", allowEnable);
    if(strcmp(old_en ,allowEnable)!=0)
	enableArpBindGlobal(allowEnable);//全局生效
    strcpy(old_en ,allowEnable);
    nvram_commit(RT2860_NVRAM);
	out:
    websRedirect(wp, "IPMac.asp");
}

/*
 * Data:            2011-06 
 * Author:          malei
 * */
#define arping_ip_num  254
#define arping_data_dir  "/tmp/arp"
#define order_data_dir   "/tmp/order"
char device[]=" br0 ";

/*
 * FunctionName:    reverseStr
 * Function:        reverse the string
 * Data:            2011-06
 * Author:          malei
 * */
static char * reverseStr(char *str)
{
    int end=strlen(str)-1,start=0;
    char ch;
    while(start<end)
    {
        ch=str[start];
        str[start]=str[end];
        str[end]=ch;
        start++;
        end--;
    }
    return str;
}

/*
 * FunctionName:    convIntToString
 * Function:        convert int to string
 * Data:            2011-06
 * Author:          malei
 * */
static char * convIntToString(int i,char *str)
{
    int a=0;
    while(i/10)
    {
        str[a++]=i%10+'0';
        i=i/10;
    }
    str[a++]=i+'0';
    str[a]='\0';
    reverseStr(str);
    return str;
}

/*
 * FunctionName:    convStringToInt
 * Function:        convert string to int
 * Data:            2011-06
 * Author:          malei
 * */
static int convStringToInt(const char * str)
{
    int end=strlen(str),start=0,i=0;
    while(start<end)
    {
        i=i*10+str[start++]-'0';
    }
    return i;

}

/*
 * FunctionName:    myarping
 * Function:        scan all the ip address by using arping
 * Data:            2011-06
 * Author:          malei
 * */
static int myarping(char *ipaddr)
{
    char s[20];
    strcpy(s,ipaddr);
    char ip[20];
    char *a[3];
    char delim[]=".";
    a[0]=strtok(s,delim);
    a[1]=strtok(NULL,delim);
    a[2]=strtok(NULL,delim);

    int i;
    ip[0]='\0';
    for(i=0;i<3;i++)
    {
        DBGPRINT("%s\n",a[i]);
        strcpy(ip+strlen(ip),a[i]);
        strcpy(ip+strlen(ip),".");
    }
    char preIp[20];
    strcpy(preIp,ip);
    char str[4];
    int len=strlen(ip);

    char tmporder[50];
    for(i=0;i<arping_ip_num;i++)
    {
        convIntToString(i+1,str);
        strcpy(ip+len,str);
        doSystem("arping -I %s %s -c 1 &",device,ip);
    }

    sleep(3);
    
    DBGPRINT("all arping order run over.\n");
   
    doSystem("cat /proc/net/arp | grep %s | grep %s > %s",device,preIp,arping_data_dir);
   
    DBGPRINT("finished writing ip and mac data to %s.\n",\
            arping_data_dir);

   return 0;

    //code for exception
out:
    return -1;
}

/*
 * ModuleName:      a simple list 
 * Function:        list 
 * Data:            2011-06 
 * Author:          malei
 * */
typedef struct LNode{
    struct LNode *next;
    char *data;
}LNode,*List;
static void insert_list(List *list,const char * ret)
{
    List p=*list,pre;
    pre=p;
    while(p)
    {
        pre=p;
        p=p->next;
    }
    p=(List)malloc(sizeof(LNode));
    if(NULL==p)
    {
        DBGPRINT("malloc error.\n");
        goto out;
   }
    p->next=NULL;
    p->data=(char*)malloc(sizeof(char)*(strlen(ret)+1));
    if(NULL==p->data)
    {
        DBGPRINT("malloc error.\n");
        goto out;
    }
    strcpy(p->data,ret);
    //DBGPRINT("%s\n",p->data);
    if(NULL==pre)
        *list=p;
    else
        pre->next=p;
    return;

out:
    //code for delete the list  and free memmory
    //code for redirect the page
    return ;
}

/*
 * FunctionName:    deal_arping_data
 * Function:        read arping data 
 * Data:            2011-06 
 * Author:          malei
 * */
static void deal_arping_data(int eid,webs_t wp,int argc,char_t **argv,char *str)
{
   char *s=(char*)malloc(sizeof(char)*(strlen(str)+1));
    if(NULL==s)
    {
        DBGPRINT("malloc s error __LINE__.\n");
        goto out;
    }
    strcpy(s,str);
    char delim[]="\n";
    char delimip[]=" ";
    char delimmac[]=" ";
    char *ret;
    int i=0;
    List list=NULL;
    ret=strtok(s,delim);
    while(NULL!=ret)
    {
        insert_list(&list,ret);
        //DBGPRINT("%d:%s\n",i++,ret);
        //DBGPRINT("%d\n",list);
        ret=strtok(NULL,delim);
    }
    free(s);
    //websWrite(wp,"i_flag[0] = 1;");
	//websWrite(wp, "i_old_ip[0]=\"%s\";\n\n",
	//	  nvram_bufget(RT2860_NVRAM, "ReadArpIPADDR"));

    //check  ip or mac
    char *CList;
    CList = nvram_bufget(RT2860_NVRAM,"ArpBindList");
  

    List pre,p=list;
    char data[100];
    i=0;
    pre=p; 
    char *ip,*mac;
    while(p)
    {
        pre=p;
       
        /// code for dealing with the data
        strcpy(data,p->data);
        DBGPRINT("%d:%s\n",i,p->data);
        //ip
        ret=strtok(data,delimip);
        ip=ret;
        //ip end
        //mac 
        ret=strtok(NULL,delimmac);
        ret=strtok(NULL,delimmac);
        ret=strtok(NULL,delimmac);
        mac=ret;
        //mac end
   
        if(strnmatch(CList,ip,INDEX_IP) \
                ||(strnmatch(CList,my_strlwr(mac),INDEX_MAC))\
                )      
        {
            DBGPRINT("ip or mac which is got by scaned was already in list .\n");
        }
        else
        {
            websWrite(wp,"i_flag[%d]=1;i_ips[%d]=\"%s   \";",i,i,ip);
            websWrite(wp,"i_macs[%d]=\"%s\";\n",i,mac);
            websWrite(wp,"i_types[%d] = \"Dynamic\"; i_intfs[%d] = \"0:ie0 \";i_uses[%d] = 110; i_ages[%d] = 513;\n\n",i,i,i,i);
            i++;
        }

        DBGPRINT("ip:%s %d\n",ip,strlen(ip));
        DBGPRINT("mac:%s %d\n",mac,strlen(mac));
        
        p=p->next;
        free(pre->data);
        free(pre);
    } 
    list=NULL; 
    pre=NULL; 

    doSystem("rm -f %s",arping_data_dir);

    websWrite(wp,"i_totalrecs = %d;\n\n",i);
  //  websWrite(wp,"totalrecs = %d;\n\n",i);
out:
    return ;

}

/*
 * FunctionName:    readarping
 * Function:        read arping data 
 * Data:            2011-06 
 * Author:          malei
 * */
static void readarping(int eid,webs_t wp,\
        int argc,char_t *argv,char * dir)
{
        FILE *fp;
    long lSize;
    char *s;
    size_t result;
    fp=fopen(dir,"r");
    if(NULL==fp)
    {
        fprintf(stderr,"fopen %s error.\n",dir);
        goto out1;
    }
    fseek(fp,0,SEEK_END);
    lSize=ftell(fp);
    rewind(fp);
    s=(char*)malloc(sizeof(char)*(lSize+1));
    if(s==NULL)
    {
        fprintf(stderr,"Memory error.\n");
        goto out2;
    }
    result=fread(s,1,lSize,fp);
    if(result!=lSize)
    {
        fprintf(stderr,"Reading %s error.\n",dir);
        goto out3;
    }
    s[result]='\0';
    //DBGPRINT("%s%d\n",s,strlen(s));
    deal_arping_data(eid,wp,argc,argv,s);
    fclose(fp);
    free(s);
    return ;

    // code for exception
out1:
    goto out;
out2:
    fclose(fp);
    goto out;
out3:
    fclose(fp);
    free(s);
out:
    return ;
}

/*
 * FunctionName:    formReadArp
 * Function:        scan the mac addr  by arping
 * Data:            2011-06 
 * Author:          malei
 * */
static void formReadArp(webs_t wp,char_t *path,char_t *query)
{
   char *ipaddr;
   ipaddr=websGetVar(wp,T("ipAddr"), T(""));
   DBGPRINT("ipaddr is %s.\n",ipaddr);
   nvram_bufset(RT2860_NVRAM, "ReadArpIPADDR", ipaddr);
   nvram_commit(RT2860_NVRAM);
   //ipaddr="192.168.1.5";
   int state=myarping(ipaddr);

   //code for exception
   if(state==-1)
       goto out;

   DBGPRINT("after myarping module.\n");

out:
   websRedirect(wp, "IPMacConfig.asp");
   return ;

}

/*
 * FunctionName:    aspOutReadArp
 * Function:        output the mac addr which is got by formReadArp
 * Data:            2011-06 
 * Author:          malei
 * */
static int aspOutReadArp(int eid,webs_t wp,int argc,char_t **argv)
{
   char lan_addr[16];
   if (-1 == getIfIp(getLanIfName(), lan_addr)) {
       lan_addr[0]='\0';
   }
   websWrite(wp,"i_old_ip[0] = \"%s\";\n",lan_addr);
   websWrite(wp,"var num =  %d;\n",50-ArpBindCounter); 
   websWrite(wp,"var totalrecs= %d;\n",ArpBindCounter); 
   websWrite(wp,"var maxtotalrecs = %d;\n",50);
   //var totalrecs = 40; //用于判断已经绑定了多少条数
   //var maxtotalrecs = 50; //总计能够绑定多少条数
   readarping(eid,wp,argc,argv,arping_data_dir);
   DBGPRINT("after readarping module.\n"); 
   getTheErrorMsg(eid, wp, argc, argv);  
out:
   // websRedirect(wp, "IPMacConfig.asp");
   return 0;
}

static void addReadArpBind(char *ip, char *mac,char *allow)
{
    char tmp[18]={0},arpAction[]=T("ACCEPT");
    strcpy(tmp,mac);
    Conversion2StdMac(tmp);
    if(!strcmp(allow,"no"))
	    strcpy(arpAction,"DROP");
  

    char order[200];
	sprintf(order,"arptables -I INPUT -s %s --source-mac %s -j %s -i %s",ip,tmp,arpAction,getLanIfName());
    doSystem("echo %s >> %s",order,order_data_dir);
	sprintf(order,"arptables -A INPUT -s %s  -j DROP -i %s",ip,getLanIfName());
    doSystem("echo %s >> %s",order,order_data_dir);
	sprintf(order,"arptables -A INPUT --source-mac %s  -j DROP -i %s",tmp,getLanIfName());
    doSystem("echo %s >> %s",order,order_data_dir);

}
/*
 * FunctionName:    addReadArp
 * Function:        add the ip amd mac address which scanned by arping
 * Data:            2011-06 
 * Author:          malei
 * */
static int addReadArp(webs_t wp, char_t *path, char_t *query,char *UserName,char *ip,char *mac)
{
    int i;
    //check ip
    char errIp[50];
    if(strcmp(ip,"0.0.0.0")==0 || strcmp(ip,"127.0.0.1")==0\
            || strncmp(ip+strlen(ip)-3,"255",3)==0\
            || strncmp(ip,"255",3)==0\
            || strncmp(ip,"255.255",7)==0\
            || strncmp(ip,"255.255.255",11)==0)
    {
        strcpy(errIp,ip);
        strcpy(errIp+strlen(errIp),"绑定的IP地址不合法!");
		setTheErrorMsg(errIp);
        goto out;
    }
    //check mac
    for(i=0;i<12;i++)
    {
        if(mac[i]>=0x41 && mac[i]<=0x5A)
            mac[i]+=0x20;
    }
    char errMac[50];
    if(strcmp(mac,"000000000000")==0 || strcmp(mac,"ffffffffffff")==0 )
    {
        strcpy(errMac,mac);
        strcpy(errMac+strlen(errMac),"绑定的MAC地址不合法!");
		setTheErrorMsg(errMac);
        goto out;
    }
    //check the length of UserName
    char errUserName[50];
    if(strlen(UserName)>10)
    {
        strcpy(errUserName,UserName);
        strcpy(errUserName+strlen(errUserName),"用户名太长");
		setTheErrorMsg(errUserName);
		goto out;
    }
    //init local variable
    char *tolower_mac,*allow,*CList,*newCList;
    int tempLen;
    allow=T("yes");
	tolower_mac=mac;
    CList = nvram_bufget(RT2860_NVRAM,"ArpBindList");
    DBGPRINT("CList=%s\n",CList);
    tempLen=strlen(CList)+strlen(UserName)+strlen(ip)+strlen(mac)+8;
  
    //check ip and mac
	char *OtherUser;
	char errMsg[128];
	char errMsg1[128];
    OtherUser= nvram_bufget(RT2860_NVRAM,"ARPBIND_ALLOWENABLE");
	if(!IsYourselfOK(OtherUser,ip,mac,errMsg,wp->ipaddr)){
		setTheErrorMsg(errMsg);
		goto out;
	}
    if(!isSave(OtherUser,ip,mac,errMsg1,wp->ipaddr)){
		setTheErrorMsg(errMsg1);
		goto out;
	}
   
    //add Username ip mac into sys
    if(ArpBindCounter >= UTT_MAX_ARP_BIND){
        setTheErrorMsg(T("已达最大配置条目，无法添加新配置！"));
        goto out;
    }
    DBGPRINT("CList=%s\n",CList);
    if(strnmatch(CList,UserName,INDEX_USERNAME)||strnmatch(CList,ip,INDEX_IP) ||(strnmatch(CList,my_strlwr(tolower_mac),INDEX_MAC))){
        setTheErrorMsg(T("输入重复!"));
        goto out;
    }
    DBGPRINT("CList=%s\n",CList);
    newCList = (char*)malloc(sizeof(char) * tempLen);
    strcpy(newCList, CList);
    if(strlen(newCList) != 0)
        strcat(newCList, UTT_SPLIT_SIGN_RECORDS);
    strcat(newCList,UserName);
    strcat(newCList,UTT_SPLIT_SIGN_STR);
    strcat(newCList,ip); 
    strcat(newCList,UTT_SPLIT_SIGN_STR);
    strcat(newCList,mac);
    strcat(newCList,UTT_SPLIT_SIGN_STR);
    strcat(newCList,allow);
    addReadArpBind(ip,mac,allow);

    //save the data to hd
    nvram_bufset(RT2860_NVRAM,"ArpBindList", newCList);
    nvram_commit(RT2860_NVRAM);
    free(newCList);
    return 0;

out:
    //code for exception
    return -1;
}

/*
 * FunctionName:    formAddReadArp
 * Function:        add ip and mac 
 * Data:            2011-06 
 * Author:          malei
 * */
static void formAddReadArp(webs_t wp,char_t *path,char_t *query)
{
    char *textarea;
    textarea=websGetVar(wp,T("textarea"), T(""));
    DBGPRINT("textarea is\n%s\n",textarea);
    int itail=0U,ihead=0U;
    for(;textarea[ihead];ihead++){
        if( textarea[ihead]==',' || textarea[ihead]==';'){
            continue;
        }
        if( textarea[ihead]!=' ' || (ihead>0 && ( (textarea[ihead-1] != ' ') && (textarea[ihead-1] != '\n') ) ) ) {
            textarea[itail++]=textarea[ihead];
        }
    }
    textarea[itail]='\0';
    DBGPRINT("textarea[%s],size[%d]\n",textarea,strlen(textarea) );

    char delim[]="\r\n";
    char delimip[]=" \r\n";
    char delimmac[]=" \r\n";
    char delimUserName[]=" \r\n";
    char data[100];
    List list=NULL;
    char *ret;
    int i=0;
    ret=strtok(textarea,delim);
    while(NULL!=ret)
    {
        if(strlen(ret)<20) {
            continue;
        }
        DBGPRINT("ret[%s]\n",ret);
        insert_list(&list,ret);
        ret=strtok(NULL,delim);
    }
    List pre,p=list;
    i=0;
    ArpReadBindCounter=convStringToInt(nvram_bufget(RT2860_NVRAM, "ArpReadBindCounter"));
    pre=p; 
    char *ip,*mac,username[11],*pUserName;
    char sys[]="sys.";
    int len=strlen(sys);
    strcpy(username,sys);
    char str[10];
    while(p)
    {
        pre=p;
        
        /// code for dealing with the data
        strcpy(data,p->data);
        DBGPRINT("%d:%s\n",i,data);
        ip=strtok(data,delimip);
        DBGPRINT("ip=%s %d\n",ip,strlen(ip));
        mac=strtok(NULL,delimmac);
        DBGPRINT("mac=%s %d\n",mac,strlen(mac));
        pUserName=strtok(NULL,delimUserName);
        if(NULL==pUserName)
        {
            DBGPRINT("Username is null.\n");
        }
        else
        {
            DBGPRINT("[username is not null ,which is [%s]%d]\n",pUserName,strlen(pUserName));
            DBGPRINT("[username is not null ,which is [%x]%d]\n",pUserName[0],strlen(pUserName));
        }
        if(NULL==pUserName)
        {
            convIntToString(1+ArpReadBindCounter,str);
            strcpy(username,sys);
            strcpy(username+len,str);
            ArpReadBindCounter++;
        }
        else
            strcpy(username,pUserName);
        DBGPRINT("username=%s %d\n",username,strlen(username));
        
        addReadArp(wp,path,query,username,ip,mac);//)
con:
        p=p->next;
        free(pre->data);
        free(pre);
        i++;
    } 
    convIntToString(ArpReadBindCounter,str);
    nvram_bufset(RT2860_NVRAM, "ArpReadBindCounter",str);
    nvram_commit(RT2860_NVRAM);
    list=NULL;
    pre=NULL;


out:
    websRedirect(wp, "IPMac.asp");

    doSystem("cat %s",order_data_dir);
    doSystem("chmod +x %s",order_data_dir);
    doSystem("%s &",order_data_dir);
    doSystem("rm -rf %s",order_data_dir);

    return ;
}

/***********************************************************************
 * 函 数 名：   formConfigArpBindConfig
 * 功能描述：   添加或修改ARP绑定配置
 * 创建日期：	2010-3-17
 * 修改日期：
 
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void formConfigArpBindConfig(webs_t wp, char_t *path, char_t *query)
{
    char *UserName,*UserNameOld,*ip,*mac,*tolower_mac,*allow,*action,*CList,*newCList,*p1,*p2,c;
    char *ipold,*macold;
    int tempLen;
    allow=T("yes");
    UserName=websGetVar(wp,T("UserName"), T(""));
    ip=websGetVar(wp,T("IP"), T(""));
    mac=websGetVar(wp,T("Mac"), T(""));
	tolower_mac=mac;
    action = websGetVar(wp, T("Action"), T("")); 
	//获取所有ARP绑定配置字段
    CList = nvram_bufget(RT2860_NVRAM,"ArpBindList");
    DBGPRINT("CList=%s\n",CList);
    tempLen=strlen(CList)+strlen(UserName)+strlen(ip)+strlen(mac)+8;
   
	char *OtherUser;
	char errMsg[128];
	char errMsg1[128];
    OtherUser= nvram_bufget(RT2860_NVRAM,"ARPBIND_ALLOWENABLE");
	if(!IsYourselfOK(OtherUser,ip,mac,errMsg,wp->ipaddr)){
		setTheErrorMsg(errMsg);
		goto out;
	}
    if(!isSave(OtherUser,ip,mac,errMsg1,wp->ipaddr)){
		setTheErrorMsg(errMsg1);
		goto out;
	}
    if(!strcmp(action, "add"))//新建
    {
        if(ArpBindCounter >= UTT_MAX_ARP_BIND)
	    {
            setTheErrorMsg(T("已达最大配置条目，无法添加新配置！"));
            goto out;
        }
	    DBGPRINT("CList=%s\n",CList);
        if(strncmp(UserName,"sys.",4)==0)
        {
            setTheErrorMsg(T("用户名不能以sys.开头!"));
	        goto out;
        }
	    if(strnmatch(CList,UserName,INDEX_USERNAME)||strnmatch(CList,ip,INDEX_IP) ||(strnmatch(CList,my_strlwr(tolower_mac),INDEX_MAC)))
	    {
            setTheErrorMsg(T("输入重复!"));
	        goto out;
        }
	    DBGPRINT("CList=%s\n",CList);
        newCList = (char*)malloc(sizeof(char) * tempLen);
        strcpy(newCList, CList);
        if(strlen(newCList) != 0)//如果已经有配置存在，添加分割符
            strcat(newCList, UTT_SPLIT_SIGN_RECORDS);
        strcat(newCList,UserName);
        strcat(newCList,UTT_SPLIT_SIGN_STR);
	    strcat(newCList,ip); 
        strcat(newCList,UTT_SPLIT_SIGN_STR);
	    strcat(newCList,mac);
	    strcat(newCList,UTT_SPLIT_SIGN_STR);
	    strcat(newCList,allow);
	    addArpBind(ip,mac,allow);//配置生效
    }
    else //修改
    {
        UserNameOld = websGetVar(wp, T("UserNameold"), T("")); 
	    ipold = websGetVar(wp, T("IPold"), T("")); 
	    macold = websGetVar(wp, T("Macold"), T(""));
	    allow = websGetVar(wp, T("Allow"), T("")); 
	    p1=strnmatch(CList,UserNameOld,INDEX_USERNAME);
	    if(p1==0)
	    {
	        setTheErrorMsg(T("该配置不存在 !"));
	        goto out;
	    }
	    p2=p1;
	    while( (*p2!=UTT_SPLIT_SIGN_RECORD) && *p2)
	    {
	        p2++;
	    }
	    c=*p1;
	    *p1=0;//将配置列表从所要修改的配置开始归0
	    if(strnmatch(CList,UserName,INDEX_USERNAME) || strnmatch(CList,ip,INDEX_IP) || strnmatch(CList,my_strlwr(tolower_mac),INDEX_MAC)
	    || (*p2 && (strnmatch(p2,UserName,INDEX_USERNAME) || strnmatch(p2,ip,INDEX_IP) || strnmatch(p2,my_strlwr(tolower_mac),INDEX_MAC)) ))
	    {
            setTheErrorMsg(T("输入重复!"));
	        *p1=c;
            goto out;
	    }
        newCList = (char*)malloc(sizeof(char) * tempLen);
	    strcpy(newCList,CList);            //copy the string before UserNameOld
        strcat(newCList,UserName);
        strcat(newCList,UTT_SPLIT_SIGN_STR);
	    strcat(newCList,ip);
        strcat(newCList,UTT_SPLIT_SIGN_STR);
        strcat(newCList,mac);
	    strcat(newCList,UTT_SPLIT_SIGN_STR);
        strcat(newCList,allow);
	    strcat(newCList,p2);                             
        //copy the string after UserNameOld record
	    delArpBind(ipold,macold,allow);//删除之前的arp -d 或是iptables -D
	    addArpBind(ip,mac,allow);//添加arp -s 或是iptables -I
    }
    nvram_bufset(RT2860_NVRAM,"ArpBindList", newCList);
    nvram_commit(RT2860_NVRAM);
    free(newCList);
out:
    websRedirect(wp, "IPMac.asp");
}

/***********************************************************************
 * 函 数 名：   enableArpBindGlobal
 * 功能描述：   全局配置生效函数
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void enableArpBindGlobal(char *enable)
{
#if 0
	if(!strcmp(enable,"on")){
		doSystem("arptables -D INPUT -j DROP -i %s 2>dev/null",getLanIfName());
	}
	else{
		doSystem("arptables -A INPUT -j DROP -i %s",getLanIfName());
	}
#endif
}

/***********************************************************************
 * 函 数 名：   addArpBind
 * 功能描述：   添加ARP绑定配置时系统添加一条arp -s配置
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void addArpBind(char *ip, char *mac,char *allow)
{
    char tmp[18]={0},arpAction[]=T("ACCEPT");
	    strcpy(tmp,mac);
        Conversion2StdMac(tmp);
	    if(!strcmp(allow,"no"))
		    strcpy(arpAction,"DROP");

		doSystem("arptables -I INPUT -s %s --source-mac %s -j %s -i %s",ip,tmp,arpAction,getLanIfName());
		doSystem("arptables -A INPUT -s %s  -j DROP -i %s",ip,getLanIfName());
		doSystem("arptables -A INPUT --source-mac %s  -j DROP -i %s",tmp,getLanIfName());
}

/***********************************************************************
 * 函 数 名：   delArpBind
 * 功能描述：   删除或修改ARP绑定配置时系统所做的配置
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void delArpBind(char *ip, char *mac, char *allow)
{
	char tmp[18]={0},arpAction[]=T("ACCEPT");
	strcpy(tmp,mac);
    Conversion2StdMac(tmp);
	if(!strcmp(allow,"no"))
	    strcpy(arpAction,"DROP");
	doSystem("arptables -D INPUT -s %s --source-mac %s -j %s -i %s",ip,tmp,arpAction,getLanIfName());
	doSystem("arptables -D INPUT -s %s  -j DROP -i %s",ip,getLanIfName());
	doSystem("arptables -D INPUT --source-mac %s  -j DROP -i %s",tmp,getLanIfName());
}

/***********************************************************************
 * 函 数 名：   modifyIptables
 * 功能描述：   列表中对允许列进行操作时生效函数
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void modifyIptables(char *ip, char *mac, char *allow)
{
	char tmp[18]={0},delAction[7]=T("DROP"),addAction[7]=T("ACCEPT");
	strcpy(tmp,mac);
    Conversion2StdMac(tmp);
	if(!strcmp(allow,"no")){
		strcpy(delAction,"ACCEPT");
		strcpy(addAction,"DROP");
	}
	doSystem("arptables -D INPUT -s %s --source-mac %s -j %s -i %s",ip,tmp,delAction,getLanIfName());
	doSystem("arptables -I INPUT -s %s --source-mac %s -j %s -i %s",ip,tmp,addAction,getLanIfName());
}

/***********************************************************************
 * 函 数 名：   delAllArpBind
 * 功能描述：   列表删除所有配置进行操作时生效函数
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void delAllArpBind(void)
{
    char_t *instList,ip[16]={0},mac[18]={0},allow[4]={0},*tmp;
    char *ptr;
	
    instList = nvram_bufget(RT2860_NVRAM,"ArpBindList");
    tmp=strtok_r(instList,UTT_SPLIT_SIGN_RECORDS,&ptr);
    while(tmp!=NULL)
    {
        if(sscanf(tmp,"%*[^,],%[^,],%[^,],%[^;]",ip,mac,allow)==3)//有问题
	{
	     delArpBind(ip,mac,allow);
	}
        tmp=strtok_r(NULL,UTT_SPLIT_SIGN_RECORDS,&ptr);
    }
}

/***********************************************************************
 * 函 数 名：   init_ArpBind
 * 功能描述：   arp bind 初始化函数
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
void init_ArpBind(void)
{
    char_t *allowEnable,*instList,ip[16]={0},mac[18]={0},allow[4]={0},*tmp;
    char *ptr;
    char *tmp_list;
	doSystem("arptables -F");

    instList = nvram_bufget(RT2860_NVRAM,"ArpBindList");
    
    if(!instList||!strlen(instList))
	goto out;
    tmp_list = (char *)malloc(strlen(instList)+1);
    strcpy(tmp_list ,instList);
    tmp=strtok_r(tmp_list,UTT_SPLIT_SIGN_RECORDS,&ptr);
    while(tmp!=NULL)
    {
         if(sscanf(tmp,"%*[^,],%[^,],%[^,],%[^;]",ip,mac,allow)==3)
	 {
		addArpBind(ip,mac,allow);
	 }
        tmp=strtok_r(NULL,UTT_SPLIT_SIGN_RECORDS,&ptr);
    }
    free(tmp_list);
out:
    allowEnable=nvram_bufget(RT2860_NVRAM,"ARPBIND_ALLOWENABLE");
    enableArpBindGlobal(allowEnable);
    strcpy(old_en ,nvram_bufget(RT2860_NVRAM,"ARPBIND_ALLOWENABLE"));
}

/***********************************************************************
 * 函 数 名：   formConfigArpBindAllow
 * 功能描述：   允许，禁止ARP绑定用户上网
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void formConfigArpBindAllow(webs_t wp, char_t *path, char_t *query)
{
    char_t *UserName,*ip,*mac,*allow,*CList,*newCList,*p1,*p2,c;
    int tempLen;

    UserName=websGetVar(wp,T("AllowID"), T(""));
    ip=websGetVar(wp,T("AllowIP"), T(""));
    mac=websGetVar(wp,T("AllowMAC"), T(""));
    allow=websGetVar(wp,T("Allow"), T(""));
    CList = nvram_bufget(RT2860_NVRAM,"ArpBindList");
    tempLen=strlen(CList)+strlen(UserName)+strlen(ip)+strlen(mac)+8;
    p1=strnmatch(CList,UserName,INDEX_USERNAME);
    if(p1==0)
    {
        setTheErrorMsg(T("该配置不存在 !"));
	goto out;
    }
	if (0 == strcmp("no",allow)){
		unsigned int UserIptmp,ipNew;
		UserIptmp=ip2int(wp->ipaddr);//WEB进行操作的IP地址
		ipNew=ip2int(ip);
		if(UserIptmp==ipNew){
			setTheErrorMsg(T("无法提交：该操作将使您无法登录设备 !"));
	        goto out;	
		}
	}
    p2=p1;
    while( (*p2!=UTT_SPLIT_SIGN_RECORD) && *p2)
    {
	    p2++;
    }
    c=*p1;
    *p1=0;
  
    newCList = (char*)malloc(sizeof(char) * tempLen);
    strcpy(newCList,CList);           
    strcat(newCList,UserName);
    strcat(newCList,UTT_SPLIT_SIGN_STR);
    strcat(newCList,ip);
    strcat(newCList,UTT_SPLIT_SIGN_STR);
    strcat(newCList,mac);
    strcat(newCList,UTT_SPLIT_SIGN_STR);
    strcat(newCList,allow);

    strcat(newCList,p2);                             
    nvram_bufset(RT2860_NVRAM,"ArpBindList",newCList);
    nvram_commit(RT2860_NVRAM);
    free(newCList);
    modifyIptables(ip,mac,allow);
out:
    websRedirect(wp, "IPMac.asp");
}

/***********************************************************************
 * 函 数 名：   formArpBindDel
 * 功能描述：	删除一条或多条记录
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void formArpBindDel(webs_t wp, char_t *path, char_t *query)
{
        char_t *UserName=NULL,*CList,*p1,*p2,*tmp;
	char_t *tmp1,*tmp2,data[32]={0},ip[16]={0},mac[18]={0},allow[4]={0};
	char *ptr;
	int ct=0;
        a_assert(wp);
        CList = nvram_bufget(RT2860_NVRAM,"ArpBindList");
        UserName = websGetVar(wp,T("delstr"),T("")); 
	if(!UserName)
	{   
	    setTheErrorMsg(T("找不到该用户名!\n"));
            goto out;
	}
        tmp=strtok_r(UserName,UTT_SPLIT_SIGN_STR,&ptr);//可能删除多条记录
	while(tmp!=NULL)
	{
	    p1=strnmatch(CList,tmp,INDEX_USERNAME);
            if(!p1)
            {
	        setTheErrorMsg(T("找不到该用户名!\n"));
	        goto out;
            }
            tmp1=p1;
	    tmp2=data;
	    while(1)
	    {
	        if( (*tmp1!=UTT_SPLIT_SIGN_CHAR) && (*tmp1!=UTT_SPLIT_SIGN_RECORD) && *tmp1 ) 
	            *tmp2++=*tmp1++;
	        else
		{
		   ct++;
		   tmp1++; 
		   if(ct==1){}
		   else if(ct==2)  strcpy(ip,data);
		   else if(ct==3) strcpy(mac,data);
		   else
		   {
			strcpy(allow,data);
		   }
		   tmp2=data;
		   memset(tmp2,0,sizeof(data));
		   if(ct>3)
		   {
		       ct=0;
		       break;
		   }
		}
	    }
		char *OtherUser;
        OtherUser= nvram_bufget(RT2860_NVRAM,"ARPBIND_ALLOWENABLE");
	    if(0!=strcmp(OtherUser,"on")){
		unsigned int UserIptmp,ipNew;
	    UserIptmp=ip2int(wp->ipaddr);//WEB进行操作的IP地址
	    ipNew=ip2int(ip);
	    if(UserIptmp==ipNew){
		    setTheErrorMsg(T("无法提交：该操作将使您无法登录设备 !"));
	        goto out;	
	    }
	}
            p2=p1;

            while( (*p2!=UTT_SPLIT_SIGN_RECORD) && *p2)
            {
	        p2++;
            }
	    if(p1!=CList) p1--;   //before p1 there is the char';'
	    else if(*p2) p2++;    //if del the first record ,jump over the char ';'
            *p1=0;
	    strcat(CList,p2);                             
            ArpBindCounter-- ;
            delArpBind(ip,mac,allow);
            tmp=strtok_r(NULL,UTT_SPLIT_SIGN_STR,&ptr);
	}
        nvram_bufset(RT2860_NVRAM, "ArpBindList",CList);
        nvram_commit(RT2860_NVRAM);
out:
        websRedirect(wp,"IPMac.asp");
}

/***********************************************************************
 * 函 数 名：   formArpBindDelAll
 * 功能描述：	删除所有记录
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void formArpBindDelAll(webs_t wp, char_t *path, char_t *query)
{
		char *OtherUser;
        OtherUser= nvram_bufget(RT2860_NVRAM,"ARPBIND_ALLOWENABLE");
	    if(0!=strcmp(OtherUser,"on")){
			setTheErrorMsg(T("无法提交：该操作将使您无法登录设备 !"));
	        goto out;
		}
	    delAllArpBind();
        nvram_bufset(RT2860_NVRAM, "ArpBindList", "");
        ArpBindCounter = 0;
        nvram_commit(RT2860_NVRAM);
out:
        websRedirect(wp, "IPMac.asp");
}

/***********************************************************************
 * 函 数 名：   formDefineArpBindConfig
 * 功能描述：	把网页上调用的asp函数转换成该文件中定义的函数
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
void formDefineArpBindConfig(void)
{
    websFormDefine(T("formArpBindGlobalConfig"), formConfigArpBindGlobalConfig);
	websFormDefine(T("formArpBindConfig"), formConfigArpBindConfig);
	websFormDefine(T("formReadArp"), formReadArp);
	websFormDefine(T("formAddReadArp"),formAddReadArp);
	websFormDefine(T("formArpBindDelAll"), formArpBindDelAll);
	websFormDefine(T("formArpBindDel"), formArpBindDel);
	websFormDefine(T("formArpBindAllow"), formConfigArpBindAllow);
	websAspDefine(T("aspOutOneArpBindConfig"), getOneArpBindConfig);
	websAspDefine(T("aspOutArpBindList"), getArpBindList);
	websAspDefine(T("aspOutReadArp"), aspOutReadArp);
	websAspDefine(T("aspOutArpBindGlobalConfig"), getArpBindGlobalConfig);
}
