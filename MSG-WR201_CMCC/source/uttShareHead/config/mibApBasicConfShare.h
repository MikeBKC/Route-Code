#ifndef MIBAPBASICCONF_SHARE_H
#define MIBAPBASICCONF_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"

#define MAX_DEV_NAME_LEN          24u  /*设备名*/
#define MAX_USER_NAME_LEN         12u  /*用户名*/
#define MAX_PASSWORD_LEN          18u
typedef struct st_ap_basic_conf_profile {
    mibHead head;
//    Uint32 work_mode;    /* 工作模式 1-fit 2-fat*/
    char dev_name[MAX_DEV_NAME_LEN];	/* AP 设备名 */
    Uint32 ip_addr; /* IP地址 */
    Uint32 net_mask; /* 子网掩码 */
    //char user_name[MAX_USER_NAME_LEN];   /* AP用户名 */
    char password[MAX_PASSWORD_LEN];	/* 密码 */
//    Uint32 max_clt;  /* 最大客户端数 */
    Uint32 channel;  /* 信道 */
	Uint16            dbm;  /* 漫游阈值 */
#if defined(WIRELESS_5G) && (WIRELESS_5G==FYES)
    Uint16            dbm_5g;  /* 漫游阈值 */
#endif
 //   Uint32 effect_conf;	/* 生效配置 1-local 其余值均为AC下发*/
	Uint32 dhcp_en;
}ApBasicConfProfile;

#endif
