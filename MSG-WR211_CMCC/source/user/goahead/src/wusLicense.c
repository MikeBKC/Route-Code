/*
 *
 *      Copyright (c) 1996 UTT Technologies, Inc.
 *      All rights reserved.
 *      Use of copyright notice does not imply publication.
 *
 *
 *                      CONFIDENTIAL INFORMATION
 *                      ------------------------
 *      This Document contains Confidential Information or Trade Secrets,
 *      or both, which are the property of UTT Technologies, Inc.
 *      This document may not be copied, reproduced, reduced to any
 *      electronic medium or machine readable form or otherwise duplicated
 *      and the information herein may not be used, disseminated or
 *      otherwise disclosed, except with the prior written consent of
 *      UTT Technologies, Inc.
 *******************************************************************************/

/*
 *      
 *******************************************************************************/
#include "uttMachine.h"
#include "mib.h"
#include "webs.h"
#include "utils.h"
#include "wsIntrn.h"
#include "um.h"
#include "profacce.h"
#include "uttfunction.h"
#include "internet.h"
#include "comExeFun.h"
#include "argcmd.h"
#include "flash_api.h"
#include "translate.h"
#if (UTT_LICENSE == FYES)
//#define LICENSE_DBG
#ifdef  LICENSE_DBG
#define LIC_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define LIC_PRINTF(fmt, args...)
#endif

/******************************** Global Data *********************************/

/*********************************** Code *************************************/
/**********************************
 * usage : <% outputLicenseData %>
 * output information about License:
 * ip:
 * Mac:
 * isforever
 * isoutdate
 * includeip
 * includemac
 */
#if 1
extern int aspOutputLicenseData(int eid, webs_t wp, int argc, char **argv)
{
    LicenseProfile *uttlicense=NULL;

    uttlicense =(LicenseProfile *)ProfGetInstPointByIndex(MIB_PROF_LICENSE, 0);
    /*
     * define varliable 
     * get value from license
     */
    int need = 1;
    int isForverLicense;    
    int isOutDate = 0;
    int remainTime = 0;
    int includeip = 0;
    int includemac = 0;
    int includeuser = 0;
    char iptmp[16]={0};
    int curlicenseSeqnum = 0;
    /* char *buffer = NULL;*/
    /* int isneedlicense = 0;*/

    /* define varliable for license information
     * license content and license status
     * license impower ip mac time ...
     */
    char *needStr = T("need = %d;");
    char *isForverStr = T("isForver = %d;");
    char *isOutDateStr = T("isOutDate = %d;");
    char *remainTimeStr = T("remaintime = %d;");
    char *impowerTimeStr = T("impowertime = %d;");
    char *ipStr = T("ip = \"%s\";");
    char *includeipStr = T("includeip = %d;");
    char *macStr = T("mac = \"%s\";");
    char *includemacStr = T("includemac = %d;");
    char *userStr = T("userinfo = \"%s\";");
    char *includeuserStr = T("includeuser = %d;");
    char *curLicense = T("curLicense = %d;");
    /*char *productsnStr = T("productsn = \"%s\";");*/
    /*char *needlicenseStr = T("needlicense = %d;");*/
    /*if it is a forverLicense*/
    if(uttlicense->timeflag == FOREVER)  
	isForverLicense = 1;
    else
    {
	isForverLicense = -1;
	/*if it is outdate*/
	if(uttlicense->isOutdate) 
	    isOutDate = 1;
	else
	    isOutDate = -1;
	/* get remain time throught
	 * license impower time and runtime
	 */
	remainTime = uttlicense->impowerTime * 30 * 24 - uttlicense->runtime;
    }

    if(uttlicense->includeuser)
	includeuser = 1;
    else
	includeuser = -1;
    if(uttlicense->includeip)
	includeip = 1;
    else
	includeip = -1;
    if(uttlicense->includemac)
	includemac = 1;
    else
	includemac = -1;
#if 0
    if(needlicense == TRUE)
	isneedlicense = 1;
    else
	isneedlicense = 0;
#endif
    curlicenseSeqnum = uttlicense->num;  //现有序号
    /* write information
     * to License.asp 
     *
     */
    websWrite(wp, T("var need;"));
    websWrite(wp, T("var isForver;"));
    websWrite(wp, T("var isOutDate;"));
    websWrite(wp, T("var remaintime;"));
    websWrite(wp, T("var impowerTime;"));
    websWrite(wp, T("var ip;"));
    websWrite(wp, T("var includeip;"));
    websWrite(wp, T("var mac;"));
    websWrite(wp, T("var includemac;"));
    websWrite(wp, T("var includeuser;"));
    websWrite(wp, T("var userinfo;"));
    websWrite(wp, T("var curLicense;"));
   /* websWrite(wp, T("var needlicense;"));*/

    websWrite(wp, curLicense, curlicenseSeqnum);
    websWrite(wp, needStr, need);
    websWrite(wp, isForverStr, isForverLicense);
    websWrite(wp, isOutDateStr, isOutDate);
    websWrite(wp, remainTimeStr, remainTime);
    websWrite(wp, impowerTimeStr, uttlicense->impowerTime);
    if(uttlicense->includeip)
    {
	sprintf(iptmp,"%d.%d.%d.%d",(uttlicense->ip&0xff000000)>>24,(uttlicense->ip&0x00ff0000)>>16,(uttlicense->ip&0x0000ff00)>>8,(uttlicense->ip&0x000000ff));
	websWrite(wp, ipStr, iptmp);
    }
    websWrite(wp, includeipStr, includeip);
    if(uttlicense->includemac)
	websWrite(wp, macStr, uttlicense->mac);
    websWrite(wp, includemacStr, includemac);	
    if(uttlicense->includeuser)
	websWrite(wp, userStr, uttlicense->userinfor);
    websWrite(wp, includeuserStr,includeuser);
    /*websWrite(wp, productsnStr, uttlicense->sn);*/
    /*getlicense(&buffer);*/
    /*websWrite(wp,T("var license = \"%s\";"),buffer);*/

    /*websWrite(wp, needlicenseStr,isneedlicense);*/
    getTheErrorMsg(eid, wp, argc, argv);
    getTheRightMsg(eid, wp, argc, argv);
    return 0;
}
#endif

/* Tips_updatenum(int num)
 * input : update num  
 * output : null
 * output tips of remain num to web page
 * invoked by formUpdateLicenseFile()
 */
void Tips_updatenum( int num )
{
    const char *ErrMsg = NULL;
   /* if update license false
    * write remain num to web
    */
   if( 1 == num)
   {
	ErrMsg = getTransDataForC(C_LANG_INDEX_LICENSE_LOAD_ERR_FOUR);
	setTheErrorMsg(ErrMsg);
   }
   if( 2 == num)
   {
	ErrMsg = getTransDataForC(C_LANG_INDEX_LICENSE_LOAD_ERR_THREE);
	setTheErrorMsg(ErrMsg);
   }
   if( 3 == num)
   {
	ErrMsg = getTransDataForC(C_LANG_INDEX_LICENSE_LOAD_ERR_TWO);
	setTheErrorMsg(ErrMsg);
   }
   if( 4 == num)
   {
	ErrMsg = getTransDataForC(C_LANG_INDEX_LICENSE_LOAD_ERR_ONE);
	setTheErrorMsg(ErrMsg);
   }
   if( 5 == num)
   {
	ErrMsg = getTransDataForC(C_LANG_INDEX_LICENSE_LOAD_ERR_ZERO);
	setTheErrorMsg(ErrMsg);
   }
}

/*
 * usage "/goform/savetime"
 * change license runtime
 * 
 */
void formsavetime(webs_t wp, char *path, char *query)
{
    LicenseProfile *uttlicense=NULL;
    uttlicense =(LicenseProfile *)ProfGetInstPointByIndex(MIB_PROF_LICENSE, 0);
    char *time=NULL;
    /*get setted runtime from web page*/
    time = websGetVar(wp, T("time"), NULL);
    /*set license runtime*/
    uttlicense->runtime = atoi(time);
    websRedirect(wp, T("/License.asp")); /*redirect web page*/
    return;
}
//extern void Print_license( _st_license *license );
int updatenum  = 0;
extern unsigned int productSn;
/*
 * usage "/goform/UpdateLicenseFile"
 * for license.asp
 * get license content and len 
 * from web page license.asp
 * then save license content 
 */
void formUpdateLicenseFile(webs_t wp, char *path, char *query)
{

    char *fn;
    char *bn=NULL;
    char *postData = NULL;
    int fnlen;
    uchar curlicenseSeqnum;
    uchar loadlicenseSeqnum;
    Uint storedSerialNum;
    LicenseProfile *uttlicense=NULL;
    LicenseProfile *licenseBuf=NULL;
    int ret = 0;
    int curtask_status = TRUE;
    const char *ErrMsg = NULL, *ErrMsg2 = NULL;

    uttlicense =(LicenseProfile *)ProfGetInstPointByIndex(MIB_PROF_LICENSE, 0);
    licenseBuf =(LicenseProfile *)ProfGetInstPointByIndex(MIB_PROF_LICENSE, 1);
    if((uttlicense == NULL) || (licenseBuf == NULL))
    {
	return;
    }
#if 0
    licenseBuf->ip = 0;
    licenseBuf->mac = {0};
    licenseBuf->userinfor = NULL;
#endif
    licenseBuf->controltype = 0;
    memset( licenseBuf->userinfor, 0, MAXUSERLEN);

    a_assert(websValid(wp));
    fn = websGetVar(wp, T("filename"), NULL);
    printf("%s %d: raw filename=%s\n",__FUNCTION__,__LINE__,fn);

    if (fn != NULL && *fn != '\0') {
	if ((int)(bn = gstrrchr(fn, '/') + 1) == 1) {
	    if ((int)(bn = gstrrchr(fn, '\\') + 1) == 1) {
		bn = fn;
	    }
	}
    }
    /*  
     * get license content pointer
     * and license length
     */
    fnlen = wp->lenPostData;
    postData = wp->postData;
    wp->postData = NULL;
    wp->lenPostData = 0;
    /*
     * check update num
     * if num >= 5 fail
     */
    if( updatenum >= 5)
    {
	ErrMsg = getTransDataForC(C_LANG_INDEX_LICENSE_LOAD_TIME_MORE);
	setTheErrorMsg(ErrMsg);
	websRedirect(wp, T("/License.asp"));
	return;
    }
    /* 
     * check license len
     * if >1000x64 fail
     */
    if((fnlen > 1000 * 64) || (NULL == postData))
    {
	updatenum++;
	Tips_updatenum( updatenum );
	websRedirect(wp, T("/License.asp"));
	return;
    }
    /* decipher update license content
     * if lawless, fail
     */
    LIC_PRINTF("%s-%d: postData=%s, fnlen=%d\n",__func__,__LINE__,postData,fnlen);
    LIC_PRINTF("%s-%d: license_len=%d\n",__func__,__LINE__,Decipher_PartB(postData));
    DecipherUTTLicense(licenseBuf,postData);
#if 1
    
    if( licenseBuf->timeflag == LAWLESS )
    {
	updatenum++;
	Tips_updatenum(updatenum);
	websRedirect(wp, T("/License.asp"));
	return;
    }
    /* get licenseSeqnum */
    LIC_PRINTF("%s-%d: licensebf->num=%d\n",__func__,__LINE__,licenseBuf->num);
#if 0
    uttlicense->num = 0;
    licenseBuf->num++;
#endif
    curlicenseSeqnum = uttlicense->num;  //现有序号
    loadlicenseSeqnum = licenseBuf->num;   //导入序号
    storedSerialNum = productSn;  //获取设备的序列号
    LIC_PRINTF("%s-%d: curlicenseSeqnum=%d, loadlicenseSeqnum=%d,sn=%d\n",__func__,__LINE__,curlicenseSeqnum,loadlicenseSeqnum,storedSerialNum);
    /*
     * check license seq num
     * must > current seq num
     * if <=current,fail
     */
    if(loadlicenseSeqnum > curlicenseSeqnum)  //比较如果导入的大于现有的
    {
	updatenum = 0;
	/*为了保证当导入的license内容覆盖全局的license时
	 * 保证不影响时间更新task的运行状态
	 * 先保存现在的license的task运行zhuangtai，然后再恢复*/
	curtask_status = uttlicense->taskRunning;
	LIC_PRINTF("%s-%d: before taskRunning=%d\n",__func__,__LINE__,curtask_status);
	memcpy(uttlicense, licenseBuf, sizeof(LicenseProfile));
	uttlicense->taskRunning = curtask_status;
	LIC_PRINTF("%s-%d: after taskRunning=%d\n",__func__,__LINE__,uttlicense->taskRunning);
#if 1
#if (CONFIG_X86_CPU == 1)
	ret = flash_write_mtdname(LICENSE_FILE, postData, 4+8, (licenseBuf->len)*2);
	flash_write_mtdname(LICENSE_FILE, postData, 0x10000, (licenseBuf->len)*2);
#else
	ret = flash_write_mtdname("License", postData, 4, (licenseBuf->len)*2);
	flash_write_mtdname("License", postData, 0x10000, (licenseBuf->len)*2);
#endif
	if(ret == -1) {
	    LIC_PRINTF("%s-%d: \n",__func__,__LINE__);
	    setTheErrorMsg(T("write license content error"));
	    return;
	}
#endif
	Print_license(uttlicense );
	Print_license(licenseBuf);
	/* judge update license is forever license or not*/
	if(licenseBuf->timeflag == FOREVER)  
	{
	    LIC_PRINTF("%s-%d: \n",__func__,__LINE__);
	    uttlicense->timeflag = licenseBuf->timeflag;
	    uttlicense->isOutdate = 0;
	    uttlicense->authResult = 0xff;
	}
	else	
	/*update license isnot forever*/
	{
	    LIC_PRINTF("%s-%d: \n",__func__,__LINE__);
	    uttlicense->runtime = 0;
	    uttlicense->impowerTime = licenseBuf->impowerTime;
	    uttlicense->isOutdate = 0;
	    uttlicense->authResult = 0xff;
	    saveLicenseRuntime(uttlicense->runtime);
	    /* judge system task is running or not*/
	    if( uttlicense->taskRunning != TRUE )
	    /*if task isnot running ,to start a task*/
	    {
		LIC_PRINTF("%s-%d:license task start\n",__func__,__LINE__);
		ipsecTaskInit();
	    }
	}
    }
    else
    {
	updatenum++;
	Tips_updatenum(updatenum);
	websRedirect(wp, T("/License.asp"));
	return;
    }
#endif
    /*webWriteNvramToFlash();*/
    ErrMsg2 = getTransDataForC(C_LANG_INDEX_LICENSE_LOAD_SUCCESS);
    setTheRightMsg(ErrMsg2);
    websRedirect(wp, T("/License.asp"));
    return;
}

#endif /*UTT_LICENSE*/
#if(UTT_LICENSE != FYES)
extern int aspOutputLicenseData(int eid, webs_t wp, int argc, char **argv)
{
    websWrite(wp, T("var need=0;"));
    websWrite(wp, T("var ip=\"\";"));
    websWrite(wp, T("var mac=\"\";"));
    websWrite(wp, T("var userinfo=\"\";"));
    websWrite(wp, T("var impowertime=0;"));
    websWrite(wp, T("var remaintime=0;"));
    getTheErrorMsg(eid, wp, argc, argv);
    return 1;
}
#endif

/********************************************************************
 * 函数： formDefineLicense
 * 功能： 页面与交互转换函数
 * 创建： 2012-10-15
 * 作者： jhou 
 *******************************************************************/
extern void formDefineLicense(void)
{
    websAspDefine(T("outputLicenseData"), aspOutputLicenseData);
#if(UTT_LICENSE == FYES)
    websFormDefine(T("UpdateLicenseFile"),formUpdateLicenseFile);
    //websFormDefine(T("formsavetime"),formsavetime);
#endif
}
