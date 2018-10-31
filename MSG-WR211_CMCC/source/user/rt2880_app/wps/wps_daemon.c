#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"


#include <linux/autoconf.h>
#include "ralink_gpio.h"
#define GPIO_DEV "/dev/gpio"


static char *saved_pidfile;

/*
 * gpio interrupt handler -
 *   SIGUSR1 - notify goAhead to start WPS (by sending SIGUSR1)
 *   SIGUSR2 - restore default value
 */
static void nvramIrqHandler(int signum)
{
    if (signum == SIGUSR1) 
    {
        printf("wps button click\n");
        system("iwpriv ra0 set WscMode=2;iwpriv ra0 set WscGetConf=1;iwpriv rai0 set WscMode=2;iwpriv rai0 set WscGetConf=1");
    }
    else if (signum == SIGUSR2)
    {
        printf("wps button servers seconds\n");
    }
    return ;
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
	info.irq = 45;

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

int main(int argc,char **argv)
{
	int fd;

	ProfInit();/*������س�ʼ������ҪΪ�����ڴ�ȣ�*/
	if (initGpio() != 0)
		exit(EXIT_FAILURE);

	fd = pidfile_acquire("/var/run/wps_daemon.pid");
	pidfile_write_release(fd);

	while (1) {
		pause();
	}

	exit(EXIT_SUCCESS);
}

