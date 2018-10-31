#ifndef MIBACMODE_SHARE_H
#define MIBACMODE_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

#if (AC_WORK_MODE == FYES)  
typedef struct st_ac_work_mode_profile {
    mibHead head;
    ProfileEnableEnum workMode;
    IPADDR gateWay;
    IPADDR pDns;
}AcWorkModeProfile;
#endif

#endif
