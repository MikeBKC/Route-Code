#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    int ret = 0;
    char buf[65536];    
    int i = 0;    
    int num = 0;

    ret = flash_read_mtdname("syslog", buf, 0x0, 0x10000);
    if (ret == -1)
    {
	ret = flash_read_mtdname("Syslog", buf, 0x0, 0x10000);
	if (ret == -1)
	{
	    printf("Read flash syslog/Syslog fail!\n");
	}
    }

    for (i=0;i<65536;i++)
    {
	if (buf[i] == -1)
	{
	    num++;
	    continue;
	}
	if (num > 4)
	{
	    printf("################################################\n");
	    printf("################################################\n");
	    printf("################################################\n");
	    printf("################################################\n");
	}
	num = 0;
	printf("%c", buf[i]);
    }
    printf("\n");

    return ret;
}
