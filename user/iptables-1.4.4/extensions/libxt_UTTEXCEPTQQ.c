/* Shared library add-on to iptables to add TCPMSS target support.
 *
 * Copyright (c) 2000 Marc Boucher
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include <xtables.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_UTTEXCEPTQQ.h>


static void UTTEXCEPTQQ_help(void)
{
}

static const struct option UTTEXCEPTQQ_opts[] = {
	{ "config", 1, NULL, '1' },
	{ }
};
static int UTTEXCEPTQQ_parse(int c, char **argv, int invert, unsigned int *flags,
                        const void *entry, struct xt_entry_target **target)
{
    struct xt_uttexceptqq_info *info
		= (struct xt_uttexceptqq_info *)(*target)->data;

    switch (c) {
	case '1' :
	    if (optarg) {
		info->sw = strtoul(optarg, NULL, 16);
	    } else {
		info->sw = 0U;
	    }
	    break;
	default:
	    return 0;
    }
    return 1;
}

static void UTTEXCEPTQQ_check(unsigned int flags)
{
    return;
}

/* Prints out the targinfo. */
static void UTTEXCEPTQQ_print(const void *ip, const struct xt_entry_target *target,
                         int numeric)
{
}

/* Saves the union ipt_targinfo in parsable form to stdout. */
static void UTTEXCEPTQQ_save(const void *ip, const struct xt_entry_target *target)
{
}

static struct xtables_target utthoststa_target = {
	.family		= NFPROTO_IPV4,
	.name		= "UTTEXCEPTQQ",
	.version	= XTABLES_VERSION,
	.size		= XT_ALIGN(sizeof(struct xt_uttexceptqq_info)),
	.userspacesize	= XT_ALIGN(sizeof(struct xt_uttexceptqq_info)),
	.help		= UTTEXCEPTQQ_help,
	.parse		= UTTEXCEPTQQ_parse,
	.final_check	= UTTEXCEPTQQ_check,
	.print		= UTTEXCEPTQQ_print,
	.save		= UTTEXCEPTQQ_save,
	.extra_opts	= UTTEXCEPTQQ_opts,
};

void _init(void)
{
	xtables_register_target(&utthoststa_target);
}
