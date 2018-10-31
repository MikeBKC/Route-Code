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


static void TCPWINDOW_help(void)
{
}

static int TCPWINDOW_parse(int c, char **argv, int invert, unsigned int *flags,
                        const void *entry, struct xt_entry_target **target)
{
        return 0;
}

static void TCPWINDOW_check(unsigned int flags)
{
    return;
}

/* Prints out the targinfo. */
static void TCPWINDOW_print(const void *ip, const struct xt_entry_target *target,
                         int numeric)
{
}

/* Saves the union ipt_targinfo in parsable form to stdout. */
static void TCPWINDOW_save(const void *ip, const struct xt_entry_target *target)
{
}

static struct xtables_target tcpwindow_target = {
	.family		= AF_INET,
	.name		= "TCPWINDOWS",
	.version	= IPTABLES_VERSION,
	.size		= XT_ALIGN(sizeof(int)),
	.userspacesize	= XT_ALIGN(sizeof(int)),
	.help		= TCPWINDOW_help,
	.parse		= TCPWINDOW_parse,
	.final_check	= TCPWINDOW_check,
	.print		= TCPWINDOW_print,
	.save		= TCPWINDOW_save,
};

void _init(void)
{
	xtables_register_target(&tcpwindow_target);
}
