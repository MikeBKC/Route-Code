int cpe_get_igd_dc_bizwifi_enable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if (MAKE_MONEY == FYES)
    name = "InternetGatewayDevice.config.makemoney.MakeMoneyEn.";
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


int cpe_set_igd_dc_bizwifi_enable(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if (MAKE_MONEY == FYES)
    name = "InternetGatewayDevice.config.makemoney.MakeMoneyEn.";
    if(strcmp(value,"1") == 0)
    {
        value = "Enable";
    }else{
        value = "Disable";
    }
    return set_method(cwmp,name,value,length,callback_reg,FALSE);
#endif
    return FAULT_CODE_9005;
}
int cpe_get_igd_dc_bizwifi_account(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if (MAKE_MONEY == FYES)
    name = "InternetGatewayDevice.config.makemoney.phone.";
    get_method(cwmp,name,value,pool);
#endif
    return FAULT_CODE_OK;
}


int cpe_set_igd_dc_bizwifi_account(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if (MAKE_MONEY == FYES)

    name = "InternetGatewayDevice.config.makemoney.isBind.";
    set_method(cwmp,name,"1",length,callback_reg,FALSE);
    name = "InternetGatewayDevice.config.makemoney.phone.";
    return set_method(cwmp,name,value,length,callback_reg,FALSE);
#endif
    return FAULT_CODE_9005;
}
int cpe_get_igd_dc_bizwifi_startTime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if (MAKE_MONEY == FYES)
    name = "InternetGatewayDevice.config.makemoney.TimeStart.";
    get_method(cwmp,name,value,pool);
#endif
    return FAULT_CODE_OK;
}


int cpe_set_igd_dc_bizwifi_startTime(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if (MAKE_MONEY == FYES)
    name = "InternetGatewayDevice.config.makemoney.TimeStart.";
    return set_method(cwmp,name,value,length,callback_reg,FALSE);
#endif
    return FAULT_CODE_9005;
}
int cpe_get_igd_dc_bizwifi_endTime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if (MAKE_MONEY == FYES)
    name = "InternetGatewayDevice.config.makemoney.TimeStop.";
    get_method(cwmp,name,value,pool);
#endif
    return FAULT_CODE_OK;
}


int cpe_set_igd_dc_bizwifi_endTime(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if (MAKE_MONEY == FYES)
    name = "InternetGatewayDevice.config.makemoney.TimeStop.";
    return set_method(cwmp,name,value,length,callback_reg,FALSE);
#endif
    return FAULT_CODE_9005;
}
int cpe_get_igd_dc_bizwifi_price(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if (MAKE_MONEY == FYES)
    name = "InternetGatewayDevice.config.makemoney.charge.";
    get_method(cwmp,name,value,pool);
#endif
    return FAULT_CODE_OK;
}


int cpe_set_igd_dc_bizwifi_price(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if (MAKE_MONEY == FYES)
    name = "InternetGatewayDevice.config.makemoney.charge.";
    return set_method(cwmp,name,value,length,callback_reg,FALSE);
#endif
    return FAULT_CODE_9005;
}
int cpe_get_igd_dc_bizwifi_SSID_2_4_G(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if (MAKE_MONEY == FYES)
    char * tmpvalue = NULL;
    char * valueIconv = NULL;
    name = "InternetGatewayDevice.config.makemoney.ssid.";
    get_method(cwmp,name,&tmpvalue,pool);
    if(tmpvalue != NULL){
	valueIconv = iconv_string(tmpvalue,"GBK","utf-8");
	if(valueIconv != NULL){
	    *value = PSTRDUP(valueIconv);
	    free(valueIconv);
	}
    }

#endif
    return FAULT_CODE_OK;
}


int cpe_set_igd_dc_bizwifi_SSID_2_4_G(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if (MAKE_MONEY == FYES)
    char * xtest = NULL;
    int  retf = FAULT_CODE_9005;
    name = "InternetGatewayDevice.config.makemoney.ssid.";
    xtest = iconv_string(value?value:"","utf-8","GBK");
    if(xtest != NULL){
	retf = set_method(cwmp,name,xtest,length,callback_reg,FALSE);
        free(xtest);
    }
    return retf; 
#endif
    return FAULT_CODE_9005;
}
int cpe_get_igd_dc_bizwifi_SSID_5_G(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if (MAKE_MONEY == FYES)
    char * tmpvalue = NULL;
    char * valueIconv = NULL;
    name = "InternetGatewayDevice.config.makemoney.ssid_5g.";
    get_method(cwmp,name,&tmpvalue,pool);
    if(tmpvalue != NULL){
	valueIconv = iconv_string(tmpvalue,"GBK","utf-8");
	if(valueIconv != NULL){
	    *value = PSTRDUP(valueIconv);
	    free(valueIconv);
	}
    }
#endif
    return FAULT_CODE_OK;
}


int cpe_set_igd_dc_bizwifi_SSID_5_G(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if (MAKE_MONEY == FYES)
    char * xtest = NULL;
    int  retf = FAULT_CODE_9005;
    name = "InternetGatewayDevice.config.makemoney.ssid_5g.";
    xtest = iconv_string(value?value:"","utf-8","GBK");
    if(xtest != NULL){
	retf = set_method(cwmp,name,xtest,length,callback_reg,FALSE);
        free(xtest);
    }
    return retf; 
#endif
    return FAULT_CODE_9005;
}
int cpe_get_igd_dc_bizwifi_freeTime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if (MAKE_MONEY == FYES)
    name = "InternetGatewayDevice.config.makemoney.FreeTime.";
    get_method(cwmp,name,value,pool);
#endif
    return FAULT_CODE_OK;
}


int cpe_set_igd_dc_bizwifi_freeTime(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if (MAKE_MONEY == FYES)
    name = "InternetGatewayDevice.config.makemoney.FreeTime.";
    return set_method(cwmp,name,value,length,callback_reg,FALSE);
#endif
    return FAULT_CODE_9005;
}
int cpe_get_igd_dc_bizwifi_security(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if (MAKE_MONEY == FYES)
    name = "InternetGatewayDevice.config.makemoney.SaftyEn.";
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


int cpe_set_igd_dc_bizwifi_security(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if (MAKE_MONEY == FYES)
    name = "InternetGatewayDevice.config.makemoney.SaftyEn.";
    if(strcmp(value,"1") == 0)
    {
        value = "Enable";
    }else{
        value = "Disable";
    }
    return set_method(cwmp,name,value,length,callback_reg,FALSE);
#endif
    return FAULT_CODE_9005;
}
int cpe_get_igd_dc_bizwifi_upRate(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if (MAKE_MONEY == FYES)
    name = "InternetGatewayDevice.config.makemoney.uRate.";
    get_method(cwmp,name,value,pool);
#endif
    return FAULT_CODE_OK;
}


int cpe_set_igd_dc_bizwifi_upRate(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if (MAKE_MONEY == FYES)
    name = "InternetGatewayDevice.config.makemoney.uRate.";
    return set_method(cwmp,name,value,length,callback_reg,FALSE);
#endif
    return FAULT_CODE_9005;
}
int cpe_get_igd_dc_bizwifi_downRate(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if (MAKE_MONEY == FYES)
    name = "InternetGatewayDevice.config.makemoney.dRate.";
    get_method(cwmp,name,value,pool);
#endif
    return FAULT_CODE_OK;
}


int cpe_set_igd_dc_bizwifi_downRate(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if (MAKE_MONEY == FYES)
    name = "InternetGatewayDevice.config.makemoney.dRate.";
    return set_method(cwmp,name,value,length,callback_reg,FALSE);
#endif
    return FAULT_CODE_9005;
}

