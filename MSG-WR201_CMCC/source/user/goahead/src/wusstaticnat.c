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
#include "mib.h"
#include "profacce.h"
#include "uttfunction.h"
#include "wusL2tpServer.h"
#include "wusPptpServer.h"
#include "translate.h"
#if(UTT_NV_1800N != FYES)
/***********************************************************************
 * º¯ Êý Ãû£º   init_remoteControl
 * ´´½¨ÈÕÆÚ£º	2011-10-11
 * ÐÞ¸ÄÈÕÆÚ£
 * ×÷Õß£º       yqyang
 * ¸½¼ÓËµÃ÷£º	³õÊ¼»¯Ô¶³Ì¹ÜÀíÅäÖÃ,Ìí¼Ó³õÊ¼»¯¹ÜÀíÅäÖÃµ¼µÚÒ»ÌõSTATICNAT ÅäÖÃ
 ***********************************************************************/
void init_remoteControl(char *remoteHttpEnable,char *httpExternPort,char *profile)
{
    char lanIp[16] = {0};
    getIfIp(getLanIfName(), lanIp);
    struct in_addr addr_ip;
    inet_aton(lanIp, &addr_ip);
    

    MibProfileType mibType = MIB_PROF_STATIC_NAT;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    StaticNatProfile *staticNatProfile= NULL;
    profAction = PROF_CHANGE_EDIT;
    ProfBackupByIndex(mibType, profAction, 0, &profhead);
    staticNatProfile = (StaticNatProfile*)ProfGetInstPointByIndex(mibType, 0);
    ProfSetNameByPoint(staticNatProfile, remCtrlName);
    staticNatProfile->head.active = strtoul(remoteHttpEnable,0,10);
    staticNatProfile->OutPort= strtoul(httpExternPort,0,10);
    staticNatProfile->protocol= 1;
    staticNatProfile->IP = addr_ip.s_addr;
    staticNatProfile->InnerPort= 80;
    staticNatProfile->PortNum= 1;
    strcpy(staticNatProfile->NatBind,profile);
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
}

/***********************************************************************
 * º¯ Êý Ãû£º   getSystemRemoteControlInfo
 * ´´½¨ÈÕÆÚ£º	2011-10-11
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       yqyang
 * ¸½¼ÓËµÃ÷£º	ÎÞ
 ***********************************************************************/

static int getSystemRemoteControlInfo(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_STATIC_NAT;
    StaticNatProfile* staticNatProfile = NULL;
    staticNatProfile = (StaticNatProfile*)ProfGetInstPointByName(mibType, remCtrlName);

    if(staticNatProfile == NULL)
    {
	websWrite(wp,"var HttpEnables=\"0\";");
	websWrite(wp,"var OutPorts=\"8081\";");
	websWrite(wp,"var Profiles=\"\";");
    
    }else
    {
	websWrite(wp,"var HttpEnables=\"%d\";",staticNatProfile->head.active);
	websWrite(wp,"var OutPorts=\"%d\";",staticNatProfile->OutPort);
	websWrite(wp,"var Profiles=\"%s\";",utt_strtoup(staticNatProfile->NatBind));
    }
    getTheErrorMsg(eid,wp,argc,argv);
#if (UTT_SMART_UI == FYES) 
    getTheRightMsg(eid,wp,argc,argv);
#endif
    return 0;
}
/***********************************************************************
 * º¯ Êý Ãû£º   formConfigSystemRemoteControlInfo
 * ´´½¨ÈÕÆÚ£º	2011-10-11
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       yqyang
 * ¸½¼ÓËµÃ÷£º	Ô¶³Ì¹ÜÀíÌá½»º¯Êý
 ***********************************************************************/

static void formConfigSystemRemoteControlInfo(webs_t wp, char_t *path, char_t *query)
{
    char_t *remoteHttpEnable, *httpExternPort, *profile;
    a_assert(wp);

    remoteHttpEnable = websGetVar(wp, T("HttpEnable"), T("")); 
    httpExternPort = websGetVar(wp, T("OutPort"), T("")); 
    profile = websGetVar(wp, T("Profile"), T("WAN1")); 
    init_remoteControl(remoteHttpEnable, httpExternPort,profile);

#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, "RemoteManage.asp");
#else
    websRedirect(wp, "Remote.asp");
#endif
}


/***********************************************************************
 * º¯ Êý Ãû£º   getNatOneMapInfo
 * ´´½¨ÈÕÆÚ£º	2011-10-11
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       yqyang
 * ¸½¼ÓËµÃ÷£º	ÎÞ
 ***********************************************************************/

static int getNatOneMapInfo(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t *edit_name;
    const char *errMsg=NULL;
    MibProfileType mibType = MIB_PROF_STATIC_NAT;
    StaticNatProfile *prof;
    if(ejArgs(argc,argv,T("%s"), &edit_name) <1) 
    {
	errMsg = getTransDataForC(C_LANG_INDEX_READ_EDIT_NAME_ERR);
	setTheErrorMsg(errMsg);
	goto out;
    }
    DBGPRINT("edit_name:%s\n",edit_name);
    if(!edit_name)
    {
	errMsg = getTransDataForC(C_LANG_INDEX_NAT_MAP_NAME_NULL);
	setTheErrorMsg(errMsg);
	goto out;
    }

    prof = (StaticNatProfile*)ProfGetInstPointByName(mibType, edit_name);
    if(prof == NULL) {
	errMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_THE_GROP_CONF);
	setTheErrorMsg(errMsg);
    } else {
	websWrite(wp,T("IDs = \"%s\";"),prof->head.name);
	websWrite(wp, T("ConfigEnables = \"%d\";"),prof->head.active);
	websWrite(wp, T("Protocols = \"%d\";"), prof->protocol);
	websWrite(wp, T("OutPorts = \"%d\";"),prof->OutPort);
	websWrite(wp, T("IPs = \"%N\";"),prof->IP);
	websWrite(wp, T("InnerPorts = \"%d\";"),prof->InnerPort);
	websWrite(wp, T("PortNums = \"%d\"; "),prof->PortNum);
	websWrite(wp, T("NatBinds = \"%s\"; "),utt_strtoup(prof->NatBind));
    }
out:
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
/***********************************************************************
 * º¯ Êý Ãû£º   getNatMapList
 * ´´½¨ÈÕÆÚ£º	2011-10-11
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       yqyang
 * ¸½¼ÓËµÃ÷£º	ÎÞ
 ***********************************************************************/

static int getNatMapList(int eid, webs_t wp, int argc, char_t **argv)
{
    unsigned int num=0;
    int min = 0, max = 0;
    int i = 0;
    MibProfileType mibType = MIB_PROF_STATIC_NAT;
    StaticNatProfile *prof= NULL;

    websWrite(wp,"var IDs=new Array();");
    websWrite(wp,"var ConfigEnables = new Array();");
    websWrite(wp,"var Protocols = new Array();");
    websWrite(wp,"var OutPorts=new Array();");
    websWrite(wp,"var IPs=new Array();");
    websWrite(wp,"var InnerPorts=new Array();");
    websWrite(wp,"var PortNums=new Array();");
    websWrite(wp,"var NatBinds=new Array();");


    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = (StaticNatProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) {
	    websWrite(wp,T("IDs[%d] = \"%s\";"),num,prof->head.name);
	    websWrite(wp, T("ConfigEnables[%d] = \"%d\";"),num,prof->head.active);
	    websWrite(wp, T("Protocols[%d] = \"%d\";"),num, prof->protocol);
	    websWrite(wp, T("OutPorts[%d] = \"%d\";"), num, prof->OutPort);
	    websWrite(wp, T("IPs[%d] = \"%N\";"), num, prof->IP);
	    websWrite(wp, T("InnerPorts[%d] = \"%d\";"), num,prof->InnerPort);
	    websWrite(wp, T("PortNums[%d] = \"%d\"; "),num, prof->PortNum);
	    websWrite(wp, T("NatBinds[%d] = \"%s\"; "),num, utt_strtoup(prof->NatBind));
	    num++;
	}
    }
    websWrite(wp,"var totalrecs=%d;",num);
    websWrite(wp,"var max_totalrecs=%d;",MAX_STATIC_NAT_PROFILES);
    getTheErrorMsg(eid, wp, argc, argv);
#if (UTT_SMART_UI == FYES) 
    getTheRightMsg(eid,wp,argc,argv);
#endif
    return 0;
}
/***********************************************************************
 * º¯ Êý Ãû£º   checkPortOverlap
 * ´´½¨ÈÕÆÚ£º	2011-11-23
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       yqyang
 * ¸½¼ÓËµÃ÷£º	¼ì²é¾²Ì¬NATµÄÍâ²¿¶Ë¿ÚÊÇ·ñÖØµþ
 ***********************************************************************/
static int checkPortOverlap(unsigned int nip, char *name,char *NatBindIf,char *OutPort, char *InnerPort,char *Proto, char *PortNum)
{ 
    DBGPRINT("%s: nip[%x] name[%s] NatBindIf[%s] OutPort[%s] InnerPort[%s] Proto[%s] PortNum[%s]\n",__FUNCTION__,nip,name,NatBindIf,OutPort, InnerPort,Proto,PortNum);
    double nOutPort = strtoul(OutPort,0,10);
    double nPortNum =  strtoul(PortNum,0,10);
    double nInnerPort = strtoul(InnerPort,0,10);
    int nProto = strtoul(Proto,0,10);
    int min = 0, max = 0;
    int i = 0;
    MibProfileType mibType = MIB_PROF_STATIC_NAT;
    StaticNatProfile *prof= NULL;
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = (StaticNatProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) {
	    if(	    (strcmp(name, prof->head.name) != 0)
		    && (strcmp(NatBindIf, prof->NatBind) == 0) 
		    && ((nProto ==  prof->protocol) || nProto == STATIC_NAT_TCP_UDP || prof->protocol == STATIC_NAT_TCP_UDP) 
	      )
	    {
		if(!((nOutPort > prof->OutPort+prof->PortNum -1) || (nOutPort + nPortNum-1 < prof->OutPort))) 
		{
		    /* Íâ²¿¶Ë¿ÚÖØµþ           */
		    return -1;
		}
		if((nip == prof->IP) && (!((nInnerPort > prof->InnerPort +prof->PortNum -1) || (nInnerPort + nPortNum-1 < prof->InnerPort))))
		{
		    /*ÏàÍ¬IP ÄÚ²¿¶Ë¿ÚÖØµþ           */
		    return -2;
		}
	    }
	}
    }
    return 0;
}

/***********************************************************************
 * º¯ Êý Ãû£º   formConfigNatMap
 * ´´½¨ÈÕÆÚ£º	2011-10-11
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       yqyang
 * ¸½¼ÓËµÃ÷£º	ÎÞ
 ***********************************************************************/

static void formConfigNatMap(webs_t wp, char_t *path, char_t *query)
{
    char_t *MapName,*ConfigEnable,*MapNameOld,*OutPort,*ip,*InnerPort,*PortNum,*Proto,*action,*NatBindIf;
    const char *errMsg=NULL;
    a_assert(wp);

    struct in_addr addr_ip;
    int instIndex;
    int retError = 0;
    MibProfileType mibType = MIB_PROF_STATIC_NAT;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    StaticNatProfile *staticNatProfile = NULL;

#if 0
    /*ÒÑÔÚ³ö³§ÅäÖÃÊ±³õÊ¼»¯*/
    staticNatProfile = (StaticNatProfile*)ProfGetInstPointByName(mibType, remCtrlName);

    if(staticNatProfile == NULL){
	init_remoteControl("0", "8081","WAN1");
    }
#endif

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
	MapNameOld = websGetVar(wp, T("IDold"), T("")); 

#if (L2TP_SERVER == FYES)
	if ((strcmp(MapName, L2TP_NAT_NAME) == 0) || (strcmp(MapNameOld, L2TP_NAT_NAME) == 0)) {
	    errMsg = getTransDataForC(C_LANG_INDEX_NOT_CHANGE_DEFAULT_MAP);
	    setTheErrorMsg(errMsg);
	    goto out;
	}
#endif
#if (PPTP_SERVER == FYES)
	if ((strcmp(MapName, PPTP_NAT_NAME) == 0) || (strcmp(MapNameOld, PPTP_NAT_NAME) == 0)) {
	    errMsg = getTransDataForC(C_LANG_INDEX_NOT_CHANGE_DEFAULT_MAP); 
	    setTheErrorMsg(errMsg);
	    goto out;
	}
#endif
#if (SNMP == FYES)
	if ((strcmp(MapName, SNMP_NAT_NAME) == 0) || (strcmp(MapNameOld, SNMP_NAT_NAME) == 0)) {
	    errMsg = getTransDataForC(C_LANG_INDEX_NOT_CHANGE_DEFAULT_MAP); 
	    setTheErrorMsg(errMsg);
	    goto out;
	}
#endif

	if(inet_aton(ip, &addr_ip) == 0){  
	    errMsg = getTransDataForC(C_LANG_INDEX_IP_PLAN_ADDR_ERR);
	    setTheErrorMsg(errMsg);
	    goto out;
	}
	instIndex = ProfGetInstIndexByName(mibType, MapNameOld);
	if(strcmp(action,"add") == 0)
	{
	    retError =  checkPortOverlap(addr_ip.s_addr,MapName,NatBindIf, OutPort, InnerPort,Proto,PortNum);
	    if(retError == -1)
	    {
#if 0
		errMsg = getTransDataForC(C_LANG_INDEX_EXTER_PORT_REPEAT);
		setTheErrorMsg(errMsg);
		goto out;
#endif
	    }
	    if(retError == -2)
	    {
		errMsg = getTransDataForC(C_LANG_INDEX_SAME_IP_PORT_REPEAT);
		setTheErrorMsg(errMsg);
		goto out;
	    }

	    if(instIndex != PROFFAIL)
	    {
		errMsg = getTransDataForC(C_LANG_INDEX_NAT_MAP_NAME_EXIST);
		setTheErrorMsg(errMsg);
		goto out;
	    }
	    instIndex = ProfNewInst(mibType, MapName, FALSE);
	    if(instIndex == PROFFAIL) 
	    {
		errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_MAX_TOTAL_ERR);
		setTheErrorMsg(errMsg);
		goto out;
	    }
	    profAction = PROF_CHANGE_ADD;

	}
	else //modify
	{
	    retError = checkPortOverlap(addr_ip.s_addr,MapNameOld,NatBindIf, OutPort,InnerPort, Proto,PortNum);
	    if(retError == -1)
	    {
#if 0
		errMsg = getTransDataForC(C_LANG_INDEX_EXTER_PORT_REPEAT);
		setTheErrorMsg(errMsg);
		goto out;
#endif
	    }
	    if(retError == -2)
	    {
		errMsg = getTransDataForC(C_LANG_INDEX_SAME_IP_PORT_REPEAT);
		setTheErrorMsg(errMsg);
		goto out;
	    }


	    if(instIndex == PROFFAIL) {
		errMsg = getTransDataForC(C_LANG_INDEX_NAT_MAP_NAME);
		setTheErrorMsg(errMsg);
		goto out;
	    }
	    if (strcmp(MapName,MapNameOld) != 0){
		int newIndex;
		newIndex = ProfGetInstIndexByName(mibType, MapName);
		if(newIndex != PROFFAIL)
		{
		    errMsg = getTransDataForC(C_LANG_INDEX_NAT_MAP_NAME_EXIST);
		    setTheErrorMsg(errMsg);
		    goto out;
		}
	    }
	    profAction = PROF_CHANGE_EDIT;
	}
	ProfBackupByIndex(mibType, profAction, instIndex, &profhead);
	staticNatProfile= (StaticNatProfile*)ProfGetInstPointByIndex(mibType, instIndex);
	ProfSetNameByPoint(staticNatProfile, MapName);
	staticNatProfile->head.active = strtoul(ConfigEnable,0,10);
	staticNatProfile->protocol= strtoul(Proto,0,10);
	staticNatProfile->OutPort = strtoul(OutPort,0,10);
	staticNatProfile->IP = addr_ip.s_addr;
	staticNatProfile->InnerPort = strtoul(InnerPort,0,10);
	staticNatProfile->PortNum = strtoul(PortNum,0,10);
	strcpy(staticNatProfile->NatBind,NatBindIf);
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
out:
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, "VirtualServer.asp");
#else
    websRedirect(wp, "NatStatic.asp");
#endif
}
/***********************************************************************
 * º¯ Êý Ãû£º   formNatMapDel
 * ´´½¨ÈÕÆÚ£º	2011-10-11
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       yqyang
 * ¸½¼ÓËµÃ÷£º	ÎÞ
 ***********************************************************************/

static void formNatMapDel(webs_t wp, char_t *path, char_t *query)
{
    char_t *MapName=NULL, *tmp = NULL, *ptr;
    const char *errMsg=NULL;
    a_assert(wp);
    MibProfileType mibType = MIB_PROF_STATIC_NAT;
    struProfAlloc *profList  = NULL;
#if 0
    int back = 0;
#endif
    
    MapName = websGetVar(wp,T("delstr"),T("")); 
    if(!strcmp(MapName,T("")))
    {   
	errMsg = getTransDataForC(C_LANG_INDEX_NAT_MAP_NAME);
	setTheErrorMsg(errMsg);
	goto out;
    }
    tmp=strtok_r(MapName, UTT_SPLIT_SIGN_STR,&ptr);
    while(tmp!=NULL)
    {
	if((!strcmp(tmp,"admin")) 
#if (L2TP_SERVER == FYES)
		|| (strcmp(tmp, L2TP_NAT_NAME) == 0)
#endif
#if (PPTP_SERVER == FYES)
		|| (strcmp(tmp, PPTP_NAT_NAME) == 0)
#endif
#if (SNMP == FYES)
		|| (strcmp(tmp, SNMP_NAT_NAME) == 0)
#endif
		)
	{
	    errMsg = getTransDataForC(C_LANG_INDEX_DEL_DEFAULT_MAP);
	    setTheErrorMsg(errMsg);
	}
	else
	{   	
	    ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profList);
	    ProfDelInstByName(mibType, tmp); 
	}
	tmp=strtok_r(NULL, UTT_SPLIT_SIGN_STR,&ptr);
    }
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
out:
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp,"VirtualServer.asp");
#else
    websRedirect(wp,"NatStatic.asp");
#endif
}
/***********************************************************************
 * º¯ Êý Ãû£º   formNatMapDelAll
 * ´´½¨ÈÕÆÚ£º	2011-10-11
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       yqyang
 * ¸½¼ÓËµÃ÷£º	ÎÞ
 ***********************************************************************/

static void formNatMapDelAll(webs_t wp, char_t *path, char_t *query)
{
    const char *tmp = NULL;
    int min = 0, max = 0;
    int i = 0;
    int back = 0;
    MibProfileType mibType = MIB_PROF_STATIC_NAT;
    Pointer prof;
    struProfAlloc *profList  = NULL;
    a_assert(wp);
    ProfInstLowHigh(mibType, &max, &min);

    for(i = min; i < max; i++) {
	prof = ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
	    tmp = ProfGetInstNameByIndex(mibType,i);
	    if((strcmp(tmp,remCtrlName) == 0) 
#if (L2TP_SERVER == FYES)
		    || (strcmp(tmp, L2TP_NAT_NAME) == 0)
#endif
#if (PPTP_SERVER == FYES)
		    || (strcmp(tmp, PPTP_NAT_NAME) == 0)
#endif
#if (SNMP == FYES)
		    || (strcmp(tmp, SNMP_NAT_NAME) == 0)
#endif
		    ) {
		continue;
	    }
	    else{
		if(back == 0) {
		    ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, i, &profList);
		    back = 1;
		}
		ProfDelInstByIndex(mibType, i);
	    }
	}
    }
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp,"VirtualServer.asp");
#else
    websRedirect(wp,"NatStatic.asp");
#endif
}
/***********************************************************************
 * º¯ Êý Ãû£º   formDefineNatMapConfig
 * ´´½¨ÈÕÆÚ£º	2011-10-11
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       yqyang
 * ¸½¼ÓËµÃ÷£º	ÎÞ
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
#endif
