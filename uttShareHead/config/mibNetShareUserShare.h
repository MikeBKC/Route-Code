#ifndef MIBNETSHAREUSER_SHARE_H
#define MIBNETSHAREUSER_SHARE_H

#if (NET_SHARE_USER == FYES)
#define ACCOUNT_PASSWD_LEN 32

typedef enum _net_share_jurisdiction{
    SHARE_READ_ONLY,
    SHARE_READ_WRITE
}net_share_jurisdiction;

typedef struct st_NetShareUser_Profile {
    mibHead head;
    Char password[ACCOUNT_PASSWD_LEN];
    net_share_jurisdiction jurisdiction;
    FunEnableEnum allow_ftp;
}NetShareUserProfile;
#endif

#endif
