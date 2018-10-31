/* Shared library add-on to iptables to add UTTWEBIND target support.
 *
 * Copyright (c) 2000 Marc Boucher
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include <xtables.h>
#include <linux/netfilter/x_tables.h>

static void UTTWEBIND_help(void)
{
    printf("UTTWEBIND target: indentify web url and dns host\n");
}


static int UTTWEBIND_parse(int c, char **argv, int invert, unsigned int *flags,
                        const void *entry, struct xt_entry_target **target)
{	
#if 0
    struct xt_uttwebind_info *info
		= (struct xt_uttwebind_info *)(*target)->data;
    char *secStr = NULL;
    u_int8_t priF;
    u_int32_t secF;

    switch (c) {
	case '1' :
	    if (!optarg) {
		xtables_error(PARAMETER_PROBLEM, "UTTWEBIND target: need arg\n");
	    }
	    /* ÊÇ·ñ°üº¬, */
	    if (!(secStr = strstr(optarg, UTTIND_ARG_SPE))) {
		xtables_error(PARAMETER_PROBLEM, "UTTWEBIND target: arg should contain ,\n");
	    }
	    /* ×ª»»´íÎó */
	    priF = (u_int8_t) strtol(optarg, NULL, 10);
	    secF = (u_int32_t) strtoul(secStr + 1, NULL, 10);
	    printf("priF=%d,secF=%d\n", priF, secF);
	    if ((priF == 0) || (secF == 0)) {
		xtables_error(PARAMETER_PROBLEM, "UTTWEBIND target: error arg\n");
	    }

	    info->priFlag = priF;
	    info->secFlag = secF;

	    break;
	default:
	    return 0;
    }
#endif
    return 1;
}

static void UTTWEBIND_check(unsigned int flags)
{
    return;
}

/* Prints out the targinfo. */
static void UTTWEBIND_print(const void *ip, const struct xt_entry_target *target,
                         int numeric)
{
    printf("WEBIND");
}

/* Saves the union ipt_targinfo in parsable form to stdout. */
static void UTTWEBIND_save(const void *ip, const struct xt_entry_target *target)
{
}

static struct xtables_target uttwebind_target = {
	.family		= AF_INET,
	.name		= "UTTWEBIND",
	.version	= IPTABLES_VERSION,
	.size		= XT_ALIGN(sizeof(int)),
	.userspacesize	= XT_ALIGN(sizeof(int)),
	.help		= UTTWEBIND_help,
	.parse		= UTTWEBIND_parse,
	.final_check	= UTTWEBIND_check,
	.print		= UTTWEBIND_print,
	.save		= UTTWEBIND_save,
};

void _init(void)
{
	xtables_register_target(&uttwebind_target);
}
