int cpe_refresh_igd_lan_lhcm_if(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
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
                
        parameter_node_t * lan1_param;
        cwmp_model_copy_parameter(param_node, &lan1_param, 1);
#if (MULTI_LAN == FYES)
	lan1_param=NULL;
        cwmp_model_copy_parameter(param_node, &lan1_param, 1);
	lan1_param=NULL;
        cwmp_model_copy_parameter(param_node, &lan1_param, 1);
	lan1_param=NULL;
        cwmp_model_copy_parameter(param_node, &lan1_param, 1);
#endif
       
        cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg); 
    }
    return FAULT_CODE_OK;
}
#if (MULTI_LAN == FYES)
static int get_lan_if_index(char *name){
    int index;
    sscanf(name,"InternetGatewayDevice.LANDevice.1.LANHostConfigManagement.IPInterface.%d.IPInterfaceIPAddress",&index);
    return index;
}
#endif
int cpe_get_l_igd_lan_lhcm_if_ip(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
    static char tmp[20];
    int index = 1;
    unsigned int ip;
    unsigned char *strip = (unsigned char *) &ip;
#if (MULTI_LAN == FYES)
    index = get_lan_if_index(name);
#endif
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
    if (ProfInstIsFree(profIF) == 0 ){
	if(index == 1){
	    ip = profIF->ether.sta.ip;
	}
#if (MULTI_LAN == FYES)
	if(index == 2){
	    ip = profIF->ether.sta.ip2;
	}
	if(index == 3){
	    ip = profIF->ether.sta.ip3;
	}
	if(index == 4){
	    ip = profIF->ether.sta.ip4;
	}
#endif
    }
    snprintf(tmp,20,"%u.%u.%u.%u",strip[0],strip[1],strip[2],strip[3]);
    *value  = tmp;
    return FAULT_CODE_OK;
}
