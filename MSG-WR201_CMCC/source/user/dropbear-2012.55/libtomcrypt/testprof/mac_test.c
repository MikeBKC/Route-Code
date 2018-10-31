/* test pmac/omac/hmac */
#include <tomcrypt_test.h>

int mac_test(void)
{
#ifdef LTC_HMAC
   DO(hmac_test()); 
#endif
#ifdef LTC_PMAC
   DO(pmac_test()); 
#endif
#ifdef LTC_OMAC
   DO(omac_test()); 
#endif
#ifdef LTC_XCBC
   DO(xcbc_test());
#endif
#ifdef LTC_F9_MODE
   DO(f9_test());
#endif
#ifdef EAX_MODE
   DO(eax_test());  
#endif
#ifdef OCB_MODE
   DO(ocb_test());  
#endif
#ifdef CCM_MODE
   DO(ccm_test());
#endif
#ifdef GCM_MODE
   DO(gcm_test());
#endif
#ifdef PELICAN
   DO(pelican_test());
#endif
   return 0;
}

/* $Source: /home/cvs/3520G/user/dropbear-2012.55/libtomcrypt/testprof/mac_test.c,v $ */
/* $Revision: 1.1 $ */
/* $Date: 2012/10/26 19:18:51 $ */
