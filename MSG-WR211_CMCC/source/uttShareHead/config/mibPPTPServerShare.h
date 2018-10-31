#ifndef MIBPPTPSERVER_SHARE_H
#define MIBPPTPSERVER_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
 
#if (PPTP_ENCRYPT_MODE == FYES)
#define  ENCRYPT_MODE_LEN  4
#endif
/*
 * PPTP全局配置结构体
 * pppAuthMode	    -	    密码验证方式
 * poolIpStart	    -	    地址池起始IP地址，网络字节序
 * poolIpCnt	    -	    地址池总IP地址数，ipEnd = poolIpStart + poolIpCnt,主机字节序
 * localIp	    -	    vpn本地虚拟IP地址,网络字节序
 * listenIp	    -	    监听端口
 * EncryptionMode   -       消息加密方式
 */
typedef struct st_pptpServerProfile {
    mibHead		head;
    ProfPppAuthEnum	pppAuthMode;
    IPADDR		poolIpStart;
    UINT32		poolIpCnt;
    IPADDR		localIp;
    IPADDR		listenIp;
	IPADDR		priDns;
	IPADDR		secDns;
#if (PPTP_ENCRYPT_MODE == FYES)
    char                EncryptionMode[ENCRYPT_MODE_LEN + 1];
#endif
    Uint		mtu;
} PPTPSrvGlobalProfile;

/*
 * PPTP服务器账号配置结构体
 * peerType	-	客户端类型：移动用户，LAN TO LAN用户
 * userName	-	账号
 * passwd	-	密码
 * bindIp	-	固定ip
 * remoteLanIpDis	-	页面显示的IP地址，网络字节序
 * remoteLanIp	-	远端内网IP地址，网络字节序
 * remoteLanNetmask-	远端内网子网掩码，网络字节序
 * pid		-	账号拨号进程
 * STime	-	账号建立隧道时，系统启动的时间uptime
 * device	-	账号使用的虚接口
 * connCnt	-	当前此账号的连接数
 */
typedef struct st_pptpSrvAccProfile {
    mibHead		head;
    ProfVPNPeerType	peerType;
    char		userName[33];
    char		passwd[UTT_PASSWD_LEN + 1];
#if (PPTP_IP_BIND == FYES)
    IPADDR		bindIp;
#endif
#if(HARD_FEATURE == FYES)
    MacAddr hardF;
#endif
    IPADDR		remoteLanIpDis;
    IPADDR		remoteLanIp;
    IPADDR		remoteLanNetmask;
#if 0
    unsigned int	pid;
    unsigned int	STime;
    char		device[UTT_INTERFACE_LEN + 1];
    int			connCnt;
#endif
} PPTPSrvAccProfile;

#endif
