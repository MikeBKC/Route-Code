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

/* $Id: auth.c,v 1.16.4.12 2016/09/19 06:23:52 cao.yongxiang Exp $ */
/** @file auth.c
    @brief Authentication handling thread
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
#include <unistd.h>
#include <syslog.h>
#include <errno.h>
#include "common.h"

#include "httpd.h"
#include "http.h"
#include "safe.h"
#include "conf.h"
#include "debug.h"
#include "auth.h"
#include "centralserver.h"
#include "fw_iptables.h"
#include "firewall.h"
#include "client_list.h"
#include "util.h"
#include <linux/netfilter_ipv4/utt_netfilter.h>
#include "signature.h"

/* Defined in clientlist.c */
extern	pthread_mutex_t	client_list_mutex;

/* Defined in util.c */
extern long served_this_session;

#ifdef PARAM_SIGN
extern pthread_mutex_t param_sign_mutex;
#endif
/** Launches a thread that periodically checks if any of the connections has timed out
@param arg Must contain a pointer to a string containing the IP adress of the client to check to check
@todo Also pass MAC adress? 
@todo This thread loops infinitely, need a watchdog to verify that it is still running?
*/  

#if (UTT_KNOWIFI == FYES)
extern void knowifi_time_check(void)
{
    MibProfileType mibType = MIB_PROF_KNOWIFI_DOMAIN;
    KnowifiDomainProfile *prof = NULL;
    freeDomain *freedomain;
    char *ip = NULL;
    int i = 0, max, min;
    unsigned long now = time(NULL);
    unsigned long timeNow = now % 86400;
    unsigned int week = 0x1 << ((3 + now / 86400) % 7);
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++)
    {
        prof = (KnowifiDomainProfile*)ProfGetInstPointByIndex(mibType, i);
        if (prof != NULL && prof->head.active == TRUE)
        {
	    if (prof->mode == KNOWIFI_MODE_TIME)
	    {
		//printf("week : %d, timeNow : %lu\n", week, timeNow);
		if ((week & prof->weekdays)
			&& timeNow >= prof->timeStart
			&& timeNow <= prof->timeEnd)
		{
		    if (prof->isFree == 0)
		    {
			//printf("mac : %s, no auth\n", prof->mac);
			ip = arp_get_by_mac(prof->mac);
			if (ip)
			{
			    strncpy(prof->ip, ip, sizeof(prof->ip));
			    fw_allow(prof->ip, prof->mac, FW_MARK_KNOWN);
			    prof->isFree = 1;
			    free(ip);
			}
		    }
		}
		else if (prof->isFree == 1)
		{
		    //printf("mac : %s, redirect\n", prof->mac);
		    fw_deny(prof->ip, prof->mac, FW_MARK_KNOWN);	
		    prof->isFree = 0;
		}
	    }
	    else if (prof->mode == KNOWIFI_MODE_FORBBIDEN && prof->isFree == 1)
	    {
		fw_deny(prof->ip, prof->mac, FW_MARK_KNOWN);	
		prof->isFree = 0;
	    }
        }
    }
    return ;
}
#endif

void
thread_client_timeout_check(const void *arg)
{
	pthread_cond_t		cond = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t		cond_mutex = PTHREAD_MUTEX_INITIALIZER;
	struct	timespec	timeout;
	
	while (1) {
		/* Sleep for config.checkinterval seconds... */
		timeout.tv_sec = time(NULL) + config_get_config()->checkinterval;
		timeout.tv_nsec = 0;

		/* Mutex must be locked for pthread_cond_timedwait... */
		pthread_mutex_lock(&cond_mutex);
		
		/* Thread safe "sleep" */
		pthread_cond_timedwait(&cond, &cond_mutex, &timeout);

		/* No longer needs to be locked */
		pthread_mutex_unlock(&cond_mutex);
	
		debug(LOG_DEBUG, "Running fw_counter()");

#if (UTT_KNOWIFI == FYES)
		knowifi_time_check();
		freeDomainCheckTimeOut(config_get_config()->checkinterval);
#else	
		fw_sync_with_authserver();
#endif
	}
}

#if (SMS_ENABLE == 1)
#if (SMS_DES_CRYPTO ==1)
extern void param_Des(const char *key, const unsigned char *data, char *enc_str);
#endif
#define RERUEN_STATE_MATCH "\"state\':"
int 
authserver_smsOperate(char *phone, int operate, char *result)
{
	int sockfd;
	ssize_t numbytes;
	size_t totalbytes;
	char buf[MAX_BUF];
	char *tmp=NULL;
	int done, nfds;
	fd_set                  readfds;
	struct timeval          timeout;
	t_auth_serv     *auth_server = NULL;
	int ret = 0, getcode = 0;
	char password[5];
#if (SMS_DES_CRYPTO == 1)
	char parambuf[1024];
	char encbuf[1024];
#endif

	if(phone !=NULL) {
	auth_server = get_auth_server();
	s_config *config = config_get_config();

	sockfd = connect_auth_server();
	if (sockfd == -1) {
	    /* Could not connect to any auth server */
	    ret = -2;
	} else {

#if (SMS_DES_CRYPTO == 1)
	memset(buf, 0, sizeof(buf));
	memset(parambuf, 0, sizeof(parambuf));
	memset(encbuf, 0, sizeof(encbuf));
	snprintf(parambuf,1024,"phone=%s&deviceid=%u",phone,config->tmpSN);
	param_Des("12345678", parambuf, encbuf);
	debug(LOG_WARNING, "encbuf =%s\n", encbuf);

	if(operate == 1) {  /*get phone password*/
	snprintf(buf, (sizeof(buf) - 1),
		"GET %s%sphone=%s&deviceid=%u&data=%s HTTP/1.0\r\n"
		"User-Agent: utt WiFi Auth %s\r\n"
		"Host: %s\r\n"
		"\r\n",
		auth_server->authserv_path,
		"json/localPhoneRegin?",
		phone,
		config->tmpSN,
		encbuf,
		VERSION,
		auth_server->authserv_hostname
	);
	} else if(operate == 2) {   /*get forgot password*/
	snprintf(buf, (sizeof(buf) - 1),
		"GET %s%sphone=%s&deviceid=%u&data=%s HTTP/1.0\r\n"
		"User-Agent: utt WiFi Auth %s\r\n"
		"Host: %s\r\n"
		"\r\n",
		auth_server->authserv_path,
		"json/localForgotPasswd?",
		phone,
		config->tmpSN,
		encbuf,
		VERSION,
		auth_server->authserv_hostname
	);
	} else {
	    /* opther operation */
	}
#else
	if(operate == 1) {  /*get phone password*/
	snprintf(buf, (sizeof(buf) - 1),
		"GET %s%sphone=%s&deviceid=%u HTTP/1.0\r\n"
		"User-Agent: utt WiFi Auth %s\r\n"
		"Host: %s\r\n"
		"\r\n",
		auth_server->authserv_path,
		"json/localPhoneRegin?",
		phone,
		config->tmpSN,
		VERSION,
		auth_server->authserv_hostname
	);
	} else if(operate == 2) {   /*get forgot password*/
	snprintf(buf, (sizeof(buf) - 1),
		"GET %s%sphone=%s&deviceid=%u HTTP/1.0\r\n"
		"User-Agent: utt WiFi Auth %s\r\n"
		"Host: %s\r\n"
		"\r\n",
		auth_server->authserv_path,
		"json/localForgotPasswd?",
		phone,
		config->tmpSN,
		VERSION,
		auth_server->authserv_hostname
	);
	} else {
	    /* opther operation */
	}
#endif

	debug(LOG_WARNING, "Sending HTTP request to auth server: [%s]\n", buf);
	send(sockfd, buf, strlen(buf), 0);

	debug(LOG_WARNING, "Reading response");
	numbytes = totalbytes = 0;
	done = 0;
	do {
	    FD_ZERO(&readfds);
	    FD_SET(sockfd, &readfds);
	    timeout.tv_sec = 15; /* XXX magic... 15 second is as good a timeout as any */
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
		    ret = -1;
		}
		else if (numbytes == 0) {
		    done = 1;
		}
		else {
		    totalbytes += numbytes;
		    debug(LOG_WARNING, "Read %d bytes, total now %d", numbytes, totalbytes);
		}
	    }
	    else if (nfds == 0) {
		debug(LOG_ERR, "Timed out reading data via select() from auth server");
		/* FIXME */
		ret = -1;
	    }
	    else if (nfds < 0) {
		debug(LOG_ERR, "Error reading data via select() from auth server: %s", strerror(errno));
		/* FIXME */
		ret = -1;
	    }
	} while (!done);

	close(sockfd);

	if(ret >=0) {
	buf[totalbytes] = '\0';
	debug(LOG_WARNING, "HTTP Response from Server[SMS RCV]: [%s]", buf);

	if ((tmp = strstr(buf, "\"state\":"))!=NULL) {
	    debug(LOG_WARNING, "tmp=%s",tmp);
	    if (sscanf(tmp, "\"state\":\"%d\"", &getcode) == 1) {
		debug(LOG_WARNING, "Auth server returned state %d", getcode);
		if(getcode == 1) { /*认证成功，允许访问网络*/
		    if ((tmp = strstr(buf, "\"code\":"))!=NULL) {
			if (sscanf(tmp, "\"code\":\"%s\"", password) == 1) {
			    debug(LOG_WARNING, "phone %s get password %s", phone, password);
			    if(result!=NULL)
				strncpy(result, password, 4);
			    ret = 1;
			} else {
			    ret = -1;
			}
		    } else {
			ret = -1;
		    }
		} else if(getcode == 2){ /*认证失败，禁止访问网络*/
		    tmp = NULL;
		    if((tmp = strstr(buf, "{\"state\":"))!=NULL) {
			if(result!=NULL)
			    snprintf(result, 100, "%s", tmp);
			debug(LOG_WARNING, "phone %s get result %s", phone, result);
		    }
		    ret = 2;
		} else {
		    ret = -1;
		}
	    } else {
		debug(LOG_WARNING, "Auth server did not return expected authentication code");
		ret = -1;
	    }
	}
	else {
	    ret = -1;
	}
	}

	}

	}
	return ret;
}
#endif

/** Authenticates a single client against the central server and returns when done
 * Alters the firewall rules depending on what the auth server says
@param r httpd request struct
*/
void
authenticate_client(request *r)
{
	t_client	*client;
	t_authresponse	auth_response;
	char	*mac,
		*token;
	char *urlFragment = NULL;
	s_config	*config = NULL;
	t_auth_serv	*auth_server = NULL;
	char timeS[TIME_STAMP_LEN];
	char *macP = NULL;

	//LOCK_CLIENT_LIST();

	client = client_list_find_by_ip(r->clientAddr);

	if (client == NULL) {
		debug(LOG_ERR, "Could not find client for %s", r->clientAddr);
		//UNLOCK_CLIENT_LIST();
		return;
	}
	
#if 0
	mac = safe_strdup(client->mac);
	token = safe_strdup(client->token);
#endif
	mac = client->mac;
	token = client->token;
	//UNLOCK_CLIENT_LIST();
	if((mac!=NULL) && (token!=NULL)) {
	memset(&auth_response,0,sizeof(t_authresponse));
	
	/* 
	 * At this point we've released the lock while we do an HTTP request since it could
	 * take multiple seconds to do and the gateway would effectively be frozen if we
	 * kept the lock.
	 */
	auth_server_request(&auth_response, REQUEST_TYPE_LOGIN, r->clientAddr, mac, token, 0, 0);
	
	//LOCK_CLIENT_LIST();
	
	/* can't trust the client to still exist after n seconds have passed */
	client = client_list_find(r->clientAddr, mac);
	
	if (client == NULL) {
		debug(LOG_ERR, "Could not find client node for %s (%s)", r->clientAddr, mac);
		//UNLOCK_CLIENT_LIST();
		//free(token);
		//free(mac);
		return;
	}
	
	//free(token);
	//free(mac);

	/* Prepare some variables we'll need below */
	config = config_get_config();
	auth_server = get_auth_server();
	
	if(auth_response.expire <= 0)
	{
	    auth_response.authcode = AUTH_DENIED;
	}

	switch(auth_response.authcode) {

	case AUTH_ERROR:
#if   (WIFIDOG_STDIF == FYES)
		/* Error talking to central server */
		debug(LOG_ERR, "Got %d from central server authenticating token %s from %s at %s", auth_response, client->token, client->ip, client->mac);
		send_http_page(r, "Error!", "Error: We did not get a valid answer from the central server");
		break;
#endif

	case AUTH_DENIED:
#if   (WIFIDOG_STDIF == FYES)
		/* Central server said invalid token */
		debug(LOG_INFO, "Got DENIED from central server authenticating token %s from %s at %s - redirecting them to denied message", client->token, client->ip, client->mac);
		safe_asprintf(&urlFragment, "%smessage=%s",
			auth_server->authserv_msg_script_path_fragment,
			GATEWAY_MESSAGE_DENIED
		);
		http_send_redirect_to_auth(r, urlFragment, "Redirect to denied message");
		free(urlFragment);
#else
		/* Re-direct them to auth server */
#if 0
		mac = NULL;
		mac = arp_get(r->clientAddr);
#endif
	memset(timeS, 0, sizeof(timeS));
	getTimeStamp(timeS);
	macP = convertMac(client->mac,0);
#ifdef PARAM_SIGN
	char tmsStr[SIGN_PARAM_VALUE_LEN];
	LOCK_PARAM_SIGN();
	initParamval();
	snprintf(tmsStr,SIGN_PARAM_VALUE_LEN,"%u",config->tmpSN);
	addParamValue("deviceid", tmsStr);
	addParamValue("gwaddress", config->gw_address);
	snprintf(tmsStr,SIGN_PARAM_VALUE_LEN,"%d",config->gw_port);
	addParamValue("gwport", tmsStr);
	addParamValue("ip", client->ip);
#if (SERVER_VERSION == FYES)
    addParamValue("v",SERV_VERSION);
#endif
	if(macP!=NULL) {
	addParamValue("mac", macP);
	} else {
	addParamValue("mac", "");
	}
	addParamValue("timestamp", timeS);
	//getSha1Result();
	unsigned char hexData1[41];
	getSha1Result(hexData1);
	UNLOCK_PARAM_SIGN();
	
	safe_asprintf(&urlFragment, "%ssign=%s%s&gwaddress=%s&gwport=%d&deviceid=%u&ip=%s&mac=%s&timestamp=%s",
		auth_server->authserv_login_script_path_fragment,
		hexData1,
		AUTH_SERV_VER,
		config->gw_address,
		config->gw_port, 
		config->tmpSN,
		client->ip,
		((macP!=NULL)?(macP):("")),
		timeS);
#else
		//ProfInit();
		safe_asprintf(&urlFragment, "%sgwaddress=%s&gwport=%d&deviceid=%u&ip=%s&mac=%s&timestamp=%s",
			auth_server->authserv_login_script_path_fragment,
			config->gw_address,
			config->gw_port, 
			config->tmpSN,
			client->ip,
			((macP!=NULL)?(macP):("")),
			timeS);
#endif
	if(macP!=NULL) {
	    free(macP);
	}
		debug(LOG_INFO, "Captured ip %s mac %s requesting [] and re-directing them to login page", client->ip, client->mac);
		if(urlFragment!=NULL) {
		http_send_redirect_to_auth(r, urlFragment, "Redirect to login page");
		free(urlFragment);
		}
		//ProfDetach();
	LOCK_CLIENT_LIST();
		client_list_delete(client);
	UNLOCK_CLIENT_LIST();
#endif
		break;

    case AUTH_VALIDATION:
		/* They just got validated for X minutes to check their email */
		debug(LOG_INFO, "Got VALIDATION from central server authenticating token %s from %s at %s"
				"- adding to firewall and redirecting them to activate message", client->token,
				client->ip, client->mac);
		client->fw_connection_state = FW_MARK_PROBATION;
		fw_deny(client->ip, client->mac, FW_MARK_PROBATION); /*防止用户认证成功后，重复认证添加规则*/
		fw_allow(client->ip, client->mac, FW_MARK_PROBATION);
		safe_asprintf(&urlFragment, "%smessage=%s",
			auth_server->authserv_msg_script_path_fragment,
			GATEWAY_MESSAGE_ACTIVATE_ACCOUNT
		);
		if(urlFragment!=NULL) {
		http_send_redirect_to_auth(r, urlFragment, "Redirect to activate message");
		free(urlFragment);
		}
	    break;

    case AUTH_ALLOWED:
		/* Logged in successfully as a regular account */
		//debug(LOG_INFO, "Got ALLOWED from central server authenticating token %s from %s at %s - "
		//		"adding to firewall and redirecting them to portal", client->token, client->ip, client->mac);
		client->fw_connection_state = FW_MARK_KNOWN;
		client->expire = auth_response.expire;
		client->need_notify = 0;
		fw_deny(client->ip, client->mac, FW_MARK_KNOWN); /*防止用户认证成功后，重复认证添加规则*/
#if (WIFI_RATE_LIMIT == FYES) /*带宽控制*/
		if((auth_response.upRate > 0) || (auth_response.downRate > 0)) {
		    /*
		     * 带宽控制客户端添加到对应的限速组中
		     * */
		    createRateLimit(client->ip, client->mac, auth_response.upRate, auth_response.downRate, 0, 0);
		} else 
#endif
#if (SWITCH_AUTH_MODE == FYES)
		    if( client->authType == WIFIDOG_CLIENT_AUTH_TYPE_SWITCH_NO_MAC_AUTH ) {
			ipsetAOrDIpStr(IPSET_GROUP_SWITCH_CLIENT_LIST, client->ip, IPSET_MEM_ADD);

		    } else 
#endif
		{
		fw_allow(client->ip, client->mac, FW_MARK_KNOWN);
		}
        served_this_session++;
#if (WIFIDOG_USER_SUCCESS_URL == FYES) /*认证成功URL*/
	if(config->successUrl != NULL && strlen(config->successUrl)>0){
	    http_send_redirect(r, config->successUrl, NULL);
	    break;
	}
#endif
	memset(timeS, 0, sizeof(timeS));
	getTimeStamp(timeS);
#if   (WIFIDOG_STDIF == FYES)
	safe_asprintf(&urlFragment, "%sgw_id=%s",
	    auth_server->authserv_portal_script_path_fragment,
	    config->gw_id
	);
	http_send_redirect_to_auth(r, urlFragment, "Redirect to portal");
	free(urlFragment);
#else
#ifdef PARAM_SIGN
	{
	char tmsStr[SIGN_PARAM_VALUE_LEN];
	char serv_version[50];
	LOCK_PARAM_SIGN();
	initParamval();
	snprintf(tmsStr,SIGN_PARAM_VALUE_LEN,"%u",config->tmpSN);
	addParamValue("deviceid", tmsStr);
	snprintf(tmsStr,SIGN_PARAM_VALUE_LEN,"%d",(client->expire/60));
	addParamValue("expire", tmsStr);
	addParamValue("token", client->token);
	addParamValue("timestamp", timeS);
#if (SERVER_VERSION == FYES)
    addParamValue("v",SERV_VERSION);
#endif
	//getSha1Result();
	unsigned char hexData1[41];
	getSha1Result(hexData1);
	UNLOCK_PARAM_SIGN();
		
	safe_asprintf(&urlFragment, "%ssign=%s%s&deviceid=%u&expire=%d&token=%s&timestamp=%s",
		auth_server->authserv_portal_script_path_fragment,
		hexData1,
		AUTH_SERV_VER,
		config->tmpSN,
		(client->expire/60),
		(client->token),
		timeS
		);
	}
#else
		//ProfInit();
		safe_asprintf(&urlFragment, "%sdeviceid=%u&expire=%d&token=%s&timestamp=%s",
			auth_server->authserv_portal_script_path_fragment,
			config->tmpSN,
			(client->expire/60),
			(client->token),
			timeS
		);
#endif
#endif
		if(urlFragment!=NULL) {
		http_send_redirect_to_auth(r, urlFragment, NULL);
		free(urlFragment);
		}
		//ProfDetach();
	    break;

    case AUTH_VALIDATION_FAILED:
		 /* Client had X minutes to validate account by email and didn't = too late */
		debug(LOG_INFO, "Got VALIDATION_FAILED from central server authenticating token %s from %s at %s "
				"- redirecting them to failed_validation message", client->token, client->ip, client->mac);
		safe_asprintf(&urlFragment, "%smessage=%s",
			auth_server->authserv_msg_script_path_fragment,
			GATEWAY_MESSAGE_ACCOUNT_VALIDATION_FAILED
		);
		if(urlFragment!=NULL) {
		http_send_redirect_to_auth(r, urlFragment, "Redirect to failed validation message");
		free(urlFragment);
		}
	    break;
    case AUTH_SIGN_ERR:
		send_http_page(r, "Sign Error", "The Signature of Auth State is invalid, you can contact with UTT Technology.");
	    break;

    default:
		debug(LOG_WARNING, "I don't know what the validation code %d means for token %s from %s at %s - sending error message", auth_response.authcode, client->token, client->ip, client->mac);
		send_http_page(r, "Internal Error", "We can not validate your request at this time");
	    break;

	}
	}

	//UNLOCK_CLIENT_LIST();
	return;
}


