#ifndef _QUEUE_H
#define _QUEUE_H

#ifdef CONN_QUEUE
#include "httpd.h"

#define MAX_CONN_QUEUE 1000+2

typedef struct st_Queue_t {
    request *conns[MAX_CONN_QUEUE];
    int head;
    int tail;
} Queue;
Queue queue;

void queue_init(void);
int queue_is_full(void);
int queue_is_empty(void);
void queue_enter(request *r);
void queue_delete(request **r);
int queue_length(void);

#endif
#endif
