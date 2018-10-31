#include "uttMachine.h"
#include "typedef.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "conver.h"
#include "mxml.h"
#include "uttMachine.h"
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
#include<time.h>
#endif
#if(IP_GRP == FYES)
#ifdef  IP_GRP_DEBUG1
#define IPGRP_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define IPGRP_PRINTF(fmt, args...)
#endif
#endif

#if(PORT_GRP == FYES)
#ifdef   PORT_GRP_DEBUG
#define  PORTGRP_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define  PORTGRP_PRINTF(fmt, args...)
#endif
#endif

extern struct st_xmlConfig  rootConfig[];
/*
 *get members from config buffer
 *
 *KeHuatao
 *2011-09-02
 */
static void profSetMembersFromStr(Pointer prof, Uint32 offset, Uint32 size, const char* buf)
{
    Uint32 i, l;
    unsigned char *members;

    l = strlen(buf);
    members = (unsigned char*)prof + offset;
    if(l>size)
    {
	l = size;
    }
    i = 0u;
    while(i<l)
    {
	if(*(buf+i) == '0')
	{
	    *(members + i) = 0;
	}
	else
	{
	    *(members + i) = 1;
	}
	i++;
    }
}
/*
 *uchar 数组类型的数据转换为str
 *
 *KeHuatao
 *
 *2011-09-02
 */
static void profGetMembersToStr(Pointer prof, Uint32 offset, Uint32 size, char* buf)
{
    Uint32 i;
    unsigned char *members, c;

    /*调整下标*/
    members = (unsigned char*)prof + offset;
    for(i=0u;i<size;i++)
    {
	c = *(members+i);
	if(c==0)
	{
	    /*非成员*/
	    buf[i] = '0';
	}
	else
	{
	    /*成员*/
	    buf[i] = '1';
	}
    }

    return;
}

static void profGetMacToStr(Pointer prof, Uint32 offset, Uint32 size, char* buf)
{
    Byte* tmp = NULL;
    tmp = ((MacAddr*)((Byte*)prof + offset))->addr;
    snprintf(buf, MAX_VALUE_LEN, "%02x:%02x:%02x:%02x:%02x:%02x", 
            (Uint8)(*tmp), (Uint8)*(tmp+1), (Uint8)*(tmp+2), (Uint8)*(tmp+3), (Uint8)*(tmp+4), (Uint8)*(tmp+5));
}

static void profSetMacFromStr(Pointer prof, Uint32 offset, Uint32 size, const char* buf)
{
    int i = 0, j = 0;
    char tmp[3] = {0};
    tmp[2] = 0;
    for(i = 0; i < 6; i++) {
        for(j = 0; j < 2; j++) {
            tmp[j] = *(buf + i*2 + j);
            ((MacAddr*)((Byte*)prof + offset))->addr[i] = strtol(tmp, NULL, 16);
//            *(Uint8*)((Byte*)prof + offset + i) = strtol(tmp, NULL, 16);
        }
        buf++;
    }
}
#if (IP_GRP == FYES)
static void profGetIpEntryToStr(Pointer prof, Uint32 offset, Uint32 size, char *buf)
{
    IPGRP_PRINTF("%s-%d:prof.head.name=%s,grpType=%d,size=%d,buf=%s\n",__FUNCTION__,__LINE__,((IpGrpProfile *)prof)->head.name,((IpGrpProfile *)prof)->grpType,size,buf);
    struct in_addr addrFrom;
    struct in_addr addrTo;
    char iptmp[UTT_IPGRP_NAME_LEN]={0};
    ipGrpEntry* ipEntry = (ipGrpEntry*)((Byte *)prof+offset);
    if(ipEntry->entryType==IPENT_GRP)
    {
	sprintf(buf,"G(%s)",ipEntry->entryVal.name);
	IPGRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
    }
#if (WEB_AUTH == FYES)
    else if(ipEntry->entryType==IPENT_WEB_AUTH)
    {
	sprintf(buf,"W(%s)",ipEntry->entryVal.name);
    }
#endif
#if (PPPOE_SERVER == FYES)
    else if(ipEntry->entryType==IPENT_POE_ACC)
    {
	sprintf(buf,"P(%s)",ipEntry->entryVal.name);
    }
#endif
    else if(ipEntry->entryType==IPENT_RANGE)
    {
    addrFrom.s_addr = ipEntry->entryVal.range.ipFrom;
    strcpy(iptmp,inet_ntoa(addrFrom));
    addrTo.s_addr = ipEntry->entryVal.range.ipTo;
    IPGRP_PRINTF("%s-%d:ipfrom=%x,ipto=%x\n",__FUNCTION__,__LINE__,ipEntry->entryVal.range.ipFrom,ipEntry->entryVal.range.ipTo);
    sprintf(buf, "P(%s-%s)", iptmp, inet_ntoa(addrTo));
    }
}

static void profSetIpEntryFromStr(Pointer prof, Uint32 offset, Uint32 size, const char *buf)
{
    ipGrpEntry* ipEn=(ipGrpEntry *)((Byte *)prof+offset);
    struct in_addr fromip, endip;
    char* start=NULL,*start1=NULL;
    char* end=NULL,*end1=NULL;
    end=strstr(buf,")");
    if(end){
    *end='\0';
#if 1
    if(buf[0]=='G')
    {
	ipEn->entryType=IPENT_GRP;
	start=strstr(buf,"(");
	strcpy(ipEn->entryVal.name,start+1);
	IPGRP_PRINTF("%s-%d:name=%s\n",__FUNCTION__,__LINE__,ipEn->entryVal.name);
    }
#if (WEB_AUTH == FYES)
    else if(buf[0]=='W')
    {
	ipEn->entryType=IPENT_WEB_AUTH;
	start=strstr(buf,"(");
	strcpy(ipEn->entryVal.name,start+1);
	IPGRP_PRINTF("%s-%d:name=%s\n",__FUNCTION__,__LINE__,ipEn->entryVal.name);
    }
#endif

    else if(buf[0]=='P')
    {
	if(strstr(buf,"-"))
	{
	ipEn->entryType=IPENT_RANGE;
	start1=strstr(buf,"(");
	end1=strstr(buf,"-");
	start=end1+1;
	*end1='\0';
	inet_aton(start1+1, &fromip);
	ipEn->entryVal.range.ipFrom=fromip.s_addr;
	inet_aton(start, &endip);
	ipEn->entryVal.range.ipTo=endip.s_addr;
	IPGRP_PRINTF("%s-%d:ipfrom=%s,ipto=%s\n",__FUNCTION__,__LINE__,start1+1,start);
	IPGRP_PRINTF("%s-%d:ipfrom=%x,ipto=%x\n",__FUNCTION__,__LINE__,ipEn->entryVal.range.ipFrom,ipEn->entryVal.range.ipTo);
	//snprintf((char *)(prof) + offset, size, "WEB_AUTH:%s", buf+1);
	}
#if (PPPOE_SERVER == FYES)
	else
	{
	ipEn->entryType=IPENT_POE_ACC;
	start=strstr(buf,"(");
	strcpy(ipEn->entryVal.name,start+1);
	IPGRP_PRINTF("%s-%d:name=%s\n",__FUNCTION__,__LINE__,ipEn->entryVal.name);
	}
#endif
    }
    else
    {
	//snprintf((char *)(prof) + offset, size, "USER_GRP:%s", buf+1);
    }
#endif
    }
}
#endif
#if(PORT_GRP == FYES)
static void profGetPortEntryToStr(Pointer prof, Uint32 offset, Uint32 size, char *buf)
{
    char tmp[UTT_PORTGRP_NAME_LEN]={0};
    PortGrpProfile *portGrp = (PortGrpProfile *)prof;
    portGrpEntry* portEntry = (portGrpEntry*)((Byte *)prof+offset);

    PORTGRP_PRINTF("\n%s-%d:prof.head.name=%s,grpType=%d,offset =%u, size=%d,",__FUNCTION__,__LINE__,((PortGrpProfile *)prof)->head.name,((PortGrpProfile *)prof)->grpType,offset,size);

    if(portEntry->entryType==PORTENT_GRP) {
        sprintf(buf,"G(%s)",portEntry->entryVal.grpName);
        PORTGRP_PRINTF("%s-%d: gbuf :%s\n",__FUNCTION__,__LINE__,buf);
    } else if(portEntry->entryType==PORTENT_SYS) {
        sprintf(buf,"S(%s)",portEntry->entryVal.grpName);
        PORTGRP_PRINTF("%s-%d: sbuf :%s\n",__FUNCTION__,__LINE__,buf);
    } else if(portEntry->entryType==PORTENT_OTHER){ 
        sprintf(buf,"O(%d)",portEntry->entryVal.protocol);
        PORTGRP_PRINTF("%s-%d: obuf :%s\n",__FUNCTION__,__LINE__,buf);
    } else if(portEntry->entryType==PORTENT_TCP){
        sprintf(buf,"T(%d-%d,%d-%d)",portEntry->entryVal.portRange.ssport,portEntry->entryVal.portRange.seport,portEntry->entryVal.portRange.dsport,portEntry->entryVal.portRange.deport);
    } else if(portEntry->entryType == PORTENT_UDP){ 
        sprintf(buf,"U(%d-%d,%d-%d)",portEntry->entryVal.portRange.ssport,portEntry->entryVal.portRange.seport,portEntry->entryVal.portRange.dsport,portEntry->entryVal.portRange.deport);
        PORTGRP_PRINTF("%s-%d: ubuf :%s\n",__FUNCTION__,__LINE__,buf);
    } else if(portEntry->entryType==PORTENT_NORMAL){
        sprintf(buf,"C(%s)",portEntry->entryVal.content);
        PORTGRP_PRINTF("%s-%d: nbuf :%s\n",__FUNCTION__,__LINE__,buf);
    } else {
        PORTGRP_PRINTF("%s-%d: null\n",__FUNCTION__,__LINE__);
    }
}

static void profSetPortEntryFromStr(Pointer prof, Uint32 offset, Uint32 size, const char *buf)
{
    PORTGRP_PRINTF("\n%s-%d:prof.head.name=%s,grpType=%d,offset =%u, size=%d,",__FUNCTION__,__LINE__,((PortGrpProfile *)prof)->head.name,((PortGrpProfile *)prof)->grpType,offset,size);
    portGrpEntry* portEn=(portGrpEntry *)((Byte *)prof+offset);
    char* start=NULL,*start1=NULL;
    char* end=NULL,*end1=NULL;
    char  tempName[20] = {0};
    char  tag[20] = {0};
    int   ss,se,ds,de;
    int   temp = 0;
    int   i = 0;
    int   sta_num = 0;
    int   protocol = 0;

    end=strstr(buf,")");
    if(end){
        *end='\0';

    if((buf[0] == 'O') || (buf[0] == 'o'))
    {
        portEn->entryType = PORTENT_OTHER;
        start=strstr(buf,"(");
        portEn->entryVal.protocol = atoi(start + 1);
        PORTGRP_PRINTF("%s-%d:protocol : %d\n",__FUNCTION__,__LINE__,portEn->entryVal.protocol);

    } 
    else if((buf[0]=='G') || (buf[0] == 'g'))
    {
        portEn->entryType=PORTENT_GRP;
        start = strstr(buf,"(");
        strcpy(portEn->entryVal.grpName,start+1);
        PORTGRP_PRINTF("%s-%d:name=%s\n",__FUNCTION__,__LINE__,portEn->entryVal.grpName);
    } 
    else if((buf[0]=='C') || (buf[0] == 'c'))
    {
        portEn->entryType=PORTENT_NORMAL;
        start = strstr(buf,"(");
        strcpy(portEn->entryVal.content,start+1);
        PORTGRP_PRINTF("%s-%d:name=%s\n",__FUNCTION__,__LINE__,portEn->entryVal.content);
    } 
    else if((buf[0] == 'T') || (buf[0] == 't') || (buf[0] == 'U') ||(buf[0] == 'u')) {

        temp = sscanf(buf,"%[UuTt](%d-%d,%d-%d)",tag,&ss,&se,&ds,&de);

#if 0 
        if((temp != 5) || (ss > se) || (ds > de) || (ss > 65535) || (se > 65535) || (ds > 65535) || (de > 65535)\
                || (ss < 0) || (se < 0) || (ds < 0) || (de < 0))
            return ;
#endif

        if((buf[0] == 'T') || (buf[0] == 't')){
            portEn->entryType = PORTENT_TCP;
        }else if((buf[0] == 'U') || (buf[0] == 'u')){
            portEn->entryType = PORTENT_UDP;
        }

        portEn->entryVal.portRange.ssport = ss;
        portEn->entryVal.portRange.seport = se;
        portEn->entryVal.portRange.dsport = ds;
        portEn->entryVal.portRange.deport = de;
        PORTGRP_PRINTF("%s____%d ; srcfrom : %d ; srcto : %d ; destfrom : %d ; destto : %d \n",__func__,__LINE__,
                portEn->entryVal.portRange.ssport,
                portEn->entryVal.portRange.seport,
                portEn->entryVal.portRange.dsport,
                portEn->entryVal.portRange.deport);

    } else if((buf[0]=='S')||(buf[0] == 's')) {

        start = strstr(buf,"(");
        strcpy(tempName, start + 1);

        for(i=0;i<STATIC_SER_NUM;i++)
        {
            if(strcmp(tempName,tcpserver[i])==0)
            {
                if(sta_num<STATIC_TCPSERV_NUM) {
                    if(sta_num==0)	{
                        ds = 20;
                    }else{
                        ds = port_start[sta_num];
                    }
                    de = port_end[sta_num];
                }else if(sta_num<(STATIC_TCPSERV_NUM+STATIC_UDPSERV_NUM)){
                    ds = port_start[sta_num];
                    de = port_end[sta_num];
                }else{
                    protocol=port_start[sta_num];
                }
                portEn->entryType=PORTENT_SYS;
                portEn->entryVal.sysGrp.portRange.ssport = 0;
                portEn->entryVal.sysGrp.portRange.seport = 65535;
                portEn->entryVal.sysGrp.portRange.dsport = ds;
                portEn->entryVal.sysGrp.portRange.deport = de;
                portEn->entryVal.sysGrp.protocol = protocol;
                strcpy(portEn->entryVal.sysGrp.sysGrpName,tempName);
                PORTGRP_PRINTF("%s-%d:name=%s\n",__FUNCTION__,__LINE__,portEn->entryVal.sysGrp.sysGrpName);
            }
        }
    }
    else
    {
	//snprintf((char *)(prof) + offset, size, "USER_GRP:%s", buf+1);
    }
    }
}
#endif
static void profGetIntToStr(Pointer prof, Uint32 offset, Uint32 size, char* buf)
{
    Uint32 value;
    switch(size) {
        case 1: 
    	value = *(Uint8*)((Byte*)prof + offset);
            break;
        case 2:
    	value = *(Uint16*)((Byte*)prof + offset);
            break;
        case 4:
        default:
    	value = *(Uint32*)((Byte*)prof + offset);
    }
    snprintf(buf, MAX_VALUE_LEN, "%d", value);
    return;
}

static void profSetIntFromStr(Pointer prof, Uint32 offset, Uint32 size, const char* buf)
{
    switch(size) {
        case 1: 
			*(Uint8*)((Byte*)prof + offset) = (Uint8)strtol(buf, NULL, 10);
            break;
        case 2:
			*(Uint16*)((Byte*)prof + offset) = (Uint16)strtol(buf, NULL, 10);
            break;
        case 4:
        default:
			*(Uint32*)((Byte*)prof + offset) = (Uint32)strtol(buf, NULL, 10);
    }
    return;
}

static void profGetHexToStr(Pointer prof, Uint32 offset, Uint32 size, char* buf)
{
    int i, min;
    Uint8* tmp = NULL;
    tmp = (Uint8 *)prof + offset;

    min = (size < (MAX_VALUE_LEN - 1)/2) ? size : ((MAX_VALUE_LEN - 1)/2);

    for (i = 0; i < min; i++) {
	sprintf(buf, "%02x", *tmp);
	buf += 2;
	tmp += 1;
    }

    return;
}

static void profSetHexFromStr(Pointer prof, Uint32 offset, Uint32 size, const char* buf)
{
    int i, min;
    char tmp[3] = {0};
    Uint8* tmp1 = NULL;

    min = (size < MAX_VALUE_LEN/2) ? size : (MAX_VALUE_LEN/2);
    tmp1 = (Uint8 *)prof + offset;

    for (i = 0; i < min; i++) {
	tmp[0] = buf[i*2];
	tmp[1] = buf[i*2 + 1];
	tmp1[i] = (Uint8)strtol(tmp, NULL, 16);
    }
    return;
}
static void profGetUintToStr(Pointer prof, Uint32 offset, Uint32 size, char* buf)
{
    Uint32 value;
    switch(size) {
        case 1: 
    	value = *(Uint8*)((Byte*)prof + offset);
            break;
        case 2:
    	value = *(Uint16*)((Byte*)prof + offset);
            break;
        case 4:
        default:
    	value = *(Uint32*)((Byte*)prof + offset);
    }
    snprintf(buf, MAX_VALUE_LEN, "%u", value);
    return;
}

static void profSetUintFromStr(Pointer prof, Uint32 offset, Uint32 size, const char* buf)
{
    switch(size) {
	case 1: 
	    *(Uint8*)((Byte*)prof + offset) = (Uint8)strtoul(buf, NULL, 10);
	    break;
	case 2:
	    *(Uint16*)((Byte*)prof + offset) = (Uint16)strtoul(buf, NULL, 10);
	    break;
	case 4:
	default:
	    *(Uint32*)((Byte*)prof + offset) = (Uint32)strtoul(buf, NULL, 10);
    }
    return;
}


static void profGetStrToStr(Pointer prof, Uint32 offset, Uint32 size, char* buf)
{
    strncpy(buf, (const char*)prof + offset, MAX_VALUE_LEN - 2);
    return;
}

static void profSetStrFromStr(Pointer prof, Uint32 offset, Uint32 size, const char* buf)
{
    strncpy((char*)prof + offset, buf, size-1);
    return;
}

static void profGetIPToStr(Pointer prof, Uint32 offset, Uint32 size, char* buf)
{
    struct in_addr inaddr;
    Uint32 value = 0u;
    value = *(Uint32*)((Byte*)prof + offset);
    inaddr.s_addr = (value);
    snprintf(buf, MAX_VALUE_LEN - 2, "%s", inet_ntoa(inaddr));
    return;
}

static void profSetIPFromStr(Pointer prof, Uint32 offset, Uint32 size, const char* buf)
{
    struct in_addr ip;
    int rt = 0;
    rt = inet_aton(buf, &ip);
    if(rt != 0)
    	*(Uint32*)((Byte*)prof + offset) = (Uint32)(ip.s_addr);
    else 
	*(Uint32*)((Byte*)prof + offset) = 0;
    return;
}

extern void profGetEnumToStr2(Pointer prof, Uint32 offset, Uint32 size, char* buf,
        LcdParamEnumValue *enumPara)
{
    Uint32 value;
    value = *(Uint32*)((Byte*)prof + offset);
    
    while(enumPara && enumPara->enumName) {
    	if(enumPara->enumValue == value) {
    	    strncpy(buf, enumPara->enumName, MAX_VALUE_LEN - 2);
    	    break;
    	}
    	enumPara++;
    }
    return;
}

static void profGetEnumToStr(Pointer prof, Uint32 offset, Uint32 size, char* buf,
        struct st_xmlConfig *conf)
{
    profGetEnumToStr2(prof, offset, size, buf,(LcdParamEnumValue *)conf->enumP);
}

static void profSetEnumFromStr(Pointer prof, Uint32 offset, Uint32 size, const char* buf,
        struct st_xmlConfig *conf)
{
    LcdParamEnumValue *enumPara;
    Uint32 value;
    value = *(Uint32*)((Byte*)prof + offset);
    enumPara = (LcdParamEnumValue *)conf->enumP;
    
    while(enumPara && enumPara->enumName) {
		if(strcmp(enumPara->enumName, buf) == 0) {
			*(Uint32*)((Byte*)prof + offset) = enumPara->enumValue;
    	    break;
		}
    	enumPara++;
    }
    return;
}

#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
#if(TIME_GRP == FYES)
static void profGetDateToStr(Pointer prof, Uint32 offset, Uint32 size, char* buf)
{
    char dtime[11];
    Uint32 value=0u;
    time_t t;
    struct tm *tm_ptr=NULL;
    value=*(Uint32*)((Byte*)prof + offset);
    if(value==1){
	snprintf(buf,MAX_VALUE_LEN-2,"%s","Forever");
    }
    else if(value==2){
	snprintf(buf,MAX_VALUE_LEN-2,"%s","0");
    }
    else{
	t=value*(24*60*60);
	tm_ptr=localtime(&t);
	sprintf(dtime,"%d",(tm_ptr->tm_year+1900));
	dtime[4]='-';
	sprintf(dtime+5,"%02d",(tm_ptr->tm_mon+1));
	dtime[7]='-';
	sprintf(dtime+8,"%02d",tm_ptr->tm_mday);
	dtime[10]='\0';    
	snprintf(buf,MAX_VALUE_LEN-2,"%s",dtime);
    }
}

static void profGetTimeToStr(Pointer prof, Uint32 offset, Uint32 size, char* buf)
{
    char ctime[6];
    int i=0;
    Uint32 value=0u;
    value = *(Uint32*)((Byte*)prof + offset);
    if(value==1){
	snprintf(buf,MAX_VALUE_LEN-2,"%s","All");
    }
    else  if(value==86401){
	snprintf(buf,MAX_VALUE_LEN-2,"%s","0");
    }
    else if(value==59){
	snprintf(buf,MAX_VALUE_LEN-2,"%s","00:00");
    }
    else{
	ctime[i++]=(value/3600)/10+'0';
	ctime[i++]=(value/3600)%10+'0';
	ctime[i++]=':';
	ctime[i++]=((value%3600)/60)/10+'0';
	ctime[i++]=((value%3600)/60)%10+'0';
	ctime[i]='\0';
	snprintf(buf,MAX_VALUE_LEN-2,"%s",ctime);
    }
}
static void profSetDateFromStr(Pointer prof, Uint32 offset, Uint32 size, const char* buf)
{
    Uint32 date=0u;
    time_t t;
    int time[3]={0};
    struct tm *tm_ptr=NULL;
    struct tm stm;
    int i=0,j=0,num=0;
    if(strcmp(buf,"Forever")==0){
	*(Uint32*)((Byte*)prof + offset) =1;
    }
    else if(strcmp(buf,"0")==0){
	*(Uint32*)((Byte*)prof + offset) =2;
    }
    else{
	i=0;
	j=0;
	num=0;
	while(buf[i]!='\0'){
	    while(buf[i]!='-'&&buf[i]!='\0'){
		num=num*10+(buf[i]-'0');
		i++;	
	    }
	    time[j++]=num;
	    num=0;
	    if(buf[i] =='-')i++;
	}
	stm.tm_year=time[0]-1900;
	stm.tm_mon=time[1]-1;
	stm.tm_mday=time[2];
	stm.tm_hour=0;
	stm.tm_min=0;
	stm.tm_sec=0;
	t=mktime(&stm);
	if(t!=-1){
	    date=t/(24*60*60); 
	}
	*(Uint32*)((Byte*)prof + offset) = date;
    } 
}

static void profSetTimeFromStr(Pointer prof, Uint32 offset, Uint32 size, const char* buf)
{
    Uint32 time=0u;
    if(strcmp(buf,"All")==0){
	*(Uint32*)((Byte*)prof + offset) =1;
    }
    else if(strcmp(buf,"0")==0){
	*(Uint32*)((Byte*)prof + offset) =86401;
    }
    else if(strcmp(buf,"00:00")==0){
	*(Uint32*)((Byte*)prof + offset) =59;
    }
    else{
	time =(((buf[0]-'0')*10+(buf[1]-'0'))*60+((buf[3]-'0')*10+(buf[4]-'0')))*60;
	*(Uint32*)((Byte*)prof + offset) = time;
    }
}
#endif
#endif

void parseProfileValue(struct st_xmlConfig *conf, int instIndex, char* buf, Pointer prof)
{

    if(prof != NULL) {
    	switch(conf->itype) {
    	    case TINT:
    		profGetIntToStr(prof, conf->offset, conf->size, buf);
    		break;
    	    case TSTR:
    	    case TSTRNUM:
    	    	profGetStrToStr(prof, conf->offset, conf->size, buf);
    	    	break;
    	    case TMACADDR:
    	    	profGetMacToStr(prof, conf->offset, conf->size, buf);
    	    	break;
    	    case TIPADDR:
    	    	profGetIPToStr(prof, conf->offset, conf->size, buf);
                break;
    	    case TENUM:
    	    	profGetEnumToStr(prof, conf->offset, conf->size, buf, conf);
    	    	break;
    	    case TMEMBERS:
    	    	profGetMembersToStr(prof, conf->offset, conf->size, buf);
		break;
#if(IP_GRP == FYES)
	    case TIPENTRY:
		profGetIpEntryToStr(prof, conf->offset, conf->size, buf);
		break;
#endif
#if(PORT_GRP == FYES)
        case TPORTENTRY:
		profGetPortEntryToStr(prof, conf->offset, conf->size, buf);
        break;
#endif
	    case TUINT:
		profGetUintToStr(prof, conf->offset, conf->size, buf);
		break;
	    case THEX:
		profGetHexToStr(prof, conf->offset, conf->size, buf);
		break;
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
#if(TIME_GRP == FYES)
	    case TTIMESEC:
		profGetTimeToStr(prof, conf->offset, conf->size, buf);
		break;
	    case TTIMEDATE:
		profGetDateToStr(prof, conf->offset, conf->size, buf);
		 break;
#endif
#endif
    	    default:
    	    	break;
    	}
    }
    return;
}

/*
 * 功能：根据prof对应的成员字符串值
 * 输入参数：
 *  offset	-   偏移量
 *  size	-   结构体成员大小
 *  itype	-   结构体成员类型
 *  prof	-   实例profile
 * 输出参数：
 *  buf		-   要获得的成员值，以字符串形式返回
 */
void parseProfileValue2(Uint32 offset, Uint32 size, IType itype, char* buf, Pointer prof)
{

    if(prof != NULL) {
    	switch(itype) {
    	    case TINT:
    		profGetIntToStr(prof, offset, size, buf);
    		break;
    	    case TSTR:
    	    case TSTRNUM:
    	    	profGetStrToStr(prof, offset, size, buf);
    	    	break;
    	    case TMACADDR:
    	    	profGetMacToStr(prof, offset, size, buf);
    	    	break;
    	    case TIPADDR:
    	    	profGetIPToStr(prof, offset, size, buf);
                break;
    	    case TMEMBERS:
    	    	profGetMembersToStr(prof, offset, size, buf);
		break;
#if(IP_GRP == FYES)
	    case TIPENTRY:
		profGetIpEntryToStr(prof, offset, size, buf);
		break;
#endif
#if(PORT_GRP == FYES)
        case TPORTENTRY:
		profGetPortEntryToStr(prof, offset, size, buf);
		break;
#endif
	    case TUINT:
		profGetUintToStr(prof, offset, size, buf);
		break;
	    case THEX:
		profGetHexToStr(prof, offset, size, buf);
		break;
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
#if(TIME_GRP == FYES)
	    case TTIMESEC:
		profGetTimeToStr(prof, offset, size, buf);
		break;
	    case TTIMEDATE:
		profGetDateToStr(prof, offset, size, buf);
		 break;
#endif
#endif
    	    default:
    	    	break;
    	}
    }
    return;
}

/*
 * 输入参数：
 *  prof    -	要比较的profile
 *  size    -	prof结构体大小
 *
 * 返回值：0 - 是默认配置
 *	 非0 - 不是默认配置
 */
extern int ProfIsDefault(MibProfileType mibType, int instIndex, Pointer prof, int size) {
    Pointer profdef;
    SlotNumber slot; 

    slot.slotNumber = SLOT_NUMBER_ANY; 

    if (!prof) {
	return 1;
    }

    BuildDefaultFactoryValue((MibProfileType)mibType, instIndex, slot, &profdef);
    if (!profdef) {
	return 1;
    }

    return memcmp(prof, profdef, size);
}

static int ProfValueIsDefault(struct st_xmlConfig *conf, 
        MibProfileType mibtype, int instIndex, Pointer prof)
{
    Pointer profdef;
    SlotNumber slot; 

    slot.slotNumber = SLOT_NUMBER_ANY; 
    BuildDefaultFactoryValue((MibProfileType)mibtype, instIndex, slot, &profdef);

    return memcmp((Byte*)prof + conf->offset, (Byte*)profdef + conf->offset, conf->size);
}

void parseProfileToXml(struct st_xmlConfig *conf, mxml_node_t *parent, MibProfileType mibtype, int instIndex, Pointer prof)
{
    SlotNumber slot; 
    slot.slotNumber = SLOT_NUMBER_ANY; 
    Pointer profdef;
    Uint size;

    char buf[MAX_VALUE_LEN] = {0};
    int i = 0, j = 0;
    int instMin, instMax;
	mxml_node_t *newNode, *caseNode, *tmpNode;
    for(i = 0; conf->name != NULL; ) {
        if(conf->child != NULL) {
            switch(conf->privilege) {
                case INST:
					newNode = mxmlNewElement(parent, conf->name);
					ProfInstLowHigh(conf->mibtype, &instMax, &instMin);
	            	for(j = instMin; j < instMax; j++) {

                                prof = _getProfilePointer(slot, conf->mibtype, j, &size, NULL, NULL);
                                BuildDefaultFactoryValue(conf->mibtype, j, slot, &profdef);
                                if(profdef != NULL && memcmp(prof, profdef, size) != 0) 
                                {
	            		    if((prof != NULL) && (ProfInstIsFree(prof) == 1)) {
	            		        continue;
	            		    }
				    		caseNode = mxmlNewElement(newNode, "case");
				    		mxmlElementSetAttr(caseNode, "name", ((mibHead*)prof)->name);

#if 0
	            		    if(((mibHead*)prof)->active == 1) {
	            		        activeP = "Yes";
	            		    } else {
	            		        activeP = "No";
	            		    }
				    		tmpNode = mxmlNewElement(caseNode, "active");
				    		mxmlNewText(tmpNode, 0, activeP);
#endif
	            		    parseProfileToXml(conf->child, caseNode, conf->mibtype, j, prof);
                                }
	            	}
	            	break;
                default:
					newNode = mxmlNewElement(parent, conf->name);
					parseProfileToXml(conf->child, newNode, mibtype, instIndex, prof);
            }
        } else {
            if(conf->privilege == VALUE) {
                if(ProfValueIsDefault(conf, mibtype, instIndex, prof) != 0 || strcmp(conf->name,"active")==0) {
                                        if(conf->size <= MAX_VALUE_LEN) {
                                            memset(buf, 0, MAX_VALUE_LEN);
					    parseProfileValue(conf, mibtype, buf, prof);
					    tmpNode = mxmlNewElement(parent, conf->name);
					    mxmlNewText(tmpNode, 0, buf);
                                        }
                }
            }
        }
		conf++;
    }
}

/**
 * 将当前内存中的配置转换为字串
 * return:0 成功 -1失败
 * 输出参数：
 * len	-   
 *
 */
int parseProfileToStr(char *buf, int *len)
{
    mxml_node_t *doc, *root;
    int ret = -1;
    int char_c = 0;

    doc = mxmlNewXML("1.0");
    if(doc != NULL) {
	root = mxmlNewElement(doc, "config");
	if(root != NULL) {
	    parseProfileToXml(rootConfig, root, 0, 0, NULL);
	    char_c = mxmlSaveString(doc, buf, CONFIG_BUFF_SIZE, MXML_NO_CALLBACK);
	    if (len != NULL) {
		*len = char_c;
	    }
	    ret = 0;
	}
	mxmlDelete(doc);
    }
    return ret;
}

#if 0
const char * fun(mxml_node_t *node, int where)
{
	const char *name;
	name = node->value.element.name;
	if(strcmp("case", name) == 0)
		return "\n";
	else 
		return "";
}
#endif

int converConfigToXml(void)
{ 
    FILE *fp;
	mxml_node_t *doc, *root;

	doc = mxmlNewXML("1.0");
	root = mxmlNewElement(doc, "config");
    parseProfileToXml(rootConfig, root, 0, 0, NULL);

	fp = fopen("filename.xml", "w+");
	mxmlSaveFile(doc, fp, MXML_NO_CALLBACK);
//	mxmlSaveFile(doc, fp, fun);
	fclose(fp);
    mxmlDelete(doc);
    return 0;
}

#if 0
static void parseXmlToNvramConfig(struct st_xmlConfig *conf, mxml_node_t *top)
{
    if(node->type == MXML_ELEMENT) {
        if(strcmp(node->value.element.name, "case") == 0) {
            /*set prof name*/
        } else strcmp(node->value.element.name, "active") == 0 {
            /*set prof active*/
        } else {
        }
    } else if(node->type == MXML_TEXT) {
    }
}
#endif

struct st_xmlConfig* findXmlConfigByName(struct st_xmlConfig *conf, const char *name)
{
    for(; conf->name != NULL; conf++) {
		if(strcmp(conf->name, name) == 0) {
			return conf;
		}
	}
	return NULL;
}

void parseProfileParaFromXml(struct st_xmlConfig *conf, mxml_node_t *node, Pointer prof)
{
#if 1 
	const char *buf;
	if (node == NULL) { /* 对应空值的情况 */
	    buf = "";
	} else {
	    buf = node->child->value.text.string;
	}
#else
	const char buf[1024] = {0};
	mxml_node_t* tmp_node = node->child;
	for(; tmp_node != NULL; tmp_node = tmp_node->next)
	{
	    strcat(buf, tmp_node->value.text.string);
	}
#endif
	switch(conf->itype) {
   	    case TINT:
    		profSetIntFromStr(prof, conf->offset, conf->size, buf);
    		break;
        case TSTR:
        case TSTRNUM:
        	profSetStrFromStr(prof, conf->offset, conf->size, buf);
        	break;
        case TMACADDR:
        	profSetMacFromStr(prof, conf->offset, conf->size, buf);
        	break;
	case TIPADDR:
    		profSetIPFromStr(prof, conf->offset, conf->size, buf);
        	break;
    	case TENUM:
    		profSetEnumFromStr(prof, conf->offset, conf->size, buf, conf);
    		break;
	case TMEMBERS:
    		profSetMembersFromStr(prof, conf->offset, conf->size, buf);
		break;
#if(IP_GRP == FYES)
	case TIPENTRY:
		profSetIpEntryFromStr(prof, conf->offset, conf->size, buf);
		break;
#endif
#if(PORT_GRP == FYES)
        case TPORTENTRY:
		profSetPortEntryFromStr(prof, conf->offset, conf->size, buf);
        break;
#endif
	case TUINT:
		profSetUintFromStr(prof, conf->offset, conf->size, buf);
		break;
	case THEX:
		profSetHexFromStr(prof, conf->offset, conf->size, buf);
		break;
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
#if(TIME_GRP == FYES)
	    case TTIMESEC:
		profSetTimeFromStr(prof, conf->offset, conf->size, buf);
		break;
	    case TTIMEDATE:
		profSetDateFromStr(prof, conf->offset, conf->size, buf);
		 break;
#endif
#endif
    	default:
    		break;
	}
}

void parseProfileOneInstFromXml(struct st_xmlConfig *conf, mxml_node_t *node, Pointer prof)
{
	struct st_xmlConfig *tmpconf = NULL;
    if(conf == NULL || node == NULL) {
		return;
	}
    for(; node != NULL; node = node->next) {
#if 0
		if(node->child == NULL) {
			continue;
		} else if(node->type == MXML_ELEMENT) {
			tmpconf = findXmlConfigByName(conf, node->value.element.name);
			if(node->child->type == MXML_TEXT) {
				if(tmpconf != NULL){
					if(tmpconf->privilege == VALUE) {
						parseProfileParaFromXml(tmpconf, node, prof);
					} else {
						parseProfileOneInstFromXml(tmpconf->child, node->child, prof);
					}
				} else {
					continue;
				}
			} else {
				parseProfileOneInstFromXml(tmpconf->child, node->child, prof);
			}
		} else {
			continue;
		}
#endif
		/*
		 * 如果node的子节点是NULL,并且对应的profile的类型是VALUE，
		 * 那么将profile中对应的值修改为
		 */
		if(node->type == MXML_ELEMENT) {
			tmpconf = findXmlConfigByName(conf, node->value.element.name);
			if (node->child == NULL) {
			    if (tmpconf != NULL && tmpconf->privilege == VALUE) {
				parseProfileParaFromXml(tmpconf, NULL, prof);
			    }
			} else if(node->child->type == MXML_TEXT) {
				if(tmpconf != NULL){
					if(tmpconf->privilege == VALUE) {
						parseProfileParaFromXml(tmpconf, node, prof);
					} else {
						parseProfileOneInstFromXml(tmpconf->child, node->child, prof);
					}
				} else {
					continue;
				}
			} else {
			    if (tmpconf != NULL) {
				parseProfileOneInstFromXml(tmpconf->child, node->child, prof);
			    }
			}
		} else {
			continue;
		}
	}
	return;
}

void parseProfileInstFromXml(struct st_xmlConfig *conf, mxml_node_t *node, MibProfileType mibtype)
{
	int instIndex = 0;
	int instMax = 0, instMin = 0;
	const char *name = NULL;
	Pointer prof = NULL;

    if(conf == NULL || node == NULL) {
        return;
    }
	ProfInstLowHigh(mibtype, &instMax, &instMin);
    for(; node != NULL; node = node->next) {
        if(node->type == MXML_ELEMENT && strcmp(node->value.element.name, "case") == 0) {
			name = mxmlElementGetAttr(node, "name");
			if(name != NULL) {
				/* here should user new fun*/
				instIndex = ProfNewInst(mibtype, name, 0);
				if(instIndex == PROFFAIL) {
				    prof = ProfGetInstPointByName(mibtype, name);
				} else {
				    prof = ProfGetInstPointByIndex(mibtype, instIndex);
                                }
				if(prof != NULL) {
					ProfSetNameByPoint(prof, (Byte*)name);
					parseProfileOneInstFromXml(conf, node->child, prof);
				} else {
                                    break;
                                }
			} else {
				continue;
			}
		} else {
			continue;
		}
	}
	return;
}

void parseProfileFromXml(struct st_xmlConfig *conf, mxml_node_t *node)
{
    struct st_xmlConfig *tmpconf = NULL;
    if(conf == NULL || node == NULL) {
        return;
    }
    for(; node != NULL; node = node->next) {
        if(node->type == MXML_ELEMENT) {
            tmpconf = findXmlConfigByName(conf, node->value.element.name);
            if(tmpconf == NULL) {
                continue;
            } else {
                if(node->child == NULL) {
                    continue;
                }
                switch(tmpconf->privilege) {
                    case VALUE:
			break;
#if 0
                    case VALUE:
                        if(node->child->type != MXML_TEXT) {
                            continue;
                        } else {
                            parseProfileValue(); 
                        }
                        break;
#endif
                    case INST:
			parseProfileInstFromXml(tmpconf->child, node->child, tmpconf->mibtype);
			break;
                    default:
                        parseProfileFromXml(tmpconf->child, node->child);
                }
            }
        } else if(node->type == MXML_TEXT) {
			continue;
        }
    }
}

int converConfigFromXml(void)
{
    FILE *fp;
    mxml_node_t *doc, *config;
    fp = fopen("filename.xml", "r");
    doc = mxmlLoadFile(NULL, fp,MXML_TEXT_CALLBACK);
    fclose(fp);
    config = mxmlFindElement(doc, doc, "config", NULL, NULL, MXML_DESCEND);
    parseProfileFromXml(rootConfig, config->child);
    mxmlDelete(doc);
    return 0;
}

/*
 * 从指定的xml文件中解析配置
 * 输入参数：
 *  fileName	-   xml格式的文件名
 * 输出参数：
 *  无
 * 返回值：
 *  1	-   转换成功
 *  0	-   失败
 *
 */
extern int converConfigFromXmlFile(const char *fileName) {
    FILE *fp;
    mxml_node_t *doc, *config;
    fp = fopen(fileName, "r");
    if(fp == NULL) {
        return 0;
    }
    doc = mxmlLoadFile(NULL, fp,MXML_TEXT_CALLBACK);
    if(doc != NULL){
	config = mxmlFindElement(doc, doc, "config", NULL, NULL, MXML_DESCEND);
	if (config != NULL) {
	    parseProfileFromXml(rootConfig, config->child);
	    mxmlDelete(doc);
	    return 1;
	}
    }
    fclose(fp);
    return 0;
}

/**
 * 从字符串中解析配置到共享内存
 * strBuf:配置字串    reset:解析之前是否恢复出厂配置
 * return: 0成功 -1失败（配置字串格式错误等）
 * 2011-10-14
 * bhou
 */
int parseProfileFromStr(char* buf, Bool reset)
{
    int ret = -1;

    mxml_node_t *doc, *config;

    doc = mxmlLoadString(NULL, buf, MXML_TEXT_CALLBACK);
    if(doc != NULL) {/*合法xml格式*/
	config = mxmlFindElement(doc, doc, "config", NULL, NULL, MXML_DESCEND);
	if(config != NULL)/*含有config*/
	{
	    if(reset)
	    {
		makeFactoryProfiles();/*恢复出厂配置*/
	    }
	    parseProfileFromXml(rootConfig, config->child);/*解析配置：与当前配置合并*/
	    ret = 0;
	}
	mxmlDelete(doc);
    }

    return ret;
}
#if 0
int main(int argc, char** argv)
{
    ProfInit();
#if 1
	if(argc == 1)
    converConfigToXml();
	else 
	converConfigFromXml();
#endif
    exit(0);
}
#endif
#if (FEATURE_AC==FYES)
/*
 * replace one element of a xml file
 * xlm_file        the xml file name
 * xml_node_name   the element name
 * xml_string      the element context in xml format 
 * Kehuatao 
 * 2012-08-07
 */
int setProfileInstToXml(char *xml_file, char* xml_node_name, char* xml_string)
{
    int i=0;
    FILE *fp;
    mxml_node_t *doc, *new_doc, *old_node, *new_node, *config;
    struct st_xmlConfig *conf=NULL;

    fp = fopen(xml_file, "r");
    if(fp!=NULL)
    {
	doc = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
	fclose(fp);
	if(doc != NULL){
	    config = mxmlFindElement(doc, doc, "config", NULL, NULL, MXML_DESCEND);
	    if (config != NULL) {
		/*delete the old node*/
		old_node = mxmlFindElement(config, config, xml_node_name, NULL, NULL, MXML_DESCEND);
		mxmlDelete(old_node);

		/*add new node*/
		new_doc = mxmlLoadString(NULL, xml_string, MXML_TEXT_CALLBACK);
		new_node = mxmlFindElement(new_doc, new_doc, xml_node_name, NULL, NULL, MXML_DESCEND);
		if (new_node != NULL) {
		   mxmlRemove(new_node);
		   mxmlDelete(new_doc);
		   new_node->parent = config;
		   new_node->prev = config->last_child;
		   config->last_child->next = new_node;
		   config->last_child = new_node;
		   new_node->next = NULL;
		   i = 1;
		}
	    }
	}

	if(i==1)
	{
	    fp = fopen(xml_file, "w");
	    mxmlSaveFile(doc, fp, MXML_TEXT_CALLBACK);
	    mxmlDelete(doc);
	    fclose(fp);
	}
    }

    return i;
}
#endif
#if (FEATURE_AP==FYES)
/*
 * parseProfileValueToXml
 *
 * KeHuatao
 * 2012-08-07
 */
int parseProfileValueToXml(struct st_xmlConfig *conf, mxml_node_t *parent, MibProfileType mibtype, Pointer prof)
{
    char buf[MAX_VALUE_LEN+1] = {0};
    mxml_node_t *tmpNode;
    int i=-1;

    if(conf!=NULL) {
	while(conf->name != NULL){
	    if(conf->privilege == VALUE) {
		if(conf->size <= MAX_VALUE_LEN) {
		    memset(buf, 0, MAX_VALUE_LEN);
		    parseProfileValue(conf, mibtype, buf, prof);
		    tmpNode = mxmlNewElement(parent, conf->name);
		    mxmlNewText(tmpNode, 0, buf);
		    i=0;
		}
	    }
	    conf++;
	}
    }

    return i;
}
/*
 * change one profile to xml string
 *
 * KeHuatao
 * 2012-08-07
 */
int parseProfileOneInstToXml(MibProfileType mibtype, Pointer prof, char *xml_buf, int len)
{
    int i=-1;
    mxml_node_t *root, *newNode, *caseNode;
    struct st_xmlConfig *conf=NULL;

    for(conf=rootConfig;conf->name!=NULL;conf++)
    {
	if(conf->mibtype==mibtype)
	{
	    break;
	}
    }
    if(conf->name!=NULL)
    {
	root = mxmlNewElement(MXML_NO_PARENT, "config");

	newNode = mxmlNewElement(root, conf->name);
	caseNode = mxmlNewElement(newNode, "case");
	mxmlElementSetAttr(caseNode, "name", ((mibHead*)prof)->name);
	i = parseProfileValueToXml(conf->child, caseNode, conf->mibtype, prof);
	if(i==0) {
	    i = mxmlSaveString(root, xml_buf, len, MXML_NO_CALLBACK);
	}
	mxmlDelete(root);
    }

    return i;
}

/*
 * get one profile from xml
 *
 * KeHuatao
 * 2012-08-07
 *
 */
void getProfileFromXml(char *xml_file, MibProfileType mibtype, Pointer prof)
{
    const char *name=NULL;
    FILE *fp;
    struct st_xmlConfig *conf=NULL;
    mxml_node_t *doc, *node;

    for(conf=rootConfig;conf->name!=NULL;conf++)
    {
	if(conf->mibtype==mibtype)
	{
	    break;
	}
    }

    if(conf->name!=NULL)
    {
		fp = fopen(xml_file, "r");
		if(fp!=NULL)
		{
			doc = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
			fclose(fp);
			if(doc != NULL){
				doc = mxmlFindElement(doc, doc, "config", NULL, NULL, MXML_DESCEND);
				if (doc != NULL) {
					node = doc->child;
					/*the profile node*/
					for(; node!= NULL; node = node->next) {
						if((node->type == MXML_ELEMENT) && (strcmp(conf->name, node->value.element.name)==0)) {
							node = node->child;
							if((node!=NULL)&&(node->type == MXML_ELEMENT) && (strcmp(node->value.element.name, "case") == 0)) {
								name = mxmlElementGetAttr(node, "name");
								if(name != NULL) {
									/*parse all profile*/
									parseProfileOneInstFromXml(conf->child, node->child, prof);
								}
							}
							break;
						}
					}
				}
			}
		}
	}
}
#endif

