#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/utsname.h>
#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"
#if (AC_WORK_MODE == FYES)

#define DEBUG_SWITCH 0
#if DEBUG_SWITCH
#define DEBUG_PRINTF(fmt, args...)               printf(fmt, ## args)
#else
#define DEBUG_PRINTF(fmt, args...)
#endif

/***************************************************
 * 函数：add_route_and_dns
 * 功能：添加路由和dns
 * 参数：无
 * 返回：无
 * 作者：hyy
 * 日期：2013-12-10
 **************************************************/ 
extern void add_route_and_dns(AcWorkModeProfile *prof)
{
    DEBUG_PRINTF("%s-%d\n",__func__, __LINE__);
	/*add route*/
	char cmdstr[128];
	struct in_addr addr_gw, addr_dns;

    DEBUG_PRINTF("%s-%dgw=%x\n",__func__, __LINE__);
	memset(cmdstr,0,128);
	addr_gw.s_addr = prof->gateWay;
	addr_dns.s_addr = prof->pDns;
	if(prof->gateWay != 0){
		sprintf(cmdstr, "ip route add default table main via %s", inet_ntoa(addr_gw));
		doSystem(cmdstr);
		sprintf(cmdstr, "ip route change default table main via %s", inet_ntoa(addr_gw));
		doSystem(cmdstr);
	}else{
		doSystem("ip route del default table main");
	}

	/*add dns*/
	if(prof->pDns != 0)
	{
		DEBUG_PRINTF("%s-%d\n",__func__, __LINE__);
		doSystem("killall -q dnsmasq");
#if(IPV6 == FYES)
		doSystem("echo 'nameserver %s' >/tmp/ipv4resolv.conf",inet_ntoa(addr_dns));
		doSystem(" cat /tmp/ipv4resolv.conf >/etc/resolv.conf");
                doSystem(" cat /tmp/ipv6resolv.conf >>/etc/resolv.conf");
#else
                doSystem("echo 'nameserver %s' >/etc/resolv.conf",inet_ntoa(addr_dns));
#endif
		doSystem("dnsmasq &");
	}
    DEBUG_PRINTF("%s-%d\n",__func__, __LINE__);

	return;
}
/***************************************************
 * 函数：del_route_and_dns
 * 功能：添加路由和dns
 * 参数：无
 * 返回：无
 * 作者：hyy
 * 日期：2013-12-10
 **************************************************/ 
void del_route_and_dns()
{
    DEBUG_PRINTF("%s-%d\n",__func__, __LINE__);
	int wanCount,i;

	/*del route*/
	doSystem("ip route del default table main");
	/*del dns*/
	doSystem("killall -q dnsmasq");
    DEBUG_PRINTF("%s-%d\n",__func__, __LINE__);

    wanCount = getWanSlotCount();
	if(wanCount > 0)
	{
		for(i = 1; i < wanCount; i++)
		{
			doSystem("wan.sh %d &",i);
		}
	}
	return;
}
/********************************************************************
* 函数： acWorkModeProfileChange
* 功能： 开关变化 生效函数
* 参数： 略
* 返回： 略
* 输出： 略
* 作者： hyy
* 创建： 2013-12-10
********************************************************************/
static void acWorkModeProfileChange(Pointer tag, MibProfileType profileType,
        ProfChangeType changeType, Uint32 index,
        const AcWorkModeProfile *oldProfileDataPtr, const AcWorkModeProfile *newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
			if(newProfileDataPtr->workMode == PROF_ENABLE)
			{
				DEBUG_PRINTF("%s-%d\n",__func__, __LINE__);
				add_route_and_dns(newProfileDataPtr);
			}else if(newProfileDataPtr->workMode == PROF_DISABLE)
			{
			    DEBUG_PRINTF("%s-%d\n",__func__, __LINE__);
				del_route_and_dns();
			}
        case PROF_CHANGE_DEL:
        case PROF_CHANGE_DELALL:
        default:/*default*/
            break;
    }
    return;
}
/****************************************************
 * 函数：funcInitApMailConf
 * 功能：apMail初始化
 * 参数：无
 * 返回：无
 * 作者：hyy
 * 日期：2013-11-04
 ***************************************************/
extern void funInitAcWorkMode(void)
{
    DEBUG_PRINTF("%s-%d\n",__func__, __LINE__);
    registerForProfileUpdates(MIB_PROF_AC_WORK_MODE, (Pointer)NULL, (ProfileUpdateFnc)acWorkModeProfileChange);/*触发函数*/
/*
    MibProfileType mibType = MIB_PROF_AC_WORK_MODE;
    AcWorkModeProfile *prof = NULL;
    prof = (AcWorkModeProfile*)ProfGetInstPointByIndex(mibType, 0);
	if(prof->workMode == PROF_ENABLE)
	{
		add_route_and_dns(prof);
	}
*/
	DEBUG_PRINTF("%s-%d\n",__func__, __LINE__);

	return;
}
#endif

