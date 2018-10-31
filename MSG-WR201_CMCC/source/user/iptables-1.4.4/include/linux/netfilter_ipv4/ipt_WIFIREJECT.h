#ifndef _IPT_WIFIREJECT_H
#define _IPT_WIFIREJECT_H
#if 0
/*
 * HTTPREDIRECT异常处方式
 * XT_C:  遇到异常返回XT_CONTINUE
 * XT_R:  遇到异常返回XT_RETURN
 * XT_D:  遇到异常返回NF_DROP
 */
typedef enum EX_OP_ENUM {
    XT_NULL = 0,
    XT_C,
    XT_R,
    XT_D,
} EX_OP_ENUM;
#endif

#define HTTP_R_LEN 20
#define HTTP_SN_LEN 8
#define HTTP_DOMAIN_LEN 50
#define HTTP_LOGIN_URL_LEN 20
#define HTTP_SECRET_KEY_LEN 20
#define SERVER_VERSION_LEN 5
struct ipt_wifireject_info {
    char redirect_dir[HTTP_R_LEN + 1];
    char redirect_val[HTTP_SN_LEN +1 ];
    char auth_domain[HTTP_DOMAIN_LEN +1 ];
    char login_url[HTTP_LOGIN_URL_LEN +1 ];
    char secret_key[HTTP_SECRET_KEY_LEN +1 ];
    char server_version[SERVER_VERSION_LEN +1 ];
    //EX_OP_ENUM ex_op;
};

#endif /*_IPT_REJECT_H*/
