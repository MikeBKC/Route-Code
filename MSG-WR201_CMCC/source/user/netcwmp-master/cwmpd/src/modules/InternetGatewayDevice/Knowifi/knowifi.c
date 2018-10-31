#if (UTT_KNOWIFI == FYES)
static int get_knowifi_index(const char *name,int *id)
{
    char type[32];

    sscanf(name, "InternetGatewayDevice.config.knowifi.Users.%d.%s", id,type);

    return *id;
}
int cpe_refresh_igd_config_knowifi_updateDomain(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    int i = 0;
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
	    cwmp_log_info("knowifi refresh node, delete param %s\n", tmp_param->name);
	    tmp_node = tmp_param->next_sibling;
	    cwmp_model_delete_parameter(tmp_param);
	    tmp_param = tmp_node; 
	}
	child_param->next_sibling = NULL;
	parameter_node_t * knowifi_param;
	for (i = 0; i < MAX_KNOWIFI_DOMAIN_COUNT; i++)
	{
	    knowifi_param = NULL;
	    cwmp_model_copy_parameter(param_node, &knowifi_param, i);
	}
    
	cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);
    }
    return FAULT_CODE_OK;	
}
int cpe_refresh_igd_config_knowifi_upusers(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    int i = 0;
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
	    cwmp_log_info("knowifi refresh node, delete param %s\n", tmp_param->name);
	    tmp_node = tmp_param->next_sibling;
	    cwmp_model_delete_parameter(tmp_param);
	    tmp_param = tmp_node; 
	}
	child_param->next_sibling = NULL;
	parameter_node_t * knowifi_param;
	for (i = 1; i <= MAX_KNOWIFI_DOMAIN_COUNT; i++)
	{
	    knowifi_param = NULL;
	    cwmp_model_copy_parameter(param_node, &knowifi_param, i);
	}
    
	cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);
    }
    return FAULT_CODE_OK;	
}

unsigned char *str2mac(const char *macStr, unsigned char *mac)
{
    char *p;
    int i;

    if (strlen(macStr) == 17)
    {
        p = macStr;
        for (i=0; i<6; i++)
        {
            mac[i] = strtol(p, &p, 16);
            p++;
        }

    }
    else
    {
        memset(mac, 0, 6);
    }

    return mac;
}
static KnowifiDomainProfile *knowifiDomainProf_lookup(unsigned char *mac)
{
    MibProfileType mibType = MIB_PROF_KNOWIFI_DOMAIN;
    KnowifiDomainProfile *prof = NULL;
    int i = 0, max, min;
    unsigned char mac1[6] = {0};
    unsigned char mac2[6] = {0};
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++)
    {
        prof = (KnowifiDomainProfile*)ProfGetInstPointByIndex(mibType, i);
        if (prof != NULL && prof->head.active == TRUE)
        {
            if (mac && prof->mac)
            {
                str2mac(mac, mac1);
                str2mac(prof->mac, mac2);
                if (memcmp(mac1, mac2, 6) == 0)
                    return prof;
            }
        }
    }
    return NULL;
}

static void knowifiDomainStatusInit()
{
    MibProfileType mibType = MIB_PROF_KNOWIFI_DOMAIN;
    KnowifiDomainProfile *prof = NULL;
    int max = 0,min = 0,i = 0;
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++)
    {
        prof = (KnowifiDomainProfile*)ProfGetInstPointByIndex(mibType, i);
        if (prof != NULL && prof->head.active == TRUE)
        {
	    prof->status = 0;
        }
    }
}

int cpe_detail_igd_knowifi_users(cwmp_t * cwmp, const char * name,parameter_list_t ** ppl, pool_t * pool)
{
    FUNCTION_TRACE();

    char buffer[20];
    char linestr[200];
    char ipAddr[20];
    char mac[20];
    FILE *fp;
    char inbit[20];
    char outbit[20];
    int i = 0, j = 0, readn=0;
    char rxBytes[64], txBytes[64];
    char iftype[20];
    int strLen = 50;

    unsigned long onlineTime = 0ul;

    char childName[128] = {0};
    char conut[10] = {0};

    unsigned char mac6[6];
    char * hostname = NULL;
    char connType[32] = {0};
    char domainStr[128] = {0};

    int max = 0,min = 0;

    char format[] = " %15[0-9.] %17[A-Fa-f0-9:] %lu %s %s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %s %s %s"; //84

    MibProfileType mibType = MIB_PROF_KNOWIFI_DOMAIN;
    KnowifiDomainProfile *prof = NULL;

#if(CLOUD_CONFIG == FYES)
    NoticeMac * maclist = nvramProfile->noticeMacList;    
#endif

    knowifiDomainStatusInit();

    fp = fopen("/proc/net/statsPerIp", "r");
    if(fp != NULL)
    {
	fgets(linestr, 200, fp);
	while (feof(fp) == 0 && ((readn = fscanf(fp, format, ipAddr, mac,&onlineTime,rxBytes,txBytes,inbit,outbit,iftype)) == 8))
	{
	    if((rxBytes[0] == '0' && txBytes[0] == '0') || strlen(mac) == 0 || strcmp(mac,"00:00:00:00:00:00") == 0
		    || strlen(ipAddr) == 0 || strcmp(ipAddr,"0.0.0.0") == 0
		    || strcmp(ipAddr, "0.0.8.6") == 0){
		continue;
	    }
    
	    prof = knowifiDomainProf_lookup(mac);
	    if (prof)
	    {
		snprintf(domainStr,128,"%s", prof->domain);
		prof->status = 1;
	    }
	    else
	    {
		strcpy(domainStr, "");
	    }

	    i++;
	    snprintf(childName,128,"%s.%d.MACAddress", name, i);
	    cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName, mac);
	    sscanf(mac,"%02x:%02x:%02x:%02x:%02x:%02x",&mac6[0],&mac6[1],&mac6[2],&mac6[3],&mac6[4],&mac6[5]);
	    for(j=0; j <MAX_NOTICE_MAC_LIST; j++ ){
		if(memcmp(mac6,maclist[j].macaddr.addr,6) == 0){
		    hostname = maclist[j].name;
		    break;
		}
	    }
	    snprintf(childName,128,"%s.%d.HostName", name, i);
	    cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName,hostname?hostname:"");
	    if (prof)
	    {
		if (!hostname)
		    strcpy(prof->deviceName, "");
		else
		    strncpy(prof->deviceName, hostname, 128);
	    }
	    hostname = NULL;

	    snprintf(childName,128,"%s.%d.IPAddress", name, i);
	    cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName, ipAddr);

	    snprintf(childName,128,"%s.%d.Status", name, i);
	    cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName,"online");
	    onlineTime = 0;

	    snprintf(childName,128,"%s.%d.Conntype", name, i);
	    if (strcmp(iftype, "1") == 0)
		strcpy(connType, "wireless");	    
	    else
		strcpy(connType, "wired");	    
	    cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName, connType);

	    snprintf(childName,128,"%s.%d.Domain", name, i);
	
	    cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName, domainStr);
	}
	fclose(fp);
    }
    ProfInstLowHigh(mibType, &max, &min);
    for (j = min; j < max; j++)
    {
        prof = (KnowifiDomainProfile*)ProfGetInstPointByIndex(mibType, j);
        if (prof != NULL && prof->head.active == TRUE)
        {
	    if (prof->status == 0)
	    {
		i++;

		snprintf(childName,128,"%s.%d.MACAddress", name, i);
		cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName, prof->mac);

		snprintf(childName,128,"%s.%d.HostName", name, i);
		cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName, prof->deviceName);
		hostname = NULL;

		snprintf(childName,128,"%s.%d.IPAddress", name, i);
		cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName, prof->ip);

		snprintf(childName,128,"%s.%d.Status", name, i);
		cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName,"offline");

		snprintf(childName,128,"%s.%d.Conntype", name, i);
		if (prof->conntype == 1)
		    strcpy(connType, "wireless");
		else
		    strcpy(connType, "wired");	    
		cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName, connType);

		snprintf(childName,128,"%s.%d.Domain", name, i);
		cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName, prof->domain);
	    }
        }
    }
    sprintf(conut, "%d", i);
    snprintf(childName,128,"%s.UserNumberOfEntries", name);
    cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName, conut);

    return FAULT_CODE_OK;
}


static int knowifi_limitNum = -1;
static int knowifi_freeNum = -1;
static int knowifi_updateCount = 0;

static char *arp_get_by_mac(char *req_mac)
{
    FILE *proc=NULL;
    char ip[16];
    char lanIp[16];
    char lanMask[16];
    int ip_int, lanIp_int, lanMask_int;
    char mac[18];
    char type[10];
    char ifname[32];
    char *reply = NULL;
    char format[51] = " %15[0-9.] %*s %s %17[A-Fa-f0-9:] %*s %s %*s %*s";
    int ret = 0;

    char mactmp1[6] = {0};
    char mactmp2[6] = {0};
    char *p1 = NULL;
    char *p2 = NULL;
    int i = 0;
    getIfIp((char *)getLanIfName(), lanIp);
    getIfNetmask((char *)getLanIfName(), lanMask);
    lanIp_int = inet_addr(lanIp);
    lanMask_int = inet_addr(lanMask);

    if (!(proc = fopen("/proc/net/arp", "r"))) {
        return NULL;
    }

    /* Skip first line */
	 while (!feof(proc) && fgetc(proc) != '\n');

	 /* Find ip, copy mac in reply */
	 reply = NULL;
    reply = (char *)malloc(16);
    if(reply!=NULL) {
	memset(reply, 0, 16);
    if(strcmp(SYSLINUXDIR, ("linux-2.6.21.x")) != 0) {
	format[42] = '\0';
    }
    memset(mac, 0, sizeof(mac));
    while (!feof(proc) && (fscanf(proc, format, ip, type, mac, ifname) == 2)) {
	if (strcmp(type, "0x0") == 0 || strcmp(ifname, "br0"))
	{
	    memset(ip, 0, sizeof(ip));
	    continue;
	}
	p1 = mac;
	p2 = req_mac;
	for (i = 0; i < 6;i++)
	{
	    mactmp1[i] = strtol(p1, &p1, 16);
	    mactmp2[i] = strtol(p2, &p2, 16);
	    p1++;
	    p2++;
	}
	if (memcmp(mactmp1, mactmp2, 6) == 0) {
	    //reply = safe_strdup(mac);
	    
	    ip_int = inet_addr(ip);
	    if (ip_int & lanMask_int != lanIp_int & lanMask_int)
	    {
		memset(ip, 0, sizeof(ip));
		continue;
	    }

	    strcpy(reply, ip);
	    ret = 1;
	    break;
	}
	memset(ip, 0, sizeof(ip));
    }
    }

    fclose(proc);
    if(ret == 0) {
	if(reply != NULL) {
	    free(reply);
	    reply = NULL;
	}
    }

    return reply;
    
}

int knowifi_update_domain(cwmp_t * cwmp)
{
    char buf[1024];
    char bufTmp[32];
    char tmpname[128];
    char *modeStr = NULL;
    char *startTime = NULL;
    char *stopTime = NULL;
    char *weekdays = NULL;
    char *domain = NULL;
    char *username = NULL;
    char *usernameGBK = NULL;
    parameter_node_t * pn;
    char get_node[128] = {0};
    char *optMac = NULL;
    char ipTmp[16] = {0};
    int instIndex = 0;

    unsigned int mode = 0;
    unsigned long timeStart = 0;
    unsigned long timeEnd = 0;
    unsigned int week = 0;
    unsigned int weekValue = 0;
    char *weekStr = NULL;
    char *timeStrtmp = NULL;
    int updateTimeMode = 0;

    int limitNum = 0, freeNum = 0, i = 0, j = 0;

    MibProfileType mibType = MIB_PROF_KNOWIFI_DOMAIN;
    KnowifiDomainProfile *prof = NULL;
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);

    pn = cwmp_get_parameter_node(cwmp->root, "InternetGatewayDevice.config.knowifi.UpdateDomain.Mode");
    modeStr = pn->value?pn->value:"0";
    pn = cwmp_get_parameter_node(cwmp->root, "InternetGatewayDevice.config.knowifi.UpdateDomain.Name");
    domain = pn->value?pn->value:"";
    pn = cwmp_get_parameter_node(cwmp->root, "InternetGatewayDevice.config.knowifi.UpdateDomain.Username");
    usernameGBK = iconv_string(pn->value?pn->value:"","utf-8","GBK");
    if (usernameGBK != NULL)
    {
	username = usernameGBK;
    }
    else
    {
	username = pn->value?pn->value:"";
    }
    limitNum = knowifi_limitNum;
    freeNum = knowifi_freeNum;

    if (strcmp(modeStr, "task") == 0)
    {
	mode = KNOWIFI_MODE_MISSION;
    }
    else if (strcmp(modeStr, "time") == 0)
    {
	updateTimeMode = 1;
	mode = KNOWIFI_MODE_TIME;
    
	pn = cwmp_get_parameter_node(cwmp->root, "InternetGatewayDevice.config.knowifi.UpdateDomain.Weekdays");
	weekdays = pn->value?pn->value:"0";
	weekStr = strtok(weekdays, ",");
	while(weekStr)
	{
	    weekValue = strtoul(weekStr, NULL, 10);
	    if (weekValue <= 7 && weekValue >= 1)
	    {
		week |= (0x1 << (weekValue - 1));
	    }   
	    weekStr = strtok(NULL, ",");
	}

	pn = cwmp_get_parameter_node(cwmp->root, "InternetGatewayDevice.config.knowifi.UpdateDomain.StartTime");
	startTime = pn->value?pn->value:"0";
	weekStr = strtok(weekdays, ",");
	timeStrtmp = strtok(startTime, ":");
	timeStart += 3600 * strtoul(timeStrtmp, NULL, 10);
	timeStrtmp = strtok(NULL, ":");
	timeStart += 60 * strtoul(timeStrtmp, NULL, 10);

	pn = cwmp_get_parameter_node(cwmp->root, "InternetGatewayDevice.config.knowifi.UpdateDomain.StopTime");
	stopTime = pn->value?pn->value:"0";
	weekStr = strtok(weekdays, ",");
	timeStrtmp = strtok(stopTime, ":");
	timeEnd += 3600 * strtoul(timeStrtmp, NULL, 10);
	timeStrtmp = strtok(NULL, ":");
	timeEnd += 60 * strtoul(timeStrtmp, NULL, 10);
    }
    else
    {
	mode = KNOWIFI_MODE_FORBBIDEN;
    }

    memset(bufTmp, 0 ,sizeof(bufTmp));
    sprintf(buf, "isTimeMode:%d", updateTimeMode);
    int macNum = 0;
    /* Ìí¼Ódomain */

    uttSemP(SEM_KNOWIFI, 0);

    for (i = 0; i < limitNum; i++)
    {
	int isNewDev = 0;
	sprintf(get_node, "InternetGatewayDevice.config.knowifi.UpdateDomain.LimitMAC.%d.MACAddress", i);
	pn = cwmp_get_parameter_node(cwmp->root, get_node);
	optMac = pn->value?pn->value:"00:00:00:00:00:00";
	weekStr = strtok(weekdays, ",");
	instIndex = ProfGetInstIndexByName(mibType, optMac);
	if(instIndex == PROFFAIL)
	{
	    isNewDev = 1;
	    instIndex = ProfNewInst(mibType, optMac, FALSE);
	    if(instIndex == PROFFAIL) {
		return FAULT_CODE_9005; 
	    }	    
	}
	prof = (KnowifiDomainProfile*)ProfGetInstPointByIndex(mibType, instIndex);
	ProfSetNameByPoint(prof, optMac);
	strncpy(prof->username, username, 128);
	strncpy(prof->mac, optMac, 18);
	strncpy(prof->domain, domain, 128);
	if (isNewDev)
	{
	    prof->isFree = 0;
	    sprintf(ipTmp, "164.253.80.%d",  instIndex);
	    strcpy(prof->ip, ipTmp);
	}
	memset(prof->token, 0, sizeof(prof->token));
	prof->tokenOverTime = 0;
	prof->mode = mode;
	prof->timeStart = timeStart;
	prof->timeEnd = timeEnd;
	prof->weekdays = week;
	prof->head.active = TRUE;
	memset(bufTmp, 0 ,sizeof(bufTmp));
	sprintf(bufTmp, ";mac%d:%s", macNum, prof->mac);
	strcat(buf, bufTmp);
	macNum ++;
    }
    memset(bufTmp, 0 ,sizeof(bufTmp));
    sprintf(bufTmp, ";macNum:%d", macNum);
    strcat(buf, bufTmp);
    if (usernameGBK)
	free(usernameGBK);

    /* É¾³ýmac */
    for (i = 0; i < freeNum; i++)
    {
	sprintf(get_node, "InternetGatewayDevice.config.knowifi.UpdateDomain.FreeMAC.%d.MACAddress", i);
	pn = cwmp_get_parameter_node(cwmp->root, get_node);
	optMac = pn->value?pn->value:"00:00:00:00:00:00";
	weekStr = strtok(weekdays, ",");
	prof = knowifiDomainProf_lookup(optMac);
	if (prof)
	{
	    doSystem("ipset -D wifi_client_group1 %s",prof->ip);
	    doSystem("ipset -D knowifi_redirect_client  %s",prof->ip);
	    ProfDelInstByName(mibType, optMac);
	}
    }
    
    nvramWriteCommit();

    uttSemV(SEM_KNOWIFI, 0);


    knowifi_limitNum = -1;
    knowifi_freeNum = -1;
    knowifi_updateCount = 0;

    send_to_lan_2060p("addDev",buf);
    

    return FAULT_CODE_OK;
}

int cpe_set_igd_updatedomain_load(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    parameter_node_t * pn;
    pn = cwmp_get_parameter_node(cwmp->root, name);
    //printf("set node : %s, event count : %d\n", name,  cwmp->event_count);
    if(pn != NULL ){
	if(pn->value == NULL){
	    pn->value = (char *)pool_pcalloc(cwmp->pool, 128);
	    if(pn->value == NULL)
		return FAULT_CODE_9002;
	}
	pn->value_length = 128;
	snprintf(pn->value,128,"%s",value);
	if (strcmp(name, "InternetGatewayDevice.config.knowifi.UpdateDomain.LimitMACNumberOfEntries") == 0)
	    knowifi_limitNum = strtol(pn->value, NULL, 10);
	else if (strcmp(name, "InternetGatewayDevice.config.knowifi.UpdateDomain.FreeMACNumberOfEntries") == 0)
	    knowifi_freeNum = strtol(pn->value, NULL, 10);

	knowifi_updateCount ++;

	if (knowifi_limitNum >= 0 
	    && knowifi_freeNum >= 0 
	    && knowifi_updateCount >= (8 + knowifi_limitNum + knowifi_freeNum))
	{
	    return knowifi_update_domain(cwmp);
	}
	    
	return FAULT_CODE_OK;
    }
    return FAULT_CODE_9005;
}

static int knowifi_upCount = 0;
int knowifi_upUsers(cwmp_t * cwmp)
{
    int ret = !FAULT_CODE_OK;
    char buf[1024] = {0};
    char tmp[128] = {0};
    char *domain = NULL;
    char *freetime = NULL;
    char upNum = 0;
    char mac[32] = {0};
    int i = 0;
    
    parameter_node_t * pn;
    pn = cwmp_get_parameter_node(cwmp->root, "InternetGatewayDevice.config.knowifi.UpUserNumberOfEntries");    
    upNum = strtol(pn->value, NULL, 10);

    for(i = 1;i <= upNum; i++)
    {
	sprintf(tmp, "InternetGatewayDevice.config.knowifi.UpUsers.%d.Domain", i);
	pn = cwmp_get_parameter_node(cwmp->root, tmp);    
	domain = pn->value;
	sprintf(tmp, "InternetGatewayDevice.config.knowifi.UpUsers.%d.Freetime", i);
	pn = cwmp_get_parameter_node(cwmp->root, tmp);    
	freetime = pn->value;

	sprintf(buf, "domain:%s;freetime:%s", domain, freetime);	
	send_to_lan_2060p("upDomain",buf);

    }

    knowifi_upCount = 0;

    return FAULT_CODE_OK;
}
int cpe_set_igd_upusers_load(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    parameter_node_t * pn;
    pn = cwmp_get_parameter_node(cwmp->root, name);
    //printf("set node : %s, event count : %d\n", name,  cwmp->event_count);
    if(pn != NULL ){
	if(pn->value == NULL){
	    pn->value = (char *)pool_pcalloc(cwmp->pool, 128);
	    if(pn->value == NULL)
		return FAULT_CODE_9002;
	}
	pn->value_length = 128;
	snprintf(pn->value,128,"%s",value);

	knowifi_upCount ++;

	if (knowifi_upCount >= 3)
	{
	    return knowifi_upUsers(cwmp);
	}
	    
	return FAULT_CODE_OK;
    }
    return FAULT_CODE_9005;
}

int cpe_set_igd_knowifi_delDomain(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    char buf[1024] = {0};
    if (!value)
	return FAULT_CODE_9005;

    sprintf(buf, "domain:%s", value);	
    send_to_lan_2060p("delDomain",buf);
    return FAULT_CODE_OK;
}
int cpe_set_igd_knowifi_forbiddenUsers_num(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    return FAULT_CODE_OK;
}
int cpe_set_igd_knowifi_forbiddenUsers(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    char buf[1024] = {0};
    if (!value)
	return FAULT_CODE_9005;

    sprintf(buf, "domain:%s", value);	
    send_to_lan_2060p("downDomain",buf);
    return FAULT_CODE_OK;
}
int cpe_get_igd_knowifi_enable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
    if (prof->knowifi_enable == 1)
    {
	*value = "1";
    }
    else
    {
	*value = "0";
    }
    
    return FAULT_CODE_OK;
}
int cpe_set_igd_knowifi_enable(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    if (!value)
	return FAULT_CODE_9005;
    MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
    struProfAlloc *profhead = NULL;
#if (WEBAUTH_ACCOUNT == FYES)
        uttSemP(SEM_WEBAUTH_NO, 0);/*Óëspeedweb³åÍ»*/
#endif
    if((prof != NULL) && (ProfInstIsFree(prof) == 0))
    {
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
	if(strcmp(value,"1") == 0)
	{
	    prof->enabled = 1;
	    prof->authType = 2;
	    prof->knowifi_enable = 1;
	}
	else
	{
	    prof->enabled = 0;
	    prof->knowifi_enable = 0;
	}
	prof->head.active = TRUE;
    }

#if (WEBAUTH_ACCOUNT == FYES)
    uttSemV(SEM_WEBAUTH_NO, 0);/*Óëspeedweb³åÍ»*/
#endif
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit(); /*save*/
    
    return FAULT_CODE_OK;
}
#endif
