/*** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***
 ***                                                               ***
 ***                  CONFIDENTIAL INFORMATION                     ***
 ***                  ------------------------                     ***
 ***    This Document contains Confidential Information or         ***
 ***    Trade Secrets, or both, which are the property of UTT      ***
 ***    Technologies, Inc. This document may not be  copied,       ***
 ***    reproduced, reduced to any electronic medium or machine    ***
 ***    readable form or otherwise duplicated and the information  ***
 ***    herein may not be used, disseminated or otherwise          ***
 ***    disclosed, except with the prior written consent of UTT    ***
 ***    Technologies, Inc.                                         ***
 ***                                                               ***
 *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***/
 
/* 
    Copyright 1991-2002 UTT Technologies, Inc.
    All rights reserved.
    Use of copyright notice does not imply publication.
 */

/*
 * SCCS ID:	$Id: ddns.h,v 1.3 2013/08/06 10:41:43 sun.jianqing Exp $
 * 
 * File:  
 * 
 * Overview:
 *  
 *
*/

#if !defined(_DDNS_H)
# define _DDNS_H

/*
 * brwang
 */
#define MALLOC  malloc
#define FREE    free 
#define Int     int
#define Char    char
#define Uint8 unsigned char
#define Uint16 unsigned short
#define Uint32 unsigned int
#define IpAddress unsigned int
#define Boolean char
#define Word short
#define MAX_PROFILE_NAME_LENGTH       11
#define MAX_DOMAIN_NAME_LENGTH         63
#define MAX_HOST_NAME_LENGTH           31
#define MAX_DDNS_KEY_LEN       63
#define MAX_UNIT_NAME_LEN    72
#define MAX_PASSWORD_LENGTH           31
#define MAXCONFS 1
#define MD5_DIGEST_LEN 16
#define UDP_PORT_ANY_NOPRIV    0xfffe
#if !defined(_DDNS_H)
#define NULL 0
#endif
#define INDIR_MASK     0xc0
#define GLOBAL_VROUTER_ID          0 
#define HZ 1
#define EV_UDP_RECV     1 
#define TRUE    1
#define MIN(a,b) (((a)<(b))?(a):(b))
#define DDNS_IPLINK_LOG "/tmp/ddns/ddns_iplink.log"
//enum ddnsTzoneTypeEnum{
//      DDNS_TZ_GUAN,
//      DDNS_TZ_NONE,
//};



#if ( UTT_DEBUG == FYES ) || ( UTT_STAT == FYES )
#define DDNS_DEBUG
#else
#undef  DDNS_DEBUG
#endif /* UTT_DEBUG */


# define NO_PREREQ	5 
# define NO_AUTH	6
# define NO_ZONE	7
# define ERRTSIG	8

# if !defined (PACKETSZ)
# define PACKETSZ	512	/* maximum packet size */
# endif

# if !defined (MAXPACKET)
# define MAXPACKET PACKETSZ
# endif

# define MAXCDNAME	255	/* maximum compressed domain name */
# define MAXLABEL	63	/* maximum length of domain label */
# define DNS_UPDATE 	5

# define NOERROR	0		/* no error */
# define FORMERR	1		/* format error */
# define SERVFAIL	2		/* server failure */
# define NXDOMAIN	3		/* non existent domain */
# define NOTIMP		4		/* not implemented */
# define REFUSED	5		/* query refused */
# define YXDOMAIN	6		// Some name that ought not to exist, does exist.
# define YXRRSET	7		// Some RRset that ought not to exist, does exist.
# define NXRRSET	8		// Some RRset that ought to exist, does not exist.
# define NOTAUTH	9		// The server is not authoritative for the zone named in the Zone Section.
# define NOTZONE	10		// A name used in the Prerequisite or Update Section is not within the zone denoted by the Zone Section

# define BADSIG		16
# define BADKEY		17
# define BADTIME	18

# define C_IN		1		/* the arpa internet */
# define C_CHAOS	3		/* for chaos net at MIT */
# define C_HS		4		/* for Hesiod name server at MIT */
# define C_ANY		255		/* wildcard match */
# define C_NONE		254		// None

# define T_A		1		/* host address */
# define T_NS		2		/* authoritative server */
# define T_MD		3		/* mail destination */
# define T_MF		4		/* mail forwarder */
# define T_CNAME	5		/* connonical name */
# define T_SOA		6		/* start of authority zone */
# define T_MB		7		/* mailbox domain name */
# define T_MG		8		/* mail group member */
# define T_MR		9		/* mail rename name */
# define T_NULL		10		/* null resource record */
# define T_WKS		11		/* well known service */
# define T_PTR		12		/* domain name pointer */
# define T_HINFO	13		/* host information */
# define T_MINFO	14		/* mailbox information */
# define T_MX		15		/* mail routing information */
# define T_TXT		16		/* text strings */
# define T_UINFO	100		/* user (finger) information */
# define T_UID		101		/* user ID */
# define T_GID		102		/* group ID */
# define T_UNSPEC	103		/* Unspecified format (binary data) */
# define T_AXFR		252		/* transfer zone of authority */
# define T_MAILB	253		/* transfer mailbox records */
# define T_MAILA	254		/* transfer mail agent records */
# define T_ANY		255		/* wildcard match */

# define T_TSIG		250		// transaction signature

#define GRT_Z(x,r) if((x)<0) return (r);

/*
 * brwang
 */
typedef enum ddns_tzone_type {
      DDNS_TZ_NONE      = 0,
      DDNS_TZ_GUAN      = 1,
      DDNS_TZ_MAX       = (24),
} DDNS_TZONE_TYPE;
enum section_t {ZONE_SEC=0,PRE_SEC,UP_SEC,AR_SEC};
typedef struct {
	Uint16	id;		/* query identification number */
#if (((CONFIG_BIG_ENDIAN) == 1) || (CONFIG_PPC_85xx == 1))
	/* fields in third byte */
	Uint8	qr:1;		/* response flag */
	Uint8	opcode:4;	/* purpose of message */
	Uint8	aa:1;		/* authoritive answer */
	Uint8	tc:1;		/* truncated message */
	Uint8	rd:1;		/* recursion desired */
	/* fields in fourth byte */
	Uint8	ra:1;		/* recursion available */
	Uint8	pr:1;		/* primary server required (non standard) */
	Uint8	unused:2;	/* unused bits */
	Uint8	rcode:4;	/* response code */
#else
	/* fields in third byte */
	Uint8	rd:1;		/* recursion desired */
	Uint8	tc:1;		/* truncated message */
	Uint8	aa:1;		/* authoritive answer */
	Uint8	opcode:4;	/* purpose of message */
	Uint8	qr:1;		/* response flag */
	/* fields in fourth byte */
	Uint8	rcode:4;	/* response code */
	Uint8	unused:2;	/* unused bits */
	Uint8	pr:1;		/* primary server required (non standard) */
	Uint8	ra:1;		/* recursion available */
# endif
	/* remaining bytes */
	Uint16	zocount;	/* The number of RRs in the Zone Section */
	Uint16	prcount;	/* The number of RRs in the Prerequisite Section */
	Uint16	upcount;	/* The number of RRs in the Update Section */
	Uint16	arcount;	/* The number of RRs in the Additional Data Section */
}DDNS_HDR;



# define MIN_DDNS_UPDATE_RETRY		30
# define MIN_DDNS_UPDATE_TIMEOUT	5


extern int updateRR_A2(const char *profile, Uint32 newip, Uint32 oldip, const char * fqdn, int ttl);
//extern void showUpdateTime( LcdTerminalHdl term );

/*
 * 此开关用来进行调试，正式版本需要关闭此开关！
 */
#define DDNS_DEBUG_2		    FYES

/*
 * 断言语句
 */

#if (DDNS_DEBUG_2 == FYES)
#define DDNS_ASSERT(x)		    ASSERT(x)
#define DDNS_ASSERT_WARN(x)	    ASSERT_WARN(x)
#else
#define DDNS_ASSERT(x)
#define DDNS_ASSERT_WARN(x)

#endif


//extern Boolean ddns_print_trigger;



#define DDNS_PRINTF(fmt, args...)  do {							\
                                            if (ddns_print_trigger) {              \
                                                printf("DDNS: " fmt, ## args);    \
                                            }                                           \
                                        } while(0)


#define DDNS_PRINTF2(fmt, args...)  do {							\
                                            if (ddns_print_trigger) {			\
                                                printf(fmt, ## args);			\
                                            }                                           \
                                        } while(0)




/*
 *
 */
#if defined (DDNS_DEBUG)
extern void ddns_show_time(void);
extern void ddns_set_time(char *cmd);
extern int ddns_show_configure(void);
extern void ddns_show_oldip(void);
extern void ddns_show_err(char *cmd);
extern void ddns_show_debug(char *cmd);
extern void ddns_test_hmac(void);
extern void ddns_encrypt(char *cmd);
extern void ddns_debug_rest(char *cmd);
extern void ddns_usage(void);
#endif

#define NUM_OF_ETHER_SLOTS 1
#if (LANPORT5 == FYES)
#define DDNS_ETHER_SLOT_COUNT           5
#else   
#define DDNS_ETHER_SLOT_COUNT           NUM_OF_ETHER_SLOTS
#endif  

//extern MAILBOX_ID _ddnsMB;

typedef enum {
    DDNS_STYPE_NONE,

    DDNS_STYPE_IPLINK,
    DDNS_STYPE_CN99,

    DDNS_STYPE_MAX
} ddns_server_type;

typedef enum {
    DDNS_UPDATE_NONE,

    DDNS_UPDATE_FAIL,
    DDNS_UPDATE_DOING,
    DDNS_UPDATE_SUC,

    DDNS_UPDATE_MAX
} ddns_update_type;



typedef struct st_ddns_update {
//    PublicDdnsProfile   ddns_profile;
    Int			(*ddns_update_func)(struct st_ddns_update *ddns_update_ptr, void *data);
    char		*ddns_state;
    ddns_update_type	update_type;
    Boolean		locked;
    Int			ethport;
    IpAddress		update_ip;
    Int			timer_id;
    ddns_server_type	stype;
    Int			retry_times;
} DDNS_UPDATE;


extern DDNS_UPDATE  ddns_update[DDNS_ETHER_SLOT_COUNT];

typedef enum {
    DDNS_NONE,
    
    DDNS_UPDATE_CN99_MSG,
    DDNS_UPDATE_IPLINK_MSG,

    DDNS_MAX
} ddns_msg_type;

typedef struct {
    ddns_msg_type	msg_type;
    void		*data;
    DDNS_UPDATE		*ddns_update_ptr;

} ddns_msg;

struct ddns_prof{
	char		name[MAX_PROFILE_NAME_LENGTH+1];
	Boolean		Enable;
	IpAddress	Server1;
	Word		Port1;
	IpAddress	Server2;
	Word		Port2;
	char		Hostname[MAX_HOST_NAME_LENGTH+1];
	Word		Updateretry;
	Word		Updatetimeout;
	Uint32		UpdateInterval;
	char		Keyname[MAX_HOST_NAME_LENGTH+1];
	char		Key[MAX_DDNS_KEY_LEN+1];
	char		enKey[MAX_DDNS_KEY_LEN+1];
	char		Profile[MAX_PROFILE_NAME_LENGTH+1];
	char		secProfile[MAX_PROFILE_NAME_LENGTH+1];
	char		LastUpdateTime[MAX_DDNS_KEY_LEN+1];
	DDNS_TZONE_TYPE	offset;
	char		interface[MAX_PROFILE_NAME_LENGTH + 1];
}; 

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
#if 0
extern void ddns_iplink_prof_changed(const struct ddns_prof *old_ddns_prof,
				     const struct ddns_prof *new_ddns_prof);


extern void ddns_load_iplink_state(char *buffer, ddns_update_type utype);
extern void ddns_update_cn99_safe(DDNS_UPDATE *update_ptr);

extern char *ddns_strncpy(char *dest, const char *src, Int len);
extern void ddns_get_localtime(char *date_time_string, Boolean gmt);
#endif


#endif  /*!defined(_DDNS_H) */
