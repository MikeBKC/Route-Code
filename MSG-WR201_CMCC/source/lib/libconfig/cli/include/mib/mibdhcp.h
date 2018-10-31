#ifndef _MIBDHCP_H
#define _MIBDHCP_H

//# if ( DHCP_PLUS == FYES )

#define MAX_REMOTEID_LEN	32
#define MAX_CIRCUITID_LEN	32	
#define MAX_CLIENTID_LEN	32

#define MAX_DOMAINNAME_LEN	32
#define MAX_HOSTNAME_LEN	16
#define MAX_PARM_REQ_LIST	32
#define	DHCP_OPTIONS_MIN	312
#define MAX_RAWOPTION_LEN	32

//#define MAX_MAC_LEN		6
#define MAX_MAC_LEN		ETHER_ADDR_LEN
//#define MAX_PORTS		3
#define MAX_PORTS		NUM_ETHER_IF
//#define MAX_PROFILE_NAME_LEN	11
#define MAX_PROFILE_NAME_LEN	MAX_PROFILE_NAME_LENGTH
	
struct RawOptionClassProfile
{
	char name[MAX_PROFILE_NAME_LEN+1]; //ʵ��������
	Boolean active;
	Uint8 	type; //option������
 	//ulong	len; //option�ĳ���
 	char	value[MAX_RAWOPTION_LEN]; //option������
	Uint8	bindingport; //�󶨵��Ķ˿�,enum value:eth1,eth2,eth3,see cmdlist.c
};

typedef enum {
	broadcast,
	unicast,
	clientflag,
}OVERLOAD_FLAGS;

typedef enum {
	disable,
	insert
}RELAY_OPTION;

typedef enum{
	keep,
	drop,
	replace
}RELAY_POLICY;

typedef enum {
      Bnode = 0x1,// B�ڵ㽫�㲥NetBIOS Name Queries��������ע�������
      Pnode = 0x2, // P�ڵ㲻ʹ�ù㲥;��֮�� ��ֱ�Ӳ�ѯ���ַ�������ѯ
      Mnode = 0x4, // M�ڵ���B�ڵ��P�ڵ�Ľ�ϣ��ȳ���B�ڵ������ٳ���P�ڵ�����
      Hnode = 0x8 // H�ڵ���P�ڵ��B�ڵ�Ľ�ϣ��ȳ���P�ڵ������ٳ���B�ڵ�����
}NBT;

struct DhcpPoolClassProfile{
     	char name[MAX_PROFILE_NAME_LEN + 1] ; //poolʵ������
	Boolean active;
	IpAddress poolstart;  //��ַ�ص���ʼ��ַ
    	Uint32  poolcount;   //��ַ�صĵ�ַ����
    	IpAddress netmask; //��ַ����
	IpAddress gateway; //����
	IpAddress PriDNS; //��DNS��ַ
	IpAddress SecDNS; //����DNS��ַ
	IpAddress PriWINS; //��WINS��ַ
	IpAddress SecWINS; //����WINS��ַ
	Boolean  bAutoIPcoexist;
	NBT	NBTType; //Client������NETBIOS�ڵ�����
	char domainName[MAX_DOMAINNAME_LEN];//����
	char	circuitId[MAX_CIRCUITID_LEN];
	u_long leasetime; //��ַ���ڷ����ȥ�ĵ�ַ������
	IpAddress giaddr; //relay agent��ip����Ϊһ�ַ������
	OVERLOAD_FLAGS flags;
	Uint8   bindingport; //�󶨵��Ķ˿�,enum value:eth1,eth2,eth3,see cmdlist.c
};

 //# endif /* DHCP_PLUS == FYES */
#endif //_MIBDHCP_H
