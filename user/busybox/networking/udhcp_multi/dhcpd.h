/* vi: set sw=4 ts=4: */
/* dhcpd.h */

#ifndef _DHCPD_H
#define _DHCPD_H

#if __GNUC_PREREQ(4,1)
# pragma GCC visibility push(hidden)
#endif


/************************************/
/* Defaults _you_ may want to tweak */
/************************************/

/* the period of time the client is allowed to use that address */
#define LEASE_TIME              (60*60*24*10) /* 10 days of seconds */
#define LEASES_FILE		CONFIG_DHCPD_LEASES_FILE

/* where to find the DHCP server configuration file */
#define DHCPD_CONF_FILE         "/etc/udhcpd.conf"
#define DHCPD_VLAN_CONF_FILE         "/etc/udhcpd_vlan.conf"

struct option_set {
	uint8_t *data;
	struct option_set *next;
};

struct static_lease {
	struct static_lease *next;
	uint8_t *mac;
	uint32_t *ip;
};
#ifdef DHCP_SERVER_MULTIIF
typedef struct _dhcp_pool_t {
	uint32_t  start_ip;              /* Start address of leases, in host order */
	uint32_t  end_ip;
	struct option_set *options;
	struct static_lease *static_leases; /* List of ip/mac pairs to assign static leases */
	struct dhcpOfferedAddr *leases;
	uint32_t lease;	                /* lease time in seconds (host order) */
	uint32_t vid;
	uint32_t max_leases;            /* maximum number of leases (including reserved address) */
	struct _dhcp_poot_t *next;
} dhcp_pool_t;
#endif
struct server_config_t {
	uint32_t server;                /* Our IP, in network order */
#if ENABLE_FEATURE_UDHCP_PORT
	uint16_t port;
#endif
	/* start,end are in host order: we need to compare start <= ip <= end */
#ifdef DHCP_SERVER_MULTIIF
    dhcp_pool_t dhcp_pool[DHCP_SERVER_HASH];     /*指向地址池链表头*/
    uint32_t vlan_num;
    uint32_t vid;
#else
	uint32_t start_ip;              /* Start address of leases, in host order */
	uint32_t end_ip;                /* End of leases, in host order */
	struct option_set *options;     /* List of DHCP options loaded from the config file */
	uint32_t lease;	                /* lease time in seconds (host order) */
	uint32_t max_leases;            /* maximum number of leases (including reserved address) */
#endif
	char *interface;                /* The name of the interface to use */
	int ifindex;                    /* Index number of the interface to use */
	uint8_t arp[6];                 /* Our arp address */
	char remaining;                 /* should the lease file be interpreted as lease time remaining, or
	                                 * as the time the lease expires */
	uint32_t auto_time;             /* how long should udhcpd wait before writing a config file.
	                                 * if this is zero, it will only write one on SIGUSR1 */
	uint32_t decline_time;          /* how long an address is reserved if a client returns a
	                                 * decline message */
	uint32_t conflict_time;         /* how long an arp conflict offender is leased for */
	uint32_t offer_time;            /* how long an offered address is reserved */
	uint32_t min_lease;             /* minimum lease a client can request */
	char *pidfile;
	char *lease_file;
	char *notify_file;              /* What to run whenever leases are written */
	uint32_t siaddr;                /* next server bootp option */
	char *sname;                    /* bootp server name */
	char *boot_file;                /* bootp boot file option */
#ifndef DHCP_SERVER_MULTIIF
	struct static_lease *static_leases; /* List of ip/mac pairs to assign static leases */
#endif
	char *option43Data;
};

#define server_config (*(struct server_config_t*)&bb_common_bufsiz1)
/* client_config sits in 2nd half of bb_common_bufsiz1 */

#if ENABLE_FEATURE_UDHCP_PORT
#define SERVER_PORT (server_config.port)
#else
#define SERVER_PORT 67
#endif

extern struct dhcpOfferedAddr *leases;


/*** leases.h ***/

struct dhcpOfferedAddr {
	uint8_t hostname[16];
	uint8_t chaddr[16];
	uint32_t yiaddr;	/* network order */
	uint32_t expires;	/* host order */
#ifdef DHCP_SERVER_MULTIIF
	uint32_t vid;
#endif
};

#ifdef DHCP_SERVER_MULTIIF
struct dhcpOfferedAddr *add_lease(uint8_t *hostname, const uint8_t *chaddr, uint32_t yiaddr, unsigned long lease, dhcp_pool_t *tmp);
#else
struct dhcpOfferedAddr *add_lease(uint8_t *hostname, const uint8_t *chaddr, uint32_t yiaddr, unsigned long lease);
#endif
int lease_expired(struct dhcpOfferedAddr *lease);
struct dhcpOfferedAddr *find_lease_by_chaddr(const uint8_t *chaddr);
struct dhcpOfferedAddr *find_lease_by_yiaddr(uint32_t yiaddr);
uint32_t find_address(int check_expired);


/*** static_leases.h ***/

/* Config file will pass static lease info to this function which will add it
 * to a data structure that can be searched later */
int addStaticLease(struct static_lease **lease_struct, uint8_t *mac, uint32_t *ip);
/* Check to see if a mac has an associated static lease */
uint32_t getIpByMac(struct static_lease *lease_struct, void *arg);
/* Check to see if an ip is reserved as a static ip */
uint32_t reservedIp(struct static_lease *lease_struct, uint32_t ip);
/* Print out static leases just to check what's going on (debug code) */
void printStaticLeases(struct static_lease **lease_struct);


/*** serverpacket.h ***/

#ifdef DHCP_SERVER_MULTIIF
int send_offer(struct dhcpMessage *oldpacket, uint32_t vid);
int send_NAK(struct dhcpMessage *oldpacket, uint32_t vid);
int send_ACK(struct dhcpMessage *oldpacket, uint32_t yiaddr, uint32_t vid);
int send_inform(struct dhcpMessage *oldpacket, uint32_t vid);
#else
int send_offer(struct dhcpMessage *oldpacket);
int send_NAK(struct dhcpMessage *oldpacket);
int send_ACK(struct dhcpMessage *oldpacket, uint32_t yiaddr);
int send_inform(struct dhcpMessage *oldpacket);
#endif


/*** files.h ***/

void read_option43(const char *file);
#ifdef DHCP_SERVER_MULTIIF
void read_config(const char *file, int reset);
#else
void read_config(const char *file);
#endif
void write_leases(void);
void read_leases(const char *file);
struct option_set *find_option(struct option_set *opt_list, uint8_t code);

#ifdef DHCP_SERVER_MULTIIF
void free_lease(dhcp_pool_t *node, int free_node);
#endif

#ifdef DHCP_SERVER_MULTIIF
dhcp_pool_t* get_current_pool(uint32_t vid, uint32_t set);
#endif

#if __GNUC_PREREQ(4,1)
# pragma GCC visibility pop
#endif

#endif
