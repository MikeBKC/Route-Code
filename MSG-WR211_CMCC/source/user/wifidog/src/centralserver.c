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

/* $Id: centralserver.c,v 1.21.8.2 2015/06/24 14:14:13 hou.bo Exp $ */
/** @file centralserver.c
  @brief Functions to talk to the central server (auth/send stats/get rules/etc...)
  @author Copyright (C) 2004 Philippe April <papril777@yahoo.com>
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>

#include "httpd.h"

#include "common.h"
#include "safe.h"
#include "util.h"
#include "auth.h"
#include "conf.h"
#include "debug.h"
#include "centralserver.h"
#include "firewall.h"
#include "../config.h"
#include "cJSON.h"
#include "uttMachine.h"
#include "mib.h"
#include <linux/netfilter_ipv4/utt_netfilter.h>
#include "profacce.h"

#if (MAKE_MONEY_HEARTBEAT != FYES)
extern pthread_t tid_ping; 
#endif

extern pthread_mutex_t	config_mutex;

#ifdef PARAM_SIGN
#include "signature.h"
extern pthread_mutex_t param_sign_mutex;
#endif

void 
do_auth(char *str,t_authresponse *authresponse)
{
    /*
     * Auth 响应的JSON内容 
     * state替代Auth 1表示通过，0失败
     * {"expire":"120","state":"1","timestamp":"20140808111701"}
     * {"state":"0","timestamp":"20140808111701"}
     *      */
    //char text[] = "{\"expire\":\"120\",\"state\":\"1\",\"timestamp\":\"20140808111701\"}";
    char *out;
    cJSON *json;
    cJSON *item;
    cJSON *macP;
    int time = 0;
#if (WIFI_RATE_LIMIT == FYES) /*带宽控制*/
    int uprate = 0;
    int downrate = 0;
#endif
    
    if(str!=NULL) {
	json=cJSON_Parse(str);
	if (!json) {
	    debug(LOG_ERR,"[JSON] Auth Json Error, string =%s, before: [%s]\n",str,cJSON_GetErrorPtr());
	    return(AUTH_ERROR);
	}
	else
	{
	debug(LOG_DEBUG, "[JSON] Auth, json print:\n%s\n",((out=cJSON_Print(json))?out:"null"));
	//debug(LOG_DEBUG,"[JSON] Auth json type =%d\n",json->type);

	if((item = cJSON_GetObjectItem(json, "state"))!=NULL) {
	    //debug(LOG_DEBUG,"[JSON] Auth 'state' type =%d\n",item->type);
	    //debug(LOG_DEBUG,"[JSON] Auth 'state' string =%s\n",item->string);
	    //debug(LOG_DEBUG,"[JSON] Auth 'state' valuestring =%s\n",item->valuestring);
	    /*state == "1" ,indicate that UTT Server is online*/
	    if(strcmp(item->valuestring, "1") == 0) {
		//debug(LOG_DEBUG,"[JSON] Auth 'state' = %s\n",item->valuestring);
		authresponse->authcode = 1; /*认证成功，允许访问网络*/
		if((item = cJSON_GetObjectItem(json, "expire"))!=NULL) {
		    //debug(LOG_DEBUG,"[JSON] Auth 'expire' type =%d\n",item->type);
		    //debug(LOG_DEBUG,"[JSON] Auth 'expire' string =%s\n",item->string);
		    debug(LOG_DEBUG,"[JSON] Auth 'expire' valuestring =%s, %d\n",item->valuestring,atoi(item->valuestring));
		    time = atoi(item->valuestring);
		    authresponse->expire = time*60;
#if 0
		    if(item->next != NULL) {
			printf("next type =%d\n",item->next->type);
			printf("next string =%s\n",item->next->string);
			printf("next valuestring =%s\n",item->next->valuestring);
		    } else {
			printf("next type =null\n");
		    }
#endif
		} else {
		    authresponse->expire = DEFAULT_AUTHTIME;
		    /*默认取值20分钟*/
		}
#if(WIFIDOG_MAC_AUTH == FYES)
		if((item = cJSON_GetObjectItem(json, "authType"))!=NULL) {
		    debug(LOG_DEBUG,"[JSON] Auth 'authType' valuestring =%s, %d\n",item->valuestring,atoi(item->valuestring));
		    if(strcmp(item->valuestring, "4") == 0){
			authresponse->authType =WIFIDOG_CLIENT_AUTH_TYPE_MAC_WHITE ;
		    }else if(strcmp(item->valuestring, "3") == 0){
			authresponse->authType =WIFIDOG_CLIENT_AUTH_TYPE_MAC_GREY ;
			if(authresponse->expire == 0)
			{
			    authresponse->authType =WIFIDOG_CLIENT_AUTH_TYPE_MAC_BLACK ;
			    authresponse->expire = DEFAULT_AUTHTIME;
			}
		    }else{
			authresponse->authType =WIFIDOG_CLIENT_AUTH_TYPE_MAC_BLACK ;
		    }
		} else {
			authresponse->authType = WIFIDOG_CLIENT_AUTH_TYPE_UNKOWN ;
		}
#endif
#if (WIFI_RATE_LIMIT == FYES) /*带宽控制*/
		/* 注意带宽的单位，需要转换
		 * 注意四字节保存大小是否超过最大值*/
		if((item = cJSON_GetObjectItem(json, "txrate"))!=NULL) {
		    debug(LOG_DEBUG,"[JSON] Auth 'txrate' valuestring =%s, %d\n",item->valuestring,atoi(item->valuestring));
		    uprate = atoi(item->valuestring);
		    authresponse->upRate = uprate;
		} else {
		    authresponse->upRate = 0;
		    debug(LOG_DEBUG,"[JSON] Auth 'txrate' string is not find.\n");
		}
		if((item = cJSON_GetObjectItem(json, "rxrate"))!=NULL) {
		    debug(LOG_DEBUG,"[JSON] Auth 'rxrate' valuestring =%s, %d\n",item->valuestring,atoi(item->valuestring));
		    downrate = atoi(item->valuestring);
		    authresponse->downRate = downrate;
		} else {
		    authresponse->downRate = 0;
		    debug(LOG_ERR,"[JSON] Auth 'rxrate' string is not find.\n");
		
		}
#endif
	    } else {
		//debug(LOG_ERR, "[JSON] Auth 'state' value %s",item->valuestring);
		authresponse->authcode = 0; /*默认为认证失败，禁止访问网路*/
		authresponse->expire = 0;
	    }
	    
	} else {
	    debug(LOG_ERR,"[JSON] Auth json has no 'state' value.");
	    authresponse->authcode = AUTH_ERROR; /*默认为认证失败，禁止访问网路*/
	}
	if(out!=NULL)
	    free(out);
	cJSON_Delete(json);
    }
    } else {
	    debug(LOG_ERR,"[JSON] Auth get string null.");
	    authresponse->authcode = AUTH_ERROR; /*默认为认证失败，禁止访问网路*/
    }
}

/** Initiates a transaction with the auth server, either to authenticate or to
 * update the traffic counters at the server
@param authresponse Returns the information given by the central server 
@param request_type Use the REQUEST_TYPE_* defines in centralserver.h
@param ip IP adress of the client this request is related to
@param mac MAC adress of the client this request is related to
@param token Authentification token of the client
@param incoming Current counter of the client's total incoming traffic, in bytes 
@param outgoing Current counter of the client's total outgoing traffic, in bytes 
*/
t_authcode
auth_server_request(t_authresponse *authresponse, const char *request_type, const char *ip, const char *mac, const char *token, unsigned long long int incoming, unsigned long long int outgoing)
{
	int sockfd;
	ssize_t	numbytes;
	size_t totalbytes;
	char buf[MAX_BUF];
	char *tmp;
        char *safe_token = NULL;
	int done, nfds;
	fd_set			readfds;
	struct timeval		timeout;
	t_auth_serv	*auth_server = NULL;
	auth_server = get_auth_server();
	s_config *config = config_get_config();
	
	/* Blanket default is error. */
	authresponse->authcode = AUTH_ERROR;
	char timeS[TIME_STAMP_LEN];
	char *macP = NULL;
	
#if 0
	if (strcmp(request_type, REQUEST_TYPE_WECHAT_ATTENTION) && strcmp(request_type, REQUEST_TYPE_MAC_VALIDATE))
	    sockfd = connect_auth_server();
	else
#endif
	    sockfd = connect_auth_server();
	if (sockfd == -1) {
		/* Could not connect to any auth server */
		return (AUTH_ERROR);
	}

	/**
	 * TODO: XXX change the PHP so we can harmonize stage as request_type
	 * everywhere.
	 */
	memset(buf, 0, sizeof(buf));
	if (token)
	    safe_token=httpdUrlEncode(token);
	memset(timeS, 0, sizeof(timeS));
	getTimeStamp(timeS);
	macP = convertMac(mac,0);
#if   (WIFIDOG_STDIF == FYES)
    snprintf(buf, (sizeof(buf) - 1),
	"GET %s%sstage=%s&ip=%s&mac=%s&token=%s&incoming=%llu&outgoing=%llu&gw_id=%s HTTP/1.0\r\n"
	"User-Agent: WiFiDog %s\r\n"
	"Host: %s\r\n"
	"\r\n",
	auth_server->authserv_path,
	auth_server->authserv_auth_script_path_fragment,
	request_type,
	ip,
	mac,
	safe_token?safe_token:"",
	incoming,
	outgoing,
                config_get_config()->gw_id,
	VERSION,
	auth_server->authserv_hostname
    );
#else
#ifdef PARAM_SIGN
	char tmsStr[SIGN_PARAM_VALUE_LEN];
	LOCK_PARAM_SIGN();
	initParamval();
	snprintf(tmsStr,SIGN_PARAM_VALUE_LEN,"%u",config->tmpSN);
	addParamValue("deviceid", tmsStr);
	addParamValue("ip", ip);
	addParamValue("mac", ((macP!=NULL)?(macP):("")));
	if (strcmp(request_type, REQUEST_TYPE_WECHAT_ATTENTION) && strcmp(request_type, REQUEST_TYPE_MAC_VALIDATE)){
	    addParamValue("token", (safe_token?safe_token:("")));
	}
	addParamValue("timestamp", timeS);
#if (SERVER_VERSION == FYES)
    addParamValue("v",SERV_VERSION);
#endif
	//getSha1Result();
	unsigned char hexData1[41];
	getSha1Result(hexData1);
	UNLOCK_PARAM_SIGN();
	char requestTypeStr[50];

	if (strcmp(request_type, REQUEST_TYPE_WECHAT_ATTENTION) && strcmp(request_type, REQUEST_TYPE_MAC_VALIDATE))
	{
	    snprintf(buf, (sizeof(buf) - 1),
		    "GET %s%ssign=%s%s&ip=%s&mac=%s&token=%s&deviceid=%u&timestamp=%s HTTP/1.0\r\n"
		    "User-Agent: utt WiFi Auth %s\r\n"
		    "Host: %s\r\n"
		    "\r\n",
		    auth_server->authserv_path,
		    auth_server->authserv_auth_script_path_fragment,
		    hexData1,
		    AUTH_SERV_VER,
		    ip,
		    ((macP!=NULL)?(macP):("")),
		    safe_token?safe_token:"",
		    config->tmpSN,
		    timeS,
		    VERSION,
		    auth_server->authserv_hostname
		    );
	}
	else
	{
	    snprintf(buf, (sizeof(buf) - 1),
		    "GET %sjson/%s?sign=%s%s&ip=%s&mac=%s&deviceid=%u&timestamp=%s HTTP/1.0\r\n"
		    "User-Agent: utt WiFi Auth %s\r\n"
		    "Host: %s\r\n"
		    "\r\n",
		    auth_server->authserv_path,
		    request_type,
		    hexData1,
		    AUTH_SERV_VER,
		    ip,
		    ((macP!=NULL)?(macP):("")),
		    config->tmpSN,
		    timeS,
		    VERSION,
		    auth_server->authserv_hostname
		    );
	}
	
#else
	//ProfInit();
	snprintf(buf, (sizeof(buf) - 1),
		"GET %s%sip=%s&mac=%s&token=%s&deviceid=%u&timestamp=%s HTTP/1.0\r\n"
		"User-Agent: utt WiFi Auth %s\r\n"
		"Host: %s\r\n"
		"\r\n",
		auth_server->authserv_path,
		auth_server->authserv_auth_script_path_fragment,
		ip,
		((macP!=NULL)?(macP):("")),
		safe_token?safe_token:"",
		config->tmpSN,
		timeS,
		VERSION,
		auth_server->authserv_hostname
	);
#endif
#endif
	if(macP != NULL) {
	    free(macP);
	}

        free(safe_token);
	//ProfDetach();

	//debug(LOG_DEBUG, "Sending HTTP request to auth server: [%s]\n", buf);
	send(sockfd, buf, strlen(buf), 0);

	//debug(LOG_DEBUG, "Reading response");
	numbytes = totalbytes = 0;
	done = 0;
	do {
		FD_ZERO(&readfds);
		FD_SET(sockfd, &readfds);
		timeout.tv_sec = 30; /* XXX magic... 30 second is as good a timeout as any */
		timeout.tv_usec = 0;
		nfds = sockfd + 1;

		nfds = select(nfds, &readfds, NULL, NULL, &timeout);

		if (nfds > 0) {
			/** We don't have to use FD_ISSET() because there
			 *  was only one fd. */
			numbytes = read(sockfd, buf + totalbytes, MAX_BUF - (totalbytes + 1));
			if (numbytes < 0) {
				debug(LOG_ERR, "An error occurred while reading from auth server: %s", strerror(errno));
				/* FIXME */
				close(sockfd);
				return (AUTH_ERROR);
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
			/* FIXME */
			close(sockfd);
			return (AUTH_ERROR);
		}
		else if (nfds < 0) {
			debug(LOG_ERR, "Error reading data via select() from auth server: %s", strerror(errno));
			/* FIXME */
			close(sockfd);
			return (AUTH_ERROR);
		}
	} while (!done);

	close(sockfd);

	buf[totalbytes] = '\0';
	//debug(LOG_DEBUG, "HTTP Response from Server: [%s]", buf);
	
#ifdef PARAM_SIGN
#if 0
	    char *sign = NULL;
	    char signP[41];
	    char *timeP = NULL;
	    int result = 0;
	    memset(signP, 0, sizeof(signP));
	    if ((sign = strstr(buf, "sign:"))!=NULL) {
		sign +=5;
		snprintf(signP, 41, "%s",sign);
		debug(LOG_WARNING, "signP =%s",signP);
		if ((timeP = strstr(buf, "timestamp:"))!=NULL) {
		    debug(LOG_WARNING, "timeP =%s",timeP);
			
	LOCK_PARAM_SIGN();
			initParamval();
			addParamValue("1111", timeP);
			//getSha1Result();
			unsigned char hexData1[41];
			getSha1Result(hexData1);
	UNLOCK_PARAM_SIGN();
			if(memcmp(signP, hexData1, 40)==0) {
			    result = 1;
			    debug(LOG_ERR,"[SIGN] auth state and time, check signature success.");
			} else {
			    debug(LOG_ERR, "[SIGN] auth state and time signature check error, return: %s, check: %s.",sign,signP);
			}
		} else {
		    debug(LOG_ERR, "[SIGN] auth state and time get no 'timestamp', return: %s.",sign);
		}
	    } else {
		debug(LOG_ERR, "[SIGN] auth state and time get no 'sign', return: %s.",buf);
	    }
	    if(result == 1) {
	    } else {
		authresponse->authcode = AUTH_SIGN_ERR;
	    }
#else
	    char *start = NULL;
	    start = strstr(buf, "{\"");
	    if(start!=NULL) {
		debug(LOG_DEBUG,"[JSON] Auth start %s",start);
		do_auth(start,authresponse);
	    }
#endif
#else
	/*以下已弃用，最初始的处理方式*/
	if ((tmp = strstr(buf, "Auth:"))) {
		if (sscanf(tmp, "Auth: %d", (int *)&authresponse->authcode) == 1) {
			debug(LOG_INFO, "Auth server returned authentication code %d", authresponse->authcode);
			if(authresponse->authcode == 1) { /*认证成功，允许访问网络*/
			    if ((tmp = strstr(buf, "expire: "))) {
				if (sscanf(tmp, "expire: %d", (int *)&authresponse->expire) == 1) {
				    authresponse->expire = (authresponse->expire)*60;
				    debug(LOG_INFO, "Auth server returned authentication expire %d", authresponse->expire);
				} else {
				    authresponse->expire = config->Auth_time;
				}
			    } else {
				authresponse->expire = config->Auth_time;
			    }
#if (WIFI_RATE_LIMIT == FYES) /*带宽控制*/
			    if ((tmp = strstr(buf, "txrate: "))) {
				/* 注意带宽的单位，需要转换
				 * 注意四字节保存大小是否超过最大值*/
				if (sscanf(tmp, "txrate: %d", (int *)&authresponse->upRate) == 1) {
				    debug(LOG_DEBUG, "Auth server returned authentication up rate %d", authresponse->upRate);
				    authresponse->upRate = (authresponse->upRate);
				}
			    }
			    if ((tmp = strstr(buf, "rxrate: "))) {
				/*注意带宽的单位，需要转换*/
				if (sscanf(tmp, "rxrate: %d", (int *)&authresponse->downRate) == 1) {
				    debug(LOG_DEBUG, "Auth server returned authentication down rate %d", authresponse->downRate);
				    authresponse->downRate = (authresponse->downRate);
				}
			    }
#endif
			} else if(authresponse->authcode == 0){ /*认证失败，禁止访问网络*/
				authresponse->expire = 0;
			} else {
			    authresponse->authcode = 0; /*默认为认证失败，禁止访问网路*/
			    authresponse->expire = 0;
			}
			return(authresponse->authcode);
		} else {
			debug(LOG_WARNING, "Auth server did not return expected authentication code");
			return(AUTH_ERROR);
		}
	}
	else {
		return(AUTH_ERROR);
	}
#endif

	/* XXX Never reached because of the above if()/else pair. */
	return(AUTH_ERROR);
}

/* Tries really hard to connect to an auth server. Returns a file descriptor, -1 on error
 */
int connect_auth_server() {
	int sockfd;

	//LOCK_CONFIG();
	sockfd = _connect_auth_server(0);
	//UNLOCK_CONFIG();

	if (sockfd == -1) {
		debug(LOG_ERR, "Failed to connect to any of the auth servers");
		mark_auth_offline();
	}
	else {
		//debug(LOG_DEBUG, "Connected to auth server");
		mark_auth_online();
	}
#ifdef WIFI_PRE_REDIRECT
#if(WIFIDOG_MAKE_MONEY == FYES)
	execute("echo 1 > /proc/sys/net/netfilter/utt_wifi_online", 0);
#else
	if((!is_auth_online()) || (!is_online())) 
	{
	    execute("echo 255 > /proc/sys/net/netfilter/utt_wifi_online", 0);
	} else {
	    execute("echo 1 > /proc/sys/net/netfilter/utt_wifi_online", 0);
	}
#endif
#endif
	return (sockfd);
}

/* Helper function called by connect_auth_server() to do the actual work including recursion
 * DO NOT CALL DIRECTLY
 @param level recursion level indicator must be 0 when not called by _connect_auth_server()
 */
int _connect_auth_server(int level) {
	s_config *config = config_get_config();
	t_auth_serv *auth_server = NULL;
	struct in_addr *h_addr;
	t_secondary_auth_server *sec_server;
	struct in_addr *sec_addr;
	char * sec_ip;
	char * sec_hostname = NULL;
	int num_servers = 0;
	char * hostname = NULL;
	char * popular_servers[] = {
		  "www.google.com",
		  "www.yahoo.com",
		  NULL
	};
	char ** popularserver;
	char * ip;
	struct sockaddr_in their_addr;
	int sockfd;

	/* XXX level starts out at 0 and gets incremented by every iterations. */
	level++;

	/*
	 * Let's calculate the number of servers we have
	 */
	for (auth_server = config->auth_servers; auth_server; auth_server = auth_server->next) {
		num_servers++;
	}
	//debug(LOG_DEBUG, "Level %d: Calculated %d auth servers in list", level, num_servers);

	if (level > num_servers) {
		/*
		 * We've called ourselves too many times
		 * This means we've cycled through all the servers in the server list
		 * at least once and none are accessible
		 */
		return (-1);
	}

	/*
	 * Let's resolve the hostname of the top server to an IP address
	 */
	auth_server = config->auth_servers;
	hostname = auth_server->authserv_hostname;
	debug(LOG_DEBUG, "Level %d: Resolving auth server [%s]", level, hostname);
	h_addr = wd_gethostbyname(hostname);
	for (sec_server = config->secondary_auth_server; sec_server != NULL;sec_server = sec_server->next){
	    sec_hostname = sec_server->hostname;
	    debug(LOG_DEBUG, "Level %d: Resolving auth server [%s]", level, hostname);
	    sec_addr = wd_gethostbyname(sec_hostname);
	    if (!sec_addr) {
		debug(LOG_DEBUG, "Level %d: Marking secondary_auth_server [%s] as bad and trying next if possible", level, sec_hostname);
	    }else{
		debug(LOG_DEBUG, "Level %d: Resolving auth server [%s] succeeded = [%s]", level, sec_hostname, sec_ip);
		sec_ip = safe_strdup(inet_ntoa(*sec_addr));
		if(sec_server->ip != NULL) free(sec_server->ip);
		sec_server->ip = sec_ip;
		free (sec_addr);
	    }
	}	

	if (!h_addr) {
		/*
		 * DNS resolving it failed
		 *
		 * Can we resolve any of the popular servers ?
		 */
		//debug(LOG_DEBUG, "Level %d: Resolving auth server [%s] failed", level, hostname);

		for (popularserver = popular_servers; *popularserver; popularserver++) {
			debug(LOG_DEBUG, "Level %d: Resolving popular server [%s]", level, *popularserver);
			h_addr = wd_gethostbyname(*popularserver);
			if (h_addr) {
				//debug(LOG_DEBUG, "Level %d: Resolving popular server [%s] succeeded = [%s]", level, *popularserver, inet_ntoa(*h_addr));
				break;
			}
			else {
				debug(LOG_DEBUG, "Level %d: Resolving popular server [%s] failed", level, *popularserver);
			}
		}

		/* 
		 * If we got any h_addr buffer for one of the popular servers, in other
		 * words, if one of the popular servers resolved, we'll assume the DNS
		 * works, otherwise we'll deal with net connection or DNS failure.
		 */
		if (h_addr) {
			free (h_addr);
			/*
			 * Yes
			 *
			 * The auth server's DNS server is probably dead. Try the next auth server
			 */
			debug(LOG_DEBUG, "Level %d: Marking auth server [%s] as bad and trying next if possible", level, hostname);
			if (auth_server->last_ip) {
				free(auth_server->last_ip);
				auth_server->last_ip = NULL;
			}
			mark_auth_server_bad(auth_server);
			return _connect_auth_server(level);
		}
		else {
			/*
			 * No
			 *
			 * It's probably safe to assume that the internet connection is malfunctioning
			 * and nothing we can do will make it work
			 */
			mark_offline();
			debug(LOG_DEBUG, "Level %d: Failed to resolve auth server and all popular servers. "
					"The internet connection is probably down", level);
			return(-1);
		}
	}
	else {
		/*
		 * DNS resolving was successful
		 */
		ip = safe_strdup(inet_ntoa(*h_addr));
		debug(LOG_DEBUG, "Level %d: Resolving auth server [%s] succeeded = [%s]", level, hostname, ip);

		if (!auth_server->last_ip || strcmp(auth_server->last_ip, ip) != 0) {
			/*
			 * But the IP address is different from the last one we knew
			 * Update it
			 */
#ifdef WIFI_PRE_REDIRECT
			fw_clear_authservers(auth_server->last_ip);
#endif
			debug(LOG_DEBUG, "Level %d: Updating last_ip IP of server [%s] to [%s]", level, hostname, ip);
			if (auth_server->last_ip) free(auth_server->last_ip);
			auth_server->last_ip = ip;

			/* Update firewall rules */
#ifndef WIFI_PRE_REDIRECT
			fw_clear_authservers();
#endif
			fw_set_authservers();
		}
		else {
			/*
			 * IP is the same as last time
			 */
			free(ip);
		}

		/*
		 * Connect to it
		 */
		//debug(LOG_DEBUG, "Level %d: Connecting to auth server %s:%d", level, hostname, auth_server->authserv_http_port);

#if (MAKE_MONEY_HEARTBEAT != FYES)
		if(tid_ping == pthread_self())
		{
		    /*不开启心跳则pthread_ping只需解析dns成功就可以了。这里的sockfd仅做为解析dns成功的标记*/
		    return 1;
		}
#endif
		their_addr.sin_family = AF_INET;
		their_addr.sin_port = htons(auth_server->authserv_http_port);
		their_addr.sin_addr = *h_addr;
		memset(&(their_addr.sin_zero), '\0', sizeof(their_addr.sin_zero));
		free (h_addr);

		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			debug(LOG_ERR, "Level %d: Failed to create a new SOCK_STREAM socket: %s", strerror(errno));
			return(-1);
		}

		if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
			/*
			 * Failed to connect
			 * Mark the server as bad and try the next one
			 */
			debug(LOG_DEBUG, "Level %d: Failed to connect to auth server %s:%d (%s). Marking it as bad and trying next if possible", level, hostname, auth_server->authserv_http_port, strerror(errno));
			close(sockfd);
			mark_auth_server_bad(auth_server);
			return _connect_auth_server(level); /* Yay recursion! */
		}
		else {
			/*
			 * We have successfully connected
			 */
			//debug(LOG_DEBUG, "Level %d: Successfully connected to auth server %s:%d", level, hostname, auth_server->authserv_http_port);
			return sockfd;
		}
	}
}
