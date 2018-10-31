/* vi: set sw=4 ts=4 sts=4: */
/*
 * main.c -- Main program for the GoAhead WebServer (LINUX version)
 *
 * Copyright (c) GoAhead Software Inc., 1995-2000. All Rights Reserved.
 *
 * See the file "license.txt" for usage and redistribution license requirements
 *
 * $Id: goahead.c,v 1.120.14.9.2.7 2017/12/26 17:00:49 wu.xiaoyu Exp $
 */

/******************************** Description *********************************/

/*
 *	Main program for for the GoAhead WebServer. This is a demonstration
 *	main program to initialize and configure the web server.
 */

/********************************* Includes ***********************************/

#include	"uemf.h"
#include	"wsIntrn.h"
#if CONFIG_RALINK_RT3052 || CONFIG_RALINK_RT3883
#include	"ralink_gpio.h"
#endif
#include	"internet.h"
#if defined INIC_SUPPORT || defined INICv2_SUPPORT
#include	"inic.h"
#endif
#if defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
#include	"legacy.h"
#endif
#include	"utils.h"
#include	"wireless.h"
#include	"firewall.h" 
#include	"management.h"
#include	"station.h"
#include	"usb.h"
#include	"media.h"
#include	<signal.h>
#include	<unistd.h> 
#include	<sys/types.h>
#include	<sys/wait.h>
#include    <sys/ioctl.h>
#include    <sched.h>
#include	"linux/autoconf.h"
#include	"config/autoconf.h" //user config

#include <sys/utsname.h>
#include <sys/mman.h>
#include <memory.h>
#include "utt_conf_define.h"

#if (UTT_NV_1800N == FYES)
#include "urcp_msg.h"
#endif

#ifdef CONFIG_RALINKAPP_SWQOS
#include      "qos.h"
#endif

#ifdef WEBS_SSL_SUPPORT
#include	"websSSL.h"
#endif

#ifdef USER_MANAGEMENT_SUPPORT
#include	"um.h"
void	formDefineUserMgmt(void);
#endif

#include	"mib.h"
#include	"profacce.h"
#include	<uttMachine.h>
#ifdef CONFIG_SWITCH_EXT
#include	<switch.h>
#endif
#if (FEATURE_PORT_VLAN == FYES)
extern void formDefinePortVlan(void);
#endif

#include "webmsg.h"
#include "flash_api.h"

//#ifdef SYS_USB  /*usb  KeHuatao*/
#undef CONFIG_USB
#undef CONFIG_USB_STORAGE
//#endif
/*********************************** Locals ***********************************/
/*
 *	Change configuration here
 */

static char_t		*rootWeb = T("/etc_ro/web");		/* Root web directory */
static char_t		*password = T("");				/* Security password */
int			port = 80;						/* Server port */
int			retries = 5;					/* Server port retries */
static int			finished;						/* Finished flag */
static char_t		*gopid = T("/var/run/goahead.pid");	/* pid file */

/****************************** Forward Declarations **************************/

static int	writeGoPid(void);
static int 	initSystem(void);
static int 	initWebs(void);
static int  websHomePageHandler(webs_t wp, char_t *urlPrefix, char_t *webDir,
				int arg, char_t *url, char_t *path, char_t *query);
extern void defaultErrorHandler(int etype, char_t *msg);
extern void defaultTraceHandler(int level, char_t *buf);
//extern void ripdRestart(void);
#ifdef B_STATS
static void printMemStats(int handle, char_t *fmt, ...);
static void memLeaks();
#endif
#ifdef CONFIG_RT2860V2_WIFI
extern void WPSAPPBCStartAll(void);
extern void WPSSingleTriggerHandler(int);
#endif
//#if defined CONFIG_USB
#if defined SYS_USB
extern void hotPluglerHandler(int);
#endif


#ifdef CONFIG_RT2860V2_STA_WSC
extern void WPSSTAPBCStartEnr(void);
#endif

#ifdef CONFIG_DUAL_IMAGE
static int set_stable_flag(void);
#endif
#ifdef CONFIG_UTT_NETFILTER
int uttNfNlSock;
#endif
#if (CONFIG_WEVO_MFG == FYES)
extern void formDefineWevoAte(void);
#endif
/*********************************** Code *************************************/
/*
 *	Main -- entry point from LINUX
 */

int main(int argc, char** argv)
{
/*
 *	Initialize the memory allocator. Allow use of malloc and start 
 *	with a 60K heap.  For each page request approx 8KB is allocated.
 *	60KB allows for several concurrent page requests.  If more space
 *	is required, malloc will be used for the overflow.
 */
#ifdef CONFIG_UTT_MT7628_FLASH_SOFTEN   
	unsigned int sn_ret = 0,md5_sn = 0;
	flash_read_mtdname("Bootloader", &sn_ret, 0x2c, 4);
	flash_read_mtdname("Bootloader", &md5_sn, 0x10, 4);

	if((sn_ret == 0xffffffff)&&(md5_sn == 0xffffffff))
	{
		char *p = NULL;
		printf("\n-----sn_ret = %x----md5_sn = %x-------------kill goahead ----------\n",sn_ret,md5_sn);
		strcpy(p,"kill goahead");/*制造一个段错误，在没有序列号的时候不启动goahead*/
	}
#endif

#ifdef CONFIG_UTT_SOFTLOCK_SPIFLASH
    unsigned int sn_ret = 0,md5_sn = 0;
    flash_read_mtdname("Bootloader", &sn_ret, 0x2c, 4);
    flash_read_mtdname("Bootloader", &md5_sn, 0x10, 4);

    if((sn_ret == 0xffffffff)&&(md5_sn == 0xffffffff))
    {
        /*char *p = NULL;
        printf("\n-----sn_ret = %x----md5_sn = %x-------------kill goahead ----------\n",sn_ret,md5_sn);
        strcpy(p,"kill goahead");*//*制造一个段错误，在没有序列号的时候不启动goahead*/
		printf("\n************please check your sn****************\n");
		return -1;
    }
#endif

	struct sigaction act_pipe;
	struct sched_param param; 
	int maxpri; 

	act_pipe.sa_handler = SIG_IGN;
	act_pipe.sa_flags |= SA_RESTART;

	bopen(NULL, (60 * 1024), B_USE_MALLOC);
	sigaction(SIGPIPE, &act_pipe, NULL);
    maxpri = sched_get_priority_max(SCHED_OTHER);
    param.sched_priority = maxpri; 
    if (sched_setscheduler(getpid(), SCHED_OTHER, &param) == -1)
    {
        perror("set sched rror\n");
    };
#if 0
	signal(SIGPIPE, SIG_IGN);
#endif


	if (writeGoPid() < 0)
		return -1;


#ifdef CONFIG_UTT_NETFILTER
	/**
	 * 防火墙netlink初始化
	 * 若需接受需重启一个线程。或修改goahead的select函数相关
	 */
	uttNfNlSock = uttNfNlSkBind(getpid(), 0);/*单播到本进程*/
	if(uttNfNlSock < 0)
	{
		return -1;
	}
#endif

    if(ProfInit() == PROFFAIL) {
		return -1;
    }
	if (initSystem() < 0)
		return -1;
/*
 *	Initialize the web server
 */
	if (initWebs() < 0) {
		return -1;
	}

#ifdef CONFIG_DUAL_IMAGE
/* Set stable flag after the web server is started */
	set_stable_flag();
#endif

#ifdef WEBS_SSL_SUPPORT
	websSSLOpen();
#endif

/*
 *	Basic event loop. SocketReady returns true when a socket is ready for
 *	service. SocketSelect will block until an event occurs. SocketProcess
 *	will actually do the servicing.
 */
	while (!finished) {
		if (socketReady(-1) || socketSelect(-1, 1000)) {
			socketProcess(-1);
		}
		websCgiCleanup();
		emfSchedProcess();
	}

#ifdef WEBS_SSL_SUPPORT
	websSSLClose();
#endif

#ifdef USER_MANAGEMENT_SUPPORT
	umClose();
#endif

/*
 *	Close the socket module, report memory leaks and close the memory allocator
 */
	websCloseServer();
	socketClose();
#ifdef B_STATS
	memLeaks();
#endif
	bclose();
	return 0;
}
/******************************************************************************/
/*
 *	Write pid to the pid file
 */
int writeGoPid(void)
{
	FILE *fp;

	fp = fopen(gopid, "w+");
	if (NULL == fp) {
		error(E_L, E_LOG, T("goahead.c: cannot open pid file"));
		return (-1);
	}
	fprintf(fp, "%d", getpid());
    fclose(fp);
	return 0;
}

#if 0//def CONFIG_RALINK_RT2880
static void goaSigHandler(int signum)
{
#ifdef CONFIG_RT2860V2_STA_WSC
	char *opmode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	char *ethCon = nvram_bufget(RT2860_NVRAM, "ethConvert");
#endif

	if (signum != SIGUSR1)
		return;

#ifdef CONFIG_RT2860V2_STA_WSC
	if(!strcmp(opmode, "2") || (!strcmp(opmode, "0") &&   !strcmp(ethCon, "1") ) )		// wireless isp mode
		WPSSTAPBCStartEnr();	// STA WPS default is "Enrollee mode".
	else
#endif
#ifdef CONFIG_RT2860V2_WIFI
		WPSAPPBCStartAll();
#endif
}
#endif

#if 0
static void goaInitGpio()
{
	int fd;
	ralink_gpio_reg_info info;

	fd = open("/dev/gpio", O_RDONLY);
	if (fd < 0) {
		perror("/dev/gpio");
		return;
	}
	//set gpio direction to input
	if (ioctl(fd, RALINK_GPIO_SET_DIR_IN, RALINK_GPIO(0)) < 0)
		goto ioctl_err;
	//enable gpio interrupt
	if (ioctl(fd, RALINK_GPIO_ENABLE_INTP) < 0)
		goto ioctl_err;
	//register my information
	info.pid = getpid();
	info.irq = 0;
	if (ioctl(fd, RALINK_GPIO_REG_IRQ, &info) < 0)
		goto ioctl_err;
	close(fd);

	//issue a handler to handle SIGUSR1
	signal(SIGUSR1, goaSigHandler);
	return;

ioctl_err:
	perror("ioctl");
	close(fd);
	return;
}
#endif

#if 0
static void dhcpcHandler(int signum)
{
//	ripdRestart();
}
#endif

/******************************************************************************/
/*
 *	Initialize System Parameters
 */
static int initSystem(void)
{
	int setDefault(void);

	if (setDefault() < 0)
		return (-1);

//#if defined CONFIG_USB
#if defined SYS_USB
	signal(SIGTTIN, hotPluglerHandler);
	hotPluglerHandler(SIGTTIN);
#endif
#if 0//def CONFIG_RALINK_RT2880
	signal(SIGUSR1, goaSigHandler);
#else
#if 0
	goaInitGpio();
#endif
#endif
#ifdef CONFIG_RT2860V2_WIFI
//	signal(SIGXFSZ, WPSSingleTriggerHandler);
#endif

	return 0;
}

unsigned int productSn;

#if 0
#if (CONFIG_ATHEROS_AR71XX == 1)
unsigned int getSystemSn()
{
    int sn;
    flash_read_mtdname("Bootloader", (char*)&sn, 0x2c, 4);
    return sn;
}
#elif ((CONFIG_X86_CPU) == 1)
unsigned int getSystemSn()
{
    int sn;
    flash_read_mtdname("/dev/sda4", (char*)&sn, 0, 4);
    return sn;
}
#endif

#if (CONFIG_RALINK_RT3052 == 1 || CONFIG_RALINK_RT3883 == 1)
unsigned int getSystemSn()
{
    int fd;
    volatile unsigned   int sn;
    void * map_base;

    if((fd=open("/dev/mem",O_RDWR|O_SYNC))==-1){
        return(-1);
    }

    map_base=mmap(0,0xff, PROT_READ, MAP_SHARED, fd, 0xbf000000);
    sn = *((volatile unsigned int *) (map_base+0x2c));
    munmap(map_base, 0xff);
    close(fd);
    return sn;
}
#endif
#endif

#if 0
static void initWdg()
{
    char* wdgEn;
    wdgEn = nvram_bufget(RT2860_NVRAM, "WDGENABLE");	
    if(strcmp(wdgEn, "on") == 0){
	    doSystem("insmod rt_timer");
    }
    return;
}
#endif

/******************************************************************************/
/*
 *	Set Default should be done by nvram_daemon.
 *	We check the pid file's existence.
 */
int setDefault(void)
{
#if 0
	FILE *fp;
	unsigned int i = 0, j = 0;
    char buftmp[MAX_UTT_CONF_NAME_LEN];
    unsigned int wanCount = 0;
#endif
#if (WIRELESS == FYES)
	char_t uttSsid[20] = {0};
#endif
	unsigned int sn = 0;
    char *tmp = NULL;
    char productID[30] ={0};
    struct utsname utsbuf;


	/*
     * retry 15 times (15 seconds)
     */
#if 0
	for (i = 0; i < 15; i++) {
		fp = fopen("/var/run/nvramd.pid", "r");
		if (fp == NULL) {
			if (i == 0)
				trace(0, T("goahead: waiting for nvram_daemon "));
			else
				trace(0, T(". "));
		} else {
			fclose(fp);
#endif

		    productSn = sn = getSystemSn();
            uname(&utsbuf);
            tmp = strtok(utsbuf.machine, " ");
            tmp = strtok(NULL, " ");

            if(tmp != NULL)
            {
                sprintf(productID,"%s",tmp);
            }
            else
            {
                sprintf(productID,"%s",utsbuf.machine);     
            }

#if 0
		    initWdg();
#endif
#if (WIRELESS == FYES)
			MibProfileType mibType = MIB_PROF_WIRELESS;
			WirelessProfile *prof= NULL;
			prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,0);
            if(!strcmp(prof->mBaseCf.SSID1,"")){
#if(UTT_TOMAX == FYES)
				sprintf(uttSsid,"ToMAX_%X", productSn);
#elif(BRAND_LEVEL_ONE == FYES)
				sprintf(uttSsid,"LevelOne_%X", productSn);
#elif(GALAXYWIND == FYES)
				sprintf(uttSsid,"GALAXYWIND_%X", productSn);
#elif (MIDDLE_SOFT == FYES)
#if 0
                productSn += ((productSn & 0x3fff) << 3);
				productSn += 0x2711;
				productSn <<= 4;
#endif
#if (SKYWORTH == FYES)
                sprintf(uttSsid,"Skyworth%s",productID+strlen(productID)-4);
#else
				sprintf(uttSsid,"ChinaNet-2.4G-%02X%02X", (0xFF & (productSn >> 8)),(0xFF & (productSn)));
#endif
#else
				sprintf(uttSsid,"UTT-HIPER_%X", productSn);
#endif
#if (WRITE_NEWMAC != FYES)
				strcpy(prof->mBaseCf.SSID1,uttSsid);
#endif
			}
#if (WIRELESS_5G == FYES)
			prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,1);
            if(!strcmp(prof->mBaseCf.SSID1,"")){
#if(UTT_TOMAX == FYES)
				sprintf(uttSsid,"ToMAX-5G_%X", productSn);
#elif(BRAND_LEVEL_ONE == FYES)
                sprintf(uttSsid,"LevelOne_5G_%X", productSn);
#elif(GALAXYWIND == FYES)
				sprintf(uttSsid,"GALAXYWIND-5G_%X", productSn);
#elif(AUTO_CHANNEL == FYES)
				sprintf(uttSsid,"Chinanet-5G-%02X%02X",(0xFF & (productSn >> 8)),(0xFF & (productSn)));
#if (SKYWORTH == FYES)
                sprintf(uttSsid,"Skyworth%s-5G",productID+strlen(productID)-4);
#endif
#else
				sprintf(uttSsid,"UTT-HIPER-5G_%X", productSn);
#endif
				strcpy(prof->mBaseCf.SSID1,uttSsid);
			}
#endif
#if 0
		    ssid = nvram_bufget(RT2860_NVRAM,"SSID1");	
            if(!strcmp(ssid,"")){
			sprintf(uttSsid,"UTT-HIPER_%x", productSn);
		        nvram_set(RT2860_NVRAM,"SSID1",uttSsid);
            }
#endif
#endif
			return 0;
#if 0
		}
		Sleep(1);
	}

	error(E_L, E_LOG, T("goahead: please execute nvram_daemon first!"));
	return (-1);
#endif
}
#if SW_FEATURE_URCP
extern void formDefineRrcpDiscovery(void);
extern void formDefineRemoteUrcp(void);
extern void formDefineTopoyDiscovery(void);
extern void formDefineConfUrcp(void);
extern void formDefineSysConfUrcp(void);
extern void formDefineAggrUrcp(void);
extern void formDefinePortVlan(void);
extern void formDefinePortVlanUrcp(void);
extern void formDefineMirrorUrcp(void);
extern void formDefinePortConfigureUrcp(void);
#endif
#if (FEATURE_AC == FYES)
extern void formDefineApManage(void);
extern void frmDefineRemoteUrcp(void);
extern void formDefineApWlanCltList(void);
#endif
/******************************************************************************/
/*
 *	Initialize the web server.
 */

static int initWebs(void)
{
	struct in_addr	intaddr;
#ifdef GA_HOSTNAME_SUPPORT
	struct hostent	*hp;
	char			host[128];
#endif
	char			webdir[128];
	char			*cp;
	char_t			wbuf[128];

    InterfaceProfile *profIF = NULL;
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);
/*
 *	Initialize the socket subsystem
 */
	socketOpen();
    

#ifdef USER_MANAGEMENT_SUPPORT
/*
 *	Initialize the User Management database
 */
#if 0
	char *admu = nvram_bufget(RT2860_NVRAM, "Login");
	char *admp = nvram_bufget(RT2860_NVRAM, "Password");
	umOpen();
	//umRestore(T("umconfig.txt"));
	//winfred: instead of using umconfig.txt, we create 'the one' adm defined in nvram
	umAddGroup(T("adm"), 0x07, AM_DIGEST, FALSE, FALSE);
	if (admu && strcmp(admu, "") && admp && strcmp(admp, "")) {
		umAddUser(admu, admp, T("adm"), FALSE, FALSE);
		umAddAccessLimit(T("/"), AM_DIGEST, FALSE, T("adm"));
	}
	else
		error(E_L, E_LOG, T("gohead.c: Warning: empty administrator account or password"));
#else
	umOpen();
#if (UTT_SMART_UI == FYES)
	umAddGroup(T("adm"), 0x07, AM_BASIC, FALSE, FALSE);
	umAddAccessLimit(T("/"), AM_BASIC, FALSE, T("adm"));
#else
	umAddGroup(T("adm"), 0x07, AM_DIGEST, FALSE, FALSE);
	umAddAccessLimit(T("/"), AM_DIGEST, FALSE, T("adm"));
#endif
#endif
#if (ADMIN_CONF == FYES)
extern void init_admin(void);
	init_admin();
#endif
#endif

#if (NET_MODE_SWITCH == FYES)
	 NetModeSwitchProfile *profNM = NULL;
	BridgeProfile *profBR = NULL;
	profNM = (NetModeSwitchProfile *)ProfGetInstPointByIndex(MIB_PROF_NET_MODE_SWITCH,0);
	if((profNM!=NULL) && (profNM->bridgeEnable==FUN_ENABLE))
	{
		profBR = (BridgeProfile *)ProfGetInstPointByIndex(MIB_PROF_BRIDGE,0);
		if(profBR!=NULL)
		{
			intaddr.s_addr = profBR->ip;
		}
		else
		{
			intaddr.s_addr = inet_addr("192.168.1.1");
		}
	}else{
#endif

#ifdef GA_HOSTNAME_SUPPORT
/*
 *	Define the local Ip address, host name, default home page and the 
 *	root web directory.
 */
	if (gethostname(host, sizeof(host)) < 0) {
		error(E_L, E_LOG, T("gohead.c: Can't get hostname"));
		return -1;
	}
	if ((hp = gethostbyname(host)) == NULL) {
		error(E_L, E_LOG, T("gohead.c: Can't get host address"));
		return -1;
	}
	memcpy((char *) &intaddr, (char *) hp->h_addr_list[0],
		(size_t) hp->h_length);
#else
/*
 * get ip address from nvram configuration (we executed initInternet)
 */
    if(profIF != NULL)  {
	    intaddr.s_addr = profIF->ether.sta.ip;
    } else {
	    intaddr.s_addr = inet_addr("192.168.1.1");
    }
	if (intaddr.s_addr == INADDR_NONE) {
		error(E_L, E_LOG, T("initWebs: failed to convert ip data"));
		return -1;
	}
#endif

#if (NET_MODE_SWITCH == FYES)
	    }
#endif

/*
 *	Set rootWeb as the root web. Modify this to suit your needs
 */
	sprintf(webdir, "%s", rootWeb);

/*
 *	Configure the web server options before opening the web server
 */
	websSetDefaultDir(webdir);
	cp = inet_ntoa(intaddr);
	ascToUni(wbuf, cp, min(strlen(cp) + 1, sizeof(wbuf)));
	websSetIpaddr(wbuf);
#ifdef GA_HOSTNAME_SUPPORT
	ascToUni(wbuf, host, min(strlen(host) + 1, sizeof(wbuf)));
#else
	//use ip address (already in wbuf) as host
#endif
	websSetHost(wbuf);

/*
 *	Configure the web server options before opening the web server
 */
	websSetDefaultPage(T("default.asp"));
	websSetPassword(password);

/* 
 *	Open the web server on the given port. If that port is taken, try
 *	the next sequential port for up to "retries" attempts.
 */
	websOpenServer(port, retries);

/*
 * 	First create the URL handlers. Note: handlers are called in sorted order
 *	with the longest path handler examined first. Here we define the security 
 *	handler, forms handler and the default web page handler.
 */
	websUrlHandlerDefine(T(""), NULL, 0, websSecurityHandler, 
		WEBS_HANDLER_FIRST);
	websUrlHandlerDefine(T("/goform"), NULL, 0, websFormHandler, 0);
	websUrlHandlerDefine(T("/cgi-bin"), NULL, 0, websCgiHandler, 0);
#if (FEATURE_AC == FYES)
	websUrlHandlerDefine(T("/api"), NULL, 0, websApiHandler, 0);
#endif
	websUrlHandlerDefine(T(""), NULL, 0, websDefaultHandler, 
		WEBS_HANDLER_LAST); 

/*
 *	Define our functions
 */
	formDefineUtilities();
	formDefineInternet();
extern void formDefineIsp(void);
    formDefineIsp();
extern void formDefineMultiPath(void);
    formDefineMultiPath();

#if (HTTP_CACHE == FYES)
	extern void formDefineCache(void);
	formDefineCache();
#endif
#if (AP_PERMISSION == FYES)
	extern void formDefineApPermission(void);
	formDefineApPermission();
#endif

#if (AP_CONF_MANAGE == FYES)
	extern void formDefineApConfManage(void);
	formDefineApConfManage();
#endif

#if (AP_DRAWING == FYES)
	extern void apDrawingInit(void);
	apDrawingInit();
	extern void formDefineApDrawing(void);
	formDefineApDrawing();
#endif

#if (SERVICE_ZONE == FYES)
	extern void formDefineServiceZone(void);
	formDefineServiceZone();
#endif
#if (TAG_VLAN == FYES)
	extern void formDefineTagVlan(void);
	formDefineTagVlan();
#endif

#if (DHCP_POOL == FYES)
	extern void formDefineDhcpPool(void);
	formDefineDhcpPool();
#endif

#if (VLAN_CONFIG == FYES)
	extern void formDefineVlanConfig(void);
	formDefineVlanConfig();
#endif

#if (DNS_AGENCY == FYES)
	extern void formDefineDnsAgency(void);
	formDefineDnsAgency();
#endif

#if (ARP_BINDS == FYES)
extern void formDefineArpBindConfig(void);
	formDefineArpBindConfig();
#endif

#if (STATIC_NAT == FYES)
extern void formDefineNatMapConfig(void);
	formDefineNatMapConfig();
#endif
#if (NAT_RULE == FYES)
extern void formDefineNatRuleConfig(void);
	formDefineNatRuleConfig();
#endif
#if (ADMIN_CONF == FYES)
extern void formDefineAdmin(void);
	formDefineAdmin();
#endif
#if (DNS_FILTER == FYES)
extern void formDefineDnsFilter(void);
	formDefineDnsFilter();
#endif
#if (CLI_FOR_ENGINEER_ONLY == FYES)
extern void formDefineCliForEngineerOnly(void);
    formDefineCliForEngineerOnly();
#endif
#if (CONNECT_LIMIT == FYES)
extern void formDefineConnLimit(void);
	formDefineConnLimit();
#endif
#if (SYSLOG == FYES)
extern void formDefineSyslog(void);
	formDefineSyslog();
#endif
#if ((SSH_CLI == FYES)||(HTTPS == FYES))
extern void formDefineDeviceLoginControl(void);
	formDefineDeviceLoginControl();
#endif
#if (HARDWARE_NAT == FYES)
extern void formDefineHardwareNAT(void);
	formDefineHardwareNAT();
#endif
#if (IDENTIFY_BIND == FYES)
extern void formDefineIdentifyBind(void);
	formDefineIdentifyBind();
#endif
#if (EXCEPT_QQ == FYES)
extern void formDefineExceptQQ(void);
	formDefineExceptQQ();
#endif
#if (EXCEPT_MSN == FYES)
extern void formDefineExceptMSN(void);
	formDefineExceptMSN();
#endif
#if (EXCEPT_ALI == FYES)
extern void formDefineExceptAli(void);
	formDefineExceptAli();
#endif
#if (FAT_FIT_SWITCH == FYES)
extern void formDefineFatFitSwitch(void);
	formDefineFatFitSwitch();
#endif
#if (WIRELESS == FYES)
extern void formDefineWirelessMacFilter(void);
	formDefineWirelessMacFilter();
extern void formDefineWirelessHostList(void);
	formDefineWirelessHostList();
#endif
#if (GROUP_BASE == FYES)
extern void formDefineGroupConfig(void);
	formDefineGroupConfig();
extern void formDefineAdvideo(void);
    formDefineAdvideo();
#endif
#if (UTTPDB == FYES)
extern void formDefinePdbConfig(void);
	formDefinePdbConfig();
#endif
#if (DMZ == FYES)
extern void formDefineDMZConfig(void);
	formDefineDMZConfig();
#endif
#if (HOTEL_PNP == FYES)
extern void formDefineHotel(void);
    formDefineHotel();
#endif
#if 0
extern void formDefineConfigFile(void);
	formDefineConfigFile();
#endif
#if (UPNP_GLOBAL_ENABLE == FYES)
extern void formDefineUpnpConfig(void);
	formDefineUpnpConfig();
#endif
extern void formDefineSoftwareUpload(void);
	formDefineSoftwareUpload();

extern void formDefinePppoeServer(void);
	formDefinePppoeServer();

#if (TASK_SCHEDULER == FYES)
extern void formDefineTaskScheduler(void);
	formDefineTaskScheduler();
#endif
#if (PPTP_CLIENT == FYES)
extern void formDefinePptpClient(void);
    formDefinePptpClient();
#endif
#if (DHCP_SERVER == FYES)
extern void formDefineDhcpServer(void);
    formDefineDhcpServer();
#endif
#if (P2P_LIMIT == FYES)
extern void formDefineP2PLimit(void);
	formDefineP2PLimit();
#endif
#if (EASY_TASK == FYES)
extern void formDefineEasyTask(void);
	formDefineEasyTask();
#endif
#if defined CONFIG_RALINKAPP_MPLAYER
	formDefineMedia();
#endif

#if (HTTP_CACHE == FYES)
extern void formDefineCache(void);
formDefineCache();
#endif
#if (NET_MODE_SWITCH == FYES)
extern void formDefineNetModeSwitch(void);
   formDefineNetModeSwitch();
#endif
#if (BRIDGE == FYES)
//extern void formDefineBridge(void);
//   formDefineBridge();
extern void formDefineCacheMode(void);
formDefineCacheMode();
#endif
#if (CACHE_SERVER == FYES)
extern void formDefineCacheServer(void);
    formDefineCacheServer();
#endif

#if (NET_SNIPER == FYES)
extern void formDefineNetSniper(void);
	formDefineNetSniper();
#endif

#if 0
	formDefineWireless();
#endif
#if defined INIC_SUPPORT || defined INICv2_SUPPORT
	formDefineInic();
#endif
#if defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
	formDefineLegacy();
#endif
//#if defined CONFIG_RT2860V2_STA || defined CONFIG_RT2860V2_STA_MODULE
#if 0
	formDefineStation();
#endif
//#endif
	formDefineFirewall();
	formDefineManagement();
extern void formDefineMultiPath(void);
    formDefineMultiPath();

#if (SETTINGS_CONF == FYES)
	extern void formDefineSettingsConf(void);
	formDefineSettingsConf();
#endif

#if (L2TP_SERVER == FYES)
	extern void formDefineL2tpServer(void);
	formDefineL2tpServer();
#endif

#if (L2TP_CLIENT == FYES)
extern void formDefineL2tpClient(void);
	formDefineL2tpClient();
#endif

#if (PPTP_SERVER == FYES)
	extern void formDefinePptpServer(void);
	formDefinePptpServer();
#endif

#if (IP_SEC == FYES)
	extern void formDefineIPSec(void);
	formDefineIPSec();
#endif

#if (UTT_LICENSE == FYES)
 	extern void formDefineLicense(void);
 	formDefineLicense();
#endif
 
#if (STATIC_ROUTE == FYES)
	extern void websDefineStaticRoute(void);
	websDefineStaticRoute();
#endif

#if (NOTICE == FYES)
	extern void websDefineNotice(void);
	websDefineNotice();
#endif

#if (CWMP == FYES)
    extern void websDefineCwmp(void);
    websDefineCwmp();
#endif

	extern void websDefineWebAuth(void);
	websDefineWebAuth();

#if (LOG_MANAGE == FYES)
	extern void websDefineLogManage(void);
	websDefineLogManage();
#endif

#if (AP_LOG_MANAGE == FYES)
extern void websDefineApLogManage(void);
	websDefineApLogManage();
#endif
#if (AP_LOAD_BALANCE == FYES )
    extern void websDefineApLoadBalance(void);
    websDefineApLoadBalance();
#endif


#if (NET_SECURITY == FYES)
	extern void websDefineFwBase(void);
	websDefineFwBase();
#endif

#if (DDNS == FYES)
	extern void websDefineDdns(void);
	websDefineDdns();
#endif
#if (FEATURE_SW_SNMP == FYES)
    extern void websDefineSnmp(void);
    websDefineSnmp();
#endif
#if (SNMP == FYES)
    extern void websDefineSNMP(void);
    websDefineSNMP();
#endif

#if (WEBAUTH_AUTO == FYES)
	void websDefineCaptcha(void);
	websDefineCaptcha();
#endif
#if (IPV6_INTERFACE == FYES)
    extern void websDefineIpv6Interface(void);
    websDefineIpv6Interface();
#endif
#if(IPV6_NOTLISTPREFIXS == FYES)
extern  void formDefineIpv6PrefixList(void);
    formDefineIpv6PrefixList();
#endif
#if (IPV6_DHCPV6 == FYES)
    extern void websDefineDhcp6(void);
    websDefineDhcp6();
#endif

#if (IPV6_RADVD == FYES)
extern void websDefineRADVDConfig(void);
    websDefineRADVDConfig();
#endif
    extern void formDefineAssit(void);
    formDefineAssit();
#if(IPV6_ROUTE == FYES)
extern  void formDefineIpv6RouteConfig(void);
    formDefineIpv6RouteConfig();
#endif
#if(IPV6_TUNNEL == FYES)
extern  void formDefineTunnelConfig(void);
    formDefineTunnelConfig();
#endif
#if(IPV6_ISATAP == FYES)
extern  void formDefineIsatapConfig(void);
    formDefineIsatapConfig();
#endif
	extern void websDefineLanConfig(void);
	websDefineLanConfig();

	extern void formDefineAssit(void);
	formDefineAssit();

#if FEATURE_SW_PORT_CONF
	extern void formDefinePortConfigure(void);
	formDefinePortConfigure();
#endif

	extern void formDefineTopPage(void);
	formDefineTopPage();

#if (UTT_SMART_UI == FYES)
	extern void formDefineUserLinkRate(void);
	formDefineUserLinkRate();
#else
#if (USER_STATUS == FYES)
	extern void formDefineUserLinkRate(void);
	formDefineUserLinkRate();
#endif
#endif/*end of UTT_SMART_UI*/

#if (FEATURE_PORT_VLAN == FYES)
	formDefinePortVlan();
#endif

#if (PVLAN_REDUCED == FYES)
	extern void formDefinePortVlan(void) ;
	formDefinePortVlan();
#endif

#if (FEATURE_SW_PORT_MIRROR == FYES)
	extern void formDefineMirror(void);
	formDefineMirror();
#endif
#if (FEATURE_SW_PORT_TRUNK == FYES)
	extern void formDefineAggr(void);
	formDefineAggr();
#endif
#if (SMART_QOS_FUN == FYES)
    extern void formDefineSmartQos(void);
    formDefineSmartQos();
#endif
#if (ANTI_SNIFFER == FYES)
    extern void formDefineAntiNetSniffer(void);
    formDefineAntiNetSniffer();
#endif
#if (PORT_MIRROR == FYES)
	extern void formDefinePortMirror(void);
	formDefinePortMirror();
#endif
#if (PORT_MIRROR_6530G == FYES)
	extern void formDefinePortMirror6530G(void);
	formDefinePortMirror6530G();
#endif
#if (FIREWALL == FYES)
extern void formDefinefirewallConfig(void);
	formDefinefirewallConfig();
#endif
#if (WIRELESS == FYES)
extern void formDefineWirelessSecurity(void);
	formDefineWirelessSecurity();
extern void formDefineWireAdvancedConfig(void);
	formDefineWireAdvancedConfig();
#if (WIRELESS_BASE_CONFIG == FYES)
extern void formDefineWireBaseConfig(void);
	formDefineWireBaseConfig();
#endif
#endif
#if (POLICY_ROUTE == FYES)
extern void formDefinePolicyRouteConfig(void);
	formDefinePolicyRouteConfig();
#endif

#if (FEATURE_PORT_STATISTICS==FYES)
	extern void formDefinePortStatics(void);
	formDefinePortStatics();
#endif
#if (IP_GRP == FYES)
extern void websDefineIpGrp(void);
	websDefineIpGrp();
#endif
#if (PORT_GRP == FYES)
extern void websDefinePortGrp(void);
	websDefinePortGrp();
#endif
#if (TIME_GRP == FYES)
extern void websDefineTimeGrp(void);
	websDefineTimeGrp();
#endif
#if (PARENTS_CONTROL == FYES)
extern void formDefineParentsControlConf(void);
	formDefineParentsControlConf();
#endif

#if SW_FEATURE_URCP
	urcp_ipc_init();
	formDefineRrcpDiscovery();
	formDefineRemoteUrcp();
	formDefineTopoyDiscovery();
	formDefineConfUrcp();
	formDefineSysConfUrcp();
	formDefineAggrUrcp();
	formDefinePortVlanUrcp();
	formDefinePortConfigureUrcp();
	formDefineMirrorUrcp();
#elif (FEATURE_AP == FYES)
	urcp_ipc_init();
#elif (FEATURE_AC == FYES)
	urcp_ipc_init();
	formDefineRemoteUrcp();
	formDefineRrcpDiscovery();
	formDefineConfUrcp();
#endif

#if (FEATURE_AC == FYES)
#if (AC_AP_SYS_INFO == FYES)
	extern void formDefineApSysInfo(void);
	formDefineApSysInfo();
#endif
#endif 

extern void initcurLang(void);
extern void formDefineLangTran(void);
	formDefineLangTran();
	initcurLang();

extern void websDefineDynConfig(void);
	websDefineDynConfig();
/*
 *	Create the Form handlers for the User Management pages
 */
#ifdef USER_MANAGEMENT_SUPPORT
	//formDefineUserMgmt();  winfred: we do it ourselves
#endif
#if (FEATURE_AC == FYES)
    formDefineApManage();
	formDefineApWlanCltList();
#if (FEATURE_AP_CONF_TEMP == FYES)
	extern void formDefineApConfTemp(void);
	formDefineApConfTemp();

extern void formDefinePushConfTemp(void);
	formDefinePushConfTemp();
#endif
#if (FEATURE_AP_BASIC_CONF == FYES)
	extern void formDefineApBasicConfUrcp(void);
	formDefineApBasicConfUrcp();
#endif
#if (FEATURE_AP_JOIN_SZ == FYES)
	extern void formDefineApJoinSZ(void);
	formDefineApJoinSZ();
#endif
#if (FEATURE_AP_MAC_FILTER == FYES)
	extern void formDefineApMacFilter(void);
	formDefineApMacFilter();
#endif
#endif
extern void formDefineLangTran(void);
	formDefineLangTran();

#if (FEATURE_AP_SW_MNG == FYES)
extern void formDefineApSoftwareManage(void);
	formDefineApSoftwareManage();
#endif
#if (FEATURE_AP_BASIC_INFO == FYES)
	extern void formDefineApBasicInfo(void);
	formDefineApBasicInfo();
#endif
#if (FEATURE_AP_EFFECT_CONF == FYES)
extern void formDefineApEffectConf(void);
	formDefineApEffectConf();
#endif
#if (FEATURE_AC == FYES)
extern void formDefinePushSoftware(void);
	formDefinePushSoftware();
#endif
#if (FIT_AP_LOCAL_CONF_TEMP == FYES)
extern void formDefineFitApLocalConfTemp(void);
	formDefineFitApLocalConfTemp();
#endif
#if (SYS_LOG_INFO == FYES)
extern void websDefineSysLogInfo(void);
	websDefineSysLogInfo();
#endif
#if (MAC_FILTER == FYES)
extern void formDefineMacFilter(void);
	formDefineMacFilter();
#endif
#if (AC_WORK_MODE == FYES)
extern void formDefineAcWorkMode(void);
	formDefineAcWorkMode();
#endif
#if (CONFIG_WEVO_MFG == FYES)
    formDefineWevoAte();
#endif

#if (NET_SHARE_MANAGE == FYES)
    extern void formDefineNetShareManage(void);
    formDefineNetShareManage();
#endif
#if (FTP_SERVER == FYES)
    extern void formDefineFtpServer(void);
    formDefineFtpServer();
#endif
#if (NET_SHARE_USER== FYES)
    extern void formDefineNetShareUser(void);
    formDefineNetShareUser();
#endif

/*
 *	Create a handler for the default home page
 */
#if (UTT_SMART_UI == FYES)
extern void formDefineNetworkStatus(void);
	formDefineNetworkStatus();

extern void formDefineAppManage(void);
	formDefineAppManage();
#endif
#if (DIAGNOSE_DETECT == FYES)
extern void formDefineDiagnose(void);
	formDefineDiagnose();
#endif

#if (ADFILTER == FYES) 
extern void formDefineAdfilter(void);
	formDefineAdfilter();
#endif
#if (MAKE_MONEY == FYES)
extern void formDefineMakeMoney(void);
    formDefineMakeMoney();
#endif
extern void websDefineAppType(void);
	websDefineAppType();
	websUrlHandlerDefine(T("/"), NULL, 0, websHomePageHandler, 0); 
	return 0;
}

/******************************************************************************/
/*
 *	Home page handler
 */

static int websHomePageHandler(webs_t wp, char_t *urlPrefix, char_t *webDir,
	int arg, char_t *url, char_t *path, char_t *query)
{
/*
 *	If the empty or "/" URL is invoked, redirect default URLs to the home page
 */
	if (*url == '\0' || gstrcmp(url, T("/")) == 0) {
#if (UTT_SMART_UI == FYES)
		websRedirect(wp, T("index_login.asp"));
#else
		websRedirect(wp, T("index.asp"));
#endif
		return 1;
	}
	return 0;
}

/******************************************************************************/
/*
 *	Default error handler.  The developer should insert code to handle
 *	error messages in the desired manner.
 */

void defaultErrorHandler(int etype, char_t *msg)
{
	write(1, msg, gstrlen(msg));
}

/******************************************************************************/
/*
 *	Trace log. Customize this function to log trace output
 */

void defaultTraceHandler(int level, char_t *buf)
{
/*
 *	The following code would write all trace regardless of level
 *	to stdout.
 */
	if (buf) {
		if (0 == level)
			write(1, buf, gstrlen(buf));
	}
}

/******************************************************************************/
/*
 *	Returns a pointer to an allocated qualified unique temporary file name.
 *	This filename must eventually be deleted with bfree();
 */
/*add by chen.bin 
 * HTTP_CACHE compile with glibc,replace tempnam() to mkstemp()
 * */
#if (HTTP_CACHE == FYES)

#if 0
char_t *websGetCgiCommName(webs_t wp)
{
	char *force_mem_upgrade = nvram_bufget(RT2860_NVRAM, "Force_mem_upgrade");
	char_t	*pname1 = NULL, *pname2 = NULL;
	char *part;

	if(!strcmp(force_mem_upgrade, "1")){
		pname1 = (char_t *)mkstemp( T("cgi"));
	}else if(wp && (wp->flags & WEBS_CGI_FIRMWARE_UPLOAD) ){
		// see if usb disk is present and available space is enough?
		if( (part = isStorageOK()) )
			pname1 = (char_t *)mkstemp(T("cgi"));
		else
			pname1 = (char_t *)mkstemp( T("cgi"));
	}else{
		pname1 = (char_t *)mkstemp(T("cgi"));
	}

	pname2 = bstrdup(B_L, pname1);
	free(pname1);

	return pname2;
}
#else
char_t *websGetCgiCommName(webs_t wp)
{
	char_t	*pname1, *pname2;

	pname1 = (char_t *)mkstemp( T("cgi"));
	pname2 = bstrdup(B_L, pname1);
	free(pname1);

	return pname2;
}
#endif

#else

//#if defined CONFIG_USB_STORAGE && defined CONFIG_USER_STORAGE
/* by ydh */
#if 0
char_t *websGetCgiCommName(webs_t wp)
{
	char *force_mem_upgrade = nvram_bufget(RT2860_NVRAM, "Force_mem_upgrade");
	char_t	*pname1 = NULL, *pname2 = NULL;
	char *part;

	if(!strcmp(force_mem_upgrade, "1")){
		pname1 = (char_t *)tempnam(T("/var"), T("cgi"));
	}else if(wp && (wp->flags & WEBS_CGI_FIRMWARE_UPLOAD) ){
		// see if usb disk is present and available space is enough?
		if( (part = isStorageOK()) )
			pname1 = (char_t *)tempnam(part, T("cgi"));
		else
			pname1 = (char_t *)tempnam(T("/var"), T("cgi"));
	}else{
		pname1 = (char_t *)tempnam(T("/var"), T("cgi"));
	}

	pname2 = bstrdup(B_L, pname1);
	free(pname1);

	return pname2;
}
#else
char_t *websGetCgiCommName(webs_t wp)
{
	char_t	*pname1, *pname2;

#ifdef CONFIG_PPC_85xx
	pname1 = (char_t *)mkstemp( T("cgi"));
#else
	pname1 = (char_t *)tempnam(T("/var"), T("cgi"));
#endif
	pname2 = bstrdup(B_L, pname1);
	free(pname1);

	return pname2;
}
#endif
#endif
/******************************************************************************/
/*
 *	Launch the CGI process and return a handle to it.
 */

int websLaunchCgiProc(char_t *cgiPath, char_t **argp, char_t **envp,
					  char_t *stdIn, char_t *stdOut)
{
	int	pid, fdin, fdout, hstdin, hstdout, rc;

	fdin = fdout = hstdin = hstdout = rc = -1; 
	if ((fdin = open(stdIn, O_RDWR | O_CREAT, 0666)) < 0 ||
		(fdout = open(stdOut, O_RDWR | O_CREAT, 0666)) < 0 ||
		(hstdin = dup(0)) == -1 ||
		(hstdout = dup(1)) == -1 ||
		dup2(fdin, 0) == -1 ||
		dup2(fdout, 1) == -1) {
		goto DONE;
	}

 	rc = pid = fork();
 	if (pid == 0) {
/*
 *		if pid == 0, then we are in the child process
 */
		if (execve(cgiPath, argp, envp) == -1) {
			printf("content-type: text/html\n\n"
				"Execution of cgi process failed\n");
		}
		exit (0);
	} 

DONE:
	if (hstdout >= 0) {
		dup2(hstdout, 1);
      close(hstdout);
	}
	if (hstdin >= 0) {
		dup2(hstdin, 0);
      close(hstdin);
	}
	if (fdout >= 0) {
		close(fdout);
	}
	if (fdin >= 0) {
		close(fdin);
	}
	return rc;
}

/******************************************************************************/
/*
 *	Check the CGI process.  Return 0 if it does not exist; non 0 if it does.
 */

int websCheckCgiProc(int handle, int *status)
{
/*
 *	Check to see if the CGI child process has terminated or not yet.  
 */
	if (waitpid(handle, status, WNOHANG) == handle) {
		return 0;
	} else {
		return 1;
	}
}

/******************************************************************************/

#ifdef B_STATS
static void memLeaks() 
{
	int		fd;

	if ((fd = gopen(T("leak.txt"), O_CREAT | O_TRUNC | O_WRONLY, 0666)) >= 0) {
		bstats(fd, printMemStats);
		close(fd);
	}
}

/******************************************************************************/
/*
 *	Print memory usage / leaks
 */

static void printMemStats(int handle, char_t *fmt, ...)
{
	va_list		args;
	char_t		buf[256];

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);
	write(handle, buf, strlen(buf));
}
#endif

/******************************************************************************/

/* added by YYhuang 07/04/02 */
int getGoAHeadServerPort(void)
{
    return port;
}

#ifdef CONFIG_DUAL_IMAGE
static int set_stable_flag(void)
{
	int set = 0;
	char *wordlist = nvram_get(UBOOT_NVRAM, "Image1Stable");

	if (wordlist) {
		if (strcmp(wordlist, "1") != 0)
			set = 1;
	}
	else
		set = 1;

	if (set) {
		printf("Set Image1 stable flag\n");
		nvram_set(UBOOT_NVRAM, "Image1Stable", "1");
	}
	
	return 0;

}
#endif
