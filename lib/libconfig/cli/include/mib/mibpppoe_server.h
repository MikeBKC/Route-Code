/*
 * $Id: mibpppoe_server.h,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
 *
 *      Copyright (c) 1997 UTT Technologies, Inc.
 *      All rights reserved.
 *      Use of copyright notice does not imply publication.
 *
 *
 *                      CONFIDENTIAL INFORMATION
 *                      ------------------------
 *      This Document contains Confidential Information or Trade Secrets,
 *      or both, which are the property of UTT Technologies, Inc.
 *      This document may not be copied, reproduced, reduced to any
 *      electronic medium or machine readable form or otherwise duplicated
 *      and the information herein may not be used, disseminated or
 *      otherwise disclosed, except with the prior written consent of
 *      UTT Technologies, Inc.
 */

#if ! defined( MIBPPPOESERVER_H )
# define MIBPPPOESERVER_H

# include "mibglob.h"

//#define MAX_ETHER_PORTS		MAX_ETHER_PORTS	/* 4240nb */
#define MAX_PPPOE_SRV_PROFILES		(SESSION_MAX - 2)

  
/* 
 * mac filter profile.
 */
typedef enum filter_type {
	ALLOW_MAC,
	DENY_MAC
}filter_type;

	
struct mac_filter_profile {
	char			hostName[MAX_PROFILE_NAME_LENGTH+1];
	Boolean			active;
	Uint8			bind_port;		/* eth1, eth2, eth3... */
	BYTE			mac[ETHER_ADDR_LEN];
	filter_type		type;
};
typedef struct mac_filter_profile MacFilterProfile;


/*
 * pppoe server profile.
 */

struct security_ctl {
	Boolean			mac_leach;			/* TRUE will filter the mac address */
	Boolean			detect_ac_spoof;		/* TRUE	will defense the AC spoof attack */
	Boolean			bind_mac_acc;			/* TRUE will bind the account to the mac */
	filter_type		mac_leach_type;			/* which type do you want to leach */
	//char 			interface[MAX_ETHER_PORTS + 1];	/* which interface this sec_ctl will apply to */
};


/*
 * @port_sess_num:	max session number per port.
 * @port_status:	decide whether the PPPoE Server will be allowed on this port.
 */
struct _port_status {
	Boolean			active;
	Uint16			port_sess_num;
	IpAddress 		pri_dns;
	IpAddress		sec_dns;
	IpAddress 		pri_wins;
	IpAddress		sec_wins;
	IpAddress		local_address;
};
/*
 * why MAX_HOST_NAME_LENGTH(31)+2?
 * we create a uniq default hangup name.
 */
#define DEFAULT_HANGUP_USERNAME		"12345678901234567890123456789012"
struct _hangup {
	char 		username[MAX_HOST_NAME_LENGTH+2];	/* username to hangup */
	Uint16 		sid;					/* session id to hangup */
	Byte  		hangup_mac[ETHER_ADDR_LEN];		/* mac to hangup */
};

#if (PPPOE_ACCOUNT == FYES)
struct pppoe_account {
    Uint32		write_interval;
    Uint32		hour_divisor;
};
#endif



/*
 * struct pppoe_server_profile:
 * 1.member list:
 * @active:		Global PPPoE Server Trigger;
 * @vlan_active:	PPPoE over Vlan Trigger;
 * @sec_ctl:		Security Controller;      
 * @max_sess_num:	max session number, must be no more than MAX_PPPOE_SRV_SES;
 * @max_mac_sess:	session number a *single* MAC can own.
 * @account_used_num:	numbers an account can be used concurrently.
 * @port:		port specified item.
 * @hangup:		hangup the specified session.
 *
 */
struct pppoe_server_profile {
	char			hostName[MAX_PROFILE_NAME_LENGTH+1];
	Boolean 		active;		
	Boolean			vlan_active;	
	Uint32 			max_sess_num;
	Uint16			max_mac_sess;
	struct _port_status	port[MAX_ETHER_PORTS];
	struct _hangup		sess_hangup;
	struct 	security_ctl	sec_ctl[MAX_ETHER_PORTS];
#if (PPPOE_ACCOUNT == FYES)
	struct pppoe_account	account;
#endif
};

typedef struct pppoe_server_profile PppoeServerProfile;
	
struct pppoe_only_profile {
	Boolean			enabled;
	Boolean			permit;
	char			indexName[MAX_PROFILE_NAME_LENGTH + 1];
	char			exceptIpGroup[MAX_PROFILE_NAME_LENGTH + 1];
};

typedef struct pppoe_only_profile PppoeOnlyProfile;





#endif /* MIBPPPOESERVER_H */

