#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#ifdef CONFIG_DEFAULTS_KERNEL_2_6_21
  #include  <linux/types.h>
  #include  <linux/socket.h>
  #include  <linux/if.h>
#endif

#include "../../autoconf.h"
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "management.h"
#include "wireless.h"
#include "linux/autoconf.h"
#include "uttfunction.h"
#include "mib.h"
#include "profacce.h"
#include "translate.h"

#if(WIRELESS == FYES)

#define  DEBUG_MACFILTER 0
#if DEBUG_MACFILTER
#define MACFILTER_DEBUG(argx) printf argx
#else
#define MACFILTER_DEBUG(argx) 
#endif

extern char* converMac6To12Byte(MacAddr macAddr);
extern int converMac12To6Byte(char *macStr, MacAddr* macAddr);

static int getWlanMacFilterMacList(int eid, webs_t wp, int argc, char_t **argv);
static void formConfigWlanMacFilter(webs_t wp, char_t *path, char_t *query);
static void formConfigWlanMacFilterDel(webs_t wp, char_t *path, char_t *query);
static void formConfigWlanMacFilterDelAll(webs_t wp, char_t *path, char_t *query);
static void formWlanMacFilterGlobalConfig(webs_t wp, char_t *path, char_t *query);

char delColon(char str[20], char ch)
{
    int i, j;
    for (i = 0; str[i]!='\0'; i++)
    {
	if(str[i]==ch)
	{
	    for(j=i;str[j]!='\0';j++)
	    {
		str[j]=str[j+1];
	    }
	}
    }
}

/***********************************************************************
 * 函 数 名：   getWlanMacFilterMacList
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static int getWlanMacFilterMacList(int eid, webs_t wp, int argc, char_t **argv)
{
    int num;
    char *tmpmac;
    const char *profName = NULL;
    int min, max, index = 0;
    MibProfileType mibType = MIB_PROF_WIRELESS_MAC_FILTER;
    WirelessMacFilterProfile *prof = NULL;
    MibProfileType mibTypeGlobal = MIB_PROF_WIRELESS_MAC_FILTER_GLOBAL;
    WirelessMacFilterGlobalProfile *profGlobal = NULL;
   
    profGlobal = (WirelessMacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeGlobal, index);
    if(profGlobal != NULL)
    {
	if (profGlobal->filterEn == FUN_DISABLE)
	{
	    websWrite(wp,"var MacFilterEnables=\"%d\";", 0);
	}
	else
	{
	    websWrite(wp,"var MacFilterEnables=\"%d\";", 1);
	}
	if (AP_MAC_ALLOW == profGlobal->action)
	{
	    websWrite(wp,"var filterRules=\"%d\";", 1);
	}
	else
	{
	    websWrite(wp,"var filterRules=\"%d\";", 2);
	}
    }
    websWrite(wp,"var filterMacs=new Array();");
    ProfInstLowHigh(mibType, &max, &min);
    num = min;
    for (index = min; index < max; index++)
    {
	prof = (WirelessMacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	profName = ProfGetInstNameByIndex(mibType, index);
	if (0 != strcmp(profName, ""))
	{
	    tmpmac = converMac6To12Byte(prof->mac);
	    Conversion2StdMac(tmpmac);
	    websWrite(wp, "filterMacs[%d] = \"%s\";", num, tmpmac);
	    num++;
	}
	else
	{
	    continue;
	}
    }
    websWrite(wp,"var totalrecs=%d;", num);
    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

#if(WIRELESS_5G == FYES)
static int getWlanMacFilterMacList_5g(int eid, webs_t wp, int argc, char_t **argv)
{
    int num;
    char *tmpmac;
    const char *profName = NULL;
    int min, max, index = 1;
    MibProfileType mibType = MIB_PROF_WIRELESS_5G_MAC_FILTER;
    Wireless5gMacFilterProfile *prof = NULL;
    MibProfileType mibTypeGlobal = MIB_PROF_WIRELESS_MAC_FILTER_GLOBAL;
    WirelessMacFilterGlobalProfile *profGlobal = NULL;
   
    profGlobal = (WirelessMacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeGlobal, index);
    if(profGlobal != NULL)
    {
	if (profGlobal->filterEn == FUN_DISABLE)
	{
	    websWrite(wp,"var MacFilterEnables=\"%d\";", 0);
	}
	else
	{
	    websWrite(wp,"var MacFilterEnables=\"%d\";", 1);
	}
	if (AP_MAC_ALLOW == profGlobal->action)
	{
	    websWrite(wp,"var filterRules=\"%d\";", 1);
	}
	else
	{
	    websWrite(wp,"var filterRules=\"%d\";", 2);
	}
    }
    websWrite(wp,"var filterMacs=new Array();");
    ProfInstLowHigh(mibType, &max, &min);
    num = min;
    for (index = min; index < max; index++)
    {
	prof = (Wireless5gMacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	profName = ProfGetInstNameByIndex(mibType, index);
	if (0 != strcmp(profName, ""))
	{
	    tmpmac = converMac6To12Byte(prof->mac);
	    Conversion2StdMac(tmpmac);
	    websWrite(wp, "filterMacs[%d] = \"%s\";", num, tmpmac);
	    num++;
	}
	else
	{
	    continue;
	}
    }
    websWrite(wp,"var totalrecs=%d;", num);
    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}
#endif

/***********************************************************************
 * 函 数 名：   formConfigWlanMacFilter
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void formConfigWlanMacFilter(webs_t wp, char_t *path, char_t *query)
{
    MacAddr tmp;
    int min, max, index, i;
    char_t *action, *mac, *oldMac, *pages;
    const char *profName = NULL;
    int isHostInfoPage = 0;
    const char *ErrMsg = NULL;
    MibProfileType mibType = MIB_PROF_WIRELESS_MAC_FILTER;
    WirelessMacFilterProfile *prof = NULL;
    ProfChangeType profAction = PROF_CHANGE_ADD;
    struProfAlloc *profhead = NULL;
	
    pages = websGetVar(wp, T("pages"), T("")); 
    if(!strcmp(pages, "WlanHostInfo.asp"))
    {
        isHostInfoPage = 1;
    }
    action = websGetVar(wp, T("Action"), T("")); 
    mac = websGetVar(wp, T("filterMac"), T(""));
    if (isHostInfoPage == 1)
    {
	delColon(mac, ':');
    }
    converMac12To6Byte(mac, &tmp);
    if(((0 == strcmp(action, "add"))) || isHostInfoPage)
    {
        if(PROFFAIL == ProfGetInstIndexFree(mibType))
	{
			ErrMsg = getTransDataForC(C_LANG_INDEX_MAC_ADDR_LIST_MAX);
            setTheErrorMsg(ErrMsg);
            websRedirect(wp, "WlanMacFilter.asp");
            return;
        }
	ProfInstLowHigh(mibType, &max, &min);
	for (index = min; index < max; index++)
	{
	    profName = ProfGetInstNameByIndex(mibType, index);
	    if(0 == strcmp(profName, mac))
	    {
			ErrMsg = getTransDataForC(C_LANG_INDEX_MAC_ADDR_LIST_EXIT_ADDR);
			setTheErrorMsg(ErrMsg);
			websRedirect(wp, "WlanMacFilter.asp");
			return;
	    }
	}
	/* Add new Profile */
	index = ProfNewInst(mibType, mac, FALSE);
	ProfBackupByIndex(mibType, profAction, index, &profhead);
	prof = (WirelessMacFilterProfile *)ProfGetInstPointByName(mibType, mac);
	if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0))
	{
	    prof->mac = tmp;
	}
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
    }
    else
    {
        oldMac = websGetVar(wp, T("oldMac"), T(""));
	for (i = 0; i <=strlen(oldMac); i++)
	{
	    if ((oldMac[i] >= 97) && (oldMac[i] <= 122))
	    {
		oldMac[i] -= 32;
	    }
	}
	if (0 == strcmp(oldMac, mac))
	{
            websRedirect(wp, "WlanMacFilter.asp");
	    return;
	}
	/* whether tmpmac already exist */
	ProfInstLowHigh(mibType, &max, &min);
	for (index = min; index < max; index++)
	{
	    profName = ProfGetInstNameByIndex(mibType, index);
	    if(0 == strcmp(mac, profName))
	    {
			ErrMsg = getTransDataForC(C_LANG_INDEX_MAC_ADDR_LIST_EXIT_ADDR);
			setTheErrorMsg(ErrMsg);
			websRedirect(wp, "WlanMacFilter.asp");
			return;
	    }
	}
	/* Change Profile */
	prof = (WirelessMacFilterProfile *)ProfGetInstPointByName(mibType, oldMac);
	ProfBackupByName(mibType, profAction, oldMac, &profhead);
	if(prof != NULL)
	{
	    prof->mac = tmp;
	}
	ProfSetNameByPoint(prof, mac);
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
    }
    if(isHostInfoPage == 1)
    {
        websRedirect(wp, "WlanHostInfo.asp");
    }
    else
    {
        websRedirect(wp, "WlanMacFilter.asp");
    }
}

#if(WIRELESS_5G == FYES)
static void formConfigWlanMacFilter_5g(webs_t wp, char_t *path, char_t *query)
{
    MacAddr tmp;
    int min, max, index, i;
    char_t *action, *mac, *oldMac, *pages;
    const char *profName = NULL;
    int isHostInfoPage = 0;
    const char *ErrMsg = NULL;
    MibProfileType mibType = MIB_PROF_WIRELESS_5G_MAC_FILTER;
    Wireless5gMacFilterProfile *prof = NULL;
    ProfChangeType profAction = PROF_CHANGE_ADD;
    struProfAlloc *profhead = NULL;
	
    pages = websGetVar(wp, T("pages"), T("")); 
    if(!strcmp(pages, "WlanHostInfo_5g.asp"))
    {
        isHostInfoPage = 1;
    }
    action = websGetVar(wp, T("Action"), T("")); 
    mac = websGetVar(wp, T("filterMac"), T(""));
    if (isHostInfoPage == 1)
    {
	delColon(mac, ':');
    }
    converMac12To6Byte(mac, &tmp);
    if(((0 == strcmp(action, "add"))) || isHostInfoPage)
    {
        if(PROFFAIL == ProfGetInstIndexFree(mibType))
		{
			ErrMsg = getTransDataForC(C_LANG_INDEX_MAC_ADDR_LIST_MAX);
            setTheErrorMsg(ErrMsg);
            websRedirect(wp, "WlanMacFilter_5g.asp");
            return;
        }
	ProfInstLowHigh(mibType, &max, &min);
	for (index = min; index < max; index++)
	{
	    profName = ProfGetInstNameByIndex(mibType, index);
	    if(0 == strcmp(profName, mac))
	    {
			ErrMsg = getTransDataForC(C_LANG_INDEX_MAC_ADDR_LIST_EXIT_ADDR);
			setTheErrorMsg(ErrMsg);
			websRedirect(wp, "WlanMacFilter_5g.asp");
			return;
	    }
	}
	/* Add new Profile */
	index = ProfNewInst(mibType, mac, FALSE);
	ProfBackupByIndex(mibType, profAction, index, &profhead);
	prof = (Wireless5gMacFilterProfile *)ProfGetInstPointByName(mibType, mac);
	if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0))
	{
	    prof->mac = tmp;
	}
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
    }
    else
    {
        oldMac = websGetVar(wp, T("oldMac"), T(""));
	for (i = 0; i <=strlen(oldMac); i++)
	{
	    if ((oldMac[i] >= 97) && (oldMac[i] <= 122))
	    {
		oldMac[i] -= 32;
	    }
	}
	if (0 == strcmp(oldMac, mac))
	{
            websRedirect(wp, "WlanMacFilter_5g.asp");
	    return;
	}
	/* whether tmpmac already exist */
	ProfInstLowHigh(mibType, &max, &min);
	for (index = min; index < max; index++)
	{
	    profName = ProfGetInstNameByIndex(mibType, index);
	    if(0 == strcmp(mac, profName))
	    {
			ErrMsg = getTransDataForC(C_LANG_INDEX_MAC_ADDR_LIST_EXIT_ADDR);
			setTheErrorMsg(ErrMsg);
			websRedirect(wp, "WlanMacFilter.asp");
			return;
	    }
	}
	/* Change Profile */
	prof = (Wireless5gMacFilterProfile *)ProfGetInstPointByName(mibType, oldMac);
	ProfBackupByName(mibType, profAction, oldMac, &profhead);
	if(prof != NULL)
	{
	    prof->mac = tmp;
	}
	ProfSetNameByPoint(prof, mac);
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
    }
    if(isHostInfoPage == 1)
    {
        websRedirect(wp, "WlanHostInfo_5g.asp");
    }
    else
    {
        websRedirect(wp, "WlanMacFilter_5g.asp");
    }
}
#endif
/***********************************************************************
 * 函 数 名：   formConfigWlanMacFilterDel
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void formConfigWlanMacFilterDel(webs_t wp, char_t *path, char_t *query)
{
    char_t *delStr, *pages, *tmp = NULL, *ptr = NULL;
    int i;
    WirelessMacFilterProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_WIRELESS_MAC_FILTER;
    ProfChangeType profAction = PROF_CHANGE_DEL;
    struProfAlloc *profhead = NULL;
    
    delStr = websGetVar(wp, T("delstr"), T(""));
    pages = websGetVar(wp, T("pages"), T("")); 
    if(0 == strcmp(delStr, ""))
    {
        if(0 == strcmp(pages, ""))
	{
            websRedirect(wp, "WlanMacFilter.asp");
        }
	else
	{
            websRedirect(wp, "WlanHostInfo.asp");
        }
        return;
    }
    delColon(delStr, ':');
    for (i = 0; i <=strlen(delStr); i++)
    {
	if ((delStr[i] >= 97) && (delStr[i] <= 122))
	{
	    delStr[i] -= 32;
	}
    }
    tmp=strtok_r(delStr, ",", &ptr);/*可能删除多条记录*/
    while(tmp != NULL)
    {
	prof = ProfGetInstPointByName(mibType, tmp);/*获取配置地址*/
	if (prof != NULL)
	{
	    ProfBackupByName(mibType, profAction, tmp, &profhead);
	    ProfDelInstByName(mibType, tmp);/*删除已有实例*/
	}
	tmp=strtok_r(NULL, ",", &ptr);/*可能删除多条记录*/
    }
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    
    if(!strcmp(pages, "")){
        websRedirect(wp, "WlanMacFilter.asp");
    } else {
        websRedirect(wp, "WlanHostInfo.asp");
    }
}

#if(WIRELESS_5G == FYES)
static void formConfigWlanMacFilterDel_5g(webs_t wp, char_t *path, char_t *query)
{
    char_t *delStr, *pages, *tmp = NULL, *ptr = NULL;
    int i;
    Wireless5gMacFilterProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_WIRELESS_5G_MAC_FILTER;
    ProfChangeType profAction = PROF_CHANGE_DEL;
    struProfAlloc *profhead = NULL;
    
    delStr = websGetVar(wp, T("delstr"), T(""));
    pages = websGetVar(wp, T("pages"), T("")); 
    if(0 == strcmp(delStr, ""))
    {
        if(0 == strcmp(pages, ""))
	{
            websRedirect(wp, "WlanMacFilter_5g.asp");
        }
	else
	{
            websRedirect(wp, "WlanHostInfo_5g.asp");
        }
        return;
    }
    delColon(delStr, ':');
    for (i = 0; i <=strlen(delStr); i++)
    {
	if ((delStr[i] >= 97) && (delStr[i] <= 122))
	{
	    delStr[i] -= 32;
	}
    }
    tmp=strtok_r(delStr, ",", &ptr);/*可能删除多条记录*/
    while(tmp != NULL)
    {
	prof = ProfGetInstPointByName(mibType, tmp);/*获取配置地址*/
	if (prof != NULL)
	{
	    ProfBackupByName(mibType, profAction, tmp, &profhead);
	    ProfDelInstByName(mibType, tmp);/*删除已有实例*/
	}
	tmp=strtok_r(NULL, ",", &ptr);/*可能删除多条记录*/
    }
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    
    if(!strcmp(pages, "")){
        websRedirect(wp, "WlanMacFilter_5g.asp");
    } else {
        websRedirect(wp, "WlanHostInfo_5g.asp");
    }
}
#endif
/***********************************************************************
 * 函 数 名：   formConfigWlanMacFilterDelAll
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void formConfigWlanMacFilterDelAll(webs_t wp, char_t *path, char_t *query)
{
    int min, max, index;
    MibProfileType mibType = MIB_PROF_WIRELESS_MAC_FILTER;
    WirelessMacFilterProfile *prof = NULL;
    ProfChangeType profAction = PROF_CHANGE_DELALL;
    struProfAlloc *profhead = NULL;
    
    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
	prof = (WirelessMacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0))
	{
	    ProfBackupByIndex(mibType, profAction, index, &profhead);
	    ProfDelInstByIndex(mibType, index);
	}
    }
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    
    websRedirect(wp, "WlanMacFilter.asp");
}

#if(WIRELESS_5G == FYES)
static void formConfigWlanMacFilterDelAll_5g(webs_t wp, char_t *path, char_t *query)
{
    int min, max, index;
    MibProfileType mibType = MIB_PROF_WIRELESS_5G_MAC_FILTER;
    Wireless5gMacFilterProfile *prof = NULL;
    ProfChangeType profAction = PROF_CHANGE_DELALL;
    struProfAlloc *profhead = NULL;
    
    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
	prof = (Wireless5gMacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0))
	{
	    ProfBackupByIndex(mibType, profAction, index, &profhead);
	    ProfDelInstByIndex(mibType, index);
	}
    }
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    
    websRedirect(wp, "WlanMacFilter_5g.asp");
}
#endif
/***********************************************************************
 * 函 数 名：   formWlanMacFilterGlobalConfig
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void formWlanMacFilterGlobalConfig(webs_t wp, char_t *path, char_t *query)
{
    char_t *macfiteren, *filterRule;
    int index = 0;
    MibProfileType mibTypeGlobal = MIB_PROF_WIRELESS_MAC_FILTER_GLOBAL;
    WirelessMacFilterGlobalProfile *profGlobal = NULL;
    ProfChangeType profAction = PROF_CHANGE_EDIT;
    struProfAlloc *profhead = NULL;

    macfiteren=websGetVar(wp, "MacFilterEnable", T(""));
    filterRule=websGetVar(wp, "filterRule", T(""));
    profGlobal = (WirelessMacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeGlobal, index);
    if(profGlobal != NULL)
    {
	ProfBackupByIndex(mibTypeGlobal, profAction, index, &profhead);
	if (0 == strncmp(macfiteren, "on", 3))
	{
	    profGlobal->filterEn = FUN_ENABLE;
	}
	else
	{
	    profGlobal->filterEn = FUN_DISABLE;
	}
	if (0 == strcmp(filterRule, "1"))
	{
	    profGlobal->action = AP_MAC_ALLOW;
	}
	else
	{
	    profGlobal->action = AP_MAC_DENY;
	}
    }
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    
    websRedirect(wp, "WlanMacFilter.asp");
}

#if(WIRELESS_5G == FYES)
static void formWlanMacFilterGlobalConfig_5g(webs_t wp, char_t *path, char_t *query)
{
    char_t *macfiteren, *filterRule;
    int index = 1;
    MibProfileType mibTypeGlobal = MIB_PROF_WIRELESS_MAC_FILTER_GLOBAL;
    WirelessMacFilterGlobalProfile *profGlobal = NULL;
    ProfChangeType profAction = PROF_CHANGE_EDIT;
    struProfAlloc *profhead = NULL;

    macfiteren=websGetVar(wp, "MacFilterEnable", T(""));
    filterRule=websGetVar(wp, "filterRule", T(""));
    profGlobal = (WirelessMacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeGlobal, index);
    if(profGlobal != NULL)
    {
	ProfBackupByIndex(mibTypeGlobal, profAction, index, &profhead);
	if (0 == strncmp(macfiteren, "on", 3))
	{
	    profGlobal->filterEn = FUN_ENABLE;
	}
	else
	{
	    profGlobal->filterEn = FUN_DISABLE;
	}
	if (0 == strcmp(filterRule, "1"))
	{
	    profGlobal->action = AP_MAC_ALLOW;
	}
	else
	{
	    profGlobal->action = AP_MAC_DENY;
	}
    }
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    
    websRedirect(wp, "WlanMacFilter_5g.asp");
}
#endif
/***********************************************************************
 * 函 数 名：   formDefineWirelessMacFilter
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
void formDefineWirelessMacFilter(void)
{
    websAspDefine(T("outputWlanMacFilterConfig"), getWlanMacFilterMacList);
    websFormDefine(T("ConfigWlanMacFilter"), formConfigWlanMacFilter);
    websFormDefine(T("ConfigWlanMacFilterDel"), formConfigWlanMacFilterDel);
    websFormDefine(T("ConfigWlanMacFilterDelAll"), formConfigWlanMacFilterDelAll);
    websFormDefine(T("ConfigWlanMacFilterGlobalConfig"), formWlanMacFilterGlobalConfig);

#if(WIRELESS_5G == FYES)
    websAspDefine(T("outputWlanMacFilterConfig_5g"), getWlanMacFilterMacList_5g);
    websFormDefine(T("ConfigWlanMacFilter_5g"), formConfigWlanMacFilter_5g);
    websFormDefine(T("ConfigWlanMacFilterDel_5g"), formConfigWlanMacFilterDel_5g);
    websFormDefine(T("ConfigWlanMacFilterDelAll_5g"), formConfigWlanMacFilterDelAll_5g);
    websFormDefine(T("ConfigWlanMacFilterGlobalConfig_5g"), formWlanMacFilterGlobalConfig_5g);
#endif
}
#endif
