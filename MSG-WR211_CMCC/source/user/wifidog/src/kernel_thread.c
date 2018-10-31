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

/* $Id: kernel_thread.c,v 1.9.6.24 2016/09/21 10:11:18 wang.lei Exp $ */
/** @file kernel_thread.c
    @brief Periodically checks in with the central auth server so the auth
    server knows the gateway is still up.  Note that this is NOT how the gateway
    detects that the central server is still up.
    @author Copyright (C) 2004 Alexandre Carmel-Veilleux <acv@miniguru.ca>
*/

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
#include <linux/netlink.h>
#include <linux/socket.h>
#include "uttMachine.h"
#include "mib.h"

#if ((WEBAUTH_WECHAT_ATTENTION == FYES) || (WEBAUTH_MAC_VALIDATE == FYES) ||(WIFIDOG_MAC_AUTH == FYES))
#include <linux/netfilter_ipv4/utt_netfilter.h>
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
extern char *iconv_string (const char *str, const char *from_codeset, const char *to_codeset);
extern int flag_updateDomainOk = 0;
extern int flag_updateDomainFirst = 1;
extern int utt_knowifi_updateDomain(void);
#endif

int nl_sock_fd;
extern long served_this_session;
extern pthread_mutex_t	client_list_mutex;
extern char maclist[];

char *ip2str(const unsigned int ip, char *ipStr)
{
    struct in_addr addr;

    addr.s_addr = ip;
    strcpy(ipStr, inet_ntoa(addr));

    return ipStr;
}

unsigned int str2ip(const char *ipStr, unsigned int *ip)
{
    struct in_addr addr;

    if (ipStr)
    {
	inet_aton(ipStr, &addr);
	*ip = addr.s_addr;
    }
    else
    {
	*ip = 0;
    }

    return *ip;
}

char *mac2str(const unsigned char *mac, char *macStr)
{
    memset(macStr, 0, 18);
    if (mac)
    {
	sprintf(macStr, "%02x:%02x:%02x:%02x:%02x:%02x", 
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    return macStr;
}

unsigned char *str2mac(const char *macStr, unsigned char *mac)
{
    char *p;
    int i;

    if (strlen(macStr) == 17)
    {
	p = macStr;
	for (i=0; i<6; i++)
	{
	    mac[i] = strtol(p, &p, 16);
	    p++;
	}

    }
    else
    {
	memset(mac, 0, 6);
    }

    return mac;
}

#if (UTT_KNOWIFI == FYES)
extern KnowifiDomainProfile *knowifiDomainProf_lookup(unsigned char *mac)
{           
    MibProfileType mibType = MIB_PROF_KNOWIFI_DOMAIN;
    KnowifiDomainProfile *prof = NULL;
    int i = 0, max, min;
    unsigned char mac1[6] = {0};
    unsigned char mac2[6] = {0};
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++)
    {
        prof = (KnowifiDomainProfile*)ProfGetInstPointByIndex(mibType, i);
        if (prof != NULL && prof->head.active == TRUE)
        {
	    if (mac && prof->mac)
	    {
		str2mac(mac, mac1);
		str2mac(prof->mac, mac2);
		if (memcmp(mac1, mac2, 6) == 0)
		    return prof;
	    }
        }
    }
    debug(LOG_ERR, "mac %s : not in redirect list\n", mac);
    return NULL;
}
static int check_knowifi_redirect_client(unsigned int ip, unsigned char *mac)
{

    char cmd[255];
    char ipStr[16] = {0};
    char macStr[18] = {0};

    unsigned long now = time(NULL);
    unsigned long timeNow = now % 86400;
    unsigned int week = (4 + now / 86400) % 7;

    freeDomain *freedomain = NULL;
    ip2str(ip, ipStr);
    mac2str(mac, macStr);

    MibProfileType mibType = MIB_PROF_KNOWIFI_DOMAIN;
    KnowifiDomainProfile *prof = knowifiDomainProf_lookup(macStr);

    if (!prof)
	return 0;

    prof->isFree = 0;
    prof->tokenOverTime = 0;
    memset(prof->token, 0, sizeof(prof->token));
    fw_deny(prof->ip, prof->mac, FW_MARK_KNOWN); /*防止用户认证成功后，重复认证添加规则*/

    if (strcmp(prof->ip, ipStr))
    {
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "ipset -D %s %s", KNOWIFI_REDIRECT_CLIENT, prof->ip);
	execute(cmd, 0);
	strncpy(prof->ip, ipStr, 16);
	fw_deny(prof->ip, prof->mac, FW_MARK_KNOWN); /*防止用户认证成功后，重复认证添加规则*/
    }
    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "ipset -D %s %s", KNOWIFI_REDIRECT_CLIENT, ipStr);
    execute(cmd, 0);
    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "ipset -A %s %s,%s", KNOWIFI_REDIRECT_CLIENT, prof->ip, prof->mac);
    execute(cmd, 0);

    if (prof->mode == KNOWIFI_MODE_MISSION)
    {
	freedomain = freeDomainFind(prof->domain);
	if (freedomain)
	{
	    prof->isFree = 1;
	    fw_allow(prof->ip, prof->mac, FW_MARK_KNOWN);
	}
	freedomain = NULL;
    }
    else if (prof->mode == KNOWIFI_MODE_TIME)
    {
	if ((0x1 << (week - 1) & prof->weekdays)
		&& timeNow >= prof->timeStart
		&& timeNow <= prof->timeEnd)
	{
	    prof->isFree = 1;
	    fw_allow(prof->ip, prof->mac, FW_MARK_KNOWN);
	}
    }

    return 1;
}
typedef struct delInst_t{
    int active[MAX_KNOWIFI_DOMAIN_COUNT];
}delInst;

void delDomainMacList(delInst *delinst, int *saveFlag)
{
    uttSemP(SEM_KNOWIFI,0);
    MibProfileType mibType = MIB_PROF_KNOWIFI_DOMAIN;
    KnowifiDomainProfile *prof = NULL;
    int i = 0, max, min;
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++)
    {
        prof = (KnowifiDomainProfile*)ProfGetInstPointByIndex(mibType, i);
        if (prof != NULL && prof->head.active == TRUE)
        {
	    if (delinst->active[i] == 0)
		ProfDelInstByIndex(mibType, i);
        }
    }

    if (*saveFlag == 1)
	nvramWriteCommit();

    uttSemV(SEM_KNOWIFI,0);
	
    return;
}
#if 0
void showDomainMacList()
{
    MibProfileType mibType = MIB_PROF_KNOWIFI_DOMAIN;
    KnowifiDomainProfile *prof = NULL;
    int i = 0, max, min;
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++)
    {
        prof = (KnowifiDomainProfile*)ProfGetInstPointByIndex(mibType, i);
        if (prof != NULL && prof->head.active == TRUE)
        {
	    printf("==================\n");
	    printf("inst : %d\nmac : %s\ndomain : %s\n", i, prof->mac, prof->domain);
	    printf("==================\n");
        }
    }

}
#endif
int addDomainMacList(char *mac, 
		    char *domain, 
		    char *username, 
		    char *mode, 
		    char *weekdaysStr, 
		    char *startTimeStr, 
		    char *stopTimeStr, 
		    delInst *delinst,
		    int *saveFlag)
{
    //printf("mac: %s\ndomain: %s\nusername: %s\nmode: %s\nweekdays: %s\nstartTime: %s\nstopTime: %s\n",
	//    mac,domain,username,mode,weekdaysStr,startTimeStr, stopTimeStr);
	//
    uttSemP(SEM_KNOWIFI,0);
    MibProfileType mibType = MIB_PROF_KNOWIFI_DOMAIN;
    KnowifiDomainProfile *prof = NULL;

    unsigned long timeStart = 0;
    unsigned long timeEnd = 0;
    unsigned int week = 0;
    unsigned int weekValue = 0;
    char *weekStr = NULL;
    char *timeStrtmp = NULL;
    char *devIp = NULL;
    char ipTmp[16] = {0};
    char weekdays[32];
    char startTime[32];
    char stopTime[32];
    int newprof = 0;

    strncpy(weekdays, weekdaysStr, 32);
    strncpy(startTime, startTimeStr, 32);
    strncpy(stopTime, stopTimeStr, 32);


    int instIndex = ProfGetInstIndexByName(mibType, mac);
    if(instIndex == PROFFAIL)
    {
	instIndex = ProfNewInst(mibType, mac, FALSE);
	if(instIndex == PROFFAIL) {
	    uttSemV(SEM_KNOWIFI,0);
	    return 0;
	}
	newprof = 1;
    }

    KnowifiDomainProfile *profold = (KnowifiDomainProfile *)malloc(sizeof(KnowifiDomainProfile));
    memset(profold, 0, sizeof(KnowifiDomainProfile));

    prof = (KnowifiDomainProfile*)ProfGetInstPointByIndex(mibType, instIndex);
    memcpy(profold, prof, sizeof(KnowifiDomainProfile));

    delinst->active[instIndex] = 1;

    ProfSetNameByPoint(prof, mac);
    strncpy(prof->username, username, 128);
    strncpy(prof->domain, domain, 128);
    strncpy(prof->mac, mac, 18);
    devIp = arp_get_by_mac(mac);
    if (!devIp)
    {
	sprintf(ipTmp, "164.253.80.%d",  instIndex);
	strcpy(prof->ip, ipTmp);
    }
    else
	strncpy(prof->ip, devIp, 16);
    free(devIp);
    memset(prof->token, 0, sizeof(prof->token));
    prof->isFree = 0;
    prof->tokenOverTime = 0;
    if (strcmp(mode, "task") == 0)
    {
	prof->mode = KNOWIFI_MODE_MISSION;
    }
    else if (strcmp(mode, "time") == 0)
    {
	prof->mode = KNOWIFI_MODE_TIME;
	weekStr = strtok(weekdays, ",");
	while(weekStr)
	{
	    weekValue = strtoul(weekStr, NULL, 10);
	    if (weekValue <= 7 && weekValue >= 1)
	    {
		week |= (0x1 << (weekValue - 1));
	    }
	    weekStr = strtok(NULL, ",");
	}
        timeStrtmp = strtok(startTime, ":");
        timeStart += 3600 * strtoul(timeStrtmp, NULL, 10);
        timeStrtmp = strtok(NULL, ":");
        timeStart += 60 * strtoul(timeStrtmp, NULL, 10);
	timeStrtmp = strtok(stopTime, ":");
	timeEnd += 3600 * strtoul(timeStrtmp, NULL, 10);
	timeStrtmp = strtok(NULL, ":");
	timeEnd += 60 * strtoul(timeStrtmp, NULL, 10);
        prof->timeStart = timeStart;
        prof->timeEnd = timeEnd;
        prof->weekdays = week;
    }
    else
    {
	prof->mode = KNOWIFI_MODE_FORBBIDEN;
    
    }
    prof->head.active = TRUE;
    
    if (newprof)
    {
	*saveFlag = 1;
    }
    else if (memcmp(prof, profold, sizeof(KnowifiDomainProfile)))
    {
	*saveFlag = 1;
    }
    free(profold);
    profold = NULL;
    //nvramWriteCommit();
    uttSemV(SEM_KNOWIFI,0);

    return 1;
}
int getUsernameFromDomainList(char *str, char *username ,char *domain)
{
    int i = 0;
    int j = 0;
    char usernameUTF8[128];
    char *temp;
    char *tempGBK;
    while(str[i])
    {
	if(str[i] == '\t' || str[i] == '\n' || str[i] == '\r' || str[i] == ' ' || str[i] == '{' || str[i] ==  ',' || str[i] == '}')
	{
	    i++;
	    continue;
	}
	while(str[i] == ' ' || str[i] == '\"' || str[i] == '\t') i++;
	memset(usernameUTF8, 0, sizeof(usernameUTF8));
	while(str[i] && j < 128 && str[i] != '\"')
	{   
	    usernameUTF8[j] = str[i];
	    j++;
	    i++;
	}
	usernameUTF8[127] = '\0';
	tempGBK = iconv_string(usernameUTF8 ,"utf-8","GBK");
	if (tempGBK)
	    temp = tempGBK;
	else
	    temp = usernameUTF8?usernameUTF8:"";
	strncpy(username, temp?temp:"", 127);
	if (tempGBK)
	    free(tempGBK);
	while(str[i] == ' ' || str[i] == '\"' || str[i] == ':' || str[i] == '\t') i++;
	j = 0;
	while(str[i] && j < 128 && str[i] != '\"')
	{   
	    domain[j] = str[i];
	    j++;
	    i++;
	}
	return 1;
    } 
    return 1;
}
int setMacListFromDomainList(char *str, 
			    char *username, 
			    char *mode, 
			    char *weekdays, 
			    char *startTime, 
			    char *stopTime, 
			    delInst *delinst, 
			    int *saveFlag)
{
    int i = 0;
    char mac[18];
    char domain[128];
    int j = 0;
    while(str[i])
    {
	if(str[i] == '\t' || str[i] == '\n' || str[i] == '\r' || str[i] == ' ' || str[i] == '{' || str[i] ==  ',' || str[i] == '}')
	{
	    i++;
	    continue;
	}
	while(str[i] == ' ' || str[i] == '\"') i++;
	memset(mac, 0, sizeof(mac));
	for(j = 0; j<17 ;j++,i++)
	{
	    mac[j] = str[i];
	}
	while(str[i] == ' ' || str[i] == '\"' || str[i] == ':' || str[i] == '\t') i++;
	j = 0;
	memset(domain, 0, sizeof(domain));
	while(str[i] && j < 128 && str[i] != '\"')
	{   
	    domain[j] = str[i];
	    j++;
	    i++;
	}
	mac[17] = '\0';
	domain[127] = '\0';
	addDomainMacList(mac, domain, username, mode, weekdays, startTime, stopTime, delinst, saveFlag);
	while(str[i] && str[i] != ',') i++;
    } 
    return 1;
}
int utt_knowifi_updateDomain(void)
{
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
    MibProfileType mibType = MIB_PROF_KNOWIFI_DOMAIN;
    KnowifiDomainProfile *prof = NULL;
    char sendMsg[255],recvMsg[MAX_BUF];
    char code[32] = {0};
    cJSON *json;
    cJSON *item;
    char *weekdays;
    char *startTime;
    char *stopTime;
    char *freeTime;
    char *modeName;
    char userName[128] = {0};
    char domain[128] = {0};

    int saveFlag = 0;
    delInst *delinst = (delInst *)malloc(sizeof(delInst));
    memset(delinst, 0, sizeof(delInst));

    //printf("--func : %s, line : %d\n", __FUNCTION__, __LINE__);

    if((profG != NULL) && (ProfInstIsFree(profG) == 0))
    {
	if(strcmp(profG->knowifi_routeId, "unknown") && strcmp(profG->knowifi_routeId, ""))
	{
	    memset(sendMsg, 0, sizeof(sendMsg));
	    memset(recvMsg, 0, sizeof(recvMsg));
	    sprintf(sendMsg, "open/knowifi/v1.0/getDomain/%s" ,profG->knowifi_routeId);
	    int res = loop_http_get(AUTH_OS_SERVER, sendMsg, recvMsg);
	    if (res > 0)
	    {
		getValueFromString(recvMsg, "code", code, 32);
		if (strcmp(code, "0") == 0)
		{
		    printf("recv: \n%s\n", recvMsg);
		    json=cJSON_Parse(recvMsg);
		    if (!json) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}
		    else
		    {
			if((item = cJSON_GetObjectItem(json, "domains")) != NULL)
			{
			    //delDomainMacList();
			    cJSON* pArrayItem = NULL;
			    int i = 0;
			    int nCount = cJSON_GetArraySize(item);
			    for(i = 0; i < nCount; i++)
			    {
				pArrayItem = cJSON_GetArrayItem(item, i);
				cJSON* pChild;
				if ((pChild = cJSON_GetObjectItem(pArrayItem, "mode")) != NULL)
				{
				    cJSON *pst = NULL;
				    pst = cJSON_GetObjectItem(pChild, "name");
				    modeName = pst?pst->valuestring:"";
				    pst = cJSON_GetObjectItem(pChild, "weekdays");
				    weekdays = pst?pst->valuestring:"";
				    pst = cJSON_GetObjectItem(pChild, "start_time");
				    startTime = pst?pst->valuestring:"";
				    pst = cJSON_GetObjectItem(pChild, "stop_time");
				    stopTime = pst?pst->valuestring:"";
				}
				if ((pChild = cJSON_GetObjectItem(pArrayItem, "username")) != NULL)
				{
				    char *temp = cJSON_Print(pChild);
				    getUsernameFromDomainList(temp, userName, domain);
				    //printf("domain : %s\n",domain);
				}
				if ((pChild = cJSON_GetObjectItem(pArrayItem, "maclist")) != NULL)
				{
				    char *temp = cJSON_Print(pChild);
				    setMacListFromDomainList(temp, userName, modeName, weekdays, startTime, stopTime, delinst, &saveFlag);
				}
				if ((pChild = cJSON_GetObjectItem(pArrayItem, "freetime")) != NULL)
				{
				    //printf("freetime : %d\n", pChild->valueint);
				    freeDomainEdit(domain, pChild->valueint);
				}
			    }
			    delDomainMacList(delinst, &saveFlag);
			    flag_updateDomainOk = 1;
			}
		    }
		    cJSON_Delete(json);
		}
	    }
	}
    }
    free(delinst);
    delinst = NULL;
    return 1;
}
#endif

void check_validate_client(const char *request_type, unsigned int ip, unsigned char *mac)
{
#if (UTT_KNOWIFI == FYES)
    check_knowifi_redirect_client(ip, mac);
    return ;
#endif
    t_client	*client;
    t_authresponse  auth_response;
    char ipStr[16], *macStr = (char*)malloc(18);
    char *tokenWechat = "WeiXinAuth", *tokenMac = "MacAuth";
    int  need_add_session = 0;
#if  1
    if(!is_online() ||!is_auth_online()){

	return ;
    }
#endif
#if(WIFIDOG_MAC_AUTH == FYES)
    s_config	*config = config_get_config();
    if(config->macAuthEn == 0U){
	return ;
    }
#endif

    ip2str(ip, ipStr);
    mac2str(mac, macStr);
    debug(LOG_ERR, "%s %d: ip=%s, mac=%s\n", __func__, __LINE__, ipStr, macStr);
    client = client_list_find(ipStr, macStr);
    if (client)
    {
#if(WIFIDOG_MAC_AUTH == FYES)
	if (auth_response.authType != WIFIDOG_CLIENT_AUTH_TYPE_MAC_WHITE && auth_response.authType == WIFIDOG_CLIENT_AUTH_TYPE_MAC_GREY && auth_response.authType != WIFIDOG_CLIENT_AUTH_TYPE_MAC_BLACK )
#endif
	{
	    debug(LOG_ERR, "client ip %s mac %s already exist.\n", ipStr, macStr);
	    return;
	}
    }

    auth_server_request(&auth_response, request_type, ipStr, macStr, NULL, 0, 0);

    debug(LOG_ERR, "response code: %d\n", auth_response.authcode);
    debug(LOG_DEBUG, "response authType: %d\n", auth_response.authType);
#if 1
    if (auth_response.authcode == AUTH_ALLOWED)
    {
	debug(LOG_ERR, "ip %s mac %s online, validtime %d second.", ipStr, macStr, auth_response.expire);
#if(WIFIDOG_MAC_AUTH == FYES)
	client = client_list_find_by_ip(ipStr);
#else
	client = client_list_find(ipStr, macStr);
#endif
	if (client == NULL)
	{
	    LOCK_CLIENT_LIST();
	    if (strcmp(request_type, REQUEST_TYPE_WECHAT_ATTENTION) == 0)
		client = client_list_append(ipStr, macStr, tokenWechat);
	    else
		client = client_list_append(ipStr, macStr, tokenMac);
	    UNLOCK_CLIENT_LIST();
	    need_add_session = 1;
	}
	client->fw_connection_state = FW_MARK_KNOWN;
	client->expire = auth_response.expire;
	client->need_notify = 0;
	client->runtime=0;
	client->authType = auth_response.authType;
	fw_deny(client->ip, client->mac, FW_MARK_KNOWN); /*防止用户认证成功后，重复认证添加规则*/

#if(WIFIDOG_MAC_AUTH == FYES)
	if(!need_add_session && strcmp(macStr,client->mac)){
	    if(client->mac != NULL){
		free(client->mac);
	    }
	    if(client->token != NULL){
		free(client->token);
	    }

	    snprintf((maclist+strlen(maclist)),1024-strlen(maclist),"%s;", client->mac);
	    client->mac = macStr;
	    client->token = safe_strdup(tokenMac);
	}
	if (client->authType == WIFIDOG_CLIENT_AUTH_TYPE_MAC_BLACK)
	{
	    need_add_session = 0;
	}
#endif
#if (WIFI_RATE_LIMIT == FYES) /*带宽控制*/
	if(client->authType != WIFIDOG_CLIENT_AUTH_TYPE_MAC_BLACK && ((auth_response.upRate > 0) || (auth_response.downRate > 0))) {
	    /*
	     * 带宽控制客户端添加到对应的限速组中
	     * */
	    createRateLimit(client->ip, client->mac, auth_response.upRate, auth_response.downRate, 0, 0);
	} else 
#endif
	{
	    fw_allow(client->ip, client->mac, FW_MARK_KNOWN);
	}
	served_this_session +=need_add_session ;
    }
    else
	free(macStr);
#else
    client = client_list_find(ipStr, macStr);
    if (client == NULL)
    {
	LOCK_CLIENT_LIST();
	client = client_list_append(ipStr, macStr, token);
	UNLOCK_CLIENT_LIST();
    }
    debug(LOG_ERR, "ip %s mac %s online, validtime %d second.", client->ip, client->mac, 240);
    client->fw_connection_state = FW_MARK_KNOWN;
    client->expire = 240;
    client->need_notify = 0;
    fw_deny(client->ip, client->mac, FW_MARK_KNOWN); /*防止用户认证成功后，重复认证添加规则*/
    fw_allow(client->ip, client->mac, FW_MARK_KNOWN);
    served_this_session++;
#endif
    return;
}

void parse_kernel(UttNlNfWifidogConf *conf)
{
    char ipStr[16], macStr[18];

    if (conf == NULL)
    {
	debug(LOG_ERR, "receive msg from kernel is null.\n");
	return;
    }

    debug(LOG_ERR, "conf->confType=%d, ip=%s, mac=%s\n", 
	    conf->confType, ip2str(conf->ip, ipStr), mac2str(conf->mac, macStr));

#if (WEBAUTH_WECHAT_ATTENTION == FYES)
    if (conf->confType == UTT_NF_WIFIDOG_WECHAT_ATTENTION)
    {
	check_validate_client(REQUEST_TYPE_WECHAT_ATTENTION, conf->ip, conf->mac);
    }
    else
#endif
#if (WEBAUTH_MAC_VALIDATE == FYES || WIFIDOG_MAC_AUTH == FYES) 
    if (conf->confType == UTT_NF_WIFIDOG_ARP_ADD)
    {
	check_validate_client(REQUEST_TYPE_MAC_VALIDATE, conf->ip, conf->mac);
    }
    else
#endif
    {
	debug(LOG_ERR, "receive msg from kernel confType(%d) is unkown.\n", conf->confType);
    }

    return;
}

void mac_validate_init(void)
{
    char *fileName = "/proc/net/arp";
    char cmd[100];
    FILE *fp = NULL;
    char arr[100];
    char ipStr[16];
    char macStr[18];
    int  flags;
    char device[10];
    unsigned int ip;
    unsigned char mac[6];
    int ret,i;
    do{
	sleep(3);
	i++;
    }while(!is_auth_online() && i < 10);

    fp = fopen(fileName, "r");
    if (fp)
    {
	while (fgets(arr, 100, fp))
	{
	    ret = sscanf(arr, "%s %*s %x %s %*s %s", ipStr, &flags, macStr, device);
	    if (strncmp(device, getLanIfName(),strlen(getLanIfName())) == 0 && flags == 0x2)
	    {
		str2ip(ipStr, &ip);
		str2mac(macStr, mac);
		check_validate_client(REQUEST_TYPE_MAC_VALIDATE, ip, mac);
	    }
	}
	fclose(fp);
    }
    else
	debug(LOG_ERR, "cannot read %s.\n", fileName);

    return;
}

/* 路由器刚启动时，尚不能上网，需要等待能够访问外网后再执行后续任务 */
void wait_for_network_ok(void)
{
    debug(LOG_DEBUG, "check network.\n");
    while (!is_auth_online())
    {
	debug(LOG_DEBUG, "network not ok, wait 1 second.");
	sleep(1);
    }

    debug(LOG_DEBUG, "network ok.\n");
}

void thread_listen_kernel(void *arg)
{
    struct sockaddr_nl nl_addr;
    struct iovec iov;
    struct msghdr msg;
    char buf[UTT_MAX_NLMSGSIZE];
    struct nlmsghdr *nlh = NULL;
    UttNlNfMsgHead *msgHead = NULL;
    UttNlNfWifidogConf  sendConf;

    memset(&msg, 0, sizeof(msg));

    nlh = (struct nlmsghdr*) buf;
    iov.iov_base = (void *)nlh;
    iov.iov_len = UTT_MAX_NLMSGSIZE;
    msg.msg_name = (void *)&nl_addr;
    msg.msg_namelen = sizeof(nl_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    unsigned int mypid = pthread_self()<<16|getpid();

    //wait_for_network_ok();

    nl_sock_fd = uttNfNlSkBind(mypid, 0);/*单播到本进程*/
    if(nl_sock_fd <  0)
    {
	debug(LOG_ERR, "uttNfNlSkBind error: %d\n", nl_sock_fd);
	pthread_exit(NULL);/*bind异常线程直接结束*/
    }

    sendConf.msgHead.nlType = UTT_NLNF_WIFIDOG;
    sendConf.confType = UTT_NFCONF_INIT;

    debug(LOG_DEBUG, "send wifidog pid to kernel.");
    uttNfNlSend(nl_sock_fd, (char *)&sendConf, sizeof(sendConf), 0, 0);/*发送给内核自己的pid*/

#if (UTT_KNOWIFI == FYES)
    utt_knowifi_updateDomain();
    flag_updateDomainFirst = 0;
    //showDomainMacList();
#endif
    mac_validate_init();

    while(1)
    {
	memset(buf, 0 ,sizeof(buf));
	if(recvmsg(nl_sock_fd, &msg, 0) > 0)/*阻塞方式接受消息*/
	{
	    msgHead = (UttNlNfMsgHead*)NLMSG_DATA(nlh);
	    if (msgHead == NULL)
	    {
		debug(LOG_ERR, "msgHead is null.\n");
		continue;
	    }

	    if (msgHead->nlType == UTT_NLNF_WIFIDOG)
	    {
		parse_kernel((UttNlNfWifidogConf *)NLMSG_DATA(nlh));
	    }
	}
    }

    return;
}
#endif
