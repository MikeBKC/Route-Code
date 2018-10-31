/*
 *
 *联动上层功能函数
 *
 *KeHuatao
 */

#ifndef __URCP_HELLO_H__
#define __URCP_HELLO_H__

#include "urcp.h"

extern uint16 receive_hello(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info);
extern uint16 get_hello(cmd_hdr_t *cmd, uchar** app_data, urcp_infor_t* urcp_info);
extern void get_dev_password(uchar* mac, uint32 ip, uchar* password);
extern void urcp_reset(void);
extern int32 urcp_discover(void);
extern void dev_table_init(void);
extern void cooperate_mac_add(void);
extern urcp_dev_t* dev_get(uchar* dev_mac, uint32 vid, uint32 dev_ip, int32 dev_type);
extern int32 urcp_mirror_can(void);
#if (AP_LOG_MANAGE == FYES)
extern void ap_syslog(ap_log_list logType, char *msg);
#endif
#endif
