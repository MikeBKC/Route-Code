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
 * SCCS ID:	$Id: hmacmd5.h,v 1.1.1.1 2011/08/17 11:10:51 brwang Exp $
 * 
 * File:  
 * 
 * Overview:
 *  
 *
*/




#ifndef HMACMD5_H
#define HMACMD5_H 1


#include "md5.h"

#define ISC_HMACMD5_KEYLENGTH 64

typedef struct {
	MD5_CTX md5ctx;
	unsigned char key[ISC_HMACMD5_KEYLENGTH];
} hmacmd5_t;


void hmacmd5_init(hmacmd5_t *hmacmd5_ctx, const unsigned char *key,
		 unsigned int len);
void hmacmd5_update(hmacmd5_t *hmacmd5_ctx, const unsigned char *buf,
		   unsigned int len);
void hmacmd5_sign(hmacmd5_t *hmacmd5_ctx, unsigned char *digest) ;
int hmacmd5_verify(hmacmd5_t *hmacmd5_ctx, unsigned char *digest);
void hmacmd5_invalidate(hmacmd5_t *hmacmd5_ctx) ;


#endif   //HMACMD5_H
