#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include	"translate.h"

#if (EXCEPT_ALI == FYES)
//#define EXCEPT_ALI_DBG
#ifdef  EXCEPT_ALI_DBG
#define EXCEPT_ALI_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define EXCEPT_ALI_PRINTF(fmt, args...)
#endif

/********************************************************************
 * 函数： aspExceptAliData
 * 功能： 页面输出
 * 创建： 2013-09-20
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
static int aspExceptAliData(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_EXCEPT_ALI;
    int min, max, index, num =0;
    ExceptAliProfile *prof = NULL;

    /*define web variable*/
    websWrite(wp,"var exceptAliEnable=\"\";");
    websWrite(wp,"var indexID=new Array();");
    websWrite(wp,"var AliNumber=new Array();");
    websWrite(wp,"var remark=new Array();\n");

    /*get SystemProfile Pointer*/
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibTypeSys, 0);
    /* get exceptQQEn value 
     * write value to web
     */
    if(profSys->exceptAliEn == FUN_ENABLE)
    {
        websWrite(wp, "exceptAliEnable=\"%s\";", T("on"));
        EXCEPT_ALI_PRINTF("%s-%d:exceptAliEnable = %d\n",__FUNCTION__,__LINE__,profSys->exceptAliEn);
    }
    else
    {
        websWrite(wp, "exceptAliEnable=\"%s\";",T("off"));
        EXCEPT_ALI_PRINTF("%s-%d:exceptAliEnable = %d\n",__FUNCTION__,__LINE__,profSys->exceptAliEn);
    }
    /* get ExcepAliProfile inst max value and min */
    ProfInstLowHigh(mibType, &max, &min);
    EXCEPT_ALI_PRINTF("%s-%d:max=%d, min=%d\n",__FUNCTION__,__LINE__,max,min);
    for (index = min; index < max; index++)
    {
        /* get ExceptAliProfile Pointer by inst index */
        prof = (ExceptAliProfile *)ProfGetInstPointByIndex(mibType, index);
        if (strlen(prof->aliNumber) != 0U)
        {
            /* output inst information
             * indexID, aliNumber, and remark
             */
            EXCEPT_ALI_PRINTF("%s-%d:indexID=%s, aliNumber=%s, remark=%s\n",__FUNCTION__,__LINE__,prof->head.name,prof->aliNumber,prof->remark);
            websWrite(wp, "indexID[%d]=\"%s\";", num, prof->head.name);
            websWrite(wp, "AliNumber[%d]=\"%s\";", num, prof->aliNumber);
            websWrite(wp, "remark[%d]=\"%s\";", num, prof->remark);
            num++;
        }
    }

    websWrite(wp,"\nvar totalrecs=%d;",num);
    websWrite(wp,"var max_totalrecs=%d;\n",max);
    /* get sys error msg */
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
/********************************************************************
 * 函数： aspOutPutOneExceptAliInfo
 * 功能： 输出一条配置
 * 创建： 2013-09-20
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
static int aspOutPutOneExceptAliInfo(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_EXCEPT_ALI;
    ExceptAliProfile *prof = NULL;
    int min = 0, max = 0, i = 0;
    /* get assigned inst name*/
    char *editName;
    const char *errMsg=NULL;

    if (ejArgs(argc, argv, T("%s"), &editName) < 1) {
        errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_READ_DATA_ERR);
        setTheErrorMsg(errMsg);
    } else {
        EXCEPT_ALI_PRINTF("edit_name: %s\n",editName);
        /* get inst max value and min value */
        ProfInstLowHigh(mibType, &max, &min);
        for (i = min; i < max; i++) {
            prof = (ExceptAliProfile *)ProfGetInstPointByIndex(mibType, i);
            /* find inst by assigned name */
            /* compare inst name through editName */
            if((prof != NULL) && (ProfInstIsFree(prof) == 0) &&
                    (editName != NULL) && (strcmp(editName, prof->head.name) == 0)) {
                EXCEPT_ALI_PRINTF("cur_indexID: %s, cur_aliNumbner: %s, cur_remark:%s\n",prof->head.name, prof->aliNumber, prof->remark);
                /*output assigned inst info*/
                websWrite(wp, "var curindexID=\"%s\";", prof->head.name);
                websWrite(wp, "var curaliNumber=\"%s\";", prof->aliNumber);
                websWrite(wp, "var curremark=\"%s\";\n", prof->remark);
                websWrite(wp, "var curindex=%d;\n", i);
                break;
            }
        }
    }
    /*get sys error msg*/
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
/********************************************************************
 * 函数： formConfigExceptAli
 * 功能： 保存页面配置
 * 创建： 2013-09-20
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
static void formConfigExceptAli(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_EXCEPT_ALI;
    ExceptAliProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction = PROF_CHANGE_ADD;
    const char *errMsg=NULL;

    char *indexID = NULL;
    char *indexIDOld = NULL;
    char *aliNumberstr = NULL;
    char *remark = NULL;
    char *ActionStr = NULL;
    int min=0, max=0, index=0,i=0;
    Uint16 webIndexInst=0U;
    char_t instId[UTT_INST_NAME_LEN + 1]={0};

    /* get web value with webs element
     * indexID
     * aliNumber
     * remark ...
     */
    indexID = websGetVar(wp, "indexIDNew", T(""));
    indexIDOld = websGetVar(wp, "indexIDOld", T(""));
    aliNumberstr = websGetVar(wp, "aliNumber", T(""));
    remark = websGetVar(wp, "remark",T(""));
    ActionStr = websGetVar(wp, "Action",T(""));
    EXCEPT_ALI_PRINTF("%s-%d:aliNumber = %s\n",__FUNCTION__,__LINE__,aliNumberstr);
    if(strlen(aliNumberstr) != 0U)
    {
        /* separate into two case
         * one is the action add inst  
         * two is the action modify inst 
         */
        if(strcmp(ActionStr, "add") == 0)
        {
            EXCEPT_ALI_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
            /*get inst max value and min value*/
            ProfInstLowHigh(mibType, &max, &min);
            EXCEPT_ALI_PRINTF("%s-%d: max = %d, min = %d\n",__FUNCTION__,__LINE__, max, min);
            /* find free inst index */
            for(i=min;i<max;i++)
            {
                prof = (ExceptAliProfile *)ProfGetInstPointByIndex(mibType, i);
                /*aliNumber is 0 ,find*/
                if(strcmp(prof->aliNumber,aliNumberstr) == 0U)
                {
                    EXCEPT_ALI_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
                    errMsg = getTransDataForC(C_LANG_INDEX_ALI_EXIST);
                    setTheErrorMsgOnFree(errMsg);
                    return;
                }
            }
            if(i==max)
            {
                webIndexInst = getWebInstName(&(nvramProfile->exceptaliInstRec.instRecMap), &(nvramProfile->exceptaliInstRec.instNameIndex));
                EXCEPT_ALI_PRINTF("%s-%d, webIndexInst = %d\n", __FUNCTION__, __LINE__, webIndexInst);
                /* portmap已满 */
                if (webIndexInst == 0U) 
                {
                    errMsg = getTransDataForC(C_LANG_INDEX_OPERATION_FAIL);
                    setTheErrorMsg(errMsg);
                    websRedirect(wp, "AlitalkFilter.asp");
                    return;
                }
                /*define inst name with ID+index*/
                memset(instId, 0, sizeof(instId));
                sprintf(instId,"ID%d", webIndexInst);
                if (ProfGetInstIndexByName(mibType, instId) != PROFFAIL) 
                {
                    errMsg = getTransDataForC(C_LANG_INDEX_INST_EXIST);
                    setTheErrorMsg(errMsg);
                    websRedirect(wp, "AlitalkFilter.asp");
                    return;
                }
                /*create new inst and get index*/
                index = ProfNewInst(mibType,instId,FALSE);
                EXCEPT_ALI_PRINTF("%s-%d:index = %d\n",__FUNCTION__,__LINE__,index);
                profAction=PROF_CHANGE_ADD;
            }
        }
        else
        {
            /*second case modify inst*/
            index = ProfGetInstIndexByName(mibType, indexID);/*get inst index by name*/
            EXCEPT_ALI_PRINTF("%s-%d:index=%d, indexIDOld = %s\n",__FUNCTION__,__LINE__,index,indexIDOld);
            /*not find error and reurn*/
            if(index==PROFFAIL)
            {
                errMsg = getTransDataForC(C_LANG_INDEX_CHANGED_CASE_NOT_EXITS);
                setTheErrorMsg(errMsg);
                websRedirect(wp, "AlitalkFilter.asp");
                return;
            }
            strcpy(instId,indexID);
            profAction=PROF_CHANGE_EDIT;
        }
        /*backup profile inst*/
        EXCEPT_ALI_PRINTF("%s-%d: name: %s\n",__FUNCTION__,__LINE__,instId);
        prof = (ExceptAliProfile *)ProfGetInstPointByIndex(mibType, index);
        ProfBackupByIndex(mibType, profAction, index, &profhead);
        /* set inst variable value
         * aliNumber
         * remark
         */
        strncpy(prof->aliNumber, aliNumberstr,MAX_QQ_REMARK_LEN);
        strcpy(prof->remark,remark);
        EXCEPT_ALI_PRINTF("%s-%d:aliNumber = %s, remark = %s\n",__FUNCTION__,__LINE__,prof->aliNumber, prof->remark);
        /*update profile inst
         * and save profile
         */
        ProfUpdate(profhead);
        ProfFreeAllocList(profhead);
    }
    else
    {
        errMsg = getTransDataForC(C_LANG_INDEX_ALI_NULL);
        setTheErrorMsg(errMsg);
    }
    nvramWriteCommit();
    websRedirect(wp, "AlitalkFilter.asp");
    return;
}
/********************************************************************
 * 函数： formExceptAliDel
 * 功能： 删除若干条配置
 * 创建： 2013-09-20
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
static void formExceptAliDel(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_EXCEPT_ALI;
    ExceptAliProfile *prof = NULL;
    struProfAlloc *profList = NULL;
    char *indexID=NULL;
    char *tmp=NULL;
    const char *errMsg=NULL;
    Uint16 webIndexInst = 0;

    /*get delete inst name*/
    indexID = websGetVar(wp, T("delstr"),T(""));
    EXCEPT_ALI_PRINTF("%s - %d: del ID: %s\n",__FUNCTION__,__LINE__,indexID);

    if(indexID != NULL)
    {
        /*separata inst name*/
        tmp = strtok(indexID, UTT_SPLIT_SIGN_STR);
        EXCEPT_ALI_PRINTF("%s-%d: tmp: %s\n",__FUNCTION__,__LINE__,tmp);
        while(tmp!=NULL)
        {
            /*back inst and delete*/
            ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profList);
            prof = (ExceptAliProfile *)ProfGetInstPointByName(mibType, tmp);
            EXCEPT_ALI_PRINTF("%s-%d: ID: %s\n",__FUNCTION__,__LINE__,prof->head.name);
            if(prof)
            {
                /*delete inst by name*/
                EXCEPT_ALI_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
                ProfDelInstByName(mibType, tmp);
		/* 清除记录map */
		if (strncmp(tmp, "ID", 2) == 0) 
		{ 
			webIndexInst = (Uint16)strtol(&(tmp[2]), NULL, 10);
			clearPortMap(&(nvramProfile->exceptaliInstRec.instRecMap), webIndexInst);
		}
            }
            /*get the next inst name*/
            tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
        }
        /*update profile and save*/
        ProfUpdate(profList);
        ProfFreeAllocList(profList);
        nvramWriteCommit();
    }
    else
    {
        errMsg = getTransDataForC(C_LANG_INDEX_ID_NOT_FIND);
        setTheErrorMsg(errMsg);
    }

    websRedirect(wp, T("AlitalkFilter.asp"));
    return;
}
/********************************************************************
 * 函数： formExceptAliDelAll
 * 功能： 删除所有配置
 * 创建： 2013-09-20
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
static void formExceptAliDelAll(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType=MIB_PROF_EXCEPT_ALI;
    ExceptAliProfile *prof = NULL;
    struProfAlloc *profList = NULL;
    int max=0, min=0, i=0;

    /*get inst max value and min value*/
    a_assert(wp);
    ProfInstLowHigh(mibType, &max, &min);
    EXCEPT_ALI_PRINTF("%s-%d;max=%d, min=%d\n",__FUNCTION__,__LINE__,max,min);
    /* delete all inst*/
    /* from min index to max */
    for(i=min;i<max;i++)
    {
        prof = (ExceptAliProfile *)ProfGetInstPointByIndex(mibType, i);
        if((prof!=NULL) && (ProfInstIsFree(prof) == 0))
        {
            /*delete inst*/
            ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, i, &profList);
            ProfDelInstByIndex(mibType, i);
        }
    }
    /* 更新map记录 */
    clearAllPortMap(&(nvramProfile->exceptaliInstRec.instRecMap));
    nvramProfile->exceptaliInstRec.instNameIndex = 0;

    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
    websRedirect(wp, "AlitalkFilter.asp");
    return;
}
/********************************************************************
 * 函数： formExceptAliGlobalConfig
 * 功能： 输出阿里旺旺全局配置
 * 创建： 2013-09-20
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
static void formExceptAliGlobalConfig(webs_t wp, char_t *path, char_t *query)
{
    char *exceptAliEnable = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    FunEnableEnum enable = FUN_DISABLE;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;

    /* get web value*/
    exceptAliEnable = websGetVar(wp, "ExceptAliEnable", T(""));
    EXCEPT_ALI_PRINTF("%s-%d:exceptAliEn = %s\n",__FUNCTION__,__LINE__,exceptAliEnable);

    /* get SystemProfile*/
    profAction = PROF_CHANGE_EDIT;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);
    ProfBackupByIndex(mibType, profAction, 0, &profhead);

    /* compare web value
     * if value on
     * set portMirrorEn enable, or set disable
     */
    if(strcmp(exceptAliEnable, "on") == 0) {
        enable = FUN_ENABLE;
        EXCEPT_ALI_PRINTF("%s-%d:enable = %d\n",__FUNCTION__,__LINE__,enable);
    }
    if(profSys->exceptAliEn != enable)
    {
        profSys->exceptAliEn = enable;
        EXCEPT_ALI_PRINTF("%s-%d:set exceptAliEn = %d\n",__FUNCTION__,__LINE__,profSys->exceptAliEn);
    }

    /* update systemProfile*/
    /* and write to flash*/
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    websRedirect(wp, "AlitalkFilter.asp");
    return;

}
/********************************************************************
 * 函数： formDefineExceptAli
 * 功能： 定义页面接口函数
 * 创建： 2013-09-20
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
void formDefineExceptAli(void)
{
    /*define asp function*/
    websAspDefine(T("aspExceptAliData"), aspExceptAliData);	    
    websAspDefine(T("aspOutPutOneExceptAliInfo"),aspOutPutOneExceptAliInfo);
    /*define form function*/
    websFormDefine(T("ConfigExceptAli"), formConfigExceptAli);    
    websFormDefine(T("formExceptAliDel"), formExceptAliDel);
    websFormDefine(T("formExceptAliDelAll"), formExceptAliDelAll);
    websFormDefine(T("formExceptAliGlobalConfig"), formExceptAliGlobalConfig);
}
#endif
