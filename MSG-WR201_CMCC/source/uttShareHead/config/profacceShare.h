#ifndef __PROFMISC_SHARE_H
#define __PROFMISC_SHARE_H

typedef enum {
	PROF_CHANGE_ADD = 0,
	PROF_CHANGE_ADD_ALL,
	PROF_CHANGE_DEL,
	PROF_CHANGE_EDIT,
	PROF_CHANGE_DELALL,
	PROF_CHANGE_SPECIAL
}ProfChangeType;

typedef void (*ProfileUpdateFnc)(Pointer		tag,
				 MibProfileType     	profileType,
                                 ProfChangeType         changType,
				 Uint32	                instIndex,
				 const Pointer		oldProfileDataPtr,
				 const Pointer		newProfileDataPtr );


typedef struct Registrant {
    struct Registrant	*next;
    Pointer		updateTag;
    MibProfileType	oldProfileType;
    ProfileUpdateFnc	updateFnc;
    Uint32		rCreTm;
    Uint32		rLastTm;
    Uint32		rUseCnt;
} Registrant;

typedef struct st_profChangeData {
    MibProfileType profType;   
    int        instIndex;      
    int        flag;
    ProfileUnion oldDataPtr;
}ProfChangeData;

struct st_linkCheckMsg {
    Uint32 status;
    Uint32 wanIndex;
};

#if (NET_SHARE_MANAGE == FYES)
#define MDEV_LEN 16
struct st_mdevMsg {
    Uint32 status;/*1:add, 0;remove*/
    char dev[MDEV_LEN];
};
#endif

typedef enum en_msg_FriType{
    MSG_PROF,
    MSG_NVRAM,
    MSG_MPGLOBALE_CHANGE,
    MSG_INTERFACE_CHANGE,
    MSG_LINECHK_STA_CHANGE,
    MSG_NL_UPDATE_STATS_INTERVAL_CHANGE,
    MSG_NL_UPDATE_AUTODEL_INTERVAL_CHANGE,
    MSG_PROF_REINIT,
    MSG_IPSEC_AUTOUP_OVER_PPTP_CHANGE,
    MSG_CACHE_STA_CHANGE,
    MSG_LINK_STA_CHANGE,
#if (TAG_VLAN == FYES)
    MSG_TAG_VLAN_ROUTE_CHG,
#endif
#if (NET_SHARE_MANAGE == FYES)
    MSG_MDEV_CHG,
#endif
}speedMainType;

typedef enum en_msg_SecType{
    MSG_PROF_CHANGE = 0,
}speedSecType;

struct st_msgPara{
    speedMainType   mainType;
    int    secType;
    union un_data{
	ProfChangeData profChangeData;
        int wanStatuses[MAX_WAN_IF_COUNT];
        int ifIndex;
        struct st_linkCheckMsg linkCheckMsg;

#if (NET_SHARE_MANAGE == FYES)
	struct st_mdevMsg  mdevMsg;
#endif

#ifdef CONFIG_UTT_NETFILTER
	int utt_stats_update_interval;
#endif
#if (DHCP_AUTO_BIND == FYES)
	int utt_autodel_update_interval;
#endif
#if (IP_SEC == FYES)
#if((PPTP_SERVER == FYES)||(PPTP_CLIENT == FYES))
	char utt_ipsec_autoup[20];
#endif
#endif
#if (CACHE_SERVER == FYES)
	int utt_cache_sta;
#endif
#if (EASY_TASK == FYES)
	int utt_link_status;
#if (WIRELESS == FYES)
	int utt_wireless_status;
#endif
#endif
    }data;
};

struct st_uttMsg{
    long mtype;
    struct st_msgPara msgPara;
};

#endif
