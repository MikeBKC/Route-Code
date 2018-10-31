#ifndef UTT_NF_NETLINK_H
#define UTT_NF_NETLINK_H

#ifdef CONFIG_UTT_NETFILTER
extern int uttNlSkBind(Uint32 pid, Uint32 grp);
extern void uttNfNlHashDelIpStr(int sock_fd, char * ipstr);
extern void uttNfNlHashDelIp(int sock_fd, unsigned long ipaddr);
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
extern void uttNfNlHashEditTrigger(int sock_fd, unsigned long ipaddr);
#endif
extern int uttNfNlSend(int sock_fd, char *data,Uint32 dataLen, Uint32 dpid, Uint32 dgrp);
extern int uttNfNlSkBind(Uint32 pid, Uint32 grp);
#endif

#endif
