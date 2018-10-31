#include    <stdio.h>
#include    "uttMachine.h"
#if(ADMIN_CONF == FYES)
#include    "mib.h"
#include    "profacce.h"

#include    <stdlib.h>
#include    <sys/ioctl.h>
#include    <net/if.h>
#include    <net/route.h>
#include    <string.h>
#include    <dirent.h>
#include    "internet.h"
#include    "webs.h"
#include    "utils.h"
#include    "firewall.h"
#include    "management.h"
#include    "station.h"
#include    "wireless.h"
#include    "um.h"
#include    "uttfunction.h"

#include    "linux/autoconf.h"  //kernel config
#include    "config/autoconf.h" //user config
#include	"translate.h"

#define     INDEX_RULEID    0 
#define     USER_ADD	"add" 
#define     USER_DEL	"del" 

extern void setSysAdm(webs_t wp, char_t *path, char_t *query);
static int getUserList(int eid, webs_t wp, int argc, char_t **argv);
static int GetMaxAdmNum(int eid, webs_t wp, int argc, char_t **argv);
static void formUser(webs_t wp, char_t *path, char_t *query);
static int getOneUserInfo(int eid, webs_t wp, int argc, char_t **argv);
static void formUserListDel(webs_t wp, char_t *path, char_t *query);
static void formUserListDelAll(webs_t wp, char_t *path, char_t *query);

/***********************************************************************
 * �� �� ����   EditUser
 * ����������	��ӻ���ɾ��һ���û�
 * �������:    UserList - �����û�����������ַ���
 *	        action   - ���ھ�������û�����ɾ���û�
 * �������ڣ�	2010-3-17
 * �޸����ڣ�
 * ���ߣ�       �ص���
 * ����˵����	��
***********************************************************************/
static int EditUser(AdminConfProfile *prof, char* action)
{
    char *userName, *passWord;
    userName = prof->head.name;
    passWord = prof->password;
    if(0 == strcmp(action,USER_ADD))
    {
	umAddUser(userName, passWord, T("adm"), FALSE, FALSE);
    }
    else
    {
	umDeleteUser(userName);
    }
    return 0;
}
/***********************************************************************
 * �� �� ����   GetMaxAccountNum
 * ����������	�������ж�ȡʵ�������
 * �������ڣ�	2010-3-17
 * �޸����ڣ�
 * ���ߣ�       �ص���
 * ����˵����	��
***********************************************************************/
static int GetMaxAdmNum(int eid, webs_t wp, int argc, char_t **argv)
{
    websWrite(wp,"var TotalAccount = \"%d\";", MAX_ADMIN_CONF_PROFILES);
    return 0;
}
    
/***********************************************************************
 * �� �� ����   getUserList
 * ����������	�������ж�ȡ�û��б�������ÿ���û�����ҳ����ʾ
 * �������ڣ�	2010-3-17
 * �޸����ڣ�
 * ���ߣ�       �ص���
 * ����˵����	��
***********************************************************************/
static int getUserList(int eid, webs_t wp, int argc, char_t **argv)
{
    int min, max, num, index = 0;
    MibProfileType mibType = MIB_PROF_ADMIN_CONF;
    AdminConfProfile *prof = NULL;

    websWrite(wp,"var UserNames=new Array();");
    ProfInstLowHigh(mibType, &max, &min);
    num = min;
    for(index = min; index < max; index++)
    {
	prof = (AdminConfProfile *)ProfGetInstPointByIndex(mibType, index);
	if (strcmp(prof->head.name, ""))
	{
	    websWrite(wp,"UserNames[%d] = \"%s\";", num, prof->head.name);
	    num++;
	}
	else
	{
	    continue;
	}
    }
    websWrite(wp,"var totalrecs=%d;", num);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
/***********************************************************************
 * �� �� ����   formUser
 * ����������	��ҳ����ֵ������flash�������ӻ��޸�ĳһ�û�
 * �������ڣ�	2010-3-17
 * �޸����ڣ�
 * ���ߣ�       �ص���
 * ����˵����	��
***********************************************************************/
static void formUser(webs_t wp, char_t *path, char_t *query)
{
    char *userName, *passWord1, *action, *userNameOld;
    int min, max, index = 0;
    MibProfileType mibType = MIB_PROF_ADMIN_CONF;
    AdminConfProfile *prof = NULL;
	const char *ErrMsg = NULL;

    userName = websGetVar(wp, T("username"), T("")); 
    passWord1 = websGetVar(wp, T("passwd1"), T("")); 
    action = websGetVar(wp, T("Action"), T(""));
    userNameOld = websGetVar(wp, T("usernameold"), T(""));
    ProfInstLowHigh(mibType, &max, &min);
    if (strcmp(userNameOld, "admin")) 
    {
	if (strlen(userName) > (UTT_INST_NAME_LEN - 1))
	{
		ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_LEN_LESS_11);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	if (strlen(passWord1) > (UTT_PASSWD_LEN - 1))
	{
		ErrMsg = getTransDataForC(C_LANG_INDEX_USER_PASSWD_LEN_LESS_31);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	if (!strcmp(action, "add"))
	{
	    for (index = min; index < max; index++)
	    {
		prof = (AdminConfProfile *)ProfGetInstPointByIndex(mibType, index);
		if (!strcmp(userName, prof->head.name))
		{
			ErrMsg = getTransDataForC(C_LANG_INDEX_LOSE_FOR_SAME_USER_NAME);
		    setTheErrorMsg(ErrMsg);
		    goto out;
		}
	    }
	    index = ProfNewInst(mibType, userName, FALSE);
	    prof = (AdminConfProfile *)ProfGetInstPointByIndex(mibType, index);
	    strcpy(prof->head.name, userName);
	    strcpy(prof->password, passWord1);
	    nvramWriteCommit();
	    EditUser(prof, USER_ADD);
	}
	else
	{
	    if (strcmp(userNameOld, userName))
	    {
		for (index = min; index < max; index++)
		{
		    prof = (AdminConfProfile *)ProfGetInstPointByIndex(mibType, index);
		    if (!strcmp(userName, prof->head.name))
		    {
				ErrMsg = getTransDataForC(C_LANG_INDEX_LOSE_FOR_SAME_USER_NAME);
				setTheErrorMsg(ErrMsg);
				goto out;
		    }
		}
		for (index = min; index < max; index++)
		{
		    prof = (AdminConfProfile *)ProfGetInstPointByIndex(mibType, index);
		    if (!strcmp(prof->head.name, userNameOld)) break;
		}
		EditUser(prof, USER_DEL);
		strcpy(prof->head.name, userName);
		strcpy(prof->password, passWord1);
		nvramWriteCommit();
		EditUser(prof, USER_ADD);
	    }
	    else
	    {
		for (index = min; index < max; index++)
		{
		    prof = (AdminConfProfile *)ProfGetInstPointByIndex(mibType, index);
		    if (!strcmp(prof->head.name, userNameOld)) break;
		}
		EditUser(prof, USER_DEL);
		strcpy(prof->head.name, userName);
		strcpy(prof->password, passWord1);
		nvramWriteCommit();
		EditUser(prof, USER_ADD);
	    }
	}
    }
    else
    {
	if(strcmp(userName, "admin"))
	{
			ErrMsg = getTransDataForC(C_LANG_INDEX_CHANGE_DEFAULT_USER_NAME);
	        setTheErrorMsg(ErrMsg);
	}
	else
	{
		setSysAdm(wp, path, query);
	}
    }

out:
    websRedirect(wp, "User.asp");
}

/***********************************************************************
 * �� �� ����   getOneUserInfo
 * ����������	��������ȡ��ĳһ�û����û��������룬����ҳ��ʾ��Ҫ��ʾ
 *	        ��һ���û�����Ϣ����ҳ�������û�������
 * �������ڣ�	2010-3-17
 * �޸����ڣ�
 * ���ߣ�       �ص���
 * ����˵����	��
***********************************************************************/
static int getOneUserInfo(int eid, webs_t wp, int argc, char_t **argv)
{
    char *edit_name;
    int min, max, index = 0;
    MibProfileType mibType = MIB_PROF_ADMIN_CONF;
    AdminConfProfile *prof = NULL;
	const char *ErrMsg = NULL;

    if(ejArgs(argc,argv,T("%s"), &edit_name) <1) 
    {
		ErrMsg = getTransDataForC(C_LANG_INDEX_READ_EDIT_NAME_VAL_ERR);
		setTheErrorMsg(ErrMsg);
        goto out;
    }
    DBGPRINT("edit_name:%s\n",edit_name);
    if(strcmp(edit_name, "admin"))
    {
	ProfInstLowHigh(mibType, &max, &min);
	for (index = min; index < max; index++)
	{
	    prof = (AdminConfProfile *)ProfGetInstPointByIndex(mibType, index);
	    if (!strcmp(edit_name, prof->head.name))
	    {
		prof =(AdminConfProfile *)ProfGetInstPointByIndex(mibType, index);
		websWrite(wp,"var usernames= \"%s\";", prof->head.name);
		websWrite(wp,"var passwd1s= \"%s\";", prof->password);
		websWrite(wp,"var passwd2s= \"%s\";", prof->password);
		break;
	    }
	    if(index == (max - 1))
	    {
			ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_USER_NAME);
			setTheErrorMsg(ErrMsg);
			goto out;
	    }
	}
    }
    else
    {
        prof = (AdminConfProfile *)ProfGetInstPointByIndex(mibType, 0);
	websWrite(wp,"var usernames= \"%s\";", "admin");
        websWrite(wp,"var passwd1s= \"%s\";", prof->password);
        websWrite(wp,"var passwd2s= \"%s\";", prof->password);
    }
out:
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
/***********************************************************************
 * �� �� ����   formUserListDel
 * ����������	��flash��ϵͳ��ɾ��ĳһ�û���Ҫɾ����һ���û�����ҳ��
 *		�����û�������
 * �������ڣ�	2010-3-17
 * �޸����ڣ�
 * ���ߣ�       �ص���
 * ����˵����	��
***********************************************************************/
static void formUserListDel(webs_t wp, char_t *path, char_t *query)
{
    char *UserName = NULL, *tmp, *ptr;
    int min, max, index = 0;
    MibProfileType mibType = MIB_PROF_ADMIN_CONF;
    AdminConfProfile *prof = NULL;
	const char *ErrMsg = NULL;

    UserName = websGetVar(wp,T("delstr"),T(""));
    tmp=strtok_r(UserName,UTT_SPLIT_SIGN_STR,&ptr);//����ɾ������ӳ��
    ProfInstLowHigh(mibType, &max, &min);
    while(tmp != NULL)
    {
	for (index = min; index < max; index++)
	{
	    prof = (AdminConfProfile *)ProfGetInstPointByIndex(mibType, index);
	    if (!strcmp(tmp, "admin"))
	    {
			ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_DEL_DEFAULT_USER);
			setTheErrorMsg(ErrMsg);
			goto strtok;
	    }
	    if (!strcmp(tmp, prof->head.name))
	    {
			EditUser(prof, USER_DEL);
			ProfDelInstByIndex(mibType, index);
			nvramWriteCommit();
	    }
	}
strtok:
        tmp=strtok_r(NULL,UTT_SPLIT_SIGN_STR,&ptr);
    }
    websRedirect(wp, "User.asp");
}
/***********************************************************************
 * �� �� ����   formUserListDelAll
 * ����������	ɾ��flash�е������û����������Ǵ�ϵͳ��Ҳɾ��
 * �������ڣ�	2010-3-17
 * �޸����ڣ�
 * ���ߣ�       �ص���
 * ����˵����	��
***********************************************************************/
static void formUserListDelAll(webs_t wp, char_t *path, char_t *query)
{
    int min, max, index = 0;
    MibProfileType mibType = MIB_PROF_ADMIN_CONF;
    AdminConfProfile *prof = NULL;
    
    ProfInstLowHigh(mibType, &max, &min);
    for (index = (min+1); index < max; index++)
    {
	prof = (AdminConfProfile *)ProfGetInstPointByIndex(mibType, index);
	EditUser(prof, USER_DEL);
	ProfDelInstByIndex(mibType, index);
    }
    nvramWriteCommit();
    websRedirect(wp, "User.asp");
}
/***********************************************************************
 * �� �� ����   init_admin
 * ����������	��flash��ȡ���������ã�������û���ϵͳ��
 * �������ڣ�	2010-3-17
 * �޸����ڣ�
 * ���ߣ�       �ص���
 * ����˵����	��
***********************************************************************/
void init_admin(void)
{
    int min, max, index = 0;
    MibProfileType mibType = MIB_PROF_ADMIN_CONF;
    AdminConfProfile *prof = NULL;
    
    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
	prof = (AdminConfProfile *)ProfGetInstPointByIndex(mibType, index);
	if (0 == strcmp(prof->head.name, "admin"))
	{
	    doSystem("chpasswd.sh %s %s", prof->head.name, prof->password);
	}
	if (!strcmp(prof->head.name, "")) break;
	EditUser(prof, USER_DEL);
	EditUser(prof, USER_ADD);
    }
}
/***********************************************************************
 * �� �� ����   formDefineAdmin
 * ����������	����ҳ�ϵ��õ�asp����ת���ɸ��ļ��ж���ĺ���
 * �������ڣ�	2010-3-17
 * �޸����ڣ�
 * ���ߣ�       �ص���
 * ����˵����	��
***********************************************************************/
void formDefineAdmin(void) {
	websAspDefine(T("aspOutUserList"),getUserList);
	websAspDefine(T("GetMaxAdmNum"),GetMaxAdmNum);
	websFormDefine(T("formUser"),formUser);
	websFormDefine(T("formUserDelAll"),formUserListDelAll);
	websFormDefine(T("formUserDel"),formUserListDel);
	websAspDefine(T("aspOutUserOneInfo"),getOneUserInfo);
}

#endif
