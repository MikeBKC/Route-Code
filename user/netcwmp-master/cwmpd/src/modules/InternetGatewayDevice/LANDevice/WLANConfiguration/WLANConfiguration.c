int  cpe_refresh_igd_wlanconfiguration(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
#if (WIRELESS == FYES)

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
            cwmp_log_info("refresh WLANConfiguration node, delete param %s\n", tmp_param->name);
            tmp_node = tmp_param->next_sibling;
            cwmp_model_delete_parameter(tmp_param);
            tmp_param = tmp_node;
        }
        child_param->next_sibling = NULL;

        int lan_index = get_index_after_paramname(param_node, "LANDevice"); 

        parameter_node_t * lanconn_param;

        lanconn_param = NULL;
        cwmp_model_copy_parameter(param_node, &lanconn_param, 1);

#if  (WIRELESS_5G == FYES)
        lanconn_param = NULL;
        cwmp_model_copy_parameter(param_node, &lanconn_param, 2);
#endif

        cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg); 
    }
#endif

    return FAULT_CODE_OK;
}
#if (WECHAT_CWMP == FYES)
int  cpe_refresh_igd_wlanconfiguration_ssid(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
#if (WIRELESS == FYES)

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
            cwmp_log_info("refresh WLANConfiguration node, delete param %s\n", tmp_param->name);
            tmp_node = tmp_param->next_sibling;
            cwmp_model_delete_parameter(tmp_param);
            tmp_param = tmp_node;
        }
        child_param->next_sibling = NULL;

        parameter_node_t *lanconn_param = NULL;
        cwmp_model_copy_parameter(param_node, &lanconn_param, 1);

#if (WLAN_MULTI_SSID == FYES)
        lanconn_param = NULL;
        cwmp_model_copy_parameter(param_node, &lanconn_param, 2);
#endif

        cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg); 
    }
#endif

    return FAULT_CODE_OK;
}
#endif
static int get_g_index(const char *name,int *id)
{
    int xg;
    char type[20];

#if (WECHAT_CWMP  == FYES)
    if(strstr(name,"SSID") == NULL){
	sscanf(name, "InternetGatewayDevice.LANDevice.1.WLANConfiguration.%d.%s", &xg,type);
    }else{
	sscanf(name, "InternetGatewayDevice.LANDevice.1.WLANConfiguration.%d.SSID.%d.%*s", &xg,id,type);
    }
#else
    sscanf(name, "InternetGatewayDevice.LANDevice.%d.WLANConfiguration.%d.%s", &xg,id,type);
#endif
#if (WLAN_MULTI_SSID == FYES)
    
#endif
    xg--;
    if(xg < 0)
    {
        return 0;
    }
    return xg;
}
int cpe_get_igd_lan_wlan_name(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if (WIRELESS == FYES)
    int id = 0;
    int index = get_g_index(name,&id);
    char *wirelessName = "2.4G";

#if (WIRELESS_5G == FYES)
 //   snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.active",index);
    if(index == 1){
	wirelessName = "5G";
    }
#endif
        *value = wirelessName;
#endif
    return FAULT_CODE_OK;
}
int cpe_get_igd_lan_wlan_ssidNuber(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if (WIRELESS == FYES)
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;
    int index=0, num=0;
#if (WIRELESS_5G == FYES)
    int id=0;
    index = get_g_index(name,&id);
#endif
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, index);
#if (WLAN_MULTI_SSID == FYES)
    if(strlen(prof->mBaseCf.SSID2) == 0){
       *value = "1";
    }else
#endif
    {
       *value = "2";
    }
#endif
    return FAULT_CODE_OK;
}
int cpe_get_igd_lan_wlan_enable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if (WIRELESS == FYES)
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};

    name = "InternetGatewayDevice.config.wireless.active.";
#if (WIRELESS_5G == FYES)
    snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.active",index);
    name = name_b;
#endif
    get_method(cwmp,name,value,pool);
    if(strcmp(*value,"Yes") == 0)
    {
        *value = "1";
    }else{
        *value = "0";
    }
#endif
    return FAULT_CODE_OK;
}

int cpe_set_igd_lan_wlan_enable(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if (WIRELESS == FYES)
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};
    name = "InternetGatewayDevice.config.wireless.active.";
#if (WIRELESS_5G == FYES)
    snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.active",index);
    name = name_b;
#endif
    if(strcmp(value,"1") == 0)
    {
        value = "Yes";
    }else{
        value = "No";
    }
    return set_method(cwmp,name,value,length,callback_reg,FALSE);
#endif
    return FAULT_CODE_9005;
}

int cpe_get_igd_lan_wlan_ssid(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if (WIRELESS == FYES)
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};
    char *xtest = NULL;
    char * tmpvalue = NULL;
    char * valueIconv = NULL;

    if(id == 1)
    {
        name = "InternetGatewayDevice.config.wireless.mbasecfg.ssid.";
    }
#if (WLAN_MULTI_SSID == FYES)
    else{
        name = "InternetGatewayDevice.config.wireless.mbasecfg.ssid2.";
    }
#endif

#if (WIRELESS_5G == FYES)
    if(id == 1)
    {
        snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.mbasecfg.ssid",index);
    }
#if (WLAN_MULTI_SSID == FYES)
    else{
        snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.mbasecfg.ssid2",index);
    }
#endif
    name = name_b;
#endif
    get_method(cwmp,name,&tmpvalue,pool);
    if(tmpvalue != NULL){
	valueIconv = iconv_string(tmpvalue,"GBK","utf-8");
	if(valueIconv != NULL){
	    *value = PSTRDUP(valueIconv);
	    free(valueIconv);
	}
    }
#if 0
    get_method(cwmp,name,value,pool);
    xtest = iconv_string(*value,"GBK","UTF-8");
#endif
#endif
    return FAULT_CODE_OK;
}
int cpe_set_igd_lan_wlan_ssid(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if (WIRELESS == FYES)
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};
    char * xtest = NULL;
    int  retf = 0;

    if(id == 1)
    {
        name = "InternetGatewayDevice.config.wireless.mbasecfg.ssid.";
    }
#if (WLAN_MULTI_SSID == FYES)
    else{
        name = "InternetGatewayDevice.config.wireless.mbasecfg.ssid2.";
    }
#endif

#if (WIRELESS_5G == FYES)
    if(id == 1)
    {
        snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.mbasecfg.ssid",index);
    }
#if (WLAN_MULTI_SSID == FYES)
    else{
        snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.mbasecfg.ssid2",index);
    }
#endif
    name = name_b;
#endif

    xtest = iconv_string(value?value:"","utf-8","GBK");
    if(xtest != NULL)
    {
        retf = set_method(cwmp,name,xtest,length,callback_reg,FALSE);
        free(xtest);
        return retf;
    }else{
        return FAULT_CODE_9007;
    }

#endif
    return FAULT_CODE_9005;
}
int cpe_get_igd_lan_wlan_channel(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if (WIRELESS == FYES)
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};

    name = "InternetGatewayDevice.config.wireless.mbasecfg.channel.";
#if (WIRELESS_5G == FYES)
    snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.mbasecfg.channel",index);
    name = name_b;
#endif
    get_method(cwmp,name,value,pool);
#endif
    return FAULT_CODE_OK;
}

int cpe_set_igd_lan_wlan_channel(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if (WIRELESS == FYES)
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};
    name = "InternetGatewayDevice.config.wireless.mbasecfg.channel.";
#if (WIRELESS_5G == FYES)
    snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.mbasecfg.channel",index);
    name = name_b;
#endif
    return set_method(cwmp,name,value,length,callback_reg,FALSE);
#endif
    return FAULT_CODE_9005;
}

int cpe_get_igd_lan_wlan_charset(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if (WIRELESS == FYES)
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};

    if(id == 1)
    {
        name = "InternetGatewayDevice.config.wireless.mbasecfg.encodeType.";
    }
#if (WLAN_MULTI_SSID == FYES)
    else{
        name = "InternetGatewayDevice.config.wireless.mbasecfg.encodeType2.";
    }
#endif

#if (WIRELESS_5G == FYES)
    if(id == 1)
    {
        snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.mbasecfg.encodeType.",index);
    }
#if (WLAN_MULTI_SSID == FYES)
    else{
        snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.mbasecfg.encodeType2.",index);
    }
#endif
    name = name_b;
#endif
    get_method(cwmp,name,value,pool);
#endif
    return FAULT_CODE_OK;
}

int cpe_set_igd_lan_wlan_charset(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if (WIRELESS == FYES)
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};

    if(id == 1)
    {
        name = "InternetGatewayDevice.config.wireless.mbasecfg.encodeType.";
    }
#if (WLAN_MULTI_SSID == FYES)
    else{
        name = "InternetGatewayDevice.config.wireless.mbasecfg.encodeType2.";
    }
#endif

#if (WIRELESS_5G == FYES)
    if(id == 1)
    {
        snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.mbasecfg.encodeType.",index);
    }
#if (WLAN_MULTI_SSID == FYES)
    else{
        snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.mbasecfg.encodeType2.",index);
    }
#endif
    name = name_b;
#endif
    set_method(cwmp,name,value,length,callback_reg,FALSE);
    return FAULT_CODE_OK;
#endif
    return FAULT_CODE_9005;
}

int cpe_get_igd_lan_wlan_basicencryptionmodes(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
#if (WIRELESS == FYES)
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};
    char value_b[100] = {0};

    name = "InternetGatewayDevice.config.wireless.msafecfg.SafeEncrypType.";
#if (WIRELESS_5G == FYES)
    snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.msafecfg.SafeEncrypType",index);
    name = name_b;
#endif
    get_method(cwmp,name,value,pool);
#endif
    return FAULT_CODE_OK;
}

int cpe_set_igd_lan_wlan_basicencryptionmodes(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if (WIRELESS == FYES)
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};
    char value_b[100] = {0};

    name = "InternetGatewayDevice.config.wireless.msafecfg.SafeEncrypType.";
#if (WIRELESS_5G == FYES)
    snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.msafecfg.SafeEncrypType",index);
    name = name_b;
#endif
    return set_method(cwmp,name,value,length,callback_reg,FALSE);
#endif
    return FAULT_CODE_9005;
}

int cpe_get_igd_lan_wlan_basicauthenticationmode(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
#if (WIRELESS == FYES)
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};
    char value_b[100] = {0};
    char  *beaconType;

    name = "InternetGatewayDevice.config.wireless.msafecfg.authMode.";
#if (WIRELESS_5G == FYES)
    snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.msafecfg.authMode",index);
    name = name_b;
#endif
    get_method(cwmp,name,&beaconType,pool);
    if(strcmp("OPEN",beaconType) != 0){
	*value = "WPA";
    }else{
	*value = "None";
    }
#if 0
    MibProfileType mibType = MIB_PROF_WIRELESS;
    struProfAlloc *profhead = NULL;
    WirelessProfile *prof= NULL;
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, index);
    if(prof != NULL){
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, index, &profhead);
    }
    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, profIndex, &profhead);
#endif
#endif
    return FAULT_CODE_OK;
}

int cpe_set_igd_lan_wlan_basicauthenticationmode(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if (WIRELESS == FYES)
    int id = 0;
    int index =0; 
#if (WIRELESS_5G == FYES)
    index = get_g_index(name,&id);
#endif
#if 0
    char name_b[100] = {0};
    char value_b[100] = {0};

    name = "InternetGatewayDevice.config.wireless.msafecfg.authMode.";
#if (WIRELESS_5G == FYES)
    sprintf(name_b,"InternetGatewayDevice.config.wireless.%d.msafecfg.authMode",index);
    name = name_b;
#endif
    if(strcmp(value,"None") != 0 ){
	value = "WPAPSKWPA2PSK";
    }else{
	value = "OPEN";
    }
    return set_method(cwmp,name,value,length,callback_reg,FALSE);
#endif
    MibProfileType mibType = MIB_PROF_WIRELESS;
    struProfAlloc *profhead = NULL;
    WirelessProfile *prof= NULL;
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, index);
    if(ProfInstIsFree(prof) == 0){
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, index, &profhead);
	if(strcmp(value,"None") == 0 ){
	    strcpy(prof->mSafeCf.SelAuthMode, "OPEN");
	    strcpy(prof->mSafeCf.ap.AuthMode.EncrypType,"NONE");
	}else{
	    strcpy(prof->mSafeCf.SelAuthMode, "WPAPSKWPA2PSK");
	    strcpy(prof->mSafeCf.ap.AuthMode.EncrypType,"TKIPAES");
	}
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
    }
    return FAULT_CODE_OK;
#endif
    return FAULT_CODE_9005;
}

int cpe_get_igd_lan_wlan_wpakey(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
#if (WIRELESS == FYES)
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};
    char value_b[100] = {0};
    char * tmpvalue = NULL;
    char * valueIconv = NULL;

    name = "InternetGatewayDevice.config.wireless.msafecfg.SafepskPsswd.";
#if (WIRELESS_5G == FYES)
    sprintf(name_b,"InternetGatewayDevice.config.wireless.%d.msafecfg.SafepskPsswd.",index);
    name = name_b;
#endif
    get_method(cwmp,name,&tmpvalue,pool);
    if(tmpvalue != NULL){
	valueIconv = iconv_string(tmpvalue,"GBK","utf-8");
	if(valueIconv != NULL){
	    *value = PSTRDUP(valueIconv);
	    free(valueIconv);
	}
    }
#if 0
    get_method(cwmp,name,value,pool);
#endif
#endif
    return FAULT_CODE_OK;
}

int cpe_set_igd_lan_wlan_wpakey(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if (WIRELESS == FYES)
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};
    char value_b[100] = {0};
    char * xtest = NULL;
    int  retf = 0;

    name = "InternetGatewayDevice.config.wireless.msafecfg.SafepskPsswd.";
#if (WIRELESS_5G == FYES)
    sprintf(name_b,"InternetGatewayDevice.config.wireless.%d.msafecfg.SafepskPsswd.",index);
    name = name_b;
#endif
    xtest = iconv_string(value?value:"","utf-8","GBK");
    if(xtest != NULL)
    {
        retf = set_method(cwmp,name,xtest,length,callback_reg,FALSE);
        free(xtest);
        return retf;
    }else{
        return FAULT_CODE_9007;
    }

#if 0
    return set_method(cwmp,name,value,length,callback_reg,FALSE);
#endif
#endif
    return FAULT_CODE_9005;
}

int cpe_get_igd_lan_wlan_wpa_timeout(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
#if (WIRELESS == FYES)
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};
    char value_b[100] = {0};

    name = "InternetGatewayDevice.config.wireless.msafecfg.Saferekeyinterval.";
#if (WIRELESS_5G == FYES)
    sprintf(name_b,"InternetGatewayDevice.config.wireless.%d.msafecfg.Saferekeyinterval.",index);
    name = name_b;
#endif
    get_method(cwmp,name,value,pool);
#endif
    return FAULT_CODE_OK;
}

int cpe_set_igd_lan_wlan_wpa_timeout(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if (WIRELESS == FYES)
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};
    char value_b[100] = {0};

    name = "InternetGatewayDevice.config.wireless.msafecfg.Saferekeyinterval.";
#if (WIRELESS_5G == FYES)
    sprintf(name_b,"InternetGatewayDevice.config.wireless.%d.msafecfg.Saferekeyinterval.",index);
    name = name_b;
#endif
    return set_method(cwmp,name,value,length,callback_reg,FALSE);
#endif
    return FAULT_CODE_9005;
}
#if (WECHAT_CWMP  == FYES)
int cpe_get_igd_lan_wlan_timeOffEnable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if ((UTT_SMART_UI == FYES) && (WIRELESS == FYES) && (WLAN_WORK_TIME == FYES))
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};

    name = "InternetGatewayDevice.config.wireless.mbasecfg.WlanTimeEn.";
#if (WIRELESS_5G == FYES)
    snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.mbasecfg.WlanTimeEn",index);
    name = name_b;
#endif
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


int cpe_set_igd_lan_wlan_timeOffEnable(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if ((UTT_SMART_UI == FYES) && (WIRELESS == FYES) && (WLAN_WORK_TIME == FYES))
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};
    name = "InternetGatewayDevice.config.wireless.mbasecfg.WlanTimeEn.";
#if (WIRELESS_5G == FYES)
    snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.mbasecfg.WlanTimeEn",index);
    name = name_b;
#endif
#if  1
    if(strcmp(value,"1") == 0)
    {
        value = "Enable";
    }else{
        value = "Disable";
    }
#endif
    return set_method(cwmp,name,value,length,callback_reg,FALSE);
#endif
    return FAULT_CODE_9005;
}

int cpe_get_igd_lan_wlan_effectiveDate(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if ((UTT_SMART_UI == FYES) && (WIRELESS == FYES) && (WLAN_WORK_TIME == FYES))
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};

    name = "InternetGatewayDevice.config.wireless.mbasecfg.day.";
#if (WIRELESS_5G == FYES)
    snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.mbasecfg.day",index);
    name = name_b;
#endif
    get_method(cwmp,name,value,pool);
#endif
    return FAULT_CODE_OK;
}


int cpe_set_igd_lan_wlan_effectiveDate(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if ((UTT_SMART_UI == FYES) && (WIRELESS == FYES) && (WLAN_WORK_TIME == FYES))
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};
    name = "InternetGatewayDevice.config.wireless.mbasecfg.day.";
#if (WIRELESS_5G == FYES)
    snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.mbasecfg.day",index);
    name = name_b;
#endif
    return set_method(cwmp,name,value,length,callback_reg,FALSE);
#endif
    return FAULT_CODE_9005;
}

int cpe_get_igd_lan_wlan_weekdayStartTime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if ((UTT_SMART_UI == FYES) && (WIRELESS == FYES) && (WLAN_WORK_TIME == FYES))
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};

    name = "InternetGatewayDevice.config.wireless.mbasecfg.weekdayTimeStart.";
#if (WIRELESS_5G == FYES)
    snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.mbasecfg.weekdayTimeStart",index);
    name = name_b;
#endif
    get_method(cwmp,name,value,pool);
#endif
    return FAULT_CODE_OK;
}


int cpe_set_igd_lan_wlan_weekdayStartTime(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if ((UTT_SMART_UI == FYES) && (WIRELESS == FYES) && (WLAN_WORK_TIME == FYES))
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};
    name = "InternetGatewayDevice.config.wireless.mbasecfg.weekdayTimeStart.";
#if (WIRELESS_5G == FYES)
    snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.mbasecfg.weekdayTimeStart",index);
    name = name_b;
#endif
    return set_method(cwmp,name,value,length,callback_reg,FALSE);
#endif
    return FAULT_CODE_9005;
}

int cpe_get_igd_lan_wlan_weekdayEndTime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if ((UTT_SMART_UI == FYES) && (WIRELESS == FYES) && (WLAN_WORK_TIME == FYES))
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};

    name = "InternetGatewayDevice.config.wireless.mbasecfg.weekdayTimeStop.";
#if (WIRELESS_5G == FYES)
    snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.mbasecfg.weekdayTimeStop",index);
    name = name_b;
#endif
    get_method(cwmp,name,value,pool);
#endif
    return FAULT_CODE_OK;
}


int cpe_set_igd_lan_wlan_weekdayEndTime(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if ((UTT_SMART_UI == FYES) && (WIRELESS == FYES) && (WLAN_WORK_TIME == FYES))
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};
    name = "InternetGatewayDevice.config.wireless.mbasecfg.weekdayTimeStop.";
#if (WIRELESS_5G == FYES)
    snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.mbasecfg.weekdayTimeStop",index);
    name = name_b;
#endif
    return set_method(cwmp,name,value,length,callback_reg,FALSE);
#endif
    return FAULT_CODE_9005;
}

int cpe_get_igd_lan_wlan_weekendStartTime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if ((UTT_SMART_UI == FYES) && (WIRELESS == FYES) && (WLAN_WORK_TIME == FYES))
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};

    name = "InternetGatewayDevice.config.wireless.mbasecfg.weekendTimeStart.";
#if (WIRELESS_5G == FYES)
    snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.mbasecfg.weekendTimeStart",index);
    name = name_b;
#endif
    get_method(cwmp,name,value,pool);
#endif
    return FAULT_CODE_OK;
}


int cpe_set_igd_lan_wlan_weekendStartTime(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if ((UTT_SMART_UI == FYES) && (WIRELESS == FYES) && (WLAN_WORK_TIME == FYES))
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};
    name = "InternetGatewayDevice.config.wireless.mbasecfg.weekendTimeStart.";
#if (WIRELESS_5G == FYES)
    snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.mbasecfg.weekendTimeStart",index);
    name = name_b;
#endif
    return set_method(cwmp,name,value,length,callback_reg,FALSE);
#endif
    return FAULT_CODE_9005;
}

int cpe_get_igd_lan_wlan_weekendEndTime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if ((UTT_SMART_UI == FYES) && (WIRELESS == FYES) && (WLAN_WORK_TIME == FYES))
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};

    name = "InternetGatewayDevice.config.wireless.mbasecfg.weekendTimeStop.";
#if (WIRELESS_5G == FYES)
    snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.mbasecfg.weekendTimeStop",index);
    name = name_b;
#endif
    get_method(cwmp,name,value,pool);
#endif
    return FAULT_CODE_OK;
}


int cpe_set_igd_lan_wlan_weekendEndTime(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
#if ((UTT_SMART_UI == FYES) && (WIRELESS == FYES) && (WLAN_WORK_TIME == FYES))
    int id = 0;
    int index = get_g_index(name,&id);
    char name_b[100] = {0};
    name = "InternetGatewayDevice.config.wireless.mbasecfg.weekendTimeStop.";
#if (WIRELESS_5G == FYES)
    snprintf(name_b,sizeof(name_b),"InternetGatewayDevice.config.wireless.%d.mbasecfg.weekendTimeStop",index);
    name = name_b;
#endif
    return set_method(cwmp,name,value,length,callback_reg,FALSE);
#endif
    return FAULT_CODE_9005;
}
int cpe_get_igd_lan_wlan_bssid(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
#if 0
    int id = 0;
    int index = get_g_index(name,&id);
#endif
    return FAULT_CODE_OK;
}
#endif
