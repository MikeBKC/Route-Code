//#include "flags.h"

#ifndef _IKE_MD5_H_
#define _IKE_MD5_H_
#include "uttMachine.h"
#if (UTT_LICENSE == FYES)
//#include "ike.h"

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

/* UINT2 defines a two byte word */
//typedef u_int16_t UINT2;

/* UINT4 defines a four byte word */
typedef unsigned int UINT4;

/* MD5.H - header file for MD5C.C
 */

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
 */

/* MD5 context. */
typedef struct {
  UINT4 state[4];                                   /* state (ABCD) */
  UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];                         /* input buffer */
} IkeMD5_CTX;

void IkeMD5Init (IkeMD5_CTX * context);
void IkeMD5Update (IkeMD5_CTX * context, const unsigned char * input, UINT4 inputLen);
void IkeMD5Final (unsigned char digest[16], IkeMD5_CTX * context);

#endif // _IKE_MD5_H_
#endif // MDF_IKE == FYES
