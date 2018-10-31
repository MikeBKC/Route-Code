#ifndef __APMAIL_SHARE_H__
#define __APMAIL_SHARE_H__


#define MAX_EMAIL_LENGTH 64
#define MAX_DOMAIN_LENGTH 60
#define MAX_PASSWD_LENGTH 30
#define MAX_EMAIL_TITLE_LENGTH 30

#define AP_MAIL_FILE_PATH "/var/log/ap_link_info.txt"
typedef struct st_ap_mail_profile {
		mibHead head;
		Uint32   spaceTime;
		char senderEmail[MAX_EMAIL_LENGTH+1];
		char senderEmailPasswd[MAX_PASSWD_LENGTH+1];
		char receiverEmail[MAX_EMAIL_LENGTH+1];
		char smtpServer[MAX_DOMAIN_LENGTH+1];
		char emailTitle[MAX_EMAIL_TITLE_LENGTH+1];
}ApMailProfile;


#endif
