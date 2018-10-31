

#ifndef MIBSYSTEM_SHARE_H
#define MIBSYSTEM_SHARE_H
enum em_line_group_mode {
    GROUP_LINE_BALANCE,
    GROUP_LINE_BACKUP
};
#if (WEBAUTH_ACCOUNT == FYES)
#define WEB_AUTH_FLASH_W    0x1
#endif

#if (APP_POLICY_REMOTE_UPDATE == FYES)
typedef enum{
    SOFT_UDTYPE= 0,/*�̼�����*/
    ADFILTER_UDTYPE,/*�����˲��Ը���*/
    MAX_UDTYPE 
}AppUpdateType;

typedef enum{
    APP_INITIAL,	    /*��ʼ״̬*/
    APP_CHECKING,	    /*����У��������������ڼ���Ƿ��и��£�*/
    APP_CHECK_FAILED,   /*���ʧ�ܣ�����վ����ʧ�ܵȣ�*/
    APP_STATS_UNCHECKED,/*δ��⵽����״̬(��С��3ʱ����ҳajax����Ҫ�ټ������)*/
    APP_WAITING_UPDATE, /*�⵽����,�ȴ�����*/
    APP_UPDATING,	    /*������*/
    APP_UPDATE_SUCCESS, /*���³ɹ�*/
    APP_UPDATE_FAILED   /*����ʧ��*/
}UpdateStatus;
#endif
#if (NET_SECURITY == FYES)
/* �������� */
typedef struct st_sys_net_security {
    FunEnableEnum  antiBlasterEn;
    FunEnableEnum  antiDDoSEn;
    FunEnableEnum  antiIPCheatEn;
    FunEnableEnum  antiUDPFloodEn;
    Uint32         max_udp_rxpp;
    FunEnableEnum  antiICMPFloodEn;
    Uint32         max_icmp_rxpp;
    FunEnableEnum  antiSYNFloodEn;
    Uint32         max_syn_rxpp;
    FunEnableEnum  antiPingReplyEn;
	FunEnableEnum  lanArpBroadcastEn;
	Uint32		   lanArpBroadcastInterval;
    IPADDR         accessCtrlFromIP;
    IPADDR         accessCtrlEndIP;
    FunEnableEnum  antiportScanfEn;
    Uint32         scanfCnt;
} SysNetSecurity;
#endif

#if (CONNECT_LIMIT == FYES)
typedef struct st_sys_connect_limit {
	FunEnableEnum	connLimitEn; //������������
	Uint32 totalConnCount;//��������
	Uint32 TCPConnCount;
	Uint32 UDPConnCount;
	Uint32 ICMPConnCount;
} SysConnLimit;
#endif

#if (SSH_CLI == FYES)
typedef struct st_sys_ssh {
	FunEnableEnum	SSHEn; /*����SSH������*/
	Uint32 Port;/*�˿ں�*/
} SysSSH;
#endif

#if (HTTPS == FYES)
typedef struct st_sys_https {
    FunEnableEnum httpsEn;  /*����https*/
} SysHttps;
#endif

#if (DNS_FILTER == FYES)
/*dns���˿�����*/
typedef struct st_dnsFilterControl{
    addrObjTypeEn addrTypeEn;/*��ַ��������*/
    IPADDR ipstart;
    IPADDR ipend;
#if (IP_GRP == FYES)
    char ipGrpName[UTT_INST_NAME_LEN + 1u];/*��ַ������*/
#endif
#if (TIME_GRP == FYES)
    char timeGrpName[UTT_INST_NAME_LEN + 1u];/*ʱ��������*/
#endif
    dnsFilterTypeEn filterType;/*������ֹ�б��е�����*/
}DnsFilterControl;
#endif

#if (PARENTS_CONTROL == FYES)
typedef struct st_globalParentsCtlProfile {
    FunEnableEnum  parentsEn;
    MacAddr parentMac1;
    MacAddr parentMac2;
    MacAddr parentMac3;
    MacAddr parentMac4;
    MacAddr parentMac5;
    MacAddr parentMac6;
    MacAddr parentMac7;
    MacAddr parentMac8;
    MacAddr parentMac9;
} globalParentsCtlProfile;
#endif
#if defined(CONFIG_RTL8367M) || defined(CONFIG_RTL8306_PHY) || defined(CONFIG_RALINK_MT7620) || defined(CONFIG_RTL8367RB)|| defined(CONFIG_RALINK_MT7621)
typedef struct portMirror_st{
    Uint32 monitorPort;
    Uint32 sourcePort;
}PortMirror;
#endif
#if (TAG_VLAN == FYES)
typedef struct pvid_st {
    Uint32 pvid1;
    Uint32 pvid2;
    Uint32 pvid3;
    Uint32 pvid4;
} ST_PVID;
#endif

typedef struct st_system_profile {
    mibHead head;
    Uint32  maxWanIfCount;
    int  lanPortCount;
    Uint32  wanIfCount;
    FunEnableEnum  popWelcome;
    enum em_line_group_mode lineGroupMode;
#if (ANTI_SNIFFER == FYES)
    FunEnableEnum  antiSnifferEn;
#endif
#if (NET_SECURITY == FYES)
    SysNetSecurity sysNetSe;
#endif
#if (DNS_FILTER == FYES)
    FunEnableEnum  dnsFilterEn;/*�Ƿ���*/
    DnsFilterControl dnsFilterControl;/*��ַʱ��ȿ�����Ϣ*/
#endif
#if (PORT_MIRROR == FYES)
#if defined(CONFIG_RTL8367M) || defined(CONFIG_RTL8306_PHY) || defined(CONFIG_RALINK_MT7620) || defined(CONFIG_RTL8367RB)|| defined(CONFIG_RALINK_MT7621)
    PortMirror     mirror;
#endif
    FunEnableEnum  portMirrorEn;
#endif
#if (EXCEPT_QQ == FYES)
    FunEnableEnum  exceptQQEn;
    FunEnableEnum  exceptEnterpriseQQEn;
#endif
#if (EXCEPT_MSN == FYES)
    FunEnableEnum  exceptMSNEn;
#endif
#if (EXCEPT_ALI == FYES)
    FunEnableEnum  exceptAliEn;
#endif
#if (PORT_MIRROR_6530G == FYES)
    FunEnableEnum  Mirror_6530G_En;
#endif
#if (UTTPDB == FYES)
    FunEnableEnum  UttNfIndentEn;
#endif
#if (GROUP_BASE == FYES)
    FunEnableEnum  UttNfRuleEn;
#endif
#if (CONNECT_LIMIT == FYES)
    SysConnLimit connLimit;
#endif
#if (TAG_VLAN == FYES)
    ST_PVID pvid;
#endif
#if (SSH_CLI == FYES)
    SysSSH  SSH;
#endif
#if (HTTPS == FYES)
    SysHttps sysHttps;
#endif
#if (HOTEL_PNP == FYES)
    FunEnableEnum hotelPnpEn;
#endif
#if (IDENTIFY_BIND == FYES)
    FunEnableEnum IdentifyBindEn;
#endif
#if (HARDWARE_NAT == FYES)
    FunEnableEnum hardwareNATEn;
#endif
#if (FIREWALL == FYES)
    FunEnableEnum  firewallEn;
#endif
#if (WEBAUTH_ACCOUNT == FYES)
    int flashWt;
#endif
#if (POLICY_ROUTE == FYES)
    FunEnableEnum policyrouteEn;
#endif
#if (PARENTS_CONTROL == FYES)
    globalParentsCtlProfile parentControlEn;
#endif
#if (EASY_TASK == FYES)
    int isDefaultConf;
#endif
#if (AP_LOAD_BALANCE == FYES)
    FunEnableEnum loadBalanceEn;
#endif
#if (SZONE_AUTO_UPDATA == FYES)
    FunEnableEnum szoneAutoUpdata;
#endif
#if(UTT_IPAUTOREP ==  FYES)
    FunEnableEnum  CheckIPEn;
#endif
#if (APP_POLICY_REMOTE_UPDATE == FYES)
    UpdateStatus appUpdateStatus[MAX_UDTYPE];/*Ӧ�ó������״̬��ÿ������Ԫ�ش���һ��Ӧ��*/
    int appUpdateCtl[MAX_UDTYPE];/*Ӧ�ø��¿��ƣ���¼Ӧ���Ƿ������¼�⣬�ֶ����»��Զ����µ�*/
#endif
#if(UTT_BRIDEG_MODE_FLAG == FYES)
    int brideg_mode_flag;
    char eth2_mac[32];
    int Elink_flag;
#endif
#if(APP_ELINKC == FYES)
    int elinkc_flag;
#endif
#if (WIFI_SLEEP_FLAG == FYES)
        FunEnableEnum wifisleep;    /* wifi sleep ����(ʵ�尴��)�ĺ�*/
#endif
}SystemProfile;
#endif
