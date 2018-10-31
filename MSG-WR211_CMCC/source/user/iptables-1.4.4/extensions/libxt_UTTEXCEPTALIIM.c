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


static void UTTEXCEPTAli_help(void)
{
}

static int UTTEXCEPTAli_parse(int c, char **argv, int invert, unsigned int *flags,
                        const void *entry, struct xt_entry_target **target)
{
        return 0;
}

static void UTTEXCEPTAli_check(unsigned int flags)
{
    return;
}

/* Prints out the targinfo. */
static void UTTEXCEPTAli_print(const void *ip, const struct xt_entry_target *target,
                         int numeric)
{
}

/* Saves the union ipt_targinfo in parsable form to stdout. */
static void UTTEXCEPTAli_save(const void *ip, const struct xt_entry_target *target)
{
}

static struct xtables_target utthoststa_target = {
	.family		= NFPROTO_IPV4,
	.name		= "UTTEXCEPTALIIM",
	.version	= XTABLES_VERSION,
	.size		= XT_ALIGN(sizeof(int)),
	.userspacesize	= XT_ALIGN(sizeof(int)),
	.help		= UTTEXCEPTAli_help,
	.parse		= UTTEXCEPTAli_parse,
	.final_check	= UTTEXCEPTAli_check,
	.print		= UTTEXCEPTAli_print,
	.save		= UTTEXCEPTAli_save,
};

void _init(void)
{
	xtables_register_target(&utthoststa_target);
}
