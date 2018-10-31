#ifndef _NETBIOS_H_
#define _NETBIOS_H_

#include <linux/switch/swioc_common.h>
#include <switch.h>

#define  OPCODE_QUERY   0u
#define	 OPCODE_REGIST  5u
#define  OPCODE_RELEASE 6u
#define  OPCODE_WACK    7u
#define  OPCODE_REFRESH 8u


#define QUESTION_TYPE_NB 0x0020u
#define QUESTION_TYPE_NBSTAT 0x0021u

#define QUESTION_CLASS_IN 0x0001u

#define NETBIOS_NAME_LENGTH 32
#define NETBIOS_PACKET_NAME_LENGTH  34

typedef struct _netbios_packet_question
{
    uchar question_name[NETBIOS_PACKET_NAME_LENGTH];
    uint16 question_type;
    uint16 question_class;
}netbios_question;

typedef struct _netbios_packet_header
{
    uint16 trans_id;
    uint16 flags;
    uint16 qdcount;
    uint16 ancount;
    uint16 nscount;
    uint16 arcount;
} netbios_header;


typedef struct _netbios_query_request
{
    uint16 trans_id;
    uint16 flags;
    uint16 qdcount;
    uint16 ancount;
    uint16 nscount;
    uint16 arcount;
    uchar question_name[NETBIOS_PACKET_NAME_LENGTH];
    uint16 question_type;
    uint16 question_class;
}netbios_query_request;

#define RR_TYPE_A 0x0000u
#define RR_TYPE_NS 0x0001u
#define RR_TYPE_NULL 0x000au
#define RR_TYPE_NB 0x0020u
#define RR_TYPE_NBSTAT 0x0021u

#define RR_CLASS_IN 0x0001u

typedef struct _netbios_rdata
{
    uint16 rdata_flags;
    uchar rdata_address[4];
}netbios_rdata;

typedef struct _netbios_query_response
{
    uchar rr_name[NETBIOS_PACKET_NAME_LENGTH];
    uint16 rr_type;
    uint16 rr_class;
    uint16 ttl[2];
    uint16 rdlen;
}netbios_query_response;


typedef struct _add_entry_array
{
    uint16 nb_flags;
    uchar nb_address[4];
}add_entry_array;

typedef struct _netbios_packet_resource
{
    uint16 rr_type;
    uint16 rr_class;
    uint32 ttl;
    uint16 rdlength;
    uint16 rdata_flags;
    uint32 rdata_address;
}netbios_resource;



extern void netbios_parse(system_frame_header_t *header, uchar *frame);
extern void netbios_query(char *host_name);
#endif

