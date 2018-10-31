#ifndef _XT_UTTLOG_H
#define _XT_UTTLOG_H

#define UTT_WEB_LOG_ENABLE   (1 << 0)
#define UTT_QQ_LOG_ENABLE   (1 << 1)
#define UTT_MSN_LOG_ENABLE   (1 << 2)
#define UTT_MAIL_LOG_ENABLE   (1 << 3)
#define UTT_LUNTAN_LOG_ENABLE   (1 << 4)
#define UTT_WEIBO_LOG_ENABLE   (1 << 5)
#define UTT_NAT_LOG_ENABLE   (1 << 6)

#define PROT_TCP               0x06    /* TCP                          */
#define PROT_UDP               0x11    /* UDP                          */
#define PROT_ICMP          0x01    /* ICMP                 */
#define PROT_AH            0x33    /* AH                   */
#define IPPORT_SMTP            25      /* Mail port */
#define IPPORT_POP3            110     /* pop3 port */
#define IPPORT_MSN             1863    /* MSN port */
#define IPPORT_SSL             443     /* SSL port */
#define IPPORT_HTTP            80      /* HTTP port*/ 
#define IPPORT_UDPQQ2          8000    /* QQ port*/
#define MAX_ERROR_LOG_MSG_LENGTH      120
#define MAX_SYSLOG_LOG_MSG_LENGTH     200
#define MAX_MBID_LEN                  20
#define OICQ_LOGIN                      0x0022
#define OICQ_LOGOUT                     0x0001
#define OICQ_LOGIN_2009                 0x0001
#define OICQ_LOGOUT_2009                0x0062
#define OICQ_2008_VERSION               0x1251
#define OICQ_TCP_LOGIN                  0x00dd
#define OICQ_TCP_2008_LOGOUT            0x00a1
#define OICQ_TCP_2009_LOGOUT            0x00b9
#define LOG_UDP_MIN_LEN			13
#define LOG_TCP_MIN_LEN                  4             
#define POP_TCP_MIN_LEN                  8             
#define MAIL_RIGHT_BRACE                0x3e
#define SUBJECT_END                     0x0d
#define MAX_IP_LEN                      16
#define MSN_ID_LEN                      32
#define ALI_ID_LEN                      25

#define SMTP_NULL_STAT                  0
#define SMTP_WAIT_SEND_DATA             1
#define SMTP_OK_SEND_DATA               2

#define POP_NULL_STAT                   0
#define POP_WAIT_RECEIVE_DATA           1

#define MAIL_ADDRESS_LEN                150
#define MAX_URL_LEN                     100
#define MAX_ACCOUNT_LEN                 30
#define ASIZE           256
/* checks to see if a tcp packet is a tcp control packet */

/*
 *  * header for qq packet
 *   */
typedef struct qq_hdr 
{
    __u8   flag;
    __u16  version;
    __u16  command;
    __u16  seq;
    __u32  qq_number;
}__attribute__ ((packed)) QQ_HDR;

struct log_host_t 
{
    struct log_host_t              *next;
    __u16                          qq_seq;
    __u32                          qq_number;
    char                            msnID[50 + 1];
    __u32                          msnIP;
    int                             popStat;
    int                             smtpStat;
};
typedef struct log_host_t log_host;
typedef struct iphdr IP_HDR;
typedef struct tcphdr TCP_HDR;
typedef struct udphdr UDP_HDR;

/*
 * sw: which logs is enabled
 */
struct xt_uttlog_info {
    u_int32_t	sw;
};

extern int _checkData(char a);

#if 1
#define MAX_ERROR_LOG_DATE_LENGTH   30
#define MAX_ERROR_DATELOG_LENGTH   (MAX_ERROR_LOG_MSG_LENGTH + MAX_ERROR_LOG_DATE_LENGTH)
/* 日志暂时保存到数组中 */
typedef struct st_uttErrLog {
    char msg[MAX_ERROR_DATELOG_LENGTH + 1];
} UttErrLog;

#define NUM_OF_ERR_LOG	400
#define START_OF_ERR_LOG 0
#endif
#endif /* _XT_UTTLOG_H */
