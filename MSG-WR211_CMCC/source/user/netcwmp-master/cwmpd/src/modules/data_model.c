#include "cwmp/model.h"
#include "data_model.h"
#include "cwmp_module.h"
#include "InternetGatewayDevice/InternetGatewayDevice.c"

model_func_t ModelFunction[] =
{
    /*InternetGatewayDevice.DeviceInfo*/
    {"cpe_get_igd_di_manufacturer", cpe_get_igd_di_manufacturer},
    {"cpe_get_igd_di_manufactureroui", cpe_get_igd_di_manufactureroui},
    {"cpe_get_igd_di_productclass", cpe_get_igd_di_productclass},
    {"cpe_get_igd_di_serialnumber", cpe_get_igd_di_serialnumber},
    {"cpe_get_igd_di_specversion", cpe_get_igd_di_specversion},
    {"cpe_get_igd_di_hardwareversion", cpe_get_igd_di_hardwareversion},
    {"cpe_get_igd_di_softwareversion", cpe_get_igd_di_softwareversion},
    {"cpe_get_igd_di_provisioningcode", cpe_get_igd_di_provisioningcode},
    {"cpe_get_igd_di_sysuptime", cpe_get_igd_di_sysuptime},
    {"cpe_get_igd_di_macaddress", cpe_get_igd_di_macaddress},
    /*InternetGatewayDevice.DeviceInfo*/

    /*InternetGatewayDevice.ManagementServer*/
    {"cpe_get_igd_ms_username", cpe_get_igd_ms_username},
    {"cpe_get_igd_ms_password", cpe_get_igd_ms_password},
    {"cpe_set_igd_ms_username", cpe_set_igd_ms_username},
    {"cpe_set_igd_ms_password", cpe_set_igd_ms_password},

    {"cpe_get_igd_ms_connectionrequesturl", cpe_get_igd_ms_connectionrequesturl},
    {"cpe_get_igd_ms_connectionrequestusername", cpe_get_igd_ms_connectionrequestusername},
    {"cpe_get_igd_ms_connectionrequestpassword", cpe_get_igd_ms_connectionrequestpassword},
    {"cpe_set_igd_ms_connectionrequestusername", cpe_set_igd_ms_connectionrequestusername},
    {"cpe_set_igd_ms_connectionrequestpassword", cpe_set_igd_ms_connectionrequestpassword},

    {"cpe_set_igd_ms_url", cpe_set_igd_ms_url},
    {"cpe_get_igd_ms_url",cpe_get_igd_ms_url},

    {"cpe_get_igd_ms_periodicinforminterval", cpe_get_igd_ms_periodicinforminterval},
    {"cpe_set_igd_ms_periodicinforminterval", cpe_set_igd_ms_periodicinforminterval},
    {"cpe_get_igd_ms_periodicinformenable", cpe_get_igd_ms_periodicinformenable},
    {"cpe_set_igd_ms_periodicinformenable", cpe_set_igd_ms_periodicinformenable},

    {"cpe_set_igd_ms_stun_url", cpe_set_igd_ms_stun_url},
    {"cpe_set_igd_ms_stun_periodicinterval", cpe_set_igd_ms_stun_periodicinterval},
    {"cpe_get_igd_ms_stun_url", cpe_get_igd_ms_stun_url},
    {"cpe_get_igd_ms_stun_periodicinterval", cpe_get_igd_ms_stun_periodicinterval},

    {"cpe_set_igd_ms_stun_username", cpe_set_igd_ms_stun_username},
    {"cpe_set_igd_ms_stun_password", cpe_set_igd_ms_stun_password},
    {"cpe_get_igd_ms_stun_username", cpe_get_igd_ms_stun_username},
    {"cpe_get_igd_ms_stun_password", cpe_get_igd_ms_stun_password},
    /*InternetGatewayDevice.ManagementServer*/


	{"cpe_refresh_igd_wandevice", cpe_refresh_igd_wandevice},
	{"cpe_refresh_igd_landevice", cpe_refresh_igd_landevice},
	{"cpe_refresh_igd_wlanconfiguration", cpe_refresh_igd_wlanconfiguration},
	{"cpe_refresh_igd_admin_config", cpe_refresh_igd_admin_config},
    {"cpe_refresh_igd_wanconnectiondevice", cpe_refresh_igd_wanconnectiondevice},
    {"cpe_refresh_igd_wanipconnection", cpe_refresh_igd_wanipconnection},
	{"cpe_refresh_igd_wireless_mbasecfg", cpe_refresh_igd_wireless_mbasecfg},

    /* InternetGatewayDevice.WhiteList*/
    {"cpe_refresh_igd_whitelist", cpe_refresh_igd_whitelist},
    {"cpe_get_igd_whitelist", cpe_get_igd_whitelist},
    {"cpe_set_igd_whitelist", cpe_set_igd_whitelist},
    {"cpe_set_igd_add_whitelist", cpe_set_igd_add_whitelist},
    /* InternetGatewayDevice.WhiteList*/

    /*用户下线*/    
    {"cpe_get_igd_wifidog_downusers",cpe_get_igd_wifidog_downusers},
    {"cpe_set_igd_wifidog_downusers",cpe_set_igd_wifidog_downusers},

#if (UTT_KNOWIFI == FYES)
    /* knowifi */
    {"cpe_get_igd_knowifi_enable",cpe_get_igd_knowifi_enable},
    {"cpe_set_igd_knowifi_enable",cpe_set_igd_knowifi_enable},
    {"cpe_detail_igd_knowifi_users",cpe_detail_igd_knowifi_users},
    {"cpe_set_igd_updatedomain_load",cpe_set_igd_updatedomain_load},
    {"cpe_set_igd_upusers_load",cpe_set_igd_upusers_load},
    {"cpe_set_igd_knowifi_delDomain",cpe_set_igd_knowifi_delDomain},
    {"cpe_set_igd_knowifi_forbiddenUsers_num",cpe_set_igd_knowifi_forbiddenUsers_num},
    {"cpe_set_igd_knowifi_forbiddenUsers",cpe_set_igd_knowifi_forbiddenUsers},
    {"cpe_refresh_igd_config_knowifi_updateDomain",cpe_refresh_igd_config_knowifi_updateDomain},
    {"cpe_refresh_igd_config_knowifi_upusers",cpe_refresh_igd_config_knowifi_upusers},
#endif

    /* WANDevice */
    {"wan_get_igd_wd_ip_connectionstatus", wan_get_igd_wd_ip_connectionstatus},
    {"wan_set_igd_wd_ip_connectionstatus", wan_set_igd_wd_ip_connectionstatus},
    {"wan_get_igd_wd_ip_externalipaddress", wan_get_igd_wd_ip_externalipaddress},
    {"wan_set_igd_wd_ip_externalipaddress", wan_set_igd_wd_ip_externalipaddress},
    {"wan_get_igd_wd_ip_macaddress", wan_get_igd_wd_ip_macaddress},
    {"wan_set_igd_wd_ip_macaddress", wan_set_igd_wd_ip_macaddress},

    /* LANDevice */
    {"cpe_refresh_igd_wlanconfiguration_ssid",cpe_refresh_igd_wlanconfiguration_ssid},
    {"cpe_get_igd_lan_wlan_name", cpe_get_igd_lan_wlan_name},
    {"cpe_get_igd_lan_wlan_ssidNuber",cpe_get_igd_lan_wlan_ssidNuber},
    {"cpe_set_igd_lan_wlan_channel", cpe_set_igd_lan_wlan_channel},
    {"cpe_get_igd_lan_wlan_channel", cpe_get_igd_lan_wlan_channel},
    {"cpe_set_igd_lan_wlan_ssid", cpe_set_igd_lan_wlan_ssid},
    {"cpe_get_igd_lan_wlan_ssid", cpe_get_igd_lan_wlan_ssid},
    {"cpe_get_igd_lan_wlan_enable", cpe_get_igd_lan_wlan_enable},
    {"cpe_set_igd_lan_wlan_enable", cpe_set_igd_lan_wlan_enable},
    {"cpe_get_igd_lan_wlan_charset", cpe_get_igd_lan_wlan_charset},
    {"cpe_set_igd_lan_wlan_charset", cpe_set_igd_lan_wlan_charset},
    {"cpe_get_igd_lan_wlan_basicencryptionmodes", cpe_get_igd_lan_wlan_basicencryptionmodes},
    {"cpe_set_igd_lan_wlan_basicencryptionmodes", cpe_set_igd_lan_wlan_basicencryptionmodes},
    {"cpe_get_igd_lan_wlan_basicauthenticationmode", cpe_get_igd_lan_wlan_basicauthenticationmode},
    {"cpe_set_igd_lan_wlan_basicauthenticationmode", cpe_set_igd_lan_wlan_basicauthenticationmode},
    {"cpe_get_igd_lan_wlan_wpakey", cpe_get_igd_lan_wlan_wpakey},
    {"cpe_set_igd_lan_wlan_wpakey", cpe_set_igd_lan_wlan_wpakey},
    {"cpe_get_igd_lan_wlan_wpa_timeout", cpe_get_igd_lan_wlan_wpa_timeout},
    {"cpe_set_igd_lan_wlan_wpa_timeout", cpe_set_igd_lan_wlan_wpa_timeout},
#if (WECHAT_CWMP == FYES)
    {"cpe_set_igd_utt_wirelessoptimize", cpe_set_igd_utt_wirelessoptimize},
    /*wifi 定时*/
    {"cpe_get_igd_lan_wlan_timeOffEnable", cpe_get_igd_lan_wlan_timeOffEnable},
    {"cpe_set_igd_lan_wlan_timeOffEnable", cpe_set_igd_lan_wlan_timeOffEnable},
    {"cpe_get_igd_lan_wlan_effectiveDate", cpe_get_igd_lan_wlan_effectiveDate},
    {"cpe_set_igd_lan_wlan_effectiveDate", cpe_set_igd_lan_wlan_effectiveDate},
    {"cpe_get_igd_lan_wlan_weekdayStartTime", cpe_get_igd_lan_wlan_weekdayStartTime},
    {"cpe_set_igd_lan_wlan_weekdayStartTime", cpe_set_igd_lan_wlan_weekdayStartTime},
    {"cpe_get_igd_lan_wlan_weekdayEndTime", cpe_get_igd_lan_wlan_weekdayEndTime},
    {"cpe_set_igd_lan_wlan_weekdayEndTime", cpe_set_igd_lan_wlan_weekdayEndTime},
    {"cpe_get_igd_lan_wlan_weekendStartTime", cpe_get_igd_lan_wlan_weekendStartTime},
    {"cpe_set_igd_lan_wlan_weekendStartTime", cpe_set_igd_lan_wlan_weekendStartTime},
    {"cpe_get_igd_lan_wlan_weekendEndTime", cpe_get_igd_lan_wlan_weekendEndTime},
    {"cpe_set_igd_lan_wlan_weekendEndTime", cpe_set_igd_lan_wlan_weekendEndTime},
    
    /*广告过滤*/
    {"cpe_get_igd_dc_adfilter_enable",cpe_get_igd_dc_adfilter_enable},
    {"cpe_set_igd_dc_adfilter_enable", cpe_set_igd_dc_adfilter_enable},

    /*游戏加速*/
    {"cpe_get_igd_dc_gamefast_enable",cpe_get_igd_dc_gamefast_enable},
    {"cpe_set_igd_dc_gamefast_enable",cpe_set_igd_dc_gamefast_enable},
    {"cpe_get_igd_dc_gamefast_uplinkrate",cpe_get_igd_dc_gamefast_uplinkrate},
    {"cpe_set_igd_dc_gamefast_uplinkrate",cpe_set_igd_dc_gamefast_uplinkrate},
    {"cpe_get_igd_dc_gamefast_downlinkrate",cpe_get_igd_dc_gamefast_downlinkrate},
    {"cpe_set_igd_dc_gamefast_downlinkrate",cpe_set_igd_dc_gamefast_downlinkrate},

    /*黑名单*/
    {"cpe_set_igd_lan_hosts_deniedMAC",cpe_set_igd_lan_hosts_deniedMAC},
    {"cpe_set_igd_lan_hosts_allowedMAC",cpe_set_igd_lan_hosts_allowedMAC},
    {"cpe_get_igd_lan_hosts_macfilter_status",cpe_get_igd_lan_hosts_macfilter_status},
    /*无线*/

    /*赚钱wifi*/
    {"cpe_set_igd_dc_bizwifi_enable",cpe_set_igd_dc_bizwifi_enable},
    {"cpe_get_igd_dc_bizwifi_enable",cpe_get_igd_dc_bizwifi_enable},
    {"cpe_set_igd_dc_bizwifi_account",cpe_set_igd_dc_bizwifi_account},
    {"cpe_get_igd_dc_bizwifi_account",cpe_get_igd_dc_bizwifi_account},
    {"cpe_set_igd_dc_bizwifi_startTime",cpe_set_igd_dc_bizwifi_startTime},
    {"cpe_get_igd_dc_bizwifi_startTime",cpe_get_igd_dc_bizwifi_startTime},
    {"cpe_set_igd_dc_bizwifi_endTime",cpe_set_igd_dc_bizwifi_endTime},
    {"cpe_get_igd_dc_bizwifi_endTime",cpe_get_igd_dc_bizwifi_endTime},
    {"cpe_set_igd_dc_bizwifi_price",cpe_set_igd_dc_bizwifi_price},
    {"cpe_get_igd_dc_bizwifi_price",cpe_get_igd_dc_bizwifi_price},
    {"cpe_set_igd_dc_bizwifi_SSID_2_4_G",cpe_set_igd_dc_bizwifi_SSID_2_4_G},
    {"cpe_get_igd_dc_bizwifi_SSID_2_4_G",cpe_get_igd_dc_bizwifi_SSID_2_4_G},
    {"cpe_set_igd_dc_bizwifi_SSID_5_G",cpe_set_igd_dc_bizwifi_SSID_5_G},
    {"cpe_get_igd_dc_bizwifi_SSID_5_G",cpe_get_igd_dc_bizwifi_SSID_5_G},
    {"cpe_set_igd_dc_bizwifi_freeTime",cpe_set_igd_dc_bizwifi_freeTime},
    {"cpe_get_igd_dc_bizwifi_freeTime",cpe_get_igd_dc_bizwifi_freeTime},
    {"cpe_set_igd_dc_bizwifi_security",cpe_set_igd_dc_bizwifi_security},
    {"cpe_get_igd_dc_bizwifi_security",cpe_get_igd_dc_bizwifi_security},
    {"cpe_set_igd_dc_bizwifi_upRate",cpe_set_igd_dc_bizwifi_upRate},
    {"cpe_get_igd_dc_bizwifi_upRate",cpe_get_igd_dc_bizwifi_upRate},
    {"cpe_set_igd_dc_bizwifi_downRate",cpe_set_igd_dc_bizwifi_downRate},
    {"cpe_get_igd_dc_bizwifi_downRate",cpe_get_igd_dc_bizwifi_downRate},

#if (PPTP_CLIENT == FYES || L2TP_CLIENT == FYES)
    /*vpn*/
    {"cpe_refresh_igd_dc_vpn",cpe_refresh_igd_dc_vpn},
    {"cpe_get_igd_dc_vpnenable",cpe_get_igd_dc_vpnenable},
    {"cpe_set_igd_dc_vpnenable",cpe_set_igd_dc_vpnenable},
    {"cpe_get_igd_dc_vpnnumber",cpe_get_igd_dc_vpnnumber},
    {"cpe_get_igd_dc_vpn_enable",cpe_get_igd_dc_vpn_enable},
    {"cpe_set_igd_dc_vpn_enable",cpe_set_igd_dc_vpn_enable},
    {"cpe_get_igd_dc_vpn_type",cpe_get_igd_dc_vpn_type},
    {"cpe_set_igd_dc_vpn_type",cpe_set_igd_dc_vpn_type},
    {"cpe_get_igd_dc_vpn_server",cpe_get_igd_dc_vpn_server},
    {"cpe_set_igd_dc_vpn_server",cpe_set_igd_dc_vpn_server},
    {"cpe_get_igd_dc_vpn_account",cpe_get_igd_dc_vpn_account},
    {"cpe_set_igd_dc_vpn_account",cpe_set_igd_dc_vpn_account},
    {"cpe_get_igd_dc_vpn_passwd",cpe_get_igd_dc_vpn_passwd},
    {"cpe_set_igd_dc_vpn_passwd",cpe_set_igd_dc_vpn_passwd},
    {"cpe_get_igd_dc_vpn_mode",cpe_get_igd_dc_vpn_mode},
    {"cpe_set_igd_dc_vpn_mode",cpe_set_igd_dc_vpn_mode},
    {"cpe_get_igd_dc_vpn_ip",cpe_get_igd_dc_vpn_ip},
    {"cpe_set_igd_dc_vpn_ip",cpe_set_igd_dc_vpn_ip},
    {"cpe_get_igd_dc_vpn_netmask",cpe_get_igd_dc_vpn_netmask},
    {"cpe_set_igd_dc_vpn_netmask",cpe_set_igd_dc_vpn_netmask},
#endif
    /*用户状态*/
    {"cpe_detail_igd_lan_hosts",cpe_detail_igd_lan_hosts},
    {"cpe_set_l_igd_lan_hosts_host_mac",cpe_set_l_igd_lan_hosts_host_mac},
    {"cpe_set_l_igd_lan_hosts_host_speedlimit_node",cpe_set_l_igd_lan_hosts_host_speedlimit_node},
    /*限速*/
    {"cpe_refresh_igd_lan_hosts_limithost",cpe_refresh_igd_lan_hosts_limithost},
    {"cpe_detail_igd_lan_hosts_limithost",cpe_detail_igd_lan_hosts_limithost},

    {"cpe_get_igd_dc_passwd",cpe_get_igd_dc_passwd},
    {"cpe_set_igd_dc_passwd",cpe_set_igd_dc_passwd},
    /*上线通知*/
    {"cpe_detail_igd_lan_newhosts",cpe_detail_igd_lan_newhosts},
    {"cpe_refresh_igd_new_host",cpe_refresh_igd_new_host},

    {"cpe_refresh_igd_lan_lhcm_if",cpe_refresh_igd_lan_lhcm_if},
    {"cpe_get_l_igd_lan_lhcm_if_number",cpe_get_l_igd_lan_lhcm_if_number},
    {"cpe_get_l_igd_lan_lhcm_if_ip",cpe_get_l_igd_lan_lhcm_if_ip},

    {"cpe_get_igd_utt_onlinenotify",cpe_get_igd_utt_onlinenotify},
    {"cpe_set_igd_utt_onlinenotify",cpe_set_igd_utt_onlinenotify},
    {"cpe_get_dc_configVersion",cpe_get_dc_configVersion},
    {"cpe_refresh_igd_host",cpe_refresh_igd_host},
#endif

    {"cpe_get_igd_wd_status",cpe_get_igd_wd_status},
    {"cpe_get_value_by_name",cpe_get_value_by_name},

};

int get_index_after_paramname(parameter_node_t * param, const char * tag_name)
{
    parameter_node_t * parent;
    parameter_node_t * tmp;
    for(parent=param->parent, tmp = param; parent; tmp = parent, parent = parent->parent)
    {
        if(TRstrcmp(parent->name, tag_name) == 0)
        {
             if(is_digit(tmp->name) == 0)
             {
                return TRatoi(tmp->name);   
             }
        }        
    }
    return -1;
}


void cwmp_model_load(cwmp_t * cwmp, const char * xmlfile)
{  
    cwmp_model_load_xml(cwmp, xmlfile, ModelFunction, sizeof(ModelFunction)/sizeof(model_func_t));
}


