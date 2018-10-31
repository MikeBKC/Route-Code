#ifndef __STA_TABLE_H__
#define __STA_TABLE_H__


typedef struct _sta_cmd_buf_t{
    uchar   mac[MAC_LEN];
    uint16  phy;
    int32   time;
    char    RSSI;
    char    ap_idx;
    int16   rate;
    int32   rx_byte;
    int32   rx_rate;
    int32   tx_byte;
    int32   tx_rate;
}sta_cmd_buf_t;

typedef struct _sta_mac_cmd_hdr_t{
    uint16 sta_num;
    uint16 tb_offset;
    int16 entry_num;
    int16 entry_size;
    sta_cmd_buf_t buf[0];
}sta_cmd_hdr_t;

#if (CLIENT_HOST_NAME == FYES)
#define NONE_NODE    0 
#define STA_TB_HASH_SIZE_AP 32
#define STA_TB_SIZE_AP (AP_MAX_CLIENT_NUM + AP_MAX_CLIENT_NUM/2)
#define CLIENT_HOST_NAME_SIZE 32
typedef struct _sta_entry_ap_t{
    tb_entry_hdr_t hdr;
    uint32 flag;	 /*2.4 or 5 ,other*/
    uint32 ip;
    uchar host_name[CLIENT_HOST_NAME_SIZE];
}sta_entry_ap_t;
#endif

#ifdef URCP_STA_MAC_TABLE 
#define  AC_STA_TABLE_TIME_MS 70000 

extern void ac_sta_table_init(void);
extern void ac_sta_table_handle(void);
extern uint16 receive_ap_mac_table(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info);
extern sta_entry_t* ac_sta_table_lookup(uchar *mac);
#endif
#if AP_SEND_CLTS_AUTO
extern void ap_trap_sta_timer_init(void);
extern void trap_ap_mac_table(void);
extern void ap_trap_sta_timer_set(uint32 sec);
extern void ap_trap_sta_enable(uint32 enable);
extern void ap_trap_sta_poll(void);
#if defined(WIRELESS_5G) && (WIRELESS_5G==FYES)
extern void ap_trap_sta_5g_poll(void);
#endif
#endif
#endif
