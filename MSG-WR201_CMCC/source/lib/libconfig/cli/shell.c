/*** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***
 ***                                                               ***
 ***                  CONFIDENTIAL INFORMATION                     ***
 ***                  ------------------------                     ***
 ***    This Document contains Confidential Information or         ***
 ***    Trade Secrets, or both, which are the property of UTT      ***
 ***    Technologies, Inc. This document may not be  copied,       ***
 ***    reproduced, reduced to any electronic medium or machine    ***
 ***    readable form or otherwise duplicated and the information  ***
 ***    herein may not be used, disseminated or otherwise          ***
 ***    disclosed, except with the prior written consent of UTT    ***
 ***    Technologies, Inc.                                         ***
 ***                                                               ***
 *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***/
 
/* 
    Copyright 1991-2002 UTT Technologies, Inc.
    All rights reserved.
    Use of copyright notice does not imply publication.
 */

/*
 * SCCS ID:	$Id: shell.c,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
 * 
 * File:  
 * 
 * Overview:
 *  
 *
*/

#include"argcmd.h"
#include"cmd.h"
#include"term.h"
#include"baeeprom.h"
#include"l2tunnel.h"
#include"nat.h"
#include"tftp.h"
#include"vrtr.h"
#include"crc32.h"

#include "exec.h"

#include  "platform.h"

#include "revision.h"

#if ( WEB_SERVER == FYES )
#include "webProf.h" //add by jxy7@2003-12-10 for WebUploadFile
#endif //WEB_SERVER

#if ( SHELL == FYES )

	/* Now this huge buffer is fixed, looks waste of memory.
	   But I have no choice because cannot allocate such a
	   continuous large block of memory */

#define IMAGE_SEG_SIZE 	65536
#define TFTP_BUFS	31

static Byte *_tftpbuf[TFTP_BUFS];
static Byte *_tftpbufp;
static Uint _tftpbufi, _tftplen;

//utt-zhangdl-added

Bool tftpLoadWeb(TermInfo *term); 
Bool tftpLoadSymbol(TermInfo *term);

int shellStr2Int(char *p, char** ptr);
void shellCmdErrorMsg(TermInfo *term,char* p1,char* p2);
int isSymbolNameFirst(char c);
int isSymbolName(char c);
TermInfo *shellTerm ;
void shellDisplayMemory(int* startAdr);

/*symbol */

typedef struct
	{
   		char *name;
    		int adr;
    		char type ;
    		int len;
	} symbol_TBL;

#define SYMBOL_BSS 1
#define SYMBOL_DATA 2
#define SYMBOL_TEXT 3	
#define SYMBOL_MAX_LEN (1024*12)	

symbol_TBL* symboltbl  = NULL;
int symbolIndex=0;
static SEM_ID _sem4Symbol;
int symbolErrorNo=0;

#define SYMBOL_OK 0
#define SYMBOL_NOTFOUND 1

int ShellDebugOn = 0;

char * symbolTextNameByRange(int adr);
char * symbolNameByRange(int adr);
unsigned int symbolValByName(const char * name);
int codeRangeAdrlast=0;

symbol_TBL*  RangeAdrLast=NULL;
symbol_TBL* symbolByName(const char *name);
symbol_TBL* symbolByAdr(int adr);

int symbolFuncParse(unsigned int funcAdr,char *p,int *shellerr,char** ptrError);

void shellDisplayMemory(int* startAdr)
{
	static int * memAdr = NULL;
	unsigned char * ch;
	int i;
	int j;
	
	if ( (int)startAdr < 0 )
	{
		memAdr = (int *)(0) ;	
	}
	if ( (int)startAdr > 0 )
	{
		memAdr = (int *)( (int)startAdr/16 * 16 ) ;	
	}
	for(i = 0 ; i< 64 ; i ++ )
	{
		if( (int(memAdr)) % 16 == 0)
		{
			_printf(shellTerm,"\r\n[%08x] ",int(memAdr));		
		}
		_printf(shellTerm,"%08x ",*memAdr);
		
		if( (int(memAdr)) % 16 == 12 )
		{
			ch = (unsigned char *)( (int) memAdr - 12 ); 
			for(j = 0 ;j < 16 ; j ++,ch++)
			{
				if( (*ch < 0x80)&&(*ch > 0x19 ) )
				{
					_printf(shellTerm,"%c",*ch);	
				}
				else
				{
					_printf(shellTerm,".");	
				}
					
			}
		}
			
		memAdr++;
	}	
	_printf(shellTerm,"\r\n");		
}			

int hexstr2int(char *p)
{
	int val = 0;
	int i;
	int tmp;

	for(i=0;i<8;i++)
	{
		if ( (*p >= '0' )&&( *p <= '9' ))
		{
			tmp = *p -'0';
		}else if((*p >= 'a' )&&( *p <= 'f' ))
		{
			tmp = *p -'a' + 10;
		}else if ((*p >= 'A' )&&( *p <= 'F' ))
		{
			tmp = *p -'A' + 10;
		}
		else
		{
			break;
		}
		val = val * 16 + tmp ;	
		p ++;
	}
	return  val;			
}

int isSymbolNameFirst(char c)
{
	if(  (  ( c <='z' ) &&(c >= 'a') )
		||( ( c <='Z' ) &&(c >= 'A'))
		||(c=='_'))
	{
		return 1;
	}
	else
	{
		return 0;	
	}				
}

int isSymbolName(char c)
{
	if(  (  ( c <='z' ) &&(c >= 'a') )
		||( ( c <='Z' ) &&(c >= 'A'))
		||( ( c <='9' ) &&(c >= '0'))
		||(c=='_'))
	{
		return 1;
	}
	else
	{
		return 0;	
	}	
}


int shellStr2Int(char *p, char** ptr)
{
	int val = 0;
	int tmp;
	int a;
	if( (*p == '0') && ( (*(p+1)== 'x') || (*(p+1)== 'X')))
	{
		a = 16;	
		p += 2;
	}
	else
	{
		a = 10 ;	
	}
	if(a==10)
	{
		while ((*p >= '0' )&&( *p <= '9' ))
		{
			val = val * 10 + *p - '0' ;	
			p ++ ;
		}
		*ptr = p ;
		return  val;
	}
	if(a==16)
	{
		for(;;)
		{
			if ( (*p >= '0' )&&( *p <= '9' ))
			{
				tmp = *p -'0';
			}else if((*p >= 'a' )&&( *p <= 'f' ))
			{
				tmp = *p -'a' + 10;
			}else if ((*p >= 'A' )&&( *p <= 'F' ))
			{
				tmp = *p -'A' + 10;
			}
			else
			{
				break;
			}
			val = val * 16 + tmp ;	
			p ++;
		}
		*ptr = p ;
		return  val;
	}
	*ptr = p ;	
	return -1;		
	
}

void shellCmdErrorMsg(TermInfo *term,char* p1,char* p2)
{
	int jj;
	if( (p1==NULL)||(p2 == NULL))
		return;
	_printf(term,"shell cmd error:%s\r\n",p1);
	_printf(term,"                ");
	for(jj=0;jj<(p2-p1);jj++)
		_printf(term," ");
	_printf(term,"^ \r\n");	
	*p2 = 0;
	termPut(term, term->lastInput);
	
	
}

char * symbolTextNameByRange(int adr)
{
	int i;
	if (NULL == symboltbl)
		return NULL;
	for( i =0;i < symbolIndex ; i++)
	{
		if(   symboltbl[i].type == SYMBOL_TEXT )
		{	
			if(  (adr >= symboltbl[i].adr) 
			&& (adr < (symboltbl[i].adr+symboltbl[i].len) )
			)	
			break;
		}
	}
	if (i < symbolIndex)
	{
		codeRangeAdrlast = adr - symboltbl[i].adr  ;
		return 	symboltbl[i].name;
	}
	else
	{
		codeRangeAdrlast = 0;
		return NULL;	
	}
}

char * symbolNameByRange(int adr)
{
	int i;
	if (NULL == symboltbl)
	{	
		RangeAdrLast = NULL ;
		codeRangeAdrlast = 0;
		return NULL;
	}
	for( i =0;i < symbolIndex ; i++)
	{
		if(  (adr >= symboltbl[i].adr) 
			&& (adr < (symboltbl[i].adr+symboltbl[i].len) )
		)	
			break;
		
	}
	if (i < symbolIndex)
	{
		codeRangeAdrlast = adr - symboltbl[i].adr  ;
		RangeAdrLast = symboltbl + i;
		return 	symboltbl[i].name;
	}
	else
	{
		RangeAdrLast = NULL ;
		codeRangeAdrlast = 0;
		return NULL;	
	}
}

unsigned int symbolValByName(const char *name)
{
	int i;
	int len; 
	unsigned int Readdata;
	symbol_TBL* findSymbol;
	char symbolName[100];
	
	for(i=0;i<100;i++)
		symbolName[i]=0;
	if(1 == isSymbolNameFirst(*name) )
	{
	 	symbolName[0] = *name;
		 name ++ ;  	
	}
	else
	{
		return NULL;	
	}	
	for(i=1;i<(100-1);i++)
	{
		if( 1 == isSymbolName(*name) ) 
		{	
			 symbolName[i] = *name;
		 	name ++ ;  
		}
		else
		{
			break;	
		}
	}
	symbolName[i]=0;	
	
	
	len = strlen(symbolName) ;
	if (NULL == symboltbl)
		return NULL;
	if (len  < 2 )
		return NULL;	
	for( i =0;i < symbolIndex ; i++)
	{
		if ( strlen(symboltbl[i].name) ==  (unsigned int)len )
		{
 			if(!strncmp(symbolName, symboltbl[i].name,len))
				break;
		}
	}
	if (i < symbolIndex)
	{
		findSymbol = symboltbl + i;
		symbolErrorNo = 0;
		switch(findSymbol->len)
		{
			case 1:
			{
				char *charTadr = (char*)( (findSymbol->adr) );
				Readdata = (unsigned int) *charTadr;
				return Readdata;
			}break;
			case 2:
			{
				short *shortTadr = (short*)((findSymbol->adr) &0xfffffffe);
				Readdata = (unsigned int) *shortTadr;
				return Readdata;
			}break;
			default:
			{
				unsigned int *intTadr = (unsigned int*)((findSymbol->adr)&0xfffffffc);
				Readdata = (unsigned int) *intTadr;
				return Readdata;
			}break;


		}
		
	}
	else
	{
		symbolErrorNo = 1;
		return 0;	
	}
}

symbol_TBL* symbolByName(const char * name)
{
	int i;
	int len; 
	char symbolName[100];
	
	for(i=0;i<100;i++)
		symbolName[i]=0;
	if( isSymbolNameFirst(*name))
	{
		symbolName[0] = *name;
		name ++ ; 	    	
	}
	else
	{
		return NULL;	
	}
	for(i=1;i<(100-1);i++)
	{
		if( isSymbolName(*name))
		 {
		 	symbolName[i] = *name;
		 	name ++ ;  
		}
		else
		{
			break;	
		}
	}
	symbolName[i]=0;	
	len = strlen(symbolName) ;
	if (NULL == symboltbl)
		return NULL;
	if (len  < 1 )
		return NULL;	
	for( i =0;i < symbolIndex ; i++)
	{
		if ( strlen(symboltbl[i].name) ==  (unsigned int)len )
		{
 			if(!strncmp(symbolName, symboltbl[i].name,len))
				break;
		}
	}
	if (i < symbolIndex)
	{
		return  (symboltbl + i);
	}
	else
		return NULL;	
}

symbol_TBL* symbolByAdr(int adr)
{
	int i;

	if (NULL == symboltbl)
		return NULL;
	if (adr  <= 0  )
		return NULL;	
	for( i =0;i < symbolIndex ; i++)
	{
		if ( symboltbl[i].adr==  adr )
		{
 				break;
		}
	}
	if (i < symbolIndex)
	{
		return  (symboltbl + i);
	}
	else
		return NULL;	
}

char * symbolNameByAdr(int adr)
{
	int i;
	if (NULL == symboltbl)
		return NULL;
	for( i =0;i < symbolIndex ; i++)
	{
		if (adr == symboltbl[i].adr)
			break;
		
	}
	if (i < symbolIndex)
	{
		return 	symboltbl[i].name;
	}
	else
		return NULL;	
}

int symbolTextLenByAdr(int adr)
{
	int i;
	if (NULL == symboltbl)
		return -1;
	for( i =0;i < symbolIndex ; i++)
	{
		if (adr == symboltbl[i].adr)
			break;
		
	}
	if (i < symbolIndex)
	{
		return 	symboltbl[i].len;
	}
	else
		return -1;	
	
}

int symbolFuncParse(unsigned int funcAdr,char *p,int *shellerr,char** ptrError)
{

	symbol_TBL*	tmpSymbol;
	
	char *ptr = NULL;
	int (*adr)(int,int,int,int,int,int,int,int,int,int) ;
	int cc;
	int re;
	int arg[10]; 
	int argType[10]; /*0=int ,1=string ,2=NULL*/
	char strArg[10][50];
	
	int i;
	int ii;
	
	unsigned int codecheck;
	
	codecheck = (unsigned int)*(unsigned int *)(funcAdr & 0x007ffffc);
	if(codecheck != 0xe1a0c00d)
	{
		*shellerr = -1 ;
		*ptrError = p;	
		return -60;	
	}
	
	
	/*parse args*/
	/*arg1,2,3,4,...,10*/
	 // _printf(shellTerm," in  shellFuncParse() ,p=%s\r\n",p);
	
	/*pares space */
	while(*p == ' ')
		p++;
	
	/*here we found the function*/

	/*parse args*/
	ii = 0;
	while( (ii < 10 )&& (*p != '\0') && (*p != 0x0a)&& (*p != 0x0d)&& (*p != ')') )
	{
		/*pares space */
		while(*p == ' ')
			p++;
		if( (*p == '(' )
			||(*p == ',' )
			)
		{
			p ++;	
		}
		else
		{
			*shellerr = -1 ;
			*ptrError = p;	
			return -1;	
		}
		while(*p == ' ')
			p++;	
		if( (*p == 0x0d ) 
			|| (*p == 0x0a )
			|| (*p == 0	)
			|| (*p == '\0')
			|| (*p == ')' )
			)
		{
			break;
		}
		/*string*/
		/* if (*p == '\"') */
		if ( (*p == '\"') ||(*p == '\'' ) )/*  " */
		{
		  	
			p++;
			i = 0;
			while( (*p != '\0' ) 
				&& (*p != '\"' ) 
				&& (*p != '\'' )
				&& (*p != 0 )
				&& (*p != 0x0d )
				&& (*p != 0x0a )
				)
			{
				strArg[ii][i++]=*p;
				p++;
				if ( i > 49)
				{
					*shellerr = -1 ;
					*ptrError = p;	
					return -2;		
				}	
			}
			if ( (*p != 0x22 ) && (*p != '\'') )/*if (*p != '\"' )*/
			{
			   *shellerr = -1 ;
			   *ptrError = p;	
			   return -4;	
			   /*p++;*/
			}
			p ++ ; /*parse " */
			strArg[ii][i]='\0';
			if (i == 0)
			{
				argType[ii] = 2 ;/*NULL*/	
			}
			else
			{
				argType[ii] = 1 ;/*string*/	
			}
			// _printf(shellTerm," arg[%d] is string\r\n",ii);
			// _printf(term,"3 as1=%d,strArg[0]=%s\r\n",as1,strPar[0]);
		}	
		else if (isdigit(*p))
		{
			
			ptr = p ;
			arg[ii] = shellStr2Int(p, &ptr);
			p = ptr ;
			if( ( *p !=' ' ) && (*p != ',') && (*p != 0) && (*p != ')')&& (*p !=0x0d)&&(*p!=0x0a))
			{
				*shellerr = -1 ;
				*ptrError = p;	
				return -6;	
			}
			argType[ii] = 0;
		 	// _printf(shellTerm," arg[%d]= %d,0x%x ,next p=%s \r\n",ii,arg[ii],arg[ii],*p);				 			
		}
		/*negative digit*/			
		else if( (*p == '-' ) && (isdigit(*(p+1))))
		{
			p++;
			ptr = p ;
			arg[ii] = 0 -  shellStr2Int(p, &ptr);
			p = ptr ;
			if( ( *p !=' ' ) && (*p != ',')&& (*p != 0)  && (*p != ')')&& (*p !=0x0d)&&(*p!=0x0a))
			{
				*shellerr = -1 ;
				*ptrError = p;	
				return -8;	
			}
			argType[ii] = 0;
		 	// _printf(shellTerm," arg[%d]= %d,0x%x ,next p=%s \r\n",ii,arg[ii],arg[ii],*p);				 			
		}
		/*NULL or symbl*/
		else
		{
			if ( ((*p == 'N' ) && (*(p+1)=='U') && (*(p+2) == 'L' ) && (*(p+3) == 'L' ))
				|| ((*p == 'n' ) && (*(p+1)=='u') && (*(p+2) == 'l' ) && (*(p+3) == 'l' ))
				)
			{
				argType[ii]  = 2 ;/*NULL*/
				p+=4;
				// _printf(term,"5  args=NULL\r\n");
				// _printf(shellTerm," arg[%d] is NULL \n",ii);
			}
			else
			{
				tmpSymbol=symbolByName(p);
				if(tmpSymbol)
				{
					cc = symbolValByName(p);
					p += strlen(tmpSymbol->name) ;
					if( (tmpSymbol->type == SYMBOL_BSS) || (tmpSymbol->type == SYMBOL_DATA))
					{
						if ( ( tmpSymbol->len ==0 )
						    ||( tmpSymbol->len ==1 )
						    ||( tmpSymbol->len ==2 )
						    ||( tmpSymbol->len ==4 )
						   )
						 {   	
							arg[ii] = cc;
						}
						else	/*string*/
						{
							arg[ii] = tmpSymbol->adr;
						}
						argType[ii] = 0;
					}
					else
					{
						_printf(shellTerm, "%s is  a invaliad symbol . \r\n",p);
						*shellerr = -1 ;
						*ptrError = p;	
						return -10;
					}
					
				}
				else
				{
					*shellerr = -1 ;
					*ptrError = p;	
					return -12;
				}	
			}
		}
		ii++;
	}
			
	ii -- ;
	adr =(int (*)(int,int,int,int,int,int,int,int,int,int) ) (funcAdr) ;
	for (i=ii+1 ; i < 10 ; i ++)
		arg[i]  = 0;
	for (i=0 ; i <= ii ; i ++)
	{
		switch (argType[i])
		{
			case 0: /*int*/
				/*arg[i] = arg[i];*/
			break ;
			case 1: /*string*/
				arg[i] = (int)&(strArg[i][0]);
			break;
			case 2:/*NULL*/
				arg[i]  = 0;
			break;
			default:
			break;	
		}
	}
	
	 // _printf(shellTerm,"adr=%08x(%x,%x,%x,%x,%x) = ",adr,arg[0],arg[2],arg[2],arg[3],arg[4]);
	
	re = (adr)(arg[0],arg[1],arg[2],arg[3],arg[4],
		arg[5],arg[6],arg[7],arg[8],arg[9]) ;
	*shellerr = 0 ;
	//  _printf(shellTerm,"%d,%x\r\n",re,re);	
	return re;
			
}

int symbolCheck1 = 0x12345678;
char* tmpSymbolString[TFTP_BUFS]={NULL};

static void _CreateSymbol(TermInfo *term)
{
    symbol_TBL* symbolTmp1;
    symbol_TBL* symbolTmp2;
    int i, j;
    char* tmp;
    char *p;
    char *p1;
    int tmpUsed;
    int adr;
    int len;
    char type;
    char syType;
    int found;	
    int cc;
    
    int TextCount = 0;
    int DataCount = 0;
    int BssCount = 0;	   
 
    syType = SYMBOL_BSS; 

    if ( NULL != symboltbl)
    {
    		FREE(symboltbl);
    		symboltbl = NULL ;
    		symbolIndex= 0;
    		for(i=0;i<TFTP_BUFS;i++)
    		{
    			if(tmpSymbolString[i]) FREE(tmpSymbolString[i]);
    			tmpSymbolString[i] = NULL;
    		}
    }
    symboltbl = (symbol_TBL*)MALLOC(SYMBOL_MAX_LEN* sizeof(symbol_TBL) ) ; /* max 10240 symbols */	
    if(NULL == symboltbl)
    {
    	_printf(term, "ERROR : can't mallo memoey for symboltbl.\r\n");
    	return ;	
    }
    p = (char *)_tftpbuf[0];

   
   /*tftp translate "0d 0a" to " 0a" */
     for(i = 0;i < TFTP_BUFS-1;i ++)
    {
	p1 = (char *)_tftpbuf[i] + IMAGE_SEG_SIZE-2;
	p = (char *)_tftpbuf[i+1];
	j = 0;
	while(*p1 != 0)
	{
		p1 ++ ;
		j ++ ;
		if(j > 10)
			break;	
	}
	j = 0;
	while((0 != *p) && (0x0a != *p))
	{	
		*p1 = *p;
		p1++;
		p++;
		j ++ ;
		if (j > 100)
			break;
	}
    }
   
    for(i = 0;i < TFTP_BUFS;i ++)
    {
    	tmp = (char*)MALLOC(1024*64);
    	tmpSymbolString[i] = tmp;
    	if(NULL == tmp)
    	{
    		_printf(term, "ERROR : can't mallo memoey for symbol string.\r\n");
    		return;
	}
    	tmpUsed = 0;
    	p = (char *)_tftpbuf[i];
    	while( (*p != 0 ) && ( p < ( (char * )_tftpbuf[i] + IMAGE_SEG_SIZE + 120)) )
    	{
    		/*find new line*/
    		cc = 0;
    		while( (0 != *p) && ( 0x0a != *p))
    		{	p++ ;
    			cc ++ ;
    			if (cc >300)
    			{
    				/* _printf(term, "find new line error.\r\n"); */
    				break;
    			}
    		}	
    		if( (0 != *p) && ( 0x0a == *p))
    		{
    			/*tftp translate "0d 0a" to " 0a" */
    			p ++ ;
    			if(0x0a != *p )
    			{
    				adr = hexstr2int(p);
    				p+=8;
    				cc = 0;
    				while((*p !='.')&&(*p != 0x0a))
    				{
    					p++;
    					if(cc > 20)
    						break;
    				}
    				if( (cc < 20 )&&('.' == *p)) /*find '.' */
    				{
    					found = 0;
    					p++;
    					type= *p;
    					switch(type)
    					{
    						case 'b':
    						{	
    							if ( ( *(p+1)=='s' ) && ( *(p+2)=='s' ) )
    							{
    								syType = SYMBOL_BSS; 
    								found = 1;		
    								TextCount ++ ;
    							}
    						}break;
    						case 't':
    						{	if ( ( *(p+1)=='e' ) && ( *(p+2)=='x' ) )
    							{
    								syType = SYMBOL_TEXT; 
    								found = 1;	
    								DataCount ++	;
    							}
    						}break;
    						case 'd':
    						{	
    							if ( ( *(p+1)=='a' ) && ( *(p+2)=='t' ) )
    							{
    								syType = SYMBOL_DATA; 
    								found = 1;	
    								BssCount++;	
    							}
    						}break;
    						default :
    							break;		
    					}
    					if((1==found)&&(symbolIndex<SYMBOL_MAX_LEN))
    					{	
    						symboltbl[symbolIndex].adr = adr;
    						symboltbl[symbolIndex].type = syType;
    						
    						/*find table*/
    						/*cc = 0;
    						while( (*p !=0)&&(*p != 0x09))
    						{	
    							p++ ;
    							cc ++ ;
    							if (cc > 100)
    							{
    								_printf(term, "invalid line.\r\n");
    								break ;
    							}
    						}
    						*/
    						while( *p != '0' )
    						{
    							p ++ ;	
    						}	
    						if(*p == 0)
    						{		_printf(term, "invalid line..\r\n");	
    								break;
    						}
    						if( (*p != '0')||(*(p+1) != '0')||(*(p+2) != '0')) 
    						{		_printf(term, "invalid line..\r\n");	
    								break;
    						}
    						p += 3;
    						len =hexstr2int(p);
    						symboltbl[symbolIndex].len = len;
    						 /*find symbol string*/
    						cc = 0; 
    						while( *p != 0x20 )
    						{
    							p ++ ;
    							cc ++ ;
    							if (cc > 20)
    								break;	
    						}	
    						if( (cc > 19 ) || (*p == 0) )
    						{		_printf(term, "invalid line..\r\n");	
    								break;
    						}
    						p ++ ; 
    						symboltbl[symbolIndex].name = ( tmp + tmpUsed );
    						cc = 0;
    						while ( *p != 0x0a)
    						{
    							tmp[tmpUsed++] = *p++;
    							if (*p == 0x0a)
    								break;
    							if (*p == 0)
    								break;
    							if ( (*p == '_' )&&(*(p+1) == '_'))
    								break;
    									
    							cc ++;
    							if(cc > 50 )
    								break; 	
    						}
    						tmp[tmpUsed++]=0;
    						symbolIndex++;
    						
    						if (tmpUsed > 1024 * 64 -64)
    						{
    							tmpUsed = 0;
    							tmp = (char*)MALLOC(1024*64);
    							if(NULL == tmp)
    							{
    								_printf(term, "ERROR : can't mallo memoey for symbol string.\r\n");
    								break;
    							}
    						}
    					}/*if((1==found)&&(symbolIndex<10*1024))*/
    					
    				}
    			} /*if(0x0a != *p )*/		
    		}/*if( (0 != *p) && ( 0x0d == *p))*/
    	} /*while( (*p != 0 ) && ( p < ( (char * )_tftpbuf[i] + IMAGE_SEG_SIZE)) )*/
    	
	if (i>1)
		if(_tftpbuf[i-1]) 
		{
			FREE(_tftpbuf[i-1]);
			_tftpbuf[i-1] = NULL;
		}
    }
    //int symbolCheck1 = 0x12345678; 
    symbolTmp1 = symbolByName("symbolCheck1");
    symbolTmp2 = symbolByName("_CreateSymbol");
    
    if( (symbolTmp1 != NULL) && (symbolTmp2 != NULL))
    {
    	
    	if (  ( (unsigned int) *(unsigned int*)( (symbolTmp1->adr) & 0x007ffffc) != 0x12345678 )
    		||( (unsigned int) *(unsigned int*)( (symbolTmp2->adr) & 0x007ffffc) != 0xe1a0c00d )
		)
    	{
    		_printf(term, "symbol file is invalid\r\n");
    		FREE(symboltbl);
    		symboltbl = NULL ;
    		symbolIndex = 0;
    		for(i=0;i<TFTP_BUFS;i++)
    		{
    			if(tmpSymbolString[i]) FREE(tmpSymbolString[i]);
    			tmpSymbolString[i] = NULL;
    		}
   	 }
    	else
    	{
 	
    		_printf(term, "Create symbol %d entries,TEXT:%d,DATA:%d,BSS:%d\r\n",
    			 symbolIndex,TextCount,DataCount,BssCount);
    		_printf(term, "\r\n");
    	}
    
    }
    else
    {
    	_printf(term, "symbol file is invalid\r\n");
    		FREE(symboltbl);
    		symboltbl = NULL ;
    		symbolIndex = 0;
    		for(i=0;i<TFTP_BUFS;i++)
    		{
    			if(tmpSymbolString[i]) FREE(tmpSymbolString[i]);
    			tmpSymbolString[i] = NULL;
    		}	
    }
}


static int
_loadSymbolCallback( int    status,
                  int     session,
                  void*   buf,
                  int     len,
                  int	tag )
{
    static int counter = 0;
    TermInfo *term = (TermInfo*)tag;
    int len1 = 0;

		
    if(status == TFTP_WRITE_DATA)
    {
	if(_tftpbufp + len <= _tftpbuf[_tftpbufi] + IMAGE_SEG_SIZE  )
	{
	    memcpy(_tftpbufp, (Byte*)buf, len);
	    _tftpbufp += len;
	}
  	else 
	{
	    
	    len1 = _tftpbufp + len - (_tftpbuf[_tftpbufi] + IMAGE_SEG_SIZE);
	    memcpy(_tftpbufp, (Byte*)buf, len - len1);
	    if(++_tftpbufi >= TFTP_BUFS)
	    {
		counter = 0;
		SemaphoreSignal(_sem4Symbol);
		return TFTP_EUERROR;
	    }
	    memcpy(_tftpbuf[_tftpbufi], (Byte*)buf + len - len1, len1);
	    _tftpbufp = _tftpbuf[_tftpbufi] + len1;
	}
	_tftplen += len;
	if(!(counter++ % (IMAGE_SEG_SIZE/TFTP_SEGSIZE)))
	    _printf(term, ".");
	if(len < TFTP_SEGSIZE)
	{
	    counter = 0;
	    SemaphoreSignal(_sem4Symbol);
	}
    	return TFTP_OK;
    } else
    {
	counter = 0;
	SemaphoreSignal(_sem4Symbol);
	return TFTP_EUERROR;
    }
}


Bool tftpLoadSymbol(TermInfo *term)
{
    int session, status;
    char *tmp, *tmp1;
    Bool result = FALSE;
    Uint32 host;
    Word tftpPort = 69;
    Uint i;
    static Bool Symbolloading = FALSE;
    ROUTE *rt = NULL;

    if(Symbolloading)
    {
	_printf(term, "symbol load running.\r\n");
	return TRUE;
    }
    if(!_sem4Symbol)
	_sem4Symbol = createSem();

    Symbolloading = TRUE;

    tmp = MALLOC(MAX_CMDLEN);
    do {
    	for(i = 0;i < TFTP_BUFS;i++)
    	{
    	    _tftpbuf[i] = (Byte*)MALLOC(IMAGE_SEG_SIZE+ 120);
	    if(!_tftpbuf[i])
	    {
		_printf(term, "Fatalerr: no more memory for tftp load, failed.\r\n");
		break;
	    }
	    memset(_tftpbuf[i],0,IMAGE_SEG_SIZE+ 120);
	}
	if(i != TFTP_BUFS) break;
    	if(!argGet(term->lastInput, 2, tmp))
	    break; 
	if((tmp1 = strstr(tmp, ":")))
	{
	    tftpPort = strtol(tmp1 + 1, 0, 10);
	    *tmp1 = 0;
	}
	if(tmp[0] == 'l' && tmp[1] == 0)
	    host = htonl(0xc8c8c80f);
	else
       	if(!tftpPort
	   || !(host = getHostByName(tmp, NULL)))
    	{
	    break;
    	}
	if (!argGet(term->lastInput, 3, tmp)) break;

	result = TRUE;

	if(!strstr(tmp, ".tbl")) strcat(tmp, ".tbl");
    	rt = NULL;
	rt = route_find_mp(host, GLOBAL_VROUTER_ID, GLOBAL_MPROUTE_TAG);
	if( rt == NULL )
	{
	    _printf(term, "No route to dest host.\r\n");
	    break;
	}

        if ( rt ) route_free ( rt ) ;

	_tftpbufp = _tftpbuf[0];
	_tftplen = 0;
	_tftpbufi = 0;
    	status = tftp_get_request( host, tmp, _loadSymbolCallback,
                               &session, tftpPort, (int)term );
	if(status != TFTP_OK)
	{
	    _printf(term, "Load failed.\r\n");
	    break;
	}
	_printf(term, "Starting receiving file:%s from %N:%u\r\n",
		   tmp, host, tftpPort);
	SemaphoreWait(_sem4Symbol);
	if(_tftpbufp == _tftpbuf[0])
	{
	    _printf(term, "Load failed.\r\n");
	    break;
	}
	
	_printf(term, "\r\nLoad OK, File size:%dK.\r\n", _tftplen >> 10);
	_CreateSymbol(term);
	
    } while(0);

    FREE(tmp);
    for(i = 0;i < TFTP_BUFS;i++)
    {
	if(_tftpbuf[i]) FREE(_tftpbuf[i]);
    }
    Symbolloading = FALSE;

    return result;
}

#if (WEB_UI == FYES)
/*for web*/
/* max web page size = 256k */
#define MAXWEBSIZE (262144)
static SEM_ID _sem4Web = NULL;
static Byte *_webbuf = NULL;
static Byte *_webbufp = NULL;
static Uint _weblen;
char webfilename[50];
char webDescfilename[100];

#include "wsIntrn.h"

void  tftpLoadWebTimeOut(int sem)
{
	SemaphoreSignal((SEM_ID)sem);
}

static void _UpdataWeb(TermInfo *term)
{
	extern websRomPageIndexType websRomPageIndex[] ;
	Uint jj;
	
	websRomPageIndexType * pwebsRom = NULL;
	unsigned char *pageUpdate = NULL;
	char_t *pagePath = NULL;
	int newPage = 0;
	
	pwebsRom = websRomPageIndex;
	while (pwebsRom->path != 0 )
	{
		/*  */
		if (! strncmp((pwebsRom->path) ,webDescfilename,strlen(webDescfilename)))
			break;
		pwebsRom ++ ;
	}
	newPage = 0;
	if (pwebsRom->path == 0 )
	{
		/* _printf(term,"no page named %s \r\n",webfilename); */
		/* tack null.html */
		pwebsRom = websRomPageIndex;
		while (pwebsRom->path != 0 )
		{
			/*  */
			if (! strncmp((pwebsRom->path) ,"/null.html",strlen("/null.html")))
			{	
				newPage = 1;
				break;
			}
			pwebsRom ++ ;
		}	
		
	}
	if (pwebsRom->path == 0 )
	{
		_printf(term," webs page rom alloc  error. \r\n");
		return ;	
	}
	//if( (int)pwebsRom->path > 0x00300000 ) 
	if( (pwebsRom->path) && !(pwebsRom->mode & WEBS_PAGE_MODE_STATIC) ) {
		FREE((void*)pwebsRom->path);
        }
	
	pageUpdate = (unsigned char *)memalloc( _weblen + 12);
	if (pageUpdate == NULL)
	{
		_printf(term,"mnemory alloc error. \r\n");
		return ;	
	} 
	for(jj=0;jj<_weblen;jj++)
	{
		pageUpdate[jj] = _webbuf[jj];
	}
	pageUpdate[jj] = 0;
	
	if(1 == newPage)
	{
		pagePath = (char_t *)malloc(sizeof(char_t) * (strlen(webDescfilename)+4));
		if( pagePath == NULL)
			return ;
		for(jj=0;jj<(strlen(webDescfilename)+1);jj++)
		{
			pagePath[jj] = 	webDescfilename[jj];
		}
		for(;jj<(strlen(webDescfilename)+4);jj++)
		{
			pagePath[jj] = 0;	
		}
		pwebsRom->path = pagePath ;
		pwebsRom->mode = WEBS_PAGE_MODE_DYNAMIC;
	}
	pwebsRom->page = pageUpdate;
	pwebsRom->size = _weblen;
	if(1 == newPage)
	{
		_printf(term,"Add new web page %s,size=%d, rom adr=%08x.\r\n",
			pwebsRom ->path,pwebsRom->size,pwebsRom->page);	
	}
	else
	{
		_printf(term,"Update web page %s,size=%d, rom adr=%08x.\r\n",
			pwebsRom->path, pwebsRom->size,pwebsRom->page);
	}
}

static int
_loadWebCallback( int    status,
                  int     session,
                  void*   buf,
                  int     len,
                  int	tag )
{
  
    TermInfo *term = (TermInfo*)tag;
    ///int len1 = 0;

		
    if(status == TFTP_WRITE_DATA)
    {
	if(_webbufp + len <= _webbuf+ MAXWEBSIZE  )
	{
	   /*_printf(term," len=%d,total=%d\r\n",len,_weblen+len);*/
	    memcpy(_webbufp, (Byte*)buf, len);
	    _webbufp += len;
	}
  	else 
	{
	   /*_printf(term," len=%d,  total=%d\r\n",len,_weblen+len);	*/
	   SemaphoreSignal(_sem4Web); 
	   
	   return TFTP_OK;
	   
	}
	_weblen += len;
	if(_weblen % 4096 == 0)
	{
		_printf(term, ".");		
	}
	/*TPTP最后一个段长度 < TFTP_SEGSIZE，标志结束 */
	if(len < TFTP_SEGSIZE)
	{
	    SemaphoreSignal(_sem4Web);
	}
	
    	return TFTP_OK;
    } else
    {
	/* _printf(term," TFTP_EUERROR\r\n");*/
	SemaphoreSignal(_sem4Web);
	return TFTP_EUERROR;
    }
}


Bool tftpLoadWeb(TermInfo *term)
{
    int session, status;
    int jj;
    char *tmp1;
    char tmp[100], tmp2[100];
    Bool result = FALSE;
    Uint32 host;
    Word tftpPort = 69;
    ///Uint i;
    static Bool webloading = FALSE;
    ROUTE *rt = NULL;

    if(webloading)
    {
	_printf(term, "symbol load running.\r\n");
	return TRUE;
    }
    if(!_sem4Web)
	_sem4Web = createSem();

    webloading = TRUE;

   if(!argGet(term->lastInput, 2, tmp))
   {
	webloading = FALSE ;
	return FALSE; 
  }
  	if((tmp1 = strstr(tmp, ":")))
	{
	    tftpPort = strtol(tmp1 + 1, 0, 10);
	    *tmp1 = 0;
	}
	if(tmp[0] == 'l' && tmp[1] == 0)
	    host = htonl(0xc8c8c80f);
	else
       	if(!tftpPort
	   || !(host = getHostByName(tmp, NULL)))
    	{
    	  webloading = FALSE ;
	  return FALSE;
    	}
	if (!argGet(term->lastInput, 3, tmp)) {
    	  	webloading = FALSE ;
		return FALSE;
        }
   
	jj = 0;	   
	for(jj=0;jj<49;jj++)
	{
		if(tmp[jj] != 0)
		{
			webfilename[jj] = tmp[jj];
		}
		else
		{
			break;	
		}
	}
	for(;jj<50;jj++)	   
		webfilename[jj] = 0;
	
	if (!argGet(term->lastInput, 4, tmp2)) 
	{	
		for(jj=0;jj<100;jj++)	   
			webDescfilename[jj] = tmp2[jj] ;
	}
	else
	{
		for(jj=0;jj<99;jj++)
		{
			if(tmp2[jj] != 0)
			{
				webDescfilename[jj] = tmp2[jj];
			}
			else
			{
				break;	
			}
		}
	}
	for(;jj<100;jj++)	   
		webDescfilename[jj] = 0;
		
	result = TRUE;

	rt = NULL;
	rt = route_find_mp(host, GLOBAL_VROUTER_ID, GLOBAL_MPROUTE_TAG);
	if( rt == NULL )
	{
	    webloading = FALSE ;
	    _printf(term, "No route to dest host.\r\n");
	    return FALSE;
	}

        if ( rt ) route_free ( rt ) ;

   _webbuf = (Byte*)MALLOC(MAXWEBSIZE);
   if(!_webbuf)
   {
	_printf(term, "Fatalerr: no more memory for tftp load, failed.\r\n");
	webloading = FALSE ;
	return FALSE;
   }

   memset(_webbuf,0,MAXWEBSIZE);
	
	_webbufp = _webbuf;
	_weblen = 0;

    	status = tftp_get_request( host, tmp, _loadWebCallback,
                               &session, tftpPort, (int)term );
	if(status != TFTP_OK)
	{
	    _printf(term, "Load failed.\r\n");
	    webloading = FALSE ;
	    if ( _webbuf ) {
		FREE(_webbuf);
		_webbuf = NULL;
	    }
	    return FALSE;
	}
	_printf(term, "Starting receiving file:%s from %N:%u\r\n",
		   tmp, host, tftpPort);

	
	SemaphoreWait(_sem4Web);
	if(_webbufp == _webbuf)
	{
	    _printf(term, "Load failed.\r\n");
	    webloading = FALSE ;
	    if ( _webbuf ) {
		FREE(_webbuf);
		_webbuf = NULL;
	    }
	    return FALSE;
	}
	
	_printf(term, "\r\nLoad OK. File size:%dK.\r\n", _weblen >> 10);
	_UpdataWeb(term);

    if(_webbuf) FREE(_webbuf);
    webloading = FALSE;
    return result;
}
void readWordFunc(unsigned int * adr)
{
	printf("%08x = %08x \n",(int)adr,*adr);	
}
void readWord(unsigned int * adr)
{
	printf(" readWord :: readWordFunc(%08x) \n",(int)adr);
	readWordFunc(adr);	
}

#endif  /* WEB_UI == FYES */

#endif  /* SHELL == FYES */

/*utt-zhangdl-end*/	
 
#if ( EXECPHOOK == FYES )


int hookCounter = 0;
int ddtmp_undef_kook_pp = 0;
void undefHookTestLoop()
{
	int i;
	int j;
	j = getMasterClock();
	i = j / 100 ;	
	printf("tick=%d, (i=%d ,hookCounter=%d) \n",i,j,hookCounter);
	j = getMasterClock();
	j += i;
	ddtmp_undef_kook_pp = j * i;

}
void undefHookTestTask()
{
	int *a;
	a = (int *)0x300002;
	for(;;)
	{
		sleep(100 * 2);
		undefHookTestLoop();
		sleep(100 * 5);
		*a = 0x1234;
	}
}



void undefHookTest()
{
	
	
	createTask( (TaskFunction)undefHookTestTask 
		    	,TASK_PRI_1 /*low*/
		    	,EXTRA_LARGE_STACK
		    	,NULL
		    	,"undefHookLoop"
		);	
	
	
}

#endif   /* EXECPHOOK == FYES */

/*utt-zhangdl-030826-added -start*/
#if (SHELL == FYES)
   
    
int termShellCmd(TermInfo *term)
{

/*shell open and close */
    if (  (! strncmp(term->lastInput, "shell",5) ) || ( *(term->lastInput)== '>'))
    {
    		int found = 0;
    		if(! strncmp(term->lastInput, "shell",5) )
    		{
    			char *p;
    			p = term->lastInput;
    			p+=5;
    			if( (*p == 0) || (*p == 0x0d) || (*p == 0x0a))
    				found =1 ;
    		}
    		if( *(term->lastInput)== '>')
    		{
    			char *p;
    			p = term->lastInput;
    			p++;
    			if( (*p == 0) || (*p == 0x0d) || (*p == 0x0a))
    				found =1 ;
    		}
    		if (found == 1)
    		{
			termPut(term, term->lastInput);
			
			if ( ShellDebugOn == 0 )
			{
				ShellDebugOn =1 ;
			 	 _printf(term, "Shell mode ON.\r\n");
			}
			else
			{
				ShellDebugOn = 0;	
			  	_printf(term, "Shell mode OFF.\r\n");

			}
			return TRUE;
		}
		
    }

	
    if (ShellDebugOn == 1  )
    {
    	char *p = NULL;
	char *ptr = NULL;
	char name[100];
	
	int re;
	int errCo;
	int i;
	int j,jj;
	char type ;
	symbol_TBL* findSymbol = NULL;
	symbol_TBL* tmpSymbol = NULL;
	int dataRight;
	int symAdr;
	char* pSaved; 
	
          dataRight = 0;
	  shellTerm = term;
	
	 p = term->lastInput ;
	while(*p ==' ') 
		p ++ ;
	if ( (*p == '\0') || (*p == 0x0a) || (*p == 0x0d) )
		return TRUE;	
	
	pSaved = p;
	termPut(term, term->lastInput);
	if  ( ! strncmp(p, "add ",4) )	
	{
	/* _printf(term,"create new  symbol\r\n");*/
	
		char newSymbName[50];
		
		int adr;
		
		int valLen;
		char * pSymbolName;
		
		if(symboltbl == NULL)
		{	
			symboltbl = (symbol_TBL*)MALLOC(SYMBOL_MAX_LEN* sizeof(symbol_TBL) ) ; /* max 10240 symbols */	
    		}
    		if(symboltbl == NULL)
    		{
    			return true;	
    		}
		p += 4;
		
		while(*p == ' ') 
			p++;
		/* type: char,short,int,char[],short[],int[] */
		if(0 == strncmp(p,"char",4) )
		{
			p+=4;
			if(*p==' ')
			{	
				valLen = 1;
			}
			else
			{
				shellCmdErrorMsg(term,pSaved,p);
				return true;	
			}
			
		}else if(0 == strncmp(p,"short",5) )
		{
			p+=5;
			if(*p==' ')
			{	
				valLen = 2;
			}
			else
			{
				shellCmdErrorMsg(term,pSaved,p);
				return true;	
			}
		}
		else if(0 == strncmp(p,"int",3) )
		{
			p+=3;
			if(*p==' ')
			{	
				valLen = 4;
			}
			else
			{
				shellCmdErrorMsg(term,pSaved,p);
				return true;	
			}
		}
		else
		{
			shellCmdErrorMsg(term,pSaved,p);
			return true;	
		}	
		p++;
		while(*p == ' ')
			p++;
		j = 0;
		/*copy symbol string*/
		if(1 == isSymbolNameFirst(*p))
		{
			newSymbName[j] = *p;
			p++;
			j++;	
		}
		else
		{
			shellCmdErrorMsg(term,pSaved,p);
			return true;
		}			
		while(1 == isSymbolName(*p))
		{
				newSymbName[j] = *p;
				p++;
				j++;
				if(j >= (50-1))
				{
					shellCmdErrorMsg(term,pSaved,p);
					return true;
				}
		}
		if(*p=='[')
		{
			int dataSize;
			p ++ ;
			dataSize = shellStr2Int(p,&p);	
			if( (dataSize<1)||(dataSize >1024*100) || ( (*p !=']') &&(*p !=' ') ))
			{
				shellCmdErrorMsg(term,pSaved,p);
				return true;	
			}
			valLen = valLen * dataSize ;
		}
		if(j <1)
		{
			shellCmdErrorMsg(term,pSaved,p);
			return true;
		}
		newSymbName[j] = 0 ;
		findSymbol = NULL ;
		findSymbol = symbolByName(newSymbName);
		if(findSymbol)
		{
			_printf(term,"symbol already exist \r\n");
			return true;
		}
		pSymbolName =(char *) malloc((j+1+8+valLen)*sizeof(char)); /*=1 for '\0' ,+8 for symbol value*/
		if(pSymbolName == NULL)
		{
			termPut(term, term->lastInput);
			return true;	
		}
		
		for(jj=0;jj<j+1+8+valLen;jj++)	
			pSymbolName[jj] = 0;
	
		for(jj=0;jj<=j;jj++)
				pSymbolName[jj] =newSymbName[jj];
		adr = (int)pSymbolName;
		adr = adr +j +1 + 1 ;
		adr = adr + 4  - adr%4; /*world align*/
		if(symbolIndex<SYMBOL_MAX_LEN)
		{
			symboltbl[symbolIndex].name = pSymbolName;
			symboltbl[symbolIndex].adr = adr;
			symboltbl[symbolIndex].type = SYMBOL_BSS;
			symboltbl[symbolIndex].len = valLen;
			_printf(term," add new symbol %s at 0x%x,len =%d.\r\n",
				symboltbl[symbolIndex].name,
				symboltbl[symbolIndex].adr,
				valLen);
			symbolIndex++;
			
		}
		else
		{
			_printf(term,"symbol table is full .\r\n");
			
		}
		termPut(term, term->lastInput);
		return true;
	}

	if  ( ! strncmp(p, "d ",2) )	
	{
		p+=2;
		while(*p == ' ')
			p++;
		if( (*p >= '0')&& ( *p <= '9' ))
		{
			///int adr;
			symAdr = (  0xfffffffc & shellStr2Int(p, &p) );	
			if( ( *p !=' ' ) && (*p !=0x0d)&&(*p!=0x0a) &&(*p!=0))
			{
				shellCmdErrorMsg(term,pSaved,p);
				return true;	
			}
			shellDisplayMemory((int*)symAdr);
		}
		else
		{
			shellCmdErrorMsg(term,pSaved,p);
			return true;		
		}
		termPut(term, term->lastInput);	
		return true;
	}

	if( (*p >= '0')&& ( *p <= '9' ))
	{
		symAdr = (  0x003ffffc & shellStr2Int(p, &p) );
		if( ( *p !=' ' ) && (*p !=0x0d)&&(*p!=0x0a) &&(*p!=0))
		{
			shellCmdErrorMsg(term,pSaved,p);
			return true;	
		}
		tmpSymbol = NULL;
		ptr = symbolNameByRange(symAdr);
		tmpSymbol = RangeAdrLast ;

		if(tmpSymbol)
		{
			_printf(term,"%08x",tmpSymbol->adr );
			if( tmpSymbol->type == SYMBOL_BSS)
			{_printf(term," .bss\t");}
			else if( tmpSymbol->type == SYMBOL_DATA)
			{_printf(term," .data\t");}
			else if( tmpSymbol->type == SYMBOL_TEXT)
			{_printf(term," .text\t");}
			else
			{_printf(term," .   \t");}
			_printf(term,"%08x %s\r\n",tmpSymbol->len, tmpSymbol->name);
			if(ptr!=NULL)
			{
	 			_printf(term, "%s + 0x%x\r\n",ptr,codeRangeAdrlast);
			}
		}
		else
		{
			_printf(term,"no symbol at 0x%x\r\n",symAdr);	
		}
		
		termPut(term, term->lastInput);	
		return true;
	}
	else if(*p == '*')
	{
		char* charTadr;
		short* shortTadr;
		unsigned int * intTadr;
		int adr;
		int data;
		p++;
		if( (*p >= '0')&& ( *p <= '9' ))
		{
			/*adr = (  0x003ffffc & shellStr2Int(p, &p) );*/
			adr =  shellStr2Int(p, &p) ;	
			if( ( *p !=' ' ) &&(*p != '=' )&& (*p !=0x0d)&&(*p!=0x0a) &&(*p!=0) )
			{
				shellCmdErrorMsg(term,pSaved,p);
				return true;	
			}
		}
		else
		{
			shellCmdErrorMsg(term,pSaved,p);
			return true;
		}
		if(adr%4 == 1)
		{
			charTadr = (char * )adr ;
			while(' ' == *p ) 
			p++;
			if(*p == '=')
			{
				p ++ ;
				data = shellStr2Int(p, &p) ;
				if( ( *p !=' ' ) && (*p !=0x0d)&&(*p!=0x0a)&& (*p !=0x0))
				{
					shellCmdErrorMsg(term,pSaved,p);
					return true;	
				}
				*charTadr = (char)data ;
				_printf(term,"\t*%08x=%02x\r\n",adr,data);
			}
			else
			{
				_printf(term,"\t*%08x=%02x\r\n",adr,*charTadr);	
			}		
		}
		else if(adr%4 == 2)
		{
			shortTadr = (short*)adr;
			while(' ' == *p ) 
			p++;
			if(*p == '=')
			{
				p ++ ;
				data = shellStr2Int(p, &p) ;
				if( ( *p !=' ' ) && (*p !=0x0d)&&(*p!=0x0a)&& (*p !=0x0))
				{
					shellCmdErrorMsg(term,pSaved,p);
					return true;	
				}
				*shortTadr = (short)data;
				_printf(term,"\t*%08x=%04x\r\n",adr,data);
			}
			else
			{
				_printf(term,"\t*%08x=%04x\r\n",adr,*shortTadr);	
			}
			
		}
		else if(adr%4 == 0)
		{
			intTadr = (unsigned int*)adr;
			while(' ' == *p ) 
			p++;
			if(*p == '=')
			{
				p ++ ;
				data = shellStr2Int(p, &p) ;
				if( ( *p !=' ' ) && (*p !=0x0d)&&(*p!=0x0a)&& (*p !=0x0))
				{
					shellCmdErrorMsg(term,pSaved,p);
					return true;	
				}
				*intTadr = (unsigned int)data;
				_printf(term,"\t*%08x=%08x\r\n",adr,data);
			}
			else
			{
				_printf(term,"\t*%08x=%08x\r\n",adr,*intTadr);	
			}
		}
		termPut(term, term->lastInput);
		return true;
	}
	else if(*p == '&')
	{
		i = 0;
		p++ ;
		if(1 == isSymbolNameFirst(*p))
		{
			name[i] = *p;
			p++;
			i++;
			while(1 == isSymbolName(*p))
			{
				name[i] = *p;
				p++;
				i++;
				if(i>90)
				{
					shellCmdErrorMsg(term,pSaved,p);
					return true;
				}	
			}
			name[i]=0;
			name[i+1]=0;
			tmpSymbol = symbolByName(name);	
			if(tmpSymbol)
			{
				_printf(term,"%08x",tmpSymbol->adr );
				if( tmpSymbol->type == SYMBOL_BSS)
				{_printf(term," .bss\t");}
				else if( tmpSymbol->type == SYMBOL_DATA)
				{_printf(term," .data\t");}
				else if( tmpSymbol->type == SYMBOL_TEXT)
				{_printf(term," .text\t");}
				else
				{_printf(term," .   \t");}
				_printf(term,"%08x %s\r\n",tmpSymbol->len, tmpSymbol->name);
			}	
		}
		else
		{
			shellCmdErrorMsg(term,pSaved,p);	
			return true;
		}
		termPut(term, term->lastInput);
		return true;
	}
	
	for(i=0;i<100;i++)
		name[i] = 0;
	i = 0;
	if(1 == isSymbolNameFirst(*p))
	{
		name[i] = *p;	
		p ++ ;
		i ++ ;
		while(1 == isSymbolName(*p))
		{
			name[i] = *p;	
			p ++ ;
			i ++ ;
			if(i>90)
			{	/*not a symbol .goto cmd line interface*/
				return false;
			}		
		}
	}
	else
	{
		return false;	
	}
	name[i]=0;
	name[i+1]=0;
	findSymbol = NULL ;
	findSymbol = symbolByName(name);
	if(findSymbol != NULL)	/*findsymbol*/
	{
		type = findSymbol->type;
		switch(type)
		{
		
			case SYMBOL_DATA:
			case SYMBOL_BSS:
			{
				i = 0;
				while(*p == ' ')
					p ++;
				if( (*p == 0)|| (*p == 0x0a) || (*p == 0x0d) ) /*read data*/
				{
					switch(findSymbol->len)
					{
						case 1:
						{
							char *charTadr = (char*)(findSymbol->adr);
							_printf(term,"(char):%s=0x%02x,%d.\r\n",findSymbol->name,*charTadr,*charTadr);			
							ptr = symbolNameByRange(*charTadr);
							if(ptr != NULL)
							{
								_printf(term,"\t %s+0x%x \r\n",ptr,codeRangeAdrlast);		
							}
						}
						break;
						case 2:
						{
							short *shortTadr = (short*)( (findSymbol->adr) &0x003ffffe);
							_printf(term,"(short):%s=0x%04x,%d.\r\n",findSymbol->name,*shortTadr,*shortTadr);			
							ptr = symbolNameByRange(*shortTadr);
							if(ptr != NULL)
							{
								_printf(term,"\t %s+0x%x \r\n",ptr,codeRangeAdrlast);		
							}
							
						}
						break;
						case 4:
						{
							int *intTadr = (int*)( (findSymbol->adr) &0x003ffffc);
							_printf(term,"(int):%s=0x%08x,%d.\r\n",findSymbol->name,*intTadr,*intTadr);			
							ptr = symbolNameByRange(*intTadr);
							if(ptr != NULL)
							{
								_printf(term,"\t %s+0x%x \r\n",ptr,codeRangeAdrlast);		
							}
							
						}
						break;
						default :
						{
							int *intTadr = (int*)( (findSymbol->adr) &0x003ffffc);
							unsigned char *p;
							int i;
							_printf(term,"%s=%08x,%08x,%08x,%08x.\r\n \t\t \"",
								findSymbol->name,*intTadr,*(intTadr+1),*(intTadr+2),*(intTadr+3));			
							p = (unsigned char *)findSymbol->adr;
							for(i=0;i<20;i++)
							{
								if( (*p > 0x19)&&( *p < 0x80 ))
								{
								_printf(term,"%c",*p);	
								}
								else
								{
								_printf(term,".");		
								}	
								p++;
							}
							_printf(term,"\"\r\n");
							ptr = symbolNameByRange(*intTadr);
							if(ptr != NULL)
							{
								_printf(term,"\t %s+0x%x \r\n",ptr,codeRangeAdrlast);		
							}
							
						}
						break;	
					
					}
					
				}
				else if(*p == '=') /*write data-start*/
				{
					int stringFound=0;
					i = 0;
					p++;
					while(*p == ' ' ) 
					{
						p ++ ;
						
					}
					
					if( (*p >= '0')&& ( *p <= '9' ))
					{
						dataRight =  shellStr2Int(p, &p);
						if( ( *p !=' ' ) && (*p !=0x0d)&&(*p!=0x0a)&& (*p !=0))
						{
							shellCmdErrorMsg(term,pSaved,p);
							return true;	
						}
					}
					else if( (*p == '-' ) && (*(p+1) >= '0')&& ( *(p+1) <= '9' ) )
					{
						p++;
						dataRight = 0 - shellStr2Int(p, &p);
						if( ( *p !=' ' ) && (*p !=0x0d)&&(*p!=0x0a)&& (*p !=0))
						{
							shellCmdErrorMsg(term,pSaved,p);
							return true;	
						}
					}
					else if( (*p == '\"')||(*p == '\'') ) /*string*/
					{
						
						if ( findSymbol->len	> 4 )
						{
							char *charTadr;
							char * pTmp;
							int len;
							int itmp,iitmp;
							stringFound = 1;
							
							charTadr = (char*)(findSymbol->adr);
							len = findSymbol->len ;
							p++;
							iitmp = 0;
							pTmp = p;
							while( (*pTmp != '\"' ) 
								&& (*pTmp != 0) 
								&& (*pTmp != '\'') 
								&& (*pTmp != 0x0d) 
								&& (*pTmp != 0x0a) )
							{
								pTmp ++;
								iitmp ++;
								if(iitmp >= len)
									break;	
							}
							if( (*pTmp == '\"' ) || (*pTmp == '\''))
							{
								/* parse string OK*/
								;
							}
							else
							{
								shellCmdErrorMsg(term,pSaved,pTmp);	
								return true;	
							}
							for(itmp=0;itmp<iitmp;itmp++)
							{
								*charTadr++ = *p++ ;	
							}
							for(;itmp<len;itmp++)
							{
								*charTadr++ = 0;
							}
							if(*p != '\"')
							{
								while( (*(charTadr+1) == 0) && (*p != '\"') &&(iitmp < (findSymbol->len-1)) )
								{
									*charTadr++ = *p++ ;	
									iitmp ++;	
								}
								*charTadr++ = 0;	
							}
							
						}
						else
						{
							dataRight = *(p+1);
							p ++ ;
						}
					}
					else if ( ((*p == 'N' ) && (*(p+1)=='U') && (*(p+2) == 'L' ) && (*(p+3) == 'L' ))
							|| ((*p == 'n' ) && (*(p+1)=='u') && (*(p+2) == 'l' ) && (*(p+3) == 'l' ))
						)
					{
						
						dataRight = 0;
					}
					else
					{
						for(i=0;i<100;i++)
							name[i] = 0;
						i = 0;
						while (isSymbolName(*p))	
						{
							name[i] = *p;
							p++;
							i++;
							if(i>90)
							{
								shellCmdErrorMsg(term,pSaved,p);	
								return true;
							}		
						}	
						name[i]=0;
						name[i+1]=0;
						tmpSymbol = symbolByName(name);
						if(tmpSymbol)
						{
							if( tmpSymbol->type == SYMBOL_TEXT )
							{/*function*/
								int errCotmp;
								char * ptrError;
								/*_printf(term," calling symbolFuncParse(%08x,%s,%08x)\r\n",
									tmpSymbol->adr,p,&errCotmp);
								*/
								re = symbolFuncParse(tmpSymbol->adr,p,&errCotmp,&ptrError);	
								if(0==errCotmp)
								{
									dataRight = re;
								}
								else
								{
									shellCmdErrorMsg(term,pSaved,ptrError);	
									return true;
								}	
							}
							else
							{
								if(tmpSymbol->len == 1)
								{
									dataRight = 	*(char *)(tmpSymbol->adr);
								}
								else if(tmpSymbol->len == 2)
								{
									dataRight = 	*(short *)((tmpSymbol->adr)&0xfffffffe);
								}else 
								{
									dataRight = 	*(int *)((tmpSymbol->adr)&0xfffffffc);
								} 
								
							}
						}
						else
						{
							_printf(term,"symbol not found .\r\n");	
						}
					}
					
					
					switch(findSymbol->len)
					{
						case 1:
						{
							char *charTadr = (char*)(findSymbol->adr);
							*charTadr = dataRight ;
							_printf(term,"DATA(char):%s=0x%02x,%d.\r\n",findSymbol->name,*charTadr,*charTadr);			
							ptr = symbolNameByRange(*charTadr);
							if(ptr != NULL)
							{
								_printf(term,"\t %s+0x%x \r\n",ptr,codeRangeAdrlast);		
							}
						}
						break;
						case 2:
						{
							short *shortTadr = (short*)( (findSymbol->adr) &0x003ffffe);
							*shortTadr = dataRight ;
							_printf(term,"DATA(short):%s=0x%04x,%d.\r\n",findSymbol->name,*shortTadr,*shortTadr);			
							ptr = symbolNameByRange(*shortTadr);
							if(ptr != NULL)
							{
								_printf(term,"\t %s+0x%x \r\n",ptr,codeRangeAdrlast);		
							}
							
						}
						break;
						case 4:
						{
							int *intTadr = (int*)( (findSymbol->adr) &0x003ffffc);
							*intTadr = dataRight ;
							_printf(term,"DATA(int):%s=0x%08x,%d.\r\n",findSymbol->name,*intTadr,*intTadr);			
							ptr = symbolNameByRange(*intTadr);
							if(ptr != NULL)
							{
								_printf(term,"\t %s+0x%x \r\n",ptr,codeRangeAdrlast);		
							}
							
						}
						break;
						default :
						{
							int *intTadr = (int*)( (findSymbol->adr) &0x003ffffc);
							if(stringFound == 0)
							{
								*intTadr = dataRight ;
								_printf(term,"DATA():%s=%08x,%d.\r\n",
									findSymbol->name,*intTadr,*intTadr);			
								ptr = symbolNameByRange(*intTadr);
								if(ptr != NULL)
								{
									_printf(term,"\t %s+0x%x \r\n",ptr,codeRangeAdrlast);		
								}
							}
						}
						break;	
					
					}	
				}	
			}break; /*SYMBOL_DATA*/
			case SYMBOL_TEXT:
			{
				int re;
				char *ptrError;
				
				re = symbolFuncParse(findSymbol->adr,p,&errCo,&ptrError);
				if(0==errCo)
				{
					_printf(term," return %d,0x%x \r\n",re,re);	
					termPut(term, term->lastInput);
					return true;
				}
				else
				{
					shellCmdErrorMsg(term,pSaved,ptrError);
					return true;
				}
				

			}break; /* SYMBOL_TEXT*/
			default:
			break;
		} /* switch(type)	case SYMBOL_BSS \ SYMBOL_DATA \ SYMBOL_TEXT*/
		return true;
	}
	else	/*not found symbol*/
	{

		/* _printf(term,"create new  symbol:not implemented yet. \r\n");*/
		
		return false;
		/*if(thisSymbol!=NULL)*/
     	}			
	
	/*creat new task*/
	
	if(! strncmp(p, "ctt",3) )
	{
		_printf(term,"creat new task :not implemented yet. \r\n");	
		termPut(term, term->lastInput);
		return true;	
	}
	
    return FALSE;
    }    /* shell on */
 
    return FALSE;
}

#endif

/*utt-zhangdl-030826-added -end*/	

