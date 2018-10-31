#ifndef _XT_HTTPPROXY_H
#define _XT_HTTPPROXY_H

#define HTTP_PROXY_DIR_LEN 35
#define HTTP_PROXY_FILE_LEN 27
struct xt_httpproxy_info {
    char proxy_dir[HTTP_PROXY_DIR_LEN + 1];
    char proxy_file[HTTP_PROXY_FILE_LEN + 1];
};

#endif /* _XT_HTTPPROXY_H */
