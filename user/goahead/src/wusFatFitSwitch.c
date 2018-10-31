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
#if (FAT_FIT_SWITCH == FYES)

#define jsFile "/etc_ro/web/js/config.js"
#define tmpFile "/etc_ro/web/js/tmp.js"
static void formFatFitSwitch(webs_t wp, char_t *path, char_t *query);
static int aspOutPutFatFitSwitch(int eid, webs_t wp, int argc, char **argv);
static int aspOutPutFatFitSwitchForLeft(int eid, webs_t wp, int argc, char **argv);
/********************************************************************
 * 函数： formDefineFatFitSwitch
 * 功能： 页面与交互转换函数
 * 参数： 无
 * 输出： 无
 * 返回： 无
 * 作者： KeHuatao
 * 创建： 2010-09-20
 *******************************************************************/
void formDefineFatFitSwitch(void)
{
    websAspDefine(T("aspOutPutFatFitSwitch"), aspOutPutFatFitSwitch);
    websFormDefine(T("formFatFitSwitch"), formFatFitSwitch);
    websAspDefine(T("aspOutPutFatFitSwitchForLeft"), aspOutPutFatFitSwitchForLeft);
}

/*******************************************************************
 *函数： formFatFitSwitch
 *功能： 页面提交函数
 *参数： 
 *返回： 无
 *作者： KeHuatao
 *创建： 2010-10-12
 ******************************************************************/
static void formFatFitSwitch(webs_t wp, char_t *path, char_t *query)
{
#if 0
    FILE *fp1,*fp2;
    char_t buffer[128] = {0};
    char *mode = websGetVar(wp, T("apMode"), T(""));

    /* clear buffer */
    memset(buffer, 0, sizeof(buffer));
    /* open jsFile */
    if ((fp1 = fopen(jsFile, "r")) == NULL)
    {
	fprintf(stderr, "failed to open %s Exiting\n", jsFile);
	return;
    }
    /* open tmp file */
    if ((fp2 = fopen(tmpFile, "w")) == NULL)
    {
	fprintf(stderr, "failed to create %s Exiting\n", tmpFile);
	return;
    }
    while (!feof(fp1))
    {
	if (fgets(buffer, fp1) != NULL)
	{
	    if (strncmp(buffer, "var", 3) != 0)
	    {
		fputs(buffer, fp2);
	    }
	    else
	    {
	    
	    }
	}
    
    }
    fclose(fp1);
    fclose(fp2);
#endif

#if 0
    char *apMode = NULL;
    MibProfileType mibType = MIB_PROF_FAT_FIT_SWITCH;
    MibProfileType mibTypeApEffect = MIB_PROF_AP_EFFECT_CONF;
    FatFitSwitchProfile *prof = NULL;
    ApEffectConfProfile *profApEffect = NULL;
    ProfChangeType profAction = PROF_CHANGE_EDIT;
    struProfAlloc *profhead = NULL;
	int flag = 0, tmp;

    apMode = websGetVar(wp, T("apMode"), T(""));
	tmp = (int)strtoul(apMode, 0, 10);

    prof = (FatFitSwitchProfile *)ProfGetInstPointByIndex(mibType, 0);
    if (prof != NULL)
    {
		if ((prof->fatEnable == 0) && (tmp == 1))
		{
			// 瘦AP转胖AP
			profApEffect = (ApEffectConfProfile *)ProfGetInstPointByIndex(mibTypeApEffect, 0);
			if (profApEffect != NULL) {
				if (PROF_ENABLE == profApEffect->config) {
					/* 如果是本地配置生效, 配置保存到flash，直接重启 */
					DBCK();
					prof->fatEnable = tmp;
					nvramWriteCommit();
					web_reload(wp, "fitApReload.asp");
				} else {
					/* 如果是下发配置生效，直接将配置保存在内存中，将配置文件发到ac上，成功则重启 */
					prof->fatEnable = tmp;

					request_ac_send_msg(wp, REQUEST_SEND_FILE_TO_AC, REQUEST_SEND_FILE);
					flag = reply_ap_revice_msg(wp);
					if (flag < 0) {
						/* 失败的情况 */
						DBCK();
						/* 还原之前的配置 */
						prof->fatEnable = 0;

						setTheErrorMsg(T("操作失败"));
						web_reload(wp, "fatFitSwitch.asp");
					} else {
						/* 成功的情况 */
						DBCK();
						web_reload(wp, "fitApReload.asp");
					}
				}
			} else {
				setTheErrorMsg(T("操作失败"));
				web_reload(wp, "fatFitSwitch.asp");
			}
		} else if ((prof->fatEnable == 1) && (tmp == 0)) {
			// 胖AP转瘦AP
			prof->fatEnable = tmp;
			/* 保存配置到flash, 后面过程需要重启*/
			nvramWriteCommit();

			request_ac_send_msg(wp, REQUEST_AC_CHANGE, REQUEST_FAT_TO_FIT);
			usleep(2000 * 1000);
			flag = reply_ap_revice_msg(wp);
			if (flag < 0) {
				/* 失败的情况 */
				DBCK();
				/* 还原之前的配置 */
				prof->fatEnable = 1;
				nvramWriteCommit();

				setTheErrorMsg(T("操作失败"));
				web_reload(wp, "fatFitSwitch.asp");
			} else {
				/* 成功的情况 */
				DBCK("-----------------------------reboot------------------------------");
				web_reload(wp, "fitApReload.asp");
			}
		} else {
			DBCK("to do nothing............");
			web_reload(wp, "fatFitSwitch.asp");
		}
    }
#else
    char *apMode = NULL;
	const char *ErrMsg = NULL;

    MibProfileType mibType = MIB_PROF_FAT_FIT_SWITCH;
    MibProfileType mibTypeApEffect = MIB_PROF_AP_EFFECT_CONF;
    FatFitSwitchProfile *prof = NULL;
    ApEffectConfProfile *profApEffect = NULL;
    ProfChangeType profAction = PROF_CHANGE_EDIT;
    struProfAlloc *profhead = NULL;
	int flag = 0, tmp, status = 0;

    apMode = websGetVar(wp, T("apMode"), T(""));
	tmp = (int)strtoul(apMode, 0, 10);

    prof = (FatFitSwitchProfile *)ProfGetInstPointByIndex(mibType, 0);
    if (prof != NULL)
    {
		if ((prof->fatEnable == 0) && (tmp == 1))
		{
			// 瘦AP转胖AP
			profApEffect = (ApEffectConfProfile *)ProfGetInstPointByIndex(mibTypeApEffect, 0);
			if (profApEffect != NULL) {
				if (PROF_ENABLE == profApEffect->config) {
					/* 如果是本地配置生效, 配置保存到flash，直接重启 */
					DBCK();
					prof->fatEnable = tmp;
					nvramWriteCommit();
					web_reload(wp, "fitApReload.asp");
				} else {
					/* 如果是下发配置生效 */
					request_ac_send_msg(wp, REQUEST_SEND_FILE_TO_AC, REQUEST_SEND_FILE);
					flag = reply_ap_revice_msg(&status);
					if (flag < 0) {
						/* AP未收到回复消息的情况 */
						DBCK();
                		ErrMsg = getTransDataForC(C_LANG_INDEX_APEFFECTCONF_FAIL);
			            setTheErrorMsg((char *)ErrMsg);


					//	setTheErrorMsg(T("操作失败"));
						web_reload(wp, "fatFitSwitch.asp");
					} else {
						/* 成功的情况 */
						DBCK();
						if (status == URCP_STATUS_FAIL) {
							DBCK();
							ErrMsg = getTransDataForC(C_LANG_INDEX_APEFFECTCONF_FAIL);
							setTheErrorMsg((char *)ErrMsg);


						//	setTheErrorMsg(T("操作失败"));
							web_reload(wp, "fatFitSwitch.asp");
						} else if (status == URCP_STATUS_FOUND_NONE) {
							DBCK();
							ErrMsg = getTransDataForC(C_LANG_INDEX_FATFIT_SWITCH_NOTFINADAC);
							setTheErrorMsg((char *)ErrMsg);


//							setTheErrorMsg(T("未找到可使用的AC"));
							makeFactoryProfiles();
							ProfReadFromFlash();
							prof->fatEnable = tmp;
							nvramWriteCommit();
							web_reload(wp, "fitApReload.asp");
						} else if (status == URCP_STATUS_OK) {
							DBCK();
							makeFactoryProfiles();
							ProfReadFromFlash();
							prof->fatEnable = tmp;
							nvramWriteCommit();
							web_reload(wp, "fitApReload.asp");
						} else {
							DBCK();
							printf("fitAp_to_fatAp.............status_unknow..................\n");
							ErrMsg = getTransDataForC(C_LANG_INDEX_APEFFECTCONF_FAIL);
							setTheErrorMsg((char *)ErrMsg);


						//	setTheErrorMsg(T("操作失败"));
							web_reload(wp, "fatFitSwitch.asp");
						}
					}
				}
			} else {
							ErrMsg = getTransDataForC(C_LANG_INDEX_APEFFECTCONF_FAIL);
							setTheErrorMsg((char *)ErrMsg);


			//	setTheErrorMsg(T("操作失败"));
				web_reload(wp, "fatFitSwitch.asp");
			}
		} else if ((prof->fatEnable == 1) && (tmp == 0)) {
			// 胖AP转瘦AP
			prof->fatEnable = tmp;
			/* 保存配置到flash, 后面过程需要重启*/
			nvramWriteCommit();

			DBCK("-----------------------------reboot------------------------------");
			web_reload(wp, "fitApReload.asp");
		} else {
			DBCK("to do nothing............");
			web_reload(wp, "fatFitSwitch.asp");
		}
    }
#endif
	return;
}


/********************************************************************
 * 函数： aspOutPutFatFitSwitchForLeft
 * 功能： 页面显示函数
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： KeHuatao
 * 创建： 2010-10-12
 ********************************************************************/ 
static int aspOutPutFatFitSwitchForLeft(int eid, webs_t wp, int argc, char **argv)
{
    Uint32 mode = 0u;
    MibProfileType mibType = MIB_PROF_FAT_FIT_SWITCH;
    FatFitSwitchProfile *prof = NULL;

    prof = (FatFitSwitchProfile *)ProfGetInstPointByIndex(mibType, 0);
    if (prof != NULL)
    {
	mode = prof->fatEnable;
    }
    if (mode == PROF_DISABLE)
    {
	websWrite(wp,"<script src=\"js/fit_config.js\" type=\"text/javascript\"></script>");
    }
    else
    {
	websWrite(wp,"<script src=\"js/config.js\" type=\"text/javascript\"></script>");
    }

    return 0;
}

/********************************************************************
 * 函数： aspOutPutFatFitSwitch
 * 功能： 页面显示函数
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： KeHuatao
 * 创建： 2010-10-12
 ********************************************************************/ 
static int aspOutPutFatFitSwitch(int eid, webs_t wp, int argc, char **argv)
{
    Uint32 mode = 0u;
    MibProfileType mibType = MIB_PROF_FAT_FIT_SWITCH;
    FatFitSwitchProfile *prof = NULL;

    prof = (FatFitSwitchProfile *)ProfGetInstPointByIndex(mibType, 0);
    if (prof != NULL)
    {
	mode = prof->fatEnable;
    }
    websWrite(wp,"var apMode=%d;", mode);
    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

#endif
