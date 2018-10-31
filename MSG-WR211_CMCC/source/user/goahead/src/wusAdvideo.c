#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"

#if (GROUP_BASE == FYES)
#define UTT_MAX_PDB_POLICY_CNT ((UTT_MAX_PDB_POLICY) * 8)

#define LIST_PDB_STR_LEN  40
#define LIST_PDB_ELLIPSIS "..."

static int UTTSetPdbPolicy(Uint8 *pdbPolicy, char_t *str);
#if (ADFILTER == FYES)
extern void adfilter_Config (int sw, int updateType);
#endif

/* ≈–∂œpdb bitŒª «∑Ò±ª…Ë÷√ */
static Boolean pdb_policy_match(Uint8 *policy, int index)
{
    if (policy[index >> 3] & (1 << (index & 7))) {
	return TRUE;
    }
        return FALSE;
}
/*
 * ∏˘æ›pdbPolicy bitŒªªÒ»°¡–±Ì÷–œ‘ æµƒΩ˚÷π”¶”√¡–(QQ;MSN;...)
 */
static char *getDenyPdbString(Uint8 *pdbPolicy) {
    int i, min, max;
    MibProfileType mibType = MIB_PROF_PDB_GLOBAL;
    PdbProfile *prof= NULL;
    static char buf[LIST_PDB_STR_LEN + 1] = {0};
    int len = sizeof(buf) - strlen(LIST_PDB_ELLIPSIS);
    int len1 = 0;
    int needSplit = 0;

    memset(buf, 0, sizeof(buf));
    ProfInstLowHigh(mibType, &max, &min);

    for (i = min; i < max; i++) { /* ±È¿˙pdb µ¿˝£¨’“≥ˆ…˙–ßµƒpdb√˚ */
	prof = (PdbProfile*)ProfGetInstPointByIndex(mibType, i);
	if (ProfInstIsFree(prof) == 0) {
	    if ((prof->index < UTT_MAX_PDB_POLICY_CNT) && pdb_policy_match(pdbPolicy, prof->index)) {
		len1 = strlen(prof->head.name); /* ªÒ»° µ¿˝√˚≥§∂» */
		if ((len1 + 1) < len) { /* buf≤ªπª»›ƒ… µ¿˝√˚≥§∂», ‘ÚÃ¯≥ˆ */
		    strcat(buf, prof->head.name);
		    strcat(buf, UTT_SPLIT_SIGN_RECORDS);
		    len -= len1;
		    len -= 1;
		} else {
		    needSplit = 1;
		    break;
		}
	    }
	}
    }

    if (needSplit == 1) {
	strcat(buf, LIST_PDB_ELLIPSIS);
    }

    return buf;
}

static void GroupConfigDel(char *GroupName)
{
    char_t *tmp = NULL;
    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    struProfAlloc *profList  = NULL;
    a_assert(wp);
  
    GroupName = "utt_action"; 

    tmp=strtok(GroupName, UTT_SPLIT_SIGN_STR);

    uttSemP(SEM_UTTNF_NO,0);
    while(tmp != NULL) {
        ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profList);
        ProfDelInstByName(mibType, tmp); 
        tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
    }
    uttSemV(SEM_UTTNF_NO,0);
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
}
/*≤ªπ‹ «∑Òø™∆Ù––Œ™π‹¿Ì£¨◊˜”√£∫÷ª «…˙≥…“ªÃıπ„∏Êπ˝¬Àµƒ≈‰÷√ */
static void MConfigAdvideo(char *GroupName,char *group_day,char *sourceIP,char *PdbPolicy,char *group_timestart,char *group_timestop)
{
    GroupBaseType webGroupType = GBT_ACT_CONTROL; 
    char_t *FromIp = NULL,*EndIp = NULL,*GroupNameType ="";
    char_t *policyStr = NULL;
    char PdbPolicy1[UTT_MAX_PDB_POLICY_CNT * 4] = {0};

    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    GroupBaseProfile *profGroupBase = NULL;
    struct in_addr addr_fromip, addr_endip;
    int instIndex;
    int semFlag = 0;
    int max = 0, min = 0;
    Uint32 maxProf = 0u;
    SystemProfile *profSys = NULL;

    a_assert(wp);

	maxProf = MAX_ACT_CONTROL_PROFILES;
    ProfInstLowHigh(mibType, &max, &min);
    
    uttSemP(SEM_UTTNF_NO,0);
    semFlag = 1;
    profGroupBase  = (GroupBaseProfile*)ProfGetInstPointByIndex(MIB_PROF_GROUP_BASE, 0);
    instIndex = ProfGetInstIndexByName(mibType, GroupName);
	if(instIndex != PROFFAIL) 
	{
	    goto out;
	}
	/*ÁªüËÆ°È°µÈù¢ÂØπÂ∫îÁªÑÁ±ªÂûãÊòØÂê¶ÂÆû‰æãÂ∑≤ÁªèÊ∑ªÂä†Âà∞ÊúÄÂ§ö*/
	if(ProfCountInstByValue(mibType, OFFANDSIZE(GroupBaseProfile, groupType), 0u, (Pointer)&webGroupType) >= maxProf)
	{
	    goto out;
	}
	instIndex = ProfNewInst(mibType, GroupName, FALSE);
    profAction = PROF_CHANGE_ADD;

    ProfBackupByIndex(mibType, profAction, instIndex, &profhead);
    profGroupBase = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, instIndex);
    
	profGroupBase->groupType = webGroupType;/*ËÆ∞ÂΩïÁ±ªÂûãÊñπ‰æø‰∏çÂêåÁöÑÈ°µÈù¢‰ΩøÁî®*/

    /*‰ª•‰∏ãÂà§Êñ≠Èò≤Ê≠¢ÂÅ∂ÁÑ∂ÊÉÖÂÜµÂá∫Áé∞ÁöÑ È°µÈù¢‰øÆÊîπÊìç‰ΩúÊó∂ È°µÈù¢‰∏éÊìç‰ΩúÁöÑÁªÑÁ±ªÂûã‰∏çÂåπÈÖç*/
    ProfSetNameByPoint(profGroupBase, GroupName);
    profGroupBase->day= days2Byte(group_day); 
    strcpy(profGroupBase->timeStart, group_timestart); 
    strcpy(profGroupBase->timeStop, group_timestop);
	strncpy(PdbPolicy1, PdbPolicy, UTT_MAX_PDB_POLICY_CNT * 4);
	UTTSetPdbPolicy((Uint8 *)profGroupBase->pdbPolicy1, PdbPolicy1);
     
	profGroupBase->addrTypeEn = ADDR_IPGRP; 
	strcpy(profGroupBase->ipGrpName, GroupNameType);
    
    semFlag = 0;
    uttSemV(SEM_UTTNF_NO,0);
	ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
    if(semFlag == 1)
    {
        uttSemV(SEM_UTTNF_NO,0);
    }
    return;
}
static int aspAdvideo(int eid, webs_t wp, int argc, char_t **argv)
{
    int flag = 0;
    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    GroupBaseProfile *prof = NULL;
    int i = 0,max = 0, min = 0;
    SystemProfile *profSys = NULL;

	profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
	if(profSys->UttNfRuleEn != FUN_ENABLE)
        goto out;
    ProfInstLowHigh(mibType, &max, &min);

    for (i = min; i < max; i++) {
        prof = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, i);
	if (ProfInstIsFree(prof) == 0) {
        if(!strcmp(prof->head.name,"utt_action"))
        {
            if(prof->head.active != 1 || strcmp(prof->timeStart,"All") || strcmp(prof->timeStop,"0") || prof->day!=127 || prof->addrTypeEn != ADDR_IPGRP || strcmp(getDenyPdbString((Uint8 *)(prof->pdbPolicy1)),"Advertise"))
            { 
                GroupConfigDel("utt_action");
                MConfigAdvideo("utt_action","111111","groupSel","143","All","0");
            }
            websWrite(wp, "advideo = 1;");
            break;
        }
	    }
	}
out:
    getTheErrorMsg(eid, wp, argc, argv);
#if (UTT_SMART_UI == FYES) 
    getTheRightMsg(eid,wp,argc,argv);

#if (ADFILTER == FYES)
    AdfilterProfile *profAdfilter = NULL;
    profAdfilter = (AdfilterProfile *)ProfGetInstPointByIndex(MIB_PROF_ADFILTER, 0);
    websWrite(wp, "var updateType = %d;",profAdfilter->updateType);
#endif
#endif
    return 0;
}

/* ËÆæÁΩÆpdb bit‰Ωç */
static void pdb_policy_set(Uint8 *policy, int index)
{
    policy[index >> 3] |= (1 << (index & 7));
}

static int UTTSetPdbPolicy(Uint8 *pdbPolicy, char_t *str)
{
    char_t *tmp = NULL;
    int index = 0;
    memset(pdbPolicy, 0, UTT_MAX_PDB_POLICY);
    tmp = strtok(str, UTT_SPLIT_SIGN_RECORDS);
    while(tmp != NULL)
    {
	index = strtol(tmp, NULL, 10);
	if ((index != 0) && (index < UTT_MAX_PDB_POLICY_CNT)) {
	    pdb_policy_set(pdbPolicy, index);
	}

	tmp=strtok(NULL, UTT_SPLIT_SIGN_RECORDS);
    }
    return 0;
}
#if (UTT_SMART_UI == FYES)
void configSysProf(int sw)
{
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    SystemProfile *profSys = NULL;

    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);

    uttSemP(SEM_UTTNF_NO,0);
    profAction = PROF_CHANGE_EDIT;
    ProfBackupByIndex(MIB_PROF_SYSTEM, profAction, 0, &profhead);
    if (sw == 1) {
	profSys->UttNfRuleEn = FUN_ENABLE;
    } else {
	profSys->UttNfRuleEn = FUN_DISABLE;
    }
    uttSemV(SEM_UTTNF_NO,0);
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
}
#endif
static void formConfigAdvideo(webs_t wp, char_t *path, char_t *query)
{
    GroupBaseType webGroupType = GBT_ACT_CONTROL; 
    char_t *GroupName = NULL,*FromIp = NULL,*EndIp = NULL,*GroupNameType = NULL,*sourceIP =NULL,*PdbPolicy = NULL;
    char_t *policyStr = NULL, *uRateStr = NULL, *dRateStr = NULL;
    char_t *group_day,*group_timestart,*group_timestop;
    char_t *action;
    char PdbPolicy1[UTT_MAX_PDB_POLICY_CNT * 4] = {0};

    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    GroupBaseProfile *profGroupBase = NULL;
    struct in_addr addr_fromip, addr_endip;
    int instIndex;
    int semFlag = 0;
    int max = 0, min = 0;
    Uint32 maxProf = 0u;

#if (UTT_SMART_UI != FYES)
    SystemProfile *profSys = NULL;

    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
	if(profSys->UttNfRuleEn != FUN_ENABLE)
    {
	    setTheErrorMsg("«Î∆Ù”√…œÕ¯––Œ™π‹¿Ì£¨∑Ò‘Ú¥À≤Ÿ◊˜≤ª…˙–ß!");
        goto out;
    }
#else
    configSysProf(1);
#endif
#if (ADFILTER == FYES)
    char_t *updateTypeStr = NULL;
    int updateType;

    updateTypeStr = websGetVar(wp,T("updateType"),"0");
    updateType = atoi(updateTypeStr);
    adfilter_Config(1,updateType);
#endif

    a_assert(wp);

    GroupName=websGetVar(wp,T("GroupName"), NULL);
    group_day=websGetVar(wp,T("day"), NULL);
    group_timestart=websGetVar(wp,T("timestart"), NULL);
    group_timestop=websGetVar(wp,T("timestop"), NULL);

    GroupNameType = websGetVar(wp,T("GroupNameType"), T("")); 
    sourceIP = websGetVar(wp,T("sourceIP"), T(""));

	maxProf = MAX_ACT_CONTROL_PROFILES;
	PdbPolicy = websGetVar(wp,T("pdbPolicy"), NULL);
    
    action = websGetVar(wp, T("Action"), T("")); 

    ProfInstLowHigh(mibType, &max, &min);
    
    uttSemP(SEM_UTTNF_NO,0);
    semFlag = 1;
    profGroupBase  = (GroupBaseProfile*)ProfGetInstPointByIndex(MIB_PROF_GROUP_BASE, 0);
    if(strcmp(action,"add") == 0)
    {
        instIndex = ProfGetInstIndexByName(mibType, GroupName);
	if(instIndex != PROFFAIL) 
	{
	    goto out;
	}
	/*ÁªüËÆ°È°µÈù¢ÂØπÂ∫îÁªÑÁ±ªÂûãÊòØÂê¶ÂÆû‰æãÂ∑≤ÁªèÊ∑ªÂä†Âà∞ÊúÄÂ§ö*/
	if(ProfCountInstByValue(mibType, OFFANDSIZE(GroupBaseProfile, groupType), 0u, (Pointer)&webGroupType) >= maxProf)
	{
	    goto out;
	}
	instIndex = ProfNewInst(mibType, GroupName, FALSE);
	    profAction = PROF_CHANGE_ADD;
    }
    ProfBackupByIndex(mibType, profAction, instIndex, &profhead);
    profGroupBase = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, instIndex);
    if(profAction == PROF_CHANGE_ADD)
    {
	profGroupBase->groupType = webGroupType;/*ËÆ∞ÂΩïÁ±ªÂûãÊñπ‰æø‰∏çÂêåÁöÑÈ°µÈù¢‰ΩøÁî®*/
	/*mantis11137*/
    }
    /*‰ª•‰∏ãÂà§Êñ≠Èò≤Ê≠¢ÂÅ∂ÁÑ∂ÊÉÖÂÜµÂá∫Áé∞ÁöÑ È°µÈù¢‰øÆÊîπÊìç‰ΩúÊó∂ È°µÈù¢‰∏éÊìç‰ΩúÁöÑÁªÑÁ±ªÂûã‰∏çÂåπÈÖç*/
    ProfSetNameByPoint(profGroupBase, GroupName);
    profGroupBase->day= days2Byte(group_day); 
    strcpy(profGroupBase->timeStart, group_timestart); 
    strcpy(profGroupBase->timeStop, group_timestop);
	strncpy(PdbPolicy1, PdbPolicy, UTT_MAX_PDB_POLICY_CNT * 4);
	UTTSetPdbPolicy((Uint8 *)profGroupBase->pdbPolicy1, PdbPolicy1);
     
	profGroupBase->addrTypeEn = ADDR_IPGRP; 
	strcpy(profGroupBase->ipGrpName, GroupNameType);
    
    semFlag = 0;
    uttSemV(SEM_UTTNF_NO,0);
	ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
    if(semFlag == 1)
    {
	uttSemV(SEM_UTTNF_NO,0);
    }
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, "advideo_Sui.asp");
#else
    websRedirect(wp, "advideo.asp");
#endif
    return;
}
/*
 *‰∏äÁΩëË°å‰∏∫ÁÆ°ÁêÜÂàóË°®Âà†Èô§ÂáΩÊï∞
 *
 */
static void formGroupConfigDel(webs_t wp, char_t *path, char_t *query)
{
    char_t *GroupName=NULL, *tmp = NULL;
    const char *errMsg=NULL;
    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    struProfAlloc *profList  = NULL;
    a_assert(wp);
  
    GroupName = "utt_action"; 

#if (UTT_SMART_UI == FYES)
    configSysProf(0);
#endif
#if (ADFILTER == FYES)
    char_t *updateTypeStr = NULL;
    int updateType;

    updateTypeStr = websGetVar(wp,T("updateType"),"0");
    updateType = atoi(updateTypeStr);
    adfilter_Config(0,updateType);
#endif
    tmp=strtok(GroupName, UTT_SPLIT_SIGN_STR);

    uttSemP(SEM_UTTNF_NO,0);
    while(tmp != NULL) {

	ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profList);
	ProfDelInstByName(mibType, tmp); 
	tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
    }
    uttSemV(SEM_UTTNF_NO,0);
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
out:
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, "advideo_Sui.asp");
#else
    websRedirect(wp, "advideo.asp");
#endif
}
void formDefineAdvideo(void)
{
    websAspDefine(T("aspAdvideo"), aspAdvideo);
    websFormDefine(T("ConfigAdvideo"), formConfigAdvideo);
    websFormDefine(T("ConfigAdvideoDel"), formGroupConfigDel);
}
#endif
