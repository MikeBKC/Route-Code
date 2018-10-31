#ifndef UTT_RT_CHIP_FUN_H_
#define UTT_RT_CHIP_FUN_H_
#include <linux/autoconf.h>
#ifdef CONFIG_UTT_WANPORT_SET
extern void wanPortSet();
#endif
#ifdef CONFIG_UTT_RDD_MIRROR
extern void rddMirrorSet(SystemProfile *profSys);
#endif
#ifdef CONFIG_UTT_RDD_PVLAN
extern void rddPvlanSet(Uint32 pno,Uint32 dPortsMsk);
#endif
#endif
