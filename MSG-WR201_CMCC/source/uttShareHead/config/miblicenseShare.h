#ifndef LICENSE_SHARE_H
#define LICENSE_SHARE_H
#include "mibtypeShare.h"
#include "mibunionShare.h"

#if ( UTT_LICENSE == FYES )

#define PARTCLEN 		10		/*��Կ���ȣ���ʾ��ʮ����������һ����Կ*/
#define MACLEN			0x0c 
#define MAXUSERLEN		32 

typedef unsigned char	    uchar;

typedef struct stlicense {
	mibHead		 head;
	int 	   	 len;				/*license���������ļ��ĳ��ȣ���ʾ�ֽ���*/
 	char       	 key[ PARTCLEN + 1 ];   	/*�����Կ*/
	int	   	 controltype;			/*license��������*/
        uint		 impowerTime;    		/*��ʱ��Ȩʱ��*/
	int	   	 timeflag;			/*license״̬*/
        uint     	 runtime;               	/*license��ʹ��ʱ��*/
	volatile int	 isOutdate;			/*��ʱlicense���� or δ���ڣ�0-δ���ڣ�1-����*/
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
	char		 content[300];

	volatile uchar	 authResult;
	int		 taskRunning;

}LicenseProfile;

#endif

#endif
