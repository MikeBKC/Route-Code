#include "uttMachine.h"
#if (IP_SEC == FYES)
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <netinet/in.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <netdb.h>
#include <syslog.h>
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "argcmd.h"
#include "a2n.h"
#include "internet.h"
#include "utt_conf_define.h"
#if((PPTP_SERVER == FYES)||(PPTP_CLIENT == FYES))
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h> 
#include "utt_pppoeClient.h"
#include "StaticRoute.h"
#endif
//#define IPSEC_DEBUG


#ifdef IPSEC_DEBUG
#define IPSEC_PRINT(fmt, args...)		printf(fmt, ## args)
#else
#define IPSEC_PRINT(fmt, args...)
#endif

/*
 * /etc/ipsec.conf 中的配置
 */
extern char* getWanIfName(int num);
static void addOnePsk(char *left,char *right,char *key);
static void checkOneConfigSA(ipsecConfProfile *prof);
static void setAllConfigSA_status();
static void clearOneConfig(ipsecConfProfile *ipsecProf);
static void addAllConfig(void);
static void addOneConfig(ipsecConfProfile *ipsecProf);
static void clearConfigFile();
static int checkInstCount_ipsec();
static void insmod_modules_ipsec(void);
static void rmmod_modules_ipsec(void);
static void startUpDomainIP(int flag);


#if 0
static void getIPfromRead(char *begin, char *retIP)
{
    int i= 0;
    IPSEC_PRINT("%s-%d: begin=%s\n",__func__,__LINE__,begin);
    while((i<=15)&&((*(begin+i)!=" ")||(*(begin+i)!="\0")||(*(begin+i)!="    ")))
    {
	*(retIP+i)=*(begin+i);
	i++;
    }
    IPSEC_PRINT("%s-%d: retip=%s\n",__func__,__LINE__,retIP);
}
static void getDomainNameIP(char *peer,char *retIP)
{
    FILE *fp;
    int flag = 0;
    char readStr[256];
    char *ptr=NULL;
    struct hostent *host;
    struct in_addr inaddr;
    if(peer!=NULL)
    {
	IPSEC_PRINT("%s-%d: peer=%s\n",__func__,__LINE__,peer);
	doSystem("nslookup %s",peer);

	if((fp = fopen("Domain_list.txt","r")) != NULL)	    /*open file*/
	{
	    IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
	    memset(readStr, 0, 256);
	    while(fgets(readStr, READ_MAX_LEN, fp)!=NULL)	/*read content every one line*/
	    {
		if(strstr(readStr, peer)!=NULL)
		{
		    flag=1;
		    continue;
		}
		if(1==flag)
		{
		    if((ptr = strstr(readStr, "Address 1:"))!=NULL)
		    {
			getIPfromRead(ptr+11, retIP);
		    }
		}
	    }
	    fclose(fp);
	}
    }
    IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
    /*get all status to file SA_status.txt`*/
    sleep(1);
    return;
}
#endif

#if ((PPTP_SERVER == FYES) ||(PPTP_CLIENT == FYES))

/********************************************************************
 * 函数： IsPptpServerUse
 * 功能： 检查此PPTP账号是否正在使用
 * 创建： 2012-10-15
 * 参数： 账号名
 * 返回： NULL	    -         此账号未使用
 *	  其它	    -         此账号正在使用
 * 输出： 略
 * 作者：  
 ********************************************************************/ 
static void PptpServerInUse(char *name, char *ifname, char *ipaddr);
void checkPPTPServer()
{
    MibProfileType mibType = MIB_PROF_PPTP_SRV_ACCOUNT;
    PPTPSrvAccProfile *profPptpA = NULL;
    PPTPSrvGlobalProfile *profPptpS;
    MibProfileType mibTypeGlobal = MIB_PROF_PPTP_SRV_GLOBAL;
    int i=0,min=0,max=0;
    char ifname[16];
    char ipaddr[16];

    profPptpS = (PPTPSrvGlobalProfile *)ProfGetInstPointByIndex(mibTypeGlobal, 0);
    printf("%s-%d: localip=%x\n",__func__,__LINE__,profPptpS->localIp);
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) {
        profPptpA = (PPTPSrvAccProfile *)ProfGetInstPointByIndex(mibType, i);	/*get profile pointer*/
	PptpServerInUse(profPptpA->head.name, ifname, ipaddr);
	if(ifname!=NULL){
	    printf("%s-%d: tmp=%s\n",__func__,__LINE__,ifname);
	    memset(ifname, 0, sizeof(ifname));
	}
    }
}
static void PptpClientInUse(char *name, char *ifname, char *srvip);
void checkPPTPClient()
{
    MibProfileType mibType = MIB_PROF_PPTP_CLI;
    PPTPCliProfile *profPptpA = NULL;
    int i=0,min=0,max=0;
    char if_name[30];
    char ipaddr[16];

    memset(if_name, 0, sizeof(if_name));
    memset(ipaddr, 0, sizeof(ipaddr));
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) {
        profPptpA = (PPTPCliProfile *)ProfGetInstPointByIndex(mibType, i);	/*get profile pointer*/
	PptpClientInUse(profPptpA->head.name,if_name,ipaddr);
	if(strlen(if_name)!=0){
	    getIfIp(if_name, ipaddr);
	    IPSEC_PRINT("%s-%d: if_name=%s,ipaddr=%s\n",__func__,__LINE__,if_name,ipaddr);
	}
    }
}
struct flock* filelock_ipsec(short type, short whence)
{
    static struct flock ret;
    ret.l_type = type ;
    ret.l_start = 0;
    ret.l_whence = whence;
    ret.l_len = 0;
    ret.l_pid = getpid();
    return &ret;
}
static void PptpServerInUse(char *name, char *ifname, char *ipaddr)
{
    MibProfileType mibType = MIB_PROF_PPTP_SRV_ACCOUNT;
    PPTPSrvAccProfile *profPptpA = NULL;
    int fd = -1;
    static char dev[8];/*最大为ppp9999*/
    int     st_size;
    off_t   start, end;
    int     fileLen;
    struct st_uttPppdClient info;
    st_size = sizeof(struct st_uttPppdClient);
    char PptpName[UTT_INST_NAME_LEN + 1];
    char if_addr[16];

    memset(PptpName, 0, sizeof(PptpName));
    memset(dev, 0, sizeof(dev));
    if((ifname==NULL)||(ifname==NULL)||(ipaddr==NULL)){
	return;
    }
    strcpy(PptpName, name/* + strlen(RT_PPTP_SERVER_PRE)*/);
    IPSEC_PRINT("%s------%d, PptpName = %s\n", __func__, __LINE__, PptpName);

    profPptpA = (PPTPSrvAccProfile *)ProfGetInstPointByName(mibType, PptpName);
    if((profPptpA != NULL) && (ProfInstIsFree(profPptpA) == 0))
    {
	fd = open(UTT_PPTP_CLIENT_INFO_FILE, O_RDWR|O_CREAT, 0666);
	if (fd != -1) 
	{/*文件打开成功*/
	    start = lseek(fd, 0, SEEK_SET);
	    end = lseek(fd, 0, SEEK_END);
	    fileLen = end - start;
	    lseek(fd, 0, SEEK_SET);
	    while(fileLen >= st_size) 
	    {
		fcntl(fd, F_SETLKW, filelock_ipsec(F_WRLCK|F_RDLCK, SEEK_SET));
		read(fd, &info, st_size);
		fcntl(fd, F_SETLKW, filelock_ipsec(F_UNLCK, SEEK_SET));
		IPSEC_PRINT("%s-%d: info.ip=%s\n",__func__,__LINE__,info.ip);
		IPSEC_PRINT("%s-%d: info.ifname=%s\n",__func__,__LINE__,info.ifname);
		IPSEC_PRINT("%s-%d: info.device=%s\n",__func__,__LINE__,info.device);
		IPSEC_PRINT("%s-%d: info.ourName=%s\n",__func__,__LINE__,info.ourName);
		IPSEC_PRINT("%s-%d: info.user=%s\n",__func__,__LINE__,info.user);
		if(strlen(info.ifname)!=0)
		    getIfIp(info.ifname, if_addr);
		IPSEC_PRINT("%s-%d: if_addr=%s\n",__func__,__LINE__,if_addr);
		memset(if_addr, 0, sizeof(if_addr));
		if(strlen(info.ifname)!=0){
		    getIfIp(info.ifname, if_addr);
		    IPSEC_PRINT("%s-%d: if_addr=%s\n",__func__,__LINE__,if_addr);
		}
		if((info.ConTime == 0) &&(strcmp(info.user,profPptpA->userName) == 0))
		{/*此账号已连接上且用户名相同*/
		    strcpy(dev, info.ifname);
		    strcpy(ifname, info.ifname);
		    strcpy(ipaddr, info.ip);
		    break;
		}
		fileLen -= st_size;/*文件长度减短*/
	    }

	    close(fd);/*关闭文件*/
	}
    }
    return;
}
/********************************************************************
 * 函数： IsPptpClientUse
 * 功能： 检查此PPTP账号是否正在使用
 * 创建： 2012-10-15
 * 参数： 账号名
 * 返回： NULL	    -         此账号未使用
 *	  其它	    -         此账号正在使用
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void PptpClientInUse(char *name, char *ifname, char *srvip)
{
    char fileName[64];
    char linestr[512];
    //static char dev[8];/*最大为ppp9999*/
    FILE* fp = NULL;
    char PptpName[UTT_INST_NAME_LEN + 1];
    char localip[16];

    memset(fileName, 0, sizeof(fileName));
    memset(linestr, 0, sizeof(linestr));
    memset(localip, 0, sizeof(localip));
    //memset(dev, 0, sizeof(dev));
    if((name ==  NULL)||(ifname==NULL)||(srvip == NULL)){
	return;
    }
    strcpy(PptpName, name/* + strlen(RT_PPTP_CLIENT_PRE)*/);
    IPSEC_PRINT("%s------%d, PptpName = %s\n", __func__, __LINE__, PptpName);
    sprintf(fileName, "/var/run/pptp_%s.ppp", PptpName);
    fp = fopen(fileName, "r");
    if(fp != NULL)
    {
	if(fgets(linestr, 512, fp) != NULL)/*读取一行*/
	{
	    sscanf(linestr, "%s%*s%*s%s%s", ifname,localip,srvip);/*读取虚接口名*/
	}
	fclose(fp);/*关闭文件*/
    }
    IPSEC_PRINT("%s-%d: ifname=%s, localip=%s, srvip=%s\n",__func__,__LINE__,ifname,localip,srvip);
    return;
}

extern void ipsecAutoupOverpptp(char *instName)
{
    MibProfileType mibipsec = MIB_PROF_IPSEC;
    ipsecConfProfile *ipsecProf = NULL;
    int min = 0, max = 0, i = 0;
    int exit_flag = 0,flag = 0;
    
    if(instName != NULL){
	IPSEC_PRINT("%s-%d: instName=%s\n",__func__,__LINE__,instName);
	
	ProfInstLowHigh(mibipsec, &max, &min);
	for (i = min; i < max; i++) {
	    ipsecProf = (ipsecConfProfile *)ProfGetInstPointByIndex(mibipsec, i);	/*get profile pointer*/
	    if((ipsecProf != NULL) && (ProfInstIsFree(ipsecProf) == 0)) {
		if(strcmp(ipsecProf->profile, instName)==0){
		    exit_flag=1;
		    break;
		}
	    }
	}
	if(exit_flag == 1) {  //存在绑定此虚接口时才向下执行
	if(checkInstCount_ipsec() != 0) {
	    doSystem("ipsec setup restart");	/*start openswan service*/
	}else{
	    doSystem("ipsec setup stop");	/*stop openswan service*/
	    return;
	}
	sleep(1);
	clearConfigFile(); /*先清空配置文件*/
	/*get inst max and min index*/
	ProfInstLowHigh(mibipsec, &max, &min);
	for (i = min; i < max; i++) {
	    flag=0;
	    ipsecProf = (ipsecConfProfile *)ProfGetInstPointByIndex(mibipsec, i);	/*get profile pointer*/
	    if((ipsecProf != NULL) && (ProfInstIsFree(ipsecProf) == 0)) {
		if(strcmp(ipsecProf->profile, instName)==0)
		{
		    flag=1;
		    clearOneConfig(ipsecProf);
		    ipsecProf->sa_conn=0U;
		    IPSEC_PRINT("%s-%d: name=%s,new sa_conn=%d\n",__func__,__LINE__,ipsecProf->head.name,ipsecProf->sa_conn);
		}
		addOneConfig(ipsecProf);
#if(VPN_CONTROL == FYES)
		if(nvramProfile->vpnProfile.count<=0)
		{
                syslog(LOG_INFO, "no free vpn session.");
		} else
#endif
		{
		//if(flag==1){
		    doSystem("ipsec auto --add \"%s\"",ipsecProf->head.name);	/**/
		    doSystem("ipsec auto --rereadsecrets");	/**/
		    doSystem("ipsec auto --up \"%s\" &",ipsecProf->head.name);	/*启动隧道连接*/
		    IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
		//}
		}
	    }
	}
	}
    }
    IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
    return;
}
#endif

/*
 * static int checkInstCount_ipsec()
 * 检测 ipsec 的配置实例数
 * 返回：实例数
 * */
static int checkInstCount_ipsec()
{
    MibProfileType mibipsec = MIB_PROF_IPSEC;
    ipsecConfProfile *ipsecProf = NULL;
    int min = 0, max = 0, i = 0;
    int count=0;

    /*get inst max and min index*/
    ProfInstLowHigh(mibipsec, &max, &min);
    for (i = min; i < max; i++) {
	ipsecProf = (ipsecConfProfile *)ProfGetInstPointByIndex(mibipsec, i);	/*get profile pointer*/
	if((ipsecProf != NULL) && (ProfInstIsFree(ipsecProf) == 0)) {
	    count++;
	}
    }

    IPSEC_PRINT("%s-%d: min=%d, max=%d, count=%d\n",__func__,__LINE__,min,max,count);
    return count;
}

/*
 * getInterfaceGWaddr()
 * input:
 *	wanIndex:   wan口索引
 * return：
 *	gwAddr:	    wan口网关地址
 * 如果wan口连接方式是PPPoE或其他，取网关地址
 */
int getIfP2pDestIp(char *ifname, char *if_addr)
{
	struct ifreq ifr;
	int skfd = 0;

	if(ifname == NULL) {
		return -1;
	}
	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	//	error(E_L, E_LOG, T("getIfIp: open socket error"));
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

/*
 * getInterfaceGWaddr(int wanIndex, char *gwAddr)
 * input:
 *	wanIndex:  
 * output:
 *	gwAddr:	    wan口(wanIndex)的网关地址
 *
 * 获取wan口的网关地址
 */
static void getInterfaceGWaddr(int wanIndex, char *gwAddr)
{
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
    enum em_eth_conn_mode connMode;
    char wanDhcpGw[MAX_IPSEC_IP_LEN];
 
    if(gwAddr!=NULL)
    {
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, wanIndex);	/*get interface profile*/
    if ((profIF != NULL) && (profIF->head.active != FALSE)) { /* 线路被删除 */
	connMode = profIF->ether.connMode;  /* 连接方式 */
	if (connMode == ETH_CONN_STATIC) {
	    /* read the gateway from nvram while static mode */
	    struct in_addr gw;
	    gw.s_addr = profIF->ether.sta.gw;
	    sprintf(gwAddr, "%s", inet_ntoa(gw));
	}else if (connMode == ETH_CONN_DHCP) {
	    /* get the dynamic gateway*/
	    getWanDhcpDefaultRoute(wanIndex, wanDhcpGw);
	    sprintf(gwAddr, "%s", wanDhcpGw);
	}else {
	    if(-1 == getIfP2pDestIp(getWanIfNamePPP(wanIndex), gwAddr)) {
		strcpy(gwAddr, "");
	    }
	    /* pppoe not gw */
	}
    }
    }
    return;
}


/*
 * checkMaskOneCount(char *mask)
 * input:   
 *	mask:	掩码地址
 * return：
 *	count:	掩码中1的个数
 * 获得掩码中 1 的个数
 *  如: 255.255.255.0 ---> 24
 */
static int checkMaskOneCount(char *mask)
{
    struct in_addr ipAddr;
    Uint32 ip=0U;
    int count=0 ,i=0;
    int ret;
    if((mask!=NULL)&&(strlen(mask)!=0))
    ret = inet_aton(mask, &ipAddr);
    ip = ntohl(ipAddr.s_addr);	    /*change to interger*/
    /*repeat 32 times*/
    for(i=0;i<32;i++)
    {
	if(((ip>>i)&0x01)==1) {
	    count++;
	}
    }
    return count;
}

/*
 * clearOneConfig()
 * input: 
 *	ipsecProf:	欲清除的profile
 * 当修改或删除配置时 清除连接
 *
 */
static void clearOneConfig(ipsecConfProfile *ipsecProf)
{
    int src_maskOneC=0;
    int dest_maskOneC=0;
    char left_subnet[MAX_IPSEC_IP_LEN+1]={0};
    char right_subnet[MAX_IPSEC_IP_LEN+1]={0};
    char if_name[16]={0};
    int IfNum=-1;
    char gw_addr[16]={0};
    char cmdStr[254]={0};
    char via_gw[50]={0};
    int flag = 1;
    memset(gw_addr, 0, sizeof(gw_addr));
    memset(cmdStr, 0, sizeof(cmdStr));
    memset(via_gw, 0, sizeof(via_gw));
    /*获取左右内网的地址*/
    src_maskOneC = checkMaskOneCount(ipsecProf->srcMask);
    sprintf(left_subnet,"%s/%d",ipsecProf->srcAddr,src_maskOneC);
    dest_maskOneC = checkMaskOneCount(ipsecProf->destMask);
    sprintf(right_subnet,"%s/%d",ipsecProf->destAddr,dest_maskOneC);
    if((strstr(ipsecProf->profile, "PS_") != NULL)||(strstr(ipsecProf->profile, "PC_") != NULL)){
	/*iptables del code*/
    }else{
    /*获取接口的索引*/
    IfNum=strtol(ipsecProf->profile, NULL, 10);
    /*获取接口名*/
    if(IfNum==0) {
	strcpy(if_name, getLanIfName());
    } else {
	strcpy(if_name, getWanIfNamePPP(IfNum));
	//getInterfaceGWaddr(IfNum, gw_addr);
    }
    IPSEC_PRINT("%s-%d: IfNum=%d, if_name=%s\n",__func__,__LINE__,IfNum,if_name);
    /*清除iptables规则*/
    doSystem("iptables -t nat -D POSTROUTING -o %s  -s %s -d %s -j ACCEPT",if_name,left_subnet,right_subnet);
    if(ipsecProf->connType != GATEWAY_FROM_DYNAMIC) {	    /*添加路由*/
	if(strlen(ipsecProf->peerIP)!=0)
	{
	    sprintf(cmdStr, "ip route del %s/32 dev %s ",ipsecProf->peerIP,if_name);
	} else {
	    sprintf(cmdStr, "ip route del %s/32 dev %s ",ipsecProf->peer,if_name);
	}
	if(0 != strlen(gw_addr))
	{
	    sprintf(via_gw,"via %s",gw_addr);
	}
	strcat(cmdStr, via_gw);
	doSystem(cmdStr);
    }
    else
    {
	if((strlen(ipsecProf->peerIP)!=0)&&(strcmp(ipsecProf->peerIP, "0.0.0.0")!=0))
	{
	    sprintf(cmdStr, "ip route del %s/32 dev %s ",ipsecProf->peerIP,if_name);
	} 
	if(0 != strlen(gw_addr))
	{
	    sprintf(via_gw,"via %s",gw_addr);
	}
	strcat(cmdStr, via_gw);
	doSystem(cmdStr);
    }
    }
    /*挂断连接*/
    doSystem("ipsec auto --down \"%s\"",ipsecProf->head.name);
#if(VPN_CONTROL == FYES)
    if(ipsecProf->sa_state == 1)
#endif
    doSystem("ipsec auto --delete \"%s\"",ipsecProf->head.name);
    ipsecProf->sa_conn=0U;  /*设置SA装态为未连接*/
    return;
}
/*
 * getTransformSet
 *
 * 获取第二阶段加密认证
 */
static void getTransformSet(ipsecConfProfile *ipsecProf, char *trans)
{
    MibProfileType mibtransformset = MIB_PROF_TRANSFORMSET;
    ipsecTransformSetProfile *ipsecTransProf = NULL;
    char transformSet[MAX_TRANSFORMSETS_LENGTH+1]={0};
    char *tmp=NULL;
    int index=-1;

    if(ipsecProf!=NULL)
    {
	/*get all transformset inst name*/
	memset(transformSet,0,sizeof(transformSet));
	strcpy(transformSet,ipsecProf->transformSet);
	/*seperate inst name*/
	tmp = strtok(transformSet, ":");
	while(tmp!=NULL)
	{
	    index = ProfGetInstIndexByName(mibtransformset, tmp);	/*get isnt index*/
	    ipsecTransProf = (ipsecTransformSetProfile *)ProfGetInstPointByIndex(mibtransformset, index);
	    if((ipsecTransProf != NULL) && (ProfInstIsFree(ipsecTransProf) == 0)) {
		/*加密算法*/
		if(ipsecTransProf->encrypt==Des) {
		    strcat(trans, "des");
		} else if(ipsecTransProf->encrypt==Tri_Des) {
		    strcat(trans, "3des");
		} else if(ipsecTransProf->encrypt==Aes) {
		    strcat(trans, "aes128");
		} else if(ipsecTransProf->encrypt==Aes192) {
		    strcat(trans, "aes192");
		} else if(ipsecTransProf->encrypt==Aes256) {
		    strcat(trans, "aes256");
		}
		/*esp 认证*/
		if(ipsecTransProf->espAuth==Md5) {
		    strcat(trans, "-md5");
		} else if(ipsecTransProf->espAuth==Sha) {
		    strcat(trans, "-sha1");
		}
#if 0
		/*AH 认证*/
		if(ipsecTransProf->auth==Md5) {
		    strcat(trans, "-md5");
		} else if(ipsecTransProf->auth==Sha) {
		    strcat(trans, "-sha1");
		}
#endif
		strcat(trans, ",");
	    }
	    tmp = strtok(NULL, ":");
	}
	trans[strlen(trans)-1]='\0'; 
	IPSEC_PRINT("%s-%d: trans=%s,strlen(trans)=%d\n",__func__,__LINE__,trans,strlen(trans));
    }
    return;
}
/*
 * getPhase1IKE 
 *
 * 获取第一阶段加密认证
 */
static void getPhase1IKE(isakmpConfProfile *isakmpProf, char *policy)
{
    MibProfileType mibisakmppolicy = MIB_PROF_ISAKMP_POLICY;
    isakmpPolicyProfile *isakmpPolicyProf = NULL;
    char isakmpPolicy[MAX_ISAKMPPOLICY_LENGTH+1]={0};
    char *tmp=NULL;
    int index=-1;

    if(isakmpProf!=NULL)
    {
	/*get all policy inst name*/
	memset(isakmpPolicy,0,sizeof(isakmpPolicy));
	strcpy(isakmpPolicy,isakmpProf->policy);
	/*seperate inst name*/
	tmp = strtok(isakmpPolicy, ":");
	while(tmp!=NULL)
	{
	    index = ProfGetInstIndexByName(mibisakmppolicy, tmp);	/*get inst index*/
	    isakmpPolicyProf = (isakmpPolicyProfile *)ProfGetInstPointByIndex(mibisakmppolicy, index);
	    if((isakmpPolicyProf != NULL) && (ProfInstIsFree(isakmpPolicyProf) == 0)) {
		/*加密算法*/
		if(isakmpPolicyProf->encrypt==Des) {
		    strcat(policy, "des");
		} else if(isakmpPolicyProf->encrypt==Tri_Des) {
		    strcat(policy, "3des");
		}
		/*认证方式*/
		if(isakmpPolicyProf->hash==Md5) {
		    strcat(policy, "-md5");
		} else {
		    strcat(policy, "-sha1");
		}
		/*组类型*/
		if(isakmpPolicyProf->group==Group1) {
		    strcat(policy, "-modp768");
		} else if(isakmpPolicyProf->group==Group2) {
		    strcat(policy, "-modp1024");
		} else {
		    strcat(policy, "-modp1536");
		}
		strcat(policy, ",");
	    }
	    /*get next inst name*/
	    tmp = strtok(NULL, ":");
	}
	policy[strlen(policy)-1]='\0'; 
	IPSEC_PRINT("%s-%d: policy=%s,strlen(policy)=%d\n",__func__,__LINE__,policy,strlen(policy));
    }
    return;
}

/*
 * static void addAllConfig()
 *
 * 添加所有连接配置到ipsec.conf配置文件中
 *
 */
static void addAllConfig()
{
    FILE *fp;
    FILE *fp_sec;
    MibProfileType mibipsec = MIB_PROF_IPSEC;
    MibProfileType mibisakmp = MIB_PROF_ISAKMP;
    ipsecConfProfile *ipsecProf = NULL;
    isakmpConfProfile *isakmpProf = NULL;
    int min = 0, max = 0, i = 0;
    int isakmpIndex=-1;
    char if_name[16]={0};
    char if_addr[16]={0};
    char gw_addr[16]={0};
    char lan_addr[16]={0};
    int IfNum=0;
    int src_maskOneC=0;
    int dest_maskOneC=0;
    char left[MAX_IPSEC_IP_LEN+1]={0};
    char leftTid[MAX_IPSEC_USERLEN+1]={0};
    char left_subnet[MAX_IPSEC_IP_LEN+1]={0};
    char right[MAX_IPSEC_IP_LEN+1]={0};
    char rightTid[MAX_IPSEC_USERLEN+1]={0};
    char right_subnet[MAX_IPSEC_IP_LEN+1]={0};
    char nat_traversal[5]={0};
    char key[MAX_PRESHAREKEY_LENGTH+1]={0};
    char phase1[MAX_ISAKMPPOLICY_LENGTH]={0};
    char phase2[MAX_ISAKMPPOLICY_LENGTH]={0};
    char ikelife[16]={0};
    char keylife[16]={0};
    char cmdStr[254]={0};
    char cmdStr_del[254]={0};
    char via_gw[50]={0};
    char pptp_name[IPSEC_PROFILE_LEN+1];
    char cli_addr[16];
    char srv_ip[16];
    int flag = 1;

    /*
     * 初始化配置
     */
    IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
    if((fp = fopen(IPSEC_CONF_FILE,"w+")) == NULL)
    {
	IPSEC_PRINT("open ipsec.conf is error!\n");
	return;
    }
    if((fp_sec = fopen(IPSEC_SECRETS_FILE,"w+")) == NULL)
    {
	IPSEC_PRINT("open ipsec.secrets is error!\n");
	return;
    }
    IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);

    /*global config*/
    fprintf(fp,"version 2.0\n");
    fprintf(fp,"config setup\n");
    fprintf(fp,"    oe=off\n");
    fprintf(fp,"    protostack=netkey\n");
    fprintf(fp,"    nat_traversal=yes\n");

#if 1
    /*get inst max and min index*/
    ProfInstLowHigh(mibipsec, &max, &min);
    for (i = min; i < max; i++) {
        ipsecProf = (ipsecConfProfile *)ProfGetInstPointByIndex(mibipsec, i);	/*get profile pointer*/
#endif
	if((ipsecProf != NULL) && (ProfInstIsFree(ipsecProf) == 0)) {
	    
	    isakmpIndex = ProfGetInstIndexByName(mibisakmp, ipsecProf->isakmpProfile);	    /*get isakmp profile index*/
	    isakmpProf = (isakmpConfProfile *)ProfGetInstPointByIndex(mibisakmp, isakmpIndex);
	    
	    memset(if_name,0,sizeof(if_name));
	    memset(if_addr, 0, sizeof(if_addr));
	    memset(left, 0, sizeof(left));
	    memset(leftTid, 0, sizeof(leftTid));
	    memset(right, 0, sizeof(right));
	    memset(rightTid, 0, sizeof(rightTid));
#if ((PPTP_SERVER == FYES) ||(PPTP_CLIENT == FYES))
	    memset(pptp_name, 0, sizeof(pptp_name));
	    memset(cli_addr, 0, sizeof(cli_addr));
	    memset(srv_ip, 0, sizeof(srv_ip));
	    memset(if_name, 0, sizeof(if_name));
	    if(strstr(ipsecProf->profile, "PS_") != NULL){
		flag = 0;
		strcpy(pptp_name, (ipsecProf->profile)+3);
		PptpServerInUse(pptp_name,if_name,cli_addr);
		if(strlen(if_name) != 0){
		    getIfIp(if_name, if_addr);
		    if(strlen(if_addr)!=0){
			strcpy(left,if_addr);
		    } else {
			strcpy(left,if_name);
		    }
		}else{
		    strcpy(left,""); //?
		    continue;
		}
		//strcpy(right,cli_addr);
		IPSEC_PRINT("%s-%d: pptp_name=%s, if_name=%s, if_addr=%s, cli_addr=%s\n",__func__,__LINE__,pptp_name,if_name,if_addr,cli_addr);
	    }else if(strstr(ipsecProf->profile, "PC_") != NULL){
		flag = 0;
		strcpy(pptp_name, (ipsecProf->profile)+3);
		IPSEC_PRINT("%s-%d: pptp_name=%s\n",__func__,__LINE__,pptp_name);
		PptpClientInUse(pptp_name,if_name, srv_ip);
		if(strlen(if_name) != 0){
		    getIfIp(if_name, if_addr);
		    if(strlen(if_addr)!=0){
			strcpy(left,if_addr);
		    } else {
			strcpy(left,if_name);
		    }
		}else{
		    strcpy(left,""); //?
		    continue;
		}
		//strcpy(right,cli_addr);
		IPSEC_PRINT("%s-%d: pptp_name=%s, if_name=%s, if_addr=%s, cli_addr=%s\n",__func__,__LINE__,pptp_name,if_name,if_addr,cli_addr);
	    }else
#endif
	    {
	    flag = 1;
	    IfNum=strtol(ipsecProf->profile, NULL, 10);
	    if(IfNum==0) {
		getIfIp(getLanIfName(),if_addr);    /*接口IP*/
		strcpy(if_name, getLanIfName());    /*接口名*/
	    } else {
		getIfIp(getWanIfNamePPP(IfNum), if_addr);	/*接口IP*/
		getInterfaceGWaddr(IfNum, gw_addr);
		strcpy(if_name, getWanIfNamePPP(IfNum));	/*接口名*/
	    }
	    IPSEC_PRINT("%s-%d: IfNum=%d, if_name=%s, if_addr=%s,gw_addr=%s\n",__func__,__LINE__,IfNum,if_name,if_addr,gw_addr);
	    if(strlen(if_addr)!=0){
		strcpy(left,if_addr);
	    } else {
		strcpy(left,if_name);
	    }
	    }
	    src_maskOneC = checkMaskOneCount(ipsecProf->srcMask);
	    sprintf(left_subnet,"%s/%d",ipsecProf->srcAddr,src_maskOneC);
	    if(isakmpProf->localType == IKE_FQDN){
		if(strlen(isakmpProf->localuser)==0) {
		sprintf(leftTid,"%s",T("%any"));
		} else {
		sprintf(leftTid,"@%s",isakmpProf->localuser);
		}
	    } else {
		if(strlen(isakmpProf->localuser)==0) {
		sprintf(leftTid,"%s",T("%any"));
		} else {
		sprintf(leftTid,"%s",isakmpProf->localuser);
		}
	    }
	    getIfIp(getLanIfName(),lan_addr);

	    if(strlen(ipsecProf->peerIP)!=0) {
		IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
		strcpy(right,ipsecProf->peerIP);
	    } else {
		strcpy(right,ipsecProf->peer);
	    }
	    if(isakmpProf->remoteType == IKE_FQDN){
		if(strlen(isakmpProf->remoteuser)==0) {
		sprintf(rightTid,"%s",T("%any"));
		} else {
		sprintf(rightTid,"@%s",isakmpProf->remoteuser);
		}
	    } else {
		if(strlen(isakmpProf->remoteuser)==0) {
		sprintf(rightTid,"%s",T("%any"));
		} else {
		sprintf(rightTid,"%s",isakmpProf->remoteuser);
		}
	    }
	    dest_maskOneC = checkMaskOneCount(ipsecProf->destMask);
	    sprintf(right_subnet,"%s/%d",ipsecProf->destAddr,dest_maskOneC);
	    IPSEC_PRINT("%s-%d: left=%s,left_subnet=%s,leftTid=%s,right=%s,right_subnet=%s,rightTid=%s\n",__func__,__LINE__,left,left_subnet,leftTid,right,right_subnet,rightTid);
	    if(ipsecProf->nat_enable==FUN_ENABLE) {	
		strcpy(nat_traversal,"yes");
	    } else {
		strcpy(nat_traversal,"no");
	    }
	    strcpy(key,isakmpProf->preshareKey);
	    IPSEC_PRINT("%s-%d: nat_travel=%s,key=%s\n",__func__,__LINE__,nat_traversal,key);
	    if(ipsecProf->connType == GATEWAY_TO_GATEWAY) {
		fprintf(fp_sec,"%s %s : PSK \"%s\"\n",left,right,key);
	    } else {
		fprintf(fp_sec,"%s %s : PSK \"%s\"\n",leftTid,rightTid,key);
	    }
#if 0
	    doSystem("ipsec_add_main_in_conf.sh  %s %s %s %s %s %s %s %s %s %s", 
		    ipsecProf->head.name, AUTH_TYPE_PSK, leftTid, left, left_subnet, rightTid, right, right_subnet, nat_traversal, key);
	    doSystem("ipsec_add_ikekey_in_conf.sh  %s %s %s %s %s %s",
		    AUTH_TYPE_PSK,); 
#endif	    
	    if((ipsecProf->connType != GATEWAY_FROM_DYNAMIC)&&(flag==1)) {	    /*添加路由*/
		memset(cmdStr, 0, sizeof(cmdStr));
		memset(cmdStr_del, 0, sizeof(cmdStr_del));
		memset(via_gw, 0, sizeof(via_gw));
		if(strlen(if_name)!=0){
		if(strlen(ipsecProf->peerIP)!=0)
		{
		    sprintf(cmdStr, "ip route replace %s/32 dev %s ",ipsecProf->peerIP,if_name);
		    sprintf(cmdStr_del, "ip route del %s/32 dev %s ",ipsecProf->peerIP,if_name);
		} else {
		    sprintf(cmdStr, "ip route replace %s/32 dev %s ",ipsecProf->peer,if_name);
		    sprintf(cmdStr_del, "ip route del %s/32 dev %s ",ipsecProf->peer,if_name);
		}
		if(0 != strlen(gw_addr))
		{
		    sprintf(via_gw,"via %s",gw_addr);
		}
		strcat(cmdStr, via_gw);
		strcat(cmdStr_del, via_gw);
		doSystem(cmdStr_del);
		doSystem(cmdStr);
		IPSEC_PRINT("%s-%d: cmdStr=%s, cmdStr_del=%s\n",__func__,__LINE__,cmdStr,cmdStr_del);
		}
	    }
	    /*添加iptables 规则*/
	    if(strlen(if_name)!=0){
	    doSystem("iptables -t nat -D POSTROUTING -o %s -s %s -d %s -j ACCEPT",if_name,left_subnet,right_subnet);
	    doSystem("iptables -t nat -I POSTROUTING -o %s -s %s -d %s -j ACCEPT",if_name,left_subnet,right_subnet);
	    IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
	    }
	    /* begine write every configdata
	     * to ipsec.config
	     */
	    fprintf(fp,"\nconn %s\n",ipsecProf->head.name);
	    fprintf(fp,"    type=%s\n","tunnel");
#if 0
	    if(ipsecProf->connType == DYNAMIC_TO_GATEWAY)
	    {
		fprintf(fp,"    left=%s\n","%defaultroute");
	    }
	    else
	    {
#endif
		fprintf(fp,"    left=%s\n",left);
#if 0
	    }
#endif
	    fprintf(fp,"    leftsubnet=%s\n",left_subnet);
	    if(ipsecProf->connType != GATEWAY_TO_GATEWAY)
	    {
		fprintf(fp,"    leftid=%s\n",leftTid);
	    }
	    fprintf(fp,"    leftsourceip=%s\n",lan_addr);	
	    if(0 != strlen(gw_addr))
		fprintf(fp,"    leftnexthop=%s\n",gw_addr);	
	    if(ipsecProf->connType == GATEWAY_FROM_DYNAMIC) {
		fprintf(fp,"    right=%s\n","\%any");
	    } else {
		fprintf(fp,"    right=%s\n",right);
	    }
	    fprintf(fp,"    rightsubnet=%s\n",right_subnet);
	    if(ipsecProf->connType != GATEWAY_TO_GATEWAY)
	    {
		fprintf(fp,"    rightid=%s\n",rightTid);
	    }
#if 1
	    memset(ikelife,0,sizeof(ikelife));
	    memset(keylife,0,sizeof(keylife));
	    if((isakmpProf->lifetime)%60 == 0) {
		sprintf(ikelife,"%d",(isakmpProf->lifetime)/60);
	    } else {
		sprintf(ikelife,"%d",((isakmpProf->lifetime)/60)+1);
	    }
	    if((ipsecProf->lifetime_sec)%60 == 0) {
		sprintf(keylife,"%d",(ipsecProf->lifetime_sec)/60);
	    } else {
		sprintf(keylife,"%d",((ipsecProf->lifetime_sec)/60)+1);
	    }
	    fprintf(fp,"    keylife=%sm\n",keylife);	/*第一阶段生存时间*/
	    fprintf(fp,"    ikelifetime=%sm\n",ikelife);    /*第二阶段生存时间*/
	    IPSEC_PRINT("%s-%d: ikelifetime=%d,keylife=%d\n",__func__,__LINE__,isakmpProf->lifetime,ipsecProf->lifetime_sec);
	    memset(phase2,0,sizeof(phase2));
	    getTransformSet(ipsecProf, phase2);
	    fprintf(fp,"    phase2=%s\n","esp");	    /*第二阶段加密认证方式*/
	    if(strlen(phase2))
	    {
		fprintf(fp,"    phase2alg=%s\n",phase2);	    /**/
	    }
	    if(isakmpProf->mode==Mainmode) {
		fprintf(fp,"    aggrmode=no\n");
	    } else {
		fprintf(fp,"    aggrmode=yes\n");
	    }
	    fprintf(fp,"    pfs=%s\n","no");	    /*是否支持完美向前*/
#if 0
#endif
	    memset(phase1,0,sizeof(phase1));
	    getPhase1IKE(isakmpProf, phase1);
	    if(strlen(phase1)!=0)
	    {
		fprintf(fp,"    ike=%s\n",phase1);	    /*第一阶段加密认证方式*/
	    }
	    fprintf(fp,"    compress=no\n");   
	    fprintf(fp,"    forceencaps=yes\n");    /*封装*/
	    fprintf(fp,"    authby=secret\n");	    /*共享密钥*/
	    if(ipsecProf->active==FUN_ENABLE) {
		fprintf(fp,"    auto=%s\n","start");	    /*start 自动建立连接*/
	    } else {
		fprintf(fp,"    auto=%s\n","add");	    /*start 自动建立连接*/
	    }
	    if(ipsecProf->anti_replay == FUN_ENABLE) {
		fprintf(fp,"    overlapip=yes\n");	    /*抗重播*/
	    } else {
		fprintf(fp,"    overlapip=no\n");	    /*抗重播*/
	    }
	    if(ipsecProf->dpd_enable == FUN_ENABLE)
	    {
		fprintf(fp,"    dpddelay=%d\n",ipsecProf->dpd_heartbeat);	    /*心跳*/
		fprintf(fp,"    dpdtimeout=%d\n",120);	    /*DPD超时*/
		fprintf(fp,"    dpdaction=restart\n"); /*DPD检测失败后重连*/	
	    }
	    //doSystem("ipsec auto --up \"%s\"",ipsecProf->head.name);
	    ipsecProf->sa_conn=0U;
	    IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
#endif
	}
#if 1
    }
#endif
    fclose(fp_sec);
    fclose(fp);
    return;
}

/*
 * static void addOnePsk(char *left,char *right,char *key)
 *
 * 添加密钥到ipsec.secrets文件中
 *
 */
static void addOnePsk(char *left,char *right,char *key)
{
    FILE *fp;
    
    if((left==NULL)||(right==NULL)||(key==NULL))
    {
	return;
    }
    /*open ipsec.secrets*/
    if((fp = fopen(IPSEC_SECRETS_FILE,"a")) == NULL)
    {
	IPSEC_PRINT("open ipsec.secrets is error!\n");
	return;
    }
    fprintf(fp,"%s %s : PSK \"%s\"\n",left,right,key);	/*write psk to file*/
    fclose(fp);	    /*close*/
    return;
}

/*
 * static void addOneConfig(ipsecConfProfile *ipsecProf)
 *
 * 添加一个连接配置到ipsec.conf文件中
 *
 */
static void addOneConfig(ipsecConfProfile *ipsecProf)
{
    FILE *fp;
    //MibProfileType mibipsec = MIB_PROF_IPSEC;
    MibProfileType mibisakmp = MIB_PROF_ISAKMP;
    //ipsecConfProfile *ipsecProf = NULL;
    isakmpConfProfile *isakmpProf = NULL;
    int isakmpIndex=-1;
    char if_name[16]={0};
    char if_addr[16]={0};
    char gw_addr[16]={0};
    char lan_addr[16]={0};
    int IfNum=0;
    int src_maskOneC=0;
    int dest_maskOneC=0;
    char left[MAX_IPSEC_IP_LEN+1]={0};
    char leftTid[MAX_IPSEC_USERLEN+1]={0};
    char left_subnet[MAX_IPSEC_IP_LEN+1]={0};
    char right[MAX_IPSEC_IP_LEN+1]={0};
    char rightTid[MAX_IPSEC_USERLEN+1]={0};
    char right_subnet[MAX_IPSEC_IP_LEN+1]={0};
    char nat_traversal[5]={0};
    char key[MAX_PRESHAREKEY_LENGTH+1]={0};
    char phase1[MAX_ISAKMPPOLICY_LENGTH]={0};
    char phase2[MAX_ISAKMPPOLICY_LENGTH]={0};
    char ikelife[16]={0};
    char keylife[16]={0};
    char cmdStr[254]={0};
    char cmdStr_del[254]={0};
    char via_gw[50]={0};
    char pptp_name[IPSEC_PROFILE_LEN+1];
    char cli_addr[16];
    char srv_ip[16];
    int flag = 1;

    /*
     * 初始化配置
     */
    memset(if_name, 0, sizeof(if_name));
    memset(if_addr, 0, sizeof(if_addr));
    memset(left, 0, sizeof(left));
    memset(leftTid, 0, sizeof(leftTid));
    memset(right, 0, sizeof(right));
    memset(rightTid, 0, sizeof(rightTid));
    memset(if_addr, 0, sizeof(if_addr));
    memset(gw_addr, 0, sizeof(gw_addr));
    if((fp = fopen(IPSEC_CONF_FILE,"a")) == NULL)
    {
	IPSEC_PRINT("open ipsec.conf is error!\n");
	return;
    }
#if 0
#endif
    if((ipsecProf != NULL) && (ProfInstIsFree(ipsecProf) == 0)) {
    
	isakmpIndex = ProfGetInstIndexByName(mibisakmp, ipsecProf->isakmpProfile);
	isakmpProf = (isakmpConfProfile *)ProfGetInstPointByIndex(mibisakmp, isakmpIndex);
#if ((PPTP_SERVER == FYES) ||(PPTP_CLIENT == FYES))
	memset(pptp_name, 0, sizeof(pptp_name));
	memset(cli_addr, 0, sizeof(cli_addr));
	memset(srv_ip, 0, sizeof(srv_ip));
	if(strstr(ipsecProf->profile, "PS_") != NULL){
	    flag = 0;
	    strcpy(pptp_name, (ipsecProf->profile)+3);
	    PptpServerInUse(pptp_name,if_name,cli_addr);
	    if(strlen(if_name) != 0){
		getIfIp(if_name, if_addr);
		if(strlen(if_addr)!=0){
		    strcpy(left,if_addr);
		} else {
		    strcpy(left,if_name);
		}
	    }else{
		strcpy(left,""); //?
		fclose(fp);	    /*close*/
		return;
	    }
	    //strcpy(right,cli_addr);
	    IPSEC_PRINT("%s-%d: pptp_name=%s, if_name=%s, if_addr=%s, cli_addr=%s\n",__func__,__LINE__,pptp_name,if_name,if_addr,cli_addr);
	}else if(strstr(ipsecProf->profile, "PC_") != NULL){
	    flag = 0;
	    strcpy(pptp_name, (ipsecProf->profile)+3);
	    IPSEC_PRINT("%s-%d: pptp_name=%s\n",__func__,__LINE__,pptp_name);
	    PptpClientInUse(pptp_name,if_name, srv_ip);
	    if(strlen(if_name) != 0){
		getIfIp(if_name, if_addr);
		if(strlen(if_addr)!=0){
		    strcpy(left,if_addr);
		} else {
		    strcpy(left,if_name);
		}
	    }else{
		strcpy(left,""); //?
		fclose(fp);	    /*close*/
		return;
	    }
	    //strcpy(right,cli_addr);
	    IPSEC_PRINT("%s-%d: pptp_name=%s, if_name=%s, if_addr=%s, cli_addr=%s\n",__func__,__LINE__,pptp_name,if_name,if_addr,cli_addr);
	}else
#endif
	{
	flag = 1;
	IfNum=strtol(ipsecProf->profile, NULL, 10);
	if(IfNum==0) {
	    
	    getIfIp(getLanIfName(),if_addr);
	    strcpy(if_name, getLanIfName());
	} else {
	    getIfIp(getWanIfNamePPP(IfNum), if_addr);
	    getInterfaceGWaddr(IfNum, gw_addr);
	    strcpy(if_name, getWanIfNamePPP(IfNum));
	}
	IPSEC_PRINT("%s-%d: IfNum=%d, if_name=%s, if_addr=%s,gw_addr=%s\n",__func__,__LINE__,IfNum,if_name,if_addr,gw_addr);
	if(strlen(if_addr)!=0){
	    strcpy(left,if_addr);
	} else {
	    strcpy(left,if_name);
	}
	}
	src_maskOneC = checkMaskOneCount(ipsecProf->srcMask);
	sprintf(left_subnet,"%s/%d",ipsecProf->srcAddr,src_maskOneC);
	if(isakmpProf->localType == IKE_FQDN){
	    if(strlen(isakmpProf->localuser)==0) {
	    sprintf(leftTid,"%s",T("%any"));
	    } else {
	    sprintf(leftTid,"@%s",isakmpProf->localuser);
	    }
	} else {
	    if(strlen(isakmpProf->localuser)==0) {
	    sprintf(leftTid,"%s",T("%any"));
	    } else {
	    sprintf(leftTid,"%s",isakmpProf->localuser);
	    }
	}
	getIfIp(getLanIfName(),lan_addr);
	
    
	if(strlen(ipsecProf->peerIP)!=0) {
	    strcpy(right,ipsecProf->peerIP);
	} else {
	    strcpy(right,ipsecProf->peer);
	}
	if(isakmpProf->remoteType == IKE_FQDN){
	    if(strlen(isakmpProf->remoteuser)==0) {
	    sprintf(rightTid,"%s",T("%any"));
	    } else {
	    sprintf(rightTid,"@%s",isakmpProf->remoteuser);
	    }
	} else {
	    if(strlen(isakmpProf->remoteuser)==0) {
	    sprintf(rightTid,"%s",T("%any"));
	    } else {
	    sprintf(rightTid,"%s",isakmpProf->remoteuser);
	    }
	}
	dest_maskOneC = checkMaskOneCount(ipsecProf->destMask);
	sprintf(right_subnet,"%s/%d",ipsecProf->destAddr,dest_maskOneC);
	IPSEC_PRINT("%s-%d: left=%s,left_subnet=%s,leftTid=%s,right=%s,right_subnet=%s,rightTid=%s\n",__func__,__LINE__,left,left_subnet,leftTid,right,right_subnet,rightTid);
	
	if(ipsecProf->nat_enable==FUN_ENABLE) {	
	    strcpy(nat_traversal,"yes");
	} else {
	    strcpy(nat_traversal,"no");
	}
	strcpy(key,isakmpProf->preshareKey);
	IPSEC_PRINT("%s-%d: nat_travel=%s,key=%s\n",__func__,__LINE__,nat_traversal,key);
	/*add psk to file ipsec.secrets*/
	if(ipsecProf->connType == GATEWAY_TO_GATEWAY) {
	    addOnePsk(left,right,key);
	} else {
	    addOnePsk(leftTid,rightTid,key);
	}
	if((ipsecProf->connType != GATEWAY_FROM_DYNAMIC)&&(flag==1)) {
	    memset(cmdStr, 0, sizeof(cmdStr));
	    memset(cmdStr_del, 0, sizeof(cmdStr_del));
	    memset(via_gw, 0, sizeof(via_gw));
	    if(strlen(if_name)!=0) {
	    if(strlen(ipsecProf->peerIP)!=0)
	    {
		sprintf(cmdStr, "ip route replace %s/32 dev %s ",ipsecProf->peerIP,if_name);
		sprintf(cmdStr_del, "ip route del %s/32 dev %s ",ipsecProf->peerIP,if_name);
	    } else {
		sprintf(cmdStr, "ip route replace %s/32 dev %s ",ipsecProf->peer,if_name);
		sprintf(cmdStr_del, "ip route del %s/32 dev %s ",ipsecProf->peer,if_name);
	    }
	    if(0 != strlen(gw_addr))
	    {
		sprintf(via_gw,"via %s",gw_addr);
	    }
	    strcat(cmdStr, via_gw);
	    strcat(cmdStr_del, via_gw);
	    doSystem(cmdStr_del);
	    doSystem(cmdStr);
	    }
	    IPSEC_PRINT("%s-%d: cmdStr=%s\n",__func__,__LINE__,cmdStr);
	}
	/*添加 iptables 规则*/
	if(strlen(if_name)!=0){
	doSystem("iptables -t nat -D POSTROUTING -o %s -s %s -d %s -j ACCEPT",if_name,left_subnet,right_subnet);
	doSystem("iptables -t nat -I POSTROUTING -o %s -s %s -d %s -j ACCEPT",if_name,left_subnet,right_subnet);
	IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
	}
	/*begine write cofig data to file*/
	fprintf(fp,"\nconn %s\n",ipsecProf->head.name);
	fprintf(fp,"    type=%s\n","tunnel");
#if 0
	if(ipsecProf->connType == DYNAMIC_TO_GATEWAY)
	{
	    fprintf(fp,"    left=%s\n","\%defaultroute");	    /*本地 公网地址*/
	}
	else
	{
#endif
	fprintf(fp,"    left=%s\n",left);
#if 0
	}
#endif
	fprintf(fp,"    leftsubnet=%s\n",left_subnet);	/*本地内网地址 掩码*/
	if(ipsecProf->connType != GATEWAY_TO_GATEWAY)
	{
	    fprintf(fp,"    leftid=%s\n",leftTid);		/*本地身份*/
	}
	fprintf(fp,"    leftsourceip=%s\n",lan_addr);	/*本地lan口地址*/
	if(0 != strlen(gw_addr))
	    fprintf(fp,"    leftnexthop=%s\n",gw_addr);	
	if(ipsecProf->connType == GATEWAY_FROM_DYNAMIC) {
	    fprintf(fp,"    right=%s\n","\%any");		/*远端 公网地址*/
	} else {
	    fprintf(fp,"    right=%s\n",right);
	}
	fprintf(fp,"    rightsubnet=%s\n",right_subnet);	/*远端 内网地址 掩码*/
	if(ipsecProf->connType != GATEWAY_TO_GATEWAY)
	{
	    fprintf(fp,"    rightid=%s\n",rightTid);	/*远端身份*/
	}
#if 1
	memset(ikelife,0,sizeof(ikelife));
	memset(keylife,0,sizeof(keylife));
	if((isakmpProf->lifetime)%60 == 0) {
	    sprintf(ikelife,"%d",(isakmpProf->lifetime)/60);
	} else {
	    sprintf(ikelife,"%d",((isakmpProf->lifetime)/60)+1);
	}
	if((ipsecProf->lifetime_sec)%60 == 0) {
	    sprintf(keylife,"%d",(ipsecProf->lifetime_sec)/60);
	} else {
	    sprintf(keylife,"%d",((ipsecProf->lifetime_sec)/60)+1);
	}
	fprintf(fp,"    keylife=%sm\n",keylife);	/*第一阶段生存时间*/
	fprintf(fp,"    ikelifetime=%sm\n",ikelife);    /*第二阶段生存时间*/
	IPSEC_PRINT("%s-%d: ikelifetime=%d,keylife=%d\n",__func__,__LINE__,isakmpProf->lifetime,ipsecProf->lifetime_sec);
	memset(phase2,0,sizeof(phase2));
	getTransformSet(ipsecProf, phase2);
	fprintf(fp,"    phase2=%s\n","esp");	    /*第二阶段加密认证方式*/
	if(strlen(phase2))
	{
	    fprintf(fp,"    phase2alg=%s\n",phase2);	    /**/
	}
	if(isakmpProf->mode==Mainmode) {
	    fprintf(fp,"    aggrmode=no\n");	/*主模式*/
	} else {
	    fprintf(fp,"    aggrmode=yes\n");	/*野蛮模式*/
	}
	fprintf(fp,"    pfs=%s\n","no");	    /*是否支持完美向前*/

	memset(phase1,0,sizeof(phase1));
	getPhase1IKE(isakmpProf, phase1);
	if(strlen(phase1)!=0)
	{
	    fprintf(fp,"    ike=%s\n",phase1);	    /*第一阶段加密认证方式*/
	}
	fprintf(fp,"    compress=no\n");   
	fprintf(fp,"    forceencaps=yes\n");    /*RFC-3948封装*/
	fprintf(fp,"    authby=secret\n");	    /*共享密钥*/
	if(ipsecProf->active == FUN_ENABLE) {
	    fprintf(fp,"    auto=%s\n","start");	    /*start 自动建立连接*/
	} else {
	    fprintf(fp,"    auto=%s\n","add");	    /*start 自动建立连接*/
	}
	if(ipsecProf->anti_replay == FUN_ENABLE) {
	    fprintf(fp,"    overlapip=yes\n");	    /*抗重播*/
	} else {
	    fprintf(fp,"    overlapip=no\n");	    /*抗重播*/
	}
	if(ipsecProf->dpd_enable == FUN_ENABLE)
	{
	    fprintf(fp,"    dpddelay=%d\n",ipsecProf->dpd_heartbeat);	    /*心跳*/
	    fprintf(fp,"    dpdtimeout=%d\n",120);	    /*DPD超时*/
	    fprintf(fp,"    dpdaction=restart\n"); /*DPD检测失败后重连*/	
	}
	ipsecProf->sa_conn=0U;
	IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
#endif
    }
    fclose(fp);	    /*close*/
    return;
}

/* 
 * ipsecConfProfileChange 
 *
 * ipsec  profile change函数
 */
static void ipsecConfProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr )
{
    ipsecConfProfile *newProf = (ipsecConfProfile *)newProfileDataPtr;
    ipsecConfProfile *oldProf = (ipsecConfProfile *)oldProfileDataPtr;

    switch(changeType) {
        case PROF_CHANGE_ADD:
	    /*
	     * 配置文件加一个账号
	     */
	    IPSEC_PRINT("%s-%d: name=%s\n",__func__,__LINE__,newProf->head.name);
	    addOneConfig(newProf);
	    /* 添加实例时，如果是第一个实例
	     *  此时ipsec时关闭的，应该首先打开ipsec*/
	    if((checkInstCount_ipsec() != 0)&&(checkInstCount_ipsec() == 1)) {
		insmod_modules_ipsec();
		doSystem("ipsec setup restart");	/*重启动openswan*/
		IPSEC_PRINT("%s-%d: ipsec start\n",__func__,__LINE__);
		startUpDomainIP(1);
	    }
#if 1
#if(VPN_CONTROL == FYES)
	    if(nvramProfile->vpnProfile.count<=0)
	    {
                syslog(LOG_INFO, "no free vpn session.");
	    } else 
#endif
	    {
	    doSystem("ipsec auto --add \"%s\"",newProf->head.name);	/**/
	    doSystem("ipsec auto --rereadsecrets");	/**/
	    IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
	    doSystem("ipsec auto --up \"%s\" &",newProf->head.name);	/*启动隧道连接*/
	    IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
	    }
#endif
	    break;
        case PROF_CHANGE_EDIT:
	    /*
	     * 先删除对应账号，再添加对应账号
	     */	    
	    clearOneConfig(oldProf);
	    newProf->sa_conn=0U;
	    IPSEC_PRINT("%s-%d: name=%s,new sa_conn=%d\n",__func__,__LINE__,newProf->head.name,newProf->sa_conn);
	    addAllConfig();
#if(VPN_CONTROL == FYES)
	    if(nvramProfile->vpnProfile.count<=0)
	    {
                syslog(LOG_INFO, "no free vpn session.");
	    } else 
#endif
	    {
	    doSystem("ipsec auto --add \"%s\"",newProf->head.name);	/**/
	    doSystem("ipsec auto --rereadsecrets");	/**/
	    IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
	    doSystem("ipsec auto --up \"%s\" &",newProf->head.name);	/*启动隧道连接*/
	    }
#if 0
	    doSystem("ipsec auto --replace \"%s\"",newProf->head.name);	/**/
	    doSystem("ipsec auto --rereadsecrets");	/**/
	    doSystem("ipsec auto --up \"%s\" &",newProf->head.name);	/*启动隧道连接*/
	    sleep(1);
#endif
	    break;
        case PROF_CHANGE_DEL:
	    /*
	     * 配置文件删除对应参数
	     */
	    clearOneConfig(oldProf);
	    addAllConfig();
	    break;
        case PROF_CHANGE_DELALL:
	    /*
	     * 配置文件删除所有账号
	     */
	    clearOneConfig(oldProf);
	    addAllConfig();
	    break;
        default:
	    break;
    }
    /*当删除实例时，如果删除后，配置中的ipsec实例数为0
     * 则关闭ipsec服务*/
    if((changeType == PROF_CHANGE_DEL)||(changeType == PROF_CHANGE_DELALL)) {
	if(checkInstCount_ipsec() == 0) {
	    rmmod_modules_ipsec();
	    doSystem("ipsec setup stop");	/*重启动openswan*/
	    IPSEC_PRINT("%s-%d: ipsec stop\n",__func__,__LINE__);
	    startUpDomainIP(0);
	}
    }
}

/*
 * checkOneConfigSA()
 * input:
 *	prof:	欲检测的 profile
 * 检测prof的 SA 状态
 */
static void checkOneConfigSA(ipsecConfProfile *prof)
{
    FILE *fp;
    char readStr[READ_MAX_LEN];
    /*open file SA_autoup_status.txt*/
    if((fp = fopen(IPSEC_SA_AUTOUP_FILE,"r")) != NULL)
    {
	while(fgets(readStr, READ_MAX_LEN, fp)!=NULL)
	{
	    if(strstr(readStr, prof->head.name)!=NULL)
	    {
		if(strstr(readStr, IPSEC_UNROUTED_FIND_STR)!=NULL)
		{
		    prof->sa_conn=0U;	/*设置状态为未建立*/
		    IPSEC_PRINT("%s-%d: read=%s\n",__func__,__LINE__,readStr);
		    break;
		}
		else
		{
		    if(strstr(readStr, IPSEC_IPSEC_SA_FIND_STR)!=NULL)
		    {
			prof->sa_conn=3U;		/*设置ipsec SA 已建立*/
			IPSEC_PRINT("%s-%d: read=%s\n",__func__,__LINE__,readStr);
			break;
		    }
		    if(strstr(readStr, IPSEC_ISAKMP_SA_FIND_STR)!=NULL)
		    {
			if(prof->sa_conn<2U)
			{
			    prof->sa_conn=2U;	/*设置状态为IPSec协商*/
			    IPSEC_PRINT("%s-%d: read=%s\n",__func__,__LINE__,readStr);
			}
		    }
		    if(prof->sa_conn < 1U)
		    {
			prof->sa_conn=1U;	/*设置状态为IKE协商*/
			IPSEC_PRINT("%s-%d: read=%s\n",__func__,__LINE__,readStr);
		    }
		}
	    }
	}
	fclose(fp);	    /*close file*/
    } else {
	IPSEC_PRINT("open SA_autoup_status.txt is error!\n");
    }
    return;
}
/*
 * setAllConfigSA_status
 *
 * get all config SA status
 */
static void setAllConfigSA_status()
{
    FILE *fp;
    MibProfileType mibipsec = MIB_PROF_IPSEC;
    ipsecConfProfile *ipsecProf = NULL;
    int min = 0, max = 0, i = 0;
    char readStr[READ_MAX_LEN];

    /*get all status to file SA_status.txt`*/
    sleep(1);
    if((fp = fopen(IPSEC_SA_STATUS_FILE,"r")) != NULL)	    /*open file*/
    {
#if 1
    memset(readStr, 0, READ_MAX_LEN);
    ProfInstLowHigh(mibipsec, &max, &min);
    while(fgets(readStr, READ_MAX_LEN, fp)!=NULL)	/*read content every one line*/
    {
#if 1 
	for (i = min; i < max; i++) {
	    ipsecProf = (ipsecConfProfile *)ProfGetInstPointByIndex(mibipsec, i);
	    if((ipsecProf != NULL) && (ProfInstIsFree(ipsecProf) == 0)) 
	    {
		if(strstr(readStr, ipsecProf->head.name)!=NULL)
		{
		    if(strstr(readStr, IPSEC_UNROUTED_FIND_STR)!=NULL)
		    {
			    ipsecProf->sa_conn=0U;	/*设置状态为未建立*/
			    IPSEC_PRINT("%s-%d: read=%s\n",__func__,__LINE__,readStr);
			    break;
		    }
		    else
		    {
			if(strstr(readStr, IPSEC_IPSEC_SA_FIND_STR)!=NULL)
			{
			    ipsecProf->sa_conn=3U;		/*设置ipsec SA 已建立*/
			    IPSEC_PRINT("%s-%d: read=%s\n",__func__,__LINE__,readStr);
			    break;
			}
			if(strstr(readStr, IPSEC_ISAKMP_SA_FIND_STR)!=NULL)
			{
			    if(ipsecProf->sa_conn < 2U)
			    {
				ipsecProf->sa_conn=2U;	/*设置状态为IPSec协商*/
				IPSEC_PRINT("%s-%d: read=%s\n",__func__,__LINE__,readStr);
			    }
			}
			if(ipsecProf->sa_conn < 1U)
			{
			    ipsecProf->sa_conn=1U;	/*设置状态为IKE协商*/
			    IPSEC_PRINT("%s-%d: read=%s\n",__func__,__LINE__,readStr);
			}
		    }
		    break;
		}
	    }
	}
#endif
    }
#endif
    fclose(fp);	    /*close file*/
    //sleep(1);
    }
    else
    {
	IPSEC_PRINT("open SA_status.txt is error!\n");
    }
    return;
}

/*
 *  createInitshFile()
 *
 * 生成时文件  在开机初始化
 * 将ipsec连接需要的内容拷贝到新建目录
 * 使得 ipsec 能正常工作
 */
static void  createInitshFile()
{
    FILE *fp;

    /*create new file*/
    if((fp = fopen("/bin/createforipsec.sh", "w+"))!=NULL)
    {
	fputs("#!/bin/sh\n", fp);
#if 0	
	fputs("PATH=$PATH:/ipsec/sbin\n", fp);
	fputs("export PATH\n", fp);
#endif
	/*create new folder*/
	fputs("rm -rf data3\n", fp);
	fputs("cd /\n", fp);
	fputs("mkdir data3\n", fp);
	fputs("cd data3\n", fp);
	fputs("mkdir wang.bingrong\n", fp);
	fputs("cd wang.bingrong/\n", fp);
	fputs("mkdir brwang\n", fp);
	fputs("cd brwang/\n", fp);
	fputs("mkdir productsoft\n", fp);
	fputs("cd productsoft/\n", fp);
	fputs("mkdir 3520G\n", fp);
	fputs("cd 3520G/\n", fp);
	fputs("mkdir mv821\n", fp);
	fputs("cd mv821/\n", fp);
	fputs("mkdir ipsec\n", fp);
	fputs("cd ipsec/\n", fp);
	fputs("#mkdir lib\n", fp);
	fputs("#cd lib/\n", fp);
	fputs("#mkdir ipsec\n", fp);
	/*copy file */
	fputs("cd /data3/wang.bingrong/brwang/productsoft/3520G/mv821/ipsec/\n", fp);
	fputs("cp -r /ipsec/* ./\n", fp);
	fputs("cd /\n", fp);
	
	fclose(fp);	/*close file*/
    }
    return ;
}


#if ((UTT_NV_810V2 == FYES)|| (UTT_NV_830V2 == FYES)|| (UTT_NV_521G == FYES) || (UTT_NV_1220G == FYES) || (UTT_NV_3320G == FYES) || (UTT_U_1000N == FYES) || (UTT_NV_840E == FYES) || (UTT_NV_800 == FYES) || (UTT_NV_840D == FYES)|| (UTT_NV_518W == FYES)|| (UTT_NV_518 == FYES)|| (UTT_NV_518WPLUS == FYES)|| (UTT_NV_518WVPN == FYES)|| (UTT_NV_842W == FYES) || (UTT_NV_840G_INT == FYES))
#define MODULE_COUNT_IPSEC 7
#define MODULE_DIR_IPSEC "/lib/modules/2.6.21/kernel/"
const char *module_name[MODULE_COUNT_IPSEC]={
	"crypto/aes_generic.ko",
	"crypto/crypto_null.ko",
	"crypto/cryptd.ko",
	"crypto/unlzma.ko",
	"crypto/gf128mul.ko",
	"crypto/aes.ko",
	"crypto/pcbc.ko"
};
#elif (CONFIG_PPC_85xx == FYES)
#define MODULE_COUNT_IPSEC 4
#define MODULE_DIR_IPSEC "lib/modules/2.6.35/kernel/"
#define MODULE_DEL_DIR_IPSEC "/lib/modules/2.6.35/kernel/drivers/"
const char *module_name[MODULE_COUNT_IPSEC]={
	"crypto/aes_generic.ko",
	"crypto/crypto_null.ko",
	"crypto/cryptd.ko",
	"crypto/gf128mul.ko"
};
#elif ((CONFIG_RALINK_MT7620 == FYES) || (CONFIG_RALINK_MT7621 == FYES)  || (CONFIG_RALINK_MT7628 == FYES))
#define MODULE_COUNT_IPSEC 12
#define MODULE_DIR_IPSEC "lib/modules/2.6.36/kernel/"
#define MODULE_DEL_DIR_IPSEC "/lib/modules/2.6.36/kernel/drivers/"
const char *module_name[MODULE_COUNT_IPSEC]={
	"crypto/authenc.ko",
	"crypto/tcrypt.ko",
	"crypto/deflate.ko",
	"crypto/aes_generic.ko",
	"crypto/md5.ko",
	"crypto/des_generic.ko",
	"crypto/cbc.ko",
	"crypto/gf128mul.ko",
	"crypto/crypto_null.ko",
	"crypto/cryptd.ko",
	"crypto/ansi_cprng.ko",
	"crypto/hmac.ko"
};
#else
#define MODULE_COUNT_IPSEC 5
#define MODULE_DIR_IPSEC "/lib/modules/2.6.30.8/kernel/"
#define MODULE_DEL_DIR_IPSEC "/lib/modules/2.6.30.8/kernel/drivers/"
const char *module_name[MODULE_COUNT_IPSEC]={
	"crypto/aes_generic.ko",
	"crypto/crypto_null.ko",
	"crypto/cryptd.ko",
	"crypto/unlzma.ko",
	"crypto/gf128mul.ko"
};
#endif
/* insmod_modules_ipsec(void)
 * 当启动IPSec服务之前，
 * 首先加载内核编译模块
 * */
static void insmod_modules_ipsec(void)
{
    int ret=0, i=0;
#if 0
#if ((UTT_NV_810V2 != FYES) && (UTT_NV_521G != FYES) && (UTT_NV_1220G != FYES) && (UTT_NV_3320G != FYES) && (UTT_U_1000N != FYES) && (UTT_NV_840E != FYES) && (UTT_NV_800 != FYES) && (UTT_NV_840D != FYES)&& (UTT_NV_518W != FYES)&& (UTT_NV_842W != FYES)&& (UTT_NV_518 != FYES)&& (UTT_NV_518WPLUS != FYES)&& (UTT_NV_518WVPN != FYES) && (UTT_NV_840G_INT != FYES))
    ret = access(MODULE_DEL_DIR_IPSEC, 00);
    if(ret == 0){
	doSystem("rm -rf %s", MODULE_DEL_DIR_IPSEC);
    }
#endif
#endif
    IPSEC_PRINT("%s_%d: ret = %d\n",__func__,__LINE__,ret);
    for (i=0;i<MODULE_COUNT_IPSEC;i++){
	IPSEC_PRINT("%s-%d: modules = %s%s\n",__func__,__LINE__,MODULE_DIR_IPSEC,module_name[i]);
	doSystem("insmod %s%s",MODULE_DIR_IPSEC,module_name[i]);
    }
    return;
}
/* rmmod_modules_ipsec(void)
 * 当停止ipsec服务时，
 * 卸载内核编译模块
 * */
static void rmmod_modules_ipsec(void)
{
    int i=0;

    for (i=0;i<MODULE_COUNT_IPSEC;i++){
	IPSEC_PRINT("%s-%d: modules = %s%s\n",__func__,__LINE__,MODULE_DIR_IPSEC,module_name[i]);
	doSystem("rmmod %s%s",MODULE_DIR_IPSEC,module_name[i]);
    }
}
/*
 * ipsec 初始化
 */
static void ipsecInit(void) 
{
    addAllConfig();
#if 0
    createInitshFile();
    doSystem("chmod a+x /bin/createforipsec.sh");   /*change file authority*/
    doSystem("source createforipsec.sh");	    /*execute sh file*/
#endif
    doSystem("echo $PATH");
    /*开机检测ipsec实例数
     * 如果实例数不为0，则开启服务
     * 否则，默认服务是关闭的*/
    if(checkInstCount_ipsec() != 0) {
	insmod_modules_ipsec();
	doSystem("ipsec setup start");		    /*start openswan service*/
	sleep(1);
	IPSEC_PRINT("%s-%d: start\n",__func__,__LINE__);
	startUpDomainIP(1);
    }
}
/*
* ipsecUpdatePortMap
* 更新 ipsec 记录map
*/
static void ipsecUpdatePortMap(void) 
{
    MibProfileType mibType = MIB_PROF_IPSEC;
    ipsecConfProfile *prof = NULL;
    int i = 0, min = 0, max = 0;
    Uint16 webInstIndex;

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) 
    {
        prof = (ipsecConfProfile *)ProfGetInstPointByIndex(mibType, i);
	if ((ProfInstIsFree(prof) == 0) && (strncmp(prof->head.name, "ID", 2) == 0)) 
	{/*取出id后面的数字*/
	    webInstIndex = (Uint16)strtol(&(prof->head.name[2]), NULL, 10);/*取配置中的id号*/
	    if (webInstIndex != 0U) 
	    {
		setPortMap(&(nvramProfile->ipsecInstRec.instRecMap), webInstIndex);/*赋值给实例号*/
	    }
	}
    }

    return;
}
#define READ_BUF_SIZE 1024
long* find_pid_by_name( char* pidName, int *count)
{
    DIR *dir=NULL;
    struct dirent *next;
    long* pidList=NULL;
    int i=0;
    dir = opendir("/proc");
    if (!dir)
    {
	//perror_msg_and_die("Cannot open /proc");
	//fprintf(STDERR,"Cannot open /proc\n");
	return pidList;
    }
    while ((next = readdir(dir)) != NULL)
    {   
	FILE *status=NULL;
	char filename[READ_BUF_SIZE];
	char buffer[READ_BUF_SIZE];
	char name[READ_BUF_SIZE];
	/* Must skip "" since that is outside /proc */
	if (strcmp(next->d_name, "..") == 0)
	    continue;
	/* If it isn't a number， we don't want it */
	if (!isdigit(*next->d_name))
	    continue;
	sprintf(filename, "/proc/%s/status", next->d_name);
	if (! (status = fopen(filename, "r")) )
	{
	    continue;
	}
	//Read first line in /proc/?pid?/status
	if (fgets(buffer,READ_BUF_SIZE-1, status) == NULL)
	{
	    fclose(status);
	    continue;
	}
	fclose(status);
	// Buffer should contain a string like "Name： binary_name"a
	sscanf(buffer,"%*s %s", name);
	if ( name != NULL && name[0] != '\0')                           
	{
	    if (strcmp(name, pidName) == 0)
	    {
		pidList=realloc( pidList, sizeof(long) * (i+2));
		pidList[i++]=strtol(next->d_name, NULL, 0);
	    }
	}
    }
    closedir(dir);
    *count = i;
    #if 0
    if (pidList)
    { 
	pidList[i]=0;
    } 
    #endif
    return pidList;
}

/*
 * static void startUpDomainIP()
 * 开机启动更新检测任务
 * 任务: 检测远端网关域名对应IP是否变化
 */
static void startUpDomainIP(int flag)
{
    long *pid=NULL;
    int count=0, i=0;
    /* 先查找是否与相同的进程在运行
     * 如果存在，则先杀掉，在重新启动
     */
    pid = find_pid_by_name("uttUpDomain", &count);
    if(pid != NULL)
    {
	IPSEC_PRINT("%s-%d: count=%d\n",__func__,__LINE__,count);
	for(i=0;i<count;i++)
	{
	    doSystem("kill %ld", *(pid+i));	/*杀掉进程*/
	    IPSEC_PRINT("%s-%d: pid%d=%ld\n",__func__,__LINE__,i,*(pid+i));
	}
	free(pid);
    }
    /*启动*/
    if(flag == 1)
    doSystem("uttUpDomain ipsec &");
    return;
}
/*
 * init_IPSEC(int wanIndex)
 * invoked by wanUpDelay
 */
extern void init_IPSEC(int wanIndex)
{
#if 1
    MibProfileType mibipsec = MIB_PROF_IPSEC;
    ipsecConfProfile *ipsecProf = NULL;
    int max=0, min=0, i=0;
    int IfNum=-1;
    int flag=0;
    ProfInstLowHigh(mibipsec, &max, &min);
    for(i=min;i<max;i++)    
    {
	ipsecProf = (ipsecConfProfile *)ProfGetInstPointByIndex(mibipsec, i);
	if((ipsecProf!=NULL)&&(ProfInstIsFree(ipsecProf) == 0))
	{
	    IfNum=strtol(ipsecProf->profile, NULL, 10);
	    if(wanIndex==IfNum)
	    {
		flag=1;
		memset(ipsecProf->oldprofile, 0, sizeof(ipsecProf->oldprofile));
	    }
	}
    }
    if(flag==1)
#endif
    {
	addAllConfig();
	if(checkInstCount_ipsec() != 0) {
	    doSystem("ipsec setup restart");	/*start openswan service*/
	}
    }
}

extern void delete_ipsec_route(int wanIndex)
{
    MibProfileType mibipsec = MIB_PROF_IPSEC;
    ipsecConfProfile *ipsecProf = NULL;
    int max=0, min=0, i=0;
    int IfNum=-1;
#if 0
    char cmdStr_del[254]={0};
    char if_name[16]={0};
#endif
    ProfInstLowHigh(mibipsec, &max, &min);
    for(i=min;i<max;i++)    
    {
	ipsecProf = (ipsecConfProfile *)ProfGetInstPointByIndex(mibipsec, i);
	if((ipsecProf!=NULL)&&(ProfInstIsFree(ipsecProf) == 0))
	{
	    IfNum=strtol(ipsecProf->profile, NULL, 10);
	    if(wanIndex==IfNum)
	    {
		clearOneConfig(ipsecProf);
#if 0
		memset(cmdStr_del, 0, sizeof(cmdStr_del));
		memset(if_name, 0, sizeof(if_name));
		strcpy(if_name, getWanIfNamePPP(IfNum));
		if(strlen(ipsecProf->peerIP)!=0U)
		{
		    sprintf(cmdStr_del, "ip route del %s/32 dev %s ",ipsecProf->peerIP,if_name);
		} else {
		    sprintf(cmdStr_del, "ip route del %s/32 dev %s ",ipsecProf->peer,if_name);
		}
		doSystem(cmdStr_del);
		IPSEC_PRINT("%s-%d: cmdStr=%s\n",__func__,__LINE__,cmdStr_del);
#endif
	    }
	}
    }
}

static void clearConfigFile()
{
    FILE *fp = NULL;
    FILE *fp_sec = NULL;
    /*
     * 初始化配置
     */
    IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
    if((fp = fopen(IPSEC_CONF_FILE,"w+")) == NULL)
    {
	IPSEC_PRINT("open ipsec.conf is error!\n");
	return;
    }
    if((fp_sec = fopen(IPSEC_SECRETS_FILE,"w+")) == NULL)
    {
	IPSEC_PRINT("open ipsec.secrets is error!\n");
	return;
    }
    IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);

    /*global config*/
    fprintf(fp,"version 2.0\n");
    fprintf(fp,"config setup\n");
    fprintf(fp,"    oe=off\n");
    fprintf(fp,"    protostack=netkey\n");
    fprintf(fp,"    nat_traversal=yes\n");

    fclose(fp_sec);
    fclose(fp);
    return;
}

extern void changeAggresModetoSecLine(int wanIndex)
{
    MibProfileType mibipsec = MIB_PROF_IPSEC;
    ipsecConfProfile *ipsecProf = NULL;
    InterfaceProfile *profIf = NULL;
    int max=0, min=0, i=0, j=0, k=0;
    int IfNum=-1;
    Uint32 wanCount = 0;
    int aliveIndex = 0;
    int flag = 0, maxCount=0;
    int exist_flag = 0;
#if (VPN_ITEM_CONTROL == FYES)
    char name_back[MAX_VPN_ITEMS_NUM][UTT_INST_NAME_LEN+1U];
#else
    char name_back[MAX_IPSEC_PROFILES][UTT_INST_NAME_LEN+1U];
#endif
    char new_profile[UTT_INST_NAME_LEN+1U];
    /*get inst min and max index*/
    memset(name_back, 0, sizeof(name_back));
    memset(new_profile, 0, sizeof(new_profile));
    wanCount = getWanSlotCount();
    for(j=1; j <= wanCount; j++) {
	if(j!=wanIndex){
	    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, j);
	    //if (profIf->runningLineMode == 1) {
	    if(_getWanLinkState(j) == 1) {
		aliveIndex = j;
		break;
	    }
	}
    }
    IPSEC_PRINT("%s-%d: aliveindex=%d\n",__func__,__LINE__,aliveIndex);
    ProfInstLowHigh(mibipsec, &max, &min);
    for(i=min;i<max;i++)
    {
	ipsecProf = (ipsecConfProfile *)ProfGetInstPointByIndex(mibipsec, i);
	if((ipsecProf!=NULL)&&(ProfInstIsFree(ipsecProf) == 0)){
	    if((strstr(ipsecProf->profile, "PS_") == NULL)&&(strstr(ipsecProf->profile, "PC_") == NULL)){
		IfNum=strtol(ipsecProf->profile, NULL, 10);	    /*获取绑定的接口索引*/
		if((wanIndex==IfNum)&&(ipsecProf->connType == DYNAMIC_TO_GATEWAY)) {
		    exist_flag = 1;
		    break;
		}
	    }
	}
    }
    if((aliveIndex>=1)&&(aliveIndex<=wanCount)&&(exist_flag == 1)){
    IfNum = -1;
    clearConfigFile(); /*先清空配置文件*/
    sprintf(new_profile, "%d", j);
    ProfInstLowHigh(mibipsec, &max, &min);
    for(i=min;i<max;i++)    
    {
	flag = 0;
	ipsecProf = (ipsecConfProfile *)ProfGetInstPointByIndex(mibipsec, i);
	if((ipsecProf!=NULL)&&(ProfInstIsFree(ipsecProf) == 0))
	{
	    if((strstr(ipsecProf->profile, "PS_") == NULL)&&(strstr(ipsecProf->profile, "PC_") == NULL)){
		IfNum=strtol(ipsecProf->profile, NULL, 10);	    /*获取绑定的接口索引*/
		if((wanIndex==IfNum)&&(ipsecProf->connType == DYNAMIC_TO_GATEWAY)) {
		    IPSEC_PRINT("%s-%d: ifNum=%d, is down\n",__func__,__LINE__,IfNum);
			strcpy(ipsecProf->oldprofile, ipsecProf->profile);
			strcpy(ipsecProf->profile, new_profile);
			IPSEC_PRINT("%s-%d: new_profile=%s,ipsec_profile=%s\n",__func__,__LINE__,new_profile,ipsecProf->profile);
			strcpy(name_back[k], ipsecProf->head.name);
			maxCount++;
			flag = 1;
		    }
		
		}
	    addOneConfig(ipsecProf);
	    if(flag==1){
		ipsecProf->sa_conn=0U;
#if(VPN_CONTROL == FYES)
		if(nvramProfile->vpnProfile.count<=0)
		{
                syslog(LOG_INFO, "no free vpn session.");
		} else
#endif
		{
		doSystem("ipsec auto --add \"%s\"",ipsecProf->head.name);	/**/
		doSystem("ipsec auto --rereadsecrets");	/**/
		doSystem("ipsec auto --up \"%s\" &",ipsecProf->head.name);	/*启动隧道连接*/
		IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
		}
	    }
	}
    }
    for(k = 0;k < maxCount;k++)
    {
	ipsecProf = (ipsecConfProfile *)ProfGetInstPointByIndex(mibipsec, k);
	if(strcmp(name_back[k], ipsecProf->head.name)==0){
	if((strstr(ipsecProf->profile, "PS_") == NULL)&&(strstr(ipsecProf->profile, "PC_") == NULL)){
	    if((strtol(ipsecProf->oldprofile,NULL,10) == wanIndex)&&(ipsecProf->connType == DYNAMIC_TO_GATEWAY)) {
		strcpy(ipsecProf->profile,ipsecProf->oldprofile);
			IPSEC_PRINT("%s-%d: name_back=%sipsecProf.name=%s,ipsec_profile=%s\n",__func__,__LINE__,name_back[k],ipsecProf->head.name,ipsecProf->profile);
	    }
	}
	}
    }
    ProfWiteToFlash();
    }
    return;
}
/*
 * 初始化函数
 */
extern void funInitIPSec(void) {
    registerForProfileUpdates(MIB_PROF_IPSEC, (Pointer)NULL, (ProfileUpdateFnc)ipsecConfProfileChange);
    ipsecInit();		/*ipsec 初始化*/
    IPSEC_PRINT("%s-%d: \n",__func__,__LINE__);
    ipsecUpdatePortMap(); 
#if 0
    /*获取绑定wan口网关*/
    char gwAddr[20];
    getInterfaceGWaddr(1, gwAddr);
    char ip[16]={0};
    getDomainNameIP("www.baidu.com", ip);
#endif
    return;
}
#endif
