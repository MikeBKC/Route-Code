

#include <stdlib.h>
#include <stdio.h>

void usage()
{
    printf("example: fc 0xb000\n \
            the biggest fc is 0xffff\n \
            ");
}

int main(int argc, char** argv)
{
    unsigned short fc = 0;

//    unsigned short tmpsn[2] = {0};
    if(argc > 2) {
        usage();
    }
    flash_read_mtdname("Bootloader", &fc, 0x2a, 2);
    if(argc == 1) {
        printf("fc. 0x%x\n", fc);
        exit(0);
    }
    if(fc != 0xffff) {
        printf("the machine has a fc No. 0x%x, you need upgrade boot\n", fc);
        exit(0);
    }
    fc = strtol(argv[1], NULL, 16);
//    printf("fc = 0x%x\n", fc);
//    tmpsn[0] = sn & 0xffff;
//    tmpsn[1] = sn >> 16;
    flash_write_mtdname("Bootloader", &fc, 0x2a, 2);
    exit(0);
}
