#ifndef MIBFATFITSWITCH_SHARE_H
#define MIBFATFITSWITCH_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
typedef struct st_fat_fit_switch_profile {
    mibHead head;
    ProfileEnableEnum fatEnable;
}FatFitSwitchProfile;

#endif
