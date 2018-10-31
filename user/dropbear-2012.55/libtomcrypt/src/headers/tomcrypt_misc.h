/* ---- BASE64 Routines ---- */
#ifdef BASE64
int base64_encode(const unsigned char *in,  unsigned long len, 
                        unsigned char *out, unsigned long *outlen);

int base64_decode(const unsigned char *in,  unsigned long len, 
                        unsigned char *out, unsigned long *outlen);
#endif

/* ---- MEM routines ---- */
void zeromem(void *dst, size_t len);
void burn_stack(unsigned long len);

const char *error_to_string(int err);

extern const char *crypt_build_settings;

/* ---- HMM ---- */
int crypt_fsa(void *mp, ...);

/* $Source: /home/cvs/3520G/user/dropbear-2012.55/libtomcrypt/src/headers/tomcrypt_misc.h,v $ */
/* $Revision: 1.1 $ */
/* $Date: 2012/10/26 19:14:56 $ */
