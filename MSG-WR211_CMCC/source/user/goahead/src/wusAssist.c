#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include         <linux/autoconf.h>

static int getAssitVar(int eid, webs_t wp, int argc, char **argv);
/********************************************************************
 * ������ fromDefineAssit
 * ���ܣ� ҳ���뽻��ת������
 * ���ߣ� bhou
 * ���ڣ� 2011-09-05
 *******************************************************************/
void formDefineAssit(void)
{
	websAspDefine(T("aspOutAssitVar"), getAssitVar);
	return;
}
/********************************************************************
 * ������ getAssitVar
 * ���ܣ� ҳ����ʾ����
 * ������ 2011-09-05
 * ���ߣ� bhou 
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

    websWrite(wp, "var loginName = \"%s\";\n", wp->userName);/*��ǰ��¼�û����û���*/
    websWrite(wp, "var deviceName = \"%s\";\n", "");/*�豸��*/
    websWrite(wp, "var deviceType = \"%s\";\n", dev_type);/*�豸����*/

    return 0;
}

