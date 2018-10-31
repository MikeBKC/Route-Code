


/*
 * added by brwang
 */

#define UTT_PPP_CLIENT_NAME_LEN     80
#define UTT_PPP_CLIENT_MAC_LEN      20
#define UTT_PPP_CLIENT_IP_LEN       20
#define UTT_PPP_CLIENT_IFNAME_LEN   20
#define UTT_PPP_CLIENT_DEVICE_LEN   20
#define UTT_PPP_CLIENT_OURNAME_LEN  20
#define UTT_PPPOE_CLIENT_INFO_FILE "/var/run/uttPppoeClientInfo"
#define UTT_PPTP_CLIENT_INFO_FILE "/var/run/uttPptpClientInfo"
#define UTT_L2TP_CLIENT_INFO_FILE "/var/run/uttL2tpClientInfo"

struct st_uttPppdClient {
    unsigned int pid;
    unsigned int STime;
    unsigned int LTime;
    unsigned int ConTime;
    unsigned long long rxByte;
    unsigned long long txByte;
    char user[UTT_PPP_CLIENT_NAME_LEN + 1];
    char mac[UTT_PPP_CLIENT_MAC_LEN + 1];
    char ip[UTT_PPP_CLIENT_IP_LEN + 1];
    char gwip[UTT_PPP_CLIENT_IP_LEN + 1];
    char ifname[UTT_PPP_CLIENT_IFNAME_LEN + 1];
    char device[UTT_PPP_CLIENT_DEVICE_LEN + 1];
    char ourName[UTT_PPP_CLIENT_OURNAME_LEN + 1];
};
