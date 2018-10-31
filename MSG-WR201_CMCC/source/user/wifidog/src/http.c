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

/* $Id: http.c,v 1.23.4.25 2016/09/01 04:58:59 cao.yongxiang Exp $ */
/** @file http.c
  @brief HTTP IO functions
  @author Copyright (C) 2004 Philippe April <papril777@yahoo.com>
  @author Copyright (C) 2007 Benoit Gr茅goire
  @author Copyright (C) 2007 David Bird <david@coova.com>

 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#include "httpd.h"

#include "safe.h"
#include "debug.h"
#include "conf.h"
#include "auth.h"
#include "firewall.h"
#include "http.h"
#include "httpd.h"
#include "client_list.h"
#include "common.h"
#include "centralserver.h"
#include "fw_iptables.h"

#include "free_hash.h"
#include "util.h"

#include "../config.h"
#include "loop_http.h"

#include "uttMachine.h"
#if (WIFIDOG_AUTH == FYES) || (WIFIDOG_MAKE_MONEY == FYES)
#include "typedef.h"
#include "mib.h"
#include "profacce.h"
#include "comExeFun.h"
#include "flash_api.h"
#endif
#if (SMS_DES_CRYPTO ==1)
#include "des.h"
#endif
#ifdef PARAM_SIGN
#include "signature.h"
extern pthread_mutex_t param_sign_mutex;
#endif
extern pthread_mutex_t	client_list_mutex;
#if (SWITCH_AUTH_MODE == FYES)
extern char lanMac[];
#endif

#if 0
/* checkExceptIpgroup
 * 检测请求客户端是否在web认证例外地址组
 *
 * 返回值：
 *	0:	不在例外地址组
 *	1:	在例外地址组
 * */
static int checkExceptIpgroup(const char *addr)
{
    debug(LOG_INFO,"%s-%d: check except ip group.\n",__func__,__LINE__);
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;
    int ret = 0;

    //ProfInit();
    profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
    /*web认证不开启不会用到计费模式返回0*/
    if((profG != NULL) && (ProfInstIsFree(profG) == 0) && (profG->enabled == 1))
    {
	debug(LOG_INFO,"%s-%d: exceptIpGrp: %s ===\n",__func__,__LINE__,profG->exceptIpGroup);
	if(profG->exceptIpGroup[0] != '\0') {
	    if(ipsetTestIpStr(profG->exceptIpGroup,addr) == 1) {
		ret = 1;
	    }
	}
    }
    //ProfDetach();
    debug(LOG_INFO,"%s-%d: ret %d ===\n",__func__,__LINE__,ret);
    return ret;
}
#endif
/** The 404 handler is also responsible for redirecting to the auth server */
void
http_callback_404(httpd *webserver, request *r)
{
	char		tmp_url[MAX_BUF];
	s_config	*config = config_get_config();
	t_auth_serv	*auth_server = get_auth_server();
	char		*mac = NULL;
	char            *url = NULL;
	int flag = 0;
	char timeS[TIME_STAMP_LEN];

	memset(tmp_url, 0, sizeof(tmp_url));
#if 0
	debug(LOG_INFO, "client addr: %s \n",r->clientAddr);
	if(checkExceptIpgroup(r->clientAddr)) {
	    debug(LOG_INFO,"client addr in except ip group, allowed.\n");
	    /*需要向客户端返回信息*/

	}
#endif
	if(r != NULL) {
	/* 
	 * XXX Note the code below assumes that the client's request is a plain
	 * http request to a standard port. At any rate, this handler is called only
	 * if the internet/auth server is down so it's not a huge loss, but still.
	 */
	mac = arp_get(r->clientAddr);
	    int ret = 0;
	    //LOCK_CLIENT_LIST();
	    ret = client_find_need_notify(r->clientAddr,mac);
	    //UNLOCK_CLIENT_LIST();
	    //debug(LOG_DEBUG, "get client need notify , ret =%d=====",ret);
        snprintf(tmp_url, (sizeof(tmp_url) - 1), "http://%s%s%s%s",
                        r->request.host,
                        r->request.path,
                        r->request.query[0] ? "?" : "",
                        r->request.query);
#if 1
	url = httpdUrlEncode(tmp_url);
#endif

	if (!is_online()) {
		/* The internet connection is down at the moment  - apologize and do not redirect anywhere */
#if 1
#else
		char * buf=NULL;
		safe_asprintf(&buf, 
			"<p>We apologize, but it seems that the internet connection that powers this hotspot is temporarily unavailable.</p>"
			"<p>If at all possible, please notify the owners of this hotspot that the internet connection is out of service.</p>"
			"<p>The maintainers of this network are aware of this disruption.  We hope that this situation will be resolved soon.</p>"
			"<p>In a while please <a href='%s'>click here</a> to try your request again.</p>", tmp_url);

		if(buf != NULL) {
                send_http_page(r, "Uh oh! Internet access unavailable!", buf);
		free(buf);
		}
#endif
		flag = 1;
		debug(LOG_INFO, "Sent %s an apology since I am not online - no point sending them to auth server", r->clientAddr);
	}
	else if (!is_auth_online()) {
		/* The auth server is down at the moment - apologize and do not redirect anywhere */
#if 0
		safe_asprintf(&buf, 
			"<p>We apologize, but it seems that we are currently unable to re-direct you to the login screen.</p>"
			"<p>The maintainers of this network are aware of this disruption.  We hope that this situation will be resolved soon.</p>"
			"<p>In a couple of minutes please <a href='%s'>click here</a> to try your request again.</p>", tmp_url);

                send_http_page(r, "Uh oh! Login screen unavailable!", buf);
		free(buf);
#else
		/*免认证处理*/
#if 1
#else
		char * buf=NULL;
		/*返回免认证页面*/
		safe_asprintf(&buf, "?deviceid=%u&ipaddr=%s",
			config->tmpSN,
			r->clientAddr);
		debug(LOG_INFO, "Captured ip %s mac %s requesting [%s] and re-directing them to login page", r->clientAddr, (mac!=NULL?mac:""), tmp_url);
		if(buf != NULL) {
		http_send_redirect_to_wifidog(r, buf, "Redirect to local login page");
		free(buf);
		}
#endif
#endif
		flag = 1;
		debug(LOG_INFO, "Sent %s an apology since auth server not online - no point sending them to auth server", r->clientAddr);
	}
	else {
	    /* 在认证有效期内被其他登陆设备替换掉的客户端
	     * 当访问网络是会被通知*/
	    if((mac!=NULL)&&(ret == 1)&&(strstr(tmp_url, "utt_wifi_auth=0")==NULL)) {
		char * buf=NULL;
		safe_asprintf(&buf, 
			"<p>您的账号已在别处登录，被迫下线！</p>");
		if(buf != NULL) {
		send_http_page(r, "退出认证!", buf);
		debug(LOG_DEBUG, "need notify client find, it will receive notice. ");
		debug(LOG_INFO, "Sent ip %s mac %s an Notice, because the mac is replaced.", r->clientAddr,mac);
		free(buf);
		}
	    } else {
#if 0
	    /* connections comes to max value - send tip message to client */
	    if(0) {
		char * buf;
		safe_asprintf(&buf, 
			"<p>Sorry,connextions come to the max value, so you will not allow to access to net.</p>"
			"<p>In a couple of minutes please <a href='%s'>click here</a> to try your request again.</p>", tmp_url);

                send_http_page(r, "Uh oh! Connection unavailable!", buf);
		free(buf);
		debug(LOG_INFO, "Sent %s an apology since it has no free connections. the max connections: %d, now connections: %d", r->clientAddr,config->conns,config->conns);
	    }
#endif

		/* Re-direct them to auth server */
		char *urlFragment=NULL;
		//ProfInit();
		char *macP = NULL;
		memset(timeS, 0, sizeof(timeS));
		getTimeStamp(timeS);
		if(mac!=NULL) {
		macP = convertMac(mac,0);
		}
#if (WIFIDOG_STDIF == FYES)
		if (!mac) {
		    /* We could not get their MAC address */
		    debug(LOG_INFO, "Failed to retrieve MAC address for ip %s, so not putting in the login request", r->clientAddr);
		    safe_asprintf(&urlFragment, "%sgw_address=%s&gw_port=%d&gw_id=%s&url=%s",
			    auth_server->authserv_login_script_path_fragment,
			    config->gw_address,
			    config->gw_port, 
			    config->gw_id,
			    url);
		} else {	    
		    debug(LOG_INFO, "Got client MAC address for ip %s: %s", r->clientAddr, mac);
		    safe_asprintf(&urlFragment, "%sgw_address=%s&gw_port=%d&gw_id=%s&mac=%s&url=%s",
			    auth_server->authserv_login_script_path_fragment,
			    config->gw_address,
			    config->gw_port, 
			    config->gw_id,
			    mac,
			    url);
		}
#else
#ifdef PARAM_SIGN
		char tmsStr[SIGN_PARAM_VALUE_LEN];
	LOCK_PARAM_SIGN();
		initParamval();
		snprintf(tmsStr,SIGN_PARAM_VALUE_LEN,"%u", config->tmpSN);
		addParamValue("deviceid", tmsStr);
		addParamValue("gwaddress", config->gw_address);
		snprintf(tmsStr,SIGN_PARAM_VALUE_LEN,"%d",config->gw_port);
		addParamValue("gwport", tmsStr);
		addParamValue("ip",r->clientAddr);
#if (SERVER_VERSION == FYES)
		addParamValue("v",SERV_VERSION);
#endif
		if(mac!=NULL) {
		    if(macP!=NULL) {
		    addParamValue("mac", macP);
		    } else {
		    addParamValue("mac", "");
		    }
		} else {
		    addParamValue("mac", "");
		}
		addParamValue("timestamp",timeS);
#if 1
		if(url!=NULL) {
		addParamValue("url",tmp_url);
		} else {
		addParamValue("url","");
		}
#endif
		//getSha1Result();
		unsigned char hexData1[41];
		getSha1Result(hexData1);
	UNLOCK_PARAM_SIGN();


		safe_asprintf(&urlFragment, "%ssign=%s%s&gwaddress=%s&gwport=%d&deviceid=%u&ip=%s&mac=%s&timestamp=%s&url=%s",
			auth_server->authserv_login_script_path_fragment,
			hexData1,
			AUTH_SERV_VER,
			config->gw_address,
			config->gw_port, 
			config->tmpSN,
			r->clientAddr,
			((macP!=NULL)?(macP):("")),
			timeS,
			(url!=NULL)?url:(""));
#else
		safe_asprintf(&urlFragment, "%sgwaddress=%s&gwport=%d&deviceid=%u&ip=%s&mac=%s&timestamp=%s&url=%s",
			auth_server->authserv_login_script_path_fragment,
			config->gw_address,
			config->gw_port, 
			config->tmpSN,
			r->clientAddr,
			((macP!=NULL)?(macP):("")),
			timeS,
			(url!=NULL)?url:(""));
#endif
#endif
		if(macP!=NULL) {
		    free(macP);
		}
		debug(LOG_INFO, "Captured ip %s mac %s requesting and re-directing them to login page", ((r!=NULL)?r->clientAddr:""), (mac!=NULL?mac:""));
		if(urlFragment != NULL) {
		http_send_redirect_to_auth(r, urlFragment, "Redirect to login page");
		free(urlFragment);
		//ProfDetach();
		}
	    }
	}
#if 1
	if(url!=NULL)
	free(url);
#endif
	if(flag == 1){
	    t_client *newclient=NULL;
	    t_client *findclient=NULL;
	    /*不返回免认证页面*/
#if 0
	    /*delete needless iptables rules*/
	    iptables_fw_add_del(IPTABLE_ADD);
#endif

	    if(mac!=NULL) {
		/* 避免相同ip不同mac的情况
		 * mantis 14784
		 * */
		if((findclient=client_list_find_by_ip(r->clientAddr))== NULL) {
	    LOCK_CLIENT_LIST();
		    newclient = client_list_append(r->clientAddr, mac, "utt_wifi_free_auth");
	    UNLOCK_CLIENT_LIST();
		}
#if 0
	    } else if(client_list_find_by_ip(r->clientAddr)==NULL) {
		newclient = client_list_append(r->clientAddr, "no mac", "utt_wifi_free_auth");
		/*getIfMac(char *ifname, char *if_hw)*/
	    } else {
#endif
	    if((findclient != NULL)&&(ret == 1)) {
		if (findclient->mac != NULL)
		    free(findclient->mac);
		findclient->mac = mac;
		findclient->fw_connection_state = FW_MARK_KNOWN;
		findclient->expire = config->freeAuth_time;
		findclient->free_auth_client = 1;
		findclient->need_notify = 0;
		ipsetAOrDIpStr(IPSET_WIFIDOG_FREE_AUTH, r->clientAddr, IPSET_MEM_ADD);
	    }
	    if(newclient != NULL) {
		newclient->fw_connection_state = FW_MARK_KNOWN;
		newclient->expire = config->freeAuth_time;
		newclient->free_auth_client = 1;
		ipsetAOrDIpStr(IPSET_WIFIDOG_FREE_AUTH, r->clientAddr, IPSET_MEM_ADD);
	    }
	    else {
		if((findclient == NULL)||((findclient!=NULL)&&(ret==0))) {
		    free(mac);
		}
	    }
	    /*write client list file*/
	    clist_list_write_file();
	    }
	}
	else {
	    free(mac);
	}
	}
}

void 
http_callback_wifidog(httpd *webserver, request *r)
{
	send_http_page(r, "WiFiDog", "Please use the menu to navigate the features of this WiFiDog installation.");
}

void 
http_callback_about(httpd *webserver, request *r)
{
	send_http_page(r, "About UTT WiFi", "This is UTT WiFi version <strong>" VERSION "</strong>");
}

void 
http_callback_status(httpd *webserver, request *r)
{
	const s_config *config = config_get_config();
	char * status = NULL;
	char *buf;

	if (config->httpdusername && 
			(strcmp(config->httpdusername, r->request.authUser) ||
			 strcmp(config->httpdpassword, r->request.authPassword))) {
		debug(LOG_INFO, "Status page requested, forcing authentication");
		httpdForceAuthenticate(r, config->httpdrealm);
		return;
	}

	status = get_status_text();
	safe_asprintf(&buf, "<pre>%s</pre>", status);
	send_http_page(r, "UTT WiFi Status", buf);
	free(buf);
	free(status);
}
/** @brief Convenience function to redirect the web browser to the auth server
 * @param r The request 
 * @param urlFragment The end of the auth server URL to redirect to (the part after path)
 * @param text The text to include in the redirect header ant the mnual redirect title */
void http_send_redirect_to_auth(request *r, char *urlFragment, char *text)
{
	char *protocol = NULL;
	int port = 80;
	t_auth_serv	*auth_server = get_auth_server();

	if((r!=NULL) && (urlFragment!=NULL)) {
	if (auth_server->authserv_use_ssl) {
		protocol = "https";
		port = auth_server->authserv_ssl_port;
	} else {
		protocol = "http";
		port = auth_server->authserv_http_port;
	}
			    		
	char *url = NULL;
	safe_asprintf(&url, "%s://%s:%d%s%s",
		protocol,
		auth_server->authserv_hostname,
		port,
		auth_server->authserv_path,
		urlFragment
	);
	if(url!=NULL) {
	http_send_redirect(r, url, text);
	free(url);
	}
	}
}

void http_send_redirect_to_wifidog(request *r, char *urlFragment, char *text)
{
	char *protocol = NULL;
	int port = 80;
	s_config	*config = config_get_config();
	t_auth_serv	*auth_server = get_auth_server();

	if((r!=NULL) && (urlFragment!=NULL) && (text!=NULL)) {
	if (auth_server->authserv_use_ssl) {
		protocol = "https";
		port = auth_server->authserv_ssl_port;
	} else {
		protocol = "http";
		port = auth_server->authserv_http_port;
	}
			    		
	char *url = NULL;
	safe_asprintf(&url, "http://%s:%d/%s%s", 
		config->gw_address, 
		config->gw_port, 
		"login.htm",
		urlFragment
	);
	if(url!=NULL) {
	http_send_redirect(r, url, text);
	free(url);
	}
	}
}
/** @brief Sends a redirect to the web browser 
 * @param r The request 
 * @param url The url to redirect to
 * @param text The text to include in the redirect header and the manual redirect link title.  NULL is acceptable */
void http_send_redirect(request *r, char *url, char *text)
{
		char *message = NULL;
		char *header = NULL;
		char *response = NULL;
		if((r!=NULL) && (url!=NULL)) {
							/* Re-direct them to auth server */
		debug(LOG_DEBUG, "Redirecting client browser to %s", url);
		safe_asprintf(&header, "Location: %s",
			url
		);
		if(text) {
			safe_asprintf(&response, "302 %s\n",
				text
			);	
		}
		else {
			safe_asprintf(&response, "302 %s\n",
				"Redirecting"
			);		
		}
		if((header != NULL) && (response != NULL)) {
		httpdSetResponse(r, response);
		httpdAddHeader(r, header);
		free(response);
		free(header);	
		}
		safe_asprintf(&message, "Please <a href='%s'>click here</a>.", url);
		if(message != NULL) {
		send_http_page(r, text ? text : "Redirection to message", message);
		free(message);
		}
		}
}

#if (SMS_DES_CRYPTO ==1)
extern void param_Des(const char *key, const unsigned char *data, char *enc_str)
{
    DES_key_schedule ks;
    DES_cblock kk;
    unsigned char inbuf[1024];
    unsigned char encbuf[1024];
    unsigned long bufsize;
    unsigned char iv[8];
    char  tmp[10];
    unsigned long i;

    /*这两行代码是必须的，否则加密结果可能不对*/
    memset(inbuf, 0, sizeof(inbuf));
    strcpy(inbuf, data);

    /*设置密文缓冲区大小*/
    bufsize = strlen(inbuf);
    debug(LOG_WARNING, "bufsize =%d",bufsize);
    if (bufsize % 8) {
	bufsize = (bufsize / 8) * 8 + 8;
    }
    debug(LOG_WARNING, "bufsize =%d",bufsize);

    /*设置密钥*/
    memset(&kk, 0, sizeof(kk));
    memset(&ks, 0, sizeof(ks));
    memset(&iv, 0, sizeof(iv));
    DES_string_to_key(key, &kk);
    DES_set_key_unchecked(&kk,&ks); 

    /*DES-CBC加密*/
    DES_ncbc_encrypt(inbuf,                 /*明文*/
	    encbuf,(long)bufsize, /*密文和密文缓冲大小*/
	    &ks,                  /*DES密钥*/
	    (DES_cblock *)iv,     /*iv值，加密后产生新的iv值*/
	    DES_ENCRYPT);         /*加密*/

    debug(LOG_WARNING, "bufsize =%d, strlen(encbuf) =%d, encbuf =%s",bufsize,strlen(encbuf),encbuf);
    memset(enc_str, 0, bufsize*2+1);
    for (i = 0; i < bufsize; i++) {
	sprintf(enc_str, "%s%02x", enc_str, encbuf[i]);
    }
    debug(LOG_WARNING, "bufsize =%d, strlen(enc_str) =%d, enc_str =%s",bufsize,strlen(enc_str),enc_str);

    //printf("des src: '%s' dst: '%s'\n", data, enc_str);

    return;
}
#endif
#if (SMS_ENABLE == 1)
void 
back_sms_result_to_web( request *r, char *buffer)
{
    char buf[MAX_BUF];
    struct 	tm *timePtr;
    time_t t;
    t = time(&t);
    timePtr = gmtime(&t);
    if(buffer!=NULL && r!=NULL) {
	snprintf(buf, (sizeof(buf) - 1),
		"HTTP/1.1 200 OK\nServer: ngx_openresty\nContent-Type: text/html;charset=utf-8\nConnection: close\n\n%s",
		buffer);   

		  //("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	send(r->clientSock, buf, strlen(buf), 0);
    }
}

void 
back_to_user_loginhtml( request *r, char *buffer)
{
    if(buffer != NULL) 
    {
		
	httpdSetResponse(r, "200 OK\n");
	httpdSetContentType(r, "text/html;charset=utf-8");
	//httpdSetContentType(r, "text/html;charset=utf-8\nContent-Encoding: gzip");
	//r->response.headersSent = 1;
	//httpdAddHeader(r, "Content-Type: text/plain; charset=utf-8\n");
	//httpdAddHeader(r, "\n");
	//strcpy(r->response.headers,
	//	" UTT Wifi Server.\n"); 
	httpdOutput(r, buffer);
	//httpdOutput(r, buffer);
    }
}

int 
getAddrePhone(char *addrePhone)
{
    int ret = 0;
    if(addrePhone != NULL) 
    {
	MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
	WebAuthGlobalProfile *profG = NULL;
	//ProfInit();
	profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
	/*web认证不开启不会用到计费模式返回0*/
#if 0
	if((profG != NULL) && (ProfInstIsFree(profG) == 0)&&(strlen(profG->sms_number) > 0))
	{
	    snprintf(addrePhone,15,"%s",profG->sms_number);
	    ret = 1;
	}
#else
	if((profG != NULL) && (ProfInstIsFree(profG) == 0)&&(strlen(profG->addrPhone) > 0))
	{
	    snprintf(addrePhone,15,"%s",profG->addrPhone);
	    ret = 1;
	}
#endif
	else{
	    //snprintf(addrePhone,15,"%s","+8613010314500");
	    ret = 0;
	}
    }
    return ret;
}

void 
http_callback_phoneRegin(httpd *webserver, request *r)
{
	httpVar * phone;
	int ret = 0;
	char retStr[100];
	char encode[300];
	char addrP[15];
	int circle = 0;
	int null_item = 0;
    //ret = string_unicode("13800100500", "13007110996", 1, "1405", encode);
    //printf("ret = %d, encode =%s\n",ret,encode);

	if ((phone = httpdGetVariableByName(r, "phone"))) {
	    debug(LOG_DEBUG,"get phone %s",phone->value);
#if 0
/*for debug */
//back_to_user_loginhtml( r,"{\"state\":\"1\",\"message\":\"1234567890\"}\n");
back_sms_result_to_web( r, "{\"state\":\"2\",\"message\":\"1234567890\"}");
return;
#endif
	    ret = authserver_smsOperate(phone->value, 1, retStr);
	    debug(LOG_DEBUG, "phoneRegin return val %d, return string %s",ret,retStr);
	    if(ret == 1) {	    /*success*/
		/*output null*/
		ret = 0;
		memset(addrP, 0, sizeof(addrP));
		if(getAddrePhone(addrP) == 1) 
		{
back_sms_result_to_web( r, "{\"state\":\"1\",\"message\":\"\"}");
		    memset(encode,0, sizeof(encode));
		    ret = string_unicode(addrP, phone->value, 1, retStr, encode);
		    if(ret > 0) 
		    {
			circle = 0;
			while(1) {
			    if(circle >=SMS_LIST_LEN) 
				circle = 0U;
			    if(nvramProfile->smslist.smsnode[circle].flag == 1) 
			    {
				null_item++;
			    } else {
				snprintf(nvramProfile->smslist.smsnode[circle].phone, MAX_PHONE_LEN,"%s",phone->value);
				nvramProfile->smslist.smsnode[circle].len = ret;
				snprintf(nvramProfile->smslist.smsnode[circle].data, MAX_SMS_DATA_LEN, "%s", encode);
				nvramProfile->smslist.smsnode[circle].flag = 1;
				debug(LOG_DEBUG, "add phone %s, msg encode %s",nvramProfile->smslist.smsnode[circle].phone,
					nvramProfile->smslist.smsnode[circle].data);
				break;
			    }
			    if(null_item>SMS_LIST_LEN) {
				snprintf(retStr, 100, "%s","{\"state\":\"2\",\"message\":\"获取密码失败!\"}");
				debug(LOG_ERR, "[SMS] add phone %s msg encode error, because sms send list is full. ",phone->value);
				//back_sms_result_to_web(r, retStr);
				break;
			    }
			    circle++;
			}
		    } else {
			debug(LOG_ERR, "[SMS] msg encode return error.");
		    }
		} else {
		    debug(LOG_ERR, "[SMS] getAddrePhone error.");
		}
	    } else if(ret == 2) {   /*return error msg*/
		back_sms_result_to_web(r, retStr);
	    
	    } else {		    /*default return error msg*/
		snprintf(retStr, 100, "%s","{\"state\":\"2\",\"message\":\"获取密码失败!\"}");
		back_sms_result_to_web(r, retStr);
	    }
	} else {
	    //back_to_user_loginhtml(r, "{\"state\":\"2\",\"message\":\"手机号不能为空！\"}");
	    //back_to_user_loginhtml(r, "{\"state\":\"2\",\"message\":\"abcd\"}");
	    back_sms_result_to_web( r, "{\"state\":\"2\",\"message\":\"no phone\"}");
	    debug(LOG_WARNING,"[SMS] No Phone form Web Page.");
	}
}

void 
http_callback_forgotPasswd(httpd *webserver, request *r)
{
	httpVar * phone;
	int ret = 0;
	char retStr[100];
	char encode[300];
	char addrP[15];
	int circle = 0;
	int null_item = 0;

	if ((phone = httpdGetVariableByName(r, "phone"))) {
	    debug(LOG_DEBUG,"get phone %s",phone->value);
	    ret = authserver_smsOperate(phone->value, 2, retStr);
	    debug(LOG_DEBUG, "forgotPasswd return val %d, return string %s",ret,retStr);
	    if(ret == 1) {	    /*success*/
		/*output null*/
		ret = 0;
		memset(addrP, 0, sizeof(addrP));
		if(getAddrePhone(addrP) == 1) 
		{
		    ret = string_unicode(addrP, phone->value, 1, retStr, encode);
		    if(ret > 0) 
		    {
			circle = 0;
			while(1) {
			    if(circle >=SMS_LIST_LEN) 
				circle = 0U;
			    if(nvramProfile->smslist.smsnode[circle].flag == 1) 
			    {
				null_item++;
			    } else {
				snprintf(nvramProfile->smslist.smsnode[circle].phone, MAX_PHONE_LEN,"%s",phone->value);
				nvramProfile->smslist.smsnode[circle].len = ret;
				snprintf(nvramProfile->smslist.smsnode[circle].data, MAX_SMS_DATA_LEN, "%s", encode);
				nvramProfile->smslist.smsnode[circle].flag = 1;
				debug(LOG_DEBUG, "add phone %s, msg encode %s",nvramProfile->smslist.smsnode[circle].phone,
					nvramProfile->smslist.smsnode[circle].data);
				break;
			    }
			    if(null_item>SMS_LIST_LEN) {
				snprintf(retStr, 100, "%s","{\"state\":\"2\",\"message\":\"获取密码失败!\"}");
				debug(LOG_WARNING, "[SMS] add phone %s msg encode error, because sms send list is full. ",phone->value);
				back_sms_result_to_web(r, retStr);
				break;
			    }
			    circle++;
			}
		    }
		} else {
		    debug(LOG_ERR, "[SMS] getAddrePhone error.");
		}
	    } else if(ret == 2) {   /*return error msg*/
		back_sms_result_to_web(r, retStr);
	    
	    } else {		    /*default return error msg*/
		snprintf(retStr, 100, "%s","{\"state\":\"2\",\"message\":\"获取密码失败!\"}");
		back_sms_result_to_web(r, retStr);
	    }
	} else {
	    back_sms_result_to_web(r, "{\"state\":\"2\",\"message\":\"手机号不能为空！\"}");
	    debug(LOG_WARNING,"[SMS] No Phone form Web Page.");
	}
}
#endif

#if (WIFIDOG_MAKE_MONEY == FYES)
/*
int isMacInFreeList(char *mac)
{
    int i=0;
    int ret=0;
    time_t t;
    int period;
//    char mac12[18];
    MakeMoneyProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_MAKE_MONEY;
    printf("%s-%d\n",__func__,__LINE__);
    prof=(MakeMoneyProfile *)ProfGetInstPointByIndex(mibType,0);
    if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof))) {
	for(i=0;i<256;i++)
	{
	    if(strcmp(converMac6To17Byte(prof->freeExpList[i].mac),"")==0)
	    {
		break;
	    }
	    if(strcmp(converMac6To17Byte(prof->freeExpList[i].mac),mac)==0) 
	    {
		ret=1;
		t=time(NULL);
		period = prof->FreeTime*60-(t-prof->freeExpList[i].time); 
		break;
	    }
	}
	if(ret == 0){
	    t=time(NULL);
	    period=prof->FreeTime*60;
	    prof->freeExpList[i].time=t;
//	    strcpy(mac12,mac);
	    strRemoveChar(mac, ':');
	    converMac12To6Byte(mac,&(prof->freeExpList[i].mac));
//	    insert(prof->freeExpList[i]);
	}
    }
    printf("%s-%d\n",__func__,__LINE__);
    
    return period;
    
}
*/

void http_salewifi_getrest(httpd *webserver, request *r)
{
    char rest[40]={0};
    t_client	*client;

    LOCK_CLIENT_LIST();
    if ((client = client_list_find_by_ip(r->clientAddr)) == NULL) { 
	sprintf(rest,"var expire=0;var runtime=0;");
    }
    else
    { 
	sprintf(rest,"var expire=%d;var runtime=%d;",client->expire,client->runtime);
    }
    UNLOCK_CLIENT_LIST();
    uttHttpdOutNoCache(r, rest);
    return;
}
/*导入配置参数xrg*/
void http_salewifi_getParam(httpd *webserver,request *r)
{
    char parm[512]={0};
    MakeMoneyProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_MAKE_MONEY;
    uttSemP(SEM_SALE_WIFI,0);
    prof = (MakeMoneyProfile *)ProfGetInstPointByIndex(mibType,0);
     if((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)))
    {	
	 sprintf(parm,"var timestarts = \"%s\";var timestops = \"%s\";var charge = \"%s\";",prof->TimeStart,prof->TimeStop,prof->charge);
    }
    uttSemV(SEM_SALE_WIFI,0);
    httpdOutput(r,parm);
    return ;
}
#if 0
/*保存配置参数没有使用*/
void http_salewifi_savePhone(httpd *webserver,request *r)
{
    printf("http_salewifi_savephone\n");
    httpVar *phone;
    MakeMoneyProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_MAKE_MONEY;
    prof = (MakeMoneyProfile *)ProfGetInstPointByIndex(mibType,0);
    phone = httpdGetVariableByName(r,"tel");
    printf("phone->value = %s\n",phone->value);
    if(phone->value[0] == '\0')
    {
	return;
    }
    if((NULL != prof)&&(0 == ProfInstIsFree((Pointer)prof)))
    {
	if(strncmp(prof->phoneNumber,phone->value,11)!=0)
	{
	    strcpy(prof->phoneNumber,phone->value);
	    prof->isBind = 1;
	    nvramWriteCommit();
	}
    
    }
    return;
}
#endif
/*赚钱wifi免费体验*/
void 
http_salewifi_experience(httpd *webserver, request *r)
{
    t_client	*client;
    char *mac;
    char macstr[18]={0};
    char url[70]={0};
    char param[600]={0};
    int period;
    MacAddr mac6;
printf("r->request.query = %s\n",r->request.query );
    if (!(mac = arp_get(r->clientAddr))) {
    /* We could not get their MAC address */
    debug(LOG_ERR, "Failed to retrieve MAC address for ip %s", r->clientAddr);
    send_http_page(r, "WiFiDog Error", "Failed to retrieve your MAC address");
    }
    else
    {
	strcpy(macstr,mac);
	
	strRemoveChar(macstr, ':');
	converMac12To6Byte(macstr,&mac6);
		printf("%d,%s-%s\n",__LINE__,macstr,converMac6To17Byte(mac6));
//	period=isMacInFreeList(macstr);
//	period=search(mac6,0);
	period=store_array_hash(mac6);/*查存储hash表mac地址是否已体验过，存在返回剩余时间值，若不存在返回设置的体验时间值*/

	printf("%s-%d,period=%d\n",__func__,__LINE__,period);
	if(period<=0)
	{
	    sprintf(param,"http://biz.greenwifi.com.cn/login.htm%s&fail=1",r->request.query);
//	    http_send_redirect(r, "http://biz.greenwifi.com.cn/login.htm?fail=1", "Redirect to local login page");
	    http_send_redirect(r, param, "Redirect to local login page");
	    return;
	}

	LOCK_CLIENT_LIST();
	if ((client = client_list_find_by_ip(r->clientAddr)) == NULL) { 
	    client= client_list_append(r->clientAddr, mac, "aaa");

	} 
	else { 
	    if (client->mac != NULL) 
		free(client->mac); 
	    client->mac = mac; 
#if (SWITCH_AUTH_MODE == FYES) 
	    if(strncmp(client->mac, lanMac, 17) == 0) { 
		client->authType = WIFIDOG_CLIENT_AUTH_TYPE_SWITCH_NO_MAC_AUTH; 
	    } 
#endif 
	    if (client->token != NULL) 
		free(client->token); 
	    client->token = safe_strdup("aaa"); 
	}

	client->fw_connection_state = FW_MARK_KNOWN; 
	client->expire = period; 
	client->need_notify = 0; 
	client->runtime = 0;
	fw_deny(client->ip, client->mac, FW_MARK_KNOWN); /*防止用户认证成功后，重复认证添加规则*/

#if (SWITCH_AUTH_MODE == FYES) 
    if( client->authType == WIFIDOG_CLIENT_AUTH_TYPE_SWITCH_NO_MAC_AUTH ) { 
	ipsetAOrDIpStr(IPSET_GROUP_SWITCH_CLIENT_LIST, client->ip, IPSET_MEM_ADD); 

    } else 
#endif 
    { 
	fw_allow(client->ip, client->mac, FW_MARK_KNOWN); 
    }

    /*write client list file*/ 
    clist_list_write_file();
    UNLOCK_CLIENT_LIST();

//    served_this_session++;
    }
    sprintf(url,"http://biz.greenwifi.com.cn/success.htm?rest=%d&fail=0",period);
 //   httpdOutput(r, param);
    http_send_redirect(r, url, "Redirect to local login page");
    return;
}
#endif
void 
http_callback_auth(httpd *webserver, request *r)
{
	t_client	*client;
	httpVar * token;
#ifdef PARAM_SIGN
	httpVar * sign;
	httpVar * timeR;
	int result = 0;
#endif
	char	*mac;
	//httpVar *logout = httpdGetVariableByName(r, "logout");
	httpVar *logout = NULL;
	if ((token = httpdGetVariableByName(r, "token"))) {
		/* They supplied variable "token" */
		if (!(mac = arp_get(r->clientAddr))) {
			/* We could not get their MAC address */
			debug(LOG_ERR, "Failed to retrieve MAC address for ip %s", r->clientAddr);
			send_http_page(r, "WiFiDog Error", "Failed to retrieve your MAC address");
		} else {
			/* We have their MAC address */
#ifdef PARAM_SIGN
	    if ((sign = httpdGetVariableByName(r, "sign"))) {
		if ((timeR = httpdGetVariableByName(r, "timestamp"))) {
		    debug(LOG_DEBUG, "sign: %s, token: %s, timestamp: %s",sign->value,token->value,timeR->value);
	LOCK_PARAM_SIGN();
		    initParamval();
		    addParamValue("token", (token->value));
		    addParamValue("timestamp", (timeR->value));
		    //getSha1Result();
		    unsigned char hexData1[41];
		    getSha1Result(hexData1);
	UNLOCK_PARAM_SIGN();
		    if(memcmp((sign->value), hexData1, 40)==0)
		    {
			result = 1;
		    } else {
			debug(LOG_ERR,"[SIGN] Token signature check error.");
			/* They did not supply variable "token" */
			send_http_page(r, "UTT WiFi error", "Token Digital Signature Error, maybe returned from other Server.");
		    }
		} else {
		    /* They did not supply variable "token" */
		    send_http_page(r, "UTT WiFi error", "Token Invalid timestamp, 'timestamp' returned from UTT WiFi Server.");
		}
	    } else {
		/* They did not supply variable "token" */
		send_http_page(r, "UTT WiFi error", "Token Invalid sign, 'sign' returned from UTT WiFi Server.");
	    }
	    if(result != 1) {
		return;
	    }
#endif

			LOCK_CLIENT_LIST();
			
			/* 避免相同ip不同mac的情况
			 * mantis 14784
			 * */
			if ((client = client_list_find_by_ip(r->clientAddr)) == NULL) {
				debug(LOG_DEBUG, "New client for %s", r->clientAddr);
				client_list_append(r->clientAddr, mac, token->value);
#if 0 /*no logout*/
			} else if (logout) {
			    t_authresponse  authresponse;
			    s_config *config = config_get_config();
			    unsigned long long incoming = client->counters.incoming;
			    unsigned long long outgoing = client->counters.outgoing;
			    char *ip = safe_strdup(client->ip);
			    char *urlFragment = NULL;
			    t_auth_serv	*auth_server = get_auth_server();
			    				    	
			    fw_deny(client->ip, client->mac, client->fw_connection_state);
			    client_list_delete(client);
			    debug(LOG_DEBUG, "Got logout from %s", client->ip);
			    
			    /* Advertise the logout if we have an auth server */
			    if (config->auth_servers != NULL) {
					UNLOCK_CLIENT_LIST();
					auth_server_request(&authresponse, REQUEST_TYPE_LOGOUT, ip, mac, token->value, 
									    incoming, outgoing);
					LOCK_CLIENT_LIST();
					
					/* Re-direct them to auth server */
					debug(LOG_INFO, "Got manual logout from client ip %s, mac %s, token %s"
					"- redirecting them to logout message", client->ip, client->mac, client->token);
					safe_asprintf(&urlFragment, "%smessage=%s",
						auth_server->authserv_msg_script_path_fragment,
						GATEWAY_MESSAGE_ACCOUNT_LOGGED_OUT
					);
					http_send_redirect_to_auth(r, urlFragment, "Redirect to logout message");
					free(urlFragment);
			    }
			    free(ip);
#endif 
 			} 
 			else {
				//debug(LOG_DEBUG, "Client for %s is already in the client list", client->ip);
				if (client->mac != NULL)
				    free(client->mac);
				client->mac = mac;
#if (SWITCH_AUTH_MODE == FYES)
				if(strncmp(client->mac, lanMac, 17) == 0) {
				    client->authType = WIFIDOG_CLIENT_AUTH_TYPE_SWITCH_NO_MAC_AUTH;
				}
#endif
				if (client->token != NULL)
				    free(client->token);
				client->token = safe_strdup(token->value);
			}
			UNLOCK_CLIENT_LIST();
			if (!logout) {
				authenticate_client(r);
			}
			/*write client list file*/
			clist_list_write_file();
			//free(mac);
		}
	} else {
		/* They did not supply variable "token" */
		send_http_page(r, "UTT WiFi error", "Invalid token");
	}
}

#if 0
/*del needless functions*/
void 
http_callback_client(httpd *webserver, request *r)
{
    char buf[] = "ipaddr[0]=\"192.168.1.2\";mac[0]=\"00:22:33:ed:45:4f\";totaltime[0]=10;usedtime[0]=5;totalrecs=1;";
    fprintf(stderr, "get client.\n");
    _httpd_net_write( r->clientSock, buf, strlen(buf));
    //httpdPrintf(r,"ipaddr[0]=%s;mac[0]=%s;totaltime[0]=10;usedtime[0]=5;totalrecs=1;","192.168.1.2","00:22:33:ed:45:4f");
}

void
http_callback_localAuth(httpd *webserver, request *r)
{
    fprintf(stderr,"=================\n");
    fprintf(stderr,"wifidog Free Auth.\n");
    fprintf(stderr,"=================\n");
    /*allow user link to net*/

    /*return success.htm*/
}
#endif

void send_http_page(request *r, const char *title, const char* message)
{
    s_config	*config = config_get_config();
    char *buffer =NULL;
    struct stat stat_info;
    int fd;
    ssize_t written;

    fd=open(config->htmlmsgfile, O_RDONLY);
    if (fd==-1) {
        debug(LOG_CRIT, "Failed to open HTML message file %s: %s", config->htmlmsgfile, strerror(errno));
        return;
    }

    if (fstat(fd, &stat_info)==-1) {
        debug(LOG_CRIT, "Failed to stat HTML message file: %s", strerror(errno));
        close(fd);
        return;
    }

    buffer=(char*)safe_malloc(stat_info.st_size+1);
    if(buffer!=NULL) {
    written=read(fd, buffer, stat_info.st_size);
    if (written==-1) {
        debug(LOG_CRIT, "Failed to read HTML message file: %s", strerror(errno));
        free(buffer);
        close(fd);
        return;
    }
    close(fd);

    buffer[written]=0;
    httpdAddVariable(r, "title", title);
    httpdAddVariable(r, "message", message);
    httpdAddVariable(r, "nodeID", config->gw_id);
    httpdOutput(r, buffer);
    free(buffer);
    }
}
typedef enum {
    PARAM_TYPE_INT,		/* int */
    PARAM_TYPE_UINT,		/* unsigned int */
    PARAM_TYPE_CHAR,		/* char */
    PARAM_TYPE_STRING,		/* char[] */
} ParamDateType;
/*
 * getParemValue
 * 参数说明:
 *	argv	    -	传输的所有参数字符串
 *	name	    -   需要获取的参数名
 *	value	    -	返回的参数值(可能是数字或者字符串)
 *	len	    -	value 的最大长度，防止越界
 *	datatype    -	value的数据类型，需要将argv中的字符串转成对应的数据类型保存到value中
 *
 *  argv中可能包含多个参数,argv的内容如下：
 *	param1:value1;param2:value2;param3:value3;...
 *  返回值:
 *	0   -	错误
 *	1   -	正确
 * */
int getParemValue(char *argv, char *name, void *value, int len, ParamDateType datatype)
{ 
    int ret=0;
    char *par_pos;

    char *value_pos;
    char *par_pos_end;
    char value_tmp[50];

    if (argv != NULL){
	par_pos = strstr(argv,name);
	if(par_pos != NULL){
	    par_pos_end = strstr(par_pos,";");
	    if(par_pos_end == NULL)
		par_pos_end = par_pos + strlen(par_pos); 
	    value_pos = strstr(par_pos,":");
	    if(par_pos_end != NULL && value_pos != NULL){
		snprintf(value_tmp,par_pos_end-value_pos,"%s",value_pos+1);
		switch(datatype)
		{
		    case PARAM_TYPE_STRING :
			snprintf(value,len,"%s",value_tmp);	
			ret = 1;
			break;
		    case PARAM_TYPE_UINT :
			sscanf(value_tmp,"%u",value);	
			ret = 1;
			break;

		    case PARAM_TYPE_INT :
			sscanf(value_tmp,"%d",value);	
			ret = 1;
			break;
		    case PARAM_TYPE_CHAR :
			sscanf(value_tmp,"%u",value);	
			ret = 1;
			break;
		    default :
			ret = 0;
			break;
		}
	    }	
	}	
    }
    return ret;
}
#if (WIFIDOG_NOTICE == FYES) /*到期推送URL*/
/*
 * wifidog_notice_operate
 * http回调函数处理http的cmd请求
 * 参数：
 *	argv_value	-   cmd请求对应的参数
 * */
int wifidog_notice_operate(char *argv_value)
{
    s_config	*config = config_get_config();
    int ret = 0;
    int flag = 1;
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;

    
    ret = getParemValue(argv_value, "time", &(config->noticeTime), sizeof(config->noticeTime), PARAM_TYPE_UINT);
    if(ret == 0) {
	flag = 0;
	debug(LOG_ERR, "[NOTICE] get notice time error.");
    }
    ret = getParemValue(argv_value, "url", config->noticeUrl, sizeof(config->noticeUrl), PARAM_TYPE_STRING);
    if(ret == 0) {
	flag = 0;
	debug(LOG_ERR, "[NOTICE] get notice time error.");
    }
    if(flag == 1 && urlCheck(config->noticeUrl,sizeof(config->noticeUrl)) ) 
    {
	profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
	if((ProfInstIsFree(profG) == 0)){
	     strcpy(profG->wifiData.noticeUrl, config->noticeUrl);
	}
	nvramWriteCommit(); /*save*/

	wifi_Notice_Handle(IPTABLE_DEL);
	wifi_Notice_Handle(IPTABLE_ADD);
    }
    return flag;
}
#endif

#if (UTT_KNOWIFI == FYES)
static int addDev(char * argv)
{
    MibProfileType mibType = MIB_PROF_KNOWIFI_DOMAIN;
    KnowifiDomainProfile *prof = NULL;
    char addMac[18];
    char *addIp;
    char cmd[255];
    char searchKey[32];
    freeDomain *freedomain;

    int macNum = 0;
    int isTimeMode = 0;
    int i = 0;
    int ret1 = getParemValue(argv, "macNum", &macNum, sizeof(int), PARAM_TYPE_INT);
    if (ret1 == 1)
    {
	for(i = 0; i < macNum; i++)
	{
	    memset(addMac, 0, sizeof(addMac));
	    sprintf(searchKey, "mac%d", i);
	    int ret = getParemValue(argv, searchKey, addMac, sizeof(addMac), PARAM_TYPE_STRING);
	    if(ret)
	    {
		prof = (KnowifiDomainProfile*)ProfGetInstPointByName(mibType, addMac);
		if (prof)
		{

		    fw_deny(prof->ip, prof->mac, FW_MARK_KNOWN);	
		    prof->isFree = 0;
		    //printf("mac : %s, ip : %s\n", prof->mac, prof->ip);
		    addIp = arp_get_by_mac(prof->mac);
		    if (addIp)
		    {
			memset(cmd, 0, sizeof(cmd));
			sprintf(cmd, "ipset -D %s %s,%s", KNOWIFI_REDIRECT_CLIENT, prof->ip, prof->mac);
			execute(cmd, 0);

			strncpy(prof->ip, addIp, sizeof(prof->ip));

			memset(cmd, 0, sizeof(cmd));
			sprintf(cmd, "ipset -A %s %s,%s", KNOWIFI_REDIRECT_CLIENT, prof->ip, prof->mac);
			execute(cmd, 0);

			freedomain = freeDomainFind(prof->domain);
			if (freedomain)
			{
			    //printf("ip : %s, free,statu : %d, mode :%d\n",prof->ip, prof->isFree, prof->mode);
			    if (prof->mode == KNOWIFI_MODE_MISSION)
			    {
				prof->isFree = 1;
				fw_allow(prof->ip, prof->mac, FW_MARK_KNOWN);
			    }
			    else
			    {
				freeDomainDel(prof->domain);
			    }
			}
			free(addIp);
		    }
		}
	    }
	}
    }

    knowifi_time_check();
    
    return 1;
}
static int allowByDomain(char * argv)
{
    char domain[128] = {0};
    int freeTime = 0;
    char *ip = NULL;
    MibProfileType mibType = MIB_PROF_KNOWIFI_DOMAIN;
    KnowifiDomainProfile *prof = NULL;
    int idx = 0, max = 0, min = 0;
    

    int ret1 = getParemValue(argv, "domain", domain, sizeof(domain), PARAM_TYPE_STRING);
    int ret2 = getParemValue(argv, "freetime", &freeTime, sizeof(int), PARAM_TYPE_INT);
    if (ret1 == 1 && ret2 == 1)
    {
	freeDomainEdit(domain, freeTime);
	ProfInstLowHigh(mibType, &max, &min);
	for (idx = min; idx < max; idx++)
	{
	    prof = (KnowifiDomainProfile*)ProfGetInstPointByIndex(mibType, idx);
	    if (prof != NULL && prof->head.active == TRUE)
	    {
		if (strcmp(prof->domain, domain) == 0)
		{
		    ip = arp_get_by_mac(prof->mac);
		    fw_deny(prof->ip, prof->mac, FW_MARK_KNOWN);	
		    prof->isFree = 0;
		    if(ip)
		    {
			fw_allow(ip, prof->mac, FW_MARK_KNOWN);
			prof->isFree = 1;
			free(ip);
		    }
		}
	    }
	}
    }
    return 1;
}
static int logoutByDomain(char * argv)
{
    char domain[128] = {0};
#if 0
    MibProfileType mibType = MIB_PROF_KNOWIFI_DOMAIN;
    KnowifiDomainProfile *prof = NULL;
    int i = 0, max = 0, min = 0;
#endif

    int ret = getParemValue(argv, "domain", domain, sizeof(domain), PARAM_TYPE_STRING);
    freeDomainDel(domain);
#if 0
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++)
    {
        prof = (KnowifiDomainProfile*)ProfGetInstPointByIndex(mibType, i);
        if (prof != NULL && prof->head.active == TRUE)
        {
            if (prof->domain && strncmp(prof->domain, domain, 128) == 0)
            {
		fw_deny(prof->ip, prof->mac, FW_MARK_KNOWN); 
		prof->isFree = 0;
            }
        }
    }
#endif
    return 1;
}
static int delByDomain(char * argv)
{
    char domain[128] = {0};
    MibProfileType mibType = MIB_PROF_KNOWIFI_DOMAIN;
    KnowifiDomainProfile *prof = NULL;
    int i = 0, max = 0, min = 0;
    char macTmp[18];
    char cmd[255];

    int ret = getParemValue(argv, "domain", domain, sizeof(domain), PARAM_TYPE_STRING);
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++)
    {
        prof = (KnowifiDomainProfile*)ProfGetInstPointByIndex(mibType, i);
        if (prof != NULL && prof->head.active == TRUE)
        {
            if (prof->domain && strncmp(prof->domain, domain, 128) == 0)
            {
		fw_deny(prof->ip, prof->mac, FW_MARK_KNOWN); 
		memset(cmd, 0, sizeof(cmd));
		sprintf(cmd, "ipset -D %s %s", KNOWIFI_REDIRECT_CLIENT, prof->ip);
		execute(cmd, 0);
		ProfDelInstByIndex(mibType, i);
            }
        }
    }
    freeDomainDel(domain);
    return 1;
}
#endif

#if (WIFIDOG_REMOTE_LOGOUT == FYES) /*远程挂断*/
static int logoutByMAC(char * argv)
{
    char tmp_argv[255];
    char *macList;
    int ret = 0;
    ret = getParemValue(argv, "mac", tmp_argv, sizeof(tmp_argv), PARAM_TYPE_STRING);
    if(ret == 1){
	macList  = convertMac (tmp_argv,1);
	debug(LOG_INFO, "LOGOUT MAC : [%s].",macList);
	ret = cli_del_by_mac(macList);
	free(macList);
    }
    return ret;
}
#endif
#if (WIFIDOG_USER_SUCCESS_URL == FYES) /*认证成功URL*/
static int setUserUrl(char * argv)
{
    s_config	*config = config_get_config();
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;
    char tmp_url[MAX_WIFIDOG_URL_LEN + 1];
    char tmp_argv[MAX_WIFIDOG_URL_LEN + 1];
    char *http_pos;
    int check_resutl;
    int ret = 0;
    ret = getParemValue(argv, "url", tmp_argv, sizeof(tmp_argv), PARAM_TYPE_STRING);
    if(ret == 1){
	debug(LOG_DEBUG, "SUCCESS URL : [%s].",tmp_argv);
	if(strlen(tmp_argv) > 0 ){
	    check_resutl = urlCheck(tmp_argv,sizeof(tmp_argv));
	    if(check_resutl == 1){
		strcpy(tmp_url,tmp_argv);
	    }else if (check_resutl == 2){
		strcpy(tmp_url,"http://");
		strcat(tmp_url,tmp_argv);
	    }
	}else{
	    strcpy(tmp_url,"");
	}
	profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
	if((ProfInstIsFree(profG) == 0))
	{
	    strcpy(profG->wifiData.successUrl,tmp_url);
	    if(config->successUrl)
	    {
		free(config->successUrl);
		config->successUrl = NULL ;
	    }

	    if(strlen(profG->wifiData.successUrl) > 0) 
	    {
		config->successUrl = safe_strdup(profG->wifiData.successUrl);
	    }
	    ret = 1;
	    nvramWriteCommit(); /*save*/
	}
    }else{

	debug(LOG_ERR, "SUCCESS URL : GET URL ERROR.");
    }
    return ret; 
}
#endif

#if (WIFIDOG_MAC_AUTH == FYES)
static int wifidog_macAuthEn(char * argv)
{
    s_config	*config = config_get_config();
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;
    int ret = 0;
    ret = getParemValue(argv, "enable", &config->macAuthEn, sizeof(config->macAuthEn), PARAM_TYPE_UINT);
    if(ret == 0) {
	debug(LOG_ERR, "[macAuthEn] get macAuthEn  error.");
    }
    profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
    if((ProfInstIsFree(profG) == 0))
    {
	profG->wifiData.macAuthEn = config->macAuthEn;
	ret = 1;
    }

    return ret;
}
#endif


void 
http_callback_operation(httpd *webserver, request *r)
{
    t_client	*client;
    httpVar * cmd;
    httpVar * argv;
    httpVar * identify;
    char *result;
    int ret = 0;
#if (WIFIDOG_NOTICE == FYES) /*到期推送URL*/
    int time = 0;
    char noticeUrl[MAX_URL_LEN+1];
#endif
#if 1
    if(!isLocalIP(r->clientAddr))
    {
	httpdOutput(r,"{\"state\":\"0\"}");
	return; 	
    }
#endif 
    if ((identify = httpdGetVariableByName(r, "identify"))) {
	if ((cmd = httpdGetVariableByName(r, "cmd"))) {
	    if ((argv = httpdGetVariableByName(r, "argv"))) {
		debug(LOG_INFO, "request identify : %s ;cmd : %s ;argv :%s\n",identify->value,cmd->value,argv->value);
		if(strcmp(identify->value,"TR069") == 0){
		    if(0){
		    }
#if (UTT_KNOWIFI == FYES)
		    else if(strcmp(cmd->value,"addDev") == 0){
			ret = addDev(argv->value);
			if(ret == 0) {
			    debug(LOG_ERR, "[KNOWIFI UP] http callback cmd operation error, cmd %s argv %s.",cmd->value,argv->value);
			}
		    }
		    else if(strcmp(cmd->value,"upDomain") == 0){
			ret = allowByDomain(argv->value);
			if(ret == 0) {
			    debug(LOG_ERR, "[KNOWIFI UP] http callback cmd operation error, cmd %s argv %s.",cmd->value,argv->value);
			}
		    }
		    else if(strcmp(cmd->value,"downDomain") == 0){
			ret = logoutByDomain(argv->value);
			if(ret == 0) {
			    debug(LOG_ERR, "[KNOWIFI DOWN] http callback cmd operation error, cmd %s argv %s.",cmd->value,argv->value);
			}
		    }
		    else if(strcmp(cmd->value,"delDomain") == 0){
			ret = delByDomain(argv->value);
			if(ret == 0) {
			    debug(LOG_ERR, "[KNOWIFI DEL] http callback cmd operation error, cmd %s argv %s.",cmd->value,argv->value);
			}
		    }
#endif
#if (WIFIDOG_REMOTE_LOGOUT == FYES) /*远程挂断*/
		    else if(strcmp(cmd->value,"logout") == 0){
			ret = logoutByMAC(argv->value);
			if(ret == 0) {
			    debug(LOG_ERR, "[LOGOUT] http callback cmd operation error, cmd %s argv %s.",cmd->value,argv->value);
			}
		    }
#endif
#if (WIFIDOG_USER_SUCCESS_URL == FYES) /*认证成功URL*/
		    else if(strcmp(cmd->value,"seturl")==0){
			ret=setUserUrl(argv->value);	 
			if(ret == 0) {
			    debug(LOG_ERR, "[SET SUCCESS URL] http callback cmd operation error, cmd %s argv %s.",cmd->value,argv->value);
			}
		    }
#endif
#if (WIFIDOG_NOTICE == FYES) /*到期推送URL*/
		    /* "argv=time:1&url=xxx" */
		    else if(strcmp(cmd->value,"Wifidog_Notice") == 0){
			ret = wifidog_notice_operate(argv->value);
			if(ret == 0) {
			    debug(LOG_ERR, "[NOTICE] http callback cmd operation error, cmd %s argv %s.",cmd->value,argv->value);
			}
		    }
#endif
#if (WIFIDOG_MAC_AUTH == FYES)
		    else if(strcmp(cmd->value,"macAuth") == 0){
			ret = wifidog_macAuthEn(argv->value);
			if(ret == 0) {
			    debug(LOG_ERR, "[macAuth enable] http callback cmd operation error, cmd %s argv %s.",cmd->value,argv->value);
			}
		    }
#endif
		    else{
			debug(LOG_ERR, "cmd error : [%s] \n",cmd->value);
		    } 

		}else{
		    debug(LOG_ERR, "identify unkown : [%s]\n",identify->value);
		} 
	    }else{
		debug(LOG_ERR, "cloudn't get get argv \n");
		ret=0;
	    }
	}else{
	    debug(LOG_ERR, "cloudn't get get cmd \n");
	    ret=0;
	}
    }else{
	debug(LOG_ERR, "cloudn't get identify \n");
	ret=0;
    }
   if(ret){
       result=safe_strdup("{\"state\":\"1\"}");
   }else{
       result=safe_strdup("{\"state\":\"0\"}");
   }
   httpdOutput(r,result);
   free(result);
}

#if (UTT_KNOWIFI == FYES)
void http_home_router_302(httpd *webserver, request *r)
{
    char sendMsg[255],recvMsg[MAX_BUF];
    char code[50] = {0};
    char token[128] = {0};
    char expires[50] = {0};
    unsigned long overTime = 0;
    char redirect_url[255] = {0};
    unsigned long now;
    KnowifiDomainProfile *prof = NULL;

    char *mac = arp_get(r->clientAddr);

    if (!mac)
    {
	return ;
    }

    prof = knowifiDomainProf_lookup(mac);
    free(mac);
    
    memset(sendMsg, 0, sizeof(sendMsg));
    memset(recvMsg, 0, sizeof(recvMsg));

    if (!prof)
	return ;

    now = time(NULL);

    if (now > prof->tokenOverTime)
    {
	sprintf(sendMsg, "open/knowifi/v1.0/getToken/%s", prof->domain);
	//printf("domain : %s\n", prof->domain);
	int res = loop_http_get(AUTH_OS_SERVER, sendMsg, recvMsg);

	if (res > 0)
	{
	    getValueFromString(recvMsg, "code", code, 50);
	    if (strcmp(code, "0") == 0)
	    {
		getValueFromString(recvMsg, "token", token, 128);
		getValueFromString(recvMsg, "expires_in", expires, 50);
		overTime = strtoul(expires, NULL, 10);
		strncpy(prof->token, token, 128);
		prof->tokenOverTime = now + overTime;
	    }
	}
    }
    else
    {
	strncpy(token, prof->token, 128);
    }
    sprintf(redirect_url, "http://www.qingxueba.com/service/setToken?token=%s", token);
    http_send_redirect(r, redirect_url, "Redirect to login page");
    
    return;
}
#endif
