#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/autoconf.h>
#include <linux/ite8712.h>
#include <asm-generic/ioctl.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"

#define GPIO_DEV "/dev/it8712-gpio"
static char *saved_pidfile;

void loadDefault(void)
{
#if (NOTICE == FYES)
    char title[2], content[2];
    memset(title, 0, sizeof(title));
    memset(content, 0, sizeof(content));
#endif
    makeFactoryProfiles();/*恢复*/
#if (NOTICE == FYES)
    saveConfigToFile(title, content, NOTIFY_HTM, NOTICE_HTML_1);
    saveConfigToFile(title, content, NOTIFY_HTM, NOTICE_HTML_2);
    saveConfigToFile(title, content, DNS_NOTIFY_HTM, NOTICE_HTML_3);
#endif
#if (NET_SNIPER == FYES)
    saveConfigToFile(title, content, NOTIFY_HTM, NOTICE_HTML_4);
#endif
#if (WEB_AUTH == FYES)
	char pictureUrls[511];
	memset(pictureUrls,0,sizeof(pictureUrls));
	saveContactToFile(pictureUrls, WEB_AUTH_PICURL_PATH);
#if(WEBAUTH_ADVANCE == FYES)
#if (CONFIG_X86_CPU == 1)
	system("rm -rf /ff/*.png /ff/*.PNG /ff/*.jpg /ff/*.JPG /ff/*.gif /ff/*.GIF /ff/*.jpeg /ff/*.JPEG /ff/*.asp");
#else
#endif
#endif
#endif
    ProfWiteToFlash();/*写flash*/
    sleep(2);
    system("reboot");

    return;
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
	pid_t pid;
	int fd, devfd;
        struct st_8316_mii data;
        char resetSec = 0;

	ProfInit();/*配置相关初始化（主要为共享内存等）*/
	fd = pidfile_acquire("/var/run/nvramd.pid");
	pidfile_write_release(fd);
#if 1
        devfd = open(GPIO_DEV, O_RDONLY);
        if((fd < 0) || (devfd <0)) {
            pause();
        }
        while(1){
            if(ioctl(devfd, IOC_GPIO_GET_RESET_STATUS, &data)< 0) {
                    resetSec = 0;
            } else {
                if(data.data & 0x1) {
                    resetSec = 0;
                } else {
                    resetSec++;
                    if(resetSec == 3) {
                        loadDefault();
                    }
                }
            }
            sleep(1);
        }
#endif

	exit(EXIT_SUCCESS);
}

