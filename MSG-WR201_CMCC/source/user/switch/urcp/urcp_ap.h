#ifndef __URCP_AP_H__
#define __URCP_AP_H__

#include "common.h"
#include "ap_msg.h"

#ifdef URCP_AC
#define SCAN_AUTO 1
extern int32 urcp_ac_discover(void);
extern void init_ap_scan_mode(void);
extern void ac_timer(void);
extern void ac_timer_init(void);
#endif

#ifdef URCP_AP
extern void ap_state_handle(void);
extern void ap_timer_init(void);
//extern void urcp_msg_change_config(struct msgbuf_t *msgbuf);
extern void get_ip_from_dhcp_server(uint32 ac_ip);
extern char * int2ip(unsigned int lip,char * ip);
extern void send_ap_clients(unsigned int sta_num, uchar *sta_mac, sta_status stu);
extern void send_ap_upper_limit_msg(unsigned int max_num);
extern int32 isValidMac(uchar* mac);
#endif

#endif
