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

/*
 * $Id: client_list.c,v 1.8.4.5 2016/08/31 13:05:35 cao.yongxiang Exp $
 */
/** @file client_list.c
  @brief Client List Functions
  @author Copyright (C) 2004 Alexandre Carmel-Veillex <acv@acv.ca>
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/unistd.h>

#include <string.h>
#include <fcntl.h>

#include "safe.h"
#include "debug.h"
#include "conf.h"
#include "client_list.h"
#include "uttMachine.h"
#if (WIFIDOG_AUTH == FYES) || (WIFIDOG_MAKE_MONEY == FYES)
#include "typedef.h"
#include "mib.h"
#include "profacce.h"
#include "comExeFun.h"
#include "flash_api.h"
#endif
#if (UTT_KNOWIFI == FYES)
#include "firewall.h"
#endif

/** Global mutex to protect access to the client list */
pthread_mutex_t client_list_mutex = PTHREAD_MUTEX_INITIALIZER;

/** @internal
 * Holds a pointer to the first element of the list 
 */ 
t_client         *firstclient = NULL;

#if (SWITCH_AUTH_MODE == FYES)
extern char lanMac[];
#endif

#if (UTT_KNOWIFI == FYES)
freeDomain *freeDomainListHead = NULL;
#endif

/** Get the first element of the list of connected clients
 */
t_client *
client_get_first_client(void)
{
    return firstclient;
}

/**
 * Initializes the list of connected clients (client)
 */
void
client_list_init(void)
{
    firstclient = NULL;
}

/** Based on the parameters it receives, this function creates a new entry
 * in the connections list. All the memory allocation is done here.
 * @param ip IP address
 * @param mac MAC address
 * @param token Token
 * @return Pointer to the client we just created
 */
t_client         *
client_list_append(const char *ip, const char *mac, const char *token)
{
    t_client         *curclient, *prevclient;

    prevclient = NULL;
    curclient = firstclient;

    while (curclient != NULL) {
        prevclient = curclient;
        curclient = curclient->next;
    }

    curclient = safe_malloc(sizeof(t_client));
    if(curclient != NULL) {
    memset(curclient, 0, sizeof(t_client));

    curclient->ip = safe_strdup(ip);
    curclient->mac = mac;
    curclient->token = safe_strdup(token);
    curclient->counters.incoming = curclient->counters.incoming_history = curclient->counters.outgoing = curclient->counters.outgoing_history = 0;
    curclient->counters.last_updated = time(NULL);
    curclient->free_auth_client = 0;
    curclient->need_notify = 0;
    curclient->expire = -1;
#if (SWITCH_AUTH_MODE == FYES)
    if(strncmp(mac, lanMac, 17) == 0) {
	/*如果ip对应的mac为LAN口mac
	 * 说明此ip可能为三层交换机下的用户
	 * 需要在认证成功后，通过ip放通*/
	curclient->authType = WIFIDOG_CLIENT_AUTH_TYPE_SWITCH_NO_MAC_AUTH;
    }
#endif

    if (prevclient == NULL) {
        firstclient = curclient;
    } else {
        prevclient->next = curclient;
    }

    //debug(LOG_INFO, "Added a new client to linked list: IP: %s Token: %s",
    //      ip, token);

    }
    return curclient;
}

/** Finds a  client by its IP and MAC, returns NULL if the client could not
 * be found
 * @param ip IP we are looking for in the linked list
 * @param mac MAC we are looking for in the linked list
 * @return Pointer to the client, or NULL if not found
 */
t_client         *
client_list_find(const char *ip, const char *mac)
{
    t_client         *ptr;

    if((ip!=NULL) && (mac!=NULL)) {
	ptr = firstclient;
	while (NULL != ptr) {
	    if (0 == strcmp(ptr->ip, ip) && 0 == strcmp(ptr->mac, mac))
		return ptr;
	    ptr = ptr->next;
	}
    }

    return NULL;
}

/**
 * Finds a  client by its IP, returns NULL if the client could not
 * be found
 * @param ip IP we are looking for in the linked list
 * @return Pointer to the client, or NULL if not found
 */
t_client         *
client_list_find_by_ip(const char *ip)
{
    t_client         *ptr;

    ptr = firstclient;
    while (NULL != ptr) {
        if (0 == strcmp(ptr->ip, ip))
            return ptr;
        ptr = ptr->next;
    }

    return NULL;
}

/**
 * Finds a  client by its Mac, returns NULL if the client could not
 * be found
 * @param mac Mac we are looking for in the linked list
 * @return Pointer to the client, or NULL if not found
 */
t_client         *
client_list_find_by_mac(const char *mac)
{
    t_client         *ptr;

    ptr = firstclient;
    while (NULL != ptr) {
        if (0 == strcmp(ptr->mac, mac))
            return ptr;
        ptr = ptr->next;
    }

    return NULL;
}

/** Finds a client by its token
 * @param token Token we are looking for in the linked list
 * @return Pointer to the client, or NULL if not found
 */
t_client *
client_list_find_by_token(const char *token)
{
    t_client         *ptr;

    ptr = firstclient;
    while (NULL != ptr) {
        if (0 == strcmp(ptr->token, token))
            return ptr;
        ptr = ptr->next;
    }

    return NULL;
}

/** @internal
 * @brief Frees the memory used by a t_client structure
 * This function frees the memory used by the t_client structure in the
 * proper order.
 * @param client Points to the client to be freed
 */
void
_client_list_free_node(t_client * client)
{

    if (client->mac != NULL)
        free(client->mac);

    if (client->ip != NULL)
        free(client->ip);

    if (client->token != NULL)
        free(client->token);

    free(client);
}

/**
 * @brief Deletes a client from the connections list
 *
 * Removes the specified client from the connections list and then calls
 * the function to free the memory used by the client.
 * @param client Points to the client to be deleted
 */
void
client_list_delete(t_client * client)
{
    t_client         *ptr;

    ptr = firstclient;

    if (ptr == NULL) {
        debug(LOG_ERR, "Node list empty!");
    } else if (ptr == client) {
        firstclient = ptr->next;
        _client_list_free_node(client);
    } else {
        /* Loop forward until we reach our point in the list. */
        while (ptr->next != NULL && ptr->next != client) {
            ptr = ptr->next;
        }
        /* If we reach the end before finding out element, complain. */
        if (ptr->next == NULL) {
            debug(LOG_ERR, "Node to delete could not be found.");
        /* Free element. */
        } else {
            ptr->next = client->next;
            _client_list_free_node(client);
        }
    }
}

/*
 * @brief delete clients from th connection list 
 * by mac ,it returned by Auth Server
 * invoked byg ping thread
 * param: macStr
 *	eg: XXXX;XXXX;XXXX;
 * */
#define SEPARATE_CHAR ";"
void 
client_delete_by_mac(char* macStr)
{
    t_client *cli = NULL;
    char *ptr = NULL;
    char *macPtr = NULL;
    if(macStr != NULL) {
	debug(LOG_DEBUG, "get need down mac list: %s",macStr);
	ptr = strtok(macStr, SEPARATE_CHAR);
	while(ptr!=NULL) {
	    macPtr = convertMac(ptr, 1);
	    if(macPtr != NULL) {
	    cli = client_list_find_by_mac(macPtr);
	    if(cli) {
		//debug(LOG_DEBUG, "get client: ip %s, mac %s, expire %d, runtime %d",
		//	cli->ip,cli->mac,cli->expire,cli->runtime);
		fw_deny(cli->ip, cli->mac, cli->fw_connection_state);
#if 0
		client_list_delete(cli);
#else
		cli->need_notify = 1; /*先不删除，标志为待通知*/
#endif
	    }
	    }
	    ptr = strtok(NULL, SEPARATE_CHAR);
	}
    }

    return;
}

int
client_find_need_notify(const char *ip, const char *mac)
{
    t_client         *ptr;
    //if(mac != NULL) 
    {
    ptr = firstclient;
    while (NULL != ptr) {
        if ((ptr->need_notify == 1) && (0 == strcmp(ptr->ip, ip)))
	{
#if 0
	    client_list_delete(ptr);
#endif
            return 1;
	}
        ptr = ptr->next;
    }
    }
    return 0;
}

struct flock* file_lock(short type, short whence)
{
    static struct flock ret;
    ret.l_type = type;
    ret.l_start = 0;
    ret.l_whence = whence;
    ret.l_len = 0;
    ret.l_pid = getpid();
    return &ret;
}

void
clist_list_write_file()
{
    t_client *node = NULL;
    int count = 0;
    char buf[200];
    int buflen = 0;
    int writelen = 0;
    FILE *fp = NULL;


    fp = fopen(WIFI_CLIENT_LIST_FILE, "w+");
    if(fp != NULL) {

	node = client_get_first_client();

	memset(buf, 0, sizeof(buf));
	fcntl(fp, F_SETLKW, file_lock(F_WRLCK|F_RDLCK, SEEK_SET));
	while (node != NULL) {
	    if(node->need_notify != 1) {
		snprintf(buf,200,"ipaddr[%d]=\"%s\";macaddr[%d]=\"%s\";totaltime[%d]=%d;usedtime[%d]=%d;freeAuth[%d]=%d;",
			count,node->ip,count,node->mac,count,node->expire,count,node->runtime,count,node->free_auth_client);
		buflen = strlen(buf);
		fwrite(buf, sizeof(char), buflen, fp);
		memset(buf, 0, sizeof(buf));
		buflen = 0;
		count++;
	    }
	    node = node->next;
	}
	snprintf(buf,200, "totalrecs=%d;\n",count);
	buflen = strlen(buf);
	fwrite(buf, sizeof(char), buflen, fp);
	fcntl(fp, F_SETLKW, file_lock(F_UNLCK, SEEK_SET));
	memset(buf, 0, sizeof(buf));
	fclose(fp);
	fp = NULL;
    }
    return ;
}

#if 0
/* init client list 
 * debug 14770*/
char ipaddr[13][15]={"192.168.1.108","192.168.1.110","192.168.1.106","192.168.1.112","192.168.1.117","192.168.1.102","192.168.1.104","192.168.1.115","192.168.1.121","192.168.1.123","192.168.1.124","192.168.1.125","192.168.1.135"};
char macaddr[13][20]={"00:26:c6:a0:c4:fc","40:0e:85:61:df:ca","34:23:ba:28:54:c8","18:9e:fc:5b:92:e6","7c:fa:df:ad:00:40","d0:22:be:48:78:77","d0:7a:b5:de:1a:1b","c8:e0:eb:b5:b3:b0","5c:0a:5b:ff:08:c6","90:c1:15:ee:ba:68","d0:22:be:85:c1:bd","00:26:c6:a0:c4:fc","00:22:00:00:00:34"};
int totaltime=7200;
int usedtime[13]={7140,7080,7020,6960,6900,6840,6840,6780,6780,6720,6720,6720,6660};
int freeAuth[13]={0,0,0,0,0,0,0,0,0,0,0,0,0};

void
initClientList_test()
{
    int i=0;
    t_client *client = NULL;
    for(i=0;i<13;i++) {
	client = client_list_append(ipaddr[i], macaddr[i], "abcdefg");
	if(client != NULL) {
	    client->expire = totaltime;
	    client->runtime = usedtime[i];
	    client->free_auth_client = freeAuth[i];
	    debug(LOG_WARNING,"add client");
	}
    }
}
#endif
#if (WIFIDOG_REMOTE_LOGOUT == FYES) /*远程挂断*/
#define SEPARATE_CHAR2 ","
int cli_del_by_mac(char* macStr)
{
    t_client *cli = NULL;
    char *ptr = NULL;
    char *macPtr = NULL;
    int ret =	0;
    if(macStr != NULL) {
	debug(LOG_DEBUG, "get need down mac list: %s",macStr);
	ptr = strtok(macStr, SEPARATE_CHAR2);
	while(ptr!=NULL) {
	    macPtr = convertMac(ptr, 1);
	    if(macPtr != NULL) {
	    cli = client_list_find_by_mac(macPtr);
	    if(cli) {
		debug(LOG_DEBUG, "get client: ip %s, mac %s, expire %d, runtime %d",
			cli->ip,cli->mac,cli->expire,cli->runtime);
		fw_deny(cli->ip, cli->mac, cli->fw_connection_state);
		cli->runtime = cli->expire;
		ret +=1 ;
	    }
	    }
	    ptr = strtok(NULL, SEPARATE_CHAR2);
	}
    }

    return ret;
}
#endif
#if (UTT_KNOWIFI == FYES)
freeDomain *freeDomainFind(char *domain)
{
    freeDomain *pst = NULL;
    if (!freeDomainListHead)
    {
	return NULL;
    }
    pst = freeDomainListHead->next;
    while(pst)
    {
	if (domain && strcmp(pst->domain, domain) == 0)
	{
	    break;
	}
	pst = pst->next;
    }
    return pst;
}
int freeDomainEdit(char *domain, unsigned long freetime)
{
    freeDomain *pst = NULL, *new = NULL, *pre = NULL; 
    if (!domain)
    {
	debug(LOG_ERR, "domain can not be Null.\n");
	return 0;
    }
    if (!freeDomainListHead)
    {
	freeDomainListHead = (freeDomain *)malloc(sizeof(freeDomain));
	if (!freeDomainListHead)
	{
	    debug(LOG_ERR, "freeDomainList init failed.\n");
	    return 0;
	}
	freeDomainListHead->next = NULL;
    }

    LOCK_CLIENT_LIST();

    pst = freeDomainFind(domain);
    if (!pst)
    {
	pre = freeDomainListHead;
	new = freeDomainListHead->next;
	while(new)
	{
	    pre = new;
	    new = new->next;
	}
	new = (freeDomain *)malloc(sizeof(freeDomain));
	pre->next = new;
	pst = new;
	strncpy(pst->domain, domain, 128);
    }
    pst->freetime = freetime;
    pst->runtime = 0;
    pst->next = NULL;

    UNLOCK_CLIENT_LIST();
    return 1;
}
int freeDomainDel(char *domain)
{
    MibProfileType mibType = MIB_PROF_KNOWIFI_DOMAIN;
    KnowifiDomainProfile *prof = NULL;
    freeDomain *pst = NULL, *pre = NULL;
    int i = 0, max = 0, min = 0;
    int ret = 0;

    pre = freeDomainListHead; 
    if (!pre)
    {
	return 0;
    }

    LOCK_CLIENT_LIST();

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++)
    {
        prof = (KnowifiDomainProfile*)ProfGetInstPointByIndex(mibType, i);
        if (prof != NULL && prof->head.active == TRUE && prof->mode == KNOWIFI_MODE_MISSION)
        {
            if (prof->domain && strncmp(prof->domain, domain, 128) == 0)
            {
		fw_deny(prof->ip, prof->mac, FW_MARK_KNOWN); 
		prof->isFree = 0;
            }
        }
    }

    pst = pre->next;
    while(pst)
    {
	if (domain && strcmp(pst->domain, domain) == 0)
	{
	    pre->next = pst->next;
	    free(pst);
	    pst = NULL;
	    ret = 1;
	    break;
	}
	pre = pst;
	pst = pst->next;
    }

    UNLOCK_CLIENT_LIST();
    return ret;
}
int freeDomainCheckTimeOut(unsigned long checkTime)
{
    if (!freeDomainListHead)
	return 0;
    freeDomain *pst = freeDomainListHead->next;
    while(pst)
    {
	if(pst->runtime >= pst->freetime)
	{
	    freeDomainDel(pst->domain);
	}
	else
	{
	    pst->runtime += checkTime;
	}
	pst = pst->next;
    }
    return 1;
}
#endif
