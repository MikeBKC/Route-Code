#ifndef MIBVPNCONTROL_SHARE_H
#define MIBVPNCONTROL_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"

#if (VPN_CONTROL == FYES)

typedef struct st_vpnProfile
{
    int count;
} VpnProfile;

#endif /* VPN_CONTROL */

#endif
