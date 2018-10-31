#ifndef MIB_PPPOESERVER_SHARE_H
#define MIB_PPPOESERVER_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"

#define ONLYPPPOE_FORWARD "onlypppoe_forward"
#define UTT_PPP_CLIENT_MAC_LEN      20
#define UTT_PPP_CLIENT_IP_LEN       20
#define UTT_PPP_CLIENT_IFNAME_LEN   20
#define UTT_PPP_CLIENT_DEVICE_LEN   20
#define UTT_PPP_CLIENT_OURNAME_LEN  20
#define UTT_PPPOE_ACCMODE_LEN       20
#define UTT_PPPOE_MAC_TYPE          8
#if (PPPOE_SELF_HELP == FYES)
#define MAX_USER_OPT_CNT	    5
#endif
#if (PPPOE_LIMIT == FYES)
#define PPPOEACC_LIMIT_CHAIN "pppoeacc_limit"
#define POE_SKIP_FIXED_LIMIT IPSET_SYS_GNAME_PRE"poe_skip_fixed_limit"
#endif

#if (PPPOE_ACCOUNT == FYES)
#define POE_DROP_CHAIN    "poe_drop_chain"   /*�����˺Ŷ�������*/
#if (NOTICE == FYES)
/*expiring account*/
#define POE_EXPIRING_ACC  IPSET_SYS_GNAME_PRE"poe_expiring_acc" /*�������˺ż���*/
#define POE_NT_OK         IPSET_SYS_GNAME_PRE"poe_nt_ok"       /*�Ѿ�ͨ������û�*/
#define POE_NT_R          IPSET_SYS_GNAME_PRE"poe_nt_r"         /*��ͨ���û���¼��http�ذ�ʱ�İ�*/
#define POE_NT_CAP_CHAIN  "poe_nt_cap_chain"  /*������ͨ���������*/
#define POE_NT_R_CHAIN    "poe_nt_r_chain"    /*������ͨ��ذ����İ�����*/

/*expired account*/
#define POE_EXPIRED_WEB IPSET_SYS_GNAME_PRE"poe_expired_web"
#define POE_NT_DROP_CHAIN "poe_nt_drop_chain" /*�����û�drop����*/
#define POE_NT_DROP_R_CHAIN "poe_nt_drop_r_chain" /*�����û�drop����*/
#endif


#define POE_EXPIRED_ACC IPSET_SYS_GNAME_PRE"poe_expired_acc"
typedef enum{
    PPPOE_ACC_NORMAL =  0,
    PPPOE_ACC_EXPIRING,
    PPPOE_ACC_EXPIRED
}emPppoeAccStatus;
#endif
typedef struct st_SessionList{
    unsigned int        pid; 
    unsigned int        STime;
    unsigned int        LTime;
    unsigned int        ConTime;/*0��ʾ���ߣ�1��ʾ����*/
    unsigned long long  rxByte;
    unsigned long long  txByte;
    char                mac[UTT_PPP_CLIENT_MAC_LEN + 1]; 
    char		ip[UTT_PPP_CLIENT_IP_LEN + 1];
    char                ifname[UTT_PPP_CLIENT_IFNAME_LEN + 1]; 
    char                device[UTT_PPP_CLIENT_DEVICE_LEN + 1]; 
    char                ourName[UTT_PPP_CLIENT_OURNAME_LEN + 1];
    short		nextIndex;/*ǰһ���Ự�洢λ���±�(��ʼ��Ϊ-1)*/
    short		accIndex;/*��profile��Ӧ���˺�����*/
} SessionList;

#if (PPPOE_LIMIT == FYES)
/*
 * ipGrpName	-   �û�������
 * count	-   ��ǰ�鱻ʹ�ô���
 * uRateLimit	-   �ϴ���������
 * dRateLimit	-   ������������
 */
typedef struct st_pppoeAccLimit {
    char    ipGrpName[IP_GROUP_NAME_LEN + 1u];
    int	    count;
    Uint32  uRateLimit;
    Uint32  dRateLimit; 
} PppoeAccLimit;
#endif

/*
 * pppoe serverȫ�����ýṹ��
 * ipStart	-   ��ʼIP��ַ
 * self_help	-   �����û���������  
 * pDns		-   ��DNS
 * sDns		-   ��DNS 
 * pppAuthMode	-   ������֤��ʽ
 * maxSessCnt	-   ϵͳ���Ự��
 * exceptIpGroup-   �����ַ��
 * limitGroup	-   pppoe�˺���������Ϣ
 */
typedef struct st_pppoeSrvGloProfile {
    mibHead		head;
    int			onlyPppoe;
    IPADDR		ipStart;
#if (PPPOE_SELF_HELP == FYES)
    FunEnableEnum	self_help;
#endif
    IPADDR		pDns;
    IPADDR		sDns;
    ProfPppAuthEnum	pppAuthMode;
    int			maxSessCnt;
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
    Uint32      ipcount;//�ɷ�������ַ����Ŀ
#endif
    char		exceptIpGroup[IP_GROUP_NAME_LEN + 1];
#if (PPPOE_LIMIT == FYES)
    PppoeAccLimit	limitGroup[MAX_PPPOE_LIMIT_IPGRP];
#endif
} PPPoESrvGloProfile;


/*
 * pppoe server�˺�����
 * pppoename	-   �˺�
 * passwd	-   ����
 * bindIp	-   �̶�IP���� 
 * accmaxconv   -   �˺����Ự��
 * AccMACBind   -   �˺Ű󶨷�ʽ
 * bindmac      -   �˺�MAC��
 * chargeType;	-   �Ʒѷ�ʽ
 * tmStart;	-   ��ʼ����
 * tmStop;	-   ��������
 * expired;	-   ��¼����״̬�����ڡ������ڡ�����
 * pid;		-   ppp���̺�
 * STime;	-   ppp������ʱ��ϵͳ����ʱ��
 * LTime;	-   ��һ�α���ʱ��ϵͳ����ʱ�䣬�ڽ����л��޸�
 * ConTime;	-   ppp�Ƿ����ӣ�Ϊ0��ʾ���ӣ���0��ʾ�Ͽ�
 * optCnt	-   �û����������޸Ĵ��� 
 * rxByte;	-   ppp����rxͳ�ƣ��ڽ���������
 * txByte;	-   txͳ�ƣ��ڽ���������
 * mac		-   pppoe�����û���mac��ַ
 * ip;		-   �����IP��ַ
 * ifname;	-   �ӿ����ƣ���ppp0��
 * device	-   ����ӿ����ƣ���br0,eth2.1��
 * ourName	-   �������ƣ�PPPoE Server��Ӧ�ķ���������pppoeSrv
 * uRateLimit	-   �ϴ���������
 * dRateLimit	-   ������������
 */
typedef struct st_pppoeSrvAccProfile {
    mibHead		head;
    char		pppoename[UTT_PPPOE_NAME_LEN+1];
    char		passwd[UTT_PPPOE_CLI_PASS_LEN+ 1];
    char                remark[UTT_REMARK_LEN + 1];
    IPADDR		bindIp;
    Uint32		uRate;
    Uint32		dRate;
    int		        accmaxconv;/*�˺����Ự����prof��ʼ��ʱ��ʼ��Ϊ1��*/
    char                AccMACBind[UTT_PPPOE_MAC_TYPE + 1];
    MacAddr             bindmac;
    MacAddr             bindmac2;
    MacAddr             bindmac3;
    MacAddr             bindmac4;
#if (PPPOE_ACCOUNT == FYES)
    ProfchargeTypeEnum  chargeType;	
    Uint32		tmStart;			
    Uint32		tmStop;		
    emPppoeAccStatus	expired;
#endif
    int actualSession;/*�˺�ʵ�ʻỰ��*/
    short sesIndex;/*���˺����һ���Ựʵ�ʴ洢λ������*/
#if (PPPOE_LIMIT == FYES)
    Uint32		uRateLimit;
    Uint32		dRateLimit;
#endif
#if 0
    unsigned int	pid; 
    unsigned int	STime;
    unsigned int	LTime;
    unsigned int	ConTime;
#if (PPPOE_SELF_HELP == FYES)
    Uint32		optCnt;
#endif
    unsigned long long	rxByte;
    unsigned long long	txByte;
    char		mac[UTT_PPP_CLIENT_MAC_LEN + 1];
    char		ip[UTT_PPP_CLIENT_IP_LEN + 1];
    char		ifname[UTT_PPP_CLIENT_IFNAME_LEN + 1];
    char		device[UTT_PPP_CLIENT_DEVICE_LEN + 1];
    char		ourName[UTT_PPP_CLIENT_OURNAME_LEN + 1];
#else
#if (PPPOE_SELF_HELP == FYES)
    Uint32		optCnt;
#endif
#endif
} PPPoESrvAccProfile;
#endif
