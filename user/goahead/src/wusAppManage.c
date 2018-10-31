#include "uttMachine.h"
#if (UTT_SMART_UI == FYES)
#if (APP_MENU == FYES)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "typedef.h"
#include "mib.h"
#include "profacce.h"
#include "webs.h"
#include "utils.h"
#include "uttfunction.h"
#include "comExeFun.h"
#include "translate.h"
#include "um.h"
#include "internet.h"
#include "wireless.h"
#include "ifmisc.h"

#define DEBUG_SWITCh 0
#if DEBUG_SWITCh
#define APPMNG_PRINT(fmt, args...)		printf(fmt, ## args)
#else
#define APPMNG_PRINT(fmt, args...)
#endif
/*****************************函数声明  start*************************************/
extern void formDefineAppManage(void);
static int delShortCutItem(char *apName);
static int addShortCutItem(char *apName);
static int showShortCutMenu(int eid, webs_t wp, int argc, char_t **argv);
static int showShortCutItemNum(int eid, webs_t wp, int argc, char_t **argv);
static void formShortCutMenuDel(webs_t wp, char_t *path, char_t *query);
static void formShortCutMenuAdd(webs_t wp, char_t *path, char_t *query);
/*****************************函数声明  end***************************************/

/********************************************************************
 * 函数： showShortCutMenu
 * 功能： 向页面输出快捷菜单数组
 * 创建： 2013-10-21
 * 参数： 无
 * 返回： 无
 * 作者： zhao.chong 
 ********************************************************************/ 
static int showShortCutMenu(int eid, webs_t wp, int argc, char_t **argv)
{
    int i, max, min, index = 0;
    MibProfileType mibType = MIB_PROF_SHORTCUT_MENU;
    ShortCutMenuProfile *prof = NULL;

    ProfInstLowHigh(mibType, &max, &min);
    websWrite(wp,"var shortcutMenu = new Array("); 
    prof = (ShortCutMenuProfile *)ProfGetInstPointByIndex(mibType, index);
    if ((prof != NULL) && (*(prof->head.name) != '\0') && (prof->head.active == TRUE)) {
	APPMNG_PRINT("func:%s,line %d,instName=%s\n",__func__,__LINE__,prof->head.name);
	for (i = 0; i < 5; i++) {
	    if (prof->appName[i][0] != '\0' ) {
		websWrite(wp,"\"%s\",",prof->appName[i]);
	    }
	}
    }
    websWrite(wp,"\"\""); 
    websWrite(wp,");\n"); 
   return 0;
}

/********************************************************************
 * 函数： showShortCutItemNum
 * 功能： 向页面输出快捷菜单中现有应用个数
 * 创建： 2013-10-21
 * 参数： 无
 * 返回： 无
 * 作者： zhao.chong 
 ********************************************************************/ 
static int showShortCutItemNum(int eid, webs_t wp, int argc, char_t **argv)
{
    int i, min, max, index = 0;
    MibProfileType mibType = MIB_PROF_SHORTCUT_MENU;
    ShortCutMenuProfile *prof = NULL;

    ProfInstLowHigh(mibType, &max, &min);
    prof = (ShortCutMenuProfile *)ProfGetInstPointByIndex(mibType, index);
    if ((prof != NULL) && (*(prof->head.name) != '\0') && (prof->head.active == TRUE)) {
	APPMNG_PRINT("func:%s,line %d,instName=%s\n",__func__,__LINE__,prof->head.name);
	for (i = 0; i < 5; i++) {
	    if (prof->appName[i][0] == '\0' ) {/*判断数组的哪一项为空*/
		break;
	    }
	}
	websWrite(wp,"var itemNum = %d;\n",i);/*输出当前快捷菜单项的个数*/
    }
   return 0;
}
/********************************************************************
 * 函数： delShortCutItem
 * 功能： 删除快捷菜单项
 * 创建： 2013-10-21
 * 参数： 应用程序名称
 * 返回： 1->成功，0->失败
 * 作者： zhao.chong 
 ********************************************************************/ 
static int delShortCutItem(char *apName)
{
    MibProfileType mibType = MIB_PROF_SHORTCUT_MENU;
    ShortCutMenuProfile *prof = NULL;
    unsigned int i,j;
    int ret = 1;

    prof = (ShortCutMenuProfile *)ProfGetInstPointByIndex(mibType,0);
    if (prof != NULL) {
	for (i = 0U; i < 5U; i++) {
	    if (strncmp(prof->appName[i],apName,50) == 0) {/*找到快捷项*/
		for (j = i+1U; j < 5U; j++) {/*快捷项之后的元素分别覆盖之前的元素*/
		    memmove((char *)prof->appName[j-1U],(const char*)prof->appName[j],50);
		}
		memset(prof->appName[j-1U],0,50);/*最后一个元素清空,为新元素添加腾出位置*/
		break;
	    }
	}
	for (i = 0U; i < 5U; i++) {
	    APPMNG_PRINT("func:%s,line %d,appName=%s\n",__func__,__LINE__,prof->appName[i]);
	}
    } else {/*获取Profile实例失败*/
	ret = 0;
    }

    return ret;
}

/********************************************************************
 * 函数： formShortCutMenuDel
 * 功能： 响应首页请求，获取请求需要的数据并返回
 * 创建： 2013-10-21
 * 参数： 略
 * 返回： 无
 * 作者： zhao.chong 
 ********************************************************************/ 
static void formShortCutMenuDel(webs_t wp, char_t *path, char_t *query)
{
    char_t *appName = NULL;
    char_t respStr[50] = "";

    appName = websGetVar(wp,T("instName"),T("")); 
    APPMNG_PRINT("func:%s,line %d,appName=%s\n",__func__,__LINE__,appName);
    if (*appName != '\0') {
	if (delShortCutItem(appName) == 0) {
	    strncpy(respStr,"del failure",50);
	}
    } else {
	strncpy(respStr,"cat't get appName from web",50);
    }
    nvramWriteCommit();
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/html\nPragma: no-cache\nCache-Control: no-cache\n\n"));
    websWrite(wp, T("%s"), respStr);
    websDone(wp, 200);
}

/********************************************************************
 * 函数： addShortCutItem
 * 功能： 添加快捷菜单项
 * 创建： 2013-10-21
 * 参数： 应用程序名称
 * 返回： 1->成功，0->失败,2->表示快捷菜单项已经存在
 * 作者： zhao.chong 
 ********************************************************************/ 
static int addShortCutItem(char *apName)
{
    MibProfileType mibType = MIB_PROF_SHORTCUT_MENU;
    ShortCutMenuProfile *prof = NULL;
    unsigned int i;
    int ret = 1, flag = 0;

    prof = (ShortCutMenuProfile *)ProfGetInstPointByIndex(mibType,0);
    if (prof != NULL) {
	for (i = 0U; i < 5U; i++) {
	    APPMNG_PRINT("func:%s,line =%d :prof->appName[%d]=%s,appName=%s\n",__func__,__LINE__,i,prof->appName[i],apName);
	    /*快捷菜单中已经存在该应用*/
	    if ((prof->appName[i][0] != '\0') && (strncmp(apName,prof->appName[i],50) == 0)) {
		APPMNG_PRINT("exist :prof->appName[%d]=%s\n",i,prof->appName[i]);
		ret = 2;/*返回值置2，用于跟页面进行交互*/
		flag = 1;/*标识符置1*/
	    } else {/*如果快捷菜单不存在该应用，则继续判断该位置是否为空*/
		if (prof->appName[i][0] == '\0') {/*有空余空间则赋值*/
		    APPMNG_PRINT("func:%s,line =%d ,appName=%s\n",__func__,__LINE__,apName);
		    strncpy(prof->appName[i],apName,50);
		    flag = 1;/*标识符置1,已经添加成功*/
		}
	    }
	    if (flag == 1) {/*标识符为1，则退出循环*/
		break;
	    }
	    APPMNG_PRINT("func:%s,line =%d , :prof->appName[%d]=%s,appName=%s\n",__func__,__LINE__,i,prof->appName[i],apName);
	}
	if (i == 5U) {/*菜单已满，删除第一个应用，将新的app添加到数组最后一个元素中*/
	    APPMNG_PRINT("func:%s,line %d ----FULL\n",__func__,__LINE__);
	    delShortCutItem(prof->appName[0U]);
	    strncpy(prof->appName[i-1U],apName,50);
	}
    } else {
	ret = 0;/*获取profile实例失败*/
    }

    return ret;
}
/********************************************************************
 * 函数： formShortCutMenuAdd
 * 功能： 响应首页请求，获取请求需要的数据并返回
 * 创建： 2013-10-21
 * 参数： 略
 * 返回： 无
 * 作者： zhao.chong 
 ********************************************************************/ 
static void formShortCutMenuAdd(webs_t wp, char_t *path, char_t *query)
{
    char_t *appName = NULL;
    int ret;

    appName = websGetVar(wp,T("instName"),T("")); 
    if (*appName != '\0') {
	ret = addShortCutItem(appName);
    }
    nvramWriteCommit();

    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/html\nPragma: no-cache\nCache-Control: no-cache\n\n"));
    websWrite(wp, T("%d"), ret);
    websDone(wp, 200);
}
/*页面title*/
static int getTitle(int eid, webs_t wp, int argc, char_t **argv)
{
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
#if(BRAND_LEVEL_ONE == FYES)
	websWrite(wp,"Levelone");
#else
	websWrite(wp,"UTT Technologies");
#endif
#else
#if(GALAXYWIND == FYES)
	websWrite(wp,"银河风云");
#elif (MIDDLE_SOFT == FYES)
    websWrite(wp,"ADMIN");
#else
	websWrite(wp,"UTT艾泰"); 
#endif
#endif
	return 0;
}
/********************************************************************
 * 函数： formDefineAppManage
 * 功能： 把网页上调用的asp函数转换成该文件中定义的函数
 * 创建： 2013-10-19
 * 参数： 无
 * 返回： 无
 * 作者： zhao.chong 
 ********************************************************************/ 
void formDefineAppManage(void)
{
    websFormDefine(T("formShortCutMenuDel"), formShortCutMenuDel);
    websFormDefine(T("formShortCutMenuAdd"), formShortCutMenuAdd);
    websAspDefine(T("aspOutputShortCutMenu"), showShortCutMenu);
    websAspDefine(T("aspGetTitle"), getTitle);
    websAspDefine(T("aspOutputShortCutItemNum"), showShortCutItemNum);
}
#endif
#endif
