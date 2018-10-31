#ifndef __UTT_RT_CHIP_API__
#define __UTT_RT_CHIP_API__
#include <linux/autoconf.h>
#include "ra_ioctl.h"

typedef struct uttRtChipApi_St{
#ifdef CONFIG_UTT_RDD_MIRROR
    void (*pRddMirrorSet) (UttRddMirrorSt* mirror);
#endif
#ifdef CONFIG_UTT_WANPORT_SET
    /*wan numbers referd chip switch ports don't inclued some virtual ports*/
    void (*pWanPortSet) (unsigned int wanPortsMask);
#endif
#ifdef CONFIG_UTT_RDD_PVLAN
    void (*pRddPvlanSet) (UttRddPvlanSt *pvlan);
#endif
#if defined(CONFIG_UTT_TAG_VLAN_3662A)
    void (*pTagVlanSet)(UttTagVlanSt *pTagVlanConf);
    void (*pTagVlanDel)(UttTagVlanSt *pTagVlanConf);
    void (*pPvidSet)(UttPvidSt *pPvidConf);
#endif
}UttRtChipApiSt;

extern UttRtChipApiSt uttRtChipApi;
#endif
