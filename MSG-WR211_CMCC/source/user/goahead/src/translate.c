#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include "utils.h"
#include "webs.h"
#include "firewall.h"
#include "internet.h"
#include "uttfunction.h"
#include "mib.h"
#include "profacce.h"

#include "translate.h"
#define TRANSLATE_DATA_IN_CONF
#define TRANSLATR_DATA_FILE_DIR "/etc_ro/web/lang/"

//extern int InitUserStatPieStr(void);
//extern char pieFlowStr[300], pieActwStr[300];

struct st_tran_Data transGlobleData[] = {
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1 && UTT_TRANS_CHINESE_SIMPLE == 1)
    {"zhcn", transData_zhcn},
#else
	{},
#endif
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1 && UTT_TRANS_ENGLISH == 1)
    {"en", transData_en},
#else
	{},
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1 && UTT_TRANS_TW == 1)
	{"zhtw", transData_zhtw},
#else
	{},
#endif
};

#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
enum em_UTT_LANGUAGE trans_curLang = LANGUAGE_ZH;
#elif (CONFIG_USER_GOAHEAD_LANG_EN == 1)
enum em_UTT_LANGUAGE trans_curLang = LANGUAGE_EN;
#elif (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
enum em_UTT_LANGUAGE trans_curLang = LANGUAGE_TW;
#endif
/*
 * 开机初始化当前语言 
 */
void initcurLang(void)
{
    MibProfileType mibType = MIB_PROF_LANG;
    LangProfile *prof;
    /*get langProfile*/
    prof = (LangProfile*)ProfGetInstPointByIndex(mibType, 0);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0))
    {
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
	if(strcmp(prof->lang, "zhcn") == 0)   /*set language*/
	{
	    trans_curLang = LANGUAGE_ZH;
	}
#endif
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
	if(strcmp(prof->lang, "en") == 0)
	{
	    trans_curLang = LANGUAGE_EN;
	}
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
	if((strcmp(prof->lang, "zhtw") == 0) && (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1))
	{
		trans_curLang = LANGUAGE_TW;
	}
#endif
    }
    else	    /*set default*/
    {
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
	trans_curLang = LANGUAGE_ZH;
#elif (CONFIG_USER_GOAHEAD_LANG_EN == 1)
	trans_curLang = LANGUAGE_EN;
#elif (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
	trans_curLang = LANGUAGE_TW;
#endif
    }
    //InitUserStatPieStr();
}


#define TRANS_DATA_LINE_MAX 512

const char* getTransDataForC(unsigned int arrayIndex)
{
    const char* rtvalue = "";
//	int i = TRANS_C_DATA_MAX;
    if(arrayIndex < TRANS_C_DATA_MAX) {
        rtvalue = *(transGlobleData[trans_curLang].transData + arrayIndex);
    }
    return rtvalue;
}
#define LINUX
#ifdef LINUX
static int aspOutLanguageTranslate(int eid, webs_t wp, int argc, char_t **argv)
{
    FILE *fp;
    char buf[TRANS_DATA_LINE_MAX] = {0};
    char *fileName = NULL;
    char *name = NULL, *value = NULL;

    if (ejArgs(argc, argv, T("%s"), &fileName) < 1) {
        websWrite(wp, T("Translate Error"));
    } else {
        snprintf(buf, TRANS_DATA_LINE_MAX,"%s%s/%s", TRANSLATR_DATA_FILE_DIR,
            transGlobleData[trans_curLang].dir, fileName);
        fp = fopen(buf, "r");
        if(fp != NULL) {
            while(fgets(buf, TRANS_DATA_LINE_MAX, fp) != NULL) {
                name = strtok(buf, "=");
                if(name != NULL) {
                    value = strtok(NULL, "\n");
                    if(value != NULL) {
                        ejSetVar(eid, name, value);
                    }
                }
            }
            fclose(fp);
        }
    }
    return 0;
}
#else
#endif

//static void fromLangeTran(webs_t wp, char_t *path, char_t *query)
void setCurLange(char * lang)
{
    if(lang!=NULL)
    {
	/*en zhcn*/
	if(strcmp(lang, "en") == 0) {
	    trans_curLang = LANGUAGE_EN;
	} else if (strcmp(lang, "zhtw") == 0){
	    trans_curLang = LANGUAGE_TW;
	} else {
		trans_curLang = LANGUAGE_ZH;
	}
    }

    return;
}

void formDefineLangTran(void)
{
    websAspDefine(T("langTran"), aspOutLanguageTranslate);
}
