

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
    FILE *fp;
    long fileS, fileE, fileL;
    int rst = 0;
    char cmd[256] = {0};
    char flashbuf[65536], filebuf[65536];
    char filename[256];
    if(argc != 3) {
        usage();
    }

    sprintf(cmd, "tftp -g -r %s %s %s", argv[2], argv[1], argv[2]);
    rst = system(cmd);

    if(rst == -1) {
        printf("upgrade faild,please check your tftpserver and file\n");
        exit(1);
    }

    sprintf(filename, "/tmp/%s", argv[2]);
    fp = fopen(filename, "r");
    if(fp == NULL) {
        printf("upgrade faild,please check your tftpserver and file\n");
        exit(1);
    }
    
    fseek(fp, 0, SEEK_SET);
    fileS = ftell(fp);
    fseek(fp, 0, SEEK_END);
    fileE = ftell(fp);
    fileL = fileE - fileS;
    fseek(fp, 0, SEEK_SET);

    if(fileL > 65535) {
        printf("your file is too big\n");
        exit(1);
    } else {
        printf("file Len %d\n",fileL);
    }

    flash_read_mtdname("Bootloader", flashbuf, 0, fileL);
    fread(filebuf, fileL, 1, fp);
#if 0
    int i;
    for(i = 0; i < 0x30; i++) {
        printf(" %x:%x\n",flashbuf[0x30+i],filebuf[0x30+i]);
    }
#endif

#if 1
//fileL-0x40
    if((flashbuf[0x28] == filebuf[0x28]) && 
            (memcmp(flashbuf+0x30, filebuf+0x30, fileL-0x30) == 0)) {
        printf("bootload is same!\n");
        flash_write_mtdname("Bootloader", filebuf, 0, fileL);
    } else {
        fclose(fp);
        printf("bootload is not same!\n");
        exit(0);
    }
#else
    if((flashbuf[0x28] == filebuf[0x28])) {
        flash_write_mtdname("Bootloader", filebuf, 0, fileL);
    }
#endif
    fclose(fp);

    printf("upgrade ok!\n");

    exit(0);
}
