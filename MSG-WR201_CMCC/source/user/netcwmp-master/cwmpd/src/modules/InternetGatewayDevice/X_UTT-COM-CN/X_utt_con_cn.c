int cpe_set_igd_utt_wirelessoptimize(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{	   
 #if(WIRELESS_5G == FYES)
    system("wlan_5g.sh");
#endif
#if(WIRELESS  == FYES)
    system("wlan.sh"); 
#endif
    sleep(2);  
    return FAULT_CODE_OK;
}
int cpe_get_igd_utt_onlinenotify(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    static char tmp[16];
#if(CLOUD_CONFIG == FYES)
    MibProfileType mibType = MIB_PROF_CLOUD_CONFIG;
    CloudConfigProfile* prof_cloud;
    prof_cloud = (CloudConfigProfile*)ProfGetInstPointByIndex(mibType, 0);
    if(ProfInstIsFree(prof_cloud) == 0)
    {
	snprintf(tmp,15,"%d",prof_cloud->noticeBanMask);
    }
    *value = tmp;
#endif
    return FAULT_CODE_OK;
}


int cpe_set_igd_utt_onlinenotify(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{	   
 #if(CLOUD_CONFIG == FYES)
    MibProfileType mibType = MIB_PROF_CLOUD_CONFIG;
    CloudConfigProfile* prof_cloud;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    if(value != NULL){
	prof_cloud = (CloudConfigProfile*)ProfGetInstPointByIndex(mibType, 0);
	if(ProfInstIsFree(prof_cloud) == 0)
	{
	    ProfBackupByIndex(mibType, profAction, 0, &profhead);
	    prof_cloud->noticeBanMask = strtol(value, NULL, 16);
	    ProfUpdate(profhead);
	    ProfFreeAllocList(profhead);
	    nvramWriteCommit();
	    return FAULT_CODE_OK;
	}
    }
#endif
    return FAULT_CODE_9005;
}

