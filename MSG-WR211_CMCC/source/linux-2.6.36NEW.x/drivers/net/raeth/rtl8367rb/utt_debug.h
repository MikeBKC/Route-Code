/*debug funtion 
 * add by bhou
 * 2012-09-11*/
#ifndef __UTT_RT3883_DEBUG_H
#define __UTT_RT3883_DEBUG_H
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include "../ra_ioctl.h"
#if UTT_RT3883_DEBUG
extern void rtkSwMacDump(void);
#endif
#endif
