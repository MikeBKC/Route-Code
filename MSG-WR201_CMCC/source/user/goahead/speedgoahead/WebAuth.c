#include <stdlib.h>
#include <sys/sysinfo.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <regex.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"
#include "spdComFun.h"
#include "comExeFun.h"
#include "flash_api.h"
#include <net/if_arp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <dirent.h>
#if (WEB_AUTH == FYES) || (WIFIDOG_MAKE_MONEY == FYES) || (CWMP == FYES)
#include "utt_conf_define.h"
#ifdef SWORD_DEBU
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif
#if (WEB_AUTH_SHARE == FYES)
#define DEBUG 0
#endif

#if (UTT_KNOWIFI == FYES)
#define UTT_KNOWIFI_WHITE_LIST	0x20
#define UTT_KNOWIFI_WHITE_LIST_DOMAIN "*.qingxueba.com"
#define UTT_KNOWIFI_WHITE_LIST_OTHER1 "qingxueba.com"
#define UTT_KNOWIFI_WHITE_LIST_OTHER2 "knowifi.me"
#endif

#if (WIFIDOG_AUTH == FYES) || (WIFIDOG_MAKE_MONEY == FYES) || (CWMP == FYES)
#define WDCTL_SOCK    "/tmp/wdctl.sock"
extern void WifiDog_IpDown(char *ip)
{
    int     sock;
    struct sockaddr_un      sa_un;
    char    request[52];
    char    buffer[4096];
    int len = 0, written = 0;

    if((ip != NULL)&&((strlen(ip)>=7)&&(strlen(ip)<=15))) {
	/* Connect to socket */
	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sock >= 0) {
	memset(&sa_un, 0, sizeof(sa_un));
	sa_un.sun_family = AF_UNIX;
	strncpy(sa_un.sun_path, WDCTL_SOCK, (sizeof(sa_un.sun_path) - 1));

	if (connect(sock, (struct sockaddr *)&sa_un, 
	    strlen(sa_un.sun_path) + sizeof(sa_un.sun_family))) {
	    close(sock);
	    return ;
	}

	memset(request, 0, 52);
	snprintf(request, 52,"ipDown %s;\r\n\r\n", ip);
	len = 0;

	while (len != strlen(request)) {
	    written = write(sock, (request + len), strlen(request) - len);
	    if (written == -1) {
		fprintf(stderr, "Write to wifidog failed.\n");
		close(sock);
		return;
	    }
	    len += written;
	}

#if 0
	len = 0;
	while ((len = read(sock, buffer, sizeof(buffer))) > 0) {
	    buffer[len] = '\0';
	    SWORD_PRINTF("wifi ip down %s",buffer);
	}
#endif

	sleep(3);
	shutdown(sock, 2);
	close(sock);
	}
    }
    return ;
}


void kill_process(char *name)
{
#if CONFIG_PPC_85xx
    /*p1010-c ����popen�ᵼ�º���һֱ�������
     * ����֮�����Ϣ���д����޷�ִ�У�����profilechange�����
     * */
    DIR *dir=NULL;
    struct dirent *next;

    if(name!=NULL) {
	dir = opendir("/proc");
	if (!dir)
	{
	    //perror_msg_and_die("Cannot open /proc");
	    //fprintf(STDERR,"Cannot open /proc\n");
	    return ;
	}
	while ((next = readdir(dir)) != NULL)
	{   
	    FILE *status=NULL;
	    char filename[WIFI_READ_BUF_SIZE];
	    char buffer[WIFI_READ_BUF_SIZE];
	    char pidName[WIFI_READ_BUF_SIZE];
	    /* Must skip "" since that is outside /proc */
	    if (strcmp(next->d_name, "..") == 0)
		continue;
	    /* If it isn't a number�� we don't want it */
	    if (!isdigit(*next->d_name))
		continue;
	    sprintf(filename, "/proc/%s/status", next->d_name);
	    if (! (status = fopen(filename, "r")) )
	    {
		continue;
	    }
	    //Read first line in /proc/?pid?/status
	    if (fgets(buffer,WIFI_READ_BUF_SIZE-1, status) == NULL)
	    {
		fclose(status);
		continue;
	    }
	    fclose(status);
	    // Buffer should contain a string like "Name�� binary_name"a
	    sscanf(buffer,"%*s %s", pidName);
	    if ( pidName[0] != '\0')                           
	    {
		if (strcmp(name, pidName) == 0)
		{
			doSystem("kill -9 %s ",next->d_name);
			SWORD_PRINTF("%s-%d: kill -9 %s ",__func__,__LINE__,next->d_name);
		}
	    }
	}
	closedir(dir);
    }
#else
    FILE *fp;
    char *p = NULL;
    char command[20];
    char buffer[100];
    char pid[10];

    if(name != NULL) {
    fp = popen("ps", "r");/*ִ������*/
    if (fp != NULL) {/*ִ�гɹ�*/
	memset(buffer, 0, 50);
	while (fgets(buffer, sizeof(buffer), fp) != NULL) {
	    if (((p = strstr(buffer,name)) != NULL)) {
		memset(pid, 0, 10);
		memset(command, 0, 20);
		sscanf(buffer, "%s", pid);
		SWORD_PRINTF("%s-%d:pid = %s\n", __func__, __LINE__, pid);
		sprintf(command, "kill -9 %s", pid);
		system(command);
	    }
	}
	pclose(fp);
    }
    }
#endif
    return;
}
#endif
#endif

#if (WEB_AUTH == FYES)
#if (NOTICE == FYES)
extern int isPoeNTOn(void);
#endif
#ifdef CONFIG_UTT_NETFILTER
#include <linux/netfilter_ipv4/utt_netfilter.h>
extern int spdNfNlSock;
#endif

#if 0
extern int initRuleToPreDrop(char * oldip, char *newip);
#endif
static void webauthProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const WebAuthProfile *oldProfileDataPtr, const WebAuthProfile *newProfileDataPtr );
static void webauthglobalProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const WebAuthGlobalProfile *oldProfileDataPtr, const WebAuthGlobalProfile *newProfileDataPtr );
static void webauthRuleAdd(void);
static void webauthRuleDel(void);
static void webauthIpDel(void);
static void webauthUpdatePortMap(void); 
static void webauthInit(void);
extern int readContactFromFile(char *content, const char *path,int size);

#if (WEBAUTH_ADVANCE == FYES)
static void writeUserPageToRam();
static void saveWebAuthDefPage();
#endif
extern void webauthAddexceptIpGroup(void);
static int  checkexceptIpGroupMem(WebAuthGlobalProfile *oldprof, WebAuthGlobalProfile *newprof);
static void checkIpIfOnLine(void);
#if (WEBAUTH_ACCOUNT == FYES)
extern struct arpreq arpLan;
extern int getWebAuthAccNum();
extern void setWebAuthAccNum(int num);
#endif
#ifdef CONFIG_UTT_NETFILTER
#if (WEB_AUTH_STALE == FYES)
static void uttNfNlStaleTimeAdd(Uint32 time);
#endif
#endif

#if (WIFIDOG_AUTH == FYES)
extern Uint32 getWebauthType(void);
#if (WHITELIST == 1)
static void whiteListProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const WhiteListProfile *oldProfileDataPtr, const WhiteListProfile *newProfileDataPtr );

#ifdef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
/*�Ƶ��ں�ʵ�֣�ͨ��iptables��WiFiMatchƥ��ģ��ʵ��,�滻else�е�ʵ�ַ�ʽ*/
static void wifidogDnsAddOrDel(WhiteListProfile *prof, ProfChangeType changeType);
static void wifidogDnsInit(ProfChangeType changeType);
#else
static void whiteList_add(WhiteListProfile *whiteListProfile);
static void whiteList_del(WhiteListProfile* whiteListProfile);
static void getDomainNameIP(char *domain,char opt);
static void whiteList_delAll(WhiteListProfile *whiteListProfile);
#endif
#endif
#define WIFI_READ_BUF_SIZE 1024
void check_wifidog_by_name( char* pidName)
{
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;
    DIR *dir=NULL;
    struct dirent *next;
    long pidList[10];
    int i=0;
    int count = 0;
    profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
    /*��ȡweb��֤ȫ�ֽṹ����*/
    if((profG != NULL) &&  (profG->head.active == TRUE) && (profG->enabled == 1U))
    {
	if(getWebauthType() == 2) {
    
	    dir = opendir("/proc");
	    if (!dir)
	    {
		//perror_msg_and_die("Cannot open /proc");
		//fprintf(STDERR,"Cannot open /proc\n");
		return ;
	    }
	    memset(pidList, 0, sizeof(pidList));
	    while ((next = readdir(dir)) != NULL)
	    {   
		FILE *status=NULL;
		char filename[WIFI_READ_BUF_SIZE];
		char buffer[WIFI_READ_BUF_SIZE];
		char name[WIFI_READ_BUF_SIZE];
		/* Must skip "" since that is outside /proc */
		if (strcmp(next->d_name, "..") == 0)
		    continue;
		/* If it isn't a number�� we don't want it */
		if (!isdigit(*next->d_name))
		    continue;
		sprintf(filename, "/proc/%s/status", next->d_name);
		if (! (status = fopen(filename, "r")) )
		{
		    continue;
		}
		//Read first line in /proc/?pid?/status
		if (fgets(buffer,WIFI_READ_BUF_SIZE-1, status) == NULL)
		{
		    fclose(status);
		    continue;
		}
		fclose(status);
		// Buffer should contain a string like "Name�� binary_name"a
		sscanf(buffer,"%*s %s", name);
		if ( name != NULL && name[0] != '\0')                           
		{
		    if (strcmp(name, pidName) == 0)
		    {
			if(i<10) {
			    pidList[i]=strtol(next->d_name, NULL, 0);
			}
			i++;
		    }
		}
	    }
	    closedir(dir);
	    count = i;
	    //syslog(LOG_INFO, "check wifidog.\n");
	    if(count <= 0)
	    {

		syslog(LOG_INFO, "++++++++++++++++++++\n");
		syslog(LOG_INFO, "UTT WITI AUTH: utt wifi auth restarting...\n");
		doSystem("wifidog-init stop ");
		sleep(2);
		kill_process("wifidog");
		kill_process("wdctl");
#if 0
		/*flush iptables rule*/
		doSystem("iptables -t nat -D PREROUTING -i %s -j WiFiDog_%s_Outgoing",getLanIfName(),getLanIfName());
		doSystem("iptables -t nat -F WiFiDog_%s_Outgoing",getLanIfName());
		doSystem("iptables -t nat -F WiFiDog_%s_WIFI2Internet",getLanIfName());
		doSystem("iptables -t nat -F WiFiDog_%s_AuthServers",getLanIfName());
		doSystem("iptables -t nat -X WiFiDog_%s_Outgoing",getLanIfName());
		doSystem("iptables -t nat -X WiFiDog_%s_WIFI2Internet",getLanIfName());
		doSystem("iptables -t nat -X WiFiDog_%s_AuthServers",getLanIfName());
	
		doSystem("iptables -t filter -D FORWARD -i %s -j WiFiDog_%s_WIFI2Internet",getLanIfName(),getLanIfName());
		doSystem("iptables -t filter -F WiFiDog_%s_WIFI2Internet",getLanIfName());
		doSystem("iptables -t filter -F WiFiDog_%s_AuthServers",getLanIfName());
		doSystem("iptables -t filter -F WiFiDog_%s_Unknown",getLanIfName());
		doSystem("iptables -t filter -X WiFiDog_%s_WIFI2Internet",getLanIfName());
		doSystem("iptables -t filter -X WiFiDog_%s_AuthServers",getLanIfName());
		doSystem("iptables -t filter -X WiFiDog_%s_Unknown",getLanIfName());
#endif
		sleep(1);
		doSystem("wifidog-init start ");
	    }
	}
	
    }
    return ;
}



#if 0
/*�����ã������ļ������������ҿɸ���*/
void wechatFree_wifidog()
{
    char *file = "/etc_ro/l7-protocols/Wechat.pat";
    char *rule = "(^\\x10\\x10\\x01.+\\x06\\x01$)|(^.\\x10\\x01.+\\x5f\\x43\\x4e$)|(^GET \/mmhead\/.+gzip\\x0d\\x0a\\x0d\\x0a$)|(^POST http\:\/\/short.weixin.qq.com)";
    FILE *fp;

    fp = fopen(file, "w");
    if (fp == NULL)
    {
	fprintf(stderr,"%s %d: cannot open file %s\n", __func__, __LINE__, file);
	return;
    }
    fprintf(fp, "Wechat\n%s", rule);
    fclose(fp);
    return ;
}
#endif

/*
 * getWebauthType
 * ��ȡ��֤��ʽ
 * ����ֵ��
 *	1;  ������֤
 *	2:  Զ����֤
 * */
extern Uint32 getWebauthType()
{
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;
    Uint32 ret = 1U;

    profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
    /*��ȡweb��֤ȫ�ֽṹ����*/
    if((profG != NULL) && (ProfInstIsFree(profG) == 0))
    {
	if((profG->authType != 1U) && (profG->authType != 2U)) {
	    ret = 1U;	    /*Ĭ��Ϊ������֤*/
	} else {
	    ret = profG->authType;	/*������֤��ʽ*/
	}
    }
    return ret;
}

#define TABLE_WIFIDOG_WIFI_TO_INTERNET	"WiFiDog_%s_WIFI2Internet"
#if 0
#define TABLE_WIFIDOG_PREROUTING	"WiFiDog_%s_Prerouting"
#define TABLE_WIFIDOG_OUTGOING		"WiFiDog_%s_Outgoing"
#define TABLE_WIFIDOG_INCOMING		"WiFiDog_%s_Incoming"
#define TABLE_WIFIDOG_WIFI_TO_INTERNET	"WiFiDog_%s_WIFI2Internet"

/* wifidogAddExceptIpGroup
 * ��ӻ���ɾ��wifidog�����ַ�����
 * param:
 *	exceptIpGrp -  �����ַ��
 *	action	    -  Iptables_INS orIptables_DEL
 * */
static void wifidogAddExceptIpGroup(char *exceptIpGrp, char action)
{
    
    if((action == Iptables_INS) && (exceptIpGrp != NULL) && (strlen(exceptIpGrp)>0)) {
	/*PREROUTING*/
	printf("================================================ \n");
	printf("add firewall iptables rule for wifidog except ip group\n");
	printf("iptables -t mangle -I " TABLE_WIFIDOG_PREROUTING " 1 -m set --set %s src -j RETURN", getLanIfName(), exceptIpGrp);
	doSystem("iptables -t mangle -I " TABLE_WIFIDOG_PREROUTING " 1 -m set --set %s src -j RETURN", getLanIfName(),exceptIpGrp);
	doSystem("iptables -t nat -I " TABLE_WIFIDOG_OUTGOING " 1 -m set --set %s src -j RETURN",getLanIfName(), exceptIpGrp);
	/*POSTROUTING*/
	doSystem("iptables -t mangle -I " TABLE_WIFIDOG_INCOMING " 1 -m set --set %s src -j RETURN",getLanIfName(), exceptIpGrp);
	/*FORWARD*/
	doSystem("iptables -t filter -I " TABLE_WIFIDOG_WIFI_TO_INTERNET " 1 -m set --set %s src -j RETURN",getLanIfName(), exceptIpGrp);
    } else if((action == Iptables_DEL) && (exceptIpGrp != NULL) && (strlen(exceptIpGrp)>0)) {
	printf("================================================ \n");
	printf("del firewall iptables rule for wifidog free except ip group \n");
	/*PREROUTING*/
	doSystem("iptables -t mangle -D " TABLE_WIFIDOG_PREROUTING " -m set --set %s src -j RETURN",getLanIfName(), exceptIpGrp);
	doSystem("iptables -t nat -D " TABLE_WIFIDOG_OUTGOING " -m set -set %s src -j RETURN",getLanIfName(), exceptIpGrp);
	/*POSTROUTING*/
	doSystem("iptables -t mangle -D " TABLE_WIFIDOG_INCOMING " -m set --set %s src -j RETURN",getLanIfName(), exceptIpGrp);
	/*FORWARD*/
	doSystem("iptables -t filter -D " TABLE_WIFIDOG_WIFI_TO_INTERNET " -m set --set %s src -j RETURN",getLanIfName(), exceptIpGrp);
    } else {
	printf("wifi auth except operate type error.\n");   
    }
}
#endif
#if (SMS_ENABLE == 1)
void usbSwitch(int flag)
{
    if(flag == 0) {
    doSystem("killall uttusbswitch");
    doSystem("killall smssend");
    } else {
    doSystem("usb_modeswitch.sh");
    sleep(3);
    doSystem("uttusbswitch &");
    }

    return;
}
#endif
/*
 * wifidogInit()
 * wifidog ��ʼ��
 * */
extern void wifidogInit()
{
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;

    profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
    /*���ؿ���,����wifidog*/
    if((profG != NULL) && (profG->head.active == TRUE) && (profG->enabled == 1U))
    {

#if CONFIG_PPC_85xx
	kill_process("uttUpDomainwifi");
#else
	kill_process("uttUpDomainwifi wifi");
#endif
	sleep(1);
	doSystem("uttUpDomainwifi wifi &");
#if (SMS_ENABLE == 1)
	if(profG->smsEnable == 1){
	usbSwitch(0);
	sleep(1);
	usbSwitch(1);
	}
#endif
	doSystem("wifidog-init start ");
#ifdef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
	/*������������ʼ��
	 * �Ƶ��ں�ʵ�֣�ͨ��iptables��WiFiMatchƥ��ģ��ʵ��*/
	wifidogDnsInit(PROF_CHANGE_ADD);
#endif
    }
    return;
}

/*
 * wifidogExit()
 * wifidog exit
 * kill wifidog 
 * */
extern void wifidogExit()
{
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;

    profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
    /*�ر� wifidog*/
    if(profG != NULL)
    {
#ifdef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
	wifidogDnsInit(PROF_CHANGE_DELALL); /*�������������������ɾ���ں�����*/
#endif
	doSystem("wifidog-init stop ");
	sleep(1);
	kill_process("wifidog");
	kill_process("wdctl");
#if (SMS_ENABLE == 1)
	usbSwitch(0);
#endif
#if CONFIG_PPC_85xx
	kill_process("uttUpDomainwifi");
#else
	kill_process("uttUpDomainwifi wifi");
#endif
    }
    return ;
}

/*vlan���ñ仯����wifidog����
 * ��vlanConfig.c ����*/
int wifiUpdateRule_VlanConfig(char action,int index,unsigned int ip, int cidr)
{
    struct in_addr addrS;
    char *ipStr = NULL;
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;

    profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
    /*���ؿ���,����wifidog*/
    if((profG != NULL) && (profG->head.active == TRUE) && (profG->enabled == 1U))
    {
	memset(&addrS, 0, sizeof(addrS));
	addrS.s_addr = ip;
	ipStr = inet_ntoa(addrS);

	if(action == Iptables_INS)
	{
	    if(ipStr != NULL) { /*�½�ipset��*/
		doSystem("ipset -q -N %s%d macipmap --network %s/%d","wifi_client_group",index,ipStr,cidr);
#if (WIFIDOG_MAC_AUTH == FYES)
		doSystem("ipset -q -N %s%d macipmap --network %s/%d ",WIFI_GROUP_BLACK_MAC,index,ipStr,cidr);
#ifdef WIFI_PRE_REDIRECT
		doSystem("iptables -t filter -%c " TABLE_WIFI_PRE_REDIRECT " 2 -m set --set %s%d src -j DROP",action,WIFI_GROUP_BLACK_MAC,index);
#else
		doSystem("iptables -t nat -%c " TABLE_WIFIDOG_WIFI_TO_INTERNET " 2 -m set --set %s%d src -j DROP",action,getLanIfName(),WIFI_GROUP_BLACK_MAC,index);
		doSystem("iptables -t filter -%c " TABLE_WIFIDOG_WIFI_TO_INTERNET " 2 -m set --set %s%d src -j DROP", action,getLanIfName(),WIFI_GROUP_BLACK_MAC,index);
#endif
#endif

#ifdef WIFI_PRE_REDIRECT
		doSystem("iptables -t filter -%c " TABLE_WIFI_PRE_REDIRECT " 2 -m set --set %s%d src -j RETURN",action,"wifi_client_group",index);
#if (VLAN_CONFIG == FYES)
		doSystem("ip route add table 1 %s/%d dev %s.%d proto kernel  scope link",ipStr,cidr,getLanIfName(),index-4);
#endif
#else
		doSystem("iptables -t nat -%c " TABLE_WIFIDOG_WIFI_TO_INTERNET " 2 -m set --set %s%d src -j RETURN",action,getLanIfName(),"wifi_client_group",index);
		doSystem("iptables -t filter -%c " TABLE_WIFIDOG_WIFI_TO_INTERNET " 2 -m set --set %s%d src -j RETURN ", action,getLanIfName(),"wifi_client_group",index);
#endif
	    }
	}
	if(action == Iptables_DEL)
	{
#ifdef WIFI_PRE_REDIRECT
#if (VLAN_CONFIG == FYES)
	    doSystem("ip route del table 1 %s/%d dev %s.%d proto kernel  scope link",ipStr,cidr,getLanIfName(),index-4);
#endif
	    doSystem("iptables -t filter -%c " TABLE_WIFI_PRE_REDIRECT " -m set --set %s%d src -j RETURN",action,"wifi_client_group",index);
#else
	    doSystem("iptables -t nat -%c " TABLE_WIFIDOG_WIFI_TO_INTERNET " -m set --set %s%d src -j RETURN",action,getLanIfName(),"wifi_client_group",index);
	    doSystem("iptables -t filter -%c " TABLE_WIFIDOG_WIFI_TO_INTERNET " -m set --set %s%d src -j RETURN ",action,getLanIfName(),"wifi_client_group",index);
#endif
	    doSystem("ipset -q -F %s%d ","wifi_client_group",index);
	    doSystem("ipset -q -X %s%d ","wifi_client_group",index);
#if (WIFIDOG_MAC_AUTH == FYES)
#ifdef WIFI_PRE_REDIRECT
	    doSystem("iptables -t filter -%c " TABLE_WIFI_PRE_REDIRECT "  -m set --set %s%d src -j DROP",action,WIFI_GROUP_BLACK_MAC,index);
#else
	    doSystem("iptables -t nat -%c " TABLE_WIFIDOG_WIFI_TO_INTERNET " -m set --set %s%d src -j DROP",action,getLanIfName(),WIFI_GROUP_BLACK_MAC,index);
	    doSystem("iptables -t filter -%c " TABLE_WIFIDOG_WIFI_TO_INTERNET " -m set --set %s%d src -j DROP",action,getLanIfName(),WIFI_GROUP_BLACK_MAC,index);
#endif
	    doSystem("ipset -q -F %s%d ",WIFI_GROUP_BLACK_MAC,index);
	    doSystem("ipset -q -X %s%d ",WIFI_GROUP_BLACK_MAC,index);
#endif
	}
    }    
    return 0;
}

static void WifiDogExceptIpgroup_update(const char *ipgroup)
{
	int	sock;
	struct sockaddr_un	sa_un;
	char	request[52];
	char	buffer[4096];
	int len = 0, written = 0;
	
	/* Connect to socket */
	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	memset(&sa_un, 0, sizeof(sa_un));
	sa_un.sun_family = AF_UNIX;
	strncpy(sa_un.sun_path, WDCTL_SOCK, (sizeof(sa_un.sun_path) - 1));

	if (connect(sock, (struct sockaddr *)&sa_un, 
			strlen(sa_un.sun_path) + sizeof(sa_un.sun_family))) {
		close(sock);
		return ;
	}

	memset(request, 0, 52);
	snprintf(request, 52,"exceptipgroup %s;\r\n\r\n", ((ipgroup!=NULL)?ipgroup:""));
	len = 0;
	while (len != strlen(request)) {
		written = write(sock, (request + len), strlen(request) - len);
		if (written == -1) {
			fprintf(stderr, "Write to wifidog failed.\n");
			close(sock);
			return;
		}
		len += written;
	}

#if 0
	len = 0;
	while ((len = read(sock, buffer, sizeof(buffer))) > 0) {
		buffer[len] = '\0';
		printf("wifi except ip group update %s",buffer);
	}
#endif

	sleep(3);
	shutdown(sock, 2);
	close(sock);
	return ;
}

/*
 * ����ʱ���������wifidog�����ʼ������web��֤��һЩ����
 * */
static void initWebAuth_start() 
{
    char buf[WEB_AUTH_CONTACT_LEN + 1];
    char buf2[WEB_AUTH_PICTURE_URL_LEN + 1];
    MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *prof = NULL;
#if (WEBAUTH_ACCOUNT == FYES)
    strcpy(arpLan.arp_dev, getLanIfName());
#endif
    prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
    if(ProfInstIsFree(prof) == 0)
    {
#if (WEBAUTH_ADVANCE == FYES)
	saveWebAuthDefPage();
	if(prof->enDefPage == 0)/*������Ĭ��web��֤ҳ��,ʹ���û��ϴ���ҳ��*/
	{
		writeUserPageToRam();
	}
	if(((prof->enpicture == 1)||(prof->enpicture == 3))&&(prof->picName[0] != '\0')&&(strlen(prof->picName) > 4))
	{
#if (CONFIG_X86_CPU == 1)
        doSystem("cp /ff/%s /etc_ro/web/images/",prof->picName);
#else
#endif
	}
#endif
	memset(buf,0,sizeof(buf));
	readContactFromFile(buf, WEB_AUTH_CONTACT_PATH,WEB_AUTH_CONTACT_LEN + 1);
	strncpy(prof->contact, buf, WEB_AUTH_CONTACT_LEN + 1);
	memset(buf2,0,sizeof(buf2));
	readContactFromFile(buf2, WEB_AUTH_PICURL_PATH, WEB_AUTH_PICTURE_URL_LEN + 1);
    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
	strncpy(prof->pictureUrl, buf2,WEB_AUTH_PICTURE_URL_LEN + 1);
#ifdef CONFIG_UTT_NETFILTER
#if (WEB_AUTH_STALE == FYES)
	if((prof->enabled == 1U)) {
	    if(prof->staleT>=5U) {
		uttNfNlStaleTimeAdd(prof->staleT);
	    }
	}
#endif
#endif
    }
}

#endif

#ifdef CONFIG_UTT_NETFILTER
#if (WEB_AUTH_STALE == FYES)
static void uttNfNlStaleTimeAdd(Uint32 time)
{
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;

    UttNlNfHashConf conf;
    memset(&conf, 0, sizeof(UttNlNfHashConf));
    conf.msgHead.nlType = UTT_NLNF_HASH;
    conf.confType = UTT_NFCONF_ADD;
    conf.staleEn = 1;
    profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
    /*���ؿ���,����wifidog*/
    if((profG != NULL) && (profG->head.active == TRUE) && (profG->enabled == 1U))
    {
    conf.staleTime =  time;
#if(WIFIDOG_AUTH == FYES)
    if(getWebauthType() == 2) {
    conf.trigger =  1U<<UTT_NF_HASH_WIFIDOG_DES_NOTIFY;
    } else {
    conf.trigger =  0U;
    }
#endif
    } else {
    conf.staleTime = 5U;
    conf.trigger =  0U;
    }
    SWORD_PRINTF(stderr,"%s-%d: conf.trigger: %u\n",__func__,__LINE__,conf.trigger);
    uttNfNlSend(spdNfNlSock, (char *)&conf, sizeof(conf), 0, 0);/*������Ϣ���ں�*/
    return;
}
#endif
#endif

#if (WEBAUTH_ACCOUNT == FYES)
/*********************************************************************
 * ���ܣ�ͳ�ƿ���web��֤�Ʒѹ��ܵ��û�����
 * ������2013-2-26
 ********************************************************************/
static int countWebAuthAccNum()
{
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;
    MibProfileType mibType = MIB_PROF_WEB_AUTH;
    WebAuthProfile *prof = NULL;
    int i = 0;
    int min = 0, max = 0;
    int num = 0;

    profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
    /*web��֤�����������õ��Ʒ�ģʽ����0*/
    if((profG != NULL) && (ProfInstIsFree(profG) == 0) && (profG->enabled != 0))
    {
	    ProfInstLowHigh(mibType, &max, &min);
	    for(i = min;i < max;i++)
	    {
		prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
		if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->timeingPatternEn == FUN_ENABLE))
		{
			num++;
		}
	    }
	    return num;
    }
    return 0;
}

/*******************************************************************
 * ���ܣ�ͳ�ƿ���web��֤�Ʒ�ģʽ���û������������Ƿ���дflash�ƻ�����
 * ������2013-02-27
 *******************************************************************/
static void startWtFlashTaskScheduler(void)
{
    int bak = 0;
    int num = 0;
    bak = getWebAuthAccNum();
    num = countWebAuthAccNum();/*�õ�ʹ�üƷѹ����û�����*/
    if ((num == 0) && (bak != 0))/*����Ҫ�Ʒѣ�ɾʱдflash�ƻ�����*/
    {
	doSystem("cron-init.sh del flashWrite");
    }
    else if ((num != 0) && (bak == 0))/*�¿����Ʒѹ��ܣ����дflash�ƻ�����*/
    {
	doSystem("cron-init.sh del flashWrite 1;cron-init.sh add \"0 */6 * * * flashWrite\"");
    }
    setWebAuthAccNum(num);/*���õ�ǰ����*/
    return;
}

#endif

#if (WEBAUTH_AUTO == FYES)
/********************************************************************
 ** ������ wechatFree
 ** ���ܣ� ��֤�ɹ�ǰ��ͨ΢��
 ** ������ 2014-01-02
 ** ������ flag: 1,���ӹ��� 0��ȥ������
 ** ���أ� ��
 ** ����� ��
 ** ���ߣ� ���Ĳ� 
 *********************************************************************/ 
void wechatFree(int flag)
{
    char *file = "/etc_ro/l7-protocols/Wechat.pat";
    char *rule = "(^\\x10\\x10\\x01.+\\x06\\x01$)|(^.\\x10\\x01.+\\x5f\\x43\\x4e$)|(^GET \/mmhead\/.+gzip\\x0d\\x0a\\x0d\\x0a$)|(^POST http\:\/\/short.weixin.qq.com)";
    FILE *fp;
    
    if (flag)
    {
	fp = fopen(file, "w");
	if (fp == NULL)
	{
	    printf("%s %d: cannot open file %s\n", __func__, __LINE__, file);
	    return;
	}
	fprintf(fp, "Wechat\n%s", rule);
	fclose(fp);

	doSystem("iptables -t mangle -I web_auth_drop -p tcp -m multiport --dport 80,443,8080 -m layer7 --l7proto Wechat -j RETURN");
	doSystem("iptables -t mangle -I POSTROUTING -p tcp -m multiport --sport 80,443,8080 -m layer7 --l7proto Wechat -j RETURN");
    } 
    else 
    {
	doSystem("iptables -t mangle -D web_auth_drop -p tcp -m multiport --dport 80,443,8080 -m layer7 --l7proto Wechat -j RETURN");
	doSystem("iptables -t mangle -D POSTROUTING -p tcp -m multiport --sport 80,443,8080 -m layer7 --l7proto Wechat -j RETURN");
	doSystem("rm %s", file);
    }

    return;
}
#endif

/********************************************************************
* ������ webauthProfileChange
* ���ܣ� ��֤���ñ仯 ��Ч����
* ������ 2012-04-17
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void webauthProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const WebAuthProfile *oldProfileDataPtr, const WebAuthProfile *newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
        case PROF_CHANGE_DEL:
        case PROF_CHANGE_DELALL:
        default:/*default*/
#if (WEBAUTH_ACCOUNT == FYES)
	    startWtFlashTaskScheduler();
#endif
            break;
    }
    return;
}

#if (NOTICE == FYES)
extern int isWAOn(void);
#endif

#if (CWMP == FYES)
#if (WIFIDOG_AUTH == FYES)
#define WIFIDOG_OPERATION_URL "GET /operation?identify=TR069"
int send_to_lan_2060p(char* cmd,char* argv)
{
    int    sockfd;
    char   sendb[1024] = {0};
    char   recvb[1024] = {0};
    struct sockaddr_in server_addr;
    char   serverIP[16] = {0};
    struct timeval tv_out;
    int    opt = SO_REUSEADDR;
    char   param[100]={0};

    memset(sendb,0,sizeof(sendb));
    sprintf(sendb,"%s&cmd=%s&argv=%s",WIFIDOG_OPERATION_URL,cmd,argv);

    getIfIp((char *)getLanIfName(), serverIP);
    snprintf(sendb+strlen(sendb),sizeof(sendb)-strlen(sendb)," HTTP/1.0\r\n"
							     "User-Agent: WiFiDog 200905\r\n"
							      "Host: %s\r\n"
							      "\r\n",serverIP);
    if (inet_aton(serverIP, &server_addr.sin_addr) == 0) 
    {
        fprintf(stderr, "the hostip is not right!");
        return 0;
    }
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {
        fprintf(stderr, " %s Socket Error:%s\n\a", __func__);
        return 0;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(2060);

    tv_out.tv_sec = 10;
    tv_out.tv_usec = 0;
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    setsockopt(sockfd, SOL_SOCKET,SO_RCVTIMEO,&tv_out, sizeof(tv_out));

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0) {
        fprintf(stderr, "%s Connect Error:%s\n\a",__func__);
        return 0;
    }
    if(send(sockfd,sendb,strlen(sendb),0) == -1)
    {
        perror("send error.\n"); 
        close(sockfd);
        return 0;
    }
    memset(recvb,0,sizeof(recvb));
    recv(sockfd,recvb,sizeof(recvb),0);

    close(sockfd);
    return 0;
}
#endif
#endif

/********************************************************************
* ������ webauthglobalProfileChange
* ���ܣ� ��֤���ر仯 ��Ч����
* ������ 2012-04-17
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void webauthglobalProfileChange(Pointer tag, MibProfileType profileType,
        ProfChangeType changeType, Uint32 index,
        const WebAuthGlobalProfile *oldProfileDataPtr, const WebAuthGlobalProfile *newProfileDataPtr )        
{
    char ip[16] = {0};
	char *ptr=NULL;
    char buf2[WEB_AUTH_PICTURE_URL_LEN + 1];
    int  tc = 0;
    switch(changeType) {
        case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
	    /* ���lan��IP */
	    getIfIp(getLanIfName(), ip);
	    if((oldProfileDataPtr->enabled == 0U) &&(newProfileDataPtr->enabled == 1U))
	    {
#if (WIFIDOG_AUTH == FYES)
		if((oldProfileDataPtr->authType != newProfileDataPtr->authType) && (getWebauthType() ==2)) {
		    wifidogInit();
		} else if((oldProfileDataPtr->authType == newProfileDataPtr->authType) && (getWebauthType() == 2)) {
		    wifidogInit();
		} else 
		{
#endif
#if 0
		initRuleToPreDrop(ip,ip);
#endif
			SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
			webauthRuleAdd();
			SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
			if((strlen(newProfileDataPtr->pictureUrl) > 6)&&(newProfileDataPtr->active_pic == 1)){
			SWORD_PRINTF("newProfileDataPtr->pictureUrl=%s\n", newProfileDataPtr->pictureUrl );
			strcpy(buf2,newProfileDataPtr->pictureUrl);
			ptr = strstr(buf2,"//");
			if(ptr != NULL){
				ptr +=2;
				ptr = strstr(ptr,"/");
				if(ptr != NULL){
					strcpy(buf2,ptr);
					doSystem("echo -ne 'Picture\n^get %s' >/etc_ro/l7-protocols/Picture.pat",buf2);
					/*�Ź�ͼƬ����*/
					doSystem("iptables -t mangle -I %s -p tcp --dport 80 -m layer7 --l7proto Picture -j RETURN", WEB_AUTH_DROP);
					SWORD_PRINTF("iptables -t mangle -I %s -p tcp --dport 80 -m layer7 --l7proto Picture -j RETURN\n", WEB_AUTH_DROP);
					/*ͼƬ���ӷ��ع���*/
					doSystem("iptables -t mangle -I POSTROUTING -p tcp --sport 80 -m layer7 --l7proto Picture -j RETURN");
				}
			}
		}
		webauthAddexceptIpGroup();
#if (WIFIDOG_AUTH == FYES)
		}
#endif
	    }
	    else if((oldProfileDataPtr->enabled == 1U) && (newProfileDataPtr->enabled == 0U))
	    {
#if 0
		initRuleToPreDrop(ip,ip);
#endif
		SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
		webauthIpDel();
		webauthRuleDel();
#if (WIFIDOG_AUTH == FYES)
		wifidogExit();
#endif
	    }
#if (WIFIDOG_AUTH == FYES)
	    else if((oldProfileDataPtr->enabled == 1U) && (newProfileDataPtr->enabled == 1U) 
		    && (oldProfileDataPtr->authType != newProfileDataPtr->authType)) 
	    {
		if(newProfileDataPtr->authType == 2) /*Զ��wifidog��֤*/
		{
		    /*��ɾ������web��֤*/
		    webauthIpDel();
		    webauthRuleDel();
		    /*����Զ��wifidog��֤*/
		    wifidogInit();
		} else {
		    /*��ɾ��Զ��wifidog��֤*/
		    wifidogExit();
		    /*��������web��֤*/
		    webauthRuleAdd();
		    webauthAddexceptIpGroup();
		}
	    } 
#if (SMS_ENABLE == 1)
	    else if((oldProfileDataPtr->enabled == 1U) && (newProfileDataPtr->enabled == 1U) 
		    && (oldProfileDataPtr->authType == newProfileDataPtr->authType) && (oldProfileDataPtr->smsEnable != newProfileDataPtr->smsEnable)) 
	    {
		if(newProfileDataPtr->authType == 2) /*Զ��wifidog��֤*/
		{
		    if(newProfileDataPtr->smsEnable == 1){
			usbSwitch(1);
		    } else {
			usbSwitch(0);
		    }
		}
	    }
#endif
#if (CWMP == FYES)
	    else if((oldProfileDataPtr->enabled == 1U) && (newProfileDataPtr->enabled == 1U) 
		    && (oldProfileDataPtr->authType == newProfileDataPtr->authType) 
		    && (newProfileDataPtr->authType == 2) /*Զ��wifidog��֤*/
            && (memcmp(&(oldProfileDataPtr->wifiData),&(newProfileDataPtr->wifiData),sizeof(newProfileDataPtr->wifiData)) != 0)
            )
	    {
		   char argv[128] = {0};
		   memset(argv,0,sizeof(argv));
#if (WIFIDOG_AUTH == FYES)
		   if (strcmp(oldProfileDataPtr->wifiData.successUrl,newProfileDataPtr->wifiData.successUrl) != 0) 
		   {
		       snprintf(argv,sizeof(argv),"url:%s",newProfileDataPtr->wifiData.successUrl);
		       send_to_lan_2060p("seturl",argv);
		   }
#if (WIFIDOG_NOTICE == FYES)
		   else if(strcmp(oldProfileDataPtr->wifiData.noticeUrl,newProfileDataPtr->wifiData.noticeUrl) != 0) 
		   {
		       snprintf(argv,sizeof(argv),"url:%s;time:%u",newProfileDataPtr->wifiData.noticeUrl,newProfileDataPtr->wifiData.noticeTime);
		       send_to_lan_2060p("Wifidog_Notice",argv);
		   }
		   else if(oldProfileDataPtr->wifiData.noticeTime !=  newProfileDataPtr->wifiData.noticeTime)
		   {
		       snprintf(argv,sizeof(argv),"url:%s;time:%u",newProfileDataPtr->wifiData.noticeUrl,newProfileDataPtr->wifiData.noticeTime);
		       send_to_lan_2060p("Wifidog_Notice",argv);
		   }
#endif
#if(WIFIDOG_MAC_AUTH == FYES)
		   else if(oldProfileDataPtr->wifiData.macAuthEn !=  newProfileDataPtr->wifiData.macAuthEn)
		   {
		       snprintf(argv,sizeof(argv),"enable:%d",newProfileDataPtr->wifiData.macAuthEn);
		       send_to_lan_2060p("macAuth",argv);
		   }
#endif
#endif
	    }
#endif
#endif
	    else if((strcmp(oldProfileDataPtr->exceptIpGroup,newProfileDataPtr->exceptIpGroup) != 0) ||
			(checkexceptIpGroupMem(oldProfileDataPtr, newProfileDataPtr) == 1 ))
		{
#if (WIFIDOG_AUTH == FYES)
		if(newProfileDataPtr->authType == 2) { /*Զ��wifidog��֤*/
		    if((oldProfileDataPtr->enabled == 1U) && (newProfileDataPtr->enabled == 1U)) {
#if 0
			wifidogAddExceptIpGroup(oldProfileDataPtr->exceptIpGroup, Iptables_DEL);
			wifidogAddExceptIpGroup(newProfileDataPtr->exceptIpGroup, Iptables_INS);
#endif
			WifiDogExceptIpgroup_update(oldProfileDataPtr->exceptIpGroup);
		    }
		} else 
#endif
		{
		webauthDelexceptIpGroup(oldProfileDataPtr);
		SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
		webauthAddexceptIpGroup();
		}
		}else if((oldProfileDataPtr->active_pic == 1) && (newProfileDataPtr->active_pic == 0))
		{/*ɾ���Ź�ͼƬ���ӵĹ���*/
			doSystem("iptables -t mangle -D %s -p tcp --dport 80 -m layer7 --l7proto Picture -j RETURN", WEB_AUTH_DROP);
			doSystem("iptables -t mangle -D POSTROUTING -p tcp --sport 80 -m layer7 --l7proto Picture -j RETURN");
		}
#if (WEBAUTH_ADVANCE == FYES)
		if(newProfileDataPtr->enDefPage == 0)/*������Ĭ��web��֤ҳ��,ʹ���û��ϴ���ҳ��*/
		{
			writeUserPageToRam();
			SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
		}else{
			doSystem("cp /etc_ro/web/WebAuth_auth_def.asp /etc_ro/web/WebAuth_auth.asp -rf");
			SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
		}
#endif
			SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
		if(((strcmp(oldProfileDataPtr->pictureUrl,newProfileDataPtr->pictureUrl) != 0)||(strlen(newProfileDataPtr->pictureUrl) > 6))&&(newProfileDataPtr->active_pic == 1)&&(newProfileDataPtr->enabled == 1U)){
			SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
			strcpy(buf2,newProfileDataPtr->pictureUrl);
			SWORD_PRINTF("newProfileDataPtr->pictureUrl=%s,buf2=%s\n", newProfileDataPtr->pictureUrl,buf2);
			ptr = strstr(buf2,"//");
			if(ptr != NULL){
				ptr +=2;
				ptr = strstr(ptr,"/");
				if(ptr != NULL){
					strcpy(buf2,ptr);
					/*ɾ���Ź���ͼƬ����*/
					doSystem("iptables -t mangle -D %s -p tcp --dport 80 -m layer7 --l7proto Picture -j RETURN", WEB_AUTH_DROP);
					doSystem("iptables -t mangle -D POSTROUTING -p tcp --sport 80 -m layer7 --l7proto Picture -j RETURN");
					doSystem("echo -ne 'Picture\n^get %s' >/etc_ro/l7-protocols/Picture.pat",buf2);
					SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
					/*�Ź�ͼƬ����*/
					doSystem("iptables -t mangle -I %s -p tcp --dport 80 -m layer7 --l7proto Picture -j RETURN", WEB_AUTH_DROP);
					SWORD_PRINTF("iptables -t mangle -I %s -p tcp --dport 80 -m layer7 --l7proto Picture -j RETURN\n", WEB_AUTH_DROP);
					/*ͼƬ���ӷ��ع���*/	
					doSystem("iptables -t mangle -I POSTROUTING -p tcp --sport 80 -m layer7 --l7proto Picture -j RETURN");
				}
			}
		}
#if (WEBAUTH_AUTO == FYES)
		if (oldProfileDataPtr->wechat.enabled == FUN_DISABLE && newProfileDataPtr->wechat.enabled == FUN_ENABLE)
		    wechatFree(1);
		else if (oldProfileDataPtr->wechat.enabled == FUN_ENABLE && newProfileDataPtr->wechat.enabled == FUN_DISABLE)
		    wechatFree(0);
#endif
#ifdef CONFIG_UTT_NETFILTER
#if (WEB_AUTH_STALE == FYES)
		uttNfNlStaleTimeAdd(newProfileDataPtr->staleT);
#endif
#endif
#if (WEBAUTH_ACCOUNT == FYES)
	    startWtFlashTaskScheduler();/*������ɾ��дflash�ƻ�����*/
#endif
            break;
        case PROF_CHANGE_DEL:
        case PROF_CHANGE_DELALL:
        default:/*default*/
            break;
    }      
    return;
}
#if 0
#if (MULTI_LAN == FYES)
/*���ӻ�ɾ����ҳ��֤��������iptables����
 * ipstr : lan��ip
 * action: Iptables_ADD or Iptables_DEL*/
extern void webauthChainAOrD(char action)
{
#if (IP_GRP == FYES)
#if 1
    doSystem("iptables -t nat -%c PREROUTING -i %s+ -m set ! --set %s dst -j %s", action, getLanIfName(), SYS_LAN_GROUP, WEB_AUTH_REDIRECT);
    SWORD_PRINTF("iptables -t nat -%c PREROUTING -i %s+ -m set ! --set %s dst -j %s", action, getLanIfName(), SYS_LAN_GROUP, WEB_AUTH_REDIRECT);
#else
	doSystem("iptables -t nat -%c PREROUTING -m uttdev --is-lan-in -m set ! --set %s dst -j %s", action, SYS_LAN_GROUP, WEB_AUTH_REDIRECT);
	SWORD_PRINTF("iptables -t nat -%c PREROUTING -m uttdev --is-lan-in -m set ! --set %s dst -j %s", action, SYS_LAN_GROUP, WEB_AUTH_REDIRECT);
#endif
#else
#error ipset has been removed
#endif
    return;
}
#else
/*���ӻ�ɾ����ҳ��֤��������iptables����
 * ipstr : lan��ip
 * action: Iptables_ADD or Iptables_DEL*/
extern void webauthChainAOrD(char* ipstr, char action)
{
#if 1
    doSystem("iptables -t nat -%c PREROUTING -i %s+ ! -d %s -j %s", action, getLanIfName(), ipstr, WEB_AUTH_REDIRECT);
    SWORD_PRINTF("iptables -t nat -%c PREROUTING -i %s+ ! -d %s -j %s\n", action, getLanIfName(), ipstr, WEB_AUTH_REDIRECT);
#else
	doSystem("iptables -t nat -%c PREROUTING -m uttdev --is-lan-in ! -d %s -j %s", action, ipstr, WEB_AUTH_REDIRECT);
	SWORD_PRINTF("iptables -t nat -%c PREROUTING -m uttdev --is-lan-in ! -d %s -j %s\n", action, ipstr, WEB_AUTH_REDIRECT);
#endif
#if 0
    /* no user */
    doSystem("iptables -t mangle -%c %s -i %s -m set ! --set %s src -j %s", action, PRE_DROP_CHAIN, getLanIfName(), SYS_WEB_AUTH_OK, WEB_AUTH_DROP);
    SWORD_PRINTF("iptables -t mangle -%c %s -i %s -m set ! --set %s src -j %s", action, PRE_DROP_CHAIN, getLanIfName(), SYS_WEB_AUTH_OK, WEB_AUTH_DROP);
#endif
    return;
}
#endif
#endif
/********************************************************************
* ������ webauthIpDel
* ���ܣ� �ر���ҳ��֤���ܺ����profile�е�Ip
* ������ 2012-04-17
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void webauthIpDel(void)
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH;
    WebAuthProfile *prof = NULL;
    int min = 0, max = 0;
    int i = 0;
#if (IP_GRP == FYES)
    struct in_addr addr;
#endif
#if (WEB_AUTH_SHARE == FYES)
	int j =-1, k =0;
#endif

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) 
    {
	prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
	{
#if (WEB_AUTH_SHARE == FYES)
#if DEBUG
	    printf("%s-%d, prof->user = %s\n",__func__, __LINE__, prof->user);
#endif
	    for(k=0;k<prof->actualSession;k++)
	    {
		j = prof->sesIndex;
		if(checkSessionIndex(prof->sesIndex) != 1)
		{
		    continue;
		}
#if DEBUG
		printf("%s-%d: session ip =0x%x\n",__func__,__LINE__,nvramProfile->session[prof->sesIndex].ip);
#endif
#if (IP_GRP == FYES)
		addr.s_addr = nvramProfile->session[prof->sesIndex].ip;
		userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
		uttNfNlHashDelIp(spdNfNlSock, nvramProfile->session[prof->sesIndex].ip);/*ɾ��hash���Ӧ��ip*/
#endif
		prof->sesIndex = nvramProfile->session[j].nextIndex;
		memset(&(nvramProfile->session[j]), 0U, sizeof(WebAuthSession));
		nvramProfile->session[j].nextIndex = -1;
	    }
	    prof->sesIndex = -1;
	    prof->actualSession = 0U;
#if DEBUG
	    printf("%s-%d: prof->user =%s,prof->maxSession =%d, prof->actualSession =%d, prof->sesIndex =%d\n",__func__,__LINE__,
		    prof->user,prof->maxSession,prof->actualSession,prof->sesIndex);
#endif
#else /*WEB_AUTH_SHARE*/
	    if(prof->ip != 0U)/*�ж�ip�Ƿ�Ϊ��*/
	    {
		SWORD_PRINTF("%s-%d, prof->user = %s\n",__func__, __LINE__, prof->user);
#if (IP_GRP == FYES)
		addr.s_addr = prof->ip;
		userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
		uttNfNlHashDelIp(spdNfNlSock, prof->ip);/*ɾ��hash���Ӧ��ip*/
#endif
		prof->ip = 0U;/*���ip*/
	    }
#endif /*WEB_AUTH_SHARE*/
	}
    }
    return;
}
#if (MULTI_LAN == FYES)
#if (IP_GRP == FYES)
/*
 * webAuthDropChainAOrD  -	�ı���ת��DROP��
 *
 * ���ܣ� 	���ӻ�ɾ��WEB��֤��ת��DROP��
 *
 * action:	Iptables_ADD or Iptables_DEL
 *
 */
static void webAuthDropChainAorD(char action)
{
	doSystem("iptables -t mangle -%c PREROUTING -i %s+ -m set ! --set %s src -m set ! --set %s dst -j %s %s", action, getLanIfName(), SYS_WEB_AUTH_OK, SYS_LAN_GROUP, WEB_AUTH_DROP, ERROR_TO_DEV_NULL);
	return;
}
#else
#error ipset has been removed
#endif
#else
/*
 * webAuthDropChainAOrD  -	�ı���ת��DROP��
 *
 * ���ܣ� 	���ӻ�ɾ��WEB��֤��ת��DROP��
 *
 * ipstr:	lan�ڵ�ַ
 * action:	Iptables_ADD or Iptables_DEL
 *
 */
static void webAuthDropChainAorD(char *ipstr, char action)
{
	doSystem("iptables -t mangle -%c PREROUTING -i %s+ -p tcp --dprot 80 -m set ! --set %s src ! -d %s dst -j %s %s", action, getLanIfName(), SYS_WEB_AUTH_OK, ipstr, WEB_AUTH_DROP, ERROR_TO_DEV_NULL);
	return;
}
#endif

#if (IP_GRP == FYES)
/*
 * webAuthReChainAOrD  -	�ı���ת��REDIRECT��
 *
 * ���ܣ� 	���ӻ�ɾ��WEB��֤��ת��REDIRECT��
 *
 * action:	Iptables_ADD or Iptables_DEL
 *
 */
static void webAuthReChainAOrD(char action)
{
#if (CACHE_SERVER==FYES)
	doSystem("iptables -t mangle -%c POSTROUTING -o %s+ -p tcp -m set --set %s src -m set ! --set %s src -m set ! --set %s dst -j HTTPREDIRECT --redirect-dir %s --return d %s", action, getLanIfName(),SYS_HTTP_PORT, SYS_LAN_GROUP, SYS_WEB_AUTH_OK, WEB_AUTH_AUTH_ASP, ERROR_TO_DEV_NULL);
#else
    /*add src ipaddr match for 0011712 */
	doSystem("iptables -t mangle -%c POSTROUTING -o %s+ -p tcp --sport 80 -m set ! --set %s src -m set ! --set %s dst -j HTTPREDIRECT --redirect-dir %s --return d %s", action, getLanIfName(), SYS_LAN_GROUP, SYS_WEB_AUTH_OK, WEB_AUTH_AUTH_ASP, ERROR_TO_DEV_NULL);
#endif
	if(action == 'D'){
	doSystem("iptables -t mangle -D POSTROUTING -p tcp --sport 80 -m layer7 --l7proto Picture -j RETURN");
	}
	return;
}
#else
#error ipset has been removed
#endif

/********************************************************************
* ������ webauthRuleDel
* ���ܣ� ɾ����������
* ������ 2012-04-17
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void webauthRuleDel(void)
{
    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
    char ip[16];
    memset(ip, 0, sizeof(ip));
    /* ���lan��IP */
    getIfIp(getLanIfName(), ip);

#if 0
#if (MULTI_LAN == FYES)
#if (IP_GRP == FYES)
    /*�������*/
#if 1
    doSystem("iptables -t nat -D PREROUTING -i %s+ -m set ! --set %s dst -j %s", getLanIfName(), SYS_LAN_GROUP, WEB_AUTH_REDIRECT);
    SWORD_PRINTF("iptables -t nat -D PREROUTING -i %s+ -m set ! --set %s dst -j %s", getLanIfName(), SYS_LAN_GROUP, WEB_AUTH_REDIRECT);
#else
	doSystem("iptables -t nat -D PREROUTING -m uttdev --is-lan-in -m set ! --set %s dst -j %s", SYS_LAN_GROUP, WEB_AUTH_REDIRECT);
	SWORD_PRINTF("iptables -t nat -D PREROUTING -m uttdev --is-lan-in -m set ! --set %s dst -j %s", SYS_LAN_GROUP, WEB_AUTH_REDIRECT);
#endif
#else
#error ipset has been removed
#endif
#else
    /*�������*/
#if 1
    doSystem("iptables -t nat -D PREROUTING -i %s+ ! -d %s -j %s", getLanIfName(), ip, WEB_AUTH_REDIRECT);
    SWORD_PRINTF("iptables -t nat -D PREROUTING -i %s+ ! -d %s -j %s\n", getLanIfName(), ip, WEB_AUTH_REDIRECT);
#else
	doSystem("iptables -t nat -D PREROUTING -m uttdev --is-lan-in ! -d %s -j %s", ip, WEB_AUTH_REDIRECT);
	SWORD_PRINTF("iptables -t nat -D PREROUTING -m uttdev --is-lan-in ! -d %s -j %s\n", ip, WEB_AUTH_REDIRECT);
#endif
#endif
    /*�����������*/
    doSystem("iptables -t nat -F %s 1>/dev/null 2>&1",WEB_AUTH_REDIRECT);
    SWORD_PRINTF("iptables -t nat -F %s 1>/dev/null 2>&1\n",WEB_AUTH_REDIRECT);
    /*ɾ����*/
    doSystem("iptables -t nat -X %s  1>/dev/null 2>&1" ,WEB_AUTH_REDIRECT);
    SWORD_PRINTF("iptables -t nat -X %s  1>/dev/null 2>&1\n" ,WEB_AUTH_REDIRECT);
#endif

    /*�������*/
#if 0
#if 1
    doSystem("iptables -t mangle -D %s -i %s+ -m set ! --set %s src -j %s", PRE_DROP_CHAIN, getLanIfName(), SYS_WEB_AUTH_OK, WEB_AUTH_DROP);
    SWORD_PRINTF("iptables -t mangle -D %s -i %s+ -m set ! --set %s src -j %s\n", PRE_DROP_CHAIN, getLanIfName(), SYS_WEB_AUTH_OK, WEB_AUTH_DROP);
#else
	doSystem("iptables -t mangle -D %s -m uttdev --is-lan-in -m set ! --set %s src -j %s", PRE_DROP_CHAIN, SYS_WEB_AUTH_OK, WEB_AUTH_DROP);
	SWORD_PRINTF("iptables -t mangle -D %s -m uttdev --is-lan-in -m set ! --set %s src -j %s\n", PRE_DROP_CHAIN, SYS_WEB_AUTH_OK, WEB_AUTH_DROP);
#endif
#else
	webAuthDropChainAorD(Iptables_DEL);
	webAuthReChainAOrD(Iptables_DEL);
#endif
    /*�����������*/
    doSystem("iptables -t mangle -F %s 1>/dev/null 2>&1",WEB_AUTH_DROP);
    SWORD_PRINTF("iptables -t mangle -F %s 1>/dev/null 2>&1\n",WEB_AUTH_DROP);
   /*ɾ����*/
    doSystem("iptables -t mangle -X %s  1>/dev/null 2>&1" ,WEB_AUTH_DROP);
    SWORD_PRINTF("iptables -t mangle -X %s  1>/dev/null 2>&1\n" ,WEB_AUTH_DROP);

    /*�����ipset��*/
    /*�˲��ֱ�����iptables�������������½��С��������õ�ipset���޷�ɾ��*/
#if 0
    ipsetDelGrp(SYS_WEB_AUTH_WEB, TRUE);/*ɾ������֤�û���*/
#endif
    ipsetDelGrp(SYS_WEB_AUTH_OK, TRUE);/*ɾ������֤�û���*/
    webauthOkGrpAdd(SYS_WEB_AUTH_OK);/*���������*/
#if 0
    webauthWebGrpAdd(SYS_WEB_AUTH_WEB);/*�������֤�û���*/
#endif
    return;
}
/********************************************************************
* ������ webauthRuleAdd
* ���ܣ� ��ӹ�������
* ������ 2012-04-17
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void webauthRuleAdd(void)
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *prof = NULL;
    char ip[16];
    memset(ip, 0, sizeof(ip));
    /* ���lan��IP */
    getIfIp(getLanIfName(), ip);
    prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
SWORD_PRINTF("%s-%d, prof->head.active = %d, prof->enabled = %d\n", __func__, __LINE__, prof->head.active, prof->enabled);
    if((prof != NULL) && (prof->head.active == TRUE) && (prof->enabled == 1U))
    {
        SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
#if 0
        /*nat PREROUTING����REDIRECT��*/
        /*�½�web_auth_redirect�����ṩREDIRECT����*/
        doSystem("iptables -t nat -N %s" ,WEB_AUTH_REDIRECT);
        SWORD_PRINTF("iptables -t nat -N %s\n" ,WEB_AUTH_REDIRECT);
        /*�̶����벢�Ҳ��Ƿ���LAN��IP��ַ������web_auth_redirect��*/
#if (MULTI_LAN == FYES)
#if (IP_GRP == FYES)
#if 1
	doSystem("iptables -t nat -A PREROUTING -i %s+ -m set ! --set %s dst -j %s", getLanIfName(), SYS_LAN_GROUP, WEB_AUTH_REDIRECT);
	SWORD_PRINTF("iptables -t nat -A PREROUTING -i %s+ -m set ! --set %s dst -j %s", getLanIfName(), SYS_LAN_GROUP, WEB_AUTH_REDIRECT);
#else
	doSystem("iptables -t nat -A PREROUTING -m uttdev --is-lan-in -m set ! --set %s dst -j %s", SYS_LAN_GROUP, WEB_AUTH_REDIRECT);
	SWORD_PRINTF("iptables -t nat -A PREROUTING -m uttdev --is-lan-in -m set ! --set %s dst -j %s", SYS_LAN_GROUP, WEB_AUTH_REDIRECT);
#endif
#else
#error ipset has been removed
#endif
#else
#if 1
	doSystem("iptables -t nat -A PREROUTING -i %s+ ! -d %s -j %s", getLanIfName(), ip, WEB_AUTH_REDIRECT);
	SWORD_PRINTF("iptables -t nat -A PREROUTING -i %s+ ! -d %s -j %s\n", getLanIfName(), ip, WEB_AUTH_REDIRECT);
#else
	doSystem("iptables -t nat -A PREROUTING -m uttdev --is-lan-in ! -d %s -j %s", ip, WEB_AUTH_REDIRECT);
	SWORD_PRINTF("iptables -t nat -A PREROUTING -m uttdev --is-lan-in ! -d %s -j %s\n", ip, WEB_AUTH_REDIRECT);
#endif
#endif
        /*���OK����*/
        doSystem("iptables -t nat -A %s -m set --set %s src -j RETURN", WEB_AUTH_REDIRECT, SYS_WEB_AUTH_OK);
        SWORD_PRINTF("iptables -t nat -A %s -m set --set %s src -j RETURN\n", WEB_AUTH_REDIRECT, SYS_WEB_AUTH_OK);
	/*��¼Դ��ַ*/
        doSystem("iptables -t nat -A %s -p tcp --dport 80 -j SET --add-set %s src", WEB_AUTH_REDIRECT, SYS_WEB_AUTH_WEB);
        SWORD_PRINTF("iptables -t nat -A %s -p tcp --dport 80 -j SET --add-set %s src\n", WEB_AUTH_REDIRECT, SYS_WEB_AUTH_WEB);
	/*���REDIRECT*/
#if 0
        doSystem("iptables -t nat -A %s -p tcp --dport 80 -j REDIRECT", WEB_AUTH_REDIRECT);
        SWORD_PRINTF("iptables -t nat -A %s -p tcp --dport 80 -j REDIRECT\n", WEB_AUTH_REDIRECT);
#else
	doSystem("iptables -t nat -A %s -p tcp --dport 80 -j DNAT --to %s", WEB_AUTH_REDIRECT, ip);
	SWORD_PRINTF("iptables -t nat -A %s -p tcp --dport 80 -j DNAT --to %s\n", WEB_AUTH_REDIRECT, ip);
#endif
#endif

	/*mangle PREROUTING ����DROP��*/
	/*�½�web_auth_drop�����ṩ��������*/
        doSystem("iptables -t mangle -N %s", WEB_AUTH_DROP);
        SWORD_PRINTF("iptables -t mangle -N %s\n", WEB_AUTH_DROP);
	webAuthDropChainAorD(Iptables_ADD);
	webAuthReChainAOrD(Iptables_ADD);
#if 0
	/*�̶������Ҳ���OK���еĽ�����web_auth_drop*/
#if 1
        doSystem("iptables -t mangle -A %s -i %s+ -m set ! --set %s src -j %s", PRE_DROP_CHAIN, getLanIfName(), SYS_WEB_AUTH_OK, WEB_AUTH_DROP);
        SWORD_PRINTF("iptables -t mangle -A %s -i %s+ -m set ! --set %s src -j %s\n", PRE_DROP_CHAIN, getLanIfName(), SYS_WEB_AUTH_OK, WEB_AUTH_DROP);
#else
	doSystem("iptables -t mangle -A %s -m uttdev --is-lan-in -m set ! --set %s src -j %s", PRE_DROP_CHAIN, SYS_WEB_AUTH_OK, WEB_AUTH_DROP);
	SWORD_PRINTF("iptables -t mangle -A %s -m uttdev --is-lan-in -m set ! --set %s src -j %s\n", PRE_DROP_CHAIN, SYS_WEB_AUTH_OK, WEB_AUTH_DROP);
#endif
#endif
	/*�����������ȼ���������http��
	 * Ϊ��ֹ��get���͵�http�����ù���ֻƥ�京��html���ݵ�http��
	 * ֻ����get��Ϊ�˷�ֹ ���ַǷ�http��������goahead æ���������i
	 */
	/*������get��*/
	doSystem("iptables -t mangle -A %s -p tcp --dport 80 -m layer7 --l7proto GetPage -j RETURN", WEB_AUTH_DROP);
	SWORD_PRINTF("iptables -t mangle -A %s -p tcp --dport 80 -m layer7 --l7proto Get -j RETURN\n", WEB_AUTH_DROP);
	/*��get������*/ 
	doSystem("iptables -t mangle -A %s -p tcp --dport 80 -m layer7 --l7proto nonZeroLen -j DROP",WEB_AUTH_DROP);
	SWORD_PRINTF("iptables -t mangle -A %s -p tcp --dport 80 -m layer7 --l7proto nonZeroLen -j DROP\n",WEB_AUTH_DROP);
	/*�Ź�80�˿�*/
        doSystem("iptables -t mangle -A %s -p tcp --dport 80 -j RETURN", WEB_AUTH_DROP);
        SWORD_PRINTF("iptables -t mangle -A %s -p tcp --dport 80 -j RETURN\n", WEB_AUTH_DROP);
	/*�Ź�dns*/
        doSystem("iptables -t mangle -A %s -p udp --dport 53 -j RETURN", WEB_AUTH_DROP);
        SWORD_PRINTF("iptables -t mangle -A %s -p udp --dport 53 -j RETURN\n", WEB_AUTH_DROP);
	/*�Ź�dhcp*/
        doSystem("iptables -t mangle -A %s -p udp --dport 67 -j RETURN", WEB_AUTH_DROP);
        SWORD_PRINTF("iptables -t mangle -A %s -p udp --dport 67 -j RETURN\n", WEB_AUTH_DROP);
		
	/*���DROP����*/
        doSystem("iptables -t mangle -A %s -j DROP", WEB_AUTH_DROP);
        SWORD_PRINTF("iptables -t mangle -A %s -j DROP\n", WEB_AUTH_DROP);
    }
    return;
}
/*********************************
 *
 *�Ƚ������ַ��ĳ�Ա��Χ�Ƿ���ͬ
 *
 *return 1����ͬ 
 *		0 ��ͬ
 * *******************************/

static int  checkexceptIpGroupMem(WebAuthGlobalProfile *oldprof, WebAuthGlobalProfile *newprof)
{

	IpGrpProfile* oldprofs1 = NULL;
	IpGrpProfile* newprofs1 = NULL;

	oldprofs1 =(IpGrpProfile*)ProfGetInstPointByName(MIB_PROF_IPGRP,oldprof->exceptIpGroup);
	newprofs1 =(IpGrpProfile*)ProfGetInstPointByName(MIB_PROF_IPGRP,newprof->exceptIpGroup);
	if(oldprofs1 != newprofs1){
	return 1;
	}
	else{
	return 0;
	}
}
/***************************************
 * ���ܣ�����������û��Ƿ��������ַ���Ա
 * �ǣ����û����ߴ���
 * ����������
 * ��������
 * ʱ�䣺20130327
 * ****************************************/

static void checkIpIfOnLine(void)
{
	MibProfileType mibType = MIB_PROF_WEB_AUTH;
	WebAuthProfile *prof = NULL;
	MibProfileType mibTypeGlb = MIB_PROF_WEB_AUTH_GLOBAL;
	WebAuthGlobalProfile *profGlb = NULL;
	int min = 0, max = 0;
	int i = 0;
	struct in_addr addr;
#if (WEB_AUTH_SHARE == FYES)
	int prev =-1, j =-1, k = 0;
	int maxSes = 0;
#endif
	
	profGlb = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeGlb, 0);

	ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++) 
	{
		
		prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
		if((prof != NULL) && (ProfInstIsFree(prof) == 0) 
				&& (prof->head.active == TRUE))
		{
#if (WEB_AUTH_SHARE == FYES)
#if DEBUG
	    printf("%s-%d: prof->user =%s,prof->maxSession =%d, prof->actualSession =%d, prof->sesIndex =%d\n",__func__,__LINE__,
		    prof->user,prof->maxSession,prof->actualSession,prof->sesIndex);
#endif
			/*����IP�����˺��Ƿ����ߣ�������ǿ������*/
			if(checkSessionIndex(prof->sesIndex) == 1) {
			j = prof->sesIndex;
			addr.s_addr = nvramProfile->session[j].ip;
			while(ipsetTestIpStr(profGlb->exceptIpGroup,inet_ntoa(addr)) == 1)
			{
#if DEBUG
			    printf("%s-%d: session->ip =0x%x\n",__func__,__LINE__,nvramProfile->session[j].ip);
#endif
			    /*����IP��OK�������*/
			    ipsetAOrDIp(SYS_WEB_AUTH_OK,nvramProfile->session[j].ip,IPSET_MEM_DEL); 
#if (IP_GRP == FYES)
			    SWORD_PRINTF("%s-----%d, inet_ntoa(addr) is %s\n", __func__, __LINE__,  inet_ntoa(addr));
			    userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
			    uttNfNlHashDelIp(spdNfNlSock, nvramProfile->session[j].ip);/*ɾ��hash���Ӧ��ip*/
#endif
			    prof->sesIndex = nvramProfile->session[j].nextIndex;
			    memset(&(nvramProfile->session[j]), 0U, sizeof(WebAuthSession));
			    nvramProfile->session[j].nextIndex = -1;
			    prof->actualSession -= 1;
			    j = prof->sesIndex;
			    if(checkSessionIndex(j) != 1) {
				break;
			    }
			    addr.s_addr = nvramProfile->session[j].ip;
			}
			}
			if(checkSessionIndex(prof->sesIndex) == 1) {
			prev = prof->sesIndex;
			j = nvramProfile->session[prev].nextIndex;
			maxSes = prof->actualSession;
			for(k=0;k<maxSes;k++)
			{
			    if(checkSessionIndex(j) != 1) 
			    {
				break;
			    }
			    addr.s_addr = nvramProfile->session[j].ip;
			    if(ipsetTestIpStr(profGlb->exceptIpGroup,inet_ntoa(addr)) == 1)
			    {
#if DEBUG
				printf("%s-%d: session ip =0x%x\n",__func__,__LINE__,nvramProfile->session[j].ip);
#endif
				/*����IP��OK�������*/
				ipsetAOrDIp(SYS_WEB_AUTH_OK,nvramProfile->session[j].ip,IPSET_MEM_DEL); 
#if (IP_GRP == FYES)
				SWORD_PRINTF("%s-----%d, inet_ntoa(addr) is %s\n", __func__, __LINE__,  inet_ntoa(addr));
				userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
				uttNfNlHashDelIp(spdNfNlSock, nvramProfile->session[j].ip);/*ɾ��hash���Ӧ��ip*/
#endif
				nvramProfile->session[prev].nextIndex = nvramProfile->session[j].nextIndex;
				memset(&(nvramProfile->session[j]), 0U, sizeof(WebAuthSession));
				nvramProfile->session[j].nextIndex = -1;
				j = nvramProfile->session[prev].nextIndex;
				prof->actualSession -= 1;
			    } else {
				prev = j;
				j = nvramProfile->session[j].nextIndex;
			    }
			}
			}
#if DEBUG
	    printf("%s-%d: prof->user =%s,prof->maxSession =%d, prof->actualSession =%d, prof->sesIndex =%d\n",__func__,__LINE__,
		    prof->user,prof->maxSession,prof->actualSession,prof->sesIndex);
#endif
#else /*WEB_AUTH_SHARE*/
			/*����IP�����˺��Ƿ����ߣ�������ǿ������*/
			if(prof->ip != 0U )
			{
			    addr.s_addr = prof->ip;
			    if(ipsetTestIpStr(profGlb->exceptIpGroup,inet_ntoa(addr)) == 1)
			    {
				/*����IP��OK�������*/
				ipsetAOrDIp(SYS_WEB_AUTH_OK,prof->ip,IPSET_MEM_DEL); 
#if (IP_GRP == FYES)
				SWORD_PRINTF("%s-----%d, inet_ntoa(addr) is %s\n", __func__, __LINE__,  inet_ntoa(addr));
				userUpToIpGrp(prof->user, inet_ntoa(addr), IPENT_WEB_AUTH, IPSET_MEM_DEL);
#endif
#ifdef CONFIG_UTT_NETFILTER
				uttNfNlHashDelIp(spdNfNlSock, prof->ip);/*ɾ��hash���Ӧ��ip*/
#endif
				prof->ip = 0U;
			    }
			}
#endif /*WEB_AUTH_SHARE*/
		}
		
	}

}

/*********************************
 * ���ܣ���������ַ�鵽OK��
 * ��������
 * ʱ�䣺20130327
 * **********************************/
extern void webauthAddexceptIpGroup(void)
{

	MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
	WebAuthGlobalProfile *prof = NULL;
	IpGrpProfile* profs1 = NULL;
	IpGrpProfile* profs2 = NULL;
	int i=0,j=0;
	IPADDR ipfrom,ipto;
	prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
	if(prof->exceptIpGroup[0] != '\0')
	{
	profs1 =(IpGrpProfile*)ProfGetInstPointByName(MIB_PROF_IPGRP,prof->exceptIpGroup);

	/*����û��Ƿ����߲�����Ӧ����*/
	checkIpIfOnLine();

	/*  ���������ӵ�OK����*/
		while( i < IPGRP_ENTRYS )
			{
				if(profs1->ipEntry[i].entryType == IPENT_GRP)
				{
					SWORD_PRINTF("%s,%d==ipgrpname=%s\n",__FILE__,__LINE__,profs1->ipEntry[i].entryVal.name);
					profs2 = (IpGrpProfile*)ProfGetInstPointByName(MIB_PROF_IPGRP,profs1->ipEntry[i].entryVal.name);
					j=0;
					while(j<IPGRP_ENTRYS){
					if(profs2->ipEntry[j].entryType == IPENT_RANGE){
					ipfrom = profs2->ipEntry[j].entryVal.range.ipFrom;
					ipto = profs2->ipEntry[j].entryVal.range.ipTo;
					SWORD_PRINTF("IPfrom=%x,ipto=%x\n",ipfrom,ipto);
					/*��ӵ�ַ��*/	
					ipsetAOrDIpRange(SYS_WEB_AUTH_OK, ipfrom, ipto, IPSET_MEM_ADD);
					}
					j++;
					}
				}
				else if (profs1->ipEntry[i].entryType == IPENT_RANGE){
					SWORD_PRINTF("%s,%d==ipgrpname=\n",__FILE__,__LINE__);
					ipfrom =  profs1->ipEntry[i].entryVal.range.ipFrom;
					ipto =  profs1->ipEntry[i].entryVal.range.ipTo;
					ipsetAOrDIpRange(SYS_WEB_AUTH_OK, ipfrom, ipto, IPSET_MEM_ADD);
				}
				i++;
			}
	}
return ;
}
/*
 * ������ҳ��֤�˺ż�¼map
 */
static void webauthUpdatePortMap(void) 
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH;
    WebAuthProfile *prof = NULL;
    int i = 0, min = 0, max = 0;
    Uint16 webInstIndex;

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) 
    {
        prof = (WebAuthProfile *)ProfGetInstPointByIndex(mibType, i);
	if ((ProfInstIsFree(prof) == 0) && (strncmp(prof->head.name, "ID", 2) == 0)) 
	{
	    webInstIndex = (Uint16)strtol(&(prof->head.name[2]), NULL, 10);
	    if (webInstIndex != 0) 
	    {
		setPortMap(&(nvramProfile->webauthInstRec.instRecMap), webInstIndex);
	    }
	}
    }

    return;
}
#if (WEBAUTH_ADVANCE == FYES)
/********************************************************************
 *���ܣ�������web��֤ҳ�汣�浽�ڴ���,��ΪĬ��ҳ��
 *������saveWebAuthDefPage
 *��������
 *���أ���
 *�������
 *������20130618
 * ******************************************************************/
static void saveWebAuthDefPage()
{
doSystem("cp -f  /etc_ro/web/WebAuth_auth.asp /etc_ro/web/WebAuth_auth_def.asp");
}
/*******************************************************************
 *����:�ӷ����ж����û��ϴ���web��֤ҳ�沢д���ڴ���
 *����:writeUserPageTpRam
 *����:��
 *���:��
 *����:ʵ�ʶ������ֽ���
 *����:20130618
 * *****************************************************************/
static void writeUserPageToRam()
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *prof = NULL;
    prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
	if(strlen(prof->pageName)> 4)
	{
#if (CONFIG_X86_CPU == 1)
		doSystem("cp -f /ff/%s /etc_ro/web/WebAuth_auth.asp",prof->pageName);
#else
#endif
	}
}
#endif

/********************************************************************
* ������ webauthInit
* ���ܣ� ��֤��ʼ������ - ��������й����������������
* ������ 2012-04-17
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void webauthInit(void)
{
    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
    char buf[WEB_AUTH_CONTACT_LEN + 1];
    char buf2[WEB_AUTH_PICTURE_URL_LEN + 1];
#if (WEBAUTH_AUTO == FYES)
    char buf3[WEB_AUTH_PICTURE_URL_LEN + 1];
    int count = 0;
    unsigned int flag = 3u;
    char device_path[MAX_STORAGE_NUM][256];
#endif
    MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *prof = NULL;
	char *ptr=NULL;
#if (WEBAUTH_ACCOUNT == FYES)
    startWtFlashTaskScheduler();/*������ɾ��дflash�ƻ�����*/
    strcpy(arpLan.arp_dev, getLanIfName());
#endif
    prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
    if(ProfInstIsFree(prof) == 0)
    {
#if (WEBAUTH_AUTO == FYES)
	if (prof->wechat.wechatPicName[0] != '\0')
	{
#if (CONFIG_X86_CPU == 1)
	doSystem("cp /ff/%s /etc_ro/web/images/weixinpic.jpg", prof->wechat.wechatPicName);    
#else
	if (isStorageExist(device_path,&count,flag)==1){
	    doSystem("cp %s%s /etc_ro/web/images/weixinpic.jpg", device_path[0], prof->wechat.wechatPicName);
	} else
	    printf("\n\ncheck no usb device.\n\n");
#endif
	}
#endif
#if (WEBAUTH_ADVANCE == FYES)
	saveWebAuthDefPage();
	if(prof->enDefPage == 0)/*������Ĭ��web��֤ҳ��,ʹ���û��ϴ���ҳ��*/
	{
		writeUserPageToRam();
	}
	if(((prof->enpicture == 1)||(prof->enpicture == 3))&&(prof->picName[0] != '\0')&&(strlen(prof->picName) > 4))
	{
#if (CONFIG_X86_CPU == 1)
        doSystem("cp /ff/%s /etc_ro/web/images/",prof->picName);
#else
#endif
	}
#endif
	memset(buf,0,sizeof(buf));
	readContactFromFile(buf, WEB_AUTH_CONTACT_PATH,WEB_AUTH_CONTACT_LEN + 1);
	strncpy(prof->contact, buf, WEB_AUTH_CONTACT_LEN + 1);
	memset(buf2,0,sizeof(buf2));
	readContactFromFile(buf2, WEB_AUTH_PICURL_PATH, WEB_AUTH_PICTURE_URL_LEN + 1);
    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
	strncpy(prof->pictureUrl, buf2,WEB_AUTH_PICTURE_URL_LEN + 1);
#if (WEBAUTH_AUTO == FYES)
	memset(buf3,0,sizeof(buf3));
	readContactFromFile(buf3, WEB_AUTH_JUMPURL_PATH, WEB_AUTH_PICTURE_URL_LEN + 1);
	strncpy(prof->jumpUrl, buf3, WEB_AUTH_PICTURE_URL_LEN + 1);
	//printf("\n\nread: %s\n\n\nprof->jumpUrl: %s\n\n\n", buf3, prof->jumpUrl);
#endif
#ifdef CONFIG_UTT_NETFILTER
#if (WEB_AUTH_STALE == FYES)
	if((prof->enabled == 1U)) {
	    if(prof->staleT>5U) {
		uttNfNlStaleTimeAdd(prof->staleT);
	    }
	}
#endif
#endif
    }
    webauthRuleDel();
    webauthRuleAdd();
	if(strlen(buf2) > 6){
	if((prof->active_pic == 1) && (strstr(buf2,"http") != NULL) && (prof->enabled == 1U))
	{
		SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
		ptr = strstr(buf2,"//");
		if(ptr != NULL){
			ptr +=2;
			ptr = strstr(ptr,"/");
			if(ptr != NULL){
				strcpy(buf2,ptr);
				doSystem("echo -ne 'Picture\n^get %s' >/etc_ro/l7-protocols/Picture.pat",buf2);
				/*�Ź�ͼƬ����*/
				doSystem("iptables -t mangle -I %s -p tcp --dport 80 -m layer7 --l7proto Picture -j RETURN", WEB_AUTH_DROP);
				SWORD_PRINTF("iptables -t mangle -I %s -p tcp --dport 80 -m layer7 --l7proto Picture -j RETURN\n", WEB_AUTH_DROP);
				/*ͼƬ���ӷ��ع���*/	
				doSystem("iptables -t mangle -I POSTROUTING -p tcp --sport 80 -m layer7 --l7proto Picture -j RETURN");
			}
		}
	}
	}
#if (WEBAUTH_AUTO == FYES)
	/* �������΢����֤����֤ǰ��ͨ΢�� */
	if (prof->wechat.enabled == FUN_ENABLE)
	    wechatFree(1);
#endif
    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
    return;
}
/********************************************************************
* ������ funInitWebAuth
* ���ܣ� ��֤���� ��ʼ����ں���
* ������ 2012-04-17
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
void funInitWebAuth(void)
{
    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
    registerForProfileUpdates(MIB_PROF_WEB_AUTH, (Pointer)NULL, (ProfileUpdateFnc)webauthProfileChange);/*��������*/
    registerForProfileUpdates(MIB_PROF_WEB_AUTH_GLOBAL, (Pointer)NULL, (ProfileUpdateFnc)webauthglobalProfileChange);/*��������*/

    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
    ipsetDelGrp(SYS_HTTP_PORT,TRUE);
	httpPortGrpAdd(SYS_HTTP_PORT);
	Uint32 sport = 80;
#if (CACHE_SERVER == FYES)
	CacheServerProfile *profCacheServer = NULL;
	MibProfileType mibType = MIB_PROF_CACHE_SERVER;
	profCacheServer = (CacheServerProfile*)ProfGetInstPointByIndex(mibType, 0);
	if(profCacheServer->cacheServerEnable == FUN_ENABLE){
		sport = profCacheServer->cacheServerPort;
	}       
#endif  
	httpPortAdd(SYS_HTTP_PORT,sport);
#if (WIFIDOG_AUTH == FYES)
#if (WHITELIST == 1)
    registerForProfileUpdates(MIB_PROF_WHITE_LIST, (Pointer)NULL, (ProfileUpdateFnc)whiteListProfileChange);/*��������*/
#endif
#if 0
    /*�����ã������ļ������������ҿɸ���*/
    wechatFree_wifidog();
#endif
    if(getWebauthType() == 1) {
    SWORD_PRINTF("%s=%d: ========== web auth init\n",__func__,__LINE__);
#endif
    webauthInit();/*��ʼ��*/
	webauthAddexceptIpGroup();
#if (WIFIDOG_AUTH == FYES)
    } else if(getWebauthType() == 2) {
	webauthOkGrpAdd(SYS_WEB_AUTH_OK);/*mantis 16779*/
	wifidogInit();
	initWebAuth_start();
    }
#endif
   /*��ʼ����¼map */
    webauthUpdatePortMap();
#if (WEBAUTH_SELF_HELP == FYES)
    doSystem("cron-init.sh del \"uttclearOptCnt webauth\";cron-init.sh add \"00 01 * * * uttclearOptCnt webauth\"");
#endif
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
    doSystem("udpserver &");
    doSystem("webudpserver &");
#endif
    return;
}
#if(WIFIDOG_AUTH == FYES)
#if(WHITELIST == 1)
/********************************************************************
* ������ whiteListProfileChange
* ���ܣ� ��֤���ñ仯 ��Ч����
* ������ 2014-07-17
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� hww 
********************************************************************/
static void whiteListProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const WhiteListProfile *oldProfileDataPtr, const WhiteListProfile *newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
#ifdef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
	/*�Ƶ��ں�ʵ�֣�ͨ��iptables��WiFiMatchƥ��ģ��ʵ��,�滻else�е�ʵ�ַ�ʽ*/
			wifidogDnsAddOrDel(newProfileDataPtr, PROF_CHANGE_ADD);
#else
			whiteList_add(newProfileDataPtr);
#endif
			break;
        case PROF_CHANGE_EDIT:
			break;
        case PROF_CHANGE_DEL:
#ifdef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
	/*�Ƶ��ں�ʵ�֣�ͨ��iptables��WiFiMatchƥ��ģ��ʵ��,�滻else�е�ʵ�ַ�ʽ*/
			wifidogDnsAddOrDel(oldProfileDataPtr, PROF_CHANGE_DEL);
#else
			whiteList_del(oldProfileDataPtr);
#endif
			break;
        case PROF_CHANGE_DELALL:
#ifdef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
	/*�Ƶ��ں�ʵ�֣�ͨ��iptables��WiFiMatchƥ��ģ��ʵ��,�滻else�е�ʵ�ַ�ʽ*/
			wifidogDnsAddOrDel(NULL, PROF_CHANGE_DELALL);
#else
			whiteList_delAll(oldProfileDataPtr);
#endif
			break;
        default:/*default*/
			break;
    }
    return;
}
#ifdef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
/*���ڼ��url
 * ����ֵ
 * ����-1
 * ip : 1
 * url :2
 */
static int checkUrl(char * domain)
{
    struct in_addr addr;
    regex_t myreg;
    char err[128];
    int resulta,resultb;
    regmatch_t match[1];
    size_t nmatch = 1;
    char pattern[] = "(http(s)?://)?(www\.)?(([0-9a-z_!~*'()-]+\.)*)";

    if (domain == NULL){
	return -1;
    }else if( 0 != inet_aton(domain, &addr)) {
    SWORD_PRINTF(" %d -- %s   %s  is ip \n",__LINE__,__func__,domain);
	return 1;
    }else{
	resulta = regcomp(&myreg, pattern, REG_EXTENDED | REG_NEWLINE);/*����ƥ��url*/
	if(resulta != 0)
	{
	    fprintf(stderr, "pattern compiled error\n");
	    regerror(resulta, &myreg, err, sizeof(err));
	    regfree(&myreg);
	    //exit(1);
	    SWORD_PRINTF(" %d -- %s   %s not is url \n",__LINE__,__func__,domain);
	    return -1;
	}
	SWORD_PRINTF(" %d -- %s   %s  is url \n",__LINE__,__func__,domain);
	regfree(&myreg);
	return 2;
    }
}

/*�Ƶ��ں�ʵ�֣�ͨ��iptables��WiFiMatchƥ��ģ��ʵ��*/
/*
 * wifidogDnsAddOrDel
 * ���������������ɾ������
 * ���������������ô����ں�
 * guo.deyuan
 */
static void wifidogDnsAddOrDel(WhiteListProfile *prof, ProfChangeType changeType) {

    UttNlNfwifiDnsConf  nlConf;
    WebAuthGlobalProfile *profG = NULL; 
    MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
    profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
    /*Զ����֤�Ƿ���*/	
    if((profG != NULL) && (profG->head.active == TRUE) && (profG->enabled == 1U) && ((profG->authType == 2U))) {

	memset(&nlConf, 0, sizeof(nlConf));
	nlConf.msgHead.nlType = UTT_NLNF_WIFIDOG_DNS_WHITE;	    /*wifidog����������*/
	/*���� ɾ��*/
	if(prof != NULL && checkUrl(prof->domain) == 1){
	    if(changeType == PROF_CHANGE_ADD) {
		ipsetAOrDIpStr(WHITE_LIST, prof->domain,IPSET_MEM_ADD);
		return ;
	    } else if(changeType == PROF_CHANGE_DEL) {
		ipsetAOrDIpStr(WHITE_LIST, prof->domain,IPSET_MEM_DEL); 
		return ;
	    }   
	}
	if(changeType == PROF_CHANGE_ADD) {
	    nlConf.confType = UTT_NFCONF_ADD;		/*add one rule*/
	} else if(changeType == PROF_CHANGE_DEL) {
	    nlConf.confType = UTT_NFCONF_DEL;		/*del one rule*/
	} else if(changeType == PROF_CHANGE_DELALL) {
	    doSystem("ipset -q -F %s ",WHITE_LIST);
	    nlConf.confType = UTT_NFCONF_DEL_ALL;	/*del all rule*/
	    printf("%s-%d: profile change type: delall\n",__func__,__LINE__);
	    uttNfNlSend(spdNfNlSock,(char*)&nlConf, sizeof(nlConf), 0 ,0 );/*������Ϣ���ں�*/
	    return;
#if (UTT_KNOWIFI == FYES)
	} else if(changeType == UTT_KNOWIFI_WHITE_LIST) {
	    nlConf.confType = UTT_NFCONF_ADD;		/*add one rule*/
#endif
	} else {
	    fprintf(stderr,"wifidog add dns error,profile change type: %d\n",changeType);
	    return;
	}


	if((prof!=NULL) && (strlen(prof->domain) > 0U)) {
	    strncpy(nlConf.entryVal.domainName, prof->domain, MAX_DOMAIN_NAME_LENGTH);/*����*/
	    nlConf.entryVal.ipEntry = NULL;
	    printf("%s-%d: profile change type: %s, domain %s\n",__func__,__LINE__,((changeType==PROF_CHANGE_ADD)?"add":"del"),nlConf.entryVal.domainName);
	    uttNfNlSend(spdNfNlSock,(char*)&nlConf, sizeof(nlConf), 0 ,0 );/*������Ϣ���ں�*/
	}
#if (UTT_KNOWIFI == FYES)
	else if (changeType == UTT_KNOWIFI_WHITE_LIST)
	{
	    strncpy(nlConf.entryVal.domainName, UTT_KNOWIFI_WHITE_LIST_DOMAIN, MAX_DOMAIN_NAME_LENGTH);/*����*/
	    nlConf.entryVal.ipEntry = NULL;
	    uttNfNlSend(spdNfNlSock,(char*)&nlConf, sizeof(nlConf), 0 ,0 );/*������Ϣ���ں�*/
	    sleep(0.1);
	    strncpy(nlConf.entryVal.domainName, UTT_KNOWIFI_WHITE_LIST_OTHER1, MAX_DOMAIN_NAME_LENGTH);/*����*/
	    nlConf.entryVal.ipEntry = NULL;
	    uttNfNlSend(spdNfNlSock,(char*)&nlConf, sizeof(nlConf), 0 ,0 );/*������Ϣ���ں�*/
	    sleep(0.1);
	    strncpy(nlConf.entryVal.domainName, UTT_KNOWIFI_WHITE_LIST_OTHER2, MAX_DOMAIN_NAME_LENGTH);/*����*/
	    nlConf.entryVal.ipEntry = NULL;
	    uttNfNlSend(spdNfNlSock,(char*)&nlConf, sizeof(nlConf), 0 ,0 );/*������Ϣ���ں�*/
	}
#endif
    }

    return;
}

static void wifidogDnsInit(ProfChangeType changeType) {
    int min, max, index;
    MibProfileType mibType = MIB_PROF_WHITE_LIST;
    WhiteListProfile *prof = NULL;

    if(changeType == PROF_CHANGE_DELALL) {
	wifidogDnsAddOrDel(NULL, changeType);
    } else {
	ProfInstLowHigh(mibType, &max, &min);
	for (index = min; index < max; index++)
	{
	    prof = (WhiteListProfile *)ProfGetInstPointByIndex(mibType, index);
	    wifidogDnsAddOrDel(prof, changeType);
	}
#if (UTT_KNOWIFI == FYES)
	wifidogDnsAddOrDel(NULL, UTT_KNOWIFI_WHITE_LIST);
#endif
    }
}
#else
static void getDomainNameIP(char *domain,char opt)
{
    struct hostent *host;
    struct in_addr inaddr;
    char **ptr = NULL;
    char ip[20];

    host = gethostbyname(domain);
    if(host!=NULL)
    {
	for(ptr=host->h_addr_list;*ptr != NULL;ptr++) {
	memcpy((char *) &inaddr, *ptr,(size_t) host->h_length);
	memset(ip, 0, sizeof(ip));
	strcpy(ip,inet_ntoa(inaddr));
	ipsetAOrDIpStr(WHITE_LIST, ip,opt);                                   
	}
    }
    return;
}
static void whiteList_add(WhiteListProfile *whiteListProfile)
{	
	WebAuthGlobalProfile *prof = NULL; 
	MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
	prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
	/*Զ����֤�Ƿ���*/	
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE) && (prof->authType == 2U)){
	    if((whiteListProfile->domain != NULL) && (strlen(whiteListProfile->domain) > 0)) {
		getDomainNameIP(whiteListProfile->domain,IPSET_MEM_ADD); 
	    }
	}
}
static void whiteList_del(WhiteListProfile* whiteListProfile)
{  
	WebAuthGlobalProfile *prof = NULL;
	MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
	prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE) && (prof->authType == 2U)){	
	    if((whiteListProfile->domain != NULL) && (strlen(whiteListProfile->domain) > 0)) {
		getDomainNameIP(whiteListProfile->domain,IPSET_MEM_DEL); 
	    }
	}

}
static void whiteList_delAll(WhiteListProfile *whiteListProfile)
{	
	WebAuthGlobalProfile *prof = NULL; 
	MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
	prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE) && (prof->authType == 2U)){	
		doSystem("ipset -q -F %s ",WHITE_LIST);
	}
}
#endif

#endif
#endif
#endif

