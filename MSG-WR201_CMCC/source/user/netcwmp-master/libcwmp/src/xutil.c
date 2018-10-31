/************************************************************************
 * Id: xutil.c                                                           *
 *                                                                      *
 * TR069 Project:  A TR069 library in C                                 *
 * Copyright (C) 2013-2014  netcwmp.netcwmp group                                *
 *                                                                      *
 *                                                                      *
 * Email: netcwmp ( & ) gmail dot com                                *
 *                                                                      *  
 ***********************************************************************/

#include "cwmp/xutil.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "conver.h"
#include "../../../lib/mxml-2.6/mxml.h"

extern struct st_xmlConfig  rootConfig[];
extern void parseProfileValue(struct st_xmlConfig *conf, int instIndex, char* buf, Pointer prof);

int x_rand(int *change)
{
    if(change == NULL)
    {
        srand(time(NULL));
    }else{
        srand(*change);
    }
    return rand();
}

int xstrcmp(const char*str1,const char*str2)
{
    char s1 , s2;

    if(str1 == NULL || str2 == NULL)
    {
        return -1;
    }

    s1 = *str1;
    s2 = *str2;
    
    while(s1 == s2)
    {
        if(s1 == 0)
        {
            return 0;
        }

        s1 = *++str1;
        s2 = *++str2;

        if(s1>='A' && s1<='Z')
        {
            s1= s1 + 32;
        }

        if(s2>='A' && s2<='Z')
        {
            s2= s2 + 32;
        }
    }
    return *str1-*str2;
}
int get_method(cwmp_t *cwmp,const char* name,char **value,pool_t* pool)
{
    struct st_xmlConfig *conf = rootConfig;
    struct st_xmlConfig *backconf = NULL;
    MibProfileType mibType = -1;
    Pointer prof = NULL;
    char*  tmp = NULL;
    int    findInstType = 0, max, min;
    char   buf[MAX_VALUE_LEN] = {0};
    char   bak[100] = {0};

    memset(buf,0,sizeof(buf));
    memset(bak,0,sizeof(bak));

    memcpy(bak,name,strlen(name));
    //cwmp_log_debug("%s ---- %d name: %s\n",__func__,__LINE__,bak);

    tmp = strtok(bak,".");

    int i = 0;
    backconf = conf;
    while(tmp)
    {
        conf = backconf;
        for(; conf->name != NULL; conf++) {
            if(TRstrcasecmp(conf->name, tmp) == 0) {
                //cwmp_log_debug("%s ---- %d ;conf->name : %s ; tmp : %s \n",__func__,__LINE__, conf->name, tmp);
                switch(conf->privilege) {
                    case INST:
                        mibType = conf->mibtype;
                        findInstType = 1;
                        break;
                    default:
                        break;
                }
                conf = conf->child;
                break;
            }
        }
        if(findInstType == 1) {
            break;
        }
        tmp = strtok(NULL,".");
    }

    if(findInstType == 1) {

        ProfInstLowHigh(mibType, &max, &min);
        if(max <= 1) 
        {
            prof = ProfGetInstPointByIndex(mibType, 0);
        } else {
            tmp = strtok(NULL,".");
            if(tmp != NULL)
            {
                //prof = ProfGetInstPointByName(mibType, tmp);
                prof = ProfGetInstPointByIndex(mibType, atoi(tmp));
                //cwmp_log_debug("%s ---- %d; tmp : %s\n",__func__,__LINE__,tmp);
            }
        }

        if(prof == NULL || tmp == NULL) 
        {
            return FAULT_CODE_9005;
        }

        tmp = strtok(NULL,".");
        while(tmp)
        {
            for(; conf->name != NULL; conf++) 
            {
                if(TRstrcasecmp(conf->name, tmp) == 0) 
                {
                //cwmp_log_debug("%s ---- %d ;conf->name : %s ; tmp : %s \n",__func__,__LINE__, conf->name, tmp);
                    if(conf->privilege == VALUE) 
                    {
                        parseProfileValue(conf, mibType, buf, prof);
                        //cwmp_log_debug("%s ---- %d; %s : %s\n",__func__,__LINE__,conf->name,buf);
                        *value = PSTRDUP(buf);
                        return FAULT_CODE_OK;
                    } else {
                        if(conf->child != NULL) 
                        {
                            conf = conf->child;
                            break;
                        }
                    }
                }
            }
            //cwmp_log_debug("%s ---- %d; tmp : %s\n",__func__,__LINE__,tmp);
            tmp = strtok(NULL,".");
        }
    }
    return 	FAULT_CODE_9005;  
}
//int set_method(cwmp_t *cwmp,const char* name,const char *value,int length,callback_register_func_t callback_reg,bool update)
int set_method(cwmp_t *cwmp,const char* name,const char *value,int length,callback_register_func_t callback_reg)
{
    struct st_xmlConfig *conf = rootConfig;
    struct st_xmlConfig *backconf = NULL;
    MibProfileType mibType = -1;
    Pointer prof = NULL;
    char*  tmp = NULL;
    int    findInstType = 0, max, min;
    char   buf[MAX_VALUE_LEN] = {0};
    char   bak[100] = {0};
    int find_param_name = 0;
    bool   update = TRUE;
    struProfAlloc *profhead = NULL;

    memset(buf,0,sizeof(buf));
    memset(bak,0,sizeof(bak));

    memcpy(bak,name,strlen(name));
#if 0
    cwmp_log_debug("%s ---- %d name: %s\n",__func__,__LINE__,bak);
    cwmp_log_debug("%s ---- %d value: %s\n",__func__,__LINE__,value);
#endif

    tmp = strtok(bak,".");

    backconf = conf;
    while(tmp)
    {
        conf = backconf;
        for(; conf->name != NULL; conf++) {
#if 0
            cwmp_log_debug("%s ---- %d ;conf->name : %s ; tmp : %s \n",__func__,__LINE__, conf->name, tmp);
#endif
            if(TRstrcasecmp(conf->name, tmp) == 0) {
                switch(conf->privilege) {
                    case INST:
                        mibType = conf->mibtype;
                        findInstType = 1;
                        break;
                    default:
                        break;
                }
                conf = conf->child;
                break;
            }
        }
        if(findInstType == 1) 
        {
            if(TRstrcasecmp(tmp,"cwmp") == 0)
            {
                update = FALSE;
            }
            break;
        }
        tmp = strtok(NULL,".");
    }

    if(findInstType == 1)
    {
        tmp = strtok(NULL,".");
        int idk = 0;
        if(tmp != NULL)
        {
#if 0
            cwmp_log_debug("%s ---- %d ; tmp: %s\n",__func__,__LINE__,tmp);
#endif
            ProfInstLowHigh(mibType, &max, &min);
            if(max <= 1) 
            {
                prof = ProfGetInstPointByIndex(mibType, 0);
            }else{
                //prof = ProfGetInstPointByName(mibType, tmp);
                prof = ProfGetInstPointByIndex(mibType, atoi(tmp));
                idk = atoi(tmp);
                tmp = strtok(NULL,".");
            }
        }
        //prof = ProfGetInstPointByIndex(mibType, atoi(tmp));
        if(prof == NULL) 
        {
            fprintf(stderr, "profile inst name error,can not find profile inst.\n");
            return FAULT_CODE_9005;
        }

        while(tmp)
        {
            for(; conf->name != NULL; conf++) 
            {
#if 0
                    cwmp_log_debug("%s ---- %d ;conf->name : %s ; tmp : %s \n",__func__,__LINE__, conf->name, tmp);
#endif
                if(TRstrcasecmp(conf->name ,tmp) == 0) 
                {
                    find_param_name = 1;
                    if(conf->privilege == VALUE) { 
                        //ProfBackupByIndex(mibType, profction, index, &profhead);
                        int back_index = PROFFAIL;
                        //back_index = ProfBackupByName(mibType, PROF_CHANGE_EDIT, idk, &profhead);
                        //back_index = ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, idk, &profhead);
                        back_index = ProfBackupByIndexAndFlag(mibType, PROF_CHANGE_EDIT, idk, &profhead,1);
#if 0
                        fprintf(stderr,"%s-%d: mibType: %d, back_index: %d\n",__func__,__LINE__,mibType,back_index);
                        fprintf(stderr,"%s-%d: idk : %d\n",__func__,__LINE__,idk);
#endif

                        mxml_node_t node;
                        mxml_node_t node_value;
                        memset(&node, 0, sizeof(node));
                        memset(&node_value, 0, sizeof(node_value));
                        if(value != NULL)
                        {
                            node_value.value.text.string = value;
                        }else{
                            node_value.value.text.string = "";
                        }
                        node.child = &node_value;
#if 0
                        fprintf(stderr, "node set value string: %s\n",node.child->value.text.string);
#endif
                        parseProfileParaFromXml(conf, &node, prof);

                        if(update == TRUE)
                        {
                            ProfUpdate(profhead);
                            fprintf(stderr, "you make profile inst '%s' to work.\n",ProfGetInstNameByIndex(mibType,idk));
                            ProfFreeAllocList(profhead);
                        }

                        nvramWriteCommit();
                        return FAULT_CODE_OK;
                    } else if(conf->child != NULL){
                        conf = conf->child;
                        break;
                    }
                }
            }
            if(find_param_name == 0) {
                if(backconf!=NULL) {
                    for(; backconf->name != NULL; backconf++) {
                        fprintf(stderr, "%s \n",backconf->name);
                    }
                }
            }
            tmp = strtok(NULL,".");
        }
    }
    return FAULT_CODE_9005;
}
int send_to_lan_2060p(char * type,char*  param)
{
    int    sockfd;
    char   sendb[1024];
    char   recvb[1024];
    struct sockaddr_in server_addr;
    char   serverIP[16] = {0};
    struct timeval tv_out;
    int    opt = SO_REUSEADDR;

    getIfIp((char *)getLanIfName(), serverIP);

    cwmp_log_debug("server ip is : %s\n", serverIP);
    if (inet_aton(serverIP, &server_addr.sin_addr) == 0) 
    {
        fprintf(stderr, "the hostip is not right!");
        return FAULT_CODE_9002;
    }
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {
        fprintf(stderr, "Socket Error:%s\n\a", strerror(errno));
        return !FAULT_CODE_9002;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(2060);

    tv_out.tv_sec = 10;
    tv_out.tv_usec = 0;
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    setsockopt(sockfd, SOL_SOCKET,SO_RCVTIMEO,&tv_out, sizeof(tv_out));

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0) {
        fprintf(stderr, "Connect Error:%s\n\a", strerror(errno));
        return FAULT_CODE_9002;
    }
    memset(sendb,0,sizeof(sendb));
    snprintf(sendb, (sizeof(sendb) - 1),
	          "GET /operation?identify=TR069&cmd=%s&argv=%s; HTTP/1.0\r\n"
	          "User-Agent: WiFiDog 200905\r\n"
	          "Host: %s\r\n"
	          "\r\n"
		  ,type,param,serverIP);
    if(send(sockfd,sendb,strlen(sendb),0) == -1)
    {
        perror("send error.\n"); 
        close(sockfd);
        return FAULT_CODE_9002;
    }
    memset(recvb,0,sizeof(recvb));
    recv(sockfd,recvb,sizeof(recvb),0);

    close(sockfd);
    if((strstr(recvb,"state") != NULL) && (strstr(recvb,"1") != NULL))
        return FAULT_CODE_OK;
    return FAULT_CODE_OK;
}

void macstr_filter_colon(const char *mac,char *retb)
{
    int i = 0, j = 0;
    char tmp[3]  = {0};
    tmp[2] = 0;

    for(i = 0; i < 6; i++) 
    {
        for(j = 0; j < 2; j++) 
        {
            tmp[j] = *(mac + i*2 + j);
        }
        strcat(retb,tmp);
        mac++;
    }
}
void macstr_add_colon(const char *mac,char *retb)
{
    int i = 0, j = 0;
    char tmp[3]  = {0};
    tmp[2] = 0;

    for(i = 0; i < 6; i++) 
    {
        for(j = 0; j < 2; j++) 
        {
            tmp[j] = *(mac + i*2 + j);
        }
        strcat(retb,tmp);
        if(i < 5)
        {
            strcat(retb,":");
        }
    }
}
void wan_lan_port_statics(unsigned int eth2Port , sw_port_stat_t *port_stat)
{
    FILE *f;
    char buf[256];
    unsigned long rx_bytes, tx_bytes;
    unsigned int rx_pkts, tx_pkts, rx_err, tx_err;
    char *p, *ifname,*portName;
    unsigned int wanIndex=0;
    
    if(eth2Port == 0){
	portName = getLanIfName();
    }else{
	portName = getWanIfName((int)eth2Port);
    }

    if ((f = fopen("/proc/net/dev", "r")) != NULL) 
    {
	fgets(buf, sizeof(buf), f);
	
	fgets(buf, sizeof(buf), f);
	while (fgets(buf, sizeof(buf), f)) 
	{
	    if ((p = strchr(buf, ':')) != NULL)
	    {
		*p = 0;
		
		if ((ifname = strrchr(buf, ' ')) == NULL) 
		{
		    ifname = buf; /* To get interface name */
		}
	       	else 
		{
		    ++ifname;
		}
		
		if (strcmp(portName,ifname) == 0)
		{
		    /* read interface informations */
		    if (sscanf(p + 1, "%lu%u%u%*u%*u%*u%*u%*u%lu%u%u", &rx_bytes,
				&rx_pkts, &rx_err, &tx_bytes, &tx_pkts, &tx_err) == 6)
		    {
		        /* statistics the current interface informations */
                        /* rx or tx packets */
		        port_stat->rxPkts = rx_pkts;
		        port_stat->txPkts = tx_pkts;
		        /* rx or tx bytes */
		        port_stat->rxOctets = rx_bytes;
                        port_stat->txOctets = tx_bytes;

		        /* rx or tx error packets */
		        port_stat->rxPktsErrors = rx_err;
		        port_stat->txPktsErrors = tx_err;

			break;
		    }
		}
	    }
	}
    }
    fclose(f); /* close the file */
    return;
}
