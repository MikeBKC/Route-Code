#include "uttMachine.h"
#if (NET_SECURITY == FYES)
#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include	"argcmd.h"
#include	"a2n.h"
#include	"translate.h"
#include	"internet.h"

#ifdef  NET_SE_DBG
#define NET_SE_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define NET_SE_PRINTF(fmt, args...)
#endif

static bool getAcccessCtrlIps(webs_t wp, IPADDR* ipFromInt, IPADDR* ipEndInt);
static void aspOutAccessCtrl(webs_t wp, SystemProfile*profSys) 
{
    struct in_addr addrS;
    InterfaceProfile *profIF = NULL;

    memset(&addrS, 0, sizeof(addrS));

	if (profSys->sysNetSe.lanArpBroadcastEn == FUN_DISABLE) {
		websWrite(wp,"var ArpBroadcastEnables=\"%s\";", T("off"));
		websWrite(wp,"var lanArpBroadcastInterval=\"%d\";", profSys->sysNetSe.lanArpBroadcastInterval);
	} else {
		websWrite(wp,"var ArpBroadcastEnables=\"%s\";", T("on"));
		websWrite(wp,"var lanArpBroadcastInterval=\"%d\";", profSys->sysNetSe.lanArpBroadcastInterval);
	}
    /*这里通过配置判断。而不是实时ip地址。是因为这样判断可以保证配置的正确性*/
    profIF = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);/*获取lan口配置*/

#if (MULTI_LAN == FYES)
    websWrite(wp, "var multiLan = \"1\";");
    addrS.s_addr = profIF->ether.sta.ip2;
    websWrite(wp, "var lanIP2=\"%s\";", inet_ntoa(addrS));/*lan口ip*/
    addrS.s_addr = profIF->ether.sta.nm2;
    websWrite(wp, "var lanMask2=\"%s\";", inet_ntoa(addrS));/*lan口ip掩码*/
    addrS.s_addr = profIF->ether.sta.ip3;
    websWrite(wp, "var lanIP3=\"%s\";", inet_ntoa(addrS));/*lan口ip*/
    addrS.s_addr = profIF->ether.sta.nm3;
    websWrite(wp, "var lanMask3=\"%s\";", inet_ntoa(addrS));/*lan口ip掩码*/
    addrS.s_addr = profIF->ether.sta.ip4;
    websWrite(wp, "var lanIP4=\"%s\";", inet_ntoa(addrS));/*lan口ip*/
    addrS.s_addr = profIF->ether.sta.nm4;
    websWrite(wp, "var lanMask4=\"%s\";", inet_ntoa(addrS));/*lan口ip掩码*/
#else
    websWrite(wp, "var multiLan = \"0\";");
#endif
    addrS.s_addr = profIF->ether.sta.ip;
    websWrite(wp, "var lanIP=\"%s\";", inet_ntoa(addrS));/*lan口ip*/
    addrS.s_addr = profIF->ether.sta.nm;
    websWrite(wp, "var lanMask=\"%s\";", inet_ntoa(addrS));/*lan口ip掩码*/

    NET_SE_PRINTF("%s------%d, profSys->sysNetSe.accessCtrlFromIP = %x\n", __func__, __LINE__, profSys->sysNetSe.accessCtrlFromIP);
    if((profSys->sysNetSe.accessCtrlFromIP == 0u) || (profSys->sysNetSe.accessCtrlEndIP == 0u))
    {
	websWrite(wp,"var accessEnable=\"%s\";", T(""));/*起始ip都为零。则为不开启*/
	websWrite(wp,"var allowFromIP=\"%s\";", wp->ipaddr);/*访问者ip地址*/
	websWrite(wp,"var allowEndIP=\"%s\";", wp->ipaddr);
    }
    else
    {
	websWrite(wp,"var accessEnable=\"%s\";", T("on"));
	addrS.s_addr = profSys->sysNetSe.accessCtrlFromIP;
	websWrite(wp,"var allowFromIP=\"%s\";", inet_ntoa(addrS));/*起始ip地址*/

	addrS.s_addr = profSys->sysNetSe.accessCtrlEndIP;
	websWrite(wp,"var allowEndIP=\"%s\";", inet_ntoa(addrS));/*结束ip地址*/
    }
    return;
}
/*
 * 页面输出
 */
static int aspOutSecDefense(int eid, webs_t wp, int argc, char_t **argv) {
    SystemProfile *profSys = NULL;

    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);

    if (profSys->sysNetSe.antiDDoSEn == FUN_ENABLE) {
	websWrite(wp,"var DDOSEnables=\"%s\";", T("on"));
    } else {
	websWrite(wp,"var DDOSEnables=\"%s\";", T("off"));
    }
    if (profSys->sysNetSe.antiIPCheatEn == FUN_ENABLE) {
	websWrite(wp,"var IPCheatw =\"%s\";", T("on"));
    } else {
	websWrite(wp,"var IPCheatw =\"%s\";", T("off"));
    }
    if (profSys->sysNetSe.antiUDPFloodEn == FUN_ENABLE) {
	websWrite(wp,"var UDPFloodw =\"%s\";", T("on"));
	websWrite(wp,"var max_udp_rxpps =\"%d\";", profSys->sysNetSe.max_udp_rxpp);
    } else {
	websWrite(wp,"var UDPFloodw =\"%s\";", T("off"));
	websWrite(wp,"var max_udp_rxpps =\"%d\";", profSys->sysNetSe.max_udp_rxpp);
    }
    if (profSys->sysNetSe.antiICMPFloodEn== FUN_ENABLE) {
	websWrite(wp,"var ICMPFloodw =\"%s\";", T("on"));
	websWrite(wp,"var max_icmp_rxpps =\"%d\";", profSys->sysNetSe.max_icmp_rxpp);
    } else {
	websWrite(wp,"var ICMPFloodw =\"%s\";", T("off"));
	websWrite(wp,"var max_icmp_rxpps =\"%d\";", profSys->sysNetSe.max_icmp_rxpp);
    }
    if (profSys->sysNetSe.antiSYNFloodEn == FUN_ENABLE) {
	websWrite(wp,"var SYNFloodw =\"%s\";", T("on"));
	websWrite(wp,"var max_syn_rxpps =\"%d\";", profSys->sysNetSe.max_syn_rxpp);
    } else {
	websWrite(wp,"var SYNFloodw =\"%s\";", T("off"));
	websWrite(wp,"var max_syn_rxpps =\"%d\";", profSys->sysNetSe.max_syn_rxpp);
    }
    if (profSys->sysNetSe.antiportScanfEn == FUN_ENABLE) {
	websWrite(wp,"var portScanfEnablew =\"%s\";", T("on"));
	websWrite(wp,"var scanfCnt =\"%d\";", profSys->sysNetSe.scanfCnt);
    } else {
	websWrite(wp,"var portScanfEnablew =\"%s\";", T("off"));
	websWrite(wp,"var scanfCnt =\"%d\";", profSys->sysNetSe.scanfCnt);
    }
    if (profSys->sysNetSe.antiBlasterEn == FUN_ENABLE) {
	websWrite(wp,"var WormEnables=\"%s\";", T("on"));
    } else {
	websWrite(wp,"var WormEnables=\"%s\";", T("off"));
    }
  if (profSys->sysNetSe.antiPingReplyEn == FUN_ENABLE) {
	websWrite(wp,"var PingDisables=\"%s\";", T("on"));
    } else {
	websWrite(wp,"var PingDisables=\"%s\";", T("off"));
    }
    aspOutAccessCtrl(wp, profSys);/*打印设备访问控制相关*/
    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

/********************************************************************
 * 函数： 获取页面设备访问控制相关数据
 * 输出： 无
 * 返回： BOOL
 * 作者： bhou
 * 日期： 2011-12-28
 *********************************************************************/ 
static bool getAcccessCtrlIps(webs_t wp, IPADDR* ipFromInt, IPADDR* ipEndInt)
{
#if 0
    Uint32 sys_ip=0u,sys_mask=0u;
#if (MULTI_LAN == FYES)
    Uint32 sys_ip2=0u,sys_mask2=0u;
    Uint32 sys_ip3=0u,sys_mask3=0u;
    Uint32 sys_ip4=0u,sys_mask4=0u;
#endif
    InterfaceProfile *profIF = NULL;
#endif
    IPADDR ip_from_int = 0u, ip_end_int = 0u;
    Bool ret = 1;
    char *accessCtlEn = NULL, *ipStr = NULL;
    struct in_addr addrS;
    memset(&addrS, 0, sizeof(addrS));

    accessCtlEn = websGetVar(wp, T("accessEnablew"), T(""));
    if(strcmp(accessCtlEn, "on")==0)/*开启设备访问控制*/
    {
	ipStr = websGetVar(wp, T("DesFromIP"), T(""));
	if(inet_aton(ipStr, &addrS)!=0)/*转化ip地址*/
	{
	    *ipFromInt = addrS.s_addr;
	    ipStr = websGetVar(wp, T("DesEndIP"), T(""));
	    
	    if(inet_aton(ipStr, &addrS)!=0)/*转化ip地址*/
	    {
		*ipEndInt = addrS.s_addr;

		/*转换为主机字节序*/
		ip_from_int = ntohl(*ipFromInt);
		ip_end_int = ntohl(*ipEndInt);
		if(ip_from_int <= ip_end_int)
		{/*起始地址不能大于结束地址*/

#if 0
		    /*这里通过配置判断。而不是实时ip地址。是因为这样判断可以保证配置的正确性*/
		    profIF = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);/*获取lan口配置*/
		    sys_ip = ntohl(profIF->ether.sta.ip);
		    sys_mask = ntohl(profIF->ether.sta.nm);
		    sys_ip = sys_ip & sys_mask ;/*管理ip所在网段*/
#if (MULTI_LAN == FYES)
		    sys_ip2 = ntohl(profIF->ether.sta.ip2);
		    sys_mask2 = ntohl(profIF->ether.sta.nm2);
		    sys_ip2 = sys_ip2 & sys_mask2 ;/*管理ip所在网段*/
		    sys_ip3 = ntohl(profIF->ether.sta.ip3);
		    sys_mask3 = ntohl(profIF->ether.sta.nm3);
		    sys_ip3 = sys_ip3 & sys_mask3 ;/*管理ip所在网段*/
		    sys_ip4 = ntohl(profIF->ether.sta.ip4);
		    sys_mask4 = ntohl(profIF->ether.sta.nm4);
		    sys_ip4 = sys_ip4 & sys_mask4 ;/*管理ip所在网段*/
#endif

		    
		    if( (((ip_from_int&sys_mask) != sys_ip ) || ((ip_end_int&sys_mask) != sys_ip)
			    || (ip_end_int ==sys_ip) || (ip_from_int== (sys_ip | (~sys_mask)))) 
#if (MULTI_LAN == FYES)
		    && (((ip_from_int&sys_mask2) != sys_ip2 ) || ((ip_end_int&sys_mask2) != sys_ip2)
			    || (ip_end_int ==sys_ip2) || (ip_from_int== (sys_ip2 | (~sys_mask2)))) 
		    && (((ip_from_int&sys_mask3) != sys_ip3 ) || ((ip_end_int&sys_mask3) != sys_ip3)
			    || (ip_end_int ==sys_ip3) || (ip_from_int== (sys_ip3 | (~sys_mask3)))) 
		    && (((ip_from_int&sys_mask4) != sys_ip4 ) || ((ip_end_int&sys_mask4) != sys_ip4)
			    || (ip_end_int ==sys_ip4) || (ip_from_int== (sys_ip4 | (~sys_mask4)))) 
#endif
		      )
		    {/*必须跟本机 lan ip在同一网段，且至少包含一个ip地址*/
			ret = 0;
		    }
		    else
		    {
			ret = 1;
		    }
#else
		    ret = 1;
#endif
		}
	    }
	}
    }
    else
    {/*未开启设备访问控制*/
	*ipFromInt = 0u;
	*ipEndInt = 0u;
	ret = 1;
    }
    return ret;
}
/*
 * 页面保存
 */
static void formFwBase(webs_t wp, char_t *path, char_t *query) {
    char *DDoSEn = NULL, *BlasterEn = NULL, *LanArpBroadcastEN = NULL;
    char *IPCheat = NULL, *UDPFlood = NULL, *max_udp_rxpps = NULL, *ICMPFlood = NULL, *max_icmp_rxpps = NULL;
    char *SYNFlood = NULL, *max_syn_rxpps = NULL, *portScanfEnable = NULL, *scanfCnt = NULL;
    IPADDR ipFromInt = 0u, ipEndInt = 0u;
	int LanArpBroadcastInterval = 0;
	char *GetValStr = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    ProfChangeType profAction;
    struProfAlloc *profhead = NULL;
	const char *ErrMsg = NULL;

    DDoSEn = websGetVar(wp, T("DDOSEnable"), T(""));
    BlasterEn = websGetVar(wp, T("WormEnable"), T(""));
    IPCheat = websGetVar(wp, T("IPCheat"), T(""));
    UDPFlood = websGetVar(wp, T("UDPFlood"), T(""));
    max_udp_rxpps = websGetVar(wp, T("max_udp_rxpps"), T(""));
    ICMPFlood = websGetVar(wp, T("ICMPFlood"), T(""));
    max_icmp_rxpps = websGetVar(wp, T("max_icmp_rxpps"), T(""));
    SYNFlood = websGetVar(wp, T("SYNFlood"), T(""));
    max_syn_rxpps = websGetVar(wp, T("max_syn_rxpps"), T(""));
    portScanfEnable = websGetVar(wp, T("portScanfEnable"), T(""));
    scanfCnt = websGetVar(wp, T("scanfCnt"), T(""));
	LanArpBroadcastEN = websGetVar(wp, T("ArpBroadcastEnable"), T(""));
    NET_SE_PRINTF("%s: DDoSEn = %s, BlasterEn = %s\n", __FUNCTION__, DDoSEn, BlasterEn);
    NET_SE_PRINTF("%s - %d: IPCheat = %s, UDPFlood = %s, max_udp_rxpps = %s, ICMPFlood =%s, max_icmp_rxpps = %s, SYNFlood = %s, max_syn_rxpps = %s, portScanfEnable = %s, scanfCnt = %s\n", 
	    __func__, __LINE__, IPCheat, UDPFlood, max_udp_rxpps, ICMPFlood, max_icmp_rxpps, SYNFlood, max_syn_rxpps, portScanfEnable, scanfCnt);
    if ((DDoSEn == NULL) || (BlasterEn == NULL) || LanArpBroadcastEN == NULL) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_OPERATION_FAIL);
		setTheErrorMsg(ErrMsg);
		goto out;
    }
    if(getAcccessCtrlIps(wp, &ipFromInt, &ipEndInt)==0)
    {
		ErrMsg = getTransDataForC(C_LANG_INDEX_DECIVE_ACCESS_CTL_IP_ERR);
		setTheErrorMsg(ErrMsg);
		goto out;
    }

    profAction = PROF_CHANGE_EDIT;
    ProfBackupByIndex(mibType, profAction, 0, &profhead);
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);
	GetValStr = websGetVar(wp, T("ArpBroadcastIntervalVal"), T(""));
	if (GetValStr == NULL) {
		goto out; //页面获取到的value是空的时候
	}
	LanArpBroadcastInterval = atoi(GetValStr);
	if (LanArpBroadcastInterval < 100 || LanArpBroadcastInterval > 5000) {
		goto out; //页面获取到的value不在准许取值范围（页面判断一次，这里判断一次）
	}
    if (strcmp(DDoSEn, "on") == 0) {
	profSys->sysNetSe.antiDDoSEn = FUN_ENABLE;
    } else {
	profSys->sysNetSe.antiDDoSEn = FUN_DISABLE;
    }
    if (strcmp(BlasterEn, "on") == 0) {
	profSys->sysNetSe.antiBlasterEn = FUN_ENABLE;
    } else {
	profSys->sysNetSe.antiBlasterEn = FUN_DISABLE;
    }
    if (strcmp(IPCheat, "on") == 0) {
	profSys->sysNetSe.antiIPCheatEn = FUN_ENABLE;
    } else {
	profSys->sysNetSe.antiIPCheatEn = FUN_DISABLE;
    }
    if (strcmp(UDPFlood, "on") == 0) {
	profSys->sysNetSe.antiUDPFloodEn = FUN_ENABLE;
    } else {
	profSys->sysNetSe.antiUDPFloodEn = FUN_DISABLE;
    }
    if (strcmp(ICMPFlood, "on") == 0) {
	profSys->sysNetSe.antiICMPFloodEn = FUN_ENABLE;
    } else {
	profSys->sysNetSe.antiICMPFloodEn = FUN_DISABLE;
    }
    if (strcmp(SYNFlood, "on") == 0) {
	profSys->sysNetSe.antiSYNFloodEn = FUN_ENABLE;
    } else {
	profSys->sysNetSe.antiSYNFloodEn = FUN_DISABLE;
    }
    if (strcmp(portScanfEnable, "on") == 0) {
	profSys->sysNetSe.antiportScanfEn = FUN_ENABLE;
    } else {
	profSys->sysNetSe.antiportScanfEn = FUN_DISABLE;
    }
    if (strcmp(LanArpBroadcastEN, "on") == 0) {
	profSys->sysNetSe.lanArpBroadcastEn = FUN_ENABLE;
    } else {
	profSys->sysNetSe.lanArpBroadcastEn = FUN_DISABLE;
    }
    profSys->sysNetSe.max_udp_rxpp = atoi(max_udp_rxpps);
    profSys->sysNetSe.max_icmp_rxpp = atoi(max_icmp_rxpps);
    profSys->sysNetSe.max_syn_rxpp = atoi(max_syn_rxpps);
    profSys->sysNetSe.scanfCnt = atoi(scanfCnt);
    profSys->sysNetSe.lanArpBroadcastInterval = LanArpBroadcastInterval;
    profSys->sysNetSe.accessCtrlFromIP = ipFromInt;
    profSys->sysNetSe.accessCtrlEndIP = ipEndInt;
    NET_SE_PRINTF("%s------%d, profSys->sysNetSe.accessCtrlFromIP = %x\n", __func__, __LINE__, profSys->sysNetSe.accessCtrlFromIP);
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
    websRedirect(wp, T("FwBase.asp"));
    return;
}

/*
 * 页面保存
 */
static void formFwBaseOut(webs_t wp, char_t *path, char_t *query) {
    char *PingEn;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    ProfChangeType profAction;
    struProfAlloc *profhead = NULL;
	const char * ErrMsg = NULL;
   
    PingEn = websGetVar(wp, T("PingDisable"), T(""));
    NET_SE_PRINTF("%s: PingEn = %s\n", __FUNCTION__, PingEn);
    if (PingEn == NULL) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_OPERATION_FAIL);
    	setTheErrorMsg(ErrMsg);
	goto out;
    }
    profAction = PROF_CHANGE_EDIT;
    ProfBackupByIndex(mibType, profAction, 0, &profhead);
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);
    if (strcmp(PingEn, "on") == 0) {
	profSys->sysNetSe.antiPingReplyEn = FUN_ENABLE;
    } else {
	profSys->sysNetSe.antiPingReplyEn = FUN_DISABLE;
    }
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
    websRedirect(wp, T("FwBaseOut.asp"));

    return;
}

#if (UTT_SMART_UI == FYES)
static int aspOutAccessControl(int eid, webs_t wp, int argc, char_t **argv) {
    SystemProfile *profSys = NULL;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    aspOutAccessCtrl(wp, profSys);/*打印设备访问控制相关*/
    getTheErrorMsg(eid, wp, argc, argv);
    getTheRightMsg(eid,wp,argc,argv);
    return 0;
}

static void formAccessCtrl(webs_t wp, char_t *path, char_t *query) {
    IPADDR ipFromInt = 0u, ipEndInt = 0u;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    ProfChangeType profAction;
    struProfAlloc *profhead = NULL;
    const char *ErrMsg = NULL;

    if(getAcccessCtrlIps(wp, &ipFromInt, &ipEndInt)==0)
    {
		ErrMsg = getTransDataForC(C_LANG_INDEX_DECIVE_ACCESS_CTL_IP_ERR);
		setTheErrorMsg(ErrMsg);
		goto out;
    }

    profAction = PROF_CHANGE_EDIT;
    ProfBackupByIndex(mibType, profAction, 0, &profhead);
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);

    profSys->sysNetSe.accessCtrlFromIP = ipFromInt;
    profSys->sysNetSe.accessCtrlEndIP = ipEndInt;

    NET_SE_PRINTF("%s------%d, profSys->sysNetSe.accessCtrlFromIP = %x\n", __func__, __LINE__, profSys->sysNetSe.accessCtrlFromIP);

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
    setSuccessTips();
    websRedirect(wp, "AccessControl_Sui.asp");
    return;
}
#endif

extern void websDefineFwBase(void) {
    websAspDefine(T("aspOutSecDefense"), aspOutSecDefense);
    websFormDefine(T("formFwBase"), formFwBase);
    websFormDefine(T("formFwBaseOut"), formFwBaseOut);
#if (UTT_SMART_UI == FYES)
    websAspDefine(T("aspOutAccessControl"), aspOutAccessControl);
    websFormDefine(T("formAccessCtrl"), formAccessCtrl);
#endif
    return;
}
#endif
