/* vi: set sw=4 ts=4: */
/* dhcpd.c
 *
 * udhcp Server
 * Copyright (C) 1999 Matthew Ramsay <matthewr@moreton.com.au>
 *			Chris Trew <ctrew@moreton.com.au>
 *
 * Rewrite by Russ Dill <Russ.Dill@asu.edu> July 2001
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"

#include <syslog.h>
#include "common.h"
#include "dhcpc.h"
#include "dhcpd.h"
#include "options.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "ifmisc.h"
#include "comExeFun.h"


#if (DHCP_AUTO_BIND == FYES)
extern void autodel(struct dhcpOfferedAddr *lease);
#endif
#ifdef DHCP_SERVER_MULTIIF
extern int init_pool(void);
extern struct _dhcp_pool_t * dhcp_pool_ptr;
uint8_t mac[MAC_LEN] = { 0 };
#endif

/* globals */
struct dhcpOfferedAddr *leases;
/* struct server_config_t server_config is in bb_common_bufsiz1 */


int udhcpd_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int udhcpd_main(int argc UNUSED_PARAM, char **argv)
{
	dbck("%d in %s of %s\n", __LINE__, __func__, __FILE__);
	fd_set rfds;
	struct timeval tv;
	int server_socket = -1, bytes, retval, max_sock;
	struct dhcpMessage packet;
	uint8_t *state, *server_id, *requested;
	uint32_t server_id_align, requested_align, static_lease_ip;
	unsigned timeout_end;
	unsigned num_ips;
	unsigned opt;
	struct option_set *option;
	struct dhcpOfferedAddr *lease, static_lease;
	USE_FEATURE_UDHCP_PORT(char *str_P;)
#ifdef DHCP_SERVER_MULTIIF
	uint32_t vid = 0;
#endif

#if ENABLE_FEATURE_UDHCP_PORT
	SERVER_PORT = 67;
	CLIENT_PORT = 68;
#endif
#if (DHCP_AUTO_BIND == FYES) || defined(CONFIG_UTT_DHCP_COLLISION) || (CLOUD_CONFIG == FYES)
    ProfInit();
#endif

	opt = getopt32(argv, "fS" USE_FEATURE_UDHCP_PORT("P:", &str_P));
	argv += optind;

	if (!(opt & 1)) { /* no -f */
		bb_daemonize_or_rexec(0, argv);
		logmode &= ~LOGMODE_STDIO;
	}

	if (opt & 2) { /* -S */
		openlog(applet_name, LOG_PID, LOG_LOCAL0);
		logmode |= LOGMODE_SYSLOG;
	}
#if ENABLE_FEATURE_UDHCP_PORT
	if (opt & 4) { /* -P */
		SERVER_PORT = xatou16(str_P);
		CLIENT_PORT = SERVER_PORT + 1;
	}
#endif
#ifdef DHCP_SERVER_MULTIIF
	if(init_pool()!=0)
	{
		dbck("%d in %s of %s\n", __LINE__, __func__, __FILE__);
		return -1;
	}
#endif
	/* Would rather not do read_config before daemonization -
	 * otherwise NOMMU machines will parse config twice */
#ifdef DHCP_SERVER_MULTIIF
	read_config(argv[0] ? argv[0] : DHCPD_CONF_FILE, 1);
#else
	read_config(argv[0] ? argv[0] : DHCPD_CONF_FILE);
#endif

#ifdef DHCP_SERVER_OPTION_43
	read_option43("/tmp/udhcpdoption43");
#endif
	/* Make sure fd 0,1,2 are open */
	bb_sanitize_stdio();
	/* Equivalent of doing a fflush after every \n */
	setlinebuf(stdout);

	/* Create pidfile */
	write_pidfile(server_config.pidfile);
	/* if (!..) bb_perror_msg("cannot create pidfile %s", pidfile); */

	bb_info_msg("%s (v"BB_VER") started", applet_name);

#ifdef DHCP_SERVER_MULTIIF
	dhcp_pool_t *tmp = NULL;
	uint32_t i = 0;

	for (i = 0; i < DHCP_SERVER_HASH; i++) {
		tmp = server_config.dhcp_pool[i].next;

		while (tmp != NULL) {
			dhcp_pool_ptr = tmp;
			dbck("%d in %s of %s, vid = %u\n", __LINE__, __func__, __FILE__, tmp->vid);
			option = find_option(tmp->options, DHCP_LEASE_TIME);
			tmp->lease = LEASE_TIME;
			if (option) {
				memcpy(&tmp->lease, option->data + 2, 4);
				tmp->lease = ntohl(tmp->lease);
			}

			/* Sanity check */
			dbck("%d in %s of %s, tmp->end_ip = %u, tmp->start_ip = %u\n", __LINE__, __func__, __FILE__, tmp->end_ip, tmp->start_ip);
			num_ips = tmp->end_ip - tmp->start_ip + 1;
			if (tmp->max_leases > num_ips) {
				dbck("%d in %s of %s, max_leases= %u, num_ips = %u\n", __LINE__, __func__, __FILE__, tmp->max_leases, num_ips);
				bb_error_msg("max_leases=%u is too big, setting to %u",
					(unsigned)tmp->max_leases, num_ips);
				tmp->max_leases = num_ips;
			}

			tmp->leases = xzalloc(tmp->max_leases * sizeof(*(tmp->leases)));

			tmp = tmp->next;
		}
	}

	read_leases(server_config.lease_file);
#else
	option = find_option(server_config.options, DHCP_LEASE_TIME);
	server_config.lease = LEASE_TIME;
	if (option) {
		memcpy(&server_config.lease, option->data + 2, 4);
		server_config.lease = ntohl(server_config.lease);
	}

	/* Sanity check */
	num_ips = server_config.end_ip - server_config.start_ip + 1;
	if (server_config.max_leases > num_ips) {
		bb_error_msg("max_leases=%u is too big, setting to %u",
			(unsigned)server_config.max_leases, num_ips);
		server_config.max_leases = num_ips;
	}

	leases = xzalloc(server_config.max_leases * sizeof(*leases));
	read_leases(server_config.lease_file);
#endif

	if (read_interface(server_config.interface, &server_config.ifindex,
			   &server_config.server, server_config.arp)) {
		retval = 1;
		goto ret;
	}
	dbck("%d in %s of %s\n", __LINE__, __func__, __FILE__);

#ifdef DHCP_SERVER_MULTIIF
	utt_getMac(server_config.interface, mac);
	dbck("mac: %02x%02x%02x%02x%02x%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
#endif

	/* Setup the signal pipe */
	udhcp_sp_setup();

	timeout_end = monotonic_sec() + server_config.auto_time;
	while (1) { /* loop until universe collapses */

		if (server_socket < 0) {
			server_socket = listen_socket(/*INADDR_ANY,*/ SERVER_PORT,
					server_config.interface);
		}

		max_sock = udhcp_sp_fd_set(&rfds, server_socket);
		if (server_config.auto_time) {
			tv.tv_sec = timeout_end - monotonic_sec();
			tv.tv_usec = 0;
		}
		retval = 0;
		if (!server_config.auto_time || tv.tv_sec > 0) {
			retval = select(max_sock + 1, &rfds, NULL, NULL,
					server_config.auto_time ? &tv : NULL);
		}

		if (retval == 0) {
			write_leases();
			timeout_end = monotonic_sec() + server_config.auto_time;
			continue;
		}
		if (retval < 0 && errno != EINTR) {
			DEBUG("error on select");
			continue;
		}

		switch (udhcp_sp_read(&rfds)) {
		case SIGUSR1:
			dbck("%d in %s of %s, SIGUSR1\n", __LINE__, __func__, __FILE__);
			bb_info_msg("Received a SIGUSR1");
			write_leases();
			/* why not just reset the timeout, eh */
			timeout_end = monotonic_sec() + server_config.auto_time;
			continue;
#ifdef DHCP_SERVER_MULTIIF
		case SIGUSR2:
			bb_info_msg("Received a SIGUSR2");
			dbck("%d in %s of %s, SIGUSR2\n", __LINE__, __func__, __FILE__);
			read_config(DHCPD_VLAN_CONF_FILE, 0);

			for (i = 0; i < DHCP_SERVER_HASH; i++) {
				tmp = server_config.dhcp_pool[i].next;

				while (tmp != NULL) {
					dhcp_pool_ptr = tmp;
					dbck("%d in %s of %s, vid = %u\n", __LINE__, __func__, __FILE__, tmp->vid);
					option = find_option(tmp->options, DHCP_LEASE_TIME);
					tmp->lease = LEASE_TIME;
					if (option) {
						memcpy(&tmp->lease, option->data + 2, 4);
						tmp->lease = ntohl(tmp->lease);
					}

					/* Sanity check */
					dbck("%d in %s of %s, tmp->end_ip = %u, tmp->start_ip = %u\n", __LINE__, __func__, __FILE__, tmp->end_ip, tmp->start_ip);
					num_ips = tmp->end_ip - tmp->start_ip + 1;
					if (tmp->max_leases > num_ips) {
						dbck("%d in %s of %s, max_leases= %u, num_ips = %u\n", __LINE__, __func__, __FILE__, tmp->max_leases, num_ips);
						bb_error_msg("max_leases=%u is too big, setting to %u",
							(unsigned)tmp->max_leases, num_ips);
						tmp->max_leases = num_ips;
					}

					if (tmp->leases == NULL) {
						tmp->leases = xzalloc(tmp->max_leases * sizeof(*(tmp->leases)));
					}

					tmp = tmp->next;
				}
			}

			read_leases(server_config.lease_file);

#if 0
			if (read_interface(server_config.interface, &server_config.ifindex,
					   &server_config.server, server_config.arp)) {
				retval = 1;
				goto ret;
			}
#endif
#endif
			break;
		case SIGTERM:
			bb_info_msg("Received a SIGTERM");
			goto ret0;
		case 0: break;		/* no signal */
		default: continue;	/* signal or error (probably EINTR) */
		}

#ifdef DHCP_SERVER_MULTIIF
		bytes = udhcp_recv_kernel_packet(SERVER_PORT, &vid, &packet, server_socket); /* this waits for a packet - idle */
#else
		bytes = udhcp_recv_kernel_packet(&packet, server_socket); /* this waits for a packet - idle */
#endif
		if (bytes < 0) {
			if (bytes == -1 && errno != EINTR) {
				DEBUG("error on read, %s, reopening socket", strerror(errno));
				close(server_socket);
				server_socket = -1;
			}
			continue;
		}
#ifdef DHCP_SERVER_MULTIIF
		tmp = get_current_pool(vid, 1);
		if (tmp == NULL) {
			dbck("%d in %s of %s, vid %d is not exist\n", __LINE__, __func__, __FILE__, vid);
			continue;
		}
#endif

		state = get_option(&packet, DHCP_MESSAGE_TYPE);
		if (state == NULL) {
			dbck("%d in %s of %s\n", __LINE__, __func__, __FILE__);
			bb_error_msg("cannot get option from packet, ignoring");
			continue;
		}

		/* Look for a static lease */
#ifdef DHCP_SERVER_MULTIIF
		static_lease_ip = getIpByMac(dhcp_pool_ptr->static_leases, &packet.chaddr);
#else
		static_lease_ip = getIpByMac(server_config.static_leases, &packet.chaddr);
#endif

		if (static_lease_ip) {
#ifdef DHCP_SERVER_MULTIIF
			dbck("%d in %s of %s, vid %d\n", __LINE__, __func__, __FILE__, dhcp_pool_ptr->vid);
#endif
			bb_info_msg("Found static lease: %x", static_lease_ip);

			memcpy(&static_lease.chaddr, &packet.chaddr, 16);
			static_lease.yiaddr = static_lease_ip;
			static_lease.expires = 0;

			lease = &static_lease;
		} else {
#ifdef DHCP_SERVER_MULTIIF
			dbck("%d in %s of %s, vid %d\n", __LINE__, __func__, __FILE__, dhcp_pool_ptr->vid);
#endif
			lease = find_lease_by_chaddr(packet.chaddr);
		}

		switch (state[0]) {
		case DHCPDISCOVER:
#ifdef DHCP_SERVER_MULTIIF
			dbck("%d in %s of %s, vid %d\n", __LINE__, __func__, __FILE__, dhcp_pool_ptr->vid);
#endif
			DEBUG("Received DISCOVER");

#ifdef DHCP_SERVER_MULTIIF
			if (send_offer(&packet, vid) < 0) {
#else
			if (send_offer(&packet) < 0) {
#endif
				bb_error_msg("send OFFER failed");
			}
			break;
		case DHCPREQUEST:
			dbck("%d in %s of %s, DHCPREQUEST\n", __LINE__, __func__, __FILE__);
			DEBUG("received REQUEST");

			requested = get_option(&packet, DHCP_REQUESTED_IP);
			server_id = get_option(&packet, DHCP_SERVER_ID);

			if (requested) memcpy(&requested_align, requested, 4);
			if (server_id) memcpy(&server_id_align, server_id, 4);

			if (lease) {
				if (server_id) {
					/* SELECTING State */
					DEBUG("server_id = %08x", ntohl(server_id_align));
					if (server_id_align == server_config.server && requested
					 && requested_align == lease->yiaddr
					) {
#ifdef DHCP_SERVER_MULTIIF
						dbck("%d in %s of %s\n", __LINE__, __func__, __FILE__);
						send_ACK(&packet, lease->yiaddr, vid);
#else
						send_ACK(&packet, lease->yiaddr);
#endif
					}
				} else if (requested) {
					/* INIT-REBOOT State */
					if (lease->yiaddr == requested_align) {
#ifdef DHCP_SERVER_MULTIIF
						dbck("%d in %s of %s\n", __LINE__, __func__, __FILE__);
						send_ACK(&packet, lease->yiaddr, vid);
#else
						send_ACK(&packet, lease->yiaddr);
#endif
					} else {
#ifdef DHCP_SERVER_MULTIIF
						dbck("%d in %s of %s\n", __LINE__, __func__, __FILE__);
						send_NAK(&packet, vid);
#else
						send_NAK(&packet);
#endif
					}
				} else if (lease->yiaddr == packet.ciaddr) {
					/* RENEWING or REBINDING State */
#ifdef DHCP_SERVER_MULTIIF
					dbck("%d in %s of %s\n", __LINE__, __func__, __FILE__);
					send_ACK(&packet, lease->yiaddr, vid);
#else
					send_ACK(&packet, lease->yiaddr);
#endif
				} else { /* don't know what to do!!!! */
#ifdef DHCP_SERVER_MULTIIF
					dbck("%d in %s of %s\n", __LINE__, __func__, __FILE__);
					send_NAK(&packet, vid);
#else
					send_NAK(&packet);
#endif
				}

			/* what to do if we have no record of the client */
			} else if (server_id) {
				/* SELECTING State */
				dbck("%d in %s of %s\n", __LINE__, __func__, __FILE__);

			} else if (requested) {
				/* INIT-REBOOT State */
				dbck("%d in %s of %s\n", __LINE__, __func__, __FILE__);
				lease = find_lease_by_yiaddr(requested_align);
				if (lease) {
					if (lease_expired(lease)) {
						/* probably best if we drop this lease */
						memset(lease->chaddr, 0, 16);
					/* make some contention for this address */
					} else
#ifdef DHCP_SERVER_MULTIIF
					dbck("%d in %s of %s\n", __LINE__, __func__, __FILE__);
						send_NAK(&packet, vid);
#else
						send_NAK(&packet);
#endif
				} else {
					uint32_t r = ntohl(requested_align);
#ifdef DHCP_SERVER_MULTIIF
					if (r < dhcp_pool_ptr->start_ip
				         || r > dhcp_pool_ptr->end_ip
					) {
						dbck("%d in %s of %s\n", __LINE__, __func__, __FILE__);
						send_NAK(&packet, vid);
#else
					if (r < server_config.start_ip
				         || r > server_config.end_ip
					) {
						dbck("%d in %s of %s\n", __LINE__, __func__, __FILE__);
						send_NAK(&packet);
#endif
					}
					/* else remain silent */
				}

			} else {
				/* RENEWING or REBINDING State */
				dbck("%d in %s of %s\n", __LINE__, __func__, __FILE__);
			}
			break;
		case DHCPDECLINE:
			dbck("%d in %s of %s, DHCPDELINE\n", __LINE__, __func__, __FILE__);
			DEBUG("Received DECLINE");
			if (lease) {
				memset(lease->chaddr, 0, 16);
#if 0
                /*edit by brwang*/
				lease->expires = time(0) + server_config.decline_time;
#else
				lease->expires = utt_getSysUpTime() + server_config.decline_time;
#endif
			}
			break;
		case DHCPRELEASE:
			dbck("%d in %s of %s, DHCPRELEASE\n", __LINE__, __func__, __FILE__);
			DEBUG("Received RELEASE");
			lease = find_lease_by_chaddr(packet.chaddr);
			if (lease){
#if (DHCP_AUTO_BIND == FYES)
				autodel(lease);
#endif
//              lease->expires = time(0);
                memset(lease,0,sizeof(struct dhcpOfferedAddr));
                if (ENABLE_FEATURE_UDHCPD_WRITE_LEASES_EARLY) {
                    /* rewrite the file with leases at every new acceptance */
                    write_leases();
                }
	        }
			break;
		case DHCPINFORM:
			dbck("%d in %s of %s, DHCPINFORM\n", __LINE__, __func__, __FILE__);
			DEBUG("Received INFORM");
#ifdef DHCP_SERVER_MULTIIF
			send_inform(&packet, vid);
#else
			send_inform(&packet);
#endif
		break;
		default:
			dbck("%d in %s of %s, default\n", __LINE__, __func__, __FILE__);
			bb_info_msg("Unsupported DHCP message (%02x) - ignoring", state[0]);
		}
	}
#if (DHCP_AUTO_BIND == FYES)
    ProfDetach();
#endif
 ret0:
	retval = 0;
 ret:
	/*if (server_config.pidfile) - server_config.pidfile is never NULL */
		remove_pidfile(server_config.pidfile);
	return retval;
}
