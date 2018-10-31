
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <memory.h>
#include        "net/if.h"    
#include        "sys/ioctl.h"
#include    "mib.h"
#include    "profacce.h"
#include    <uttMachine.h>

#define TXBYTE          0     
#define TXPACKET        1     
#define RXBYTE          2     
#define RXPACKET        3     
#define RST_BUTTON_COUNT 7

#define SOFT_RST_PIN 0
#define WPS_B_IN 1
#define WAN_RST_OUT 2
#define SYS_LED 4
#define WLAN_LED 5
#define WPS_LED0 6
#define WPS_LED1 7

#define SET_BIT_ONE(a) (1 << a)
#define SET_BIT_ZERO(a) (~(1 << a))

#if ((NOTICE == FYES)&&(WEB_AUTH == FYES))
#define CHECK_RST(a,b)  if(a == b){ \
                            makeFactoryProfiles();\
			    saveConfigToFile(title, content, NOTIFY_HTM, NOTICE_HTML_1);\
			    saveConfigToFile(title, content, NOTIFY_HTM, NOTICE_HTML_2);\
			    saveConfigToFile(title, content, DNS_NOTIFY_HTM, NOTICE_HTML_3);\
			    saveConfigToFile(title, content, NOTIFY_HTM, NOTICE_HTML_4);\
				char pictureUrls[511];\
				memset(pictureUrls,0,sizeof(pictureUrls));\
				saveContactToFile(pictureUrls, WEB_AUTH_PICURL_PATH);\
                            ProfWiteToFlash();\
                            system("reboot");\
                        }
#elif((NOTICE == FYES)&&(WEB_AUTH != FYES))
#define CHECK_RST(a,b)  if(a == b){ \
                            makeFactoryProfiles();\
			    saveConfigToFile(title, content, NOTIFY_HTM, NOTICE_HTML_1);\
			    saveConfigToFile(title, content, NOTIFY_HTM, NOTICE_HTML_2);\
			    saveConfigToFile(title, content, DNS_NOTIFY_HTM, NOTICE_HTML_3);\
			    saveConfigToFile(title, content, NOTIFY_HTM, NOTICE_HTML_4);\
                            ProfWiteToFlash();\
                            system("reboot");\
                        }
#elif((NOTICE != FYES)&&(WEB_AUTH == FYES))
#define CHECK_RST(a,b)  if(a == b){ \
                            makeFactoryProfiles();\
				char pictureUrls[511];\
				memset(pictureUrls,0,sizeof(pictureUrls));\
				saveContactToFile(pictureUrls, WEB_AUTH_PICURL_PATH);\
                            ProfWiteToFlash();\
                            system("reboot");\
                        }
#else
#define CHECK_RST(a,b)  if(a == b){ \
                            makeFactoryProfiles();\
                            ProfWiteToFlash();\
                            system("reboot");\
                        }
#endif
void * map_base;
FILE *f;
int n,fd;

static void gpioFun(volatile int* gpioEn, volatile int* gpioIn, volatile int* gpioOut)
{
#if (NOTICE == FYES)
    char title[2], content[2];
    memset(title, 0, sizeof(title));
    memset(content, 0, sizeof(content));
#endif
    int rstButton = 0,countFlag = 0;
    *gpioEn = *gpioEn & SET_BIT_ZERO(SOFT_RST_PIN) ;
    *gpioEn = *gpioEn & SET_BIT_ZERO(WLAN_LED) ;
    *gpioEn = *gpioEn | SET_BIT_ONE(SYS_LED);
    *gpioEn = *gpioEn | SET_BIT_ONE(WAN_RST_OUT);

    *gpioEn = *gpioEn & SET_BIT_ZERO(WPS_LED0);
    *gpioEn = *gpioEn & SET_BIT_ZERO(WPS_LED1);

    *gpioOut = *gpioOut & SET_BIT_ZERO(SYS_LED);
    *gpioOut = *gpioOut & SET_BIT_ZERO(WLAN_LED);
    while(1){
        if((*gpioIn) & 0x1)
            rstButton = 0;
        else
            rstButton++;
        CHECK_RST(rstButton, RST_BUTTON_COUNT);
        usleep(200000);
        if((*gpioIn) & 0x1)
            rstButton = 0;
        else
            rstButton++;
        CHECK_RST(rstButton, RST_BUTTON_COUNT);
       *gpioEn = *gpioEn & 0xffffffef;
        *gpioOut = *gpioOut | 0x10;
        usleep(200000);
        *gpioOut = *gpioOut & 0xffffffef;
        *gpioEn = *gpioEn | 0x10;
        countFlag++;
    }
}

int main(int argc,char *argv[])
{
    volatile int *gpioEn,*gpioOut,*gpioIn;
    if(ProfInit() == PROFFAIL) {
        return -1;
    }

    if((fd=open("/dev/mem",O_RDWR|O_SYNC))==-1){
        return(-1);
    }
    map_base=mmap(0,0xff,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0x18040000);
    gpioEn = (volatile unsigned int *) (map_base+0);
    gpioIn = (volatile unsigned int *) (map_base+4);
    gpioOut = (volatile unsigned int *) (map_base+8);
    close(fd);
    gpioFun(gpioEn,gpioIn,gpioOut);
    munmap(map_base,0xff);//½â³ýÓ³Éä¹ØÏµ
    exit(0);
}

