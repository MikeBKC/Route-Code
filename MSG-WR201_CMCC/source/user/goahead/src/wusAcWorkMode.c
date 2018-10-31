#include <stdio.h>
#include <unistd.h>
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "mib.h"
#include "profacce.h"
#include "uttfunction.h"
#include "urcp_msg.h"
#include "translate.h"

#include    <sys/ioctl.h>
#include    <arpa/inet.h> /*inet_addr*/
#include    <string.h>
#include    <dirent.h>
#include    <linux/autoconf.h>
#include    <uttMachine.h>
#include    "webmsg.h"
#include    "webs.h"

#if defined CONFIG_SWITCH_EXT || (FEATURE_AC == FYES)
#include    <switch.h>
#include    <sw_cfg.h>
#include    <urcp_ipc.h>
#endif



#if (AC_WORK_MODE == FYES)

static void formAcWorkMode(webs_t wp, char_t *path, char_t *query);
static int aspOutPutAcWorkMode(int eid, webs_t wp, int argc, char **argv);
/********************************************************************
 * ������ formDefineAcWorkMode
 * ���ܣ� ҳ���뽻��ת������
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� KeHuatao
 * ������ 2010-09-20
 *******************************************************************/
void formDefineAcWorkMode(void)
{
    websAspDefine(T("aspOutPutAcWorkMode"), aspOutPutAcWorkMode);
    websFormDefine(T("formAcWorkMode"), formAcWorkMode);
}

/********************************************************************
 * ������ cmd_ac_set_mode_send_msg 
 * ���ܣ� ����Ϣ��URCP����URCP�������AP
 * ������ ����ģʽ
 * ����� ��
 * ���أ� ��
 * ���ߣ� KeHuatao
 * ������ 2013-05-31
 *******************************************************************/
extern void cmd_ac_set_mode_send_msg(webs_t wp, char mode)
{
    urcp_ac_set_mode_msg_t urcp_ac_set_mode_msg;
    memset(&urcp_ac_set_mode_msg, 0, sizeof(struct _urcp_ac_set_mode_msg_t));

	urcp_ac_set_mode_msg.general_msg.user_ip = ip2int(wp->ipaddr);
	urcp_ac_set_mode_msg.general_msg.opt_type = MISC_SET_GENERAL;/* ������Ϣ���� */

    urcp_ac_set_mode_msg.ac_mode = mode;

	/*����Ϣ,֪ͨ���������̷���*/
	sendwebmsgbuffer(MSG_TYPE_URCP, URCP_CMD_SET_AC_MODE, (char*)(&urcp_ac_set_mode_msg), sizeof(struct _urcp_ac_set_mode_msg_t));
    return;

}
/*******************************************************************
 *������ formAcWorkMode
 *���ܣ� ҳ���ύ����
 *������
 *���أ� ��
 *���ߣ� KeHuatao
 *������ 2013-5-31
 ******************************************************************/
static void formAcWorkMode(webs_t wp, char_t *path, char_t *query)
{
    char *acMode = NULL,*ac_gateWay = NULL,*ac_dns = NULL;
    MibProfileType mibType = MIB_PROF_AC_WORK_MODE;
    AcWorkModeProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
	int flag = 0, tmp, status = 0;
    struct in_addr addr_dns,addr_gw;
    const char *errMsg=NULL;

    acMode = websGetVar(wp, T("acMode"), T(""));
	ac_gateWay = websGetVar(wp, "acGateWay", T(""));
	ac_dns = websGetVar(wp, "acDns", T(""));
	tmp = (int)strtoul(acMode, 0, 10);
	if(tmp == 1)
	{
          /*�ж����ص�ַ��DNS��ַ�Ƿ�Ϸ�*/
		if (0 == inet_aton(ac_gateWay, &addr_gw)) {
            errMsg = getTransDataForC(C_LANG_INDEX_GATEWAY_ERR);
            setTheErrorMsg(errMsg);
            websRedirect(wp, "acMode.asp");
            return;
		}
        if (0 == inet_aton(ac_dns, &addr_dns)) {
            errMsg = getTransDataForC(C_LANG_INDEX_INVALID_MAIN_DNS_SERVER);
            setTheErrorMsg(errMsg);
            websRedirect(wp, "acMode.asp");
            return;
		}
	}
    prof = (AcWorkModeProfile*)ProfGetInstPointByIndex(mibType, 0);
    if (prof != NULL)
    {
		ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
		if (prof->workMode != tmp )
        {
            prof->workMode= tmp;

            cmd_ac_set_mode_send_msg(wp,(char)prof->workMode);/*����Ϣ*/
            sleep(1u);/*�ȴ�һ��*/
            cmd_ac_set_mode_send_msg(wp,(char)prof->workMode);/*����Ϣ*/
            sleep(1u);/*�ȴ�һ��*/
            cmd_ac_set_mode_send_msg(wp,(char)prof->workMode);/*����Ϣ*/

        }
		if(tmp == 1)
		{
            prof->gateWay = addr_gw.s_addr;
            prof->pDns = addr_dns.s_addr;
		}
		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
	    nvramWriteCommit();
    }

	websRedirect(wp, "acMode.asp");
	return;
}


/********************************************************************
 * ������ aspOutPutAcWorkMode
 * ���ܣ� ҳ����ʾ����
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� KeHuatao
 * ������ 2013-5-31
 ********************************************************************/ 
static int aspOutPutAcWorkMode(int eid, webs_t wp, int argc, char **argv)
{
    Uint32 mode = 0u;
    MibProfileType mibType = MIB_PROF_AC_WORK_MODE;
    AcWorkModeProfile *prof = NULL;

    prof = (AcWorkModeProfile *)ProfGetInstPointByIndex(mibType, 0);

	mode = prof->workMode;
	websWrite(wp,"var ac_gateWay = \"%N\";\n", prof->gateWay);
	websWrite(wp,"var ac_Dns = \"%N\";\n",prof->pDns);
    websWrite(wp,"var acMode=%d;", mode);

    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

#endif
