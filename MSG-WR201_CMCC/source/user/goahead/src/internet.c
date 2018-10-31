/* vi: set sw=4 ts=4 sts=4 fdm=marker: */
/*
 *	internet.c -- Internet Settings
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: internet.c,v 1.58.16.2 2016/01/07 10:06:25 wang.lei Exp $
 */

#include	<stdlib.h>
#include	<sys/ioctl.h>
#include	<net/if.h>
#include	<net/route.h>
#include    <string.h>
#include    <dirent.h>
#include    <net/if_arp.h>
#include    <netinet/in.h>
#include    <net/if.h>
#include	"internet.h"
#include	"webs.h"
#include	"utils.h"
#include 	"firewall.h"
#include	"management.h"
#include	"station.h"
#include	"wireless.h"
#include	"uttfunction.h"
#include    "utt_conf_define.h"
#include    "md5.h"
#include	"linux/autoconf.h"  //kernel config
#include	"config/autoconf.h" //user config
#include	"mib.h"
#include	"profacce.h"
#include	"ifmisc.h"
#include	"translate.h"
#include    "comExeFun.h"

#define     INDEX_RULEID    0 
#define     ROUTE_ADD         "add" 
#define     ROUTE_DEL         "del" 
struct portMap
{
      int m_PortMappingEnabled;
      long int m_PortMappingLeaseDuration;
      char m_RemoteHost[16];
      char m_ExternalPort[6];
      char m_InternalPort[6];
      char m_PortMappingProtocol[4];
      char m_InternalClient[16];
      char m_PortMappingDescription[50];

      int expirationEventId;
      long int expirationTime;

      struct portMap* next;
      struct portMap* prev;
}; 


#if (WIRELESS == FYES)
extern int get3GSlotIndex(void);
extern int getAPCliSlotIndex(void);
#endif

#if (FEATURE_PORT_STATISTICS==FYES)
extern void getPortStaticsForSvg(webs_t wp, char comma);
#endif
extern void initPolicyFile(void);
static int getGraphInfoForSvg(int eid, webs_t wp, int argc, char_t **argv);
static void formGraphShowForSvg(webs_t wp, char * path, char * query);

static int getDns(int eid, webs_t wp, int argc, char_t **argv);
#if 0
static int getIfIsUpWeb(int eid, webs_t wp, int argc, char_t **argv);
#endif
static int getLanIp(int eid, webs_t wp, int argc, char_t **argv);
#if 0
static int getLanMac(int eid, webs_t wp, int argc, char_t **argv);
#endif
static int getLanIfNameWeb(int eid, webs_t wp, int argc, char_t **argv);
static int getLanNetmask(int eid, webs_t wp, int argc, char_t **argv);
static int getWanIp(int eid, webs_t wp, int argc, char_t **argv);
static int getWanMac(int eid, webs_t wp, int argc, char_t **argv);
static int getWlanMac(int eid, webs_t wp, int argc, char_t **argv);
static int getWanIfNameWeb(int eid, webs_t wp, int argc, char_t **argv);
static int getWanNetmask(int eid, webs_t wp, int argc, char_t **argv);
static int getWanIfOption(int eid, webs_t wp, int argc, char_t **argv);
static int getWanIfCount(int eid, webs_t wp, int argc, char_t **argv);
#if (POLICY_ROUTE == FYES)
static int getWanConfigCount(int eid, webs_t wp, int argc, char_t **argv);
#endif
#if (USER_DEF_WAN_COUNT == FYES)
static int formWanIfCount(webs_t wp, char_t *path, char_t *query);
#endif
extern void ProfWiteToFlash(void);
extern void  makeFactoryProfiles(void);
//add by cxhu 2009-11-27
#if BRW
int getNetConfig(int eid, webs_t wp, int argc, char_t **argv);
#endif
#if 0
static int getWanGateway(int eid, webs_t wp, int argc, char_t **argv);
static void ConfigLANConfig(webs_t wp, char_t *path, char_t *query);
static int getLANConfig(int eid, webs_t wp, int argc, char_t **argv);
#endif
#if 0
static void formReleaseConnect(webs_t wp,char_t *path,char_t*query);
static void formReConnect(webs_t wp,char_t *path,char_t*query);
#endif
#if 0
static void form3GReConnect(webs_t wp,char_t *path,char_t*query);
#endif

static int outputTheErrorMsg(int eid, webs_t wp, int argc, char_t **argv);
static int outputTheRightMsg(int eid, webs_t wp, int argc, char_t **argv);
static int getIfNameList(int eid, webs_t wp, int argc, char_t **argv);
extern long int strtol(const char *nptr, char **endptr, int base);
extern char_t *websGetVar(webs_t wp, char_t *var, char_t *defaultGetValue);
extern void websRedirect(webs_t wp, char_t *url);
static int getWanLinkState(int eid, webs_t wp, int argc, char_t **argv);
static int getWanDns(int eid, webs_t wp, int argc, char_t **argv);
#if 0
static void formUpdateRoutePdb(webs_t wp, char_t *path, char_t *query);
#endif
static int getUpPdbFromWhichPage(int eid, webs_t wp, int argc, char_t **argv);
static int getLicenseNeedFlag(int eid, webs_t wp, int argc, char_t **argv);
#if 0
inline void zebraRestart(void);
void ripdRestart(void);
#endif
int init_dhcpserver(void);

#if (PARENTS_CONTROL == FYES) || (MAC_FILTER == FYES)
static int getArpFromArptable(int eid,webs_t wp,int argc,char_t **argv);
static int getCurrentIPMacInfo(int eid, webs_t wp, int argc, char_t **argv);
#endif
/***********************************************************************
 * 函 数 名：   formDefineInternet
 * 功能描述：   form,asp注册函数
 * 输入参数:    
 * 输出参数：	
 * 返回值：	    
 * 创建日期：	2010-3-16
 * 修改日期：
 * 作者：       王炳荣	
 * 附加说明：	无
***********************************************************************/
void formDefineInternet(void) {
#if 0
	websAspDefine(T("aspOutStaticRouteList"),getStaticRouteList);
	websFormDefine(T("formStaticRoute"),formStaticRoute);
#endif
	websAspDefine(T("aspOutgetIfNameList"),getIfNameList);
#if 0
	websFormDefine(T("formStaticRouteDelAll"),formStaticRouteListDelAll);
	websFormDefine(T("formStaticRouteDel"),formStaticRouteListDel);
	websAspDefine(T("aspOutStaticRouteOneInfo"),getStaticOneRouteInfo);
	websAspDefine(T("getDhcpCliList"), getDhcpCliList);
#endif
	websAspDefine(T("getDns"), getDns);
//	websAspDefine(T("getIfIsUpWeb"), getIfIsUpWeb);
	websAspDefine(T("getLanIp"), getLanIp);
#if 0
	websAspDefine(T("getLanMac"), getLanMac);
#endif
	websAspDefine(T("getLanIfNameWeb"), getLanIfNameWeb);
	websAspDefine(T("getLanNetmask"), getLanNetmask);
	websAspDefine(T("getWanIp"), getWanIp);
	websAspDefine(T("getWanMac"), getWanMac);
	websAspDefine(T("getWlanMac"), getWlanMac);
	websAspDefine(T("getWanIfNameWeb"), getWanIfNameWeb);
	websAspDefine(T("getWanNetmask"), getWanNetmask);
	websAspDefine(T("getWanIfOption"), getWanIfOption);
	websAspDefine(T("getWanIfCount"), getWanIfCount);
#if (POLICY_ROUTE == FYES)
	websAspDefine(T("getWanConfigCount"), getWanConfigCount);
#endif
#if( USER_DEF_WAN_COUNT == FYES )
	websFormDefine(T("WanIfCountConfig"), formWanIfCount);
#endif
#if 0
	websFormDefine(T("form3GReConnect"), form3GReConnect);
#endif
#if BRW
	websAspDefine(T("outputNetConfig"), getNetConfig);
#endif
	websAspDefine(T("outputTheErrorMsg"), outputTheErrorMsg);
	websAspDefine(T("outputTheRightMsg"), outputTheRightMsg);
	websAspDefine(T("getGraphInfoForSvg"), getGraphInfoForSvg);
	websFormDefine(T("formGraphShowForSvg"), formGraphShowForSvg);
	websAspDefine(T("getWanLinkState"), getWanLinkState);
	websAspDefine(T("getWanDns"), getWanDns);
#if 0
	websFormDefine(T("formUpISPRoute"), formUpdateRoutePdb);
#endif
	websAspDefine(T("outputUpPdbFromWhichPage"), getUpPdbFromWhichPage);
	websAspDefine(T("getLicenseNeedFlag"), getLicenseNeedFlag);

#if (PARENTS_CONTROL == FYES) || (MAC_FILTER == FYES)
	websAspDefine(T("getArpFromArptable"), getArpFromArptable);
	websAspDefine(T("getCurrentIPMacInfo"),getCurrentIPMacInfo);
#endif
}

static int outputTheErrorMsg(int eid, webs_t wp, int argc, char_t **argv){
	 getTheErrorMsg(eid, wp, argc, argv);  
       return 0;
}
static int outputTheRightMsg(int eid, webs_t wp, int argc, char_t **argv){
	 getTheRightMsg(eid, wp, argc, argv);  
       return 0;
}

char *wanReName(char * wanName)
{
    if(strcmp(wanName, "WAN3") ==0 ) {
        return "3G";
    } else if(strcmp(wanName, "WAN4") ==0 ) { 
        return "APClient";
    } else if(strcmp(wanName, "LAN0") ==0 ) { 
        return "LAN";
    }else {
        return wanName;
    }
}
     
/***********************************************************************
 * 函 数 名：   getWLanIfName
 * 功能描述：	取出wlan name
 * 输入参数:    
 * 输出参数：	
 * 返回值：     wlan name
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       胡传香
 * 附加说明：	无
***********************************************************************/
char* getWLanIfName(void)
{
	char *if_name = "ra0";
    return if_name;
}
 
/***********************************************************************
 * 函 数 名：   getDns
 * 功能描述：	根据type读写DNS
 * 输入参数:    eid.wp,argc,argv
 * 输出参数：	
 * 返回值：     0
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       胡传香
 * 附加说明：	无
***********************************************************************/
static int getDns(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	char buf[80] = {0}, ns_str[11], dns[16] = {0};
	int type, idx = 0, req = 0;

	if (ejArgs(argc, argv, T("%d"), &type) == 1) {
		if (1 == type)
			req = 1;
		else if (2 == type)
			req = 2;
		else
			return websWrite(wp, T(""));
	}

	fp = fopen("/etc/resolv.conf", "r");
	if (NULL == fp)
		return websWrite(wp, T(""));
	while (fgets(buf, sizeof(buf), fp) != NULL) {
		if (sscanf(buf, "%s%s", ns_str, dns) != 2)
			continue;
		idx++;
		if (idx == req) {
			websWrite(wp, T("%s"), dns);
			break;
		}
	}
	fclose(fp);
	return 0;
}

#if 0
/*
 * arguments: name - interface name (ex. eth0, rax ..etc)
 * description: write the existence of given interface,
 *              0 = ifc is down, 1 = ifc is up
 */
static int getIfIsUpWeb(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t *name;
	char up[2] = "1";

	if (ejArgs(argc, argv, T("%s"), &name) < 1) {
		websError(wp, 400, T("Insufficient args\n"));
		return -1;
	}
	up[0] = (getIfIsUp(name) == 1)? '1' : '0';
	return websWrite(wp, T("%s"), up);
}
#endif


/***********************************************************************
 * 函 数 名：   getLanIp
 * 功能描述：	取出lan口ip
 * 输入参数:    wid,wp,argc,argv
 * 输出参数：	
 * 返回值：     0
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       胡传香
 * 附加说明：	无
***********************************************************************/
static int getLanIp(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_addr[16];

	if (-1 == getIfIp(getLanIfName(), if_addr)) {
		//websError(wp, 500, T("getLanIp: calling getIfIp error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_addr);
}

#if 0
/***********************************************************************
 * 函 数 名：   getLanMac
 * 功能描述：	取出lan口mac
 * 输入参数:    wid,wp,argc,argv
 * 输出参数：	
 * 返回值：     0
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       胡传香
 * 附加说明：	无
***********************************************************************/
static int getLanMac(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_mac[18];

	if (-1 == getIfMac(getLanIfName(), if_mac)) {
		//websError(wp, 500, T("getLanIp: calling getIfMac error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_mac);
}
#endif

/***********************************************************************
 * 函 数 名：   getLanIfNameWeb
 * 功能描述：	取出lan口接口name
 * 输入参数:    wid,wp,argc,argv
 * 输出参数：	
 * 返回值：     0
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       胡传香
 * 附加说明：	无
***********************************************************************/
static int getLanIfNameWeb(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char *name = getLanIfName();

	if (ejArgs(argc, argv, T("%d"), &type) == 1) {
		if (1 == type) {
			return websWrite(wp, T("%s"), name);
		}
	}
	ejSetResult(eid, name);
	return 0;
}

/***********************************************************************
 * 函 数 名：   getLanNetmask
 * 功能描述：	取出lan口netmask
 * 输入参数:    wid,wp,argc,argv
 * 输出参数：	
 * 返回值：     0
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       胡传香
 * 附加说明：	无
***********************************************************************/
static int getLanNetmask(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_net[16];

	if (-1 == getIfNetmask(getLanIfName(), if_net)) {
		//websError(wp, 500, T("getLanNetmask: calling getIfNetmask error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_net);
}

/***********************************************************************
 * 函 数 名：   getWanIp
 * 功能描述：	取出wan口ip
 * 输入参数:    wid,wp,argc,argv
 * 输出参数：	
 * 返回值：     0
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       胡传香
 * 附加说明：	无
***********************************************************************/
static int getWanIp(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_addr[16];
    int type = 0;


	if (ejArgs(argc, argv, T("%d"), &type) == 1) {
        if ( type <= getWanSlotCount()) {
            if (0 == getIfIp(getWanIfNamePPP(type), if_addr)) {
	            return websWrite(wp, T("%s"), if_addr);
	        } 
        }
    }
	
	return websWrite(wp, T(""));
}
/***********************************************************************
 * 函 数 名：   getWanMac
 * 功能描述：	取出wan口mac
 * 输入参数:    wid,wp,argc,argv
 * 输出参数：	
 * 返回值：     0
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       胡传香
 * 附加说明：	无
***********************************************************************/
static int getWanMac(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_mac[18];
    int type = 0;

	if (ejArgs(argc, argv, T("%d"), &type) == 1) {
        if ( type <= getWanSlotCount()) {
            if (0 == getIfMac(getWanIfName(type), if_mac)) {
	            return websWrite(wp, T("%s"), if_mac);
	        }
        }
    }
   	return websWrite(wp, T(""));
}

/***********************************************************************
 * 函 数 名：   getWlanMac
 * 功能描述：	取出wlan接口的mac
 * 输入参数:    eid,wp,argc,argv
 * 输出参数：	
 * 返回值：     0
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       胡传香
 * 附加说明：	无
***********************************************************************/
static int getWlanMac(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_mac[18];

	if (-1 == getIfMac("ra0", if_mac)) {
		//websError(wp, 500, T("getLanIp: calling getIfMac error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_mac);
}

/*
 * arguments: type - 0 = return WAN interface name (default)
 *                   1 = write WAN interface name
 * description: return or write WAN interface name accordingly
 */
/***********************************************************************
 * 函 数 名：   getWanIfNameWeb
 * 功能描述：	取出wan口的名字
 * 输入参数:    eid,wp,argc.argv
 * 输出参数：	
 * 返回值：	    0
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       胡传香
 * 附加说明：	无
***********************************************************************/
static int getWanIfNameWeb(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char *name = getWanIfName(0);

	if (ejArgs(argc, argv, T("%d"), &type) == 1) {
		if (1 == type) {
			return websWrite(wp, T("%s"), name);
		}
	}
	ejSetResult(eid, name);
	return 0;
}

/***********************************************************************
 * 函 数 名：   getWanNetmask
 * 功能描述：	取出wan的netmask
 * 输入参数:    eid,wp,argc,argv
 * 输出参数：	
 * 返回值：	    0
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       胡传香
 * 附加说明：	无
***********************************************************************/
static int getWanNetmask(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_net[16];
    int type = 0;

	if (ejArgs(argc, argv, T("%d"), &type) == 1) {
        if ( type <= getWanSlotCount()) {
            if (0 == getIfNetmask(getWanIfNamePPP(type), if_net)) {
	            return websWrite(wp, T("%s"), if_net);
	        }
        }
	}
   	return websWrite(wp, T(""));
}

int getIfP2pDestIp(char *ifname, char *if_addr)
{
	struct ifreq ifr;
	int skfd = 0;

    if(ifname == NULL) {
		return -1;
    }
	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		error(E_L, E_LOG, T("getIfIp: open socket error"));
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFDSTADDR, &ifr) < 0) {
		close(skfd);
		return -1;
	}
	strcpy(if_addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

	close(skfd);
	return 0;
}

static int getWanDhcpDefaultRoute(int wannum, char* ipaddr)
{
    FILE *fp;
    char buf[MAX_FILE_NAME_LEN];
    int returnval = 0;

    /*
     * set file name
     */
    sprintf(buf, DHCP_ROUTE_FILE, getWanIfName(wannum));

    fp = fopen(buf, "r");
    if(fp != NULL) {
        fgets(buf, 16, fp);
        sscanf(buf, "%s", ipaddr);
        returnval = 0;
    } else {
        returnval = -1;
    }
    return returnval;
}

int getWanGwByIndex(int wanIndex, char* gw)
{
    InterfaceProfile *profIf = NULL;
    MibProfileType mibType = MIB_PROF_INTERFACE;
    struct in_addr tmp;
    /*
     * brwang need edit
     */
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, wanIndex);

    if(profIf->ether.connMode == ETH_CONN_STATIC) {
        tmp.s_addr = profIf->ether.sta.gw;
        strcpy(gw, inet_ntoa(tmp));
    } else if(profIf->ether.connMode == ETH_CONN_DHCP){
        if(getWanDhcpDefaultRoute(wanIndex, gw) == -1) {
            strcpy(gw, "");
        }
    } else {
        if(-1 == getIfP2pDestIp(getWanIfNamePPP(wanIndex), gw)) {
            strcpy(gw, "");
        }
    }
    return 0;
}

static int getWanIfOption(int eid, webs_t wp, int argc, char_t **argv)
{
    Uint32 i = 0;
    for(i = 1; i <= getWanSlotCount(); i++) {
        websWrite(wp, T("<option value=\"%d\">WAN%d</option>\n"), i, i);
    }
    return 0;
}

static int getWanIfCount(int eid, webs_t wp, int argc, char_t **argv)
{
    websWrite(wp, T("var wanIfCount=%d;"), getWanSlotCount());
    websWrite(wp, T("var maxwanIfCount=%d;"), MAX_WAN_IF_COUNT);
    return 0;
}

#if (POLICY_ROUTE == FYES)
/*****
 * getWanConfigCount(int eid, webs_t wp, int argc, char_t **argv)
 * 输出已配置的WAN口 及配置的WAN口数量
 * 若此WAN口已配置，则输出 1
 * 否则输出为 0
 * */
static int getWanConfigCount(int eid, webs_t wp, int argc, char_t **argv)
{
	InterfaceProfile *profIf = NULL;
	MibProfileType mibType = MIB_PROF_INTERFACE;
	int wanCount = 0;
	int wanConfCount = 0;
	int i = 1;
	/*
	 */
	wanCount = getWanSlotCount();
	websWrite(wp, T("var wanIfConf=new Array();"));
	for(i=1;i<=wanCount;i++)
	{
		profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, i);
		if(profIf->head.active == PROF_ENABLE) {
#if (WIRELESS == FYES)
			/* 3G接口，输出3G连接类型 */   
			if ((i == get3GSlotIndex()) || (i == getAPCliSlotIndex())) {
				websWrite(wp, T("wanIfConf[%d]=%d;"), i, 1);
				wanConfCount++;
			} else 
#endif
			{
				if(profIf->ether.connMode == ETH_CONN_STATIC) {
					websWrite(wp, T("wanIfConf[%d]=%d;"), i, 1);
					wanConfCount++;
				} else if(profIf->ether.connMode == ETH_CONN_DHCP){
					websWrite(wp, T("wanIfConf[%d]=%d;"), i, 1);
					wanConfCount++;
				} else if(profIf->ether.connMode == ETH_CONN_PPPOE){
					websWrite(wp, T("wanIfConf[%d]=%d;"), i, 1);
					wanConfCount++;
				} else {
					websWrite(wp, T("wanIfConf[%d]=%d;"), i, 0);
				}
				DBGPRINT("%s-%d: i =%d, connType =%s\n",__func__,__LINE__,i,MibGetStrByEnum(profIf->ether.connMode, ethConnModeEnum));
			}
		} else {
			websWrite(wp, T("wanIfConf[%d]=%d;"), i, 0);
			DBGPRINT("%s-%d: i =%d, connType =%s\n",__func__,__LINE__,i,T("no"));
		}
#if 0
#endif
		DBGPRINT("%s-%d: i =%d\n",__func__,__LINE__,i);
	}
	websWrite(wp, T("var wanIfConfCount=%d;\r\n"), wanConfCount);
}
#endif
/*****
 * wanIsUsed_byuttFunctions(int index)
 * 判断此WAN口是否被策略路由作为转发接口
 * 返回值：
 *		1  被绑定
 *		0  未被绑定
 * */
extern int wanIsUsed_byuttFunctions(int index)
{
	int ret = 0;
	int i = 0, min, max;
	int wanIndex = 0;
#if (POLICY_ROUTE == FYES)
	MibProfileType mibType_pr = MIB_PROF_POLICY_ROUTE;
	PolicyRouteProfile *prof_pr = NULL;
#endif

#if (POLICY_ROUTE == FYES)
	ProfInstLowHigh(mibType_pr, &max, &min);
	for(i = min; i < max; i++) {
		prof_pr = (PolicyRouteProfile*)ProfGetInstPointByIndex(mibType_pr, i);
		if((prof_pr != NULL) && (ProfInstIsFree(prof_pr) == 0)) {
			if(strlen(prof_pr->interfaceName)!=0) {
				wanIndex = strtoul((prof_pr->interfaceName)+3,0,10);
				DBGPRINT("%s-%d: del index =%d, wanIndex =%d\n",__func__,__LINE__,index,wanIndex);
				if(index == wanIndex) {
					ret = 1;
				}
			}
		}
	}
#endif
	return ret;
}
#if( USER_DEF_WAN_COUNT == FYES )
static int formWanIfCount(webs_t wp, char_t *path, char_t *query) 
{
	int				rc=1;
	int				WanNo=0;
    char			*wannumber = NULL;
    SystemProfile	*profSys = NULL;
#if ((UTT_NV_520W == FYES) || (UTT_NV_A308W == FYES) || (UTT_NV_A310Wv2 == FYES) || (UTT_NV_A309W == FYES) || (UTT_NV_WBR_6016 == FYES)|| (UTT_NV_A307W == FYES) || (UTT_NV_A310W == FYES) || (UTT_U_1000N == FYES)|| (UTT_NV_512W == FYES)|| (UTT_NV_518W == FYES)|| (UTT_NV_518WVPN == FYES)|| (UTT_NV_842W == FYES)|| (UTT_NV_518WPLUS == FYES)|| (UTT_NV_518WLITE == FYES) || (UTT_NV_512WV2 == FYES)|| (UTT_NV_518WV2 == FYES) || (UTT_NV_D904W == FYES))
    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;
#endif
	const char *errMsg = NULL;
#if (UTT_LICENSE == FYES)
    LicenseProfile uttlicense;
    LicenseProfile *licensePtr;
    memset(&uttlicense, 0, sizeof(LicenseProfile));
    licensePtr =(LicenseProfile *)ProfGetInstPointByIndex(MIB_PROF_LICENSE, 0);
    if((licensePtr != NULL)){
		memcpy(&uttlicense, licensePtr,sizeof(LicenseProfile));
    }
#endif

    wannumber = websGetVar (wp, T("PortNumber"), T(""));
	WanNo = atoi(wannumber);
    profSys = (SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    if(WanNo > 4 || WanNo < 1)
    {
		errMsg = getTransDataForC(C_LANG_INDEX_DYN_WAN_SETNUM_ERR);
		setTheErrorMsg(errMsg);
    }else{
#if ((UTT_NV_520W == FYES) ||(UTT_NV_A308W == FYES) || (UTT_NV_A310Wv2 == FYES) || (UTT_NV_A309W == FYES) || (UTT_NV_WBR_6016 == FYES) || (UTT_NV_A307W == FYES) || (UTT_NV_A310W == FYES) ||(UTT_U_1000N == FYES)|| (UTT_NV_512W == FYES)|| (UTT_NV_518W == FYES)|| (UTT_NV_518WVPN == FYES)|| (UTT_NV_842W == FYES) || (UTT_NV_512WV2 == FYES)|| (UTT_NV_518WV2 == FYES) ||(UTT_NV_D904W == FYES))
		if((WanNo + 1) == profSys->wanIfCount)
#elif ((UTT_NV_518WPLUS == FYES)||(UTT_NV_518WLITE == FYES))
		if((WanNo + 2) == profSys->wanIfCount)
#else
		if(WanNo == profSys->wanIfCount)
#endif
		{
			errMsg = getTransDataForC(C_LANG_INDEX_DYN_WAN_NUM_NOCHANGE);
			setTheErrorMsg(errMsg);
		}else{
			makeFactoryProfiles();/*恢复出厂配置*/
			profSys->lanPortCount += (profSys->wanIfCount - WanNo);
#if ((UTT_NV_520W == FYES) || (UTT_NV_A308W == FYES)|| (UTT_NV_A310Wv2 == FYES) || (UTT_NV_A309W == FYES) || (UTT_NV_WBR_6016 == FYES) || (UTT_NV_A307W == FYES) || (UTT_NV_A310W == FYES) ||(UTT_U_1000N == FYES)|| (UTT_NV_512W == FYES)|| (UTT_NV_518W == FYES)|| (UTT_NV_518WVPN == FYES)|| (UTT_NV_842W == FYES) || (UTT_NV_512WV2 == FYES)|| (UTT_NV_518WV2 == FYES) || (UTT_NV_D904W == FYES))
			profSys->lanPortCount -= 1;
			profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, profSys->wanIfCount);
			profIf->head.active = PROF_ENABLE;/*将原来的无线接口改为默认up*/
			profSys->wanIfCount = WanNo + 1;
			profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, profSys->wanIfCount);
			profIf->head.active = PROF_DISABLE;/*新的无线接口默认down*/
#elif ((UTT_NV_518WPLUS == FYES)||(UTT_NV_518WLITE == FYES))
			profSys->lanPortCount -= 2;
			profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, profSys->wanIfCount-1);
			profIf->head.active = PROF_ENABLE;/*将原来的3G接口改为默认up*/
			profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, profSys->wanIfCount);
			profIf->head.active = PROF_ENABLE;/*将原来的无线接口改为默认up*/
			profSys->wanIfCount = WanNo + 2;
			profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, profSys->wanIfCount -1);
			profIf->head.active = PROF_DISABLE;/*新的3G接口默认down*/
			profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, profSys->wanIfCount);
			profIf->head.active = PROF_DISABLE;/*新的无线接口默认down*/
#else
			profSys->wanIfCount = WanNo;
#endif
#if (UTT_LICENSE == FYES)
    licensePtr =(LicenseProfile *)ProfGetInstPointByIndex(MIB_PROF_LICENSE, 0);
    if((licensePtr != NULL)){
		memcpy(licensePtr,&uttlicense,sizeof(LicenseProfile));
    }
#endif
			ProfWiteToFlash();/*write flash*/
			errMsg = getTransDataForC(C_LANG_INDEX_DYN_WAN_SETCONF_SAVE);
			setTheRightMsg(errMsg);
			rc = 0;
		}
	}
#if (UTT_TOMAX == FYES)     //Tomax页面跳转不一样
	websRedirect(wp, "portnumber.asp");
#else
	websRedirect(wp, "WANportnumber.asp");
#endif
	return rc;
}
#endif



/***********************************************************************
 * 函 数 名：   getNums
 * 功能描述：	从配置中根据分割符算出位置
 * 输入参数:    字符串，分割符
 * 输出参数：	
 * 返回值：	    位置
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       胡传香
 * 附加说明：	无
***********************************************************************/
static inline int getNums(char *value, char delimit)
{
    char *pos = value;
    int count=1;
    if(!pos || !strlen(pos))
        return 0;
    while( (pos = strchr(pos, delimit))){
        pos = pos+1;
        count++;
    }
    return count;
}

/***********************************************************************
 * 函 数 名：   getIfNameList
 * 功能描述：	返回所有接口名字
 * 输入参数:    eid,wp,argc,argv
 * 输出参数：	
 * 返回值：	    0
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       胡传香
 * 附加说明：	无
***********************************************************************/
static int getIfNameList(int eid, webs_t wp, int argc, char_t **argv)
{
    websWrite(wp,"<option value=wan>WAN</option>\n");
    websWrite(wp,T("<option value=lan>LAN</option>\n"));
    return 0;
}



#if 0
void ripdRestart(void)
{
	char lan_ip[16], wan_ip[16], lan_mask[16], wan_mask[16];

	char *opmode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	char *password = nvram_bufget(RT2860_NVRAM, "Password");
	char *RIPEnable = nvram_bufget(RT2860_NVRAM, "RIPEnable");

	doSystem("killall -q ripd");

	if(!opmode||!strlen(opmode))
		return;
	if(!strcmp(opmode, "0"))	// bridge
		return;

	if(!RIPEnable || !strlen(RIPEnable) || !strcmp(RIPEnable,"0"))
        return;

	if(!password || !strlen(password))
		password = "rt2880";

	doSystem("echo \"hostname linux.router1\" > /etc/ripd.conf ");
	doSystem("echo \"password %s\" >> /etc/ripd.conf ", password);
	doSystem("echo \"router rip\" >> /etc/ripd.conf ");

	// deal with WAN
	if(getIfIp(getWanIfName(), wan_ip) != -1){
		if(getIfNetmask(getWanIfName(), wan_mask) != -1){
			doSystem("echo \"network %s/%d\" >> /etc/ripd.conf", wan_ip, netmask_aton(wan_mask));
			doSystem("echo \"network %s\" >> /etc/ripd.conf", getWanIfName());
		}else
			printf("ripdRestart(): The WAN IP is still undeterminated...\n");
	}else
		DBGPRINT("ripdRestart(): The WAN IP is still undeterminated...\n");

	// deal with LAN
	if(getIfIp(getLanIfName(), lan_ip) != -1){
		if(getIfNetmask(getLanIfName(), lan_mask) != -1){
			doSystem("echo \"network %s/%d\" >> /etc/ripd.conf", lan_ip, netmask_aton(lan_mask));
			doSystem("echo \"network %s\" >> /etc/ripd.conf", getLanIfName());
		}
	}
	doSystem("echo \"version 2\" >> /etc/ripd.conf");
	doSystem("echo \"log syslog\" >> /etc/ripd.conf");
	doSystem("ripd -f /etc/ripd.conf -d");
}

inline void zebraRestart(void)
{
	char *opmode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	char *password = nvram_bufget(RT2860_NVRAM, "Password");

	char *RIPEnable = nvram_bufget(RT2860_NVRAM, "RIPEnable");

	doSystem("killall -q zebra");

	if(!opmode||!strlen(opmode))
		return;
	if(!strcmp(opmode, "0"))	// bridge
		return;

	if(!RIPEnable || !strlen(RIPEnable) || !strcmp(RIPEnable,"0"))
		return;

	if(!password || !strlen(password))
		password = "rt2880";

	doSystem("echo \"hostname linux.router1\" > /etc/zebra.conf ");
	doSystem("echo \"password %s\" >> /etc/zebra.conf ", password);
	doSystem("echo \"enable password rt2880\" >> /etc/zebra.conf ");
	doSystem("echo \"log syslog\" >> /etc/zebra.conf ");
	doSystem("zebra -d -f /etc/zebra.conf");
}
#endif


#define    OP_INTERVAL   1   //两次操作的合法最小时间间隔


/***********************************************************************
 * 函 数 名：   getUptime 
 * 功能描述：	取出开机时间
 * 输入参数:    
 * 输出参数：	开机到现在的时间
 * 返回值：	
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       王炳荣	
 * 附加说明：	无
***********************************************************************/
time_t getUptime()
{
    FILE*fp;
    time_t time;
    fp=fopen("/proc/uptime","r");
    if(!fp)
    {
       return 0;
    }
    else if(fscanf(fp,"%ld",&time)==1)
    {
        fclose(fp);
        return time;
    }
    fclose(fp);
    return 0;
}

/***********************************
 *
 *get the connected time
 *
 * ********************************/



/***********************************************************************
 * 函 数 名：   getpppoeStatus
 * 功能描述:    判断ppp0有没有注册好
 * 输入参数:    
 * 输出参数：	
 * 返回值：	    0,1
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       王炳荣	
 * 附加说明：	无
***********************************************************************/
int getpppoeStatus(int wannum)
{
    int returnval = 0;
    char if_addr[16];

    if(getIfIp(getWanIfNamePPP(wannum), if_addr) == 0) {
        returnval = 1;
    } else {
        returnval = 0;
    }
    return returnval;
}


/***********************************************************************
 * 函 数 名：   asp_netdev 
 * 功能描述：   输出svg所需要的参数 
 * 输入参数:    wp
 * 输出参数：	
 * 返回值：	    0 
 * 创建日期：	2010-3-16
 * 修改日期：
 * 作者：       王炳荣	
 * 附加说明：	无
***********************************************************************/
#define UTT_MULTIWAN_STRCMP_IFNAME
static int which_wan_name_fun(char *ifname)
{
    int i = 0;
    int result = 0;
    for(i = 0; i < getWanSlotCount(); i++) {
       if(strcmp(ifname, getWanIfNamePPP(i+1)) == 0) {
           result = i+1;
           break;
       }
    }
    return result;
}

static void asp_netdev(webs_t wp)
{
	FILE *f;
	char buf[256];
	unsigned long rx, tx;
	char *p;
	char *ifname;
	char comma = ' ';
    int wanIndex=0, i=0;
    char webMsg[MAX_WAN_IF_COUNT+1][128];
	const char *Message = NULL;

    memset(webMsg, 0x00, sizeof(webMsg));
	websWrite(wp,"\n\nnetdev={");
	if ((f = fopen("/proc/net/dev", "r")) != NULL) {
		fgets(buf, sizeof(buf), f);
		fgets(buf, sizeof(buf), f);
		while (fgets(buf, sizeof(buf), f)) {
			if ((p = strchr(buf, ':')) == NULL) continue;
			*p = 0;
			if ((ifname = strrchr(buf, ' ')) == NULL) {
                ifname = buf;
            } else {
                ++ifname;
            }
#if(UTT_NV_1800N == FYES)
			if(strcmp(ifname, getEthLanIfName()) == 0)
			{/*有线口流量*/
				if (sscanf(p + 1, "%lu%*lu%*u%*u%*u%*u%*u%*u%lu", &rx, &tx) == 2) 
				{
					Message=getTransDataForC(C_LANG_INDEX_INTERNET_WIRED);
					snprintf(webMsg[0],128, "'%s':{rx:0x%lx,tx:0x%lx}", Message,rx, tx);
				}
			}
			else
			{
				if(strcmp(ifname, getApIfName()) == 0)
				{/*无线口流量*/
					if (sscanf(p + 1, "%lu%*lu%*u%*u%*u%*u%*u%*u%lu", &rx, &tx) == 2) 
					{
						Message=getTransDataForC(C_LANG_INDEX_INTERNET_WIRELESS);
						snprintf(webMsg[1],128, "'%s':{rx:0x%lx,tx:0x%lx}",Message, rx, tx);
					}
				}
			}
#else
            wanIndex=which_wan_name_fun(ifname);
			if (wanIndex || (strcmp(ifname, getLanIfName()) == 0)
#if UTT_WLAN
                   || (strcmp(ifname, getWLanIfName()) == 0) 
#endif
                   ){

				if (sscanf(p + 1, "%lu%*u%*u%*u%*u%*u%*u%*u%lu", &rx, &tx) != 2) continue;
				/*snprintf(webMsg[wanIndex],128, "'%s':{rx:0x%lx,tx:0x%lx}", ifname, rx, tx);*/
				if(wanIndex == 0)
				{
					snprintf(webMsg[wanIndex],128, "'LAN':{rx:0x%lx,tx:0x%lx}", rx, tx);
				}
				else
				{
#if (WIRELESS == FYES)
					if (wanIndex == get3GSlotIndex()) {
						snprintf(webMsg[wanIndex],128, "'3G':{rx:0x%lx,tx:0x%lx}", rx, tx);
					} else if (wanIndex == getAPCliSlotIndex()) {
						snprintf(webMsg[wanIndex],128, "'APClient':{rx:0x%lx,tx:0x%lx}", rx, tx);
					} else
#endif
					{
						snprintf(webMsg[wanIndex],128, "'WAN%d':{rx:0x%lx,tx:0x%lx}", wanIndex, rx, tx);
					}
				}
			}
#endif
		}

        DBGPRINT("==========\n");
        comma = ' ';
#if(UTT_NV_1800N != FYES)
        for (i=1; i<=getWanSlotCount(); i++) {
            if(strlen(webMsg[i]) != 0) {
		        websWrite(wp, "%c%s", comma, webMsg[i]);
				comma = ',';/*只要打印过一次。则再次打印 首部必须是 逗号*/
            }
            DBGPRINT("wan%d:%c%s\n", i, comma, webMsg[i]);
        }
#endif
		if(strlen(webMsg[0]) != 0) {
			websWrite(wp, "%c%s", comma, webMsg[0]);
			comma = ',';/*只要打印过一次。则再次打印 首部必须是 逗号*/
		}
#if(UTT_NV_1800N == FYES)
		if(strlen(webMsg[1]) != 0) {
			websWrite(wp, "%c%s", comma, webMsg[1]);
			comma = ',';/*只要打印过一次。则再次打印 首部必须是 逗号*/
		}
#endif
		fclose(f);
	}

#if (FEATURE_PORT_STATISTICS==FYES)
	getPortStaticsForSvg(wp, comma);
#endif

	websWrite(wp,"};\n");
}

/***********************************************************************
 * 函 数 名：   formGraphShowForSvg 
 * 功能描述：   svg所需的post处理函数
 * 输入参数:    wp,path,query
 * 输出参数：	
 * 返回值：	    0 
 * 创建日期：	2010-3-16
 * 修改日期：
 * 作者：       王炳荣	
 * 附加说明：	无
***********************************************************************/
static void formGraphShowForSvg(webs_t wp, char * path, char * query) 
{
     websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain;charset=gb2312\nPragma: no-cache\nCache-Control: no-cache\n\n"));
     asp_netdev(wp);
     websDone(wp, 200);
}

/***********************************************************************
 * 函 数 名：   getGraphInfoForSvg 
 * 功能描述：   svg所需的一些基本参数
 * 输入参数:    eid,wp,argc,argv
 * 输出参数：	
 * 返回值：	    0 
 * 创建日期：	2010-3-16
 * 修改日期：
 * 作者：       王炳荣	
 * 附加说明：	无
***********************************************************************/
static int getGraphInfoForSvg(int eid, webs_t wp, int argc, char_t **argv)
{
    int i = 0;
    websWrite(wp,"nvram = {");
    
    websWrite(wp,"lan_ifname: \'%s\',", getLanIfName());
    for(i = 0; i < getWanSlotCount(); i++) {
        websWrite(wp,"wan%d_ifname: \'%s\',", i, getWanIfNamePPP(i + 1));
    }
#if UTT_WLAN
    websWrite(wp,"wl_ifname: \'%s\',", getWLanIfName());
#endif
    websWrite(wp,"wan_proto: \'static\'," );
    websWrite(wp,"web_svg: \'1\'," );
    websWrite(wp,"rstats_colors: \'\'," );
    websWrite(wp,"http_id: \'TID65929cbc61a4d7d7\'" );
    websWrite(wp," };" );
    return 0;
}

#if 0
/***********************************************************************
 * 函 数 名：   form3GReConnect
 * 功能描述：   3g重新连接
 * 输入参数:    wp,path,query
 * 输出参数：	
 * 返回值：	     
 * 创建日期：	2010-3-16
 * 修改日期：
 * 作者：       王炳荣	
 * 附加说明：	无
***********************************************************************/
static void form3GReConnect(webs_t wp,char_t *path,char_t*query)
{
    if( (getUptime()-preFormTime[2]) < OP_INTERVAL)
	goto out;
    else 
	preFormTime[2]=getUptime();
    upWAN("3G",NULL);
	sleep(2);
out:
    websRedirect(wp, "WANConfig.asp");
}
#endif


#if 0
/***********************************************************************
 * 函 数 名：   utt_tolower
 * 功能描述：   大写转换小写函数
 * 输入参数:    字符
 * 输出参数：	
 * 返回值：	    字符
 * 创建日期：	2010-3-16
 * 修改日期：
 * 作者：       王炳荣	
 * 附加说明：	无
***********************************************************************/
static char utt_tolower(char x)
{
	    if(x <= 'Z' && x >= 'A')
			         return (x | 0x20) ;
		    else
				         return x;
}
#endif

static int getWanLinkState(int eid, webs_t wp, int argc, char_t **argv)
{
    int wanIndex = 0;
	if (ejArgs(argc, argv, T("%d"), &wanIndex) == 1) {
        if ( wanIndex <= getWanSlotCount()) {
	            return websWrite(wp, T("%d"), _getWanLinkState(wanIndex));
	    } 
    }
	return websWrite(wp, T(""));
}

/* read the given line (not more than 127 bytes) from filename to buf */
char* read_file_line(char *filename, char *buf, int line)
{
    int i=1;
    FILE *fp; 
    char*p;
    fp=fopen(filename, "r");
    if (fp == NULL ) {
        printf("open %s error\n", filename);
    }else {
        /* the content of the file is a byte, which indicant 
         * the link status of the chnl_name port. 
         * while '1' for up, and '0' for down */
        while ((fgets((void *)buf, 127U, fp)) > 0U) {
            if( i == line ) {
                if( (p=strchr(buf, '\n')) != NULL ) {
                    *p='\0';
                }
                break;
            }
            i++;
        }
        fclose(fp);
    }
    return buf;
}

static int getWanDns(int eid, webs_t wp, int argc, char_t **argv)
{
    int i, wanIndex = 0, dnsNo=0;
    char dns[127]={0}, filename[127];
    //DBGPRINT("in getWanDns\n");
	if ((i=ejArgs(argc, argv, T("%d,%d"), &wanIndex, &dnsNo)) < 1) {
        DBGPRINT("ejArgs ret[%d]\n", i);
        return websWrite(wp, T("Insufficient args\n"));
    }else{
        if ( wanIndex <= getWanSlotCount()) {
            sprintf(filename, "/var/run/wan%d.dns", wanIndex);
			memset(dns, 0, 127);
			read_file_line(filename, dns, dnsNo);
			memmove(dns, &dns[11], 16); 
			if(dns[strlen(dns)-1] == '\n')
				strcpy(dns,"0.0.0.0");
	        websWrite(wp, T("%s"), dns);
	    } 
    }
	return 0;
}
char PdbFromWhichPage[0x20];
void setUpPdbFromWhichPage(char *page)
{
    strncpy(PdbFromWhichPage, page, 20);
    return;
}

static int getUpPdbFromWhichPage(int eid, webs_t wp, int argc, char_t **argv)
{
    websWrite(wp, "var MainPage=\"%s\";", PdbFromWhichPage);
    return 0;
}

static int getLicenseNeedFlag(int eid, webs_t wp, int argc, char_t **argv)
{
	websWrite(wp, "var needlicense;");
#if(UTT_LICENSE == FYES)
	websWrite(wp, "needlicense=%d;",_judgeUttSN());
#else
	websWrite(wp, "needlicense=%d;",0);
#endif
	return 0;
}


extern int moveInstToFirst(char *sID, char * tID, MibProfileType proftype)
{
    int tmp = 0, min = 0, max = 0, i = 0, tagIndex = 0, srcIndex = 0;
    struProfAlloc *proflist  = NULL;
    struProfAlloc *proflist2  = NULL;

    tagIndex = ProfGetInstIndexByName(proftype,tID);
    srcIndex = ProfGetInstIndexByName(proftype,sID);
    ProfInstLowHigh(proftype, &max, &min);

    DBGPRINT("%s------%d,srcIndex = %d, tagIndex = %d\n", __func__, __LINE__, srcIndex, tagIndex);
    if(tagIndex != (srcIndex + 1))
    {                                           /*需要调整位置*/
	for(i = min; i < max; i++)
	{                                     /*  从头开始备份  */
	    /* 假如找到目标位置， 就将源实例备份插    入到目标实例位置   随后也需要备份 目标实例，整条练就完成的连接起来了 */  
	    if(i==tagIndex)
	    {   
		ProfBackupByIndex(proftype,PROF_CHANGE_EDIT,srcIndex, &proflist);  
	    }
	    if(i==srcIndex) continue;                                  /*源实例位置 在tagIndex位置已经插入，现在不需要插入了*/
	    ProfBackupByIndex(proftype,100,i, &proflist);  /*100代表备份，但是不触发profilechange*/
	}
	tmp = 1;
    }
    tagIndex=0;                                                        /*恢复位置从头开始*/
    proflist2 = proflist;
    while(proflist!= NULL) 
    {                                            /*恢复  从开始位置恢复表*/
	_ProfRestore(proflist->proftype, tagIndex++, proflist->oldProf);
	proflist = proflist->next;
    }

    ProfUpdate(proflist2);

    ProfFreeAllocList(proflist);
    ProfFreeAllocList(proflist2);
    return tmp;
}
#if (PARENTS_CONTROL == FYES) || (MAC_FILTER == FYES) || (ARP_BINDS == FYES)
/**
 * 功能： 从linux系统arp表的一个条目中解析出来ip地址以及mac地址
 * 参数:  arpEntry:linux arp表的一行（该值在解析的过程中会被改变）
 *        ipStr   :得到的ip地址
 *        macStr  :得到的mac地址
 * 返回： 1解析正确 0非法条目
 * 作者:  bhou
 * 时间:  2011-11-01
 */
extern bool getIpMacFromSysArpEntry(char*arpEntry, char*ipStr ,char* macStr, char*ifName)
{
    char *ptr, *tmptok1, *split=" ";
    int ct=0;
    unsigned long flags = 0u;
    bool ret = 0, breakTag = 0;

    if (arpEntry != NULL)/*读取成功*/
    {
	tmptok1 = strtok_r(arpEntry,split,&ptr);
	while(tmptok1 != NULL)
	{
	    ct++;
	    if(ct == 1)/*第一项对应ipStr地址*/
	    {
		if(ipStr!=NULL)
		{
		    strcpy(ipStr,tmptok1);
		    /*系统arp条目一般不会出错，但需考虑arp表头等信息*/
		    if(strCountChar(ipStr, '.') < 3)
		    {/*非法ip地址*/
			breakTag = 1;
		    }
		}
	    }
	    else if(ct == 3)
	    {/*第3项为flags*/
		flags = strtoul(tmptok1,NULL,16);
		if(flags == 0u)
		{/*证明该arp缓存暂时过期*/
		    breakTag = 1;
		}
	    }
	    else if(ct==4) 
	    {/*第四项为对应的mac地址*/
		if(macStr!=NULL)
		{
		    strcpy(macStr,tmptok1);
		}
	    }
	    else if(ct == 6)
	    {/*第六项为接口*/
		strRemoveChar(tmptok1, '\n');/*移除多余的换行*/
#if (DHCP_POOL == FYES)
		ret = 1;
		breakTag = 1;
#else
		if(strcasecmp(tmptok1, ifName)==0)
		{
		    ret = 1;
		    breakTag = 1;
		}
#endif
	    }
	    if(breakTag != 0)
	    {
		break;/*一个循环仅一个break。LDRA要求*/
	    }
	    tmptok1 = strtok_r(NULL, split,&ptr);
	}
    }

    return  ret;
}

#endif
#if (PARENTS_CONTROL == FYES) || (MAC_FILTER == FYES)
/***********************************************************************
 * 函 数 名：getArpFromArptable
 * 功能描述：
 * 输入参数：null
 * 输出参数：null
 * 创建日期：2013-05-09
 * 修改日期：
 * 作者：       
 * 附加说明：无
 ***********************************************************************/
static int getArpFromArptable(int eid,webs_t wp,int argc,char_t **argv)
{
	FILE *fp;
	char buf[96], ipStr[32], macStr[32];
	int i = 0;

	memset(ipStr, 0, sizeof(ipStr));
	memset(macStr, 0, sizeof(macStr));

	websWrite(wp,"var i_flag=new Array();");
	websWrite(wp,"var i_ips=new Array();");
	websWrite(wp,"var i_macs=new Array();");
	websWrite(wp,"var i_totalrecs=new Array();");
	websWrite(wp,"\r\n");

	fp=fopen("/proc/net/arp","r");/*读取系统arp表*/
	if(fp != NULL) {
		while(!feof(fp))
		{
			memset(buf, 0, sizeof(buf));
			fgets(buf, sizeof(buf), fp);/*读取一条arp条目*/
			if(strlen(buf) >= ARP_VALID_MIN_LEN)/*合法arp条目*/
			{
				/*从arp条目中解析出来ip以及mac地址*/
				if(getIpMacFromSysArpEntry(buf, ipStr, macStr, getLanIfName())!=0)
				{
					strRemoveChar(macStr, ':');/*移除mac地址中的连接符*/
					websWrite(wp,"i_flag[%d]=1;i_ips[%d]=\"%s\";",i,i,ipStr);
					websWrite(wp,"i_macs[%d]=\"%s\";\n",i,macStr);
					i++;
				}
			}
		}
	}
	websWrite(wp,"i_totalrecs = %d;\n",i);
	fclose(fp);/*关闭文件流*/
	return 0;
}
static struct arpreq arpLan = {
	.arp_pa.sa_family = AF_INET,
	.arp_ha.sa_family = AF_INET
};
static int getCurrentIPMacInfo(int eid, webs_t wp, int argc, char_t **argv)
{
    IPADDR ip = 0U;
    char *mac = NULL;
    MacAddr macAddr;

    memset(&macAddr, 0U, sizeof(macAddr));
    ip = inet_addr((const char *)(wp->ipaddr));	/*字符型ip地址转换为整型*/
    strcpy(arpLan.arp_dev, getLanIfName());	/*接口名称*/
    ((struct sockaddr_in*)&arpLan.arp_pa)->sin_addr.s_addr = ip;
    memset(arpLan.arp_ha.sa_data, 0, sizeof(arpLan.arp_ha.sa_data));
    mac = getMacByIpIoctl(&arpLan);/*通过arp表查询当前ip的对应mac地址*/
    websWrite(wp,"var currentIP=\"%N\";",ip);
    if (mac != NULL)/*查到了*/
    {
	memcpy(&macAddr, mac, 6);
	websWrite(wp,"var currentMac=\"%s\";\n",converMac6To12Byte(macAddr));
    } else {
	websWrite(wp,"var currentMac=\"\";\n");
    }
    return 0;
}
#endif

