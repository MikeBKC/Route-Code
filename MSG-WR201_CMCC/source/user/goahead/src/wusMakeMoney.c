#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include	"wuScript.h"
#include    <string.h>
#include	"translate.h"
#include    <uttMachine.h>

#if (MAKE_MONEY == FYES)
#define SWORD_DEBUG 0
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif
static int aspGetPhone(int eid, webs_t wp, int argc, char_t **argv)
{
    MakeMoneyProfile *prof = NULL;
    char *phone;
    MibProfileType mibType = MIB_PROF_MAKE_MONEY;   
    phone = websGetVar(wp, T("tel"), T(""));
    if(phone[0] == '\0')
    {
	return 0;
    }
    prof = (MakeMoneyProfile *)ProfGetInstPointByIndex(mibType,0);
    if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof))) {
        if(strncmp(prof->phoneNumber,phone,11) != 0) 
        {   
            strcpy(prof->phoneNumber,phone);
	    prof->isBind = 1;
            nvramWriteCommit();
        }   
    }   
    return 1;
}
#if 0
static int aspoutSaleWifiParam(int eid, webs_t wp, int argc, char_t **argv)
{
    MakeMoneyProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_MAKE_MONEY;   
    prof = (MakeMoneyProfile *)ProfGetInstPointByIndex(mibType,0);
    if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof))) {
	websWrite(wp,"var timestarts = \"%s\";",prof->TimeStart);
	websWrite(wp,"var timestops = \"%s\";",prof->TimeStop);
	websWrite(wp,"var charge = \"%s\";",prof->charge);
    }	
    return 0;
}
#endif
static int aspoutPhone(int eid, webs_t wp, int argc, char_t **argv)
{
    MakeMoneyProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_MAKE_MONEY;   
    prof = (MakeMoneyProfile *)ProfGetInstPointByIndex(mibType,0);
    if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof))) {
	websWrite(wp,"var phone = \"%s\";",prof->phoneNumber);
	websWrite(wp,"var isBind = %d;",prof->isBind);
    }	
    return 0;
}

static int aspMakeMoney(int eid, webs_t wp, int argc, char_t **argv)
{
    MakeMoneyProfile *prof = NULL;
    InterfaceProfile *profI = NULL;
    WirelessProfile *profW = NULL;
    MibProfileType mibType = MIB_PROF_MAKE_MONEY;   
    prof = (MakeMoneyProfile *)ProfGetInstPointByIndex(mibType,0);
    if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof))) {
	websWrite(wp,"var MakeMoneyEnables = \"%d\";",prof->MakeMoneyEn);
	websWrite(wp,"var saftyEnables = \"%d\";",prof->SaftyEn);
	websWrite(wp,"var dRate = \"%lu\";",prof->dRate);
	websWrite(wp,"var uRate = \"%lu\";",prof->uRate);
	websWrite(wp,"var timestarts = \"%s\";",prof->TimeStart);
	websWrite(wp,"var timestops = \"%s\";",prof->TimeStop);
//	websWrite(wp,"var account = \"%s\";",prof->account);
	websWrite(wp,"var charge = \"%s\";",prof->charge);
	websWrite(wp,"var FreeTime = \"%d\";",prof->FreeTime);
	websWrite(wp,"var ssid = \"%s\";",prof->ssid);
	websWrite(wp,"var ssid_5g = \"%s\";",prof->ssid_5g);
#if (WLAN_MULTI_SSID == FYES)/*xrg增加多ssid和多ssid的编码方式*/
	websWrite(wp,"var ssid2 = \"%s\";",prof->ssid2);
	websWrite(wp,"var ssid2_5g = \"%s\";",prof->ssid2_5g);
	websWrite(wp,"var encodeType=\"%s\";",prof->encodeType); 
	websWrite(wp,"var encodeType2=\"%s\";",prof->encodeType2); 
	websWrite(wp,"var encodeType_5g=\"%s\";",prof->encodeType_5g); 
	websWrite(wp,"var encodeType2_5g=\"%s\";",prof->encodeType2_5g); 
#endif
    }	
    profI = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 1);
    if ((NULL != profI) && (0 == ProfInstIsFree((Pointer)profI))) {
	websWrite(wp,"var smartQosEn = %d;",profI->smartQosEn);
    }
    profW = (WirelessProfile*)ProfGetInstPointByIndex(MIB_PROF_WIRELESS, 0);
    if(profW != NULL)
    {
	websWrite(wp,"var wireEn = %d;",profW->head.active);
	websWrite(wp, "var weekdaytimestarts=\"%s\";", profW->mBaseCf.weekdayTimeStart);
	websWrite(wp, "var weekdaytimestops=\"%s\";", profW->mBaseCf.weekdayTimeStop);
	websWrite(wp, "var weekendtimestarts=\"%s\";", profW->mBaseCf.weekendTimeStart);
	websWrite(wp, "var weekendtimestops=\"%s\";", profW->mBaseCf.weekendTimeStop);

    }
    profW = (WirelessProfile*)ProfGetInstPointByIndex(MIB_PROF_WIRELESS, 1);
    if(profW != NULL)
    {
	websWrite(wp,"var wireEn_5g = %d;",profW->head.active);
    }

    getTheErrorMsg(eid, wp, argc, argv); 
    getTheRightMsg(eid, wp, argc, argv); 
    return 0;
}
static void formMakeMoney(webs_t wp, char_t *path, char_t *query)
{
   char_t *MakeMoneyEn,*SaftyEn,*dRate,*uRate,*startTime,*stopTime,*account,*charge,*FreeTime,*ssid,*ssid_5g;

   char_t *encodeType;/*xrg ssid编码方式，GB2312或UTF-8*/
   char_t *encodeType_5g;/*ssid编码方式，GB2312或UTF-8*/
#if (WLAN_MULTI_SSID == FYES)
   char_t *ssid2;
   char_t *ssid2_5g;
   char_t *encodeType2;
   char_t *encodeType2_5g;
#endif
   char ssidstr[25],ssidstr_5g[35];
   char ssid_tmp[25],ssid_5g_tmp[35];/*xrg添加为了得到原先的ssid名字*/
   unsigned int Sn;

   MakeMoneyProfile *prof = NULL;
   MibProfileType mibType = MIB_PROF_MAKE_MONEY;   
   WirelessProfile *profW = NULL;
   struProfAlloc *profhead = NULL;
   Sn = getSystemSn();
   MakeMoneyEn = websGetVar(wp, T("makeMoneyEnable"), T(""));
   SaftyEn = websGetVar(wp, T("saftyEnable"), T(""));
   dRate = websGetVar(wp, T("MaxDownRatetxt"), T(""));
   uRate = websGetVar(wp, T("uMaxDownRatetxt"), T(""));
   startTime = websGetVar(wp, T("timestart"), T(""));
   stopTime = websGetVar(wp, T("timestop"), T(""));
//   account = websGetVar(wp, T("Account"), T(""));
   charge = websGetVar(wp, T("FeeScale"), T(""));
   FreeTime = websGetVar(wp, T("FreeTime"), T(""));

   ssid = websGetVar(wp, T("ssid"), T(""));
   ssid_5g = websGetVar(wp, T("ssid_5g"), T(""));

#if (WLAN_MULTI_SSID == FYES)/*xrg增加多ssid和多ssid的编码方式*/
   ssid2 = websGetVar(wp, T("ssid2"), T(""));
   ssid2_5g = websGetVar(wp, T("ssid2_5g"), T(""));
   encodeType = websGetVar(wp, T("encodeType"), T("GB2312"));
   encodeType_5g = websGetVar(wp, T("encodeType_5g"), T("GB2312"));
   encodeType2 = websGetVar(wp, T("encodeType2"), T("GB2312"));
   encodeType2_5g = websGetVar(wp, T("encodeType2_5g"), T("GB2312"));
#endif
   memset(ssidstr,0,sizeof(ssidstr));
   memset(ssidstr_5g,0,sizeof(ssidstr_5g));
   sprintf(ssidstr,"%s元30分钟_%X",charge,Sn);
   sprintf(ssidstr_5g,"%s元30分钟5G_%X",charge,Sn);
   SWORD_PRINTF("%s-%d,MakeMoneyEn = %s,SaftyEn=%s,dRate=%s,uRate=%s,start=%s,stop=%s\n",__func__,__LINE__,MakeMoneyEn,SaftyEn,dRate,uRate,startTime,stopTime); 
   SWORD_PRINTF("ssid = %s..ssid2 = %s..encodeType= %s..encodeType2 = %s\n",ssid,ssid2,encodeType,encodeType2); 
   SWORD_PRINTF("ssid_5g = %s..ssid2_5g = %s..encodeType_5g= %s..encodeType2_5g = %s\n",ssid_5g,ssid2_5g,encodeType_5g,encodeType2_5g); 
    uttSemP(SEM_SALE_WIFI,0);

   prof = (MakeMoneyProfile *)ProfGetInstPointByIndex(mibType,0);

   if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof))) {

       ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
       if(strcmp(SaftyEn,"on") == 0){
	   prof->SaftyEn = FUN_ENABLE;
	}else {
	    prof->SaftyEn = FUN_DISABLE;
	}
	prof->MakeMoneyEn = atoi(MakeMoneyEn);
	prof->dRate = atoi(dRate);
	prof->uRate = atoi(uRate);
	strcpy(prof->TimeStart,startTime);
	strcpy(prof->TimeStop,stopTime);
/*xrg*/
	sprintf(ssid_tmp,"%s元30分钟_%X",prof->charge,Sn);
	sprintf(ssid_5g_tmp,"%s元30分钟5G_%X",prof->charge,Sn);

	if(strstr(ssid,ssid_tmp) != NULL ){
	    strcpy(prof->ssid,ssidstr);
	}else{
	    strcpy(prof->ssid,ssid);
	}
	if(strstr(ssid_5g,ssid_5g_tmp) != NULL ){
	    strcpy(prof->ssid_5g,ssidstr_5g);
	}else{
	    strcpy(prof->ssid_5g,ssid_5g);
	}
/*xrg*/
#if (WLAN_MULTI_SSID == FYES)
	if(strstr(ssid2,ssid_tmp) != NULL ){
	    strcpy(prof->ssid2,ssidstr);
	}else{
	    strcpy(prof->ssid2,ssid2);
	}
	if(strstr(ssid2_5g,ssid_5g_tmp) != NULL ){
	    strcpy(prof->ssid2_5g,ssidstr_5g);
	}else{
	    strcpy(prof->ssid2_5g,ssid2_5g);
	}

	strncpy(prof->encodeType,encodeType,7);/*保存ssid加密方式*/
	strncpy(prof->encodeType2,encodeType2,7);
	strncpy(prof->encodeType_5g,encodeType_5g,7);
	strncpy(prof->encodeType2_5g,encodeType2_5g,7);
#endif
//	strcpy(prof->account,account);
	strcpy(prof->charge,charge);
	prof->FreeTime = atoi(FreeTime);
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);

	nvramWriteCommit();
	setTheRightMsg("saveok");
   } 
   uttSemV(SEM_SALE_WIFI,0);
   websRedirect(wp,"makeMoney.asp");
}
void formDefineMakeMoney(void)
{
    websFormDefine(T("formMakeMoney"), formMakeMoney);
    websAspDefine(T("aspMakeMoney"), aspMakeMoney);
//    websAspDefine(T("aspoutSaleWifiParam"), aspoutSaleWifiParam);
    websAspDefine(T("aspGetPhone"), aspGetPhone);
    websAspDefine(T("aspoutPhone"), aspoutPhone);
}
#endif
