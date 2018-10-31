
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

#if (IPV6_TUNNEL == FYES)



static int getTunnelInfo(int eid, webs_t wp, int argc, char **argv)
{
    int index=0;
    MibProfileType mibType = MIB_PROF_TUNNEL;
    TunnelProfile *prof = NULL;
    prof = (TunnelProfile *)ProfGetInstPointByIndex(mibType , index);
    
printf("prof->tunnelEnabled = %d\n",prof->tunnelEnabled);
    websWrite(wp,"var tunnelEnabled = %d;",prof->tunnelEnabled);
    websWrite(wp,"var local_ip4_addr =\"%N\";",prof->local_ip4_addr);
    websWrite(wp,"var remote_ip4_addr =\"%N\";",prof->remote_ip4_addr);
    websWrite(wp,"var remote_ip6_addr =\"%s\";",prof->remote_ip6_addr);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

static int formTunnel(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_TUNNEL;
    TunnelProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    char *local_ip4_addr = NULL, *remote_ip4_addr = NULL, *remote_ip6_addr = NULL, *tunnelEnabled = NULL;
    
    local_ip4_addr = websGetVar(wp, "local_ip4_addr", T(""));
    remote_ip4_addr = websGetVar(wp, "remote_ip4_addr", T(""));
    remote_ip6_addr = websGetVar(wp, "remote_ip6_addr", T(""));
    tunnelEnabled = websGetVar(wp, "tunnelEnabled",T("") );
    printf("tunnelEnabled = %s --%d--%s\n",tunnelEnabled,__LINE__,__func__);
    
    prof = (TunnelProfile *)ProfGetInstPointByIndex(mibType , 0);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0)) 
    { 
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead); 
	if(strcmp(tunnelEnabled,"on") == 0){
	    prof->tunnelEnabled = 1;
            printf("****%s--%d---\n",__func__,__LINE__);
	}else{
	    prof->tunnelEnabled = 0;
	}
	prof->local_ip4_addr = inet_addr(local_ip4_addr);
	prof->remote_ip4_addr = inet_addr(remote_ip4_addr);
        strcpy(prof->remote_ip6_addr,remote_ip6_addr); 
printf("***prof->tunnelEnabled = %d\n",prof->tunnelEnabled);
	ProfUpdate(profhead); 
	ProfFreeAllocList(profhead); 
	nvramWriteCommit();
     
    } 
    websRedirect(wp, "tunnel.asp"); 
    return 0; 
} 

extern void formDefineTunnelConfig(void) 
{ 
    websAspDefine(T("aspOutTunnelInfo"),getTunnelInfo); 
    websFormDefine(T("formTunnel"),formTunnel);    

}

#endif
