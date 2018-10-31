#ifndef _QUEUE_MULTI__H
#define _QUEUE_MULTI_H

#ifdef CONN_MULTI_QUEUE
#include "httpd.h"

#define MAX_CONN_MULTI_QUEUE 500+2

typedef struct st_Queue_t {
    request *conns[MAX_CONN_MULTI_QUEUE];
    int head;
    int tail;
    int index;
} Queue;

#define QUEUE_COUNT 3
Queue queue[QUEUE_COUNT];

static pthread_t thread_pid[QUEUE_COUNT];

void queue_init(int index);
int queue_is_full(int index);
int queue_is_empty(int index);
void queue_enter(request *r,int index);
void queue_delete(request **r, int index);
int queue_length(int index);

#endif
#endif
