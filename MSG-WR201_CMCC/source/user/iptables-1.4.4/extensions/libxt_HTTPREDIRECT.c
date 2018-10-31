/* Shared library add-on to iptables to add HTTPREDIRECT target support.
 *
 * Copyright (c) 2000 Marc Boucher
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include <xtables.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_HTTPREDIRECT.h>


static void HTTPREDIRECT_help(void)
{
    printf("HTTPREDIRECT target options:\n"
" --redirect-dir value	    no more than 20 character\n"	    
" --return value    if HTTPREDIRECT not work, which operation it will do. d - drop, c - continue, r - return\n"	    
	    );
}

static const struct option HTTPREDIRECT_opts[] = {
	{ "redirect-dir", 1, NULL, '1' },
	{ "return", 1, NULL, '2' },
	{ }
};

static int HTTPREDIRECT_parse(int c, char **argv, int invert, unsigned int *flags,
                        const void *entry, struct xt_entry_target **target)
{	
    struct xt_httpredirect_info *httpr_info
		= (struct xt_httpredirect_info *)(*target)->data;

    switch (c) {
	case '1' :
	    if (strlen(optarg) > HTTP_R_LEN) {
		xtables_error(PARAMETER_PROBLEM, "HTTPREDIRECT target: optarg[%s] len exceed %d", optarg, HTTP_R_LEN);
	    }
	    strncpy(httpr_info->redirect_dir, optarg, HTTP_R_LEN);
	    break;
	case '2':
	    if (strcmp(optarg, "c") == 0) {
		httpr_info->ex_op = XT_C;
	    } else if (strcmp(optarg, "r") == 0) {
		httpr_info->ex_op = XT_R;
	    } else if (strcmp(optarg, "d") == 0) {
		httpr_info->ex_op = XT_D;
	    } else {
		xtables_error(PARAMETER_PROBLEM, "HTTPREDIRECT target: optarg[%s] should be c,r or d", optarg);
	    }
	    break;
	default:
	    return 0;
    }
    return 1;
}

static void HTTPREDIRECT_check(unsigned int flags)
{
    return;
}

/* Prints out the targinfo. */
static void HTTPREDIRECT_print(const void *ip, const struct xt_entry_target *target,
                         int numeric)
{
    struct xt_httpredirect_info *httpr_info
		= (struct xt_httpredirect_info *)target->data;

    if (strlen(httpr_info->redirect_dir) != 0) {
	printf("redirect-dir %s", httpr_info->redirect_dir);
    }

    switch(httpr_info->ex_op) {
	case XT_C:
	    printf(" ex_retrun continue");
	    break;
	case XT_R:
	    printf(" ex_retrun return");
	    break;
	case XT_D:
	    printf(" ex_retrun drop");
	    break;
	default:
	    break;
    }
}

/* Saves the union ipt_targinfo in parsable form to stdout. */
static void HTTPREDIRECT_save(const void *ip, const struct xt_entry_target *target)
{
}

static struct xtables_target httpredirect_target = {
	.family		= AF_INET,
	.name		= "HTTPREDIRECT",
	.version	= XTABLES_VERSION,
	.size		= XT_ALIGN(sizeof(struct xt_httpredirect_info)),
	.userspacesize	= XT_ALIGN(sizeof(struct xt_httpredirect_info)),
	.help		= HTTPREDIRECT_help,
	.parse		= HTTPREDIRECT_parse,
	.final_check	= HTTPREDIRECT_check,
	.print		= HTTPREDIRECT_print,
	.save		= HTTPREDIRECT_save,
	.extra_opts	= HTTPREDIRECT_opts,
};

void _init(void)
{
	xtables_register_target(&httpredirect_target);
}
