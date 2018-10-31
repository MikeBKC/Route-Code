#ifndef _MIB_RDD_PORT_VLAN_SHARE_H_
#define _MIB_RDD_PORT_VLAN_SHARE_H_

#include "mibunionShare.h"
#include "mibtypeShare.h"

typedef struct _port_vlan_profile{
    mibHead head;
    char description[UTT_INST_NAME_LEN+1];
    unsigned char  members[MAX_LAN_PORT_COUNT + 1];
}RddPortVlanProfile;

#endif
