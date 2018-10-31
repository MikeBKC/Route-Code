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

/*ipsec ���ü���Կ�ļ�*/
#define AUTH_TYPE_PSK "psk"
#define IPSEC_CONF_FILE "/etc/ipsec.conf"
#define IPSEC_SECRETS_FILE "/etc/ipsec.secrets"
#define IPSEC_SA_STATUS_FILE "/var/run/SA_status.txt"
#define IPSEC_SA_AUTOUP_FILE "/var/run/SA_autoup_status.txt"
/*���ӷ�ʽ*/
typedef enum en_ipsecConnType{
    GATEWAY_TO_GATEWAY = 1,	/*���ص�����*/
    DYNAMIC_TO_GATEWAY,		/*��̬���ӵ�����*/
    GATEWAY_FROM_DYNAMIC	/*�Է���̬���ӵ�����*/
}ipsecConnType;
/*�����㷨*/
typedef enum encryptWay {
    Des = 1,	/*des*/
    Tri_Des,	/*3des*/
    Aes,	/*aes128*/
    Aes192,	/*aes192*/
    Aes256,	/*aes256*/
    Rc4		/*rc4*/
}EncryptWay;
/*��֤�㷨*/
typedef enum authWay {
    Md5 = 1,	/*md5*/
    Sha		/*sha*/
}AuthWay;
/*������*/
typedef enum groupType {
    Group1 = 1,	    /*group1 --- modp768*/
    Group2,	    /*group2 --- modp1024*/
    Group5	    /*group5 --- modp1536*/
}GroupType;

typedef struct st_ipsecconfigProfile {
    mibHead head;
    FunEnableEnum active;				    /*������ֹ*/
    ipsecConnType connType;				    /*���ӷ�ʽ*/
    char peer[MAX_DOMAIN_NAME_LENGTH+1];		    /*Զ������(����)*/
    char peerIP[MAX_IPSEC_IP_LEN+1];			    /*Զ������IP*/
    char destAddr[MAX_IPSEC_IP_LEN+1];			    /*Զ��������ַ*/
    char destMask[MAX_IPSEC_IP_LEN+1];			    /*Զ����������*/
    char profile[IPSEC_PROFILE_LEN+1];			    /*���ذ�*/
    char srcAddr[MAX_IPSEC_IP_LEN+1];			    /*����������ַ*/
    char srcMask[MAX_IPSEC_IP_LEN+1];			    /*������������*/
    Uint32 protocol;					    /*Э��*/
    Uint32 srcPort;					    /*Դ�˿�*/
    Uint32 destPort;					    /*Ŀ�Ķ˿�*/
    Uint32 lifetime_sec;				    /*�ڶ��׶�����ʱ��*/
    Uint32 lifetime_kbytes;				    /*�������*/
    FunEnableEnum anti_replay;				    /*���ز�*/
    FunEnableEnum dpd_enable;				    /*DPD*/
    Uint32 dpd_heartbeat;				    /*����*/
    FunEnableEnum nat_enable;				    /*NAT��͸*/
    Uint32 nat_port;					    /*�˿�*/
    Uint32 nat_keepalive;				    /*ά��*/
    char isakmpProfile[UTT_INST_NAME_LEN+1];		    /*isakmp ��*/
    char transformSet[MAX_TRANSFORMSETS_LENGTH+1];	    /*�ڶ��׶μ�����֤*/
    int flag;						    /*���޸�profileʱ��ʹ�ô˳�Ա*/

    Uint sa_conn;					    /*SA״̬ 0--δ���� 1--IKEЭ�� 2--IPSecЭ�� 3--�ѽ���*/
    Uint packet_out;					    /*������ܰ�����*/
    Uint packet_in;					    /*������ܰ�����*/
    char oldprofile[IPSEC_PROFILE_LEN+1];			    /*���ذ�*/
#if (VPN_CONTROL == FYES)
    Uint sa_state;					    /*sa״̬������vpn��������*/
#endif
}ipsecConfProfile;
#if (VPN_CONTROL == FYES)
typedef struct st_state {
    char name[UTT_INST_NAME_LEN+1];
    Uint sa_state;					    /*sa״̬������vpn��������*/
} saStatus;
#endif

/*�ڶ��׶μ�����֤��ʽ*/
typedef struct st_ipsectransformSet {
    mibHead head;
    char name[UTT_INST_NAME_LEN+1];
    EncryptWay encrypt;		/*�����㷨*/
    AuthWay espAuth;		/*esp��֤*/
    AuthWay auth;		/*ah��֤*/

}ipsecTransformSetProfile;
/*Э��ģʽ*/
typedef enum negotmode {
    Mainmode = 1,	    /*��ģʽ*/
    Aggresmode		    /*Ұ��ģʽ*/
}negotMode;
/*��Կ����*/
typedef enum authenmethod {
    pre_share=1,	    /*Ԥ����*/
    rsa_encr,		    /*rsa*/
    rsa_sig
}authenmethod;
/*�������*/
typedef enum ipsecuserType {
    IKE_FQDN = 1,		/*����*/
    IKE_USERFQDN,		/*Email��ַ*/
    IKE_KEYID,			/*�����ֽ���*/
    IKE_IPV4ADDR		/*IP��ַ*/
}ipsecUserType;
/**/
typedef struct st_isakmpconfProfile {
    mibHead head;
    char remoteuser[MAX_IPSEC_USERLEN+1];		/*Զ�����ID*/
    ipsecUserType remoteType;				/*Զ���������*/
    char localuser[MAX_IPSEC_USERLEN+1];		/*�������ID*/
    ipsecUserType localType;				/*�����������*/
    char preshareKey[MAX_PRESHAREKEY_LENGTH+1];		/*Ԥ������Կ*/
    negotMode mode;					/*Э��ģʽ*/
    FunEnableEnum aggresEncrypt;			/*Ұ��ģʽЭ�̼���*/
    Uint32 lifetime;					/*��һ�׶�����ʱ��*/
    char policy[MAX_ISAKMPPOLICY_LENGTH+1];		/*��һ�׶μ�����֤��ʽ*/

}isakmpConfProfile;

/*��һ�׶μ�����֤*/
typedef struct st_isakmppolicyProfile {
    mibHead head;
    EncryptWay encrypt;			/*�����㷨*/
    AuthWay hash;			/*��֤�㷨*/
    GroupType group;			/*������ group1---modp768   group2---modp1024  grou[5---modp1536*/
}isakmpPolicyProfile;
#endif
