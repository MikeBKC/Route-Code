#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "mib.h"
#include "profacce.h"
#include "uttfunction.h"
#include "translate.h"

#ifdef  WIRE_BASE_DBG
#define WIRE_BASE_PRNTF(fmt, args...)		printf(fmt, ## args)
#else
#define WIRE_BASE_PRNTF(fmt, args...)
#endif
#if (FEATURE_AP == FYES)
static int  aspOutWifiMaxRate(int eid, webs_t wp, int argc, char_t **argv)
{
    websWrite(wp , "var maxRate = %d;\n", MAX_WIFI_RATE);
}
#endif
#if (WIRELESS_BASE_CONFIG == FYES)
#if 0
static void getWirelessBaseInfo(int eid, webs_t wp, int argc, char_t **argv);
#endif
extern int converMac12To6Byte(char *macStr, MacAddr* macAddr);
extern char* converMac6To12Byte(MacAddr macAddr);
static int  aspOutPutApScan(int eid, webs_t wp, int argc, char_t **argv);
static int  ApScan(int eid, webs_t wp, int argc, char_t **argv);
static void delMacColon(char str[20], char ch);
#if (UTT_SMART_UI == FYES)
static void formApScan(webs_t wp, char_t *path, char_t *query);
static void formApScanResult(webs_t wp, char_t *path, char_t *query);
extern int wireless_date_check(void);
static void formApcliConnStatus(webs_t wp, char_t *path, char_t *query);
static int getWifiMac(WirelessProfile *prof, char *if_name, char wifiMac[]);
static int  getWifiInfo(int eid, webs_t wp, int argc, char_t **argv);
#if (UTT_BRIDEG_MODE_FLAG == FYES)
static void Formchangemodel(webs_t wp, char_t *path, char_t *query);
static int outputChangemodel(int eid, webs_t wp, int argc, char_t **argv);
#endif
/********************************************************************
 * º¯Êý£º formApScan 
 * ¹¦ÄÜ£º ÏìÓ¦Ò³ÃæÇëÇó£¬Ö´ÐÐAPÉ¨ÃèµÄÃüÁî
 * ´´½¨£º 2013-11-20
 * ²ÎÊý£º ÂÔ
 * ·µ»Ø£º ÎÞ
 * Êä³ö£º ÂÔ
 * ×÷Õß£º ÕÔ³å 
 ********************************************************************/ 
static void formApScan(webs_t wp, char_t *path, char_t *query)
{
    char_t *strProfIndex;
    int profIndex;

    strProfIndex = websGetVar(wp, T("profIndex"), T("0"));
    profIndex = strtoul(strProfIndex, 0, 10);
    if (profIndex == 1) {
	system("iwpriv rai0 set CountryRegionABand=0;iwpriv rai0 set SiteSurvey=1");
    } else {
	system("iwpriv ra0 set CountryRegion=1;iwpriv ra0 set SiteSurvey=1");
    }
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain;charset=GB2312\nPragma: no-cache\nCache-Control: no-cache\n\n"));
    websDone(wp,200);
}

/********************************************************************
 * º¯Êý£º formApcliConnStatus 
 * ¹¦ÄÜ£º ÏìÓ¦Ò³ÃæÇëÇó£¬»ñÈ¡ApclientµÄÁ¬½ÓÐÅÏ¢
 * ´´½¨£º 2013-12-18
 * ²ÎÊý£º ÂÔ
 * ·µ»Ø£º ÎÞ
 * Êä³ö£º ÂÔ
 * ×÷Õß£º ÕÔ³å 
 ********************************************************************/ 
static void formApcliConnStatus(webs_t wp, char_t *path, char_t *query)
{
    char_t *strProfIndex;
    int profIndex;
    FILE *fp;
    char line[32], *tmp;
    unsigned int connStatus = 0U;

    strProfIndex = websGetVar(wp, T("profIndex"), T("0"));/*»ñÈ¡ÎÞÏßÊµÀýË÷Òý£¬ÓÃÓÚ±êÊ¶5G»¹ÊÇ2.4G*/
    profIndex = strtoul(strProfIndex, 0, 10);
    memset(line,0,sizeof(line));

    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain;charset=GB2312\nPragma: no-cache\nCache-Control: no-cache\n\n"));
    if (profIndex == 1) {/*5GÍøÂç*/
	fp = popen("iwpriv apclii0 get_apc_st", "r");
    } else {
	fp = popen("iwpriv apcli0 get_apc_st", "r");
    }

    if (fp != NULL) {
	fgets(line, sizeof(line), fp);/*¹ýÂËµôµÚÒ»ÐÐ±êÌâÐÅÏ¢*/
	memset(line,0,sizeof(line));
	fgets(line, sizeof(line), fp);
	tmp = strchr(line,'\n');
	if (tmp != NULL) {
	    *tmp = '\0';
	}
	if (*line != '\0') {
	    websWrite(wp,T("var %s;"),line);
	} else if (profIndex == 1){
	    websWrite(wp,T("var apclii0=0;"));
	} else {
	    websWrite(wp,T("var apcli0=0;"));
	}
	pclose(fp);
    } else {/*Á¬½ÓÖÐ*/
	if (profIndex == 1) {/*5GÍøÂç*/
	    websWrite(wp,T("var apclii0=2;"));
	}else {
	    websWrite(wp,T("var apcli0=2;"));
	}
    }
    websDone(wp,200);
}
/********************************************************************
 * º¯Êý£º formApScanResult 
 * ¹¦ÄÜ£º ·µ»ØAPÉ¨ÃèµÄ½á¹ûÁÐ±í
 * ´´½¨£º 2013-11-20
 * ²ÎÊý£º ÂÔ
 * ·µ»Ø£º ÎÞ
 * Êä³ö£º ÂÔ
 * ×÷Õß£º ÕÔ³å 
 ********************************************************************/ 
static void formApScanResult(webs_t wp, char_t *path, char_t *query)
{
    FILE *fp;
    char line[128], ssid[33], bssid[24], encrypt[30];
    unsigned int ch = 0u, signal = 0u;
    int total = 0, n = 0;
    char_t *strProfIndex;
    int profIndex;

    strProfIndex = websGetVar(wp, T("profIndex"), T("0"));
    profIndex = strtoul(strProfIndex, 0, 10);
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain;charset=GBK\nPragma: no-cache\nCache-Control: no-cache\n\n"));

    if (profIndex == 1) {
	fp = popen("iwpriv rai0 get_site_survey", "r");
    } else {
	fp = popen("iwpriv ra0 get_site_survey", "r");
    }
    if (fp != NULL) {
	fgets(line, sizeof(line), fp);
	fgets(line, sizeof(line), fp);

	while (fgets(line, sizeof(line), fp) != NULL) {
	    memset(bssid, 0, sizeof(bssid));
	    memset(ssid, 0, sizeof(ssid));
	    memset(encrypt, 0, sizeof(encrypt));

	    n = sscanf(line, "%u%s%s%s%u", &ch, ssid, bssid, encrypt, &signal);
	    WIRE_BASE_PRNTF("ch = %u, ssid = %s, bssid = %s, encrypt = %s, signal = %u, n = %d\n", ch, ssid, bssid, encrypt, signal, n);
	    /*¿ÉÄÜÉ¨ÃèµÃµ½µÄSSIDÎª¿Õ£¬µ«ÊÇÍ¨¹ýsscanf¸³ÖµµÄÊ±ºò»áÊ¹ssid¸³ÖµÎªmac*/
	    if (n < 5 || (strchr(ssid,':') != NULL) || (strchr(bssid,':') == NULL)) {
		continue;
	    }

	    delMacColon(bssid, ':');

	    websWrite(wp, "bridgebssids[%d] = \"%s\";\n", total, bssid);
	    websWrite(wp, "bridgessids[%d] = \"%s\";\n", total, ssid);
	    websWrite(wp, "channelsArr[%d] = \"%d\";\n", total, ch);
	    websWrite(wp, "signalStrength[%d] = \"%d%%\";\n", total, signal);
	    websWrite(wp, "EncryptTypes[%d] = \"%s\";\n", total, encrypt);
#if 0
	    if (strncmp(encrypt, "NONE", 4) == 0) {
		    websWrite(wp, "Encrypts[%d] = \"%s\";\n", total, "·ñ");
	    } else {
		    websWrite(wp, "Encrypts[%d] = \"%s\";\n", total, "ÊÇ");
	    }
#endif

	    // ´ËÐÐÓ¦¸ÃÓÉÒ³ÃæÐ´Èë
	    websWrite(wp, "choice[%d] = \"%s\";\n", total, "Ñ¡Ôñ");
	    total++;
	}

	    pclose(fp);
    }

    websWrite(wp,"var totalrecs = %d;\n", total);
    websWrite(wp,"var max_totalrecs = %d;\n", total);

    websDone(wp, 200);
}
#endif
static int  ApScan(int eid, webs_t wp, int argc, char_t **argv)
{
    system("iwpriv ra0 set SiteSurvey=1");
    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}
#if (UTT_BRIDEG_MODE_FLAG == FYES)
static int outputChangemodel(int eid, webs_t wp, int argc, char_t **argv)
{
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0); 
    websWrite(wp,"var ChangeModel = %d;\n", profSys->brideg_mode_flag); 
}
#endif
static int  aspOutPutApScan(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	char line[128], ssid[33], bssid[24], encrypt[30];
	unsigned int ch = 0u, signal = 0u;
	int total = 0, n = 0;

	websWrite(wp , "var bssids = new Array();\n");
	websWrite(wp , "var ssids = new Array();\n");
	websWrite(wp , "var channels = new Array();\n");
	websWrite(wp , "var signalStrength = new Array();\n");
	websWrite(wp , "var Encrypts = new Array();\n");
	websWrite(wp , "var choice = new Array();\n");

	fp = popen("iwpriv ra0 get_site_survey", "r");
	if (fp != NULL) {
		fgets(line, sizeof(line), fp);
		fgets(line, sizeof(line), fp);

		while (fgets(line, sizeof(line), fp) != NULL) {
			memset(bssid, 0, sizeof(bssid));
			memset(ssid, 0, sizeof(ssid));
			memset(encrypt, 0, sizeof(encrypt));

			n = sscanf(line, "%u %s %s %s %u", &ch, ssid, bssid, encrypt, &signal);
			WIRE_BASE_PRNTF("ch = %u, ssid = %s, bssid = %s, encrypt = %s, signal = %u, n = %d\n", ch, ssid, bssid, encrypt, signal, n);
			if (n < 5 || (strchr(ssid,':') != NULL) || (strchr(bssid,':') == NULL)) {
			    continue;
			}

			delMacColon(bssid, ':');

			websWrite(wp, "bssids[%d] = \"%s\";\n", total, bssid);
			websWrite(wp, "ssids[%d] = \"%s\";\n", total, ssid);
			websWrite(wp, "channels[%d] = \"%d\";\n", total, ch);
			websWrite(wp, "signalStrength[%d] = \"%d%%\";\n", total, signal);

			if (strncmp(encrypt, "NONE", 4) == 0) {
				websWrite(wp, "Encrypts[%d] = \"%s\";\n", total, "·ñ");
			} else {
				websWrite(wp, "Encrypts[%d] = \"%s\";\n", total, "ÊÇ");
			}

			// ´ËÐÐÓ¦¸ÃÓÉÒ³ÃæÐ´Èë
			websWrite(wp, "choice[%d] = \"%s\";\n", total, "Ñ¡Ôñ");
			total++;
		}

		pclose(fp);
	}

    websWrite(wp,"var totalrecs = %d;\n", total);
    websWrite(wp,"var max_totalrecs = %d;\n", total);

    getTheErrorMsg(eid, wp, argc, argv);

	return 0;
}
static void delMacColon(char str[20], char ch)
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


#if (WLAN_WORK_TIME == FYES)
/*
 * ÎÞÏßÊ±¼äÉèÖÃ
 */
extern void editWlanTimeConfig(webs_t wp, char_t *path, char_t *query, int lock)
{
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;    
    struProfAlloc *profhead = NULL;
    char *dayStr = NULL;
    char *weekdayStart, *weekdayStop, *weekendStart, *weekendStop;
#if (UTT_SMART_UI == FYES)
    char *wlanTimeEn = NULL;
    int status = 0;

    wlanTimeEn = websGetVar(wp, T("WrlessEnable"), T(""));
#endif
    dayStr = websGetVar(wp, T("day"), NULL);/*ÐÇÆÚ*/
    weekdayStart = websGetVar(wp, T("weekdaytimestart"), NULL);/*¹¤×÷ÈÕ¿ªÊ¼Ê±¼ä*/
    weekdayStop = websGetVar(wp, T("weekdaytimestop"), NULL);/*¹¤×÷ÈÕ½áÊøÊ±¼ä*/
    weekendStart = websGetVar(wp, T("weekendtimestart"), NULL);/*ÖÜÄ©¿ªÊ¼Ê±¼ä*/
    weekendStop = websGetVar(wp, T("weekendtimestop"), NULL);/*ÖÜÄ©½áÊøÊ±¼ä*/

    prof = (WirelessProfile *)ProfGetInstPointByIndex(mibType, 0);
    if (ProfInstIsFree(prof) == PROFOK)
    {
	if (lock)/*lockÎª1ÔòÐèÒªËø¶¨£¬Îª0²»ÐèÒªËø¶¨*/
	{
	    uttSemP(SEM_WIFI_NO,0);
	}
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
#if (UTT_SMART_UI == FYES)
	if (strcmp(wlanTimeEn, "1") == 0)
	{
	    prof->mBaseCf.WlanTimeEn = FUN_ENABLE;
	}
	else
	{
	    prof->mBaseCf.WlanTimeEn = FUN_DISABLE;
	}
#endif
	if (dayStr != NULL)
	{
	    prof->mBaseCf.day= days2Byte(dayStr);/*×ª»»ÈÕÆÚ*/
	}
	if ((weekdayStart != NULL) && (weekdayStop != NULL) && (weekendStart != NULL) && (weekendStop != NULL))
	{
	    strcpy(prof->mBaseCf.weekdayTimeStart, weekdayStart);
	    strcpy(prof->mBaseCf.weekdayTimeStop, weekdayStop);
	    strcpy(prof->mBaseCf.weekendTimeStart, weekendStart);
	    strcpy(prof->mBaseCf.weekendTimeStop, weekendStop);
	}

#if (UTT_SMART_UI == FYES)
	status = wireless_date_check();
#endif
	if (lock)
	{
	    uttSemV(SEM_WIFI_NO,0);
	}
#if (UTT_SMART_UI == FYES)
	if ((status != -1) && (status != prof->time_active))
	{
#endif
	    ProfUpdate(profhead);
#if (UTT_SMART_UI == FYES)
	}
#endif
	ProfFreeAllocList(profhead);
    }
}
#endif

#if (UTT_SMART_UI == FYES)
/*ÐÂ·ç¸ñUI ÅäÖÃÏòµ¼ÎÞÏß2.4G±£´æ*/
static void formWlanWizard_Sui(webs_t wp, char_t *path, char_t *query)
{
    char *radio, *ssid, *broadcastEnable, *pskPsswds;
    MibProfileType mibType = MIB_PROF_WIRELESS;
    struProfAlloc *profhead = NULL;
    WirelessProfile *prof= NULL;
    char *ErrMsg = NULL;
    int profIndex = 0;
    
    radio = websGetVar(wp, T("ebabledwifi"), T(""));
    ssid = websGetVar(wp, T("wifiname"), T(""));
    broadcastEnable = websGetVar(wp, T("hidenet"), T(""));
    pskPsswds = websGetVar(wp, T("wifipwd"), T(""));

    uttSemP(SEM_WIFI_NO,0);
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, profIndex);
    if (ProfInstIsFree(prof) == PROFOK)
    {
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);

	if (!strncmp(radio, "on", 3)) {
	    prof->head.active = 1;
	}
	else{
	    prof->head.active = 0;
	}

	if (0 == strlen(ssid)) {
	    ErrMsg = getTransDataForC(C_LANG_INDEX_SSID_CANT_NULL);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	strcpy(prof->mBaseCf.SSID1,ssid);
	if (strcmp(broadcastEnable, "on") == 0)
	{
	    prof->mBaseCf.SSIDBroadCastEn = FUN_DISABLE;
	}
	else
	{
	    prof->mBaseCf.SSIDBroadCastEn = FUN_ENABLE;
	}
	if (strcmp(pskPsswds, "") == 0)
	{
	    strcpy(prof->mSafeCf.SelAuthMode, "OPEN");
	    strcpy(prof->mSafeCf.ap.AuthMode.EncrypType,"NONE");
	}
	else
	{
	    strcpy(prof->mSafeCf.SelAuthMode, "WPAPSKWPA2PSK");
	    strcpy(prof->mSafeCf.ap.AuthMode.pskPsswd, pskPsswds);
	    if(strcmp(prof->mSafeCf.ap.AuthMode.EncrypType,"NONE") == 0)
	    {
		strcpy(prof->mSafeCf.ap.AuthMode.EncrypType,"TKIPAES");
	    }
	}

	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
    }

out:
    uttSemV(SEM_WIFI_NO,0);
#if (WIRELESS_5G == FYES)
    websRedirect(wp,"WireWizard_Sui_5g.asp");
#else
    websRedirect(wp,"index_login.asp");
#endif
}

#if (WIRELESS_5G == FYES)
/*ÐÂ·ç¸ñUI ÅäÖÃÏòµ¼ÎÞÏß5G±£´æ*/
static void formWlanWizard_Sui_5g(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_WIRELESS;
    struProfAlloc *profhead = NULL;
    char *radio_5g, *ssid_5g, *broadcastEnable_5g, *pskPsswds_5g;
    WirelessProfile *prof5G= NULL;
    char *ErrMsg = NULL;
    int profIndex = 1;
    
    radio_5g = websGetVar(wp, T("ebabledwifi"), T(""));
    ssid_5g = websGetVar(wp, T("wifiname"), T(""));
    broadcastEnable_5g = websGetVar(wp, T("hidenet"), T(""));
    pskPsswds_5g = websGetVar(wp, T("wifipwd"), T(""));

    uttSemP(SEM_WIFI_NO,0);
    prof5G = (WirelessProfile*)ProfGetInstPointByIndex(mibType, profIndex);
    if (ProfInstIsFree(prof5G) == PROFOK)
    {
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 1, &profhead);

	if (!strncmp(radio_5g, "on", 3)) {
	    prof5G->head.active = 1;
	}
	else{
	    prof5G->head.active = 0;
	}

	if (0 == strlen(ssid_5g)) {
	    ErrMsg = getTransDataForC(C_LANG_INDEX_SSID_CANT_NULL);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	strcpy(prof5G->mBaseCf.SSID1,ssid_5g);
	if (strcmp(broadcastEnable_5g, "on") == 0)
	{
	    prof5G->mBaseCf.SSIDBroadCastEn = FUN_DISABLE;
	}
	else
	{
	    prof5G->mBaseCf.SSIDBroadCastEn = FUN_ENABLE;
	}
	if (strcmp(pskPsswds_5g, "") == 0)
	{
	    strcpy(prof5G->mSafeCf.SelAuthMode, "OPEN");
	    strcpy(prof5G->mSafeCf.ap.AuthMode.EncrypType,"NONE");
	}
	else
	{
	    strcpy(prof5G->mSafeCf.SelAuthMode, "WPAPSKWPA2PSK");
	    strcpy(prof5G->mSafeCf.ap.AuthMode.pskPsswd, pskPsswds_5g);
	    if(strcmp(prof5G->mSafeCf.ap.AuthMode.EncrypType,"NONE") == 0)
	    {
		strcpy(prof5G->mSafeCf.ap.AuthMode.EncrypType,"TKIPAES");
	    }
	}
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
    }

out:
    uttSemV(SEM_WIFI_NO,0);
    websRedirect(wp,"index_login.asp");
}
#endif
#if (UTT_BRIDEG_MODE_FLAG == FYES)
static void Formchangemodel(webs_t wp, char_t *path, char_t *query)
{
    char *brctlname = NULL;
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0); 
    brctlname = websGetVar(wp, T("brctlname"), T(""));
    if(profSys != NULL)
    { 
        if(!strcmp(brctlname,"1"))
        {
            profSys->brideg_mode_flag = 1;
        }
        else
        {
            profSys->brideg_mode_flag = 0;
        }
    } 
    nvramWriteCommit();
    if (profSys->brideg_mode_flag == 1)
    {
            doSystem("route_to_bridge.sh");
    }else{
            doSystem("bridge_to_route.sh");
    }
}
#endif 
#if (APP_ELINKC == FYES)
static void aspOutPutElinkc(int eid, webs_t wp, int argc, char_t **argv)
{
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0); 
    websWrite(wp,"var elinkc_flag = %d;\n", profSys->elinkc_flag); 
}
static void FormElinkc(webs_t wp, char_t *path, char_t *query)
{
    char *elinkc_flag = NULL;
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0); 
    elinkc_flag = websGetVar(wp, T("elinkc_flag"), T(""));
    if(profSys != NULL)
    { 
        if(!strcmp(elinkc_flag,"1"))
        {
            profSys->elinkc_flag = 1;
        }
        else
        {
            profSys->elinkc_flag = 0;
        }
    } 
    nvramWriteCommit();
    system("elinkc.sh &");

} 


#endif
/*ÐÂ·ç¸ñUI  ÎÞÏßÉèÖÃ½»»¥*/
static void wirelessBasic_Sui(webs_t wp, char_t *path, char_t *query)
{
    int uttSemVflag = 0;
    char_t *DefaultKeyID, *Key1Str,  *Key2Str, *Key3Str,  *Key4Str, *secModeVal,*roamthre;
    char_t *keyFormat,*keylen1,*keylen2,*keylen3,*keylen4;
    char_t *security_mode,*keyUpdateCyc,*pskPsswds,*cipher;
    char_t *wirelessmode,*ssid, *sz11gChannel,*n_bandwidth,*radio,*broadcastEnable;
    char_t *txPower;
#if (WMM_ENABLE == FYES)
    char_t *wmmEnable = NULL;
#endif
    MibProfileType mibType = MIB_PROF_WIRELESS;
    struProfAlloc *profhead = NULL;
    WirelessProfile *prof= NULL;
    const char * ErrMsg = NULL;
    char_t *wdsEnable = NULL;/*wds¿ª¹Ø*/
    int apworkmode = 1;/*Ä¬ÈÏ¹¤×÷Ä£Ê½ÎªAP Mode*/
    char_t *bridgeSSID;/*¶Ô¶ËµÄSSID*/
    char_t *remoteMac;/*¶Ô¶ËµÄMACµØÖ·*/
    char_t *def_mac = "000000000000";/*Ä¬ÈÏµÄmacµØÖ·,Ö÷ÒªÓÃÓÚ¸øWds½á¹¹ÌåµÄÆäËûÈý¸ömac¸³Öµ*/
    char_t *wdswpapsk;/*WDS¿ªÆô£¬AES»òTKIP¼ÓÃÜÊ±µÄÔ¤¹²ÏíÃÜÔ¿*/
    char_t *wdsKeyFormat;/*WDS¿ªÆô£¬WEP¼ÓÃÜµÄÃØÔ¿¸ñÊ½*/
    char_t *wdsKeySerNo;/*WDS¿ªÆô£¬WEP¼ÓÃÜµÄÃØÔ¿ÐòºÅ*/
    char_t *wdsWepKey1,*wdsWepKey2,*wdsWepKey3,*wdsWepKey4;/*Ã¿¸öÐòºÅ¶ÔÓ¦µÄÃØÔ¿*/
    char_t *wdsWepKey1Len,*wdsWepKey2Len,*wdsWepKey3Len,*wdsWepKey4Len;/*Ã¿¸öWEPÃØÔ¿¶ÔÓ¦µÄ¼ÓÃÜ³¤¶È*/
    char_t *mode,*enc;
    char_t *strProfIndex;
    char_t *encodeType;/*ssid±àÂë·½Ê½£¬GB2312»òUTF-8*/
#if (WLAN_MULTI_SSID == FYES)
    char_t *ssid2;
    char_t *encodeType2;
#if (ADD_SSID == FYES)
    char_t *ssid3;
    char_t *ssid4;
    char_t *encodeType3;
    char_t *encodeType4;
#endif
#if (UTT_SSID_ISOLATION == FYES)
    char_t *ssidSeparate;
#endif
#if (UTT_SSID2_ENCRYPTION == FYES)
    char_t *DefaultKeyID_2, *Key1Str_2,  *Key2Str_2, *Key3Str_2,  *Key4Str_2;
    char_t *keyFormat_2,*keylen1_2,*keylen2_2,*keylen3_2,*keylen4_2;
    char_t *security_mode_2,*keyUpdateCyc_2,*pskPsswds_2,*cipher_2;
#endif
#endif
    int profIndex;
#if(WIRELESS_5G == FYES)
    char_t *n_vht_bandwidth;
    char_t *country;
    char_t *countryRegionABand;
#endif
#if (WLAN_MULTI_SSID == FYES)
    ssid2 = websGetVar(wp, T("ssid2"), T("")); 
#if (ADD_SSID == FYES)
    ssid3 = websGetVar(wp, T("ssid3"), T("")); 
    ssid4 = websGetVar(wp, T("ssid4"), T("")); 
#endif
#if (ADD_SSID == FYES)
    encodeType3 = websGetVar(wp, T("encodeType3"), T("UTF-8"));
    encodeType4 = websGetVar(wp, T("encodeType4"), T("UTF-8"));
#else
    encodeType2 = websGetVar(wp, T("encodeType2"), T("UTF-8"));
#endif
#if (UTT_SSID_ISOLATION == FYES)
    ssidSeparate = websGetVar(wp, T("ssidSeparate"), T("")); 
#endif
#endif
#if(MIDDLE_SOFT == FYES)
    encodeType = websGetVar(wp, T("encodeType"), T("UTF-8"));
#else
    encodeType = websGetVar(wp, T("encodeType"), T("GB2312"));
#endif
    /*»ñÈ¡ÊµÀýË÷Òý,0->2.4G,1->5G*/
    strProfIndex = websGetVar(wp, T("profIndex"), T("0"));
    profIndex = strtoul(strProfIndex, 0, 10);
    roamthre = websGetVar(wp, T("roamthre"), T(""));
    /*»ñÈ¡WDS¿ªÆôÊ±µÄ°²È«Ä£Ê½*/
    mode = websGetVar(wp, T("apcli_mode"), T("OPEN"));
    /* ¼ÓÃÜËã·¨         */
    enc = websGetVar(wp, T("apcli_enc"), T("NONE"));
    /*»ñÈ¡¶Ô¶Ëssid*/
    bridgeSSID = websGetVar(wp, T("bridgessid"), T(""));
    /*»ñÈ¡¶Ô¶ËmacµØÖ·*/
    remoteMac= websGetVar(wp, T("bridgebssid"), def_mac);
    /* ÆôÓÃÎÞÏß¹¦ÄÜ 2  */
    radio = websGetVar(wp, T("ebabledwifi"), T(""));
    /*ÊÇ·ñ¿ªÆôWDSÄ£Ê½*/
    wdsEnable = websGetVar(wp, T("wdsEnable"), T("0"));/*Ä¬ÈÏ¹Ø±Õ*/
    /*»ñÈ¡wds¿ªÆô£¬AES»òTKIP¼ÓÃÜÊ±µÄÔ¤¹²ÏíÃÜÔ¿*/
    wdswpapsk= websGetVar(wp,T("wdspskPsswd"),T(""));
    /*»ñÈ¡wds¿ªÆô£¬WEP¼ÓÃÜÊ±µÄÃÜÔ¿¸ñÊ½*/
    wdsKeyFormat = websGetVar(wp,T("wdsKeyFormat"),T("1"));
    /*»ñÈ¡wds¿ªÆô£¬WEP¼ÓÃÜÊ±µÄÃÜÔ¿ÐòºÅ*/
    wdsKeySerNo = websGetVar(wp,T("wdsKeySerNo"),T("0"));
    /*»ñÈ¡wds¿ªÆô£¬WEP¼ÓÃÜÊ±µÄ¸÷¸öÃØÔ¿¼°¼ÓÃÜ³¤¶È*/
    wdsWepKey1 = websGetVar(wp, T("wdsWepKey1"), T(""));
    wdsWepKey2 = websGetVar(wp, T("wdsWepKey2"), T(""));
    wdsWepKey3 = websGetVar(wp, T("wdsWepKey3"), T(""));
    wdsWepKey4 = websGetVar(wp, T("wdsWepKey4"), T(""));
    wdsWepKey1Len = websGetVar(wp, T("wdsWepKey1Len"), T(""));
    wdsWepKey2Len = websGetVar(wp, T("wdsWepKey2Len"), T(""));
    wdsWepKey3Len = websGetVar(wp, T("wdsWepKey3Len"), T(""));
    wdsWepKey4Len = websGetVar(wp, T("wdsWepKey4Len"), T(""));
    /*    ssid         */
    ssid = websGetVar(wp, T("ssid"), T("")); 
    /* ÎÞÏßÄ£Ê½  9: bgn mode  */
    wirelessmode = websGetVar(wp, T("wrlessMode"), T("")); 
    /* ÐÅµÀ */ 
    sz11gChannel = websGetVar(wp, T("channel"), T("")); 
    /* 0 ÆµµÀ´ø¿í */
    n_bandwidth = websGetVar(wp, T("chanWidth"), T(""));
    txPower = websGetVar(wp, T("txPower"), T(""));
#if(WIRELESS_5G == FYES)
    /* VHT ÆµµÀ´ø¿í*/
    n_vht_bandwidth = websGetVar(wp, T("vhtChanWidth"), T(""));
    country = websGetVar(wp, T("country"), T(""));
#ifdef CONFIG_USER_GOAHEAD_LANG_EN
    countryRegionABand = websGetVar(wp, T("countryRegionABand"), T("7"));
#else
    countryRegionABand = websGetVar(wp, T("countryRegionABand"), T("10"));
#endif
#endif
    /* ÆôÓÃSSID¹ã²¥ */
    broadcastEnable = websGetVar(wp, T("hidenet"), T("")); 
    security_mode = websGetVar(wp, T("security_mode"), T(""));
    secModeVal = websGetVar(wp, T("secMode"), T(""));
    Key1Str = websGetVar(wp, T("wepkey1"), T(""));
    Key2Str = websGetVar(wp, T("wepkey2"), T(""));
    Key3Str = websGetVar(wp, T("wepkey3"), T(""));
    Key4Str = websGetVar(wp, T("wepkey4"), T(""));
    DefaultKeyID = websGetVar(wp, T("keynum"), T(""));
    keyFormat = websGetVar(wp, T("keyFormat"), T(""));
    keylen1 = websGetVar(wp, T("keyLength1"), T(""));
    keylen2 = websGetVar(wp, T("keyLength2"), T(""));
    keylen3 = websGetVar(wp, T("keyLength3"), T(""));
    keylen4 = websGetVar(wp, T("keyLength4"), T(""));
    keyUpdateCyc = websGetVar(wp, T("keyRenewalInterval"), T("0"));
    pskPsswds = websGetVar(wp, T("pskPsswd"), T(""));
    cipher = websGetVar(wp, T("cipher"), T(""));
#if (UTT_SSID2_ENCRYPTION == FYES)
    security_mode_2 = websGetVar(wp, T("security_mode_2"), T(""));
    Key1Str_2 = websGetVar(wp, T("wepkey1_2"), T(""));
    Key2Str_2 = websGetVar(wp, T("wepkey2_2"), T(""));
    Key3Str_2 = websGetVar(wp, T("wepkey3_2"), T(""));
    Key4Str_2 = websGetVar(wp, T("wepkey4_2"), T(""));
    DefaultKeyID_2 = websGetVar(wp, T("keynum_2"), T(""));
    keyFormat_2 = websGetVar(wp, T("keyFormat_2"), T(""));
    keylen1_2 = websGetVar(wp, T("keyLength1_2"), T(""));
    keylen2_2 = websGetVar(wp, T("keyLength2_2"), T(""));
    keylen3_2 = websGetVar(wp, T("keyLength3_2"), T(""));
    keylen4_2 = websGetVar(wp, T("keyLength4_2"), T(""));
    pskPsswds_2 = websGetVar(wp, T("pskPsswd_2"), T(""));
    cipher_2 = websGetVar(wp, T("cipher_2"), T(""));
#endif

#if (WMM_ENABLE == FYES)
    wmmEnable = websGetVar(wp, T("wmmEnabled"), T("")); 
#endif
    uttSemP(SEM_WIFI_NO,0);
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, profIndex);
    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, profIndex, &profhead);
    clearRadiusSet(prof);

    //SSID, Multiple SSID
    if (0 == strlen(ssid)) {
        ErrMsg = getTransDataForC(C_LANG_INDEX_SSID_CANT_NULL);
        setTheErrorMsg(ErrMsg);
	goto out;
    }

    if (!strncmp(radio, "on", 3)) {
	prof->head.active = 1;
    }
    else{
	prof->head.active = 0;
    }
    strcpy(prof->mBaseCf.SSID1,ssid);
#if (WLAN_MULTI_SSID == FYES)
    strcpy(prof->mBaseCf.SSID2,ssid2);
#if (ADD_SSID == FYES)
    strcpy(prof->mBaseCf.SSID3,ssid3);
    strcpy(prof->mBaseCf.SSID4,ssid4);
#endif
#if (MAKE_MONEY == FYES)
        prof->mBaseCf.BssidNum = 4; 
#elif (ADD_SSID == FYES)
        prof->mBaseCf.BssidNum = 4; 
#else
        prof->mBaseCf.BssidNum = 2;
#endif
#endif
    if (strcmp(broadcastEnable, "on") == 0)
    {
        prof->mBaseCf.SSIDBroadCastEn = FUN_DISABLE;
    }
    else
    {
        prof->mBaseCf.SSIDBroadCastEn = FUN_ENABLE;
    }
#if (WMM_ENABLE == FYES)
    if(strcmp(wmmEnable,"on") == 0)
    {
        prof->advance.wmm = FUN_ENABLE;
    }else{
        prof->advance.wmm = FUN_DISABLE;
    }
#endif
    strncpy(prof->mBaseCf.encodeType,encodeType,7);/*±£´æssid¼ÓÃÜ·½Ê½*/
#if (WLAN_MULTI_SSID == FYES)
    strncpy(prof->mBaseCf.encodeType2,encodeType2,7);/*±£´æssid¼ÓÃÜ·½Ê½*/
#if (ADD_SSID == FYES)
    strncpy(prof->mBaseCf.encodeType3,encodeType3,7);/*±£´æssid¼ÓÃÜ·½Ê½*/
    strncpy(prof->mBaseCf.encodeType4,encodeType4,7);/*±£´æssid¼ÓÃÜ·½Ê½*/
#endif
#endif
    prof->mBaseCf.WirelessMode = strtoul(wirelessmode, 0, 10);
    if (!strncmp(wirelessmode, "4", 2) || !strncmp(wirelessmode, "7", 2)) {//g, gn
	prof->mBaseCf.BasicRate = 351;
	prof->mBaseCf.HT_TxStream = 1;
	prof->mBaseCf.HT_RxStream = 1;
    }
    else if (!strncmp(wirelessmode, "1", 2)) { //b
	prof->mBaseCf.BasicRate = 3;
	prof->mBaseCf.HT_TxStream = 2;
	prof->mBaseCf.HT_RxStream = 2;
    }
    else {//bg,bgn,n
	prof->mBaseCf.BasicRate = 15;
	prof->mBaseCf.HT_TxStream = 2;
	prof->mBaseCf.HT_RxStream = 2;
    }

#if (WIRELESS_5G == FYES)
/* 7612E xStream set 2 */
    if ( profIndex == 1) {
	prof->mBaseCf.HT_TxStream = UTT_XSTREAM_NUM_5G;
	prof->mBaseCf.HT_RxStream = UTT_XSTREAM_NUM_5G;
    }
#endif

    //11abg Channel or AutoSelect
    if (0 == strlen(sz11gChannel)) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_CHANNEL_CANT_NULL);
	setTheErrorMsg(ErrMsg);
	goto out;
    }
    if (!strncmp(sz11gChannel, "0", 2)){
#if (AUTO_CHANNEL == FYES)
        if( profIndex == 1 ){
#if (MSG_WR211 == FYES)
            prof->mBaseCf.CountryRegion = 4;
#else
            prof->mBaseCf.CountryRegion = 10;
#endif
        }else{
        prof->mBaseCf.CountryRegion = 0;
        }
#endif
        prof->mBaseCf.AutoChannelSelect = 2;
    }
    else {
#if (AUTO_CHANNEL == FYES)
    if( profIndex == 1 ){
        prof->mBaseCf.CountryRegion = 10;
    }else{
        prof->mBaseCf.CountryRegion = 1;
    }
#endif
    prof->mBaseCf.AutoChannelSelect = 0;
    }
    if (0 != strlen(sz11gChannel))
    {
	prof->mBaseCf.Channel = strtoul(sz11gChannel, 0, 10);
#if 0
	if (profIndex == 1) {
	    doSystem("iwpriv rai0 set Channel=%s", sz11gChannel);
	} else {
	    doSystem("iwpriv ra0 set Channel=%s", sz11gChannel);
	}
#endif
    }
    prof->mBaseCf.ChanWidth = strtoul(n_bandwidth,0,10);
    prof->mBaseCf.TxPower = strtoul(txPower,0,10);
#if(WIRELESS_5G == FYES)
    if (profIndex == 1) {
    prof->mBaseCf.VChanWidth = strtoul(n_vht_bandwidth,0,10);
    prof->mBaseCf.Country = strtoul(country,0,10);
    if (!strncmp(sz11gChannel, "0", 2)){
        prof->mBaseCf.CountryRegionABand = 4;
    }else{
        prof->mBaseCf.CountryRegionABand = strtoul(countryRegionABand,0,10);
    }
    }
#endif
    if( prof->mBaseCf.ChanWidth == 1)
    {/*20/40 auto*/
	prof->mBaseCf.HT_BSSCoexistence = 1;
    }
    else
    {
	prof->mBaseCf.HT_BSSCoexistence = 0;
    }
    if (prof->mBaseCf.ChanWidth == 2) {/*20m*/
	prof->mBaseCf.ChanWidth = 1;
    } 

    /*APµÄ°²È«Ä£Ê½ÉèÖÃ*/
    if( !strcmp(security_mode, "Disable")){			
	strcpy(prof->mSafeCf.SelAuthMode, "OPEN");
	strcpy(prof->mSafeCf.ap.AuthMode.EncrypType,"NONE");
	prof->mSafeCf.ap.AuthMode.IEEE8021X = 0;
    }else if( (!strcmp(security_mode, "OPEN")) ||  (!strcmp(security_mode, "SHARED")) || (!strcmp(security_mode, "WEPAUTO")) ){		
	/* !------------------       Open Mode , Shared Mode , WEPAUTO mode----------------*/
	strcpy(prof->mSafeCf.SelAuthMode, security_mode);
	strcpy(prof->mSafeCf.ap.AuthMode.EncrypType,"WEP");
    prof->mSafeCf.ap.AuthMode.IEEE8021X = 0;
	prof->mSafeCf.ap.AuthMode.wepKeyID= strtoul(DefaultKeyID, 0, 10);
	strcpy(prof->mSafeCf.ap.AuthMode.wepkey1, Key1Str);
	strcpy(prof->mSafeCf.ap.AuthMode.wepkey2, Key2Str);
	strcpy(prof->mSafeCf.ap.AuthMode.wepkey3, Key3Str);
	strcpy(prof->mSafeCf.ap.AuthMode.wepkey4, Key4Str);
	/*none write dat*/
    prof->mSafeCf.ap.AuthMode.wepkeyLength1 = strtoul(keylen1, 0, 10);
	prof->mSafeCf.ap.AuthMode.wepkeyLength2 = strtoul(keylen2, 0, 10);
	prof->mSafeCf.ap.AuthMode.wepkeyLength3 = strtoul(keylen3, 0, 10);
	prof->mSafeCf.ap.AuthMode.wepkeyLength4 = strtoul(keylen4, 0, 10);
	/*none write dat*/
    prof->mSafeCf.ap.AuthMode.wepKeyType = strtoul(keyFormat, 0, 10);
    }else if(!strcmp(security_mode, "WPA") || !strcmp(security_mode, "WPA1WPA2")||!strcmp(security_mode, "WPA2") ){	
	/* !------------------WPA Enterprise Mode ----------------*/
	/*set radius para	    WPA1WPA2 mixed mode */
	strcpy(prof->mSafeCf.SelAuthMode, security_mode);
	conf8021x(prof, wp);	
	strcpy(prof->mSafeCf.ap.AuthMode.EncrypType,cipher);
	prof->mSafeCf.ap.AuthMode.RekeyInterval = strtoul(keyUpdateCyc, 0, 10);
#if (UTT_SSID2_ENCRYPTION == FYES)
	/*±Â¹Üssid2²ÉÓÃÊ²Ã´¼ÓÃÜ·½Ê½£¬Í¬Ê±¸üÐÂÆä½á¹¹ÌåµÄ³ÉÔ±*/
	prof->mSafeCf.ap.AuthMode_2.RekeyInterval = prof->mSafeCf.ap.AuthMode.RekeyInterval;
#endif
	prof->mSafeCf.ap.AuthMode.IEEE8021X = 0;
    }else if( !strcmp(security_mode, "WPAPSK")||!strcmp(security_mode, "WPA2PSK") || !strcmp(security_mode, "WPAPSKWPA2PSK") ){
    strcpy(prof->mSafeCf.ap.AuthMode.EncrypType,cipher);
	prof->mSafeCf.ap.AuthMode.RekeyInterval = strtoul(keyUpdateCyc, 0, 10);
#if (UTT_SSID2_ENCRYPTION == FYES)
	/*±Â¹Üssid2²ÉÓÃÊ²Ã´¼ÓÃÜ·½Ê½£¬Í¬Ê±¸üÐÂÆä½á¹¹ÌåµÄ³ÉÔ±*/
	prof->mSafeCf.ap.AuthMode_2.RekeyInterval = prof->mSafeCf.ap.AuthMode.RekeyInterval;
#endif
	prof->mSafeCf.ap.AuthMode.IEEE8021X = 0;
	strcpy(prof->mSafeCf.ap.AuthMode.pskPsswd, pskPsswds);
	strcpy(prof->mSafeCf.SelAuthMode, security_mode);
    }
#if (UTT_WLAN_WAPI == FYES)
    else if(strcmp(security_mode, "WAPI") == 0)
    {
#if (UTT_SSID2_ENCRYPTION == FYES)
        /*±Â¹Üssid2²ÉÓÃÊ²Ã´¼ÓÃÜ·½Ê½£¬Í¬Ê±¸üÐÂÆä½á¹¹ÌåµÄ³ÉÔ±*/
        prof->mSafeCf.ap.AuthMode_2.RekeyInterval = prof->mSafeCf.ap.AuthMode.RekeyInterval;
#endif
            prof->mSafeCf.ap.AuthMode.IEEE8021X = 0;
        strcpy(prof->mSafeCf.ap.AuthMode.pskPsswd, pskPsswds);
        strcpy(prof->mSafeCf.SelAuthMode, security_mode);
    }
#endif
    else
    {; }
#if (UTT_SSID2_ENCRYPTION == FYES)
    /*SSID2 APµÄ°²È«Ä£Ê½ÉèÖÃ*/
    if(!strcmp(security_mode_2, "Disable")){			
	strcpy(prof->mSafeCf.SelAuthMode_2, "OPEN");
	strcpy(prof->mSafeCf.ap.AuthMode_2.EncrypType,"NONE");
	prof->mSafeCf.ap.AuthMode_2.IEEE8021X = 0;
    }else if((!strcmp(security_mode_2, "OPEN")) ||  (!strcmp(security_mode_2, "SHARED")) || (!strcmp(security_mode_2, "WEPAUTO")) ){		
	/* !------------------       Open Mode , Shared Mode , WEPAUTO mode----------------*/
	strcpy(prof->mSafeCf.SelAuthMode_2, security_mode_2);
	strcpy(prof->mSafeCf.ap.AuthMode_2.EncrypType,"WEP");
	prof->mSafeCf.ap.AuthMode_2.IEEE8021X = 0;
	prof->mSafeCf.ap.AuthMode_2.wepKeyID= strtoul(DefaultKeyID_2, 0, 10);
	strcpy(prof->mSafeCf.ap.AuthMode_2.wepkey1, Key1Str_2);
	strcpy(prof->mSafeCf.ap.AuthMode_2.wepkey2, Key2Str_2);
	strcpy(prof->mSafeCf.ap.AuthMode_2.wepkey3, Key3Str_2);
	strcpy(prof->mSafeCf.ap.AuthMode_2.wepkey4, Key4Str_2);
	prof->mSafeCf.ap.AuthMode_2.wepkeyLength1 = strtoul(keylen1_2, 0, 10);
	prof->mSafeCf.ap.AuthMode_2.wepkeyLength2 = strtoul(keylen2_2, 0, 10);
	prof->mSafeCf.ap.AuthMode_2.wepkeyLength3 = strtoul(keylen3_2, 0, 10);
	prof->mSafeCf.ap.AuthMode_2.wepkeyLength4 = strtoul(keylen4_2, 0, 10);
	prof->mSafeCf.ap.AuthMode_2.wepKeyType = strtoul(keyFormat_2, 0, 10);
    }else if(!strcmp(security_mode_2, "WPA") || !strcmp(security_mode_2, "WPA1WPA2")||!strcmp(security_mode_2, "WPA2") ){	
	/* !------------------WPA Enterprise Mode ----------------*/
	/*set radius para	    WPA1WPA2 mixed mode */
	strcpy(prof->mSafeCf.SelAuthMode_2, security_mode_2);
	conf8021x_2(prof, wp);	
	strcpy(prof->mSafeCf.ap.AuthMode_2.EncrypType,cipher_2);
	/*ssid2ºÍssid1¹«ÓÃÍ¬Ò»¸öÃØÔ¿¸üÐÂÖÜÆÚ*/
	prof->mSafeCf.ap.AuthMode_2.RekeyInterval = strtoul(keyUpdateCyc, 0, 10);
	/*Í¬Ñù£¬±Â¹Üssid1²ÉÓÃÊ²Ã´¼ÓÃÜ·½Ê½£¬Í¬Ê±¸üÐÂÆä½á¹¹ÌåµÄ³ÉÔ±*/
	prof->mSafeCf.ap.AuthMode.RekeyInterval = prof->mSafeCf.ap.AuthMode_2.RekeyInterval;
	prof->mSafeCf.ap.AuthMode_2.IEEE8021X = 0;
    }else if( !strcmp(security_mode_2, "WPAPSK")||!strcmp(security_mode_2, "WPA2PSK") || !strcmp(security_mode_2, "WPAPSKWPA2PSK") ){
	strcpy(prof->mSafeCf.ap.AuthMode_2.EncrypType,cipher_2);
	prof->mSafeCf.ap.AuthMode_2.IEEE8021X = 0;
	/*ssid2ºÍssid1¹«ÓÃÍ¬Ò»¸öÃØÔ¿¸üÐÂÖÜÆÚ*/
	prof->mSafeCf.ap.AuthMode_2.RekeyInterval = strtoul(keyUpdateCyc, 0, 10);
	/*Í¬Ñù£¬±Â¹Üssid1²ÉÓÃÊ²Ã´¼ÓÃÜ·½Ê½£¬Í¬Ê±¸üÐÂÆä½á¹¹ÌåµÄ³ÉÔ±*/
	prof->mSafeCf.ap.AuthMode.RekeyInterval = prof->mSafeCf.ap.AuthMode_2.RekeyInterval;
	strcpy(prof->mSafeCf.ap.AuthMode_2.pskPsswd, pskPsswds_2);
	strcpy(prof->mSafeCf.SelAuthMode_2, security_mode_2);
    }
#endif
    /*WDS°²È«Ä£Ê½ÉèÖÃ*/
    if (strncmp(wdsEnable,"1",2) == 0) {/*¿ªÆôWDS*/
	apworkmode = 5;/*WDSÊÇ¹¤×÷Ä£Ê½5,Í¨¹ýAPClient ModeÊµÏÖWDS*/
	prof->mBaseCf.apClient.apclientEnable = 1;
	prof->mBaseCf.OperationMode = 3;
	strncpy(prof->mBaseCf.apClient.wdsSwitch,"on",3);
	strcpy(prof->mBaseCf.apClient.ApCliSsid, bridgeSSID);
	converMac12To6Byte(remoteMac, &(prof->mBaseCf.apClient.APCliMacs));
	strcpy(prof->mBaseCf.SelAuthMode, mode);
	strcpy(prof->mBaseCf.apClient.AuthMode.EncrypType, enc);
	strcpy(prof->mBaseCf.apClient.AuthMode.pskPsswd, wdswpapsk);
	prof->mBaseCf.apClient.AuthMode.wepKeyID= strtoul(wdsKeySerNo, 0, 10);
	strcpy(prof->mBaseCf.apClient.AuthMode.wepkey1, wdsWepKey1);
	strcpy(prof->mBaseCf.apClient.AuthMode.wepkey2, wdsWepKey2);
	strcpy(prof->mBaseCf.apClient.AuthMode.wepkey3, wdsWepKey3);
	strcpy(prof->mBaseCf.apClient.AuthMode.wepkey4, wdsWepKey4);
	prof->mBaseCf.apClient.AuthMode.wepkeyLength1 = strtoul(wdsWepKey1Len, 0, 10);
	prof->mBaseCf.apClient.AuthMode.wepkeyLength2 = strtoul(wdsWepKey2Len, 0, 10);
	prof->mBaseCf.apClient.AuthMode.wepkeyLength3 = strtoul(wdsWepKey3Len, 0, 10);
	prof->mBaseCf.apClient.AuthMode.wepkeyLength4 = strtoul(wdsWepKey4Len, 0, 10);
	prof->mBaseCf.apClient.AuthMode.wepKeyType = strtoul(wdsKeyFormat, 0, 10);
    } else {
	prof->mBaseCf.apClient.apclientEnable = 0;
	prof->mBaseCf.OperationMode  = 1;
	strncpy(prof->mBaseCf.apClient.wdsSwitch,"off",4);
    }
    prof->mBaseCf.apWorkMode = apworkmode;
    prof->mBaseCf.apWds.wdsEnable = apworkmode;
#if (UTT_SSID_ISOLATION == FYES)
    prof->mBaseCf.ssidSeparate = strtoul(ssidSeparate, 0 ,10);
#endif

    uttSemV(SEM_WIFI_NO,0);
    uttSemVflag = 1;
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
    if (uttSemVflag == 0) 
    {
	uttSemV(SEM_WIFI_NO,0);
    }
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
#endif
    if (profIndex == 0) {
	websRedirect(wp,"WirelessSeting_Sui.asp");
    } else {
	websRedirect(wp,"WirelessSeting5g_Sui.asp");
    }
}

#endif

static void wirelessBasic(webs_t wp, char_t *path, char_t *query)
{
    char_t *wirelessmode,*apworkmode,*ssid, /* *hssid,*/*apmac,*apssid,*sz11gChannel,*n_bandwidth,*radio,*broadcastEnable ,*wpsmac0,*wpsmac1,*wpsmac2,*wpsmac3,*WdsList;
    char *def_mac = "000000000000";
    int oldworkmode;
#if 0
    char	 hidden_ssid[16];
#endif
#if(WLAN_MULTI_SSID == FYES)
char_t *encodeType;
char_t *ssid2;
char_t *encodeType2;
#endif
    int is_n=0;
    char_t *bridgeSSID;
    bridgeSSID = websGetVar(wp, T("bridgessid"), T(""));
    MibProfileType mibType = MIB_PROF_WIRELESS;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    WirelessProfile *prof= NULL;
    const char * ErrMsg = NULL;
    /* ÆôÓÃÎÞÏß¹¦ÄÜ 2  */
    radio = websGetVar(wp, T("WrlessEnable"), T(""));
    /* AP¹¤×÷Ä£Ê½      */
    apworkmode = websGetVar(wp, T("APWorkMode"), T(""));
    /*    ssid         */
    ssid = websGetVar(wp, T("ssid"), T("")); 
#if(WLAN_MULTI_SSID == FYES)
    ssid2 = websGetVar(wp, T("ssid2"), T(""));
    encodeType2 = websGetVar(wp, T("encodeType2"), T("")); 
    encodeType = websGetVar(wp, T("encodeType"), T(""));
#endif

    /* ÎÞÏßÄ£Ê½  9: bgn mode  */
    wirelessmode = websGetVar(wp, T("wrlessMode"), T("")); 
    /* ÐÅµÀ */ 
    sz11gChannel = websGetVar(wp, T("channel"), T("")); 
    /* 0 ÆµµÀ´ø¿í */
    n_bandwidth = websGetVar(wp, T("chanWidth"), T(""));
    /* ÆôÓÃSSID¹ã²¥ */
    broadcastEnable = websGetVar(wp, T("broadcastEnable"), T("")); 
    /* APµÄSSID    */
    apssid=websGetVar(wp, T("APssid"), T(""));
    /* APclient µÄAP MAC */
    apmac=websGetVar(wp, T("APCliMacs"), T(""));
    /* WDSµÄAP MAC */
    wpsmac0=websGetVar(wp, T("APMac0"), def_mac);
    wpsmac1=websGetVar(wp, T("APMac1"), def_mac);
    wpsmac2=websGetVar(wp, T("APMac2"), def_mac);
    wpsmac3=websGetVar(wp, T("APMac3"), def_mac);
    if (strlen(wpsmac1) == 0) { wpsmac1 = def_mac; }
    if (strlen(wpsmac2) == 0) { wpsmac2 = def_mac; }
    if (strlen(wpsmac3) == 0) { wpsmac3 = def_mac; }

    /*WDSµÄMACLIST */
    WdsList=websGetVar(wp, T("wds_list"), T(""));
    /* °²È«Ä£Ê½   */
    char_t *mode, *enc, *wpapsk, *keyid, *keytype,
	   *key1, *key2, *key3, *key4,*keylen1,*keylen2,*keylen3,*keylen4,*wdswpapsk;
    mode = websGetVar(wp, T("apcli_mode"), T("OPEN"));
    /* ¼ÓÃÜËã·¨         */
    enc = websGetVar(wp, T("apcli_enc"), T("NONE"));
    /*  Ô¤¹²ÏíÃÜÔ¿      */
    wpapsk = websGetVar(wp, T("apclipskPsswd"), T("12345678"));
    /*  WDSÔ¤¹²ÏíÃÜÔ¿  */
    wdswpapsk = websGetVar(wp, T("wdspskPsswd"), T(""));
    /*ÃÜÔ¿ÐòºÅ  */
    keyid = websGetVar(wp, T("apclikeynum"), T("1"));
    /*ÃÜÔ¿¸ñÊ½ */
    keytype = websGetVar(wp, T("apclikeyFormat"), T("1"));
    /* ÃÜÔ¿1-4µÄÖµ         */
    key1 = websGetVar(wp, T("apcliwepkey1"), T(""));
    key2 = websGetVar(wp, T("apcliwepkey2"), T(""));
    key3 = websGetVar(wp, T("apcliwepkey3"), T(""));
    key4 = websGetVar(wp, T("apcliwepkey4"), T(""));

    keylen1=websGetVar(wp, T("apclikeyLength1"), T(""));
    keylen2=websGetVar(wp, T("apclikeyLength2"), T(""));
    keylen3=websGetVar(wp, T("apclikeyLength3"), T(""));
    keylen4=websGetVar(wp, T("apclikeyLength4"), T(""));

    //secmode end
    //strcpy(mac,apmac);
    //Conversion2StdMac(mac);
    //printf("radio=%s,wirelessmode=%s,ssid=%s,broadcastEnable=%s,sz11gChannel=%s,n_bandwidtw=%s\napmac=%s wpsmac0 = %s,wpsmac1 = %s,wpsmac2 = %s,wpsmac3 = %s\n mode = %s enc = %s\n",radio,wirelessmode,ssid,broadcastEnable,sz11gChannel,n_bandwidth,apmac,wpsmac0,wpsmac1,wpsmac2,wpsmac3,mode,enc);


    uttSemP(SEM_WIFI_NO,0);
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 0);
    profAction = PROF_CHANGE_EDIT;
    ProfBackupByIndex(mibType, profAction, 0, &profhead);

    if (!strncmp(radio, "on", 3)) {
	//doSystem("iwpriv ra0 set RadioOn=1");
	//nvram_bufset(RT2860_NVRAM, "RadioOff", "0");
	prof->head.active = 1;
    }
    else{
	//doSystem("iwpriv ra0 set RadioOn=0");
	//nvram_bufset(RT2860_NVRAM, "RadioOff", "1");
	prof->head.active = 0;
    }
    //nvram_bufset(RT2860_NVRAM, "WirelessMode", wirelessmode);
    oldworkmode = prof->mBaseCf.apWorkMode;
    prof->mBaseCf.WirelessMode = strtoul(wirelessmode, 0, 10);

    //BasicRate: bg,bgn,n:15, b:3; g,gn:351
    if (!strncmp(wirelessmode, "4", 2) || !strncmp(wirelessmode, "7", 2)) {//g, gn
	//nvram_bufset(RT2860_NVRAM, "BasicRate", "351");
	prof->mBaseCf.BasicRate = 351;
	prof->mBaseCf.HT_TxStream = 1;
	prof->mBaseCf.HT_RxStream = 1;
    }
    else if (!strncmp(wirelessmode, "1", 2)) { //b
	//nvram_bufset(RT2860_NVRAM, "BasicRate", "3");
	prof->mBaseCf.BasicRate = 3;
	prof->mBaseCf.HT_TxStream = 2;
	prof->mBaseCf.HT_RxStream = 2;
    }
    else {//bg,bgn,n
	prof->mBaseCf.BasicRate = 15;
	prof->mBaseCf.HT_TxStream = 2;
	prof->mBaseCf.HT_RxStream = 2;
	//nvram_bufset(RT2860_NVRAM, "BasicRate", "15");
    }

    if (!strncmp(wirelessmode, "6", 2) || !strncmp(wirelessmode, "9", 2)) {
	is_n = 1;
    }
    //SSID, Multiple SSID
    if (0 == strlen(ssid)) {
        ErrMsg = getTransDataForC(C_LANG_INDEX_SSID_CANT_NULL);
        setTheErrorMsg(ErrMsg);
        goto out;
    }
    //nvram_bufset(RT2860_NVRAM, "SSID1", ssid);
    strcpy(prof->mBaseCf.SSID1,ssid);
#if(WLAN_MULTI_SSID == FYES)
    strcpy(prof->mBaseCf.SSID2, ssid2);
#if (MAKE_MONEY == FYES)
        prof->mBaseCf.BssidNum = 4; 
#else
        prof->mBaseCf.BssidNum = 2;
#endif
#endif
#if(WLAN_MULTI_SSID == FYES)
    strncpy(prof->mBaseCf.encodeType,encodeType,7);/*±£´æssid¼ÓÃÜ·½Ê½*/
    strncpy(prof->mBaseCf.encodeType2,encodeType2,7);/*±£´æssid¼ÓÃÜ·½Ê½*/
#endif




#if 0
    if (strchr(hssid, '0') != NULL) {
	sprintf(hidden_ssid, "%s", "1");
    }
    else {
	sprintf(hidden_ssid, "%s", "0");
    }
#endif

    //11abg Channel or AutoSelect
    if (0 == strlen(sz11gChannel)) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_CHANNEL_CANT_NULL);
	setTheErrorMsg(ErrMsg);
	goto out;
    }
    if (!strncmp(sz11gChannel, "0", 2)){
	//nvram_bufset(RT2860_NVRAM, "AutoChannelSelect", "1");
	prof->mBaseCf.AutoChannelSelect = 2;
    }
    else {
	//nvram_bufset(RT2860_NVRAM, "AutoChannelSelect", "0");
	prof->mBaseCf.AutoChannelSelect = 0;
    }
    if (0 != strlen(sz11gChannel))
    {
	//nvram_bufset(RT2860_NVRAM, "Channel", sz11gChannel);
	prof->mBaseCf.Channel = strtoul(sz11gChannel, 0, 10);
	doSystem("iwpriv ra0 set Channel=%s", sz11gChannel);
    }

    //	if (is_n) 
#if 0
    {
	nvram_bufset(RT2860_NVRAM, "HT_BW", n_bandwidth);
    }
#endif
    prof->mBaseCf.ChanWidth = strtoul(n_bandwidth,0,10);
    if( prof->mBaseCf.ChanWidth == 1)
    {/*20/40 auto*/
	prof->mBaseCf.HT_BSSCoexistence = 1;
    }
    else
    {
	prof->mBaseCf.HT_BSSCoexistence = 0;
    }
    if (prof->mBaseCf.ChanWidth == 2) {/*20m*/
	prof->mBaseCf.ChanWidth = 1;
    } 
    //ap workmode
    int deleteWan4Flag=0;
    if( (strcmp(apworkmode, "5") !=0) && oldworkmode == 5 ) {
	deleteWan4Flag=1;
    }
    if (!strncmp(apworkmode, "1", 2)) {//ap
	//nvram_bufset(RT2860_NVRAM, "HideSSID", hidden_ssid);//8Î»or 1Î»?
	//nvram_bufset(RT2860_NVRAM, "WdsEnable", T("0"));
	prof->mBaseCf.apClient.apclientEnable = 0;
	prof->mBaseCf.OperationMode  = 1;
	prof->mBaseCf.SSIDBroadCastEn = strtoul(broadcastEnable, 0 ,10);
    }
    else if(!strncmp(apworkmode, "5", 2)){//apclient
	prof->mBaseCf.apClient.apclientEnable = 1;
	prof->mBaseCf.OperationMode = 3;
	strcpy(prof->mBaseCf.apClient.ApCliSsid, apssid);
	prof->mBaseCf.SSIDBroadCastEn = strtoul(broadcastEnable, 0 ,10);
	converMac12To6Byte(apmac, &(prof->mBaseCf.apClient.APCliMacs));
	strcpy(prof->mBaseCf.SelAuthMode, mode);
	strcpy(prof->mBaseCf.apClient.AuthMode.EncrypType, enc);
	strcpy(prof->mBaseCf.apClient.AuthMode.pskPsswd, wpapsk);
	prof->mBaseCf.apClient.AuthMode.wepKeyID= strtoul(keyid, 0, 10);
	strcpy(prof->mBaseCf.apClient.AuthMode.wepkey1, key1);
	strcpy(prof->mBaseCf.apClient.AuthMode.wepkey2, key2);
	strcpy(prof->mBaseCf.apClient.AuthMode.wepkey3, key3);
	strcpy(prof->mBaseCf.apClient.AuthMode.wepkey4, key4);
	prof->mBaseCf.apClient.AuthMode.wepkeyLength1 = strtoul(keylen1, 0, 10);
	prof->mBaseCf.apClient.AuthMode.wepkeyLength2 = strtoul(keylen2, 0, 10);
	prof->mBaseCf.apClient.AuthMode.wepkeyLength3 = strtoul(keylen3, 0, 10);
	prof->mBaseCf.apClient.AuthMode.wepkeyLength4 = strtoul(keylen4, 0, 10);
	prof->mBaseCf.apClient.AuthMode.wepKeyType = strtoul(keytype, 0, 10);
#if 0
	//nvram_bufset(RT2860_NVRAM, "ApCliSsid", apssid);
	//nvram_bufset(RT2860_NVRAM, "ApCliEnable", "1");
	//nvram_bufset(RT2860_NVRAM, "OperationMode", T("3"));//change the opmode to apcli
	//nvram_bufset(RT2860_NVRAM, "HideSSID", hidden_ssid);
	//nvram_bufset(RT2860_NVRAM, "ApCliBssid", apmac);
	//nvram_bufset(RT2860_NVRAM, "WdsEnable", T("0"));
	//nvram_bufset(RT2860_NVRAM, "ApCliAuthMode", mode);
	nvram_bufset(RT2860_NVRAM, "ApCliEncrypType", enc);
	nvram_bufset(RT2860_NVRAM, "ApCliWPAPSK", wpapsk);
	nvram_bufset(RT2860_NVRAM, "ApCliDefaultKeyId", keyid);
	nvram_bufset(RT2860_NVRAM, "ApCliKey1Type", keytype);
	nvram_bufset(RT2860_NVRAM, "ApCliKey1Str", key1);
	nvram_bufset(RT2860_NVRAM, "ApCliKey1Len", keylen1);
	nvram_bufset(RT2860_NVRAM, "ApCliKey2Type", keytype);
	nvram_bufset(RT2860_NVRAM, "ApCliKey2Str", key2);
	nvram_bufset(RT2860_NVRAM, "ApCliKey2Len", keylen2);
	nvram_bufset(RT2860_NVRAM, "ApCliKey3Type", keytype);
	nvram_bufset(RT2860_NVRAM, "ApCliKey3Str", key3);
	nvram_bufset(RT2860_NVRAM, "ApCliKey3Len", keylen3);
	nvram_bufset(RT2860_NVRAM, "ApCliKey4Type", keytype);
	nvram_bufset(RT2860_NVRAM, "ApCliKey4Str", key4);
	nvram_bufset(RT2860_NVRAM, "ApCliKey4Len", keylen4);
#endif
    }
    else if(!strncmp(apworkmode, "3", 2)){//repeater
	if (*bridgeSSID == '\0') {
	    memset(prof->mBaseCf.apWds.bridgeSSID1,0,sizeof(prof->mBaseCf.apWds.bridgeSSID1));
	} else {
	    strcpy(prof->mBaseCf.apWds.bridgeSSID1,bridgeSSID);
	}
	prof->mBaseCf.apClient.apclientEnable = 0;
	prof->mBaseCf.OperationMode  = 1;
	prof->mBaseCf.SSIDBroadCastEn = strtoul(broadcastEnable, 0 ,10);
	converMac12To6Byte(wpsmac0,&(prof->mBaseCf.apWds.WPSMacs0));
	converMac12To6Byte(wpsmac1,&(prof->mBaseCf.apWds.WPSMacs1));
	converMac12To6Byte(wpsmac2,&(prof->mBaseCf.apWds.WPSMacs2));
	converMac12To6Byte(wpsmac3,&(prof->mBaseCf.apWds.WPSMacs3));
	strcpy(prof->mBaseCf.apWds.WdsList,WdsList);
	//nvram_bufset(RT2860_NVRAM, "WdsEnable", apworkmode);
	//nvram_bufset(RT2860_NVRAM, "WdsList", apmac);
	//nvram_bufset(RT2860_NVRAM, "HideSSID", hidden_ssid);//8Î»or 1Î»?
    }
    else if(!strncmp(apworkmode, "2", 2)){//p-t-p /bridge
	//nvram_bufset(RT2860_NVRAM, "WdsEnable", apworkmode);
	//nvram_bufset(RT2860_NVRAM, "WdsList", apmac);
	strcpy(prof->mBaseCf.apWds.WdsList,WdsList);
	prof->mBaseCf.apClient.apclientEnable = 0;
	prof->mBaseCf.OperationMode  = 1;
	converMac12To6Byte(wpsmac0,&(prof->mBaseCf.apWds.WPSMacs0));
	converMac12To6Byte(wpsmac1,&(prof->mBaseCf.apWds.WPSMacs1));
	converMac12To6Byte(wpsmac2,&(prof->mBaseCf.apWds.WPSMacs2));
	converMac12To6Byte(wpsmac3,&(prof->mBaseCf.apWds.WPSMacs3));
    }
    else{//p-t-mp/azy mode
	//nvram_bufset(RT2860_NVRAM, "WdsEnable", apworkmode);
	//nvram_bufset(RT2860_NVRAM, "HideSSID", hidden_ssid);//8Î»or 1Î»?
	prof->mBaseCf.apClient.apclientEnable = 0;
	prof->mBaseCf.OperationMode  = 1;
	prof->mBaseCf.SSIDBroadCastEn = strtoul(broadcastEnable, 0 ,10);
    }
#if 0
    if (strncmp(apworkmode, "5", 2)) {
	//nvram_bufset(RT2860_NVRAM, "ApCliEnable", "0");
	//nvram_bufset(RT2860_NVRAM, "OperationMode", T("1"));//change the opmode to gateway
    }
#endif
    /*  WDSµÄ°²È«Ä£Ê½²ÎÊý */
    if(!strncmp(apworkmode, "2", 2)||!strncmp(apworkmode, "3", 2)||!strncmp(apworkmode, "4", 2)){//secmode
	if(!strncmp(mode,"TKIP",5)||!strncmp(mode,"AES",4)){
	    strcpy(prof->mBaseCf.apWds.AuthMode.pskPsswd,wdswpapsk); 
	    prof->mBaseCf.apWds.WdsPhyMode = 1;
	    //nvram_bufset(RT2860_NVRAM, "WdsKey", wdswpapsk);
	    //nvram_bufset(RT2860_NVRAM, "WdsPhyMode", "1");
	}
	else if(!strncmp(mode,"WEP",4)){
	    /*  ÎÞÏß°²È«ÅäÖÃºÍ»ù±¾ÅäÖÃµÄWDSµÄ°²È«ÅäÖÃ´æ´¢ÔÚÍ¬Ò»½á¹¹ÌåÖÐ   */
	    prof->mBaseCf.apWds.AuthMode.wepKeyID= strtoul(keyid, 0, 10);
	    strcpy(prof->mBaseCf.apWds.AuthMode.wepkey1, key1);
	    strcpy(prof->mBaseCf.apWds.AuthMode.wepkey2, key2);
	    strcpy(prof->mBaseCf.apWds.AuthMode.wepkey3, key3);
	    strcpy(prof->mBaseCf.apWds.AuthMode.wepkey4, key4);
	    prof->mBaseCf.apWds.AuthMode.wepkeyLength1 = strtoul(keylen1, 0, 10);
	    prof->mBaseCf.apWds.AuthMode.wepkeyLength2 = strtoul(keylen2, 0, 10);
	    prof->mBaseCf.apWds.AuthMode.wepkeyLength3 = strtoul(keylen3, 0, 10);
	    prof->mBaseCf.apWds.AuthMode.wepkeyLength4 = strtoul(keylen4, 0, 10);
	    prof->mBaseCf.apWds.AuthMode.wepKeyType = strtoul(keytype, 0, 10);
	    /* add by ydh Ôö¼ÓEncrypType£¬AuthModeÅäÖÃ,²ÎÕÕ520W */
	    strcpy(prof->mBaseCf.apWds.AuthMode.EncrypType, "WEP");
//	    strcpy(prof->mSafeCf.SelAuthMode, "WEPAUTO");
	    //nvram_bufset(RT2860_NVRAM, "DefaultKeyID", keyid);
	    //nvram_bufset(RT2860_NVRAM, "Key1Str1", key1);
	    //nvram_bufset(RT2860_NVRAM, "Key2Str1", key2);
	    //nvram_bufset(RT2860_NVRAM, "Key3Str1", key3);
	    //nvram_bufset(RT2860_NVRAM, "Key4Str1", key4);
	    //nvram_bufset(RT2860_NVRAM, "Key1Type", keytype);
	    //nvram_bufset(RT2860_NVRAM, "Key2Type", keytype);
	    //nvram_bufset(RT2860_NVRAM, "Key3Type", keytype);
	    //nvram_bufset(RT2860_NVRAM, "Key4Type", keytype);
	    //nvram_bufset(RT2860_NVRAM, "Key1Len", keylen1);
	    //nvram_bufset(RT2860_NVRAM, "Key2Len", keylen2);
	    //nvram_bufset(RT2860_NVRAM, "Key3Len", keylen3);
	    //nvram_bufset(RT2860_NVRAM, "Key4Len", keylen4);

	}
	//nvram_bufset(RT2860_NVRAM, "WdsEncrypType", mode); //Î´±£´æ
	/* mantis 8856, ÎÞÏß°²È«ÅäÖÃºÍwds°²È«ÅäÖÃ²»Í¬ */
	strncpy(prof->mBaseCf.apWds.wdsEncrypType, mode, UTT_WDS_MODE_LEN);
    }
    prof->mBaseCf.apWorkMode = strtoul(apworkmode,0,10);
    prof->mBaseCf.apWds.wdsEnable = strtoul(apworkmode, 0, 10);
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
#if (WLAN_WORK_TIME == FYES)
    editWlanTimeConfig(wp, path, query, 0);
#endif
    nvramWriteCommit();

#if 0
    initWlan();
    if (deleteWan4Flag == 1 ) {
	killDownWan(4);
    }
#endif
out:
    uttSemV(SEM_WIFI_NO,0);

#if 0
/*ÎªÁËÒ³ÃæÏìÓ¦ËÙ¶È¡£ÕâÀïÏÈ²»Ìí¼Ó*/
    /*12771 : ¸ÕÅäÖÃÒ³Ãæºó ÎÞÏß½Ó¿Ú¿ÉÄÜ»¹Î´ÆðÀ´
     * getWlanCurrentMac ÀïÒ²Ðè±£Ö¤¡£ÒòÎª ÕâÀï²»ÄÜË¯ÃßÌ«³¤
     * Í¬Ê±ÕâÀïÒ²²»ÄÜÉ¾³ýsleep
     * ÒòÎªÈôÁ½¸öÈËÍ¬Ê± ²Ù×÷Ò³Ãæ  Ôò¿ÉÄÜgetWlanCurrentMac»á±»Ìø¹ý
     *
     * Òª±£Ö¤×î³£ÓÃµÄÊ¹ÓÃÇé¿öÒ³Ãæ·´Ó³ËÙ¶È ±È½Ï¿ì ÇÒÃ»ÎÊÌâ*/
    sleep(3);
#endif

    websRedirect(wp,"WlanBase.asp");

}
#if 0
static void getWirelessBaseInfo(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL,*prof_5g = NULL;
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,0);
    if(prof == NULL){
        return 0;
    }
     if (prof->head.active == 1) {
        websWrite(wp,"var WrlessEnables=\"%s\";",T("on"));
    } else{
        websWrite(wp,"var WrlessEnables=\"%s\";",T("off"));
    }
    websWrite(wp,"var ssids=\"%s\";",prof->mBaseCf.SSID1);
    #if(WLAN_MULTI_SSID == FYES)
    websWrite(wp,"var encodeType=\"%s\";",prof->mBaseCf.encodeType);
    websWrite(wp,"var ssid2s=\"%s\";",prof->mBaseCf.SSID2);
    websWrite(wp,"var encodeType2=\"%s\";",prof->mBaseCf.encodeType2);
#endif
    websWrite(wp,"var APWorkModes=\"%d\";",prof->mBaseCf.apWorkMode);
#if (WLAN_MULTI_SSID == FYES)
     websWrite(wp,"var multi_ssid = 1;");
#else
     websWrite(wp,"var multi_ssid = 0;");
#endif
     websWrite(wp,"var wrlessModes=\"%d\";",prof->mBaseCf.WirelessMode);
    websWrite(wp,"var broadcastEnables=\"%d\";",prof->mBaseCf.SSIDBroadCastEn);
    websWrite(wp,"var channels=\"%d\";",prof->mBaseCf.Channel);
        if (prof->mBaseCf.ChanWidth == 1) {
                if (prof->mBaseCf.HT_BSSCoexistence == 0) {
                        websWrite(wp,"var chanWidths=\"%d\";\n", 2);
                } else {
                        websWrite(wp,"var chanWidths=\"%d\";\n", 1);
                }
        } else {
                websWrite(wp,"var chanWidths=\"%d\";\n", 0);
        }    
#if(WIRELESS_5G == FYES)
    prof_5g = (WirelessProfile*)ProfGetInstPointByIndex(mibType,1);
    if(prof_5g == NULL){
        return 0;
    }
     if (prof_5g->head.active == 1) {
        websWrite(wp,"var WrlessEnables_5g=\"%s\";",T("on"));
    } else{
        websWrite(wp,"var WrlessEnables_5g=\"%s\";",T("off"));
    }
    websWrite(wp,"var ssids_5g=\"%s\";",prof_5g->mBaseCf.SSID1);
    #if(WLAN_MULTI_SSID == FYES)
    websWrite(wp,"var encodeType_5g=\"%s\";",prof_5g->mBaseCf.encodeType);
    websWrite(wp,"var ssid2s_5g=\"%s\";",prof_5g->mBaseCf.SSID2);
    websWrite(wp,"var encodeType2_5g=\"%s\";",prof_5g->mBaseCf.encodeType2);
#endif
    websWrite(wp,"var APWorkModes_5g=\"%d\";",prof_5g->mBaseCf.apWorkMode);
#if (WLAN_MULTI_SSID == FYES)
     websWrite(wp,"var multi_ssid_5g = 1;");
#else
     websWrite(wp,"var multi_ssid_5g = 0;");
#endif
     websWrite(wp,"var wrlessModes_5g=\"%d\";",prof_5g->mBaseCf.WirelessMode);
    websWrite(wp,"var broadcastEnables_5g=\"%d\";",prof_5g->mBaseCf.SSIDBroadCastEn);
    websWrite(wp,"var channels_5g=\"%d\";",prof_5g->mBaseCf.Channel);
        if (prof_5g->mBaseCf.ChanWidth == 1) {
                if (prof_5g->mBaseCf.HT_BSSCoexistence == 0) {
                        websWrite(wp,"var chanWidths_5g=\"%d\";\n", 2);
                } else {
                        websWrite(wp,"var chanWidths_5g=\"%d\";\n", 1);
                }
        } else {
                websWrite(wp,"var chanWidths_5g=\"%d\";\n", 0);
        }    

#endif
}
#endif

static int getWirelessBaseConfig(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;
#if (WLAN_WORK_TIME == FYES)
    UINT32 daysTemp = 0U;
    char_t data[16];
#endif
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,0);
    if(prof == NULL){
	return 0;
    }


    /* È«¾ÖÊä³ö */
#if(BRAND_LEVEL_ONE == FYES||BRAND_INDIA == FYES)
    websWrite(wp,"var Ctrlchannel=1;");
#else
    websWrite(wp,"var Ctrlchannel=0;");
#endif
    if (prof->head.active == 1) {
	websWrite(wp,"var WrlessEnables=\"%s\";",T("on"));
    } else{
	websWrite(wp,"var WrlessEnables=\"%s\";",T("off"));
    }
    websWrite(wp,"var ssids=\"%s\";",prof->mBaseCf.SSID1);
#if(WLAN_MULTI_SSID == FYES)
    websWrite(wp,"var encodeType=\"%s\";",prof->mBaseCf.encodeType); 
    websWrite(wp,"var ssid2s=\"%s\";",prof->mBaseCf.SSID2);
#if (ADD_SSID == FYES)
    websWrite(wp,"var ssid3s=\"%s\";",prof->mBaseCf.SSID3);
    websWrite(wp,"var ssid4s=\"%s\";",prof->mBaseCf.SSID4);
#endif
    websWrite(wp,"var encodeType2=\"%s\";",prof->mBaseCf.encodeType2);
#if (ADD_SSID == FYES)
    websWrite(wp,"var encodeType3=\"%s\";",prof->mBaseCf.encodeType3);
    websWrite(wp,"var encodeType4=\"%s\";",prof->mBaseCf.encodeType4);
#endif
#endif
    websWrite(wp,"var APWorkModes=\"%d\";",prof->mBaseCf.apWorkMode);
#if (WLAN_MULTI_SSID == FYES)
     websWrite(wp,"var multi_ssid = 1;");
#else
     websWrite(wp,"var multi_ssid = 0;");
#endif
    websWrite(wp,"var roamthres=\"%d\";",prof->mBaseCf.roamthre);
    /*Ìí¼ÓWDS¹¦ÄÜÊä³ö*/
#if (UTT_SMART_UI == FYES)
    if (prof->mBaseCf.apWorkMode == 5) {
	    websWrite(wp,"var WDSEnableds= \"1\";");
    } else {
	    websWrite(wp,"var WDSEnableds= \"0\";");
    }
#if (WIRELESS_5G == FYES)
	websWrite(wp,"var wifi_5g = \"on\";");
#else
	websWrite(wp,"var wifi_5g = \"off\";");
#endif
#else
    if (prof->mBaseCf.apWorkMode == 3) {
	    websWrite(wp,"var WDSEnableds= \"1\";");
    } else {
	    websWrite(wp,"var WDSEnableds= \"0\";");
    }
#endif
    /* WPSÊä³ö                   */
#if (UTT_SMART_UI == FYES)
    websWrite(wp,"var bridgeSSIDs=\"%s\";",prof->mBaseCf.apClient.ApCliSsid);
    websWrite(wp,"var briggeBSSIDs=\"%s\";",converMac6To12Byte(prof->mBaseCf.apClient.APCliMacs));
    websWrite(wp,"var encodeType=\"%s\";",prof->mBaseCf.encodeType);
#if (WLAN_MULTI_SSID == FYES)
    websWrite(wp,"var ssid2s=\"%s\";",prof->mBaseCf.SSID2);
    websWrite(wp,"var encodeType2=\"%s\";",prof->mBaseCf.encodeType2);
#if (UTT_SSID_ISOLATION == FYES)
    websWrite(wp,"var ssidSeparate=\"%d\";",prof->mBaseCf.ssidSeparate);
#endif
#endif
#else
    websWrite(wp,"var bridgeSSIDs=\"%s\";",prof->mBaseCf.apWds.bridgeSSID1);
    websWrite(wp,"var briggeBSSIDs=\"%s\";",converMac6To12Byte(prof->mBaseCf.apWds.WPSMacs0));
#endif
    websWrite(wp,"var wrlessModes=\"%d\";",prof->mBaseCf.WirelessMode);
    websWrite(wp,"var broadcastEnables=\"%d\";",prof->mBaseCf.SSIDBroadCastEn);
    websWrite(wp,"var channels=\"%d\";",prof->mBaseCf.Channel);
	if (prof->mBaseCf.ChanWidth == 1) {
		if (prof->mBaseCf.HT_BSSCoexistence == 0) {
			websWrite(wp,"var chanWidths=\"%d\";\n", 2);
		} else {
			websWrite(wp,"var chanWidths=\"%d\";\n", 1);
		}
	} else {
		websWrite(wp,"var chanWidths=\"%d\";\n", 0);
	}
    /* ÐÅºÅÇ¿¶È --by cao.yongxiang */
    websWrite(wp,"var txPowers=\"%d\";",prof->mBaseCf.TxPower);

#if (WMM_ENABLE == FYES)
    websWrite(wp,"var wmmenables=\"%d\";",prof->advance.wmm);
#endif
    /* APCient  input    */
    websWrite(wp,"var APssids=\"%s\";",prof->mBaseCf.apClient.ApCliSsid);
    websWrite(wp,"var APCliMac=\"%s\";",converMac6To12Byte(prof->mBaseCf.apClient.APCliMacs));

#if (UTT_SMART_UI == FYES)
    websWrite(wp,"var WdsSecModes=\"%s\";", prof->mBaseCf.SelAuthMode);
    websWrite(wp,"var wdsEncType=\"%s\";", prof->mBaseCf.apClient.AuthMode.EncrypType);
    websWrite(wp,"var wdspskPsswds=\"%s\";",prof->mBaseCf.apClient.AuthMode.pskPsswd);
    websWrite(wp,"var wdskeynums=\"%d\";", prof->mBaseCf.apClient.AuthMode.wepKeyID);
    websWrite(wp,"var wdswepkey1s=\"%s\";",prof->mBaseCf.apClient.AuthMode.wepkey1);
    websWrite(wp,"var wdswepkey2s=\"%s\";",prof->mBaseCf.apClient.AuthMode.wepkey2);
    websWrite(wp,"var wdswepkey3s=\"%s\";",prof->mBaseCf.apClient.AuthMode.wepkey3);
    websWrite(wp,"var wdswepkey4s=\"%s\";",prof->mBaseCf.apClient.AuthMode.wepkey4);
    websWrite(wp,"var wdskeyFormats=\"%d\";",prof->mBaseCf.apClient.AuthMode.wepKeyType);
    websWrite(wp,"var wdskeyLength1s=\"%d\";", prof->mBaseCf.apClient.AuthMode.wepkeyLength1);
    websWrite(wp,"var wdskeyLength2s=\"%d\";", prof->mBaseCf.apClient.AuthMode.wepkeyLength2);
    websWrite(wp,"var wdskeyLength3s=\"%d\";", prof->mBaseCf.apClient.AuthMode.wepkeyLength3);
    websWrite(wp,"var wdskeyLength4s=\"%d\";\n", prof->mBaseCf.apClient.AuthMode.wepkeyLength4);
#else
    websWrite(wp,"var wdspskPsswds=\"%s\";",prof->mBaseCf.apWds.AuthMode.pskPsswd);
    websWrite(wp,"var wdskeynums=\"%d\";", prof->mBaseCf.apWds.AuthMode.wepKeyID);
    websWrite(wp,"var wdswepkey1s=\"%s\";",prof->mBaseCf.apWds.AuthMode.wepkey1);
    websWrite(wp,"var wdswepkey2s=\"%s\";",prof->mBaseCf.apWds.AuthMode.wepkey2);
    websWrite(wp,"var wdswepkey3s=\"%s\";",prof->mBaseCf.apWds.AuthMode.wepkey3);
    websWrite(wp,"var wdswepkey4s=\"%s\";",prof->mBaseCf.apWds.AuthMode.wepkey4);
    websWrite(wp,"var wdskeyFormats=\"%d\";",prof->mBaseCf.apWds.AuthMode.wepKeyType);
    websWrite(wp,"var wdskeyLength1s=\"%d\";", prof->mBaseCf.apWds.AuthMode.wepkeyLength1);
    websWrite(wp,"var wdskeyLength2s=\"%d\";", prof->mBaseCf.apWds.AuthMode.wepkeyLength2);
    websWrite(wp,"var wdskeyLength3s=\"%d\";", prof->mBaseCf.apWds.AuthMode.wepkeyLength3);
    websWrite(wp,"var wdskeyLength4s=\"%d\";\n", prof->mBaseCf.apWds.AuthMode.wepkeyLength4);
    /* WPSÊä³ö                   */
    websWrite(wp,"var APMacs0=\"%s\";",converMac6To12Byte(prof->mBaseCf.apWds.WPSMacs0));
    websWrite(wp,"var APMacs1=\"%s\";",converMac6To12Byte(prof->mBaseCf.apWds.WPSMacs1));
    websWrite(wp,"var APMacs2=\"%s\";",converMac6To12Byte(prof->mBaseCf.apWds.WPSMacs2));
    websWrite(wp,"var APMacs3=\"%s\";",converMac6To12Byte(prof->mBaseCf.apWds.WPSMacs3));
    /* mantis 8856 */
    websWrite(wp,"var wdsSecModes=\"%s\";", prof->mBaseCf.apWds.wdsEncrypType);

    websWrite(wp,"var apcliSecModes=\"%s\";", prof->mBaseCf.SelAuthMode);
    websWrite(wp,"var apcliAuthModes=\"%s\";", prof->mBaseCf.apClient.AuthMode.EncrypType);
    websWrite(wp,"var apclipskPsswds=\"%s\";", prof->mBaseCf.apClient.AuthMode.pskPsswd);
    websWrite(wp,"var apclikeynums=\"%d\";",prof->mBaseCf.apClient.AuthMode.wepKeyID);
    websWrite(wp,"var apcliwepkey1s=\"%s\";", prof->mBaseCf.apClient.AuthMode.wepkey1);
    websWrite(wp,"var apcliwepkey2s=\"%s\";", prof->mBaseCf.apClient.AuthMode.wepkey2);
    websWrite(wp,"var apcliwepkey3s=\"%s\";", prof->mBaseCf.apClient.AuthMode.wepkey3);
    websWrite(wp,"var apcliwepkey4s=\"%s\";", prof->mBaseCf.apClient.AuthMode.wepkey4);
    websWrite(wp,"var apclikeyFormats=\"%d\";", prof->mBaseCf.apClient.AuthMode.wepKeyType);
    websWrite(wp,"var apclikeyLength1s=\"%d\";", prof->mBaseCf.apClient.AuthMode.wepkeyLength1);
    websWrite(wp,"var apclikeyLength2s=\"%d\";", prof->mBaseCf.apClient.AuthMode.wepkeyLength2);
    websWrite(wp,"var apclikeyLength3s=\"%d\";", prof->mBaseCf.apClient.AuthMode.wepkeyLength3);
    websWrite(wp,"var apclikeyLength4s=\"%d\";\n", prof->mBaseCf.apClient.AuthMode.wepkeyLength4);
#endif
#if 0
    websWrite(wp,"var apcliAuthModes=\"%s\";", nvram_bufget(RT2860_NVRAM,"ApCliEncrypType"));
    websWrite(wp,"var apclipskPsswds=\"%s\";", nvram_bufget(RT2860_NVRAM,"ApCliWPAPSK"));
    websWrite(wp,"var apclikeynums=\"%s\";", nvram_bufget(RT2860_NVRAM,"ApCliDefaultKeyId"));
    websWrite(wp,"var apcliwepkey1s=\"%s\";", nvram_bufget(RT2860_NVRAM,"ApCliKey1Str"));
    websWrite(wp,"var apcliwepkey2s=\"%s\";", nvram_bufget(RT2860_NVRAM,"ApCliKey2Str"));
    websWrite(wp,"var apcliwepkey3s=\"%s\";", nvram_bufget(RT2860_NVRAM,"ApCliKey3Str"));
    websWrite(wp,"var apcliwepkey4s=\"%s\";", nvram_bufget(RT2860_NVRAM,"ApCliKey4Str"));
    websWrite(wp,"var apclikeyFormats=\"%s\";\n", nvram_bufget(RT2860_NVRAM,"ApCliKey1Type"));
    websWrite(wp,"var apclikeyLength1s=\"%s\";", nvram_bufget(RT2860_NVRAM,"ApCliKey1Len"));
    websWrite(wp,"var apclikeyLength2s=\"%s\";", nvram_bufget(RT2860_NVRAM,"ApCliKey2Len"));
    websWrite(wp,"var apclikeyLength3s=\"%s\";", nvram_bufget(RT2860_NVRAM,"ApCliKey3Len"));
    websWrite(wp,"var apclikeyLength4s=\"%s\";", nvram_bufget(RT2860_NVRAM,"ApCliKey4Len"));
#endif



#if 0
    websWrite(wp,"var wdsSecModes=\"%s\";", nvram_bufget(RT2860_NVRAM,"WdsEncrypType"));
    websWrite(wp,"var wdspskPsswds=\"%s\";", nvram_bufget(RT2860_NVRAM,"WdsKey"));
    //websWrite(wp,"var wdskeynums=\"%s\";", nvram_bufget(RT2860_NVRAM,"WdsDefaultKeyId"));
    websWrite(wp,"var wdskeynums=\"%s\";", nvram_bufget(RT2860_NVRAM,"DefaultKeyID"));
    websWrite(wp,"var wdswepkey1s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Key1Str1"));
    websWrite(wp,"var wdswepkey2s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Key2Str1"));
    websWrite(wp,"var wdswepkey3s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Key3Str1"));
    websWrite(wp,"var wdswepkey4s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Key4Str1"));
    //websWrite(wp,"var wdskeyFormats=\"%s\";\n", nvram_bufget(RT2860_NVRAM,"WdsKeyType"));
    websWrite(wp,"var wdskeyFormats=\"%s\";\n", nvram_bufget(RT2860_NVRAM,"Key1Type"));
    websWrite(wp,"var wdskeyLength1s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Key1Len"));
    websWrite(wp,"var wdskeyLength2s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Key2Len"));
    websWrite(wp,"var wdskeyLength3s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Key3Len"));
    websWrite(wp,"var wdskeyLength4s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Key4Len"));
#endif
#if (WLAN_WORK_TIME == FYES)
	daysTemp = prof->mBaseCf.day;
	Byte2Days(daysTemp, data);
	websWrite(wp, "var days = \"%s\";", data);/*ÈÕÆÚ*/
	websWrite(wp, "var weekdaytimestarts=\"%s\";", prof->mBaseCf.weekdayTimeStart);
	websWrite(wp, "var weekdaytimestops=\"%s\";", prof->mBaseCf.weekdayTimeStop);
	websWrite(wp, "var weekendtimestarts=\"%s\";", prof->mBaseCf.weekendTimeStart);
	websWrite(wp, "var weekendtimestops=\"%s\";", prof->mBaseCf.weekendTimeStop);
	websWrite(wp, "var wlanworktimeEn=%d;", 1);
#else
	websWrite(wp, "var wlanworktimeEn=%d;", 0);
#endif
    getTheErrorMsg(eid, wp, argc, argv);  

  #if (UTT_SMART_UI == FYES)
     getTheRightMsg(eid,wp,argc,argv);
  #endif
    return 0;
}


#if (UTT_SMART_UI == FYES)

#if (WLAN_WORK_TIME == FYES)
/*ÐÂ·ç¸ñUI  ÎÞÏß¶¨Ê±Ò³ÃæÊä³ö²ÎÊý*/
static int getWirelessTimeConfig(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;
    UINT32 daysTemp = 0U;
    char_t data[16];
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,0);
    if(ProfInstIsFree(prof) == PROFOK){
	websWrite(wp, "wlanworktimeEn=\"%d\";", prof->mBaseCf.WlanTimeEn);
	daysTemp = prof->mBaseCf.day;
	Byte2Days(daysTemp, data);
	websWrite(wp, "var days = \"%s\";", data);/*ÈÕÆÚ*/
	websWrite(wp, "var weekdaytimestarts=\"%s\";", prof->mBaseCf.weekdayTimeStart);
	websWrite(wp, "var weekdaytimestops=\"%s\";", prof->mBaseCf.weekdayTimeStop);
	websWrite(wp, "var weekendtimestarts=\"%s\";", prof->mBaseCf.weekendTimeStart);
	websWrite(wp, "var weekendtimestops=\"%s\";", prof->mBaseCf.weekendTimeStop);
    }
    getTheErrorMsg(eid, wp, argc, argv); 
    #if (UTT_SMART_UI == FYES)
      getTheRightMsg(eid,wp,argc,argv);
  #endif 
    return 0;
}


/*ÐÂ·ç¸ñUI  ÎÞÏß¶¨Ê±±£´æ*/
static void wirelessTime_Sui(webs_t wp, char_t *path, char_t *query)
{
    editWlanTimeConfig(wp, path, query, 1);
    nvramWriteCommit();
     #if (UTT_SMART_UI == FYES) 
    setSuccessTips(); 
    #endif  
    websRedirect(wp,"TimedWireless.asp");
}
#endif


/*ÐÂ·ç¸ñUI ÅäÖÃÏòµ¼Ò³ÃæÊä³öÎÞÏß¿ª¹Ø×´Ì¬ºÍÃû³ÆÃÜÂëµÈ*/
static int  aspoutWlanWizard_Sui(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,0);
    if (ProfInstIsFree(prof) == PROFOK)
    {
	/* È«¾ÖÊä³ö */
	if (prof->head.active == 1) {
	    websWrite(wp,"WrlessEnables=\"%s\";",T("on"));
	}
	else{
	    websWrite(wp,"WrlessEnables=\"%s\";",T("off"));
	}
	websWrite(wp,"ssids=\"%s\";",prof->mBaseCf.SSID1);
	websWrite(wp,"broadcastEnables=\"%d\";",prof->mBaseCf.SSIDBroadCastEn);
	if (!strcmp(prof->mSafeCf.SelAuthMode, "WPAPSKWPA2PSK"))
	{
	    websWrite(wp,"pskPsswds=\"%s\";",prof->mSafeCf.ap.AuthMode.pskPsswd);
	}
	else
	{
	    websWrite(wp,"pskPsswds=\"\";");
	}
    }
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

#if (WIRELESS_5G == FYES)
/*ÐÂ·ç¸ñUI ÅäÖÃÏòµ¼Ò³ÃæÊä³öÎÞÏß5G¿ª¹Ø×´Ì¬ºÍÃû³ÆÃÜÂëµÈ*/
static int  aspoutWlanWizard_Sui_5g(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,1);
    if (ProfInstIsFree(prof) == PROFOK)
    {
	/* È«¾ÖÊä³ö */
	if (prof->head.active == 1) {
	    websWrite(wp,"WrlessEnables_5g=\"%s\";",T("on"));
	}
	else{
	    websWrite(wp,"WrlessEnables_5g=\"%s\";",T("off"));
	}
	websWrite(wp,"ssids_5g=\"%s\";",prof->mBaseCf.SSID1);
	websWrite(wp,"broadcastEnables_5g=\"%d\";",prof->mBaseCf.SSIDBroadCastEn);
	if (!strcmp(prof->mSafeCf.SelAuthMode, "WPAPSKWPA2PSK"))
	{
	    websWrite(wp,"pskPsswds_5g=\"%s\";",prof->mSafeCf.ap.AuthMode.pskPsswd);
	}
	else
	{
	    websWrite(wp,"pskPsswds_5g=\"\";");
	}
    }
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
#endif

#endif


static int  getWlanCurrentMac(int eid, webs_t wp, int argc, char_t **argv)
{
    char if_hw[18] = {0};
    int waitCount = 0;
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,0);

    if(ProfInstIsFree(prof) == PROFOK)
    {
        /*Èô²»¿ªÆôÎÞÏß¹¦ÄÜ ÔòÎÞÏß ½Ó¿ÚmacÊÇ¶Á²»µ½µÄ¡£¹ÊÎÞÐèµÈ´ý*/
	if( (prof->head.active == TRUE)
#if (WLAN_WORK_TIME == FYES)
		&& (prof->time_active == 1)
#endif
	  )
	{
#if 0
	    /*wirelessBasic×îºóÈôÕë¶Ô12771ÎÊÌâ Ë¯Ãß3s¡£Ôò´Ë´¦¾Í²»ÐèÒª6sÕâÃ´¾ÃÁË*/
#endif
	    while(waitCount < 10)
	    {
		if (-1 == getIfMac("ra0", if_hw))
		{/*12771 : ¸ÕÅäÖÃÒ³Ãæºó ÎÞÏß½Ó¿Ú¿ÉÄÜ»¹Î´ÆðÀ´*/
		    sleep(1);
		    waitCount++;
		}
		else 
		{
		    break;
		}
	    }
	    if(waitCount == 10) 
	    {
		if_hw[0] = '\0';
	    }
	}
    }
    return websWrite(wp, T("%s"), if_hw);
}

#if(WIRELESS_5G == FYES)
static void wirelessBasic_5g(webs_t wp, char_t *path, char_t *query)
{
    char_t *wirelessmode,*apworkmode,*ssid, /* *hssid,*/*apmac,*apssid,*sz11gChannel,*n_bandwidth,*n_vht_bandwidth,*radio,*broadcastEnable ,*wpsmac0,*wpsmac1,*wpsmac2,*wpsmac3,*WdsList;
    char *def_mac = "000000000000";
    int oldworkmode;
#if 0
    char	 hidden_ssid[16];
#endif
#if(WLAN_MULTI_SSID == FYES)
    char_t *encodeType;
    char_t *encodeType2, *ssid2;
#endif

    int is_n=0;

    MibProfileType mibType = MIB_PROF_WIRELESS;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    WirelessProfile *prof= NULL;
    const char * ErrMsg = NULL;
    /* ÆôÓÃÎÞÏß¹¦ÄÜ 2  */
    radio = websGetVar(wp, T("WrlessEnable"), T(""));
    /* AP¹¤×÷Ä£Ê½      */
    apworkmode = websGetVar(wp, T("APWorkMode"), T(""));
    /*    ssid         */
    ssid = websGetVar(wp, T("ssid"), T(""));
#if(WLAN_MULTI_SSID == FYES) 
    encodeType = websGetVar(wp, T("encodeType"), T(""));
    ssid2 = websGetVar(wp, T("ssid2"), T(""));
    encodeType2 = websGetVar(wp, T("encodeType2"), T("")); 
#endif
    /* ÎÞÏßÄ£Ê½  9: bgn mode  */
    wirelessmode = websGetVar(wp, T("wrlessMode"), T("")); 
    /* ÐÅµÀ */ 
    sz11gChannel = websGetVar(wp, T("channel"), T("")); 
    /* 0 ÆµµÀ´ø¿í */
    n_bandwidth = websGetVar(wp, T("chanWidth"), T(""));
    n_vht_bandwidth = websGetVar(wp, T("vhtChanWidth"), T(""));
    /* ÆôÓÃSSID¹ã²¥ */
    broadcastEnable = websGetVar(wp, T("broadcastEnable"), T("")); 
    /* APµÄSSID    */
    apssid=websGetVar(wp, T("APssid"), T(""));
    /* APclient µÄAP MAC */
    apmac=websGetVar(wp, T("APCliMacs"), T(""));
    /* WDSµÄAP MAC */
	char_t *bridgeSSID;
	bridgeSSID = websGetVar(wp, T("bridgessid"), T(""));
    wpsmac0=websGetVar(wp, T("APMac0"), def_mac);
    wpsmac1=websGetVar(wp, T("APMac1"), def_mac);
    wpsmac2=websGetVar(wp, T("APMac2"), def_mac);
    wpsmac3=websGetVar(wp, T("APMac3"), def_mac);
    if (strlen(wpsmac1) == 0) {
	wpsmac1 = def_mac;
    }
    if (strlen(wpsmac2) == 0) {
	wpsmac2 = def_mac;
    }
    if (strlen(wpsmac3) == 0) {
	wpsmac3 = def_mac;
    }

    /*WDSµÄMACLIST */
    WdsList=websGetVar(wp, T("wds_list"), T(""));
    /* °²È«Ä£Ê½   */
    char_t *mode, *enc, *wpapsk, *keyid, *keytype,
	   *key1, *key2, *key3, *key4,*keylen1,*keylen2,*keylen3,*keylen4,*wdswpapsk;
    mode = websGetVar(wp, T("apcli_mode"), T("OPEN"));
    /* ¼ÓÃÜËã·¨         */
    enc = websGetVar(wp, T("apcli_enc"), T("NONE"));
    /*  Ô¤¹²ÏíÃÜÔ¿      */
    wpapsk = websGetVar(wp, T("apclipskPsswd"), T("12345678"));
    /*  WDSÔ¤¹²ÏíÃÜÔ¿  */
    wdswpapsk = websGetVar(wp, T("wdspskPsswd"), T(""));
    /*ÃÜÔ¿ÐòºÅ  */
    keyid = websGetVar(wp, T("apclikeynum"), T("1"));
    /*ÃÜÔ¿¸ñÊ½ */
    keytype = websGetVar(wp, T("apclikeyFormat"), T("1"));
    /* ÃÜÔ¿1-4µÄÖµ         */
    key1 = websGetVar(wp, T("apcliwepkey1"), T(""));
    key2 = websGetVar(wp, T("apcliwepkey2"), T(""));
    key3 = websGetVar(wp, T("apcliwepkey3"), T(""));
    key4 = websGetVar(wp, T("apcliwepkey4"), T(""));

    keylen1=websGetVar(wp, T("apclikeyLength1"), T(""));
    keylen2=websGetVar(wp, T("apclikeyLength2"), T(""));
    keylen3=websGetVar(wp, T("apclikeyLength3"), T(""));
    keylen4=websGetVar(wp, T("apclikeyLength4"), T(""));

    //secmode end
    //strcpy(mac,apmac);
    //Conversion2StdMac(mac);
    //printf("radio=%s,wirelessmode=%s,ssid=%s,broadcastEnable=%s,sz11gChannel=%s,n_bandwidtw=%s\napmac=%s wpsmac0 = %s,wpsmac1 = %s,wpsmac2 = %s,wpsmac3 = %s\n mode = %s enc = %s\n",radio,wirelessmode,ssid,broadcastEnable,sz11gChannel,n_bandwidth,apmac,wpsmac0,wpsmac1,wpsmac2,wpsmac3,mode,enc);


    uttSemP(SEM_WIFI_NO,0);
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 1);
    profAction = PROF_CHANGE_EDIT;
    ProfBackupByIndex(mibType, profAction, 1, &profhead);

    if (!strncmp(radio, "on", 3)) {
	//doSystem("iwpriv ra0 set RadioOn=1");
	//nvram_bufset(RT2860_NVRAM, "RadioOff", "0");
	prof->head.active = 1;
    }
    else{
	//doSystem("iwpriv ra0 set RadioOn=0");
	//nvram_bufset(RT2860_NVRAM, "RadioOff", "1");
	prof->head.active = 0;
    }
    //nvram_bufset(RT2860_NVRAM, "WirelessMode", wirelessmode);
    oldworkmode = prof->mBaseCf.apWorkMode;
    prof->mBaseCf.WirelessMode = strtoul(wirelessmode, 0, 10);
#if 0
    //BasicRate: bg,bgn,n:15, b:3; g,gn:351
    if (!strncmp(wirelessmode, "2", 2)) {// 802.11a
		prof->mBaseCf.BasicRate = 15;
		prof->mBaseCf.HT_TxStream = 1;
		prof->mBaseCf.HT_RxStream = 1;
    } else if (!strncmp(wirelessmode, "8", 2)) {
		prof->mBaseCf.BasicRate = 15;
		prof->mBaseCf.HT_TxStream = 2;
		prof->mBaseCf.HT_RxStream = 2;
    } else if (!strncmp(wirelessmode, "14", 3)) {
		prof->mBaseCf.BasicRate = 15;
		prof->mBaseCf.HT_TxStream = 2;
		prof->mBaseCf.HT_RxStream = 2;
	} else {//bg,bgn,n
		prof->mBaseCf.BasicRate = 15;
		prof->mBaseCf.HT_TxStream = 2;
		prof->mBaseCf.HT_RxStream = 2;
	//nvram_bufset(RT2860_NVRAM, "BasicRate", "15");
    }
#endif
    prof->mBaseCf.BasicRate = 15;
    prof->mBaseCf.HT_TxStream = UTT_XSTREAM_NUM_5G;
    prof->mBaseCf.HT_RxStream = UTT_XSTREAM_NUM_5G;

    if (!strncmp(wirelessmode, "6", 2) || !strncmp(wirelessmode, "9", 2)) {
	is_n = 1;
    }

    //SSID, Multiple SSID
    if (0 == strlen(ssid)) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_SSID_CANT_NULL);
	setTheErrorMsg(ErrMsg);
	goto out;
    }
    //nvram_bufset(RT2860_NVRAM, "SSID1", ssid);
    strcpy(prof->mBaseCf.SSID1,ssid);
#if(WLAN_MULTI_SSID == FYES)
    strcpy(prof->mBaseCf.SSID2, ssid2);
#if (MAKE_MONEY == FYES)
        prof->mBaseCf.BssidNum = 4; 
#else
        prof->mBaseCf.BssidNum = 2;
#endif
    strncpy(prof->mBaseCf.encodeType,encodeType,7);/*±£´æssid¼ÓÃÜ·½Ê½*/
    strncpy(prof->mBaseCf.encodeType2,encodeType2,7);/*±£´æssid¼ÓÃÜ·½Ê½*/
#endif



#if 0
    if (strchr(hssid, '0') != NULL) {
	sprintf(hidden_ssid, "%s", "1");
    }
    else {
	sprintf(hidden_ssid, "%s", "0");
    }
#endif

    //11abg Channel or AutoSelect
    if (0 == strlen(sz11gChannel)) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_CHANNEL_CANT_NULL);
	setTheErrorMsg(ErrMsg);
	goto out;
    }
    if (!strncmp(sz11gChannel, "0", 2)){
	prof->mBaseCf.AutoChannelSelect = 2;
    }
    else {
	prof->mBaseCf.AutoChannelSelect = 0;
    }
    if (0 != strlen(sz11gChannel))
    {
	prof->mBaseCf.Channel = strtoul(sz11gChannel, 0, 10);
	doSystem("iwpriv rai0 set Channel=%s", sz11gChannel);
    }

    //	if (is_n) 
#if 0
    {
	nvram_bufset(RT2860_NVRAM, "HT_BW", n_bandwidth);
    }
#endif
    prof->mBaseCf.ChanWidth = strtoul(n_bandwidth,0,10);

    if( prof->mBaseCf.ChanWidth == 1)
    {/*20/40 auto*/
	prof->mBaseCf.HT_BSSCoexistence = 1;
    }
    else
    {
	prof->mBaseCf.HT_BSSCoexistence = 0;
    }
    if (prof->mBaseCf.ChanWidth == 2) {/*20m*/
	prof->mBaseCf.ChanWidth = 1;
    } 

    prof->mBaseCf.VChanWidth = strtoul(n_vht_bandwidth,0,10);

    //ap workmode
    int deleteWan4Flag=0;
    if( (strcmp(apworkmode, "5") !=0) && oldworkmode == 5 ) {
	deleteWan4Flag=1;
    }
    if (!strncmp(apworkmode, "1", 2)) {//ap
	//nvram_bufset(RT2860_NVRAM, "HideSSID", hidden_ssid);//8Î»or 1Î»?
	//nvram_bufset(RT2860_NVRAM, "WdsEnable", T("0"));
	prof->mBaseCf.apClient.apclientEnable = 0;
	prof->mBaseCf.OperationMode  = 1;
	prof->mBaseCf.SSIDBroadCastEn = strtoul(broadcastEnable, 0 ,10);
    }
    else if(!strncmp(apworkmode, "5", 2)){//apclient
	prof->mBaseCf.apClient.apclientEnable = 1;
	prof->mBaseCf.OperationMode = 3;
	strcpy(prof->mBaseCf.apClient.ApCliSsid, apssid);
	prof->mBaseCf.SSIDBroadCastEn = strtoul(broadcastEnable, 0 ,10);
	converMac12To6Byte(apmac, &(prof->mBaseCf.apClient.APCliMacs));
	strcpy(prof->mBaseCf.SelAuthMode, mode);
	strcpy(prof->mBaseCf.apClient.AuthMode.EncrypType, enc);
	strcpy(prof->mBaseCf.apClient.AuthMode.pskPsswd, wpapsk);
	prof->mBaseCf.apClient.AuthMode.wepKeyID= strtoul(keyid, 0, 10);
	strcpy(prof->mBaseCf.apClient.AuthMode.wepkey1, key1);
	strcpy(prof->mBaseCf.apClient.AuthMode.wepkey2, key2);
	strcpy(prof->mBaseCf.apClient.AuthMode.wepkey3, key3);
	strcpy(prof->mBaseCf.apClient.AuthMode.wepkey4, key4);
	prof->mBaseCf.apClient.AuthMode.wepkeyLength1 = strtoul(keylen1, 0, 10);
	prof->mBaseCf.apClient.AuthMode.wepkeyLength2 = strtoul(keylen2, 0, 10);
	prof->mBaseCf.apClient.AuthMode.wepkeyLength3 = strtoul(keylen3, 0, 10);
	prof->mBaseCf.apClient.AuthMode.wepkeyLength4 = strtoul(keylen4, 0, 10);
	prof->mBaseCf.apClient.AuthMode.wepKeyType = strtoul(keytype, 0, 10);
    }
    else if(!strncmp(apworkmode, "3", 2)){//repeater
	strcpy(prof->mBaseCf.apWds.bridgeSSID1,bridgeSSID);
	prof->mBaseCf.apClient.apclientEnable = 0;
	prof->mBaseCf.OperationMode  = 1;
	prof->mBaseCf.SSIDBroadCastEn = strtoul(broadcastEnable, 0 ,10);
	converMac12To6Byte(wpsmac0,&(prof->mBaseCf.apWds.WPSMacs0));
	converMac12To6Byte(wpsmac1,&(prof->mBaseCf.apWds.WPSMacs1));
	converMac12To6Byte(wpsmac2,&(prof->mBaseCf.apWds.WPSMacs2));
	converMac12To6Byte(wpsmac3,&(prof->mBaseCf.apWds.WPSMacs3));
	strcpy(prof->mBaseCf.apWds.WdsList,WdsList);
    }
    else if(!strncmp(apworkmode, "2", 2)){//p-t-p /bridge
	prof->mBaseCf.apClient.apclientEnable = 0;
	prof->mBaseCf.OperationMode  = 1;
	converMac12To6Byte(wpsmac0,&(prof->mBaseCf.apWds.WPSMacs0));
	converMac12To6Byte(wpsmac1,&(prof->mBaseCf.apWds.WPSMacs1));
	converMac12To6Byte(wpsmac2,&(prof->mBaseCf.apWds.WPSMacs2));
	converMac12To6Byte(wpsmac3,&(prof->mBaseCf.apWds.WPSMacs3));
    }
    else{//p-t-mp/azy mode
	prof->mBaseCf.apClient.apclientEnable = 0;
	prof->mBaseCf.OperationMode  = 1;
	prof->mBaseCf.SSIDBroadCastEn = strtoul(broadcastEnable, 0 ,10);
    }
    /*  WDSµÄ°²È«Ä£Ê½²ÎÊý */
    if(!strncmp(apworkmode, "2", 2)||!strncmp(apworkmode, "3", 2)||!strncmp(apworkmode, "4", 2)){//secmode
	if(!strncmp(mode,"TKIP",5)||!strncmp(mode,"AES",4)){
	    strcpy(prof->mSafeCf.ap.AuthMode.pskPsswd,wdswpapsk); 
	    prof->mBaseCf.apWds.WdsPhyMode = 1;
	}
	else if(!strncmp(mode,"WEP",4)){
	    /*  ÎÞÏß°²È«ÅäÖÃºÍ»ù±¾ÅäÖÃµÄWDSµÄ°²È«ÅäÖÃ´æ´¢ÔÚÍ¬Ò»½á¹¹ÌåÖÐ   */
	    prof->mSafeCf.ap.AuthMode.wepKeyID= strtoul(keyid, 0, 10);
	    strcpy(prof->mSafeCf.ap.AuthMode.wepkey1, key1);
	    strcpy(prof->mSafeCf.ap.AuthMode.wepkey2, key2);
	    strcpy(prof->mSafeCf.ap.AuthMode.wepkey3, key3);
	    strcpy(prof->mSafeCf.ap.AuthMode.wepkey4, key4);
	    prof->mSafeCf.ap.AuthMode.wepkeyLength1 = strtoul(keylen1, 0, 10);
	    prof->mSafeCf.ap.AuthMode.wepkeyLength2 = strtoul(keylen2, 0, 10);
	    prof->mSafeCf.ap.AuthMode.wepkeyLength3 = strtoul(keylen3, 0, 10);
	    prof->mSafeCf.ap.AuthMode.wepkeyLength4 = strtoul(keylen4, 0, 10);
	    prof->mSafeCf.ap.AuthMode.wepKeyType = strtoul(keytype, 0, 10);
	    /* add by ydh Ôö¼ÓEncrypType£¬AuthModeÅäÖÃ,²ÎÕÕ520W */
	    strcpy(prof->mSafeCf.ap.AuthMode.EncrypType, "WEP");
	    strcpy(prof->mSafeCf.SelAuthMode, "WEPAUTO");
	}
	//nvram_bufset(RT2860_NVRAM, "WdsEncrypType", mode); //Î´±£´æ
	/* mantis 8856, ÎÞÏß°²È«ÅäÖÃºÍwds°²È«ÅäÖÃ²»Í¬ */
	strncpy(prof->mBaseCf.apWds.wdsEncrypType, mode, UTT_WDS_MODE_LEN);
    }
    prof->mBaseCf.apWorkMode = strtoul(apworkmode,0,10);
    prof->mBaseCf.apWds.wdsEnable = strtoul(apworkmode, 0, 10);
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
#if (WLAN_WORK_TIME == FYES)
    editWlanTimeConfig(wp, path, query, 0);
#endif
    nvramWriteCommit();
out:
    uttSemV(SEM_WIFI_NO,0);

#if 0
/*ÎªÁËÒ³ÃæÏìÓ¦ËÙ¶È¡£ÕâÀïÏÈ²»í¼Ó*/
    /*12771 : ¸ÕÅäÖÃÒ³Ãæºó ÎÞÏß½Ó¿Ú¿ÉÄÜ»¹Î´ÆðÀ´
     * getWlanCurrentMac ÀïÒ²Ðè±£Ö¤¡£ÒòÎª ÕâÀï²»ÄÜË¯ÃßÌ«³¤
     * Í¬Ê±ÕâÀïÒ²²»ÄÜÉ¾³ýsleep
     * ÒòÎªÈôÁ½¸öÈËÍ¬Ê± ²Ù×÷Ò³Ãæ  Ôò¿ÉÄÜgetWlanCurrentMac»á±»Ìø¹ý
     *
     * Òª±£Ö¤×î³£ÓÃµÄÊ¹ÓÃÇé¿öÒ³Ãæ·´Ó³ËÙ¶È ±È½Ï¿ì ÇÒÃ»ÎÊÌâ*/
    sleep(3);
#endif

    websRedirect(wp,"WlanBase_5g.asp");

}
#endif

#if(WIRELESS_5G == FYES)
static int getWirelessBaseConfig_5g(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;
#if (WLAN_WORK_TIME == FYES)
    UINT32 daysTemp = 0U;
    char_t data[16];
#endif
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,1);
        if(prof == NULL){
        return 0;
    }

    /* È«¾ÖÊä³ö */
    if (prof->head.active == 1) {
        websWrite(wp,"var WrlessEnables=\"%s\";",T("on"));
    } else{
        websWrite(wp,"var WrlessEnables=\"%s\";",T("off"));
    }
    websWrite(wp,"var ssids=\"%s\";",prof->mBaseCf.SSID1);
    websWrite(wp,"var APWorkModes=\"%d\";",prof->mBaseCf.apWorkMode);

#if (WLAN_MULTI_SSID == FYES)
    websWrite(wp,"var ssid2s=\"%s\";",prof->mBaseCf.SSID2);
    websWrite(wp,"var encodeType=\"%s\";",prof->mBaseCf.encodeType);
    websWrite(wp,"var encodeType2=\"%s\";",prof->mBaseCf.encodeType2);
     websWrite(wp,"var multi_ssid = 1;");
#else
     websWrite(wp,"var multi_ssid = 0;");
#endif

    /*Ìí¼ÓWDS¹¦ÄÜÊä³ö*/
#if (UTT_SMART_UI == FYES)
    if (prof->mBaseCf.apWorkMode == 5) {
	    websWrite(wp,"var WDSEnableds= \"1\";");
    } else {
	    websWrite(wp,"var WDSEnableds= \"0\";");
    }
#else
    if (prof->mBaseCf.apWorkMode == 1) {
	    websWrite(wp,"var WDSEnableds= \"0\";");
    } else {
	    websWrite(wp,"var WDSEnableds= \"1\";");
    }
#endif
    /* WPSÊä³ö */
#if (UTT_SMART_UI == FYES)
    websWrite(wp,"var bridgeSSIDs=\"%s\";",prof->mBaseCf.apClient.ApCliSsid);
    websWrite(wp,"var briggeBSSIDs=\"%s\";",converMac6To12Byte(prof->mBaseCf.apClient.APCliMacs));
    websWrite(wp,"var encodeType=\"%s\";",prof->mBaseCf.encodeType);
#if (WLAN_MULTI_SSID == FYES)
    websWrite(wp,"var ssid2s=\"%s\";",prof->mBaseCf.SSID2);
    websWrite(wp,"var encodeType2=\"%s\";",prof->mBaseCf.encodeType2);
#if (UTT_SSID_ISOLATION == FYES)
    websWrite(wp,"var ssidSeparate=\"%d\";",prof->mBaseCf.ssidSeparate);
#endif
#endif
#else
    websWrite(wp,"var bridgeSSIDs=\"%s\";",prof->mBaseCf.apWds.bridgeSSID1);
    websWrite(wp,"var briggeBSSIDs=\"%s\";",converMac6To12Byte(prof->mBaseCf.apWds.WPSMacs0));
#endif
    websWrite(wp,"var wrlessModes=\"%d\";",prof->mBaseCf.WirelessMode);
    websWrite(wp,"var broadcastEnables=\"%d\";",prof->mBaseCf.SSIDBroadCastEn);
    websWrite(wp,"var channels=\"%d\";",prof->mBaseCf.Channel);
	if (prof->mBaseCf.ChanWidth == 1) {
		if (prof->mBaseCf.HT_BSSCoexistence == 0) {
			websWrite(wp,"var chanWidths=\"%d\";\n", 2);
		} else {
			websWrite(wp,"var chanWidths=\"%d\";\n", 1);
		}
	} else {
		websWrite(wp,"var chanWidths=\"%d\";\n", 0);
	}

	websWrite(wp,"var vhtChanWidths=\"%d\";\n", prof->mBaseCf.VChanWidth);
#if (WMM_ENABLE == FYES)
	websWrite(wp,"var wmmenables=\"%d\";\n", prof->advance.wmm);
#endif
    /* ÐÅºÅÇ¿¶È --by cao.yongxiang*/
    websWrite(wp,"var txPowers=\"%d\";\n", prof->mBaseCf.TxPower);

    /* APCient  input    */
    websWrite(wp,"var APssids=\"%s\";",prof->mBaseCf.apClient.ApCliSsid);
    websWrite(wp,"var APCliMac=\"%s\";",converMac6To12Byte(prof->mBaseCf.apClient.APCliMacs));
#if (UTT_SMART_UI == FYES)
    websWrite(wp,"var countrys=\"%d\";", prof->mBaseCf.Country);
    websWrite(wp,"var WdsSecModes=\"%s\";", prof->mBaseCf.SelAuthMode);
    websWrite(wp,"var wdsEncType=\"%s\";", prof->mBaseCf.apClient.AuthMode.EncrypType);
    websWrite(wp,"var wdspskPsswds=\"%s\";",prof->mBaseCf.apClient.AuthMode.pskPsswd);
    websWrite(wp,"var wdskeynums=\"%d\";", prof->mBaseCf.apClient.AuthMode.wepKeyID);
    websWrite(wp,"var wdswepkey1s=\"%s\";",prof->mBaseCf.apClient.AuthMode.wepkey1);
    websWrite(wp,"var wdswepkey2s=\"%s\";",prof->mBaseCf.apClient.AuthMode.wepkey2);
    websWrite(wp,"var wdswepkey3s=\"%s\";",prof->mBaseCf.apClient.AuthMode.wepkey3);
    websWrite(wp,"var wdswepkey4s=\"%s\";",prof->mBaseCf.apClient.AuthMode.wepkey4);
    websWrite(wp,"var wdskeyFormats=\"%d\";",prof->mBaseCf.apClient.AuthMode.wepKeyType);
    websWrite(wp,"var wdskeyLength1s=\"%d\";", prof->mBaseCf.apClient.AuthMode.wepkeyLength1);
    websWrite(wp,"var wdskeyLength2s=\"%d\";", prof->mBaseCf.apClient.AuthMode.wepkeyLength2);
    websWrite(wp,"var wdskeyLength3s=\"%d\";", prof->mBaseCf.apClient.AuthMode.wepkeyLength3);
    websWrite(wp,"var wdskeyLength4s=\"%d\";\n", prof->mBaseCf.apClient.AuthMode.wepkeyLength4);
#else
    /* WPSÊä³ö                   */
    websWrite(wp,"var APMacs0=\"%s\";",converMac6To12Byte(prof->mBaseCf.apWds.WPSMacs0));
    websWrite(wp,"var APMacs1=\"%s\";",converMac6To12Byte(prof->mBaseCf.apWds.WPSMacs1));
    websWrite(wp,"var APMacs2=\"%s\";",converMac6To12Byte(prof->mBaseCf.apWds.WPSMacs2));
    websWrite(wp,"var APMacs3=\"%s\";",converMac6To12Byte(prof->mBaseCf.apWds.WPSMacs3));
    /* mantis 8856 */
    websWrite(wp,"var wdsSecModes=\"%s\";", prof->mBaseCf.apWds.wdsEncrypType);

    websWrite(wp,"var apcliSecModes=\"%s\";", prof->mBaseCf.SelAuthMode);
    websWrite(wp,"var apcliAuthModes=\"%s\";", prof->mBaseCf.apClient.AuthMode.EncrypType);
    websWrite(wp,"var apclipskPsswds=\"%s\";", prof->mBaseCf.apClient.AuthMode.pskPsswd);
    websWrite(wp,"var apclikeynums=\"%d\";",prof->mBaseCf.apClient.AuthMode.wepKeyID);
    websWrite(wp,"var apcliwepkey1s=\"%s\";", prof->mBaseCf.apClient.AuthMode.wepkey1);
    websWrite(wp,"var apcliwepkey2s=\"%s\";", prof->mBaseCf.apClient.AuthMode.wepkey2);
    websWrite(wp,"var apcliwepkey3s=\"%s\";", prof->mBaseCf.apClient.AuthMode.wepkey3);
    websWrite(wp,"var apcliwepkey4s=\"%s\";", prof->mBaseCf.apClient.AuthMode.wepkey4);
    websWrite(wp,"var apclikeyFormats=\"%d\";", prof->mBaseCf.apClient.AuthMode.wepKeyType);
    websWrite(wp,"var apclikeyLength1s=\"%d\";", prof->mBaseCf.apClient.AuthMode.wepkeyLength1);
    websWrite(wp,"var apclikeyLength2s=\"%d\";", prof->mBaseCf.apClient.AuthMode.wepkeyLength2);
    websWrite(wp,"var apclikeyLength3s=\"%d\";", prof->mBaseCf.apClient.AuthMode.wepkeyLength3);
    websWrite(wp,"var apclikeyLength4s=\"%d\";\n", prof->mBaseCf.apClient.AuthMode.wepkeyLength4);

    websWrite(wp,"var wdspskPsswds=\"%s\";",prof->mSafeCf.ap.AuthMode.pskPsswd);
    websWrite(wp,"var wdskeynums=\"%d\";", prof->mSafeCf.ap.AuthMode.wepKeyID);
    websWrite(wp,"var wdswepkey1s=\"%s\";",prof->mSafeCf.ap.AuthMode.wepkey1);
    websWrite(wp,"var wdswepkey2s=\"%s\";",prof->mSafeCf.ap.AuthMode.wepkey2);
    websWrite(wp,"var wdswepkey3s=\"%s\";",prof->mSafeCf.ap.AuthMode.wepkey3);
    websWrite(wp,"var wdswepkey4s=\"%s\";",prof->mSafeCf.ap.AuthMode.wepkey4);
    websWrite(wp,"var wdskeyFormats=\"%d\";",prof->mSafeCf.ap.AuthMode.wepKeyType);
    websWrite(wp,"var wdskeyLength1s=\"%d\";", prof->mSafeCf.ap.AuthMode.wepkeyLength1);
    websWrite(wp,"var wdskeyLength2s=\"%d\";", prof->mSafeCf.ap.AuthMode.wepkeyLength2);
    websWrite(wp,"var wdskeyLength3s=\"%d\";", prof->mSafeCf.ap.AuthMode.wepkeyLength3);
    websWrite(wp,"var wdskeyLength4s=\"%d\";\n", prof->mSafeCf.ap.AuthMode.wepkeyLength4);
#endif

#if 0
    websWrite(wp,"var apcliAuthModes=\"%s\";", nvram_bufget(RT2860_NVRAM,"ApCliEncrypType"));
    websWrite(wp,"var apclipskPsswds=\"%s\";", nvram_bufget(RT2860_NVRAM,"ApCliWPAPSK"));
    websWrite(wp,"var apclikeynums=\"%s\";", nvram_bufget(RT2860_NVRAM,"ApCliDefaultKeyId"));
    websWrite(wp,"var apcliwepkey1s=\"%s\";", nvram_bufget(RT2860_NVRAM,"ApCliKey1Str"));
    websWrite(wp,"var apcliwepkey2s=\"%s\";", nvram_bufget(RT2860_NVRAM,"ApCliKey2Str"));
    websWrite(wp,"var apcliwepkey3s=\"%s\";", nvram_bufget(RT2860_NVRAM,"ApCliKey3Str"));
    websWrite(wp,"var apcliwepkey4s=\"%s\";", nvram_bufget(RT2860_NVRAM,"ApCliKey4Str"));
    websWrite(wp,"var apclikeyFormats=\"%s\";\n", nvram_bufget(RT2860_NVRAM,"ApCliKey1Type"));
    websWrite(wp,"var apclikeyLength1s=\"%s\";", nvram_bufget(RT2860_NVRAM,"ApCliKey1Len"));
    websWrite(wp,"var apclikeyLength2s=\"%s\";", nvram_bufget(RT2860_NVRAM,"ApCliKey2Len"));
    websWrite(wp,"var apclikeyLength3s=\"%s\";", nvram_bufget(RT2860_NVRAM,"ApCliKey3Len"));
    websWrite(wp,"var apclikeyLength4s=\"%s\";", nvram_bufget(RT2860_NVRAM,"ApCliKey4Len"));
#endif



#if 0
    websWrite(wp,"var wdsSecModes=\"%s\";", nvram_bufget(RT2860_NVRAM,"WdsEncrypType"));
    websWrite(wp,"var wdspskPsswds=\"%s\";", nvram_bufget(RT2860_NVRAM,"WdsKey"));
    //websWrite(wp,"var wdskeynums=\"%s\";", nvram_bufget(RT2860_NVRAM,"WdsDefaultKeyId"));
    websWrite(wp,"var wdskeynums=\"%s\";", nvram_bufget(RT2860_NVRAM,"DefaultKeyID"));
    websWrite(wp,"var wdswepkey1s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Key1Str1"));
    websWrite(wp,"var wdswepkey2s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Key2Str1"));
    websWrite(wp,"var wdswepkey3s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Key3Str1"));
    websWrite(wp,"var wdswepkey4s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Key4Str1"));
    //websWrite(wp,"var wdskeyFormats=\"%s\";\n", nvram_bufget(RT2860_NVRAM,"WdsKeyType"));
    websWrite(wp,"var wdskeyFormats=\"%s\";\n", nvram_bufget(RT2860_NVRAM,"Key1Type"));
    websWrite(wp,"var wdskeyLength1s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Key1Len"));
    websWrite(wp,"var wdskeyLength2s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Key2Len"));
    websWrite(wp,"var wdskeyLength3s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Key3Len"));
    websWrite(wp,"var wdskeyLength4s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Key4Len"));
#endif
#if (WLAN_WORK_TIME == FYES)
	daysTemp = prof->mBaseCf.day;
	Byte2Days(daysTemp, data);
	websWrite(wp, "var days = \"%s\";", data);/*ÈÕÆÚ*/
	websWrite(wp, "var weekdaytimestarts=\"%s\";", prof->mBaseCf.weekdayTimeStart);
	websWrite(wp, "var weekdaytimestops=\"%s\";", prof->mBaseCf.weekdayTimeStop);
	websWrite(wp, "var weekendtimestarts=\"%s\";", prof->mBaseCf.weekendTimeStart);
	websWrite(wp, "var weekendtimestops=\"%s\";", prof->mBaseCf.weekendTimeStop);
	websWrite(wp, "var wlanworktimeEn=%d;", 1);
#else
	websWrite(wp, "var wlanworktimeEn=%d;", 0);
#endif
    getTheErrorMsg(eid, wp, argc, argv);  
#if (UTT_SMART_UI == FYES)
    getTheRightMsg(eid,wp,argc,argv);
#endif

    return 0;
}
#endif

#if (UTT_SMART_UI == FYES)
/********************************************************************
 * º¯Êý£º getWifiMac
 * ¹¦ÄÜ£º »ñÈ¡ÎÞÏßÍø¿¨macµØÖ·
 * ´´½¨£º 2013-12-20
 * ²ÎÊý£º prof->ÎÞÏßprofileÊµÀý£¬if_name->Íø¿¨Ãû³Æ£¬wifiMac->ÓÃÀ´´æ·ÅÎÞÏßmacµØÖ·
 * ·µ»Ø£º ÎÞ
 * ×÷Õß£º zhao.chong 
 ********************************************************************/ 
static int getWifiMac(WirelessProfile *prof, char *if_name, char wifiMac[])
{
    int waitCount = 0;

    if(ProfInstIsFree(prof) == PROFOK) {
        /*Èô²»¿ªÆôÎÞÏß¹¦ÄÜ ÔòÎÞÏß ½Ó¿ÚmacÊÇ¶Á²»µ½µÄ¡£¹ÊÎÞÐèµÈ´ý*/
	if( (prof->head.active == TRUE)
#if (WLAN_WORK_TIME == FYES)
		&& (prof->time_active == 1)
#endif
	  )
	{
	    while(waitCount < 10) {
		if (-1 == getIfMac(if_name, wifiMac)){/*¸ÕÅäÖÃÒ³Ãæºó ÎÞÏß½Ó¿Ú¿ÉÄÜ»¹Î´ÆðÀ´*/
		    sleep(1);
		    waitCount++;
		} else {
		    break;
		}
	    }
	    if(waitCount == 10) {
		wifiMac[0] = '\0';
	    }
	}
    }
    return 0;
}

/********************************************************************
 * º¯Êý£º getWifiInfo
 * ¹¦ÄÜ£º »ñÈ¡ÎÞÏßÍøÂçÐÅÏ¢
 * ´´½¨£º 2013-12-20
 * ²ÎÊý£º ÂÔ
 * ·µ»Ø£º ÎÞ
 * ×÷Õß£º zhao.chong 
 ********************************************************************/ 
static int  getWifiInfo(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;
    char wifiMac[18], if_name[5];
    unsigned int switch5G = 0U;/*5GÍøÂç¿ª¹Ø£¬ÓÃÓÚ¿ØÖÆÒ³ÃæÉÏµÄÏà¹ØÐÅÏ¢ÏÔÊ¾*/

#if(WIRELESS_5G == FYES)
    WirelessProfile *prof5G= NULL;
    switch5G = 1U;
#endif
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,0);
    if(prof != NULL){
	/*Êä³ö2.4GÎÞÏßÍøÂçÉèÖÃÐÅÏ¢*/
	if (prof->head.active == 1) {
	    websWrite(wp,"var WrlessEnables=\"%s\";",T("on"));
	}
	else{
	    websWrite(wp,"var WrlessEnables=\"%s\";",T("off"));
	}
	websWrite(wp,"var ssids=\"%s\";",prof->mBaseCf.SSID1);
	memset(wifiMac,0,sizeof(wifiMac));
	memset(if_name,0,sizeof(if_name));
	strncpy(if_name,"ra0",sizeof(if_name));
	getWifiMac(prof, if_name, wifiMac);/*»ñÈ¡2.4GÎÞÏßMAC*/
	websWrite(wp,"var wlanCurMac=\"%s\";",wifiMac);/*Êä³ö2.4GÎÞÏßmacµØÖ·*/
	websWrite(wp,"var channels=\"%d\";",prof->mBaseCf.Channel);/*Êä³ö2.4GÎÞÏßÐÅµÀ*/
    }
#if(WIRELESS_5G == FYES)
    prof5G = (WirelessProfile*)ProfGetInstPointByIndex(mibType,1);
    if(prof5G != NULL){
	/*Êä³ö5GÎÞÏßÍøÂçÉèÖÃÐÅÏ¢*/
	if (prof5G->head.active == 1) {
	    websWrite(wp,"var WrlessEnables_5g=\"%s\";",T("on"));
	}
	else{
	    websWrite(wp,"var WrlessEnables_5g=\"%s\";",T("off"));
	}
	websWrite(wp,"var ssids_5g=\"%s\";",prof5G->mBaseCf.SSID1);
	memset(wifiMac,0,sizeof(wifiMac));
	memset(if_name,0,sizeof(if_name));
	strncpy(if_name,"rai0",sizeof(if_name));
	getWifiMac(prof5G, if_name, wifiMac);/*»ñÈ¡5GÎÞÏßMAC*/
	websWrite(wp,"var wlanCurMac_5g=\"%s\";",wifiMac);/*Êä³ö5GÎÞÏßmacµØÖ·*/
	websWrite(wp,"var channels_5g=\"%d\";",prof5G->mBaseCf.Channel);/*Êä³ö5GÎÞÏßÐÅµÀ*/
    }
#endif
    websWrite(wp,"var switch5G=\"%d\";",switch5G);/*Êä³ö5GÎÞÏßÐÅÏ¢ÏÔÊ¾¿ª¹Ø*/
    return 0;
}
#endif

#if(WIRELESS_5G == FYES)
static int  getWlanCurrentMac_5g(int eid, webs_t wp, int argc, char_t **argv)
{
    char if_hw[18] = {0};
    int waitCount = 0;
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,1);

    if(ProfInstIsFree(prof) == PROFOK)
    {
        /*Èô²»¿ªÆôÎÞÏß¹¦ÄÜ ÔòÎÞÏß ½Ó¿ÚmacÊÇ¶Á²»µ½µÄ¡£¹ÊÎÞÐèµÈ´ý*/
	if( (prof->head.active == TRUE)
#if (WLAN_WORK_TIME == FYES)
		&& (prof->time_active == 1)
#endif
	  )
	{
#if 0
	    /*wirelessBasic×îºóÈôÕë¶Ô12771ÎÊÌâ Ë¯Ãß3s¡£Ôò´Ë´¦¾Í²»ÐèÒª10sÕâÃ´¾ÃÁË*/
#endif
	    while(waitCount < 10)
	    {
		if (-1 == getIfMac("rai0", if_hw))
		{/*12771 : ¸ÕÅäÖÃÒ³Ãæºó ÎÞÏß½Ó¿Ú¿ÉÄÜ»¹Î´ÆðÀ´*/
		    sleep(1);
		    waitCount++;
		}
		else 
		{
		    break;
		}
	    }
	    if(waitCount == 10) 
	    {
		if_hw[0] = '\0';
	    }
	}
    }
    return websWrite(wp, T("%s"), if_hw);
}
#endif

#if(WIRELESS_5G == FYES)


static int  ApScan5G(int eid, webs_t wp, int argc, char_t **argv)
{
	system("iwpriv rai0 set SiteSurvey=1");

    getTheErrorMsg(eid, wp, argc, argv);

	return 0;
}

static int  aspOutPutApScan5G(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	char line[128], ssid[33], bssid[24], encrypt[30];
	unsigned int ch = 0u, signal = 0u;
	int total = 0, n = 0;

	websWrite(wp , "var bssids = new Array();\n");
	websWrite(wp , "var ssids = new Array();\n");
	websWrite(wp , "var signalStrength = new Array();\n");
	websWrite(wp , "var channels = new Array();\n");
	websWrite(wp , "var Encrypts = new Array();\n");
	websWrite(wp , "var choice = new Array();\n");

//	system("iwpriv rai0 set SiteSurvey=1");
//	sleep(10);
	fp = popen("iwpriv rai0 get_site_survey", "r");
	if (fp != NULL) {
		fgets(line, sizeof(line), fp);
		fgets(line, sizeof(line), fp);

		while (fgets(line, sizeof(line), fp) != NULL) {
			memset(bssid, 0, sizeof(bssid));
			memset(ssid, 0, sizeof(ssid));
			memset(encrypt, 0, sizeof(encrypt));

			n = sscanf(line, "%u %s %s %s %u", &ch, ssid, bssid, encrypt, &signal);
			if (n < 5 || (strchr(ssid,':') != NULL) || (strchr(bssid,':') == NULL)) {
				continue;
			}

			delMacColon(bssid, ':');

			websWrite(wp, "bssids[%d] = \"%s\";\n", total, bssid);
			websWrite(wp, "ssids[%d] = \"%s\";\n", total, ssid);
			websWrite(wp, "signalStrength[%d] = \"%d%%\";\n", total, signal);
			websWrite(wp, "channels[%d] = \"%d\";\n", total, ch);

			if (strncmp(encrypt, "NONE", 4) == 0) {
				websWrite(wp, "Encrypts[%d] = \"%s\";\n", total, "·ñ");
			} else {
				websWrite(wp, "Encrypts[%d] = \"%s\";\n", total, "ÊÇ");
			}

			// ´ËÐÐÓ¦¸ÃÓÉÒ³ÃæÐ´Èë
			websWrite(wp, "choice[%d] = \"%s\";\n", total, "Ñ¡Ôñ");
			total++;
		}

		pclose(fp);
	}

    websWrite(wp,"var totalrecs = %d;\n", total);
    websWrite(wp,"var max_totalrecs = %d;\n", total);

    getTheErrorMsg(eid, wp, argc, argv);

	return 0;
}
#endif

static   void  restartWireless(webs_t wp, char_t *path, char_t *query)
{
	    #if(WIRELESS_5G == FYES)
	    system("wlan_5g.sh");
	    #endif
	     #if(WIRELESS  == FYES)
	    system("wlan.sh"); 
	    #endif
	    sleep(2);  
            websDone(wp, 200); 
}
extern void formDefineWireBaseConfig(void) {
    websFormDefine(T("ConfigWirelessBase"), wirelessBasic);
    websAspDefine(T("outputWlanBaseList"), getWirelessBaseConfig);
    websAspDefine(T("getWlanCurrentMac"), getWlanCurrentMac);
    websFormDefine(T("restartWireless"), restartWireless);
#if 0
    websAspDefine(T("outputWlanBaseInfo"), getWirelessBaseInfo);
#endif
#if(WIRELESS_5G == FYES)
    websAspDefine(T("outputWlanBaseList_5g"), getWirelessBaseConfig_5g);
    websAspDefine(T("aspOutPutApScan5G"), aspOutPutApScan5G);
    websAspDefine(T("ApScan5G"), ApScan5G);
    websFormDefine(T("ConfigWirelessBase_5g"), wirelessBasic_5g);
    websAspDefine(T("getWlanCurrentMac_5g"), getWlanCurrentMac_5g);
#endif
#if (UTT_SMART_UI == FYES)
    websFormDefine(T("ConfigWirelessBase_Sui"), wirelessBasic_Sui);
    websFormDefine(T("formWlanWizard_Sui"), formWlanWizard_Sui);
#if (WIRELESS_5G == FYES)
    websFormDefine(T("formWlanWizard_Sui_5g"), formWlanWizard_Sui_5g);
    websAspDefine(T("aspoutWlanWizard_Sui_5g"), aspoutWlanWizard_Sui_5g);
#endif
    websAspDefine(T("aspoutWlanWizard_Sui"), aspoutWlanWizard_Sui);
    websFormDefine(T("formApScan"), formApScan);
    websFormDefine(T("formApScanResult"), formApScanResult);
    websFormDefine(T("formApcliConnStatus"), formApcliConnStatus);
    websAspDefine(T("getWifiInfo"), getWifiInfo);
#if (WLAN_WORK_TIME == FYES)
    websAspDefine(T("outputWlanTimeConfig"), getWirelessTimeConfig);
    websFormDefine(T("ConfigWirelessTime_Sui"), wirelessTime_Sui);
#endif
#endif
    websAspDefine(T("ApScan"), ApScan);
    websAspDefine(T("aspOutPutApScan"), aspOutPutApScan);
#if (FEATURE_AP == FYES)
    websAspDefine(T("aspOutWifiMaxRate"), aspOutWifiMaxRate);
#endif
#if (UTT_BRIDEG_MODE_FLAG == FYES)
    websFormDefine(T("Formchangemodel"), Formchangemodel);
    websAspDefine(T("outputChangemodel"), outputChangemodel);
#endif
    
#if (APP_ELINKC == FYES)
    websAspDefine(T("aspOutPutElinkc"),aspOutPutElinkc);
    websFormDefine(T("FormElinkc"),FormElinkc);
#endif
}
#endif
