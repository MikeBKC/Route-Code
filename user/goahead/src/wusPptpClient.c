#include "uttMachine.h"
#if (PPTP_CLIENT == FYES)
#include	<stdlib.h>
#include	<sys/ioctl.h>
#include	<net/if.h>
#include	<net/route.h>
#include        <string.h>
#include        <dirent.h>
#include	"internet.h"
#include	"webs.h"
#include	"utils.h"
#include 	"firewall.h"
#include	"management.h"
#include	"station.h"
#include	"uttfunction.h"
#include    "linux/mii.h"
#include    "linux/sockios.h"
/*
 * kernel config
 */
#include	"linux/autoconf.h"
/*
 * user config
 */
#include	"config/autoconf.h" 
#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include	"argcmd.h"
#include	"translate.h"
#include    "utt_pppoeClient.h" /*调用killOnePptpClient函数*/

#ifdef PPTP_DBG
#define PPTP_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define PPTP_PRINTF(fmt, args...)
#endif

#if (STATIC_ROUTE_BIND_PPTP == FYES)
extern int isUserInStaticRoute(char *name);
#endif
extern int isOverlapInnerIp(Uint32 ip, Uint32 nm);
extern int getNetConfig(int eid, webs_t wp, int argc, char_t **argv);

#if 0
static int pptpcnt(char *pptpList)
{
    int i=0;
    char *tmp, *tmpList;
#if 0
    tmp=strtok(pptpList,UTT_SPLIT_SIGN_RECORDS);
    for(i=0; tmp!=NULL;i++) {
        tmp=strtok(NULL, UTT_SPLIT_SIGN_RECORDS);
    }
#endif
    tmp=tmpList=pptpList;
    for(i=0; tmp!=NULL;i++) {
        tmp=strchr(tmpList,UTT_SPLIT_SIGN_RECORD);
        tmpList=tmp+1;
    }
    DBGPRINT("pptpcnt[%d]\n", i);
    return i;
}
#endif

#if 0
static void formConfigPptpClientConfig(webs_t wp, char_t *path, char_t *query)
{
    char *action, *editName, *PPTPClientEnable, *chnlName, *userName, *passwd, *AuthType,*serverLanIp,*serverLanNetMask, *serverIp;
    char *pptpList, *newList, *p1, *p2, cmdstr[128];
    int tempLen;

    DBGPRINT("********formConfigPptpClientConfig********\n");
    a_assert(wp);
    pptpList = nvram_bufget(RT2860_NVRAM,"PptpClients");
    action = websGetVar(wp, "Action", T(""));
    PPTPClientEnable = websGetVar(wp, "PPTPClientEnable", T(""));
    if(strcmp(PPTPClientEnable, "on") != 0) {
        PPTPClientEnable="off";
    }
    chnlName = websGetVar(wp, "setName", T(""));
    userName = websGetVar(wp, "userName", T(""));
    passwd = websGetVar(wp, "passwd", T(""));
    if(p1=strchr(passwd, UTT_SPLIT_SIGN_RECORD)) {
        *p1=(char)0x1FU;
    }
    AuthType = websGetVar(wp, "AuthType", T(""));
    serverLanIp = websGetVar(wp, "serverLanIp", T(""));
    serverLanNetMask = websGetVar(wp, "serverLanNetMask", T(""));
#if 0
    if(ip2int(serverLanIp) & ~ip2int(serverLanNetMask) != 0U ) {
            setTheErrorMsg(T("IP地址网段和掩码配置不一致"));
            goto OUT;
    }
#else
    unsigned int sLanIp=0U;
    char serverLanNet[16];
    sLanIp=ip2int(serverLanIp) & ip2int(serverLanNetMask);
    int2ip(sLanIp, serverLanNet);
    strcpy(serverLanIp, serverLanNet);
#endif
    serverIp = websGetVar(wp, "serverIp", T(""));

    DBGPRINT("pptpList before change:%s\n", pptpList);
    DBGPRINT("new record:chnlName[%s],PPTPClientEnable[%s], userName[%s],passwd[%s],AuthType[%s],serverLanIp[%s],serverLanNetMask[%s],serverIp[%s]\n",
            chnlName,PPTPClientEnable,userName,passwd,AuthType,serverLanIp,serverLanNetMask,serverIp);
    tempLen=strlen(pptpList)+strlen(chnlName)+strlen(PPTPClientEnable)+strlen(userName)+strlen(passwd)+strlen(AuthType)+60;

    DBGPRINT("Action[%s]\n", action);
    if (strcmp(action, "add")==0) {
        if(pptpcnt(pptpList) >= UTT_MAX_PPTP_CLIENT) {
            setTheErrorMsg(T("已达到最大数目"));
            goto OUT;
        }
        if (strnmatch(pptpList,chnlName,0)) {
            setTheErrorMsg(T("规则名重复"));
            goto OUT;
        }
        newList = (char*)malloc(sizeof(char) * tempLen);
        strcpy(newList, pptpList);
        p1=newList+strlen(pptpList);
        if(strlen(newList) != 0)
        {
            strcat(newList, UTT_SPLIT_SIGN_RECORDS);
            p1++;
        }
        sprintf(p1, "%s,%s,%s,%s,%s,%s,%s,%s", chnlName,PPTPClientEnable,userName,passwd,AuthType,serverLanIp,serverLanNetMask,serverIp);
    } else { /* modify */
        editName = websGetVar(wp, "oldSetName", T(""));
        DBGPRINT("oldChnlName[%s]\n", editName);
        p1=strnmatch(pptpList,editName,0);
        if (p1==0) {
            setTheErrorMsg(T("找不到该规则"));
            goto OUT;
        }
        DBGPRINT("stop pptp client\n");
        doSystem("pptp-down.sh pptp_%s ", editName);
        p2=p1;
        while( (*p2!=UTT_SPLIT_SIGN_RECORD) && *p2){
            p2++;
        } /* *p2=';'or '\0' */
        if( (strcmp(chnlName,editName)!=0) && strnmatch(pptpList,chnlName,0)) {
            setTheErrorMsg(T("规则名重复"));
            goto OUT;
        }
        *p1=0;
        newList = (char*)malloc(sizeof(char) * tempLen);
        strcpy(newList,pptpList);
        p1=newList+strlen(pptpList);
        sprintf(p1,"%s,%s,%s,%s,%s,%s,%s,%s%s",chnlName,PPTPClientEnable,userName,passwd,AuthType,serverLanIp,serverLanNetMask,serverIp,p2);
    }
    DBGPRINT("nvram_commit:PptpClients[%s][%d], tempLen[%d]\n", newList, strlen(newList), tempLen);
    nvram_bufset(RT2860_NVRAM,"PptpClients",newList);
    nvram_commit(RT2860_NVRAM);
    if (!strcmp(PPTPClientEnable, "on")) {
        DBGPRINT("start pptp client\n");
        doSystem("pptp.sh %s %s %s %s KeepAlive >/etc/options.pptp_%s", userName, passwd, serverIp, AuthType, chnlName);
        sprintf(cmdstr, "`ps |grep \"pppd file /etc/options.pptp_%s $\"|sed \"s/^ *//g\"|sed \"s/ .*$//g\"`", chnlName);
        doSystem("kill -9 %s >/dev/null 2>&1", cmdstr);
        DBGPRINT("cmdstr[%s]\n", cmdstr);
        doSystem("ip route del %s scope link", serverIp); /* delete error route */
        doSystem("pppd file /etc/options.pptp_%s &", chnlName);
    }
    free(newList);
OUT:
    websRedirect(wp, "PPTPClientList.asp");
    return;
}
#endif

/*
 * pptp客户端配置页面保存函数
 */
static void formConfigPptpClientConfig(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_PPTP_CLI;
    struProfAlloc *profhead = NULL;
    PPTPCliProfile *prof = NULL;
    int instIndex;
    int editIndex;
    ProfChangeType profAction;
    struct in_addr addr_ip, addr_mask;
    ProfPppAuthEnum mode;
	const char *ErrMsg = NULL;

    char *action = NULL, *editName = NULL, *PPTPClientEnable = NULL, *chnlName = NULL, 
	 *userName = NULL, *passwd = NULL, *AuthType = NULL, *serverLanIp = NULL,
	 *serverLanNetMask = NULL, *serverIp = NULL, *mtu = NULL;
#if (VPN_PROXY == FYES)
    char *vpnMode = NULL;
#endif
#if (PPTP_ENCRYPT_MODE == FYES)
    char *EncryptionMode = NULL;
#endif
    char *PPTPClientNATEnable = NULL;
#if (STATIC_ROUTE_BIND_PPTP == FYES)
    //char UserName[UTT_INST_NAME_LEN + 1 + 12];/*(PPTPC)用户名*/
    char UserName[33];//1.7.4 vpn帐号长度改为32
    char referedMsg[100];
    const char *ErrMsg1 = NULL;
#endif
#if 0
    char *pptpList, *newList, *p1, *p2, cmdstr[128];
#endif
#if 0
    int tempLen;
#endif

#if 0
    DBGPRINT("********formConfigPptpClientConfig********\n");
#endif
    a_assert(wp);
#if 0
    pptpList = nvram_bufget(RT2860_NVRAM,"PptpClients");
#endif
    action = websGetVar(wp, "Action", T(""));
    PPTPClientEnable = websGetVar(wp, "PPTPClientEnable", T(""));
    if(strcmp(PPTPClientEnable, "on") != 0) {
        PPTPClientEnable="off";
    }
    PPTPClientNATEnable = websGetVar(wp, "PPTPClientNATEnable", T(""));
    if(strcmp(PPTPClientNATEnable, "on") != 0) {
        PPTPClientNATEnable="off";
    }
    chnlName = websGetVar(wp, "setName", T(""));
    userName = websGetVar(wp, "userName", T(""));
    passwd = websGetVar(wp, "passwd", T(""));
#if 0
    if(p1=strchr(passwd, UTT_SPLIT_SIGN_RECORD)) {
        *p1=(char)0x1FU;
    }
#endif
    AuthType = websGetVar(wp, "AuthType", T(""));
    serverLanIp = websGetVar(wp, "serverLanIp", T(""));
    serverLanNetMask = websGetVar(wp, "serverLanNetMask", T(""));
#if 0
    if(ip2int(serverLanIp) & ~ip2int(serverLanNetMask) != 0U ) {
            setTheErrorMsg(T("IP地址网段和掩码配置不一致"));
            goto OUT;
    }
#else
#if 0
    unsigned int sLanIp=0U;
    char serverLanNet[16];
    sLanIp=ip2int(serverLanIp) & ip2int(serverLanNetMask);
    int2ip(sLanIp, serverLanNet);
    strcpy(serverLanIp, serverLanNet);
#endif
#endif
    serverIp = websGetVar(wp, "serverIp", T(""));
    editName = websGetVar(wp, "oldSetName", T(""));
    mtu = websGetVar(wp, "MTU", T("1478"));
#if (VPN_PROXY == FYES)
    vpnMode = websGetVar(wp, "vpnMode", T(""));
#endif
#if (PPTP_ENCRYPT_MODE == FYES)
    EncryptionMode = websGetVar(wp, T("EncryptionMode"), NULL);
#endif

#if (VPN_PROXY == FYES)
    PPTP_PRINTF("%s:chnlName=%s,enable=%s,user=%s,passwd=%s,authType=%s,lanIp=%s,lanNm=%s,\
	    serverIp=%s,action=%s,editName=%s, EncryptionMode = %s, vpnMode = %s\n", __FUNCTION__, chnlName, PPTPClientEnable,\
	    userName, passwd, AuthType, serverLanIp, serverLanNetMask, serverIp, action,\
	    editName, EncryptionMode, vpnMode);
#elif (PPTP_ENCRYPT_MODE == FYES)
    PPTP_PRINTF("%s:chnlName=%s,enable=%s,user=%s,passwd=%s,authType=%s,lanIp=%s,lanNm=%s,\
	    serverIp=%s,action=%s,editName=%s, EncryptionMode = %s\n", __FUNCTION__, chnlName, PPTPClientEnable,\
	    userName, passwd, AuthType, serverLanIp, serverLanNetMask, serverIp, action,\
	    editName, EncryptionMode);
#else
    PPTP_PRINTF("%s:chnlName=%s,enable=%s,user=%s,passwd=%s,authType=%s,lanIp=%s,lanNm=%s,\
	    serverIp=%s,action=%s,editName=%s\n", __FUNCTION__, chnlName, PPTPClientEnable,\
	    userName, passwd, AuthType, serverLanIp, serverLanNetMask, serverIp, action,\
	    editName);
#endif
    /* 参数判断 */
#if (PPTP_ENCRYPT_MODE == FYES)
    if ((chnlName == NULL) || (PPTPClientEnable == NULL) || (userName == NULL) || \
	    (passwd == NULL) || (AuthType == NULL) || (serverLanIp == NULL) || \
	    (serverLanNetMask == NULL) || (serverIp == NULL) || (action == NULL) || (editName == NULL) || (EncryptionMode == NULL)) {
#else
    if ((chnlName == NULL) || (PPTPClientEnable == NULL) || (userName == NULL) || \
	    (passwd == NULL) || (AuthType == NULL) || (serverLanIp == NULL) || \
	    (serverLanNetMask == NULL) || (serverIp == NULL) || (action == NULL) || (editName == NULL)) {
#endif
		ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_PARAMETER_ERR);
	setTheErrorMsg(ErrMsg);
	goto out;
    }
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


#if 0
    DBGPRINT("pptpList before change:%s\n", pptpList);
    DBGPRINT("new record:chnlName[%s],PPTPClientEnable[%s], userName[%s],passwd[%s],AuthType[%s],serverLanIp[%s],serverLanNetMask[%s],serverIp[%s]\n",
            chnlName,PPTPClientEnable,userName,passwd,AuthType,serverLanIp,serverLanNetMask,serverIp);
    tempLen=strlen(pptpList)+strlen(chnlName)+strlen(PPTPClientEnable)+strlen(userName)+strlen(passwd)+strlen(AuthType)+60;

    DBGPRINT("Action[%s]\n", action);
#endif
    if (strcmp(action, "add") == 0) {
#if (VPN_ITEM_CONTROL == FYES)
	if(checkVpnFreeItems() == 0) {
	    ErrMsg = getTransDataForC(C_LANG_INDEX_MAX_VPN_ITEM);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
#endif
	/*判断用户名是否重复*/
	if(ProfGetInstPointByValue(mibType, OFFANDSIZE(PPTPCliProfile, userName), TSTR, userName, -1) != PROFFAIL)
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
	if(ProfGetInstPointByValue(mibType, OFFANDSIZE(PPTPCliProfile, userName), TSTR, userName, editIndex) != PROFFAIL)
	{
	    ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_ALREADY_EXITS);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
#if (STATIC_ROUTE_BIND_PPTP == FYES)
	if(strcmp(chnlName, editName) != 0)
	{
	    snprintf(UserName, sizeof(UserName), "(PPTPC)%s", editName);
	    PPTP_PRINTF("%s-%d, UserName = %s\n", __func__, __LINE__, UserName);
	    if(isUserInStaticRoute(UserName) == 1)
	    {/*判断此隧道名否被静态路由引用*/ 
		ErrMsg1 = getTransDataForC(C_LANG_INDEX_USER);
		ErrMsg = getTransDataForC(C_LANG_INDEX_USER_IS_USE_IN_STATIC_ROUTE);
		snprintf(referedMsg, sizeof(referedMsg), "%s%s%s", ErrMsg1, editName, ErrMsg);
		setTheErrorMsg((const char *)referedMsg);
		goto out;
	    }
	}
#endif
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
    prof = (PPTPCliProfile *)ProfGetInstPointByIndex(mibType, instIndex);

    ProfSetNameByPoint(prof, chnlName);
    if (strcmp(PPTPClientEnable, "on") == 0) {
	prof->head.active = 1;
    } else {
	prof->head.active = 0;
    }
    if (strcmp(PPTPClientNATEnable, "on") == 0) {
	prof->Natenable = 1;
    } else {
	prof->Natenable = 0;
    }
    strncpy(prof->userName, userName, 32);
    strncpy(prof->passwd, passwd, UTT_PASSWD_LEN);
    prof->pppAuthMode = mode;
    prof->remoteLanIp = addr_ip.s_addr & addr_mask.s_addr;
    /* mantis 8180 */
    prof->remoteLanIpDis = addr_ip.s_addr;
    prof->remoteLanNetmask = addr_mask.s_addr;
    strncpy(prof->serverHost, serverIp, UTT_HOST_LEN);
#if (PPTP_ENCRYPT_MODE == FYES)
    strcpy(prof->EncryptionMode, EncryptionMode);
#endif
    prof->mtu = strtol(mtu, NULL, 10);
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
    websRedirect(wp, "PPTPClientList.asp");
#endif
    return;
}
/* read a line (not more than 127 bytes) from filename to buf */
char* read_file(char *filename, char *buf)
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

#if 0
static int aspGetPptpClientConfig(int eid, webs_t wp, int argc, char_t **argv)
{
    char *p1;
    char_t *PptpClientList, *tmpRecord, fileName[64], buf[128];
    char_t ChnlName[32],Enable[4],UsrName[32],PassWord[32],AuthType[8],ChnlLanIp[32],ChnlLanNm[32],ChnlSerIp[32];
    int i = 0, n=0;

    char_t *PPTPClientEnables=T("PPTPClientEnables[%d]=\"%s\";\n");
    char_t *setNames=T("setNames[%d]=\"%s\";\n");
    char_t *userNames=T("userNames[%d]=\"%s\";\n");
    char_t *serverLanIps=T("serverLanIps[%d]=\"%s\";\n");
    char_t *serverLanNetMasks=T("serverLanNetMasks[%d]=\"%s\";\n");
    char_t *statuss=T("statuss[%d]=\"%s\";\n");
    char_t *usetimes=T("usetimes[%d]=\"%s\";\n");
    char_t *outtimes=T("outtimes[%d]=\"%s\";\n");
    char_t *outboundss=T("outboundss[%d]=\"%s\";\n");
    char_t *inboundss=T("inboundss[%d]=\"%s\";\n");
    websWrite(wp, "var PPTPClientEnables = new Array();");
    websWrite(wp, "var setNames = new Array();");
    websWrite(wp, "var userNames = new Array();");
    websWrite(wp, "var serverLanIps = new Array();");
    websWrite(wp, "var serverLanNetMasks = new Array();");
    websWrite(wp, "var statuss=new Array();");
    websWrite(wp, "var usetimes = new Array();");
    websWrite(wp, "var outtimes = new Array();");
    websWrite(wp, "var outboundss = new Array();");
    websWrite(wp, "var inboundss = new Array();");

    PptpClientList=nvram_bufget(RT2860_NVRAM,"PptpClients");
    tmpRecord=PptpClientList;
    while(*tmpRecord) {
        n = sscanf(tmpRecord,"%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,]%[^;]",
                ChnlName,Enable,UsrName,PassWord,AuthType,ChnlLanIp,ChnlLanNm,ChnlSerIp);
        if ( n == 8 ) {
            if(p1=strchr(PassWord, 0x1FU)){
                *p1=UTT_SPLIT_SIGN_RECORD;
            }
            websWrite(wp, PPTPClientEnables, i, Enable);
            websWrite(wp, setNames, i, ChnlName);
            websWrite(wp, userNames, i, UsrName);
            websWrite(wp, serverLanIps, i, ChnlLanIp);
            websWrite(wp, serverLanNetMasks, i, ChnlLanNm);
            sprintf(fileName, "/var/run/pptp_%slink.status", ChnlName);
            websWrite(wp, statuss, i, read_file(fileName, buf));
            if (strcmp(Enable,"on") || strcmp(buf, "1") ) {
                websWrite(wp, usetimes, i, "-");
                websWrite(wp, outtimes, i, "-");
                websWrite(wp, outboundss, i, "-");
                websWrite(wp, inboundss, i, "-");
            } else {
                unsigned long begin, now;
                char *p;
                sprintf(fileName, "/var/run/pptp_%s.uptime", ChnlName);
                read_file(fileName, buf);
                p=strchr(buf,'.');
                *p='\0';
                begin=atol(buf);
                DBGPRINT("begin time[%ld]\n",begin);
                read_file("/proc/uptime", buf);
                p=strchr(buf,'.');
                *p='\0';
                now=atol(buf);
                DBGPRINT("now time[%ld]\n",now);
                sprintf(buf, "%ld", now-begin);
                websWrite(wp, usetimes, i, buf);
                websWrite(wp, outtimes, i, "0");
                long long txByte, rxByte;
                sprintf(fileName, "/var/run/pptp_%s.ppp", ChnlName);
                read_file(fileName, buf);
                if(p=strchr(buf,' '))
                    *p='\0';
                DBGPRINT("ppp interface[%s]\n",buf);
                txByte=getIfStatistic(buf, TXBYTE);       //send bytes
                rxByte=getIfStatistic(buf, RXBYTE);       //receive bytes
                sprintf(buf, "%lld", txByte);
                DBGPRINT("ppp outboundss[%s]\n",buf);
                websWrite(wp, outboundss, i, buf);
                sprintf(buf, "%lld", rxByte);
                DBGPRINT("ppp inboundss[%s]\n",buf);
                websWrite(wp, inboundss, i, buf);
            }
            i++;
        }
        tmpRecord=NextRecordStart(tmpRecord);
    }
    websWrite(wp, "var totalrecs=%d;", i);
    websWrite(wp, "var max_totalrecs=%d;", MAX_PPTP_NUM);
    /*getNetConfig(eid, wp, argc, argv); */
    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}
#endif
/*
 * 列表页面输出pptp client信息
 */
static int aspGetPptpClientConfig(int eid, webs_t wp, int argc, char_t **argv) {
    MibProfileType mibType = MIB_PROF_PPTP_CLI;
    PPTPCliProfile *prof = NULL;
    int min = 0, max = 0, i = 0, totalrecs = 0;
    char fileName[64] = {0}, buf[128] = {0};

    char_t *PPTPClientEnables=T("PPTPClientEnables[%d]=\"%s\";\n");
    char_t *setNames=T("setNames[%d]=\"%s\";\n");
    char_t *userNames=T("userNames[%d]=\"%s\";\n");
    char_t *serverGwIps = T("serverGwIps[%d] = \"%s\";\n");
    char_t *serverLanIps=T("serverLanIps[%d]=\"%N\";\n");
    char_t *serverLanNetMasks=T("serverLanNetMasks[%d]=\"%N\";\n");
    char_t *statuss=T("statuss[%d]=\"%s\";\n");
    char_t *usetimes=T("usetimes[%d]=\"%s\";\n");
    char_t *outtimes=T("outtimes[%d]=\"%s\";\n");
    char_t *outboundss=T("outboundss[%d]=\"%s\";\n");
    char_t *inboundss=T("inboundss[%d]=\"%s\";\n");
    websWrite(wp, "var PPTPClientEnables = new Array();");
    websWrite(wp, "var setNames = new Array();");
    websWrite(wp, "var userNames = new Array();");
    websWrite(wp, "var serverGwIps = new Array();");
    websWrite(wp, "var serverLanIps = new Array();");
    websWrite(wp, "var serverLanNetMasks = new Array();");
    websWrite(wp, "var statuss=new Array();");
    websWrite(wp, "var usetimes = new Array();");
    websWrite(wp, "var outtimes = new Array();");
    websWrite(wp, "var outboundss = new Array();");
    websWrite(wp, "var inboundss = new Array();");
#if(CONFIG_USER_GOAHEAD_LANG_EN == FYES)
	 websWrite(wp, "var SmartModevpnModes=\"%d\"; ",0);
#else
	 websWrite(wp, "var SmartModevpnModes=\"%d\"; ",1);
#endif

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) {
	prof = (PPTPCliProfile *)ProfGetInstPointByIndex(mibType, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
	    /*
	     * 输出配置信息
	     */
	    if (prof->head.active == 1) {
		websWrite(wp, PPTPClientEnables, totalrecs, "on");
	    } else {
		websWrite(wp, PPTPClientEnables, totalrecs, "off");
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
            sprintf(fileName, "/var/run/pptp_%slink.status", prof->head.name);
      //      websWrite(wp, statuss, totalrecs, read_file(fileName, buf));
	     if (strcmp(read_file(fileName, buf), "0") == 0 && prof->connectStatus == 1)
                websWrite(wp, statuss, totalrecs, "-1");
            else 
                websWrite(wp, statuss, totalrecs, read_file(fileName, buf));
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
                sprintf(fileName, "/var/run/pptp_%s.uptime", prof->head.name);
                read_file(fileName, buf);
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
                websWrite(wp, usetimes, totalrecs, buf);
                websWrite(wp, outtimes, totalrecs, "0");
		/* 获得下载，上传流量 */
                sprintf(fileName, "/var/run/pptp_%s.ppp", prof->head.name);
                read_file(fileName, buf);
                if((p=strchr(buf,' ')) != NULL)
                    *p='\0';
                txByte=getIfStatistic(buf, TXBYTE);       //send bytes
                rxByte=getIfStatistic(buf, RXBYTE);       //receive bytes
                sprintf(buf, "%lld", txByte);
                websWrite(wp, outboundss, totalrecs, buf);
                sprintf(buf, "%lld", rxByte);
                websWrite(wp, inboundss, totalrecs, buf);
	    }
	    totalrecs++;
	}
    }
    websWrite(wp, "var totalrecs=%d;", totalrecs);
    websWrite(wp, "var max_totalrecs=%d;", max);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

#if 0
static int aspGetPptpClientOneConfig(int eid, webs_t wp, int argc, char_t **argv)
{
    char *tmpRecord, *EditName, *PptpClientList, *p1;
    char_t ChnlName[32],Enable[4],UsrName[32],PassWord[32],AuthType[8],ChnlLanIp[32],ChnlLanNm[32],ChnlSerIp[32];

    if(ejArgs(argc,argv,T("%s"), &EditName) <1) {
        printf("dad %s\n",EditName);
	setTheErrorMsg(T("读取edit_Name变量失败!"));
    } else {
        DBGPRINT("EditName[%s]\n", EditName);
        PptpClientList=nvram_bufget(RT2860_NVRAM,"PptpClients");
        tmpRecord=PptpClientList;
        while(*tmpRecord) {
            sscanf(tmpRecord,"%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^;]",
                    ChnlName,Enable,UsrName,PassWord,AuthType,ChnlLanIp,ChnlLanNm,ChnlSerIp);
            if( (strcmp(ChnlName, EditName) == 0) ) {
                if(p1=strchr(PassWord, 0x1FU)){
                    *p1=UTT_SPLIT_SIGN_RECORD;
                }
                websWrite(wp, "var PPTPClientEnables=\"%s\"; ", Enable);
                websWrite(wp, "var setNames=\"%s\"; ", ChnlName);
                websWrite(wp, "var userNames=\"%s\"; ", UsrName);
                websWrite(wp, "var passwds=\"%s\"; ", PassWord);
                websWrite(wp, "var AuthTypes=\"%s\"; ", AuthType);
                websWrite(wp, "var serverLanIps=\"%s\"; ", ChnlLanIp);
                websWrite(wp, "var serverLanNetMasks=\"%s\"; ", ChnlLanNm);
                websWrite(wp, "var serverIps=\"%s\"; ", ChnlSerIp);
                break;
            }
            tmpRecord=NextRecordStart(tmpRecord);
        }
        websWrite(wp, "var totalrecs=1;");
        websWrite(wp, "var max_totalrecs=%d;", MAX_PPTP_NUM);
    }
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
#endif
/*
 * 配置页面输出信息
 */
static int aspGetPptpClientOneConfig(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_PPTP_CLI;
    PPTPCliProfile *prof = NULL;

    char *EditName = NULL;
    struct in_addr ip, nm;
	const char *ErrMsg = NULL;
#if (VPN_PROXY == FYES)
    char *p;
    char buf[128] = {0};
    char fileName[64] = {0};
    unsigned long begin, now;
    websWrite(wp, "var useTimes=\"\"; ");
    websWrite(wp, "var vpnModes=\"\"; ");
#endif
#if 0
    char *tmpRecord, *EditName, *PptpClientList, *p1;
    char_t ChnlName[32],Enable[4],UsrName[32],PassWord[32],AuthType[8],ChnlLanIp[32],ChnlLanNm[32],ChnlSerIp[32];
#endif
    websWrite(wp, "var PPTPClientEnables=\"\"; ");
    websWrite(wp, "var PPTPClientNATEnables=\"\"; ");
    websWrite(wp, "var setNames=\"\"; ");
    websWrite(wp, "var userNames=\"\"; ");
    websWrite(wp, "var passwds=\"\"; ");
    websWrite(wp, "var AuthTypes=\"\"; ");
    websWrite(wp, "var serverLanIps=\"\"; ");
    websWrite(wp, "var serverLanNetMasks=\"\"; ");
    websWrite(wp, "var serverIps=\"\"; ");
#if (PPTP_ENCRYPT_MODE == FYES)
    websWrite(wp, "var EncryptionModes=\"\"; ");
#endif
    websWrite(wp, "var mtu=1478; ");

    if(ejArgs(argc,argv,T("%s"), &EditName) <1) {
		PPTP_PRINTF("%s: EditName = %s\n", __FUNCTION__, EditName);
		ErrMsg = getTransDataForC(C_LANG_INDEX_READ_EDIT_NAME_VAL_ERR);
		setTheErrorMsg(ErrMsg);
    } else {
#if 0
        DBGPRINT("EditName[%s]\n", EditName);
        PptpClientList=nvram_bufget(RT2860_NVRAM,"PptpClients");
        tmpRecord=PptpClientList;
        while(*tmpRecord) {
            sscanf(tmpRecord,"%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^;]",
                    ChnlName,Enable,UsrName,PassWord,AuthType,ChnlLanIp,ChnlLanNm,ChnlSerIp);
            if( (strcmp(ChnlName, EditName) == 0) ) {
                if(p1=strchr(PassWord, 0x1FU)){
                    *p1=UTT_SPLIT_SIGN_RECORD;
                }
                websWrite(wp, "var PPTPClientEnables=\"%s\"; ", Enable);
                websWrite(wp, "var setNames=\"%s\"; ", ChnlName);
                websWrite(wp, "var userNames=\"%s\"; ", UsrName);
                websWrite(wp, "var passwds=\"%s\"; ", PassWord);
                websWrite(wp, "var AuthTypes=\"%s\"; ", AuthType);
                websWrite(wp, "var serverLanIps=\"%s\"; ", ChnlLanIp);
                websWrite(wp, "var serverLanNetMasks=\"%s\"; ", ChnlLanNm);
                websWrite(wp, "var serverIps=\"%s\"; ", ChnlSerIp);
                break;
            }
            tmpRecord=NextRecordStart(tmpRecord);

        }
        websWrite(wp, "var totalrecs=1;");
        websWrite(wp, "var max_totalrecs=%d;", MAX_PPTP_NUM);
#endif
	prof = (PPTPCliProfile *)ProfGetInstPointByName(mibType, EditName);
	if (prof == NULL) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_READ_CONFIG_FAIL);
	    setTheErrorMsg(ErrMsg);
	} else {
	    if (prof->head.active == 0) {
		websWrite(wp, "var PPTPClientEnables=\"%s\"; ", "off");
	    } else {
		websWrite(wp, "var PPTPClientEnables=\"%s\"; ", "on");
#if (VPN_PROXY == FYES)
		sprintf(fileName, "/var/run/pptp_%slink.status", prof->head.name);
		read_file(fileName, buf);
		if(strcmp(buf, "1") == 0)
		{
		    memset(fileName, 0, sizeof(fileName));
		    memset(buf, 0, sizeof(buf));
		    /* 读pptp建立连接的时间 */
		    sprintf(fileName, "/var/run/pptp_%s.uptime", prof->head.name);
		    read_file(fileName, buf);
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
	    if (prof->Natenable == 0) {
		websWrite(wp, "var PPTPClientNATEnables=\"%s\"; ", "off");
	    } else {
		websWrite(wp, "var PPTPClientNATEnables=\"%s\"; ", "on");
	    }
            websWrite(wp, "var setNames=\"%s\"; ", prof->head.name);
            websWrite(wp, "var userNames=\"%s\"; ", prof->userName);
            websWrite(wp, "var passwds=\"%s\"; ", prof->passwd);
            websWrite(wp, "var AuthTypes=\"%s\"; ", getEnumString(pppAuthEnum, prof->pppAuthMode));
#if 0
	    ip.s_addr = prof->remoteLanIp;
#else
	    /* mantis bug 8180 */
	    ip.s_addr = prof->remoteLanIpDis;
#endif
            websWrite(wp, "var serverLanIps=\"%s\"; ", inet_ntoa(ip));
	    nm.s_addr = prof->remoteLanNetmask;
            websWrite(wp, "var serverLanNetMasks=\"%s\"; ", inet_ntoa(nm));
            websWrite(wp, "var serverIps=\"%s\"; ", prof->serverHost);
#if (PPTP_ENCRYPT_MODE == FYES)
	    websWrite(wp, "var EncryptionModes = \"%s\";", prof->EncryptionMode);
#endif
	    websWrite(wp, "var mtu = %d;", prof->mtu);
#if (VPN_PROXY == FYES)
	    websWrite(wp, "var vpnModes = %d;", prof->vpnMode);
#endif
	}
    }
    getTheErrorMsg(eid, wp, argc, argv);
#if (VPN_PROXY == FYES)
    getTheRightMsg(eid, wp, argc, argv);
#endif
    return 0;
}

#if 0
/*
 * pptp server和pptp client实例删除放到同一个函数 formPptpListDelAll 在src/wusPptpServer.c中
 */
static void formPptpClientDel(webs_t wp, char_t *path, char_t *query)
{
    char *tmp,*pptpList,*delstr,*p1,*p2;
    DBGPRINT("**********formPptpClientDel**********\n");
    a_assert(wp);
    tmp = nvram_bufget(RT2860_NVRAM,"PptpClients");
    pptpList =(char *)malloc(strlen(tmp)+1);
    strcpy(pptpList,tmp);
    DBGPRINT("before del pptpList[%s]\n", pptpList);
    delstr = websGetVar(wp,T("delstr"),T(""));
    if(strlen(delstr) <= 0) {
        goto out;
    }
    DBGPRINT("delstr[%s]\n", delstr);
    if(!strcmp(delstr,T(""))){
        setTheErrorMsg(T("找不到此隧道名"));
        goto out;
    }
    tmp=strtok(delstr,UTT_SPLIT_SIGN_STR);
    while(tmp!=NULL) {
        DBGPRINT("del channel[%s]\n", tmp);
        doSystem("pptp-down.sh pptp_%s ", tmp);
        p1=strnmatch(pptpList,tmp,0);
        if(!p1) {
            setTheErrorMsg(T("找不到该隧道"));
            goto out;
        }
        p2=p1;
        /*sendMsgForSpeed*/
        while( (*p2!=UTT_SPLIT_SIGN_RECORD) && *p2) {
            p2++;
        }//*p2=';'or '\0'
        if(p1!=pptpList) p1--;   //before p1 there is the char';'
        else if(*p2) p2++;    //if del the first record ,jump over the char ';'
        DBGPRINT("before strcat:p1[%s],p2[%s]\n", p1, p2);
        *p1=0;
        strcat(pptpList,p2); 
        tmp=strtok(NULL,UTT_SPLIT_SIGN_STR);
    }
    DBGPRINT("nvram_commit:PptpClients[%s]\n", pptpList);
    nvram_bufset(RT2860_NVRAM,"PptpClients",pptpList);
    nvram_commit(RT2860_NVRAM);
out:
    free(pptpList);
    websRedirect(wp,"PPTPClientList.asp");
    return;
}
static void formPptpClientDial(webs_t wp, char_t *path, char_t *query)
{
    char *tmp,*dialstr, *pptplist, pptpinfo[128]={0}, *p1,*serverIp;
    char fileName[64], buf[128], cmdstr[128];
    DBGPRINT("**********formPptpClientDial**********\n");
    a_assert(wp);
    dialstr = websGetVar(wp,T("dialstr"),T(""));
    DBGPRINT("dialstr[%s]\n", dialstr);
    if(!strcmp(dialstr,T(""))){
        setTheErrorMsg(T("找不到此隧道名"));
        goto out;
    }
    pptplist=nvram_bufget(RT2860_NVRAM,"PptpClients");
    tmp=strtok(dialstr,UTT_SPLIT_SIGN_STR);
    while(tmp!=NULL) {
        DBGPRINT("dialstr channel[%s]\n", tmp);
        sprintf(fileName, "/var/run/pptp_%slink.status", tmp);
        read_file(fileName, buf);
        if(!strcmp(buf, "1")) {
            goto out;
        }
#if 1
        strncpy(pptpinfo,strnmatch(pptplist, tmp, 0),127);
        if(p1=strchr(pptpinfo, UTT_SPLIT_SIGN_RECORD))
            *p1='\0';
        DBGPRINT("pptpinfo[%s]\n", pptpinfo);
        serverIp=strrchr(pptpinfo, UTT_SPLIT_SIGN_CHAR)+1;
        DBGPRINT("serverIp[%s]\n", serverIp);
#endif
        sprintf(cmdstr, "`ps |grep \"pppd file /etc/options.pptp_%s $\"|sed \"s/^ *//g\"|sed \"s/ .*$//g\"`", tmp);
        doSystem("kill -9 %s >/dev/null 2>&1", cmdstr);
        DBGPRINT("cmdstr[%s]\n", cmdstr);
        doSystem("ip route del %s scope link", serverIp);
        doSystem("pppd file /etc/options.pptp_%s &", tmp);
        tmp=strtok(NULL,UTT_SPLIT_SIGN_STR);
    }
out:
    websRedirect(wp,"PPTPClientList.asp");
    return;
}
#endif
/*
 * 列表拨号
 */
static void formPptpClientDial(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_PPTP_CLI;
    PPTPCliProfile *prof = NULL;

    char *tmp,*dialstr; 
	const char *ErrMsg = NULL;
#if 0
    *pptplist, pptpinfo[128]={0}, *p1,*serverIp;
#endif
    char fileName[64], buf[128], cmdstr[128];
#if 0
    DBGPRINT("**********formPptpClientDial**********\n");
#endif
    a_assert(wp);
    dialstr = websGetVar(wp,T("dialstr"),T(""));
#if 0
    DBGPRINT("dialstr[%s]\n", dialstr);
#endif
    PPTP_PRINTF("%s, dailstr = %s\n", __FUNCTION__, dialstr);

    if(!strcmp(dialstr,T(""))){
		ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_CANT_TUNNEL_ERR);
        setTheErrorMsg(ErrMsg);
        goto out;
    }
#if 0
    pptplist=nvram_bufget(RT2860_NVRAM,"PptpClients");
#endif
    tmp=strtok(dialstr,UTT_SPLIT_SIGN_STR);
    while(tmp!=NULL) {
	PPTP_PRINTF("%s: tmp = %s\n", __FUNCTION__, tmp);
	prof = (PPTPCliProfile *)ProfGetInstPointByName(mibType, tmp);
	if ((prof == NULL) || (prof->head.active == 0)) { /* 未启用 */
#if 0
	    goto out;
#endif
	    tmp = strtok(NULL, UTT_SPLIT_SIGN_STR);
	    continue;
	}
	 if (prof->connectStatus == 1)
        {    
            /* 已在页面上点击建立 */
            tmp = strtok(NULL, UTT_SPLIT_SIGN_STR);
            continue;
        }    

	if (prof->connectStatus == 1)
	{
	    /* 已在页面上点击建立 */
	    tmp = strtok(NULL, UTT_SPLIT_SIGN_STR);
	    continue;
	}

#if 0
        DBGPRINT("dialstr channel[%s]\n", tmp);
#endif
        sprintf(fileName, "/var/run/pptp_%slink.status", tmp);
        read_file(fileName, buf);
        if(strcmp(buf, "1") == 0) { /* 已连接 */
#if 0
            goto out;
#endif
	    tmp = strtok(NULL, UTT_SPLIT_SIGN_STR);
	    continue;
        }
#if(VPN_CONTROL == FYES)
	if(nvramProfile->vpnProfile.count<=0)
	{
	    ErrMsg = getTransDataForC(C_LANG_INDEX_MAX_VPN_COUNT);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
#endif
#if 0
        strncpy(pptpinfo,strnmatch(pptplist, tmp, 0),127);
        if(p1=strchr(pptpinfo, UTT_SPLIT_SIGN_RECORD))
            *p1='\0';
        DBGPRINT("pptpinfo[%s]\n", pptpinfo);
        serverIp=strrchr(pptpinfo, UTT_SPLIT_SIGN_CHAR)+1;
        DBGPRINT("serverIp[%s]\n", serverIp);
#endif
        sprintf(cmdstr, "`ps |grep \"pppd file /etc/options.pptp_%s $\"|sed \"s/^ *//g\"|sed \"s/ .*$//g\"`", tmp);
        doSystem("kill -9 %s >/dev/null 2>&1", cmdstr);
	PPTP_PRINTF("%s: cmdstr = %s\n", __FUNCTION__, cmdstr);
        doSystem("ip route del %s scope link", prof->serverHost);
        doSystem("pppd file /etc/options.pptp_%s &", tmp);
        tmp=strtok(NULL,UTT_SPLIT_SIGN_STR);
	prof->connectStatus = 1;
     }
out:
    sleep(3);
#if (VPN_PROXY == FYES)
    websRedirect(wp,"VPNSui.asp");
#else
    websRedirect(wp,"PPTPClientList.asp");
#endif
    return;
}
#if 0
static void formPptpClientHang(webs_t wp, char_t *path, char_t *query)
{
    char *tmp,*hangstr;
    DBGPRINT("**********formPptpClientHang**********\n");
    a_assert(wp);
    hangstr = websGetVar(wp,T("hangstr"),T(""));
    DBGPRINT("hangstr[%s]\n", hangstr);
    if(strcmp(hangstr,T(""))==0){
        setTheErrorMsg(T("找不到此隧道名"));
        goto out;
    }
    tmp=strtok(hangstr,UTT_SPLIT_SIGN_STR);
    while(tmp!=NULL) {
        DBGPRINT("Hang channel[%s]\n", tmp);
        doSystem("pptp-down.sh pptp_%s ", tmp);
        tmp=strtok(NULL,UTT_SPLIT_SIGN_STR);
    }
out:
    websRedirect(wp,"PPTPClientList.asp");
    return;
}
#endif
/*
 * 删除指定用户pptp拨号
 */
static void killOnePptpClient(char *user)
{
	PPTP_PRINTF("%s: user=%s\n", __FUNCTION__, user);
    int fd = -1;
    struct st_uttPppdClient info;
    int st_size = sizeof(struct st_uttPppdClient);
    fd = open(UTT_PPTP_CLIENT_INFO_FILE, O_RDONLY);
    if(fd != -1) {
        while(read(fd, &info, st_size) > 0) {
        PPTP_PRINTF("%s: user=%s,pid=%d,STime=%d,LTime=%d,ConTime=%d,rx=%lld,tx=%lld\n",
            __FUNCTION__, info.user, info.pid, info.STime,info.LTime,info.ConTime,
            info.rxByte,info.txByte); 
        PPTP_PRINTF("mac=%s, ip=%s, ifname=%s, devie=%s\n", info.mac, info.ip,
            info.ifname,info.device);
            if(strncmp(user, info.user, UTT_PPP_CLIENT_NAME_LEN) == 0) {
                doSystem("kill -INT %d", info.pid);
#if 0
/*挂断pptp服务端时，并发数加一在pptpd中处理*/
#if (VPN_CONTROL == FYES)
		doSystem("vpnHandle plus ");
#endif
#endif
        break;
            }
        }
    close(fd);
    }
   return;

}

/*
 * 列表挂断
 */
static void formPptpClientHang(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_PPTP_CLI;
    PPTPCliProfile *prof = NULL;
    char *tmp,*tmpsrv,*hangstr,*hangstrsrv;
    char fileName[64] = {0}, buf[10] = {0};
	const char *ErrMsg = NULL;
#if 0
    DBGPRINT("**********formPptpClientHang**********\n");
#endif
    sleep(3);
    a_assert(wp);
    hangstr = websGetVar(wp,T("hangstr"),T(""));
    hangstrsrv = websGetVar(wp,T("hangstrsrv"),T(""));
#if 0
    DBGPRINT("hangstr[%s]\n", hangstr);
#endif
    if((strcmp(hangstr,T(""))==0)&&(strcmp(hangstrsrv,T(""))==0)){
		ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_CANT_TUNNEL_ERR);
        setTheErrorMsg(ErrMsg);
        goto out;
    }
    tmp=strtok(hangstr,UTT_SPLIT_SIGN_STR);
    while(tmp!=NULL) {
	/* mantis 8921: 已断开不再挂断 */
	sprintf(fileName, "/var/run/pptp_%slink.status", tmp);
        read_file(fileName, buf);
//	if (strcmp(buf, "0") == 0) { /* 已断开 */
	prof = (PPTPCliProfile *)ProfGetInstPointByName(mibType, tmp);
        if (strcmp(buf, "0") == 0 && prof && prof->connectStatus == 0) { /* 已断开 */
	    tmp = strtok(NULL, UTT_SPLIT_SIGN_STR);
	    continue;
	}
	if (prof && prof->connectStatus == 1)
            prof->connectStatus = 0;

	if (prof && prof->connectStatus == 1)
	    prof->connectStatus = 0;
#if 0
        DBGPRINT("Hang channel[%s]\n", tmp);
#endif
	PPTP_PRINTF("%s: tmp = %s\n", __FUNCTION__, tmp);
        doSystem("pptp-down.sh pptp_%s ", tmp);
        tmp=strtok(NULL,UTT_SPLIT_SIGN_STR);
    }

	tmpsrv = strtok(hangstrsrv,UTT_SPLIT_SIGN_STR);
	while (tmpsrv!=NULL) {
	uttSemP(SEM_PPPOE_NO,0);
	/*挂断此账号*/
	killOnePptpClient(tmpsrv);
	
	uttSemV(SEM_PPPOE_NO,0);
	tmpsrv=strtok(NULL,UTT_SPLIT_SIGN_STR);
	}


out:
#if (VPN_PROXY == FYES)
    sleep(2);
    websRedirect(wp,"VPNSui.asp");
#else
    websRedirect(wp,"PPTPClientList.asp");
#endif
    return;
}
/********************************************************************
 * 函数： formPPTPCliAccAllow
 * 功能： 列表中的允许
 * 参数： 无
 * 输出： 无
 * 返回： 无
 * 作者： Jianqing.Sun
 * 创建： 2013-03-08
 *******************************************************************/
static void formPPTPCliAccAllow(webs_t wp, char *path, char *query)
{
    UINT32 Allow;
    char_t *AllowID = NULL;
    struProfAlloc *profhead = NULL;
    MibProfileType mibType = MIB_PROF_PPTP_CLI;
    ProfChangeType profAction;
    PPTPCliProfile *profA = NULL;
    int instIndex;
    const char *ErrMsg = NULL;

    Allow = (UINT32)strtoul(websGetVar(wp, T("Allow"), NULL),NULL,0);
    AllowID = websGetVar(wp, T("AllowID"), NULL);

    if (strlen(AllowID) == 0)
    {
	/* check variable */
	ErrMsg = getTransDataForC(C_LANG_INDEX_PLEASESELECT_USER);
	setTheErrorMsg(ErrMsg);
	goto out;
    }

    profAction = PROF_CHANGE_EDIT;
    instIndex = ProfGetInstPointByValue(mibType, OFFANDSIZE(PPTPCliProfile, userName), TSTR, AllowID, -1);
    if(instIndex == PROFFAIL) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_DID_NOT_FIND_CASE);
	setTheErrorMsg(ErrMsg);
	goto out;
    }
    ProfBackupByIndex(mibType, profAction, instIndex, &profhead);
    profA = (PPTPCliProfile*)ProfGetInstPointByIndex(mibType, instIndex);

    profA->head.active = Allow;/*允许或禁止*/

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
    websRedirect(wp, "PPTPClientList.asp");/*页面跳转*/
}

#if (VPN_PROXY == FYES)
static void formPPTPConnStatus(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_PPTP_CLI;
    PPTPCliProfile *prof = NULL;
    char *p;
    char buf[128] = {0};
    char fileName[64] = {0};
    unsigned long begin, now;
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain;charset=GB2312\nPragma: no-cache\nCache-Control: no-cache\n\n"));
    prof = (PPTPCliProfile *)ProfGetInstPointByName(mibType, "VPN_PPTP");
    if (prof == NULL) {
	websWrite(wp, T("var connStatus=\"-\"; "));
    }
    else if(prof->head.active == 0)
    {
	websWrite(wp, T("var connStatus=\"-\"; "));
    }
    else
    {
	sprintf(fileName, "/var/run/pptp_%slink.status", prof->head.name);
	read_file(fileName, buf);
	if(strcmp(buf, "1") == 0)
	{
	    memset(fileName, 0, sizeof(fileName));
	    memset(buf, 0, sizeof(buf)); 
	    /* 读pptp建立连接的时间 */
	    sprintf(fileName, "/var/run/pptp_%s.uptime", prof->head.name);
	    read_file(fileName, buf);
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
 * fun for multi path
 * define asp and form fun
 */
void formDefinePptpClient(void) {
    websAspDefine(T("aspPptpClientConfig"), aspGetPptpClientConfig);
    websAspDefine(T("aspPptpClientOneConfig"), aspGetPptpClientOneConfig);
    websFormDefine(T("formPptpClientConfig"), formConfigPptpClientConfig);
#if 0
    websFormDefine(T("formDelPPTP"), formPptpClientDel);
#endif
    websFormDefine(T("formDia"), formPptpClientDial);
    websFormDefine(T("formhang"), formPptpClientHang);
    websFormDefine(T("formPPTPCliAccAllow"), formPPTPCliAccAllow);
#if (VPN_PROXY == FYES)
    websFormDefine(T("formPPTPConnStatus"), formPPTPConnStatus);
#endif
}
#endif
