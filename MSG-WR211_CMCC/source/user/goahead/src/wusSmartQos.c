#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include        "translate.h"

#if ((SMART_QOS_FUN == FYES) || (GAME_BOOST == FYES))
static int aspSmartQos(int eid, webs_t wp, int argc, char_t **argv)
{
    int i = 0, max = 0,min=0;
#if (SMART_QOS_FUN == FYES)
    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *prof = NULL;

    char_t *txBand=T("txBands[%d] = \"%d\";");
    char_t *rxBand=T("rxBands[%d] = \"%d\";");

    websWrite(wp,"var txBands=new Array();");
    websWrite(wp,"var rxBands=new Array();");

    max = getWanSlotCount();
    for(i = 1; i <= max; i++) {
        prof = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, i);
        if(prof != NULL && ProfInstIsFree(prof) == 0) {
            websWrite(wp, txBand, i, prof->txRate);
            websWrite(wp, rxBand, i, prof->rxRate);
        }
    }

    prof = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, 1);
    websWrite(wp, "var smartQosEn=%d;", prof->smartQosEn);
#endif
#if (GAME_BOOST == FYES)
	MibProfileType mibTypePdb = MIB_PROF_PDB_GLOBAL;
	PdbProfile *pdbprof= NULL;

	ProfInstLowHigh(mibTypePdb, &max, &min);

	for (i = min; i < max; i++) {
		pdbprof = (PdbProfile*)ProfGetInstPointByIndex(mibTypePdb, i); 
		if (ProfInstIsFree(pdbprof) == 0) {
			if (pdbprof->type == SpeedupPdb) {
				if(pdbprof->head.active == FUN_ENABLE)
					websWrite(wp, "%s= \"on\";", pdbprof->head.name);
				else
				websWrite(wp, "%s= \"off\";", pdbprof->head.name);
			}   
		}   
	}   
	websWrite(wp,"var GameBoost = 1;");
#else
	websWrite(wp,"var GameBoost = 0;");
#endif
	/*如果是英文版本就隐藏掉页面上关于游戏加速的相关，传值到页面上*/
#if (CONFIG_USER_GOAHEAD_LANG_EN == FYES)
	websWrite(wp,"var GameBoostShow = 0;");
#else
	websWrite(wp,"var GameBoostShow = 1;");
#endif
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1 && UTT_TRANS_ENGLISH == 1)
	websWrite(wp, "var englishSwitch = 1;");
#else
	websWrite(wp, "var englishSwitch = 0;");
#endif
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1 && UTT_TRANS_ENGLISH == 1)
    websWrite(wp, "var englishSwitch = 1;");
#else
    websWrite(wp, "var englishSwitch = 0;");
#endif

    getTheErrorMsg(eid, wp, argc, argv);
  #if (UTT_SMART_UI == FYES) 
getTheRightMsg(eid,wp,argc,argv);
  #endif
    return 0;
}

extern void editSmartQosConfig(webs_t wp, char_t *path, char_t *query)
{
#if (SMART_QOS_FUN == FYES)
    int max = 0, i = 0;
    char *smartQosEnS = NULL, *rxBand[MAX_WAN_IF_COUNT] = {NULL}, *txBand[MAX_WAN_IF_COUNT] = {NULL};
    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *prof = NULL;
    FunEnableEnum smartQosEn = FUN_DISABLE;
    char buftmp[20] = {0};
#endif
    struProfAlloc *profhead = NULL;
#if (GAME_BOOST == FYES)
    char *gamePolicy= NULL;
	gamePolicy = websGetVar(wp, "gamePolicy", T(""));
#endif

#if (GAME_BOOST == FYES)
	MibProfileType mibTypePdb = MIB_PROF_PDB_GLOBAL;
	PdbProfile *pdbprof= NULL;
	int index=0,j=0,min=0,flag=FALSE;
#if (UTT_SMART_UI == FYES)
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1 && UTT_TRANS_ENGLISH == 1)
	int checked[7]={0};
#else
	int checked[19]={0};
#endif
#else
	int checked[9]={0};
#endif
	char_t *tmp=NULL;
	i = 0;
	if((gamePolicy!=NULL)&&(strlen(gamePolicy)!=0)){
		tmp = strtok(gamePolicy,UTT_SPLIT_SIGN_RECORDS);
		while(tmp!=NULL){
			index = strtol(tmp,NULL,10);
			checked[i++]=index;
			tmp=strtok(NULL, UTT_SPLIT_SIGN_RECORDS);
		}
	}
	ProfInstLowHigh(mibTypePdb, &max, &min);
	for (i = min; i < max; i++) {
		pdbprof = (PdbProfile*)ProfGetInstPointByIndex(mibTypePdb, i);
		if(pdbprof==NULL)
			break;
		if (ProfInstIsFree(pdbprof) == 0&&pdbprof->type == SpeedupPdb) {
			ProfBackupByIndex(mibTypePdb, PROF_CHANGE_EDIT, i, &profhead);
			flag=0;

#if (UTT_SMART_UI == FYES)
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1 && UTT_TRANS_ENGLISH == 1)
			for(j=0;j<7;j++){
#else
			for(j=0;j<19;j++){
#endif
#else
			for(j=0;j<9;j++){
#endif
				if(checked[j]==0)
				break;
				if(i == checked[j]){
					flag=TRUE;
					pdbprof->head.active = FUN_ENABLE;
					break;
				}
			}
			if((flag == FALSE)&&(pdbprof->head.active!=FUN_DISABLE)){
				pdbprof->head.active = FUN_DISABLE;
			}
		}
	}
	ProfUpdate(profhead);
#endif
#if (SMART_QOS_FUN == FYES)
    max = getWanSlotCount();

    smartQosEnS = websGetVar(wp, "smartQosEn", T(""));
    for(i = 1; i <= max; i++) {
        sprintf(buftmp, "txBand%d", i);
        txBand[i-1] = websGetVar(wp, buftmp, T("0"));
        sprintf(buftmp, "rxBand%d", i);
        rxBand[i-1] = websGetVar(wp, buftmp, T("0"));
    }

    if(strcmp(smartQosEnS, "on") == 0) {
        smartQosEn = FUN_ENABLE;
    }

    for(i = 1; i <= max; i++) {
        prof = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, i);
        if(prof != NULL && ProfInstIsFree(prof) == 0) {
            ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, i, &profhead);
            prof->smartQosEn = smartQosEn;
            if(txBand[i-1] != NULL && rxBand[i-1] != NULL) {
                prof->txRate = strtol(txBand[i-1], NULL, 10);
                prof->rxRate = strtol(rxBand[i-1], NULL, 10);
            }
        }
    }

    ProfUpdate(profhead);
#endif

    ProfFreeAllocList(profhead);
}

static void formConfigSmartQos(webs_t wp, char_t *path, char_t *query)
{
    editSmartQosConfig(wp, path, query);
    nvramWriteCommit();
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, "SmartQos_Sui.asp");
#else
    websRedirect(wp, "SmartQos.asp");
#endif
    return;
}

/*
 * getRateArr		-	获取千兆设备的速率数组
 *
 * 若设备是千兆设备则将重定义速率数组，否则什么也不做，
 * 直接使用common.js中的速率数组
 */
static int getRateArr(int eid, webs_t wp, int argc, char **argv)
{
#if (MAX_SPDMODE_RDD == 1000)
	websWrite(wp, "RateArr = new Array(\"64K\", \"128K\", \"256K\", \"512K\", \"1M\", \"1.5M\", \"2M\", \"3M\", \"4M\", \"5M\", \"6M\", \"7M\", \"8M\", \"9M\", \"10M\", \"11M\", \"12M\", \"13M\", \"14M\", \"15M\", \"16M\", \"17M\", \"18M\", \"19M\", \"20M\", \"25M\", \"30M\", \"35M\", \"40M\", \"45M\", \"50M\", \"90M\", \"100M\", \"1000M\");\n");
	websWrite(wp, "RateArrValue = new Array(\"64\", \"128\", \"256\", \"512\", \"1000\", \"1500\", \"2000\", \"3000\", \"4000\", \"5000\", \"6000\", \"7000\", \"8000\", \"9000\", \"10000\", \"11000\", \"12000\", \"13000\", \"14000\", \"15000\", \"16000\", \"17000\", \"18000\", \"19000\", \"20000\", \"25000\", \"30000\", \"35000\", \"40000\", \"45000\", \"50000\", \"90000\", \"100000\", \"1000000\");\n");
#endif
	return 0;
}

void formDefineSmartQos(void)
{
    websAspDefine(T("aspSmartQos"), aspSmartQos);
    websFormDefine(T("formConfigSmartQos"), formConfigSmartQos);
    websAspDefine(T("aspOutputRateArr"), getRateArr);
}

#endif
