#ifndef APLOADBALANCE_SHARE_H
#define APLOADBALANCE_SHARE_H

#include "mibunionShare.h"
#include "mibtypeShare.h"

#if (AP_LOAD_BALANCE == FYES)

#define BAL_NUM 5  /*负载均衡组内最大AP数，不要随便更改，更改还要更改cliaaa.c文件中内容*/
#define MAX_DEV_NAME_LEN_AC          12u  /*设备名*/

typedef struct _Balance {
    MacAddr macAddr;
    char dev_name[MAX_DEV_NAME_LEN_AC];
    Uint32 sn;
    Uint32 isAlive;  /*当前AP是否在线*/
    Uint32 client;  /*当前AP上的客户端数*/
    Uint32 isFull;  /*当前AP是否允许STA关联*/
} Balance;

typedef struct _st_apLoadBalance_profile {
    mibHead head;
	ProfileEnableEnum lbEn;
    Uint32 num;            /*此负载均衡组有多少个AP*/
    Uint32 threshold;       /*负载均衡阈值*/
    Uint32 source_sum;       /*负载均衡差值*/
    Balance ap[BAL_NUM];   /*组内AP*/
} ApLoadBalanceProfile;

#endif

#endif
