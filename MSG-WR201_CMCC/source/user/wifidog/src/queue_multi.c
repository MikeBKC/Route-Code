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
#include "queue_multi.h"
#ifdef CONN_MULTI_QUEUE

void 
queue_init(int index)
{
    int i=0;
    if((index>=0) && (index < QUEUE_COUNT)) {
	memset(&queue[index], 0, sizeof(Queue));
	queue[index].head = -1;
	queue[index].tail = 0;
    } else {
	memset(queue, 0, sizeof(queue));
	for(i=0;i<QUEUE_COUNT;i++) {
	queue[i].head = -1;
	queue[i].tail = 0;
	}
    }
    return;
}

int 
queue_is_full(int index)
{
    int flag = 0;

    if(((queue[index].head > queue[index].tail) && (queue[index].tail+1 == queue[index].head))
	    || (((queue[index].tail+1) == MAX_CONN_MULTI_QUEUE) && (queue[index].head == 0)))
    {
	flag = 1;
    }

    return flag;
}

int 
queue_is_empty(int index)
{
    int flag = 0;

    if(((queue[index].tail > queue[index].head) && (queue[index].head+1 == queue[index].tail)) 
	    || (((queue[index].head+1) == MAX_CONN_MULTI_QUEUE) && (queue[index].tail == 0)))
    {
	flag = 1;
    }

    return flag;
}

void 
queue_enter(request *r,int index)
{
    int next_index = 0;
    int enter_index = 0;
    if(r != NULL) {
	if(queue_is_full(index) == 0)
	{
	    debug(LOG_DEBUG, "enter client lientAddr: %s, queue index %d +++++",r->clientAddr,index);
	    enter_index = queue[index].tail;
	    if(queue[index].tail+1 == MAX_CONN_MULTI_QUEUE)
	    {
		next_index = 0;
	    } else {
		next_index = queue[index].tail+1;
	    }
	    debug(LOG_DEBUG, "queue[%d] head: %d, tail: %d",index,queue[index].head,queue[index].tail);
	    queue[index].conns[enter_index] = r;
	    queue[index].tail = next_index;
#if 0
	    if(queue[index].conns[enter_index] == NULL) {
	    debug(LOG_DEBUG, "queue[%d] enter tail is null =====",index);
	    }else{
	    debug(LOG_DEBUG, "queue[%d] tail enter_index: %d, clientSock: %d, lientAddr: %s",index,enter_index,queue[index].conns[queue[index].tail-1]->clientSock,queue[index].conns[queue[index].tail-1]->clientAddr);
	    }
	    if(queue[index].head == -1)
	    {
		queue[index].head = 0;
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
queue_delete(request **r, int index)
{
    int next_index = 0;
    if(queue_is_empty(index) ==0) 
    {

	    if(queue[index].head+1 == MAX_CONN_MULTI_QUEUE)
	    {
		next_index = 0;
	    } else {
		next_index = queue[index].head+1;
	    }
	    //debug(LOG_DEBUG, "befor, queue[%d] head: %d, tail: %d",index,queue[index].head,queue[index].tail);
	    *r = queue[index].conns[next_index];
	    queue[index].head = next_index;
	    debug(LOG_DEBUG, "after, queue[%d] head: %d, tail: %d",index,queue[index].head,queue[index].tail);
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
queue_length(int index)
{
    int count = 0;
#if 1
    if(queue_is_empty(index) ==0) {}
    if(queue[index].tail > queue[index].head) {
	count = (queue[index].tail - queue[index].head - 1);
    } else {
	count = MAX_CONN_MULTI_QUEUE - (queue[index].head - queue[index].tail + 1);
    }
    //debug(LOG_DEBUG, "queue[%d] head: %d, queue tail: %d",index,queue[index].head,queue[index].tail);
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
