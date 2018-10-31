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


static void UTTHOSTSTA_help(void)
{
}

static int UTTHOSTSTA_parse(int c, char **argv, int invert, unsigned int *flags,
                        const void *entry, struct xt_entry_target **target)
{
        return 0;
}

static void UTTHOSTSTA_check(unsigned int flags)
{
    return;
}

/* Prints out the targinfo. */
static void UTTHOSTSTA_print(const void *ip, const struct xt_entry_target *target,
                         int numeric)
{
}

/* Saves the union ipt_targinfo in parsable form to stdout. */
static void UTTHOSTSTA_save(const void *ip, const struct xt_entry_target *target)
{
}

static struct xtables_target utthoststa_target = {
	.family		= NFPROTO_IPV4,
	.name		= "UTTHOSTSTA",
	.version	= XTABLES_VERSION,
	.size		= XT_ALIGN(sizeof(int)),
	.userspacesize	= XT_ALIGN(sizeof(int)),
	.help		= UTTHOSTSTA_help,
	.parse		= UTTHOSTSTA_parse,
	.final_check	= UTTHOSTSTA_check,
	.print		= UTTHOSTSTA_print,
	.save		= UTTHOSTSTA_save,
};

void _init(void)
{
	xtables_register_target(&utthoststa_target);
}
