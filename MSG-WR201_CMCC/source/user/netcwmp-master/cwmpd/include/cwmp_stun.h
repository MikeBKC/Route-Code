/*
 * auth liwenchang
 * 2014-05-28
 * */

#ifndef __CWMP_STUN_H__
#define __CWMP_STUN_H__

#include "cwmp_module.h"

#define STUN_MAX_LEN 1024

struct stun_pkt {
    unsigned short type;
    unsigned short length;
    unsigned int transId1;
    unsigned int transId2;
    unsigned int transId3;
    unsigned int transId4;
    unsigned char attributes[STUN_MAX_LEN];
};

struct stun_attr {
    unsigned short type;
    unsigned short length;
    unsigned char  value[STUN_MAX_LEN];
};

struct stun_mapped_address{
    unsigned padding : 8;
    unsigned family : 8;
    unsigned short port;
    unsigned char address[4];
};

int stun_task_build(cwmp_t * cwmp);


#endif

