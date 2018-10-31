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

extern void setSysAdm(webs_t wp, char_t *path, char_t *query);

#if (UTT_SMART_UI == FYES)
#if (UTT_LOGIN_LIMIT == FYES)
extern int allowLoginFlag;
extern unsigned long forbidLoginTimeEnd;
static int aspGetForbidLessTime(int eid, webs_t wp, int argc, char_t **argv);
#endif
static int isDefaultPassword(int eid, webs_t wp, int argc, char_t **argv);
#endif

#if (UTT_SMART_UI == FYES)
/***********************************************************************
 * 函 数 名：   isDefaultPassword
 * 功能描述：	判断当前用户的密码是否为默认密码
 * 创建日期：	2013-11-7
 * 修改日期：
 * 作者：       赵冲
 * 附加说明：	无
***********************************************************************/
static int isDefaultPassword(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_ADMIN_CONF;
    AdminConfProfile *prof = NULL;
    char_t username[UTT_INST_NAME_LEN+1] = "admin";/*默认名称admin*/
    char_t defaultPasswd[UTT_PASSWD_LEN+1] = "admin";/*默认密码为admin*/
    const char *ErrMsg = NULL;

    prof = (AdminConfProfile *)ProfGetInstPointByName(mibType,username);

    if (prof == NULL) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_USER_NAME);
	setTheErrorMsg(ErrMsg);
    } else {
	/*如果当前用户的密码为默认密码，则向客户端输出1，否则输出0*/
	if (strncmp((const char*)defaultPasswd,(const char*)prof->password,UTT_PASSWD_LEN) == 0) {
	    websWrite(wp,"var isDefaultPasswd = %d;", 1);
	} else {
	    websWrite(wp,"var isDefaultPasswd = %d;", 0);
	}
    }
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
#if (UTT_LOGIN_LIMIT == FYES)
extern char* read_file_sui(char *filename, char *buf)
{
    FILE *fp;

    fp=fopen(filename, "r");
    if (fp == NULL ) {
        printf("open %s error\n", filename);
    }else {
        /* the content of the file is a byte, which indicant 
         * the link status of the chnl_name port. 
         * while '1' for up, and '0' for down */
        if ((fread((void *)buf, 127U, 1U, fp)) < 0U) {
            printf("read %s error\n", filename);
        } else {
            char *p=strchr(buf, '\n');
            if(p != NULL){
                *p='\0';
            }
        }
        fclose(fp);
    }
    return buf;
}
static int aspGetForbidLessTime(int eid, webs_t wp, int argc, char_t **argv)
{
    char buf[128]={0},*p;
    unsigned long nowTime,endTime;
    read_file_sui("/proc/uptime", buf);
    p=strchr(buf,'.');
    *p='\0';
    nowTime=strtol(buf, NULL, 10);
    endTime=forbidLoginTimeEnd;
    if (allowLoginFlag)
    {
	websWrite(wp,"var lessTimes = %d;", -1);
    }
    else
    {
	websWrite(wp,"var lessTimes = %d;", endTime - nowTime);
    }
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
#endif
#endif
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
    setSysAdm(wp, path, query);
#if (LOGIN_PAGE_DIY == FYES)
    websRedirect(wp, "login.asp");
#else
    websRedirect(wp, "index_login.asp");
#endif
    return;
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
    MibProfileType mibType = MIB_PROF_ADMIN_CONF;
    AdminConfProfile *prof = NULL;
    prof = (AdminConfProfile *)ProfGetInstPointByIndex(mibType, 0);
    doSystem("chpasswd.sh %s %s", prof->head.name, prof->password);
    umDeleteUser(prof->head.name);
    umAddUser(prof->head.name, prof->password, T("adm"), FALSE, FALSE);
    return;
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
	websFormDefine(T("formUser"),formUser);
#if (UTT_SMART_UI == FYES)
	websAspDefine(T("isDefaultPassword"),isDefaultPassword);
#if (UTT_LOGIN_LIMIT == FYES)
	websAspDefine(T("aspForbidLessTime"),aspGetForbidLessTime);
#endif
#endif
}

#endif
