/* �ͺ� */
var platform = "nvA755W";
/* һ���˵� */
var NetPara=1;
var WlsConf=1;
var AdvConf=1;
var wlanMng=0;	//AP���й���
var SwFun=0;
var UserMng=1;
var PM=1;
var BM=1;
var Fw=1;
var VPN=1;
var Urcp=0;
var SysMng=1;
var SysSts=1;
var CusSrv=1;

var total=17;

/* �����˵� */
var FastConfig=1;	//��������
var RunStatus=1;	//����״̬
var InterfaceFlow=1;	//�ӿ�����
var stat_detailed = 0;
var ReBoot=1;	//�����豸

//��������
var WANConfig=1;	//WAN������
var DoubleRoad=0;	//��·���
var portnumber=0;	//�˿���Ŀ
var LANConfig=1;	//LAN����
var DhcpServer=1;	//DHCP������
var DDNS=1;	//DDNS
var UPnP=1;	//UPnP
var fitApIfConf=0;	//��AP LAN������
var DhcpServerForAc=0;	//Fat ap DHCP������
var WANPort=1;		//WAN���������ã���̬�˿ڣ�

//��������
var WlanBase=1;	//��������
var remove_apclient = 1;/*ȥ��APClient*/
var WlanSecurity=1;	//���߰�ȫ
var WlanMacFilter=1;	//����MAC��ַ����
var WlanAdvanced=1;	//�߼���������
var WlanMultiSsid=0;	//��SSID
var WlanHostInfo=1;	//���������б�
var fatFitSwitch=0;
var wlanPara=0;	//���߲�������
var apParaSwitch=0;	//ѡ������

//AP���й���
var apManage=0;   //AP����
var serviceZone=0;   //������
var apTmp=0;   //AP����ģ��
var apSoftMng=0;   //AP�������
var getApConf=0;	//AP�����ļ�
var wlanCltList=0;   //���߿ͻ���

//�߼�����
var NatStatic=1;	//��̬NAT
var Route=1;	//·������
var PolicyRoute=1;
var AntiNetSnifer=0;	//������
var Hotel=0;		//���弴��
var PortMirror8367M=0;
var PortMirror=0;
var PortMirror840E=0;
var PortMirror6530G=0;	//6530G�Ķ˿ھ�����
var port_vlan_rdd=0; /*�˿�Vlan*/
var PortSpeed=0;
var Syslog=1;
var Cache=0;   

//���繲��
NetShareManage=1;
FtpServer=1;
ShareAccountList=1;

//�������� 
var ports=0;	//�˿ڹ���
var mirror=0;	//�˿ھ���
var port_vlan=0;	//�˿�VLAN
var aggregation=0; //1000m
var aggregation_100M=0;	//100M�˿ڻ��

//�û�����
var UserLinkRate=1;	//�û���������
var PPPoEGlobal=1;	//PPPoEȫ������
var IPMac=0;	//IP-Mac��
var WebAuth = 0; /*��ҳ��֤*/

//��Ϊ����
var GlobalBase=0;	//������Ϊ����
var PdbUpdata=0;	//���Կ�
var Notice=1;	//ͨ��
var FwAddress=1;	//��ַ��
var UserGroup=1;	//�û���
var TimeRange=1;	//ʱ���
var QQExcept=0;		//QQ������
var MSNExcept=0;	//MSN������
var NetAction=1;	//������Ϊ���

//�������
var Group=1;	//��ϸ������
var SmartQos=1;	//���Դ���
var P2PLimit=1; //P2P����

//����ǽ
var FwBase=1;	//��ȫ����
var FireWall=0;	//���ʿ��Ʋ���
var DnsFilter=1;	//��������
var ConnectionLimit=1;	//��������
var MacFilter=1;	//MAC��ַ����
var ParentsControl=0;	//�ҳ�����

//VPN
var PPTPClientList=1;	//PPTP
var Encrypt = 1;     /*mppe����*/
var L2TPList=0;	//L2TP
var IPSEC=1;	//IPSec

//��������
var discover=0;	//��������
var linkageConf=0;	//��������
var topoy=0;	//��������

//ϵͳ����
var User=1;	//����Ա����
var EasyTask=0;	//�׹���
var langChange=1;	//����ѡ��
var SysClock=1;	//ϵͳʱ��
var DeviceConfig=1;	//���ù���
var SoftwareUpdate=1;	//�������
var Remote=1;	//Զ�̹���
var TaskScheduler=1;	//�ƻ�����
var DeviceLoginControl=0;	/*�豸��½����*/
var SSH=0;   /*�豸���ʿ������SSH�Ƿ���ʾ*/
var Https=0;	/*�豸���ʿ������Https�Ƿ���ʾ*/	
var License=0; //License
var DeviceLoginControl=0;   /*�豸��½����*/
var SSH=0;   /*�豸���ʿ������SSH*/
var Https=0;  /*�豸���ʿ������Https*/

//ϵͳ״̬
var RunStatus=1;	//����״̬
var FluxStat=0;	//����ͳ��
var SysInfo=1;	//ϵͳ��Ϣ
var SysLogInfo=1;       /*ϵͳ��־*/
var UserStats=0;	//�û�״̬
var fitApClts=0;//��AP�ͻ����б�
var fitApSysInfo=0;	//��APϵͳ��Ϣ

//�ͻ�����
var SaleService=1;	//�ͻ�����

//1.1�ڶ���ҳ��
var WebAuth=0;	//��ҳ��֤
var AddressGroup=1; //��ַ��
var UserGroup=1; //�û���
var SeviceGroup=1; //������
var TimeRange=1; //ʱ���
var QQWhiteList=1; //QQ������
var MSNWhiteList=1; //MSN������
var UPnP=1; //���弴��

//AP���й���
var apManage=0;   //AP����
var serviceZone=0;   //������
var apTmp=0;   //AP����ģ��
var apSoftMng=0;   //AP�������
var getApConf=0;	//AP�����ļ�
var wlanCltList=0;   //���߿ͻ���
var wlanMng=0;	//AP���й���
var fitApIfConf=0;	//��AP LAN������
var DhcpServerForAc=0;	//Fat ap DHCP������
var fatFitSwitch=0;
var wlanPara=0;
var apParaSwitch=0;
var Cache=0;
var fitApClts=0;
var fitApSysInfo=0;
var AliExcept=0;
