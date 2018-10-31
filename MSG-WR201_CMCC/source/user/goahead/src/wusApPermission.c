
#include <string.h>
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "mib.h"
#include "profacce.h"
#include "uttfunction.h"
#include "wuScript.h"
#include "translate.h"

#if (AP_PERMISSION == FYES)

/**********************************************************
* 函数名：assignProf
* 函数功能：修改默认配置选项
* 作者： kchen
**********************************************************/
static void assignProf(ApPermissionProfile *prof, char *headName, char *name, char *numStart, char *numEnd, char *flag)
{
	strcpy(prof->head.name, headName);
	strcpy(prof->ap_name, name);
	prof->head.active = TRUE;
	prof->num_start = (Uint32)strtoul(numStart, (char *)NULL, 10);
	prof->num_end = (Uint32)strtoul(numEnd, (char *)NULL, 10);
	if (0 == strcmp(flag, "allow")) {
		/* 允许的情况 */
		prof->action = PROF_ENABLE;
	} else {
		/* 禁止的情况 */
		prof->action = PROF_DISABLE;
	}

	return;
}

/**********************************************************
* 函数名：formApPermissionDefault
* 函数功能：保存默认配置选项
* 作者： kchen
**********************************************************/
static void formApPermissionDefault(webs_t wp, char_t *path, char_t *query)
{
	MibProfileType mibType = MIB_PROF_AP_PERMISSION;
	ApPermissionProfile *prof = NULL;
	ProfChangeType profType = PROF_CHANGE_ADD;
	struProfAlloc *profhead = NULL;

	char *defPms = NULL;

	/*默认配置选项值可以修改允许和禁止*/
	defPms = websGetVar(wp, "DefPms", T(""));

	prof = (ApPermissionProfile *)ProfGetInstPointByIndex(mibType, 0);
	if ((NULL != prof) && (TRUE == prof->head.active)) {
		ProfBackupByIndex(mibType, profType, 0, &profhead);
		/* 实际进行默认配置的修改*/
		assignProf(prof, "P0", "default", "0", "0", defPms);
		/* 功能生效*/
		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
		nvramWriteCommit();
	}

	websRedirect(wp, "apPermission.asp");
	return;
}

/**********************************************************
* 函数名：isOverlay
* 函数功能：判断AP权限中序列号是否重叠
* 作者： kchen
**********************************************************/
static int isOverlay(int sStart, int sEnd, int dStart, int dEnd)
{
	/*默认返回值为1 */
	int ret = 1;

	if ((sStart <= dEnd) && (sStart >= dStart)) {
		/* 重叠返回0 */
		ret = 0;
	}

	if ((sEnd <= dEnd) && (sEnd >= dStart)) {
		/* 重叠返回0 */
		ret = 0;
	}

	if ((sStart <= dStart) && (sEnd >= dEnd)) {
		/* 重叠返回0 */
		ret = 0;
	}

	return ret;
}

static void formApPermissionConfig(webs_t wp, char_t *path, char_t *query)
{
	char *newName, *beginNo, *numStart, *numEnd, *flag;
	char headName[UTT_INST_NAME_LEN];
	int min, max, index = 0, indexTmp = 0;
	Uint32 nStart, oldStartNum, nEnd;
	struProfAlloc *profhead = NULL;
	MibProfileType mibType = MIB_PROF_AP_PERMISSION;
	ApPermissionProfile *prof = NULL;
	ProfChangeType profType = PROF_CHANGE_ADD;
	const char *ErrMsg = NULL;
	memset(headName, '\0', sizeof(headName));

	newName = websGetVar(wp, "names", T(""));
	numStart = websGetVar(wp, "serialNoBegin", T(""));
	beginNo = websGetVar(wp, "beginNo", T(""));
	numEnd = websGetVar(wp, "serialNoEnd", T(""));
	flag = websGetVar(wp, "nonDefPms", T("PROF_ENABLE"));

	ProfInstLowHigh(mibType, &max, &min);

	if (strlen(newName) > (UTT_INST_NAME_LEN - 1)) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_LEN_LESS_11);
		setTheErrorMsg(ErrMsg);
		//setTheErrorMsg(T("用户名不能超过11个字节"));
		goto out;
	}

	nStart = (Uint32)strtoul(numStart, NULL, 10);
	nEnd   = (Uint32)strtoul(numEnd, NULL, 10);
	if (nStart > nEnd) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_END_NO_BIGGER_THAN_START_NO);
		setTheErrorMsg(ErrMsg);
		//setTheErrorMsg(T("结束序列号应大于起始序列号"));
		goto out;
	}

	if ('\0' == *beginNo) {
		for (index = min; index < max; index++) {
			prof = (ApPermissionProfile *)ProfGetInstPointByIndex(mibType, index);
			if ((NULL != prof) && (TRUE == prof->head.active) && (ProfInstIsFree((Pointer)prof) == 0)) {
				if (0 == isOverlay(nStart, nEnd, prof->num_start, prof->num_end)) {
					ErrMsg = getTransDataForC(C_LANG_INDEX_SERIAL_NO_OVERLAP);
					setTheErrorMsg(ErrMsg);
					//setTheErrorMsg(T("序列号重叠"));
					goto out;
				}
			}
		}

		index = ProfNewInst(mibType, numStart, FALSE);
		profType = PROF_CHANGE_ADD;
	} else {
		oldStartNum = (Uint32)strtoul(beginNo, NULL, 10);
		for (index = min; index < max; index++) {
			prof = (ApPermissionProfile *)ProfGetInstPointByIndex(mibType, index);
			if ((NULL != prof) && (TRUE == prof->head.active) && (ProfInstIsFree((Pointer)prof) == 0)) {
				if (oldStartNum == prof->num_start) {
					indexTmp = index;
				} else {
					if (0 == isOverlay(nStart, nEnd, prof->num_start, prof->num_end)) {
						ErrMsg = getTransDataForC(C_LANG_INDEX_SERIAL_NO_OVERLAP);
						setTheErrorMsg(ErrMsg);
						//setTheErrorMsg(T("序列号重叠"));
						goto out;
					}
				}
			}
		}
		index = indexTmp;
		profType = PROF_CHANGE_EDIT;
	}

	prof = (ApPermissionProfile *)ProfGetInstPointByIndex(mibType, index);
	if (NULL != prof) {
		sprintf(headName, "P%d", index);
		ProfBackupByIndex(mibType, profType, index, &profhead);
		assignProf(prof, headName, newName, numStart, numEnd, flag);
		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
		nvramWriteCommit();
	}

out:
	websRedirect(wp, "apPermission.asp");
	return;
}

/**********************************************************
* 函数名：formApListStaticDel
* 函数功能：删除选中AP权限配置选项
* 作者： kchen
**********************************************************/
static void formApListStaticDel(webs_t wp, char_t *path, char_t *query)
{
	char *delStr = NULL, *tmp;
	int min, max, tmpNum, index = 0, change = 0;
	MibProfileType mibType = MIB_PROF_AP_PERMISSION;
	ApPermissionProfile *prof = NULL;
	struProfAlloc *profhead = NULL;
	const char *ErrMsg = NULL;

	/* 从页面获取需要删除的条目 */
	delStr = websGetVar(wp,T("delstr"),T(""));

	ProfInstLowHigh(mibType, &max, &min);

	/* 可能有多个需要删除的选项，用逗号分隔 */
	tmp = strtok(delStr, UTT_SPLIT_SIGN_STR);
	while(tmp != NULL) {
		tmpNum = (Uint32)strtoul(tmp, (char *)NULL, 10);
		for (index = min; index < max; index++) {
			prof = (ApPermissionProfile *)ProfGetInstPointByIndex(mibType, index);
			if ((NULL != prof) && (TRUE == prof->head.active) && (ProfInstIsFree((Pointer)prof) == 0)) {
				/* 比较起始序列号 */
				if (tmpNum == prof->num_start) {
					/* 默认配置不让删除 */
					if (!strcmp("P0", prof->head.name)) {
						ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_DEL_DEFAULT);
						/* 错误信息：默认选项不可以删除 */
						setTheErrorMsg(ErrMsg);
						//setTheErrorMsg(T("不能删除默认选项"));
					} else {
						/* 备份旧的实例 */
						ProfBackupByIndex(mibType, PROF_CHANGE_DEL, index, &profhead);
						ProfDelInstByIndex(mibType, index);
						/* 配置有改动置1 */
						change = 1;
					}
				}
			}
		}

		/*寻找下一个删除的实例*/
		tmp = strtok((char *)NULL, UTT_SPLIT_SIGN_STR);
	}

	if (1 == change) {
		/* 配置有改动的情况 */
		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
		/* 写flash */
		nvramWriteCommit();
	}

	websRedirect(wp, "apPermission.asp");
	return;
}

/**********************************************************
* 函数名：formApListStaticDelAll
* 函数功能：删除全部AP权限配置选项
* 作者： kchen
**********************************************************/
static void formApListStaticDelAll(webs_t wp, char_t *path, char_t *query)
{
	int min, max, index = 0, change = 0;
	MibProfileType mibType = MIB_PROF_AP_PERMISSION;
	ApPermissionProfile *prof = NULL;
	struProfAlloc *profhead = NULL;

	ProfInstLowHigh(mibType, &max, &min);

	/* 默认选项不能删除，从第一个实例开始删 */
	for (index = (min + 1); index < max; index++) {
		prof = (ApPermissionProfile *)ProfGetInstPointByIndex(mibType, index);
		if (NULL != prof) {
			if (0 == change) {
				/* 旧的实例备份 */
				ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, index, &profhead);
				change = 1;
			}
			ProfDelInstByIndex(mibType, index);
	   }
	}

	if (1 == change) {
		/*有修改的情况*/
		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
		/* 写flash */
		nvramWriteCommit();
	}

	websRedirect(wp, "apPermission.asp");
	return;
}

/**********************************************************
* 函数名：getApPermisionGroup
* 函数功能：获取AP权限页面信息
* 作者： kchen
**********************************************************/
static int getApPermisionGroup(int eid, webs_t wp, int argc, char **argv)
{
	int min, max, index, arrayNum = 0;
	MibProfileType mibType = MIB_PROF_AP_PERMISSION;
	ApPermissionProfile *prof = NULL;

	/* AP权限名称 */
	websWrite(wp, "var names = new Array();\n");
	/* AP权限起始序列号 */
	websWrite(wp, "var noBegin = new Array();\n");
	/* AP权限结束序列号 */
	websWrite(wp, "var noEnd = new Array();\n");
	/* AP权限禁止允许 */
	websWrite(wp, "var actions = new Array();\n");

	ProfInstLowHigh(mibType, &max, &min);

	for (index = min; index < max; index++) {
		prof = (ApPermissionProfile *)ProfGetInstPointByIndex(mibType, index);
			if((prof != NULL) && (ProfInstIsFree((Pointer)prof) == 0)&& (prof->head.active == TRUE)) {
				websWrite(wp, "names[%d] = \"%s\";", arrayNum, prof->ap_name);
				websWrite(wp, "noBegin[%d] = \"%d\";", arrayNum, prof->num_start);
				websWrite(wp, "noEnd[%d] = \"%d\";", arrayNum, prof->num_end);
				if (prof->action == PROF_ENABLE) {
					/* AP权限允许 */
					websWrite(wp, "actions[%d] = \"%s\";", arrayNum, "1");
				} else {
					websWrite(wp, "actions[%d] = \"%s\";", arrayNum, "0");
				}
				arrayNum++;
			}
	}

	websWrite(wp, "var totalrecs = %d;", arrayNum);
	getTheErrorMsg(eid, wp, argc, argv);
	return 0;
}

/**********************************************************
* 函数名：formDefineApPermission
* 函数功能：页面调用函数与交互函数相对应
* 作者： kchen
**********************************************************/
void formDefineApPermission(void)
{
	websAspDefine(T("aspOutPutApPermission"), getApPermisionGroup);
	websFormDefine(T("formApPermissionDefault"), formApPermissionDefault);
	websFormDefine(T("formApPermissionConfig"), formApPermissionConfig);
	websFormDefine(T("formApListStaticDel"), formApListStaticDel);
	websFormDefine(T("formApListStaticDelAll"), formApListStaticDelAll);
}

#endif
