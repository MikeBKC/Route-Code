/* Shared library add-on to iptables to add bridge port matching support. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <xtables.h>
#include <linux/netfilter/xt_wifiMatch.h>
#if defined(__GLIBC__) && __GLIBC__ == 2
#include <net/ethernet.h>
#else
#include <linux/if_ether.h>
#endif

static void wifiMatch_help(void)
{
	printf(
"wifiMatch match options:\n"
" --add-dns-ip			add domain name ip\n"
);
}

static const struct option wifiMatch_opts[] = {
	{ "add-dns-ip", 0, NULL, '1' },
#if 0
	{ "", 0, NULL, '2' },
#endif
	{ .name = NULL }
};

static int
wifiMatch_parse(int c, char **argv, int invert, unsigned int *flags,
              const void *entry, struct xt_entry_match **match)
{
	struct xt_wifiMatch_info *info =
		(struct xt_wifiMatch_info*)(*match)->data;

	switch (c) {
		case '1':
#if 0
		/*这里判断命令是否可以重入两次。如-mark 中的--mark 是否可以写两次*/
		if (*flags & XT_WIFIMATCH_OP_DNS_WHITE_LIST_ADD_IP)
			goto multiple_use;
#endif
		xtables_check_inverse(optarg, &invert, &optind, 0, argv);
		info->wififlag |= XT_WIFIMATCH_OP_DNS_WHITE_LIST_ADD_IP;
#if 0
		/*禁止使用 '!'*/
		if (invert)
			info->invert |= XT_WIFIMATCH_OP_DNS_WHITE_LIST_ADD_IP;
#endif
		*flags |= XT_WIFIMATCH_OP_DNS_WHITE_LIST_ADD_IP;
		break;

#if 0
		case '2':
		info->wififlag |= ;

		*flags |= ;
		break;

		case '3':
		info->wififlag |= ;

		*flags |= ;
		break;
#endif

	default:
		return 0;
	}

	return 1;
#if 0
multiple_use:
	xtables_error(PARAMETER_PROBLEM,
	   "multiple use of the same uttdev option is not allowed");
#endif
}

static void wifiMatch_check(unsigned int flags)
{
	int para_cnt = 0;

	if (flags == 0)
		xtables_error(PARAMETER_PROBLEM, "wifiMatch: no wifiMatch option specified");

	/* is-lan-in, lan-to-wan, wan-to-lan can only have one */
	if (flags & XT_WIFIMATCH_OP_DNS_WHITE_LIST_ADD_IP) {
	    para_cnt++;
	}
}

static void
wifiMatch_print(const void *ip, const struct xt_entry_match *match, int numeric)
{
	const struct xt_wifiMatch_info *info = (const void *)match->data;

	printf("wifiMatch match");
	if (info->wififlag & XT_WIFIMATCH_OP_DNS_WHITE_LIST_ADD_IP)
		printf(" --add-dns-ip ");
	
#if 0
	if (info->wififlag & xx)
		printf("  ");
#endif

	printf(" ");
}

static void wifiMatch_save(const void *ip, const struct xt_entry_match *match)
{
	const struct xt_wifiMatch_info *info = (const void *)match->data;

	if (info->wififlag & XT_WIFIMATCH_OP_DNS_WHITE_LIST_ADD_IP)
		printf(" --add-dns-ip ");
}

static struct xtables_match wifiMatch_match = {
	.family		= NFPROTO_UNSPEC,
	.name		= "wifiMatch",
	.version	= XTABLES_VERSION,
	.size		= XT_ALIGN(sizeof(struct xt_wifiMatch_info)),
	.userspacesize	= XT_ALIGN(sizeof(struct xt_wifiMatch_info)),
	.help		= wifiMatch_help,
	.parse		= wifiMatch_parse,
	.final_check	= wifiMatch_check,
	.print		= wifiMatch_print,
	.save		= wifiMatch_save,
	.extra_opts	= wifiMatch_opts,
};

void _init(void)
{
	xtables_register_match(&wifiMatch_match);
}
