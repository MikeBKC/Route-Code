#ifndef MIB_PPTPCLIENT_SHARE_H
#define MIB_PPTPCLIENT_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

#if (PPTP_ENCRYPT_MODE == FYES)
#define  ENCRYPT_MODE_LEN  4
#endif
/*
 * PPTP�ͻ����˺����ýṹ��
 * userName	-	�˺�
 * passwd	-	����
 * pppAuthMode  -	������֤��ʽ
 * EncryptionMode   -   ��Ϣ���ܷ�ʽ
 * remoteLanIpDis -	�û����õ�IP��ַ����ҳ����ʾ
 * remoteLanIp	-	Զ������IP��ַ�������ֽ���
 * remoteLanNetmask-	Զ�������������룬�����ֽ���
 * serverHost	-	�����������ַ
 * connectStatus    -   Ĭ��Ϊ0��ҳ��㽨����ֵΪ1�����ڷ�ֹ�ظ��㽨�������쳣
 * vpnMode	-	VPNģʽ��ȫ��Ϊ0���ض�����Ϊ1������ģʽΪ2
 */
typedef struct st_pptpCliProfile {
    mibHead		head;
    int                 Natenable;
    char		userName[33]; //1.7.4 �޸�vpn����Ϊ32λ
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
