

#ifndef MIBINTERFACE_SHARE_H
#define MIBINTERFACE_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#if (MULTI_LAN == FYES)
#define SYS_LAN_GROUP  IPSET_SYS_GNAME_PRE"lan_group"
#define SYS_LAN_RANGE_GROUP  IPSET_SYS_GNAME_PRE"lan_range_group"
#endif
#if (ARP_BINDS == FYES)
#define ARP_BIND_CHAIN  "arp_bind_input_chain"
#endif
#if (PORT_CONF_REDUCED == FYES)
#define BIT_SET(VAL,BIT,BITVAL)  ( (VAL &(~(1u <<BIT))) | (BITVAL << BIT) )
#endif
enum em_if_balance_mode {
    IF_MIAN_LINE,
    IF_BACKUP_LINE
};

enum em_eth_conn_mode {
    ETH_CONN_DHCP,
    ETH_CONN_STATIC,
    ETH_CONN_PPPOE
};

struct st_ethernet_static {
    IPADDR ip;
    IPADDR nm;
#if (MULTI_LAN == FYES)
    IPADDR ip2;
    IPADDR nm2;
    IPADDR ip3;
    IPADDR nm3;
    IPADDR ip4;
    IPADDR nm4;
#endif
    IPADDR gw;
    IPADDR pd;
    IPADDR sd;
};

enum em_ppp_conn_mode {
    PPP_CONN_KEEPLIVE,
    PPP_CONN_DEMAND,
    PPP_CONN_MANUAL
};

enum em_ppp_sp_dail {
    PPP_DAIL_NORMAL,
    PPP_DAIL_SP1,
    PPP_DAIL_SP2,
    PPP_DAIL_SP3
};

struct st_ethernet_pppoe {
    Uchar user[UTT_PPPOE_CLI_NAME_LEN+1];
    Uchar passwd[UTT_PPPOE_CLI_PASS_LEN+1];
    enum em_ppp_conn_mode pppConnMode;
    enum em_ppp_sp_dail spDial;
    enum en_profPppAuthMode authType;
    Uint idleTime;
    Uint mtu;
};

#if (WIRELESS == FYES)
enum em_3g_isp_mdde {
    ISP_CMC,
    ISP_CUC,
    ISP_CDC
};

enum em_3g_auth_type {
    AUTH_SIM,
    AUTH_PASSAUTH
};

struct st_ethernet_3g {
    Uchar wan_3g_dev[UTT_3G_DEV_LEN+1];
    enum em_3g_isp_mdde wan_3g_isp;
    enum em_3g_auth_type wan_3g_authtype;
    Uchar wan_3g_pincode[UTT_3G_LEN+1];
    Uchar wan_3g_authname[UTT_3G_LEN+1];
    Uchar wan_3g_authpasswd[UTT_3G_LEN+1];
    Uchar wan_3g_connname[UTT_3G_LEN+1];
    Uchar wan_3g_dialnum[UTT_3G_LEN+1];
    Uchar wan_3g_pppname[UTT_3G_LEN+1];
    Uchar wan_3g_ppppasswd[UTT_3G_LEN+1];
};
#endif

#if (PORT_CONF_REDUCED == FYES)
/*
 * 端口速率存储模式到设置模式的转变
 */
typedef struct st_PortSpdSetSt{
    int autoneg;    /*自动协商*/
    int speed;	    /*速率*/
    int fdx;	    /*full duplex */
} PortSpdSetSt;
#endif

struct st_ethernet_if {
    enum em_eth_conn_mode connMode;
    struct st_ethernet_static sta;
    struct st_ethernet_pppoe pppoe;
#if (WIRELESS == FYES)
    struct st_ethernet_3g wan3g;
#endif
    MacAddr    mac;
#if (PORT_CONF_REDUCED == FYES)
    SwPortConfMode       speedMode;/*端口模式*/
#endif
};

typedef struct st_interface {
    mibHead head;
    struct st_ethernet_if   ether;
#if (SMART_QOS_FUN == FYES)
    FunEnableEnum   smartQosEn;
#endif
#if (ANTI_SNIFFER == FYES)
    FunEnableEnum   antiSnifEn;
#endif
#if (ARP_BINDS == FYES)
    FunEnableEnum   arpAllowOtherEn;
#endif
    Uint32          txRate;
    Uint32          rxRate;
    Uint32          isp;
    Uint32          isp1;
    Uint32          isp2;
    Uint32          detectInterval;
    Uint32          detectCount;
    IPADDR          detectIp;
#if 0
    IPADDR          srcRouteIpS;
    IPADDR          srcRouteIpE;
#endif
    enum em_if_balance_mode lineBalMode;
    enum em_if_balance_mode runningLineMode;
    Uchar           ifName[UTT_INST_NAME_LEN+1];
#if (NAT_GLOBAL_ENABLE == FYES)
    int NatActive;
#endif
#if (UPNP_GLOBAL_ENABLE == FYES)
    int UpnpActive;
#endif
   FunEnableEnum   BindWanEn;
   MacAddr    GateWayMac; 
}InterfaceProfile;

#endif
