/* Shared library add-on to iptables to add HTTPPROXY target support.
 *
 * Copyright (c) 2000 Marc Boucher
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include <xtables.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_HTTPPROXY.h>


static void HTTPPROXY_help(void)
{
    printf("HTTPPROXY target options:\n"
" --proxy-dir value	    no more than 35 character\n"	    
" --proxy-file value	    no more than 27 character\n"	    
	    );
}

static const struct option HTTPPROXY_opts[] = {
	{ "proxy-dir", 1, NULL, '1' },
	{ "proxy-file", 1, NULL, '2' },
	{ }
};

static int HTTPPROXY_parse(int c, char **argv, int invert, unsigned int *flags,
                        const void *entry, struct xt_entry_target **target)
{	
    struct xt_httpproxy_info *http_proxy_info
		= (struct xt_httpproxy_info *)(*target)->data;

    switch (c) {
	case '1' :
	    if (optarg != NULL && strlen(optarg) <= HTTP_PROXY_DIR_LEN) {
		strncpy(http_proxy_info->proxy_dir, optarg, HTTP_PROXY_DIR_LEN);
#if 0
		printf("-----func:%s,line:%d,proxy_dir=%s---\n",__func__,__LINE__,http_proxy_info->proxy_dir);
#endif
	    } else {
		xtables_error(PARAMETER_PROBLEM, "HTTPPROXY target: optarg[%s] len exceed %d", optarg, HTTP_PROXY_DIR_LEN);
	    }
	    break;
	case '2' :
	    if (optarg != NULL && strlen(optarg) <= HTTP_PROXY_FILE_LEN) {
		strncpy(http_proxy_info->proxy_file, optarg, HTTP_PROXY_FILE_LEN);
#if 0
		printf("-----func:%s,line:%d,proxy_file=%s---\n",__func__,__LINE__,http_proxy_info->proxy_file);
#endif
	    } else {
		xtables_error(PARAMETER_PROBLEM, "HTTPPROXY target: optarg[%s] len exceed %d", optarg, HTTP_PROXY_FILE_LEN);
	    }
	    break;
	default:
	    return 0;
    }
    return 1;
}

static void HTTPPROXY_check(unsigned int flags)
{
    return;
}

/* Prints out the targinfo. */
static void HTTPPROXY_print(const void *ip, const struct xt_entry_target *target,
                         int numeric)
{
    struct xt_httpproxy_info *http_proxy_info
		= (struct xt_httpproxy_info *)target->data;

    if (strlen(http_proxy_info->proxy_dir) != 0) {
	printf("proxy-dir %s", http_proxy_info->proxy_dir);
    }

    if (strlen(http_proxy_info->proxy_file) != 0) {
	printf("proxy-file %s", http_proxy_info->proxy_file);
    }

}

/* Saves the union ipt_targinfo in parsable form to stdout. */
static void HTTPPROXY_save(const void *ip, const struct xt_entry_target *target)
{
}

static struct xtables_target httpproxy_target = {
	.family		= AF_INET,
	.name		= "HTTPPROXY",
	.version	= XTABLES_VERSION,
	.size		= XT_ALIGN(sizeof(struct xt_httpproxy_info)),
	.userspacesize	= XT_ALIGN(sizeof(struct xt_httpproxy_info)),
	.help		= HTTPPROXY_help,
	.parse		= HTTPPROXY_parse,
	.final_check	= HTTPPROXY_check,
	.print		= HTTPPROXY_print,
	.save		= HTTPPROXY_save,
	.extra_opts	= HTTPPROXY_opts,
};

void _init(void)
{
	xtables_register_target(&httpproxy_target);
}
