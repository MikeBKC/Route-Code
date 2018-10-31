#include	<time.h>
#include	<sys/time.h>
#include    <sys/sysinfo.h>

//InternetGatewayDevice.DeviceInfo.Manufacturer
int cpe_get_igd_di_manufacturer(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    FUNCTION_TRACE();
    *value = cwmp_conf_pool_get(pool, "cwmp:cpe_manufacture");
    cwmp_log_debug("cpe_get_igd_di_manufacturer: value is %s", *value);
    return	FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.ManufacturerOUI
int cpe_get_igd_di_manufactureroui(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    FUNCTION_TRACE();
    *value = cwmp_conf_pool_get(pool, "cwmp:cpe_oui");
    return	FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.ProductClass
int cpe_get_igd_di_productclass(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    FUNCTION_TRACE();
    *value = cwmp_conf_pool_get(pool, "cwmp:cpe_pc");
    return	FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.SerialNumber
int cpe_get_igd_di_serialnumber(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	FUNCTION_TRACE();
    *value = cwmp_conf_pool_get(pool, "cwmp:cpe_sn");
    return	FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.MacAddress
int cpe_get_igd_di_macaddress(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	FUNCTION_TRACE();
#if 0
    *value = cwmp_conf_pool_get(pool, "cwmp:cpe_sn");
#else
    char idk[20] = {0};

    name = "InternetGatewayDevice.config.interface.0.ethernet.mac.";
    get_method(cwmp,name,value,pool);

    macstr_filter_colon(*value,idk);

    *value = PSTRDUP(idk);
#endif
    return	FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.SpecVersion
int cpe_get_igd_di_specversion(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	FUNCTION_TRACE();
    *value = cwmp_conf_pool_get(pool, "cwmp:cpe_specver");
    return	FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.HardwareVersion
int cpe_get_igd_di_hardwareversion(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	FUNCTION_TRACE();
    *value = cwmp_conf_pool_get(pool, "cwmp:cpe_hwver");
    return	FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.SoftwareVersion
int cpe_get_igd_di_softwareversion(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	FUNCTION_TRACE();
    *value = cwmp_conf_pool_get(pool, "cwmp:cpe_version");
    return	FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.ProvisioningCode
int cpe_get_igd_di_provisioningcode(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	FUNCTION_TRACE();
    *value = cwmp_conf_pool_get(pool, "cwmp:cpe_pc");
    return	FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.UpTime
int cpe_get_igd_di_sysuptime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	FUNCTION_TRACE();
	struct sysinfo info;
	const char uptime[20] = {0};
	sysinfo(&info);

	snprintf(uptime,sizeof(uptime),"%d",(int) info.uptime);
	*value = PSTRDUP(uptime);
    return	FAULT_CODE_OK;
}
