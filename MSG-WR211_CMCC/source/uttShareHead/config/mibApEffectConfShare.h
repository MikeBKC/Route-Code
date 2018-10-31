#ifndef MIBAPEFFECTCONF_SHARE_H
#define MIBAPEFFECTCONF_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

typedef struct st_ap_effect_conf_profile {
    mibHead head;
    ProfileEnableEnum config;
}ApEffectConfProfile;

#endif
