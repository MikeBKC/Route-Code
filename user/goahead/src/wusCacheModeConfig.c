#include <stdio.h>
#include <unistd.h>
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "mib.h"
#include "profacce.h"
#include "uttfunction.h"
#if (BRIDGE == FYES)

static void formCacheModeConfig(webs_t wp, char_t *path, char_t *query);
static int aspOutPutCacheModeConfig(int eid, webs_t wp, int argc, char **argv);
/********************************************************************
 * ������ formDefineCacheMode
 * ���ܣ� ҳ���뽻��ת������
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� chen.bin
 * ������ 2013-04-07
 *******************************************************************/
void formDefineCacheMode(void)
{
    websAspDefine(T("aspOutPutCacheModeConfig"), aspOutPutCacheModeConfig);
    websFormDefine(T("formCacheModeConfig"), formCacheModeConfig);
}

/*******************************************************************
 *������ formCacheModeConfig
 *���ܣ� ҳ���ύ����
 *������ 
 *���أ� ��
 *���ߣ� chen.bin
 *������ 2013-04-07
 ******************************************************************/
static void formCacheModeConfig(webs_t wp, char_t *path, char_t *query)
{
	MibProfileType mibType = MIB_PROF_BRIDGE;
	BridgeProfile *prof = NULL;
	struProfAlloc *profhead = NULL;
	char_t *ip;
	char_t *mask;
	char_t *defaultGW;
	char_t *priDns;
	char_t *secDns;
	Uint32 brIp,brMask,brGW,brDns1,brDns2;

	ip = websGetVar(wp,T("ip"),T(""));
	mask = websGetVar(wp,T("mask"),T(""));
	defaultGW = websGetVar(wp,T("defaultGW"),T(""));
	priDns = websGetVar(wp,T("priDns"),T(""));
	secDns = websGetVar(wp,T("secDns"),T(""));


	brIp = ip2int(ip);
	brMask = ip2int(mask);
	brGW = ip2int(defaultGW);
	brDns1 = ip2int(priDns);
	brDns2 = ip2int(secDns);
	
	prof = (BridgeProfile *)ProfGetInstPointByIndex(mibType,0);
	if((prof != NULL)&&(ProfInstIsFree(prof)==0))
	{
		ProfBackupByIndex(mibType,PROF_CHANGE_EDIT,0,&profhead);
		prof->ip = htonl(brIp);
		prof->nm = htonl(brMask);
		prof->gw = htonl(brGW);
		prof->dns1 = htonl(brDns1);
		prof->dns2 = htonl(brDns2);
	
		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
		nvramWriteCommit();
	}
	websRedirect(wp,"netConfig.asp");
}




/********************************************************************
 * ������ aspOutPutCacheModeConfig
 * ���ܣ� ҳ����ʾ����
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� chen.bin
 * ������ 2013-04-07
 ********************************************************************/ 
static int aspOutPutCacheModeConfig(int eid, webs_t wp, int argc, char **argv)
{
	MibProfileType mibType = MIB_PROF_BRIDGE;
	BridgeProfile *prof = NULL;

	prof = (BridgeProfile *)ProfGetInstPointByIndex(mibType,0);
	if(prof!=NULL)
	{
		websWrite(wp,"var brIP=\"%N\";",prof->ip);
		websWrite(wp,"var brNM=\"%N\";",prof->nm);
		websWrite(wp,"var brGW=\"%N\";",prof->gw);
		websWrite(wp,"var brDNS1=\"%N\";",prof->dns1);
		websWrite(wp,"var brDNS2=\"%N\";",prof->dns2);
	}
	else
	{
		websWrite(wp,"var brIP=\"192.168.1.252\";");
		websWrite(wp,"var brNM=\"255.255.255.255\";");
		websWrite(wp,"var brGW=\"0.0.0.0\";");
		websWrite(wp,"var brDNS1=\"0.0.0.0\";");
		websWrite(wp,"var brDNS2=\"0.0.0.0\";");
	}
    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

#endif
