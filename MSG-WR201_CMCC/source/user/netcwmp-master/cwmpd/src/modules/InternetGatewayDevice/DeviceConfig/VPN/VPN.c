int cpe_refresh_igd_dc_vpn(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    FUNCTION_TRACE();

    if(!param_node)
    {
	return FAULT_CODE_9002;
    }
    parameter_node_t * tmp_param, *tmp_node, *child_param;
    child_param = param_node->child;
    if(child_param)
    {
	for(tmp_param=child_param->next_sibling; tmp_param; )
	{
	    cwmp_log_info("refresh Host node, delete param %s\n", tmp_param->name);
	    tmp_node = tmp_param->next_sibling;
	    cwmp_model_delete_parameter(tmp_param);
	    tmp_param = tmp_node;
	}
	child_param->next_sibling = NULL;

	parameter_node_t * vpn_param;
	cwmp_model_copy_parameter(param_node, &vpn_param, 1);
	cwmp_model_copy_parameter(param_node, &vpn_param, 2);

	cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg); 
    }
    return FAULT_CODE_OK;
}
static int  getVpntype(char * name){
    int type ;
    sscanf(name,"InternetGatewayDevice.DeviceConfig.VPN.%d.%*s",&type);
    return type;
}
int cpe_get_igd_dc_vpn_enable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    int type = getVpntype(name);
    int index = 0;
    char tmpname[128];
    switch(type){
#if (PPTP_CLIENT == FYES)
	case 1: 
	    snprintf(tmpname,128,"InternetGatewayDevice.config.pptpClient.active");
	    break;
#endif
#if (L2TP_CLIENT == FYES)
	case 2: 
	    snprintf(tmpname,128,"InternetGatewayDevice.config.l2tpClient.active");
	    break;
#endif
	default : ; 	
    }
    get_method(cwmp,tmpname,value,pool);
    if(strcmp(*value,"Yes") == 0)
    {
	*value = "1";
    }else{
	*value = "0";
    }
    return FAULT_CODE_OK;
}


int cpe_set_igd_dc_vpn_enable(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    char tmpname[128];
    int index = 0;
    int type = getVpntype(name);

    switch(type){
#if (PPTP_CLIENT == FYES)
	case 1: 
	    snprintf(tmpname,128,"InternetGatewayDevice.config.pptpClient.active");
	    break;
#endif
#if (L2TP_CLIENT == FYES)
	case 2: 
	    snprintf(tmpname,128,"InternetGatewayDevice.config.l2tpClient.active");
	    break;
#endif
	default:  return FAULT_CODE_9005; 	
    }
    if(strcmp(value,"1") == 0)
    {
	value = "Yes";
    }else{
	value = "No";
    }
    return set_method(cwmp,tmpname,value,length,callback_reg,FALSE);
}
int cpe_get_igd_dc_vpn_type(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    return FAULT_CODE_OK;
}


int cpe_set_igd_dc_vpn_type(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    return FAULT_CODE_OK;
}
int cpe_get_igd_dc_vpn_server(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
#if  1
    int type = getVpntype(name);
    int index = 0;
    char tmpname[128];

    switch(type){
#if (PPTP_CLIENT == FYES)
	case 1: 
	    snprintf(tmpname,128,"InternetGatewayDevice.config.pptpClient.serverHost");
	    break;
#endif
#if (L2TP_CLIENT == FYES)
	case 2: 
	    snprintf(tmpname,128,"InternetGatewayDevice.config.l2tpClient.serverHost");
	    break;
#endif
	default : ; 	
    }
    get_method(cwmp,tmpname,value,pool);
#endif

    return FAULT_CODE_OK;
}

int cpe_set_igd_dc_vpn_server(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    char tmpname[128];
    int index = 0;
    int type = getVpntype(name);
printf("%s  -- %d type = %d\n",__func__,__LINE__,type);
    switch(type){
#if (PPTP_CLIENT == FYES)
	case 1: 
	    snprintf(tmpname,128,"InternetGatewayDevice.config.pptpClient.serverHost");
	    break;
#endif
#if (L2TP_CLIENT == FYES)
	case 2: 
	    snprintf(tmpname,128,"InternetGatewayDevice.config.l2tpClient.serverHost");
	    break;
#endif
	default:  return FAULT_CODE_9005; 	
    }
printf("%s  -- %d tmpname = %s\n",__func__,__LINE__,tmpname);
    return set_method(cwmp,tmpname,value,length,callback_reg,FALSE);
}
int cpe_get_igd_dc_vpn_account(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    int type = getVpntype(name);
    int index = 0;
    char tmpname[128];

    switch(type){
#if (PPTP_CLIENT == FYES)
	case 1: 
	    snprintf(tmpname,128,"InternetGatewayDevice.config.pptpClient.userName");
	    break;
#endif
#if (L2TP_CLIENT == FYES)
	case 2: 
	    snprintf(tmpname,128,"InternetGatewayDevice.config.l2tpClient.userName");
	    break;
#endif
	default : ; 	
    }
    get_method(cwmp,tmpname,value,pool);
    return FAULT_CODE_OK;

}


int cpe_set_igd_dc_vpn_account(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    char tmpname[128];
    int index = 0;
    int type = getVpntype(name);
    switch(type){
#if (PPTP_CLIENT == FYES)
	case 1: 
	    snprintf(tmpname,128,"InternetGatewayDevice.config.pptpClient.userName");
	    break;
#endif
#if (L2TP_CLIENT == FYES)
	case 2: 
	    snprintf(tmpname,128,"InternetGatewayDevice.config.l2tpClient.userName");

	    break;
#endif
	default:  return FAULT_CODE_9005; 	
    }

    return set_method(cwmp,tmpname,value,length,callback_reg,FALSE);
}
int cpe_get_igd_dc_vpn_passwd(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    int type = getVpntype(name);
    int index = 0;
    char tmpname[128];
    switch(type){
#if (PPTP_CLIENT == FYES)
	case 1: 
	    snprintf(tmpname,128,"InternetGatewayDevice.config.pptpClient.passwd");
	    break;
#endif
#if (L2TP_CLIENT == FYES)
	case 2: 
	    snprintf(tmpname,128,"InternetGatewayDevice.config.l2tpClient.passwd");
	    break;
#endif
	default : ; 	
    }
    get_method(cwmp,tmpname,value,pool);

    return FAULT_CODE_OK;
}


int cpe_set_igd_dc_vpn_passwd(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    char tmpname[128];
    int index = 0;
    int type = getVpntype(name);
    switch(type){
#if (PPTP_CLIENT == FYES)
	case 1: 
	    snprintf(tmpname,128,"InternetGatewayDevice.config.pptpClient.passwd");
	    break;
#endif
#if (L2TP_CLIENT == FYES)
	case 2: 
	    snprintf(tmpname,128,"InternetGatewayDevice.config.l2tpClient.passwd");

	    break;
#endif
	default:  return FAULT_CODE_9005; 	
    }

    return set_method(cwmp,tmpname,value,length,callback_reg,FALSE);
}
int cpe_get_igd_dc_vpn_mode(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    int type = getVpntype(name);
    int index = 0;
    char tmpname[128];
    switch(type){
#if (PPTP_CLIENT == FYES)
	case 1: 
	    snprintf(tmpname,128,"InternetGatewayDevice.config.pptpClient.vpnMode");
	    break;
#endif
#if (L2TP_CLIENT == FYES)
	case 2: 
	    snprintf(tmpname,128,"InternetGatewayDevice.config.l2tpClient.vpnMode");
	    break;
#endif
	default :return FAULT_CODE_9005;	
    }
    get_method(cwmp,tmpname,value,pool);
    return FAULT_CODE_OK;
}


int cpe_set_igd_dc_vpn_mode(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if 1
    char tmpname[128];
    char *vpnName;
    int index = 0;
    int type = getVpntype(name);
    switch(type){
#if (PPTP_CLIENT == FYES && VPN_PROXY == FYES)
	case 1: 
	    snprintf(tmpname,128,"InternetGatewayDevice.config.pptpClient.vpnMode");
	    break;
#endif
#if (L2TP_CLIENT == FYES && VPN_PROXY == FYES)
	case 2: 
	    snprintf(tmpname,128,"InternetGatewayDevice.config.l2tpClient.vpnMode");

	    break;
#endif
	default:  return FAULT_CODE_9005; 	
    }

    return set_method(cwmp,tmpname,value,length,callback_reg,FALSE);
#endif
    return FAULT_CODE_9005;
}
int cpe_get_igd_dc_vpn_ip(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    int type = getVpntype(name);
    int index = 0;
    char tmpname[128];
    switch(type){
#if (PPTP_CLIENT == FYES)
	case 1: 
	    //snprintf(tmpname,128,"InternetGatewayDevice.config.pptpClient.0.remoteLanIp");
	    snprintf(tmpname,128,"InternetGatewayDevice.config.pptpClient.remoteLanIpDis");
	    break;
#endif
#if (L2TP_CLIENT == FYES)
	case 2: 
//	    snprintf(tmpname,128,"InternetGatewayDevice.config.l2tpClient.remoteLanIp");
	     snprintf(tmpname,128,"InternetGatewayDevice.config.l2tpClient.remoteLanIpDis");
	    break;
#endif
	default : ; 	
    }
    get_method(cwmp,tmpname,value,pool);

    return FAULT_CODE_OK;
}


int cpe_set_igd_dc_vpn_ip(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    char tmpname[128];
    int index = 0;
    int type = getVpntype(name);

    switch(type){
#if (PPTP_CLIENT == FYES)
	case 1: 
	    //snprintf(tmpname,128,"InternetGatewayDevice.config.pptpClient.0.remoteLanIp");
	    snprintf(tmpname,128,"InternetGatewayDevice.config.pptpClient.remoteLanIpDis");
	    break;
#endif
#if (L2TP_CLIENT == FYES)
	case 2: 
	   // snprintf(tmpname,128,"InternetGatewayDevice.config.l2tpClient.remoteLanIp");
	       snprintf(tmpname,128,"InternetGatewayDevice.config.l2tpClient.remoteLanIpDis");

	    break;
#endif
	default:  return FAULT_CODE_9005; 	
    }

    return set_method(cwmp,tmpname,value,length,callback_reg,FALSE);
}
int cpe_get_igd_dc_vpn_netmask(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    int type = getVpntype(name);
    int index = 0;
    char tmpname[128];
    switch(type){
#if (PPTP_CLIENT == FYES)
	case 1: 
	    snprintf(tmpname,128,"InternetGatewayDevice.config.pptpClient.remoteLanNetmask");
	    break;
#endif
#if (L2TP_CLIENT == FYES)
	case 2: 
	    snprintf(tmpname,128,"InternetGatewayDevice.config.l2tpClient.remoteLanNetmask");
	    break;
#endif
	default : ; 	
    }
    get_method(cwmp,tmpname,value,pool);
    return FAULT_CODE_OK;
}


int cpe_set_igd_dc_vpn_netmask(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    char tmpname[128];
    int index = 0;
    int type = getVpntype(name);

    switch(type){
#if (PPTP_CLIENT == FYES)
	case 1: 
	    snprintf(tmpname,128,"InternetGatewayDevice.config.pptpClient.remoteLanNetmask");
	    break;
#endif
#if (L2TP_CLIENT == FYES)
	case 2: 
	    snprintf(tmpname,128,"InternetGatewayDevice.config.l2tpClient.remoteLanNetmask");

	    break;
#endif
	default:  return FAULT_CODE_9005; 	
    }

    return set_method(cwmp,tmpname,value,length,callback_reg,FALSE);
}

