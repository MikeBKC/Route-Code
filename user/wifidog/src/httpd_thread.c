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

/* $Id: httpd_thread.c,v 1.4 2014/11/08 11:32:52 guo.deyuan Exp $ */

/** @file httpd_thread.c
    @brief Handles on web request.
    @author Copyright (C) 2004 Alexandre Carmel-Veilleux <acv@acv.ca>
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <errno.h>

#include "httpd.h"

#include "../config.h"
#include "common.h"
#include "debug.h"
#include "httpd_thread.h"
#ifdef CONN_QUEUE
#include "queue.h"
#endif
#ifdef CONN_MULTI_QUEUE
#include "queue_multi.h"
#endif

#ifdef CONN_QUEUE
extern Queue queue;
#endif
/** Main request handling thread.
@param args Two item array of void-cast pointers to the httpd and request struct
*/
void
thread_httpd(void *args)
{
#ifdef CONN_MULTI_QUEUE
	void	**params;
	httpd	*webserver;
	request	*r = NULL;
	//static int index = 0;
	int index= -1;
	
	params = (void **)args;
	webserver = *params;
	index = *((int *)(*(params+1)));
	debug(LOG_DEBUG, "index [%d] (httd)",index);
	if(((index>=0)&&(index<QUEUE_COUNT))) {
	    debug(LOG_DEBUG, "create pthread index: %d",index);
	} else {
	    debug(LOG_ERR, "[ERR] create pthread error, index [%d], and exiting...",index);
	    return;
	}
	//r = *(params + 1);
	//free(params); /* XXX We must release this ourselves. */
	debug(LOG_DEBUG, "connection queue length: %d",queue_length(index));
	while(1) {
	    queue_delete(&r, index);
	    if(r != NULL) {
	
#if 0
		if (httpdReadRequest(webserver, r) == 0) {
#endif
		    /*
		     * We read the request fine
		     */
		    debug(LOG_DEBUG, "Processing request from %s, queue index %d", r->clientAddr,index);
		    //debug(LOG_ERR, "Calling httpdProcessRequest() for %s", r->clientAddr);
		    httpdProcessRequest(webserver, r);
		    //debug(LOG_ERR, "Returned from httpdProcessRequest() for %s", r->clientAddr);
#if 0
		}
		else {
		    debug(LOG_DEBUG, "No valid request received from %s", r->clientAddr);
		}
#endif
		debug(LOG_DEBUG, "Closing connection with %s, queue index %d", r->clientAddr,index);
		httpdEndRequest(r);
	    } else {
		usleep(10000); /*10ms, 即0.1s*/
	    }
	}
#else /*CONN_MULTI_QUEUE*/
#ifdef CONN_QUEUE
	void	**params;
	httpd	*webserver;
	request	*r = NULL;
	
	params = (void **)args;
	webserver = *params;
	//r = *(params + 1);
	//free(params); /* XXX We must release this ourselves. */
	debug(LOG_DEBUG, "connection queue length: %d",queue_length());
	while(1) {
	    queue_delete(&r);
	    if(r != NULL) {
	
#if 0
		if (httpdReadRequest(webserver, r) == 0) {
#endif
		    /*
		     * We read the request fine
		     */
		    debug(LOG_DEBUG, "Processing request from %s", r->clientAddr);
		    //debug(LOG_DEBUG, "Calling httpdProcessRequest() for %s", r->clientAddr);
		    httpdProcessRequest(webserver, r);
		    //debug(LOG_DEBUG, "Returned from httpdProcessRequest() for %s", r->clientAddr);
#if 0
		}
		else {
		    debug(LOG_DEBUG, "No valid request received from %s", r->clientAddr);
		}
#endif
		debug(LOG_DEBUG, "Closing connection with %s", r->clientAddr);
		httpdEndRequest(r);
	    } else {
		usleep(100000); /*100ms, 即0.1s*/
	    }
	}
#else
	void	**params;
	httpd	*webserver;
	request	*r;
	
	params = (void **)args;
	webserver = *params;
	r = *(params + 1);
	free(params); /* XXX We must release this ourselves. */
	
	if (httpdReadRequest(webserver, r) == 0) {
		/*
		 * We read the request fine
		 */
		debug(LOG_DEBUG, "Processing request from %s", r->clientAddr);
		//debug(LOG_DEBUG, "Calling httpdProcessRequest() for %s", r->clientAddr);
		httpdProcessRequest(webserver, r);
		//debug(LOG_DEBUG, "Returned from httpdProcessRequest() for %s", r->clientAddr);
	}
	else {
		debug(LOG_DEBUG, "No valid request received from %s", r->clientAddr);
	}
	debug(LOG_DEBUG, "Closing connection with %s", r->clientAddr);
	httpdEndRequest(r);
#endif
#endif /*end CONN_MULTI_QUEUE*/
}
