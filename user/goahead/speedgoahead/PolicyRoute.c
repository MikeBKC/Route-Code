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
#include "pdb.h"

#if (POLICY_ROUTE == FYES)
#ifdef CONFIG_UTT_NETFILTER
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICYROUTE
#define DBGPRINT policy_route_debug_print
#include <linux/netfilter_ipv4/utt_netfilter.h>
extern int spdNfNlSock;
#if 1
static int uttNfNlPolicyRouteDelAll(void);
static void uttNfNlPolicyRouteAdd(void);

static void policyouteProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const PolicyRouteProfile *oldProfileDataPtr, const PolicyRouteProfile *newProfileDataPtr );
static void policyrouteglobalProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr );
static void policyrouteInit(void);


/********************************************************************
* 函数： uttNfNlPolicyRouteDelAll(void)
* 功能： 删除内核对应的所有路由策略
* 创建： 2013-02-28
* 参数： 略
* 返回： 略
* 输出： 略
* 作者：  
********************************************************************/
static int uttNfNlPolicyRouteDelAll(void)
{
#if 1
    UttNlNfPolicyRouteConf  nlConf;

    uttSemP(SEM_UTTNF_NO,0);/*加信号量防止冲突*/
    memset(&nlConf, 0, sizeof(nlConf));
    nlConf.msgHead.nlType = UTT_NLNF_POLICY_ROUTE;
    /*删除所有*/
    nlConf.confType = UTT_NFCONF_DEL_ALL;
#ifdef CONFIG_UTT_DEBUG
#ifdef CONFIG_UTT_DEB_POLICY_ROUTE
    nlConf.debugType = UTT_DEBUG_INIT;
#endif
#endif

    DBGPRINT("%s-%d: nltype =%d, conftype =%d\n",__func__, __LINE__,nlConf.msgHead.nlType,nlConf.confType);
    uttNfNlSend(spdNfNlSock,(char*)&nlConf, sizeof(nlConf), 0U ,0U );/*发送消息给内核*/
    uttSemV(SEM_UTTNF_NO,0);/*如果加了锁。则应该释放该锁*/
#endif
    DBGPRINT("%s-%d\n",__func__, __LINE__);
    return 0;
}

/********************************************************************
* 函数： uttNfNlPolicyRouteAdd(void) 
* 功能： 增加配置
* 创建： 2013-02-28
* 参数： 略
* 返回： 略
* 输出： 略
* 作者：  
********************************************************************/
static void uttNfNlPolicyRouteAdd(void) 
{
#if 1
    MibProfileType mibType = MIB_PROF_POLICY_ROUTE;
    PolicyRouteProfile *prof = NULL;
    UttNlNfPolicyRouteConf  nlConf;
    int min, max, index;

    memset(&nlConf, 0, sizeof(nlConf));
    nlConf.msgHead.nlType = UTT_NLNF_POLICY_ROUTE;
    /*增加*/
    nlConf.confType = UTT_NFCONF_ADD;
#ifdef CONFIG_UTT_DEBUG
#ifdef CONFIG_UTT_DEB_POLICY_ROUTE
    nlConf.debugType = UTT_DEBUG_INIT;
#endif
#endif
    DBGPRINT("%s-%d: nltype =%d, conftype =%d\n",__func__, __LINE__,nlConf.msgHead.nlType,nlConf.confType);

    uttSemP(SEM_UTTNF_NO,0);/*加信号量防止冲突*/
    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++) 
    {
	prof = (PolicyRouteProfile*)ProfGetInstPointByIndex(mibType, index); /*get prof pointer*/
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)
		&& (prof->head.active == TRUE))
	{
	    
	    nlConf.entryVal.bindMark = strtoul(prof->interfaceName+3, NULL, 16)*0x10;
		DBGPRINT("%s-%d, index =%d,bindMark =0x%x\n",__func__,__LINE__,index,nlConf.entryVal.bindMark);
	    /*源地址*/
	    if(prof->ip.srcIP.entryType == PR_IP_RANGE) {	/*地址段*/
		nlConf.entryVal.ip.srcIP.entryType = IP_RANGE;
		nlConf.entryVal.ip.srcIP.entryVal.range.ipFrom = ntohl(prof->ip.srcIP.entryVal.range.ipFrom);
		nlConf.entryVal.ip.srcIP.entryVal.range.ipTo = ntohl(prof->ip.srcIP.entryVal.range.ipTo);
		DBGPRINT("%s-%d, srcip type =%d,ipfrom =0x%x,ipto =0x%x\n",__func__,__LINE__,
			nlConf.entryVal.ip.srcIP.entryType,nlConf.entryVal.ip.srcIP.entryVal.range.ipFrom,nlConf.entryVal.ip.srcIP.entryVal.range.ipTo);
	    }else if(prof->ip.srcIP.entryType == PR_IP_GROUP) {	    /*地址组*/
		/*如果地址组为所有地址时，
		 * 保存为起始地址和结束地址都为0的地址段形式*/
		if(strlen(prof->ip.srcIP.entryVal.name) == 0) {
		nlConf.entryVal.ip.srcIP.entryType = IP_RANGE;
		nlConf.entryVal.ip.srcIP.entryVal.range.ipFrom = 0U;
		nlConf.entryVal.ip.srcIP.entryVal.range.ipTo = 0U;
		DBGPRINT("%s-%d, srcip type =%d,ipfrom =0x%x,ipto =0x%x\n",__func__,__LINE__,
			nlConf.entryVal.ip.srcIP.entryType,nlConf.entryVal.ip.srcIP.entryVal.range.ipFrom,nlConf.entryVal.ip.srcIP.entryVal.range.ipTo);
		
		}else{
		nlConf.entryVal.ip.srcIP.entryType = IP_GROUP;
		strncpy(nlConf.entryVal.ip.srcIP.entryVal.name, prof->ip.srcIP.entryVal.name ,PR_IPGRP_NAME_LEN);
		DBGPRINT("%s-%d, srcip type =%d,ipgrpname =%s\n",__func__,__LINE__,
			nlConf.entryVal.ip.srcIP.entryType,nlConf.entryVal.ip.srcIP.entryVal.name);
		}
	    }
	    /*目的地址*/
	    if(prof->ip.dstIP.entryType == PR_IP_RANGE) {	
		nlConf.entryVal.ip.dstIP.entryType = IP_RANGE;
		nlConf.entryVal.ip.dstIP.entryVal.range.ipFrom = ntohl(prof->ip.dstIP.entryVal.range.ipFrom);
		nlConf.entryVal.ip.dstIP.entryVal.range.ipTo = ntohl(prof->ip.dstIP.entryVal.range.ipTo);
		DBGPRINT("%s-%d, dstip type =%d,ipfrom =0x%x,ipto =0x%x\n",__func__,__LINE__,
			nlConf.entryVal.ip.dstIP.entryType,nlConf.entryVal.ip.dstIP.entryVal.range.ipFrom,nlConf.entryVal.ip.dstIP.entryVal.range.ipTo);
	    }else if(prof->ip.dstIP.entryType == PR_IP_GROUP) {
		if(strlen(prof->ip.dstIP.entryVal.name) == 0) {
		nlConf.entryVal.ip.dstIP.entryType = IP_RANGE;
		nlConf.entryVal.ip.dstIP.entryVal.range.ipFrom = 0U;
		nlConf.entryVal.ip.dstIP.entryVal.range.ipTo = 0U;
		
		}else{
		nlConf.entryVal.ip.dstIP.entryType = IP_GROUP;
		strncpy(nlConf.entryVal.ip.dstIP.entryVal.name, prof->ip.dstIP.entryVal.name ,PR_IPGRP_NAME_LEN);
		DBGPRINT("%s-%d, dstip type =%d,ipgrpname =%s\n",__func__,__LINE__,
			nlConf.entryVal.ip.dstIP.entryType,nlConf.entryVal.ip.dstIP.entryVal.name);
		}
	    }
	    /*保存协议、端口等信息*/
	    nlConf.entryVal.apply.protocol = prof->apply.protocol;
	    nlConf.entryVal.apply.dstFromPort = prof->apply.dstFromPort;
	    nlConf.entryVal.apply.dstEndPort = prof->apply.dstEndPort;
	    nlConf.entryVal.prTime.wday = prof->prtime.wday;
	    nlConf.entryVal.prTime.timestart = prof->prtime.timestart;
	    nlConf.entryVal.prTime.timestop = prof->prtime.timestop;
	    DBGPRINT("%s-%d, apply protocol =%u,dstfromPort =%u,dstendPort =%u\n", __func__, __LINE__,
			nlConf.entryVal.apply.protocol,nlConf.entryVal.apply.dstFromPort,nlConf.entryVal.apply.dstEndPort);
	    DBGPRINT("%s-%d, prtime wday =0x%x,timestart =%u,timestop =%u\n", __func__, __LINE__,
		    nlConf.entryVal.prTime.wday,nlConf.entryVal.prTime.timestart,nlConf.entryVal.prTime.timestop);
	    uttNfNlSend(spdNfNlSock,(char*)&nlConf, sizeof(nlConf), 0U ,0U );/*发送消息给内核*/
	}
    }
    uttSemV(SEM_UTTNF_NO,0);/*如果加了锁。则应该释放该锁*/
#endif
    DBGPRINT("%s-%d\n",__func__, __LINE__);
    return;
}
/*
 * isPolicyRouteEnable
 * 判断策略路由总开关是否打开
 * 返回值:
 * */
static int isPolicyRouteEnable(void)
{
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    unsigned int ret = 0U;
    DBGPRINT("%s-%d\n",__func__, __LINE__);

    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibTypeSys, 0);
    if(profSys->policyrouteEn == FUN_ENABLE) 
    {
	ret = 1U;
    }
    return ret;
}
/********************************************************************
* 函数： policyrouteProfileChange
* 功能： 策略路由配置变化 生效函数
* 创建： 2013-02-28
* 参数： 略
* 返回： 略
* 输出： 略
* 作者：  
********************************************************************/
static void policyrouteProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const PolicyRouteProfile *oldProfileDataPtr, const PolicyRouteProfile *newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
        case PROF_CHANGE_DEL:
        case PROF_CHANGE_DELALL:
	    if(isPolicyRouteEnable() == 1U) {
		uttNfNlPolicyRouteDelAll(); /*删除所有配置*/
		uttNfNlPolicyRouteAdd();    /*然后在增加配置*/
		DBGPRINT("%s-%d: \n",__func__,__LINE__);
	    }
	    break;
        default:/*default*/
            break;
    }
    return;
}
/********************************************************************
* 函数： policyrouteglobalProfileChange
* 功能： 策略路由开关变化 生效函数
* 创建： 2013-02-28
* 参数： 略
* 返回： 略
* 输出： 略
* 作者：  
********************************************************************/
static void policyrouteglobalProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
	    break;
        case PROF_CHANGE_EDIT:
	    DBGPRINT("%s-%d: old global enable %d, new global enable %d\n",__func__,__LINE__,
		    oldProfileDataPtr->policyrouteEn,newProfileDataPtr->policyrouteEn);
	    if((oldProfileDataPtr->policyrouteEn == FUN_ENABLE) && (newProfileDataPtr->policyrouteEn == FUN_DISABLE))
	    {/*开启变为关闭*/
		uttNfNlPolicyRouteDelAll();
		DBGPRINT("%s-%d: \n",__func__,__LINE__);
	    }
	    else if((oldProfileDataPtr->policyrouteEn == FUN_DISABLE) && (newProfileDataPtr->policyrouteEn == FUN_ENABLE))
	    {/*关闭变为开启*/
		uttNfNlPolicyRouteAdd();
		DBGPRINT("%s-%d: \n",__func__,__LINE__);
	    }
	    else
	    {
		/*test*/
	    }
	    break;
        case PROF_CHANGE_DEL:
	    break;
        case PROF_CHANGE_DELALL:
	    break;
        default:/*default*/
            break;
    }
    return;
}
#endif
#if 0
static void addTest()
{
    UttNlNfPolicyRouteConf  nlConf;

    memset(&nlConf, 0, sizeof(nlConf));
    nlConf.msgHead.nlType = UTT_NLNF_POLICY_ROUTE;
    DBGPRINT("%s-%d\n",__func__, __LINE__);
    /*增加*/
    nlConf.confType = UTT_NFCONF_ADD;
    uttNfNlSend(spdNfNlSock,(char*)&nlConf, sizeof(nlConf), 0U ,0U );/*发送消息给内核*/
}
#endif
/********************************************************************
* 函数： policyrouteInit
* 功能： 策略路由初始化函数
* 创建： 2013-02-28
* 参数： 略
* 返回： 略
* 输出： 略
* 作者：  
********************************************************************/
static void policyrouteInit(void)
{
#if 1
    if(isPolicyRouteEnable() == 1U) 
    {
	/*如果总开关打开
	 * 则增加配置到内核*/
	uttNfNlPolicyRouteDelAll();
	uttNfNlPolicyRouteAdd();
	DBGPRINT("%s-%d\n",__func__, __LINE__);
    }
#endif
#if 0
    addTest();
#endif
    return;
}
/********************************************************************
* 函数： funInitPolicyRoute
* 功能： 策略路由功能 初始化入口函数
* 创建： 2013-02-28
* 参数： 略
* 返回： 略
* 输出： 略
* 作者：  
********************************************************************/
void funInitPolicyRoute(void)
{
#if 1
    DBGPRINT("%s-%d\n",__func__, __LINE__);
    registerForProfileUpdates(MIB_PROF_SYSTEM, (Pointer)NULL, (ProfileUpdateFnc)policyrouteglobalProfileChange);/*触发函数*/
    registerForProfileUpdates(MIB_PROF_POLICY_ROUTE, (Pointer)NULL, (ProfileUpdateFnc)policyrouteProfileChange);/*触发函数*/
#endif

    DBGPRINT("%s-%d\n",__func__, __LINE__);
    policyrouteInit();/*初始化*/
    return;
}
#endif
#endif
#endif
