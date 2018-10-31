#ifndef SPEEDWEB_INTERFACE_H
#define SPEEDWEB_INTERFACE_H

#define DBG_LAN
#ifdef DBG_LAN
#define LC_PRINTF(fmt, args...)		printf(fmt, ## args)
#else 
#define LC_PRINTF(fmt, args...)
#endif

extern int initLan(void);
#endif
