#pragma once
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"

#if (VLAN_CONFIG == FYES)

typedef struct st_vlan_config_profile {
    mibHead head;
	ProfileEnableEnum vlanEn;
//	char interface[UTT_INTERFACE_LEN];
	Uint32 vid;
    IPADDR ip;
    IPADDR netmask;
}VlanConfigProfile;

#endif
