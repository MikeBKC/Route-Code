int cpe_get_dc_configVersion(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    *value = "20150731152901";
    return FAULT_CODE_OK;
}
int cpe_get_igd_dc_passwd(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if (ADMIN_CONF == FYES)
    FUNCTION_TRACE();
    name = "InternetGatewayDevice.config.adminConf.password.";
    get_method(cwmp,name,value,pool);
#endif
    return FAULT_CODE_OK;
}


int cpe_set_igd_dc_passwd(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if (ADMIN_CONF == FYES)
    int  error;
    doSystem("killall goahead &");
    name = "InternetGatewayDevice.config.adminConf.password.";
    set_method(cwmp,name,value,length,callback_reg,FALSE);
    sleep(1);
    doSystem("goahead &");

    return FAULT_CODE_OK;
#endif
    return FAULT_CODE_9005;
}
int cpe_get_igd_dc_vpnnumber(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    static  char tmp[10];
    int num =0;
#if (PPTP_CLIENT == FYES)
    num++;
#endif
#if (L2TP_CLIENT == FYES)
    num++;
#endif
    snprintf(tmp,10,"%d",num);
    *value = tmp;
    return FAULT_CODE_OK;
}
int cpe_get_igd_dc_vpnenable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char *enable = "0"; 
#if (PPTP_CLIENT == FYES)
    MibProfileType pptpmibType = MIB_PROF_PPTP_CLI;
    PPTPCliProfile *pptpprof = NULL;
    pptpprof = (PPTPCliProfile *)ProfGetInstPointByName(pptpmibType, "VPN_PPTP");
    if (pptpprof != NULL && pptpprof->head.active == 1) {
	enable = "1" ;
    }
#endif
#if (L2TP_CLIENT == FYES)
    MibProfileType l2tpmibType = MIB_PROF_L2TP_CLIENT;
    L2TPCliProfile *l2tpprof = NULL;
    l2tpprof = (L2TPCliProfile *)ProfGetInstPointByName(l2tpmibType, "VPN_L2TP");
    if (l2tpprof != NULL && l2tpprof->head.active == 1) {
	enable = "2" ;
    }
#endif
    *value = enable;  
    return FAULT_CODE_OK;
}
int cpe_set_igd_dc_vpnenable(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    int instIndex  = 0;
    struProfAlloc *profhead = NULL;
    if(value == NULL){
	return FAULT_CODE_9005;
    }
#if (PPTP_CLIENT == FYES)
    MibProfileType pptpmibType = MIB_PROF_PPTP_CLI;
    FUNCTION_TRACE();
    PPTPCliProfile *pptpprof = NULL;
    FUNCTION_TRACE();
    if('2' == value[0]){
    FUNCTION_TRACE();
	instIndex = ProfGetInstIndexByName(pptpmibType, "VPN_PPTP");
	pptpprof = (PPTPCliProfile *)ProfGetInstPointByIndex(pptpmibType, instIndex);
	if (pptpprof != NULL && pptpprof->head.active == 1) {
	    ProfBackupByIndex(pptpmibType, PROF_CHANGE_EDIT, instIndex, &profhead);
	    pptpprof->head.active = 0;
	    ProfUpdate(profhead);
	    ProfFreeAllocList(profhead);
	    nvramWriteCommit();
	}
    }
#endif
#if (L2TP_CLIENT == FYES)
    MibProfileType l2tpmibType = MIB_PROF_L2TP_CLIENT;
    L2TPCliProfile *l2tpprof = NULL;
    if('1' == value[0]){
	instIndex = ProfGetInstIndexByName(l2tpmibType, "VPN_L2TP");
	l2tpprof = (L2TPCliProfile *)ProfGetInstPointByIndex(l2tpmibType, instIndex);
	if (l2tpprof != NULL && l2tpprof->head.active == 1) {
	    ProfBackupByIndex(l2tpmibType, PROF_CHANGE_EDIT, instIndex, &profhead);
	    l2tpprof->head.active = 0;
	    ProfUpdate(profhead);
	    ProfFreeAllocList(profhead);
	    nvramWriteCommit();
	}
    }
#endif
    return FAULT_CODE_OK;
}

