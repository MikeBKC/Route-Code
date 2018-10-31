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
 * SCCS ID:	$Id: mibcbq.h,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
 * 
 * File:  
 * 
 * Overview:
 *  
 *
*/





	/* The length of cbq filter and class name */
//#define MAX_CBQ_NAME_LEN	15
#define MAX_CBQ_NAME_LEN	MAX_PROFILE_NAME_LENGTH
#define MAX_CBQ_HANDLE_LEN	MAX_PROFILE_NAME_LENGTH

struct commQdiscProfile{
    char dev[MAX_CBQ_HANDLE_LEN+1];
    char parent[MAX_CBQ_HANDLE_LEN+1];             
    char handle[MAX_CBQ_HANDLE_LEN+1];               
};

typedef struct commQdiscProfile CommQdiscProfile;

struct cbqDevQdiscProfile{
    struct  commQdiscProfile comm;
    Uint32  bandwidth;            	//unit: byte
    Uint16  avpkt;                  	//unit: byte                
    Uint8   ewma;                    	// ewma_log,    0-31, default::   5
    short   cell;                    	// unit; byte   cell must be 2^n, default::  8  
    Uint16  mpu;                    	// byte         default::  64 for ethernet
    Uint32  allot;                  	// byte         default::  1514 for ethernet
    Uint8   help;                    	// default::0
    Boolean active;  	 	   	//default: no
};

typedef struct cbqDevQdiscProfile CbqDevQdisc;

enum qdisctype {qFIFO,qCBQ,qSFQ} ;

struct cbqClassProfile{
/* pls keep these 3 items first */
    char    dev[MAX_CBQ_HANDLE_LEN+1];
    char    parent[MAX_CBQ_HANDLE_LEN+1];             
    char    classid[MAX_CBQ_HANDLE_LEN+1];            
/* ********************** */
    Uint32  rate;               //unit: byte
    Uint32  bandwidth;           //unit: byte
    int     minidle;            //-0x7FFFFFFF
    Uint16  minburst;           // default:0
    Uint16  maxburst;           // default:5 0<=maxburst<6 because the 64bit limit in tc_cbq.c
    Uint8   bounded;            //  default:0
    Uint8   isolated;           //  default:0
    Uint8   ewma;               //  default:5
    short   cell;                //unit: byte   default:-1
    Uint8   prio;
    Uint16  mpu;                //unit:byte     default:0
    short   allot;               //unit: byte   default:0
    Uint16  avpkt;               //unit: byte
    Uint32  weight;
    char    split[MAX_CBQ_HANDLE_LEN+1];               //  default:NULL
    Uint32  defmap;             //  default:0
    Uint8 help;                 //default:0
    char    name[MAX_CBQ_NAME_LEN+1];
    Boolean active;  	 	//default: no
};

typedef struct cbqClassProfile CbqClassProfile;

struct sfqQdiscProfile{
    struct commQdiscProfile comm;
    int quantum;                //default: dev->mtu
    int perturb;                //default: 10  unit: second
    Uint8 help;                 //default::0
    char name[MAX_CBQ_NAME_LEN+1];
    Boolean active;  	 	//default: no
};

typedef struct sfqQdiscProfile SfqQdiscProfile;

struct cbqClassProfile * requestCbqClassProfile(int);
struct sfqQdiscProfile * requestSfqQdiscProfile(int);

enum matchtype {MATCH};         //当前支持match
enum prototype {p_ETHER,p_IP,p_UDP,p_TCP};  //当前支持ip,udp,tcp

#define NOFRAG 		0x1
#define FIRSTFRAG 	0x2
#define DF 		0x4
#define MF 		0x8

typedef struct {
/* pls keep these 3 items first */
    char    dev[MAX_CBQ_HANDLE_LEN+1];
    char    parent[MAX_CBQ_HANDLE_LEN+1];
    char    handle[MAX_CBQ_HANDLE_LEN+1];
/* ********************** */
    Uint8   priority;
    char    protocol[15];
    matchtype type1;      	//当前支持match
    prototype type2;        	//当前支持ip,udp,tcp
    Uchar   smac[6];          	//undefine
    Uchar   dmac[6];          	//undefine
    Uint32  sip;            	//default:0
    Uint32  sipmask;        	//default:0
    Uint32  dip;            	//default:0
    Uint32  dipmask;        	//default:0
    Uint16  sport;           	//default:0
    Uint16  spmask;         	//default:0
    Uint16  dport;           	//default:0
    Uint16  dpmask;         	//default:0
    Uint8   tos;        	//  default: ff
    Uint8   ihl;        	//  default: 0
    Uint8   ip_prot;    	//  default:0
    Uint8   t_frag;     	//  default:0
    Uint8   icmp_type;  	//  default:255
    Uint8   icmp_code;  	//  default:255
    char    flowid[MAX_CBQ_HANDLE_LEN+1];
    char    name[MAX_CBQ_NAME_LEN+1];
    Boolean active;  	 	//default: no
//
// zhz@20040428, added to support ip/port range
//
    Uint32  sipFrom;        //default:0
    Uint32  sipEnd;         //default:0
    Uint32  dipFrom;        //default:0
    Uint32  dipEnd;         //default:0
    Uint16 sportFrom;       //default:0
    Uint16 sportEnd;        //default:0
    Uint16 dportFrom;       //default:0
    Uint16 dportEnd;        //default:0
} CbqFilterProfile;


CbqFilterProfile * requestCbqFilterProfile(int);



