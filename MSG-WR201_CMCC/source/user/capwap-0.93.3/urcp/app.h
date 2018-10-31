
#ifndef  __URCPAPP_H__
#define  __URCPAPP_H__

#ifdef URCP_CMD_SERIAL_NO_GET_REPLY
uint16 receive_serial_no(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_DEV_NAME_GET_REPLY
uint16 receive_dev_name(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_UP_PORTS_GET_REPLY
uint16 receive_up_ports(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_PRODUCT_MODEL_GET_REPLY
uint16 receive_product_model(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_SW_VERSION_GET_REPLY
uint16 receive_sw_version(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_HW_VERSION_GET_REPLY
uint16 receive_hw_version(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_SERIAL_NO_GET
uint16 get_serial_no(cmd_hdr_t *cmd_buf, uchar** app_data, urcp_infor_t* urcp_info);
uint16 trap_get_serial_no(cmd_hdr_t *cmd_buf, uchar** app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_SW_VERSION_GET
uint16 get_sw_version(cmd_hdr_t *cmd_buf, uchar **app, urcp_infor_t* urcp_info);
uint16 trap_get_sw_version(cmd_hdr_t *cmd_buf, uchar **app, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_HW_VERSION_GET
uint16 get_hw_version(cmd_hdr_t *cmd_buf, uchar **app, urcp_infor_t* urcp_info);
uint16 trap_get_hw_version(cmd_hdr_t *cmd_buf, uchar **app, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_PRODUCT_MODEL_GET
uint16 get_product_model(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
uint16 trap_get_product_model(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_DHCP_EN_GET
uint16 get_ap_dhcp_en(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
uint16 trap_get_ap_dhcp_en(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_ROAMING_TH_GET
uint16 get_ap_roaming_th(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_ROAMING_TH_5G_GET
uint16 get_ap_roaming_th_5g(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_ROAMING_TH_5G_SET
uint16 set_ap_roaming_th_5g(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_DEV_NAME_GET
uint16 get_dev_name(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
uint16 trap_get_dev_name(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_UP_PORTS_GET
uint16 get_up_ports(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_MIRROR_CONFIG_GET
uint16 get_mirror_config(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_MIRROR_CONFIG_GET_REPLY
uint16 receive_mirror_config(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_infor);
#endif
#ifdef URCP_CMD_MIRROR_CONFIG_SET
uint16 set_mirror_config(cmd_hdr_t *cmd, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#if defined(URCP_CMD_MIRROR_CONFIG_GET_REPLY) || defined(URCP_CMD_MIRROR_CONFIG_SET_REPLY) 
extern void urcp_mirror_cb(uint16 reqId, session_type_t a_type,urcp_status_t urcp_state);
#endif
#ifdef URCP_CMD_PASSWD_AUTH_GET
uint16 get_passwd_auth(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_PASSWD_AUTH_GET_REPLY
void urcp_passwd_auth_cb(uint16 reqId, session_type_t type1, urcp_status_t urcp_state);
#endif
#ifdef URCP_CMD_AGGR_CONFIG_GET
uint16 get_aggr_config (cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AGGR_CONFIG_SET
uint16 set_aggr_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_AGGR_CONFIG_GET_REPLY
uint16 receive_aggr_config(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info);
#endif
#if defined(URCP_CMD_AGGR_CONFIG_GET_REPLY) || defined(URCP_CMD_AGGR_CONFIG_SET_REPLY)
extern void urcp_aggr_cb(uint16 reqId, session_type_t a_type,urcp_status_t urcp_state);
#endif
#ifdef URCP_CMD_RSTP_LACP_CONFIG_GET
uint16 get_rstp_lacp_config(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_RSTP_LACP_CONFIG_GET_REPLY
uint16 receive_rstp_lacp_config(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_infor);
void urcp_rstp_lacp_cb(uint16 reqId, session_type_t a_type,urcp_status_t urcp_state);
#endif
#if defined(URCP_CMD_VLAN_CONFIG_GET) || defined(URCP_CMD_QVLAN_PCONF_CONFIG_GET)
uint16 get_vlan_config(cmd_hdr_t *cmd, uchar* app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_VLAN_CONFIG_SET
uint16 set_vlan_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_VLAN_CONFIG_GET_REPLY
uint16 receive_vlan_config(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_infor);
void urcp_vlan_cb(uint16 reqId, session_type_t a_type,urcp_status_t urcp_state);
#endif
#ifdef URCP_CMD_QVLAN_PCONF_CONFIG_GET
uint16 get_qvlan_pconf_config(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_QVLAN_PCONF_CONFIG_GET_REPLY
uint16 receive_qvlan_pconf_config(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_infor);
void urcp_qvlan_pconf_cb(uint16 reqId, session_type_t a_type,urcp_status_t urcp_state);
#endif
#ifdef URCP_CMD_PORT_CONFIG_GET
uint16 get_port_conf_config(cmd_hdr_t *cmd_buf, uchar** app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_PORT_CONFIG_SET
uint16 set_port_conf_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_PORT_CONFIG_GET_REPLY
uint16 receive_port_conf_config(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_infor);
#endif
#if defined(URCP_CMD_PORT_CONFIG_GET_REPLY) || defined(URCP_CMD_PORT_CONFIG_SET_REPLY)
void urcp_port_conf_cb(uint16 reqId, session_type_t a_type,urcp_status_t urcp_state);
#endif
#ifdef URCP_CMD_QOS_RATE_LIMIT_CONFIG_GET 
uint16 get_qos_rate_limit_config(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_QOS_RATE_LIMIT_CONFIG_SET
uint16 set_qos_rate_limit_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_QOS_RATE_LIMIT_CONFIG_GET_REPLY
uint16 receive_qos_rate_limit_config(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_infor);
#endif
#if defined(URCP_CMD_QOS_RATE_LIMIT_CONFIG_GET_REPLY) || defined (URCP_CMD_QOS_RATE_LIMIT_CONFIG_SET_REPLY)
void urcp_qos_rate_limit_cb(uint16 reqId, session_type_t a_type,urcp_status_t urcp_state);
#endif
#ifdef URCP_CMD_STORM_RATE_LIMIT_CONFIG_GET
uint16 get_storm_rate_limit_config(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_STORM_RATE_LIMIT_CONFIG_SET
uint16 set_storm_rate_limit_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_STORM_RATE_LIMIT_CONFIG_GET_REPLY
uint16 receive_storm_rate_limit_config(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_infor);
#endif
#if defined(URCP_CMD_STORM_RATE_LIMIT_CONFIG_GET_REPLY) || defined(URCP_CMD_STORM_RATE_LIMIT_CONFIG_SET_REPLY)
void urcp_storm_rate_limit_cb(uint16 reqId, session_type_t a_type,urcp_status_t urcp_state);
#endif
uint16 build_cmd_alone_reply_cmd(cmd_hdr_t *cmd, uchar** app_data);
extern void urcp_cmd_alone_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state);

#ifdef URCP_CMD_SAFEBIND_CONFI_SET
extern void set_safebind_config(void);
#endif
#ifdef URCP_CMD_SYS_CONF_CONFIG_GET
uint16 get_sys_conf_config(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_SYS_CONF_CONFIG_SET
uint16 set_sys_conf_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_SYS_CONF_CONFIG_GET_REPLY
uint16 receive_sys_conf_config(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_infor);
#endif
#if defined(URCP_CMD_SYS_CONF_CONFIG_GET_REPLY) || defined(URCP_CMD_SYS_CONF_CONFIG_SET_REPLY)
extern void urcp_sys_conf_cb(uint16 reqId, session_type_t a_type,urcp_status_t urcp_state);
#endif

#ifdef URCP_CMD_SYS_INFO_CONFIG_GET
uint16 get_sys_info_config (cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_SYS_INFO_CONFIG_GET_REPLY
uint16 receive_sys_info_config(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_infor);
#endif
#if defined(URCP_CMD_SYS_INFO_CONFIG_GET_REPLY)
extern void urcp_sys_info_cb(uint16 reqId, session_type_t a_type,urcp_status_t urcp_state);
#endif

#ifdef URCP_CMD_IP_FILTER_CONFIG_GET
uint16 get_ip_filter_config(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_IP_FILTER_CONFIG_GET_REPLY
uint16 receive_ip_filter_config(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_infor);
#endif
#ifdef URCP_CMD_IP_FILTER_CONFIG_GET_REPLY
extern void urcp_ip_filter_cb(uint16 reqId, session_type_t a_type,urcp_status_t urcp_state);
#endif
#ifdef URCP_CMD_PASSWORD_CONFIG_SET_REPLY
extern void urcp_password_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state);
#endif
#ifdef URCP_CMD_PASSWORD_CONFIG_SET
uint16 set_password_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_AP_CHANNEL_GET_REPLY
uint16 receive_channel(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_CURRENT_CLIENTS_GET_REPLY
uint16 receive_clients(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_SSID_GET_REPLY
uint16 receive_ssid(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif /*end URCP_WEB*/
#ifdef URCP_CMD_AP_WORK_MODE_GET_REPLY
uint16 receive_ap_work_mode(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor);
#endif
#ifdef URCP_CMD_IP_ADDR_GET_REPLY
uint16 receive_ip_addr(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor);
#endif
#ifdef URCP_CMD_NETMASK_GET_REPLY
uint16 receive_netmask(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor);
#endif
#ifdef URCP_CMD_GATEWAY_GET_REPLY
uint16 receive_gateway(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor);
#endif
#ifdef URCP_CMD_AP_USER_NAME_GET_REPLY
uint16 receive_ap_user_name(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor);
#endif
#ifdef URCP_CMD_AP_PASSWORD_GET_REPLY
uint16 receive_ap_password(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor);
#endif
#ifdef URCP_CMD_AP_MAX_CLIENT_GET_REPLY
uint16 receive_ap_max_client(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor);
uint16 receive_ap_max_client_new(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor);
#endif
#ifdef URCP_CMD_AP_CHANNEL_GET_REPLY
uint16 receive_ap_channel(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor);
#endif
#ifdef URCP_CMD_AP_EFFECT_CONF_GET_REPLY
uint16 receive_ap_effect_conf(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor);
#endif

void urcp_wl_mac_filter_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state);
extern uint16 cmd_unknow(uint32 unknow_type, uchar** app_data);
#ifdef URCP_CMD_DISCOVER_GET
uint16 get_discover(cmd_hdr_t *cmd, uchar **app, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_DISCOVER_GET_REPLY
uint16 receive_discover(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_UPDATE_FIRMWARE_GET
uint16 get_ap_update_fireware(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_UPDATE_FIRMWARE_GET_REPLY
uint16 receive_ap_update_fireware(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_UPDATE_PROFILE_GET
uint16 get_ap_update_profile(cmd_hdr_t* cmd, uchar** app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_UPDATE_PROFILE_GET_REPLY
uint16 receive_ap_update_profile(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_AUTH_REQ_GET_REPLY
uint16 receive_ap_auth(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_AUTH_REQ_GET
uint16 get_ap_auth_reqest(cmd_hdr_t* cmd, uchar** app_data, urcp_infor_t* urcp_info);
#endif

#ifdef URCP_CMD_AP_CHANNEL_GET
uint16 get_ap_channel(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
uint16 trap_get_ap_channel(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_SSID_GET
uint16 get_ap_ssid(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
uint16 trap_get_ap_ssid(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_CURRENT_CLIENTS_GET 
uint16 get_ap_clients(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
uint16 trap_get_ap_clients(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_WORK_MODE_GET
uint16 get_ap_work_mode(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_IP_ADDR_GET
uint16 get_ip_addr(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_NETMASK_GET
uint16 get_netmask_addr(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_GATEWAY_GET
uint16 get_gateway_addr(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_USER_NAME_GET
uint16 get_ap_user_name(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_PASSWORD_GET
uint16 get_ap_password(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_MAX_CLIENT_GET
uint16 get_ap_max_client(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
uint16 get_ap_max_client_new(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_EFFECT_CONF_GET
uint16 get_ap_effect_conf(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif

#ifdef URCP_CMD_AP_WORK_MODE_SET
uint16 set_work_mode_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_AP_DHCP_EN_SET
uint16 set_dhcp_en_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_DEV_NAME_SET
uint16 set_dev_name_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_IP_ADDR_SET
uint16 set_ip_addr_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_NETMASK_SET
uint16 set_netmask_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_GATEWAY_SET
uint16 set_gateway_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_AP_USER_NAME_SET
uint16 set_user_name_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_AP_PASSWORD_SET
uint16 set_password_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_AP_MAX_CLIENT_SET
uint16 set_max_client_config(cmd_hdr_t *cmd, uchar** app_data);
uint16 set_max_client_config_new(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_AP_ROAMING_TH_SET
uint16 set_ap_roaming_th(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_AP_CHANNEL_SET
uint16 set_ap_channel_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_AP_CHANNEL_SET_5G
uint16 set_ap_channel_config_5g(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_AP_EFFECT_CONF_SET
uint16 set_effect_conf_config(cmd_hdr_t *cmd, uchar** app_data);
#endif

#ifdef URCP_CMD_WIRELESS_CLT_GET
uint16 get_wl_clt(cmd_hdr_t *cmd_buf, uchar** app_data, urcp_infor_t* urcp_info);
#endif

#ifdef URCP_CMD_WIRELESS_CLT_GET_REPLY
uint16 receive_wl_clt(cmd_hdr_t *cmd_buf, urcp_dev_t **dev, urcp_infor_t* urcp_info);
#endif

#ifdef URCP_CMD_AP_BASIC_CONFIG_GET_REPLY
void urcp_ap_basic_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state);
#endif

#ifdef URCP_CMD_CONFIG_TEMPLATE_SET_REPLY
void urcp_ap_config_template_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state);
#endif

#ifdef URCP_CMD_CONFIG_TEMPLATE_SET
extern void service_zone_for_conf_temp(void);
extern uint16 set_ap_config_template(cmd_hdr_t *cmd, uchar** app_data);
#endif

#ifdef URCP_CMD_REBOOT_CONFIG_SET
extern void sys_reboot(void);
#endif
#ifdef URCP_CMD_RESET_CONFIG_SET
extern void sys_reset(void);
#endif

#ifdef URCP_CMD_AIR_PROBE_GET_REPLY
extern uint16 receive_air_probe(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AIR_PROBE_GET
extern uint16 get_air_probe(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_WIRELESS_MODE_GET
extern uint16 get_wireless_mode(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
extern uint16 trap_get_wireless_mode(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_WIRELESS_MODE_GET_REPLY
extern uint16 receive_wireless_mode(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_WL_MAC_FILTER_SET
extern uint16 set_wl_mac_filter(cmd_hdr_t *cmd, uchar** app_data);
#endif


#ifdef URCP_CMD_AP_JOIN_SZ_SET_REPLY
extern void urcp_ap_join_sz_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state);
#endif

#ifdef URCP_CMD_AP_SZ_NAME_GET_REPLY
uint16 receive_ap_sz_name(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_SZ_NUM_GET_REPLY
uint16 receive_ap_sz_num(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif

#ifdef URCP_CMD_AP_MAC_FILTER_SET_REPLY
void urcp_ap_mac_filter_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state);
#endif
#ifdef URCP_CMD_MAC_FILTER_NUM_SET
uint16 set_mac_filter_num(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_MAC_FILTER_SET
uint16 set_mac_filter(cmd_hdr_t *cmd, uchar** app_data);
#endif

#if 0
#ifdef URCP_CMD_SZ_STATUS_SET
uint16 set_sz_status_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#endif
#ifdef URCP_CMD_SZ_NAME_SET
uint16 set_sz_name_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_SZ_SHARE_LIMIT
uint16 set_sz_share_limit_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_SZ_SSID_SET
uint16 set_sz_ssid_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_SZ_SSID_BC_SET
uint16 set_sz_ssid_bc_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_SZ_SSID_ISOLATE_SET
uint16 set_sz_ssid_isolate_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_SZ_VID_SET
uint16 set_sz_vid_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_SZ_AUTH_SET
uint16 set_sz_auth_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_SZ_WEP_SET
uint16 set_sz_wep_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_SZ_WPA_SET
uint16 set_sz_wpa_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_SZ_PSK_SET
uint16 set_sz_psk_config(cmd_hdr_t *cmd, uchar** app_data);
//void serivce_zone(void);
void wl_24G_init(void);
void wl_reinit(void);
#endif
#ifdef URCP_CMD_AP_BASIC_CONFIG_SET
void ap_basic_config(void);
#endif
#if 0
#ifdef URCP_CMD_SZ_DHCP_SET
uint16 set_sz_dhcp_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_SZ_START_IP_SET
uint16 set_sz_start_ip_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_SZ_END_IP_SET
uint16 set_sz_end_ip_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_SZ_GW_SET
uint16 set_sz_gw_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_SZ_SUBMASK_SET
uint16 set_sz_submask_config(cmd_hdr_t *cmd, uchar** app_data);
#endif
#endif
#ifdef URCP_CMD_AP_SZ_NAME_GET
uint16 get_ap_sz_name(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_SZ_NUM_GET
uint16 get_ap_sz_num(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
uint16 trap_get_ap_sz_num(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_CHANGE_CONF_GET
uint16 ac_change_conf(cmd_hdr_t *cmd, uchar **app_data);
#endif
#ifdef URCP_CMD_AC_CHANGE_CONF_SET
void urcp_ap_request_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state);
#endif
#ifdef URCP_CMD_AP_AFFIRM_GET
uint16 get_affirm(cmd_hdr_t *cmd, uchar **app, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_AFFIRM_GET_REPLY
uint16 receive_affirm(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_AUTH_PRE_GET
uint16 get_ap_auth_pre(cmd_hdr_t* cmd, uchar** app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AC_MODE_SET
uint16 set_ac_work_mode (cmd_hdr_t *cmd, uchar **app_data);
#endif
#ifdef URCP_CMD_AP_DHCP_EN_GET_REPLY
uint16 receive_ap_dhcp_en(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor);
#endif
#ifdef URCP_CMD_AP_ROAMING_TH_GET_REPLY
uint16 receive_ap_roaming_th(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor);
uint16 receive_ap_roaming_th_5g(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor);
#endif
#ifdef URCP_TRAP_CMD_UPPER_LIMIT
uint16 trap_set_upper_limit(cmd_hdr_t *cmd_buf,urcp_dev_t **dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_TRAP_CMD_UPPER_LIMIT_SYSLOG
uint16 trap_syslog_upper_limit(cmd_hdr_t *cmd_buf, urcp_dev_t **dev, urcp_infor_t *urcp_info);
#endif
#ifdef URCP_CMD_AP_LB_MAX_CLIENT_SET
uint16 set_lb_max_client_config(cmd_hdr_t *cmd, uchar** app_data);
#endif

#if AP_WL_POWER_AUTO
#ifdef URCP_CMD_AP_RA0_MAC_GET
uint16 get_ap_ra0_mac(cmd_hdr_t* cmd, uchar** app_data, urcp_infor_t* urcp_info);
uint16 trap_get_ap_ra0_mac(cmd_hdr_t* cmd, uchar** app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_RA0_MAC_GET_REPLY
uint16 receive_ap_ra0_mac(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor);
#endif
#ifdef URCP_TRAP_AP_RA0_MAC
uint16 trap_receive_ap_ra0_mac(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_RA0_SSID_GET
uint16 get_ap_ra0_ssid(cmd_hdr_t* cmd, uchar** app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_RA0_SSID_GET_REPLY
uint16 receive_ap_ra0_ssid(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor);
#endif
#ifdef URCP_CMD_AP_WL_POWER_SET
uint16 set_ap_wl_power(cmd_hdr_t *cmd, uchar** app_data);
#endif
#ifdef URCP_CMD_AP_WL_POWER_GET
uint16 get_ap_wl_power(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
uint16 trap_get_ap_wl_power(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_WL_POWER_GET_REPLY
uint16 receive_ap_wl_power(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_TRAP_AP_WL_POWER
uint16 trap_receive_ap_wl_power(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_SITE_SURVEY_SET
uint16 set_ap_site_survey(cmd_hdr_t *cmd, uchar** app_data);
#endif
#endif

#ifdef URCP_TRAP_DISCOVER
uint16 trap_receive_discover(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_TRAP_PRODUCT_MODEL
uint16 trap_receive_product_model(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_TRAP_SERIAL_NO
uint16 trap_receive_serial_no(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_TRAP_DEV_NAME
uint16 trap_receive_dev_name(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_TRAP_SW_VERSION
uint16 trap_receive_sw_version(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_TRAP_HW_VERSION
uint16 trap_receive_hw_version(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_TRAP_AP_SSID
uint16 trap_receive_ssid(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_TRAP_AP_SSID_5G
uint16 trap_receive_ssid_5g(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_TRAP_AP_CHANNEL
uint16 receive_ap_channel(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_TRAP_CURRENT_CLIENTS
uint16 trap_receive_clients(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_TRAP_CURRENT_CLIENTS_5G
uint16 trap_receive_clients_5g(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_TRAP_WIRELESS_MODE
uint16 trap_receive_wireless_mode(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_TRAP_WIRELESS_MODE_5G
uint16 trap_receive_wireless_mode_5g(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_TRAP_AP_DHCP_EN
uint16 trap_receive_ap_dhcp_en(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor);
#endif
#ifdef URCP_TRAP_AP_SZ_NUM
uint16 trap_receive_ap_sz_num(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_TRAP_AP_CHANNEL
uint16 trap_receive_ap_channel(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info);
uint16 trap_receive_ap_channel_new(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_TRAP_AP_CHANNEL_5G
uint16 trap_receive_ap_channel_new_5g(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_TRAP_CMD_STA_INFO
uint16 trap_set_sta_info(cmd_hdr_t *cmd_buf,urcp_dev_t **dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_TRAP_CMD_STA_INFO_SYSLOG
uint16 trap_syslog_sta_info(cmd_hdr_t *cmd_buf, urcp_dev_t **dev, urcp_infor_t *urcp_info);
#endif

#ifdef URCP_TRAP_CMD_STA_INFO_AUTO
uint16 trap_receive_clts_info(cmd_hdr_t *cmd_buf,urcp_dev_t **dev, urcp_infor_t* urcp_info);
#endif

#ifdef URCP_CMD_SZONE_UPDATA_GET
uint16 trap_get_szone_updata_ack(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_TRAP_CMD_SZONE_UPDATA_ACK
uint16 trap_receive_szone_updata_ack(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif

#ifdef URCP_CMD_AP_MF_NAME_GET
uint16 trap_get_ap_mf_name(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_TRAP_AP_MF_NAME
uint16 trap_receive_ap_mf_name(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_DISCONNECT_STA_SET
uint16 ap_disconnect_sta(cmd_hdr_t* cmd, uchar** app);
#endif
#ifdef URCP_TRAP_CMD_AP_STAT
uint16 ac_save_stat_call_back(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info);
uint16 ac_save_rate_call_back(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info);
#endif
#ifdef URCP_CMD_AP_SYS_INFO_GET
uint16 trap_get_ap_sys_info(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
uint16 trap_get_ap_rf(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
uint16 trap_get_ap_rf_rtx(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
#endif

#ifdef URCP_TRAP_AP_SYS_INFO
uint16 trap_receive_ap_sys_info( cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info);
uint16 trap_receive_ap_rf_info( cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info );
uint16 trap_receive_ap_rf_rtx_info( cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info );
#endif

#ifdef URCP_TRAP_AP_SYS_INFO_5G
uint16 trap_receive_ap_rf_rtx_info_5g( cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info );
uint16 trap_receive_ap_rf_info_5g( cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info );
#endif

#ifdef URCP_CMD_AP_ADD_MAC_BLACK
uint16 ap_add_mac_black(cmd_hdr_t *cmd, uchar** app_data);
void serivce_zone_mac(void);
#endif

#endif
