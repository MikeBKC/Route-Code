#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "ifmisc.h"
#include "internet.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#if (MAC_FILTER == FYES)

static void macFilterInit(void);
static void editMacFilterJumpRule(char action, char *ifName);
static void editOneMacFilterRule(const MacFilterProfile *prof, char action);
static void macFilterDelAll(void);
extern void wirelessMacFilterGlobal(unsigned int index);
static void  WireMacFilterInit(void);
#if (WIRELESS == FYES)
extern void wirelessMacFilterDelAll(void);
#if (WIRELESS_5G == FYES)
extern void wireless5gMacFilterDelAll(void);
#endif
#endif
/*根据静态局部变量判断当前存在规则条数,
 * 无规则存在需要删除INPUT链到MAC过滤链的规则，
 * 有规则时需要保证跳转规则和链都存在*/
static void editMacFilterJumpRule(char action, char *ifName)
{
    MibProfileType mibType = MIB_PROF_MAC_FILTER_GLOBAL;
    MacFilterGlobalProfile *prof = NULL;
    static int macFilterCount = 0;
    static int buildFlag = 0;/*规则已添加标志 1--已添加 0--未添加*/

    if ((action == Iptables_ADD) || (action == Iptables_INS))/*添加规则*/
    {
	macFilterCount += 1;
    }
    else if (action == Iptables_DEL)/*删除规则*/
    {
	macFilterCount -= 1;
    }
    else if (action == Iptables_FLUSH) /*清空规则*/
    {
	macFilterCount = 0;
    }/*还有一种Iptables_SPECIAL，不改变计数*/

    prof = (MacFilterGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
    if (ProfInstIsFree(prof) == PROFOK) /*检查是否有效*/
    {
	if (buildFlag == 0)
	{
	    /*由无规则到有规则，或者不允许所有人*/
	    if ((macFilterCount > 0) || ((prof->head.active == TRUE) && (prof->action == MAC_ALLOW)))
	    {
		doSystem("arptables -N %s", MAC_FILTER_CHAIN);/*新建MAC地址过滤链*/
		doSystem("arptables -I INPUT -j %s -i %s+", MAC_FILTER_CHAIN, ifName);/*添加IMPUT到过滤链的跳转规则*/
		buildFlag = 1;
	    }
	}
	else/*跳转规则已添加过*/
	{
	    if ((macFilterCount <= 0) && ((prof->head.active != TRUE) || (prof->action != MAC_ALLOW))) /*有规则到无规则，且不是开启只允许状态*/
	    {
		doSystem("arptables -D INPUT -j %s -i %s+ 1>/dev/null 2>&1", MAC_FILTER_CHAIN, ifName);/*删除跳转规则*/
		doSystem("arptables -X %s 1>/dev/null 2>&1", MAC_FILTER_CHAIN);/*完全删除该过滤链*/
		buildFlag = 0;
	    }
	}
	if ((prof->head.active == TRUE) && (prof->action == MAC_ALLOW))
	{
	    doSystem("arptables -D %s -j DROP 1>/dev/null 2>&1", MAC_FILTER_CHAIN);
	    doSystem("arptables -A %s -j DROP", MAC_FILTER_CHAIN);
	}
    }
}

/*MAC地址过滤全局设置profilechange 触发函数*/
void macFilterGlobalProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const MacFilterGlobalProfile *oldProf, const MacFilterGlobalProfile *newProf)
{
    switch(changeType) {
	case PROF_CHANGE_ADD:
	    break;
	case PROF_CHANGE_EDIT:
	    /*每次全局配置改变后重新初始化一遍*/
	    macFilterInit();
#if(MAC_FILTER_JION_WIFI == FYES)
	    WireMacFilterInit();
#endif
	    break;
	case PROF_CHANGE_DEL:
	    break;
	case PROF_CHANGE_DELALL:
	    break;
	default:/*null*/
	    break;
    }
}


/*mac地址过滤账号profilechange触发函数*/
void macFilterProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const MacFilterProfile* oldProfileDataPtr, const MacFilterProfile* newProfileDataPtr )
{
    switch(changeType) {
	case PROF_CHANGE_ADD:
	    editOneMacFilterRule(newProfileDataPtr, Iptables_INS);/*添加*/
#if(MAC_FILTER_JION_WIFI == FYES)
	    WireMacFilterInit();
#endif
	    break;
	case PROF_CHANGE_ADD_ALL:
	    macFilterInit();/*批量添加时重新初始化所有规则*/
#if(MAC_FILTER_JION_WIFI == FYES)
	    WireMacFilterInit();
#endif
	    break;
	case PROF_CHANGE_EDIT:
	    editOneMacFilterRule(oldProfileDataPtr, Iptables_DEL);/*删除*/
	    editOneMacFilterRule(newProfileDataPtr, Iptables_INS);
#if(MAC_FILTER_JION_WIFI == FYES)
	    WireMacFilterInit();
#endif
	    break;
	case PROF_CHANGE_DEL:
	    editOneMacFilterRule(oldProfileDataPtr, Iptables_DEL);
#if(MAC_FILTER_JION_WIFI == FYES)
	    WireMacFilterInit();
#endif
	    break;
	case PROF_CHANGE_DELALL:
	    macFilterDelAll();
#if(MAC_FILTER_JION_WIFI == FYES)
	    WireMacFilterInit();
#endif
	    break;
	default:/*null*/
	    break;
    }
}


/*删除或添加一条arptables规则*/
static void editOneMacFilterRule(const MacFilterProfile *prof, char action)
{
    MibProfileType mibTypeG = MIB_PROF_MAC_FILTER_GLOBAL;
    MacFilterGlobalProfile *profG = NULL;
    char *tmpmac = NULL;

    profG = (MacFilterGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
    if ((ProfInstIsFree(profG) == PROFOK) && (profG->head.active == TRUE)) /*检查是否有效*/
    {
	editMacFilterJumpRule(action, getLanIfName());
	tmpmac = converMac6To12Byte(prof->mac);/*转换mac地址*/
	Conversion2StdMac(tmpmac);
	if (profG->action == MAC_ALLOW)/*允许时符合条件的返回，其余丢弃*/
	{
	    doSystem("arptables -%c %s --source-mac %s -j RETURN", action, MAC_FILTER_CHAIN, tmpmac);
	}
	else/*禁止时符合条件的丢弃，其余返回*/
	{
	    doSystem("arptables -%c %s --source-mac %s -j DROP", action, MAC_FILTER_CHAIN, tmpmac);
	}
	doSystem("ip neigh flush all");/*清除arp，让规则立即生效*/
    }
}


/*删除所有MAC过滤规则*/
static void macFilterDelAll(void)
{
    doSystem("arptables -F %s", MAC_FILTER_CHAIN);/*删除所有过滤规则*/
    editMacFilterJumpRule(Iptables_FLUSH, getLanIfName());/*删除INPUT跳转规则和过滤链*/
    doSystem("ip neigh flush all");/*清除arp，让他立即生效*/
}
#if(MAC_FILTER_JION_WIFI == FYES)
#if(WIRELESS == FYES)
//无线全局变量和mac地址的copy和初始化函数
static void  WireMacFilterInit(void)
{
    int index = 0, min = 0, max = 0;
    MibProfileType mibTypeG = MIB_PROF_MAC_FILTER_GLOBAL;
    MacFilterGlobalProfile *profG = NULL;
    MibProfileType mibType = MIB_PROF_MAC_FILTER;
    MacFilterProfile *prof = NULL;
    char *tmpmac;
    WirelessMacFilterProfile * wire_prof=NULL;
#if (WIRELESS_5G == FYES)
    Wireless5gMacFilterProfile *wire_5g_prof=NULL;
#endif
    WirelessMacFilterGlobalProfile *profGlobal;
#if (WIRELESS_5G == FYES)
    WirelessMacFilterGlobalProfile *profGlobal_5g;
#endif
    profG = (MacFilterGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, index);
    profGlobal=(WirelessMacFilterGlobalProfile *)ProfGetInstPointByIndex(MIB_PROF_WIRELESS_MAC_FILTER_GLOBAL, 0);//2.4G
#if (WIRELESS_5G == FYES)
    profGlobal_5g=(WirelessMacFilterGlobalProfile *)ProfGetInstPointByIndex(MIB_PROF_WIRELESS_MAC_FILTER_GLOBAL, 1);//5G
#endif
    //全局变量copy
    if (profG->head.active == TRUE)
    {
	profGlobal->filterEn= FUN_ENABLE;
#if (WIRELESS_5G == FYES)
	profGlobal_5g->filterEn= FUN_ENABLE;
#endif
    }
    else
    {
	profGlobal->filterEn = FUN_DISABLE;
#if (WIRELESS_5G == FYES)
	profGlobal_5g->filterEn = FUN_DISABLE;
#endif
    }

    if(profG->action==MAC_ALLOW)
    {
	profGlobal->action=AP_MAC_ALLOW;
#if (WIRELESS_5G == FYES)
	profGlobal_5g->action=AP_MAC_ALLOW;
#endif
    }
    else
    {
	profGlobal->action=AP_MAC_DENY;
#if (WIRELESS_5G == FYES)
	profGlobal_5g->action=AP_MAC_DENY;
#endif
    }
    ProfInstLowHigh(mibType, &max, &min);
    //mac地址copy
    for (index = min; index < max; index++)
    {
	prof = (MacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	tmpmac = converMac6To12Byte(prof->mac);
	if(ProfInstIsFree(prof) == PROFOK)
	{
	    wire_prof=(WirelessMacFilterProfile *)ProfGetInstPointByIndex(MIB_PROF_WIRELESS_MAC_FILTER, index);
	    wire_prof->mac=prof->mac;
	    strcpy(wire_prof->head.name,tmpmac);
#if (WIRELESS_5G == FYES)
	    wire_5g_prof=(Wireless5gMacFilterProfile *)ProfGetInstPointByIndex(MIB_PROF_WIRELESS_5G_MAC_FILTER, index);
	    wire_5g_prof->mac=prof->mac;
	    strcpy(wire_5g_prof->head.name,tmpmac);
#endif
	}
	else
	{
	    ProfDelInstByIndex(MIB_PROF_WIRELESS_MAC_FILTER, index);
#if (WIRELESS_5G == FYES)
	    ProfDelInstByIndex(MIB_PROF_WIRELESS_5G_MAC_FILTER, index);
#endif
	}
    }
	    wirelessMacFilterGlobal(0);
#if (WIRELESS_5G == FYES)
	    wirelessMacFilterGlobal(1);
#endif
}
#endif
#endif
/* 初始化函数，开机或者每次修改全局配置都调用一次*/
static void macFilterInit(void)
{
    MibProfileType mibTypeG = MIB_PROF_MAC_FILTER_GLOBAL;
    MacFilterGlobalProfile *profG = NULL;
    MibProfileType mibType = MIB_PROF_MAC_FILTER;
    MacFilterProfile *prof = NULL;
   
    int index = 0, min = 0, max = 0;
   
    macFilterDelAll();/*删除所有MAC过滤规则*/
    
    profG = (MacFilterGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, index);
    if (ProfInstIsFree(profG) == PROFOK) /*检查是否有效*/
    {
	if (profG->head.active == TRUE)/*功能不开启则不建立规则*/
	{
	    /*针对只允许添加的mac访问但没添加mac地址到profile的情况特殊处理，添加一条丢弃所有规则*/
	    editMacFilterJumpRule(Iptables_SPECIAL, getLanIfName());
	    ProfInstLowHigh(mibType, &max, &min);
	    for (index = min; index < max; index++)
	    {
		prof = (MacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
		if(ProfInstIsFree(prof) == PROFOK)
		{
		    editOneMacFilterRule(prof, Iptables_INS);/*根据每个prof添加规则*/
		}
	    }
	}
    }
    return;
}


/*
 * 初始化
 */
void funInitMacFilter(void)
{
    registerForProfileUpdates(MIB_PROF_MAC_FILTER_GLOBAL, NULL, (ProfileUpdateFnc)macFilterGlobalProfileChange);/*MAC地址过滤全局设置*/
    registerForProfileUpdates(MIB_PROF_MAC_FILTER, NULL, (ProfileUpdateFnc)macFilterProfileChange);/*MAC地址过滤条目配置*/
    macFilterInit();/*开机初始化*/
#if(MAC_FILTER_JION_WIFI == FYES)
    WireMacFilterInit();
#endif
}
#endif
