#ifndef _IPT_REJECT_H
#define _IPT_REJECT_H

enum ipt_reject_with {
	IPT_ICMP_NET_UNREACHABLE,
	IPT_ICMP_HOST_UNREACHABLE,
	IPT_ICMP_PROT_UNREACHABLE,
	IPT_ICMP_PORT_UNREACHABLE,
	IPT_ICMP_ECHOREPLY,
	IPT_ICMP_NET_PROHIBITED,
	IPT_ICMP_HOST_PROHIBITED,
	IPT_TCP_RESET,
#if CONFIG_NETFILTER_XT_TARGET_UTTDNSREDIRECT
	IPT_DNS_REDIRECT,
#endif
	IPT_ICMP_ADMIN_PROHIBITED
};

struct ipt_reject_info {
	enum ipt_reject_with with;      /* reject type */
#if CONFIG_NETFILTER_XT_TARGET_UTTDNSREDIRECT
        __u32 ip;
#endif
};

extern void send_dns_redirect(struct sk_buff *oldskb, __u32 redirectIp);
#endif /*_IPT_REJECT_H*/
