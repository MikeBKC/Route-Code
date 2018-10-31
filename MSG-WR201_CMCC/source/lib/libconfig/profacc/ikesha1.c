/*
SHA-1 in C
By Steve Reid <steve@edmweb.com>
100% Public Domain

Test Vectors (from FIPS PUB 180-1)
"abc"
  A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
  84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
A million repetitions of "a"
  34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
*/

/* #define LITTLE_ENDIAN * This should be #define'd already, if true. */
/* #define SHA1HANDSOFF * Copies data before messing with it. */
//#include "flags.h"
#include "uttMachine.h"
#if (UTT_LICENSE == FYES)

#define SHA1HANDSOFF

#include "string.h"

#include "ikesha1.h"

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

/* blk0() and blk() perform the initial expand. */
/* I got the idea of expanding during the round function from SSLeay */
#ifdef LIBCONFIG_LITTLE_ENDIAN
#define blk0(i) (block->l[i] = (rol(block->l[i],24)&0xFF00FF00) |(rol(block->l[i],8)&0x00FF00FF))
#elif defined(LIBCONFIG_BIG_ENDIAN)
#define blk0(i) block->l[i]
#else
#error "Endianness not defined!"
#endif
#define blk(i) (block->l[i&15] = rol(block->l[(i+13)&15]^block->l[(i+8)&15]^block->l[(i+2)&15]^block->l[i&15],1))

/* (RR0+RR1), RR2, RR3, RR4 are the different operations used in SHA1 */
#define RR0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define RR1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define RR2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define RR3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define RR4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);


/* Hash a single 512-bit block. This is the core of the algorithm. */

void IkeSHA1Transform(unsigned long state[5], const unsigned char buffer[64])
{
	unsigned long a, b, c, d, e;
	typedef union {
    	unsigned char c[64];
    	unsigned long l[16];
	} CHAR64LONG16;

#ifdef SHA1HANDSOFF
	CHAR64LONG16 block[1];  /* use array to appear as a pointer */
    memcpy(block, buffer, 64);
#else
    /* The following had better never be used because it causes the
     * pointer-to-const buffer to be cast into a pointer to non-const.
     * And the result is written through.  I threw a "const" in, hoping
     * this will cause a diagnostic.
     */
	CHAR64LONG16* block = (const CHAR64LONG16*)buffer;
#endif
    /* Copy context->state[] to working vars */
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];
    /* 4 rounds of 20 operations each. Loop unrolled. */
    RR0(a,b,c,d,e, 0); RR0(e,a,b,c,d, 1); RR0(d,e,a,b,c, 2); RR0(c,d,e,a,b, 3);
    RR0(b,c,d,e,a, 4); RR0(a,b,c,d,e, 5); RR0(e,a,b,c,d, 6); RR0(d,e,a,b,c, 7);
    RR0(c,d,e,a,b, 8); RR0(b,c,d,e,a, 9); RR0(a,b,c,d,e,10); RR0(e,a,b,c,d,11);
    RR0(d,e,a,b,c,12); RR0(c,d,e,a,b,13); RR0(b,c,d,e,a,14); RR0(a,b,c,d,e,15);
    RR1(e,a,b,c,d,16); RR1(d,e,a,b,c,17); RR1(c,d,e,a,b,18); RR1(b,c,d,e,a,19);
    RR2(a,b,c,d,e,20); RR2(e,a,b,c,d,21); RR2(d,e,a,b,c,22); RR2(c,d,e,a,b,23);
    RR2(b,c,d,e,a,24); RR2(a,b,c,d,e,25); RR2(e,a,b,c,d,26); RR2(d,e,a,b,c,27);
    RR2(c,d,e,a,b,28); RR2(b,c,d,e,a,29); RR2(a,b,c,d,e,30); RR2(e,a,b,c,d,31);
    RR2(d,e,a,b,c,32); RR2(c,d,e,a,b,33); RR2(b,c,d,e,a,34); RR2(a,b,c,d,e,35);
    RR2(e,a,b,c,d,36); RR2(d,e,a,b,c,37); RR2(c,d,e,a,b,38); RR2(b,c,d,e,a,39);
    RR3(a,b,c,d,e,40); RR3(e,a,b,c,d,41); RR3(d,e,a,b,c,42); RR3(c,d,e,a,b,43);
    RR3(b,c,d,e,a,44); RR3(a,b,c,d,e,45); RR3(e,a,b,c,d,46); RR3(d,e,a,b,c,47);
    RR3(c,d,e,a,b,48); RR3(b,c,d,e,a,49); RR3(a,b,c,d,e,50); RR3(e,a,b,c,d,51);
    RR3(d,e,a,b,c,52); RR3(c,d,e,a,b,53); RR3(b,c,d,e,a,54); RR3(a,b,c,d,e,55);
    RR3(e,a,b,c,d,56); RR3(d,e,a,b,c,57); RR3(c,d,e,a,b,58); RR3(b,c,d,e,a,59);
    RR4(a,b,c,d,e,60); RR4(e,a,b,c,d,61); RR4(d,e,a,b,c,62); RR4(c,d,e,a,b,63);
    RR4(b,c,d,e,a,64); RR4(a,b,c,d,e,65); RR4(e,a,b,c,d,66); RR4(d,e,a,b,c,67);
    RR4(c,d,e,a,b,68); RR4(b,c,d,e,a,69); RR4(a,b,c,d,e,70); RR4(e,a,b,c,d,71);
    RR4(d,e,a,b,c,72); RR4(c,d,e,a,b,73); RR4(b,c,d,e,a,74); RR4(a,b,c,d,e,75);
    RR4(e,a,b,c,d,76); RR4(d,e,a,b,c,77); RR4(c,d,e,a,b,78); RR4(b,c,d,e,a,79);
    /* Add the working vars back into context.state[] */
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    /* Wipe variables */
    a = b = c = d = e = 0;
#ifdef SHA1HANDSOFF
    memset(block, '\0', sizeof(block));
#endif
}


/* SHA1Init - Initialize new context */

void IkeSHA1Init(IkeSHA1_CTX* context)
{
    /* SHA1 initialization constants */
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
    context->state[4] = 0xC3D2E1F0;
    context->count[0] = context->count[1] = 0;
}


/* Run your data through this. */

void IkeSHA1Update(IkeSHA1_CTX* context, const unsigned char* data, unsigned long len)
{
	unsigned long i;
	unsigned long j;

    j = context->count[0];
    if ((context->count[0] += len << 3) < j)
	context->count[1]++;
    context->count[1] += (len>>29);
    j = (j >> 3) & 63;
    if ((j + len) > 63) 
    {
        memcpy(&context->buffer[j], data, (i = 64-j));
        IkeSHA1Transform(context->state, context->buffer);
        for ( ; i + 63 < len; i += 64) 
        {
            IkeSHA1Transform(context->state, &data[i]);
        }
        j = 0;
    }
    else i = 0;
    memcpy(&context->buffer[j], &data[i], len - i);
}


/* Add padding and return the message digest. */

void IkeSHA1Final(unsigned char digest[20], IkeSHA1_CTX* context)
{
	unsigned i;
	unsigned char finalcount[8];
	unsigned char c;

    for (i = 0; i < 8; i++) {
        finalcount[i] = (unsigned char)((context->count[(i >= 4 ? 0 : 1)]
         >> ((3-(i & 3)) * 8) ) & 255);  /* Endian independent */
    }

    c = 0200;
    IkeSHA1Update(context, &c, 1);
    while ((context->count[0] & 504) != 448) {
		c = 0000;
        IkeSHA1Update(context, &c, 1);
    }
    IkeSHA1Update(context, finalcount, 8);  /* Should cause a SHA1Transform() */
    for (i = 0; i < 20; i++) {
        digest[i] = (unsigned char)((context->state[i>>2] >> ((3-(i & 3)) * 8) ) & 255);
    }
    /* Wipe variables */
    memset(context, '\0', sizeof(*context));
    memset(&finalcount, '\0', sizeof(finalcount));
}

#endif // MDF_IKE == FYES
