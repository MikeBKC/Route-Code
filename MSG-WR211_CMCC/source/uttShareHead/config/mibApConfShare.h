#pragma once

#include "mibunionShare.h"
#include "mibtypeShare.h"

#if (AP_CONF == FYES)

typedef struct st_ap_conf_profile {
	mibHead head;
	char types[UTT_SW_MODEL_LEN];
	char softVersion[UTT_SW_VERSION_LEN];
	char firmVersion[UTT_HW_VERSION_LEN];
        int  serial;
} ApConfProfile;

#endif
