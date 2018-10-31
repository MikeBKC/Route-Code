#ifndef MIBTRUNK_SHARE_H
#define MIBTRUNK_SHARE_H

#include <linux/switch/swioc_common.h>
typedef struct st_trunk_profile {
	mibHead head;
	int tMode;	
	int tGroup;
	char tName[UTT_INST_NAME_LEN + 1];
	unsigned char tMembers[PORT_ARRAY_SIZE];
} TrunkProfile;

#endif
