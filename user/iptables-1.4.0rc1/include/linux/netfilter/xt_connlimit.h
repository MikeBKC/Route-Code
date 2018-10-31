#ifndef _XT_CONNLIMIT_H
#define _XT_CONNLIMIT_H
#include <linux/netfilter/nf_conntrack_tuple.h>

struct xt_connlimit_data;

struct xt_connlimit_info {
	union {
#if 0
		union nf_inet_addr mask;
#endif
		union nf_conntrack_address mask;

		union {
			__be32 v4_mask;
			__be32 v6_mask[4];
		};
	};
	unsigned int limit, inverse;

	/* Used internally by the kernel */
	struct xt_connlimit_data *data __attribute__((aligned(8)));
};

#endif /* _XT_CONNLIMIT_H */
