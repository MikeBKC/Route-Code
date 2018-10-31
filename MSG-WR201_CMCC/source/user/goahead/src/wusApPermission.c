
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
* ��������assignProf
* �������ܣ��޸�Ĭ������ѡ��
* ���ߣ� kchen
**********************************************************/
static void assignProf(ApPermissionProfile *prof, char *headName, char *name, char *numStart, char *numEnd, char *flag)
{
	strcpy(prof->head.name, headName);
	strcpy(prof->ap_name, name);
	prof->head.active = TRUE;
	prof->num_start = (Uint32)strtoul(numStart, (char *)NULL, 10);
	prof->num_end = (Uint32)strtoul(numEnd, (char *)NULL, 10);
	if (0 == strcmp(flag, "allow")) {
		/* �������� */
		prof->action = PROF_ENABLE;
	} else {
		/* ��ֹ����� */
		prof->action = PROF_DISABLE;
	}

	return;
}

/**********************************************************
* ��������formApPermissionDefault
* �������ܣ�����Ĭ������ѡ��
* ���ߣ� kchen
**********************************************************/
static void formApPermissionDefault(webs_t wp, char_t *path, char_t *query)
{
	MibProfileType mibType = MIB_PROF_AP_PERMISSION;
	ApPermissionProfile *prof = NULL;
	ProfChangeType profType = PROF_CHANGE_ADD;
	struProfAlloc *profhead = NULL;

	char *defPms = NULL;

	/*Ĭ������ѡ��ֵ�����޸�����ͽ�ֹ*/
	defPms = websGetVar(wp, "DefPms", T(""));

	prof = (ApPermissionProfile *)ProfGetInstPointByIndex(mibType, 0);
	if ((NULL != prof) && (TRUE == prof->head.active)) {
		ProfBackupByIndex(mibType, profType, 0, &profhead);
		/* ʵ�ʽ���Ĭ�����õ��޸�*/
		assignProf(prof, "P0", "default", "0", "0", defPms);
		/* ������Ч*/
		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
		nvramWriteCommit();
	}

	websRedirect(wp, "apPermission.asp");
	return;
}

/**********************************************************
* ��������isOverlay
* �������ܣ��ж�APȨ�������к��Ƿ��ص�
* ���ߣ� kchen
**********************************************************/
static int isOverlay(int sStart, int sEnd, int dStart, int dEnd)
{
	/*Ĭ�Ϸ���ֵΪ1 */
	int ret = 1;

	if ((sStart <= dEnd) && (sStart >= dStart)) {
		/* �ص�����0 */
		ret = 0;
	}

	if ((sEnd <= dEnd) && (sEnd >= dStart)) {
		/* �ص�����0 */
		ret = 0;
	}

	if ((sStart <= dStart) && (sEnd >= dEnd)) {
		/* �ص�����0 */
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
		//setTheErrorMsg(T("�û������ܳ���11���ֽ�"));
		goto out;
	}

	nStart = (Uint32)strtoul(numStart, NULL, 10);
	nEnd   = (Uint32)strtoul(numEnd, NULL, 10);
	if (nStart > nEnd) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_END_NO_BIGGER_THAN_START_NO);
		setTheErrorMsg(ErrMsg);
		//setTheErrorMsg(T("�������к�Ӧ������ʼ���к�"));
		goto out;
	}

	if ('\0' == *beginNo) {
		for (index = min; index < max; index++) {
			prof = (ApPermissionProfile *)ProfGetInstPointByIndex(mibType, index);
			if ((NULL != prof) && (TRUE == prof->head.active) && (ProfInstIsFree((Pointer)prof) == 0)) {
				if (0 == isOverlay(nStart, nEnd, prof->num_start, prof->num_end)) {
					ErrMsg = getTransDataForC(C_LANG_INDEX_SERIAL_NO_OVERLAP);
					setTheErrorMsg(ErrMsg);
					//setTheErrorMsg(T("���к��ص�"));
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
						//setTheErrorMsg(T("���к��ص�"));
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
* ��������formApListStaticDel
* �������ܣ�ɾ��ѡ��APȨ������ѡ��
* ���ߣ� kchen
**********************************************************/
static void formApListStaticDel(webs_t wp, char_t *path, char_t *query)
{
	char *delStr = NULL, *tmp;
	int min, max, tmpNum, index = 0, change = 0;
	MibProfileType mibType = MIB_PROF_AP_PERMISSION;
	ApPermissionProfile *prof = NULL;
	struProfAlloc *profhead = NULL;
	const char *ErrMsg = NULL;

	/* ��ҳ���ȡ��Ҫɾ������Ŀ */
	delStr = websGetVar(wp,T("delstr"),T(""));

	ProfInstLowHigh(mibType, &max, &min);

	/* �����ж����Ҫɾ����ѡ��ö��ŷָ� */
	tmp = strtok(delStr, UTT_SPLIT_SIGN_STR);
	while(tmp != NULL) {
		tmpNum = (Uint32)strtoul(tmp, (char *)NULL, 10);
		for (index = min; index < max; index++) {
			prof = (ApPermissionProfile *)ProfGetInstPointByIndex(mibType, index);
			if ((NULL != prof) && (TRUE == prof->head.active) && (ProfInstIsFree((Pointer)prof) == 0)) {
				/* �Ƚ���ʼ���к� */
				if (tmpNum == prof->num_start) {
					/* Ĭ�����ò���ɾ�� */
					if (!strcmp("P0", prof->head.name)) {
						ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_DEL_DEFAULT);
						/* ������Ϣ��Ĭ��ѡ�����ɾ�� */
						setTheErrorMsg(ErrMsg);
						//setTheErrorMsg(T("����ɾ��Ĭ��ѡ��"));
					} else {
						/* ���ݾɵ�ʵ�� */
						ProfBackupByIndex(mibType, PROF_CHANGE_DEL, index, &profhead);
						ProfDelInstByIndex(mibType, index);
						/* �����иĶ���1 */
						change = 1;
					}
				}
			}
		}

		/*Ѱ����һ��ɾ����ʵ��*/
		tmp = strtok((char *)NULL, UTT_SPLIT_SIGN_STR);
	}

	if (1 == change) {
		/* �����иĶ������ */
		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
		/* дflash */
		nvramWriteCommit();
	}

	websRedirect(wp, "apPermission.asp");
	return;
}

/**********************************************************
* ��������formApListStaticDelAll
* �������ܣ�ɾ��ȫ��APȨ������ѡ��
* ���ߣ� kchen
**********************************************************/
static void formApListStaticDelAll(webs_t wp, char_t *path, char_t *query)
{
	int min, max, index = 0, change = 0;
	MibProfileType mibType = MIB_PROF_AP_PERMISSION;
	ApPermissionProfile *prof = NULL;
	struProfAlloc *profhead = NULL;

	ProfInstLowHigh(mibType, &max, &min);

	/* Ĭ��ѡ���ɾ�����ӵ�һ��ʵ����ʼɾ */
	for (index = (min + 1); index < max; index++) {
		prof = (ApPermissionProfile *)ProfGetInstPointByIndex(mibType, index);
		if (NULL != prof) {
			if (0 == change) {
				/* �ɵ�ʵ������ */
				ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, index, &profhead);
				change = 1;
			}
			ProfDelInstByIndex(mibType, index);
	   }
	}

	if (1 == change) {
		/*���޸ĵ����*/
		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
		/* дflash */
		nvramWriteCommit();
	}

	websRedirect(wp, "apPermission.asp");
	return;
}

/**********************************************************
* ��������getApPermisionGroup
* �������ܣ���ȡAPȨ��ҳ����Ϣ
* ���ߣ� kchen
**********************************************************/
static int getApPermisionGroup(int eid, webs_t wp, int argc, char **argv)
{
	int min, max, index, arrayNum = 0;
	MibProfileType mibType = MIB_PROF_AP_PERMISSION;
	ApPermissionProfile *prof = NULL;

	/* APȨ������ */
	websWrite(wp, "var names = new Array();\n");
	/* APȨ����ʼ���к� */
	websWrite(wp, "var noBegin = new Array();\n");
	/* APȨ�޽������к� */
	websWrite(wp, "var noEnd = new Array();\n");
	/* APȨ�޽�ֹ���� */
	websWrite(wp, "var actions = new Array();\n");

	ProfInstLowHigh(mibType, &max, &min);

	for (index = min; index < max; index++) {
		prof = (ApPermissionProfile *)ProfGetInstPointByIndex(mibType, index);
			if((prof != NULL) && (ProfInstIsFree((Pointer)prof) == 0)&& (prof->head.active == TRUE)) {
				websWrite(wp, "names[%d] = \"%s\";", arrayNum, prof->ap_name);
				websWrite(wp, "noBegin[%d] = \"%d\";", arrayNum, prof->num_start);
				websWrite(wp, "noEnd[%d] = \"%d\";", arrayNum, prof->num_end);
				if (prof->action == PROF_ENABLE) {
					/* APȨ������ */
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
* ��������formDefineApPermission
* �������ܣ�ҳ����ú����뽻���������Ӧ
* ���ߣ� kchen
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
