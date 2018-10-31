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
    /*����ͨ�������жϡ�������ʵʱip��ַ������Ϊ�����жϿ��Ա�֤���õ���ȷ��*/
    profIF = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);/*��ȡlan������*/

#if (MULTI_LAN == FYES)
    websWrite(wp, "var multiLan = \"1\";");
    addrS.s_addr = profIF->ether.sta.ip2;
    websWrite(wp, "var lanIP2=\"%s\";", inet_ntoa(addrS));/*lan��ip*/
    addrS.s_addr = profIF->ether.sta.nm2;
    websWrite(wp, "var lanMask2=\"%s\";", inet_ntoa(addrS));/*lan��ip����*/
    addrS.s_addr = profIF->ether.sta.ip3;
    websWrite(wp, "var lanIP3=\"%s\";", inet_ntoa(addrS));/*lan��ip*/
    addrS.s_addr = profIF->ether.sta.nm3;
    websWrite(wp, "var lanMask3=\"%s\";", inet_ntoa(addrS));/*lan��ip����*/
    addrS.s_addr = profIF->ether.sta.ip4;
    websWrite(wp, "var lanIP4=\"%s\";", inet_ntoa(addrS));/*lan��ip*/
    addrS.s_addr = profIF->ether.sta.nm4;
    websWrite(wp, "var lanMask4=\"%s\";", inet_ntoa(addrS));/*lan��ip����*/
#else
    websWrite(wp, "var multiLan = \"0\";");
#endif
    addrS.s_addr = profIF->ether.sta.ip;
    websWrite(wp, "var lanIP=\"%s\";", inet_ntoa(addrS));/*lan��ip*/
    addrS.s_addr = profIF->ether.sta.nm;
    websWrite(wp, "var lanMask=\"%s\";", inet_ntoa(addrS));/*lan��ip����*/

    NET_SE_PRINTF("%s------%d, profSys->sysNetSe.accessCtrlFromIP = %x\n", __func__, __LINE__, profSys->sysNetSe.accessCtrlFromIP);
    if((profSys->sysNetSe.accessCtrlFromIP == 0u) || (profSys->sysNetSe.accessCtrlEndIP == 0u))
    {
	websWrite(wp,"var accessEnable=\"%s\";", T(""));/*��ʼip��Ϊ�㡣��Ϊ������*/
	websWrite(wp,"var allowFromIP=\"%s\";", wp->ipaddr);/*������ip��ַ*/
	websWrite(wp,"var allowEndIP=\"%s\";", wp->ipaddr);
    }
    else
    {
	websWrite(wp,"var accessEnable=\"%s\";", T("on"));
	addrS.s_addr = profSys->sysNetSe.accessCtrlFromIP;
	websWrite(wp,"var allowFromIP=\"%s\";", inet_ntoa(addrS));/*��ʼip��ַ*/

	addrS.s_addr = profSys->sysNetSe.accessCtrlEndIP;
	websWrite(wp,"var allowEndIP=\"%s\";", inet_ntoa(addrS));/*����ip��ַ*/
    }
    return;
}
/*
 * ҳ�����
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
    aspOutAccessCtrl(wp, profSys);/*��ӡ�豸���ʿ������*/
    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

/********************************************************************
 * ������ ��ȡҳ���豸���ʿ����������
 * ����� ��
 * ���أ� BOOL
 * ���ߣ� bhou
 * ���ڣ� 2011-12-28
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
    if(strcmp(accessCtlEn, "on")==0)/*�����豸���ʿ���*/
    {
	ipStr = websGetVar(wp, T("DesFromIP"), T(""));
	if(inet_aton(ipStr, &addrS)!=0)/*ת��ip��ַ*/
	{
	    *ipFromInt = addrS.s_addr;
	    ipStr = websGetVar(wp, T("DesEndIP"), T(""));
	    
	    if(inet_aton(ipStr, &addrS)!=0)/*ת��ip��ַ*/
	    {
		*ipEndInt = addrS.s_addr;

		/*ת��Ϊ�����ֽ���*/
		ip_from_int = ntohl(*ipFromInt);
		ip_end_int = ntohl(*ipEndInt);
		if(ip_from_int <= ip_end_int)
		{/*��ʼ��ַ���ܴ��ڽ�����ַ*/

#if 0
		    /*����ͨ�������жϡ�������ʵʱip��ַ������Ϊ�����жϿ��Ա�֤���õ���ȷ��*/
		    profIF = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);/*��ȡlan������*/
		    sys_ip = ntohl(profIF->ether.sta.ip);
		    sys_mask = ntohl(profIF->ether.sta.nm);
		    sys_ip = sys_ip & sys_mask ;/*����ip��������*/
#if (MULTI_LAN == FYES)
		    sys_ip2 = ntohl(profIF->ether.sta.ip2);
		    sys_mask2 = ntohl(profIF->ether.sta.nm2);
		    sys_ip2 = sys_ip2 & sys_mask2 ;/*����ip��������*/
		    sys_ip3 = ntohl(profIF->ether.sta.ip3);
		    sys_mask3 = ntohl(profIF->ether.sta.nm3);
		    sys_ip3 = sys_ip3 & sys_mask3 ;/*����ip��������*/
		    sys_ip4 = ntohl(profIF->ether.sta.ip4);
		    sys_mask4 = ntohl(profIF->ether.sta.nm4);
		    sys_ip4 = sys_ip4 & sys_mask4 ;/*����ip��������*/
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
		    {/*��������� lan ip��ͬһ���Σ������ٰ���һ��ip��ַ*/
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
    {/*δ�����豸���ʿ���*/
	*ipFromInt = 0u;
	*ipEndInt = 0u;
	ret = 1;
    }
    return ret;
}
/*
 * ҳ�汣��
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
		goto out; //ҳ���ȡ����value�ǿյ�ʱ��
	}
	LanArpBroadcastInterval = atoi(GetValStr);
	if (LanArpBroadcastInterval < 100 || LanArpBroadcastInterval > 5000) {
		goto out; //ҳ���ȡ����value����׼��ȡֵ��Χ��ҳ���ж�һ�Σ������ж�һ�Σ�
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
 * ҳ�汣��
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
    aspOutAccessCtrl(wp, profSys);/*��ӡ�豸���ʿ������*/
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
