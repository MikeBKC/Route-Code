#ifndef LICENSE_SHARE_H
#define LICENSE_SHARE_H
#include "mibtypeShare.h"
#include "mibunionShare.h"

#if ( UTT_LICENSE == FYES )

#define PARTCLEN 		10		/*密钥长度，表示由十个随机数组成一个密钥*/
#define MACLEN			0x0c 
#define MAXUSERLEN		32 

typedef unsigned char	    uchar;

typedef struct stlicense {
	mibHead		 head;
	int 	   	 len;				/*license整个加密文件的长度，表示字节数*/
 	char       	 key[ PARTCLEN + 1 ];   	/*随机密钥*/
	int	   	 controltype;			/*license控制类型*/
        uint		 impowerTime;    		/*临时授权时间*/
	int	   	 timeflag;			/*license状态*/
        uint     	 runtime;               	/*license已使用时间*/
	volatile int	 isOutdate;			/*临时license过期 or 未过期，0-未过期，1-过期*/
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
	char		 content[300];

	volatile uchar	 authResult;
	int		 taskRunning;

}LicenseProfile;

#endif

#endif
