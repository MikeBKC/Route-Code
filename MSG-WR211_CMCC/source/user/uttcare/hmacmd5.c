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
 * SCCS ID:	$Id: hmacmd5.c,v 1.2.12.2 2015/12/09 11:41:02 cao.yongxiang Exp $
 * 
 * File:  
 * 
 * Overview:
 *  
 *
*/




#include "string.h"
#include "hmacmd5.h"
#include "ddns.h"


#define PADLEN 64
#define IPAD 0x36
#define OPAD 0x5C

void hmacmd5_init(hmacmd5_t *hmacmd5_ctx, const unsigned char *key,
		 unsigned int len)
{
	unsigned char ipad[PADLEN];
	int i;

	memset(hmacmd5_ctx, 0, sizeof (hmacmd5_t));
	if (len > sizeof(hmacmd5_ctx->key)) {
		MD5_CTX md5ctx;
		MD5Init(&md5ctx);
		MD5Update(&md5ctx, key, len);
		MD5Final(hmacmd5_ctx->key,&md5ctx);
		//len = MD5_DIGEST_LEN;		
	} 
	else{
		memcpy(hmacmd5_ctx->key, key, len);
	}

	MD5Init(&hmacmd5_ctx->md5ctx);
	memset(ipad, IPAD, sizeof (ipad));
	for (i = 0; i < PADLEN; i++)
		ipad[i] ^= hmacmd5_ctx->key[i];
	MD5Update(&hmacmd5_ctx->md5ctx, ipad, sizeof(ipad));
}		 

/*
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */
void
hmacmd5_update(hmacmd5_t *hmacmd5_ctx, const unsigned char *buf,
		   unsigned int len)
{
	MD5Update(&hmacmd5_ctx->md5ctx, buf, len);
}

void
hmacmd5_invalidate(hmacmd5_t *hmacmd5_ctx) {	
	memset(hmacmd5_ctx, 0, sizeof (hmacmd5_ctx));	
}		

/*
 * Compute signature - finalize MD5 operation and reapply MD5.
 */
void
hmacmd5_sign(hmacmd5_t *hmacmd5_ctx, unsigned char *digest) {
	unsigned char opad[PADLEN];
	int i;

	MD5Final(digest,&hmacmd5_ctx->md5ctx);

	memset(opad, OPAD, sizeof (opad));
	for (i = 0; i < PADLEN; i++)
		opad[i] ^= hmacmd5_ctx->key[i];

	MD5Init(&hmacmd5_ctx->md5ctx);
	MD5Update(&hmacmd5_ctx->md5ctx, opad, sizeof(opad));
	MD5Update(&hmacmd5_ctx->md5ctx, digest, MD5_DIGEST_LEN);
	MD5Final(digest, &hmacmd5_ctx->md5ctx );
	hmacmd5_invalidate(hmacmd5_ctx);
}

/*
 * Verify signature - finalize MD5 operation and reapply MD5, then
 * compare to the supplied digest.
 */
int
hmacmd5_verify(hmacmd5_t *hmacmd5_ctx, unsigned char *digest) {
	unsigned char newdigest[MD5_DIGEST_LEN];

	hmacmd5_sign(hmacmd5_ctx, newdigest);
	return ((memcmp(digest, newdigest, MD5_DIGEST_LEN) == 0));
}
		 
