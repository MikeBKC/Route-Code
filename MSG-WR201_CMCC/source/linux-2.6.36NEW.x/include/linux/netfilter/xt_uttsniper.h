#ifndef _XT_UTTSNIPER_H
#define _XT_UTTSNIPER_H

#include <linux/types.h>

#ifdef __KERNEL__
#include <linux/if.h>
#endif


#define XT_UTTSNIPER_OP_NEEDNOTICE		0x01

struct xt_uttsniper_info {
	__u8 invert;
	__u8 bitmask;
};

#endif /*_XT_UTTSNIPER_H*/
