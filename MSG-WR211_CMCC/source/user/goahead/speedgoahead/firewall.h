#ifndef SPEEDWEB_FIREWALL_H
#define SPEEDWEB_FIREWALL_H

#if 0
/* IP������ */
#define IPPORT_FILTER_CHAIN	    "macipport_forward"
/* url keyword������ */
#define UK_FILTER_CHAIN    "url_keyword_forward"
#endif
/* ���ʿ��Ʋ��Թ����� */
#define ACL_FILTER_CHAIN    "acl_filter_forward"
#define ACL_FILTER_INPUT_CHAIN    "acl_filter_input"


#define FW_CMD_LEN  300
#define FW_IPRANGE_LEN  114
#define FW_DAY_LEN  64
#define FW_IP_LEN   15

extern void fw_init(void);
extern void funInitFirewall(void);
#endif
