#include	"uttMachine.h"
#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include	"argcmd.h"

/*��wusSSH.c����ΪwusDeviceLoginControl.c*/

#if ((SSH_CLI == FYES)||(HTTPS == FYES))
#define SWORD_DEBUG 0
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

void formDefineDeviceLoginControl(void);
static int32_t aspOutDeviceLoginControl(int32_t eid, webs_t wp, int32_t argc, char_t **argv);
static void formDeviceLoginControl(webs_t wp, char_t *path, char_t *query);

/********************************************************************
 * ������ aspOutDeviceLoginControl
 * ���ܣ� ҳ�����
 * ������ 2012-10-25
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/
static int32_t aspOutDeviceLoginControl(int32_t eid, webs_t wp, int32_t argc, char_t **argv) 
{
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    profSys = ProfGetInstPointByIndex(mibType, 0);

    if((profSys != NULL) && (ProfInstIsFree((Pointer)profSys) == 0)) 
    {
#if (SSH_CLI == FYES)
		websWrite(wp, "var SSHEnables =\"%d\";", profSys->SSH.SSHEn);
		websWrite(wp, "var SSHPorts = \"%d\";", profSys->SSH.Port);
#endif
#if (HTTPS == FYES)
		websWrite(wp, "var httpsEnables =\"%d\";", profSys->sysHttps.httpsEn);
#endif
    }

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

/********************************************************************
 * ������ formDeviceLoginControl
 * ���ܣ� ҳ�汣��
 * ������ 2012-10-25
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/
static void formDeviceLoginControl(webs_t wp, char_t *path, char_t *query) 
{
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    struProfAlloc *profhead = NULL;
#if (SSH_CLI == FYES)
    char_t *enable = NULL, *Port = NULL;
    enable = websGetVar(wp, "SSHEnable", T(""));
    Port = websGetVar(wp, "SSHPort", T(""));
    SWORD_PRINTF("%s------%d, enable = %s, Port = %s\n", __func__, __LINE__, enable, Port);
#endif
#if (HTTPS == FYES)
    char_t *httpsEnable = NULL;
    httpsEnable = websGetVar(wp, "HttpsEnable", T(""));
#endif

    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);

    if((profSys != NULL) && (ProfInstIsFree((Pointer)profSys) == 0)) 
    {
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
#if (SSH_CLI == FYES)	
	if (strcmp(enable, "on") == 0) 
	{/*����SSH�ű���ip����Ϣ*/
	    profSys->SSH.SSHEn = FUN_ENABLE;
	    (profSys->SSH.Port) = (Uint32)strtoul(Port, NULL, 10);
	} 
	else 
	{
	    profSys->SSH.SSHEn = FUN_DISABLE;
	}
#endif
#if (HTTPS == FYES)
	/*����ҳ���ȡֵ����https���ܵĿ���*/
	if (strcmp(httpsEnable, "on") == 0) 
	{
	    profSys->sysHttps.httpsEn = FUN_ENABLE;
	} 
	else 
	{
	    profSys->sysHttps.httpsEn = FUN_DISABLE;
	}
#endif
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
    }
    websRedirect(wp, "DeviceLoginControl.asp");
    return;
}

/********************************************************************
 * ������ formDefineDeviceLoginControl
 * ���ܣ� ����ҳ��ӿں���
 * ������ 2012-10-25
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/
void formDefineDeviceLoginControl(void)
{
    websAspDefine(T("aspOutDeviceLoginControl"), aspOutDeviceLoginControl);	    /*define asp function*/
    websFormDefine(T("formDeviceLoginControl"), formDeviceLoginControl);    /*define form function*/
}
#endif 
