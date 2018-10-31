

#include	<stdlib.h>
#include	<sys/ioctl.h>
#include	<net/if.h>
#include	<net/route.h>
#include    <string.h>
#include    <dirent.h>
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include "wuScript.h"
#include "translate.h"
#define    OP_INTERVAL   1
#define    IP_LEN        16
#define    URL_LEN       64
#define    BUF_LEN       96
#define    MAC_LEN       32
typedef struct
{
    time_t time;
    long long rxByte;
    long long txByte;
}linkstatus;

#if (UTT_SMART_UI == FYES)
static struct arpreq arpLan = {
.arp_pa.sa_family = AF_INET,
.arp_ha.sa_family = AF_INET
};
#endif

static void readScanArp(int eid,webs_t wp,int argc,char_t **argv);
extern bool getIpMacFromSysArpEntry(char*arpEntry, char*ipStr ,char* macStr, char*ifName);
extern time_t getUptime();
extern int getWanGwByIndex(int wanIndex, char* gw);
extern int getpppoeStatus(int wannum);

#if (WIRELESS == FYES)
extern int getAPCliSlotIndex(void);
extern int get3GSlotIndex(void);
#endif
#if (EASY_TASK == FYES)
extern void closeDefConf();
#endif

/***********************************************************************
 * 函 数 名：   initWan
 * 功能描述：	发送初始化wan口的message
 * 输入参数:    
 * 输出参数：	
 * 返回值：	    0
 * 创建日期：	2010-3-16
 * 修改日期：
 * 作者：       王炳荣	
 * 附加说明：	无
***********************************************************************/
int initWan(int wanIndex)
{
#if (EASY_TASK == FYES)
    closeDefConf();
#endif
    doSystem("wan.sh %d\n", wanIndex);
    //UttSendMsgForSpeed(MSG_INTERFACE_CHANGE, 0, &wanIndex, sizeof(int));
    return 0;
}

static  time_t connTime[MAX_WAN_IF_COUNT];
static	linkstatus LinkStatus[MAX_WAN_IF_COUNT];
/***********************************************************************
 * 函 数 名：   formReleaseConnect
 * 功能描述：   挂断连接
 * 输入参数:    wp,path,query
 * 输出参数：	
 * 返回值：	     
 * 创建日期：	2010-3-16
 * 修改日期：
 * 作者：       王炳荣	
 * 附加说明：	无
***********************************************************************/
static void formReleaseConnect(webs_t wp,char_t *path,char_t*query)
{
    char_t *wannum;
    int _wannum = 0;
    InterfaceProfile *profIf = NULL;
    MibProfileType mibType = MIB_PROF_INTERFACE;

    wannum = websGetVar(wp, T("Isp_Name"), T(""));
    if(strlen(wannum) <=0 ) {
        wannum = websGetVar(wp, T("PortName"), T(""));
    }
    _wannum = strtol(wannum, NULL, 10);

    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, _wannum);


    if(profIf->ether.connMode == ETH_CONN_PPPOE) {
        doSystem("ppp-off %s", wannum);
        if(profIf->ether.pppoe.pppConnMode != PPP_CONN_MANUAL) {
	    doSystem("wan.sh %s",wannum);
        } 
	sleep(2);
    } else if(profIf->ether.connMode == ETH_CONN_DHCP) {
        doSystem("udhcpc-down.sh %s", wannum);
    } else {
    }
#if (UTT_SMART_UI == FYES)
     setSuccessTips();
    websRedirect(wp, (char_t*)"ConfWizard_Sui.asp?guide=0");
#else
    websRedirect(wp, "WANConfig.asp");
#endif
}

/***********************************************************************
 * 函 数 名：   formReConnect
 * 功能描述：   dhcpc重新连接
 * 输入参数:    wp,path,query
 * 输出参数：	
 * 返回值：	     
 * 创建日期：	2010-3-16
 * 修改日期：
 * 作者：       王炳荣	
 * 附加说明：	无
***********************************************************************/
static time_t preFormTime[MAX_WAN_IF_COUNT]={0};
static void formReConnect(webs_t wp,char_t *path,char_t*query)
{
    char *wannum;
    unsigned int _wannum = 0, arrayindex = 0;
    wannum = websGetVar(wp, T("Isp_Name"), T(""));
    _wannum = strtol(wannum, NULL, 10);
    arrayindex = _wannum - 1;

    if( (getUptime()- preFormTime[arrayindex]) > OP_INTERVAL) {
        preFormTime[arrayindex]=getUptime();
        doSystem("wan.sh %d", _wannum);
	sleep(1);
    } 
    sleep(1);
#if (UTT_SMART_UI == FYES)
     setSuccessTips();
    websRedirect(wp, (char_t*)"ConfWizard_Sui.asp?guide=0");
#else
    websRedirect(wp, "WANConfig.asp");
#endif
}

#if 0
void killDownWan(int wanIndex)
{
    char buf[MAX_UTT_CONF_NAME_LEN] = {0};
    char *connMode;
    add_wanx(_CONF_WAN_CONNECT_MODE, wanIndex, buf);
    connMode=nvram_bufget(RT2860_NVRAM, buf);
    if(strcmp(connMode, "PPPOE")!= 0) {
        sendMsgForSpeed(WAN_LINK_DOWN, wanIndex, 0, NULL, 0);
    }
    add_wanx(_CONF_MULTIPATH_DETECTINTERVAL, wanIndex, buf);
    nvram_bufset(RT2860_NVRAM, buf, "0");
    nvram_commit(RT2860_NVRAM);
    sendMsgForSpeed(DOUBLE_WAN_CONF_CHG, wanIndex, UTT_SPEED_WEB_SEND_MSG_ACT_CHK, NULL, 0);
    add_wanx(_CONF_WAN_CONNECT_MODE, wanIndex, buf);
    nvram_bufset(RT2860_NVRAM, buf, "");
    nvram_commit(RT2860_NVRAM);
}
#endif

static void formRoadDel(webs_t wp, char_t *path, char_t *query)
{
    char* delstr;
    int wanIndex;
    InterfaceProfile *profIf = NULL;
    MibProfileType mibType = MIB_PROF_INTERFACE;
    struProfAlloc *profhead = NULL;
    const char *errMsg = NULL;

    delstr = websGetVar(wp, "delstr", T(""));
    wanIndex = strtol(delstr, NULL, 10);

    if(wanIndex != 1) {
        profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, wanIndex);
        ProfBackupByIndex(mibType, PROF_CHANGE_DEL, wanIndex, &profhead);
        ProfDelInstByIndex(mibType, wanIndex);
	/*如果此WAN口被策略路由作为绑定转发接口
	 * 当删除此WAN口配置时，会给出策略路由失效的提示*/
	if(wanIsUsed_byuttFunctions(wanIndex)) {
	    errMsg = getTransDataForC(C_LANG_INDEX_POLICY_ROUTE_ERR);
	    DBGPRINT("%s-%d: errMsg =%s\n",__func__,__LINE__,errMsg);
	    setTheErrorMsg(errMsg);
	}
        profIf->head.active = FALSE;
        ProfUpdate(profhead);
        ProfFreeAllocList(profhead);
        nvramWriteCommit();
//        initWan(wanIndex);
    }
    websRedirect(wp, "WANConfig.asp");
    return;
}

int setWanx(webs_t wp, int wanIndex, char_t *wanNo, 
        char_t *ctype, InterfaceProfile *profIf)
{
    char_t	*ip, *nm, *gw, *pd, *sd;
    char_t	*user, *pass;
    char lan_addr[16]={0}, lan_mask[16]={0};
    in_addr_t lanIp, wanIp, lanMask, wanMask;
#if (MULTI_LAN == FYES)
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
    unsigned int lanIp2 = 0U, lanIp3 = 0U, lanIp4 = 0U;
    unsigned int lanMask2 = 0U, lanMask3 = 0U, lanMask4 = 0U;
    const char *errMsg = NULL;
#endif
    char buftmp[MAX_UTT_CONF_NAME_LEN] = {0};
    SystemProfile *profSys = NULL;
    const char *invalidIP=NULL;
    const char *wanErr=NULL;
    const char *notSame=NULL;
    const char *invalidAccess=NULL;

#if 0
    struProfAlloc *profhead = NULL;
#endif
    int ret = 0;
//#ifdef CONFIG_USER_3G
#if (WIRELESS == FYES)
    char_t	*usb3g_dev;
    char_t	*isp3g,*auth_type_3g,*pincode,*auth_name,
    *auth_passwd,*conn_name,*dialnum,*pppname,*ppppasswd;
    usb3g_dev=isp3g=auth_type_3g=pincode=auth_name=auth_passwd=conn_name=dialnum=pppname=ppppasswd=NULL;
#endif
    ip = nm = gw = pd = sd = user = pass = NULL;
    lanIp=wanIp=lanMask=wanMask=0U;

    if(profIf->head.active == 0) {
        profIf->head.active = 1;
    }

    profSys = (SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
#if 0
    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, wanIndex, &profhead);
#endif
//#ifdef CONFIG_USER_3G
#if (WIRELESS == FYES)
    if ((wanIndex == get3GSlotIndex()) && (strcmp(ctype, "") !=0 )) {
    	usb3g_dev = websGetVar(wp, T("Dev3G"), T(""));
    	isp3g = websGetVar(wp, T("ISP3G"), T(""));
    	auth_type_3g=websGetVar(wp, T("AuthType3G"), T(""));
    	if (!strncmp(auth_type_3g, "SIM", 4)) {
	   pincode = websGetVar(wp, T("PinCode"), T(""));
#if 0
	   nvram_bufset(RT2860_NVRAM, "wan_3g_pincode", pincode);
#endif
	   strncpy(profIf->ether.wan3g.wan_3g_pincode, pincode, UTT_3G_LEN);
    	} else {
	    auth_name = websGetVar(wp, T("AuthName"), T(""));
	    auth_passwd = websGetVar(wp, T("AuthPasswd"), T(""));
#if 0
    		nvram_bufset(RT2860_NVRAM, "wan_3g_authname", auth_name);
    		nvram_bufset(RT2860_NVRAM, "wan_3g_authpasswd", auty_passwd);
#endif
	    strncpy(profIf->ether.wan3g.wan_3g_authname, auth_name, UTT_3G_LEN);
	    strncpy(profIf->ether.wan3g.wan_3g_authpasswd, auth_passwd, UTT_3G_LEN);
    	}
    	conn_name = websGetVar(wp, T("ConnName"), T(""));
    	dialnum = websGetVar(wp, T("DialNum"), T(""));
    	pppname = websGetVar(wp, T("PPPName"), T(""));
    	ppppasswd = websGetVar(wp, T("PPPPasswd"), T(""));
    	
	profIf->ether.wan3g.wan_3g_isp = MibGetEnumByStr(isp3g, ispMode3G);
	profIf->ether.wan3g.wan_3g_authtype = MibGetEnumByStr(auth_type_3g, authType3G);

	strncpy(profIf->ether.wan3g.wan_3g_connname, conn_name, UTT_3G_LEN);
	strncpy(profIf->ether.wan3g.wan_3g_dialnum, dialnum, UTT_3G_LEN);
	strncpy(profIf->ether.wan3g.wan_3g_pppname, pppname, UTT_3G_LEN);
	strncpy(profIf->ether.wan3g.wan_3g_ppppasswd, ppppasswd, UTT_3G_LEN);
	strncpy(profIf->ether.wan3g.wan_3g_dev, usb3g_dev, UTT_3G_DEV_LEN);
        profIf->ether.connMode = ETH_CONN_PPPOE;

#if 0
    	nvram_bufset(RT2860_NVRAM, "wan_3g_isp", isp3g);
    	nvram_bufset(RT2860_NVRAM, "wan_3g_authtype", auth_type_3g);
    	nvram_bufset(RT2860_NVRAM, "wan_3g_connname", conn_name);
    	nvram_bufset(RT2860_NVRAM, "wan_3g_dialnum", dialnum);
    	nvram_bufset(RT2860_NVRAM, "wan_3g_pppname", pppname);
    	nvram_bufset(RT2860_NVRAM, "wan_3g_ppppasswd", ppppasswd);
    	nvram_bufset(RT2860_NVRAM, "wan_3g_dev", usb3g_dev);
    	nvram_bufset(RT2860_NVRAM, add_wanx(_CONF_WAN_CONNECT_MODE, wanIndex, buftmp), "PPPOE");
#endif
    } else
#endif
    if (!strncmp(ctype, "STATIC", 7) ) {
        sprintf(buftmp, "staticIp%s", wanNo);
        ip = websGetVar(wp, T(buftmp), T(""));
        sprintf(buftmp, "staticNetmask%s", wanNo);
        nm = websGetVar(wp, T(buftmp), T("0"));
        sprintf(buftmp, "staticGateway%s", wanNo);
        gw = websGetVar(wp, T(buftmp), T(""));
        sprintf(buftmp, "staticPriDns%s", wanNo);
        pd = websGetVar(wp, T(buftmp), T(""));
        sprintf(buftmp, "staticSecDns%s", wanNo);
        sd = websGetVarNull(wp, T(buftmp), T("0.0.0.0"), T("0.0.0.0"));
    
#if(UTT_NV_1800N != FYES)
        getIfIp(getLanIfName(), lan_addr);
        getIfNetmask(getLanIfName(), lan_mask);
	lanMask = inet_addr(lan_mask);
	lanIp = inet_addr(lan_addr);
#if(MULTI_LAN == FYES)
	profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
	if ((profIF == NULL) || (profIF->head.active == 0)) {
	    errMsg = getTransDataForC(C_LANG_INDEX_LAN_CONFIG_ERR);
	    setTheErrorMsg(errMsg);
	    return -1;
	}
	lanIp2 = profIF->ether.sta.ip2;
	lanIp3 = profIF->ether.sta.ip3;
	lanIp4 = profIF->ether.sta.ip4;
	lanMask2 = profIF->ether.sta.nm2;
	lanMask3 = profIF->ether.sta.nm3;
	lanMask4 = profIF->ether.sta.nm4;
#endif
#endif
	wanIp = inet_addr(ip);
        wanMask = inet_addr(nm);
          
        if(wanIp == 0xffffffff || wanIp == 0 || 
                ((ntohl(wanIp) & 0xe0000000) == 0xe0000000)) {
    	    invalidIP = getTransDataForC(C_LANG_INDEX_INVALID_IP);
	    setTheErrorMsg(invalidIP);
            return -1;
        }
	/* mantis 8674: wan口地址不能配置广播地址 */
	if ((wanIp & (~wanMask)) == (~wanMask)) {
    	    wanErr = getTransDataForC(C_LANG_INDEX_WANCONFIG_ERR);
	    setTheErrorMsg(wanErr);
	    return -1;
	}

#if(UTT_NV_1800N != FYES)
#if ((DHCP_SERVER == FYES) || (PPTP_SERVER == FYES) || (L2TP_SERVER == FYES) || (PPPOE_SERVER == FYES))
	/* 与dhcp pppoe l2tp pptp地址池冲突 */
	{
	    IpPoolEnum cfPoolType = IP_POOL_NONE;
	    cfPoolType = ipPoolConflict(IP_POOL_NONE, wanIp, wanIp);
	    if (cfPoolType != IP_POOL_NONE) {
		setPoolConflictMsg(cfPoolType);
		return -1;
	    }
	}
#endif
#endif

#if(UTT_NV_1800N != FYES)
        if (((wanIp & wanMask) == (lanIp & wanMask)) 
            || ((wanIp & lanMask) == (lanIp & lanMask))
#if(MULTI_LAN == FYES)
	    || ((wanIp & wanMask) == (lanIp2 & wanMask)) || ((wanIp & lanMask2) == (lanIp2 & lanMask2)) 
	    || ((wanIp & wanMask) == (lanIp3 & wanMask)) || ((wanIp & lanMask3) == (lanIp3 & lanMask3)) 
	    || ((wanIp & wanMask) == (lanIp4 & wanMask)) || ((wanIp & lanMask4) == (lanIp4 & lanMask4)) 
#endif 
	    )
	    {
    	    notSame = getTransDataForC(C_LANG_INDEX_LAN_NOT_SAME_WAN);
	    setTheErrorMsg(notSame);
    	    ret = -1;
        } else
#endif
	{
            profIf->ether.connMode = ETH_CONN_STATIC;
            profIf->ether.sta.ip = wanIp;
            profIf->ether.sta.nm = wanMask;
            profIf->ether.sta.gw = inet_addr(gw);
            profIf->ether.sta.pd = inet_addr(pd);
            profIf->ether.sta.sd = inet_addr(sd);
        }
    } else if (!strncmp(ctype, "DHCP", 5)) {
        profIf->ether.connMode = ETH_CONN_DHCP;
    } else if (!strncmp(ctype, "PPPOE", 6)) {
        char_t *pppoe_opmode, *pppoe_optime,*pppoe_authtype,*pppoe_mtu,*pppoe_dlmode;
        sprintf(buftmp, "pppoeUser%s", wanNo);
        user = websGetVar(wp, T(buftmp), T(""));
        sprintf(buftmp, "pppoePass%s", wanNo);
        pass = websGetVar(wp, T(buftmp), T(""));
        sprintf(buftmp, "pppoeOPMode%s", wanNo);
        pppoe_opmode = websGetVar(wp, T(buftmp), T("KEEPALIVE"));
        sprintf(buftmp, "pppoeAuthType%s", wanNo);
        pppoe_authtype=websGetVar(wp, T(buftmp), T("EITHER"));
        sprintf(buftmp, "MTU%s", wanNo);
        pppoe_mtu=websGetVar(wp, T(buftmp), T("1480"));
        sprintf(buftmp, "pppoeIdleTime%s", wanNo);
        pppoe_optime = websGetVar(wp, T(buftmp), T("0"));
        sprintf(buftmp, "pppoeDailMode%s", wanNo);
        pppoe_dlmode = websGetVar(wp, T(buftmp), T("NORMAL"));
    
        profIf->ether.connMode = ETH_CONN_PPPOE;
        printf(" web mode %x\n",profIf->ether.connMode);
        strncpy((char*)profIf->ether.pppoe.user, user, UTT_PPPOE_CLI_NAME_LEN);
        strncpy((char*)profIf->ether.pppoe.passwd, pass, UTT_PPPOE_CLI_PASS_LEN);
        profIf->ether.pppoe.pppConnMode = MibGetEnumByStr(pppoe_opmode, pppConnModeEnum);
        profIf->ether.pppoe.authType = MibGetEnumByStr(pppoe_authtype, pppAuthEnum);
        profIf->ether.pppoe.spDial = MibGetEnumByStr(pppoe_dlmode, pppSpDailEnum);
        profIf->ether.pppoe.idleTime = strtol(pppoe_optime, NULL, 10);
        profIf->ether.pppoe.mtu = strtol(pppoe_mtu, NULL, 10);
	if (profIf->detectIp == 0U)
	{
	    profIf->detectInterval = 0U;
	    profIf->detectCount = 10U;
	}
    } else {
    	invalidAccess = getTransDataForC(C_LANG_INDEX_INVALID_ACCESS_TYPE);
	setTheErrorMsg(invalidAccess);
    	ret = -1;
    }

    if(ret != -1) {
        if(profSys->lineGroupMode == GROUP_LINE_BALANCE) {
            profIf->runningLineMode = 1;
        } else {
            if(profIf->lineBalMode == IF_MIAN_LINE) {
                profIf->runningLineMode = 1;
            } else {
                profIf->runningLineMode = 0;
            }
        }
    }
#if 0
    if(ret == 0) {
         ProfUpdate(profhead);
    }
    ProfFreeAllocList(profhead);
#endif

    return ret;
}


#if (UTT_SMART_UI == FYES)
/*新风格UI  WAN口配置函数*/
static void formWanIfConfig_Sui(webs_t wp, char_t *path, char_t *query)
{
    char *ip = NULL, *nm = NULL, *gw = NULL, *pd = NULL, *sd = NULL;
    char lan_addr[16]={0}, lan_mask[16]={0};
    in_addr_t lanIp, wanIp, lanMask, wanMask;
    char *page_type = NULL, *conn_type = NULL;
    char *pppName = NULL, *pppPwd = NULL, *pppoe_authtype = NULL;
    char *mac = NULL;
    char *pppSpDail = NULL,*connMode = NULL, *pppMtu = NULL;
    InterfaceProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_INTERFACE;
    struProfAlloc *profhead = NULL;
#if (MULTI_LAN == FYES)
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
    unsigned int lanIp2 = 0U, lanIp3 = 0U, lanIp4 = 0U;
    unsigned int lanMask2 = 0U, lanMask3 = 0U, lanMask4 = 0U;
#endif
    const char *invalidIP=NULL;
    const char *wanErr=NULL;
    const char *notSame=NULL;
    const char *errMsg = NULL;
    int isErr = 0;

    page_type = websGetVar(wp, T("page_type"), T(""));
    conn_type = websGetVar(wp, T("conn_type"), T(""));

    prof = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, 1);
    if (ProfInstIsFree(prof) == PROFOK)
    {
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 1, &profhead);
	if (strcmp(conn_type, "pppoe") == 0)
	{
	    pppName = websGetVar(wp, T("adsl_account"), T(""));
	    pppPwd = websGetVar(wp, T("adsl_pwd"), T(""));
	    pppMtu = websGetVar(wp, T("mtu"), T("1480"));
	    prof->ether.connMode = ETH_CONN_PPPOE;
	    strncpy((char*)prof->ether.pppoe.user, pppName, UTT_PPPOE_CLI_NAME_LEN);
	    strncpy((char*)prof->ether.pppoe.passwd, pppPwd, UTT_PPPOE_CLI_PASS_LEN);
	    prof->ether.pppoe.mtu = strtol(pppMtu, NULL, 10);
	    if (strcmp(page_type, "0") == 0)/*上网设置*/
	    {
		mac = websGetVar(wp, T("adsl_mac"), T(""));
		pppSpDail = websGetVar(wp, T("pppoeDailMode"), T("NORMAL"));
		pppoe_authtype = websGetVar(wp, T("pppoeAuthType"), T("EITHER"));
		connMode = websGetVar(wp, T("pppoeOPMode"), T("KEEPALIVE"));
		converMac12To6Byte(mac, &(prof->ether.mac));
		prof->ether.pppoe.pppConnMode = MibGetEnumByStr(connMode, pppConnModeEnum);
		prof->ether.pppoe.authType = MibGetEnumByStr(pppoe_authtype, pppAuthEnum);
		prof->ether.pppoe.spDial = MibGetEnumByStr(pppSpDail, pppSpDailEnum);
	    }
	}
	else if (strcmp(conn_type, "static") == 0)
	{
	    connMode = websGetVar(wp, T("ipset"), T(""));
	    if (strcmp(connMode, "dhcp") == 0)
	    {
		prof->ether.connMode = ETH_CONN_DHCP;
		if (strcmp(page_type, "0") == 0)/*上网设置*/
		{
		    mac = websGetVar(wp, T("dhcpmac"), T(""));
		    converMac12To6Byte(mac, &(prof->ether.mac));
		}
	    }
	    else if (strcmp(connMode, "static") == 0)
	    {
		ip = websGetVar(wp, T("ip"), T(""));
		nm = websGetVar(wp, T("mask"), T("0"));
		gw = websGetVar(wp, T("gw"), T(""));
		pd = websGetVar(wp, T("priDns"), T(""));
		sd = websGetVarNull(wp, T("seCDns"), T("0.0.0.0"), T("0.0.0.0"));
#if(UTT_NV_1800N != FYES)
		getIfIp(getLanIfName(), lan_addr);
		getIfNetmask(getLanIfName(), lan_mask);
		lanMask = inet_addr(lan_mask);
		lanIp = inet_addr(lan_addr);
#if(MULTI_LAN == FYES)
		profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
		if ((profIF == NULL) || (profIF->head.active == 0)) {
		    errMsg = getTransDataForC(C_LANG_INDEX_LAN_CONFIG_ERR);
		    setTheErrorMsg(errMsg);
		    isErr = 1;
		    goto OUT;
		}
		lanIp2 = profIF->ether.sta.ip2;
		lanIp3 = profIF->ether.sta.ip3;
		lanIp4 = profIF->ether.sta.ip4;
		lanMask2 = profIF->ether.sta.nm2;
		lanMask3 = profIF->ether.sta.nm3;
		lanMask4 = profIF->ether.sta.nm4;
#endif
#endif
		wanIp = inet_addr(ip);
		wanMask = inet_addr(nm);

		if(wanIp == 0xffffffff || wanIp == 0 || 
			((ntohl(wanIp) & 0xe0000000) == 0xe0000000)) {
		    invalidIP = getTransDataForC(C_LANG_INDEX_INVALID_IP);
		    setTheErrorMsg(invalidIP);
		    isErr = 1;
		    goto OUT;
		}
		/* mantis 8674: wan口地址不能配置广播地址 */
		if ((wanIp & (~wanMask)) == (~wanMask)) {
		    wanErr = getTransDataForC(C_LANG_INDEX_WANCONFIG_ERR);
		    setTheErrorMsg(wanErr);
		    isErr = 1;
		    goto OUT;
		}
#if ((DHCP_SERVER == FYES) || (PPTP_SERVER == FYES) || (L2TP_SERVER == FYES) || (PPPOE_SERVER == FYES))
		/* 与dhcp pppoe l2tp pptp地址池冲突 */
		{
		    IpPoolEnum cfPoolType = IP_POOL_NONE;
		    cfPoolType = ipPoolConflict(IP_POOL_NONE, wanIp, wanIp);
		    if (cfPoolType != IP_POOL_NONE) {
			setPoolConflictMsg(cfPoolType);
			isErr = 1;
			goto OUT;
		    }
		}
#endif
#if(UTT_NV_1800N != FYES)
		if (((wanIp & wanMask) == (lanIp & wanMask)) 
			|| ((wanIp & lanMask) == (lanIp & lanMask))
#if(MULTI_LAN == FYES)
			|| ((wanIp & wanMask) == (lanIp2 & wanMask)) || ((wanIp & lanMask2) == (lanIp2 & lanMask2)) 
			|| ((wanIp & wanMask) == (lanIp3 & wanMask)) || ((wanIp & lanMask3) == (lanIp3 & lanMask3)) 
			|| ((wanIp & wanMask) == (lanIp4 & wanMask)) || ((wanIp & lanMask4) == (lanIp4 & lanMask4)) 
#endif 
		   )
		{
		    notSame = getTransDataForC(C_LANG_INDEX_LAN_NOT_SAME_WAN);
		    setTheErrorMsg(notSame);
		    isErr = 1;
		    goto OUT;
		} else
#endif
		{
		    prof->ether.connMode = ETH_CONN_STATIC;
		    prof->ether.sta.ip = wanIp;
		    prof->ether.sta.nm = wanMask;
		    prof->ether.sta.gw = inet_addr(gw);
		    prof->ether.sta.pd = inet_addr(pd);
		    prof->ether.sta.sd = inet_addr(sd);
		    if (strcmp(page_type, "0") == 0)/*上网设置*/
		    {
			mac = websGetVar(wp, T("staticmac"), T(""));
			converMac12To6Byte(mac, &(prof->ether.mac));
		    }
		}
	    }
	}
	ProfUpdate(profhead);/*更新*/
	ProfFreeAllocList(profhead);
	initWan(1);
	nvramWriteCommit();
    }
OUT:
    if (strcmp(page_type, "1") == 0)/*配置向导*/
    {
	if (isErr)
	{
	    websRedirect(wp, (char_t*)"ConfWizard_Sui.asp?guide=1");
	}
	else
	{
	    websRedirect(wp, (char_t*)"WireWizard_Sui.asp");
	}
    }
    else/*上网设置*/
    {
#if(UTT_SMART_UI==FYES)
          setSuccessTips();
#endif
	websRedirect(wp, (char_t*)"ConfWizard_Sui.asp?guide=0");
    }
    return;
}
#endif


/***********************************************************************
 * 函 数 名：   setWan
 * 功能描述：   wan口配置函数
 * 输入参数:    wp,path,query
 * 输出参数：	
 * 返回值：	     
 * 创建日期：	2010-3-16
 * 修改日期：
 * 作者：       王炳荣	
 * 附加说明：	无
***********************************************************************/
static void formWanIfConfig(webs_t wp, char_t *path, char_t *query)
{
    char_t *ctype, *wanNo, *isp, *isp1, *isp2, *mac,*GateWayMac;
    const char *errMsg=NULL;
    struProfAlloc *profhead = NULL;
	char_t *enabled = NULL,*BindWanEn = NULL;
#if 1
#if (PORT_CONF_REDUCED == FYES)
    char *mode;
    SwPortConfMode oldMode;
#endif
#endif

    ctype = websGetVar(wp, T("connectionType"), T("")); 
#if (CAPWAP_CONFIG_AP == FYES)
    char_t *dns = NULL;
    ApCapwapConfigProfile *profab_ac = NULL;
    MibProfileType mibTypeab_ac = MIB_PROF_AP_CAPWAP_CONFIG;
    profab_ac = (ApCapwapConfigProfile *)ProfGetInstPointByIndex(mibTypeab_ac,0);
    char *ac_host= NULL;
    char ac_buff[32]= "";
    int i = 0;
    int flag = 0;
    char *web_flag = NULL;

    web_flag = websGetVar(wp, T("web_flag"), T(""));
    //    printf("%s, web_flag:%s\n", __func__, web_flag);
    if (atoi(web_flag) != 1)
    {
	/*瘦AP，动态IP则从此跳转，静态IP开始设置后在跳转*/
	if ((ctype != NULL) && (strcmp(ctype, "STATIC") != 0))
	{
	    /*动态IP*/
	    goto OUT;
	}
    }
#endif

#if 0
    char buftmp[MAX_UTT_CONF_NAME_LEN] = {0};
#endif
    int wanIndex = 0;
    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;

	enabled = websGetVar(wp, T("enabled"), T(""));
	BindWanEn = websGetVar(wp, T("BindWanEn"), T(""));
    ctype = websGetVar(wp, T("connectionType"), T("")); 
    wanNo = websGetVar(wp, T("PortName"), T("0"));
    isp = websGetVar(wp, T("ISPType"), T(""));
    isp1 = websGetVar(wp, T("ISPType1"), T(""));
    isp2 = websGetVar(wp, T("ISPType2"), T(""));
    mac=websGetVar(wp, T("MacAddr"), T(""));
    GateWayMac=websGetVar(wp, T("GateWayMac"), T(""));
#if 1
#if (PORT_CONF_REDUCED == FYES)
    mode = websGetVar(wp, T("WanMode"), T(""));
#endif
#endif
    int isChange = 0;

    wanIndex=(int)strtol((const char*)wanNo, (char**)NULL, 10);

    if(wanIndex < 1 || wanIndex > getWanSlotCount()) {
        errMsg = getTransDataForC(C_LANG_INDEX_INVALID_INTERFACE);
	setTheErrorMsg(errMsg);
        goto OUT;
    }
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, wanIndex);
    if((profIf->isp != strtol((const char*)isp, (char**)NULL, 10))||(profIf->isp1 != strtol((const char*)isp1, (char**)NULL, 10))||(profIf->isp2 != strtol((const char*)isp2, (char**)NULL, 10)))
    {
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, wanIndex, &profhead);
	profIf->isp = strtol((const char*)isp, (char**)NULL, 10);
	profIf->isp1 = strtol((const char*)isp1, (char**)NULL, 10);
	profIf->isp2 = strtol((const char*)isp2, (char**)NULL, 10);
	isChange = 1;
    }
#if (NAT_GLOBAL_ENABLE == FYES)
    if(strcmp(enabled,"1") == 0)
    {
	profIf->NatActive = 1;
    }
    else if(strcmp(enabled,"0") == 0)
    {
	profIf->NatActive = 0;
    }
#endif
    profIf->isp = strtol((const char*)isp, (char**)NULL, 10);
    profIf->isp1 = strtol((const char*)isp1, (char**)NULL, 10);
    profIf->isp2 = strtol((const char*)isp2, (char**)NULL, 10);

#if 0
    /* Conversion2StdMac is not null terminated */
    strcpy(buftmp,mac);
    Conversion2StdMac(buftmp);
    buftmp[17]='\0'; 
#endif
    converMac12To6Byte(mac, &(profIf->ether.mac));
    
    if(strcmp(BindWanEn,"MANUAL") == 0){
	converMac12To6Byte(GateWayMac, &(profIf->GateWayMac));
	profIf->BindWanEn = FUN_ENABLE;
    }else{
	profIf->BindWanEn = FUN_DISABLE;
	converMac12To6Byte("000000000000", &(profIf->GateWayMac));
    }
#if 1
#if (PORT_CONF_REDUCED == FYES)
#if (WIRELESS == FYES)
    if ((wanIndex == getAPCliSlotIndex()) || (wanIndex == get3GSlotIndex())) {
	    profIf->ether.speedMode=PORT_MODE_DISABLED;
    } 
    else
#endif
    {
	oldMode = profIf->ether.speedMode;
	if(strcmp(mode, "0")==0) {
	    oldMode=PORT_MODE_10FDX;
	} else if(strcmp(mode, "1")==0) {
	    oldMode=PORT_MODE_100FDX;
	} else if(strcmp(mode, "2")==0) {
	    oldMode=PORT_MODE_1GFDX;
	} else if(strcmp(mode, "3")==0) {
	    oldMode=PORT_MODE_AUTO;
	} else if(strcmp(mode, "4")==0) {
	    oldMode=PORT_MODE_10HDX;
	} else if(strcmp(mode, "5")==0) {
	    oldMode=PORT_MODE_100HDX;
	} else {
	    oldMode=PORT_MODE_DISABLED;
	}
    }

    if(profIf->ether.speedMode != oldMode) {
	if (isChange == 0) {
		ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, wanIndex, &profhead);
		isChange = 1;
	}
	profIf->ether.speedMode = oldMode;
    }
#endif
#endif
#if 0
    /*检测接入类型是否发生变化*/
    int cctype=0;
    if(!strcmp(ctype,"DHCP")){
        cctype = 0;
    } else if (!strcmp(ctype, "STATIC")) {
        cctype = 1;
    }else if (!strcmp(ctype, "PPPOE")) {
        cctype = 2;
    }
    if(cctype!=profIf->ether.connMode){
	if (isChange == 0) {
		ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, wanIndex, &profhead);
		isChange = 1;
    }
     profIf->ether.connMode = cctype; 
    }
#endif
    
    if (isChange == 1) {
	    ProfUpdate(profhead);/*更新*/
	    ProfFreeAllocList(profhead);
    }
//    strncpy(profIf->ether.mac, mac, UTT_MAC_STR_LEN);
//#ifdef CONFIG_RT2860V2_AP_MODULE
#if (WIRELESS == FYES)
    if (wanIndex == getAPCliSlotIndex()) {
        WirelessProfile *profW = (WirelessProfile *)ProfGetInstPointByIndex(MIB_PROF_WIRELESS, 0);
	if ((profW != NULL) && (profW->mBaseCf.apWorkMode != AP_CLIENT_MODE)) {
	    errMsg=getTransDataForC(C_LANG_INDEX_CONFIG_APMODE);
	    setTheErrorMsg(errMsg);
            goto OUT;
	}
#if 0
	    char *apworkmode=nvram_bufget(RT2860_NVRAM, "apworkmode");
        if(strcmp(apworkmode, "5") != 0 ) {
            setTheErrorMsg(T("请先配置AP工作模式为 APClient Mode"));
            goto OUT;
        }
#endif
    }
#endif
    if(setWanx(wp, wanIndex, "", ctype, profIf) == 0) {
	initWan(wanIndex);
	nvramWriteCommit();
#if (CAPWAP_CONFIG_AP == FYES)
        sleep(3);
#endif
    }
#if (FEATURE_AP_BASIC_INFO == FYES) 
    ApBasicConfProfile *profab = NULL;
    MibProfileType mibTypeab = MIB_PROF_AP_BASIC_CONF;
    profab = (ApBasicConfProfile*)ProfGetInstPointByIndex(mibTypeab,0);
    if ((profab != NULL) && (strcmp(ctype, "DHCP") != 0))
    {
	profab->dhcp_en = 0;
    }
    else if(profab != NULL)
    {
	profab->dhcp_en = 1;
    }
#endif
#if (EASY_TASK == FYES)
    closeDefConf();
#endif

OUT:
#if (CAPWAP_CONFIG_AP == FYES)
    if (profab_ac != NULL)
    {
	ProfBackupByIndex(mibTypeab_ac, PROF_CHANGE_EDIT, 0, &profhead);
	for(i = 0; i < 3; i++)
	{
	    sprintf(ac_buff, "Ac_ip%d", i);
	    ac_host = websGetVar(wp, T(ac_buff), T(""));
	    if (ac_host != NULL)
	    {
		if (ac_host[0] != '\0')
		{
		    profab_ac->head.active = TRUE;
		    flag = 1;
		    strncpy(profab_ac->acHostInfo[i], ac_host, MAX_CAPWAP_AC_HOSTNAME);
		}
		else
		{
		    profab_ac->acHostInfo[i][0] = '\0';
		}
	    }
	}
	if (flag != 1)
	{
	    profab_ac->head.active = FALSE;
	}
	else
	{

	    dns = websGetVarNull(wp, T("ManDns"), T("0.0.0.0"), T("0.0.0.0"));
	    profab_ac->pd = inet_addr(dns);
	    dns = websGetVarNull(wp, T("SecDns"), T("0.0.0.0"), T("0.0.0.0"));
	    profab_ac->sd = inet_addr(dns);
	}
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
    }

    if (atoi(web_flag) == 1)
    {	
	websRedirect(wp, (char_t*)"WANConfig.asp");
    }
    else
    {
	websRedirect(wp, (char_t*)"fitApIfConf.asp");
    }
#else
    websRedirect(wp, (char_t*)"WANConfig.asp");
#endif
    return;
}

/***********************************************************************
 * 函 数 名：   formConfigFastDirection
 * 功能描述：   快速配置向导form函数
 * 输入参数:    wp,path,query
 * 输出参数：	
 * 返回值：	     
 * 创建日期：	2010-3-16
 * 修改日期：
 * 作者：       王炳荣	
 * 附加说明：	无
***********************************************************************/
static void formConfigFastDirection(webs_t wp, char_t *path, char_t *query)
{
    char_t  *ConnMode; 
    InterfaceProfile *profIf = NULL;
    MibProfileType mibType = MIB_PROF_INTERFACE;
//#ifdef CONFIG_RT2860V2_AP_MODULE
#if 0
    char_t  *apssid, *apmac, *chanWidth, *ssids, *channels, *wrlessMode;
    char_t  *secmode, *enc, *wpapsk, *keyid, *keytype,
	    *key1, *key2, *key3, *key4,*keylen1,*keylen2,*keylen3,*keylen4;
    char buftmp[MAX_UTT_CONF_NAME_LEN] = {0};
    MibProfileType wirelessmibType = MIB_PROF_WIRELESS;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    WirelessProfile *prof= NULL;

#endif

    /* get wireless ap value */
//#ifdef CONFIG_RT2860V2_AP_MODULE
#if 0
    prof = (WirelessProfile*)ProfGetInstPointByIndex(wirelessmibType, 0);
    profAction = PROF_CHANGE_EDIT;
    ProfBackupByIndex(wirelessmibType, profAction, 0, &profhead);

    ssids = websGetVar(wp, T("ssid"), T("")); 
    chanWidth = websGetVar(wp, T("chanWidth"), T("")); 
    wrlessMode = websGetVar(wp, T("wrlessMode"), T("")); 
    channels = websGetVar(wp, T("channel"), T("")); 
    
    /*basicrate: bg,bgn,n:15, b:3; g,gn:351 */
    if (!strncmp(wrlessMode, "4", 2) || !strncmp(wrlessMode, "7", 2)){ //g, gn
	//nvram_bufset(RT2860_NVRAM, "BasicRate", "351");
	prof->mBaseCf.BasicRate = 351;
    } else if (!strncmp(wrlessMode, "1", 2)){ //b
	//nvram_bufset(RT2860_NVRAM, "BasicRate", "3");
	prof->mBaseCf.BasicRate = 3;
    } else {//bg,bgn,n
	//nvram_bufset(RT2860_NVRAM, "BasicRate", "15");
	prof->mBaseCf.BasicRate = 15;
    }
    if (!strncmp(wrlessMode, "6", 2) || !strncmp(wrlessMode, "9", 2)){
	//nvram_bufset(RT2860_NVRAM, "HT_BW", chanWidth);
	prof->mBaseCf.ChanWidth = strtoul(chanWidth,0,10);
    }
    if (!strncmp(channels, "0", 2)) {
	//nvram_bufset(RT2860_NVRAM, "AutoChannelSelect", "1");
	prof->mBaseCf.AutoChannelSelect = 1;
    } else {
	//nvram_bufset(RT2860_NVRAM, "AutoChannelSelect", "0");
	prof->mBaseCf.AutoChannelSelect = 0;
    }
    if (0 != strlen(channels)) {
    	//nvram_bufset(RT2860_NVRAM, "Channel", channels);
	prof->mBaseCf.Channel = strtoul(channels, 0, 10);
    	doSystem("iwpriv ra0 set Channel=%s", channels);
    }
    //nvram_bufset(RT2860_NVRAM, "SSID1", ssids);
    //nvram_bufset(RT2860_NVRAM,"WirelessMode",wrlessMode);
    prof->mBaseCf.WirelessMode = strtoul(wrlessMode, 0, 10);
    strcpy(prof->mBaseCf.SSID1,ssids);
    /* wireless ap configure save end */
    /*get apclient web value */
    ConnMode=websGetVar(wp, T("connectionType4"), T(""));
    //nvram_bufset(RT2860_NVRAM, "ApCliEnable", "0");
    //nvram_bufset(RT2860_NVRAM, "apworkmode", "1");
    prof->mBaseCf.apClient.apclientEnable = 0;
    prof->mBaseCf.apWorkMode = 1;

    if(strlen(ConnMode)) {
        printf("Wan4ConnMode[%s]\n", ConnMode);
        apssid=websGetVar(wp, T("APssid"), T("")); 
        apmac=websGetVar(wp, T("wds_list"), T("")); 
        secmode = websGetVar(wp, T("apcli_mode"), T("open"));
        enc = websGetVar(wp, T("apcli_enc"), T("none"));
        wpapsk = websGetVar(wp, T("apclipskPsswd"), T("12345678"));//预共享密钥
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

#if 0
        nvram_bufset(RT2860_NVRAM, add_wanx(_CONF_WAN_CONNECT_MODE, 4, buftmp), "DHCP");
#endif

    prof->mBaseCf.apClient.apclientEnable = 1;
    prof->mBaseCf.apWorkMode = 5;
    strcpy(prof->mBaseCf.apClient.ApCliSsid, apssid);
    converMac12To6Byte(apmac, &(prof->mBaseCf.apClient.APCliMacs));
    strcpy(prof->mBaseCf.SelAuthMode, secmode);
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
/*
        nvram_bufset(RT2860_NVRAM, "ApCliEnable", "1");
	nvram_bufset(RT2860_NVRAM, "apworkmode", "5");
	nvram_bufset(RT2860_NVRAM, "ApCliBssid", apmac);
	nvram_bufset(RT2860_NVRAM, "ApCliSsid", apssid);
	nvram_bufset(RT2860_NVRAM, "ApCliAuthMode", secmode);
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
	*/
    }else {
#if 0
        nvram_bufset(RT2860_NVRAM, add_wanx(_CONF_WAN_CONNECT_MODE, 4, buftmp), "");
#endif
    }
    /* ap client configure save end */
#if 0
    nvram_commit(RT2860_NVRAM);
    sendMsgForSpeed(INIT_WLAN, 0, 0, NULL, 0); 
#endif
#endif
    /*initWan(4); do it in INIT_WLAN*/
    /* modify wan1 configure */
    
    /*检测接入类型是否发生变化*/
    ConnMode=websGetVar(wp, T("connectionTypew"), T(""));
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, 1);
#if 0
    struProfAlloc *profhead = NULL;
    int isChange = 0;
    int cctype = 0;
    if(!strcmp(ConnMode,"DHCP")){
        cctype = 0;
    } else if (!strcmp(ConnMode, "STATIC")) {
        cctype = 1;
    }else if (!strcmp(ConnMode, "PPPOE")) {
        cctype = 2;
    }
    if(cctype!=profIf->ether.connMode){
	if (isChange == 0) {
		ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 1, &profhead);
		isChange = 1;
    }
     profIf->ether.connMode = cctype; 
    }
    if (isChange == 1) {
	    ProfUpdate(profhead);/*更新*/
	    ProfFreeAllocList(profhead);
    }

    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, 1);
#endif
    if(strlen(ConnMode)){
        setWanx(wp, 1, "", ConnMode, profIf);
    }
    initWan(1);

//#ifdef CONFIG_RT2860V2_AP_MODULE
#if 0
    /* modify wan2 configure */
    ConnMode=websGetVar(wp, T("connectionType2"), T(""));
    nvram_bufset(RT2860_NVRAM, add_wanx(_CONF_WAN_CONNECT_MODE, 2, buftmp), "");
    nvram_commit(RT2860_NVRAM);
    if(strlen(ConnMode)){
        setWanx(wp, 2, "2", ConnMode, profIf);
    }
    initWan(2);

    /* modify wan3 configure */
    ConnMode=websGetVar(wp, T("connectionType3"), T(""));
    nvram_bufset(RT2860_NVRAM, add_wanx(_CONF_WAN_CONNECT_MODE, 3, buftmp), "");
    nvram_commit(RT2860_NVRAM);
    if(strlen(ConnMode)){
        setWanx(wp, 3, "", ConnMode);
    }
    initWan(3);
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
#endif
    nvramWriteCommit();
    if(webErrorIsOk)
        websRedirect(wp, "SysInfo.asp");
    else
        websRedirect(wp, "FastConfig.asp");
    return;    
}

#if (WIRELESS == FYES)
/***********************************************************************
 * 函 数 名：   formConfigFastDirectionW
 * 功能描述：   无线快速配置向导form函数
 * 输入参数:    wp,path,query
 * 输出参数：	
 * 返回值：	     
 * 修改日期：
 * 附加说明：	无
***********************************************************************/
static void formConfigFastDirectionW(webs_t wp, char_t *path, char_t *query)
{
    char_t  *ConnMode; 
    InterfaceProfile *profIf = NULL;
    MibProfileType mibType = MIB_PROF_INTERFACE;
#if (WIRELESS == FYES)
    char_t  *apssid, *apmac, *chanWidth, *ssids, *channels, *wrlessMode;
    char_t  *secmode, *enc, *wpapsk, *keyid, *keytype,
	    *key1, *key2, *key3, *key4,*keylen1,*keylen2,*keylen3,*keylen4;
    char buftmp[MAX_UTT_CONF_NAME_LEN] = {0};
    MibProfileType wirelessmibType = MIB_PROF_WIRELESS;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    WirelessProfile *prof= NULL;
    int slotAPCli, slot3G;
#endif

#if (WIRELESS == FYES)
    /* get wireless ap value */
    prof = (WirelessProfile*)ProfGetInstPointByIndex(wirelessmibType, 0);
    profAction = PROF_CHANGE_EDIT;
    ProfBackupByIndex(wirelessmibType, profAction, 0, &profhead);

    ssids = websGetVar(wp, T("ssid"), T("")); 
    chanWidth = websGetVar(wp, T("chanWidth"), T("")); 
    wrlessMode = websGetVar(wp, T("wrlessMode"), T("")); 
    channels = websGetVar(wp, T("channel"), T("")); 
    
    /*basicrate: bg,bgn,n:15, b:3; g,gn:351 */
    if (!strncmp(wrlessMode, "4", 2) || !strncmp(wrlessMode, "7", 2)){ //g, gn
	//nvram_bufset(RT2860_NVRAM, "BasicRate", "351");
	prof->mBaseCf.BasicRate = 351;
    } else if (!strncmp(wrlessMode, "1", 2)){ //b
	//nvram_bufset(RT2860_NVRAM, "BasicRate", "3");
	prof->mBaseCf.BasicRate = 3;
    } else {//bg,bgn,n
	//nvram_bufset(RT2860_NVRAM, "BasicRate", "15");
	prof->mBaseCf.BasicRate = 15;
    }
    if (!strncmp(wrlessMode, "6", 2) || !strncmp(wrlessMode, "9", 2)){
	//nvram_bufset(RT2860_NVRAM, "HT_BW", chanWidth);
	prof->mBaseCf.ChanWidth = strtoul(chanWidth,0,10);
    }
    if (!strncmp(channels, "0", 2)) {
	//nvram_bufset(RT2860_NVRAM, "AutoChannelSelect", "1");
	prof->mBaseCf.AutoChannelSelect = 2;
    } else {
	//nvram_bufset(RT2860_NVRAM, "AutoChannelSelect", "0");
	prof->mBaseCf.AutoChannelSelect = 0;
    }
    if (0 != strlen(channels)) {
    	//nvram_bufset(RT2860_NVRAM, "Channel", channels);
	prof->mBaseCf.Channel = strtoul(channels, 0, 10);
    	doSystem("iwpriv ra0 set Channel=%s", channels);
    }
    //nvram_bufset(RT2860_NVRAM, "SSID1", ssids);
    //nvram_bufset(RT2860_NVRAM,"WirelessMode",wrlessMode);
    prof->mBaseCf.WirelessMode = strtoul(wrlessMode, 0, 10);
    strcpy(prof->mBaseCf.SSID1,ssids);
    /* wireless ap configure save end */
    /*get apclient web value */
    ConnMode=websGetVar(wp, T("connectionType4"), T(""));
    //nvram_bufset(RT2860_NVRAM, "ApCliEnable", "0");
    //nvram_bufset(RT2860_NVRAM, "apworkmode", "1");
    prof->mBaseCf.apClient.apclientEnable = 0;
    prof->mBaseCf.apWorkMode = 1;
    slotAPCli = getAPCliSlotIndex();
    if(slotAPCli > 0)
    {
	profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, slotAPCli);

	if(strlen(ConnMode)) {
	    apssid=websGetVar(wp, T("APssid"), T("")); 
	    apmac=websGetVar(wp, T("wds_list"), T("")); 
	    secmode = websGetVar(wp, T("apcli_mode"), T("open"));
	    enc = websGetVar(wp, T("apcli_enc"), T("none"));
	    wpapsk = websGetVar(wp, T("apclipskPsswd"), T("12345678"));//预共享密钥
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

#if 0
	    nvram_bufset(RT2860_NVRAM, add_wanx(_CONF_WAN_CONNECT_MODE, 4, buftmp), "DHCP");
#endif
	    profIf->head.active = 1;
	    profIf->ether.connMode = ETH_CONN_DHCP;

	    prof->mBaseCf.apClient.apclientEnable = 1;
	    prof->mBaseCf.apWorkMode = 5;
	    strcpy(prof->mBaseCf.apClient.ApCliSsid, apssid);
	    converMac12To6Byte(apmac, &(prof->mBaseCf.apClient.APCliMacs));
	    strcpy(prof->mBaseCf.SelAuthMode, secmode);
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
	    /*
	       nvram_bufset(RT2860_NVRAM, "ApCliEnable", "1");
	       nvram_bufset(RT2860_NVRAM, "apworkmode", "5");
	       nvram_bufset(RT2860_NVRAM, "ApCliBssid", apmac);
	       nvram_bufset(RT2860_NVRAM, "ApCliSsid", apssid);
	       nvram_bufset(RT2860_NVRAM, "ApCliAuthMode", secmode);
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
	       */
	}else {
	    /* 关闭ap client ? */
	    profIf->head.active = 0;
#if 0
	    nvram_bufset(RT2860_NVRAM, add_wanx(_CONF_WAN_CONNECT_MODE, 4, buftmp), "");
#endif
	}
	/* ap client configure save end */
    }
#if 0
    nvram_commit(RT2860_NVRAM);
    sendMsgForSpeed(INIT_WLAN, 0, 0, NULL, 0); 
#endif
#endif
    /*initWan(4); do it in INIT_WLAN*/
    /* modify wan1 configure */

    ConnMode=websGetVar(wp, T("connectionType"), T(""));
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, 1);
    if(strlen(ConnMode)){
        setWanx(wp, 1, "", ConnMode, profIf);
    } 
    initWan(1);

#if  (WIRELESS == FYES)
#if 0
    /* modify wan2 configure */
    ConnMode=websGetVar(wp, T("connectionType2"), T(""));
    nvram_bufset(RT2860_NVRAM, add_wanx(_CONF_WAN_CONNECT_MODE, 2, buftmp), "");
    nvram_commit(RT2860_NVRAM);
    if(strlen(ConnMode)){
        setWanx(wp, 2, "2", ConnMode, profIf);
    }
    initWan(2);
#endif

    /* modify 3G configure */
    ConnMode=websGetVar(wp, T("connectionType3"), T(""));
#if 0
    nvram_bufset(RT2860_NVRAM, add_wanx(_CONF_WAN_CONNECT_MODE, 3, buftmp), "");
    nvram_commit(RT2860_NVRAM);
#endif
    slot3G = get3GSlotIndex();

    if(slot3G > 0)
    {
	profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, slot3G);

	if(strlen(ConnMode)){
	    setWanx(wp, slot3G, "", ConnMode, profIf);
	}
	/* mantis 8817,快速向导可删除3G线路 */
	else {
	    profIf->head.active = 0;
	    profIf->ether.connMode = ETH_CONN_DHCP;
	}

	initWan(slot3G);
    }

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
#endif
    nvramWriteCommit();
    if(webErrorIsOk)
        websRedirect(wp, "SysInfo.asp");
    else
        websRedirect(wp, "FastConfig.asp");
    return;    
}

#if (WIRELESS_5G == FYES)
static void formConfigFastDirectionW_5g(webs_t wp, char_t *path, char_t *query)
{
    char_t  *ConnMode;
    InterfaceProfile *profIf = NULL;
    MibProfileType mibType = MIB_PROF_INTERFACE;

    char_t  *chanWidth_24, *ssids_24, *channels_24, *wrlessMode_24;
    char_t  *chanWidth_5, *ssids_5, *channels_5, *wrlessMode_5, *vhtChannelBand_5;
    char buftmp[MAX_UTT_CONF_NAME_LEN] = {0};
    MibProfileType wirelessmibType = MIB_PROF_WIRELESS;
    struProfAlloc *profhead = NULL;
    struProfAlloc *profhead_5g = NULL;
    ProfChangeType profAction;
    WirelessProfile *prof= NULL;
    int slotAPCli, slot3G;

    /* get wireless ap value */
    prof = (WirelessProfile*)ProfGetInstPointByIndex(wirelessmibType, 0);
    profAction = PROF_CHANGE_EDIT;
    ProfBackupByIndex(wirelessmibType, profAction, 0, &profhead);

    ssids_24 = websGetVar(wp, T("ssid_24"), T("")); 
    chanWidth_24 = websGetVar(wp, T("chanWidth_24"), T("")); 
    wrlessMode_24 = websGetVar(wp, T("wrlessModes_24"), T("")); 
    channels_24 = websGetVar(wp, T("channel_24"), T("")); 
    
    /*basicrate: bg,bgn,n:15, b:3; g,gn:351 */
    if (!strncmp(wrlessMode_24, "4", 2) || !strncmp(wrlessMode_24, "7", 2)){ //g, gn
	prof->mBaseCf.BasicRate = 351;
    } else if (!strncmp(wrlessMode_24, "1", 2)){ //b
	prof->mBaseCf.BasicRate = 3;
    } else {//bg,bgn,n
	prof->mBaseCf.BasicRate = 15;
    }
    if (!strncmp(wrlessMode_24, "6", 2) || !strncmp(wrlessMode_24, "9", 2)){
	prof->mBaseCf.ChanWidth = strtoul(chanWidth_24,0,10);
    }
    if (!strncmp(channels_24, "0", 2)) {
	prof->mBaseCf.AutoChannelSelect = 2;
    } else {
	prof->mBaseCf.AutoChannelSelect = 0;
    }
    if (0 != strlen(channels_24)) {
	prof->mBaseCf.Channel = strtoul(channels_24, 0, 10);
    	doSystem("iwpriv ra0 set Channel=%s", channels_24);
    }
    prof->mBaseCf.WirelessMode = strtoul(wrlessMode_24, 0, 10);
    strcpy(prof->mBaseCf.SSID1,ssids_24);
    /* wireless ap configure save end */
    ConnMode=websGetVar(wp, T("connectionType4"), T(""));
    prof->mBaseCf.apClient.apclientEnable = 0;
    prof->mBaseCf.apWorkMode = 1;
    /*initWan(4); do it in INIT_WLAN*/
    /* modify wan1 configure */
    /* get wireless ap value */

	/* 5g wireless*/
    prof = (WirelessProfile*)ProfGetInstPointByIndex(wirelessmibType, 1);
    profAction = PROF_CHANGE_EDIT;
    ProfBackupByIndex(wirelessmibType, profAction, 1, &profhead_5g);

    ssids_5 = websGetVar(wp, T("ssid"), T("")); 
    chanWidth_5 = websGetVar(wp, T("chanWidth"), T("")); 
    wrlessMode_5 = websGetVar(wp, T("wrlessMode"), T("")); 
    channels_5 = websGetVar(wp, T("channel"), T("")); 
    vhtChannelBand_5 = websGetVar(wp, T("vhtChanWidth"), T("")); 

    prof->mBaseCf.WirelessMode = strtoul(wrlessMode_5, 0, 10);
#if 1
    if (!strncmp(wrlessMode_5, "2", 2)) {// 802.11a
		prof->mBaseCf.BasicRate = 15;
		prof->mBaseCf.HT_TxStream = 1;
		prof->mBaseCf.HT_RxStream = 1;
    } else if (!strncmp(wrlessMode_5, "8", 2)) {
		prof->mBaseCf.BasicRate = 15;
		prof->mBaseCf.HT_TxStream = 2;
		prof->mBaseCf.HT_RxStream = 2;
    } else if (!strncmp(wrlessMode_5, "14", 3)) {
		prof->mBaseCf.BasicRate = 15;
		prof->mBaseCf.HT_TxStream = 2;
		prof->mBaseCf.HT_RxStream = 2;
	} else {//bg,bgn,n
		prof->mBaseCf.BasicRate = 15;
		prof->mBaseCf.HT_TxStream = 2;
		prof->mBaseCf.HT_RxStream = 2;
    }
#endif
    strcpy(prof->mBaseCf.SSID1,ssids_5);
    if (!strncmp(channels_5, "0", 2)){
	prof->mBaseCf.AutoChannelSelect = 2;
    }
    else {
	prof->mBaseCf.AutoChannelSelect = 0;
    }
	prof->mBaseCf.Channel = strtoul(channels_5, 0, 10);
	doSystem("iwpriv ra0 set Channel=%s", channels_5);

    prof->mBaseCf.ChanWidth = strtoul(chanWidth_5,0,10);
	if (prof->mBaseCf.ChanWidth == 2) {
		prof->mBaseCf.ChanWidth = 1;
		prof->mBaseCf.HT_BSSCoexistence = 0;
	} else {
		prof->mBaseCf.HT_BSSCoexistence = 1;
	}

    prof->mBaseCf.VChanWidth = strtoul(vhtChannelBand_5,0,10);


    ConnMode=websGetVar(wp, T("connectionType"), T(""));
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, 1);
    if(strlen(ConnMode)){
        setWanx(wp, 1, "", ConnMode, profIf);
    } 
    initWan(1);

#if  (WIRELESS == FYES)
#if 0
    /* modify wan2 configure */
    ConnMode=websGetVar(wp, T("connectionType2"), T(""));
    nvram_bufset(RT2860_NVRAM, add_wanx(_CONF_WAN_CONNECT_MODE, 2, buftmp), "");
    nvram_commit(RT2860_NVRAM);
    if(strlen(ConnMode)){
        setWanx(wp, 2, "2", ConnMode, profIf);
    }
    initWan(2);
#endif

    /* modify 3G configure */
    ConnMode=websGetVar(wp, T("connectionType3"), T(""));
#if 0
    nvram_bufset(RT2860_NVRAM, add_wanx(_CONF_WAN_CONNECT_MODE, 3, buftmp), "");
    nvram_commit(RT2860_NVRAM);
#endif
    slot3G = get3GSlotIndex();

    if(slot3G > 0)
    {
	profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, slot3G);

	if(strlen(ConnMode)){
	    setWanx(wp, slot3G, "", ConnMode, profIf);
	}
	/* mantis 8817,快速向导可删除3G线路 */
	else {
	    profIf->head.active = 0;
	    profIf->ether.connMode = ETH_CONN_DHCP;
	}

	initWan(slot3G);
    }

    ProfUpdate(profhead_5g);
    ProfFreeAllocList(profhead_5g);
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
#endif
    nvramWriteCommit();
    if(webErrorIsOk)
        websRedirect(wp, "SysInfo.asp");
    else
        websRedirect(wp, "FastConfig.asp");
    return;    
}
#endif
#endif

#if (FEATURE_AC== FYES && UTT_SMART_UI != FYES)
#define DEBUG_CK 0

#if DEBUG_CK
	#define debug() do { \
						printf("debug: %d in %s()\n", __LINE__, __func__); \
						} while (0);
#else
	#define debug()
#endif
extern void cmd_ac_set_mode_send_msg(webs_t wp, char mode);


static int workModeInit(webs_t wp, char_t *path, char_t *query)
{
    char *acMode = NULL,*ac_gateWay = NULL,*ac_dns = NULL;
    MibProfileType mibType = MIB_PROF_AC_WORK_MODE;
    AcWorkModeProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
	int flag = 0, tmp, status = 0;
    struct in_addr addr_dns,addr_gw;
    const char *errMsg=NULL;

    acMode = websGetVar(wp, T("acMode"), T(""));
	ac_gateWay = websGetVar(wp, "acGateWay", T(""));
	ac_dns = websGetVar(wp, "acDns", T(""));
	tmp = (int)strtoul(acMode, 0, 10);
	if(tmp == 1)
	{
          /*判断网关地址和DNS地址是否合法*/
		if (0 == inet_aton(ac_gateWay, &addr_gw)) {
            errMsg = getTransDataForC(C_LANG_INDEX_GATEWAY_ERR);
            setTheErrorMsg(errMsg);
            return (-1);
		}
        if (0 == inet_aton(ac_dns, &addr_dns)) {
            errMsg = getTransDataForC(C_LANG_INDEX_INVALID_MAIN_DNS_SERVER);
            setTheErrorMsg(errMsg);
            return (-1);
		}
	}
    prof = (AcWorkModeProfile*)ProfGetInstPointByIndex(mibType, 0);
    if (prof != NULL)
    {
		ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
		if (prof->workMode != tmp )
        {
            prof->workMode= tmp;

            cmd_ac_set_mode_send_msg(wp,(char)prof->workMode);/*发消息*/
            sleep(1u);/*等待一秒*/
            cmd_ac_set_mode_send_msg(wp,(char)prof->workMode);/*发消息*/
            sleep(1u);/*等待一秒*/
            cmd_ac_set_mode_send_msg(wp,(char)prof->workMode);/*发消息*/

        }
		if(tmp == 1)
		{
            prof->gateWay = addr_gw.s_addr;
            prof->pDns = addr_dns.s_addr;
		}
		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
		nvramWriteCommit();
    }
	return 0;
}
static int vlanConfigInit(webs_t wp, char_t *path, char_t *query)
{
    char_t  *new_name, *dhcp_s, *dhcp_m,
			*dhcp_vid,*dhcp_en;
    Uint32 tmpNum = 0, lanIp = 0;
	int max, min, index, indexTmp;
	ProfChangeType profType = PROF_CHANGE_ADD;
    unsigned int nMaxIp;
    MibProfileType mibType = MIB_PROF_VLAN_CONFIG;
#if (MULTI_LAN == FYES)
	Uint32 lanIp1 = 0, lanIp2 = 0, lanIp3 = 0;
#endif

    VlanConfigProfile *prof = NULL;
    struct in_addr addrS, addrE, addrNm, addrGw;
    InterfaceProfile *profIf = NULL;
    struProfAlloc *profhead = NULL;
    IpPoolEnum cfPoolType = IP_POOL_NONE;
    const char *ErrMsg = NULL;

	new_name = websGetVar(wp, T("VlanName"), T(""));
	dhcp_en = websGetVar(wp, T("VlanEnable"), T(""));
	dhcp_vid = websGetVar(wp, T("VlanVid"), T(""));
    dhcp_s = websGetVar(wp, T("VlanStart"), T(""));
    dhcp_m = websGetVar(wp, T("VlanMask"), T(""));

    if (0 == inet_aton(dhcp_s, &addrS)) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_START_IP);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("无效起始地址"));
		return (-1);
    }
    if (0 == inet_aton(dhcp_m, &addrNm)) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_SUBMASK);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("无效子网掩码"));
		return (-1);
    }

    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);
    lanIp = ntohl(profIf->ether.sta.ip);
    if(lanIp == ntohl(addrS.s_addr)){
		ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("DHCP地址池包含了LAN口地址"));
		return (-1);
    }
#if (MULTI_LAN == FYES)
    lanIp1 = ntohl(profIf->ether.sta.ip2);
    if((lanIp1 == ntohl(addrS.s_addr))) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("DHCP地址池包含了LAN口地址"));
		return (-1);
    }

    lanIp2 = ntohl(profIf->ether.sta.ip3);
    if((lanIp2 == ntohl(addrS.s_addr))) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("DHCP地址池包含了LAN口地址"));
		return (-1);
    }

    lanIp3 = ntohl(profIf->ether.sta.ip4);
    if((lanIp3 == ntohl(addrS.s_addr))) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("DHCP地址池包含了LAN口地址"));
		return (-1);
    }
#endif
	ProfInstLowHigh(mibType, &max, &min);

		/*vid是否冲突*/
		for (index = min; index < max; index++) {
			prof = (VlanConfigProfile *)ProfGetInstPointByIndex(mibType, index);
			if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)) && (TRUE == prof->head.active)) {
				if (prof->vid == strtol(dhcp_vid, NULL, 10)) {
					//ErrMsg = getTransDataForC(C_LANG_INDEX_SAME_VID_EXIST);
					//setTheErrorMsg((char *)ErrMsg);
					setTheErrorMsg(T("操作失败，系统中存在相同VID的地址池"));
					return (-1);
				}

				if ((prof->ip & prof->netmask) == (addrS.s_addr & prof->netmask)) {
					setTheErrorMsg(T("操作失败，虚接口地址网段重复！"));
					return (-1);
				}

				if ((prof->ip & addrNm.s_addr) == (addrS.s_addr & addrNm.s_addr)) {
					setTheErrorMsg(T("操作失败，虚接口地址网段重复！"));
					return (-1);
				}
			}
		}

		index = ProfNewInst(mibType, new_name, FALSE);
		profType = PROF_CHANGE_ADD;

		prof = (VlanConfigProfile *)ProfGetInstPointByIndex(mibType, index);
		ProfBackupByIndex(mibType, profType, index, &profhead);
		if (NULL != prof) {
			strcpy(prof->head.name, new_name);
			if(strcmp(dhcp_en,"true") == 0)
			{
				prof->vlanEn = PROF_ENABLE;
			}else{
				prof->vlanEn = PROF_DISABLE;
			}
			prof->vid = strtol(dhcp_vid, NULL, 10);
			prof->ip = addrS.s_addr;
			prof->netmask = addrNm.s_addr;
		}
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
	nvramWriteCommit();

	return 0;
}
#if (DHCP_POOL ==FYES)
static int dhcpPoolInit(webs_t wp, char_t *path, char_t *query)
{
	char_t  *new_name,*dhcp_s, *dhcp_e, *dhcp_m,
			*dhcp_pd, *dhcp_sd, *dhcp_g, *dhcp_l, *dhcp_vid,*dhcp_en;
    Uint32 tmpNum = 0, lanIp = 0;
	char tmp[32];
#if (MULTI_LAN == FYES)
	Uint32 lanIp1 = 0, lanIp2 = 0, lanIp3 = 0;
#endif
	int max, min, index, indexTmp;
	ProfChangeType profType = PROF_CHANGE_ADD;
    unsigned int nMaxIp,vid = 0;
    MibProfileType mibType = MIB_PROF_DHCP_POOL;
#if (SERVICE_ZONE == FYES)
	MibProfileType mibTypeSz = MIB_PROF_SERVICE_ZONE;
	ServiceZoneProfile *profSz = NULL;
	struProfAlloc *profheadSz = NULL;
#endif
#if (TAG_VLAN == FYES)
	MibProfileType mibTypeTv = MIB_PROF_TAG_VLAN;
	TagVlanProfile *profTv = NULL;
	struProfAlloc *profheadTv = NULL;
#endif

#if (VLAN_CONFIG == FYES)
	int maxV, minV, indexV;
    MibProfileType mibTypeV = MIB_PROF_VLAN_CONFIG;
    VlanConfigProfile *profV = NULL;
#endif

    DhcpPoolProfile *prof = NULL;
    struct in_addr addrS, addrE, addrNm, addrGw;
    InterfaceProfile *profIf = NULL;
    struProfAlloc *profhead = NULL;
    IpPoolEnum cfPoolType = IP_POOL_NONE;
    const char *ErrMsg = NULL;
    UserProfile *profUser;

	new_name = websGetVar(wp, T("poolName"), T(""));
	dhcp_en = websGetVar(wp, T("DhcpEnable"), T(""));
	dhcp_vid = websGetVar(wp, T("dhcpVid"), T(""));
    dhcp_s = websGetVar(wp, T("dhcpStart"), T(""));
    dhcp_e = websGetVar(wp, T("dhcpEnd"), T(""));
    dhcp_m = websGetVar(wp, T("dhcpMask"), T(""));
    dhcp_pd = websGetVar(wp, T("dhcpPriDns"), T(""));
    dhcp_sd = websGetVarNull(wp, T("dhcpSecDns"), T("0.0.0.0"), T("0.0.0.0"));
    dhcp_g = websGetVar(wp, T("dhcpGateway"), T(""));
    dhcp_l = websGetVar(wp, T("dhcpLease"), T("86400"));

	if (strncmp(dhcp_vid, "VIF", 3) == 0) {
        vid = strtol(dhcp_vid + 3, NULL, 10);
	}

	debug();
    if (0 == inet_aton(dhcp_s, &addrS)) {
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_START_IP);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("无效起始地址"));
		return (-1);
    }
    if (0 == inet_aton(dhcp_e, &addrE)) {
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_END_IP);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("无效结束地址"));
		return (-1);
    }
    if (0 == inet_aton(dhcp_m, &addrNm)) {
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_SUBMASK);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("无效子网掩码"));
		return (-1);
    }
    if (0 == inet_aton(dhcp_g, &addrGw)) {
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_GATEWAY);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("无效网关"));
		return (-1);
    }
    /*
     * 判断地址池
     */
	debug();
    cfPoolType = ipPoolConflict(IP_POOL_DHCP, addrS.s_addr, addrE.s_addr);
    if (cfPoolType != IP_POOL_NONE) {
	debug();
	setPoolConflictMsg(cfPoolType);
	return (-1);
    }

    /*
     * 是否与dhcp server地址池冲突
     */
	MibProfileType mibTypeDhcp = MIB_PROF_DHCP_POOL;
	DhcpPoolProfile *profDhcp = NULL;
	ProfInstLowHigh(mibTypeDhcp, &max, &min);

	for (index = min; index < max; index++) {
		profDhcp = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibTypeDhcp, index);
        if ((NULL != profDhcp) ) {
                if((ntohl(addrS.s_addr) & ntohl(addrNm.s_addr)) >= (ntohl(profDhcp->ipStart) & ntohl(addrNm.s_addr) )
                       && (ntohl(addrS.s_addr) & ntohl(addrNm.s_addr)) <= (ntohl(profDhcp->ipEnd) & ntohl(addrNm.s_addr))
                    ){
					debug();
					ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_POOL_ADDCONFLICK);
					setTheErrorMsg(ErrMsg);
					return (-1);
                }
                if((ntohl(addrE.s_addr)&ntohl(addrNm.s_addr)) >= (ntohl(profDhcp->ipStart) & ntohl(addrNm.s_addr))
                       && (ntohl(addrE.s_addr) & ntohl(addrNm.s_addr)) <= (ntohl(profDhcp->ipEnd) & ntohl(addrNm.s_addr))
                    ){
					debug();
					ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_POOL_ADDCONFLICK);
	                setTheErrorMsg(ErrMsg);
					return (-1);
                }
		}
	}


#if(UTT_NV_1800N != FYES)
	debug();
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);
    lanIp = ntohl(profIf->ether.sta.ip);
    if(lanIp <= ntohl(addrE.s_addr) && lanIp >= ntohl(addrS.s_addr)){
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("DHCP地址池包含了LAN口地址"));
		return (-1);
    }
#if (MULTI_LAN == FYES)
	debug();
    lanIp1 = ntohl(profIf->ether.sta.ip2);
    if((lanIp1 <= ntohl(addrE.s_addr) && lanIp1 >= ntohl(addrS.s_addr))) {
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("DHCP地址池包含了LAN口地址"));
		return (-1);
    }

    lanIp2 = ntohl(profIf->ether.sta.ip3);
    if((lanIp2 <= ntohl(addrE.s_addr) && lanIp2 >= ntohl(addrS.s_addr))) {
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("DHCP地址池包含了LAN口地址"));
		return (-1);
    }

    lanIp3 = ntohl(profIf->ether.sta.ip4);
    if((lanIp3 <= ntohl(addrE.s_addr) && lanIp3 >= ntohl(addrS.s_addr))) {
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("DHCP地址池包含了LAN口地址"));
		return (-1);
    }
#endif
#else
	debug();
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 1);
    if( profIf->ether.connMode == ETH_CONN_STATIC)
    {/*只有静态ip类型判断。因动态类型无法判断*/
	lanIp = ntohl(profIf->ether.sta.ip);
	if(lanIp <= ntohl(addrE.s_addr) && lanIp >= ntohl(addrS.s_addr)){
	debug();
	    ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN);
	    setTheErrorMsg((char *)ErrMsg);
	    //setTheErrorMsg(T("DHCP地址池包含了LAN口地址"));
		return (-1);
	}
    }
#endif

	if( !strncmp(new_name,"default",7) ){
	debug();
		if((addrS.s_addr & addrNm.s_addr) != (htonl(lanIp) & addrNm.s_addr)){
		debug();
			ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_IP_LAN_IP_NOT_SAME_NETWORK);
			setTheErrorMsg((char *)ErrMsg);
			//setTheErrorMsg(T("DHCPPOOL地址和LANIP地址不在同一网络!"));1
			return (-1);
		}
	}

    if((addrS.s_addr & addrNm.s_addr) != (addrGw.s_addr & addrNm.s_addr)){
	debug();
	ErrMsg = getTransDataForC(C_LANG_INDEX_START_IP_GATEWAY_NOT_SAME_NETWORK);
	setTheErrorMsg((char *)ErrMsg);
        //setTheErrorMsg(T("起始地址和网关地址不在同一网络!"));
		return (-1);
    }

    if((addrS.s_addr & addrNm.s_addr) != (addrE.s_addr & addrNm.s_addr)){
	debug();
	ErrMsg = getTransDataForC(C_LANG_INDEX_START_IP_END_IP_NOT_SAME_NETWORK);
	setTheErrorMsg((char *)ErrMsg);
        //setTheErrorMsg(T("起始地址和结束地址不在同一网络"));
		return (-1);
    }

    nMaxIp = (addrS.s_addr & addrNm.s_addr) | (~(addrNm.s_addr));     //the max ip
	debug();
    if(addrE.s_addr >= nMaxIp) {
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_END_IP_TOO_BIG);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg (T("结束地址过大! "));
		return (-1);
    }

	debug();
	ProfInstLowHigh(mibType, &max, &min);
	ProfInstLowHigh(mibTypeV, &maxV, &minV);

	debug();
		for (index = min; index < max; index++) {
			prof = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibType, index);
			if (NULL != prof) {
				if (!strcmp(new_name, prof->head.name)) {
				debug();
					ErrMsg = getTransDataForC(C_LANG_INDEX_SAME_ACCOUNT_EXIST);
					setTheErrorMsg((char *)ErrMsg);
					//setTheErrorMsg(T("操作失败，系统中存在同名帐号"));
					return (-1);
				}

			}
		}
		/*vid是否冲突*/
		for (index = min; index < max; index++) {
			prof = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibType, index);
			if (NULL != prof) {
				if (prof->vid == vid) {
				debug();
					//ErrMsg = getTransDataForC(C_LANG_INDEX_SAME_VID_EXIST);
					//setTheErrorMsg((char *)ErrMsg);
					setTheErrorMsg(T("操作失败，系统中存在相同VID的地址池"));
					return (-1);
				}

			}
		}
#if (VLAN_CONFIG == FYES)
		profV = (VlanConfigProfile *)ProfGetInstPointByName(mibTypeV, dhcp_vid);
		if (profV != NULL) {
			if (profV->netmask != addrNm.s_addr) {
			debug();
			//		printf("%d in %s of %s\n", __LINE__, __func__, __FILE__);
					setTheErrorMsg(T("操作失败，网关地址与虚接口地址不符！"));
					return (-1);
			}

			if((addrS.s_addr & addrNm.s_addr) != (profV->ip & addrNm.s_addr)){
			debug();
//			ErrMsg = getTransDataForC(C_LANG_INDEX_START_IP_GATEWAY_NOT_SAME_NETWORK);
//			setTheErrorMsg((char *)ErrMsg);
			//		printf("%d in %s of %s\n", __LINE__, __func__, __FILE__);
					setTheErrorMsg(T("操作失败，地址池与虚接口地址不在同一网络！"));
			    //setTheErrorMsg(T("起始地址和网关地址不在同一网络!"));
					return (-1);
			}

			if((addrS.s_addr & addrNm.s_addr) != (profV->ip & addrNm.s_addr)){
			debug();
			    //setTheErrorMsg(T("起始地址和结束地址不在同一网络"));
			//		printf("%d in %s of %s\n", __LINE__, __func__, __FILE__);
					setTheErrorMsg(T("操作失败，地址池与虚接口地址不在同一网络！"));
					return (-1);
			}

			if(ntohl(profV->ip) <= ntohl(addrE.s_addr) && ntohl(profV->ip) >= ntohl(addrS.s_addr)){
			 debug();
//				ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN);
//				setTheErrorMsg((char *)ErrMsg);
				setTheErrorMsg(T("DHCP地址池包含了虚接口地址"));
				return (-1);
			 }
		}
#endif
		index = ProfNewInst(mibType, new_name, FALSE);
		profType = PROF_CHANGE_ADD;

	debug();
		prof = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibType, index);
		ProfBackupByIndex(mibType, PROF_CHANGE_EDIT , index, &profhead);
		if (NULL != prof) {
	debug();
			strcpy(prof->head.name, new_name);
			if(strcmp(dhcp_en,"true") == 0)
			{
				prof->dhcpEn = PROF_ENABLE;
			}else{
				prof->dhcpEn = PROF_DISABLE;
			}
			prof->vid = vid;
//			memset(prof->interface, '\0', sizeof(prof->interface));
//			sprintf(prof->interface, "%s", (char *)getLanIfName());
			if (prof->vid == 0) {
				strcpy(prof->interface,"LAN");
			} else {
				memset(tmp, 0, sizeof(tmp));
				sprintf(tmp, "VIF%d", prof->vid);
				strcpy(prof->interface, tmp);
			}
			prof->ipStart = addrS.s_addr;
			prof->ipEnd = addrE.s_addr;
			prof->netmask = addrNm.s_addr;
			prof->gateway = addrGw.s_addr;
			prof->lease = strtol(dhcp_l, NULL, 10);
			prof->pDns = inet_addr(dhcp_pd);
			prof->sDns = inet_addr(dhcp_sd);
		}

	debug();
#if (SERVICE_ZONE == FYES)
    ProfUpdate(profheadSz);
    ProfFreeAllocList(profheadSz);
#endif
#if (TAG_VLAN == FYES)
    ProfUpdate(profheadTv);
    ProfFreeAllocList(profheadTv);
#endif
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
	nvramWriteCommit();
	debug();

	return 0;
}
#endif
static int serviceZoneInit(webs_t wp, char_t *path, char_t *query)
{
	char *newName, *serviceEn, *ssid, *vif, *ssidBcast, *ssidIsolate,
		 *vid, *maxClt, *gw, *netmask, *dhcpEn, *dhcpPool, *securMode,*ssidEncodeType;
	char *vlanType, *apVlan;
	Uint32 apVidBits[128];
	int vlanT;
	int ret;
	struct wep_security_web_get wepSecurityWeb;
	struct wpa_security_web_get	wpaSecurityWeb;
	struct wpa_psk_security_web_get wpaPskSecurityWeb;
	int min, max, index, indexTmp;
	Uint32 vidVal = 0, maxCltVal, radiusPortVal = 0, wpaupVal = 0, wpaPskUpVal,encodeTypeVal;
	struct in_addr addrVif, addrGw, addrNm, addrRadius;
	struProfAlloc *profhead = NULL;
	struProfAlloc *profheadDhcp = NULL;
	MibProfileType mibType = MIB_PROF_SERVICE_ZONE;
	MibProfileType mibTypeDhcp = MIB_PROF_DHCP_POOL;
	ProfChangeType profType;
	ServiceZoneProfile *prof = NULL;
	DhcpPoolProfile *profDhcp = NULL;
	const char *ErrMsg = NULL;

	newName = websGetVar(wp, "FzoneName", T(""));
	serviceEn = websGetVar(wp, "szEn", T(""));
//	vif = websGetVar(wp, "ifAddr", T(""));
	ssid = websGetVar(wp, "ssid", T(""));
	ssidEncodeType = websGetVar(wp, "ssidEncodeType", T(""));
	ssidBcast = websGetVar(wp, "broadcastEn", T(""));
	ssidIsolate = websGetVar(wp, "isolateEn", T(""));
	vid = websGetVar(wp, "FvlanId", T(""));
	maxClt = websGetVar(wp, "max_user", T(""));
	gw = websGetVar(wp, "ipAddr", T(""));
//	netmask = websGetVar(wp, "maskAddr", T(""));
	dhcpEn = websGetVar(wp, "dhcpServer", T(""));
	dhcpPool = websGetVar(wp, "pool", T("default"));
	securMode = websGetVar(wp, "encryType", T(""));
	vlanType = websGetVar(wp, "vlanType", T(""));
	apVlan = websGetVar(wp, "apvlan", T(""));

	if('\0' == ssidEncodeType)
	{
        encodeTypeVal = 0;
	}else{
        encodeTypeVal =(Uint32)strtoul(ssidEncodeType, NULL, 10);
	}

	vlanT = (Uint16)strtoul(vlanType, NULL, 10);
	memset(apVidBits, 0, sizeof(apVidBits));

	if (strlen(newName) > (UTT_INST_NAME_LEN - 1)) {
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_SZ_NAME_OUT_OF_RANGE);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("服务区名称不能超过11个字节"));
		return (-1);
	}
	if (0 == inet_aton(gw, &addrGw)) {
	debug();
		ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_GATEWAY);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("无效网关"));
		return (-1);
	}
	wepSecurityWeb.auMethod = websGetVar(wp, "wepAuthType", T(""));
	wepSecurityWeb.kType = websGetVar(wp, "wepFormat", T(""));
	wepSecurityWeb.keyCh = websGetVar(wp, "wepRadio", T(""));
	wepSecurityWeb.key1_passwd = websGetVar(wp, "wepKey1Text", T(""));
	wepSecurityWeb.key1_kFmt = websGetVar(wp, "wepKey1Type", T(""));
	wepSecurityWeb.key2_passwd = websGetVar(wp, "wepKey2Text", T(""));
	wepSecurityWeb.key2_kFmt = websGetVar(wp, "wepKey2Type", T(""));
	wepSecurityWeb.key3_passwd = websGetVar(wp, "wepKey3Text", T(""));
	wepSecurityWeb.key3_kFmt = websGetVar(wp, "wepKey3Type", T(""));
	wepSecurityWeb.key4_passwd = websGetVar(wp, "wepKey4Text", T(""));
	wepSecurityWeb.key4_kFmt = websGetVar(wp, "wepKey4Type", T(""));

	wpaSecurityWeb.wVersion = websGetVar(wp, "wpaVersion", T(""));
	wpaSecurityWeb.sAlg = websGetVar(wp, "wpaEncrp", T(""));
	wpaSecurityWeb.radiusIp = websGetVar(wp, "radiusIP", T(""));
	wpaSecurityWeb.radiusPort = websGetVar(wp, "radiusPort", T(""));
	wpaSecurityWeb.radiusPasswd = websGetVar(wp, "radiusPsswd", T(""));
	wpaSecurityWeb.updataTime = websGetVar(wp, "wpaTime", T(""));

	wpaPskSecurityWeb.wVersion = websGetVar(wp, "pskVersion", T(""));
	wpaPskSecurityWeb.sAlg = websGetVar(wp, "pskEncry", T(""));
	wpaPskSecurityWeb.sharePasswd = websGetVar(wp, "pskPsswd", T(""));
	wpaPskSecurityWeb.updataTime = websGetVar(wp, "pskTime", T(""));
	wpaPskUpVal = (Uint32)strtoul(wpaPskSecurityWeb.updataTime, NULL, 10);

	if (!strcmp(securMode, "1")) {
/* WEP的密码检查交由页面来做控制 */
#if 0
		if (strlen(wepSecurityWeb.key1_passwd) > (UTT_PASSWD_LEN - 1) ||
			strlen(wepSecurityWeb.key2_passwd) > (UTT_PASSWD_LEN - 1) ||
			strlen(wepSecurityWeb.key3_passwd) > (UTT_PASSWD_LEN - 1) ||
			strlen(wepSecurityWeb.key4_passwd) > (UTT_PASSWD_LEN - 1)) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_PASSWORD_OUT_OF_RANGE);
			setTheErrorMsg((char *)ErrMsg);
			//setTheErrorMsg(T("密码长度不能超过11个字节"));
			goto out;
		}
#endif
	} else if (!strcmp(securMode, "2")) {
		radiusPortVal = (Uint16)strtoul(wpaSecurityWeb.radiusPort, NULL, 10);
		if (radiusPortVal > 65536 || radiusPortVal < 0) {
			debug();
			ErrMsg = getTransDataForC(C_LANG_INDEX_SERVICE_PORT_OUT_OF_RANGE);
			setTheErrorMsg((char *)ErrMsg);
			//setTheErrorMsg(T("端口应在0~65536之间"));
			return (-1);
		}
		if (0 == inet_aton(wpaSecurityWeb.radiusIp, &addrRadius)) {
			debug();
			ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_RADIUS_IP);
			setTheErrorMsg((char *)ErrMsg);
			//setTheErrorMsg(T("无效Radius服务器IP"));
			return (-1);
		}
		if (strlen(wpaSecurityWeb.radiusPasswd) > 31) {
			debug();
			ErrMsg = getTransDataForC(C_LANG_INDEX_RADIUS_PASSWORD_OUT_OF_RANGE);
			setTheErrorMsg((char *)ErrMsg);
			//setTheErrorMsg(T("Radius密码长度不能超过31个字节"));
			return (-1);
		}
		wpaupVal = (Uint32)strtoul(wpaSecurityWeb.updataTime, NULL, 10);
		if (wpaupVal > 86400 || (wpaupVal < 60 && wpaupVal != 0)) {
			debug();
			ErrMsg = getTransDataForC(C_LANG_INDEX_PASSWORD_PERIOD_OUT_OF_RANGE);
			setTheErrorMsg((char *)ErrMsg);
			//setTheErrorMsg(T("密码更新周期超出取值范围"));
			return (-1);
		}
	} else if (!strcmp(securMode, "3")) {
		if (wpaPskUpVal> 86400 || (wpaPskUpVal< 60 && wpaPskUpVal!= 0)) {
			debug();
			ErrMsg = getTransDataForC(C_LANG_INDEX_PASSWORD_PERIOD_OUT_OF_RANGE);
			setTheErrorMsg((char *)ErrMsg);
			//setTheErrorMsg(T("密码更新周期超出取值范围"));
			return (-1);
		}
	} else {
	}

	ProfInstLowHigh(mibType, &max, &min);

	// 无线vlan
	if (vlanT == 0) {
		if ('\0' == *vid) {
			vidVal = 0;
		} else {
			vidVal = (Uint16)strtoul(vid, NULL, 10);
		}
#if 0
		if (vidVal > 4094 || vidVal < 0 || ((0 == vidVal) && (strcmp(newName, "default")))) {
#else
		if (vidVal > 4094 || vidVal < 0 ) {
#endif
			debug();
			ErrMsg = getTransDataForC(C_LANG_INDEX_VLAN_ID_OUT_OF_RANGE);
			setTheErrorMsg((char *)ErrMsg);
			//setTheErrorMsg(T("VLAN ID超出取值范围"));
			return (-1);
		}

		// AP vlan
	} else if (vlanT == 1) {
		vidVal = 0;
		vif = 0;
		ret = parseVidStr(apVidBits , apVlan);

		if (ret != 0) {
			ErrMsg = getTransDataForC("AP管理vlan值错误");
			setTheErrorMsg((char *)ErrMsg);
			return (-1);
		}
	}

	/* add new an option*/
		debug();
		for (index = min; index < max; index++) {
			prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, index);
			if (NULL != prof) {
				if (!strcmp(newName, prof->head.name)) {
					debug();
					ErrMsg = getTransDataForC(C_LANG_INDEX_SZ_NAME_CONFLICT);
					setTheErrorMsg((char *)ErrMsg);
					//setTheErrorMsg("操作失败，服务区名称冲突");
					return (-1);
				}
				if (vlanT == 0) {
					if ((vidVal != 0) && (prof->vlanType == 0) && (vidVal == prof->vid)) {
						ErrMsg = getTransDataForC(C_LANG_INDEX_VLAN_ID_CONFLICT);
						setTheErrorMsg((char *)ErrMsg);
						//setTheErrorMsg("操作失败，VLAN ID冲突");
						return (-1);
					}
				} else {
					if (prof->vlanType == 1) {
						ret = judgeApBits(apVidBits, prof->apVidBits, MAX_VID_BITS_LEN);
						if (ret == -1) {
							debug();
							ErrMsg = getTransDataForC(C_LANG_INDEX_VLAN_ID_CONFLICT);
							setTheErrorMsg((char *)ErrMsg);
							//setTheErrorMsg("操作失败，VLAN ID冲突");
							return (-1);
						}
					}

				}
			}
		}

		if ((0 == strcmp(serviceEn, "1")) && (0 == strcmp(dhcpEn, "1"))) {
		debug();
			profDhcp = (DhcpPoolProfile *)ProfGetInstPointByName(mibTypeDhcp, dhcpPool);
			if (NULL != profDhcp) {
				if (PROF_DISABLE == profDhcp->dhcpEn) {
					ProfBackupByName(mibTypeDhcp, PROF_CHANGE_EDIT, dhcpPool, &profheadDhcp);
					profDhcp->dhcpEn = PROF_ENABLE;
					sprintf(profDhcp->interface, "%s.%d", (char *)getLanIfName(), vidVal);
				} else {
					debug();
					ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_POOL_USED);
					setTheErrorMsg((char *)ErrMsg);
					//setTheErrorMsg(T("操作失败,地址池已被使用"));
					return (-1);
				}
			}
		}

		index = ProfNewInst(mibType, newName, FALSE);
		profType = PROF_CHANGE_ADD;

	prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, index);
	if (NULL == prof) {
	debug();
		return (-1);
	}
	ProfBackupByIndex(mibType, profType, index, &profhead);

	strcpy(prof->head.name, newName);
	prof->head.active = TRUE;
	if (0 == strcmp(serviceEn, "1")) {
		prof->serviceEn = PROF_ENABLE;
	} else {
		prof->serviceEn = PROF_DISABLE;
	}
	strcpy(prof->ssid, ssid);
#if(SSID_ENCODE_TYPE == FYES)
    prof->ssid_encode_type = encodeTypeVal;
#endif
	if (0 == strcmp(ssidBcast, "1")) {
		prof->ssidBcast = PROF_ENABLE;
	} else {
		prof->ssidBcast = PROF_DISABLE;
	}
	if (0 == strcmp(ssidIsolate, "1")) {
		prof->ssidIsolate = PROF_ENABLE;
	} else {
		prof->ssidIsolate = PROF_DISABLE;
	}
	prof->vlanType = vlanT;
	prof->vid = vidVal;
	memcpy(prof->apVidBits, apVidBits, sizeof(apVidBits));
	if (vlanT == 0) {
		memset(prof->strApVid, 0, MAX_AP_VID_STR_LEN);
#if 0
		prof->virif = addrVif.s_addr;
		prof->netmask = addrNm.s_addr;
#endif
	} else {
		memcpy(prof->strApVid, apVlan, MAX_AP_VID_STR_LEN);
		prof->virif = 0u;
		if (index != 0) {
			prof->netmask = 0u;
		}
	}
	maxCltVal = (Uint16)strtoul(maxClt, NULL, 10);
	prof->client = maxCltVal;
	prof->gw = addrGw.s_addr;
	if (0 == strcmp(securMode, "1")) {
		prof->securMode = WEP;

		if (0 == strcmp(wepSecurityWeb.auMethod, "0")) {
			prof->wepSecurity.auMethod = AUTO_AUTH;
		} else if (0 == strcmp(wepSecurityWeb.auMethod, "1")) {
			prof->wepSecurity.auMethod = OPEN_SYS;
		} else {
			prof->wepSecurity.auMethod = SHARE_KEY;
		}

		if (0 == strcmp(wepSecurityWeb.kType, "1")) {
			prof->wepSecurity.kType = HEX;
		} else {
			prof->wepSecurity.kType = ASCII;
		}

		if (0 == strcmp(wepSecurityWeb.keyCh, "3")) {
			prof->wepSecurity.kNum1.key_state_action = PROF_DISABLE;
			prof->wepSecurity.kNum2.key_state_action = PROF_DISABLE;
			prof->wepSecurity.kNum3.key_state_action = PROF_DISABLE;
			prof->wepSecurity.kNum4.key_state_action = PROF_ENABLE;
		} else if (0 == strcmp(wepSecurityWeb.keyCh, "2")) {
			prof->wepSecurity.kNum1.key_state_action = PROF_DISABLE;
			prof->wepSecurity.kNum2.key_state_action = PROF_DISABLE;
			prof->wepSecurity.kNum3.key_state_action = PROF_ENABLE;
			prof->wepSecurity.kNum4.key_state_action = PROF_DISABLE;
		} else if (0 == strcmp(wepSecurityWeb.keyCh, "1")) {
			prof->wepSecurity.kNum1.key_state_action = PROF_DISABLE;
			prof->wepSecurity.kNum2.key_state_action = PROF_ENABLE;
			prof->wepSecurity.kNum3.key_state_action = PROF_DISABLE;
			prof->wepSecurity.kNum4.key_state_action = PROF_DISABLE;
		} else {
			prof->wepSecurity.kNum1.key_state_action = PROF_ENABLE;
			prof->wepSecurity.kNum2.key_state_action = PROF_DISABLE;
			prof->wepSecurity.kNum3.key_state_action = PROF_DISABLE;
			prof->wepSecurity.kNum4.key_state_action = PROF_DISABLE;
		}

		strcpy(prof->wepSecurity.kNum1.passwd, wepSecurityWeb.key1_passwd);
		if (0 == strcmp(wepSecurityWeb.key1_kFmt, "0")) {
			prof->wepSecurity.kNum1.kFmt = FORBID;
		} else if (0 == strcmp(wepSecurityWeb.key1_kFmt, "1")) {
			prof->wepSecurity.kNum1.kFmt = BIT_64;
		} else {
			prof->wepSecurity.kNum1.kFmt = BIT_128;
		}
		strcpy(prof->wepSecurity.kNum2.passwd, wepSecurityWeb.key2_passwd);
		if (0 == strcmp(wepSecurityWeb.key2_kFmt, "0")) {
			prof->wepSecurity.kNum2.kFmt = FORBID;
		} else if (0 == strcmp(wepSecurityWeb.key2_kFmt, "1")) {
			prof->wepSecurity.kNum2.kFmt = BIT_64;
		} else {
			prof->wepSecurity.kNum2.kFmt = BIT_128;
		}
		strcpy(prof->wepSecurity.kNum3.passwd, wepSecurityWeb.key3_passwd);
		if (0 == strcmp(wepSecurityWeb.key3_kFmt, "0")) {
			prof->wepSecurity.kNum3.kFmt = FORBID;
		} else if (0 == strcmp(wepSecurityWeb.key3_kFmt, "1")) {
			prof->wepSecurity.kNum3.kFmt = BIT_64;
		} else {
			prof->wepSecurity.kNum3.kFmt = BIT_128;
		}
		strcpy(prof->wepSecurity.kNum4.passwd, wepSecurityWeb.key4_passwd);
		if (0 == strcmp(wepSecurityWeb.key4_kFmt, "0")) {
			prof->wepSecurity.kNum4.kFmt = FORBID;
		} else if (0 == strcmp(wepSecurityWeb.key4_kFmt, "1")) {
			prof->wepSecurity.kNum4.kFmt = BIT_64;
		} else {
			prof->wepSecurity.kNum4.kFmt = BIT_128;
		}
	} else if (0 == strcmp(securMode, "2")) {
		prof->securMode = WPA1_AND_WPA2;

		if (0 == strcmp(wpaSecurityWeb.wVersion, "0")) {
			prof->wpaSecurity.wVersion = AUTO_WPA;
		} else if (0 == strcmp(wpaSecurityWeb.wVersion, "1")) {
			prof->wpaSecurity.wVersion = WPA;
		} else {
			prof->wpaSecurity.wVersion = WPA2;
		}
		if (0 == strcmp(wpaSecurityWeb.sAlg, "0")) {
			prof->wpaSecurity.sAlg = AUTO_SEC;
		} else if (0 == strcmp(wpaSecurityWeb.sAlg, "1")) {
			prof->wpaSecurity.sAlg = TKIP;
		} else {
			prof->wpaSecurity.sAlg = AES;
		}
		prof->wpaSecurity.radiusIp = addrRadius.s_addr;
		prof->wpaSecurity.radiusPort = radiusPortVal;
		strcpy(prof->wpaSecurity.radiusPasswd,wpaSecurityWeb.radiusPasswd);
		prof->wpaSecurity.updataTime = wpaupVal;
	} else if (0 == strcmp(securMode, "3")) {
		prof->securMode = WPA1_PSK_AND_WPA2_PSK;

		if (0 == strcmp(wpaPskSecurityWeb.wVersion, "0")) {
			prof->wpaPskSecurity.wVersion = AUTO_WPA;
		} else if (0 == strcmp(wpaPskSecurityWeb.wVersion, "1")) {
			prof->wpaPskSecurity.wVersion = WPA;
		} else {
			prof->wpaPskSecurity.wVersion = WPA2;
		}
		if (0 == strcmp(wpaPskSecurityWeb.sAlg, "0")) {
			prof->wpaPskSecurity.sAlg = AUTO_SEC;
		} else if (0 == strcmp(wpaPskSecurityWeb.sAlg, "1")) {
			prof->wpaPskSecurity.sAlg = TKIP;
		} else {
			prof->wpaPskSecurity.sAlg = AES;
		}
		strcpy(prof->wpaPskSecurity.sharePasswd, wpaPskSecurityWeb.sharePasswd);
		prof->wpaPskSecurity.updataTime = wpaPskUpVal;
	} else {
		prof->securMode = NONE;
	}

	if (0 == strcmp(dhcpEn, "1")) {
		prof->dhcpEn = PROF_ENABLE;
	} else {
		prof->dhcpEn = PROF_DISABLE;
	}
	strcpy(prof->dhcpPool, dhcpPool);

	if (NULL != profhead) {
		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
	}

	if (NULL != profheadDhcp) {
		ProfUpdate(profheadDhcp);
		ProfFreeAllocList(profheadDhcp);
	}
	nvramWriteCommit();

	return 0;
}
static void formConfigFastDirectionAC(webs_t wp, char_t *path, char_t *query)
{
	char *acMode=NULL,*ConnMode=NULL,*Action=NULL,*Flag1=NULL,*Flag2=NULL,*Flag3=NULL,*Flag4=NULL,*Flag5=NULL,*Flag6=NULL;
	InterfaceProfile *profIf=NULL;
	int ret,actionVal = 0,tmp;

    Action = websGetVar(wp, T("pageAction"), T(""));
	actionVal = (int)strtoul(Action, 0, 10);

    Flag1 = websGetVar(wp, T("modifyFlag1"), T(""));
    Flag2 = websGetVar(wp, T("modifyFlag2"), T(""));
    Flag3 = websGetVar(wp, T("modifyFlag3"), T(""));
    Flag4 = websGetVar(wp, T("modifyFlag4"), T(""));
    Flag5 = websGetVar(wp, T("modifyFlag5"), T(""));
    Flag6 = websGetVar(wp, T("modifyFlag6"), T(""));
	/*网关模式*/
	if((actionVal >= 2)&&(strcmp(Flag1,"change")==0))
	{
		ret = workModeInit(wp, path, query);
		if( ret != 0 )
		{
	        websRedirect(wp, "apManage.asp");
			return;
		}
		/*WAN1口配置*/
		if((actionVal >= 3)&&(strcmp(Flag2,"change")==0))
		{
		    acMode = websGetVar(wp, T("acMode"), T(""));
			tmp = (int)strtoul(acMode, 0, 10);
			if(tmp == 0)/*0为网关模式，1为旁路模式*/
			{
				ConnMode=websGetVar(wp, T("connectionType"), T(""));
				profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 1);
				if(strlen(ConnMode)){
					setWanx(wp, 1, "", ConnMode, profIf);
				}
				initWan(1);
			}
		}
	}

	/*点击页面上的快捷按钮直接配置WAN1口，单独处理*/
	if((actionVal == 3)&&(strcmp(Flag2,"change")==0)&&(strcmp(Flag1,"change") != 0))
	{
		ConnMode=websGetVar(wp, T("connectionType"), T(""));
		profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 1);
		if(strlen(ConnMode))
		{
			setWanx(wp, 1, "", ConnMode, profIf);
		}
		initWan(1);
	}

	/*虚接口配置*/
	if(( actionVal >= 4)&&(strcmp(Flag3,"change")==0))
	{
		ret = vlanConfigInit(wp,path,query);
		if( ret != 0 )
		{
	        websRedirect(wp, "apManage.asp");
			return;
		}
	}
#if (DHCP_POOL ==FYES)
	/*DHCP服务器配置*/
	if((actionVal >= 5)&&(strcmp(Flag4,"change")==0))
	{
		ret = dhcpPoolInit(wp, path, query);
		if( ret != 0 )
		{
	        websRedirect(wp, "apManage.asp");
			return;
		}
	}
#endif

	/*服务区配置*/
	if((actionVal >= 6)&&(strcmp(Flag5,"change")==0))
	{
		ret = serviceZoneInit(wp, path, query);
		if( ret != 0 )
		{
			websRedirect(wp, "apManage.asp");
			return;
		}
	}

	websRedirect(wp, "firstPage.asp");
	return;
}
#endif
static int aspFastConfigPopWelcome(int eid, webs_t wp, int argc, char_t **argv)
{
    SystemProfile *profSys = NULL;
    profSys = (SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    websWrite(wp, "var notPopUps=\"%d\";", !(profSys->popWelcome));
    return 0;
}

static int aspFastConfigNet(int eid, webs_t wp, int argc, char_t **argv)
{
    int wanIndex = 0, i;
    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;
    if ((i=ejArgs(argc, argv, T("%d"), &wanIndex)) < 1) {
        return websWrite(wp, T("Insufficient args\n"));
    }

    if(wanIndex > 0 && wanIndex <= getWanSlotCount()) {
        profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, wanIndex);
        websWrite(wp, "var ConnTypes=\"%s\";", 
                MibGetStrByEnum(profIf->ether.connMode, ethConnModeEnum));
        websWrite(wp, "var Masks=\"%N\";", profIf->ether.sta.nm);
        websWrite(wp, "var IPs=\"%N\";", profIf->ether.sta.ip);
        websWrite(wp, "var GateWays=\"%N\";", profIf->ether.sta.gw);
        websWrite(wp, "var MainDnss=\"%N\";", profIf->ether.sta.pd);
        websWrite(wp, "var SecDnss=\"%N\";", profIf->ether.sta.sd);
        websWrite(wp, "var UserNames=\"%s\";", profIf->ether.pppoe.user);
        websWrite(wp, "var PassWds=\"%s\";", profIf->ether.pppoe.passwd);
    }
    return 0;
}

#if (WIRELESS == FYES)
#if (WAN_3G_SWITCH  == FYES )
/*
 * get_usb_device - 获取是否存在能够识别的3G卡
 *
 * 现在是需要读取proc文件下的usb状态查看是否存在能够识别的3G卡
 * 3G卡未插和不能识别均显示未识别
 *
 * 返回值：
 *  0或1     3G卡未插或不能识别
 *  2     存在能够识别的3G卡
 */

static int get_usb_device(void)
{
    FILE* fp = NULL;
    char *p = NULL;
    char command[100];
    char buffer[256];
    int count = 0;

    /*数组初始化*/
    memset(command, 0, sizeof(command));
    memset(buffer, 0, sizeof(buffer));

    strcpy(command, "cat /proc/bus/usb/devices");
    fp = popen(command, "r");/*执行命令*/
    if (fp != NULL) {/*执行成功*/
	while (fgets(buffer, sizeof(buffer), fp) != NULL) {
	    p = strstr(buffer, "Vendor=");
	    if (p != NULL) {
		count++;
	    }
	}
	pclose(fp);
    } 
    return count;
}
/*
 * get_3g_status - 获取3G状态
 *
 * 现在是需要读取proc文件下的usb状态查看是否有3G卡插上
 * 然后通过判断是否有拨号进程来判断3G拨号状态
 *
 * 输入参数：
 *  index WAN口对应3G索引
 * 返回值：
 *  0     未识别
 *  1     已识别
 *  2     正在拨号
 *  3     拨号失败
 *  4     已连接
 */
static int get_3g_status(int index)
{
    int i = 0;
    FILE* fp = NULL;
    int wan_3g_status = 0;/*未识别*/
    char ProdID[10];
    char Vendor[10];
    char buffer[256];
    char process_name[50];
    char command[100];
    char ip[16];


    /*数组初始化*/
    memset(ProdID, 0, sizeof(ProdID));
    memset(Vendor, 0, sizeof(Vendor));
    memset(buffer, 0, sizeof(buffer));
    memset(process_name, 0, sizeof(process_name));
    memset(command, 0, sizeof(command));
    memset(ip, 0, sizeof(ip));

    if (getIfIp(getWanIfNamePPP(index), ip) == 0) { 
	wan_3g_status = 4;/*已连接*/
    } else if ((get_usb_device() == 2) || (get_usb_device() == 3)) {
	wan_3g_status = 1;/*已识别*/
	memset(buffer, 0, sizeof(buffer));
	strcpy(process_name, "pppd file /etc/3g");
	sprintf(command, "ps -ef | grep '%s' | grep -v grep | wc -l",process_name);
	fp = popen(command, "r");/*执行命令*/
	if (fp != NULL) {/*执行成功*/
	    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
		i = atoi(buffer);
	    }
	    pclose(fp);
	}
	if (i == 1) {
	    wan_3g_status = 2;/*正在拨号*/
	} else {
	    wan_3g_status = 3;/*拨号失败*/
	    printf("Dial num of process wrong:%s\n",process_name);
	}
    } else {
	/*test*/
    }
    return wan_3g_status;
}
#endif
/*
 * 输出3G配置信息
 */
static int aspOutput3GConfig(int eid, webs_t wp, int argc, char_t **argv) {
    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;
    int index;

    index = get3GSlotIndex();
#if (WAN_3G_SWITCH  == FYES )
    websWrite(wp, T("var status_3g_wan = %d;"), get_3g_status(index));
#else
    websWrite(wp, T("var status_3g_wan = -1;"));
#endif
    if (index != -1) {
	profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, index);
	if (profIf != NULL) {
	    websWrite(wp, T("var Dev3Gs = \"%s\";"), profIf->ether.wan3g.wan_3g_dev);
	    websWrite(wp, T("var ISP3Gs = \"%s\";"), MibGetStrByEnum(profIf->ether.wan3g.wan_3g_isp, ispMode3G));
	    websWrite(wp, T("var AuthType3Gs = \"%s\";"), MibGetStrByEnum(profIf->ether.wan3g.wan_3g_authtype, authType3G));
	    websWrite(wp, T("var PinCodes = \"%s\";"), profIf->ether.wan3g.wan_3g_pincode);
	    websWrite(wp, T("var AuthNames = \"%s\";"), profIf->ether.wan3g.wan_3g_authname);
	    websWrite(wp, T("var AuthPasswds = \"%s\";"), profIf->ether.wan3g.wan_3g_authpasswd);
	    websWrite(wp, T("var ConnNames = \"%s\";"), profIf->ether.wan3g.wan_3g_connname);
	    websWrite(wp, T("var DialNums = \"%s\";"), profIf->ether.wan3g.wan_3g_dialnum);
	    websWrite(wp, T("var PPPNames = \"%s\";"), profIf->ether.wan3g.wan_3g_pppname);
	    websWrite(wp, T("var PPPPasswds = \"%s\";"), profIf->ether.wan3g.wan_3g_ppppasswd);
	}
    }
    /* 3G接口和AP Client接口 */
    websWrite(wp, T("var slot3G = %d;"), get3GSlotIndex());
    websWrite(wp, T("var slotAPCli = %d;"), getAPCliSlotIndex());
    return 0;
}

/*
 * FastConfig_OpMode.asp页面输出
 */
static int aspOutputFastOpMode(int eid, webs_t wp, int argc, char_t **argv) {
    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;
    int i, arrayIndex;
    char_t *ConnTypes=T("ConnTypes[%d]=\"%s\";\n");

    websWrite(wp,"var ConnTypes=new Array();\n");
    for(i = 1; i <= getWanSlotCount(); i++) {
	arrayIndex = i - 1;
        profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, i);
        if(profIf->head.active == PROF_ENABLE) {
#if 0
	    /* 3G接口，输出3G连接类型 */
	    if (i == get3GSlotIndex()) {
		websWrite(wp, ConnTypes, arrayIndex, "3G");
	    } else 
#endif
	    {
		websWrite(wp, ConnTypes, arrayIndex, 
			MibGetStrByEnum(profIf->ether.connMode, ethConnModeEnum));
	    }
        } else {
            websWrite(wp, ConnTypes, arrayIndex, "");
        }
    }

    /* 3G接口和AP Client接口 */
    websWrite(wp, T("var slot3G = %d;"), get3GSlotIndex());
    websWrite(wp, T("var slotAPCli = %d;"), getAPCliSlotIndex());

    return 0;
}
/*
 *输出可接入方式种类 
 */
static int aspOutputConnNum(int eid, webs_t wp, int argc, char_t **argv) {
	if(get3GSlotIndex()>0)
		websWrite(wp,T("var connNum = 2;"));
	else
		websWrite(wp,T("var connNum = 1;"));
}
#else
static int aspOutput3GConfig(int eid, webs_t wp, int argc, char_t **argv) {
    websWrite(wp, T("var status_3g_wan = -1;"));
    websWrite(wp, T("var Dev3Gs = \"\";"));
    websWrite(wp, T("var ISP3Gs = \"\";"));
    websWrite(wp, T("var AuthType3Gs = \"\";"));
    websWrite(wp, T("var PinCodes = \"\";"));
    websWrite(wp, T("var AuthNames = \"\";"));
    websWrite(wp, T("var AuthPasswds = \"\";"));
    websWrite(wp, T("var ConnNames = \"\";"));
    websWrite(wp, T("var DialNums = \"\";"));
    websWrite(wp, T("var PPPNames = \"\";"));
    websWrite(wp, T("var PPPPasswds = \"\";"));
    /* 3G接口和AP Client接口 */
    websWrite(wp, T("var slot3G = %d;"), -1);
    websWrite(wp, T("var slotAPCli = %d;"), -1);
    return 0;
}
/*
 *输出可接入方式种类 
 */
static int aspOutputConnNum(int eid, webs_t wp, int argc, char_t **argv) {
		websWrite(wp,T("var connNum = 1;"));
}
#endif

#if (UTT_SMART_UI == FYES)
/*新风格UI  读取WAN口配置到页面*/
static int aspWanConfig_Sui(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
    struct in_addr addrS;
    char *usermac = NULL;
    MacAddr macAddr;

    profIF = (InterfaceProfile*)ProfGetInstPointByIndex(mibTypeIF, 1);
    if (ProfInstIsFree(profIF) == PROFOK)
    {
	websWrite(wp, "ConnTypes=\"%s\";", MibGetStrByEnum(profIF->ether.connMode, ethConnModeEnum));
	websWrite(wp, "IPs=\"%N\";", profIF->ether.sta.ip);
	websWrite(wp, "Masks=\"%N\";", profIF->ether.sta.nm);
	websWrite(wp, "GateWays=\"%N\";", profIF->ether.sta.gw);
	websWrite(wp, "MainDnss=\"%N\";", profIF->ether.sta.pd);
	websWrite(wp, "SecDnss=\"%N\";", profIF->ether.sta.sd);
	websWrite(wp, "mac=\"%s\";", converMac6To12Byte(profIF->ether.mac));
	websWrite(wp, "UserNames=\"%s\";", profIF->ether.pppoe.user);
	websWrite(wp, "PassWds=\"%s\";", profIF->ether.pppoe.passwd);
	websWrite(wp, "ppp_pppoeOPModes=\"%s\";", MibGetStrByEnum(profIF->ether.pppoe.pppConnMode, pppConnModeEnum));
	websWrite(wp, "ppp_pppoeDailModes=\"%s\";", MibGetStrByEnum(profIF->ether.pppoe.spDial, pppSpDailEnum));
	websWrite(wp, "ppp_AuthTypess=\"%s\";", MibGetStrByEnum(profIF->ether.pppoe.authType, pppAuthEnum));
	websWrite(wp, "ppp_MTUs=\"%d\";", profIF->ether.pppoe.mtu);
	if(inet_aton(wp->ipaddr, &addrS) != 0)/*转化ip地址*/ 
	{
#if (MULTI_LAN == FYES)
	    if (isIpInLanNet(wp->ipaddr) != 0)
#else
	    if (isIpInIfNet(addrS.s_addr, getLanIfName()) != 0)
#endif	
	    {
		((struct sockaddr_in*)&arpLan.arp_pa)->sin_addr.s_addr = addrS.s_addr;
		memset(arpLan.arp_ha.sa_data, 0, sizeof(arpLan.arp_ha.sa_data));
		usermac = getMacByIpIoctl(&arpLan);/*通过arp表查询当前ip的对应mac地址*/
		if (usermac != NULL)
		{
		    memcpy(&macAddr, usermac, 6);
		    websWrite(wp, "usermac=\"%s\";", converMac6To12Byte(macAddr));
		}
	    }
	}
    }
    getTheErrorMsg(eid, wp, argc, argv);
#if (UTT_SMART_UI == FYES)
    getTheRightMsg(eid, wp, argc, argv);
#endif
    return 0;
}
#endif

static int aspWanConfig(int eid, webs_t wp, int argc, char_t **argv)
{
    int i = 0;
    int arrayIndex = 0;
    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;
    char_t *sta_Masks=T("sta_Masks[%d]=\"%N\";\n");
    char_t *sta_IPs=T("sta_IPs[%d]=\"%N\";\n");
    char_t *sta_GateWays=T("sta_GateWays[%d]=\"%N\";\n");
    char_t *sta_MainDnss=T("sta_MainDnss[%d]=\"%N\";\n");
    char_t *sta_SecDnss=T("sta_SecDnss[%d]=\"%N\";\n");
    char_t *ppp_UserNames=T("ppp_UserNames[%d]=\"%s\";\n");
    char_t *ppp_Passwds=T("ppp_Passwds[%d]=\"%s\";\n");
    char_t *ppp_AuthTypess=T("ppp_AuthTypess[%d]=\"%s\";\n");
    char_t *ppp_pppoeOPModes=T("ppp_pppoeOPModes[%d]=\"%s\";\n");
    char_t *ppp_pppoeDailModes=T("ppp_pppoeDailModes[%d]=\"%s\";\n");
    char_t *ppp_pppoeIdleTimes=T("ppp_pppoeIdleTimes[%d]=\"%d\";\n");
    char_t *ppp_MTUs=T("ppp_MTUs[%d]=\"%d\";\n");
    char_t *ISPTypes=T("ISPTypes[%d]=\"%d\";\n");
    char_t *ISPTypes1=T("ISPTypes1[%d]=\"%d\";\n");
    char_t *ISPTypes2=T("ISPTypes2[%d]=\"%d\";\n");
    char_t *PortNames=T("PortNames[%d]=\"%d\";\n");
    char_t *ConnTypes=T("ConnTypes[%d]=\"%s\";\n");
    char_t *mac=T("mac[%d]=\"%s\";\n");
    char_t *GateWayMac=T("GateWayMac[%d]=\"%s\";\n");
    char_t *BindWanEn=T("BindWanEn[%d]=\"%s\";\n");
    char_t *lively=T("lively[%d]=\"%d\";\n");
#if (CAPWAP_CONFIG_AP == FYES)
    char_t *ac_ip=T("ac_ip%d=\"%s\";\n");
#endif
#if 1
#if (PORT_CONF_REDUCED == FYES)
    char_t *wanMode=T("wanMode[%d]=%d;\n");
#endif
#endif
    char_t *maxMode=T("maxSpeed[%d]=%d;\n");
    websWrite(wp,"var mac=new Array();\n");
    websWrite(wp,"var GateWayMac=new Array();\n");
    websWrite(wp,"var BindWanEn=new Array();\n");
    websWrite(wp,"var sta_Masks=new Array();\n");
    websWrite(wp,"var sta_IPs=new Array();\n");
    websWrite(wp,"var sta_GateWays=new Array();\n");
    websWrite(wp,"var sta_MainDnss=new Array();\n");
    websWrite(wp,"var sta_SecDnss=new Array();\n");
    websWrite(wp,"var ppp_UserNames=new Array();\n");
    websWrite(wp,"var ppp_Passwds=new Array();\n");
    websWrite(wp,"var ppp_AuthTypess=new Array();\n");
    websWrite(wp,"var ppp_pppoeOPModes=new Array();\n");
    websWrite(wp,"var ppp_pppoeDailModes=new Array();\n");
    websWrite(wp,"var ppp_pppoeIdleTimes=new Array();\n");
    websWrite(wp,"var ppp_MTUs=new Array();\n");
    websWrite(wp,"var ISPTypes=new Array();\n");
    websWrite(wp,"var ISPTypes1=new Array();\n");
    websWrite(wp,"var ISPTypes2=new Array();\n");
    websWrite(wp,"var i_flag=new Array();\n");
    websWrite(wp,"var GateWayMac_scan=new Array();\n");
    websWrite(wp,"var PortNames=new Array();\n");
    websWrite(wp,"var ConnTypes=new Array();\n");
	websWrite(wp, "var lively=new Array();\n");
#if (CAPWAP_CONFIG_AP == FYES)
    websWrite(wp, "var ac_ip=new Array();\n");
    websWrite(wp, "var man_dns=new Array();\n");
    websWrite(wp, "var sec_dns=new Array();\n");
    websWrite(wp, "var ac_ip_flag=%d;\n",1);
#else
    websWrite(wp, "var ac_ip_flag=%d;\n",0);
#endif
#if 1
#if (PORT_CONF_REDUCED == FYES)
    websWrite(wp,"var wanMode=new Array();\n");
#endif
#endif
    websWrite(wp,"var maxSpeed=new Array();\n");
    for(i = 1; i <= getWanSlotCount(); i++) {
        arrayIndex = i - 1;
        profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, i);
        websWrite(wp, PortNames, arrayIndex, i);
        websWrite(wp, ISPTypes, arrayIndex, profIf->isp);
        websWrite(wp, ISPTypes1, arrayIndex, profIf->isp1);
        websWrite(wp, ISPTypes2, arrayIndex, profIf->isp2);
        if(profIf->head.active == PROF_ENABLE) {
#if (WIRELESS == FYES)
	    /* 3G接口，输出3G连接类型 */
	    if (i == get3GSlotIndex()) {
		websWrite(wp, ConnTypes, arrayIndex, "3G");
	    } else 
#endif
	    {
		websWrite(wp, ConnTypes, arrayIndex, 
			MibGetStrByEnum(profIf->ether.connMode, ethConnModeEnum));
	    }
        } else {
            websWrite(wp, ConnTypes, arrayIndex, "");
        }
//        websWrite(wp, mac, arrayIndex, profIf->ether.mac);
#if (NAT_GLOBAL_ENABLE == FYES)        
        websWrite(wp, lively, arrayIndex, profIf->NatActive);
#endif
        websWrite(wp, mac, arrayIndex, converMac6To12Byte(profIf->ether.mac));
        websWrite(wp, GateWayMac, arrayIndex, converMac6To12Byte(profIf->GateWayMac));
	if(profIf->BindWanEn == FUN_ENABLE){
	    websWrite(wp, BindWanEn, arrayIndex, "MANUAL");
	    websWrite(wp, GateWayMac, arrayIndex, converMac6To12Byte(profIf->GateWayMac));
	}else{
	    websWrite(wp, BindWanEn, arrayIndex, "NONE");
	}
	websWrite(wp," i_flag[%d] = 0;\n",arrayIndex);
//        websWrite(wp," GateWayMac_scan[0] = "";\n");
	readScanArp(eid,wp,argc,argv);
		
#if 1
#if (PORT_CONF_REDUCED == FYES)
#if (WIRELESS == FYES)
    if ((i == getAPCliSlotIndex()) || (i == get3GSlotIndex())) {
	//websWrite(wp, wanMode,arrayIndex,-1);
	websWrite(wp, maxMode,arrayIndex,-1);
    }else
#endif
    {
#ifdef CONFIG_SWITCH_EXT
#if (UTT_NV_3520G == FYES)
	if(MAX_SPDMODE_RDD==10){
	    websWrite(wp, maxMode, arrayIndex, 10);
	}else if(MAX_SPDMODE_RDD==100){
	    websWrite(wp, maxMode, arrayIndex, 100);
	}else if(MAX_SPDMODE_RDD==1000){
	    websWrite(wp, maxMode, arrayIndex, 1000);
	}else{
	    websWrite(wp, maxMode,arrayIndex,-1);
	}
	websWrite(wp, wanMode,arrayIndex,profIf->ether.speedMode);
#else
	websWrite(wp, maxMode, arrayIndex, -1);
#endif
#else
	if(MAX_SPDMODE_RDD==10){
	    websWrite(wp, maxMode, arrayIndex, 10);
	}else if(MAX_SPDMODE_RDD==100){
	    websWrite(wp, maxMode, arrayIndex, 100);
	}else if(MAX_SPDMODE_RDD==1000){
	    websWrite(wp, maxMode, arrayIndex, 1000);
	}else{
	    websWrite(wp, maxMode,arrayIndex,-1);
	}
	websWrite(wp, wanMode,arrayIndex,profIf->ether.speedMode);
#endif
    }
#else
	//websWrite(wp, wanMode,arrayIndex,-1);
	websWrite(wp, maxMode,arrayIndex,-1);

#endif
#endif
        websWrite(wp, sta_Masks, arrayIndex, profIf->ether.sta.nm);
        websWrite(wp, sta_IPs, arrayIndex, profIf->ether.sta.ip);
        websWrite(wp, sta_GateWays, arrayIndex, profIf->ether.sta.gw);
        websWrite(wp, sta_MainDnss, arrayIndex, profIf->ether.sta.pd);
        websWrite(wp, sta_SecDnss, arrayIndex, profIf->ether.sta.sd);
        websWrite(wp, ppp_UserNames, arrayIndex, profIf->ether.pppoe.user);
        websWrite(wp, ppp_Passwds, arrayIndex, profIf->ether.pppoe.passwd);
        websWrite(wp, ppp_AuthTypess, arrayIndex, 
                MibGetStrByEnum(profIf->ether.pppoe.authType, pppAuthEnum));
        websWrite(wp, ppp_pppoeOPModes, arrayIndex, 
                MibGetStrByEnum(profIf->ether.pppoe.pppConnMode, pppConnModeEnum));
        websWrite(wp, ppp_pppoeDailModes, arrayIndex, 
                MibGetStrByEnum(profIf->ether.pppoe.spDial, pppSpDailEnum));
        websWrite(wp, ppp_pppoeIdleTimes, arrayIndex, profIf->ether.pppoe.idleTime);
        websWrite(wp, ppp_MTUs, arrayIndex, profIf->ether.pppoe.mtu);
    }
#if (CAPWAP_CONFIG_AP == FYES)
#if 0
    ApBasicConfProfile *profab = NULL;
    MibProfileType mibTypeab = MIB_PROF_AP_BASIC_CONF;
    int j = 0;
    profab = (ApBasicConfProfile*)ProfGetInstPointByIndex(mibTypeab,0);
    if (profab != NULL)
    {
	for (j = 0; j < 3; j++)
	{
	    websWrite(wp, ac_ip, j, profab->acHostInfo[j]);
	}
    }
#else
    ApCapwapConfigProfile *profac = NULL;
    MibProfileType mibTypeac = MIB_PROF_AP_CAPWAP_CONFIG;
    int j = 0;
    unsigned int ip;
    unsigned int mask;
    char *name = NULL;
    
    profac = (ApCapwapConfigProfile*)ProfGetInstPointByIndex(mibTypeac,0);
    if (profac != NULL)
    {
	for (j = 0; j < 3; j++)
	{
	    if (profac->head.active == TRUE)
	    {
		websWrite(wp, ac_ip, j, profac->acHostInfo[j]);
	    }
	    else
	    { 
		websWrite(wp, ac_ip, j, "");
	    }
	}
	if (profac->head.active == TRUE)
	{
	    websWrite(wp, "man_dns = \"%N\";\n", profac->pd);
	    websWrite(wp, "sec_dns = \"%N\";\n", profac->sd);
	}
    }
    name = getLanIfName();
    get_ip(name, &ip);
    websWrite(wp, T("var ipAddr=\"%N\";\n"), ip);
    get_mask(name, &mask);
    websWrite(wp, T("var subMask = \"%N\";\n"), mask);

#endif
#endif
    websWrite(wp, "var totalrecs=%d;", i-1);
    getTheErrorMsg(eid, wp, argc, argv);
    getTheRightMsg(eid, wp, argc, argv);
    return 0;
}

/***********************************************************************
 * 函 数 名：   getConnectTime
 * 功能描述：   返回连接时间
 * 输入参数:    
 * 输出参数：	
 * 返回值：	
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       王炳荣	
 * 附加说明：	无
***********************************************************************/

static time_t getUptimeWanx(int wanIndex)
{
    FILE*fp;
    time_t time;
    char buf[64] = {0};
    sprintf(buf, "/var/run/wan%d.uptime", wanIndex);
    fp=fopen(buf, "r");
    if(!fp) {
       return 0;
    } else if(fscanf(fp,"%ld",&time)==1) {
        fclose(fp);
        return time;
    }
    fclose(fp);
    return 0;
}

static void getConnectTime(webs_t wp, int wannum)
{
    static  time_t tmpTime;
    int     arrayIndex = wannum - 1;
    tmpTime = getUptimeWanx(wannum);
    if(tmpTime < connTime[arrayIndex]) {
        tmpTime = connTime[arrayIndex];
    }
    websWrite(wp,"ConnTimes[%d]=\"%d\";\n", arrayIndex, getUptime()-tmpTime);
}

/***********************************************************************
 * 函 数 名：   getWANLinkRate
 * 功能描述：	取出接口速率
 * 输入参数:    上下行速度
 * 输出参数：	
 * 返回值：	
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       王炳荣	
 * 附加说明：	无
***********************************************************************/
static void getWANLinkRate(long long *uplink, long long  *downlink, int wannum)
{
    time_t   timesec,tmp;
    long long txByte, rxByte;
    static int upRate=0,downRate=0;
    int index = 0;
    char *wanName;
    index = wannum - 1;
    timesec=time((time_t *)NULL);
    tmp=timesec-LinkStatus[index].time;
    if(!tmp) {
	    *uplink=upRate;
	    *downlink=downRate;
	    return;
    }
    wanName = getWanIfNamePPP(wannum);
    txByte=getIfStatistic(wanName, TXBYTE);       //send bytes
    rxByte=getIfStatistic(wanName, RXBYTE);       //receive bytes
    if(rxByte < LinkStatus[index].rxByte) {
	    *downlink=0;
    } else {
	    *downlink=(rxByte-LinkStatus[index].rxByte)/tmp;
    }
    if(txByte<LinkStatus[index].txByte) {
        *uplink=0;
    } else {
        *uplink=(txByte-LinkStatus[index].txByte)/tmp;
    }
    LinkStatus[index].time=timesec;
    LinkStatus[index].rxByte=rxByte;
    LinkStatus[index].txByte=txByte;
    upRate=*uplink;
    downRate=*downlink;
}

/***********************************************************************
 * 函 数 名：   aspIspRunningConfig
 * 功能描述：   返回连接状态和连接时间
 * 输入参数:    eid,wp,argc,argv
 * 输出参数：	
 * 返回值：	    0 
 * 创建日期：	2010-3-16
 * 修改日期：
 * 作者：       王炳荣	
 * 附加说明：	无
***********************************************************************/
int aspIspRunningConfig(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_3G
    char_t ip[16];
#endif
    long long up,down;
    int state=0;
    int i = 0;
    char ratebuf[0x20];
    InterfaceProfile *profIf = NULL;
    Uint32 wanCount = 0;
    char if_addr[16];

    char_t *ConnStatuss=T("ConnStatuss[%d]=\"%d\";\n");
    char_t *SpeedUps=T("SpeedUps[%d]=\"%s\";\n");
    char_t *SpeedDowns=T("SpeedDowns[%d]=\"%s\";\n");

#if 1
    char_t *IPs=T("IPs[%d] = \"%s\";");
    char_t *Masks=T("Masks[%d] = \"%s\";");
    char_t *Gws=T("GateWays[%d] = \"%s\";");
    websWrite(wp,"var GateWays=new Array();");
    websWrite(wp,"var IPs=new Array();");
    websWrite(wp,"var Masks=new Array();");
#endif

    websWrite(wp,"var ConnStatuss=new Array();\n");
    websWrite(wp,"var SpeedDowns=new Array();\n");
    websWrite(wp,"var SpeedUps=new Array();\n");
    websWrite(wp,"var ConnTimes=new Array();\n");
    wanCount = getWanSlotCount();
    for(i = 1; i <= wanCount; i++) {
        state = 0;
        up = 0;
        down = 0;
//        wannum = i + 1;

        profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, i);
        if (0 == getIfIp(getWanIfNamePPP(i), if_addr)) {
            websWrite(wp, IPs, i-1, if_addr);
        } else{
            websWrite(wp, IPs, i-1, "");
        }
        
        if (0 == getIfNetmask(getWanIfNamePPP(i), if_addr)) {
            websWrite(wp, Masks, i-1, if_addr);
        } else{
            websWrite(wp, Masks, i-1, "");
        }

        if (0 == getWanGwByIndex(i, if_addr)) {
            websWrite(wp, Gws, i-1, if_addr);
        } else{
            websWrite(wp, Gws, i-1, "");
        }

#ifdef CONFIG_USER_3G
	if (i == get3GSlotIndex()) {
	    	if((getIfIp(getWanIfNamePPP(i),ip)==0) /*&& wanIsUp*/ ) {
	    		state=1;
		}
        }else 
#endif 
        {
            switch(profIf->ether.connMode) {
                case ETH_CONN_DHCP:
	    	    if(getIfIp(getWanIfName(i), NULL) == 0) {
	    		state=1;
                    }
                    state &= get_phylink_status(i);
                    break;
                case ETH_CONN_STATIC:
	            state=get_phylink_status(i);
                    break;
                case ETH_CONN_PPPOE:
	            state = getpppoeStatus(i);
                    state &= get_phylink_status(i);
                    break;
                default:
                    break;
            }
        }

        websWrite(wp, ConnStatuss, i-1, state);
        /*
         * 断开连接，时间清零
         */
        if(state == 0) {
            connTime[i-1] = getUptime();
        } else {
            getWANLinkRate(&up, &down, i);
        }
        getConnectTime(wp, i);

        sprintf(ratebuf, "%lld", (down << 3));
        websWrite(wp, SpeedDowns, i-1, ratebuf);
        sprintf(ratebuf, "%lld", (up << 3));
        websWrite(wp, SpeedUps, i-1, ratebuf);
    }
    return 0;
}

static void FastConfFirstLoad(webs_t wp, char_t *path, char_t *query)
{
    SystemProfile *profSys = NULL;
    profSys = (SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);

#if (FEATURE_AP == FYES)
	MibProfileType mibType = MIB_PROF_FAT_FIT_SWITCH;
	FatFitSwitchProfile *prof = NULL;

	prof = (FatFitSwitchProfile *)ProfGetInstPointByIndex(mibType, 0);
	if (prof->fatEnable == PROF_DISABLE) {
#if(UTT_TOMAX == FYES)
		websRedirect(wp, "SysInfo.asp");
#else
		websRedirect(wp, "fitApSysInfo.asp");
#endif
	} else {
		if(profSys->popWelcome == 1) {
			websRedirect(wp, "FastConfig.asp");
		} else {
			websRedirect(wp, "SysInfo.asp");
		}
	}
#elif (NET_MODE_SWITCH == FYES)
	NetModeSwitchProfile *profNM = NULL;
	profNM = (NetModeSwitchProfile*)ProfGetInstPointByIndex(MIB_PROF_NET_MODE_SWITCH,0);
	if(profNM!=NULL&&profNM->bridgeEnable==FUN_ENABLE)
	{
        websRedirect(wp, "cachePage.asp");
	}else if(profSys->popWelcome == 1) {
        websRedirect(wp, "FastConfig.asp");
    } else {
        websRedirect(wp, "SysInfo.asp");
    }
#elif (FEATURE_AC== FYES)
	if(profSys->popWelcome == 1) {
		websRedirect(wp, "FastConfig.asp");
	} else {
		websRedirect(wp, "firstPage.asp");
	}
#else
	if(profSys->popWelcome == 1) {
		websRedirect(wp, "FastConfig.asp");
	} else {
		websRedirect(wp, "SysInfo.asp");
	}
#endif
}

static void formFastConfPop(webs_t wp, char_t *path, char_t *query)
{
    SystemProfile *profSys = NULL;
    char_t  *notPopUp;
    notPopUp = websGetVar(wp, T("notPopUp"), T("")); 
    profSys = (SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    if(strcmp(notPopUp, "on") == 0) {
        profSys->popWelcome = FUN_DISABLE;
    } else {
        profSys->popWelcome = FUN_ENABLE;
    }
    nvramWriteCommit();
    websRedirect(wp, "FastConfig.asp");
    return;    
}

/*
 * 从/proc/net/arp中读取网关mac输出到页面
 */
static void readScanArp(int eid,webs_t wp,int argc,char_t **argv)
{
    FILE *fp;
    int _wannum =0,flag =0;
    char *wannum=NULL,*errMsg=NULL;
    char buf[BUF_LEN+1],GwIp[IP_LEN+1], ipStr[IP_LEN+1], macStr[MAC_LEN+1], tmpMacStr[MAC_LEN+1];
    MacAddr macAddr;
    struct in_addr addrS;
    IPADDR arpScanNet = 0u;

    memset(ipStr, 0, sizeof(ipStr));
    memset(GwIp, 0, sizeof(GwIp));
    memset(macStr, 0, sizeof(macStr));
    memset(&macAddr, 0, sizeof(macAddr));
    memset(&addrS, 0, sizeof(addrS));
    memset(&tmpMacStr, 0, sizeof(tmpMacStr));
    wannum = websGetVar(wp, T("wannum"), T(""));/*扫描网段*/
    _wannum = strtol(wannum, NULL, 10); 
    if(*wannum != '\0')/*扫描后跳转过来的*/
    {
	if (getWanGwByIndex(_wannum,GwIp) !=0){
	    errMsg = getTransDataForC(C_LANG_INDEX_GET_GW_MAC_FAIL); 
	    setTheErrorMsg(errMsg);
	    return;
	}

	fp=fopen("/proc/net/arp","r");/*读取系统arp表*/
	while(!feof(fp))
	{    
	    memset(buf, 0, sizeof(buf));
	    fgets(buf, sizeof(buf), fp);/*读取一条arp条目*/
	    if(strlen(buf) >= ARP_VALID_MIN_LEN)/*合法arp条目*/
            {    
		/*从arp条目中解析出来ip以及mac地址*/
		if(getIpMacFromSysArpEntry(buf, ipStr, macStr, getWanIfNamePPP(_wannum))!=0)
		{    

		    if(strncmp(ipStr,GwIp,IP_LEN) == 0){ 
		    strncpy(tmpMacStr, macStr, sizeof(tmpMacStr) - 1u); 
		    strRemoveChar(tmpMacStr, ':');/*移除mac地址中的连接符*/
		    converMac12To6Byte(tmpMacStr, &macAddr);/*转化为6字节形式mac地址*/
		    websWrite(wp,"i_flag[%d]=1;GateWayMac_scan[%d]=\"%s\";",_wannum-1,_wannum-1,tmpMacStr);
		    websWrite(wp,"BindWanEn[%d] = \"MANUAL\";",_wannum-1);
		    websWrite(wp,"var wannum=\"%s\";",wannum);
		    flag =1;    //成功读取mac
		    break;
		    }
		}    
	    }   
	} 
	if(flag == 0){                   //读取mac失败
//	    errMsg = "获取网关MAC地址失败，请检查您的线路连接状态是否为已连接或者静态网关ip是否填写正确！";
            errMsg = getTransDataForC(C_LANG_INDEX_GET_GW_MAC_FAIL); 
	    setTheErrorMsg(errMsg);
	}

    }else{
	return; 
    } 
}

/*
 * 页面点击扫描调用该函数发arping包获取上层网关mac
 */    
static void getGwMac(webs_t wp, char_t *path, char_t *query)
{
    char ipStr[IP_LEN+1];
    char retUrl[URL_LEN+1];
    char *wannum=NULL;
    int _wannum =0;
    const char *errMsg=NULL;
    wannum = websGetVar(wp, T("PortName"), T(""));
    _wannum = strtol(wannum, NULL, 10); 
//    printf("profile =%s\n",wannum);
    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, _wannum);
//    printf("***%s\n",MibGetStrByEnum(profIf->ether.connMode, ethConnModeEnum));
    if(strcmp(MibGetStrByEnum(profIf->ether.connMode, ethConnModeEnum),"STATIC") != 0){
	errMsg = getTransDataForC(C_LANG_INDEX_GET_GW_MAC_FAIL);
	setTheErrorMsg(errMsg);
	websRedirect(wp, "WANConfig.asp");
	return;
    }

    memset(retUrl,0,sizeof(retUrl));
    if (getWanGwByIndex(_wannum,ipStr) !=0){
	return;
    }
    doSystem("ip neigh flush dev %s", getWanIfNamePPP(_wannum));/*先清空arp表*/
    doSystem("arping -q -I %s %s -c 1 &",getWanIfNamePPP(_wannum),ipStr);/*利用arping发送广播包*/
    sleep(3);/*等待arp回复信息*/
    snprintf(retUrl,URL_LEN, "WANConfig.asp?wannum=%d",_wannum);/*将扫描WAN口记录*/
    websRedirect(wp, retUrl);
    return;    
}

void formDefineIsp(void) {
    websAspDefine(T("aspIspRunningConfig"), aspIspRunningConfig);
    websAspDefine(T("OutputConnNum"), aspOutputConnNum);
#if (WIRELESS == FYES)
    websAspDefine(T("Output3GConfig"), aspOutput3GConfig);
    websAspDefine(T("OutputFastOpMode"), aspOutputFastOpMode);
    websFormDefine(T("formConfigFastDirectionW"), formConfigFastDirectionW);
#if (WIRELESS_5G == FYES)
    websFormDefine(T("formConfigFastDirectionW_5g"), formConfigFastDirectionW_5g);
#endif
#else
    websAspDefine(T("Output3GConfig"), aspOutput3GConfig);
#endif
#if (FEATURE_AC == FYES && UTT_SMART_UI != FYES)
    websFormDefine(T("formConfigFastDirectionAC"), formConfigFastDirectionAC);
#endif
    websAspDefine(T("aspFastConfigPopWelcome"), aspFastConfigPopWelcome);
    websAspDefine(T("aspFastConfigNet"), aspFastConfigNet);
#if (UTT_SMART_UI == FYES)
    websAspDefine(T("aspWanConfig_Sui"), aspWanConfig_Sui);
    websFormDefine(T("formWanIfConfig_Sui"), formWanIfConfig_Sui);
#endif
    websAspDefine(T("aspWanConfig"), aspWanConfig);
    websFormDefine(T("formWanIfConfig"), formWanIfConfig);
    websFormDefine(T("formConfigFastDirection"), formConfigFastDirection);
    websFormDefine(T("formReleaseConnect"), formReleaseConnect);
    websFormDefine(T("formReConnect"), formReConnect);
    websFormDefine(T("formRoadDel"), formRoadDel);
    websFormDefine(T("formFastConfPop"), formFastConfPop);
    websFormDefine(T("FastConfFirstLoad"), FastConfFirstLoad);
    websFormDefine(T("getGwMac"), getGwMac);
#if (UTT_SMART_UI == FYES)
    strcpy(arpLan.arp_dev, getLanIfName());
#endif
}
