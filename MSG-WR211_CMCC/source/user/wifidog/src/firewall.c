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
 * $Id: firewall.c,v 1.12.4.4 2016/08/31 01:51:31 cao.yongxiang Exp $
 */
/** @internal
  @file firewall.c
  @brief Firewall update functions
  @author Copyright (C) 2004 Philippe April <papril777@yahoo.com>
  2006 Benoit Grégoire, Technologies Coeus inc. <bock@step.polymtl.ca>
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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/time.h>

#ifdef __linux__
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netpacket/packet.h>
#endif

#if defined(__NetBSD__)
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#endif

#include "httpd.h"
#include "safe.h"
#include "debug.h"
#include "conf.h"
#include "firewall.h"
#include "fw_iptables.h"
#include "auth.h"
#include "centralserver.h"
#include "client_list.h"
#include "uttMachine.h"
#if (WIFIDOG_AUTH == FYES) || (WIFIDOG_MAKE_MONEY == FYES)
#include "typedef.h"
#include "mib.h"
#include "profacce.h"
#include "comExeFun.h"
#include "flash_api.h"
#endif

extern pthread_mutex_t client_list_mutex;

/* from commandline.c */
extern pid_t restart_orig_pid;
extern char maclist[];
#if (SWITCH_AUTH_MODE == FYES)
extern char lanMac[];
#endif



/**
 * Allow a client access through the firewall by adding a rule in the firewall to MARK the user's packets with the proper
 * rule by providing his IP and MAC address
 * @param ip IP address to allow
 * @param mac MAC address to allow
 * @param fw_connection_state fw_connection_state Tag
 * @return Return code of the command
 */
int
fw_allow(char *ip, char *mac, int fw_connection_state)
{
    debug(LOG_DEBUG, "Allowing %s %s with fw_connection_state %d", ip, mac, fw_connection_state);

    return iptables_fw_access(FW_ACCESS_ALLOW, ip, mac, fw_connection_state);
}

/**
 * @brief Deny a client access through the firewall by removing the rule in the firewall that was fw_connection_stateging the user's traffic
 * @param ip IP address to deny
 * @param mac MAC address to deny
 * @param fw_connection_state fw_connection_state Tag
 * @return Return code of the command
 */
int
fw_deny(char *ip, char *mac, int fw_connection_state)
{
    debug(LOG_DEBUG, "Denying %s %s with fw_connection_state %d", ip, mac, fw_connection_state);

    return iptables_fw_access(FW_ACCESS_DENY, ip, mac, fw_connection_state);
}

/* XXX DCY */
/**
 * Get an IP's MAC address from the ARP cache.
 * Go through all the entries in /proc/net/arp until we find the requested
 * IP address and return the MAC address bound to it.
 * @todo Make this function portable (using shell scripts?)
 */
char           *
arp_get(char *req_ip)
{
    FILE           *proc=NULL;
	 char ip[16];
	 char mac[18];
	 char * reply = NULL;
	 char format[51] = " %15[0-9.] %*s %*s %17[A-Fa-f0-9:] %*s %*s %*s %*s";
	 int ret = 0;

    if (!(proc = fopen("/proc/net/arp", "r"))) {
        return NULL;
    }

    /* Skip first line */
	 while (!feof(proc) && fgetc(proc) != '\n');

	 /* Find ip, copy mac in reply */
	 reply = NULL;
    reply = (char *)malloc(18);
    if(reply!=NULL) {
	memset(reply, 0, 18);
    if(strcmp(SYSLINUXDIR, ("linux-2.6.21.x")) != 0) {
	format[42] = '\0';
    }
    memset(mac, 0, sizeof(mac));
    while (!feof(proc) && (fscanf(proc, format, ip, mac) == 2)) {
		  if (strcmp(ip, req_ip) == 0) {
				//reply = safe_strdup(mac);
				if(strlen(mac) == 17) {
				strcpy(reply, mac);
				ret = 1;
				break;
				}
		  }
		memset(mac, 0, sizeof(mac));
    }
    }

    fclose(proc);
    if(ret == 0) {
	if(reply != NULL) {
#if (SWITCH_AUTH_MODE == FYES)
	    /*
	     * 默认获取LAN口mac地址
	     * 主要是三层交换机下的主机可能获取不到mac地址*/
	    strncpy(reply, lanMac, 17);
#else
	    free(reply);
	    reply = NULL;
#endif
	}
    }

    return reply;
}

char *arp_get_by_mac(char *req_mac)
{
    FILE *proc=NULL;
    char ip[16];
    char mac[18];
    char type[16];
    char ifname[16];
    char *reply = NULL;
    char format[51] = " %15[0-9.] %*s %s %17[A-Fa-f0-9:] %*s %s %*s %*s";
    int ret = 0;

    char mactmp1[6] = {0};
    char mactmp2[6] = {0};
    char *p1 = NULL;
    char *p2 = NULL;
    int i = 0;
    if (!(proc = fopen("/proc/net/arp", "r"))) {
        return NULL;
    }

    /* Skip first line */
	 while (!feof(proc) && fgetc(proc) != '\n');

	 /* Find ip, copy mac in reply */
	 reply = NULL;
    reply = (char *)malloc(16);
    if(reply!=NULL) {
	memset(reply, 0, 16);
    if(strcmp(SYSLINUXDIR, ("linux-2.6.21.x")) != 0) {
	format[42] = '\0';
    }
    memset(mac, 0, sizeof(mac));
    while (!feof(proc) && (fscanf(proc, format, ip, type, mac, ifname) == 4)) {
	//printf("type: %s, ifname: %s\n",type,ifname);
	if (strcmp(type, "0x0") == 0 || strcmp(ifname, "br0"))
	    continue;
	p1 = mac;
	p2 = req_mac;
	for (i = 0; i < 6;i++)
	{
	    mactmp1[i] = strtol(p1, &p1, 16);
	    mactmp2[i] = strtol(p2, &p2, 16);
	    p1++;
	    p2++;
	}
	if (memcmp(mactmp1, mactmp2, 6) == 0) {
	    //reply = safe_strdup(mac);
	    strcpy(reply, ip);
	    ret = 1;
	    break;
	}
	memset(ip, 0, sizeof(ip));
    }
    }

    fclose(proc);
    if(ret == 0) {
	if(reply != NULL) {
	    free(reply);
	    reply = NULL;
	}
    }

    return reply;
    
}

/** Initialize the firewall rules
 */
int
fw_init(void)
{
    int flags, oneopt = 1, zeroopt = 0;
	 int result = 0;
	 t_client * client = NULL;

    debug(LOG_INFO, "Creating ICMP socket");
    if ((icmp_fd = socket (AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1 ||
            (flags = fcntl(icmp_fd, F_GETFL, 0)) == -1 ||
             fcntl(icmp_fd, F_SETFL, flags | O_NONBLOCK) == -1 ||
             setsockopt(icmp_fd, SOL_SOCKET, SO_RCVBUF, &oneopt, sizeof(oneopt)) ||
             setsockopt(icmp_fd, SOL_SOCKET, SO_DONTROUTE, &zeroopt, sizeof(zeroopt)) == -1) {
        debug(LOG_ERR, "Cannot create ICMP raw socket.");
        return 0;
    }

    debug(LOG_INFO, "Initializing Firewall");
    result = iptables_fw_init();

	 if (restart_orig_pid) {
		 debug(LOG_INFO, "Restoring firewall rules for clients inherited from parent");
		 LOCK_CLIENT_LIST();
		 client = client_get_first_client();
		 while (client) {
			 fw_allow(client->ip, client->mac, client->fw_connection_state);
			 client = client->next;
		 }
		 UNLOCK_CLIENT_LIST();
	 }

	 return result;
}

/** Remove all auth server firewall whitelist rules
 */
#ifdef WIFI_PRE_REDIRECT
void
fw_clear_authservers(char *ip)
{
	debug(LOG_INFO, "Clearing the authservers list");
	if(ip != NULL)
	    iptables_fw_clear_authservers(ip);
}
#else
void
fw_clear_authservers(void)
{
	debug(LOG_INFO, "Clearing the authservers list");
	iptables_fw_clear_authservers();
}
#endif

/** Add the necessary firewall rules to whitelist the authservers
 */
void
fw_set_authservers(void)
{
	debug(LOG_INFO, "Setting the authservers list");
	iptables_fw_set_authservers();
}

/** Remove the firewall rules
 * This is used when we do a clean shutdown of WiFiDog.
 * @return Return code of the fw.destroy script
 */
int
fw_destroy(void)
{
    if (icmp_fd != 0) {
        debug(LOG_INFO, "Closing ICMP socket");
        close(icmp_fd);
    }

    debug(LOG_INFO, "Removing Firewall rules");
    return iptables_fw_destroy();
}

/**Probably a misnomer, this function actually refreshes the entire client list's traffic counter, re-authenticates every client with the central server and update's the central servers traffic counters and notifies it if a client has logged-out.
 * @todo Make this function smaller and use sub-fonctions
 */
void
fw_sync_with_authserver(void)
{
    t_authresponse  authresponse;
    char            *token, *ip, *mac;
    t_client        *p1, *p2;
    unsigned long long	    incoming, outgoing;
    s_config *config = config_get_config();
#if(WIFIDOG_MAC_AUTH == FYES)
    int length=0;
    char *mac_tmp,*macP;
#endif

#if (WIFIDOG_NOTICE == FYES)
    /*到期推送URL*/
    unsigned int noticeTime = config->noticeTime; /*单位为分钟*/
#endif
#ifdef REQUEST_COUNTERS
    if (-1 == iptables_fw_counters_update()) {
        debug(LOG_ERR, "Could not get counters from firewall!");
        return;
    }
#endif
#if 0
    /*delete needless iptables rules*/
    time_t tmp;
    time(&tmp);
    if(is_auth_online()) {
	iptables_fw_add_del(IPTABLE_DEL);
    } else {
	iptables_fw_add_del(IPTABLE_ADD);
    }
#endif
    debug(LOG_DEBUG, "=============== client timeout");

    //LOCK_CLIENT_LIST();

    for (p1 = p2 = client_get_first_client(); NULL != p1; p1 = p2) {
        p2 = p1->next;

        ip = safe_strdup(p1->ip);
        token = safe_strdup(p1->token);
        mac = safe_strdup(p1->mac);
	    outgoing = p1->counters.outgoing;
	    incoming = p1->counters.incoming;

	    //UNLOCK_CLIENT_LIST();
        /* Ping the client, if he responds it'll keep activity on the link.
         * However, if the firewall blocks it, it will not help.  The suggested
         * way to deal witht his is to keep the DHCP lease time extremely
         * short:  Shorter than config->checkinterval * config->clienttimeout */
        //icmp_ping(ip);
#ifdef REQUEST_COUNTERS
        /* Update the counters on the remote server only if we have an auth server */
        if (config->auth_servers != NULL) {
            auth_server_request(&authresponse, REQUEST_TYPE_COUNTERS, ip, mac, token, incoming, outgoing);
        }
#endif
	    //LOCK_CLIENT_LIST();

        if (!(p1 = client_list_find(ip, mac))) {
            debug(LOG_ERR, "Node %s was freed while being re-validated!", (ip!=NULL)?(ip):(""));
        } else {
        	time_t	current_time=time(NULL);
        	debug(LOG_INFO, "Checking client %s for timeout:  Last updated %ld (%ld seconds ago), timeout delay %ld seconds, current time %ld, ",
                        p1->ip, p1->counters.last_updated, current_time-p1->counters.last_updated, config->checkinterval * config->clienttimeout, current_time);
	    p1->runtime += config->checkinterval;
            debug(LOG_DEBUG, "%s, expire time %d, run time %d======\n", ip,p1->expire,p1->runtime);
#if 1
	    if((p1->runtime >= p1->expire && p1->expire > 0)) {
#else
            if (p1->counters.last_updated +
				(config->checkinterval * config->clienttimeout)
				<= current_time) {
                /* Timing out user */
                debug(LOG_INFO, "%s - Inactive for more than %ld seconds, removing client and denying in firewall",
                        p1->ip, config->checkinterval * config->clienttimeout);
#endif
#if (WIFIDOG_MAKE_MONEY != FYES)
		if(p1->free_auth_client == 1) {
		ipsetAOrDIpStr(IPSET_WIFIDOG_FREE_AUTH, p1->ip, IPSET_MEM_DEL);
		}
#endif
#if (WIFIDOG_NOTICE == FYES)
		/*到期推送URL
		 * 客户端ip到期且没有通告时
		 * 从通告组中删除*/
		ipsetAOrDIpStr(WIFI_IPSET_NOTICE_GROUP, p1->ip, IPSET_MEM_DEL);
#endif
#if (WIFI_RATE_LIMIT == FYES) /*带宽控制*/
		if((((p1->ctrlType)>>WIFIDOG_CLIENT_CTRL_TYPE_RATE_LIMIT)&0x01) == 1) {
		    debug(LOG_ERR, "Rate Limit delete client ip %s, mac %s, ctrlType %d",p1->ip,p1->mac,p1->ctrlType);
		    rateRuleIPAddOrDel(p1->data.index, p1->ip, IPSET_MEM_DEL);
		} else 
#endif
#if (SWITCH_AUTH_MODE == FYES)
		    if( p1->authType == WIFIDOG_CLIENT_AUTH_TYPE_SWITCH_NO_MAC_AUTH ) {
			ipsetAOrDIpStr(IPSET_GROUP_SWITCH_CLIENT_LIST, p1->ip, IPSET_MEM_DEL);
		    } else
#endif
		{
			fw_deny(p1->ip, p1->mac, p1->fw_connection_state);
		}
LOCK_CLIENT_LIST();
                client_list_delete(p1);
UNLOCK_CLIENT_LIST();
#if(WIFIDOG_MAC_AUTH == FYES)
		arpctl(ip,'d');
#endif
		debug(LOG_DEBUG, "%s, delete client list node ==========\n", ip);

#if 0
		/* delete the logout request, because auth server do not receive logout packet
		 * and it conflicted with ping thread for LOCK_CONFIG */
                /* Advertise the logout if we have an auth server */
                if (config->auth_servers != NULL) {
		    if((p1->free_auth_client != 1)&&(p1->need_notify != 1)&&(is_auth_online())) { /* free auth client conflict for the config lock with ping thread*/
					UNLOCK_CLIENT_LIST();
					auth_server_request(&authresponse, REQUEST_TYPE_LOGOUT, ip, mac, token, 0, 0);
					LOCK_CLIENT_LIST();
		    }
                }
#endif
            } else {
#if (WIFIDOG_NOTICE == FYES)
	/* 只有客户端ip还未到期，才执行通告判断
	 * 以下情况需要添加到需通告组:
	 * 1、客户端到期
	 * 2、还未添加到需通告组
	 * */
#if 1
	    if((noticeTime > 0U) && (((p1->expire)-(p1->runtime)) <= (noticeTime*60)) 
		    && (ipsetTestIpStr(WIFI_IPSET_NOTICE_GROUP, p1->ip) != 1)) {
		    /*除下列类型客户端外都需通告
		     * 1、免认证用户
		     * 2、mac黑白名单用户*/
		    if((p1->free_auth_client != 1) &&
			    (p1->authType != WIFIDOG_CLIENT_AUTH_TYPE_MAC_BLACK) &&
			    (p1->authType != WIFIDOG_CLIENT_AUTH_TYPE_MAC_WHITE)) {
			ipsetAOrDIpStr(WIFI_IPSET_NOTICE_GROUP, p1->ip, IPSET_MEM_DEL);
			ipsetAOrDIpStr(WIFI_IPSET_NOTICE_GROUP, p1->ip, IPSET_MEM_ADD);
		    }
		}
#endif
#endif /*WIFIDOG_NOTICE*/
#ifdef REQUEST_COUNTERS
                /*
                 * This handles any change in
                 * the status this allows us
                 * to change the status of a
                 * user while he's connected
                 *
                 * Only run if we have an auth server
                 * configured!
                 */
                if (config->auth_servers != NULL) {
                    switch (authresponse.authcode) {
                        case AUTH_DENIED:
                            debug(LOG_NOTICE, "%s - Denied. Removing client and firewall rules", p1->ip);
                            fw_deny(p1->ip, p1->mac, p1->fw_connection_state);
                            client_list_delete(p1);
                            break;

                        case AUTH_VALIDATION_FAILED:
                            debug(LOG_NOTICE, "%s - Validation timeout, now denied. Removing client and firewall rules", p1->ip);
                            fw_deny(p1->ip, p1->mac, p1->fw_connection_state);
                            client_list_delete(p1);
                            break;

                        case AUTH_ALLOWED:
                            if (p1->fw_connection_state != FW_MARK_KNOWN) {
                                debug(LOG_INFO, "%s - Access has changed to allowed, refreshing firewall and clearing counters", p1->ip);
                                //WHY did we deny, then allow!?!? benoitg 2007-06-21
                                //fw_deny(p1->ip, p1->mac, p1->fw_connection_state);

                                if (p1->fw_connection_state != FW_MARK_PROBATION) {
     p1->counters.incoming = p1->counters.outgoing = 0;
                                }
                                else {
                                	//We don't want to clear counters if the user was in validation, it probably already transmitted data..
                                    debug(LOG_INFO, "%s - Skipped clearing counters after all, the user was previously in validation", p1->ip);
                                }
                                p1->fw_connection_state = FW_MARK_KNOWN;
                                fw_allow(p1->ip, p1->mac, p1->fw_connection_state);
                            }
                            break;

                        case AUTH_VALIDATION:
                            /*
                             * Do nothing, user
                             * is in validation
                             * period
                             */
                            debug(LOG_INFO, "%s - User in validation period", p1->ip);
                            break;

                              case AUTH_ERROR:
                                    debug(LOG_WARNING, "Error communicating with auth server - leaving %s as-is for now", p1->ip);
                                    break;

                        default:
                            debug(LOG_ERR, "I do not know about authentication code %d", authresponse.authcode);
                            break;
                    }
                }
#endif

#if(WIFIDOG_MAC_AUTH == FYES)
		if(p1->authType == WIFIDOG_CLIENT_AUTH_TYPE_MAC_WHITE || p1->authType == WIFIDOG_CLIENT_AUTH_TYPE_MAC_GREY || p1->authType == WIFIDOG_CLIENT_AUTH_TYPE_MAC_BLACK){
		    mac_tmp = arp_get(p1->ip);
		    if(!mac_tmp || (strcmp(mac_tmp,p1->mac))){

			p1->runtime = p1->expire;
			length = strlen(maclist);
			macP = convertMac(p1->mac,0);
			snprintf((maclist+length),1024-length,"%s;",macP);
			free(macP);
		    }else if((p1->authType == WIFIDOG_CLIENT_AUTH_TYPE_MAC_WHITE || p1->authType == WIFIDOG_CLIENT_AUTH_TYPE_MAC_GREY) && p1->runtime%600 == 0){
		    arpctl(p1->ip,'d');
		    }else if(p1->authType == WIFIDOG_CLIENT_AUTH_TYPE_MAC_BLACK && p1->runtime%300){
			arpctl(p1->ip,'d');
		    }
		    free (mac_tmp);
		}
#endif
            }
        }

        free(token);
        free(ip);
        free(mac);
    }
#if (WIFIDOG_MAKE_MONEY != FYES)
#ifdef WIFI_PRE_REDIRECT
	checkFreeAuth();
#endif
#endif
    /*write client list file*/
    clist_list_write_file();
    //UNLOCK_CLIENT_LIST();
}

#if (WIFIDOG_MAKE_MONEY != FYES)
#ifdef WIFI_PRE_REDIRECT
/* checkFreeAuth
 * 将内核中添加到免认证ipset组中的ip添加到clientlist链表中
 * 通过ipset -L查找ip是否添加到clientlist链表中。
 * 否则添加到链表中，以方便显示到页面中
 * */
static void checkFreeAuth(void)
{

    int ret = 0;
    int flag = 0;
    t_client *newclient=NULL;
    t_client *findclient=NULL;
    char tmpstr[100];
    char linestr[50 + 1];
    FILE *fp = NULL;
    char number[16];
    char *mac = NULL;
    struct in_addr intaddr;
    s_config *config = config_get_config();
    int rc = 0;

    memset(tmpstr, 0, sizeof(tmpstr));
#if CONFIG_PPC_85xx
    sprintf(tmpstr, "ipset -L %s > /etc/wifiFreeList", IPSET_WIFIDOG_FREE_AUTH);
    rc = execute(tmpstr, 0);
    if (rc!=0) {
	debug(LOG_ERR, "execute command failed(%d): %s", rc, tmpstr);
	return;
    }
    fp = fopen("/etc/wifiFreeList", "r");
#else
    sprintf(tmpstr, "ipset -L %s", IPSET_WIFIDOG_FREE_AUTH);
    fp = popen(tmpstr, "r");/*执行命令*/
#endif
    if(fp != NULL)
    {
	while (fgets(linestr, 100 , fp) != NULL)/*读取一行*/
	{
	    debug(LOG_DEBUG,"%s-%d: linestr: %s\n", __func__, __LINE__, linestr);
	    if(flag == 0) {
		if(strstr(linestr,"Members") != NULL )/*从下一行开始读取免认证IP*/
		{
		    flag = 1;
		    continue;
		} else {
		    continue;
		}
	    }
	    memset(number, 0, sizeof(number));
	    sscanf(linestr, "%s", number);
	    intaddr.s_addr = inet_addr(number);
	    if (intaddr.s_addr == INADDR_NONE) { /*检测IP的合法性*/
		continue;
	    }
	    debug(LOG_DEBUG,"%s-%d: ip %s\n", __func__, __LINE__, number);
	    mac = arp_get(number);
	    if(mac!=NULL) {
		/*
		 * 先查找IP是否在client链表中，分下面的两种情况处理：
		 * 1、查找到，判断是否为踢下线的IP，是则修改状态为为免认证，否则不做处理
		 * 2、未查找到。则添加到链表中
		 * */
		ret = client_find_need_notify(number,mac);
		/* 避免相同ip不同mac的情况
		 * mantis 14784
		 * */
		if((findclient=client_list_find_by_ip(number))== NULL) {
		    LOCK_CLIENT_LIST();
		    newclient = client_list_append(number, mac, "utt_wifi_free_auth");
		    UNLOCK_CLIENT_LIST();
		}
		if((findclient != NULL)&&(ret == 1)) {
		    if (findclient->mac != NULL)
			free(findclient->mac);
		    findclient->mac = mac;
		    findclient->fw_connection_state = FW_MARK_KNOWN;
		    findclient->expire = config->freeAuth_time;
		    findclient->free_auth_client = 1;
		    findclient->need_notify = 0;
		}
		if(newclient != NULL) {
		    newclient->fw_connection_state = FW_MARK_KNOWN;
		    newclient->expire = config->freeAuth_time;
		    newclient->free_auth_client = 1;
		}
		else {
		    if((findclient == NULL)||((findclient!=NULL)&&(ret==0))) {
			free(mac);
		    }
		}
	    }
	}
#if CONFIG_PPC_85xx
	fclose(fp);
#else
	pclose(fp);/*关闭通道*/
#endif

    }
}
#endif
#endif

void
icmp_ping(char *host)
{
	struct sockaddr_in saddr;
#if defined(__linux__) || defined(__NetBSD__)
	struct {
		struct ip ip;
		struct icmp icmp;
	} packet;
#endif
	unsigned int i, j;
	int opt = 2000;
	unsigned short id = rand16();

	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	inet_aton(host, &saddr.sin_addr);
#if defined(HAVE_SOCKADDR_SA_LEN) || defined(__NetBSD__)
	saddr.sin_len = sizeof(struct sockaddr_in);
#endif

#if defined(__linux__) || defined(__NetBSD__)
	memset(&packet.icmp, 0, sizeof(packet.icmp));
	packet.icmp.icmp_type = ICMP_ECHO;
	packet.icmp.icmp_id = id;

	for (j = 0, i = 0; i < sizeof(struct icmp) / 2; i++)
		j += ((unsigned short *)&packet.icmp)[i];

	while (j >> 16)
		j = (j & 0xffff) + (j >> 16);

	packet.icmp.icmp_cksum = (j == 0xffff) ? j : ~j;

	if (setsockopt(icmp_fd, SOL_SOCKET, SO_RCVBUF, &opt, sizeof(opt)) == -1)
		debug(LOG_ERR, "setsockopt(): %s", strerror(errno));

	if (sendto(icmp_fd, (char *)&packet.icmp, sizeof(struct icmp), 0,
	           (const struct sockaddr *)&saddr, sizeof(saddr)) == -1)
		debug(LOG_ERR, "sendto(): %s", strerror(errno));

	opt = 1;
	if (setsockopt(icmp_fd, SOL_SOCKET, SO_RCVBUF, &opt, sizeof(opt)) == -1)
		debug(LOG_ERR, "setsockopt(): %s", strerror(errno));
#endif

	return;
}

unsigned short rand16(void) {
  static int been_seeded = 0;

  if (!been_seeded) {
    unsigned int seed = 0;
    struct timeval now;

    /* not a very good seed but what the heck, it needs to be quickly acquired */
    gettimeofday(&now, NULL);
    seed = now.tv_sec ^ now.tv_usec ^ (getpid() << 16);

    srand(seed);
    been_seeded = 1;
    }

    /* Some rand() implementations have less randomness in low bits
     * than in high bits, so we only pay attention to the high ones.
     * But most implementations don't touch the high bit, so we
     * ignore that one.
     **/
      return( (unsigned short) (rand() >> 15) );
}
