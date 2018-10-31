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


static void UTTNETSNIPER_help(void)
{
}

static int UTTNETSNIPER_parse(int c, char **argv, int invert, unsigned int *flags,
                        const void *entry, struct xt_entry_target **target)
{
        return 0;
}

static void UTTNETSNIPER_check(unsigned int flags)
{
    return;
}

/* Prints out the targinfo. */
static void UTTNETSNIPER_print(const void *ip, const struct xt_entry_target *target,
                         int numeric)
{
}

/* Saves the union ipt_targinfo in parsable form to stdout. */
static void UTTNETSNIPER_save(const void *ip, const struct xt_entry_target *target)
{
}

static struct xtables_target uttnetsniper_target = {
	.family		= AF_INET,
	.name		= "UTTNETSNIPER",
	.version	= IPTABLES_VERSION,
	.size		= XT_ALIGN(sizeof(int)),
	.userspacesize	= XT_ALIGN(sizeof(int)),
	.help		= UTTNETSNIPER_help,
	.parse		= UTTNETSNIPER_parse,
	.final_check	= UTTNETSNIPER_check,
	.print		= UTTNETSNIPER_print,
	.save		= UTTNETSNIPER_save,
};

void _init(void)
{
	xtables_register_target(&uttnetsniper_target);
}
