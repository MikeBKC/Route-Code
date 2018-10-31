#include	<string.h>
#include	<stdarg.h>
#include	"uttMachine.h"
#include	"mib.h"
#include	"profacce.h"

#if 0
#define DBGPRINT(fmt, args...)	    printf(fmt,##args)
#else
#define DBGPRINT(fmt, args...) 
#endif

#if (PORT_CONF_REDUCED == FYES)
static void doSystem(const char* _Format, ...)
{
    char cmdstr[128];/*应该命令主要针对ipset故不会太大*/
    va_list   args; 

    memset(cmdstr, 0, sizeof(cmdstr));

    /*生成命令字符串*/
    va_start(args,_Format); 
    vsnprintf(cmdstr, sizeof(cmdstr)-1, _Format,   args   ); 
    va_end(args);

    DBGPRINT(cmdstr);
    /*运行命令*/
    system(cmdstr);
    return;
}
/*ethtool 设置*/
static void ethtoolSet(char* ifName, PortSpdSetSt mode)
{
    if(mode.autoneg == 1)
    {
	/*自动协商*/
#if defined CONFIG_IT8712_AR8316
	doSystem("ethtool -s %s autoneg on speed 1000 ", ifName);
#else
	doSystem("ethtool -s %s autoneg on", ifName);
#endif
    }
    else
    {	/*强制设置*/
	doSystem("ethtool -s %s autoneg off speed %d  duplex %s", ifName, mode.speed,
	    (mode. fdx == 1?"full":"half"));
    }
    DBGPRINT("%s-%d: \n",__func__,__LINE__);
}

static void  converModeTospdSetMode(SwPortConfMode speedMode, PortSpdSetSt *spdSetMode)
{
    if(speedMode == PORT_MODE_AUTO)
    {
	spdSetMode->autoneg = 1;
	spdSetMode->speed = 0;
	spdSetMode->fdx = 0;
    }else if(speedMode == PORT_MODE_10FDX){
	spdSetMode->autoneg = 0;
	spdSetMode->speed = 10;
	spdSetMode->fdx = 1;
    } else if(speedMode == PORT_MODE_100FDX) {
	spdSetMode->autoneg = 0;
	spdSetMode->speed = 100;
	spdSetMode->fdx = 1;
    
    } else if(speedMode == PORT_MODE_1GFDX) {
	spdSetMode->autoneg = 0;
	spdSetMode->speed = 1000;
	spdSetMode->fdx = 1;
    
    } else if(speedMode == PORT_MODE_10HDX) {
	spdSetMode->autoneg = 0;
	spdSetMode->speed = 10;
	spdSetMode->fdx = 0;
    
    } else if(speedMode == PORT_MODE_100HDX) {
	spdSetMode->autoneg = 0;
	spdSetMode->speed = 100;
	spdSetMode->fdx = 0;
    
    } else {
	spdSetMode->autoneg = 0;
	spdSetMode->speed = 0;
	spdSetMode->fdx = 0;
    }
    DBGPRINT("%s-%d: converMode autoneg=%d, speed=%d, fdx=%d\n",__func__,__LINE__,spdSetMode->autoneg,spdSetMode->speed,spdSetMode->fdx);
    return;
}

static void usage()
{
    printf("usage: uttportset  wanindex \n");
    printf("                   value range (1 ~ 4)\n");
    return;
}
#endif
/* ar8316 设置完口端口速率
 * 开机启动时使用此程序设置*/
int main(int argc, char **argv) 
{
#if (PORT_CONF_REDUCED == FYES)
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
    SwPortConfMode speedMode;
    PortSpdSetSt spdSetMode;
    int index = 0;

    if(argc < 2) {
	usage();
	return -1;
    }
    index = strtol(argv[1], NULL, 10);
    if((index < 1) || (index > 4)) {
	usage();
	return -1;
    }
    /*共享内存初始化*/
    if(ProfInit() == PROFFAIL) 
    {
	return -1;
    }
 
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, index);
    if ((profIF != NULL) && (profIF->head.active == 1)) 
    {
#if defined(CONFIG_IT8712_AR8316) 
	converModeTospdSetMode((profIF->ether.speedMode), &spdSetMode);
	ethtoolSet(getWanIfNamePPP(index),spdSetMode);
#endif
    }
    /*共享内存分离*/
    ProfDetach();
    DBGPRINT("%s-%d: \n",__func__,__LINE__);
#endif
    return 0;
}
