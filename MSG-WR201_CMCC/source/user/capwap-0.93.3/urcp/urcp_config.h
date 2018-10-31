#ifndef __URCP_CONFIG_H__
#define __URCP_CONFIG_H__

#define FILE_ID_CONFIG  0u
#define FILE_ID_LOG     1u
#define FILE_ID_IMAGE	2u
#define FILE_ID_CONFIG_TEMPLATE	    3u

#ifdef URCP_WEB
extern void urcp_msg_read_file(struct msgbuf_t *web_msg, uint32 file_id);
extern void urcp_msg_write_conf(struct msgbuf_t *web_msg);
extern void urcp_rev_conf_cb(void *ptr, urcp_status_t urcp_state);
extern void create_nvram_file(void);
extern void urcp_msg_write_file(struct msgbuf_t *web_msg, uint32 file_id);
#endif /*end URCP_WEB*/

extern int32 get_conf_file_size(char *fname);

#ifdef URCP_AP
extern uint16 urcp_auto_get_file(uchar *mac_addr, uint32 file_id);
extern void urcp_rev_conf_cb(void *ptr, urcp_status_t urcp_state);
extern void create_nvram_file(void);
extern void urcp_conf_backup_cb(void *ptr, urcp_status_t urcp_state);
extern void upload_current_config_file_to_ac(char *mac, unsigned int sn);
#endif

extern void create_image_file(void);
#endif 
