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
	printf(
"  --ttl-set value		Set TTL to <value 0-255>\n"
, IPTABLES_VERSION);
}

static int IPCRACK_parse(int c, char **argv, int invert, unsigned int *flags,
                     const void *entry, struct xt_entry_target **target)
{
	struct ipt_IPCRACK_info *info = (struct ipt_TTL_info *) (*target)->data;
	unsigned int value;

	if (*flags & IPT_IPCRACK_USED) {
		exit_error(PARAMETER_PROBLEM, 
				"Can't specify TTL option twice");
	}

	if (!optarg) 
		exit_error(PARAMETER_PROBLEM, 
				"TTL: You must specify a value");

	if (check_inverse(optarg, &invert, NULL, 0))
		exit_error(PARAMETER_PROBLEM,
				"TTL: unexpected `!'");
	
	if (string_to_number(optarg, 0, 255, &value) == -1)
		exit_error(PARAMETER_PROBLEM,
		           "TTL: Expected value between 0 and 255");

	info->ttl = value;
	*flags |= IPT_IPCRACK_USED;

	return 1;
}

static void IPCRACK_check(unsigned int flags)
{
	if (!(flags & IPT_IPCRACK_USED))
		exit_error(PARAMETER_PROBLEM,
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

static struct iptables_target ipcrack_target = {
	.next		= NULL, 
	.name		= "IPCRACK",
	.version	= IPTABLES_VERSION,
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
	register_target(&ipcrack_target);
}
