#include "uttMachine.h"
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "mib.h"
#include "profacce.h"
#include "uttfunction.h"
#include "argcmd.h"
#include "ifmisc.h"
#include "wuScript.h"
#include "wusL2tpServer.h"
#include "wusPptpServer.h"
#include "translate.h"

#ifdef DBG_LAN
#define LC_PRINTF(fmt, args...)		printf(fmt, ## args)
#else 
#define LC_PRINTF(fmt, args...)
#endif


#if (STATIC_ROUTE == FYES)
static int isStaticRouteIncludeLanIp(Uint32 lanIp, Uint32 lanMask) {
    MibProfileType mibType = MIB_PROF_STATIC_ROUTE;
    StaticRouteProfile *profRoute = NULL;
    int min = 0, max = 0, i = 0;
    int rst = 0;

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) {
	profRoute = (StaticRouteProfile *)ProfGetInstPointByIndex(mibType, i);
	if (ProfInstIsFree(profRoute) == 0) {
	    if ((lanIp & lanMask) == (ntohl(profRoute->dstAddr) & ntohl(profRoute->netmask))) {
		rst = 1;
		break;
	    }
	}
    }
    return rst;
}
#endif


/*
 * lan口配置页面配置函数
 */
static void formLanConfig(webs_t wp, char_t *path, char_t *query)
{
#if (DHCP_POOL == FYES)
	int min = 0, max = 0, index = 0;
#endif
#if 0
#if (HTTP_CACHE == FYES)
	struct in_addr  intaddr;
	char_t oldLanIP[128],cmd[128];
	char_t *cp;
#endif
#endif
#if ((DHCP_SERVER == FYES) || (NET_SECURITY == FYES) || (DHCP_POOL ==FYES) )
    unsigned int nIp, eIp, uNet, nIp1, eIp1;
#if (MULTI_SSID_DHCP_POOL == FYES)
    unsigned int ssid1_nIp, ssid1_eIp, ssid1_nIp1, ssid1_eIp1;
    unsigned int ssid2_nIp, ssid2_eIp, ssid2_uNet, ssid2_nIp1, ssid2_eIp1;
#if (WIRELESS_5G == FYES)
    unsigned int ssid1_nIp_5g, ssid1_eIp_5g, ssid1_nIp1_5g, ssid1_eIp1_5g;
    unsigned int ssid2_nIp_5g, ssid2_eIp_5g, ssid2_uNet_5g, ssid2_nIp1_5g, ssid2_eIp1_5g;
#endif
#endif
#endif
    IpPoolEnum cfPoolType = IP_POOL_NONE;
    Uint32 lanIp, wanIp, wanMask, lanMask;
#if(MULTI_LAN == FYES)
    Uint32 lanIp2, lanMask2, lanIp3, lanMask3, lanIp4, lanMask4;
    char *ip2, *nm2, *ip3, *nm3, *ip4, *nm4;
#endif
#if 1
#if (PORT_CONF_REDUCED == FYES)
    char *mode;
    SwPortConfMode oldMode;
#endif
#endif
    int i = 0;
    char if_addr[16] = {0};
    char if_nm[16] = {0};
    const char *errMsg=NULL;

    Uint32 wanCount = 0;
    char *ip, *nm, *mac;
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;

    ProfChangeType profAction;
    struProfAlloc *profhead = NULL;

#if( UTT_IPAUTOREP  ==  FYES)
    SystemProfile *profSystem=NULL;//MIB_PROF_SYSTEM
    profSystem=(SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    char_t *check=websGetVar(wp, T("CheckAgencyEnable"), T(""));    //CheckAgencyEnable
    if (strcmp(check,"on") == 0)
    {
         profSystem->CheckIPEn = FUN_ENABLE;	
    }
    else
    {
	 profSystem->CheckIPEn = FUN_DISABLE;
    }
#endif

#if (UTT_SMART_UI == FYES)
    /*开启新UI后，DNS代理设置功能在LAN口配置页面*/
    char_t *dnsAgency;
    dnsAgency = websGetVar(wp, T("dnsAgencyEnable"), T(""));
#endif

#if (DHCP_POOL == FYES)
    MibProfileType mibTypeDhcpP = MIB_PROF_DHCP_POOL;
	DhcpPoolProfile *dhcpPool = NULL;
	DhcpPoolProfile *dhcpPool1 = NULL;
#if (DNS_AGENCY == FYES)
	MibProfileType mibTypeDnsAg = MIB_PROF_DNS_AGENCY;
	DnsAgencyProfile *profDns = NULL;
#endif
#endif

#if (DHCP_SERVER == FYES)
    MibProfileType mibTypeDhcpS = MIB_PROF_DHCP_SERVER;
    DhcpServerProfile *profDhcpS = NULL;
#endif

#if (STATIC_NAT == FYES)
    MibProfileType mibTypeNat = MIB_PROF_STATIC_NAT;
    StaticNatProfile *profNat = NULL;
#endif

#if (L2TP_SERVER == FYES)
    MibProfileType mibTypeL2tp = MIB_PROF_L2TP_SRV_GLOBAL;
    L2TPSrvGlobalProfile *profL2tp = NULL;
#endif

#if (PPTP_SERVER == FYES)
    MibProfileType mibTypePptp = MIB_PROF_PPTP_SRV_GLOBAL;
    PPTPSrvGlobalProfile *profPptp = NULL;
#endif

#if (NET_SECURITY == FYES)
    SystemProfile *profSys = NULL;
#endif
    ip = websGetVar(wp, T("lanIp"), T(""));
    nm = websGetVar(wp, T("lanNetmask"), T(""));
    mac = websGetVar(wp, T("lanMac"), T(""));
	
    lanIp = ip2int(ip);
    lanMask = ip2int(nm);
    LC_PRINTF("%s: ip=%s, nm=%s, lanIp=0x%x, lanMask=0x%x, lanMac=%s\n", 
	    __FUNCTION__, ip, nm, lanIp, lanMask, mac);
#if(MULTI_LAN == FYES)
#if (UTT_SMART_UI == FYES)
    ip2 = websGetVar(wp, T("lanIp2"), T("0.0.0.0"));
    nm2 = websGetVar(wp, T("lanNetmask2"), T("255.255.255.0"));
    ip3 = websGetVar(wp, T("lanIp3"), T("0.0.0.0"));
    nm3 = websGetVar(wp, T("lanNetmask3"), T("255.255.255.0"));
    ip4 = websGetVar(wp, T("lanIp4"), T("0.0.0.0"));
    nm4 = websGetVar(wp, T("lanNetmask4"), T("255.255.255.0"));
#else
    ip2 = websGetVar(wp, T("lanIp2"), T(""));
    nm2 = websGetVar(wp, T("lanNetmask2"), T(""));
    ip3 = websGetVar(wp, T("lanIp3"), T(""));
    nm3 = websGetVar(wp, T("lanNetmask3"), T(""));
    ip4 = websGetVar(wp, T("lanIp4"), T(""));
    nm4 = websGetVar(wp, T("lanNetmask4"), T(""));
#endif

    lanIp2 = ip2int(ip2);
    lanMask2 = ip2int(nm2);
    lanIp3 = ip2int(ip3);
    lanMask3 = ip2int(nm3);
    lanIp4 = ip2int(ip4);
    lanMask4 = ip2int(nm4);

    LC_PRINTF("%s-%d: ip2=%s, nm2=%s, lanIp2=0x%x, lanMask2=0x%x\n", 
	    __func__, __LINE__, ip2, nm2, lanIp2, lanMask2);
    LC_PRINTF("%s-%d: ip3=%s, nm3=%s, lanIp3=0x%x, lanMask3=0x%x\n", 
	    __func__, __LINE__, ip3, nm3, lanIp3, lanMask3);
    LC_PRINTF("%s-%d: ip4=%s, nm4=%s, lanIp4=0x%x, lanMask4=0x%x\n", 
	    __func__, __LINE__, ip4, nm4, lanIp4, lanMask4);
#endif
#if 1
#if (PORT_CONF_REDUCED == FYES)
#if (UTT_SMART_UI == FYES)
    mode = websGetVar(wp, T("LanMode"), T("3"));
#else
    mode = websGetVar(wp, T("LanMode"), T(""));
#endif
    LC_PRINTF("%s-%d: mode=%s\n", __func__, __LINE__, mode);
#endif
#endif
/*mantis 12545,1.5更改后静态路由目的网络可以包含lan ip，不再需要该判断*/
#if 0
#if (STATIC_ROUTE == FYES)
    /*
     * 判断静态路由目的网络是否包含lan ip
     */
    if (isStaticRouteIncludeLanIp(lanIp, lanMask) 
#if(MULTI_LAN == FYES)
	    || isStaticRouteIncludeLanIp(lanIp2, lanMask2)
	    || isStaticRouteIncludeLanIp(lanIp3, lanMask3)
	    || isStaticRouteIncludeLanIp(lanIp4, lanMask4)
#endif
       )
	{
   	setTheErrorMsg(T("静态路由目的网络包含LAN口IP"));
    	goto out;
    }
#endif
#endif
	
    wanCount = getWanSlotCount();

    /*
     * 判断lan口ip和wan口ip配置是否重叠,wan口固定IP情况
     */
    for (i = 1; i <= wanCount; i++) {
#if 0
	profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, i);
	if ((profIF == NULL) || (profIF->head.active == 0)) {
	    LC_PRINTF("%s: interface profile get error.\n", __FUNCTION__);
	    continue;
	}

	/*
	 * wan固定IP
	 */
	if (profIF->ether.connMode == ETH_CONN_STATIC) {
	    wanIp = ntohl(profIF->ether.sta.ip);
	    wanMask = ntohl(profIF->ether.sta.nm);
	    LC_PRINTF("%s:wanIp=0x%x, wanMask=0x%x\n", __FUNCTION__, wanIp, wanMask);
	    /*
	     * 检测是否和wan口在同一网段
	     */
	    if (((wanIp & wanMask) == (lanIp & wanMask)) || ((wanIp & lanMask) == (lanIp & lanMask))) {
	    	setTheErrorMsg(T("LAN口IP地址不能与WAN口IP在同一网段"));
	    	goto out;
	    }
	}
#endif
	if ((getIfIp(getWanIfNamePPP(i), if_addr) == 0) && 
		(getIfNetmask(getWanIfNamePPP(i), if_nm) == 0)) {
	    wanIp = ntohl(inet_addr(if_addr));
	    wanMask = ntohl(inet_addr(if_nm));
	     /*
	     * 检测是否和wan口在同一网段
	     */
	    if (((wanIp & wanMask) == (lanIp & wanMask)) || ((wanIp & lanMask) == (lanIp & lanMask))
#if(MULTI_LAN == FYES)
	    || ((wanIp & wanMask) == (lanIp2 & wanMask)) || ((wanIp & lanMask2) == (lanIp2 & lanMask2)) 
	    || ((wanIp & wanMask) == (lanIp3 & wanMask)) || ((wanIp & lanMask3) == (lanIp3 & lanMask3)) 
	    || ((wanIp & wanMask) == (lanIp4 & wanMask)) || ((wanIp & lanMask4) == (lanIp4 & lanMask4)) 
#endif
	       )
		{
	    	errMsg = getTransDataForC(C_LANG_INDEX_LAN_NOT_SAME_WAN);
		setTheErrorMsg(errMsg);
	    	goto out;
	    }
	}
    }

    /*
     * 获得lan口profile
     */
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
    if ((profIF == NULL) || (profIF->head.active == 0)) {
	errMsg = getTransDataForC(C_LANG_INDEX_LAN_CONFIG_ERR);
	setTheErrorMsg(errMsg);
	goto out;
    }
#if 0
#if (HTTP_CACHE == FYES)
	 if(profIF != NULL)  {
		intaddr.s_addr = profIF->ether.sta.ip;
	} else {
		intaddr.s_addr = inet_addr("192.168.1.1");
	}
	cp = inet_ntoa(intaddr);
	ascToUni(oldLanIP, cp, min(strlen(cp) + 1, sizeof(oldLanIP)));
#endif
#endif
    /* lan ip地址是否和地址池冲突 */
#if ((L2TP_SERVER == FYES) || (PPTP_SERVER == FYES) || (PPPOE_SERVER == FYES))
    cfPoolType = ipPoolConflict(IP_POOL_NONE, htonl(lanIp), htonl(lanIp));
    if (cfPoolType != IP_POOL_NONE)
    {
	setPoolConflictMsg(cfPoolType);
	goto out;
    }
#if(MULTI_LAN == FYES)
    if(lanIp2 != 0U)
    {
	cfPoolType = ipPoolConflict(IP_POOL_NONE, htonl(lanIp2), htonl(lanIp2));
    }
    if (cfPoolType != IP_POOL_NONE)
    {
	setPoolConflictMsg(cfPoolType);
	goto out;
    }
    if(lanIp3 != 0U)
    {
	cfPoolType = ipPoolConflict(IP_POOL_NONE, htonl(lanIp3), htonl(lanIp3));
    }
    if (cfPoolType != IP_POOL_NONE)
    {
	setPoolConflictMsg(cfPoolType);
	goto out;
    }
    if(lanIp4 != 0U)
    {
	cfPoolType = ipPoolConflict(IP_POOL_NONE, htonl(lanIp4), htonl(lanIp4));
    }
    if (cfPoolType != IP_POOL_NONE)
    {
	setPoolConflictMsg(cfPoolType);
	goto out;
    }
#endif
#endif

    LC_PRINTF("%s-%d\n", __func__, __LINE__); 
#if (DHCP_SERVER == FYES)
    /*
     * 获得dhcp起始结束ip
     */
    profDhcpS = (DhcpServerProfile*)ProfGetInstPointByIndex(mibTypeDhcpS, 0);
    if (profDhcpS == NULL) {
   	errMsg = getTransDataForC(C_LANG_INDEX_DHCP_SERVER_ERR);
	setTheErrorMsg(errMsg);
	goto out;
    }

	nIp = ntohl(profDhcpS->ipStart);
	eIp = ntohl(profDhcpS->ipEnd);
	/* 主机序，lan口网段, dhcp地址池网段 */
#if 0
	uNet = lanIp & lanMask;
	/* dhcp地址池随lanIp变化后的地址,主机字节序 */
	nIp1 = uNet + (nIp & (~lanMask));
	eIp1 = uNet + (eIp & (~lanMask));
#else
	uNet = lanIp & 0xffffff00;
	/* dhcp地址池随lanIp变化后的地址,主机字节序 */
	nIp1 = uNet + (nIp & 0x000000ff);
	eIp1 = uNet + (eIp & 0x000000ff);

#endif
    LC_PRINTF("%s: nIp=0x%x,eIp=0x%x,uNet=0x%x,nIp1=0x%x,eIp1=0x%x\n", 
	    __FUNCTION__ , nIp, eIp, uNet, nIp1, eIp1);
    if((lanIp <= eIp1) && (lanIp >= nIp1)){
	/* mantis 8712 */
	if (nIp1 != nIp) {
	    errMsg = getTransDataForC(C_LANG_INDEX_DHCP_CHANGE_ERR);
	    setTheErrorMsg(errMsg);
	} else {
	    errMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN); 
	    setTheErrorMsg(errMsg);
	}
		goto out;
	}
#if (MULTI_SSID_DHCP_POOL == FYES)
    /*if (profDhcpS->ssid1_dhcp_enable == PROF_ENABLE) {*/
	ssid1_nIp = ntohl(profDhcpS->ssid1_ipStart);
	ssid1_eIp = ntohl(profDhcpS->ssid1_ipEnd);
	ssid1_nIp1 = uNet + (ssid1_nIp & 0x000000ff);
	ssid1_eIp1 = uNet + (ssid1_eIp & 0x000000ff);
	if((lanIp <= ssid1_eIp1) && (lanIp >= ssid1_nIp1)){
	    if (ssid1_nIp1 != ssid1_nIp) {
		errMsg = getTransDataForC(C_LANG_INDEX_DHCP_CHANGE_ERR_SSID1);
		setTheErrorMsg(errMsg);
	    } else {
		errMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN_SSID1);
		setTheErrorMsg(errMsg);
	    }
	    goto out;
	}
   /* }*/
    /*if (profDhcpS->ssid2_dhcp_enable == PROF_ENABLE) {*/
	ssid2_nIp = ntohl(profDhcpS->ssid2_ipStart);
	ssid2_eIp = ntohl(profDhcpS->ssid2_ipEnd);
	ssid2_nIp1 = uNet + (ssid2_nIp & 0x000000ff);
	ssid2_eIp1 = uNet + (ssid2_eIp & 0x000000ff);
	if((lanIp <= ssid2_eIp1) && (lanIp >= ssid2_nIp1)){
	    if (ssid2_nIp1 != ssid2_nIp) {
		errMsg = getTransDataForC(C_LANG_INDEX_DHCP_CHANGE_ERR_SSID2);
		setTheErrorMsg(errMsg);
	    } else {
		errMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN_SSID2);
		setTheErrorMsg(errMsg);
	    }
	    goto out;
	}
    /*}*/

#if (WIRELESS_5G == FYES)
    /*if (profDhcpS->ssid1_dhcp_enable_5g == PROF_ENABLE) {*/
	ssid1_nIp_5g = ntohl(profDhcpS->ssid1_ipStart_5g);
	ssid1_eIp_5g = ntohl(profDhcpS->ssid1_ipEnd_5g);
	ssid1_nIp1_5g = uNet + (ssid1_nIp_5g & 0x000000ff);
	ssid1_eIp1_5g = uNet + (ssid1_eIp_5g & 0x000000ff);
	if((lanIp <= ssid1_eIp1_5g) && (lanIp >= ssid1_nIp1_5g)){
	    if (ssid1_nIp1_5g != ssid1_nIp_5g) {
		errMsg = getTransDataForC(C_LANG_INDEX_DHCP_CHANGE_ERR_SSID1_5G);
		setTheErrorMsg(errMsg);
	    } else {
		errMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN_SSID1_5G);
		setTheErrorMsg(errMsg);
	    }
	    goto out;
	}
    /*}*/
   /* if (profDhcpS->ssid2_dhcp_enable_5g == PROF_ENABLE) {*/
	ssid2_nIp_5g = ntohl(profDhcpS->ssid2_ipStart_5g);
	ssid2_eIp_5g = ntohl(profDhcpS->ssid2_ipEnd_5g);
	ssid2_nIp1_5g = uNet + (ssid2_nIp_5g & 0x000000ff);
	ssid2_eIp1_5g = uNet + (ssid2_eIp_5g & 0x000000ff);
	if((lanIp <= ssid2_eIp1_5g) && (lanIp >= ssid2_nIp1_5g)){
	    if (ssid2_nIp1_5g != ssid2_nIp_5g) {
		errMsg = getTransDataForC(C_LANG_INDEX_DHCP_CHANGE_ERR_SSID2_5G);
		setTheErrorMsg(errMsg);
	    } else {
		errMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN_SSID2_5G);
		setTheErrorMsg(errMsg);
	    }
	    goto out;
	}
   /* }*/
#endif
#endif
	/*
	 * 判断地址池
	 */
	cfPoolType = ipPoolConflict(IP_POOL_DHCP, htonl(nIp1), htonl(eIp1));
	if (cfPoolType != IP_POOL_NONE) {
		setPoolConflictMsg(cfPoolType);
		goto out;
	}
#if (MULTI_SSID_DHCP_POOL == FYES)
	/*2.4G ssid dhcp pool*/
	printf("lwang: %s---%d\n",__FILE__, __LINE__);
	/*if (profDhcpS->ssid1_dhcp_enable == PROF_ENABLE) {*/
	printf("lwang: %s---%d\n",__FILE__, __LINE__);
	    cfPoolType = ipPoolConflict(IP_POOL_DHCP, htonl(ssid1_nIp1), htonl(ssid1_eIp1));
	    if (cfPoolType != IP_POOL_NONE) {
		    setPoolConflictMsg(cfPoolType);
		    goto out;
	    }
	/*}*/
	printf("lwang: %s---%d\n",__FILE__, __LINE__);
	/*if (profDhcpS->ssid2_dhcp_enable == PROF_ENABLE) {*/
	    cfPoolType = ipPoolConflict(IP_POOL_DHCP, htonl(ssid2_nIp1), htonl(ssid2_eIp1));
	    if (cfPoolType != IP_POOL_NONE) {
		    setPoolConflictMsg(cfPoolType);
		    goto out;
	    }
	/*}*/
#if (WIRELESS_5G == FYES)
	/*5G ssid dhcp pool*/
	/*if (profDhcpS->ssid1_dhcp_enable_5g == PROF_ENABLE) {*/
	    cfPoolType = ipPoolConflict(IP_POOL_DHCP, htonl(ssid1_nIp1_5g), htonl(ssid1_eIp1_5g));
	    if (cfPoolType != IP_POOL_NONE) {
		    setPoolConflictMsg(cfPoolType);
		    goto out;
	    }
	/*}*/
	/*if (profDhcpS->ssid2_dhcp_enable_5g == PROF_ENABLE) {*/
	    cfPoolType = ipPoolConflict(IP_POOL_DHCP, htonl(ssid2_nIp1_5g), htonl(ssid2_eIp1_5g));
	    if (cfPoolType != IP_POOL_NONE) {
		    setPoolConflictMsg(cfPoolType);
		    goto out;
	    }
	/*}*/
#endif
#endif

	profAction = PROF_CHANGE_EDIT;
	ProfBackupByIndex(mibTypeDhcpS, profAction, 0, &profhead);

	if(profDhcpS->gateway != htonl(lanIp))
	{
		/*
		 * IP地址段修改
		 */
		profDhcpS->ipStart = htonl(nIp1);
		profDhcpS->ipEnd = htonl(eIp1);
#if (MULTI_SSID_DHCP_POOL == FYES)
		/*if (profDhcpS->ssid1_dhcp_enable == PROF_ENABLE) {*/
		    profDhcpS->ssid1_ipStart = htonl(ssid1_nIp1);
		    profDhcpS->ssid1_ipEnd = htonl(ssid1_eIp1);
		/*}*/
		/*if (profDhcpS->ssid2_dhcp_enable == PROF_ENABLE) {*/
		    profDhcpS->ssid2_ipStart = htonl(ssid2_nIp1);
		    profDhcpS->ssid2_ipEnd = htonl(ssid2_eIp1);
		/*}*/
#if (WIRELESS_5G == FYES)
		/*if (profDhcpS->ssid1_dhcp_enable_5g == PROF_ENABLE) {*/
		    profDhcpS->ssid1_ipStart_5g = htonl(ssid1_nIp1_5g);
		    profDhcpS->ssid1_ipEnd_5g = htonl(ssid1_eIp1_5g);
		/*}*/
		/*if (profDhcpS->ssid2_dhcp_enable_5g == PROF_ENABLE) {*/
		    profDhcpS->ssid2_ipStart_5g = htonl(ssid2_nIp1_5g);
		    profDhcpS->ssid2_ipEnd_5g = htonl(ssid2_eIp1_5g);
		/*}*/
#endif
#endif
		profDhcpS->gateway = htonl(lanIp);
	}
#if (UTT_SMART_UI == FYES)
	/*新UI设置DNS代理是否开启是在LAN口配置的页面*/
	if (*dnsAgency != '\0') {/*设置dhcp profile的dns代理字段*/
	    if (strcmp(dnsAgency,"on") == 0) {
		profDhcpS->DnsProxyEn = FUN_ENABLE;
	    } else {
		profDhcpS->DnsProxyEn = FUN_DISABLE;
	    }
	}
#endif
	/*
	 * dns代理开启，主dns修改为lan口ip地址
	 */
	if (profDhcpS->DnsProxyEn == FUN_ENABLE) {
		profDhcpS->pDns = htonl(lanIp);
	}
#if 0
	/* modify dhcp address range */
	if((strncmp(nm, "255.255.255.", 12) == 0) && (((lanIp & 0x000000FFU) < 100) || ((lanIp & 0x000000FFU) > 200))) {

		profDhcpS->ipStart = htonl((lanIp & 0xffffff00) | 100);
		profDhcpS->ipEnd = htonl((lanIp & 0xffffff00) | 200);
		profDhcpS->gateway = htonl(lanIp);
		profDhcpS->pDns = htonl(lanIp);
		LC_PRINTF("%s: ipS=0x%x, ipE=0x%x, gw=0x%x, dns=0x%x\n", 
				__FUNCTION__, profDhcpS->ipStart, profDhcpS->ipEnd, profDhcpS->gateway, profDhcpS->pDns);
	}
#endif
#endif

#if (DHCP_POOL == FYES)

	/*
	 * 获得dhcp起始结束ip
	 */
	dhcpPool = (DhcpPoolProfile*)ProfGetInstPointByIndex(mibTypeDhcpP, 0);
	if (dhcpPool == NULL) {
		setTheErrorMsg(T("DHCP服务器错误"));
		goto out;
	}

	nIp = ntohl(dhcpPool->ipStart);
	eIp = ntohl(dhcpPool->ipEnd);
	uNet = lanIp & 0xffffff00;
	/* dhcp地址池随lanIp变化后的地址,主机字节序 */
	nIp1 = uNet + (nIp & 0x000000ff);
	eIp1 = uNet + (eIp & 0x000000ff);

    LC_PRINTF("%s: nIp=0x%x,eIp=0x%x,uNet=0x%x,nIp1=0x%x,eIp1=0x%x\n", 
	    __FUNCTION__ , nIp, eIp, uNet, nIp1, eIp1);
    if((lanIp <= eIp1) && (lanIp >= nIp1)){
	/* mantis 8712 */
	if (nIp1 != nIp) {
	    errMsg = getTransDataForC(C_LANG_INDEX_DHCP_CHANGE_ERR);
	    setTheErrorMsg(errMsg);
	} else {
	    errMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN); 
	    setTheErrorMsg(errMsg);
	}
		goto out;
	}
	ProfInstLowHigh(mibTypeDhcpP, &max, &min);

	for (index = min; index < max; index++) {
		if (index != 0) {
			dhcpPool1 = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibTypeDhcpP, index);
			if(ipSegmentOverlap(nIp1, eIp1, ntohl(dhcpPool1->ipStart), ntohl(dhcpPool1->ipEnd)) == TRUE){
				errMsg = getTransDataForC(C_LANG_INDEX_DHCP_POOL_ADDCONFLICK);
				setTheErrorMsg(errMsg);
			    goto out;
			}
		}

#if(MULTI_LAN == FYES)
#if 0
		if (dhcpPool1 != NULL) {
			if (((lanIp2 != 0) && (htonl(lanIp2) == dhcpPool1->gateway))
				|| ((lanIp3 != 0) && (htonl(lanIp3) == dhcpPool1->gateway))
				|| ((lanIp4 != 0) && (htonl(lanIp4) == dhcpPool1->gateway))) {
				errMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN);
				setTheErrorMsg(errMsg);
				goto out;
			} else {
				continue;
			}
		}
#endif
#endif
	}
	/*
	 * 判断地址池
	 */
	cfPoolType = ipPoolConflict(IP_POOL_DHCP, htonl(nIp1), htonl(eIp1));
	if (cfPoolType != IP_POOL_NONE) {
		setPoolConflictMsg(cfPoolType);
		goto out;
	}

	// 1.9 project
#if 0
	profAction = PROF_CHANGE_EDIT;
	ProfBackupByIndex(mibTypeDhcpP, profAction, 0, &profhead);
#endif

	dhcpPool->netmask = htonl(lanMask);
	if(dhcpPool->gateway != htonl(lanIp))
	{
		/*
		 * IP地址段修改
		 */
		dhcpPool->ipStart = htonl(nIp1);
		dhcpPool->ipEnd = htonl(eIp1);
		dhcpPool->gateway = htonl(lanIp);

#if (SERVICE_ZONE == FYES)
		ServiceZoneProfile *profSz = NULL;
		MibProfileType mibTypeSz = MIB_PROF_SERVICE_ZONE;
		profSz = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibTypeSz, 0);
		profSz->gw = dhcpPool->gateway;
#endif
	}
#if (DNS_AGENCY == FYES)
	/*
	 * dns代理开启，主dns修改为lan口ip地址
	 */
	profDns = (DnsAgencyProfile *)ProfGetInstPointByIndex(mibTypeDnsAg, 0);
	if (NULL != profDns) {
		if (profDns->DnsProxyEn == FUN_ENABLE) {
			dhcpPool->pDns = htonl(lanIp);
		}
	}
#endif

#endif

#if (NET_SECURITY == FYES)
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    nIp = ntohl(profSys->sysNetSe.accessCtrlFromIP);
    eIp = ntohl(profSys->sysNetSe.accessCtrlEndIP);
    uNet = lanIp & lanMask;
    /*网段变化。则清设备访问控制*/
    if((((nIp&lanMask)!=uNet) || ((eIp&lanMask)!=uNet))
#if(MULTI_LAN == FYES)
    && (((nIp&lanMask2)!=uNet) || ((eIp&lanMask2)!=uNet)) 
    && (((nIp&lanMask3)!=uNet) || ((eIp&lanMask3)!=uNet))
    && (((nIp&lanMask4)!=uNet) || ((eIp&lanMask4)!=uNet))
#endif
      )
    {
	/*这里不需备份实例，触发speedweb，设备访问相关操作
	 *因设备访问控制相关变化。会根据speedweb中的lan口初始化函数变化*/
	profSys->sysNetSe.accessCtrlFromIP = 0u;
	profSys->sysNetSe.accessCtrlEndIP = 0u;
    }
#endif

#if (STATIC_NAT == FYES)
    /*
     * 远程管理静态nat 内网ip根据lan变化
     */
    profNat = (StaticNatProfile *)ProfGetInstPointByName(mibTypeNat, remCtrlName);
    if (profNat != NULL) {
#if 0
	/* lan口profile change会更新nat,所以不用再调用profile change */
	profAction = PROF_CHANGE_EDIT;
	ProfBackupByName(mibTypeNat, profAction, remCtrlName, &profhead);
#endif
        profNat->IP = htonl(lanIp);
        LC_PRINTF("%s: profNat->IP=0x%x\n", __FUNCTION__, profNat->IP);
    }
#endif


#if 0
#if (HTTP_CACHE == FYES)
#if (NET_MODE_SWITCH == FYES)
	NetModeSwitchProfile *netModeSwitchProf = NULL;
	netModeSwitchProf = (NetModeSwitchProfile *)ProfGetInstPointByIndex(MIB_PROF_NET_MODE_SWITCH,0);
	if((netModeSwitchProf==NULL)||(netModeSwitchProf->bridgeEnable==FUN_DISABLE))
	{
#endif
	/*
	*      *  * 修改ats数据转发规则，根据LAN地址
	*           *   */
	CacheProfile *profCache = NULL;
	profCache = (CacheProfile *)ProfGetInstPointByIndex(MIB_PROF_CACHE,0);
	if(profCache!=NULL && profCache->cacheEnable)
	{
	sprintf(cmd,"iptables -t nat -D PREROUTING  ! -d  %s -p tcp --dport 80 -j DNAT --to %s:3128",oldLanIP,oldLanIP);
	doSystem(cmd);
	sprintf(cmd,"iptables -t nat -A PREROUTING  ! -d  %s -p tcp --dport 80 -j DNAT --to %s:3128",ip,ip);
	doSystem(cmd);
	}
#if (NET_MODE_SWITCH == FYES)
	}
#endif
#endif
#endif
    /*
     * 备份lan口配置
     */
    profAction = PROF_CHANGE_EDIT;
    ProfBackupByIndex(mibTypeIF, profAction, 0, &profhead);

    profIF->ether.sta.ip = htonl(lanIp);
    profIF->ether.sta.nm = htonl(lanMask);
#if (MULTI_LAN == FYES)
    profIF->ether.sta.ip2 = htonl(lanIp2);
    profIF->ether.sta.nm2 = htonl(lanMask2);
    profIF->ether.sta.ip3 = htonl(lanIp3);
    profIF->ether.sta.nm3 = htonl(lanMask3);
    profIF->ether.sta.ip4 = htonl(lanIp4);
    profIF->ether.sta.nm4 = htonl(lanMask4);
#endif

    converMac12To6Byte(mac, &(profIF->ether.mac));
#if 1
#if (PORT_CONF_REDUCED == FYES)
    oldMode = profIF->ether.speedMode;
    if(strcmp(mode, "0")==0) {
	profIF->ether.speedMode=PORT_MODE_10FDX;
    } else if(strcmp(mode, "1")==0) {
	profIF->ether.speedMode=PORT_MODE_100FDX;
    } else if(strcmp(mode, "2")==0) {
	profIF->ether.speedMode=PORT_MODE_1GFDX;
    } else if(strcmp(mode, "3")==0) {
	profIF->ether.speedMode=PORT_MODE_AUTO;
    } else if(strcmp(mode, "4")==0) {
	profIF->ether.speedMode=PORT_MODE_10HDX;
    } else if(strcmp(mode, "5")==0) {
	profIF->ether.speedMode=PORT_MODE_100HDX;
    } else {
	profIF->ether.speedMode=PORT_MODE_DISABLED;
    }

#endif
#endif
#if (L2TP_SERVER == FYES)
    /* 监听ip变化 */
    profL2tp = (L2TPSrvGlobalProfile *)ProfGetInstPointByIndex(mibTypeL2tp, 0);
    if (profL2tp != NULL) {
	profAction = PROF_CHANGE_EDIT;
        ProfBackupByIndex(mibTypeL2tp, profAction, 0, &profhead);
	profL2tp->listenIp = htonl(lanIp);
	LC_PRINTF("%s: profL2tp->listenIp = 0x%x\n", __FUNCTION__, profL2tp->listenIp);
#if (STATIC_NAT == FYES)
	/* l2tp静态映射变化 */
	if (profL2tp->head.active == 1) {
	    profNat = (StaticNatProfile *)ProfGetInstPointByName(mibTypeNat, L2TP_NAT_NAME);
	    if (profNat != NULL) {
#if 0
		/* lan口 profile change会更新nat,所以此处无需再调用nat profile change了*/
		profAction = PROF_CHANGE_EDIT;
		ProfBackupByName(mibTypeNat, profAction, L2TP_NAT_NAME, &profhead);
#endif
		profNat->IP = htonl(lanIp);
		LC_PRINTF("%s: l2tp - profNat->IP=0x%x\n", __FUNCTION__, profNat->IP);
	    }
	}
#endif
    }
#endif

#if (PPTP_SERVER == FYES)
    /* 监听ip变化 */
    profPptp = (PPTPSrvGlobalProfile *)ProfGetInstPointByIndex(mibTypePptp, 0);
    if (profPptp != NULL) {
	profAction = PROF_CHANGE_EDIT;
        ProfBackupByIndex(mibTypePptp, profAction, 0, &profhead);
	profPptp->listenIp = htonl(lanIp);
	LC_PRINTF("%s: profPptp->listenIp = 0x%x\n", __FUNCTION__, profPptp->listenIp);
#if (STATIC_NAT == FYES)
	/* pptp静态映射变化 */
	if (profPptp->head.active == 1) {
	    profNat = (StaticNatProfile *)ProfGetInstPointByName(mibTypeNat, PPTP_NAT_NAME);
	    if (profNat != NULL) {
		profNat->IP = htonl(lanIp);
		LC_PRINTF("%s: pptp - profNat->IP=0x%x\n", __FUNCTION__, profNat->IP);
	    }
	}
#endif
    }
#endif

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
#if (PORT_CONF_REDUCED == FYES)
    if(oldMode != profIF->ether.speedMode) {
	sleep(4U);
    }
#endif
out:
#if (UTT_SMART_UI == FYES)
     setSuccessTips();
    websRedirect(wp, "lanConfig.asp");
#else
    websRedirect(wp, "LANConfig.asp");
#endif
}

/*
 * lan页面output函数
 */
static int aspOutputLANConfig(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
#if (UTT_SMART_UI == FYES)
      getTheRightMsg(eid,wp,argc,argv);
#endif

#if (UTT_SMART_UI == FYES)
    DhcpServerProfile *profDhcpS = NULL;
    MibProfileType mibType = MIB_PROF_DHCP_SERVER;

    profDhcpS = (DhcpServerProfile*)ProfGetInstPointByIndex(mibType, 0);
    websWrite(wp, "var dnspEnbls=\"%s\";",
    MibGetStrByEnum(profDhcpS->DnsProxyEn, enDisableEnum));
#endif
#if(UTT_IPAUTOREP==  FYES)
    SystemProfile *profSystem=NULL;//MIB_PROF_SYSTEM
    profSystem=(SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    websWrite(wp, "var CheckIPEnbls=\"%s\";",MibGetStrByEnum(profSystem->CheckIPEn, enDisableEnum));   
#endif
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
#if (MULTI_LAN == FYES)
#if UTT_NV_WX20S
	websWrite(wp, "var multiLan = \"0\";");
#else
	websWrite(wp, "var multiLan = \"1\";");
#endif
#else
	websWrite(wp, "var multiLan = \"0\";");
#endif
#if (BRAND_LEVEL_ONE == FYES)
	websWrite(wp, "var DomainAccess = \"1\";");
#else
	websWrite(wp, "var DomainAccess = \"0\";");
#endif
    if ((profIF == NULL) || (profIF->head.active == 0)) {
	websWrite(wp, "var lanIps = \"\";");
	websWrite(wp, "var lanNMs = \"\";");
#if (MULTI_LAN == FYES)
	websWrite(wp, "var lanIp2s = \"\";");
	websWrite(wp, "var lanNM2s = \"\";");
	websWrite(wp, "var lanIp3s = \"\";");
	websWrite(wp, "var lanNM3s = \"\";");
	websWrite(wp, "var lanIp4s = \"\";");
	websWrite(wp, "var lanNM4s = \"\";");
#endif
	websWrite(wp, "var lanMacs = \"\";");
    } else {
	websWrite(wp, "var lanIps = \"%N\";", profIF->ether.sta.ip);
	websWrite(wp, "var lanNMs = \"%N\";", profIF->ether.sta.nm);
#if (MULTI_LAN == FYES)
	websWrite(wp, "var lanIp2s = \"%N\";", profIF->ether.sta.ip2);
	websWrite(wp, "var lanNM2s = \"%N\";", profIF->ether.sta.nm2);
	websWrite(wp, "var lanIp3s = \"%N\";", profIF->ether.sta.ip3);
	websWrite(wp, "var lanNM3s = \"%N\";", profIF->ether.sta.nm3);
	websWrite(wp, "var lanIp4s = \"%N\";", profIF->ether.sta.ip4);
	websWrite(wp, "var lanNM4s = \"%N\";", profIF->ether.sta.nm4);
#endif
	websWrite(wp, "var lanMacs = \"%s\";", converMac6To12Byte(profIF->ether.mac));
#if 1
#if (PORT_CONF_REDUCED == FYES)
	websWrite(wp, "var lanModes = %d;",profIF->ether.speedMode );
#ifdef CONFIG_SWITCH_EXT
		websWrite(wp, "var maxSpeed=%d;", -1);
#else
		if(MAX_SPDMODE_RDD==10){
		    websWrite(wp, "var maxSpeed=%d;", 10);
		}else if(MAX_SPDMODE_RDD==100){
		    websWrite(wp, "var maxSpeed=%d;", 100);
		}else if(MAX_SPDMODE_RDD==1000){
		    websWrite(wp, "var maxSpeed=%d;", 1000);
		}else {
		    websWrite(wp, "var maxSpeed=%d;", -1);
		}
#endif
#else
	//websWrite(wp, "var lanModes = %d;", -1);
	websWrite(wp, "var maxSpeed=%d;", -1);
#endif
#endif
    }
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

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

/*
 * 页面句柄
 */
extern void websDefineLanConfig(void) {
    websAspDefine(T("outputLANConfig"), aspOutputLANConfig);
    websFormDefine(T("ConfigLANConfig"), formLanConfig);
    websAspDefine(T("getLanMac"), getLanMac);

    return;
}

