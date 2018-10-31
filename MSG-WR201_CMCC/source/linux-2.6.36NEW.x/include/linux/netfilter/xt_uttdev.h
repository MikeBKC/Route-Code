#ifndef _XT_UTTDEV_H
#define _XT_UTTDEV_H

#include <linux/types.h>

#ifdef __KERNEL__
#include <linux/if.h>
#endif

#define XT_UTTDEV_OP_IN		0x01
#define XT_UTTDEV_OP_OUT		0x02
#define XT_UTTDEV_OP_ISLANIN		0x04

#if 1 /* dhyu */
#define XT_UTTDEV_OP_LANTOWAN		0x08
#define XT_UTTDEV_OP_WANTOLAN		0x10
#define XT_UTTDEV_OP_WIFISALEIN         0x20
#endif

struct xt_uttdev_info {
	char uttindev[IFNAMSIZ];
	char in_mask[IFNAMSIZ];
#if 0
	char uttoutdev[IFNAMSIZ];
	char out_mask[IFNAMSIZ];
#endif
	__u8 invert;
	__u8 bitmask;
};

#endif /*_XT_UTTDEV_H*/
