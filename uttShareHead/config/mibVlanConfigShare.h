#ifndef _H_MIB_VLAN_CONFIG_H_
#define _H_MIB_VLAN_CONFIG_H_

#include "mibunionShare.h"
#include "mibtypeShare.h"

typedef struct st_vlan_config_profile {
    mibHead head;
	ProfileEnableEnum vlanEn;
//	char interface[UTT_INTERFACE_LEN];
	Uint32 vid;
    IPADDR ip;
    IPADDR netmask;
}VlanConfigProfile;

#endif
