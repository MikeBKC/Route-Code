int cpe_get_igd_dc_gamefast_enable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if ((SMART_QOS_FUN == FYES) || (GAME_BOOST == FYES))
#if 0
    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *prof = NULL;
    prof = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, 1);
    if(prof != NULL && prof->smartQosEn  == 1){
	*value="1";
    }else{
	*value="0";
    }
#endif
    name = "InternetGatewayDevice.config.interface.1.smartQosEn";
    get_method(cwmp,name,value,pool);
    if(strcmp(*value,"Enable") == 0)
    {
        *value = "1";
    }else{
        *value = "0";
    }

#endif
    return FAULT_CODE_OK;
}


int cpe_set_igd_dc_gamefast_enable(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if ((SMART_QOS_FUN == FYES) || (GAME_BOOST == FYES))

    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *prof = NULL;
    FunEnableEnum smartQosEn = FUN_DISABLE;
    struProfAlloc *profhead = NULL;
    int i, max;
    max = getWanSlotCount();
    if(strcmp(value, "1") == 0) {
        smartQosEn = FUN_ENABLE;
    }
    for(i = 1; i <= max; i++) {
        prof = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, i);
        if(prof != NULL && ProfInstIsFree(prof) == 0) {
            ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, i, &profhead);
            prof->smartQosEn = smartQosEn;
        }
    }

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    return FAULT_CODE_OK;
#endif
    return FAULT_CODE_9005;
}

int cpe_get_igd_dc_gamefast_uplinkrate(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if ((SMART_QOS_FUN == FYES) || (GAME_BOOST == FYES))
    name = "InternetGatewayDevice.config.interface.1.rxBand";
    get_method(cwmp,name,value,pool);
#endif
    return FAULT_CODE_OK;
}

int cpe_set_igd_dc_gamefast_uplinkrate(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if ((SMART_QOS_FUN == FYES) || (GAME_BOOST == FYES))
    name = "InternetGatewayDevice.config.interface.1.rxBand";
    return set_method(cwmp,name,value,length,callback_reg,FALSE);
#endif
    return FAULT_CODE_9005;
}
int cpe_get_igd_dc_gamefast_downlinkrate(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if ((SMART_QOS_FUN == FYES) || (GAME_BOOST == FYES))
    name = "InternetGatewayDevice.config.interface.1.txBand";
    get_method(cwmp,name,value,pool);
#endif
    return FAULT_CODE_OK;
}

int cpe_set_igd_dc_gamefast_downlinkrate(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if ((SMART_QOS_FUN == FYES) || (GAME_BOOST == FYES))
    name = "InternetGatewayDevice.config.interface.1.txBand";
    return set_method(cwmp,name,value,length,callback_reg,FALSE);
#endif
    return FAULT_CODE_9005;
}

