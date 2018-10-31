int cpe_detail_igd_lan_newhosts(cwmp_t * cwmp, const char * name,parameter_list_t ** ppl, pool_t * pool)
{    
#if(CLOUD_CONFIG == FYES)
    CloudConfigProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_CLOUD_CONFIG;
    char childName[128];
    char buffer[8];
    char *mac12 , *hostName = NULL;
    int index,j, i=0;

    NoticeMac * maclist = nvramProfile->noticeMacList;
    prof = (CloudConfigProfile*)ProfGetInstPointByIndex(mibType,0);

    if ((0 == ProfInstIsFree((Pointer)prof)))
    {
	for(index=0; index <MAX_NOTICE_MAC_LIST; index++ ){
	    if(1 == maclist[index].sendFlag){
		i++;
		snprintf(childName,128,"%s.Host.%d.MACAddress",name,i);
		// mac12 = converMac6To12Byte(nvramProfile->noticeMacList.macaddr);
		mac12 = converMac6To12Byte(maclist[index].macaddr);
		cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName,mac12);
		snprintf(childName,128,"%s.Host.%d.HostName",name,i);
		hostName = maclist[index].name;

		for(j =0 ; j < MAX_FRIEND_NUM ; j++){
		    if(memcmp(&maclist[index].macaddr,&prof->friendMac[j],sizeof(MacAddr)) == 0){
			hostName = prof->friendName[j];		
		    }
		}	
	    	cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName,hostName?hostName:"");
		maclist[index].sendFlag =0;
	    }
	}

	snprintf(buffer,8,"%d",i);
	snprintf(childName,128,"%s.HostNumberOfEntries",name);
	cwmp_add_parameter_to_list_by_nameValue(pool,  *ppl,childName,buffer);
	nvramProfile->needSendCount=0;	
    }
#endif
}
