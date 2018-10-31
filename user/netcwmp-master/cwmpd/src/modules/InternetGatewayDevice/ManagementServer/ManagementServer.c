
#include "mib.h"
#include "profacce.h"
#include "argcmd.h"
int cpe_get_localip(const char * eth_name, char *hostip)
{
    register int fd,intrface,retn=0;
    struct ifreq buf[32];
    struct ifconf ifc;
    char domain_host[100] = {0};
    char local_ip_addr[20] = {0};
    char local_mac[20] = {0};
    //Get Domain Name --------------------------------------------------
    strcpy(local_ip_addr, "127.0.0.1");
    if (!hostip)
        return -1;
    if (getdomainname(&domain_host[0], 100) != 0)
    {
        return -1;
    }
    //------------------------------------------------------------------
    //Get IP Address & Mac Address ----------------------------------------
    if ((fd=socket(AF_INET,SOCK_DGRAM,0))>=0)
    {
        ifc.ifc_len=sizeof buf;
        ifc.ifc_buf=(caddr_t)buf;
        if (!ioctl(fd,SIOCGIFCONF,(char*)&ifc))
        {
            intrface=ifc.ifc_len/sizeof(struct ifreq);
            while (intrface-->0)
            {
                if (!(ioctl(fd,SIOCGIFFLAGS,(char*)&buf[intrface])))
                {
                    if (buf[intrface].ifr_flags&IFF_PROMISC)
                    {
                        retn++;
                    }
                }
                //Get IP Address
                if (!(ioctl(fd,SIOCGIFADDR,(char*)&buf[intrface])))
                {
		    if(strcmp(eth_name, buf[intrface].ifr_name) == 0)
		    {
                    sprintf(local_ip_addr, "%s", inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr));
		
		    break;
		    }
                }
                //Get Hardware Address

            }//While
        }
    }
    if ( fd > 0 )
    {
        close(fd);
    }

    strcpy(hostip, local_ip_addr);

    return CWMP_OK;
}

//InternetGatewayDevice.ManagementServer.Username
//InternetGatewayDevice.ManagementServer.acsUsername
int cpe_get_igd_ms_username(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    name = "InternetGatewayDevice.config.cwmp.acsUsername.";
    get_method(cwmp,name,value,pool);
    return FAULT_CODE_OK;
}

int cpe_set_igd_ms_username(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    if(set_method(cwmp,name,value,length,callback_reg,FALSE) == 1)
    {
        //cwmp->acs_user = XPSTRDUP(value);
    }
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.ManagementServer.Password
//InternetGatewayDevice.ManagementServer.acsPassword
int cpe_get_igd_ms_password(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    name = "InternetGatewayDevice.config.cwmp.acsPassword.";
    get_method(cwmp,name,value,pool);
    return FAULT_CODE_OK;
}

int cpe_set_igd_ms_password(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    if(set_method(cwmp,name,value,length,callback_reg,FALSE) == 1)
    {
       //cwmp->acs_pwd = XPSTRDUP(value);
    }
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.ManagementServer.URL
//InternetGatewayDevice.ManagementServer.cwmp.acsUrl
int cpe_get_igd_ms_url(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    name = "InternetGatewayDevice.config.cwmp.acsUrl.";
    get_method(cwmp,name,value,pool);
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.ManagementServer.URL
int cpe_set_igd_ms_url(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{

    if(set_method(cwmp,name,value,length,callback_reg,FALSE) == 1)
    {
        cwmp->acs_url = XPSTRDUP(value);
    }
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.ManagementServer.ConnectionRequestURL
int cpe_get_igd_ms_connectionrequesturl(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char buf[256]={0};
#if 0
    char local_ip[32]={0};
    cpe_get_localip("eth2.2", local_ip);
#else
    char if_addr[16] = {0};
	getIfIp(getWanIfNamePPP(1), if_addr);
#endif

    int port = cwmp_conf_get_int("cwmpd:httpd_port");
#if 0
    snprintf(buf, 256, "http://%s:%d", local_ip, port);
#else
    snprintf(buf, 256, "http://%s:%d", if_addr, port);
#endif
    *value = PSTRDUP(buf);
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.ManagementServer.ConnectionRequestUsername
//InternetGatewayDevice.ManagementServer.cwmp.cpeUsername
int cpe_get_igd_ms_connectionrequestusername(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    name = "InternetGatewayDevice.config.cwmp.cpeUsername.";
    get_method(cwmp,name,value,pool);
    return FAULT_CODE_OK;
}
int cpe_set_igd_ms_connectionrequestusername(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    if(set_method(cwmp,name,value,length,callback_reg,FALSE) == 1)
    {
        cwmp->cpe_user = XPSTRDUP(value);
    }
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.ManagementServer.ConnectionRequestPassword
//InternetGatewayDevice.ManagementServer.cwmp.cpePassword
int cpe_get_igd_ms_connectionrequestpassword(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    name = "InternetGatewayDevice.config.cwmp.cpePassword.";
    get_method(cwmp,name,value,pool);
    return FAULT_CODE_OK;
}
int cpe_set_igd_ms_connectionrequestpassword(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    if(set_method(cwmp,name,value,length,callback_reg,FALSE) == 1)
    {
        cwmp->cpe_user = XPSTRDUP(value);
    }
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.ManagementServer.PeriodicInformEnable
//
//
//
//
int cpe_get_igd_ms_periodicinformenable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    *value = cwmp_conf_pool_get(pool, "cwmp:cpe_username");
    return FAULT_CODE_OK;
}
int cpe_set_igd_ms_periodicinformenable(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    return FAULT_CODE_OK;
}
//InternetGatewayDevice.ManagementServer.PeriodicInformInterval
//InternetGatewayDevice.ManagementServer.cwmp.periodicInformInterval
int cpe_get_igd_ms_periodicinforminterval(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    name = "InternetGatewayDevice.config.cwmp.periodicInformInterval.";
    get_method(cwmp,name,value,pool);
    return FAULT_CODE_OK;
}
int cpe_set_igd_ms_periodicinforminterval(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    //name = "InternetGatewayDevice.config.cwmp.periodicInformInterval.";
    if(set_method(cwmp,name,value,length,callback_reg,FALSE) == FAULT_CODE_OK)
    {
        cwmp->acs_period = strtol(value,NULL,10);
    }
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.ManagementServer.STUNURL
//InternetGatewayDevice.ManagementServer.cwmp.stunUrl
int cpe_get_igd_ms_stun_url(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    get_method(cwmp,name,value,pool);
    return FAULT_CODE_OK;
}
int cpe_set_igd_ms_stun_url(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    if(set_method(cwmp,name,value,length,callback_reg,FALSE) == 1)
    {
        cwmp->stun_url = XPSTRDUP(value);
    }
    return FAULT_CODE_OK;
}
//InternetGatewayDevice.ManagementServer.STUNPeriodicInterval
//InternetGatewayDevice.ManagementServer.cwmp.stunPeriodicInterval
int cpe_get_igd_ms_stun_periodicinterval(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    get_method(cwmp,name,value,pool);
    return FAULT_CODE_OK;
}
int cpe_set_igd_ms_stun_periodicinterval(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    if(set_method(cwmp,name,value,length,callback_reg,FALSE) == 1)
    {
        cwmp->stun_period = strtol(value,NULL,10);
    }
    return FAULT_CODE_OK;
}
//InternetGatewayDevice.ManagementServer.STUNUsername
//InternetGatewayDevice.ManagementServer.cwmp.stunUsername
int cpe_get_igd_ms_stun_username(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    get_method(cwmp,name,value,pool);
    return FAULT_CODE_OK;
}
int cpe_set_igd_ms_stun_username(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    if(set_method(cwmp,name,value,length,callback_reg,FALSE) == 1)
    {
        cwmp->stun_user = XPSTRDUP(value);
    }
    return FAULT_CODE_OK;
}
//InternetGatewayDevice.ManagementServer.STUNPassword
//InternetGatewayDevice.ManagementServer.cwmp.stunPassword
int cpe_get_igd_ms_stun_password(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    get_method(cwmp,name,value,pool);
    return FAULT_CODE_OK;
}
int cpe_set_igd_ms_stun_password(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    if(set_method(cwmp,name,value,length,callback_reg,FALSE) == 1)
    {
        cwmp->stun_pwd = XPSTRDUP(value);
    }
    return FAULT_CODE_OK;
}
