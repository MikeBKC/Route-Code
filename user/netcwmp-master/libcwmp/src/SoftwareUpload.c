#include	"math.h"
#include	"stdint.h"
#include	<linux/autoconf.h>
#include    <sys/utsname.h>
#include    <sys/types.h>
#include	"flash_api.h"
#include	"mib.h"
#include    "uttMachine.h"
#include    "stdarg.h"
//#include    "cwmp/cwmp.h"
typedef char char_t;
#define DBGPRINT(fmt, args...) 
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
#if defined( CONFIG_BOARD_MTK7620_E) || defined(CONFIG_RALINK_MT7621) || defined(CONFIG_RALINK_MT7628) || defined(CONFIG_BOARD_MTK7620_EN)
        uint16_t        ih_fc;          /* fc mode for uboot */
        uint8_t         ih_update;      /* Image update control*/
        uint8_t         ih_reverse;
#else
        uint32_t kernelLen;
#endif
        uint32_t kernelCrc;
} image_header_t;
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
int get_file_len(char* fn)
{
    FILE *fp = NULL; 
    int len = 0;
    if((fp = fopen(fn,"r")) > 0)
    {   
        fseek(fp,0,SEEK_END);
        len = ftell(fp);
        fseek(fp,0,SEEK_SET);
        fclose(fp);
    }
    return len;
}

void doSystem(const char* _Format, ...)
{
    char cmdstr[128];/*应该命令主要针对ipset故不会太大*/
    va_list   args; 

    memset(cmdstr, 0, sizeof(cmdstr));

    /*生成命令字符串*/
    va_start(args,_Format); 
    vsnprintf(cmdstr, sizeof(cmdstr)-1, _Format,   args   ); 
    va_end(args);

#if 0
    printf("cmd : %s\n",cmdstr);
#endif
    /*运行命令*/
    system(cmdstr);
    return;
}
extern unsigned long crc32 (unsigned long, const unsigned char *, unsigned int);
//void formUpdateFirmware(webs_t wp, char * path, char * query) 
int formUpdateFirmware(char* filename)
{
    static int updateFlag = 0;
    int bRet = 0;
    char *     fn = filename;
    char *     bn = NULL;
    char *postData;
    int fnlen; 
	int faultcode = 0;
    char_t *tmp;
    int ret = 0;
    image_header_t *head;
    image_header_t localHead;
    int headLen;
    struct utsname utsbuf;
    const char *ErrMsg=NULL;
#ifdef UBOOT_HEADER
    unsigned int checksum;
	unsigned int tmp_dcrc;
#elif defined(CONFIG_PPC_85xx)
	unsigned int checksum;
#endif
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
    //a_assert(filename);
    
    bRet = 1;
  
    if (fn != NULL && *fn != '\0') {        
      if ((int)(bn = wcsrchr(fn, '/') + 1) == 1) {
          if ((int)(bn = wcsrchr(fn, '\\') + 1) == 1) {
              bn = fn;
          }
      }
    }

    fnlen = get_file_len(fn);
    if((postData = malloc(fnlen)) == NULL)
    {
        faultcode = 9019;
        goto OUT;
    }

    FILE * fp = NULL;
    if((fp = fopen(fn,"r")) == NULL)
    {
        faultcode = 9019;
        goto OUT;
    }

    if(fread(postData,1,fnlen,fp) == 0)
    {
        faultcode = 9019;
        goto OUT;
    }

    /* "软件上传中，请勿重复操作！",*/
    if(updateFlag == 1u)
        goto OUT;

    /* "无效的文件",*/
    if( fnlen <= sizeof(image_header_t) )
    {
        faultcode = 9019;
        goto OUT;
    }

    head = (image_header_t*)postData;

    ret = flash_read_mtdname("Kernel", (char*)&localHead, 0, sizeof(image_header_t) );
    /* "软件升级失败",*/
    if (ret == -1) 
    {
        faultcode = 9019;
        ret = 0;
        goto OUT;
    }

    uname(&utsbuf);
    tmp = strtok(utsbuf.version, " ");

	/*"软件型号错误，请重新选择软件",*/
#if (CONFIG_X86_CPU == 1)
    if(strcmp(utsbuf.version, (char *)head->name) != 0) 
    {
        faultcode = 9019;
        goto OUT;
    }
#else
    cwmp_log_debug("%s____%d;localHead.name : %s\n",__func__,__LINE__,localHead.name);
    cwmp_log_debug("%s____%d;head->name : %s\n",__func__,__LINE__,head->name);
    if(strcmp(localHead.name, head->name) != 0) 
    {
        cwmp_log_debug("VER ERROR:local[%s],web[%s]\n", localHead.name, head->name);
        faultcode = 9019;
        goto OUT;
    }
#endif
#if (UTT_LICENSE == FYES)
    if(_judgeUttSN() == 1)
    {
    memset(buf, 0, 11);
    memset(ver, 0, sizeof(ver));
#ifdef UBOOT_HEADER
    strcpy(buf, UTT_FUNCVER);
    if(isdigit(buf[0]))
    {
	pv = buf;
    }else{
	pv = &buf[1];
    }
    localfunc = (unsigned short)strtol(pv, NULL, 10);
    loadfunc = head->ih_update;
    /* "软件升级失败",*/
    if(loadfunc < localfunc) 
    {
	cwmp_log_debug("%s-%d: localHead->ih_update=%d,localfunc=%d,loadfunc=%d\n",__func__,__LINE__,localHead.ih_update,localfunc,loadfunc);
    faultcode = 9019;
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
    cwmp_log_debug("%s-%d: localfunc=%d,loadfunc=%d,localHead.ih_update =%d\n",__func__,__LINE__,localfunc,loadfunc,loadfunc);
    /* "软件升级失败",*/
    if(loadfunc < localfunc) {
      fprintf(stderr,"%s-%d: localfunc=%d,loadfunc=%d,localHead.ih_update =%d\n",__func__,__LINE__,localfunc,loadfunc);
      faultcode = 9019;
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
    /* "软件升级失败",*/
    if(loadfunc < localfunc) {
	cwmp_log_debug("%s-%d: tmp_ver=%d, swVer=%d, hwVer=%d,sec_ver=%d,third_ver=%d,load_funcVer=%d\n",__func__,__LINE__,
		tmp_ver,swVer,hwVer,swVer&0x70,swVer&0xF,head->funcVer);
	cwmp_log_debug("%s-%d: swver_local=%d, hw_local=%d,localfuncVer=%d\n",__func__,__LINE__,swVer_local,hwVer_local,localfunc);
    faultcode = 9019;
	goto OUT;
    }
#endif
#endif
    }
#endif

    headLen = sizeof(image_header_t);
#ifdef UBOOT_HEADER
	tmp_dcrc = head->ih_dcrc;
    checksum = ntohl(head->ih_hcrc);
    head->ih_hcrc = htonl(0);	/* clear for re-calculation */
    head->ih_dcrc = htonl(0);	/* clear for re-calculation */
	/*"软件校验和错误",*/
    if (crc32 (0, postData, headLen) != checksum) {
    faultcode = 9019;
	goto OUT;
    }
    head->ih_hcrc = htonl(checksum);

	head->ih_dcrc = tmp_dcrc;
    checksum = crc32 (0, (postData+8), (fnlen-8));
	/*"软件校验和错误",*/
    if ( checksum != ntohl(head->ih_dcrc)) {
    faultcode = 9019;
	goto OUT;
    }
#elif defined(CONFIG_PPC_85xx)
    cwmp_log_debug("head->ih_hcrc = 0x%x, head->ih_dcrc = 0x%x\n", head->ih_hcrc, head->ih_dcrc);
    checksum = ntohl(head->ih_hcrc);
    head->ih_hcrc = htonl(0);	/* clear for re-calculation */
    cwmp_log_debug("crc32 (0, postData, headLen) = 0x%x\n", crc32 (0, postData, headLen));
	/*"软件校验和错误",*/
    if (crc32 (0, postData, headLen) != checksum) 
    {
        faultcode = 9019;
	    goto OUT;
    }
    head->ih_hcrc = htonl(checksum);

    checksum = crc32 (0, (postData+headLen), (fnlen-headLen));
    cwmp_log_debug("checksum = 0x%x\n", checksum);
	/*"软件校验和错误",*/
    if ( checksum != ntohl(head->ih_dcrc)) 
    {
        faultcode = 9019;
	    goto OUT;
    }
#else
	/*"软件校验和错误",*/
    if (crc32 (0, (const unsigned char *)postData+8, fnlen - 8) != ntohl(head->imageCrc)) {
	cwmp_log_debug("%s-%d: creat_crc=%ld, load_crc=%ld\n",__func__,__LINE__,crc32 (0, (const unsigned char *)postData+8, fnlen - 8),ntohl(head->imageCrc));
    faultcode = 9019;
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
    /* "软件升级失败",*/
    if(ret == -1) 
    {
        ret = 0;
        faultcode = 9019;
        goto OUT;
    }

    if (bRet) 
    {
	updateFlag = 1;/*防止重启过程中，goahead再次收到写flash升级数据包*/
    }
    ret = 1;
OUT:
    if(postData) 
    {
        free(postData);
    }
    if(fp) 
    {
        fclose(fp);
    }
    return faultcode;
}
void ramFree(long long *freeDisk)
{
	FILE *fp = NULL;
	char *p;
	unsigned char buffer[512];
	char tmp[20];

	memset(tmp, '\0', sizeof(tmp));
	doSystem("free > /tmp/free_space");
	fp = fopen("/tmp/free_space", "r");
	if (NULL != fp) {
		fgets(buffer, sizeof(buffer), fp);
		memset(buffer, '\0', sizeof(buffer));
		fgets(buffer, sizeof(buffer), fp);
		fclose(fp);
	}
	p = buffer;
	while (*p == ' ') {
		p++;
	}
	sscanf(p, "%*s %*s %*s %s", tmp);

	*freeDisk = atoll(tmp);
	*freeDisk = *freeDisk * 1024;

	return;
}
void x_reboot(void)
{
#if (WEBAUTH_ACCOUNT == FYES)
    doSystem("rebootS \\&");
#else
    doSystem("reboot&");
#endif
}
#if 0
int main(int argc,char** argv)
{
    if(argc != 2)
    {
        cwmp_log_debug("usage:\n\t%s filename\n","update");
        return 0;
    }
    cwmp_log_debug("filename : %s\n",argv[1]);
    formUpdateFirmware(argv[1]);
    
    return 0;
}
#endif
