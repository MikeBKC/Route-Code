/* Shared library add-on to iptables to add bridge port matching support. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <xtables.h>
#include <linux/netfilter/xt_uttdev.h>
#if defined(__GLIBC__) && __GLIBC__ == 2
#include <net/ethernet.h>
#else
#include <linux/if_ether.h>
#endif

static void uttdev_help(void)
{
#if 0
	printf(
"uttdev v%s options:\n"
" --uttdev-in [!] input name[+]		bridge port name ([+] for wildcard)\n"
" --uttdev-out [!] output name[+]	bridge port name ([+] for wildcard)\n"
" [!] --is-lan-in			arrived on a bridge device\n"
"\n", IPTABLES_VERSION);
#endif
	printf(
"uttdev v%s options:\n"
" [!] --is-lan-in			is a packet come from lan\n"
#if 1
" --lan-to-wan			is a packet come from lan to wan, use in forward chain\n"
" --wan-to-lan			is a packet come from wan to lan, use in forward chain\n"
#endif
"\n", IPTABLES_VERSION);

}

static const struct option uttdev_opts[] = {
#if 0
	{ "uttdev-in", 1, NULL, '1' },
	{ "uttdev-out", 1, NULL, '2' },
#endif
	{ "is-lan-in", 0, NULL, '3' },
#if 1
	{ "lan-to-wan", 0, NULL, '4' },
	{ "wan-to-lan", 0, NULL, '5' },
#endif
	{ }
};

static int
uttdev_parse(int c, char **argv, int invert, unsigned int *flags,
              const void *entry, struct xt_entry_match **match)
{
	struct xt_uttdev_info *info =
		(struct xt_uttdev_info*)(*match)->data;

	switch (c) {
#if 0
	case '1':
		if (*flags & XT_UTTDEV_OP_IN)
			goto multiple_use;
		check_inverse(optarg, &invert, &optind, 0);
		parse_interface(argv[optind-1], info->physindev,
				(unsigned char *)info->in_mask);
		if (invert)
			info->invert |= XT_UTTDEV_OP_IN;
		info->bitmask |= XT_UTTDEV_OP_IN;
		*flags |= XT_UTTDEV_OP_IN;
		break;

	case '2':
		if (*flags & XT_UTTDEV_OP_OUT)
			goto multiple_use;
		check_inverse(optarg, &invert, &optind, 0);
		parse_interface(argv[optind-1], info->physoutdev,
				(unsigned char *)info->out_mask);
		if (invert)
			info->invert |= XT_UTTDEV_OP_OUT;
		info->bitmask |= XT_UTTDEV_OP_OUT;
		*flags |= XT_UTTDEV_OP_OUT;
		break;
#endif

	case '3':
#if 0
		/*这里判断命令是否可以重入两次。如-mark 中的--mark 是否可以写两次*/
		if (*flags & XT_UTTDEV_OP_ISLANIN)
		    goto multiple_use;
#endif
		check_inverse(optarg, &invert, &optind, 0);
		info->bitmask |= XT_UTTDEV_OP_ISLANIN;
		if (invert)
		    info->invert |= XT_UTTDEV_OP_ISLANIN;
		*flags |= XT_UTTDEV_OP_ISLANIN;
		break;
#if 1
	case '4':
		info->bitmask |= XT_UTTDEV_OP_LANTOWAN;

		*flags |= XT_UTTDEV_OP_LANTOWAN;
		break;

	case '5':
		info->bitmask |= XT_UTTDEV_OP_WANTOLAN;

		*flags |= XT_UTTDEV_OP_WANTOLAN;
		break;
#endif

	default:
		return 0;
	}

	return 1;
#if 0
multiple_use:
	exit_error(PARAMETER_PROBLEM,
	   "multiple use of the same uttdev option is not allowed");
#endif

}

static void uttdev_check(unsigned int flags)
{
	int para_cnt = 0;

	if (flags == 0)
		exit_error(PARAMETER_PROBLEM, "UTTDEV: no uttdev option specified");
#if 1
	/* is-lan-in, lan-to-wan, wan-to-lan can only have one */
	if (flags & XT_UTTDEV_OP_ISLANIN) {
	    para_cnt++;
	}
	if (flags & XT_UTTDEV_OP_LANTOWAN) {
	    para_cnt++;
	}
	if (flags & XT_UTTDEV_OP_WANTOLAN) {
	    para_cnt++;
	}

	if (para_cnt > 1) {
		exit_error(PARAMETER_PROBLEM, "UTTDEV: too many param");
	}
#endif
}

static void
uttdev_print(const void *ip, const struct xt_entry_match *match, int numeric)
{
	struct xt_uttdev_info *info =
		(struct xt_uttdev_info*)match->data;

	printf("UTTDEV match");
	if (info->bitmask & XT_UTTDEV_OP_ISLANIN)
		printf("%s --is-lan-in",
		       info->invert & XT_UTTDEV_OP_ISLANIN ? " !":"");
#if 0
	if (info->bitmask & XT_UTTDEV_OP_IN)
		printf("%s --uttdev-in %s",
		(info->invert & XT_UTTDEV_OP_IN) ? " !":"", info->physindev);
	if (info->bitmask & XT_UTTDEV_OP_OUT)
		printf("%s --uttdev-out %s",
		(info->invert & XT_UTTDEV_OP_OUT) ? " !":"", info->physoutdev);
#endif
#if 1
	if (info->bitmask & XT_UTTDEV_OP_LANTOWAN)
		printf(" --lan-to-wan ");
	if (info->bitmask & XT_UTTDEV_OP_WANTOLAN)
		printf(" --wan-to-lan ");
#endif

	printf(" ");
}

static void uttdev_save(const void *ip, const struct xt_entry_match *match)
{
	struct xt_uttdev_info *info =
		(struct xt_uttdev_info*)match->data;

	if (info->bitmask & XT_UTTDEV_OP_ISLANIN)
		printf("%s --is-lan-in",
		       info->invert & XT_UTTDEV_OP_ISLANIN ? " !":"");
#if 0
	if (info->bitmask & XT_UTTDEV_OP_IN)
		printf("%s --uttdev-in %s",
		(info->invert & XT_UTTDEV_OP_IN) ? " !":"", info->physindev);
	if (info->bitmask & XT_UTTDEV_OP_OUT)
		printf("%s --uttdev-out %s",
		(info->invert & XT_UTTDEV_OP_OUT) ? " !":"", info->physoutdev);
#endif
#if 1
	if (info->bitmask & XT_UTTDEV_OP_LANTOWAN)
		printf("--lan-to-wan ");
	if (info->bitmask & XT_UTTDEV_OP_WANTOLAN)
		printf("--wan-to-lan ");
#endif
	printf(" ");
}

static struct xtables_match uttdev_match = {
	.family		= AF_INET,
	.name		= "uttdev",
	.version	= IPTABLES_VERSION,
	.size		= XT_ALIGN(sizeof(struct xt_uttdev_info)),
	.userspacesize	= XT_ALIGN(sizeof(struct xt_uttdev_info)),
	.help		= uttdev_help,
	.parse		= uttdev_parse,
	.final_check	= uttdev_check,
	.print		= uttdev_print,
	.save		= uttdev_save,
	.extra_opts	= uttdev_opts,
};

static struct xtables_match uttdev_match6 = {
	.family		= AF_INET6,
	.name		= "uttdev",
	.version	= IPTABLES_VERSION,
	.size		= XT_ALIGN(sizeof(struct xt_uttdev_info)),
	.userspacesize	= XT_ALIGN(sizeof(struct xt_uttdev_info)),
	.help		= uttdev_help,
	.parse		= uttdev_parse,
	.final_check	= uttdev_check,
	.print		= uttdev_print,
	.save		= uttdev_save,
	.extra_opts	= uttdev_opts,
};

void _init(void)
{
	xtables_register_match(&uttdev_match);
	xtables_register_match(&uttdev_match6);
}
