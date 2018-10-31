#ifndef _XT_UTTDEV_H
#define _XT_UTTDEV_H

#include <linux/types.h>

#ifdef __KERNEL__
#include <linux/if.h>
#endif

/*域名白名单放通*/
#define XT_WIFIMATCH_OP_DNS_WHITE_LIST_ADD_IP		0x01
#if 0 
#define XT_WIFIMATCH_OP_			0x02
#define XT_WIFIMATCH_OP_			0x04
#endif

struct xt_wifiMatch_info {
#if 0
	char uttindev[IFNAMSIZ];
	char in_mask[IFNAMSIZ];
	char uttoutdev[IFNAMSIZ];
	char out_mask[IFNAMSIZ];
#endif
	__u8	invert;
	__u32	wififlag;
};

#endif /*_XT_UTTDEV_H*/
