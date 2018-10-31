
//#include "flags.h"
//#include "gendef.h"

//#include "snmpv3_key.h"
#include "ikemd5.h"
#include "ikesha1.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "uttMachine.h"
#if( UTT_LICENSE == FYES )
#define SNMPV3_KEY_DEBUG
#undef  SNMPV3_KEY_DEBUG

#if defined SNMPV3_KEY_DEBUG
#define DEBUG(x) printf x
#else
#define DEBUG(x) ;
#endif

#ifndef QUITFUN
//#undef QUITFUN
//edit by brwang rval = SNMP_ERR_SC_GENERAL_FAILURE;  del _
#define QUITFUN(e, l)                   \
        if (e != SNMP_OK) {         \
            rval = SNMP_ERR_SC_GENERAL_FAILURE;  \
            goto l ;                \
                }
#endif
#if defined(CONFIG_BOARD_MTK7620_E) || defined(CONFIG_BOARD_MTK7620_EN)
#define u_char unsigned char
#define u_int unsigned int 
#define u_long unsigned long
#else
typedef unsigned char u_char;
typedef unsigned int u_int;
typedef unsigned long u_long;
#endif
void password_to_key_md5( unsigned char *password, unsigned int passwordlen, unsigned char *engineID, unsigned int engineLength, unsigned char *key)
{      IkeMD5_CTX     MD; 
       u_char     *cp, password_buf[64];   
       u_long      password_index = 0; 
       u_long      count = 0, i;    
       IkeMD5Init (&MD);   /* initialize MD5 */ 
       /**********************************************/      
       /* Use while loop until we've done 1 Megabyte */  
       /**********************************************/     
       while (count < 1048576) {         
           cp = password_buf;        
                                                                                                                                               for (i = 0; i < 64; i++)
                                                                                                                                               {  
            /*************************************************/     
            /* Take the next octet of the password, wrapping */           
            /* to the beginning of the password as necessary.*/          
            /*************************************************/        
            *cp++ = password[password_index++ % passwordlen]; 
                                                                                                                                               }
           IkeMD5Update (&MD, password_buf, 64);      
           count += 64;    
       }     
       IkeMD5Final (key, &MD);         
#if defined SNMPV3_KEY_DEBUG
        DEBUG(("authKey before localized: \n"));
        for (int i = 0; i < 16; i++)
            DEBUG(("%x",*(key+i)));
        DEBUG(("\n"));
#endif
       /* tell MD5 we're done */ 
       /*****************************************************/   
       /* Now localize the key with the engineID and pass   */    
       /* through MD5 to produce final key                  */    
       /* May want to ensure that engineLength <= 32,       */   
       /* otherwise need to use a buffer larger than 64     */ 
       /*****************************************************/    
       memcpy(password_buf, key, 16);     
       memcpy(password_buf+16, engineID, engineLength);  
       memcpy(password_buf+16+engineLength, key, 16);  
       IkeMD5Init(&MD);     
       IkeMD5Update(&MD, password_buf, 32+engineLength);   
       IkeMD5Final(key, &MD);    
#if defined SNMPV3_KEY_DEBUG
        DEBUG(("engineID used to localize authKey is :"));
        for (int j = 0; j < 12; j++)
            DEBUG(("%x",*(engineID+j)));
        DEBUG(("\n"));

        DEBUG(("final output of the authKey is :"));
        for (int k = 0; k < 16; k++)
            DEBUG(("%x",*(key+k)));
        DEBUG(("\n"));
#endif
       return;
}


void password_to_key_sha1( u_char *password,    /* IN */    
                          u_int   passwordlen, /* IN */    
                          u_char *engineID,    /* IN  - pointer to snmpEngineID  */    
                          u_int   engineLength,/* IN  - length of snmpEngineID */    
                          u_char *key)         /* OUT - pointer to caller 16-octet buffer */ 
{      IkeSHA1_CTX     MD; 
       u_char     *cp, password_buf[64];   
       u_long      password_index = 0; 
       u_long      count = 0, i;    
       IkeSHA1Init (&MD);   /* initialize MD5 */ 
       /**********************************************/      
       /* Use while loop until we've done 1 Megabyte */  
       /**********************************************/     
       while (count < 1048576) {         
           cp = password_buf;        
                                                                                                                                               for (i = 0; i < 64; i++)
                                                                                                                                               {  
            /*************************************************/     
            /* Take the next octet of the password, wrapping */           
            /* to the beginning of the password as necessary.*/          
            /*************************************************/        
            *cp++ = password[password_index++ % passwordlen]; 
                                                                                                                                               }
           IkeSHA1Update (&MD, password_buf, 64);      
           count += 64;    
       }     
       IkeSHA1Final (key, &MD);         
#if defined SNMPV3_KEY_DEBUG
        DEBUG(("authKey before localized: \n"));
        for (int i = 0; i < 20; i++)
            DEBUG(("%x",*(key+i)));
        DEBUG(("\n"));
#endif
       /* tell MD5 we're done */ 
       /*****************************************************/   
       /* Now localize the key with the engineID and pass   */    
       /* through MD5 to produce final key                  */    
       /* May want to ensure that engineLength <= 32,       */   
       /* otherwise need to use a buffer larger than 64     */ 
       /*****************************************************/    
       memcpy(password_buf, key, 20);     
       memcpy(password_buf+20, engineID, engineLength);  
       memcpy(password_buf+20+engineLength, key, 20);  
       IkeSHA1Init(&MD);     
       IkeSHA1Update(&MD, password_buf, 40+engineLength);   
       IkeSHA1Final(key, &MD);    
#if defined SNMPV3_KEY_DEBUG
        DEBUG(("engineID used to localize authKey is :"));
        for (int j = 0; j < 12; j++)
            DEBUG(("%x",*(engineID+j)));
        DEBUG(("\n"));

        DEBUG(("final output of the authKey is :"));
        for (int k = 0; k < 20; k++)
            DEBUG(("%x",*(key+k)));
        DEBUG(("\n"));
#endif
       return;
}

/*
 *
 * Function: hmac_md5
 */
void
hmac_md5(
    unsigned char*  text,                /* pointer to data stream */
    int             text_len,            /* length of data stream */
    unsigned char*  key,                 /* pointer to authentication key */
    int             key_len,             /* length of authentication key */
    u_char*         digest)              /* caller digest to be filled in */
{     
    IkeMD5_CTX context;      
    unsigned char k_ipad[65];    /* inner padding -               
                                  * key XORd with ipad  */       
    unsigned char k_opad[65];    /* outer padding -  
                                  * key XORd with opad    */   
    unsigned char tk[16];        
    int i;        /* if key is longer than 64 bytes reset it to key=MD5(key) */       
    if (key_len > 64) 
    {             
        IkeMD5_CTX      tctx;            
        IkeMD5Init(&tctx);             
        IkeMD5Update(&tctx, key, key_len);   
        IkeMD5Final(tk, &tctx);            
        key = tk;              
        key_len = 16;     
    }        
    /*         
     * the HMAC_MD5 transform looks like:         
     *         
     * MD5(K XOR opad, MD5(K XOR ipad, text))         
     * where K is an n byte key         
     * ipad is the byte 0x36 repeated 64 timesKrawczyk,et.al.                                
     * opad is the byte 0x5c repeated 64 times         
     * and text is the data being protected         
     */        
    /* start out by storing key in pads */        
    memset( k_ipad,0, sizeof(k_ipad));        
    memset( k_opad,0, sizeof(k_opad));
    memcpy(k_ipad, key,key_len);
    memcpy(k_opad,key,key_len);
    /* XOR key with ipad and opad values */        
    for (i=0; i<64; i++) 
    {                
        k_ipad[i] ^= 0x36;                
        k_opad[i] ^= 0x5c;        
    }        
    /*         
     * perform inner MD5         
     */        
    IkeMD5Init(&context);                   
    /* init context for 1st                                              
     * pass 
     */        
    IkeMD5Update(&context, k_ipad, 64);     
    /* start with inner pad 
     */        
    IkeMD5Update(&context, text, text_len); 
    /* then text of datagram */        
    IkeMD5Final(digest, &context);          
    /* finish up 1st pass */        

    /*         
     * perform outer MD5         
     */        
    IkeMD5Init(&context);                   
    /* init context for 2nd                                              
     * pass 
     */        
    IkeMD5Update(&context, k_opad, 64);     
    /* start with outer pad */        
    IkeMD5Update(&context, digest, 16);     
    /* then results of 1st                                              
     * hash 
     */           
    IkeMD5Final(digest, &context);          
    /* finish up 2nd pass */
}

void
hmac_sha1(
    unsigned char*  text,                /* pointer to data stream */
    int             text_len,            /* length of data stream */
    unsigned char*  key,                 /* pointer to authentication key */
    int             key_len,             /* length of authentication key */
    u_char*         digest)              /* caller digest to be filled in */
{     
    IkeSHA1_CTX context;      
    unsigned char k_ipad[65];    /* inner padding -               
                                  * key XORd with ipad  */       
    unsigned char k_opad[65];    /* outer padding -  
                                  * key XORd with opad    */   
    unsigned char tk[16];        
    int i;        /* if key is longer than 64 bytes reset it to key=MD5(key) */       
    if (key_len > 64) 
    {             
        IkeMD5_CTX      tctx;            
        IkeMD5Init(&tctx);             
        IkeMD5Update(&tctx, key, key_len);   
        IkeMD5Final(tk, &tctx);            
        key = tk;              
        key_len = 16;     
    }        
    /*         
     * the HMAC_MD5 transform looks like:         
     *         
     * MD5(K XOR opad, MD5(K XOR ipad, text))         
     * where K is an n byte key         
     * ipad is the byte 0x36 repeated 64 timesKrawczyk,et.al.                                
     * opad is the byte 0x5c repeated 64 times         
     * and text is the data being protected         
     */        
    /* start out by storing key in pads */        
    memset( k_ipad,0, sizeof(k_ipad));        
    memset( k_opad,0, sizeof(k_opad));
    memcpy(k_ipad, key,key_len);
    memcpy(k_opad,key,key_len);
    /* XOR key with ipad and opad values */        
    for (i=0; i<64; i++) 
    {                
        k_ipad[i] ^= 0x36;                
        k_opad[i] ^= 0x5c;        
    }        
    /*         
     * perform inner MD5         
     */        
    IkeSHA1Init(&context);                   
    /* init context for 1st                                              
     * pass 
     */        
    IkeSHA1Update(&context, k_ipad, 64);     
    /* start with inner pad 
     */        
    IkeSHA1Update(&context, text, text_len); 
    /* then text of datagram */        
    IkeSHA1Final(digest, &context);          
    /* finish up 1st pass */        

    /*         
     * perform outer MD5         
     */        
    IkeSHA1Init(&context);                   
    /* init context for 2nd                                              
     * pass 
     */        
    IkeSHA1Update(&context, k_opad, 64);     
    /* start with outer pad */        
    IkeSHA1Update(&context, digest, 20);     
    /* then results of 1st                                              
     * hash 
     */           
    IkeSHA1Final(digest, &context);          
    /* finish up 2nd pass */
}

#if (1)
/*·¢²¼ÃÜÔ¿*/
char	utt_DEFGPartKey[10] = { 0xf9, 0x92, 0xaa, 0xcd, 0x99, 0xfc, 0xd3, 0xb9, 0x23, 0xdf };
char   UTTKEY[10] = { 0x10, 0x32, 0x49, 0x3d, 0x12, 0x10, 0x79, 0xd9,0x98,0x6d };
#else
/*²âÊÔÃÜÔ¿*/
char	utt_DEFGPartKey[10] = { 0x89, 0x12, 0xac, 0xb1, 0x09, 0xfa, 0x53, 0x96, 0x5f, 0x33 };
char	UTTKEY[10] = { 0x12, 0x33, 0x44, 0x34, 0x12, 0x19, 0x76, 0x89,0x98,0x67 };
#endif


void cryptoLicenseDEFGPart(char *randomKey, int keyLen, 
        char* licenseData, int licenseLen)
{
    char *tmpKey;
    int tmpL = 0;
    int i = 0, j = 0;

    tmpKey = (char*)malloc(sizeof(char)*keyLen);

    for(i = keyLen - 1; i > 0; i--) {
        tmpKey[i] = randomKey[i] ^ randomKey[i-1];
    }
    tmpKey[0] = randomKey[0];

    for(i = 0; i < keyLen && i < 10; i++) {
        tmpKey[i] += randomKey[i] + utt_DEFGPartKey[i];
    }

    tmpL = keyLen/2;
    for(i = 0; i < tmpL; i++) {
        tmpKey[2*i] ^= tmpKey[2*i+1];
    }

    while(licenseLen >= keyLen) {
        for(i = 0; i < keyLen; i++) {
            licenseData[i+j*keyLen] ^= tmpKey[i];
        }
        j++;
        licenseLen -= keyLen;
    }

    for(i = 0; i < licenseLen; i++) {
        licenseData[i+j*keyLen] ^= tmpKey[i];
    }
    return;
}

void decryptoLicenseDEFGPart(char *randomKey, int keyLen, 
        char* licenseData, int licenseLen)
{
    cryptoLicenseDEFGPart(randomKey, keyLen, licenseData, licenseLen);
    return;
}

#endif
