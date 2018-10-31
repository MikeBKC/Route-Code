#include "uttMachine.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "daughter.h"
#include <pthread.h>
#include <unistd.h>
#if (UTT_LICENSE == FYES)
//#define LIBLICENSE_DEBUG
#ifdef LIBLICENSE_DEBUG
#define LIC_PRINT(fmt, args...)		printf(fmt, ## args)
#else
#define LIC_PRINT(fmt, args...)
#endif

#define MAX_MODEL_LEN	15
#define MAX_FC_LEN	5
#define MAX_MODEL_COUNT 18
static pthread_t ipsec_task_thread;
typedef struct st_product_fc_t {
    unsigned char model[MAX_MODEL_LEN+1];
    unsigned char fc[MAX_FC_LEN+1];
}Product_fc;
Product_fc model_to_fc[MAX_MODEL_COUNT]={
    {"5830G", "6000"},	    /*lv5830G*/
    {"6830G", "6400"},	    /*lv6830G*/
    {"6530G", "6200"},	    /*lv6530G*/
    {"N8000", "a000"},	    /*lvN8000*/
    {"2200V2", "72"},	    /*lv2200v2*/
    {"4000V2", "74"},	    /*lv4000v2*/
    {"6000V3", "7600"},	    /*lv6000v3*/
    {"3640G", "5c00"},	    /*qv3640G*/
    {"N1800", "8100"},	    /*qvN1800*/
    {"4840G", "6e00"},	    /*qv4840*/
    {"NE1200", "7000"},	    /*qvNE1200*/
    {"4330G", "ae00"},	    /*lv4330G*/
    {"N800", "8200"},	    /*qvN800*/
    {"U4000", "6e00"},	    /*qvU4000*/
    {"1000Nv2", "b6"},    /*qvU1000Nv2*/
    {"U3000", "5c00"},	    /*qvU3000*/
    {"U1900", "6c"},	    /*qv3640G*/
    {"U1100", "42"}	    /*qvU1100*/
    /*如果添加型号，请注意修改MAX_MODEL_COUNT的值*/
};

extern void getCurrentFC(unsigned short int *fc)
{   
    unsigned char tmp_model[MAX_MODEL_LEN+1];
    unsigned char tmp_fc[MAX_FC_LEN+1];
    int i=0; 

    memset(tmp_model, 0, sizeof(tmp_model));
    memset(tmp_fc, 0, sizeof(tmp_fc));
    /*set product model*/
#if(UTT_LV_5830G == FYES)
    strcpy(tmp_model, "5830G");
#elif(UTT_LV_6830G == FYES)
    strcpy(tmp_model, "6830G");
#elif defined(CONFIG_UTT_LV2200)
    strcpy(tmp_model, "2200V2");
#elif defined(CONFIG_UTT_LV4000)
    strcpy(tmp_model, "4000V2");
#elif defined(CONFIG_UTT_LV6000)
    strcpy(tmp_model, "6000V3");
#elif defined(CONFIG_UTT_LVN8000)
    strcpy(tmp_model, "N8000");
#elif(UTT_LV_4330G == FYES)
    strcpy(tmp_model, "4330G");
#elif(UTT_QV_N1800 == FYES)
    strcpy(tmp_model, "N1800");
#elif(UTT_QV_NE1200 == FYES)
    strcpy(tmp_model, "NE1200");
#elif(UTT_U_1100 == FYES)
    strcpy(tmp_model, "U1100");
#elif(UTT_NV_U1000NV2 == FYES)
    strcpy(tmp_model, "1000Nv2");
#elif(UTT_QV_3640 == FYES)
    strcpy(tmp_model, "3640G");
#elif(UTT_QV_4840 == FYES)
    strcpy(tmp_model, "4840G");
#elif(UTT_QV_N800 == FYES)
    strcpy(tmp_model, "N800");
#elif(UTT_U_4000 == FYES)
    strcpy(tmp_model, "U4000");
#elif(UTT_U_3000 == FYES)
    strcpy(tmp_model, "U3000");
#elif(UTT_U_1900 == FYES)
    strcpy(tmp_model, "U1900");
#elif(UTT_LV_6530G == FYES) 
/*将6530G置于最后，防止其他型号的uttMachine.h中叶定义了此型号宏*/
#if 0
    strcpy(tmp_model, "6530G");
#endif
#else
    strcpy(tmp_model, "");
#endif
    /*search prduct fc
     * search product fc in array model_to_fc
     * on the basis of product model*/
    if(fc != NULL)
    {   
	for(i=0;i<MAX_MODEL_COUNT;i++) {
	    if(strcmp(tmp_model, model_to_fc[i].model)==0){
		strcpy(tmp_fc, model_to_fc[i].fc);
		break;
	    }
	}
	if(strlen(tmp_fc)!=0U) {
	    *fc = strtoul(tmp_fc, NULL, 16);
	}
	//printf("%s-%d: tmp_model=%s,tmp_fc=%s,fc=%u\n",__func__,__LINE__,tmp_model,tmp_fc,*fc);
    }
    return;
}

extern char   UTTKEY[10];
/*字符串拷贝*/
extern void _license_strncpy( uchar *des, const uchar *src, uint len )
{
    uint i =0u;
    
    if( 0u != len && NULL != src )
    {
	for( i=0u; i<len; i++ )
	{
	    des[i] = src[i]; 
	}
    }
}

/*功能：随机密钥与采用RSA算法加密D，E，F，G部分生产最后结果的最部分(长度为keylen)加密
 *参数：secretkey 到会密钥加密后的结果
 * */
void _encryptSecretKeyByData( char *secretkey, int keylen, char *key, char *cryptoData, uint cryptoDataLen )
{
	int i;

	for( i=0; i<keylen; i++ )
	{
		secretkey[i] = key[i];
		secretkey[i] ^= cryptoData[ cryptoDataLen - keylen + i ];
	}
	return;	
}

extern u_Word getrouteFC(void)
{
    u_Word uttFC=0U;

#if ((CONFIG_X86_CPU) == 1)
    flash_read_mtdname(LICENSE_FILE, &uttFC, 4, 2);
#else
    flash_read_mtdname("Bootloader", &uttFC, 0x2a, sizeof(uttFC));
#endif
    
    return uttFC;
}
/*hmac_sha1加密*/
void creatPartA( char licensetype, char *partA, char *resultDEFG, uint resultDEFGLen, uchar *key ) 
{
	u_Word	routefc = 0u;			/*路由器的功能号*/
	uint  routesn = 0u;			/*路由器的序列号*/
	uchar	*hmac_sha1data = NULL;		/*hmac_sha1的data*/
	int	hmac_sha1datalen = 0;		/*hmac_sha1的data长度*/
	uchar	*hmac_sha1key = NULL;		/*hmac_sha1的key*/
	int	hmac_sha1keylen = 0;		/*hmac_sha1的key长度*/
	uchar	fc_buf[FCLEN] = {0};
	uchar	sn_buf[SNLEN] = {0}; 

	/*hmac_sha1的data*/
	//routefc = getrouteFC();
	getCurrentFC(&routefc);
	//sprintf( (char *)fc_buf, "%04x", routefc );
	osSprintf( (char *)fc_buf, (char*)(&routefc), FCLEN );
	//Print( (char*)fc_buf, 4 );
	//printf("deci_fc:%x\n", routefc);
	hmac_sha1datalen = resultDEFGLen + UTTKEYLEN + FCLEN;
	hmac_sha1data = ( uchar * )malloc( hmac_sha1datalen + 1 );
	if( NULL == hmac_sha1data )
	{
		printf( "Memory craeted failed !!!\n" );	
		return;
	}
	memset( hmac_sha1data, 0, ( hmac_sha1datalen + 1 ) );
	_license_strncpy( hmac_sha1data, (uchar*)resultDEFG, resultDEFGLen );
	_license_strncpy( ( hmac_sha1data + resultDEFGLen ), (uchar*)UTTKEY, UTTKEYLEN );
	_license_strncpy( ( hmac_sha1data + resultDEFGLen + UTTKEYLEN ), fc_buf, FCLEN );

	/*hmac_sha1的key*/
	//routesn = getSystemVersionHandle()->serialNumber;	    /*路由的序列号*/
	routesn = getSystemSn();	    /*路由的序列号*/
#if (UTT_LICENSE_10 == FYES)
	sprintf( (char *)sn_buf, "%10d", routesn );
#else
	sprintf( (char *)sn_buf, "%08d", routesn );
#endif
	//Print( (char*)sn_buf, 8 );
	//printf("deci_sn:%x\n", routesn);
	hmac_sha1keylen = PARTCLEN + SNLEN + 1;
	hmac_sha1key = ( uchar * )malloc( hmac_sha1keylen + 1 );
	if( NULL == hmac_sha1key )
	{
		printf( "Memory craeted failed !!!\n" );	
		return;
	}
	memset( hmac_sha1key, 0, ( hmac_sha1keylen + 1 ) );
	_license_strncpy( hmac_sha1key, key, PARTCLEN );
	_license_strncpy( ( hmac_sha1key + PARTCLEN ), sn_buf, SNLEN );
	hmac_sha1key[ PARTCLEN + SNLEN ] = (uchar)licensetype;

	int tmp_i = 0;
	LIC_PRINT("fc_buf =%s\n",fc_buf);
	LIC_PRINT("sn_buf =%s\n",sn_buf);
	LIC_PRINT("hmac_sha1data =");
	for (tmp_i =0; tmp_i<hmac_sha1datalen;tmp_i++) {
	    LIC_PRINT("%x ",hmac_sha1data[tmp_i]);
	}
	LIC_PRINT("  hmac_sha1datalen =%d\n",hmac_sha1datalen);
	LIC_PRINT("hmac_sha1key =");
	for (tmp_i =0; tmp_i<hmac_sha1keylen;tmp_i++) {
	    LIC_PRINT("%x ",hmac_sha1key[tmp_i]);
	}
	LIC_PRINT("  hmac_sha1datalen =%d\n",hmac_sha1keylen);
	/*用hmac_sha1加密*/
	hmac_sha1( hmac_sha1data, hmac_sha1datalen, hmac_sha1key, hmac_sha1keylen, ( uchar * )partA );
	free( hmac_sha1data );
	free( hmac_sha1key );
	return;
}

/*功能：把E部分的内容与随机密钥key加密
 *
 * */
void _encrptPartEByRandomKey( char *key, int keylen, char *partE )
{
	int i;

	if( keylen < 6 )	
	{
		printf("随机密钥长度小于6\n");
	}
	else
	{
		for( i=0; i<PARTELEN ; i++ )
		{
			partE[i] ^= key[i+2];
		}
	}
	return ;
}

/*功能：检测partE前两个字节的某位为1时，对应的后两个字节的位是否也为1
 *
 * */
int check_partE( char *partE )
{
	int i, j;
	char partE_front, partE_back;

	for( j=0; j<2; j++ )
	{
		partE_front = partE[j];
		partE_back = partE[j+2];

		for( i=0; i< 8; i++ )
		{
			/*检测当每个信息检测标识位为1时，对应的信息包含标识位是否也为1*/
			if( ( 0x01 == ( partE_front & 0x01 ) ) && ( 0x01 != ( partE_back & 0x01 ) ) )
			{
				return FALSE;
			}
			partE_front >>= 1;
			partE_back >>= 1;
		}
	}
	return TRUE;
}

void Print( char *buf, int len )
{
    int i = 0;

    printf("\n");
    for( i=0; i<len; i++ )
    {
	if( i%10 == 0 )
	{
	    LIC_PRINT("+++++++++++++++\n");
	}
	LIC_PRINT("Dec:%4d, Hex:%2x\n", ( uchar )buf[i], ( uchar )buf[i]);
    }
}

/*以字符保存自字符的ASCII码*/
extern void osSprintf( char *des, char *src, int len )
{
    int i=0;
    char tmp;

    for( i=0; i<len; i++ )
    {
	tmp = src[i];	
	if( ( tmp & 0x0f ) > 0x09 )
	{
	    des[2*i+1] = 'a' + ( ( tmp & 0x0f ) - 0x0a );
	}
	else
	{
	    des[2*i+1] = '0' + ( tmp & 0x0f );
	}
	if( ( ( tmp & 0xf0 ) >> 4 ) > 0x09 )
	{
	    des[2*i] = 'a' + ( ( ( tmp & 0xf0 ) >>4 ) - 0x0a );
	}
	else
	{
	    des[2*i] = '0' + ( ( tmp & 0xf0 ) >>4 );
	}
    }
}

extern void saveLicenseRuntime(uint time)
{
    uchar cryptoRuntime[17]={0};
    uchar timetochar[9]={0};
    char *saveRuntimeData=NULL;
    int len=0;
    int writelen=0;
    char *licensebuf=NULL;
    char partAB[51];
    LicenseProfile *uttlicense=NULL;

    uttlicense = (LicenseProfile*)ProfGetInstPointByIndex(MIB_PROF_LICENSE, 0);
    /* 解析license的长度
     * 长度保存在B部分*/
#if 0
    memset(partAB, 0, sizeof(partAB));
    flash_read_mtdname("/dev/sda4", partAB, 4+8, 50);
    len = Decipher_PartB((char *)partAB);
#endif 
    len = uttlicense->len;
    licensebuf = (char *)malloc(len*2+1);
    if(licensebuf == NULL)
   	return;
    memset(licensebuf,0,len*2+1);
   /*get license content from flash*/
#if ((CONFIG_X86_CPU) == 1)
    flash_read_mtdname(LICENSE_FILE, licensebuf, 4+8, len*2);
#else
    flash_read_mtdname("License", licensebuf, 4, len*2);
#endif
    LIC_PRINT("%s-%d: license len = %d, licensebuf=%s\n",__func__,__LINE__,len,licensebuf);
    
    saveRuntimeData = (char *)malloc(len*2+16+8+1);
    if(saveRuntimeData == NULL)
	return;
    memset(saveRuntimeData,0,len*2+16+8+1);
    writelen = len*2+16+8;
    /* save runtime content 
     * license content + runtime + hnac_md5 
     */
    /*change time to hex*/
    sprintf((char *)timetochar,"%08x",time);
    LIC_PRINT("%s-%d: timetochar = %s\n",__func__,__LINE__,timetochar);
    hmac_md5((uchar *)uttlicense->key,10,timetochar,strlen(timetochar),cryptoRuntime); /*the result of hmac_md5*/
    //LIC_PRINT("%s-%d: cryptoRuntime = %s\n",__func__,__LINE__,cryptoRuntime);
    //_printarray( (char *)cryptoRuntime, 16 );
    memcpy(saveRuntimeData,licensebuf,len*2);
    memcpy(saveRuntimeData+len*2,(const char*)timetochar,8u);
    memcpy(saveRuntimeData+len*2+8,(const char*)cryptoRuntime,16u);
    LIC_PRINT("%s-%d: saveRuntimeDate = %s, len=%d, writelen=%d\n",__func__,__LINE__,saveRuntimeData,strlen(saveRuntimeData),writelen);
#if 1
#if ((CONFIG_X86_CPU) == 1)
    flash_write_mtdname(LICENSE_FILE, saveRuntimeData, 4+8, writelen);
    flash_write_mtdname(LICENSE_FILE, saveRuntimeData, 0x10000, writelen);
#else
    flash_write_mtdname("License", saveRuntimeData, 4, writelen);
    flash_write_mtdname("License", saveRuntimeData, 0x10000, writelen);
#endif
#endif
    LIC_PRINT("%s-%d: savetime done.\n",__func__,__LINE__);

    free(licensebuf);
    free(saveRuntimeData);
    return;
}

/* 
 * getRuntimeFromFlash
 * get license runtime saved in flash
 * output: time
 * invoked by licenseAuthMain()
 */
void getRuntimeFromFlash(uint *time)
{
    char runtime[9]={0};
    uint timetmp;
    char partAB[51];
    char *licensebuf = NULL;
    int len = 0;
    
    /* 解析license的长度
     * 长度保存在B部分*/
    memset(partAB, 0, sizeof(partAB));
#if ((CONFIG_X86_CPU) == 1)
    flash_read_mtdname(LICENSE_FILE, partAB, 4+8, 50);
#else
    flash_read_mtdname("License", partAB, 4, 50);
#endif
    len = Decipher_PartB((char *)partAB);
    
    licensebuf = (char *)malloc(len*2+8+1);
    if(licensebuf == NULL)
   	return;
    memset(licensebuf,0,len*2+8+1);
   /*get license content from flash*/
#if ((CONFIG_X86_CPU) == 1)
    flash_read_mtdname(LICENSE_FILE, licensebuf, 4+8, len*2+8);
#else
    flash_read_mtdname("License", licensebuf, 4, len*2+8);
#endif
    memset(runtime, 0, sizeof(runtime));
    memcpy(runtime,(char *)(licensebuf + len*2 ),8);
    LIC_PRINT("%s-%d: license len = %d, licensebuf=%s,runtime=%s\n",__func__,__LINE__,len,licensebuf,runtime);
    runtime[8]=0;
    timetmp = strtol(runtime,NULL,0x10);
    //timetmp = runtime[0]*1+runtime[1]*256+runtime[2]*256*256+runtime[3]*256*256*256;
    *time = timetmp;
    
    if(NULL != licensebuf)
    {
	free(licensebuf);
    }
    
    return;
}

static void ipsecUpdateTask(void)
{
	LicenseProfile *uttlicense=NULL;
    
	uttlicense = (LicenseProfile*)ProfGetInstPointByIndex(MIB_PROF_LICENSE, 0);
	uttlicense->taskRunning = TRUE;
	LIC_PRINT("%s-%d: authResult=%d, taskRunning=%d\n",__func__,__LINE__,uttlicense->authResult,uttlicense->taskRunning);
	pthread_detach(pthread_self());/*线程退出后立即释放资源.若指定pthread_join可不用此举*/
	while(1)
	{
	    /*update every one hour*/
	    sleep(HOUR);
	    /* check authResult cnd license status
	     * if authResult is 0xff and timeflag is forever
	     * init taskRunning id false
	     */
	    if(uttlicense->authResult != 0xff || uttlicense->timeflag == FOREVER)
	    {
		uttlicense->taskRunning = FALSE;
		LIC_PRINT("%s-%d: authResult=%d, taskRunning=%d\n",__func__,__LINE__,uttlicense->authResult,uttlicense->taskRunning);
		pthread_exit(NULL);/*放在循环开头退出。保证锁资源被正常释放*/
	    }
	    /* one hour later
	     * runtime +1
	     * and save new runtime
	     */
	    uttlicense->runtime++;
	    saveLicenseRuntime(uttlicense->runtime);
	    /*check runtime and impowertime*/
	    if(uttlicense->runtime >= uttlicense->impowerTime * 30 * 24)
	    {
		uttlicense->authResult = 0;
		uttlicense->isOutdate = 1;

		uttlicense->taskRunning = FALSE; 
		LIC_PRINT("%s-%d: authResult=%d, taskRunning=%d\n",__func__,__LINE__,uttlicense->authResult,uttlicense->taskRunning);
		pthread_exit(NULL);/*放在循环开头退出。保证锁资源被正常释放*/
	    }
	    //getRuntimeFromFlash(&gettime);
	    LIC_PRINT("%s-%d: authResult=%d, taskRunning=%d, runtime=%d\n",__func__,__LINE__,uttlicense->authResult,uttlicense->taskRunning,uttlicense->runtime);
	}
	return;
}
/*
 * void ipsecTaskInit(void)
 * 
 * 创建一个线程 来 统计更新license的运行时间
 */
void ipsecTaskInit(void)
{
	if( pthread_create(&ipsec_task_thread, NULL, (void*)ipsecUpdateTask, NULL) != 0) 
	{
	    printf("Create threat for poll task error!\n");
	}
    
    return;
}

/*
 * _judgeUttSN(void)
 * 判断设备序列号是否在受控的范围内
 */
extern int _judgeUttSN(void)
{
    uint routesn = 0U;
    int ret = FALSE;

    /*get utt device sn*/
    routesn = getSystemSn();
    /*judge device sn */
    if(routesn >= 13170001U) {
	ret = TRUE;
    }
    LIC_PRINT("%s-%d: routesn = %u, ret = %d\n",__func__,__LINE__,routesn,ret);
    return ret;
}

#if 0
extern Bool checkRuntime()
{
    uint time;
    uchar timetochar[9] = {0};
    uchar cryptoRuntime[17]={0};
    uchar getcryptoRuntime[17]={0};
getRuntimeFromFlash(&time);
    sprintf((char *)timetochar,"%08x",time);
    hmac_md5((uchar *)uttlicense.key,10,timetochar,strlen(timetochar),cryptoRuntime);
#if(TWO_FLASH == FYES)
    memcpy(getcryptoRuntime,(char *)(SECOND_FLASH_END_ADDR - FLASHREGIONSIZE + uttlicense.len*2 + 8),16);
#else
    memcpy(getcryptoRuntime,(char *)(BOOT_ADDR + FLASH_SIZE- FLASHREGIONSIZE + uttlicense.len*2 + 8),16);
#endif
    //printf("get cryptruntime = %s\n",getcryptoRuntime);
    if(strcmp((const char*)cryptoRuntime,(const char*)getcryptoRuntime) == 0)
	return TRUE;
    else
	return FALSE;
    
}
#endif

#endif
