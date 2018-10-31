#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"

#ifdef CONFIG_RALINK_GPIO
#include <linux/autoconf.h>
#include "ralink_gpio.h"
#define GPIO_DEV "/dev/gpio"
#endif

static char *saved_pidfile;

static void switch_change(int status, WirelessProfile *prof)
{
    if (prof->head.active != status)
    {
        prof->head.active = status;
    }
    return;
}

static void wireless_switch_change(int status)
{
    MibProfileType mibType2 = MIB_PROF_WIRELESS; 

    WirelessProfile *prof = NULL;
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType2, 0);

#if (WIRELESS_5G == FYES)
    WirelessProfile *prof5G = NULL;
    prof5G = (WirelessProfile*)ProfGetInstPointByIndex(mibType2, 1);
#endif

    switch_change(status,prof);
#if (WIRELESS_5G == FYES)
    switch_change(status,prof5G);
#endif     

    //system("wlan.sh");
    system("wlan.sh");
#if (WIRELESS_5G == FYES)
    system("wlan_5g.sh");
#endif
  // nvramWriteCommit();
    ProfWiteToFlashNoSyn();
}

/*
 * gpio interrupt handler -
 *   SIGUSR1 - notify goAhead to start WPS (by sending SIGUSR1)
 *   SIGUSR2 - restore default value
 */
static void nvramIrqHandler(int signum)
{    
    SystemProfile *profSys = NULL;

    MibProfileType mibType = MIB_PROF_SYSTEM;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);

#if (WIFI_SLEEP_FLAG == FYES)
    if (signum == SIGUSR1)
    {
        printf("-----click one time........\n");
        if(profSys -> wifisleep == FUN_ENABLE)
        {
            profSys -> wifisleep = FUN_DISABLE;
            wireless_switch_change(0);
            system("mmap w 0x10000600 0x0;mmap w 0x10000064 0x501");
        }
        else
        {
            profSys -> wifisleep = FUN_ENABLE;
            wireless_switch_change(1);   
            system("mmap w 0x10000600 0x15;mmap w 0x10000064 0x0");
        }
    }
    else if(signum == SIGUSR2)
    {
        printf("-----click long times------\n");       
    }
    else
    {
	return;
    }
#endif
}

/*
 * init gpio interrupt -
 *   1. config gpio interrupt mode
 *   2. register my pid and request gpio pin 0
 *   3. issue a handler to handle SIGUSR1 and SIGUSR2
 */
int initGpio(void)
{ 
    int fd;
    ralink_gpio_reg_info info;

    info.pid = getpid();
#if (RAISECOM_211 == FYES)
    info.irq = 5;
#else
    info.irq = 37;
#endif
    fd = open(GPIO_DEV, O_RDONLY);
    if (fd < 0) {
        perror(GPIO_DEV);
        return -1;
    }
    //set gpio direction to input
    if (ioctl(fd, RALINK_GPIO_SET_DIR_IN, (1<<info.irq)) < 0)
        goto ioctl_err;
    //enable gpio interrupt
    if (ioctl(fd, RALINK_GPIO_ENABLE_INTP) < 0)
        goto ioctl_err;

    //register my information
    if (ioctl(fd, RALINK_GPIO_REG_IRQ, &info) < 0)
        goto ioctl_err;
    close(fd);

    //issue a handler to handle SIGUSR1 and SIGUSR2
    signal(SIGUSR1, nvramIrqHandler);
    signal(SIGUSR2, nvramIrqHandler);
    return 0;

ioctl_err:
    perror("ioctl");
    close(fd);
    return -1;
}

static void pidfile_delete(void)
{
    if (saved_pidfile) unlink(saved_pidfile);
}

int pidfile_acquire(const char *pidfile)
{
    int pid_fd;
    if (!pidfile) return -1;

    pid_fd = open(pidfile, O_CREAT | O_WRONLY, 0644);
    if (pid_fd < 0) {
        printf("Unable to open pidfile %s: %m\n", pidfile);
    } else {
        lockf(pid_fd, F_LOCK, 0);
        if (!saved_pidfile)
            atexit(pidfile_delete);
        saved_pidfile = (char *) pidfile;
    }
    return pid_fd;
}

void pidfile_write_release(int pid_fd)
{
    FILE *out;

    if (pid_fd < 0) return;

    if ((out = fdopen(pid_fd, "w")) != NULL) {
        fprintf(out, "%d\n", getpid());
        fclose(out);
    }
    lockf(pid_fd, F_UNLCK, 0);
    close(pid_fd);
}

void wifiLedInit()
{
    SystemProfile *profSys = NULL;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
#if(WIFI_SLEEP_FLAG == FYES)
    if(profSys->wifisleep == FUN_ENABLE){
        /*亮灯 开无线*/
        //system("mmap w 0x10000600 0x15;mmap w 0x10000064 0x0");

    }else{
        /*灭灯 关无线*/
        //system("mmap w 0x10000600 0x0;mmap w 0x10000064 0x501");

    }
#endif
}

int main(int argc,char **argv)
{
    int fd;

    ProfInit();/*配置相关初始化（主要为共享内存等）*/

    if (initGpio() != 0)
        exit(EXIT_FAILURE);

    fd = pidfile_acquire("/var/run/wifi_sleep.pid");
    pidfile_write_release(fd);

    //wifiLedInit();


    while (1) {
        pause();
    }

    exit(EXIT_SUCCESS);
}

