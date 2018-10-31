

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
    SOFT_UDTYPE= 0,/*固件更新*/
    ADFILTER_UDTYPE,/*广告过滤策略更新*/
    MAX_UDTYPE 
}AppUpdateType;

typedef enum{
    APP_INITIAL,	    /*初始状态*/
    APP_CHECKING,	    /*检测中（表明检测进程正在检测是否有更新）*/
    APP_CHECK_FAILED,   /*检测失败（如网站访问失败等）*/
    APP_STATS_UNCHECKED,/*未检测到更新状态(不小于3时，则页ajax不需要再继续检测)*/
    APP_WAITING_UPDATE, /*测到更新,等待更新*/
    APP_UPDATING,	    /*更新中*/
    APP_UPDATE_SUCCESS, /*更新成功*/
    APP_UPDATE_FAILED   /*更新失败*/
}UpdateStatus;
#endif
#if (NET_SECURITY == FYES)
/* 攻击防御 */
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
	FunEnableEnum	connLimitEn; //启用连接限制
	Uint32 totalConnCount;//总连接数
	Uint32 TCPConnCount;
	Uint32 UDPConnCount;
	Uint32 ICMPConnCount;
} SysConnLimit;
#endif

#if (SSH_CLI == FYES)
typedef struct st_sys_ssh {
	FunEnableEnum	SSHEn; /*启用SSH命令行*/
	Uint32 Port;/*端口号*/
} SysSSH;
#endif

#if (HTTPS == FYES)
typedef struct st_sys_https {
    FunEnableEnum httpsEn;  /*启用https*/
} SysHttps;
#endif

#if (DNS_FILTER == FYES)
/*dns过滤控制器*/
typedef struct st_dnsFilterControl{
    addrObjTypeEn addrTypeEn;/*地址对象类型*/
    IPADDR ipstart;
    IPADDR ipend;
#if (IP_GRP == FYES)
    char ipGrpName[UTT_INST_NAME_LEN + 1u];/*地址组名称*/
#endif
#if (TIME_GRP == FYES)
    char timeGrpName[UTT_INST_NAME_LEN + 1u];/*时间组名称*/
#endif
    dnsFilterTypeEn filterType;/*允许还禁止列表中的内容*/
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
    FunEnableEnum  dnsFilterEn;/*是否开启*/
    DnsFilterControl dnsFilterControl;/*地址时间等控制信息*/
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
    UpdateStatus appUpdateStatus[MAX_UDTYPE];/*应用程序更新状态，每个数组元素代表一个应用*/
    int appUpdateCtl[MAX_UDTYPE];/*应用更新控制，记录应用是否开启更新检测，手动更新或自动更新等*/
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
        FunEnableEnum wifisleep;    /* wifi sleep 开关(实体按键)的宏*/
#endif
}SystemProfile;
#endif
