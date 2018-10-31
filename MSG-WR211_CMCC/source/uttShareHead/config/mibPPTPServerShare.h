#ifndef MIBPPTPSERVER_SHARE_H
#define MIBPPTPSERVER_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
 
#if (PPTP_ENCRYPT_MODE == FYES)
#define  ENCRYPT_MODE_LEN  4
#endif
/*
 * PPTPȫ�����ýṹ��
 * pppAuthMode	    -	    ������֤��ʽ
 * poolIpStart	    -	    ��ַ����ʼIP��ַ�������ֽ���
 * poolIpCnt	    -	    ��ַ����IP��ַ����ipEnd = poolIpStart + poolIpCnt,�����ֽ���
 * localIp	    -	    vpn��������IP��ַ,�����ֽ���
 * listenIp	    -	    �����˿�
 * EncryptionMode   -       ��Ϣ���ܷ�ʽ
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
 * PPTP�������˺����ýṹ��
 * peerType	-	�ͻ������ͣ��ƶ��û���LAN TO LAN�û�
 * userName	-	�˺�
 * passwd	-	����
 * bindIp	-	�̶�ip
 * remoteLanIpDis	-	ҳ����ʾ��IP��ַ�������ֽ���
 * remoteLanIp	-	Զ������IP��ַ�������ֽ���
 * remoteLanNetmask-	Զ�������������룬�����ֽ���
 * pid		-	�˺Ų��Ž���
 * STime	-	�˺Ž������ʱ��ϵͳ������ʱ��uptime
 * device	-	�˺�ʹ�õ���ӿ�
 * connCnt	-	��ǰ���˺ŵ�������
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
