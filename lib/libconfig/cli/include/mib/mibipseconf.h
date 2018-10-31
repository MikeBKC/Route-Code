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
 * 
 * File:  
 * 
 * Overview:
 *  
 *
*/

#if ! defined ( _MIBIPSECONF_H )
#define _MIBIPSECONF_H

#include "mibglob.h"

#if (IP_SEC_AES256 != FNO)
#define MAX_KEY_LENGTH		 	66   /* for AES256 ,need 32 bytes */
#else
#define MAX_KEY_LENGTH		 	50   /* for tri_des ,need 24 bytes */
#endif

 /*it should be equal or more than 1+MAX_IPSECONF_PROFILES*/
#define MAX_IPSECFILTER_COUNT           (MAX_IPSECONF_PROFILES+1)

#define MAX_TRANSFORMSETS_LENGTH	128
#define MAX_ISAKMPPOLICY_LENGTH		128
#define MTU_IPSEC_LAYER			1400
typedef enum IpsecMethod {
	Manual=0,
	Isakmp
}IpsecMethod;

typedef enum IpsecMode {
	Tunnel=0,
	Transport
}IpsecMode;

typedef enum EncryptWay {
	Des = 1,
	Tri_Des,
	Aes,
	Rc4
#if (IP_SEC_AES192 != FNO)
	,Aes192,
#endif
#if (IP_SEC_AES256 != FNO)
	Aes256
#endif
}EncryptWay;

typedef enum AuthWay {
	Md5 = 1,
	Sha
}AuthWay;

typedef struct ipseconfProfile {
	char 			name[MAX_PROFILE_NAME_LENGTH + 1];
	Boolean   		active;
        Boolean                 usable;
	IpsecMethod  		method;
	IpsecMode 		mode;
//	char			peer[MAX_HOST_NAME_LENGTH + 1];
	char			peer[MAX_DOMAIN_NAME_LENGTH + 1];
	char			infilter[MAX_PROFILE_NAME_LENGTH + 1];
	char			outfilter[MAX_PROFILE_NAME_LENGTH + 1];
	char    		transformSet[MAX_TRANSFORMSETS_LENGTH+1];
//	char                    tunnelSrc[MAX_HOST_NAME_LENGTH + 1];
 //       char                    tunnelDest[MAX_HOST_NAME_LENGTH + 1];
        char                    profile[MAX_PROFILE_NAME_LENGTH + 1];
	char     		encryptKey[MAX_KEY_LENGTH];
	char      		espAuthKey[MAX_KEY_LENGTH];	
	char    		authKey[MAX_KEY_LENGTH];
	Uint32     		espInSpi;
	Uint32     		espOutSpi;
	Uint32	    		ahInSpi;
	Uint32     		ahOutSpi;
	Uint16			mtu;
	char			isakmprofile[MAX_PROFILE_NAME_LENGTH + 1];
	Uint32                  lifetime_secs;
        Uint32                  lifetime_kbytes;
	Uint8			anti_replay;
	Uint8			nat_keepalive;
	Uint8			dpd_heartbeat;
	Uint8			dpd_timeout;
	Uint8			dpd_active;;
}IPSecConfProfile;


typedef struct ipsecTransformSet {
	char name[MAX_PROFILE_NAME_LENGTH + 1 ];
	Boolean   	active;
	EncryptWay	encrypt;
	AuthWay		espAuth;
	AuthWay		auth;
}IPSecTransformSetProfile;

typedef enum negotmode {
	Mainmode,
	Aggresmode
}negotmode;
typedef enum grouptype {
	Group1=1,
	Group2,
	Group5
}grouptype;
typedef enum authenmethod {
	pre_share=1,
	rsa_encr,
	rsa_sig
}authenmethod;
#define MAX_PRESHAREKEY_LENGTH 128
typedef struct isakmpConfig {
	char 	name[MAX_PROFILE_NAME_LENGTH + 1 ];
	Boolean active;
	negotmode mode;
	char	presharekey[MAX_PRESHAREKEY_LENGTH+1];
	char	policy[MAX_ISAKMPPOLICY_LENGTH+1];
	char	localuser[MAX_HOST_NAME_LENGTH+1];
	char	remoteuser[MAX_HOST_NAME_LENGTH+1];
	Uint32	lifetime;
	Uint8	aggre_I2_encrypt;
}IsakmpConfProfile;
typedef struct isakmpPolicy{
	char 		name[MAX_PROFILE_NAME_LENGTH + 1 ];
	Boolean   	active;
	authenmethod	authentication;
	EncryptWay	encrypt;
	AuthWay		hash;
	grouptype	group;
}IsakmpPolicyProfile;

typedef struct ipsecManual {
        char                    name[MAX_PROFILE_NAME_LENGTH + 1];
	Boolean   		active;
        Boolean                 usable;
        IpsecMethod             method;
        IpsecMode               mode;
	char                    infilter[MAX_PROFILE_NAME_LENGTH + 1];
        char                    outfilter[MAX_PROFILE_NAME_LENGTH + 1];
        char                    tunnelSrc[MAX_HOST_NAME_LENGTH + 1];
        char                    tunnelDest[MAX_HOST_NAME_LENGTH + 1];
	EncryptWay      	encrypt;
        AuthWay         	espAuth;
        AuthWay         	auth;
        char                    encryptKey[MAX_KEY_LENGTH];
        char                    espAuthKey[MAX_KEY_LENGTH];
        char                    authKey[MAX_KEY_LENGTH];
        Uint32                  espInSpi;
        Uint32                  espOutSpi;
        Uint32                  ahInSpi;
        Uint32                  ahOutSpi;
	//Uint32          	lifetime_secs;
        //Uint32          	lifetime_kbytes;
	Uint16			mtu;
        char                    profile[MAX_PROFILE_NAME_LENGTH + 1];
	Uint8			anti_replay;
}IPSecManual;
typedef struct saByIsakmp {
        IpsecMode               mode;
	int			direction; //0----out,1----in
        Uint32                  tunnelSrc;
        Uint32                  tunnelDest;
        EncryptWay              encrypt;
        AuthWay                 espAuth;
        AuthWay                 auth;
        char                    encryptKey[MAX_KEY_LENGTH];
        char                    espAuthKey[MAX_KEY_LENGTH];
        char                    authKey[MAX_KEY_LENGTH];
	int                     encrKeyLen;
        int                     espAuthKeyLen;
        int                     authKeyLen;
        Uint32                  espSpi;
        Uint32                  ahSpi;
        Uint32                  lifetime_secs;
        Uint32                  lifetime_kbytes;
}SaByIsakmp;
#endif /* _MIBIPSECONF_H */
