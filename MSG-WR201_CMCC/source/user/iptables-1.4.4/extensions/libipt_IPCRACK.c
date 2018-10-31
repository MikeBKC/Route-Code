/*
 * This program is distributed under the terms of GNU GPL
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <iptables.h>

#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv4/ipt_IPCRACK.h>

#define IPT_IPCRACK_USED	1

static void IPCRACK_help(void)
{
	printf("  --ttl-set value		Set TTL to <value 0-255>\n");
}

static int IPCRACK_parse(int c, char **argv, int invert, unsigned int *flags,
                     const void *entry, struct xt_entry_target **target)
{
	struct ipt_IPCRACK_info *info = (struct ipt_IPCRACK_info *) (*target)->data;
	unsigned int value;

	if (*flags & IPT_IPCRACK_USED) {
		xtables_error(PARAMETER_PROBLEM, 
				"Can't specify TTL option twice");
	}

	if (!optarg) 
		xtables_error(PARAMETER_PROBLEM, 
				"TTL: You must specify a value");

	if (xtables_check_inverse(optarg, &invert, NULL, 0))
		xtables_error(PARAMETER_PROBLEM,
				"TTL: unexpected `!'");
	
	if (!xtables_strtoui(optarg, NULL, &value, 0, UINT8_MAX))
		xtables_error(PARAMETER_PROBLEM,
		           "TTL: Expected value between 0 and 255");

	info->ttl = value;
	*flags |= IPT_IPCRACK_USED;

	return 1;
}

static void IPCRACK_check(unsigned int flags)
{
	if (!(flags & IPT_IPCRACK_USED))
		xtables_error(PARAMETER_PROBLEM,
				"IPCRACK: You must specify an action");
}

static void IPCRACK_save(const void *ip, const struct xt_entry_target *target)
{
    return;
}


static void IPCRACK_print(const void *ip, const struct xt_entry_target *target,
                      int numeric)
{
	const struct ipt_IPCRACK_info *info =
		(struct ipt_IPCRACK_info *) target->data;

	printf("ip crack ttl set to %u", info->ttl);
}

static const struct option IPCRACK_opts[] = {
	{ "ttl-set", 1, NULL, '1' },
	{ }
};

static struct xtables_target ipcrack_target = {
	.name		= "IPCRACK",
	.version	= XTABLES_VERSION,
	.family		= NFPROTO_IPV4,
	.size		= IPT_ALIGN(sizeof(struct ipt_IPCRACK_info)),
	.userspacesize	= IPT_ALIGN(sizeof(struct ipt_IPCRACK_info)),
	.help		= IPCRACK_help,
	.parse		= IPCRACK_parse,
	.final_check	= IPCRACK_check,
	.print		= IPCRACK_print,
	.save		= IPCRACK_save,
	.extra_opts	= IPCRACK_opts,
};

void _init(void)
{
	xtables_register_target(&ipcrack_target);
}
