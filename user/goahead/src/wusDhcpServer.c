
#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include	"wuScript.h"
#include        <string.h>
#include	"translate.h"
#include	"ifmisc.h"
#include        "comExeFun.h"

#if (DHCP_SERVER == FYES)
#define SWORD_DEBUG 0
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif
#if (DHCP_AUTO_BIND == FYES)
extern ARP_MATCH matchFromArpBinds(IPADDR ipAddr, MacAddr macAddr, int* arpIndex);
static void formDhcpAutoBindConfig(webs_t wp, char_t *path, char_t *query);
static int getDhcpAutoBind(int eid, webs_t wp, int argc, char_t **argv);
#endif
extern char* converMac6To12Byte(MacAddr macAddr);
#if (ARP_BINDS == FYES)
extern void webArpBindDelAll(webs_t wp);
extern void webArpBindDel(webs_t wp);
extern void webConfigArpBindConfig(webs_t wp);
#endif
static void formDhcpServerConfig(webs_t wp, char_t *path, char_t *query)
{
    char_t  *dhcp_tp,*dhcp_s, *dhcp_e, *dhcp_m, *dhcp_pd, *dhcp_sd, *dhcp_g, *dhcp_l;
#if (UTT_SMART_UI == FYES)
    /*启用新的UI后，新定义的变量*/
    char if_addr[16];/*存放LAN口地址*/
#else
    char_t *dhcp_de, *DhcpIspDns, *DhcpIspDns2;
#endif
    Uint32 lanIp = 0;
    int i;
    unsigned int nMaxIp;
    MibProfileType mibType = MIB_PROF_DHCP_SERVER;
    DhcpServerProfile *profDhcpS = NULL;
    struct in_addr addrS, addrE, addrNm, addrGw;
    InterfaceProfile *profIf = NULL;
    struProfAlloc *profhead = NULL;
    IpPoolEnum cfPoolType = IP_POOL_NONE;
	const char *ErrMsg = NULL;
#if (MULTI_SSID_DHCP_POOL == FYES)
    char_t  *ssid1_dhcp_switch,*ssid1_dhcp_start,*ssid1_dhcp_end;
    char_t  *ssid2_dhcp_switch,*ssid2_dhcp_start,*ssid2_dhcp_end;
    struct in_addr ssid1_addrS, ssid1_addrE;
    struct in_addr ssid2_addrS, ssid2_addrE;
    ProfileEnableEnum ssid1_dhcp_enable = PROF_DISABLE;
    ProfileEnableEnum ssid2_dhcp_enable = PROF_DISABLE;
#if (WIRELESS_5G == FYES)
    char_t  *ssid1_dhcp_switch_5g,*ssid1_dhcp_start_5g,*ssid1_dhcp_end_5g;
    char_t  *ssid2_dhcp_switch_5g,*ssid2_dhcp_start_5g,*ssid2_dhcp_end_5g;
    struct in_addr ssid1_addrS_5g, ssid1_addrE_5g;
    struct in_addr ssid2_addrS_5g, ssid2_addrE_5g;
    ProfileEnableEnum ssid1_dhcp_enable_5g = PROF_DISABLE;
    ProfileEnableEnum ssid2_dhcp_enable_5g = PROF_DISABLE;
#endif
#endif
#if (DHCP_SERVER_OPTION_43 == FYES)
    char_t *option43Type, *option43Addr;
#endif
   
    dhcp_tp = websGetVar(wp, T("lanDhcpType"), T("DISABLE"));
    dhcp_s = websGetVar(wp, T("dhcpStart"), T(""));
    dhcp_e = websGetVar(wp, T("dhcpEnd"), T(""));
#if (MULTI_SSID_DHCP_POOL == FYES)
    ssid1_dhcp_switch = websGetVar(wp, T("ssid1_dhcp_switch"), T("OFF"));
    ssid1_dhcp_start = websGetVar(wp, T("ssid1_dhcpStart"), T("192.168.1.52"));
    ssid1_dhcp_end = websGetVar(wp, T("ssid1_dhcpEnd"), T("192.168.1.101"));

    ssid2_dhcp_switch = websGetVar(wp, T("ssid2_dhcp_switch"), T("OFF"));
    ssid2_dhcp_start = websGetVar(wp, T("ssid2_dhcpStart"), T("192.168.1.102"));
    ssid2_dhcp_end = websGetVar(wp, T("ssid2_dhcpEnd"), T("192.168.1.151"));
#if (WIRELESS_5G == FYES)
    ssid1_dhcp_switch_5g = websGetVar(wp, T("ssid1_dhcp_switch_5g"), T("OFF"));
    ssid1_dhcp_start_5g = websGetVar(wp, T("ssid1_dhcpStart_5g"), T("192.168.1.152"));
    ssid1_dhcp_end_5g = websGetVar(wp, T("ssid1_dhcpEnd_5g"), T("192.168.1.201"));

    ssid2_dhcp_switch_5g = websGetVar(wp, T("ssid2_dhcp_switch_5g"), T("OFF"));
    ssid2_dhcp_start_5g = websGetVar(wp, T("ssid2_dhcpStart_5g"), T("192.168.1.202"));
    ssid2_dhcp_end_5g = websGetVar(wp, T("ssid2_dhcpEnd_5g"), T("192.168.1.251"));
#endif
#endif
//#if (UTT_SMART_UI == FYES)
#if 0
    /*开启新的UI后，网关掩码默认为255.255.255.0*/
    dhcp_m = websGetVar(wp, T("dhcpMask"), T("255.255.255.0"));
#else
    dhcp_m = websGetVar(wp, T("dhcpMask"), T(""));
#endif
#if (UTT_SMART_UI != FYES)
    /*开启新UI后，dns代理功能不会出现在DHCP配置页面，该功能转移到了LAN口配置页面中去了*/
    dhcp_de=websGetVar(wp, T("dnspEnbl"), T(""));
#endif
    dhcp_pd = websGetVar(wp, T("dhcpPriDns"), T(""));
    dhcp_sd = websGetVarNull(wp, T("dhcpSecDns"), T("0.0.0.0"), T("0.0.0.0"));
#if 0
    /*启用新的UI后，网关地址默认为LAN口地址*/
    if (getIfIp(getLanIfName(), if_addr) != -1) {
	dhcp_g = if_addr;
    }
#else
    dhcp_g = websGetVar(wp, T("dhcpGateway"), T(""));
#endif
    dhcp_l = websGetVar(wp, T("dhcpLease"), T("86400"));
#if (UTT_SMART_UI != FYES)
    DhcpIspDns = websGetVarNull(wp, T("ISPDns"), T("0.0.0.0"), T("0.0.0.0")); 
    DhcpIspDns2 = websGetVarNull(wp, T("ISPDns2"), T("0.0.0.0"), T("0.0.0.0"));
#endif
#if (DHCP_SERVER_OPTION_43 == FYES)
    option43Type = websGetVar(wp, T("option43Type"), T(""));
    option43Addr = websGetVar(wp, T("option43Addr"), T(""));
#endif

    profDhcpS = (DhcpServerProfile*)ProfGetInstPointByIndex(mibType, 0);
    if (0 == inet_aton(dhcp_s, &addrS)) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_START_ADDR);
    	setTheErrorMsg(ErrMsg);
    	goto out;
    }
    if (0 == inet_aton(dhcp_e, &addrE)) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_END_ADDR);
    	setTheErrorMsg(ErrMsg);
    	goto out;
    }
#if (MULTI_SSID_DHCP_POOL == FYES)
    if (strncmp(ssid1_dhcp_switch,"ON",3) == 0) {
	if (0 == inet_aton(ssid1_dhcp_start, &ssid1_addrS)) {
	    ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_START_ADDR_SSID1);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	if (0 == inet_aton(ssid1_dhcp_end, &ssid1_addrE)) {
	    ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_END_ADDR_SSID1);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	ssid1_dhcp_enable = PROF_ENABLE;
    }
    if (strncmp(ssid2_dhcp_switch,"ON",3) == 0) {
	if (0 == inet_aton(ssid2_dhcp_start, &ssid2_addrS)) {
	    ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_START_ADDR_SSID2);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	if (0 == inet_aton(ssid2_dhcp_end, &ssid2_addrE)) {
	    ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_END_ADDR_SSID2);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	ssid2_dhcp_enable = PROF_ENABLE;
    }
#if (WIRELESS_5G == FYES)
    if (strncmp(ssid1_dhcp_switch_5g,"ON",3) == 0) {
	if (0 == inet_aton(ssid1_dhcp_start_5g, &ssid1_addrS_5g)) {
	    ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_START_ADDR_SSID1_5G);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	if (0 == inet_aton(ssid1_dhcp_end_5g, &ssid1_addrE_5g)) {
	    ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_END_ADDR_SSID1_5G);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	ssid1_dhcp_enable_5g = PROF_ENABLE;
    }
    if (strncmp(ssid2_dhcp_switch_5g,"ON",3) == 0) {
	if (0 == inet_aton(ssid2_dhcp_start_5g, &ssid2_addrS_5g)) {
	    ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_START_ADDR_SSID2_5G);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	if (0 == inet_aton(ssid2_dhcp_end_5g, &ssid2_addrE_5g)) {
	    ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_END_ADDR_SSID2_5G);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	ssid2_dhcp_enable_5g = PROF_ENABLE;
    }
#endif
#endif
    if (0 == inet_aton(dhcp_m, &addrNm)) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_MASK_ADDR);
    	setTheErrorMsg(ErrMsg);
    	goto out;
    }
    if (0 == inet_aton(dhcp_g, &addrGw)) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_GATEWAY);
    	setTheErrorMsg(ErrMsg);
    	goto out;
    }
    /*
     * 判断地址池
     */
    cfPoolType = ipPoolConflict(IP_POOL_DHCP, addrS.s_addr, addrE.s_addr);
    if (cfPoolType != IP_POOL_NONE) {
#if (MULTI_SSID_DHCP_POOL == FYES)
	setPoolConflictMsgNew(cfPoolType,IP_POOL_DHCP_WIRED);
#else
	setPoolConflictMsg(cfPoolType);
#endif
	goto out;
    }
#if (MULTI_SSID_DHCP_POOL == FYES)
    if (ssid1_dhcp_enable == PROF_ENABLE) {
	cfPoolType = ipPoolConflict(IP_POOL_DHCP, ssid1_addrS.s_addr, ssid1_addrE.s_addr);
	if (cfPoolType != IP_POOL_NONE) {
	    /*ssid1地址池冲突*/
	    setPoolConflictMsgNew(cfPoolType,IP_POOL_DHCP_SSID1);
	    goto out;
	}
	if (ipSegmentOverlap(ntohl(addrS.s_addr),ntohl(addrE.s_addr),ntohl(ssid1_addrS.s_addr),ntohl(ssid1_addrE.s_addr))) {
	    setDhcpPoolConflictMsg(IP_POOL_DHCP_SSID1,IP_POOL_DHCP_WIRED);
	    goto out;
	}
    }
    if (ssid2_dhcp_enable == PROF_ENABLE) {
	cfPoolType = ipPoolConflict(IP_POOL_DHCP, ssid2_addrS.s_addr, ssid2_addrE.s_addr);
	if (cfPoolType != IP_POOL_NONE) {
	    /*ssid2地址池冲突*/
	    setPoolConflictMsgNew(cfPoolType,IP_POOL_DHCP_SSID2);
	    goto out;
	}
	if (ipSegmentOverlap(ntohl(addrS.s_addr),ntohl(addrE.s_addr),ntohl(ssid2_addrS.s_addr),ntohl(ssid2_addrE.s_addr))) {
	    setDhcpPoolConflictMsg(IP_POOL_DHCP_SSID2,IP_POOL_DHCP_WIRED);
	    goto out;
	}
	if (ssid1_dhcp_enable == PROF_ENABLE) {
	    if (ipSegmentOverlap(ntohl(ssid1_addrS.s_addr),ntohl(ssid1_addrE.s_addr),ntohl(ssid2_addrS.s_addr),ntohl(ssid2_addrE.s_addr))) {
		setDhcpPoolConflictMsg(IP_POOL_DHCP_SSID2,IP_POOL_DHCP_SSID1);
		goto out;
	    }
	}
    }
#if (WIRELESS_5G == FYES)
    if (ssid1_dhcp_enable_5g == PROF_ENABLE) {
	cfPoolType = ipPoolConflict(IP_POOL_DHCP, ssid1_addrS_5g.s_addr, ssid1_addrE_5g.s_addr);
	if (cfPoolType != IP_POOL_NONE) {
	    /*5G ssid1地址池冲突*/
	    setPoolConflictMsgNew(cfPoolType,IP_POOL_DHCP_SSID1_5G);
	    goto out;
	}
	if (ipSegmentOverlap(ntohl(addrS.s_addr),ntohl(addrE.s_addr),ntohl(ssid1_addrS_5g.s_addr),ntohl(ssid1_addrE_5g.s_addr))) {
	    setDhcpPoolConflictMsg(IP_POOL_DHCP_SSID1_5G,IP_POOL_DHCP_WIRED);
	    goto out;
	}

	if (ssid1_dhcp_enable == PROF_ENABLE) {
	    if (ipSegmentOverlap(ntohl(ssid1_addrS_5g.s_addr),ntohl(ssid1_addrE_5g.s_addr),ntohl(ssid1_addrS.s_addr),ntohl(ssid1_addrE.s_addr))) {
		setDhcpPoolConflictMsg(IP_POOL_DHCP_SSID1_5G,IP_POOL_DHCP_SSID1);
		goto out;
	    }
	}
	if (ssid2_dhcp_enable == PROF_ENABLE) {
	    if (ipSegmentOverlap(ntohl(ssid1_addrS_5g.s_addr),ntohl(ssid1_addrE_5g.s_addr),ntohl(ssid2_addrS.s_addr),ntohl(ssid2_addrE.s_addr))) {
		setDhcpPoolConflictMsg(IP_POOL_DHCP_SSID1_5G,IP_POOL_DHCP_SSID2);
		goto out;
	    }
	}
    }
    if (ssid2_dhcp_enable_5g == PROF_ENABLE) {
	cfPoolType = ipPoolConflict(IP_POOL_DHCP, ssid2_addrS_5g.s_addr, ssid2_addrE_5g.s_addr);
	if (cfPoolType != IP_POOL_NONE) {
	    /*5G ssid2地址池冲突*/
	    setPoolConflictMsgNew(cfPoolType,IP_POOL_DHCP_SSID2_5G);
	    goto out;
	}
	if (ipSegmentOverlap(ntohl(addrS.s_addr),ntohl(addrE.s_addr),ntohl(ssid2_addrS_5g.s_addr),ntohl(ssid2_addrE_5g.s_addr))) {
	    setDhcpPoolConflictMsg(IP_POOL_DHCP_SSID2_5G,IP_POOL_DHCP_WIRED);
	    goto out;
	}
	if (ssid1_dhcp_enable == PROF_ENABLE) {
	    if (ipSegmentOverlap(ntohl(ssid2_addrS_5g.s_addr),ntohl(ssid2_addrE_5g.s_addr),ntohl(ssid1_addrS.s_addr),ntohl(ssid1_addrE.s_addr))) {
		setDhcpPoolConflictMsg(IP_POOL_DHCP_SSID2_5G,IP_POOL_DHCP_SSID1);
		goto out;
	    }
	}
	if (ssid2_dhcp_enable == PROF_ENABLE) {
	    if (ipSegmentOverlap(ntohl(ssid2_addrS_5g.s_addr),ntohl(ssid2_addrE_5g.s_addr),ntohl(ssid2_addrS.s_addr),ntohl(ssid2_addrE.s_addr))) {
		setDhcpPoolConflictMsg(IP_POOL_DHCP_SSID2_5G,IP_POOL_DHCP_SSID2);
		goto out;
	    }
	}
	if (ssid1_dhcp_enable_5g == PROF_ENABLE) {
	    if (ipSegmentOverlap(ntohl(ssid2_addrS_5g.s_addr),ntohl(ssid2_addrE_5g.s_addr),ntohl(ssid1_addrS_5g.s_addr),ntohl(ssid1_addrE_5g.s_addr))) {
		setDhcpPoolConflictMsg(IP_POOL_DHCP_SSID2_5G,IP_POOL_DHCP_SSID1_5G);
		goto out;
	    }
	}
    }
#endif
#endif
   
#if(UTT_NV_1800N != FYES)
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);
    lanIp = ntohl(profIf->ether.sta.ip);
    if(lanIp <= ntohl(addrE.s_addr) && lanIp >= ntohl(addrS.s_addr)){
		ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN);
    	setTheErrorMsg(ErrMsg);
    	goto out;
    }
#if (MULTI_SSID_DHCP_POOL == FYES)
    if (ssid1_dhcp_enable == PROF_ENABLE) {
	if(lanIp <= ntohl(ssid1_addrE.s_addr) && lanIp >= ntohl(ssid1_addrS.s_addr)){
	    /*ssid1 DHCP地址池包含lan口地址*/
	    ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
    }
    if (ssid2_dhcp_enable == PROF_ENABLE) {
	if(lanIp <= ntohl(ssid2_addrE.s_addr) && lanIp >= ntohl(ssid2_addrS.s_addr)){
	    /*ssid2 DHCP地址池包含lan口地址*/
	    ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
    }

#if (WIRELESS_5G == FYES)
    if (ssid1_dhcp_enable_5g == PROF_ENABLE) {
	if(lanIp <= ntohl(ssid1_addrE_5g.s_addr) && lanIp >= ntohl(ssid1_addrS_5g.s_addr)){
	    /*5G ssid1 DHCP地址池包含lan口地址*/
	    ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
    }
    if (ssid2_dhcp_enable_5g == PROF_ENABLE) {
	if(lanIp <= ntohl(ssid2_addrE_5g.s_addr) && lanIp >= ntohl(ssid2_addrS_5g.s_addr)){
	    /*5G ssid2 DHCP地址池包含lan口地址*/
	    ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
    }
#endif
#endif
#else
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 1);
    if( profIf->ether.connMode == ETH_CONN_STATIC)
    {/*只有静态ip类型判断。因动态类型无法判断*/
	lanIp = ntohl(profIf->ether.sta.ip);
	if(lanIp <= ntohl(addrE.s_addr) && lanIp >= ntohl(addrS.s_addr)){
		ErrMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN);
    	setTheErrorMsg(ErrMsg);
	    goto out;
	}
    }
#endif
    
    if((addrS.s_addr & addrNm.s_addr) != (addrGw.s_addr & addrNm.s_addr)){
#if (UTT_SMART_UI == FYES)
		ErrMsg = getTransDataForC(C_LANG_INDEX_START_LAN_ADDR_DIFF);
#else
		ErrMsg = getTransDataForC(C_LANG_INDEX_START_MASK_ADDR_DIFF);
#endif
        setTheErrorMsg(ErrMsg);
   	goto out;
    }

    if((addrS.s_addr & addrNm.s_addr) != (addrE.s_addr & addrNm.s_addr)){
		ErrMsg = getTransDataForC(C_LANG_INDEX_START_END_ADDR_DIFF);
        setTheErrorMsg(ErrMsg);
   	goto out;
    }

    nMaxIp = (addrS.s_addr & addrNm.s_addr) | (~(addrNm.s_addr));     //the max ip 
    if(addrE.s_addr >= nMaxIp) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_END_ADDR_TOO_LARGE);
    	setTheErrorMsg (ErrMsg);
        goto out; 
    }

    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
    if (!strncmp(dhcp_tp, "DISABLE", 8))
        profDhcpS->enable = FUN_DISABLE;
    else if (!strncmp(dhcp_tp, "SERVER", 7)) 
        profDhcpS->enable = FUN_ENABLE;

    profDhcpS->ipStart = addrS.s_addr;
    profDhcpS->ipEnd = addrE.s_addr;
#if (MULTI_SSID_DHCP_POOL == FYES)
    profDhcpS->ssid1_dhcp_enable = ssid1_dhcp_enable;
    if (ssid1_dhcp_enable == PROF_ENABLE) {
	profDhcpS->ssid1_ipStart = ssid1_addrS.s_addr;
	profDhcpS->ssid1_ipEnd = ssid1_addrE.s_addr;
    }
    profDhcpS->ssid2_dhcp_enable = ssid2_dhcp_enable;
    if (ssid2_dhcp_enable == PROF_ENABLE) {
	profDhcpS->ssid2_ipStart = ssid2_addrS.s_addr;
	profDhcpS->ssid2_ipEnd = ssid2_addrE.s_addr;
    }
#if (WIRELESS_5G == FYES)
    profDhcpS->ssid1_dhcp_enable_5g = ssid1_dhcp_enable_5g;
    if (ssid1_dhcp_enable_5g == PROF_ENABLE) {
	profDhcpS->ssid1_ipStart_5g = ssid1_addrS_5g.s_addr;
	profDhcpS->ssid1_ipEnd_5g = ssid1_addrE_5g.s_addr;
    }
    profDhcpS->ssid2_dhcp_enable_5g = ssid2_dhcp_enable_5g;
    if (ssid2_dhcp_enable_5g == PROF_ENABLE) {
	profDhcpS->ssid2_ipStart_5g = ssid2_addrS_5g.s_addr;
	profDhcpS->ssid2_ipEnd_5g = ssid2_addrE_5g.s_addr;
    }
#endif
#endif
    profDhcpS->netmask = addrNm.s_addr;
    profDhcpS->gateway = addrGw.s_addr;
    profDhcpS->lease = strtol(dhcp_l, NULL, 10);
#if (DHCP_SERVER_OPTION_43 == FYES)
    profDhcpS->option43Type = option43Type[0] - '0';
    memset(profDhcpS->option43Addr, 0, sizeof(profDhcpS->option43Addr));
    memset(profDhcpS->option43Addr2, 0, sizeof(profDhcpS->option43Addr2));
    if (option43Type != DHCP_OPTION_43_TYPE_DISABLE)
	strncpy(profDhcpS->option43Addr, option43Addr, DHCP_OPTION_43_LEN);
    if (strlen(option43Addr) > DHCP_OPTION_43_LEN)
	strncpy(profDhcpS->option43Addr2, option43Addr+DHCP_OPTION_43_LEN, DHCP_OPTION_43_LEN);
#endif
#if (UTT_SMART_UI != FYES)
    if(strcmp(dhcp_de, "on") == 0) {
        profDhcpS->DnsProxyEn = FUN_ENABLE;
    } else {
        profDhcpS->DnsProxyEn = FUN_DISABLE;
    }
#endif
    profDhcpS->pDns = inet_addr(dhcp_pd);
    profDhcpS->sDns = inet_addr(dhcp_sd);
#if (UTT_SMART_UI != FYES)
    profDhcpS->pPDns = inet_addr(DhcpIspDns);
    profDhcpS->sPDns = inet_addr(DhcpIspDns2);
#endif
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);

    nvramWriteCommit();
out:
#if (UTT_SMART_UI == FYES)
    /*新UI启用，采用新的页面*/
    setSuccessTips();
    websRedirect(wp,"dhcpServer.asp");
#else
    websRedirect(wp,"DhcpServer.asp");
#endif
}

static int aspDhcpOption43Show(int eid, webs_t wp, int argc, char_t **argv)
{
#if (DHCP_SERVER_OPTION_43 == FYES)
    DhcpServerProfile *profDhcpS = NULL;
    MibProfileType mibType = MIB_PROF_DHCP_SERVER;
    char showBuf[DHCP_OPTION_43_LEN*2+1];
    struct in_addr addr;
    int i;

    profDhcpS = (DhcpServerProfile*)ProfGetInstPointByIndex(mibType, 0);
    memset(showBuf, 0, sizeof(showBuf));
    if (profDhcpS->option43Type == DHCP_OPTION_43_TYPE_HEX)
    {
	inet_aton(profDhcpS->option43Addr, &addr);
	sprintf(showBuf,"%02x%02x%02x%02x%02x%02x%02x%02x",
		0x2b, 0x06, 0x01, 0x04, 
#if defined(CONFIG_BOARD_P1010_C)
        (addr.s_addr>>24)&0xff, (addr.s_addr>>16)&0xff,
		(addr.s_addr>>8)&0xff, (addr.s_addr)&0xff);
#else
		(addr.s_addr)&0xff, (addr.s_addr>>8)&0xff,
		(addr.s_addr>>16)&0xff, (addr.s_addr>>24)&0xff);
#endif
    }
    else if (profDhcpS->option43Type == DHCP_OPTION_43_TYPE_ASCII)
    {
	sprintf(showBuf, "%02x%02x%02x%02x", 
		0x2b, strlen(profDhcpS->option43Addr)+2, 
		0x03, strlen(profDhcpS->option43Addr)); 
	for (i=0; i<strlen(profDhcpS->option43Addr); i++)
	    sprintf(showBuf, "%s%02x", showBuf, profDhcpS->option43Addr[i]);
    }
    else if (profDhcpS->option43Type == DHCP_OPTION_43_TYPE_COSTUM)
    {
	strcpy(showBuf, profDhcpS->option43Addr);
	if (profDhcpS->option43Addr2[0] != '\0')
	    strcat(showBuf, profDhcpS->option43Addr2);
    }

    if (strlen(showBuf) > 40)
    {
	showBuf[40] = '.';
	showBuf[41] = '.';
	showBuf[42] = '.';
	showBuf[43] = '\0';
    }
    websWrite(wp, "%s", showBuf);
#endif
}

/***********************************************************************
 * 函 数 名：   aspDhcpServerConfig
 * 功能描述：   dhcp页面错误返回函数
 * 输入参数:    eid,wp,argc,argv
 * 输出参数：	
 * 返回值：	    0
 * 创建日期：	2010-3-16
 * 修改日期：
 * 作者：       王炳荣	
 * 附加说明：	无
***********************************************************************/
static int aspDhcpServerConfig(int eid, webs_t wp, int argc, char_t **argv)
{
    DhcpServerProfile *profDhcpS = NULL;
    MibProfileType mibType = MIB_PROF_DHCP_SERVER;

    UserProfile *prof=NULL;
    int i,min,max,number=0;

    char addrBuf[DHCP_OPTION_43_LEN*2+1];

    profDhcpS = (DhcpServerProfile*)ProfGetInstPointByIndex(mibType, 0);

    websWrite(wp, "var lanDhcpTypes=\"%s\";", 
            MibGetStrByEnum(profDhcpS->enable, enDisableEnum));

    websWrite(wp,"var FixIPs = new Array();");
    ProfInstLowHigh(MIB_PROF_USER, &max, &min);

    for(i = min; i < max; i++) {
        prof = (UserProfile*)ProfGetInstPointByIndex(MIB_PROF_USER, i);
        if(prof != NULL && ProfInstIsFree(prof) == 0) {
            websWrite(wp, "FixIPs[%d]=\"%N\";", number, prof->ip);
            number++;
        }
    }

    websWrite(wp, "var number=\"%d\";", number);
    websWrite(wp, "var dhcpStarts=\"%N\";", profDhcpS->ipStart);
    websWrite(wp, "var dhcpMasks=\"%N\";", profDhcpS->netmask);
    websWrite(wp, "var dhcpEnds=\"%N\";", profDhcpS->ipEnd);
#if (MULTI_SSID_DHCP_POOL == FYES)
    websWrite(wp, "var ssid1_dhcp_enables=\"%s\";", 
            MibGetStrByEnum(profDhcpS->ssid1_dhcp_enable, enDisableEnum));
    websWrite(wp, "var ssid1_dhcpStarts=\"%N\";", profDhcpS->ssid1_ipStart);
    websWrite(wp, "var ssid1_dhcpEnds=\"%N\";", profDhcpS->ssid1_ipEnd);

    websWrite(wp, "var ssid2_dhcp_enables=\"%s\";", 
            MibGetStrByEnum(profDhcpS->ssid2_dhcp_enable, enDisableEnum));
    websWrite(wp, "var ssid2_dhcpStarts=\"%N\";", profDhcpS->ssid2_ipStart);
    websWrite(wp, "var ssid2_dhcpEnds=\"%N\";", profDhcpS->ssid2_ipEnd);
#if (WIRELESS_5G == FYES)
    websWrite(wp, "var ssid1_dhcp_enables_5g=\"%s\";", 
            MibGetStrByEnum(profDhcpS->ssid1_dhcp_enable_5g, enDisableEnum));
    websWrite(wp, "var ssid1_dhcpStarts_5g=\"%N\";", profDhcpS->ssid1_ipStart_5g);
    websWrite(wp, "var ssid1_dhcpEnds_5g=\"%N\";", profDhcpS->ssid1_ipEnd_5g);

    websWrite(wp, "var ssid2_dhcp_enables_5g=\"%s\";", 
            MibGetStrByEnum(profDhcpS->ssid2_dhcp_enable_5g, enDisableEnum));
    websWrite(wp, "var ssid2_dhcpStarts_5g=\"%N\";", profDhcpS->ssid2_ipStart_5g);
    websWrite(wp, "var ssid2_dhcpEnds_5g=\"%N\";", profDhcpS->ssid2_ipEnd_5g);
#endif
#endif
    websWrite(wp, "var dhcpGateways=\"%N\";", profDhcpS->gateway);
#if (UTT_SMART_UI != FYES)
    websWrite(wp, "var dnspEnbls=\"%s\";",
            MibGetStrByEnum(profDhcpS->DnsProxyEn, enDisableEnum));
#endif

    websWrite(wp, "var dhcpLeases=\"%d\";", profDhcpS->lease);
    websWrite(wp, "var dhcpPriDnss=\"%N\";", profDhcpS->pDns);
    websWrite(wp, "var dhcpSecDnss=\"%N\";", profDhcpS->sDns);
#if (UTT_SMART_UI != FYES)
    websWrite(wp, "var ISPDnss=\"%N\";", profDhcpS->pPDns);
    websWrite(wp, "var ISPDnss2=\"%N\";", profDhcpS->sPDns);
#endif
#if (DHCP_SERVER_OPTION_43 == FYES)
    strcpy(addrBuf, profDhcpS->option43Addr);
    if (profDhcpS->option43Addr2[0] != '\0')
	strcat(addrBuf, profDhcpS->option43Addr2);
    websWrite(wp, "var dhcpOption43Types=\"%d\";", profDhcpS->option43Type);
    websWrite(wp, "var dhcpOption43Addrs=\"%s\";", addrBuf);
#else
    websWrite(wp, "var dhcpOption43Types=\"%d\";", -1);
    websWrite(wp, "var dhcpOption43Addrs=\"%s\";", "");
#endif
    getTheErrorMsg(eid, wp, argc, argv); 
 #if (UTT_SMART_UI == FYES)
  getTheRightMsg(eid,wp,argc,argv); 
#endif
    return 0;
}

/***********************************************************************
 * 函 数 名：   getDhcpCliList
 * 功能描述：   dhcp页面返回客户端列表
 * 输入参数:    wp,eid,argc,argv
 * 输出参数：	
 * 返回值：	    0
 * 创建日期：	2010-3-16
 * 修改日期：
 * 作者：       王炳荣	
 * 附加说明：	无
***********************************************************************/
static int aspDhcpCliList(int eid, webs_t wp, int argc, char_t **argv)
{
    FILE *fp;
    int ct=0;
    char tmpmac[256]={0};
    DhcpServerProfile *profDhcpS = NULL;
    MibProfileType mibType = MIB_PROF_DHCP_SERVER;
    int i;
    struct in_addr addr;
    unsigned long expires;
	const char * TimeData;
	TimeData = getTransDataForC(C_LANG_INDEX_SECONDS);

    char_t *ips=T("UserIPs[%d]=\"%s\";");
    char_t *macs=T("UserMacs[%d]=\"%s\";");
    char_t *mask=T("UserMasks[%d]=\"%N\";");
    char_t *lefttime=T("UserLeftTimes[%d]=\"%d%s\";");

    websWrite(wp,"var UserIPs=new Array();");
    websWrite(wp,"var UserMacs=new Array();");
    websWrite(wp,"var UserMasks=new Array();");
    websWrite(wp,"var UserLeftTimes=new Array();");
    profDhcpS = (DhcpServerProfile*)ProfGetInstPointByIndex(mibType, 0);
    
    struct dhcpOfferedAddr {
    	unsigned char hostname[16];
    	unsigned char mac[16];
    	unsigned long ip;
    	unsigned long expires;
    } lease;

    doSystem("killall -q -USR1 udhcpd");
    
    fp = fopen("/var/udhcpd.leases", "r");
    if (NULL == fp)
    	return websWrite(wp,"var totalrecs=%d;",ct);
    
    while (fread(&lease, 1, sizeof(lease), fp) == sizeof(lease)) {
    	for (i = 0; i < 6; i++){
    		sprintf(tmpmac,"%s%02X",tmpmac, lease.mac[i]);
    	}
    	Conversion2StdMac(tmpmac);
    	websWrite(wp, macs,ct, tmpmac);
    	addr.s_addr = lease.ip;
    	websWrite(wp,ips ,ct, inet_ntoa(addr));
    	websWrite(wp,mask,ct,profDhcpS->netmask);
    	expires = ntohl(lease.expires);
    	websWrite(wp,lefttime ,ct, expires, TimeData);
    	memset(tmpmac,0,strlen(tmpmac));
    	ct++;
    }
    fclose(fp);
    websWrite(wp,"var totalrecs=%d;",ct);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

static void formConfigDhcpList_static(webs_t wp, char_t *path, char_t *query)
{
    char_t *ip;

    struct in_addr addr;
    DhcpServerProfile *profDhcpS = NULL;
	const char *ErrMsg = NULL;
   
    a_assert(wp);
    
    ip=websGetVar(wp,T("IP"), T(""));
    profDhcpS = (DhcpServerProfile*)ProfGetInstPointByIndex(MIB_PROF_DHCP_SERVER, 0);

    if((inet_aton(ip, &addr) == 0) || 
            ((addr.s_addr & profDhcpS->netmask) != (profDhcpS->ipStart & profDhcpS->netmask))) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_IP_PLAN_ADDR_ERR);
        setTheErrorMsg(ErrMsg);
    	goto out;
    }

    
    if((ntohl(addr.s_addr) > ntohl(profDhcpS->ipEnd)) || (ntohl(addr.s_addr) < ntohl(profDhcpS->ipStart))) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_OVERFLOW_IP_ADDR);
    	setTheErrorMsg(ErrMsg);
    	goto out;
    }

#if (ARP_BINDS == FYES)
    webConfigArpBindConfig(wp);
#endif
    out:
    websRedirect(wp, "DhcpFix.asp");
}
#if (DHCP_AUTO_BIND == FYES)

/********************************************************************
 * 函数： formDhcpAutoBindConfig
 * 功能： 页面相关信息
 * 创建： 2012-09-12
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void formDhcpAutoBindConfig(webs_t wp, char_t *path, char_t *query)
{
    char_t *chkDhcpNewAutobind= NULL, *chkDhcpDelAutobind = NULL;

    DhcpServerProfile *profDhcpS = NULL;
    struProfAlloc *profhead = NULL;
    
    chkDhcpNewAutobind = websGetVar(wp,T("chkDhcpNewAutobind"), T(""));/*从页面结构体中取值*/
    chkDhcpDelAutobind = websGetVar(wp,T("chkDhcpDelAutobind"), T(""));/*从页面结构体中取值*/
    profDhcpS = (DhcpServerProfile*)ProfGetInstPointByIndex(MIB_PROF_DHCP_SERVER, 0);

    if((profDhcpS != NULL) && (ProfInstIsFree(profDhcpS) == 0))
    {/*profile使用*/
	ProfBackupByIndex(MIB_PROF_DHCP_SERVER, PROF_CHANGE_EDIT, 0, &profhead);
	if(strcmp(chkDhcpNewAutobind, "on") == 0)
	{
	    profDhcpS->DhcpNewAutobind = FUN_ENABLE;
	}
	else
	{
	    profDhcpS->DhcpNewAutobind = FUN_DISABLE;
	}
	if(strcmp(chkDhcpDelAutobind, "on") == 0)
	{
	    profDhcpS->DhcpDelAutobind = FUN_ENABLE;
	}
	else
	{
	    profDhcpS->DhcpDelAutobind = FUN_DISABLE;
	}
    }
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit(); /*save*/
    websRedirect(wp, "DhcpAutoBind.asp");
    return;
}
/********************************************************************
 * 函数： getDhcpAutoBind
 * 功能： 页面输出相关信息
 * 创建： 2012-09-12
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int getDhcpAutoBind(int eid, webs_t wp, int argc, char_t **argv)
{
    DhcpServerProfile *profDhcpS = NULL;

    profDhcpS = (DhcpServerProfile*)ProfGetInstPointByIndex(MIB_PROF_DHCP_SERVER, 0);
    if((profDhcpS != NULL) && (ProfInstIsFree(profDhcpS) == 0))
    {
	websWrite(wp,"var DhcpNewAutobind = %d;", profDhcpS->DhcpNewAutobind);
	websWrite(wp,"var DhcpDelAutobind = %d;", profDhcpS->DhcpDelAutobind);
    }

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

#endif

static int getDhcpList_static(int eid, webs_t wp, int argc, char_t **argv)
{
    int min = 0, max = 0;
    int i = 0;
    int totalrecs = 0;
    MibProfileType mibType = MIB_PROF_USER;
    UserProfile *prof;
    char_t *UserNames=T("FixUserNames[%d] = \"%s\";");
    char_t *ips =T("FixIPs[%d] = \"%N\";");
    char_t *macs = T("FixMacs[%d] = \"%s\"; ");
    websWrite(wp,"var FixUserNames=new Array();");
    websWrite(wp,"var FixIPs = new Array();");
    websWrite(wp,"var FixMacs = new Array();");

    ProfInstLowHigh(mibType, &max, &min);

    for(i = min; i < max; i++) {
        prof = (UserProfile*)ProfGetInstPointByIndex(mibType, i);
        if(prof != NULL && ProfInstIsFree(prof) == 0) {
            websWrite(wp, UserNames, totalrecs, prof->head.name);
            websWrite(wp, ips, totalrecs, prof->ip);
#if 0
	    Conversion2StdMac(prof->userMac);
#endif
            websWrite(wp, macs, totalrecs, converMac6To12Byte(prof->mac));
            totalrecs++;
        }
    }

    websWrite(wp,"var totalrecs=%d;", totalrecs);
    websWrite(wp,"var max_totalrecs=%d;", max);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

static int getDhcpOneClientInfo(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t *edit_name = NULL;
    UserProfile *prof;
    MibProfileType mibType = MIB_PROF_USER;
    const char *ErrMsg = NULL;

    if(ejArgs(argc,argv,T("%s"), &edit_name) < 1) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_READ_EDIT_NAME_VAL_ERR);
		setTheErrorMsg(ErrMsg);
        goto out;
    }

    if(edit_name == NULL) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_NOT_NULL);
        setTheErrorMsg(ErrMsg);
        goto out;
    }

    prof = (UserProfile*)ProfGetInstPointByName(mibType, edit_name);
    if(prof == NULL) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_USER_NAME);
        setTheErrorMsg(ErrMsg);
    } else {
        websWrite(wp, "var FixUserNames=\"%s\";", prof->head.name);
        websWrite(wp, "var FixIPs=\"%N\";", prof->ip);
        websWrite(wp, "var FixMacs=\"%s\";", converMac6To12Byte(prof->mac));
    }

out:
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

static void formDhcpListStaticDel(webs_t wp, char_t *path, char_t *query)
{
#if (ARP_BINDS == FYES)
    webArpBindDel(wp);
#endif
#if 0
    char_t *UserName=NULL, *tmp = NULL;
    MibProfileType mibType = MIB_PROF_USER;
    struProfAlloc *profList  = NULL;
    int back = 0;
    a_assert(wp);
    
    UserName = websGetVar(wp,T("delstr"),T("")); 
    if(UserName == NULL) {   
        setTheErrorMsg(T("找不到该用户名!"));
        goto out;
    }

    tmp=strtok(UserName, UTT_SPLIT_SIGN_STR);
    while(tmp != NULL) {
        if(back == 0) {
            ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profList);
            back = 1;
        }
        ProfDelInstByName(mibType, tmp); 
        tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
    }

    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
out:
#endif
    websRedirect(wp,"DhcpFix.asp");
}

static void formDhcpListStaticDelAll(webs_t wp, char_t *path, char_t *query)
{
#if 0
    int min = 0, max = 0;
    int i = 0;
    int back = 0;
    MibProfileType mibType = MIB_PROF_USER;
    Pointer prof;
    struProfAlloc *profList  = NULL;

    a_assert(wp);
    ProfInstLowHigh(mibType, &max, &min);

    for(i = min; i < max; i++) {
        prof = ProfGetInstPointByIndex(mibType, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
            if(back == 0) {
                ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, i, &profList);
                back = 1;
            }
            ProfDelInstByIndex(mibType, i);
        }
    }
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
#endif
#if (ARP_BINDS == FYES)
    webArpBindDelAll(wp);
#endif
    websRedirect(wp, "DhcpFix.asp");
}

void formDefineDhcpServer(void) 
{
    websFormDefine(T("formDhcpServerConfig"), formDhcpServerConfig);
    websAspDefine(T("aspDhcpServerConfig"), aspDhcpServerConfig);
    websAspDefine(T("aspOutputDhcpOption43Show"), aspDhcpOption43Show);
    websAspDefine(T("outputDhcpStaticList"), getDhcpList_static);
    websAspDefine(T("aspOutDhcpOneClientInfo"), getDhcpOneClientInfo);
    websAspDefine(T("aspDhcpCliList"), aspDhcpCliList);
    websFormDefine(T("formDhcpListStaticDelAll"),formDhcpListStaticDelAll);
    websFormDefine(T("formDhcpListStaticDel"),formDhcpListStaticDel);
    websFormDefine(T("formDhcpListStatic"), formConfigDhcpList_static);
#if (DHCP_AUTO_BIND == FYES)
    websFormDefine(T("formDhcpAutoBindConfig"), formDhcpAutoBindConfig);
    websAspDefine(T("getDhcpAutoBind"), getDhcpAutoBind);
#endif
}

#endif
