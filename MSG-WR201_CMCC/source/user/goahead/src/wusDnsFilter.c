#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "uttfunction.h"
#include "mib.h"
#include "profacce.h"
#include "translate.h"

#if(DNS_FILTER == FYES)

#ifdef DNS_FILTER_DBG
#define DNS_FILTER_PRINTF(fmt, args...)		printf(fmt, ## args)	
#else
#define DNS_FILTER_PRINTF(fmt, args...)
#endif
#define DNS_INPUT_CHAIN        "dns_input_chain"
#define DNS_FORWARD_CHAIN       "dns_forward_chain"

extern int getIpGrpList(int eid, webs_t wp, int argc, char_t **argv);
static int GetMaxDomainNameNum(int eid, webs_t wp, int argc, char_t **argv);
static int DnsFilter_Sw(int eid, webs_t wp, int argc, char_t **argv);
static void formConfigDnsFilterDel(webs_t wp, char_t *path, char_t *query);
static void formConfigDnsFilterDelAll(webs_t wp, char_t *path, char_t *query);
static void formConfigDnsFilterGlobal(webs_t wp, char_t *path, char_t *query);
static int getDnsFilterList(int eid, webs_t wp, int argc, char_t **argv);
static void websHostFilter(webs_t wp, char_t *path, char_t *query);

/***********************************************************************
 *   函 数 名：   GetMaxDomainNameNum
 *   功能描述：   从配置中读取实例最大数
 *   创建日期：   2010-3-17
 *   修改日期：
 *   作者：       柏德秀
 *   附加说明：   无
 * ***********************************************************************/
static int GetMaxDomainNameNum(int eid, webs_t wp, int argc, char_t **argv)
{
    websWrite(wp,"var MaxDomainName = \"%d\";", MAX_DNS_FILTER_PROFILES);
    return 0;
}

/***********************************************************************
 * 函 数 名：   DnsFilter_Sw
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static int DnsFilter_Sw(int eid, webs_t wp, int argc, char_t **argv)
{
    SystemProfile *profSys = NULL;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0); 

    if (profSys->dnsFilterEn == FUN_ENABLE)
    {
	websWrite(wp,"var DnsFilterEnables=\"%s\";", T("on"));
    }
    else
    {
	websWrite(wp,"var DnsFilterEnables=\"%s\";", T("off"));
    }
    if(profSys->dnsFilterControl.addrTypeEn == ADDR_IPRANGE)
    {
	websWrite(wp,"var FromIPs = \"%N\";",profSys->dnsFilterControl.ipstart);
	websWrite(wp,"var EndIPs = \"%N\";",profSys->dnsFilterControl.ipend);
	websWrite(wp,"var sourceIPs = \"%d\";",ADDR_IPRANGE);
    }
    else
    {
#if (IP_GRP == FYES)
	websWrite(wp,"var ipGrpNames = \"%s\";",profSys->dnsFilterControl.ipGrpName);
	websWrite(wp,"var sourceIPs = \"%d\";",ADDR_IPGRP);
#endif
    }
#if (TIME_GRP == FYES)
    websWrite(wp,"var timeRangeNames = \"%s\";",profSys->dnsFilterControl.timeGrpName);
#endif
    websWrite(wp,"var enableTypes = \"%d\";", profSys->dnsFilterControl.filterType);
    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

/***********************************************************************
 * 函 数 名：   formConfigDnsFilterDel
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void formConfigDnsFilterDel(webs_t wp, char_t *path, char_t *query)
{
    int min, max, index;
    char *ptr, *PolicyName=NULL;
    MibProfileType mibType = MIB_PROF_DNS_FILTER;
    DnsFilterProfile *prof = NULL;
    ProfChangeType profAction = PROF_CHANGE_DEL;
    struProfAlloc *profhead = NULL;
#ifndef CONFIG_PERFORMANCE_OPTIMIZATION
    SystemProfile *profSys = NULL;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0); 
#endif

    PolicyName = websGetVar(wp, T("delstr"), T("")); 
    PolicyName = strtok_r(PolicyName,UTT_SPLIT_SIGN_STR,&ptr);
    if(!strcmp(PolicyName,"")) {
        websRedirect(wp, "DnsFilter.asp");
        return;
    }
    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
	prof = (DnsFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	if(!strcmp(prof->domainName, PolicyName))
	{
	    ProfBackupByIndex(mibType, profAction, index, &profhead);
	    ProfDelInstByIndex(mibType, index);
	    ProfUpdate(profhead);
	    ProfFreeAllocList(profhead);
	    nvramWriteCommit();
	    break;
	}
    }
#ifndef CONFIG_PERFORMANCE_OPTIMIZATION
    if (profSys->dnsFilterEn == FUN_DISABLE)
    {
	doSystem("iptables -t filter -F " DNS_INPUT_CHAIN "");
	doSystem("iptables -t filter -F " DNS_FORWARD_CHAIN "");
    }
#endif
    websRedirect(wp, "DnsFilter.asp");
}

/***********************************************************************
 * 函 数 名：   formConfigDnsFilterDelAll
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void formConfigDnsFilterDelAll(webs_t wp, char_t *path, char_t *query)
{
    int min, max, index;
    MibProfileType mibType = MIB_PROF_DNS_FILTER;
    DnsFilterProfile *prof = NULL;

#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    ProfChangeType profAction = PROF_CHANGE_DELALL;
    struProfAlloc *profhead = NULL;
    ProfBackupByIndex(mibType, profAction, 0, &profhead);/*发送delall消息*/
#else
    SystemProfile *profSys = NULL;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0); 
#endif

    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
	prof = (DnsFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	ProfDelInstByIndex(mibType, index);
    }
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
#endif
    
    nvramWriteCommit();
    nvramWriteCommit();
#ifndef CONFIG_PERFORMANCE_OPTIMIZATION
    doSystem("iptables -t filter -F " DNS_INPUT_CHAIN "");
    doSystem("iptables -t filter -F " DNS_FORWARD_CHAIN "");

    /* mantis 0014184 */
    if((profSys->dnsFilterControl.filterType == DNSFILTER_DOMAIN_EN) && (profSys->dnsFilterEn == FUN_ENABLE))
    {
	doSystem("iptables -A %s -j DROP ", DNS_INPUT_CHAIN);
	doSystem("iptables -A %s -j DROP ", DNS_FORWARD_CHAIN);
    }
#endif
    
    websRedirect(wp, "DnsFilter.asp");
}

/***********************************************************************
 * 函 数 名：   formConfigDnsFilterGlobal
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void formConfigDnsFilterGlobal(webs_t wp, char_t *path, char_t *query)
{
	const char* ErrMsg = NULL;
    SystemProfile *profSys = NULL;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0); 
    char *dnsFilterEnable;
    dnsFilterEnable = websGetVar(wp, T("DnsFilterEnable"), T("")); 
    MibProfileType mibType = MIB_PROF_SYSTEM;
    struProfAlloc *profhead = NULL;
    struct in_addr addr_SrcFromIp,addr_SrcEndIp;
    char_t *srcFromIp,*srcEndIp,*GroupName,*sourceIP,*timeRangeName,*enableType;
    ProfChangeType profAction = PROF_CHANGE_EDIT;

    srcFromIp = websGetVar(wp,T("FromIP"), T(""));
    srcEndIp = websGetVar(wp,T("EndIP"), T(""));
    GroupName = websGetVar(wp,T("GroupName"), T("")); 
    sourceIP = websGetVar(wp,T("sourceIP"), T(""));
    timeRangeName = websGetVar(wp,T("timeRangeName"), T(""));
    enableType = websGetVar(wp,T("enableType"), T(""));
    DNS_FILTER_PRINTF("%s-----%d,FromIP = %s, EndIP = %s, GroupName = %s, sourceIP = %s, timeRangeName = %s, enableType = %s\n", __func__, __LINE__, srcFromIp, srcEndIp, GroupName, sourceIP, timeRangeName, enableType);

    if((strcmp(sourceIP, "ipRange") == 0) &&((inet_aton(srcFromIp, &addr_SrcFromIp) == 0) || (inet_aton(srcEndIp, &addr_SrcEndIp) == 0)))
    {
		ErrMsg = getTransDataForC(C_LANG_INDEX_IP_PLAN_ADDR_ERR);
		setTheErrorMsg(ErrMsg);
		goto out;
    }
    ProfBackupByIndex(mibType, profAction, 0, &profhead);
    if (!strcmp(dnsFilterEnable, "on"))
    {
	profSys->dnsFilterEn = FUN_ENABLE;
    }
    else
    {
	profSys->dnsFilterEn = FUN_DISABLE;
    }
    if(strcmp(sourceIP, "ipRange") == 0)
    {
	profSys->dnsFilterControl.addrTypeEn = ADDR_IPRANGE; 
	profSys->dnsFilterControl.ipstart = addr_SrcFromIp.s_addr; 
	profSys->dnsFilterControl.ipend = addr_SrcEndIp.s_addr; 
    }
    else
    {
#if (IP_GRP == FYES)
	profSys->dnsFilterControl.addrTypeEn = ADDR_IPGRP; 
	strcpy(profSys->dnsFilterControl.ipGrpName, GroupName);
	DNS_FILTER_PRINTF("%s-----%d,profSys->dnsFilterControl.ipGrpName = %s, GroupName = %s\n", __func__, __LINE__, profSys->dnsFilterControl.ipGrpName, GroupName);
#endif
    }
#if (TIME_GRP == FYES)
    strcpy(profSys->dnsFilterControl.timeGrpName, timeRangeName);
#endif
    if(strcmp(enableType,"ACCEPT") == 0)
    {
	profSys->dnsFilterControl.filterType = DNSFILTER_DOMAIN_DIS;
    }
    else
    {
	profSys->dnsFilterControl.filterType = DNSFILTER_DOMAIN_EN;
    }
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    if (strcmp(dnsFilterEnable, "on"))
    {
	doSystem("iptables -t filter -F " DNS_INPUT_CHAIN "");
	doSystem("iptables -t filter -F " DNS_FORWARD_CHAIN "");
    }

out:
    websRedirect(wp, "DnsFilter.asp");
}

/***********************************************************************
 * 函 数 名：   getDnsFilterList
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static int getDnsFilterList(int eid, webs_t wp, int argc, char_t **argv)
{
    int min, max, index, num =0;
    MibProfileType mibType = MIB_PROF_DNS_FILTER;
    DnsFilterProfile *prof = NULL;
    
    websWrite(wp,"var DnsLists=new Array();");
    ProfInstLowHigh(mibType, &max, &min);
    num = min;
    for (index = min; index < max; index++)
    {
	prof = (DnsFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	if (strcmp(prof->domainName, ""))
	{
	    websWrite(wp, "DnsLists[%d] = \"%s\";", num, prof->domainName);
	    num++;
	}
	else
	{
	    continue;
	}
    }

    websWrite(wp,"var totalrecs=%d;",num);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

/***********************************************************************
 * 函 数 名：   websHostFilter
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void websHostFilter(webs_t wp, char_t *path, char_t *query)
{
    SystemProfile *profSys = NULL;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0); 
    int min, max, index;
    char tmp[100];
    char *rule = websGetVar(wp, T("addHostFilter"), T(""));
    MibProfileType mibType = MIB_PROF_DNS_FILTER;
    DnsFilterProfile *prof = NULL;
    ProfChangeType profAction = PROF_CHANGE_ADD;
    struProfAlloc *profhead = NULL;

    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
	/* Blank */
	prof = (DnsFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	if (0 == strcmp(prof->domainName, ""))
	{
	    break;
	}
    }
    sprintf(tmp, "%d", index);
    index = ProfNewInst(mibType, tmp, FALSE);
    /* Backup */
    ProfBackupByIndex(mibType, profAction, index, &profhead);
    /* Get pointer */
    prof = (DnsFilterProfile *)ProfGetInstPointByName(mibType, tmp);
    /* Change value */
    strcpy(prof->domainName, rule);
    /* Whether effect immediately */
    if (profSys->dnsFilterEn == FUN_ENABLE)
    {
        /* Effect immediately */
        ProfUpdate(profhead);
    }
    /* Release */
    ProfFreeAllocList(profhead);
    /* Write config */
    nvramWriteCommit();
    websRedirect(wp, "DnsFilter.asp");
   
    return;
}


/***********************************************************************
 * 函 数 名：   formDefineFirewall
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
void formDefineDnsFilter(void)
{
    websAspDefine(T("aspOutDnsFilterList"), getDnsFilterList);
    websAspDefine(T("DnsFilter_Sw"), DnsFilter_Sw);
    websAspDefine(T("GetMaxDomainNameNum"), GetMaxDomainNameNum);
    websFormDefine(T("formConfigDnsFilterDel"), formConfigDnsFilterDel);
    websFormDefine(T("formConfigDnsFilterDelAll"), formConfigDnsFilterDelAll);
    websFormDefine(T("formConfigDnsFilterGlobal"), formConfigDnsFilterGlobal);
    websFormDefine(T("websHostFilter"), websHostFilter);
#if (IP_GRP == FYES)
    websAspDefine(T("aspOutIpGrpList"),getIpGrpList);
#endif
}
#endif
