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

/* $Id: wdctl_thread.c,v 1.9.8.1 2015/06/23 15:57:35 hou.bo Exp $ */
/** @file wdctl_thread.c
    @brief Monitoring and control of wifidog, server part
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
#include <signal.h>
#include <errno.h>

#include "common.h"
#include "httpd.h"
#include "util.h"
#include "conf.h"
#include "debug.h"
#include "auth.h"
#include "centralserver.h"
#include "fw_iptables.h"
#include "firewall.h"
#include "client_list.h"
#include "wdctl_thread.h"
#include "gateway.h"
#include "safe.h"

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "profacce.h"
#include "comExeFun.h"
#include "flash_api.h"
/* Defined in clientlist.c */
extern	pthread_mutex_t	client_list_mutex;
extern	pthread_mutex_t	config_mutex;

extern char maclist[];
/* From commandline.c: */
extern char ** restartargv;
static void *thread_wdctl_handler(void *);
#if (WIFIDOG_MAKE_MONEY != FYES)
static void wdctl_exceptipgroup(int fd, char *ipPtr);
#endif
static void wdctl_ipdown(int fd, char *ipPtr);
#if 0
/*dele wdctl_client*/
static void wdctl_client(int fd);
#endif
static void wdctl_status(int);
static void wdctl_stop(int);
static void wdctl_reset(int, char *);
static void wdctl_restart(int);
static void wdctl_debug(int fd , char * argv);
#if 0
static void wdctl_show(int fd, char *arg);
#endif

/** Launches a thread that monitors the control socket for request
@param arg Must contain a pointer to a string containing the Unix domain socket to open
@todo This thread loops infinitely, need a watchdog to verify that it is still running?
*/  
void
thread_wdctl(void *arg)
{
	int	fd;
	char	*sock_name;
	struct 	sockaddr_un	sa_un;
	int result;
	pthread_t	tid;
	socklen_t len;

	debug(LOG_DEBUG, "Starting wdctl.");

	memset(&sa_un, 0, sizeof(sa_un));
	sock_name = (char *)arg;
	debug(LOG_DEBUG, "Socket name: %s", sock_name);

	if (strlen(sock_name) > (sizeof(sa_un.sun_path) - 1)) {
		/* TODO: Die handler with logging.... */
		debug(LOG_ERR, "WDCTL socket name too long");
		exit(1);
	}
	

	debug(LOG_DEBUG, "Creating socket");
	wdctl_socket_server = socket(PF_UNIX, SOCK_STREAM, 0);

	debug(LOG_DEBUG, "Got server socket %d", wdctl_socket_server);

	/* If it exists, delete... Not the cleanest way to deal. */
	unlink(sock_name);

	debug(LOG_DEBUG, "Filling sockaddr_un");
	strcpy(sa_un.sun_path, sock_name); /* XXX No size check because we
					    * check a few lines before. */
	sa_un.sun_family = AF_UNIX;
	
	debug(LOG_DEBUG, "Binding socket (%s) (%d)", sa_un.sun_path,
			strlen(sock_name));
	
	/* Which to use, AF_UNIX, PF_UNIX, AF_LOCAL, PF_LOCAL? */
	if (bind(wdctl_socket_server, (struct sockaddr *)&sa_un, strlen(sock_name) 
				+ sizeof(sa_un.sun_family))) {
		debug(LOG_ERR, "Could not bind control socket: %s",
				strerror(errno));
		pthread_exit(NULL);
	}

	if (listen(wdctl_socket_server, 5)) {
		debug(LOG_ERR, "Could not listen on control socket: %s",
				strerror(errno));
		pthread_exit(NULL);
	}

	while (1) {
		len = sizeof(sa_un); 
		memset(&sa_un, 0, len);
		if ((fd = accept(wdctl_socket_server, (struct sockaddr *)&sa_un, &len)) == -1){
			debug(LOG_ERR, "Accept failed on control socket: %s",
					strerror(errno));
		} else {
			debug(LOG_DEBUG, "Accepted connection on wdctl socket %d (%s)", fd, sa_un.sun_path);
			result = pthread_create(&tid, NULL, &thread_wdctl_handler, (void *)fd);
			if (result != 0) {
				debug(LOG_ERR, "FATAL: Failed to create a new thread (wdctl handler) - exiting");
				termination_handler(0);
			}
			pthread_detach(tid);
		}
	}
}


static void *
thread_wdctl_handler(void *arg)
{
	int	fd,
		done,
		i;
	char	request[MAX_BUF];
	ssize_t	read_bytes,
		len;

	debug(LOG_DEBUG, "Entering thread_wdctl_handler....");

	fd = (int)arg;
	
	debug(LOG_DEBUG, "Read bytes and stuff from %d", fd);

	/* Init variables */
	read_bytes = 0;
	done = 0;
	memset(request, 0, sizeof(request));
	
	/* Read.... */
	while (!done && read_bytes < (sizeof(request) - 1)) {
		len = read(fd, request + read_bytes,
				sizeof(request) - read_bytes);

		/* Have we gotten a command yet? */
		for (i = read_bytes; i < (read_bytes + len); i++) {
			if (request[i] == '\r' || request[i] == '\n') {
				request[i] = '\0';
				done = 1;
			}
		}
		
		/* Increment position */
		read_bytes += len;
	}

	if (strncmp(request, "status", 6) == 0) {
		wdctl_status(fd);
	} else if (strncmp(request, "stop", 4) == 0) {
		wdctl_stop(fd);
	} else if (strncmp(request, "reset", 5) == 0) {
		wdctl_reset(fd, (request + 6));
	} else if (strncmp(request, "restart", 7) == 0) {
		wdctl_restart(fd);
	}
#if(WIFIDOG_MAKE_MONEY != FYES)
	else if (strncmp(request, "exceptipgroup", 13) == 0) {
		wdctl_exceptipgroup(fd, request+14);
	}
#endif
	else if(strncmp(request, "ipDown", 6) ==0) {
		wdctl_ipdown(fd, request+7);
	}else if(strncmp(request, "debug", 5) ==0) {
		wdctl_debug(fd, request+6);
	}
#if 0
/*dele wdctl_client*/
	else if (strncmp(request, "client", 6) == 0) {
		wdctl_client(fd);
	}
#endif
#if 0
	else if (strncmp(request, "show", 4) == 0) {
	    wdctl_show(fd, request + 5);
	}
#endif

	if (!done) {
		debug(LOG_ERR, "Invalid wdctl request.");
		shutdown(fd, 2);
		close(fd);
		pthread_exit(NULL);
	}

	debug(LOG_DEBUG, "Request received: [%s]", request);
	
	shutdown(fd, 2);
	close(fd);
	debug(LOG_DEBUG, "Exiting thread_wdctl_handler....");

	return NULL;
}

#if (WIFIDOG_MAKE_MONEY != FYES)
static void 
wdctl_exceptipgroup(int fd, char *ipPtr)
{
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;
    char buf[] = "ok";
    char *ptr = NULL;

    //ProfInit();
    profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
    if((ipPtr != NULL)&&(strlen(ipPtr)>0)) {
    ptr = strstr(ipPtr, ";");
    if(ptr != NULL) {
	*ptr = '\0';
	debug(LOG_DEBUG, "del except ip group: %s",ipPtr);
	if(strlen(ipPtr)>0) {
	    iptables_fw_add_del_exceptIp(IPTABLE_DEL, ipPtr);
	}

    }
    }
    /*web认证不开启不会用到计费模式返回0*/
    if((profG != NULL) && (ProfInstIsFree(profG) == 0))
    {
	if(profG->exceptIpGroup[0] != '\0') {
	    debug(LOG_DEBUG, "add except ip group: %s",ipPtr);
	    iptables_fw_add_del_exceptIp(IPTABLE_ADD, profG->exceptIpGroup);
	}
    }
    //ProfDetach();
    //write(fd, buf, strlen(buf));
}
#endif
static void 
wdctl_ipdown(int fd, char *ipPtr)
{
    char buf[] = "ok";
    char *ptr = NULL;
    t_client	*node=NULL;
    int length = 0;

    if((ipPtr != NULL)&&(strlen(ipPtr)>0)) {
	ptr = strstr(ipPtr, ";");
	if(ptr != NULL) {
	    *ptr = '\0';
	    debug(LOG_DEBUG, "%s: del ip: %s",__func__,ipPtr);
	    if((strlen(ipPtr)>=7)&&(strlen(ipPtr)<=15)) {
		if ((node = client_list_find_by_ip(ipPtr)) != NULL) {
		    node->runtime = node->expire;
		    if(node->mac!=NULL) {
			length = strlen(maclist);
			if(length<(MAX_MACLIST_LEN-18)) {
			snprintf((maclist+length),(MAX_MACLIST_LEN-length),"%s;",convertMac(node->mac,0));
			}
		    }
		}
	    }
	}
    }
    //write(fd, buf, strlen(buf));
}

#if 0
/*dele wdctl_client*/
static void 
wdctl_client(int fd)
{
    t_client *node = NULL;
    int count = 0;
    char buf[4096];
    int buflen = 0;
    int writelen = 0;

    //LOCK_CLIENT_LIST();
    //_httpd_net_write( r->clientSock, buf, strlen(buf));
    //write(fd, buf, strlen(buf));

    node = client_get_first_client();

    memset(buf, 0, sizeof(buf));
    while (node != NULL) {
	if(node->need_notify != 1) {
	buflen = strlen(buf);
	snprintf(buf+buflen,(4096-buflen),"ipaddr[%d]=\"%s\";macaddr[%d]=\"%s\";totaltime[%d]=%d;usedtime[%d]=%d;freeAuth[%d]=%d;",
		count,node->ip,count,node->mac,count,node->expire,count,node->runtime,count,node->free_auth_client);
	count++;
	}
	node = node->next;
	if(count%10 == 0) {
	    write(fd, buf, strlen(buf));
	    debug(LOG_DEBUG, "%s-%d:write client buffer: %s",__func__,__LINE__,buf);
	    memset(buf, 0, sizeof(buf));
	}
    }
    buflen = strlen(buf);
    snprintf(buf+buflen,4096-buflen, "totalrecs=%d;\n",count);
    //UNLOCK_CLIENT_LIST();
    debug(LOG_DEBUG, "%s-%d:write client buffer: %s",__func__,__LINE__,buf);
    write(fd, buf, strlen(buf));
    memset(buf, 0, sizeof(buf));
    return ;
}
#endif

static void
wdctl_status(int fd)
{
	char * status = NULL;
	int len = 0;

	status = get_status_text();
	len = strlen(status);

	write(fd, status, len);

	free(status);
}

/** A bit of an hack, self kills.... */
static void
wdctl_stop(int fd)
{
	pid_t	pid;

	pid = getpid();
	kill(pid, SIGINT);
}

static void
wdctl_restart(int afd)
{
	int	sock,
		fd;
	char	*sock_name;
	struct 	sockaddr_un	sa_un;
	s_config * conf = NULL;
	t_client * client = NULL;
	char * tempstring = NULL;
	pid_t pid;
	ssize_t written;
	socklen_t len;

	conf = config_get_config();

	debug(LOG_NOTICE, "Will restart myself");

	/*
	 * First, prepare the internal socket
	 */
	memset(&sa_un, 0, sizeof(sa_un));
	sock_name = conf->internal_sock;
	debug(LOG_DEBUG, "Socket name: %s", sock_name);

	if (strlen(sock_name) > (sizeof(sa_un.sun_path) - 1)) {
		/* TODO: Die handler with logging.... */
		debug(LOG_ERR, "INTERNAL socket name too long");
		return;
	}

	debug(LOG_DEBUG, "Creating socket");
	sock = socket(PF_UNIX, SOCK_STREAM, 0);

	debug(LOG_DEBUG, "Got internal socket %d", sock);

	/* If it exists, delete... Not the cleanest way to deal. */
	unlink(sock_name);

	debug(LOG_DEBUG, "Filling sockaddr_un");
	strcpy(sa_un.sun_path, sock_name); /* XXX No size check because we check a few lines before. */
	sa_un.sun_family = AF_UNIX;
	
	debug(LOG_DEBUG, "Binding socket (%s) (%d)", sa_un.sun_path, strlen(sock_name));
	
	/* Which to use, AF_UNIX, PF_UNIX, AF_LOCAL, PF_LOCAL? */
	if (bind(sock, (struct sockaddr *)&sa_un, strlen(sock_name) + sizeof(sa_un.sun_family))) {
		debug(LOG_ERR, "Could not bind internal socket: %s", strerror(errno));
		return;
	}

	if (listen(sock, 5)) {
		debug(LOG_ERR, "Could not listen on internal socket: %s", strerror(errno));
		return;
	}
	
	/*
	 * The internal socket is ready, fork and exec ourselves
	 */
	debug(LOG_DEBUG, "Forking in preparation for exec()...");
	pid = safe_fork();
	if (pid > 0) {
		/* Parent */

		/* Wait for the child to connect to our socket :*/
		debug(LOG_DEBUG, "Waiting for child to connect on internal socket");
		len = sizeof(sa_un);
		if ((fd = accept(sock, (struct sockaddr *)&sa_un, &len)) == -1){
			debug(LOG_ERR, "Accept failed on internal socket: %s", strerror(errno));
			close(sock);
			return;
		}

		close(sock);

		debug(LOG_DEBUG, "Received connection from child.  Sending them all existing clients");

		/* The child is connected. Send them over the socket the existing clients */
		LOCK_CLIENT_LIST();
		client = client_get_first_client();
		while (client) {
			/* Send this client */
			safe_asprintf(&tempstring, "CLIENT|ip=%s|mac=%s|token=%s|fw_connection_state=%u|fd=%d|counters_incoming=%llu|counters_outgoing=%llu|counters_last_updated=%lu\n", client->ip, convertMac(client->mac, 0), client->token, client->fw_connection_state, client->fd, client->counters.incoming, client->counters.outgoing, client->counters.last_updated);
			debug(LOG_DEBUG, "Sending to child client data: %s", tempstring);
			len = 0;
			while (len != strlen(tempstring)) {
				written = write(fd, (tempstring + len), strlen(tempstring) - len);
				if (written == -1) {
					debug(LOG_ERR, "Failed to write client data to child: %s", strerror(errno));
					free(tempstring);
					break;
				}
				else {
					len += written;
				}
			}
			free(tempstring);
			client = client->next;
		}
		UNLOCK_CLIENT_LIST();

		close(fd);

		debug(LOG_INFO, "Sent all existing clients to child.  Committing suicide!");

		shutdown(afd, 2);
		close(afd);

		/* Our job in life is done. Commit suicide! */
		wdctl_stop(afd);
	}
	else if(pid == 0) {
		/* Child */
		close(wdctl_socket_server);
		close(icmp_fd);
		close(sock);
		shutdown(afd, 2);
		close(afd);
		debug(LOG_NOTICE, "Re-executing myself (%s)", restartargv[0]);
		setsid();
		execvp(restartargv[0], restartargv);
		/* If we've reached here the exec() failed - die quickly and silently */
		debug(LOG_ERR, "I failed to re-execute myself: %s", strerror(errno));
		debug(LOG_ERR, "Exiting without cleanup");
		exit(1);
	}

}

static void
wdctl_reset(int fd, char *arg)
{
	t_client	*node;

	debug(LOG_DEBUG, "Entering wdctl_reset...");
	
	LOCK_CLIENT_LIST();
	debug(LOG_DEBUG, "Argument: %s (@%x)", arg, arg);
	
	/* We get the node or return... */
	if ((node = client_list_find_by_ip(arg)) != NULL);
	else if ((node = client_list_find_by_mac(arg)) != NULL);
	else {
		debug(LOG_DEBUG, "Client not found.");
		UNLOCK_CLIENT_LIST();
		write(fd, "No", 2);
		return;
	}

	debug(LOG_DEBUG, "Got node %x.", node);
	
	/* deny.... */
	/* TODO: maybe just deleting the connection is not best... But this
	 * is a manual command, I don't anticipate it'll be that useful. */
	fw_deny(node->ip, node->mac, node->fw_connection_state);
	client_list_delete(node);
	
	UNLOCK_CLIENT_LIST();
	
	write(fd, "Yes", 3);
	
	/*write client list file*/
	clist_list_write_file();
	debug(LOG_DEBUG, "Exiting wdctl_reset...");
}

#if 0
static void
client_show(t_client *node)
{
    if(node != NULL) {
	fprintf(stderr,"client ip: %s, mac: %s, token: %s\n",node->ip,node->mac,node->token);
	fprintf(stderr,"client counters incoming: %lu,  outgoing: %lu, last_uodated time: %u\n\n",
		node->counters.incoming,node->counters.outgoing,node->counters.last_updated);
    }
    return;
}
static void
wdctl_show(int fd, char *arg)
{
    t_client *node;
    char wrStr[20];	
    int count = 0;

    debug(LOG_DEBUG, "Entering wdctl_show...");
    
    LOCK_CLIENT_LIST();
    debug(LOG_DEBUG,"Argument: %s (@%x)",arg,arg);

    memset(wrStr, 0, 20);
    if(strcmp(arg, "All") == 0) {
	node = client_get_first_client();

	if (node == NULL) {
	    debug(LOG_DEBUG, "Client not found.");
	    UNLOCK_CLIENT_LIST();
	    write(fd, "no client find", 14);
	} else {
	    while (node != NULL) {
		client_show(node);
		node = node->next;
		count++;
	    }
	    snprintf(wrStr, 20, "%d clients find", count);
	    write(fd, wrStr, strlen(wrStr));
	}
    }else if(((node = client_list_find_by_ip(arg)) != NULL) || ((node = client_list_find_by_mac(arg)) != NULL)) {
	client_show(node);
	write(fd, "client find", 11);
    } else {
	debug(LOG_DEBUG, "Client not found.");
	write(fd, "no client find", 14);
    }
    UNLOCK_CLIENT_LIST();

    return;
}
#endif
static void wdctl_debug(int fd , char * argv)
{
	s_config * config = NULL;

	config = config_get_config();
	if(strncmp(argv,"on",2) == 0){
	    if(config->debuglevel == LOG_DEBUG){
		printf("debuging...\n");
		return ;
	    }  

	    FILE *fp = fopen("/tmp/wifidog_log","a+");
	    config->debuglevel = 7; 
	    if(fp == NULL){
		return ;
	    }

	    fseek(fp,0,SEEK_SET);
	    ftruncate(fileno(fp),0);
	    fprintf(fp,"%d\n",stderr);
	    stderr = fp ;

	}else{
	    config->debuglevel = DEFAULT_DEBUGLEVEL; 
	    if(stderr){
		FILE *tmp;
		fseek(stderr,0,SEEK_SET);
		fscanf(stderr,"%d",&tmp);
		fclose(stderr);
		stderr = tmp;
	    }
	}
}
