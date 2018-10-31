#ifndef APLOADBALANCE_SHARE_H
#define APLOADBALANCE_SHARE_H

#include "mibunionShare.h"
#include "mibtypeShare.h"

#if (AP_LOAD_BALANCE == FYES)

#define BAL_NUM 5  /*���ؾ����������AP������Ҫ�����ģ����Ļ�Ҫ����cliaaa.c�ļ�������*/
#define MAX_DEV_NAME_LEN_AC          12u  /*�豸��*/

typedef struct _Balance {
    MacAddr macAddr;
    char dev_name[MAX_DEV_NAME_LEN_AC];
    Uint32 sn;
    Uint32 isAlive;  /*��ǰAP�Ƿ�����*/
    Uint32 client;  /*��ǰAP�ϵĿͻ�����*/
    Uint32 isFull;  /*��ǰAP�Ƿ�����STA����*/
} Balance;

typedef struct _st_apLoadBalance_profile {
    mibHead head;
	ProfileEnableEnum lbEn;
    Uint32 num;            /*�˸��ؾ������ж��ٸ�AP*/
    Uint32 threshold;       /*���ؾ�����ֵ*/
    Uint32 source_sum;       /*���ؾ����ֵ*/
    Balance ap[BAL_NUM];   /*����AP*/
} ApLoadBalanceProfile;

#endif

#endif
