#ifndef _XT_HTTPQQ_H
#define _XT_HTTPQQ_H

#include <linux/types.h>

#ifdef __KERNEL__
#include <linux/if.h>
#endif

#define XT_HTTPQQ_MATCH_CHILD		0x01
#define XT_HTTPQQ_MATCH_PARENT		0x02

#define UTT_HTTP_QQ_MATCH_FLAG		0x01
#define UTT_HTTP_QQ_NOT_MATCH_FLAG	0x02

struct xt_httpQQ_info {
	__u8 invert;
	__u8 flag;
};

#endif /*_XT_HTTPQQ_H*/
