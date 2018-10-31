#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include         <linux/autoconf.h>

static int getAssitVar(int eid, webs_t wp, int argc, char **argv);
/********************************************************************
 * 函数： fromDefineAssit
 * 功能： 页面与交互转换函数
 * 作者： bhou
 * 日期： 2011-09-05
 *******************************************************************/
void formDefineAssit(void)
{
	websAspDefine(T("aspOutAssitVar"), getAssitVar);
	return;
}
/********************************************************************
 * 函数： getAssitVar
 * 功能： 页面显示函数
 * 创建： 2011-09-05
 * 作者： bhou 
 ********************************************************************/ 
static int getAssitVar(int eid, webs_t wp, int argc, char **argv)
{
    char dev_type[32];
    memset(dev_type, 0, sizeof(dev_type));

#ifdef CONFIG_SWITCH_EXT
#ifdef CONFIG_VSC7390
    strncpy(dev_type, "VSC7390", sizeof(dev_type) - 1);
#else
#ifdef CONFIG_IP1725
    strncpy(dev_type, "IP1725", sizeof(dev_type) - 1);
#endif
#endif
#endif

    websWrite(wp, "var loginName = \"%s\";\n", wp->userName);/*当前登录用户，用户名*/
    websWrite(wp, "var deviceName = \"%s\";\n", "");/*设备名*/
    websWrite(wp, "var deviceType = \"%s\";\n", dev_type);/*设备类型*/

    return 0;
}

