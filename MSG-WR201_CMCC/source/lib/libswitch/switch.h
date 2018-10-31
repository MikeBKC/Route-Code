
#ifndef __SWITCH_H__
#define __SWITCH_H__
#include <linux/switch/switch.h>
#include <linux/autoconf.h>

#define VID_NULL 0

#ifdef CONFIG_VSC7390
/* Get field from IFH0/IFH1 */
/*以下宏定义仅仅适用于SPARX_G24*/
#define O_IFH_PORT     42     /* 46-42: Port */
#define M_IFH_PORT     0x1f
#define O_IFH_TAGGED   63     /* 63   : Tagged */ 
#define M_IFH_TAGGED   0x1
#define O_IFH_UPRIO    29     /* 31-29: Tag priority */
#define M_IFH_UPRIO    0x7
#define O_IFH_CFI      28     /* 28   : CFI */
#define M_IFH_CFI      0x1
#define O_IFH_VID      16     /* 27-16: VID */
#define M_IFH_VID      0xfff   

#define IFH_GET(ifh0, ifh1, field) ((O_IFH_##field > 31 ? (ifh0 >> (O_IFH_##field - 32)) : (ifh1 >> O_IFH_##field)) & M_IFH_##field)
#endif

#define ETH_P_URCP	 0x8899
/* Tag Priority */
typedef uint32 vtss_tagprio_t;   /* 0-7 */

/* Tag Control Information (according to IEEE 802.1Q) */
typedef struct _vtss_tci_t {
    vid_t     vid;     /* VLAN ID */
    BOOL           cfi;     /* Canonical Format Indicator */
    vtss_tagprio_t tagprio; /* Tag priority */
} vtss_tci_t;
typedef struct _system_frame_header_t{
    uint32                length;         /* Frame length excluding CRC */
    uint32                source_port_no; /* Source port number */
    BOOL                  arrived_tagged; /* TRUE is frame was tagged */
    vtss_tci_t            tag;            /* VLAN tag from frame or VLAN port setup */
    uchar*                raw;              /*指向原始二层数据包第一个字节*/
    uchar*                data;             /*指向联动数据包头，即0x8899后面的第一个字节*/
    int32                 fromL3;
}system_frame_header_t;
/******************
 *  *type: packet ID. 8100 or 8899
 *  *value: proto info
 *  *protoid_len: packet ID or lenth
 *****************/
typedef struct _tagInfo_t {
	uint16  type;
	uint16  value;
	uint16  proid_len;
}tagInfo_t;

typedef void (*dir_pf)(char* file, void* param);
extern const char *str_macaddr(const unsigned char *macaddr);

extern int get_mask(char *ifname, unsigned int *if_net);
extern int get_ip(char *ifname, unsigned int *if_addr);
extern int get_mac(char *ifname, unsigned char *if_hw);
extern int set_mask(char *ifnmae, unsigned int mask);
extern int set_ip(char *ifname, unsigned int sysIp);
extern void set_gw(char *gwaddr);

extern unsigned int bit_array_to_int(unsigned char array[], unsigned int tsize);
extern int int_to_bit_array(unsigned char member[], unsigned int len, unsigned int value);

extern void print_ip_addr (unsigned int ip);
extern int validate_ip_addr (unsigned int ip_addr);
extern int validate_netmask (unsigned int mask);
extern int retrieve_ip_addr (unsigned char *ip_buffer, unsigned int *nIp);

extern char* int2ip(unsigned int lip,char * ip);
extern unsigned int ip2int(char *ip);

extern int get_switch_if_name(char * if_name);
extern int sw_frame_header_pull(char *raw, system_frame_header_t * sys_header);
extern int create_raw_socket(int eth_proto);
extern int mac_cmp(unsigned char *mac1, unsigned char* mac2);
extern unsigned int port2int(unsigned int port_no);
extern unsigned int port2ext(unsigned int port_no);
extern int32 h2_send_frame(uchar*buf, uint32 len, int32 socket);
extern int32 h2_send_by_port(uint32 port_no, uchar *frame, uint32 length, vid_t vid);
extern int32 h2_fwd_frame(uchar*buf, uint32 len, uint32 vid, uint32 exclude_port);
extern void frame_dump(unsigned char* frame, int length);
//extern int32 getSystemSn(void);
extern void mem_dec_cpy(char *dst, char *src, int len);
extern int connect_capwap(void);
extern int get_verinfo_from_file(char * file, int* hwVer, int* swVer);
#endif

