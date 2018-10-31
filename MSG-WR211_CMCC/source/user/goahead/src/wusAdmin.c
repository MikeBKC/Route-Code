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
 * 函 数 名：   EditUser
 * 功能描述：	添加或者删除一个用户
 * 输入参数:    UserList - 含有用户名和密码的字符串
 *	        action   - 用于决定添加用户还是删除用户
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
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
 * 函 数 名：   GetMaxAccountNum
 * 功能描述：	从配置中读取实例最大数
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static int GetMaxAdmNum(int eid, webs_t wp, int argc, char_t **argv)
{
    websWrite(wp,"var TotalAccount = \"%d\";", MAX_ADMIN_CONF_PROFILES);
    return 0;
}
    
/***********************************************************************
 * 函 数 名：   getUserList
 * 功能描述：	从配置中读取用户列表，分析出每个用户名送页面显示
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
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
 * 函 数 名：   formUser
 * 功能描述：	从页面获得值，存入flash，并增加或修改某一用户
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
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
 * 函 数 名：   getOneUserInfo
 * 功能描述：	从配置中取出某一用户的用户名与密码，送网页显示，要显示
 *	        哪一个用户的信息由网页传来的用户名决定
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
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
 * 函 数 名：   formUserListDel
 * 功能描述：	从flash和系统中删除某一用户，要删除哪一个用户由网页传
 *		来的用户名决定
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void formUserListDel(webs_t wp, char_t *path, char_t *query)
{
    char *UserName = NULL, *tmp, *ptr;
    int min, max, index = 0;
    MibProfileType mibType = MIB_PROF_ADMIN_CONF;
    AdminConfProfile *prof = NULL;
	const char *ErrMsg = NULL;

    UserName = websGetVar(wp,T("delstr"),T(""));
    tmp=strtok_r(UserName,UTT_SPLIT_SIGN_STR,&ptr);//可能删除多条映射
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
 * 函 数 名：   formUserListDelAll
 * 功能描述：	删除flash中的所有用户，并把它们从系统中也删除
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
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
 * 函 数 名：   init_admin
 * 功能描述：	从flash中取出所有配置，并添加用户到系统中
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
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
 * 函 数 名：   formDefineAdmin
 * 功能描述：	把网页上调用的asp函数转换成该文件中定义的函数
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
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
