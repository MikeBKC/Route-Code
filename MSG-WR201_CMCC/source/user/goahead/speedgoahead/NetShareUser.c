#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "internet.h"
#include "mibunion.h"
#include "profacce.h"
#include "ifmisc.h"
#include "comExeFun.h"
#include <signal.h>
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
#include    <usb-user.h>
#endif
#if (NET_SHARE_USER == FYES)


static void NetShareUserAdd(NetShareUserProfile *prof);
static void NetShareUserEdit(NetShareUserProfile *oldProf,NetShareUserProfile *prof);
static void NetShareUserDel(NetShareUserProfile *prof);

void netShareUserProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType aChangeType, Uint32 index,
        const NetShareUserProfile* oldProfileDataPtr, const NetShareUserProfile* newProfileDataPtr )
{/*对于端口管理所有实例永久存在*/
    switch(aChangeType) {
        case PROF_CHANGE_ADD:
            NetShareUserAdd(newProfileDataPtr);
            break;
        case PROF_CHANGE_EDIT:/*编辑*/
            NetShareUserEdit(oldProfileDataPtr,newProfileDataPtr);
            break;
        case PROF_CHANGE_DEL:
            NetShareUserDel(oldProfileDataPtr);
            break;
        case PROF_CHANGE_DELALL:
            break;
        default:/*默认*/
            break;
    }
}

static void NetShareUserAdd(NetShareUserProfile *prof)
{
    char tmp[256];

    memset(tmp,0,sizeof(tmp));

    if(prof != NULL ){
        sprintf(tmp,"adduser %s > /dev/null",prof->head.name);
        system(tmp);
        sprintf(tmp,"chpasswd.sh %s %s ",prof->head.name,prof->password);
        system(tmp);

        sprintf(tmp,"touch /etc/smb.conf; smbpasswd -a %s %s",prof->head.name,prof->password);
        system(tmp);
    }
#if 0
    if(prof->allow_ftp == FUN_ENABLE){
        ftpServerInit();
    }
    netShareManageInit();
#endif
}

static void NetShareUserEdit(NetShareUserProfile *oldProf,NetShareUserProfile *prof)
{
    char tmp[256];

    memset(tmp,0,sizeof(tmp));

    if(oldProf != NULL ){
        sprintf(tmp,"smbpasswd -x %s",oldProf->head.name);
        system(tmp);

        sprintf(tmp,"deluser %s",oldProf->head.name);
        system(tmp);
    }
    if(prof != NULL ){
        sprintf(tmp,"adduser %s > /dev/null",prof->head.name);
        system(tmp);
        sprintf(tmp,"chpasswd.sh %s %s ",prof->head.name,prof->password);
        system(tmp);

        sprintf(tmp,"touch /etc/smb.conf; smbpasswd -a %s %s > /dev/null",prof->head.name,prof->password);
        system(tmp);
    }
#if 0
    if( oldProf->allow_ftp != prof->allow_ftp ){
        ftpServerInit();
    }
    netShareManageInit();
#endif
}

static void NetShareUserDel(NetShareUserProfile *prof)
{
    char tmp[256];

    memset(tmp,0,sizeof(tmp));

    if(prof != NULL ){
        sprintf(tmp,"smbpasswd -x %s",prof->head.name);
        system(tmp);

        sprintf(tmp,"deluser %s",prof->head.name);
        system(tmp);
    }
#if 0
    if(prof->allow_ftp == FUN_ENABLE){
        ftpServerInit();
    }
    netShareManageInit();
#endif
}

static void netShareUserInit(void)
{
    MibProfileType mibType= MIB_PROF_NET_SHARE_USER;
    NetShareUserProfile *prof= NULL;
    MibProfileType mibTypeAd = MIB_PROF_ADMIN_CONF;
    AdminConfProfile *profAd = NULL;
    int max,min,i;
    char tmp[256];

    memset(tmp,0,sizeof(tmp));

    profAd = (AdminConfProfile *)ProfGetInstPointByIndex(mibTypeAd, 0);
    if(  profAd != NULL ){
    sprintf(tmp,"touch /etc/smb.conf; smbpasswd -a %s %s > /dev/null",profAd->head.name,profAd->password);
    system(tmp);
    }

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min+1; i < max; i++) {
        prof = (NetShareUserProfile *)ProfGetInstPointByIndex(mibType, i);
        if((prof!= NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE)) {
            sprintf(tmp,"adduser %s  > /dev/null",prof->head.name);
            system(tmp);
            sprintf(tmp,"chpasswd.sh %s %s ",prof->head.name,prof->password);
            system(tmp);

            sprintf(tmp,"touch /etc/smb.conf; smbpasswd -a %s %s > /dev/null",prof->head.name,prof->password);
            system(tmp);
        }
    }
}

void funInitNetShareUser(void)
{
    registerForProfileUpdates(MIB_PROF_NET_SHARE_USER, (Pointer)NULL, (ProfileUpdateFnc)netShareUserProfileChange);
    netShareUserInit();//配置初始化生效
    return;
}


#endif
