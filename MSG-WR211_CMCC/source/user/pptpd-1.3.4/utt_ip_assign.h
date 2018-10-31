#define PPTP_IP_BIND
#define UTT_MSG_PPTP_SRV_FILE "/tmp/uttPptpSrvMsg"
typedef struct st_utt_msg_pptp_srv_data {
    unsigned int pptp_ass_ip; /* pppoe server分配的IP */
    unsigned int ppp_ass_ip; /* ppp分配的IP, 都是网络序 */
} utt_msg_pptp_srv_data;

typedef struct st_utt_msg_pptp_srv {
    long int msg_type;
    struct st_utt_msg_pptp_srv_data msg_data;
} utt_msg_pptp_srv;
