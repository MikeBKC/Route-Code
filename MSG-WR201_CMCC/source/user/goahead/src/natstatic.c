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

#define    INDEX_MAPNAME    0
#define    INDEX_OUTPORT    3 
#define    INDEX_IP         4
#define    INDEX_INNERPORT  5

extern void iptablesPortForwardFlush(void);
extern void iptablesPortForwardRun(void);

void RemCtrlToNatList(char *map);
/***********************************************************************
 * 函 数 名：   init_remoteControl
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
void init_remoteControl()
{
	char lanIp[16] = {0},list[40]={0},*enable,*profile;
        getIfIp(getLanIfName(), lanIp);
	enable=nvram_bufget(RT2860_NVRAM,"REMOTEHTTPEN");
	profile=nvram_bufget(RT2860_NVRAM,"REMOTEPROFILE");
	if(!(*enable))
	    enable="2";
        sprintf(list,"%s,%s,TCP,%s,%s,80,1,%s",remCtrlName,enable,nvram_bufget(RT2860_NVRAM,"REMOTEHTTPPORT"),lanIp, profile);
	RemCtrlToNatList(list);
}
/***********************************************************************
 * 函 数 名：   getSystemRemoteControlInfo
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/

static int getSystemRemoteControlInfo(int eid, webs_t wp, int argc, char_t **argv)
{
    websWrite(wp,"var HttpEnables=\"%s\";",nvram_bufget(RT2860_NVRAM,"REMOTEHTTPEN"));
    websWrite(wp,"var OutPorts=\"%s\";",nvram_bufget(RT2860_NVRAM,"REMOTEHTTPPORT"));
    websWrite(wp,"var Profiles=\"%s\";",nvram_bufget(RT2860_NVRAM,"REMOTEPROFILE"));
    getTheErrorMsg(eid,wp,argc,argv);
    return 0;
}
/***********************************************************************
 * 函 数 名：   formConfigSystemRemoteControlInfo
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/

static void formConfigSystemRemoteControlInfo(webs_t wp, char_t *path, char_t *query)
{
    char_t *remoteHttpEnable, *httpExternPort, *profile;
    a_assert(wp);
    remoteHttpEnable = websGetVar(wp, T("HttpEnable"), T("")); 
    httpExternPort = websGetVar(wp, T("OutPort"), T("")); 
    profile = websGetVar(wp, T("Profile"), T("WAN1")); 
    nvram_bufset(RT2860_NVRAM,"REMOTEHTTPEN", remoteHttpEnable);
    nvram_bufset(RT2860_NVRAM,"REMOTEHTTPPORT", httpExternPort);
    nvram_bufset(RT2860_NVRAM,"REMOTEPROFILE", profile);
    init_remoteControl();
    websRedirect(wp, "Remote.asp");
    nvram_commit(RT2860_NVRAM);
}


static int NatMapCounter=0; 
/***********************************************************************
 * 函 数 名：   delOneRecord
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/



char* delOneRecord(char * list,char * name)
{
    char *p1,*p2;
    p1=strnmatch(list,name,INDEX_MAPNAME);// || strnmatch(MapList,OutPort,INDEX_OUTPORT))
    if(p1)
    {
         p2=p1;
         while( (*p2!=UTT_SPLIT_SIGN_RECORD) && *p2)
         {
            p2++;
         }//*p2=';'or '\0'
         if(p1!=list) p1--;   //before p1 there is the char';'
         else if(*p2) p2++;    //if del the first record ,jump over the char ';'
         *p1=0;
         strcat(list,p2);
    }
    return p1;
}
/***********************************************************************
 * 函 数 名：   RemCtrlToNatList
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
void RemCtrlToNatList(char *map)
{
    char_t *MapList,*newList,name[7]={0};
    int len;
    MapList = nvram_bufget(RT2860_NVRAM,"NatMapList");
    strncpy(name,map,strlen(remCtrlName)); 
    delOneRecord(MapList,name);
    len=strlen(MapList)+strlen(map)+2;
    newList = (char*)malloc(len);
    strcpy(newList,map);
    if(*MapList)
       sprintf((newList+strlen(newList)),";%s",MapList);
    nvram_bufset(RT2860_NVRAM,"NatMapList",newList);
    nvram_commit(RT2860_NVRAM);
    sendMsgForSpeed(INIT_STATIC_NAT,0,0,NULL,0);
    free(newList);
}
/***********************************************************************
 * 函 数 名：   getNatOneMapInfo
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/

static int getNatOneMapInfo(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t *edit_name,*MapList,*tmp,MapName[12],enable[2],proto[8],OutPort[6],ip[16],InnerPort[6],PortNum[6],NatBind[8];
    DBGPRINT("***********getNatOneMapInfo**********\n");
    if(ejArgs(argc,argv,T("%s"), &edit_name) <1) 
    {
	setTheErrorMsg(T("读取edit_Name变量失败!"));
        goto out;
    }
    DBGPRINT("edit_name:%s\n",edit_name);
    if(!edit_name)
    {
	DBGPRINT("用户名不能为空\n");    
        setTheErrorMsg(T("用户名不能为空!"));
        goto out;
    }
    MapList = nvram_bufget(RT2860_NVRAM,"NatMapList");
    tmp=strnmatch(MapList,edit_name,INDEX_MAPNAME);

   // DBGPRINT("tmp1:%s\n",tmp1);
    if(!tmp)
    {
        setTheErrorMsg(T("找不到该用户!"));
        goto out;
    }
   // DBGPRINT("def\n");
    if(sscanf(tmp,"%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^;]",MapName,enable,proto,OutPort,ip,InnerPort,PortNum,NatBind)==8)
    {
        websWrite(wp,T("IDs = \"%s\";"),MapName);
        websWrite(wp, T("ConfigEnables = \"%s\";"),enable);
        websWrite(wp, T("Protocols = \"%s\";"), proto);
        websWrite(wp, T("OutPorts = \"%s\";"),OutPort);
        websWrite(wp, T("IPs = \"%s\";"),ip);
        websWrite(wp, T("InnerPorts = \"%s\";"),InnerPort);
        websWrite(wp, T("PortNums = \"%s\"; "),PortNum);
        websWrite(wp, T("NatBinds = \"%s\"; "),utt_strtoup(NatBind));
    }
out:
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
/***********************************************************************
 * 函 数 名：   getNatMapList
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/

static int getNatMapList(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t *tmpRecord, *MapList,MapName[12],enable[2],proto[8],OutPort[6],ip[16],InnerPort[6],PortNum[6],NatBind[8];
    int  num = 0;
    DBGPRINT("**********getNatMapList**********\n");
    websWrite(wp,"var IDs=new Array();");
    websWrite(wp,"var ConfigEnables = new Array();");
    websWrite(wp,"var Protocols = new Array();");
    websWrite(wp,"var OutPorts=new Array();");
    websWrite(wp,"var IPs=new Array();");
    websWrite(wp,"var InnerPorts=new Array();");
    websWrite(wp,"var PortNums=new Array();");
    websWrite(wp,"var NatBinds=new Array();");


    MapList = nvram_bufget(RT2860_NVRAM,"NatMapList");
    tmpRecord=MapList;
    while(*tmpRecord)
    {
	sscanf(tmpRecord,"%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^;]",MapName,enable,proto,OutPort,ip,InnerPort,PortNum,NatBind);

        websWrite(wp,T("IDs[%d] = \"%s\";"),num,MapName);
        websWrite(wp, T("ConfigEnables[%d] = \"%s\";"),num,enable);
	websWrite(wp, T("Protocols[%d] = \"%s\";"),num, proto);
        websWrite(wp, T("OutPorts[%d] = \"%s\";"), num, OutPort);
	websWrite(wp, T("IPs[%d] = \"%s\";"), num, ip);
	websWrite(wp, T("InnerPorts[%d] = \"%s\";"), num,InnerPort);
	websWrite(wp, T("PortNums[%d] = \"%s\"; "),num, PortNum);
	websWrite(wp, T("NatBinds[%d] = \"%s\"; "),num, wanReName(utt_strtoup(NatBind)));
        num++;
        tmpRecord=NextRecordStart(tmpRecord);
    }
    NatMapCounter = num;
    DBGPRINT("counter of the Map is %d\n",NatMapCounter);
    websWrite(wp,"var totalrecs=%d;",num);
//    websWrite(wp,"var max_totalrecs=%d;",UTT_MAX_NAT_MAP);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

#define addrecord   strcat(newList,MapName);\
        strcat(newList,UTT_SPLIT_SIGN_STR);\
	strcat(newList,ConfigEnable); \
        strcat(newList,UTT_SPLIT_SIGN_STR);\
	strcat(newList,Proto);\
        strcat(newList,UTT_SPLIT_SIGN_STR);\
	strcat(newList,OutPort);\
        strcat(newList,UTT_SPLIT_SIGN_STR);\
	strcat(newList,ip);\
        strcat(newList,UTT_SPLIT_SIGN_STR);\
	strcat(newList,InnerPort);\
        strcat(newList,UTT_SPLIT_SIGN_STR);\
	strcat(newList,PortNum)
/***********************************************************************
 * 函 数 名：   formConfigNatMap
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/

static void formConfigNatMap(webs_t wp, char_t *path, char_t *query)
{
    char_t *MapName,*ConfigEnable,*MapNameOld,*OutPort,*ip,*InnerPort,*PortNum,*Proto,*action,*NatBindIf,*MapList,*newList,*p1,*p2;
    int tempLen;
    DBGPRINT("********formConfigNatMap********\n");
    a_assert(wp);
    MapList = nvram_bufget(RT2860_NVRAM,"NatMapList");

    MapName=websGetVar(wp,T("ID"), T(""));
    ConfigEnable=websGetVar(wp,T("ConfigEnable"), T(""));
    Proto=websGetVar(wp,T("Protocol"), T(""));
    OutPort=websGetVar(wp,T("OutPort"), T(""));
    ip=websGetVar(wp,T("IP"), T(""));
    InnerPort=websGetVar(wp,T("InnerPort"), T(""));
    PortNum=websGetVar(wp,T("PortNum"), T(""));
    action = websGetVar(wp, T("Action"), T("")); 
    NatBindIf=websGetVar(wp, T("NatBind"), T(""));
    utt_strtoup(NatBindIf);

    DBGPRINT("%s Nat\n", action);
    DBGPRINT("NatMapList before change:%s\n",MapList);
    DBGPRINT("new record:%s,%s,%s,%s,%s,%s,%s,%s\n",MapName,ConfigEnable,Proto,OutPort,ip,InnerPort,PortNum,NatBindIf);
    tempLen=strlen(MapList)+strlen(MapName)+strlen(ConfigEnable)+\
	strlen(Proto)+strlen(OutPort)+strlen(ip)+strlen(InnerPort)+strlen(PortNum)+strlen(NatBindIf)+8+8;
    
    if(!strcmp(action, "add"))
    {
        if(NatMapCounter >= UTT_MAX_NAT_MAP)
	{
            setTheErrorMsg(T("已达最大条目！"));
	    goto out;
        }
	if( 
           strnmatch(MapList,MapName,INDEX_MAPNAME)// || strnmatch(MapList,OutPort,INDEX_OUTPORT))
	//    || ( strnmatch(MapList,ip,INDEX_IP) && (strnmatch(MapList,InnerPort,INDEX_INNERPORT)))      
	  )
	{
            setTheErrorMsg(T("名称重复"));
	    goto out;
        }
        
        newList = (char*)malloc(sizeof(char) * tempLen);
        strcpy(newList, MapList); 
	p1=newList+strlen(MapList);//记录新添加的规则的起始位置，作为参数调用函数 EditOneNatMap()
        if(strlen(newList) != 0)
	{
	    strcat(newList, UTT_SPLIT_SIGN_RECORDS);
	    p1++;
	}
#if 0
        addrecord;   
#else
	sprintf(p1,"%s,%s,%s,%s,%s,%s,%s,%s",MapName,ConfigEnable,Proto,OutPort,ip,InnerPort,PortNum,NatBindIf);
#endif
    }
    else //modify
    {
        MapNameOld = websGetVar(wp, T("IDold"), T("")); 
	if(
            (strcmp(MapNameOld,MapName)!=0) && strnmatch(MapList,MapName,INDEX_MAPNAME)
  	 )
	{
            setTheErrorMsg(T("名称重复"));
            DBGPRINT("Old Name[%s],New Name[%s]\n", MapNameOld, MapName);
            goto out;
	}
	p1=strnmatch(MapList,MapNameOld,INDEX_MAPNAME);
	if(p1==0)
	{
	    setTheErrorMsg(T("找不到该条映射!"));
	    goto out;
	}
	p2=p1;//要修改的映射记录的起始位置
	while( (*p2!=UTT_SPLIT_SIGN_RECORD) && *p2)
	{
	    p2++;
	}//*p2=';'or '\0'，要修改的映射记录的结束位置的下一个字节处
	//DBGPRINT("MapList:%s\n",MapList);
	//DBGPRINT("p2:%s\n",p2);
	*p1=0;
        newList = (char*)malloc(sizeof(char) * tempLen);
	strcpy(newList,MapList);            //copy the string before UserNameOld
//	DBGPRINT("newList_MapList:%s\n",newList);
//	DBGPRINT("p2%s\n",p2);
	p1=newList+strlen(MapList);//记录新添加的规则的起始位置，作为参数调用函数 EditOneNatMap()
#if 0
        addrecord;   
       	strcat(newList,p2);                             //copy the string after UserNameOld record
#else
	sprintf(p1,"%s,%s,%s,%s,%s,%s,%s,%s%s",MapName,ConfigEnable,Proto,OutPort,ip,InnerPort,PortNum,NatBindIf,p2);
#endif
    }
    nvram_bufset(RT2860_NVRAM,"NatMapList",newList);
    nvram_commit(RT2860_NVRAM);
    free(newList);
    init_remoteControl();
out:
    websRedirect(wp, "NatStatic.asp");
}
/***********************************************************************
 * 函 数 名：   formNatMapDel
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/

static void formNatMapDel(webs_t wp, char_t *path, char_t *query)
{
        char_t *MapName=NULL,*MapList,*tmp;
	DBGPRINT("**********formNatMapDel**********\n");
        a_assert(wp);
        MapList = nvram_bufget(RT2860_NVRAM,"NatMapList");
        MapName = websGetVar(wp,T("delstr"),T("")); 
	if(!strcmp(MapName,T("")))
	{   
	    setTheErrorMsg(T("找不到该用户名"));
            goto out;
	}
       // DBGPRINT("del MapName:%s \n",MapName);
        tmp=strtok(MapName,UTT_SPLIT_SIGN_STR);//可能删除多条映射
	while(tmp!=NULL)
	{
	    if(!strcmp(tmp,remCtrlName))
	    {
	        setTheErrorMsg(T("不能删除远程管理端口映射"));
	    }
	    else
	        delOneRecord(MapList,tmp);
            tmp=strtok(NULL,UTT_SPLIT_SIGN_STR);
	}
        nvram_bufset(RT2860_NVRAM, "NatMapList",MapList);
        nvram_commit(RT2860_NVRAM);
	init_remoteControl();
out:
        websRedirect(wp,"NatStatic.asp");
}
/***********************************************************************
 * 函 数 名：   formNatMapDelAll
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/

static void formNatMapDelAll(webs_t wp, char_t *path, char_t *query)
{
    //DBGPRINT("formNatMapDelAll\n");  
    a_assert(wp);
    nvram_bufset(RT2860_NVRAM, "NatMapList", "");
    nvram_commit(RT2860_NVRAM);
    init_remoteControl();
    websRedirect(wp, "NatStatic.asp");
}
/***********************************************************************
 * 函 数 名：   formDefineNatMapConfig
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/

void formDefineNatMapConfig(void)
{ 
    websFormDefine(T("formNatStaticMap"),formConfigNatMap);
    websFormDefine(T("formNatStaticMapDelAll"),formNatMapDelAll);
    websFormDefine(T("formNatStaticMapDel"),formNatMapDel);
    websAspDefine(T("aspOutNatStaticOneMapInfo"),getNatOneMapInfo);
    websAspDefine(T("aspOutNatStaticMap"),getNatMapList);
    websFormDefine(T("formRemoteControl"),  formConfigSystemRemoteControlInfo);
    websAspDefine(T("aspOutRemoteControlInfo"),  getSystemRemoteControlInfo);
}
