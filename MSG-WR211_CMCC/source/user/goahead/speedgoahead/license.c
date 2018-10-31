#include "string.h"
#include "mib.h"
#include "profacce.h"
#include "flash_api.h"
#include "internet.h"

#if( UTT_LICENSE == FYES )

//#define LICENSE_DEBUG
#ifdef LICENSE_DEBUG
#define LIC_PRINT(fmt, args...)		printf(fmt, ## args)
#else
#define LIC_PRINT(fmt, args...)
#endif

extern void _printarray( char *buf, int len );
void Print( char *buf, int len );

/*global varliable*/
int needlicense = 0;

static unsigned char license_exist_flag[4]={0x32, 0x30, 0x31, 0x32}; 
/*PART_C*/

/*���ܣ����������Կ����ת��Ϊ���ֶ�Ӧ���ַ�������key�С�
 * ������len key�Ŀռ��С
 * */
//extern unsigned long random( unsigned long   max );
static void _creatRandomSecretKey( char *key, int keylen )
{	
	int i;

	for( i=0; i<keylen; i++ )
	{
		key[i] = random()%0xff;				/*ȡ����Ӧλ�ϵ�����*/
	}
	return ;
}


/*PART_D*/

/*license���*/

static char getLicenseSequNum( void )
{
	return 0x00;
}

/*���ܣ������key���ַ�byte(D����)����
 *
 * */
static uchar _encryptAByteByRandomKey( char *key, int keylen, char byte )
{
	int i=0;

	for( i=0; i<keylen; i++ )
	{
		 byte ^= key[i];
	}
	return (uchar)byte;
}


/*PART_E*/

/*E���ֵ���Ϣ*/

static void getLicensePartE( char *originE )
{
	originE[0] = 0x00;
	originE[1] = 0x00;
	originE[2] = 0x00;
	originE[3] = 0x00;
}



void creatPartE( char *buf, char *key )
{

	getLicensePartE( buf );
	if( check_partE( buf ) )
	{
		_encrptPartEByRandomKey( key, PARTCLEN, buf );
	}
	else
	{
		printf( "The massage of partE is wronge in the license!!!\n" );
	}
	//Print( buf, PARTELEN );
	return ;
}

/*PART_F*/

int  ip  = 0xc0a81001;
char mac[ MACLEN + 1 ] = { '1','a','2','b','3','c','4','d','5','e','6','f','\0' };
char usermessage[ USERMESSAGELEN + 1 ] =  {'�','�','�','�','�','�','�','�','�','�','�','�','\0'};

int AddInformation( char **partF, int len, char *src, int srcsize )
{
    char *tmppartF = NULL;

    tmppartF = ( char * )malloc( len + srcsize + 2);
    if( NULL == tmppartF)
    {
	printf( "REALLOC was failed!!!\n" );
	return FALSE;
    }
    if( NULL != *partF )
    {
	_license_strncpy( (uchar*)tmppartF, (uchar*)*partF, (uint)len );
	free( *partF );
    }
    *partF = tmppartF;
    ( *partF )[ len + 1 ] = srcsize;
    _license_strncpy( (uchar*)( *partF + ( len + 2 ) ), (uchar*)src, ( uint )srcsize );
    len += (srcsize + 2 );
    return len;
}

static int getlicensePartF( char **partF )
{
    char originE[ PARTELEN + 1 ] = {'0'};	/*license�����E����*/
    char buf[2] = {'0'};
    int	 i = 0;
    //int	 *tmp = NULL;
    int  len = 0;

    getLicensePartE( originE );
    if( 0 !=  *( (int*)originE ) )
    {
    _license_strncpy( (uchar*)buf, (uchar*)( originE+2 ), 2u );
    for( i=0; i<16; i++ )
    {
	if( 0x01 == ( buf[ i/8 ] & 0x01 ) )
	{
	    switch( i )
	    {

		/*F���ְ���ip*/
		case 0:
		    ip = _license_ntohl( ip );
		    len = AddInformation( partF, len, (char *)(&ip), IPLEN );
		    break;
		    /*����mac*/
		case 1:	
		    len = AddInformation( partF, len, mac, MACLEN );
		    break;
		    /*�����û���Ϣ*/
		case 2:	
		    len = AddInformation( partF, len, usermessage, USERMESSAGELEN );
		    break;
		default:
		    printf( "The part F of license is wrong!!!\n" );
		    break;
	    }
	}
	buf[ i/8 ] >>= 1; 
    }
    }
    return len;
}

/*PART_G*/

static char getLicenseType( void )
{
	return TIME_CONTROL;
}
/*license����Ȩʱ��*/
static void getLicenseServicetime( char *licensetime )
{
	licensetime[0] = '0';	/*��λ*/
	licensetime[1] = '0';
	licensetime[2] = '1';
}

/*���ܣ�����Կkey����license��ʱ��
 *������buf ���ؼ��ܺ��ֵ
 * */
static void  _encryptLicenseTime( uchar *date, uchar *key, uchar *buf ) 
{
	/*���ַ���ʽ����ʱ���ASCII��*/
	osSprintf( (char*)buf, (char*)date, 3 );
	buf[0] ^= key[3];
	buf[1] ^= key[5];
	buf[2] ^= key[4];
	buf[3] ^= key[1];
	buf[4] ^= key[0];
	buf[5] ^= key[2];
	return ;
}

int creatPartG( char **buf, char *key )
{
	char controltype;			/*license��������*/
	char licensetime[4] = {'0'};		/*license��ʹ������*/
	int  len = 0;

	controltype = getLicenseType();
	if( TIME_CONTROL == controltype )
	{
		len += 9;
		*buf = ( char *)malloc( sizeof(char) * ( 1+2+TIME_VALUE_LEN +1 ) );
		if( NULL == *buf )
		{
		    printf("Memory created failed!!!\n"); 
		    return FALSE;
		}
		(*buf)[ 1+2+TIME_VALUE_LEN ] = '\0';
		(*buf)[0] = TIME_CONTROL;
		(*buf)[1] = ( TIME_VALUE_LEN & 0x00ff );
		(*buf)[2] = ( ( TIME_VALUE_LEN & 0xff00 ) >> 8 );  
	}
	else
	{
		printf("UTT don't have the type of license!!!\n");
		return FALSE;
	}
	getLicenseServicetime( licensetime );
	_encryptLicenseTime( (uchar*)licensetime, (uchar*)key, (uchar*)( *buf+3 ) );
	return len;
}

/*���license��DEFG����*/
int creatpartDEFG( char **DEFGPart, uchar *partD, char *partE, char *partF, char *partG, int FLen, int GLen )
{
	int  Len = 0;			/*DEFG�ĳ���*/
	int  i = 0;

	Len = PARTDLEN + PARTELEN + FLen + GLen;
	*DEFGPart = ( char * )malloc( Len + 1 );
	if( NULL == *DEFGPart )
	{
		printf( "Memory craeted failed !!!\n" );	
		return 0;
	}
	(*DEFGPart)[ Len ] = '\0';
	_license_strncpy( (uchar*)*DEFGPart , partD, PARTDLEN );
	_license_strncpy( ( (uchar*)*DEFGPart  + PARTDLEN ), (uchar*)partE, PARTELEN );
	if( NULL != partF )
	{
	    _license_strncpy( ( (uchar*)*DEFGPart  + PARTDLEN + PARTELEN ), (uchar*)partF, FLen );
	}
	for( i=0; i<GLen; i++ )
	{
	    (*DEFGPart)[PARTDLEN + PARTELEN + FLen + i] = partG[i];
	}
	//printf("DEFGPart_data_make:%s\n", DEFGPart);
	return Len;
}

/*����license�ĳ���*/
static void _encryptLicenseLen( int len, char *key, char *buf )
{
	if( 0xffff >= len )
	{
    		buf[1] = ( len & 0xff );
   		buf[0] = ( ( len>>8 ) & 0xff );
	}
	else
	{
		printf("The len of license is wrong!!!\n");
	}
	return;
}

extern u_Word getrouteFC(void);
//extern const SystemVersion * getSystemVersionHandle( void );


/*��licensedata�ַ���Ӧ��ASCII�뱣�棬����16���Ʊ�ʾ*/
static void saveuttlicenseByHex( char **buf, int licenselen, char *partA, char *partB, char *partC, char *DEFGPart, int len )
{
	uchar	*licensedata = NULL;		/*����license���ܺ������*/

	licensedata = ( uchar *)malloc( licenselen +1 );
	*buf = ( char *)malloc( licenselen*2 +1 );
	if( NULL == licensedata || NULL == *buf )
	{
		printf( "Memory craeted failed !!!\n" );	
		return;
	}
	licensedata[ licenselen ] = '\0';
	( *buf )[ licenselen * 2 ] = '\0';
	_license_strncpy( licensedata, (uchar*)partA, (uint)PARTALEN );
	_license_strncpy( ( licensedata + PARTALEN ) , (uchar*)partB, (uint)PARTBLEN );
	_license_strncpy( ( licensedata + PARTALEN +PARTBLEN ), (uchar*)partC, (uint)PARTCLEN );
	_license_strncpy( ( licensedata + PARTALEN +PARTBLEN + PARTCLEN ), (uchar*)DEFGPart, (uint)len );

       	/*��16���Ʊ���license*/	
	//Print( (char*)licensedata, licenselen );
	osSprintf( *buf, (char*)licensedata, licenselen );
	//printf("flash_buf_make:%s\n", *buf );
	free( licensedata );
    	return ;
}


/*����license*/
static void _creatuttlicense( char **buf, int *len )
{
	char  partA[ PARTALEN + 1 ] = {'0'};	/*hmac_sha1�Ľ��*/
	char  partB[ PARTBLEN + 1 ] = {'0'};	/*���ܺ��license����*/
	char  partC[ PARTCLEN + 1 ] = {'0'};	/*���ܺ������Կ*/
	uchar  partD;				/*���ܺ��license���*/
	char  partE[ PARTELEN + 1 ] = {'0'};	/*���ܺ��E��������*/
	char *partF = NULL;			/*ָ��F���ֵ�����*/
	char *partG = NULL;			/*ָ��G���ֵ�����*/
	int  DEFGPartLen = 0;			/*D,E,F,G���ֽں�*/
	char *DEFGPart = NULL;			/*ָ��д��D,E,F,G��������*/
	int  licenselen = 0u;			/*LICENSE_LEN*/
	char randkey[ PARTCLEN + 1 ] = {'0'};	/*�����Կ*/
	char licensenum;			/*License���*/
	int  partFLen = 0;
	int  partGLen = 0;
	char controltype;			/*license��������*/

	/*PART_C*/
	_creatRandomSecretKey( randkey, PARTCLEN );
	//printf("\nsecretkey_make:\n");
	//Print( randkey, PARTCLEN );
	

	/*PART_D*/
	licensenum = getLicenseSequNum();
	partD =  _encryptAByteByRandomKey( randkey, PARTCLEN, licensenum );
	//printf("partD_make:%x\n", partD);

	/*PART_E*/
	creatPartE( partE, randkey );
	
	/*PART_F*/
	partFLen = getlicensePartF( &partF );
	
	/*PART_G
	 *Ŀǰ�ݿ���ʱ�����
	 * */
	partGLen = creatPartG( &partG, randkey );

	/*����DEFG����*/
	DEFGPartLen = creatpartDEFG( &DEFGPart, &partD, partE, partF, partG, partFLen, partGLen );
	cryptoLicenseDEFGPart( randkey, PARTCLEN, DEFGPart, DEFGPartLen );

	/*���������Կ*/
	_encryptSecretKeyByData( partC, PARTCLEN, randkey, DEFGPart, DEFGPartLen );

	/*PART_B
	 * ����license�ĳ���*/
	licenselen = PARTALEN + PARTBLEN + PARTCLEN + PARTDLEN + PARTELEN + partFLen + partGLen;
	*len = licenselen * 2;
	_encryptLicenseLen( licenselen, randkey, partB );

	/*PART_A*/

	controltype = getLicenseType();
	//printf( "license_partA_make\n" );
	creatPartA( controltype, partA, DEFGPart, ( uint )DEFGPartLen, ( uchar * )randkey );

	/*���������ļ��ܺ��license*/
	saveuttlicenseByHex( buf, licenselen, partA, partB, partC, DEFGPart, DEFGPartLen );

	if( NULL != partF )
	{
	    free( partF );
	}
	if( NULL != partG )
	{
	    free( partG );
	}
	if( NULL != DEFGPart )
	{
	    free( DEFGPart );
	}
}

extern  int Decipher_PartB( char *license_addr );

extern int _judgeNeedCreatTmpLicense()
{
    unsigned char buffer[21]={0};
    int i;

    memset(buffer, 0, sizeof(buffer));
#if (CONFIG_X86_CPU == 1)
    flash_read_mtdname(LICENSE_FILE, (char *)buffer, 4+4, 20);
#else
    flash_read_mtdname("License", (char *)buffer, 0, 20);
#endif
    LIC_PRINT("%s-%d: buffer=%s\n",__func__,__LINE__,buffer);
    for(i=0;i<4;i++)
    {
	printf("%s-%d: buffer[%d]=0x%x\n",__func__,__LINE__,i,buffer[i]);
	if(buffer[i] != license_exist_flag[i])
	{
	    LIC_PRINT("need create templicense\n");
	    return 1;
	}
    }
    return 0;
}

/*
 * void licenseAuthMain(void)
 * ipsec ����������
 */
int licenseSetValid = 0;
void licenseAuthMain(void)
{
    char    partAB[51];
    char    *buf=NULL;
    char    *content=NULL;
#if 1
    char    *licensebuf=NULL;
#endif
    int	    retneed;
    int	    len = 0;
    int	    buflen = 0;
    int	    ret=0;
    uint  getruntime=0;
    LicenseProfile *uttlicense=NULL;

    uttlicense = (LicenseProfile*)ProfGetInstPointByIndex(MIB_PROF_LICENSE, 0);
    
    if(_judgeUttSN() != FALSE) {
    LIC_PRINT("need license control.\n");

    /*�ж��Ƿ���Ҫ������ʱlicense*/
    retneed = _judgeNeedCreatTmpLicense();
    LIC_PRINT("%s-%d: retneed=%d\n",__func__,__LINE__,retneed);
#if 0
    retneed = TRUE;
#endif
    if(retneed == TRUE)
    {
	printf("need tmp license.\n");
	_creatuttlicense(&buf,&buflen);
	if((buf!=NULL)&&(uttlicense!=NULL)&&(buflen!=0))
	{
#if 1
	    /*��license֮ǰ���"2012", ���Ա�ʾflash���Ѵ���license*/
	    licensebuf = (char *)malloc(buflen+5);
	    if(licensebuf == NULL)
	    {
		LIC_PRINT("%s-%d: malloc error\n",__func__,__LINE__);
		return;
	    }
	    memset(licensebuf, 0, buflen+5);
	    memcpy(licensebuf, license_exist_flag, 4);
	    memcpy(licensebuf+4, buf, buflen);
	    LIC_PRINT("%s-%d: licensebuf=%s\n",__func__,__LINE__,licensebuf);
#endif
	    LIC_PRINT("%s-%d: license_buf=%s\n",__func__,__LINE__,buf);
#if (CONFIG_X86_CPU == 1)
	    ret = flash_write_mtdname(LICENSE_FILE, licensebuf, 4+4, buflen+4);
	    flash_write_mtdname(LICENSE_FILE, buf, 0x10000, buflen);
#else
	    ret = flash_write_mtdname("License", licensebuf, 0, buflen+4);
	    flash_write_mtdname("License", buf, 0x10000, buflen);
#endif
	    if(ret == -1) {
		printf("Could not open mtd device or Write license error.\n");
		free(buf);
		free(licensebuf);
		return;
	    }

	}
	else
	{
	    LIC_PRINT("create license error\n");
	    return;
	}
	//LicenseWriteToFlash(buf,buflen);
    }

    LIC_PRINT("%s-%d: \n",__func__,__LINE__);
    /* ����license�ĳ���
     * ���ȱ�����B����*/
    memset(partAB, 0, sizeof(partAB));
#if (CONFIG_X86_CPU == 1)
    LIC_PRINT("%s-%d: \n",__func__,__LINE__);
    flash_read_mtdname(LICENSE_FILE, partAB, 4+8, 50);
#else
    flash_read_mtdname("License", partAB, 4, 50);
#endif
    LIC_PRINT("%s-%d: partAB=%s\n",__func__,__LINE__,partAB);
    len = Decipher_PartB((char *)partAB);
    LIC_PRINT("%s-%d: len=%d\n",__func__,__LINE__,len);
    /* ���ݽ�����license����
     * ��ȡ����license����*/
    if(len != 0) {
	content = (char *)malloc(len*2+1);
	if(content == NULL)
	{
	    LIC_PRINT("malloc error!");
	    return;
	}
	memset(content, 0, len*2+1);
    }else {
	return;
    }
#if (CONFIG_X86_CPU == 1)
    flash_read_mtdname(LICENSE_FILE, content, 4+8, len*2);
#else
    flash_read_mtdname("License", content, 4, len*2);
#endif
    LIC_PRINT("%s-%d: content=%s\n",__func__,__LINE__,content);
    /*����license*/
    if( LICENSECHANGED ==  DecipherUTTLicense( uttlicense, content) )
    {
	printf("\nThe license have been changed in flash.\n");
	uttlicense->num = 0x0a;
	uttlicense->authResult = 0x00;
	LIC_PRINT("%s-%d: \n",__func__,__LINE__);
	free(content);
	return;
    }
    Print_license(uttlicense);
    LIC_PRINT("%s-%d: authResult=%d\n",__func__,__LINE__,uttlicense->authResult);

    if(uttlicense->timeflag != LAWLESS)  /*license �Ϸ�*/
    {
	if(uttlicense->timeflag == FOREVER) /*license ������licensee*/
	{
	    LIC_PRINT("%s-%d: forever license\n", __FUNCTION__, __LINE__);	
	    uttlicense->authResult = 0xff;
	}
	else    /*license ����ʱlicense */
	{
	    LIC_PRINT("%s-%d: tmp license\n", __FUNCTION__, __LINE__);	
#if 1
	    if(retneed == TRUE)
	    {
		LIC_PRINT("%s-%d: need create tmp license\n", __FUNCTION__, __LINE__);	    
#if 1
		uttlicense->isOutdate = 0;
		uttlicense->authResult = 0xff;
		uttlicense->runtime = 0U;
		LIC_PRINT("%s-%d: isOutdate=%d, authResult=%u,runtime=%u\n",__func__,__LINE__,uttlicense->isOutdate,uttlicense->authResult,uttlicense->runtime);
		saveLicenseRuntime(uttlicense->runtime);
		ipsecTaskInit();
		//getRuntimeFromFlash(&getruntime);
		LIC_PRINT("%s-%d: getruntime=%u\n", __FUNCTION__, __LINE__,getruntime);	
#endif
	    }
	    else
	    {
		LIC_PRINT("%s-%d: not create tmp license\n", __FUNCTION__, __LINE__);	
#if 1
		getRuntimeFromFlash(&getruntime);
		LIC_PRINT("\nget runtime =%d\n",getruntime);
		if(getruntime>=uttlicense->impowerTime*30*24)
		{
		    LIC_PRINT("========func:%s, line:%d\n", __FUNCTION__, __LINE__);	
		    uttlicense->isOutdate = 1;
		    uttlicense->authResult = 0;
		    uttlicense->runtime=getruntime;
		    uttlicense->taskRunning = FALSE;
		}
		else
		{
		    LIC_PRINT("========func:%s, line:%d\n", __FUNCTION__, __LINE__);	
		    uttlicense->isOutdate = 0;
		    uttlicense->runtime=getruntime;
		    uttlicense->authResult = 0xff;
		    ipsecTaskInit();
		}
#endif
	    }	    
#endif
	}
    }
    else 
    {
	LIC_PRINT("========func:%s, line:%d\n", __FUNCTION__, __LINE__);	
	/*license ���Ϸ�*/
	uttlicense->authResult = 0x00;
    }
    free(buf);
    free(content);
    free(licensebuf);
    }
    LIC_PRINT("========func:%s, line:%d\n", __FUNCTION__, __LINE__);	
    return;
}


#endif
