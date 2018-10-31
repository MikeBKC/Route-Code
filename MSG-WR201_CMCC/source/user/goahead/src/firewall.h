/*
 *	inic.h -- Wireless Configuration Header 
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: firewall.h,v 1.4 2012/01/12 03:24:52 yu.dehai Exp $
 */


#define PROTO_UNKNOWN	0
#define PROTO_TCP		1
#define PROTO_UDP		2
#define PROTO_TCP_UDP	3
#define PROTO_ICMP		4
#define PROTO_NONE		5


#define RULE_MODE_DISABLE	0
#define RULE_MODE_DROP		1
#define RULE_MODE_ACCEPT	2

#define ACTION_DROP		0
#define ACTION_ACCEPT	1

#define WEB_FILTER_CHAIN        "web_filter"
#define WEB_FILTER_CHAIN1       "web_filter1"
#define DNS_INPUT_CHAIN        "dns_input_chain"
#define DNS_FORWARD_CHAIN       "dns_forward_chain"
#define IPPORT_FILTER_CHAIN	"macipport_filter"

#define DMZ_CHAIN			"DMZ"
#define DMZ_CHAIN1			"DMZ1"
#define PORT_FORWARD_CHAIN	"port_forward"
#define PORT_FORWARD_CHAIN1	"port_forward1"
#define PORT_FORWARD_RULE_CHAIN	"port_forward_rule"
#define GLOBAL_CHAIN		"global"
#define GROUP_CHAIN		"group"
#define GROUP_LIMIT_CHAIN	"group_limit"


#define MAGIC_NUM			"IPCHAINS"			

#define HTML_NO_FIREWALL_UNDER_BRIDGE_MODE	\
"<img src=\"/graphics/warning.gif\"><font color=#ff0000>&nbsp; &nbsp;Warning: The current operation mode is \"Bridge mode\" and these settings may not be functional.</font>"

void iptablesFilterClear(void);
#if 0
void iptablesFilterRun(char *rule);
#endif
void iptablesWebsFilterRun(void);
void formDefineFirewall(void);
#if 0
/* unused */
void firewall_init(void);
#endif
int isIpNetmaskValid(char *s);	// export for QoS
int isMacValid(char *);			// export for QoS
