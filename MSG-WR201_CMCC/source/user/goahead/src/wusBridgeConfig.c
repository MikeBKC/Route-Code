#include <stdio.h>
#include <unistd.h>
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "mib.h"
#include "profacce.h"
#include "uttfunction.h"
#if (BRIDGE == FYES)

static void formBridgeConfig(webs_t wp, char_t *path, char_t *query);
static int aspOutPutBridgeConfig(int eid, webs_t wp, int argc, char **argv);
/********************************************************************
 * ������ formDefineBridge
 * ���ܣ� ҳ���뽻��ת������
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� chen.bin
 * ������ 2013-01-29
 *******************************************************************/
void formDefineBridge(void)
{
    websAspDefine(T("aspOutPutBridgeConfig"), aspOutPutBridgeConfig);
    websFormDefine(T("formBridgeConfig"), formBridgeConfig);
}

/*******************************************************************
 *������ formBridgeConfig
 *���ܣ� ҳ���ύ����
 *������ 
 *���أ� ��
 *���ߣ� chen.bin
 *������ 2013-01-29
 ******************************************************************/
static void formBridgeConfig(webs_t wp, char_t *path, char_t *query)
{
	MibProfileType mibType = MIB_PROF_BRIDGE;
	BridgeProfile *prof = NULL;
	struProfAlloc *profhead = NULL;
	char_t *ip;
	char_t *mask;
	char_t *defaultGW;
	char_t *priDns;
	char_t *secDns;
	char_t *if0Enable;
	char_t *if1Enable;
	char_t *if2Enable;
	char_t *if3Enable;
	Uint32 brIp,brMask,brGW,brDns1,brDns2;

	ip = websGetVar(wp,T("ip"),T(""));
	mask = websGetVar(wp,T("mask"),T(""));
	defaultGW = websGetVar(wp,T("defaultGW"),T(""));
	priDns = websGetVar(wp,T("priDns"),T(""));
	secDns = websGetVar(wp,T("secDns"),T(""));
	if0Enable = websGetVar(wp,T("if0Enable"),T(""));
	if1Enable = websGetVar(wp,T("if1Enable"),T(""));
	if2Enable = websGetVar(wp,T("if2Enable"),T(""));
	if3Enable = websGetVar(wp,T("if3Enable"),T(""));


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
		if(strcmp(if0Enable,"1")==0)
		{
			prof->interface[0].enable = FUN_ENABLE;		
		}
		else
		{
			prof->interface[0].enable = FUN_DISABLE;		
		}
		if(strcmp(if1Enable,"1")==0)
		{
			prof->interface[1].enable = FUN_ENABLE;		
		}
		else
		{
			prof->interface[1].enable = FUN_DISABLE;		
		}
		if(strcmp(if2Enable,"1")==0)
		{
			prof->interface[2].enable = FUN_ENABLE;		
		}
		else
		{
			prof->interface[2].enable = FUN_DISABLE;		
		}
		if(strcmp(if3Enable,"1")==0)
		{
			prof->interface[3].enable = FUN_ENABLE;		
		}
		else
		{
			prof->interface[3].enable = FUN_DISABLE;		
		}
		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
		nvramWriteCommit();
	}
	websRedirect(wp,"bridge.asp");
}




/********************************************************************
 * ������ aspOutPutBridgeConfig
 * ���ܣ� ҳ����ʾ����
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� chen.bin
 * ������ 2013-01-29
 ********************************************************************/ 
static int aspOutPutBridgeConfig(int eid, webs_t wp, int argc, char **argv)
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
		websWrite(wp,"var if0Enable=%d;",prof->interface[0].enable);
		websWrite(wp,"var if1Enable=%d;",prof->interface[1].enable);
		websWrite(wp,"var if2Enable=%d;",prof->interface[2].enable);
		websWrite(wp,"var if3Enable=%d;",prof->interface[3].enable);
	}
	else
	{
		websWrite(wp,"var brIP=\"192.168.1.1\";");
		websWrite(wp,"var brNM=\"255.255.255.255\";");
		websWrite(wp,"var brGW=\"0.0.0.0\";");
		websWrite(wp,"var brDNS1=\"0.0.0.0\";");
		websWrite(wp,"var brDNS2=\"0.0.0.0\";");
		websWrite(wp,"var if0Enable=1;");
		websWrite(wp,"var if1Enable=1;");
		websWrite(wp,"var if2Enable=0;");
		websWrite(wp,"var if3Enable=0;");
	}
    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

#endif
