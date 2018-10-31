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

/* $Id: firewall.h,v 1.4.8.1 2016/08/02 02:34:09 cao.yongxiang Exp $ */
/** @file firewall.h
    @brief Firewall update functions
    @author Copyright (C) 2004 Philippe April <papril777@yahoo.com>
*/

#ifndef _FIREWALL_H_
#define _FIREWALL_H_
#include "uttMachine.h"
#include "mib.h"
#include "profacce.h"

int icmp_fd;

/** Used by fw_iptables.c */
typedef enum _t_fw_marks {
    FW_MARK_PROBATION = 1, /**< @brief The client is in probation period and must be authenticated 
			    @todo: VERIFY THAT THIS IS ACCURATE*/
    FW_MARK_KNOWN = 2,  /**< @brief The client is known to the firewall */ 
    FW_MARK_LOCKED = 254 /**< @brief The client has been locked out */
} t_fw_marks;

/** @brief Initialize the firewall */
int fw_init(void);

/** @brief Clears the authservers list */
#ifdef WIFI_PRE_REDIRECT
void fw_clear_authservers(char *ip);
#else
void fw_clear_authservers(void);
#endif

/** @brief Sets the authservers list */
void fw_set_authservers(void);

/** @brief Destroy the firewall */
int fw_destroy(void);

/** @brief Allow a user through the firewall*/
int fw_allow(char *ip, char *mac, int profile);

/** @brief Deny a client access through the firewall*/
int fw_deny(char *ip, char *mac, int profile);

/** @brief Refreshes the entire client list */
void fw_sync_with_authserver(void);

/** @brief Get an IP's MAC address from the ARP cache.*/
char *arp_get(char *req_ip);
char *arp_get_by_mac(char *req_mac);

/** @brief ICMP Ping an IP */
void icmp_ping(char *host);

/** @brief cheap random */
unsigned short rand16(void);

#ifdef WIFI_PRE_REDIRECT
static void checkFreeAuth(void);
#endif
#endif /* _FIREWALL_H_ */
