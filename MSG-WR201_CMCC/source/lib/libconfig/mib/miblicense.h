#ifndef LICENSE_H
#define LICENSE_H
#include "mibtype.h"
#include "mibunion.h"
#include "miblicenseShare.h"

#if (UTT_LICENSE == FYES)
# include "stdio.h"
# include "stdlib.h"
# include "string.h"
# include "time.h"
# include "unistd.h"

#define LICENSECHANGED 		1
#define TRUE 			1
#define FALSE 			0
#define PARTALEN 		20		/*license的A部分长度，表示字节数*/
#define PARTBLEN 		2
#define PARTDLEN 		1
#define PARTELEN 		4
#define TIME_CONTROL 		0x01		/*时间控制*/
#define FUNCTION_CONTROL 	0x02		/*功能控制*/	
#define TERRACE_CONTROL 	0x03		/*平台控制*/
#define TIME_VALUE_LEN 		0x0006		/*时间对应的长度*/

/*license状态*/
#define FOREVER 		0x02		/*永久*/
#define EFFECTIVE 		0x01		/*临时*/
#define LAWLESS 		0x00		/*非法*/

#define HOUR			(60*60)

#define UTTKEYLEN		0x0a 
#define FCLEN			4 

#if (UTT_LICENSE_10 == FYES)
#define SNLEN			10 
#else
#define SNLEN			8 
#endif

#define TYPENUM			0x03 
#define IPLEN			0x04 
#define USERMESSAGELEN		12

#define LICENSE_BIG_ENDIAN	1
#define LICENSE_LITTLE_ENDIAN	0

//#define _LICENSEPRINT( x )   do{ if( _LicenseDumping ){ printf x ; } }while( 0 )

//#define UTTKEY			"AITAIKEJI"	/*utt保留字段*/

typedef unsigned short int  u_Word;
#if defined(CONFIG_BOARD_MTK7620_E) || defined(CONFIG_BOARD_MTK7620_EN)
#define uint unsigned int
#else
typedef unsigned int	    uint;
#endif
//typedef unsigned long	    uint32;

typedef struct st_license
{
	int 	   	 len;				/*license整个加密文件的长度，表示字节数*/
 	char       	 key[ PARTCLEN + 1 ];   	/*随机密钥*/
	int	   	 controltype;			/*license控制类型*/
        uint		 impowerTime;    		/*临时授权时间*/
	int	   	 timeflag;			/*license状态*/
        uint     	 runtime;               	/*license已使用时间*/
        //volatile int    taskRunning;
	volatile int	 isOutdate;			/*临时license过期 or 未过期，0-未过期，1-过期*/
        //int        	 updatenum;             	/*带入失败次数*/
	uchar 		 num;				/*license序号*/
	uint 		 ip;
	char		 mac[ MACLEN + 1 ];
	char 		 userinfor[ MAXUSERLEN +1 ];	/*用户信息*/
	int		 checkip;			/*是否检测ip*/
	int 		 checkmac;			/*是否检测Mac*/
	int		 checkuser;			/*是否检测用户信息*/
	int		 includeip;			/*F是否包含ip*/
	int 		 includemac;			/*F是否包含Mac*/
	int		 includeuser;			/*F是否包含用户信息*/


}_st_license;

extern LicenseProfile uttlicense;
extern volatile uchar authResult;
extern int needlicense;
extern int _LicenseDumping;	
extern int taskRunning;

void
hmac_md5(
    unsigned char*  text,                /* pointer to data stream */
    int             text_len,            /* length of data stream */
    unsigned char*  key,                 /* pointer to authentication key */
    int             key_len,             /* length of authentication key */
    uchar*         digest);             /* caller digest to be filled in */



void
hmac_sha1(
    unsigned char*  text,                /* pointer to data stream */
    int             text_len,            /* length of data stream */
    unsigned char*  key,                 /* pointer to authentication key */
    int             key_len,             /* length of authentication key */
    uchar*         digest);             /* caller digest to be filled in */

/*licenseData中指向的内容为经过运算后的*/
void cryptoLicenseDEFGPart(char *randomKey, int keyLen, 
        char* licenseData, int licenseLen);


/*licenseData中指向的内容为经过运算后的*/
void decryptoLicenseDEFGPart(char *randomKey, int keyLen, 
        char* licenseData, int licenseLen);
int DecipherUTTLicense( LicenseProfile *uttlicense, char *licenseaddr );
int  getIntactLicense( char **buf, int len, char *license_addr );
extern int Decipher_PartB( char *license_addr );

extern void licenseAuthMain();
//extern int LicenseWriteToFlash(char *licensedata,int fnlen);
void getRuntimeFromFlash(uint *time);
extern void saveLicenseRuntime(uint time);
extern void LicenseUpdateRunTimeTask();

extern int  license_endian;
extern int isBigEndian( void );
extern uint _license_ntohl( uint data );
extern u_Word _license_ntohs( u_Word data );

extern int check_partE( char *partE );
extern void _license_strncpy( uchar *des, const uchar *src, uint len );
void _encryptSecretKeyByData( char *secretkey, int keylen, char *key, char *cryptoData, uint cryptoDataLen );
void creatPartA( char licensetype, char *partA, char *resultDEFG, uint resultDEFGLen, uchar *key );
void _encrptPartEByRandomKey( char *key, int keylen, char *partE );
void Print( char *buf, int len );
extern void osSprintf( char *des, char *src, int len );
extern void Print_license( LicenseProfile *license );
extern void ipsecTaskInit(void);
extern int _judgeUttSN(void);
#endif
#endif
