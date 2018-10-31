#ifndef MIBMIRROR_SHARE_H
#define MIBMIRROR_SHARE_H 

#include <linux/switch/swioc_common.h>
/* port mirror methods */
#define MIRROR_DISABLE 				0
#define MIRROR_EGRESS				1
#define MIRROR_INGRESS				2
#define MIRROR_EGRESS_INGRESS		3

#define SW_MIRROR_PORT_DISABLE      0u
#define SW_MIRROR_PORT_ENABLE       1u

#define PORT_MAX_NUM	24

typedef struct st_mirror_profile {
	mibHead head;
	int mirrorMode; 
	unsigned char monitoredPort[PORT_MAX_NUM + 1];
	unsigned int mirrorPort;
} MirrorProfile;

#endif
