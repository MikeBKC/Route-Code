#ifndef __PROFMISC_H
#define __PROFMISC_H

#define PROFFAIL -1
#define PROFOK  0
#define PROF_UNUSED_INDEX -1  /*profile中不使用的索引*/
#define ProfInstIsActive(prof) (((mibHead*)(prof))->active)
#define UTT_MSG_TYPE_1 44
#define SUCCESS			1
#define FAIL			0

#if CONFIG_UTT_DEBUG
#define ERROR_TO_DEV_NULL ""
#else
#define ERROR_TO_DEV_NULL "1>/dev/null 2>&1"
#endif

#if ((UTT_LV_6530G == FYES) || (UTT_LV_4330G == FYES) || (UTT_WX_2000 == FYES) || (UTT_LV_N8000 == FYES))
#define CONFIG_FLASH_SIZE 0x40000u/*配置文件压缩后的大小*/
#define CONFIG_BUFF_SIZE  10u*CONFIG_FLASH_SIZE/*配置文件无压缩大小*/
#elif ((UTT_QV_4320G == FYES) || (UTT_QV_4220G == FYES) || (UTT_QV_4240G == FYES) || (UTT_QV_N1800 == FYES) || (UTT_QV_2620G == FYES) || (UTT_QV_2610G == FYES) || (UTT_QV_1210G == FYES) || (UTT_QV_1220G == FYES) ||(UTT_QV_3320G == FYES))
#define CONFIG_FLASH_SIZE 0x20000u/*配置文件压缩后的大小*/
#define CONFIG_BUFF_SIZE  10u*CONFIG_FLASH_SIZE/*配置文件无压缩大小*/
#else
#define CONFIG_FLASH_SIZE 0x10000u/*配置文件压缩后的大小*/
#define CONFIG_BUFF_SIZE  10u*CONFIG_FLASH_SIZE/*配置文件无压缩大小*/
#endif
#define CONFIG_HEADER_LEN 32u
#define Z_OK 0 /*该宏为zlib中压缩解压缩成功返回的标志。但由于zlib.h跟本文件会有数据类型的重定义。故再次声明*/
#if (UTT_NV_D908W == FYES)
/* AP的配置文件保存在AC设备中的 'apConfig' 分区 */
#define CONFIG_AP_FLASH_SIZE 0x10000u/*AP 配置文件压缩后的大小*/
#define CONFIG_AP_BUFF_SIZE  10u*CONFIG_AP_FLASH_SIZE/*AP 配置文件无压缩大小*/
extern int ap_conf_flash_write(char *buf, off_t to, size_t len, char *mtd_name);
extern int ap_conf_flash_read(char *buf, off_t from, size_t len, char *mtd_name);
extern int apConfigWriteToFlash(void);
extern int apConfigReadFromFlash(void);
extern int apDrawingWriteToFlash(void);
extern int apDrawingReadFromFlash(void);
#endif

#if (NOTICE == FYES)
#define NOTICE_HTML_START_1 "```Notice Html 1```"
#define NOTICE_HTML_1_NAME "Notice1.asp"
#define NOTICE_HTML_1 "/etc_ro/web/"NOTICE_HTML_1_NAME
#define NOTICE_HTML_START_2 "```Notice Html 2```"
#define NOTICE_HTML_2_NAME "Notice2.asp"
#define NOTICE_HTML_2 "/etc_ro/web/"NOTICE_HTML_2_NAME
#define NOTICE_HTML_START_3 "```Notice Html 3```"
#define NOTICE_HTML_3_NAME "Notice3.asp"
#define NOTICE_HTML_3 "/etc_ro/web/"NOTICE_HTML_3_NAME
#define NOTICE_HTML_START_4 "```Notice Html 4```"
#define NOTICE_HTML_4_NAME "Notice4.asp"
#define NOTICE_HTML_4 "/etc_ro/web/"NOTICE_HTML_4_NAME
#define NOTIFY_HTM		"/etc_ro/web/Notify.htm"
#define DNS_NOTIFY_HTM		"/etc_ro/web/DnsNotify.htm"


#define NOTIFY_AUTO_SKIP_SIZE  150
#endif
#if (WEB_AUTH == FYES)
#define WEB_AUTH_CONTACT_START	"```WebAuth Contact```"
#define WEB_AUTH_CONTACT_FILE	"WebAuthContact.txt"
#define WEB_AUTH_CONTACT_PATH	"/etc_ro/web/"WEB_AUTH_CONTACT_FILE
#define WEB_AUTH_PICURL_START	"```WebAuth Picurl```"
#define WEB_AUTH_PICURL_FILE	"WebAuthPicUrl.txt"
#define WEB_AUTH_PICURL_PATH	"/etc_ro/web/"WEB_AUTH_PICURL_FILE
#if (WEBAUTH_AUTO == FYES)
#define WEB_AUTH_JUMPURL_START	"```WebAuth Jumpurl```"
#define WEB_AUTH_JUMPURL_FILE	"WebAuthJumpUrl.txt"
#define WEB_AUTH_JUMPURL_PATH	"/etc_ro/web/"WEB_AUTH_JUMPURL_FILE
#endif
#endif
#if ((NOTICE == FYES) || (WEB_AUTH == FYES))
#define NOTICE_TILTLE_LEN   	20
#define NOTICE_MANAGER_LEN  	20
#define NOTICE_CONTENT_LEN  	2000
#define	NOTIFY_HTM1_MOUDLE_SIZE	1000
#define	NOTIFY_HTM1_SIZE	( NOTICE_TILTLE_LEN  + NOTICE_MANAGER_LEN + NOTICE_CONTENT_LEN + NOTIFY_HTM1_MOUDLE_SIZE )
#endif

#if (FEATURE_OLDFS == FYES)
#define CONFIG_FILE_PATH "/dev/sda3"
#define LICENSE_FILE "/dev/sda4"
#else
#define CONFIG_FILE_PATH "/dev/sda2"
#define LICENSE_FILE "/dev/sda3"
#endif

#include "conver.h"
#include "a2n.h"
#include "profacceShare.h"

typedef struct st_LinearPortNumber {
    Word        linearPortNumber;
} LinearPortNumber;

struct st_profInstCount{
    int min;
    int max;
};

typedef struct _struProfAlloc{
    MibProfileType proftype;   
    Pointer oldProf;
    int        instIndex;      
    int        profsize;      
    int        flag;      
    ProfChangeType		   changeType;      
    struct _struProfAlloc  *next;
    struct _struProfAlloc  *tail;
} struProfAlloc;

typedef struProfAlloc* pstruProfAlloc;

extern NvramProfiles*    nvramProfile;

extern Pointer _getProfilePointer(SlotNumber	slot,
		   MibProfileType	profileType,
		   Word			instIndex,
                   Uint			*length,
		   Uint			*maxInst,
		   Uint			*minInst);

extern Boolean ProfGetInstByIndex(MibProfileType  profileType, Word instIndex, Pointer dataPtr ); 
extern Pointer ProfGetInstPointByIndex(MibProfileType profileType, int instIndex);
extern int ProfGetInstIndexByName(MibProfileType profType, const char *name);
extern Pointer ProfGetInstPointByName(MibProfileType profType, const char *name);
extern const char*  ProfGetInstNameByIndex(MibProfileType profType, int instIndex);
extern int ProfDelInstByName(MibProfileType profType, const char* instname);
extern int ProfDelInstByIndex(MibProfileType profType, int instIndex);
extern int ProfInstIsFree(Pointer prof);
extern int ProfNewInst(MibProfileType proftype, const char* instname, Boolean bOverWrite);
extern int  ProfBackupByIndex(MibProfileType proftype, ProfChangeType changeType, int instIndex, pstruProfAlloc *proflist);
extern int  ProfBackupByName(MibProfileType proftype, ProfChangeType changeType, char *instName, pstruProfAlloc *proflist);
extern void ProfFreeAllocList(struProfAlloc *listhead);
extern void  ProfRestore(struProfAlloc *profhead);
extern void ProfUpdate(struProfAlloc * profhead);
extern void registerForProfileUpdates(MibProfileType mibType, Pointer updateTag, ProfileUpdateFnc updateFnc);
extern void reportProfileUpdate(SlotNumber slot, ProfChangeType changType, MibProfileType profileType, Uint32 instIndex, Pointer oldDataPtr,int flag);
extern void ProfInstLowHigh(MibProfileType proftype, int *max, int *min);
extern int UttMsgInit(void);
extern char* ProfShmInit(void);
extern int ProfInit(void);
extern unsigned int getSystemSn(void);
extern void ProfSetNameByPoint(Pointer prof, const Byte* name);
extern int profileBuildFactory(MibProfileType profType, Pointer *_prof);
extern int parseProfileFromStr(char* buf, Bool reset);
extern void ProfWiteToFlash(void);
#if (EASY_TASK == FYES)
extern int IsDefaultConf(void);
#endif
extern void ProfReadFromFlash(void);
extern void nvramWriteCommit(void);
extern int  ProfDetach(void);
extern int parseProfileToStr(char *buf, int *len);
extern int MibGetEnumByStr(char *str, const LcdParamEnumValue *enumPara);
extern char* MibGetStrByEnum(int value, const LcdParamEnumValue *enumPara);
extern int ProfCountInstNotFree(MibProfileType profType);
extern int ProfGetInstIndexFree(MibProfileType profType);
extern char* ProfBuildDefaultName(char* prefix, int maxNo);
extern int ProfGetMaxDefaultNameNo(MibProfileType profType, char* prefix);
extern int ProfGetInstPointByValue(MibProfileType profType, Uint32 offset, Uint32 size, IType itype, char *value, int specialIndex);
extern Uint32 ProfCountInstByValue(MibProfileType profType, Uint32 offset, Uint32 size, Uint32 cmpSize, Pointer value); 
extern void UttSendMsgForSpeed(speedMainType mainType, speedSecType secType, void* data, int size);
#if (NOTICE == FYES)
extern int saveConfigToFile(char *title, char *content, const char *source, const char *path);
#endif
extern char* getLanIfName(void);
extern int getIfIp(char *ifname, char *if_addr);
int getWanSlotCount(void);
#if 1

/* 自动新建实例 */
#define WEB_INST_START 1
#define MAX_WEB_INST 2048
extern Boolean isPortMapSet(PortMap *pm, Uint16 instIndex);

extern void setPortMap(PortMap *pm, Uint16 instIndex);

extern void clearPortMap(PortMap *pm, Uint16 instIndex);
extern void clearAllPortMap(PortMap *pm);
extern Uint16 getWebInstName(PortMap *pm, Uint16 *instIndexInst);
#endif
extern unsigned int strRemoveChar(char *sourceStr, char removeChar);
extern char * my_strlwr(char *str);
#ifdef CONFIG_UTT_DEBUG
extern void debug_print(Pointer para, const char *fmt, ...);
#endif
#if CONFIG_XPORT_LOG
extern int is_limit();
#endif
#endif
