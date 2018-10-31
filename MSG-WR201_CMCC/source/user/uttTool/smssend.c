#include   <stdio.h>
#include   <string.h>
#include   <unistd.h>
#include   <fcntl.h>
#include   <errno.h>
#include   <termios.h>
#include   <syslog.h>
#include   <sys/time.h>
#include   "uttMachine.h"
#include   "mib.h"
#include   "profacce.h"

#if (WIFIDOG_AUTH == FYES) || (WIFIDOG_MAKE_MONEY == FYES)
#if (SMS_ENABLE == 1)
int openport(char *Dev)
{
    int fd = open( Dev, O_RDWR|O_NOCTTY|O_NDELAY );
    if (-1 == fd)
    {
        perror("Can''t Open Serial Port");
        return -1;
    }
    else
        return fd;
}

int setport(int fd, int baud,int databits,int stopbits,int parity)
{
    int baudrate;
    struct	  termios   newtio;
    switch(baud)
    {
        case 300:
            baudrate=B300;
            break;
        case 600:
            baudrate=B600;
            break;
        case 1200:
            baudrate=B1200;
            break;
        case 2400:
            baudrate=B2400;
            break;
        case 4800:
            baudrate=B4800;
            break;
        case 9600:
            baudrate=B9600;
            break;
        case 19200:
            baudrate=B19200;
            break;
        case 38400:
            baudrate=B38400;
            break;
        default :
            baudrate=B9600;
            break;
    }
    tcgetattr(fd,&newtio);
    bzero(&newtio,sizeof(newtio));
    //setting   c_cflag
    newtio.c_cflag	  &=~CSIZE;
    switch (databits) /*设置数据位数*/
    {
        case 7:
            newtio.c_cflag |= CS7; //7位数据位
            break;
        case 8:
            newtio.c_cflag |= CS8; //8位数据位
            break;
        default:
            newtio.c_cflag |= CS8;
            break;
    }
    switch (parity) //设置校验
    {
        case 'n':
        case 'N':
            newtio.c_cflag &= ~PARENB;   /* Clear parity enable */
            newtio.c_iflag &= ~INPCK;	/* Enable parity checking */
            break;
        case 'o':
        case 'O':
            newtio.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/
            newtio.c_iflag |= INPCK;	       /* Disnable parity checking */
            break;
        case 'e':
        case 'E':
            newtio.c_cflag |= PARENB;	/* Enable parity */
            newtio.c_cflag &= ~PARODD;   /* 转换为偶效验*/
            newtio.c_iflag |= INPCK;	 /* Disnable parity checking */
            break;
        case 'S':
        case 's':  /*as no parity*/
            newtio.c_cflag &= ~PARENB;
            newtio.c_cflag &= ~CSTOPB;break;
        default:
            newtio.c_cflag &= ~PARENB;   /* Clear parity enable */
            newtio.c_iflag &= ~INPCK;	/* Enable parity checking */
            break;
    }
    switch (stopbits)//设置停止位
    {
        case 1:
            newtio.c_cflag &= ~CSTOPB;  //1
            break;
        case 2:
            newtio.c_cflag |= CSTOPB;  //2
            break;
        default:
            newtio.c_cflag &= ~CSTOPB;
            break;
    }
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;
    newtio.c_cflag	  |=   (CLOCAL|CREAD);
    newtio.c_oflag|=OPOST;
    newtio.c_iflag	  &=~(IXON|IXOFF|IXANY);
    cfsetispeed(&newtio,baudrate);
    cfsetospeed(&newtio,baudrate);
    tcflush(fd,	  TCIFLUSH);
    if (tcsetattr(fd,TCSANOW,&newtio) != 0)
    {
        perror("SetupSerial 3");
        return -1;
    }
    return 0;
}

int readport(int fd,char *buf,int len,int maxwaittime)//读数据，参数为串口，BUF，长度，超时时间
{
    int no=0;int rc;int rcnum=len;
    struct timeval tv;
    fd_set readfd;
    tv.tv_sec=maxwaittime/1000;	//SECOND
    tv.tv_usec=maxwaittime%1000*1000;  //USECOND
    FD_ZERO(&readfd);
    FD_SET(fd,&readfd);
    rc=select(fd+1,&readfd,NULL,NULL,&tv);
    if(rc>0)
    {
        while(len)
        {
            rc=read(fd,&buf[no],1);
            if(rc>0)
                no=no+1;
            len=len-1;
        }
        if(no!=rcnum)
            return -1;	   //如果收到的长度与期望长度不一样，返回-1
        return rcnum;	     //收到长度与期望长度一样，返回长度
    }
    else
    {
        return -1;
    }
    return -1;
}

int writeport(int fd,char *buf,int len)	 //发送数据
{
    write(fd,buf,len);
}

void clearport(int fd)	    //如果出现数据与规约不符合，可以调用这个函数来刷新串口读写数据
{
    tcflush(fd,TCIOFLUSH);
}

int checksmsSendSupport()
{
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;
    int ret = 1;

    profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
    /*支持本地短信发送*/
    if((profG != NULL) && (profG->head.active == TRUE) && (profG->enabled == 1U) && (profG->smsEnable == 1U)) {
	ret =1;
    }

    return ret;
}
void usage()
{
    printf(" usage:\n");
    printf("	smssend dev\n");
    printf("	    dev  -  3g serial port\n");
}

#define CURRENT_RCVBUF_LEN 1024
#define CURRENT_BUF_LEN 128
#define MAX_UNSIGNED_INT_VAL (4294967295)

void initSmsList()
{
    int circle = 0U;
    for(circle=0U;circle < SMS_LIST_LEN;circle++) 
    {
	memset(&(nvramProfile->smslist.smsnode[circle]), 0, sizeof(smsinfo));
	nvramProfile->smslist.smsnode[circle].flag = 0;
    }
}

#define MATCH_ADDR_STR "+CSCA:" 
void getAddrPhoneNum(char *dev)
{
    char rcvbuf[CURRENT_RCVBUF_LEN];
    char buf[CURRENT_BUF_LEN];
    char addre[15];
    int ret = 0;
    int fd = -1;
    int circle = 0;
    char *ptr = NULL;

    if(dev !=NULL) {
	MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
	WebAuthGlobalProfile *profG = NULL;

	profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
	memset(profG->addrPhone, 0, sizeof(profG->addrPhone));
    /*支持本地短信发送*/
	fd = openport(dev);     //打开串口
	if(fd>0)
	{
	    printf("%s-%d: \n",__func__,__LINE__);
	    ret=setport(fd,115200,8,1,'n');	 //设置串口，波特率，数据位，停止位，校验
	    if(ret<0)
	    {
		syslog(LOG_ERR, "[SMS] smssenf, Can't Set Serial Port [%s]!",dev);
		return ret;
	    }
	    ret = 0;
	    while(circle < 3) {
		memset(rcvbuf, 0, sizeof(rcvbuf));
		writeport(fd,"at",3);

		buf[0]=0x0d;
		writeport(fd,buf,1);
		writeport(fd,"AT+CSCA?\r",9);
		//clearport(fd);	    //如果出现数据与规约不符合，可以调用这个函数来刷新串口读写数据
		readport(fd,rcvbuf,CURRENT_RCVBUF_LEN,3000); //读数据，参数为串口，BUF，长度，超时时间
		printf("%s-%d: rcvbuf =%s\n",__func__,__LINE__,rcvbuf);
		memset(addre, 0, sizeof(addre));
		if(((ptr=strstr(rcvbuf, MATCH_ADDR_STR))!=NULL) && (strstr(rcvbuf, "OK")!=NULL))
		{
		    snprintf(addre, 15, "%s",ptr+8);
		    if((strncmp(addre,"+86",3)==0)&&(strlen(addre)==14))
		    {
			if((profG != NULL)) {
			    snprintf(profG->addrPhone, 15, "%s", addre);
			    ret =1;
			    printf("%s-%d: Get SMSC success =%s\n",__func__,__LINE__,addre);
			    break;
			}
		    } else {
			memset(addre, 0, sizeof(addre));
		    }
#if 0
		    if(sscanf(ptr, "+CSCA:\"%s\"", addre) ==1)
		    {
			printf("addre =%s\n",addre);
			if(strlen(addre) == 14) {
			    printf("get addre success.\n",addre);
			    ret = 1;
			    break;
			}
		    }
#endif
		}
		circle++;
	    }
	    if(ret != 1) {
		syslog(LOG_ERR, "[SMS] smssend, Can't Get SMSC!",dev);
	    }
	    close(fd);
	} else {
	    syslog(LOG_ERR, "[SMS] smssend, Can't Open Serial Port [%s]!",dev);
	}
    }

    return ret;
}

/*
 * 
 * */
int main(int argc, char **argv)
{
    int   fd=-1, ret=-1;
    char rcvbuf[CURRENT_RCVBUF_LEN];
    char buf[CURRENT_BUF_LEN];
    char sendbuf[MAX_SMS_DATA_LEN];
    char dev[MAX_DEV_NAME];
    unsigned int circle = 0U, null_item = 0U;
    int file_closed = 0;
    int need_break = 0;

    ProfInit();
    if(argc>=2) {
	if(strcmp(argv[1], "show")==0)
	{
	    for(circle=0U;circle < SMS_LIST_LEN;circle++) 
	    {
		printf("smsnode[%d].flag =%d\n",circle,nvramProfile->smslist.smsnode[circle].flag);
		printf("smsnode[%d].phone =%s\n",circle,nvramProfile->smslist.smsnode[circle].phone);
		printf("smsnode[%d].len =%d\n",circle,nvramProfile->smslist.smsnode[circle].len);
		printf("smsnode[%d].data =%s\n",circle,nvramProfile->smslist.smsnode[circle].data);
	    }
	    
	    ProfDetach();
	    return 0;
	} else if(strcmp(argv[1], "clean")==0) {
	    for(circle=0U;circle < SMS_LIST_LEN;circle++) 
	    {
		memset(&(nvramProfile->smslist.smsnode[circle]), 0, sizeof(smsinfo));
		printf("smsnode[%d].flag =%d\n",circle,nvramProfile->smslist.smsnode[circle].flag);
		printf("smsnode[%d].phone =%s\n",circle,nvramProfile->smslist.smsnode[circle].phone);
		printf("smsnode[%d].len =%d\n",circle,nvramProfile->smslist.smsnode[circle].len);
		printf("smsnode[%d].data =%s\n",circle,nvramProfile->smslist.smsnode[circle].data);
	    }
	    
	    ProfDetach();
	    return 0;
	} else if(strcmp(argv[1], "test") ==0) {

		strcpy(nvramProfile->smslist.smsnode[1].phone, "13817595514");
		nvramProfile->smslist.smsnode[1].len =117;
		strcpy(nvramProfile->smslist.smsnode[1].data, "0891683110304105F011000D91685168352822F40008B0663010827E6CF079D1628030114E0A7F515BC67801FF1A0031003900380039FF0C51ED6B64624B673A53F7548C5BC6780153EF514D8D394F7F75286B64590453CA51765B83005500540054514D8D390057006900460069FF0C5BC678016C384E4567096548FF01");
		nvramProfile->smslist.smsnode[1].flag =1;
	    ProfDetach();
	    return 0;
	}
	circle = 0U;
    }
    if(argc >= 2) 
    {
	if(strlen(argv[1]) > 0)
	{
	    snprintf(dev, 20,"/dev/%s",argv[1]);
	    snprintf(nvramProfile->smslist.dev, 20,"/dev/%s",argv[1]);
	    printf("%s-%d: dev %s\n",__func__,__LINE__,dev);
	}
	/*判断是否允许本地短信发送*/
	if(checksmsSendSupport() == 1) 
	{
	    getAddrPhoneNum(dev);
	    initSmsList();
	    while(1)
	    {
	
		fd = openport(dev);     //打开串口
		if(fd>0)
		{
	printf("%s-%d: \n",__func__,__LINE__);
		    ret=setport(fd,115200,8,1,'n');	 //设置串口，波特率，数据位，停止位，校验
		    if(ret<0)
		    {
			fprintf(stderr,"[SMS] Can't Set Serial Port [%s]!\n",dev);
			syslog(LOG_ERR, "[SMS] Can't Set Serial Port [%s]!",dev);
			return;
		    }
	printf("%s-%d: \n",__func__,__LINE__);

		    while(1) 
		    { 
			if(circle >=SMS_LIST_LEN) 
			    circle = 0U; 
			if(nvramProfile->smslist.smsnode[circle].flag == 1) 
			{
			    /*如果之前一直在执行空循环，即没有待发送的短信，说明此时的串口通信已关闭
			     * 需要退出此while循环，重新打开串口*/
			    if((need_break==1)){
				need_break = 0;
				null_item = 0;
				break;
			    }
			    memset(sendbuf, 0, sizeof(sendbuf));
			    memset(rcvbuf, 0, sizeof(rcvbuf));
#if 1
			    writeport(fd,"at",3);

			    buf[0]=0x0d;
			    writeport(fd,buf,1);
			    writeport(fd,"AT+CMGF=0\r",10);
			    sleep(1);
			    snprintf(buf, CURRENT_BUF_LEN, "AT+CMGS=%d\r",nvramProfile->smslist.smsnode[circle].len);
			    writeport(fd,buf,strlen(buf));
			    sleep(1);
			    snprintf(sendbuf, MAX_SMS_DATA_LEN, "%s",nvramProfile->smslist.smsnode[circle].data);
			    writeport(fd,sendbuf,strlen(sendbuf));
			    buf[0]=0x1a;
			    writeport(fd,buf,1);
			    sleep(1);
			    clearport(fd);	    //如果出现数据与规约不符合，可以调用这个函数来刷新串口读写数据
			    readport(fd,rcvbuf,CURRENT_RCVBUF_LEN,3000); //读数据，参数为串口，BUF，长度，超时时间
			    printf("%s-%d: rcvbuf =%s\n",__func__,__LINE__,rcvbuf);
#else
			    writeport(fd,"at",3);

			    buf[0]=0x0d;
			    writeport(fd,buf,1);
			    writeport(fd,"AT+CMGF=1\r",10);
			    sleep(1);
			    snprintf(buf, CURRENT_BUF_LEN, "AT+CMGS=\"+86%s\"\r",nvramProfile->smslist.smsnode[circle].phone);
			    writeport(fd,buf,strlen(buf));
			    sleep(1);
			    snprintf(sendbuf, MAX_SMS_DATA_LEN, "%s",nvramProfile->smslist.smsnode[circle].data);
			    writeport(fd,sendbuf,strlen(sendbuf));
			    buf[0]=0x1a;
			    writeport(fd,buf,1);
			    sleep(1);
			    clearport(fd);	    //如果出现数据与规约不符合，可以调用这个函数来刷新串口读写数据
			    readport(fd,rcvbuf,CURRENT_RCVBUF_LEN,3000); //读数据，参数为串口，BUF，长度，超时时间
			    printf("%s-%d: rcvbuf =%s\n",__func__,__LINE__,rcvbuf);

#endif
			    printf("\n");
			    /*free send buf*/
			    memset(&(nvramProfile->smslist.smsnode[circle]), 0, sizeof(smsinfo));
			    nvramProfile->smslist.smsnode[circle].flag = 0;
			    null_item = 0;
			    sleep(2); /*等待2s，再发送下一条短信*/
			} else {
			    null_item++;
			}
			/* 如果null_item >= SMS_LIST_LEN
			 * 表示已经运行了一个空循环，认为此事没有待发送的短信
			 * 退出玄幻，暂时先关闭串口通信*/
			if(null_item > SMS_LIST_LEN){
			    if(file_closed != 1) {
				close(fd);
				fd = -1;
				file_closed = 1;
			    }
			    need_break = 1;
			    null_item = 0;
			    sleep(2); 
			    //break;
			}
			circle++;
		    }
#if 0
		    null_item = 0;
		    close(fd);
		    if(checksmsSendSupport() == 0) {
			/*再次判断本地发送短信是否支持，不支持则退出此程序*/
			//break;
		    }
#endif
		} else {
		    fprintf(stderr,"[SMS] Can't Open Serial Port [%s]!\n",dev);
		    syslog(LOG_ERR, "[SMS] Can't Open Serial Port [%s]!",dev);
		    return;
		}
		//sleep(5); /*睡眠5s，然后再重新打开串口通信*/
	    }
	}
    } else {
	usage();
    }
    ProfDetach();
    return 0;
}
#else
int main(int argc, char **argv)
{
    return 0;
}

#if 0
main()
{
    char rcvbuf[CURRENT_RCVBUF_LEN];
    int   fd,rc,i,ret;
    char buf[CURRENT_BUF_LEN];
    char *dev ="/dev/ttyUSB3";    //串口号 /dev/ttyS0  对应于串口1
    fd	=  openport(dev);     //打开串口
    if(fd>0)
    {
        ret=setport(fd,115200,8,1,'n');	 //设置串口，波特率，数据位，停止位，校验
        if(ret<0)
        {
            printf("Can't Set Serial Port!\n");
            exit(0);
        }
    }
    else
    {
        printf("Can't Open Serial Port!\n");
        exit(0);
    }
    {
        {
#if 1
            writeport(fd,"at",3);

            buf[0]=0x0d;
            writeport(fd,buf,1);
            writeport(fd,"AT+CMGF=0\r",10);
            sleep(1);
	    //clearport(fd);	    //如果出现数据与规约不符合，可以调用这个函数来刷新串口读写数据
	    //readport(fd,rcvbuf,CURRENT_RCVBUF_LEN,1000); //读数据，参数为串口，BUF，长度，超时时间
	    //printf("%s-%d: rcvbuf =%s\n",__func__,__LINE__,rcvbuf);
            writeport(fd,"AT+CMGS=117\r",12);
            sleep(1);
	    //clearport(fd);	    //如果出现数据与规约不符合，可以调用这个函数来刷新串口读写数据
	    //readport(fd,rcvbuf,CURRENT_RCVBUF_LEN,1000); //读数据，参数为串口，BUF，长度，超时时间
	    //printf("%s-%d: rcvbuf =%s\n",__func__,__LINE__,rcvbuf);
            writeport(fd,"0891683110304105F011000D91683118575915F40008B0663010827E6CF079D1628030114E0A7F515BC67801FF1A0031003400300037FF0C51ED6B64624B673A53F7548C5BC6780153EF514D8D394F7F75286B64590453CA51765B83005500540054514D8D390057004900460049FF0C5BC678016C384E4567096548FF01",252);
            buf[0]=0x1a;
            writeport(fd,buf,1);
	    sleep(1);
	    clearport(fd);	    //如果出现数据与规约不符合，可以调用这个函数来刷新串口读写数据
	    readport(fd,rcvbuf,CURRENT_RCVBUF_LEN,10000); //读数据，参数为串口，BUF，长度，超时时间
	    printf("%s-%d: rcvbuf =%s\n",__func__,__LINE__,rcvbuf);
#else
            writeport(fd,"at",3);

            buf[0]=0x0d;
            writeport(fd,buf,1);
            writeport(fd,"AT+CMGF=1\r",10);
            sleep(1);
            writeport(fd,"at+cmgs=\"+8618516151111\"\r",25);
            sleep(1);
            writeport(fd,"ff",2);
            buf[0]=0x1a;
            writeport(fd,buf,1);

#endif
            printf("\n");
        }
        close(fd);
    }
}
#endif
#endif
#endif
