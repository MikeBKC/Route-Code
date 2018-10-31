#ifndef MIB_PPTPCLIENT_SHARE_H
#define MIB_PPTPCLIENT_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

#if (PPTP_ENCRYPT_MODE == FYES)
#define  ENCRYPT_MODE_LEN  4
#endif
/*
 * PPTP客户端账号配置结构体
 * userName	-	账号
 * passwd	-	密码
 * pppAuthMode  -	密码验证方式
 * EncryptionMode   -   消息加密方式
 * remoteLanIpDis -	用户配置的IP地址，在页面显示
 * remoteLanIp	-	远端内网IP地址，网络字节序
 * remoteLanNetmask-	远端内网子网掩码，网络字节序
 * serverHost	-	隧道服务器地址
 * connectStatus    -   默认为0，页面点建立后值为1，用于防止重复点建立导致异常
 * vpnMode	-	VPN模式，全局为0、特定网段为1、智能模式为2
 */
typedef struct st_pptpCliProfile {
    mibHead		head;
    int                 Natenable;
    char		userName[33]; //1.7.4 修改vpn长度为32位
    char		passwd[UTT_PASSWD_LEN + 1];
    ProfPppAuthEnum	pppAuthMode;
#if (PPTP_ENCRYPT_MODE == FYES)
    char                EncryptionMode[ENCRYPT_MODE_LEN + 1];
#endif
    IPADDR		remoteLanIpDis;
    IPADDR		remoteLanIp;
    IPADDR		remoteLanNetmask;
    char		serverHost[UTT_HOST_LEN + 1];
    Uint		mtu;
    Uint                connectStatus;
    Uint		vpnMode;
} PPTPCliProfile;

#endif
