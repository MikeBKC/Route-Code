
#include <stdio.h>
#include <string.h>
#include <mib.h>
#include "typedef.h"
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "uttfunction.h"
#include "profacce.h"
#include "uttMachine.h"
#include "comExeFun.h"
#include "ipsetApi.h"
#include "translate.h"

#if (IPV6_ISATAP == FYES)



static int getIsatapInfo(int eid, webs_t wp, int argc, char **argv)
{
    int index=0;
    MibProfileType mibType = MIB_PROF_ISATAP;
    IsatapProfile *prof = NULL;
    prof = (IsatapProfile *)ProfGetInstPointByIndex(mibType , index);
    
    websWrite(wp,"var ISATAPEnabled = %d;",prof->ISATAPEnabled);
    websWrite(wp,"var ISATAP_IPv6Pre =\"%s\";",prof->ISATAP_IPv6Pre);
    websWrite(wp,"var ISATAP_IPv4Gat =\"%N\";",prof->ISATAP_IPv4Gat);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

static int formIsatap(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_ISATAP;
    IsatapProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    char *ISATAPEnabled = NULL, *ISATAP_IPv6Pre = NULL, *ISATAP_IPv4Gat = NULL;
    
    ISATAPEnabled = websGetVar(wp, "ISATAPEnabled",T("") );
    ISATAP_IPv6Pre = websGetVar(wp, "ISATAP_IPv6Pre", T(""));
    ISATAP_IPv4Gat = websGetVar(wp, "ISATAP_IPv4Gat", T(""));
    
    prof = (IsatapProfile *)ProfGetInstPointByIndex(mibType , 0);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0)) 
    { 
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead); 
	if(strcmp(ISATAPEnabled,"on") == 0){
	    prof->ISATAPEnabled = 1;
	}else{
	    prof->ISATAPEnabled = 0;
	}
	prof->ISATAP_IPv4Gat = inet_addr(ISATAP_IPv4Gat);
        strcpy(prof->ISATAP_IPv6Pre,ISATAP_IPv6Pre); 
	ProfUpdate(profhead); 
	ProfFreeAllocList(profhead); 
	nvramWriteCommit();
     
    } 
    websRedirect(wp, "isatap.asp"); 
    return 0; 
} 

extern void formDefineIsatapConfig(void) 
{ 
    websAspDefine(T("aspOutIsatapInfo"),getIsatapInfo); 
    websFormDefine(T("formIsatap"),formIsatap);    

}

#endif
