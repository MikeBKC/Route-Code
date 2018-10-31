#include "uttMachine.h"
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "mib.h"
#include "profacce.h"
#include "uttfunction.h"
#include "argcmd.h"
#include "ifmisc.h"
#include "wuScript.h"
#include "wusL2tpServer.h"
#include "wusPptpServer.h"
#include "translate.h"
#if (IPV6_RADVD == FYES)
static int  ConverCharToInt(char *str){
    int res=0; 
    if(str!=NULL &&str[0]!='\0'){ 
     while(*str!='\0'){
	res=res*10+(*str-'0');
	str++;
	}
    }
    return res;
}


static  void  ConverIntToChar(int res, char *des){
    sprintf(des,"%d",res);
}
/*
 * 函数名称：aspOutputRADVDConfig
 * 函数功能：向页面输出相关信息
 */
static int aspOutputRADVDConfig(int eid, webs_t wp, int argc, char_t **argv)
{
   MibProfileType mibType=MIB_PROF_RADVD;
   RadvdProfile   *radProf=NULL;
   Ipv6PrefixListProfile *prof=NULL;
   char tmp[5];
   int index,max,min;
   long PrefixstaleTime = 71582788;
   radProf=ProfGetInstPointByIndex(mibType, 0); 
   if(radProf!=NULL&&radProf->head.name[0]!='\0'){
	if(radProf->head.active==1){
	    websWrite(wp, "var radvdEnabled =\"1\";");
	}else{
	    websWrite(wp, "var radvdEnabled =\"0\";");
	}
        
	websWrite(wp,"var manageEnabled = %d;",radProf->manageEnabled);
	websWrite(wp,"var otherEnabled = %d;",radProf->otherEnabled);
	ConverIntToChar(radProf->noticeInterval,tmp);
	websWrite(wp, "var adInterval=\"%s\";",tmp);
	
	ConverIntToChar(radProf->routerPreferences,tmp);
	websWrite(wp, "var routeType=\"%s\";",tmp);
	
	ConverIntToChar(radProf->MTU_config,tmp);
	websWrite(wp, "var mtuSet = %d;",radProf->MTU_config);

	ConverIntToChar(radProf->routerLifetime,tmp);
	websWrite(wp, "var routeLifeTime = %d;",radProf->routerLifetime);
    }
    else{
    //TODO
    }
    ProfInstLowHigh(MIB_PROF_IPV6_NOTLISTPREFIXS, &max, &min);
    for(index=min;index<max;index++)
    {
	prof = (Ipv6PrefixListProfile *)ProfGetInstPointByIndex(MIB_PROF_IPV6_NOTLISTPREFIXS, index);
	if(prof!=NULL &&  (ProfInstIsFree(prof) == 0)){ 
	    if(prof->PrefixstaleTime < PrefixstaleTime){
		PrefixstaleTime = prof->PrefixstaleTime;
	    }
	}

    }
    getTheErrorMsg(eid, wp, argc, argv);  
    websWrite(wp,"PrefixstaleTime= %ld;",PrefixstaleTime);
    return 0;
}

/*
 *函数名称：formRDVDConfig
 *函数功能：配置页面生效函数
 */
static void formRDVDConfig(webs_t wp, char_t *path, char_t *query)
{
    int min, max, index, i;
    MibProfileType mibTypeGlobal=MIB_PROF_RADVD;
    RadvdProfile   *radProf=NULL;
    struProfAlloc *profhead=NULL;
    ProfChangeType profAction=PROF_CHANGE_ADD;
    char *radvdEnabled ,*manageEnabled,*otherEnabled,*adInterval,*routeType,*mtuSet,*routeLifeTime;
    
    index = 0;

    radvdEnabled = websGetVar(wp, T("radvdEnabled"), T("0"));
    manageEnabled = websGetVar(wp, T("manageEnabled"), T("0"));
    otherEnabled = websGetVar(wp, T("otherEnabled"), T("0"));
    adInterval = websGetVar(wp, T("adInterval"), T("30"));
    routeType =  websGetVar(wp, T("routeType"), T("1"));
    mtuSet = websGetVar(wp, T("mtuSet"), T("1500"));
    routeLifeTime = websGetVar(wp, T("routeLifeTime"), T("3600"));
   printf("%s %s %s %s %s\n",radvdEnabled,adInterval,routeType,mtuSet,routeLifeTime); 
    radProf=(RadvdProfile *)ProfGetInstPointByIndex(mibTypeGlobal, index);
    if(radProf!=NULL){
	ProfBackupByIndex(mibTypeGlobal, profAction, index, &profhead);
	    if(strncmp(radvdEnabled,"on",3)==0){
		radProf->head.active=1;
	    }else{  
		radProf->head.active=0;
	    }
	    if(strncmp(manageEnabled,"on",3)==0){
		radProf->manageEnabled=1;
	    }else{  
		radProf->manageEnabled=0;
	    }
 	    if(strncmp(otherEnabled,"on",3)==0){
		radProf->otherEnabled=1;
	    }else{  
		radProf->otherEnabled=0;
	    }
	    printf("adInterval=%d\n",ConverCharToInt(adInterval));
	    radProf->noticeInterval=ConverCharToInt(adInterval);
	    radProf->routerPreferences=ConverCharToInt(routeType);
	    radProf->MTU_config=ConverCharToInt(mtuSet);
	    radProf->routerLifetime=ConverCharToInt(routeLifeTime);
    }
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    websRedirect(wp, "radvd.asp");
}
/*
 * 页面句柄
 */
extern void websDefineRADVDConfig(void) {
    websAspDefine(T("outputRADVDConfig"), aspOutputRADVDConfig);
    websFormDefine(T("ConfigRDVDConfig"), formRDVDConfig);
    return;
}
#endif
