#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <errno.h>

#include "debug.h"
#include "queue.h"
#ifdef CONN_QUEUE

void 
queue_init()
{
    memset(&queue, 0, sizeof(Queue));
    queue.head = -1;
    queue.tail = 0;
    return;
}

int 
queue_is_full()
{
    int flag = 0;

    if(((queue.head > queue.tail) && (queue.tail+1 == queue.head))
	    || (((queue.tail+1) == MAX_CONN_QUEUE) && (queue.head == 0)))
    {
	flag = 1;
    }

    return flag;
}

int 
queue_is_empty()
{
    int flag = 0;

    if(((queue.tail > queue.head) && (queue.head+1 == queue.tail)) 
	    || (((queue.head+1) == MAX_CONN_QUEUE) && (queue.tail == 0)))
    {
	flag = 1;
    }

    return flag;
}

void 
queue_enter(request *r)
{
    int next_index = 0;
    int enter_index = 0;
    if(r != NULL) {
	if(queue_is_full() == 0)
	{
	    debug(LOG_DEBUG, "enter client lientAddr: %s +++++",r->clientAddr);
	    enter_index = queue.tail;
	    if(queue.tail+1 == MAX_CONN_QUEUE)
	    {
		next_index = 0;
	    } else {
		next_index = queue.tail+1;
	    }
	    debug(LOG_DEBUG, "queue head: %d, tail: %d",queue.head,queue.tail);
	    queue.conns[enter_index] = r;
	    queue.tail = next_index;
#if 0
	    if(queue.conns[enter_index] == NULL) {
	    debug(LOG_DEBUG, "queue enter tail is null =====");
	    }else{
	    debug(LOG_DEBUG, "queue tail enter_index: %d, clientSock: %d, lientAddr: %s",enter_index,queue.conns[queue.tail-1]->clientSock,queue.conns[queue.tail-1]->clientAddr);
	    }
	    if(queue.head == -1)
	    {
		queue.head = 0;
	    }
	    if(head == NULL)
	    {
		tail = r;
		head = tail;
	    } else {
		tail->next = r;
		tail = r;
	    }
#endif
	} else {
	    httpdEndRequest(r); /*连接队列满，则释放此连接*/
	}
    }

    return;
}

void 
queue_delete(request **r)
{
    int next_index = 0;
    if(queue_is_empty() ==0) 
    {

	    if(queue.head+1 == MAX_CONN_QUEUE)
	    {
		next_index = 0;
	    } else {
		next_index = queue.head+1;
	    }
	    debug(LOG_DEBUG, "befor, queue head: %d, tail: %d",queue.head,queue.tail);
	    *r = queue.conns[next_index];
	    queue.head = next_index;
	    debug(LOG_DEBUG, "after, queue head: %d, tail: %d",queue.head,queue.tail);
#if 0
    if(head == NULL)
    {
	r = NULL;
    } else {
	r = head;

	head = head->next;
    }
#endif
    } else {
	*r = NULL;
    }
    return;
}

int 
queue_length()
{
    int count = 0;
#if 1
    if(queue_is_empty() ==0) {}
    if(queue.tail > queue.head) {
	count = (queue.tail - queue.head - 1);
    } else {
	count = MAX_CONN_QUEUE - (queue.head - queue.tail + 1);
    }
    debug(LOG_DEBUG, "queue head: %d, queue tail: %d",queue.head,queue.tail);
#else
    request *ptr = NULL;
    if(head != NULL)
    {
	ptr = head;

	while(ptr != NULL)
	{
	    count++;
	    ptr = ptr->next;
	}
    }
#endif
    return count;
}

#endif
