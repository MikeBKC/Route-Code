/* vi: set sw=4 ts=4: */
/*
 * files.c -- DHCP server file manipulation *
 * Rewrite by Russ Dill <Russ.Dill@asu.edu> July 2001
 */

#include <netinet/ether.h>

#include "common.h"
#include "dhcpd.h"
#include "options.h"

#if ( CLOUD_CONFIG == FYES)

#include	"mib.h"
#include	"profacce.h"
#include "mibunionShare.h"

static void wirte_hostname_noticemac(struct dhcpOfferedAddr *lease);
#endif
#ifdef DHCP_SERVER_MULTIIF
struct _dhcp_pool_t *dhcp_pool_ptr = NULL;
/* on these functions, make sure your datatype matches */
static int read_start_ip(const char *line, void *arg)
{
	len_and_sockaddr *lsa;

	lsa = host_and_af2sockaddr(line, 0, AF_INET);
	if (!lsa)
		return 0;

	dhcp_pool_ptr->start_ip = ntohl(lsa->u.sin.sin_addr.s_addr);

	free(lsa);
	return 1;
}

static int read_end_ip(const char *line, void *arg)
{
	len_and_sockaddr *lsa;

	lsa = host_and_af2sockaddr(line, 0, AF_INET);
	if (!lsa)
		return 0;

	dhcp_pool_ptr->end_ip = ntohl(lsa->u.sin.sin_addr.s_addr);

	free(lsa);
	return 1;
}
#endif

static int read_ip(const char *line, void *arg)
{
	len_and_sockaddr *lsa;

	lsa = host_and_af2sockaddr(line, 0, AF_INET);
	if (!lsa)
		return 0;
	*(uint32_t*)arg = lsa->u.sin.sin_addr.s_addr;
	free(lsa);
	return 1;
}

static int read_mac(const char *line, void *arg)
{
	struct ether_addr *temp_ether_addr;

	temp_ether_addr = ether_aton_r(line, (struct ether_addr *)arg);
	if (temp_ether_addr == NULL)
		return 0;
	return 1;
}


static int read_str(const char *line, void *arg)
{
	char **dest = arg;

	free(*dest);
	*dest = xstrdup(line);
	return 1;
}

#ifdef DHCP_SERVER_MULTIIF
static int read_max_leases(const char *line, void *arg)
{
	dhcp_pool_ptr->max_leases = bb_strtou32(line, NULL, 10);

	return errno == 0;
}
#endif

static int read_u32(const char *line, void *arg)
{
	*(uint32_t*)arg = bb_strtou32(line, NULL, 10);

	return errno == 0;
}

#ifdef DHCP_SERVER_MULTIIF
/*
 * 功能： 从配置文件中读取vid，并分配该vlan地址池所需内存
 * 
 * 作者： KeHuatao
 * 日期:  20131204
 */
static int read_vid(const char *line, void *arg)
{
	uint32_t val = 0u, found = 0u;
	dhcp_pool_t *tmp = NULL, *prev = NULL;
	uint32_t key = 0;

	val = bb_strtou32(line, NULL, 10);

	if ((val < 0u) || (val >= 4095u)) {
		return errno == 0;
	}

	key = dhcp_server_hash(val);
	dbck("%d in %s of %s, val = %d\n", __LINE__, __func__, __FILE__, val);

	tmp = server_config.dhcp_pool[key].next;
	prev = &(server_config.dhcp_pool[key]);
	/*遍历pool链表*/
	while(tmp!=NULL){
		/*各节点按vid由小到大顺序链接*/
		if(tmp->vid < val)
		{
			dbck("%d in %s of %s, tmp->vid = %d\n", __LINE__, __func__, __FILE__, tmp->vid);
			prev = tmp;
			tmp = tmp -> next;
		}
		else if(tmp->vid == val)
		{
			dbck("%d in %s of %s, tmp->vid = %d\n", __LINE__, __func__, __FILE__, tmp->vid);
			found  = 1;
			break;
		}
		else
		{
			dbck("%d in %s of %s, tmp->vid = %d\n", __LINE__, __func__, __FILE__, tmp->vid);
			tmp = prev;
			break;
		}
	}

	if(found==0u)
	{
		dbck("%d in %s of %s, val = %d\n", __LINE__, __func__, __FILE__, val);
		tmp = (dhcp_pool_t*)xzalloc(sizeof(dhcp_pool_t));
		tmp->vid = val;
		tmp->max_leases = 1024;
		tmp->leases = NULL;
		tmp->next = prev->next;
		prev->next = tmp;
	} else {
		free_lease(tmp, 0);
	}
	dhcp_pool_ptr = tmp;

	return errno == 0;
}
#ifdef DHCP_SERVER_MULTIIF
extern void clear_lease(const uint8_t *chaddr, uint32_t yiaddr, dhcp_pool_t *tmp);
void free_lease(dhcp_pool_t *node, int free_node)
{
	struct static_lease *next = NULL, *tmp = NULL;
	struct option_set *curr = NULL, *follow = NULL;

	dbck("free_lease vid %d\n", node->vid);
	tmp = node->static_leases;
	while (tmp != NULL) {
		next = tmp->next;

		free(tmp->mac);
		free(tmp->ip);
		free(tmp);

		tmp = next;
	}
	node->static_leases = NULL;

	curr = node->options;
	while (curr != NULL) {
		follow = curr->next;

		free(curr->data);
		free(curr);

		curr = follow;
	}

	node->options = NULL;

	if (free_node == 1) {
		free(node);
	}

	return;
}
#endif

static int del_vlan(const char *line, void *arg)
{
	uint32_t val = 0u, found = 0u;
	dhcp_pool_t *tmp = NULL, *prev = NULL;
	uint32_t key = 0;

	val = bb_strtou32(line, NULL, 10);

	dbck("%d in %s of %s, vid = %d\n", __LINE__, __func__, __FILE__, val);

	if ((val < 0u) || (val >= 4095u)) {
		return tmp;
	}

	key = dhcp_server_hash(val);

	tmp = server_config.dhcp_pool[key].next;
	prev = &(server_config.dhcp_pool[key]);
	/*遍历pool链表*/
	while(tmp!=NULL){
		/*各节点按vid由小到大顺序链接*/
		if(tmp->vid < val)
		{
			prev = tmp;
			tmp = tmp -> next;
		}
		else if(tmp->vid == val)
		{
			prev->next = tmp->next;
			found  = 1;
			break;
		}
		else
		{
			break;
		}
	};
	if(found == 1)
	{
		dbck("%d in %s of %s, vid = %d\n", __LINE__, __func__, __FILE__, val);
		free_lease(tmp, 1);
	}

	return errno == 0;
}
#endif
static int read_yn(const char *line, void *arg)
{
	char *dest = arg;

	if (!strcasecmp("yes", line)) {
		*dest = 1;
		return 1;
	}
	if (!strcasecmp("no", line)) {
		*dest = 0;
		return 1;
	}
	return 0;
}


/* find option 'code' in opt_list */
struct option_set *find_option(struct option_set *opt_list, uint8_t code)
{
	while (opt_list && opt_list->data[OPT_CODE] < code)
		opt_list = opt_list->next;

	if (opt_list && opt_list->data[OPT_CODE] == code)
		return opt_list;
	return NULL;
}


/* add an option to the opt_list */
static void attach_option(struct option_set **opt_list,
		const struct dhcp_option *option, char *buffer, int length)
{
	struct option_set *existing, *new, **curr;

	existing = find_option(*opt_list, option->code);
	if (!existing) {
		DEBUG("Attaching option %02x to list", option->code);

#if ENABLE_FEATURE_RFC3397
		if ((option->flags & TYPE_MASK) == OPTION_STR1035)
			/* reuse buffer and length for RFC1035-formatted string */
			buffer = (char *)dname_enc(NULL, 0, buffer, &length);
#endif

		/* make a new option */
		new = xmalloc(sizeof(*new));
		new->data = xmalloc(length + 2);
		new->data[OPT_CODE] = option->code;
		new->data[OPT_LEN] = length;
		memcpy(new->data + 2, buffer, length);

		curr = opt_list;
		while (*curr && (*curr)->data[OPT_CODE] < option->code)
			curr = &(*curr)->next;

		new->next = *curr;
		*curr = new;
#if ENABLE_FEATURE_RFC3397
		if ((option->flags & TYPE_MASK) == OPTION_STR1035 && buffer != NULL)
			free(buffer);
#endif
		return;
	}

	/* add it to an existing option */
	DEBUG("Attaching option %02x to existing member of list", option->code);
	if (option->flags & OPTION_LIST) {
#if ENABLE_FEATURE_RFC3397
		if ((option->flags & TYPE_MASK) == OPTION_STR1035)
			/* reuse buffer and length for RFC1035-formatted string */
			buffer = (char *)dname_enc(existing->data + 2,
					existing->data[OPT_LEN], buffer, &length);
#endif
		if (existing->data[OPT_LEN] + length <= 255) {
			existing->data = xrealloc(existing->data,
					existing->data[OPT_LEN] + length + 3);
			if ((option->flags & TYPE_MASK) == OPTION_STRING) {
				/* ' ' can bring us to 256 - bad */
				if (existing->data[OPT_LEN] + length >= 255)
					return;
				/* add space separator between STRING options in a list */
				existing->data[existing->data[OPT_LEN] + 2] = ' ';
				existing->data[OPT_LEN]++;
			}
			memcpy(existing->data + existing->data[OPT_LEN] + 2, buffer, length);
			existing->data[OPT_LEN] += length;
		} /* else, ignore the data, we could put this in a second option in the future */
#if ENABLE_FEATURE_RFC3397
		if ((option->flags & TYPE_MASK) == OPTION_STR1035 && buffer != NULL)
			free(buffer);
#endif
	} /* else, ignore the new data */
}


/*added by brwang for mantis bug 8051*/
extern uint32_t utt_dhcpSrvNm;
/* read a dhcp option and add it to opt_list */
static int read_opt(const char *const_line, void *arg)
{
#ifdef DHCP_SERVER_MULTIIF
	struct option_set **opt_list = &(dhcp_pool_ptr->options);
#else
	struct option_set **opt_list = arg;
#endif
	char *opt, *val, *endptr;
	char *line;
	const struct dhcp_option *option;
	int retval, length, idx;
	char buffer[8] ALIGNED(4);
	uint16_t *result_u16 = (uint16_t *) buffer;
	uint32_t *result_u32 = (uint32_t *) buffer;

	/* Cheat, the only const line we'll actually get is "" */
	line = (char *) const_line;
	opt = strtok(line, " \t=");
	if (!opt)
		return 0;

	idx = index_in_strings(dhcp_option_strings, opt); /* NB: was strcasecmp! */
	if (idx < 0)
		return 0;
	option = &dhcp_options[idx];

	retval = 0;
	do {
		val = strtok(NULL, ", \t");
		if (!val) break;
		length = dhcp_option_lengths[option->flags & TYPE_MASK];
		retval = 0;
		opt = buffer; /* new meaning for variable opt */
		switch (option->flags & TYPE_MASK) {
		case OPTION_IP:
			retval = read_ip(val, buffer);
            if(option->code == DHCP_SUBNET) {
                utt_dhcpSrvNm = ntohl(*((uint32_t*)buffer));
            }
			break;
		case OPTION_IP_PAIR:
			retval = read_ip(val, buffer);
			val = strtok(NULL, ", \t/-");
			if (!val)
				retval = 0;
			if (retval)
				retval = read_ip(val, buffer + 4);
			break;
		case OPTION_STRING:
#if ENABLE_FEATURE_RFC3397
		case OPTION_STR1035:
#endif
			length = strlen(val);
			if (length > 0) {
				if (length > 254) length = 254;
				opt = val;
				retval = 1;
			}
			break;
		case OPTION_BOOLEAN:
			retval = read_yn(val, buffer);
			break;
		case OPTION_U8:
			buffer[0] = strtoul(val, &endptr, 0);
			retval = (endptr[0] == '\0');
			break;
		/* htonX are macros in older libc's, using temp var
		 * in code below for safety */
		/* TODO: use bb_strtoX? */
		case OPTION_U16: {
			unsigned long tmp = strtoul(val, &endptr, 0);
			*result_u16 = htons(tmp);
			retval = (endptr[0] == '\0' /*&& tmp < 0x10000*/);
			break;
		}
		case OPTION_S16: {
			long tmp = strtol(val, &endptr, 0);
			*result_u16 = htons(tmp);
			retval = (endptr[0] == '\0');
			break;
		}
		case OPTION_U32: {
			unsigned long tmp = strtoul(val, &endptr, 0);
			*result_u32 = htonl(tmp);
			retval = (endptr[0] == '\0');
			break;
		}
		case OPTION_S32: {
			long tmp = strtol(val, &endptr, 0);
			*result_u32 = htonl(tmp);
			retval = (endptr[0] == '\0');
			break;
		}
		default:
			break;
		}
		if (retval)
			attach_option(opt_list, option, opt, length);
	} while (retval && option->flags & OPTION_LIST);
	return retval;
}

static int read_staticlease(const char *const_line, void *arg)
{
	char *line;
	char *mac_string;
	char *ip_string;
	uint8_t *mac_bytes;
	uint32_t *ip;

	/* Allocate memory for addresses */
	mac_bytes = xmalloc(sizeof(unsigned char) * 8);
	ip = xmalloc(sizeof(uint32_t));

	/* Read mac */
	line = (char *) const_line;
	mac_string = strtok(line, " \t");
	read_mac(mac_string, mac_bytes);

	/* Read ip */
	ip_string = strtok(NULL, " \t");
	read_ip(ip_string, ip);

#ifdef DHCP_SERVER_MULTIIF
	addStaticLease(&(dhcp_pool_ptr->static_leases), mac_bytes, ip);

	if (ENABLE_FEATURE_UDHCP_DEBUG) printStaticLeases(&(dhcp_pool_ptr->static_leases));
#else
	addStaticLease(arg, mac_bytes, ip);

	if (ENABLE_FEATURE_UDHCP_DEBUG) printStaticLeases(arg);
#endif


	return 1;
}


struct config_keyword {
	const char *keyword;
	int (*handler)(const char *line, void *var);
	void *var;
	const char *def;
};

static const struct config_keyword keywords[] = {
	/* keyword       handler   variable address               default */
#ifndef DHCP_SERVER_MULTIIF
	{"start",        read_ip,  &(server_config.start_ip),     "192.168.0.20"},
	{"end",          read_ip,  &(server_config.end_ip),       "192.168.0.254"},
	{"max_leases",   read_u32, &(server_config.max_leases),   "1024"},
#endif
	{"interface",    read_str, &(server_config.interface),    "eth0"},
	/* Avoid "max_leases value not sane" warning by setting default
	 * to default_end_ip - default_start_ip + 1: */
	{"remaining",    read_yn,  &(server_config.remaining),    "yes"},
	{"auto_time",    read_u32, &(server_config.auto_time),    "7200"},
	{"decline_time", read_u32, &(server_config.decline_time), "3600"},
	{"conflict_time",read_u32, &(server_config.conflict_time),"3600"},
	{"offer_time",   read_u32, &(server_config.offer_time),   "60"},
	{"min_lease",    read_u32, &(server_config.min_lease),    "60"},
	{"pidfile",      read_str, &(server_config.pidfile),      "/var/run/udhcpd.pid"},
	{"siaddr",       read_ip,  &(server_config.siaddr),       "0.0.0.0"},
	{"lease_file",   read_str, &(server_config.lease_file),   LEASES_FILE},
	/* keywords with no defaults must be last! */
#ifdef DHCP_SERVER_MULTIIF
	{"option",       read_opt, NULL,      ""},
	{"opt",          read_opt, NULL,      ""},
#else
	{"option",       read_opt, &(server_config.options),      ""},
	{"opt",          read_opt, &(server_config.options),      ""},
#endif
	{"notify_file",  read_str, &(server_config.notify_file),  ""},
	{"sname",        read_str, &(server_config.sname),        ""},
	{"boot_file",    read_str, &(server_config.boot_file),    ""},
#ifdef DHCP_SERVER_MULTIIF
	{"start",        read_start_ip,  NULL,     ""},
	{"end",          read_end_ip,  NULL,       ""},
	{"max_leases",   read_max_leases, NULL,   ""},
	{"static_lease", read_staticlease, NULL, ""},
	{"vid", read_vid, NULL, ""},
	{"del_vlan", del_vlan, NULL, ""},
#else
	{"static_lease", read_staticlease, &(server_config.static_leases), ""},
#endif
	/* ADDME: static lease */
};
#ifdef DHCP_SERVER_MULTIIF
enum { KWS_WITH_DEFAULTS = ARRAY_SIZE(keywords) - 11 };
#else
enum { KWS_WITH_DEFAULTS = ARRAY_SIZE(keywords) - 6 };
#endif

#ifdef DHCP_SERVER_OPTION_43
void read_option43(const char *file)
{
	FILE *fp = fopen(file, "r");
	char buf[151], *p;
	int i = 0;

	server_config.option43Data = NULL;
	memset(buf, 0, sizeof(buf));

	fp = fopen(file, "r"); 
	while (fp == NULL)
	{
		i++;
		if (i>10) 
		{
			dbck("%s %d: cannot read option 43 file: %s, wait more than 10 seconds, give up.", 
					__func__, __LINE__, file);
			return;
		}

		sleep(1);
		fp = fopen(file, "r");
	}

	fgets(buf, 150, fp);
	fclose(fp);
    
    server_config.option43Data = (char*)malloc(strlen(buf)/2);
#if defined(CONFIG_BOARD_P1010_C)
    if(server_config.option43Data == NULL) {
        return 0;
    }
    p = buf;
    i = 0;
    while(*p)
    {
        server_config.option43Data[i] = strtol(p, &p, 16);
        i++;
    }
#else
	if (server_config.option43Data != NULL)
	{
		memset(server_config.option43Data, 0, strlen(buf)/2);
		for (i=0; i<strlen(buf); i+=2)
		{
			sscanf(&buf[i], "%02x%*s", &server_config.option43Data[i/2]);
//			dbck("buf[%d]=%c, buf[%d]=%c, get tmp:[%d]\n", i, buf[i], i+1, buf[i+1], server_config.option43Data[i/2]);
		}
	}
#endif
   }

#endif

void read_config(const char *file
#ifdef DHCP_SERVER_MULTIIF
		, int reset
#endif
		)
{
	dbck("%d in %s of %s, file = %s\n", __LINE__, __func__, __FILE__, file);
	parser_t *parser;
	const struct config_keyword *k;
	unsigned i;
	char *token[2];

#ifdef DHCP_SERVER_MULTIIF
	if(reset==1)
	{
#endif
		for (i = 0; i < KWS_WITH_DEFAULTS; i++)
			keywords[i].handler(keywords[i].def, keywords[i].var);
#ifdef DHCP_SERVER_MULTIIF
	}
#endif

	parser = config_open(file);
	while (config_read(parser, token, 2, 2, "# \t", PARSE_NORMAL)) {
		for (k = keywords, i = 0; i < ARRAY_SIZE(keywords); k++, i++) {
			if (!strcasecmp(token[0], k->keyword)) {
				if (!k->handler(token[1], k->var)) {
					bb_error_msg("can't parse line %u in %s",
							parser->lineno, file);
					/* reset back to the default value */
					k->handler(k->def, k->var);
				}
				break;
			}
		}
	}
	config_close(parser);

#ifndef DHCP_SERVER_MULTIIF
	server_config.start_ip = ntohl(server_config.start_ip);
	server_config.end_ip = ntohl(server_config.end_ip);
#endif
}

#if (CLOUD_CONFIG == FYES)
static void wirte_hostname_noticemac(struct dhcpOfferedAddr *lease)
{
	MacAddr tmp;
	memset(&tmp,0,sizeof(MacAddr));	
	int i;
	for(i = 0;i < MAX_NOTICE_MAC_LIST;i++)
	{
		if(memcmp(nvramProfile->noticeMacList[i].macaddr.addr,lease->chaddr,ETH_ALEN)==0)
		{
			dbck("%d in %s of %s\n", __LINE__, __func__, __FILE__);
			memcpy(nvramProfile->noticeMacList[i].name,lease->hostname,16);
			return;
		}
		else if(memcmp(nvramProfile->noticeMacList[i].macaddr.addr,&tmp,ETH_ALEN)==0)
		{
			memcpy(nvramProfile->noticeMacList[i].macaddr.addr,lease->chaddr,ETH_ALEN);
			memcpy(nvramProfile->noticeMacList[i].name,lease->hostname,16);
			dbck("busybox is not have line = %d\n", __LINE__);
			return;
		}
		else
		{
			dbck("%s error\n",__func__);
		}	
	}	

    unsigned int ret = ((nvramProfile->noticemac_list_count)++)%MAX_NOTICE_MAC_LIST;
	memcpy(nvramProfile->noticeMacList[i].macaddr.addr,lease->chaddr,ETH_ALEN);
	memcpy(nvramProfile->noticeMacList[i].name,lease->hostname,16);
	dbck("the list over");
	return ;
}
#endif

void write_leases(void)
{
	int fp;
	unsigned i;
#if 0
    /*edit by brwang*/
	time_t curr = time(0);
#else
	time_t curr = utt_getSysUpTime();
#endif
	unsigned long tmp_time;
    static char nullAddr[16] = {0};

#ifdef DHCP_SERVER_MULTIIF
	dhcp_pool_t *tmp = NULL;
	uint32_t j = 0;
	tmp = server_config.dhcp_pool;

	fp = open_or_warn(server_config.lease_file, O_WRONLY|O_CREAT|O_TRUNC);
	if (fp < 0) {
		return;
	}

	for (j = 0; j < DHCP_SERVER_HASH; j++) {
		tmp = server_config.dhcp_pool[j].next;

		while (tmp != NULL) {
			for (i = 0; i < tmp->max_leases; i++) {
				if (tmp->leases[i].yiaddr != 0 && (memcmp(tmp->leases[i].chaddr, nullAddr, 16) != 0)) {
					/* screw with the time in the struct, for easier writing */
					tmp_time = tmp->leases[i].expires;

					if (server_config.remaining) {
						if (lease_expired(&(tmp->leases[i]))){
		                    continue;
		//					leases[i].expires = 0;
		                }
						else tmp->leases[i].expires -= curr;
					} /* else stick with the time we got */
					tmp->leases[i].expires = htonl(tmp->leases[i].expires);
					tmp->leases[i].vid = htonl(tmp->vid);

					// FIXME: error check??
					full_write(fp, &tmp->leases[i], sizeof(tmp->leases[i]));

					/* then restore it when done */
					tmp->leases[i].expires = tmp_time;
				}
			}

			tmp = tmp->next;
		}

	}

	close(fp);
#else
	fp = open_or_warn(server_config.lease_file, O_WRONLY|O_CREAT|O_TRUNC);
	if (fp < 0) {
		return;
	}

	for (i = 0; i < server_config.max_leases; i++) {
		if (leases[i].yiaddr != 0 && (memcmp(leases[i].chaddr, nullAddr, 16) != 0)) {

			/* screw with the time in the struct, for easier writing */
			tmp_time = leases[i].expires;

			if (server_config.remaining) {
				if (lease_expired(&(leases[i]))){
                    continue;
//					leases[i].expires = 0;
                }
				else leases[i].expires -= curr;
			} /* else stick with the time we got */
			leases[i].expires = htonl(leases[i].expires);
			// FIXME: error check??
			//
#if ( CLOUD_CONFIG == FYES)					
	/*增加写入noticemac by xrg*/
			dbck("%d  before wirte__hostname_noticemac xrg in %s of %s\n", __LINE__, __func__, __FILE__);
uttSemP(SEM_CLOUD_CONFIG,0);/*speedweb也会进行遍历读写，加锁防止冲突*/
				wirte_hostname_noticemac(&leases[i]);
uttSemV(SEM_CLOUD_CONFIG,0);/*speedweb也会进行遍历读写，加锁防止冲突*/
			dbck("%d  after wirte__hostname_noticemac xrg in %s of %s\n", __LINE__, __func__, __FILE__);
#endif		
			full_write(fp, &leases[i], sizeof(leases[i]));

			/* then restore it when done */
			leases[i].expires = tmp_time;
		}
	}
	close(fp);
#endif

	if (server_config.notify_file) {
// TODO: vfork-based child creation
		char *cmd = xasprintf("%s %s", server_config.notify_file, server_config.lease_file);
		system(cmd);
		free(cmd);
	}
}


void read_leases(const char *file)
{
#ifdef DHCP_SERVER_MULTIIF
	int fp;
	unsigned int i, vid = 0;
	struct dhcpOfferedAddr lease;
	dhcp_pool_t *tmp = NULL;

	fp = open_or_warn(file, O_RDONLY);
	if (fp < 0) {
		return;
	}

	i = 0;
	while (full_read(fp, &lease, sizeof(lease)) == sizeof(lease)) {
		/* ADDME: is it a static lease */
		uint32_t y = ntohl(lease.yiaddr);
		vid = ntohl(lease.vid);
		dbck("%d in %s of %s, get_current_pool\n", __LINE__, __func__, __FILE__);
		tmp = get_current_pool(vid, 0);
		if (tmp != NULL) {
			if (y >= tmp->start_ip && y <= tmp->end_ip) {
				lease.expires = ntohl(lease.expires);
				if (!server_config.remaining)
#if	0
	            /*edit by brwang*/
					lease.expires -= time(NULL);
#else
					lease.expires -= utt_getSysUpTime();
#endif
				dbck("%d in %s of %s\n", __LINE__, __func__, __FILE__);
				if (!(add_lease(lease.hostname, lease.chaddr, lease.yiaddr, lease.expires, tmp))) {
					bb_error_msg("too many leases while loading %s", file);
					break;
				}
				i++;
			}
		}
	}
	DEBUG("Read %d leases", i);
	close(fp);
#else
	int fp;
	unsigned i;
	struct dhcpOfferedAddr lease;

	fp = open_or_warn(file, O_RDONLY);
	if (fp < 0) {
		return;
	}

	i = 0;
	while (i < server_config.max_leases
	 && full_read(fp, &lease, sizeof(lease)) == sizeof(lease)
	) {
		/* ADDME: is it a static lease */
		uint32_t y = ntohl(lease.yiaddr);
		if (y >= server_config.start_ip && y <= server_config.end_ip) {
			lease.expires = ntohl(lease.expires);
			if (!server_config.remaining)
#if 0
                /*edit by brwang*/
				lease.expires -= time(NULL);
#else
				lease.expires -= utt_getSysUpTime();
#endif
			if (!(add_lease(lease.hostname, lease.chaddr, lease.yiaddr, lease.expires))) {
				bb_error_msg("too many leases while loading %s", file);
				break;
			}
			i++;
		}
	}
	DEBUG("Read %d leases", i);
	close(fp);
#endif
}
#ifdef DHCP_SERVER_MULTIIF
/*
 *功能： 此处初始化hash的第一个节点，第一个节点只作为链表的首节点，不用来存储
 *作者： KeHuatao
 *日期： 20131205
 */
int init_pool(void)
{
	int ret = 0, i = 0;

	for (i = 0; i < DHCP_SERVER_HASH; i++) {
		server_config.dhcp_pool[i].vid = 4096;
		server_config.dhcp_pool[i].next = NULL;
	}

	dhcp_pool_ptr = &server_config.dhcp_pool[0];

	return ret;
}
/*
 *功能： 根据vlan，找到对应的地址池并设置lease指针
 *
 *作者： KeHuatao
 *日期：20131205
 */
dhcp_pool_t* get_current_pool(uint32_t vid, uint32_t set)
{
	dbck("%d in %s of %s, get vid = %u\n", __LINE__, __func__, __FILE__, vid);
	dhcp_pool_t *tmp=NULL;
	uint32_t key = 0;

	if ((vid < 0u) || (vid >= 4095u)) {
		return tmp;
	}

	key = dhcp_server_hash(vid);

    /*lookup the pool by vid */
	tmp = server_config.dhcp_pool[key].next;

	/*遍历pool链表*/
	while (tmp != NULL) {
		/*各节点按vid由小到大顺序链接*/
		if (tmp->vid < vid) {
			tmp = tmp->next;
		} else if(tmp->vid == vid) {
			if (set == 1) {
				dhcp_pool_ptr = tmp;
			}
			break;
		} else {
			tmp = NULL;
			break;
		}
	}

	return tmp;
}
#endif

