/*
 ***************************************************************************
 * 
*/
#ifndef __SMNT_AIRKISS_H__
#define __SMNT_AIRKISS_H__

//#include "rtmp_type.h"
//#include "rtmp.h"//RTMP_ADAPTER
#include "rtmp_comm.h"
#include "rt_config.h"
#include "airkiss.h"


#define AK_CH_MONITER_TIMER 500

/*typedef struct _MONITOR_CH_LIST {
    INT monitor_ch;
} MONITOR_CH_LIST, *PMONITOR_CH_LIST;
*/

INT ak_change_channel(void);
INT ak_init(RTMP_ADAPTER *pAd, char *crypt_key);

/*VOID ak_chMonitorTimeout(
    IN PVOID SystemSpecific1,
    IN PVOID FunctionContext,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3);*/

INT ak_get_Channel(VOID);

VOID ak_rcv_frame_handle(RTMP_ADAPTER *pAd, PHEADER_802_11 phdr, USHORT data_len);

airkiss_status_t ak_get_status(VOID);

INT ak_get_locked_ch(BSS_TABLE *Tab, UCHAR *pBssid);

VOID ak_test(RTMP_ADAPTER *pAd);

#endif /* end of __SMNT_AIRKISS_H__ */

