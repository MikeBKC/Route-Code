#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include "utils.h"
#include "webs.h"
#include "firewall.h"
#include "internet.h"
#include "uttfunction.h"
#include "mib.h"
#include "profacce.h"
#include "translate.h"

#if (DMZ == FYES)

/***********************************************************************
 * 函 数 名：   getDMZIP
 * 创建日期：	2011-10-21
 * 修改日期：
 * 作者：       yqyang
 * 附加说明：	无
 ***********************************************************************/
static int getDMZIP(int eid, webs_t wp, int argc, char_t **argv)
{

    MibProfileType mibType = MIB_PROF_DMZ_NAT;
    DMZProfile *prof= NULL;
    prof = (DMZProfile*)ProfGetInstPointByIndex(mibType, 0);

    websWrite(wp,"var DMZIPs=\"%N\";",prof->DMZIP);
    websWrite(wp,"var DMZEnables=\"%d\";",prof->head.active);

    getTheErrorMsg(eid, wp, argc, argv);
#if (UTT_SMART_UI==FYES)
    getTheRightMsg(eid,wp,argc,argv);
#endif
    return 0;
}
/***********************************************************************
 * 函 数 名：   formDMZIP 
 * 创建日期：	2011-10-21
 * 修改日期：
 * 作者：       yqyang
 * 附加说明：	无
 ***********************************************************************/
static void formDMZIP(webs_t wp, char_t *path, char_t *query)
{
    char *dmzE, *ip_address;
    const char *errMsg=NULL;
    int dmzenable = 0;
    struct in_addr addr_ip;
    MibProfileType mibType = MIB_PROF_DMZ_NAT;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    DMZProfile *prof= NULL;

    dmzE = websGetVar(wp, T("DMZEnable"), T(""));
    ip_address = websGetVar(wp, T("DMZIP"), T(""));

    DBGPRINT("dmzE[%s] ip_address[%s]\n", dmzE,ip_address);
    if(inet_aton(ip_address, &addr_ip) == 0){  
	errMsg = getTransDataForC(C_LANG_INDEX_IP_PLAN_ADDR_ERR);
	setTheErrorMsg(errMsg);
	goto out;
    }
    if(strcmp(dmzE, "on") == 0)
    {
	dmzenable = 1;
    }
    profAction = PROF_CHANGE_EDIT;
    ProfBackupByIndex(mibType, profAction, 0, &profhead);
    prof = (DMZProfile*)ProfGetInstPointByIndex(mibType, 0);
    prof->head.active = dmzenable;
    prof->DMZIP = addr_ip.s_addr;
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, "DMZConfig.asp");
#else
    websRedirect(wp, "DMZ.asp");
#endif
}

void formDefineDMZConfig(void)
{

    websFormDefine(T("formDMZIP"),formDMZIP);
    websAspDefine(T("aspOutDMZIP"),getDMZIP);

}
#endif
