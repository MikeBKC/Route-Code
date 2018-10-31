#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"
#include "spdComFun.h"
#include "comExeFun.h"
#if (IP_GRP == FYES)
#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif
static void exceptIpGroupProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const IpGrpProfile *oldProfileDataPtr, const IpGrpProfile *newProfileDataPtr );

static void ipGrpAdd(IpGrpProfile *prof);
static void ipGrpEdit(IpGrpProfile *curGrp);
#if (PPPOE_SERVER == FYES)
void ipGrpUpPoe(char*grpName, char* username, char addOrDel);
#endif
#if (WEB_AUTH == FYES)
extern void webauthAddexceptIpGroup(void);
extern void webauthDelexceptIpGroup(WebAuthGlobalProfile *prof);
void ipGrpUpWeb(char*grpName, char* username, char addOrDel);
void IPGrpUpWebExIp(char* grpName);
#endif
void ipGrpBuildSet(IpGrpProfile *prof, char *setName);
int ipGrpDepToSub(IpGrpProfile *rootProf, char* subName);
int ipGrpDepSearch(IpGrpProfile *rootProf, char *subName);
int ipGrpSearchUser(int grpProfIndex, char*userName, ipGrpEntryType userType, int*searched);
static void ipGroupInit(void);
#ifdef CONFIG_UTT_NETFILTER
extern int uttNfNlPolicyDelAll(void);
#endif
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
extern int profChangeNfNlSock;
extern void sendMsgToKernel(int socket, MibProfileType proftype, int instIndex, Pointer oldDataPtr, int profsize, ProfChangeType changeType);
#endif
/********************************************************************
* ������ exceptIpGroupProfileChange
* ���ܣ� ��֤���ñ仯 ��Ч����
* ������ 2012-05-04
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void exceptIpGroupProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const IpGrpProfile *oldProfileDataPtr, const IpGrpProfile *newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
	    ipGrpAdd((IpGrpProfile *)newProfileDataPtr);
	    break;
        case PROF_CHANGE_EDIT:
	    ipGrpEdit((IpGrpProfile *)newProfileDataPtr);
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    SystemProfile *profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0); 
	    if ((profSys->dnsFilterEn == FUN_ENABLE) && /*�������˿���*/
		 (profSys->dnsFilterControl.addrTypeEn == ADDR_IPGRP) && /*�����û���*/
		 (strcmp(newProfileDataPtr->head.name,profSys->dnsFilterControl.ipGrpName) == 0)) {

		sendMsgToKernel(profChangeNfNlSock, profileType,index, (Pointer)oldProfileDataPtr,sizeof(*oldProfileDataPtr),changeType);
	    }
#endif
	    break;
        case PROF_CHANGE_DEL:
        case PROF_CHANGE_DELALL:
        default:/*default*/
            break;
    }
    return;
}
/**
 * ip�����
 */
static void ipGrpAdd(IpGrpProfile *prof)
{
    char *setName = prof->head.name;/*ipset����*/
    SWORD_PRINTF("%s--------%d, prof->grpType = %d, setName = %s\n",__func__, __LINE__, prof->grpType, setName);
    if(prof->grpType == IPGRP_IP)
    {
	/*��Ա����Ϊ�̶�ip*/
	ipGrpIpAddSet(setName);/*�����Ӧipset��*/
    }
    else
    {
	if(prof->grpType == IPGRP_USER)
	{/*��Ա����Ϊ�û���*/
	    ipGrpUserAddSet(setName);/*�����Ӧipset��*/
	}
    }
    ipGrpBuildSet(prof, setName);/*������ipset��*/
    return;

}
/********************************************************************
* ������ ipGrpEdit
* ���ܣ� ip���޸�
*	 ��setNameΪ���༭������,��������setName���鱻���¹���
* ������ 2012-05-07
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void ipGrpEdit(IpGrpProfile *curGrp)
{
    MibProfileType mibType = MIB_PROF_IPGRP;
    IpGrpProfile* prof = NULL;

    int i = 0, min = 0, max = 0;
#if (GROUP_BASE == FYES)
    int clearKAct = 0;/*�ں���Ϊ���������־*/
    clearKAct = isIpGrpReredByNf(curGrp);/*�������鱻��Ϊ����������������ں˲���*/

    if(clearKAct == 1)
    {
	uttSemP(SEM_UTTNF_NO,0);/*���ź�����ֹ��ͻ*/
	uttNfNlPolicyDelAll();/*����Ϣ֪ͨ�ں�ɾ������*/
    }
#endif
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) 
    {
	prof = (IpGrpProfile*)ProfGetInstPointByIndex(mibType, i);
	if(ProfInstIsFree(prof) == 0 && curGrp->grpType == prof->grpType)/*����ʹ����*/
	{/*ֻ����ͬ���͵���*/
	    if(ipGrpDepSearch(prof, curGrp->head.name) == 0)/*prof���õ�����curGrpName*/
	    {
		SWORD_PRINTF("%s------%d, curGrp->head.name = %s\n",__func__, __LINE__, curGrp->head.name);
		ipsetFlushGrp(prof->head.name);/*���ԭ��������*/
		ipGrpBuildSet(prof, prof->head.name);/*���¹���������*/
#if (WEB_AUTH == FYES)
		IPGrpUpWebExIp(prof->head.name);
#endif
	    }
	}
    }
#if (GROUP_BASE == FYES)
    if(clearKAct == 1)
    {
	uttSemV(SEM_UTTNF_NO,0);/*�������������Ӧ���ͷŸ���*/
    }
#endif
    return;
}
 /********************************************************************
 * ������ ipGrpDel
 * ���ܣ� ip��ɾ��
 * ������ 2012-05-07
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/
void ipGrpDel(IpGrpProfile *delProf)
{
     ipsetDelGrp(delProf->head.name, TRUE);
     return;
}

#if (PPPOE_SERVER == FYES)
 /********************************************************************
 * ������ ipGrpUpPoe
 * ���ܣ� ����pppoe�û�����Ӧ���û���
 * ������ 2012-05-07
 * ������ 
 *	  username:pppoe�û���
 *	  grpName���������û���
 *	  addOrDel�����µ�ʱ��ɾ������ӵ�ǰ�û�����Ӧ��ipset��
          IPSET_MEM_ADD/ IPSET_MEM_DEL
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/
void ipGrpUpPoe(char*grpName, char* username, char addOrDel)
{
    int poeAccIndex = 0;
    MibProfileType mibType = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *poeAcc = NULL;/*pppoe�˺�profile*/

    SWORD_PRINTF("%s--------%d, grpName = %s, username = %s\n", __func__, __LINE__, grpName, username);
    uttSemP(SEM_PPPOE_NO,0);/*���ź�������ֹ�������̷���*/

    poeAccIndex = ProfGetInstPointByValue(mibType, 
	    OFFANDSIZE(PPPoESrvAccProfile, pppoename), TSTR, username, -1);
    if(poeAccIndex != PROFFAIL)
    {
	poeAcc = ProfGetInstPointByIndex(mibType, poeAccIndex);
    }
    if(ProfInstIsFree(poeAcc) == 0)
    { /*�˺�ʹ����*/
//	SWORD_PRINTF("%s--------%d, poeAcc->ConTime = %d\n", __func__, __LINE__, poeAcc->ConTime);
#if 0
	if(poeAcc->ConTime == 0U)
	{/*����*/
	    ipsetAOrDIpStr(grpName, poeAcc->ip, addOrDel);/*������Ӧ��ipset��*/
	}
#else
	AccAddIpStr(poeAcc, grpName, addOrDel);
#endif
    }

    uttSemV(SEM_PPPOE_NO,0);/*�ͷ��ź���*/
    return;
}
#endif
#if (WEB_AUTH == FYES)
 /********************************************************************
 * ������ ipGrpUpWeb
 * ���ܣ� ����web��֤�û�����Ӧ���û���
 * ������ 2012-05-07
 * ������ 
 *	  username:web��֤�û���
 *	  grpName���������û���
 *	  addOrDel�����µ�ʱ��ɾ������ӵ�ǰ�û�����Ӧ��ipset��
          IPSET_MEM_ADD/ IPSET_MEM_DEL
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/
void ipGrpUpWeb(char*grpName, char* username, char addOrDel)
{
    int webUserIndex = 0;
    MibProfileType mibType = MIB_PROF_WEB_AUTH;
    WebAuthProfile *webUser = NULL;/*web��֤�û�profile*/
    SWORD_PRINTF("%s--------%d, grpName = %s, username = %s\n", __func__, __LINE__, grpName, username);
#if (WEB_AUTH_SHARE == FYES)
	int i = 0, j = -1;
#endif
    /*���ź���Ԥ��*/
    webUserIndex = ProfGetInstPointByValue(mibType, 
	    OFFANDSIZE(WebAuthProfile, user), TSTR, username, -1);
    if(webUserIndex != PROFFAIL)
    {
	webUser = ProfGetInstPointByIndex(mibType, webUserIndex);
    }
    if(ProfInstIsFree(webUser) == 0)
    { /*�˺�ʹ����*/
#if (WEB_AUTH_SHARE == FYES)
#if DEBUG
	printf("%s-%d: prof actualSession =%d\n",__func__,__LINE__,webUser->actualSession);
#endif
	j = webUser->sesIndex;
	for(i=0;i<webUser->actualSession;i++)
	{
	    if(checkSessionIndex(j) != 1)
	    {
		break;
	    }
#if DEBUG
	    printf("%s-%d: session ip =0x%x\n", __func__, __LINE__,nvramProfile->session[j].ip); 
#endif
	    ipsetAOrDIp(grpName, nvramProfile->session[j].ip, addOrDel);/*������Ӧ��ipset��*/
	    j = nvramProfile->session[j].nextIndex;
	}
#else /*WEB_AUTH_SHARE*/
	SWORD_PRINTF("%s--------%d, webUser->ip = %x\n", __func__, __LINE__, webUser->ip); 
	if(webUser->ip != 0u)
	{/*����*/
	    ipsetAOrDIp(grpName, webUser->ip, addOrDel);/*������Ӧ��ipset��*/
	}
#endif /*WEB_AUTH_SHARE*/
    }

    /*�ͷ��ź���Ԥ��*/
    return;
}
/*���������ַ��*/
void IPGrpUpWebExIp(char* grpName)
{

	MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
	WebAuthGlobalProfile *prof = NULL;
	int i=0;

	prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)&&(strcmp(prof->exceptIpGroup,grpName) == 0))
	{
	SWORD_PRINTF("%s--------%d, add webauth ipgroup\n", __func__, __LINE__); 
	webauthAddexceptIpGroup();
	}
	return;
}

#endif
/**
 * ����ip��ַ��
 * porf:ip��ַ��porfileָ��
 * setName:��ӵ���ipset��
 */
void ipGrpBuildSet(IpGrpProfile *prof, char *setName)
{
    IpGrpProfile *subGrp = NULL;
    int i = 0, maxEntrys = 0;

    if(prof->grpType == IPGRP_IP)
    {
	maxEntrys =  IPGRP_IP_ENTRYS;
    }
    else
    {
	maxEntrys = IPGRP_USER_ENTRYS;
    }
    SWORD_PRINTF("%s--------%d, maxEntrys = %d, setName = %s\n",__func__, __LINE__, maxEntrys, setName);
    /*ѭ�����������������Ŀ*/
    for(i = 0; i < maxEntrys; i++)
    {
	if(prof->ipEntry[i].entryType == IPENT_RANGE)
	{/*����Ŀ����Ϊip��Χ*/
	    SWORD_PRINTF("%s--------%d, prof->ipEntry[i].range.ipFrom = %x, prof->ipEntry[i].range.ipTo = %x\n",__func__, __LINE__, prof->ipEntry[i].entryVal.range.ipFrom, prof->ipEntry[i].entryVal.range.ipTo);
	    ipsetAOrDIpRange(setName,
		    prof->ipEntry[i].entryVal.range.ipFrom,
		    prof->ipEntry[i].entryVal.range.ipTo,
		    IPSET_MEM_ADD);/*��ӵ���Ӧ��ipset��*/
	}

#if (PPPOE_SERVER == FYES)
	else if(prof->ipEntry[i].entryType == IPENT_POE_ACC)
	{/*����Ŀ����Ϊpppoe �˺�*/
	    SWORD_PRINTF("%s--------%d, prof->ipEntry[i].entryType = %d\n",__func__, __LINE__, prof->ipEntry[i].entryType);
	    ipGrpUpPoe(setName, prof->ipEntry[i].entryVal.name, IPSET_MEM_ADD);
	}
#endif

#if (WEB_AUTH == FYES)
	else if(prof->ipEntry[i].entryType == IPENT_WEB_AUTH)
	{/*����Ŀ����Ϊweb��֤�û�*/
	    SWORD_PRINTF("%s--------%d, prof->ipEntry[i].entryType = %d\n",__func__, __LINE__, prof->ipEntry[i].entryType);
	    ipGrpUpWeb(setName, prof->ipEntry[i].entryVal.name, IPSET_MEM_ADD);
	}
#endif
	else if(prof->ipEntry[i].entryType == IPENT_GRP)
	{/*����Ŀ����Ϊ������ַ��*/
	    SWORD_PRINTF("%s--------%d, prof->ipEntry[i].entryType = %d\n",__func__, __LINE__, prof->ipEntry[i].entryType);
	    subGrp = (IpGrpProfile*)ProfGetInstPointByName(MIB_PROF_IPGRP, prof->ipEntry[i].entryVal.name);
	    if(ProfInstIsFree(subGrp) == 0)/*����ʹ����*/
	    {
		ipGrpBuildSet(subGrp, setName);/*�����������ݵ�setName*/
	    }
	}
	else
	{
	}
    }
    return;

}

/********************************************************************
* ������ ipGroupInit
* ���ܣ� ��֤��ʼ������ - ��������й����������������
* ������ 2012-05-04
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void ipGroupInit(void)
{
    MibProfileType mibType = MIB_PROF_IPGRP;
    IpGrpProfile* prof = NULL;

    int i = 0, min = 0, max = 0;

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) 
    {
	prof = (IpGrpProfile*)ProfGetInstPointByIndex(mibType, i);
	if(ProfInstIsFree(prof) == 0)/*����ʹ����*/
	{
	    /*���ﲻɾ���顣��Ϊ�˷�ֹ speedweb�����������
	     * ��ɵ� iptables���ø���Ĺ���Ϊɾ��
	     * ��ʱ�齫ɾ������������Ҳ����*/
	    ipsetFlushGrp(prof->head.name); /*�������*/
	    ipGrpAdd(prof);
	}  
    }

    return;
}
/********************************************************************
* ������ funInitIpGroup
* ���ܣ� ��֤���� ��ʼ����ں���
* ������ 2012-05-04
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
void funInitIpGroup(void)
{
    SWORD_PRINTF("%s------%d\n",__func__, __LINE__);
    registerForProfileUpdates(MIB_PROF_IPGRP, (Pointer)NULL, (ProfileUpdateFnc)exceptIpGroupProfileChange);/*��������*/

    SWORD_PRINTF("%s------%d\n",__func__, __LINE__);
    ipGroupInit();/*��ʼ��*/
#if (MULTI_LAN == FYES)
    doSystem("ipset -q -N %s iphash", SYS_LAN_GROUP);
    doSystem("ipset -q -N %s nethash --hashsize 4", SYS_LAN_RANGE_GROUP);
#endif
    return;
}
#endif
