/* Shared library add-on to iptables to add bridge port matching support. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <xtables.h>
#include <linux/netfilter/xt_httpQQ.h>
#if defined(__GLIBC__) && __GLIBC__ == 2
#include <net/ethernet.h>
#else
#include <linux/if_ether.h>
#endif

static void httpQQ_help(void)
{
	printf(
"httpQQ match options:\n"
" [!] --is-match			is a packet match for http QQ login\n"
);
}

static const struct option httpQQ_opts[] = {
	{ "is-match", 0, NULL, '3' },
	{ .name = NULL }
};

static int
httpQQ_parse(int c, char **argv, int invert, unsigned int *flags,
              const void *entry, struct xt_entry_match **match)
{
	struct xt_httpQQ_info *info =
		(struct xt_httpQQ_info*)(*match)->data;

	switch (c) {
		case '3':
#if 0
		/*这里判断命令是否可以重入两次。如-mark 中的--mark 是否可以写两次*/
		if (*flags & XT_UTTDEV_OP_ISLANIN)
			goto multiple_use;
#endif
#if 1
		xtables_check_inverse(optarg, &invert, &optind, 0);
		if (invert)
			info->invert |= XT_HTTPQQ_MATCH_CHILD;
#endif
#if DEBUG
		printf("%s-%d: invert =%d\n",__func__,__LINE__,invert);
#endif
#if 1
		info->flag |= XT_HTTPQQ_MATCH_CHILD;
		*flags |= XT_HTTPQQ_MATCH_CHILD;
#endif
		break;

#if 0
		case '4':
		info->bitmask |= XT_UTTDEV_OP_LANTOWAN;

		*flags |= XT_UTTDEV_OP_LANTOWAN;
		break;

		case '5':
		info->bitmask |= XT_UTTDEV_OP_WANTOLAN;

		*flags |= XT_UTTDEV_OP_WANTOLAN;
		break;

		case '6':
		info->bitmask |= XT_UTTDEV_OP_ISLANOUT;

		*flags |= XT_UTTDEV_OP_ISLANOUT;
		break;
#endif

	default:
		return 0;
	}

	return 1;
}

static void httpQQ_check(unsigned int flags)
{
	int para_cnt = 0;

	if (flags == 0)
		xtables_error(PARAMETER_PROBLEM, "httpQQ: no httpQQ option specified");

	/* is-match */
	if (flags & XT_HTTPQQ_MATCH_CHILD) {
	    para_cnt++;
	}
#if 0
	if (flags & XT_UTTDEV_OP_LANTOWAN) {
	    para_cnt++;
	}
	if (flags & XT_UTTDEV_OP_WANTOLAN) {
	    para_cnt++;
	}
	if (flags & XT_UTTDEV_OP_ISLANOUT) {
	    para_cnt++;
	}
#endif

	if (para_cnt > 1) {
		xtables_error(PARAMETER_PROBLEM, "httpQQ: too many param");
	}
}

static void
httpQQ_print(const void *ip, const struct xt_entry_match *match, int numeric)
{
	const struct xt_httpQQ_info *info = (const void *)match->data;

	printf("httpQQ match");
	if (info->flag & XT_HTTPQQ_MATCH_CHILD)
		printf("%s --is-match",
		       info->invert & XT_HTTPQQ_MATCH_CHILD ? " !":"");
	
#if 0
	if (info->bitmask & XT_UTTDEV_OP_LANTOWAN)
		printf(" --lan-to-wan ");
	if (info->bitmask & XT_UTTDEV_OP_WANTOLAN)
		printf(" --wan-to-lan ");
	if (info->bitmask & XT_UTTDEV_OP_ISLANOUT)
		printf(" --is-lan-out ");
#endif

#if 0
	if (info->bitmask & XT_UTTDEV_OP_IN)
		printf("%s --uttdev-in %s",
		(info->invert & XT_UTTDEV_OP_IN) ? " !":"", info->physindev);

	if (info->bitmask & XT_UTTDEV_OP_OUT)
		printf("%s --uttdev-out %s",
		(info->invert & XT_UTTDEV_OP_OUT) ? " !":"", info->physoutdev);
#endif
	printf(" ");
}

static void httpQQ_save(const void *ip, const struct xt_entry_match *match)
{
	const struct xt_httpQQ_info *info = (const void *)match->data;

	if (info->flag & XT_HTTPQQ_MATCH_CHILD)
		printf("%s--is-match ",
		       (info->invert & XT_HTTPQQ_MATCH_CHILD) ? "! " : "");
#if 0
	if (info->bitmask & XT_UTTDEV_OP_LANTOWAN)
		printf("--lan-to-wan ");
	if (info->bitmask & XT_UTTDEV_OP_WANTOLAN)
		printf("--wan-to-lan ");
	if (info->bitmask & XT_UTTDEV_OP_ISLANOUT)
		printf("--is-lan-out ");
#endif
#if 0
	if (info->bitmask & XT_UTTDEV_OP_IN)
		printf("%s--uttdev-in %s ",
		       (info->invert & XT_UTTDEV_OP_IN) ? "! " : "",
		       info->physindev);

	if (info->bitmask & XT_UTTDEV_OP_OUT)
		printf("%s--uttdev-out %s ",
		       (info->invert & XT_UTTDEV_OP_OUT) ? "! " : "",
		       info->physoutdev);
#endif
}

static struct xtables_match httpQQ_match = {
	.family		= NFPROTO_UNSPEC,
	.name		= "httpQQ",
	.version	= XTABLES_VERSION,
	.size		= XT_ALIGN(sizeof(struct xt_httpQQ_info)),
	.userspacesize	= XT_ALIGN(sizeof(struct xt_httpQQ_info)),
	.help		= httpQQ_help,
	.parse		= httpQQ_parse,
	.final_check	= httpQQ_check,
	.print		= httpQQ_print,
	.save		= httpQQ_save,
	.extra_opts	= httpQQ_opts,
};

void _init(void)
{
	xtables_register_match(&httpQQ_match);
}
