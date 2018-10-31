#include	"uttMachine.h"
#include	"typedef.h"
#include	<mib.h>
#include    "mib.h"
#include    "wsIntrn.h"

#if (APP_POLICY_REMOTE_UPDATE == FYES)

extern void getAllAppType(int eid, webs_t wp, int argc, char **argv);
static void getFirmWareAppType(int eid, webs_t wp, int argc, char **argv);
static void getAdFilterAppType(int eid, webs_t wp, int argc, char **argv);

/*������е�APP����*/
void getAllAppType(int eid, webs_t wp, int argc, char **argv)
{
	websWrite(wp, "var SOFT_UDTYPE = %d;",SOFT_UDTYPE);
	websWrite(wp, "var ADFILTER_UDTYPE = %d;",ADFILTER_UDTYPE);
}

/*����̼�����APP����*/
static void getFirmWareAppType(int eid, webs_t wp, int argc, char **argv)
{
	websWrite(wp, "var SOFT_UDTYPE = %d;",SOFT_UDTYPE);
}

/*����̼�����APP����*/
static void getAdFilterAppType(int eid, webs_t wp, int argc, char **argv)
{
	websWrite(wp, "var ADFILTER_UDTYPE = %d;",ADFILTER_UDTYPE);
}
extern void websDefineAppType(void)
{
    websAspDefine(T("getAllAppType"), getAllAppType);
    websAspDefine(T("getFirmWareAppType"), getFirmWareAppType);
    websAspDefine(T("getAdFilterAppType"), getAdFilterAppType);
}
#else
static void getFirmWareAppType(int eid, webs_t wp, int argc, char **argv);
static void getFirmWareAppType(int eid, webs_t wp, int argc, char **argv)
{
	websWrite(wp, "var SOFT_UDTYPE = %d;",0);
}
extern void websDefineAppType(void)
{
    websAspDefine(T("getFirmWareAppType"), getFirmWareAppType);
}

#endif
