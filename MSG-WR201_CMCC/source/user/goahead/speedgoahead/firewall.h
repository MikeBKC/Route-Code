#ifndef SPEEDWEB_FIREWALL_H
#define SPEEDWEB_FIREWALL_H

#if 0
/* IP过滤链 */
#define IPPORT_FILTER_CHAIN	    "macipport_forward"
/* url keyword过滤链 */
#define UK_FILTER_CHAIN    "url_keyword_forward"
#endif
/* 访问控制策略过滤链 */
#define ACL_FILTER_CHAIN    "acl_filter_forward"
#define ACL_FILTER_INPUT_CHAIN    "acl_filter_input"


#define FW_CMD_LEN  300
#define FW_IPRANGE_LEN  114
#define FW_DAY_LEN  64
#define FW_IP_LEN   15

extern void fw_init(void);
extern void funInitFirewall(void);
#endif
