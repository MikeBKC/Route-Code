#ifndef __MIB_APBLACKWHITELIST_SHARE_H__
#define __MIB_APBLACKWHITELIST_SHARE_H__

#include "mibunionShare.h"
#include "mibtypeShare.h"

#define MAX_STA_PER_BLACKLIST 16

#define AP_BLACK_LIST 0
#define AP_WHITE_LIST 1



typedef struct st_apBlackWhiteListProfile {
	mibHead head;
	Uint32 mode;
	Uint32 num;
	char  sz[UTT_INST_NAME_LEN+1];
	MacAddr mac[MAX_STA_PER_BLACKLIST];
} ApBlackWhiteListProfile;





#endif
