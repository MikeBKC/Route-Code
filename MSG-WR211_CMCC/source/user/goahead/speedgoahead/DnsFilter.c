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

#if (DNS_FILTER == FYES)
#ifdef DNS_FILTER_DEBUG
#define DNS_FILTER_PRINTF(fmt, args...)          printf(fmt, ## args)
#else
#define DNS_FILTER_PRINTF(fmt, args...)
#endif

#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
extern int profChangeNfNlSock;
extern void sendMsgToKernel(int socket, MibProfileType proftype, int instIndex, Pointer oldDataPtr, int profsize, ProfChangeType changeType);
static void domainAddInit(void);
#endif
#if (NOTICE == FYES)
static void DnsNoticeDisChain(char action, char *chain, char *name, char *ip);
static void DnsNoticeEnChain(char action, char *chain, char *ip);
static void DnsDisNotice(int action);
static void DnsEnNotice(int action);
#endif
static void DnsConfigChain(char action, char *chain, char *name, char *target);

static void dnsFilterDrop(char *chain);
static void dnsFilterType(DnsFilterProfile *prof, SystemProfile *profSys, char *chain);
static void dnsFilterTimeIp(SystemProfile *profSys, char action, char *fir_chain, char *pre_chain, char *chain);
#if (TIME_GRP == FYES)
extern void dnsFilterTime(TimeGrpProfile *timeGrpProf, char action, char *fir_chain, char *pre_chain, char *chain);
#else
extern void dnsFilterTime(char action, char *fir_chain, char *pre_chain, char *chain);
#endif
static void dnsFilterIp(SystemProfile *profSys, char action, char *fir_chain, char *pre_chain);
static void dnsFilterProfInit()
{
    int min,max,index;
    char tmp[10];
    MibProfileType mibType = MIB_PROF_DNS_FILTER;
    DnsFilterProfile *prof = NULL; 
    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
	prof = (DnsFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	sprintf(tmp, "%d", index);
	if((strcmp(prof->domainName,"")!=0) && (strcmp(prof->head.name,tmp)!=0))//当结构体中存在域名时候检查序列号是否和index一致
	{
	    strcpy(prof->head.name,tmp);
	}
    }
}
/***********************************************************************
 * 函 数 名：   dnsFilterInit
 * 创建日期：	2011-11-02
 * 修改日期：
 * 作者：       
 * 附加说明：	初始化函数
***********************************************************************/
extern void dnsFilterInit(SystemProfile *oldProf)
{
    int min, max, index;
    MibProfileType mibType = MIB_PROF_DNS_FILTER;
    DnsFilterProfile *prof;
    SystemProfile *profSys = NULL;
#if (NOTICE == FYES)
    NoticeProfile *profNotice = NULL;
    char if_addr[16];

    memset(if_addr, 0, sizeof(if_addr));
    getIfIp(getLanIfName(), if_addr);
#endif

    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0); 
    
    doSystem("iptables -F " DNS_PRE_INPUT_CHAIN "");
    doSystem("iptables -F " DNS_INPUT_CHAIN "");
    doSystem("iptables -F " DNS_PRE_FORWARD_CHAIN "");
    doSystem("iptables -F " DNS_FORWARD_CHAIN "");

    if(oldProf != NULL)
    {
	DNS_FILTER_PRINTF("%s------%d\n", __func__, __LINE__);
	dnsFilterTimeIp(oldProf, Iptables_DEL, PRE_DNS_INPUT_CHAIN, DNS_PRE_INPUT_CHAIN, DNS_INPUT_CHAIN);
	dnsFilterTimeIp(oldProf, Iptables_DEL, PRE_DNS_FORWARD_CHAIN, DNS_PRE_FORWARD_CHAIN, DNS_FORWARD_CHAIN);
    }
    else
    {
	DNS_FILTER_PRINTF("%s------%d\n", __func__, __LINE__);
	dnsFilterTimeIp(profSys, Iptables_DEL, PRE_DNS_INPUT_CHAIN, DNS_PRE_INPUT_CHAIN, DNS_INPUT_CHAIN);
	dnsFilterTimeIp(profSys, Iptables_DEL, PRE_DNS_FORWARD_CHAIN, DNS_PRE_FORWARD_CHAIN, DNS_FORWARD_CHAIN);
    }
    dnsFilterTimeIp(profSys, Iptables_ADD, PRE_DNS_INPUT_CHAIN, DNS_PRE_INPUT_CHAIN, DNS_INPUT_CHAIN);
    dnsFilterTimeIp(profSys, Iptables_ADD, PRE_DNS_FORWARD_CHAIN, DNS_PRE_FORWARD_CHAIN, DNS_FORWARD_CHAIN);

    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
	prof = (DnsFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	if (!strcmp(prof->domainName, ""))
	{
	    continue;
	}
	if (profSys->dnsFilterEn == FUN_ENABLE)
	{
	    if (prof != NULL)
	    {
		dnsFilterType(prof, profSys, DNS_INPUT_CHAIN);
		dnsFilterType(prof, profSys, DNS_FORWARD_CHAIN);
	    }
	}
	else
	{
	    doSystem("iptables -t filter -F " DNS_INPUT_CHAIN "");
	    doSystem("iptables -t filter -F " DNS_FORWARD_CHAIN "");
	}
    }
    if((profSys->dnsFilterControl.filterType == DNSFILTER_DOMAIN_EN) && (profSys->dnsFilterEn == FUN_ENABLE))
    {/*只允许profile中的域名。其他的禁止*/
#if (NOTICE == FYES)
	profNotice = (NoticeProfile*)ProfGetInstPointByIndex(MIB_PROF_NOTICE, 2);
	if((profNotice != NULL) && (ProfInstIsFree(profNotice) == 0)) {
	    if (profNotice->head.active == 1) {
		DnsNoticeEnChain(Iptables_ADD, DNS_INPUT_CHAIN, if_addr);
		DnsNoticeEnChain(Iptables_ADD, DNS_FORWARD_CHAIN, if_addr);
	    } else {
#endif
		dnsFilterDrop(DNS_INPUT_CHAIN);
		dnsFilterDrop(DNS_FORWARD_CHAIN);
#if (NOTICE == FYES)
	    }
	} else {
	    dnsFilterDrop(DNS_INPUT_CHAIN);
	    dnsFilterDrop(DNS_FORWARD_CHAIN);
	}
#endif
    } 
    return;
}
static void dnsFilterDrop(char *chain)
{
    doSystem("iptables -A %s -j DROP", chain); /*禁止其他dns包*/
    DNS_FILTER_PRINTF("iptables -A %s -j DROP\n", chain); /*禁止其他dns包*/
    return;
}
/********************************************************************
* 函数： dnsFilterTimeIp
* 功能： 添加时间组及ip组关联
* 创建： 2012-05-10
* 参数： 略
* 返回： 略
* 输出： 略
* 作者： Jianqing.Sun 
********************************************************************/
static void dnsFilterTimeIp(SystemProfile *profSys, char action, char *fir_chain, char *pre_chain, char *chain)
{
#if (TIME_GRP == FYES)
    TimeGrpProfile *timeGrpProf = NULL;

    if(strlen(profSys->dnsFilterControl.timeGrpName) != 0U)
    {
	timeGrpProf = (TimeGrpProfile *)ProfGetInstPointByName(MIB_PROF_TIMEGRP, profSys->dnsFilterControl.timeGrpName);
    }
    else
    {
	timeGrpProf = NULL;
    }
#endif

    dnsFilterIp(profSys, action, fir_chain, pre_chain);
#if (TIME_GRP == FYES)
    dnsFilterTime(timeGrpProf, action, fir_chain, pre_chain, chain);
#else
    dnsFilterTime(action, fir_chain, pre_chain, chain);
#endif
    return;
}
/********************************************************************
* 函数： dnsFilterTime
* 功能： 添加时间组关联
* 创建： 2012-05-10
* 参数： 略
* 返回： 略
* 输出： 略
* 作者： Jianqing.Sun 
********************************************************************/
#if (TIME_GRP == FYES)
extern void dnsFilterTime(TimeGrpProfile *timeGrpProf, char action, char *fir_chain, char *pre_chain, char *chain)
#else
extern void dnsFilterTime(char action, char *fir_chain, char *pre_chain, char *chain)
#endif
{
#if (TIME_GRP == FYES)

    int i = 0;
    char *daystr = NULL;
    char *timestr = NULL;
    char *datestr = NULL;

    if((timeGrpProf != NULL)&&(strcmp(timeGrpProf->datestart,"Forever")!=0))
    {
	datestr = getDateForIptables(timeGrpProf->datestart, timeGrpProf->datestop);/*iptables 日期字段*/
    }
#endif

#if (TIME_GRP == FYES)

    if((timeGrpProf != NULL)&&(strcmp(timeGrpProf->datestart,"Forever")!=0))
    {
	/*开始到结束日期的dns包才匹配*/
	doSystem("iptables -%c %s -p udp --dport 53 -m time %s -j %s", action, fir_chain, datestr, pre_chain);
	DNS_FILTER_PRINTF("iptables -%c %s -p udp --dport 53 -m time %s -j %s\n", action, fir_chain, datestr, pre_chain);
    }
    else
    {/*时间组名为空代表所有时段*/
	/*dns包才匹配*/
	doSystem("iptables -%c %s -p udp --dport 53 -j %s", action, fir_chain, pre_chain);
	DNS_FILTER_PRINTF("iptables -%c %s -p udp --dport 53 -j %s\n", action, fir_chain, pre_chain);
    }
#else
    /*dns包才匹配*/
    doSystem("iptables -%c %s -p udp --dport 53 -j %s", action, fir_chain, pre_chain);
    DNS_FILTER_PRINTF("iptables -%c %s -p udp --dport 53 -j %s\n", action, fir_chain, pre_chain);
#endif

#if (TIME_GRP == FYES)
    if((timeGrpProf != NULL)&&(strcmp(timeGrpProf->datestart,"Forever")!=0))
    {
	for(i=0;i < TIMEGRP_ENTRYS; i++)
	{ /*有一个时间段符合的则 跳转到过滤链表*/
	    daystr = getDayForIptables(timeGrpProf->timeEntry[i].day);
	    timestr = getTimeForIptables(timeGrpProf->timeEntry[i].timestart,timeGrpProf->timeEntry[i].timestop);
	    if(timeGrpProf->timeEntry[i].enable == FUN_ENABLE)
	    {
		doSystem("iptables -%c %s %s %s -j %s", action, pre_chain, timestr, daystr, chain);
		DNS_FILTER_PRINTF("iptables -%c %s %s %s -j %s\n", action, pre_chain, timestr, daystr, chain);
	    }
	}
    }
    else
    {/*时间组名为空代表所有时段*/
	doSystem("iptables -%c %s -j %s", action, pre_chain, chain);
	DNS_FILTER_PRINTF("iptables -%c %s -j %s\n", action, pre_chain, chain);
    }
#else
    doSystem("iptables -%c %s -j %s", action, pre_chain, chain);
    DNS_FILTER_PRINTF("iptables -%c %s -j %s\n", action, pre_chain, chain);
#endif
    return;
}

/********************************************************************
* 函数： dnsFilterIp
* 功能： ip组关联
* 创建： 2012-05-21
* 参数： 略
* 返回： 略
* 输出： 略
* 作者： Jianqing.Sun 
********************************************************************/
static void dnsFilterIp(SystemProfile *profSys, char action, char *fir_chain, char *pre_chain)
{
    char *ipstr = NULL;
    ipstr = getIpRangeForIptables_opposite(profSys->dnsFilterControl.ipstart, profSys->dnsFilterControl.ipend, 1);
    if(profSys->dnsFilterControl.addrTypeEn == ADDR_IPRANGE)
    {/*非控制网段的不匹配*/
	doSystem("iptables -%c %s %s -j RETURN", action, pre_chain, ipstr); 
	DNS_FILTER_PRINTF("iptables -%c %s %s -j RETURN\n", action, pre_chain, ipstr); 
    }
#if (IP_GRP == FYES)
    else
    {/*非控制组的不匹配*/
	if(profSys->dnsFilterControl.addrTypeEn == ADDR_IPGRP)
	{
	    if(strlen(profSys->dnsFilterControl.ipGrpName) != 0U)
	    {
		doSystem("iptables -%c %s -m set ! --set %s src -j RETURN", action, pre_chain, profSys->dnsFilterControl.ipGrpName);
		DNS_FILTER_PRINTF("iptables -%c %s -m set ! --set %s src -j RETURN\n", action, pre_chain, profSys->dnsFilterControl.ipGrpName);
	    }
	}
    }
#endif
    return;
}
/********************************************************************
* 函数： dnsFilterType
* 功能： 过滤类型添加
* 创建： 2012-05-10
* 参数： 略
* 返回： 略
* 输出： 略
* 作者： Jianqing.Sun 
********************************************************************/
static void dnsFilterType(DnsFilterProfile *prof, SystemProfile *profSys, char *chain)
{
    char target[60];
    memset(target, 0, sizeof(target));
#if (NOTICE == FYES)
    NoticeProfile *profNotice = NULL;
    char if_addr[16];
#endif

    DNS_FILTER_PRINTF("%s------%d,profSys->dnsFilterControl.filterType = %d\n", __func__, __LINE__, profSys->dnsFilterControl.filterType);
    if(profSys->dnsFilterControl.filterType == DNSFILTER_DOMAIN_DIS)
    {/*只禁止profile中的域名。其他的允许*/
#if (NOTICE == FYES)
	profNotice = (NoticeProfile*)ProfGetInstPointByIndex(MIB_PROF_NOTICE, 2);
	if((profNotice != NULL) && (ProfInstIsFree(profNotice) == 0)) {
	    if (profNotice->head.active == 1) {
		memset(if_addr, 0, sizeof(if_addr));
		getIfIp(getLanIfName(), if_addr);
		sprintf(target, "REJECT --reject-with dns-redirect:%s", if_addr);
	    } else {
#endif
		strcpy(target,"DROP");
#if (NOTICE == FYES)
	    }
	} else {
	    strcpy(target,"DROP");
	}
#endif
    } 
    else 
    {
	if(profSys->dnsFilterControl.filterType == DNSFILTER_DOMAIN_EN)
	{/*只允许profile中的域名。其他的禁止*/
	    strcpy(target,"RETURN");
	} 
    }
    doSystem("iptables -A %s -m domain --name \"%s\" -j %s", chain, prof->domainName, target);
    DNS_FILTER_PRINTF("iptables -A %s -m domain --name \"%s\" -j %s\n", chain, prof->domainName, target);
    return;
}
#if (NOTICE == FYES)
static void DnsNoticeDisChain(char action, char *chain, char *name, char *ip)
{
    doSystem("iptables -%c %s -m domain --name \"%s\" -j REJECT --reject-with dns-redirect:%s", action, chain, name, ip);
    return;
}
static void DnsNoticeEnChain(char action, char *chain, char *ip)
{
    doSystem("iptables -%c %s -j REJECT --reject-with dns-redirect:%s", action, chain, ip);
    return;
}
static void DnsDisNotice(int action)
{
    MibProfileType mibType = MIB_PROF_DNS_FILTER;
    DnsFilterProfile *prof = NULL;
    int min, max, index;
    InterfaceProfile *profIF = NULL;
    char if_addr[16];

    memset(if_addr, 0, sizeof(if_addr));
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);
    if ((profIF != NULL) && (profIF->head.active == 1)) {/*判断lan口profile是否正常*/
	/*此处不能用getIfIp来取Lan口，因为getIfIp取出来的ip可能还是改Lan口之前的ip*/
	int2ip(htonl(profIF->ether.sta.ip), if_addr);
    }

    ProfInstLowHigh(mibType, &max, &min);

    doSystem("iptables -F " DNS_INPUT_CHAIN "");
    doSystem("iptables -F " DNS_FORWARD_CHAIN "");
    for (index = min; index < max; index++) {
	prof = (DnsFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	if ((prof != NULL) && (ProfInstIsFree(prof) == 0)){
	    if (strcmp(prof->domainName, "") != 0) {
		if (action == 0) {
		    DnsConfigChain(Iptables_ADD, DNS_INPUT_CHAIN, prof->domainName, DROP_CHAIN);
		    DnsConfigChain(Iptables_ADD, DNS_FORWARD_CHAIN, prof->domainName, DROP_CHAIN);
		} else {
		    DnsNoticeDisChain(Iptables_ADD, DNS_INPUT_CHAIN, prof->domainName, if_addr);
		    DnsNoticeDisChain(Iptables_ADD, DNS_FORWARD_CHAIN, prof->domainName, if_addr);
		}
	    }
	}
    }
    return;
}
static void DnsEnNotice(int action)
{
    MibProfileType mibType = MIB_PROF_DNS_FILTER;
    DnsFilterProfile *prof = NULL;
    int min, max, index;
    char if_addr[16];

    memset(if_addr, 0, sizeof(if_addr));
    getIfIp(getLanIfName(), if_addr);

    if (action == 0) {
	DnsNoticeEnChain(Iptables_DEL, DNS_INPUT_CHAIN, if_addr);
	DnsNoticeEnChain(Iptables_DEL, DNS_FORWARD_CHAIN, if_addr);
	dnsFilterDrop(DNS_INPUT_CHAIN);
	dnsFilterDrop(DNS_FORWARD_CHAIN);
    } else {
	doSystem("iptables -F " DNS_INPUT_CHAIN "");
	doSystem("iptables -F " DNS_FORWARD_CHAIN "");

	ProfInstLowHigh(mibType, &max, &min);

	for (index = min; index < max; index++) {
	    prof = (DnsFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	    if ((prof != NULL) && (ProfInstIsFree(prof) == 0)){
		if (strcmp(prof->domainName, "") != 0) {
		    DnsConfigChain(Iptables_ADD, DNS_INPUT_CHAIN, prof->domainName, RETURN_CHAIN);
		    DnsConfigChain(Iptables_ADD, DNS_FORWARD_CHAIN, prof->domainName, RETURN_CHAIN);
		}
	    }
	}
	DnsNoticeEnChain(Iptables_ADD, DNS_INPUT_CHAIN, if_addr);
	DnsNoticeEnChain(Iptables_ADD, DNS_FORWARD_CHAIN, if_addr);
    }
    return;
}
extern void DnsNoticeChange(int action)
{
    SystemProfile *profSys = NULL;

    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0); 
    if (profSys->dnsFilterEn == FUN_ENABLE)  {
	if (profSys->dnsFilterControl.filterType == DNSFILTER_DOMAIN_DIS) {
	    /*只禁止profile中的域名。其他的允许*/
	    DnsDisNotice(action);
	} 
	else if (profSys->dnsFilterControl.filterType == DNSFILTER_DOMAIN_EN) {
	    /*只允许profile中的域名。其他的禁止*/
	    DnsEnNotice(action);
	} else {
	    /*none*/
	}
    }
    return;
}
#endif
static void DnsConfigChain(char action, char *chain, char *name, char *target)
{
    doSystem("iptables -%c %s -m domain --name \"%s\" -j %s", action, chain, name, target);
    return;
}
static void dnsFilterChangeChain(DnsFilterProfile *prof, int change)
{
    SystemProfile *profSys = NULL;
#if (NOTICE == FYES)
    NoticeProfile *profNotice = NULL;
    char if_addr[16];

#endif
    char action;

    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0); 
    if ((profSys != NULL) && (ProfInstIsFree(profSys) == 0)) {
	if(profSys->dnsFilterControl.filterType == DNSFILTER_DOMAIN_DIS) {
	    /*只禁止profile中的域名。其他的允许*/
	    if (change == 1) {
		action = Iptables_ADD;
	    } else {
		action = Iptables_DEL;
	    }
#if (NOTICE == FYES)
	    profNotice = (NoticeProfile*)ProfGetInstPointByIndex(MIB_PROF_NOTICE, 2);
	    if((profNotice != NULL) && (ProfInstIsFree(profNotice) == 0)) {
		if (profNotice->head.active == 1) {
		    memset(if_addr, 0, sizeof(if_addr));
		    getIfIp(getLanIfName(), if_addr);
		    DnsNoticeDisChain(action, DNS_INPUT_CHAIN, prof->domainName, if_addr);
		    DnsNoticeDisChain(action, DNS_FORWARD_CHAIN, prof->domainName, if_addr);
		} else {
#endif
		    DnsConfigChain(action, DNS_INPUT_CHAIN, prof->domainName, DROP_CHAIN);
		    DnsConfigChain(action, DNS_FORWARD_CHAIN, prof->domainName, DROP_CHAIN);
#if (NOTICE == FYES)
		}
	    }
#endif
	} else if(profSys->dnsFilterControl.filterType == DNSFILTER_DOMAIN_EN) {
	    if (change == 1) {
		action = Iptables_INS;
	    } else {
		action = Iptables_DEL;
	    }
	    DnsConfigChain(action, DNS_INPUT_CHAIN, prof->domainName, RETURN_CHAIN);
	    DnsConfigChain(action, DNS_FORWARD_CHAIN, prof->domainName, RETURN_CHAIN);
	} else {
	    /*none*/
	}
    }
    return;
}
/***********************************************************************
 * 函 数 名：   dnsFilterAdd 
 * 创建日期：	2011-11-02
 * 修改日期：
 * 作者：       
 * 附加说明：	添加函数
***********************************************************************/
void dnsFilterAdd(Uint32 index)
{
    DnsFilterProfile *prof;
    MibProfileType mibType = MIB_PROF_DNS_FILTER;

    prof = (DnsFilterProfile *)ProfGetInstPointByIndex(mibType, index);
    if ((prof != NULL) && (ProfInstIsFree(prof) == 0)) { 
	if(strcmp(prof->domainName, "") != 0) {
	    dnsFilterChangeChain(prof, 1);
	}
    }
    return;
}

/***********************************************************************
 * 函 数 名：   dnsFilterDel 
 * 创建日期：	2011-11-02
 * 修改日期：
 * 作者：       
 * 附加说明：	删除函数
***********************************************************************/
void dnsFilterDel(DnsFilterProfile *prof)
{
    if ((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
	if(strcmp(prof->domainName, "") != 0) {
	    dnsFilterChangeChain(prof, 0);
	}
    }
    return;
}

/***********************************************************************
 * 函 数 名：   dnsFilterDelAll
 * 创建日期：	2011-11-02
 * 修改日期：
 * 作者：       
 * 附加说明：	删除所有函数
***********************************************************************/
void dnsFilterDelAll(void)
{
    doSystem("iptables -t filter -F " DNS_INPUT_CHAIN "");
    doSystem("iptables -t filter -F " DNS_FORWARD_CHAIN "");
}

/***********************************************************************
 * 函 数 名：   dnsFilterProfileChange
 * 创建日期：	2011-11-02
 * 修改日期：
 * 作者：       
 * 附加说明：	生效函数
***********************************************************************/
void dnsFilterProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const DnsFilterProfile *oldProfileDataPtr, const DnsFilterProfile *newProfileDataPtr )
{
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    sendMsgToKernel(profChangeNfNlSock, profileType,index, (Pointer)oldProfileDataPtr,sizeof(*oldProfileDataPtr),changeType);
#else
    switch(changeType) {
        case PROF_CHANGE_ADD:
	    dnsFilterAdd(index);
	    break;
        case PROF_CHANGE_EDIT:
	    DNS_FILTER_PRINTF("%s------%d\n", __func__, __LINE__);
	    dnsFilterInit(NULL);
	    break;
        case PROF_CHANGE_DEL:
	    dnsFilterDel((DnsFilterProfile *)oldProfileDataPtr);
	    break;
        case PROF_CHANGE_DELALL:
	    dnsFilterDelAll();
	    break;
        default:
	    break;
    };
#endif
}

#if (FIREWALL == FYES)
extern void fw_rule_init(void);
#endif
extern void dnsFilter_rule_init()
{
    SystemProfile *profSys = NULL;

    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    if(profSys->dnsFilterEn == FUN_ENABLE )
    {
	/* 对于只存在一条的链，可以先删除确保不会存在两条链 */
	doSystem("iptables -D INPUT -j %s", PRE_DNS_INPUT_CHAIN);
	doSystem("iptables -D FORWARD -j %s", PRE_DNS_FORWARD_CHAIN);
	doSystem("iptables -I INPUT -j %s", PRE_DNS_INPUT_CHAIN);
	doSystem("iptables -I FORWARD -j %s", PRE_DNS_FORWARD_CHAIN);
    }     
    return ;
}

static void dnsFilterGlobalChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr )
{
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    /**
     * 域名过滤由关闭到开启
     */
    if ((oldProfileDataPtr->dnsFilterEn == FALSE) && (newProfileDataPtr->dnsFilterEn == TRUE)) {
	domainAddInit();/*添加域名内核hash表*/
    } else {
	if ((oldProfileDataPtr->dnsFilterEn == TRUE) && (newProfileDataPtr->dnsFilterEn == FALSE)) {/*由开启到关闭*/
	    /*删除域名内核hash表项*/
	    sendMsgToKernel(profChangeNfNlSock, MIB_PROF_DNS_FILTER,0, (Pointer)NULL,0,PROF_CHANGE_DELALL);
	}
    }
    sendMsgToKernel(profChangeNfNlSock, profileType,index, (Pointer)oldProfileDataPtr,sizeof(*oldProfileDataPtr),changeType);
#else
    SystemProfile *newProf = (SystemProfile *)newProfileDataPtr;
    SystemProfile *oldProf = (SystemProfile *)oldProfileDataPtr;
#if (NOTICE == FYES)
    NoticeProfile *profNotice = NULL;
#endif

    switch(changeType) {
        case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
	case PROF_CHANGE_DEL:
        case PROF_CHANGE_DELALL:
	    if ((newProf->dnsFilterEn != oldProf->dnsFilterEn) || 
		    (memcmp(&newProf->dnsFilterControl, &oldProf->dnsFilterControl, sizeof(DnsFilterControl))!=0))
	    {
		dnsFilterInit(oldProf);
#if (NOTICE == FYES)
		profNotice = (NoticeProfile*)ProfGetInstPointByIndex(MIB_PROF_NOTICE, 2);
		if((profNotice != NULL) && (ProfInstIsFree(profNotice) == 0)) {
		    if (profNotice->head.active == 1) {
			DnsNoticeChange(1);
		    }
		}
#endif
	    }

	    if ((oldProf->dnsFilterEn == FUN_ENABLE) 
		    && (newProf->dnsFilterEn == FUN_DISABLE)) { /* 开->关 */

		doSystem("iptables -D INPUT -j %s", PRE_DNS_INPUT_CHAIN);
		doSystem("iptables -D FORWARD -j %s", PRE_DNS_FORWARD_CHAIN);
	    } else if ((oldProf->dnsFilterEn == FUN_DISABLE) 
		    && (newProf->dnsFilterEn == FUN_ENABLE)) { /* 关->开 */
		/* 对于只存在一条的链，可以先删除确保不会存在两条链 */
		doSystem("iptables -D INPUT -j %s", PRE_DNS_INPUT_CHAIN);
		doSystem("iptables -D FORWARD -j %s", PRE_DNS_FORWARD_CHAIN);
		doSystem("iptables -I INPUT -j %s", PRE_DNS_INPUT_CHAIN);
		doSystem("iptables -I FORWARD -j %s", PRE_DNS_FORWARD_CHAIN);
#if 0
#if (FIREWALL == FYES)
		/* 确保域名过滤优先级高于防火墙优先级 */
		fw_rule_init();
#endif
#endif
	    }
	    break;
	default:
	    break;
    }
#endif
}

#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
static void domainAddInit(void)
{
    int min = 0, max = 0,i;
    DnsFilterProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_DNS_FILTER;
    
    ProfInstLowHigh(mibType, &max, &min);

    for (i = min; i < max; i++) {
	prof = (DnsFilterProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE)) {
	    DNS_FILTER_PRINTF("#######func:%s,line:%d,prof->head.name=%s###########\n",__func__,__LINE__,prof->head.name);
	    sendMsgToKernel(profChangeNfNlSock, mibType,i, (Pointer)NULL,0,PROF_CHANGE_ADD);
	}
    }
}
#endif
/***********************************************************************
 * 函 数 名：   funInitDnsFilter
 * 创建日期：	2011-11-02
 * 修改日期：
 * 作者：       
 * 附加说明：	注册函数
***********************************************************************/
void funInitDnsFilter(void)
{
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    int min = 0, max = 0,i;
    SystemProfile *profSys = NULL;
    MibProfileType mibType;;
    
    mibType = MIB_PROF_SYSTEM;
    ProfInstLowHigh(mibType, &max, &min);
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);
    if((profSys != NULL) && (ProfInstIsFree(profSys) == 0) && (profSys->head.active == TRUE)) {
	DNS_FILTER_PRINTF("#######func:%s,line:%d,profSys->head.name=%s###########\n",__func__,__LINE__,profSys->head.name);
	if (profSys->dnsFilterEn == TRUE) {
	    sendMsgToKernel(profChangeNfNlSock, mibType,0, (Pointer)NULL,0,PROF_CHANGE_ADD);
	    domainAddInit();
	}
    }
    registerForProfileUpdates(MIB_PROF_DNS_FILTER, (Pointer)NULL, (ProfileUpdateFnc)dnsFilterProfileChange);
    registerForProfileUpdates(MIB_PROF_SYSTEM, NULL, (ProfileUpdateFnc)dnsFilterGlobalChange);
#else
    SystemProfile *profSys = NULL;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0); 

    registerForProfileUpdates(MIB_PROF_DNS_FILTER, (Pointer)NULL, (ProfileUpdateFnc)dnsFilterProfileChange);
    registerForProfileUpdates(MIB_PROF_SYSTEM, NULL, (ProfileUpdateFnc)dnsFilterGlobalChange);

    /*为了解决删除中间的域名重启后会导致再添加新域名后覆盖原来的域名问题*/
    dnsFilterProfInit();

    DNS_FILTER_PRINTF("%s------%d\n", __func__, __LINE__);
    doSystem("iptables -N " PRE_DNS_INPUT_CHAIN "");
    doSystem("iptables -N " PRE_DNS_FORWARD_CHAIN "");

    doSystem("iptables -N " DNS_PRE_INPUT_CHAIN "");
    doSystem("iptables -N " DNS_INPUT_CHAIN "");
    doSystem("iptables -N " DNS_PRE_FORWARD_CHAIN "");
    doSystem("iptables -N " DNS_FORWARD_CHAIN "");

    if (profSys->dnsFilterEn == FUN_ENABLE) {
	doSystem("iptables -D INPUT -j %s", PRE_DNS_INPUT_CHAIN);
	doSystem("iptables -D FORWARD -j %s", PRE_DNS_FORWARD_CHAIN);
	doSystem("iptables -A INPUT -j %s", PRE_DNS_INPUT_CHAIN);
	doSystem("iptables -A FORWARD -j %s", PRE_DNS_FORWARD_CHAIN);
    }
    dnsFilterInit(NULL);
#endif
}
#endif
