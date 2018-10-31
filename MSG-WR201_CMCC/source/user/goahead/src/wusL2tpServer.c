#include "uttMachine.h"
#if (L2TP_SERVER == FYES)
#include	<stdlib.h>
#include	<stdio.h>
#include	<sys/sysinfo.h>
#include        <fcntl.h>
#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include	"argcmd.h"
#include	"wuScript.h"
#include	"utt_pppoeClient.h"
#include	"wusL2tpServer.h"
#include		"translate.h"
//#define L2TP_DEBUG

#ifdef L2TP_DEBUG
#define L2TP_PRINT(fmt, args...)		printf(fmt, ## args)
#else
#define L2TP_PRINT(fmt, args...)
#endif

extern const LcdParamEnumValue pppAuthEnum[];
extern const LcdParamEnumValue vpnPeerTypeEnum[];
extern long long getIfStatistic(char *interface, int type);
extern struct flock* file_lock(short type, short whence);
#if (STATIC_NAT == FYES)
extern void init_remoteControl(char *remoteHttpEnable,char *httpExternPort,char *profile);
#endif
struct flock* file_lock(short type, short whence)
{
    static struct flock ret;
    ret.l_type = type ;
    ret.l_start = 0;
    ret.l_whence = whence;
    ret.l_len = 0;
    ret.l_pid = getpid();
    return &ret;
}
/*
 * 页面全局配置提交,处理函数
 */
static void formL2tpSrvGlobalConfig(webs_t wp, char_t *path, char_t *query) {
    char *enabled, *authType, *poolStart, *poolCnt, *localIp,*priDns, *secDns;
	const char *ErrMsg = NULL;
    MibProfileType mibType = MIB_PROF_L2TP_SRV_GLOBAL;
    L2TPSrvGlobalProfile *profL2tpS = NULL;
#if (STATIC_NAT == FYES)
    MibProfileType mibTypeNat = MIB_PROF_STATIC_NAT;
    StaticNatProfile *profNat = NULL;
    StaticNatProfile *profRemote = NULL;
    int instIndex;
#endif

    InterfaceProfile *profIf = NULL;
    Uint32 ifIp, wanCount;
#if (MULTI_LAN == FYES)
    Uint32 ifIp2, ifIp3, ifIp4;
#endif
    Uint32 lanIp = htonl(0xc0a80101);
    int i;

    struct in_addr addrStart, addrLocal, addrPriDns, addrSecDns;
    UINT32 cnt;

    ProfChangeType profAction;
    struProfAlloc *profhead = NULL;
    IpPoolEnum cfPoolType = IP_POOL_NONE;
    
    /*
     * 页面取值
     */
    enabled = websGetVar(wp, T("enable"), T("DISABLE"));
    authType = websGetVar(wp, T("authType"), NULL);
    poolStart = websGetVar(wp, T("poolStart"), NULL);
    poolCnt = websGetVar(wp, T("poolCount"), NULL);
    localIp = websGetVar(wp, T("localIp"), NULL);
	priDns = websGetVar(wp,T("priDns"), NULL);
	secDns = websGetVar(wp,T("secDns"), NULL);

    L2TP_PRINT("%s: enabled = %s, authType = %s, poolStart = %s, poolCnt = %s, localIp = %s\n",
	    __FUNCTION__, enabled, authType, poolStart, poolCnt, localIp);

    /*
     * 输入参数合法性
     */
    if ((authType == NULL) || (poolStart == NULL) || (poolCnt == NULL) || (localIp == NULL)) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_L2TP_SERVER_PARAERR);
			setTheErrorMsg((char *)ErrMsg);
//setTheErrorMsg(T("配置参数错误"));
    	goto out;
    }

    if (0 == inet_aton(poolStart, &addrStart)) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_ADDR_POOL_INIT_ADDR);
			setTheErrorMsg((char *)ErrMsg);

    //	setTheErrorMsg(T("无效地址池起始地址"));
    	goto out;
    }
    if (0 == inet_aton(localIp, &addrLocal)) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_SERVER_IP_ADDR);
			setTheErrorMsg((char *)ErrMsg);

    //	setTheErrorMsg(T("无效服务端IP地址"));
    	goto out;
    }
    if (0 == inet_aton(priDns, &addrPriDns)) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_MAIN_DNS_SERVER);
		setTheErrorMsg(ErrMsg);
		goto out;
    }
    if (0 == inet_aton(secDns, &addrSecDns)) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_SEC_DNS_SERVER);
		setTheErrorMsg(ErrMsg);
		goto out;
    }
    cnt = (UINT32)strtol(poolCnt, NULL, 10);
    if (cnt == 0) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_ADDR_POOL_ADDR_NUM);
		setTheErrorMsg((char *)ErrMsg);

    //	setTheErrorMsg(T("无效地址池地址数"));
    	goto out;
    }
    /*
     * 服务器地址不能包含在地址池内
     */
    if ((ntohl(addrLocal.s_addr) >= ntohl(addrStart.s_addr)) && 
	    (ntohl(addrLocal.s_addr) < ntohl(addrStart.s_addr) + cnt)) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_SERVER_ADDR_CANT_INC_IN_POOL);
		setTheErrorMsg((char *)ErrMsg);

//	setTheErrorMsg(T("服务器地址不能包含在地址池内"));
    	goto out;
    }
    /* 地址池不能包含接口ip */
    wanCount = getWanSlotCount();
    for (i = 0; i <= wanCount; i++) {
	profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, i);
	if ((profIf == NULL) || (ProfInstIsFree(profIf) == 1) || ((profIf->ether.connMode != ETH_CONN_STATIC) && (i != 0))) {
	    continue;
	}
	if (i == 0) {
	    lanIp = profIf->ether.sta.ip;
	}
	ifIp = ntohl(profIf->ether.sta.ip);
#if (MULTI_LAN == FYES)
	ifIp2 = ntohl(profIf->ether.sta.ip2);
	ifIp3 = ntohl(profIf->ether.sta.ip3);
	ifIp4 = ntohl(profIf->ether.sta.ip4);
#endif
	if ((ifIp >= ntohl(addrStart.s_addr) && ifIp < ntohl(addrStart.s_addr) + cnt)
#if (MULTI_LAN == FYES)
	    || (ifIp2 >= ntohl(addrStart.s_addr) && ifIp2 < ntohl(addrStart.s_addr) + cnt) 
	    || (ifIp3 >= ntohl(addrStart.s_addr) && ifIp3 < ntohl(addrStart.s_addr) + cnt) 
	    || (ifIp4 >= ntohl(addrStart.s_addr) && ifIp4 < ntohl(addrStart.s_addr) + cnt) 
#endif
	   ){
		ErrMsg = getTransDataForC(C_LANG_INDEX_ADDR_POOL_CANT_CONTAIN_INF_IP);
		setTheErrorMsg((char *)ErrMsg);

	   // setTheErrorMsg(T("地址池不能包含接口IP"));
	    goto out;
	}
    }

    /*
     * 判断地址池
     */
    cfPoolType = ipPoolConflict(IP_POOL_L2TP, addrStart.s_addr, htonl(ntohl(addrStart.s_addr) + cnt));
    if (cfPoolType != IP_POOL_NONE) {
	setPoolConflictMsg(cfPoolType);
	goto out;
    }

    /*
     * 静态映射判断
     */
#if (STATIC_NAT == FYES)
#if 0
    /* 出厂配置已建 */
    /* 建pptp实例要先初始化一条remote实例 */
    profRemote = (StaticNatProfile*)ProfGetInstPointByName(mibTypeNat, remCtrlName);
    if(profRemote == NULL){
//	init_remoteControl("0", "8081","WAN1");
	init_remoteControl("0", "80","WAN1");
    }
#endif

    instIndex = ProfGetInstIndexByName(mibTypeNat, L2TP_NAT_NAME);
    if (strncmp(enabled, "DISABLE", 8) == 0) {
	/* 存在则删除 */
	if (instIndex != PROFFAIL) {
	    profAction = PROF_CHANGE_DEL;
	    ProfBackupByIndex(mibTypeNat, profAction, instIndex, &profhead);
	    ProfDelInstByIndex(mibTypeNat, instIndex);
	}
    } else {
	/* 不存在则新建 */
	if (instIndex == PROFFAIL) {
	    instIndex = ProfNewInst(mibTypeNat, L2TP_NAT_NAME, FALSE);
	    if (instIndex == PROFFAIL) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_L2TP_SERVER_STATIC_ERR);
		setTheErrorMsg((char *)ErrMsg);

	//	setTheErrorMsg("静态映射已达最大条数，无法建立l2tp需要的静态映射");
		goto out;
	    }

	    profAction = PROF_CHANGE_ADD;
	    ProfBackupByIndex(mibTypeNat, profAction, instIndex, &profhead);
	    profNat = (StaticNatProfile *)ProfGetInstPointByIndex(mibTypeNat, instIndex);
	    ProfSetNameByPoint(profNat, L2TP_NAT_NAME);
	    profNat->head.active = 1;
	    profNat->protocol = STATIC_NAT_UDP;
	    profNat->OutPort = L2TP_PORT;
	    profNat->IP = lanIp;
	    profNat->InnerPort = L2TP_PORT;
	    profNat->PortNum = 1;
	    strcpy(profNat->NatBind, "WAN1");
	    L2TP_PRINT("%s: name=%s,active=%d,pro=%d,outport=%d,ip=0x%x,innerport=%d,portnum=%d,bind=%s\n", 
		    __FUNCTION__, profNat->head.name, profNat->head.active, profNat->protocol, profNat->OutPort, 
		    profNat->IP, profNat->InnerPort, profNat->PortNum, profNat->NatBind);
	}
    }
#endif

    profAction = PROF_CHANGE_EDIT;
    ProfBackupByIndex(mibType, profAction, 0, &profhead);
    /*
     * 获得profile准备保存配置
     */
    profL2tpS = (L2TPSrvGlobalProfile *)ProfGetInstPointByIndex(mibType, 0);

    if (strncmp(enabled, "DISABLE", 8) == 0) {
        profL2tpS->head.active = 0;
    } else {
	profL2tpS->head.active = 1;
    }

    profL2tpS->pppAuthMode = getEnumValue(pppAuthEnum, authType);

    profL2tpS->poolIpStart = addrStart.s_addr;
    profL2tpS->localIp = addrLocal.s_addr;
	profL2tpS->priDns = addrPriDns.s_addr;
	profL2tpS->secDns = addrSecDns.s_addr;
    profL2tpS->poolIpCnt = cnt;
    /* 获得lan口ip地址，作为监听端口地址 */
    profL2tpS->listenIp = lanIp;
   	
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
    websRedirect(wp,"L2TPSrvGlobal.asp");
}

/*
 * 页面全局配置输出
 */
static int aspOutputL2tpSrvGlobalData(int eid, webs_t wp, int argc, char_t **argv) {
    MibProfileType mibType = MIB_PROF_L2TP_SRV_GLOBAL;
    L2TPSrvGlobalProfile *profL2tpS = NULL;

    /*
     * 获得profile准备保存配置
     */
    profL2tpS = (L2TPSrvGlobalProfile *)ProfGetInstPointByIndex(mibType, 0);

    if (profL2tpS == NULL) {
	return -1;
    }

    /*
     * 页面输出
     */
    websWrite(wp, "var enables = %d;", profL2tpS->head.active);
    websWrite(wp, "var authTypes = \"%s\";", getEnumString(pppAuthEnum, profL2tpS->pppAuthMode));
    websWrite(wp, "var poolIpStarts = \"%N\";", profL2tpS->poolIpStart);
    websWrite(wp, "var poolIpCnts = %d;", profL2tpS->poolIpCnt);
    websWrite(wp, "var localIps = \"%N\";", profL2tpS->localIp);
    websWrite(wp, "var priDns = \"%N\";", profL2tpS->priDns); /*主DNS服务器*/
    websWrite(wp, "var secDns = \"%N\";", profL2tpS->secDns); /*备用DNS服务器*/
#if (VPN_ITEM_CONTROL == FYES)
    websWrite(wp, "var maxPoolCnt = %d;", MAX_VPN_ITEMS_NUM);
#else
    websWrite(wp, "var maxPoolCnt = %d;", MAX_L2TP_SRV_ACCOUNT_PROFILES);
#endif

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

/*
 * 页面L2TP服务器账号配置提交
 */
static void formL2tpSrvAccountConfig(webs_t wp, char_t *path, char_t *query) {
    MibProfileType mibType = MIB_PROF_L2TP_SRV_ACCOUNT;
    struProfAlloc *profhead = NULL;
    L2TPSrvAccProfile *profL2tpA = NULL;
    int instIndex;
    ProfChangeType profAction;
    struct in_addr addr_ip, addr_mask;

    char *instName, *peerType, *userName, *passwd, *remoteLanIp, *remoteLanNetmask, *action;
	const char *ErrMsg = NULL;

    instName = websGetVar(wp, T("instName"), T(""));
    peerType = websGetVar(wp, T("userType"), T(""));
    userName = websGetVar(wp, T("userName"), T(""));
    passwd = websGetVar(wp, T("passwd"), T(""));
    if (strcmp(peerType, "lantolan") == 0) {
	remoteLanIp = websGetVar(wp, T("remoteLanIp"), T(""));
	remoteLanNetmask = websGetVar(wp, T("remoteLanNetmask"), T(""));
    } else {
	remoteLanIp = T("0.0.0.0");
	remoteLanNetmask = T("0.0.0.0");
    }
    action = websGetVar(wp, T("Action"), T(""));

    L2TP_PRINT("%s: instName=%s,peerType=%s,userName=%s,passwd=%s,remoteLanIp=%s,remoteLanNetmask=%s,action=%s\n",
	    __FUNCTION__, instName, peerType, userName, passwd, remoteLanIp, remoteLanNetmask, action);

    if ((instName == NULL) || (peerType == NULL) || (userName == NULL) || (passwd == NULL) ||
	    (remoteLanIp == NULL) || (remoteLanNetmask == NULL) || (action == NULL)) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_PARAMETER_ERR);
		setTheErrorMsg((char *)ErrMsg);

//	setTheErrorMsg(T("参数错误！"));
	goto out;
    }

    if ((inet_aton(remoteLanIp, &addr_ip) == 0) || (inet_aton(remoteLanNetmask, &addr_mask) == 0)) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_IP_ADDR_ERR);
		setTheErrorMsg((char *)ErrMsg);

//	setTheErrorMsg(T("IP地址输入错误！"));
	goto out;
    }

    /* 远端内网IP网段不能与内网网段重叠 */
    if (strcmp(peerType, "lantolan") == 0) {
	if (isOverlapInnerIp(ntohl(addr_ip.s_addr), ntohl(addr_mask.s_addr)) == 1) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_REMOTE_ERR);
		setTheErrorMsg((char *)ErrMsg);

	   // setTheErrorMsg(T("远端内网网段不能与本地内网网段重叠"));
	    goto out;
	}
    }

    instIndex = ProfGetInstIndexByName(mibType, instName);

    if (strcmp(action, "add") == 0) {
        if(instIndex != PROFFAIL) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_TUNNEL_NAME_ERR);
		setTheErrorMsg((char *)ErrMsg);

    	   // setTheErrorMsg(T("隧道名称输入重复!"));
    	    goto out;
        }
#if (VPN_ITEM_CONTROL == FYES)
	if(checkVpnFreeItems() == 0) {
	    ErrMsg = getTransDataForC(C_LANG_INDEX_MAX_VPN_ITEM);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
#endif

        instIndex = ProfNewInst(mibType, instName, FALSE);

        if(instIndex == PROFFAIL) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_MAX_TOTAL_ERR);
		setTheErrorMsg((char *)ErrMsg);

    	//    setTheErrorMsg(T("已达最大条目！"));
    	    goto out;
        }
        profAction = PROF_CHANGE_ADD;
    } else {
	if(instIndex == PROFFAIL) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_CANT_TUNNEL_ERR);
		setTheErrorMsg((char *)ErrMsg);

    	   // setTheErrorMsg(T("找不到该隧道名称！"));
    	    goto out;
        }
        profAction = PROF_CHANGE_EDIT;
    }

    ProfBackupByIndex(mibType, profAction, instIndex, &profhead);

    profL2tpA = (L2TPSrvAccProfile *)ProfGetInstPointByIndex(mibType, instIndex);

    ProfSetNameByPoint(profL2tpA, instName);
    profL2tpA->peerType = getEnumValue(vpnPeerTypeEnum, peerType);
    strncpy(profL2tpA->userName, userName, 32); //1.7.4修改vpn帐号长度为32
    strncpy(profL2tpA->passwd, passwd, UTT_PASSWD_LEN);
    profL2tpA->remoteLanIp = addr_ip.s_addr & addr_mask.s_addr;
    profL2tpA->remoteLanIpDis = addr_ip.s_addr;
    profL2tpA->remoteLanNetmask = addr_mask.s_addr;

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
    websRedirect(wp, "L2TPList.asp");

    return;
}

/*
 * l2tp服务器页面列表删除
 */
static void formL2tpSrvListDel(webs_t wp, char_t *path, char_t *query) {
    char *instName = NULL, *tmp = NULL;
    struProfAlloc *profList  = NULL;
    int back = 0;
	const char *ErrMsg = NULL;
    MibProfileType mibType = MIB_PROF_L2TP_SRV_ACCOUNT;
#if (L2TP_CLIENT == FYES)
    MibProfileType mibTypeC = MIB_PROF_L2TP_CLIENT;
    char *instName_cli = NULL;
    instName_cli = websGetVar(wp, T("delstrCli"), T(""));
    L2TP_PRINT("%s: instName_cli = %s\n", __FUNCTION__, instName_cli);
#endif

    instName = websGetVar(wp, T("delstr"), T(""));

    L2TP_PRINT("%s: instName = %s\n", __FUNCTION__, instName);

    if ((instName == NULL)
#if (L2TP_CLIENT == FYES)
	    &&(instName_cli == NULL)
#endif
	    ) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_CANT_TUNNEL_ERR);
		setTheErrorMsg((char *)ErrMsg);

       // setTheErrorMsg(T("找不到该隧道名称!"));
        goto out;
    }

    tmp = strtok(instName, UTT_SPLIT_SIGN_STR);
    while(tmp != NULL) {
	if (back == 0) {
            ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profList);
            back = 1;
	}
	ProfDelInstByName(mibType, tmp); 
        tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
    }

#if (L2TP_CLIENT == FYES)
    tmp = NULL;
    tmp = strtok(instName_cli, UTT_SPLIT_SIGN_STR);
    while(tmp != NULL) {
	ProfBackupByName(mibTypeC, PROF_CHANGE_DEL, tmp, &profList);
	ProfDelInstByName(mibTypeC, tmp); 
        tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
    }
#endif
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
out:
    websRedirect(wp,"L2TPList.asp");
    return;
}

/*
 * l2tp服务器页面列表全部删除
 */
static void formL2tpSrvListDelAll(webs_t wp, char_t *path, char_t *query) {
    int min = 0, max = 0;
    int i = 0;
    int back = 0;
    MibProfileType mibType = MIB_PROF_L2TP_SRV_ACCOUNT;
    Pointer prof;
    struProfAlloc *profList  = NULL;
#if (L2TP_CLIENT == FYES)
    MibProfileType mibTypeC = MIB_PROF_L2TP_CLIENT;
#endif

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
#if (L2TP_CLIENT == FYES)
    ProfInstLowHigh(mibTypeC, &max, &min);

    for(i = min; i < max; i++) {
        prof = ProfGetInstPointByIndex(mibTypeC, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
	    ProfBackupByIndex(mibTypeC, PROF_CHANGE_DELALL, i, &profList);
            ProfDelInstByIndex(mibTypeC, i);
        }
    }
#endif
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
    websRedirect(wp, "L2TPList.asp");
    return;
}

/*
 * l2tp服务器页面列表信息输出
 */
static int aspOutputL2tpSrvListData(int eid, webs_t wp, int argc, char_t **argv) {
    MibProfileType mibType = MIB_PROF_L2TP_SRV_ACCOUNT;
    L2TPSrvAccProfile *profL2tpA = NULL;
    int min = 0, max = 0, i = 0, totalrecs = 0;
    int fd;
    struct st_uttPppdClient info;
    struct sysinfo sysInfo;
    int st_size = sizeof(struct st_uttPppdClient);
    off_t   start, end;
    int fileLen;
    int found;
    char buf[30];
    long long rx, tx;

    L2TPSrvGlobalProfile *profL2tpS = NULL;

    char_t *instNames = T("instNames[%d] = \"%s\";");
    char_t *peerTypes = T("peerTypes[%d] = \"%s\";");
    char_t *userNames = T("userNames[%d] = \"%s\";");
    char_t *remoteLanIps = T("remoteLanIps[%d] = \"%N\";");
    char_t *remoteNetmasks = T("remoteNetmasks[%d] = \"%N\";");
    char_t *statuss = T("statuss[%d] = \"%s\";");
    char_t *useTimes = T("useTimes[%d] = \"%d\";");
    char_t *inboundss = T("inboundss[%d] = \"%s\";");
    char_t *outboundss = T("outboundss[%d] = \"%s\";");

    websWrite(wp, "var instNames = new Array();");
    websWrite(wp, "var peerTypes = new Array();");
    websWrite(wp, "var userNames = new Array();");
    websWrite(wp, "var remoteLanIps = new Array();");
    websWrite(wp, "var remoteNetmasks = new Array();");
    websWrite(wp, "var statuss = new Array();");
    websWrite(wp, "var useTimes = new Array();");
    websWrite(wp, "var inboundss = new Array();");
    websWrite(wp, "var outboundss = new Array();");

    /* 读状态信息 */
    fd = open(UTT_L2TP_CLIENT_INFO_FILE, O_RDWR);
    sysinfo(&sysInfo);

    ProfInstLowHigh(mibType, &max, &min);

    for(i = min; i < max; i++) {
        profL2tpA = (L2TPSrvAccProfile *)ProfGetInstPointByIndex(mibType, i);
        if((profL2tpA != NULL) && (ProfInstIsFree(profL2tpA) == 0)) {
            websWrite(wp, instNames, totalrecs, profL2tpA->head.name);
            websWrite(wp, peerTypes, totalrecs, getEnumString(vpnPeerTypeEnum, profL2tpA->peerType));
            websWrite(wp, userNames, totalrecs, profL2tpA->userName);
#if 0
	    websWrite(wp, remoteLanIps, totalrecs, profL2tpA->remoteLanIp);
#endif
	    websWrite(wp, remoteLanIps, totalrecs, profL2tpA->remoteLanIpDis);
            websWrite(wp, remoteNetmasks, totalrecs, profL2tpA->remoteLanNetmask);
#if 0
	    websWrite(wp, statuss, totalrecs, T("-"));
	    websWrite(wp, useTimes, totalrecs, T("-"));
	    websWrite(wp, inboundss, totalrecs, T("-"));
	    websWrite(wp, outboundss, totalrecs, T("-"));
#endif
	    if (fd != -1) {
		start = lseek(fd, 0, SEEK_SET);
		end = lseek(fd, 0, SEEK_END);
		fileLen = end - start;
		lseek(fd, 0, SEEK_SET);
		found = 0;
		while (fileLen >= st_size) {
		    fcntl(fd, F_SETLKW, file_lock(F_WRLCK|F_RDLCK, SEEK_SET));
		    read(fd, &info, st_size);
		    fcntl(fd, F_SETLKW, file_lock(F_UNLCK, SEEK_SET));

		    if ((info.ConTime == 0) && (strcmp(info.ip, "") != 0) && (strcmp(info.user, profL2tpA->userName) == 0)) {
			found = 1;
			websWrite(wp, statuss, totalrecs, T("1"));
			websWrite(wp, useTimes, totalrecs, sysInfo.uptime - info.STime);
			tx = getIfStatistic(info.ifname, TXBYTE);
			rx = getIfStatistic(info.ifname, RXBYTE);
			sprintf(buf, "%lld", tx);
			websWrite(wp, outboundss, totalrecs, buf);
			sprintf(buf, "%lld", rx);
			websWrite(wp, inboundss, totalrecs, buf);
			break;
		    }
		    fileLen -= st_size;
		}
		if (found == 0) {
		    websWrite(wp, statuss, totalrecs, T("0"));
		    websWrite(wp, T("useTimes[%d] = \"%s\";"), totalrecs, T("-"));
		    websWrite(wp, inboundss, totalrecs, T("-"));
		    websWrite(wp, outboundss, totalrecs, T("-"));
		}
	    } else {
		websWrite(wp, statuss, totalrecs, T("0"));
		websWrite(wp, T("useTimes[%d] = \"%s\";"), totalrecs, T("-"));
		websWrite(wp, inboundss, totalrecs, T("-"));
		websWrite(wp, outboundss, totalrecs, T("-"));
	    }

            totalrecs++;
        }
    }
    if (fd != -1) {
	close(fd);
    }

    profL2tpS = (L2TPSrvGlobalProfile *)ProfGetInstPointByIndex(MIB_PROF_L2TP_SRV_GLOBAL, 0);
    if (profL2tpS == NULL) {
	websWrite(wp,"var srv_enables = 0;");
    } else {
	websWrite(wp,"var srv_enables = %d;", profL2tpS->head.active);
    }

    websWrite(wp,"var totalrecs = %d;", totalrecs);
#if (VPN_ITEM_CONTROL == FYES)
    websWrite(wp,"var max_totalrecs = 0;");
#else
    websWrite(wp,"var max_totalrecs = %d;", max);
#endif
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

/*
 * l2tp服务器账号配置页面信息输出
 * 根据读入隧道名称，输出相关隧道名称信息
 */
static int aspOutputL2tpSrvAccountData(int eid, webs_t wp, int argc, char_t **argv) {
    MibProfileType mibType = MIB_PROF_L2TP_SRV_ACCOUNT;
    L2TPSrvAccProfile *profL2tpA = NULL;
    int min = 0, max = 0, i = 0;

    char *editName;

    websWrite(wp, "var instNames = \"\";");
    websWrite(wp, "var peerTypes = \"\";");
    websWrite(wp, "var userNames = \"\";");
    websWrite(wp, "var remoteLanIps = \"\";");
    websWrite(wp, "var remoteNetmasks = \"\";");
    websWrite(wp, "var passwds = \"\";");

    if (ejArgs(argc, argv, T("%s"), &editName) < 1) {
	setTheErrorMsg(T("读取数据失败！"));
    } else {
	ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++) {
	    profL2tpA = (L2TPSrvAccProfile *)ProfGetInstPointByIndex(mibType, i);
	    if((profL2tpA != NULL) && (ProfInstIsFree(profL2tpA) == 0) &&
		    (editName != NULL) && (strcmp(editName, profL2tpA->head.name) == 0)) {
		websWrite(wp, "instNames = \"%s\";", profL2tpA->head.name);
		websWrite(wp, "peerTypes = \"%s\";", getEnumString(vpnPeerTypeEnum, profL2tpA->peerType));
		websWrite(wp, "userNames = \"%s\";", profL2tpA->userName);
		websWrite(wp, "passwds = \"%s\";", profL2tpA->passwd);
#if 0
		websWrite(wp, "remoteLanIps = \"%N\";", profL2tpA->remoteLanIp);
#endif
		websWrite(wp, "remoteLanIps = \"%N\";", profL2tpA->remoteLanIpDis);
		websWrite(wp, "remoteNetmasks = \"%N\";", profL2tpA->remoteLanNetmask);
		break;
	    }
	}
    }
    
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

/*
 * 定义页面处理句柄
 */
extern void formDefineL2tpServer(void) {

    websAspDefine(T("OutputL2tpSrvGlobalData"), aspOutputL2tpSrvGlobalData);
    websFormDefine(T("l2tpSrvGlobalConfig"), formL2tpSrvGlobalConfig);
    websAspDefine(T("OutputL2tpSrvListData"), aspOutputL2tpSrvListData);
    websFormDefine(T("l2tpSrvAccountConfig"), formL2tpSrvAccountConfig);
    websFormDefine(T("l2tpSrvListDel"), formL2tpSrvListDel);
    websFormDefine(T("l2tpSrvListDelAll"), formL2tpSrvListDelAll);
    websAspDefine(T("OutputL2tpSrvAccountData"), aspOutputL2tpSrvAccountData);

    return;
}
#endif
