#if (P2P_LIMIT == FYES)
#ifndef MIBP2PLIMIT_SHARE_H
#define MIBP2PLIMIT_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

typedef enum{
    EXCLUSIVE_RATE_P2P = 1,/*独享带宽*/
    SHARE_RATE_P2P = 2 /*共享带宽*/
}P2P_LIMIT_POLICY;

/*
 * limitPolicy - 限速策略，独享或共享
 * uRate - 上传速率限速
 * dRate - 下载速率限速
 * day - 星期
 * timeStart - 起始时间
 * timeStop - 结束时间
 */
typedef struct st_p2pLimit_profile
{
    mibHead head;
    P2P_LIMIT_POLICY limitPolicy;/*限速策略*/
    Uint32 uRate;
    Uint32 dRate;
#if (IP_GRP == FYES)
    char ipGrpName[UTT_INST_NAME_LEN + 1u];/*地址组名称*/
#endif
    UINT32 day;
    char timeStart[UTT_TIME_LEN];
    char timeStop[UTT_TIME_LEN];
}P2PLimitProfile;
#endif
#endif
