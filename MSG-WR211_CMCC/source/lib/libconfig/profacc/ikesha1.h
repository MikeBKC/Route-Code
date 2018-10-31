/*
SHA-1 in C
By Steve Reid <steve@edmweb.com>
100% Public Domain
*/
//#include "flags.h"

#ifndef _IKE_SHA1_H_
#define _IKE_SHA1_H_
#include "uttMachine.h"
#if (UTT_LICENSE == FYES)
//#include "ike.h"

typedef struct {
    unsigned long state[5];
    unsigned long count[2];
    unsigned char buffer[64];
} IkeSHA1_CTX;

void IkeSHA1Transform(unsigned long state[5], const unsigned char buffer[64]);
void IkeSHA1Init(IkeSHA1_CTX* context);
void IkeSHA1Update(IkeSHA1_CTX* context, const unsigned char* data, unsigned long len);
void IkeSHA1Final(unsigned char digest[20], IkeSHA1_CTX* context);

#endif
#endif
