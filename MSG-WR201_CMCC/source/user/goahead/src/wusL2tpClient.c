#include "uttMachine.h"
#if (L2TP_CLIENT == FYES)
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

/*
 * 页面L2TP服务器账号配置提交
 */
static void formL2tpCliAccountConfig(webs_t wp, char_t *path, char_t *query) {
    MibProfileType mibType = MIB_PROF_L2TP_CLIENT;
    struProfAlloc *profhead = NULL;
    L2TPCliProfile *prof = NULL;
    int instIndex;
    int editIndex;
    ProfChangeType profAction;
    struct in_addr addr_ip, addr_mask;
    ProfPppAuthEnum mode;
    const char *ErrMsg = NULL;

    char *action=NULL, *L2TPClientEnable=NULL, *chnlName=NULL, *userName=NULL, *passwd=NULL,
	 *AuthType=NULL, *serverLanIp=NULL, *serverLanNetMask=NULL, *serverIp=NULL, *editName=NULL, *mtu=NULL;
#if (VPN_PROXY == FYES)
    char *vpnMode=NULL;
#endif

    a_assert(wp);
    action = websGetVar(wp, T("Action"), T(""));
    L2TPClientEnable = websGetVar(wp, "L2TPClientEnable", T(""));
    if(strcmp(L2TPClientEnable, "on") != 0) {
        L2TPClientEnable="off";
    }
    chnlName = websGetVar(wp, "setName", T(""));
    userName = websGetVar(wp, "userName", T(""));
    passwd = websGetVar(wp, "passwd", T(""));
    AuthType = websGetVar(wp, "AuthType", T(""));
    serverLanIp = websGetVar(wp, "serverLanIp", T(""));
    serverLanNetMask = websGetVar(wp, "serverLanNetMask", T(""));
    serverIp = websGetVar(wp, "serverIp", T(""));
    editName = websGetVar(wp, "oldSetName", T(""));
    mtu = websGetVar(wp, "MTU", T("1478"));
#if (VPN_PROXY == FYES)
    vpnMode = websGetVar(wp, "vpnMode", T("2"));
#endif

    if ((chnlName == NULL) || (L2TPClientEnable == NULL) || (userName == NULL) || \
	    (passwd == NULL) || (AuthType == NULL) || (serverLanIp == NULL) || \
	    (serverLanNetMask == NULL) || (serverIp == NULL) || (action == NULL) || (editName == NULL)) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_PARAMETER_ERR);
	setTheErrorMsg(ErrMsg);
	goto out;
    }
    L2TP_PRINT("%s-%d: action=%s, l2tpenable=%s, chnlName=%s, user=%s, passwd=%s, authType=%s, lanIp=%s, lanNm=%s,\
	    serverIp=%s,editName=%s,mtu=%s\n", __FUNCTION__, __LINE__,action, L2TPClientEnable, chnlName,\
	    userName, passwd, AuthType, serverLanIp, serverLanNetMask, serverIp, editName,mtu);
    /*
     * ip地址判断
     */
    if (inet_aton(serverLanIp, &addr_ip) == 0) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_CLIENT_REMOTE_CIN_ERR);
		setTheErrorMsg(ErrMsg);
		goto out;
    }
    if (inet_aton(serverLanNetMask, &addr_mask) == 0) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_CLIENT_REMOTE_MASK_CIN_ERR);
		setTheErrorMsg(ErrMsg);
		goto out;
    }
    /* 远端内网IP网段不能与内网网段重叠 */
    if(ntohl(addr_ip.s_addr) != 0) {
	if (isOverlapInnerIp(ntohl(addr_ip.s_addr), ntohl(addr_mask.s_addr)) == 1) {
	    ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_CLIENT_REMOTE_LOCAL_S_ERR);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
    }
    /*
     * 密码验证类型
     */
    mode = getEnumValue(pppAuthEnum, AuthType);
    if (mode == -1) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_USELESS_PASSWD_CHECK_ERR);
	setTheErrorMsg(ErrMsg);
	goto out;
    }

    instIndex = ProfGetInstIndexByName(mibType, chnlName);
#if (VPN_PROXY == FYES)
    if (instIndex == PROFFAIL) action = "add";
    else action = "modify";
#endif

    if (strcmp(action, "add") == 0) {
	/*判断用户名是否重复*/
	if(ProfGetInstPointByValue(mibType, OFFANDSIZE(L2TPCliProfile, userName), TSTR, userName, -1) != PROFFAIL)
	{
	    ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_ALREADY_EXITS);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
        if(instIndex != PROFFAIL) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_TUNNEL_NAME_ERR);
		    setTheErrorMsg(ErrMsg);
		    goto out;
        }
#if (VPN_ITEM_CONTROL == FYES)
	if(checkVpnFreeItems() == 0) {
	    ErrMsg = getTransDataForC(C_LANG_INDEX_MAX_VPN_ITEM);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
#endif

        instIndex = ProfNewInst(mibType, chnlName, FALSE);

        if(instIndex == PROFFAIL) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_MAX_TOTAL_ERR);
			setTheErrorMsg(ErrMsg);
		    goto out;
        }
        profAction = PROF_CHANGE_ADD;
    } else {
	editIndex= ProfGetInstIndexByName(mibType, editName);
	/*判断用户名是否重复*/
	if(ProfGetInstPointByValue(mibType, OFFANDSIZE(L2TPCliProfile, userName), TSTR, userName, editIndex) != PROFFAIL)
	{
	    ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_ALREADY_EXITS);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	/* 判断是否修改设置名 */
	if (strcmp(chnlName, editName) != 0) {
	    if (instIndex != PROFFAIL) { /* 不能修改成已存在的用户名 */
			ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_CLIENT_TUNNEL_EXITS_ERR);
			setTheErrorMsg(ErrMsg);
			goto out;
	    }
	    /* 根据old user获取profile索引 */
	    instIndex = ProfGetInstIndexByName(mibType, editName);
	}
		if(instIndex == PROFFAIL) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_CANT_TUNNEL_ERR);
			setTheErrorMsg(ErrMsg);
			goto out;
		}
        profAction = PROF_CHANGE_EDIT;
	/* profile change 调用 pptp-down.sh */
    }

    ProfBackupByIndex(mibType, profAction, instIndex, &profhead);
    prof = (L2TPCliProfile *)ProfGetInstPointByIndex(mibType, instIndex);

    ProfSetNameByPoint(prof, chnlName);
    if (strcmp(L2TPClientEnable, "on") == 0) {
	prof->head.active = 1;
#if (VPN_PROXY == FYES)
	prof->connectStatus = 1;
#endif
    } else {
	prof->head.active = 0;
#if (VPN_PROXY == FYES)
	prof->connectStatus = 0;
#endif
    }

    strncpy(prof->userName, userName, 32); //1.7.4 修改vpn帐号长度为32
    strncpy(prof->passwd, passwd, UTT_PASSWD_LEN);
    prof->pppAuthMode = mode;
    prof->remoteLanIp = addr_ip.s_addr & addr_mask.s_addr;
    /* mantis 8180 */
    prof->remoteLanIpDis = addr_ip.s_addr;
    prof->remoteLanNetmask = addr_mask.s_addr;
    strncpy(prof->serverHost, serverIp, UTT_HOST_LEN);
    prof->mtu = strtol(mtu, NULL, 10);
    L2TP_PRINT("%s-%d: prof->head.active =%d, head.name =%s, prof->username =%s, prof->passwd =%s,\ 
	    prof->pppAuthMode =%d, prof->remoteLanIp =0x%x, prof->remoteLanIpDis =0x%x, prof->remoteNetMask =0x%x,\
	    prof->serverHost =%s, prof->mtu =%d\n",__func__,__LINE__,prof->head.active,prof->head.name,prof->userName,\
	    prof->passwd,prof->pppAuthMode,prof->remoteLanIp,prof->remoteLanIpDis, prof->remoteLanNetmask,\
	    prof->serverHost,prof->mtu);
#if (VPN_PROXY == FYES)
    prof->vpnMode = strtol(vpnMode, NULL, 5);
#endif

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
#if (VPN_PROXY == FYES)
    setSuccessTips();
    sleep(2);
    websRedirect(wp,"VPNSui.asp");
#else
    websRedirect(wp, "L2TPList.asp");
#endif
    return;
}

/* read a line (not more than 127 bytes) from filename to buf */
char* read_file_l2tp(char *filename, char *buf)
{
    FILE *fp;

    fp=fopen(filename, "r");
    if (fp == NULL ) {
	printf("open %s error\n", filename);
    }else {
	/* the content of the file is a byte, which indicant 
	 * the link status of the chnl_name port. 
	 * while '1' for up, and '0' for down */
	if ((fread((void *)buf, 127U, 1U, fp)) < 0U) {
	    printf("read %s error\n", filename);
	} else {
	    char *p=strchr(buf, '\n');
	    if(p != NULL){
		*p='\0';
	    }
	}
	fclose(fp);
    }
    return buf;
}

/*
 * l2tp服务器页面列表信息输出
 */
static int aspOutputL2tpCliListData(int eid, webs_t wp, int argc, char_t **argv) {
    MibProfileType mibType = MIB_PROF_L2TP_CLIENT;
    L2TPCliProfile *prof = NULL;
    int min = 0, max = 0, i = 0, totalrecs = 0;
    struct st_uttPppdClient info;
    struct sysinfo sysInfo;
    int st_size = sizeof(struct st_uttPppdClient);
    char buf[128] = {0};
    char fileName[64] = {0};
    long long rx, tx;

    char_t *L2TPClientEnables=T("L2TPClientEnables[%d]=\"%s\";\n");
    char_t *setNames=T("cli_setNames[%d]=\"%s\";\n");
    char_t *userNames=T("cli_userNames[%d]=\"%s\";\n");
    char_t *serverGwIps = T("cli_serverGwIps[%d] = \"%s\";\n");
    char_t *serverLanIps=T("cli_serverLanIps[%d]=\"%N\";\n");
    char_t *serverLanNetMasks=T("cli_serverLanNetMasks[%d]=\"%N\";\n");
    char_t *statuss=T("cli_statuss[%d]=\"%s\";\n");
    char_t *usetimes=T("cli_usetimes[%d]=\"%s\";\n");
    char_t *outtimes=T("cli_outtimes[%d]=\"%s\";\n");
    char_t *outboundss=T("cli_outboundss[%d]=\"%s\";\n");
    char_t *inboundss=T("cli_inboundss[%d]=\"%s\";\n");
    websWrite(wp, "var L2TPClientEnables = new Array();");
    websWrite(wp, "var cli_setNames = new Array();");
    websWrite(wp, "var cli_userNames = new Array();");
    websWrite(wp, "var cli_serverGwIps = new Array();");
    websWrite(wp, "var cli_serverLanIps = new Array();");
    websWrite(wp, "var cli_serverLanNetMasks = new Array();");
    websWrite(wp, "var cli_statuss=new Array();");
    websWrite(wp, "var cli_usetimes = new Array();");
    websWrite(wp, "var cli_outtimes = new Array();");
    websWrite(wp, "var cli_outboundss = new Array();");
    websWrite(wp, "var cli_inboundss = new Array();");

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) {
	prof = (L2TPCliProfile *)ProfGetInstPointByIndex(mibType, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
	    /*
	     * 输出配置信息
	     */
	    if (prof->head.active == 1) {
		websWrite(wp, L2TPClientEnables, totalrecs, "on");
	    } else {
		websWrite(wp, L2TPClientEnables, totalrecs, "off");
	    }
            websWrite(wp, setNames, totalrecs, prof->head.name);
            websWrite(wp, userNames, totalrecs, prof->userName);
#if 0
            websWrite(wp, serverLanIps, totalrecs, prof->remoteLanIp);
#endif
	    websWrite(wp, serverGwIps, totalrecs, prof->serverHost);
	    /* mantis 8180 */
            websWrite(wp, serverLanIps, totalrecs, prof->remoteLanIpDis);
            websWrite(wp, serverLanNetMasks, totalrecs, prof->remoteLanNetmask);
	    /*
	     * 输出速率等信息
	     */
#if 0
            websWrite(wp, statuss, totalrecs, "0");
	    websWrite(wp, usetimes, totalrecs, "-");
            websWrite(wp, outtimes, totalrecs, "-");
            websWrite(wp, outboundss, totalrecs, "-");
            websWrite(wp, inboundss, totalrecs, "-");
#else
            sprintf(fileName, "/var/run/l2tp_%slink.status", prof->head.name);
            websWrite(wp, statuss, totalrecs, read_file_l2tp(fileName, buf));
	    if ((prof->head.active == 0) || (strcmp(buf, "1")) != 0) {
		websWrite(wp, usetimes, totalrecs, "-");
                websWrite(wp, outtimes, totalrecs, "-");
                websWrite(wp, outboundss, totalrecs, "-");
                websWrite(wp, inboundss, totalrecs, "-");
	    } else { /* 读速率等信息 */
		unsigned long begin, now;
                char *p;
                long long txByte, rxByte;

		/* 读pptp建立连接的时间 */
                sprintf(fileName, "/var/run/l2tp_%s.uptime", prof->head.name);
                read_file_l2tp(fileName, buf);
                p=strchr(buf,'.');
                *p='\0';
                begin=strtol(buf, NULL, 10);
		/* 读当前时间 */
                read_file_l2tp("/proc/uptime", buf);
                p=strchr(buf,'.');
                *p='\0';
                now=strtol(buf, NULL, 10);  
		/* 获得建立时间 */
                sprintf(buf, "%ld", now-begin);
                websWrite(wp, usetimes, totalrecs, buf);
                websWrite(wp, outtimes, totalrecs, "0");
		/* 获得下载，上传流量 */
                sprintf(fileName, "/var/run/l2tp_%s.ppp", prof->head.name);
                read_file_l2tp(fileName, buf);
                if((p=strchr(buf,' ')) != NULL)
                    *p='\0';
                txByte=getIfStatistic(buf, TXBYTE);       //send bytes
                rxByte=getIfStatistic(buf, RXBYTE);       //receive bytes
                sprintf(buf, "%lld", txByte);
                websWrite(wp, outboundss, totalrecs, buf);
                sprintf(buf, "%lld", rxByte);
                websWrite(wp, inboundss, totalrecs, buf);
	    }
#endif
	    totalrecs++;
	}
    }
    websWrite(wp, "var cli_totalrecs=%d;", totalrecs);
    websWrite(wp, "var cli_max_totalrecs=%d;", max);

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

/*
 * l2tp服务器账号配置页面信息输出
 * 根据读入隧道名称，输出相关隧道名称信息
 */
static int aspOutputL2tpCliAccountData(int eid, webs_t wp, int argc, char_t **argv) {
    MibProfileType mibType = MIB_PROF_L2TP_CLIENT;
    L2TPCliProfile *prof = NULL;
    struct in_addr ip, nm;
    const char *ErrMsg = NULL;
    char *EditName;
#if (VPN_PROXY == FYES)
    char *p;
    char buf[128] = {0};
    char fileName[64] = {0};
    unsigned long begin, now;
    websWrite(wp, "var useTimes=\"\"; ");
    websWrite(wp, "var vpnModes=\"\"; ");
#endif
#if(CONFIG_USER_GOAHEAD_LANG_EN == FYES)
	 websWrite(wp, "var SmartModevpnModes=\"%d\"; ",0);
#else
	 websWrite(wp, "var SmartModevpnModes=\"%d\"; ",1);
#endif
    websWrite(wp, "var L2TPClientEnables=\"\"; ");
    websWrite(wp, "var setNames=\"\"; ");
    websWrite(wp, "var userNames=\"\"; ");
    websWrite(wp, "var passwds=\"\"; ");
    websWrite(wp, "var AuthTypes=\"\"; ");
    websWrite(wp, "var serverLanIps=\"\"; ");
    websWrite(wp, "var serverLanNetMasks=\"\"; ");
    websWrite(wp, "var serverIps=\"\"; ");
    websWrite(wp, "var mtu=1478; ");
    
    if(ejArgs(argc,argv,T("%s"), &EditName) <1) {
		L2TP_PRINT("%s: EditName = %s\n", __FUNCTION__, EditName);
		ErrMsg = getTransDataForC(C_LANG_INDEX_READ_EDIT_NAME_VAL_ERR);
		setTheErrorMsg(ErrMsg);
    } else {
	L2TP_PRINT("%s: EditName = %s\n", __FUNCTION__, EditName);
	prof = (L2TPCliProfile *)ProfGetInstPointByName(mibType, EditName);
	if (prof == NULL) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_READ_CONFIG_FAIL);
	    setTheErrorMsg(ErrMsg);
	} else {
	    if (prof->head.active == 0) {
		websWrite(wp, "var L2TPClientEnables=\"%s\"; ", "off");
	    } else {
		websWrite(wp, "var L2TPClientEnables=\"%s\"; ", "on");
#if (VPN_PROXY == FYES)
                sprintf(fileName, "/var/run/l2tp_%slink.status", prof->head.name);
                read_file_l2tp(fileName, buf);
                if(strcmp(buf, "1") == 0)
                {
                    memset(fileName, 0, sizeof(fileName));
                    memset(buf, 0, sizeof(buf));
                    /* 读pptp建立连接的时间 */
                    sprintf(fileName, "/var/run/l2tp_%s.uptime", prof->head.name);
                    read_file_l2tp(fileName, buf);
                    p=strchr(buf,'.');
                    *p='\0';
                    begin=strtol(buf, NULL, 10);
                    /* 读当前时间 */
                    read_file("/proc/uptime", buf);
                    p=strchr(buf,'.');
                    *p='\0';
                    now=strtol(buf, NULL, 10);
                    /* 获得建立时间 */
                    sprintf(buf, "%ld", now-begin);
                    websWrite(wp, "var useTimes=\"%s\"; ", buf);
                }
		else if(prof->connectStatus == 1)
		{
		    websWrite(wp, "var useTimes=\"%s\"; ", "connecting");
		}
		else
		{
		    websWrite(wp, "var useTimes=\"%s\"; ", "-");
		}
#endif
	    }
            websWrite(wp, "var setNames=\"%s\"; ", prof->head.name);
            websWrite(wp, "var userNames=\"%s\"; ", prof->userName);
            websWrite(wp, "var passwds=\"%s\"; ", prof->passwd);
            websWrite(wp, "var AuthTypes=\"%s\"; ", getEnumString(pppAuthEnum, prof->pppAuthMode));
	    ip.s_addr = prof->remoteLanIpDis;
            websWrite(wp, "var serverLanIps=\"%s\"; ", inet_ntoa(ip));
	    nm.s_addr = prof->remoteLanNetmask;
            websWrite(wp, "var serverLanNetMasks=\"%s\"; ", inet_ntoa(nm));
            websWrite(wp, "var serverIps=\"%s\"; ", prof->serverHost);
	    websWrite(wp, "var mtu = %d;", prof->mtu);
#if (VPN_PROXY == FYES)
	    websWrite(wp, "var vpnModes = %d;", prof->vpnMode);
#endif
	}
    }
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

static void killOneL2tpClient(char *user) 
{
    int fd = -1;
    struct st_uttPppdClient info;
    int st_size = sizeof(struct st_uttPppdClient);
    if(user != NULL) {
    fd = open(UTT_L2TP_CLIENT_INFO_FILE, O_RDONLY);

    if(fd != -1) {
        while (read(fd, &info, st_size) > 0) {
	    L2TP_PRINT("%s: user=%s,pid=%d,STime=%d,LTime=%d,ConTime=%d,rx=%lld,tx=%lld\n",__FUNCTION__,info.user,info.pid,info.STime,info.LTime,info.ConTime,info.rxByte,info.txByte);
	    if (info.ConTime == 0) {
            if(strncmp(user, info.user, UTT_PPP_CLIENT_NAME_LEN) == 0) {
                doSystem("kill -INT %d", info.pid);
#if 0 //(VPN_CONTROL == FYES)
		/*服务端控制条目在pppd中处理*/
		doSystem("vpnHandle plus ");
#endif
		break;
            }
	    }
        }
	close(fd);
    }
    }
    return;
}

static void formL2tpClientHang(webs_t wp, char_t *path, char_t *query) 
{
#if (VPN_PROXY == FYES)
    MibProfileType mibType = MIB_PROF_L2TP_CLIENT;
    L2TPCliProfile *prof = NULL;
#endif
    char *tmp,*tmpsrv,*hangstr,*hangstrsrv;
    char fileName[64] = {0}, buf[10] = {0};
	const char *ErrMsg = NULL;

    a_assert(wp);
    hangstr = websGetVar(wp,T("hangstr"),T(""));
    hangstrsrv = websGetVar(wp,T("hangstrsrv"),T(""));

    if((strcmp(hangstr,T(""))==0)&&(strcmp(hangstrsrv,T(""))==0)){
		ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_CANT_TUNNEL_ERR);
        setTheErrorMsg(ErrMsg);
        goto out;
    }
    L2TP_PRINT("%s: hangstr =%s,hangstrsrv =%s\n", __FUNCTION__,hangstr,hangstrsrv);
    tmp=strtok(hangstr,UTT_SPLIT_SIGN_STR);
    while(tmp!=NULL) {
	L2TP_PRINT("%s: tmp = %s\n", __FUNCTION__, tmp);
	/* mantis 8921: 已断开不再挂断 */
	sprintf(fileName, "/var/run/l2tp_%slink.status", tmp);
        read_file_l2tp(fileName, buf);
#if (VPN_PROXY == FYES)
	prof = (L2TPCliProfile *)ProfGetInstPointByName(mibType,tmp);
	if ((prof->connectStatus != 1) && (strcmp(buf, "0") == 0)) {
	    prof->connectStatus = 0;
	    tmp = strtok(NULL, UTT_SPLIT_SIGN_STR);
	    continue;
	}
	prof->connectStatus = 0;
#else
	if (strcmp(buf, "0") == 0) { /* 已断开 */
	    tmp = strtok(NULL, UTT_SPLIT_SIGN_STR);
	    continue;
	}
#endif

	doSystem("l2tp-down.sh l2tp_%s ",tmp);
	doSystem("echo 'd %s' > %s",tmp, L2TP_CONTROL_PIPE);
	usleep(100000); /*0.1s*/
        tmp=strtok(NULL,UTT_SPLIT_SIGN_STR);
    }

    { /*l2tp server hang*/
	tmpsrv = strtok(hangstrsrv,UTT_SPLIT_SIGN_STR);
	while (tmpsrv!=NULL) {
	uttSemP(SEM_PPPOE_NO,0);
	/*挂断此账号*/
	killOneL2tpClient(tmpsrv);
	
	uttSemV(SEM_PPPOE_NO,0);
	tmpsrv=strtok(NULL,UTT_SPLIT_SIGN_STR);
	}
    }

out:
#if (VPN_PROXY == FYES)
    sleep(3);
    websRedirect(wp,"VPNSui.asp");
#else
    websRedirect(wp,"L2TPList.asp");
#endif
    return;
}

static void formL2tpClientDial(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_L2TP_CLIENT;
    L2TPCliProfile *prof = NULL;

    char *tmp,*dialstr; 
	const char *ErrMsg = NULL;
    char fileName[64], buf[128], cmdstr[128];

    a_assert(wp);
    dialstr = websGetVar(wp,T("dialstr"),T(""));

    L2TP_PRINT("%s, dialstr = %s\n", __FUNCTION__, dialstr);

    if(!strcmp(dialstr,T(""))){
		ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_CANT_TUNNEL_ERR);
        setTheErrorMsg(ErrMsg);
        goto out;
    }

    tmp=strtok(dialstr,UTT_SPLIT_SIGN_STR);
    while(tmp!=NULL) {
	L2TP_PRINT("%s: tmp = %s\n", __FUNCTION__, tmp);
	prof = (L2TPCliProfile *)ProfGetInstPointByName(mibType, tmp);
	if ((prof == NULL) || (prof->head.active == 0)) { /* 未启用 */
	    tmp = strtok(NULL, UTT_SPLIT_SIGN_STR);
	    continue;
	}

        sprintf(fileName, "/var/run/l2tp_%slink.status", tmp);
        read_file_l2tp(fileName, buf);
        if(strcmp(buf, "1") == 0) { /* 已连接 */
	    tmp = strtok(NULL, UTT_SPLIT_SIGN_STR);
	    continue;
        }
#if (VPN_CONTROL == FYES)
	if(nvramProfile->vpnProfile.count<=0)
	{
	    ErrMsg = getTransDataForC(C_LANG_INDEX_MAX_VPN_COUNT);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
#endif
	doSystem("l2tp-down.sh l2tp_%s ",tmp);
	doSystem("echo 'd %s' > %s",tmp, L2TP_CONTROL_PIPE);
	usleep(500000); /*0.5s*/
	doSystem("echo 'c %s' > %s",tmp, L2TP_CONTROL_PIPE);
        tmp=strtok(NULL,UTT_SPLIT_SIGN_STR);
#if (VPN_PROXY == FYES)
	prof->connectStatus = 1;
#endif
    }
out:
#if (VPN_PROXY == FYES)
    sleep(3);
    websRedirect(wp,"VPNSui.asp");
#else
    websRedirect(wp,"L2TPList.asp");
#endif
    return;
}


#if (VPN_PROXY == FYES)
static void formL2TPConnStatus(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_L2TP_CLIENT;
    L2TPCliProfile *prof = NULL;
    char *p;
    char buf[128] = {0};
    char fileName[64] = {0};
    unsigned long begin, now;
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain;charset=GB2312\nPragma: no-cache\nCache-Control: no-cache\n\n"));
    prof = (PPTPCliProfile *)ProfGetInstPointByName(mibType, "VPN_L2TP");
    if (prof == NULL) {
	websWrite(wp, T("var connStatus=\"-\"; "));
    }
    else if(prof->head.active == 0)
    {
	websWrite(wp, T("var connStatus=\"-\"; "));
    }
    else
    {
	sprintf(fileName, "/var/run/l2tp_%slink.status", prof->head.name);
	read_file(fileName, buf);
	if(strcmp(buf, "1") == 0)
	{
	    memset(fileName, 0, sizeof(fileName));
	    memset(buf, 0, sizeof(buf)); 
	    /* 读pptp建立连接的时间 */
	    sprintf(fileName, "/var/run/l2tp_%s.uptime", prof->head.name);
	    read_file_l2tp(fileName, buf);
	    p=strchr(buf,'.');
	    *p='\0';
	    begin=strtol(buf, NULL, 10);
	    /* 读当前时间 */
	    read_file_l2tp("/proc/uptime", buf);
	    p=strchr(buf,'.');
	    *p='\0';
	    now=strtol(buf, NULL, 10);
	    /* 获得建立时间 */
	    sprintf(buf, "%ld", now-begin);
	    websWrite(wp, T("var connStatus=\"%s\"; "), buf);
	}
	else if(prof->connectStatus == 1)
	{
	    websWrite(wp, T("var connStatus=\"connecting\"; "));
	}
	else
	{
	    websWrite(wp, T("var connStatus=\"-\"; "));
	}
    }
    websDone(wp,200);
}
#endif

/*
 * 定义页面处理句柄
 */
extern void formDefineL2tpClient(void) {

    websAspDefine(T("OutputL2tpCliListData"), aspOutputL2tpCliListData);
    websFormDefine(T("l2tpCliAccountConfig"), formL2tpCliAccountConfig);
    websAspDefine(T("OutputL2tpCliAccountData"), aspOutputL2tpCliAccountData);
    websFormDefine(T("formL2tpHang"), formL2tpClientHang);
    websFormDefine(T("formL2tpDial"), formL2tpClientDial);
#if (VPN_PROXY == FYES)
    websFormDefine(T("formL2TPConnStatus"), formL2TPConnStatus);
#endif

    return;
}
#endif
