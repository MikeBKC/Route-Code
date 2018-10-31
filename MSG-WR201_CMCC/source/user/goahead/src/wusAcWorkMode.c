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
 * 函数： formDefineAcWorkMode
 * 功能： 页面与交互转换函数
 * 参数： 无
 * 输出： 无
 * 返回： 无
 * 作者： KeHuatao
 * 创建： 2010-09-20
 *******************************************************************/
void formDefineAcWorkMode(void)
{
    websAspDefine(T("aspOutPutAcWorkMode"), aspOutPutAcWorkMode);
    websFormDefine(T("formAcWorkMode"), formAcWorkMode);
}

/********************************************************************
 * 函数： cmd_ac_set_mode_send_msg 
 * 功能： 发消息给URCP，让URCP发命令给AP
 * 参数： 工作模式
 * 输出： 无
 * 返回： 无
 * 作者： KeHuatao
 * 创建： 2013-05-31
 *******************************************************************/
extern void cmd_ac_set_mode_send_msg(webs_t wp, char mode)
{
    urcp_ac_set_mode_msg_t urcp_ac_set_mode_msg;
    memset(&urcp_ac_set_mode_msg, 0, sizeof(struct _urcp_ac_set_mode_msg_t));

	urcp_ac_set_mode_msg.general_msg.user_ip = ip2int(wp->ipaddr);
	urcp_ac_set_mode_msg.general_msg.opt_type = MISC_SET_GENERAL;/* 设置消息类型 */

    urcp_ac_set_mode_msg.ac_mode = mode;

	/*发消息,通知交换机进程发包*/
	sendwebmsgbuffer(MSG_TYPE_URCP, URCP_CMD_SET_AC_MODE, (char*)(&urcp_ac_set_mode_msg), sizeof(struct _urcp_ac_set_mode_msg_t));
    return;

}
/*******************************************************************
 *函数： formAcWorkMode
 *功能： 页面提交函数
 *参数：
 *返回： 无
 *作者： KeHuatao
 *创建： 2013-5-31
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
          /*判断网关地址和DNS地址是否合法*/
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

            cmd_ac_set_mode_send_msg(wp,(char)prof->workMode);/*发消息*/
            sleep(1u);/*等待一秒*/
            cmd_ac_set_mode_send_msg(wp,(char)prof->workMode);/*发消息*/
            sleep(1u);/*等待一秒*/
            cmd_ac_set_mode_send_msg(wp,(char)prof->workMode);/*发消息*/

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
 * 函数： aspOutPutAcWorkMode
 * 功能： 页面显示函数
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： KeHuatao
 * 创建： 2013-5-31
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
