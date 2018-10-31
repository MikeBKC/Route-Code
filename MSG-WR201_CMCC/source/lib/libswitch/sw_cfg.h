#ifndef __CFG_H__
#define __CFG_H__
#include "urcp_ipc.h"


#define SW_SHM_KEY     0xAA09 
#define SW_MSG_KEY     0xAB09 
#define SW_SEM_KEY     0xAC09 
extern void shm_init(void);
extern int sw_shm_init(void);
extern urcp_shm_t* get_shm_ptr(void);
extern int sw_sem_init(void);
extern int sw_msg_init(void);
extern int sw_sem_v(int semid, int no_wait);
extern int sw_sem_p(int semid, int no_wait);

extern uint32 urcp_dev_table_hash(unsigned char* mac, uint32 ip);
extern urcp_dev_t* urcp_dev_table_lookup(unsigned char* mac, uint32 ip);
extern uint32 urcp_read_port_num(unsigned char mac[MAC_LEN], uint32 ip);


extern void cfg_write_urcp_swap_conf(char *conf_data, uint32 ulen);
extern void cfg_read_urcp_swap_conf(char *dst_conf, uint32 ulen);
extern void init_misc_opt_assist(misc_opt_assist*misc_buff,unsigned char* misc_addr,uint32 u_unit_size,uint32 u_misc_buffer_size);
extern void urcp_create_general_record(unsigned char macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type,misc_opt_assist* misc_buff,unsigned char*  config);
extern int32 urcp_rev_general_get_reply(unsigned char macaddr[MAC_LEN], uint16 id,misc_opt_assist* misc_buff,unsigned char*conf);
extern urcp_status_t urcp_read_general_conf(unsigned char mac[MAC_LEN], uint32 userIp, uint32 type, misc_opt_assist* misc_buff ,unsigned char* conf);

extern void urcp_create_mirror_record(unsigned char macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type, urcp_mirror_conf_t* mirror_config);
extern urcp_status_t urcp_read_mirror_conf(unsigned char mac[MAC_LEN], uint32 userIp, uint32 type, urcp_mirror_conf_t* conf);
#if 0
extern int32 urcp_rev_mirror_set_reply(unsigned char macaddr[MAC_LEN], uint16 id, unsigned char *errmsg);
#endif
extern int32 urcp_rev_mirror_get_reply(unsigned char macaddr[MAC_LEN], uint16 id, urcp_mirror_conf_t *conf);
extern int32 urcp_mirror_set_status(uint16 id, urcp_status_t error_status);

extern void urcp_create_aggr_record(unsigned char macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type,urcp_aggr_conf_t* aggr_config);
extern urcp_status_t urcp_read_aggr_conf(unsigned char mac[MAC_LEN], uint32 userIp, uint32 type, urcp_aggr_conf_t* conf);
extern int32 urcp_rev_aggr_get_reply(unsigned char macaddr[MAC_LEN], uint16 id, urcp_aggr_conf_t *conf);
extern int32 urcp_aggr_set_status(uint16 uid, urcp_status_t error_status);

extern void urcp_create_vlan_record(unsigned char macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type,urcp_vlan_conf_t* vlan_config);
extern urcp_status_t urcp_read_vlan_conf(unsigned char mac[MAC_LEN], uint32 userIp, uint32 type, urcp_vlan_conf_t* conf);
extern int32 urcp_rev_vlan_get_reply(unsigned char macaddr[MAC_LEN], uint16 uid, urcp_vlan_conf_t *conf);
extern int32 urcp_vlan_set_status(uint16 uid, urcp_status_t error_status);

extern void urcp_create_qvlan_pconf_record(unsigned char macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type, urcp_qvlan_pconf_conf_t* qvlan_pconf_config);
extern urcp_status_t urcp_read_qvlan_pconf_conf(unsigned char mac[MAC_LEN], uint32 userIp, uint32 type, urcp_qvlan_pconf_conf_t* conf);
extern int32 urcp_rev_qvlan_pconf_get_reply(unsigned char macaddr[MAC_LEN], uint16 id, urcp_qvlan_pconf_conf_t *conf);
extern int32 urcp_qvlan_pconf_set_status(uint16 uid, urcp_status_t error_status);


extern urcp_status_t urcp_read_passwd_auth(unsigned char mac[MAC_LEN], uint32 userIp, uint32 type);
extern void urcp_create_passwd_auth_record(unsigned char macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type);
extern int32 urcp_passwd_auth_set_status(uint16 id, urcp_status_t error_status);


extern void urcp_create_rstp_lacp_record(unsigned char macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type, urcp_rstp_lacp_conf_t* rstp_lacp_config);
extern urcp_status_t urcp_read_rstp_lacp_conf(unsigned char mac[MAC_LEN], uint32 userIp, uint32 type, urcp_rstp_lacp_conf_t* conf);
extern int32 urcp_rev_rstp_lacp_get_reply(unsigned char macaddr[MAC_LEN], uint16 id, urcp_rstp_lacp_conf_t *conf);
extern int32 urcp_rstp_lacp_set_status(uint16 uid, urcp_status_t error_status);

extern void init_port_conf_misc_opt_assist(misc_opt_assist* misc_buff);
extern int32 urcp_port_conf_set_status(uint16 uid, urcp_status_t error_status);

extern void init_qos_rate_limit_misc_opt_assist(misc_opt_assist* misc_buff);
extern int32 urcp_qos_rate_limit_set_status(uint16 uid, urcp_status_t error_status);

extern void init_storm_rate_limit_misc_opt_assist(misc_opt_assist* misc_buff);
extern int32 urcp_storm_rate_limit_set_status(uint16 uid, urcp_status_t error_status);

extern void init_sys_conf_misc_opt_assist(misc_opt_assist* misc_buff);
extern int32 urcp_sys_conf_set_status(uint16 uid, urcp_status_t error_status);

extern void init_sys_info_misc_opt_assist(misc_opt_assist* misc_buff);
extern int32 urcp_sys_info_set_status(uint16 uid, urcp_status_t error_status);

extern void init_ip_filter_misc_opt_assist(misc_opt_assist* misc_buff);
extern int32 urcp_ip_filter_set_status(uint16 uid, urcp_status_t error_status);

extern void init_cmd_alone_misc_opt_assist(misc_opt_assist* misc_buff);
extern int32 urcp_cmd_alone_set_status(uint16 uid, urcp_status_t error_status);
extern urcp_status_t urcp_read_cmd_alone_conf(unsigned char mac[MAC_LEN], uint32 userIp, uint32 utype);

extern void init_password_misc_opt_assist(misc_opt_assist* misc_buff);
extern int32 urcp_password_set_status(uint16 uid, urcp_status_t error_status);
extern void urcp_create_password_record(unsigned char macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type,unsigned char *passwd_config);
extern urcp_status_t urcp_read_password(unsigned char mac[MAC_LEN], uint32 userIp, uint32 type);

extern int32 urcp_config_set_status(uint16 uid, urcp_status_t error_status);
extern urcp_status_t urcp_read_config_conf(unsigned char mac[MAC_LEN], uint32 userIp, uint32 type);
extern void urcp_create_config_record(unsigned char macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type);

/* remote AP basic Configuration */
extern int32 urcp_ap_basic_set_status(uint16 uid, urcp_status_t error_status);
extern void urcp_create_ap_basic_record(uchar macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type);
extern urcp_status_t urcp_read_ap_basic_conf(uchar mac[MAC_LEN], uint32 userIp, uint32 type);
extern void init_ap_basic_misc_opt_assist(misc_opt_assist* misc_buff);

/* remote AP config template */
extern void init_ap_conf_temp_misc_opt_assist(misc_opt_assist* misc_buff);
extern void urcp_create_ap_conf_temp_record(unsigned char macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type);
extern urcp_status_t urcp_read_ap_conf_temp(unsigned char mac[MAC_LEN], uint32 userIp, uint32 type);
extern int32 urcp_ap_conf_temp_set_status(uint16 uid, urcp_status_t error_status);

extern void init_ap_join_sz_misc_opt_assist(misc_opt_assist* misc_buff);
extern void urcp_create_ap_join_sz_record(uchar macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type);
extern urcp_status_t urcp_read_ap_join_sz_conf(uchar mac[MAC_LEN], uint32 userIp, uint32 type);
extern int32 urcp_ap_join_sz_set_status(uint16 uid, urcp_status_t error_status);

extern int32 urcp_wl_mac_filter_set_status(uint16 uid, urcp_status_t error_status);


extern void init_ap_mac_filter_misc_opt_assist(misc_opt_assist* misc_buff);
extern void urcp_create_ap_mac_filter_record(uchar macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type);
extern urcp_status_t urcp_read_ap_mac_filter_conf(uchar mac[MAC_LEN], uint32 userIp, uint32 type);
extern int32 urcp_ap_mac_filter_set_status(uint16 uid, urcp_status_t error_status);

#endif

