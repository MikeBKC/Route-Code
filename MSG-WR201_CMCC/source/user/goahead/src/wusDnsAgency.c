
#include <string.h>
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "mib.h"
#include "profacce.h"
#include "uttfunction.h"
#include "wuScript.h"

#if (DNS_AGENCY == FYES)

static void formDnsAgency(webs_t wp, char_t *path, char_t *query)
{
	MibProfileType mibType = MIB_PROF_DNS_AGENCY;
	DnsAgencyProfile *prof = NULL;
	ProfChangeType profType = PROF_CHANGE_EDIT;
	struProfAlloc *profhead = NULL;
	char *dnspEnblw, *ispDns, *ispDns2;

	dnspEnblw = websGetVar(wp, "dnspEnblw", T(""));
	ispDns = websGetVar(wp, "ISPDns", T(""));
	ispDns2 = websGetVar(wp, "ISPDns2", T(""));

	prof = (DnsAgencyProfile *)ProfGetInstPointByIndex(mibType, 0);
	if (NULL != prof) {
		ProfBackupByIndex(mibType, profType, 0, &profhead);
		if (!strcmp("on", dnspEnblw)) {
			prof->DnsProxyEn = PROF_ENABLE;
		} else {
			prof->DnsProxyEn = PROF_DISABLE;
		}
		prof->pDns = inet_addr(ispDns);
		prof->sDns = inet_addr(ispDns2);

		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
		nvramWriteCommit();
	}

	websRedirect(wp, "DnsProxyForAc.asp");
	return;
}

static int getDnsAgency(int eid, webs_t wp, int argc, char **argv)
{
	MibProfileType mibType = MIB_PROF_DNS_AGENCY;
	DnsAgencyProfile *prof = NULL;

	prof = (DnsAgencyProfile *)ProfGetInstPointByIndex(mibType, 0);
	if (PROF_ENABLE == prof->DnsProxyEn) {
		websWrite(wp, "var dnspEnbls = \"%s\";\n", "Enable");
	} else {
		websWrite(wp, "var dnspEnbls = \"%s\";\n", "Disable");
	}
	websWrite(wp, "var ISPDnss = \"%N\";\n", prof->pDns);
	websWrite(wp, "var ISPDnss2 = \"%N\";\n", prof->sDns);

	getTheErrorMsg(eid, wp, argc, argv);
	return 0;
}

void formDefineDnsAgency(void)
{
	websAspDefine(T("aspOutPutDnsAgency"), getDnsAgency);
	websFormDefine(T("formDnsAgency"),formDnsAgency);
}

#endif
