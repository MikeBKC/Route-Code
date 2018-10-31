int  cpe_refresh_igd_host(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
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
#if 0
int cpe_set_l_igd_lan_hosts_host_speedlimit_startTime(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    return FAULT_CODE_9005;
}
#endif
/*dependent on order of node*/
int cpe_set_l_igd_lan_hosts_host_mac(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if (MACADDR_ADD_LIMIT == FYES)
    int min, max, index = 0;
    GroupBaseType webGroupType = GBT_FINE_LIMIT; 
    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    struProfAlloc *profhead = NULL;
    //ProfChangeType profAction=PROF_CHANGE_EDIT;
    ProfChangeType profAction=PROF_CHANGE_SPECIAL;
    GroupBaseProfile *profGroupBase = NULL;
    MacAddr macAddr;

    char * GroupName ;
    int instIndex;
    parameter_node_t * pn;
    unsigned int uRate,dRate;

    pn = cwmp_get_parameter_node(cwmp->root, "InternetGatewayDevice.LANDevice.1.Hosts.Host.1.MACAddress");
    if(pn != NULL && pn->value != NULL){
	GroupName = pn->value ;
#if 0
	instIndex = ProfGetInstIndexByName(mibType, GroupName);
#endif
	if(strlen(pn->value) > 0){
	    memset(&macAddr, 0, sizeof(macAddr));
	    converMac12To6Byte(pn->value, &macAddr);
	}else{
	    cwmp_log_error("%s  %d  mac error\n",__func__,__LINE__);
	}

	if(value != NULL){
	    uRate = strtoul(value, NULL, 0);/*上传速率*/
	}
	pn = cwmp_get_parameter_node(cwmp->root, "InternetGatewayDevice.LANDevice.1.Hosts.Host.1.SpeedLimit.DownMaxRate");
	if(pn != NULL && pn->value != NULL){
	    dRate = strtoul(pn->value, NULL, 0);/*上传速率*/
	}

	ProfInstLowHigh(mibType, &max, &min);
	for(index = min; index < max; index++) {
	    profGroupBase = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, index);
	    if(ProfInstIsFree(profGroupBase) == 0 && profGroupBase->groupType == webGroupType && profGroupBase->limitPolicy == 3) {
		if(memcmp(macAddr.addr,profGroupBase->mac_addr.addr,6) == 0){
		    break;
		}
	    }
	}

	if(dRate == 0 && uRate==0){
	    if(index < max){
		ProfBackupByIndex(mibType, PROF_CHANGE_DEL, index, &profhead);
		ProfDelInstByIndex(mibType, index);    
		goto finish;
	    }else{
		return FAULT_CODE_OK;
	    }
	}

	if(index == max) 
	{
	    instIndex = ProfGetInstIndexByName(mibType, GroupName);
	    if(instIndex == PROFFAIL){
		if(ProfCountInstByValue(mibType, OFFANDSIZE(GroupBaseProfile, groupType), 0u, (Pointer)&webGroupType) >= MAX_FINE_LIMIT_PROFILES){
		    goto finish;
		}
		instIndex = ProfNewInst(mibType, GroupName, FALSE);
		index = instIndex ;
		profAction = PROF_CHANGE_ADD;

		ProfBackupByIndex(mibType, profAction, instIndex, &profhead);
		profGroupBase = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, instIndex);
		profGroupBase->groupType = webGroupType;/*记录类型方便不同的页面使用*/
		profGroupBase->day= 127; 

		strcpy(profGroupBase->timeStart, "All"); 
		strcpy(profGroupBase->timeStop, "0");

		profGroupBase->limitPolicy = 3;/*限速策略*/
		profGroupBase->mac_addr = macAddr;
	    }
	}else{
	    ProfBackupByIndex(mibType, PROF_CHANGE_SPECIAL, index, &profhead);
	}
	profGroupBase->uRate = uRate;
	profGroupBase->dRate = dRate;
#if 1
finish:
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
	return FAULT_CODE_OK;
    }
#endif
#endif
    return FAULT_CODE_9005;
}

int cpe_set_l_igd_lan_hosts_host_speedlimit_node(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    parameter_node_t * pn;
    pn = cwmp_get_parameter_node(cwmp->root, name);
    if(pn != NULL ){
	if(pn->value == NULL){
	    pn->value = (char *)pool_pcalloc(cwmp->pool, 128);
	    if(pn->value == NULL)
		return FAULT_CODE_9002;
	}
	pn->value_length = 128;
	snprintf(pn->value,128,"%s",value);
	return FAULT_CODE_OK;
    }
    return FAULT_CODE_9005;
}
#if 0
int cpe_set_l_igd_lan_hosts_host_endTime(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    return FAULT_CODE_9005;
}
int cpe_set_l_igd_lan_hosts_host_speedlimit_UpMaxRate(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    return FAULT_CODE_9005;
}
int cpe_set_l_igd_lan_hosts_host_speedlimit_UpMinRate(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    return FAULT_CODE_9005;
}
int cpe_set_l_igd_lan_hosts_host_speedlimit_DownMaxRate(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    return FAULT_CODE_9005;
}
int cpe_set_l_igd_lan_hosts_host_speedlimit_DownMinRate(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    return FAULT_CODE_9005;
}
#endif
