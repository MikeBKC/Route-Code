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

/*¹¦ÄÜ£º²úÉúËæ»úÃÜÔ¿£¬²¢×ª»»ÎªÊı×Ö¶ÔÓ¦µÄ×Ö·û±£´æÔÚkeyÖĞ¡£
 * ²ÎÊı£ºlen keyµÄ¿Õ¼ä´óĞ¡
 * */
//extern unsigned long random( unsigned long   max );
static void _creatRandomSecretKey( char *key, int keylen )
{	
	int i;

	for( i=0; i<keylen; i++ )
	{
		key[i] = random()%0xff;				/*È¡³ö¶ÔÓ¦Î»ÉÏµÄÊı×Ö*/
	}
	return ;
}


/*PART_D*/

/*licenseĞòºÅ*/

static char getLicenseSequNum( void )
{
	return 0x00;
}

/*¹¦ÄÜ£»Ëæ»úÂëkeyÓë×Ö·ûbyte(D²¿·Ö)¼ÓÃÜ
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

/*E²¿·ÖµÄĞÅÏ¢*/

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
char usermessage[ USERMESSAGELEN + 1 ] =  {'É','Ï','º','£','°','¬','Ì','©','¿','Æ','¼','¼','\0'};

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
    char originE[ PARTELEN + 1 ] = {'0'};	/*license×î³õµÄE²¿·Ö*/
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

		/*F²¿·Ö°üº¬ip*/
		case 0:
		    ip = _license_ntohl( ip );
		    len = AddInformation( partF, len, (char *)(&ip), IPLEN );
		    break;
		    /*°üº¬mac*/
		case 1:	
		    len = AddInformation( partF, len, mac, MACLEN );
		    break;
		    /*°üº¬ÓÃ»§ĞÅÏ¢*/
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
/*licenseµÄÊÚÈ¨Ê±¼ä*/
static void getLicenseServicetime( char *licensetime )
{
	licensetime[0] = '0';	/*°ÙÎ»*/
	licensetime[1] = '0';
	licensetime[2] = '1';
}

/*¹¦ÄÜ£ºÓëÃÜÔ¿key¼ÓÃÜlicenseµÄÊ±¼ä
 *²ÎÊı£ºbuf ´ø»Ø¼ÓÃÜºóµÄÖµ
 * */
static void  _encryptLicenseTime( uchar *date, uchar *key, uchar *buf ) 
{
	/*ÒÔ×Ö·ûĞÎÊ½±£´æÊ±¼äµÄASCIIÂë*/
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
	char controltype;			/*license¿ØÖÆÀàĞÍ*/
	char licensetime[4] = {'0'};		/*licenseµÄÊ¹ÓÃÔÂÊı*/
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

/*×éºÏlicenseµÄDEFG²¿·Ö*/
int creatpartDEFG( char **DEFGPart, uchar *partD, char *partE, char *partF, char *partG, int FLen, int GLen )
{
	int  Len = 0;			/*DEFGµÄ³¤¶È*/
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

/*¼ÓÃÜlicenseµÄ³¤¶È*/
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


/*ÓÃlicensedata×Ö·û¶ÔÓ¦µÄASCIIÂë±£´æ£¬²¢ÓÃ16½øÖÆ±íÊ¾*/
static void saveuttlicenseByHex( char **buf, int licenselen, char *partA, char *partB, char *partC, char *DEFGPart, int len )
{
	uchar	*licensedata = NULL;		/*±£´ælicense¼ÓÃÜºóµÄÄÚÈİ*/

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

       	/*ÒÔ16½øÖÆ±£´ælicense*/	
	//Print( (char*)licensedata, licenselen );
	osSprintf( *buf, (char*)licensedata, licenselen );
	//printf("flash_buf_make:%s\n", *buf );
	free( licensedata );
    	return ;
}


/*Éú²úlicense*/
static void _creatuttlicense( char **buf, int *len )
{
	char  partA[ PARTALEN + 1 ] = {'0'};	/*hmac_sha1µÄ½á¹û*/
	char  partB[ PARTBLEN + 1 ] = {'0'};	/*¼ÓÃÜºóµÄlicense³¤¶È*/
	char  partC[ PARTCLEN + 1 ] = {'0'};	/*¼ÓÃÜºóËæ»úÃÜÔ¿*/
	uchar  partD;				/*¼ÓÃÜºóµÄlicenseĞòºÅ*/
	char  partE[ PARTELEN + 1 ] = {'0'};	/*¼ÓÃÜºóµÄE²¿·ÖÄÚÈİ*/
	char *partF = NULL;			/*Ö¸ÏòF²¿·ÖµÄÄÚÈİ*/
	char *partG = NULL;			/*Ö¸ÏòG²¿·ÖµÄÄÚÈİ*/
	int  DEFGPartLen = 0;			/*D,E,F,GµÄ×Ö½ÚºÍ*/
	char *DEFGPart = NULL;			/*Ö¸ÏòĞ´ÈëD,E,F,G²¿·ÖÄÚÈİ*/
	int  licenselen = 0u;			/*LICENSE_LEN*/
	char randkey[ PARTCLEN + 1 ] = {'0'};	/*Ëæ»úÃÜÔ¿*/
	char licensenum;			/*LicenseĞòºÅ*/
	int  partFLen = 0;
	int  partGLen = 0;
	char controltype;			/*license¿ØÖÆÀàĞÍ*/

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
	 *Ä¿Ç°Ôİ¿¼ÂÇÊ±¼ä¿ØÖÆ
	 * */
	partGLen = creatPartG( &partG, randkey );

	/*¼ÓÃÜDEFG²¿·Ö*/
	DEFGPartLen = creatpartDEFG( &DEFGPart, &partD, partE, partF, partG, partFLen, partGLen );
	cryptoLicenseDEFGPart( randkey, PARTCLEN, DEFGPart, DEFGPartLen );

	/*¼ÓÃÜËæ»úÃÜÔ¿*/
	_encryptSecretKeyByData( partC, PARTCLEN, randkey, DEFGPart, DEFGPartLen );

	/*PART_B
	 * ¼ÓÃÜlicenseµÄ³¤¶È*/
	licenselen = PARTALEN + PARTBLEN + PARTCLEN + PARTDLEN + PARTELEN + partFLen + partGLen;
	*len = licenselen * 2;
	_encryptLicenseLen( licenselen, randkey, partB );

	/*PART_A*/

	controltype = getLicenseType();
	//printf( "license_partA_make\n" );
	creatPartA( controltype, partA, DEFGPart, ( uint )DEFGPartLen, ( uchar * )randkey );

	/*±£´æÍêÕûµÄ¼ÓÃÜºóµÄlicense*/
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
 * ipsec Æô¶¯Ö÷º¯Êı
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

    /*ÅĞ¶ÏÊÇ·ñĞèÒªÉú³ÉÁÙÊ±license*/
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
	    /*ÔÚlicenseÖ®Ç°Ìí¼Ó"2012", ÓÃÒÔ±íÊ¾flashÖĞÒÑ´æÔÚlicense*/
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
    /* ½âÎölicenseµÄ³¤¶È
     * ³¤¶È±£´æÔÚB²¿·Ö*/
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
    /* ¸ù¾İ½âÎöµÄlicense³¤¶È
     * »ñÈ¡Õû¸ölicenseÄÚÈİ*/
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
    /*½âÎölicense*/
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

    if(uttlicense->timeflag != LAWLESS)  /*license ºÏ·¨*/
    {
	if(uttlicense->timeflag == FOREVER) /*license ÊÇÓÀ¾Ãlicensee*/
	{
	    LIC_PRINT("%s-%d: forever license\n", __FUNCTION__, __LINE__);	
	    uttlicense->authResult = 0xff;
	}
	else    /*license ÊÇÁÙÊ±license */
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
	/*license ²»ºÏ·¨*/
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
