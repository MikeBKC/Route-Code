#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"

#if (WIRELESS == FYES)

extern char* converMac6To12Byte(MacAddr macAddr);
extern int converMac12To6Byte(char *macStr, MacAddr* macAddr);
void wirelessMacFilterAdd();
#if (WIRELESS_5G == FYES)
void wireless5gMacFilterAdd();
#endif

/***********************************************************************
 * 函 数 名：   wirelessMacFilterGlobal
 * 创建日期：	2011-11-02
 * 修改日期：
 * 作者：       
 * 附加说明：	初始化函数
 ***********************************************************************/
void wirelessMacFilterGlobal(unsigned int index)
{
    MibProfileType mibTypeGlobal = MIB_PROF_WIRELESS_MAC_FILTER_GLOBAL;
    WirelessMacFilterGlobalProfile *profGlobal;

    if (index == 0) {
	wirelessMacFilterAdd();
    }

    if (index == 1) {
#if (WIRELESS_5G == FYES)
	wireless5gMacFilterAdd();
#endif
    }
#if(WLAN_MULTI_SSID == FYES)
    WirelessProfile *WirelessProf= (WirelessProfile *)ProfGetInstPointByIndex(MIB_PROF_WIRELESS,0);
    UINT32  BssidNum=WirelessProf->mBaseCf.BssidNum;
#endif

    profGlobal = (WirelessMacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeGlobal, index);
    if (profGlobal->filterEn == FUN_DISABLE)
    {
        if (index == 0) {
            doSystem("iwpriv ra0 set AccessPolicy=%d", 0);
#if(WLAN_MULTI_SSID == FYES)
            if(BssidNum ==2){
                doSystem("iwpriv ra1 set AccessPolicy=%d", 0);
            }
#endif
#if (MIDDLE_SOFT == FYES)
            else if(BssidNum == 4){
                doSystem("iwpriv ra2 set AccessPolicy=%d", 0);
                doSystem("iwpriv ra3 set AccessPolicy=%d", 0);
            }
#endif
        } else {
            doSystem("iwpriv rai0 set AccessPolicy=%d", 0);
#if(WLAN_MULTI_SSID == FYES)
            if(BssidNum ==2){
                doSystem("iwpriv rai1 set AccessPolicy=%d", 0);
            }
#endif
        }
    }
    else
    {
        if (profGlobal->action == AP_MAC_ALLOW)
        {
            if (index == 0) {
                doSystem("iwpriv ra0 set AccessPolicy=%d", 1);
#if(WLAN_MULTI_SSID == FYES)
                if(BssidNum >=2){
                    doSystem("iwpriv ra1 set AccessPolicy=%d", 1);
                }
#endif
#if (MIDDLE_SOFT == FYES)
                if(BssidNum == 4){
                    doSystem("iwpriv ra2 set AccessPolicy=%d", 1);
                    doSystem("iwpriv ra3 set AccessPolicy=%d", 1);
                }
#endif
            } else {
                doSystem("iwpriv rai0 set AccessPolicy=%d", 1);
#if(WLAN_MULTI_SSID == FYES)
                if(BssidNum >=2){
                    doSystem("iwpriv rai1 set AccessPolicy=%d", 1);
                }
#endif
            }
        }
        else
        {
            if (index == 0) {
                doSystem("iwpriv ra0 set AccessPolicy=%d", 2);
#if(WLAN_MULTI_SSID == FYES)
                if(BssidNum >=2){
                    doSystem("iwpriv ra1 set AccessPolicy=%d", 2);
                }
#endif
#if (MIDDLE_SOFT == FYES)
                if(BssidNum == 4){
                    doSystem("iwpriv ra2 set AccessPolicy=%d", 2);
                    doSystem("iwpriv ra3 set AccessPolicy=%d", 2);
                }
#endif
            } else {
                doSystem("iwpriv rai0 set AccessPolicy=%d", 2);
#if(WLAN_MULTI_SSID == FYES)
                if(BssidNum >=2){
                    doSystem("iwpriv rai1 set AccessPolicy=%d", 2);
                }
#endif
            }
        }
    }
}

/***********************************************************************
 * 函 数 名：   wirelessMacFilterAdd 
 * 创建日期：	2011-11-02
 * 修改日期：
 * 作者：       
 * 附加说明：	添加函数
 ***********************************************************************/
void wirelessMacFilterAdd()
{
    int min, max, index;
    char wlanMac[1024] = {0};
    char *tmpmac; 
    WirelessMacFilterProfile *prof;
    WirelessMacFilterGlobalProfile *profGlobal;
    MibProfileType mibType = MIB_PROF_WIRELESS_MAC_FILTER;
    MibProfileType mibTypeGlobal = MIB_PROF_WIRELESS_MAC_FILTER_GLOBAL;
#if(WLAN_MULTI_SSID == FYES)
    WirelessProfile *WirelessProf= (WirelessProfile *)ProfGetInstPointByIndex(MIB_PROF_WIRELESS,0);
    UINT32  BssidNum=WirelessProf->mBaseCf.BssidNum;
#endif

    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
	prof = (WirelessMacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0))
	{
	    tmpmac = converMac6To12Byte(prof->mac);
	    Conversion2StdMac(tmpmac);
	    strcat(wlanMac, tmpmac);
	    strcat(wlanMac, ";");
	}
	else
	{
	    continue;
	}
    }
    wlanMac[strlen(wlanMac)-1] = '\0';
    doSystem("iwpriv ra0  set ACLClearAll=1");
#if(WLAN_MULTI_SSID == FYES)
    if(BssidNum >= 2){
	doSystem("iwpriv ra1  set ACLClearAll=1");
    }
#endif
#if (MIDDLE_SOFT == FYES)
    if(BssidNum == 4)
    {
        doSystem("iwpriv ra2  set ACLClearAll=1");
	    doSystem("iwpriv ra3  set ACLClearAll=1");
    }
#endif
    doSystem("iwpriv ra0 set \"ACLAddEntry=%s\" ", wlanMac);
#if(WLAN_MULTI_SSID == FYES)
    if(BssidNum >= 2){
	doSystem("iwpriv ra1 set \"ACLAddEntry=%s\" ", wlanMac);
    }
#endif
#if (MIDDLE_SOFT == FYES)
    if(BssidNum == 4){
	    doSystem("iwpriv ra2 set \"ACLAddEntry=%s\" ", wlanMac);
	    doSystem("iwpriv ra3 set \"ACLAddEntry=%s\" ", wlanMac);
    }
#endif
    profGlobal = (WirelessMacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeGlobal, 0);
    if (profGlobal->filterEn == FUN_ENABLE)
    {
	if (profGlobal->action == AP_MAC_ALLOW)
	{
	    doSystem("iwpriv ra0 set AccessPolicy=%d", 1); 
#if(WLAN_MULTI_SSID == FYES)
	    if(BssidNum >= 2){
		doSystem("iwpriv ra1 set AccessPolicy=%d", 1); 
	    }
#endif
	}
	else if (profGlobal->action == AP_MAC_DENY)
	{
	    doSystem("iwpriv ra0 set AccessPolicy=%d", 2); 
#if(WLAN_MULTI_SSID == FYES)
	    if(BssidNum >= 2){
		doSystem("iwpriv ra1 set AccessPolicy=%d", 2);
	    }
#endif
	}
    }
    else
    {
	doSystem("iwpriv ra0 set AccessPolicy=%d", 0); 
#if(WLAN_MULTI_SSID == FYES)
	if(BssidNum >= 2){
	    doSystem("iwpriv ra1 set AccessPolicy=%d", 0);
	}
#endif
    }
}

#if (WIRELESS_5G == FYES)
void wireless5gMacFilterAdd()
{
    int min, max, index;
    char wlanMac[1024] = {0};
    char *tmpmac; 
    Wireless5gMacFilterProfile *prof;
    WirelessMacFilterGlobalProfile *profGlobal;
    MibProfileType mibType = MIB_PROF_WIRELESS_5G_MAC_FILTER;
    MibProfileType mibTypeGlobal = MIB_PROF_WIRELESS_MAC_FILTER_GLOBAL;

#if(WLAN_MULTI_SSID == FYES)
    WirelessProfile *WirelessProf= (WirelessProfile *)ProfGetInstPointByIndex(MIB_PROF_WIRELESS,0);
    UINT32  BssidNum=WirelessProf->mBaseCf.BssidNum;
#endif

    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
	prof = (Wireless5gMacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0))
	{
	    tmpmac = converMac6To12Byte(prof->mac);
	    Conversion2StdMac(tmpmac);
	    strcat(wlanMac, tmpmac);
	    strcat(wlanMac, ";");
	}
	else
	{
	    continue;
	}
    }
    wlanMac[strlen(wlanMac)-1] = '\0';
    doSystem("iwpriv rai0  set ACLClearAll=1");
#if(WLAN_MULTI_SSID == FYES)
    if(BssidNum == 2){
	doSystem("iwpriv rai1  set ACLClearAll=1");
    }
#endif
    doSystem("iwpriv rai0 set \"ACLAddEntry=%s\" ", wlanMac);
#if(WLAN_MULTI_SSID == FYES)
    if(BssidNum >= 2){
	doSystem("iwpriv rai1 set \"ACLAddEntry=%s\" ", wlanMac);
    }
#endif
    profGlobal = (WirelessMacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeGlobal, 1);
    if (profGlobal->filterEn == FUN_ENABLE)
    {
	if (profGlobal->action == AP_MAC_ALLOW)
	{
	    doSystem("iwpriv rai0 set AccessPolicy=%d", 1); 
#if(WLAN_MULTI_SSID == FYES)
	    if(BssidNum >= 2){
		doSystem("iwpriv rai1 set AccessPolicy=%d", 1);
	    }
#endif
	}
	else if (profGlobal->action == AP_MAC_DENY)
	{
	    doSystem("iwpriv rai0 set AccessPolicy=%d", 2); 
#if(WLAN_MULTI_SSID == FYES)
	    if(BssidNum >= 2){
		doSystem("iwpriv rai1 set AccessPolicy=%d", 2);
	    }
#endif
	}
    }
    else
    {
	doSystem("iwpriv rai0 set AccessPolicy=%d", 0); 
#if(WLAN_MULTI_SSID == FYES)
	if(BssidNum >= 2){
	    doSystem("iwpriv rai1 set AccessPolicy=%d", 0);
	}
#endif
    }
}
#endif
/***********************************************************************
 * 函 数 名：   wirelessMacFilterDel
 * 创建日期：	2011-11-02
 * 修改日期：
 * 作者：       
 * 附加说明：	删除函数
 ***********************************************************************/
void wirelessMacFilterDel()
{
    int min, max, index;
    char wlanMac[1024] = {0};
    char *tmpmac; 
    const char *profName = NULL;
    WirelessMacFilterProfile *prof;
    WirelessMacFilterGlobalProfile *profGlobal;
    MibProfileType mibType = MIB_PROF_WIRELESS_MAC_FILTER;
    MibProfileType mibTypeGlobal = MIB_PROF_WIRELESS_MAC_FILTER_GLOBAL;
#if(WLAN_MULTI_SSID == FYES)
    WirelessProfile *WirelessProf= (WirelessProfile *)ProfGetInstPointByIndex(MIB_PROF_WIRELESS,0);
    UINT32  BssidNum=WirelessProf->mBaseCf.BssidNum;
#endif

    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
	prof = (WirelessMacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	profName = ProfGetInstNameByIndex(mibType, index);
	if (0 != strcmp(profName, ""))
	{
	    tmpmac = converMac6To12Byte(prof->mac);
	    Conversion2StdMac(tmpmac);
	    strcat(wlanMac, tmpmac);
	    strcat(wlanMac, ";");
	}
	else
	{
	    continue;
	}
    }
    wlanMac[strlen(wlanMac)-1] = '\0';
    doSystem("iwpriv ra0  set ACLClearAll=1");
#if(WLAN_MULTI_SSID == FYES)
    if( BssidNum >= 2 ){
	doSystem("iwpriv ra1  set ACLClearAll=1");
    }
#endif
    doSystem("iwpriv ra0 set \"ACLAddEntry=%s\" ", wlanMac);
#if(WLAN_MULTI_SSID == FYES)
    if(BssidNum >= 2){
	doSystem("iwpriv ra1 set \"ACLAddEntry=%s\" ", wlanMac);
    }
#endif
    profGlobal = (WirelessMacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeGlobal, 0);
    if (profGlobal->filterEn == FUN_ENABLE)
    {
	if (profGlobal->action == AP_MAC_ALLOW)
	{
	    doSystem("iwpriv ra0 set AccessPolicy=%d", 1); 
#if(WLAN_MULTI_SSID == FYES)
	    if(BssidNum >= 2){
		doSystem("iwpriv ra1 set AccessPolicy=%d", 1);
	    }
#endif
	}
	else if (profGlobal->action == AP_MAC_DENY)
	{
	    doSystem("iwpriv ra0 set AccessPolicy=%d", 2); 
#if(WLAN_MULTI_SSID == FYES)
	    if(BssidNum >= 2){
		doSystem("iwpriv ra1 set AccessPolicy=%d", 2);
	    }
#endif
	}
    }
    else
    {
	doSystem("iwpriv ra0 set AccessPolicy=%d", 0); 
#if(WLAN_MULTI_SSID == FYES)
	if(BssidNum >= 2){
	    doSystem("iwpriv ra1 set AccessPolicy=%d", 0);
	}
#endif
    }
}

#if (WIRELESS_5G == FYES)
void wireless5gMacFilterDel()
{
    int min, max, index;
    char wlanMac[1024] = {0};
    char *tmpmac; 
    const char *profName = NULL;
    Wireless5gMacFilterProfile *prof;
    WirelessMacFilterGlobalProfile *profGlobal;
    MibProfileType mibType = MIB_PROF_WIRELESS_5G_MAC_FILTER;
    MibProfileType mibTypeGlobal = MIB_PROF_WIRELESS_MAC_FILTER_GLOBAL;
#if(WLAN_MULTI_SSID == FYES)
    WirelessProfile *WirelessProf= (WirelessProfile *)ProfGetInstPointByIndex(MIB_PROF_WIRELESS,0);
    UINT32  BssidNum=WirelessProf->mBaseCf.BssidNum;
#endif

    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
	prof = (Wireless5gMacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	profName = ProfGetInstNameByIndex(mibType, index);
	if (0 != strcmp(profName, ""))
	{
	    tmpmac = converMac6To12Byte(prof->mac);
	    Conversion2StdMac(tmpmac);
	    strcat(wlanMac, tmpmac);
	    strcat(wlanMac, ";");
	}
	else
	{
	    continue;
	}
    }
    wlanMac[strlen(wlanMac)-1] = '\0';
    doSystem("iwpriv rai0  set ACLClearAll=1");
#if(WLAN_MULTI_SSID == FYES)
    if(BssidNum >= 2){
	doSystem("iwpriv rai1  set ACLClearAll=1");
    }
#endif
    doSystem("iwpriv rai0 set \"ACLAddEntry=%s\" ", wlanMac);
#if(WLAN_MULTI_SSID == FYES)
    if(BssidNum >= 2){
	doSystem("iwpriv rai1 set \"ACLAddEntry=%s\" ", wlanMac);
    }
#endif
    profGlobal = (WirelessMacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeGlobal, 1);
    if (profGlobal->filterEn == FUN_ENABLE)
    {
	if (profGlobal->action == AP_MAC_ALLOW)
	{
	    doSystem("iwpriv rai0 set AccessPolicy=%d", 1); 
#if(WLAN_MULTI_SSID == FYES)
	    if(BssidNum >= 2){
		doSystem("iwpriv rai1 set AccessPolicy=%d", 1);
	    }
#endif
	}
	else if (profGlobal->action == AP_MAC_DENY)
	{
	    doSystem("iwpriv rai0 set AccessPolicy=%d", 2); 
#if(WLAN_MULTI_SSID == FYES)
	    if(BssidNum >= 2){
		doSystem("iwpriv rai1 set AccessPolicy=%d", 2);
	    }
#endif
	}
    }
    else
    {
	doSystem("iwpriv rai0 set AccessPolicy=%d", 0); 
#if(WLAN_MULTI_SSID == FYES)
	if(BssidNum >= 2){
	    doSystem("iwpriv rai1 set AccessPolicy=%d", 0);
	}
#endif
    }
}
#endif
/***********************************************************************
 * 函 数 名：   wirelessMacFilterDelAll
 * 创建日期：	2011-11-02
 * 修改日期：
 * 作者：       
 * 附加说明：	删除所有函数
 ***********************************************************************/
void wirelessMacFilterDelAll(void)
{
#if(WLAN_MULTI_SSID == FYES)
    WirelessProfile *WirelessProf= (WirelessProfile *)ProfGetInstPointByIndex(MIB_PROF_WIRELESS,0);
    UINT32  BssidNum=WirelessProf->mBaseCf.BssidNum;
#endif

    doSystem("iwpriv ra0  set ACLClearAll=1");
#if(WLAN_MULTI_SSID == FYES)
    if(BssidNum >= 2){
	doSystem("iwpriv ra1  set ACLClearAll=1");
    }
#endif
}

#if (WIRELESS_5G == FYES)
void wireless5gMacFilterDelAll(void)
{
#if(WLAN_MULTI_SSID == FYES)
    WirelessProfile *WirelessProf= (WirelessProfile *)ProfGetInstPointByIndex(MIB_PROF_WIRELESS,0);
    UINT32  BssidNum=WirelessProf->mBaseCf.BssidNum;
#endif

    doSystem("iwpriv rai0 set ACLClearAll=1");
#if(WLAN_MULTI_SSID == FYES)
    if(BssidNum >= 2){
	doSystem("iwpriv rai1 set ACLClearAll=1");
    }
#endif
}
#endif
/***********************************************************************
 * 函 数 名：   wirelessMacFilterProfileChange
 * 创建日期：	2011-11-02
 * 修改日期：
 * 作者：       
 * 附加说明：	生效函数
 ***********************************************************************/
void wirelessMacFilterProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const WirelessMacFilterProfile *oldProfileDataPtr, const WirelessMacFilterProfile *newProfileDataPtr )
{
    switch(changeType) {
	case PROF_CHANGE_ADD:
	    wirelessMacFilterAdd();
	    break;
	case PROF_CHANGE_EDIT:
	    break;
	case PROF_CHANGE_DEL:
	    wirelessMacFilterDel();
	    break;
	case PROF_CHANGE_DELALL:
	    wirelessMacFilterDelAll();
	    break;
	default:
	    break;
    };
}

#if (WIRELESS_5G == FYES)
void wireless5gMacFilterProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const Wireless5gMacFilterProfile *oldProfileDataPtr, const Wireless5gMacFilterProfile *newProfileDataPtr )
{
    switch(changeType) {
	case PROF_CHANGE_ADD:
	    wireless5gMacFilterAdd();
	    break;
	case PROF_CHANGE_EDIT:
	    break;
	case PROF_CHANGE_DEL:
	    wireless5gMacFilterDel();
	    break;
	case PROF_CHANGE_DELALL:
	    wireless5gMacFilterDelAll();
	    break;
	default:
	    break;
    };
}
#endif

void wirelessMacFilterGlobalProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const WirelessMacFilterProfile *oldProfileDataPtr, const WirelessMacFilterProfile *newProfileDataPtr )
{
    switch(changeType) {
	case PROF_CHANGE_ADD:
	    break;
	case PROF_CHANGE_EDIT:
	    wirelessMacFilterGlobal(index);
	    break;
	case PROF_CHANGE_DEL:
	    break;
	case PROF_CHANGE_DELALL:
	    break;
	default:
	    break;
    };
}

/***********************************************************************
 * 函 数 名：   funInitWirelessMacFilter
 * 创建日期：	2011-11-02
 * 修改日期：
 * 作者：       
 * 附加说明：	注册函数
 ***********************************************************************/
void funInitWirelessMacFilter(void)
{
    registerForProfileUpdates(MIB_PROF_WIRELESS_MAC_FILTER, (Pointer)NULL, (ProfileUpdateFnc)wirelessMacFilterProfileChange);
}

#if (WIRELESS_5G == FYES)
void funInitWireless5gMacFilter(void)
{
    registerForProfileUpdates(MIB_PROF_WIRELESS_5G_MAC_FILTER, (Pointer)NULL, (ProfileUpdateFnc)wireless5gMacFilterProfileChange);
}
#endif

void funInitWirelessMacFilterGlobal(void)
{
    registerForProfileUpdates(MIB_PROF_WIRELESS_MAC_FILTER_GLOBAL, (Pointer)NULL, (ProfileUpdateFnc)wirelessMacFilterGlobalProfileChange);
#if(FEATURE_AP == FYES)
    MibProfileType apModeMibType = MIB_PROF_FAT_FIT_SWITCH;
    FatFitSwitchProfile *apModeProf = NULL;

    apModeProf = (FatFitSwitchProfile *)ProfGetInstPointByIndex(apModeMibType, 0);
    if ((apModeProf->fatEnable == PROF_ENABLE))
	{
        wirelessMacFilterGlobal(0);
#if (WIRELESS_5G == FYES)
    wirelessMacFilterGlobal(1);
#endif
	}
#else
    wirelessMacFilterGlobal(0);
#if (WIRELESS_5G == FYES)
    wirelessMacFilterGlobal(1);
#endif
#endif
}
#endif
