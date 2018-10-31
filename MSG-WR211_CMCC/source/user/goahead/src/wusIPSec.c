#include	"uttMachine.h"
#if (IP_SEC == FYES)
#include	<stdlib.h>
#include	<stdio.h>
#include	<sys/sysinfo.h>
#include        <fcntl.h>
#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include	"argcmd.h"
#include	"wuScript.h"
#include	"utt_pppoeClient.h"
#include	"wusPptpServer.h"
#include	"translate.h"
#include	"ifmisc.h"


#ifdef IPSEC_DEBUG
#define IPSEC_PRINT(fmt, args...)		printf(fmt, ## args)
#else
#define IPSEC_PRINT(fmt, args...)
#endif

extern int getWanGwByIndex(int wanIndex, char* gw);
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
    socketOpen();
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
    socketClose();
    return;
}

#if 0
/*
 * updateDomainIP()
 *
 * 更新域名 IP地址
 *
 * 如果IP变化，则重新连接ipsec
 */
extern void updateDomainIP()
{
    MibProfileType mibipsec = MIB_PROF_IPSEC;
    ipsecConfProfile *ipsecProf = NULL;
    struProfAlloc *profList = NULL;
    int max=0, min=0,index=-1;
    ProfChangeType profAction = PROF_CHANGE_EDIT;
    char domainIP[MAX_IPSEC_IP_LEN+1]={0};

    while(1)
    {
	sleep(5*60);	/* updae every 60 seconds */
	ProfInstLowHigh(mibipsec, &max, &min);
	for(index=min;index < max;index++)
	{
	    memset(domainIP, 0, sizeof(domainIP));
	    ipsecProf = (ipsecConfProfile *)ProfGetInstPointByIndex(mibipsec, index);
	    if((ipsecProf!=NULL)&&(ProfInstIsFree(ipsecProf) == 0))
	    {
		getDomainNameIP(ipsecProf->peer, domainIP);	/* get domain ip */
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
	ProfUpdate(profList);	    /*update*/
	ProfFreeAllocList(profList);    /*erlease*/
	IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
	nvramWriteCommit();		    /*save*/
	profList=NULL;
	}
    }
}
#endif

#if 1
/*
 * setipsecTransformSet
 *
 * 设置二阶段加密认证
 */
static void setipsecTransformSet(char *trans)
{
    char transform_id[20];
    MibProfileType mibtransformset = MIB_PROF_TRANSFORMSET;
    ipsecTransformSetProfile *ipsecTransProf = NULL;
    int index=-1;

    if(trans==NULL) return;
    if((strlen(trans)<3) || (strcmp(trans,"000")==0)) return;
    /* generate transform instance name */
    strcpy(transform_id,"tra_");
    strcat(transform_id,trans);
    IPSEC_PRINT("%s-%d: trans=%s,transform_id=%s\n",__func__,__LINE__,trans,transform_id);
    /* check instance exist or not  */
    if(ProfGetInstIndexByName(mibtransformset, transform_id) != PROFFAIL)
    {
	return;
    }
    /* create new instance */
    index = ProfNewInst(mibtransformset, transform_id, FALSE);
    IPSEC_PRINT("%s-%d:index=%d\n",__FUNCTION__,__LINE__,index);
    if (PROFFAIL != index)
    {
    
	ipsecTransProf = (ipsecTransformSetProfile *)ProfGetInstPointByIndex(mibtransformset, index);
	IPSEC_PRINT("%s-%d:head.name=%s\n",__FUNCTION__,__LINE__,ipsecTransProf->head.name); 
	if((ipsecTransProf!=NULL)&&(ProfInstIsFree(ipsecTransProf) == 0))
	{
	/* 设置加密算法 */
	switch(trans[0]) {
	    case '1':
		ipsecTransProf->encrypt=Des;
		break;
	    case '2':
		ipsecTransProf->encrypt=Tri_Des;
		break;
	    case '3':
		ipsecTransProf->encrypt=Aes;
		break;
	    case '4':
		ipsecTransProf->encrypt=Aes192;
		break;
	    case '5':
		ipsecTransProf->encrypt=Aes256;
		break;
	    case '0':
		ipsecTransProf->encrypt=0;
	    break;
	}
	/* 设置esp认证 */
	switch(trans[1]) {
	    case '1':
		ipsecTransProf->espAuth=Md5;
		break;
	    case '2':
		ipsecTransProf->espAuth=Sha;
		break;
	    case '0':
		ipsecTransProf->espAuth=0;
		break;
	}
	/* 设置AH认证 */
	switch(trans[2]) {
	    case '1':
		ipsecTransProf->auth=Md5;
		break;
	    case '2':
		ipsecTransProf->auth=Sha;
		break;
	    case '0':
		ipsecTransProf->auth=0;
		break;
	}
	IPSEC_PRINT("%s-%d: encrypt=%d,espauth=%d,auth=%d\n",__func__,__LINE__,ipsecTransProf->encrypt,ipsecTransProf->espAuth,ipsecTransProf->auth);
	}
    }
    return;
}

/*
 * setisakmpPolicyValue()
 *
 * 设置第一阶段加密认证
 *
 */
static void setisakmpPolicyValue(char *policy)
{
    MibProfileType mibisakmppolicy = MIB_PROF_ISAKMP_POLICY;
    isakmpPolicyProfile *isakmpPolicyProf = NULL;
    char policy_id[20];
    int index=-1;

    if(policy==NULL) return;
    if((strlen(policy)<3) || (strcmp(policy,"000")==0)) return;
    /* generate policy instance */
    strcpy(policy_id,"p_");
    strcat(policy_id,policy);
    IPSEC_PRINT("%s-%d: policy=%s,policy_id=%s\n",__func__,__LINE__,policy,policy_id);
    /* check instance exist or not */
    if(ProfGetInstIndexByName(mibisakmppolicy, policy_id) != PROFFAIL)
    {
	return;
    }
    /* create new instance */
    index = ProfNewInst(mibisakmppolicy, policy_id, FALSE);
    IPSEC_PRINT("%s-%d:index=%d\n",__FUNCTION__,__LINE__,index);
    if (PROFFAIL != index)
    {
	/* get policy profile pointer */
	isakmpPolicyProf = (isakmpPolicyProfile *)ProfGetInstPointByIndex(mibisakmppolicy, index);
	IPSEC_PRINT("%s-%d:head.name=%s\n",__FUNCTION__,__LINE__,isakmpPolicyProf->head.name); 
	if((isakmpPolicyProf!=NULL)&&(ProfInstIsFree(isakmpPolicyProf) == 0))
	{
	/* 设置加密算法 */
	switch(policy[0]) {
	    case '1':
		isakmpPolicyProf->encrypt = Des;
		break;
	    case '2':
		isakmpPolicyProf->encrypt = Tri_Des;
		break;
	    case '3':
		isakmpPolicyProf->encrypt = Aes;
		break;
	    case '4':
		isakmpPolicyProf->encrypt = Aes192;
		break;
	    case '5':
		isakmpPolicyProf->encrypt = Aes256;
	    break;
	}
	/* 设置认证方式 */
	switch(policy[1]) {
	    case '1':
		isakmpPolicyProf->hash = Md5;
		break;
	    case '2':
		isakmpPolicyProf->hash = Sha;
		break;
	}
	/* 设置组类型 */
	switch(policy[2]) {
	    case '1':
		isakmpPolicyProf->group = Group1;
		break;
	    case '2':
		isakmpPolicyProf->group = Group2;
		break;
	    case '3':
		isakmpPolicyProf->group = Group5;
		break;
	}
	IPSEC_PRINT("%s-%d: encrypt=%d,hash=%d,group=%d\n",__func__,__LINE__,isakmpPolicyProf->encrypt,isakmpPolicyProf->hash,isakmpPolicyProf->group);
	}
    }

    return;
}
#endif

/*
 * isakmpConfigAdd
 *
 * add new ipsec profile config
 *
 * invoke by formIPSecIsakmp_Config
 */
static void isakmpConfigAdd(webs_t wp)
{
    MibProfileType mibipsec = MIB_PROF_IPSEC;
    MibProfileType mibisakmp = MIB_PROF_ISAKMP;
    ipsecConfProfile *ipsecProf = NULL;
    isakmpConfProfile *isakmpProf = NULL;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction = PROF_CHANGE_ADD;
    char instId[UTT_INST_NAME_LEN + 1];
    Uint32 webIndexInst=0U;
    int index=-1;
    int isakmpindex=-1;
    char isakmpId[UTT_INST_NAME_LEN + 1];
    char trans_form[MAX_TRANSFORMSETS_LENGTH+1];
    char policyStr[MAX_ISAKMPPOLICY_LENGTH+1];
    char domainIP[MAX_IPSEC_IP_LEN+1]={0};
    const char *errmsg=NULL;
    /*set varliable for get web value*/
    char *connType = NULL;
    char *peer = NULL;
    char *remoteAddr = NULL;
    char *remoteMask = NULL;
    char *remoteUser = NULL;
    char *remoteUserType = NULL;
    char *localBind = NULL;
    char *localAddr = NULL;
    char *localMask = NULL;
    char *localUser = NULL;
    char *localUserType = NULL;
    char *preshareKey = NULL;
    char *transform1=NULL,*transform2=NULL,*transform3=NULL,*transform4=NULL;
    char *policy1=NULL,*policy2=NULL,*policy3=NULL,*policy4=NULL;
    char *protocol = NULL;
    char *srcPort = NULL, *destPort = NULL;
    char *negMode = NULL;
    char *aggresEncrypt = NULL;
    char *isakmpLifetime = NULL;
    char *lifetime = NULL;
    char *kblifetime = NULL;
    char *antiReplay = NULL;
    char *dpdEnable = NULL;
    char *dpdheartbeat = NULL;
    char *natt = NULL;
    char *natt_port = NULL;
    char *natt_keepalive = NULL;
    /*get web value*/
    connType = websGetVar(wp, "connType", T(""));
	IPSEC_PRINT("%s-%d: connType=%s\n",__FUNCTION__,__LINE__,connType);
    peer = websGetVar(wp, "APeer", T(""));
	IPSEC_PRINT("peer=%s\n",peer);
    remoteAddr = websGetVar(wp, "ADestAddr", T(""));
	IPSEC_PRINT("remoteAddr=%s\n",remoteAddr);
    remoteMask = websGetVar(wp, "ADestMask", T(""));
	IPSEC_PRINT("remoteMask=%s\n",remoteMask);
    remoteUser = websGetVar(wp, "ARemoteUser", T(""));
	IPSEC_PRINT("remoteUser=%s\n",remoteUser);
    remoteUserType = websGetVar(wp, "ARemoteUserType", T(""));
	IPSEC_PRINT("remoteUserType=%s\n",remoteUserType);
    localBind = websGetVar(wp, "ALocalBinding", T(""));
	IPSEC_PRINT("localBind=%s\n",localBind);
    localAddr = websGetVar(wp, "ASrcAddr", T(""));
	IPSEC_PRINT("localAddr=%s\n",localAddr);
    localMask = websGetVar(wp, "ASrcMask", T(""));
	IPSEC_PRINT("localMask=%s\n",localMask);
    localUser = websGetVar(wp, "ALocalUser", T(""));
	IPSEC_PRINT("localUser=%s\n",localUser);
    localUserType = websGetVar(wp, "ALocalUserType", T(""));
	IPSEC_PRINT("localUserType=%s\n",localUserType);
    preshareKey = websGetVar(wp, "APresharedKey", T(""));
	IPSEC_PRINT("presharekey=%s\n",preshareKey);
    transform1 = websGetVar(wp, "ATransformset1", T(""));
	IPSEC_PRINT("transform1=%s\n",transform1);
    transform2 = websGetVar(wp, "ATransformset2", T(""));
	IPSEC_PRINT("transform2=%s\n",transform2);
    transform3 = websGetVar(wp, "ATransformset3", T(""));
	IPSEC_PRINT("transform3=%s\n",transform3);
    transform4 = websGetVar(wp, "ATransformset4", T(""));
	IPSEC_PRINT("transform4=%s\n",transform4);
    protocol = websGetVar(wp, "AProtocol", T(""));
	IPSEC_PRINT("protocol=%s\n",protocol);
    srcPort = websGetVar(wp, "AsrcPort", T(""));
	IPSEC_PRINT("srcPort=%s\n",srcPort);
    destPort = websGetVar(wp, "AdestPort", T(""));
	IPSEC_PRINT("destPort=%s\n",destPort);
    negMode = websGetVar(wp, "ANegMode", T(""));
	IPSEC_PRINT("negMode=%s\n",negMode);
    aggresEncrypt = websGetVar(wp, "AAggresEncrypt", T(""));
	IPSEC_PRINT("aggresEncrypt=%s\n",aggresEncrypt);
    isakmpLifetime = websGetVar(wp, "AIsakmpLifetime", T(""));
	IPSEC_PRINT("isakmplifetime=%s\n",isakmpLifetime);
    policy1 = websGetVar(wp, "APolicy1", T(""));
	IPSEC_PRINT("policy1=%s\n",policy1);
    policy2 = websGetVar(wp, "APolicy2", T(""));
	IPSEC_PRINT("policy2=%s\n",policy2);
    policy3 = websGetVar(wp, "APolicy3", T(""));
	IPSEC_PRINT("policy3=%s\n",policy3);
    policy4 = websGetVar(wp, "APolicy4", T(""));
	IPSEC_PRINT("policy4=%s\n",policy4);
    lifetime = websGetVar(wp, "ALifetime", T(""));
	IPSEC_PRINT("lifetime=%s\n",lifetime);
    kblifetime = websGetVar(wp, "AKBLifetime", T(""));
	IPSEC_PRINT("kblifetime=%s\n",kblifetime);
    antiReplay = websGetVar(wp, "AAntiReplay", T(""));
	IPSEC_PRINT("antiReplay=%s\n",antiReplay);
    dpdEnable = websGetVar(wp, "ADpdEnabled", T(""));
	IPSEC_PRINT("dpdEnable=%s\n",dpdEnable);
    dpdheartbeat = websGetVar(wp, "ADpdHeartBeat", T(""));
	IPSEC_PRINT("dpdheartbeat=%s\n",dpdheartbeat);
    natt = websGetVar(wp, "ANatT", T(""));
	IPSEC_PRINT("natt=%s\n",natt);
    natt_port = websGetVar(wp, "NatTPortNum", T(""));
	IPSEC_PRINT("natt_port=%s\n",natt_port);
    natt_keepalive= websGetVar(wp, "ANatKeepalive", T(""));
	IPSEC_PRINT("%s-%d: natt_keepalive=%s\n",__FUNCTION__,__LINE__,natt_keepalive);
#if (VPN_ITEM_CONTROL == FYES)
    if(checkVpnFreeItems() == 0) {
	errmsg = getTransDataForC(C_LANG_INDEX_MAX_VPN_ITEM);
	setTheErrorMsg(errmsg);
	return;
    }
#endif
    /* get inst map */
    webIndexInst = getWebInstName(&(nvramProfile->ipsecInstRec.instRecMap), &(nvramProfile->ipsecInstRec.instNameIndex));
    IPSEC_PRINT("%s-%d:webIndexInst=%d\n",__FUNCTION__,__LINE__,webIndexInst);
    if(0 == webIndexInst)
    {
	setTheErrorMsg(T("error."));
	return;
    }
    memset(instId,0,sizeof(instId));
    snprintf(instId, UTT_INST_NAME_LEN, "ID%d", webIndexInst);	/* set inst name */
    IPSEC_PRINT("%s-%d: webIndexInst=%d, instId=%s\n",__FUNCTION__,__LINE__,webIndexInst,instId);
    /* check inst exist or not */
    if(ProfGetInstIndexByName(mibipsec, instId) != PROFFAIL)
    {
	errmsg = getTransDataForC(C_LANG_INDEX_CASE_ALREADY_EXITS);
	setTheErrorMsg(errmsg);
	return;
    }
    /* create new inst */
    index = ProfNewInst(mibipsec, instId, FALSE);
    IPSEC_PRINT("%s-%d:index=%d\n",__FUNCTION__,__LINE__,index);
    if (PROFFAIL == index)
    {
	clearPortMap(&(nvramProfile->ipsecInstRec.instRecMap), webIndexInst);
	errmsg = getTransDataForC(C_LANG_INDEX_CANNOT_CREATE_INST);
	setTheErrorMsg(errmsg);
	return;
    }
    profAction=PROF_CHANGE_ADD;		/* set change type */
    ipsecProf = (ipsecConfProfile *)ProfGetInstPointByIndex(mibipsec, index);	/*get profile pointer*/
    ProfBackupByIndex(mibipsec, profAction, index, &profhead);	    /* backup profile */
    if((ipsecProf!=NULL)&&(ProfInstIsFree(ipsecProf) == 0))	    /* set profile value */
    {
	IPSEC_PRINT("%s-%d:\n",__FUNCTION__,__LINE__);
	ipsecProf->active=FUN_ENABLE;
	/* set connect type */
	if(strcmp(connType, "1")==0)
	{
	    ipsecProf->connType = GATEWAY_TO_GATEWAY;
	}
	else if(strcmp(connType, "2")==0)
	{
	    ipsecProf->connType = DYNAMIC_TO_GATEWAY;
	}
	else if(strcmp(connType, "3")==0)
	{  
	    ipsecProf->connType = GATEWAY_FROM_DYNAMIC;
	    strcpy(ipsecProf->peer, "0.0.0.0");
	}
	IPSEC_PRINT("%s-%d:head.name=%s,active=%d,connType=%d\n",__FUNCTION__,__LINE__,ipsecProf->head.name,ipsecProf->active,ipsecProf->connType); 
	if(strlen(peer)!=0)
	{
	    strcpy(ipsecProf->peer,peer);
	    IPSEC_PRINT("%s-%d:\n",__FUNCTION__,__LINE__);
	}
	memset(domainIP,0,sizeof(domainIP));
	getDomainNameIP(peer,domainIP);
	memset(ipsecProf->peerIP,0,sizeof(ipsecProf->peerIP));
	if(strlen(domainIP)!=0)
	{
	    strcpy(ipsecProf->peerIP,domainIP);
	}
	strcpy(ipsecProf->destAddr,remoteAddr);
	strcpy(ipsecProf->destMask,remoteMask);
	strcpy(ipsecProf->profile,localBind);	/*need modify*/
	strcpy(ipsecProf->srcAddr,localAddr);
	strcpy(ipsecProf->srcMask,localMask);
	ipsecProf->protocol = (Uint32)strtoul(protocol, 0, 10);
	ipsecProf->srcPort = (Uint32)strtoul(srcPort, 0, 10);
	ipsecProf->destPort = (Uint32)strtoul(destPort, 0, 10);
	ipsecProf->lifetime_sec = (Uint32)strtoul(lifetime, 0, 10);
	ipsecProf->lifetime_kbytes = (Uint32)strtoul(kblifetime, 0, 10);
	IPSEC_PRINT("%s-%d: peer=%s,peerIP=%s,destAddr=%s,destMask=%s,localBind=%s,localAddr=%s,localMask=%s\n",__func__,__LINE__,ipsecProf->peer,ipsecProf->peerIP,ipsecProf->destAddr,ipsecProf->destMask,ipsecProf->profile,ipsecProf->srcAddr,ipsecProf->srcMask);
	IPSEC_PRINT("%s-%d: protocol=%d,srcPort=%d,destPort=%d,lifetime_sec=%d,lifetime_kbytes=%d\n",__func__,__LINE__,ipsecProf->protocol,ipsecProf->srcPort,ipsecProf->destPort,ipsecProf->lifetime_sec,ipsecProf->lifetime_kbytes);
	
#if 1
	/* 设置第二阶段加密认证 */
	setipsecTransformSet(transform1);
	setipsecTransformSet(transform2);
	setipsecTransformSet(transform3);
	setipsecTransformSet(transform4);
#endif	
	memset(trans_form, 0, sizeof(trans_form));
	strcpy(trans_form,"tra_");
	strcat(trans_form,transform1);
	strcat(trans_form,":tra_");
	strcat(trans_form,transform2);
	strcat(trans_form,":tra_");
	strcat(trans_form,transform3);
	strcat(trans_form,":tra_");
	strcat(trans_form,transform4);
	strcpy(ipsecProf->transformSet, trans_form);
	IPSEC_PRINT("%s-%d: trans_form=%s,ipsecProf->transformSet=%s\n",__func__,__LINE__,trans_form,ipsecProf->transformSet);
	/* 抗重播 */
	if(strcmp(antiReplay, "on")==0)
	{
	    ipsecProf->anti_replay = FUN_ENABLE;
	}
	else
	{
	    ipsecProf->anti_replay = FUN_DISABLE;
	}
	/*DPD*/
	if(strcmp(dpdEnable, "on")==0)
	{
	    ipsecProf->dpd_enable = FUN_ENABLE;
	}
	else
	{
	    ipsecProf->dpd_enable = FUN_DISABLE;
	}
	/*心跳*/
	ipsecProf->dpd_heartbeat = (Uint32)strtoul(dpdheartbeat, 0, 10);
	/* NAT 穿透 */
	if(strcmp(natt, "on")==0)
	{
	    ipsecProf->nat_enable = FUN_ENABLE;
	}
	else
	{
	    ipsecProf->nat_enable = FUN_DISABLE;
	}
	/* 端口 及 维持秒数 */
	ipsecProf->nat_port = (Uint32)strtoul(natt_port, 0, 10);
	ipsecProf->nat_keepalive = (Uint32)strtoul(natt_keepalive, 0, 10);
	IPSEC_PRINT("%s-%d: antiReplay=%d,dpd_enable=%d,dpd_heartbeat=%d,natt=%d,natt=port=%d,natt-keepalive=%d\n",__func__,__LINE__,ipsecProf->anti_replay,ipsecProf->dpd_enable,ipsecProf->dpd_heartbeat,ipsecProf->nat_enable,ipsecProf->nat_port,ipsecProf->nat_keepalive);
	
	memset(isakmpId, 0, sizeof(isakmpId));
	strcpy(isakmpId, instId);
	strcat(isakmpId, "A");		/*设置isakmp实例名*/
	strcpy(ipsecProf->isakmpProfile,isakmpId);
	IPSEC_PRINT("%s-%d: ipsecProf->isakmpProfile=%s\n",__func__,__LINE__,ipsecProf->isakmpProfile);
	/*check exist or not*/
	if(ProfGetInstIndexByName(mibisakmp, isakmpId) != PROFFAIL)
	{
	    errmsg = getTransDataForC(C_LANG_INDEX_CASE_ALREADY_EXITS);
	    setTheErrorMsg(errmsg);
	    return;
	}
	isakmpindex = ProfNewInst(mibisakmp, isakmpId, FALSE);
	IPSEC_PRINT("%s-%d: isakmpindex=%d, isakmpId=%s\n",__func__,__LINE__,isakmpindex,isakmpId);
	if (PROFFAIL == isakmpindex)
	{
	    errmsg = getTransDataForC(C_LANG_INDEX_CANNOT_CREATE_INST);
	    setTheErrorMsg(errmsg);
	    return;
	}
	isakmpProf = (isakmpConfProfile *)ProfGetInstPointByIndex(mibisakmp, isakmpindex);
	if((isakmpProf!=NULL)&&(ProfInstIsFree(isakmpProf) == 0))
	{
	    strcpy(isakmpProf->remoteuser,remoteUser);	    /*设置远端身份*/
	    /*设置远端身份类型*/
	    if(strcmp(remoteUserType,"IKE_FQDN")==0)
	    {
		isakmpProf->remoteType=IKE_FQDN;
	    }
	    else if(strcmp(remoteUserType,"IKE_USERFQDN")==0)
	    {
		isakmpProf->remoteType=IKE_USERFQDN;
	    }
	    else if(strcmp(remoteUserType,"IKE_KEYID")==0)
	    {
		isakmpProf->remoteType=IKE_KEYID;
	    }
	    else if(strcmp(remoteUserType,"IKE_IPV4ADDR")==0)
	    {
		isakmpProf->remoteType=IKE_IPV4ADDR;
	    }
	    strcpy(isakmpProf->localuser,localUser);	    /*设置本地身份*/
	    /*设置本地身份类型*/
	    if(strcmp(localUserType,"IKE_FQDN")==0)
	    {
		isakmpProf->localType=IKE_FQDN;
	    }
	    else if(strcmp(localUserType,"IKE_USERFQDN")==0)
	    {
		isakmpProf->localType=IKE_USERFQDN;
	    }
	    else if(strcmp(localUserType,"IKE_KEYID")==0)
	    {
		isakmpProf->localType=IKE_KEYID;
	    }
	    else if(strcmp(localUserType,"IKE_IPV4ADDR")==0)
	    {
		isakmpProf->localType=IKE_IPV4ADDR;
	    }
	    strcpy(isakmpProf->preshareKey,preshareKey);    /*预共享密钥*/
	    /*协商模式*/
	    if(strcmp(negMode, "Main")==0)
	    {
		isakmpProf->mode = Mainmode;
	    }
	    else
	    {
		isakmpProf->mode = Aggresmode;
	    }
	    if(isakmpProf->mode == Aggresmode)
	    {
		/*野蛮模式协商加密*/
		if(strcmp(aggresEncrypt, "on")==0)
		{
		    isakmpProf->aggresEncrypt = FUN_ENABLE;
		}
		else
		{
		    isakmpProf->aggresEncrypt = FUN_DISABLE;
		}
	    }
	    isakmpProf->lifetime = (Uint32)strtoul(isakmpLifetime, 0, 10);	/*第一阶段生存时间*/
	    IPSEC_PRINT("%s-%d: isakmpProfile=%s,remoteuser=%s,remoteType=%d,localuser=%s,localType=%d,preshareKey=%s,negMode=%d,aggresEncrypt=%d,lifetime=%d\n",__func__,__LINE__,ipsecProf->isakmpProfile,isakmpProf->remoteuser,isakmpProf->remoteType,isakmpProf->localuser,isakmpProf->localType,isakmpProf->preshareKey,isakmpProf->mode,isakmpProf->aggresEncrypt,isakmpProf->lifetime);
#if 1
	    /*设置第一阶段加密认证*/
	    setisakmpPolicyValue(policy1);
	    setisakmpPolicyValue(policy2);
	    setisakmpPolicyValue(policy3);
	    setisakmpPolicyValue(policy4);
#endif
	    memset(policyStr, 0, sizeof(policyStr));
	    strcpy(policyStr,"p_");
	    strcat(policyStr,policy1);
	    strcat(policyStr,":p_");
	    strcat(policyStr,policy2);
	    strcat(policyStr,":p_");
	    strcat(policyStr,policy3);
	    strcat(policyStr,":p_");
	    strcat(policyStr,policy4);
	    strcpy(isakmpProf->policy, policyStr);
	    IPSEC_PRINT("%s-%d: policyStr=%s,isakmpProf->policy=%s\n",__func__,__LINE__,policyStr,isakmpProf->policy);
	}
	//strcpy(ipsecProf->,);
    }

    /* update profile inst
     * and save profile
     */
    if(profhead != NULL)
    {
	ProfUpdate(profhead);		/*update*/
	IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
	ProfFreeAllocList(profhead);	/*release*/
	IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
    }
#if 1
    nvramWriteCommit();			/*save*/
#endif
    return;
}

/*
 * isakmpConfigModify
 *
 * modify profile config
 *
 * invoke by formIPSecIsakmp_Config
 */
static void isakmpConfigModify(webs_t wp)
{
    MibProfileType mibipsec = MIB_PROF_IPSEC;
    MibProfileType mibisakmp = MIB_PROF_ISAKMP;
    ipsecConfProfile *ipsecProf = NULL;
    isakmpConfProfile *isakmpProf = NULL;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction = PROF_CHANGE_EDIT;
    int index=-1;
    int isakmpindex=-1;
    char isakmpId[UTT_INST_NAME_LEN + 1];
    char trans_form[MAX_TRANSFORMSETS_LENGTH+1];
    char policyStr[MAX_ISAKMPPOLICY_LENGTH+1];
    char domainIP[MAX_IPSEC_IP_LEN+1]={0};
    const char *errmsg=NULL;
    /*define varliable*/
    char *ipsecID = NULL;
    char *connType = NULL;
    char *peer = NULL;
    char *remoteAddr = NULL;
    char *remoteMask = NULL;
    char *remoteUser = NULL;
    char *remoteUserType = NULL;
    char *localBind = NULL;
    char *localAddr = NULL;
    char *localMask = NULL;
    char *localUser = NULL;
    char *localUserType = NULL;
    char *preshareKey = NULL;
    char *transform1=NULL,*transform2=NULL,*transform3=NULL,*transform4=NULL;
    char *policy1=NULL,*policy2=NULL,*policy3=NULL,*policy4=NULL;
    char *protocol = NULL;
    char *srcPort = NULL, *destPort = NULL;
    char *negMode = NULL;
    char *aggresEncrypt = NULL;
    char *isakmpLifetime = NULL;
    char *lifetime = NULL;
    char *kblifetime = NULL;
    char *antiReplay = NULL;
    char *dpdEnable = NULL;
    char *dpdheartbeat = NULL;
    char *natt = NULL;
    char *natt_port = NULL;
    char *natt_keepalive = NULL;
    /*get web value*/
    ipsecID = websGetVar(wp, "instID", T(""));
    connType = websGetVar(wp, "connType", T(""));
	IPSEC_PRINT("%s-%d: connType=%s\n",__FUNCTION__,__LINE__,connType);
    peer = websGetVar(wp, "APeer", T(""));
	IPSEC_PRINT("peer=%s\n",peer);
    remoteAddr = websGetVar(wp, "ADestAddr", T(""));
	IPSEC_PRINT("remoteAddr=%s\n",remoteAddr);
    remoteMask = websGetVar(wp, "ADestMask", T(""));
	IPSEC_PRINT("remoteMask=%s\n",remoteMask);
    remoteUser = websGetVar(wp, "ARemoteUser", T(""));
	IPSEC_PRINT("remoteUser=%s\n",remoteUser);
    remoteUserType = websGetVar(wp, "ARemoteUserType", T(""));
	IPSEC_PRINT("remoteUserType=%s\n",remoteUserType);
    localBind = websGetVar(wp, "ALocalBinding", T(""));
	IPSEC_PRINT("localBind=%s\n",localBind);
    localAddr = websGetVar(wp, "ASrcAddr", T(""));
	IPSEC_PRINT("localAddr=%s\n",localAddr);
    localMask = websGetVar(wp, "ASrcMask", T(""));
	IPSEC_PRINT("localMask=%s\n",localMask);
    localUser = websGetVar(wp, "ALocalUser", T(""));
	IPSEC_PRINT("localUser=%s\n",localUser);
    localUserType = websGetVar(wp, "ALocalUserType", T(""));
	IPSEC_PRINT("localUserType=%s\n",localUserType);
    preshareKey = websGetVar(wp, "APresharedKey", T(""));
	IPSEC_PRINT("presharekey=%s\n",preshareKey);
    transform1 = websGetVar(wp, "ATransformset1", T(""));
	IPSEC_PRINT("transform1=%s\n",transform1);
    transform2 = websGetVar(wp, "ATransformset2", T(""));
	IPSEC_PRINT("transform2=%s\n",transform2);
    transform3 = websGetVar(wp, "ATransformset3", T(""));
	IPSEC_PRINT("transform3=%s\n",transform3);
    transform4 = websGetVar(wp, "ATransformset4", T(""));
	IPSEC_PRINT("transform4=%s\n",transform4);
    protocol = websGetVar(wp, "AProtocol", T(""));
	IPSEC_PRINT("protocol=%s\n",protocol);
    srcPort = websGetVar(wp, "AsrcPort", T(""));
	IPSEC_PRINT("srcPort=%s\n",srcPort);
    destPort = websGetVar(wp, "AdestPort", T(""));
	IPSEC_PRINT("destPort=%s\n",destPort);
    negMode = websGetVar(wp, "ANegMode", T(""));
	IPSEC_PRINT("negMode=%s\n",negMode);
    aggresEncrypt = websGetVar(wp, "AAggresEncrypt", T(""));
	IPSEC_PRINT("aggresEncrypt=%s\n",aggresEncrypt);
    isakmpLifetime = websGetVar(wp, "AIsakmpLifetime", T(""));
	IPSEC_PRINT("isakmplifetime=%s\n",isakmpLifetime);
    policy1 = websGetVar(wp, "APolicy1", T(""));
	IPSEC_PRINT("policy1=%s\n",policy1);
    policy2 = websGetVar(wp, "APolicy2", T(""));
	IPSEC_PRINT("policy2=%s\n",policy2);
    policy3 = websGetVar(wp, "APolicy3", T(""));
	IPSEC_PRINT("policy3=%s\n",policy3);
    policy4 = websGetVar(wp, "APolicy4", T(""));
	IPSEC_PRINT("policy4=%s\n",policy4);
    lifetime = websGetVar(wp, "ALifetime", T(""));
	IPSEC_PRINT("lifetime=%s\n",lifetime);
    kblifetime = websGetVar(wp, "AKBLifetime", T(""));
	IPSEC_PRINT("kblifetime=%s\n",kblifetime);
    antiReplay = websGetVar(wp, "AAntiReplay", T(""));
	IPSEC_PRINT("antiReplay=%s\n",antiReplay);
    dpdEnable = websGetVar(wp, "ADpdEnabled", T(""));
	IPSEC_PRINT("dpdEnable=%s\n",dpdEnable);
    dpdheartbeat = websGetVar(wp, "ADpdHeartBeat", T(""));
	IPSEC_PRINT("dpdheartbeat=%s\n",dpdheartbeat);
    natt = websGetVar(wp, "ANatT", T(""));
	IPSEC_PRINT("natt=%s\n",natt);
    natt_port = websGetVar(wp, "NatTPortNum", T(""));
	IPSEC_PRINT("natt_port=%s\n",natt_port);
    natt_keepalive= websGetVar(wp, "ANatKeepalive", T(""));
	IPSEC_PRINT("%s-%d: natt_keepalive=%s\n",__FUNCTION__,__LINE__,natt_keepalive);
    /*get inst index*/
    index = ProfGetInstIndexByName(mibipsec, ipsecID);
    IPSEC_PRINT("%s-%d:index=%d\n",__FUNCTION__,__LINE__,index);
    /*check inst exist or not */
    if (PROFFAIL == index)
    {
	errmsg = getTransDataForC(C_LANG_INDEX_CHANGED_CASE_NOT_EXITS);
	setTheErrorMsg(errmsg);
	websRedirect(wp, "IPSec.asp");
	return;
    }
    profAction=PROF_CHANGE_EDIT;    /*set change type*/
    ipsecProf = (ipsecConfProfile *)ProfGetInstPointByIndex(mibipsec, index);	/*get pointer*/
    ProfBackupByIndex(mibipsec, profAction, index, &profhead);			/*backup profile*/
    IPSEC_PRINT("%s-%d:head.name=%s\n",__FUNCTION__,__LINE__,ipsecProf->head.name); 
    if((ipsecProf!=NULL)&&(ProfInstIsFree(ipsecProf) == 0))
    {
	/* set connect type */
	if(strcmp(connType, "1")==0)
	{
	    ipsecProf->connType = GATEWAY_TO_GATEWAY;
	}
	else if(strcmp(connType, "2")==0)
	{
	    ipsecProf->connType = DYNAMIC_TO_GATEWAY;
	}
	else if(strcmp(connType, "3")==0)
	{  
	    ipsecProf->connType = GATEWAY_FROM_DYNAMIC;
	}
	strcpy(ipsecProf->peer,peer);
	memset(domainIP,0,sizeof(domainIP));
	getDomainNameIP(peer,domainIP);
	memset(ipsecProf->peerIP,0,sizeof(ipsecProf->peerIP));
	if(strlen(domainIP)!=0)
	{
	    IPSEC_PRINT("%s-%d:\n",__FUNCTION__,__LINE__); 
	    strcpy(ipsecProf->peerIP,domainIP);
	}
	strcpy(ipsecProf->destAddr,remoteAddr);
	strcpy(ipsecProf->destMask,remoteMask);
	strcpy(ipsecProf->profile,localBind);	/*need modify*/
	strcpy(ipsecProf->srcAddr,localAddr);
	strcpy(ipsecProf->srcMask,localMask);
	ipsecProf->protocol = (Uint32)strtoul(protocol, 0, 10);
	ipsecProf->srcPort = (Uint32)strtoul(srcPort, 0, 10);
	ipsecProf->destPort = (Uint32)strtoul(destPort, 0, 10);
	ipsecProf->lifetime_sec = (Uint32)strtoul(lifetime, 0, 10);
	ipsecProf->lifetime_kbytes = (Uint32)strtoul(kblifetime, 0, 10);
	IPSEC_PRINT("%s-%d: peer=%s,peerIP=%s,destAddr=%s,destMask=%s,localBind=%s,localAddr=%s,localMask=%s\n",__func__,__LINE__,ipsecProf->peer,ipsecProf->peerIP,ipsecProf->destAddr,ipsecProf->destMask,ipsecProf->profile,ipsecProf->srcAddr,ipsecProf->srcMask);
	IPSEC_PRINT("%s-%d: protocol=%d,srcPort=%d,destPort=%d,lifetime_sec=%d,lifetime_kbytes=%d\n",__func__,__LINE__,ipsecProf->protocol,ipsecProf->srcPort,ipsecProf->destPort,ipsecProf->lifetime_sec,ipsecProf->lifetime_kbytes);
	
#if 1
	/*设置第二阶段加密认证*/
	setipsecTransformSet(transform1);
	setipsecTransformSet(transform2);
	setipsecTransformSet(transform3);
	setipsecTransformSet(transform4);
#endif	
	memset(trans_form, 0, sizeof(trans_form));
	strcpy(trans_form,"tra_");
	strcat(trans_form,transform1);
	strcat(trans_form,":tra_");
	strcat(trans_form,transform2);
	strcat(trans_form,":tra_");
	strcat(trans_form,transform3);
	strcat(trans_form,":tra_");
	strcat(trans_form,transform4);
	strcpy(ipsecProf->transformSet, trans_form);
	IPSEC_PRINT("%s-%d: trans_form=%s,ipsecProf->transformSet=%s\n",__func__,__LINE__,trans_form,ipsecProf->transformSet);
	/*抗重播*/
	if(strcmp(antiReplay, "on")==0)
	{
	    ipsecProf->anti_replay = FUN_ENABLE;
	}
	else
	{
	    ipsecProf->anti_replay = FUN_DISABLE;
	}
	/* DPD */
	if(strcmp(dpdEnable, "on")==0)
	{
	    ipsecProf->dpd_enable = FUN_ENABLE;
	}
	else
	{
	    ipsecProf->dpd_enable = FUN_DISABLE;
	}
	/* 心跳 */
	ipsecProf->dpd_heartbeat = (Uint32)strtoul(dpdheartbeat, 0, 10);
	/* NAT 穿透 */
	if(strcmp(natt, "on")==0)
	{
	    ipsecProf->nat_enable = FUN_ENABLE;
	}
	else
	{
	    ipsecProf->nat_enable = FUN_DISABLE;
	}
	/* 端口 及 维持秒数 */
	ipsecProf->nat_port = (Uint32)strtoul(natt_port, 0, 10);
	ipsecProf->nat_keepalive = (Uint32)strtoul(natt_keepalive, 0, 10);
	IPSEC_PRINT("%s-%d: antiReplay=%d,dpd_enable=%d,dpd_heartbeat=%d,natt=%d,natt=port=%d,natt-keepalive=%d\n",__func__,__LINE__,ipsecProf->anti_replay,ipsecProf->dpd_enable,ipsecProf->dpd_heartbeat,ipsecProf->nat_enable,ipsecProf->nat_port,ipsecProf->nat_keepalive);
#if 0	
#endif
	IPSEC_PRINT("%s-%d: ipsecProf->isakmpProfile=%s\n",__func__,__LINE__,ipsecProf->isakmpProfile);
	isakmpindex = ProfGetInstIndexByName(mibisakmp, ipsecProf->isakmpProfile);	    /* get isakmp profile index */
	IPSEC_PRINT("%s-%d: isakmpindex=%d, isakmpId=%s\n",__func__,__LINE__,isakmpindex,isakmpId);
	isakmpProf = (isakmpConfProfile *)ProfGetInstPointByIndex(mibisakmp, isakmpindex);  /* get pointer */
	if((isakmpProf!=NULL)&&(ProfInstIsFree(isakmpProf) == 0))
	{
	    strcpy(isakmpProf->remoteuser,remoteUser);	    /*设置远端身份*/
	    /* 设置远端身份类型 */
	    if(strcmp(remoteUserType,"IKE_FQDN")==0)
	    {
		isakmpProf->remoteType=IKE_FQDN;
	    }
	    else if(strcmp(remoteUserType,"IKE_USERFQDN")==0)
	    {
		isakmpProf->remoteType=IKE_USERFQDN;
	    }
	    else if(strcmp(remoteUserType,"IKE_KEYID")==0)
	    {
		isakmpProf->remoteType=IKE_KEYID;
	    }
	    else if(strcmp(remoteUserType,"IKE_IPV4ADDR")==0)
	    {
		isakmpProf->remoteType=IKE_IPV4ADDR;
	    }
	    strcpy(isakmpProf->localuser,localUser);	/*设置本地身份*/
	    /* 设置本地身份类型 */
	    if(strcmp(localUserType,"IKE_FQDN")==0)
	    {
		isakmpProf->localType=IKE_FQDN;
	    }
	    else if(strcmp(localUserType,"IKE_USERFQDN")==0)
	    {
		isakmpProf->localType=IKE_USERFQDN;
	    }
	    else if(strcmp(localUserType,"IKE_KEYID")==0)
	    {
		isakmpProf->localType=IKE_KEYID;
	    }
	    else if(strcmp(localUserType,"IKE_IPV4ADDR")==0)
	    {
		isakmpProf->localType=IKE_IPV4ADDR;
	    }
	    strcpy(isakmpProf->preshareKey,preshareKey);    /* 设置预共享密钥 */
	    /* 设置协商模式 */
	    if(strcmp(negMode, "Main")==0)
	    {
		isakmpProf->mode = Mainmode;
	    }
	    else
	    {
		isakmpProf->mode = Aggresmode;
	    }
	    if(isakmpProf->mode == Aggresmode)
	    {
		/* 野蛮模式协商加密 */
		if(strcmp(aggresEncrypt, "on")==0)
		{
		    isakmpProf->aggresEncrypt = FUN_ENABLE;
		}
		else
		{
		    isakmpProf->aggresEncrypt = FUN_DISABLE;
		}
	    }
	    isakmpProf->lifetime = (Uint32)strtoul(isakmpLifetime, 0, 10);  /* 第一阶段生存时间 */
	    IPSEC_PRINT("%s-%d: isakmpProfile=%s,remoteuser=%s,remoteType=%d,localuser=%s,localType=%d,preshareKey=%s,negMode=%d,aggresEncrypt=%d,lifetime=%d\n",__func__,__LINE__,ipsecProf->isakmpProfile,isakmpProf->remoteuser,isakmpProf->remoteType,isakmpProf->localuser,isakmpProf->localType,isakmpProf->preshareKey,isakmpProf->mode,isakmpProf->aggresEncrypt,isakmpProf->lifetime);
#if 1
	    /* 设置第一阶段加密认证 */
	    setisakmpPolicyValue(policy1);
	    setisakmpPolicyValue(policy2);
	    setisakmpPolicyValue(policy3);
	    setisakmpPolicyValue(policy4);
#endif
	    memset(policyStr, 0, sizeof(policyStr));
	    strcpy(policyStr,"p_");
	    strcat(policyStr,policy1);
	    strcat(policyStr,":p_");
	    strcat(policyStr,policy2);
	    strcat(policyStr,":p_");
	    strcat(policyStr,policy3);
	    strcat(policyStr,":p_");
	    strcat(policyStr,policy4);
	    strcpy(isakmpProf->policy, policyStr);
	    IPSEC_PRINT("%s-%d: policyStr=%s,isakmpProf->policy=%s\n",__func__,__LINE__,policyStr,isakmpProf->policy);
	}
	//strcpy(ipsecProf->,);
    }
    if(ipsecProf->flag==0)
	ipsecProf->flag=1;
    else
	ipsecProf->flag=0;

    /* update profile inst
     * and save profile
     */
    if(profhead != NULL)
    {
	ProfUpdate(profhead);		/*update*/
	ProfFreeAllocList(profhead);	/*release*/
	IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
    }
#if 1
    nvramWriteCommit();			/*save*/
#endif
    return;
}

/*
 * formIPSecIsakmp_Config
 * /goform/IPSecIsakmp_Config
 * 添加或修改配置
 */
static void  formIPSecIsakmp_Config(webs_t wp, char_t *path, char_t *query)
{
    char *action = NULL;

    action =  websGetVar(wp, "Action", T(""));	/* Add or Modify */
    IPSEC_PRINT("%s-%d: action=%s\n",__FUNCTION__,__LINE__,action);
    
    if(strcmp(action,"add")==0)
    {
	isakmpConfigAdd(wp);	    /* add profile config */
    }
    else
    {
	isakmpConfigModify(wp);	    /* modify profile config */
    }
    websRedirect(wp, "IPSec.asp");
    return;
}

static void clearisakmpProf(char *Prof)
{
    MibProfileType mibisakmp = MIB_PROF_ISAKMP;
    isakmpConfProfile *isakmpProf = NULL;
#if 0
    struProfAlloc *profList = NULL;
#endif
    if((NULL==Prof)||(0==strlen(Prof)))  return;
    /*back inst and delete*/
#if 0
    ProfBackupByName(mibipsec, PROF_CHANGE_DEL, tmp, &profList);
#endif
    isakmpProf = (isakmpConfProfile *)ProfGetInstPointByName(mibisakmp, Prof);	    /*get pointer*/
    IPSEC_PRINT("%s-%d: Prof=%s, ID=%s\n",__FUNCTION__,__LINE__,Prof,isakmpProf->head.name);
    if(isakmpProf!=NULL)
    {
		/*delete inst by name*/
		IPSEC_PRINT("%s-%d:\n",__FUNCTION__,__LINE__);
		ProfDelInstByName(mibisakmp, Prof);
    }
	/*update profile and save*/
#if 0
	ProfUpdate(profList);
	ProfFreeAllocList(profList);
#endif
	//nvramWriteCommit();	/*save*/

    return;
}
static void clearAllTransformSet()
{
    MibProfileType mibtransformset = MIB_PROF_TRANSFORMSET;
    ipsecTransformSetProfile *ipsecTransProf = NULL;
    int max=0, min=0, i=0;
    a_assert(wp);
    /*get inst max value and min value*/
    ProfInstLowHigh(mibtransformset, &max, &min);
    IPSEC_PRINT("%s-%d: max=%d, min=%d\n",__FUNCTION__,__LINE__,max,min);
    /* delete all inst
     * from min index to max 
     */
    for(i=min;i<max;i++)
    {
	ipsecTransProf = (ipsecTransformSetProfile *)ProfGetInstPointByIndex(mibtransformset, i);
	if((ipsecTransProf!=NULL) && (ProfInstIsFree(ipsecTransProf) == 0))
	{
	    IPSEC_PRINT("%s-%d: \n",__FUNCTION__,__LINE__);
	    /*delete inst*/
	    ProfDelInstByIndex(mibtransformset, i);
	}
    }
    //nvramWriteCommit();		    /*save*/
    return;
}
static void clearAllisakmpPolicyProf()
{
    MibProfileType mibisakmppolicy = MIB_PROF_ISAKMP_POLICY;
    isakmpPolicyProfile *isakmpPolicyProf = NULL;
    int max=0, min=0, i=0;
    a_assert(wp);
    /*get inst max value and min value*/
    ProfInstLowHigh(mibisakmppolicy, &max, &min);
    IPSEC_PRINT("%s-%d: max=%d, min=%d\n",__FUNCTION__,__LINE__,max,min);
    /* delete all inst
     * from min index to max 
     */
    for(i=min;i<max;i++)
    {
	isakmpPolicyProf = (isakmpPolicyProfile *)ProfGetInstPointByIndex(mibisakmppolicy, i);
	if((isakmpPolicyProf!=NULL) && (ProfInstIsFree(isakmpPolicyProf) == 0))
	{
	    IPSEC_PRINT("%s-%d: \n",__FUNCTION__,__LINE__);
	    /*delete inst*/
	    ProfDelInstByIndex(mibisakmppolicy, i);
	}
    }

    //nvramWriteCommit();		    /*save*/
    return;
}

/*
 * formIPSecList_Del
 * /goform/IPSecList_Del
 * delete isamp profile
 */
static void  formIPSecList_Del(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibipsec = MIB_PROF_IPSEC;
    ipsecConfProfile *ipsecProf = NULL;
    struProfAlloc *profList = NULL;
    
    char *indexID=NULL;
    char *tmp=NULL;
    const char *errmsg=NULL;
    Uint16 webIndex = 0U;

    /*get delete inst name*/
    indexID = websGetVar(wp, T("delstr"),T(""));
    IPSEC_PRINT("%s - %d: del ID: %s\n",__FUNCTION__,__LINE__,indexID);

    if(indexID != NULL)
    {
	/*separata inst name*/
	tmp = strtok(indexID, UTT_SPLIT_SIGN_STR);
	while(tmp!=NULL)
	{
	    /*back inst and delete*/
	    ProfBackupByName(mibipsec, PROF_CHANGE_DEL, tmp, &profList);
	    ipsecProf = (ipsecConfProfile *)ProfGetInstPointByName(mibipsec, tmp);
	    IPSEC_PRINT("%s-%d:tmp=%s, ID=%s\n",__FUNCTION__,__LINE__,tmp,ipsecProf->head.name);
	    if(ipsecProf!=NULL)
	    {
		IPSEC_PRINT("%s-%d:\n",__FUNCTION__,__LINE__);
#if 0
		clearTransformSet(ipsecProf->transformSet);
		clearisakmpPolicyProf(ipsecProf->isakmpProfile);
#endif
		/*delete isakmp profile first*/
		clearisakmpProf(ipsecProf->isakmpProfile);
		/*delete inst by name*/
		ProfDelInstByName(mibipsec, tmp);
		/*need clear port map*/
		if(strncmp(tmp,"ID",2)==0)
		{
		   webIndex = (Uint16)strtol(tmp+2,NULL,10);
		   IPSEC_PRINT("%s-%d: clear webIndex=%d\n",__func__,__LINE__,webIndex);
		   clearPortMap(&(nvramProfile->ipsecInstRec.instRecMap), webIndex);	    /*clear port map*/
		}
	    }
	    /*get the next inst name*/
	    tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
	}


	/*update profile and save*/
	ProfUpdate(profList);
	ProfFreeAllocList(profList);
#if 1
	nvramWriteCommit();	/*save*/
#endif
    }
    else
    {
	errmsg = getTransDataForC(C_LANG_INDEX_DID_NOT_FIND_CASE);
	setTheErrorMsg(errmsg);
    }

    websRedirect(wp, T("IPSec.asp"));
    return;
}

/*
 * formIPSecList_DelAll
 * /goform/IPSecList_DelAll
 * delete all profile
 */
static void  formIPSecList_DelAll(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibipsec = MIB_PROF_IPSEC;
    ipsecConfProfile *ipsecProf = NULL;
    struProfAlloc *profList = NULL;
    int max=0, min=0, i=0;

    a_assert(wp);
    /*get inst max value and min value*/
    ProfInstLowHigh(mibipsec, &max, &min);
    /* delete all inst
     * from min index to max 
     */
    for(i=min;i<max;i++)
    {
	ipsecProf = (ipsecConfProfile *)ProfGetInstPointByIndex(mibipsec, i);
	if((ipsecProf!=NULL) && (ProfInstIsFree(ipsecProf) == 0))
	{
	    ProfBackupByIndex(mibipsec, PROF_CHANGE_DELALL, i, &profList);
	    IPSEC_PRINT("%s-%d: \n",__FUNCTION__,__LINE__);
#if 1
	    clearAllTransformSet();
	    clearAllisakmpPolicyProf();
#endif
	    /*delete isakmp profile first*/
	    clearisakmpProf(ipsecProf->isakmpProfile);
	    /*delete inst*/
	    ProfDelInstByIndex(mibipsec, i);
	}
    }

    /* 更新map记录 */
    clearAllPortMap(&(nvramProfile->ipsecInstRec.instRecMap));
    nvramProfile->ipsecInstRec.instNameIndex = 0;
    ProfUpdate(profList);	    /*update*/
    IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
    ProfFreeAllocList(profList);    /*erlease*/
    IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
#if 1
    nvramWriteCommit();		    /*save*/
#endif
    websRedirect(wp, "IPSec.asp");
    return;
}

/*
 * formConfigIPSecAllow
 * /goform/formIPSecAllow
 * set profile allow or forbidden
 */
static void  formConfigIPSecAllow(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibipsec = MIB_PROF_IPSEC;
    ipsecConfProfile *ipsecProf = NULL;
    FunEnableEnum allowEn = FUN_ENABLE;
    int instIndex = -1;

    char *UserName = NULL;
    char *allow = NULL;
    const char *errmsg = NULL;
    const char *ErrMsg = NULL;

    a_assert(wp);
    UserName=websGetVar(wp,T("AllowID"), T(""));/*用户名*/
    allow=websGetVar(wp,T("Allow"), T(""));/*允许*/
    instIndex = ProfGetInstIndexByName(mibipsec, UserName);
    if(instIndex == PROFFAIL) 
    {/*欲修改的配置不存在*/
	errmsg = getTransDataForC(C_LANG_INDEX_THE_CONFIG_NOT_FIND);
	setTheErrorMsgOnFree(errmsg);
    }
    else
    {	/*要操作的配置存在*/
	IPSEC_PRINT("%s-%d: instIndex=%d\n",__FUNCTION__,__LINE__,instIndex);
	ipsecProf = (ipsecConfProfile *)ProfGetInstPointByIndex(mibipsec, instIndex);
	if((ipsecProf!=NULL) && (ProfInstIsFree(ipsecProf) == 0))
	{
	    if (0 == strcmp("no",allow))/*禁止该绑定用户*/
	    {
		allowEn = FUN_DISABLE;
	    }
	    IPSEC_PRINT("%s-%d: allow=%s,allowEn=%d\n",__FUNCTION__,__LINE__,allow,allowEn);
	    /* modify  */
	    if(ipsecProf->active != allowEn)
	    {
		ipsecProf->active = allowEn;
		IPSEC_PRINT("%s-%d: active=%d\n",__FUNCTION__,__LINE__,ipsecProf->active);
#if 1
		if(ipsecProf->active == FUN_ENABLE)
		{
#if(VPN_CONTROL == FYES)
		    if(nvramProfile->vpnProfile.count<=0)
		    {
			ErrMsg = getTransDataForC(C_LANG_INDEX_MAX_VPN_COUNT);
			setTheErrorMsg(ErrMsg);
			goto out;
		    }
#endif
		    doSystem("ipsec auto --down \"%s\"",ipsecProf->head.name);
		    doSystem("ipsec auto --add \"%s\"",ipsecProf->head.name);
		    doSystem("ipsec auto --rereadsecrets");
		    doSystem("ipsec auto --up \"%s\" &",ipsecProf->head.name);
		    IPSEC_PRINT("%s-%d: name=%s\n",__func__,__LINE__,ipsecProf->head.name);
		    //sleep(1);
		}
		else
		{
		    doSystem("ipsec auto --down \"%s\"",ipsecProf->head.name);
#if(VPN_CONTROL == FYES)
		if(ipsecProf->sa_state == 1)
#endif
		    doSystem("ipsec auto --delete \"%s\"",ipsecProf->head.name);
		    ipsecProf->sa_conn=0U;	    /* set sa ststus: not connected*/
		    IPSEC_PRINT("%s-%d: sa_conn=%d\n",__func__,__LINE__,ipsecProf->sa_conn);
		}
#endif
	    }
	}
    }

#if 1
    nvramWriteCommit();		/*save*/
#endif
out:
    websRedirect(wp, "IPSec.asp");
    return;
}
/*
 * setRouteForServer(ipsecConfProfile *ipsecProf)
 * 当作为服务端时，ipsec sa 连接成功后，增加到客户端的路由
 * for mantis bug 0010448
 */
static void setRouteForServer(ipsecConfProfile *ipsecProf)
{
    char if_name[16]={0};
    char if_addr[16]={0};
    char gw_addr[16]={0};
    int IfNum=0;
    char cmdStr[254]={0};
    char cmdStr_del[254]={0};
    char via_gw[50]={0};
    
    IfNum=strtol(ipsecProf->profile, NULL, 10);
    if(IfNum==0) {
    
	getIfIp(getLanIfName(),if_addr);
	strcpy(if_name, getLanIfName());
    } else {
	getIfIp(getWanIfNamePPP(IfNum), if_addr);
	getWanGwByIndex(IfNum, gw_addr);
	strcpy(if_name, getWanIfNamePPP(IfNum));
    }
    IPSEC_PRINT("%s-%d: IfNum=%d, if_name=%s, if_addr=%s,gw_addr=%s\n",__func__,__LINE__,IfNum,if_name,if_addr,gw_addr);
    if(ipsecProf->connType == GATEWAY_FROM_DYNAMIC) {
	memset(cmdStr, 0, sizeof(cmdStr));
	memset(cmdStr_del, 0, sizeof(cmdStr_del));
	memset(via_gw, 0, sizeof(via_gw));
	if(0 != strlen(gw_addr))
	{
	    sprintf(via_gw,"via %s",gw_addr);
	}
	if((strlen(ipsecProf->peer)!=0)&&(strcmp(ipsecProf->peer,"0.0.0.0")!=0))
	{
	    if(strcmp(ipsecProf->peer,ipsecProf->peerIP)==0)
	    {
		sprintf(cmdStr, "ip route replace %s/32 dev %s ",ipsecProf->peer,if_name);
		strcat(cmdStr, via_gw);
		doSystem(cmdStr);
	    } else {
		if((strlen(ipsecProf->peerIP)!=0)&&(strcmp(ipsecProf->peerIP, "0.0.0.0")!=0))
		{
		    sprintf(cmdStr_del, "ip route del %s/32 dev %s ",ipsecProf->peerIP,if_name);
		    strcat(cmdStr_del, via_gw);
		    doSystem(cmdStr_del);
		}
		sprintf(cmdStr, "ip route add %s/32 dev %s ",ipsecProf->peer,if_name);
		strcat(cmdStr, via_gw);
		doSystem(cmdStr);
	    }
	}
	IPSEC_PRINT("%s-%d: cmdStr=%s\n",__func__,__LINE__,cmdStr);
    }
}
/*
 * get SA status
 * from file /ipsec/SA_autoup_status.txt
 */
static void checkConfigSA_status(ipsecConfProfile *prof)
{
    FILE *fp;
    fpos_t home;
    char readStr[READ_MAX_LEN];
    char getIP[30];
    char *ptr=NULL;
    char instName[UTT_INST_NAME_LEN+3];
    int flag=0;
    /* open file */
    if((fp = fopen(IPSEC_SA_AUTOUP_FILE,"r")) != NULL)
    {
	memset(instName, 0, sizeof(instName));
	sprintf(instName, "\"%s\"", prof->head.name); /*add instName for bug 10443*/
	IPSEC_PRINT("%s-%d: instName=%s\n",__func__,__LINE__,instName);
	prof->sa_conn=0U;	/*先设置状态为未建立 ? */
	if(prof->connType == GATEWAY_FROM_DYNAMIC)
	    strcpy(prof->peer, "0.0.0.0");
	fgetpos(fp,&home);
	while(fgets(readStr, READ_MAX_LEN, fp)!=NULL)
	{
	    /* check conn name */
	    if(strstr(readStr, instName)!=NULL)
	    {
		if((strstr(readStr, IPSEC_UNROUTED_FIND_STR)!=NULL)||(strstr(readStr, IPSEC_PROSPECTIVE_FIND_STR)!=NULL))
		{
		    flag=1;
		    prof->sa_conn=0U;	/*设置状态为未建立*/
		    IPSEC_PRINT("%s-%d: read=%s\n",__func__,__LINE__,readStr);
		    //break;
		}
		else
		{
		    /* check ipsec connect success or not  */
		    if(strstr(readStr, IPSEC_IPSEC_SA_FIND_STR)!=NULL)
		    {
			prof->sa_conn=3U;	/* set status: ipsec sa established*/
			IPSEC_PRINT("%s-%d: read=%s\n",__func__,__LINE__,readStr);
			/*
			 * 如果连接方式为对端动态连接到本地
			 * 则获取对端ip
			 */
			if(prof->connType == GATEWAY_FROM_DYNAMIC)
			{
			    fsetpos(fp,&home);
			    fscanf(fp,"%*s%*s%*s%s%*s",getIP);
			    IPSEC_PRINT("%s-%d: getIP=%s\n",__func__,__LINE__,getIP);
			    ptr=strstr(getIP, ":");
			    if(ptr!=NULL)
			    {
				*ptr='\0';
				IPSEC_PRINT("%s-%d: getIP=%s\n",__func__,__LINE__,getIP);
			    }
			    strcpy(prof->peer, getIP);
			    setRouteForServer(prof); /*for mantis bug 0010448,注意不能与下一句调整顺序*/
			    strcpy(prof->peerIP, getIP);
			}
			break;
		    }
		    /* check isakmp sa connect success or not  */
		    if(strstr(readStr, IPSEC_ISAKMP_SA_FIND_STR)!=NULL)
		    {
			if(prof->sa_conn < 2U)	/*set: isakmp sa established */
			{
			    prof->sa_conn=2U;
			    IPSEC_PRINT("%s-%d: read=%s\n",__func__,__LINE__,readStr);
			}
		    }
		    if(prof->sa_conn < 1U)
		    {
			prof->sa_conn=1U;	/*设置状态为IKE协商*/
			IPSEC_PRINT("%s-%d: read=%s\n",__func__,__LINE__,readStr);
		    }
		}
	    }
	    fgetpos(fp,&home);
	}
	if((prof->sa_conn == 1U)&&(flag==1))
	{
	    prof->sa_conn = 0U;
	}
	fclose(fp);	/* close file */
    } else {
	IPSEC_PRINT("open SA_autoup_status.txt is error!\n");
    }
    return;
}
/*
 * /goform/IPSec_ReqSA
 * 
 * 建立SA连接
 */
static void  formIPSec_ReqSA(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibipsec = MIB_PROF_IPSEC;
    ipsecConfProfile *ipsecProf = NULL;
    char *tmp=NULL;
    int index = -1;
    const char *ErrMsg=NULL;

    char *reqStr = NULL;
    /* get connect names */
    reqStr = websGetVar(wp, "reqstr", T(""));
    IPSEC_PRINT("%s-%d: reqStr=%s\n",__func__,__LINE__,reqStr);
    if((reqStr!=NULL) && (strlen(reqStr)!=0))
    {
	/*split name with :*/
	tmp = strtok(reqStr, UTT_SPLIT_SIGN_COLONS);
	while(tmp != NULL)
	{
	    IPSEC_PRINT("%s-%d: tmp=%s\n",__func__,__LINE__,tmp);
	    index = ProfGetInstIndexByName(mibipsec, tmp);
		IPSEC_PRINT("%s-%d: index=%d\n",__func__,__LINE__,index);
	    if(index != PROFFAIL)
	    {
		ipsecProf = (ipsecConfProfile *)ProfGetInstPointByIndex(mibipsec, index);
		IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
		if(ipsecProf->active == FUN_ENABLE) {	/*允许*/
		/*建立隧道连接*/
#if(VPN_CONTROL == FYES)
	    if(nvramProfile->vpnProfile.count<=0)
	    {
		ErrMsg = getTransDataForC(C_LANG_INDEX_MAX_VPN_COUNT);
		setTheErrorMsg(ErrMsg);
		goto out;
	    }
#endif
#if 0
		doSystem("ipsec setup restart");
#endif
		doSystem("ipsec auto --down \"%s\"",tmp);
		doSystem("ipsec auto --add \"%s\"",tmp);
		doSystem("ipsec auto --rereadsecrets");
		doSystem("ipsec auto --up \"%s\" &",tmp);
		IPSEC_PRINT("%s-%d: tmp=%s\n",__func__,__LINE__,tmp);
		}
		else{	    /*禁止*/
		    setTheErrorMsg("Forbidden");
		}
	    }
	    tmp = strtok(NULL, UTT_SPLIT_SIGN_COLONS);
	}
    }

out:
    websRedirect(wp, "IPSec.asp");
    return;
}
/*
 * /goform/IPSec_Clear
 * 
 * 挂断SA连接
 */
static void  formIPSec_ClearSA(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibipsec = MIB_PROF_IPSEC;
    ipsecConfProfile *ipsecProf = NULL;
    char *tmp=NULL;
    int index = -1;

    char *reqStr = NULL;
    /*get clear names*/
    reqStr = websGetVar(wp, "sa_delstr", T(""));
    IPSEC_PRINT("%s-%d: delStr=%s\n",__func__,__LINE__,reqStr);
    if((reqStr!=NULL) && (strlen(reqStr)!=0))
    {
	/* split name with : */
	tmp = strtok(reqStr, UTT_SPLIT_SIGN_COLONS);
	while(tmp != NULL)
	{
	    index = ProfGetInstIndexByName(mibipsec, tmp);
	    if(index != PROFFAIL)
	    {
		/* get profile pointer */
		ipsecProf = (ipsecConfProfile *)ProfGetInstPointByIndex(mibipsec, index);
		/*清除隧道连接*/
#if 1
		doSystem("ipsec auto --down \"%s\"",tmp);
#if(VPN_CONTROL == FYES)
		if(ipsecProf->sa_state == 1)
#endif
		doSystem("ipsec auto --delete \"%s\"",tmp);
		ipsecProf->sa_conn=0U;	    /* set sa ststus: not connected*/
		IPSEC_PRINT("%s-%d: sa_conn=%d\n",__func__,__LINE__,ipsecProf->sa_conn);
#endif
	    }
	    tmp = strtok(NULL, UTT_SPLIT_SIGN_COLONS);
	}
    }
    /*redirect web page*/
    websRedirect(wp, "IPSec.asp");
    return;
}

/*
 * aspOutputIPSecListData()
 *
 * output ipsec data
 */
static int aspOutputIPSecListData(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibipsec = MIB_PROF_IPSEC;
    MibProfileType mibisakmp = MIB_PROF_ISAKMP;
    ipsecConfProfile *ipsecProf = NULL;
    isakmpConfProfile *isakmpProf = NULL;
    int isakmpIndex=-1;
    int max=0, min=0, i=0;
    int totalrecs=0;
    /* define web varliable */
    websWrite(wp,"var ids=new Array();");
    websWrite(wp,"var allow=new Array();");
    websWrite(wp,"var connType=new Array();");
    websWrite(wp,"var peer=new Array();");
    websWrite(wp,"var remoteAddr=new Array();");
    websWrite(wp,"var remoteMask=new Array();");
    websWrite(wp,"var remoteUser=new Array();");
    websWrite(wp,"var remoteUserType=new Array();");
    websWrite(wp,"var localBind=new Array();");
    websWrite(wp,"var localAddr=new Array();");
    websWrite(wp,"var localMask=new Array();");
    websWrite(wp,"var localUser=new Array();");
    websWrite(wp,"var localUserType=new Array();");
    websWrite(wp,"var preshareKey=new Array();");
    websWrite(wp,"var transform=new Array();");
    websWrite(wp,"var protocol=new Array();");
    websWrite(wp,"var srcPort=new Array();");
    websWrite(wp,"var destPort=new Array();");
    websWrite(wp,"var negMode=new Array();");
    websWrite(wp,"var aggresEncrypt=new Array();");
    websWrite(wp,"var isakmplifetime=new Array();");
    websWrite(wp,"var policy=new Array();");
    websWrite(wp,"var lifetime_sec=new Array();");
    websWrite(wp,"var lifetime_kbytes=new Array();");
    websWrite(wp,"var anti_replay=new Array();");
    websWrite(wp,"var dpdEnable=new Array();");
    websWrite(wp,"var dpdheartbeat=new Array();");
    websWrite(wp,"var nattEnable=new Array();");
    websWrite(wp,"var natt_port=new Array();");
    websWrite(wp,"var natt_keepalive=new Array();\n");
    websWrite(wp,"var sa_conn=new Array();\n");
    websWrite(wp,"var packet_out=new Array();\n");
    websWrite(wp,"var packet_in=new Array();\n");
#if 1 
    doSystem("ipsec auto --status>%s",IPSEC_SA_AUTOUP_FILE);
    IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
    usleep(500);
#endif
    /* get profile inst max and min index */
    ProfInstLowHigh(mibipsec, &max, &min);
    for(i=min;i<max;i++)    
    {
	/* get profile pointer */
	ipsecProf = (ipsecConfProfile *)ProfGetInstPointByIndex(mibipsec, i);
	if((ipsecProf!=NULL)&&(ProfInstIsFree(ipsecProf) == 0))
	{
#if 1	
	    checkConfigSA_status(ipsecProf);	/*set sa status*/
	    IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
#endif
	    /*begine write ipsec data*/
	    websWrite(wp,"ids[%d]=\"%s\";",totalrecs,ipsecProf->head.name);
	    if(ipsecProf->active == FUN_ENABLE)
	    {
		websWrite(wp,"allow[%d]=\"%s\";",totalrecs,"yes");
	    }
	    else
	    {
		websWrite(wp,"allow[%d]=\"%s\";",totalrecs,"no");
	    }
	    /* 连接方式 */
	    if(ipsecProf->connType==GATEWAY_TO_GATEWAY)
	    {
		websWrite(wp,"connType[%d]=\"%s\";",totalrecs,"1");
	    }
	    else if(ipsecProf->connType==DYNAMIC_TO_GATEWAY)
	    {
		websWrite(wp,"connType[%d]=\"%s\";",totalrecs,"2");
	    }
	    else
	    {
		websWrite(wp,"connType[%d]=\"%s\";",totalrecs,"3");
	    }
	    /* 远端 */
	    websWrite(wp,"peer[%d]=\"%s\";",totalrecs,ipsecProf->peer);
	    websWrite(wp,"remoteAddr[%d]=\"%s\";",totalrecs,ipsecProf->destAddr);
	    websWrite(wp,"remoteMask[%d]=\"%s\";",totalrecs,ipsecProf->destMask);
	    /* 本地 */
	    websWrite(wp,"localBind[%d]=\"%s\";",totalrecs,ipsecProf->profile);
	    websWrite(wp,"localAddr[%d]=\"%s\";",totalrecs,ipsecProf->srcAddr);
	    websWrite(wp,"localMask[%d]=\"%s\";",totalrecs,ipsecProf->srcMask);
	    websWrite(wp,"transform[%d]=\"%s\";",totalrecs,ipsecProf->transformSet);
	    websWrite(wp,"protocol[%d]=%d;",totalrecs,ipsecProf->protocol);
	    websWrite(wp,"srcPort[%d]=%d;",totalrecs,ipsecProf->srcPort);
	    websWrite(wp,"destPort[%d]=%d;",totalrecs,ipsecProf->destPort);
	    websWrite(wp,"lifetime_sec[%d]=%d;",totalrecs,ipsecProf->lifetime_sec);
	    websWrite(wp,"lifetime_kbytes[%d]=%d;",totalrecs,ipsecProf->lifetime_kbytes);
	    IPSEC_PRINT("%s-%d: localAddr=%s,localMask=%s,transform=%s\n",__func__,__LINE__,ipsecProf->srcAddr,ipsecProf->srcMask,ipsecProf->transformSet);
	    if(ipsecProf->anti_replay==FUN_ENABLE)
	    {
		websWrite(wp,"anti_replay[%d]=\"%s\";",totalrecs,"on");	/*need */
	    }
	    else
	    {
		websWrite(wp,"anti_replay[%d]=\"%s\";",totalrecs,"off");	/*need */
	    }
	    if(ipsecProf->dpd_enable==FUN_ENABLE)
	    {
		websWrite(wp,"dpdEnable[%d]=\"%s\";",totalrecs,"on");	/*need*/
	    }
	    else
	    {
		websWrite(wp,"dpdEnable[%d]=\"%s\";",totalrecs,"off");	/*need*/
	    }
	    websWrite(wp,"dpdheartbeat[%d]=%d;",totalrecs,ipsecProf->dpd_heartbeat);
	    if(ipsecProf->nat_enable==FUN_ENABLE)
	    {
		websWrite(wp,"nattEnable[%d]=\"%s\";",totalrecs,"on");	/*need*/
	    }
	    else
	    {
		websWrite(wp,"nattEnable[%d]=\"%s\";",totalrecs,"off");	/*need*/
	    }
	    websWrite(wp,"natt_port[%d]=%d;",totalrecs,ipsecProf->nat_port);
	    websWrite(wp,"natt_keepalive[%d]=%d;",totalrecs,ipsecProf->nat_keepalive);
	    websWrite(wp,"sa_conn[%d]=%d;",totalrecs,ipsecProf->sa_conn);
	    websWrite(wp,"packet_out[%d]=%d;",totalrecs,ipsecProf->packet_out);
	    websWrite(wp,"packet_in[%d]=%d;",totalrecs,ipsecProf->packet_in);
	    isakmpIndex = ProfGetInstIndexByName(mibisakmp, ipsecProf->isakmpProfile);
	    if(isakmpIndex != PROFFAIL)
	    {
		isakmpProf = (isakmpConfProfile *)ProfGetInstPointByIndex(mibisakmp, isakmpIndex);
		if((isakmpProf != NULL)&&(ProfInstIsFree(ipsecProf) == 0))
		{
		    IPSEC_PRINT("%s-%d: isakmpIndex=%d\n",__func__,__LINE__,isakmpIndex);
		    websWrite(wp,"remoteUser[%d]=\"%s\";",totalrecs,isakmpProf->remoteuser);
		    if(isakmpProf->remoteType==IKE_FQDN)
		    {
			websWrite(wp,"remoteUserType[%d]=\"%s\";",totalrecs,"IKE_FQDN");
		    }
		    else if(isakmpProf->remoteType==IKE_USERFQDN)
		    {
			websWrite(wp,"remoteUserType[%d]=\"%s\";",totalrecs,"IKE_USERFQDN");
		    }
		    else if(isakmpProf->remoteType==IKE_KEYID)
		    {
			websWrite(wp,"remoteUserType[%d]=\"%s\";",totalrecs,"IKE_KEYID");
		    }
		    else if(isakmpProf->remoteType==IKE_IPV4ADDR)
		    {
			websWrite(wp,"remoteUserType[%d]=\"%s\";",totalrecs,"IKE_IPV4ADDR");
		    }
		    websWrite(wp,"localUser[%d]=\"%s\";",totalrecs,isakmpProf->localuser);
		    if(isakmpProf->localType==IKE_FQDN)
		    {
			websWrite(wp,"localUserType[%d]=\"%s\";",totalrecs,"IKE_FQDN");
		    }
		    else if(isakmpProf->localType==IKE_USERFQDN)
		    {
			websWrite(wp,"localUserType[%d]=\"%s\";",totalrecs,"IKE_USERFQDN");
		    }
		    else if(isakmpProf->localType==IKE_KEYID)
		    {
			websWrite(wp,"localUserType[%d]=\"%s\";",totalrecs,"IKE_KEYID");
		    }
		    else if(isakmpProf->localType==IKE_IPV4ADDR)
		    {
			websWrite(wp,"localUserType[%d]=\"%s\";",totalrecs,"IKE_IPV4ADDR");
		    }
		    websWrite(wp,"preshareKey[%d]=\"%s\";",totalrecs,isakmpProf->preshareKey);
		    if(isakmpProf->mode==Mainmode)
		    {
			websWrite(wp,"negMode[%d]=\"%s\";",totalrecs,"Main");
		    }
		    else
		    {
			websWrite(wp,"negMode[%d]=\"%s\";",totalrecs,"Aggres");
		    }
		    if(isakmpProf->aggresEncrypt == FUN_ENABLE)
		    {
			websWrite(wp,"aggresEncrypt[%d]=\"%s\";",totalrecs,"on");
		    }
		    else
		    {
			websWrite(wp,"aggresEncrypt[%d]=\"%s\";",totalrecs,"off");
		    }
		    websWrite(wp,"isakmplifetime[%d]=%d;",totalrecs,isakmpProf->lifetime);
		    websWrite(wp,"policy[%d]=\"%s\";\n",totalrecs,isakmpProf->policy);

		}
	    }
	    totalrecs++;
	}

    }
    
    websWrite(wp,"\nvar totalrecs=%d;",totalrecs);
    websWrite(wp,"var max_totalrecs=%d;\n",max);	    /* max inst num */
    getTheErrorMsg(eid, wp, argc, argv);	    /* get wrong msg */
    return 0;
}

/*
 * aspOutputOneIPSecData()
 * output one ipsec data 
 * invoke by IPSec_edit.asp
 */
static int aspOutputOneIPSecData(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibipsec = MIB_PROF_IPSEC;
    MibProfileType mibisakmp = MIB_PROF_ISAKMP;
    ipsecConfProfile *ipsecProf = NULL;
    isakmpConfProfile *isakmpProf = NULL;
    int isakmpIndex=-1;
    int max=0, min=0, i=0;
    
    /* get assigned inst name*/
    char *editName;
    const char *errmsg=NULL;
    if (ejArgs(argc, argv, T("%s"), &editName) < 1) {
	errmsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_READ_DATA_ERR);
	setTheErrorMsg(errmsg);
    } else {
	IPSEC_PRINT("edit_name: %s\n",editName);
	/* get inst max value and min value */
	ProfInstLowHigh(mibipsec, &max, &min);
	for(i=min;i<max;i++)
	{
	    /* get profile pointer */
	    ipsecProf = (ipsecConfProfile *)ProfGetInstPointByIndex(mibipsec, i);
	    if((ipsecProf!=NULL)&&(ProfInstIsFree(ipsecProf) == 0)&&(editName != NULL) && (strcmp(editName, ipsecProf->head.name) == 0))
	    {
		websWrite(wp,"var cur_ids=\"%s\";",ipsecProf->head.name);
		/* 连接方式 */
		if(ipsecProf->connType==GATEWAY_TO_GATEWAY) {
		    websWrite(wp,"var cur_connType=\"%s\";","1");
		} else if(ipsecProf->connType==DYNAMIC_TO_GATEWAY) {
		    websWrite(wp,"var cur_connType=\"%s\";","2");
		} else {
		    websWrite(wp,"var cur_connType=\"%s\";","3");
		}
		/* 远端 */
		websWrite(wp,"var cur_peer=\"%s\";",ipsecProf->peer);
		websWrite(wp,"var cur_remoteAddr=\"%s\";",ipsecProf->destAddr);
		websWrite(wp,"var cur_remoteMask=\"%s\";",ipsecProf->destMask);
		/* 本地 */
		websWrite(wp,"var cur_localBind=\"%s\";",ipsecProf->profile);
		websWrite(wp,"var cur_localAddr=\"%s\";",ipsecProf->srcAddr);
		websWrite(wp,"var cur_localMask=\"%s\";",ipsecProf->srcMask);
		websWrite(wp,"var cur_transform=\"%s\";",ipsecProf->transformSet);
		websWrite(wp,"var cur_protocol=%d;",ipsecProf->protocol);
		websWrite(wp,"var cur_srcPort=%d;",ipsecProf->srcPort);
		websWrite(wp,"var cur_destPort=%d;",ipsecProf->destPort);
		websWrite(wp,"var cur_lifetime_sec=%d;",ipsecProf->lifetime_sec);
		websWrite(wp,"var cur_lifetime_kbytes=%d;",ipsecProf->lifetime_kbytes);
		if(ipsecProf->anti_replay==FUN_ENABLE) {
		    websWrite(wp,"var cur_anti_replay=\"%s\";","on");	/*need */
		} else {
		    websWrite(wp,"var cur_anti_replay=\"%s\";","off");	/*need */
		}
		if(ipsecProf->dpd_enable==FUN_ENABLE) {
		    websWrite(wp,"var cur_dpdEnable=\"%s\";","on");	/*need*/
		} else {
		    websWrite(wp,"var cur_dpdEnable=\"%s\";","off");	/*need*/
		}
		websWrite(wp,"var cur_dpdheartbeat=%d;",ipsecProf->dpd_heartbeat);
		if(ipsecProf->nat_enable==FUN_ENABLE) {
		    websWrite(wp,"var cur_nattEnable=\"%s\";","on");	/*need*/
		} else {
		    websWrite(wp,"var cur_nattEnable=\"%s\";","off");	/*need*/
		}
		websWrite(wp,"var cur_natt_port=%d;",ipsecProf->nat_port);
		websWrite(wp,"var cur_natt_keepalive=%d;",ipsecProf->nat_keepalive);
		isakmpIndex = ProfGetInstIndexByName(mibisakmp, ipsecProf->isakmpProfile);
		if(isakmpIndex != PROFFAIL)
		{
		    isakmpProf = (isakmpConfProfile *)ProfGetInstPointByIndex(mibisakmp, isakmpIndex);
		    if((isakmpProf != NULL)&&(ProfInstIsFree(ipsecProf) == 0))
		    {
			IPSEC_PRINT("%s-%d: isakmpIndex=%d\n",__func__,__LINE__,isakmpIndex);
			websWrite(wp,"var cur_remoteUser=\"%s\";",isakmpProf->remoteuser);
			/*身份类型*/
			if(isakmpProf->remoteType==IKE_FQDN) {
			    websWrite(wp,"var cur_remoteUserType=\"%s\";","IKE_FQDN");
			} else if(isakmpProf->remoteType==IKE_USERFQDN) {
			    websWrite(wp,"var cur_remoteUserType=\"%s\";","IKE_USERFQDN");
			} else if(isakmpProf->remoteType==IKE_KEYID) {
			    websWrite(wp,"var cur_remoteUserType=\"%s\";","IKE_KEYID");
			} else if(isakmpProf->remoteType==IKE_IPV4ADDR) {
			    websWrite(wp,"var cur_remoteUserType=\"%s\";","IKE_IPV4ADDR");
			}
			websWrite(wp,"var cur_localUser=\"%s\";",isakmpProf->localuser);
			/*身份类型*/
			if(isakmpProf->localType==IKE_FQDN) {
			    websWrite(wp,"var cur_localUserType=\"%s\";","IKE_FQDN");
			} else if(isakmpProf->localType==IKE_USERFQDN) {
			    websWrite(wp,"var cur_localUserType=\"%s\";","IKE_USERFQDN");
			} else if(isakmpProf->localType==IKE_KEYID) {
			    websWrite(wp,"var cur_localUserType=\"%s\";","IKE_KEYID");
			} else if(isakmpProf->localType==IKE_IPV4ADDR) {
			    websWrite(wp,"var cur_localUserType=\"%s\";","IKE_IPV4ADDR");
			}
			websWrite(wp,"var cur_preshareKey=\"%s\";",isakmpProf->preshareKey);
			/*协商模式*/
			if(isakmpProf->mode==Mainmode) {
			    websWrite(wp,"var cur_negMode=\"%s\";","Main");
			} else {
			    websWrite(wp,"var cur_negMode=\"%s\";","Aggres");
			}
			/*协商模式加密*/
			if(isakmpProf->aggresEncrypt == FUN_ENABLE) {
			    websWrite(wp,"var cur_aggresEncrypt=\"%s\";","on");
			} else {
			    websWrite(wp,"var cur_aggresEncrypt=\"%s\";","off");
			}
			websWrite(wp,"var cur_isakmplifetime=%d;",isakmpProf->lifetime);
			websWrite(wp,"var cur_policy=\"%s\";\n",isakmpProf->policy);

		    }
		}
	    }
	}

    }
    /*get error msg*/
    getTheErrorMsg(eid, wp, argc, argv);	
    return 0;
}
#if (PPTP_SERVER == FYES)
/*
 * aspOutputPptpSrv_Data
 * 
 * 输出pptp server用户类型和用户名和MAC地址
 * 
 */ 
static int aspOutputPptpSrv_Data(int eid, webs_t wp, int argc, char_t **argv) {
    MibProfileType mibType = MIB_PROF_PPTP_SRV_ACCOUNT;
    PPTPSrvAccProfile *profPptpA = NULL;
    int min = 0, max = 0, i = 0, index = 0;
    
    websWrite(wp, "var instNames_srv = new Array();");
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
        profPptpA = (PPTPSrvAccProfile *)ProfGetInstPointByIndex(mibType, i);
        if((profPptpA != NULL) && (ProfInstIsFree(profPptpA) == 0)) {
	    websWrite(wp, "instNames_srv[%d] = \"%s\";", index++, profPptpA->head.name);
	}
    }
    websWrite(wp, "\n");

    return 0;
}
#endif
#if (PPTP_CLIENT == FYES)
static int aspGetPptpClient_Data(int eid, webs_t wp, int argc, char_t **argv) {
    MibProfileType mibType = MIB_PROF_PPTP_CLI;
    PPTPCliProfile *prof = NULL;
    int min = 0, max = 0, i = 0, index = 0;

    websWrite(wp, "var instNames_cli = new Array();");
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) {
	prof = (PPTPCliProfile *)ProfGetInstPointByIndex(mibType, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
	    websWrite(wp, "instNames_cli[%d] = \"%s\";", index++, prof->head.name);
	}
    }
    websWrite(wp, "\n");
}
#endif

/*
 * 定义页面处理句柄
 */
extern void formDefineIPSec(void) {
    /* define asp */
    websAspDefine(T("aspOutputIPSecListData"), aspOutputIPSecListData);
    websAspDefine(T("OutputOneIPSecData"), aspOutputOneIPSecData);
#if (PPTP_SERVER == FYES)
    websAspDefine(T("OutputPptpSrvData"), aspOutputPptpSrv_Data);
#endif
#if (PPTP_CLIENT == FYES)
    websAspDefine(T("OutputPptpClientData"), aspGetPptpClient_Data);
#endif
#if 0
    websAspDefine(T("OutputPPPoE_Data"), aspOutputPPPoE_Data);
#endif
    /*define goform*/
    websFormDefine(T("IPSecIsakmp_Config"), formIPSecIsakmp_Config);
    websFormDefine(T("IPSecList_Del"), formIPSecList_Del);
    websFormDefine(T("IPSecList_DelAll"), formIPSecList_DelAll);
    websFormDefine(T("formIPSecAllow"), formConfigIPSecAllow);
    websFormDefine(T("IPSec_ReqSA"), formIPSec_ReqSA);
    websFormDefine(T("IPSec_Clear"), formIPSec_ClearSA);
    //updateDomainIP();
    return;
}
#endif
