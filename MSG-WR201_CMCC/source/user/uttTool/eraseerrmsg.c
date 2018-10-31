#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/autoconf.h>

int main()
{
    int ret = 0;
    ret = flash_erase_mtdname("syslog");
    if (ret == -1)
    {
	ret = flash_erase_mtdname("Syslog");
	if (ret == -1)
	{
	    printf("Erase flash syslog fail!\n");
	}    
    }
    return ret;
}
