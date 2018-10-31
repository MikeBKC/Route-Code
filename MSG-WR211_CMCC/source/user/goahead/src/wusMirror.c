


#include <stdio.h>
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "mib.h"
#include "profacce.h"
#include "uttfunction.h"
#if (FEATURE_SW_PORT_MIRROR == FYES)
#include "mibMirror.h"

char * mirror_port = "Port";
char * mirror_source[PORT_MAX_NUM+1] = { "R10","R11","R12","R13","R14","R15","R16","R17","R18","R19","R1a",
                                                "R1b","R1c","R1d","R1e","R1f","R20","R21","R22","R23","R24","R25","R26","R27",NULL};


//extern void formDefineMirrorUrcp(void);
void formDefineMirror(void);
void formMirrorNotRedirect(webs_t wp, char_t *path, char_t *query);
static void formMirror(webs_t wp, char_t *path, char_t *query);
static int getMirror(int eid, webs_t wp, int argc, char **argv);
/********************************************************************
 * ������ formDefineTopoyDiscovery
 * ���ܣ� ҳ���뽻��ת������
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� KeHuatao
 * ������ 2010-09-20
 *******************************************************************/
void formDefineMirror(void)
{
    websAspDefine(T("aspOutPutMirror"), getMirror);
    websFormDefine(T("formMirror"), formMirror);
#if 0
    formDefineMirrorUrcp();
#endif
}

/*******************************************************************
 *������ formMirrorNotRedirect
 *���ܣ� ҳ���ύ���(����תҳ��)
 *������ 
 *���أ� ��
 *���ߣ� KeHuatao/edit by bhou
 *������ 2010-10-12
 ******************************************************************/
void formMirrorNotRedirect(webs_t wp, char_t *path, char_t *query)
{
    char* stri;
    unsigned int  i, j = 0u, change = 0u;
    unsigned int  port_mask; 
	int profile_no = 0;
	struProfAlloc *profhead = NULL;
	ProfChangeType profAction = PROF_CHANGE_EDIT;
	MibProfileType mibType = MIB_PROF_MIRROR;
	MirrorProfile *prof = NULL;

	/* Get mirror profile according to profile_no */	
	prof = (MirrorProfile*)ProfGetInstPointByIndex(mibType, profile_no);
	/* Backup this profile */
	ProfBackupByIndex(mibType, profAction, profile_no, &profhead);

	if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0))
	{
		/*��ȡ����˿�*/
	    stri = websGetVar(wp, mirror_port, T(""));
		port_mask = (unsigned int)strtoul(stri, NULL, 10);
	    if(port_mask != prof->mirrorPort)
		{
			prof->mirrorPort = port_mask;
			change = 1u;
		}

		/*����Դ�˿�*/
		for(i=PORT_NO_START;i<PORT_NO_END+START_INDEX-1;i++)
	    {
			/*ȡҳ����ֵ*/
			stri = websGetVar(wp, mirror_source[j], T(""));
			j = j + 1u;
			if(strtoul(stri, NULL, 10) == 1u)
			{
				if(prof->monitoredPort[i] != SW_MIRROR_PORT_ENABLE)
				{
					/* read date from web page, if the condition is true ,
					 * save the moniteredPort to the profile.
					 */
					prof->monitoredPort[i] = SW_MIRROR_PORT_ENABLE;
					change = 1u;
				}
			}
			else
			{
				if(prof->monitoredPort[i] != SW_MIRROR_PORT_DISABLE)
				{
					prof->monitoredPort[i] = SW_MIRROR_PORT_DISABLE;
					change = 1u;
				}
			}
		}

	    /*�Ƿ����仯*/
		if(change==1u)
	    {
			prof->mirrorMode = MIRROR_EGRESS_INGRESS;
			ProfUpdate(profhead); /* update profile */
			ProfFreeAllocList(profhead); /* free alloc profile list */
			nvramWriteCommit();
		}
	}
}
/*******************************************************************
 *������ formMirror
 *���ܣ� ҳ���ύ����
 *������ 
 *���أ� ��
 *���ߣ� KeHuatao
 *������ 2010-10-12
 ******************************************************************/
static void formMirror(webs_t wp, char_t *path, char_t *query)
{
    formMirrorNotRedirect(wp, path,query);
    websRedirect(wp, "switch/mirror.asp");
}


/********************************************************************
 * ������ getMirror
 * ���ܣ� ҳ����ʾ����
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� KeHuatao
 * ������ 2010-10-12
 ********************************************************************/ 
static int getMirror(int eid, webs_t wp, int argc, char **argv)
{
	int profile_no = 0;
	unsigned i;
	MibProfileType mibType = MIB_PROF_MIRROR;
	MirrorProfile *prof;

    /*�˿���*/
    websWrite(wp, "var numPorts = %d;\n", PORT_MAX_NUM);

	prof=(MirrorProfile*)ProfGetInstPointByIndex(mibType, profile_no);
	if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0))
	{
		/*����˿�*/
    	websWrite(wp, "var mirrorPort = %d;\n", prof->mirrorPort);
	
		/* mirror mode : Disable, egress, ingress, egresss and ingress */
		websWrite(wp, "var mirrorMode = %d;\n", prof->mirrorMode);
	
		/*����Դ�˿�*/
    	websWrite(wp, "var mirrorSource = new Array(");
	

   		/*��������bit*/
	    for(i=PORT_NO_START;i<PORT_NO_END+START_INDEX-1; i++)
    	{
			if(i!=PORT_NO_START)
			{
	    		websWrite(wp, ",");
			}

			if(prof->monitoredPort[i] == SW_MIRROR_PORT_ENABLE)/*����*/
			{
			    websWrite(wp, "\"checked\"");
			}
			else/*����*/
			{
	    		websWrite(wp, "\"\"");
			}
    	}
	}
	else
	{
		websWrite(wp, "var mirrorPort = 0;\n");
		websWrite(wp, "var mirrorSource = new Array(");
		for(i=PORT_NO_START;i<PORT_NO_END+START_INDEX-1; i++)
		{
			if(i!=PORT_NO_START)
			{
				websWrite(wp, ",");
			}
			websWrite(wp, "\"\"");
		}
	}
    /*��β*/
    websWrite(wp, ");\n");
   
   	/*���������*/
    getAggrMember(wp);
    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

#endif
