
/*
 * this file contain all utt conf name
 */


#ifndef _UTT_CONF_DEFINE_H
#define _UTT_CONF_DEFINE_H

//#include	<uttMachine.h>

#define SERVER_IP "202.101.35.232"
#define ASPIPMASK "255.255.255.0"
/*
 * brwang add macro
 */
#define UTT_SPEED_WEB_SEND_MSG_ACT_DEL 1
#define UTT_SPEED_WEB_SEND_MSG_ACT_ADD 2
#define UTT_SPEED_WEB_SEND_MSG_ACT_WGHT 3
#define UTT_SPEED_WEB_SEND_MSG_ACT_CHK 4

#define LAN_AC_DEL_LAN_RULE 1
#define LAN_AC_ADD_LAN_RULE 2

#if (HOTEL_PNP == FYES)
#define HOTEL_PNP_CONNMARK 0x1 
#endif

#if (TAG_VLAN == FYES)
#define TAG_VLAN_ROUTE_TABLE 149 
#define TAG_VLAN_ROUTE_RULE_PRIO    7000
#endif

#define LAN_ROUTE_TABLE_BASE 1
#define WANCHK_ROUTE_TABLE_BASE 90
#define SRCIP_ROUTE_TABLE_BASE 100
#define ISP_ROUTE_TABLE_BASE 110
#define USR_ROUTE_TABLE_BASE 120
#define USR_ROUTE_LAN_TABLE_BASE 2
#define ISP_BACKUP_ROUTE_TABLE_BASE 130

#define USR_ROUTE_LAN_RULE_PRIO_BASE 2
#define LAN_ROUTE_RULE_PRIO_BASE 1
#if (HOTEL_PNP == FYES)
#define HOTEL_PNP_ROUTE_TABLE 150
#define HOTEL_PNP_ROUTE_RULE_PRIO   8000
#endif

#define WANCHK_ROUTE_RULE_PRIO_BASE 9000
#define FWMARK_ROUTE_RULE_PRIO_BASE 9100
#define SRCIP_ROUTE_RULE_PRIO_BASE 10000
#define SRCIP_ROUTE_RULE_INTERVAL 1000
#define ISP_ROUTE_RULE_PRIO_BASE 12000
#define ISP_BACKUP_ROUTE_RULE_PRIO_BASE 12100
#define USR_ROUTE_RULE_PRIO_BASE 10200
#define UPNP_DEMO_PORT_BASE 10
#define ISP_DEFAULT_COUNT 3 /*支持运营商总数*/
#define ISP_TELECOM 10000 /*中国电信*/
#define ISP_UNICOM 10010 /*中国联通*/
#define ISP_MOBILE 10086/*中国网通*/

#define MAX_COMMAND_LEN    256
#define MAX_FILE_NAME_LEN 256
#define MAX_PPP_NAME_LEN 64U
#define DEV_PPP_IF_NAME "/var/run/%s.ppp"
#define MAX_UTT_IF_NAME_LEN 0x32


#define MAX_UTT_CONF_NAME_LEN      64
#define MAX_UTT_PASSWD_LEN      64
#define MAX_UTT_ASPIP_LEN      16
#define ISP_POLIY_FILE_SIZE     8192
#define MAX_PPTP_NUM    0x02
#define MAX_Task_NAME_LEN 64U
#define MAX_Task_TIME_LEN 15
#define MAX_Task_TYPE_LEN 03

#define DHCP_ROUTE_FILE "/var/run/dhcp_%s.route"
#define PPPOE_SERVER_USER_ORI_FILE "/etc/ppp/pppoesuer"
#define PPPOE_SERVER_USER_PAP_FILE "/etc/ppp/pap-secrets"
#define PPPOE_SERVER_USER_CHAP_FILE "/etc/ppp/chap-secrets"
#define PPPOE_SERVER_BIND_IP_FILE "/etc/ppp/pppoe-bind-ip"
#if (PPTP_IP_BIND == FYES)
#define PPTP_SERVER_BIND_IP_FILE "/etc/ppp/pptp-bind-ip"
#endif
#define PPPOE_SRV_NAME	"pppoeSrv"
#define L2TP_SRV_NAME	"UttVPNserver"
#define PPTP_SRV_NAME	"pptpd"

#define _CONF_LAN_IP                        "lan_ipaddr"
#define _CONF_LAN_NETMASK                   "lan_netmask"
#define _CONF_LAN_MAC                       "lan_macaddr"

#define _CONF_DHCP_SERVER_IP_START          "DhcpStart"
#define _CONF_DHCP_SERVER_IP_END            "DhcpEnd"
#define _CONF_DHCP_SERVER_ENABLE            "DhcpEn"
#define _CONF_DHCP_SERVER_NETMASK           "DhcpNm"
#define _CONF_DHCP_SERVER_DNSP_EN           "DhcpPEn"
#define _CONF_DHCP_SERVER_DNS1              "DhcpDns1"
#define _CONF_DHCP_SERVER_DNS2              "DhcpDns2"
#define _CONF_DHCP_SERVER_GATEWAY           "DhcpGw"
#define _CONF_DHCP_SERVER_LEASE             "DhcpLse"
#define _CONF_DHCP_SERVER_USER              "DhcpSUserList"
#define _CONF_DHCP_SERVER_ISPDNS              "DhcpIspDns"
#define _CONF_DHCP_SERVER_ISPDNS2              "DhcpIspDns2"


#define _CONF_PREFIX_WAN                    "Wan%d"
#define _CONF_WAN_ISP                       "ISP"
#define _CONF_WAN_MP_STATUS                 "Status"
#define _CONF_WAN_MP_STATUS_WEB             "StatusW"

#define _CONF_WAN_MAC                       "Mac"
#define _CONF_WAN_CONNECT_MODE              "ConnMode"
#define _CONF_WAN_STATIC_IP                 "IP"
#define _CONF_WAN_STATIC_NETMASK            "Nm"
#define _CONF_WAN_STATIC_GATEWAY            "Gw"
#define _CONF_WAN_STATIC_PRIMARY_DNS        "Dns1"
#define _CONF_WAN_STATIC_SECOND_DNS         "Dns2"

#define _CONF_WAN_PPPOE_USER                "PoeUsr"
#define _CONF_WAN_PPPOE_PASSWD              "PoePwd"
#define _CONF_WAN_PPPOE_OPMODE              "PoeOpMode"
#define _CONF_WAN_PPPOE_IDLETIME            "PoeIdle"
#define _CONF_WAN_PPPOE_AUTHTYPE            "PoeAuth"
#define _CONF_WAN_PPPOE_MTU                 "PoeMtu"
#define _CONF_WAN_PPPOE_DIALMODE            "PoeDlMode"

#define _CONF_MULTIPATH_DETECTMETHOD        "MPChkMode"
#define _CONF_MULTIPATH_IDBIND              "MPIDBind"
#define _CONF_MULTIPATH_TYPE                "MPCombMode"

#define _CONF_MULTIPATH_DETECTINTERVAL      "ChkInt"
#define _CONF_MULTIPATH_DETECTCOUNT         "ChkCnt"
#define _CONF_MULTIPATH_WEIGHT              "Weight"
#define _CONF_MULTIPATH_IPSTART             "SrcIpStart"
#define _CONF_MULTIPATH_IPEND               "SrcIpEnd"
#define _CONF_MULTIPATH_DETECTIP            "ChkIP"

#define _CONF_PPPOE_S_ENABLE                "PoeSerEn"
#define _CONF_PPPOE_S_STARTIP               "PoeSerStart"
#define _CONF_PPPOE_S_CLIENTCOUNT           "PoeSerCliCnt"
#define _CONF_PPPOE_S_PRIDNS                "PoeSerDns1"
#define _CONF_PPPOE_S_SECDNS                "PoeSerDns2"
#define _CONF_PPPOE_S_AUTHTYPE              "PoeSerAuth"
#define _CONF_PPPOE_S_ONLINECOUNT           "PoeSerOnlCnt"
#define _CONF_PPPOE_S_USER                  "PoeSerUsrs"
#define _CONF_PPPOE_S_ASPIP                 "PoeSerAspip"

#define _CONF_FAST_CONFIG_POP               "FastConfPop"

#define _CONF_Task_S_CRONLIST               "CronList"
#define _CONF_Task_S_USER		    "CronUser"
#endif

#define NOTICE_GRP_CONNMARK 0x1000
#define NOTICE_POE_CONNMARK 0x2000
#define NOTICE_SNIPER_CONNMARK 0x3000
#define SALEWIFI_MARK 0x4000

#define SYS_HTTP_PORT "sys_http_port"
#define SYS_CACHE_EXCEPTION "sys_cache_exception"
#define CACHE_EXCEPTION_DST "cache_exception_dst"
