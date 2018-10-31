#include <linux/autoconf.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>

#include "ra_ioctl.h"
#include "uttRtChipApi.h"

/* global api function sets builded by utt 
 * and if you want add a new type chip
 * you must  register new api functions  for it */
UttRtChipApiSt uttRtChipApi;


static void uttRtChipConf(UttRtChipFuncSt* confSt)
{
    switch(confSt->confType)
    {
#ifdef CONFIG_UTT_RDD_MIRROR
	case UTT_RDD_MIRROR_SET:
            uttRtChipApi.pRddMirrorSet(&(confSt->confData.mirror));
	    break;
#endif
#ifdef CONFIG_UTT_WANPORT_SET
	case UTT_WANPORT_SET:
            uttRtChipApi.pWanPortSet(confSt->confData.wanPortsMask);
	    break;
#endif
#ifdef CONFIG_UTT_RDD_PVLAN
	case UTT_RDD_PVLAN_SET:
            uttRtChipApi.pRddPvlanSet(&(confSt->confData.pvlan));
	    break;
#endif
#if defined(CONFIG_UTT_TAG_VLAN_3662A)
	case UTT_PVID_SET:
	    uttRtChipApi.pPvidSet(&(confSt->confData.pvidConf));
	    break;
	case UTT_TAG_VLAN_SET:
	    uttRtChipApi.pTagVlanSet(&(confSt->confData.tagVlanConf));
	    break;
	case UTT_TAG_VLAN_DEL:
	    uttRtChipApi.pTagVlanDel(&(confSt->confData.tagVlanConf));
	    break;
#endif
	default:
	    break;
    }
    return;
}
/**
 * ioctl perform communication between user and kernel
 * you can build it with netlink too
 */
void uttRtChipIoctl(char *user_data)
{
    UttRtChipFuncSt confSt;
    copy_from_user(&confSt, user_data, sizeof(UttRtChipFuncSt));
    uttRtChipConf(&confSt);
    return;
}
