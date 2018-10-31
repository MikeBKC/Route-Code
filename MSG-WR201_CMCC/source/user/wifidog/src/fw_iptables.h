/********************************************************************\
 * This program is free software; you can redistribute it and/or    *
 * modify it under the terms of the GNU General Public License as   *
 * published by the Free Software Foundation; either version 2 of   *
 * the License, or (at your option) any later version.              *
 *                                                                  *
 * This program is distributed in the hope that it will be useful,  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
 * GNU General Public License for more details.                     *
 *                                                                  *
 * You should have received a copy of the GNU General Public License*
 * along with this program; if not, contact:                        *
 *                                                                  *
 * Free Software Foundation           Voice:  +1-617-542-5942       *
 * 59 Temple Place - Suite 330        Fax:    +1-617-542-2652       *
 * Boston, MA  02111-1307,  USA       gnu@gnu.org                   *
 *                                                                  *
\********************************************************************/

/* $Id: fw_iptables.h,v 1.11.4.1 2015/06/23 15:57:34 hou.bo Exp $ */
/** @file fw_iptables.h
    @brief Firewall iptables functions
    @author Copyright (C) 2004 Philippe April <papril777@yahoo.com>
*/

#ifndef _FW_IPTABLES_H_
#define _FW_IPTABLES_H_

#include "firewall.h"
#include "../config.h"
#include "uttMachine.h"
#include "mib.h"
#include "profacce.h"

/*@{*/ 
/**Iptable table names used by WifiDog */
#define TABLE_WIFIDOG_OUTGOING  "WiFiDog_$ID$_Outgoing"
#define TABLE_WIFIDOG_WIFI_TO_INTERNET "WiFiDog_$ID$_WIFI2Internet"
#define TABLE_WIFIDOG_WIFI_TO_ROUTER "WiFiDog_$ID$_WIFI2Router"
#define TABLE_WIFIDOG_INCOMING  "WiFiDog_$ID$_Incoming"
#define TABLE_WIFIDOG_PREROUTING  "WiFiDog_$ID$_Prerouting"
#define TABLE_WIFIDOG_AUTHSERVERS "WiFiDog_$ID$_AuthServers"
#define TABLE_WIFIDOG_GLOBAL  "WiFiDog_$ID$_Global"
#define TABLE_WIFIDOG_VALIDATE  "WiFiDog_$ID$_Validate"
#define TABLE_WIFIDOG_KNOWN     "WiFiDog_$ID$_Known"
#define TABLE_WIFIDOG_UNKNOWN   "WiFiDog_$ID$_Unknown"
#define TABLE_WIFIDOG_LOCKED    "WiFiDog_$ID$_Locked"
#define TABLE_WIFIDOG_TRUSTED    "WiFiDog_$ID$_Trusted"

#define IPSET_WIFIDOG_FREE_AUTH	"wifi_free_auth_group"
#ifdef WIFI_IPSET_LAN
#define WIFI_GROUP_PREFIX "wifi_client_group"
#endif
#if (SWITCH_AUTH_MODE == FYES)
#define IPSET_GROUP_SWITCH_CLIENT_LIST "wifi_group_switch_client_allow"
#endif
/*@}*/ 
typedef enum fw_op_type_ {
    IPTABLE_ADD,
    IPTABLE_DEL
} fw_op_type_t;
/** Used by iptables_fw_access to select if the client should be granted of denied access */
typedef enum fw_access_t_ {
    FW_ACCESS_ALLOW,
    FW_ACCESS_DENY
} fw_access_t;

#if (WIFI_RATE_LIMIT == FYES)
#define MAX_WIFIDOG_RATE_LIMIT_GROUP 5
#define MAX_IPSET_NAME_LENGTH 32
/*带宽控制*/
typedef struct st_rate_limit {
    char name[MAX_IPSET_NAME_LENGTH+1];
    int upRate;
    int downRate;
    int mark;
    int sort;
    int count;
}RateLimitInfo;
extern RateLimitInfo rateLimitGroup[];
#endif
/** @brief Initialize the firewall */
int iptables_fw_init(void);

/** @brief Initializes the authservers table */
void iptables_fw_set_authservers(void);

/** @brief Clears the authservers table */
#ifdef WIFI_PRE_REDIRECT
void iptables_fw_clear_authservers(char *ip);
#else
void iptables_fw_clear_authservers(void);
#endif

/** @brief Destroy the firewall */
int iptables_fw_destroy(void);

/** @brief Helper function for iptables_fw_destroy */
int iptables_fw_destroy_mention( const char * table, const char * chain, const char * mention);

/** @brief Define the access of a specific client */
int iptables_fw_access(fw_access_t type, const char *ip, const char *mac, int tag);

/** @brief All counters in the client list */
int iptables_fw_counters_update(void);

static void getDomainNameIP(char *domain,char *ipset_grp);
int iptables_fw_add_del(fw_op_type_t opt);
void iptables_del_ifOnline(void);
#if (WIFIDOG_MAKE_MONEY != FYES)
void iptables_add_weixin_ipset(void);
#endif
#ifdef WIFI_IPSET_LAN
void wifidogAddgrp(int index, unsigned int ip, unsigned int cidr);
void wifidogDelgrp(int index);
void wifidogAorDIpMac(const char *ipStr, const char *mac, char addorDel);
void initLanIpset(void);
#if (VLAN_CONFIG == FYES)
void wifidogDelVlanGrp();
#endif
#endif
int iptables_fw_add_del_exceptIp(fw_op_type_t opt, char *exceptIpGrp);
#if (WIFIDOG_NOTICE == FYES)
extern void wifi_Notice_Handle(fw_op_type_t operate);
int getNoticeTime(unsigned int *time);
#endif
#if (WIFI_RATE_LIMIT == FYES)
int createRateLimit(char *ip, char *mac, int upRate, int downRate, int version, int authType);
void destoryRateLimit();
void initRateLimit();
void rateRuleIPAddOrDel(int limitGroupIndex,char *ip ,char opt);
#endif
#endif /* _IPTABLES_H_ */
