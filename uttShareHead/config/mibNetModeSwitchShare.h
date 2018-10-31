#ifndef MIBNETMODESWITCH_SHARE_H
#define MIBNETMODESWITCH_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
typedef struct st_net_mode_switch_profile {
    mibHead head;
    ProfileEnableEnum bridgeEnable;
}NetModeSwitchProfile;

#endif
