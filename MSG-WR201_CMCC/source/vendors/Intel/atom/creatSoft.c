#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <arpa/inet.h>
#define LINUX_IMAGE 0
#define REOS_IMAGE 1

#define SWVER_1 4
#define SWVER_2 3
#define SWVER_3 4
#define HWVER_1 3
#define HWVER_2 2


struct ff{
    int a;
    int b;
    int c;
};

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef unsigned long uLong;
typedef struct image_header {
    uint32_t imageLen;
    uint32_t imageCrc;
    uint32_t arch;
    uint32_t fsLen;
    uint32_t os;
	uint16_t systemVer;
    uint16_t imageType;
    uint16_t unuse;
    uint16_t funcVer;
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
    unsigned int tmpInt;
    int fileOffsetHead, fileOffsetEnd, crc;
    int fsLen, imageLen, kernelLen, fullLen, i = 0;
	unsigned short systemVer = 0u; 
	int ver[3];
	int ver_max;
	char *pv = NULL;
	unsigned short func_ver = 0U;
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
    arch[1] = 'L';
    arch[2] = 'V';
    arch[3] = 0xa;
    memcpy(&head.arch, arch, sizeof(int));
    strcpy(head.name,argv[1]);

	if( argc > 7 ){
		if(isdigit(argv[7][0])){
			pv = argv[7];	
		}else{
			pv = argv[7] + 1;
		}
		func_ver=(unsigned short)strtoul(pv,NULL,10);
		head.funcVer = func_ver;
	}

	if( argc > 8 ){
		memset(ver,0,sizeof(ver));
		if(isdigit(argv[8][0])){
			pv = argv[8];	
		}else{
			pv = argv[8] + 1;
		}
		if( pv = strtok(pv,".") ){
			if(!isdigit(pv[0])){
				printf("Soft: format of Version is wrong\n");
				exit(1);
			}
			ver[i]=strtol(pv,NULL,10);
			ver_max = (int) pow(2,SWVER_1);
			if(ver[i]>ver_max){
				printf("Soft: Version number is too big\n");
				exit(1);
			}
			while( pv = strtok(NULL,".") ){
				i++;    
				if(i>2) {
					printf("Soft: Version too many items\n");
					exit(1);
				}
				ver[i]=strtol(pv,NULL,10);
				switch(i){
					case 0:
						ver_max = (int) pow(2,SWVER_1);
						break;
					case 1:
						ver_max = (int) pow(2,SWVER_2);
						break;
					case 2:
						ver_max = (int) pow(2,SWVER_3);
						break;
					default:
						break;
				}
				if(ver[i]>ver_max){
					printf("Soft: Version number is too big\n");
					exit(1);
				}
			}       
		}
		systemVer = (unsigned short)( ( ver[0]<<(HWVER_1+HWVER_2+SWVER_3+SWVER_2) ) + ( ver[1]<<(HWVER_1+HWVER_2+SWVER_3) ) + ( ver[2]<<(HWVER_1+HWVER_2) ) );
	}
	memset(ver,0,sizeof(ver));
	i=0;    
	if( argc > 9 ){
		if(isdigit(argv[9][0])){
			pv = argv[9];	
		}else{
			pv = argv[9] + 1;
		}
		if( pv = strtok(pv,".") ){
			if(!isdigit(pv[0])){
				printf("Hard: format of Version is wrong\n");
				exit(1);
			}
			ver[i]=strtol(pv,NULL,10);
			ver_max = (int) pow(2,HWVER_1);
			if(ver[i]>ver_max){
				printf("Hard: Version number is too big\n");
				exit(1);
			}

			while( pv =strtok(NULL,".") ){
				i++;    
				if(i>1) {
					printf("Hard: Version too many items\n");
					exit(1);
				}
				ver[i]=strtol(pv,NULL,10);
				switch(i){
					case 0:
						ver_max = (int) pow(2,HWVER_1);
						break;
					case 1:
						ver_max = (int) pow(2,HWVER_2);
						break;
					default:
						break;
				}
				if(ver[i]>ver_max){
					printf("Hard: Version number is too big\n");
					exit(1);
				}

			}       
		}
		systemVer += (unsigned short) ( ( ver[0] << (HWVER_2) ) + ver[1] );
	}
	head.systemVer = htons(systemVer);

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

    tmp = (char*)malloc(sizeof(char) * kernelLen);
    fread(tmp, kernelLen, 1, fp);
    head.kernelCrc = htonl(crc32(0, tmp, kernelLen));
    free(tmp);
    fclose(fp);

    if(head.os == htonl(LINUX_IMAGE) && argc == 11) {
        fp = fopen(argv[10],"r");
        if(fp==NULL){
            printf("open %s error\n",argv[10]);
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
    if(argc == 11) {
        sprintf(buf,"cat %s >> %s",argv[10], argv[2]);
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
