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
#define PARTALEN 		20		/*license��A���ֳ��ȣ���ʾ�ֽ���*/
#define PARTBLEN 		2
#define PARTDLEN 		1
#define PARTELEN 		4
#define TIME_CONTROL 		0x01		/*ʱ�����*/
#define FUNCTION_CONTROL 	0x02		/*���ܿ���*/	
#define TERRACE_CONTROL 	0x03		/*ƽ̨����*/
#define TIME_VALUE_LEN 		0x0006		/*ʱ���Ӧ�ĳ���*/

/*license״̬*/
#define FOREVER 		0x02		/*����*/
#define EFFECTIVE 		0x01		/*��ʱ*/
#define LAWLESS 		0x00		/*�Ƿ�*/

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

//#define UTTKEY			"AITAIKEJI"	/*utt�����ֶ�*/

typedef unsigned short int  u_Word;
#if defined(CONFIG_BOARD_MTK7620_E) || defined(CONFIG_BOARD_MTK7620_EN)
#define uint unsigned int
#else
typedef unsigned int	    uint;
#endif
//typedef unsigned long	    uint32;

typedef struct st_license
{
	int 	   	 len;				/*license���������ļ��ĳ��ȣ���ʾ�ֽ���*/
 	char       	 key[ PARTCLEN + 1 ];   	/*�����Կ*/
	int	   	 controltype;			/*license��������*/
        uint		 impowerTime;    		/*��ʱ��Ȩʱ��*/
	int	   	 timeflag;			/*license״̬*/
        uint     	 runtime;               	/*license��ʹ��ʱ��*/
        //volatile int    taskRunning;
	volatile int	 isOutdate;			/*��ʱlicense���� or δ���ڣ�0-δ���ڣ�1-����*/
        //int        	 updatenum;             	/*����ʧ�ܴ���*/
	uchar 		 num;				/*license���*/
	uint 		 ip;
	char		 mac[ MACLEN + 1 ];
	char 		 userinfor[ MAXUSERLEN +1 ];	/*�û���Ϣ*/
	int		 checkip;			/*�Ƿ���ip*/
	int 		 checkmac;			/*�Ƿ���Mac*/
	int		 checkuser;			/*�Ƿ����û���Ϣ*/
	int		 includeip;			/*F�Ƿ����ip*/
	int 		 includemac;			/*F�Ƿ����Mac*/
	int		 includeuser;			/*F�Ƿ�����û���Ϣ*/


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

/*licenseData��ָ�������Ϊ����������*/
void cryptoLicenseDEFGPart(char *randomKey, int keyLen, 
        char* licenseData, int licenseLen);


/*licenseData��ָ�������Ϊ����������*/
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
