#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uttMachine.h"
#include "typedef.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ifmisc.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "conver.h"
#include "uttfunction.h"
#if (UTT_SMART_UI == FYES || WLAN_MULTI_SSID == FYES)
#include <iconv-user.h>
char *skip_list[] = {
    "",//0
    "",//1
    "",//2
    "",//3
    "",//4
    "",//5
    "",//6
    "",//7
    "",//8
    "36;40;44;48;",//9
    "",//10
    "",//11
    "",//12
    "",//13
    "",//14
    "36;40;44;48;52;",//15
    "",//16
    ""//17
};
#endif
#include "mxml.h"

int uttcli_debug = 0;

char* int2ip(unsigned int lip,char * ip)
{
    unsigned char i=0;        
    char *tmp=ip;             

    while(i++<3)              
    {  
	sprintf(tmp,"%u.",(lip&0xff000000)>>24);
	lip=lip<<8;           
	tmp+=strlen(tmp);     
    }  
    sprintf(tmp,"%u",(lip&0xff000000)>>24);
    return ip;
}

/* Change to std mac  */
void Conversion2StdMac(char* mac)
{   
    char *p;
    p=mac+16;
    mac=mac+11;
    do
    {
	*p--=*mac--;
	*p--=*mac--;
	*p=':';
    }while(--p!=mac);
}
extern struct st_xmlConfig  rootConfig[];
extern void parseProfileValue(struct st_xmlConfig *conf, int instIndex, char* buf, Pointer prof);
/*
 * getÃüÁî»ñµÃÖ¸¶¨ÊµÀýµÄÊôÐÔÖµ
 * getbynm»ñÈ¡Ê±¡£¼´±ãÊµÀý×î´óÊýÎª1Ò²±ØÐëÊäÈëÊµÀýÃû£¨ÊÊÓÃÓÚÊµÀý1µ½¶à¿É±äµÄÇé¿ö£©
 * instÃüÁîÏÔÊ¾Ö¸¶¨¹¦ÄÜµÄËùÓÐÊµÀý,ÒÔ¿Õ¸ñ¼ä¸ô
 * set TODO
 */
void usage()
{
    printf("usage: \n");
    printf("    set [inst] [name] [param] [value]\n\
    getbynm [param]\n\
        get [param]\n\
        new [inst] [name]\n\
     delete [inst] [name]\n\
	make_ap_config wireless\n\
       inst [param]\n");
}

#if (WIRELESS == FYES)
#define AP_BUF_LEN  1024
char ap_conf_buf[AP_BUF_LEN + 1];
/*AP×î¶àÓÐ4¸ö·þÎñÇø£¬Ã¿¸ö·þÎñÇøÃ¿´Î×î¶àÏÂ·¢4¸ö¹ýÂËµØÖ·×é*/
typedef struct st_apMacMap{
    char sz[UTT_INST_NAME_LEN + 1];
	char filterName[4][UTT_INST_NAME_LEN + 1];
	int  rule;
}apMacMap;
apMacMap macMap[4];
/***********************************************************************
 *  º¯ÊýÃû£ºInttoApconf
 *  ²ÎÊý£ºkey : ÎÞÏßÄ£¿éµÄ²ÎÊýÃû  ±È½ÏµÄ²ÎÊýÃû  ´Óprofile»ñÈ¡µÄÖµ
 *  ·µ»ØÖµ£ºÎÞÏß²ÎÊýµÄ×Ö·û´®±äÁ¿
 *  ÃèÊö£º½«ÕûÐÎ×ª»¯ÎªÎÞÏß²ÎÊý
 ***********************************************************************/
static void InttoApconf(const char *key,const char *str,int profnum)
{
    if((key == NULL) || (str == NULL)) {
	return;
    }
    if(strcmp(key,str) == 0) {
	sprintf(ap_conf_buf,"%d",profnum);
    }
}
/***********************************************************************
 *  º¯ÊýÃû£ºDistoApconf
 *  ²ÎÊý£ºkey : ÎÞÏßÄ£¿éµÄ²ÎÊýÃû  ±È½ÏµÄ²ÎÊýÃû  ´Óprofile»ñÈ¡µÄÖµ
 *  ·µ»ØÖµ£ºÎÞÏß²ÎÊýµÄ×Ö·û´®±äÁ¿
 *  ÃèÊö:  ½«²¼¶û±äÁ¿0×ª»¯Îª1 1×ª»¯Îª0
 ***********************************************************************/
static void DistoApconf(const char *key,const char *str,int profnum)
{
    if((key == NULL) || (str == NULL)) {
	return;
    }
    if(strcmp(key,str) == 0) {
	if(profnum == 0) {
	    strcpy(ap_conf_buf,"1");
	}else if (profnum == 1){
	    strcpy(ap_conf_buf,"0");
	}else {; }
    }
}
/***********************************************************************
 *  º¯ÊýÃû£ºStrtoApconf
 *  ²ÎÊý£ºkey : ÎÞÏßÄ£¿éµÄ²ÎÊýÃû  ±È½ÏµÄ²ÎÊýÃû  ´Óprofile»ñÈ¡µÄÖµ
 *  ·µ»ØÖµ£ºÎÞÏß²ÎÊýµÄ×Ö·û´®±äÁ¿
 *  ÃèÊö£º½«×Ö·û´®×ª»¯ÎªÎÞÏß²ÎÊý
 ***********************************************************************/
#if (UTT_SMART_UI == FYES || WLAN_MULTI_SSID == FYES)
static void StrtoApconf(const char *key,const char *str,char *profstr)
{
    WirelessProfile *prof = NULL;
#if (MAKE_MONEY == FYES)
    MakeMoneyProfile *profM= NULL;
#endif
    unsigned char flag = 0U;
    MibProfileType mibType = MIB_PROF_WIRELESS;
    char *ssid = NULL;
#if (WIRELESS_5G == FYES)
    WirelessProfile *prof5G = NULL;
#endif
    if((key == NULL) || (str == NULL) || (profstr == NULL)) {
        return;
    }
    
    if(strcmp(key,str) == 0)
    {
        if (strcmp(str,"SSID1") == 0)
        {
            /*2.4GÍøÂç*/
            prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 0);
            if ((strcmp(prof->mBaseCf.SSID1,profstr) == 0) && (strcmp(prof->mBaseCf.encodeType,"GBK") != 0)) {
                flag = 1U;
            }
#if (WIRELESS_5G == FYES)
            else {
                prof5G = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 1);
                if ((strcmp(prof5G->mBaseCf.SSID1,profstr) == 0) && (strcmp(prof5G->mBaseCf.encodeType,"GBK") != 0)) {
                    flag = 1U;
                }
            }
#endif
            if (flag == 1U) {
                if (strcmp(prof->mBaseCf.SSID1,profstr) == 0) {
                    ssid = iconv_string(profstr,"GBK",prof->mBaseCf.encodeType);
                }
#if (WIRELESS_5G == FYES)
                else {
                    ssid = iconv_string(profstr,"GBK",prof5G->mBaseCf.encodeType);
                }
#endif
                if (ssid != NULL) {
                    strcpy(ap_conf_buf,ssid);
                    free(ssid);
                    ssid = NULL;
                } else {
                    strcpy(ap_conf_buf,profstr);
                }
            } else {
                strcpy(ap_conf_buf,profstr);
            }
#if (WLAN_MULTI_SSID == FYES)
        }
        else if (strcmp(str,"SSID2") == 0)
        {
            prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 0);
            if ((strcmp(prof->mBaseCf.SSID2,profstr) == 0) && (strcmp(prof->mBaseCf.encodeType2,"GBK") != 0)) {
                flag = 1U;
            }
#if (WIRELESS_5G == FYES)
            else {
                prof5G = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 1);
                if ((strcmp(prof5G->mBaseCf.SSID2,profstr) == 0) && (strcmp(prof5G->mBaseCf.encodeType2,"GBK") != 0)) {
                    flag = 1U;
                }
            }
#endif
            if (flag == 1U) {
                if (strcmp(prof->mBaseCf.SSID2,profstr) == 0) {
                    ssid = iconv_string(profstr,"GBK",prof->mBaseCf.encodeType2);
                }
#if (WIRELESS_5G == FYES)
                else {
                    ssid = iconv_string(profstr,"GBK",prof5G->mBaseCf.encodeType2);
                }
#endif
                if (ssid != NULL) {
                    strcpy(ap_conf_buf,ssid);
                    free(ssid);
                    ssid = NULL;
                } else {
                    strcpy(ap_conf_buf,profstr);
                }
            } else {
                strcpy(ap_conf_buf,profstr);
            }
#if (MIDDLE_SOFT == FYES)
        }
        else if (strcmp(str,"SSID3") == 0)
        {
            prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 0);
            if ((strcmp(prof->mBaseCf.SSID3,profstr) == 0) && (strcmp(prof->mBaseCf.encodeType3,"GBK") != 0)) {
                flag = 1U;
            }
#if 0
#if (WIRELESS_5G == FYES)
            else {
                prof5G = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 1);
                if ((strcmp(prof5G->mBaseCf.SSID2,profstr) == 0) && (strcmp(prof5G->mBaseCf.encodeType2,"GBK") != 0)) {
                    flag = 1U;
                }
            }
#endif
#endif
            if (flag == 1U) {
                if (strcmp(prof->mBaseCf.SSID3,profstr) == 0) {
                    ssid = iconv_string(profstr,"GBK",prof->mBaseCf.encodeType3);
                }
#if 0
#if (WIRELESS_5G == FYES)
                else {
                    ssid = iconv_string(profstr,"GBK",prof5G->mBaseCf.encodeType2);
                }
#endif
#endif
                if (ssid != NULL) {
                    strcpy(ap_conf_buf,ssid);
                    free(ssid);
                    ssid = NULL;
                } else {
                    strcpy(ap_conf_buf,profstr);
                }
            } else {
                strcpy(ap_conf_buf,profstr);
            }
        }else if (strcmp(str,"SSID4") == 0)
            {
                prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 0);
                if ((strcmp(prof->mBaseCf.SSID4,profstr) == 0) && (strcmp(prof->mBaseCf.encodeType4,"GBK") != 0)) {
                    flag = 1U;
                }
#if 0
#if (WIRELESS_5G == FYES)
                else {
                    prof5G = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 1);
                    if ((strcmp(prof5G->mBaseCf.SSID2,profstr) == 0) && (strcmp(prof5G->mBaseCf.encodeType2,"GBK") != 0)) {
                        flag = 1U;
                    }
                }
#endif
#endif
                if (flag == 1U) {
                    if (strcmp(prof->mBaseCf.SSID4,profstr) == 0) {
                        ssid = iconv_string(profstr,"GBK",prof->mBaseCf.encodeType4);
                    }
#if 0
#if (WIRELESS_5G == FYES)
                    else {
                        ssid = iconv_string(profstr,"GBK",prof5G->mBaseCf.encodeType2);
                    }
#endif
#endif
                    if (ssid != NULL) {
                        strcpy(ap_conf_buf,ssid);
                        free(ssid);
                        ssid = NULL;
                    } else {
                        strcpy(ap_conf_buf,profstr);
                    }
                } else {
                    strcpy(ap_conf_buf,profstr);
                }
#endif
#endif
#if (MAKE_MONEY == FYES)
	} else if (strcmp(str,"SSID3") == 0)
	{
	    profM = (MakeMoneyProfile*)ProfGetInstPointByIndex(MIB_PROF_MAKE_MONEY, 0);

	    if((strcmp(profM->ssid,profstr) == 0) && (strcmp(profM->encodeType,"GBK") != 0)) 
	    {
		ssid = iconv_string(profstr,"GBK",profM->encodeType);
	    }
	    else if((strcmp(profM->ssid_5g,profstr) == 0) && (strcmp(profM->encodeType_5g,"GBK") != 0))
	    {
		ssid = iconv_string(profstr,"GBK",profM->encodeType_5g);
	    }
	    else
	    {
		strcpy(ap_conf_buf,profstr);	
	    }
	   
	    if (ssid != NULL) {
                    strcpy(ap_conf_buf,ssid);
                    free(ssid);
                    ssid = NULL;
                } else {
                    strcpy(ap_conf_buf,profstr);
                } 
	} 
	else if (strcmp(str,"SSID4") == 0)
	{
	    profM = (MakeMoneyProfile*)ProfGetInstPointByIndex(MIB_PROF_MAKE_MONEY, 0);

	    if((strcmp(profM->ssid2,profstr) == 0) && (strcmp(profM->encodeType2,"GBK") != 0)) 
	    {
		ssid = iconv_string(profstr,"GBK",profM->encodeType2);
	    }
	    else if((strcmp(profM->ssid2_5g,profstr) == 0) && (strcmp(profM->encodeType2_5g,"GBK") != 0))
	    {
		ssid = iconv_string(profstr,"GBK",profM->encodeType2_5g);
	    }
	    else
	    {
		strcpy(ap_conf_buf,profstr);	
	    }
	   
	    if (ssid != NULL) {
                    strcpy(ap_conf_buf,ssid);
                    free(ssid);
                    ssid = NULL;
                } else {
                    strcpy(ap_conf_buf,profstr);
                } 
#endif
#if 0
/*xrg ÐÞ¸ÄÎªÈýÖÖ±àÂë¸ñÊ½*/
#if (MAKE_MONEY == FYES)
        } else if (strcmp(str,"SSID3") == 0) {
            profM = (MakeMoneyProfile*)ProfGetInstPointByIndex(MIB_PROF_MAKE_MONEY, 0);
                    ssid = iconv_string(profstr,"GBK","UTF-8");
                if (ssid != NULL) {
                    strcpy(ap_conf_buf,ssid);
                    free(ssid);
                    ssid = NULL;
                } else {
                    strcpy(ap_conf_buf,profstr);
                }    
#endif
#if (MAKE_MONEY == FYES)
        } else if (strcmp(str,"SSID4") == 0) { 
            profM = (MakeMoneyProfile*)ProfGetInstPointByIndex(MIB_PROF_MAKE_MONEY, 0);
                    ssid = iconv_string(profstr,"GB2312","GBK");
     
                if (ssid != NULL) {
                    strcpy(ap_conf_buf,ssid);
                    free(ssid);
                    ssid = NULL;
                } else {
                    strcpy(ap_conf_buf,profstr);
                }
#endif
#endif
        } else {
            strcpy(ap_conf_buf,profstr);
        }
    }
}
#else
static void StrtoApconf(const char *key,const char *str,char *profstr)
{
    if((key == NULL) || (str == NULL) || (profstr == NULL)) {
        return;
    }
    if(strcmp(key,str) == 0) {
        strcpy(ap_conf_buf,profstr);
    }
}
#endif
/***********************************************************************
 *  º¯ÊýÃû£ºMactoApconf
 *  ²ÎÊý£ºkey : ÎÞÏßÄ£¿éµÄ²ÎÊýÃû  ±È½ÏµÄ²ÎÊýÃû  ´Óprofile»ñÈ¡µÄÖµ
 *  ·µ»ØÖµ£ºÎÞÏß²ÎÊýµÄ×Ö·û´®±äÁ¿
 *  ÃèÊö£º½«MACµØÖ·×ª»¯ÎªÎÞÏß²ÎÊý
 ***********************************************************************/
static void MactoApconf(const char *key,const char *str,MacAddr macAddr)
{
    if((key == NULL) || (str == NULL) ) {
	return;
    }
    if(strcmp(key,str) == 0) {
	strcpy(ap_conf_buf,converMac6To17Byte(macAddr));
    }
}
/***********************************************************************
 *  º¯ÊýÃû£ºIptoApconf
 *  ²ÎÊý£ºkey : ÎÞÏßÄ£¿éµÄ²ÎÊýÃû  ±È½ÏµÄ²ÎÊýÃû  ´Óprofile»ñÈ¡µÄÖµ
 *  ·µ»ØÖµ£ºÎÞÏß²ÎÊýµÄ×Ö·û´®±äÁ¿
 *  ÃèÊö£º½«IpµØÖ·×ª»¯ÎªÎÞÏß²ÎÊý
 ***********************************************************************/
static void IptoApconf(const char *key,const char *str,UINT32 IP)
{
    struct in_addr sAddr;
    memset(&sAddr, 0, sizeof(sAddr));
    if((key == NULL) || (str == NULL) || (IP == 0)) {
	return;
    }
    if(strcmp(key,str) == 0) {
	sAddr.s_addr = IP;
	strcpy(ap_conf_buf,inet_ntoa(sAddr));
    }
}


/***********************************************************************
 *  º¯ÊýÃû£º	prof_to_apconf
 *  ²ÎÊý£º	key : ÎÞÏßÄ£¿éµÄ²ÎÊýÃû
 *  ·µ»ØÖµ£º    ÎÞÏß²ÎÊýkeyµÄÖµ
 *  ÃèÊö£º      ÎÞÏß²ÎÊýKEYµÄÖµ
 ***********************************************************************/
static char *prof_to_apconf(const char *key, int num) {
    char str[64];
#if (FEATURE_AP == FYES)
    int min, max, index, count=0;
    char wlanMac[1024] = {0};
    char *tmpmac = NULL;
#ifdef FIT_AP_WL_24G_PF_START
    unsigned int start_index = FIT_AP_WL_24G_PF_START;
#else
    unsigned int start_index = 1;
#endif

    memset(ap_conf_buf, 0, AP_BUF_LEN + 1);
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;
    WirelessMacFilterGlobalProfile *macFltGlb = NULL;
    WirelessMacFilterProfile *macFlt = NULL;
    MibProfileType mibTypeMacFlt = MIB_PROF_WIRELESS_MAC_FILTER;
    MibProfileType mibTypeMacFltGlb = MIB_PROF_WIRELESS_MAC_FILTER_GLOBAL;
    macFltGlb = (WirelessMacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeMacFltGlb, 0);
    FatFitSwitchProfile *apModeProf = NULL;
    MibProfileType apModeMibType = MIB_PROF_FAT_FIT_SWITCH;

#if (WIRELESS_5G == FYES)
    if (0 == num)
    {
	start_index = FIT_AP_WL_24G_PF_START;
	prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 0);
    }
    else if(1 == num)
    {
	start_index = FIT_AP_WL_5G_PF_START;
	prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 1);
    }
#else
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 0);
#endif

    apModeProf = (FatFitSwitchProfile *)ProfGetInstPointByIndex(apModeMibType, 0);
    if((prof == NULL) || (apModeProf == NULL)){
	return 0;
    }
    /*
     * ap Ö¸¶¨²ÎÊýµÄprofile×ª»»
     * ²ÎÊýÊÇ±£´æÔÚap_conf_bufÖÐ·µ»ØµÄ
     * ×¢Òâ£ºÈç¹ûap_conf_buf³¤¶È²»×ãÒÔ±£´æap²ÎÊý£¬ÔòÐèÒª¼Ó³¤
     */
    /* 
     eg:
     if (strcmp(key, "BssidNum") == 0) {
	    strncpy(ap_conf_buf, prof->bssidNum, AP_BUF_LEN);
     }
     */
    /*str			prof*/
    /*----------------------- base config------------------------------*/
    //RadioOn			active
    InttoApconf(key,"RadioOn",prof->head.active);

    int i, n=1;
    WirelessProfile *profW = NULL;
    WirelessProfile *prof1 = NULL;
    WirelessProfile *prof2 = NULL;
    WirelessProfile *prof3 = NULL;
    WirelessProfile *prof4 = NULL;
   
    prof1 = (WirelessProfile *)ProfGetInstPointByIndex(mibType, start_index);
    prof2 = (WirelessProfile *)ProfGetInstPointByIndex(mibType, start_index + 1);
    prof3 = (WirelessProfile *)ProfGetInstPointByIndex(mibType, start_index + 2);
    prof4 = (WirelessProfile *)ProfGetInstPointByIndex(mibType, start_index + 3);
    for (i=start_index; i<(start_index + MAX_FIT_AP_SERVICE_ZONE_PROFILES); i++)
    {
	profW = (WirelessProfile *)ProfGetInstPointByIndex(mibType, i);
	if ((profW != NULL) && (profW->head.active == TRUE))
	{
	    count++;
	}
    }

    if ((apModeProf->fatEnable == PROF_DISABLE) && (count > 0))	/* fit ap */
    {
	n = 1;
	for (i=start_index; i<(start_index + MAX_FIT_AP_SERVICE_ZONE_PROFILES); i++)
	{
	    profW = (WirelessProfile *)ProfGetInstPointByIndex(mibType, i);
	    if ((profW != NULL) && (profW->head.active == TRUE))
	    {
		memset(str, '\0', sizeof(str));
		sprintf(str,"SSID%d",n);
		n++;
		StrtoApconf(key,str,profW->mBaseCf.SSID1);
	    }
	}
	InttoApconf(key,"BssidNum",count);
	
	memset(str, '\0', sizeof(str));
	sprintf(str,"%d;%d;%d;%d", (prof1->mBaseCf.SSIDBroadCastEn == FUN_ENABLE ? 0:1), (prof2->mBaseCf.SSIDBroadCastEn == FUN_ENABLE ? 0:1), (prof3->mBaseCf.SSIDBroadCastEn == FUN_ENABLE ? 0:1), (prof4->mBaseCf.SSIDBroadCastEn == FUN_ENABLE ? 0:1));
	StrtoApconf(key,"HideSSID",str);
	
	memset(str, '\0', sizeof(str));
	sprintf(str,"%s;%s;%s;%s", prof1->mSafeCf.SelAuthMode, prof2->mSafeCf.SelAuthMode, prof3->mSafeCf.SelAuthMode, prof4->mSafeCf.SelAuthMode);
	StrtoApconf(key,"AuthMode",str);

	memset(str, '\0', sizeof(str));
	sprintf(str,"%s;%s;%s;%s",prof1->mSafeCf.ap.AuthMode.EncrypType, prof2->mSafeCf.ap.AuthMode.EncrypType, prof3->mSafeCf.ap.AuthMode.EncrypType, prof4->mSafeCf.ap.AuthMode.EncrypType);
	StrtoApconf(key,"EncrypType",str);

	memset(str, '\0', sizeof(str));
	sprintf(str,"%d;%d;%d;%d",prof1->mSafeCf.ap.AuthMode.wepKeyID, prof2->mSafeCf.ap.AuthMode.wepKeyID, prof3->mSafeCf.ap.AuthMode.wepKeyID, prof4->mSafeCf.ap.AuthMode.wepKeyID);
	StrtoApconf(key,"DefaultKeyID",str);
	
	StrtoApconf(key,"Key1Str1",prof1->mSafeCf.ap.AuthMode.wepkey1);
	StrtoApconf(key,"Key2Str1",prof1->mSafeCf.ap.AuthMode.wepkey2);
	StrtoApconf(key,"Key3Str1",prof1->mSafeCf.ap.AuthMode.wepkey3);
	StrtoApconf(key,"Key4Str1",prof1->mSafeCf.ap.AuthMode.wepkey4);
	StrtoApconf(key,"Key1Str2",prof2->mSafeCf.ap.AuthMode.wepkey1);
	StrtoApconf(key,"Key2Str2",prof2->mSafeCf.ap.AuthMode.wepkey2);
	StrtoApconf(key,"Key3Str2",prof2->mSafeCf.ap.AuthMode.wepkey3);
	StrtoApconf(key,"Key4Str2",prof2->mSafeCf.ap.AuthMode.wepkey4);
	StrtoApconf(key,"Key1Str3",prof3->mSafeCf.ap.AuthMode.wepkey1);
	StrtoApconf(key,"Key2Str3",prof3->mSafeCf.ap.AuthMode.wepkey2);
	StrtoApconf(key,"Key3Str3",prof3->mSafeCf.ap.AuthMode.wepkey3);
	StrtoApconf(key,"Key4Str3",prof3->mSafeCf.ap.AuthMode.wepkey4);
	StrtoApconf(key,"Key1Str4",prof4->mSafeCf.ap.AuthMode.wepkey1);
	StrtoApconf(key,"Key2Str4",prof4->mSafeCf.ap.AuthMode.wepkey2);
	StrtoApconf(key,"Key3Str4",prof4->mSafeCf.ap.AuthMode.wepkey3);
	StrtoApconf(key,"Key4Str4",prof4->mSafeCf.ap.AuthMode.wepkey4);

	memset(str, '\0', sizeof(str));
	sprintf(str,"%d;%d;%d;%d",prof1->mSafeCf.ap.AuthMode.wepKeyType,prof2->mSafeCf.ap.AuthMode.wepKeyType,prof3->mSafeCf.ap.AuthMode.wepKeyType,prof4->mSafeCf.ap.AuthMode.wepKeyType);
	StrtoApconf(key,"Key1Type",str);
	memset(str, '\0', sizeof(str));
	sprintf(str,"%d;%d;%d;%d",prof1->mSafeCf.ap.AuthMode.wepKeyType,prof2->mSafeCf.ap.AuthMode.wepKeyType,prof3->mSafeCf.ap.AuthMode.wepKeyType,prof4->mSafeCf.ap.AuthMode.wepKeyType);
	StrtoApconf(key,"Key2Type",str);
	memset(str, '\0', sizeof(str));
	sprintf(str,"%d;%d;%d;%d",prof1->mSafeCf.ap.AuthMode.wepKeyType,prof2->mSafeCf.ap.AuthMode.wepKeyType,prof3->mSafeCf.ap.AuthMode.wepKeyType,prof4->mSafeCf.ap.AuthMode.wepKeyType);
	StrtoApconf(key,"Key3Type",str);
	memset(str, '\0', sizeof(str));
	sprintf(str,"%d;%d;%d;%d",prof1->mSafeCf.ap.AuthMode.wepKeyType,prof2->mSafeCf.ap.AuthMode.wepKeyType,prof3->mSafeCf.ap.AuthMode.wepKeyType,prof4->mSafeCf.ap.AuthMode.wepKeyType);
	StrtoApconf(key,"Key4Type",str);

	StrtoApconf(key,"WPAPSK1",prof1->mSafeCf.ap.AuthMode.pskPsswd);
	StrtoApconf(key,"WPAPSK2",prof2->mSafeCf.ap.AuthMode.pskPsswd);
	StrtoApconf(key,"WPAPSK3",prof3->mSafeCf.ap.AuthMode.pskPsswd);
	StrtoApconf(key,"WPAPSK4",prof4->mSafeCf.ap.AuthMode.pskPsswd);
	
	memset(str, '\0', sizeof(str));
	sprintf(str,"%d;%d;%d;%d",prof1->mSafeCf.ap.AuthMode.IEEE8021X,prof2->mSafeCf.ap.AuthMode.IEEE8021X,prof3->mSafeCf.ap.AuthMode.IEEE8021X,prof4->mSafeCf.ap.AuthMode.IEEE8021X);
	StrtoApconf(key,"IEEE8021X",str);

	memset(str, '\0', sizeof(str));
	char ip1[16]={0}; 
	char ip2[16]={0}; 
	char ip3[16]={0}; 
	char ip4[16]={0}; 
	sprintf(str,"%s;%s;%s;%s",int2ip(ntohl(prof1->mSafeCf.ap.AuthMode.RADIUS_Server),ip1),int2ip(ntohl(prof2->mSafeCf.ap.AuthMode.RADIUS_Server),ip2),int2ip(ntohl(prof3->mSafeCf.ap.AuthMode.RADIUS_Server),ip3),int2ip(ntohl(prof4->mSafeCf.ap.AuthMode.RADIUS_Server),ip4));
	StrtoApconf(key,"RADIUS_Server",str);

	memset(str, '\0', sizeof(str));
	sprintf(str,"%d;%d;%d;%d",prof1->mSafeCf.ap.AuthMode.RADIUS_Port,prof2->mSafeCf.ap.AuthMode.RADIUS_Port,prof3->mSafeCf.ap.AuthMode.RADIUS_Port,prof4->mSafeCf.ap.AuthMode.RADIUS_Port);
	StrtoApconf(key,"RADIUS_Port",str);

	memset(str, '\0', sizeof(str));
	sprintf(str,"%s;%s;%s;%s",prof1->mSafeCf.ap.AuthMode.RADIUS_Key,prof2->mSafeCf.ap.AuthMode.RADIUS_Key,prof3->mSafeCf.ap.AuthMode.RADIUS_Key,prof4->mSafeCf.ap.AuthMode.RADIUS_Key);
	StrtoApconf(key,"RADIUS_Key",str);
    }
    else    /* fat ap */
    {
	InttoApconf(key,"BssidNum",prof->mBaseCf.BssidNum);
	//SSID1			SSID1
	StrtoApconf(key,"SSID1",prof->mBaseCf.SSID1);
	//HideSSID			SSIDBroadCastEn
	DistoApconf(key,"HideSSID",prof->mBaseCf.SSIDBroadCastEn);
	StrtoApconf(key,"AuthMode",prof->mSafeCf.SelAuthMode);
	//EncrypType		EncrypType
	StrtoApconf(key,"EncrypType",prof->mSafeCf.ap.AuthMode.EncrypType);
	//ApCliAuthMode		mBaseCf.SelAuthMode
	StrtoApconf(key,"ApCliAuthMode",prof->mBaseCf.SelAuthMode);
	//DefaultKeyID		wepKeyID
	InttoApconf(key,"DefaultKeyID",prof->mSafeCf.ap.AuthMode.wepKeyID);
	//Key1Str1			wepkey1			
	//Key2Str1			wepkey2			
	//Key3Str1			wepkey3			
	//Key4Str1			wepkey4			
	StrtoApconf(key,"Key1Str1",prof->mSafeCf.ap.AuthMode.wepkey1);
	StrtoApconf(key,"Key2Str1",prof->mSafeCf.ap.AuthMode.wepkey2);
	StrtoApconf(key,"Key3Str1",prof->mSafeCf.ap.AuthMode.wepkey3);
	StrtoApconf(key,"Key4Str1",prof->mSafeCf.ap.AuthMode.wepkey4);
	//Key1Type			wepKeyType
	//Key2Type			wepKeyType
	//Key3Type			wepKeyType
	//Key4Type			wepKeyType	
	InttoApconf(key,"Key1Type",prof->mSafeCf.ap.AuthMode.wepKeyType);
	InttoApconf(key,"Key2Type",prof->mSafeCf.ap.AuthMode.wepKeyType);
	InttoApconf(key,"Key3Type",prof->mSafeCf.ap.AuthMode.wepKeyType);
	InttoApconf(key,"Key4Type",prof->mSafeCf.ap.AuthMode.wepKeyType);
	//WPAPSK		pskPsswd
	StrtoApconf(key,"WPAPSK1",prof->mSafeCf.ap.AuthMode.pskPsswd);
	//IEEE8021X			IEEE8021X
	InttoApconf(key,"IEEE8021X",prof->mSafeCf.ap.AuthMode.IEEE8021X);
	//RADIUS_Server		RADIUS_Server
	IptoApconf(key,"RADIUS_Server",prof->mSafeCf.ap.AuthMode.RADIUS_Server);
	//RADIUS_Port		RADIUS_Port
	InttoApconf(key,"RADIUS_Port",prof->mSafeCf.ap.AuthMode.RADIUS_Port);
	//RADIUS_Key		RADIUS_Key
	StrtoApconf(key,"RADIUS_Key",prof->mSafeCf.ap.AuthMode.RADIUS_Key);
    
    }
    //WirelessMode		WirelessMode
    InttoApconf(key,"WirelessMode",prof->mBaseCf.WirelessMode);
    //BasicRate			BasicRate
    InttoApconf(key,"BasicRate",prof->mBaseCf.BasicRate);
    //AutoChannelSelect		AutoChannelSelect
    InttoApconf(key,"AutoChannelSelect",prof->mBaseCf.AutoChannelSelect);
    //BYPASS            BYPASS
    InttoApconf(key,"BYPASS",prof->mBaseCf.BYPASS);
    //Channel			Channel
    InttoApconf(key,"Channel",prof->mBaseCf.Channel);
    //HT_BW			ChanWidth
    InttoApconf(key,"HT_BW",prof->mBaseCf.ChanWidth);
    //VHT_BW			VChanWidth
    InttoApconf(key,"VHT_BW",prof->mBaseCf.VChanWidth);
    //VHT_SGI
    InttoApconf(key,"VHT_SGI",1);
    //own_ip_addr		own_ip_addr
    IptoApconf(key,"own_ip_addr",prof->mBaseCf.own_ip_addr);
    //EAPifname			EAPifname
    StrtoApconf(key,"EAPifname",prof->mBaseCf.EAPifname);
    //PreAuthifname		PreAuthifname
    StrtoApconf(key,"PreAuthifname",prof->mBaseCf.PreAuthifname);

    /*---------------------- AP Client ----------------------------- */
    //ApCliEnable		apclientEnable
    InttoApconf(key,"ApCliEnable",prof->mBaseCf.apClient.apclientEnable);
    //ApCliBssid		APCliMacs
    MactoApconf(key,"ApCliBssid",prof->mBaseCf.apClient.APCliMacs);
    //ApCliSsid			ApCliSsid
    StrtoApconf(key,"ApCliSsid",prof->mBaseCf.apClient.ApCliSsid);
    //ApCliEncrypType		AuthMode.EncrypType
    StrtoApconf(key,"ApCliEncrypType",prof->mBaseCf.apClient.AuthMode.EncrypType);
    //ApCliWPAPSK		pskPsswd
    StrtoApconf(key,"ApCliWPAPSK",prof->mBaseCf.apClient.AuthMode.pskPsswd);
    //ApCliDefaultKeyId		wepKeyID
    InttoApconf(key,"ApCliDefaultKeyId",prof->mBaseCf.apClient.AuthMode.wepKeyID);
    //ApCliKey1Type;		wepKeyType
    InttoApconf(key,"ApCliKey1Type",prof->mBaseCf.apClient.AuthMode.wepKeyType);
    InttoApconf(key,"ApCliKey2Type",prof->mBaseCf.apClient.AuthMode.wepKeyType);
    InttoApconf(key,"ApCliKey3Type",prof->mBaseCf.apClient.AuthMode.wepKeyType);
    InttoApconf(key,"ApCliKey4Type",prof->mBaseCf.apClient.AuthMode.wepKeyType);
    //ApCliKey1Str;		wepkey1
    //ApCliKey2Str;		wepkey2	
    //ApCliKey3Str;		wepkey2
    //ApCliKey4Str;		wepkey2
    StrtoApconf(key,"ApCliKey1Str",prof->mBaseCf.apClient.AuthMode.wepkey1);
    StrtoApconf(key,"ApCliKey2Str",prof->mBaseCf.apClient.AuthMode.wepkey2);
    StrtoApconf(key,"ApCliKey3Str",prof->mBaseCf.apClient.AuthMode.wepkey3);
    StrtoApconf(key,"ApCliKey4Str",prof->mBaseCf.apClient.AuthMode.wepkey4);


/*  AP °²È«Ä£Ê½ÌØÊâ²¿·Ö  */
    //RekeyInterval		RekeyInterval
    InttoApconf(key,"RekeyInterval",prof->mSafeCf.ap.AuthMode.RekeyInterval);


    //WDS
    //WdsEncrypType		mBaseCf.SelAuthMode
//    StrtoApconf(key,"WdsEncrypType",prof->mBaseCf.SelAuthMode);
    StrtoApconf(key,"WdsEncrypType",prof->mBaseCf.apWds.wdsEncrypType);

    //WdsEnable			WdsEnable
    InttoApconf(key,"WdsEnable",prof->mBaseCf.apWds.wdsEnable);
    //WdsPhyMode		WdsPhyMode
    InttoApconf(key,"WdsPhyMode",prof->mBaseCf.apWds.WdsPhyMode);
    //WdsList			WPSMacs0-3
   StrtoApconf(key,"WdsList",prof->mBaseCf.apWds.WdsList);

   //WdsKey			pskPsswd
   if (strcmp(prof->mBaseCf.apWds.wdsEncrypType, "WEP") == 0) {
       StrtoApconf(key, "WdsKey", "");
   } else {
       StrtoApconf(key,"WdsKey",prof->mBaseCf.apWds.AuthMode.pskPsswd);
   }

   memset(str, 0, sizeof(str));
   if (prof->mBaseCf.apWds.AuthMode.wepKeyID == 1)
       strcpy(str, prof->mBaseCf.apWds.AuthMode.wepkey1);
   else if (prof->mBaseCf.apWds.AuthMode.wepKeyID == 2)
       strcpy(str, prof->mBaseCf.apWds.AuthMode.wepkey2);
   else if (prof->mBaseCf.apWds.AuthMode.wepKeyID == 3)
       strcpy(str, prof->mBaseCf.apWds.AuthMode.wepkey3);
   else if (prof->mBaseCf.apWds.AuthMode.wepKeyID == 4)
       strcpy(str, prof->mBaseCf.apWds.AuthMode.wepkey4);
   else
       strcpy(str, "iderror!");

   StrtoApconf(key,"Wds0Key",str);
   StrtoApconf(key,"Wds1Key",str);
   StrtoApconf(key,"Wds2Key",str);
   StrtoApconf(key,"Wds3Key",str);

   //¸ß¼¶
   //RTSThreshold		rts
   InttoApconf(key,"RTSThreshold",prof->advance.rts);
   //FragThreshold		section
   InttoApconf(key,"FragThreshold",prof->advance.section);
   //BeaconPeriod		beacon
    InttoApconf(key,"BeaconPeriod",prof->advance.beacon);
    //DtimPeriod		dtim
    InttoApconf(key,"DtimPeriod",prof->advance.dtim);
    //TxBurst			TxBurst
    InttoApconf(key,"TxBurst",prof->advance.TxBurst);
    //TxPreamble		preamble 
    InttoApconf(key,"TxPreamble",prof->advance.preamble);
    //WmmCapable		wmm
    InttoApconf(key,"WmmCapable",prof->advance.wmm);

    /*----------------------- wireless mac filter ------------------------------*/
    // globle
#if (FIT_AP_MAC_FILTER == FYES)
    if ((apModeProf->fatEnable == PROF_ENABLE))
#endif
	{
    if (macFltGlb->filterEn == FUN_DISABLE)
    {
	InttoApconf(key, "AccessPolicy0", 0);
    }
    else
    {
	if (macFltGlb->action == AP_MAC_ALLOW)
	{
	    InttoApconf(key, "AccessPolicy0", 1);
	}
	else
	{
	    InttoApconf(key, "AccessPolicy0", 2);
	}
    }
    if (macFltGlb->action == AP_MAC_ALLOW)
    {
	InttoApconf(key, "AccessPolicyRule", 1);
    }
    else
    {
	InttoApconf(key, "AccessPolicyRule", 2);
    }

    // mac filter
    ProfInstLowHigh(mibTypeMacFlt, &max, &min);
    for (index = min; index < max; index++)
    {   
        macFlt = (WirelessMacFilterProfile *)ProfGetInstPointByIndex(mibTypeMacFlt, index);
        if((macFlt != NULL) && (ProfInstIsFree(((Pointer)macFlt)) == 0)) 
        {
	    tmpmac = converMac6To12Byte(macFlt->mac);
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
    StrtoApconf(key, "AccessControlList0", wlanMac);
#if (FIT_AP_MAC_FILTER == FYES)
	}
    else
    {
        ApMacFilterProfile *mfProf = NULL;
        int mf_index, sz_max,sz_min,sz_index;
	unsigned char mac_null[6] = "";
        ProfInstLowHigh(MIB_PROF_FIT_AP_SZ, &sz_max, &sz_min);
        if(strncmp(key,"Access",6) == 0)
        {
            for (sz_index = sz_min; sz_index < sz_max; sz_index++)
            {
                if(strncmp(key,"AccessPolicy",12) == 0)
                {
                    memset(str, '\0', sizeof(str));
                    sprintf(str,"AccessPolicy%d", sz_index);
                    InttoApconf(key, str, macMap[sz_index].rule); /*AccessPolicy0-9 rule*/
                }else
                {
                    memset(wlanMac,'\0',sizeof(wlanMac));
                    for(mf_index = 0;mf_index < 4;mf_index++)/*±éÀúmacMap£¬³õÊ¼»¯MAC¹ýÂË*/
                    {
                        if(strlen(macMap[sz_index].filterName[mf_index]) > 0)
                        {
			   // printf("%s, filterName:%s\n", __func__, macMap[sz_index].filterName[mf_index]);
                            mfProf = (ApMacFilterProfile *)ProfGetInstPointByName(MIB_PROF_AP_MAC_FILTER, &(macMap[sz_index].filterName[mf_index]));
                            if((mfProf != NULL)&&(mfProf->head.active == TRUE))
                            {
                            //    for (i = 0; i < mfProf->num; i++)
				for (i = 0; i < MAX_MAC_BLACK_NUM; i++)
                                {
				    if (memcmp(&(mfProf->mac[i]), mac_null, 6) == 0)
				    {
					continue;
				    }
                                    tmpmac = converMac6To12Byte(mfProf->mac[i]);
                                    Conversion2StdMac(tmpmac);
                                    strcat(wlanMac, tmpmac);
                                    strcat(wlanMac, ";");
                                }
                            }
                        }
                    }
                    if(strlen(wlanMac) > 0)
                    {
                        wlanMac[strlen(wlanMac) - 1] = '\0';
                    }
                    memset(str, '\0', sizeof(str));
                    sprintf(str,"AccessControlList%d",sz_index);
                    StrtoApconf(key, str, wlanMac);
                }
            }
        }
#endif
    }

    /* RalinkÐèÒªµÄÒ»Ð©³õÊ¼»¯²ÎÊý */
    InttoApconf(key,"CountryRegion",prof->mBaseCf.CountryRegion);
    InttoApconf(key,"CountryRegionABand",prof->mBaseCf.CountryRegionABand);
    InttoApconf(key,"TxPower",prof->mBaseCf.TxPower);
    InttoApconf(key,"PktAggregate",prof->mBaseCf.PktAggregate);
    StrtoApconf(key,"APAifsn",prof->mBaseCf.APAifsn);
    StrtoApconf(key,"APCwmin",prof->mBaseCf.APCwmin);
    StrtoApconf(key,"APCwmax",prof->mBaseCf.APCwmax);
    StrtoApconf(key,"APTxop",prof->mBaseCf.APTxop);
    StrtoApconf(key,"APACM",prof->mBaseCf.APACM);
    StrtoApconf(key,"BSSAifsn",prof->mBaseCf.BSSAifsn);
    StrtoApconf(key,"BSSCwmin",prof->mBaseCf.BSSCwmin);
    StrtoApconf(key,"BSSCwmax",prof->mBaseCf.BSSCwmax);
    StrtoApconf(key,"BSSTxop",prof->mBaseCf.BSSTxop);
    StrtoApconf(key,"BSSACM",prof->mBaseCf.BSSACM);
    StrtoApconf(key,"AckPolicy",prof->mBaseCf.AckPolicy);
    InttoApconf(key,"APSDCapable",prof->mBaseCf.APSDCapable);
    InttoApconf(key,"DLSCapable",prof->mBaseCf.DLSCapable);
//    InttoApconf(key,"NoForwarding",prof->mBaseCf.NoForwarding);

	memset(str, '\0', sizeof(str));
	sprintf(str,"%d;%d;%d;%d",prof->mBaseCf.NoForwarding,prof->mBaseCf.NoForwarding,prof->mBaseCf.NoForwarding,prof->mBaseCf.NoForwarding);
//    StrtoApconf(key,"NoForwarding",str);

    InttoApconf(key,"NoForwardingBTNBSSID",prof->mBaseCf.NoForwarding);
    InttoApconf(key,"ShortSlot",prof->mBaseCf.ShortSlot);
    InttoApconf(key,"CSPeriod",prof->mBaseCf.CSPeriod);
    InttoApconf(key,"PreAuth",prof->mBaseCf.PreAuth);
    StrtoApconf(key,"RekeyMethod",prof->mBaseCf.RekeyMethod);
    InttoApconf(key,"PMKCachePeriod",prof->mBaseCf.PMKCachePeriod);
    InttoApconf(key,"HT_HTC",prof->mBaseCf.HT_HTC);
    InttoApconf(key,"HT_BSSCoexistence", prof->mBaseCf.HT_BSSCoexistence);
    InttoApconf(key,"HT_RDG",prof->mBaseCf.HT_RDG);
    InttoApconf(key,"HT_MpduDensity",prof->mBaseCf.HT_MpduDensity);
#if ((UTT_WA_500N==FYES) || (UTT_WA_501N==FYES) || (UTT_AP_801 == FYES))
    if(prof->mBaseCf.Channel!=0) {
	prof->mBaseCf.HT_EXTCHA = 1;
    }
    else{
	prof->mBaseCf.HT_EXTCHA = 0;
    }
#endif
    InttoApconf(key,"HT_EXTCHA",prof->mBaseCf.HT_EXTCHA);
    InttoApconf(key,"HT_AutoBA",prof->mBaseCf.HT_AutoBA);
    InttoApconf(key,"HT_BAWinSize",prof->mBaseCf.HT_BAWinSize);
    InttoApconf(key,"HT_GI",prof->mBaseCf.HT_GI);
    InttoApconf(key,"HT_STBC",prof->mBaseCf.HT_STBC);
    InttoApconf(key,"HT_MCS",prof->mBaseCf.HT_MCS);
    InttoApconf(key,"HT_TxStream",prof->mBaseCf.HT_TxStream);
    InttoApconf(key,"HT_RxStream",prof->mBaseCf.HT_RxStream);
    InttoApconf(key,"HT_PROTECT",prof->mBaseCf.HT_PROTECT);
    InttoApconf(key,"RADIUS_Acct_Port",prof->mBaseCf.RADIUS_Acct_Port);
#else

    int min, max, index;
    char wlanMac[1024] = {0};
    char *tmpmac;
    memset(ap_conf_buf, 0, AP_BUF_LEN + 1);
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;
#if (MAKE_MONEY == FYES)
    MakeMoneyProfile *profM= NULL;
#endif
    WirelessMacFilterGlobalProfile *macFltGlb = NULL;
    WirelessMacFilterProfile *macFlt = NULL;
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, num);
#if (MAKE_MONEY == FYES)
    profM = (MakeMoneyProfile*)ProfGetInstPointByIndex(MIB_PROF_MAKE_MONEY,0);
#endif
    MibProfileType mibTypeMacFlt = MIB_PROF_WIRELESS_MAC_FILTER;
    MibProfileType mibTypeMacFltGlb = MIB_PROF_WIRELESS_MAC_FILTER_GLOBAL;
    macFltGlb = (WirelessMacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeMacFltGlb, num);
    if(prof == NULL){
	return 0;
    }

    /*
     * ap Ö¸¶¨²ÎÊýµÄprofile×ª»»
     * ²ÎÊýÊÇ±£´æÔÚap_conf_bufÖÐ·µ»ØµÄ
     * ×¢Òâ£ºÈç¹ûap_conf_buf³¤¶È²»×ãÒÔ±£´æap²ÎÊý£¬ÔòÐèÒª¼Ó³¤
     */
    /* 
     eg:
     if (strcmp(key, "BssidNum") == 0) {
	    strncpy(ap_conf_buf, prof->bssidNum, AP_BUF_LEN);
     }
     */
    /*str			prof*/
    /*----------------------- base config------------------------------*/
    //RadioOn			active
    InttoApconf(key,"RadioOn",prof->head.active);
    //SSID1			SSID1
    StrtoApconf(key,"SSID1",prof->mBaseCf.SSID1);
#if (WLAN_MULTI_SSID == FYES)
    StrtoApconf(key,"SSID2",prof->mBaseCf.SSID2);
#if (MIDDLE_SOFT == FYES)
    StrtoApconf(key,"SSID3",prof->mBaseCf.SSID3);
    StrtoApconf(key,"SSID4",prof->mBaseCf.SSID4);
#endif
#if (MAKE_MONEY == FYES)/*xrgÔö¼Óssid*/
    if(num == 0){   //2.4G
        StrtoApconf(key,"SSID3",profM->ssid);
        StrtoApconf(key,"SSID4",profM->ssid2);
    }else if(num == 1){//5G
        StrtoApconf(key,"SSID3",profM->ssid_5g);
        StrtoApconf(key,"SSID4",profM->ssid2_5g);
    }
#endif
#endif
    //WirelessMode		WirelessMode
    InttoApconf(key,"WirelessMode",prof->mBaseCf.WirelessMode);
    //BasicRate			BasicRate
    InttoApconf(key,"BasicRate",prof->mBaseCf.BasicRate);
    //AutoChannelSelect		AutoChannelSelect
    InttoApconf(key,"AutoChannelSelect",prof->mBaseCf.AutoChannelSelect);
#if ((UTT_SMART_UI == FYES) && (WIRELESS_5G == FYES))
    if ((strcmp(key,"AutoChannelSkipList") == 0) && (num == 1)) {
	if (prof->mBaseCf.AutoChannelSelect != 0) {
	    StrtoApconf(key,"AutoChannelSkipList",skip_list[prof->mBaseCf.Country]);
	}
    }
#endif
    //Channel			Channel
    InttoApconf(key,"Channel",prof->mBaseCf.Channel);
    //HT_BW			ChanWidth
    InttoApconf(key,"HT_BW",prof->mBaseCf.ChanWidth);
    InttoApconf(key,"VHT_BW",prof->mBaseCf.VChanWidth);
    //VHT_SGI
    InttoApconf(key,"VHT_SGI",1);
    //HideSSID			SSIDBroadCastEn
#if (WLAN_MULTI_SSID == FYES)
    memset(str, '\0', sizeof(str));
#if (MAKE_MONEY == FYES)
    sprintf(str,"%d;%d;%d;%d",(prof->mBaseCf.SSIDBroadCastEn == FUN_ENABLE ? 0:1),(prof->mBaseCf.SSIDBroadCastEn == FUN_ENABLE ? 0:1),0,0);
#elif (MIDDLE_SOFT == FYES)
    sprintf(str,"%d;%d;%d;%d", (prof->mBaseCf.SSIDBroadCastEn == FUN_ENABLE ? 0:1), (prof->mBaseCf.SSIDBroadCastEn == FUN_ENABLE ? 0:1),      (prof->mBaseCf.SSIDBroadCastEn == FUN_ENABLE ? 0:1), (prof->mBaseCf.SSIDBroadCastEn == FUN_ENABLE ? 0:1));
#else
    sprintf(str,"%d;%d;%d;%d", (prof->mBaseCf.SSIDBroadCastEn == FUN_ENABLE ? 0:1), (prof->mBaseCf.SSIDBroadCastEn == FUN_ENABLE ? 0:1));
#endif
    StrtoApconf(key,"HideSSID",str);
#else
    DistoApconf(key,"HideSSID",prof->mBaseCf.SSIDBroadCastEn);
#endif
    //own_ip_addr		own_ip_addr
    IptoApconf(key,"own_ip_addr",prof->mBaseCf.own_ip_addr);
    //EAPifname			EAPifname
    StrtoApconf(key,"EAPifname",prof->mBaseCf.EAPifname);
    //PreAuthifname		PreAuthifname
    StrtoApconf(key,"PreAuthifname",prof->mBaseCf.PreAuthifname);

    /*---------------------- AP Client ----------------------------- */
    //ApCliEnable		apclientEnable
    InttoApconf(key,"ApCliEnable",prof->mBaseCf.apClient.apclientEnable);
    //ApCliBssid		APCliMacs
    MactoApconf(key,"ApCliBssid",prof->mBaseCf.apClient.APCliMacs);
    //ApCliSsid			ApCliSsid
    StrtoApconf(key,"ApCliSsid",prof->mBaseCf.apClient.ApCliSsid);
    //ApCliAuthMode		mBaseCf.SelAuthMode
    StrtoApconf(key,"ApCliAuthMode",prof->mBaseCf.SelAuthMode);
    //ApCliEncrypType		AuthMode.EncrypType
    StrtoApconf(key,"ApCliEncrypType",prof->mBaseCf.apClient.AuthMode.EncrypType);
    //ApCliWPAPSK		pskPsswd
    StrtoApconf(key,"ApCliWPAPSK",prof->mBaseCf.apClient.AuthMode.pskPsswd);
    //ApCliDefaultKeyId		wepKeyID
    InttoApconf(key,"ApCliDefaultKeyId",prof->mBaseCf.apClient.AuthMode.wepKeyID);
    //ApCliKey1Type;		wepKeyType
    InttoApconf(key,"ApCliKey1Type",prof->mBaseCf.apClient.AuthMode.wepKeyType);
    InttoApconf(key,"ApCliKey2Type",prof->mBaseCf.apClient.AuthMode.wepKeyType);
    InttoApconf(key,"ApCliKey3Type",prof->mBaseCf.apClient.AuthMode.wepKeyType);
    InttoApconf(key,"ApCliKey4Type",prof->mBaseCf.apClient.AuthMode.wepKeyType);
    //ApCliKey1Str;		wepkey1
    //ApCliKey2Str;		wepkey2	
    //ApCliKey3Str;		wepkey2
    //ApCliKey4Str;		wepkey2
    StrtoApconf(key,"ApCliKey1Str",prof->mBaseCf.apClient.AuthMode.wepkey1);
    StrtoApconf(key,"ApCliKey2Str",prof->mBaseCf.apClient.AuthMode.wepkey2);
    StrtoApconf(key,"ApCliKey3Str",prof->mBaseCf.apClient.AuthMode.wepkey3);
    StrtoApconf(key,"ApCliKey4Str",prof->mBaseCf.apClient.AuthMode.wepkey4);


/*  AP °²È«Ä£Ê½ÌØÊâ²¿·Ö  */
    //RADIUS_Server		RADIUS_Server
#if (WLAN_MULTI_SSID == FYES)
    char ip1[16]={0}; 
    char ip2[16]={0}; 
#if (UTT_SSID2_ENCRYPTION == FYES)
    sprintf(str,"%s;%s",int2ip(ntohl(prof->mSafeCf.ap.AuthMode.RADIUS_Server),ip1),int2ip(ntohl(prof->mSafeCf.ap.AuthMode_2.RADIUS_Server),ip2));
#else
#if(MAKE_MONEY == FYES)
    sprintf(str,"%s;%s;%s;%s",int2ip(ntohl(prof->mSafeCf.ap.AuthMode.RADIUS_Server),ip1),int2ip(ntohl(prof->mSafeCf.ap.AuthMode.RADIUS_Server),ip2),int2ip(ntohl(prof->mSafeCf.ap.AuthMode.RADIUS_Server),ip1),int2ip(ntohl(prof->mSafeCf.ap.AuthMode.RADIUS_Server),ip2));
#else
    sprintf(str,"%s;%s",int2ip(ntohl(prof->mSafeCf.ap.AuthMode.RADIUS_Server),ip1),int2ip(ntohl(prof->mSafeCf.ap.AuthMode.RADIUS_Server),ip2));
#endif
#endif
    StrtoApconf(key,"RADIUS_Server",str);
#else
    IptoApconf(key,"RADIUS_Server",prof->mSafeCf.ap.AuthMode.RADIUS_Server);
#endif
    //RADIUS_Port		RADIUS_Port
#if (WLAN_MULTI_SSID == FYES)
    memset(str, '\0', sizeof(str));
#if (UTT_SSID2_ENCRYPTION == FYES)
    sprintf(str,"%d;%d",prof->mSafeCf.ap.AuthMode.RADIUS_Port,prof->mSafeCf.ap.AuthMode_2.RADIUS_Port);
#else
#if (MAKE_MONEY == FYES)
    sprintf(str,"%d;%d;%d;%d",prof->mSafeCf.ap.AuthMode.RADIUS_Port,prof->mSafeCf.ap.AuthMode.RADIUS_Port,prof->mSafeCf.ap.AuthMode.RADIUS_Port,prof->mSafeCf.ap.AuthMode.RADIUS_Port);
#else
    sprintf(str,"%d;%d",prof->mSafeCf.ap.AuthMode.RADIUS_Port,prof->mSafeCf.ap.AuthMode.RADIUS_Port);
#endif
#endif
    StrtoApconf(key,"RADIUS_Port",str);
#else
    InttoApconf(key,"RADIUS_Port",prof->mSafeCf.ap.AuthMode.RADIUS_Port);
#endif
    //RADIUS_Key		RADIUS_Key
#if (WLAN_MULTI_SSID == FYES)
    memset(str, '\0', sizeof(str));
#if (UTT_SSID2_ENCRYPTION == FYES)
    sprintf(str,"%s;%s",prof->mSafeCf.ap.AuthMode.RADIUS_Key,prof->mSafeCf.ap.AuthMode_2.RADIUS_Key);
#else
#if (MAKE_MONEY == FYES)
    sprintf(str,"%s;%s;%s;%s",prof->mSafeCf.ap.AuthMode.RADIUS_Key,prof->mSafeCf.ap.AuthMode.RADIUS_Key,prof->mSafeCf.ap.AuthMode.RADIUS_Key,prof->mSafeCf.ap.AuthMode.RADIUS_Key);
#else
    sprintf(str,"%s;%s",prof->mSafeCf.ap.AuthMode.RADIUS_Key,prof->mSafeCf.ap.AuthMode.RADIUS_Key);
#endif
#endif
    StrtoApconf(key,"RADIUS_Key",str);
#else
    StrtoApconf(key,"RADIUS_Key",prof->mSafeCf.ap.AuthMode.RADIUS_Key);
#endif
    //IEEE8021X			IEEE8021X
#if (WLAN_MULTI_SSID == FYES)
    memset(str, '\0', sizeof(str));
#if (UTT_SSID2_ENCRYPTION == FYES)
    sprintf(str,"%d;%d",prof->mSafeCf.ap.AuthMode.IEEE8021X,prof->mSafeCf.ap.AuthMode_2.IEEE8021X);
#else
#if (MAKE_MONEY == FYES)
    sprintf(str,"%d;%d;%d;%d",prof->mSafeCf.ap.AuthMode.IEEE8021X,prof->mSafeCf.ap.AuthMode.IEEE8021X,prof->mSafeCf.ap.AuthMode.IEEE8021X,prof->mSafeCf.ap.AuthMode.IEEE8021X);
#elif (MIDDLE_SOFT == FYES)
    sprintf(str,"%d;%d;%d;%d",prof->mSafeCf.ap.AuthMode.IEEE8021X,prof->mSafeCf.ap.AuthMode.IEEE8021X,prof->mSafeCf.ap.AuthMode.IEEE8021X,prof->mSafeCf.ap.AuthMode.IEEE8021X);
#else
    sprintf(str,"%d;%d",prof->mSafeCf.ap.AuthMode.IEEE8021X,prof->mSafeCf.ap.AuthMode.IEEE8021X);
#endif
#endif
    StrtoApconf(key,"IEEE8021X",str);
#else
    InttoApconf(key,"IEEE8021X",prof->mSafeCf.ap.AuthMode.IEEE8021X);
#endif
    //RekeyInterval		RekeyInterval
    InttoApconf(key,"RekeyInterval",prof->mSafeCf.ap.AuthMode.RekeyInterval);


    //WDS
    //WdsEncrypType		mBaseCf.SelAuthMode
//    StrtoApconf(key,"WdsEncrypType",prof->mBaseCf.SelAuthMode);
    StrtoApconf(key,"WdsEncrypType",prof->mBaseCf.apWds.wdsEncrypType);

    //WdsEnable			WdsEnable
    InttoApconf(key,"WdsEnable",prof->mBaseCf.apWds.wdsEnable);
    //WdsPhyMode		WdsPhyMode
    InttoApconf(key,"WdsPhyMode",prof->mBaseCf.apWds.WdsPhyMode);
    //WdsList			WPSMacs0-3
   StrtoApconf(key,"WdsList",prof->mBaseCf.apWds.WdsList);

   if (strcmp(prof->mBaseCf.apWds.wdsEncrypType, "WEP") == 0) {
       StrtoApconf(key, "WdsKey", "");
   } else {
       StrtoApconf(key,"WdsKey",prof->mBaseCf.apWds.AuthMode.pskPsswd);
   }

   memset(str, 0, sizeof(str));
   if (prof->mBaseCf.apWds.AuthMode.wepKeyID == 1)
       strcpy(str, prof->mBaseCf.apWds.AuthMode.wepkey1);
   else if (prof->mBaseCf.apWds.AuthMode.wepKeyID == 2)
       strcpy(str, prof->mBaseCf.apWds.AuthMode.wepkey2);
   else if (prof->mBaseCf.apWds.AuthMode.wepKeyID == 3)
       strcpy(str, prof->mBaseCf.apWds.AuthMode.wepkey3);
   else if (prof->mBaseCf.apWds.AuthMode.wepKeyID == 4)
       strcpy(str, prof->mBaseCf.apWds.AuthMode.wepkey4);
   else
       strcpy(str, "iderror!");

   StrtoApconf(key,"Wds0Key",str);
   StrtoApconf(key,"Wds1Key",str);
   StrtoApconf(key,"Wds2Key",str);
   StrtoApconf(key,"Wds3Key",str);

   //WPAPSK		pskPsswd
   StrtoApconf(key,"WPAPSK1",prof->mSafeCf.ap.AuthMode.pskPsswd);
#if (WLAN_MULTI_SSID == FYES)
#if (UTT_SSID2_ENCRYPTION == FYES)
   StrtoApconf(key,"WPAPSK2",prof->mSafeCf.ap.AuthMode_2.pskPsswd);
#elif (MIDDLE_SOFT == FYES)
   StrtoApconf(key,"WPAPSK2",prof->mSafeCf.ap.AuthMode.pskPsswd);
   StrtoApconf(key,"WPAPSK3",prof->mSafeCf.ap.AuthMode.pskPsswd);
   StrtoApconf(key,"WPAPSK4",prof->mSafeCf.ap.AuthMode.pskPsswd);
#else
   StrtoApconf(key,"WPAPSK2",prof->mSafeCf.ap.AuthMode.pskPsswd);
#endif
#endif
#if (MAKE_MONEY == FYES)
   StrtoApconf(key,"WPAPSK3","");
   StrtoApconf(key,"WPAPSK4","");
#endif
   //WdsKey			pskPsswd
#if (WLAN_MULTI_SSID == FYES)
    memset(str, '\0', sizeof(str));
#if (UTT_SSID2_ENCRYPTION == FYES)
    sprintf(str,"%s;%s", prof->mSafeCf.SelAuthMode, prof->mSafeCf.SelAuthMode_2);
#else
#if (MAKE_MONEY == FYES)
    sprintf(str,"%s;%s;OPEN;OPEN", prof->mSafeCf.SelAuthMode, prof->mSafeCf.SelAuthMode);
#elif (MIDDLE_SOFT == FYES)
    sprintf(str,"%s;%s;%s;%s", prof->mSafeCf.SelAuthMode, prof->mSafeCf.SelAuthMode,prof->mSafeCf.SelAuthMode,prof->mSafeCf.SelAuthMode);
#else
    sprintf(str,"%s;%s", prof->mSafeCf.SelAuthMode, prof->mSafeCf.SelAuthMode);
#endif
#endif
   StrtoApconf(key,"AuthMode",str);
#else
   StrtoApconf(key,"AuthMode",prof->mSafeCf.SelAuthMode);
#endif
   //EncrypType		EncrypType
#if (WLAN_MULTI_SSID == FYES)
    memset(str, '\0', sizeof(str));
#if (UTT_SSID2_ENCRYPTION == FYES)
    sprintf(str,"%s;%s", prof->mSafeCf.ap.AuthMode.EncrypType, prof->mSafeCf.ap.AuthMode_2.EncrypType);
#else
#if (MAKE_MONEY == FYES)
    sprintf(str,"%s;%s;NONE;NONE", prof->mSafeCf.ap.AuthMode.EncrypType, prof->mSafeCf.ap.AuthMode.EncrypType);
#elif (MIDDLE_SOFT == FYES)
    sprintf(str,"%s;%s;%s;%s", prof->mSafeCf.ap.AuthMode.EncrypType, prof->mSafeCf.ap.AuthMode.EncrypType,prof->mSafeCf.ap.AuthMode.EncrypType, prof->mSafeCf.ap.AuthMode.EncrypType);
#else 
    sprintf(str,"%s;%s", prof->mSafeCf.ap.AuthMode.EncrypType, prof->mSafeCf.ap.AuthMode.EncrypType);
#endif
#endif
   StrtoApconf(key,"EncrypType",str);
#else
   StrtoApconf(key,"EncrypType",prof->mSafeCf.ap.AuthMode.EncrypType);
#endif
   //DefaultKeyID		wepKeyID
#if (WLAN_MULTI_SSID == FYES)
    memset(str, '\0', sizeof(str));
#if (UTT_SSID2_ENCRYPTION == FYES)
    sprintf(str,"%d;%d", prof->mSafeCf.ap.AuthMode.wepKeyID, prof->mSafeCf.ap.AuthMode_2.wepKeyID);
#else
#if (MAKE_MONEY == FYES)
    sprintf(str,"%d;%d;%d;%d", prof->mSafeCf.ap.AuthMode.wepKeyID, prof->mSafeCf.ap.AuthMode.wepKeyID, prof->mSafeCf.ap.AuthMode.wepKeyID, prof->mSafeCf.ap.AuthMode.wepKeyID);
#elif (MIDDLE_SOFT == FYES)
    sprintf(str,"%d;%d;%d;%d", prof->mSafeCf.ap.AuthMode.wepKeyID, prof->mSafeCf.ap.AuthMode.wepKeyID,prof->mSafeCf.ap.AuthMode.wepKeyID,prof->mSafeCf.ap.AuthMode.wepKeyID);
#else
    sprintf(str,"%d;%d", prof->mSafeCf.ap.AuthMode.wepKeyID, prof->mSafeCf.ap.AuthMode.wepKeyID);
#endif
#endif
   StrtoApconf(key,"DefaultKeyID",str);
#else
    InttoApconf(key,"DefaultKeyID",prof->mSafeCf.ap.AuthMode.wepKeyID);
#endif
    //Key1Str1			wepkey1			
    //Key2Str1			wepkey2			
    //Key3Str1			wepkey3			
    //Key4Str1			wepkey4			
    StrtoApconf(key,"Key1Str1",prof->mSafeCf.ap.AuthMode.wepkey1);
    StrtoApconf(key,"Key2Str1",prof->mSafeCf.ap.AuthMode.wepkey2);
    StrtoApconf(key,"Key3Str1",prof->mSafeCf.ap.AuthMode.wepkey3);
    StrtoApconf(key,"Key4Str1",prof->mSafeCf.ap.AuthMode.wepkey4);
#if (WLAN_MULTI_SSID == FYES)
#if (UTT_SSID2_ENCRYPTION == FYES)
    StrtoApconf(key,"Key1Str2",prof->mSafeCf.ap.AuthMode_2.wepkey1);
    StrtoApconf(key,"Key2Str2",prof->mSafeCf.ap.AuthMode_2.wepkey2);
    StrtoApconf(key,"Key3Str2",prof->mSafeCf.ap.AuthMode_2.wepkey3);
    StrtoApconf(key,"Key4Str2",prof->mSafeCf.ap.AuthMode_2.wepkey4);
#elif (MIDDLE_SOFT == FYES)
    StrtoApconf(key,"Key1Str2",prof->mSafeCf.ap.AuthMode.wepkey1);
    StrtoApconf(key,"Key2Str2",prof->mSafeCf.ap.AuthMode.wepkey2);
    StrtoApconf(key,"Key3Str2",prof->mSafeCf.ap.AuthMode.wepkey3);
    StrtoApconf(key,"Key4Str2",prof->mSafeCf.ap.AuthMode.wepkey4);
    StrtoApconf(key,"Key1Str3",prof->mSafeCf.ap.AuthMode.wepkey1);
    StrtoApconf(key,"Key2Str3",prof->mSafeCf.ap.AuthMode.wepkey2);
    StrtoApconf(key,"Key3Str3",prof->mSafeCf.ap.AuthMode.wepkey3);
    StrtoApconf(key,"Key4Str3",prof->mSafeCf.ap.AuthMode.wepkey4);
    StrtoApconf(key,"Key1Str4",prof->mSafeCf.ap.AuthMode.wepkey1);
    StrtoApconf(key,"Key2Str4",prof->mSafeCf.ap.AuthMode.wepkey2);
    StrtoApconf(key,"Key3Str4",prof->mSafeCf.ap.AuthMode.wepkey3);
    StrtoApconf(key,"Key4Str4",prof->mSafeCf.ap.AuthMode.wepkey4);
#else
    StrtoApconf(key,"Key1Str2",prof->mSafeCf.ap.AuthMode.wepkey1);
    StrtoApconf(key,"Key2Str2",prof->mSafeCf.ap.AuthMode.wepkey2);
    StrtoApconf(key,"Key3Str2",prof->mSafeCf.ap.AuthMode.wepkey3);
    StrtoApconf(key,"Key4Str2",prof->mSafeCf.ap.AuthMode.wepkey4);
#endif
#endif
#if (MAKE_MONEY == FYES)
    StrtoApconf(key,"Key1Str3",prof->mSafeCf.ap.AuthMode.wepkey1);
    StrtoApconf(key,"Key2Str3",prof->mSafeCf.ap.AuthMode.wepkey2);
    StrtoApconf(key,"Key3Str3",prof->mSafeCf.ap.AuthMode.wepkey3);
    StrtoApconf(key,"Key4Str3",prof->mSafeCf.ap.AuthMode.wepkey4);
    StrtoApconf(key,"Key1Str4",prof->mSafeCf.ap.AuthMode.wepkey1);
    StrtoApconf(key,"Key2Str4",prof->mSafeCf.ap.AuthMode.wepkey2);
    StrtoApconf(key,"Key3Str4",prof->mSafeCf.ap.AuthMode.wepkey3);
    StrtoApconf(key,"Key4Str4",prof->mSafeCf.ap.AuthMode.wepkey4);

#endif
    //Key1Type			wepKeyType
    //Key2Type			wepKeyType
    //Key3Type			wepKeyType
    //Key4Type			wepKeyType	
#if (WLAN_MULTI_SSID == FYES)
    memset(str, '\0', sizeof(str));
#if (UTT_SSID2_ENCRYPTION == FYES)
    sprintf(str,"%d;%d",prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode_2.wepKeyType);
#else
#if (MAKE_MONEY == FYES)
    sprintf(str,"%d;%d;%d;%d",prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType);
#elif (MIDDLE_SOFT == FYES)
    sprintf(str,"%d;%d;%d;%d",prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType);
#else
    sprintf(str,"%d;%d",prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType);
#endif
#endif
    StrtoApconf(key,"Key1Type",str);
    memset(str, '\0', sizeof(str));
#if (UTT_SSID2_ENCRYPTION == FYES)
    sprintf(str,"%d;%d",prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode_2.wepKeyType);
#else
#if (MAKE_MONEY == FYES)
    sprintf(str,"%d;%d;%d;%d",prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType);
#elif (MIDDLE_SOFT == FYES)
    sprintf(str,"%d;%d;%d;%d",prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType);
#else 
    sprintf(str,"%d;%d",prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType);
#endif
#endif
    StrtoApconf(key,"Key2Type",str);
    memset(str, '\0', sizeof(str));
#if (UTT_SSID2_ENCRYPTION == FYES)
    sprintf(str,"%d;%d",prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode_2.wepKeyType);
#else
#if (MAKE_MONEY == FYES)
    sprintf(str,"%d;%d;%d;%d",prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType);
#elif (MIDDLE_SOFT == FYES)
    sprintf(str,"%d;%d;%d;%d",prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType);
#else
    sprintf(str,"%d;%d",prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType);
#endif
#endif
    StrtoApconf(key,"Key3Type",str);
    memset(str, '\0', sizeof(str));
#if (UTT_SSID2_ENCRYPTION == FYES)
    sprintf(str,"%d;%d",prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode_2.wepKeyType);
#else
#if (MAKE_MONEY == FYES)
    sprintf(str,"%d;%d;%d;%d",prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType);
#elif (MIDDLE_SOFT == FYES)
    sprintf(str,"%d;%d;%d;%d",prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType);
#else
    sprintf(str,"%d;%d",prof->mSafeCf.ap.AuthMode.wepKeyType,prof->mSafeCf.ap.AuthMode.wepKeyType);
#endif
#endif
    StrtoApconf(key,"Key4Type",str);
#else
    InttoApconf(key,"Key1Type",prof->mSafeCf.ap.AuthMode.wepKeyType);
    InttoApconf(key,"Key2Type",prof->mSafeCf.ap.AuthMode.wepKeyType);
    InttoApconf(key,"Key3Type",prof->mSafeCf.ap.AuthMode.wepKeyType);
    InttoApconf(key,"Key4Type",prof->mSafeCf.ap.AuthMode.wepKeyType);
#endif

    
    //¸ß¼¶
    //RTSThreshold		rts
    InttoApconf(key,"RTSThreshold",prof->advance.rts);
    //FragThreshold		section
    InttoApconf(key,"FragThreshold",prof->advance.section);
    //BeaconPeriod		beacon
    InttoApconf(key,"BeaconPeriod",prof->advance.beacon);
    //DtimPeriod		dtim
    InttoApconf(key,"DtimPeriod",prof->advance.dtim);
    //TxBurst			TxBurst
    InttoApconf(key,"TxBurst",prof->advance.TxBurst);
    //TxPreamble		preamble 
    InttoApconf(key,"TxPreamble",prof->advance.preamble);
    //WmmCapable		wmm
    InttoApconf(key,"WmmCapable",prof->advance.wmm);

    /*----------------------- wireless mac filter ------------------------------*/
    // globle
    if (macFltGlb->filterEn == FUN_DISABLE)
    {
	InttoApconf(key, "AccessPolicy0", 0);
    }
    else
    {
	if (macFltGlb->action == AP_MAC_ALLOW)
	{
	    InttoApconf(key, "AccessPolicy0", 1);
	}
	else
	{
	    InttoApconf(key, "AccessPolicy0", 2);
	}
    }
    if (macFltGlb->action == AP_MAC_ALLOW)
    {
	InttoApconf(key, "AccessPolicyRule", 1);
    }
    else
    {
	InttoApconf(key, "AccessPolicyRule", 2);
    }

    // mac filter
    ProfInstLowHigh(mibTypeMacFlt, &max, &min);
    for (index = min; index < max; index++)
    {   
        macFlt = (WirelessMacFilterProfile *)ProfGetInstPointByIndex(mibTypeMacFlt, index);
        if((macFlt != NULL) && (ProfInstIsFree(((Pointer)macFlt)) == 0)) 
        {
	    tmpmac = converMac6To12Byte(macFlt->mac);
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
    StrtoApconf(key, "AccessControlList0", wlanMac);

    /* RalinkÐèÒªµÄÒ»Ð©³õÊ¼»¯²ÎÊý */

    InttoApconf(key,"CountryRegion",prof->mBaseCf.CountryRegion);
    InttoApconf(key,"CountryRegionABand",prof->mBaseCf.CountryRegionABand);
    InttoApconf(key,"BssidNum",prof->mBaseCf.BssidNum);
    InttoApconf(key,"TxPower",prof->mBaseCf.TxPower);
    InttoApconf(key,"PktAggregate",prof->mBaseCf.PktAggregate);
    StrtoApconf(key,"APAifsn",prof->mBaseCf.APAifsn);
    StrtoApconf(key,"APCwmin",prof->mBaseCf.APCwmin);
    StrtoApconf(key,"APCwmax",prof->mBaseCf.APCwmax);
    StrtoApconf(key,"APTxop",prof->mBaseCf.APTxop);
    StrtoApconf(key,"APACM",prof->mBaseCf.APACM);
    StrtoApconf(key,"BSSAifsn",prof->mBaseCf.BSSAifsn);
    StrtoApconf(key,"BSSCwmin",prof->mBaseCf.BSSCwmin);
    StrtoApconf(key,"BSSCwmax",prof->mBaseCf.BSSCwmax);
    StrtoApconf(key,"BSSTxop",prof->mBaseCf.BSSTxop);
    StrtoApconf(key,"BSSACM",prof->mBaseCf.BSSACM);
    StrtoApconf(key,"AckPolicy",prof->mBaseCf.AckPolicy);
    InttoApconf(key,"APSDCapable",prof->mBaseCf.APSDCapable);
    InttoApconf(key,"DLSCapable",prof->mBaseCf.DLSCapable);
//    InttoApconf(key,"NoForwarding",prof->mBaseCf.NoForwarding);

	memset(str, '\0', sizeof(str));
	sprintf(str,"%d;%d;%d;%d",prof->mBaseCf.NoForwarding,prof->mBaseCf.NoForwarding,prof->mBaseCf.NoForwarding,prof->mBaseCf.NoForwarding);
//    StrtoApconf(key,"NoForwarding",str);

    InttoApconf(key,"NoForwardingBTNBSSID",prof->mBaseCf.NoForwarding);
    InttoApconf(key,"ShortSlot",prof->mBaseCf.ShortSlot);
    InttoApconf(key,"CSPeriod",prof->mBaseCf.CSPeriod);
    InttoApconf(key,"PreAuth",prof->mBaseCf.PreAuth);
    StrtoApconf(key,"RekeyMethod",prof->mBaseCf.RekeyMethod);
    InttoApconf(key,"PMKCachePeriod",prof->mBaseCf.PMKCachePeriod);
    InttoApconf(key,"HT_HTC",prof->mBaseCf.HT_HTC);
    InttoApconf(key,"HT_BSSCoexistence", prof->mBaseCf.HT_BSSCoexistence);
    InttoApconf(key,"HT_RDG",prof->mBaseCf.HT_RDG);
    InttoApconf(key,"HT_MpduDensity",prof->mBaseCf.HT_MpduDensity);
    InttoApconf(key,"HT_EXTCHA",prof->mBaseCf.HT_EXTCHA);
    InttoApconf(key,"HT_AutoBA",prof->mBaseCf.HT_AutoBA);
    InttoApconf(key,"HT_BAWinSize",prof->mBaseCf.HT_BAWinSize);
    InttoApconf(key,"HT_GI",prof->mBaseCf.HT_GI);
    InttoApconf(key,"HT_STBC",prof->mBaseCf.HT_STBC);
    InttoApconf(key,"HT_MCS",prof->mBaseCf.HT_MCS);
    InttoApconf(key,"HT_TxStream",prof->mBaseCf.HT_TxStream);
    InttoApconf(key,"HT_RxStream",prof->mBaseCf.HT_RxStream);
    InttoApconf(key,"HT_PROTECT",prof->mBaseCf.HT_PROTECT);
    InttoApconf(key,"RADIUS_Acct_Port",prof->mBaseCf.RADIUS_Acct_Port);

#endif

    return ap_conf_buf;
}

#if (WIRELESS_5G == FYES)
#if (FIT_AP_MAC_FILTER == FYES)
static void creatApSzMacMap5G(void)
{
    FitAp5gSzProfile *prof = NULL;
    ApMacFilterProfile *mfProf = NULL;
    int sz_index,max,min,index;
    int mf_max,mf_min,mf_index;
	int i;
    ProfInstLowHigh(MIB_PROF_FIT_AP_SZ_5G, &max, &min);
    ProfInstLowHigh(MIB_PROF_AP_MAC_FILTER, &mf_max, &mf_min);
    memset(&macMap,0,sizeof(apMacMap)*4);

    for (index = min; index < max; index++) 
    {
        i=0;
        prof = (FitAp5gSzProfile *)ProfGetInstPointByIndex(MIB_PROF_FIT_AP_SZ_5G, index);
        if ((prof != NULL)&&(prof->head.active == TRUE))
        {
            strcpy(macMap[index].sz,prof->head.name);/*¼ÇÂ¼·þÎñÇøÃû×Ö*/
            for (mf_index = mf_min; mf_index < mf_max; mf_index++)
            {
                mfProf = (ApMacFilterProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_MAC_FILTER, mf_index);
                if(( mfProf != NULL)&&(mfProf->head.active == TRUE))
                {
                    for(sz_index = 0; sz_index < MAX_AP_MAC_FILTER_NUM; sz_index++)//±éÀú¹ýÂË×é°üº¬µÄ·þÎñÇøÃû
                    {
                        if(mfProf->sz[sz_index][0] != '\0')/*·þÎñÇøÃûºÏ·¨*/
                        {
                            if(strcmp(mfProf->sz[sz_index],prof->head.name) == 0)/*ÊÇ·ñ°üº¬·þÎñÇø*/
                            {
                                strcpy(macMap[index].filterName[i],mfProf->head.name);
								macMap[index].rule = mfProf->mode;
                                i++;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return ;
}
#endif
#endif

#if (FIT_AP_MAC_FILTER == FYES)
/*Í³¼Æ·þÎñÇøÒýÓÃmac¹ýÂËµØÖ·×é*/
static void creatApSzMacMap(void)
{
    FitApSzProfile *prof = NULL;
    ApMacFilterProfile *mfProf = NULL;
    int sz_index,max,min,index;
    int mf_max,mf_min,mf_index;
	int i;
    ProfInstLowHigh(MIB_PROF_FIT_AP_SZ, &max, &min);
    ProfInstLowHigh(MIB_PROF_AP_MAC_FILTER, &mf_max, &mf_min);
    memset(&macMap,0,sizeof(apMacMap)*4);

    for (index = min; index < max; index++) 
    {
        i=0;
        prof = (FitApSzProfile *)ProfGetInstPointByIndex(MIB_PROF_FIT_AP_SZ, index);
        if ((prof != NULL)&&(prof->head.active == TRUE))
        {
            strcpy(macMap[index].sz,prof->head.name);/*¼ÇÂ¼·þÎñÇøÃû×Ö*/
            for (mf_index = mf_min; mf_index < mf_max; mf_index++)
            {
                mfProf = (ApMacFilterProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_MAC_FILTER, mf_index);
                if(( mfProf != NULL)&&(mfProf->head.active == TRUE))
                {
                    for(sz_index = 0; sz_index < MAX_AP_MAC_FILTER_NUM; sz_index++)//±éÀú¹ýÂË×é°üº¬µÄ·þÎñÇøÃû
                    {
                        if(mfProf->sz[sz_index][0] != '\0')/*·þÎñÇøÃûºÏ·¨*/
                        {
                            if(strcmp(mfProf->sz[sz_index],prof->head.name) == 0)/*ÊÇ·ñ°üº¬·þÎñÇø*/
                            {
                                strcpy(macMap[index].filterName[i],mfProf->head.name);
								macMap[index].rule = mfProf->mode;
                                i++;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return ;
}
#endif

/*
 * ¸ù¾ÝprofileÉú³ÉapÄ£¿éËùÐèÒªµÄÅäÖÃ
 * /etc/Wireless/RT2860/RT2860.dat
 */
static void gen_ap_config(Pointer *prof) {
    FILE *fp = NULL;
    int  i, ssid_num = 1;
    char tx_rate[16], wmm_enable[16];

    system("mkdir -p /etc/Wireless/RT2860");
    fp = fopen("/etc/Wireless/RT2860/RT2860.dat", "w+");

    if (fp == NULL) {
	return;
    }
    fprintf(fp, "#The word of \"Default\" must not be removed\n");
    fprintf(fp, "Default\n");
#define FPRINT_NUM(x, y) fprintf(fp, #x"=%d\n", atoi(prof_to_apconf(#x, y)));
#define FPRINT_STR(x, y) fprintf(fp, #x"=%s\n", prof_to_apconf(#x, y));

	FPRINT_NUM(CountryRegion, 0);
	FPRINT_NUM(CountryRegionABand, 0);
	FPRINT_STR(CountryCode, 0);
	FPRINT_NUM(BssidNum, 0);
	ssid_num = atoi(prof_to_apconf("BssidNum", 0));

	FPRINT_STR(SSID1, 0);
	FPRINT_STR(SSID2, 0);
    FPRINT_STR(SSID3, 0);
	FPRINT_STR(SSID4, 0);
	FPRINT_STR(SSID5, 0);
	FPRINT_STR(SSID6, 0);
	FPRINT_STR(SSID7, 0);
	FPRINT_STR(SSID8, 0);

	FPRINT_NUM(WirelessMode, 0);
	FPRINT_STR(FixedTxMode, 0);

	//TxRate(FixedRate)
	bzero(tx_rate, sizeof(char)*16);
	for (i = 0; i < ssid_num; i++)
	{
		sprintf(tx_rate+strlen(tx_rate), "%d",
				atoi(prof_to_apconf("TxRate", 0)));
		sprintf(tx_rate+strlen(tx_rate), "%c", ';');
	}
	tx_rate[strlen(tx_rate) - 1] = '\0';
	fprintf(fp, "TxRate=%s\n", tx_rate);

#if (FEATURE_AP == FYES)
	FPRINT_NUM(BYPASS, 0);
#endif
	FPRINT_NUM(Channel, 0);
	FPRINT_NUM(BasicRate, 0);
	FPRINT_NUM(BeaconPeriod, 0);
	FPRINT_NUM(DtimPeriod, 0);
	FPRINT_NUM(TxPower, 0);
	FPRINT_NUM(DisableOLBC, 0);
	FPRINT_NUM(BGProtection, 0);
	fprintf(fp, "TxAntenna=\n");
	fprintf(fp, "RxAntenna=\n");
	FPRINT_NUM(TxPreamble, 0);
	FPRINT_NUM(RTSThreshold , 0);
	FPRINT_NUM(FragThreshold , 0);
	FPRINT_NUM(TxBurst, 0);
	FPRINT_NUM(PktAggregate, 0);
	fprintf(fp, "TurboRate=0\n");

#if ((UTT_WA_500N==FYES) || (UTT_WA_501N==FYES) || (UTT_AP_801 == FYES))
	FPRINT_NUM(AutoProvisionEn, 0);
	FPRINT_NUM(FreqDelta, 0);
	FPRINT_NUM(ITxBfEn, 0);
	FPRINT_NUM(PreAntSwitch, 0);
	FPRINT_NUM(PhyRateLimit, 0);
	FPRINT_NUM(DebugFlags, 0);
	FPRINT_NUM(ETxBfEnCond, 0);
	FPRINT_NUM(ITxBfTimeout, 0);
	FPRINT_NUM(ETxBfTimeout, 0);
	FPRINT_NUM(ETxBfNoncompress, 0);
	FPRINT_NUM(ETxBfIncapable, 0);
	FPRINT_NUM(FineAGC, 0);
	FPRINT_NUM(StreamMode, 0);
	FPRINT_NUM(DfsOutdoor, 0);
	FPRINT_NUM(SymRoundFromCfg, 0);
	FPRINT_NUM(BusyIdleFromCfg, 0);
	FPRINT_NUM(DfsRssiHighFromCfg, 0);
	FPRINT_NUM(DfsRssiLowFromCfg, 0);
	FPRINT_NUM(DFSParamFromConfig, 0);
	FPRINT_NUM(HT_BSSCoexistence, 0);
	FPRINT_NUM(GreenAP, 0);
	FPRINT_NUM(WCNTest, 0);
#endif
	//WmmCapable
	bzero(wmm_enable, sizeof(char)*16);
	for (i = 0; i < ssid_num; i++)
	{
		sprintf(wmm_enable+strlen(wmm_enable), "%d",
//				atoi(nvram_bufget(mode, "WmmCapable")));
				atoi(prof_to_apconf("WmmCapable", 0)));
		sprintf(wmm_enable+strlen(wmm_enable), "%c", ';');
	}
	wmm_enable[strlen(wmm_enable) - 1] = '\0';
	fprintf(fp, "WmmCapable=%s\n", wmm_enable);

	FPRINT_STR(APAifsn, 0);
	FPRINT_STR(APCwmin, 0);
	FPRINT_STR(APCwmax, 0);
	FPRINT_STR(APTxop, 0);
	FPRINT_STR(APACM, 0);
	FPRINT_STR(BSSAifsn, 0);
	FPRINT_STR(BSSCwmin, 0);
	FPRINT_STR(BSSCwmax, 0);
	FPRINT_STR(BSSTxop, 0);
	FPRINT_STR(BSSACM, 0);
	FPRINT_STR(AckPolicy, 0);
	FPRINT_STR(APSDCapable, 0);
	FPRINT_STR(DLSCapable, 0);
//	FPRINT_STR(NoForwarding, 0);
	FPRINT_NUM(NoForwardingBTNBSSID, 0);
	FPRINT_STR(HideSSID, 0);
	FPRINT_NUM(ShortSlot, 0);
	FPRINT_NUM(AutoChannelSelect, 0);

	FPRINT_STR(IEEE8021X, 0);
	FPRINT_NUM(IEEE80211H, 0);
	FPRINT_NUM(CarrierDetect, 0);

	FPRINT_NUM(CSPeriod, 0);
	FPRINT_STR(RDRegion, 0);
	FPRINT_NUM(StationKeepAlive, 0);
	FPRINT_NUM(DfsLowerLimit, 0);
	FPRINT_NUM(DfsUpperLimit, 0);
	FPRINT_NUM(FixDfsLimit, 0);
	FPRINT_NUM(LongPulseRadarTh, 0);
	FPRINT_NUM(AvgRssiReq, 0);
	FPRINT_NUM(DFS_R66, 0);
	FPRINT_STR(BlockCh, 0);

	FPRINT_NUM(GreenAP, 0);
	FPRINT_STR(PreAuth, 0);
	FPRINT_STR(AuthMode, 0);
	FPRINT_STR(EncrypType, 0);
/*kurtis: WAPI*/
	FPRINT_STR(WapiPsk1, 0);
	FPRINT_STR(WapiPskType, 0);
	FPRINT_STR(Wapiifname, 0);
	FPRINT_STR(WapiAsCertPath, 0);
	FPRINT_STR(WapiUserCertPath, 0);
	FPRINT_STR(WapiAsIpAddr, 0);
	FPRINT_STR(WapiAsPort, 0);

	FPRINT_NUM(BssidNum, 0);

	FPRINT_STR(RekeyMethod, 0);
	FPRINT_NUM(RekeyInterval, 0);
	FPRINT_STR(PMKCachePeriod, 0);

	FPRINT_NUM(MeshAutoLink, 0);
	FPRINT_STR(MeshAuthMode, 0);
	FPRINT_STR(MeshEncrypType, 0);
	FPRINT_NUM(MeshDefaultkey, 0);
	FPRINT_STR(MeshWEPKEY, 0);
	FPRINT_STR(MeshWPAKEY, 0);
	FPRINT_STR(MeshId, 0);

	//WPAPSK
	FPRINT_STR(WPAPSK1, 0);
	FPRINT_STR(WPAPSK2, 0);
	FPRINT_STR(WPAPSK3, 0);
	FPRINT_STR(WPAPSK4, 0);
	FPRINT_STR(WPAPSK5, 0);
	FPRINT_STR(WPAPSK6, 0);
	FPRINT_STR(WPAPSK7, 0);
	FPRINT_STR(WPAPSK8, 0);

	FPRINT_STR(DefaultKeyID, 0);
	FPRINT_STR(Key1Type, 0);
	FPRINT_STR(Key1Str1, 0);
	FPRINT_STR(Key1Str2, 0);
	FPRINT_STR(Key1Str3, 0);
	FPRINT_STR(Key1Str4, 0);
	FPRINT_STR(Key1Str5, 0);
	FPRINT_STR(Key1Str6, 0);
	FPRINT_STR(Key1Str7, 0);
	FPRINT_STR(Key1Str8, 0);

	FPRINT_STR(Key2Type, 0);
	FPRINT_STR(Key2Str1, 0);
	FPRINT_STR(Key2Str2, 0);
	FPRINT_STR(Key2Str3, 0);
	FPRINT_STR(Key2Str4, 0);
	FPRINT_STR(Key2Str5, 0);
	FPRINT_STR(Key2Str6, 0);
	FPRINT_STR(Key2Str7, 0);
	FPRINT_STR(Key2Str8, 0);

	FPRINT_STR(Key3Type, 0);
	FPRINT_STR(Key3Str1, 0);
	FPRINT_STR(Key3Str2, 0);
	FPRINT_STR(Key3Str3, 0);
	FPRINT_STR(Key3Str4, 0);
	FPRINT_STR(Key3Str5, 0);
	FPRINT_STR(Key3Str6, 0);
	FPRINT_STR(Key3Str7, 0);
	FPRINT_STR(Key3Str8, 0);

	FPRINT_STR(Key4Type, 0);
	FPRINT_STR(Key4Str1, 0);
	FPRINT_STR(Key4Str2, 0);
	FPRINT_STR(Key4Str3, 0);
	FPRINT_STR(Key4Str4, 0);
	FPRINT_STR(Key4Str5, 0);
	FPRINT_STR(Key4Str6, 0);
	FPRINT_STR(Key4Str7, 0);
	FPRINT_STR(Key4Str8, 0);

	FPRINT_NUM(HSCounter, 0);

	FPRINT_NUM(HT_HTC, 0);
	FPRINT_NUM(HT_BSSCoexistence, 0);
	FPRINT_NUM(HT_RDG, 0);
	FPRINT_NUM(HT_LinkAdapt, 0);
	FPRINT_NUM(HT_OpMode, 0);
	FPRINT_NUM(HT_MpduDensity, 0);
	FPRINT_NUM(HT_EXTCHA, 0);
	FPRINT_NUM(HT_BW, 0);
	FPRINT_NUM(HT_AutoBA, 0);
	FPRINT_NUM(HT_BADecline, 0);
	FPRINT_NUM(HT_AMSDU, 0);
	FPRINT_NUM(HT_BAWinSize, 0);
	FPRINT_NUM(HT_GI, 0);
	FPRINT_NUM(HT_STBC, 0);
	FPRINT_NUM(HT_MCS, 0);
	FPRINT_NUM(HT_TxStream, 0);
	FPRINT_NUM(HT_RxStream, 0);
	FPRINT_NUM(HT_PROTECT, 0);
	FPRINT_NUM(HT_DisallowTKIP, 0);

#if(UTT_HAS_WPS_BUTTON == FYES)
	fprintf(fp, "WscConfMode=7\n");
	fprintf(fp, "WscConfStatus=2\n");
#else
	FPRINT_NUM(WscConfMode, 0);

	//WscConfStatus
//	if (atoi(nvram_bufget(mode, "WscConfigured")) == 0)
	if (atoi(prof_to_apconf("WscConfigured", 0)) == 0)
		fprintf(fp, "WscConfStatus=%d\n", 1);
	else
		fprintf(fp, "WscConfStatus=%d\n", 2);
//	if (strcmp(nvram_bufget(mode, "WscVendorPinCode"), "") != 0)
	if (strcmp(prof_to_apconf("WscVendorPinCode", 0), "") != 0)
		FPRINT_STR(WscVendorPinCode, 0);
#endif
#if(FIT_AP_MAC_FILTER == FYES)
    creatApSzMacMap();
#endif
	FPRINT_NUM(AccessPolicy0, 0);
	FPRINT_STR(AccessControlList0, 0);
	FPRINT_NUM(AccessPolicy1, 0);
	FPRINT_STR(AccessControlList1, 0);
	FPRINT_NUM(AccessPolicy2, 0);
	FPRINT_STR(AccessControlList2, 0);
	FPRINT_NUM(AccessPolicy3, 0);
	FPRINT_STR(AccessControlList3, 0);
	FPRINT_NUM(AccessPolicy4, 0);
	FPRINT_STR(AccessControlList4, 0);
	FPRINT_NUM(AccessPolicy5, 0);
	FPRINT_STR(AccessControlList5, 0);
	FPRINT_NUM(AccessPolicy6, 0);
	FPRINT_STR(AccessControlList6, 0);
	FPRINT_NUM(AccessPolicy7, 0);
	FPRINT_STR(AccessControlList7, 0);
	FPRINT_NUM(AccessPolicyRule, 0);

	FPRINT_NUM(WdsEnable, 0);
	FPRINT_STR(WdsPhyMode, 0);
	FPRINT_STR(WdsEncrypType, 0);
	FPRINT_STR(WdsList, 0);
	FPRINT_STR(WdsKey, 0);
	FPRINT_STR(Wds0Key, 0);
	FPRINT_STR(Wds1Key, 0);
	FPRINT_STR(Wds2Key, 0);
	FPRINT_STR(Wds3Key, 0);
	FPRINT_STR(RADIUS_Server, 0);
	FPRINT_STR(RADIUS_Port, 0);
	FPRINT_STR(RADIUS_Key, 0);
	FPRINT_STR(RADIUS_Acct_Server, 0);
	FPRINT_NUM(RADIUS_Acct_Port, 0);
	FPRINT_STR(RADIUS_Acct_Key, 0);
	FPRINT_STR(own_ip_addr, 0);
	FPRINT_STR(Ethifname, 0);
	FPRINT_STR(EAPifname, 0);
	FPRINT_STR(PreAuthifname, 0);
	FPRINT_NUM(session_timeout_interval, 0);
	FPRINT_NUM(idle_timeout_interval, 0);
	FPRINT_NUM(WiFiTest, 0);
	FPRINT_NUM(TGnWifiTest, 0);

	//AP Client parameters
	FPRINT_NUM(ApCliEnable, 0);
	FPRINT_STR(ApCliSsid, 0);
	FPRINT_STR(ApCliBssid, 0);
	FPRINT_STR(ApCliAuthMode, 0);
	FPRINT_STR(ApCliEncrypType, 0);
	FPRINT_STR(ApCliWPAPSK, 0);
	FPRINT_NUM(ApCliDefaultKeyId, 0);
	FPRINT_NUM(ApCliKey1Type, 0);
	FPRINT_STR(ApCliKey1Str, 0);
	FPRINT_NUM(ApCliKey2Type, 0);
	FPRINT_STR(ApCliKey2Str, 0);
	FPRINT_NUM(ApCliKey3Type, 0);
	FPRINT_STR(ApCliKey3Str, 0);
	FPRINT_NUM(ApCliKey4Type, 0);
	FPRINT_STR(ApCliKey4Str, 0);

	//Radio On/Off
//	if (atoi(nvram_bufget(mode, "RadioOff")) == 1)
#if 0
	if (atoi(prof_to_apconf("RadioOff", 0)) == 1)
		fprintf(fp, "RadioOn=0\n");
	else
		fprintf(fp, "RadioOn=1\n");
#endif
	FPRINT_NUM(RadioOn, 0);
	/*
	 * There are no SSID/WPAPSK/Key1Str/Key2Str/Key3Str/Key4Str anymore since driver1.5 , but 
	 * STA WPS still need these entries to show the WPS result(That is the only way i know to get WPAPSK key) and
	 * so we create empty entries here.   --YY
	 */
	fprintf(fp, "SSID=\nWPAPSK=\nKey1Str=\nKey2Str=\nKey3Str=\nKey4Str=\n");
        fclose(fp);
}

#if (WIRELESS_5G == FYES)
static void gen_ap_config_5g(Pointer *prof) {
    FILE *fp = NULL;
    int  i, ssid_num = 1;
    char tx_rate[16], wmm_enable[16];

    system("mkdir -p /etc/Wireless/iNIC");
    fp = fopen("/etc/Wireless/iNIC/iNIC_ap.dat", "w+");

    if (fp == NULL) {
	return;
    }

    fprintf(fp, "#The word of \"Default\" must not be removed\n");
    fprintf(fp, "Default\n");

#define FPRINT_NUM(x, y) fprintf(fp, #x"=%d\n", atoi(prof_to_apconf(#x, y)));
#define FPRINT_STR(x, y) fprintf(fp, #x"=%s\n", prof_to_apconf(#x, y));

	FPRINT_NUM(CountryRegion, 1);
	FPRINT_NUM(CountryRegionABand, 1);
	FPRINT_STR(CountryCode, 1);
	FPRINT_NUM(BssidNum, 1);
	ssid_num = atoi(prof_to_apconf("BssidNum", 1));

	FPRINT_STR(SSID1, 1);
	FPRINT_STR(SSID2, 1);
	FPRINT_STR(SSID3, 1);
	FPRINT_STR(SSID4, 1);
	FPRINT_STR(SSID5, 1);
	FPRINT_STR(SSID6, 1);
	FPRINT_STR(SSID7, 1);
	FPRINT_STR(SSID8, 1);

	FPRINT_NUM(WirelessMode, 1);
	FPRINT_STR(FixedTxMode, 1);

	//TxRate(FixedRate)
	bzero(tx_rate, sizeof(char)*16);
	for (i = 0; i < ssid_num; i++)
	{
		sprintf(tx_rate+strlen(tx_rate), "%d",
				atoi(prof_to_apconf("TxRate", 1)));
		sprintf(tx_rate+strlen(tx_rate), "%c", ';');
	}
	tx_rate[strlen(tx_rate) - 1] = '\0';
	fprintf(fp, "TxRate=%s\n", tx_rate);

#if (FEATURE_AP == FYES)
	FPRINT_NUM(BYPASS, 1);
#endif
	FPRINT_NUM(Channel, 1);
	FPRINT_NUM(BasicRate, 1);
	FPRINT_NUM(BeaconPeriod, 1);
	FPRINT_NUM(DtimPeriod, 1);
	FPRINT_NUM(TxPower, 1);
	FPRINT_NUM(DisableOLBC, 1);
	FPRINT_NUM(BGProtection, 1);
	fprintf(fp, "TxAntenna=\n");
	fprintf(fp, "RxAntenna=\n");
	FPRINT_NUM(TxPreamble, 1);
	FPRINT_NUM(RTSThreshold , 1);
	FPRINT_NUM(FragThreshold , 1);
	FPRINT_NUM(TxBurst, 1);
	FPRINT_NUM(PktAggregate, 1);
	FPRINT_NUM(AutoProvisionEn, 1);
	fprintf(fp, "TurboRate=0\n");
	FPRINT_NUM(FreqDelta, 1);
	FPRINT_NUM(ITxBfEn, 1);
	FPRINT_NUM(PreAntSwitch, 1);
	FPRINT_NUM(PhyRateLimit, 1);
	FPRINT_NUM(DebugFlags, 1);
	FPRINT_NUM(ETxBfEnCond, 1);
	FPRINT_NUM(ITxBfTimeout, 1);
	FPRINT_NUM(ETxBfTimeout, 1);
	FPRINT_NUM(ETxBfNoncompress, 1);
	FPRINT_NUM(ETxBfIncapable, 1);
	FPRINT_NUM(FineAGC, 1);
	FPRINT_NUM(StreamMode, 1);
	FPRINT_STR(StreamModeMac0, 1);
	FPRINT_STR(StreamModeMac1, 1);
	FPRINT_STR(StreamModeMac2, 1);
	FPRINT_STR(StreamModeMac3, 1);
	FPRINT_NUM(DfsOutdoor, 1);
	FPRINT_NUM(SymRoundFromCfg, 1);
	FPRINT_NUM(BusyIdleFromCfg, 1);
	FPRINT_NUM(DfsRssiHighFromCfg, 1);
	FPRINT_NUM(DfsRssiLowFromCfg, 1);
	FPRINT_NUM(DFSParamFromConfig, 1);

	FPRINT_STR(FCCParamCh0, 1);
	FPRINT_STR(FCCParamCh1, 1);
	FPRINT_STR(FCCParamCh2, 1);
	FPRINT_STR(FCCParamCh3, 1);
	FPRINT_STR(CEParamCh0, 1);
	FPRINT_STR(CEParamCh1, 1);
	FPRINT_STR(CEParamCh2, 1);
	FPRINT_STR(CEParamCh3, 1);
	FPRINT_STR(JAPParamCh0, 1);
	FPRINT_STR(JAPParamCh1, 1);
	FPRINT_STR(JAPParamCh2, 1);
	FPRINT_STR(JAPParamCh3, 1);
	FPRINT_STR(JAPW53ParamCh0, 1);
	FPRINT_STR(JAPW53ParamCh1, 1);
	FPRINT_STR(JAPW53ParamCh2, 1);
	FPRINT_STR(JAPW53ParamCh3, 1);

	FPRINT_NUM(HT_BSSCoexistence, 1);
	FPRINT_NUM(GreenAP, 1);
	FPRINT_NUM(WCNTest, 1);
	//WmmCapable
	bzero(wmm_enable, sizeof(char)*16);
	for (i = 0; i < ssid_num; i++)
	{
		sprintf(wmm_enable+strlen(wmm_enable), "%d",
//				atoi(nvram_bufget(mode, "WmmCapable")));
				atoi(prof_to_apconf("WmmCapable", 1)));
		sprintf(wmm_enable+strlen(wmm_enable), "%c", ';');
	}
	wmm_enable[strlen(wmm_enable) - 1] = '\0';
	fprintf(fp, "WmmCapable=%s\n", wmm_enable);

	FPRINT_STR(APAifsn, 1);
	FPRINT_STR(APCwmin, 1);
	FPRINT_STR(APCwmax, 1);
	FPRINT_STR(APTxop, 1);
	FPRINT_STR(APACM, 1);
	FPRINT_STR(BSSAifsn, 1);
	FPRINT_STR(BSSCwmin, 1);
	FPRINT_STR(BSSCwmax, 1);
	FPRINT_STR(BSSTxop, 1);
	FPRINT_STR(BSSACM, 1);
	FPRINT_STR(AckPolicy, 1);
	FPRINT_STR(APSDCapable, 1);
	FPRINT_STR(DLSCapable, 1);
//	FPRINT_STR(NoForwarding, 1);
	FPRINT_NUM(NoForwardingBTNBSSID, 1);
	FPRINT_STR(HideSSID, 1);
	FPRINT_NUM(ShortSlot, 1);
	FPRINT_NUM(AutoChannelSelect, 1);
#if ((UTT_SMART_UI == FYES) && (WIRELESS_5G == FYES))
	FPRINT_STR(AutoChannelSkipList, 1);
#endif

	FPRINT_STR(IEEE8021X, 1);
	FPRINT_NUM(IEEE80211H, 1);
	FPRINT_NUM(CarrierDetect, 1);

	FPRINT_NUM(CSPeriod, 1);
	FPRINT_STR(RDRegion, 1);
	FPRINT_NUM(StationKeepAlive, 1);
	FPRINT_NUM(DfsLowerLimit, 1);
	FPRINT_NUM(DfsUpperLimit, 1);
	FPRINT_NUM(FixDfsLimit, 1);
	FPRINT_NUM(LongPulseRadarTh, 1);
	FPRINT_NUM(AvgRssiReq, 1);
	FPRINT_NUM(DFS_R66, 1);
	FPRINT_STR(BlockCh, 1);

	FPRINT_NUM(GreenAP, 1);
	FPRINT_STR(PreAuth, 1);
	FPRINT_STR(AuthMode, 1);
	FPRINT_STR(EncrypType, 1);
/*kurtis: WAPI*/
	FPRINT_STR(WapiPsk1, 1);
	FPRINT_STR(WapiPsk2, 1);
	FPRINT_STR(WapiPsk3, 1);
	FPRINT_STR(WapiPsk4, 1);
	FPRINT_STR(WapiPsk5, 1);
	FPRINT_STR(WapiPsk6, 1);
	FPRINT_STR(WapiPsk7, 1);
	FPRINT_STR(WapiPsk8, 1);
	FPRINT_STR(WapiPskType, 1);
	FPRINT_STR(Wapiifname, 1);
	FPRINT_STR(WapiAsCertPath, 1);
	FPRINT_STR(WapiUserCertPath, 1);
	FPRINT_STR(WapiAsIpAddr, 1);
	FPRINT_STR(WapiAsPort, 1);

	FPRINT_NUM(BssidNum, 1);

	FPRINT_STR(RekeyMethod, 1);
	FPRINT_NUM(RekeyInterval, 1);
	FPRINT_STR(PMKCachePeriod, 1);

	FPRINT_NUM(MeshAutoLink, 1);
	FPRINT_STR(MeshAuthMode, 1);
	FPRINT_STR(MeshEncrypType, 1);
	FPRINT_NUM(MeshDefaultkey, 1);
	FPRINT_STR(MeshWEPKEY, 1);
	FPRINT_STR(MeshWPAKEY, 1);
	FPRINT_STR(MeshId, 1);

	//WPAPSK
	FPRINT_STR(WPAPSK1, 1);
	FPRINT_STR(WPAPSK2, 1);
	FPRINT_STR(WPAPSK3, 1);
	FPRINT_STR(WPAPSK4, 1);
	FPRINT_STR(WPAPSK5, 1);
	FPRINT_STR(WPAPSK6, 1);
	FPRINT_STR(WPAPSK7, 1);
	FPRINT_STR(WPAPSK8, 1);

	FPRINT_STR(DefaultKeyID, 1);
	FPRINT_STR(Key1Type, 1);
	FPRINT_STR(Key1Str1, 1);
	FPRINT_STR(Key1Str2, 1);
	FPRINT_STR(Key1Str3, 1);
	FPRINT_STR(Key1Str4, 1);
	FPRINT_STR(Key1Str5, 1);
	FPRINT_STR(Key1Str6, 1);
	FPRINT_STR(Key1Str7, 1);
	FPRINT_STR(Key1Str8, 1);

	FPRINT_STR(Key2Type, 1);
	FPRINT_STR(Key2Str1, 1);
	FPRINT_STR(Key2Str2, 1);
	FPRINT_STR(Key2Str3, 1);
	FPRINT_STR(Key2Str4, 1);
	FPRINT_STR(Key2Str5, 1);
	FPRINT_STR(Key2Str6, 1);
	FPRINT_STR(Key2Str7, 1);
	FPRINT_STR(Key2Str8, 1);

	FPRINT_STR(Key3Type, 1);
	FPRINT_STR(Key3Str1, 1);
	FPRINT_STR(Key3Str2, 1);
	FPRINT_STR(Key3Str3, 1);
	FPRINT_STR(Key3Str4, 1);
	FPRINT_STR(Key3Str5, 1);
	FPRINT_STR(Key3Str6, 1);
	FPRINT_STR(Key3Str7, 1);
	FPRINT_STR(Key3Str8, 1);

	FPRINT_STR(Key4Type, 1);
	FPRINT_STR(Key4Str1, 1);
	FPRINT_STR(Key4Str2, 1);
	FPRINT_STR(Key4Str3, 1);
	FPRINT_STR(Key4Str4, 1);
	FPRINT_STR(Key4Str5, 1);
	FPRINT_STR(Key4Str6, 1);
	FPRINT_STR(Key4Str7, 1);
	FPRINT_STR(Key4Str8, 1);

	FPRINT_NUM(HSCounter, 1);

	FPRINT_NUM(HT_HTC, 1);
	FPRINT_NUM(HT_BSSCoexistence, 1);
	FPRINT_NUM(HT_RDG, 1);
	FPRINT_NUM(HT_LinkAdapt, 1);
	FPRINT_NUM(HT_OpMode, 1);
	FPRINT_NUM(HT_MpduDensity, 1);
	FPRINT_NUM(HT_EXTCHA, 1);
	FPRINT_NUM(HT_BW, 1);
	FPRINT_NUM(VHT_BW, 1);
	FPRINT_NUM(VHT_SGI,1);
	FPRINT_NUM(HT_AutoBA, 1);
	FPRINT_NUM(HT_BADecline, 1);
	FPRINT_NUM(HT_AMSDU, 1);
	FPRINT_NUM(HT_BAWinSize, 1);
	FPRINT_NUM(HT_GI, 1);
	FPRINT_NUM(HT_STBC, 1);
	FPRINT_NUM(HT_MCS, 1);
	FPRINT_NUM(HT_TxStream, 1);
	FPRINT_NUM(HT_RxStream, 1);
	FPRINT_NUM(HT_PROTECT, 1);
	FPRINT_NUM(HT_DisallowTKIP, 1);

#if(UTT_HAS_WPS_BUTTON == FYES)
	fprintf(fp, "WscConfMode=7\n");
	fprintf(fp, "WscConfStatus=2\n");
#else
	FPRINT_NUM(WscConfMode, 1);

	//WscConfStatus
//	if (atoi(nvram_bufget(mode, "WscConfigured")) == 0)
	if (atoi(prof_to_apconf("WscConfigured", 1)) == 0)
		fprintf(fp, "WscConfStatus=%d\n", 1);
	else
		fprintf(fp, "WscConfStatus=%d\n", 2);
//	if (strcmp(nvram_bufget(mode, "WscVendorPinCode"), "") != 0)
	if (strcmp(prof_to_apconf("WscVendorPinCode", 1), "") != 0)
		FPRINT_STR(WscVendorPinCode, 1);
#endif
#if(FIT_AP_MAC_FILTER == FYES)
    creatApSzMacMap5G();
#endif
	FPRINT_NUM(AccessPolicy0, 1);
	FPRINT_STR(AccessControlList0, 1);
	FPRINT_NUM(AccessPolicy1, 1);
	FPRINT_STR(AccessControlList1, 1);
	FPRINT_NUM(AccessPolicy2, 1);
	FPRINT_STR(AccessControlList2, 1);
	FPRINT_NUM(AccessPolicy3, 1);
	FPRINT_STR(AccessControlList3, 1);
	FPRINT_NUM(AccessPolicy4, 1);
	FPRINT_STR(AccessControlList4, 1);
	FPRINT_NUM(AccessPolicy5, 1);
	FPRINT_STR(AccessControlList5, 1);
	FPRINT_NUM(AccessPolicy6, 1);
	FPRINT_STR(AccessControlList6, 1);
	FPRINT_NUM(AccessPolicy7, 1);
	FPRINT_STR(AccessControlList7, 1);
	FPRINT_NUM(AccessPolicyRule, 1);

	FPRINT_NUM(WdsEnable, 1);
	FPRINT_STR(WdsPhyMode, 1);
	FPRINT_STR(WdsEncrypType, 1);
	FPRINT_STR(WdsList, 1);
	FPRINT_STR(WdsKey, 1);
	FPRINT_STR(RADIUS_Server, 1);
	FPRINT_STR(RADIUS_Port, 1);
	FPRINT_STR(RADIUS_Key, 1);
	FPRINT_STR(RADIUS_Acct_Server, 1);
	FPRINT_NUM(RADIUS_Acct_Port, 1);
	FPRINT_STR(RADIUS_Acct_Key, 1);
	FPRINT_STR(own_ip_addr, 1);
	FPRINT_STR(Ethifname, 1);
	FPRINT_STR(EAPifname, 1);
	FPRINT_STR(PreAuthifname, 1);
	FPRINT_NUM(session_timeout_interval, 1);
	FPRINT_NUM(idle_timeout_interval, 1);
	FPRINT_NUM(WiFiTest, 1);
	FPRINT_NUM(TGnWifiTest, 1);

	//AP Client parameters
	FPRINT_NUM(ApCliEnable, 1);
	FPRINT_STR(ApCliSsid, 1);
	FPRINT_STR(ApCliBssid, 1);
	FPRINT_STR(ApCliAuthMode, 1);
	FPRINT_STR(ApCliEncrypType, 1);
	FPRINT_STR(ApCliWPAPSK, 1);
	FPRINT_NUM(ApCliDefaultKeyId, 1);
	FPRINT_NUM(ApCliKey1Type, 1);
	FPRINT_STR(ApCliKey1Str, 1);
	FPRINT_NUM(ApCliKey2Type, 1);
	FPRINT_STR(ApCliKey2Str, 1);
	FPRINT_NUM(ApCliKey3Type, 1);
	FPRINT_STR(ApCliKey3Str, 1);
	FPRINT_NUM(ApCliKey4Type, 1);
	FPRINT_STR(ApCliKey4Str, 1);

	//Radio On/Off
//	if (atoi(nvram_bufget(mode, "RadioOff")) == 1)
#if 0
	if (atoi(prof_to_apconf("RadioOff", 1)) == 1)
		fprintf(fp, "RadioOn=0\n");
	else
		fprintf(fp, "RadioOn=1\n");
#endif
	FPRINT_NUM(RadioOn, 1);
	/*
	 * There are no SSID/WPAPSK/Key1Str/Key2Str/Key3Str/Key4Str anymore since driver1.5 , but 
	 * STA WPS still need these entries to show the WPS result(That is the only way i know to get WPAPSK key) and
	 * so we create empty entries here.   --YY
	 */
	fprintf(fp, "SSID=\nWPAPSK=\nKey1Str=\nKey2Str=\nKey3Str=\nKey4Str=\n");
        fclose(fp);
}
#endif
#endif

#if ((UTT_WA_500N==FYES) || (UTT_WA_501N==FYES) || (UTT_AP_801 == FYES))
int get_e2p_val(int addr)
{
    char buf[64], data[32];
    int n=-1;
    FILE* fp=NULL;

    sprintf(buf, "iwpriv ra0 e2p %d", addr);
    fp = popen(buf, "r");
    if(fp!=NULL)
    {
	fgets(buf, sizeof(buf), fp);
	fgets(buf, sizeof(buf), fp);
	if(sscanf(buf, "%*[^:]:%s", data)==1)
	{
	    n = strtoul(data, NULL, 16);
	}

	fclose(fp);
    }

    return n;
}

void set_e2p_val(int addr, int val)
{
    char buf[64];

    sprintf(buf, "iwpriv ra0 e2p %04x=%04x\n", addr, val);
    system(buf);
    printf("%s: %s\n", __func__, buf);
}
void init_e2p_mac(void)
{
    unsigned int serno = 0, n, val1, val2, k1=0, k2=0, k3=0;

    serno = getSystemSn();
    serno += ((serno & 0x3fff) << 3);
    serno += 0x2711;
    n = serno & 0x3u;
    if(n != 0)
    {
	serno = (serno & (~0x3u)) + (n<<2);
    }

    val1 = 0xaa | (( (serno >> 16) & 0xffu)<<8);
    val2 = ((serno >> 8) & 0xffu) | ((serno & 0xffu)<<8);
    system("ifconfig ra0 up");
    if(get_e2p_val(4) !=  0x2200)
    {
	k1 = 1;
    }
    if(get_e2p_val(6) != val1)
    {
	k2 = 1;
    }
    if(get_e2p_val(8) != val2)
    {
	k3 = 1;
    }
    if((k1 | k2 | k3)!=0)
    {
	printf("%s: set ra0 mac\n", __func__);
	sleep(1);
	if(k1>0){
	    set_e2p_val(4, 0x2200);
	}
	if(k2>0){
	    set_e2p_val(6, val1);
	}
	if(k3>0){
	    set_e2p_val(8, val2);
	}
	system("rmmod rt2860v2_ap");
	system("insmod /lib/modules/2.6.21/kernel/drivers/net/wireless/rt2860v2_ap/rt2860v2_ap.ko");
    }
}
#endif
#if (IP_SEC == FYES)
struct ipsec_encrypt_t {
char name[10];
char encrypt[16];
};

struct ipsec_encrypt_t policyArray[8] = { {"p_212", "3des-md5-group2"}, {"p_213", "3des-md5-group5"}, {"p_222", "3des-sha-group2"}, {"p_223", "3des-sha-group5"}, {"p_112", "des-md5-group2"}, {"p_113", "des-md5-group5"}, {"p_122", "des-sha-group2"}, {"p_123", "des-sha-group5"}};
struct ipsec_encrypt_t transArray[15] = { 
{"tra_200", "esp-3des"}, 
{"tra_100", "esp-des"}, 
{"tra_110", "esp-des-md5"}, 
{"tra_120", "esp-des-sha"},
{"tra_210", "esp-3des-md5"}, 
{"tra_220", "esp-3des-sha"}, 
{"tra_300", "esp-aes128"}, 
{"tra_400", "esp-aes192"},
{"tra_500", "esp-aes256"},
{"tra_310", "esp-aes128-md5"},
{"tra_320", "esp-aes128-sha"},
{"tra_410", "esp-aes192-md5"},
{"tra_420", "esp-aes192-sha"},
{"tra_510", "esp-aes256-md5"},
{"tra_520", "esp-aes256-sha"}
};
#endif
int profileInst_new(MibProfileType mibtype, const char *instname)
{
    Pointer newprof = NULL;
    int newinstIndex = 0;
    Uint16 webIndexInst=0U;
    char instId[UTT_INST_NAME_LEN + 1]={0};
    int index = PROFFAIL;
    int inst_full = 0;
    int isakmp_name_true = 0;
    int policy_name_true  = 0;
    int trans_name_true = 0;

    if((instname != NULL) && (strlen(instname) > 0))
    {
	newprof = ProfGetInstPointByName(mibtype, instname);
	if(newprof != NULL) {
	    fprintf(stderr, "the inst assigned %s existed, please recheck.\n",instname);
	    return 0;
	}
	if(0
#if (EXCEPT_QQ == FYES)
		|| (mibtype==MIB_PROF_EXCEPT_QQ ) 
#endif
#if (PPPOE_SERVER == FYES)
		|| (mibtype==MIB_PROF_PPPOE_SRV_ACC)
#endif
#if (EXCEPT_MSN == FYES)
		|| (mibtype==MIB_PROF_EXCEPT_MSN) 
#endif
#if (EXCEPT_ALI == FYES)
		|| (mibtype==MIB_PROF_EXCEPT_ALI) 
#endif
#if (WEB_AUTH == FYES)
		|| (mibtype==MIB_PROF_WEB_AUTH) 
#endif
#if (IP_SEC == FYES)
		|| (mibtype==MIB_PROF_IPSEC)
#endif
		) {
	    /*Ô¶¯Éú³ÉÊµÀýÃû
	     * Èç: ID+(1,2,3......)*/

	    while (index == PROFFAIL) {
		if (0){}
#if (EXCEPT_QQ == FYES)
		else if(mibtype==MIB_PROF_EXCEPT_QQ ) {
		webIndexInst = getWebInstName(&(nvramProfile->exceptqqInstRec.instRecMap), &(nvramProfile->exceptqqInstRec.instNameIndex));
		} 
#endif
#if (EXCEPT_MSN == FYES)
		else if(mibtype==MIB_PROF_EXCEPT_MSN) {
		webIndexInst = getWebInstName(&(nvramProfile->exceptmsnInstRec.instRecMap), &(nvramProfile->exceptmsnInstRec.instNameIndex));
		} 
#endif
#if (EXCEPT_ALI == FYES)
		else if(mibtype==MIB_PROF_EXCEPT_ALI) {
		webIndexInst = getWebInstName(&(nvramProfile->exceptaliInstRec.instRecMap), &(nvramProfile->exceptaliInstRec.instNameIndex));
		} 
#endif
#if (PPPOE_SERVER == FYES)
		else if((mibtype==MIB_PROF_PPPOE_SRV_ACC)) {
		webIndexInst = getWebInstName(&(nvramProfile->pppoeInstRec.instRecMap), &(nvramProfile->pppoeInstRec.instNameIndex));
		}
#endif
#if (WEB_AUTH == FYES)
		else if(mibtype==MIB_PROF_WEB_AUTH) {
		webIndexInst = getWebInstName(&(nvramProfile->webauthInstRec.instRecMap), &(nvramProfile->webauthInstRec.instNameIndex));
		}
#endif
#if (IP_SEC == FYES)
		else if(mibtype==MIB_PROF_IPSEC) {
		webIndexInst = getWebInstName(&(nvramProfile->ipsecInstRec.instRecMap), &(nvramProfile->ipsecInstRec.instNameIndex));
		}
#endif
		if (webIndexInst > 0U) 
		{
		    /*define inst name with ID+index*/
		    memset(instId, 0, sizeof(instId));
		    sprintf(instId,"ID%d", webIndexInst);
		    /*create new inst and get index*/
		    index = ProfNewInst(mibtype,instId,FALSE);
		} else {
		    inst_full = 1;
		    fprintf(stderr, "you can not create a new inst, because it reach up to the max iterms.\n");
		    break;
		}
	    }
	    if(inst_full == 0) {
		fprintf(stderr, "you do not assign a inst name, it will be created automatically.\n");
		fprintf(stderr, "the new inst name is '%s'\n",instId);
	    }
	    return 0;
	    /*end  ×Ô¶¯Éú³ÉÊµÀýÃû*/
	} else {
	/* here should user new fun*/
#if (IP_SEC == FYES)
	    if(mibtype == MIB_PROF_ISAKMP) {
		ipsecConfProfile *prof = NULL;
		int min=0, max=0,i=0;
		int inst_num = 0;
		ProfInstLowHigh(MIB_PROF_IPSEC, &max, &min);
		for(i=min;i<max;i++) {
		    prof = (ipsecConfProfile *)ProfGetInstPointByIndex(MIB_PROF_IPSEC, i);
		    if((prof!=NULL)&&(ProfInstIsFree(prof) == 0)) {
			memset(instId, 0, sizeof(instId));
			snprintf(instId,UTT_INST_NAME_LEN+1,"%s%s",prof->head.name,"A" );
			if(strcmp(instId, instname) == 0) {
			    isakmp_name_true = 1;
			    break;
			}
			inst_num++;
		    }
		}
		if(isakmp_name_true != 1) {
		    if(inst_num == 0) {
			fprintf(stderr, "sorry, you have no 'ipsecConf' inst, first you should new a 'ipsecConf' inst.\n");
		    } else {
			fprintf(stderr, "you must specify the following 'isakmpConf' inst name, be used by 'ipsecConf isakmpProfile' param value\n" );
			fprintf(stderr, "%13s  -  %13s\n","isakmpConf","ipsecConf");
			for(i=min;i<max;i++) {
			    prof = (ipsecConfProfile *)ProfGetInstPointByIndex(MIB_PROF_IPSEC, i);
			    if((prof!=NULL)&&(ProfInstIsFree(prof) == 0)) {
				memset(instId, 0, sizeof(instId));
				snprintf(instId,UTT_INST_NAME_LEN+1,"%s%s",prof->head.name,"A" );
				fprintf(stderr, "%13s  -  %13s\n",instId,prof->head.name);
			    }
			}
		    }
		    return 0;
		}
	    }
	    else if(mibtype == MIB_PROF_ISAKMP_POLICY) {
		int i=0;

		for(i=0;i<8;i++) {
		    if(strcmp(policyArray[i].name, instname) == 0) {
			policy_name_true = 1;
			break;
		    }
		}
		if(policy_name_true != 1) {
		    fprintf(stderr, "you must specify the following 'policyConf' inst name, be used by 'isakmpConf policy' param value\n" );
		    fprintf(stderr, "%11s  -  %21s\n","inst name","encryption ");
		    for(i=0;i<8;i++) {
			fprintf(stderr, "%11s  -  %21s\n",policyArray[i].name,policyArray[i].encrypt);
		    }
		    return 0;
		}
	    }
	    else if(mibtype == MIB_PROF_TRANSFORMSET) {
		int i=0;


		for(i=0;i<15;i++) {
		    if(strcmp(transArray[i].name, instname) == 0) {
			trans_name_true = 1;
			break;
		    }
		}
		if(trans_name_true != 1) {
		    fprintf(stderr, "you must specify the following 'transformsetConf' inst name, be used by 'ipsecConf transformSet' param value\n" );
		    fprintf(stderr, "%11s  -  %21s\n","inst name","encryption ");
		    for(i=0;i<15;i++) {
			fprintf(stderr, "%11s  -  %21s\n",transArray[i].name,transArray[i].encrypt);
		    }
		    return 0;
		}
	    }
#endif
	    newinstIndex = ProfNewInst(mibtype, instname, 0);
	    if(newinstIndex == PROFFAIL) {
		newprof = ProfGetInstPointByName(mibtype, instname);
	    } else {
		newprof = ProfGetInstPointByIndex(mibtype, newinstIndex);
	    }
	    if(newprof != NULL) {
		ProfSetNameByPoint(newprof, (Byte*)instname);
		fprintf(stderr, "new inst name: '%s'\n",instname);
	    } else {
		fprintf(stderr, "new inst error, please recheck inst name '%s'.\n",((mibHead*)newprof)->name);
	    }
	}
    }
    return 0;
}
int profileInst_delete(MibProfileType mibtype, char *instname)
{
    Pointer delprof = NULL;
    struProfAlloc *profList = NULL;
    Uint16 webIndexInst=0U;
    char isakmp_name[UTT_INST_NAME_LEN+1];

    if((instname != NULL) && ((strlen(instname) > 0)&&(strlen(instname)<=UTT_INST_NAME_LEN)))
    {
	/*back inst and delete*/
	delprof = ProfGetInstPointByName(mibtype, instname);
	if(delprof!=NULL)
	{
#if (IP_SEC == FYES)
	    if(mibtype==MIB_PROF_IPSEC) {
		memset(isakmp_name, 0, sizeof(isakmp_name));
		snprintf(isakmp_name,UTT_INST_NAME_LEN+1, "%s", ((ipsecConfProfile *)delprof)->isakmpProfile);
	    }
#endif
	    /*delete inst by name*/
	    ProfBackupByName(mibtype, PROF_CHANGE_DEL, instname, &profList);
	    ProfDelInstByName(mibtype, instname);
	    fprintf(stderr, "delete inst name: %s\n",instname);
	    /* Çå³ý¼ÇÂ¼map */
	    if (strncmp(instname, "ID", 2) == 0) {
		webIndexInst = (Uint16)strtol(&(instname[2]), NULL, 10);
		if (0){}
#if (EXCEPT_QQ == FYES)
		else if(mibtype==MIB_PROF_EXCEPT_QQ ) {
		    clearPortMap(&(nvramProfile->exceptqqInstRec.instRecMap), webIndexInst);
		} 
#endif
#if (EXCEPT_MSN == FYES)
		else if(mibtype==MIB_PROF_EXCEPT_MSN) {
		    clearPortMap(&(nvramProfile->exceptmsnInstRec.instRecMap), webIndexInst);
		}
#endif
#if (EXCEPT_ALI == FYES)
		else if(mibtype==MIB_PROF_EXCEPT_ALI) {
		    clearPortMap(&(nvramProfile->exceptaliInstRec.instRecMap), webIndexInst);
		} 
#endif
#if (PPPOE_SERVER == FYES)
		else if(mibtype==MIB_PROF_PPPOE_SRV_ACC) {
		    clearPortMap(&(nvramProfile->pppoeInstRec.instRecMap), webIndexInst);
		}
#endif
#if (WEB_AUTH == FYES)
		else if(mibtype==MIB_PROF_WEB_AUTH) {
		    clearPortMap(&(nvramProfile->webauthInstRec.instRecMap), webIndexInst);
		} 
#endif
#if (IP_SEC == FYES)
		else if(mibtype==MIB_PROF_IPSEC) {
		    { /*del isakmpProfile inst*/
			if(strlen(isakmp_name) > 0)
			{
			    ProfDelInstByName(MIB_PROF_ISAKMP, isakmp_name);
			}
		    }
		    clearPortMap(&(nvramProfile->ipsecInstRec.instRecMap), webIndexInst);	    /*clear port map*/
		}
#endif
	    }
	    /*update profile and save*/
	    ProfUpdate(profList);
	    ProfFreeAllocList(profList);
	    nvramWriteCommit();
	} else {
	fprintf(stderr, "sorry, delete error, please recheck argument '%s'\n",instname);
	}
    } else {
	fprintf(stderr, "sorry, delete error, please recheck argument '%s'\n",instname);
    }
    return 0;
}
int profileInst_set()
{
    return 0;
}

void setWanUpnp(const char *value)
{
#if (UPNP_GLOBAL_ENABLE == FYES)
	MibProfileType mibType = MIB_PROF_INTERFACE;
	InterfaceProfile *profIf = NULL;
	int i;
	for(i = 1; i <= getWanSlotCount(); i++) {
	    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, i);
	    profIf->UpnpActive = (int)strtol((const char*)value, (char**)NULL, 10);
	}
	system("upnp.sh");
#endif
}
int CheckDate(const char name[],const char value[],struct st_xmlConfig *conf)
{
    int value_num;
    if((conf->itype == TINT))
    {
	value_num=atoi(value);

	/*itype TINT check*/
#if (PPPOE_SERVER == FYES)
	if(strcmp(name,"maxSessCnt")==0)//PPPoEÏµÍ³»á»°Êý
	{
	    if(value_num <= MAX_PPPOE_SRV_SESSION_PROFILES && value_num >= 1){
		return 1;
	    }
	    else {
		fprintf(stderr,"Session must be a number, between 1 to %d.\n",MAX_PPPOE_SRV_SESSION_PROFILES);
		return 0;
	    }
	}
	if(strcmp(name,"accMaxConv")==0)//PPPoEÕËºÅ×î´ó»á»°Êý
	{
	    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
	    PPPoESrvGloProfile *profS = NULL;
	    /*global profile inst*/
	    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS,0);

	    /*²»´óÓÚÓÃ»§ËùÉèÖÃµÄÏµÍ³×î´ó»á»°Êý*/
	    if(value_num >=1 && value_num <= profS->maxSessCnt) {
		return 1;
	    } else {
		fprintf(stderr,"Sessions must be a number, between 1 to %d.\n",profS->maxSessCnt);
		return 0;
	    }
	}
#endif
#if (WEB_AUTH_SHARE == FYES)
	if(strcmp(name,"maxsession")==0)//webÈÏÖ¤»á»°Êý
	{
	    if(value_num >=1 && value_num <= MAX_WEB_AUTH_SESSION) {
		return 1;
	    } else {
		fprintf(stderr,"Sessions must be a number, between 1 to %d.\n",MAX_WEB_AUTH_SESSION);
		return 0;
	    }
	}
#endif
    /*end TINT*/ 
    } else if((conf->itype == TUINT)) {
	;
    } else if((conf->itype == TSTR)) {
	;
    } else if((conf->itype == TENUM)) {
	;
    } else {
	;
    }
    return 1;
}

#ifdef RELOCATE_OLD_PROFILE
/***************************
 *
 * D525Æ½Ì¨AC2.6°æ±¾ºóµÄÈí¼þÊ¹ÓÃÖ÷¸ÉÏß¶¨ÒåµÄ·ÖÇø¸ñÊ½ºóÐè°Ñ¾ÉÅäÖÃ°áµ½ÐÂ·ÖÇø
 * µ±¾É°æ±¾µÄÈí¼þÒÑ¾­ºÜÉÙÓÐÈËÓÃ,»òÕßCONFIG_FLASH_SIZE´óÓÚ1MµÄÊ±ºò£¬ÐèÒª°Ñ±¾º¯ÊýÉ¾³ý
 * ²»Ö§³Ö½µ¼¶´¦Àí
 * KeHuatao
 * 20150521
 * */
void readConfFromOldPartition(void)
{
    unsigned int sn, offset;
    unsigned char buf[CONFIG_FLASH_SIZE];/*ÒÑÑ¹ËõÅäÖÃ*/
    unsigned char magicStr[]="UTTRELOC";

    /*get sn from the old partition. offset:0x80f4000*/
    memset(buf, 0, sizeof(buf));
    offset = CONFIG_FLASH_SIZE*4; /*0x40000*4 = 0x100000(1M)*/
    flash_read_mtdname(CONFIG_FILE_PATH, buf, offset, strlen(magicStr));

    if(strcmp(buf, magicStr) != 0)
    {
	printf("relocate the profile in the old partition ...\n");

	sn = 0;
	/*get sn from the old partition. offset:0x80f4000*/
	flash_read_mtdname("/dev/sda4", (unsigned char*)&sn, 0x80f4000, 4);

	/*write to the current partition*/
	flash_write_mtdname("/dev/sda3", (unsigned char*)&sn, 0, 4);
#if 0
	/*clear sn in the old inpartition*/
	sn = 0;
	flash_write_mtdname("/dev/sda4", (unsigned char*)&sn, 0x80f4000, 4);
#endif
	memset(buf, 0, sizeof(buf));
	/*get xml profile from the old partition. offset:0x4ec0000*/
	flash_read_mtdname("/dev/sda4", buf, 0x4ec0000, CONFIG_FLASH_SIZE);                     

	/*write to the current partition*/
	flash_write_mtdname(CONFIG_FILE_PATH, buf, 0, CONFIG_FLASH_SIZE);

	/*tag*/
	flash_write_mtdname(CONFIG_FILE_PATH, magicStr, offset, strlen(magicStr));

	printf("done\n");
    }
}
#endif
int main(int argc, char** argv)
{
    int i = 0, findInstType = 0;
    Pointer prof = NULL;
    struct st_xmlConfig *conf = rootConfig;
    MibProfileType mibType = -1;
    char buf[MAX_VALUE_LEN] = {0};
    int max, min;
    int find_param_name = 0;
    struct st_xmlConfig *backconf = NULL;
    struProfAlloc *profhead = NULL;

    if (strcmp(argv[1],"get_hostname") == 0){
#if (BRAND_LEVEL_ONE == FYES)
	printf("LEVELONE");
#elif(GALAXYWIND == FYES)
	printf("GALAXYWIND");
#elif(MIDDLE_SOFT == FYES)
    printf("ADMIN");
#else
	printf("UTT");
#endif
	return;
    }
    if (strcmp(argv[1],"get_brandName") == 0){
#if (BRAND_LEVEL_ONE == FYES)
	printf("LEVELONE");
#elif(GALAXYWIND == FYES)
	printf("GALAXYWIND");
#elif(MIDDLE_SOFT == FYES)
    printf("ADMIN");
#else
	printf("UTT");
#endif
	return;
    }
    if (strcmp(argv[1],"get_brandHttp") == 0){
#if (BRAND_LEVEL_ONE == FYES)
	printf("http://www.level1.com");
#elif(GALAXYWIND == FYES)
	printf("");
#elif(MIDDLE_SOFT == FYES)
    printf("");
#else
	printf("http://utt.com.cn");
#endif
	return;
    }
    if (strcmp(argv[1],"get_brandNameC") == 0){
#if (BRAND_LEVEL_ONE == FYES)
	printf("LEVELONE");
#elif(GALAXYWIND == FYES)
	printf("GALAXYWIND");
#elif(MIDDLE_SOFT == FYES)
    printf("ADMIN");
#else
	printf("UTT C");
#endif
	return;
    }
    if (strcmp(argv[1],"get_brandDescriptions") == 0){
#if (BRAND_LEVEL_ONE == FYES)
	printf("AC1200 Dual Band Wireless Router");
#elif(GALAXYWIND == FYES)
	printf("GALAXYWIND");
#elif(MIDDLE_SOFT == FYES)
    printf("ADMIN");
#else
	printf("UTT 802.11 SoC Solution");
#endif
	return;
    }

#if ((UTT_WA_500N==FYES) || (UTT_WA_501N==FYES) || (UTT_AP_801 == FYES))
    if (strcmp(argv[1], "set_ra0_macaddr") == 0) {
	    ProfInit();
	    init_e2p_mac();
	    return;
    }
#endif    
#ifdef RELOCATE_OLD_PROFILE
    if (strcmp(argv[1], "relocate") == 0) {
	    readConfFromOldPartition();
	    return;
    }
#endif    

    if(argc < 2) {
        usage();
    }

    for(i = 2; i < argc; i++) {
        for(; conf->name != NULL; conf++) {
            if(strcmp(conf->name, argv[i]) == 0) {
                switch(conf->privilege) {
                    case INST:
                        mibType = conf->mibtype;
                        findInstType = 1;
                        break;
                    default:
                        break;
                }
                conf = conf->child;
                break;
            }
        }
        if(findInstType == 1) {
            break;
        }
    }

    ProfInit();
    /*
     * find the inst prof
     */
    if(findInstType == 1) {
        ProfInstLowHigh(mibType, &max, &min);
	if ((strcmp(argv[1], "get") == 0) || (strcmp(argv[1], "getbynm") == 0)) {
	    i++;
	    if((max <= 1) && (strcmp(argv[1], "get")==0)) {
		prof = ProfGetInstPointByIndex(mibType, 0);
	    } else {
		prof = ProfGetInstPointByName(mibType, argv[i]);
		i++;
	    }
	    if(prof == NULL) {
		ProfDetach();
		exit(1);
	    }

	    for(; i < argc; i++) {
		for(; conf->name != NULL; conf++) {
		    if(strcmp(conf->name, argv[i]) == 0) {
			if(conf->privilege == VALUE) {
			    parseProfileValue(conf, mibType, buf, prof);
			    printf("%s", buf);
			    ProfDetach();
			    exit(0);
			} else {
			    if(conf->child != NULL) {
				conf = conf->child;
				break;
			    } else {
				ProfDetach();
				exit(1);
			    }
			}

#if 0
                    switch(conf->privilege) {
                        case VALUE:
			    parseProfileValue(conf, mibType, buf, prof);
                            printf("%s", buf);
                            exit(0);
                            break;
                        default:
                            break;
                    }
#endif
		    }
		}
	    } /* for(i; i< argc;i++) */
	} else if (strcmp(argv[1], "new") == 0) { /*new ÃüÁî*/
	    /*new profile inst*/
		i++;
	    if(i+1<=argc) {
		if(uttcli_debug == 1) 
		fprintf(stderr, "para argv[%d]: %s\n",i,argv[i]);
		/* here should user new fun*/
		profileInst_new(mibType, argv[i]);
	    } else {
		fprintf(stderr, "please input inst name.\n");
		usage();
	    }
	    /*end new profile inst*/
	} else if (strcmp(argv[1], "delete") == 0) { /*delete ÃüÁî*/
	    /*delete profile inst*/
		i++;
	    if(i+1<=argc) {
		if(uttcli_debug == 1) 
		fprintf(stderr, "para argv[%d]: %s\n",i,argv[i]);
		/*back inst and delete*/
		profileInst_delete(mibType, argv[i]);
	    } else {
		fprintf(stderr, "please input inst name.\n");
		usage();
	    }
	    /*end delete profile inst*/
	} else if (strcmp(argv[1], "set") == 0) { /* setÃüÁî */
	    i++;
	if((i+1)>argc) { /*para error*/
	    fprintf(stderr, "please input inst name.\n");
	} else {
	    if(uttcli_debug == 1) 
	    fprintf(stderr, " argv[%d] =%s\n",i,argv[i]);
	    prof = ProfGetInstPointByName(mibType, argv[i]);
	    int inst_name_index = i;
	    
	    if(prof==NULL) {
		fprintf(stderr, "profile inst name error,can not find profile inst.\n");
	    } else {
		i++;
		if((i+1)>argc) { /*para error*/
		    /*print help info */
		    for(; conf->name != NULL; conf++) {
			    fprintf(stderr, "%s \n",conf->name);
		    }
		} else {
		for(;i<argc;i++) {
		backconf = conf;
		for(; conf->name != NULL; conf++) {
		    /*param name*/
		    if(strcmp(conf->name ,argv[i]) == 0) {
			find_param_name = 1;
			i++;
		    if(uttcli_debug == 1) 
		    fprintf(stderr,"%d: argv[%d] =%s\n",__LINE__,i,argv[i]);
			if((i+1)>argc) { /*para error*/
			    if(conf->child != NULL) {
				conf = conf->child;
				for(; conf->name != NULL; conf++) {
				    fprintf(stderr, "%s \n",conf->name);
				}
			    } else if((conf->itype == TINT)||(conf->itype == TUINT)) {
				fprintf(stderr,"please input value:\n");
				fprintf(stderr,"para type: int\n");
			    } else if(conf->itype == TSTR) {
				fprintf(stderr,"please input value:\n");
				fprintf(stderr,"para type: string\n");
#if (IP_SEC == FYES)
				if((mibType == MIB_PROF_IPSEC)) {
				    if(strcmp(conf->name, "transformSet") == 0) {
					fprintf(stderr, "you must specify para value with flowing value:\n");
					fprintf(stderr, "such as: tra_xxx[:tra_xxx][:tra_xxx][tra_xxx]\n");
					fprintf(stderr, "%11s  -  %21s\n","value","encryption ");
					int tra_i = 0;
					for(tra_i=0;tra_i<15;tra_i++) {
					    fprintf(stderr, "%11s  -  %21s\n",transArray[tra_i].name,transArray[tra_i].encrypt);
					}
				    } else if(strcmp(conf->name, "isakmpProfile") == 0) {
					fprintf(stderr, "you must specify para 'isakmpProfile' value with '%sA';\nyou must new 'isakmpConf' inst with name '%sA',or the ipsec inst function will not work.\n",argv[inst_name_index],argv[inst_name_index]);
				    }
				}
				

				if((mibType == MIB_PROF_ISAKMP)&&(strcmp(conf->name, "policy") == 0)) {
				    fprintf(stderr, "you must specify para value with flowing value:\n");
				    fprintf(stderr, "such as: p_xxx[:p_xxx][:p_xxx][p_xxx]\n");
				    fprintf(stderr, "%11s  -  %21s\n","value","encryption ");
				    int p_i = 0;
				    for(p_i=0;p_i<8;p_i++) {
					fprintf(stderr, "%11s  -  %21s\n",policyArray[p_i].name,policyArray[p_i].encrypt);
				    }
				}
#endif		
			    } else if(conf->itype == TENUM) {
				LcdParamEnumValue *tmp;
				tmp = (LcdParamEnumValue *)(conf->enumP);
				fprintf(stderr,"please input value:\n");
				fprintf(stderr,"enum type,select value: \n");
				for(tmp;((tmp->enumName!=NULL)&&(strcmp(tmp->enumName, "Null")!=0));tmp++) {
				    fprintf(stderr,"    %s\n",tmp->enumName);
				}
			    } else if(conf->itype == TIPADDR) {
				fprintf(stderr,"please input value:\n");
				fprintf(stderr, "para type: ip addr\n");
			    } else if(conf->itype == TMACADDR) {
				fprintf(stderr,"please input value:\n");
				fprintf(stderr, "para type: mac addr\n");
			    } else if(conf->itype == THEX) {
				fprintf(stderr,"please input value:\n");
				fprintf(stderr, "para type: hexadecimal number\n");
			    } else if(conf->itype == TMEMBERS) {
				fprintf(stderr,"please input value:\n");
				//fprintf(stderr, "para type: ip addr\n");
			    } else if(conf->itype == TIPENTRY) {
				fprintf(stderr,"please input value:\n");
				fprintf(stderr, "para type: ip entry \n");
			    }
			    break;
			} else {
			if(conf->privilege == VALUE) { /*param value*/
			    if((i+1)<=argc) {
				//ProfBackupByIndex(mibType, profction, index, &profhead);
				int back_index = PROFFAIL;
				back_index = ProfBackupByName(mibType, PROF_CHANGE_EDIT, argv[inst_name_index], &profhead);
				if(uttcli_debug == 1) 
				fprintf(stderr,"%s-%d: mibType: %d, back_index: %d\n",__func__,__LINE__,mibType,back_index);
#if (IP_SEC == FYES)
				if((mibType == MIB_PROF_IPSEC)) {
				if(strcmp(conf->name, "transformSet") == 0) {
				    int trns_success = 0;
				    int trans_error = 0;
				    int trans_num = 0;
				    int trans_index = 0;
				    char trans_str[MAX_TRANSFORMSETS_LENGTH+1];
				    memset(trans_str, 0, sizeof(trans_str));
				    strncpy(trans_str, argv[i],MAX_TRANSFORMSETS_LENGTH);
				    if((strlen(trans_str)<=0)||(strlen(trans_str)+1)%8 != 0) {
					trans_error = 1;
				    } else {
					trans_num = (strlen(trans_str)+1)/8;
					for(trans_index=0;trans_index<trans_num;trans_index++) {
					    int p_i = 0;
					    for(p_i=0;p_i<15;p_i++) {
						if(strncmp(transArray[p_i].name, &(trans_str[0+trans_index*8]),7) ==0) {
						    trns_success = 1;
						    break;
						}
					    }
					    trans_str[0+trans_index*8+7] = '\0';
					    if(trns_success == 0) {
						trans_error = 1;
						break;
					    }
					    trns_success = 0;
					}
				    }
				    if(trans_error == 1) {
					fprintf(stderr, "you must specify para value with flowing value:\n");
					fprintf(stderr, "such as: tra_xxx[:tra_xxx][:tra_xxx][tra_xxx]\n");
					fprintf(stderr, "%11s  -  %21s\n","value","encryption ");
					int tra_i = 0;
					for(tra_i=0;tra_i<15;tra_i++) {
					    fprintf(stderr, "%11s  -  %21s\n",transArray[tra_i].name,transArray[tra_i].encrypt);
					}
					return 0;
				    }
				    fprintf(stderr,"you should verify 'transformsetConf' insts with name %s %s %s %s are existed,or the ipsec inst will not work.\n",&trans_str[0],&trans_str[8],&trans_str[16],&trans_str[24]);

				} else if(strcmp(conf->name, "isakmpProfile") ==0) {
				    char isakmp_name[UTT_INST_NAME_LEN+1];
				    memset(isakmp_name, 0, UTT_INST_NAME_LEN+1);
				    snprintf(isakmp_name, UTT_INST_NAME_LEN+1, "%sA",argv[inst_name_index]);
				    if(strcmp(isakmp_name, argv[i]) !=0) {
					fprintf(stderr, "you must specify para 'isakmpProfile' value with '%sA'.\n",argv[inst_name_index]);
					return 0;
				    }
				    fprintf(stderr, "you should verify a 'isakmpConf' inst with name '%sA' is existed,or the ipsec inst function will not work.\n",argv[inst_name_index]);
				}
				}

				if((mibType == MIB_PROF_ISAKMP)&&(strcmp(conf->name, "policy") == 0)) {
				    int policy_success = 0;
				    int policy_error = 0;
				    int policy_num = 0;
				    int policy_index = 0;
				    char policy_str[MAX_ISAKMPPOLICY_LENGTH+1];
				    memset(policy_str, 0, sizeof(policy_str));
				    strncpy(policy_str, argv[i],MAX_ISAKMPPOLICY_LENGTH);
				    if((strlen(policy_str)<=0)||(strlen(policy_str)+1)%6 != 0) {
					policy_error = 1;
				    } else {
					policy_num = (strlen(policy_str)+1)/6;
					for(policy_index=0;policy_index<policy_num;policy_index++) {
					    int p_i = 0;
					    for(p_i=0;p_i<8;p_i++) {
						if(strncmp(policyArray[p_i].name, &(policy_str[0+policy_index*6]),5) ==0) {
						    policy_success = 1;
						    break;
						}
					    }
					    policy_str[0+policy_index*6+5] = '\0';
					    if(policy_success == 0) {
						policy_error = 1;
						break;
					    }
					    policy_success = 0;
					}
				    }
				    if(policy_error == 1) {
					fprintf(stderr, "you must specify para value with flowing value:\n");
					fprintf(stderr, "such as: p_xxx[:p_xxx][:p_xxx][p_xxx]\n");
					fprintf(stderr, "%11s  -  %21s\n","value","encryption ");
					int p_i = 0;
					for(p_i=0;p_i<8;p_i++) {
					    fprintf(stderr, "%11s  -  %21s\n",policyArray[p_i].name,policyArray[p_i].encrypt);
					}
					return 0;
				    }
				    fprintf(stderr,"you should verify 'policyConf' insts with name %s %s %s %s are existed, or the ipsec inst will not work.\n",&policy_str[0],&policy_str[6],&policy_str[12],&policy_str[18]);
				}
#endif		
#if 1 /*edit interface  0*/
/*check lan ip config error or not 
 * and will update dhcp server config*/
    if((mibType == MIB_PROF_INTERFACE)&&(strcmp(argv[inst_name_index],"0") == 0)&&(strcmp(conf->name, "ip") == 0)) {
				
    Uint32 lanIp = 0U;
#if 0
    if(checkIpAddr(argv[i]) == 0) {
	fprintf(stderr,"ip addr error.\n");
	return 0;
    }
#endif
    lanIp = ip2int(argv[i]);
#if ((DHCP_SERVER == FYES))
    unsigned int nIp, eIp, uNet, nIp1, eIp1;
    MibProfileType mibTypeDhcpS = MIB_PROF_DHCP_SERVER;
    DhcpServerProfile *profDhcpS = NULL;
    /*
     * »ñµÃdhcpÆðÊ¼½áÊøip
     */
    profDhcpS = (DhcpServerProfile*)ProfGetInstPointByIndex(mibTypeDhcpS, 0);
    if (profDhcpS != NULL) {
    
	nIp = ntohl(profDhcpS->ipStart);
	eIp = ntohl(profDhcpS->ipEnd);
	/* Ö÷»úÐò£¬lan¿ÚÍø¶Î, dhcpµØÖ·³ØÍø¶Î */
	uNet = lanIp & 0xffffff00;
	/* dhcpµØÖ·³ØËælanIp±ä»¯ºóµÄµØÖ·,Ö÷»ú×Ö½ÚÐò */
	nIp1 = uNet + (nIp & 0x000000ff);
	eIp1 = uNet + (eIp & 0x000000ff);

	fprintf(stderr,"%s: lanIp=0x%x,nIp=0x%x,eIp=0x%x,uNet=0x%x,nIp1=0x%x,eIp1=0x%x\n", 
	    __FUNCTION__ ,lanIp, nIp, eIp, uNet, nIp1, eIp1);
	if((lanIp <= eIp1) && (lanIp >= nIp1)){
	    /* mantis 8712 */
	    if (nIp1 != nIp) {
		fprintf(stderr,"±ä¶¯ºóµÄDHCPµØÖ·³Ø½«°üº¬LAN¿ÚµØÖ·£¬²Ù×÷Ê§°Ü\n");
	    } else {
		fprintf(stderr,"DHCPµØÖ·³Ø°üº¬ÁËLAN¿ÚµØÖ·\n");
	    }
		return 0;
	}
#if 0
	/*
	 * ÅÐ¶ÏµØÖ·³Ø
	 */
	cfPoolType = ipPoolConflict(IP_POOL_DHCP, htonl(nIp1), htonl(eIp1));
	if (cfPoolType != IP_POOL_NONE) {
		setPoolConflictMsg(cfPoolType);
		goto out;
	}
#endif

	ProfBackupByIndex(mibTypeDhcpS, PROF_CHANGE_EDIT, 0, &profhead);
	if(profDhcpS->gateway != htonl(lanIp))
	{
		/*
		 * IPµØÖ·¶ÎÐÞ¸Ä
		 */
		profDhcpS->ipStart = htonl(nIp1);
		profDhcpS->ipEnd = htonl(eIp1);
		profDhcpS->gateway = htonl(lanIp);
	}
    }
#endif
#if (STATIC_NAT == FYES)
    MibProfileType mibTypeNat = MIB_PROF_STATIC_NAT;
    StaticNatProfile *profNat = NULL;
    /*
     * Ô¶³Ì¹ÜÀí¾²Ì¬nat ÄÚÍøip¸ù¾Ýlan±ä»¯
     */
    profNat = (StaticNatProfile *)ProfGetInstPointByName(mibTypeNat, remCtrlName);
    if (profNat != NULL) {
        profNat->IP = htonl(lanIp);
    }
#endif
#if (PPTP_SERVER == FYES)
    MibProfileType mibTypePptp = MIB_PROF_PPTP_SRV_GLOBAL;
    PPTPSrvGlobalProfile *profPptp = NULL;
    /* ¼àÌýip±ä»¯ */
    profPptp = (PPTPSrvGlobalProfile *)ProfGetInstPointByIndex(mibTypePptp, 0);
    if (profPptp != NULL) {
        ProfBackupByIndex(mibTypePptp, PROF_CHANGE_EDIT, 0, &profhead);
	profPptp->listenIp = htonl(lanIp);
#if (STATIC_NAT == FYES)
	/* pptp¾²Ì¬Ó³Éä±ä»¯ */
	if (profPptp->head.active == 1) {
	    profNat = (StaticNatProfile *)ProfGetInstPointByName(mibTypeNat, "pptp");
	    if (profNat != NULL) {
		profNat->IP = htonl(lanIp);
	    }
	}
#endif
    }
#endif
    }
#if (PPTP_SERVER == FYES)
    else if((mibType == MIB_PROF_PPTP_SRV_GLOBAL)&&(strcmp(argv[inst_name_index],"pptpServer") == 0)&&(strcmp(conf->name, "active") == 0)) {
#if (STATIC_NAT == FYES)
    MibProfileType mibTypeNat = MIB_PROF_STATIC_NAT;
    StaticNatProfile *profNat = NULL;
    int instIndex = PROFFAIL;
    Uint32 lanIp = 0U;
    char ipStr[16]={0};
    getIfIp((char *)getLanIfName(), ipStr);
    lanIp = ip2int(ipStr);
    instIndex = ProfGetInstIndexByName(mibTypeNat, "pptp");
    if (strncmp(argv[i], "No", 3) == 0) {
	/* ´æÔÚÔòÉ¾³ý */
	if (instIndex != PROFFAIL) {
	    ProfBackupByIndex(mibTypeNat, PROF_CHANGE_DEL, instIndex, &profhead);
	    ProfDelInstByIndex(mibTypeNat, instIndex);
	}
    } else {
	/* ²»´æÔÚÔòÐÂ½¨ */
	if (instIndex == PROFFAIL) {
	    instIndex = ProfNewInst(mibTypeNat, "pptp", FALSE);
	    if (instIndex != PROFFAIL) {
	    
	    ProfBackupByIndex(mibTypeNat, PROF_CHANGE_ADD, instIndex, &profhead);
	    profNat = (StaticNatProfile *)ProfGetInstPointByIndex(mibTypeNat, instIndex);
	    ProfSetNameByPoint(profNat, "pptp");
	    profNat->head.active = 1;
	    profNat->protocol = STATIC_NAT_TCP;
	    profNat->OutPort = 1723;
	    profNat->IP = ntohl(lanIp);
	    profNat->InnerPort = 1723;
	    profNat->PortNum = 1;
	    strcpy(profNat->NatBind, "WAN1");
	    }
	}
    }
#endif
    }
#endif

#endif /*end edit interface 0 */
				mxml_node_t node;
				mxml_node_t node_value;
				memset(&node, 0, sizeof(node));
				memset(&node_value, 0, sizeof(node_value));
				node_value.value.text.string = (argv[i]);
				node.child = &node_value;
				fprintf(stderr, "node set value string: %s\n",node.child->value.text.string);
				if(CheckDate(argv[inst_name_index+1],node_value.value.text.string,conf) == 0)
				{
				    fprintf(stderr,"ERROR:fail to set!\n");
				    exit(0);
				}
				if(strcmp(conf->name,"UpnpActive")==0){
				    setWanUpnp(node_value.value.text.string);
				    ProfFreeAllocList(profhead);
				    nvramWriteCommit();
				    return;
				}
				parseProfileParaFromXml(conf, &node, prof);
				
				/* update profile inst
				 * and save profile
				 */
				//if((strcmp(argv[argc-1], "done") == 0)||(mibType == MIB_PROF_SYSTEM)) 
				{
				   // ProfUpdate(profhead);
				    fprintf(stderr, "you make profile inst '%s' to work.\n",argv[inst_name_index]);
				}
				ProfFreeAllocList(profhead);
				nvramWriteCommit();
			    }
			} else if(conf->child != NULL){
				conf = conf->child;
				i--;
				break;
			}
			}
		    } else {
		    }
		}
		if(find_param_name == 0) {
		    /*print help info */
		    if(backconf!=NULL) {
			for(; backconf->name != NULL; backconf++) {
			    fprintf(stderr, "%s \n",backconf->name);
			}
		    }
		}
		}
		}
	    }
	}
	    /*set profile inst papram value*/
	} else if (strcmp(argv[1], "inst") == 0) { /* instÃüÁî */
	    mibHead *prof1 = NULL;
	    for (i = min; i < max; i++) {
		prof1 = (mibHead *)ProfGetInstPointByIndex(mibType, i);
		if (ProfInstIsFree(prof1) == 0) {
		    printf("%s ", prof1->name);
		}
	    }
	} 
#if (WIRELESS == FYES)
	else if (strcmp(argv[1], "make_ap_config") == 0) {
	    prof = ProfGetInstPointByIndex(mibType, 0);
	    gen_ap_config(prof);
	}
#if (WIRELESS_5G == FYES)
	else if (strcmp(argv[1], "make_ap_config_5g") == 0) {
	    prof = ProfGetInstPointByIndex(mibType, 1);
	    gen_ap_config_5g(prof);
	}
#endif
#endif
	else {

	}
    }
    else {
	conf = rootConfig;
	if ((strcmp(argv[1], "set")==0)||(strcmp(argv[1], "new")==0)||(strcmp(argv[1],"delete")==0)||(strcmp(argv[1],"get") ==0)||(strcmp(argv[1],"inst")==0)) { /* setÃüÁî */
	    for(; conf->name != NULL; conf++) {
		fprintf(stderr, "   %s\n",conf->name);
	    }
	}
    }
    ProfDetach();
    exit(0);
}
