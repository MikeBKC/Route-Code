#if ! defined(MIBGROUP_H)
#define MIBGROUP_H

#ifndef ETHER_ADDR_LEN
#define ETHER_ADDR_LEN 6
#endif

#define MAX_STR_LENGTH		32

#define MAX_IPGROUP_LINE_COUNT 10	
#define MAX_IPGROUP_LINE_LENGTH 31
#define MAX_IPGROUP_NAME_LENGTH 11


#define MAX_PORTGROUP_LINE_COUNT 10	
#define MAX_PORTGROUP_LINE_LENGTH 31
#define MAX_PORTGROUP_NAME_LENGTH 11

/*2007.11.15*/
typedef enum en_ipclass{
		IP_LAN,IP_WAN1,IP_WAN2,IP_WAN3,IP_WAN4,IP_ALLWAN
}ipClass;

/*2007.11.15*/
typedef enum en_portclass{
		IP_NORMAL,IP_KEY,IP_URL,IP_DNS,IP_MAC
}portClass;

typedef struct st_ipgroup_profile {
	ipClass	flag; 			/*2007.11.15*/
        char name[MAX_IPGROUP_NAME_LENGTH+1];
        unsigned char active;
       // unsigned char self;
        char line[MAX_IPGROUP_LINE_COUNT][MAX_IPGROUP_LINE_LENGTH+1];
} IpGroupProfile;

typedef struct st_ipsegment{
	unsigned long IpStart;
	unsigned long IpEnd;
	struct st_ipsegment *next;
}IpSegment;

typedef struct st_ipgroup{
	ipClass	flag; 			/*2007.11.15*/
	char name[MAX_IPGROUP_NAME_LENGTH+1];
	IpSegment *headSegment;
	struct st_ipgroup *next;
}IpGroup;


/* portGroup */
typedef struct st_portgroup_profile {
        char name[MAX_PORTGROUP_NAME_LENGTH+1];
        portClass	flag;				/*2007.11.15*/
        unsigned char active;
        //unsigned char self;
        char line[MAX_PORTGROUP_LINE_COUNT][MAX_PORTGROUP_LINE_LENGTH+1];
} PortGroupProfile;

typedef struct st_portsegment{
	unsigned short int srcPortStart;
	unsigned short int srcPortEnd;
	unsigned short int destPortStart;
	unsigned short int destPortEnd;
	struct st_portsegment *next;
}PortSegment;

typedef struct st_protocol{
	unsigned short int protocol;
	struct st_protocol *next;
}PortProtocol;

/*2007.11.15*/
typedef	struct st_strseg{
	char		str[MAX_PORTGROUP_LINE_LENGTH+1];
	int length;
	struct st_strseg *next;
}StrSegment;

/*2007.12.19*/
typedef struct st_macseg{
	unsigned char mac[ETHER_ADDR_LEN];//ETHER_ADDR_LEN
	struct st_macseg *next;
}MacSegment;

typedef struct st_portgroup{
	char name[MAX_PORTGROUP_NAME_LENGTH+1];
        portClass	flag;				/*2007.11.15*/
	PortSegment	 *udpheadSegment;
	PortSegment 	*tcpheadSegment;
	PortProtocol	 *otherheadSegment;
	StrSegment	*urlheadSegment;
	StrSegment	*keyheadSegment;
	StrSegment	*dnsheadSegment;
	MacSegment	*macheadSegment;	/*2007.12.19*/
	struct st_portgroup *next;
}PortGroup;
#endif
