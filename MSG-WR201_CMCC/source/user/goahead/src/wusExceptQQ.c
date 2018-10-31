#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include	"translate.h"
#if (EXCEPT_QQ == FYES)
#define TELECOM_QQ_LEN		11
#define TELECOM_QQ_HEAD_LEN	3
#define TELECOM_TRANSFORM_LEN	2

#ifdef  EXCEPT_QQ_DBG
#define EXCEPT_QQ_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define EXCEPT_QQ_PRINTF(fmt, args...)
#endif

/***********************
 * aspExceptQQData
 * guo.deyuan
 * 2012-04-26 
 */
static int aspExceptQQData(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_EXCEPT_QQ;
    int min, max, index, num =0;
    ExceptQQProfile *prof = NULL;
    
    /*define web variable*/
    websWrite(wp, "var exceptQQEnable=\"\";");
    websWrite(wp, "var exceptEnterpriseQQEnable=\"\";");
    websWrite(wp,"var indexID=new Array();");
    websWrite(wp,"var qqNumber=new Array();");
    websWrite(wp,"var remark=new Array();\n");
    char qqNumber[TELECOM_QQ_LEN+1];
    char qqNumberHead[TELECOM_QQ_HEAD_LEN+1];
    char qqNumberTrans[TELECOM_QQ_LEN-TELECOM_TRANSFORM_LEN+1];
    /*get SystemProfile Pointer*/
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibTypeSys, 0);
    /* get exceptQQEn value 
     * write value to web
     */
    if(profSys->exceptQQEn == FUN_ENABLE)
    {
	websWrite(wp, "exceptQQEnable=\"%s\";", T("on"));
	EXCEPT_QQ_PRINTF("%s-%d:exceptQQEnable = %d\n",__FUNCTION__,__LINE__,profSys->exceptQQEn);
    }
    else
    {
	websWrite(wp, "exceptQQEnable=\"%s\";",T("off"));
	EXCEPT_QQ_PRINTF("%s-%d:exceptQQEnable = %d\n",__FUNCTION__,__LINE__,profSys->exceptQQEn);
    }
    if(profSys->exceptEnterpriseQQEn == FUN_ENABLE)
    {
	websWrite(wp, "exceptEnterpriseQQEnable=\"%s\";", T("on"));
	EXCEPT_QQ_PRINTF("%s-%d:exceptEnterpriseQQEnable = %d\n",__FUNCTION__,__LINE__,profSys->exceptEnterpriseQQEn);
    }
    else
    {
	websWrite(wp, "exceptEnterpriseQQEnable=\"%s\";",T("off"));
	EXCEPT_QQ_PRINTF("%s-%d:exceptEnterpriseQQEnable = %d\n",__FUNCTION__,__LINE__,profSys->exceptEnterpriseQQEn);
    }
    /* get ExcepQQProfile inst max value and min */
    ProfInstLowHigh(mibType, &max, &min);
    EXCEPT_QQ_PRINTF("%s-%d:max=%d, min=%d\n",__FUNCTION__,__LINE__,max,min);
    for (index = min; index < max; index++)
    {
	/* get ExceptQQProfile Pointer by inst index */
	prof = (ExceptQQProfile *)ProfGetInstPointByIndex(mibType, index);
	if (prof->qqNumber > 0)
	{
	    /* output inst information
	     * indexID, qqNumber, and remark
	     */
	    EXCEPT_QQ_PRINTF("%s-%d:indexID=%s, qqHead=%u, qqNumber=%u, remark=%s\n",__FUNCTION__,__LINE__,prof->head.name,prof->phoneHead,prof->qqNumber,prof->remark);
	    websWrite(wp, "indexID[%d]=\"%s\";", num, prof->head.name);
	    memset(qqNumber, 0, sizeof(qqNumber));
	    memset(qqNumberHead, 0, sizeof(qqNumberHead));
	    memset(qqNumberTrans, 0, sizeof(qqNumberTrans));
	    EXCEPT_QQ_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
	    if(prof->phoneHead!=0)
	    {
		sprintf(qqNumberHead, "%u", prof->phoneHead);
		strncpy(qqNumber, qqNumberHead, TELECOM_QQ_HEAD_LEN);
		EXCEPT_QQ_PRINTF("%s-%d:qqNumber=%s\n",__FUNCTION__,__LINE__,qqNumber);
		sprintf(qqNumberTrans, "%u", prof->qqNumber);
		strncat(qqNumber, &qqNumberTrans[2], TELECOM_QQ_LEN-TELECOM_QQ_HEAD_LEN);
		EXCEPT_QQ_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
		websWrite(wp, "qqNumber[%d]=\"%s\";", num, qqNumber);
	    }
	    else
	    {
		sprintf(qqNumber,"%u",prof->qqNumber);
		websWrite(wp, "qqNumber[%d]=\"%s\";", num, qqNumber);
		EXCEPT_QQ_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
	    }
	    EXCEPT_QQ_PRINTF("%s-%d:qqNumber=%s\n",__FUNCTION__,__LINE__,qqNumber);
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
 * aspOutPutOneEceptQQInfo
 * output inst information specified name 
 * 2012-04-26
 8 guo.deyuan
 */
static int aspOutPutOneExceptQQInfo(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_EXCEPT_QQ;
    ExceptQQProfile *prof = NULL;
    int min = 0, max = 0, i = 0;
    char qqNumber[TELECOM_QQ_LEN+1];
    char qqNumberHead[TELECOM_QQ_HEAD_LEN+1];
    char qqNumberTrans[TELECOM_QQ_LEN-TELECOM_TRANSFORM_LEN+1];
    /* get assigned inst name*/
    char *editName;
    const char *errMsg=NULL;
    if (ejArgs(argc, argv, T("%s"), &editName) < 1) {
	errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_READ_DATA_ERR);
	setTheErrorMsg(errMsg);
    } else {
	EXCEPT_QQ_PRINTF("edit_name: %s\n",editName);
	/* get inst max value and min value */
	ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++) {
	    prof = (ExceptQQProfile *)ProfGetInstPointByIndex(mibType, i);
	    /* find inst by assigned name 
	     * compare inst name through editName 
	     */
	    if((prof != NULL) && (ProfInstIsFree(prof) == 0) &&
		    (editName != NULL) && (strcmp(editName, prof->head.name) == 0)) {
		memset(qqNumber, 0, TELECOM_QQ_LEN+1);
		memset(qqNumberHead, 0, TELECOM_QQ_HEAD_LEN+1);
		memset(qqNumberTrans, 0, TELECOM_QQ_LEN-TELECOM_TRANSFORM_LEN+1);
		EXCEPT_QQ_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
		EXCEPT_QQ_PRINTF("cur_indexID: %s, cur_qqNumbner: %u, cur_remark:%s\n",prof->head.name, prof->qqNumber, prof->remark);
		/*output assigned inst info*/
		websWrite(wp, "var curindexID=\"%s\";", prof->head.name);
		if(prof->phoneHead!=0)
		{
		    sprintf(qqNumberHead, "%u", prof->phoneHead);
		    strncpy(qqNumber, qqNumberHead, TELECOM_QQ_HEAD_LEN);
		    EXCEPT_QQ_PRINTF("%s-%d:qqNumber=%s\n",__FUNCTION__,__LINE__,qqNumber);
		    sprintf(qqNumberTrans, "%u", prof->qqNumber);
		    strncat(qqNumber, &qqNumberTrans[2], TELECOM_QQ_LEN-TELECOM_QQ_HEAD_LEN);
		    EXCEPT_QQ_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
		    websWrite(wp, "var curqqNumber=\"%s\";", qqNumber);
		}
		else
		{
		    sprintf(qqNumber,"%u",prof->qqNumber);
		    websWrite(wp, "var curqqNumber=\"%s\";", qqNumber);
		}
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
 * formConfigQQFilter
 * 2012-04-26
 * guo.deyuan
 */
static void formConfigExceptQQ(webs_t wp, char_t *path, char_t *query)
{
#if 1
    MibProfileType mibType = MIB_PROF_EXCEPT_QQ;
    ExceptQQProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    ProfChangeType profction = PROF_CHANGE_ADD;
    const char *errMsg=NULL;
    
    char *indexID = NULL;
    char *indexIDOld = NULL;
    char *qqNumberstr = NULL;
    char *remark = NULL;
    char *ActionStr = NULL;
    Uint32 qqNumber=0U;
    Uint32 phoneHead=0U;
    int min=0, max=0, index=0,i=0;
    Uint16 webIndexInst=0U;
    char_t instId[UTT_INST_NAME_LEN + 1];
    char telecomPhonehead[TELECOM_QQ_HEAD_LEN+1]={0};
    char telecomPhonetail[TELECOM_QQ_LEN-TELECOM_QQ_HEAD_LEN+1]={0};
    char telecomTransform[TELECOM_TRANSFORM_LEN+1]="27";

    /* get web value with webs element
     * indexID
     * qqNumber
     * remark ...
     */
    indexID = websGetVar(wp, "indexIDNew", T(""));
    indexIDOld = websGetVar(wp, "indexIDOld", T(""));
    qqNumberstr = websGetVar(wp, "qqNumber", T(""));
    remark = websGetVar(wp, "remark",T(""));
    ActionStr = websGetVar(wp, "Action",T(""));
    EXCEPT_QQ_PRINTF("%s-%d:qqNumberstr = %s,qqNumber=%lu\n",__FUNCTION__,__LINE__,qqNumberstr,strtoul(qqNumberstr,0,10));
    if(strlen(qqNumberstr)==TELECOM_QQ_LEN)
    {
	strncpy(telecomPhonehead, qqNumberstr, TELECOM_QQ_HEAD_LEN);
	telecomPhonehead[TELECOM_QQ_HEAD_LEN]=0;
	qqNumberstr +=TELECOM_QQ_HEAD_LEN;
	strncpy(telecomPhonetail, qqNumberstr, TELECOM_QQ_LEN-TELECOM_QQ_HEAD_LEN);
	telecomPhonetail[TELECOM_QQ_LEN-TELECOM_QQ_HEAD_LEN]=0;
	memset(qqNumberstr, 0, TELECOM_QQ_LEN+1);
	strncpy(qqNumberstr, telecomTransform, TELECOM_TRANSFORM_LEN);
	strncat(qqNumberstr, telecomPhonetail, TELECOM_QQ_LEN-TELECOM_QQ_HEAD_LEN);
	EXCEPT_QQ_PRINTF("%s-%d:telecomPhonehead=%s,telecoPhonetail=%s,teleconTransform=%s,qqNumberstr=%s\n",__FUNCTION__,__LINE__,telecomPhonehead,telecomPhonetail,telecomTransform,qqNumberstr);
    }
    qqNumber = strtoul(qqNumberstr,0,10);
    phoneHead = strtoul(telecomPhonehead,0,10);
    EXCEPT_QQ_PRINTF("%s-%d:phonehead=%u,qqNumber = %u\n",__FUNCTION__,__LINE__,phoneHead,qqNumber);
    /* separate into two case
     * one is the action add inst  
     * two is the action modify inst 
     */
    if(strcmp(ActionStr, "add") == 0)
    {
	EXCEPT_QQ_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
	/*get inst max value and min value*/
	ProfInstLowHigh(mibType, &max, &min);
	EXCEPT_QQ_PRINTF("%s-%d: max = %d, min = %d\n",__FUNCTION__,__LINE__, max, min);
	/* find free inst index */
	for(i=min;i<max;i++)
	{
	    prof = (ExceptQQProfile *)ProfGetInstPointByIndex(mibType, i);
	    EXCEPT_QQ_PRINTF("%s-%d:index = %d\n",__FUNCTION__,__LINE__,i);
	    /*qqNumber is 0 ,find*/
	    if(prof->qqNumber == qqNumber)
	    {
		EXCEPT_QQ_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
		errMsg = getTransDataForC(C_LANG_INDEX_QQ_NUM_EXIST);
		setTheErrorMsgOnFree(errMsg);
		return;
	    }
	}
	if(i==max)
	{
	    webIndexInst = getWebInstName(&(nvramProfile->exceptqqInstRec.instRecMap), &(nvramProfile->exceptqqInstRec.instNameIndex));
	    if(webIndexInst==0)
	    {
		errMsg = getTransDataForC(C_LANG_INDEX_OPERATION_FAIL);
		setTheErrorMsg(errMsg);
		websRedirect(wp, "QQFilter.asp");
		return;
	    }
	    memset(instId, 0, sizeof(instId));
	    /*define inst name with ID+index*/
	    snprintf(instId, UTT_INST_NAME_LEN, "ID%d", webIndexInst);
	    if (ProfGetInstIndexByName(mibType, instId) != PROFFAIL) 
	    {
		errMsg = getTransDataForC(C_LANG_INDEX_INST_EXIST);
		setTheErrorMsg(errMsg);
		websRedirect(wp, "QQFilter.asp");
		return;
	    }
	/*create new inst and get index*/
	index = ProfNewInst(mibType,instId,FALSE);
	EXCEPT_QQ_PRINTF("%s-%d:index = %d\n",__FUNCTION__,__LINE__,index);
	profction=PROF_CHANGE_ADD;
	}
    }
    else
    {
	/*second case modify inst*/
	index = ProfGetInstIndexByName(mibType, indexID);/*get inst index by name*/
	EXCEPT_QQ_PRINTF("%s-%d:index=%d, indexIDOld = %s\n",__FUNCTION__,__LINE__,index,indexIDOld);
	/*not find error and reurn*/
	if(index==PROFFAIL)
	{
	    setTheErrorMsg(T("not exist"));
	    websRedirect(wp, "QQFilter.asp");
	    return;
	}
	strcpy(instId,indexID);
	profction=PROF_CHANGE_EDIT;
    }
    /*backup profile inst*/
    EXCEPT_QQ_PRINTF("%s-%d: name: %s\n",__FUNCTION__,__LINE__,instId);
    prof = (ExceptQQProfile *)ProfGetInstPointByIndex(mibType, index);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0))
    {
    ProfBackupByIndex(mibType, profction, index, &profhead);
    /* set inst variable value
     * qqNumber
     * remark
     */
    prof->qqNumber = qqNumber;
    prof->phoneHead = phoneHead;
    //memset(prof->remark,0,MAX_QQ_REMARK_LEN+1);
    strcpy(prof->remark,remark);
    EXCEPT_QQ_PRINTF("%s-%d:qqHead=%u,qqNumber = %u, remark = %s\n",__FUNCTION__,__LINE__,prof->phoneHead,prof->qqNumber, prof->remark);
    }
    /*update profile inst
     * and save profile
     */
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
#if 0
out:
#endif
    websRedirect(wp, "QQFilter.asp");
    return;
#endif
}

/*****************************
 * formExceptQQDel
 * 2012-04-26
 * guo.deyuan
 */
static void formExceptQQDel(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_EXCEPT_QQ;
    ExceptQQProfile *prof = NULL;
    struProfAlloc *profList = NULL;
    char *indexID=NULL;
    char *tmp=NULL;
    const char *errMsg=NULL;
    Uint16 webIndexInst = 0;

    /*get delete inst name*/
    indexID = websGetVar(wp, T("delstr"),T(""));
    EXCEPT_QQ_PRINTF("%s - %d: del ID: %s\n",__FUNCTION__,__LINE__,indexID);

    if(indexID != NULL)
    {
	/*separata inst name*/
	tmp = strtok(indexID, UTT_SPLIT_SIGN_STR);
	EXCEPT_QQ_PRINTF("%s-%d: tmp: %s\n",__FUNCTION__,__LINE__,tmp);
	while(tmp!=NULL)
	{
	    /*back inst and delete*/
	    ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profList);
	    prof = (ExceptQQProfile *)ProfGetInstPointByName(mibType, tmp);
	    EXCEPT_QQ_PRINTF("%s-%d: ID: %s\n",__FUNCTION__,__LINE__,prof->head.name);
	    if(prof)
	    {
		/*delete inst by name*/
		EXCEPT_QQ_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
		ProfDelInstByName(mibType, tmp);
		/* 清除记录map */
		if (strncmp(tmp, "ID", 2) == 0) 
		{ 
			webIndexInst = (Uint16)strtol(&(tmp[2]), NULL, 10);
			clearPortMap(&(nvramProfile->exceptqqInstRec.instRecMap), webIndexInst);
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
    
    websRedirect(wp, T("QQFilter.asp"));
    return;
}
	
/******************************
 * formExceptQQDelAll
 * 2012-04-26
 * guo.deyuan
 */
static void formExceptQQDelAll(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType=MIB_PROF_EXCEPT_QQ;
    ExceptQQProfile *prof = NULL;
    struProfAlloc *profList = NULL;
    int max=0, min=0, i=0;
    
    /*get inst max value and min value*/
    a_assert(wp);
    ProfInstLowHigh(mibType, &max, &min);
    EXCEPT_QQ_PRINTF("%s-%d;max=%d, min=%d\n",__FUNCTION__,__LINE__,max,min);
    /* delete all inst
     * from min index to max 
     */
    for(i=min;i<max;i++)
    {
	prof = (ExceptQQProfile *)ProfGetInstPointByIndex(mibType, i);
	if((prof!=NULL) && (ProfInstIsFree(prof) == 0))
	{
	    /*delete inst*/
	    ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, i, &profList);
	    ProfDelInstByIndex(mibType, i);
	}
    }
    /* 更新map记录 */
    clearAllPortMap(&(nvramProfile->exceptqqInstRec.instRecMap));
    nvramProfile->exceptqqInstRec.instNameIndex = 0;

    
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
    websRedirect(wp, "QQFilter.asp");
    return;
}

/******************************
 * formExceptQQGlobalConfig
 * 2012-04-26
 * guo.deyuan 
 */
static void formExceptQQGlobalConfig(webs_t wp, char_t *path, char_t *query)
{
    char *exceptQQEnable = NULL;
    char *ExceptEnterpriseQQEnable = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    FunEnableEnum enable = FUN_DISABLE;
    FunEnableEnum EnterpriseEnable = FUN_DISABLE;
    struProfAlloc *profhead = NULL;
    ProfChangeType profction;

    /* get web value */
    exceptQQEnable = websGetVar(wp, "ExceptQQEnable", T(""));
    EXCEPT_QQ_PRINTF("%s-%d:exceptQQEn = %s\n",__FUNCTION__,__LINE__,exceptQQEnable);
    ExceptEnterpriseQQEnable = websGetVar(wp, "ExceptEnterpriseQQEnable", T(""));
    EXCEPT_QQ_PRINTF("%s-%d:ExceptEnterpriseQQEnable = %s\n", __func__, __LINE__, ExceptEnterpriseQQEnable);
    
    /* get SystemProfile */
    profction = PROF_CHANGE_EDIT;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);
    ProfBackupByIndex(mibType, profction, 0, &profhead);
    
    /* compare web value 
     * if value on
     * set portMirrorEn enable, or set disable
     */
    if(strcmp(exceptQQEnable, "on") == 0) 
    {
        enable = FUN_ENABLE;
	EXCEPT_QQ_PRINTF("%s-%d:enable = %d\n",__FUNCTION__,__LINE__,enable);
    }
    if(strcmp(ExceptEnterpriseQQEnable, "on") == 0) 
    {
	EnterpriseEnable = FUN_ENABLE;
	EXCEPT_QQ_PRINTF("%s-%d:EnterpriseEnable = %d\n", __func__, __LINE__, EnterpriseEnable);
    }
    if(profSys->exceptQQEn != enable)
    {
	profSys->exceptQQEn = enable;
	EXCEPT_QQ_PRINTF("%s-%d:set exceptQQEn = %d\n",__FUNCTION__,__LINE__,profSys->exceptQQEn);
    }
    if(profSys->exceptEnterpriseQQEn != EnterpriseEnable)
    {
	profSys->exceptEnterpriseQQEn = EnterpriseEnable;
	EXCEPT_QQ_PRINTF("%s-%d:set exceptEnterpriseQQEn = %d\n",__FUNCTION__,__LINE__,profSys->exceptEnterpriseQQEn);
    }
    
    /* update systemProfile
     * and write to flash
     */
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    websRedirect(wp, "QQFilter.asp");
    return;

}
#define QQFilterMaxLen	MAX_EXCEPT_QQ_PROFILES * (TELECOM_QQ_LEN + MAX_QQ_REMARK_LEN + 4)
extern void formConfigQQFilterImport(webs_t wp, char *path, char *query)
{
    char *data, *qqname, *remark;
    char defaultRemark[2] = "";
    char username[TELECOM_QQ_LEN + 1];
    Uint32 qqNumber = 0u;
    MibProfileType mibType = MIB_PROF_EXCEPT_QQ;
    char *pUser[MAX_EXCEPT_QQ_PROFILES] = {0};
    char user[QQFilterMaxLen];  
    int j=0, k=0, i=0;
    Uint16 webIndexInst = 0;
    char instId[UTT_INST_NAME_LEN + 1] = {0};
    int instIndex = 0;
    ProfChangeType profction;
    struProfAlloc *profList  = NULL;
    ExceptQQProfile *prof = NULL;
    const char *ErrMsg = NULL;
    Uint32 phoneHead = 0u;
    char telecomPhonehead[TELECOM_QQ_HEAD_LEN+1]={0};
    char telecomPhonetail[TELECOM_QQ_LEN-TELECOM_QQ_HEAD_LEN+1]={0};
    char telecomTransform[TELECOM_TRANSFORM_LEN+1]="27";

    data = websGetVar(wp, T("data"), NULL);
    memset(user, 0, sizeof(user));              // * 初始化数组 */
    for(j = 0;j<strlen(data);j++)               // * 将脚本中得到的数据拷贝到数组中 */
    {
	user[j] = data[j];
    }
    pUser[0] = strtok(user,";");
    if(!pUser[0])				// * 实例信息为空 */
    {
	websRedirect(wp,T("QQFilterImport.asp"));
	return ;
    }

    for(k = 1; k < MAX_EXCEPT_QQ_PROFILES ; k++)                        // * 从data中逐个读取出实例变量 */
    {
	pUser[k] = strtok(NULL,";");
	if(!pUser[k])
	{
	    break;
	}
    }
    for(i = 0;i<MAX_EXCEPT_QQ_PROFILES;i++)
    {
	EXCEPT_QQ_PRINTF("pUser[%d] = %s\n", i, pUser[i]);
	if(!pUser[i])
	{
	    break;
	}
	qqname = strtok(pUser[i], " ");	// * 得到用户名 */
	remark = strtok(NULL, " ");		// * 得到密码 */ 


	EXCEPT_QQ_PRINTF("remark = %s\n", remark);
	if (remark == NULL) {
	    remark = &defaultRemark;
	}
	EXCEPT_QQ_PRINTF("qqname = %s\n",qqname);
	EXCEPT_QQ_PRINTF("remark = %s\n", remark);
	if ((qqname == NULL) || (remark == NULL)) {
	    ErrMsg = getTransDataForC(C_LANG_INDEX_CONFIG_ARGS_ERR);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}

	memset(username, 0, sizeof(username));
	memset(telecomPhonehead, 0, sizeof(telecomPhonehead));
	memset(telecomPhonetail, 0, sizeof(telecomPhonetail));
	strncpy(username, qqname, TELECOM_QQ_LEN);
	EXCEPT_QQ_PRINTF("username = %s\n", username);
	if(strlen(username)== TELECOM_QQ_LEN)
	{
	    strncpy(telecomPhonehead, username, TELECOM_QQ_HEAD_LEN);
	    telecomPhonehead[TELECOM_QQ_HEAD_LEN]=0;
	    strncpy(telecomPhonetail, &username[TELECOM_QQ_HEAD_LEN], TELECOM_QQ_LEN-TELECOM_QQ_HEAD_LEN);
	    telecomPhonetail[TELECOM_QQ_LEN-TELECOM_QQ_HEAD_LEN]=0;
	    memset(username, 0, sizeof(username));
	    strncpy(username, telecomTransform, TELECOM_TRANSFORM_LEN);
	    strncat(username, telecomPhonetail, TELECOM_QQ_LEN-TELECOM_QQ_HEAD_LEN);
	    EXCEPT_QQ_PRINTF("%s-%d:telecomPhonehead=%s,telecoPhonetail=%s,teleconTransform=%s,username=%s\n",__FUNCTION__,__LINE__,telecomPhonehead,telecomPhonetail,telecomTransform,username);
	}
	qqNumber = strtoul(username,0,10);
	phoneHead = strtoul(telecomPhonehead,0,10);
	/*
	 * 用户名是否重复
	 */
	if (ProfGetInstPointByValue(mibType, OFFANDSIZE(ExceptQQProfile, qqNumber), TINT, username, -1) != PROFFAIL) {
	    ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_ALREADY_EXITS);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}

	webIndexInst = getWebInstName(&(nvramProfile->exceptqqInstRec.instRecMap), &(nvramProfile->exceptqqInstRec.instNameIndex));
	/* portmap已满 */
	if (webIndexInst == 0) {
	    ErrMsg = getTransDataForC(C_LANG_INDEX_OPERATION_FAIL);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	snprintf(instId, UTT_INST_NAME_LEN, "ID%d", webIndexInst);
	/* 已存在实例名 */
	if (ProfGetInstIndexByName(mibType, instId) != PROFFAIL) {
	    ErrMsg = getTransDataForC(C_LANG_INDEX_CASE_ALREADY_EXITS);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	instIndex = ProfNewInst(mibType, instId, FALSE);

	if(instIndex == PROFFAIL) {
	    clearPortMap(&(nvramProfile->exceptqqInstRec.instRecMap), webIndexInst);
	    ErrMsg = getTransDataForC(C_LANG_INDEX_MAXIMUM_NUM_HAS_REACHED);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	profction = PROF_CHANGE_ADD;

	ProfBackupByIndex(mibType, profction, instIndex, &profList);
	prof = (ExceptQQProfile *)ProfGetInstPointByIndex(mibType, instIndex);

	/* 索引为实例名  */
	ProfSetNameByPoint(prof, instId);
	prof->phoneHead = phoneHead;
	prof->qqNumber = qqNumber;
	strncpy(prof->remark, remark, MAX_QQ_REMARK_LEN);
    } 

    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
out:
    websRedirect(wp, "QQFilter.asp");
    return ;
}
/***********************************
 *formConfigQQFilterExport(webs_t wp, char *path, char *query)
 *
 *action= /goform/formQQFilterExport
 *
 *write QQFilter information to assigned file setted by webpage
 *
 *add by zkh
 ************************************/

static Boolean ReadQQFilterUserFromProfile(char *buf)
{
    MibProfileType mibType = MIB_PROF_EXCEPT_QQ;
    ExceptQQProfile *prof = NULL;
    int min = 0, max = 0, i = 0, totalrecs = 0;
    char username[TELECOM_QQ_LEN + 1];
    char qqNumberHead[TELECOM_QQ_HEAD_LEN+1];
    char qqNumberTrans[TELECOM_QQ_LEN-TELECOM_TRANSFORM_LEN+1];

    ProfInstLowHigh(mibType, &max, &min);

    EXCEPT_QQ_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
    for (i = min; i < max; i++) {
	prof = (ExceptQQProfile *)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
	    memset(username, 0, sizeof(username));
	    if(prof->phoneHead != 0)
	    {
		sprintf(qqNumberHead, "%u", prof->phoneHead);
		strncpy(username, qqNumberHead, TELECOM_QQ_HEAD_LEN);
		EXCEPT_QQ_PRINTF("%s-%d:username=%s\n",__FUNCTION__,__LINE__,username);
		sprintf(qqNumberTrans, "%u", prof->qqNumber);
		strncat(username, &qqNumberTrans[2], TELECOM_QQ_LEN-TELECOM_QQ_HEAD_LEN);
		EXCEPT_QQ_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
	    } else {
		sprintf(username, "%u", prof->qqNumber);
	    }
	    if( QQFilterMaxLen > ( strlen(buf) + strlen(username) ) )
	    {
		strcat(buf,username);
	    }
	    if( QQFilterMaxLen > ( strlen(buf) + strlen(" ") ) )
	    {
		strcat(buf," ");
	    }
	    if( QQFilterMaxLen > ( strlen(buf) + strlen(prof->remark) ) )
	    {
		strcat(buf,prof->remark);
	    }
	    if( QQFilterMaxLen > ( strlen(buf) + strlen("\r\n") ) )
	    {
		strcat(buf,"\r\n");
	    }

	    totalrecs++;
	}
    }

    EXCEPT_QQ_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
    return TRUE;
}
extern void formConfigQQFilterExport(webs_t wp, char *path, char *query)
{
    char filename[30];
    char buf[QQFilterMaxLen];
    int len = 0;

    EXCEPT_QQ_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
    memset( buf, 0, QQFilterMaxLen );
    memset( filename, 0, sizeof(filename));
    strcpy(filename, "QQFilterserverinfo.txt");

    ReadQQFilterUserFromProfile(buf);
    len = strlen(buf);
    wimDownloadFile(wp,filename,buf,len);
    EXCEPT_QQ_PRINTF("%s-%d:%s\n",__FUNCTION__,__LINE__,buf);
    return;
}

/******************************
 * formDefineExceptQQ
 * 2012-04-26
 * guo.deyuan 
 */
void formDefineExceptQQ(void)
{
    /*define asp function*/
    websAspDefine(T("aspExceptQQData"), aspExceptQQData);	    
    websAspDefine(T("aspOutPutOneExceptQQInfo"),aspOutPutOneExceptQQInfo);
    /*define form function*/
    websFormDefine(T("ConfigExceptQQ"), formConfigExceptQQ);    
    websFormDefine(T("formExceptQQDel"), formExceptQQDel);
    websFormDefine(T("formExceptQQDelAll"), formExceptQQDelAll);
    websFormDefine(T("formExceptQQGlobalConfig"), formExceptQQGlobalConfig);
    websFormDefine(T("formQQFilterImport"), formConfigQQFilterImport);
    websFormDefine(T("formQQFilterExport"), formConfigQQFilterExport);
}
#endif
