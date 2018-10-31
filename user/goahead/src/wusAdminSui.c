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
 * �� �� ����   isDefaultPassword
 * ����������	�жϵ�ǰ�û��������Ƿ�ΪĬ������
 * �������ڣ�	2013-11-7
 * �޸����ڣ�
 * ���ߣ�       �Գ�
 * ����˵����	��
***********************************************************************/
static int isDefaultPassword(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_ADMIN_CONF;
    AdminConfProfile *prof = NULL;
    char_t username[UTT_INST_NAME_LEN+1] = "admin";/*Ĭ������admin*/
    char_t defaultPasswd[UTT_PASSWD_LEN+1] = "admin";/*Ĭ������Ϊadmin*/
    const char *ErrMsg = NULL;

    prof = (AdminConfProfile *)ProfGetInstPointByName(mibType,username);

    if (prof == NULL) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_USER_NAME);
	setTheErrorMsg(ErrMsg);
    } else {
	/*�����ǰ�û�������ΪĬ�����룬����ͻ������1���������0*/
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
 * �� �� ����   formUser
 * ����������	��ҳ����ֵ������flash�������ӻ��޸�ĳһ�û�
 * �������ڣ�	2010-3-17
 * �޸����ڣ�
 * ���ߣ�       �ص���
 * ����˵����	��
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
 * �� �� ����   init_admin
 * ����������	��flash��ȡ���������ã�������û���ϵͳ��
 * �������ڣ�	2010-3-17
 * �޸����ڣ�
 * ���ߣ�       �ص���
 * ����˵����	��
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
 * �� �� ����   formDefineAdmin
 * ����������	����ҳ�ϵ��õ�asp����ת���ɸ��ļ��ж���ĺ���
 * �������ڣ�	2010-3-17
 * �޸����ڣ�
 * ���ߣ�       �ص���
 * ����˵����	��
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
