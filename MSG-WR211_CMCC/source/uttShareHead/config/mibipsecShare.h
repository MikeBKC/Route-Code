#ifndef MIB_IPSEC_SHARE_H
#define MIB_IPSEC_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

#define MAX_IPSEC_ARGLEN 255
#define MAX_DOMAIN_NAME_LENGTH 63
#define MAX_IPSEC_IP_LEN 20
#define MAX_PRESHAREKEY_LENGTH 128
#define MAX_ISAKMPPOLICY_LENGTH 128
#define MAX_TRANSFORMSETS_LENGTH 128
#define MAX_IPSEC_USERLEN 31
#define READ_MAX_LEN 1024
#define IPSEC_PROFILE_LEN 35

#define IPSEC_UNROUTED_FIND_STR "unrouted"
#define IPSEC_PROSPECTIVE_FIND_STR "prospective erouted"
#define IPSEC_EROUTED_HOLD_FIND_STR "erouted HOLD"
#define IPSEC_ISAKMP_SA_FIND_STR "ISAKMP SA established"
#define IPSEC_IPSEC_SA_FIND_STR "IPsec SA established"

/*ipsec 配置及密钥文件*/
#define AUTH_TYPE_PSK "psk"
#define IPSEC_CONF_FILE "/etc/ipsec.conf"
#define IPSEC_SECRETS_FILE "/etc/ipsec.secrets"
#define IPSEC_SA_STATUS_FILE "/var/run/SA_status.txt"
#define IPSEC_SA_AUTOUP_FILE "/var/run/SA_autoup_status.txt"
/*连接方式*/
typedef enum en_ipsecConnType{
    GATEWAY_TO_GATEWAY = 1,	/*网关到网关*/
    DYNAMIC_TO_GATEWAY,		/*动态连接到网关*/
    GATEWAY_FROM_DYNAMIC	/*对方动态连接到网关*/
}ipsecConnType;
/*加密算法*/
typedef enum encryptWay {
    Des = 1,	/*des*/
    Tri_Des,	/*3des*/
    Aes,	/*aes128*/
    Aes192,	/*aes192*/
    Aes256,	/*aes256*/
    Rc4		/*rc4*/
}EncryptWay;
/*认证算法*/
typedef enum authWay {
    Md5 = 1,	/*md5*/
    Sha		/*sha*/
}AuthWay;
/*组类型*/
typedef enum groupType {
    Group1 = 1,	    /*group1 --- modp768*/
    Group2,	    /*group2 --- modp1024*/
    Group5	    /*group5 --- modp1536*/
}GroupType;

typedef struct st_ipsecconfigProfile {
    mibHead head;
    FunEnableEnum active;				    /*允许或禁止*/
    ipsecConnType connType;				    /*连接方式*/
    char peer[MAX_DOMAIN_NAME_LENGTH+1];		    /*远端网关(域名)*/
    char peerIP[MAX_IPSEC_IP_LEN+1];			    /*远端网关IP*/
    char destAddr[MAX_IPSEC_IP_LEN+1];			    /*远端内网地址*/
    char destMask[MAX_IPSEC_IP_LEN+1];			    /*远端内网掩码*/
    char profile[IPSEC_PROFILE_LEN+1];			    /*本地绑定*/
    char srcAddr[MAX_IPSEC_IP_LEN+1];			    /*本地内网地址*/
    char srcMask[MAX_IPSEC_IP_LEN+1];			    /*本地内网掩码*/
    Uint32 protocol;					    /*协议*/
    Uint32 srcPort;					    /*源端口*/
    Uint32 destPort;					    /*目的端口*/
    Uint32 lifetime_sec;				    /*第二阶段生存时间*/
    Uint32 lifetime_kbytes;				    /*最大流量*/
    FunEnableEnum anti_replay;				    /*抗重播*/
    FunEnableEnum dpd_enable;				    /*DPD*/
    Uint32 dpd_heartbeat;				    /*心跳*/
    FunEnableEnum nat_enable;				    /*NAT穿透*/
    Uint32 nat_port;					    /*端口*/
    Uint32 nat_keepalive;				    /*维持*/
    char isakmpProfile[UTT_INST_NAME_LEN+1];		    /*isakmp 名*/
    char transformSet[MAX_TRANSFORMSETS_LENGTH+1];	    /*第二阶段加密认证*/
    int flag;						    /*当修改profile时，使用此成员*/

    Uint sa_conn;					    /*SA状态 0--未建立 1--IKE协商 2--IPSec协商 3--已建立*/
    Uint packet_out;					    /*外出加密包个数*/
    Uint packet_in;					    /*进入解密包个数*/
    char oldprofile[IPSEC_PROFILE_LEN+1];			    /*本地绑定*/
#if (VPN_CONTROL == FYES)
    Uint sa_state;					    /*sa状态，用于vpn并发控制*/
#endif
}ipsecConfProfile;
#if (VPN_CONTROL == FYES)
typedef struct st_state {
    char name[UTT_INST_NAME_LEN+1];
    Uint sa_state;					    /*sa状态，用于vpn并发控制*/
} saStatus;
#endif

/*第二阶段加密认证方式*/
typedef struct st_ipsectransformSet {
    mibHead head;
    char name[UTT_INST_NAME_LEN+1];
    EncryptWay encrypt;		/*加密算法*/
    AuthWay espAuth;		/*esp认证*/
    AuthWay auth;		/*ah认证*/

}ipsecTransformSetProfile;
/*协商模式*/
typedef enum negotmode {
    Mainmode = 1,	    /*主模式*/
    Aggresmode		    /*野蛮模式*/
}negotMode;
/*密钥类型*/
typedef enum authenmethod {
    pre_share=1,	    /*预共享*/
    rsa_encr,		    /*rsa*/
    rsa_sig
}authenmethod;
/*身份类型*/
typedef enum ipsecuserType {
    IKE_FQDN = 1,		/*域名*/
    IKE_USERFQDN,		/*Email地址*/
    IKE_KEYID,			/*特殊字节流*/
    IKE_IPV4ADDR		/*IP地址*/
}ipsecUserType;
/**/
typedef struct st_isakmpconfProfile {
    mibHead head;
    char remoteuser[MAX_IPSEC_USERLEN+1];		/*远端身份ID*/
    ipsecUserType remoteType;				/*远端身份类型*/
    char localuser[MAX_IPSEC_USERLEN+1];		/*本地身份ID*/
    ipsecUserType localType;				/*本地身份类型*/
    char preshareKey[MAX_PRESHAREKEY_LENGTH+1];		/*预共享密钥*/
    negotMode mode;					/*协商模式*/
    FunEnableEnum aggresEncrypt;			/*野蛮模式协商加密*/
    Uint32 lifetime;					/*第一阶段生存时间*/
    char policy[MAX_ISAKMPPOLICY_LENGTH+1];		/*第一阶段加密认证方式*/

}isakmpConfProfile;

/*第一阶段加密认证*/
typedef struct st_isakmppolicyProfile {
    mibHead head;
    EncryptWay encrypt;			/*加密算法*/
    AuthWay hash;			/*认证算法*/
    GroupType group;			/*组类型 group1---modp768   group2---modp1024  grou[5---modp1536*/
}isakmpPolicyProfile;
#endif
