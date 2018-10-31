#ifndef MIB_DDNS_SHARE_H
#define MIB_DDNS_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

/*
 * ddns配置，目前只支持一条ddns配置（3520G）
 * ddnsProvider	-   ddns供应商
 * ddnsHost	-   主机名
 * ddnsAccount	-   账号
 * ddnsPasswd	-   密码
 * ddnsIf	-   DDNS绑定接口
 */
typedef struct st_ddnsProfile {
    mibHead		head;
    char		ddnsProvider[UTT_DDNS_PROVIDER_LEN + 1];
    char		ddnsHost[UTT_DDNS_HOST_LEN + 1];
    char		ddnsAccount[UTT_DDNS_ACCOUNT_LEN + 1];
    char		ddnsPasswd[UTT_DDNS_PASSWD_LEN + 1];
    char		ddnsIf[UTT_INTERFACE_LEN + 1];
} DDNSProfile;

#endif
