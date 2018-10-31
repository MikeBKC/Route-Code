
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>    
#include <sys/stat.h>    
#include <fcntl.h>
#include <stdint.h>

#include	"flash_api.h"
#include	<linux/autoconf.h>
#include        <sys/utsname.h>
#include	"mib.h"
//#include	"webImpMisc.h"
#include <sys/mman.h>



/* 两个头结构中必须保证ih_dcrc，systemVer，name的位置相同 */

typedef struct image_header {
        uint32_t imageLen;
        uint32_t imageCrc;
        uint32_t arch;
        uint32_t fsLen;
        uint32_t os;
	uint32_t systemVer;
	uint16_t unuse;
	uint16_t funcVer;
        uint32_t loadAddr;
        uint32_t entryAddr;
        uint8_t  name[20];
#if 1
        uint16_t        ih_fc;          /* fc mode for uboot */
        uint8_t         ih_update;      /* Image update control*/
        uint8_t         ih_reverse;
#else
        uint32_t kernelLen;
#endif
        uint32_t kernelCrc;
} image_header_t;
int main() 
{
    static int updateFlag = 0;
    int bRet = 0;
    char *     fn;
    void *map_addr;
    int fnlen; 
    int ret = 0;
    image_header_t *head;
    image_header_t localHead;
    int headLen;
    struct utsname utsbuf;
    char *tmp;
    const char *ErrMsg=NULL;
    int map_fd;
    int fileS, fileE, fileL;

    map_fd = open("/kernelImage", O_RDWR);
    if(map_fd < 0) {
	fprintf(stderr,"cannot open file /kernelImage");
	return ;
    }
    fileS = lseek(map_fd, 0, SEEK_SET);
    fileE = lseek(map_fd, 0, SEEK_END);
    lseek(map_fd, 0, SEEK_SET);
    fileL = fileE - fileS;
    fnlen = fileL;
    fileL = fileL % 4096 == 0 ? fileL : fileL + 4096 - fileL/4096;
    map_addr = mmap(0, fileL, PROT_READ|PROT_WRITE, MAP_SHARED, map_fd, 0);
    head = (image_header_t*)map_addr;

    ret = flash_read_mtdname("Kernel", (char*)&localHead, 0, sizeof(image_header_t) );
    if (ret == -1) {
	munmap(map_addr, fileL);
	close(map_fd);
	system("rm -rf /kernelImage");
	fprintf(stderr,"read Kernel error");
	return ;
    }

    uname(&utsbuf);
    tmp = strtok(utsbuf.version, " ");

    if(strcmp(localHead.name, head->name) != 0) {
	munmap(map_addr, fileL);
	close(map_fd);
	system("rm -rf /kernelImage /ratio &");
	fprintf(stderr,"VER ERROR:local[%s],web[%s]\n", localHead.name, head->name);
	return ;
    }
    headLen = sizeof(image_header_t);
    if (crc32(0, (const unsigned char *)map_addr+8, fnlen - 8) != ntohl(head->imageCrc)) {
	munmap(map_addr, fileL);
	close(map_fd);
	system("rm -rf /kernelImage");
	fprintf(stderr,"CRC check error");
	return ;
    }


    mtd_unlock("Kernel");
    ret = flash_write_mtdname("Kernel", map_addr, 0, fnlen);
    if(ret == -1) {

	munmap(map_addr, fileL);
	close(map_fd);
	system("rm -rf /kernelImage");
	fprintf(stderr,"write to Kernel Error");
	return ;
    }
}
