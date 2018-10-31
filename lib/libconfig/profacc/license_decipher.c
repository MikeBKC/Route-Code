#include "mib.h"
#include "profacce.h"
#include "daughter.h"

#if ( UTT_LICENSE == FYES )

//#define LICENSE_DEBUG
#ifdef LICENSE_DEBUG
#define _LICENSEPRINT(fmt, args...)		printf(fmt, ## args)
#else
#define _LICENSEPRINT(fmt, args...)
#endif

extern void _printarray( char *buf, int len );

/*从十六进制字符得到对应的字符
 *如：“39”->'9'
 * */
void switchHexToChar( char *des, const char *src, int dessize )
{
	int i=0;
	char tmp[2] = {'0'};

	for( i=0; i<dessize; i++ )
	{
		tmp[0] = src[ 2*i ];
		des[i] = ( strtol( tmp, NULL, 0x10 ) << 4 );
		tmp[0] = src[ 2*i + 1 ];
		des[i] += strtol( tmp, NULL, 0x10 );
	}
}


/*PART_B*/
int Decipher_PartB( char *license_addr )
{
	char buf[ ( PARTBLEN * 2 ) +1 ] = {'0'};
	int len = 0;

	if( NULL != license_addr );
	{
	    //memcpy( buf, ( license_addr + ( PARTALEN * 2 ) ), ( PARTBLEN * 2 ) );
	    _license_strncpy( (uchar *) buf, (uchar*)( license_addr + ( PARTALEN * 2 ) ), ( PARTBLEN * 2 ) );
	    //printf( "deci_len_buf:%s\n", buf );
	    switchHexToChar( (char*)(&len)+2, buf, PARTBLEN );
	    //len = strtol( buf, NULL, 0x10 );						/*license的长度*/
	    //printf("deci_len:%x\n", len);
	}
	return _license_ntohl( len );
}

/*获取完整的license*/
int getIntactLicense( char **buf, int len, char *license_addr )
{
	char *tmp = NULL;
	int  rc =1;

	if( NULL != license_addr )
	{
	    *buf = ( char* )malloc( len + 1 );
	    tmp = ( char* )malloc( len * 2 + 1 );
	
	    if( NULL != *buf && NULL != tmp)
	    {
		    memset( *buf, 0, (len + 1) );
		    memset( tmp, 0, (len * 2+ 1) );
		    //(*buf)[ len ] = '\0';
		    //tmp[ len * 2 ] = '\0';
		    //memcpy( tmp, license_addr, ( len * 2 ) );
		    _license_strncpy( (uchar*)tmp, (uchar*)license_addr, ( uint )( len * 2 ) );
		    _LICENSEPRINT("\ndeci_licesne_buf:%s\n", tmp);
		    _LICENSEPRINT("deci_licesne_buf_ASCII:\n");
		    _printarray( tmp, len*2 );

		    /*形式转换*/
		    switchHexToChar( *buf, tmp, len );
		    //Print( *buf, len);
	    }
	    else
	    {
		    rc = 0;
		    printf( "Created memory was failed for licensebuf!!!\n" );
	    }
#if 1
	    if( NULL != tmp )
	    {
		free( tmp );
	    }
#endif
	}
	else
	{
	    rc = 0;
	    printf( "license_addr is NULL!!!\n" );
	}
	return rc;
}

extern void _encryptSecretKeyByData( char *secretkey, int keylen, char *key, char *cryptoData, uint cryptoDataLen );

/*PART_C*/

void Decipher_PartC( char *buf, LicenseProfile *license )
{
	_encryptSecretKeyByData( license->key, PARTCLEN, ( buf + PARTALEN + PARTBLEN ), ( buf + PARTALEN + PARTBLEN + PARTCLEN ), ( license->len - PARTALEN - PARTBLEN - PARTCLEN ) );

}

/*PART_A*/
extern void creatPartA( char licensetype, char *partA, char *resultDEFG, uint resultDEFGLen, uchar *key );

void checklicensepartA( int type, char * licensebuf, LicenseProfile *license )
{
	int 	len =  PARTALEN + PARTBLEN + PARTCLEN;
	char 	partA[ PARTALEN + 1 ] = {'0'} ;
	char 	buf[ PARTALEN + 1 ] = {'0'} ;
	char	typechar = type;

	//printf( "\nlicense_partA_deci\n" );
	creatPartA( typechar, buf, ( licensebuf+len ), ( license->len - len ), ( uchar *)license->key);
	_license_strncpy( (uchar*)partA, (uchar*)licensebuf, PARTALEN );

	/*系统自我生成的A部分与客户输入的licenseA部分比较*/
	if( strcmp( partA, buf ) != 0 )
	{
		license->timeflag = LAWLESS;
	}
	else
	{
		license->timeflag = EFFECTIVE;
		license->controltype = typechar;
	}
	
}

int checkLicenseIntact( char *licensebuf, LicenseProfile *license ) 
{
	uint 	i = 0u;

	license->timeflag = LAWLESS;
	/*license的类型匹配,TYPENUM控制类型的总数*/
	for( i=0u; ( i<TYPENUM ) && ( LAWLESS == license->timeflag ); i++ )
	{
		checklicensepartA( (0x01+i), licensebuf, license );
	}
	return license->timeflag;
}

/*PART_DEFG*/

void DecipherDEFGPart( char **DEFGPart, char *licensebuf, LicenseProfile *license )
{
	int 	len = PARTALEN + PARTBLEN + PARTCLEN;		/*ABC的长度*/

	*DEFGPart = ( char * )malloc( license->len - len + 1 );
	if( NULL != *DEFGPart)
	{
	    (*DEFGPart)[ license->len - len ] = '\0';
	    _license_strncpy( (uchar*)*DEFGPart, (uchar*)( licensebuf + len ), ( license->len - len ) );
	    decryptoLicenseDEFGPart( license->key, PARTCLEN, *DEFGPart, ( license->len - len ) );
	}
	//printf("DEFGPart_deci:\n");
	//Print( *DEFGPart, ( license->len - len ) );
}

/*PART_D*/
void checklicensenum( LicenseProfile *license )
{
    if( license->num == 0x01 )
    {
	license->timeflag = EFFECTIVE;
    }
    else
    {
	license->timeflag = LAWLESS;
    }
    return ;
}

int Decipher_PartD( char *src, LicenseProfile *license )
{
	int i=0;

	license->num = (uchar)src[0];
	//printf("partD_deci:%x\n", license->num);
	for( i=PARTCLEN; i>0; i-- )
	{
		license->num ^= license->key[ i-1 ];
	}

	/*检测license序号的合法*/
	//checklicensenum( license);
	return license->timeflag;
}

/*PART_E*/

/*读取检测信息标识位及包含信息标识位的值*/
void readSignFromPartE( char *buf, LicenseProfile *license )
{

	if( buf[0] & 0x01 )
	{
		license->checkip = TRUE;
		license->includeip = TRUE;
	}
	else
	{
		license->checkip = FALSE;
		if( buf[2] & 0x01 )
		{
			license->includeip = TRUE;
		}
		else
		{
			license->includeip = FALSE;
		}
	}
	if( buf[0] & 0x02 )
	{
		license->checkmac = TRUE;
		license->includemac = TRUE;
	}
	else
	{
		license->checkmac = FALSE;
		if( buf[2] & 0x02 )
		{
			license->includemac = TRUE;
		}
		else
		{
			license->includemac = FALSE;
		}
	}
	if( buf[0] & 0x04 )
	{
		license->checkuser = TRUE;
		license->includeuser = TRUE;
	}
	else
	{
		license->checkuser = FALSE;
		if( buf[2] & 0x04 )
		{
			license->includeuser = TRUE;
		}
		else
		{
			license->includeuser = FALSE;
		}
	}
}

extern int check_partE( char * );
extern void _encrptPartEByRandomKey( char *key, int keylen, char *partE );

int Decipher_PartE( char *src, LicenseProfile *license )
{
	char buf[ PARTELEN + 1 ] = {'0'};

	_license_strncpy( (uchar*)buf, (uchar*)( src + 1 ), PARTELEN );
	_encrptPartEByRandomKey( license->key, PARTCLEN, buf );
	if( check_partE( buf ) )
	{
		license->timeflag = EFFECTIVE;

		/*读取信息标位*/
		readSignFromPartE( buf, license );
	}
	else
	{
		license->timeflag = LAWLESS;
	}
	return license->timeflag;
}

/*PART_F*/

int Decipher_PartF( uchar *src, LicenseProfile *license )
{
	uint	len = PARTDLEN + PARTELEN;	/*DE部分总长度*/ 
	int	partflen = 0;			/*统计license的F部分长度*/	
	int	userinforlen = 0;

	if( TRUE == license->includeip )
	{
		license->ip = 0;
		_license_strncpy( (uchar*)&(license->ip), (src + ( len + partflen + 2 )), ( uint )IPLEN );
		license->ip = _license_ntohl( license->ip );
		partflen += ( 2 + IPLEN );
	}
	if( TRUE == license->includemac )
	{
		memset( license->mac, 0, ( MACLEN +1 ) );
		_license_strncpy( (uchar*)license->mac, (src + ( len + partflen + 2 )), ( uint )MACLEN );
		partflen += ( 2 + MACLEN );
	}
	if( TRUE == license->includeuser )
	{
		userinforlen = (int)src[ len + partflen + 1 ];
#if 0
		license->userinfor = ( char * )malloc( userinforlen + 1 );
		if( NULL == license->userinfor )
		{
			printf( "Memory craeted failed !!!\n" );	
			return FALSE;
		}
		license->userinfor[ userinforlen ] = '\0';
#endif
		memset( license->userinfor, 0, ( MAXUSERLEN +1 ) );
		_license_strncpy( (uchar*)license->userinfor, (src + ( len + partflen + 2 )), ( uint )userinforlen );
		//Print( license->userinfor, userinforlen );
		partflen += (2 + userinforlen);
	}
	return partflen;

}

extern int  getUserIp( void );
extern void getUserMac( char * );
extern void getUserInformation( char * );

/*检测F部分的信息*/

int checkPartFInformation( LicenseProfile *license )
{
	LicenseProfile *uttlicense=NULL;
	uttlicense =  (LicenseProfile*)ProfGetInstPointByIndex(MIB_PROF_LICENSE, 0);

	if(uttlicense != NULL){
	/*检测用户的ip地址*/
	if( ( TRUE == license->checkip ) && ( license->ip != uttlicense->ip ) )
	{
		license->timeflag = LAWLESS;
	}
	/*检测用户的mac地址*/
	else if( ( TRUE == license->checkmac ) && ( 0 != strcmp( license->mac, uttlicense->mac ) ) )
	{
		license->timeflag = LAWLESS;
	}
	/*检测用户信息*/
	else if( ( TRUE == license->checkuser ) && ( 0 != strcmp( license->userinfor, uttlicense->userinfor ) ) )
	{
		license->timeflag = LAWLESS;
	}
	else
	{
		license->timeflag = EFFECTIVE;
	}
	return license->timeflag;
	}
}


/*PART_G*/

/*licenseG部分的内容暂且只考虑时间控制类型*/
void Decipher_PartG( char *src, LicenseProfile *license, int flen )
{
	char buf[ TIME_VALUE_LEN + 1 ] = {'0'};
	char time[ TIME_VALUE_LEN/2 + 1 ] = {'0'};
	int len = PARTDLEN + PARTELEN + flen;

	_license_strncpy((uchar*) buf, (uchar*)( src+len + 1 + 2 ), TIME_VALUE_LEN );

	/*用随机密钥解密出时间*/
	buf[0] ^= (uchar)license->key[3];
	buf[1] ^= (uchar)license->key[5];
	buf[2] ^= (uchar)license->key[4];
	buf[3] ^= (uchar)license->key[1];
	buf[4] ^= (uchar)license->key[0];
	buf[5] ^= (uchar)license->key[2];
	switchHexToChar( time, buf, TIME_VALUE_LEN/2 );

	if( FALSE != strcmp( "NAN", time ) )
	{
		license->impowerTime = strtol( time, NULL, 10 );
	}
	else
	{
		license->timeflag = FOREVER;
	}
}

void _printarray( char *buf, int len )
{
    int i = 0;

    _LICENSEPRINT("\n");
    for( i=0; i<len; i++ )
    {
	_LICENSEPRINT("%3x", ( uchar )buf[i]);
    }
    _LICENSEPRINT("\n");
}

/*功能：解密license加密文件*/
int DecipherUTTLicense( LicenseProfile *uttlicense, char *licenseaddr )
{
	char	*licensebuf = NULL;
	char	*DEFGPart = NULL;
	int	fpartlen = 0;

	_LICENSEPRINT("\n====func:%s, line:%d\n", __FUNCTION__, __LINE__);
	_LICENSEPRINT("license get from flash:\n");
	_printarray( licenseaddr, ( ( PARTALEN+PARTBLEN+PARTCLEN+PARTDLEN+PARTELEN ) * 2 ) );
	uttlicense->timeflag = LAWLESS;	    /*默认为非法license*/
	/*解密PART_B*/
	uttlicense->len = Decipher_PartB( licenseaddr );

	if( ( uttlicense->len < 46 ) || ( uttlicense->len > 100 ) )
	{
	    printf((" LEN of license is wrong !!!\n"));
	    return LICENSECHANGED;
	}

	/*获取完整的license*/
	if( 0 == getIntactLicense( &licensebuf, uttlicense->len, licenseaddr ) )
	{
	    printf(("Decipher PartA of license failed !!!\n"));
	    return LICENSECHANGED;
	}
#if 1

	/*解密PART_C*/
	Decipher_PartC( licensebuf, uttlicense );

	/*检测license完整性，并解密出控制类型*/
	if( LAWLESS != checkLicenseIntact( licensebuf, uttlicense ) )
	{
		/*解密license的DEFGPart*/
		DecipherDEFGPart( &DEFGPart, licensebuf, uttlicense );
		//printf("===%d, %s\n", __LINE__, __FUNCTION__);
		if( NULL == DEFGPart )
		{
		    uttlicense->timeflag = LAWLESS;
		    _LICENSEPRINT( "Decipher DEFGPart of license failed !!!\n" );
		    return LICENSECHANGED;
		}

		/*解密PART_D*/
		if( LAWLESS != Decipher_PartD( DEFGPart, uttlicense ) )			/*检查序号是否合法*/
		{
			/*解密PART_E*/
			if ( LAWLESS != Decipher_PartE( DEFGPart, uttlicense ) )	/*检测信息标识位是否合法*/
			{
				/*解密PART_F*/
				fpartlen = Decipher_PartF( ( uchar * )DEFGPart, uttlicense );

				/*检测F部分的信息*/
				if( LAWLESS != checkPartFInformation( uttlicense ) )
				{
					/*解密Part_G*/
					Decipher_PartG( DEFGPart, uttlicense, fpartlen );
				}
				else
				{
					return LICENSECHANGED;
				}
			}
			else
			{
			    return LICENSECHANGED;
			}
		}
		else
		{
		    return LICENSECHANGED;
		}
	}
	else
	{
	    return LICENSECHANGED;
	}
#endif
	if( NULL != licensebuf )
	{
	    free( licensebuf );
	}
	if( NULL != DEFGPart )
	{
	    free( DEFGPart );
	}
	Print_license( uttlicense );
	return 0;
}

void Print_license( LicenseProfile *license )
{
    _LICENSEPRINT( "\n======   license	======\n" );
    _LICENSEPRINT( "license->len:%d\n",	license->len );
    _LICENSEPRINT( "license->key:\n" );
    Print( license->key, PARTCLEN );
    _LICENSEPRINT( "license->num:%d\n",	license->num );
    _LICENSEPRINT( "license->controltype:%d\n",license->controltype );
    _LICENSEPRINT( "license->timeflag:%d\n",	license->timeflag );
    _LICENSEPRINT( "license->impowerTime:%d\n",license->impowerTime );
    //_LICENSEPRINT( "license->ip:\n" );
    //xPrint( (char *)&license->ip, IPLEN );
    _LICENSEPRINT( "license->ip:%x\n",		license->ip );
    _LICENSEPRINT( "license->mac:%s\n",	license->mac );
    _LICENSEPRINT( "license->userinfor:%s\n",  license->userinfor );
    _LICENSEPRINT( "license->checkip:%d\n",	license->checkip );
    _LICENSEPRINT( "license->includeip:%d\n",	license->includeip );
    _LICENSEPRINT( "license->checkmac:%d\n",	license->checkmac );
    _LICENSEPRINT( "license->includemac:%d\n",	license->includemac );
    _LICENSEPRINT( "license->checkuser:%d\n",	license->checkuser );
    _LICENSEPRINT( "license->includeuser:%d\n",license->includeuser );
}


/*
 *判断大小端
 * */
extern int isBigEndian( void )
{
    uint i = 0x12345678;
    uchar a = 0u;
    int license_endian = LICENSE_BIG_ENDIAN;	    /*默认为大端*/

    a = (( uchar *)(&i))[0];
    if( 0x78 == a)
    { 
	_LICENSEPRINT( "The cpu is little endian.\n" );
	license_endian = LICENSE_LITTLE_ENDIAN;
	
    }
    else
    {
	_LICENSEPRINT( "The cpu is big endian.\n" );
	license_endian = LICENSE_BIG_ENDIAN;
    }
    return license_endian;	
}

/*大小端转换*/

extern uint _license_ntohl( uint data )
{
    uint    tmp = 0u;
    uint    i = 0u;	

    if( isBigEndian() == LICENSE_LITTLE_ENDIAN ) 
    {
	for( i=0u; i<sizeof(uint); i++ )
	{
	    tmp <<= 8;
	    tmp += ( data >> 8*i ) & 0x000000ff;
	}
    }
    else
    {
	tmp = data;
    }
    return tmp;
}

extern u_Word _license_ntohs( u_Word data )
{
    u_Word    tmp = 0u;
    uint    i = 0u;	

    if( isBigEndian() == LICENSE_LITTLE_ENDIAN ) 
    {
	for( i=0u; i<sizeof(u_Word); i++ )
	{
	    tmp <<= 8;
	    tmp += ( data >> 8*i ) & 0x00ff;
	}
    }
    else
    {
	tmp = data;
    }
    return tmp;
    
}

#endif
