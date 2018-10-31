/* Shared library add-on to iptables to add bridge port matching support. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <xtables.h>
#include <linux/netfilter/xt_uttsniper.h>
#if defined(__GLIBC__) && __GLIBC__ == 2
#include <net/ethernet.h>
#else
#include <linux/if_ether.h>
#endif

static void uttsniper_help(void)
{
	printf(
"uttsniper match options:\n"
" [!] --is-need-notice			is internet sharing,need to notice\n"
);
}

static const struct option uttsniper_opts[] = {
	{ "is-need-notice", 0, NULL, '1' },
	{ .name = NULL }
};

static int
uttsniper_parse(int c, char **argv, int invert, unsigned int *flags,
              const void *entry, struct xt_entry_match **match)
{
	struct xt_uttsniper_info *info =
		(struct xt_uttsniper_info*)(*match)->data;

	switch (c) {
		case '1':
		xtables_check_inverse(optarg, &invert, &optind, 0,argv);
		info->bitmask |= XT_UTTSNIPER_OP_NEEDNOTICE;
		if (invert)
			info->invert |= XT_UTTSNIPER_OP_NEEDNOTICE;
		*flags |= XT_UTTSNIPER_OP_NEEDNOTICE;
		break;
	default:
		return 0;
	}

	return 1;
}

static void uttsniper_check(unsigned int flags)
{
	int para_cnt = 0;

	if (flags == 0)
		xtables_error(PARAMETER_PROBLEM, "UTTSNIPER: no uttsniper option specified");

	if (flags & XT_UTTSNIPER_OP_NEEDNOTICE) {
	    para_cnt++;
	}

	if (para_cnt > 1) {
		xtables_error(PARAMETER_PROBLEM, "UTTSNIPER: too many param");
	}
}

static void
uttsniper_print(const void *ip, const struct xt_entry_match *match, int numeric)
{
	const struct xt_uttsniper_info *info = (const void *)match->data;

	printf("UTTSNIPER match");
	if (info->bitmask & XT_UTTSNIPER_OP_NEEDNOTICE)
		printf("%s --is-need-notice",
		       info->invert & XT_UTTSNIPER_OP_NEEDNOTICE ? " !":"");
	printf(" ");
}

static void uttsniper_save(const void *ip, const struct xt_entry_match *match)
{
	const struct xt_uttsniper_info *info = (const void *)match->data;

	if (info->bitmask & XT_UTTSNIPER_OP_NEEDNOTICE)
		printf("%s--is-need-notcie ",
		       (info->invert & XT_UTTSNIPER_OP_NEEDNOTICE) ? "! " : "");
}

static struct xtables_match uttsniper_match = {
	.family		= NFPROTO_UNSPEC,
	.name		= "uttsniper",
	.version	= XTABLES_VERSION,
	.size		= XT_ALIGN(sizeof(struct xt_uttsniper_info)),
	.userspacesize	= XT_ALIGN(sizeof(struct xt_uttsniper_info)),
	.help		= uttsniper_help,
	.parse		= uttsniper_parse,
	.final_check	= uttsniper_check,
	.print		= uttsniper_print,
	.save		= uttsniper_save,
	.extra_opts	= uttsniper_opts,
};

void _init(void)
{
	xtables_register_match(&uttsniper_match);
}
