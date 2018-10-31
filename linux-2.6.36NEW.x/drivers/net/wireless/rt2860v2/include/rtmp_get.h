#ifndef __RTMP_GET_H__
#define __RTMP_GET_H__


#ifdef CONFIG_RT_IOCTL_GET_SUPPORT
#define RT_IOCTL_CONFIG_GET
#define RT_IOCTL_CHANNEL_GET
#define RT_IOCTL_TXPOWER_GET
#ifdef BYPASS_SUPPORT
#define RT_IOCTL_BYPASS_GET
#endif
#ifdef UTT_MAXSTANUM
#define RT_IOCTL_UTTMAXSTA_GET
#endif
#endif


#endif
