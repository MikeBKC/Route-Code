#ifndef MIB_DDNS_SHARE_H
#define MIB_DDNS_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

/*
 * ddns���ã�Ŀǰֻ֧��һ��ddns���ã�3520G��
 * ddnsProvider	-   ddns��Ӧ��
 * ddnsHost	-   ������
 * ddnsAccount	-   �˺�
 * ddnsPasswd	-   ����
 * ddnsIf	-   DDNS�󶨽ӿ�
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
