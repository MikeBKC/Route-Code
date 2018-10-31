#ifndef _XT_UTTLOG_H
#define _XT_UTTLOG_H

#define UTT_WEB_LOG_ENABLE   (1 << 0)
#define UTT_QQ_LOG_ENABLE   (1 << 1)
#define UTT_MSN_LOG_ENABLE   (1 << 2)
#define UTT_MAIL_LOG_ENABLE   (1 << 3)

/*
 * sw: which logs is enabled
 */
struct xt_uttlog_info {
    u_int32_t	sw;
	u_int32_t   port;
};
#endif /* _XT_UTTLOG_H */
