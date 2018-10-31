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
#ifdef CONFIG_UTT_NETFILTER
#include <linux/netfilter_ipv4/utt_netfilter.h>
extern int spdNfNlSock;
#endif

#if (LOG_MANAGE == FYES)
#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

static void logManageProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const LogManageProfile *oldProfileDataPtr, const LogManageProfile *newProfileDataPtr );
#if 0
static void logManageKlog(LogManageProfile *oldProf, LogManageProfile *newProf);
#endif
static void logManageChainAOrD(LogManageProfile *prof, char action);
static void logManageEim(LogManageProfile *prof);
static void logManageInit(void);

/********************************************************************
* 函数： logManageProfileChange
* 功能： 行为审计配置变化 生效函数
* 创建： 2012-04-17
* 参数： 略
* 返回： 略
* 输出： 略
* 作者： Jianqing.Sun 
********************************************************************/
static void logManageProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const LogManageProfile *oldProfileDataPtr, const LogManageProfile *newProfileDataPtr )
{
    switch(changeType) 
    {
        case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
#if 0
	    logManageKlog(oldProfileDataPtr, newProfileDataPtr);
#endif
	    logManageChainAOrD((LogManageProfile *)oldProfileDataPtr, Iptables_DEL);
	    logManageChainAOrD((LogManageProfile *)newProfileDataPtr, Iptables_ADD);
	    logManageEim((LogManageProfile *)newProfileDataPtr);
	    break;
        case PROF_CHANGE_DEL:
        case PROF_CHANGE_DELALL:
        default:/*default*/
            break;
    }
    return;
}
#if 0
static void logManageKlog(LogManageProfile *oldProf, LogManageProfile *newProf)
{
    int logSwitchOld = 0;
    int logSwitchNew = 0;
    if((oldProf != NULL) && (newProf != NULL))
    {
	logSwitchOld = ((oldProf->webLogEnable) | (oldProf->QQLogEnable << 1) | (oldProf->MSNLogEnable << 2)| (oldProf->mailLogEnable << 3));
	logSwitchNew = ((newProf->webLogEnable) | (newProf->QQLogEnable << 1) | (newProf->MSNLogEnable << 2)| (newProf->mailLogEnable << 3));
	SWORD_PRINTF("%s------%d, logSwitchOld = %x, logSwitchNew= %x\n",__func__, __LINE__, logSwitchOld, logSwitchNew);
	if((logSwitchOld == 0) && (logSwitchNew != 0))
	{
	    doSystem("klogd");
	    SWORD_PRINTF("%s------%d, klogd\n",__func__, __LINE__);
	}
	else if((logSwitchOld != 0) && (logSwitchNew == 0))
	{
	    doSystem("killall klogd");
	    SWORD_PRINTF("%s------%d, killall klogd\n",__func__, __LINE__);
	}
	else
	{
	    /*test*/
	}
    }
    else if((oldProf == NULL) && (newProf != NULL))
    {
	logSwitchNew = ((newProf->webLogEnable) | (newProf->QQLogEnable << 1) | (newProf->MSNLogEnable << 2)| (newProf->mailLogEnable << 3));
	if(logSwitchNew != 0)
	{
	    doSystem("klogd");
	    SWORD_PRINTF("%s------%d, klogd\n",__func__, __LINE__);
	}
    }
    else
    {
	/*test*/
    }
}
#endif
/*增加或删除网页行为审计规则链到iptables规则
 * ipstr : lan口ip
 * action: Iptables_ADD or Iptables_DEL*/
static void logManageChainAOrD(LogManageProfile *prof, char action)
{
    int logSwitch = 0;
#if (CACHE_SERVER == FYES)
	CacheServerProfile *profCacheServer = NULL;
	profCacheServer = (CacheServerProfile *)ProfGetInstPointByIndex(MIB_PROF_CACHE_SERVER,0);
#endif
    if(prof != NULL)
    {
	logSwitch = ((prof->webLogEnable) | (prof->QQLogEnable << 1) | (prof->MSNLogEnable << 2)| (prof->mailLogEnable << 3)
#if CONFIG_XPORT_LOG 
 | (prof->luntanLogEnable << 4)  | (prof->weiboLogEnable << 5)
#endif
);
	SWORD_PRINTF("%s------%d, logSwitch = %x\n",__func__, __LINE__, logSwitch);
	if(logSwitch != 0)
	{
#if 0
	    doSystem("iptables -t mangle -%c PREROUTING -j UTTLOG --config 0x%x", action, logSwitch);
	    SWORD_PRINTF("iptables -t mangle -%c PREROUTING -j UTTLOG --config 0x%x\n", action, logSwitch);
#endif
		Uint32 port = 80u;
#if (CACHE_SERVER == FYES)
		if((profCacheServer!=NULL)&&(profCacheServer->head.active)){
			port = profCacheServer->cacheServerPort;
		}
#endif
		if(action==Iptables_DEL){
#if (CACHE_SERVER == FYES)
			 doSystem("iptables -t mangle -%c POSTROUTING -j UTTLOG --config 0x%x --http-port %u", action, logSwitch,port);
			SWORD_PRINTF("iptables -t mangle -%c POSTROUTING -j UTTLOG --config 0x%x --http-port %u\n", action, logSwitch,port);
#endif
			 doSystem("iptables -t mangle -%c POSTROUTING -j UTTLOG --config 0x%x --http-port 80", action, logSwitch);
		}else{
		    doSystem("iptables -t mangle -%c POSTROUTING -j UTTLOG --config 0x%x --http-port %u", action, logSwitch,port);
			 SWORD_PRINTF("iptables -t mangle -%c POSTROUTING -j UTTLOG --config 0x%x --http-port %u\n", action, logSwitch,port);
		}
	}
    }
    return;
}

static void logManageEim(LogManageProfile *prof) 
{
    UttNlNfEimConf nlMsg;
    memset(&nlMsg, 0, sizeof(nlMsg));

    nlMsg.msgHead.nlType = UTT_NLNF_EIM;
    nlMsg.entryVal.button = prof->eimLogEnable;
#if CONFIG_XPORT_LOG
    nlMsg.entryVal.natLogEnable = prof->NatLogEnable;
#endif
    uttNfNlSend(spdNfNlSock,(char*)&nlMsg, sizeof(nlMsg), 0 ,0 );/*发送消息给内核*/
    return;
}
/********************************************************************
* 函数： logManageInit
* 功能： 行为审计初始化函数 - 先清空现有规则链表，再重新添加
* 创建： 2012-04-17
* 参数： 略
* 返回： 略
* 输出： 略
* 作者： Jianqing.Sun 
********************************************************************/
static void logManageInit(void)
{
    MibProfileType mibType = MIB_PROF_LOG_MANAGE;
    LogManageProfile *prof = NULL;

    prof = (LogManageProfile *)ProfGetInstPointByIndex(mibType, 0);
    logManageChainAOrD(prof, Iptables_ADD);
    logManageEim(prof);
#if 0
    logManageKlog(NULL, prof);
#endif

    return;
}

/********************************************************************
* 函数： funInitLogManage
* 功能： 行为审计功能 初始化入口函数
* 创建： 2012-04-17
* 参数： 略
* 返回： 略
* 输出： 略
* 作者： Jianqing.Sun 
********************************************************************/
void funInitLogManage(void)
{
    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
    registerForProfileUpdates(MIB_PROF_LOG_MANAGE, (Pointer)NULL, (ProfileUpdateFnc)logManageProfileChange);/*触发函数*/

    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
    logManageInit();/*初始化*/
    return;
}
#endif
