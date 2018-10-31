
#ifndef __UTT_MACHINE_H
#define __UTT_MACHINE_H

#include<linux/autoconf.h>

#define FYES 1
#define FNO  0

#define UTT_NV_A651W FYES
/*���ù���*/
#define SETTINGS_CONF FYES
/*
 * system config
 */
#define USER_DEF_WAN_COUNT FNO

/*Ʒʤ�����ԣ���ר��LOGO��*/
#define OEM_PISEN FYES

/* ��¼���� */
#define UTT_LOGIN_LIMIT FYES

#define MAKE_MONEY FNO

#if(MAKE_MONEY == FYES)
#define MAKE_MONEY_DOMAIN "biz.greewifi.com.cn"
#define WIFIDOG_MAKE_MONEY FNO
#define WIFIDOG_USER_SECRET_KEY FNO /* "��֤��Կ�޸ġ�*/
#endif

/*tr069�ͻ���*/
#define CWMP FNO
#if (CWMP == FYES)
#define WECHAT_CWMP FNO
#endif 

/*�·��UI,��������ʹ�þ�UI*/
#define UTT_SMART_UI FYES

#if (UTT_SMART_UI == FYES)
/*�·��UI �����м�*/
#define SMART_WLAN_REPEATER FNO

/*�·��UI �ƹ���*/
#define SMART_CLOUD_SHARE FNO

/*�·��UI ��ȫ����*/
#define SMART_SECURITY_CONF FNO
#endif

#define ANTI_SNIFFER FNO
#define SMART_QOS_FUN FYES
#define GAME_BOOST FYES

/* ��ȫ���� */
#define NET_SECURITY FYES
#define DEF_LAN_ARP_INTERVAL 100 

#define MAX_WAN_IF_COUNT 1
#define MAX_LAN_PORT_COUNT 4

#define MAX_INTERFACE_PROFILES (MAX_WAN_IF_COUNT+1)

/*
 * NAT Active global Enable
 */
#define NAT_GLOBAL_ENABLE FYES
/*
 *  UPNP Active global Enable
 */
#define UPNP_GLOBAL_ENABLE FYES

/*
 * service group, ip group
 */
#define OBJECT_GROUP FYES
#define MAX_IP_GROUP_PROFILES 20
#define MAX_PORT_GROUP_PROFILES 20

#define ARP_BINDS  FYES
/*
 * user ctl
 */
#define MAX_USER_PROFILES 256
/*
 * dhcp server
 */
#define DHCP_SERVER FYES
#define MAX_DHCP_SERVER_PROFILES 1
#if (DHCP_SERVER == FYES)
#define DHCP_AUTO_BIND FNO
#endif

#ifdef CONFIG_UTT_NETFILTER
/*
 *pdb 
 *  
 */
#define UTTPDB FYES
#define MAX_SYS_RES_PDB          5
#define MAX_PDB_PROFILES	 256

/*��Ϊ���*/
#define LOG_MANAGE FNO
#define MAX_LOG_MANAGE_PROFILES 1
#endif

#define CLOUD_MANAGE  FYES
#define CLOUD_CONFIG  FYES

#if (UTTPDB == FYES)
/*user status*/
#define USER_STATUS FYES

/*
 * group base
 */
#define GROUP_BASE FYES
#define MACADDR_ADD_LIMIT FYES
#define MAX_FINE_LIMIT_PROFILES 10u/*��ϸ�����������Ŀ*/
#define MAX_ACT_CONTROL_PROFILES 20u/*��Ϊ���������Ŀ*/
#define MAX_GROUP_BASE_PROFILES  MAX_FINE_LIMIT_PROFILES + MAX_ACT_CONTROL_PROFILES
#define UTT_MAX_GROUP_PDBPOLICY    254 
#define UTT_MAX_PDB_POLICY  32


#if (GROUP_BASE == FYES)
/* except qq
 */
#define EXCEPT_QQ FNO
#define MAX_EXCEPT_QQ_PROFILES 256

/*
 * except msn
 */
#define EXCEPT_MSN FNO
#define MAX_EXCEPT_MSN_PROFILES 256
#endif

#endif

/*
 * static nat 
 *  by yqyang
 */
#define	STATIC_NAT FYES
#define MAX_STATIC_NAT_PROFILES	10

/*
 * nat rule
 *
 */
#define NAT_RULE FYES
#define MAX_NAT_RULE_PROFILES    10
#define MAX_NAT_BIND_IP 20
/*
 *DMZ
 */
#define DMZ FYES
#define MAX_DMZ_NAT_PROFILES 1

/*
 * firewall
 */
#define FIREWALL  FNO
//#define FW_DBG
#define MAX_FIREWALL_FILTER_PROFILES 64

/*
 * l2tp server
 */
#define L2TP_SERVER			FNO
#define MAX_L2TP_SRV_GLOBAL_PROFILES	1
#define MAX_L2TP_SRV_ACCOUNT_PROFILES   1

/*
 * l2tp client
 */
#define L2TP_CLIENT                     FNO
#define MAX_L2TP_CLIENT_PROFILES        1

/*
 * pptp server
 */
#define PPTP_SERVER			FNO
#define MAX_PPTP_SRV_GLOBAL_PROFILES	1
#define MAX_PPTP_SRV_ACCOUNT_PROFILES   1

/*
 * pptp client
 */
#define PPTP_CLIENT			FNO
//#define PPTP_DBG
#define MAX_PPTP_CLI_PROFILES		1

#if (UTT_SMART_UI == FYES)
#define VPN_PROXY                       FNO
#define VPN_CONTROL                     FNO/*VPN��������*/
#define MAX_VPN_SESSION_NUM             1/*VPN������Ŀ��*/
#endif

/*
 * route
 */
#define STATIC_ROUTE			FYES
#define MAX_STATIC_ROUTE_PROFILES	10

#define HOTEL_PNP                       FNO
/*
 * DDNS
 */
#define DDNS				FNO
//#define DDNS_DBG
#define MAX_DDNS_PROFILES		1     /* Ŀǰֻ֧��1��DDNS����, 3520G */
#define DDNS_UTTCARE                    FNO

/*
 * PPPoE Server
 */
#define PPPOE_SERVER			FNO
//#define PPPOE_SRV_DBG
#define MAX_PPPOE_SRV_GLO_PROFILES	1
#define MAX_PPPOE_SRV_ACC_PROFILES	512
#define MAX_PPPOE_SRV_SESSION_PROFILES  100

#if (PPPOE_SERVER == FYES)
/*
 * PPPoE selfhelp
 */
#define PPPOE_SELF_HELP			FYES
/*
 * PPPoE�Ʒ�
 */
#define PPPOE_ACCOUNT FNO

#define PPPOE_LIMIT FNO
#define MAX_PPPOE_LIMIT_IPGRP   10

#endif


/*
 * administrator configuration
 */
#define ADMIN_CONF FYES
#define MAX_ADMIN_CONF_PROFILES 1

/*
 * dns Filter
 */
#define DNS_FILTER FYES
#define MAX_DNS_FILTER_PROFILES 64


/*
 * wireless
 */
#define WIRELESS FYES
#define WIRELESS_5G FNO
#define MAX_WIRELESS_PROFILES 2


#if (WIRELESS == FYES)
#if (UTT_SMART_UI == FYES)
/*���߶�ssid*/
#define WLAN_MULTI_SSID FYES
#endif
#endif

/*
 * wire Base Config
 *
 */
#define WIRELESS_BASE_CONFIG FYES
#define MAX_WIRELESS_BASE_CONFIG_PROFILES 2

/* ������Чʱ�� */
#define WLAN_WORK_TIME FYES

/*
 * wireless Mac Filter
 */
#define WIRELESS_MAC_FILTER FYES
#define MAX_WIRELESS_MAC_FILTER_GLOBAL_PROFILES 2
#define MAX_WIRELESS_MAC_FILTER_PROFILES 50
#define MAX_WIRELESS_5G_MAC_FILTER_PROFILES 50

#ifdef CONFIG_SWITCH_EXT

/*
 * wireless Security
 */
#define WIRE_SECURITY_DBG 
#define WIRELESS_SECURITY_CONFIG FYES

/*
 * WlanAdvanced
 */
#define WIRE_ADVANCED_DBG
#define WIRELESS_ADVANCED_CONFIG FYES


/*sw port conf*/
#define FEATURE_SW_PORT_CONF  FYES
#define MAX_SW_PORT_CONF_PROFILES 24

/*
 *port VLAN
 */
#define MAX_PORT_VLAN_PROFILES  24
#define FEATURE_PORT_VLAN  FYES

/* mirror */
#define FEATURE_SW_PORT_MIRROR FYES
#define MAX_MIRROR_PROFILES 1

/* trunk */
#define FEATURE_SW_PORT_TRUNK	FYES
#ifdef CONFIG_IP1725
#define MAX_TRUNK_PROFILES	2
#else
#ifdef CONFIG_VSC7390
#define MAX_TRUNK_PROFILES	24

#endif
#endif

/*��ͬ�ͺŵĲ��컯*/
#ifdef CONFIG_IP1725

#define FEATURE_ACL             FYES
#define MAX_ACL_PROFILES        16 

#else

#define FEATURE_ACL             FNO

#endif

#else   /*CONFIG_SWITCH_EXT*/

#define FEATURE_SW_PORT_CONF FNO 
#define FEATURE_PORT_VLAN  FNO
#define FEATURE_SW_PORT_MIRROR FNO
#define FEATURE_SW_PORT_TRUNK FNO
#define FEATURE_ACL             FNO

#endif  /*CONFIG_SWITCH_EXT*/

#define TASK_SCHEDULER FYES
#define MAX_TASK_SCHED_PROFILES	5

/*ntp*/
#define FEATURE_NTP  FYES

#define FEATURE_LANG  FYES

#define FEATURE_PORT_STATISTICS  FYES

/* wireless */
#define WIRELESS FYES
#define WIRELESS_MAC_FILTER FYES
#define MAX_WIRELESS_MAC_FILTER_PROFILES 50

/*
 * notice
 */
#define NOTICE FNO
#define MAX_NOTICE_PROFILES 3

/*
 * ��ҳ��֤
 */
#define WEB_AUTH FNO
#define MAX_WEB_AUTH_PROFILES 80
#define MAX_WEB_AUTH_GLOBAL_PROFILES 1

#if (WEB_AUTH == FYES)
/*
 * WebAuth selfhelp
 */
#define WEBAUTH_SELF_HELP	FYES

/*web��֤�Ʒ�*/
#define WEBAUTH_ACCOUNT	FNO
#define WEBAUTH_ADVANCE FNO
#endif

/* NAT���Ự�� */
#define MAX_NAT_SESSIONS 30000

#define POLICY_ROUTE FNO
#define MAX_POLICY_ROUTE_PROFILES 10
/*
 * ip group
 */
#define IP_GRP FYES
#define MAX_IPGRP_PROFILES 64

/**
 * time group
 * */
#define TIME_GRP FYES
#define MAX_TIMEGRP_PROFILES 10

/*
 * connlimit
 */
#define CONNECT_LIMIT FYES
#define DEF_TOTAL_CONN_COUNT    1500
#define DEF_TCP_CONN_COUNT  1000
#define DEF_UDP_CONN_COUNT  800
#define DEF_ICMP_CONN_COUNT 100

/*��LAN��*/
#define MULTI_LAN FYES

/*
 * �򻯰�˿�����
 * ���ڷǸ��ӽ�������(24�ڽ���оƬΪ���ӹ���)
 * �˿ڿ���
 */
#define PORT_CONF_REDUCED FNO
#if (PORT_CONF_REDUCED == FYES)
#ifdef CONFIG_RALINK_RT3052
#define MAX_SPDMODE_RDD 100	/*����˿ڿ����������ģʽ*/
#elif  defined(CONFIG_RALINK_MT7628)
#define MAX_SPDMODE_RDD 100	/*����˿ڿ����������ģʽ*/
#elif  defined(CONFIG_RTL8306_PHY)
#define MAX_SPDMODE_RDD 100	/*����˿ڿ����������ģʽ*/
#elif defined(CONFIG_IT8712_AR8316)
#define MAX_SPDMODE_RDD 1000	/*����˿ڿ����������ģʽ*/
#endif
#endif

#ifndef CONFIG_SWITCH_EXT/*���ӽ�������*/
#define PVLAN_REDUCED   FNO/*�����port vlan*/
#endif

/*pptp�̶�ip*/
#if (PPTP_SERVER == FYES)
#define PPTP_IP_BIND FYES
#endif

#define PORT_MIRROR FNO
#define UTT_NV_1800N FNO

#define IP_SEC FNO
#define MAX_IPSEC_PROFILES     10 
#define MAX_TRANSFORMSET_PROFILES  20 
#define MAX_ISAKMP_PROFILES         10
#define MAX_ISAKMP_POLICY_PROFILES      20

#define P2P_LIMIT FNO
/*
 *��̬·�ɰ���ӿ�
 */
#if((PPTP_SERVER == FYES) && (PPTP_CLIENT == FYES) && (STATIC_ROUTE == FYES))
#define STATIC_ROUTE_BIND_PPTP FYES
#endif

/*
 *Syslog
 */
#define SYSLOG FNO

/*pptp ���ܷ�ʽ*/
#if (PPTP_SERVER == FYES)
#define PPTP_ENCRYPT_MODE FYES
#endif

/*SSH�����е�½*/
#define SSH_CLI FNO
#define SSH_PORT 22

#define UTT_LICENSE FNO
#define MAX_LICENSE_PROFILES 2

/*��ݰ�*/
#define IDENTIFY_BIND FNO

/*https���ʿ���*/
#define HTTPS FNO

/*�׹���*/
#define EASY_TASK FYES

/*�ҳ�����*/
#define PARENTS_CONTROL FNO
#define MAX_PARENTS_CONTROL_PROFILES 10

/*ϵͳ��־��Ϣ*/
#define SYS_LOG_INFO FNO

/*MAC��ַ����*/
#define MAC_FILTER FYES
#define MAX_MAC_FILTER_GLOBAL_PROFILES 1
#define MAX_MAC_FILTER_PROFILES 20


/*��ϼ����ƺ�*/
#define DIAGNOSE_DETECT FYES

/*Ӧ�ó���˵�*/
#define APP_MENU FYES
#define MAX_SHORTCUT_MENU_PROFILES 1

/*cJSON�⿪��*/
#define CJSON FYES

/*�Զ����¼ҳ����ƺ�*/
#define LOGIN_PAGE_DIY FNO

/*���繲��*/
#define NET_SHARE_MANAGE FNO
#define MAX_STORAGE_NUM 4
#define FTP_SERVER FNO
#define MAX_FTP_SHARE_DIR 1
#define NET_SHARE_USER FNO
#define MAX_NET_SHARE_USER 4

/*udp�˿ڷǷ����Ӷ���*/
#define UDP_PORT_SCAN_DEFENSE FYES

/*�����������û�ͳ��*/
#define SHOW_ALL_WIRE_MAC FYES
#if 0
#define DDNS_NO_IP FYES 
#define DDNS_DYNDNS FYES
#endif

#define ADFILTER FNO
#define MAX_ADFILTER_PROFILES 1

#define MAC_FILTER_JION_WIFI FYES
#if 0
#define DDNS_NO_IP FYES 
#define DDNS_DYNDNS FYES 
#endif

#define DDNS_IP_LINK_HIDE FNO

//��������
#define  UTT_DOMAINACCESS  FNO


/*
 *    29 * Զ��wifidog��֤
 *       30 * */
#define UTT_KNOWIFI FNO
#if (UTT_KNOWIFI == FYES)
#define MAX_KNOWIFI_DOMAIN_COUNT 30
#endif

#define WIFIDOG_AUTH FNO
#if(WIFIDOG_AUTH == FYES)
#define WHITELIST 1 /*Զ����֤��������������*/
#define MAX_WHITE_LIST 20  /*Զ����֤�����������б���Ŀ��*/
#define WIFIDOG_NOTICE FNO /*����ͨ��*/
#define WIFI_RATE_LIMIT FNO /*�������*/
#define WEB_AUTH_STALE FNO
#endif

#define UTT_LOGIN_LIMIT FYES
#define APP_POLICY_REMOTE_UPDATE FNO
//IP��ͻ�Զ��޸�
#define UTT_IPAUTOREP FNO
#include "uttMachineAllMacro.h"
#endif
//����5G�ŵ�ѡ��149-165
#define UTT_5G_CHANNEL_149_TO_165 FNO
//A651W 5G�ŵ����52��56��60��64
#define UTT_5G_CHANNEL_52_TO_64 FYES
#define RAISECOM_A30W FNO
#define BRAND_INDIA FNO
#define RANDOM_PASS FNO
/*chinatelecom macro*/
#define APP_ELINKC FNO
#define MIDDLE_SOFT FYES
#define AUTO_CHANNEL FYES
#define UTT_HAS_WPS_BUTTON FNO
#define UTT_BRIDEG_MODE_FLAG FYES
#define LAN_DEFAULT_IPADDR FNO
#define CTRL_SERIALNUM FYES
#define WIFI_SLEEP_FLAG FYES
#define WRITE_NEWMAC FYES
#define IP_3_254 FYES
#define RAISECOM_MATCHINE FYES
#define RAISECOM_CMCC FYES
#define RAISECOM_211 FYES
