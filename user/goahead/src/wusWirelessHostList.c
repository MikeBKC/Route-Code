#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include "../../autoconf.h"
#ifdef CONFIG_DEFAULTS_KERNEL_2_6_21
    #include  <linux/types.h>
    #include  <linux/socket.h>
    #include  <linux/if.h>
#endif
#include <linux/wireless.h>

#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "management.h"
#include "wireless.h"
#include <user_oid.h>
#include "linux/autoconf.h"
#include "uttfunction.h"
#include "mib.h"
#include "profacce.h"
#include "translate.h"

#if(WIRELESS == FYES)

#define  DEBUG_HOSTLIST 0
#if DEBUG_HOSTLIST
#define HOSTLIST_DEBUG(argx) printf argx
#else
#define HOSTLIST_DEBUG(argx) 
#endif


#if (UTT_SMART_UI == FYES)
int isWilessClient(char *mac);
#endif
extern char* converMac6To12Byte(MacAddr macAddr);
extern int converMac12To6Byte(char *macStr, MacAddr* macAddr);
char delColon(char str[20], char ch);

static void formConfigAddAllMAC(webs_t wp, char * path, char * query);
static int getStationFromIoctl( webs_t wp, char *wlanMac,int printfOrAddMac);
static int  getWlanStationList(int eid, webs_t wp, int argc, char_t **argv);

#ifndef FEATURE_AP 
#if 0 /*defined in config/user_oid.h*/
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
#endif
typedef struct _RT_802_11_MAC_ENTRY {
#if  defined(CONFIG_RALINK_RT5350) || defined(CONFIG_RALINK_MT7620) || defined(CONFIG_RALINK_MT7621) || defined(CONFIG_RALINK_MT7628)
        unsigned char            ApIdx;
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
#if defined(CONFIG_BOARD_MTK7621_F) || defined(CONFIG_RALINK_MT7612E) 
	RT_802_11_MAC_ENTRY      Entry[116]; //MAX_LEN_OF_MAC_TABLE = 32
#else
	RT_802_11_MAC_ENTRY      Entry[64]; //MAX_LEN_OF_MAC_TABLE = 32
#endif
} RT_802_11_MAC_TABLE;
#endif
int s;
/***********************************************************************
 * 函 数 名：   formConfigAddAllMAC
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void formConfigAddAllMAC(webs_t wp, char * path, char * query) 
{
HOSTLIST_DEBUG(("\n---> formConfigAddAllMAC() begin <---\n"));
    a_assert(wp);
    int min, max, index;
    char wlanMac[1024] = {0};
    char *tmpmac; 
    const char *profName = NULL;
    MibProfileType mibType = MIB_PROF_WIRELESS_MAC_FILTER;
    WirelessMacFilterProfile *prof = NULL;
    
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
    getStationFromIoctl(wp, wlanMac, 0);
    websRedirect(wp, "WlanHostInfo.asp");
HOSTLIST_DEBUG(("\n---> formConfigAddAllMAC() end <---\n"));
}

#if (WIRELESS_5G == FYES)
static void formConfigAddAllMAC_5g(webs_t wp, char * path, char * query)
{
    a_assert(wp);
    int min, max, index;
    char wlanMac[1024] = {0};
    char *tmpmac; 
    const char *profName = NULL;
    MibProfileType mibType = MIB_PROF_WIRELESS_5G_MAC_FILTER;
    Wireless5gMacFilterProfile *prof = NULL;
    
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
    getStationFromIoctl_5g(wp, wlanMac, 0);
    websRedirect(wp, "WlanHostInfo_5g.asp");
}
#endif

#if (UTT_SMART_UI == FYES)
/********************************************************************
 * 函数： isWilessClient
 * 功能： 判断给定mac是否是无线用户的mac
 * 创建： 2013-10-24
 * 参数： 要判断的mac地址
 * 返回： 1->是；0->不是
 * 输出： 0,1
 * 作者： 赵冲 
 ********************************************************************/ 
int isWilessClient(char *mac)
{
    int isWirelessClient = 0;
    int i = 0, flag = 0;
    char buf[32];
    struct iwreq iwr;
    RT_802_11_MAC_TABLE table = {0};

    s = socket(AF_INET, SOCK_DGRAM, 0);
    strncpy(iwr.ifr_name, "ra0", IFNAMSIZ);
    iwr.u.data.pointer = (caddr_t) &table;
    if (s >= 0)
    {
	if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE, &iwr) < 0)
	{
	    flag = 1;
	}
	close(s);
    } else {
	flag = 1;
    }

    if (flag != 1) {
	for (i = 0; i < table.Num; i++) {
	    sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", table.Entry[i].Addr[0], table.Entry[i].Addr[1],table.Entry[i].Addr[2], table.Entry[i].Addr[3],table.Entry[i].Addr[4], table.Entry[i].Addr[5]);
	    if (strcasecmp(mac,buf) == 0) {
		isWirelessClient = 1;
		break;
	    }
	}
    }

    return isWirelessClient;
}

/*查找无流量无线用户*/
#if (SHOW_ALL_WIRE_MAC == FYES)
#define ARP_VALID_MIN_LEN 19 /*合法arp条目的最小长度*/
extern int searchNode(struct macTreeNode *node, unsigned char *macaddr);
extern int convertDateTimeFromSecToStr(unsigned long sec, char *timeStr, int strLen);
extern bool getIpMacFromSysArpEntry(char*arpEntry, char*ipStr ,char* macStr, char*ifName);


/*在arp表中根据mac查找ip*/
void getIpByMacFromSysArpTb(char *mac, char *ipStr)
{
    FILE *fp;
    char buf[96], macStr[32];

    memset(macStr, 0, sizeof(macStr));
    
    fp=fopen("/proc/net/arp","r");/*读取系统arp表*/

    while(!feof(fp))
    {
	memset(buf, 0, sizeof(buf));
	fgets(buf, sizeof(buf), fp);/*读取一条arp条目*/
	if(strlen(buf) >= ARP_VALID_MIN_LEN)/*合法arp条目*/
	{
	    /*从arp条目中解析出来ip以及mac地址*/
	    if(getIpMacFromSysArpEntry(buf, ipStr, macStr, getLanIfName())!=0)
	    {
		if (strcasecmp(mac, macStr) == 0) {
		    fclose(fp);/*关闭文件流*/
		    return;
		}
	    }
	}
    }
    fclose(fp);/*关闭文件流*/
    
    strcpy(ipStr, "");
    return;
}

void getUnusedWireMac(webs_t wp, int *indexp, struct macTreeNode *rootNode, int is5g)
{
    int i = 0;
    struct iwreq iwr;
    RT_802_11_MAC_TABLE table = {0};
    char timeStr[50];
    char macStr[32];
    char ipStr[32];

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (is5g){
	strncpy(iwr.ifr_name, "rai0", IFNAMSIZ);
    }else{
	strncpy(iwr.ifr_name, "ra0", IFNAMSIZ);
    }
    iwr.u.data.pointer = (caddr_t) &table;

    memset(ipStr, 0, sizeof(ipStr));

    if (s >= 0)
    {
	if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE, &iwr) >= 0)
	{
	    for (i = 0; i < table.Num; i++) {
		if (!searchNode(rootNode, table.Entry[i].Addr))
		{
		    memset(macStr, 0, sizeof(macStr));
		    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", table.Entry[i].Addr[0], table.Entry[i].Addr[1], table.Entry[i].Addr[2], table.Entry[i].Addr[3], table.Entry[i].Addr[4], table.Entry[i].Addr[5]);
		    websWrite(wp, "Macs[%d]=\"%s\";\n", *indexp, macStr);
		    websWrite(wp, "inbits[%d]=\"0\";\n", *indexp);
		    websWrite(wp, "outbits[%d]=\"0\";\n", *indexp);
		    memset(timeStr, 0, sizeof(timeStr));
		    convertDateTimeFromSecToStr(0U, timeStr, sizeof(timeStr));
		    websWrite(wp, "startTimes[%d]=\"%s\";\n", *indexp, timeStr);
		    websWrite(wp, "rxBytes[%d]=\"0\";\n", *indexp);
		    websWrite(wp, "txBytes[%d]=\"0\";\n", *indexp);
		    websWrite(wp, "connectType[%d]=\"wireless\";\n", *indexp);
		    getIpByMacFromSysArpTb(macStr, ipStr);
		    websWrite(wp, "IPs[%d]=\"%s\";\n", *indexp, ipStr);
		    (*indexp)++;
		}
	    }
	}
	close(s);
    }
}
/*得到无流量用户的数量*/
void getUnusedWireNum(int *indexp, struct macTreeNode *rootNode, int is5g)
{
    int i = 0;
    struct iwreq iwr;
    RT_802_11_MAC_TABLE table = {0};

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (is5g){
	strncpy(iwr.ifr_name, "rai0", IFNAMSIZ);
    }else{
	strncpy(iwr.ifr_name, "ra0", IFNAMSIZ);
    }
    iwr.u.data.pointer = (caddr_t) &table;

    if (s >= 0)
    {
	if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE, &iwr) >= 0)
	{
	    for (i = 0; i < table.Num; i++) {
		if (!searchNode(rootNode, table.Entry[i].Addr))
		{
		    (*indexp)++;
		}
	    }
	}
	close(s);
    }
}
#endif
#endif
/***********************************************************************
 * 函 数 名：   getStationFromIoctl
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static int getStationFromIoctl( webs_t wp, char *wlanMac,int printfOrAddMac)
{
HOSTLIST_DEBUG(("\n---> getStationFromIoctl() begin <---\n"));
	MacAddr tmp;
	int i,totalrece=0, iscommit=0;
	int min, max, index;
	struct iwreq iwr;
	int filterrule = 0;
	char *tmpmac;
	const char *profName = NULL;
	const char *ErrMsg = NULL;
	char oldMacList2[1024] = {0};
	RT_802_11_MAC_TABLE table = {0};
	WirelessMacFilterProfile *prof = NULL;
	WirelessMacFilterGlobalProfile *profGlobal = NULL;
	MibProfileType mibType = MIB_PROF_WIRELESS_MAC_FILTER;
	MibProfileType mibTypeGlobal = MIB_PROF_WIRELESS_MAC_FILTER_GLOBAL;
	ProfChangeType profAction = PROF_CHANGE_ADD;
	struProfAlloc *profhead = NULL;

	s = socket(AF_INET, SOCK_DGRAM, 0);
	strncpy(iwr.ifr_name, "ra0", IFNAMSIZ);
	iwr.u.data.pointer = (caddr_t) &table;

	if (s < 0)
	{
		return -1;
	}

	if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE, &iwr) < 0)
	{
	    close(s);
	    if(printfOrAddMac)
		websWrite(wp,"var totalrecs=%d;", 0);
	    return -1;
	}
	for (i = 0; i < table.Num; i++)
	{
		if(printfOrAddMac)
		{
			char *HostMacs = T("HostMacs[%d] = \"%s\"; ");
        		char *Allows = T("Allows[%d] = \"%s\"; ");
			char *BWs = T("BWs[%d] = \"%s\"; ");
			char buf[32];
			sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", table.Entry[i].Addr[0], table.Entry[i].Addr[1],table.Entry[i].Addr[2], table.Entry[i].Addr[3],table.Entry[i].Addr[4], table.Entry[i].Addr[5]);
			websWrite(wp,HostMacs, totalrece, buf);
			if(strstri(wlanMac, buf))
			{
           			websWrite(wp,Allows, totalrece, T("yes"));
			}
        		else
			{
            			websWrite(wp,Allows, totalrece, T("no"));
			}
			memset(buf, 0,32);
			if(table.Entry[i].TxRate.field.BW == 0)
			{
				sprintf(buf, "20M");
			}
			else
			{
				sprintf(buf, "40M");
			}
			websWrite(wp,BWs, totalrece, buf);
		 }    
		else
		{	
    			char mac[20];
    			//char *mac = NULL;
   			int macStrLen=0;
			char oldMacList[1024] = {0};
			ProfInstLowHigh(mibType, &max, &min);
			for (index = min; index < max; index++)
			{
			    prof = (WirelessMacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
			    profName = ProfGetInstNameByIndex(mibType, index);
			    if (0 != strcmp(profName, ""))
			    {
				tmpmac = converMac6To12Byte(prof->mac);
				Conversion2StdMac(tmpmac);
				strcat(oldMacList, tmpmac);
				strcat(oldMacList, ";");
			    }
			    else
			    {
				continue;
			    }
			}
			oldMacList[strlen(oldMacList)-1] = '\0';
			profGlobal = (WirelessMacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeGlobal, 0);
			if (profGlobal->filterEn == FUN_DISABLE)
			{
			    filterrule = 0;	
			}
			if (AP_MAC_ALLOW == profGlobal->action)
			{
			    filterrule = 1;	
			}
			else
			{
			    filterrule = 2;	
			}
			memset(mac, 0, 20);
    			sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",  table.Entry[i].Addr[0], table.Entry[i].Addr[1],table.Entry[i].Addr[2], table.Entry[i].Addr[3],table.Entry[i].Addr[4], table.Entry[i].Addr[5]);
			macStrLen = strlen(oldMacList);
   			if(strstri(oldMacList,mac))
			{
				continue;
			}
			delColon(mac, ':');
			index = ProfNewInst(mibType, mac, FALSE);
			if (index == PROFFAIL)
			{
				ErrMsg = getTransDataForC(C_LANG_INDEX_MAC_FILTER_LIST_MAX);
			    setTheErrorMsgOnFree(ErrMsg);
			    return 0;
			}
			ProfBackupByIndex(mibType, profAction, index, &profhead);
			prof = (WirelessMacFilterProfile *)ProfGetInstPointByName(mibType, mac);
			converMac12To6Byte(mac, &tmp);
			prof->mac = tmp;
			nvramWriteCommit();
			iscommit=1;
		}
		totalrece++;
	}
	if(printfOrAddMac)
	{
	    websWrite(wp,"var totalrecs=%d;", totalrece);
	}
	close(s);
	if(iscommit)
	{
	    ProfInstLowHigh(mibType, &max, &min);
	    for (index = min; index < max; index++)
	    {
		prof = (WirelessMacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
		profName = ProfGetInstNameByIndex(mibType, index);
		if (0 != strcmp(profName, ""))
		{
		    tmpmac = converMac6To12Byte(prof->mac);
		    Conversion2StdMac(tmpmac);
		    strcat(oldMacList2, tmpmac);
		    strcat(oldMacList2, ";");
		}
		else
		{
		    continue;
		}
	    }
	    oldMacList2[strlen(oldMacList2)-1] = '\0';
	    doSystem("iwpriv ra0 set \"ACLAddEntry=%s\" ", oldMacList2);
	    if(filterrule)
	    {
		if((filterrule == 2) && (profGlobal->filterEn == FUN_ENABLE))
		{
		    doSystem("iwpriv ra0 set AccessPolicy=%d", filterrule);
		}
		else if ((filterrule == 1) && (profGlobal->filterEn == FUN_ENABLE))
		{
		    doSystem("iwpriv ra0 set AccessPolicy=%d", 1);
		}
	    }
	    else
	    {
		    doSystem("iwpriv ra0 set AccessPolicy=%d", 0);
	    }
	}
HOSTLIST_DEBUG(("\n---> getStationFromIoctl() end <---\n"));
	return 0;
}

#if (WIRELESS_5G == FYES)
static int getStationFromIoctl_5g( webs_t wp, char *wlanMac,int printfOrAddMac)
{
	MacAddr tmp;
	int i,totalrece=0, iscommit=0;
	int min, max, index;
	struct iwreq iwr;
	int filterrule = 0;
	char *tmpmac;
	const char *profName = NULL;
	const char *ErrMsg = NULL;
	char oldMacList2[1024] = {0};
	RT_802_11_MAC_TABLE table = {0};
	Wireless5gMacFilterProfile *prof = NULL;
	WirelessMacFilterGlobalProfile *profGlobal = NULL;
	MibProfileType mibType = MIB_PROF_WIRELESS_5G_MAC_FILTER;
	MibProfileType mibTypeGlobal = MIB_PROF_WIRELESS_MAC_FILTER_GLOBAL;
	ProfChangeType profAction = PROF_CHANGE_ADD;
	struProfAlloc *profhead = NULL;
	WirelessProfile *profWl = NULL;
	MibProfileType mibTypeWl = MIB_PROF_WIRELESS;

	profWl = (WirelessProfile*)ProfGetInstPointByIndex(mibTypeWl, 1);

	s = socket(AF_INET, SOCK_DGRAM, 0);
	strncpy(iwr.ifr_name, "rai0", IFNAMSIZ);
	iwr.u.data.pointer = (caddr_t) &table;

	if (s < 0)
	{
		return -1;
	}

	if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE, &iwr) < 0)
	{
	    close(s);
	    if(printfOrAddMac)
		websWrite(wp,"var totalrecs=%d;", 0);
	    return -1;
	}
	for (i = 0; i < table.Num; i++)
	{
		if(printfOrAddMac)
		{
			char *HostMacs = T("HostMacs[%d] = \"%s\"; ");
        		char *Allows = T("Allows[%d] = \"%s\"; ");
			char *BWs = T("BWs[%d] = \"%s\"; ");
			char buf[32];
			sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", table.Entry[i].Addr[0], table.Entry[i].Addr[1],table.Entry[i].Addr[2], table.Entry[i].Addr[3],table.Entry[i].Addr[4], table.Entry[i].Addr[5]);
			websWrite(wp,HostMacs, totalrece, buf);
			if(strstri(wlanMac, buf))
			{
           			websWrite(wp,Allows, totalrece, T("yes"));
			}
        		else
			{
            			websWrite(wp,Allows, totalrece, T("no"));
			}
			memset(buf, 0,32);
			if(table.Entry[i].TxRate.field.BW == 0)
			{
				if (profWl->mBaseCf.VChanWidth == 1) {
				sprintf(buf, "80M");
				} else {
				sprintf(buf, "20M");
				}
			}
			else
			{
				sprintf(buf, "40M");
			}
			websWrite(wp,BWs, totalrece, buf);
		 }    
		else
		{	
    			char mac[20];
    			//char *mac = NULL;
   			int macStrLen=0;
			char oldMacList[1024] = {0};
			ProfInstLowHigh(mibType, &max, &min);
			for (index = min; index < max; index++)
			{
			    prof = (Wireless5gMacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
			    profName = ProfGetInstNameByIndex(mibType, index);
			    if (0 != strcmp(profName, ""))
			    {
				tmpmac = converMac6To12Byte(prof->mac);
				Conversion2StdMac(tmpmac);
				strcat(oldMacList, tmpmac);
				strcat(oldMacList, ";");
			    }
			    else
			    {
				continue;
			    }
			}
			oldMacList[strlen(oldMacList)-1] = '\0';
			profGlobal = (WirelessMacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeGlobal, 1);
			if (profGlobal->filterEn == FUN_DISABLE)
			{
			    filterrule = 0;	
			}
			if (AP_MAC_ALLOW == profGlobal->action)
			{
			    filterrule = 1;	
			}
			else
			{
			    filterrule = 2;	
			}
			memset(mac, 0, 20);
    			sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",  table.Entry[i].Addr[0], table.Entry[i].Addr[1],table.Entry[i].Addr[2], table.Entry[i].Addr[3],table.Entry[i].Addr[4], table.Entry[i].Addr[5]);
			macStrLen = strlen(oldMacList);
   			if(strstri(oldMacList,mac))
			{
				continue;
			}
			delColon(mac, ':');
			index = ProfNewInst(mibType, mac, FALSE);
			if (index == PROFFAIL)
			{
				ErrMsg = getTransDataForC(C_LANG_INDEX_MAC_FILTER_LIST_MAX);
			    setTheErrorMsgOnFree(ErrMsg);
			    return 0;
			}
			ProfBackupByIndex(mibType, profAction, index, &profhead);
			prof = (Wireless5gMacFilterProfile *)ProfGetInstPointByName(mibType, mac);
			converMac12To6Byte(mac, &tmp);
			prof->mac = tmp;
			nvramWriteCommit();
			iscommit=1;
		}
		totalrece++;
	}
	if(printfOrAddMac)
	{
	    websWrite(wp,"var totalrecs=%d;", totalrece);
	}
	close(s);
	if(iscommit)
	{
	    ProfInstLowHigh(mibType, &max, &min);
	    for (index = min; index < max; index++)
	    {
		prof = (Wireless5gMacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
		profName = ProfGetInstNameByIndex(mibType, index);
		if (0 != strcmp(profName, ""))
		{
		    tmpmac = converMac6To12Byte(prof->mac);
		    Conversion2StdMac(tmpmac);
		    strcat(oldMacList2, tmpmac);
		    strcat(oldMacList2, ";");
		}
		else
		{
		    continue;
		}
	    }
	    oldMacList2[strlen(oldMacList2)-1] = '\0';
	    doSystem("iwpriv rai0 set \"ACLAddEntry=%s\" ", oldMacList2);
	    if(filterrule)
	    {
		if((filterrule == 2) && (profGlobal->filterEn == FUN_ENABLE))
		{
		    doSystem("iwpriv rai0 set AccessPolicy=%d", filterrule);
		}
		else if ((filterrule == 1) && (profGlobal->filterEn == FUN_ENABLE))
		{
		    doSystem("iwpriv rai0 set AccessPolicy=%d", 1);
		}
	    }
	    else
	    {
		    doSystem("iwpriv rai0 set AccessPolicy=%d", 0);
	    }
	}
	return 0;
}
#endif

/***********************************************************************
 * 函 数 名：   getWlanStationList
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static int getWlanStationList(int eid, webs_t wp, int argc, char_t **argv)
{
HOSTLIST_DEBUG(("\n---> getWlanStationList() begin <---\n"));
    const char *profName = NULL;
    char wlanMac[1024] = {0};
    int min, max, index;
    char *tmpmac; 
    websWrite(wp,"var HostMacs=new Array();");
    websWrite(wp,"var Allows=new Array();");
    websWrite(wp,"var BWs=new Array();");
    websWrite(wp,"var SGIs=new Array();");
    MibProfileType mibType = MIB_PROF_WIRELESS_MAC_FILTER;
    WirelessMacFilterProfile *prof = NULL;
    
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
    getStationFromIoctl(wp, wlanMac, 1);
    getTheErrorMsg(eid, wp, argc, argv);  
HOSTLIST_DEBUG(("\n---> getWlanStationList() begin <---\n"));
    return 0;
}

#if (WIRELESS_5G == FYES)
static int getWlanStationList_5g(int eid, webs_t wp, int argc, char_t **argv)
{
    const char *profName = NULL;
    char wlanMac[1024] = {0};
    int min, max, index;
    char *tmpmac; 
    websWrite(wp,"var HostMacs=new Array();");
    websWrite(wp,"var Allows=new Array();");
    websWrite(wp,"var BWs=new Array();");
    websWrite(wp,"var SGIs=new Array();");
    MibProfileType mibType = MIB_PROF_WIRELESS_5G_MAC_FILTER;
    Wireless5gMacFilterProfile *prof = NULL;
    
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
    getStationFromIoctl_5g(wp, wlanMac, 1);
    getTheErrorMsg(eid, wp, argc, argv);  
    return 0;
}
#endif

/***********************************************************************
 * 函 数 名：   formDefineWirelessMacFilter
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
void formDefineWirelessHostList(void)
{
    websFormDefine(T("ConfigAddAllMAC"), formConfigAddAllMAC);
    websAspDefine(T("aspOutWlanStationList"), getWlanStationList);

#if (WIRELESS_5G == FYES)
    websFormDefine(T("ConfigAddAllMAC_5g"), formConfigAddAllMAC_5g);
    websAspDefine(T("aspOutWlanStationList_5g"), getWlanStationList_5g);
#endif
}
#endif
