#include <stdio.h>

#include "phruncall.h"
#include "phupdate.h"
#include "log.h"
#include <signal.h>     /* for singal handle */
#ifndef WIN32
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <netdb.h>
#include <unistd.h>     /* for close() */

#endif
#if (DDNS_ADVANCE == FYES)
static void create_pidfile(char *interface)
{
    FILE *pidfile;
    char pidfilename[128];
    sprintf(pidfilename, "/var/run/ddns_%s.pid", interface);
	
    if ((pidfile = fopen(pidfilename, "w")) != NULL) {
		fprintf(pidfile, "%d\n", getpid());
		(void) fclose(pidfile);
    } else {
		printf("Failed to create pid file %s: %m", pidfilename);
		pidfilename[0] = 0;
    }
}
#endif
PHGlobal global;
PH_parameter parameter;
static void my_handleSIG (int sig)
{
	if (sig == SIGINT)
	{
		printf ("signal = SIGINT\n");
		phddns_stop(&global);
		exit(0);
	}
	if (sig == SIGTERM)
	{
		printf ("signal = SIGTERM\n");
		phddns_stop(&global);
	}
	signal (sig, my_handleSIG);
}

//状态更新回调
static void myOnStatusChanged(int status, long data)
{
	printf("myOnStatusChanged %s", convert_status_code(status));
	if (status == okKeepAliveRecved)
	{
		printf(", IP: %d", data);
	}
	if (status == okDomainsRegistered)
	{
		printf(", UserType: %d", data);
	}
	printf("\n");
}

//域名注册回调
static void myOnDomainRegistered(char *domain)
{
	printf("myOnDomainRegistered %s\n", domain);
}

//用户信息XML数据回调
static void myOnUserInfo(char *userInfo, int len)
{
	printf("myOnUserInfo %s\n", userInfo);
}

//域名信息XML数据回调
static void myOnAccountDomainInfo(char *domainInfo, int len)
{
	printf("myOnAccountDomainInfo %s\n", domainInfo);
}

int main(int argc, char *argv[])
{
#if (DDNS_ADVANCE == FYES)
        char interface[5];
#endif
	void (*ohandler) (int);
#ifdef WIN32
	WORD VersionRequested;		// passed to WSAStartup
	WSADATA  WsaData;			// receives data from WSAStartup
	int error;

	VersionRequested = MAKEWORD(2, 0);

	//start Winsock 2
	error = WSAStartup(VersionRequested, &WsaData);
#endif

	ohandler = signal (SIGINT, my_handleSIG);
	if (ohandler != SIG_DFL) {
		printf ("previous signal handler for SIGINT is not a default handler\n");
		signal (SIGINT, ohandler);
	}

	init_global(&global);
	init_parameter(&parameter);
#if (DDNS_ADVANCE == FYES)
	if( checkparameter(argc,argv,&global,&parameter,interface) != 0 ) {
#else
	    if( checkparameter(argc,argv,&global,&parameter) != 0 ) {
#endif
//		printf ("Parameter has an unexpected error,please read the help above.Thank you for your patient!\n");
		return 0;
	}

	if( parameter.bDaemon == 1 ) {
#ifndef WIN32
		printf("phddns started as daemon!\n");
		if(0 != daemon(0,0))
		{
			printf("daemon(0,0) failed\n");
			return -1;
		}
#else
		printf("phddns started daemon failed\n");
#endif
	}

	log_open( parameter.logfile, 1);	//empty file will cause we printf to stdout

#if (DDNS_ADVANCE == FYES)
	create_pidfile(interface);
#endif
	global.cbOnStatusChanged = myOnStatusChanged;
	global.cbOnDomainRegistered = myOnDomainRegistered;
	global.cbOnUserInfo = myOnUserInfo;
	global.cbOnAccountDomainInfo = myOnAccountDomainInfo;

	set_default_callback(&global);
	//以下key为花生壳官方开源客户端专用，严禁用于其他用途，后果自负！
	global.clientinfo = 0x186618AE; 
	global.challengekey = 0xFFED7940;
	
	for (;;)
	{
#if (DDNS_ADVANCE == FYES)
             	int next = phddns_step(&global,interface);
		refreshIpTime2File(&global, 0,interface);
#else
		int next = phddns_step(&global);
		refreshIpTime2File(&global, 0);
#endif
		sleep(next);
	}
	phddns_stop(&global);
	return 0;
}
