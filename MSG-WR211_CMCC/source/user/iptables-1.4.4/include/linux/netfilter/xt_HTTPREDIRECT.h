#ifndef _XT_HTTPREDIRECT_H
#define _XT_HTTPREDIRECT_H

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

#define HTTP_R_LEN 100
struct xt_httpredirect_info {
    char redirect_dir[HTTP_R_LEN + 1];
    EX_OP_ENUM ex_op;
};



#endif /* _XT_HTTPREDIRECT_H */
