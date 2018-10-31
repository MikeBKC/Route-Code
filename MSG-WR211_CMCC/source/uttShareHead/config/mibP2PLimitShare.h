#if (P2P_LIMIT == FYES)
#ifndef MIBP2PLIMIT_SHARE_H
#define MIBP2PLIMIT_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

typedef enum{
    EXCLUSIVE_RATE_P2P = 1,/*�������*/
    SHARE_RATE_P2P = 2 /*�������*/
}P2P_LIMIT_POLICY;

/*
 * limitPolicy - ���ٲ��ԣ��������
 * uRate - �ϴ���������
 * dRate - ������������
 * day - ����
 * timeStart - ��ʼʱ��
 * timeStop - ����ʱ��
 */
typedef struct st_p2pLimit_profile
{
    mibHead head;
    P2P_LIMIT_POLICY limitPolicy;/*���ٲ���*/
    Uint32 uRate;
    Uint32 dRate;
#if (IP_GRP == FYES)
    char ipGrpName[UTT_INST_NAME_LEN + 1u];/*��ַ������*/
#endif
    UINT32 day;
    char timeStart[UTT_TIME_LEN];
    char timeStop[UTT_TIME_LEN];
}P2PLimitProfile;
#endif
#endif
