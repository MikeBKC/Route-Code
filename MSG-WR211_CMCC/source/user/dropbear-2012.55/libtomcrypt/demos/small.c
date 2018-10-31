/* small demo app that just includes a cipher/hash/prng */
#include <tomcrypt.h>

int main(void)
{
   register_cipher(&rijndael_enc_desc);
   register_prng(&yarrow_desc);
   register_hash(&sha256_desc);
   return 0;
}

/* $Source: /home/cvs/3520G/user/dropbear-2012.55/libtomcrypt/demos/small.c,v $ */
/* $Revision: 1.1 $ */
/* $Date: 2012/10/26 19:19:21 $ */
