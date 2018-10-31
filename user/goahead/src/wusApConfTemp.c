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
#include <mibUrcp.h>

#if(FEATURE_AP_CONF_TEMP == FYES)

#define DEBUG_TEMP 0
#if DEBUG_TEMP
#define TEMP_DEBUG(fmt, argx...) printf(fmt, ##argx)
#else
#define TEMP_DEBUG(fmt, argx...)
#endif

static void formConfigApConfTemp(webs_t wp, char_t *path, char_t *query);
static int aspOutPutApConfTempList(int eid, webs_t wp, int argc, char_t **argv);
static int getOneApConfTempEntry(int eid, webs_t wp, int argc, char_t **argv);

#if 0
char typeArr[32][UTT_INST_NAME_LEN + 1];
char firmwareArr[32][UTT_INST_NAME_LEN + 1];
#endif

/***********************************************************************
 * º¯ Êý Ãû£º   formConfigApConfTemp
 * ´´½¨ÈÕÆÚ£º	2010-3-17
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       °ØµÂÐã
 * ¸½¼ÓËµÃ÷£º	ÎÞ
 ***********************************************************************/
static void formConfigApConfTemp(webs_t wp, char_t *path, char_t *query)
{
	TEMP_DEBUG("\n---> formConfigApConfTemp() begin <---\n");
	char  *oldName, *name,*wireless, *sgi, *preamble, *wmm, *rate, *bw,*wlFre, *channel, *power, *manual, *mode, *pBeacon;
	char  *vhtbw;
	Uint32 min, max, index, indexTmp;
	ApConfTempProfile *prof = NULL;
	MibProfileType mibType = MIB_PROF_AP_CONF_TEMP;
	const char* ErrMsg = NULL;

	oldName = websGetVar(wp, T("oldEntry"), T("")); 
	name = websGetVar(wp, T("tempName"), T("")); 
	wireless = websGetVar(wp, T("en_wireless"), T("")); 
	sgi = websGetVar(wp, T("SGI"), T("")); 
	preamble = websGetVar(wp, T("preamble"), T("")); 
	wmm = websGetVar(wp, T("wmm"), T("")); 
	rate = websGetVar(wp, T("rate"), T("")); 
	bw = websGetVar(wp, T("BW"), T("")); 
	vhtbw = websGetVar(wp, T("VHTBW"),T(""));
	wlFre = websGetVar(wp, T("wlFre"), T("")); 
	channel = websGetVar(wp, T("channel"), T("")); 
	power = websGetVar(wp, T("power"), T("")); 
	manual = websGetVar(wp, T("manual"), T("")); 
	mode = websGetVar(wp, T("mode"), T("")); 
	pBeacon = websGetVar(wp, T("BeaconPeriod"), T("")); 
	TEMP_DEBUG("\n---> oldName:%s name:%s wireless:%d sgi:%d preamble:%d wmm:%d rate:%ld bw:%ld wlFre: %d channel:%ld power:%ld manual:%ld mode:%ld <---\n", oldName, name, atoi(wireless), atoi(sgi),atoi(preamble), atoi(wmm), strtoul(rate, 0, 10), strtoul(bw, 0, 10), strtoul(wlFre, 0, 10), strtoul(channel, 0 , 10), strtoul(power, 0, 10), strtoul(manual, 0, 10), strtoul(mode, 0, 10));
	ProfInstLowHigh(mibType, &max, &min);
	if (strcmp(oldName, "") == 0)
	{
		TEMP_DEBUG("\n---> add <---\n");
		/* Ìí¼Ó */
		index = ProfNewInst(mibType, name, FALSE);
		prof = (ApConfTempProfile *)ProfGetInstPointByIndex(mibType, index);
		if (prof != NULL)
		{
			strcpy(prof->tempName, name);
			prof->wireless = atoi(wireless);
			prof->sgi = atoi(sgi);
			prof->preamble = atoi(preamble);
			prof->wmm = atoi(wmm);
			/*
			   if (atoi(rate) == 0)
			   {
			   prof->rate = AUTO_RATE;
			   }
			   */
			if (atoi(rate) == 11)
			{
				prof->rate = M11;
			}
			else if (atoi(rate) == 54)
			{
				prof->rate = M54;
			}
			else if (atoi(rate) == 150)
			{
				prof->rate = M150;
			}
			else if (atoi(rate) == 300)
			{
				prof->rate = M300;
			}
			else
			{
				prof->rate = AUTO_RATE;
			}
			prof->bw = strtoul(bw, 0, 10);
			prof->VChanWidth = strtoul(vhtbw, 0, 10);
			if(strtoul(wlFre, 0, 10) == 1u)
			{
			    prof->wlFre = RF_FLAG_5G;
			}else if(strtoul(wlFre, 0, 10) == 0u)
			{
			    prof->wlFre = RF_FLAG_24G;
			}
			prof->channel = strtoul(channel, 0, 10);
			prof->power = strtoul(power, 0, 10);
			if (atoi(manual) == 1)
			{
				prof->manPower = LOW;
			}
			else if (atoi(manual) == 2)
			{
				prof->manPower = MIDDLE;
			}
			else
			{
				prof->manPower = HIGH;
			}
			if(prof->wlFre == RF_FLAG_5G)
			{
			    prof->mode = strtoul(mode, 0, 10) - 1u;
			    if((prof->mode==3) || (prof->mode==4)) /*not 11AC*/
			    {
				prof->VChanWidth = 0;
			    }
			}
			else if(prof->wlFre == RF_FLAG_24G)
			{
			    prof->mode = strtoul(mode, 0, 10);
			}
            prof->BeaconPeriod = strtoul(pBeacon,0,10);
		}
	}
	else
	{
		TEMP_DEBUG("\n---> Modify <---\n");
		/* ÐÞ¸Ä */
		TEMP_DEBUG("oldName = %s\n", oldName);
		indexTmp = ProfGetInstIndexByName(mibType, oldName);
		if (indexTmp == PROFFAIL) {
			TEMP_DEBUG("%d in %s()\n", __LINE__, __func__);
			websRedirect(wp, "apTmp.asp");
			return;
		}

		for (index = min; index < max; index++)
		{
			if (index != indexTmp) {
				prof = (ApConfTempProfile *)ProfGetInstPointByIndex(mibType, index);
				if ((NULL != prof) && (strcmp(prof->tempName, name) == 0))
				{
					TEMP_DEBUG("%d in %s()\n", __LINE__, __func__);
					TEMP_DEBUG("\n---> tempName:%s <---\n", prof->tempName);
					ErrMsg = getTransDataForC(C_LANG_INDEX_AP_TMP_NAME_CONFLICT);
					setTheErrorMsg((char *)ErrMsg);
					//setTheErrorMsg("²Ù×÷Ê§°Ü£¬ÅäÖÃÄ£°åÃû³Æ³åÍ»");
					websRedirect(wp, "apTmp.asp");
					return;
				}
			}
		}

		prof = (ApConfTempProfile *)ProfGetInstPointByName(mibType, oldName);
		if (NULL != prof) {
			ProfSetNameByPoint(prof, name);
			TEMP_DEBUG("%d in %s()\n", __LINE__, __func__);
		}

		TEMP_DEBUG("\n---> index:%d <---\n", index);
		prof->head.active = TRUE;
		strcpy(prof->tempName, name);
		prof->wireless = atoi(wireless);
		prof->sgi = atoi(sgi);
		prof->preamble = atoi(preamble);
		prof->wmm = atoi(wmm);
		if (atoi(rate) == 11)
		{
			prof->rate = M11;
		}
		else if (atoi(rate) == 54)
		{
			prof->rate = M54;
		}
		else if (atoi(rate) == 150)
		{
			prof->rate = M150;
		}
		else if (atoi(rate) == 300)
		{
			prof->rate = M300;
		}
		else
		{
			prof->rate = AUTO_RATE;
		}
		prof->bw = strtoul(bw, 0, 10);
		prof->VChanWidth = strtoul(vhtbw, 0, 10);
		if(strtoul(wlFre, 0, 10) == 1u)
		{
		    prof->wlFre = RF_FLAG_5G;
		}else if(strtoul(wlFre, 0, 10) == 0u)
		{
		    prof->wlFre = RF_FLAG_24G;
		}
		prof->channel = strtoul(channel, 0, 10);
		prof->power = strtoul(power, 0, 10);
		if (atoi(manual) == 1)
		{
			prof->manPower = LOW;
		}
		else if (atoi(manual) == 2)
		{
			prof->manPower = MIDDLE;
		}
		else
		{
			prof->manPower = HIGH;
		}
		if(prof->wlFre == RF_FLAG_24G)
		{
		    prof->mode = strtoul(mode, 0, 10);
		}else if(prof->wlFre == RF_FLAG_5G)
		{
		    prof->mode = strtoul(mode, 0, 10) - 1u;
		}
        prof->BeaconPeriod = strtoul(pBeacon,0,10);
	}
	TEMP_DEBUG("\n---> PROF:  name:%s wireless:%d sgi:%d preamble:%d wmm:%d rate:%d bw:%d ,wlFre:%d channel:%d power:%d manual:%d mode:%d <---\n", prof->tempName,prof->wireless, prof->sgi, prof->preamble, prof->wmm, prof->rate, prof->bw,prof->wlFre, prof->channel, prof->power, prof->manPower, prof->mode);
	nvramWriteCommit();

	websRedirect(wp, "apTmp.asp");
	TEMP_DEBUG("\n---> formConfigApConfTemp() end <---\n");
}

/***********************************************************************
 * º¯ Êý Ãû£º   aspOutPutApConfTempList
 * ´´½¨ÈÕÆÚ£º	2010-3-17
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       °ØµÂÐã
 * ¸½¼ÓËµÃ÷£º	ÎÞ
 ***********************************************************************/
static int aspOutPutApConfTempList(int eid, webs_t wp, int argc, char_t **argv)
{
	TEMP_DEBUG("\n---> aspOutPutApConfTempList() begin <---\n");
	int min, max, index, count = 0, num = 0;
	char tempName[320], wlFre[320], wlMode[320], wlRate[320];
	ApConfTempProfile *prof = NULL;
	MibProfileType mibType = MIB_PROF_AP_CONF_TEMP;

	memset(tempName, '\0', sizeof(tempName));
	memset(wlFre, '\0', sizeof(wlFre));
	memset(wlMode, '\0', sizeof(wlMode));
	memset(wlRate, '\0', sizeof(wlRate));
	ProfInstLowHigh(mibType, &max, &min);

	for (index = min; index < max; index++)
	{
		prof = (ApConfTempProfile *)ProfGetInstPointByIndex(mibType, index);
		if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active==TRUE) && (prof->tempName[0] != '\0'))
		{
			TEMP_DEBUG("\n---> index:%d <---\n", index);
			num = strlen(tempName);
			sprintf(&tempName[num], "\"%s\",", prof->tempName);
			num = strlen(wlFre);
			if(prof->wlFre == RF_FLAG_24G)
			{
				sprintf(&wlFre[num], "\"%s\",", "2.4GHz");
			}else if(prof->wlFre == RF_FLAG_5G)
			{
			sprintf(&wlFre[num], "\"%s\",", "5GHz");
			}

			num = strlen(wlMode);
			if(prof->wlFre == RF_FLAG_24G)
			{
			    if(prof->mode == 1)
			    {
				sprintf(&wlMode[num], "\"%s\",", "11g");
			    }else if(prof->mode == 2)
			    {
				sprintf(&wlMode[num], "\"%s\",", "11n");
			    }else if(prof->mode == 3)
			    {
				sprintf(&wlMode[num], "\"%s\",", "11b/g/n");
			    }
			}else if(prof->wlFre == RF_FLAG_5G)
			{
			    if(prof->mode == 3)
			    {
				sprintf(&wlMode[num], "\"%s\",", "11a");
			    }else if(prof->mode == 4)
			    {
				sprintf(&wlMode[num], "\"%s\",", "11a/n");
			    }else if(prof->mode == 5)
			    {
				sprintf(&wlMode[num], "\"%s\",", "11vht AC/AN/A"); 
			    }else if(prof->mode == 6)
			    {
				sprintf(&wlMode[num], "\"%s\",", "11vht AC/AN");
			    }
			}
			num = strlen(wlRate);
			if (prof->rate == M11)
			{
				sprintf(&wlRate[num], "\"%s\",", "11M");
			}
			else if(prof->rate == M54)
			{
				sprintf(&wlRate[num], "\"%s\",", "54M");
			}
			else if(prof->rate == M150)
			{
				sprintf(&wlRate[num], "\"%s\",", "150M");
			}
			else if(prof->rate == M300)
			{
				sprintf(&wlRate[num], "\"%s\",", "300M");
			}
			else if(prof->rate == AUTO_RATE)
			{
				sprintf(&wlRate[num], "\"%s\",", "×Ô¶¯");
			}
			count++;
		}
	}
	/*²»Îª¿Õ*/
	if(count > 0u)
	{
		num = strlen(tempName)-1u;
		tempName[num] = '\0';
		num = strlen(wlFre)-1u;
		wlFre[num] = '\0';
		num = strlen(wlMode)-1u;
		wlMode[num] = '\0';
		num = strlen(wlRate)-1u;
		wlRate[num] = '\0';
	}

	websWrite(wp, "var tmpname = new Array(%s);\n", tempName);/*  */
	websWrite(wp, "var wlFres = new Array(%s);\n", wlFre);/*  */
	websWrite(wp, "var wlModes = new Array(%s);\n", wlMode);/*  */
	websWrite(wp, "var wlRates = new Array(%s);\n", wlRate);/*  */

	websWrite(wp,"var totalrecs = %d;", count);
	getTheErrorMsg(eid, wp, argc, argv);
	TEMP_DEBUG("\n---> aspOutPutApConfTempList() end <---\n");
	return 0;
}

/***********************************************************************
 * º¯ Êý Ãû£º   aspOutPutApConfTempNameList
 * ´´½¨ÈÕÆÚ£º	2010-3-17
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       °ØµÂÐã
 * ¸½¼ÓËµÃ÷£º	ÎÞ
 ***********************************************************************/
static int aspOutPutApConfTempNameList(int eid, webs_t wp, int argc, char_t **argv)
{
	TEMP_DEBUG("\n---> aspOutPutApConfTempNameList() begin <---\n");
	int min, max, index, count = 0, num = 0;
	char tempName[320];
	ApConfTempProfile *prof = NULL;
	MibProfileType mibType = MIB_PROF_AP_CONF_TEMP;

	memset(tempName, '\0', sizeof(tempName));
	ProfInstLowHigh(mibType, &max, &min);

	for (index = min; index < max; index++)
	{
		prof = (ApConfTempProfile *)ProfGetInstPointByIndex(mibType, index);
		if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active==TRUE))
		{
			TEMP_DEBUG("\n---> index:%d <---\n", index);
			num = strlen(tempName);
			sprintf(&tempName[num], "\"%s\",", prof->tempName);
			count++;
		}
	}
	/*²»Îª¿Õ*/
	if(count > 0u)
	{
		num = strlen(tempName)-1u;
		tempName[num] = '\0';
	}

	websWrite(wp, "var tmpname = new Array(%s);\n", tempName);/*  */
	TEMP_DEBUG("\n---> aspOutPutApConfTempNameList() end <---\n");
	return 0;
}

/***********************************************************************
 * º¯ Êý Ãû£º   getOneApConfTempEntry
 * ¹¦ÄÜÃèÊö£º	¶ÁÒ»¸öÌõÄ¿ËÍÒ³Ãæ
 * ´´½¨ÈÕÆÚ£º	2010-3-17
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       °ØµÂÐã
 * ¸½¼ÓËµÃ÷£º	ÎÞ
 ***********************************************************************/
static int getOneApConfTempEntry(int eid, webs_t wp, int argc, char_t **argv)
{
	TEMP_DEBUG("\n---> getOneApConfTempEntry() begin <---\n");
	char *edit_name;
	int min, max, index;
	MibProfileType mibType = MIB_PROF_AP_CONF_TEMP;
	ApConfTempProfile *prof = NULL;

	ejArgs(argc,argv,T("%s"), &edit_name); 
	TEMP_DEBUG("edit_name:%s\n",edit_name);
	ProfInstLowHigh(mibType, &max, &min);
	for (index = min; index < max; index++)
	{
		prof = (ApConfTempProfile *)ProfGetInstPointByIndex(mibType, index);
		if (strcmp(edit_name, prof->tempName) == 0)
		{
			TEMP_DEBUG("\n---> PROF:  name:%s wireless:%d sgi:%d preamble:%d wmm:%d rate:%d bw:%d ,wlFre:%d channel:%d power:%d manual:%d mode:%d <---\n", prof->tempName,prof->wireless, prof->sgi, prof->preamble, prof->wmm, prof->rate, prof->bw,prof->wlFre, prof->channel, prof->power, prof->manPower, prof->mode);
			websWrite(wp,"var tempName = \"%s\";\n", prof->tempName);
			websWrite(wp,"var wireless = %s;\n", (prof->wireless == 1 ? "true" : "false"));
			websWrite(wp,"var sgi = %s;\n", (prof->sgi == 1 ? "true" : "false"));
			websWrite(wp,"var preamble = %s;\n", (prof->preamble == 1 ? "true" : "false"));
			websWrite(wp,"var wmm = %s;\n", (prof->wmm == 1 ? "true" : "false"));
			if (prof->rate == AUTO_RATE)
			{
				TEMP_DEBUG("\n%d\n",0);
				websWrite(wp,"var rate = %d;\n", 0);
			}
			else if (prof->rate == M11)
			{
				TEMP_DEBUG("\n%d\n",11);
				websWrite(wp,"var rate = %d;\n", 11);
			}
			else if (prof->rate == M54)
			{
				TEMP_DEBUG("\n%d\n",54);
				websWrite(wp,"var rate = %d;\n", 54);
			}
			else if (prof->rate == M150)
			{
				TEMP_DEBUG("\n%d\n",150);
				websWrite(wp,"var rate = %d;\n", 150);
			}
			else if(prof->rate == M300)
			{
				TEMP_DEBUG("\n%d\n",300);
				websWrite(wp,"var rate = %d;\n", 300);
			}
			websWrite(wp,"var bw = %d;\n", prof->bw);
			websWrite(wp,"var VHTBW = %d;\n", prof->VChanWidth);
			if(prof->wlFre == RF_FLAG_5G)
			{
			    websWrite(wp,"var wlFre = 1;\n");
			}else if(prof->wlFre == RF_FLAG_24G)
			{
			    websWrite(wp,"var wlFre = 0;\n");
			}
			websWrite(wp,"var channel = %d;\n", prof->channel);
			websWrite(wp,"var power = %d;\n", prof->power);
			if (prof->manPower == LOW)
			{
				TEMP_DEBUG("\n%d\n",1);
				websWrite(wp,"var manPower = %d;\n", 1);
			}
			else if (prof->manPower == MIDDLE)
			{
				TEMP_DEBUG("\n%d\n",2);
				websWrite(wp,"var manPower = %d;\n", 2);
			}
			else
			{
				TEMP_DEBUG("\n%d\n",3);
				websWrite(wp,"var manPower = %d;\n", 3);
			}
			if(prof->wlFre == RF_FLAG_24G)
			{
			    websWrite(wp,"var mode = %d;\n", prof->mode);
			}else if(prof->wlFre == RF_FLAG_5G)
			{
			    websWrite(wp,"var mode = %d;\n", prof->mode + 1);
			}
			if (prof->BeaconPeriod == 0)
			{
			    websWrite(wp,"var beaconPeriod = %d;\n", 100);
			}
			else
			{
			    websWrite(wp,"var beaconPeriod = %d;\n", prof->BeaconPeriod);
			}
			break;
		}
	}
	getTheErrorMsg(eid, wp, argc, argv);
	TEMP_DEBUG("\n---> getOneApConfTempEntry() end <---\n");
	return 0;
}

/***********************************************************************
 * º¯ Êý Ãû£º   formApConfTempDel
 * ¹¦ÄÜÃèÊö£º	´ÓflashºÖÐÉ¾³ýÄ³Ò»ÌõÄ¿
 * ´´½¨ÈÕÆÚ£º	2010-3-17
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       °ØµÂÐã
 * ¸½¼ÓËµÃ÷£º	ÎÞ
 ***********************************************************************/
static void formApConfTempDel(webs_t wp, char_t *path, char_t *query)
{
	TEMP_DEBUG("\n---> formApConfTempDel() begin <---\n");
	char *tmpName = NULL, *tmp, *ptr;
	int min, max, index = 0;
	ApConfTempProfile *prof = NULL;
	MibProfileType mibType = MIB_PROF_AP_CONF_TEMP;

	tmpName = websGetVar(wp,T("delstr"),T(""));	/* »ñÈ¡É¾³ýµÄÐÅÏ¢ */
	TEMP_DEBUG("\ntmpName : %s\n", tmpName);
	tmp=strtok_r(tmpName,UTT_SPLIT_SIGN_STR,&ptr);
	ProfInstLowHigh(mibType, &max, &min);
	while(tmp != NULL)
	{
		for (index = min; index < max; index++)
		{
			prof = (ApConfTempProfile *)ProfGetInstPointByIndex(mibType, index);
			if (strcmp(tmp, prof->tempName) == 0)
			{
				ProfDelInstByIndex(mibType, index); /* É¾³ý */
			}
		}
		tmp=strtok_r((char *)NULL,UTT_SPLIT_SIGN_STR,&ptr);
	}
	nvramWriteCommit();	/* ±£´æ */
	websRedirect(wp, "apTmp.asp");
	TEMP_DEBUG("\n---> formApConfTempDel() end <---\n");
}

/***********************************************************************
 * º¯ Êý Ãû£º   formApConfTempDelAll
 * ¹¦ÄÜÃèÊö£º	É¾³ýflashÖÐµÄËùÓÐÌõÄ¿
 * ´´½¨ÈÕÆÚ£º	2010-3-17
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       °ØµÂÐã
 * ¸½¼ÓËµÃ÷£º	ÎÞ
 ***********************************************************************/
static void formApConfTempDelAll(webs_t wp, char_t *path, char_t *query)
{
	TEMP_DEBUG("\n---> formApConfTempDelAll() begin <---\n");
	int min, max, index = 0;
	ApConfTempProfile *prof = NULL;
	MibProfileType mibType = MIB_PROF_AP_CONF_TEMP;

	ProfInstLowHigh(mibType, &max, &min);
	for (index = min; index < max; index++)
	{
		ProfDelInstByIndex(mibType, index);
	}
	nvramWriteCommit();
	websRedirect(wp, "apTmp.asp");
	TEMP_DEBUG("\n---> formApConfTempDelAll() end <---\n");
}

/***********************************************************************
 * º¯ Êý Ãû£º   formSetTempAsDefault
 * ¹¦ÄÜÃèÊö£º	½«Ä£°æÉèÖÃÎªÄ¬ÈÏ
 * ´´½¨ÈÕÆÚ£º	2010-3-17
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       °ØµÂÐã
 * ¸½¼ÓËµÃ÷£º	ÎÞ
 ***********************************************************************/
#if 0
void initDefault(void)
{
	int n = 0, min, max, index = 0;
	ApConfTempProfile *prof = NULL;
	MibProfileType mibType = MIB_PROF_AP_CONF_TEMP;

	memset(typeArr,'\0',sizeof(typeArr));
	memset(firmwareArr,'\0',sizeof(firmwareArr));
	ProfInstLowHigh(mibType, &max, &min);
	for (index = min; index < max; index++)
	{
		prof = (ApConfTempProfile *)ProfGetInstPointByIndex(mibType, index);
		if ((prof != NULL) && (prof->def == PROF_ENABLE))
		{
			strcpy(typeArr[n],prof->suitType);
			strcpy(firmwareArr[n],prof->firmware);
			n++; 
		}
	}
}
#endif
#if 0
/***********************************************************************
 * º¯ Êý Ãû£º   formSetTempAsDefault
 * ¹¦ÄÜÃèÊö£º	½«Ä£°æÉèÖÃÎªÄ¬ÈÏ
 * ´´½¨ÈÕÆÚ£º	2010-3-17
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       °ØµÂÐã
 * ¸½¼ÓËµÃ÷£º	ÎÞ
 ***********************************************************************/
static void formSetTempAsDefault(webs_t wp, char_t *path, char_t *query)
{
	TEMP_DEBUG("\n< %s %d >\n", __func__, __LINE__);
	char *tName = NULL;
	int min, max, index1 = 0, index2 = 0;
	ApConfTempProfile *prof1 = NULL;
	ApConfTempProfile *prof2 = NULL;
	MibProfileType mibType = MIB_PROF_AP_CONF_TEMP;

	tName = websGetVar(wp, T("defName"), T("")); 
	TEMP_DEBUG("\ndefName:%s\n", tName);
	ProfInstLowHigh(mibType, &max, &min);
	for (index1 = min; index1 < max; index1++)
	{
		prof1 = (ApConfTempProfile *)ProfGetInstPointByIndex(mibType, index1);
		if (strcmp(tName, prof1->tempName) == 0)
		{
			prof1->def = PROF_ENABLE;
			break;
		}
	}
	for (index2 = min; index2 < max; index2++)
	{
		prof2 = (ApConfTempProfile *)ProfGetInstPointByIndex(mibType, index2);
		if ((strcmp(prof1->suitType, prof2->suitType) == 0) && (strcmp(prof1->firmware, prof2->firmware) == 0) && (index2 != index1))
		{
			prof2->def = PROF_DISABLE;
		}
	}
	nvramWriteCommit();
	websRedirect(wp, "apTmp.asp");
	TEMP_DEBUG("\n---> formSetTempAsDefault() end <---\n");
}

/***********************************************************************
 * º¯ Êý Ãû£º   formClearTempAsDefault
 * ¹¦ÄÜÃèÊö£º	Çå³ýÄ¬ÈÏÄ£°æ
 * ´´½¨ÈÕÆÚ£º	2010-3-17
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       °ØµÂÐã
 * ¸½¼ÓËµÃ÷£º	ÎÞ
 ***********************************************************************/
static void formClearTempAsDefault(webs_t wp, char_t *path, char_t *query)
{
	TEMP_DEBUG("\n---> formClearTempAsDefault() begin <---\n");
	char *tpName = NULL;
	int min, max, index = 0;
	ApConfTempProfile *prof = NULL;
	MibProfileType mibType = MIB_PROF_AP_CONF_TEMP;

	tpName = websGetVar(wp, T("defName"), T("")); 
	TEMP_DEBUG("\ntpName : %s\n", tpName);
	ProfInstLowHigh(mibType, &max, &min);
	for (index = min; index < max; index++)
	{
		prof = (ApConfTempProfile *)ProfGetInstPointByIndex(mibType, index);
		if (strcmp(tpName, prof->tempName) == 0)
		{
			prof->def = PROF_DISABLE;
			break;
		}
	}
	nvramWriteCommit();
	websRedirect(wp, "apTmp.asp");
	TEMP_DEBUG("\n---> formClearTempAsDefault() end <---\n");
}
#endif
/***********************************************************************
 * º¯ Êý Ãû£º   formDefineApConfTemp
 * ´´½¨ÈÕÆÚ£º	2010-3-17
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       °ØµÂÐã
 * ¸½¼ÓËµÃ÷£º	ÎÞ
 ***********************************************************************/
void formDefineApConfTemp(void)
{
	websAspDefine(T("aspOutPutApConfTempList"), aspOutPutApConfTempList);
	websAspDefine(T("aspOutPutApConfTempNameList"), aspOutPutApConfTempNameList);
	websAspDefine(T("getOneApConfTempEntry"), getOneApConfTempEntry);
	websFormDefine(T("formConfigApConfTemp"), formConfigApConfTemp);
	websFormDefine(T("formApConfTempDel"), formApConfTempDel);
	websFormDefine(T("formApConfTempDelAll"), formApConfTempDelAll);
//	websFormDefine(T("formSetTempAsDefault"), formSetTempAsDefault);
//	websFormDefine(T("formClearTempAsDefault"), formClearTempAsDefault);
}
#endif
