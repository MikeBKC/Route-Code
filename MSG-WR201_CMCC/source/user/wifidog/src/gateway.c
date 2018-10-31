/********************************************************************\
 * This program is free software; you can redistribute it and/or    *
 * modify it under the terms of the GNU General Public License as   *
 * published by the Free:Software Foundation; either version 2 of   *
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

/* $Id: gateway.c,v 1.16.4.7 2016/08/02 02:33:33 cao.yongxiang Exp $ */
/** @internal
  @file gateway.c
  @brief Main loop
  @author Copyright (C) 2004 Philippe April <papril777@yahoo.com>
  @author Copyright (C) 2004 Alexandre Carmel-Veilleux <acv@miniguru.ca>
 */

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
/* for strerror() */
#include <string.h>

/* for wait() */
#include <sys/wait.h>

/* for unix socket communication*/
#include <sys/socket.h>
#include <sys/un.h>

#include "free_hash.h"
#include "common.h"
#include "httpd.h"
#include "safe.h"
#include "debug.h"
#include "conf.h"
#include "gateway.h"
#include "firewall.h"
#include "commandline.h"
#include "auth.h"
#include "http.h"
#include "client_list.h"
#include "wdctl_thread.h"
#include "ping_thread.h"
#include "httpd_thread.h"
#include "util.h"
#include "signature.h"
#ifdef CONN_QUEUE
#include "queue.h"
#endif
#ifdef CONN_MULTI_QUEUE
#include "queue_multi.h"
#endif
#include "uttMachine.h"
#if (WIFIDOG_AUTH == FYES)
#include "typedef.h"
#include "mib.h"
#include "profacce.h"
#include "comExeFun.h"
#endif

/** XXX Ugly hack 
 * We need to remember the thread IDs of threads that simulate wait with pthread_cond_timedwait
 * so we can explicitly kill them in the termination handler
 */
#if (WIFIDOG_MAKE_MONEY == FYES)
extern struct hash_list_node *Hash_List[HASH_KEY];
#endif
static pthread_t tid_fw_counter = 0;
#if (MAKE_MONEY_HEARTBEAT != FYES)
pthread_t tid_ping = 0; 
#else
static pthread_t tid_ping = 0; 
#endif
#ifdef CONN_QUEUE
static pthread_t tid_httpd = 0;
#endif
#if (WEBAUTH_WECHAT_ATTENTION == FYES || (WIFIDOG_MAC_AUTH == FYES))
static pthread_t tid_listen_kernel = 0;
#endif

/* The internal web server */
httpd * webserver = NULL;

/* from commandline.c */
extern char ** restartargv;
extern pid_t restart_orig_pid;
t_client *firstclient;

/* from client_list.c */
extern pthread_mutex_t client_list_mutex;

/* Time when wifidog started  */
time_t started_time = 0;

int authEn_ping = 1;

char maclist[MAX_MACLIST_LEN+1];
#if (SWITCH_AUTH_MODE == FYES)
char lanMac[18];
#endif
/* Appends -x, the current PID, and NULL to restartargv
 * see parse_commandline in commandline.c for details
 *
 * Why is restartargv global? Shouldn't it be at most static to commandline.c
 * and this function static there? -Alex @ 8oct2006
 */
void append_x_restartargv(void) {
	int i;

	for (i=0; restartargv[i]; i++);

	restartargv[i++] = safe_strdup("-x");
	safe_asprintf(&(restartargv[i++]), "%d", getpid());
}

/* @internal
 * @brief During gateway restart, connects to the parent process via the internal socket
 * Downloads from it the active client list
 */
void get_clients_from_parent(void) {
	int sock;
	struct sockaddr_un	sa_un;
	s_config * config = NULL;
	char linebuffer[MAX_BUF];
	int len = 0;
	char *running1 = NULL;
	char *running2 = NULL;
	char *token1 = NULL;
	char *token2 = NULL;
	char onechar;
	char *command = NULL;
	char *key = NULL;
	char *value = NULL;
	t_client * client = NULL;
	t_client * lastclient = NULL;

	config = config_get_config();
	
	debug(LOG_INFO, "Connecting to parent to download clients");

	/* Connect to socket */
	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	memset(&sa_un, 0, sizeof(sa_un));
	sa_un.sun_family = AF_UNIX;
	strncpy(sa_un.sun_path, config->internal_sock, (sizeof(sa_un.sun_path) - 1));

	if (connect(sock, (struct sockaddr *)&sa_un, strlen(sa_un.sun_path) + sizeof(sa_un.sun_family))) {
		debug(LOG_ERR, "Failed to connect to parent (%s) - client list not downloaded", strerror(errno));
		return;
	}

	debug(LOG_INFO, "Connected to parent.  Downloading clients");

	LOCK_CLIENT_LIST();

	command = NULL;
	memset(linebuffer, 0, sizeof(linebuffer));
	len = 0;
	client = NULL;
	/* Get line by line */
	while (read(sock, &onechar, 1) == 1) {
		if (onechar == '\n') {
			/* End of line */
			onechar = '\0';
		}
		linebuffer[len++] = onechar;

#if 0
		if (!onechar) {
			/* We have a complete entry in linebuffer - parse it */
			debug(LOG_DEBUG, "Received from parent: [%s]", linebuffer);
			running1 = linebuffer;
			while ((token1 = strsep(&running1, "|")) != NULL) {
				if (!command) {
					/* The first token is the command */
					command = token1;
				}
				else {
				/* Token1 has something like "foo=bar" */
					running2 = token1;
					key = value = NULL;
					while ((token2 = strsep(&running2, "=")) != NULL) {
						if (!key) {
							key = token2;
						}
						else if (!value) {
							value = token2;
						}
					}
				}

				if (strcmp(command, "CLIENT") == 0) {
					/* This line has info about a client in the client list */
					if (!client) {
						/* Create a new client struct */
						client = safe_malloc(sizeof(t_client));
						memset(client, 0, sizeof(t_client));
					}
				}

				if (key && value) {
					if (strcmp(command, "CLIENT") == 0) {
						/* Assign the key into the appropriate slot in the connection structure */
						if (strcmp(key, "ip") == 0) {
							client->ip = safe_strdup(value);
						}
						else if (strcmp(key, "mac") == 0) {
							client->mac = safe_strdup(value);
						}
						else if (strcmp(key, "token") == 0) {
							client->token = safe_strdup(value);
						}
						else if (strcmp(key, "fw_connection_state") == 0) {
							client->fw_connection_state = atoi(value);
						}
						else if (strcmp(key, "fd") == 0) {
							client->fd = atoi(value);
						}
						else if (strcmp(key, "counters_incoming") == 0) {
							client->counters.incoming_history = atoll(value);
							client->counters.incoming = client->counters.incoming_history;
						}
						else if (strcmp(key, "counters_outgoing") == 0) {
							client->counters.outgoing_history = atoll(value);
							client->counters.outgoing = client->counters.outgoing_history;
						}
						else if (strcmp(key, "counters_last_updated") == 0) {
							client->counters.last_updated = atol(value);
						}
						else {
							debug(LOG_NOTICE, "I don't know how to inherit key [%s] value [%s] from parent", key, value);
						}
					}
				}
			}

			/* End of parsing this command */
			if (client) {
				/* Add this client to the client list */
				if (!firstclient) {
					firstclient = client;
					lastclient = firstclient;
				}
				else {
					lastclient->next = client;
					lastclient = client;
				}
			}

			/* Clean up */
			command = NULL;
			memset(linebuffer, 0, sizeof(linebuffer));
			len = 0;
			client = NULL;
		}
#endif
	}

	UNLOCK_CLIENT_LIST();
	debug(LOG_INFO, "Client list downloaded successfully from parent");

	close(sock);
}

/**@internal
 * @brief Handles SIGCHLD signals to avoid zombie processes
 *
 * When a child process exits, it causes a SIGCHLD to be sent to the
 * process. This handler catches it and reaps the child process so it
 * can exit. Otherwise we'd get zombie processes.
 */
void
sigchld_handler(int s)
{
	int	status;
	pid_t rc;
	
	debug(LOG_DEBUG, "Handler for SIGCHLD called. Trying to reap a child");

	rc = waitpid(-1, &status, WNOHANG);

	debug(LOG_DEBUG, "Handler for SIGCHLD reaped child PID %d", rc);
}

/** Exits cleanly after cleaning up the firewall.  
 *  Use this function anytime you need to exit after firewall initialization */
void
termination_handler(int s)
{
	static	pthread_mutex_t	sigterm_mutex = PTHREAD_MUTEX_INITIALIZER;

	debug(LOG_INFO, "Handler for termination caught signal %d", s);

	/* Makes sure we only call fw_destroy() once. */
	if (pthread_mutex_trylock(&sigterm_mutex)) {
		debug(LOG_INFO, "Another thread already began global termination handler. I'm exiting");
		pthread_exit(NULL);
	}
	else {
		debug(LOG_INFO, "Cleaning up and exiting");
	}

	debug(LOG_INFO, "Flushing firewall rules...");
	fw_destroy();

	/* XXX Hack
	 * Aparently pthread_cond_timedwait under openwrt prevents signals (and therefore
	 * termination handler) from happening so we need to explicitly kill the threads 
	 * that use that
	 */
	if (tid_fw_counter) {
		debug(LOG_INFO, "Explicitly killing the fw_counter thread");
		pthread_kill(tid_fw_counter, SIGKILL);
	}
	if (tid_ping) {
		debug(LOG_INFO, "Explicitly killing the ping thread");
		pthread_kill(tid_ping, SIGKILL);
	}
#ifdef CONN_QUEUE
	if (tid_httpd) {
		debug(LOG_INFO, "Explicitly killing the httpd thread");
		pthread_kill(tid_httpd, SIGKILL);
	}
#endif
#ifdef CONN_MULTI_QUEUE
	{
	    int i=0;
	    for(i=0;i<QUEUE_COUNT;i++) {
		if(thread_pid[i])
		debug(LOG_INFO, "Explicitly killing the httpd thread index [%d]",i);
		pthread_kill(thread_pid[i], SIGKILL);
	    }
	}
#endif
	ProfDetach();

	debug(LOG_NOTICE, "Exiting...");
	exit(s == 0 ? 1 : 0);
}

/** @internal 
 * Registers all the signal handlers
 */
static void
init_signals(void)
{
	struct sigaction sa;

	debug(LOG_DEBUG, "Initializing signal handlers");
	
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		debug(LOG_ERR, "sigaction(): %s", strerror(errno));
		exit(1);
	}

	/* Trap SIGPIPE */
	/* This is done so that when libhttpd does a socket operation on
	 * a disconnected socket (i.e.: Broken Pipes) we catch the signal
	 * and do nothing. The alternative is to exit. SIGPIPE are harmless
	 * if not desirable.
	 */
	sa.sa_handler = SIG_IGN;
	if (sigaction(SIGPIPE, &sa, NULL) == -1) {
		debug(LOG_ERR, "sigaction(): %s", strerror(errno));
		exit(1);
	}

	sa.sa_handler = termination_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	/* Trap SIGTERM */
	if (sigaction(SIGTERM, &sa, NULL) == -1) {
		debug(LOG_ERR, "sigaction(): %s", strerror(errno));
		exit(1);
	}

	/* Trap SIGQUIT */
	if (sigaction(SIGQUIT, &sa, NULL) == -1) {
		debug(LOG_ERR, "sigaction(): %s", strerror(errno));
		exit(1);
	}

	/* Trap SIGINT */
	if (sigaction(SIGINT, &sa, NULL) == -1) {
		debug(LOG_ERR, "sigaction(): %s", strerror(errno));
		exit(1);
	}
}

/**@internal
 * Main execution loop 
 */
static void
main_loop(void)
{
	int result;
	pthread_t	tid;
	s_config *config = config_get_config();
	request *r;
	void **params=NULL;
#ifdef CONN_MULTI_QUEUE
	int enter_index=0;
#endif

    /* Set the time when wifidog started */
	if (!started_time) {
		debug(LOG_INFO, "Setting started_time");
		started_time = time(NULL);
	}
	else if (started_time < MINIMUM_STARTED_TIME) {
		debug(LOG_WARNING, "Detected possible clock skew - re-setting started_time");
		started_time = time(NULL);
	}

	/* If we don't have the Gateway IP address, get it. Can't fail. */
	if (!config->gw_address) {
		debug(LOG_DEBUG, "Finding IP address of %s", config->gw_interface);
		if ((config->gw_address = get_iface_ip(config->gw_interface)) == NULL) {
			debug(LOG_ERR, "Could not get IP address information of %s, exiting...", config->gw_interface);
			exit(1);
		}
		debug(LOG_DEBUG, "%s = %s", config->gw_interface, config->gw_address);
	}

	/* If we don't have the Gateway ID, construct it from the internal MAC address.
	 * "Can't fail" so exit() if the impossible happens. */
	if (!config->gw_id) {
    	debug(LOG_DEBUG, "Finding MAC address of %s", config->gw_interface);
    	if ((config->gw_id = get_iface_mac(config->gw_interface)) == NULL) {
			debug(LOG_ERR, "Could not get MAC address information of %s, exiting...", config->gw_interface);
			exit(1);
		}
#if (SWITCH_AUTH_MODE == FYES)
		memset(lanMac, 0,sizeof(lanMac));
		strncpy(lanMac, config->gw_id, 17);
		if(strlen(lanMac) != 17) {
		    debug(LOG_ERR, "lan interface %s, mac %s, lan not equal 17\n", config->gw_interface, lanMac);
		}
#endif
		debug(LOG_DEBUG, "%s = %s", config->gw_interface, config->gw_id);
	}

	/* Initializes the web server */
	debug(LOG_NOTICE, "Creating web server on %s:%d", config->gw_address, config->gw_port);
	if ((webserver = httpdCreate(config->gw_address, config->gw_port)) == NULL) {
		debug(LOG_ERR, "Could not create web server: %s", strerror(errno));
		exit(1);
	}

	debug(LOG_DEBUG, "Assigning callbacks to web server");
#if (WIFIDOG_STDIF == FYES)
	httpdAddCContent(webserver, "/", "wifidog", 0, NULL, http_callback_wifidog);
	httpdAddCContent(webserver, "/wifidog", "", 0, NULL, http_callback_wifidog);
	httpdAddCContent(webserver, "/wifidog", "about", 0, NULL, http_callback_about);
	httpdAddCContent(webserver, "/wifidog", "status", 0, NULL, http_callback_status);
	httpdAddCContent(webserver, "/wifidog", "auth", 0, NULL, http_callback_auth);
#else
	httpdAddCContent(webserver, "/", "wifidog", 0, NULL, http_callback_wifidog);
	httpdAddCContent(webserver, "/", "about", 0, NULL, http_callback_about);
	httpdAddCContent(webserver, "/", "status", 0, NULL, http_callback_status);
	httpdAddCContent(webserver, "/", "operation", 0, NULL, http_callback_operation);
	httpdAddCContent(webserver, "/", "auth", 0, NULL, http_callback_auth);
	httpdAddCContent(webserver, "/", "operation", 0, NULL, http_callback_operation);
#if (SMS_ENABLE == 1)
	httpdAddCContent(webserver, "/json/", "phoneRegin", 0, NULL, http_callback_phoneRegin);
	httpdAddCContent(webserver, "/json/", "forgotPasswd", 0, NULL, http_callback_forgotPasswd);
#endif
#if 0
	httpdAddCContent(webserver, "/", "clientList", 0, NULL, http_callback_client);
	httpdAddCContent(webserver, "/", "submit_localAuth", 0, NULL, http_callback_localAuth);

	httpdSetFileBase(webserver, "/etc_ro/web");
	httpdAddFileContent(webserver, "/", "login.htm", HTTP_TRUE, NULL, "login.htm");
#endif
#endif

#if (WIFIDOG_MAKE_MONEY == FYES)
    httpdAddCContent(webserver, "/", "salewifi", 0, NULL, http_salewifi_experience);
    httpdAddCContent(webserver, "/", "getrest", 0, NULL, http_salewifi_getrest);

    httpdAddCContent(webserver, "/", "getParam", 0, NULL, http_salewifi_getParam);
#if 0   
    httpdAddCContent(webserver, "/", "savePhone", 0, NULL, http_salewifi_savePhone);
#endif
#elif (UTT_KNOWIFI == FYES)
    httpdAddCContent(webserver, "/", "homeRouter", 0, NULL, http_home_router_302);
#endif
#if !defined(WIFI_PRE_REDIRECT)
	httpdAddC404Content(webserver, http_callback_404);
#endif

	/* Reset the firewall (if WiFiDog crashed) */
	fw_destroy();
	/* Then initialize it */
	if (!fw_init()) {
		debug(LOG_ERR, "FATAL: Failed to initialize firewall");
		exit(1);
	}

	/* Start clean up thread */
	result = pthread_create(&tid_fw_counter, NULL, (void *)thread_client_timeout_check, NULL);
	if (result != 0) {
	    debug(LOG_ERR, "FATAL: Failed to create a new thread (fw_counter) - exiting");
	    termination_handler(0);
	}
	pthread_detach(tid_fw_counter);

	/* Start control thread */
	result = pthread_create(&tid, NULL, (void *)thread_wdctl, (void *)safe_strdup(config->wdctl_sock));
	if (result != 0) {
		debug(LOG_ERR, "FATAL: Failed to create a new thread (wdctl) - exiting");
		termination_handler(0);
	}
	pthread_detach(tid);
	
	/* Start heartbeat thread */

	result = pthread_create(&tid_ping, NULL, (void *)thread_ping, NULL);
	if (result != 0) {
	    debug(LOG_ERR, "FATAL: Failed to create a new thread (ping) - exiting");
		termination_handler(0);
	}
	pthread_detach(tid_ping);
#if (WEBAUTH_WECHAT_ATTENTION == FYES || (WIFIDOG_MAC_AUTH == FYES))
	/* handle msg from kernel, include wechat attention msg and arp add msg */
	void thread_listen_kernel(void *arg);
	result = pthread_create(&tid_listen_kernel, NULL, (void *)thread_listen_kernel, NULL);
	if (result != 0) {
	    debug(LOG_ERR, "FATAL: Failed to create a new thread (listen_kernel) - exiting");
		termination_handler(0);
	}
	pthread_detach(tid_listen_kernel);
#endif
#ifdef CONN_MULTI_QUEUE
	int i=0,j=0;
	int queue_index[QUEUE_COUNT];
	queue_init(QUEUE_COUNT);
	params = safe_malloc((2*QUEUE_COUNT) * sizeof(void *));
	if(params != NULL) {
	    for(i=0;(i<QUEUE_COUNT&&j<2*QUEUE_COUNT);i++) {
		*(params + j) = webserver;
		queue_index[i]=i;
		*(params + j + 1) = &(queue_index[i]);
		debug(LOG_DEBUG, "index [%d] ,param %d",queue_index[i],*((int *)(*(params+j+1))));

		result = pthread_create(&thread_pid[i], NULL, (void *)thread_httpd, (void *)(params+j));
		if (result != 0) {
		    debug(LOG_ERR, "FATAL: Failed to create a new thread index [%d] (httpd) - exiting",i);
		    termination_handler(0);
		}
		pthread_detach(thread_pid[i]);
		j+=2;
	    }
	}
#endif /*CONN_MULTI_QUEUE*/
#ifdef CONN_QUEUE
	queue_init();
	/*
	 * We got a connection
	 *
	 * We should create another thread
	 */
	/* The void**'s are a simulation of the normal C
	 * function calling sequence. */
	params = safe_malloc(sizeof(void *));
	*params = webserver;

	result = pthread_create(&tid_httpd, NULL, (void *)thread_httpd, (void *)params);
	if (result != 0) {
		debug(LOG_ERR, "FATAL: Failed to create a new thread (httpd) - exiting");
		termination_handler(0);
	}
	pthread_detach(tid_httpd);
#endif /*end CONN_QUEUE*/
	
	debug(LOG_NOTICE, "Waiting for connections");
	while(1) {
		r = httpdGetConnection(webserver, NULL);

		/* We can't convert this to a switch because there might be
		 * values that are not -1, 0 or 1. */
		if (webserver->lastError == -1) {
			/* Interrupted system call */
			continue; /* restart loop */
		}
		else if (webserver->lastError < -1) {
			/*
			 * FIXME
			 * An error occurred - should we abort?
			 * reboot the device ?
			 */
			debug(LOG_ERR, "FATAL: httpdGetConnection returned unexpected value %d, exiting.", webserver->lastError);
			//termination_handler(0);
		}
		else if (r != NULL) {
#ifdef CONN_MULTI_QUEUE
#if 1
		if (httpdReadRequest(webserver, r) == 0) {
#endif
		    //debug(LOG_DEBUG, "Received connection from %s, spawning worker thread.", r->clientAddr);
		    if((enter_index<0) || (enter_index>=QUEUE_COUNT)) {
			enter_index=0;
		    }
		    queue_enter(r,enter_index);
		    enter_index++;
#if 1
		}
		else {
		    //debug(LOG_DEBUG, "No valid request received from %s.", r->clientAddr);
		    httpdEndRequest(r);
		}
#endif
#else /*CONN_MULTI_QUEUE*/
#ifdef CONN_QUEUE
#if 1
		if (httpdReadRequest(webserver, r) == 0) {
#endif
		    debug(LOG_DEBUG, "Received connection from %s, spawning worker thread.", r->clientAddr);
		    queue_enter(r);
#if 1
		}
		else {
		    debug(LOG_DEBUG, "No valid request received from %s.", r->clientAddr);
		    httpdEndRequest(r);
		}
#endif
#else /*end CONN_QUEUE*/
			/*
			 * We got a connection
			 *
			 * We should create another thread
			 */
			debug(LOG_INFO, "Received connection from %s, spawning worker thread", r->clientAddr);
			/* The void**'s are a simulation of the normal C
			 * function calling sequence. */
			params = safe_malloc(2 * sizeof(void *));
			*params = webserver;
			*(params + 1) = r;

			result = pthread_create(&tid, NULL, (void *)thread_httpd, (void *)params);
			if (result != 0) {
				debug(LOG_ERR, "FATAL: Failed to create a new thread (httpd) - exiting");
				termination_handler(0);
			}
			pthread_detach(tid);
#endif /*end CONN_QUEUE*/
#endif /*end CONN_MULTI_QUEUE*/
		}
		else {
			/* webserver->lastError should be 2 */
			/* XXX We failed an ACL.... No handling because
			 * we don't set any... */
		}
	}

	/* never reached */
}

static void 
checkAuthHost(s_config *config)
{
#if (WIFIDOG_MAKE_MONEY == FYES)
    if(config->auth_servers->authserv_hostname)
    {
	free(config->auth_servers->authserv_hostname);
	config->auth_servers->authserv_hostname = safe_strdup(MAKE_MONEY_DOMAIN);
    }
#else
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;
    
    //ProfInit();
    profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
    /*web认证不开启不会用到计费模式返回0*/
    if((profG != NULL) && (ProfInstIsFree(profG) == 0))
    {
	if(strlen(profG->testDomain) > 0) 
	{
	    if(config->auth_servers->authserv_hostname)
	    {
		free(config->auth_servers->authserv_hostname);
		config->auth_servers->authserv_hostname = safe_strdup(profG->testDomain);
	    }
	}
	if(strlen(profG->login_url) > 0) 
	{
	    if(config->auth_servers->authserv_login_script_path_fragment)
	    {
		free(config->auth_servers->authserv_login_script_path_fragment);
		config->auth_servers->authserv_login_script_path_fragment= safe_strdup(profG->login_url);
	    }
	}
	if(strlen(profG->auth_url) > 0) 
	{
	    if(config->auth_servers->authserv_auth_script_path_fragment)
	    {
		free(config->auth_servers->authserv_auth_script_path_fragment);
		config->auth_servers->authserv_auth_script_path_fragment = safe_strdup(profG->auth_url);
	    }
	}
	if(strlen(profG->success_url) > 0) 
	{
	    if(config->auth_servers->authserv_portal_script_path_fragment)
	    {
		free(config->auth_servers->authserv_portal_script_path_fragment);
		config->auth_servers->authserv_portal_script_path_fragment = safe_strdup(profG->success_url);
	    }
	}
	if(strlen(profG->ping_url) > 0) 
	{
	    if(config->auth_servers->authserv_ping_script_path_fragment)
	    {
		free(config->auth_servers->authserv_ping_script_path_fragment);
		config->auth_servers->authserv_ping_script_path_fragment = safe_strdup(profG->ping_url);
	    }
	}
#if (WIFIDOG_USER_SUCCESS_URL == FYES)
	if(strlen(profG->wifiData.successUrl) > 0) 
	{
	    urlCheck(profG->wifiData.successUrl,sizeof(profG->wifiData.successUrl));
	    if(config->successUrl)
	    {
		free(config->successUrl);
	    }
	    config->successUrl = safe_strdup(profG->wifiData.successUrl);
	}
#endif
#if (WIFIDOG_USER_SECRET_KEY == FYES)
	if(strlen(profG->wifiData.secret_key) > 0) 
	{
	     if(config->secret_key)
	    {
		free(config->secret_key);
	    }
	    config->secret_key = safe_strdup(profG->wifiData.secret_key);

	}
#endif
#if (WIFIDOG_MAC_AUTH == FYES)
	    config->macAuthEn = profG->wifiData.macAuthEn;
#endif
    }
#endif

    //ProfDetach();
    return;
}
/** Reads the configuration file and then starts the main loop */
int main(int argc, char **argv) {

#if 1
	/*segment fault debug*/
	struct rlimit coredump;
	memset(&coredump, 0, sizeof(struct rlimit));
	coredump.rlim_cur = RLIM_INFINITY;
	coredump.rlim_max = RLIM_INFINITY;
	setrlimit(RLIMIT_CORE, &coredump);
#endif
	ProfInit();
	s_config *config = config_get_config();
	config_init();

	parse_commandline(argc, argv);
	
#if (WIFIDOG_MAKE_MONEY == FYES)
//	init_hash();
	initFreeHash();	
#endif
	/* Initialize the config */
	config_read(config->configfile);
	config_validate();
	debug(LOG_DEBUG, "before check, config auth server host name: %s",config->auth_servers->authserv_hostname);
	/*检测是否配置的测试域名，是则切换到测试域名*/
	checkAuthHost(config);
	config_validate();
	if(config->auth_servers->authserv_hostname == NULL) {
	    debug(LOG_ERR,"auth server hostname is null.\n");
	    return;
	}
	debug(LOG_DEBUG, "after check, config auth server host name: %s",config->auth_servers->authserv_hostname);

	/* Initializes the linked list of connected clients */
	client_list_init();

	/* Init the signals to catch chld/quit/etc */
	init_signals();

	if (restart_orig_pid) {
		/*
		 * We were restarted and our parent is waiting for us to talk to it over the socket
		 */
		get_clients_from_parent();

		/*
		 * At this point the parent will start destroying itself and the firewall. Let it finish it's job before we continue
		 */
		while (kill(restart_orig_pid, 0) != -1) {
			debug(LOG_INFO, "Waiting for parent PID %d to die before continuing loading", restart_orig_pid);
			sleep(1);
		}

		debug(LOG_INFO, "Parent PID %d seems to be dead. Continuing loading.");
	}

	if (config->daemon) {

		debug(LOG_INFO, "Forking into background");

		switch(safe_fork()) {
			case 0: /* child */
				setsid();
				append_x_restartargv();
				main_loop();
				break;

			default: /* parent */
				exit(0);
				break;
		}
	}
	else {
		append_x_restartargv();
		main_loop();
	}

	return(0); /* never reached */
}
