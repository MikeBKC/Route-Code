

/*
 * added by brwang at 2009/03/23
 * for config wireless 
 */
#include	"math.h"
#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"flash_api.h"
#include	"uttfunction.h"
#include	<linux/autoconf.h>
#include        <sys/utsname.h>
#include	"mib.h"
#include <uttMachine.h>
#include	"translate.h"
//#include	"webImpMisc.h"
#include <sys/mman.h>

#if (UTT_LICENSE == FYES)
#if defined(CONFIG_BOARD_P1010_C) || defined(CONFIG_BOARD_MTK7620_E) || defined(CONFIG_BOARD_MTK7620_EN) 
#ifndef CONFIG_LICENSE_PARTION
#error "please open CONFIG_LICENSE_PARTION in kerel!"
#endif
#endif
#define SWVER_1 8
#define SWVER_2 8
#define SWVER_3 8
#define HWVER_1 4
#define HWVER_2 4
#endif


#ifdef UBOOT_HEADER

#define IH_MAGIC        0x27051956      /* Image Magic Number           */
#define IH_NMLEN        20          /* Image Name Length            */

/* 两个头结构中必须保证ih_dcrc，systemVer，name的位置相同 */
typedef struct image_header {
	uint32_t	ih_magic;	/* Image Header Magic Number	*/
	uint32_t	ih_dcrc;	/* Image Data CRC Checksum	*/
	uint32_t	ih_size;	/* Image Data Size		*/
	uint32_t	ih_load;	/* Data	 Load  Address		*/
	uint32_t	ih_ep;		/* Entry Point Address		*/
	uint32_t	systemVer;	/* Image Creation Timestamp	*/
	uint32_t	ih_time;	/* Image Creation Timestamp	*/
	uint32_t	ih_hcrc;	/* Image Header CRC Checksum	*/
	uint8_t		ih_os;		/* Operating System		*/
	uint8_t		ih_arch;	/* CPU architecture		*/
	uint8_t		ih_type;	/* Image Type			*/
	uint8_t		ih_comp;	/* Compression Type		*/
	uint8_t		name[IH_NMLEN];	/* Image Name		*/
#if 1
	uint16_t	ih_fc;		/* fc mode for uboot */
	uint8_t		ih_update;	/* Image update control		*/
	uint8_t		ih_reserve; 
	//uint16_t	ih_reserve; 
#else
	uint32_t    ih_reserve;
#endif
	uint32_t	ih_ksz;		/* Kernel Part Size		*/
} image_header_t;
#elif defined(CONFIG_PPC_85xx)
#define IH_NMLEN        32          /* Image Name Length            */
typedef struct image_header {
	uint32_t	ih_magic;	/* Image Header Magic Number	*/
	uint32_t	ih_hcrc;	/* Image Header CRC Checksum	*/
	uint32_t	ih_time;	/* Image Creation Timestamp	*/
	uint32_t	ih_size;	/* Image Data Size		*/
	uint32_t	ih_load;	/* Data	 Load  Address		*/
	uint32_t	ih_ep;		/* Entry Point Address		*/
	uint32_t	ih_dcrc;	/* Image Data CRC Checksum	*/
	uint8_t		ih_os;		/* Operating System		*/
	uint8_t		ih_arch;	/* CPU architecture		*/
	uint8_t		ih_type;	/* Image Type			*/
	uint8_t		ih_comp;	/* Compression Type		*/
	uint8_t		name[IH_NMLEN];	/* Image Name		*/
} image_header_t;
#elif (UTT_AP_801 == FYES)
typedef struct image_header {
	    uint32_t imageLen;
		uint32_t imageCrc;
		uint32_t arch;
		uint32_t fsLen;
        uint32_t loadAddr;
		uint32_t systemVer;
        uint32_t entryAddr;
	    uint32_t os;
	uint16_t unuse;
	uint16_t funcVer;
		uint8_t  name[20];
        uint32_t kernelLen;
        uint32_t kernelCrc;
} image_header_t;
#else
typedef struct image_header {
        uint32_t imageLen;
        uint32_t imageCrc;
        uint32_t arch;
        uint32_t fsLen;
        uint32_t os;
	    uint32_t systemVer;
	uint16_t unuse;
	uint16_t funcVer;
        uint32_t loadAddr;
        uint32_t entryAddr;
        uint8_t  name[20];
#if defined( CONFIG_BOARD_MTK7620_E) || defined(CONFIG_RALINK_MT7621)  || defined(CONFIG_RALINK_MT7628) || defined(CONFIG_BOARD_MTK7620_EN)
        uint16_t        ih_fc;          /* fc mode for uboot */
        uint8_t         ih_update;      /* Image update control*/
        uint8_t         ih_reverse;
#else
        uint32_t kernelLen;
#endif
        uint32_t kernelCrc;
} image_header_t;
#endif
#if (UTT_SMART_UI == FYES)
static int getFlashMtdSize(int eid, webs_t wp, int argc, char_t **argv);
/********************************************************************
 * 函数： getFlashMtdSize
 * 功能： 获取Flash的mtd kernel size
 * 创建： 2013-12-3
 * 参数： 略
 * 返回： 略
 * 作者： zhao.chong 
 ********************************************************************/ 
static int getFlashMtdSize(int eid, webs_t wp, int argc, char_t **argv) 
{
    size_t size = 0;
    char_t *mtdName = NULL;

    if (ejArgs(argc, argv, T("%s"), &mtdName) == 1) {/*首先读取mtd名称*/
	if (mtdName != NULL) {
	    size = flash_mtd_size(mtdName);
	}
    }

    websWrite(wp, T("var mtdKernelSize = %ld;"), size);

    return 0;
}
#endif
#if (UTT_LICENSE == FYES)
int get_verinfo_from_file(char * file, int* hwVer, int* swVer){
    FILE * fp;
    int ret = -1;
    image_header_t head;
    int tmp; 
            
    if ( fp = fopen( file, "r" ) ){
	fread( &head, 1, sizeof(image_header_t), fp );
	tmp = (int)ntohl( head.systemVer );
	if ( swVer ){
	    *swVer = (tmp>>(HWVER_1+HWVER_2));
	}        
	if ( hwVer ){
	    *hwVer = (tmp & (0xFFFFFFFF>>(SWVER_1+SWVER_2+SWVER_3)));
	}
	ret = 0;
	fclose(fp);
    }else{   
	ret = -1;
    }        
    return ret;
}   

int get_version_from_local_license(int *swVer, int *hwVer)
{
    int i=0;
    unsigned int ver[3];
    int ver_max=0;
    char *pv=NULL;
    char buf[11];

    memset(buf, 0, 11);
    memset(ver, 0, sizeof(ver));
    
//    strcpy(buf, UTT_SWVERSION);
    strncpy(buf, UTT_SWVERSION, 10);

    if(isdigit(buf[0])){
	pv = buf;
    }else{
	pv = &buf[1];
    }
    DBGPRINT("%s-%d: %s\n",__func__,__LINE__,pv);
    if( pv = strtok(pv,".") ){
	if(!isdigit(pv[0])){
	    DBGPRINT("Soft: format of Version is wrong\n");
	    exit(1);
	}
	ver[i]=strtol(pv,NULL,10);
	ver_max = (int) pow(2,SWVER_1);
	if(ver[i]>ver_max){
	    DBGPRINT("Soft: Version number is too big\n");
	    exit(1);
	}
	while( pv = strtok(NULL,".") ){
	    i++;
	    if(i>2) {
		DBGPRINT("Soft: Version too many items\n");
		exit(1);
	    }
	    ver[i]=strtol(pv,NULL,10);
	    switch(i){
	    case 0:
		ver_max = (int) pow(2,SWVER_1);
		break;
	    case 1:
		ver_max = (int) pow(2,SWVER_2);
		break;
	    case 2:
		ver_max = (int) pow(2,SWVER_3);
		break;
	    default:
		break;
	    }
	    if(ver[i]>ver_max){
		DBGPRINT("Soft: Version number is too big\n");
		exit(1);
	    }
	}
    }
    *swVer = (unsigned int)( ( ver[0]<<(SWVER_3+SWVER_2) ) + ( ver[1]<<(SWVER_3) ) + ( ver[2]) );

    i = 0;
    pv=NULL;
    ver_max = 0;
    memset(buf, 0, 5);
    memset(ver, 0, sizeof(ver));
//    strcpy(buf, UTT_HWMODEL);
    strncpy(buf, UTT_HWMODEL, 10);

    if(isdigit(buf[0])){
	pv = buf;
    }else{
	pv = &buf[1];
    }
    DBGPRINT("%s-%d: buf=%s,pv=%s\n",__func__,__LINE__,buf,pv);
    if( pv = strtok(pv,".") ){
	if(!isdigit(pv[0])){
	    DBGPRINT("Hard: format of Version is wrong\n");
	    exit(1);
	}
	ver[i]=strtol(pv,NULL,10);
	ver_max = (int) pow(2,HWVER_1);
	if(ver[i]>ver_max){
	    DBGPRINT("Hard: Version number is too big\n");
	    exit(1);
	}

	while( pv =strtok(NULL,".") ){
	    i++;
	    if(i>1) {
		DBGPRINT("Hard: Version too many items\n");
		exit(1);
	    }
	    ver[i]=strtol(pv,NULL,10);
	    switch(i){
	    case 0:
		ver_max = (int) pow(2,HWVER_1);
		break;
	    case 1:
		ver_max = (int) pow(2,HWVER_2);
		break;
	    default:
		break;
	    }
	    if(ver[i]>ver_max){
		DBGPRINT("Hard: Version number is too big\n");
		exit(1);
	    }
	}
    }
    *hwVer = (unsigned int) ( ( ver[0] << (HWVER_2) ) + ver[1] );
    return 0;
}
#endif

extern unsigned long crc32 (unsigned long, const unsigned char *, unsigned int);
void formUpdateFirmware(webs_t wp, char * path, char * query) 
{
    static int updateFlag = 0;
    int bRet = 0;
    char *     fn;
    char *     bn = NULL;
    char *ReBoot;
    char *postData;
    int fnlen; 
    int ret = 0;
    image_header_t *head;
    image_header_t localHead;
    int headLen;
    struct utsname utsbuf;
    char_t *tmp;
    const char *ErrMsg=NULL;
#ifdef UBOOT_HEADER
    unsigned int checksum;
	unsigned int tmp_dcrc;
#elif defined(CONFIG_PPC_85xx)
	unsigned int checksum;
#endif
#if 1
#if (UTT_LICENSE == FYES)
    int swVer = 0;
    int hwVer = 0;
    int tmp_ver = 0;
    int swVer_local = 0;
    int hwVer_local = 0;
    int i=0;
    int ver[3];
    int ver_max=0;
    char *pv=NULL;
    char buf[11];
    unsigned short localfunc = 0U;
    unsigned short loadfunc = 0U;
#endif
#endif

    a_assert(websValid(wp));

    fn = websGetVar(wp, T("filename"), T(""));

    ReBoot = websGetVar(wp,T("reBoot"),  T(""));
    
    if (strcmp(ReBoot,"on") ==0) {
        bRet = 1;
    }
  
    if (fn != NULL && *fn != '\0') {        
      if ((int)(bn = gstrrchr(fn, '/') + 1) == 1) {
          if ((int)(bn = gstrrchr(fn, '\\') + 1) == 1) {
              bn = fn;
          }
      }
    }

    fnlen = wp->lenPostData;
    postData = wp->postData;
    wp->postData = NULL;
    wp->lenPostData = 0;

    if(updateFlag == 1u)
    {
	ErrMsg=getTransDataForC(C_LANG_INDEX_SOFTWARE_UPLOADING);
	setTheErrorMsg(ErrMsg);
        goto OUT;
    }
    if( fnlen <= sizeof(image_header_t) ){
	ErrMsg=getTransDataForC(C_LANG_INDEX_INVALID_FILE);
        setTheErrorMsg(ErrMsg);
        goto OUT;
    }

    head = (image_header_t*)postData;

    ret = flash_read_mtdname("Kernel", (char*)&localHead, 0, sizeof(image_header_t) );
    if (ret == -1) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_SOFTWARE_UPGRADE_FAILED);
        setTheErrorMsg(ErrMsg);
        goto OUT;
    }

    uname(&utsbuf);
    tmp = strtok(utsbuf.version, " ");

#if (CONFIG_X86_CPU == 1)
    if(strcmp(utsbuf.version, (char *)head->name) != 0) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_SOFTWARE_TYPE_ERROR);
        setTheErrorMsg(ErrMsg);
        goto OUT;
    }
#else
    if(strcmp(localHead.name, head->name) != 0) {
        DBGPRINT("VER ERROR:local[%s],web[%s]\n", localHead.name, head->name);
	ErrMsg = getTransDataForC(C_LANG_INDEX_SOFTWARE_TYPE_ERROR);
        setTheErrorMsg(ErrMsg);
        goto OUT;
    }
#endif
#if 1
#if (UTT_LICENSE == FYES)
    if(_judgeUttSN() == 1){
    memset(buf, 0, 11);
    memset(ver, 0, sizeof(ver));
#ifdef UBOOT_HEADER
    strcpy(buf, UTT_FUNCVER);
    if(isdigit(buf[0])){
	pv = buf;
    }else{
	pv = &buf[1];
    }
    localfunc = (unsigned short)strtol(pv, NULL, 10);
    loadfunc = head->ih_update;
    if(loadfunc < localfunc) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_SOFTWARE_UPGRADE_FAILED);
	printf("%s-%d: localHead->ih_update=%d,localfunc=%d,loadfunc=%d\n",__func__,__LINE__,localHead.ih_update,localfunc,loadfunc);
	setTheErrorMsg(ErrMsg);
	goto OUT;
    }
#elif defined(CONFIG_PPC_85xx)
#elif (UTT_AP_801 == FYES)
#else
#if defined(CONFIG_BOARD_MTK7620_E) || defined(CONFIG_BOARD_MTK7620_EN)
    strcpy(buf, UTT_FUNCVER);
    if(isdigit(buf[0])){
      pv = buf;
    }else{
      pv = &buf[1];
    }
    localfunc = (unsigned short)strtol(pv, NULL, 10);
    loadfunc = head->ih_update;
    printf("%s-%d: localfunc=%d,loadfunc=%d,localHead.ih_update =%d\n",__func__,__LINE__,localfunc,loadfunc,loadfunc);
    if(loadfunc < localfunc) {
      ErrMsg=		getTransDataForC(C_LANG_INDEX_SOFTWARE_UPGRADE_FAILED);
      fprintf(stderr,"%s-%d: localfunc=%d,loadfunc=%d,localHead.ih_update =%d\n",__func__,__LINE__,localfunc,loadfunc);
      setTheErrorMsg(ErrMsg);
      goto OUT;
    }       
#else
    tmp_ver = (int)ntohs( head->systemVer );
    swVer = (tmp_ver>>(HWVER_1+HWVER_2));
    hwVer = (tmp_ver & (0xFFFF>>(SWVER_1+SWVER_2+SWVER_3)));
    get_version_from_local_license(&swVer_local, &hwVer_local);
    strcpy(buf, UTT_FUNCVER);
    if(isdigit(buf[0])){
	pv = buf;
    }else{
	pv = &buf[1];
    }
    localfunc = (unsigned short)strtol(pv, NULL, 10);
    loadfunc = head->funcVer;
    DBGPRINT("%s-%d: localfunc=%d,loadfunc=%d\n",__func__,__LINE__,localfunc,loadfunc);
    if(loadfunc < localfunc) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_SOFTWARE_UPGRADE_FAILED);
	setTheErrorMsg(ErrMsg);
	printf("%s-%d: tmp_ver=%d, swVer=%d, hwVer=%d,sec_ver=%d,third_ver=%d,load_funcVer=%d\n",__func__,__LINE__,
		tmp_ver,swVer,hwVer,swVer&0x70,swVer&0xF,head->funcVer);
	printf("%s-%d: swver_local=%d, hw_local=%d,localfuncVer=%d\n",__func__,__LINE__,swVer_local,hwVer_local,localfunc);
	goto OUT;
    }
#endif
#endif
    }
#endif
#endif

    headLen = sizeof(image_header_t);
#ifdef UBOOT_HEADER
	tmp_dcrc = head->ih_dcrc;
    checksum = ntohl(head->ih_hcrc);
    head->ih_hcrc = htonl(0);	/* clear for re-calculation */
    head->ih_dcrc = htonl(0);	/* clear for re-calculation */
    if (crc32 (0, postData, headLen) != checksum) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_CHECK_SUM_ERROR);
        setTheErrorMsg(ErrMsg);
	goto OUT;
    }
    head->ih_hcrc = htonl(checksum);

	head->ih_dcrc = tmp_dcrc;
    checksum = crc32 (0, (postData+8), (fnlen-8));
    if ( checksum != ntohl(head->ih_dcrc)) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_CHECK_SUM_ERROR);
        setTheErrorMsg(ErrMsg);
	goto OUT;
    }
#elif defined(CONFIG_PPC_85xx)
    printf("head->ih_hcrc = 0x%x, head->ih_dcrc = 0x%x\n", head->ih_hcrc, head->ih_dcrc);
    checksum = ntohl(head->ih_hcrc);
    head->ih_hcrc = htonl(0);	/* clear for re-calculation */
    printf("crc32 (0, postData, headLen) = 0x%x\n", crc32 (0, postData, headLen));
    if (crc32 (0, postData, headLen) != checksum) {
	    ErrMsg = getTransDataForC(C_LANG_INDEX_CHECK_SUM_ERROR);
	    setTheErrorMsg(ErrMsg);
	    goto OUT;
    }
    head->ih_hcrc = htonl(checksum);

    checksum = crc32 (0, (postData+headLen), (fnlen-headLen));
    printf("checksum = 0x%x\n", checksum);
    if ( checksum != ntohl(head->ih_dcrc)) {
	    ErrMsg = getTransDataForC(C_LANG_INDEX_CHECK_SUM_ERROR);
	    setTheErrorMsg(ErrMsg);
	    goto OUT;
    }
#else
    if (crc32 (0, (const unsigned char *)postData+8, fnlen - 8) != ntohl(head->imageCrc)) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_CHECK_SUM_ERROR);
        setTheErrorMsg(ErrMsg);
	printf("%s-%d: creat_crc=%ld, load_crc=%ld\n",__func__,__LINE__,crc32 (0, (const unsigned char *)postData+8, fnlen - 8),ntohl(head->imageCrc));
	goto OUT;
    }
#endif

#if (FEATURE_AP == FYES)
	doSystem("rm -rf /tmp/urcp");
#endif

#if (CONFIG_X86_CPU == 1)
    ret = flash_write_mtdname("/dev/sda", postData + headLen, 0, fnlen - headLen);
#else
    mtd_unlock("Kernel");
    ret = flash_write_mtdname("Kernel", postData, 0, fnlen);
#endif
    if(ret == -1) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_SOFTWARE_UPGRADE_FAILED);
	setTheErrorMsg(ErrMsg);
	goto OUT;
    }

    if (bRet) {
#if (UTT_SMART_UI == FYES)
        websRedirect(wp, "upgrade_Sui.asp");
#else
        websRedirect(wp, "update.asp");
#endif
	updateFlag = 1;/*防止重启过程中，goahead再次收到写flash升级数据包*/
#if (WEBAUTH_ACCOUNT == FYES)
	doSystem("rebootS \\&");
#else
	doSystem("reboot&");
#endif
    }
    else
    {
#if (UTT_SMART_UI == FYES)
	websRedirect(wp, "FirmwareUpgrade_Sui.asp");
#else
	websRedirect(wp, "SoftwareUpdate.asp");
#endif
    }
OUT:
    if(postData) {
	bfree(B_L, postData);
    }

    if(!webErrorIsOk)
    {/*有错*/
	if(updateFlag == 1)
	{/*软件升级中被重复点击*/
#if (UTT_SMART_UI == FYES)
	    websRedirect(wp, "upgrade_Sui.asp");
#else
	    websRedirect(wp, "update.asp");
#endif
	}
	else
	{
#if (UTT_SMART_UI == FYES)
	    websRedirect(wp, "FirmwareUpgrade_Sui.asp");
#else
	    websRedirect(wp, "SoftwareUpdate.asp");
#endif
	}
    }
    return;    
}

#if (UTT_SMART_UI == FYES)
#if (APP_POLICY_REMOTE_UPDATE == FYES)
void formAutoUpdateFirmware(webs_t wp, char * path, char * query) 
{
    static int updateFlag = 0;
    int bRet = 0;
    char *     fn;
    void *map_addr;
    int fnlen; 
    int ret = 0;
    image_header_t *head;
    image_header_t localHead;
    int headLen;
    struct utsname utsbuf;
    char_t *tmp;
    const char *ErrMsg=NULL;
    int map_fd;
    int fileS, fileE, fileL;

    a_assert(websValid(wp));

    map_fd = open("/kernelImage", O_RDWR);
    if(map_fd < 0) {
        printf("cannot open file /kernelImage\n");
        return ;
    }
    fileS = lseek(map_fd, 0, SEEK_SET);
    fileE = lseek(map_fd, 0, SEEK_END);
    lseek(map_fd, 0, SEEK_SET);
    fileL = fileE - fileS;
    fnlen = fileL;
    fileL = fileL % 4096 == 0 ? fileL : fileL + 4096 - fileL/4096;
    map_addr = mmap(0, fileL, PROT_READ|PROT_WRITE, MAP_SHARED, map_fd, 0);
    head = (image_header_t*)map_addr;

    ret = flash_read_mtdname("Kernel", (char*)&localHead, 0, sizeof(image_header_t) );
    if (ret == -1) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_SOFTWARE_UPGRADE_FAILED);
        setTheErrorMsg(ErrMsg);
        goto OUT;
    }

    uname(&utsbuf);
    tmp = strtok(utsbuf.version, " ");

    if(strcmp(localHead.name, head->name) != 0) {
        printf("VER ERROR:local[%s],web[%s]\n", localHead.name, head->name);
	ErrMsg = getTransDataForC(C_LANG_INDEX_SOFTWARE_TYPE_ERROR);
        setTheErrorMsg(ErrMsg);
        goto OUT;
    }

    headLen = sizeof(image_header_t);
    if (crc32(0, (const unsigned char *)map_addr+8, fnlen - 8) != ntohl(head->imageCrc)) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_CHECK_SUM_ERROR);
        setTheErrorMsg(ErrMsg);
	printf("%s-%d: creat_crc=%ld, load_crc=%ld\n",__func__,__LINE__,crc32 (0, (const unsigned char *)map_addr+8, fnlen - 8),ntohl(head->imageCrc));
	goto OUT;
    }


    mtd_unlock("Kernel");
    ret = flash_write_mtdname("Kernel", map_addr, 0, fnlen);
    if(ret == -1) {
	ErrMsg = getTransDataForC(C_LANG_INDEX_SOFTWARE_UPGRADE_FAILED);
	setTheErrorMsg(ErrMsg);
    }
OUT:
    munmap(map_addr, fileL);
    close(map_fd);
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/html\nPragma: no-cache\nCache-Control: no-cache\n\n"));
    if(!webErrorIsOk)
    {/*有错*/
	doSystem("rm -rf /kernelImage /ratio &");
	websWrite(wp, T("%s"), "FirmwareUpgrade_Sui.asp");
    } else {
	websWrite(wp, T("%s"), "upgrade_Sui.asp");
	doSystem("reboot&");
    }
    websDone(wp, 200);
    return;    
}
#endif
#endif
#if 0
void formBackupFirmware(webs_t wp, char *path, char *query)
{
    FILE *fp;
    int len;
    char dFilename[50] = "uttWlanSoftware.bin"; 
    char buf[64],*tmp;
    char *fileContext,*fileContextTmp;
    int headLen = sizeof(image_header_t);
    int fileLen = headLen + PB44_ROOTFS_LEN + PB44_VMLINUX_LEN;
    fileContextTmp = fileContext = malloc(sizeof(char) * fileLen);
    if(fileContext == NULL)
        return ;

    fp = popen("uname -m", "r");
    if(fp){
      /*
       * should add file head
       */
    fgets(buf, 64, fp);
    pclose(fp);
    tmp = strtok(buf,"\n");
    strcpy(((image_header_t*)fileContext)->ih_name,tmp);
    fileContext += headLen;
    FlashRead(fileContext, 0, PB44_VMLINUX_LEN, MTD_BLOCK_LINUX);

    fileContext += PB44_VMLINUX_LEN;
    FlashRead(fileContext, 0, PB44_ROOTFS_LEN, MTD_BLOCK_ROOTFS);

    wimDownloadFile(wp,(char *)dFilename, fileContextTmp, fileLen);
    
    free(fileContextTmp);
    }
    return;
}
#endif

void formDefineSoftwareUpload(void)
{
	websFormDefine(T("UpdateFirmware"), formUpdateFirmware);
#if (UTT_SMART_UI == FYES)
#if (APP_POLICY_REMOTE_UPDATE == FYES)
	websFormDefine(T("formAutoUpdateFirmware"), formAutoUpdateFirmware);
#endif
	websAspDefine(T("getFlashMtdSize"), getFlashMtdSize);
#endif
//	websFormDefine(T("BackupFirmware"), formBackupFirmware);
}
