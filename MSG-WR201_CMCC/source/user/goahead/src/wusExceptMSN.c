#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include	"translate.h"
#if (EXCEPT_MSN == FYES)

#ifdef  EXCEPT_MSN_DBG
#define EXCEPT_MSN_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define EXCEPT_MSN_PRINTF(fmt, args...)
#endif

/***********************
 * aspExceptMSNData
 * guo.deyuan
 * 2012-05-02 
 */
static int aspExceptMSNData(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_EXCEPT_MSN;
    int min, max, index, num =0;
    ExceptMSNProfile *prof = NULL;
    
    /*define web variable*/
    websWrite(wp, "var exceptMSNEnable=\"\";");
    websWrite(wp,"var indexID=new Array();");
    websWrite(wp,"var msnNumber=new Array();");
    websWrite(wp,"var remark=new Array();\n");
    
    /*get SystemProfile Pointer*/
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibTypeSys, 0);
    /* get exceptQQEn value 
     * write value to web
     */
    if(profSys->exceptMSNEn == FUN_ENABLE)
    {
	websWrite(wp, "exceptMSNEnable=\"%s\";", T("on"));
	EXCEPT_MSN_PRINTF("%s-%d:exceptMSNEnable = %d\n",__FUNCTION__,__LINE__,profSys->exceptMSNEn);
    }
    else
    {
	websWrite(wp, "exceptMSNEnable=\"%s\";",T("off"));
	EXCEPT_MSN_PRINTF("%s-%d:exceptMSNEnable = %d\n",__FUNCTION__,__LINE__,profSys->exceptMSNEn);
    }
    /* get ExcepMSNProfile inst max value and min */
    ProfInstLowHigh(mibType, &max, &min);
    EXCEPT_MSN_PRINTF("%s-%d:max=%d, min=%d\n",__FUNCTION__,__LINE__,max,min);
    for (index = min; index < max; index++)
    {
	/* get ExceptMSNProfile Pointer by inst index */
	prof = (ExceptMSNProfile *)ProfGetInstPointByIndex(mibType, index);
	if (strlen(prof->msnNumber) != 0U)
	{
	    /* output inst information
	     * indexID, msnNumber, and remark
	     */
	    EXCEPT_MSN_PRINTF("%s-%d:indexID=%s, msnNumber=%s, remark=%s\n",__FUNCTION__,__LINE__,prof->head.name,prof->msnNumber,prof->remark);
	    websWrite(wp, "indexID[%d]=\"%s\";", num, prof->head.name);
	    websWrite(wp, "msnNumber[%d]=\"%s\";", num, prof->msnNumber);
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

/***********************
 * aspOutPutOneEceptMSNInfo
 * output inst information specified name 
 * 2012-05-02
 8 guo.deyuan
 */
static int aspOutPutOneExceptMSNInfo(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_EXCEPT_MSN;
    ExceptMSNProfile *prof = NULL;
    int min = 0, max = 0, i = 0;
    /* get assigned inst name*/
    char *editName;
    const char *errMsg=NULL;
    if (ejArgs(argc, argv, T("%s"), &editName) < 1) {
	errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_READ_DATA_ERR);
	setTheErrorMsg(errMsg);
    } else {
	EXCEPT_MSN_PRINTF("edit_name: %s\n",editName);
	/* get inst max value and min value */
	ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++) {
	    prof = (ExceptMSNProfile *)ProfGetInstPointByIndex(mibType, i);
	    /* find inst by assigned name 
	     * compare inst name through editName 
	     */
	    if((prof != NULL) && (ProfInstIsFree(prof) == 0) &&
		    (editName != NULL) && (strcmp(editName, prof->head.name) == 0)) {
		EXCEPT_MSN_PRINTF("cur_indexID: %s, cur_msnNumbner: %s, cur_remark:%s\n",prof->head.name, prof->msnNumber, prof->remark);
		/*output assigned inst info*/
		websWrite(wp, "var curindexID=\"%s\";", prof->head.name);
		websWrite(wp, "var curmsnNumber=\"%s\";", prof->msnNumber);
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

/**********************
 * formConfigExceptMSN
 * 2012-05-02
 * guo.deyuan
 */
static void formConfigExceptMSN(webs_t wp, char_t *path, char_t *query)
{
#if 1
    MibProfileType mibType = MIB_PROF_EXCEPT_MSN;
    ExceptMSNProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction = PROF_CHANGE_ADD;
    const char *errMsg=NULL;

    char *indexID = NULL;
    char *indexIDOld = NULL;
    char *msnNumberstr = NULL;
    char *remark = NULL;
    char *ActionStr = NULL;
    int min=0, max=0, index=0,i=0;
    Uint16 webIndexInst=0U;
    char_t instId[UTT_INST_NAME_LEN + 1]={0};

    /* get web value with webs element
     * indexID
     * msnNumber
     * remark ...
     */
    indexID = websGetVar(wp, "indexIDNew", T(""));
    indexIDOld = websGetVar(wp, "indexIDOld", T(""));
    msnNumberstr = websGetVar(wp, "msnNumber", T(""));
    remark = websGetVar(wp, "remark",T(""));
    ActionStr = websGetVar(wp, "Action",T(""));
    EXCEPT_MSN_PRINTF("%s-%d:msnNumber = %s\n",__FUNCTION__,__LINE__,msnNumberstr);
    if(strlen(msnNumberstr) != 0U)
    {
    /* separate into two case
     * one is the action add inst  
     * two is the action modify inst 
     */
    if(strcmp(ActionStr, "add") == 0)
    {
	EXCEPT_MSN_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
	/*get inst max value and min value*/
	ProfInstLowHigh(mibType, &max, &min);
	EXCEPT_MSN_PRINTF("%s-%d: max = %d, min = %d\n",__FUNCTION__,__LINE__, max, min);
	/* find free inst index */
	for(i=min;i<max;i++)
	{
	    prof = (ExceptMSNProfile *)ProfGetInstPointByIndex(mibType, i);
	    /*msnNumber is 0 ,find*/
	    if(strcmp(prof->msnNumber,msnNumberstr) == 0U)
	    {
		EXCEPT_MSN_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
		errMsg = getTransDataForC(C_LANG_INDEX_MSN_EXIST);
		setTheErrorMsgOnFree(errMsg);
		return;
	    }
	}
	if(i==max)
	{
	    webIndexInst = getWebInstName(&(nvramProfile->exceptmsnInstRec.instRecMap), &(nvramProfile->exceptmsnInstRec.instNameIndex));
	    EXCEPT_MSN_PRINTF("%s-%d, webIndexInst = %d\n", __FUNCTION__, __LINE__, webIndexInst);
	    /* portmap已满 */
	    if (webIndexInst == 0U) 
	    {
		errMsg = getTransDataForC(C_LANG_INDEX_OPERATION_FAIL);
		setTheErrorMsg(errMsg);
		websRedirect(wp, "MSNFilter.asp");
		return;
	    }
	    /*define inst name with ID+index*/
	    memset(instId, 0, sizeof(instId));
	    sprintf(instId,"ID%d", webIndexInst);
	    if (ProfGetInstIndexByName(mibType, instId) != PROFFAIL) 
	    {
		errMsg = getTransDataForC(C_LANG_INDEX_INST_EXIST);
		setTheErrorMsg(errMsg);
		websRedirect(wp, "MSNFilter.asp");
		return;
	    }
	    /*create new inst and get index*/
	    index = ProfNewInst(mibType,instId,FALSE);
	    EXCEPT_MSN_PRINTF("%s-%d:index = %d\n",__FUNCTION__,__LINE__,index);
	    profAction=PROF_CHANGE_ADD;
	}
    }
    else
    {
	/*second case modify inst*/
	index = ProfGetInstIndexByName(mibType, indexID);/*get inst index by name*/
	EXCEPT_MSN_PRINTF("%s-%d:index=%d, indexIDOld = %s\n",__FUNCTION__,__LINE__,index,indexIDOld);
	/*not find error and reurn*/
	if(index==PROFFAIL)
	{
	    setTheErrorMsg(T("not exist"));
	    websRedirect(wp, "MSNFilter.asp");
	    return;
	}
	strcpy(instId,indexID);
	profAction=PROF_CHANGE_EDIT;
    }
    /*backup profile inst*/
    EXCEPT_MSN_PRINTF("%s-%d: name: %s\n",__FUNCTION__,__LINE__,instId);
    prof = (ExceptMSNProfile *)ProfGetInstPointByIndex(mibType, index);
    ProfBackupByIndex(mibType, profAction, index, &profhead);
    /* set inst variable value
     * msnNumber
     * remark
     */
    strcpy(prof->msnNumber, msnNumberstr);
    strcpy(prof->remark,remark);
    EXCEPT_MSN_PRINTF("%s-%d:msnNumber = %s, remark = %s\n",__FUNCTION__,__LINE__,prof->msnNumber, prof->remark);
    /*update profile inst
     * and save profile
     */
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    }
    else
    {
	errMsg = getTransDataForC(C_LANG_INDEX_MSN_NULL);
	setTheErrorMsg(errMsg);
    }
    nvramWriteCommit();
    websRedirect(wp, "MSNFilter.asp");
    return;
#endif
}

/*****************************
 * formExceptMSNDel
 * 2012-04-26
 * guo.deyuan
 */
static void formExceptMSNDel(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_EXCEPT_MSN;
    ExceptMSNProfile *prof = NULL;
    struProfAlloc *profList = NULL;
    char *indexID=NULL;
    char *tmp=NULL;
    const char *errMsg=NULL;
    Uint16 webIndexInst = 0;

    /*get delete inst name*/
    indexID = websGetVar(wp, T("delstr"),T(""));
    EXCEPT_MSN_PRINTF("%s - %d: del ID: %s\n",__FUNCTION__,__LINE__,indexID);

    if(indexID != NULL)
    {
	/*separata inst name*/
	tmp = strtok(indexID, UTT_SPLIT_SIGN_STR);
	EXCEPT_MSN_PRINTF("%s-%d: tmp: %s\n",__FUNCTION__,__LINE__,tmp);
	while(tmp!=NULL)
	{
	    /*back inst and delete*/
	    ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profList);
	    prof = (ExceptMSNProfile *)ProfGetInstPointByName(mibType, tmp);
	    EXCEPT_MSN_PRINTF("%s-%d: ID: %s\n",__FUNCTION__,__LINE__,prof->head.name);
	    if(prof)
	    {
		/*delete inst by name*/
		EXCEPT_MSN_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
		ProfDelInstByName(mibType, tmp);
		/* 清除记录map */
		if (strncmp(tmp, "ID", 2) == 0) 
		{ 
			webIndexInst = (Uint16)strtol(&(tmp[2]), NULL, 10);
			clearPortMap(&(nvramProfile->exceptmsnInstRec.instRecMap), webIndexInst);
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
    
    websRedirect(wp, T("MSNFilter.asp"));
    return;
}
	
/******************************
 * formExceptMSNDelAll
 * 2012-05-02
 * guo.deyuan
 */
static void formExceptMSNDelAll(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType=MIB_PROF_EXCEPT_MSN;
    ExceptMSNProfile *prof = NULL;
    struProfAlloc *profList = NULL;
    int max=0, min=0, i=0;
    
    /*get inst max value and min value*/
    a_assert(wp);
    ProfInstLowHigh(mibType, &max, &min);
    EXCEPT_MSN_PRINTF("%s-%d;max=%d, min=%d\n",__FUNCTION__,__LINE__,max,min);
    /* delete all inst
     * from min index to max 
     */
    for(i=min;i<max;i++)
    {
	prof = (ExceptMSNProfile *)ProfGetInstPointByIndex(mibType, i);
	if((prof!=NULL) && (ProfInstIsFree(prof) == 0))
	{
	    /*delete inst*/
	    ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, i, &profList);
	    ProfDelInstByIndex(mibType, i);
	}
    }
    /* 更新map记录 */
    clearAllPortMap(&(nvramProfile->exceptmsnInstRec.instRecMap));
    nvramProfile->exceptmsnInstRec.instNameIndex = 0;
    
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
    websRedirect(wp, "MSNFilter.asp");
    return;
}

/******************************
 * formExceptQQGlobalConfig
 * 2012-04-26
 * guo.deyuan 
 */
static void formExceptMSNGlobalConfig(webs_t wp, char_t *path, char_t *query)
{
    char *exceptMSNEnable = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    FunEnableEnum enable = FUN_DISABLE;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;

    /* get web value */
    exceptMSNEnable = websGetVar(wp, "ExceptMSNEnable", T(""));
    EXCEPT_MSN_PRINTF("%s-%d:exceptMSNEn = %s\n",__FUNCTION__,__LINE__,exceptMSNEnable);
    
    /* get SystemProfile */
    profAction = PROF_CHANGE_EDIT;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);
    ProfBackupByIndex(mibType, profAction, 0, &profhead);
    
    /* compare web value 
     * if value on
     * set portMirrorEn enable, or set disable
     */
    if(strcmp(exceptMSNEnable, "on") == 0) {
        enable = FUN_ENABLE;
	EXCEPT_MSN_PRINTF("%s-%d:enable = %d\n",__FUNCTION__,__LINE__,enable);
    }
    if(profSys->exceptMSNEn != enable)
    {
	profSys->exceptMSNEn = enable;
	EXCEPT_MSN_PRINTF("%s-%d:set exceptMSNEn = %d\n",__FUNCTION__,__LINE__,profSys->exceptMSNEn);
    }
    
    /* update systemProfile
     * and write to flash
     */
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    websRedirect(wp, "MSNFilter.asp");
    return;

}

/******************************
 * formDefineExceptMSN
 * 2012-05-02
 * guo.deyuan 
 */
void formDefineExceptMSN(void)
{
    /*define asp function*/
    websAspDefine(T("aspExceptMSNData"), aspExceptMSNData);	    
    websAspDefine(T("aspOutPutOneExceptMSNInfo"),aspOutPutOneExceptMSNInfo);
    /*define form function*/
    websFormDefine(T("ConfigExceptMSN"), formConfigExceptMSN);    
    websFormDefine(T("formExceptMSNDel"), formExceptMSNDel);
    websFormDefine(T("formExceptMSNDelAll"), formExceptMSNDelAll);
    websFormDefine(T("formExceptMSNGlobalConfig"), formExceptMSNGlobalConfig);
}
#endif
