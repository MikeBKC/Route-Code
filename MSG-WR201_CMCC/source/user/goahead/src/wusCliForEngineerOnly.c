#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "uttfunction.h"
#include "mib.h"
#include "profacce.h"
#include "translate.h"


#if(CLI_FOR_ENGINEER_ONLY == FYES)
#ifdef CLI_FOR_ENGINEER_ONLY_DBG
#define DNS_FILTER_PRINTF(fmt, args...)		printf(fmt, ## args)	
#else
#define DNS_FILTER_PRINTF(fmt, args...)
#endif

static int GetMaxCommandNum(int eid, webs_t wp, int argc, char_t **argv);
static void formConfigCliForEngineerOnlyDel(webs_t wp, char_t *path, char_t *query);
static void formConfigCliForEngineerOnlyDelAll(webs_t wp, char_t *path, char_t *query);
static int getCommandList(int eid, webs_t wp, int argc, char_t **argv);
static void formConfigCliForEngineerOnly(webs_t wp, char_t *path, char_t *query);

/***********************************************************************
 *   功能描述：   从配置中读取实例最大数
 *   作者：       wang.xiaohua
 *   时间：       2015年1月7日
 *   附加说明：   无
 * ***********************************************************************/
static int GetMaxCommandNum(int eid, webs_t wp, int argc, char_t **argv)
{
    websWrite(wp,"var MaxCommandNum= \"%d\";", MAX_CLI_FOR_ENGINEER_ONLY_PROFILES);
    return 0;
}

/***********************************************************************
 * 函 数 名：   formConfigCliForEngineerOnlyDel
 * 创建日期：	2015-1-7
 * 修改日期：
 * 作者：       wang.xiaohua
 * 附加说明：	无
 ***********************************************************************/
static void formConfigCliForEngineerOnlyDel(webs_t wp, char_t *path, char_t *query)
{
    int min, max, index;
    char *ptr, *Entry=NULL; //每一条命令
    MibProfileType mibType = MIB_PROF_CLI_FOR_ENGINEER_ONLY;
    CliForEngineerOnlyProfile *prof = NULL;
    ProfChangeType profAction = PROF_CHANGE_DEL;
    struProfAlloc *profhead = NULL;
    
    Entry = websGetVar(wp, T("delstr"), T("")); 
    Entry = strtok_r(Entry,UTT_SPLIT_SIGN_STR,&ptr);
    if(!strcmp(Entry, "")) {
        websRedirect(wp, "uttCli.asp");
        return;
    }
    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
        prof = (CliForEngineerOnlyProfile *)ProfGetInstPointByIndex(mibType, index);
        if(!strcmp(prof->command, Entry))
        {
            ProfBackupByIndex(mibType, profAction, index, &profhead);
            ProfDelInstByIndex(mibType, index);
            ProfUpdate(profhead);
            ProfFreeAllocList(profhead);
            nvramWriteCommit();
            break;
        }
    }
    websRedirect(wp, "uttCli.asp");
}

/***********************************************************************
 * 函 数 名：   formConfigCliForEngineerOnlyDelAll
 * 创建日期：	2015-1-7
 * 修改日期：
 * 作者：       wang.xiaohua
 * 附加说明：	无
 ***********************************************************************/
static void formConfigCliForEngineerOnlyDelAll(webs_t wp, char_t *path, char_t *query)
{
    int min, max, index;
    MibProfileType mibType = MIB_PROF_CLI_FOR_ENGINEER_ONLY;
    CliForEngineerOnlyProfile *prof = NULL;
    ProfChangeType profAction = PROF_CHANGE_DELALL;
    struProfAlloc *profhead = NULL;

    ProfBackupByIndex(mibType, profAction, 0, &profhead); 
    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
        prof = (CliForEngineerOnlyProfile *)ProfGetInstPointByIndex(mibType, index);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0))
        {
            ProfDelInstByIndex(mibType, index);
        }
    }
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    websRedirect(wp, "uttCli.asp");
}

/***********************************************************************
 * 函 数 名：   getCommandList
 * 创建日期：	2015-1-7
 * 修改日期：
 * 作者：       wang.xiaohua
 * 附加说明：	无
 ***********************************************************************/
static int getCommandList(int eid, webs_t wp, int argc, char_t **argv)
{
    int min, max, index, num =0;
    MibProfileType mibType = MIB_PROF_CLI_FOR_ENGINEER_ONLY;
    CliForEngineerOnlyProfile *prof = NULL;

    websWrite(wp,"var CommandLists=new Array();");
    ProfInstLowHigh(mibType, &max, &min);
    num = min;
    for (index = min; index < max; index++)
    {
        prof = (CliForEngineerOnlyProfile *)ProfGetInstPointByIndex(mibType, index);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0) && prof->head.active == TRUE)
        {
            websWrite(wp, "CommandLists[%d] = \"%s\";", num, prof->command);
            num++;
        }
        else
        {
            continue;
        }
    }
    websWrite(wp,"var totalrecs=%d;",num);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

/***********************************************************************
 * 函 数 名：   formConfigCliForEngineerOnly
 * 创建日期：	2015-1-7
 * 修改日期：
 * 作者：       wang.xiaohua
 * 附加说明：	无
 ***********************************************************************/
static void formConfigCliForEngineerOnly(webs_t wp, char_t *path, char_t *query)
{
    int min, max, index;
    char tmp[100];
    char *cmd= websGetVar(wp, T("addCommand"), T(""));
    MibProfileType mibType = MIB_PROF_CLI_FOR_ENGINEER_ONLY;
    CliForEngineerOnlyProfile *prof = NULL;
    ProfChangeType profAction = PROF_CHANGE_ADD;
    struProfAlloc *profhead = NULL;

    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
        prof = (CliForEngineerOnlyProfile *)ProfGetInstPointByIndex(mibType, index);
        if (prof->head.active == FALSE)
        {
            break;
        }
    }
    sprintf(tmp, "%d", index);
    index = ProfNewInst(mibType, tmp, FALSE);
    ProfBackupByIndex(mibType, profAction, index, &profhead); /* Backup */ 
    prof = (CliForEngineerOnlyProfile *)ProfGetInstPointByName(mibType, tmp);/* Get pointer */
    strcpy(prof->command, cmd); /* Change value */
    ProfUpdate(profhead); /* Effect immediately */
    ProfFreeAllocList(profhead); /* Release */
    nvramWriteCommit(); /* Write config */
    websRedirect(wp, "uttCli.asp");
    return;
}

/***********************************************************************
 * 函 数 名：   formDefineCliForEngineerOnly
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
 ***********************************************************************/
extern void formDefineCliForEngineerOnly(void)
{
    websAspDefine(T("aspOutCommandList"), getCommandList);
    websAspDefine(T("aspOutMaxCommandNum"), GetMaxCommandNum);
    websFormDefine(T("formConfigCliForEngineerOnlyDel"), formConfigCliForEngineerOnlyDel);
    websFormDefine(T("formConfigCliForEngineerOnlyDelAll"), formConfigCliForEngineerOnlyDelAll);
    websFormDefine(T("formConfigCliForEngineerOnly"), formConfigCliForEngineerOnly);
}
#endif
