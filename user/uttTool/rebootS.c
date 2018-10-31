#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uttMachine.h"
#include "typedef.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ifmisc.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "conver.h"


int main(int argc, char **argv)
{
    int i = 0;
    char buf[24];
    memset(buf, 0, sizeof(buf));
    
    system("flashWrite");
    
    strcpy(buf, "reboot");

    for (i=1;i<argc;i++)
    {
	strcat(buf, " ");
	strcat(buf, argv[i]);
    }

    system(buf);
    return 0;
}
