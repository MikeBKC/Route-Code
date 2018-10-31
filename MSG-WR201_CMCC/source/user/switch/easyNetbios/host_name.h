#ifndef _HOST_NAME_H_
#define _HOST_NAME_H_

#define NETBIOS_FILE  "/etc/hostNames"

#define HOST_NAMT_TIMEOUT 100u 
#define NETBIOS_QUERY_TIMEOUT 3u

#define BCAST_REQ_RETRY_TIMEOUT 250u
#define BCAST_REQ_RETRY_COUNT 3u

#define REGIST_ADD 1u
#define QUERY_ADD 2u

extern void clear_host_table(void);
extern void add_host_name(uchar mac_addr[], char *host_name, uint32 ip_addr, uint32 add_type);
extern void print_host_name_table(void);
extern void  renew_host_name_table(void);
extern void renew_host_name_table_timeout(void);
void delete_host_name(uchar mac_addr[]);

#endif
