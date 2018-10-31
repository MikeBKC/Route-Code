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
 * 函数： formDefineApEffectConf
 * 功能： 页面与交互转换函数
 * 参数： 无
 * 输出： 无
 * 返回： 无
 * 作者： xhhu
 * 创建： 2012-07-10
 *******************************************************************/
void formDefineApEffectConf(void)
{
    websAspDefine(T("aspOutPutApEffectConf"), aspOutPutApEffectConf);
    websFormDefine(T("formApEffectConf"), formApEffectConf);
}

/*******************************************************************
 *函数： formApEffectConf
 *功能： 页面提交函数
 *参数： 
 *返回： 无
 *作者： xhhu
 *创建： 2012-07-10
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
			// AC下发配置 -> 本地配置
			prof->config = tmp;

			request_ac_send_msg(wp, REQUEST_SEND_FILE_TO_AC, REQUEST_SEND_FILE);
			ret = reply_ap_revice_msg(wp);
			DBCK("ret = %d", ret);
			if (ret < 0) {
				DBCK();
				/*出错情况, 还原之前的配置 */
				prof->config = PROF_DISABLE;
				setTheErrorMsg(T("操作失败"));
				web_reload(wp, "apParaSwitch.asp");
			} else {
				/* 正确的情况 */
				DBCK("-----------------------reboot-----------------------");
				/* 重启倒计时页面 */
				web_reload(wp, "fitApReload.asp");
			}
		} else if ((PROF_ENABLE == prof->config) && (PROF_DISABLE == tmp)) {
			// 本地配置 -> AC下发配置
			prof->config = tmp;
			/* 保存配置到flash，必须放在这，后面过程中有重启 */
			nvramWriteCommit();

			/* 发消息给urcp，要求发包给ac修改配置 */
			request_ac_send_msg(wp, REQUEST_AC_CHANGE, REQUEST_CHANGE_CONF);
			/* 接收AC回复消息，成功则重启 */
			ret = reply_ap_revice_msg(wp);
			DBCK("ret = %d", ret);
			if (ret < 0) {
				DBCK();
				/*出错情况, 还原之前的配置 */
				prof->config = PROF_ENABLE;
				nvramWriteCommit();

				setTheErrorMsg(T("操作失败"));
				web_reload(wp, "apParaSwitch.asp");
			} else {
				/* 正确的情况 */
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
			// AC下发配置 -> 本地配置

			request_ac_send_msg(wp, REQUEST_SEND_FILE_TO_AC, REQUEST_SEND_FILE);
			ret = reply_ap_revice_msg(&status);
			DBCK("down_to_local: ret = %d, status = %d", ret, status);
			if (ret < 0) {
				DBCK();
				/*未收到消息的情况 */
				ErrMsg=getTransDataForC(C_LANG_INDEX_APEFFECTCONF_FAIL);

				setTheErrorMsg(ErrMsg);
				web_reload(wp, "apParaSwitch.asp");
			} else {
				/* 收到消息的情况 */
				if (status == URCP_STATUS_FAIL) {
				/* 发送配置文件给AC失败的情况 */
				ErrMsg=getTransDataForC(C_LANG_INDEX_APEFFECTCONF_FAIL);

					setTheErrorMsg(ErrMsg);
					web_reload(wp, "apParaSwitch.asp");
				} else if (status == URCP_STATUS_FOUND_NONE) {
				/* 没有AC的情况 */
//					setTheErrorMsg(T("未找到可使用的AC"));
					makeFactoryProfiles();
					ProfReadFromFlash();
					prof->config = tmp;
					nvramWriteCommit();
					DBCK("-----------------------reboot-----------------------");
					web_reload(wp, "fitApReload.asp");
				} else if (status == URCP_STATUS_OK) {
				/* 发送配置文件成功的情况 */
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
				/* 重启倒计时页面 */
			}
		} else if ((PROF_ENABLE == prof->config) && (PROF_DISABLE == tmp)) {
			// 本地配置 -> AC下发配置
			prof->config = tmp;
			/* 保存配置到flash，必须放在这，后面过程中有重启 */
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
 * 函数： aspOutPutApEffectConf
 * 功能： 页面显示函数
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xhhu
 * 创建： 2012-07-10
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
