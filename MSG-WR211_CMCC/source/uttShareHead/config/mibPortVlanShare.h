#ifndef _MIB_PORT_VLAN_SHARE_H_
#define _MIB_PORT_VLAN_SHARE_H_

#include "mibunionShare.h"
#include "mibtypeShare.h"
#include <linux/switch/swioc_common.h>

typedef struct _port_vlan_profile{
    mibHead head;
    char description[UTT_INST_NAME_LEN+1];
    unsigned char  members[PORT_ARRAY_SIZE];
}PortVlanProfile;

#endif
