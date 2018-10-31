#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "typedef.h"
#include "mib.h"
#include "profacce.h"
#include "comExeFun.h"

#if (WIFIDOG_MAKE_MONEY == FYES)
#define MAX_FREE_ERP_LIST 1024
#define HASH_KEY 255

/*
typedef struct st_MacAddr
{
    unsigned char addr[6];
}MacAddr;

typedef struct st_FreeExp
{
    MacAddr mac;
    unsigned int time;
}FreeExp;
*/

struct hash_list_node
{
    FreeExp *mac_data;
    struct hash_list_node *next;
};


//int macAddrCmp(MacAddr *mac1,MacAddr *mac2);
extern void init_hash();
extern void initFreeHash();
extern int store_array_hash(MacAddr mac);
#endif
