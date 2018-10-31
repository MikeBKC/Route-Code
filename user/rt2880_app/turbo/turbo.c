#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"

#if (FEATURE_AP == FYES)
#include <sys/msg.h>
#endif
#ifdef CONFIG_RALINK_GPIO
#include <linux/autoconf.h>
#include "ralink_gpio.h"
#define GPIO_DEV "/dev/gpio"
#endif


#ifdef CONFIG_RT2880_L2_MANAGE
int ramad_start(void);
#endif

#if defined(CONFIG_BOARD_MTK7620_E) || defined(CONFIG_BOARD_MTK7620_I) || defined(CONFIG_BOARD_MTK7620_EN)
#define   UNKNOWN 39
#elif defined(CONFIG_BOARD_MTK7620_H)
#define   UNKNOWN 39
#elif defined(CONFIG_BOARD_MTK7620_G)
#define   UNKNOWN 13 
#endif

#if (FEATURE_AP == FYES)

#define MSG_TYPE_BACKUP  0x05u
#define SW_MSG_KEY  0xAB00
#define SW_MSG_TYPE     0x38
int sw_msg_init(void);
int sendwebmsg(int msg_type, int web_id);
#endif

static void doSystem(const char* _Format, ...)
{
    char cmdstr[128];/*应该命令主要针对ipset故不会太大*/
    va_list   args; 

    memset(cmdstr, 0, sizeof(cmdstr));

    /*生成命令字符串*/
    va_start(args,_Format); 
    vsnprintf(cmdstr, sizeof(cmdstr)-1, _Format,   args   ); 
    va_end(args);

    /*运行命令*/
    system(cmdstr);
    return;
}

static char *saved_pidfile;

/*
 * gpio interrupt handler -
 *   SIGUSR1 - notify goAhead to start WPS (by sending SIGUSR1)
 *   SIGUSR2 - restore default value
 */
static void nvramIrqHandler(int signum)
{
    WirelessProfile *prof= NULL;
    MibProfileType mibType = MIB_PROF_WIRELESS;

    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 0);
	if ((prof != NULL) && (prof->head.active == 1)){
		    if (prof->turbo == 0) {
#if !defined(CONFIG_BOARD_MTK7621_F)
			    doSystem("gpio l %d 2 2 4000 0 1&",UNKNOWN);
#endif
			    system("iwpriv ra0 set TxPower=100");
			    prof->turbo = 1;
			    prof->mBaseCf.TxPower = 100;
		    } else {
#if !defined(CONFIG_BOARD_MTK7621_F)
			    doSystem("gpio l %d 0 1 0 0 0&",UNKNOWN);
#endif
			    system("iwpriv ra0 set TxPower=50");
			    prof->turbo = 0u;
			    prof->mBaseCf.TxPower = 50;
		    }
#if (FEATURE_AP == FYES)
		sendwebmsg(MSG_TYPE_BACKUP, 0);
#endif
	        nvramWriteCommit();
	}


}

static void nvramIrqHandler2(int signum)
{
    WirelessProfile *prof= NULL;
    MibProfileType mibType = MIB_PROF_WIRELESS;

    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 0);
    if (prof != NULL) {
        if (prof->head.active == 1) {
            if (prof->turbo == 1) {
#if !defined(CONFIG_BOARD_MTK7621_F)
                doSystem("gpio l %d 2 2 4000 0 1&",UNKNOWN);
#endif
                system("iwpriv ra0 set TxPower=100");
            } else {
#if !defined(CONFIG_BOARD_MTK7621_F)
                doSystem("gpio l %d 0 1 0 0 0&",UNKNOWN);
#endif
                system("iwpriv ra0 set TxPower=50");
            }
        } else {
#if !defined(CONFIG_BOARD_MTK7621_F)
            doSystem("gpio l %d 0 1 0 0 0&",UNKNOWN);
#endif

        }
	}
	return;
}

static void initTurbo(void)
{
    WirelessProfile *prof= NULL;
    MibProfileType mibType = MIB_PROF_WIRELESS;

#if !defined(FEATURE_AP) || (FEATURE_AP==FNO)
    sleep(5);
#endif
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 0);
    if (prof != NULL) {
        if (prof->head.active == 1) {
            if (prof->turbo == 1) {
#if !defined(CONFIG_BOARD_MTK7621_F)
                doSystem("gpio l %d 2 2 4000 0 1&",UNKNOWN);
#endif
#if 0 /*在wlan.sh中初始化, KeHuatao*/
                system("iwpriv ra0 set TxPower=100"); 
#endif
            } else {
#if !defined(CONFIG_BOARD_MTK7621_F)
                doSystem("gpio l %d 0 1 0 0 0&",UNKNOWN);
#endif
#if 0 /*在wlan.sh中初始化, KeHuatao*/
                system("iwpriv ra0 set TxPower=50");
#endif
            }
        } else {
#if !defined(CONFIG_BOARD_MTK7621_F)
            doSystem("gpio l %d 0 1 0 0 0&",UNKNOWN);
#endif
        }
    }

	return;
}
/*
 * init gpio interrupt -
 *   1. config gpio interrupt mode
 *   2. register my pid and request gpio pin 0
 *   3. issue a handler to handle SIGUSR1 and SIGUSR2
 */
int initGpio(void)
{
#ifndef CONFIG_RALINK_GPIO
#if 0
	signal(SIGUSR1, nvramIrqHandler);
#endif
	signal(SIGUSR2, nvramIrqHandler2);
	return 0;
#else
	int fd;
	ralink_gpio_reg_info info;

	info.pid = getpid();
#if defined(CONFIG_RALINK_MT7621)
	info.irq = 18;
#else
#if defined(CONFIG_BOARD_MTK7620_E) || defined(CONFIG_BOARD_MTK7620_I) || defined(CONFIG_BOARD_MTK7620_EN)
	info.irq = 12;
#elif defined(CONFIG_BOARD_MTK7620_H)
 	info.irq = 12;
#elif defined(CONFIG_BOARD_MTK7620_G)
	info.irq = 1;
#endif
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
#if defined(CONFIG_RALINK_MT7620) || defined(CONFIG_RALINK_MT7621)
	signal(SIGUSR1, nvramIrqHandler);
#endif
#if (FEATURE_AP == FYES)
	signal(SIGUSR2, nvramIrqHandler);
#else
	signal(SIGUSR2, nvramIrqHandler2);
#endif

	/* init */
	initTurbo();

	return 0;

ioctl_err:
	perror("ioctl");
	close(fd);
	return -1;
#endif
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
#if (FEATURE_AP == FYES)

static int msgId = -1;
struct sw_msg_t {
     int msg_type;
     int msg_code;
};
struct msgbuf_t {
    long mtype;
    struct sw_msg_t sw_msg;
}; 
int sw_msg_init(void)
{
     int id;
 
    /*create the msg queue*/
     id = msgget((key_t)SW_MSG_KEY, IPC_CREAT | 0666);
     if(id == -1)
     {
         perror("switch msgget error");
     }
 
     return id;
}
/*
 *send message
 *KeHuatao
 */
int sendwebmsg(int msg_type, int web_id)
{
    struct msgbuf_t webmsg;

    memset(&webmsg, 0, sizeof(struct msgbuf_t));
    webmsg.mtype = SW_MSG_TYPE;
    webmsg.sw_msg.msg_type = msg_type;
    webmsg.sw_msg.msg_code = web_id;
    if(msgsnd(msgId, &webmsg, sizeof(webmsg.sw_msg), 0)==-1)
    {
	/*error*/
	perror("goahead: message send error:");
    }
    return 0;
}
#endif
int main(int argc,char **argv)
{
	int fd;

	ProfInit();/*配置相关初始化（主要为共享内存等）*/
	if (initGpio() != 0)
		exit(EXIT_FAILURE);

	fd = pidfile_acquire("/var/run/nvramd.pid");
	pidfile_write_release(fd);

#if (FEATURE_AP == FYES)
       msgId = sw_msg_init();
#endif
#ifdef CONFIG_RT2880_L2_MANAGE
	//start the management daemon (blocking)
	ramad_start();
#else
	while (1) {
		pause();
	}
#endif

	exit(EXIT_SUCCESS);
}

