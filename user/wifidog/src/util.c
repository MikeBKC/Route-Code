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
 * $Id: util.c,v 1.17.4.2 2015/06/24 14:14:13 hou.bo Exp $
 */
/**
  @file util.c
  @brief Misc utility functions
  @author Copyright (C) 2004 Philippe April <papril777@yahoo.com>
  @author Copyright (C) 2006 Benoit Gr茅goire <bock@step.polymtl.ca>
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
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

#if defined(__NetBSD__)
#include <sys/socket.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <util.h>
#endif

#ifdef __linux__
#include <netinet/in.h>
#include <net/if.h>
#endif

#include <string.h>
#include <pthread.h>
#include <netdb.h>

#if(WIFIDOG_MAC_AUTH == FYES)
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <net/if_arp.h>
#endif
#include "common.h"
#include "client_list.h"
#include "safe.h"
#include "util.h"
#include "conf.h"
#include "debug.h"

#include "../config.h"
#include "uttMachine.h"
#include "mib.h"
#include "profacce.h"

static pthread_mutex_t ghbn_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Defined in ping_thread.c */
extern time_t started_time;

/* Defined in clientlist.c */
extern	pthread_mutex_t	client_list_mutex;
extern	pthread_mutex_t	config_mutex;

/* Defined in commandline.c */
extern pid_t restart_orig_pid;

/* XXX Do these need to be locked ? */
static time_t last_online_time = 0;
static time_t last_offline_time = 0;
static time_t last_auth_online_time = 0;
static time_t last_auth_offline_time = 0;

long served_this_session = 0;

extern int auth_off_flag;
extern int authEn_ping;
extern char timestamp[TIME_STAMP_LEN];

extern char* parsePDB(char* pdb,const char* key){
	const char *split = ";";
	char *p,*index,*result=NULL;
	p = strtok(pdb,split);
	while(p!=NULL){
		index = strstr(p,key);
		if(index!=NULL){
			result = index+strlen(key)+1;
			//break;
		}
		p = strtok(NULL,split);
	}
	return result;
}

/*
 * 组建温馨匹配策略规则*/
extern char* buildRule(char *pdb,char * result){
	char rule[256],target[256],*par;
	memset(rule,0,256);
	strcpy(target,pdb);
	par = parsePDB(target,"protocol");
	if((par!=NULL)&&(strlen(par)>0)){
		sprintf(rule+strlen(rule)," -p %s ",par);
		par==NULL;
	} else {
		sprintf(rule+strlen(rule)," -p %s ","tcp");
	}
	strcpy(target,pdb);
	par = parsePDB(target,"sport");
	if(par!=NULL){
		sprintf(rule+strlen(rule)," --sport %s ",par);
	}
	strcpy(target,pdb);
	par=parsePDB(target,"dport");
	if((par!=NULL)&&(strlen(par)>0)){
		sprintf(rule+strlen(rule)," -m multiport --dport 80,443,8080,%s ",par);
	} else {
		sprintf(rule+strlen(rule)," -m multiport --dport 80,443,8080 ");
	}
	strcpy(target,pdb);
	par = parsePDB(target,"name");
	if(par!=NULL){
		sprintf(rule+strlen(rule)," -m layer7 --l7proto %s ",par);
	strcpy(result,rule);
	}
	return result;
}

/** Fork a child and execute a shell command, the parent
 * process waits for the child to return and returns the child's exit()
 * value.
 * @return Return code of the command
 */
int
execute(char *cmd_line, int quiet)
{
        int pid,
            status,
            rc;

        const char *new_argv[4];
        new_argv[0] = "/bin/sh";
        new_argv[1] = "-c";
        new_argv[2] = cmd_line;
        new_argv[3] = NULL;

        pid = safe_fork();
        if (pid == 0) {    /* for the child process:         */
                /* We don't want to see any errors if quiet flag is on */
                if (quiet) close(2);
                if (execvp("/bin/sh", (char *const *)new_argv) == -1) {    /* execute the command  */
                        debug(LOG_ERR, "execvp(): %s", strerror(errno));
                } else {
                        debug(LOG_ERR, "execvp() failed");
		}
		exit(1);
        }

	if(pid!=-1) { 
        /* for the parent:      */
	debug(LOG_DEBUG, "Waiting for PID %d to exit", pid);
	rc = waitpid(pid, &status, 0);
	debug(LOG_DEBUG, "Process PID %d exited", rc);

        return (WEXITSTATUS(status));
	}
        return -1;
}

	struct in_addr *
wd_gethostbyname(const char *name)
{
	struct hostent *he;
	struct in_addr *h_addr, *in_addr_temp;

	/* XXX Calling function is reponsible for free() */

	h_addr = safe_malloc(sizeof(struct in_addr));
	if(h_addr != NULL) {

	LOCK_GHBN();

	he = gethostbyname(name);

	if (he == NULL) {
		free(h_addr);
		UNLOCK_GHBN();
		return NULL;
	}


	in_addr_temp = (struct in_addr *)he->h_addr_list[0];
	h_addr->s_addr = in_addr_temp->s_addr;

	mark_online();
	UNLOCK_GHBN();

	}
	return h_addr;
}

	char *
get_iface_ip(const char *ifname)
{
#if defined(__linux__)
	struct ifreq if_data;
	struct in_addr in;
	char *ip_str;
	int sockd;
	u_int32_t ip;

	/* Create a socket */
	if ((sockd = socket (AF_INET, SOCK_PACKET, htons(0x8086))) < 0) {
		debug(LOG_ERR, "socket(): %s", strerror(errno));
		return NULL;
	}

	/* Get IP of internal interface */
	strcpy (if_data.ifr_name, ifname);

	/* Get the IP address */
	if (ioctl (sockd, SIOCGIFADDR, &if_data) < 0) {
		debug(LOG_ERR, "ioctl(): SIOCGIFADDR %s", strerror(errno));
		return NULL;
	}
	memcpy ((void *) &ip, (void *) &if_data.ifr_addr.sa_data + 2, 4);
	in.s_addr = ip;

	ip_str = inet_ntoa(in);
	close(sockd);
	return safe_strdup(ip_str);
#elif defined(__NetBSD__)
	struct ifaddrs *ifa, *ifap;
	char *str = NULL;

	if (getifaddrs(&ifap) == -1) {
		debug(LOG_ERR, "getifaddrs(): %s", strerror(errno));
		return NULL;
	}
	/* XXX arbitrarily pick the first IPv4 address */
	for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
		if (strcmp(ifa->ifa_name, ifname) == 0 &&
				ifa->ifa_addr->sa_family == AF_INET)
			break;
	}
	if (ifa == NULL) {
		debug(LOG_ERR, "%s: no IPv4 address assigned");
		goto out;
	}
	str = safe_strdup(inet_ntoa(
				((struct sockaddr_in *)ifa->ifa_addr)->sin_addr));
out:
	freeifaddrs(ifap);
	return str;
#else
	return safe_strdup("0.0.0.0");
#endif
}

	char *
get_iface_mac(const char *ifname)
{
#if defined(__linux__)
	int r, s;
	struct ifreq ifr;
#if (WIFIDOG_STDIF == FYES)
	char *hwaddr, mac[13];
#else
	char *hwaddr, mac[18];
#endif

	strcpy(ifr.ifr_name, ifname);

	s = socket(PF_INET, SOCK_DGRAM, 0);
	if (-1 == s) {
		debug(LOG_ERR, "get_iface_mac socket: %s", strerror(errno));
		return NULL;
	}

	r = ioctl(s, SIOCGIFHWADDR, &ifr);
	if (r == -1) {
		debug(LOG_ERR, "get_iface_mac ioctl(SIOCGIFHWADDR): %s", strerror(errno));
		close(s);
		return NULL;
	}

	hwaddr = ifr.ifr_hwaddr.sa_data;
	close(s);
#if (WIFIDOG_STDIF == FYES)
	snprintf(mac, sizeof(mac), "%02X%02X%02X%02X%02X%02X", 
			hwaddr[0] & 0xFF,
			hwaddr[1] & 0xFF,
			hwaddr[2] & 0xFF,
			hwaddr[3] & 0xFF,
			hwaddr[4] & 0xFF,
			hwaddr[5] & 0xFF
		);
#else
	snprintf(mac, sizeof(mac), "%02x-%02x-%02x-%02x-%02x-%02x", 
			hwaddr[0] & 0xFF,
			hwaddr[1] & 0xFF,
			hwaddr[2] & 0xFF,
			hwaddr[3] & 0xFF,
			hwaddr[4] & 0xFF,
			hwaddr[5] & 0xFF
		);
#endif

	return safe_strdup(mac);
#elif defined(__NetBSD__)
	struct ifaddrs *ifa, *ifap;
	const char *hwaddr;
	char mac[13], *str = NULL;
	struct sockaddr_dl *sdl;

	if (getifaddrs(&ifap) == -1) {
		debug(LOG_ERR, "getifaddrs(): %s", strerror(errno));
		return NULL;
	}
	for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
		if (strcmp(ifa->ifa_name, ifname) == 0 &&
				ifa->ifa_addr->sa_family == AF_LINK)
			break;
	}
	if (ifa == NULL) {
		debug(LOG_ERR, "%s: no link-layer address assigned");
		goto out;
	}
	sdl = (struct sockaddr_dl *)ifa->ifa_addr;
	hwaddr = LLADDR(sdl);
	snprintf(mac, sizeof(mac), "%02X%02X%02X%02X%02X%02X",
			hwaddr[0] & 0xFF, hwaddr[1] & 0xFF,
			hwaddr[2] & 0xFF, hwaddr[3] & 0xFF,
			hwaddr[4] & 0xFF, hwaddr[5] & 0xFF);

	str = safe_strdup(mac);
out:
	freeifaddrs(ifap);
	return str;
#else
	return NULL;
#endif
}

/* convertMac
 * mac 地址转换
 *  flag: 
 *	    0 -- ':' -> '-'
 *	    1 -- '-' -> ':'
 * */
#define SEPARATE_MAC_CHAR1 ":"
#define SEPARATE_MAC_CHAR2 "-"
char *convertMac(const char *mac, int flag)
{
    char *ptr = NULL;
    char *macPtr = NULL;
    ptr = strdup(mac);

    if((ptr!=NULL)&&(strlen(ptr)>0)) {
	if(flag == 0) {
	    macPtr = strstr(ptr, SEPARATE_MAC_CHAR1);
	    while(macPtr != NULL) {
		*macPtr = '-';
		macPtr = strstr(ptr, SEPARATE_MAC_CHAR1);
	    }
	} else if(flag == 1) {
	    macPtr = strstr(ptr, SEPARATE_MAC_CHAR2);
	    while(macPtr != NULL) {
		*macPtr = ':';
		macPtr = strstr(ptr, SEPARATE_MAC_CHAR2);
	    }
	}
	debug(LOG_DEBUG,"after convert ptr:%s, original mac: %s\n",ptr,mac);
    } else {
	ptr = mac;
    }
    return ptr;
}

#if 0
void getTimeStamp(void)
{ 
    time_t t = time(NULL);
    struct tm *tblock=NULL;

    memset(timestamp, 0, sizeof(timestamp));
    tblock = localtime(&t);
    snprintf(timestamp,TIME_STAMP_LEN,"%04d%02d%02d%02d%02d%02d",(tblock->tm_year+1900),(tblock->tm_mon+1),(tblock->tm_mday-1),
    tblock->tm_hour,tblock->tm_min,tblock->tm_sec);
    debug(LOG_DEBUG,"sec =%d, min =%d, hour =%d, dd =%d, mm =%d, yy =%d, wday =%d,yday =%d,isdst =%d,zone =%s\n",
    tblock->tm_sec,tblock->tm_min,tblock->tm_hour,tblock->tm_mday,tblock->tm_mon,tblock->tm_year,
    tblock->tm_wday,tblock->tm_yday,tblock->tm_isdst,tblock->tm_zone);
    return ;
}
#else
void getTimeStamp(char *timestamp)
{ 
    time_t t = time(NULL);
    struct tm *tblock=NULL;

    if(timestamp != NULL) {
	if((tblock = localtime(&t)) != NULL) 
	{
	    snprintf(timestamp,TIME_STAMP_LEN,"%04d%02d%02d%02d%02d%02d",(tblock->tm_year+1900),(tblock->tm_mon+1),(tblock->tm_mday-1),
		    tblock->tm_hour,tblock->tm_min,tblock->tm_sec);
	    //debug(LOG_DEBUG,"sec =%d, min =%d, hour =%d, dd =%d, mm =%d, yy =%d, wday =%d,yday =%d,isdst =%d,zone =%s\n",
		    //tblock->tm_sec,tblock->tm_min,tblock->tm_hour,tblock->tm_mday,tblock->tm_mon,tblock->tm_year,
		    //tblock->tm_wday,tblock->tm_yday,tblock->tm_isdst,tblock->tm_zone);
	}
    }
    return ;
}
#endif

int headbits(unsigned int nm)
{
    int i;
    /* it is nothing, only a bit operation */
    for(i=0; ((nm&0x80000000U) != 0U) && (i<=32) ; i++){
	/* the first bit of the nm is 1 */
	nm<<=1U;
    }
    return i;
}

	char *
get_ext_iface(void)
{
#ifdef __linux__
	FILE *input=NULL;
	char *device, *gw;
	int i = 1;
	int keep_detecting = 1;
	pthread_cond_t		cond = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t		cond_mutex = PTHREAD_MUTEX_INITIALIZER;
	struct	timespec	timeout;
	device = (char *)malloc(16);
	gw = (char *)malloc(16);
	debug(LOG_DEBUG, "get_ext_iface(): Autodectecting the external interface from routing table");
	while(keep_detecting) {
		input = fopen("/proc/net/route", "r");
		if(input != NULL) {
		while (!feof(input)) {
			/* XXX scanf(3) is unsafe, risks overrun */ 
			fscanf(input, "%s %s %*s %*s %*s %*s %*s %*s %*s %*s %*s\n", device, gw);
			if (strcmp(gw, "00000000") == 0) {
				free(gw);
				debug(LOG_INFO, "get_ext_iface(): Detected %s as the default interface after try %d", device, i);
				fclose(input);
				return device;
			}
		}
		fclose(input);
		}
		debug(LOG_ERR, "get_ext_iface(): Failed to detect the external interface after try %d (maybe the interface is not up yet?).  Retry limit: %d", i, NUM_EXT_INTERFACE_DETECT_RETRY);
		/* Sleep for EXT_INTERFACE_DETECT_RETRY_INTERVAL seconds */
		timeout.tv_sec = time(NULL) + EXT_INTERFACE_DETECT_RETRY_INTERVAL;
		timeout.tv_nsec = 0;
		/* Mutex must be locked for pthread_cond_timedwait... */
		pthread_mutex_lock(&cond_mutex);	
		/* Thread safe "sleep" */
		pthread_cond_timedwait(&cond, &cond_mutex, &timeout);
		/* No longer needs to be locked */
		pthread_mutex_unlock(&cond_mutex);
		//for (i=1; i<=NUM_EXT_INTERFACE_DETECT_RETRY; i++) {
		if (NUM_EXT_INTERFACE_DETECT_RETRY != 0 && i>NUM_EXT_INTERFACE_DETECT_RETRY) {
			keep_detecting = 0;
		}
		i++;
	}
	debug(LOG_ERR, "get_ext_iface(): Failed to detect the external interface after %d tries, aborting", i);
	//exit(1);
	free(device);
	free(gw);
#endif
	return NULL;
	}

	void mark_online() {
#if (WIFIDOG_MAKE_MONEY == FYES)
#else
		int before;
		int after;

		before = is_online();
		time(&last_online_time);
		after = is_online();

		if (before != after) {
			debug(LOG_INFO, "ONLINE status became %s", (after ? "ON" : "OFF"));
		}
		if((started_time < ASSIGNED_TIME) && (last_online_time > ASSIGNED_TIME)) {
		    started_time = last_online_time;
		}
#if !defined(WIFI_PRE_REDIRECT)
		iptables_del_ifOnline();
#endif
#endif

	}

	void mark_offline() {
#if (WIFIDOG_MAKE_MONEY == FYES)
#else
		int before;
		int after;

		before = is_online();
		time(&last_offline_time);
		after = is_online();

		if (before != after) {
			debug(LOG_INFO, "ONLINE status became %s", (after ? "ON" : "OFF"));
		}
#if 0
		if(last_online_time == 0) {
		    last_online_time = last_offline_time;
		}
#endif

		/* If we're offline it definately means the auth server is offline */
		mark_auth_offline();
#endif
	}

	int is_online() {
#if (WIFIDOG_MAKE_MONEY == FYES)
		return (1);
#else
		debug(LOG_DEBUG, "auth_off_flag: %d, last_online_time: %d, last_offline_time: %d ",auth_off_flag,last_online_time,last_offline_time);
#if 1
		if (last_online_time == 0 || (last_offline_time - last_online_time) >= (config_get_config()->checkinterval * 2) ) {
#else
		/*设备刚启动，默认网络在线*/
		if ((last_offline_time - last_online_time) >= (config_get_config()->checkinterval * 2) ) {
#endif
			/* We're probably offline */
			return (0);
		}
		else {
			/* We're probably online */
			return (1);
		}
#endif
	}

	void mark_auth_online() {
#if (WIFIDOG_MAKE_MONEY == FYES)
		auth_off_flag = 0;
#else
		int before;
		int after;

		before = is_auth_online();
		time(&last_auth_online_time);
		after = is_auth_online();

		auth_off_flag = 0;
		if (before != after) {
			debug(LOG_INFO, "AUTH_ONLINE status became %s", (after ? "ON" : "OFF"));
		}

		/* If auth server is online it means we're definately online */
		mark_online();
		iptables_add_weixin_ipset();
#endif

	}

	void mark_auth_offline() {
#if (WIFIDOG_MAKE_MONEY == FYES)
		auth_off_flag = 1;
#else
		int before;
		int after;

		before = is_auth_online();
		time(&last_auth_offline_time);
		after = is_auth_online();

		auth_off_flag = 1;
		if (before != after) {
			debug(LOG_INFO, "AUTH_ONLINE status became %s", (after ? "ON" : "OFF"));
		}
		if(last_auth_online_time == 0) {
		    last_auth_online_time = last_auth_offline_time;
		}
#endif

	}

	int is_auth_online() {
#if (WIFIDOG_MAKE_MONEY == FYES)
			return (1);
#else
		debug(LOG_DEBUG, "auth_off_flag: %d,last_auth_online_time: %d, last_auth_offline_time: %d ",auth_off_flag,last_auth_online_time,last_auth_offline_time);
		if (!is_online()) {
			/* If we're not online auth is definately not online :) */
			return (0);
		}
#if 0
		else if (last_auth_online_time == 0 || (last_auth_offline_time - last_auth_online_time) >= (config_get_config()->checkinterval * 2) ) {
#else
		/*设备刚启动，默认服务器在线*/
		else if ((last_auth_offline_time - last_auth_online_time) >= (config_get_config()->checkinterval * 2) ) {
#endif
			/* Auth is  probably offline */
			return (0);
		}
		else if((auth_off_flag == 1)&&((last_auth_offline_time - last_auth_online_time) >= (3*10))) {
			/* Auth is  probably offline */
			return (0);
		}
		else if(authEn_ping != 1) {
			/* Auth is  probably offline */
			return (0);
		}
		else {
			/* Auth is probably online */
			return (1);
		}
#endif
	}

	/*
	 * @return A string containing human-readable status text. MUST BE free()d by caller
	 */
	char * get_status_text() {
		char buffer[STATUS_BUF_SIZ];
		ssize_t len;
		s_config *config;
		t_auth_serv *auth_server;
		t_client	*first;
		int		count=0;
		unsigned long int uptime = 0;
		unsigned int days = 0, hours = 0, minutes = 0, seconds = 0;
		t_trusted_mac *p;

		len = 0;
		snprintf(buffer, (sizeof(buffer) - len), "UTT WiFi status\n\n");
		len = strlen(buffer);

		if((started_time < ASSIGNED_TIME) && (time(NULL) > ASSIGNED_TIME)) {
		    started_time = time(NULL);
		}
		uptime = time(NULL) - started_time;
		days    = uptime / (24 * 60 * 60);
		uptime -= days * (24 * 60 * 60);
		hours   = uptime / (60 * 60);
		uptime -= hours * (60 * 60);
		minutes = uptime / 60;
		uptime -= minutes * 60;
		seconds = uptime;

		snprintf((buffer + len), (sizeof(buffer) - len), "Version: " VERSION "\n");
		len = strlen(buffer);

		snprintf((buffer + len), (sizeof(buffer) - len), "Uptime: %ud %uh %um %us\n", days, hours, minutes, seconds);
		len = strlen(buffer);

		snprintf((buffer + len), (sizeof(buffer) - len), "Has been restarted: ");
		len = strlen(buffer);
		if (restart_orig_pid) {
			snprintf((buffer + len), (sizeof(buffer) - len), "yes (from PID %d)\n", restart_orig_pid);
			len = strlen(buffer);
		}
		else {
			snprintf((buffer + len), (sizeof(buffer) - len), "no\n");
			len = strlen(buffer);
		}

		snprintf((buffer + len), (sizeof(buffer) - len), "Internet Connectivity: %s\n", (is_online() ? "yes" : "no"));
		len = strlen(buffer);

		snprintf((buffer + len), (sizeof(buffer) - len), "Auth server reachable: %s\n", (is_auth_online() ? "yes" : "no"));
		len = strlen(buffer);

		snprintf((buffer + len), (sizeof(buffer) - len), "Clients served this session: %lu\n\n", served_this_session);
		len = strlen(buffer);

		//LOCK_CLIENT_LIST();

		first = client_get_first_client();

		if (first == NULL) {
			count = 0;
		} else {
			if(first->need_notify != 1)  /*不包括前置下线的client*/
			count = 1;
			while (first->next != NULL) {
				first = first->next;
				if(first->need_notify != 1) 
				count++;
			}
		}

		snprintf((buffer + len), (sizeof(buffer) - len), "%d clients "
				"connected.\n", count);
		len = strlen(buffer);

		first = client_get_first_client();

		count = 0;
		while (first != NULL) {
		    if(first->need_notify != 1) {
			snprintf((buffer + len), (sizeof(buffer) - len), "\nClient %d\n", count);
			len = strlen(buffer);

			snprintf((buffer + len), (sizeof(buffer) - len), "  IP: %s MAC: %s\n", first->ip, first->mac);
			len = strlen(buffer);

			snprintf((buffer + len), (sizeof(buffer) - len), "  Token: %s\n", first->token);
			len = strlen(buffer);

			snprintf((buffer + len), (sizeof(buffer) - len), "  Downloaded: %llu\n  Uploaded: %llu\n" , first->counters.incoming, first->counters.outgoing);
			len = strlen(buffer);

			count++;
		    }
			first = first->next;
		}

		//UNLOCK_CLIENT_LIST();

		config = config_get_config();

		if (config->trustedmaclist != NULL) {
			snprintf((buffer + len), (sizeof(buffer) - len), "\nTrusted MAC addresses:\n");
			len = strlen(buffer);

			for (p = config->trustedmaclist; p != NULL; p = p->next) {
				snprintf((buffer + len), (sizeof(buffer) - len), "  %s\n", p->mac);
				len = strlen(buffer);
			}
		}

		snprintf((buffer + len), (sizeof(buffer) - len), "\nAuthentication servers:\n");
		len = strlen(buffer);

		//LOCK_CONFIG();

		for (auth_server = config->auth_servers; auth_server != NULL; auth_server = auth_server->next) {
			snprintf((buffer + len), (sizeof(buffer) - len), "  Host: %s (%s)\n", auth_server->authserv_hostname, auth_server->last_ip);
			len = strlen(buffer);
		}
		t_secondary_auth_server *sec_server;
		for (sec_server = config->secondary_auth_server; sec_server != NULL;sec_server = sec_server->next){
		    snprintf((buffer + len), (sizeof(buffer) - len), "  Other_Host: %s (%s)\n", sec_server->hostname, sec_server->ip);
		    len = strlen(buffer);
		}

		//UNLOCK_CONFIG();

		return safe_strdup(buffer);
	}
#if(WIFIDOG_MAC_AUTH == FYES)
/*
 *使ARP信息发生变更，内核发送MAC变更消息
 */
	static int arp_del(int sock,char *ip) 
	{
	    struct arpreq arpreq;
	    struct sockaddr_in *sin;
	    struct in_addr ina;
	    int rc;
	    unsigned char *hw_addr;

	    debug(LOG_DEBUG,"del arp entry for IP : %s\n", ip);

	    /*you must add this becasue some system will return "Invlid argument"
	      because some argument isn't zero */
	    memset(&arpreq, 0, sizeof(struct arpreq));

	    sin = (struct sockaddr_in *) &arpreq.arp_pa;
	    memset(sin, 0, sizeof(struct sockaddr_in));
	    sin->sin_family = AF_INET;
	    ina.s_addr = inet_addr(ip);
	    memcpy(&sin->sin_addr, (char *) &ina, sizeof(struct in_addr));

	    strcpy(arpreq.arp_dev, getLanIfName());
	    rc = ioctl(sock, SIOCDARP, &arpreq);
	    hw_addr = (unsigned char *) arpreq.arp_ha.sa_data;
	    debug(LOG_DEBUG, "HWAddr found : %02x:%02x:%02x:%02x:%02x:%02x\n",hw_addr[0], hw_addr[1], hw_addr[2], hw_addr[3], hw_addr[4],hw_addr[5]);
	    if (rc < 0)
	    {
		debug(LOG_ERR, "del arp error....");
		return -1;
	    }

	    return 0;
	}
	int arpctl(char * ip,char opt)
	{
	    int sd;
	    sd = socket(AF_INET, SOCK_DGRAM, 0);
	    if (sd < 0)
	    {
		perror("socket() error\n");
		exit(1);
	    }
	    if(opt == 'd'){
		arp_del(sd,ip);
	    }
	    close(sd);
	}
#endif
int  isLocalIP(char *ip)
{
    char *localIP;
    int result =0;
    localIP=get_iface_ip(getLanIfName());
    if(ip != NULL && localIP !=NULL && strcmp(localIP,ip) == 0)
    {
	result = 1;
    }
    free(localIP);
    return result;
}
int  urlCheck(char *url,int len){
    char * urlpro,*tmpurl;
    urlpro = strstr(url,"://");
    if(urlpro == NULL){
	urlpro = strstr(url,":/");
	if(urlpro != NULL){
	    tmpurl=(char*)safe_malloc(strlen(url)+2);
	    strncpy(tmpurl,url,urlpro+2-url);
	    tmpurl[urlpro+2-url] = '\0';
	    strcat(tmpurl,"/");
	    strcat(tmpurl,urlpro+2);
	    strncpy(url,tmpurl,len);
	    if(tmpurl!=NULL){
		free(tmpurl);
	    }
	    return 1;
	}else{
	    return 2;
	}
    }else{
	return 1;
    }
    return 0;
}

