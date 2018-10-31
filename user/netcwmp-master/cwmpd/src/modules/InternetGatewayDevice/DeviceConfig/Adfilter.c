int cpe_get_igd_dc_adfilter_enable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    GroupBaseProfile *prof = NULL;
    int i = 0,max = 0, min = 0;
    SystemProfile *profSys = NULL;
    *value = "0";

    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    if(profSys->UttNfRuleEn == FUN_ENABLE){
	ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++) {
	    prof = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, i);
	    if (ProfInstIsFree(prof) == 0 && !strcmp(prof->head.name,"utt_action"))
	    {
		*value = "1";
		break;
	    }
	}

    }
    return FAULT_CODE_OK;
}


int cpe_set_igd_dc_adfilter_enable(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    char * GroupName = "utt_action";
    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    GroupBaseType webGroupType = GBT_ACT_CONTROL;
    GroupBaseProfile *profGroupBase = NULL;
    SystemProfile *profSys = NULL;
    struProfAlloc *profhead  = NULL,*profhead2  = NULL;
    int instIndex;

    FUNCTION_TRACE();
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    ProfBackupByIndex(MIB_PROF_SYSTEM, PROF_CHANGE_EDIT, 0, &profhead2);
    if(strcmp(value,"1") == 0)
    {
	instIndex = ProfGetInstIndexByName(mibType, GroupName);
	if(instIndex != PROFFAIL) 
	{
	    return FAULT_CODE_OK;
	}
	if(ProfCountInstByValue(mibType, OFFANDSIZE(GroupBaseProfile, groupType), 0u, (Pointer)&webGroupType) >= MAX_ACT_CONTROL_PROFILES)
	{
	    return FAULT_CODE_9005;
	}
	instIndex = ProfNewInst(mibType, GroupName, FALSE);
	ProfBackupByIndex(mibType, PROF_CHANGE_ADD, instIndex, &profhead);
	profGroupBase = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, instIndex);
	profGroupBase->groupType = webGroupType;/*记录类型方便不同的页面使用*/
	ProfSetNameByPoint(profGroupBase, GroupName);
	profSys->UttNfRuleEn = FUN_ENABLE;

    }else{
	profSys->UttNfRuleEn = FUN_DISABLE;
	ProfBackupByName(mibType, PROF_CHANGE_DEL, GroupName, &profhead);
	ProfDelInstByName(mibType, GroupName);    
    }

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    ProfUpdate(profhead2);
    ProfFreeAllocList(profhead2);
    nvramWriteCommit();

    return FAULT_CODE_OK;
}

