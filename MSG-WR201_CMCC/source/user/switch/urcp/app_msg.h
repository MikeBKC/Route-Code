
#ifndef  __URCPAPP_MSG_H__
#define  __URCPAPP_MSG_H__
#include "urcp.h"

#define CONFIG_FILE        "config"
#define LOG_FILE           "log"
#define TFTP_ROOT          "/tmp/urcp/"
#define IMAGE_FILE         "image"
#define CONFIG_TEMPLATE_FILE	"template"
#define TFTP_CONFIG_ROOT   RAM_CONFIG_PATH      /*配置文件存放目录*/

#define NONE_VLAN  0u
#define PORT_VLAN  1u
#define TAG_VLAN   2u

#ifdef URCP_AC
extern int32 urcp_msg_get_general(urcp_general_msg_t* urcp_general_msg, uint32 cmd_type, misc_opt_assist* misc_buff, void (*func_cb)(uint16 reqid, session_type_t type, urcp_status_t state));
extern int32 urcp_msg_set_general(urcp_general_msg_t* urcp_general_msg, uint32 cmd_type, misc_opt_assist* misc_buff,unsigned char* urcp_config, void (*func_cb)(uint16 reqid, session_type_t type, urcp_status_t state));

extern int32 urcp_msg_get_port_config(struct msgbuf_t *web_msg);
extern int32 urcp_msg_set_port_config(struct msgbuf_t *web_msg);


extern int32 urcp_msg_get_mirror(struct msgbuf_t *web_msg);
extern int32 urcp_msg_set_mirror(struct msgbuf_t *web_msg);

extern int32 urcp_msg_aggr_config(struct msgbuf_t *web_msg);
extern int32 urcp_msg_get_passwd_auth(struct msgbuf_t *web_msg, uint32 web_id);
extern int32 urcp_msg_get_rstp_lacp(struct msgbuf_t *web_msg); 

extern int32 urcp_msg_vlan_config(struct msgbuf_t *web_msg);
extern int32 urcp_msg_qvlan_port_config(struct msgbuf_t *web_msg);

extern int32 urcp_msg_get_qos_rate_limit_config(struct msgbuf_t *web_msg);
extern int32 urcp_msg_set_qos_rate_limit_config(struct msgbuf_t *web_msg);

extern int32 urcp_msg_get_storm_rate_limit_config(struct msgbuf_t *web_msg);
extern int32 urcp_msg_set_storm_rate_limit_config(struct msgbuf_t *web_msg);

extern int32 urcp_msg_cmd_alone_config(struct msgbuf_t *web_msg);

extern int32 urcp_msg_get_sys_info_config(struct msgbuf_t *web_msg);

extern int32 urcp_msg_get_sys_conf_config(struct msgbuf_t *web_msg);
extern int32 urcp_msg_set_sys_conf_config(struct msgbuf_t *web_msg);

extern int32 urcp_msg_get_ip_filter_config(struct msgbuf_t *web_msg);

extern int32 urcp_msg_password_config(struct msgbuf_t *web_msg);

extern int32 urcp_msg_ap_basic_config(struct msgbuf_t *msgbuf);
extern int32 urcp_msg_set_ap_basic_config(struct msgbuf_t *msgbuf);
extern int32 urcp_msg_get_ap_basic_config(struct msgbuf_t *web_msg);
extern int32 urcp_msg_set_ap_join_sz(struct msgbuf_t *msgbuf);

#endif /*end URCP_AC*/

extern void urcp_file_cb(void *ptr, urcp_status_t urcp_state, const char *path);
extern void urcp_copy_put_file(void *ptr, urcp_status_t urcp_state, uint32 serial_no);
extern void urcp_get_log_cb(void *ptr, urcp_status_t urcp_state);
extern void urcp_get_image_cb(void *ptr, urcp_status_t urcp_state);
extern int32 create_log_file(char *file);

#endif
