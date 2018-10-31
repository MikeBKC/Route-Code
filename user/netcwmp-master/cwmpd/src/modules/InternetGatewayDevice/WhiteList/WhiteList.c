int  cpe_refresh_igd_whitelist(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
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
            cwmp_log_info("refresh WANDevice node, delete param %s\n", tmp_param->name);
            tmp_node = tmp_param->next_sibling;
            cwmp_model_delete_parameter(tmp_param);
            tmp_param = tmp_node;
        }
        child_param->next_sibling = NULL;
                
#if(WIFIDOG_AUTH == FYES)
#if(WHITELIST == FYES)
        parameter_node_t * whitelist;
        int i = 1;
        for(i = 1;i <= MAX_WHITE_LIST;i++)
            cwmp_model_copy_parameter(param_node, &whitelist, i);
#endif
#endif
        cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg); 
    }
    return FAULT_CODE_OK;
}

static int get_white_list_index(const char *name)
{
    int id;
    sscanf(name, "InternetGatewayDevice.WhiteList.%d.domain", &id);
    id--;

    return id;
}
int cpe_get_igd_whitelist(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    int index = get_white_list_index(name);
    char buf[100] = {0};

    snprintf(buf,sizeof(buf),"InternetGatewayDevice.whiteList.%d.domain.",index);
    name = buf;
    get_method(cwmp,name,value,pool);
    return FAULT_CODE_OK;
}

extern int global_one_request;
int cpe_set_igd_whitelist(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if(WIFIDOG_AUTH == FYES)
#if(WHITELIST == FYES)
    MibProfileType mibType = MIB_PROF_WHITE_LIST;
    ProfChangeType profAction = PROF_CHANGE_DELALL;
    int min, max, index = 0;
    char tmp[100];
    WhiteListProfile *prof = NULL;
    int index_x = get_white_list_index(name);
    struProfAlloc *profhead = NULL;

    ProfInstLowHigh(mibType, &max, &min);
    if(index_x < min || index_x > max) {
        return FAULT_CODE_9007;
    }

    if(global_one_request == 1)
    {
        global_one_request = 0; 
        ProfBackupByIndex(mibType, profAction, 0, &profhead);/*发送delall消息*/
        for (index = min; index < max; index++)
        {
            ProfDelInstByIndex(mibType, index);/*删除域名*/
        }
        ProfUpdate(profhead);
        ProfFreeAllocList(profhead);
    }
    profhead = NULL;
    index = index_x;

    profAction = PROF_CHANGE_ADD;
    sprintf(tmp, "%d", index);
    index = ProfNewInst(mibType, tmp, TRUE);
    /* Backup */
    ProfBackupByIndex(mibType, profAction, index, &profhead);
    /* Get pointer */
    prof = (WhiteListProfile *)ProfGetInstPointByName(mibType, tmp);
    /* Change value */
    strncpy(prof->domain, value,sizeof(prof->domain));
    /* Effect immediately */
    ProfUpdate(profhead);
    /* Release */
    ProfFreeAllocList(profhead);
    /* Write config */
    nvramWriteCommit();

    return FAULT_CODE_OK;
#endif
#endif
    return FAULT_CODE_9005;
}

int cpe_set_igd_add_whitelist(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if(WIFIDOG_AUTH == FYES)
#if(WHITELIST == FYES)
    int min, max, index,one_exist = -1;
    char tmp[100];
    MibProfileType mibType = MIB_PROF_WHITE_LIST;
    WhiteListProfile *prof = NULL;
    ProfChangeType profAction = PROF_CHANGE_ADD;
    struProfAlloc *profhead = NULL;

    if(value == NULL || strlen(value) == 0)
        return FAULT_CODE_9007;

    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
        /* Blank */
        prof = (WhiteListProfile *)ProfGetInstPointByIndex(mibType, index);
        if (0 == strcmp(prof->domain, value))
        {
            return FAULT_CODE_OK;
        }

        if (0 == strcmp(prof->domain, "") && -1 == one_exist)
        {
            one_exist = index;
        }
    }
    if(one_exist != -1 && one_exist < max)
    {
        index = one_exist;
        sprintf(tmp, "%d", index);
        index = ProfNewInst(mibType, tmp, FALSE);
        /* Backup */
        ProfBackupByIndex(mibType, profAction, index, &profhead);
        /* Get pointer */
        prof = (WhiteListProfile *)ProfGetInstPointByName(mibType, tmp);
        /* Change value */
        strcpy(prof->domain, value);
        /* Effect immediately */
        ProfUpdate(profhead);
        /* Release */
        ProfFreeAllocList(profhead);
        /* Write config */
        nvramWriteCommit();
    }else{

        return FAULT_CODE_9005;
    }
    return FAULT_CODE_OK;
#endif
#endif
    return FAULT_CODE_9005;
}
