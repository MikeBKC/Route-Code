int  cpe_refresh_igd_lan_hosts_limithost(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
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
       
        cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg); 
    }
    return FAULT_CODE_OK;
}
int cpe_detail_igd_lan_hosts_limithost(cwmp_t * cwmp, const char * name,parameter_list_t ** ppl, pool_t * pool)
{
#if (MACADDR_ADD_LIMIT == FYES) 
    int gbmin, gbmax, gbindex = 0,num=0;
    MibProfileType gbmibType = MIB_PROF_GROUP_BASE;
    GroupBaseType webGroupType = GBT_FINE_LIMIT;
    GroupBaseProfile *gbprof = NULL;  
    ProfInstLowHigh(gbmibType, &gbmax, &gbmin);
    char childName[128];
    char buffer[20];
    char *mac;

    for(gbindex = gbmin; gbindex < gbmax; gbindex++) {
	gbprof = (GroupBaseProfile*)ProfGetInstPointByIndex(gbmibType, gbindex);
	if(ProfInstIsFree(gbprof) == 0 && gbprof->groupType == webGroupType && gbprof->limitPolicy == 3) {
	    num++;
	    snprintf(childName,128,"%s.%d.SpeedLimit.MACAddress",name,num);
	    //snprintf(buffer,20,"%02x:%02x:%02x:%02x:%02x:%02x",name,i);
	    mac = converMac6To12Byte(gbprof->mac_addr);
	    cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName,mac);

	    snprintf(childName,128,"%s.%d.SpeedLimit.StartTime",name,num);
	    // snprintf(buffer,20,"%d",gbprof->uRate);
	    cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName,gbprof->timeStart);

	    snprintf(childName,128,"%s.%d.SpeedLimit.EndTime",name,num);
	    // snprintf(buffer,20,"%d",gbprof->uRate);
	    cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName,gbprof->timeStop);

	    snprintf(childName,128,"%s.%d.SpeedLimit.UpMaxRate",name,num);
	    snprintf(buffer,20,"%d",gbprof->uRate);
	    cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName,buffer);

	    snprintf(childName,128,"%s.%d.SpeedLimit.DownMaxRate",name,num);
	    snprintf(buffer,20,"%d",gbprof->dRate);
	    cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName,buffer);
//	    num++;
	}
    }
    snprintf(childName,128,"InternetGatewayDevice.LANDevice.1.Hosts.LimitHostNumberOfEntries");
//    num--;
    snprintf(buffer,20,"%d",num);
    cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName,buffer);
#endif 
}
