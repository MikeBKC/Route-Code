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


static void UTTPOLICY_help(void)
{
}

static int UTTPOLICY_parse(int c, char **argv, int invert, unsigned int *flags,
                        const void *entry, struct xt_entry_target **target)
{
        return 0;
}

static void UTTPOLICY_check(unsigned int flags)
{
    return;
}

/* Prints out the targinfo. */
static void UTTPOLICY_print(const void *ip, const struct xt_entry_target *target,
                         int numeric)
{
}

/* Saves the union ipt_targinfo in parsable form to stdout. */
static void UTTPOLICY_save(const void *ip, const struct xt_entry_target *target)
{
}

static struct xtables_target uttpolicy_target = {
	.family		= AF_INET,
	.name		= "UTTPOLICY",
	.version	= IPTABLES_VERSION,
	.size		= XT_ALIGN(sizeof(int)),
	.userspacesize	= XT_ALIGN(sizeof(int)),
	.help		= UTTPOLICY_help,
	.parse		= UTTPOLICY_parse,
	.final_check	= UTTPOLICY_check,
	.print		= UTTPOLICY_print,
	.save		= UTTPOLICY_save,
};

void _init(void)
{
	xtables_register_target(&uttpolicy_target);
}
