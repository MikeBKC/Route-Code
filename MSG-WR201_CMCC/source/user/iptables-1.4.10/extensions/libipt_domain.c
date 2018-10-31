#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <iptables.h>
#include <xtables.h>

#define DOMAIN_VERSION          "0.0.2"
#define MAX_CHAR                50

struct ipt_domain_info
{
    char name[MAX_CHAR];
    char org_name[MAX_CHAR];
    int len;
};

static void help(void)
{
	printf(
	"domain v%s options:\n"
	"  --name \"www.chinaunix.net\"            Match the domain named \"www.chinaunix.net\"\n"
	"  --name \"chinaunix.net\"                Match the domain named \"chinaunix.net\"\n"
	"                                        include www.chinunix.net, bbs.chinaunix.net, man.chinaunix.net\n"
	"\n", DOMAIN_VERSION);
}


static struct option opts[] = {
	{ "name", 1, 0, '1' },
	{ 0 }
};


static void init(struct ipt_entry_match *m)
{
	struct ipt_domain_info *info = (struct ipt_domain_info *)m->data;
	/**nfcache |= NFC_UNKNOWN;*/
	memset(info->name, 0, MAX_CHAR);
	memset(info->org_name, 0, MAX_CHAR);
}


static void parse_name(char *name, struct ipt_domain_info *info)
{
	int i, char_len = 0;
	int len = strlen(name) + 1;

	if (len > MAX_CHAR)
		xtables_error(PARAMETER_PROBLEM,
					"The strings `%s' is too long (MAX_CHAR = %d",
					name, MAX_CHAR);

	strcpy(info->name + 1, name);
	strcpy(info->org_name, name);
	info->name[0] = '.';

	for (i = len - 1; i >= 0; i--) {
		if (info->name[i] == '.') {
			info->name[i] = char_len;
			char_len = 0;
		} else {
			char_len++;
		}
	}
	info->len = len;

	/* ×Ö·û×ª»» debug ²¿·Ö */
/*
	printf("\n");
	for (i = 0; i < len; i++)
		printf("%X ", (unsigned char)info->name[i]);

	printf("\n");
*/
}


static int parse(int c, char **argv, int invert, unsigned int *flags,
			const struct ipt_entry *entry,
			/*unsigned int *nfcache, */
			struct ipt_entry_match **match)
{
	struct ipt_domain_info *info = (struct ipt_domain_info *)(*match)->data; 

	xtables_check_inverse(optarg, &invert, &optind, 0, argv);
	switch (c) {
		case '1':
			parse_name(argv[optind - 1], info);
			*flags = 1;
			break;

		default:
			return 0;
	}

	return 1;
}


static void final_check(unsigned int flags)
{
	if (!flags) xtables_error(PARAMETER_PROBLEM, "domain expection an option");
}


static void print(const struct ipt_ip *ip, const struct ipt_entry_match *match,
			int numeric)
{
	const struct ipt_domain_info *info = (const struct ipt_domain_info *)match->data;

	printf("domain --name \"%s\"", info->org_name);
}


static void save(const struct ipt_ip *ip, const struct ipt_entry_match *match)
{

	const struct ipt_domain_info *info = (const struct ipt_domain_info *)match->data;

	printf("--name \"%s\" ", info->org_name);
}


static struct xtables_match domain = {  /* iptables_match */
//	.next		= NULL, 
	.name		= "domain",
//	.revision	= 0, 
	.version	= XTABLES_VERSION,
	.family		= NFPROTO_IPV4,
	.size		= IPT_ALIGN(sizeof(struct ipt_domain_info)),
	.userspacesize	= IPT_ALIGN(sizeof(struct ipt_domain_info)),
	.help		= &help,
	.init		= &init,
	.parse		= &parse,
	.final_check	= &final_check,
	.print		= &print,
	.save		= &save,
	.extra_opts	= opts
};


void _init(void)
{
	xtables_register_match(&domain);
}
