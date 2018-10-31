#ifndef MIBFTPSERVER_SHARE_H
#define MIBFTPSERVER_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

#if (FTP_SERVER == FYES)
#define FTP_STORAGE "storage"
#define NET_SHARE_DIR_NAME_LEN 32
#define NET_SHARE_VOLUME_LEN 64
#define NET_SHARE_FILE_PATH_LEN 128

typedef struct st_FtpServer_Profile {
    mibHead head;
    FunEnableEnum ftpEnable;
    FunEnableEnum allow_wan_visit;
    Int32 ftp_port;
#if (UTT_SMART_UI == FYES)
    FunEnableEnum need_passwd;
    int change;
    unsigned int status;/*当前是否插入移动设备，1：已插入 0：未插入*/
#endif
}FtpServerProfile;

typedef struct st_FtpShareDir_Profile {
    mibHead head;
    Char net_share_volume_name[NET_SHARE_DIR_NAME_LEN];
    Char net_share_volume_path[NET_SHARE_VOLUME_LEN];
    FunEnableEnum select_all;
    Char net_share_file_path[NET_SHARE_FILE_PATH_LEN];
}FtpShareDirProfile;
#endif
#endif
