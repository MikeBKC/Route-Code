#include "DeviceInfo/DeviceInfo.c"
#include "ManagementServer/ManagementServer.c"
#include "WANDevice/WANDevice.c"
#include "WANDevice/WANConnectionDevice/WANConnectionDevice.c"
#include "WANDevice/WANConnectionDevice/WANIPConnection.c"
#include "webauthglobal/webauthglobal.c"

#include "WhiteList/WhiteList.c"
#include "LANDevice/LANDevice.c"
#include "LANDevice/WLANConfiguration/WLANConfiguration.c"
#include "X_UTT-COM-CN/X_utt_con_cn.c"
#if (WECHAT_CWMP == FYES)
#include "LANDevice/Hosts/Hosts.c"
#include "LANDevice/Hosts/Host/Host.c"
#include "LANDevice/NewHosts/NewHosts.c"
#include "LANDevice/NewHosts/Host/Host.c"
#include "LANDevice/Hosts/LimitHost/LimitHost.c"
#include "LANDevice/Hosts/LANHostConfigManagement/LANHostConfigManagement.c"
#include "LANDevice/LANHostConfigManagement/LANHostConfigManagement.c"
#include "LANDevice/LANHostConfigManagement/IPInterface/IPInterface.c"

#include "DeviceConfig/SmartQos.c"
#include "DeviceConfig/Adfilter.c"
#include "DeviceConfig/DeviceConfig.c"
#include "DeviceConfig/BizWiFi.c"
#include "DeviceConfig/VPN/VPN.c"
#include "Knowifi/knowifi.c"
#endif

char* cpe_get_igd_device_summary(void * arg, void * pool)
{
    //pool_t * p = (pool_t *)pool;
    return	NULL;
}

char* cpe_get_igd_lan_device_number_of_entries(void * arg, void * pool)
{
    //pool_t * p = (pool_t *)pool;
    return NULL;
}





