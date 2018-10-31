#define     DOT     ","
#define     LOGOUT      "logout"
int cpe_get_igd_wifidog_downusers(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    return FAULT_CODE_OK;
}
int cpe_set_igd_wifidog_downusers(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    int ret = !FAULT_CODE_OK;
    char *buf = NULL;
    buf = (char*)malloc(strlen(value) + 10);

    sprintf(buf,"mac:%s",value);
    ret = send_to_lan_2060p(LOGOUT,buf);
    free(buf);
    return ret;
}
