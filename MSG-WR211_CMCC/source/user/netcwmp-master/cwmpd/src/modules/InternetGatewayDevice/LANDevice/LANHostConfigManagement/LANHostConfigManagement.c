int cpe_get_l_igd_lan_lhcm_if_number(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
    static char tmp[5];
    int i=0;
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
    if (ProfInstIsFree(profIF) == 0 ){
	i++;
#if (MULTI_LAN == FYES)
	if(profIF->ether.sta.ip2 != 0){
	    i++;
	}
	if(profIF->ether.sta.ip3 != 0){
	    i++;
	}
	if(profIF->ether.sta.ip4 != 0){
	    i++;
	}
#endif
    }
    snprintf(tmp,4,"%d",i);
    *value  = tmp;
    return FAULT_CODE_OK;
}
