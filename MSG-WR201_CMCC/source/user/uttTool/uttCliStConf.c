#include	<string.h>
#include	"uttMachine.h"
#include	"mib.h"
#include	"profacce.h"
#include        "cliConvert.h"


int main(int argc, char **argv) {
    ProfInit();
    char* buf = cliConfCreat();
    if(buf != NULL)
    {
	printf(buf);
	cliConfFree(buf);
    }   

    return 0;
}
