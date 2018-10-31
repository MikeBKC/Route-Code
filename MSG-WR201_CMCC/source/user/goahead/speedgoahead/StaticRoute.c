#include "uttMachine.h"
#if (STATIC_ROUTE == FYES)
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <syslog.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "argcmd.h"
#include "comExeFun.h"
#include "a2n.h"
#include "internet.h"
#include "StaticRoute.h"
#include "utt_conf_define.h"
#if(STATIC_ROUTE_BIND_PPTP == FYES)
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h> 
#include "utt_pppoeClient.h"
#endif


//#define SWORD_DEBUG 1
#if SWORD_DEBUG 
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif
#if (CAPWAP_CONFIG_AP == FYES)
extern void set_capwap_dns(void);
#endif
/*
 * initServer.cÖÐ¶¨ÒåµÄº¯Êý
 */
extern int headbits(unsigned int nm);
extern int _getWanLinkState(int wanNo);
extern int isWanInUse(int wanNo);

extern int HaveOtherMainAlive(int wanNo);
extern int get_phylink_status(int wanIndex);

#if(STATIC_ROUTE_BIND_PPTP == FYES)
static char* IsPptpServerUse(char *name);
static char* IsPptpClientUse(char *name);
static void EditStaticRoute(StaticRouteProfile *prof, int flag, char *dev);
#else
static void EditStaticRoute(StaticRouteProfile *prof, int flag);
#endif
#if ((EASY_TASK == FYES) && (HOTEL_PNP == FYES))
extern void addGwForETask(void);
extern void delRtForETask(void);
#endif

#if (UTT_IPAUTOREP ==FYES)
extern void IPClashAutoRepair(void);
#endif
#if(UTT_DOMAINACCESS ==FYES)
int isWanLinkStatus;
#endif
/* read the given $line (not more than 127 bytes) from $filename to $buf 
 * buf is not null-terminated if line string is 127 bytes */
static char* read_file_line(char *filename, char *buf, int line)
{
    int i=1;
    FILE *fp; 
    char*p;
    /* open file */
    fp=fopen(filename, "r");
    if (fp == NULL ) {
        DBGPRINT("open %s error\n", filename);
    }else {
        /* the content of the file is a byte, which indicant 
         * the link status of the chnl_name port. 
         * while '1' for up, and '0' for down */
        while ((fgets((void *)buf, 127U, fp)) > 0U) {
            if( i == line ) {
                /* make sure the buf is null terminated */
                p = strchr(buf, '\n');
                if( p != NULL ) {
                    *p='\0';
                }
                /* it is the pointed line, just retun */
                break;
            }
	    /* Ã»¶Á³öÅäÖÃ£¬Çå¿Õ */
	    memset(buf, 0, 127);
            i++;
        }
        /* close file at last*/
        fclose(fp);
    }
    return buf;
}

#if(STATIC_ROUTE_BIND_PPTP == FYES)
#else
/*
 * ÃèÊö£º
 *  ÅÐ¶Ï¾²Ì¬Â·ÓÉ°ó¶¨µÄÊÇÊµ½Ó¿Ú»¹ÊÇÐé½Ó¿Ú£¨°ó¶¨ÔÚLAN,WANÈÏÎªÊÇÊµ½Ó¿Ú;
 *  °ó¶¨ÔÚpptp l2tp²¦ºÅ½Ó¿ÚÈÏÎªÊÇÐé½Ó¿Ú£©
 *
 * ÊäÈë²ÎÊý£º
 *  interface - ¾²Ì¬Â·ÓÉ°ó¶¨µÄ½Ó¿Ú 
 *
 * Êä³ö²ÎÊý£º
 *  index - Êµ½Ó¿Ú»òÕßÐé½Ó¿Ú¶ÔÓ¦µÄindex£¬¼´´ÓÄÄ¸ö½Ó¿Ú³öÈ¥de
 *
 * ·µ»ØÖµ£º1 - Êµ½Ó¿Ú£¬2 - Ðé½Ó¿Ú , -1 - ´íÎó
 */
static int routeInterfaceType(char *interface, int *index) {
    SR_PRINTF("%s: interface=%s\n", __FUNCTION__, interface);
    /*
     * Èç¹û½Ó¿ÚÒÔLAN»òÕßWAN¿ªÍ·£¬ÔòÈÏÎªÊÇÊµ½Ó¿Ú
     */
    if (strncmp(interface, RT_LAN_PRE, strlen(RT_LAN_PRE)) == 0) { /* lan¿Ú */
	*index = 0;
	return 1;
    } else if (strncmp(interface, RT_WAN_PRE, strlen(RT_WAN_PRE)) == 0) { /* wan¿Ú */
	/*
	 * wan½Ó¿Ú³¤¶ÈÎª4¸ö×Ö·û£¬ÀýÈçWAN1,WAN2,WAN3µÈ
	 */
	if (strlen(interface) != 4) {
	    return -1;
	}
	*index = interface[3] - '0';
	return 1;
    } else {
	return 2;
    }
}
#endif

#if 0
/*
 * ÃèÊö£º   
 *  true interface route change
 *  °ó¶¨ÔÚÕæÊµ½Ó¿ÚµÄÂ·ÓÉ·¢Éú±ä»¯
 * ÊäÈë²ÎÊý£º
 *  index - ¶ÔÓ¦µÄ½Ó¿Ú
 */
static void TIRouteChg(int index) {
    /* ÏÈÇå³ý¶ÔÓ¦½Ó¿ÚÓÃ»§Â·ÓÉ */
    FlushUserRoute(index);
    /* ÔÙÖØÐÂÌí¼Ó½Ó¿Ú¶ÔÓ¦ÓÃ»§Â·ÓÉ */
    SR_PRINTF("%s:index=%d,state=%d\n", __FUNCTION__, index, _getWanLinkState(index));
    if ((index == 0) || _getWanLinkState(index)) { /* lan¿Ú»òÕßwan¿Úup */
        AddUserRoute(index);
    }
    return;
}
#else
#if(STATIC_ROUTE_BIND_PPTP == FYES)
extern void TIRouteChg(void) 
{
    MibProfileType mibType = MIB_PROF_STATIC_ROUTE;
    StaticRouteProfile *profRoute = NULL;
    int min = 0, max = 0, i = 0;
    int wanNo;
    char *dev = NULL;

    SR_PRINTF("%s has been called\n", __FUNCTION__);
    /* ÇåÂ·ÓÉ±í */
    FlushUserRoute();
    /* ÖØÐÂÅäÖÃÂ·ÓÉ±í */

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) {
	profRoute = (StaticRouteProfile *)ProfGetInstPointByIndex(mibType, i);
	if ((profRoute != NULL) && (ProfInstIsFree(profRoute) == 0)) {
	    /* lan¿ÚÂ·ÓÉ */
	    if (strncmp(profRoute->interfaceName, RT_LAN_PRE, strlen(RT_LAN_PRE)) == 0) {
		EditStaticRoute(profRoute, 2, NULL);
	    } 
	    /* wan¿ÚÂ·ÓÉ */
	    else if (strncmp(profRoute->interfaceName, RT_WAN_PRE, strlen(RT_WAN_PRE)) == 0) {
		wanNo = profRoute->interfaceName[3] - '0';
		/* ½Ó¿ÚÆðÀ´ÁË */
		if (_getWanLinkState(wanNo)) {
		    EditStaticRoute(profRoute, 2, NULL);
		}
	    }
	    else if(strncmp(profRoute->interfaceName, RT_PPTP_SERVER_PRE, strlen(RT_PPTP_SERVER_PRE)) == 0)
	    {/*PPTP·þÎñÆ÷Ðé½Ó¿Ú*/
		dev = IsPptpServerUse(profRoute->interfaceName);
		SR_PRINTF("%s-%d, dev = %s\n", __FUNCTION__, __LINE__, dev);
		if(*dev != '\0')
		{
		    EditStaticRoute(profRoute, 2, dev);
		}
	    }
	    else if(strncmp(profRoute->interfaceName, RT_PPTP_CLIENT_PRE, strlen(RT_PPTP_CLIENT_PRE)) == 0)
	    {/*PPTP¿Í»§¶ËÐé½Ó¿Ú*/
		dev = IsPptpClientUse(profRoute->interfaceName);
		SR_PRINTF("%s-%d, dev = %s\n", __FUNCTION__, __LINE__, dev);
		if(*dev != '\0')
		{
		    EditStaticRoute(profRoute, 2, dev);
		}
	    } else {
		SR_PRINTF("%s: no lan or wan route\n", __FUNCTION__);;
	    }
	}
    }

    doSystem("echo 1 >/proc/sys/net/ipv4/route/flush ");

    return;
}
#else
/*
 * ÃèÊö£º
 *  true interface route change
 *  Êµ½Ó¿ÚÂ·ÓÉ·¢Éú±ä»¯
 * ÊäÈë²ÎÊý£º
 *  ÎÞ
 * ËµÃ÷£º
 *  ÓÃ»§Êµ½Ó¿Ú¾²Ì¬Â·ÓÉÅäÖÃÔÚÒ»ÕÅÂ·ÓÉ±íÖÐ:120.
 *  520ÊÇÃ¿¸ö½Ó¿ÚÅäÖÃÒ»ÕÅÂ·ÓÉ±í£¬ÎªÁË½â¾ömantis 8175
 */
extern void TIRouteChg(void) {
    MibProfileType mibType = MIB_PROF_STATIC_ROUTE;
    StaticRouteProfile *profRoute = NULL;
    int min = 0, max = 0, i = 0;
    int wanNo;

    SR_PRINTF("%s has been called\n", __FUNCTION__);
    /* ÇåÂ·ÓÉ±í */
    FlushUserRoute();
    /* ÖØÐÂÅäÖÃÂ·ÓÉ±í */

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) {
	profRoute = (StaticRouteProfile *)ProfGetInstPointByIndex(mibType, i);
	if ((profRoute != NULL) && (ProfInstIsFree(profRoute) == 0)) {
	    /* lan¿ÚÂ·ÓÉ */
	    if (strncmp(profRoute->interfaceName, RT_LAN_PRE, strlen(RT_LAN_PRE)) == 0) {
		EditStaticRoute(profRoute, 2);
	    } 
	    /* wan¿ÚÂ·ÓÉ */
	    else if (strncmp(profRoute->interfaceName, RT_WAN_PRE, strlen(RT_WAN_PRE)) == 0) {
		wanNo = profRoute->interfaceName[3] - '0';
		/* ½Ó¿ÚÆðÀ´ÁË */
		if (_getWanLinkState(wanNo)) {
		    EditStaticRoute(profRoute, 2);
		}
	    } else {
		SR_PRINTF("%s: no lan or wan route\n", __FUNCTION__);;
	    }
	}
    }

//    if ((index == 0) || _getWanLinkState(index)) { /* lan¿Ú»òÕßwan¿Úup */
    /* flush route cache */
    doSystem("echo 1 >/proc/sys/net/ipv4/route/flush ");

    return;
}
#endif

#endif

#if(STATIC_ROUTE_BIND_PPTP == FYES)
/******************************************************************** 
 *  º¯Êý£º staticRouteProfileChange 
 *  ¹¦ÄÜ£º ¾²Ì¬Â·ÓÉprofile change 
 *  ´´½¨£º 2012-10-15 
 *  ²ÎÊý£º ÂÔ 
 *  ·µ»Ø£º ÂÔ 
 *  Êä³ö£º ÂÔ 
 *  ×÷Õß£º Jianqing.Sun  
 *********************************************************************/ 
static void staticRouteProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr )
{
    switch(changeType) 
    {
	case PROF_CHANGE_ADD:
	    TIRouteChg();
	    break;
	case PROF_CHANGE_EDIT:
	    /* for mantis 8175 */
	    TIRouteChg();
	    break;
	case PROF_CHANGE_DEL:
	    TIRouteChg();
	    break;
	case PROF_CHANGE_DELALL:
	    /*
	     * É¾³ýÓÃ»§Â·ÓÉ
	     */
	    FlushUserRoute();
	    break;
	default:
	    break;
    }
}
#else
/*
 * ¾²Ì¬Â·ÓÉprofile change
 */
static void staticRouteProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr )
{
    StaticRouteProfile *newProf = (StaticRouteProfile *)newProfileDataPtr;
    StaticRouteProfile *oldProf = (StaticRouteProfile *)oldProfileDataPtr;
    int indexNew, iTypeNew, indexOld, iTypeOld;
#if 0
    Uint32 wanCount = 0;
#endif

    switch(changeType) {
        case PROF_CHANGE_ADD:
	    SR_PRINTF("%s:ADD, interface=%s\n", __FUNCTION__, newProf->interfaceName);
	    /* »ñµÃÐÂ½¨ÅäÖÃ½Ó¿ÚÀàÐÍ */
	    iTypeNew = routeInterfaceType(newProf->interfaceName, &indexNew);
	    if (iTypeNew == 1) { /* Êµ½Ó¿Ú */
#if 0
		/* ÏÈÇå³ý¶ÔÓ¦½Ó¿ÚÓÃ»§Â·ÓÉ */
		FlushUserRoute(indexNew);
		/* ÔÙÖØÐÂÌí¼Ó½Ó¿Ú¶ÔÓ¦ÓÃ»§Â·ÓÉ */
		if ((indexNew == 0) || _getWanLinkState(indexNew)) { /* lan¿Ú»òÕßwan¿Úup */
		    AddUserRoute(indexNew);
		}
#endif
#if 0
		TIRouteChg(indexNew);
#else
		/* for mantis 8175 */
		TIRouteChg();
#endif
	    } else if (iTypeNew == 2) { /* Ðé½Ó¿Ú */
		/* TODO */
	    } else { /* ´íÎó */
		syslog(LOG_INFO, "%s: ADD, get route bind interface error.interface:%s.", 
			__FUNCTION__, newProf->interfaceName);
	    }
            break;
        case PROF_CHANGE_EDIT:
	    SR_PRINTF("%s:EDIT\n", __FUNCTION__);
	    iTypeNew = routeInterfaceType(newProf->interfaceName, &indexNew);
	    iTypeOld = routeInterfaceType(oldProf->interfaceName, &indexOld);

	    /*
	     * ·ÖÀà£º
	     *  1 newºÍold¶¼ÊÇÊµ½Ó¿Ú
	     *  2 newÊµ oldÐé
	     *  3 newÐé oldÊµ
	     *  4 newÐé oldÐé
	     */
	    if ((iTypeNew == 1) && (iTypeOld == 1)) { /* 1 */
		/* ½Ó¿ÚÏàÍ¬£¬Ö»ÒªÕë¶Ô´Ë½Ó¿ÚÓÃ»§Â·ÓÉÐÞ¸Ä */
#if 0
		if (indexNew == indexOld) {
#if 0
		    FlushUserRoute(indexNew);
		    if ((indexNew == 0) || _getWanLinkState(indexNew)) {
			AddUserRoute(indexNew);
		    }
#endif
		    TIRouteChg(indexNew);
		} else { /* ·ñÔòÐèÒª·Ö±ðÐÞ¸Ä */
#if 0
		    FlushUserRoute(indexOld);
		    if ((indexOld == 0) || _getWanLinkState(indexOld)) { 
			AddUserRoute(indexOld);
		    }
		    FlushUserRoute(indexNew);
		    if ((indexNew == 0) || _getWanLinkState(indexNew)) { 
			AddUserRoute(indexNew);
		    }
#endif
		    TIRouteChg(indexOld);
		    TIRouteChg(indexNew);
		}
#else
		/* for mantis 8175 */
		TIRouteChg();
#endif
	    } else if ((iTypeNew == 1) && (iTypeOld == 2)) { /* 2 */
		/* TODO */
	    } else if ((iTypeNew == 2) && (iTypeOld == 1)) { /* 3 */
		/* TODO */
	    } else if ((iTypeNew == 2) && (iTypeOld == 2)) { /* 4 */
		/* TODO */
	    } else {
		syslog(LOG_INFO, "%s: EDIT, get route bind interface error.new:%s,old:%s.", 
			__FUNCTION__, newProf->interfaceName, oldProf->interfaceName);
	    }

            break;
        case PROF_CHANGE_DEL:
	    SR_PRINTF("%s:DEL\n", __FUNCTION__);
	    iTypeOld = routeInterfaceType(oldProf->interfaceName, &indexOld);
	    if (iTypeOld == 1) {
#if 0
		TIRouteChg(indexOld);
#else
		/* for mantis 8175 */
		TIRouteChg();
#endif
	    } else if (iTypeOld == 2){
		/*
		 * TODO
		 */
	    } else {
		syslog(LOG_INFO, "%s: DEL, get route bind interface error.interface:%s.", 
			__FUNCTION__, newProf->interfaceName);
	    }
            break;
        case PROF_CHANGE_DELALL:
	    SR_PRINTF("%s:DELALL\n", __FUNCTION__);
	    /*
	     * É¾³ýÊµ½Ó¿ÚÓÃ»§Â·ÓÉ
	     */
#if 0
	    wanCount = getWanSlotCount();
	    for (indexOld = 0; indexOld <= wanCount; indexOld++) {
		TIRouteChg(indexOld);
	    }
#else
	    FlushUserRoute();
#endif
	    /*
	     * É¾³ýÐé½Ó¿ÚÓÃ»§Â·ÓÉ
	     */
	    /*
	     * TODO
	     */
            break;
        default:
            break;
    }
}
#endif

/*
 * ³õÊ¼»¯
 */
static void staticRouteInit(void) {

    return;
}

extern void funInitStaticRoute(void) {
    registerForProfileUpdates(MIB_PROF_STATIC_ROUTE, NULL, staticRouteProfileChange);
    staticRouteInit();
    return;
}

#if 0
/* flush all the route that user configed */
extern void FlushUserRoute(int wanNo)
{
    /* different wan port using different route table */
    doSystem("ip route flush table %d", USR_ROUTE_TABLE_BASE + wanNo);
}
#else
/* 
 * flush all the route that user configed
 * for mantis 8175
 */
extern void FlushUserRoute(void)
{
    /* different wan port using different route table */
    doSystem("ip route flush table %d", USR_ROUTE_TABLE_BASE);
    doSystem("ip route flush table %d", USR_ROUTE_LAN_TABLE_BASE);
}

#endif

#if(STATIC_ROUTE_BIND_PPTP == FYES)
struct flock* file_lock(short type, short whence)
{
    static struct flock ret;
    ret.l_type = type ;
    ret.l_start = 0;
    ret.l_whence = whence;
    ret.l_len = 0;
    ret.l_pid = getpid();
    return &ret;
}
/********************************************************************
 * º¯Êý£º IsPptpServerUse
 * ¹¦ÄÜ£º ¼ì²é´ËPPTPÕËºÅÊÇ·ñÕýÔÚÊ¹ÓÃ
 * ´´½¨£º 2012-10-15
 * ²ÎÊý£º ÕËºÅÃû
 * ·µ»Ø£º NULL	    -         ´ËÕËºÅÎ´Ê¹ÓÃ
 *	  ÆäËü	    -         ´ËÕËºÅÕýÔÚÊ¹ÓÃ
 * Êä³ö£º ÂÔ
 * ×÷Õß£º Jianqing.Sun 
 ********************************************************************/ 
static char* IsPptpServerUse(char *name)
{
    MibProfileType mibType = MIB_PROF_PPTP_SRV_ACCOUNT;
    PPTPSrvAccProfile *profPptpA = NULL;
    int fd = -1;
    static char dev[8];/*×î´óÎªppp9999*/
    int     st_size;
    off_t   start, end;
    int     fileLen;
    struct st_uttPppdClient info;
    st_size = sizeof(struct st_uttPppdClient);
    char PptpName[UTT_INST_NAME_LEN + 1];

    memset(PptpName, 0, sizeof(PptpName));
    memset(dev, 0, sizeof(dev));
    strcpy(PptpName, name + strlen(RT_PPTP_SERVER_PRE));
    SR_PRINTF("%s------%d, PptpName = %s\n", __func__, __LINE__, PptpName);

    profPptpA = (PPTPSrvAccProfile *)ProfGetInstPointByName(mibType, PptpName);
    if((profPptpA != NULL) && (ProfInstIsFree(profPptpA) == 0))
    {
	fd = open(UTT_PPTP_CLIENT_INFO_FILE, O_RDWR|O_CREAT, 0666);
	if (fd != -1) 
	{/*ÎÄ¼þ´ò¿ª³É¹¦*/
	    start = lseek(fd, 0, SEEK_SET);
	    end = lseek(fd, 0, SEEK_END);
	    fileLen = end - start;
	    lseek(fd, 0, SEEK_SET);
	    while(fileLen >= st_size) 
	    {
		fcntl(fd, F_SETLKW, file_lock(F_WRLCK|F_RDLCK, SEEK_SET));
		read(fd, &info, st_size);
		fcntl(fd, F_SETLKW, file_lock(F_UNLCK, SEEK_SET));
		if((info.ConTime == 0) &&(strcmp(info.user,profPptpA->userName) == 0))
		{/*´ËÕËºÅÒÑÁ¬½ÓÉÏÇÒÓÃ»§ÃûÏàÍ¬*/
		    strcpy(dev, info.ifname);
		    break;
		}
		fileLen -= st_size;/*ÎÄ¼þ³¤¶È¼õ¶Ì*/
	    }

	    close(fd);/*¹Ø±ÕÎÄ¼þ*/
	}
    }
    return dev;
}
/********************************************************************
 * º¯Êý£º IsPptpClientUse
 * ¹¦ÄÜ£º ¼ì²é´ËPPTPÕËºÅÊÇ·ñÕýÔÚÊ¹ÓÃ
 * ´´½¨£º 2012-10-15
 * ²ÎÊý£º ÕËºÅÃû
 * ·µ»Ø£º NULL	    -         ´ËÕËºÅÎ´Ê¹ÓÃ
 *	  ÆäËü	    -         ´ËÕËºÅÕýÔÚÊ¹ÓÃ
 * Êä³ö£º ÂÔ
 * ×÷Õß£º Jianqing.Sun 
 ********************************************************************/ 
static char* IsPptpClientUse(char *name)
{
    char fileName[64];
    char linestr[512];
    static char dev[8];/*×î´óÎªppp9999*/
    FILE* fp = NULL;
    char PptpName[UTT_INST_NAME_LEN + 1];

    memset(fileName, 0, sizeof(fileName));
    memset(linestr, 0, sizeof(linestr));
    memset(dev, 0, sizeof(dev));
    strcpy(PptpName, name + strlen(RT_PPTP_CLIENT_PRE));
    SR_PRINTF("%s------%d, PptpName = %s\n", __func__, __LINE__, PptpName);
    sprintf(fileName, "/var/run/pptp_%s.ppp", PptpName);
    fp = fopen(fileName, "r");
    if(fp != NULL)
    {
	if(fgets(linestr, 512, fp) != NULL)/*¶ÁÈ¡Ò»ÐÐ*/
	{
	    sscanf(linestr, "%s", dev);/*¶ÁÈ¡Ðé½Ó¿ÚÃû*/
	}
	fclose(fp);/*¹Ø±ÕÎÄ¼þ*/
    }
    return dev;
}
#endif
#if(STATIC_ROUTE_BIND_PPTP == FYES)
static void EditStaticRoute(StaticRouteProfile *prof, int flag, char *dev)
#else
static void EditStaticRoute(StaticRouteProfile *prof, int flag)
#endif
{
/* 
 * EditStaticRoute edit a static route, $prof gives a route infomation
 *      in the profile format. flag indentfy the action add or delete
 * this is used for user config route now. 
 */
#if 0
    char cmdstr[128], buf[64], cmdtmp[32];
    char RouteName[32], enable[2], aIp[32], aGw[32], metric[2], outdev[8],*poutdev, action[8];
    char_t *tmpRecord, *wanConnMode;
    unsigned int dstIp,dstmask,gw;
    int wanNo=0;

    /* initiate all local variables */
    memset(cmdstr, 0x00, sizeof(cmdstr));
    memset(cmdtmp, 0x00, sizeof(cmdtmp));
    memset(buf, 0x00, sizeof(buf));
    memset(RouteName, 0x00, sizeof(RouteName));
    memset(enable, 0x00, sizeof(enable));
    memset(aIp, 0x00, sizeof(aIp));
    memset(aGw, 0x00, sizeof(aGw));
    memset(metric, 0x00, sizeof(metric));
    memset(outdev, 0x00, sizeof(outdev));
    memset(action, 0x00, sizeof(action));
#endif
    char action[RT_ACTION_LEN + 1], cmdstr[RT_CMD_LEN], cmdtmp[RT_TMP_LEN];
    char *poutdev = NULL;
    enum em_eth_conn_mode wanConnMode = ETH_CONN_STATIC;
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
    struct in_addr dstIp, gwIp;
    int wanNo = 0;
    int islan = 0;

    memset(action, 0, RT_ACTION_LEN + 1);
    memset(cmdstr, 0, RT_CMD_LEN);
    memset(cmdtmp, 0, RT_TMP_LEN);

    /* translate the flag to meet ip route's need */
    if (flag == 1) {
        strncpy(action, RT_ACTION_ADD, RT_ACTION_LEN);
    }else if (flag == 2 ){
        strncpy(action, RT_ACTION_REP, RT_ACTION_LEN);
    }else {
        /* delete action can be execute repeatly */
        strncpy(action, RT_ACTION_DEL, RT_ACTION_LEN);
    }
#if 0
    tmpRecord=RouteRec;

    /* get the route data field 
     * to corresponding variables */
    sscanf(tmpRecord,"%[^,],%[^,],%x,%x,%x,%[^,],%[^;]",RouteName,enable,&dstIp,&dstmask,&gw,metric,outdev);
#endif
    if (prof->head.active == 0) {
        /*
	 * if this record is diabled, 
         * it sould be returned 
	 */ 
        SR_PRINTF("route[%s] is disabled.\n", prof->head.name);
    } else {
	SR_PRINTF("%s: interface=%s\n", __FUNCTION__, prof->interfaceName);
        /* get port connect mode, 
         * and the port name */
        if(strcmp(prof->interfaceName, "LAN0") == 0){
            poutdev = (char *)getLanIfName();
            wanConnMode = ETH_CONN_STATIC;
	    islan = 1;
	    SR_PRINTF("%s:LAN, poutdev=%s\n", __FUNCTION__, poutdev);
        } else if (strncmp(prof->interfaceName, RT_WAN_PRE, strlen(RT_WAN_PRE)) == 0) {
            wanNo = prof->interfaceName[3] - '0';
            poutdev = getWanIfNamePPP(wanNo);
	    SR_PRINTF("%s: WAN, poutdev=%s\n", __FUNCTION__, poutdev);
	    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, wanNo);
	    if (profIF == NULL) {
		SR_PRINTF("%s: catn't get wan interface profile \n", __FUNCTION__);
		return;
	    }
	    wanConnMode = profIF->ether.connMode;
#if(STATIC_ROUTE_BIND_PPTP == FYES)
	}
	else if(strncmp(prof->interfaceName, RT_PPTP_PRE, strlen(RT_PPTP_PRE)) == 0)
	{/*PPTPÐé½Ó¿Ú*/
	    poutdev = dev;
	    SR_PRINTF("%s------%d: PPTP, poutdev=%s\n", __FUNCTION__, __LINE__, poutdev);
            wanConnMode = ETH_CONN_PPPOE;/*Ðé½Ó¿ÚÕâÖÖµãµ½µãµÄ²»ÓÃ¼ÓÍø¹Ø*/
#endif
        } else { /* Ðé½Ó¿Ú */
	    SR_PRINTF("%s: virtual port\n", __FUNCTION__);
	    /*
	     * TODO
	     */
	}

	/*
	 * »ñµÃÄ¿µÄipµØÖ·ºÍÍø¹Ø
	 */
	dstIp.s_addr = prof->dstAddr;
	gwIp.s_addr = prof->gwAddr;

        SR_PRINTF("EditStaticRoute:%s: RouteName[%s], wanNo[%s]\n",action, prof->head.name, poutdev);
        /* different wan port using different route table */
#if 0
        sprintf(cmdstr, "ip route %s table %d %s/%d dev %s metric %s ", action,
            USR_ROUTE_TABLE_BASE+wanNo, int2ip(dstIp, aIp), headbits(dstmask), poutdev, metric); 
#endif
#if 0
	sprintf(cmdstr, "ip route %s table %d %s/%d dev %s metric %d ", action, 
		USR_ROUTE_TABLE_BASE+wanNo, inet_ntoa(dstIp), headbits(ntohl(prof->netmask)),
		poutdev, prof->preference); 
#else
	if(islan == 1) {
	sprintf(cmdstr, "ip route %s table %d %s/%d dev %s metric %d ", action, 
		USR_ROUTE_LAN_TABLE_BASE, inet_ntoa(dstIp), headbits(ntohl(prof->netmask)),
		poutdev, prof->preference); 
	} else {
	/* for mantis 8175 */
	sprintf(cmdstr, "ip route %s table %d %s/%d dev %s metric %d ", action, 
		USR_ROUTE_TABLE_BASE, inet_ntoa(dstIp), headbits(ntohl(prof->netmask)),
		poutdev, prof->preference); 
	}
#endif

#if 0
        if (strncmp("DHCP", wanConnMode, 4) == 0) {
            /* use the user's configure 
             * instead of dynamic gateway */
            int2ip(gw, aGw);
#if 0 
            getWanDhcpDefaultRoute(wanNo, aGw);
#endif
            sprintf(cmdtmp, "via %s ", aGw);
            strcat(cmdstr, cmdtmp);
        } else if (strncmp("STATIC", wanConnMode, 6) == 0) {
            /* use the user's configure gateway */
            int2ip(gw, aGw);
            sprintf(cmdtmp, "via %s ", aGw);
            strcat(cmdstr, cmdtmp);
        } else {
            /* pppoe connect mode */
            DBGPRINT("not via when pppoe mode\n");
        }
#endif
	if (wanConnMode == ETH_CONN_DHCP) {
            /* use the user's configure 
             * instead of dynamic gateway */
#if 0
            int2ip(gw, aGw);
#endif
#if 0 
            getWanDhcpDefaultRoute(wanNo, aGw);
#endif
	    /* Ê¹ÓÃµÄÓÃ»§ÅäÖÃÍø¹Ø¶ø²»ÊÇ¶¯Ì¬Íø¹Ø£¬bug? */
            sprintf(cmdtmp, "via %s ", inet_ntoa(gwIp));
            strcat(cmdstr, cmdtmp);
        } else if (wanConnMode == ETH_CONN_STATIC) {
            /* use the user's configure gateway */
#if 0
            int2ip(gw, aGw);
#endif
            sprintf(cmdtmp, "via %s ", inet_ntoa(gwIp));
            strcat(cmdstr, cmdtmp);
        } else {
            /* pppoe connect mode */
            SR_PRINTF("not via when pppoe mode\n");
        }

        doSystem(cmdstr);
	SR_PRINTF("%s-cmdstr:%s\n", __FUNCTION__, cmdstr);
    }
    return;
}

#if 0
/* mantis 8175 */
/* add the route list on wanNo */
extern void AddUserRoute(int wanNo)
{
#if 0
    char *tmpRecord, *tmp1, *tmp2, *RouteList = NULL;
    char wanName[5], RouteInfo[64];
    /* get user route configure */
    RouteList = nvram_bufget(RT2860_NVRAM,"RouteList");
    tmpRecord=RouteList;
#endif
    MibProfileType mibType = MIB_PROF_STATIC_ROUTE;
    StaticRouteProfile *profRoute = NULL;
    int min = 0, max = 0, i = 0;

    SR_PRINTF("%s has been called\n", __FUNCTION__);

    ProfInstLowHigh(mibType, &max, &min);

    for (i = min; i < max; i++) {
	profRoute = (StaticRouteProfile *)ProfGetInstPointByIndex(mibType, i);
	if ((profRoute != NULL) && (ProfInstIsFree(profRoute) == 0)) {
	    SR_PRINTF("%s:interface=%s\n", __FUNCTION__, profRoute->interfaceName);
	    if (strncmp(profRoute->interfaceName, RT_WAN_PRE, strlen(RT_WAN_PRE)) == 0) { /* wan¿Ú */
		/*
		 * ÐÞ¸Ä¶ÔÓ¦wan
		 */
		SR_PRINTF("%s: wanNo=%d, interfaceIndex=%d\n", __FUNCTION__, wanNo, (profRoute->interfaceName[3]-'0'));
		if (wanNo == (profRoute->interfaceName[3] - '0')) {
		    EditStaticRoute(profRoute, 2);
		}
	    } else if (strncmp(profRoute->interfaceName, RT_LAN_PRE, strlen(RT_LAN_PRE)) == 0) { /* lan¿Ú */
		/*
		 * ÐÞ¸Älan¿Ú
		 */
		if (wanNo == 0) { /* 520WÃ»ÓÐÕâÌõÅÐ¶Ï,bug? µ±ÖØ½¨wan¿ÚÂ·ÓÉÊ±£¬Ã»±ØÒªÖØ½¨lanÂ·ÓÉ*/
		    EditStaticRoute(profRoute, 2);
		}
	    } else { /* Ðé½Ó¿Ú */
		/*
		 * TODO
		 */
	    }
	}
    }

#if 0
    /* main loop */
    while(*tmpRecord != '\0') {
        memset(RouteInfo, 0x00, sizeof(RouteInfo));
        /* copy one record */
        strncpy(RouteInfo, tmpRecord, 63);
        char *p=NULL;
        p=strchr(RouteInfo, ';');
        /* last record */
        if(p!=NULL){
            *p='\0';
        }
        DBGPRINT("EditStaticRoute[%s]\n", RouteInfo);
        tmp2=strstr(RouteInfo, "WAN");
        /* this route used for wan port */
        if (tmp2 != NULL) {
            strncpy(wanName, tmp2, 5);
            /* get the wan index */
            if (wanNo == ((int)wanName[3]-0x30) ) {
                /* this wan's route should be changed */
                EditStaticRoute(RouteInfo, 2);
            }
        } else {
            /* this route used for lan port, 
             * anyway add it */
            tmp1=strstr(RouteInfo, "LAN");
            if (tmp1 != NULL) {
                EditStaticRoute(RouteInfo, 2);
            }else {
                /* configure error, invalid route  */
                DBGPRINT("RouteList configure error\n");
            }
        }
        /* get next record */
        tmpRecord=NextRecordStart(tmpRecord);
    }
#endif
}
#endif

//#define NAT_LOAD_BALANCE
#if defined(NAT_LOAD_BALANCE)

#define NLB_MAX_PORT_NUM MAX_WAN_IF_COUNT
/*
 * ¸üÐÂnat load balance¹æÔò
 * inUseCnt  ¼¸¸öwan´¦ÔÚÁ¬½Ó×´Ì¬
 * port          ´¦ÔÚÁ¬½Ó×´Ì¬µÄwan indexÊý×é
 */
static void update_nlb_rule(int inUseCnt, int *port) {
	int i, n = 0;
	/* Çå¿Õ¹æÔò */
	doSystem("iptables -t mangle -F nlb_chain > /dev/null 2>&1");

	if (inUseCnt <= 1) {
		return;
	}

	doSystem("iptables -t mangle -A nlb_chain  -p tcp -m  multiport  --dports 80,443 -j ACCEPT");
	for (i = 0; i < NLB_MAX_PORT_NUM; i++) {
		if (port[i] == 0) {
			continue;
		}
		doSystem("iptables -t mangle -A nlb_chain -m statistic --mode nth --every %d --packet %d -j CONNMARK --set-mark 0x%d0/0xfe", inUseCnt, n, (i + 1));
		n++;                                                                                                                 
	}
	doSystem("iptables -t mangle -A nlb_chain -j CONNMARK --restore-mark --mask 0xfe");
}
#endif

/*
 * ÃèÊö£º
 *  ¶àÏßÂ·Çé¿ö£¬ÊµÏÖ¸ºÔØ¾ùºâ
 *  µ¥ÏßÂ·Çé¿ö£¬ÊµÏÖÅäÖÃÄ¬ÈÏÂ·ÓÉ
 */
extern void ModifyDefaultRoute(int wanNo, int action)
{
/* ModifyDefaultRoute modify default route in table main.
 * $wanNo and $action is not used, just a vase.
 */
    char cmdstr[RT_CMD_LEN * 2], cmdtmp[RT_TMP_LEN * 2];
    char wanDhcpGw[RT_IP_LEN];
    char *wanIfName;
    int i, weight;
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
    enum em_eth_conn_mode wanConnType;
    Uint32 rate, wanCount = 0;
    char ip_addr[16];
    struct in_addr addr;
    long ip_get = 0u;
#if defined(NAT_LOAD_BALANCE)
    int cnt = 0;
    int port[NLB_MAX_PORT_NUM] = {0};
#endif

    /* initiate all local variable */
#if 0
    memset(cmdstr,0x00, sizeof(cmdstr));
    memset(cmdtmp,0x00, sizeof(cmdtmp));
    memset(buf,0x00, sizeof(buf));
    memset(wanDhcpGw,0x00, sizeof(wanDhcpGw));
#endif
    memset(cmdstr, 0, RT_CMD_LEN * 2);
    memset(cmdtmp, 0, RT_TMP_LEN * 2);
    memset(wanDhcpGw, 0, RT_IP_LEN);

    SR_PRINTF("ModifyDefaultRoute.\n");

    /* using equalize for 
     * load proportion balance application */
    sprintf(cmdstr, "ip route replace default equalize " );

    wanCount = getWanSlotCount();
    for(i = 1; i <= wanCount; i++) {
        /* only when the wan is in use, it processed 
         * see more for fuction isWanInUse */
        if (isWanInUse(i) == 1) {
	    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, i);
	    if (profIF == NULL) {
		SR_PRINTF("%s: get interface profile failed\n", __FUNCTION__);
		continue;
	    }

            /* get connect mode */
#if 0
            wanConnType = nvram_bufget(RT2860_NVRAM, add_wanx("ConnMode", i, buf));
#endif
	    wanConnType = profIF->ether.connMode;
	    
#if 0
	    /* 
	     * mantis 8402: dhcp¶Ï¿ªÔö¼ÓÅÐ¶Ï
	     * mantis 8427: ¹Ì¶¨IPÎÞÐèÅÐ¶Ï½Ó¿ÚÊÇ·ñ¶Ï¿ª
	     */
	    if ((get_phylink_status(i) == 0) && (wanConnType != ETH_CONN_STATIC)) {
		continue;
	    }
#endif

            /* get wan port name */
            wanIfName = getWanIfNamePPP(i);

            SR_PRINTF("ModifyDefaultRoute: Add:wanIfName[%s],wanConnType[%d]\n", wanIfName, wanConnType);

            /* make sure weight is in [1 ... 256 ] */
	    /* ¸ºÔØ¾ùºâ°´ÏßÂ·´ø¿í¾ùºâ */
	    rate = profIF->rxRate;
	    /* 
	     * È¨ÖØÈ¡ÖµÎªÏßÂ·´ø¿í³ý512Kbit 
	     * ÕâÑùÈ¡·¨Óöµ½Ð¡´ø¿í¾Í²»×¼È·ÁË,bug?
	     */
	    /* mantis 8513: 512ÐÞ¸ÄÎª500 */
	    weight = rate/(500);
	    if (weight < 1) {
		weight = 1;
#if 0
	    /*Ç§Õ×Éè±¸È¨ÖØ±È³¬¹ý256*/
	    } else if (weight > 256) {
		weight = 256;
#endif
	    }
#if 0
            int iweight=0;
            rate = nvram_bufget(RT2860_NVRAM, add_wanx("Weight", i, buf));
            iweight=(int)strtol(rate, (char **)NULL, 0);
            iweight/=512;
            if(iweight < 1) {
                iweight=1;
            }
            if(iweight >256) {
                iweight=256;
            }
            sprintf(weight, "%d", iweight);
#endif
            /* add the next hop */
	    if (wanConnType == ETH_CONN_PPPOE) {
                /* if the wanIfName is wrong, do nothing */
                if(strlen(wanIfName) >= 4) {
                    sprintf(cmdtmp,"nexthop dev %s weight %d ", wanIfName, weight);
                    strcat(cmdstr, cmdtmp);
                }
	    } else if (wanConnType == ETH_CONN_DHCP) {
		/* get dhcp gateway */
                getWanDhcpDefaultRoute(i, wanDhcpGw);
                /* if the wanDhcpGw is wrong, do nothing */
		SR_PRINTF("%s: wanDhcpGw=%s\n", __FUNCTION__, wanDhcpGw);
                if(strlen(wanDhcpGw) >= 7) {
                    sprintf(cmdtmp,"nexthop via %s dev %s weight %d ", wanDhcpGw, wanIfName, weight);
                    strcat(cmdstr, cmdtmp);
                }
                /*ÇÅ½ÓÄ£Ê½Ìí¼ÓIP½øÈëIPSET×é*/
                /*
                if(getIfIp("br1",ip_addr) == 0)
                {
                       ip_get = inet_addr(ip_addr);
                       ip_get = ip_get & 0xFFFFFF;
                       memcpy(&addr,&ip_get, 4);
                       doSystem("ipset -A sys_lan_group %s;ipset -A sys_lan_range_group %s/24",ip_addr,inet_ntoa(addr)); 
                } */
	    } else if (wanConnType == ETH_CONN_STATIC) {
		/* using user config gateway */
		struct in_addr gw;
		gw.s_addr = profIF->ether.sta.gw;
                sprintf(cmdtmp,"nexthop via %s dev %s weight %d ", 
			inet_ntoa(gw), wanIfName, weight);
#if 0
                wanGw = nvram_bufget(RT2860_NVRAM, add_wanx("Gw", i, buf));
                sprintf(cmdtmp,"nexthop via %s dev %s weight %s ", wanGw, wanIfName, weight);
#endif
                strcat(cmdstr, cmdtmp);
	    } else {
		SR_PRINTF("%s: invalid connType: %d", __FUNCTION__, wanConnType);
	    }
#if 0
            if(strncmp("PPPOE", wanConnType, 5) == 0) {
                /* if the wanIfName is wrong, do nothing */
                if(strlen(wanIfName) >= 4U) {
                    sprintf(cmdtmp,"nexthop dev %s weight %s ", wanIfName, weight);
                    strcat(cmdstr, cmdtmp);
                }
            } else if (strncmp("DHCP", wanConnType, 4) == 0) {
                /* get dhcp gateway */
                getWanDhcpDefaultRoute(i, wanDhcpGw);
                /* if the wanDhcpGw is wrong, do nothing */
                if(strlen(wanDhcpGw) >= 7U) {
                    sprintf(cmdtmp,"nexthop via %s dev %s weight %s ", wanDhcpGw, wanIfName, weight);
                    strcat(cmdstr, cmdtmp);
                }
            } else if (strncmp("STATIC", wanConnType, 6) == 0) {
                /* using user config gateway */
                wanGw = nvram_bufget(RT2860_NVRAM, add_wanx("Gw", i, buf));
                sprintf(cmdtmp,"nexthop via %s dev %s weight %s ", wanGw, wanIfName, weight);
                strcat(cmdstr, cmdtmp);
            } else {
                /* invalid ConnMode */
                DBGPRINT("invalid ConnMode[%s]\n", wanConnType);
            }
#endif
#if defined(NAT_LOAD_BALANCE)
	    cnt++;
	    port[i - 1] = 1;
#endif
        }
    }
    /* there is not any wan port in use */
    if (strstr(cmdstr, "nexthop") == NULL) {
        doSystem("ip route del default ");
#if ((EASY_TASK == FYES) && (HOTEL_PNP == FYES))
	addGwForETask();
#endif
#if(UTT_DOMAINACCESS ==FYES)
    isWanLinkStatus= 0;
#endif
    }
    else
    {
#if(UTT_DOMAINACCESS ==FYES)
	isWanLinkStatus= 1;
#endif
#if ((EASY_TASK == FYES) && (HOTEL_PNP == FYES))
	delRtForETask();
#endif
#if defined(NAT_LOAD_BALANCE)
	update_nlb_rule(cnt, port);
#endif

	doSystem(cmdstr);
#if (UTT_IPAUTOREP ==FYES)
	SystemProfile *profSystem=(SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
	if (profSystem != NULL) {
	    if(profSystem->CheckIPEn == FUN_ENABLE){
		IPClashAutoRepair();
	    }
	}
#endif
    }
#if (HOTEL_PNP ==FYES || EASY_TASK == FYES || UTT_DOMAINACCESS ==FYES)
     editDNSRedirectRule();
#endif
    /* call system to execute */
    SR_PRINTF("%s-cmdstr:%s\n", __FUNCTION__, cmdstr);
    doSystem("echo 1 >/proc/sys/net/ipv4/route/flush ");  /* flush cache */
}

/* 
 * delOneWanDefaultRoute delete a default route of srcip for NO:$wanIndex wan port
 */
extern void delOneWanDefaultRoute(int wanIndex)
{
    /* different wan port using different route table */
    doSystem("ip route flush table %d", SRCIP_ROUTE_TABLE_BASE + wanIndex);
}

extern void addOneWanDefaultRoute(int wanIndex)
{
/* 
 * addOneWanDefaultRoute add a default route of srcip for NO:$wanIndex wan port
 */
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
    enum em_eth_conn_mode wanConnType;
    char *wanIfName;
    char cmdstr[RT_CMD_LEN], gw_addr[RT_IP_LEN];
    char cmdstr1[RT_CMD_LEN];
    char ip_addr[RT_IP_LEN], nm_addr[RT_IP_LEN];
    struct in_addr gw, wanSeg;
    int table = 0;

#if 0
    char buf[MAX_UTT_CONF_NAME_LEN], cmdstr[128];
    char *wanConnType;
    int table = 0;
    char *wanIfName, *wanGw;
    char if_addr[16];
    /* initiate all the local variables */
    memset(cmdstr, 0x00, sizeof(cmdstr));
    memset(buf, 0x00, sizeof(buf));
    memset(if_addr, 0x00, sizeof(if_addr));
#endif
    memset(cmdstr, 0, RT_CMD_LEN);
    memset(gw_addr, 0, RT_IP_LEN);
    memset(cmdstr1, 0, RT_CMD_LEN);
    memset(ip_addr, 0, RT_IP_LEN);
    memset(nm_addr, 0, RT_IP_LEN);

    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, wanIndex);
    if ((profIF == NULL) || (profIF->head.active == 0)) { /* ÏßÂ·±»É¾³ý */
	SR_PRINTF("%s: interface profile get error.\n", __FUNCTION__);
	return;
    }
    /* get wan connect mode */
#if 0
    wanConnType = nvram_bufget(RT2860_NVRAM, add_wanx("ConnMode", wanIndex, buf));
#endif
    wanConnType = profIF->ether.connMode;
    table = SRCIP_ROUTE_TABLE_BASE + wanIndex;
    /* get the wan port name. */
    wanIfName = getWanIfNamePPP(wanIndex);

#if 0
    if(strncmp("PPPOE", wanConnType, 5) == 0) {
        /* PPPOE Mode has no gateway ip */
        sprintf(cmdstr, "ip route replace default table %d dev %s", table, wanIfName);
    } else if(strncmp("STATIC", wanConnType, 6) == 0) {
        /* Use the gateway user config while STATIC Mode */
        wanGw = nvram_bufget(RT2860_NVRAM, add_wanx("Gw", wanIndex, buf));
        sprintf(cmdstr, "ip route replace default table %d via %s dev %s", table, wanGw, wanIfName);
    } else if(strncmp("DHCP", wanConnType, 4) == 0) {
        /* get the Dynamic gateway while DHCP connnect mode */
        getWanDhcpDefaultRoute(wanIndex, if_addr);
        sprintf(cmdstr, "ip route replace default table %d via %s dev %s", table, if_addr, wanIfName);
    } else {
        /* invalid ConnMode */
        DBGPRINT("invalid ConnMode[%s]\n", wanConnType);
    }
#endif
#if 1
    if(wanConnType == ETH_CONN_PPPOE) {
        /* PPPOE Mode has no gateway ip */
        sprintf(cmdstr, "ip route replace default table %d dev %s", table, wanIfName);
    } else if(wanConnType == ETH_CONN_STATIC) {
        /* Use the gateway user config while STATIC Mode */
#if 0
        wanGw = nvram_bufget(RT2860_NVRAM, add_wanx("Gw", wanIndex, buf));
#endif
	gw.s_addr = profIF->ether.sta.gw;
        sprintf(cmdstr, "ip route replace default table %d via %s dev %s", table, inet_ntoa(gw), wanIfName);
	/*
	 * ±êÇ©Â·ÓÉ£¬Èç¹ûÊÇwan¿ÚÍø¶Î²»·¢ËÍµ½Íø¹Ø£¬Ö±½Ó·¢ËÍµ½¶ÔÓ¦wan¿Ú, mantis 8055
	 */
	wanSeg.s_addr = profIF->ether.sta.ip & profIF->ether.sta.nm;
	sprintf(cmdstr1, "ip route replace %s/%d table %d dev %s", inet_ntoa(wanSeg), headbits(ntohl(profIF->ether.sta.nm)), table,  wanIfName);
    } else if(wanConnType == ETH_CONN_DHCP) {
        /* get the Dynamic gateway while DHCP connnect mode */
        getWanDhcpDefaultRoute(wanIndex, gw_addr);
        sprintf(cmdstr, "ip route replace default table %d via %s dev %s", table, gw_addr, wanIfName);
	/*
	 * ±êÇ©Â·ÓÉ£¬Èç¹ûÊÇwan¿ÚÍø¶Î²»·¢ËÍµ½Íø¹Ø£¬Ö±½Ó·¢ËÍµ½¶ÔÓ¦wan¿Ú, mantis 8055
	 */
	/* »ñÈ¡ipºÍnetmaskÕýÈ· */
	if ((getIfIp(wanIfName, ip_addr) == 0) && (getIfNetmask(wanIfName, nm_addr) == 0)) {
	    wanSeg.s_addr = inet_addr(ip_addr) & inet_addr(nm_addr);
	    sprintf(cmdstr1, "ip route replace %s/%d table %d dev %s", inet_ntoa(wanSeg), 
		    headbits(ntohl(inet_addr(nm_addr))), table, wanIfName);
	}
    } else {
        /* invalid ConnMode */
        SR_PRINTF("invalid ConnMode[%d]\n", wanConnType);
    }

    /* call system */
    doSystem(cmdstr);
    if (strlen(cmdstr1) != 0) {
        doSystem(cmdstr1);
        SR_PRINTF("%s-cmdstr1:%s\n", __FUNCTION__, cmdstr1);
    }
    SR_PRINTF("%s-cmdstr:%s\n", __FUNCTION__, cmdstr);
#endif
    return;
}

#if 0
/* flush the ISP route in table ISP_ROUTE_TABLE_BASE 
 * $wanIndex points which wan's route should be flushed
 */
extern void ISPRouteTableFlush(int wanIndex)
{
    int table = 0;
    /* different wan port using different route table */
    table = ISP_ROUTE_TABLE_BASE + wanIndex;
    doSystem("ip route flush table %d", table);
    return;
}

/* add ISP route for NO:wanIndex wan port*/
extern void ISPRouteTableAdd(int wanIndex)
{
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
#if(UTTPDB == FYES)
    MibProfileType mibTypePDB = MIB_PROF_PDB_GLOBAL;
    PdbProfile *profPDB = NULL;
    int pdbLen = 0;
#endif
    char *wanIfName = NULL, *pdb = NULL;
    int isPppoe = 0;
    int table = 0, i = 0;
    long netmask;
    enum em_eth_conn_mode wanConnType;
    struct in_addr addr;

#if(UTTPDB == FYES)
    char pdbname[UTT_INST_NAME_LEN + 1];
#endif
    char gw_addr[RT_IP_LEN];
    char ipbuf[RT_IP_LEN];
    char cmdstr[RT_CMD_LEN];

#if(UTTPDB == FYES)
    memset(pdbname, 0, sizeof(pdbname));
#endif
    memset(gw_addr, 0, RT_IP_LEN);
    memset(ipbuf, 0, RT_IP_LEN);
#if 0
    char buf[MAX_UTT_CONF_NAME_LEN], cmdstr[128];
    /* pdbname point to the ISP route name in flash.
     * pdb point to the dest ip addr  database of ISP route info
     */
    char tmpstr[1] = {'\0'};
    char *pdbname=NULL, *pdb=tmpstr;
    long netmask=0L;
    /* the length of the ISP policy file */
    size_t pdbLen = 0U;
    struct in_addr in={0};
    char if_addr[16];
    char ipbuf[0x10];
    char *wanIfName=NULL, *wanGw=NULL, *wanConnType=NULL;
    int isPppoe = 0;
    int table = 0;

    /* initiate all the local variables */
    memset(cmdstr, 0x00, sizeof(cmdstr));
    /* used to storage the nvram name */
    memset(buf, 0x00, sizeof(buf));
    memset(if_addr, 0x00, sizeof(if_addr));
    memset(ipbuf, 0x00, sizeof(ipbuf));
#endif

    /* get  wanxISP name which configed by user */
#if 0
    pdbname = nvram_bufget(RT2860_NVRAM, add_wanx("ISP", wanIndex, buf));
#endif
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, wanIndex);

    if ((profIF == NULL) || (profIF->head.active == FALSE)) { /* ÏßÂ·±»É¾³ý */
	return;
    }

    SR_PRINTF("update the ISP[%d] route\n", profIF->isp);
    if(profIF->isp != 0U) { /* ÏßÂ·ÅäÖÃÁËisp²ßÂÔ */
#if(UTTPDB == FYES)
	/* »ñµÃpdbnameÊµÀýÃû */
        snprintf(pdbname, UTT_INST_NAME_LEN, "%d", profIF->isp);
	
	profPDB = (PdbProfile *)ProfGetInstPointByName(mibTypePDB, (const char *)pdbname);

	if ((profPDB == NULL) || (profPDB->head.active == FALSE) || 
		(profPDB->type != RoutePdb)) { /* »ñÈ¡¾²Ì¬Â·ÓÉpdb²ßÂÔÊ§°Ü */
	    SR_PRINTF("%s:get pdb profile failed\n", __FUNCTION__);
	    return;
	}
#endif

#if 0
        pdb = nvram_bufget(RT2860_NVRAM, pdbname);
        pdbLen = strlen(pdb);
#endif
    
        /* get the wan Name and connect mode */
        wanIfName = getWanIfNamePPP(wanIndex);
#if 0
        wanConnType = nvram_bufget(RT2860_NVRAM, add_wanx("ConnMode", wanIndex, buf));
#endif
	wanConnType = profIF->ether.connMode;

        if(wanConnType == ETH_CONN_PPPOE) {
            /* set the pppoe mode flag */
            isPppoe = 1;
        } else if(wanConnType == ETH_CONN_STATIC) {
	    struct in_addr gw;
	    gw.s_addr = profIF->ether.sta.gw;
	    strcpy(gw_addr, inet_ntoa(gw));
	    SR_PRINTF("%s: static gw_addr=%s\n", __FUNCTION__, gw_addr);
        } else if(wanConnType == ETH_CONN_DHCP) {
            /* get the dynamic gateway info while DHCP mode */
            getWanDhcpDefaultRoute(wanIndex, gw_addr);
	    SR_PRINTF("%s: dhcp gw_addr=%s\n", __FUNCTION__, gw_addr);
        } else {
            SR_PRINTF("invalid ConnMode[%d]\n", wanConnType);
        }

        table = ISP_ROUTE_TABLE_BASE + wanIndex;
    
#if(UTTPDB == FYES)
        /* add all the ISP dest network route */
	for (i = 0; i < MAX_PDB_LINE_COUNT; i++) {
	    pdb = profPDB->line[i];
	    pdbLen = strlen(pdb);
#if 0
	    SR_PRINTF("%s: pdb=%s\n", __FUNCTION__, pdb);
#endif

	    while (pdbLen >= RT_PDB_LEN) {
		memset(ipbuf, 0, RT_IP_LEN);
		memcpy(ipbuf, pdb, RT_PDB_IP_LEN); /* ÃèÊöipµØÖ·µÄpdb²ßÂÔ */

#if 0
		SR_PRINTF("%s: ipbuf_ip=%s\n", __FUNCTION__, ipbuf);
#endif

		/* ½«ipµØÖ·µÄ16½øÖÆ×Ö·û´®±íÊ¾ÐÎÊ½£¬×ª»»Îªin_addr */
		addr.s_addr = htonl((Uint32)strtoll(ipbuf, (char **)NULL, 16));

#if 0
		SR_PRINTF("%s: addr_int=%x, addr_str=%s\n", __FUNCTION__, addr.s_addr, inet_ntoa(addr));
#endif

		/* »ñµÃÚÂë */
		memset(ipbuf, 0, RT_IP_LEN);
		memcpy(ipbuf, &(pdb[RT_PDB_IP_LEN]), RT_PDB_MASK_LEN);
		netmask = strtol(ipbuf, (char **)NULL, 16);

#if 0
		SR_PRINTF("%s: ipbuf_nm=%s\n", __FUNCTION__, ipbuf);
#endif

		memset(cmdstr, 0, RT_CMD_LEN);
		if (isPppoe == 1) {
		    /* pppoe mode has no gateway */
		    sprintf(cmdstr, "ip route replace %s/%ld dev %s table %d", inet_ntoa(addr), netmask, wanIfName, table);
		    doSystem(cmdstr);
		    SR_PRINTF("%s-cmdstr:%s\n", __FUNCTION__, cmdstr);
		} else {
		    sprintf(cmdstr, "ip route replace %s/%ld via %s dev %s table %d", inet_ntoa(addr), netmask, gw_addr, wanIfName, table);
		    doSystem(cmdstr);
		    SR_PRINTF("%s-cmdstr:%s\n", __FUNCTION__, cmdstr);
		}
		pdb = &pdb[RT_PDB_LEN];
		pdbLen -= RT_PDB_LEN;
	    }
	}
#endif
#if 0
        while(pdbLen >= 10U) {
            memset(ipbuf, 0, 0x10);
            memcpy(ipbuf, pdb, 8);
            ipbuf[8]='\0';
            /* in order to get the using lan ip
             *      instead of the user's configure
             */
            in.s_addr = strtoll(ipbuf, (char **)NULL, 16);
            in.s_addr = ntohl((unsigned int)in.s_addr);
#if 0
            memcpy(&(in.s_addr), pdb, 4);
            memcpy(&netmask, pdb + 4, 1);
#endif
            memset(ipbuf, 0, 0x10);
            memcpy(ipbuf, &pdb[8], 2);
            /* netmaks is CIDR mode */
            netmask = strtol(ipbuf, (char **)NULL, 16);
            if(isPppoe == 1) {
                /* pppoe mode has no gateway */
                sprintf(cmdstr, "ip route replace %s/%ld dev %s table %d", inet_ntoa(in), netmask, wanIfName, table);
                doSystem(cmdstr);
            } else {
                sprintf(cmdstr, "ip route replace %s/%ld via %s dev %s table %d", inet_ntoa(in), netmask, wanGw, wanIfName, table);
                doSystem(cmdstr);
            }
            /* change the offset */
            pdb = &pdb[10];
            pdbLen -= 10U;
        }
#endif
    }
    doSystem("echo 1 >/proc/sys/net/ipv4/route/flush ");
    return;
}
#else
/********************************************************************
* º¯Êý£º ISPModeToTable
* ¹¦ÄÜ£º ¸ù¾ÝÔËÓªÉÌºÍÖ÷±¸Ä£Ê½È¡µÃtableºÅ
* ´´½¨£º 2012-11-06
* ²ÎÊý£º isp	     -     ÔËÓªÉÌ
*	 mode        -     Ö÷±¸Ä£Ê½
* ·µ»Ø£º ret         -     tableºÅ
* Êä³ö£º ÂÔ
* ×÷Õß£º Jianqing.Sun 
********************************************************************/
static int ISPModeToTable(int isp, int mode)
{
    int ret = 0;
    switch(isp) 
    {
	case ISP_TELECOM:
	    ret = ISP_ROUTE_TABLE_BASE + 20 * mode + 1;
	    break;
	case ISP_UNICOM:
	    ret = ISP_ROUTE_TABLE_BASE + 20 * mode + 2;
	    break;
	case ISP_MOBILE:
	    ret = ISP_ROUTE_TABLE_BASE + 20 * mode + 3;
	    break;
	default:/*default*/
	    break;
    }      
    return ret;
}

/********************************************************************
* º¯Êý£º getIspWeightCmd
* ¹¦ÄÜ£º ¸ù¾ÝtableºÅÈ¡µÃËùÐèÏßÂ·ÐÅÏ¢
* ´´½¨£º 2012-11-06
* ²ÎÊý£º table	      -     tableºÅ
* ·µ»Ø£º cmdtmpstr    -     ÏßÂ·ÐÅÏ¢
* Êä³ö£º ÂÔ
* ×÷Õß£º Jianqing.Sun 
********************************************************************/
static char *getIspWeightCmd(int table)
{
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
    char *wanIfName = NULL;
    enum em_eth_conn_mode wanConnType;
    static char cmdtmpstr[256];
    char cmdtmp[128];
    char gw_addr[RT_IP_LEN];
    int wanCount = 0, i = 0, weight = 0;
    int isPppoe = 0, rate = 0;

    memset(cmdtmpstr, 0, sizeof(cmdtmpstr));
    memset(cmdtmp, 0, sizeof(cmdtmp));
    memset(gw_addr, 0, RT_IP_LEN);

    wanCount = getWanSlotCount();
    for(i = 1; i <= wanCount; i++) 
    {/*È¡µÃÔËÓªÉÌ±íµÄËùÓÐÒÑÁ¬½ÓÏßÂ·*/
	SWORD_PRINTF("%s-%d, i = %d\n", __func__, __LINE__, i);
	if(_getWanLinkState(i) == 1)
	{
	    SWORD_PRINTF("%s-%d, i = %d\n", __func__, __LINE__, i);
	    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, i);

	    if ((profIF != NULL) || (profIF->head.active == TRUE)) 
	    { /* ÏßÂ·´æÔÚ */

		isPppoe = 0;/*Ã¿´Î¿ªÊ¼Ê±¶¼³õÊ¼»¯*/
		SWORD_PRINTF("%s-%d, update the ISP[%d] route, table = %d, profIF->lineBalMode = %d\n", __func__, __LINE__, profIF->isp, table, profIF->lineBalMode);
		if(((profIF->isp != 0U) && (ISPModeToTable(profIF->isp, profIF->lineBalMode)) == table) 
		    || ((profIF->isp1 != 0U) && (ISPModeToTable(profIF->isp1, profIF->lineBalMode)) == table)
		    || ((profIF->isp2 != 0U) && (ISPModeToTable(profIF->isp2, profIF->lineBalMode)) == table))
		{ /* ÏßÂ·ÅäÖÃÁËisp²ßÂÔ */
		    /* get the wan Name and connect mode */
		    wanIfName = getWanIfNamePPP(i);
		    wanConnType = profIF->ether.connMode;

		    if(wanConnType == ETH_CONN_PPPOE) 
		    {
			/* set the pppoe mode flag */
			isPppoe = 1;
		    } 
		    else if(wanConnType == ETH_CONN_STATIC) 
		    {
			struct in_addr gw;
			gw.s_addr = profIF->ether.sta.gw;
			strcpy(gw_addr, inet_ntoa(gw));
			SWORD_PRINTF("%s: static gw_addr=%s\n", __FUNCTION__, gw_addr);
		    } 
		    else if(wanConnType == ETH_CONN_DHCP) 
		    {
			/* get the dynamic gateway info while DHCP mode */
			getWanDhcpDefaultRoute(i, gw_addr);
			SWORD_PRINTF("%s: dhcp gw_addr=%s\n", __FUNCTION__, gw_addr);
		    } 
		    else 
		    {
			SWORD_PRINTF("invalid ConnMode[%d]\n", wanConnType);
		    }
		    SWORD_PRINTF("%s-%d: wanIfName[%s],wanConnType[%d]\n", __func__, __LINE__, wanIfName, wanConnType);

		    /* make sure weight is in [1 ... 256 ] */
		    /* ¸ºÔØ¾ùºâ°´ÏßÂ·´ø¿í¾ùºâ */
		    rate = profIF->rxRate;
		    /* 
		     * È¨ÖØÈ¡ÖµÎªÏßÂ·´ø¿í³ý512Kbit 
		     * ÕâÑùÈ¡·¨Óöµ½Ð¡´ø¿í¾Í²»×¼È·ÁË,bug?
		     */
		    /* mantis 8513: 512ÐÞ¸ÄÎª500 */
		    weight = rate/(500);
		    if (weight < 1) 
		    {
			weight = 1;
		    } 
		    else if (weight > 256) 
		    {
			weight = 256;
		    }
		    if (isPppoe == 1) 
		    {
			sprintf(cmdtmp, "nexthop dev %s weight %d ", wanIfName, weight);
		    }
		    else
		    {
			sprintf(cmdtmp, "nexthop via %s dev %s weight %d ", gw_addr, wanIfName, weight);
		    }
		    strcat(cmdtmpstr, cmdtmp);
		}
	    }
	}
    }
    return cmdtmpstr;
}
/********************************************************************
* º¯Êý£º IspRoute
* ¹¦ÄÜ£º ¸ù¾ÝtableºÅºÍÔËÓªÉÌÐÞ¸Ä¶ÔÓ¦±í
* ´´½¨£º 2012-11-06
* ²ÎÊý£º table	      -     tableºÅ
*	 isp          -     ÔËÓªÉÌ
* ·µ»Ø£º ÂÔ
* Êä³ö£º ÂÔ
* ×÷Õß£º Jianqing.Sun 
********************************************************************/
static void IspRoute(int table, int isp)
{
#if(UTTPDB == FYES)
    MibProfileType mibTypePDB = MIB_PROF_PDB_GLOBAL;
    PdbProfile *profPDB = NULL;
    int pdbLen = 0;
#endif
    int i = 0;
    char cmdstr[300];
    char *pdb = NULL;
    long netmask = 0;
    struct in_addr addr;
#if(UTTPDB == FYES)
    char pdbname[UTT_INST_NAME_LEN + 1];
#endif
    char ipbuf[RT_IP_LEN];

#if(UTTPDB == FYES)
    memset(pdbname, 0, sizeof(pdbname));
#endif
    memset(ipbuf, 0, sizeof(RT_IP_LEN));

    doSystem("ip route flush table %d", table);
#if(UTTPDB == FYES)
    /* »ñµÃpdbnameÊµÀýÃû */
    snprintf(pdbname, UTT_INST_NAME_LEN, "%d", isp);
    profPDB = (PdbProfile *)ProfGetInstPointByName(mibTypePDB, (const char *)pdbname);
    if ((profPDB != NULL) && (profPDB->head.active == TRUE) && (profPDB->type == RoutePdb)) 
    { /* »ñÈ¡¾²Ì¬Â·ÓÉpdb²ßÂÔ³É¹¦*/

	/* add all the ISP dest network route */
	for (i = 0; i < MAX_PDB_LINE_COUNT; i++) 
	{
	    pdb = profPDB->line[i];
	    pdbLen = strlen(pdb);
	    SWORD_PRINTF("%s-%d: pdb=%s\n", __func__, __LINE__, pdb);

	    while (pdbLen >= RT_PDB_LEN) 
	    {
		memset(ipbuf, 0, RT_IP_LEN);
		memcpy(ipbuf, pdb, RT_PDB_IP_LEN); /* ÃèÊöipµØÖ·µÄpdb²ßÂÔ */

		SWORD_PRINTF("%s-%d: ipbuf_ip=%s\n", __func__, __LINE__, ipbuf);

		/* ½«ipµØÖ·µÄ16½øÖÆ×Ö·û´®±íÊ¾ÐÎÊ½£¬×ª»»Îªin_addr */
		addr.s_addr = htonl((Uint32)strtoll(ipbuf, (char **)NULL, 16));

		SWORD_PRINTF("%s-%d: addr_int=%x, addr_str=%s\n", __func__, __LINE__, addr.s_addr, inet_ntoa(addr));

		/* »ñµÃÑÚÂë */
		memset(ipbuf, 0, RT_IP_LEN);
		memcpy(ipbuf, &(pdb[RT_PDB_IP_LEN]), RT_PDB_MASK_LEN);
		netmask = strtol(ipbuf, (char **)NULL, 16);

		SWORD_PRINTF("%s-%d: ipbuf_nm=%s\n", __func__, __LINE__, ipbuf);

		memset(cmdstr, 0, sizeof(cmdstr));
		sprintf(cmdstr, "ip route replace %s/%ld equalize table %d ", inet_ntoa(addr), netmask, table);
		SWORD_PRINTF("%s-%d:cmdstr:%s\n", __func__, __LINE__, cmdstr);

		strcat(cmdstr, getIspWeightCmd(table));
		SWORD_PRINTF("%s-%d, cmdstr:%s\n", __func__, __LINE__, cmdstr);
		doSystem(cmdstr);
		pdb = &pdb[RT_PDB_LEN];
		pdbLen -= RT_PDB_LEN;
	    }
	}
    }
#endif
    return;
}
/********************************************************************
* º¯Êý£º ISPRouteTable
* ¹¦ÄÜ£º ¸ù¾ÝWAN¿ÚË÷ÒýÐÞ¸ÄÔËÓªÉÌ¶ÔÓ¦±í
* ´´½¨£º 2012-11-06
* ²ÎÊý£º wanIndex	-     WAN¿ÚË÷Òý
* ·µ»Ø£º ÂÔ
* Êä³ö£º ÂÔ
* ×÷Õß£º Jianqing.Sun 
********************************************************************/
extern void ISPRouteTable(int wanIndex)
{
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;

    int table = 0,table1 = 0,table2 = 0;

    /* get  wanxISP name which configed by user */
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, wanIndex);

    if ((profIF != NULL) && (profIF->head.active == TRUE)) 
    { /* ÏßÂ·´æÔÚ */
	SWORD_PRINTF("%s-%d:update the ISP[%d] route\n", __func__, __LINE__, profIF->isp);
	if(profIF->isp != 0U) 
	{ /* ÏßÂ·ÅäÖÃÁËisp²ßÂÔ */
//            printf("%s--%d\n",__func__,__LINE__);
	    table = ISPModeToTable(profIF->isp, profIF->lineBalMode);
	    IspRoute(table, profIF->isp);
	}
	if(profIF->isp1 != 0U) 
	{ /* ÏßÂ·ÅäÖÃÁËisp²ßÂÔ */
//            printf("%s--%d\n",__func__,__LINE__);
	    table1 = ISPModeToTable(profIF->isp1, profIF->lineBalMode);
	    IspRoute(table1, profIF->isp1);
	}
	if(profIF->isp2 != 0U) 
	{ /* ÏßÂ·ÅäÖÃÁËisp²ßÂÔ */
//            printf("%s--%d\n",__func__,__LINE__);
	    table2 = ISPModeToTable(profIF->isp2, profIF->lineBalMode);
	    IspRoute(table2, profIF->isp2);
	}

    }
    return;
}
/********************************************************************
* º¯Êý£º ISPRouteOldModifyTable
* ¹¦ÄÜ£º ÏßÂ·ÔËÓªÉÌ¸Ä±äÊ±ÐÞ¸ÄÔ­À´µÄtable
* ´´½¨£º 2012-11-06
* ²ÎÊý£º prof        -     ¾Éprofile
* ·µ»Ø£º ÂÔ
* Êä³ö£º ÂÔ
* ×÷Õß£º Jianqing.Sun 
********************************************************************/
extern void ISPRouteOldModifyTable(InterfaceProfile *profIF,int ispNo)
{
    int table = 0,table1 = 0,table2 = 0;
    if(ispNo == 0){
//            printf("%s--%d\n",__func__,__LINE__);
	table = ISPModeToTable(profIF->isp, profIF->lineBalMode);
	IspRoute(table, profIF->isp);
    }else if(ispNo == 1){
//            printf("%s--%d\n",__func__,__LINE__);
	table1 = ISPModeToTable(profIF->isp1, profIF->lineBalMode);
	IspRoute(table1, profIF->isp1);
    }else if(ispNo == 2){
//            printf("%s--%d\n",__func__,__LINE__);
	table2 = ISPModeToTable(profIF->isp2, profIF->lineBalMode);
	IspRoute(table2, profIF->isp2);
    }
    return;
}

/********************************************************************
* º¯Êý£º ISPModifyOldTable
* ¹¦ÄÜ£º Ö÷±¸±ä»¯Ê±¶ÔËùÔÚÔ­À´ËùÔÚÏßÂ·¸ü¸Ä
* ´´½¨£º 2012-11-06
* ²ÎÊý£º wanIndex    -     WAN¿ÚË÷Òý
*	 flag:
*	   -1        -     ÓÉÖ÷±ä±¸
*	    1        -     ÓÉ±¸±äÖ÷
* ·µ»Ø£º ÂÔ
* Êä³ö£º ÂÔ
* ×÷Õß£º Jianqing.Sun 
********************************************************************/
extern void ISPModifyOldTable(int wanIndex, int flag)
{
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
    int table = 0,table1 = 0,table2 = 0;

    /* get  wanxISP name which configed by user */
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, wanIndex);

    if ((profIF != NULL) && (profIF->head.active == TRUE)) 
    { /* ÏßÂ·´æÔÚ */
	SWORD_PRINTF("%s-%d:update the ISP[%d] route\n", __func__, __LINE__, profIF->isp);
	if(profIF->isp != 0U) 
	{ /* ÏßÂ·ÅäÖÃÁËisp²ßÂÔ */
	    table = ISPModeToTable(profIF->isp, profIF->lineBalMode) + flag * 20;
	    IspRoute(table, profIF->isp);
	}
	if(profIF->isp1 != 0U) 
	{ /* ÏßÂ·ÅäÖÃÁËisp²ßÂÔ */
	    table1 = ISPModeToTable(profIF->isp1, profIF->lineBalMode) + flag * 20;
	    IspRoute(table1, profIF->isp1);
	}
	if(profIF->isp2 != 0U) 
	{ /* ÏßÂ·ÅäÖÃÁËisp²ßÂÔ */
	    table2 = ISPModeToTable(profIF->isp2, profIF->lineBalMode) + flag * 20;
	    IspRoute(table2, profIF->isp2);
	}

    }
    return;
}
/********************************************************************
* º¯Êý£º ISPRouteTableFlush
* ¹¦ÄÜ£º ¸ù¾ÝWAN¿ÚË÷ÒýÐÞ¸ÄÔËÓªÉÌ¶ÔÓ¦±í
* ´´½¨£º 2012-11-06
* ²ÎÊý£º wanIndex	-     WAN¿ÚË÷Òý
* ·µ»Ø£º ÂÔ
* Êä³ö£º ÂÔ
* ×÷Õß£º Jianqing.Sun 
********************************************************************/
extern void ISPRouteTableFlush(int wanIndex)
{
    ISPRouteTable(wanIndex);
    return;
}

/********************************************************************
* º¯Êý£º ISPRouteTableFlush
* ¹¦ÄÜ£º ¸ù¾ÝWAN¿ÚË÷ÒýÐÞ¸ÄÔËÓªÉÌ¶ÔÓ¦±í
* ´´½¨£º 2012-11-06
* ²ÎÊý£º wanIndex	-     WAN¿ÚË÷Òý
* ·µ»Ø£º ÂÔ
* Êä³ö£º ÂÔ
* ×÷Õß£º Jianqing.Sun 
********************************************************************/
extern void ISPRouteTableAdd(int wanIndex)
{
    ISPRouteTable(wanIndex);
    doSystem("echo 1 >/proc/sys/net/ipv4/route/flush ");
    return;
}
#endif

#if (DDNS == FYES)
/* delete dns route */
void delete_dns_route(int wanNo)
{
#if(UTT_NV_1800N != FYES)
    /* ddns TODO edit dhyu */
    MibProfileType mibType = MIB_PROF_DDNS;
    DDNSProfile *prof = NULL;

    char wanname[8];
    memset(wanname, 0x00, sizeof(wanname));
    /* delete the route for dns address */
    doSystem("delete_dnsroute.sh %d", wanNo);
    /* delete the route for ddns address */

    prof = (DDNSProfile *)ProfGetInstPointByIndex(mibType, 0);
    if (prof == NULL) {
	SR_PRINTF("%s: ddnsProfile get error\n", __FUNCTION__);
	return;
    }

    sprintf(wanname, "WAN%d", wanNo);
    if(strcmp(wanname, prof->ddnsIf) == 0 ) {
        doSystem("ip route flush table 100");
    }
#endif
}
#endif

/* genarate dns config file /etc/resolve.con, and add dns route */
void add_dns_route(int wanNo)
{
    /* add dns to /etc/resolv.conf */
#if 0
    char *connMode=NULL, dns1[128], dns2[128], filename[32]; 
    char wanGw[32], wanDhcpGw[24], *wanIfName=NULL, buf[64], cmdtmp[128];
    /* clear all local vairables */
    memset(dns1, 0x00, sizeof(dns1));
    memset(dns2, 0x00, sizeof(dns2));
    memset(filename, 0x00, sizeof(filename));
    /* buf, and cmdtmp is tmporary variables */
    memset(buf, 0x00, sizeof(buf));
    memset(cmdtmp, 0x00, sizeof(cmdtmp));
    memset(wanGw, 0x00, sizeof(wanGw));
    memset(wanDhcpGw, 0x00, sizeof(wanDhcpGw));
#endif
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
    enum em_eth_conn_mode connMode;
    char *wanIfName = NULL;

    char wanDhcpGw[RT_IP_LEN];
    char wanGw[RT_TMP_LEN];
    char filename[RT_TMP_LEN];
    char dns1[RT_FILE_LINE_LEN + 1];
    char dns2[RT_FILE_LINE_LEN + 1];
    char cmdtmp[RT_CMD_LEN];

    memset(wanGw, 0, RT_TMP_LEN);
    memset(wanDhcpGw, 0, RT_IP_LEN);
    memset(filename, 0, RT_TMP_LEN);
    memset(dns1, 0, (RT_FILE_LINE_LEN + 1));
    memset(dns2, 0, (RT_FILE_LINE_LEN + 1));
    memset(cmdtmp, 0, RT_CMD_LEN);

    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, wanNo);
    if ((profIF == NULL) || (profIF->head.active == FALSE)) { /* ÏßÂ·±»É¾³ý */
	return;
    }

    /* get connect mode */
#if 0
    connMode = nvram_bufget(RT2860_NVRAM, add_wanx("ConnMode", wanNo, buf));
#endif
    connMode = profIF->ether.connMode;

    if (connMode == ETH_CONN_STATIC) {
        /* read the gateway from nvram while static mode */
	struct in_addr gw;
	gw.s_addr = profIF->ether.sta.gw;
#if 0
        sprintf(wanGw, "via %s", nvram_bufget(RT2860_NVRAM, add_wanx("Gw", wanNo, buf)));
#endif
	sprintf(wanGw, "via %s", inet_ntoa(gw));
    }else if (connMode == ETH_CONN_DHCP) {
        /* get the dynamic gateway*/
        getWanDhcpDefaultRoute(wanNo, wanDhcpGw);
        sprintf(wanGw, "via %s", wanDhcpGw);
    }else {
        /* pppoe not gw */
    }
    /* read dns1 from the file */
    sprintf(filename, "/var/run/wan%d.dns", wanNo);
    read_file_line(filename, dns1, 1);
    memmove(dns1, &dns1[11], 16);
    /* read dns2 from the file */
    read_file_line(filename, dns2, 2);
    memmove(dns2, &dns2[11], 16);
    /* get wan interface name */
    wanIfName = getWanIfNamePPP(wanNo);
    /* add the dns to file /etc/resolv.conf */
    sprintf(cmdtmp, "config-dns.sh %d %s %s", wanNo, dns1, dns2);
    doSystem(cmdtmp);
    /* add the dns1 route meanwhile */
    if ((strcmp(dns1, "")!=0) && (strcmp(dns1, "0.0.0.0") != 0)) {
	memset(cmdtmp, 0, RT_CMD_LEN);
        sprintf(cmdtmp, "ip route replace %s/32 %s dev %s metric %d", dns1, wanGw, wanIfName, wanNo);
        doSystem(cmdtmp);
	SR_PRINTF("%s-cmdtmp:%s\n", __FUNCTION__, cmdtmp);
    }
    /* add the dns2 route meanwhile */
    if ((strcmp(dns2, "")!=0) && (strcmp(dns2, "0.0.0.0") != 0)) {
	memset(cmdtmp, 0, RT_CMD_LEN);
        sprintf(cmdtmp, "ip route replace %s/32 %s dev %s metric %d", dns2, wanGw, wanIfName, wanNo);
        doSystem(cmdtmp);
	SR_PRINTF("%s-cmdtmp: %s\n", __FUNCTION__, cmdtmp);
    }
#if (CAPWAP_CONFIG_AP == FYES)
    set_capwap_dns();
#endif
}


#if 0
/*
 * ÏßÂ·¼ì²â
 */
void LinkChkChg(int wanNo)
{
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
    enum em_eth_conn_mode wanConnType;

    Uint32 chkInt;
    struct in_addr chkIp;
    char wanDhcpGw[32], *wanIfName;
#if 0
    chkIp=nvram_bufget(RT2860_NVRAM,add_wanx(_CONF_MULTIPATH_DETECTIP, wanNo, buf));
    chkInt=nvram_bufget(RT2860_NVRAM,add_wanx(_CONF_MULTIPATH_DETECTINTERVAL, wanNo, buf));
    
    wanConnType = nvram_bufget(RT2860_NVRAM, add_wanx(_CONF_WAN_CONNECT_MODE, wanNo, buf));
#endif
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, wanNo);
    if ((profIF == NULL) || (profIF->head.active == FALSE)) { /* ÏßÂ·±»É¾³ý */
	return;
    }

    chkIp.s_addr = profIF->detectIp;
    chkInt = profIF->detectInterval;
    wanConnType = profIF->ether.connMode;
    wanIfName = getWanIfNamePPP(wanNo);
    SR_PRINTF("LinkChkChg: wanConnType[%d],wanIfName[%s],chkIp[%s]\n", wanConnType, wanIfName, inet_ntoa(chkIp));
    doSystem("ip route flush table  %d", WANCHK_ROUTE_TABLE_BASE+wanNo);
    if(chkInt != 0U){
        if(wanConnType == ETH_CONN_PPPOE) {
            doSystem("ip route replace table %d %s/32 dev %s",WANCHK_ROUTE_TABLE_BASE+wanNo, inet_ntoa(chkIp), wanIfName);
        } else if (wanConnType == ETH_CONN_DHCP) {
            getWanDhcpDefaultRoute(wanNo, wanDhcpGw);
            SR_PRINTF("wanDhcpGw[%s]\n", wanDhcpGw);
            doSystem("ip route replace table %d %s/32 via %s dev %s",WANCHK_ROUTE_TABLE_BASE+wanNo, inet_ntoa(chkIp), wanDhcpGw, wanIfName);
        } else if (wanConnType == ETH_CONN_STATIC) {
	    struct in_addr gw;
	    gw.s_addr = profIF->ether.sta.gw;
#if 0
            Gw = nvram_bufget(RT2860_NVRAM, add_wanx(_CONF_WAN_STATIC_GATEWAY, wanNo, buf));
#endif
            SR_PRINTF("LinkChkChg:Gw[%s]\n", inet_ntoa(gw));
            doSystem("ip route replace table %d %s/32 via %s dev %s",WANCHK_ROUTE_TABLE_BASE+wanNo, inet_ntoa(chkIp), inet_ntoa(gw), wanIfName);
        }
    }

    doSystem("kill -9 `cat /var/run/linkcheck%d.pid`", wanNo);
    doSystem("linkcheck.sh %d >/dev/null 2>&1 &", wanNo);
}
#endif

/*
 * ÏßÂ·×éºÏ£¬ÄÚ²¿IPµØÖ·¶Î×ßÒ»¸ö½Ó¿Ú
 * ÊäÈë²ÎÊý£º
 * wanIndex - ½Ó¿Ú
 * action - ²Ù×÷ÀàÐÍ£ºdel, add
 * ipS - ÆðÊ¼IPµØÖ·£¬Ö÷»ú×Ö½ÚÐò
 * ipE - ½áÊøIPµØÖ·£¬Ö÷»ú×Ö½ÚÐò
 */
extern void wanIpRuleFun(int wanIndex, int action, IPADDR ipS, IPADDR ipE)
{
#if 0
    char *ipstart, *ipend;
    struct in_addr ipStart, ipEnd;
#endif
    struct in_addr ipC;
#if 0
    unsigned int ipS, ipE;
#endif
    unsigned int i, j=0;
    unsigned int netmask = 0;
    unsigned int mask = 0;
    unsigned int tmp = 0;
    int havedec = 0;
    int table = 0;
    int priority = 0;
    int cycleCount = 0;
 
    priority = SRCIP_ROUTE_RULE_PRIO_BASE + SRCIP_ROUTE_RULE_INTERVAL + wanIndex;
#if 0
    ipstart = nvram_bufget(RT2860_NVRAM, add_wanx(_CONF_MULTIPATH_IPSTART, wanIndex, buf));
    ipend = nvram_bufget(RT2860_NVRAM, add_wanx(_CONF_MULTIPATH_IPEND, wanIndex, buf));
#endif
    SR_PRINTF("%s: ipS=%x, ipE=%x\n", __FUNCTION__, ipS, ipE);
    if((ipS != 0U) && (ipE != 0U)) {

#if 0
        inet_aton(ipstart, &ipStart);
        inet_aton(ipend, &ipEnd);
        ipS = ntohl(ipStart.s_addr);
        ipE = ntohl(ipEnd.s_addr);
#endif

        tmp = ipE;
        SR_PRINTF("%x %x\n", ipS, ipE);

        /*
	 * ÒòÎªip rule add fromÖ»ÄÜÌí¼ÓÑÚÂëÐÎÊ½£¬²»ÄÜÌí¼ÓµØÖ··¶Î§£¬Èçx.x.x.x-x.x.x.x
	 * ËùÒÔÐèÒª½«x.x.x.x-x.x.x.x×ª»¯ÎªÑÚÂëÐÎÊ½ x.x.x.x/x
         * °ÑipµØÖ·¶Î»®·Ö³ÉÑÚÂë¸ñÊ½
         */
        while(tmp >= ipS) {

            cycleCount++;
            if(cycleCount > 100) {
                break;
            }

            havedec = 0;

            for(i = 0; i < 32; i++) {
                if((tmp & (1 << i)) != 0) {
                    j = i;
                    i = 32;
                }
            }
            SR_PRINTF("ff1 %x\n", j);
            
            for(i = j; i < 32; i++) {
                mask ++;
                netmask += 1 << i;
            }

            SR_PRINTF("ff2 %x\n",netmask);

            while(1) {
                if((((tmp + 1) & netmask) == (tmp & netmask))
                        || (((ipS - 1) & netmask) == (tmp & netmask))) {
                    mask ++;
                    j -= 1;
                    if(j >= 0)
                    netmask += (1 << (j));
                    if(havedec)
                        break;
                } else {
                    mask--;
                    if(j >= 0)
                    netmask -= (1 << (j));
                    j += 1;
                    havedec = 1;
                }
            }

            SR_PRINTF("ff2222 %x %x %x \n",j, tmp ,netmask);
            for(j = tmp; ((j & netmask) == (tmp & netmask)); j--) {
                SR_PRINTF("fffff\n");
            }
            ipC.s_addr = ntohl(tmp);
        /*
         * doSystem if action == 1 add 0 del
         */
            table = SRCIP_ROUTE_TABLE_BASE + wanIndex;
            if(action == UTT_SPEED_WEB_SEND_MSG_ACT_ADD) {
                doSystem("ip rule add from %s/%d priority %d table %d",inet_ntoa(ipC), mask, priority, table);
            } else {
                doSystem("ip rule del from %s/%d table %d",inet_ntoa(ipC), mask, table);
            }
            mask = 0;
            netmask = 0;
            tmp = j;
        }
    }
    doSystem("echo 1 >/proc/sys/net/ipv4/route/flush ");
    return;
}

/* _modifyLanIpRuleAndRoute modify the route rule of lan port(br0)'s IP.
 * it will not follow the srcip route regular 
 * action ÎªdelÊ±£¬É¾³ýµÄipµØÖ·,ÍøÂç×Ö½ÚÐò ,µ±actionÎª0Ê±£¬´«Èë0
 */
extern void _modifyLanIpRuleAndRoute(int action, Uint32 delIp) 
{
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;

    struct in_addr addr;
    char lanIp[RT_IP_LEN];
#if (MULTI_LAN == FYES)
    struct in_addr addr2;
    struct in_addr addr3;
    struct in_addr addr4;
    char lanIp2[RT_IP_LEN];
    char lanIp3[RT_IP_LEN];
    char lanIp4[RT_IP_LEN];
#endif

    if (action == LAN_AC_DEL_LAN_RULE) {
	addr.s_addr = delIp;
    } else {
	profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
	if ((profIF == NULL) || (profIF->head.active == FALSE)) { /* ÏßÂ·±»É¾³ý */
	    return;
	}

	addr.s_addr = profIF->ether.sta.ip;
#if (MULTI_LAN == FYES)
	addr2.s_addr = profIF->ether.sta.ip2;
	addr3.s_addr = profIF->ether.sta.ip3;
	addr4.s_addr = profIF->ether.sta.ip4;
#endif
    }

    strcpy(lanIp, inet_ntoa(addr));
#if (MULTI_LAN == FYES)
    strcpy(lanIp2, inet_ntoa(addr2));
    strcpy(lanIp3, inet_ntoa(addr3));
    strcpy(lanIp4, inet_ntoa(addr4));
#endif

#if 0
    char *lanIp = nvram_bufget(RT2860_NVRAM, _CONF_LAN_IP);
#endif
    /* delete the orignal rule */
    if(action == LAN_AC_DEL_LAN_RULE) {
	/* mantis 8537 */
	doSystem("ip rule del from %s table %d prio %d", lanIp, USR_ROUTE_TABLE_BASE,
            SRCIP_ROUTE_RULE_PRIO_BASE);

         doSystem("ip rule del from %s table main prio %d", lanIp,
            SRCIP_ROUTE_RULE_PRIO_BASE);
    } else {
        /* add new rule while lan ip changes*/
	/* mantis 8537 */
	doSystem("ip rule add from %s table %d prio %d", lanIp, USR_ROUTE_TABLE_BASE,
            SRCIP_ROUTE_RULE_PRIO_BASE);

         doSystem("ip rule add from %s table main prio %d", lanIp,
            SRCIP_ROUTE_RULE_PRIO_BASE);
#if (MULTI_LAN == FYES)
	 if(addr2.s_addr != 0U)
	 {
	     doSystem("ip rule add from %s table %d prio %d", lanIp2, USR_ROUTE_TABLE_BASE,
		     SRCIP_ROUTE_RULE_PRIO_BASE);

	     doSystem("ip rule add from %s table main prio %d", lanIp2,
		     SRCIP_ROUTE_RULE_PRIO_BASE);
	 }
	 if(addr3.s_addr != 0U)
	 {
	     doSystem("ip rule add from %s table %d prio %d", lanIp3, USR_ROUTE_TABLE_BASE,
		     SRCIP_ROUTE_RULE_PRIO_BASE);

	     doSystem("ip rule add from %s table main prio %d", lanIp3,
		     SRCIP_ROUTE_RULE_PRIO_BASE);
	 }
	 if(addr4.s_addr != 0U)
	 {
	     doSystem("ip rule add from %s table %d prio %d", lanIp4, USR_ROUTE_TABLE_BASE,
		     SRCIP_ROUTE_RULE_PRIO_BASE);

	     doSystem("ip rule add from %s table main prio %d", lanIp4,
		     SRCIP_ROUTE_RULE_PRIO_BASE);
	 }
#endif
    }
    return;
}

/* judge the wanNo port is alive. while alive return 1, else return 0
 * if the line status of wanNo is up, 
 *      and it is main port  it is alive
 *      else if is backup port and there no alive-main-line. it is alive
 * else it is sleep, return 0 */
extern int isWanInUse(int wanNo)
{
    char buf[64]; 
    int linkState=0, AliveMainLines=0;

    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;

    memset(buf, 0x00, sizeof(buf));
    /* get the line status ,main or backup line */
#if 0
    wanStatus=nvram_bufget(RT2860_NVRAM,add_wanx("Status", wanNo, buf));
#endif
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibType, wanNo);

    if( _getWanLinkState(wanNo) == 1){
        /* the line status is up 
         * get the number of alive-main-lines*/
        AliveMainLines=HaveOtherMainAlive(0);
        if ( AliveMainLines>=1) {
            if (profIF->runningLineMode == 1) {
                /* port is alive */
                linkState = 1;
            } 
        }else {
            if (profIF->runningLineMode == 0) {
                /* port is alive */
                linkState=1;
            }
        }
    }
    return linkState;
}
#endif
