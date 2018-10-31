#ifndef _XT_HTTPREDIRECT_H
#define _XT_HTTPREDIRECT_H

/*
 * HTTPREDIRECT�쳣����ʽ
 * XT_C:  �����쳣����XT_CONTINUE
 * XT_R:  �����쳣����XT_RETURN
 * XT_D:  �����쳣����NF_DROP
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
