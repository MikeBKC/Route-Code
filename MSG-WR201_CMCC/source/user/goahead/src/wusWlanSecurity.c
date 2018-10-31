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


#ifdef  WIRE_SECURITY_DBG
#define SECURITY_PRNTF(fmt, args...)                printf(fmt, ## args)  
#else
#define SECURITY_PRNTF (fmt, args...)   
#endif

#if (WIRELESS == FYES)
void clearRadiusSet(WirelessProfile *prof)
{
    prof->mSafeCf.ap.AuthMode.RADIUS_Server  = 0u;
    prof->mSafeCf.ap.AuthMode.RADIUS_Port = 1812u;
    memset(prof->mSafeCf.ap.AuthMode.RADIUS_Key,0,sizeof(prof->mSafeCf.ap.AuthMode.RADIUS_Key)+1);
}

void  updateFlash8021x(WirelessProfile *prof) {
    int OperationMode;
    char lan_if_addr[16];
    struct in_addr ip;
    OperationMode = prof->mBaseCf.OperationMode;
    if(OperationMode == 0)
    {
	return;
    }
    if(prof->mSafeCf.ap.AuthMode.RADIUS_Server == 0)
    {
	return;
    }

    if (getIfIp(getLanIfName(), lan_if_addr) == -1)
    {
	return;
    }
    if (0 == inet_aton(lan_if_addr, &ip)) {
	return;
    }    
    prof->mBaseCf.own_ip_addr  = ip.s_addr;
    strcpy(prof->mBaseCf.EAPifname, getLanIfName());
    strcpy(prof->mBaseCf.PreAuthifname, getLanIfName());
}
void  conf8021x(WirelessProfile *prof,webs_t wp)
{
    char_t *RadiusServerIP, *RadiusServerPort, *RadiusServerSecret;
    const char * ErrMsg = NULL;
    struct in_addr ip;

    RadiusServerIP = websGetVar(wp, T("radiusIP"), T(""));
    RadiusServerPort = websGetVar(wp, T("radiusPort"), T("1812"));
    RadiusServerSecret = websGetVar(wp, T("radiusPsswd"), T(""));

    if (0 == inet_aton(RadiusServerIP, &ip)) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_RADIUS_INVAL_SER_IP);
	setTheErrorMsg(ErrMsg);
    }    
    else {
	prof->mSafeCf.ap.AuthMode.RADIUS_Server  = ip.s_addr;
	prof->mSafeCf.ap.AuthMode.RADIUS_Port = strtoul(RadiusServerPort,0,10);
	strcpy(prof->mSafeCf.ap.AuthMode.RADIUS_Key,RadiusServerSecret);
	updateFlash8021x(prof);
    }
}
#if (UTT_SSID2_ENCRYPTION == FYES)
void  conf8021x_2(WirelessProfile *prof,webs_t wp)
{
    char_t *RadiusServerIP_2, *RadiusServerPort_2, *RadiusServerSecret_2;
    const char * ErrMsg = NULL;
    struct in_addr ip;

    RadiusServerIP_2 = websGetVar(wp, T("radiusIP_2"), T(""));
    RadiusServerPort_2 = websGetVar(wp, T("radiusPort_2"), T("1812"));
    RadiusServerSecret_2 = websGetVar(wp, T("radiusPsswd_2"), T(""));

    if (0 == inet_aton(RadiusServerIP_2, &ip)) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_RADIUS_INVAL_SER_IP);
	setTheErrorMsg(ErrMsg);
    }    
    else {
	prof->mSafeCf.ap.AuthMode_2.RADIUS_Server  = ip.s_addr;
	prof->mSafeCf.ap.AuthMode_2.RADIUS_Port = strtoul(RadiusServerPort_2,0,10);
	strcpy(prof->mSafeCf.ap.AuthMode_2.RADIUS_Key,RadiusServerSecret_2);
	updateFlash8021x(prof);
    }
}
#endif

static void APSecurity(webs_t wp, char_t *path, char_t *query)
{
    char_t *DefaultKeyID, *Key1Str,  *Key2Str, *Key3Str,  *Key4Str;
    char_t *keyFormat,*keylen1,*keylen2,*keylen3,*keylen4;
    char_t *security_mode,*keyUpdateCyc,*pskPsswds,*cipher;

    MibProfileType mibType = MIB_PROF_WIRELESS;
    ProfChangeType profAction;
    struProfAlloc *profhead = NULL;
    WirelessProfile *prof= NULL;

    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,0);
    if(prof == NULL){
	return;
    } 
    profAction = PROF_CHANGE_EDIT;
    ProfBackupByIndex(mibType, profAction, 0, &profhead);

    security_mode = websGetVar(wp, T("security_mode"), T(""));
    /*	clear Radius settings  */
    clearRadiusSet(prof);

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

    //printf("DefaultKeyID = %s, Key1Str = %s,  Key2Str = %s, Key3Str = %s,  Key4Str = %s\n",DefaultKeyID, Key1Str,  Key2Str, Key3Str, Key4Str);
    //printf("keyFormat= %s, keylen1 = %s,  keylen2 = %s, keylen3 = %s,  keylen4 = %s\n",keyFormat, keylen1 ,  keylen2, keylen3, keylen4);
    //printf("security_mode = %s keyUpdateCyc = %s pskPsswd  = %s cipher = %s\n",security_mode,keyUpdateCyc,pskPsswds,cipher);
    /* ------------------Disable Mode --------------*/
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
	prof->mSafeCf.ap.AuthMode.wepkeyLength1 = strtoul(keylen1, 0, 10);
	prof->mSafeCf.ap.AuthMode.wepkeyLength2 = strtoul(keylen2, 0, 10);
	prof->mSafeCf.ap.AuthMode.wepkeyLength3 = strtoul(keylen3, 0, 10);
	prof->mSafeCf.ap.AuthMode.wepkeyLength4 = strtoul(keylen4, 0, 10);
	prof->mSafeCf.ap.AuthMode.wepKeyType = strtoul(keyFormat, 0, 10);
    }else if(!strcmp(security_mode, "WPA") || !strcmp(security_mode, "WPA1WPA2")||!strcmp(security_mode, "WPA2") ){	
	/* !------------------WPA Enterprise Mode ----------------*/
	/*set radius para	    WPA1WPA2 mixed mode */
	strcpy(prof->mSafeCf.SelAuthMode, security_mode);
	conf8021x(prof, wp);	
	strcpy(prof->mSafeCf.ap.AuthMode.EncrypType,cipher);
	prof->mSafeCf.ap.AuthMode.RekeyInterval = strtoul(keyUpdateCyc, 0, 10);
	prof->mSafeCf.ap.AuthMode.IEEE8021X = 0;
    }else if( !strcmp(security_mode, "WPAPSK")||!strcmp(security_mode, "WPA2PSK") ||    !strcmp(security_mode, "WPAPSKWPA2PSK") ){
	strcpy(prof->mSafeCf.ap.AuthMode.EncrypType,cipher);
	prof->mSafeCf.ap.AuthMode.RekeyInterval = strtoul(keyUpdateCyc, 0, 10);
	prof->mSafeCf.ap.AuthMode.IEEE8021X = 0;
	strcpy(prof->mSafeCf.ap.AuthMode.pskPsswd, pskPsswds);
	strcpy(prof->mSafeCf.SelAuthMode, security_mode);
    }
    else
    {; }
    /*                          */
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    websRedirect(wp,"WlanSecurity.asp");

}
static int getWirelessSecurityConfig(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,0);
    if(prof == NULL){
	return 0;
    }

    //int mssid=0;
    websWrite(wp,"var profflag=\"%d\";", prof->profflag);
    websWrite(wp,"var AuthModes=\"%s\";", prof->mSafeCf.SelAuthMode);
    websWrite(wp,"var keyFormats=\"%d\";",prof->mSafeCf.ap.AuthMode.wepKeyType);
    websWrite(wp,"var keyLength1s=\"%d\";",prof->mSafeCf.ap.AuthMode.wepkeyLength1);
    websWrite(wp,"var keyLength2s=\"%d\";",prof->mSafeCf.ap.AuthMode.wepkeyLength2);
    websWrite(wp,"var keyLength3s=\"%d\";",prof->mSafeCf.ap.AuthMode.wepkeyLength3);
    websWrite(wp,"var keyLength4s=\"%d\";",prof->mSafeCf.ap.AuthMode.wepkeyLength4);
    websWrite(wp,"var wepkey1s=\"%s\";",prof->mSafeCf.ap.AuthMode.wepkey1);
    websWrite(wp,"var wepkey2s=\"%s\";",prof->mSafeCf.ap.AuthMode.wepkey2);
    websWrite(wp,"var wepkey3s=\"%s\";",prof->mSafeCf.ap.AuthMode.wepkey3);
    websWrite(wp,"var wepkey4s=\"%s\";",prof->mSafeCf.ap.AuthMode.wepkey4);
    websWrite(wp,"var keynums=\"%d\";",prof->mSafeCf.ap.AuthMode.wepKeyID);
    websWrite(wp,"var wpaCiphers=\"%s\";",prof->mSafeCf.ap.AuthMode.EncrypType);
    websWrite(wp,"var radiusIPs=\"%N\";",prof->mSafeCf.ap.AuthMode.RADIUS_Server);
    websWrite(wp,"var radiusPorts=\"%d\";",prof->mSafeCf.ap.AuthMode.RADIUS_Port);
    websWrite(wp,"var radiusPsswds=\"%s\";",prof->mSafeCf.ap.AuthMode.RADIUS_Key);
    websWrite(wp,"var keyUpdateCycs=\"%d\";",prof->mSafeCf.ap.AuthMode.RekeyInterval);
    websWrite(wp,"var wpapskCiphers=\"%s\";",prof->mSafeCf.ap.AuthMode.EncrypType);
    websWrite(wp,"var pskPsswds=\"%s\";",prof->mSafeCf.ap.AuthMode.pskPsswd);
    websWrite(wp,"var pskkeyUpdateCycs=\"%d\";",prof->mSafeCf.ap.AuthMode.RekeyInterval);
#if (UTT_SSID2_ENCRYPTION == FYES)
    websWrite(wp,"var AuthModes_2=\"%s\";", prof->mSafeCf.SelAuthMode_2);
    websWrite(wp,"var keyFormats_2=\"%d\";",prof->mSafeCf.ap.AuthMode_2.wepKeyType);
    websWrite(wp,"var keyLength1s_2=\"%d\";",prof->mSafeCf.ap.AuthMode_2.wepkeyLength1);
    websWrite(wp,"var keyLength2s_2=\"%d\";",prof->mSafeCf.ap.AuthMode_2.wepkeyLength2);
    websWrite(wp,"var keyLength3s_2=\"%d\";",prof->mSafeCf.ap.AuthMode_2.wepkeyLength3);
    websWrite(wp,"var keyLength4s_2=\"%d\";",prof->mSafeCf.ap.AuthMode_2.wepkeyLength4);
    websWrite(wp,"var wepkey1s_2=\"%s\";",prof->mSafeCf.ap.AuthMode_2.wepkey1);
    websWrite(wp,"var wepkey2s_2=\"%s\";",prof->mSafeCf.ap.AuthMode_2.wepkey2);
    websWrite(wp,"var wepkey3s_2=\"%s\";",prof->mSafeCf.ap.AuthMode_2.wepkey3);
    websWrite(wp,"var wepkey4s_2=\"%s\";",prof->mSafeCf.ap.AuthMode_2.wepkey4);
    websWrite(wp,"var keynums_2=\"%d\";",prof->mSafeCf.ap.AuthMode_2.wepKeyID);
    websWrite(wp,"var wpaCiphers_2=\"%s\";",prof->mSafeCf.ap.AuthMode_2.EncrypType);
    websWrite(wp,"var radiusIPs_2=\"%N\";",prof->mSafeCf.ap.AuthMode_2.RADIUS_Server);
    websWrite(wp,"var radiusPorts_2=\"%d\";",prof->mSafeCf.ap.AuthMode_2.RADIUS_Port);
    websWrite(wp,"var radiusPsswds_2=\"%s\";",prof->mSafeCf.ap.AuthMode_2.RADIUS_Key);
    websWrite(wp,"var wpapskCiphers_2=\"%s\";",prof->mSafeCf.ap.AuthMode_2.EncrypType);
    websWrite(wp,"var pskPsswds_2=\"%s\";",prof->mSafeCf.ap.AuthMode_2.pskPsswd);
    /*key更新周期跟ssid1采用同一个*/
    websWrite(wp,"var keyUpdateCycs_2=\"%d\";",prof->mSafeCf.ap.AuthMode_2.RekeyInterval);
    websWrite(wp,"var pskkeyUpdateCycs_2=\"%d\";",prof->mSafeCf.ap.AuthMode_2.RekeyInterval);
#endif
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

#if(WIRELESS_5G == FYES)
static void APSecurity_5g(webs_t wp, char_t *path, char_t *query)
{
    char_t *DefaultKeyID, *Key1Str,  *Key2Str, *Key3Str,  *Key4Str;
    char_t *keyFormat,*keylen1,*keylen2,*keylen3,*keylen4;
    char_t *security_mode,*keyUpdateCyc,*pskPsswds,*cipher;

    MibProfileType mibType = MIB_PROF_WIRELESS;
    ProfChangeType profAction;
    struProfAlloc *profhead = NULL;
    WirelessProfile *prof= NULL;

    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,1);
    if(prof == NULL){
	return;
    } 
    profAction = PROF_CHANGE_EDIT;
    ProfBackupByIndex(mibType, profAction, 1, &profhead);

    security_mode = websGetVar(wp, T("security_mode"), T(""));
    /*	clear Radius settings  */
    clearRadiusSet(prof);

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

    //printf("DefaultKeyID = %s, Key1Str = %s,  Key2Str = %s, Key3Str = %s,  Key4Str = %s\n",DefaultKeyID, Key1Str,  Key2Str, Key3Str, Key4Str);
    //printf("keyFormat= %s, keylen1 = %s,  keylen2 = %s, keylen3 = %s,  keylen4 = %s\n",keyFormat, keylen1 ,  keylen2, keylen3, keylen4);
    //printf("security_mode = %s keyUpdateCyc = %s pskPsswd  = %s cipher = %s\n",security_mode,keyUpdateCyc,pskPsswds,cipher);
    /* ------------------Disable Mode --------------*/
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
	prof->mSafeCf.ap.AuthMode.wepkeyLength1 = strtoul(keylen1, 0, 10);
	prof->mSafeCf.ap.AuthMode.wepkeyLength2 = strtoul(keylen2, 0, 10);
	prof->mSafeCf.ap.AuthMode.wepkeyLength3 = strtoul(keylen3, 0, 10);
	prof->mSafeCf.ap.AuthMode.wepkeyLength4 = strtoul(keylen4, 0, 10);
	prof->mSafeCf.ap.AuthMode.wepKeyType = strtoul(keyFormat, 0, 10);
    }else if(!strcmp(security_mode, "WPA") || !strcmp(security_mode, "WPA1WPA2")||!strcmp(security_mode, "WPA2") ){	
	/* !------------------WPA Enterprise Mode ----------------*/
	/*set radius para	    WPA1WPA2 mixed mode */
	strcpy(prof->mSafeCf.SelAuthMode, security_mode);
	conf8021x(prof, wp);	
	strcpy(prof->mSafeCf.ap.AuthMode.EncrypType,cipher);
	prof->mSafeCf.ap.AuthMode.RekeyInterval = strtoul(keyUpdateCyc, 0, 10);
	prof->mSafeCf.ap.AuthMode.IEEE8021X = 0;
    }else if( !strcmp(security_mode, "WPAPSK")||!strcmp(security_mode, "WPA2PSK") ||    !strcmp(security_mode, "WPAPSKWPA2PSK") ){
	strcpy(prof->mSafeCf.ap.AuthMode.EncrypType,cipher);
	prof->mSafeCf.ap.AuthMode.RekeyInterval = strtoul(keyUpdateCyc, 0, 10);
	prof->mSafeCf.ap.AuthMode.IEEE8021X = 0;
	strcpy(prof->mSafeCf.ap.AuthMode.pskPsswd, pskPsswds);
	strcpy(prof->mSafeCf.SelAuthMode, security_mode);
    }
    else
    {; }
    /*                          */
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    websRedirect(wp,"WlanSecurity_5g.asp");

}
#endif

#if(WIRELESS_5G == FYES)
static int getWirelessSecurityConfig_5g(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,1);
    if(prof == NULL){
	return 0;
    }

    //int mssid=0;
    
    websWrite(wp,"var AuthModes=\"%s\";", prof->mSafeCf.SelAuthMode);
    websWrite(wp,"var profflag=\"%d\";", prof->profflag); 
    websWrite(wp,"var keyFormats=\"%d\";",prof->mSafeCf.ap.AuthMode.wepKeyType);
    websWrite(wp,"var keyLength1s=\"%d\";",prof->mSafeCf.ap.AuthMode.wepkeyLength1);
    websWrite(wp,"var keyLength2s=\"%d\";",prof->mSafeCf.ap.AuthMode.wepkeyLength2);
    websWrite(wp,"var keyLength3s=\"%d\";",prof->mSafeCf.ap.AuthMode.wepkeyLength3);
    websWrite(wp,"var keyLength4s=\"%d\";",prof->mSafeCf.ap.AuthMode.wepkeyLength4);
    websWrite(wp,"var wepkey1s=\"%s\";",prof->mSafeCf.ap.AuthMode.wepkey1);
    websWrite(wp,"var wepkey2s=\"%s\";",prof->mSafeCf.ap.AuthMode.wepkey2);
    websWrite(wp,"var wepkey3s=\"%s\";",prof->mSafeCf.ap.AuthMode.wepkey3);
    websWrite(wp,"var wepkey4s=\"%s\";",prof->mSafeCf.ap.AuthMode.wepkey4);
    websWrite(wp,"var keynums=\"%d\";",prof->mSafeCf.ap.AuthMode.wepKeyID);
    websWrite(wp,"var wpaCiphers=\"%s\";",prof->mSafeCf.ap.AuthMode.EncrypType);
    websWrite(wp,"var radiusIPs=\"%N\";",prof->mSafeCf.ap.AuthMode.RADIUS_Server);
    websWrite(wp,"var radiusPorts=\"%d\";",prof->mSafeCf.ap.AuthMode.RADIUS_Port);
    websWrite(wp,"var radiusPsswds=\"%s\";",prof->mSafeCf.ap.AuthMode.RADIUS_Key);
    websWrite(wp,"var keyUpdateCycs=\"%d\";",prof->mSafeCf.ap.AuthMode.RekeyInterval);
    websWrite(wp,"var wpapskCiphers=\"%s\";",prof->mSafeCf.ap.AuthMode.EncrypType);
    websWrite(wp,"var pskPsswds=\"%s\";",prof->mSafeCf.ap.AuthMode.pskPsswd);
    websWrite(wp,"var pskkeyUpdateCycs=\"%d\";",prof->mSafeCf.ap.AuthMode.RekeyInterval);
#if (UTT_SSID2_ENCRYPTION == FYES)
    websWrite(wp,"var AuthModes_2=\"%s\";", prof->mSafeCf.SelAuthMode_2);
    websWrite(wp,"var keyFormats_2=\"%d\";",prof->mSafeCf.ap.AuthMode_2.wepKeyType);
    websWrite(wp,"var keyLength1s_2=\"%d\";",prof->mSafeCf.ap.AuthMode_2.wepkeyLength1);
    websWrite(wp,"var keyLength2s_2=\"%d\";",prof->mSafeCf.ap.AuthMode_2.wepkeyLength2);
    websWrite(wp,"var keyLength3s_2=\"%d\";",prof->mSafeCf.ap.AuthMode_2.wepkeyLength3);
    websWrite(wp,"var keyLength4s_2=\"%d\";",prof->mSafeCf.ap.AuthMode_2.wepkeyLength4);
    websWrite(wp,"var wepkey1s_2=\"%s\";",prof->mSafeCf.ap.AuthMode_2.wepkey1);
    websWrite(wp,"var wepkey2s_2=\"%s\";",prof->mSafeCf.ap.AuthMode_2.wepkey2);
    websWrite(wp,"var wepkey3s_2=\"%s\";",prof->mSafeCf.ap.AuthMode_2.wepkey3);
    websWrite(wp,"var wepkey4s_2=\"%s\";",prof->mSafeCf.ap.AuthMode_2.wepkey4);
    websWrite(wp,"var keynums_2=\"%d\";",prof->mSafeCf.ap.AuthMode_2.wepKeyID);
    websWrite(wp,"var wpaCiphers_2=\"%s\";",prof->mSafeCf.ap.AuthMode_2.EncrypType);
    websWrite(wp,"var radiusIPs_2=\"%N\";",prof->mSafeCf.ap.AuthMode_2.RADIUS_Server);
    websWrite(wp,"var radiusPorts_2=\"%d\";",prof->mSafeCf.ap.AuthMode_2.RADIUS_Port);
    websWrite(wp,"var radiusPsswds_2=\"%s\";",prof->mSafeCf.ap.AuthMode_2.RADIUS_Key);
    websWrite(wp,"var wpapskCiphers_2=\"%s\";",prof->mSafeCf.ap.AuthMode_2.EncrypType);
    websWrite(wp,"var pskPsswds_2=\"%s\";",prof->mSafeCf.ap.AuthMode_2.pskPsswd);
    /*key更新周期跟ssid1采用同一个*/
    websWrite(wp,"var keyUpdateCycs_2=\"%d\";",prof->mSafeCf.ap.AuthMode_2.RekeyInterval);
    websWrite(wp,"var pskkeyUpdateCycs_2=\"%d\";",prof->mSafeCf.ap.AuthMode_2.RekeyInterval);
#endif
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
#endif

extern void formDefineWirelessSecurity(void) {
    websAspDefine(T("aspOutWlanSecurityConfig"), getWirelessSecurityConfig);
    websFormDefine(T("APSecurity"), APSecurity);

#if(WIRELESS_5G == FYES)
    websAspDefine(T("aspOutWlanSecurityConfig_5g"), getWirelessSecurityConfig_5g);
    websFormDefine(T("APSecurity_5g"), APSecurity_5g);
#endif
}
#endif
