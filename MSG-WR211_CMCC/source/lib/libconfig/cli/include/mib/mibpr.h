#if !defined(MIBPR_H)
#define  MIBPR_H

#define MAX_POLICY_ROUTE_PROFILES	    20

struct st_policy_route {
    char	    hostName[MAX_PROFILE_NAME_LENGTH + 1];
    char	    interface[MAX_PROFILE_NAME_LENGTH + 1];
    Boolean	    active;
    Boolean	    lively;	    /* ��Ч���أ�����ʵ������ɾ�����ö��������� */
    IpAddress	    ip_from;
    IpAddress	    ip_end;
    Uint16	    port_from;
    Uint16	    port_end;
    Uint8	    protocol_id;
    Int		    pkt_size_from;
    Int		    pkt_size_end;
};

typedef struct st_policy_route PolicyRouteProfile;





#endif
