#if !defined(MIBDDNS_H)
#define  MIBDDNS_H

#define MAX_PUBLIC_DDNS_PROFILES		    5

struct st_public_ddns {
    char            hostName[MAX_PROFILE_NAME_LENGTH + 1];
    char            interface[MAX_PROFILE_NAME_LENGTH + 1];
    Boolean         active;
    Boolean         lively;
    char	    serverName[MAX_DOMAIN_NAME_LENGTH + 1];
    IpAddress	    serverIp;
    char	    hostname[MAX_DOMAIN_NAME_LENGTH + 1];
    Char	    username[MAX_UNIT_NAME_LEN + 1];
    Char	    password[MAX_PASSWORD_LENGTH + 1];
    Int		    retryTimes;
    Int		    retryInterval;
};

typedef  struct st_public_ddns PublicDdnsProfile;




#endif
