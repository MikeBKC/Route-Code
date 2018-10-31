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
#include <linux/netfilter/xt_TCPMSS.h>


static void TCPOPTION_help(void)
{
}

static int TCPOPTION_parse(int c, char **argv, int invert, unsigned int *flags,
                        const void *entry, struct xt_entry_target **target)
{
        return 0;
}

static void TCPOPTION_check(unsigned int flags)
{
    return;
}

/* Prints out the targinfo. */
static void TCPOPTION_print(const void *ip, const struct xt_entry_target *target,
                         int numeric)
{
}

/* Saves the union ipt_targinfo in parsable form to stdout. */
static void TCPOPTION_save(const void *ip, const struct xt_entry_target *target)
{
}

static struct xtables_target tcpoption_target = {
	.family		= NFPROTO_IPV4,
	.name		= "TCPOPTION",
	.version	= XTABLES_VERSION,
	.size		= XT_ALIGN(sizeof(int)),
	.userspacesize	= XT_ALIGN(sizeof(int)),
	.help		= TCPOPTION_help,
	.parse		= TCPOPTION_parse,
	.final_check	= TCPOPTION_check,
	.print		= TCPOPTION_print,
	.save		= TCPOPTION_save,
};

void _init(void)
{
	xtables_register_target(&tcpoption_target);
}
