#ifndef MIBL2TPSERVER_SHARE_H
#define MIBL2TPSERVER_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

/*
 * L2TPȫ�����ýṹ��
 * pppAuthMode	    -	    ������֤��ʽ
 * poolIpStart	    -	    ��ַ����ʼIP��ַ�������ֽ���
 * poolIpCnt	    -	    ��ַ����IP��ַ����ipEnd = poolIpStart + poolIpCnt,�����ֽ���
 * localIp	    -	    vpn��������IP��ַ,�����ֽ���
 * listenIp	    -	    ����ip��ַ,����lan�ڵ�ַ
 */
typedef struct st_l2tpServerProfile {
    mibHead		head;
    ProfPppAuthEnum	pppAuthMode;
    IPADDR		poolIpStart;
    UINT32		poolIpCnt;
    IPADDR		localIp;
    IPADDR		listenIp;
	IPADDR		priDns;
	IPADDR		secDns;
} L2TPSrvGlobalProfile;

/*
 * L2TP�������˺����ýṹ��
 * peerType	-	�ͻ������ͣ��ƶ��û���LAN TO LAN�û�
 * userName	-	�˺�
 * passwd	-	����
 * remoteLanIpDis	-	ҳ����ʾ��IP��ַ�������ֽ���
 * remoteLanIp	-	Զ������IP��ַ�������ֽ���
 * remoteLanNetmask-	Զ�������������룬�����ֽ���
 * pid		-	�˺Ų��Ž���
 * STime	-	�˺Ž������ʱ��ϵͳ������ʱ��uptime
 * device	-	�˺�ʹ�õ���ӿ�
 * connCnt	-	��ǰ���˺ŵ�������
 */
typedef struct st_l2tpSrvAccProfile {
    mibHead		head;
    ProfVPNPeerType	peerType;
    char		userName[33];
    char		passwd[UTT_PASSWD_LEN + 1];
    IPADDR		remoteLanIpDis;
    IPADDR		remoteLanIp;
    IPADDR		remoteLanNetmask;
#if 0
    unsigned int	pid;
    unsigned int	STime;
    char		device[UTT_INTERFACE_LEN + 1];
    int			connCnt;
#endif
} L2TPSrvAccProfile;

#endif
