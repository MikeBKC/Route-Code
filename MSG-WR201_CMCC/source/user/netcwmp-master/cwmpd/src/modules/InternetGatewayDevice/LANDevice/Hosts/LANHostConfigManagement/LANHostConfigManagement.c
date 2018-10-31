int cpe_set_igd_lan_hosts_deniedMAC(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    MibProfileType mibType = MIB_PROF_MAC_FILTER;
    MibProfileType mibTypeGlobal = MIB_PROF_MAC_FILTER_GLOBAL;
    MacFilterProfile *prof = NULL;
    MacFilterGlobalProfile *profGlobal = NULL;
    struProfAlloc *profhead = NULL;
    int min, max, index = 0;
    MacAddr tmpMac;
    int  instIndex ;

    profGlobal = (MacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeGlobal, 0);
    if(ProfInstIsFree(profGlobal) == 0 && profGlobal->action == 1){

	ProfInstLowHigh(mibType, &max, &min);
	converMac12To6Byte(value, &tmpMac);
	for(index = min; index < max; index++) {
	    prof = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, index);
	    if(memcmp(tmpMac.addr,prof->mac.addr,6) == 0){
		break;
	    }
	}
#if 0
	instIndex = ProfGetInstIndexByName(mibType, value);
#endif
	if(index == max) 
	{
	    instIndex = ProfNewInst(mibType, value, FALSE);
	    ProfBackupByIndex(mibType, PROF_CHANGE_ADD, instIndex, &profhead);
	    prof= (MacFilterProfile*)ProfGetInstPointByIndex(mibType, instIndex);
	    memcpy(prof->mac.addr, tmpMac.addr, sizeof(prof->mac.addr));
	    ProfUpdate(profhead);/*给speedweb发消息*/
	    ProfFreeAllocList(profhead);
	    nvramWriteCommit();/*保存flash*/
	}
	return FAULT_CODE_OK;
    }
    return FAULT_CODE_9005;
}

int cpe_set_igd_lan_hosts_allowedMAC(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    MibProfileType mibType = MIB_PROF_MAC_FILTER;
    MibProfileType mibTypeGlobal = MIB_PROF_MAC_FILTER_GLOBAL;
    MacFilterProfile *prof = NULL;
    MacFilterGlobalProfile *profGlobal = NULL;
    struProfAlloc *profhead = NULL;
    int min, max, index = 0;
    MacAddr tmpMac;

    profGlobal = (MacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeGlobal, 0);
    if(ProfInstIsFree(profGlobal) == 0 && profGlobal->action == 1){
	ProfInstLowHigh(mibType, &max, &min);
	converMac12To6Byte(value, &tmpMac);

	for(index = min; index < max; index++) {
	    prof = (MacFilterProfile*)ProfGetInstPointByIndex(mibType, index);
	    if(memcmp(tmpMac.addr,prof->mac.addr,6) == 0){
		break;
	    }
	}
	if(index < max){
	    ProfBackupByIndex(mibType, PROF_CHANGE_DEL, index, &profhead);
	    ProfDelInstByIndex(mibType, index);    

	    ProfUpdate(profhead);/*给speedweb发消息*/
	    ProfFreeAllocList(profhead);
	    nvramWriteCommit();/*保存flash*/
	}
	return FAULT_CODE_OK;
    }
    return FAULT_CODE_9005;
}
int cpe_get_igd_lan_hosts_macfilter_status(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    

    MibProfileType mibTypeGlobal = MIB_PROF_MAC_FILTER_GLOBAL;
    MacFilterGlobalProfile *profGlobal = NULL;
    char *status = "0";

    profGlobal = (MacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeGlobal, 0);
    if(ProfInstIsFree(profGlobal) == 0 && profGlobal->head.active == 1){
	if(profGlobal->action == 1){
	    status = "2";
	}else{
	    status = "1";
	}
    }
    *value = status ;
    return FAULT_CODE_OK;
}


