#ifndef __SW_IOCTL_H__
#define __SW_IOCTL_H__

#include "linux/switch/swioc_common.h"

#define SWIOC_DEV_NAME          "/dev/switch_ext" /*switch ioctl×¢²áÉè±¸Ãû*/
#define OPEN_SW_DEV_ERROR       "open "SWIOC_DEV_NAME" error:"

extern int32 sw_ioctl(int32 cmd, unsigned char* pData);
extern void sw_ioctl_error(const char* p_str);
extern int32 sw_mac_table_forget_vid_mac(vid_mac_t* mac);
extern int32 sw_mac_table_static_learn(mac_table_entry_t* entry);

extern int32 sw_mac_table_age_time_set(mac_age_time_t age_time);
extern int32 sw_mac_table_age_time_get(mac_age_time_t *age_time);
extern int32 sw_mac_table_age_time_enable_set(int32 enable);
extern int32 sw_mac_table_age_time_enable_get(int32 *enable);

#if SW_FEATURE_PORT_VLAN
#if 0
extern int32 sw_pvlan_port_members_get(pvlan_no_t pvlan_no, BOOL member[PORT_ARRAY_SIZE]);
#endif
extern int32 sw_pvlan_port_members_set(pvlan_no_t pvlan_no, BOOL member[PORT_ARRAY_SIZE]);
#endif

#if SW_FEATURE_PORT_CONF
extern int32 sw_port_conf_set(port_no_t port_no, port_conf_t* conf);
extern int32 sw_link_status_get(port_no_t port_no, char* status);
#endif

#if SW_FEATURE_PORT_MIRROR
/* mirror mode: disable, ingress, egress, ingress and egress */
extern int32 sw_mirror_egress_ingress_port_mode_set(BOOL port_no[PORT_ARRAY_SIZE], int32 m_mode);
extern int32 sw_mirror_egress_ingress_port_mode_get(BOOL port_no[PORT_ARRAY_SIZE], int32 *m_mode);
/* watching port */
extern int32 sw_mirror_monitor_port_set(port_no_t port_no);
extern int32 sw_mirror_monitor_port_get(port_no_t *port_no);
#endif

extern int32 sw_vlan_type_get(void);

#if SW_FEATURE_ACL
extern int32 sw_acl_enable(int32 enable);
extern int32 sw_acl_status_get(int32 *enable);
extern int32 sw_acl_add(sw_acl_t * acl);
extern int32 sw_acl_del(int32 id);
extern int32 sw_acl_get(sw_acl_t * acl);
#endif

#if SW_FEATURE_PORT_TRUNK
extern int32 sw_trunk_group_set(int32 group_no, BOOL members[PORT_ARRAY_SIZE]);
extern int32 sw_trunk_group_get(int32 group_no, BOOL members[PORT_ARRAY_SIZE]);
extern int32 sw_trunk_mode_set(int32 value);
extern int32 sw_trunk_mode_get(int32 *value);
extern int32 sw_trunk_members_get(port_no_t port_no, BOOL member[PORT_ARRAY_SIZE]);
#endif

#if SW_FEATURE_PACKET
#if 0
extern int32 sw_bcp_get_dst_ports(int32 exclude_port, BOOL** dst_ports);
#endif
extern int32 sw_fwd_get_dst_ports(fwd_dst_ports_cond_t* cond, fwd_dst_ports_ret_t* ret);
#endif

#if SW_FEATURE_PORT_STATISTICS
extern int32 sw_port_counter_get(port_no_t port_no, sw_port_stat_t *stat);
extern int32 sw_port_counter_clear(port_no_t port_no);
#endif

extern int32 sw_cpu_vid_get(void);
extern void sw_check_ports_link_status(BOOL *ports);
#if SW_FEATURE_MAC_TABLE_OPT
extern int32 sw_show_mac_table(void);
#endif

#if SW_FEATURE_PORT_PRIORITY
extern int32 sw_port_priority_set(uint32 port_prio[PORT_ARRAY_SIZE]);
#endif

#if SW_FEATURE_STORM_CONTROL
extern int32 sw_broadcast_storm_control_set(BOOL ctrl_port[PORT_ARRAY_SIZE], uint32 value, BOOL timer_sel);
#endif

#if SW_FEATURE_RATE_LIMIT
extern int32 sw_port_bandwidth_func_set(port_no_t port_no, uchar type, uint32 tx_rate, uint32 rx_rate);
extern int32 sw_port_bandwidth_throttle_set(uchar throttle);
#endif

#endif
