#ifndef _SIGNATURE_H
#define _SIGNATURE_H
/*
 * the signatureof wifidog's parameter to verify
 *
 * */

#include <linux/skbuff.h>
#include "sha1.h"

#define SECRET_KEY_VALUE "1623`@wifi"
#if 0
unsigned char sha1InData[1024];
unsigned char sha1Data[21];
unsigned char hexData[41];
#endif

#define SIGN_PARAM_NAME_LEN 31
#define SIGN_PARAM_VALUE_LEN 512
extern  int wifi_debug_print(const char* fmt, ...);
/*
 * param    - 
 * value    -
 * mark	    - 
 * */
typedef struct sign_t {
    char param[SIGN_PARAM_NAME_LEN];
    char value[SIGN_PARAM_VALUE_LEN];
    int mark;
} sign; 

extern void initParamval(void);
extern int addParamValue(char *param, char *value);
char * set_secret_key(char * value);
#if 0
extern int getSha1Result(void);
#else
extern int getSha1Result(char *outData);
#endif

static unsigned char isAcceptable[96] =

/* Overencodes */
#define URL_XALPHAS     (unsigned char) 1
#define URL_XPALPHAS    (unsigned char) 2

/*      Bit 0           xalpha          -- see HTFile.h
**      Bit 1           xpalpha         -- as xalpha but with plus.
**      Bit 2 ...       path            -- as xpalpha but with /
*/
    /*   0 1 2 3 4 5 6 7 8 9 A B C D E F */
    {    7,0,0,0,0,0,0,0,0,0,7,0,0,7,7,7,       /* 2x   !"#$%&'()*+,-./ */
         7,7,7,7,7,7,7,7,7,7,0,0,0,0,0,0,       /* 3x  0123456789:;<=>?  */
         7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,       /* 4x  @ABCDEFGHIJKLMNO */
         7,7,7,7,7,7,7,7,7,7,7,0,0,0,0,7,       /* 5X  PQRSTUVWXYZ[\]^_ */
         0,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,       /* 6x  `abcdefghijklmno */
         7,7,7,7,7,7,7,7,7,7,7,0,0,0,0,0 };     /* 7X  pqrstuvwxyz{\}~ DEL */
 
#define ACCEPTABLE(a)   ( a>=32 && a<128 && ((isAcceptable[a-32]) & mask))
static char *hex_wifi = "0123456789ABCDEF";
char *httpdUrlEncode(const char *str);

#endif /*_SIGNATURE_H*/
