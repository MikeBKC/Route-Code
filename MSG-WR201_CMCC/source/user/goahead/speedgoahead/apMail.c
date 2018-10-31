#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>
#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"
#if (FEATURE_AC == FYES)&&(FEATURE_AP_MAIL== FYES)

#define DEBUG_SWITCH 0
#if DEBUG_SWITCH
#define DEBUG_PRINTF(fmt, args...)               printf(fmt, ## args)
#else
#define DEBUG_PRINTF(fmt, args...)
#endif


extern void ap_send_mail(void);
extern Uint32 get_ap_mail_time(void);
extern int get_ap_mail_enable(void);
/***************************************************
 * 函数：send email
 * 功能：发送邮件
 * 参数：无
 * 返回：无
 * 作者：hyy
 * 日期：2013-11-04
 **************************************************/ 
extern void ap_send_mail(void)
{

	DEBUG_PRINTF("%s-%d\n",__func__, __LINE__);
	FILE *fp = NULL;
	char c;
	char *tmp=NULL;
	char revisions[32];
	struct utsname utsbuf;
	MibProfileType mibType = MIB_PROF_AP_MAIL;
	ApMailProfile *prof = NULL;

	memset(revisions, '\0', sizeof(revisions));
	uname(&utsbuf);
	tmp = strtok(utsbuf.version, " ");
	tmp = strtok(NULL, " ");
	sprintf(revisions, "%s%s", utsbuf.version, tmp);

	prof = (ApMailProfile *)ProfGetInstPointByIndex(mibType,0);

	DEBUG_PRINTF("spaceTime=%u,sender=%s,senderpass=%s,receive=%s,subject=%s,server=%s\n",prof->spaceTime,prof->senderEmail,prof->senderEmailPasswd,prof->receiverEmail,prof->emailTitle,prof->smtpServer);
	/*读文件是否有内容*/
	uttSemP(SEM_AP_MAIL,0);
	if ((fp = fopen(AP_MAIL_FILE_PATH,"a+")) == NULL)
	{
		printf("Open the file %s error!",AP_MAIL_FILE_PATH);
		return;
	}
	c = fgetc(fp);
	fclose(fp);
	uttSemV(SEM_AP_MAIL,0);
	fp = NULL;

	if (c!=EOF)
	{
		doSystem("cp /var/log/ap_link_info.txt /var/log/ap_link_log.txt -rf");
		doSystem("echo 'This E-mail is from device %s. Please read the attachment!' | sendmail -H %s:%s#%s -s %s -a /var/log/ap_link_log.txt -f %s %s",revisions,prof->senderEmail,prof->senderEmailPasswd,prof->smtpServer,prof->emailTitle, prof->senderEmail,prof->receiverEmail);
		DEBUG_PRINTF("%s-%d\n",__func__, __LINE__);
		/*清空文件*/
		uttSemP(SEM_AP_MAIL,0);
		if ((fp = fopen(AP_MAIL_FILE_PATH,"w+")) == NULL)
		{
			printf("Open the file %s error!",AP_MAIL_FILE_PATH);
			return;
		}
		fclose(fp);
		uttSemV(SEM_AP_MAIL,0);
		fp = NULL;
	}
	DEBUG_PRINTF("%s-%d\n",__func__, __LINE__);
    return;
}

/***************************************************
 * 函数：get_ap_mail_time
 * 功能：获取发送邮件间隔的时间
 * 参数：无
 * 返回：无
 * 作者：hyy
 * 日期：2013-11-04
 **************************************************/ 
extern Uint32 get_ap_mail_time(void)
{
	Uint32 ret = 0;
	MibProfileType mibType = MIB_PROF_AP_MAIL;
	ApMailProfile *prof = NULL;
	prof = (ApMailProfile *)ProfGetInstPointByIndex(mibType,0);
	uttSemP(SEM_AP_MAIL,0);
		ret = prof->spaceTime * 60u;
	uttSemV(SEM_AP_MAIL,0);
	return ret;
}

/***************************************************
 * 函数：get_ap_mail_enable
 * 功能：获取是否启用功能
 * 参数：无
 * 返回：无
 * 作者：hyy
 * 日期：2013-11-04
 **************************************************/ 
extern int get_ap_mail_enable(void)
{
	int ret = 0;
	MibProfileType mibType = MIB_PROF_AP_MAIL;
	ApMailProfile *prof = NULL;
	prof = (ApMailProfile *)ProfGetInstPointByIndex(mibType,0);
	uttSemP(SEM_AP_MAIL,0);
		if(prof->head.active == TRUE){
		ret = 1;
		}else{
		ret = 0;
		}
	uttSemV(SEM_AP_MAIL,0);
		return ret;
}
/********************************************************************
* 函数： apMailProfileChange
* 功能： 开关变化 生效函数
* 参数： 略
* 返回： 略
* 输出： 略
* 作者： hyy
* 创建： 2013-11-04
********************************************************************/
static void apMailProfileChange(Pointer tag, MibProfileType profileType,
        ProfChangeType changeType, Uint32 index,
        const ApMailProfile *oldProfileDataPtr, const ApMailProfile *newProfileDataPtr )        
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
        case PROF_CHANGE_DEL:
        case PROF_CHANGE_DELALL:
        default:/*default*/
            break;
    }
    return;
}
/****************************************************
 * 函数：funcInitApMailConf
 * 功能：apMail初始化
 * 参数：无
 * 返回：无
 * 作者：hyy
 * 日期：2013-11-04
 ***************************************************/
extern void funInitApMailConf(void)
{
    DEBUG_PRINTF("%s-%d\n",__func__, __LINE__);
    registerForProfileUpdates(MIB_PROF_AP_MAIL, (Pointer)NULL, (ProfileUpdateFnc)apMailProfileChange);/*触发函数*/
}
#endif

