#include <stdio.h>
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "uttfunction.h"
#include "mib.h"
#include "profacce.h"
#include "translate.h"
#include <uttMachine.h>

#ifdef CONFIG_WEVO_MFG

static void formWevoAte(webs_t wp, char_t *path, char_t *query)
{
    char sz_cmd_buf[64] = {0};
    char sz_lan_ipaddr[24] = {0};
    unsigned int lanIp;
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;

    websHeader(wp);
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
    if ((profIF != NULL) && (profIF->head.active != FALSE)) {
	lanIp = ntohl(profIF->ether.sta.ip);
	int2ip(lanIp, sz_lan_ipaddr);

	sprintf(sz_cmd_buf,"killall wevo_mfg;wevo_mfg -a %s &",sz_lan_ipaddr);

	system(sz_cmd_buf);

	websWrite(wp,T("load ated success."));
    }
    else
    {
	websWrite(wp,T("load ated fail."));
    }
    websFooter(wp);
    websDone(wp,200);
}

void formDefineWevoAte(void)
{
    websFormDefine(T("ate"), formWevoAte);
}
#endif
