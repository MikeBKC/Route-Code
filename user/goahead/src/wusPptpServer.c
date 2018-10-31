#include "uttMachine.h"
#if (PPTP_SERVER == FYES)
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
#include	"wusPptpServer.h"
#include	"translate.h"


//#define PPTP_DEBUG

#ifdef PPTP_DEBUG
#define PPTP_PRINT(fmt, args...)		printf(fmt, ## args)
#else
#define PPTP_PRINT(fmt, args...)
#endif

extern const LcdParamEnumValue pppAuthEnum[];
extern const LcdParamEnumValue vpnPeerTypeEnum[];
extern long long getIfStatistic(char *interface, int type);
#if 0
#if (STATIC_NAT == FYES)
extern void init_remoteControl(char *remoteHttpEnable,char *httpExternPort,char *profile);
#endif
#endif

#if (PPTP_IP_BIND == FYES)
static int ipAllowUserBind(char *user, char *ip);
static int existNonPoolBindIp(Uint32 ipS, Uint32 ipE);
#endif
#if (L2TP_SERVER == FYES)
extern struct flock* file_lock(short type, short whence);
#else
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
#endif
/*
 * 页面全局配置提交,处理函数
 */
static void formPptpSrvGlobalConfig(webs_t wp, char_t *path, char_t *query) {
    char *enabled, *authType, *poolStart, *poolCnt, *localIp, *priDns, *secDns;

	const char *ErrMsg = NULL;
    MibProfileType mibType = MIB_PROF_PPTP_SRV_GLOBAL;
    PPTPSrvGlobalProfile *profPptpS = NULL;
#if (STATIC_NAT == FYES)
    MibProfileType mibTypeNat = MIB_PROF_STATIC_NAT;
    StaticNatProfile *profNat = NULL;
#if 0
    StaticNatProfile *profRemote = NULL;
#endif
    int instIndex;
#endif
#if (PPTP_ENCRYPT_MODE == FYES)
    char *EncryptionMode = NULL;
#endif

    InterfaceProfile *profIf = NULL;
    Uint32 ifIp, wanCount;
    Uint32 mtu;
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
    mtu = websGetVar(wp,T("MTU"), NULL);
#if (PPTP_ENCRYPT_MODE == FYES)
    EncryptionMode = websGetVar(wp, T("EncryptionMode"), NULL);
#endif

#if (PPTP_ENCRYPT_MODE == FYES)
    PPTP_PRINT("%s: enabled = %s, authType = %s, poolStart = %s, poolCnt = %s, localIp = %s, EncryptionMode = %s\n",
	    __FUNCTION__, enabled, authType, poolStart, poolCnt, localIp, EncryptionMode);
#else
    PPTP_PRINT("%s: enabled = %s, authType = %s, poolStart = %s, poolCnt = %s, localIp = %s\n",
	    __FUNCTION__, enabled, authType, poolStart, poolCnt, localIp);
#endif
    /*
     * 输入参数合法性
     */
#if (PPTP_ENCRYPT_MODE == FYES)
    if ((authType == NULL) || (poolStart == NULL) || (poolCnt == NULL) || (localIp == NULL) || (EncryptionMode == NULL)) {
#else
    if ((authType == NULL) || (poolStart == NULL) || (poolCnt == NULL) || (localIp == NULL)) {
#endif
		ErrMsg = getTransDataForC(C_LANG_INDEX_CINFIG_PARAMETER_WRONG);
		setTheErrorMsg(ErrMsg);
		goto out;
    }

    if (0 == inet_aton(poolStart, &addrStart)) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_ADDR_POOL_INIT_ADDR);
		setTheErrorMsg(ErrMsg);
		goto out;
    }
    if (0 == inet_aton(localIp, &addrLocal)) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_SERVER_IP_ADDR);
		setTheErrorMsg(ErrMsg);
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
		setTheErrorMsg(ErrMsg);
		goto out;
    }

#if (PPTP_IP_BIND == FYES)
    /*
     * 看是否存在不在地址池的固定ip
     */
    if (existNonPoolBindIp(ntohl(addrStart.s_addr), ntohl(addrStart.s_addr) + cnt) == 1) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_EXIT_FIX_IP_IN_POOL);
	setTheErrorMsg(ErrMsg);
	goto out;
    }
#endif
    /*
     * 服务器地址不能包含在地址池内
     */
    if ((ntohl(addrLocal.s_addr) >= ntohl(addrStart.s_addr)) && 
	    (ntohl(addrLocal.s_addr) < ntohl(addrStart.s_addr) + cnt)) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_SERVER_ADDR_CANT_INC_IN_POOL);
		setTheErrorMsg(ErrMsg);
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
	    )
	{
		ErrMsg = getTransDataForC(C_LANG_INDEX_ADDR_POOL_CANT_CONTAIN_INF_IP);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
    }

    /*
     * 判断地址池
     */
    cfPoolType = ipPoolConflict(IP_POOL_PPTP, addrStart.s_addr, htonl(ntohl(addrStart.s_addr) + cnt));
    if (cfPoolType != IP_POOL_NONE) {
	setPoolConflictMsg(cfPoolType);
	goto out;
    }

    /*
     * 静态映射判断
     */
#if (STATIC_NAT == FYES)
#if 0
    /*已在出厂配置时初始化*/
    /* 初始化远程管理静态映射 */
    profRemote = (StaticNatProfile*)ProfGetInstPointByName(mibTypeNat, remCtrlName);
    if(profRemote == NULL){
	init_remoteControl("0", "8081","WAN1");
    }
#endif

    instIndex = ProfGetInstIndexByName(mibTypeNat, PPTP_NAT_NAME);
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
	    instIndex = ProfNewInst(mibTypeNat, PPTP_NAT_NAME, FALSE);
	    if (instIndex == PROFFAIL) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_STATIC_ERR);
			setTheErrorMsg(ErrMsg);
		goto out;
	    }

	    profAction = PROF_CHANGE_ADD;
	    ProfBackupByIndex(mibTypeNat, profAction, instIndex, &profhead);
	    profNat = (StaticNatProfile *)ProfGetInstPointByIndex(mibTypeNat, instIndex);
	    ProfSetNameByPoint(profNat, PPTP_NAT_NAME);
	    profNat->head.active = 1;
	    profNat->protocol = STATIC_NAT_TCP;
	    profNat->OutPort = PPTP_PORT;
	    profNat->IP = lanIp;
	    profNat->InnerPort = PPTP_PORT;
	    profNat->PortNum = 1;
	    strcpy(profNat->NatBind, "WAN1");
	    PPTP_PRINT("%s: name=%s,active=%d,pro=%d,outport=%d,ip=0x%x,innerport=%d,portnum=%d,bind=%s\n", 
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
    profPptpS = (PPTPSrvGlobalProfile *)ProfGetInstPointByIndex(mibType, 0);

    if (strncmp(enabled, "DISABLE", 8) == 0) {
        profPptpS->head.active = 0;
    } else {
	profPptpS->head.active = 1;
    }

    profPptpS->pppAuthMode = getEnumValue(pppAuthEnum, authType);

    profPptpS->poolIpStart = addrStart.s_addr;
    profPptpS->localIp = addrLocal.s_addr;
	profPptpS->priDns = addrPriDns.s_addr;
	profPptpS->secDns = addrSecDns.s_addr;

#if (PPTP_ENCRYPT_MODE == FYES)
    strcpy(profPptpS->EncryptionMode, EncryptionMode);
#endif
    profPptpS->poolIpCnt = cnt;
    /* 获得lan口ip地址，作为监听端口地址 */
    profPptpS->listenIp = lanIp;
    profPptpS->mtu = strtol(mtu, NULL, 10);
   	
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
    websRedirect(wp,"PPTPSrvGlobal.asp");
}

#if (PPTP_IP_BIND == FYES)
/********************************************************************
 * 函数： ipAllowUserBind
 * 功能： 判断是否允许账号，IP绑定
 * 创建： 2012-10-11
 * 参数： 
 *  user    -	页面提交的账号
 *  ip	    -	页面提交的绑定IP地址
 * 返回： 
 *  1	    -   允许
 *  0	    -   不允许
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int ipAllowUserBind(char *user, char *ip)
{
    int ret = 1, fd = -1, fileLen = 0;
    off_t   start, end;
    struct st_uttPppdClient info;
    int st_size = sizeof(struct st_uttPppdClient);

    memset(&info, 0, st_size);
    fd = open(UTT_PPTP_CLIENT_INFO_FILE, O_RDWR);
    PPTP_PRINT("%s - %d: fd = %d, file=%s\n", __FUNCTION__, __LINE__, fd, UTT_PPTP_CLIENT_INFO_FILE);
    if (fd != -1) 
    {/*文件打开成功*/
	start = lseek(fd, 0, SEEK_SET);/*文件开头*/
	end = lseek(fd, 0, SEEK_END);/*文件结尾*/
	fileLen = end - start;/*文件长度*/
	lseek(fd, 0, SEEK_SET);
	while (fileLen >= st_size) 
	{/*一直查找直到文件长度小于结构体长度*/
	    fcntl(fd, F_SETLKW, file_lock(F_WRLCK|F_RDLCK, SEEK_SET));
	    read(fd, &info, st_size);/*将内容读到结构体中*/
	    fcntl(fd, F_SETLKW, file_lock(F_UNLCK, SEEK_SET));

	    PPTP_PRINT("%s - %d:conTime = %d, ip=%s, user=%s\n", 
		    __FUNCTION__, __LINE__, info.ConTime, info.ip, info.user);
	    if ((info.ConTime == 0U) && (strcmp(info.ip, "") != 0) && (strcmp(info.ip, ip) == 0)) 
	    {/*此ip正在使用*/
		if(strcmp(user, info.user) != 0)
		{/*使用此ip的不是要绑定的用户*/
		    ret = 0;
		}
	    }
	    fileLen -= st_size;/*每次比较后总长度减去结构体长度*/
	}
	close(fd);/*关闭文件*/
    }
    return ret;
}
/********************************************************************
 * 函数： existNonPoolBindIp
 * 功能： 判断是否存在不在地址池内的固定IP分配地址
 * 创建： 2012-10-08
 * 参数： ipS    -    地址池起始地址
 *	  ipE    -    地址池结束地址
 * 返回：  1     -    存在
 *         0     -    不存在
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static int existNonPoolBindIp(Uint32 ipS, Uint32 ipE) 
{
    MibProfileType mibTypeA = MIB_PROF_PPTP_SRV_ACCOUNT;
    PPTPSrvAccProfile *profA = NULL;
    int flag = 0;
    int i = 0, min = 0, max = 0;
    Uint32 ip = 0U;

    ProfInstLowHigh(mibTypeA, &max, &min);
    for (i = min; i < max; i++) 
    {
	profA = (PPTPSrvAccProfile *)ProfGetInstPointByIndex(mibTypeA, i);
	if((profA != NULL) && (ProfInstIsFree(profA) == 0) && (profA->bindIp != 0U)) 
	{
	    ip = ntohl(profA->bindIp);/*调整字节序*/
	    if ((ip < ipS) || (ip >= ipE)) 
	    {/*固定ip地址不在地址池内*/
		flag = 1;
		break;
	    }
	}
    }
    return flag;
}
#endif

/*
 * 功能：页面全局配置输出
 *	参数：页面交互
 *	修改时间：20130423
 */
static int aspOutputPptpSrvGlobalData(int eid, webs_t wp, int argc, char_t **argv) {
    MibProfileType mibType = MIB_PROF_PPTP_SRV_GLOBAL;
    PPTPSrvGlobalProfile *profPptpS = NULL;
	int ret=0;
    /*
     * 获得profile准备保存配置
     */
    profPptpS = (PPTPSrvGlobalProfile *)ProfGetInstPointByIndex(mibType, 0);

    if (profPptpS == NULL) {
	return -1;
    }else{

    /*
     * 页面输出
     */
    websWrite(wp, "var enables = %d;", profPptpS->head.active); /*启用开关*/
    websWrite(wp, "var authTypes = \"%s\";", getEnumString(pppAuthEnum, profPptpS->pppAuthMode));
    websWrite(wp, "var poolIpStarts = \"%N\";", profPptpS->poolIpStart); /*地址池起始地址*/
    websWrite(wp, "var poolIpCnts = %d;", profPptpS->poolIpCnt);
    websWrite(wp, "var localIps = \"%N\";", profPptpS->localIp);
    websWrite(wp, "var priDns = \"%N\";", profPptpS->priDns); /*主DNS服务器*/
    websWrite(wp, "var secDns = \"%N\";", profPptpS->secDns); /*备用DNS服务器*/
#if (PPTP_ENCRYPT_MODE == FYES)
    websWrite(wp, "var EncryptionModes = \"%s\";", profPptpS->EncryptionMode);
#endif
#if (VPN_ITEM_CONTROL == FYES)
    websWrite(wp, "var maxPoolCnt = %d;", MAX_VPN_ITEMS_NUM); /*最大会话数*/
#else
    websWrite(wp, "var maxPoolCnt = %d;", MAX_PPTP_SRV_ACCOUNT_PROFILES); /*最大会话数*/
#endif
    websWrite(wp, "var mtu = %u;", profPptpS->mtu);

    getTheErrorMsg(eid, wp, argc, argv);
	}
    return ret;
}

/*
 * 页面PPTP服务器账号配置提交
 */
static void formPptpSrvAccountConfig(webs_t wp, char_t *path, char_t *query) {
    MibProfileType mibType = MIB_PROF_PPTP_SRV_ACCOUNT;
    struProfAlloc *profhead = NULL;
    PPTPSrvAccProfile *profPptpA = NULL;
    int instIndex;
    ProfChangeType profAction;
    struct in_addr addr_ip, addr_mask;
	const char *ErrMsg = NULL;

    char *instName, *peerType, *userName, *passwd, *remoteLanIp, *remoteLanNetmask, *action;
#if (PPTP_IP_BIND == FYES)
    char *bindIp = NULL;
    struct in_addr bindip;
    Uint32 h_bindIp = 0, h_ipStart = 0;
    MibProfileType mibTypeGlobal = MIB_PROF_PPTP_SRV_GLOBAL;
    PPTPSrvGlobalProfile *profS = NULL;
#endif
#if(HARD_FEATURE == FYES)
    char *lanMacStr=NULL;
#endif
    instName = websGetVar(wp, T("instName"), T(""));
    peerType = websGetVar(wp, T("userType"), T(""));
    userName = websGetVar(wp, T("userName"), T(""));
    passwd = websGetVar(wp, T("passwd"), T(""));
#if (PPTP_IP_BIND == FYES)
    bindIp = websGetVar(wp, T("bindIp"), T(""));
#endif
    if (strcmp(peerType, "lantolan") == 0) {
	remoteLanIp = websGetVar(wp, T("remoteLanIp"), T(""));
	remoteLanNetmask = websGetVar(wp, T("remoteLanNetmask"), T(""));
    } else {
#if(HARD_FEATURE == FYES)
	lanMacStr = websGetVar(wp, T("LanMac"), T(""));
	PPTP_PRINT("%s-%d: lanMacStr =%s\n",__func__,__LINE__,lanMacStr);
#endif
	remoteLanIp = T("0.0.0.0");
	remoteLanNetmask = T("0.0.0.0");
    }
    action = websGetVar(wp, T("Action"), T(""));

#if (PPTP_IP_BIND == FYES)
    PPTP_PRINT("%s: instName=%s,peerType=%s,userName=%s,passwd=%s,remoteLanIp=%s,remoteLanNetmask=%s,action=%s, bindIp = %s\n",
	    __FUNCTION__, instName, peerType, userName, passwd, remoteLanIp, remoteLanNetmask, action, bindIp);
#else
    PPTP_PRINT("%s: instName=%s,peerType=%s,userName=%s,passwd=%s,remoteLanIp=%s,remoteLanNetmask=%s,action=%s\n",
	    __FUNCTION__, instName, peerType, userName, passwd, remoteLanIp, remoteLanNetmask, action);
#endif

    if ((instName == NULL) || (peerType == NULL) || (userName == NULL) || (passwd == NULL) ||
	    (remoteLanIp == NULL) || (remoteLanNetmask == NULL) || (action == NULL)) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_PARAMETER_ERR);
		setTheErrorMsg(ErrMsg);
		goto out;
    }

#if (PPTP_IP_BIND == FYES)
    /*
     * 固定IP地址
     */
    if (strlen(bindIp) == 0) {
	bindip.s_addr = 0;
    } else {
	if (0 == inet_aton(bindIp, &bindip)) {
	    ErrMsg = getTransDataForC(C_LANG_INDEX_STATIC_IP_ADDR_CIN_ERR);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
    }
    profS = (PPTPSrvGlobalProfile*)ProfGetInstPointByIndex(mibTypeGlobal, 0);
    if ((profS == NULL) || (ProfInstIsFree(profS) == 1)) {
	PPTP_PRINT("%s: get profile error\n", __FUNCTION__);
	ErrMsg = getTransDataForC(C_LANG_INDEX_GET_ARGS_ERR);
	setTheErrorMsg(ErrMsg);
	goto out;
    }
    /*判断固定ip是否在地址池内*/
    h_bindIp = ntohl(bindip.s_addr);
    h_ipStart = ntohl(profS->poolIpStart);
    if ((h_bindIp != 0) && (h_bindIp < h_ipStart || h_bindIp >= (h_ipStart + profS->poolIpCnt))) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_EXIT_FIX_IP_IN_POOL);
	setTheErrorMsg(ErrMsg);
	goto out;
    }
    if(ipAllowUserBind(userName, bindIp) == 0)
    {/*此ip已被其它用户使用*/
	ErrMsg = getTransDataForC(C_LANG_INDEX_BANNED_BOUND_ALREADY_ASSIGN_IP);
	setTheErrorMsg(ErrMsg);
	goto out;
    }
#endif

    if ((inet_aton(remoteLanIp, &addr_ip) == 0) || (inet_aton(remoteLanNetmask, &addr_mask) == 0)) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_IP_ADDR_ERR);
	setTheErrorMsg(ErrMsg);
	goto out;
    }

    /* 远端内网IP网段不能与内网网段重叠 */
    if (strcmp(peerType, "lantolan") == 0) {
	if (isOverlapInnerIp(ntohl(addr_ip.s_addr), ntohl(addr_mask.s_addr)) == 1) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_REMOTE_ERR);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
    }

    instIndex = ProfGetInstIndexByName(mibType, instName);

    if (strcmp(action, "add") == 0) {
#if (VPN_ITEM_CONTROL == FYES)
	if(checkVpnFreeItems() == 0) {
	    ErrMsg = getTransDataForC(C_LANG_INDEX_MAX_VPN_ITEM);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
#endif
	/*判断用户名是否重复*/
	if(ProfGetInstPointByValue(mibType, OFFANDSIZE(PPTPSrvAccProfile, userName), TSTR, userName, -1) != PROFFAIL)
	{
	    ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_ALREADY_EXITS);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
#if (PPTP_IP_BIND == FYES)
	/*
	 * 判断固定IP地址是否重复
	 */
	if ((bindip.s_addr != 0) && (ProfGetInstPointByValue(mibType, OFFANDSIZE(PPTPSrvAccProfile, bindIp), TIPADDR, bindIp, -1) != PROFFAIL)) {
	    ErrMsg = getTransDataForC(C_LANG_INDEX_STATIC_IP_ADDR_ALREADY_EXITS);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
#endif
        if(instIndex != PROFFAIL) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_TUNNEL_NAME_ERR);
			setTheErrorMsg(ErrMsg);
		    goto out;
        }

        instIndex = ProfNewInst(mibType, instName, FALSE);

        if(instIndex == PROFFAIL) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_MAX_TOTAL_ERR);
		    setTheErrorMsg(ErrMsg);
		    goto out;
        }
        profAction = PROF_CHANGE_ADD;
    } else {
	/*判断用户名是否重复*/
	if(ProfGetInstPointByValue(mibType, OFFANDSIZE(PPTPSrvAccProfile, userName), TSTR, userName, instIndex) != PROFFAIL)
	{
	    ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_ALREADY_EXITS);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	if(instIndex == PROFFAIL) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_CANT_TUNNEL_ERR);
    	    setTheErrorMsg(ErrMsg);
    	    goto out;
        }
#if (PPTP_IP_BIND == FYES)
	/*
	 * 判断固定IP地址是否重复
	 */
	if ((bindip.s_addr != 0) && (ProfGetInstPointByValue(mibType, OFFANDSIZE(PPTPSrvAccProfile, bindIp), TIPADDR, bindIp, instIndex) != PROFFAIL)) {
	    ErrMsg = getTransDataForC(C_LANG_INDEX_STATIC_IP_ADDR_ALREADY_EXITS);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
#endif
        profAction = PROF_CHANGE_EDIT;
    }

    ProfBackupByIndex(mibType, profAction, instIndex, &profhead);

    profPptpA = (PPTPSrvAccProfile *)ProfGetInstPointByIndex(mibType, instIndex);

    ProfSetNameByPoint(profPptpA, instName);
    profPptpA->peerType = getEnumValue(vpnPeerTypeEnum, peerType);
    strncpy(profPptpA->userName, userName, 32); //1.7.4 修改vpn帐号长度为32
    strncpy(profPptpA->passwd, passwd, UTT_PASSWD_LEN);
#if (PPTP_IP_BIND == FYES)
    profPptpA->bindIp = bindip.s_addr;
#endif
#if(HARD_FEATURE == FYES)
    if ((strcmp(peerType, "lantolan") != 0)&&(lanMacStr!=NULL)) {
    if(strlen(lanMacStr) < 12) {
	memset(&(profPptpA->hardF), 0U, sizeof(profPptpA->hardF));
    } else {
	converMac12To6Byte(lanMacStr, &(profPptpA->hardF));
    }
    }
#endif
    profPptpA->remoteLanIp = addr_ip.s_addr & addr_mask.s_addr;
    /* mantis 8180 */
    profPptpA->remoteLanIpDis = addr_ip.s_addr;
    profPptpA->remoteLanNetmask = addr_mask.s_addr;

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
    websRedirect(wp, "PPTPClientList.asp");

    return;
}

#if (STATIC_ROUTE_BIND_PPTP == FYES)
/********************************************************************
 * 函数： isUserInStaticRoute
 * 功能： 检查此PPTP账号是否被静态路由引用
 * 创建： 2012-10-23
 * 参数： 账号名
 * 返回： 0         -         此账号未使用
 *	  1	    -         此账号正在使用
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
extern int isUserInStaticRoute(char *name)
{
    MibProfileType mibType = MIB_PROF_STATIC_ROUTE;
    StaticRouteProfile *profRoute = NULL;
    int min = 0, max = 0, i = 0;
    int ret = 0;

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) 
    {/*循环所有profile*/
	profRoute = (StaticRouteProfile *)ProfGetInstPointByIndex(mibType, i);
	if ((profRoute != NULL) && (ProfInstIsFree(profRoute) == 0)) 
	{
	    if(strcmp(name, profRoute->interfaceName) == 0)
	    {/*此账号被引用*/
		ret = 1;
	    }
	}
    }
    PPTP_PRINT("%s-%d, ret = %d\n", __func__, __LINE__, ret);
    return ret;
}
#endif
#if(IP_SEC == FYES)
/********************************************************************
 * 函数： isUserInIPSecProf
 * 功能： 检查此PPTP账号是否被IPSec引用
 * 参数： 账号名
 * 返回： 0         -         此账号未使用
 *	  1	    -         此账号正在使用
 ********************************************************************/ 
extern int isUserInIPSecProf(char *name)
{
    MibProfileType mibType = MIB_PROF_IPSEC;
    ipsecConfProfile *ipsecProf = NULL;

    int min = 0, max = 0, i = 0;
    int ret = 0;

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) 
    {/*循环所有profile*/
	ipsecProf = (ipsecConfProfile *)ProfGetInstPointByIndex(mibType, i);
	if ((ipsecProf != NULL) && (ProfInstIsFree(ipsecProf) == 0)) 
	{
	    if(strcmp(name, ipsecProf->profile) == 0)
	    {/*此账号被引用*/
		ret = 1;
	    }
	}
    }
    PPTP_PRINT("%s-%d, ret = %d\n", __func__, __LINE__, ret);
    return ret;
}
#endif
/*
 * pptp页面列表删除
 */
static void formPptpListDel(webs_t wp, char_t *path, char_t *query) {
    char *instName = NULL, *tmp = NULL;
    char *srvInst = NULL;
    struProfAlloc *profList  = NULL;
    int back = 0;
    MibProfileType mibType = MIB_PROF_PPTP_SRV_ACCOUNT;
#if (PPTP_CLIENT == FYES)
    MibProfileType mibTypeC = MIB_PROF_PPTP_CLI;
#if 0
    int back1 = 0;
#endif
#endif
#if (STATIC_ROUTE_BIND_PPTP == FYES)
    //char UserName[UTT_INST_NAME_LEN + 1 + 12];/*(PPTPS)用户名*/
    char UserName[33];//1.7.4 修改vpn帐号长度为32
    char referedMsg[100];
    const char *ErrMsg1 = NULL;

    memset(referedMsg, 0, sizeof(referedMsg));
#endif
#if(IP_SEC == FYES)
    char profName[UTT_INST_NAME_LEN + 1 + 12];/*PS_*/
    char *errMsg_ipsec[100];
    const char *ErrMsg2 = NULL;
    memset(errMsg_ipsec, 0, sizeof(errMsg_ipsec));
#endif
	const char *ErrMsg = NULL;

    instName = websGetVar(wp, T("delstr"), T(""));
    /* server删除实例 */
    srvInst = websGetVar(wp, T("delstrsrv"), T(""));

    PPTP_PRINT("%s: instName = %s, srvInst = %s\n", __FUNCTION__, instName, srvInst);

    if ((instName == NULL) || (srvInst == NULL)) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_CANT_TUNNEL_ERR);
        setTheErrorMsg(ErrMsg);
        goto out;
    }

    if (strlen(srvInst) != 0) { /* 有需要删除的server实例 */
	tmp = strtok(srvInst, UTT_SPLIT_SIGN_STR);
	while(tmp != NULL) {
#if (STATIC_ROUTE_BIND_PPTP == FYES)
	    snprintf(UserName, sizeof(UserName), "(PPTPS)%s", tmp);
	    PPTP_PRINT("%s-%d, UserName = %s\n", __func__, __LINE__, UserName);
	    if(isUserInStaticRoute(UserName) == 1)
	    {/*查看此用户名是否被静态路由引用*/
		ErrMsg1 = getTransDataForC(C_LANG_INDEX_USER);
		ErrMsg = getTransDataForC(C_LANG_INDEX_USER_IS_USE_IN_STATIC_ROUTE);
		snprintf(referedMsg, sizeof(referedMsg), "%s%s%s", ErrMsg1, tmp, ErrMsg);
		setTheErrorMsg((const char *)referedMsg);
		ProfUpdate(profList);
		ProfFreeAllocList(profList);
		nvramWriteCommit();
		goto out;
	    }
	    back=1;
	    /*由于前面可能直接goto到out了而没有发消息给speedweb进行处理，故只备份一次会造成在不能删除账号后面的账号不能被删除了,故这里每次都要备份*/
	    ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profList);
#endif
#if(IP_SEC == FYES)
	    memset(profName, 0, sizeof(profName));
	    snprintf(profName, sizeof(profName), "PS_%s", tmp);
	    PPTP_PRINT("%s-%d, profName = %s\n", __func__, __LINE__, profName);
	    if(isUserInIPSecProf(profName) == 1)
	    {/*查看此用户名是否被IPSec引用*/
		ErrMsg2 = getTransDataForC(C_LANG_INDEX_USER_IS_USE_IN_IPSEC);
		snprintf(errMsg_ipsec, sizeof(errMsg_ipsec), "%s%s", tmp, ErrMsg2);
		setTheErrorMsg((const char *)errMsg_ipsec);
		ProfUpdate(profList);
		ProfFreeAllocList(profList);
		nvramWriteCommit();
		goto out;
	    }
	    back=1;
	    /*由于前面可能直接goto到out了而没有发消息给speedweb进行处理，故只备份一次会造成在不能删除账号后面的账号不能被删除了,故这里每次都要备份*/
	    ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profList);

#endif
	    if (back == 0) {
		ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profList);
		back = 1;
	    }
	    ProfDelInstByName(mibType, tmp); 
	    tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
	}
    }
#if (PPTP_CLIENT == FYES)
    if (strlen(instName) != 0) { /* 有需要删除的client实例 */
	tmp = strtok(instName, UTT_SPLIT_SIGN_STR);
	while(tmp != NULL) {
#if (STATIC_ROUTE_BIND_PPTP == FYES)
	    snprintf(UserName, sizeof(UserName), "(PPTPC)%s", tmp);
	    PPTP_PRINT("%s-%d, UserName = %s\n", __func__, __LINE__, UserName);
	    if(isUserInStaticRoute(UserName) == 1)
	    {/*查看此用户名是否被静态路由引用*/
		ErrMsg1 = getTransDataForC(C_LANG_INDEX_USER);
		ErrMsg = getTransDataForC(C_LANG_INDEX_USER_IS_USE_IN_STATIC_ROUTE);
		snprintf(referedMsg, sizeof(referedMsg), "%s%s%s", ErrMsg1, tmp, ErrMsg);
		setTheErrorMsg((const char *)referedMsg);
		ProfUpdate(profList);
		ProfFreeAllocList(profList);
		nvramWriteCommit();
		goto out;
	    }
#endif
#if(IP_SEC == FYES)
	    memset(profName, 0, sizeof(profName));
	    snprintf(profName, sizeof(profName), "PC_%s", tmp);
	    PPTP_PRINT("%s-%d, profName = %s\n", __func__, __LINE__, profName);
	    if(isUserInIPSecProf(profName) == 1)
	    {/*查看此用户名是否被IPSec引用*/
		ErrMsg2 = getTransDataForC(C_LANG_INDEX_USER_IS_USE_IN_IPSEC);
		snprintf(errMsg_ipsec, sizeof(errMsg_ipsec), "%s%s", tmp, ErrMsg2);
		setTheErrorMsg((const char *)errMsg_ipsec);
		ProfUpdate(profList);
		ProfFreeAllocList(profList);
		nvramWriteCommit();
		goto out;
	    }
#endif

	    /*
	     * 删除的都备份，在profile change时，针对每个删除的实例调用脚本
	     */
	    ProfBackupByName(mibTypeC, PROF_CHANGE_DEL, tmp, &profList);
	    ProfDelInstByName(mibTypeC, tmp); 
	    tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
	}
    }
#endif

    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
out:
    websRedirect(wp,"PPTPClientList.asp");
    return;
}

/*
 * pptp页面列表全部删除
 */
static void formPptpListDelAll(webs_t wp, char_t *path, char_t *query) {
    int min = 0, max = 0;
    int i = 0;
    int back = 0;
    MibProfileType mibType = MIB_PROF_PPTP_SRV_ACCOUNT;
    Pointer prof;
    struProfAlloc *profList  = NULL;
#if (PPTP_CLIENT == FYES)
    MibProfileType mibTypeC = MIB_PROF_PPTP_CLI;
#if 0
    int back1;
#endif
#endif
#if (STATIC_ROUTE_BIND_PPTP == FYES)
    //char UserName[UTT_INST_NAME_LEN + 1 + 12];/*(PPTPS)用户名*/
    char UserName[33];//1.7.4修改vpn帐号长度为32
    PPTPSrvAccProfile *profPptp = NULL;
    PPTPCliProfile *profPptpC = NULL;
    char referedMsg[100];
    const char *ErrMsg = NULL;
    const char *ErrMsg1 = NULL;

    memset(referedMsg, 0, sizeof(referedMsg));
#endif
#if (IP_SEC == FYES)
    char profName[UTT_INST_NAME_LEN + 1 + 12];/*(PPTPS)用户名*/
    PPTPSrvAccProfile *prof_srv = NULL;
    PPTPCliProfile *prof_cli = NULL;
    char errMsg_ipsec[100];
    const char *ErrMsg2 = NULL;
    memset(errMsg_ipsec, 0, sizeof(errMsg_ipsec));
    memset(profName, 0, sizeof(profName));
#endif
    a_assert(wp);
    ProfInstLowHigh(mibType, &max, &min);
#if (STATIC_ROUTE_BIND_PPTP == FYES)
    for(i = min; i < max; i++) {
        prof = ProfGetInstPointByIndex(mibType, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0)) 
	{/*PPTP服务端由于PROF_CHANGE_DELALL会删除所有账号信息，故要预先检查是否有不能被删除的账号，若有，则直接退出，一个都不删除*/
	    profPptp = ProfGetInstPointByIndex(mibType, i);
	    snprintf(UserName, sizeof(UserName), "(PPTPS)%s", profPptp->head.name);
	    PPTP_PRINT("%s-%d, UserName = %s\n", __func__, __LINE__, UserName);
	    if(isUserInStaticRoute(UserName) == 1)
	    {/*查看此用户名是否被静态路由引用*/
		ErrMsg1 = getTransDataForC(C_LANG_INDEX_USER);
		ErrMsg = getTransDataForC(C_LANG_INDEX_USER_IS_USE_IN_STATIC_ROUTE);
		snprintf(referedMsg, sizeof(referedMsg), "%s%s%s", ErrMsg1, profPptp->head.name, ErrMsg);
		setTheErrorMsg((const char *)referedMsg);
		goto out;
	    }
	}
    }
#endif
#if (IP_SEC == FYES)
    for(i = min; i < max; i++) {
        prof = ProfGetInstPointByIndex(mibType, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0)) 
	{/*PPTP服务端由于PROF_CHANGE_DELALL会删除所有账号信息，故要预先检查是否有不能被删除的账号，若有，则直接退出，一个都不删除*/
	    prof_srv = ProfGetInstPointByIndex(mibType, i);
	    snprintf(profName, sizeof(profName), "PS_%s", prof_srv->head.name);
	    PPTP_PRINT("%s-%d, profName = %s\n", __func__, __LINE__, profName);
	    if(isUserInIPSecProf(profName) == 1)
	    {/*查看此用户名是否被IPSec引用*/
		ErrMsg2 = getTransDataForC(C_LANG_INDEX_USER_IS_USE_IN_IPSEC);
		snprintf(errMsg_ipsec, sizeof(errMsg_ipsec), "%s%s", prof_srv->head.name, ErrMsg2);
		setTheErrorMsg((const char *)errMsg_ipsec);
		goto out;
	    }
	}
    }
#endif

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
#if (PPTP_CLIENT == FYES)
    ProfInstLowHigh(mibTypeC, &max, &min);

    for(i = min; i < max; i++) {
        prof = ProfGetInstPointByIndex(mibTypeC, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
#if (STATIC_ROUTE_BIND_PPTP == FYES)
	    profPptpC = ProfGetInstPointByIndex(mibTypeC, i);
	    snprintf(UserName, sizeof(UserName), "(PPTPC)%s", profPptpC->head.name);
	    PPTP_PRINT("%s-%d, UserName = %s\n", __func__, __LINE__, UserName);
	    if(isUserInStaticRoute(UserName) == 1)
	    {/*查看此用户名是否被静态路由引用*/
		ErrMsg1 = getTransDataForC(C_LANG_INDEX_USER);
		ErrMsg = getTransDataForC(C_LANG_INDEX_USER_IS_USE_IN_STATIC_ROUTE);
		snprintf(referedMsg, sizeof(referedMsg), "%s%s%s", ErrMsg1, profPptpC->head.name, ErrMsg);
		setTheErrorMsg((const char *)referedMsg);
		/*PPTP客户端由于PROF_CHANGE_DELALL只删除本身相关数据，故可以类似PROF_CHANGE_DEL的处理，遇到不能删除的则发消息给speedweb触发profile change将已删除的账号的相关信息删除*/
		ProfUpdate(profList);
		ProfFreeAllocList(profList);
		nvramWriteCommit();
		goto out;
	    }
#endif
#if(IP_SEC == FYES)
	    prof_cli = ProfGetInstPointByIndex(mibTypeC, i);
	    snprintf(profName, sizeof(profName), "PC_%s", prof_cli->head.name);
	    PPTP_PRINT("%s-%d, profName = %s\n", __func__, __LINE__, profName);
	    if(isUserInIPSecProf(profName) == 1)
	    {/*查看此用户名是否被IPSec引用*/
		ErrMsg2 = getTransDataForC(C_LANG_INDEX_USER_IS_USE_IN_IPSEC);
		snprintf(errMsg_ipsec, sizeof(errMsg_ipsec), "%s%s", prof_cli->head.name, ErrMsg2);
		setTheErrorMsg((const char *)errMsg_ipsec);
		/*PPTP客户端由于PROF_CHANGE_DELALL只删除本身相关数据，故可以类似PROF_CHANGE_DEL的处理，遇到不能删除的则发消息给speedweb触发profile change将已删除的账号的相关信息删除*/
		ProfUpdate(profList);
		ProfFreeAllocList(profList);
		nvramWriteCommit();
		goto out;
	    }
#endif

	    /*
	     * 删除的实例都备份
	     */
            ProfBackupByIndex(mibTypeC, PROF_CHANGE_DELALL, i, &profList);
            ProfDelInstByIndex(mibTypeC, i);
        }
    }
#endif
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
#if ((STATIC_ROUTE_BIND_PPTP == FYES)||(IP_SEC == FYES))
out:
#endif
    websRedirect(wp, "PPTPClientList.asp");
    return;
}

/*
 * pptp服务器页面列表信息输出
 */
static int aspOutputPptpSrvListData(int eid, webs_t wp, int argc, char_t **argv) {
    MibProfileType mibType = MIB_PROF_PPTP_SRV_ACCOUNT;
    PPTPSrvAccProfile *profPptpA = NULL;
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

    PPTPSrvGlobalProfile *profPptpS = NULL;

    char_t *enables = T("srvAcc_enables[%d] = \"%d\";");
    char_t *instNames = T("srv_instNames[%d] = \"%s\";");
    char_t *peerTypes = T("srv_peerTypes[%d] = \"%s\";");
    char_t *userNames = T("srv_userNames[%d] = \"%s\";");
    char_t *remoteGwIps = T("srv_remoteGwIps[%d] = \"%s\";");
    char_t *remoteLanIps = T("srv_remoteLanIps[%d] = \"%N\";");
    char_t *remoteNetmasks = T("srv_remoteNetmasks[%d] = \"%N\";");
    char_t *statuss = T("srv_statuss[%d] = \"%s\";");
    char_t *useTimes = T("srv_useTimes[%d] = \"%d\";");
    char_t *inboundss = T("srv_inboundss[%d] = \"%s\";");
    char_t *outboundss = T("srv_outboundss[%d] = \"%s\";");

    websWrite(wp, "var srvAcc_enables = new Array();");
    websWrite(wp, "var srv_instNames = new Array();");
    websWrite(wp, "var srv_peerTypes = new Array();");
    websWrite(wp, "var srv_userNames = new Array();");
    websWrite(wp, "var srv_remoteGwIps = new Array();");
    websWrite(wp, "var srv_remoteLanIps = new Array();");
    websWrite(wp, "var srv_remoteNetmasks = new Array();");
    websWrite(wp, "var srv_statuss = new Array();");
    websWrite(wp, "var srv_useTimes = new Array();");
    websWrite(wp, "var srv_inboundss = new Array();");
    websWrite(wp, "var srv_outboundss = new Array();");

    /* 读状态信息 */
    fd = open(UTT_PPTP_CLIENT_INFO_FILE, O_RDWR);
    sysinfo(&sysInfo);
    PPTP_PRINT("%s: fd = %d, file=%s\n", __FUNCTION__, fd, UTT_PPTP_CLIENT_INFO_FILE);

    ProfInstLowHigh(mibType, &max, &min);

#if 0
    if (fd != -1) {
	start = lseek(fd, 0, SEEK_SET);
	end = lseek(fd, 0, SEEK_END);
	fileLen = end - start;
	lseek(fd, 0, SEEK_SET);
	found = 0;

    } else {
	for(i = min; i < max; i++) {
	    profPptpA = (PPTPSrvAccProfile *)ProfGetInstPointByIndex(mibType, i);
	    if((profPptpA != NULL) && (ProfInstIsFree(profPptpA) == 0)) {
		websWrite(wp, instNames, totalrecs, profPptpA->head.name);
		websWrite(wp, peerTypes, totalrecs, getEnumString(vpnPeerTypeEnum, profPptpA->peerType));
		websWrite(wp, userNames, totalrecs, profPptpA->userName);
#if 0
	    websWrite(wp, remoteLanIps, totalrecs, profPptpA->remoteLanIp);
#endif
		/* mantis 8180 */
		websWrite(wp, remoteLanIps, totalrecs, profPptpA->remoteLanIpDis);
		websWrite(wp, remoteNetmasks, totalrecs, profPptpA->remoteLanNetmask);
		websWrite(wp, statuss, totalrecs, T("0"));
		websWrite(wp, useTimes, totalrecs, T("-"));
		websWrite(wp, inboundss, totalrecs, T("-"));
		websWrite(wp, outboundss, totalrecs, T("-"));
	    }
	}
    }
#endif
    for(i = min; i < max; i++) {
        profPptpA = (PPTPSrvAccProfile *)ProfGetInstPointByIndex(mibType, i);
        if((profPptpA != NULL) && (ProfInstIsFree(profPptpA) == 0)) {
	    websWrite(wp, enables, totalrecs, profPptpA->head.active);
            websWrite(wp, instNames, totalrecs, profPptpA->head.name);
            websWrite(wp, peerTypes, totalrecs, getEnumString(vpnPeerTypeEnum, profPptpA->peerType));
            websWrite(wp, userNames, totalrecs, profPptpA->userName);
#if 0
	    websWrite(wp, remoteLanIps, totalrecs, profPptpA->remoteLanIp);
#endif
	    /* mantis 8180 */
	    websWrite(wp, remoteLanIps, totalrecs, profPptpA->remoteLanIpDis);
            websWrite(wp, remoteNetmasks, totalrecs, profPptpA->remoteLanNetmask);
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

		    PPTP_PRINT("%s:conTime = %d, ip=%s, user=%s, userName=%s\n", __FUNCTION__, info.ConTime,
			    info.ip, info.user, profPptpA->userName);
		    if ((info.ConTime == 0) && (strcmp(info.ip, "") != 0) && (strcmp(info.user, profPptpA->userName) == 0)) {
			found = 1;
			websWrite(wp, statuss, totalrecs, T("1"));
			websWrite(wp, useTimes, totalrecs, sysInfo.uptime - info.STime);
			websWrite(wp, remoteGwIps, totalrecs, info.gwip);
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
		    PPTP_PRINT("%s: user=%s, not found\n", __FUNCTION__, profPptpA->userName);
		    websWrite(wp, statuss, totalrecs, T("0"));
		    websWrite(wp, T("srv_useTimes[%d] = \"%s\";"), totalrecs, T("-"));
		    websWrite(wp, remoteGwIps, totalrecs, T("0.0.0.0"));
		    websWrite(wp, inboundss, totalrecs, T("-"));
		    websWrite(wp, outboundss, totalrecs, T("-"));
		}
	    } else {
		websWrite(wp, statuss, totalrecs, T("0"));
		websWrite(wp, T("srv_useTimes[%d] = \"%s\";"), totalrecs, T("-"));
		websWrite(wp, remoteGwIps, totalrecs, T("0.0.0.0"));
		websWrite(wp, inboundss, totalrecs, T("-"));
		websWrite(wp, outboundss, totalrecs, T("-"));
	    }

           totalrecs++;
        }
    }

    if (fd != -1) {
	close(fd);
    }

    websWrite(wp,"var srv_totalrecs = %d;", totalrecs);
#if (VPN_ITEM_CONTROL == FYES)
    websWrite(wp,"var srv_max_totalrecs = 0;"); /*并发条目数由pptp client 输出*/
#else
    websWrite(wp,"var srv_max_totalrecs = %d;", max);
#endif

    profPptpS = (PPTPSrvGlobalProfile *)ProfGetInstPointByIndex(MIB_PROF_PPTP_SRV_GLOBAL, 0);
    if (profPptpS == NULL) {
	websWrite(wp,"var srv_enables = 0;");
    } else {
	websWrite(wp,"var srv_enables = %d;", profPptpS->head.active);
    }

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

static int aspOutputPptpSrvAccountHardFeature(int eid, webs_t wp, int argc, char_t **argv) {
#if (HARD_FEATURE == FYES)
    websWrite(wp, "var hardFlag = %d;", 1);
#else
    websWrite(wp, "var hardFlag = %d;", 0);
#endif
    return 0;
}
/*
 * pptp服务器账号配置页面信息输出
 * 根据读入隧道名称，输出相关隧道名称信息
 */
static int aspOutputPptpSrvAccountData(int eid, webs_t wp, int argc, char_t **argv) {
    MibProfileType mibType = MIB_PROF_PPTP_SRV_ACCOUNT;
    PPTPSrvAccProfile *profPptpA = NULL;
    int min = 0, max = 0, i = 0;

    char *editName;
	const char *ErrMsg = NULL;

    websWrite(wp, "var instNames = \"\";");
    websWrite(wp, "var peerTypes = \"\";");
    websWrite(wp, "var userNames = \"\";");
    websWrite(wp, "var remoteLanIps = \"\";");
    websWrite(wp, "var remoteNetmasks = \"\";");
    websWrite(wp, "var passwds = \"\";");

    if (ejArgs(argc, argv, T("%s"), &editName) < 1) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_READ_DATA_ERR);
	setTheErrorMsg(ErrMsg);
    } else {
	ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++) {
	    profPptpA = (PPTPSrvAccProfile *)ProfGetInstPointByIndex(mibType, i);
	    if((profPptpA != NULL) && (ProfInstIsFree(profPptpA) == 0) &&
		    (editName != NULL) && (strcmp(editName, profPptpA->head.name) == 0)) {
		websWrite(wp, "instNames = \"%s\";", profPptpA->head.name);
		websWrite(wp, "peerTypes = \"%s\";", getEnumString(vpnPeerTypeEnum, profPptpA->peerType));
		websWrite(wp, "userNames = \"%s\";", profPptpA->userName);
		websWrite(wp, "passwds = \"%s\";", profPptpA->passwd);
#if (PPTP_IP_BIND == FYES)
		if(profPptpA->bindIp == 0)
		{
		    websWrite(wp, "var bindIps = \"\";");
		}
		else
		{
		    websWrite(wp, "var bindIps = \"%N\";", profPptpA->bindIp);
		}
#endif
#if (HARD_FEATURE == FYES)
		websWrite(wp, "var LanMacStr = \"%s\";", converMac6To12Byte(profPptpA->hardF));
#endif
#if 0
		websWrite(wp, "remoteLanIps = \"%N\";", profPptpA->remoteLanIp);
#endif
		websWrite(wp, "remoteLanIps = \"%N\";", profPptpA->remoteLanIpDis);
		websWrite(wp, "remoteNetmasks = \"%N\";", profPptpA->remoteLanNetmask);
		break;
	    }
	}
    }
    
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
/********************************************************************
 * 函数： formPPTPSrvAccAllow
 * 功能： 列表中的允许
 * 参数： 无
 * 输出： 无
 * 返回： 无
 * 作者： Jianqing.Sun
 * 创建： 2013-03-08
 *******************************************************************/
static void formPPTPSrvAccAllow(webs_t wp, char *path, char *query)
{
    UINT32 Allow;
    char_t *AllowID = NULL;
    struProfAlloc *profhead = NULL;
    MibProfileType mibType = MIB_PROF_PPTP_SRV_ACCOUNT;
    ProfChangeType profAction;
    PPTPSrvAccProfile *profA = NULL;
    int instIndex;
    const char *ErrMsg = NULL;

    Allow = (UINT32)strtoul(websGetVar(wp, T("Allow"), NULL),NULL,0);
    AllowID = websGetVar(wp, T("AllowID"), NULL);

    PPTP_PRINT("%s-%d: Allow = %d, AllowID = %s\n", __func__, __LINE__, Allow, AllowID);
    if (strlen(AllowID) == 0)
    {
	/* check variable */
	ErrMsg = getTransDataForC(C_LANG_INDEX_PLEASESELECT_USER);
	setTheErrorMsg(ErrMsg);
	goto out;
    }

    profAction = PROF_CHANGE_EDIT;
    instIndex = ProfGetInstPointByValue(mibType, OFFANDSIZE(PPTPSrvAccProfile, userName), TSTR, AllowID, -1);
    if(instIndex == PROFFAIL) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_DID_NOT_FIND_CASE);
	setTheErrorMsg(ErrMsg);
	goto out;
    }
    ProfBackupByIndex(mibType, profAction, instIndex, &profhead);
    profA = (PPTPSrvAccProfile*)ProfGetInstPointByIndex(mibType, instIndex);
    profA->head.active = Allow;/*允许或禁止*/
    PPTP_PRINT("%s-%d: profA->head.active = %d\n", __func__, __LINE__, profA->head.active);

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
    websRedirect(wp, "PPTPClientList.asp");/*页面跳转*/
}


/*
 * 定义页面处理句柄
 */
extern void formDefinePptpServer(void) {

    websAspDefine(T("OutputPptpSrvGlobalData"), aspOutputPptpSrvGlobalData);
    websFormDefine(T("pptpSrvGlobalConfig"), formPptpSrvGlobalConfig);
    websAspDefine(T("OutputPptpSrvListData"), aspOutputPptpSrvListData);
    websFormDefine(T("pptpSrvAccountConfig"), formPptpSrvAccountConfig);
    websFormDefine(T("pptpListDel"), formPptpListDel);
    websFormDefine(T("pptpListDelAll"), formPptpListDelAll);
    websFormDefine(T("formPPTPSrvAccAllow"), formPPTPSrvAccAllow);
    websAspDefine(T("OutputPptpSrvAccountData"), aspOutputPptpSrvAccountData);
    websAspDefine(T("OutputHardFeatureFlag"), aspOutputPptpSrvAccountHardFeature);

    return;
}
#endif
