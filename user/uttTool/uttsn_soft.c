

#include <stdlib.h>
#include <stdio.h>
#include "hmac_md5.h"
#include <unistd.h>

#define MD5_SN_ADDR 0x10

/*hmd5µÄÃÜÔ¿*/
static char IDDATA[4]={0x2d, 0x4b, 0x68, 0x3a};/*510v5*/
//static char IDDATA[4]={0x2c, 0x4b, 0x68, 0x3a};/*510v4*/

void usage()
{
    printf("example: sn 1234567 \n \
            the biggest sn is 4*1024*1024*1024\n \
            ");
}

int main(int argc, char** argv)
{
    unsigned int sn = 0;

    unsigned int md5_sn = 0;
    unsigned char md5_be_sn[4];
    unsigned char md5_af_sn[4];

    unsigned short tmpsn[2] = {0};


    if(argc > 2) {
        usage();
    }
    flash_read_mtdname("Bootloader", &sn, 0x2c, 4);
    if(argc == 1) {
//	flash_read_mtdname("Bootloader", &md5_sn, 0x10, 4);
        printf("serio No. %d\n", sn);
   //     printf("md5 serio No. %d\n", md5_sn);
        exit(0);
    }
#if 1
    if(sn != 0xffffffff) {
        printf("the machine has a serio No. %d, you need upgrade boot\n", sn);
  //      printf("the machine has a md5 sn serio No. %d, you need upgrade boot\n", md5_sn);
        exit(0);
    }
#endif
    memset(md5_be_sn,0,sizeof(md5_be_sn));
    memset(md5_af_sn,0,sizeof(md5_af_sn));

    sn = strtol(argv[1], NULL, 10);
    tmpsn[0] = sn & 0xffff;
    tmpsn[1] = sn >> 16;
    printf("write sn = %d\n",sn);
    flash_write_mtdname("Bootloader", tmpsn, 0x2c, 4);

    usleep(200);
    memcpy(md5_be_sn,&sn,4);
    hmac_md5((char*)(IDDATA), 4, (unsigned char*)md5_be_sn, 4,md5_af_sn);
    memcpy(&md5_sn,md5_af_sn,4);
    
    tmpsn[0] = md5_sn & 0xffff;
    tmpsn[1] = md5_sn >> 16;
    flash_write_mtdname("Bootloader", tmpsn, 0x10, 4);

    exit(0);
}
