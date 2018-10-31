#ifndef MIBADMINCONF_SHARE_H
#define MIBADMINCONF_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

typedef struct st_AdminConf_Profile {
    mibHead head;
	char	password[UTT_PASSWD_LEN + 1];
} AdminConfProfile;
#endif
