#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define CHECKMAX 256
static void checkreboot();
static void checkreboot()
{
    FILE *fp;
    char buf[CHECKMAX];
    char IP[16];
    int count = 0;
    memset(buf,0,CHECKMAX);
    if((fp = fopen("/proc/net/statsPerIp","r")) == NULL)
    {
        perror("fopen");
        return;
    }
    while(fgets(buf,CHECKMAX,fp) != NULL)
    {
        memset(IP,0,16);
        sscanf(buf,"%s %*s",IP);
        memset(buf,0,CHECKMAX);
        if(strcmp(IP,"127.0.0.1") == 0)
        {
            continue;
        }
        count++;   
    }
    fclose(fp);
    if(count <= 1)
    {
        system("reboot");
    }
}
int main(int argc,char *argv[])
{
    int timecount = 0;
    while(timecount < 12)
    {
        checkreboot();
        sleep(600);
        timecount+=1;
    }
    return 0;
}
