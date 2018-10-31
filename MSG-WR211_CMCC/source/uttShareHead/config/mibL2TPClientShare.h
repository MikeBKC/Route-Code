#ifndef MIBL2TPCLIENT_SHARE_H
#define MIBL2TPCLIENT_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

#define L2TP_CONTROL_PIPE "/var/run/xl2tpd/l2tp-control"
#define L2TP_CLI_FILE_PREFIX "/etc/options.l2tp_"
/*
 * L2TP客户端账号配置结构体
 * userName	-	账号
 * passwd	-	密码
 * pppAuthMode  -	密码验证方式
 * remoteLanIpDis -	用户配置的IP地址，在页面显示
 * remoteLanIp	-	远端内网IP地址，网络字节序
 * remoteLanNetmask-	远端内网子网掩码，网络字节序
 * serverHost	-	隧道服务器地址
 * connectStatus    -   SOHO界面VPN添加,页面点建立后值为1，用于防止重复点建立导致异常
 * vpnMode	-	VPN模式，全局为0，特定网段为1，智能模式为2
 */
typedef struct st_l2tpCliProfile {
    mibHead		head;
    char		userName[33];
    char		passwd[UTT_PASSWD_LEN + 1];
    ProfPppAuthEnum	pppAuthMode;
    IPADDR		remoteLanIpDis;
    IPADDR		remoteLanIp;
    IPADDR		remoteLanNetmask;
    char		serverHost[UTT_HOST_LEN + 1];
    Uint		mtu;
    Uint		connectStatus;
    Uint		vpnMode;
} L2TPCliProfile;

#endif
