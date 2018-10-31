#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <arpa/inet.h>
#define LINUX_IMAGE 0
#define REOS_IMAGE 1
struct ff{
    int a;
    int b;
    int c;
};

typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef unsigned long uLong;
typedef struct image_header {
    uint32_t imageLen;
    uint32_t imageCrc;
    uint32_t arch;
    uint32_t fsLen;
    uint32_t os;
    uint32_t imageType;
    uint16_t fc;
    uint16_t imageComp;
    uint32_t loadAddr;
    uint32_t entryAddr;
    uint8_t  name[20];
    uint32_t kernelLen;
    uint32_t kernelCrc;
} image_header_t;


extern unsigned long crc32(uLong crc, char const *buf, uLong len); 

#if 0
int main(int argc,char *argv[])
{
    FILE *fp;
    while (--argc > 0 && **++argv == '-') {
        while (*++*argv) {             
             switch (**argv) {  
                 case 'N':
                     break;
                 case 'E':
                     break;
                 case 'M':
                     break;
                 case 'O':
                     break;
                 case 'f':
                     break;

             }
        }
    
    }
}
#endif

int main(int argc,char *argv[])
{
    FILE *fp;
    char buf[200], *tmp, arch[4];
    char *pv=NULL;
    unsigned int tmpInt;
    unsigned short func_mode;
    int fileOffsetHead, fileOffsetEnd, crc;
    int fsLen, imageLen, kernelLen, fullLen;
    image_header_t head;
    image_header_t* tmphead;
    memset(&head, 0, sizeof(image_header_t));

    tmpInt=(unsigned int)(strtoll(argv[3],NULL,16));
    printf("%x\n",tmpInt);
    //head.entryAddr = (tmpInt >> 24);
    //head.entryAddr |= (tmpInt << 24);
    //head.entryAddr |= ((tmpInt << 8) & 0x00ff0000);
    //head.entryAddr |= ((tmpInt >> 8) & 0x0000ff00);

    head.entryAddr = htonl(tmpInt);

    tmpInt=(unsigned int)(strtoll(argv[4],NULL,16));
    //head.loadAddr = (tmpInt >> 24);
    //head.loadAddr |= (tmpInt << 24);
    //head.loadAddr |= ((tmpInt << 8) & 0x00ff0000);
    //head.loadAddr |= ((tmpInt >> 8) & 0x0000ff00);
    head.loadAddr = htonl(tmpInt);
    arch[0] = 'U';
    arch[1] = 'M';
    arch[2] = 'V';
    arch[3] = 0xa;
    memcpy(&head.arch, arch, sizeof(int));
    strcpy(head.name,argv[1]);

    if(!strcmp(argv[5],"linux")){
        head.os = htonl(LINUX_IMAGE);
    }else if(!strcmp(argv[5],"reos")){
        head.os = htonl(REOS_IMAGE);
    }else{
        printf("os type error,must linux or reos\n");
        exit(1);
    }
    
    fp = fopen(argv[6],"r");
    if(fp==NULL){
        printf("open %s error\n",argv[6]);
        exit(1);
    }
    fseek(fp,0,SEEK_SET);
    fileOffsetHead = ftell(fp);
    fseek(fp,0,SEEK_END);
    fileOffsetEnd = ftell(fp);
    printf("%d %d \n",fileOffsetHead,fileOffsetEnd);
    kernelLen = fileOffsetEnd - fileOffsetHead;
    head.kernelLen = htonl(kernelLen);
    fseek(fp,0,SEEK_SET);

    if( argc > 7 ){
        pv = argv[7];
        func_mode=(unsigned short)strtoul(pv,NULL,16);
        *((unsigned short*)(&head.fc)) = htons(func_mode);
    }

    tmp = (char*)malloc(sizeof(char) * kernelLen);
    fread(tmp, kernelLen, 1, fp);
    head.kernelCrc = htonl(crc32(0, tmp, kernelLen));
    free(tmp);
    fclose(fp);

    if(head.os == htonl(LINUX_IMAGE) && argc == 9) {
        fp = fopen(argv[8],"r");
        if(fp==NULL){
            printf("open %s error\n",argv[8]);
            exit(1);
        }
        fseek(fp,0,SEEK_SET);
        fileOffsetHead = ftell(fp);
        fseek(fp,0,SEEK_END);
        fileOffsetEnd = ftell(fp);
        fsLen = fileOffsetEnd - fileOffsetHead;
        head.fsLen = htonl(fsLen);
        fclose(fp);
    }else{
        fsLen = 0;
        head.fsLen = 0;
    }
    fullLen = fsLen + kernelLen + sizeof(image_header_t);
    imageLen = fullLen - 2*sizeof(int);
    head.imageLen = htonl(imageLen);
    fp = fopen(argv[2],"w+");
    fwrite(&head,sizeof(image_header_t),1,fp);
    fclose(fp);
//    printf("%d\n",&(((image_header_t*)head)->ih_ep));
    printf("%x\n",head.entryAddr);

    sprintf(buf,"cat %s >> %s",argv[6], argv[2]);
    system(buf);
    if(argc == 9) {
        sprintf(buf,"cat %s >> %s",argv[8], argv[2]);
        system(buf);
    }
    printf("%x %x %x\n",head.imageLen,head.fsLen,head.kernelLen);

    fp = fopen(argv[2],"rb");
    if(fp==NULL){
        printf("open %s error\n",argv[2]);
        exit(1);
    }
    fseek(fp,0,SEEK_SET);

    tmp = (char*)malloc(sizeof(char) * (fullLen));
    tmphead = (image_header_t*)tmp;
    fread(tmp, fullLen, 1, fp);
    tmphead->imageCrc = htonl(crc32(0, tmp+8, imageLen));
    fclose(fp);
    fp = fopen(argv[2],"w+");
    if(fp==NULL){
        printf("open %s error\n",argv[2]);
        exit(1);
    }
    fwrite(tmp, fullLen, 1, fp);
    free(tmp);
    fclose(fp);

    exit(0);
}
