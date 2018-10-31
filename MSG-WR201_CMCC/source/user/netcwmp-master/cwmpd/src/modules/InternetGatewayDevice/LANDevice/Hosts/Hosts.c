#if 0
struct dhcpOfferedAddr {
    uint8_t hostname[16];
    uint8_t chaddr[16];
    uint32_t yiaddr;    /* network order */
    uint32_t expires;   /* host order */
};
struct dhcpOfferedAddr * init_leases(struct dhcpOfferedAddr *leases, int *length){
     FILE *lfp;
     struct dhcpOfferedAddr lease ;
     int i = 0,rsize,size=sizeof(lease);
     lfp = fopen("/var/udhcpd.leases", "r");
     if(lfp != NULL){
            while ((rsize = fread(&leases[i], size,1,lfp)) == 1){
		cwmp_log_debug("%s -- %d hostname = %s \n",__func__,__LINE__,leases[i].hostname);
		i++ ;
		if(i > 255)
		    break;
            }
	    printf(" rsize : %d ; size %d  errno : %d \n",rsize,size,errno);
	fclose(lfp);
    }
    *length  = i;
    return  leases ;
}
char * get_hostname_by_mac(struct dhcpOfferedAddr *leases,int len, unsigned char * mac){
    int i =0;
    for(;i<len;i++){
	if(memcmp(mac,leases[i].chaddr,6) == 0){
		return &(leases[i].hostname[0]);
	}
    }
    return NULL;
}
#endif
int cpe_detail_igd_lan_hosts(cwmp_t * cwmp, const char * name,parameter_list_t ** ppl, pool_t * pool)
{    
    int min, max, index = 0;
    int gbmin, gbmax, gbindex = 0;
    MibProfileType mibType = MIB_PROF_MAC_FILTER; 
    MacFilterProfile *prof = NULL; 
    MibProfileType mibTypeGlobal = MIB_PROF_MAC_FILTER_GLOBAL;
    MacFilterGlobalProfile *profGlobal = NULL;
    MibProfileType gbmibType = MIB_PROF_GROUP_BASE;
    GroupBaseType webGroupType = GBT_FINE_LIMIT;
    GroupBaseProfile *gbprof = NULL;  

    char buffer[20];
    char linestr[200];
    char ipAddr[20];
    char mac[20];
    FILE *fp,*tfp;
    char inbit[20];
    char outbit[20];
    int i = 0,j = 0, leases_len,readn=0;

    Uint64 buffer_int = 0U;
    char rxBytes[64], txBytes[64];
    char IPs[20];
    char iftype[20];
    int strLen = 50;
    char timeStr[50];
    char childName[128];
    unsigned long onlineTime = 0ul;

    time_t t;
#if 0
    struct dhcpOfferedAddr leases[256];
#endif
    unsigned char mac6[6];
    char * hostname;
    char * accessLimit = "0";
    
    char format[] = " %15[0-9.] %17[A-Fa-f0-9:] %lu %s %s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %s %s %s";//84
#if(CLOUD_CONFIG == FYES)
    NoticeMac * maclist = nvramProfile->noticeMacList;
#endif

    tfp=fopen("/proc/uptime","r");
    if(!tfp)
    {
	t  = time(0);
    }           
    else if(fscanf(tfp,"%ld",&t)==1)
    {           
	fclose(tfp);
    }       
    fclose(tfp); 

#if 0
    init_leases(leases,&leases_len);
#endif
    profGlobal = (MacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeGlobal, 0);
    ProfInstLowHigh(mibType, &max, &min);
#if (MACADDR_ADD_LIMIT == FYES) 
    ProfInstLowHigh(gbmibType, &gbmax, &gbmin);
#endif 
    fp = fopen("/proc/net/statsPerIp", "r");
    if(fp != NULL)
    {
	fgets(linestr, 200, fp);
//printf(" %s  --- %d  feof(fp) = %d \n",__func__,__LINE__,feof(fp));
//printf(" %s  --- %d  num = %d\n",__func__,__LINE__,(fscanf(fp, format, ipAddr, mac,&onlineTime,rxBytes,txBytes,inbit,outbit,iftype)));
	while (feof(fp) == 0 && ((readn = fscanf(fp, format, ipAddr, mac,&onlineTime,rxBytes,txBytes,inbit,outbit,iftype)) == 8))
	{
#if 1
	    if((rxBytes[0] == '0' && txBytes[0] == '0')|| strlen(mac) == 0 || strcmp(mac,"00:00:00:00:00:00") == 0 
		|| strlen(ipAddr) == 0 || strcmp(ipAddr,"0.0.0.0") == 0
		|| strcmp(ipAddr, "0.0.8.6") == 0){
		continue ;
	}
#endif

	i++;
	snprintf(childName,128,"%s.Host.%d.IPAddress",name,i);
	cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName,ipAddr);

	snprintf(childName,128,"%s.Host.%d.MACAddress",name,i);
	cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName,mac);

	snprintf(childName,128,"%s.Host.%d.HostName",name,i);
	sscanf(mac,"%02x:%02x:%02x:%02x:%02x:%02x",&mac6[0],&mac6[1],&mac6[2],&mac6[3],&mac6[4],&mac6[5]);
#if 0
	hostname =  get_hostname_by_mac(leases,leases_len,mac6);
#endif
#if (MACADDR_ADD_LIMIT == FYES) 
	for(j=0; j <MAX_NOTICE_MAC_LIST; j++ ){
	    if(memcmp(mac6,maclist[j].macaddr.addr,6) == 0){
		hostname = maclist[j].name;
	    }
	}
#endif
	cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName,hostname?hostname:"");
	hostname=NULL;

	snprintf(childName,128,"%s.Host.%d.AccessLimit",name,i);
	if(ProfInstIsFree(profGlobal) == 0 &&  profGlobal->head.active){

	    for (index = min; index < max; index++){
		prof = (MacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
		if (ProfInstIsFree(profGlobal) == 0)
		{
		    if(memcmp(mac6,prof->mac.addr,6) == 0){
			accessLimit = profGlobal->action?"1":"2";
		    }
		}
	    }
	}
	cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName,accessLimit);
	accessLimit = "0";
#if (MACADDR_ADD_LIMIT == FYES) 
	    for(gbindex = gbmin; gbindex < gbmax; gbindex++) {
		gbprof = (GroupBaseProfile*)ProfGetInstPointByIndex(gbmibType, gbindex);
		if(ProfInstIsFree(gbprof) == 0 && gbprof->groupType == webGroupType && gbprof->limitPolicy == 3) {
		    if(memcmp(mac6,gbprof->mac_addr.addr,6) == 0){
			snprintf(childName,128,"%s.Host.%d.SpeedLimit.UpMaxRate",name,i);
			snprintf(buffer,20,"%d",gbprof->uRate);
			cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName,buffer);

			snprintf(childName,128,"%s.Host.%d.SpeedLimit.DownMaxRate",name,i);
			snprintf(buffer,20,"%d",gbprof->dRate);
			cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName,buffer);
		    }
		}
	    }
#endif 

	snprintf(childName,128,"%s.Host.%d.InterfaceType",name,i);
	cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName,iftype);

	onlineTime = t - (time_t)onlineTime;
	snprintf(timeStr,50,"%lu",onlineTime);
	snprintf(childName,128,"%s.Host.%d.Uptime",name,i);
	cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName,timeStr);

	snprintf(childName,128,"%s.Host.%d.DownRate",name,i);
	cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName,inbit);

	snprintf(childName,128,"%s.Host.%d.UpRate",name,i);
	cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName,outbit);

	snprintf(childName,128,"%s.Host.%d.TotalBytesReceived",name,i);
	cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName,rxBytes);
    }
	snprintf(buffer,20,"%d",i);
	snprintf(childName,128,"%s.HostNumberOfEntries",name);
	cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName,buffer);
	fclose(fp);    
    }

    return FAULT_CODE_OK;
}
