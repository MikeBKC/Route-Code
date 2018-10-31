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

/* $Id: ping_thread.c,v 1.16.8.12 2016/09/01 04:48:12 cao.yongxiang Exp $ */
/** @file ping_thread.c
    @brief Periodically checks in with the central auth server so the auth
    server knows the gateway is still up.  Note that this is NOT how the gateway
    detects that the central server is still up.
    @author Copyright (C) 2004 Alexandre Carmel-Veilleux <acv@miniguru.ca>
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <errno.h>

#include "../config.h"
#include "safe.h"
#include "common.h"
#include "conf.h"
#include "debug.h"
#include "ping_thread.h"
#include "util.h"
#include "centralserver.h"
#include "fw_iptables.h"
#include "client_list.h"
#include "cJSON.h"
#if (UTT_KNOWIFI == FYES)
#include "loop_http.h"
#include "des.h"
#endif

static void ping(void);

extern time_t started_time;
extern int authEn_ping;
extern char maclist[MAX_MACLIST_LEN+1];

extern pthread_mutex_t client_list_mutex;
int auth_off_flag = 0;
extern void client_delete_by_mac(char*);
#ifdef PARAM_SIGN
#include "signature.h"
extern pthread_mutex_t param_sign_mutex;
#endif

#if (UTT_KNOWIFI == FYES)
static int utt_knowifi_register(void);
extern int flag_updateDomainOk;
extern int flag_updateDomainFirst; 
#endif

/** Launches a thread that periodically checks in with the wifidog auth server to perform heartbeat function.
@param arg NULL
@todo This thread loops infinitely, need a watchdog to verify that it is still running?
*/  
void
thread_ping(void *arg)
{
	pthread_cond_t		cond = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t		cond_mutex = PTHREAD_MUTEX_INITIALIZER;
	struct	timespec	timeout;
	
	while (1) {
		/* Make sure we check the servers at the very begining */
		debug(LOG_DEBUG, "Running ping()");
#if (MAKE_MONEY_HEARTBEAT != FYES)
	        connect_auth_server();
#else
		ping();
#endif
#if (UTT_KNOWIFI == FYES)
		utt_knowifi_register();	
#if 1
		if (flag_updateDomainOk == 0 && flag_updateDomainFirst == 0)
		    utt_knowifi_updateDomain();
#endif
#endif
		
		/* Sleep for config.checkinterval seconds... */
		if(auth_off_flag == 1) {
		timeout.tv_sec = time(NULL) + 10;
		} else {
		timeout.tv_sec = time(NULL) + config_get_config()->checkinterval;
		}
		timeout.tv_nsec = 0;

		/* Mutex must be locked for pthread_cond_timedwait... */
		pthread_mutex_lock(&cond_mutex);
		
		/* Thread safe "sleep" */
		pthread_cond_timedwait(&cond, &cond_mutex, &timeout);

		/* No longer needs to be locked */
		pthread_mutex_unlock(&cond_mutex);
	}
}

#if (UTT_KNOWIFI == FYES)
unsigned int getSystemSn(void);
extern char lanMac[];
static int utt_knowifi_register(void)
{
    
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;
    profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);

    char sendMsg[255],recvMsg[255];
    char *cipherStr = NULL;
    int cipherLen = 0;
    unsigned int uttSn = getSystemSn();
    char snStr[11];
    char actCode[10] = {0};
    char *url_encode_actCode = NULL;
    int actCode_len;

    char code[32] = {0};
    char msg[128] = {0};
    char routeId[128] = {0};

    if((profG != NULL) && (ProfInstIsFree(profG) == 0))
    {
	if (profG->knowifi_enable == 1)
	{
	    if(strcmp(profG->knowifi_routeId, "unknown") && strcmp(profG->knowifi_routeId, ""))
	    {
		return 0;
	    }
	    
	    memset(snStr, 0, 11);
	    memset(sendMsg, 0, sizeof(sendMsg));
	    memset(recvMsg, 0, sizeof(recvMsg));
	    snprintf(snStr, 11, "%u",uttSn);
	    if((cipherStr = DES_Encrypt(snStr, snStr, &cipherLen)) != NULL)
	    {
		*(cipherStr+6) = '\0';
		strncpy(actCode, cipherStr, 7);
	    }
	    else
	    {
		sprintf(actCode, "%u", uttSn);
	    }
	    url_encode_actCode = php_url_encode(actCode, strlen(actCode), &actCode_len);
	    sprintf(sendMsg, "sn=%s&mac=%s&code=%s",snStr,lanMac , url_encode_actCode);
	    free(url_encode_actCode);

	    int res = loop_http_post(AUTH_OS_SERVER, "open/knowifi/v1.0/register", sendMsg, recvMsg);
	    if (res > 0)
	    {
		getValueFromString(recvMsg, "code", code, 32);
		if (strcmp(code, "0") == 0)
		{
		    getValueFromString(recvMsg, "routerid",routeId , 128);
		    printf("routeid get success\n");
		    
		    strncpy(profG->knowifi_routeId, routeId, 128);
		    nvramWriteCommit();
		}
	    }
	}
    }

    return 1;
}
#endif

#if (MAKE_MONEY_HEARTBEAT != FYES)
void 
parse_ping(char *str, int *state)
{
    /*
     *      {"mac":["00-e0-66-29-0b-ab","90-18-7c-96-f0-c6"],"state":"1","timestamp":"20140808110711"}
     *      345          *      {"mac":[],"state":"1","timestamp":"20140808110711"}
     *      346          *      */
    //char text[] = "{\"mac\":[\"00-e0-66-29-0b-ab\",\"90-18-7c-96-f0-c6\"],\"state\":\"1\",\"timestamp\":\"20140808110711\"}";
    char *out;
    cJSON *json;
    cJSON *item;
    cJSON *macP;
    t_client *cli = NULL;
    char *macPtr = NULL;
    if(str != NULL) {
	json=cJSON_Parse(str);
	if (!json) {
	    debug(LOG_ERR,"[JSON] Ping JSON Error, str= %s, before: [%s]\n",str,cJSON_GetErrorPtr());
	    *state = 0;
	}
	else
	{
	    debug(LOG_DEBUG, "[JSON] Ping json print:\n%s\n",((out=cJSON_Print(json))!=NULL)?out:"null");
	    //debug(LOG_DEBUG,"[JSON] Ping type =%d\n",json->type);

	    if((item = cJSON_GetObjectItem(json, "state"))!=NULL) {
	    //debug(LOG_DEBUG, "[JSON] Ping 'state' type =%d\n",item->type);
	    //debug(LOG_DEBUG,"[JSON] Ping 'state' string =%s\n",item->string);
	    debug(LOG_DEBUG,"[JSON] Ping 'state' valuestring =%s\n",item->valuestring);
	    /*state == "1" ,indicate that UTT Server is online*/
	    if(strcmp(item->valuestring, "1") == 0) {
		*state = 1;
	    } else {
		*state = 0;
	    }
	    if((item = cJSON_GetObjectItem(json, "mac"))!=NULL) {
		//debug(LOG_DEBUG,"[JSON] Ping 'mac' type =%d\n",item->type);
		if(item->child != NULL) {
		    macP = item->child;
		    while(macP!=NULL) {
			//debug(LOG_DEBUG,"[JSON] Ping next 'mac' type =%d\n",macP->type);
			if(macP->valuestring != NULL) { 
			    /*need cown mac */
			    debug(LOG_DEBUG, "[JSON] Ping next 'mac' valuestring =%s\n",macP->valuestring);
	    
			    macPtr = convertMac(macP->valuestring, 1);
			    if(macPtr != NULL) {
				cli = client_list_find_by_mac(macPtr);
				if(cli) {
				    debug(LOG_DEBUG, "[JSON] Ping get client: ip %s, mac %s, expire %d, runtime %d",
					cli->ip,cli->mac,cli->expire,cli->runtime);
				    fw_deny(cli->ip, cli->mac, cli->fw_connection_state);
				    cli->need_notify = 1; /*先不删除，标志为待通知*/
				}
				free(macPtr);
				macPtr = NULL;
			    }
			}
			/* get the next need down mac */
			macP = macP->next;
		    }
		} else {
		    debug(LOG_DEBUG,"[JSON] Ping No Mac need down.\n");
		}
	    }
	    
	} else {
	    /* no 'state' reurned ,means that UTT Server is offline */
	    debug(LOG_DEBUG, "[JSON] Ping no 'state' in JSON format.");
	    *state = 0;
	}
	if(out!=NULL)
	    free(out);
	cJSON_Delete(json);
    }
    } else {
	*state = 0;
    } 
}

/** @internal
 * This function does the actual request.
 */
static void
ping(void)
{
        ssize_t			numbytes;
        size_t	        	totalbytes;
	int			sockfd, nfds, done;
	char			request[MAX_BUF];
	fd_set			readfds;
	struct timeval		timeout;
	FILE * fh;
	unsigned long int sys_uptime  = 0;
	unsigned int      sys_memfree = 0;
	//float             sys_load    = 0;
	t_auth_serv	*auth_server = NULL;
	auth_server = get_auth_server();
	char 		  sys_load_str[10];	
#ifdef PARAM_SIGN
#else
	char *pongP = NULL;
#endif
	char timeS[TIME_STAMP_LEN];
	memset(sys_load_str, 0, sizeof(sys_load_str));
	
	debug(LOG_DEBUG, "Entering ping()");
	
	/*
	 * The ping thread does not really try to see if the auth server is actually
	 * working. Merely that there is a web server listening at the port. And that
	 * is done by connect_auth_server() internally.
	 */
	sockfd = connect_auth_server();
#if 0
	/*delete needless iptables rules*/
	time_t tmp;
	time(&tmp);
	if(is_auth_online()) {
	    debug(LOG_DEBUG, "=== auth server on, wifi free auth, time: %s=== ping_thread",ctime(&tmp));
	    iptables_fw_add_del(IPTABLE_DEL);
	} else {
	    debug(LOG_DEBUG, "=== auth server off, wifi free auth, time: %s=== ping_thread",ctime(&tmp));
	    iptables_fw_add_del(IPTABLE_ADD);
	}
#endif
	if (sockfd == -1) {
		/*
		 * No auth servers for me to talk to
		 */
		return;
	}
	//ProfInit();
	/*
	 * Populate uptime, memfree and load
	 */
	if ((fh = fopen("/proc/uptime", "r"))) {
		fscanf(fh, "%lu", &sys_uptime);
		fclose(fh);
	}
	if ((fh = fopen("/proc/meminfo", "r"))) {
		while (!feof(fh)) {
			if (fscanf(fh, "MemFree: %u", &sys_memfree) == 0) {
				/* Not on this line */
				while (!feof(fh) && fgetc(fh) != '\n');
			}
			else {
				/* Found it */
				break;
			}
		}
		fclose(fh);
	}
	if ((fh = fopen("/proc/loadavg", "r"))) {
		//fscanf(fh, "%f", &sys_load);
		fscanf(fh, "%s", sys_load_str);
		fclose(fh);
	}
	memset(timeS, 0, sizeof(timeS));
	getTimeStamp(timeS);
#if (WIFIDOG_STDIF == FYES)
	snprintf(request, sizeof(request) - 1,
		"GET %s%sgw_id=%s&sys_uptime=%lu&sys_memfree=%u&sys_load=%s&wifidog_uptime=%lu HTTP/1.0\r\n"
		"User-Agent: WiFiDog %s\r\n"
		"Host: %s\r\n"
		"\r\n",
		auth_server->authserv_path,
		auth_server->authserv_ping_script_path_fragment,
		config_get_config()->gw_id,
		sys_uptime,
		sys_memfree,
		sys_load_str,
		(long unsigned int)((long unsigned int)time(NULL) - (long unsigned int)started_time),
		VERSION,
		auth_server->authserv_hostname);
#else
#ifdef PARAM_SIGN
	char tmsStr[SIGN_PARAM_VALUE_LEN];
	long unsigned int wifiuptime = 0U;
	wifiuptime = (long unsigned int)((long unsigned int)time(NULL) - (long unsigned int)started_time);
	LOCK_PARAM_SIGN();
	initParamval();
	snprintf(tmsStr,SIGN_PARAM_VALUE_LEN,"%u",config_get_config()->tmpSN);
	addParamValue("deviceid", tmsStr);
	snprintf(tmsStr,SIGN_PARAM_VALUE_LEN,"%lu",sys_uptime);
	addParamValue("sys_uptime", tmsStr);
	snprintf(tmsStr,SIGN_PARAM_VALUE_LEN,"%u",sys_memfree);
	addParamValue("sys_memfree", tmsStr);
	addParamValue("sys_load", sys_load_str);
	snprintf(tmsStr,SIGN_PARAM_VALUE_LEN,"%lu", wifiuptime);
	addParamValue("wifi_uptime", tmsStr);
	addParamValue("timestamp", timeS);
	addParamValue("mac", maclist);
#if (SERVER_VERSION == FYES)
    addParamValue("v",SERV_VERSION);
#endif
	//getSha1Result();
	unsigned char hexData1[41];
	getSha1Result(hexData1);
	UNLOCK_PARAM_SIGN();
	/*
	 * Prep & send request
	 */
	snprintf(request, sizeof(request) - 1,
			"GET %s%ssign=%s%s&deviceid=%u&sys_uptime=%lu&sys_memfree=%u&sys_load=%s&wifi_uptime=%lu&timestamp=%s&mac=%s HTTP/1.0\r\n"
			"User-Agent: utt WiFi Auth %s\r\n"
			"Host: %s\r\n"
			"\r\n",
			auth_server->authserv_path,
			auth_server->authserv_ping_script_path_fragment,
			hexData1,
			AUTH_SERV_VER,
			config_get_config()->tmpSN,
			sys_uptime,
			sys_memfree,
			sys_load_str,
			wifiuptime,
			timeS,
			maclist,
			VERSION,
			auth_server->authserv_hostname);
#else

	/*
	 * Prep & send request
	 */
	snprintf(request, sizeof(request) - 1,
			"GET %s%sdeviceid=%u&sys_uptime=%lu&sys_memfree=%u&sys_load=%s&wifi_uptime=%lu&timestamp=%s&mac=%s HTTP/1.0\r\n"
			"User-Agent: utt WiFi Auth %s\r\n"
			"Host: %s\r\n"
			"\r\n",
			auth_server->authserv_path,
			auth_server->authserv_ping_script_path_fragment,
			config_get_config()->tmpSN,
			sys_uptime,
			sys_memfree,
			sys_load_str,
			(long unsigned int)((long unsigned int)time(NULL) - (long unsigned int)started_time),
			timeS,
			maclist,
			VERSION,
			auth_server->authserv_hostname);
#endif
#endif

	debug(LOG_DEBUG, "HTTP Request to Server: [%s]", request);
	memset(maclist, 0, sizeof(maclist));
	send(sockfd, request, strlen(request), 0);

	debug(LOG_DEBUG, "Reading response");
	//ProfDetach();
	numbytes = totalbytes = 0;
	done = 0;
	do {
		FD_ZERO(&readfds);
		FD_SET(sockfd, &readfds);
		timeout.tv_sec = 15; /* XXX magic... 30 second */
		timeout.tv_usec = 0;
		nfds = sockfd + 1;

		nfds = select(nfds, &readfds, NULL, NULL, &timeout);

		if (nfds > 0) {
			/** We don't have to use FD_ISSET() because there
			 *  was only one fd. */
			numbytes = read(sockfd, request + totalbytes, MAX_BUF - (totalbytes + 1));
			if (numbytes < 0) {
				debug(LOG_ERR, "An error occurred while reading from auth server: %s", strerror(errno));
				/* FIXME */
				close(sockfd);
				return;
			}
			else if (numbytes == 0) {
				done = 1;
			}
			else {
				totalbytes += numbytes;
				//debug(LOG_DEBUG, "Read %d bytes, total now %d", numbytes, totalbytes);
			}
		}
		else if (nfds == 0) {
			debug(LOG_ERR, "Timed out reading data via select() from auth server");
			authEn_ping = 0;
			/* FIXME */
			close(sockfd);
			return;
		}
		else if (nfds < 0) {
			debug(LOG_ERR, "Error reading data via select() from auth server: %s", strerror(errno));
			authEn_ping = 0;
			/* FIXME */
			close(sockfd);
			return;
		}
	} while (!done);
	close(sockfd);

	debug(LOG_DEBUG, "Done reading reply, total %d bytes", totalbytes);

	request[totalbytes] = '\0';

	debug(LOG_DEBUG, "HTTP Response from Server: [%s]", request);
	
#ifdef PARAM_SIGN
#if 0
    /* 2.2 的ping返回值采用JSON的格式返回
     * 且不带数字签名
     * 处理在else中
     * */
	    char *signP = NULL;
	    char *timeP = NULL;
	    int result = 0;
	    signP = pongP+5;
	    debug(LOG_WARNING, "signP =%s",signP);
	    if((timeP = strstr(signP, ";"))!=NULL) {
		*timeP = '\0';
		timeP +=1;
		debug(LOG_WARNING, "signP =%s,timeP =%s",signP,timeP);
		if(strlen(signP) ==40) {
		    initParamval();
		    addParamValue("2222", timeP);
		    getSha1Result();
		    if(memcmp(signP, hexData, 40)==0) {
			result = 1;
			debug(LOG_ERR,"[SIGN] Ping  signature check success.");
		    } else {
			debug(LOG_ERR, "[SIGN] Ping signature check error, return: %s, check: %s.",pongP,signP);
		    }
		} else {
		    debug(LOG_ERR, "[SIGN] Ping get signature length error, return: %s.",pongP);
		}
	    } else {
		    debug(LOG_ERR, "[SIGN] Ping get no signature, return: %s.",pongP);
	    }

	    if(result == 1) {
		LOCK_CLIENT_LIST();
		debug(LOG_DEBUG, "Auth Server Says: Pong");
		authEn_ping = 1;
		client_delete_by_mac(timeP+14);
		UNLOCK_CLIENT_LIST();
	    } else {
		debug(LOG_ERR, "[SIGN] Ping signature check error.");
		authEn_ping = 0;
	    }
#else
	    /* 2.2 返回值为JSON格式
	     * 添加了JSON的处理函数
	     * */
	    char *start = NULL;
	    if ((start=strstr(request, "{\"mac\":")) != NULL) {
		parse_ping(start, &authEn_ping);
	    } else {
		authEn_ping = 0;
		debug(LOG_DEBUG,"[JSON] Ping the string of ping returned, is not the incorect format.");
	    }
#endif

#else
	if ((pongP=strstr(request, "Pong")) == NULL) {
		debug(LOG_WARNING, "Auth server did NOT say pong!");
		authEn_ping = 0;
		/* FIXME */
	}
	else {
		LOCK_CLIENT_LIST();
		debug(LOG_DEBUG, "Auth Server Says: Pong");
		authEn_ping = 1;
		client_delete_by_mac(request+5);
		UNLOCK_CLIENT_LIST();
	}
#endif
	/*write client list file*/
	//clist_list_write_file();

	return;	
}
#endif /* MAKE_MONEY_HEARTBEAT != FYES*/
