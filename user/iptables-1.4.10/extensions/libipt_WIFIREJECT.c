/* Shared library add-on to iptables to add customized REJECT support.
 *
 * (C) 2000 Jozsef Kadlecsik <kadlec@blackhole.kfki.hu>
 */
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <xtables.h>
#include <linux/netfilter_ipv4/ipt_WIFIREJECT.h>
#include <linux/version.h>
#include <netinet/in.h>

/* If we are compiling against a kernel that does not support
 * IPT_ICMP_ADMIN_PROHIBITED, we are emulating it.
 * The result will be a plain DROP of the packet instead of
 * reject. -- Maciej Soltysiak <solt@dns.toxicfilms.tv>
#ifndef IPT_ICMP_ADMIN_PROHIBITED
#define IPT_ICMP_ADMIN_PROHIBITED	IPT_TCP_RESET + 1
#endif
 */

#define UTTDNS_ARG_SPE	":"

static void WIFIREJECT_help(void)
{
    printf("WIFIREJECT target options:\n"
" --redirect-dir value	    no more than 20 character\n"	    
" --redirect-val value      such as: sn\n"	    
" --auth-domain value       auth server domain,such as: domain:port\n"
" --login-url value         login request url\n"
" --unit-price value        unit price for charge network\n"
	    );
}

static const struct option WIFIREJECT_opts[] = {
	{ "redirect-dir", 1, NULL, '1' },
	{ "redirect-val", 1, NULL, '2' },
	{ "auth-domain", 1, NULL, '3' },
	{ "login-url", 1, NULL, '4' },
	{ "secret-key", 1, NULL, '5' },
	{ "server-version", 1, NULL, '6' },
	{ "unit-price", 1, NULL, '7' },
	{ }
};

static void WIFIREJECT_init(struct xt_entry_target *t)
{
#if 0
	struct ipt_wifireject_info *reject = (struct ipt_wifireject_info *)t->data;

	/* default */
	reject->ex_op = XT_D;
#endif
}

static int WIFIREJECT_parse(int c, char **argv, int invert, unsigned int *flags,
                        const void *entry, struct xt_entry_target **target)
{
	struct ipt_wifireject_info *reject = (struct ipt_wifireject_info *)(*target)->data;

	switch(c) {
	case '1':
	    if (strlen(optarg) > HTTP_R_LEN) {
		xtables_error(PARAMETER_PROBLEM, "WIFIREJECT target: optarg[%s] len exceed %d", optarg, HTTP_R_LEN);
	    }
	    strncpy(reject->redirect_dir, optarg, HTTP_R_LEN);
	    //reject->ex_op = XT_D;
	    break;
	case '2':
	    if ((strlen(optarg) > HTTP_SN_LEN) || (strlen(optarg) < 7)) {
		xtables_error(PARAMETER_PROBLEM, "WIFIREJECT target: optarg[%s] len exceed %d", optarg, HTTP_SN_LEN);
	    }
	    memset(reject->redirect_dir, 0, sizeof(reject->redirect_dir));                                                            
	    strncpy(reject->redirect_val, optarg, HTTP_SN_LEN);
	    //reject->ex_op = XT_D;
	    break;
	case '3':
	    if ((strlen(optarg) > HTTP_DOMAIN_LEN)) {
		xtables_error(PARAMETER_PROBLEM, "WIFIREJECT target: optarg[%s] len exceed %d", optarg, HTTP_DOMAIN_LEN);
	    }
	    strncpy(reject->auth_domain, optarg, HTTP_DOMAIN_LEN);
	    break;
	case '4':
	    if ((strlen(optarg) > HTTP_LOGIN_URL_LEN)) {
		xtables_error(PARAMETER_PROBLEM, "WIFIREJECT target: optarg[%s] len exceed %d", optarg, HTTP_LOGIN_URL_LEN);
	    }
	    strncpy(reject->login_url, optarg, HTTP_LOGIN_URL_LEN);
	    break;
	case '5':
	    if ((strlen(optarg) > HTTP_SECRET_KEY_LEN)) {
		xtables_error(PARAMETER_PROBLEM, "WIFIREJECT target: optarg[%s] len exceed %d", optarg, HTTP_SECRET_KEY_LEN);
	    }
	    strncpy(reject->secret_key, optarg, HTTP_SECRET_KEY_LEN);
	    break;

	case '6':
	    if ((strlen(optarg) > SERVER_VERSION_LEN)) {
		xtables_error(PARAMETER_PROBLEM, "WIFIREJECT target: optarg[%s] len exceed %d", optarg, HTTP_SECRET_KEY_LEN);
	    }
	    strncpy(reject->server_version, optarg, SERVER_VERSION_LEN);
	    break;
	case '7':
	    if ((strlen(optarg) > UNIT_PRICE_LEN)) {
		xtables_error(PARAMETER_PROBLEM, "WIFIREJECT target: optarg[%s] len exceed %d", optarg, UNIT_PRICE_LEN);
	    }
	    strncpy(reject->unitPrice, optarg, UNIT_PRICE_LEN);
	    break;

	default:
		/* Fall through */
		xtables_error(PARAMETER_PROBLEM, "WIFIREJECT target: Invalid argument");
		return 0;
	}
	return 1;
}

static void WIFIREJECT_print(const void *ip, const struct xt_entry_target *target,
                         int numeric)
{
    struct ipt_wifireject_info *wifi_info
		= (struct ipt_wifireject_info *)target->data;

    if (strlen(wifi_info->redirect_dir) != 0) {
	printf(" redirect-dir %s", wifi_info->redirect_dir);
    }
    if (strlen(wifi_info->redirect_val) != 0) {
	printf(" redirect-val %s", wifi_info->redirect_val);
    }
    if (strlen(wifi_info->auth_domain) != 0) {
	printf(" auth-domain %s", wifi_info->auth_domain);
    }
    if (strlen(wifi_info->login_url) != 0) {
	printf("login-url %s", wifi_info->login_url);
    }
    if (wifi_info->unitPrice[0] != '\0') {
	printf("unit-price %s", wifi_info->unitPrice);
    }
}

static void WIFIREJECT_save(const void *ip, const struct xt_entry_target *target)
{
}

static struct xtables_target reject_tg_reg = {
	.name		= "WIFIREJECT",
	.version	= XTABLES_VERSION,
	.family		= NFPROTO_IPV4,
	.size		= XT_ALIGN(sizeof(struct ipt_wifireject_info)),
	.userspacesize	= XT_ALIGN(sizeof(struct ipt_wifireject_info)),
	.help		= WIFIREJECT_help,
	.init		= WIFIREJECT_init,
	.parse		= WIFIREJECT_parse,
	.print		= WIFIREJECT_print,
	.save		= WIFIREJECT_save,
	.extra_opts	= WIFIREJECT_opts,
};

void _init(void)
{
	xtables_register_target(&reject_tg_reg);
}
