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
    int32 tb_offset;
    int16 entry_num;
    int16 entry_size;
    sta_cmd_buf_t buf[0];
}sta_cmd_hdr_t;

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
#endif
#endif
