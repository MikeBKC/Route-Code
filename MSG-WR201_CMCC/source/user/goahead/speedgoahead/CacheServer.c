#include "uttMachine.h"
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"
#include "utt_conf_define.h"
#if 0
#if (WEB_AUTH == FYES)
extern void webauthUpdatePort(const CacheServerProfile *oldProfileDataPtr);
#endif
#if (NOTICE == FYES)
extern void noticeUpdateSport(const CacheServerProfile *oldProfileDataPtr);
#endif
#endif

/********************************************************************
 * 函数： CacheServerInit
 * 功能： 初始化函数
 * 创建： 2013-04-03
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： chen.bin 
 ********************************************************************/ 
static void CacheServerInit(void) 
{
    CacheServerProfile *profCacheServer = NULL;
    MibProfileType mibType = MIB_PROF_CACHE_SERVER;
    profCacheServer = (CacheServerProfile*)ProfGetInstPointByIndex(mibType, 0);
	char ip[16] = {0};
	getIfIp(getLanIfName(),ip);

    if((profCacheServer != NULL) ) 
    {
		profCacheServer->head.active = FUN_DISABLE;
//		nvramWriteCommit();
		if(profCacheServer->cacheServerEnable == FUN_ENABLE){
		/*添加数据转发规则*/
			doSystem("cache_check.sh %s&",profCacheServer->cacheServerIp);
		}
    }
    return;
}

extern void cacheNatRuleAOrD(int sta){
	CacheServerProfile *profCacheServer = NULL;
	LogManageProfile *profLog = NULL;
    MibProfileType mibType = MIB_PROF_CACHE_SERVER;
    profCacheServer = (CacheServerProfile*)ProfGetInstPointByIndex(mibType, 0);
	profLog = (LogManageProfile *)ProfGetInstPointByIndex(MIB_PROF_LOG_MANAGE,0);
	char ip[16] = {0};
	Uint32 port = 80u;
	int logSwitch = 0;
	getIfIp(getLanIfName(),ip);
	doSystem("iptables -t mangle -F CACHE_EXCEPTION >/dev/null 2>&1");
	doSystem("iptables -t nat -F CACHE_CHAIN_DNAT >/dev/null 2>&1");
	doSystem("iptables -t nat -F CACHE_CHAIN_SNAT >/dev/null 2>&1");
	if(profLog != NULL){
		logSwitch = ((profLog->webLogEnable) | (profLog->QQLogEnable << 1) | (profLog->MSNLogEnable << 2)| (profLog->mailLogEnable << 3));
		doSystem("iptables -t mangle -D POSTROUTING -j UTTLOG --config 0x%x --http-port 80", logSwitch);
		doSystem("iptables -t mangle -D POSTROUTING -j UTTLOG --config 0x%x --http-port %u", logSwitch,profCacheServer->cacheServerPort);
	}
	if(sta == 1){
	    if((profCacheServer != NULL) ) 
	 {
		if(profCacheServer->cacheServerEnable == FUN_ENABLE){
		/*添加数据转发规则*/
			profCacheServer->head.active = FUN_ENABLE;
			nvramWriteCommit();
			port =  profCacheServer->cacheServerPort;
			doSystem("iptables -t mangle -A CACHE_EXCEPTION -p tcp --dport 80 -m  layer7  --l7proto httpRequest -j RETURN");
			doSystem("iptables -t mangle -A CACHE_EXCEPTION  ! -d %s -p tcp --dport 80 -m  layer7  --l7proto nonZeroLen -j SET --add-set cache_exception_dst dst",ip);
			doSystem("iptables -t nat -A  CACHE_CHAIN_DNAT ! -d %s -m set ! --set %s src -m set ! --set %s dst  -p tcp --dport 80 -j DNAT --to %s:%u",ip,SYS_CACHE_EXCEPTION,CACHE_EXCEPTION_DST,profCacheServer->cacheServerIp,profCacheServer->cacheServerPort);
			doSystem("iptables -t nat -A  CACHE_CHAIN_SNAT  -d %s  -p tcp --dport %u -j SNAT --to %s",profCacheServer->cacheServerIp,profCacheServer->cacheServerPort,ip);
			if(logSwitch != 0){
					doSystem("iptables -t mangle -A POSTROUTING -j UTTLOG --config 0x%x --http-port %u", logSwitch,profCacheServer->cacheServerPort);
			}
			ipsetAOrDIpStr(SYS_WEB_AUTH_OK,profCacheServer->cacheServerIp,IPSET_MEM_ADD);
		}
	 }
 
	}else{
		profCacheServer->head.active = FUN_DISABLE;
		nvramWriteCommit();
		if(logSwitch !=0 ){
			doSystem("iptables -t mangle -A POSTROUTING -j UTTLOG --config 0x%x --http-port 80", logSwitch);
		}
		ipsetAOrDIpStr(SYS_WEB_AUTH_OK,profCacheServer->cacheServerIp,IPSET_MEM_DEL);
	}

	ipsetFlushGrp(SYS_HTTP_PORT);
	httpPortAdd(SYS_HTTP_PORT,port);

}

/********************************************************************
 * 函数： CacheServerProfileChange
 * 功能： profile change 函数
 * 创建： 2013-04-03
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： chen.bin
 ********************************************************************/ 
static void CacheServerProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const CacheServerProfile *oldProfileDataPtr, const CacheServerProfile *newProfileDataPtr)
{
    switch(changeType) 
    {
	case PROF_CHANGE_ADD:
	case PROF_CHANGE_EDIT:
	    if(memcmp(oldProfileDataPtr, newProfileDataPtr, sizeof(CacheServerProfile)) != 0)
	    {
			//清除规则
			doSystem("iptables -t nat -F CACHE_CHAIN_DNAT >/dev/null 2>&1");
			doSystem("iptables -t nat -F CACHE_CHAIN_SNAT >/dev/null 2>&1");
//			CacheServerInit();
			buildCacheExceptionGrp();
			if((oldProfileDataPtr->cacheServerEnable!=newProfileDataPtr->cacheServerEnable)){

				if(newProfileDataPtr->cacheServerEnable == FUN_ENABLE){
					doSystem("cache_check.sh %s&",newProfileDataPtr->cacheServerIp);
				}else{
					doSystem("killall -q cachecheck");
				}
				ipsetFlushGrp(SYS_HTTP_PORT);
				httpPortAdd(SYS_HTTP_PORT,80u);
#if 0
#if (WEB_AUTH == FYES)
			webauthUpdatePort(oldProfileDataPtr);
#endif
#if (NOTICE == FYES)
			noticeUpdateSport(oldProfileDataPtr);
#endif
#endif
			}else{
				doSystem("killall -q cachecheck");
				doSystem("cache_check.sh %s&",newProfileDataPtr->cacheServerIp);
			}

	    }
	    break;
	case PROF_CHANGE_DEL:
	case PROF_CHANGE_DELALL:
	default:/*test*/
	    break;
    }
    return;
}

extern void buildCacheExceptionGrp(void){
    CacheServerProfile *profCacheServer = NULL;
    MibProfileType mibType = MIB_PROF_CACHE_SERVER;
    profCacheServer = (CacheServerProfile*)ProfGetInstPointByIndex(mibType, 0);
	ipsetFlushGrp(SYS_CACHE_EXCEPTION);
	doSystem("ipset -q -A %s %s",SYS_CACHE_EXCEPTION,profCacheServer->cacheServerIp);
	int min = 0,max=0,i=0;
	MibProfileType type = MIB_PROF_NAT_RULE;
	NatRuleProfile *prof = NULL;
	ProfInstLowHigh(type,&max,&min);
	for(i=min; i<max ; i++){
		prof = (NatRuleProfile*)ProfGetInstPointByIndex(type, i);
		if(prof != NULL && ProfInstIsFree(prof) == 0) {
			ipsetAOrDIpRange(SYS_CACHE_EXCEPTION,prof->InFromIP,prof->InEndIP,IPSET_MEM_ADD);
		}
	}
	
}

/********************************************************************
 * 函数： funInitCacheServer
 * 功能： 初始化函数
 * 创建： 2013-04-03
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： chen.bin
 ********************************************************************/ 
extern void funInitCacheServer(void) 
{
	
    registerForProfileUpdates(MIB_PROF_CACHE_SERVER,(Pointer)NULL, (ProfileUpdateFnc)CacheServerProfileChange);
	doSystem("iptables -t mangle -N CACHE_EXCEPTION");
	doSystem("iptables -t nat -N CACHE_CHAIN_DNAT");
	doSystem("iptables -t nat -N CACHE_CHAIN_SNAT");
	doSystem("iptables -t mangle -F CACHE_EXCEPTION>/dev/null 2>&1");
	doSystem("iptables -t nat -F CACHE_CHAIN_DNAT >/dev/null 2>&1");
	doSystem("iptables -t nat -F CACHE_CHAIN_SNAT >/dev/null 2>&1");
	doSystem("iptables -t mangle -A PREROUTING -j CACHE_EXCEPTION");
	doSystem("iptables -t nat -A PREROUTING -j CACHE_CHAIN_DNAT");
	doSystem("iptables -t nat -A POSTROUTING -j CACHE_CHAIN_SNAT");
	ipGrpIpAddSet(SYS_CACHE_EXCEPTION);
	ipGrpIpAddSet(CACHE_EXCEPTION_DST);
	buildCacheExceptionGrp();
    CacheServerInit();
    return;

}
