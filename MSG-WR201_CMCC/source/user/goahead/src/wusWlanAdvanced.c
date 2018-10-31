#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "mib.h"
#include "profacce.h"
#include "uttfunction.h"

#ifdef WIRE_ADVANCED_DBG 
#define WIRE_ADVANCED_PRINTF(fmt, args...)                printf(fmt, ## args)  
#else
#define WIRE_ADVANCED_PRINTF (fmt, args...)   
#endif

#if (WIRELESS == FYES)
/* goform/wirelessAdvanced */
static void wirelessAdvanced(webs_t wp, char_t *path, char_t *query)
{ 
    MibProfileType mibType = MIB_PROF_WIRELESS;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    WirelessProfile *prof= NULL;
    char_t *beacon, *dtim, *fragment, *rts,*short_preamble, *wmm_capable;

     int	wlan_mode=0;

    /*fetch from web input */
    beacon = websGetVar(wp, T("beacon"), T("100"));
    dtim = websGetVar(wp, T("dtim"), T("1"));
    fragment = websGetVar(wp, T("fragment"), T("2346"));
    rts = websGetVar(wp, T("rts"), T("2347"));
    short_preamble = websGetVar(wp, T("short_preamble"), T("0"));
    wmm_capable = websGetVar(wp, T("wmm_capable"), T("0"));
    printf("rts=%s, fragment=%s,beacon=%s,dtim=%s,short_preamble=%s,wmm_capable=%s\n",rts , fragment,beacon,dtim,short_preamble,wmm_capable);

    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 0);
    profAction = PROF_CHANGE_EDIT;
    ProfBackupByIndex(mibType, profAction, 0, &profhead);

    /* set to nvram */
    prof->advance.rts= strtoul(rts,0,10);
    prof->advance.beacon = strtoul(beacon,0,10);
    prof->advance.section = strtoul(fragment,0,10);
    prof->advance.dtim = strtoul(dtim,0,10);
    prof->advance.preamble= strtoul(short_preamble,0,10);
    prof->advance.wmm = strtoul(wmm_capable,0,10);

    wlan_mode =  prof->mBaseCf.apWorkMode;
    if (!strncmp(wmm_capable, "1", 2)) {
	if (wlan_mode < 5)
	    //nvram_bufset(RT2860_NVRAM, "TxBurst", "0");
	    prof->advance.TxBurst = 0;
    }
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    //initWlan();
    websRedirect(wp,"WlanAdvanced.asp");
}
static int getWirelessAdvancedConfig(int eid, webs_t wp, int argc, char_t **argv)
{

    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,0);
    if(prof == NULL){
	return 0;
    }
    websWrite(wp,"var rtss=\"%d\";",prof->advance.rts);
    websWrite(wp,"var fragments=\"%d\";",prof->advance.section);
    websWrite(wp,"var beacons=\"%d\";",prof->advance.beacon);
    websWrite(wp,"var dtims=\"%d\";",prof->advance.dtim);
    websWrite(wp,"var short_preambles=\"%d\";",prof->advance.preamble);
    websWrite(wp,"var wmm_capables=\"%d\";\n", prof->advance.wmm);
    getTheErrorMsg(eid, wp, argc, argv);  
    return 0;
}

#if (WIRELESS_5G == FYES)
static void wirelessAdvanced_5g(webs_t wp, char_t *path, char_t *query)
{ 
    MibProfileType mibType = MIB_PROF_WIRELESS;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    WirelessProfile *prof= NULL;
    char_t *beacon, *dtim, *fragment, *rts,*short_preamble, *wmm_capable;

     int	wlan_mode=0;

    /*fetch from web input */
    beacon = websGetVar(wp, T("beacon"), T("100"));
    dtim = websGetVar(wp, T("dtim"), T("1"));
    fragment = websGetVar(wp, T("fragment"), T("2346"));
    rts = websGetVar(wp, T("rts"), T("2347"));
    short_preamble = websGetVar(wp, T("short_preamble"), T("0"));
    wmm_capable = websGetVar(wp, T("wmm_capable"), T("0"));
    printf("rts=%s, fragment=%s,beacon=%s,dtim=%s,short_preamble=%s,wmm_capable=%s\n",rts , fragment,beacon,dtim,short_preamble,wmm_capable);

    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 1);
    profAction = PROF_CHANGE_EDIT;
    ProfBackupByIndex(mibType, profAction, 1, &profhead);

    /* set to nvram */
    prof->advance.rts= strtoul(rts,0,10);
    prof->advance.beacon = strtoul(beacon,0,10);
    prof->advance.section = strtoul(fragment,0,10);
    prof->advance.dtim = strtoul(dtim,0,10);
    prof->advance.preamble= strtoul(short_preamble,0,10);
    prof->advance.wmm = strtoul(wmm_capable,0,10);

    wlan_mode =  prof->mBaseCf.apWorkMode;
    if (!strncmp(wmm_capable, "1", 2)) {
	if (wlan_mode < 5)
	    //nvram_bufset(RT2860_NVRAM, "TxBurst", "0");
	    prof->advance.TxBurst = 0;
    }
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    //initWlan();
    websRedirect(wp,"WlanAdvanced_5g.asp");
}

static int getWirelessAdvancedConfig_5g(int eid, webs_t wp, int argc, char_t **argv)
{

    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,1);
    if(prof == NULL){
	return 0;
    }
    websWrite(wp,"var rtss=\"%d\";",prof->advance.rts);
    websWrite(wp,"var fragments=\"%d\";",prof->advance.section);
    websWrite(wp,"var beacons=\"%d\";",prof->advance.beacon);
    websWrite(wp,"var dtims=\"%d\";",prof->advance.dtim);
    websWrite(wp,"var short_preambles=\"%d\";",prof->advance.preamble);
    websWrite(wp,"var wmm_capables=\"%d\";\n", prof->advance.wmm);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
#endif
extern void formDefineWireAdvancedConfig(void){
    websAspDefine(T("aspOutWirelessAdvancedConfig"), getWirelessAdvancedConfig);
    websFormDefine(T("wirelessAdvanced"), wirelessAdvanced);

#if (WIRELESS_5G == FYES)
    websAspDefine(T("aspOutWirelessAdvancedConfig_5g"), getWirelessAdvancedConfig_5g);
    websFormDefine(T("wirelessAdvanced_5g"), wirelessAdvanced_5g);
#endif
}
#endif
