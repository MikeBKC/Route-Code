/* Shared library add-on to iptables to add UTTLOG target support.
 *
 * Copyright (c) 2000 Marc Boucher
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include <xtables.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_UTTLOG.h>


static void UTTLOG_help(void)
{
    printf("UTTLOG target options:\n"
" --config value hex 0x11223344.eg, --config 0x3\n"
" it is web config. means which log has been setted to yes\n"
	    );
}

static const struct option UTTLOG_opts[] = {
	{ "config", 1, NULL, '1' },
	{ "http-port", 1, NULL, '2' },
	{ }
};

static int UTTLOG_parse(int c, char **argv, int invert, unsigned int *flags,
                        const void *entry, struct xt_entry_target **target)
{	
    struct xt_uttlog_info *info
		= (struct xt_uttlog_info *)(*target)->data;

    switch (c) {
	case '1' :
	    if (optarg) {
		info->sw = strtoul(optarg, NULL, 16);
	    } else {
		info->sw = 0U;
	    }
	    break;
	case '2' :
		if (optarg) {
		info->port = strtoul(optarg, NULL, 10);
		} else {
		info->port = 80u;
		}
		break;
	default:
	    return 0;
    }
    return 1;
}

static void UTTLOG_check(unsigned int flags)
{
    return;
}

/* Prints out the targinfo. */
static void UTTLOG_print(const void *ip, const struct xt_entry_target *target,
                         int numeric)
{
}

/* Saves the union ipt_targinfo in parsable form to stdout. */
static void UTTLOG_save(const void *ip, const struct xt_entry_target *target)
{
}

static struct xtables_target uttlog_target = {
	.family		= AF_INET,
	.name		= "UTTLOG",
	.version	= XTABLES_VERSION,
	.size		= XT_ALIGN(sizeof(struct xt_uttlog_info)),
	.userspacesize	= XT_ALIGN(sizeof(struct xt_uttlog_info)),
	.help		= UTTLOG_help,
	.parse		= UTTLOG_parse,
	.final_check	= UTTLOG_check,
	.print		= UTTLOG_print,
	.save		= UTTLOG_save,
	.extra_opts	= UTTLOG_opts,
};

void _init(void)
{
	xtables_register_target(&uttlog_target);
}
