#include <mib.h>
#include "urcp.h"
#include "common.h"
#include "urcp_hello.h"
#include "app.h"
#include "urcp_cmd.h"
#if (CLIENT_HOST_NAME == FYES)
#include "hash.h"
#endif
#include "trap_sta.h"

urcp_trap_cmd_tb_t trap_cmd_tb[] = {
	{
		TRAP_DISCOVER,

		URCP_TRAP_LEVEL_INFO,

		NULL,

#ifdef URCP_TRAP_DISCOVER
		trap_receive_discover,
#else
		NULL,
#endif
	},

	{
		TRAP_PRODUCT_MODEL,

		URCP_TRAP_LEVEL_INFO,

		NULL,

#ifdef URCP_TRAP_PRODUCT_MODEL
		trap_receive_product_model,
#else
		NULL,
#endif
	},

	{
		TRAP_SERIAL_NO,

		URCP_TRAP_LEVEL_INFO,

		NULL,

#ifdef URCP_TRAP_SERIAL_NO
		trap_receive_serial_no,
#else
		NULL,
#endif
	},

	{
		TRAP_DEV_NAME,

		URCP_TRAP_LEVEL_INFO,

		NULL,

#ifdef URCP_TRAP_DEV_NAME
		trap_receive_dev_name,
#else
		NULL,
#endif
	},

	{
		TRAP_SW_VERSION,

		URCP_TRAP_LEVEL_INFO,

		NULL,

#ifdef URCP_TRAP_SW_VERSION
		trap_receive_sw_version,
#else
		NULL,
#endif
	},

	{
		TRAP_HW_VERSION,

		URCP_TRAP_LEVEL_INFO,

		NULL,

#ifdef URCP_TRAP_HW_VERSION
		trap_receive_hw_version,
#else
		NULL,
#endif
	},

	{
		TRAP_AP_SSID,

		URCP_TRAP_LEVEL_INFO,

		NULL,

#ifdef URCP_TRAP_AP_SSID
		trap_receive_ssid,
#else
		NULL,
#endif
	},
	{
	    TRAP_AP_SSID_5G,

	    URCP_TRAP_LEVEL_INFO,

	    NULL,

#ifdef URCP_TRAP_AP_SSID_5G
	    trap_receive_ssid_5g,
#else
	    NULL,
#endif
	},

	{
		TRAP_AP_CHANNEL,

		URCP_TRAP_LEVEL_INFO,

		NULL,

#ifdef URCP_TRAP_AP_CHANNEL
		trap_receive_ap_channel,
#else
		NULL,
#endif
	},
	
	{
		TRAP_AP_CLIENTS,

		URCP_TRAP_LEVEL_INFO,

		NULL,

#ifdef URCP_TRAP_CURRENT_CLIENTS
		trap_receive_clients,
#else
		NULL,
#endif
	},
	{
	    TRAP_AP_CLIENTS_5G,

	    URCP_TRAP_LEVEL_INFO,

	    NULL,

#ifdef URCP_TRAP_CURRENT_CLIENTS_5G
	    trap_receive_clients_5g,
#else
	    NULL,
#endif
	},

	{
		TRAP_WIRELESS_MODE,

		URCP_TRAP_LEVEL_INFO,

		NULL,

#ifdef URCP_TRAP_WIRELESS_MODE
		trap_receive_wireless_mode,
#else
		NULL,
#endif
	},
	{
	    TRAP_WIRELESS_MODE_5G,

	    URCP_TRAP_LEVEL_INFO,

	    NULL,

#ifdef URCP_TRAP_WIRELESS_MODE_5G
	    trap_receive_wireless_mode_5g,
#else
	    NULL,
#endif
	},

	{
		TRAP_AP_DHCP_EN,

		URCP_TRAP_LEVEL_INFO,

		NULL,

#ifdef URCP_TRAP_AP_DHCP_EN
		trap_receive_ap_dhcp_en,
#else
		NULL,
#endif
	},

	{
		TRAP_AP_SZ_NUM,

		URCP_TRAP_LEVEL_INFO,

		NULL,

#ifdef URCP_TRAP_AP_SZ_NUM
		trap_receive_ap_sz_num,
#else
		NULL,
#endif
	},

    {
        TRAP_AP_RA0_MAC,

		URCP_TRAP_LEVEL_INFO,

        NULL,
#ifdef URCP_TRAP_AP_RA0_MAC
        trap_receive_ap_ra0_mac,
#else
        NULL,
#endif
    },

    {
        TRAP_AP_WL_POWER,

		URCP_TRAP_LEVEL_INFO,

        NULL,

#ifdef URCP_TRAP_AP_WL_POWER
        trap_receive_ap_wl_power,
#else
        NULL,
#endif
    },

	{
		TRAP_UPPER_LIMIT,

		URCP_TRAP_LEVEL_INFO,

#ifdef URCP_TRAP_CMD_UPPER_LIMIT_SYSLOG
		trap_syslog_upper_limit,       //命名规范：trap_syslog_xxxx
#else
		NULL,
#endif

#ifdef URCP_TRAP_CMD_UPPER_LIMIT
		trap_set_upper_limit,          //命名规范：trap_set_xxxx
#else
		NULL,
#endif
	},

	{
		TRAP_STA_INFO,

		URCP_TRAP_LEVEL_INFO,

#ifdef URCP_TRAP_CMD_STA_INFO_SYSLOG
		trap_syslog_sta_info,       //命名规范：trap_syslog_xxxx
#else
		NULL,
#endif

#ifdef URCP_TRAP_CMD_STA_INFO
		trap_set_sta_info,          //命名规范：trap_set_xxxx
#else
		NULL,
#endif
	},
	{
		TRAP_STA_INFO_AUTO,

		URCP_TRAP_LEVEL_INFO,

		NULL,

#ifdef URCP_TRAP_CMD_STA_INFO_AUTO
		trap_receive_clts_info,
#else
		NULL,
#endif
	},
    {
		TRAP_SZONE_UPDATA,

		URCP_TRAP_LEVEL_INFO,

		NULL,

#ifdef URCP_TRAP_CMD_SZONE_UPDATA_ACK
		trap_receive_szone_updata_ack,
#else
		NULL,
#endif
	},
	{
		TRAP_AP_MF_NAME,

		URCP_TRAP_LEVEL_INFO,

		NULL,

#ifdef URCP_TRAP_AP_MF_NAME
		trap_receive_ap_mf_name,
#else
		NULL,
#endif
	},

	{
		TRAP_AP_CHANNEL_NEW,

		URCP_TRAP_LEVEL_INFO,

		NULL,

#ifdef URCP_TRAP_AP_CHANNEL
		trap_receive_ap_channel_new,
#else
		NULL,
#endif

	},
	{
	    TRAP_AP_CHANNEL_NEW_5G,

	    URCP_TRAP_LEVEL_INFO,

	    NULL,

#ifdef URCP_TRAP_AP_CHANNEL_5G
	    trap_receive_ap_channel_new_5g,
#else
	    NULL,
#endif

	},

	{
		TRAP_AP_MAC_TABLE,

		URCP_TRAP_LEVEL_INFO,

		NULL,

#ifdef URCP_STA_MAC_TABLE 
		receive_ap_mac_table,
#else
		NULL,
#endif
	},
#if defined(URCP_AC) && defined(URCP_TRAP_CMD_AP_STAT)
	{
		TRAP_CMD_AP_STAT,
		URCP_TRAP_LEVEL_INFO,
		NULL,
		ac_save_stat_call_back,
	},
	{
		TRAP_CMD_AP_RATE,
		URCP_TRAP_LEVEL_INFO,
		NULL,
		ac_save_rate_call_back,
	},
#endif	

	{	    
	    TRAP_AP_SYS_INFO,

	    URCP_TRAP_LEVEL_INFO,	
	    
	    NULL,

#ifdef URCP_TRAP_AP_SYS_INFO	
	    trap_receive_ap_sys_info,
#else	    
	    NULL,
#endif	
	},

	{	    
	    TRAP_AP_RF_INFO,

	    URCP_TRAP_LEVEL_INFO,	
	    
	    NULL,

#ifdef URCP_TRAP_AP_SYS_INFO	
	    trap_receive_ap_rf_info,
#else	    
	    NULL,
#endif	
	},
	{	    
	    TRAP_AP_RF_INFO_5G,

	    URCP_TRAP_LEVEL_INFO,	

	    NULL,

#ifdef URCP_TRAP_AP_SYS_INFO_5G	
	    trap_receive_ap_rf_info_5g,
#else	    
	    NULL,
#endif	
	},

	{	    
	    TRAP_AP_RF_RTX_INFO,

	    URCP_TRAP_LEVEL_INFO,	
	    
	    NULL,

#ifdef URCP_TRAP_AP_SYS_INFO	
	    trap_receive_ap_rf_rtx_info,
#else	    
	    NULL,
#endif	
	},
	{	    
	    TRAP_AP_RF_RTX_INFO_5G,

	    URCP_TRAP_LEVEL_INFO,	
	    
	    NULL,

#ifdef URCP_TRAP_AP_SYS_INFO_5G	
	    trap_receive_ap_rf_rtx_info_5g,
#else	    
	    NULL,
#endif	
	},

	{
	    TRAP_END, URCP_TRAP_LEVEL_DEBUG, NULL, NULL
	}
};

urcp_cmd_tb_t cmd_tb[] = {
    {
	CMD_HELLO, 0, 
#ifdef URCP_CMD_HELLO_GET
	get_hello,
#else
	NULL,
#endif
#ifdef URCP_CMD_HELLO_GET_REPLY
	receive_hello,
#else
	NULL,
#endif
	NULL, NULL
    },
    { 
	CMD_SERIAL_NO, 0, 
#ifdef URCP_CMD_SERIAL_NO_GET
	get_serial_no,
#else
        NULL,
#endif
#ifdef URCP_CMD_SERIAL_NO_GET_REPLY
	receive_serial_no,
#else
	NULL,
#endif
	NULL, NULL
    },
    { 
	CMD_DEV_NAME, 0, 
#ifdef URCP_CMD_DEV_NAME_GET
	get_dev_name, 
#else
	NULL,
#endif
#ifdef URCP_CMD_DEV_NAME_GET_REPLY
	receive_dev_name, 
#else
	NULL,
#endif
#ifdef URCP_CMD_DEV_NAME_SET
	set_dev_name_config, 
#else
	NULL,
#endif
	NULL
    },
    { 
	CMD_AP_DHCP_EN, 0, 
#ifdef URCP_CMD_AP_DHCP_EN_GET
	get_ap_dhcp_en, 
#else
	NULL,
#endif
#ifdef URCP_CMD_AP_DHCP_EN_GET_REPLY
	receive_ap_dhcp_en, 
#else
	NULL,
#endif
#ifdef URCP_CMD_AP_DHCP_EN_SET
	set_dhcp_en_config, NULL
#else
	NULL, NULL
#endif
    },
    { 
	CMD_UP_PORTS, 0, 
#ifdef URCP_CMD_UP_PORTS_GET
	get_up_ports, 
#else
	NULL,
#endif
#ifdef URCP_CMD_UP_PORTS_GET_REPLY
	receive_up_ports, 
#else
	NULL,
#endif
	NULL, NULL
    },
    { 
	CMD_SW_VERSION, 0, 
#ifdef URCP_CMD_SW_VERSION_GET
	get_sw_version, 
#else
	NULL,
#endif
#ifdef URCP_CMD_SW_VERSION_GET_REPLY
	receive_sw_version, 
#else
	NULL,
#endif
	NULL, NULL
    },
    { 
	CMD_HW_VERSION, 0, 
#ifdef URCP_CMD_HW_VERSION_GET
	get_hw_version, 
#else
	NULL,
#endif
#ifdef URCP_CMD_HW_VERSION_GET_REPLY
	receive_hw_version, 
#else
	NULL,
#endif
	NULL, NULL
    },
    { 
	CMD_PRODUCT_MODEL, 0, 
#ifdef URCP_CMD_PRODUCT_MODEL_GET
	get_product_model, 
#else
	NULL,
#endif
#ifdef URCP_CMD_PRODUCT_MODEL_GET_REPLY
	receive_product_model,
#else
	NULL,
#endif
	NULL, NULL
    },
    { 
	CMD_MIRROR_CONFIG, 1, 
#ifdef URCP_CMD_MIRROR_CONFIG_GET
	get_mirror_config, 
#else
	NULL,
#endif
#ifdef URCP_CMD_MIRROR_CONFIG_GET_REPLY
	receive_mirror_config, 
#else
	NULL,
#endif
#ifdef URCP_CMD_MIRROR_CONFIG_SET
	set_mirror_config, 
#else
	NULL,
#endif
	NULL
    },
    { 
	CMD_PASSWD_AUTH, 1, 
#ifdef URCP_CMD_PASSWD_AUTH_GET
	get_passwd_auth, 
#else
	NULL,
#endif
	NULL, NULL, NULL
    },
    { 
	CMD_AGGR_CONFIG, 1, 
#ifdef URCP_CMD_AGGR_CONFIG_GET
	get_aggr_config, 
#else
	NULL,
#endif
#ifdef URCP_CMD_AGGR_CONFIG_GET_REPLY
	receive_aggr_config, 
#else
	NULL,
#endif
#ifdef URCP_CMD_AGGR_CONFIG_SET
	set_aggr_config,
#else
	NULL,
#endif
	NULL
    },
    { 
	CMD_RSTP_LACP_CONFIG, 1, 
#ifdef URCP_CMD_RSTP_LACP_CONFIG_GET
	get_rstp_lacp_config, 
#else
	NULL,
#endif
#ifdef URCP_CMD_RSTP_LACP_CONFIG_GET_REPLY
	receive_rstp_lacp_config, 
#else
	NULL,
#endif
	NULL, NULL
    },
    { 
	CMD_VLAN_CONFIG, 1, 
#ifdef URCP_CMD_VLAN_CONFIG_GET
	get_vlan_config, 
#else
	NULL,
#endif
#ifdef URCP_CMD_VLAN_CONFIG_GET_REPLY
	receive_vlan_config, 
#else
	NULL,
#endif
#ifdef URCP_CMD_VLAN_CONFIG_SET
	set_vlan_config,
#else
        NULL,
#endif
        NULL
    },
    { 
	CMD_QVLAN_PCONF_CONFIG, 1, 
#ifdef URCP_CMD_QVLAN_PCONF_CONFIG_GET
	get_qvlan_pconf_config, 
#else
        NULL,
#endif
#ifdef URCP_CMD_QVLAN_PCONF_CONFIG_GET_REPLY
	receive_qvlan_pconf_config, 
#else
        NULL,
#endif
	NULL, NULL
    },
    { 
	CMD_PORT_CONFIG, 1, 
#ifdef URCP_CMD_PORT_CONFIG_GET
	get_port_conf_config, 
#else
        NULL,
#endif
#ifdef URCP_CMD_PORT_CONFIG_GET_REPLY
	receive_port_conf_config, 
#else
        NULL,
#endif
#ifdef URCP_CMD_PORT_CONFIG_SET
	set_port_conf_config, NULL/*端口设置生效，可能使端口暂时断开链接*/
//	set_port_conf_config, shm_set_port_configure()/*端口设置生效，可能使端口暂时断开链接*/
#else
	NULL, NULL
#endif
    },
    { 
	CMD_QOS_RATE_LIMIT_CONFIG, 1, 
#ifdef URCP_CMD_QOS_RATE_LIMIT_CONFIG_GET
	get_qos_rate_limit_config, 
#else
	NULL,
#endif
#ifdef URCP_CMD_QOS_RATE_LIMIT_CONFIG_GET_REPLY
	receive_qos_rate_limit_config,
#else
	NULL, 
#endif
#ifdef URCP_CMD_QOS_RATE_LIMIT_CONFIG_SET
	set_qos_rate_limit_config,
#else
	NULL,
#endif
	NULL
    },
    { 
	CMD_STORM_RATE_LIMIT_CONFIG, 1, 
#ifdef URCP_CMD_STORM_RATE_LIMIT_CONFIG_GET
	get_storm_rate_limit_config, 
#else
	NULL, 
#endif
#ifdef URCP_CMD_STORM_RATE_LIMIT_CONFIG_GET_REPLY
	receive_storm_rate_limit_config, 
#else
	NULL, 
#endif
#ifdef URCP_CMD_STORM_RATE_LIMIT_CONFIG_SET
	set_storm_rate_limit_config,
#else
	NULL, 
#endif
	NULL
    },
    { 
	CMD_REBOOT_CONFIG, 0, NULL, NULL, 
#ifdef URCP_CMD_REBOOT_CONFIG_SET
	build_cmd_alone_reply_cmd, sys_reboot
#else
	NULL, NULL
#endif
    },
    { 
	CMD_RESET_CONFIG, 0, NULL, NULL, 
#ifdef URCP_CMD_RESET_CONFIG_SET
	build_cmd_alone_reply_cmd, sys_reset
#else
	NULL, NULL
#endif
    },
    { 
	CMD_SAFEBIND_CONFIG, 0, NULL, NULL, 
#ifdef URCP_CMD_SAFEBIND_CONFI_SET
	build_cmd_alone_reply_cmd, set_safebind_config
#else
	NULL, NULL
#endif
    },
    { 
	CMD_SYS_CONF_CONFIG, 1, 
#ifdef URCP_CMD_SYS_CONF_CONFIG_GET
	get_sys_conf_config, 
#else
	NULL,
#endif
#ifdef URCP_CMD_SYS_CONF_CONFIG_GET_REPLY
	receive_sys_conf_config,
#else
	NULL,
#endif
#ifdef URCP_CMD_SYS_CONF_CONFIG_SET
        set_sys_conf_config,
#else
	NULL, 
#endif
	NULL
    },
    { 
	CMD_IP_FILTER_CONFIG, 1, 
#ifdef URCP_CMD_IP_FILTER_CONFIG_GET
	get_ip_filter_config, 
#else
	NULL,
#endif
#ifdef URCP_CMD_IP_FILTER_CONFIG_GET_REPLY
	receive_ip_filter_config, 
#else
	NULL,
#endif
	NULL, NULL
    },
    { 
	CMD_SYS_INFO_CONFIG, 1, 
#ifdef URCP_CMD_SYS_INFO_CONFIG_GET
	get_sys_info_config, 
#else
	NULL,
#endif
#ifdef URCP_CMD_SYS_INFO_CONFIG_GET_REPLY
	receive_sys_info_config, 
#else
	NULL,
#endif
	NULL, NULL
    },
    { 
	CMD_PASSWORD_CONFIG, 0, NULL, NULL,
#ifdef URCP_CMD_PASSWORD_CONFIG_SET
	set_password_config,
#else
	NULL, 
#endif
	NULL
    },
    { 
	CMD_MIRROR_CAN, 0, NULL, NULL, 
#ifdef URCP_CMD_MIRROR_CAN_SET
	//set_mirror_source_mask_contrl,
	NULL,
#else
	NULL, 
#endif
	NULL
    },
    { 
	CMD_DISCOVER, 0, 
#ifdef URCP_CMD_DISCOVER_GET
	get_discover,
#else
	NULL, 
#endif
#ifdef URCP_CMD_DISCOVER_GET_REPLY
	receive_discover,
#else
	NULL, 
#endif
	NULL, NULL
    },
    { 
	CMD_AP_AUTH, 0, NULL, NULL, NULL, NULL
    },
    { 
	CMD_AP_AUTH_REQ, 0, 
#ifdef URCP_CMD_AP_AUTH_REQ_GET
	get_ap_auth_reqest,
#else
	NULL,
#endif
#ifdef URCP_CMD_AP_AUTH_REQ_GET_REPLY
	receive_ap_auth,
#else
	NULL,
#endif
	NULL, NULL
    },
    { 
	CMD_AP_UPDATE_FIRMWARE, 0,
#ifdef URCP_CMD_AP_UPDATE_FIRMWARE_GET
	get_ap_update_fireware,
#else
	NULL,
#endif
#ifdef URCP_CMD_AP_UPDATE_FIRMWARE_GET_REPLY
	receive_ap_update_fireware,
#else
        NULL, 
#endif
	NULL, NULL
    },
    { 
	CMD_AP_UPDATE_PROFILE, 0, 
#ifdef URCP_CMD_AP_UPDATE_PROFILE_GET
	get_ap_update_profile,
#else
	NULL,
#endif
#ifdef URCP_CMD_AP_UPDATE_PROFILE_GET_REPLY
	receive_ap_update_profile,
#else
	NULL, 
#endif
	NULL, NULL
    },
    { 
	CMD_AP_SSID, 0, 
#ifdef URCP_CMD_AP_SSID_GET
	get_ap_ssid, 
#else
	NULL, 
#endif
#ifdef URCP_CMD_AP_SSID_GET_REPLY
	receive_ssid, 
#else
	NULL, 
#endif
	NULL, NULL
    },
    { 
	CMD_AP_CHANNEL, 0, 
#ifdef URCP_CMD_AP_CHANNEL_GET
	get_ap_channel, 
#else
	NULL, 
#endif
#ifdef URCP_CMD_AP_CHANNEL_GET_REPLY
	receive_ap_channel,
#else
	NULL, 
#endif
#ifdef URCP_CMD_AP_CHANNEL_SET
	set_ap_channel_config, NULL
#else
	NULL, NULL
#endif
	},
    { 
	CMD_AP_CHANNEL_5G, 0, 
#if 0
	get_ap_channel, 
#else
	NULL, 
#endif
#if 0
	receive_ap_channel,
#else
	NULL, 
#endif
#ifdef URCP_CMD_AP_CHANNEL_SET_5G
	set_ap_channel_config_5g, NULL
#else
	    NULL, NULL
#endif
    },

    { 
	CMD_CURRENT_CLIENTS, 0, 
#ifdef URCP_CMD_CURRENT_CLIENTS_GET
	get_ap_clients, 
#else
	NULL, 
#endif
#ifdef URCP_CMD_CURRENT_CLIENTS_GET_REPLY
	receive_clients, 
#else
	NULL, 
#endif
	NULL, NULL
    },
    { 
	CMD_AP_WORK_MODE, 1, 
#ifdef URCP_CMD_AP_WORK_MODE_GET
	get_ap_work_mode, 
#else
	NULL,
#endif
#ifdef URCP_CMD_AP_WORK_MODE_GET_REPLY
	receive_ap_work_mode, 
#else
	NULL,
#endif
#ifdef URCP_CMD_AP_WORK_MODE_SET
	set_work_mode_config, 
#else
	NULL,
#endif
#ifdef URCP_CMD_AP_BASIC_CONFIG_SET
    ap_basic_config
#else
	NULL
#endif
    },
    { 
	CMD_IP_ADDR, 1, 
#ifdef URCP_CMD_IP_ADDR_GET
	get_ip_addr, 
#else
	NULL,
#endif
#ifdef URCP_CMD_IP_ADDR_GET_REPLY
	receive_ip_addr, 
#else
	NULL,
#endif
#ifdef URCP_CMD_IP_ADDR_SET
	set_ip_addr_config, 
#else
	NULL,
#endif
	NULL
    },
    { 
	CMD_NETMASK, 1, 
#ifdef URCP_CMD_NETMASK_GET
	get_netmask_addr, 
#else
	NULL,
#endif
#ifdef URCP_CMD_NETMASK_GET_REPLY
	receive_netmask, 
#else
	NULL,
#endif
#ifdef URCP_CMD_NETMASK_SET
	set_netmask_config, 
#else
	NULL,
#endif
	NULL
    },
    { 
	CMD_GATEWAY, 1, 
#ifdef URCP_CMD_GATEWAY_GET
	get_gateway_addr, 
#else
	NULL,
#endif
#ifdef URCP_CMD_GATEWAY_GET_REPLY
	receive_gateway, 
#else
	NULL,
#endif
#ifdef URCP_CMD_GATEWAY_SET
	set_gateway_config, 
#else
	NULL,
#endif
	NULL
    },
    { 
	CMD_AP_USER_NAME, 1, 
#ifdef URCP_CMD_AP_USER_NAME_GET
	get_ap_user_name, 
#else
	NULL,
#endif
#ifdef URCP_CMD_AP_USER_NAME_GET_REPLY
	receive_ap_user_name, 
#else
	NULL,
#endif
#ifdef URCP_CMD_AP_USER_NAME_SET
	set_user_name_config, 
#else
	NULL,
#endif
    NULL
    },
    { 
	CMD_AP_PASSWORD, 1, 
#ifdef URCP_CMD_AP_PASSWORD_GET
	NULL,//get_ap_password, 
#else
	NULL,
#endif
#ifdef URCP_CMD_AP_PASSWORD_GET_REPLY
	NULL,//receive_ap_password, 
#else
	NULL,
#endif
#ifdef URCP_CMD_AP_PASSWORD_SET
	set_password_config, 
#else
	NULL,
#endif
	NULL
    },
    { 
	CMD_AP_ROAMING_TH, 0, 
#ifdef URCP_CMD_AP_ROAMING_TH_GET
	get_ap_roaming_th,
#else
	NULL,
#endif
#ifdef URCP_CMD_AP_ROAMING_TH_GET_REPLY
	receive_ap_roaming_th,
#else
	NULL,
#endif
#ifdef URCP_CMD_AP_ROAMING_TH_SET
	set_ap_roaming_th, 
#else
	NULL,
#endif
	NULL
    },
    { 
	CMD_AP_ROAMING_TH_5G, 0, 
#ifdef URCP_CMD_AP_ROAMING_TH_5G_GET
	get_ap_roaming_th_5g,
#else
	NULL,
#endif
#ifdef URCP_CMD_AP_ROAMING_TH_GET_REPLY
	receive_ap_roaming_th_5g,
#else
	NULL,
#endif
#ifdef URCP_CMD_AP_ROAMING_TH_5G_SET
	set_ap_roaming_th_5g, 
#else
	NULL,
#endif
	NULL
    },
    { 
	CMD_AP_MAX_CLIENT, 1, 
#ifdef URCP_CMD_AP_MAX_CLIENT_GET
	get_ap_max_client, 
#else
	NULL,
#endif
#ifdef URCP_CMD_AP_MAX_CLIENT_GET_REPLY
	receive_ap_max_client, 
#else
	NULL,
#endif
#ifdef URCP_CMD_AP_MAX_CLIENT_SET
	set_max_client_config, 
#else
	NULL,
#endif
	NULL
    },
    { 
	CMD_AP_MAX_CLIENT_NEW, 1, 
#ifdef URCP_CMD_AP_MAX_CLIENT_GET
	get_ap_max_client_new, 
#else
	NULL,
#endif
#ifdef URCP_CMD_AP_MAX_CLIENT_GET_REPLY
	receive_ap_max_client_new, 
#else
	NULL,
#endif
#ifdef URCP_CMD_AP_MAX_CLIENT_SET
	set_max_client_config_new, 
#else
	NULL,
#endif
	NULL
    },

    { 
	CMD_AP_EFFECT_CONF, 1, 
#ifdef URCP_CMD_AP_EFFECT_CONF_GET
	get_ap_effect_conf, 
#else
	NULL,
#endif
#ifdef URCP_CMD_AP_EFFECT_CONF_GET_REPLY
	receive_ap_effect_conf, 
#else
	NULL,
#endif
#ifdef URCP_CMD_AP_EFFECT_CONF_SET
	set_effect_conf_config, 
#else
	NULL,
#endif
	NULL
    },
    {
	CMD_WIRELESS_CLT, 0,
#ifdef URCP_CMD_WIRELESS_CLT_GET
	get_wl_clt,
#else
        NULL,
#endif
#ifdef URCP_CMD_WIRELESS_CLT_GET_REPLY
	receive_wl_clt,
#else
	NULL,
#endif
	NULL, NULL
    },

    {
	CMD_AIR_PROBE, 0,
#ifdef URCP_CMD_AIR_PROBE_GET
	get_air_probe,
#else
	NULL,
#endif
#ifdef URCP_CMD_AIR_PROBE_GET_REPLY
	receive_air_probe,
#else
	NULL, 
#endif
	NULL, NULL
    },
    {
	CMD_WIRELESS_MODE, 0,
#ifdef URCP_CMD_WIRELESS_MODE_GET
	get_wireless_mode,
#else
	NULL,
#endif
#ifdef URCP_CMD_WIRELESS_MODE_GET_REPLY
	receive_wireless_mode,
#else
	NULL, 
#endif
	NULL, NULL
    },
    {	
	CMD_WL_MAC_FILTER, 0,
	NULL, NULL, 
#ifdef URCP_CMD_WL_MAC_FILTER_SET
	set_wl_mac_filter,
#else
	NULL,
#endif
	NULL
    },
    {
	CMD_CONFIG_TEMPLATE, 1,
	NULL,
	NULL,
#ifdef URCP_CMD_CONFIG_TEMPLATE_SET 
	set_ap_config_template, NULL   //service_zone_for_conf_temp
#else
	NULL, NULL
#endif       
    },
#if 0
    { 
	CMD_SZ_STATUS, 1, 
	NULL,
	NULL,
#ifdef URCP_CMD_SZ_STATUS_SET
	set_sz_status_config, 
#else
	NULL,
#endif
	NULL
    },
#endif
    {
	CMD_MAC_FILTER_NUM, 1, 
	NULL,
	NULL,
#ifdef URCP_CMD_MAC_FILTER_NUM_SET
	set_mac_filter_num,
#else
	NULL,
#endif
	NULL
    },
    {
	CMD_MAC_FILTER, 1, 
	NULL,
	NULL,
#ifdef URCP_CMD_MAC_FILTER_SET
	set_mac_filter, 
#ifdef URCP_CMD_AP_ADD_MAC_BLACK 
	serivce_zone_mac
#else

#if defined(WIRELESS_5G) && (WIRELESS_5G==FYES)
	wl_reinit
#else
//	serivce_zone
	wl_24G_init
#endif

#endif
#else
	NULL, NULL
#endif
    },
    {
	CMD_SZ_NAME, 0, 
	NULL,
	NULL,
#ifdef URCP_CMD_SZ_NAME_SET
	set_sz_name_config, 
#else
	NULL,
#endif
	NULL
    },
    {
	CMD_SZ_SHARE_LIMIT, 0, 
	NULL,
	NULL,
#ifdef URCP_CMD_SZ_SHARE_LIMIT
	set_sz_share_limit_config, 
#else
	NULL,
#endif
	NULL
    },  
    { 
	CMD_SZ_SSID, 0, 
	NULL,
	NULL,
#ifdef URCP_CMD_SZ_SSID_SET
	set_sz_ssid_config, 
#else
	NULL,
#endif
	NULL
    },
    { 
	CMD_SZ_SSID_BC, 0, 
	NULL,
	NULL,
#ifdef URCP_CMD_SZ_SSID_BC_SET
	set_sz_ssid_bc_config, 
#else
	NULL,
#endif
	NULL
    },
    { 
	CMD_SZ_SSID_ISOLATE, 0,
	NULL,
	NULL,
#ifdef URCP_CMD_SZ_SSID_ISOLATE_SET
	set_sz_ssid_isolate_config,
#else
	NULL,
#endif
	NULL
    },
    { 
	CMD_SZ_VID, 0, 
	NULL,
	NULL,
#ifdef URCP_CMD_SZ_VID_SET
	set_sz_vid_config, 
#else
	NULL,
#endif
	NULL
    },
    { 
	CMD_SZ_AUTH, 0, 
	NULL,
	NULL,
#ifdef URCP_CMD_SZ_AUTH_SET
	set_sz_auth_config, 
#else
	NULL,
#endif
	NULL
    },
    { 
	CMD_SZ_WEP, 0, 
	NULL,
	NULL,
#ifdef URCP_CMD_SZ_WEP_SET
	set_sz_wep_config, 
#else
	NULL,
#endif
	NULL
    },
    { 
	CMD_SZ_WPA, 0, 
	NULL,
	NULL,
#ifdef URCP_CMD_SZ_WPA_SET
	set_sz_wpa_config, 
#else
	NULL,
#endif
	NULL
    },
    { 
	CMD_SZ_PSK, 0, 
	NULL,
	NULL,
#ifdef URCP_CMD_SZ_PSK_SET
	set_sz_psk_config,
#if defined(WIRELESS_5G) && (WIRELESS_5G==FYES)
	wl_reinit
#else
//	serivce_zone
	wl_24G_init
#endif
#else
	NULL, NULL
#endif
    },
#if 0
    { 
	CMD_SZ_DHCP, 1, 
	NULL,
	NULL,
#ifdef URCP_CMD_SZ_DHCP_SET
	set_sz_dhcp_config, 
#else
	NULL,
#endif
	NULL
    },
    { 
	CMD_SZ_START_IP, 1, 
	NULL,
	NULL,
#ifdef URCP_CMD_SZ_START_IP_SET
	set_sz_start_ip_config, 
#else
	NULL,
#endif
	NULL
    },
    { 
	CMD_SZ_END_IP, 1, 
	NULL,
	NULL,
#ifdef URCP_CMD_SZ_END_IP_SET
	set_sz_end_ip_config, 
#else
	NULL,
#endif
	NULL
    },
    { 
	CMD_SZ_GW, 1, 
	NULL,
	NULL,
#ifdef URCP_CMD_SZ_GW_SET
	set_sz_gw_config, 
#else
	NULL,
#endif
	NULL
    },
    { 
	CMD_SZ_SUBMASK, 1, 
	NULL,
	NULL,
#ifdef URCP_CMD_SZ_SUBMASK_SET
	set_sz_submask_config, 
#else
	NULL,
#endif
	NULL
    },
    { 
	CMD_AP_SZ_NAME, 0, 
#ifdef URCP_CMD_AP_SZ_NAME_GET
	get_ap_sz_name, 
#else
	NULL, 
#endif
#ifdef URCP_CMD_AP_SZ_NAME_GET_REPLY
	receive_ap_sz_name, 
#else
	NULL, 
#endif
	NULL, NULL
    },
#endif
    { 
	CMD_AP_SZ_NUM, 0, 
#ifdef URCP_CMD_AP_SZ_NUM_GET
	get_ap_sz_num, 
#else
	NULL, 
#endif
#ifdef URCP_CMD_AP_SZ_NUM_GET_REPLY
	receive_ap_sz_num, 
#else
	NULL, 
#endif
	NULL, NULL
    },
    {
	REQUEST_CHANGE_CONF, 0, NULL, NULL,
#ifdef URCP_CMD_CHANGE_CONF_GET
	ac_change_conf,
#else
	NULL,
#endif
	NULL
    },
    {
	REQUEST_FAT_TO_FIT, 0, NULL, NULL,
#ifdef URCP_CMD_CHANGE_CONF_GET
	ac_change_conf,
#else
	NULL,
#endif
	NULL
    },
    { 
	CMD_AP_AFFIRM, 0, 
#ifdef URCP_CMD_AP_AFFIRM_GET
	get_affirm,
#else
	NULL, 
#endif
#ifdef URCP_CMD_AP_AFFIRM_GET_REPLY
	receive_affirm,
#else
	NULL, 
#endif
	NULL, NULL
    },
    {
	CMD_AP_AUTH_PRE, 0, 
#ifdef URCP_CMD_AP_AUTH_PRE_GET
	get_ap_auth_pre,
#else
	NULL,
#endif
	NULL,
	NULL, NULL
    },
    {
	CMD_SETACMODE_CONFIG, 0, 
	NULL,
	NULL,
#ifdef URCP_CMD_AC_MODE_SET
    set_ac_work_mode,
#else
	NULL,
#endif
    NULL
    },
    {
	CMD_AP_LB_MAX_CLIENT, 0, 
	NULL,
	NULL,
#ifdef URCP_CMD_AP_LB_MAX_CLIENT_SET
    set_lb_max_client_config,
#else
	NULL,
#endif
    NULL
    },
    {
	CMD_AP_RA0_MAC, 0, 
#ifdef URCP_CMD_AP_RA0_MAC_GET
	get_ap_ra0_mac,
#else
    NULL,
#endif
#ifdef URCP_CMD_AP_RA0_MAC_GET_REPLY
    receive_ap_ra0_mac,
#else
	NULL,
#endif
	NULL,
    NULL
    },
    {
	CMD_AP_RA0_SSID, 0, 
#ifdef URCP_CMD_AP_RA0_SSID_GET
	get_ap_ra0_ssid,
#else
    NULL,
#endif
#ifdef URCP_CMD_AP_RA0_SSID_GET_REPLY
    receive_ap_ra0_ssid,
#else
	NULL,
#endif
	NULL,
    NULL
    },
    {
	CMD_AP_WL_POWER, 0, 
#ifdef URCP_CMD_AP_WL_POWER_GET
    get_ap_wl_power,
#else
    NULL,
#endif
#ifdef URCP_CMD_AP_WL_POWER_GET_REPLY
    receive_ap_wl_power,
#else
	NULL,
#endif
#ifdef URCP_CMD_AP_WL_POWER_SET
	set_ap_wl_power,
#else
	NULL,
#endif
    NULL
    },
    {
	CMD_AP_SITE_SURVEY, 0, 
    NULL,
	NULL,
#ifdef URCP_CMD_AP_SITE_SURVEY_SET
	set_ap_site_survey,
#else
	NULL,
#endif
    NULL
    },
#ifdef URCP_CMD_AP_DISCONNECT_STA_SET
    {
	CMD_SET_DISASSOC, 0, 
	NULL,
	NULL,
	ap_disconnect_sta,
	NULL
    },
#endif
#ifdef URCP_CMD_AP_ADD_MAC_BLACK
    {
	CMD_AP_ADD_MAC_BLACK, 0, 
	NULL,
	NULL,
	ap_add_mac_black, serivce_zone_mac
    },
#endif
    {
	CMD_END, 0, NULL, NULL, NULL, NULL
    }
};
