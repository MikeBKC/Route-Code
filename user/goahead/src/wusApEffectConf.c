#include <stdio.h>
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "mib.h"
#include "profacce.h"
#include "uttfunction.h"
#include "urcp_msg.h"
#include "translate.h"
#if (FEATURE_AP_EFFECT_CONF == FYES)

static void formApEffectConf(webs_t wp, char_t *path, char_t *query);
static int aspOutPutApEffectConf(int eid, webs_t wp, int argc, char **argv);

/********************************************************************
 * ������ formDefineApEffectConf
 * ���ܣ� ҳ���뽻��ת������
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� xhhu
 * ������ 2012-07-10
 *******************************************************************/
void formDefineApEffectConf(void)
{
    websAspDefine(T("aspOutPutApEffectConf"), aspOutPutApEffectConf);
    websFormDefine(T("formApEffectConf"), formApEffectConf);
}

/*******************************************************************
 *������ formApEffectConf
 *���ܣ� ҳ���ύ����
 *������ 
 *���أ� ��
 *���ߣ� xhhu
 *������ 2012-07-10
 ******************************************************************/
static void formApEffectConf(webs_t wp, char_t *path, char_t *query)
{
#if 0
    char *conf = NULL;
	int tmp, ret = 0;
    MibProfileType mibType = MIB_PROF_AP_EFFECT_CONF;
    ApEffectConfProfile *prof = NULL;
    ProfChangeType profAction = PROF_CHANGE_EDIT;
    struProfAlloc *profhead = NULL;

    conf = websGetVar(wp, T("conf"), T(""));
	tmp = (int)strtoul(conf, 0, 10);

    prof = (ApEffectConfProfile *)ProfGetInstPointByIndex(mibType, 0);
    /* Backup */
    if (prof != NULL)
    {
		if ((PROF_DISABLE == prof->config) && (PROF_ENABLE == tmp)) {
			// AC�·����� -> ��������
			prof->config = tmp;

			request_ac_send_msg(wp, REQUEST_SEND_FILE_TO_AC, REQUEST_SEND_FILE);
			ret = reply_ap_revice_msg(wp);
			DBCK("ret = %d", ret);
			if (ret < 0) {
				DBCK();
				/*�������, ��ԭ֮ǰ������ */
				prof->config = PROF_DISABLE;
				setTheErrorMsg(T("����ʧ��"));
				web_reload(wp, "apParaSwitch.asp");
			} else {
				/* ��ȷ����� */
				DBCK("-----------------------reboot-----------------------");
				/* ��������ʱҳ�� */
				web_reload(wp, "fitApReload.asp");
			}
		} else if ((PROF_ENABLE == prof->config) && (PROF_DISABLE == tmp)) {
			// �������� -> AC�·�����
			prof->config = tmp;
			/* �������õ�flash����������⣬��������������� */
			nvramWriteCommit();

			/* ����Ϣ��urcp��Ҫ�󷢰���ac�޸����� */
			request_ac_send_msg(wp, REQUEST_AC_CHANGE, REQUEST_CHANGE_CONF);
			/* ����AC�ظ���Ϣ���ɹ������� */
			ret = reply_ap_revice_msg(wp);
			DBCK("ret = %d", ret);
			if (ret < 0) {
				DBCK();
				/*�������, ��ԭ֮ǰ������ */
				prof->config = PROF_ENABLE;
				nvramWriteCommit();

				setTheErrorMsg(T("����ʧ��"));
				web_reload(wp, "apParaSwitch.asp");
			} else {
				/* ��ȷ����� */
				DBCK("-----------------------reboot-----------------------");
				web_reload(wp, "fitApReload.asp");
			}
		} else {
			DBCK("to do nothing.............");
			web_reload(wp, "apParaSwitch.asp");
		}
    }
#else
    char *conf = NULL;
	const char *ErrMsg=NULL;
	int tmp, ret = 0, status = 0;
    MibProfileType mibType = MIB_PROF_AP_EFFECT_CONF;
    ApEffectConfProfile *prof = NULL;
    ProfChangeType profAction = PROF_CHANGE_EDIT;
    struProfAlloc *profhead = NULL;

    conf = websGetVar(wp, T("conf"), T(""));
	tmp = (int)strtoul(conf, 0, 10);

    prof = (ApEffectConfProfile *)ProfGetInstPointByIndex(mibType, 0);
    /* Backup */
    if (prof != NULL)
    {
		if ((PROF_DISABLE == prof->config) && (PROF_ENABLE == tmp)) {
			// AC�·����� -> ��������

			request_ac_send_msg(wp, REQUEST_SEND_FILE_TO_AC, REQUEST_SEND_FILE);
			ret = reply_ap_revice_msg(&status);
			DBCK("down_to_local: ret = %d, status = %d", ret, status);
			if (ret < 0) {
				DBCK();
				/*δ�յ���Ϣ����� */
				ErrMsg=getTransDataForC(C_LANG_INDEX_APEFFECTCONF_FAIL);

				setTheErrorMsg(ErrMsg);
				web_reload(wp, "apParaSwitch.asp");
			} else {
				/* �յ���Ϣ����� */
				if (status == URCP_STATUS_FAIL) {
				/* ���������ļ���ACʧ�ܵ���� */
				ErrMsg=getTransDataForC(C_LANG_INDEX_APEFFECTCONF_FAIL);

					setTheErrorMsg(ErrMsg);
					web_reload(wp, "apParaSwitch.asp");
				} else if (status == URCP_STATUS_FOUND_NONE) {
				/* û��AC����� */
//					setTheErrorMsg(T("δ�ҵ���ʹ�õ�AC"));
					makeFactoryProfiles();
					ProfReadFromFlash();
					prof->config = tmp;
					nvramWriteCommit();
					DBCK("-----------------------reboot-----------------------");
					web_reload(wp, "fitApReload.asp");
				} else if (status == URCP_STATUS_OK) {
				/* ���������ļ��ɹ������ */
					makeFactoryProfiles();
					ProfReadFromFlash();
					prof->config = tmp;
					nvramWriteCommit();
					DBCK("-----------------------reboot-----------------------");
					web_reload(wp, "fitApReload.asp");
				} else {
					printf("down_to_local.............status_unknow..................\n");
				ErrMsg=getTransDataForC(C_LANG_INDEX_APEFFECTCONF_FAIL);

					setTheErrorMsg(ErrMsg);
				}
				/* ��������ʱҳ�� */
			}
		} else if ((PROF_ENABLE == prof->config) && (PROF_DISABLE == tmp)) {
			// �������� -> AC�·�����
			prof->config = tmp;
			/* �������õ�flash����������⣬��������������� */
			nvramWriteCommit();
			DBCK("-----------------------reboot-----------------------");
			web_reload(wp, "fitApReload.asp");
		} else {
			DBCK("to do nothing.............");
			web_reload(wp, "apParaSwitch.asp");
		}
    }

#endif
	return;
}

/********************************************************************
 * ������ aspOutPutApEffectConf
 * ���ܣ� ҳ����ʾ����
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� xhhu
 * ������ 2012-07-10
 ********************************************************************/ 
static int aspOutPutApEffectConf(int eid, webs_t wp, int argc, char **argv)
{
    Uint32 conf = 0u;
    MibProfileType mibType = MIB_PROF_AP_EFFECT_CONF;
    ApEffectConfProfile *prof = NULL;

    prof = (ApEffectConfProfile *)ProfGetInstPointByIndex(mibType, 0);
    if (prof != NULL)
    {
	conf = prof->config;
    }
    websWrite(wp,"var config=%d;", conf);
    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

#endif
