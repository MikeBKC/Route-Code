
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

#if (IPV6_DHCPV6 == FYES)



static int getDhcp6Info(int eid, webs_t wp, int argc, char **argv)
{
    int index=0;
    MibProfileType mibType = MIB_PROF_DHCP6;
    Dhcp6Profile *prof = NULL;
    prof = (Dhcp6Profile *)ProfGetInstPointByIndex(mibType , index);
    websWrite(wp,"var dhcpEnabled = %d;",prof->dhcpEnabled);
    websWrite(wp,"var sta_ipv6_addr =\"%s\";",prof->sta_ipv6_addr);
    websWrite(wp,"var end_ipv6_addr =\"%s\";",prof->end_ipv6_addr);
    websWrite(wp,"var dhcpPrefixLength = %d;",prof->dhcpPrefixLength);
//    websWrite(wp,"var ipv6dhcpGateway =\"%s\";",prof->ipv6dhcpGateway);
    websWrite(wp,"var staleTime = %lu;",prof->staleTime);
    websWrite(wp,"var ipv6DnsZ =\"%s\";",prof->ipv6DnsZ);
    websWrite(wp,"var ipv6DnsB =\"%s\";",prof->ipv6DnsB);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

static int formDhcp6(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_DHCP6;
    Dhcp6Profile *prof = NULL;
    struProfAlloc *profhead = NULL;
    char *sta_ipv6_addr = NULL, *end_ipv6_addr = NULL, *ipv6DnsZ = NULL, *ipv6DnsB = NULL;
    char *dhcpEnabled = NULL;
    int dhcpPrefixLength;
    unsigned long staleTime;
    
    sta_ipv6_addr = websGetVar(wp, "sta_ipv6_addr", T(""));
    end_ipv6_addr = websGetVar(wp, "end_ipv6_addr", T(""));
//    ipv6dhcpGateway = websGetVar(wp, "ipv6dhcpGateway", T(""));
    ipv6DnsZ = websGetVar(wp, "ipv6DnsZ", T(""));
    ipv6DnsB = websGetVar(wp, "ipv6DnsB", T(""));
    dhcpEnabled = websGetVar(wp, "dhcpEnabled",T(""));
    dhcpPrefixLength = (int)strtoul(websGetVar(wp, T("dhcpPrefixLength"), NULL),NULL,0);
    staleTime = (unsigned long)strtoul(websGetVar(wp, T("staleTime"), NULL),NULL,0);
    
    prof = (Dhcp6Profile *)ProfGetInstPointByIndex(mibType , 0);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0)) 
    { 
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead); 
	if(strcmp(dhcpEnabled,"on") == 0){
	    prof->dhcpEnabled = 1;
	}else{
	    prof->dhcpEnabled = 0;
	}
	strcpy(prof->sta_ipv6_addr,sta_ipv6_addr); 
	strcpy(prof->end_ipv6_addr,end_ipv6_addr); 
//	strcpy(prof->ipv6dhcpGateway,ipv6dhcpGateway); 
	strcpy(prof->ipv6DnsZ,ipv6DnsZ); 
	strcpy(prof->ipv6DnsB,ipv6DnsB); 
        prof->dhcpPrefixLength = dhcpPrefixLength;
        prof->staleTime = staleTime;
	ProfUpdate(profhead); 
	ProfFreeAllocList(profhead); 
	nvramWriteCommit();
     
    } 
    websRedirect(wp, "DHCPv6.asp"); 
    return 0; 
} 

extern void websDefineDhcp6(void) 
{ 
    websAspDefine(T("aspOutDhcp6Info"),getDhcp6Info); 
    websFormDefine(T("formDhcp6"),formDhcp6);    

}

#endif
