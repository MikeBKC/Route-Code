#include	"uttMachine.h"
#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"comExeFun.h"
#include	"uttfunction.h"
#include	"argcmd.h"
#include        "wuScript.h"
#ifdef CONFIG_DEFAULTS_KERNEL_2_6_21
    #include  <linux/types.h>
    #include  <linux/socket.h>
    #include  <linux/if.h>
#endif
#include <linux/wireless.h>
#include <ctype.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#define SIOCDEVPRIVATE              0x8BE0
#define SIOCIWFIRSTPRIV             SIOCDEVPRIVATE
#define RTPRIV_IOCTL_GET_MAC_TABLE      (SIOCIWFIRSTPRIV + 0x0F)
#define MAX_NUMBER_OF_MAC               (116 - ((16 + 1) * 1))
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
    unsigned char            ApIdx;
    unsigned char            Addr[6];
    unsigned char            Aid;
    unsigned char            Psm;     // 0:PWR_ACTIVE, 1:PWR_SAVE                                       
    unsigned char            MimoPs;  // 0:MMPS_STATIC, 1:MMPS_DYNAMIC, 3:MMPS_Enabled                  
    char                     AvgRssi0;
    char                     AvgRssi1;
    char                     AvgRssi2;
    unsigned int             ConnectedTime;
    MACHTTRANSMIT_SETTING    TxRate;
    unsigned int LastRxRate;
    short StreamSnr[3];
    short SoundingRespSnr[3];
} RT_802_11_MAC_ENTRY;
typedef struct _RT_802_11_MAC_TABLE {
    unsigned long            Num;
    RT_802_11_MAC_ENTRY      Entry[64]; //MAX_LEN_OF_MAC_TABLE = 32             
} RT_802_11_MAC_TABLE;
#if(WIRELESS_5G == FYES)
typedef struct _RT_802_11_MAC_TABLE_5G {
        unsigned long            Num;
            RT_802_11_MAC_ENTRY      Entry[MAX_NUMBER_OF_MAC]; //MAX_LEN_OF_MAC_TABLE = 32          
}RT_802_11_MAC_TABLE_5G;
#endif

#if (UTT_SMART_UI == FYES)
/*新UI开启*/
#include	"translate.h"
#define USER_MAC_PROC_FILE "/proc/net/statsPerIp"
#define USER_RATE_PROC_FILE "/proc/net/flowS"
#define USER_UTT_APPSTATUS_FILE "/proc/uttAppStat"
#define MAX_LINE_LEN        200  

#ifdef CONFIG_UTT_SKB_IIFTYPE
typedef enum {
    DEFAULT_IFTYPE = 0,/*当不需要指定接口类型值时，可赋值为默认值0*/

    IS_WIRELESS,/*表示数据包来自无线接口*/

    MAX_IFTYPE

} IFTYPE;
#endif

#if (SHOW_ALL_WIRE_MAC == FYES)
typedef struct macTreeNode{
    unsigned char	Addr[6];//mac地址
    struct macTreeNode	*left;
    struct macTreeNode	*right;
}MacTreeNode, *pMacTreeNode;

extern void getUnusedWireMac(webs_t wp, int *indexp, struct macTreeNode *rootNode, int is5g);
extern void getUnusedWireNum(int *indexp, struct macTreeNode *rootNode, int is5g);
#endif

extern time_t getUptime();
extern int isWilessClient(char *mac);
static int outputUserStatistics(int eid, webs_t wp, int argc, char **argv);
#if (UTT_BRIDEG_MODE_FLAG == FYES)
static int outputbridgeusers(int eid, webs_t wp, int argc, char **argv);
int getbridgeusers(void);
#endif
static int OutputTimeFlow(webs_t wp, char_t *ip, int index);
int convertDateTimeFromSecToStr(unsigned long sec, char *timeStr, int strLen);
#if (CLOUD_CONFIG == FYES)
/***********************************************************************
 *  * 函 数 名：   getHostnameByMac
 *   * 功能描述：   根据mac从我的上线提醒表中得到hostname
 *    * 创建日期：  2015-11-11
 *      * 作者：       xrg
 *       * 附加说明：	参数出入的为17位的mac比如,00:22:aa:11:22:33
 *        ***********************************************************************/
static char *getHostnameByMac(char *mac)
{
    uttSemP(SEM_CLOUD_CONFIG,0);
    MacAddr tmp_mac;
    int i;

    memset(&tmp_mac,0,sizeof(tmp_mac));
    for(i = 0;i < MAX_NOTICE_MAC_LIST;i++)
    {
	if(strcmp(converMac6To17Byte(nvramProfile->noticeMacList[i].macaddr),mac)==0)
	{
	    uttSemV(SEM_CLOUD_CONFIG,0);
	    return &(nvramProfile->noticeMacList[i].name);
	}
    }
    uttSemV(SEM_CLOUD_CONFIG,0);
    return NULL;	
}
#endif
#if (SHOW_ALL_WIRE_MAC == FYES)
static int compareNode(unsigned char *addr, unsigned char *newaddr, int pos)
{
    if (newaddr[pos] > addr[pos])
    {
	return 2;
    }
    else if (newaddr[pos] < addr[pos])
    {
	return 0;
    }
    else if(pos == 5)
    {
	return 1;
    }
    return compareNode(addr, newaddr, ++pos);
}

static void insertNode(pMacTreeNode *nodep, pMacTreeNode newNode)
{
    int result = 0;
    if (*nodep == NULL){
	*nodep = newNode;
	return;
    }
    pMacTreeNode node = *nodep;
    result = compareNode(node->Addr, newNode->Addr, 0);
    if (result == 2){
	insertNode(&node->right, newNode);
    }
    else if (result == 0){
	insertNode(&node->left, newNode);
    }
}

int searchNode(pMacTreeNode node, unsigned char *macaddr)
{
    int result = 0;
    if (node == NULL){
	return 0;
    }
    result = compareNode(node->Addr, macaddr, 0);
    if (result == 1){
	return 1;
    }else if (result == 2){
	if (node->right){
	    return searchNode(node->right, macaddr);
	}
    }else{
	if (node->left){
	    return searchNode(node->left, macaddr);
	}
    }
    return 0;
}

static void showTree(pMacTreeNode node)
{
    if (node == NULL){
	return;
    }
    showTree(node->left);
    printf("========%02X:%02X:%02X:%02X:%02X:%02X\n", node->Addr[0], node->Addr[1], node->Addr[2], node->Addr[3], node->Addr[4], node->Addr[5]);
    showTree(node->right);
}

static void releaseTree(pMacTreeNode node)
{
    if (!node){
	return;
    }
    if (node->left){
	releaseTree(node->left);
    }
    if (node->right){
	releaseTree(node->right);
    }
    free(node);
}
#endif

/********************************************************************
 * 函数： formDefineUserLinkRate
 * 功能： 页面与交互转换函数
 * 创建： 2012-06-11
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
void formDefineUserLinkRate(void)
{
    websAspDefine(T("outputUserStatistics"), outputUserStatistics);
#if (UTT_BRIDEG_MODE_FLAG == FYES)
    websAspDefine(T("outputbridgeusers"), outputbridgeusers);
#endif
}


/********************************************************************
 * 函数： convertDateTimeFromSecToStr 
 * 功能： 将以秒形式的时间转换成"x天x小时x分x秒"形式的字符串
 * 创建： 2013-10-25
 * 参数： sec->以秒为单位的时间；timeStr->存放转换后的时间串；strLen->timeStr数组的长度
 * 返回： 0->失败；1->成功
 * 输出： 时间串
 * 作者： 赵冲 
 ********************************************************************/ 
int convertDateTimeFromSecToStr(unsigned long sec, char *timeStr, int strLen)
{
    int updays=0,uphours=0,upminutes=0,upsecs=0;
    const char *com_day = NULL, *com_hour = NULL, *com_minute = NULL, *com_second = NULL;
    int ret = 0;

    updays = sec / (60*60*24);
    upminutes = sec / 60;
    uphours = (upminutes / 60) % 24;
    upminutes %= 60;
    upsecs = sec % 60;
#if (UTT_SMART_UI == FYES)
    com_day = getTransDataForC(C_LANG_INDEX_SHORT_DAY);
    com_hour = getTransDataForC(C_LANG_INDEX_SHORT_HOUR);
    com_minute = getTransDataForC(C_LANG_INDEX_SHORT_MINUTE);
    com_second = getTransDataForC(C_LANG_INDEX_SHORT_SECOND);
#else
    com_day = getTransDataForC(C_LANG_INDEX_DAYS);
    com_hour = getTransDataForC(C_LANG_INDEX_HOURS);
    com_minute = getTransDataForC(C_LANG_INDEX_MINUTES);
    com_second = getTransDataForC(C_LANG_INDEX_SECONDS);
#endif
    if ( com_day != NULL &&
	     com_hour != NULL &&
	     com_minute != NULL &&
	     com_second != NULL ) {
	snprintf(timeStr,strLen,"%d%s%d%s%d%s%d%s",updays, com_day, uphours, com_hour, upminutes, com_minute, upsecs, com_second);
	ret = 1;
    }

    return ret;
}
/********************************************************************
 * 函数： OutputTimeFlow
 * 功能： 输出用户上下行速率
 * 创建： 2012-06-11
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int OutputTimeFlow(webs_t wp, char_t *ip, int index)
{
    FILE *fp;
    char_t linestr[MAX_LINE_LEN];
    char_t UserIp[20];
    char_t inbit[20];
    char_t outbit[20];
    int noRead = 1;
    memset(inbit,0,sizeof(inbit));
    memset(outbit,0,sizeof(outbit));
    fp = fopen(USER_RATE_PROC_FILE, "r");
    if(fp != NULL)
    {
	while (feof(fp) == 0)
	{
	    memset(linestr, 0, sizeof(linestr));
	    if (fgets(linestr, MAX_LINE_LEN, fp) != NULL)
	    {
		sscanf(linestr, "%s%s%s", UserIp, inbit, outbit);
		if(strcmp(UserIp, ip) == 0)
		{
		    websWrite(wp, "inbits[%d]=\"%d\";\n", index, (strtoul(inbit, 0, 10) * 8U/1024U));
		    websWrite(wp, "outbits[%d]=\"%d\";\n", index, (strtoul(outbit, 0, 10)* 8U/1024U));
		    noRead = 0;

		}
	    }
	}
	fclose(fp);    
    }
    /* 如果速率没读到，显示0 */
    if (noRead == 1) {
	websWrite(wp, "inbits[%d]=\"0\";\n", index);
        websWrite(wp, "outbits[%d]=\"0\";\n", index);
    }
    return 0;
}
/********************************************************************
 * 函数： outputUserStatistics 
 * 功能： 输出用户所有状态
 * 创建： 2013-10-24
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： 赵冲 
 ********************************************************************/ 
static int outputUserStatistics(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    char_t buffer[20];
    char_t linestr[MAX_LINE_LEN];
    char_t ipAddr[20];
    FILE *fp;
    char_t inbit[20];
    char_t outbit[20];
    int i = 0,j = 0;
    int mac_not_zero = 0;

    unsigned int ty_rx_zero = 0;
    unsigned int ty_tx_zero = 0;

    Uint64 buffer_int = 0U;
    char_t rxBytes[64], txBytes[64];
    char IPs[20];
    int strLen = 50;/*时间串数组长度*/
    char timeStr[50];
    unsigned long onlineTime = 0ul;
    unsigned int perIpStats[10] = {0};
    IFTYPE iftype;

////////////
#if (UTT_KNOWIFI == FYES)
	MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
	WebAuthGlobalProfile *profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
	MibProfileType mibType = MIB_PROF_KNOWIFI_DOMAIN;
	KnowifiDomainProfile *prof = NULL;
	int k, max, min;
	int flag = 0;
	ProfInstLowHigh(mibType, &max, &min);
#endif
///////////

#if (SHOW_ALL_WIRE_MAC == FYES)
    pMacTreeNode rootNode = NULL;
    pMacTreeNode newNode = NULL;
#endif

/*xrg*/
#if (ARP_BINDS == FYES)
    char arp_mac[20];
    char *arp_bind_name = NULL;
    char arp_ip[20];
    memset(arp_mac,0,sizeof(arp_mac));
    memset(arp_ip,0,sizeof(arp_ip));
#endif
    memset(buffer,0,sizeof(buffer));
    memset(ipAddr,0,sizeof(ipAddr));
    memset(rxBytes,0,sizeof(rxBytes));
    memset(txBytes,0,sizeof(txBytes));
    memset(IPs,0,sizeof(IPs));
    /*get SystemProfile Pointer*/
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibTypeSys, 0);
    fp = fopen(USER_MAC_PROC_FILE, "r");
    if(fp != NULL)
    {
	memset(linestr, 0, sizeof(linestr));
	fgets(linestr, MAX_LINE_LEN, fp);
	while (feof(fp) == 0)
	{
	    mac_not_zero = 0;

	    ty_rx_zero = 0;
	    ty_tx_zero = 0;

	    if (fscanf(fp, "%s", buffer) == 1)/*IP*/
	    {
		strcpy(IPs, buffer);
		strcpy(ipAddr, buffer);
#if (ARP_BINDS == FYES)
		strcpy(arp_ip,buffer);	    
#endif
	    } else { /* IP没有读出来, continue. feof引起问题 */
		continue;
	    }

	    if (fscanf(fp, "%s", buffer) == 1)/*Mac*/
	    {
		if(strcmp(buffer, "00:00:00:00:00:00") != 0)
		{
		    mac_not_zero = 1;
#if (ARP_BINDS == FYES)
		strcpy(arp_mac,buffer);	    
#endif
		    websWrite(wp, "Macs[%d]=\"%s\";\n", i, buffer);
		}
	    }
#if 0
	    /*如果mac地址不为0，则判断是否为无线用户*/
	    if(strcmp(buffer, "00:00:00:00:00:00") != 0) {
		if (isWilessClient(buffer)) {/*是无线用户*/
		    websWrite(wp, "connectType[%d]=\"wireless\";\n", i);
		} else {
		    websWrite(wp, "connectType[%d]=\"wired\";\n", i);
		}
	    }
#endif
	    if(mac_not_zero == 0) /* 跳过mac地址全零的固定接入 */
	    {
		/* 跳过此行 */
		fgets(linestr, MAX_LINE_LEN, fp);
		continue;
	    }
		websWrite(wp, "IPs[%d]=\"%s\";\n", i, IPs);
	    if(strcmp(IPs, "0.0.8.6") == 0) /* IP为0.0.8.6的过滤 */
	    {
		/* 跳过此行 */
		fgets(linestr, MAX_LINE_LEN, fp);
		continue;
	    }
#if (SHOW_ALL_WIRE_MAC == FYES)
	    newNode = malloc(sizeof(MacTreeNode));
	    memset(newNode, 0, sizeof(MacTreeNode));
	    sscanf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", &newNode->Addr[0], &newNode->Addr[1], &newNode->Addr[2], &newNode->Addr[3], &newNode->Addr[4], &newNode->Addr[5]);
	    newNode->left = NULL;
	    newNode->right = NULL;
	    insertNode(&rootNode, newNode);
#endif
		
#if 0
		OutputTimeFlow(wp, ipAddr, i);/*上下行速率*/
#endif
	    if (fscanf(fp, "%s", buffer) == 1)/*上网时间*/
	    {
		onlineTime = getUptime() - (time_t)strtoul(buffer, 0 ,10);/*取得上网时间*/
		memset(timeStr,0,strLen);/*初始化时间串数组*/
		if (convertDateTimeFromSecToStr(onlineTime,timeStr,strLen) == 1) {
		    websWrite(wp, "startTimes[%d]=\"%s\";\n", i, timeStr);
		}
	    }
	    if (fscanf(fp, "%llu", &buffer_int) == 1)/*总上行流量*/
	    {
		if(buffer_int==0U)
		{
		    ty_rx_zero = 1;
		}
		sprintf(rxBytes, "%llu", (Uint64)(buffer_int >> 17U));
		websWrite(wp, "rxBytes[%d]=\"%s\";\n", i, rxBytes);
	    }
	    if (fscanf(fp, "%llu", &buffer_int) == 1)/*总下行流量*/
	    {
		if(buffer_int==0U)
		{
		    ty_tx_zero = 1;
		}
		sprintf(txBytes, "%llu", (Uint64)(buffer_int >> 17U));
		websWrite(wp, "txBytes[%d]=\"%s\";\n", i, txBytes);
	    }

	    if(ty_tx_zero && ty_rx_zero)
	    {
		/* 跳过此行 */
		fgets(linestr, MAX_LINE_LEN, fp);
		continue;
	    }

	    for (j = 0; j < 10; j++) {
		fscanf(fp, "%u", &perIpStats[j]);
	    }
	    if(fscanf(fp,"%s",outbit))
	    {    
		websWrite(wp, "outbits[%d]=\"%d\";\n", i, (strtoul(outbit, 0, 10) * 8U/1024U));
	    }    
	    else 
	    {    
		websWrite(wp, "outits[%d]=\"0\";\n", i);
	    }    

	    if(fscanf(fp,"%s",inbit))
	    {    
		websWrite(wp, "inbits[%d]=\"%d\";\n", i, (strtoul(inbit, 0, 10)* 8U/1024U));
	    }    
	    else 
	    {    
		websWrite(wp, "inbits[%d]=\"0\";\n", i);
	    }
	    if (fscanf(fp,"%u",&iftype)) {
		if (iftype == IS_WIRELESS) {/*是无线用户*/
		    websWrite(wp, "connectType[%d]=\"wireless\";\n", i);
		} else {
		    websWrite(wp, "connectType[%d]=\"wired\";\n", i);
		}
	    }
/*xrg  Username*/
#if (ARP_BINDS == FYES)
	   if((arp_bind_name = getNameByIPMac(IPs, arp_mac)) != NULL)  
	    {
#if (UTT_KNOWIFI == FYES)
		if (profG->knowifi_enable == 1)
		{
		    for(k = min; k < max; k++)
		    {
			prof = (KnowifiDomainProfile*)ProfGetInstPointByIndex(mibType, k);
			if (prof != NULL && ProfInstIsFree(prof) == 0)
			{
			    if(strcmp(prof->mac,arp_mac) == 0)
			    {
				flag = 1;
				break;
			    }
			}
		    }
		}
		if(flag == 1)
		{
			
			websWrite(wp, "Username[%d]=\"%s\";\n", i,prof->username );
			flag = 0;
		}
		else 
		{
			websWrite(wp, "Username[%d]=\"%s\";\n", i, arp_bind_name);
		}
#else 
			websWrite(wp, "Username[%d]=\"%s\";\n", i, arp_bind_name);
#endif
		//printf("arp bind = %s\n",arp_bind_name); 
	    }
#if (CLOUD_CONFIG == FYES)
	    else if((arp_bind_name = getHostnameByMac(arp_mac)) != NULL)
	    {
			
#if (UTT_KNOWIFI == FYES)
		if (profG->knowifi_enable == 1)
		{
		    for(k = min; k < max; k++)
		    {
			prof = (KnowifiDomainProfile*)ProfGetInstPointByIndex(mibType, k);
			if (prof != NULL && ProfInstIsFree(prof) == 0)
			{
			    if(strcmp(prof->mac,arp_mac) == 0)
			    {
				flag = 1;
				break;
			    }
			}
		    }
		}
		if(flag == 1)
		{
			
			websWrite(wp, "Username[%d]=\"%s\";\n", i,prof->username );
			flag = 0;
		}
		else 
		{
			websWrite(wp, "Username[%d]=\"%s\";\n", i, arp_bind_name);
		}
#else 
			websWrite(wp, "Username[%d]=\"%s\";\n", i, arp_bind_name);
#endif
		//printf("username= %s\n",arp_bind_name); 
	    }
#endif
#endif
	    else
	    {
		websWrite(wp, "Username[%d]=\"\";\n", i);
		//printf("arp_bind_name and username == NULL\n"); 
	    }

	    i++;
	}
#if (SHOW_ALL_WIRE_MAC == FYES)
/*
	getUnusedWireMac(wp, &i, rootNode, 0);
#if (WIRELESS_5G == FYES)
	getUnusedWireMac(wp, &i, rootNode, 1);
#endif
*/
	releaseTree(rootNode);
#endif
	fclose(fp);    
    }
    websWrite(wp, "var totalrecs=%d;", i);

    return 0;
}

#if (UTT_BRIDEG_MODE_FLAG == FYES)
static int outputbridgeusers(int eid, webs_t wp, int argc, char **argv)
{
    int s,i,strLen = 48;
#if(WIRELESS_5G == FYES)
    int j = 0,socket_5g = 0;
    struct iwreq iwr_5g;
    char buf_5g[32];
    memset(buf_5g,0,sizeof(buf_5g));
    char timeStr_5g[48] = {0};
     MibProfileType mibType = MIB_PROF_WIRELESS;
     WirelessProfile *prof= NULL;
     prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 1);
#endif
    struct iwreq iwr;
    char buf[32];
    char timeStr[48];
    memset(buf,0,sizeof(buf));
    RT_802_11_MAC_TABLE table = {0};
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
        return -1;
    }
    for(i = 0;i < table.Num; i++)
    {
        sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", table.Entry[i].Addr[0], table.Entry[i].Addr[1],table.Entry[i].Addr[2], table.Entry[i].Addr[3],table.Entry[i].Addr[4], table.Entry[i].Addr[5]);
                websWrite(wp, "Macs[%d]=\"%s\";\n", i, buf);
        if (convertDateTimeFromSecToStr(table.Entry[i].ConnectedTime,timeStr,strLen) == 1) {
                websWrite(wp, "startTimes[%d]=\"%s\";\n", i, timeStr);
        }
		websWrite(wp, "connectType[%d]=\"wireless\";\n", i);
        if(table.Entry[i].TxRate.field.BW == 0)
        {
		    websWrite(wp, "BWs[%d]=\"20M\";\n", i);
        }else
        {
		    websWrite(wp, "BWs[%d]=\"40M\";\n", i);
        }
        memset(timeStr,0,strlen(timeStr));/*初始化时间串数组*/
        memset(buf,0,sizeof(buf));
    }
    close(s);
#if(WIRELESS_5G == FYES)
    RT_802_11_MAC_TABLE_5G table_5g = {0};
    socket_5g = socket(AF_INET, SOCK_DGRAM, 0);
    strncpy(iwr_5g.ifr_name, "rai0", IFNAMSIZ);
    iwr_5g.u.data.pointer = (caddr_t) &table_5g;
    if (socket_5g < 0)
    {
        return -1;
    }
    if (ioctl(socket_5g, RTPRIV_IOCTL_GET_MAC_TABLE, &iwr_5g) < 0)
    {
        close(socket_5g);
        return -1;
    }
    for(j;j < table_5g.Num; j++)
    {
        sprintf(buf_5g, "%02X:%02X:%02X:%02X:%02X:%02X", table_5g.Entry[j].Addr[0], table_5g.Entry[j].Addr[1],table_5g.Entry[j].Addr[2], table_5g.Entry[j].Addr[3],table_5g.Entry[j].Addr[4], table_5g.Entry[j].Addr[5]);
        websWrite(wp, "Macs[%d]=\"%s\";\n", j+i, buf_5g);
        if (convertDateTimeFromSecToStr(table_5g.Entry[j].ConnectedTime,timeStr_5g,strLen) == 1) {
                    websWrite(wp, "startTimes[%d]=\"%s\";\n", i+j, timeStr_5g);
        }
        websWrite(wp, "connectType[%d]=\"wireless\";\n", i+j);
        if(table_5g.Entry[j].TxRate.field.BW == 0)
        {
            if (prof->mBaseCf.VChanWidth == 1) {
                websWrite(wp, "BWs[%d]=\"80M\";\n", i+j);
            } else {
                websWrite(wp, "BWs[%d]=\"20M\";\n", i+j);
            }
        }else
        {
                websWrite(wp, "BWs[%d]=\"40M\";\n", i+j);
        }
        memset(timeStr_5g,0,strlen(timeStr_5g));/*初始化时间串数组*/
        memset(buf_5g,0,sizeof(buf_5g));
    }
    close(socket_5g);
#endif
    websWrite(wp, "var totalrecs=%d;", i+j);
    return 0;
}
int getbridgeusers(void)
{
    int i,s;
    struct iwreq iwr;
#if(WIRELESS_5G == FYES)
    int j = 0,socket_5g = 0;
    struct iwreq iwr_5g;
#endif
   
    RT_802_11_MAC_TABLE table = {0};
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
        return -1;
    }

    i = table.Num;  
    close(s);
#if (WIRELESS_5G == FYES)
    RT_802_11_MAC_TABLE_5G table_5g = {0};
    socket_5g = socket(AF_INET, SOCK_DGRAM, 0);
    strncpy(iwr_5g.ifr_name, "rai0", IFNAMSIZ);
    iwr_5g.u.data.pointer = (caddr_t) &table_5g;
    if (socket_5g < 0)
    {
        return -1;
    }
    if (ioctl(socket_5g, RTPRIV_IOCTL_GET_MAC_TABLE, &iwr_5g) < 0)
    {
        close(socket_5g);
        return -1;
    }
    j = table_5g.Num;  
    close(socket_5g);
#endif

    return i+j;
}
#endif
#if (SHOW_ALL_WIRE_MAC == FYES)
/*取得所有用户，包括无流量用户*/
int getAllUserNum(void)
{
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    char_t buffer[20];
    char_t linestr[MAX_LINE_LEN];
    FILE *fp;
    int i = 0,j = 0;
    int mac_not_zero = 0;
    Uint64 buffer_int = 0U;
    char IPs[20];
    unsigned int perIpStats[10] = {0};
    unsigned int ty_rx_zero = 0;
    unsigned int ty_tx_zero = 0;
    IFTYPE iftype;
    pMacTreeNode rootNode = NULL;
    pMacTreeNode newNode = NULL;

    memset(buffer,0,sizeof(buffer));
    memset(IPs,0,sizeof(IPs));
    /*get SystemProfile Pointer*/
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibTypeSys, 0);
    fp = fopen(USER_MAC_PROC_FILE, "r");
    if(fp != NULL)
    {
	memset(linestr, 0, sizeof(linestr));
	fgets(linestr, MAX_LINE_LEN, fp);
	while (feof(fp) == 0)
	{
	    mac_not_zero = 0;
	    ty_rx_zero = 0;
	    ty_tx_zero = 0;
	    if (fscanf(fp, "%s", buffer) == 1)/*IP*/
	    {
		strcpy(IPs, buffer);
	    } else { /* IP没有读出来, continue. feof引起问题 */
		continue;
	    }

	    if (fscanf(fp, "%s", buffer) == 1)/*Mac*/
	    {
		if(strcmp(buffer, "00:00:00:00:00:00") != 0)
		{
		    mac_not_zero = 1;
		}
	    }

	    if(mac_not_zero == 0) /* 跳过mac地址全零的固定接入 */
	    {
		/* 跳过此行 */
		fgets(linestr, MAX_LINE_LEN, fp);
		continue;
	    }
	    if(strcmp(IPs, "0.0.8.6") == 0) /* IP为0.0.8.6的过滤 */
	    {
		/* 跳过此行 */
		fgets(linestr, MAX_LINE_LEN, fp);
		continue;
	    }

	    if (fscanf(fp, "%s", buffer) != 1)/*上网时间*/
	    {
		    //printf("time error\n");
	    }
	    if (fscanf(fp, "%llu", &buffer_int) == 1)/*总上行流量*/
	    {
		if(buffer_int==0U)
		{
		    ty_rx_zero = 1;
		}
	    }
	    if (fscanf(fp, "%llu", &buffer_int) == 1)/*总下行流量*/
	    {
		if(buffer_int==0U)
		{
		    ty_tx_zero = 1;
		}
	    }

	    if(ty_tx_zero && ty_rx_zero)
	    {
		/* 跳过此行 */
		fgets(linestr, MAX_LINE_LEN, fp);
		continue;
	    }
	    newNode = malloc(sizeof(MacTreeNode));
	    memset(newNode, 0, sizeof(MacTreeNode));
	    sscanf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", &newNode->Addr[0], &newNode->Addr[1], &newNode->Addr[2], &newNode->Addr[3], &newNode->Addr[4], &newNode->Addr[5]);
	    newNode->left = NULL;
	    newNode->right = NULL;
	    insertNode(&rootNode, newNode);
	    fgets(linestr, MAX_LINE_LEN, fp);/*跳过对无用信息的读取*/

#if 0
	    fscanf(fp, "%s", buffer);
	    fscanf(fp, "%llu", &buffer_int);
	    fscanf(fp, "%llu", &buffer_int);
	    for (j = 0; j < 10; j++) {
		fscanf(fp, "%u", &perIpStats[j]);
	    }
	    fscanf(fp,"%u",&iftype);
#endif

	    i++;
	}
/*
	getUnusedWireNum(&i, rootNode, 0);
#if (WIRELESS_5G == FYES)
	getUnusedWireNum(&i, rootNode, 1);
#endif
*/
	releaseTree(rootNode);
	fclose(fp);    
    }

    return i;
}
#endif

#else

#if (USER_STATUS == FYES)

#include	"translate.h"
#ifdef CONFIG_UTT_NETFILTER
#include <include/linux/netfilter_ipv4/utt_netfilter.h>
#include <include/linux/netfilter/xt_UTTHOSTSTA.h>
extern int uttNfNlSock;
#endif

extern time_t getUptime();
extern char* getNameByIPMac(char *ip, char *mac);
#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif
static __u64 adjust_count = 0U;
static __u64 adjust_count_control= 1U;
static __u64 adjust_p2p = 0U;
static __u64 adjust_video = 0U;
static __u64 adjust_game = 0U;
static __u64 adjust_shop = 0U;
static __u64 adjust_sns = 0U;
static __u64 adjust_webgame = 0U;
static __u64 adjust_forum = 0U;
static __u64 adjust_p2p_backup = 0U;
static __u64 adjust_video_backup = 0U;
static __u64 adjust_game_backup = 0U;
static __u64 adjust_shop_backup = 0U;
static __u64 adjust_sns_backup = 0U;
static __u64 adjust_webgame_backup = 0U;
static __u64 adjust_forum_backup = 0U;
static __u64 adjust_other = 0U;
static __u64 adjust_other_drop = 0U;/*没有分配但是不显示的丢弃*/
static FunEnableEnum enableBackUp = FUN_DISABLE;
//static int aspOutputUserStats(int eid, webs_t wp, int argc, char_t ** argv);
static void formRateForUserLink(webs_t wp, char_t *path, char_t *query);
static int OutputTimeFlow(webs_t wp, char_t *ip, int index);
static __u64 adjustOtherFlowPercent(__u64 total); 
#if 0
static int OutputUserAndAuth(webs_t wp, char_t *ip, int index);
#endif
#if (PPPOE_SERVER == FYES)
static int OutputPPPoE(webs_t wp, char_t *ip, int index);
static void formPPPoERemark(char_t *userName, char_t *remark);
#endif
#if (WEB_AUTH == FYES)
static int OutputWebAuth(webs_t wp, char_t *ip, int index);
static void formWebAuthRemark(char_t *userName, char_t *remark);
#endif
#if 0
static int OutputIpGroup(webs_t wp, char_t *ip, int index);
#else
#if (IP_GRP == FYES)
static int OutputIpGroup(webs_t wp, char_t *UserName, int index, ipGrpEntryType userType);
static int ipGrpSearchIp(IpGrpProfile *curProf, char *userName, ipGrpEntryType userType);
static int userNameIsIn(IpGrpProfile *curProf, int i, char *userName, ipGrpEntryType userType);
#endif
#endif
static int OutputPie(webs_t wp);
static int getUserStatusGlobal(int eid, webs_t wp, int argc, char **argv);
#ifdef CONFIG_UTT_NETFILTER
static int clearsUserStats(void);
#endif
#define USER_MAC_PROC_FILE "/proc/net/statsPerIp"
#define USER_RATE_PROC_FILE "/proc/net/flowS"
#define USER_UTT_APPSTATUS_FILE "/proc/uttAppStat"
#define MAX_LINE_LEN        200  

#if (ARP_BINDS == FYES)
extern char* getMacByIpFromSysArpTb(char *arpIp);
#endif
#define SERIOUS_LVL 70
#define LIGHT_LVL   50

/*
 *  根据用户行为统计，输出影响工作级别
 *  输入参数：	
 *	ipStats	-   用户行为统计
 *  输出参数：
 *	无
 *  返回值：
 *	1   -	严重
 *	2   -	轻微
 *	3   -	正常
 */
static int userBvLevel(unsigned int *ipStats, int total) {
    unsigned int badBv = 0;
    unsigned int totalBv = 0;
    unsigned int percent = 0;
    int rst = 3;
    int i;

    badBv = ipStats[UTT_APP_STOCK_ARR] + ipStats[UTT_APP_GAME_ARR] + ipStats[UTT_APP_SHOP_ARR] + ipStats[UTT_APP_SNS_ARR] + ipStats[UTT_APP_WEBGAME_ARR]; /* 恶性行为统计：股票，游戏，购物，社交，网页游戏 */

    /* 用户总行为统计 */
    for (i = 0; i < total; i++) {
	totalBv += ipStats[i];
    }

    /* 比率 */
    if (totalBv == 0) {
	percent = 0;
    } else {
	percent = badBv * 100 / totalBv;
    }

    if (percent > SERIOUS_LVL) {
	rst = 1;
    } else if (percent > LIGHT_LVL) {
	rst = 2;
    } else {
	rst = 3;
    }

    return rst;
}

#define READ_RATE_BY_HASH 0
#if READ_RATE_BY_HASH
/*ip对应速率*/
typedef struct st_rate{
    char ipstr[20];
    unsigned long inbit;
    unsigned long outbit;
    struct st_rate *next;
}*Rate;

Rate *ipHashTab = NULL;/*数组，ip最后八位做为下标，保存速率结构体*/

/*构建hash数组，通过读取proc文件获得信息*/
static void initHashTab(void)
{
    FILE *fp;
    char_t linestr[MAX_LINE_LEN];
    char_t UserIp[20];
    char_t inbit[20];
    char_t outbit[20];
    Rate tmp;
    Rate new;
    int index;
    int ipnum1, ipnum2, ipnum3, ipnum4;
    memset(inbit,0,sizeof(inbit));
    memset(outbit,0,sizeof(outbit));
    fp = fopen(USER_RATE_PROC_FILE, "r");
    if(fp != NULL)
    {
	ipHashTab = malloc(257 * sizeof(Rate));
	memset(ipHashTab, 0, sizeof(Rate) * 257);
	if (ipHashTab == NULL)
	{
	    return;
	}
	while (feof(fp) == 0)
	{
	    memset(linestr, 0, sizeof(linestr));
	    if (fgets(linestr, MAX_LINE_LEN, fp) != NULL)
	    {
		sscanf(linestr, "%s%s%s", UserIp, inbit, outbit);
		sscanf(UserIp, "%d.%d.%d.%d", &ipnum1, &ipnum2, &ipnum3, &ipnum4);
		index = ipnum4;

		new = malloc(sizeof(struct st_rate));
		if (new == NULL)
		{
		    return;
		}
		memset(new, 0, sizeof(struct st_rate));
		strcpy(new->ipstr, UserIp);
		new->inbit = (strtoul(inbit, 0, 10) * 8U/1024U);
		new->outbit = (strtoul(outbit, 0, 10) * 8U/1024U);
		new->next = NULL;

		if (*(ipHashTab + index) != NULL)
		{
		    tmp = *(ipHashTab + index);
		    while(tmp->next != NULL)
		    {
			tmp = tmp->next;
		    }			
		    tmp->next = new;
		}
		else
		{
		    *(ipHashTab + index) = new;
		}
	    }
	}
	fclose(fp);    
    }
}

/*销毁hash数组*/
static void destroyHashTab(void)
{
    Rate tmp;
    Rate next;
    int index = 0;
    for (index = 0; index < 257; index++)
    {
	tmp = *(ipHashTab + index);
	while (tmp != NULL)
	{
	    next = tmp->next;
	    free(tmp);
	    tmp = next;
	}

    }
    free(ipHashTab);
}

/*在hash数组中查找对应ip的速率，找不到输出0*/
static int getRateInHashTab(webs_t wp, char_t *ip, int index)
{
    int ipnum1, ipnum2, ipnum3, ipnum4;
    int i;
    int inbit = 0, outbit = 0;
    Rate tmp;
    
    sscanf(ip, "%d.%d.%d.%d", &ipnum1, &ipnum2, &ipnum3, &ipnum4);
    i = ipnum4;
    tmp = *(ipHashTab + i);
    while (tmp != NULL)
    {
	if (!strcmp(tmp->ipstr, ip))
	{
	    inbit = tmp->inbit;
	    outbit = tmp->outbit; 
	    break;
	}
	tmp = tmp->next;
    }

    websWrite(wp, "inbits[%d]=\"%d\";\n", index, inbit);
    websWrite(wp, "outbits[%d]=\"%d\";\n", index, outbit);

    return 0;
}
#endif
static  int MaskOutsideIP(FILE *fp,char *IPs)
{
    char buffer[64]= {0};

    Uint64 offset_F=0;
    Uint64 buffer_int = 0U;
    IpPoolEnum poolType=IP_POOL_NONE;
    char if_addr[16] = {0};
    char if_nm[16] = {0};
    Uint32 lanIp,lanMask,ip_addr;
    Uint64 Upload,Download;
    offset_F=ftell(fp);
    fscanf(fp, "%s", buffer);
    if(fscanf(fp, "%llu", &buffer_int)==1)
    {
    Upload=(Uint64)(buffer_int);
    }
    if(fscanf(fp, "%llu", &buffer_int)==1)
    {
    Download=(Uint64)(buffer_int);
    }
    ip_addr=ip2int(IPs);
    if((getIfIp(getLanIfName(),if_addr)==0 )&&(getIfNetmask(getLanIfName(),if_nm)==0))
    {
    lanIp = ip2int(if_addr);
    lanMask = ip2int(if_nm);
    }

    fseek(fp,offset_F,SEEK_SET);

    if(((lanIp&lanMask)!=(ip_addr&lanMask))
#if (DHCP_SERVER == FYES)
    && (ipPoolConflict(poolType,htonl(ip_addr),htonl(ip_addr)!=IP_POOL_DHCP))
#endif   
    && Upload<10000 && Download<10000   
      )
      {
        return 1;
      }
    return 0;
}
/********************************************************************
 * 函数： formRateForUserLink
 * 功能： 输出用户所有状态
 * 创建： 2012-06-11
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void formRateForUserLink(webs_t wp, char_t *path, char_t *query)
{
#if 1
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
#endif
    char_t buffer[20];
    char_t linestr[MAX_LINE_LEN];
    char_t ipAddr[20];
    FILE *fp;
    char_t inbit[20];
    char_t outbit[20];
    int i = 0, pppoeWrite = 0;
#if (WEB_AUTH == FYES)
    int WebAuthflag = 0;
#endif
    int mac_not_zero = 0;
    Uint64 buffer_int = 0U;
    char_t rxBytes[64], txBytes[64];
    char IPs[20];
#if (ARP_BINDS == FYES)
    char *arp_mac = NULL;
    char *arp_bind_name = NULL;
#endif
    unsigned int perIpStats[UTT_APP_MAX_ARR - 1] = {0};
    int j;
    int lvl = 3; /* 输出影响工作级别 */
    const char *str1=NULL, *str2=NULL, *str3=NULL, *str4=NULL, *str5=NULL, *str6=NULL, *str7=NULL, *str8=NULL, *str9=NULL, *str10=NULL, *str11=NULL;

    memset(buffer,0,sizeof(buffer));
    memset(ipAddr,0,sizeof(ipAddr));
    memset(rxBytes,0,sizeof(rxBytes));
    memset(txBytes,0,sizeof(txBytes));
    memset(IPs,0,sizeof(IPs));
#if 1
    /*get SystemProfile Pointer*/
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibTypeSys, 0);
    /* get UttNfIndentEn value 
     * write value to web
     */
#endif
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain;charset=gb2312\nPragma: no-cache\nCache-Control: no-cache\n\n"));
    if(profSys->UttNfIndentEn == FUN_ENABLE)
    {
	OutputPie(wp);
    }
    else
    {
	str1 = getTransDataForC(C_LANG_INDEX_SW_TALK);
	str2 = getTransDataForC(C_LANG_INDEX_SW_P2P);
	str3 = getTransDataForC(C_LANG_INDEX_SW_STOCK);
	str4 = getTransDataForC(C_LANG_INDEX_SW_NET_VIDEO);
	str5 = getTransDataForC(C_LANG_INDEX_SW_NET_GAME);
	str6 = getTransDataForC(C_LANG_INDEX_SW_SHOPPING_SITE);
	str7 = getTransDataForC(C_LANG_INDEX_SW_SOCIAL_NET_SITE);
	str8 = getTransDataForC(C_LANG_INDEX_SW_WEB_GAME);
	str9 = getTransDataForC(C_LANG_INDEX_SW_MAIL);
	str10 = getTransDataForC(C_LANG_INDEX_SW_FORUM);
	str11 = getTransDataForC(C_LANG_INDEX_SW_OTHERS);
	websWrite(wp, "pieFlowValues = {\"%s\":0,\"%s\":0,\"%s\":0,\"%s\":0,\"%s\":0,\"%s\":0,\"%s\":0,\"%s\":0,\"%s\":0,\"%s\":0,\"%s\":0};\n",str1,str2,str3,str4,str5,str6,str7,str8, str9,str10,str11);
	websWrite(wp, "pieActValues = {\"%s\":0,\"%s\":0,\"%s\":0,\"%s\":0,\"%s\":0,\"%s\":0,\"%s\":0,\"%s\":0,\"%s\":0,\"%s\":0};\n",str1,str2,str3,str4,str5,str6,str7,str8,str9,str10);
    }
    fp = fopen(USER_MAC_PROC_FILE, "r");
    if(fp != NULL)
    {
	memset(linestr, 0, sizeof(linestr));
	fgets(linestr, MAX_LINE_LEN, fp);
#if READ_RATE_BY_HASH
	initHashTab();
#endif
	while (feof(fp) == 0)
	{
	    mac_not_zero = 0;
#if (ARP_BINDS == FYES)
	    /*mantis:11768*/
	    arp_mac = NULL;
#endif
	    if (fscanf(fp, "%s", buffer) == 1)/*IP*/
	    {
		strcpy(IPs, buffer);
		strcpy(ipAddr, buffer);
	    } else { /* IP没有读出来, continue. feof引起问题 */
		continue;
	    }

#if (PPPOE_SERVER == FYES)
	    pppoeWrite = OutputPPPoE(wp, ipAddr, i);
#endif
	    if (fscanf(fp, "%s", buffer) == 1)/*Mac*/
	    {
		if (pppoeWrite == 0) { /* 未输出PPPoE客户端Mac */
#if (ARP_BINDS == FYES)
		    if ((strcmp(buffer, "00:00:00:00:00:00") == 0) &&
			    ((arp_mac = getMacByIpFromSysArpTb(ipAddr)) != NULL)) {
			/*
			 * 如果MAC是00:00:00:00:00:00表示内核没有读取MAC
			 * 需要在用户空间通过ARP读取
			 */
			if(strcmp(arp_mac, "00:00:00:00:00:00") != 0)
			{
			    mac_not_zero = 1;
			    websWrite(wp, "Macs[%d]=\"%s\";\n", i, arp_mac);
			}
		    } 
		    else 
#endif
		    {
			if(strcmp(buffer, "00:00:00:00:00:00") != 0)
			{
			    mac_not_zero = 1;
			    websWrite(wp, "Macs[%d]=\"%s\";\n", i, buffer);
			}
		    }
		}
		else 
		{ /* pppoe接入认为mac地址全零合法 */
		    mac_not_zero = 1;
		}
	    }
	    if(mac_not_zero == 0) /* 跳过mac地址全零的固定接入 */
	    {
		/* 跳过此行 */
		fgets(linestr, MAX_LINE_LEN, fp);
		continue;
	    }
		websWrite(wp, "IPs[%d]=\"%s\";\n", i, IPs);
	    if(strcmp(IPs, "0.0.8.6") == 0) /* IP为0.0.8.6的过滤 */
	    {
		/* 跳过此行 */
		fgets(linestr, MAX_LINE_LEN, fp);
		continue;
	    }
#if 1
           if(MaskOutsideIP(fp,IPs)==1)
	    {
		fgets(linestr, MAX_LINE_LEN, fp);
		continue;
	    }
#endif
#if READ_RATE_BY_HASH
		getRateInHashTab(wp, ipAddr, i);
#else
#if 0
		OutputTimeFlow(wp, ipAddr, i);/*上下行速率*/
#endif
#endif
#if 0
		OutputIpGroup(wp, ipAddr, i);/*用户所属组*/
#endif
#if (WEB_AUTH == FYES)
		WebAuthflag = OutputWebAuth(wp, ipAddr, i);/*用户类型和用户名*/
#endif
		if((pppoeWrite == 0 ) 
#if (WEB_AUTH == FYES)
			&& (WebAuthflag == 0)
#endif
			)
		{
#if (ARP_BINDS == FYES)
		if (((arp_bind_name = getNameByIPMac(IPs, arp_mac)) != NULL) || ((arp_bind_name = getNameByIPMac(IPs, buffer)) != NULL))
		{
		    websWrite(wp, "UserNames[%d]=\"%s\";\n", i, arp_bind_name);
		}
		else
#endif
		{
		    websWrite(wp, "UserNames[%d]=\"\";\n", i);
		}
		    websWrite(wp, "AuthTypes[%d]=\"\";\n", i);
		    websWrite(wp, "remarks[%d]=\"\";\n", i);
#if (IP_GRP == FYES)
		    OutputIpGroup(wp, ipAddr, i, IPENT_RANGE);
#endif
		}
	    if (fscanf(fp, "%s", buffer) == 1)/*上网时间*/
	    {
		websWrite(wp, "startTimes[%d]=\"%ld\";\n", i, (getUptime() - (time_t)strtoul(buffer, 0 ,10)));
	    }
	    if (fscanf(fp, "%llu", &buffer_int) == 1)/*总上行流量*/
	    {
		sprintf(rxBytes, "%llu", (Uint64)(buffer_int >> 17U));
		websWrite(wp, "rxBytes[%d]=\"%s\";\n", i, rxBytes);
	    }
	    if (fscanf(fp, "%llu", &buffer_int) == 1)/*总下行流量*/
	    {
		sprintf(txBytes, "%llu", (Uint64)(buffer_int >> 17U));
		websWrite(wp, "txBytes[%d]=\"%s\";\n", i, txBytes);
	    }

	    if(profSys->UttNfIndentEn == FUN_ENABLE)
	    {
		/* 单个ip统计信息 */
		websWrite(wp, "perIpActs[%d] = new Array(", i);
		for (j = 0; j < UTT_APP_MAX_ARR - 1; j++) {
		    fscanf(fp, "%u", &perIpStats[j]);
		    if (j == UTT_APP_MAX_ARR - 2) {
			websWrite(wp, "%u", perIpStats[j]);
		    } else {
			websWrite(wp, "%u,", perIpStats[j]);
		    }
		}
		websWrite(wp, ");\n");

		/* 输出影响工作级别 */
		lvl = userBvLevel(perIpStats, UTT_APP_MAX_ARR - 1);
		websWrite(wp, "perIpLevels[%d]=\"level%d\";\n", i, lvl);
	    }
	    else
	    {
		for (j = 0; j < UTT_APP_MAX_ARR - 1; j++) {
		    fscanf(fp, "%u", &perIpStats[j]);
		}
		websWrite(wp, "perIpActs[%d] = new Array(0,0,0,0,0,0,0,0,0,0);\n", i);
		websWrite(wp, "perIpLevels[%d]=\"level0\";\n", i);
	    }
	    if(fscanf(fp,"%s",outbit))
	    {    
		websWrite(wp, "outbits[%d]=\"%d\";\n", i, (strtoul(outbit, 0, 10) * 8U/1024U));
	    }    
	    else 
	    {    
		websWrite(wp, "outits[%d]=\"0\";\n", i);
	    }    

	    if(fscanf(fp,"%s",inbit))
	    {    
		websWrite(wp, "inbits[%d]=\"%d\";\n", i, (strtoul(inbit, 0, 10)* 8U/1024U));
	    }    
	    else 
	    {    
		websWrite(wp, "inbits[%d]=\"0\";\n", i);
	    }  

	    i++;
	}
#if READ_RATE_BY_HASH
	destroyHashTab();
#endif
	fclose(fp);    
    }
    websWrite(wp, "var totalrecs=%d;", i);
#if 0
    }
#endif
    websDone(wp, 200);
}

/********************************************************************
 * 函数： OutputTimeFlow
 * 功能： 输出用户上下行速率
 * 创建： 2012-06-11
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int OutputTimeFlow(webs_t wp, char_t *ip, int index)
{
    FILE *fp;
    char_t linestr[MAX_LINE_LEN];
    char_t UserIp[20];
    char_t inbit[20];
    char_t outbit[20];
    int noRead = 1;
    memset(inbit,0,sizeof(inbit));
    memset(outbit,0,sizeof(outbit));
    fp = fopen(USER_RATE_PROC_FILE, "r");
    if(fp != NULL)
    {
	while (feof(fp) == 0)
	{
	    memset(linestr, 0, sizeof(linestr));
	    if (fgets(linestr, MAX_LINE_LEN, fp) != NULL)
	    {
		sscanf(linestr, "%s%s%s", UserIp, inbit, outbit);
		if(strcmp(UserIp, ip) == 0)
		{
		    websWrite(wp, "inbits[%d]=\"%d\";\n", index, (strtoul(inbit, 0, 10) * 8U/1024U));
		    websWrite(wp, "outbits[%d]=\"%d\";\n", index, (strtoul(outbit, 0, 10)* 8U/1024U));
		    noRead = 0;

		}
	    }
	}
	fclose(fp);    
    }
    /* 如果速率没读到，显示0 */
    if (noRead == 1) {
	websWrite(wp, "inbits[%d]=\"0\";\n", index);
        websWrite(wp, "outbits[%d]=\"0\";\n", index);
    }
    return 0;
}

#if 0
/********************************************************************
 * 函数： OutputUserAndAuth
 * 功能： 输出用户类型和用户名
 * 创建： 2012-06-11
 * 参数： 略
 * 返回： 1 - PPPoE用户; 0 - 非PPPoE用户
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int OutputUserAndAuth(webs_t wp, char_t *ip, int index)
{
    int PPPoEflag = 0;
#if (WEB_AUTH == FYES)
    int WebAuthflag = 0;
#endif

#if (PPPOE_SERVER == FYES)
    PPPoEflag = OutputPPPoE(wp, ip, index);
#endif
#if (WEB_AUTH == FYES)
    WebAuthflag = OutputWebAuth(wp, ip, index);
#endif
    if((PPPoEflag == 0) 
#if (WEB_AUTH == FYES)
	    && (WebAuthflag == 0)
#endif
	    )
    {
	websWrite(wp, "UserNames[%d]=\"\";\n", index);
	websWrite(wp, "AuthTypes[%d]=\"\";\n", index);
	websWrite(wp, "remarks[%d]=\"\";\n", index);
    } 
#if (PPPOE_SERVER == FYES)
    else if (PPPoEflag != 0) {
	return 1;
    }
#endif

    return 0;
}
#endif
extern char utt_tolower(char x);
static void utt_tolower_str(char *buf, char *str, int len) {
    int i;
    for (i = 0; i < len; i++) {
	if (*str != '\0') {
	    *buf++ = utt_tolower(*str++);
	} else {
	    break;
	}
    }
}

#if (PPPOE_SERVER == FYES)
#define MAC_BUF_LEN 20
#if 0
/********************************************************************
 * 函数： OutputPPPoE
 * 功能： 输出PPPoE用户类型和用户名和MAC地址
 * 创建： 2012-06-11
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int OutputPPPoE(webs_t wp, char_t *ip, int index)
{
    MibProfileType mibTypePPPoE = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *profPPPoE = NULL;
    int flag = 0;
    int min = 0, max = 0;
    int i = 0;
    char buf[MAC_BUF_LEN + 1] = {0};
    ProfInstLowHigh(mibTypePPPoE, &max, &min);
    for (i = min; i < max; i++) 
    {
	profPPPoE = (PPPoESrvAccProfile*)ProfGetInstPointByIndex(mibTypePPPoE, i);
	if((profPPPoE != NULL) && (ProfInstIsFree(profPPPoE) == 0) 
		&& (profPPPoE->head.active == TRUE))
	{
 	    if((profPPPoE->ConTime == 0) && (strcmp(profPPPoE->ip, ip) == 0))
	    {
		memset(buf, 0, MAC_BUF_LEN);
#if 0
		if(strcmp(profPPPoE->mac, "00:00:00:00:00:00") != 0)
		{
#endif
		flag = 1;
		websWrite(wp, "UserNames[%d]=\"%s\";\n", index, profPPPoE->pppoename);
		websWrite(wp, "AuthTypes[%d]=\"PPPoE\";\n", index);
		websWrite(wp, "remarks[%d]=\"%s\";\n", index, profPPPoE->remark);
		utt_tolower_str(buf, profPPPoE->mac, MAC_BUF_LEN);
		websWrite(wp, "Macs[%d]=\"%s\";\n", index, buf);
#if (IP_GRP == FYES)
		OutputIpGroup(wp, profPPPoE->pppoename, index, IPENT_POE_ACC);
#endif
#if 0
		}
#endif
	    }
	}
    }
    return flag;
}
#else
/********************************************************************
 * 函数： OutputPPPoE
 * 功能： 输出PPPoE用户类型和用户名和MAC地址
 * 创建： 2012-08-20
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int OutputPPPoE(webs_t wp, char_t *ip, int index)
{
    MibProfileType mibType = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *prof = NULL;

    MibProfileType mibTypePPPoE = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *profPPPoE = NULL;

    int flag = 0;
    int i = 0;
    char buf[MAC_BUF_LEN + 1] = {0};
    struct in_addr pppoeIp;

    /*
     * 此处为加上信号量锁定此操作
     */
    uttSemP(SEM_PPPOE_NO,0);

    prof = (PPPoESrvGloProfile*)ProfGetInstPointByIndex(mibType, 0);

    if(prof != NULL)
    {
	inet_aton(ip, &pppoeIp);
	i = ntohl(pppoeIp.s_addr) - ntohl(prof->ipStart);
	SWORD_PRINTF("%s-----%d, i = %d\n", __func__, __LINE__, i);
	if(SesIndexIsOk(i) == 1)
	{
	    if(nvramProfile->Session[i].ConTime == 0U)
	    {
		profPPPoE = (PPPoESrvAccProfile*)ProfGetInstPointByIndex(mibTypePPPoE, nvramProfile->Session[i].accIndex);
		flag = 1;
		websWrite(wp, "UserNames[%d]=\"%s\";\n", index, profPPPoE->pppoename);
		websWrite(wp, "AuthTypes[%d]=\"PPPoE\";\n", index);
		websWrite(wp, "remarks[%d]=\"%s\";\n", index, profPPPoE->remark);
		utt_tolower_str(buf, nvramProfile->Session[i].mac, MAC_BUF_LEN);
		websWrite(wp, "Macs[%d]=\"%s\";\n", index, buf);
#if (IP_GRP == FYES)
		OutputIpGroup(wp, profPPPoE->pppoename, index, IPENT_POE_ACC);
#endif
	    }
	}
    }
    /*
     *此处为发信号解锁
     */
    uttSemV(SEM_PPPOE_NO,0);
    return flag;
}
#endif
#endif

#if (WEB_AUTH == FYES)
#if (WEB_AUTH_SHARE == FYES)
extern int searchSessionByIp(WebAuthProfile *prof,Uint32 ip);
#endif
/********************************************************************
 * 函数： _OutputWebAuth
 * 功能： 输出WebAuth类型和用户名
 * 创建： 2014-01-26
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int _OutputWebAuth(webs_t wp, char_t *ip, int index, MibProfileType mibTypeWeb)
{
    int flag = 0;
    WebAuthProfile *profWeb = NULL;
    int min = 0, max = 0;
    int i = 0;
    struct in_addr addr;

    ProfInstLowHigh(mibTypeWeb, &max, &min);
    for (i = min; i < max; i++) 
    {
	profWeb = (WebAuthProfile*)ProfGetInstPointByIndex(mibTypeWeb, i);
	if((profWeb != NULL) && (ProfInstIsFree(profWeb) == 0) 
		&& (profWeb->head.active == TRUE))
	{
	    inet_aton(ip, &addr);
#if (WEB_AUTH_SHARE == FYES)
	    if(searchSessionByIp(profWeb, addr.s_addr) == 1)
#else
	    if(profWeb->ip == addr.s_addr)
#endif
	    {
		flag = 1;
		websWrite(wp, "UserNames[%d]=\"%s\";\n", index, profWeb->user);
		websWrite(wp, "AuthTypes[%d]=\"WEB\";\n", index);
		websWrite(wp, "remarks[%d]=\"%s\";\n", index, profWeb->remark);
#if (IP_GRP == FYES)
		OutputIpGroup(wp, profWeb->user, index, IPENT_WEB_AUTH);
#endif
	    }
	}
    }
    return flag;
}
/********************************************************************
 * 函数： OutputWebAuth
 * 功能： 输出WebAuth类型和用户名
 * 创建： 2014-01-26
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： 李文昌
 ********************************************************************/ 
static int OutputWebAuth(webs_t wp, char_t *ip, int index)
{
    int flag = 0;
    MibProfileType mibTypeWeb = MIB_PROF_WEB_AUTH;

    flag = _OutputWebAuth(wp, ip, index, mibTypeWeb);

#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
    if (flag == 0) 
    {
	mibTypeWeb = MIB_PROF_WEB_AUTH_CPT;
	flag = _OutputWebAuth(wp, ip, index, mibTypeWeb);
    }
#endif

    return flag;
}
#endif
#if 0
/********************************************************************
 * 函数： OutputIpGroup
 * 功能： 输出用户所属组
 * 创建： 2012-06-11
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int OutputIpGroup(webs_t wp, char_t *ip, int index)
{
#if (IP_GRP == FYES)
    MibProfileType mibType = MIB_PROF_IPGRP;
    IpGrpProfile* prof = NULL;
    int i = 0, min = 0, max = 0, flag = 0;
#endif
#if 0
    char_t group[3200];

    memset(group, 0, sizeof(group));
#endif
#if (IP_GRP == FYES)
    ProfInstLowHigh(mibType, &max, &min);
    /*遍历整个profile*/
    websWrite(wp, "groupContents[%d]=\"", index);
    for(i = min; i < max; i++) 
    {
	prof = (IpGrpProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
	{
	    if(ipsetTestIpStr(prof->head.name, ip) == 1)
	    {
		flag = 1;
#if 0
		strcat(group,prof->head.name);
		strcat(group," ");
#endif
		websWrite(wp, "%s ", prof->head.name);
	    }
	}
    }
    websWrite(wp, "\";\n");
#else
#if 0
    websWrite(wp, "groupContents[%d]=\"%s\";\n", index, group);
#endif
    websWrite(wp, "groupContents[%d]=\"\";\n", index);
#endif

#if 0
    if(flag == 0)
    {
	websWrite(wp, "groupContents[%d]=\"\";\n", index);
    }
#endif
    return 0;
}
#else
#if (IP_GRP == FYES)
/********************************************************************
 * 函数： OutputIpGroup
 * 功能： 输出用户所属组
 * 创建： 2012-06-11
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int OutputIpGroup(webs_t wp, char_t *UserName, int index, ipGrpEntryType userType)
{
#if (IP_GRP == FYES)
    MibProfileType mibType = MIB_PROF_IPGRP;
    IpGrpProfile* prof = NULL;
    int i = 0, min = 0, max = 0, flag = 0;
#endif
#if (IP_GRP == FYES)
    ProfInstLowHigh(mibType, &max, &min);
    /*遍历整个profile*/
    websWrite(wp, "groupContents[%d]=\"", index);
    for(i = min; i < max; i++) 
    {
	prof = (IpGrpProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
	{
	    if(ipGrpSearchIp(prof, UserName, userType) == 1)
	    {
		flag = 1;
		websWrite(wp, "%s ", prof->head.name);
	    }
	}
    }
    websWrite(wp, "\";\n");
#else
    websWrite(wp, "groupContents[%d]=\"\";\n", index);
#endif

    return 0;
}
#endif

#if (IP_GRP == FYES)
/********************************************************************
 * 函数： ipGrpSearchIp
 * 功能： 遍历组判断用户名或ip地址是否在组中
 * 创建： 2012-07-10
 * 参数： 
 *	curProf	    : 所查找组的prof
 *	userName    : 要查找的用户名
 *	userType    : 用户类型 
 *
 * 返回：
 *   -1 ip组中不含有该用户
 *    1 ip组中含有该用户
 * 输出： 略
 * 作者： Jianqing.Sun 
 *******************************************************************/ 
static int ipGrpSearchIp(IpGrpProfile *curProf, char *userName, ipGrpEntryType userType)
{
    int flag = 0;
    int i = 0, maxEntrys = 0;
#if 0
    struct in_addr userIp;
#endif
    IpGrpProfile* subProf = NULL;
    MibProfileType mibType = MIB_PROF_IPGRP;

    if(curProf->grpType == IPGRP_IP)
    {
	maxEntrys =  IPGRP_IP_ENTRYS;
    }
    else
    {
	maxEntrys = IPGRP_USER_ENTRYS;
    }
    for(i=0; i< maxEntrys; i++) 
    {/*遍历所有成员*/
	if(curProf->ipEntry[i].entryType == userType)
	{	
	    flag = userNameIsIn(curProf, i, userName, userType);
	    SWORD_PRINTF("%s-----%d, userName = %s, flag = %d\n", __func__, __LINE__, userName, flag);
#if 0
	    if(userType == IPENT_RANGE)
	    {
		inet_aton(userName, &userIp);
		if((userIp.s_addr > curProf->ipEntry[i].entryVal.range.ipFrom) && (userIp.s_addr < curProf->ipEntry[i].entryVal.range.ipTo))
		{
		    flag = 1;/*找到*/
		}
	    }
	    else
	    {
		if(strcmp(userName, curProf->ipEntry[i].entryVal.name) == 0)
		{
		    flag = 1;/*找到*/
		}
	    }
#endif
	}
	else 
	{ 
	    if(curProf->ipEntry[i].entryType == IPENT_GRP)
	    {/*在子组中查找*/
		subProf = ProfGetInstPointByName(mibType, curProf->ipEntry[i].entryVal.name);
		if((subProf != NULL) && (ProfInstIsFree(subProf) == 0) && (subProf->head.active == TRUE))
		{
		    flag = ipGrpSearchIp(subProf, userName, userType); 
		}
	    } 
	}
	if(flag == 1)
	{/*找到则退出 */
	    break;
	}
    }

    return flag;
}
/********************************************************************
 * 函数： userNameIsIn
 * 功能： 判断用户名或ip地址是否在组中
 * 创建： 2012-07-10
 * 参数： 
 *	curProf	    : 所查找组的prof
 *       index      : 组的条目号 
 *	userName    : 要查找的用户名
 *	userType    : 用户类型 
 *
 * 返回：
 *   -1 ip组中不含有该用户
 *    1 ip组中含有该用户
 * 输出： 略
 * 作者： Jianqing.Sun 
 *******************************************************************/ 
static int userNameIsIn(IpGrpProfile *curProf, int i, char *userName, ipGrpEntryType userType)
{
    int flag = 0;
    struct in_addr userIp;
    if(userType == IPENT_RANGE)
    {
	inet_aton(userName, &userIp);
	if((ntohl(userIp.s_addr) >= ntohl(curProf->ipEntry[i].entryVal.range.ipFrom)) && (ntohl(userIp.s_addr) <= ntohl(curProf->ipEntry[i].entryVal.range.ipTo)))
	{
	    flag = 1;/*找到*/
	    SWORD_PRINTF("%s-----%d, curProf->head.name = %s, userName = %s, userIp.s_addr = %x, curProf->ipEntry[i].entryVal.range.ipFrom = %x, curProf->ipEntry[i].entryVal.range.ipTo = %x\n",
		    __func__, __LINE__, curProf->head.name, userName, ntohl(userIp.s_addr), 
		    ntohl(curProf->ipEntry[i].entryVal.range.ipFrom), ntohl(curProf->ipEntry[i].entryVal.range.ipTo));
	}
    }
    else
    {
	if(strcmp(userName, curProf->ipEntry[i].entryVal.name) == 0)
	{
	    flag = 1;/*找到*/
	    SWORD_PRINTF("%s-----%d, userName = %s, flag = %d\n", __func__, __LINE__, userName, flag);
	}
    }
    return flag;
}
#endif
#endif

/*
 * 如果other流量超过总流量1/3，则调整other流量。目前调整到1/3
 */
static __u64 adjustOtherFlow(UttAppSta *stats) {
    __u64 totalExpOther = 0;
    __u64 ExpOther = 0U;
    __u64 totalOther = 0U;/*其它流量*/
#if 1
    __u64 count = 0U;
#endif
    int i;

    stats[UTT_APP_P2P_ARR].bytes += adjust_p2p;
    stats[UTT_APP_VIDEO_ARR].bytes += adjust_video;
    stats[UTT_APP_GAME_ARR].bytes += adjust_game;
    stats[UTT_APP_SHOP_ARR].bytes += adjust_shop;
    stats[UTT_APP_SNS_ARR].bytes += adjust_sns;
    stats[UTT_APP_WEBGAME_ARR].bytes += adjust_webgame;
    stats[UTT_APP_FORUM_ARR].bytes += adjust_forum;
    /* 除了other流量外，其余总流量 */
    for (i = 0; i < UTT_APP_MAX_ARR - 1; i++) {
	totalExpOther += stats[i].bytes;
    }

    SWORD_PRINTF("%s-%d, stats[UTT_APP_OTHER_ARR].bytes = %llu, adjust_other_drop = %llu\n", __func__, __LINE__, stats[UTT_APP_OTHER_ARR].bytes, adjust_other_drop);
    if(stats[UTT_APP_OTHER_ARR].bytes > adjust_other)
    {
	totalOther = stats[UTT_APP_OTHER_ARR].bytes - adjust_other;/*这次增加的其它流量*/
    }
    SWORD_PRINTF("%s-%d, totalOther = %llu\n", __func__, __LINE__, totalOther);
    adjust_other = stats[UTT_APP_OTHER_ARR].bytes;/*备份此次其它流量*/
    if(stats[UTT_APP_OTHER_ARR].bytes > adjust_other_drop)
    {
	stats[UTT_APP_OTHER_ARR].bytes -= adjust_other_drop;/*因每次其它最多只能33%，减去上次没有分配而丢掉的*/
    }
#if 0
    totalExp =  stats[UTT_APP_OTHER_ARR].bytes + totalExpOther;/*总的流量*/
#endif
#if 0
    if ((totalExpOther/2 != 0) && (stats[UTT_APP_OTHER_ARR].bytes > (totalExpOther/2))) { /* 如果other流量大于1/3 */
#endif
#if 0
	return totalExpOther/2;
#else
#if 1
	if((stats[UTT_APP_P2P_ARR].bytes - adjust_p2p_backup) != 0U)
	{
	    count += 5U;
	}
	if((stats[UTT_APP_VIDEO_ARR].bytes - adjust_video_backup) != 0U)
	{
	    count += 9U;
	}
	if((stats[UTT_APP_GAME_ARR].bytes - adjust_game_backup) != 0U)
	{
	    count += 1U;
	}
	if((stats[UTT_APP_SHOP_ARR].bytes - adjust_shop_backup)!= 0U)
	{
	    count += 2U;
	}
	if((stats[UTT_APP_SNS_ARR].bytes - adjust_sns_backup) != 0U)
	{
	    count += 2U;
	}
	if((stats[UTT_APP_WEBGAME_ARR].bytes - adjust_webgame_backup) != 0U)
	{
	    count += 2U;
	}
	if((stats[UTT_APP_FORUM_ARR].bytes - adjust_forum_backup) != 0U)
	{
	    count += 2U;
	}
#endif
#if 0
	ExpOther = adjustOtherFlowPercent(totalExp);
#endif
#if 1
	SWORD_PRINTF("%s-%d, count = %llu\n", __func__, __LINE__, count);
	SWORD_PRINTF("%s-%d, %llu %llu %llu %llu %llu %llu %llu\n", __func__, __LINE__, stats[UTT_APP_P2P_ARR].bytes, stats[UTT_APP_VIDEO_ARR].bytes, stats[UTT_APP_GAME_ARR].bytes, stats[UTT_APP_SHOP_ARR].bytes, stats[UTT_APP_SNS_ARR].bytes, stats[UTT_APP_WEBGAME_ARR].bytes, stats[UTT_APP_FORUM_ARR].bytes);
	if(count != 0U)
	{
	    totalExpOther += totalOther;/*如果要分配增加的流量则总的流量要加上增加的流量*/
	}
	    ExpOther = adjustOtherFlowPercent(totalExpOther);
#if 0
	    adjust_other += ExpOther;
#endif
	    SWORD_PRINTF("%s-%d, totalOther = %llu\n", __func__, __LINE__, totalOther);
	    if((stats[UTT_APP_P2P_ARR].bytes - adjust_p2p_backup) != 0U)
	    {
		adjust_p2p_backup = stats[UTT_APP_P2P_ARR].bytes;
		adjust_p2p += totalOther * 5U / count;
		stats[UTT_APP_P2P_ARR].bytes += totalOther * 5U / count;
	    }
	    if((stats[UTT_APP_VIDEO_ARR].bytes - adjust_video_backup) != 0U)
	    {
		adjust_video_backup = stats[UTT_APP_VIDEO_ARR].bytes;
		adjust_video += totalOther * 9U / count;
		stats[UTT_APP_VIDEO_ARR].bytes += totalOther * 9U / count;
	    }
	    if((stats[UTT_APP_GAME_ARR].bytes - adjust_game_backup) != 0U)
	    {
		adjust_game_backup = stats[UTT_APP_GAME_ARR].bytes;
		adjust_game += totalOther * 1U / count;
		stats[UTT_APP_GAME_ARR].bytes += totalOther * 1U / count;
	    }
	    if((stats[UTT_APP_SHOP_ARR].bytes - adjust_shop_backup)!= 0U)
	    {
		adjust_shop_backup = stats[UTT_APP_SHOP_ARR].bytes;
		adjust_shop += totalOther * 2U / count;
		stats[UTT_APP_SHOP_ARR].bytes += totalOther * 2U / count;
	    }
	    if((stats[UTT_APP_SNS_ARR].bytes - adjust_sns_backup) != 0U)
	    {
		adjust_sns_backup = stats[UTT_APP_SNS_ARR].bytes;
		adjust_sns += totalOther * 2U / count;
		stats[UTT_APP_SNS_ARR].bytes += totalOther * 2U / count;
	    }
	    if((stats[UTT_APP_WEBGAME_ARR].bytes - adjust_webgame_backup) != 0U)
	    {
		adjust_webgame_backup = stats[UTT_APP_WEBGAME_ARR].bytes;
		adjust_webgame += totalOther * 2U / count;
		stats[UTT_APP_WEBGAME_ARR].bytes += totalOther * 2U / count;
	    }
	    if((stats[UTT_APP_FORUM_ARR].bytes - adjust_forum_backup) != 0U)
	    {
		adjust_forum_backup = stats[UTT_APP_FORUM_ARR].bytes;
		adjust_forum += totalOther * 2U / count;
		stats[UTT_APP_FORUM_ARR].bytes+= totalOther * 2U / count;
	    }
#if 0
	}
	else
	{
	    ExpOther = adjustOtherFlowPercent(totalExpOther * 5 / 4);
 	    adjust_other_drop += (stats[UTT_APP_OTHER_ARR].bytes - ExpOther); 
#if 0
	    adjust_other += ExpOther;
#endif
	}
#endif
	SWORD_PRINTF("%s-%d, ExpOther = %llu\n", __func__, __LINE__, ExpOther);
	SWORD_PRINTF("%s-%d, %llu %llu %llu %llu %llu %llu %llu\n", __func__, __LINE__, stats[UTT_APP_P2P_ARR].bytes, stats[UTT_APP_VIDEO_ARR].bytes, stats[UTT_APP_GAME_ARR].bytes, stats[UTT_APP_SHOP_ARR].bytes, stats[UTT_APP_SNS_ARR].bytes, stats[UTT_APP_WEBGAME_ARR].bytes, stats[UTT_APP_FORUM_ARR].bytes);
#endif
	return ExpOther;
#endif
#if 0
    } else {
	return stats[UTT_APP_OTHER_ARR].bytes;
    }
#endif
}

static __u64 adjustOtherFlowPercent(__u64 total) 
{
    __u64 adjust_other_data = 0U;
    adjust_other_data = (total * (20U + adjust_count + ((rand() * 10) / (RAND_MAX+1.0)))) / (80U - adjust_count);
    if((adjust_count != 10U) && (adjust_count_control == 1U))
    {
	if((rand() % 2) == 1)
	{
	    adjust_count ++;
	}
	if(adjust_count == 10)
	{
	    adjust_count_control = 0;
	}
    }
    else 
    {
	if((rand() % 2) == 1)
	{
	    adjust_count --;
	}
	if(adjust_count == 0)
	{
	    adjust_count_control = 1;
	}
    }
    return adjust_other_data;
}

char pieFlowStr[300];
char pieActwStr[300];
int InitUserStatPieStr(void)
{
	const char *str1=NULL, *str2=NULL, *str3=NULL, *str4=NULL, *str5=NULL, *str6=NULL, *str7=NULL, *str8=NULL, *str9=NULL, *str10=NULL, *str11=NULL;
	str1 = getTransDataForC(C_LANG_INDEX_SW_TALK);
	str2 = getTransDataForC(C_LANG_INDEX_SW_P2P);
	str3 = getTransDataForC(C_LANG_INDEX_SW_STOCK);
	str4 = getTransDataForC(C_LANG_INDEX_SW_NET_VIDEO);
	str5 = getTransDataForC(C_LANG_INDEX_SW_NET_GAME);
	str6 = getTransDataForC(C_LANG_INDEX_SW_SHOPPING_SITE);
	str7 = getTransDataForC(C_LANG_INDEX_SW_SOCIAL_NET_SITE);
	str8 = getTransDataForC(C_LANG_INDEX_SW_WEB_GAME);
	str9 = getTransDataForC(C_LANG_INDEX_SW_MAIL);
	str10 = getTransDataForC(C_LANG_INDEX_SW_FORUM);
	str11 = getTransDataForC(C_LANG_INDEX_SW_OTHERS);
	sprintf(pieFlowStr, "{\"%s\":%%llu,\"%s\":%%llu,\"%s\":%%llu,\
\"%s\":%%llu,\"%s\":%%llu,\"%s\":%%llu,\"%s\":%%llu,\
\"%s\":%%llu,\"%s\":%%llu,\"%s\":%%llu,\"%s\":%%llu}", str1, str2, str3, str4, str5, str6, str7, str8, str9, str10, str11);
	sprintf(pieActwStr, "{\"%s\":%%u,\"%s\":%%u,\"%s\":%%u,\
\"%s\":%%u,\"%s\":%%u,\"%s\":%%u,\"%s\":%%u,\
\"%s\":%%u,\"%s\":%%u,\"%s\":%%u}", str1, str2, str3, str4, str5, str6, str7, str8, str9, str10);

	return 0;
}

/********************************************************************
 * 函数： OutputPie
 * 功能： 输出饼图所需数据
 * 创建： 2012-06-15
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 *******************************************************************/ 
static int OutputPie(webs_t wp)
{
    FILE *fp;
    int i;
    UttAppSta uttStats[UTT_APP_MAX_ARR];
    char_t buffer[30];
    char_t pieFlowValues[500], pieActValues[500];
    memset(buffer, 0, sizeof(buffer));
    memset(uttStats, 0, sizeof(uttStats));
    memset(pieFlowValues, 0, sizeof(pieFlowValues));
    memset(pieActValues, 0, sizeof(pieActValues));
    fp = fopen(USER_UTT_APPSTATUS_FILE, "r");
    if(fp != NULL)
    {
#if 0
	while (feof(fp) == 0)
	{
	    if (fscanf(fp, "%s", buffer) == 1)/*聊天软件*/
	    {
		strcat(pieFlowValues,"\"聊天软件\":");
		strcat(pieFlowValues, buffer);
	    }
	    if (fscanf(fp, "%s", buffer) == 1)/*聊天软件*/
	    {
		strcat(pieActValues,"\"聊天软件\":");
		strcat(pieActValues, buffer);
	    }
	    if (fscanf(fp, "%s", buffer) == 1)/*P2P软件*/
	    {
		strcat(pieFlowValues,",\"P2P软件\":");
		strcat(pieFlowValues, buffer);
	    }
	    if (fscanf(fp, "%s", buffer) == 1)/*P2P软件*/
	    {
		strcat(pieActValues,",\"P2P软件\":");
		strcat(pieActValues, buffer);
	    }
	    if (fscanf(fp, "%s", buffer) == 1)/*股票软件*/
	    {
		strcat(pieFlowValues,",\"股票软件\":");
		strcat(pieFlowValues, buffer);
	    }
	    if (fscanf(fp, "%s", buffer) == 1)/*股票软件*/
	    {
		strcat(pieActValues,",\"股票软件\":");
		strcat(pieActValues, buffer);
	    }
	    if (fscanf(fp, "%s", buffer) == 1)/*网络视频*/
	    {
		strcat(pieFlowValues,",\"网络视频\":");
		strcat(pieFlowValues, buffer);
	    }
	    if (fscanf(fp, "%s", buffer) == 1)/*网络视频*/
	    {
		strcat(pieActValues,",\"网络视频\":");
		strcat(pieActValues, buffer);
	    }
	    if (fscanf(fp, "%s", buffer) == 1)/*网络游戏*/
	    {
		strcat(pieFlowValues,",\"网络游戏\":");
		strcat(pieFlowValues, buffer);
	    }
	    if (fscanf(fp, "%s", buffer) == 1)/*网络游戏*/
	    {
		strcat(pieActValues,",\"网络游戏\":");
		strcat(pieActValues, buffer);
	    }
	    if (fscanf(fp, "%s", buffer) == 1)/*购物网站*/
	    {
		strcat(pieFlowValues,",\"购物网站\":");
		strcat(pieFlowValues, buffer);
	    }
	    if (fscanf(fp, "%s", buffer) == 1)/*购物网站*/
	    {
		strcat(pieActValues,",\"购物网站\":");
		strcat(pieActValues, buffer);
	    }
	    if (fscanf(fp, "%s", buffer) == 1)/*社交网站*/
	    {
		strcat(pieFlowValues,",\"社交网站\":");
		strcat(pieFlowValues, buffer);
	    }
	    if (fscanf(fp, "%s", buffer) == 1)/*社交网站*/
	    {
		strcat(pieActValues,",\"社交网站\":");
		strcat(pieActValues, buffer);
	    }
	    if (fscanf(fp, "%s", buffer) == 1)/*网页游戏*/
	    {
		strcat(pieFlowValues,",\"网页游戏\":");
		strcat(pieFlowValues, buffer);
	    }
	    if (fscanf(fp, "%s", buffer) == 1)/*网页游戏*/
	    {
		strcat(pieActValues,",\"网页游戏\":");
		strcat(pieActValues, buffer);
	    }
	    if (fscanf(fp, "%s", buffer) == 1)/*邮件*/
	    {
		strcat(pieFlowValues,",\"邮件\":");
		strcat(pieFlowValues, buffer);
	    }
	    if (fscanf(fp, "%s", buffer) == 1)/*邮件*/
	    {
		strcat(pieActValues,",\"邮件\":");
		strcat(pieActValues, buffer);
	    }
	    if (fscanf(fp, "%s", buffer) == 1)/*论坛*/
	    {
		strcat(pieFlowValues,",\"论坛\":");
		strcat(pieFlowValues, buffer);
	    }
	    if (fscanf(fp, "%s", buffer) == 1)/*论坛*/
	    {
		strcat(pieActValues,",\"论坛\":");
		strcat(pieActValues, buffer);
	    }
	    if (fscanf(fp, "%s", buffer) == 1)/*其它*/
	    {
		strcat(pieFlowValues,",\"其它\":");
		strcat(pieFlowValues, buffer);
	    }
	    fscanf(fp, "%s", buffer);
	}
#endif
	for (i = 0; i < UTT_APP_MAX_ARR; i++) {
	    fscanf(fp, "%llu", &(uttStats[i].bytes));
	    fscanf(fp, "%u", &(uttStats[i].conns));
	}
	fclose(fp);    
#if 0
	sprintf(pieFlowValues,"\{");
	sprintf(pieActValues,"\{");

	strcat(pieFlowValues,"}");
	strcat(pieActValues,"}");
	websWrite(wp, "pieFlowValues = %s;\n", pieFlowValues);
	websWrite(wp, "pieActValues = %s;\n", pieActValues);
#endif
    }
    /* 调整other流量 */
    uttStats[UTT_APP_OTHER_ARR].bytes = adjustOtherFlow(uttStats);
    /* 饼图流量 */
    sprintf(pieFlowValues, pieFlowStr, uttStats[UTT_APP_IM_ARR].bytes, uttStats[UTT_APP_P2P_ARR].bytes, uttStats[UTT_APP_STOCK_ARR].bytes, uttStats[UTT_APP_VIDEO_ARR].bytes, uttStats[UTT_APP_GAME_ARR].bytes, uttStats[UTT_APP_SHOP_ARR].bytes, uttStats[UTT_APP_SNS_ARR].bytes, uttStats[UTT_APP_WEBGAME_ARR].bytes, uttStats[UTT_APP_MAIL_ARR].bytes, uttStats[UTT_APP_FORUM_ARR].bytes, uttStats[UTT_APP_OTHER_ARR].bytes);

    /* 饼图行为统计 */
    sprintf(pieActValues, pieActwStr, uttStats[UTT_APP_IM_ARR].conns, uttStats[UTT_APP_P2P_ARR].conns, uttStats[UTT_APP_STOCK_ARR].conns, uttStats[UTT_APP_VIDEO_ARR].conns, uttStats[UTT_APP_GAME_ARR].conns, uttStats[UTT_APP_SHOP_ARR].conns, uttStats[UTT_APP_SNS_ARR].conns, uttStats[UTT_APP_WEBGAME_ARR].conns, uttStats[UTT_APP_MAIL_ARR].conns, uttStats[UTT_APP_FORUM_ARR].conns);

	//printf("\n\n\n%s\n\n\n", pieFlowValues);
    websWrite(wp, "pieFlowValues = %s;\n", pieFlowValues);
    websWrite(wp, "pieActValues = %s;\n", pieActValues);

    return 0;
}


/********************************************************************
 * 函数： formUserStatusGlobalConfig
 * 功能： 读取页面控件写入全局开关值
 * 创建： 2012-06-11
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void formUserStatusGlobalConfig(webs_t wp, char_t *path, char_t *query)
{
    char *UserStatusEnable = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    FunEnableEnum enable = FUN_DISABLE;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;

    /* get web value */
    UserStatusEnable = websGetVar(wp, "UserStatusEnable1", T(""));
    
    /* get SystemProfile */
    profAction = PROF_CHANGE_EDIT;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);
    ProfBackupByIndex(mibType, profAction, 0, &profhead);
    
    /* compare web value 
     * if value on
     * set UserStatus enable, or set disable
     */
    if(strcmp(UserStatusEnable, "on") == 0) {
        enable = FUN_ENABLE;
    }
#if 0
    if(profSys->UttNfIndentEn != enable)
    {
	profSys->UttNfIndentEn = enable;
    }
#else
    profSys->UttNfIndentEn = enable;
#endif

    /* update systemProfile
     * and write to flash
     */
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    websRedirect(wp, "UserLinkRate.asp");
    return;
}
/********************************************************************
 * 函数： formUserStatusRemark
 * 功能： 读取页面控件写入备注
 * 创建： 2012-06-25
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void formUserStatusRemark(webs_t wp, char_t *path, char_t *query)
{
    char_t *userName = NULL;
    char_t *authType = NULL;
    char_t *remark = NULL;

    /* get web value */
    userName = websGetVar(wp, "userName", T(""));
    authType = websGetVar(wp, "authType", T(""));
    remark = websGetVar(wp, "remark", T(""));
    
#if (PPPOE_SERVER == FYES)
    if(strcmp(authType, "PPPoE") == 0)
    {
	formPPPoERemark(userName, remark);
    } 
    else
#endif
#if (WEB_AUTH == FYES) 
    if(strcmp(authType, "WEB") == 0)
    {
	formWebAuthRemark(userName, remark);
    } 
    else
#endif
    {
	/*test*/
    }
    websRedirect(wp, "UserLinkRate.asp");
    return;
}
#if (PPPOE_SERVER == FYES)
/********************************************************************
 * 函数： formPPPoERemark
 * 功能： 读取页面控件写入备注
 * 创建： 2012-06-25
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void formPPPoERemark(char_t *userName, char_t *remark)
{
    MibProfileType mibTypePPPoE = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *profPPPoE = NULL;
    int min = 0, max = 0;
    int i = 0;
    ProfInstLowHigh(mibTypePPPoE, &max, &min);
    for (i = min; i < max; i++) 
    {
	profPPPoE = (PPPoESrvAccProfile*)ProfGetInstPointByIndex(mibTypePPPoE, i);
	if((profPPPoE != NULL) && (ProfInstIsFree(profPPPoE) == 0) 
		&& (profPPPoE->head.active == TRUE))
	{
	    if(strcmp(profPPPoE->pppoename, userName) == 0)
	    {
		strcpy(profPPPoE->remark, remark);
	    }
	}
    }
    nvramWriteCommit(); /*save*/
    return;
}
#endif

#if (WEB_AUTH == FYES)
/********************************************************************
 * 函数： _formWebAuthRemark
 * 功能： 读取页面控件写入备注
 * 创建： 2012-06-25
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void _formWebAuthRemark(char_t *userName, char_t *remark, MibProfileType mibTypeWeb)
{
    WebAuthProfile *profWeb = NULL;
    int min = 0, max = 0;
    int i = 0;

    ProfInstLowHigh(mibTypeWeb, &max, &min);
    for (i = min; i < max; i++) 
    {
	profWeb = (WebAuthProfile*)ProfGetInstPointByIndex(mibTypeWeb, i);
	if((profWeb != NULL) && (ProfInstIsFree(profWeb) == 0) 
		&& (profWeb->head.active == TRUE))
	{
	    if(strcmp(profWeb->user, userName) == 0)
	    {
		strcpy(profWeb->remark, remark);
	    }
	}
    }
    nvramWriteCommit(); /*save*/
    return;
}
/********************************************************************
 * 函数： formWebAuthRemark
 * 功能： 读取页面控件写入备注
 * 创建： 2014-01-26
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： 李文昌
 ********************************************************************/ 
static void formWebAuthRemark(char_t *userName, char_t *remark)
{
    MibProfileType mibTypeWeb = MIB_PROF_WEB_AUTH;

    _formWebAuthRemark(userName, remark, mibTypeWeb);

#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
    mibTypeWeb = MIB_PROF_WEB_AUTH_CPT;
    _formWebAuthRemark(userName, remark, mibTypeWeb);
#endif

    return;
}
#endif
/********************************************************************
 * 函数： UserStatusClearConfig
 * 功能： 清除数据
 * 创建： 2012-06-11
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void UserStatusClearConfig(webs_t wp, char_t *path, char_t *query)
{
#ifdef CONFIG_UTT_NETFILTER
    clearsUserStats();
#endif
    websRedirect(wp, "UserLinkRate.asp");
    return;
}
/********************************************************************
 * 函数： getUserStatusGlobal
 * 功能： 向页面输出全局开关值
 * 创建： 2012-06-11
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int getUserStatusGlobal(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    websWrite(wp, "var UserStatusEnable=\"\";");
	InitUserStatPieStr();
    /*get SystemProfile Pointer*/
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibTypeSys, 0);
    /* get UttNfIndentEn value 
     * write value to web
     */
    if(profSys->UttNfIndentEn == FUN_ENABLE)
    {
	websWrite(wp, "UserStatusEnable=\"%s\";", T("on"));
    }
    else
    {
	websWrite(wp, "UserStatusEnable=\"%s\";",T("off"));
    }
    if((profSys->UttNfIndentEn == FUN_ENABLE) && (enableBackUp == FUN_DISABLE))
    {
#ifdef CONFIG_UTT_NETFILTER
	clearsUserStats();
#endif
    }
    enableBackUp = profSys->UttNfIndentEn;
	getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

/*
 * 清空单个IP行为统计信息
 */
static void formClearIpBvStats(webs_t wp, char_t *path, char_t *query) {
#ifdef CONFIG_UTT_NETFILTER
    char *ipStr = NULL;
    struct in_addr ip;
    int ret = 0;
    UttNlNfStatsMsg nlMsg;
    const char *errMsg=NULL;

    ipStr = websGetVar(wp, "ipaddr", T(""));
    if ((ipStr == NULL) || (inet_aton(ipStr, &ip) == 0)) {
	errMsg = getTransDataForC(C_LANG_INDEX_OPERATION_FAIL);
	setTheErrorMsg(errMsg);
	goto out;
    }
    memset(&nlMsg, 0, sizeof(nlMsg));
    nlMsg.msgHead.nlType = UTT_NLNF_STATS;
    /*增加*/
    nlMsg.msgType = UTT_STATS_SEP_CLEAR;
    nlMsg.entryVal.ip = ip.s_addr;
    ret = uttNfNlSend(uttNfNlSock,(char*)&nlMsg, sizeof(nlMsg), 0 ,0 );/*发送消息给内核*/

out:
#endif
    websRedirect(wp, "UserLinkRate.asp");
    return;
}

/********************************************************************
 * 函数： formDefineUserLinkRate
 * 功能： 页面与交互转换函数
 * 创建： 2012-06-11
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
void formDefineUserLinkRate(void)
{
    //websAspDefine(T("outputUserStats"), aspOutputUserStats);
    websFormDefine(T("formRateForUserLink"), formRateForUserLink);
    websFormDefine(T("formUserStatusGlobalConfig"), formUserStatusGlobalConfig);
    websFormDefine(T("formUserStatusRemark"), formUserStatusRemark);
    websFormDefine(T("UserStatusClearConfig"), UserStatusClearConfig);
    websAspDefine(T("aspOutUserStatusGlobal"), getUserStatusGlobal);     
    websFormDefine(T("ClearIpBvStats"), formClearIpBvStats);
}

#ifdef CONFIG_UTT_NETFILTER
/**
 * 发消息让内核清空用户统计流量等
 */
static int clearsUserStats(void)
{
    int ret = 0;
    UttNlNfStatsMsg nlMsg;
    memset(&nlMsg, 0, sizeof(nlMsg));
    nlMsg.msgHead.nlType = UTT_NLNF_STATS;
    /*增加*/
    nlMsg.msgType = UTT_STATS_CLEAR;
    ret = uttNfNlSend(uttNfNlSock,(char*)&nlMsg, sizeof(nlMsg), 0 ,0 );/*发送消息给内核*/
    adjust_p2p = 0U;
    adjust_video = 0U;
    adjust_game = 0U;
    adjust_shop = 0U;
    adjust_sns = 0U;
    adjust_webgame = 0U;
    adjust_forum = 0U;
    adjust_other = 0U;
    adjust_p2p_backup = 0U;
    adjust_video_backup = 0U;
    adjust_game_backup = 0U;
    adjust_shop_backup = 0U;
    adjust_sns_backup = 0U;
    adjust_webgame_backup = 0U;
    adjust_forum_backup = 0U;
    adjust_other_drop = 0U;/*没有分配但是不显示的丢弃*/
    return ret;
}
#endif
#endif

#endif/*end of UTT_SMART_UI*/
