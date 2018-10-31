/* Shared library add-on to iptables to add UTTINDENTIFY target support.
 *
 * Copyright (c) 2000 Marc Boucher
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include <xtables.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_UTTINDENTIFY.h>

#define UTTIND_ARG_SPE	","


static void UTTINDENTIFY_help(void)
{
    printf("UTTINDENTIFY target options:\n"
" --set priFlag,secFlag.eg, --set 1,2\n"
	    );
}

static const struct option UTTINDENTIFY_opts[] = {
	{ "set", 1, NULL, '1' },
	{ }
};

static int UTTINDENTIFY_parse(int c, char **argv, int invert, unsigned int *flags,
                        const void *entry, struct xt_entry_target **target)
{	
    struct xt_uttindentify_info *info
		= (struct xt_uttindentify_info *)(*target)->data;
    char *secStr = NULL;
    u_int8_t priF;
    u_int32_t secF;

    switch (c) {
	case '1' :
	    if (!optarg) {
		xtables_error(PARAMETER_PROBLEM, "UTTINDENTIFY target: need arg\n");
	    }
	    /* ÊÇ·ñ°üº¬, */
	    if (!(secStr = strstr(optarg, UTTIND_ARG_SPE))) {
		xtables_error(PARAMETER_PROBLEM, "UTTINDENTIFY target: arg should contain ,\n");
	    }
	    /* ×ª»»´íÎó */
	    priF = (u_int8_t) strtol(optarg, NULL, 10);
	    secF = (u_int32_t) strtoul(secStr + 1, NULL, 10);
	    printf("priF=%d,secF=%d\n", priF, secF);
	    if ((priF == 0) || (secF == 0)) {
		xtables_error(PARAMETER_PROBLEM, "UTTINDENTIFY target: error arg\n");
	    }

	    info->priFlag = priF;
	    info->secFlag = secF;

	    break;
	default:
	    return 0;
    }
    return 1;
}

static void UTTINDENTIFY_check(unsigned int flags)
{
    return;
}

/* Prints out the targinfo. */
static void UTTINDENTIFY_print(const void *ip, const struct xt_entry_target *target,
                         int numeric)
{
    struct xt_uttindentify_info *info
		= (struct xt_uttindentify_info *)target->data;

    printf("IND SET %d,%d", info->priFlag, info->secFlag);
}

/* Saves the union ipt_targinfo in parsable form to stdout. */
static void UTTINDENTIFY_save(const void *ip, const struct xt_entry_target *target)
{
}

static struct xtables_target uttindentify_target = {
	.family		= AF_INET,
	.name		= "UTTINDENTIFY",
	.version	= XTABLES_VERSION,
	.size		= XT_ALIGN(sizeof(struct xt_uttindentify_info)),
	.userspacesize	= XT_ALIGN(sizeof(struct xt_uttindentify_info)),
	.help		= UTTINDENTIFY_help,
	.parse		= UTTINDENTIFY_parse,
	.final_check	= UTTINDENTIFY_check,
	.print		= UTTINDENTIFY_print,
	.save		= UTTINDENTIFY_save,
	.extra_opts	= UTTINDENTIFY_opts,
};

void _init(void)
{
	xtables_register_target(&uttindentify_target);
}
