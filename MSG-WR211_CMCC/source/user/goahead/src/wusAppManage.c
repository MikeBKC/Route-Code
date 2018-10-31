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
/*****************************��������  start*************************************/
extern void formDefineAppManage(void);
static int delShortCutItem(char *apName);
static int addShortCutItem(char *apName);
static int showShortCutMenu(int eid, webs_t wp, int argc, char_t **argv);
static int showShortCutItemNum(int eid, webs_t wp, int argc, char_t **argv);
static void formShortCutMenuDel(webs_t wp, char_t *path, char_t *query);
static void formShortCutMenuAdd(webs_t wp, char_t *path, char_t *query);
/*****************************��������  end***************************************/

/********************************************************************
 * ������ showShortCutMenu
 * ���ܣ� ��ҳ�������ݲ˵�����
 * ������ 2013-10-21
 * ������ ��
 * ���أ� ��
 * ���ߣ� zhao.chong 
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
 * ������ showShortCutItemNum
 * ���ܣ� ��ҳ�������ݲ˵�������Ӧ�ø���
 * ������ 2013-10-21
 * ������ ��
 * ���أ� ��
 * ���ߣ� zhao.chong 
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
	    if (prof->appName[i][0] == '\0' ) {/*�ж��������һ��Ϊ��*/
		break;
	    }
	}
	websWrite(wp,"var itemNum = %d;\n",i);/*�����ǰ��ݲ˵���ĸ���*/
    }
   return 0;
}
/********************************************************************
 * ������ delShortCutItem
 * ���ܣ� ɾ����ݲ˵���
 * ������ 2013-10-21
 * ������ Ӧ�ó�������
 * ���أ� 1->�ɹ���0->ʧ��
 * ���ߣ� zhao.chong 
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
	    if (strncmp(prof->appName[i],apName,50) == 0) {/*�ҵ������*/
		for (j = i+1U; j < 5U; j++) {/*�����֮���Ԫ�طֱ𸲸�֮ǰ��Ԫ��*/
		    memmove((char *)prof->appName[j-1U],(const char*)prof->appName[j],50);
		}
		memset(prof->appName[j-1U],0,50);/*���һ��Ԫ�����,Ϊ��Ԫ������ڳ�λ��*/
		break;
	    }
	}
	for (i = 0U; i < 5U; i++) {
	    APPMNG_PRINT("func:%s,line %d,appName=%s\n",__func__,__LINE__,prof->appName[i]);
	}
    } else {/*��ȡProfileʵ��ʧ��*/
	ret = 0;
    }

    return ret;
}

/********************************************************************
 * ������ formShortCutMenuDel
 * ���ܣ� ��Ӧ��ҳ���󣬻�ȡ������Ҫ�����ݲ�����
 * ������ 2013-10-21
 * ������ ��
 * ���أ� ��
 * ���ߣ� zhao.chong 
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
 * ������ addShortCutItem
 * ���ܣ� ��ӿ�ݲ˵���
 * ������ 2013-10-21
 * ������ Ӧ�ó�������
 * ���أ� 1->�ɹ���0->ʧ��,2->��ʾ��ݲ˵����Ѿ�����
 * ���ߣ� zhao.chong 
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
	    /*��ݲ˵����Ѿ����ڸ�Ӧ��*/
	    if ((prof->appName[i][0] != '\0') && (strncmp(apName,prof->appName[i],50) == 0)) {
		APPMNG_PRINT("exist :prof->appName[%d]=%s\n",i,prof->appName[i]);
		ret = 2;/*����ֵ��2�����ڸ�ҳ����н���*/
		flag = 1;/*��ʶ����1*/
	    } else {/*�����ݲ˵������ڸ�Ӧ�ã�������жϸ�λ���Ƿ�Ϊ��*/
		if (prof->appName[i][0] == '\0') {/*�п���ռ���ֵ*/
		    APPMNG_PRINT("func:%s,line =%d ,appName=%s\n",__func__,__LINE__,apName);
		    strncpy(prof->appName[i],apName,50);
		    flag = 1;/*��ʶ����1,�Ѿ���ӳɹ�*/
		}
	    }
	    if (flag == 1) {/*��ʶ��Ϊ1�����˳�ѭ��*/
		break;
	    }
	    APPMNG_PRINT("func:%s,line =%d , :prof->appName[%d]=%s,appName=%s\n",__func__,__LINE__,i,prof->appName[i],apName);
	}
	if (i == 5U) {/*�˵�������ɾ����һ��Ӧ�ã����µ�app��ӵ��������һ��Ԫ����*/
	    APPMNG_PRINT("func:%s,line %d ----FULL\n",__func__,__LINE__);
	    delShortCutItem(prof->appName[0U]);
	    strncpy(prof->appName[i-1U],apName,50);
	}
    } else {
	ret = 0;/*��ȡprofileʵ��ʧ��*/
    }

    return ret;
}
/********************************************************************
 * ������ formShortCutMenuAdd
 * ���ܣ� ��Ӧ��ҳ���󣬻�ȡ������Ҫ�����ݲ�����
 * ������ 2013-10-21
 * ������ ��
 * ���أ� ��
 * ���ߣ� zhao.chong 
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
/*ҳ��title*/
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
	websWrite(wp,"���ӷ���");
#elif (MIDDLE_SOFT == FYES)
    websWrite(wp,"ADMIN");
#else
	websWrite(wp,"UTT��̩"); 
#endif
#endif
	return 0;
}
/********************************************************************
 * ������ formDefineAppManage
 * ���ܣ� ����ҳ�ϵ��õ�asp����ת���ɸ��ļ��ж���ĺ���
 * ������ 2013-10-19
 * ������ ��
 * ���أ� ��
 * ���ߣ� zhao.chong 
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
