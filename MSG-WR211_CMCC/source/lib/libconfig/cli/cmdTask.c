/*** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***
 ***                                                               ***
 ***                  CONFIDENTIAL INFORMATION                     ***
 ***                  ------------------------                     ***
 ***    This Document contains Confidential Information or         ***
 ***    Trade Secrets, or both, which are the property of UTT      ***
 ***    Technologies, Inc.    This document may not be  copied,    ***
 ***    reproduced, reduced to any electronic medium or machine    ***
 ***    readable form or otherwise duplicated and the information  ***
 ***    herein may not be used, disseminated or otherwise          ***
 ***    disclosed, except with the prior written consent of UTT    ***
 ***    Technologies, Inc.                                         ***
 ***                                                               ***
 *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***/

/*
    Copyright 1993-1998 UTT Technologies, Inc.
    All rights reserved.
    Use of copyright notice does not imply publication.
 */


/*
 *
 * File:      cmdTask.c
 *
 * Overview:  Interface between UTT and syslog2.0
 */

#include "flags.h"
#include "webScript.h"
#include "wusWebServer.h"
#include "cmdlist.h"
//add by jyshen 070308
#if (IPP2P == FYES)
#include "linux_config.h"
#include "ipt_ipp2p.h"
#endif
//end jyshen

typedef void (cmdFunc)(void*);

typedef struct _preDefinedCmd 
{
	const char* cmdName;
	cmdFunc* func;
} preDefinedCmd;

static void Reboot(void*)
{
	wioReloadDevice();
}

extern bool ArpRequest();
extern bool BindAll_Config();

static void Bindall(void*)
{
	ArpRequest();
	sleep(1000);
	BindAll_Config();
}
	
preDefinedCmd pdbCmd[] = {
		{"reboot", Reboot},
		{"bindall", Bindall},
		{NULL, NULL},
};

void cmdAuto_heartbeat(unsigned long tag1, unsigned long tag2);

MAILBOX* cmdAutoMailBox;
static Bool isDebug = FALSE;	//added by jyshen 070312

int cmdAutoInterval = 10;  //10 second
int resetInterval = 1;      // 1 second
int resetTimeCount = 0;
#define PREDEFINED 0x02 

static void cmdAutoTask(Pointer p);

//added by jyshen 070312
static void autoDebugCmd( void )
{
	isDebug = !isDebug;
	if(isDebug) {
		printf("Autocmd debug On\r\n");
	}
	else {
		printf("Autocmd debug Off\r\n");
	}
}
//end by jyshen

void cmdAutoInit()
{
	//add by jyshen 070308
	cmdpdb_createhead(&cmdpdbhead);	
	registerForProfileUpdates(PROFILE_PDB, 0,  C_CAST(ProfileUpdateFnc)cmdpdbProfChanged);
	mtMonitorCreateCommand( "autocmd", "autocmd", autoDebugCmd, FALSE);
	//end jyshen
	cmdAutoMailBox = createMailbox();
	createTask((TaskFunction)cmdAutoTask, TASK_PRI_2, MED_STACK, NULL, "cmdAutoTask");

}

#define 	INTERVAL 	10

static Uint timeToInt(Uint8 uHour, Uint8 uMinute, Uint8 uSec)
{
	//Uint time = 0, timeLoop = 10, uDay = 0;
        // commented by llliu at REOS WSA(Warning Sweeping Action). 
	Uint time = 0,  uDay = 0;

	if (uHour == 0xff)
	{
		uHour = 0;
	}

	if (uMinute == 0xff)
	{
		uMinute = 0;
	}
	
	if (uSec > 59)
	{
		uSec = 0;
		uMinute++;

		if (uMinute > 59)
		{
			uMinute = 0;
			uHour++;

			if (uHour > 23) 
			{
				uHour = 0;
				uDay = 24 * 3600;
			}
		}
	}

	time = uDay + uHour * 3600 + uMinute* 60 + uSec;
	return time;
}

#if (IP_SEC == FYES)
char *trimstr(char *str0);
void sendIcmpPacketTest(Uint32 ping_src,Uint32 ping_dst);
void cmdAutoPing_x(char *line);
#endif

void cmdDhcpAutoBind();
extern Boolean ipIsDhcpAssigned(Uint32 ip);
extern void BindDhcpAssigned(Uint32 *ips,int count);

void cmdAutoRun()
{
	Bool isRun = FALSE;
	char line[MAX_CMDLINE_SIZE+3] = {0};
	//static Bool isAlredyRun = FALSE;
	Uint interval = cmdAutoInterval, time1 = 0, time2 = 0;
	const Uint8 uFlag = 0xff, uIntervalFlag = 60;
	cmdpdbcontent* cmdNode = NULL;
	static AbsoluteTime myCurrentTime = 0;
	ExpandedTime expTime;
    	ExpandedDate expDate;

	requestAbsoluteTime(&myCurrentTime);
	absoluteTimeToExpandedDate(myCurrentTime, &expDate);
	absoluteTimeToExpandedTime(myCurrentTime, &expTime, TRUE);
	if (isDebug) printf("day = %d currenttime = %d:%d:%d\n", expDate.days, expTime.hours, expTime.minutes, expTime.seconds);
	cmdNode = cmdpdbhead->next;

	while (cmdNode != NULL)
	{
		if (cmdNode->isEnable == FALSE)	//判断开关标志位
		{
			cmdNode = cmdNode->next;
			continue;
		}
		
		
		switch (cmdNode->type)
		{	
			case 0x01:	//every week
				//do sth				
				if (cmdNode->Uweek == expDate.days)
				{
					if (cmdNode->uHour == 0xff || cmdNode->uMinute == 0xff || 
						cmdNode->uSecond == 0xff)
					{
						break;
					}

					time1 = timeToInt(expTime.hours, expTime.minutes, 
						expTime.seconds);
					time2 = timeToInt(cmdNode->uHour, cmdNode->uMinute, 
						cmdNode->uSecond);
					
					if ( ((time1 - time2) < interval && (time1 - time2) >= 0) ) //&& cmdNode->isActive)
					{
						isRun = TRUE;
					}
				}
			break;
			case 0x02:	//everyday
				//do sth
				if (cmdNode->uHour == 0xff || cmdNode->uMinute == 0xff || 
					cmdNode->uSecond == 0xff)
				{
						break;
				}

				time1 = timeToInt(expTime.hours, expTime.minutes, 
						expTime.seconds);
				time2 = timeToInt(cmdNode->uHour, cmdNode->uMinute, 
						cmdNode->uSecond);

				if ( ((time1 - time2) < interval && (time1 - time2) >= 0) )	//&& cmdNode->isActive)
				{
					isRun = TRUE;
				}
			break;
			case 0x03:	//everyhour
				//do sth				
				if (cmdNode->uMinute == 0xff || cmdNode->uSecond == 0xff)
				{
					break;
				}

				time1 = timeToInt(uFlag, expTime.minutes, expTime.seconds);
				time2 = timeToInt(uFlag, cmdNode->uMinute, cmdNode->uSecond);

				if ( ((time1- time2) < interval && (time1 - time2) >= 0) )	//&& cmdNode->isActive)
				{
					isRun = TRUE;
				}
			break;
			case 0x04:	//everymin
				//do sth
				if (cmdNode->uSecond == 0xff)
				{
					break;
				}

				time1 = timeToInt(uFlag, uFlag, expTime.seconds);
				time2 = timeToInt(uFlag, uFlag, cmdNode->uSecond);

				if ( ((time1 - time2) < interval && (time1 - time2) >= 0) )	//&& cmdNode->isActive)
				{
					isRun = TRUE;
				}
			break;
			case 0x05:	//run cmd after boot-strap
				//do sth
				//waitForInitCompletion();
				//通过运行时间来判断是否刚开机
				if (sysUpTime() > uIntervalFlag || !cmdNode->isActive)
				{
					break;
				}
				
				isRun = TRUE;
				cmdNode->isActive = FALSE;
			break;
		}

		if (isRun)
		{	
			if (cmdNode->uFlag == PREDEFINED)
			{
				int i = 0;
				
				while (pdbCmd[i].cmdName != NULL)
				{
					if (strcmpi(pdbCmd[i].cmdName, cmdNode->cmdLine) == 0)
					{
						sprintf(line, "%s\r\n", cmdNode->cmdLine);
						if (isDebug) printf(line);
						pdbCmd[i].func(NULL);
						break;
					}

					i++;
				}
			}
                        else
                        {
#if (IP_SEC == FYES)
                                sprintf(line, "%s", cmdNode->cmdLine);
                                if( strlen(line) > 20 && (!strncmp("ping -x",line,7)) ) {
					cmdAutoPing_x(line);
                                }
                                else {
#endif
					sprintf(line, "%s", cmdNode->cmdLine);
					if(!strcmp("DHCPAUTOBIND",line)) {
						cmdDhcpAutoBind();
					}
					else {
                                        	sprintf(line, "%s\r\n", cmdNode->cmdLine);
                                        	cliParseData(line);
                                        	if (isDebug) printf(line);
					}
#if (IP_SEC == FYES)
                                }
#endif
                        }

			isRun = FALSE;
		}
		
		cmdNode = cmdNode->next;
	}
	
	//switch ()
}

#if 0
void cmdAutoRun()
{
	char line[MAX_PDBCONTENT_SIZE+2] = {0};
	Uint interval = 10, time1 = 0, time2 = 0;
	const Uint8 uFlag = 0xff, uIntervalFlag = 60;
	cmdpdbcontent* cmdNode = NULL;
	AbsoluteTime myCurrentTime = 0;
	ExpandedTime expTime;
    	ExpandedDate expDate;
	requestAbsoluteTime(&myCurrentTime);
	absoluteTimeToExpandedDate(myCurrentTime, &expDate);
	absoluteTimeToExpandedTime(myCurrentTime, &expTime, TRUE);
	printf("day = %d time1 = %d:%d:%d\n", expDate.days, expTime.hours, expTime.minutes, expTime.seconds);
	cmdNode = cmdpdbhead->next;

	while (cmdNode != NULL)
	{
		if (cmdNode->isEnable == FALSE)	//判断开关标志位
		{
			cmdNode = cmdNode->next;
			continue;
		}
		
		switch (cmdNode->type)
		{
			case 0x01:	//every week
				//do sth
				if (!cmdNode->isActive && cmdNode->uLastDay != 0xffffffff)
				{
					if (cmdNode->uLastDay != (expDate.year * 365 + expDate.month * 30 + expDate.date))
					{
						cmdNode->isActive = TRUE;
						cmdNode->uLastDay = 0xffffffff;
					}
				}
				
				if (cmdNode->Uweek == expDate.days)
				{
					if (cmdNode->uHour == 0xff || cmdNode->uMinute == 0xff || 
						cmdNode->uSecond == 0xff)
					{
						break;
					}

					time1 = timeToInt(expTime.hours, expTime.minutes, 
						expTime.seconds);
					time2 = timeToInt(cmdNode->uHour, cmdNode->uMinute, 
						cmdNode->uSecond + interval);
					
					if ( ((time2 - time1) <= interval && (time2 - time1) >= 0) && cmdNode->isActive)
					{
						sprintf(line, "%s\r\n", cmdNode->cmdLine);
						printf(line);
						cliParseData(line);
						cmdNode->isActive = FALSE;
						cmdNode->uLastDay = expDate.year * 365 + expDate.month  * 30 + expDate.date;
					}
				}
			break;
			case 0x02:	//everyday
				//do sth
				
				if (!cmdNode->isActive && cmdNode->uLastDay != 0xffffffff)
				{
					if (cmdNode->uLastDay != (expDate.year * 365 + expDate.month * 30 + expDate.date))
					{
						cmdNode->isActive = TRUE;
						cmdNode->uLastDay = 0xffffffff;
					}
				}
				

				if (cmdNode->uHour == 0xff || cmdNode->uMinute == 0xff || 
						cmdNode->uSecond == 0xff)
				{
						break;
				}

				time1 = timeToInt(expTime.hours, expTime.minutes, 
						expTime.seconds);
				time2 = timeToInt(cmdNode->uHour, cmdNode->uMinute, 
						cmdNode->uSecond + interval);

				if ( ((time2 - time1) <= interval && (time2 - time1) >= 0)  cmdNode->isActive)
				{
					sprintf(line, "%s\r\n", cmdNode->cmdLine);
					printf(line);
					cliParseData(line);
					cmdNode->isActive = FALSE;
					cmdNode->uLastDay = expDate.year * 365 + expDate.month * 30 + expDate.date;
				}
			break;
			case 0x03:	//everyhour
				//do sth
				
				if (!cmdNode->isActive && cmdNode->uLastHour != 0xff)
				{
					if (cmdNode->uLastDay != (expDate.year * 365 + expDate.month * 30 + expDate.date) ||
						cmdNode->uLastHour != expTime.hours)
					{
						cmdNode->isActive = TRUE;
						cmdNode->uLastDay = 0xffffffff;
						cmdNode->uLastHour= 0xff;
					}
				}
				
				
				if (cmdNode->uMinute == 0xff || cmdNode->uSecond == 0xff)
				{
					break;
				}

				time1 = timeToInt(uFlag, expTime.minutes, expTime.seconds);
				time2 = timeToInt(cmdNode->uHour, cmdNode->uMinute, 
						cmdNode->uSecond + interval);

				if ( ((time2 - time1) <= interval && (time2 - time1) >= 0) && cmdNode->isActive)
				{
					sprintf(line, "%s\r\n", cmdNode->cmdLine);
					printf(line);
					cliParseData(line);
					cmdNode->isActive = FALSE;
					cmdNode->uLastDay = expDate.year * 365 + expDate.month * 30 + expDate.date;
					cmdNode->uLastHour= expTime.hours;
				}
			break;
			case 0x04:	//everymin
				//do sth
				
				if (!cmdNode->isActive && cmdNode->uLastMinute!= 0xff)
				{
					if (cmdNode->uLastDay != (expDate.year * 365 + expDate.month * 30 + expDate.date) ||
						cmdNode->uLastHour != expTime.hours ||
						cmdNode->uLastMinute!= expTime.minutes)
					{
						cmdNode->isActive = TRUE;
						cmdNode->uLastDay = 0xffffffff;
						cmdNode->uLastHour= 0xff;
						cmdNode->uLastMinute= 0xff;
					}
				}
				
				
				if (cmdNode->uSecond == 0xff)
				{
					break;
				}

				time1 = timeToInt(uFlag, uFlag, expTime.seconds);
				time2 = timeToInt(cmdNode->uHour, cmdNode->uMinute, 
						cmdNode->uSecond + interval);

				if ( ((time2 - time1) <= interval && (time2 - time1) >= 0) && cmdNode->isActive)
				{
					sprintf(line, "%s\r\n", cmdNode->cmdLine);
					printf(line);
					cliParseData(line);
					cmdNode->isActive = FALSE;
					cmdNode->uLastDay = expDate.year * 365 + expDate.month * 30 + expDate.date;
					cmdNode->uLastHour= expTime.hours;
					cmdNode->uLastMinute= expTime.minutes;
				}
			break;
			case 0x05:	//run cmd after boot-strap
				//do sth
				//waitForInitCompletion();
				//通过运行时间来判断是否刚开机
				if (sysUpTime() > uIntervalFlag || !cmdNode->isActive)
				{
					break;
				}
				
				sprintf(line, "%s\r\n", cmdNode->cmdLine);
				printf(line);
				cliParseData(line);
				cmdNode->isActive = FALSE;
			break;
		}

		cmdNode = cmdNode->next;
	}
	
	//switch ()
}
#endif
extern bool clrNvramPressed();
void reset_heartbeat(unsigned long tag1, unsigned long tag2)
{
	sendMsg(cmdAutoMailBox , (void *)"r");
}
void cmdResetRun()
{
	if(clrNvramPressed())
	{
		resetTimeCount++;
		if(resetTimeCount == 5)
		{	
			resetTimeCount = 0;
			printf("RESET button pressed, clearing user NVRAM profile...\r\n");
			buildNvram(TRUE);
			commandReset(RESET_BY_NVRAM_REBUILT);
			//printf("RESET button pressed\n");
		}
	}
	else if(resetTimeCount !=0)
		resetTimeCount = 0;
}

static void cmdAutoTask(Pointer p)
{
 
	char *cmdMsg = NULL;
	waitForInitCompletion();
	timeout(cmdAuto_heartbeat, 0L, 0L, cmdAutoInterval*10);  
	timeout(reset_heartbeat,0L,0L,resetInterval*10);
	while (true)
	{
		

		cmdMsg = (char *)recvMsg(cmdAutoMailBox);

		if (cmdMsg == NULL)
		{
			continue;	
		}
		else if(strcmp(cmdMsg,"r") == 0)
		{
			cmdResetRun();
			timeout(reset_heartbeat,0L,0L,resetInterval*10);
		}
		else
		{
			cmdAutoRun();
			timeout(cmdAuto_heartbeat, 0L, 0L, cmdAutoInterval*10);
		}
	}
}
# if 0
static void cmdAutoTask(Pointer p)
{
 
	char *cmdMsg = NULL;
	waitForInitCompletion();
	timeout(cmdAuto_heartbeat, 0L, 0L, cmdAutoInterval*10);  
	while (true)
	{

		cmdMsg = (char *)recvMsg(cmdAutoMailBox);

		if (cmdMsg == NULL)
		{
			continue;	
		}
		cmdAutoRun();
		timeout(cmdAuto_heartbeat, 0L, 0L, cmdAutoInterval*10);  
	}
}
# endif
void cmdAuto_heartbeat(unsigned long tag1, unsigned long tag2)
{
	sendMsg(cmdAutoMailBox , (void *)"hi");
}

#if (IP_SEC == FYES)

void sendIcmpPacketTest(Uint32 ping_src,Uint32 ping_dst)
{

#define ICMP_PACK_TEST_LEN 20
  ICMP_PKT       *icmp;
  int             plen;

    if( (ping_src == 0) || (ping_dst == 0) ) return;
    plen = ICMP_PACK_TEST_LEN +
                sizeof(icmp->icmpp_ip) + sizeof(icmp->icmpp_mac) + sizeof(icmp->icmpp_icmp);

    icmp = (ICMP_PKT*) MALLOC(plen);
    if (!icmp)  return;

    memset(icmp, 0, plen);

    /* set ip_src */

    icmp->icmpp_ip.ip_src = ping_src;

    /*
     * * Set the opcode.
     */

    icmp->icmpp_icmp.icmp_type = ICMP_ECHO_REQ;
    icmp->icmpp_icmp.icmp_code = 0;
    icmp->icmpp_icmp.icmp_chsum = 0;

    icmp->icmpp_icmp.icmp_part1 = htons(50);
    icmp->icmpp_icmp.icmp_part2 = htons(51);

     /*
     * * Perform checksum on the icmp part.
     */

    icmp->icmpp_icmp.icmp_chsum =
        ip_checksum(&icmp->icmpp_icmp, (ICMP_PACK_TEST_LEN + sizeof(ICMP_HDR)));

    /*
     * * send it out with time-to-live=(u_char)ip_stats.ips_default_ttl;.
     */
    ip_send_opts(icmp, ICMP_PACK_TEST_LEN + sizeof(ICMP_HDR),
                       ping_dst,
                       PROT_ICMP,
                       0,                               /* Type of Service = 0 */
                       0,                               /* Assign us an id     */
                       FALSE,                           /* Allow Fragmentation */
                       ip_stats.ips_default_ttl,        /* Maximum ttl         */
                       TRUE,                            /* Locally generated   */
                       PACKET_BYPASS_IP_ROUTING,        /* no policy_routing flags */
                       GLOBAL_VROUTER_ID);

    return;
}
char *trimstr(char *str0)
{
        char *p;
        if(strlen(str0) == 0) return str0;
        p = str0;
        while(*p != '\0') {
                if(*p == ' ') p++;
                else break;
        }
        return p;
}

void cmdAutoPing_x(char *line)
{
	char *pp,*qq,*rr;
        Uint32 ipsrc,masksrc,ipdst,maskdst;
        pp = strstr(line,"ping -x");
        if(pp!= NULL) {
        	qq = pp + strlen("ping -x");
                pp = trimstr(qq);
                qq = pp;
                rr = strstr(qq,".");
                if(rr != NULL) {
                	pp = strstr(rr," ");
                        if(pp != NULL) {
                       		rr = trimstr(pp+1);
                                *pp = '\0';
                                ipAddressStringToValue(qq,&ipsrc,&masksrc);
                                ipAddressStringToValue(rr,&ipdst,&maskdst);
                                if(isDebug) printf("ping -x %N %N\r\n",ipsrc,ipdst);
                                sendIcmpPacketTest(ipsrc,ipdst);
                        }
                }

        }
}

#endif

void cmdDhcpAutoBind()
{
	int i,instHigh,instLow;
        NPW_TYPE value;
        MibProfileType proftype = MIB_PROF_NAME_PASS;
	struProfAlloc *proflist = NULL;
	Uint32 ipaddr;
	const int MAX_DEL_DHCP_BIND = 200; 
	const int MAX_BINDED_COUNT = 700;
	int delIndex[MAX_DEL_DHCP_BIND],delcount=0;
	char nickname[MAX_ARGLEN];
	static Uint32 *ipBinded = NULL;
	int bindedCount=0;

	if(ipBinded == NULL) {
		ipBinded = (Uint32 *)MALLOC(sizeof(ipaddr)*MAX_BINDED_COUNT);
		if(ipBinded == NULL) {
			printf("cmdDhcpAutoBind: memory malloc failed!\r\n");
			return;
		}
	}

	if(isDebug) printf("Dhcp Auto Bind IP/MAC !!\r\n");

        instHigh = WebProfInstHigh(proftype);
        instLow = WebProfInstLow(proftype);
        
	for(int i = instLow; i <= instHigh; i++) {
                if(InstIsActive(proftype, i) == FALSE) 
		{
		    continue;
		}
                ArgGetInt(proftype,i,"type",(int *)&value);
		/*updated by jfgu@090626*/
		//if ((value == NPW_DHCP_STATIC) || (value == NPW_ARP_STATIC))	
                if(((value & NPW_DHCP_STATIC ) == NPW_DHCP_STATIC)
		    || (value == NPW_DHCP_AUTOBIND))
                {
                        ArgGetInt(proftype,  i, T("ip"), (int *)&ipaddr);
			ArgGetStr(proftype,  i, T("nick"),(char *)nickname);
			if(ipaddr != 0 && bindedCount < MAX_BINDED_COUNT) {
				ipBinded[bindedCount++] = ipaddr;
			}
			if (isDebug) printf("user dhcp binding ,ip is %N\r\n",ipaddr);
			if(strcmp(nickname,"guest") == 0 ) {
				if( (ipaddr != 0) && (delcount < MAX_DEL_DHCP_BIND) && (ipIsDhcpAssigned(ipaddr) == FALSE) ) {
					delIndex[delcount++] = i; // remember the index to del
					bindedCount--;  // delete the ip from the binded ip list
				}
			}
                }
        }
	if (isDebug) printf("we will delete %d user(dhcp)!\r\n",delcount);
	setWebProfError(eWebProfOk,NULL);
	for(i=0;i<delcount;i++) {
		WebProfBackupByIndex(proftype, delIndex[i], &proflist); 
		webProfDelInstByIndex(proftype, delIndex[i]);
	}
	if(webLastError == eWebProfOk) {//no error occured
		WebProfUpdate(proflist);
    	}
    	else {	 
		WebProfRestore(proflist); 	
    	}
	FreeProfAllocList(proflist);
	//binding the ip which not bind
	BindDhcpAssigned(ipBinded,bindedCount);
}

void bindDhcpAssignedIpMac(char *ip,char *mac)
{
        MibProfileType proftype = MIB_PROF_NAME_PASS;
        struProfAlloc *proflist = NULL;
        const char *type;
        int index = -1;
        Boolean isNew = FALSE;
        if(ip == NULL || mac == NULL) return;
	if (isDebug) printf("bindDhcpAssignedIpMac ip is %s,mac is %s\r\n",ip,mac);
        if( (index=GetInstIndexByArgName(proftype, NULL,ip)) == -1 ) {
                if((index=webProfNewInst(proftype, ip, FALSE)) == -1) {
                        return;
                }
                isNew = TRUE;
        }
	/*updated by jfgu@090626*/
	//type = ArgEnumString(proftype, "type", NPW_DHCP_AUTOBIND);
        type = ArgEnumString(proftype,"type",NPW_DHCP_STATIC);
        WebProfBackupByIndex(proftype, index, &proflist);
        setWebProfError(eWebProfOk,NULL);
        ArgSetValue(proftype,ip,"nick","guest");
        ArgSetValue(proftype, ip, "type", type);                //set user type DHCP_STATIC
        ArgSetValue(proftype, ip, "ip", ip);            
        ArgSetValue(proftype, ip, "mac", mac);
        if(webLastError == eWebProfOk) {
                WebProfUpdate(proflist);
        }
        else {
                if(isNew) {
                        webProfDelInstByIndex(proftype, index);
                }else {
                        WebProfRestore(proflist);
                }
        }
	FreeProfAllocList(proflist);
}
