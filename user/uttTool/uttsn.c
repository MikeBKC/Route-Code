

#include <stdlib.h>
#include <stdio.h>

void usage()
{
    printf("example: sn 1234567 \n \
            the biggest sn is 4*1024*1024*1024\n \
            ");
}

int main(int argc, char** argv)
{
    unsigned int sn = 0;
    unsigned short tmpsn[2] = {0};
    if(argc > 2) {
        usage();
    }
    flash_read_mtdname("Bootloader", &sn, 0x2c, 4);
    if(argc == 1) {
        printf("serio No. %d\n", sn);
        exit(0);
    }
    if(sn != 0xffffffff) {
        printf("the machine has a serio No. %d, you need upgrade boot\n", sn);
        exit(0);
    }
    sn = strtol(argv[1], NULL, 10);
    tmpsn[0] = sn & 0xffff;
    tmpsn[1] = sn >> 16;
    flash_write_mtdname("Bootloader", tmpsn, 0x2c, 4);
    exit(0);
}
