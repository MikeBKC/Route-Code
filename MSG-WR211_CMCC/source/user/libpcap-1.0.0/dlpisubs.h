/*
 * @(#) $Header: /home/cvs/3520G/user/libpcap-1.0.0/dlpisubs.h,v 1.1 2013/01/25 19:02:33 chen.bin Exp $
 */

#ifndef dlpisubs_h
#define	dlpisubs_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Functions used by dlpisubs.c.
 */
int pcap_stats_dlpi(pcap_t *, struct pcap_stat *);
int pcap_process_pkts(pcap_t *, pcap_handler, u_char *, int, u_char *, int);
int pcap_process_mactype(pcap_t *, u_int);
#ifdef HAVE_SYS_BUFMOD_H
int pcap_conf_bufmod(pcap_t *, int, int);
#endif
int pcap_alloc_databuf(pcap_t *);
int strioctl(int, int, int, char *);

#ifdef __cplusplus
}
#endif

#endif
