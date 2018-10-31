#include <tomcrypt_test.h>

void run_cmd(int res, int line, char *file, char *cmd)
{
   if (res != CRYPT_OK) {
      fprintf(stderr, "%s (%d)\n%s:%d:%s\n", error_to_string(res), res, file, line, cmd);
      if (res != CRYPT_NOP) {
         exit(EXIT_FAILURE);
      }
   }
}

/* $Source: /home/cvs/3520G/user/dropbear-2012.55/libtomcrypt/testprof/test_driver.c,v $ */
/* $Revision: 1.1 $ */
/* $Date: 2012/10/26 19:18:40 $ */
