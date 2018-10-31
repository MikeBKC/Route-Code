
#include	<stdlib.h>
#include	<sys/ioctl.h>
#include	<net/if.h>
#include	<net/route.h>
#include    <string.h>
#include    <dirent.h>
#include	"internet.h"
#include	"webs.h"
#include	"utils.h"
#include 	"firewall.h"
#include	"management.h"
#include	"station.h"
#include	"wireless.h"
#include	"uttfunction.h"
#include    "utt_conf_define.h"
#include    "md5.h"
#include	"linux/autoconf.h"  //kernel config
#include	"config/autoconf.h" //user config
#include	"mib.h"
#include	"profacce.h"
#include	"ifmisc.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include "wuScript.h"
#include "translate.h"

#if (IPV6_INTERFACE == FYES)
static int getIpv6Lan(int eid, webs_t wp, int argc, char **argv)
{
     int index=0;
     MibProfileType mibType = MIB_PROF_IPV6_INTERFACE;
     Ipv6InterfaceProfile *prof = NULL;
     prof = (Ipv6InterfaceProfile *)ProfGetInstPointByIndex(mibType , index);
     if((prof != NULL) && (ProfInstIsFree(prof) == 0))
     { 
	 websWrite(wp,"var ipv6LAN_addr = \"%s\";",prof->ipv6LAN_addr);
	 websWrite(wp,"var LANprefixLength = \"%d\";",prof->LANprefixLength);
        websWrite(wp,"var LanAutoEnabled = \"%d\";",prof->LanAutoEnabled);
     }
	 prof = (Ipv6InterfaceProfile *)ProfGetInstPointByIndex(mibType , 1);
	 if((prof != NULL) && (ProfInstIsFree(prof) == 0))
	 { 
	     websWrite(wp,"var ipv6WAN_addr = \"%s\";",prof->ipv6WAN_addr);
	     websWrite(wp,"var WANprefixLength = \"%d\";",prof->WANprefixLength);
	 }
         getTheErrorMsg(eid, wp, argc, argv);
         return 0;
}
static int getIpv6Wan(int eid, webs_t wp, int argc, char **argv)
{
     int i,totalrecs;
     MibProfileType mibType = MIB_PROF_IPV6_INTERFACE;
     Ipv6InterfaceProfile *prof = NULL,*prof1 = NULL,*prof2 = NULL;
     totalrecs = getWanSlotCount();
     websWrite(wp,"var ipv6WAN_addr = new Array();\n");        
     websWrite(wp,"var WANprefixLength = new Array();\n");        
     websWrite(wp,"var ipv6Gateway = new Array();\n");        
     websWrite(wp,"var ipv6Dns1 = new Array();\n");        
     websWrite(wp,"var ipv6Dns2 = new Array();\n");       
      
     websWrite(wp,"var PortNames = new Array();\n");       
     websWrite(wp,"var totalrecs = %d;\n",totalrecs);       
     for(i = 1; i <= totalrecs; i++) {
    
	 prof = (Ipv6InterfaceProfile *)ProfGetInstPointByIndex(mibType , i);
	 if((prof != NULL) && (ProfInstIsFree(prof) == 0))
	 {
	     websWrite(wp,"PortNames[%d] = %d;",i-1,prof->port);
	     websWrite(wp,"ipv6WAN_addr[%d] = \"%s\";",i-1,prof->ipv6WAN_addr);
	     websWrite(wp,"WANprefixLength[%d] = \"%d\";",i-1,prof->WANprefixLength);
	     websWrite(wp,"ipv6Gateway[%d] = \"%s\";",i-1,prof->ipv6Gateway);
	     websWrite(wp,"ipv6Dns1[%d] = \"%s\";",i-1,prof->ipv6Dns1);
	  	     websWrite(wp,"ipv6Dns2[%d] = \"%s\";",i-1,prof->ipv6Dns2);
	 }
     }
     prof = (Ipv6InterfaceProfile *)ProfGetInstPointByIndex(mibType , 0);
     if((prof != NULL) && (ProfInstIsFree(prof) == 0))
     { 
	 websWrite(wp,"var ipv6LAN_addr = \"%s\";",prof->ipv6LAN_addr);
	 websWrite(wp,"var LANprefixLength = \"%d\";",prof->LANprefixLength);
     }
     getTheErrorMsg(eid, wp, argc, argv);
     return 0;
}

static int Ipv6LanDel(webs_t wp, char_t *path, char_t *query)
{
   MibProfileType mibType = MIB_PROF_IPV6_INTERFACE;
   Ipv6InterfaceProfile *prof = NULL;
   struProfAlloc *profhead = NULL;
   prof = (Ipv6InterfaceProfile *)ProfGetInstPointByIndex(mibType , 0);
   if((prof != NULL) && (ProfInstIsFree(prof) == 0)) 
    {   
        ProfBackupByIndex(mibType, PROF_CHANGE_DEL, 0, &profhead); 
	ProfDelInstByIndex(mibType, 0);
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
    }
   websRedirect(wp, "ipv6LANConfig.asp");
   return 0;
}
static int formIpv6Lan(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_IPV6_INTERFACE;
    Ipv6InterfaceProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    char *ipv6LAN_addr = NULL,*LanAutoEnabled = NULL;
    int LANprefixLength;
    ipv6LAN_addr = websGetVar(wp, "ipv6LAN_addr", T(""));
    LanAutoEnabled = websGetVar(wp, "LanAutoEnabled", T(""));
    LANprefixLength = (int)strtoul(websGetVar(wp, T("LANprefixLength"), NULL),NULL,0);
    prof = (Ipv6InterfaceProfile *)ProfGetInstPointByIndex(mibType , 0);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0)) 
    { 
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
	if(strcmp(LanAutoEnabled,"on") == 0) {
	    prof->LanAutoEnabled = 1;
	}else{
	    prof->LanAutoEnabled = 0;
	}
	strcpy(prof->ipv6LAN_addr,ipv6LAN_addr); 
	prof->LANprefixLength = LANprefixLength; 
	ProfUpdate(profhead); 
	ProfFreeAllocList(profhead); 
	nvramWriteCommit(); 
    } 
    websRedirect(wp, "ipv6LANConfig.asp"); 
    return 0; 
}
 
static int Ipv6WanDel(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_IPV6_INTERFACE;
    Ipv6InterfaceProfile *prof = NULL;
    struProfAlloc *profhead = NULL;    
    prof = (Ipv6InterfaceProfile *)ProfGetInstPointByIndex(mibType , 1);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0)) 
    {   
	ProfBackupByIndex(mibType, PROF_CHANGE_DEL, 1, &profhead); 
	ProfDelInstByIndex(mibType, 1);
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
    }
    websRedirect(wp, "ipv6WANConfig.asp");
    return 0;

}
static int formIpv6Wan(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_IPV6_INTERFACE;
    Ipv6InterfaceProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    char *ipv6WAN_addr = NULL, *ipv6Gateway = NULL, *ipv6Dns1 = NULL, *ipv6Dns2 = NULL;
    int WANprefixLength, wanNo;
    wanNo = (int)strtoul(websGetVar(wp, T("PortName"), NULL),NULL,0);

    ipv6WAN_addr = websGetVar(wp, "ipv6WAN_addr", T(""));
    ipv6Gateway = websGetVar(wp, "ipv6Gateway", T(""));
    ipv6Dns1 = websGetVar(wp, "ipv6Dns1", T(""));
    ipv6Dns2 = websGetVar(wp, "ipv6Dns2", T(""));
    WANprefixLength = (int)strtoul(websGetVar(wp, T("WANprefixLength"), NULL),NULL,0);
    printf("wanNo = %d --%s--%d\n", wanNo,__func__,__LINE__);
    prof = (Ipv6InterfaceProfile *)ProfGetInstPointByIndex(mibType , wanNo);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0)) 
    { 
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, wanNo, &profhead);
        prof->head.active = 1; 
	strcpy(prof->ipv6WAN_addr,ipv6WAN_addr); 
	strcpy(prof->ipv6Gateway,ipv6Gateway); 
	strcpy(prof->ipv6Dns1,ipv6Dns1); 
	strcpy(prof->ipv6Dns2,ipv6Dns2); 
	prof->WANprefixLength = WANprefixLength; 
        prof->port = wanNo;
       printf("wanNo=%d,prof->port=%d\n",wanNo,prof->port);
	ProfUpdate(profhead); 
	ProfFreeAllocList(profhead); 
	nvramWriteCommit(); 
    }
     
    printf("WANprefixLength = %d --%s--%d\n", prof->WANprefixLength,__func__,__LINE__);
    websRedirect(wp, "ipv6WANConfig.asp"); 
    return 0; 
} 

extern void websDefineIpv6Interface(void) 
{ 
    websAspDefine(T("aspOutputIpv6Lan"),getIpv6Lan); 
    websAspDefine(T("aspOutputIpv6Wan"),getIpv6Wan); 
    websFormDefine(T("formIpv6Lan"),formIpv6Lan);     
    websFormDefine(T("formIpv6Wan"),formIpv6Wan);     
    websFormDefine(T("Ipv6LanDel"),Ipv6LanDel);     
    websFormDefine(T("Ipv6WanDel"),Ipv6WanDel);     
}   

#endif
