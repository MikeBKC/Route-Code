#ifndef __WIFI_NETLINK_H__
#define __WIFI_NETLINK_H__

#define UTT_MAX_NLMSGSIZE 1024

#ifndef MAC_LEN
#define MAC_LEN    6u
#endif


typedef enum {

	UTT_NLWIFI_STA_INFO = 0, /* �û���Ϣ */


} UttNlWifiType;

typedef enum {
	UTT_NLWIFI_STA_INIT = 0, /* ��ʼ�� */

	UTT_NLWIFI_STA_ASSOC = 1, /* �û����� */

	UTT_NLWIFI_STA_DISASSOC = 2, /* �û�ȡ������ */

	UTT_NLWIFI_STA_UPPER_LIMIT =3, /* AP�������û����Ѵ����� */

} UttWifiConfType;

typedef struct st_wifiNlMsgHead {
	UttNlWifiType   nlType;
} UttNlWifiMsgHead;

/* utt wifi netlink message struct */
typedef struct st_uttNlWifiStr {
	UttNlWifiMsgHead msgHead;
	UttWifiConfType  confType;
	unsigned int     sta_num;
        unsigned int     flag; /*2.4G or 5G ...*/
	unsigned char sta_mac[MAC_LEN];

} UttNlWifiStr;

#endif
