#ifndef _XT_LAYER7_H
#define _XT_LAYER7_H

#define MAX_PATTERN_LEN 8192
#define MAX_PROTOCOL_LEN 256

struct xt_layer7_info {
    char protocol[MAX_PROTOCOL_LEN];
    char pattern[MAX_PATTERN_LEN];
    u_int8_t invert;
    u_int8_t pkt;
    /* 匹配已识别会话标识, 包括unknown */
    char classified;
};

#endif /* _XT_LAYER7_H */
