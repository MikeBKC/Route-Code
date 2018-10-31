#include <stdio.h>
#include <unistd.h>
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "mib.h"
#include "profacce.h"
#include "uttfunction.h"
#if (NET_MODE_SWITCH== FYES)

static void formNetModeSwitch(webs_t wp, char_t *path, char_t *query);
static int aspOutPutNetModeSwitch(int eid, webs_t wp, int argc, char **argv);
static int aspOutPutNetModeSwitchForLeft(int eid, webs_t wp, int argc, char **argv);
/********************************************************************
 * ������ formDefineNetModeSwitch
 * ���ܣ� ҳ���뽻��ת������
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� chen.bin
 * ������ 2013-01-29
 *******************************************************************/
void formDefineNetModeSwitch(void)
{
    websAspDefine(T("aspOutPutNetModeSwitch"), aspOutPutNetModeSwitch);
    websFormDefine(T("formNetModeSwitch"), formNetModeSwitch);
    websAspDefine(T("aspOutPutNetModeSwitchForLeft"), aspOutPutNetModeSwitchForLeft);
}

/*******************************************************************
 *������ formNetModeSwitch
 *���ܣ� ҳ���ύ����
 *������ 
 *���أ� ��
 *���ߣ� chen.bin
 *������ 2013-01-29
 ******************************************************************/
static void formNetModeSwitch(webs_t wp, char_t *path, char_t *query)
{
	char_t *netMode = NULL;
	MibProfileType mibType = MIB_PROF_NET_MODE_SWITCH;
	NetModeSwitchProfile *prof = NULL;
	int mode ;

	netMode = websGetVar(wp,T("netMode"),T(""));
	mode = (int)strtoul(netMode,0,10);
	
	prof = (NetModeSwitchProfile *)ProfGetInstPointByIndex(mibType,0);
	if(prof != NULL)
	{
		if((prof->bridgeEnable == 1)&&(mode==0))
		{
			prof->bridgeEnable = PROF_DISABLE;
			nvramWriteCommit();
			web_reload(wp,"netModeReload.asp");
		}
		else if ((prof->bridgeEnable==0)&&(mode==1))
		{
			prof->bridgeEnable = PROF_ENABLE;
			nvramWriteCommit();
			web_reload(wp,"netModeReload.asp");
		}
		else
		{
			web_reload(wp,"netModeSwitch.asp");
		}
	}
	return;
}


/********************************************************************
 * ������ aspOutPutNetModeSwitchForLeft
 * ���ܣ� ҳ����ʾ����
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� chen.bin
 * ������ 2013-01-29
 ********************************************************************/ 
static int aspOutPutNetModeSwitchForLeft(int eid, webs_t wp, int argc, char **argv)
{
	Uint32 mode = 0u;
	MibProfileType mibType = MIB_PROF_NET_MODE_SWITCH;
	NetModeSwitchProfile *prof = NULL;

	prof = (NetModeSwitchProfile *)ProfGetInstPointByIndex(mibType,0);
	if(prof!=NULL)
	{
		mode = prof->bridgeEnable;
	}		
	if(mode == PROF_ENABLE)
	{
		websWrite(wp,"<script src=\"js/br_config.js\" type=\"text/javascript\"></script>");
	}
	else
	{
		websWrite(wp,"<script src=\"js/config.js\" type=\"text/javascript\"></script>");
	}

    return 0;
}

/********************************************************************
 * ������ aspOutPutNetModeSwitch
 * ���ܣ� ҳ����ʾ����
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� chen.bin
 * ������ 2013-01-29
 ********************************************************************/ 
static int aspOutPutNetModeSwitch(int eid, webs_t wp, int argc, char **argv)
{
	Uint32 mode = 0u;
	MibProfileType mibType = MIB_PROF_NET_MODE_SWITCH;
	NetModeSwitchProfile *prof = NULL;

	prof = (NetModeSwitchProfile *)ProfGetInstPointByIndex(mibType,0);
	if(prof!=NULL)
	{
		mode = prof->bridgeEnable;
	}
    websWrite(wp,"var netMode=%d;",mode);
    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

#endif
