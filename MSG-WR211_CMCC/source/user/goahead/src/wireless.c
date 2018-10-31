/* vi: set sw=4 ts=4 sts=4: */
/*
 *	wireless.c -- Wireless Settings 
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: wireless.c,v 1.13.14.2 2015/08/17 16:03:36 hou.bo Exp $
 */

#include	<stdlib.h>
#include	<sys/ioctl.h>
#include	<arpa/inet.h>
#include	"../../autoconf.h"
#ifdef CONFIG_DEFAULTS_KERNEL_2_6_21
  #include	<linux/types.h>
  #include	<linux/socket.h>
  #include	<linux/if.h>
#endif
#include	<linux/wireless.h>

#include	"internet.h"
#include	"management.h"
#include	"nvram.h"
#include	"utils.h"
#include	"webs.h"
#include	"wireless.h"
#define AP_MODE
#include	"oid.h"

#include	"linux/autoconf.h"
#include	"uttfunction.h"
#include	"translate.h"

/*
 * RT2860, RTINIC, RT2561
 */
static int default_shown_mbssid[3]  = {0,0,0};

extern int g_wsc_configured;

#if 0
static int  getWlan11aChannels(int eid, webs_t wp, int argc, char_t **argv);
static int  getWlan11bChannels(int eid, webs_t wp, int argc, char_t **argv);
static int  getWlan11gChannels(int eid, webs_t wp, int argc, char_t **argv);
static int  getWlanApcliBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getWlanChannel(int eid, webs_t wp, int argc, char_t **argv);
#endif
static int  getWlanCurrentMac(int eid, webs_t wp, int argc, char_t **argv);
static int  getWlanStationList(int eid, webs_t wp, int argc, char_t **argv);
static void formConfigAddAllMAC(webs_t wp, char * path, char * query) ;
static int getStatiionFromIoctl( webs_t wp, char *wlanMac,int printfOrAddMac);
#if 0
static int  getDLSBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getDFSBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getCarrierBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getWlanM2UBuilt(int eid, webs_t wp, int argc, char_t **argv);
#endif
static void wirelessBasic(webs_t wp, char_t *path, char_t *query);
//add by cxhu
static int getWirelessBaseConfig(int eid, webs_t wp, int argc, char_t **argv);
static int getWirelessAdvancedConfig(int eid, webs_t wp, int argc, char_t **argv);
static int getWirelessSecurityConfig(int eid, webs_t wp, int argc, char_t **argv);
static void formWlanMacFilterGlobalConfig(webs_t wp, char_t *path, char_t *query);
static int getWlanMacFilterMacList(int eid, webs_t wp, int argc, char_t **argv);
static void formConfigWlanMacFilter(webs_t wp, char_t *path, char_t *query);
static void formConfigWlanMacFilterDel(webs_t wp, char_t *path, char_t *query);
static void formConfigWlanMacFilterDelAll(webs_t wp, char_t *path, char_t *query);
static int getWlanRunStation(int eid, webs_t wp, int argc, char_t **argv);
static int getWlanIp(int eid, webs_t wp, int argc, char_t **argv, int wlanNoUp);
static int getWlanMac(int eid, webs_t wp, int argc, char_t **argv, int wlanNoUp);
static int getWlanNetmask(int eid, webs_t wp, int argc, char_t **argv, int wlanNoUp);
//end by cxhu
static void wirelessAdvanced(webs_t wp, char_t *path, char_t *query);
static void wirelessWds(webs_t wp, char_t *path, char_t *query);
static void wirelessApcli(webs_t wp, char_t *path, char_t *query);
#if 0
static void wirelessWmm(webs_t wp, char_t *path, char_t *query);
#endif
static void wirelessGetSecurity(webs_t wp, char_t *path, char_t *query);
static void APSecurity(webs_t wp, char_t *path, char_t *query);
static int  isWPSConfiguredASP(int eid, webs_t wp, int argc, char_t **argv);
int deleteNthValueMulti(int index[], int count, char *value, char delimit);		/* for Access Policy list deletion*/
static void APDeleteAccessPolicyList(webs_t wp, char_t *path, char_t *query);
void DeleteAccessPolicyList(int nvram, webs_t wp, char_t *path, char_t *query);
#ifdef CONFIG_RT2860V2_AP_MESH
static void wirelessMesh(webs_t wp, char_t *path, char_t *query);
static void meshManualLink(webs_t wp, char_t *path, char_t *query);
static int ShowMeshState(int eid, webs_t wp, int argc, char_t **argv);
#endif

void formDefineWireless(void) {
#if 0
	websAspDefine(T("getWlan11aChannels"), getWlan11aChannels);
	websAspDefine(T("getWlan11bChannels"), getWlan11bChannels);
	websAspDefine(T("getWlan11gChannels"), getWlan11gChannels);
	websAspDefine(T("getWlanApcliBuilt"), getWlanApcliBuilt);
	websAspDefine(T("getWlanChannel"), getWlanChannel);
#endif
	websAspDefine(T("getWlanCurrentMac"), getWlanCurrentMac);
	//websAspDefine(T("aspOutWlanStationList"), getWlanStationList);
#if 0
	websAspDefine(T("getDLSBuilt"), getDLSBuilt);
	websAspDefine(T("getDFSBuilt"), getDFSBuilt);
	websAspDefine(T("getCarrierBuilt"), getCarrierBuilt);
	websAspDefine(T("getWlanM2UBuilt"), getWlanM2UBuilt);
#endif
	websAspDefine(T("outputWlanRunStation"), getWlanRunStation);//add by cxhu getWlanStaInfo
	
	websAspDefine(T("isWPSConfiguredASP"), isWPSConfiguredASP);
//	websAspDefine(T("isAntennaDiversityBuilt"), isAntennaDiversityBuilt);
//	websAspDefine(T("outputWlanBaseList"), getWirelessBaseConfig);
	websAspDefine(T("aspOutWirelessAdvancedConfig"), getWirelessAdvancedConfig);
	//websAspDefine(T("outputWlanMacFilterConfig"), getWlanMacFilterMacList);
	//websFormDefine(T("ConfigWlanMacFilterDelAll"), formConfigWlanMacFilterDelAll);
	//websFormDefine(T("ConfigWlanMacFilterDel"), formConfigWlanMacFilterDel);
	//websFormDefine(T("ConfigWlanMacFilter"), formConfigWlanMacFilter);
	//websFormDefine(T("ConfigWlanMacFilterGlobalConfig"), formWlanMacFilterGlobalConfig);
	//websFormDefine(T("ConfigAddAllMAC"), formConfigAddAllMAC);
	

#if 0
#ifdef CONFIG_RT2860V2_AP_MESH
	websAspDefine(T("ShowMeshState"), ShowMeshState);
#endif
#ifdef CONFIG_RT2860V2_RT3XXX_AP_ANTENNA_DIVERSITY
	websFormDefine(T("AntennaDiversity"), AntennaDiversity);
	websFormDefine(T("getAntenna"), getAntenna);
#endif
#endif
	//websFormDefine(T("ConfigWirelessBase"), wirelessBasic);
	websFormDefine(T("wirelessAdvanced"), wirelessAdvanced);
	websFormDefine(T("wirelessWds"), wirelessWds);
	websFormDefine(T("wirelessApcli"), wirelessApcli);
#if 0
	websFormDefine(T("wirelessWmm"), wirelessWmm);
#endif
	websFormDefine(T("wirelessGetSecurity"), wirelessGetSecurity);
#if 0
	websAspDefine(T("aspOutWlanSecurityConfig"), getWirelessSecurityConfig);
#endif
	websFormDefine(T("ConfigWlanMacFilterGlobalConfig"), formWlanMacFilterGlobalConfig);
	
#if 0
	websFormDefine(T("APSecurity"), APSecurity);
#endif
	websFormDefine(T("APDeleteAccessPolicyList"), APDeleteAccessPolicyList);
#if 0
#ifdef CONFIG_RT2860V2_AP_MESH
	websFormDefine(T("wirelessMesh"), wirelessMesh);
	websFormDefine(T("meshManualLink"), meshManualLink);
#endif
#endif
}

#if 0
/*
 * description: write 802.11a channels in <select> tag
 */
static int getWlan11aChannels(int eid, webs_t wp, int argc, char_t **argv)
{
	int  idx = 0, channel;
	char *value = nvram_bufget(RT2860_NVRAM,"CountryCode");
	char *channel_s = nvram_bufget(RT2860_NVRAM, "Channel");

	channel = (channel_s == NULL)? 0 : atoi(channel_s);
	if ((value == NULL) || (strcmp(value, "") == 0) ||
			(strcmp(value, "US") == 0) || (strcmp(value, "FR") == 0) ||
			(strcmp(value, "IE") == 0) || (strcmp(value, "JP") == 0) ||
			(strcmp(value, "HK") == 0)) {
		for (idx = 0; idx < 4; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
	}
	if ((value == NULL) || (strcmp(value, "") == 0) ||
			(strcmp(value, "US") == 0) || (strcmp(value, "FR") == 0) ||
			(strcmp(value, "IE") == 0) || (strcmp(value, "TW") == 0) ||
			(strcmp(value, "HK") == 0)) {
		for (idx = 4; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
	}
	if ((value == NULL) || (strcmp(value, "") == 0)) {
		for (idx = 16; idx < 27; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
	}
	if ((value == NULL) || (strcmp(value, "") == 0) ||
			(strcmp(value, "US") == 0) || (strcmp(value, "TW") == 0) ||
			(strcmp(value, "CN") == 0) || (strcmp(value, "HK") == 0)) {
		for (idx = 28; idx < 32; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	}
	if ((value == NULL) || (strcmp(value, "") == 0) ||
			(strcmp(value, "US") == 0) || (strcmp(value, "CN") == 0) ||
			(strcmp(value, "HK") == 0)) {
		return websWrite(wp,
				T("<option value=165 %s>5825MHz (Channel 165)</option>\n"),
				(165 == channel)? "selected" : "");
	}
	return 0;
}

/*
 * description: write 802.11b channels in <select> tag
 */
static int getWlan11bChannels(int eid, webs_t wp, int argc, char_t **argv)
{
	int idx = 0, channel;
	char *value = nvram_bufget(RT2860_NVRAM, "CountryCode");
	char *channel_s = nvram_bufget(RT2860_NVRAM, "Channel");

	channel = (channel_s == NULL)? 0 : atoi(channel_s);
	if ((value == NULL) || (strcmp(value, "") == 0) ||
			(strcmp(value, "US") == 0) || (strcmp(value, "JP") == 0) ||
			(strcmp(value, "FR") == 0) || (strcmp(value, "IE") == 0) ||
			(strcmp(value, "TW") == 0) || (strcmp(value, "CN") == 0) ||
			(strcmp(value, "HK") == 0)) {
		for (idx = 0; idx < 11; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	}

	if ((value == NULL) || (strcmp(value, "") == 0) ||
			(strcmp(value, "JP") == 0) || (strcmp(value, "TW") == 0) ||
			(strcmp(value, "FR") == 0) ||
			(strcmp(value, "IE") == 0) || (strcmp(value, "CN") == 0) ||
			(strcmp(value, "HK") == 0)) {
		for (idx = 11; idx < 13; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	}

	if ((value == NULL) || (strcmp(value, "") == 0) ||
			(strcmp(value, "JP") == 0)) {
		return websWrite(wp,
				T("<option value=14 %s>2484MHz (Channel 14)</option>\n"),
				(14 == channel)? "selected" : "");
	}
	return 0;
}

/*
 * description: write 802.11g channels in <select> tag
 */
static int getWlan11gChannels(int eid, webs_t wp, int argc, char_t **argv)
{
	int idx = 0, channel;
	char *value = nvram_bufget(RT2860_NVRAM, "CountryCode");
	char *channel_s = nvram_bufget(RT2860_NVRAM, "Channel");

	channel = (channel_s == NULL)? 0 : atoi(channel_s);
	if ((value == NULL) || (strcmp(value, "") == 0) ||
			(strcmp(value, "US") == 0) || (strcmp(value, "JP") == 0) ||
			(strcmp(value, "FR") == 0) || (strcmp(value, "IE") == 0) ||
			(strcmp(value, "TW") == 0) || (strcmp(value, "CN") == 0) ||
			(strcmp(value, "HK") == 0)) {
		for (idx = 0; idx < 11; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	}                                                                           
	if ((value == NULL) || (strcmp(value, "") == 0) ||
			(strcmp(value, "JP") == 0) || (strcmp(value, "TW") == 0) ||
			(strcmp(value, "FR") == 0) || (strcmp(value, "IE") == 0) ||
			(strcmp(value, "CN") == 0) || (strcmp(value, "HK") == 0)) {
		for (idx = 11; idx < 13; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	}

	if ((value == NULL) || (strcmp(value, "") == 0)) {
		return websWrite(wp,
				T("<option value=14 %s>2484MHz (Channel 14)</option>\n"),
				(14 == channel)? "selected" : "");
	}
	return 0;
}

static int getWlanApcliBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_RT2860V2_AP_APCLI
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

/*
 * description: write channel number or 0 if auto-select
 */
static int getWlanChannel(int eid, webs_t wp, int argc, char_t **argv)
{
	char *value = nvram_bufget(RT2860_NVRAM, "AutoChannelSelect");

	if (NULL == value)
		return websWrite(wp, T("9"));
	if (!strncmp(value, "1", 2))
		return websWrite(wp, T("0"));

	value = nvram_bufget(RT2860_NVRAM, "Channel");
	if (NULL == value)
		return websWrite(wp, T("9"));
	else
		return websWrite(wp, T("%s"), value);
}

#endif
/*
 * description: write MAC address from interface 'ra0'
 */
static int getWlanCurrentMac(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_hw[18] = {0};

	if (-1 == getIfMac("ra0", if_hw))
		return websWrite(wp, T(" "));
	return websWrite(wp, T("%s"), if_hw);
}

typedef union _MACHTTRANSMIT_SETTING {
	struct  {
		unsigned short  MCS:7;  // MCS
		unsigned short  BW:1;   //channel bandwidth 20MHz or 40 MHz
		unsigned short  ShortGI:1;
		unsigned short  STBC:2; //SPACE
		unsigned short  rsv:3;
		unsigned short  MODE:2; // Use definition MODE_xxx.
	} field;
	unsigned short      word;
} MACHTTRANSMIT_SETTING;

typedef struct _RT_802_11_MAC_ENTRY {
#if  defined(CONFIG_RALINK_RT5350) || defined(CONFIG_RALINK_MT7620) || defined(CONFIG_RALINK_MT7621)|| defined(CONFIG_RALINK_MT7628)
	unsigned char           ApIdx;
#endif
	unsigned char            Addr[6];
	unsigned char            Aid;
	unsigned char            Psm;     // 0:PWR_ACTIVE, 1:PWR_SAVE
	unsigned char            MimoPs;  // 0:MMPS_STATIC, 1:MMPS_DYNAMIC, 3:MMPS_Enabled
	char                     AvgRssi0;
	char                     AvgRssi1;
	char                     AvgRssi2;
	unsigned int             ConnectedTime;
	MACHTTRANSMIT_SETTING    TxRate;
#if defined(CONFIG_RALINK_RT5350)
	unsigned int LastRxRate;
	int StreamSnr[3];
	int SoundingRespSnr[3];
#elif defined(CONFIG_RALINK_MT7620) || defined(CONFIG_RALINK_MT7621) || defined(CONFIG_RALINK_MT7628)
	unsigned int LastRxRate;
	short StreamSnr[3];
	short SoundingRespSnr[3];
#endif				/* RTMP_RBUS_SUPPORT */
} RT_802_11_MAC_ENTRY;

typedef struct _RT_802_11_MAC_TABLE {
	unsigned long            Num;
#ifndef AP_MAX_CLIENT_NUM
#if defined(CONFIG_BOARD_MTK7621_F)
	RT_802_11_MAC_ENTRY      Entry[116]; //MAX_LEN_OF_MAC_TABLE = 32
#else
	RT_802_11_MAC_ENTRY      Entry[64]; //MAX_LEN_OF_MAC_TABLE = 32
#endif
#else
	RT_802_11_MAC_ENTRY      Entry[AP_MAX_CLIENT_NUM]; //MAX_LEN_OF_MAC_TABLE = 32
#endif
} RT_802_11_MAC_TABLE;
int s;
int currentMacFilterCounter = 0;
static int getWlanStationList(int eid, webs_t wp, int argc, char_t **argv)
{
       char *wlanMac;
	 char str[32];
	 int mbssid=0;
    	 sprintf(str, "AccessControlList%d", mbssid);
        websWrite(wp,"var HostMacs=new Array();");
        websWrite(wp,"var Allows=new Array();");
        websWrite(wp,"var BWs=new Array();");
	 websWrite(wp,"var SGIs=new Array();");
        wlanMac = nvram_bufget(RT2860_NVRAM,str);
	
        getStatiionFromIoctl(wp, wlanMac, 1);
        getTheErrorMsg(eid, wp, argc, argv);  
        return 0;
}
static void formConfigAddAllMAC(webs_t wp, char * path, char * query) 
{
    a_assert(wp);
    char *wlanMac;
    char str[32];
    int mbssid=0;
    sprintf(str, "AccessControlList%d", mbssid);
    wlanMac = nvram_bufget(RT2860_NVRAM,str);
    getStatiionFromIoctl(wp, wlanMac, 0);
    websRedirect(wp, "WlanHostInfo.asp");
}
static int getStatiionFromIoctl( webs_t wp, char *wlanMac,int printfOrAddMac)
{
	int i,totalrece=0, iscommit=0;
	struct iwreq iwr;
	int mbssid=0;
	char str[32];
	char *newMac,*oldMacList,*filterrule=NULL;
	RT_802_11_MAC_TABLE table = {0};

	s = socket(AF_INET, SOCK_DGRAM, 0);
	strncpy(iwr.ifr_name, "ra0", IFNAMSIZ);
	iwr.u.data.pointer = (caddr_t) &table;

	if (s < 0) {
		return -1;
	}

#ifdef CONFIG_RT2860V2_AP_V24_DATA_STRUCTURE 
    if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT, &iwr) >= 0)
#else
	if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE, &iwr) < 0) {
#endif
	    close(s);
	    if(printfOrAddMac)
		websWrite(wp,"var totalrecs=%d;", 0);
	    return -1;
	}
	for (i = 0; i < table.Num; i++) {
		if(printfOrAddMac){
			char *HostMacs = T("HostMacs[%d] = \"%s\"; ");
        		char *Allows = T("Allows[%d] = \"%s\"; ");
       	      char *BWs = T("BWs[%d] = \"%s\"; ");
			// char *SGIs = T("SGIs[%d] = \"%u\"; ");
			char buf[32];
			sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", table.Entry[i].Addr[0], table.Entry[i].Addr[1],table.Entry[i].Addr[2], table.Entry[i].Addr[3],table.Entry[i].Addr[4], table.Entry[i].Addr[5]);
			websWrite(wp,HostMacs, totalrece, buf);
			if(strstri(wlanMac, buf))
           			 websWrite(wp,Allows, totalrece, T("yes"));
        		else
            			websWrite(wp,Allows, totalrece, T("no"));
			memset(buf, 0,32);
			if(table.Entry[i].TxRate.field.BW == 0)
				sprintf(buf, "20M");
			else
				sprintf(buf, "40M");
			websWrite(wp,BWs, totalrece, buf);
		//print_sta_stats(wp, si->isi_macaddr, totalrece, wlanMac, athIf);
		 }    
		else{	
			
    			char mac[20],str2[32];
   			 int macStrLen=0;
			
	 		
    			sprintf(str, "AccessControlList%d", mbssid);
			oldMacList=nvram_bufget(RT2860_NVRAM,str);
			sprintf(str2, "AccessPolicy%d", mbssid);
	 		filterrule=nvram_bufget(RT2860_NVRAM,str2);
    			memset(mac, 0, 20);
    			sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",  table.Entry[i].Addr[0], table.Entry[i].Addr[1],table.Entry[i].Addr[2], table.Entry[i].Addr[3],table.Entry[i].Addr[4], table.Entry[i].Addr[5]);
			//macStrLen = strlen(wlanMac);
			macStrLen = strlen(oldMacList);
   			if(currentMacFilterCounter >= UTT_MAX_WLAN_MAC_FILTER){
				return 0;
			}
   			if(strstri(oldMacList,mac)){//strstri(wlanMac,mac)
				//return;
				continue;
			}

   			newMac = (char*)malloc(sizeof(char) * (macStrLen + 20));
    			memset(newMac, 0, (macStrLen + 20));
    			strcpy(newMac, oldMacList);//strcpy(newMac, wlanMac)
    			if(strlen(newMac) != 0)
        		strcat(newMac, UTT_SPLIT_SIGN_RECORDS);
    			strcat(newMac, mac);
    			nvram_bufset(RT2860_NVRAM, str, newMac);
			iscommit=1;
			//nvram_commit(RT2860_NVRAM);
    				//shengxiao
   			free(newMac);
		}
		totalrece++;
	}
	if(printfOrAddMac)
	    websWrite(wp,"var totalrecs=%d;", totalrece);
	close(s);
	if(iscommit){
		nvram_commit(RT2860_NVRAM);//for bindall mac 
		oldMacList = nvram_bufget(RT2860_NVRAM,str);
		//doSystem("iwpriv ra0 set ACLClearAll=1");
        doSystem("iwpriv ra0 set \"ACLAddEntry=%s\" ",oldMacList);
        if(filterrule){
		    if(!strcmp(filterrule,"2"))
		        doSystem("iwpriv ra0 set AccessPolicy=%s",filterrule);//shengxiao
        }
#if 0 
		mbssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));
		doSystem("ralink_init make_wireless_config rt2860");
		for(i=0; i<mbssid_num; i++){
			doSystem("ifconfig ra%d down", i);
		}
		for(i=0; i<mbssid_num; i++){
			doSystem("ifconfig ra%d up", i);
		}
#endif
	}
	return 0;
}

#if 0
static int getDLSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_RT2860V2_AP_DLS
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getDFSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_RT2860V2_AP_DFS
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getCarrierBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_RT2860V2_AP_CARRIER
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getWlanM2UBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_RT2860V2_AP_IGMP_SNOOP
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static void revise_mbss_value(int old_num, int new_num)
{
	/* {{{ The parameters that support multiple BSSID is listed as followed,
	   1.) SSID,                 char SSID[33];
	   2.) AuthMode,             char AuthMode[14];
	   3.) EncrypType,           char EncrypType[8];
	   4.) WPAPSK,               char WPAPSK[65];
	   5.) DefaultKeyID,         int  DefaultKeyID;
	   6.) Key1Type,             int  Key1Type;
	   7.) Key1Str,              char Key1Str[27];
	   8.) Key2Type,             int  Key2Type;
	   9.) Key2Str,              char Key2Str[27];
	   10.) Key3Type,            int  Key3Type;
	   11.) Key3Str,             char Key3Str[27];
	   12.) Key4Type,            int  Key4Type;
	   13.) Key4Str,             char Key4Str[27];
	   14.) AccessPolicy,
	   15.) AccessControlList,
	   16.) NoForwarding,
	   17.) IEEE8021X,           int  IEEE8021X;
	   18.) TxRate,              int  TxRate;
	   19.) HideSSID,            int  HideSSID;
	   20.) PreAuth,             int  PreAuth;
	   21.) WmmCapable
	                             int  SecurityMode;
                             	 char VlanName[20];
	                             int  VlanId;
	                             int  VlanPriority;
	}}} */
	char new_value[264], *old_value, *p;
	int i;

#define MBSS_INIT(field, default_value) \
	do { \
		old_value = nvram_bufget(RT2860_NVRAM, #field); \
		snprintf(new_value, 264, "%s", old_value); \
		p = new_value + strlen(old_value); \
		for (i = old_num; i < new_num; i++) { \
			snprintf(p, 264 - (p - new_value), ";%s", default_value); \
			p += 1 + strlen(default_value); \
		} \
		nvram_bufset(RT2860_NVRAM, #field, new_value); \
	} while (0)

#define MBSS_REMOVE(field) \
	do { \
		old_value = nvram_bufget(RT2860_NVRAM, #field); \
		snprintf(new_value, 264, "%s", old_value); \
		p = new_value; \
		for (i = 0; i < new_num; i++) { \
			if (0 == i) \
				p = strchr(p, ';'); \
			else \
				p = strchr(p+1, ';'); \
			if (NULL == p) \
				break; \
		} \
		if (p) \
			*p = '\0'; \
		nvram_bufset(RT2860_NVRAM, #field, new_value); \
	} while (0)

	if (new_num > old_num) {
		//MBSS_INIT(SSID, "ssid");
		MBSS_INIT(AuthMode, "OPEN");
		MBSS_INIT(EncrypType, "NONE");
		//MBSS_INIT(WPAPSK, "12345678");
		MBSS_INIT(DefaultKeyID, "1");
		MBSS_INIT(Key1Type, "0");
		//MBSS_INIT(Key1Str, "");
		MBSS_INIT(Key2Type, "0");
		//MBSS_INIT(Key2Str, "");
		MBSS_INIT(Key3Type, "0");
		//MBSS_INIT(Key3Str, "");
		MBSS_INIT(Key4Type, "0");
		//MBSS_INIT(Key4Str, "");
/*		MBSS_INIT(AccessPolicy0, "0");
		MBSS_INIT(AccessControlList0, "");
		MBSS_INIT(AccessPolicy1, "0");
		MBSS_INIT(AccessControlList1, "");
		MBSS_INIT(AccessPolicy2, "0");
		MBSS_INIT(AccessControlList2, "");
		MBSS_INIT(AccessPolicy3, "0");
		MBSS_INIT(AccessControlList3, ""); */
		MBSS_INIT(NoForwarding, "0");
		MBSS_INIT(NoForwardingBTNBSSID, "0");
		MBSS_INIT(IEEE8021X, "0");
		MBSS_INIT(TxRate, "0");
		//MBSS_INIT(HideSSID, "0");
		MBSS_INIT(PreAuth, "0");
		MBSS_INIT(WmmCapable, "1");
		for (i = old_num + 1; i <= new_num; i++) {
			nvram_bufset(RT2860_NVRAM, racat("WPAPSK", i), "12345678");
			nvram_bufset(RT2860_NVRAM, racat("Key1Str", i), "");
			nvram_bufset(RT2860_NVRAM, racat("Key2Str", i), "");
			nvram_bufset(RT2860_NVRAM, racat("Key3Str", i), "");
			nvram_bufset(RT2860_NVRAM, racat("Key4Str", i), "");
			// The index of AccessPolicy & AccessControlList starts at 0.
			nvram_bufset(RT2860_NVRAM, racat("AccessPolicy", i-1), "0");
			nvram_bufset(RT2860_NVRAM, racat("AccessControlList", i-1), "");
		}
	}
	else if (new_num < old_num) {
		//MBSS_REMOVE(SSID);
		MBSS_REMOVE(AuthMode);
		MBSS_REMOVE(EncrypType);
		//MBSS_REMOVE(WPAPSK);
		MBSS_REMOVE(DefaultKeyID);
		MBSS_REMOVE(Key1Type);
		//MBSS_REMOVE(Key1Str);
		MBSS_REMOVE(Key2Type);
		//MBSS_REMOVE(Key2Str);
		MBSS_REMOVE(Key3Type);
		//MBSS_REMOVE(Key3Str);
		MBSS_REMOVE(Key4Type);
		//MBSS_REMOVE(Key4Str);
/*		MBSS_REMOVE(AccessPolicy0);
		MBSS_REMOVE(AccessControlList0);
		MBSS_REMOVE(AccessPolicy1);
		MBSS_REMOVE(AccessControlList1);
		MBSS_REMOVE(AccessPolicy2);
		MBSS_REMOVE(AccessControlList2);
		MBSS_REMOVE(AccessPolicy3);
		MBSS_REMOVE(AccessControlList3); */
		MBSS_REMOVE(NoForwarding);
		MBSS_REMOVE(NoForwardingBTNBSSID);
		MBSS_REMOVE(IEEE8021X);
		MBSS_REMOVE(TxRate);
		MBSS_REMOVE(HideSSID);
		MBSS_REMOVE(PreAuth);
		MBSS_REMOVE(WmmCapable);
		for (i = new_num + 1; i <= old_num; i++) {
			nvram_bufset(RT2860_NVRAM, racat("SSID", i), "");
			nvram_bufset(RT2860_NVRAM, racat("WPAPSK", i), "");
			nvram_bufset(RT2860_NVRAM, racat("Key1Str", i), "");
			nvram_bufset(RT2860_NVRAM, racat("Key2Str", i), "");
			nvram_bufset(RT2860_NVRAM, racat("Key3Str", i), "");
			nvram_bufset(RT2860_NVRAM, racat("Key4Str", i), "");
			// The index of AccessPolicy & AccessControlList starts at 0.
			nvram_bufset(RT2860_NVRAM, racat("AccessPolicy", i-1), "0");
			nvram_bufset(RT2860_NVRAM, racat("AccessControlList", i-1), "");
		}
	}
}
#endif

static void initWlan()
{
    sendMsgForSpeed(INIT_WLAN, 0, 0, NULL, 0);
}

/* goform/ConfigWirelessBase
 * add by cxhu 091221
 * for the wireless baseconfig
 */
static void wirelessBasic(webs_t wp, char_t *path, char_t *query)
{
	char_t	*wirelessmode,*apworkmode,*ssid, *hssid,*apmac,*apssid,*sz11gChannel,*n_bandwidth,*radio;
	char	 hidden_ssid[16];
	int is_n=0;
	const char * ErrMsg = NULL;
	//wirelessmode=apworkmode=ssid=hssid=apmac=sz11gChannel=n_bandwidth=radio=NULL;
	//fetch from web input
	radio = websGetVar(wp, T("WrlessEnable"), T(""));//wrlessenable//2
	apworkmode = websGetVar(wp, T("APWorkMode"), T(""));
	wirelessmode = websGetVar(wp, T("wrlessMode"), T("")); //9: bgn mode//9
	ssid = websGetVar(wp, T("ssid"), T("")); //ssid
	hssid = websGetVar(wp, T("broadcastEnable"), T("")); //ssid广播
	sz11gChannel = websGetVar(wp, T("channel"), T("")); //信道
	n_bandwidth = websGetVar(wp, T("chanWidth"), T(""));//频道带宽//0
	apmac=websGetVar(wp, T("wds_list"), T(""));//ap-macadd
	apssid=websGetVar(wp, T("APssid"), T(""));//ap-ssid

	//secmode
	char_t *mode, *enc, *wpapsk, *keyid, *keytype,
			*key1, *key2, *key3, *key4,*keylen1,*keylen2,*keylen3,*keylen4,*wdswpapsk;

	//fetch from web input
	mode = websGetVar(wp, T("apcli_mode"), T("OPEN"));
	enc = websGetVar(wp, T("apcli_enc"), T("NONE"));
	wpapsk = websGetVar(wp, T("apclipskPsswd"), T("12345678"));//预共享密钥
	wdswpapsk = websGetVar(wp, T("wdspskPsswd"), T(""));//WDS预共享密钥
	keyid = websGetVar(wp, T("apclikeynum"), T("1"));//密钥
	keytype = websGetVar(wp, T("apclikeyFormat"), T("1"));//密钥格式
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
//	printf("radio=%s,,wirelessmode=%s,ssid=%s,hssid=%s,sz11gChannel=%s,n_bandwidth=%s,apmac=%s\n",radio,wirelessmode,ssid,hssid,sz11gChannel,n_bandwidth,apmac);

	if (!strncmp(radio, "on", 3)) {
		//doSystem("iwpriv ra0 set RadioOn=1");
		nvram_bufset(RT2860_NVRAM, "RadioOff", "0");
	}
	 else{
		//doSystem("iwpriv ra0 set RadioOn=0");
		nvram_bufset(RT2860_NVRAM, "RadioOff", "1");
	 }
	nvram_bufset(RT2860_NVRAM, "WirelessMode", wirelessmode);
	
	//BasicRate: bg,bgn,n:15, b:3; g,gn:351
	if (!strncmp(wirelessmode, "4", 2) || !strncmp(wirelessmode, "7", 2)) //g, gn
		nvram_bufset(RT2860_NVRAM, "BasicRate", "351");
	else if (!strncmp(wirelessmode, "1", 2)) //b
		nvram_bufset(RT2860_NVRAM, "BasicRate", "3");
	else //bg,bgn,n
		nvram_bufset(RT2860_NVRAM, "BasicRate", "15");

	if (!strncmp(wirelessmode, "6", 2) || !strncmp(wirelessmode, "9", 2))
		is_n = 1;

	//SSID, Multiple SSID
	if (0 == strlen(ssid)) {
		nvram_commit(RT2860_NVRAM);
		ErrMsg = getTransDataForC(C_LANG_INDEX_SSID_CANT_NULL);
		setTheErrorMsg(ErrMsg);
           	 goto out;
	}
	nvram_bufset(RT2860_NVRAM, "SSID1", ssid);
     if (strchr(hssid, '0') != NULL)
		sprintf(hidden_ssid, "%s", "1");
	else
		sprintf(hidden_ssid, "%s", "0");
	
	//Broadcast SSID
	//nvram_bufset(RT2860_NVRAM, "HideSSID", hidden_ssid);//8位or 1位?
	

	//11abg Channel or AutoSelect
	if (0 == strlen(sz11gChannel)) {
		nvram_commit(RT2860_NVRAM);
		ErrMsg = getTransDataForC(C_LANG_INDEX_CHANNEL_CANT_NULL);
		setTheErrorMsg(ErrMsg);
           	 goto out;
	}
	if (!strncmp(sz11gChannel, "0", 2))
		nvram_bufset(RT2860_NVRAM, "AutoChannelSelect", "1");
	else
		nvram_bufset(RT2860_NVRAM, "AutoChannelSelect", "0");
	if (0 != strlen(sz11gChannel))
	{
		nvram_bufset(RT2860_NVRAM, "Channel", sz11gChannel);
		doSystem("iwpriv ra0 set Channel=%s", sz11gChannel);
	}
	
//	if (is_n) 
	{
		nvram_bufset(RT2860_NVRAM, "HT_BW", n_bandwidth);
	}
	//ap workmode
    int deleteWan4Flag=0;
    char *oldworkmode=nvram_bufget(RT2860_NVRAM, "apworkmode");
    if( (strcmp(apworkmode, "5") !=0) && (strcmp(oldworkmode, "5") ==0) ) {
        deleteWan4Flag=1;
    }
	if (!strncmp(apworkmode, "1", 2)) {//ap
		nvram_bufset(RT2860_NVRAM, "HideSSID", hidden_ssid);//8位or 1位?
		nvram_bufset(RT2860_NVRAM, "WdsEnable", T("0"));
	}
	
	else if(!strncmp(apworkmode, "5", 2)){//apclient
	    apmac=websGetVar(wp, T("APCliMacs"), T(""));
		nvram_bufset(RT2860_NVRAM, "ApCliEnable", "1");
		nvram_bufset(RT2860_NVRAM, "OperationMode", T("3"));//change the opmode to apcli
		nvram_bufset(RT2860_NVRAM, "HideSSID", hidden_ssid);
		nvram_bufset(RT2860_NVRAM, "ApCliBssid", apmac);
		nvram_bufset(RT2860_NVRAM, "ApCliSsid", apssid);
		nvram_bufset(RT2860_NVRAM, "WdsEnable", T("0"));
		
		nvram_bufset(RT2860_NVRAM, "ApCliAuthMode", mode);
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
		
	}
	else if(!strncmp(apworkmode, "3", 2)){//repeater
		nvram_bufset(RT2860_NVRAM, "WdsEnable", apworkmode);
		nvram_bufset(RT2860_NVRAM, "WdsList", apmac);
		nvram_bufset(RT2860_NVRAM, "HideSSID", hidden_ssid);//8位or 1位?
	}
	else if(!strncmp(apworkmode, "2", 2)){//p-t-p /bridge
		nvram_bufset(RT2860_NVRAM, "WdsEnable", apworkmode);
		nvram_bufset(RT2860_NVRAM, "WdsList", apmac);
	}
	else{//p-t-mp/azy mode
		nvram_bufset(RT2860_NVRAM, "WdsEnable", apworkmode);
		nvram_bufset(RT2860_NVRAM, "HideSSID", hidden_ssid);//8位or 1位?
	}
	if (strncmp(apworkmode, "5", 2)) {
		nvram_bufset(RT2860_NVRAM, "ApCliEnable", "0");
		nvram_bufset(RT2860_NVRAM, "OperationMode", T("1"));//change the opmode to gateway
		}
	//sleep(1);
	if(!strncmp(apworkmode, "2", 2)||!strncmp(apworkmode, "3", 2)||!strncmp(apworkmode, "4", 2)){//secmode
		if(!strncmp(mode,"TKIP",5)||!strncmp(mode,"AES",4)){
			nvram_bufset(RT2860_NVRAM, "WdsKey", wdswpapsk);
			nvram_bufset(RT2860_NVRAM, "WdsPhyMode", "1");
			//nvram_bufset(RT2860_NVRAM, "Wds1Key", wdswpapsk);
			//nvram_bufset(RT2860_NVRAM, "Wds2Key", wdswpapsk);
			//nvram_bufset(RT2860_NVRAM, "Wds3Key", wdswpapsk);
		}
		else if(!strncmp(mode,"WEP",4)){
#if 0
			nvram_bufset(RT2860_NVRAM, "WdsDefaultKeyId", keyid);
			nvram_bufset(RT2860_NVRAM, "Wds0Key", key1);
			nvram_bufset(RT2860_NVRAM, "Wds1Key", key2);
			nvram_bufset(RT2860_NVRAM, "Wds2Key", key3);
			nvram_bufset(RT2860_NVRAM, "Wds3Key", key4);
			nvram_bufset(RT2860_NVRAM, "WdsKeyType", keytype);
			nvram_bufset(RT2860_NVRAM, "WdsKey1Len", keylen1);
			nvram_bufset(RT2860_NVRAM, "WdsKey2Len", keylen2);
			nvram_bufset(RT2860_NVRAM, "WdsKey3Len", keylen3);
			nvram_bufset(RT2860_NVRAM, "WdsKey4Len", keylen4);
#else
			nvram_bufset(RT2860_NVRAM, "DefaultKeyID", keyid);
			nvram_bufset(RT2860_NVRAM, "Key1Str1", key1);
			nvram_bufset(RT2860_NVRAM, "Key2Str1", key2);
			nvram_bufset(RT2860_NVRAM, "Key3Str1", key3);
			nvram_bufset(RT2860_NVRAM, "Key4Str1", key4);
			nvram_bufset(RT2860_NVRAM, "Key1Type", keytype);
			nvram_bufset(RT2860_NVRAM, "Key2Type", keytype);
			nvram_bufset(RT2860_NVRAM, "Key3Type", keytype);
			nvram_bufset(RT2860_NVRAM, "Key4Type", keytype);
			nvram_bufset(RT2860_NVRAM, "Key1Len", keylen1);
			nvram_bufset(RT2860_NVRAM, "Key2Len", keylen2);
			nvram_bufset(RT2860_NVRAM, "Key3Len", keylen3);
			nvram_bufset(RT2860_NVRAM, "Key4Len", keylen4);
			nvram_bufset(RT2860_NVRAM, "EncrypType", "WEP");
			nvram_bufset(RT2860_NVRAM, "AuthMode", "WEPAUTO");

#endif
		}
		nvram_bufset(RT2860_NVRAM, "WdsEncrypType", mode);
		
	}
	nvram_bufset(RT2860_NVRAM, "apworkmode", apworkmode);
	nvram_commit(RT2860_NVRAM);
//#if CONFIG_RT2860V2_AP == CONFIG_MIPS
	/* this is a workaround:
	 *  when AP is built as kernel
	 *  if more ssids are created, driver won't exe RT28xx_MBSS_Init again
	 *  therefore, we reboot to make it available
	 *  (PS. CONFIG_MIPS would be "y")
	 */
	//if (new_bssid_num > old_bssid_num)
		//doSystem("reboot");
//#endif
#if 1
//    sleep(1);
//    doSystem("ifconfig ra0 down");
	initWlan();
    if (deleteWan4Flag == 1 ) {
        //killDownWan(4);
    }
#endif
out:
    websRedirect(wp,"WlanBase.asp");
	
}
static int getWirelessBaseConfig(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t	*radio,*apworkmode;
	radio=nvram_bufget(RT2860_NVRAM,"RadioOff");
	apworkmode=nvram_bufget(RT2860_NVRAM,"apworkmode");
	if (!strncmp(radio, "0", 2)) {
		 websWrite(wp,"var WrlessEnables=\"%s\";",T("on"));
	}
	else{
		 websWrite(wp,"var WrlessEnables=\"%s\";",T("off"));
	}
    websWrite(wp,"var APWorkModes=\"%s\";",nvram_bufget(RT2860_NVRAM,"apworkmode"));
    websWrite(wp,"var ssids=\"%s\";",nvram_bufget(RT2860_NVRAM,"SSID1"));
    websWrite(wp,"var channels=\"%s\";",nvram_bufget(RT2860_NVRAM,"Channel"));
    websWrite(wp,"var broadcastEnables=\"%s\";",nvram_bufget(RT2860_NVRAM,"HideSSID"));
    websWrite(wp,"var wrlessModes=\"%s\";",nvram_bufget(RT2860_NVRAM,"WirelessMode"));
    websWrite(wp,"var APssids=\"%s\";",nvram_bufget(RT2860_NVRAM,"ApCliSsid"));
    
	websWrite(wp,"var APCliMac=\"%s\";",nvram_bufget(RT2860_NVRAM,"ApCliBssid"));
	websWrite(wp,"var APMacs=\"%s\";",nvram_bufget(RT2860_NVRAM,"WdsList"));
    websWrite(wp,"var chanWidths=\"%s\";\n", nvram_bufget(RT2860_NVRAM,"HT_BW"));
	
	websWrite(wp,"var apcliSecModes=\"%s\";", nvram_bufget(RT2860_NVRAM,"ApCliAuthMode"));
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

	websWrite(wp,"var wdsSecModes=\"%s\";", nvram_bufget(RT2860_NVRAM,"WdsEncrypType"));
	websWrite(wp,"var wdspskPsswds=\"%s\";", nvram_bufget(RT2860_NVRAM,"WdsKey"));
	//websWrite(wp,"var wdskeynums=\"%s\";", nvram_bufget(RT2860_NVRAM,"WdsDefaultKeyId"));
	websWrite(wp,"var wdskeynums=\"%s\";", nvram_bufget(RT2860_NVRAM,"DefaultKeyID"));
#if 0
	websWrite(wp,"var wdswepkey1s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Wds0Key"));
	websWrite(wp,"var wdswepkey2s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Wds1Key"));
	websWrite(wp,"var wdswepkey3s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Wds2Key"));
	websWrite(wp,"var wdswepkey4s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Wds3Key"));
#else
	websWrite(wp,"var wdswepkey1s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Key1Str1"));
	websWrite(wp,"var wdswepkey2s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Key2Str1"));
	websWrite(wp,"var wdswepkey3s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Key3Str1"));
	websWrite(wp,"var wdswepkey4s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Key4Str1"));
#endif
	//websWrite(wp,"var wdskeyFormats=\"%s\";\n", nvram_bufget(RT2860_NVRAM,"WdsKeyType"));
	websWrite(wp,"var wdskeyFormats=\"%s\";\n", nvram_bufget(RT2860_NVRAM,"Key1Type"));
	websWrite(wp,"var wdskeyLength1s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Key1Len"));
	websWrite(wp,"var wdskeyLength2s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Key2Len"));
	websWrite(wp,"var wdskeyLength3s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Key3Len"));
	websWrite(wp,"var wdskeyLength4s=\"%s\";", nvram_bufget(RT2860_NVRAM,"Key4Len"));

    getTheErrorMsg(eid, wp, argc, argv);  

    return 0;
}
static int getWlanRunStation(int eid, webs_t wp, int argc, char_t **argv)
{

	int wlanUp;
    
    wlanUp = getIfStatistic("ra0",HAVEINTERFACE);
	char_t	*radio;
	radio=nvram_bufget(RT2860_NVRAM,"RadioOff");
	if (!strncmp(radio, "0", 2)) {
		 websWrite(wp,"var wlessconnStats=\"%s\";",T("on"));
	}
	else{
		 websWrite(wp,"var wlessconnStats=\"%s\";",T("off"));
	}
    getWlanNetmask(eid, wp, argc, argv, (1- wlanUp));
    getWlanMac(eid, wp, argc, argv, (1- wlanUp));
    getWlanIp(eid, wp, argc, argv, (1- wlanUp));
    websWrite(wp,"var APWorkModes=\"%s\";",nvram_bufget(RT2860_NVRAM,"apworkmode"));
    websWrite(wp,"var ssids=\"%s\";",nvram_bufget(RT2860_NVRAM,"SSID1"));
    websWrite(wp,"var channels=\"%s\";",nvram_bufget(RT2860_NVRAM,"Channel"));
    websWrite(wp,"var wrlessModes=\"%s\";",nvram_bufget(RT2860_NVRAM,"WirelessMode"));
    return 0;
}
/*
 * description: write WLAN ip address accordingly
 */
static int getWlanIp(int eid, webs_t wp, int argc, char_t **argv, int wlanNoUp)
{
	char if_addr[16];
        char *out = T("wrlessIPs=\"%s\";");

	if (wlanNoUp || (-1 == getIfIp(getLanName(), if_addr))) {
		//websError(wp, 500, T("getWanIp: calling getIfIp error\n"));
		return websWrite(wp, out, T(""));
	}
	return websWrite(wp, out, if_addr);
}
/*
 * description: write WLAN MAC address accordingly
 */
static int getWlanMac(int eid, webs_t wp, int argc, char_t **argv, int wlanNoUp)
{
	char if_mac[18];
        char *out = T("wrlessMacs=\"%s\";");

	if (wlanNoUp || (-1 == getIfMac("ra0", if_mac))) {
		//websError(wp, 500, T("getLanIp: calling getIfMac error\n"));
		return websWrite(wp, out, T(""));
	}
	return websWrite(wp, out, if_mac);
}

/*
 * description: write WLAN subnet mask accordingly
 */
static int getWlanNetmask(int eid, webs_t wp, int argc, char_t **argv, int wlanNoUp)
{
	char if_net[16];
        char *out = T("wrlessMasks=\"%s\";");
        if (wlanNoUp || (-1 == getIfNetmask(getWanIfName(4), if_net))) {
		//websError(wp, 500, T("getWanNetmask: calling getIfNetmask error\n"));
		return websWrite(wp, out, T(""));
	}
	return websWrite(wp, out, if_net);
}
/* goform/wirelessAdvanced */
static void wirelessAdvanced(webs_t wp, char_t *path, char_t *query)
{
	char_t	 *beacon, *dtim, *fragment, *rts,*short_preamble, *wmm_capable;
	int	wlan_mode=0;

	//fetch from web input
	beacon = websGetVar(wp, T("beacon"), T("100"));
	dtim = websGetVar(wp, T("dtim"), T("1"));
	fragment = websGetVar(wp, T("fragment"), T("2346"));
	rts = websGetVar(wp, T("rts"), T("2347"));
	short_preamble = websGetVar(wp, T("short_preamble"), T("0"));
	wmm_capable = websGetVar(wp, T("wmm_capable"), T("0"));
	//printf("beacon=%s,dtim=%s,fragment=%s,rts=%s,short_preamble=%s,wmm_capable=%s,\n",beacon,dtim,fragment,rts,short_preamble,wmm_capable);
	wlan_mode = atoi(nvram_bufget(RT2860_NVRAM, "WirelessMode"));
	//set to nvram
	nvram_bufset(RT2860_NVRAM, "BeaconPeriod", beacon);
	nvram_bufset(RT2860_NVRAM, "DtimPeriod", dtim);
	nvram_bufset(RT2860_NVRAM, "FragThreshold1", fragment);
	nvram_bufset(RT2860_NVRAM, "RTSThreshold", rts);
	nvram_bufset(RT2860_NVRAM, "TxPreamble", short_preamble);
	nvram_bufset(RT2860_NVRAM, "WmmCapable", wmm_capable);//whether need atoi?

	if (!strncmp(wmm_capable, "1", 2)) {
		if (wlan_mode < 5)
			nvram_bufset(RT2860_NVRAM, "TxBurst", "0");
	}
	nvram_commit(RT2860_NVRAM);
	initWlan();
//out:
	
	websRedirect(wp,"WlanAdvanced.asp");
}
static int getWirelessAdvancedConfig(int eid, webs_t wp, int argc, char_t **argv)
{
    
    websWrite(wp,"var rtss=\"%s\";",nvram_bufget(RT2860_NVRAM,"RTSThreshold"));
    websWrite(wp,"var fragments=\"%s\";",nvram_bufget(RT2860_NVRAM,"FragThreshold1"));
    websWrite(wp,"var beacons=\"%s\";",nvram_bufget(RT2860_NVRAM,"BeaconPeriod"));
    websWrite(wp,"var dtims=\"%s\";",nvram_bufget(RT2860_NVRAM,"DtimPeriod"));
    websWrite(wp,"var short_preambles=\"%s\";",nvram_bufget(RT2860_NVRAM,"TxPreamble"));
    websWrite(wp,"var wmm_capables=\"%s\";", nvram_bufget(RT2860_NVRAM,"WmmCapable"));
    getTheErrorMsg(eid, wp, argc, argv);  

    return 0;
}
/* goform/wirelessWds */
static void wirelessWds(webs_t wp, char_t *path, char_t *query)
{
	char_t	*wds_mode, *wds_phy_mode, *wds_encryp_type, *wds_encryp_key0,
			*wds_encryp_key1,*wds_encryp_key2, *wds_encryp_key3, *wds_list;

	wds_mode = websGetVar(wp, T("wds_mode"), T("0"));
	wds_phy_mode = websGetVar(wp, T("wds_phy_mode"), T(""));
	wds_encryp_type = websGetVar(wp, T("wds_encryp_type"), T(""));
	wds_encryp_key0 = websGetVar(wp, T("wds_encryp_key0"), T(""));
	wds_encryp_key1 = websGetVar(wp, T("wds_encryp_key1"), T(""));
	wds_encryp_key2 = websGetVar(wp, T("wds_encryp_key2"), T(""));
	wds_encryp_key3 = websGetVar(wp, T("wds_encryp_key3"), T(""));
	wds_list = websGetVar(wp, T("wds_list"), T(""));

	nvram_bufset(RT2860_NVRAM, "WdsEnable", wds_mode);
	if (strncmp(wds_mode, "0", 2)) {
		nvram_bufset(RT2860_NVRAM, "WdsPhyMode", wds_phy_mode);
		nvram_bufset(RT2860_NVRAM, "WdsEncrypType", wds_encryp_type);
		nvram_bufset(RT2860_NVRAM, "Wds0Key", wds_encryp_key0);
		nvram_bufset(RT2860_NVRAM, "Wds1Key", wds_encryp_key1);
		nvram_bufset(RT2860_NVRAM, "Wds2Key", wds_encryp_key2);
		nvram_bufset(RT2860_NVRAM, "Wds3Key", wds_encryp_key3);
		if (!strncmp(wds_mode, "2", 2) || !strncmp(wds_mode, "3", 2)) {
			if (0 != strlen(wds_list))
				nvram_bufset(RT2860_NVRAM, "WdsList", wds_list);
		}
	}
	nvram_commit(RT2860_NVRAM);

	initInternet();

	//debug print
	websHeader(wp);
	websWrite(wp, T("wds_mode: %s<br>\n"), wds_mode);
	websWrite(wp, T("wds_phy_mode: %s<br>\n"), wds_phy_mode);
	websWrite(wp, T("wds_encryp_type: %s<br>\n"), wds_encryp_type);
	websWrite(wp, T("wds_encryp_key0: %s<br>\n"), wds_encryp_key0);
	websWrite(wp, T("wds_encryp_key1: %s<br>\n"), wds_encryp_key1);
	websWrite(wp, T("wds_encryp_key2: %s<br>\n"), wds_encryp_key2);
	websWrite(wp, T("wds_encryp_key3: %s<br>\n"), wds_encryp_key3);
	websWrite(wp, T("wds_list: %s<br>\n"), wds_list);
	websFooter(wp);
	websDone(wp, 200);
}

/* goform/wirelessApcli */
static void wirelessApcli(webs_t wp, char_t *path, char_t *query)
{
	char_t	*ssid, *bssid, *mode, *enc, *wpapsk, *keyid, *keytype,
			*key1, *key2, *key3, *key4;

	//fetch from web input
	ssid = websGetVar(wp, T("apcli_ssid"), T(""));
	bssid = websGetVar(wp, T("apcli_bssid"), T(""));
	mode = websGetVar(wp, T("apcli_mode"), T("OPEN"));
	enc = websGetVar(wp, T("apcli_enc"), T("NONE"));
	wpapsk = websGetVar(wp, T("apcli_wpapsk"), T("12345678"));
	keyid = websGetVar(wp, T("apcli_default_key"), T("1"));
	keytype = websGetVar(wp, T("apcli_key1type"), T("1"));
	key1 = websGetVar(wp, T("apcli_key1"), T(""));
	key2 = websGetVar(wp, T("apcli_key2"), T(""));
	key3 = websGetVar(wp, T("apcli_key3"), T(""));
	key4 = websGetVar(wp, T("apcli_key4"), T(""));

	if (gstrlen(ssid) == 0) {
		websError(wp, 200, "SSID is empty");
		return;
	}

	nvram_bufset(RT2860_NVRAM, "ApCliEnable", "1");
	nvram_bufset(RT2860_NVRAM, "ApCliSsid", ssid);
	nvram_bufset(RT2860_NVRAM, "ApCliBssid", bssid);
	nvram_bufset(RT2860_NVRAM, "ApCliAuthMode", mode);
	nvram_bufset(RT2860_NVRAM, "ApCliEncrypType", enc);
	nvram_bufset(RT2860_NVRAM, "ApCliWPAPSK", wpapsk);
	nvram_bufset(RT2860_NVRAM, "ApCliDefaultKeyId", keyid);
	nvram_bufset(RT2860_NVRAM, "ApCliKey1Type", keytype);
	nvram_bufset(RT2860_NVRAM, "ApCliKey1Str", key1);
	nvram_bufset(RT2860_NVRAM, "ApCliKey2Type", keytype);
	nvram_bufset(RT2860_NVRAM, "ApCliKey2Str", key2);
	nvram_bufset(RT2860_NVRAM, "ApCliKey3Type", keytype);
	nvram_bufset(RT2860_NVRAM, "ApCliKey3Str", key3);
	nvram_bufset(RT2860_NVRAM, "ApCliKey4Type", keytype);
	nvram_bufset(RT2860_NVRAM, "ApCliKey4Str", key4);
	nvram_commit(RT2860_NVRAM);
	initInternet();

	//debug print
	websHeader(wp);
	websWrite(wp, T("ssid: %s<br>\n"), ssid);
	websWrite(wp, T("bssid: %s<br>\n"), bssid);
	websWrite(wp, T("mode: %s<br>\n"), mode);
	websWrite(wp, T("enc: %s<br>\n"), enc);
	if (!strcmp(mode, "WPAPSK") || !strcmp(mode, "WPA2PSK")) {
		websWrite(wp, T("wpapsk: %s<br>\n"), wpapsk);
	}
	if (!strcmp(mode, "OPEN") || !strcmp(mode, "SHARED")) {
		websWrite(wp, T("keyid: %s<br>\n"), keyid);
		websWrite(wp, T("keytype: %s<br>\n"), keytype);
		websWrite(wp, T("key1: %s<br>\n"), key1);
		websWrite(wp, T("key2: %s<br>\n"), key2);
		websWrite(wp, T("key3: %s<br>\n"), key3);
		websWrite(wp, T("key4: %s<br>\n"), key4);
	}
	websFooter(wp);
	websDone(wp, 200);
}

void WPSRestart(void);

#if 0
/* goform/wirelessWmm */
static void wirelessWmm(webs_t wp, char_t *path, char_t *query)
{
	char_t	*ap_aifsn_all, *ap_cwmin_all, *ap_cwmax_all, *ap_txop_all,
			*ap_acm_all, *ap_ackpolicy_all,
			*sta_aifsn_all, *sta_cwmin_all, *sta_cwmax_all, *sta_txop_all,
			*sta_acm_all;

	ap_aifsn_all = websGetVar(wp, T("ap_aifsn_all"), T(""));
	ap_cwmin_all = websGetVar(wp, T("ap_cwmin_all"), T(""));
	ap_cwmax_all = websGetVar(wp, T("ap_cwmax_all"), T(""));
	ap_txop_all = websGetVar(wp, T("ap_txop_all"), T(""));
	ap_acm_all = websGetVar(wp, T("ap_acm_all"), T(""));
	ap_ackpolicy_all = websGetVar(wp, T("ap_ackpolicy_all"), T(""));
	sta_aifsn_all = websGetVar(wp, T("sta_aifsn_all"), T(""));
	sta_cwmin_all = websGetVar(wp, T("sta_cwmin_all"), T(""));
	sta_cwmax_all = websGetVar(wp, T("sta_cwmax_all"), T(""));
	sta_txop_all = websGetVar(wp, T("sta_txop_all"), T(""));
	sta_acm_all = websGetVar(wp, T("sta_acm_all"), T(""));

	if (0 != strlen(ap_aifsn_all))
		nvram_bufset(RT2860_NVRAM, "APAifsn", ap_aifsn_all);
	if (0 != strlen(ap_cwmin_all))
		nvram_bufset(RT2860_NVRAM, "APCwmin", ap_cwmin_all);
	if (0 != strlen(ap_cwmax_all))
		nvram_bufset(RT2860_NVRAM, "APCwmax", ap_cwmax_all);
	if (0 != strlen(ap_txop_all))
		nvram_bufset(RT2860_NVRAM, "APTxop", ap_txop_all);
	if (0 != strlen(ap_acm_all))
		nvram_bufset(RT2860_NVRAM, "APACM", ap_acm_all);
	if (0 != strlen(ap_ackpolicy_all))
		nvram_bufset(RT2860_NVRAM, "AckPolicy", ap_ackpolicy_all);
	if (0 != strlen(sta_aifsn_all))
		nvram_bufset(RT2860_NVRAM, "BSSAifsn", sta_aifsn_all);
	if (0 != strlen(sta_cwmin_all))
		nvram_bufset(RT2860_NVRAM, "BSSCwmin", sta_cwmin_all);
	if (0 != strlen(sta_cwmax_all))
		nvram_bufset(RT2860_NVRAM, "BSSCwmax", sta_cwmax_all);
	if (0 != strlen(sta_txop_all))
		nvram_bufset(RT2860_NVRAM, "BSSTxop", sta_txop_all);
	if (0 != strlen(sta_acm_all))
		nvram_bufset(RT2860_NVRAM, "BSSACM", sta_acm_all);

	nvram_commit(RT2860_NVRAM);

	doSystem("ifconfig ra0 down");
	doSystem("ralink_init make_wireless_config rt2860");
	doSystem("ifconfig ra0 up");
	//after ra0 down&up we must restore WPS status
	WPSRestart();

	websHeader(wp);
	websWrite(wp, T("ap_aifsn_all: %s<br>\n"), ap_aifsn_all);
	websWrite(wp, T("ap_cwmin_all: %s<br>\n"), ap_cwmin_all);
	websWrite(wp, T("ap_cwmax_all: %s<br>\n"), ap_cwmax_all);
	websWrite(wp, T("ap_txop_all: %s<br>\n"), ap_txop_all);
	websWrite(wp, T("ap_acm_all: %s<br>\n"), ap_acm_all);
	websWrite(wp, T("ap_ackpolicy_all: %s<br>\n"), ap_ackpolicy_all);
	websWrite(wp, T("sta_aifsn_all: %s<br>\n"), sta_aifsn_all);
	websWrite(wp, T("sta_cwmin_all: %s<br>\n"), sta_cwmin_all);
	websWrite(wp, T("sta_cwmax_all: %s<br>\n"), sta_cwmax_all);
	websWrite(wp, T("sta_txop_all: %s<br>\n"), sta_txop_all);
	websWrite(wp, T("sta_acm_all: %s<br>\n"), sta_acm_all);
	websFooter(wp);
	websDone(wp, 200);
}
#endif

void restart8021XDaemon(int nvram)
{
	int i, num, apd_flag = 0;
	char *auth_mode = nvram_bufget(nvram, "AuthMode");
	char *ieee8021x = nvram_bufget(nvram, "IEEE8021X");
	char *num_s = nvram_bufget(nvram, "BssidNum");
	if(!num_s)
		return;
	num = atoi(num_s);

	if(nvram == RT2860_NVRAM)
		doSystem("killall -9 rt2860apd 1>/dev/null 2>&1");
	else if(nvram == RTINIC_NVRAM)
		doSystem("killall rtinicapd 1>/dev/null 2>&1");
	else if(nvram == RT2561_NVRAM)
		doSystem("killall rt61apd 1>/dev/null 2>&1");
	
	/*
	 * In fact we only support mbssid[0] to use 802.1x radius settings.
	 */
	for(i=0; i<num; i++){
		char tmp_auth[128];
		if( getNthValueSafe(i, auth_mode, ';', tmp_auth, 128) != -1){
			if(!strcmp(tmp_auth, "WPA") || !strcmp(tmp_auth, "WPA2") || !strcmp(tmp_auth, "WPA1WPA2")){
				apd_flag = 1;
				break;
			}
		}

		if( getNthValueSafe(i, ieee8021x, ';', tmp_auth, 128) != -1){
			if(!strcmp(tmp_auth, "1")){
				apd_flag = 1;
				break;
			}
		}
	}

	if(apd_flag){
		if(nvram == RT2860_NVRAM)
			doSystem("rt2860apd");	
		else if(nvram == RTINIC_NVRAM)
			doSystem("rtinicapd");
		else if(nvram == RT2561_NVRAM)
			doSystem("rt61apd");
	}
}


/* LFF means "Load From Flash" ...*/
#define LFF(result, nvram, x, n)	\
							do{		char tmp[128];										\
									if(! ( x  = nvram_bufget(nvram, #x)) )				\
										tmp[0] = '\0';									\
									else{												\
										if( getNthValueSafe(n, x, ';', tmp, 128) != -1){\
											gstrncat(result, tmp, 4096);				\
										}												\
									}gstrncat(result, "\r", 4096);						\
							}while(0)

/* Load from Web */
#define LFW(x, y)	do{												\
						if(! ( x = websGetVar(wp, T(#y), T(""))))	\
							return;									\
					}while(0)

void getSecurity(int nvram, webs_t wp, char_t *path, char_t *query)
{
	int num_ssid, i;
	char *num_s = nvram_bufget(nvram, "BssidNum");
	char_t result[4096];
	
	char_t *PreAuth, *AuthMode, *EncrypType, *DefaultKeyID, *Key1Type, *Key2Type,
		   *Key3Type, *Key4Type, *RekeyMethod, *RekeyInterval, *PMKCachePeriod, *IEEE8021X;
	char_t *RADIUS_Server, *RADIUS_Port, *RADIUS_Key, *session_timeout_interval;

//	printf("***** nvram = %d\n", nvram);

	if(num_s)
		num_ssid = atoi(num_s);
	else
		num_ssid = 1;

	result[0] = '\0';

	// deal with shown MBSSID
	if(default_shown_mbssid[nvram] > atoi(num_s))
		default_shown_mbssid[nvram] = 0;
	sprintf(result, "%d\n",  default_shown_mbssid[nvram]);

	if ((RT2860_NVRAM == nvram) || (RTINIC_NVRAM == nvram)) {
		for(i=0; i<num_ssid; i++) {
			//LFF(result, nvram, SSID, i);
			gstrncat(result, nvram_bufget(nvram, racat("SSID", i+1)), 4096);
			gstrncat(result, "\r", 4096);
			LFF(result, nvram, PreAuth, i);
			LFF(result, nvram, AuthMode, i);
			LFF(result, nvram, EncrypType, i);
			LFF(result, nvram, DefaultKeyID, i);
			LFF(result, nvram, Key1Type, i);
			//LFF(result, nvram, Key1Str, i);
			gstrncat(result, nvram_bufget(nvram, racat("Key1Str", i+1)), 4096);
			gstrncat(result, "\r", 4096);
			LFF(result, nvram, Key2Type, i);
			//LFF(result, nvram, Key2Str, i);
			gstrncat(result, nvram_bufget(nvram, racat("Key2Str", i+1)), 4096);
			gstrncat(result, "\r", 4096);
			LFF(result, nvram, Key3Type, i);
			//LFF(result, nvram, Key3Str, i);
			gstrncat(result, nvram_bufget(nvram, racat("Key3Str", i+1)), 4096);
			gstrncat(result, "\r", 4096);
			LFF(result, nvram, Key4Type, i);
			//LFF(result, nvram, Key4Str, i);
			gstrncat(result, nvram_bufget(nvram, racat("Key4Str", i+1)), 4096);
			gstrncat(result, "\r", 4096);
			//LFF(result, nvram, WPAPSK, i);
			gstrncat(result, nvram_bufget(nvram, racat("WPAPSK", i+1)), 4096);
			gstrncat(result, "\r", 4096);

			LFF(result, nvram, RekeyMethod, i);
			LFF(result, nvram, RekeyInterval, i);
			LFF(result, nvram, PMKCachePeriod, i);
			LFF(result, nvram, IEEE8021X, i);
			LFF(result, nvram, RADIUS_Server, i);
			LFF(result, nvram, RADIUS_Port, i);
			LFF(result, nvram, RADIUS_Key, i);
			LFF(result, nvram, session_timeout_interval, i);

			// access control related.
			gstrncat(result, nvram_bufget(nvram, racat("AccessPolicy", i)), 4096);
			gstrncat(result, "\r", 4096);
			gstrncat(result, nvram_bufget(nvram, racat("AccessControlList", i)), 4096);
			gstrncat(result, "\r", 4096);
			gstrncat(result, "\n", 4096);
		}
	} else if (RT2561_NVRAM == nvram) {
		char_t *SSID, *Key1Str, *Key2Str, *Key3Str, *Key4Str, *WPAPSK;
		for(i=0; i<num_ssid; i++) {
			LFF(result, nvram, SSID, i);
			LFF(result, nvram, PreAuth, i);
			LFF(result, nvram, AuthMode, i);
			LFF(result, nvram, EncrypType, i);
			LFF(result, nvram, DefaultKeyID, i);
			LFF(result, nvram, Key1Type, i);
			LFF(result, nvram, Key1Str, i);
			LFF(result, nvram, Key2Type, i);
			LFF(result, nvram, Key2Str, i);
			LFF(result, nvram, Key3Type, i);
			LFF(result, nvram, Key3Str, i);
			LFF(result, nvram, Key4Type, i);
			LFF(result, nvram, Key4Str, i);
			LFF(result, nvram, WPAPSK, i);
			LFF(result, nvram, RekeyMethod, i);
			LFF(result, nvram, RekeyInterval, i);
			LFF(result, nvram, PMKCachePeriod, i);
			LFF(result, nvram, IEEE8021X, i);
			LFF(result, nvram, RADIUS_Server, i);
			LFF(result, nvram, RADIUS_Port, i);
			LFF(result, nvram, RADIUS_Key, i);
			LFF(result, nvram, session_timeout_interval, i);
			gstrncat(result, "\n", 4096);

		/* access control related.
		   gstrncat(result, nvram_bufget(nvram, racat("AccessPolicy", i)), 4096);
		   gstrncat(result, "\r", 4096);
		   gstrncat(result, nvram_bufget(nvram, racat("AccessControlList", i)), 4096);
		   gstrncat(result, "\r", 4096);
		*/
		}
	}

	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	websWrite(wp, T("%s"), result);
	websDone(wp, 200);

}

static void wirelessGetSecurity(webs_t wp, char_t *path, char_t *query)
{
	return getSecurity(RT2860_NVRAM, wp, path, query);
}

/*
 *   TODO:   move to util.c?
 */
static void STFs(int nvram, int index, char *flash_key, char *value)
{
	char *result;
	char *tmp = nvram_bufget(nvram, flash_key);
	if(!tmp)
		tmp = "";
	result = setNthValue(index, tmp, value);
	nvram_bufset(nvram, flash_key, result);
	return ;
}


#define STF(nvram, index, flash_key)	STFs(nvram, index, #flash_key, flash_key)

void updateFlash8021x(int nvram)
{
	char lan_if_addr[16];
	char *RADIUS_Server;
	char *operation_mode;

	// if(! (operation_mode = nvram_bufget(nvram, "OperationMode")))
	if(! (operation_mode = nvram_bufget(RT2860_NVRAM, "OperationMode")))
		return;

	if(! (RADIUS_Server = nvram_bufget(nvram, "RADIUS_Server")))
		return;

	if(!strlen(RADIUS_Server))
		return;

	if(*operation_mode == '0'){ // Bridge
		if (getIfIp(getLanIfName(), lan_if_addr) == -1)
			return;
		nvram_bufset(nvram, "own_ip_addr", lan_if_addr);
		if ((RT2860_NVRAM == nvram) || (RTINIC_NVRAM == nvram)) {
			nvram_bufset(nvram, "EAPifname", getLanIfName());
			nvram_bufset(nvram, "PreAuthifname", getLanIfName());
		} else if (RT2561_NVRAM == nvram)
			nvram_bufset(nvram, "Ethifname", getLanIfName());
	}else if(*operation_mode == '1'){	// Gateway
		if (getIfIp(getLanIfName(), lan_if_addr) == -1)
			return;
		nvram_bufset(nvram, "own_ip_addr", lan_if_addr);
		if ((RT2860_NVRAM == nvram) || (RTINIC_NVRAM == nvram)) {
			nvram_bufset(nvram, "EAPifname", getLanIfName());
			nvram_bufset(nvram, "PreAuthifname", getLanIfName());
		} else if (RT2561_NVRAM == nvram)
			nvram_bufset(nvram, "Ethifname", getLanIfName());
	}else if(*operation_mode == '2'){	// Wireless gateway
		if (getIfIp(getLanIfName(), lan_if_addr) == -1)
			return;
		nvram_bufset(nvram, "own_ip_addr", lan_if_addr);
		if ((RT2860_NVRAM == nvram) || (RTINIC_NVRAM == nvram)) {
			nvram_bufset(nvram, "EAPifname", getLanIfName());
			nvram_bufset(nvram, "PreAuthifname", getLanIfName());
		} else if (RT2561_NVRAM == nvram)
			nvram_bufset(nvram, "Ethifname", getLanIfName());
	}else{
		return;
	}
	nvram_commit(nvram);
}


int AccessPolicyHandle(int nvram, webs_t wp, int mbssid)
{
	char_t *apselect, *newap_list;
	char str[32];
	char ap_list[2048];

	if(mbssid > 8 || mbssid < 0)
		return -1;

	sprintf(str, "apselect_%d", mbssid);	// UI on web page
	apselect = websGetVar(wp, str, T(""));
	if(!apselect){
		return -1;
	}

	sprintf(str, "AccessPolicy%d", mbssid);
	nvram_bufset(nvram, str, apselect);

	sprintf(str, "newap_text_%d", mbssid);
	newap_list = websGetVar(wp, str, T(""));
	if(!newap_list)
		return -1;
	if(!gstrlen(newap_list))
		return 0;
	sprintf(str, "AccessControlList%d", mbssid);
	sprintf(ap_list, "%s", nvram_bufget(nvram, str));
	if(strlen(ap_list))
		sprintf(ap_list, "%s;%s", ap_list, newap_list);
	else
		sprintf(ap_list, "%s", newap_list);

	nvram_bufset(nvram, str, ap_list);
	return 0;
}

static int getWlanMacFilterMacList(int eid, webs_t wp, int argc, char_t **argv)
{
    char *tmp = NULL, *wlanMac = NULL;
    int  num = 0;
    char isfirst = 1;
    char *macs = T("filterMacs[%d] = \"%s\"; ");
	char str[32],str1[32];
	int mbssid=0;
     sprintf(str, "AccessPolicy%d", mbssid);
    sprintf(str1, "AccessControlList%d", mbssid);
    websWrite(wp,"var MacFilterEnables=\"%s\";", nvram_bufget(RT2860_NVRAM,str));
    websWrite(wp,"var filterRules=\"%s\";", nvram_bufget(RT2860_NVRAM,"AccessPolicyRule"));

    websWrite(wp,"var filterMacs=new Array();");
    wlanMac = nvram_bufget(RT2860_NVRAM,str1);
//    printf("mac list %s\n",wlanMac);
    tmp = strtok(wlanMac, UTT_SPLIT_SIGN_RECORDS);

    while(tmp){
        websWrite(wp, macs, num, tmp);
        num++;
        if(isfirst){
            isfirst = 0;
        }else{
            *(tmp - 1) = UTT_SPLIT_SIGN_RECORD;
        }
        if(num > UTT_MAX_WLAN_MAC_FILTER)
            break;
        tmp = strtok(NULL, UTT_SPLIT_SIGN_RECORDS);
    }

    currentMacFilterCounter = num;
    websWrite(wp,"var totalrecs=%d;",num);

    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}
static void formConfigWlanMacFilter(webs_t wp, char_t *path, char_t *query)
{
	char str[32],str2[32],tmpmac[18]={0};
	int mbssid=0;
	
	char_t *action, *mac, *wlanMac, *tmp, *newMac, *oldMac, *pages,*filterrule;
    int macStrLen, isHostInfoPage = 0;
	const char * ErrMsg = NULL;
	
	sprintf(str, "AccessControlList%d", mbssid);
    a_assert(wp);
	 sprintf(str2, "AccessPolicy%d", mbssid);
	 filterrule=nvram_bufget(RT2860_NVRAM,str2);
    wlanMac = nvram_bufget(RT2860_NVRAM,str);
    macStrLen = strlen(wlanMac);
    pages = websGetVar(wp, T("pages"), T("")); 
    if(!strcmp(pages, "WlanHostInfo.asp")) {
        isHostInfoPage = 1;
    }
    action = websGetVar(wp, T("Action"), T("")); 
    mac = websGetVar(wp, T("filterMac"), T("")); 
    strcpy(tmpmac,mac);
	if(strlen(tmpmac)<=12)
		Conversion2StdMac(tmpmac);
    if((!strcmp(action, "add")) || isHostInfoPage){
        if(currentMacFilterCounter >= UTT_MAX_WLAN_MAC_FILTER){
			ErrMsg = getTransDataForC(C_LANG_INDEX_MAC_ADDR_LIST_MAX); 
            setTheErrorMsg(ErrMsg);
            websRedirect(wp, "WlanMacFilter.asp");
            return;
        }
        if(strstri(wlanMac,tmpmac)){
			ErrMsg = getTransDataForC(C_LANG_INDEX_MAC_ADDR_LIST_EXIT_ADDR); 
            setTheErrorMsg(ErrMsg);
            websRedirect(wp, "WlanMacFilter.asp");
            return;
        }
        newMac = (char*)malloc(sizeof(char) * (macStrLen + 20));
        memset(newMac, 0, (macStrLen + 20));
        strcpy(newMac, wlanMac);
        if(strlen(newMac) != 0)
            strcat(newMac, UTT_SPLIT_SIGN_RECORDS);
        strcat(newMac, tmpmac);
        nvram_bufset(RT2860_NVRAM, str, newMac);
        free(newMac);
    }
	else{
        oldMac = websGetVar(wp, T("oldMac"), T("")); 
        if(strcmp(oldMac,tmpmac)){
            if((!strstri(oldMac,tmpmac)) && strstri(wlanMac,tmpmac)){
				ErrMsg = getTransDataForC(C_LANG_INDEX_MAC_ADDR_LIST_EXIT_ADDR); 
				setTheErrorMsg(ErrMsg);
                websRedirect(wp, "WlanMacFilter.asp");
                return;
            }
        }else{
                websRedirect(wp, "WlanMacFilter.asp");
                return;
        }
        tmp = strstri(wlanMac,oldMac);
        if(tmp)
            memcpy(tmp, tmpmac, strlen(tmpmac));
        nvram_bufset(RT2860_NVRAM, str, wlanMac);
    }

   

    nvram_commit(RT2860_NVRAM);
	wlanMac = nvram_bufget(RT2860_NVRAM,str);
	//doSystem("iwpriv ra0 set ACLClearAll=1");
	doSystem("iwpriv ra0 set \"ACLAddEntry=%s\" ",wlanMac);
	if(!strcmp(filterrule,"2"))
		doSystem("iwpriv ra0 set AccessPolicy=%s",filterrule);//shengxiao
	// 生效
	#if 0 
	  mbssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));
		doSystem("ralink_init make_wireless_config rt2860");
		for(i=0; i<mbssid_num; i++){
			doSystem("ifconfig ra%d down", i);
		}
		for(i=0; i<mbssid_num; i++){
			doSystem("ifconfig ra%d up", i);
		}
	#endif
	
    if(isHostInfoPage) {
        websRedirect(wp, "WlanHostInfo.asp");
    } else {
        websRedirect(wp, "WlanMacFilter.asp");
    }
}
static void formConfigWlanMacFilterDel(webs_t wp, char_t *path, char_t *query)
{
    char_t *delStr, *wlanMac, *tmp, *tmp1, *newWlanMac, *pages,*filterrule;
    int macStrLen, isFrist = 1;
    int num = 0,mbssid=0;
	char str[32],str2[32];
    a_assert(wp);
    sprintf(str, "AccessControlList%d", mbssid);
    sprintf(str2, "AccessPolicy%d", mbssid);
    delStr = websGetVar(wp, T("delstr"), T("")); 
    pages = websGetVar(wp, T("pages"), T("")); 
    filterrule=nvram_bufget(RT2860_NVRAM,str2);
    if(!strcmp(delStr,"")){
        if(!strcmp(pages, "")){
            websRedirect(wp, "WlanMacFilter.asp");
        } else {
            websRedirect(wp, "WlanHostInfo.asp");
        }
        return;
    }
        
    wlanMac = nvram_bufget(RT2860_NVRAM,str);
    macStrLen = strlen(wlanMac);
    newWlanMac = (char*)malloc(sizeof(char) * macStrLen);
    memset(newWlanMac, 0, macStrLen);
    if(delStr && strcmp(delStr,"")){
        tmp = strtok(wlanMac, UTT_SPLIT_SIGN_RECORDS);
        while(tmp){
            tmp1 = strstri(delStr, tmp);
            if(!tmp1){
                num++;
                if(isFrist){
                    isFrist = 0;
                } else {
                    strcat(newWlanMac, UTT_SPLIT_SIGN_RECORDS);
                }
                strcat(newWlanMac, tmp); 
            }
            tmp = strtok(NULL, UTT_SPLIT_SIGN_RECORDS);
        }
    }
    nvram_bufset(RT2860_NVRAM, str, newWlanMac);

    currentMacFilterCounter = num;

    if(!strcmp(pages, "")){
        websRedirect(wp, "WlanMacFilter.asp");
    } else {
        websRedirect(wp, "WlanHostInfo.asp");
    }
    nvram_commit(RT2860_NVRAM);
    free(newWlanMac);
	wlanMac = nvram_bufget(RT2860_NVRAM,str);
	//doSystem("iwpriv ra0  set ACLClearAll=1");
	doSystem("iwpriv ra0 set \"ACLAddEntry=%s\" ",wlanMac);
	if(!strcmp(filterrule,"1"))
		doSystem("iwpriv ra0 set AccessPolicy=%s",filterrule);//shengxiao
	#if 0
	doSystem("ralink_init make_wireless_config rt2860");
	mbssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));
		doSystem("ralink_init make_wireless_config rt2860");
		for(i=0; i<mbssid_num; i++){
			doSystem("ifconfig ra%d down", i);
		}
		for(i=0; i<mbssid_num; i++){
			doSystem("ifconfig ra%d up", i);
		}
	#endif
}

static void formConfigWlanMacFilterDelAll(webs_t wp, char_t *path, char_t *query)
{
    int mbssid=0;
	char str[32];
    a_assert(wp);
    sprintf(str, "AccessControlList%d", mbssid);
    nvram_bufset(RT2860_NVRAM, str, "");
    currentMacFilterCounter = 0;
    websRedirect(wp, "WlanMacFilter.asp");
    nvram_commit(RT2860_NVRAM);
    doSystem("iwpriv ra0  set ACLClearAll=1");
	#if 0
	doSystem("ralink_init make_wireless_config rt2860");
	mbssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));
		doSystem("ralink_init make_wireless_config rt2860");
		for(i=0; i<mbssid_num; i++){
			doSystem("ifconfig ra%d down", i);
		}
		for(i=0; i<mbssid_num; i++){
			doSystem("ifconfig ra%d up", i);
		}
	#endif
}

static void formWlanMacFilterGlobalConfig(webs_t wp, char_t *path, char_t *query)
{
    char_t *apselect, *macfiteren,*filterRule;
	char str[32];
	int mbssid=0;
	macfiteren=websGetVar(wp, "MacFilterEnable", T(""));
	filterRule=websGetVar(wp, "filterRule", T(""));
	if (strncmp(macfiteren, "on", 3)) {
		apselect=T("0");
	}
	else{
		apselect=filterRule;
	}

	sprintf(str, "AccessPolicy%d", mbssid);
	nvram_bufset(RT2860_NVRAM, str, apselect);
	nvram_bufset(RT2860_NVRAM,"AccessPolicyRule", filterRule);
      nvram_commit(RT2860_NVRAM);
	doSystem("iwpriv ra0 set AccessPolicy=%s",apselect);//shengxiao
	  // 生效
#if 0
	  mbssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));
		doSystem("ralink_init make_wireless_config rt2860");
		for(i=0; i<mbssid_num; i++){
			doSystem("ifconfig ra%d down", i);
		}
		for(i=0; i<mbssid_num; i++){
			doSystem("ifconfig ra%d up", i);
		}
#endif
		//WPSRestart();//?cxhu
//out:
	websRedirect(wp,"WlanMacFilter.asp");
	
}
void conf8021x(int nvram, webs_t wp, int mbssid)
{
	char_t *RadiusServerIP, *RadiusServerPort, *RadiusServerSecret;//, *RadiusServerIdleTimeout;

	LFW(RadiusServerIP, radiusIP);
	LFW(RadiusServerPort, radiusPort);
	LFW(RadiusServerSecret, radiusPsswd);
	// LFW(RadiusServerIdleTimeout, RadiusServerIdleTimeout);
	//if(!gstrlen(RadiusServerSessionTimeout))
		//RadiusServerSessionTimeout = "0";

	STFs(nvram, mbssid, "RADIUS_Server", RadiusServerIP);
	STFs(nvram, mbssid, "RADIUS_Port", RadiusServerPort);
	STFs(nvram, mbssid, "RADIUS_Key", RadiusServerSecret);
	//STFs(nvram, mbssid, "session_timeout_interval", RadiusServerSessionTimeout);

	updateFlash8021x(nvram);
	//restart8021XDaemon(nvram);
}
void confWEP(int nvram, webs_t wp, int mbssid)
{
	
	char_t *DefaultKeyID, *Key1Type, *Key1Str, *Key2Type, *Key2Str, *Key3Type, *Key3Str, *Key4Type, *Key4Str;
	char_t * keyFormat,*keylen1,*keylen2,*keylen3,*keylen4;
	//used key
	LFW(DefaultKeyID, keynum);
	//key
	LFW(Key1Str, wepkey1);
	LFW(Key2Str, wepkey2);
	LFW(Key3Str, wepkey3);
	LFW(Key4Str, wepkey4);
	LFW(keyFormat, keyFormat);
	//keylen
	LFW(keylen1, keyLength1);
	LFW(keylen2, keyLength2);
	LFW(keylen3, keyLength3);
	LFW(keylen4, keyLength4);
	Key1Type=Key2Type=Key3Type=Key4Type=keyFormat;
	/*
	LFW(Key1Type, WEP1Select);
	LFW(Key2Type, WEP2Select);
	LFW(Key3Type, WEP3Select);
	LFW(Key4Type, WEP4Select);
	*/
	STF(nvram, mbssid, DefaultKeyID);
	STF(nvram, mbssid, Key1Type);
	STF(nvram, mbssid, Key2Type);
	STF(nvram, mbssid, Key3Type);
	STF(nvram, mbssid, Key4Type);
	//set the keylen
	nvram_bufset(nvram, "Key1Len", keylen1);
	nvram_bufset(nvram, "Key2Len", keylen2);
	nvram_bufset(nvram, "Key3Len", keylen3);
	nvram_bufset(nvram, "Key4Len", keylen4);
	
	
	if ((RT2860_NVRAM == nvram) || (RTINIC_NVRAM == nvram)){
		nvram_bufset(nvram, racat("Key1Str", mbssid+1), Key1Str);
		nvram_bufset(nvram, racat("Key2Str", mbssid+1), Key2Str);
		nvram_bufset(nvram, racat("Key3Str", mbssid+1), Key3Str);
		nvram_bufset(nvram, racat("Key4Str", mbssid+1), Key4Str);
	} else if (RT2561_NVRAM == nvram) {
		// nvram_bufset(nvram, "Key1Str", Key1Str);
		// nvram_bufset(nvram, "Key2Str", Key2Str);
		// nvram_bufset(nvram, "Key3Str", Key3Str);
		// nvram_bufset(nvram, "Key4Str", Key4Str);
		STF(nvram, mbssid, Key1Str);
		STF(nvram, mbssid, Key2Str);
		STF(nvram, mbssid, Key3Str);
		STF(nvram, mbssid, Key4Str);
	}
}
void confWPAGeneral(int nvram, webs_t wp, int mbssid)
{
	char *cipher_str;

	LFW(cipher_str, cipher);
//	LFW(key_renewal_interval, keyUpdateCyc);

	switch(cipher_str[0]){
	case '0':
		STFs(nvram, mbssid, "EncrypType", "TKIP");
		break;
	case '1':
		STFs(nvram, mbssid, "EncrypType", "AES");
		break;
	case '2':
		STFs(nvram, mbssid, "EncrypType", "TKIPAES");
		return;							// there is no tkip-aes mixed mode in WPA-PSK.
	}

//	STFs(nvram, mbssid, "DefaultKeyID", "2");	// DefaultKeyID is 2
//	STFs(nvram, mbssid, "RekeyInterval", websGetVar(wp, T("keyUpdateCyc"), T("0")));
	STFs(nvram, mbssid, "RekeyMethod", "TIME");		
	STFs(nvram, mbssid, "IEEE8021X", "0");
}

inline void clearRadiusSetting(int nvram, int mbssid)
{
	char *RADIUS_Server, *RADIUS_Port, *RADIUS_Key;
	//char *session_timeout_interval = NULL;

	RADIUS_Server = nvram_bufget(nvram, "RADIUS_Server");
	RADIUS_Port = nvram_bufget(nvram, "RADIUS_Port");
	RADIUS_Key = nvram_bufget(nvram, "RADIUS_Key");
	//session_timeout_interval = nvram_bufget(nvram, "session_timeout_interval");

	nvram_bufset(nvram, "RADIUS_Server", setNthValue(mbssid, RADIUS_Server, ""));
	nvram_bufset(nvram, "RADIUS_Port", setNthValue(mbssid, RADIUS_Port, "1812"));
	nvram_bufset(nvram, "RADIUS_Key", setNthValue(mbssid, RADIUS_Key, ""));
	//nvram_bufset(nvram, "session_timeout_interval", setNthValue(mbssid, session_timeout_interval, ""));
            
	return;
}

//add by cxhu for wlansecurity
void Security(int nvram, webs_t wp, char_t *path, char_t *query)
{
	int mbssid;
	char_t *security_mode;

	/*LFW(SSID, ssidIndex);
	if(!gstrlen(SSID))
		return;
*/
	mbssid =0;//because have only one ssid

	default_shown_mbssid[nvram] = mbssid;

	LFW(security_mode, security_mode);

#if 0
#ifndef CONFIG_RALINK_RT2880
	// RT2880: GPIO13 is belong to WPS PBC indicator.

	if (!strcmp(security_mode, "Disable") || !strcmp(security_mode, "OPEN"))
		ledAlways(13, LED_OFF); //turn off security LED (gpio 13)
	else
		ledAlways(13, LED_ON); //turn on security LED (gpio 13)

#endif
#endif

	//clear Radius settings
	clearRadiusSetting(nvram, mbssid);
  
	if( !strcmp(security_mode, "Disable")){			// !------------------       Disable Mode --------------
		STFs(nvram, mbssid, "AuthMode", "OPEN");
		STFs(nvram, mbssid, "EncrypType", "NONE");
		STFs(nvram, mbssid, "IEEE8021X", "0");
	}else if( !strcmp(security_mode, "OPEN")){		// !------------------       Open Mode ----------------
		confWEP(nvram, wp, mbssid);
		STFs(nvram, mbssid, "AuthMode", security_mode);
		STFs(nvram, mbssid, "EncrypType", "WEP");
		STFs(nvram, mbssid, "IEEE8021X", "0");
	}else if( !strcmp(security_mode, "SHARED")){	// !------------------       Shared Mode ----------------
		//char *security_shared_mode;
		confWEP(nvram, wp, mbssid);

		//LFW(security_shared_mode, security_shared_mode);

		STFs(nvram, mbssid, "AuthMode", security_mode);
		//if(!strcmp(security_shared_mode, "None"))
			//STFs(nvram, mbssid, "EncrypType", "NONE");
		//else
			STFs(nvram, mbssid, "EncrypType", "WEP");

		STFs(nvram, mbssid, "IEEE8021X", "0");
	}else if( !strcmp(security_mode, "WEPAUTO")){ // !------------------       WEP Auto Mode ----------------
		confWEP(nvram, wp, mbssid);
		STFs(nvram, mbssid, "AuthMode", security_mode);
		STFs(nvram, mbssid, "EncrypType", "WEP");
		STFs(nvram, mbssid, "IEEE8021X", "0");
	}else if(	!strcmp(security_mode, "WPA") || 
				!strcmp(security_mode, "WPA1WPA2")||!strcmp(security_mode, "WPA2") ){	// !------------------		WPA Enterprise Mode ----------------
		conf8021x(nvram, wp, mbssid);	//set radius para						// !------------------		WPA1WPA2 mixed mode
		confWPAGeneral(nvram, wp, mbssid);//cipher and upcyc
	    STFs(nvram, mbssid, "RekeyInterval", websGetVar(wp, T("keyUpdateCyc"), T("0")));

		STFs(nvram, mbssid, "AuthMode", security_mode);
		STFs(nvram, mbssid, "IEEE8021X", "0");
	}
	
	else if( !strcmp(security_mode, "WPAPSK")||!strcmp(security_mode, "WPA2PSK") ||	
				!strcmp(security_mode, "WPAPSKWPA2PSK") ){
		char *pass_phrase_str;

		confWPAGeneral(nvram, wp, mbssid);
	    STFs(nvram, mbssid, "RekeyInterval", websGetVar(wp, T("pskkeyUpdateCyc"), T("0")));
		LFW(pass_phrase_str, pskPsswd);
		
		STFs(nvram, mbssid, "AuthMode", security_mode);
		STFs(nvram, mbssid, "IEEE8021X", "0");

		if ((RT2860_NVRAM == nvram) || (RTINIC_NVRAM == nvram)){
			nvram_bufset(nvram, racat("WPAPSK", mbssid+1), pass_phrase_str);
		} else if (RT2561_NVRAM == nvram) {
			// nvram_bufset(nvram, "WPAPSK", pass_phrase_str);
			STFs(nvram, mbssid, "WPAPSK", pass_phrase_str);
		}
	}
	else{
		return;
	}
	nvram_commit(nvram);

	sendMsgForSpeed(WLAN_SEC_INIT, 0, 0, NULL, 0);
#if 0
	/*
	 *  TODO: move to inic.c
	 */
	mbssid_num = atoi(nvram_bufget(nvram, "BssidNum"));
	apworkmode=nvram_bufget(nvram, "apworkmode");
	int j;
	if(nvram == RT2860_NVRAM){
		doSystem("ralink_init make_wireless_config rt2860");
		for(i=0; i<mbssid_num; i++)
			doSystem("ifconfig ra%d down", i);
		if(!strcmp(apworkmode,"2")||!strcmp(apworkmode,"3")||!strcmp(apworkmode,"4")){
			for(j=0;j<4;j++)//add by cxhu for the secmode  when apmode is apcli or wds
				doSystem("ifconfig wds%d down",j);
			for(j=0;j<4;j++)
				doSystem("ifconfig wds%d up",j);
		}
		if(!strcmp(apworkmode,"5")){
			doSystem("ppp-off");
			doSystem("ifconfig apcli0 down");
			doSystem("ifconfig apcli0 up");
			//initWan();
		}
		for(i=0; i<mbssid_num; i++)
			doSystem("ifconfig ra%d up", i);
		if(!strcmp(apworkmode,"5"))
			initWan();
		
	     
		//WPSRestart();//?cxhu
	}else if(nvram == RTINIC_NVRAM){
		doSystem("ralink_init make_wireless_config inic");
		for(i=0; i<mbssid_num; i++){
			doSystem("ifconfig rai%d down", i);
			doSystem("ifconfig rai%d up", i);
		}
		 WPSRestart();
	} else if(nvram == RT2561_NVRAM) {
		doSystem("ralink_init make_wireless_config rt2561");
		for(i=0; i<mbssid_num; i++){
			doSystem("ifconfig raL%d down", i);
			doSystem("ifconfig raL%d up", i);
		}
		 WPSRestart();
	}else
		printf("*** Unknown interface.\n");
#endif

	//restart8021XDaemon(nvram);
//out:
	
	websRedirect(wp,"WlanSecurity.asp");
	

}
static void APSecurity(webs_t wp, char_t *path, char_t *query)
{
	Security(RT2860_NVRAM, wp, path, query);
}
static int getWirelessSecurityConfig(int eid, webs_t wp, int argc, char_t **argv)
{
    int mssid=0;
    websWrite(wp,"AuthModes=\"%s\";", nvram_bufget(RT2860_NVRAM, "AuthMode"));

   // websWrite(wp,"var wepAuthModes=\"%s\";",nvram_bufget(RT2860_NVRAM,"AuthMode") );
    websWrite(wp,"var keyFormats=\"%s\";",nvram_bufget(RT2860_NVRAM, "Key1Type"));
    websWrite(wp,"var keyLength1s=\"%s\";",nvram_bufget(RT2860_NVRAM,"Key1Len" ));
    websWrite(wp,"var keyLength2s=\"%s\";",nvram_bufget(RT2860_NVRAM, "Key2Len"));
    websWrite(wp,"var keyLength3s=\"%s\";",nvram_bufget(RT2860_NVRAM,  "Key3Len"));
    websWrite(wp,"var keyLength4s=\"%s\";",nvram_bufget(RT2860_NVRAM,  "Key4Len"));
    websWrite(wp,"var wepkey1s=\"%s\";",nvram_bufget(RT2860_NVRAM,racat("Key1Str", mssid+1) ));
    websWrite(wp,"var wepkey2s=\"%s\";",nvram_bufget(RT2860_NVRAM,racat("Key2Str", mssid+1)  ));
    websWrite(wp,"var wepkey3s=\"%s\";",nvram_bufget(RT2860_NVRAM, racat("Key3Str", mssid+1) ));
    websWrite(wp,"var wepkey4s=\"%s\";",nvram_bufget(RT2860_NVRAM, racat("Key4Str", mssid+1) ));
    websWrite(wp,"var keynums=\"%s\";",nvram_bufget(RT2860_NVRAM,"DefaultKeyID" ));

    //websWrite(wp,"var wpaAuthModes=\"%s\";",nvram_bufget(RT2860_NVRAM, ));
    websWrite(wp,"var wpaCiphers=\"%s\";",nvram_bufget(RT2860_NVRAM,"EncrypType" ));
    websWrite(wp,"var radiusIPs=\"%s\";",nvram_bufget(RT2860_NVRAM,"RADIUS_Server" ));
    websWrite(wp,"var radiusPorts=\"%s\";",nvram_bufget(RT2860_NVRAM, "RADIUS_Port"));
    websWrite(wp,"var radiusPsswds=\"%s\";",nvram_bufget(RT2860_NVRAM,"RADIUS_Key"));
    websWrite(wp,"var keyUpdateCycs=\"%s\";",nvram_bufget(RT2860_NVRAM, "RekeyInterval"));

   // websWrite(wp,"var wpapskAuthModes=\"%s\";",nvram_bufget(RT2860_NVRAM, ));
    websWrite(wp,"var wpapskCiphers=\"%s\";",nvram_bufget(RT2860_NVRAM, "EncrypType"));
    websWrite(wp,"var pskPsswds=\"%s\";",nvram_bufget(RT2860_NVRAM, racat("WPAPSK", mssid+1)));
    websWrite(wp,"var pskkeyUpdateCycs=\"%s\";",nvram_bufget(RT2860_NVRAM,"RekeyInterval" ));
	getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

void DeleteAccessPolicyList(int nvram, webs_t wp, char_t *path, char_t *query)
{
	int mbssid, aplist_num;
	char str[32], apl[64*20], *tmp;
	sscanf(query, "%d,%d", &mbssid, &aplist_num);

	sprintf(str, "AccessControlList%d", mbssid);
	if(!(tmp = nvram_bufget(nvram, str)))
		return;
	strcpy(apl, tmp);

	deleteNthValueMulti(&aplist_num, 1, apl, ';');

	nvram_bufset(nvram, str, apl);

	default_shown_mbssid[nvram] = mbssid;

	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	websWrite(wp, T("ok done"));
	websDone(wp, 200);
	
}

static void APDeleteAccessPolicyList(webs_t wp, char_t *path, char_t *query)
{
	DeleteAccessPolicyList(RT2860_NVRAM, wp, path, query);
}

#ifdef CONFIG_RT2860V2_AP_MESH
/* goform/wirelessMesh */
static void wirelessMesh(webs_t wp, char_t *path, char_t *query)
{
	char_t	*meshenable, *mid, *hostname;
	char_t	*autolink;
	char_t	*mode, *encrypt_type, *wepkey, *wep_select, *wpakey;
	char defaultkey[2];

	//fetch from web input
	meshenable = websGetVar(wp, T("MeshEnable"), T("")); 
	mid = websGetVar(wp, T("MeshID"), T("")); 
	hostname = websGetVar(wp, T("HostName"), T("")); 
	autolink = websGetVar(wp, T("AutoLinkEnable"), T(""));
	mode = websGetVar(wp, T("security_mode"), T(""));
	strcpy(defaultkey, "");
	if (0 == strcmp(mode, "OPEN"))
	{
		encrypt_type = websGetVar(wp, T("open_encrypt_type"), T(""));
		if (0 == strcmp(encrypt_type, "WEP"))
			strcpy(defaultkey, "1");
	}
	else if (0 == strcmp(mode, "WPANONE"))
	{
		encrypt_type = websGetVar(wp, T("wpa_cipher"), T(""));
	}
	wepkey = websGetVar(wp, T("wep_key"), T(""));
	wep_select = websGetVar(wp, T("wep_select"), T(""));
	wpakey = websGetVar(wp, T("passphrase"), T(""));

	// store to flash
	nvram_bufset(RT2860_NVRAM, "MeshEnabled", meshenable);
	nvram_bufset(RT2860_NVRAM, "MeshId", mid);
	nvram_bufset(RT2860_NVRAM, "MeshHostName", hostname);
	nvram_bufset(RT2860_NVRAM, "MeshAutoLink", autolink);
	nvram_bufset(RT2860_NVRAM, "MeshAuthMode", mode);
	nvram_bufset(RT2860_NVRAM, "MeshEncrypType", encrypt_type);
	nvram_bufset(RT2860_NVRAM, "MeshDefaultkey", defaultkey);
	nvram_bufset(RT2860_NVRAM, "MeshWEPKEY", wepkey);
	nvram_bufset(RT2860_NVRAM, "MeshWEPKEYType", wep_select);
	nvram_bufset(RT2860_NVRAM, "MeshWPAKEY", wpakey);

	nvram_commit(RT2860_NVRAM);
	
	// restart network
	doSystem("internet.sh");

	// debug print
	websHeader(wp);
	websWrite(wp, T("MeshEnable: %s<br>\n"), meshenable);
	websWrite(wp, T("MeshID: %s<br>\n"), mid);
	websWrite(wp, T("HostName: %s<br>\n"), hostname);
	websWrite(wp, T("AutoLinkEnable: %s<br>\n"), autolink);
	websWrite(wp, T("security_mode: %s<br>\n"), mode);
	websWrite(wp, T("encrypt_type: %s<br>\n"), encrypt_type);
	websWrite(wp, T("defaultkey: %s<br>\n"), defaultkey);
	websWrite(wp, T("wep_key: %s<br>\n"), wepkey);
	websWrite(wp, T("wep_select: %s<br>\n"), wep_select);
	websWrite(wp, T("passphrase: %s<br>\n"), wpakey);
	websFooter(wp);
	websDone(wp, 200);
}

/* goform/meshManualLink */
static void meshManualLink(webs_t wp, char_t *path, char_t *query)
{
	char_t *action, *mpmac;

	// fetch from web input
	action = websGetVar(wp, T("link_action"), T(""));
	mpmac = websGetVar(wp, T("mpmac"), T(""));
	
	// link action
	if (0 == strcmp(action, "add"))
		doSystem("iwpriv mesh0 set MeshAddLink=%s", mpmac);
	if (0 == strcmp(action, "del"))
		doSystem("iwpriv mesh0 set MeshDelLink=%s", mpmac);
	sleep(1);

	websRedirect(wp, "wireless/mesh.asp");
}

typedef struct _MESH_NEIGHBOR_ENTRY_INFO {
	char			Rssi;
	unsigned char	HostName[32 + 1];
	unsigned char	MacAddr[6];
	unsigned char	MeshId[32 + 1];
	unsigned char	Channel;
	unsigned char	Status;
	unsigned char	MeshEncrypType;
} MESH_NEIGHBOR_ENTRY_INFO;

typedef struct _MESH_NEIGHBOR_INFO {
	MESH_NEIGHBOR_ENTRY_INFO	Entry[64];
	unsigned char				num;
} MESH_NEIGHBOR_INFO;

/* goform/ShowMeshState */
static int ShowMeshState(int eid, webs_t wp, int argc, char_t **argv)
{
	int socket_id, ret, i;
	struct iwreq wrq;
	MESH_NEIGHBOR_INFO *neighbor;

	if (NULL == (neighbor = (MESH_NEIGHBOR_INFO *) malloc(sizeof(MESH_NEIGHBOR_INFO))))
	{
		fprintf(stderr, "can't allocat memory in MESH_NEIGHBOR_INFO !\n");
		return -1;
	}
	memset(neighbor, 0, sizeof(MESH_NEIGHBOR_INFO));

	if ((socket_id = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		fprintf(stderr, "ShowMeshState: open socket error !\n");
		websError(wp, 500, "ioctl sockey failed !");
		return -1;
	}

	strcpy(wrq.ifr_name, "mesh0");
	wrq.u.data.length = sizeof(MESH_NEIGHBOR_INFO);
	wrq.u.data.pointer = (caddr_t) neighbor;
	wrq.u.data.flags = OID_802_11_MESH_LIST;
	if ((ret = ioctl(socket_id, RT_PRIV_IOCTL, &wrq)) < 0)
	{
		fprintf(stderr, "ShowMeshState: ioctl -> OID_802_11_MESH_LIST error !\n");
		websError(wp, 500, "ioctl -> OID_802_11_MESH_LIST failed!");
		close(socket_id);
		return -1;
	}
	
	for(i=0;i<neighbor->num;i++)
	{
		websWrite(wp, T("<tr align=\"center\">"));
		if (1 == neighbor->Entry[i].Status)
			websWrite(wp, T("<td>%s</td>"), "<img src=\"/graphics/handshake.gif\">");
		else
			websWrite(wp, T("<td>%s</td>"), "<br />");
		websWrite(wp, T("<td>%02X:%02X:%02X:%02X:%02X:%02X</td>"), 
					  neighbor->Entry[i].MacAddr[0],
					  neighbor->Entry[i].MacAddr[1],
					  neighbor->Entry[i].MacAddr[2],
					  neighbor->Entry[i].MacAddr[3],
					  neighbor->Entry[i].MacAddr[4],
					  neighbor->Entry[i].MacAddr[5]);
		websWrite(wp, T("<td>%d</td>"), neighbor->Entry[i].Rssi);
		websWrite(wp, T("<td>%s</td>"), neighbor->Entry[i].MeshId);
		websWrite(wp, T("<td>%s</td>"), neighbor->Entry[i].HostName);
		websWrite(wp, T("<td>%d</td>"), neighbor->Entry[i].Channel);
		if (neighbor->Entry[i].MeshEncrypType == 1)
			websWrite(wp, T("<td>%s</td>"), "OPEN-WEP");
		else if (neighbor->Entry[i].MeshEncrypType == 2)
			websWrite(wp, T("<td>%s</td>"), "WPANONE-TKIP");
		else if (neighbor->Entry[i].MeshEncrypType == 3)
			websWrite(wp, T("<td>%s</td>"), "WPANONE-AES");
		else
			websWrite(wp, T("<td>%s</td>"), "OPEN-NONE");
		websWrite(wp, T("</tr>"));
	}
	close(socket_id);

	return 0;
}
#endif

static int isWPSConfiguredASP(int eid, webs_t wp, int argc, char_t **argv)
{
	if(g_wsc_configured){
		websWrite(wp, T("1"));
	}else
		websWrite(wp, T("0"));	
	return 0;
}

