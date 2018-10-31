/********************************************************************\
 * This program is free software; you can redistribute it and/or    *
 * modify it under the terms of the GNU General Public License as   *
 * published by the Free Software Foundation; either version 2 of   *
 * the License, or (at your option) any later version.              *
 *                                                                  *
 * This program is distributed in the hope that it will be useful,  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
 * GNU General Public License for more details.                     *
 *                                                                  *
 * You should have received a copy of the GNU General Public License*
 * along with this program; if not, contact:                        *
 *                                                                  *
 * Free Software Foundation           Voice:  +1-617-542-5942       *
 * 59 Temple Place - Suite 330        Fax:    +1-617-542-2652       *
 * Boston, MA  02111-1307,  USA       gnu@gnu.org                   *
 *                                                                  *
\********************************************************************/

/* $Id: wdctl.c,v 1.9.8.1 2015/06/23 15:57:35 hou.bo Exp $ */
/** @file wdctl.c
    @brief Monitoring and control of wifidog, client part
    @author Copyright (C) 2004 Alexandre Carmel-Veilleux <acv@acv.ca>
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <syslog.h>
#include <errno.h>

#include "wdctl.h"
#include "uttMachine.h"
#if (WIFIDOG_AUTH == FYES) || (WIFIDOG_MAKE_MONEY == FYES)
#include "typedef.h"
#include "mib.h"
#include "profacce.h"
#include "comExeFun.h"
#include "flash_api.h"
#endif

s_config config;

static void usage(void);
static void init_config(void);
static void parse_commandline(int, char **);
static int connect_to_server(char *);
static size_t send_request(int, char *);
static void wdctl_status(void);
static void wdctl_stop(void);
static void wdctl_reset(void);
static void wdctl_restart(void);
#if(WIFIDOG_MAKE_MONEY != FYES)
static int set_test_domain(char *domain);
static int set_login_url(char *domain);
static int set_auth_url(char *domain);
static int set_success_url(char *domain);
static int set_ping_url(char *domain);
#endif
#if (WIFIDOG_USER_SECRET_KEY == FYES)
static int set_secret_key(char *key);
#endif
#if(WIFIDOG_MAC_AUTH == FYES)
static int macAuthEnable(char * enable);
#endif
static void wifidog_debug();

/** @internal
 * @brief Print usage
 *
 * Prints usage, called when wdctl is run with -h or with an unknown option
 */
static void
usage(void)
{
    printf("Usage: wdctl [options] command [arguments]\n");
    printf("\n");
    printf("options:\n");
    printf("  -s <path>         Path to the socket\n");
    printf("  -t                set test domain name\n");
    printf("  -l                set login url\n");
    printf("  -a                set auth url\n");
#if (WIFIDOG_USER_SECRET_KEY == FYES)
    printf("  -k                set secret key\n");
#endif
    printf("  -S                set success url\n");
    printf("  -p                set ping url\n");
    printf("  -A                print complete information\n");
    printf("  -f                set flush 'domain name','login url','auth url','success url','ping url'\n");
    printf("  -h                print help information\n");
    printf("\n");
    printf("commands:\n");
    printf("  reset [mac|ip]    Reset the specified mac or ip connection\n");
    printf("  status            Obtain the status of wifidog\n");
    printf("  stop              Stop the running wifidog\n");
#if(WIFIDOG_MAC_AUTH == FYES)
    printf("  macAuth [up|down] enable or disenable macAuth\n");
#endif
    printf("  debug [on|off]	echo debug info to /tmp/wifidog_log\n");
    //printf("  restart           Re-start the running wifidog (without disconnecting active users!)\n");
    printf("\n");
}

/** @internal
 *
 * Init default values in config struct
 */
static void
init_config(void)
{

	config.socket = strdup(DEFAULT_SOCK);
	config.command = WDCTL_UNDEF;
}

/** @internal
 *
 * Uses getopt() to parse the command line and set configuration values
 */
void
parse_commandline(int argc, char **argv)
{
    extern int optind;
    int c;

#if (WIFIDOG_MAKE_MONEY != FYES)
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;
    profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
#endif

    while (-1 != (c = getopt(argc, argv, "s:t:l:a:S:p:k:hfA"))) {
        switch(c) {
            case 'h':
                usage();
                exit(1);
                break;

            case 's':
                if (optarg) {
		    free(config.socket);
		    config.socket = strdup(optarg);
                }
                break;

#if (WIFIDOG_MAKE_MONEY != FYES)
	    case 't':
		if(optarg) {
		    if(set_test_domain(optarg) == 1)
		   {
			fprintf(stderr, "set test domain name: %s\n", optarg);
		    }
		} 
		else {
		    if(set_test_domain(NULL) == 1)
		    {
			fprintf(stderr, "delete test domain name.\n");
		    }
		}
                exit(1);
                break;
	    case 'l':
		if(optarg) {
		    if(set_login_url(optarg) == 1)
		    {
			fprintf(stderr, "set login url: %s\n", optarg);
		    }
		} 
		else {
		    if(set_login_url(NULL) == 1)
		    {
			fprintf(stderr, "delete login url.\n");
		    }
		}
                exit(1);
                break;
	    case 'a':
		if(optarg) {
		    if(set_auth_url(optarg) == 1)
		    {
			fprintf(stderr, "set auth url: %s\n", optarg);
		    }
		} else {
		    if(set_auth_url(NULL) == 1)
		    {
			fprintf(stderr, "delete auth url.\n");
		    }
		}
                exit(1);
                break;
	    case 'S':
		if(optarg) {
		    if(set_success_url(optarg) == 1)
		    {
			fprintf(stderr, "set success url: %s\n", optarg);
		    }
		} else {
		    if(set_success_url(NULL) == 1)
		    {
			fprintf(stderr, "delete success url.\n");
		    }
		}
                exit(1);
                break;
	    case 'p':
		if(optarg) {
		    if(set_ping_url(optarg) == 1)
		    {
			fprintf(stderr, "set ping url: %s\n", optarg);
		    }
		} else {
		    if(set_ping_url(NULL) == 1)
		    {
			fprintf(stderr, "delete ping url.\n");
		    }
		}
                exit(1);
                break;
	    case 'f':
		if(set_test_domain(NULL) == 1)
		{
		    fprintf(stderr, "delete test domain name.\n");
	            
		}
		 
		if(set_login_url(NULL) == 1)
		{
	            fprintf(stderr, "delete login url.\n");
		}

		if(set_auth_url(NULL) == 1)
		{
		    fprintf(stderr, "delete auth url.\n");
		}

		if(set_success_url(NULL) == 1)
		{
		    fprintf(stderr, "delete success url.\n");
		}
	        if(set_ping_url(NULL) == 1)
		{
		    fprintf(stderr, "delete ping url.\n");
		}
#if (WIFIDOG_USER_SECRET_KEY == FYES)
		if(set_secret_key(NULL) == 1)
		{
		    fprintf(stderr, "delete secret key.\n");
		}
#endif
#if(WIFIDOG_MAC_AUTH == FYES)
		 macAuthEnable("up");
#endif
                exit(1);
                break;
	    case 'A':
               printf("Complete information :\n");
	       printf("the test domain name: %s\n",profG->testDomain);
	       printf("the login url:        %s\n",profG->login_url);
	       printf("the auth url:         %s\n",profG->auth_url);
	       printf("the success url:      %s\n",profG->success_url);
	       printf("the ping url:         %s\n",profG->ping_url);          
#if (WIFIDOG_USER_SECRET_KEY == FYES)
	       printf("the secret key:       %s\n",profG->wifiData.secret_key);           
#endif
#if(WIFIDOG_MAC_AUTH == FYES)
	       printf("the macAuth :         %s\n",profG->wifiData.macAuthEn == 1U ? "enable":"disenable");           
#endif
	       exit(1);
	       break;
	    
#if (WIFIDOG_USER_SECRET_KEY == FYES)
	    case 'k':
	       if(optarg) {
		   if(set_secret_key(optarg) == 1)
		   {
		       fprintf(stderr, "set secret key: %s\n", optarg);
		   }
	       } 
	       else {
		   if(set_secret_key(NULL) == 1)
		   {
		       fprintf(stderr, "delete secret key.\n");
		   }
	       }
                exit(1);
                break;
#endif
#endif /*WIFIDOG_MAKE_MONEY*/
	    
	    default:
                usage();
                exit(1);
                break;
        }
    }

    if ((argc - optind) <= 0) {
	    usage();
	    exit(1);
    }

    if (strcmp(*(argv + optind), "status") == 0) {
	    config.command = WDCTL_STATUS;
    } else if (strcmp(*(argv + optind), "stop") == 0) {
	    config.command = WDCTL_STOP;
    } else if (strcmp(*(argv + optind), "debug") == 0) {
	    config.command = WDCTL_DEBUG;
	   //wifidog_debug();
	if ((argc - (optind + 1)) <= 0) {
	    fprintf(stderr, "wdctl: Error: You must specify  up or down (up to enbable aand down to disable)\n");
	    usage();
	    exit(1);
	}
	config.param = strdup(*(argv + optind + 1));
    }else if (strcmp(*(argv + optind), "reset") == 0) {
	    config.command = WDCTL_KILL;
	    if ((argc - (optind + 1)) <= 0) {
		    fprintf(stderr, "wdctl: Error: You must specify an IP "
				    "or a Mac address to reset\n");
		    usage();
		    exit(1);
	    }
	    config.param = strdup(*(argv + optind + 1));
#if 0
    } else if (strcmp(*(argv + optind), "restart") == 0) {
	    config.command = WDCTL_RESTART;
    }
      else if (strcmp(*(argv + optind), "show") == 0) {
	    char clientAll[4] = "All";
	    config.command = WDCTL_SHOW;
	    if ((argc - (optind + 1)) <= 0) {
		config.param = strdup(clientAll);
		fprintf(stderr, "config.param %s\n",config.param);
	    } else {
		config.param = strdup(*(argv + optind + 1));
		fprintf(stderr, "config.param %s",config.param);
	    }
#endif
      }
#if(WIFIDOG_MAC_AUTH == FYES)
    else if (strcmp(*(argv + optind), "macAuth") == 0) {
	if ((argc - (optind + 1)) <= 0) {
	    fprintf(stderr, "wdctl: Error: You must specify  up or down (up to enbable aand down to disable)\n");
	    usage();
	    exit(1);
	}
	macAuthEnable(*(argv + optind + 1));
	exit(1);
    }
#endif
else {
	    fprintf(stderr, "wdctl: Error: Invalid command \"%s\"\n", *(argv + optind));
	    usage();
	    exit(1);
    }
}
#if(WIFIDOG_MAKE_MONEY != FYES)
static int 
set_test_domain(char *domain)
{
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;
    int flag = 0;
    
	//ProfInit();

	profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
	/*web认证不开启不会用到计费模式返回0*/
	if((profG != NULL) && (ProfInstIsFree(profG) == 0))
	{
	    memset(profG->testDomain, 0, sizeof(profG->testDomain));
	    if((domain != NULL) && (strlen(domain) > 0)) {
		snprintf(profG->testDomain, 50, "%s", domain);
	    }
	    
		nvramWriteCommit(); /*save*/
	    flag = 1;
	}

	//ProfDetach();
    return flag;
}

static int 
set_login_url(char *domain)
{
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;
    int flag = 0;
    
	//ProfInit();

	profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
	/*web认证不开启不会用到计费模式返回0*/
	if((profG != NULL) && (ProfInstIsFree(profG) == 0))
	{
	    memset(profG->login_url, 0, sizeof(profG->login_url));
	    if((domain != NULL) && (strlen(domain) > 0)) {
		snprintf(profG->login_url, 50, "%s", domain);
	    }
	    
		nvramWriteCommit(); /*save*/
	    flag = 1;
	}

	//ProfDetach();
    return flag;
}
static int 
set_auth_url(char *domain)
{
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;
    int flag = 0;
    
	//ProfInit();

	profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
	/*web认证不开启不会用到计费模式返回0*/
	if((profG != NULL) && (ProfInstIsFree(profG) == 0))
	{
	    memset(profG->auth_url, 0, sizeof(profG->auth_url));
	    if((domain != NULL) && (strlen(domain) > 0)) {
		snprintf(profG->auth_url, 50, "%s", domain);
	    }
	    
		nvramWriteCommit(); /*save*/
	    flag = 1;
	}

	//ProfDetach();
    return flag;
}
static int 
set_success_url(char *domain)
{
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;
    int flag = 0;
    
	//ProfInit();

	profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
	/*web认证不开启不会用到计费模式返回0*/
	if((profG != NULL) && (ProfInstIsFree(profG) == 0))
	{
	    memset(profG->success_url, 0, sizeof(profG->success_url));
	    if((domain != NULL) && (strlen(domain) > 0)) {
		snprintf(profG->success_url, 50, "%s", domain);
	    }
	    
		nvramWriteCommit(); /*save*/
	    flag = 1;
	}

	//ProfDetach();
    return flag;
}
static int 
set_ping_url(char *domain)
{
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;
    int flag = 0;
    
	//ProfInit();

	profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
	/*web认证不开启不会用到计费模式返回0*/
	if((profG != NULL) && (ProfInstIsFree(profG) == 0))
	{
	    memset(profG->ping_url, 0, sizeof(profG->ping_url));
	    if((domain != NULL) && (strlen(domain) > 0)) {
		snprintf(profG->ping_url, 50, "%s", domain);
	    }
	    
		nvramWriteCommit(); /*save*/
	    flag = 1;
	}

	//ProfDetach();
    return flag;
}
#endif
static int
connect_to_server(char *sock_name)
{
	int sock;
	struct sockaddr_un	sa_un;
	
	/* Connect to socket */
	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	memset(&sa_un, 0, sizeof(sa_un));
	sa_un.sun_family = AF_UNIX;
	strncpy(sa_un.sun_path, sock_name, (sizeof(sa_un.sun_path) - 1));

	if (connect(sock, (struct sockaddr *)&sa_un, 
			strlen(sa_un.sun_path) + sizeof(sa_un.sun_family))) {
		fprintf(stderr, "wdctl: wifidog probably not started (Error: %s)\n", strerror(errno));
		exit(1);
	}

	return sock;
}

static size_t
send_request(int sock, char *request)
{
	size_t	len;
        ssize_t written;
		
	len = 0;
	while (len != strlen(request)) {
		written = write(sock, (request + len), strlen(request) - len);
		if (written == -1) {
			fprintf(stderr, "Write to wifidog failed: %s\n",
					strerror(errno));
			exit(1);
		}
		len += written;
	}

	return len;
}

static void
wdctl_status(void)
{
	int	sock;
	char	buffer[4096];
	char	request[16];
	int	len;

	sock = connect_to_server(config.socket);
		
	strncpy(request, "status\r\n\r\n", 15);

	len = send_request(sock, request);
	
	while ((len = read(sock, buffer, sizeof(buffer))) > 0) {
		buffer[len] = '\0';
		printf("%s", buffer);
	}

	shutdown(sock, 2);
	close(sock);


}
static void
wdctl_stop(void)
{
	int	sock;
	char	buffer[4096];
	char	request[16];
	int	len;

	sock = connect_to_server(config.socket);
		
	strncpy(request, "stop\r\n\r\n", 15);

	len = send_request(sock, request);
	
	while ((len = read(sock, buffer, sizeof(buffer))) > 0) {
		buffer[len] = '\0';
		printf("%s", buffer);
	}

	shutdown(sock, 2);
	close(sock);
}

void
wdctl_reset(void)
{
	int	sock;
	char	buffer[4096];
	char	request[64];
	size_t	len;
	int	rlen;

	sock = connect_to_server(config.socket);
		
	strncpy(request, "reset ", 64);
	strncat(request, config.param, (64 - strlen(request)));
	strncat(request, "\r\n\r\n", (64 - strlen(request)));

	len = send_request(sock, request);
	
	len = 0;
	memset(buffer, 0, sizeof(buffer));
	while ((len < sizeof(buffer)) && ((rlen = read(sock, (buffer + len),
				(sizeof(buffer) - len))) > 0)){
		len += rlen;
	}

	if (strcmp(buffer, "Yes") == 0) {
		printf("Connection %s successfully reset.\n", config.param);
	} else if (strcmp(buffer, "No") == 0) {
		printf("Connection %s was not active.\n", config.param);
	} else {
		fprintf(stderr, "wdctl: Error: WiFiDog sent an abnormal "
				"reply.\n");
	}

	shutdown(sock, 2);
	close(sock);
}

static void
wdctl_restart(void)
{
	int	sock;
	char	buffer[4096];
	char	request[16];
	int	len;

	sock = connect_to_server(config.socket);
		
	strncpy(request, "restart\r\n\r\n", 15);

	len = send_request(sock, request);
	
	while ((len = read(sock, buffer, sizeof(buffer))) > 0) {
		buffer[len] = '\0';
		printf("%s", buffer);
	}

	shutdown(sock, 2);
	close(sock);
}

#if 0
static void
wdctl_show(void)
{
    int sock;
    char buffer[4096];
    char request[64];
    int len;

    sock = connect_to_server(config.socket);

    strncpy(request, "show ", 64);
    strncat(request, config.param, (64-strlen(request)));
    strncat(request, "\r\n\r\n", (64-strlen(request)));

    len = send_request(sock, request);

    fprintf(stderr, "request %s, len =%u\n",request,len);
    while((len = read(sock, buffer, sizeof(buffer))) > 0) {
	buffer[len] = '\0';
	printf("%s\n",buffer);
    }

    shutdown(sock, 2);
    close(sock);
}
#endif

int
main(int argc, char **argv)
{

	ProfInit();
	/* Init configuration */
	init_config();
	parse_commandline(argc, argv);

	switch(config.command) {
	case WDCTL_STATUS:
		wdctl_status();
		break;
	
	case WDCTL_STOP:
		wdctl_stop();
		break;

	case WDCTL_KILL:
		wdctl_reset();
		break;
		
	case WDCTL_RESTART:
		wdctl_restart();
		break;

#if 0
	case WDCTL_SHOW:
		wdctl_show();
		break;
#endif
	case WDCTL_DEBUG:
		wifidog_debug();
		break;

	default:
		/* XXX NEVER REACHED */
		fprintf(stderr, "Oops\n");
		exit(1);
		break;
	}
#if 0 
	    //ProfInit();
	    fprintf(stderr,"%s-%d: wdctl printf.\n",__func__,__LINE__);
	    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
	    WebAuthGlobalProfile *profG = NULL;

	    profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
	    /*web认证不开启不会用到计费模式返回0*/
	    if((profG != NULL) && (ProfInstIsFree(profG) == 0))
	    {
		fprintf(stderr,"%s-%d: authType %u \n",__func__,__LINE__, profG->authType);
	    }
	    //ProfDetach();
#endif
	    ProfDetach();
	exit(0);
}
#if (WIFIDOG_USER_SECRET_KEY == FYES)
static int 
set_secret_key(char *key)
{
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;
    int flag = 0;
   char * p ;
    for(p = key ; p &&  *p !='\0';p++){
	char ch = *p;
	if( ('A' <= ch  && ch >= 'Z')
		|| ('a' <= ch && ch >= 'z')
		|| ('0' <= ch && ch >= '9')){
	    continue;
	}else{
	    printf("请不要包含特殊字符");	
	    exit(1);
	}
    }
    profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
    /*web认证不开启不会用到计费模式返回0*/
    if(ProfInstIsFree(profG) == 0)
    {
	memset(profG->wifiData.secret_key, 0, sizeof(profG->wifiData.secret_key));
	if((key != NULL) && (strlen(key) > 0)) {
	    snprintf(profG->wifiData.secret_key,50,"%s",key);
	}

	nvramWriteCommit(); /*save*/
	flag = 1;
    }

    return flag;
}
#endif
#if(WIFIDOG_MAC_AUTH == FYES)
static int macAuthEnable(char * enable)
{
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;
    profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
    if((ProfInstIsFree(profG) == 0)){
	if( enable != NULL && strcmp(enable,"up") == 0){
	    profG->wifiData.macAuthEn = 1U;
	}else{
	    profG->wifiData.macAuthEn = 0U;
	}
    }
	nvramWriteCommit(); /*save*/
}
#endif
static void 
wifidog_debug()
{
	int	sock;
	char	buffer[4096];
	char	request[64];
	size_t	len;
	int	rlen;

	sock = connect_to_server(config.socket);
	strncpy(request, "debug ", 64);
	strncat(request, config.param, (64 - strlen(request)));
	strncat(request, "\r\n\r\n", (64 - strlen(request)));

	len = send_request(sock, request);
	shutdown(sock, 2);
	close(sock);

}

