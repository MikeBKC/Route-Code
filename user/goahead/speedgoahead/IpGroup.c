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
* 函数： exceptIpGroupProfileChange
* 功能： 认证配置变化 生效函数
* 创建： 2012-05-04
* 参数： 略
* 返回： 略
* 输出： 略
* 作者： Jianqing.Sun 
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
	    if ((profSys->dnsFilterEn == FUN_ENABLE) && /*域名过滤开启*/
		 (profSys->dnsFilterControl.addrTypeEn == ADDR_IPGRP) && /*引用用户组*/
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
 * ip组添加
 */
static void ipGrpAdd(IpGrpProfile *prof)
{
    char *setName = prof->head.name;/*ipset组名*/
    SWORD_PRINTF("%s--------%d, prof->grpType = %d, setName = %s\n",__func__, __LINE__, prof->grpType, setName);
    if(prof->grpType == IPGRP_IP)
    {
	/*成员类型为固定ip*/
	ipGrpIpAddSet(setName);/*添加相应ipset组*/
    }
    else
    {
	if(prof->grpType == IPGRP_USER)
	{/*成员类型为用户名*/
	    ipGrpUserAddSet(setName);/*添加相应ipset组*/
	}
    }
    ipGrpBuildSet(prof, setName);/*构建该ipset组*/
    return;

}
/********************************************************************
* 函数： ipGrpEdit
* 功能： ip组修改
*	 若setName为被编辑的组名,所有引用setName的组被重新构造
* 创建： 2012-05-07
* 参数： 略
* 返回： 略
* 输出： 略
* 作者： Jianqing.Sun 
********************************************************************/
static void ipGrpEdit(IpGrpProfile *curGrp)
{
    MibProfileType mibType = MIB_PROF_IPGRP;
    IpGrpProfile* prof = NULL;

    int i = 0, min = 0, max = 0;
#if (GROUP_BASE == FYES)
    int clearKAct = 0;/*内核行为管理清除标志*/
    clearKAct = isIpGrpReredByNf(curGrp);/*若果该组被行为管理引用了则清除内核策略*/

    if(clearKAct == 1)
    {
	uttSemP(SEM_UTTNF_NO,0);/*加信号量防止冲突*/
	uttNfNlPolicyDelAll();/*发消息通知内核删除策略*/
    }
#endif
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) 
    {
	prof = (IpGrpProfile*)ProfGetInstPointByIndex(mibType, i);
	if(ProfInstIsFree(prof) == 0 && curGrp->grpType == prof->grpType)/*该组使用了*/
	{/*只遍历同类型的组*/
	    if(ipGrpDepSearch(prof, curGrp->head.name) == 0)/*prof引用到了组curGrpName*/
	    {
		SWORD_PRINTF("%s------%d, curGrp->head.name = %s\n",__func__, __LINE__, curGrp->head.name);
		ipsetFlushGrp(prof->head.name);/*清空原有组内容*/
		ipGrpBuildSet(prof, prof->head.name);/*重新构造组内容*/
#if (WEB_AUTH == FYES)
		IPGrpUpWebExIp(prof->head.name);
#endif
	    }
	}
    }
#if (GROUP_BASE == FYES)
    if(clearKAct == 1)
    {
	uttSemV(SEM_UTTNF_NO,0);/*如果加了锁。则应该释放该锁*/
    }
#endif
    return;
}
 /********************************************************************
 * 函数： ipGrpDel
 * 功能： ip组删除
 * 创建： 2012-05-07
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/
void ipGrpDel(IpGrpProfile *delProf)
{
     ipsetDelGrp(delProf->head.name, TRUE);
     return;
}

#if (PPPOE_SERVER == FYES)
 /********************************************************************
 * 函数： ipGrpUpPoe
 * 功能： 更新pppoe用户到相应的用户组
 * 创建： 2012-05-07
 * 参数： 
 *	  username:pppoe用户名
 *	  grpName：所属的用户组
 *	  addOrDel：更新的时候删除或添加当前用户到相应的ipset组
          IPSET_MEM_ADD/ IPSET_MEM_DEL
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/
void ipGrpUpPoe(char*grpName, char* username, char addOrDel)
{
    int poeAccIndex = 0;
    MibProfileType mibType = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *poeAcc = NULL;/*pppoe账号profile*/

    SWORD_PRINTF("%s--------%d, grpName = %s, username = %s\n", __func__, __LINE__, grpName, username);
    uttSemP(SEM_PPPOE_NO,0);/*加信号量。防止其他进程访问*/

    poeAccIndex = ProfGetInstPointByValue(mibType, 
	    OFFANDSIZE(PPPoESrvAccProfile, pppoename), TSTR, username, -1);
    if(poeAccIndex != PROFFAIL)
    {
	poeAcc = ProfGetInstPointByIndex(mibType, poeAccIndex);
    }
    if(ProfInstIsFree(poeAcc) == 0)
    { /*账号使用了*/
//	SWORD_PRINTF("%s--------%d, poeAcc->ConTime = %d\n", __func__, __LINE__, poeAcc->ConTime);
#if 0
	if(poeAcc->ConTime == 0U)
	{/*在线*/
	    ipsetAOrDIpStr(grpName, poeAcc->ip, addOrDel);/*更新相应的ipset组*/
	}
#else
	AccAddIpStr(poeAcc, grpName, addOrDel);
#endif
    }

    uttSemV(SEM_PPPOE_NO,0);/*释放信号量*/
    return;
}
#endif
#if (WEB_AUTH == FYES)
 /********************************************************************
 * 函数： ipGrpUpWeb
 * 功能： 更新web认证用户到相应的用户组
 * 创建： 2012-05-07
 * 参数： 
 *	  username:web认证用户名
 *	  grpName：所属的用户组
 *	  addOrDel：更新的时候删除或添加当前用户到相应的ipset组
          IPSET_MEM_ADD/ IPSET_MEM_DEL
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/
void ipGrpUpWeb(char*grpName, char* username, char addOrDel)
{
    int webUserIndex = 0;
    MibProfileType mibType = MIB_PROF_WEB_AUTH;
    WebAuthProfile *webUser = NULL;/*web认证用户profile*/
    SWORD_PRINTF("%s--------%d, grpName = %s, username = %s\n", __func__, __LINE__, grpName, username);
#if (WEB_AUTH_SHARE == FYES)
	int i = 0, j = -1;
#endif
    /*加信号量预留*/
    webUserIndex = ProfGetInstPointByValue(mibType, 
	    OFFANDSIZE(WebAuthProfile, user), TSTR, username, -1);
    if(webUserIndex != PROFFAIL)
    {
	webUser = ProfGetInstPointByIndex(mibType, webUserIndex);
    }
    if(ProfInstIsFree(webUser) == 0)
    { /*账号使用了*/
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
	    ipsetAOrDIp(grpName, nvramProfile->session[j].ip, addOrDel);/*更新相应的ipset组*/
	    j = nvramProfile->session[j].nextIndex;
	}
#else /*WEB_AUTH_SHARE*/
	SWORD_PRINTF("%s--------%d, webUser->ip = %x\n", __func__, __LINE__, webUser->ip); 
	if(webUser->ip != 0u)
	{/*在线*/
	    ipsetAOrDIp(grpName, webUser->ip, addOrDel);/*更新相应的ipset组*/
	}
#endif /*WEB_AUTH_SHARE*/
    }

    /*释放信号量预留*/
    return;
}
/*更新例外地址组*/
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
 * 构建ip地址组
 * porf:ip地址组porfile指针
 * setName:添加到的ipset组
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
    /*循环搜索该组的所有条目*/
    for(i = 0; i < maxEntrys; i++)
    {
	if(prof->ipEntry[i].entryType == IPENT_RANGE)
	{/*该条目类型为ip范围*/
	    SWORD_PRINTF("%s--------%d, prof->ipEntry[i].range.ipFrom = %x, prof->ipEntry[i].range.ipTo = %x\n",__func__, __LINE__, prof->ipEntry[i].entryVal.range.ipFrom, prof->ipEntry[i].entryVal.range.ipTo);
	    ipsetAOrDIpRange(setName,
		    prof->ipEntry[i].entryVal.range.ipFrom,
		    prof->ipEntry[i].entryVal.range.ipTo,
		    IPSET_MEM_ADD);/*添加到相应的ipset组*/
	}

#if (PPPOE_SERVER == FYES)
	else if(prof->ipEntry[i].entryType == IPENT_POE_ACC)
	{/*该条目类型为pppoe 账号*/
	    SWORD_PRINTF("%s--------%d, prof->ipEntry[i].entryType = %d\n",__func__, __LINE__, prof->ipEntry[i].entryType);
	    ipGrpUpPoe(setName, prof->ipEntry[i].entryVal.name, IPSET_MEM_ADD);
	}
#endif

#if (WEB_AUTH == FYES)
	else if(prof->ipEntry[i].entryType == IPENT_WEB_AUTH)
	{/*该条目类型为web认证用户*/
	    SWORD_PRINTF("%s--------%d, prof->ipEntry[i].entryType = %d\n",__func__, __LINE__, prof->ipEntry[i].entryType);
	    ipGrpUpWeb(setName, prof->ipEntry[i].entryVal.name, IPSET_MEM_ADD);
	}
#endif
	else if(prof->ipEntry[i].entryType == IPENT_GRP)
	{/*该条目类型为其他地址组*/
	    SWORD_PRINTF("%s--------%d, prof->ipEntry[i].entryType = %d\n",__func__, __LINE__, prof->ipEntry[i].entryType);
	    subGrp = (IpGrpProfile*)ProfGetInstPointByName(MIB_PROF_IPGRP, prof->ipEntry[i].entryVal.name);
	    if(ProfInstIsFree(subGrp) == 0)/*该组使用了*/
	    {
		ipGrpBuildSet(subGrp, setName);/*构建子组内容到setName*/
	    }
	}
	else
	{
	}
    }
    return;

}

/********************************************************************
* 函数： ipGroupInit
* 功能： 认证初始化函数 - 先清空现有规则链表，再重新添加
* 创建： 2012-05-04
* 参数： 略
* 返回： 略
* 输出： 略
* 作者： Jianqing.Sun 
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
	if(ProfInstIsFree(prof) == 0)/*该组使用了*/
	{
	    /*这里不删除组。是为了防止 speedweb重启等情况。
	     * 造成的 iptables引用该组的规则为删除
	     * 这时组将删除不掉。内容也不变*/
	    ipsetFlushGrp(prof->head.name); /*清空内容*/
	    ipGrpAdd(prof);
	}  
    }

    return;
}
/********************************************************************
* 函数： funInitIpGroup
* 功能： 认证功能 初始化入口函数
* 创建： 2012-05-04
* 参数： 略
* 返回： 略
* 输出： 略
* 作者： Jianqing.Sun 
********************************************************************/
void funInitIpGroup(void)
{
    SWORD_PRINTF("%s------%d\n",__func__, __LINE__);
    registerForProfileUpdates(MIB_PROF_IPGRP, (Pointer)NULL, (ProfileUpdateFnc)exceptIpGroupProfileChange);/*触发函数*/

    SWORD_PRINTF("%s------%d\n",__func__, __LINE__);
    ipGroupInit();/*初始化*/
#if (MULTI_LAN == FYES)
    doSystem("ipset -q -N %s iphash", SYS_LAN_GROUP);
    doSystem("ipset -q -N %s nethash --hashsize 4", SYS_LAN_RANGE_GROUP);
#endif
    return;
}
#endif
