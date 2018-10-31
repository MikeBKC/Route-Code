#ifndef __APSOFTMNG_SHARE_H__
#define __APSOFTMNG_SHARE_H__

#include "mibunionShare.h"
#include "mibtypeShare.h"

typedef struct st_ap_software_mng_profile {
    mibHead head;
    char sw_name[UTT_SW_NAME_LEN];
    char sw_version[UTT_SW_VERSION_LEN];
    char sw_model[UTT_SW_MODEL_LEN];
    char hw_version[UTT_HW_VERSION_LEN];
    unsigned int defaulted;
}ApSwMngProfile;

#endif
