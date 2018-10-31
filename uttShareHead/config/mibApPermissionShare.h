#pragma once

#include "mibunionShare.h"
#include "mibtypeShare.h"

#if (AP_PERMISSION == FYES)

typedef struct st_ap_permission_profile {
	mibHead head;
	char ap_name[UTT_INST_NAME_LEN];
	Uint32 num_start;
	Uint32 num_end;
	ProfileEnableEnum action;
} ApPermissionProfile;

#endif
