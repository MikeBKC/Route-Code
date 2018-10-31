#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#ifdef CONFIG_DEFAULTS_KERNEL_2_6_21
#include  <linux/types.h>
#include  <linux/socket.h>
#include  <linux/if.h>
#endif

#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "management.h"
#include "wireless.h"
#include "oid.h"
#include "linux/autoconf.h"
#include "uttfunction.h"
#include "mib.h"
#include "profacce.h"
#include "translate.h"

#if(IPV6_NOTLISTPREFIXS == FYES)
static int ConverCharToInt(char *src){
    int length=0;
    while(*src!='\0'){
	length=length*10+(*src-'0');
	src++;
    }
    return length;
}

static int  CheckIpv6PrefixByName(MibProfileType profType,char *instantname)
{
    MibProfileType  mibType = profType;
    Ipv6PrefixListProfile *prof=NULL;
    int min,max;
    int index;
    ProfInstLowHigh(mibType, &max, &min);
    for(index=min;index<max;index++){
	prof = (Ipv6PrefixListProfile *)ProfGetInstPointByIndex(mibType,index);
	if((prof!=NULL) && (ProfInstIsFree(prof) == PROFOK) && (strcmp(prof->DesPrefix,instantname)==0)){
		return index;
	}
    }
    return -1;
    
}
static void formConfigIpv6PrefixList(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_IPV6_NOTLISTPREFIXS;
    struProfAlloc *profhead = NULL;
    Ipv6PrefixListProfile  *prof = NULL,*prof1=NULL;
    int instIndex=-1,index=-1;
    int min,max;
    ProfChangeType profAction;
    const char *errMsg=NULL;
    char *DesPrefix=NULL,*DesPrefixold=NULL,*DesPrefixLength=NULL,*PrefixstaleTime=NULL,*action=NULL,*AdvAutoEnabled=NULL;

    action = websGetVar(wp, T("Action"), "");
    DesPrefix = websGetVar(wp, T("DesPrefix"), "");
    DesPrefixold = websGetVar(wp, T("DesPrefixold"), "");
    DesPrefixLength = websGetVar(wp, T("DesPrefixLength"), "");
    PrefixstaleTime = websGetVar(wp, T("PrefixstaleTime"), "");
    AdvAutoEnabled = websGetVar(wp, T("AdvAutoEnabled"), "");

    //printf("action:%s DesPrefix:%s DesPrefixold:%s  DesPrefixLength:%s PrefixstaleTime:%s\n",action,DesPrefix,DesPrefixold,DesPrefixLength,PrefixstaleTime);
    if((action==NULL )||(DesPrefix==NULL) ||(DesPrefixLength==NULL) ||(PrefixstaleTime==NULL))//检查新添加的prof是否合法
    {
	errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_PARAMETER_ERR);
	setTheErrorMsg(errMsg);
	goto out;
    }
   //由于通告前缀长度大于12个字符,所以在检查是否存在实例,不能比较head.name字段了.该字段只是验证实例是否存在。 
    instIndex = CheckIpv6PrefixByName(mibType,DesPrefix);
    index= CheckIpv6PrefixByName(mibType,DesPrefixold);
    if (strcmp(action, "add") == 0) {
	if(instIndex != PROFFAIL) {
	    errMsg = getTransDataForC(C_LANG_INDEX_ROUTE_NAME_REPEAT);
	    setTheErrorMsg(errMsg);
	    goto out;
	}

	ProfInstLowHigh(mibType, &max, &min);
	for(instIndex= min;instIndex<max;instIndex++){
	    prof1 =(Ipv6PrefixListProfile *) ProfGetInstPointByIndex(mibType,instIndex);
	    if(prof1!=NULL && strlen(prof1->head.name)==0){
		break;
	    }
	}
	if(instIndex >=max) {
	    errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_MAX_TOTAL_ERR);
	    setTheErrorMsg(errMsg);
	    goto out;
	}
	profAction = PROF_CHANGE_ADD;
    } else {
	if(instIndex != PROFFAIL && index == PROFFAIL) {
	    errMsg = getTransDataForC(C_LANG_INDEX_ROUTE_NAME_NOT_FIND);
	    setTheErrorMsg(errMsg);
	    goto out;
	}
	instIndex = index;
	profAction = PROF_CHANGE_EDIT;
    }

    ProfBackupByIndex(mibType, profAction, instIndex, &profhead);

    prof = (Ipv6PrefixListProfile *)ProfGetInstPointByIndex(mibType, instIndex);

    ProfSetNameByPoint(prof, DesPrefix);
    memcpy(prof->DesPrefix,DesPrefix,sizeof(prof->DesPrefix));
    prof->DesPrefixLength=ConverCharToInt(DesPrefixLength);
    prof->PrefixstaleTime=ConverCharToInt(PrefixstaleTime);
    if(strcmp(AdvAutoEnabled,"on") == 0){
	prof->head.active = 1;
    }else{
	prof->head.active = 0;
    }
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
    websRedirect(wp, "ipv6PrefixList.asp");
}
static void formConfigIpv6PrefixListDel(webs_t wp, char_t *path, char_t *query)
{
    int num=0;
    int min, max, index = 0;
    char *ptr;
    MibProfileType mibType=  MIB_PROF_IPV6_NOTLISTPREFIXS;
    Ipv6PrefixListProfile *prof=NULL;
    struProfAlloc *profhead = NULL;
    const char *errMsg=NULL;
    char *route = NULL, *tmp = NULL;
    route = websGetVar(wp, T("delstr"), T(""));
    if (route == NULL) {
	errMsg = getTransDataForC(C_LANG_INDEX_ROUTE_NAME_NOT_FIND);
	setTheErrorMsg(errMsg);
	goto out;
    } 
    if (strlen(route) != 0) {
	tmp=strtok_r(route, UTT_SPLIT_SIGN_STR,&ptr);
	while(tmp!=NULL){
	    ProfInstLowHigh(mibType, &max, &min);
	    for(index=min;index<max;index++)
	    {
		prof = (Ipv6PrefixListProfile *) ProfGetInstPointByIndex(mibType,index);
		if(prof!=NULL && ProfInstIsFree(prof) == PROFOK &&(strcmp(prof->DesPrefix,tmp))==0)
		{
		    break;
		}
		
	    }
	    if(index<max){	
		ProfBackupByIndex(mibType, PROF_CHANGE_DEL, index, &profhead);
		ProfDelInstByIndex(mibType, index);/*删除已有实例*/	
	    }	
	    else
	    {
		errMsg = getTransDataForC(C_LANG_INDEX_SUBMIT_FOR_NO_LOGIN);
		setTheErrorMsgOnFree(errMsg);
	    }
	    tmp=strtok_r(NULL, ",",&ptr);
	}
    }
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();/*保存flash*/
out:
    websRedirect(wp, "ipv6PrefixList.asp");
 
}

static void formConfigIpv6PrefixListDelAll(webs_t wp, char_t *path, char_t *query)
{
    int min = 0, max = 0;
    int i = 0;
    int back = 0;
    MibProfileType mibType = MIB_PROF_IPV6_NOTLISTPREFIXS;
    Pointer  *prof=NULL;
    struProfAlloc *profList = NULL;

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = ProfGetInstPointByIndex(mibType, i);
	if(prof!=NULL && (ProfInstIsFree(prof) == 0)){
	    if(back == 0) {
		ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, i, &profList);
		back=1;
	    }
	    ProfDelInstByIndex(mibType, i);
	}
    }
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
    websRedirect(wp, "ipv6PrefixList.asp");

}
static int getIpv6PrefixList(int eid, webs_t wp, int argc, char_t **argv)
{
    int num=0;	    
    int min,max,index=0;
    MibProfileType  mibType = MIB_PROF_IPV6_NOTLISTPREFIXS;
    Ipv6PrefixListProfile *prof=NULL;
    websWrite(wp,"var DesPrefix=new Array();");
    websWrite(wp,"var DesPrefixLength=new Array();");
    websWrite(wp,"var PrefixstaleTime=new Array();");
    websWrite(wp,"var AdvAutoEnabled=new Array();");

    ProfInstLowHigh(mibType, &max, &min);
    num=min;
    for(index=min;index<max;index++)
    {
	prof = (Ipv6PrefixListProfile *)ProfGetInstPointByIndex(mibType, index);
	if(prof!=NULL &&  (ProfInstIsFree(prof) == 0)){
	    websWrite(wp,"DesPrefix[%d]=\"%s\";",num,prof->DesPrefix);
	    websWrite(wp,"DesPrefixLength[%d]= %d;",num,prof->DesPrefixLength);
	    websWrite(wp,"PrefixstaleTime[%d]= %d;",num,prof->PrefixstaleTime);
	    if(prof->head.active == 1){
		websWrite(wp,"AdvAutoEnabled[%d]=\"是\";",num);
	    }else{
		websWrite(wp,"AdvAutoEnabled[%d]=\"否\";",num);
	    }
	    num++;
	}
	else
	{
	    continue;
	}
    }
    websWrite(wp,"var totalrecs=%d;", num);
    websWrite(wp,"var max_totalrecs=%d;", MAX_IPV6_NOTLISTPREFIXS_PROFILES);/*最大条目数，即性能指标*/
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
static int getAdInterval(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_RADVD;
    RadvdProfile   *radProf = NULL;
    radProf=ProfGetInstPointByIndex(mibType, 0);
    websWrite(wp,"var adInterval = %d;", radProf->noticeInterval);
    return 0;
}

static int aspOutputIpv6PrefixListOneInfo(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_IPV6_NOTLISTPREFIXS;
    Ipv6PrefixListProfile  *prof = NULL;
    RadvdProfile   *radProf = NULL;
    int min = 0, max = 0, i = 0;

    char *editName;
    const char *errMsg=NULL;

    if (ejArgs(argc, argv, T("%s"), &editName) < 1) {
	errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_READ_DATA_ERR);
	setTheErrorMsg(errMsg);
    } else {
	ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++) {
	    prof = (Ipv6PrefixListProfile *)ProfGetInstPointByIndex(mibType, i);
	    if((prof != NULL) && (ProfInstIsFree(prof) == 0) &&
		    (editName != NULL) && (strcmp(editName, prof->DesPrefix) == 0)) {
		websWrite(wp, "DesPrefix = \"%s\";", prof->DesPrefix);
		websWrite(wp, "DesPrefixLength = \"%d\";", prof->DesPrefixLength);
		websWrite(wp, "PrefixstaleTime = \"%d\";", prof->PrefixstaleTime);
		websWrite(wp, "AdvAutoEnabled = \"%d\";", prof->head.active);
		break;
	    }
	}
    }
    radProf=ProfGetInstPointByIndex(MIB_PROF_RADVD, 0);
    websWrite(wp,"var adInterval=%d;", radProf->noticeInterval);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
/***********************************************************************
 * 函 数 名：   formDefineIpv6RouteConfig
 * 创建日期：	2014-9-25
 * 修改日期：
 * 附加说明：	无
 ***********************************************************************/
extern  void formDefineIpv6PrefixList(void)
{
    websFormDefine(T("ConfigIpv6PrefixList"), formConfigIpv6PrefixList);
    websFormDefine(T("ConfigIpv6PrefixListDel"), formConfigIpv6PrefixListDel);
    websFormDefine(T("ConfigIpv6PrefixListDelAll"), formConfigIpv6PrefixListDelAll);
    websAspDefine(T("outputIpv6PrefixListConfig"), getIpv6PrefixList);
    websAspDefine(T("aspOutputIpv6PrefixListOneInfo"), aspOutputIpv6PrefixListOneInfo);
    websAspDefine(T("getAdInterval"), getAdInterval);
}
#endif
