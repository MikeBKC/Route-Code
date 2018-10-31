#ifndef MIBL2TPCLIENT_SHARE_H
#define MIBL2TPCLIENT_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

#define L2TP_CONTROL_PIPE "/var/run/xl2tpd/l2tp-control"
#define L2TP_CLI_FILE_PREFIX "/etc/options.l2tp_"
/*
 * L2TP�ͻ����˺����ýṹ��
 * userName	-	�˺�
 * passwd	-	����
 * pppAuthMode  -	������֤��ʽ
 * remoteLanIpDis -	�û����õ�IP��ַ����ҳ����ʾ
 * remoteLanIp	-	Զ������IP��ַ�������ֽ���
 * remoteLanNetmask-	Զ�������������룬�����ֽ���
 * serverHost	-	�����������ַ
 * connectStatus    -   SOHO����VPN���,ҳ��㽨����ֵΪ1�����ڷ�ֹ�ظ��㽨�������쳣
 * vpnMode	-	VPNģʽ��ȫ��Ϊ0���ض�����Ϊ1������ģʽΪ2
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
