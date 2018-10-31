#include "uttMachine.h"
#if (NET_SHARE_USER == FYES)
#include	<stdlib.h>
#include	<stdio.h>
#include	<sys/sysinfo.h>
#include    <fcntl.h>
#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include	"wuScript.h"
#include	"translate.h"

static int aspOutputNetShareUserList(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_NET_SHARE_USER;
    NetShareUserProfile *prof = NULL;
    int min, max, num = 0, index = 0;

    websWrite(wp,"var UserNames=new Array();");

    ProfInstLowHigh(mibType, &max, &min);
    for(index = min; index < max; index++)
    {
	prof = (NetShareUserProfile *)ProfGetInstPointByIndex(mibType, index);
	if (strcmp(prof->head.name, ""))
	{
	    websWrite(wp,"UserNames[%d] = \"%s\";", num, prof->head.name);
	    websWrite(wp,"passwords[%d] = \"%s\";", num, prof->password);
	    websWrite(wp,"authoritys[%d] = \"%s\";", num, prof->jurisdiction ? "rw" : "r");
	    websWrite(wp,"ftpauths[%d] = \"%s\";", num, prof->allow_ftp ? "on" : "off" );
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

static int aspOutNetShreUserOneInfo(int eid, webs_t wp, int argc, char_t **argv)
{
    char *edit_name;
    MibProfileType mibType = MIB_PROF_NET_SHARE_USER;
    NetShareUserProfile *prof = NULL;
    MibProfileType mibTypeAd = MIB_PROF_ADMIN_CONF;
    AdminConfProfile *profAd = NULL;
    const char *ErrMsg = NULL;

    if(ejArgs(argc,argv,T("%s"), &edit_name) <1) 
    {
        ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_USER_NAME_NOG);
        setTheErrorMsg(ErrMsg);
        goto out;
    }

    prof = (NetShareUserProfile *)ProfGetInstPointByName(mibType, edit_name);
    if(prof != NULL)
    {
        websWrite(wp,"var usernames= \"%s\";\n", prof->head.name);
        if( strcmp("admin",edit_name) == 0 ){
        profAd = (AdminConfProfile *)ProfGetInstPointByIndex(mibTypeAd, 0);
        websWrite(wp,"var passwd1s= \"%s\";\n", profAd->password);
        websWrite(wp,"var passwd2s= \"%s\";\n", profAd->password);
        }else{
        websWrite(wp,"var passwd1s= \"%s\";\n", prof->password);
        websWrite(wp,"var passwd2s= \"%s\";\n", prof->password);
        }
	    websWrite(wp,"Auth  = %d;\n",  prof->jurisdiction );
	    websWrite(wp,"ftpAuth = %d;\n",  prof->allow_ftp );
    }
out:
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

static void formNetShareUser(webs_t wp, char_t *path, char_t *query)
{
    char *userName, *passWord1, *action, *userNameOld,*Auth,*ftpAuth;
    int min, max, index = 0;
    MibProfileType mibType = MIB_PROF_NET_SHARE_USER;
    struProfAlloc *profhead = NULL;
    NetShareUserProfile *prof = NULL;
    const char *ErrMsg = NULL;

    userName = websGetVar(wp, T("username"), T("")); 
    passWord1 = websGetVar(wp, T("passwd1"), T("")); 
    action = websGetVar(wp, T("Action"), T(""));
    userNameOld = websGetVar(wp, T("usernameold"), T(""));
    Auth = websGetVar(wp, T("auth"), T(""));
    ftpAuth = websGetVar(wp, T("ftpauth"), T(""));

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
                prof = (NetShareUserProfile *)ProfGetInstPointByIndex(mibType, index);
                if (!strcmp(userName, prof->head.name))
                {
                    ErrMsg = getTransDataForC(C_LANG_INDEX_LOSE_FOR_SAME_USER_NAME);
                    setTheErrorMsg(ErrMsg);
                    goto out;
                }
            }
            index = ProfNewInst(mibType, userName, FALSE);
		    ProfBackupByIndex(mibType, PROF_CHANGE_ADD, index, &profhead);
            prof = (NetShareUserProfile *)ProfGetInstPointByIndex(mibType, index);
            strcpy(prof->head.name, userName);
            strcpy(prof->password, passWord1);
            prof->jurisdiction = strtol(Auth,NULL,10);
            prof->allow_ftp = strtol(ftpAuth,NULL,10);
        }else{
            if (strcmp(userNameOld, userName))
            {
                if (!strcmp(userNameOld, "guest")){
//                    setTheErrorMsg("不能更改guest用户");
                    ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_MODIFY_GUEST_USER);
                    setTheErrorMsg(ErrMsg);
                    goto out;
                }
                for (index = min; index < max; index++)
                {
                    prof = (NetShareUserProfile *)ProfGetInstPointByIndex(mibType, index);
                    if (!strcmp(userName, prof->head.name))
                    {
                        ErrMsg = getTransDataForC(C_LANG_INDEX_LOSE_FOR_SAME_USER_NAME);
                        setTheErrorMsg(ErrMsg);
                        goto out;
                    }
                }
                for (index = min; index < max; index++)
                {
                    prof = (NetShareUserProfile *)ProfGetInstPointByIndex(mibType, index);
                    if (!strcmp(prof->head.name, userNameOld)) break;
                }
		        ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, index, &profhead);
                strcpy(prof->head.name, userName);
                strcpy(prof->password, passWord1);
                prof->jurisdiction = strtol(Auth,NULL,10);
                prof->allow_ftp = strtol(ftpAuth,NULL,10);
            }
            else
            {
                for (index = min; index < max; index++)
                {
                    prof = (NetShareUserProfile *)ProfGetInstPointByIndex(mibType, index);
                    if (!strcmp(prof->head.name, userNameOld)) break;
                }
		        ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, index, &profhead);
                strcpy(prof->head.name, userName);
                strcpy(prof->password, passWord1);
                prof->jurisdiction = strtol(Auth,NULL,10);
                prof->allow_ftp = strtol(ftpAuth,NULL,10);
            }
        }
        ProfUpdate(profhead);/*给speedweb发消息*/
        ProfFreeAllocList(profhead);
        nvramWriteCommit();/*保存flash*/
    }
    else
    {
//        setTheErrorMsg("不能修改默认管理员");
        ErrMsg = getTransDataForC(C_LANG_INDEX_CHANGE_DEFAULT_USER_NAME);
        setTheErrorMsg(ErrMsg);
    }

out:
    websRedirect(wp, "ShareAccountList.asp");
}
static void formNetShareUserDel(webs_t wp, char_t *path, char_t *query)
{
    char *UserName = NULL, *tmp, *ptr;
    int min, max, index = 0;
    MibProfileType mibType = MIB_PROF_NET_SHARE_USER;
    NetShareUserProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
	const char *ErrMsg = NULL;

    UserName = websGetVar(wp,T("delstr"),T(""));
    tmp=strtok_r(UserName,UTT_SPLIT_SIGN_STR,&ptr);//可能删除多条映射
    ProfInstLowHigh(mibType, &max, &min);
    while(tmp != NULL)
    {
	for (index = min; index < max; index++)
	{
	    prof = (NetShareUserProfile *)ProfGetInstPointByIndex(mibType, index);
	    if (!strcmp(tmp, "admin"))
	    {
			ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_DEL_DEFAULT_USER);
			setTheErrorMsg(ErrMsg);
			goto strtok;
	    }
	    if (!strcmp(tmp, "guest"))
	    {
//			setTheErrorMsg("不能删除guest用户");
			ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_DEL_GUEST_USER);
			setTheErrorMsg(ErrMsg);
			goto strtok;
	    }
	    if (!strcmp(tmp, prof->head.name))
	    {
		    ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profhead);
			ProfDelInstByIndex(mibType, index);
	    }
	}
strtok:
        tmp=strtok_r(NULL,UTT_SPLIT_SIGN_STR,&ptr);
    }

	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();/*保存flash*/

    websRedirect(wp, "ShareAccountList.asp");

}
static void formNetShareUserDelAll(webs_t wp, char_t *path, char_t *query)
{
    int min, max, index = 0;
    MibProfileType mibType = MIB_PROF_NET_SHARE_USER;
    NetShareUserProfile *prof = NULL;
    struProfAlloc *profhead = NULL;

    ProfInstLowHigh(mibType, &max, &min);
    for (index = (min+2); index < max; index++)
    {
        prof = (NetShareUserProfile *)ProfGetInstPointByIndex(mibType, index);
        ProfBackupByIndex(mibType, PROF_CHANGE_DEL, index, &profhead);
        ProfDelInstByIndex(mibType, index);
    }

    ProfUpdate(profhead);/*发消息*/
    ProfFreeAllocList(profhead);
    nvramWriteCommit();

    websRedirect(wp, "ShareAccountList.asp");
}

extern void formDefineNetShareUser(void) {
    websAspDefine(T("aspOutputNetShareUserList"), aspOutputNetShareUserList);
    websAspDefine(T("aspOutNetShreUserOneInfo"), aspOutNetShreUserOneInfo);
	websFormDefine(T("formNetShareUser"), formNetShareUser);
	websFormDefine(T("formNetShareUserDel"),formNetShareUserDel);
	websFormDefine(T("formNetShareUserDelAll"),formNetShareUserDelAll);
}

#endif
