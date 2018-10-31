#ifndef MIBPARENTCONTROL_SHARE_H
#define MIBPARENTCONTROL_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"
#if (PARENTS_CONTROL == FYES)
#define MAX_ALLOW_URL_ITEM 10U
#define MAX_URL_NAME_LEN 46U

typedef struct st_allowurl {
    char url[MAX_URL_NAME_LEN+1];
}AllowURL;
/* */
typedef struct st_parentsControlProfile {
    mibHead head;
    MacAddr mac;	/*小孩 MAC*/
#if (TIME_GRP == FYES)
    char timeGrp[UTT_INST_NAME_LEN + 1u];/*生效时间组*/
#endif
#if (UTT_SMART_UI == FYES)
    UINT32 day;
    char timeStart[UTT_TIME_LEN];
    char timeStop[UTT_TIME_LEN];
#endif
    AllowURL allowURL[MAX_ALLOW_URL_ITEM];

} ParentsControlProfile;
#endif
#endif
