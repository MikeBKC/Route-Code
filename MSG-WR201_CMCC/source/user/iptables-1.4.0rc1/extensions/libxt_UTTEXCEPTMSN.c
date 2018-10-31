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


static void UTTEXCEPTMSN_help(void)
{
}

static int UTTEXCEPTMSN_parse(int c, char **argv, int invert, unsigned int *flags,
                        const void *entry, struct xt_entry_target **target)
{
        return 0;
}

static void UTTEXCEPTMSN_check(unsigned int flags)
{
    return;
}

/* Prints out the targinfo. */
static void UTTEXCEPTMSN_print(const void *ip, const struct xt_entry_target *target,
                         int numeric)
{
}

/* Saves the union ipt_targinfo in parsable form to stdout. */
static void UTTEXCEPTMSN_save(const void *ip, const struct xt_entry_target *target)
{
}

static struct xtables_target uttexceptmsn_target = {
	.family		= AF_INET,
	.name		= "UTTEXCEPTMSN",
	.version	= IPTABLES_VERSION,
	.size		= XT_ALIGN(sizeof(int)),
	.userspacesize	= XT_ALIGN(sizeof(int)),
	.help		= UTTEXCEPTMSN_help,
	.parse		= UTTEXCEPTMSN_parse,
	.final_check	= UTTEXCEPTMSN_check,
	.print		= UTTEXCEPTMSN_print,
	.save		= UTTEXCEPTMSN_save,
};

void _init(void)
{
	xtables_register_target(&uttexceptmsn_target);
}
