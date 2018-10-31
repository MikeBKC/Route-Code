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


static void HTTPCRACK_help(void)
{
}

static int HTTPCRACK_parse(int c, char **argv, int invert, unsigned int *flags,
                        const void *entry, struct xt_entry_target **target)
{
        return 0;
}

static void HTTPCRACK_check(unsigned int flags)
{
    return;
}

/* Prints out the targinfo. */
static void HTTPCRACK_print(const void *ip, const struct xt_entry_target *target,
                         int numeric)
{
}

/* Saves the union ipt_targinfo in parsable form to stdout. */
static void HTTPCRACK_save(const void *ip, const struct xt_entry_target *target)
{
}

static struct xtables_target httpcrack_target = {
	.family		= AF_INET,
	.name		= "HTTPCRACK",
	.version	= IPTABLES_VERSION,
	.size		= XT_ALIGN(sizeof(int)),
	.userspacesize	= XT_ALIGN(sizeof(int)),
	.help		= HTTPCRACK_help,
	.parse		= HTTPCRACK_parse,
	.final_check	= HTTPCRACK_check,
	.print		= HTTPCRACK_print,
	.save		= HTTPCRACK_save,
};

void _init(void)
{
	xtables_register_target(&httpcrack_target);
}
