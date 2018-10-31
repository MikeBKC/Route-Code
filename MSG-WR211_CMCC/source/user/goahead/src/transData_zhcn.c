
#include "translate.h"
const char* transData_zhcn[] = {
    "�޷���ȡ������ѡ��",//C_LANG_INDEX_CACHE_LIST_FAIL
	"�޷���ȡ�ռ�ʹ����",//C_LANG_INDEX_CACHE_SIZE_FAIL
	"�޷���ȡ����ͳ������",//C_LANG_INDEX_BANDWIDTH_FAIL
	"��������",//C_LANG_INDEX_BANDWIDTH_REQUEST
	"ʵ������",//C_LANG_INDEX_BANDWIDTH_RESPONSE
	"��ʡ����",//C_LANG_INDEX_BANDWIDTH_SAVE
	"������",//C_LANG_INDEX_CACHE_TOTAL
	"��ʹ��",//C_LANG_INDEX_CACHE_USED
	"δʹ��",//C_LANG_INDEX_CACHE_FREE,
	"����Ʒ���״̬ʧ��",//C_LANG_INDEX_CACHE_CHECK_FAIL
	"����ʧ��",//C_LANG_INDEX_OPERATION_FAIL
	"�豸���ʿ��ƣ�ip��ַ�Σ��������",//C_LANG_INDEX_DECIVE_ACCESS_CTL_IP_ERR
	"IP ��ַ����!",//C_LANG_INDEX_IP_PLAN_ADDR_ERR
	"�������ظ���",//C_LANG_INDEX_STRATEGY_NAME_REPEATED
	"�Ѵ������Ŀ��",//C_LANG_INDEX_MAXIMUM_NUM_HAS_REACHED
	"�Ҳ����ò�������",//C_LANG_INDEX_CANT_FIND_STRA_NAME
	"�������Ѿ����ڣ�",//C_LANG_INDEX_STRATEGY_NAME_EXISTS
	"��ȡedit_Name����ʧ�ܣ�",//C_LANG_INDEX_READ_EDIT_NAME_VAL_ERR
	"����������Ϊ��",//C_LANG_INDEX_STRATEGY_NAME_CANT_NULL
	"�Ҳ�����������",//C_LANG_INDEX_CANT_FIND_THE_GROP_CONF
	"�����û�",//C_LANG_INDEX_ALL_USERS
	"�Ҳ����ò�������\n",//C_LANG_INDEX_CANT_FIND_STRA_NAME_GN
	"��",
	"ʱ",
	"��",
	"��",
	"��",
	"ʱ",
	"��",
	"��",
	"���ò�������",//C_LANG_INDEX_CINFIG_PARAMETER_WRONG
	"��Ч��ַ����ʼ��ַ",
	"��Ч�����IP��ַ",
	"��Ч��ַ�ص�ַ��",
	"��������ַ���ܰ����ڵ�ַ����",
	"��ַ�ز��ܰ����ӿ�IP",//C_LANG_INDEX_ADDR_POOL_CANT_CONTAIN_INF_IP
	"��̬ӳ���Ѿ��ﵽ����������޷�����pptp��Ҫ�ľ�̬ӳ��",
	"��������",
	"IP��ַ�������",
	"Զ���������β����뱾�����������ص�",
	"������������ظ���",
	"�Ѵ������Ŀ��",
	"�Ҳ�����������ƣ�",
	"��ȡ����ʧ��",//C_LANG_INDEX_PPTP_SERVER_READ_DATA_ERR
	"Զ�����������ַ�������",
	"Զ���������������������",
	"Զ���������β����뱾�����������ص�",
	"��Ч������֤��ʽ",
	"��������Ѵ��ڣ�",//C_LANG_INDEX_PPTP_CLIENT_TUNNEL_EXITS_ERR
	"��ȡ����ʧ��",
	"�û������ܳ���11���ֽڣ�",
	"���벻�ܳ���31���ֽڣ�",
	"����ʧ�ܣ�ϵͳ�д���ͬ���˺ţ�",
	"��ֹ�޸�Ĭ���û���",
	"�Ҳ������û�",// C_LANG_INDEX_CANT_FIND_USER_NAME
	"����ɾ��Ĭ�Ϲ���Ա",//C_LANG_INDEX_CANT_DEL_DEFAULT_USER
	"����������Ϊ��",
	"�����������ظ���",
	"�ƻ����񼺾��ﵽ����������������µļƻ�����",
	"�Ҳ�������������",
	"���������Ȳ��ܳ���11λ��",//C_LANG_INDEX_TASK_NAME_SHOULD_LESS_11
	"��ЧIP��ַ",	    //C_LANG_INDEX_INVALID_IP
	"WAN��IP��ַ�������óɹ㲥��ַ",
	"LAN��IP��ַ������WAN��IP��ͬһ����",
	"��Ч���뷽ʽ",
	"���²���ʧ�ܣ��Բ��þɲ���!",
	"���²��Գɹ�!",
	"��Ч�ӿ�",
	"��������AP����ģʽ APClient Mode",
	"��ȡedit_Name����ʧ��!",	//C_LANG_INDEX_READ_EDIT_NAME_ERR
	"Դ��ַ·�ɵ���ʼ��ַ���ڽ�����ַ",
	"���IP������LAN������IP",
	"���IP������WAN��IP",
	"LAN����ʧ��",
	"DHCP����������",
	"�䶯���DHCP��ַ�ؽ�����LAN�ڵ�ַ������ʧ��",
	"DHCP��ַ�ذ�����LAN�ڵ�ַ",
#if (MULTI_SSID_DHCP_POOL == FYES)
	"�䶯�������SSID1 DHCP��ַ�ؽ�����LAN�ڵ�ַ������ʧ��",
	"����SSID1 DHCP��ַ�ذ�����LAN�ڵ�ַ",
	"�䶯�������SSID2 DHCP��ַ�ؽ�����LAN�ڵ�ַ������ʧ��",
	"����SSID2 DHCP��ַ�ذ�����LAN�ڵ�ַ",
#if (WIRELESS_5G == FYES)
	"�䶯������� 5G SSID1 DHCP��ַ�ؽ�����LAN�ڵ�ַ������ʧ��",
	"���� 5G SSID1 DHCP��ַ�ذ�����LAN�ڵ�ַ",
	"�䶯������� 5G SSID2 DHCP��ַ�ؽ�����LAN�ڵ�ַ������ʧ��",
	"���� 5G SSID2 DHCP��ַ�ذ�����LAN�ڵ�ַ",
#endif
#endif
	"��ӿڰ�����LAN�ڵ�ַ",
	"�Ҳ������û���",//C_LANG_INDEX_USER_NAME_NOT_FIND
	"����ɾ��Զ�̹���˿�ӳ��",// C_LANG_INDEX_NAT_MAP_DEL
	"�Ҳ����þ�̬ӳ����",
	"����ɾ��ϵͳĬ�Ͼ�̬ӳ��",
	"�û�������Ϊ��!",// C_LANG_INDEX_USER_NAME_NOT_NULL
	"��̬ӳ��������Ϊ��!",
	"�����ظ�",//C_LANG_INDEX_NAME_REPEAT
	"�Ҳ�������ӳ��!",// C_LANG_INDEX_NAT_MAP_NOT_FIND
	"�����½������޸�ϵͳĬ�Ͼ�̬ӳ��",
	"�ⲿ�˿��ص�!",
	"��ͬIP�ڲ��˿��ص�!",
	"��̬ӳ�����Ѿ�����!",
	"����ɾ��Ĭ�Ϲ���",
	"�Ҳ����ù���",
	"�������ظ�",
	"���ⲿip������!",
	"�ⲿIP��ַ���ص�",
	"�ڲ���ʼIP��ַ����LAN����ͬһ����!",//C_LANG_INDEX_INNER_START_IP_LAN
	"�ڲ�����IP��ַ����LAN����ͬһ����!",// C_LANG_INDEX_INNER_END_IP_LAN
	"�ⲿIP��ַ�β��ܰ������ӿ�IP��ַ!",// C_LANG_INDEX_EXTER_IP_NOT_INCLUDE
	"�ⲿIP��ַ�β��ܵ��ڸ��ӿ�IP��ַ!",//C_LANG_INDEX_EXTER_IP_NOT_EQUAL
	"�ڲ�IP��ַ�β��ܰ������ӿ�IP��ַ!",//C_LANG_INDEX_INNER_IP_NOT_INCLUDE
	"�ڲ�IP��ַ�β��ܵ��ڸ��ӿ�IP��ַ!",
	"�Ҳ���·������!",
	"Ŀ�ĵ�ַ����",
	"��ַ/�������",
	"���ص�ַ����",
	"Ŀ�����粻�ܰ�������IP��ַ",
	"���ȼ�����",
	"·�����������ظ�!",
	"��������Ϊ��!",
	"�����Ѿ�����!",
	"��ip�������ܴ���",
	"�����ò�����!",//C_LANG_INDEX_THE_CONFIG_NOT_EXIST,
	"�޷��ύ���ò�����ʹ���޷���¼�豸!",//C_LANG_INDEX_SUBMIT_FOR_NO_LOGIN
	"�Ҳ���������",
	"�޷��ύ��ֻ����IP/MAC���û������ò�����ʹ�����ڰ��û��б���޷���¼�豸",
	"�޷��ύ���ò�����ʹ����¼��MAC����û��б����MAC��һ�£��޷���¼�豸",
	"�޷��ύ���ò�����ʹ����¼��IP����û��б��IP��һ�£��޷���¼�豸",
	"����",
	"�����ݣ�ʧ��",
	"��",//C_LANG_INDEX_FAIL_BIND_NUM
	"��ȡ����ʵ����ʧ��!",//C_LANG_INDEX_READ_INST_NAME_ERR
	"�Ѵ����������Ŀ���޷���������ã�",//C_LANG_INDEX_NOT_ADD_NEW
	"�û���������sys.��ͷ",//C_LANG_INDEX_NAME_NOT_BEGINE_WITH
	"�����ظ�!",//C_LANG_INDEX_INPUT_REPEAT
	"�û��������ظ�!",//C_LANG_INDEX_USER_INPUT_REPEAT
	"��IP��ַ�����󶨣��޷��ظ���!",//C_LANG_INDEX_IP_HAVE_BINDED
	"��MAC��ַ�����󶨣��޷��ظ���!",//C_LANG_INDEX_MAC_HAVE_BINDED
	"��IP��MAC��ַ�����󶨣��޷��ظ���!",//C_LANG_INDEX_IPMAC_HAVE_BINDED
	"��Ч��ʼIP��ַ",//C_LANG_INDEX_INVALID_START_IP_ADDR
	"��Ч��DNS������",
	"��Ч��DNS������",//C_LANG_INDEX_INVALID_SEC_DNS_SERVER
	"��Чϵͳ���Ự��",
	"���ڷǵ�ַ���ڵĹ̶�IP����",//C_LANG_INDEX_EXIT_FIX_IP_IN_POOL
	"LAN��IP��ַ���ܰ�����PPPoE��ַ����",//C_LANG_INDEX_LAN_IP_CANT_IN_PPPoE_POOL
	"�̶�IP��ַ�������",
	"������ȡ����",
	"�̶�IP��ַ����PPPoE��ַ���ڣ�",//C_LANG_INDEX_STATIC_IP_ADDR_CANT_IN_PPPoE_POOL
	"��ֹ���ѷ���IP",//C_LANG_INDEX_BANNED_BOUND_ALREADY_ASSIGN_IP
	"�û����Ѵ��ڣ�",//C_LANG_INDEX_USER_NAME_ALREADY_EXITS
	"�̶�IP��ַ�Ѵ���",//C_LANG_INDEX_STATIC_IP_ADDR_ALREADY_EXITS
	"ʵ���Ѵ���",//C_LANG_INDEX_CASE_ALREADY_EXITS
	"�Ѵ������Ŀ��",//C_LANG_INDEX_ALREADY_THE_MAX_TITLE
	"�޸ĵ�ʵ�������ڣ�",//C_LANG_INDEX_CHANGED_CASE_NOT_EXITS
	"���û���",//C_LANG_INDEX_BE_USER_GROUP
	"���ã��޷��޸ģ�",//C_LANG_INDEX_USE_CANT_BE_CHANGE
	"�Ҳ����û�����",
	"���ã��޷�ɾ����",
	"������",
	"����",
	"������",
	"����",//C_LANG_INDEX_EXPIRE
	"��ѡ���û�",
	"δ�ҵ�ʵ����",//C_LANG_INDEX_DID_NOT_FIND_CASE
	"���ò�������",
	"��û��Ȩ�޽��д˲�����",
	"�ύ�����������������룡",
	"�ύ����������������ȷ�����벻ͬ��",//C_LANG_INDEX_SUBMIT_WRONG_PARMETERS_PASSWD_DIFF
	"�����ɹ�",//C_LANG_INDEX_SUCCESSFUL_OPERATION
	"��������д����",//C_LANG_INDEX_OLD_PASSWD_CIN_ERR
	"�޸��ѳ���5�Σ����ղ����޸ģ�",//C_LANG_INDEX_HAVE_CHANGED_5_TIMES_CANT_TODAY

	"�Ҳ�������!",
	"ʱ����",
	"�������ã��޷�ɾ����",
	"ʵ���Ѵ���!",
	"�޸ĵ�ʵ�������ڣ�",
	"ʱ����������޸ġ�",
	"��ѡ���û�",
	"δ�ҵ�ʵ����",
	"�������Ѵ�������Ϊ������Ϣ�б��ϸ��������Ϣ�б���!",
	"�Ҳ���ID",//C_LANG_INDEX_ID_NOT_FIND
	"QQ�����Ѵ���",//C_LANG_INDEX_QQ_NUM_EXIST
	"MSN�˺��Ѵ���",//C_LANG_INDEX_MSN_EXIST
	"���������˺��Ѵ���",//C_LANG_INDEX_ALI_EXIST
	"MSNΪ�ա�",
	"���������ʺ�Ϊ��!",
	"ip��ַ�������ܴ���65535!",
	"MD5�Ƿ������²���ʧ�ܣ��Բ��þɲ���!",
	"δʹ��",//C_LANG_INDEX_NOT_IN_USE
	"ʹ����",//C_LANG_INDEX_IN_USE
	"��֤�ɹ���",
	"��֤ʧ�ܣ�",
	"���ύ�Ĳ�������",
	"��������ȷ�����벻ͬ",//C_LANG_INDEX_PASSWD_NEW_PASSWD_DIFF
	"��û��Ȩ�ޣ�",//C_LANG_INDEX_YOU_HAVE_NO_LEGAL_POWER
	"���޸ĳ���5��",
	"���˺�δ����",
	"��û��Ȩ�޽��д˲�����",//C_LANG_INDEX_NO_LEGAL_POWER_FOR_OPERATION
	"��",
	"��ip�������ܴ���",
	"������ʹ�ã��޷�ɾ����",
	"����",
	"ʹ�ã��޷�ɾ����",//C_LANG_INDEX_USING_CANT_DEL
	"����",
	"���Ͳ���ͬ",//C_LANG_INDEX_TYPE_DIFF
	"�鲻���໥������",
	"����",
	"�����Ȳ��ܴ���",
	"���޷������",
	"������",//C_LANG_INDEX_NEGATION
	"��ϵͳ������ͻ",//C_LANG_INDEX_SYSTEM_GROUP_CONFLICT
	/*wusUserStats.c*/
	"�������",//C_LANG_INDEX_SW_TALK
	"P2P���",//C_LANG_INDEX_SW_P2P
	"��Ʊ���",//C_LANG_INDEX_SW_STOCK
	"������Ƶ",//C_LANG_INDEX_SW_NET_VIDEO
	"������Ϸ",//C_LANG_INDEX_SW_NET_GAME
	"������վ",//C_LANG_INDEX_SW_SHOPPING_SITE
	"�罻��վ",//C_LANG_INDEX_SW_SOCIAL_NET_SITE
	"��ҳ��Ϸ",//C_LANG_INDEX_SW_WEB_GAME
	"�ʼ�",//C_LANG_INDEX_SW_MAIL
	"��̳",//C_LANG_INDEX_SW_FORUM
	"����",//C_LANG_INDEX_SW_OTHERS
	/*wusDhcpServer.c*/
#if (MULTI_SSID_DHCP_POOL == FYES)
	"��Ч������ʼ��ַ",
	"��Ч���߽�����ַ",
	"��Ч������SSID1��ʼ��ַ",
	"��Ч������SSID1������ַ",
	"��Ч������SSID2��ʼ��ַ",
	"��Ч������SSID2������ַ",
#if (WIRELESS_5G == FYES)
	"��Ч��5G����SSID1��ʼ��ַ",
	"��Ч��5G����SSID1������ַ",
	"��Ч��5G����SSID2��ʼ��ַ",
	"��Ч��5G����SSID2������ַ",
#endif
#else
	"��Ч��ʼ��ַ",
	"��Ч������ַ",
#endif
	"��Ч��������",
	"��Ч����",
	"��ʼ��ַ��LAN�ڵ�ַ����ͬһ���磡",
	"��ʼ��ַ�����ص�ַ����ͬһ���磡",
	"��ʼ��ַ�ͽ�����ַ����ͬһ���磡",
	"������ַ����",
	"IP��ַ������Χ��",// C_LANG_INDEX_OVERFLOW_IP_ADDR
	"SSID����Ϊ��",//C_LANG_INDEX_SSID_CANT_NULL
	"�ŵ�����Ϊ��",//C_LANG_INDEX_CHANNEL_CANT_NULL
	"Radius������IP��Ч",//C_LANG_INDEX_RADIUS_INVAL_SER_IP
	/* wireless Wlan* */
	"MAC��ַ�б���Ŀ�Ѿ��ﵽ���ֵ��",//C_LANG_INDEX_MAC_ADDR_LIST_MAX
	"MAC��ַ�б������д˵�ַ",//C_LANG_INDEX_MAC_ADDR_LIST_EXIT_ADDR
	"����MAC��ַ�����б��Ѿ��ﵽ�����Ŀ��",//C_LANG_INDEX_MAC_FILTER_LIST_MAX
	"�����û����Ѿ��ﵽ���ֵ",//C_LANG_INDEX_USER_GROUP_LIST_MAX
	"���",//C_LANG_INDEX_USER
	"���ڱ���̬·������",//C_LANG_INDEX_USER_IS_USE_IN_STATIC_ROUTE
	"�����Ѵ��ڴ��û���",//C_LANG_INDEX_USER_NAME_EXIT_IN_GROUP
	"�޷�����ʵ��",//C_LANG_INDEX_CANNOT_CREATE_INST

	/**************** REOS SE V1.2 *******************/

	"������ʧ��",
	"VLAN ID����ȡֵ��Χ",
	"���������Ʋ��ܳ���11���ֽ�",
	"��Ч��������",
	"���볤�Ȳ��ܳ���11���ֽ�",
	"�˿�Ӧ��0~65536֮��",
	"��ЧRadius������IP",
	"Radius���볤�Ȳ��ܳ���31���ֽ�",
	"����������ڳ���ȡֵ��Χ",
	"����ʧ�ܣ����������Ƴ�ͻ",
	"����ʧ�ܣ�����ģ�����Ƴ�ͻ",
	"����ʧ�ܣ�VLAN ID��ͻ",
	"����ʧ��,��ַ���ѱ�ʹ��",
	"����ɾ��Ĭ��ѡ��",
	"����ϴ��У������ظ�������",
	"��Ч���ļ�",
	"����ͺŴ���������ѡ�����",
	"���У��ʹ���",
	"��Ч�����ã���ͬ�ͺ���ͬӲ���汾�����ֻ������һ��ΪĬ��",
	"��Ч��ʼ��ַ",
	"��Ч������ַ",
	"��ʼ��ַ�����ص�ַ����ͬһ���磡",
	"��ʼ��ַ�ͽ�����ַ����ͬһ���磡",
	"������ַ����",
	"����ʧ�ܣ�ϵͳ�д���ͬ���ʺ�",
	"����ɾ��ʹ���е�ѡ��",
	"�Ҳ���ɾ������",
	"�Ҳ�����Ҫ�޸�Ȩ�޵�AP",
	"�������к�Ӧ������ʼ���к�",
	"���к��ص�",
	"DHCP��ַ�ص�ַ��LAN��IP��ַ����ͬһ����!",
	"�Ѵﵽ��������ȡ�ļ���",
	"ʣ��洢�ռ䲻��",
	"ʣ���ڴ治��",
	"����ʧ��",
	"����ͼƬ̫���ļ���С����Ϊ60K��",	//C_LANG_INDEX_AP_DRAWING_FILE_LIMIT_D908W

	"���ڱ�IPSec���ã�",
	"�ѳ�����Σ���Ҫ�����豸���ܵ��롣",
	"�ѳɹ�����License��",
	"�������Ϊ�Ƿ�License,ʣ�ർ�����Ϊ4��.",
	"�������Ϊ�Ƿ�License,ʣ�ർ�����Ϊ3��.",
	"�������Ϊ�Ƿ�License,ʣ�ർ�����Ϊ2��.",
	"�������Ϊ�Ƿ�License,ʣ�ർ�����Ϊ1��.",
	"�������Ϊ�Ƿ�License,ʣ�ർ�����Ϊ0��.",
	"WAN���������ò���.",	    //C_LANG_INDEX_DYN_WAN_SETNUM_ERR
	"�˿�����δ�ı�",	    //C_LANG_INDEX_DYN_WAN_NUM_NOCHANGE
	"�����ѱ��棬�ֶ���������Ч",	//C_LANG_INDEX_DYN_WAN_SETCONF_SAVE
	"����",
	"������",
	"����",
	"����",
	"�ر�",
	"�˺Ź���",
	"û��ʣ��ʱ��",
	"ɾ����WAN�����ã����ܻᵼ�²���·��ʧЧ",  //C_LANG_INDEX_POLICY_ROUTE_ERR
	"�������ʧ��",
	"�ļ�������������С��32���ַ�����",
	"ͼƬ��С���󣨱���С��10M��",
	"�ѳɹ�����ͼƬ��",
	"ͼƬ��ʽ����",
	"����ͼƬ�����ڣ��뵼��",
	"д�ļ�ʧ��",	
	"�ļ���ʽ����",
	"�����ļ���ȡ����",
    "��ַ�ط�Χ��Ҫ�����Ѱ󶨵ľ�̬IP��ַ",       //C_LANG_INDEX_STATIC_IP_NOT_IN_DHCP_POOL
	"PPPOE�˺�������������10���������",	//C_LANG_INDEX_PPPOE_LIMIT_CANT_MORE
	"�ҳ�����MAC��С������MAC������ͬ",
	"��ȡ��֤ҳ��ʧ�ܣ������³��Ի������豸����",
	"�ϴ��ļ���ʽ����",
	"�ϴ��ļ���С����",
	"�ѳɹ��ϴ���֤ҳ��",
	"����",//C_LANG_INDEX_INTERNET_WIRED
	"����",//C_LANG_INDEX_INTERNET_WIRELESS
	"�Զ�",//C_LANG_INDEX_APBASICINFO_AUTO
	"����ʧ��",//C_LANG_INDEX_APEFFECTCONF_FAIL
	"�Ѵ������Ŀ��",//C_LANG_INDEX_APMANAGE_TIP1
	"�õ�ַ���ڵ�ַ��û�����������������ܱ��󶨣�",
	"û�иõ�ַ����Ӧ�ĵ�ַ�أ����ܱ��󶨣�",
	"�õ�ַ��û�����������������ܱ���̬�󶨣�",
	"�õ�ַ���ڵ�ַ��û�����������������ܱ��󶨣�",
	"��DHCP��������ַ�س�ͻ",
	"��PPPoE��������ַ�س�ͻ",
	"��L2TP��������ַ�س�ͻ",
	"��PPTP��������ַ�س�ͻ",
	"Ĭ�ϵ�ַ�ص��������벻�������޸ģ���Ҫ��LAN������ȥ�޸ģ�",
    "��ѡ��ɱ༭",
	"δ�ҵ���ʹ�õ�AC",//C_LANG_INDEX_FATFIT_SWITCH_NOTFINADAC
	"���ò�������",//C_LANG_INDEX_L2TP_SERVER_PARAERR
	"��̬ӳ���Ѵ�����������޷�����l2tp��Ҫ�ľ�̬ӳ��",//C_LANG_INDEX_L2TP_SERVER_STATIC_ERR
	"vlan ���ʹ���!",//C_LANG_INDEX_PORT_VLAN_TYPE_ERR
	"����ʧ�ܣ���ǰ�豸vlan���ͣ��Ƕ˿�vlan!",//C_LANG_INDEX_PORT_VLAN_FAIL_NOT_VLAN
	"�����",
	"vlan���Ʋ��ܳ���11���ַ�",//C_LANG_INDEX_TV_NAME_OUT_OF_RANGE
	"����ʧ�ܣ����������Ƴ�ͻ",//C_LANG_INDEX_TV_NAME_CONFLICT
    "�̶�IP��ַ�����ظ���",
    "�����޸�����.",
    "������IP��ַ�κ���Чʱ������Ѿ����ڣ�",
    "�����ȶ�ȡ����",//C_LANG_INDEX_RATE_UNIT_ERR
	"�Ѵﵽ�˺����Ự��",  //C_LANG_INDEX_MAX_ACC_SESSION
	"�Ѵﵽϵͳ���Ự��",  //C_LANG_INDEX_MAX_SYS_SESSION
	"����Web��֤�Ựʧ��",   //C_LANG_INDEX_APPLY_SES_ERR
	"���ܴ���ϵͳ���Ự��", //C_LANG_INDEX_INVALID_ACC_SESSION
	"�޷����ӣ��ѴﵽVPN��󲢷���", //C_LANG_INDEX_MAX_VPN_COUNT
	"�޷���ӣ��ѴﵽVPN���������Ŀ", //C_LANG_INDEX_MAX_VPN_ITEM
	"����ɾ��guest�û�",//C_LANG_INDEX_CANT_DEL_GUEST_USER
    "�����޸�guest�û�",//C_LANG_INDEX_CANT_MODIFY_GUEST_USER
    "ע�⣺�����ļ���æ�����Ժ��޸ģ�",//C_LANG_INDEX_SHARE_FILE_IS_BUSY
    "��Ч�ӿڵ�ַ",//C_LANG_INDEX_INVALID_INTERFACE_ADDR
    "��",//C_LANG_INDEX_SHORT_DAY
    "ʱ",//C_LANG_INDEX_SHORT_HOUR
    "��",//C_LANG_INDEX_SHORT_MINUTE
    "��",//C_LANG_INDEX_SHORT_SECOND
	"�ѱ����õ���ӿ��޷�ɾ����",//C_LANG_INDEX_CAN_NOT_DEL_VIF
	"���1Сʱ",//C_LANG_INDEX_AP_FLOW_LAST_HOUR
	"�޷���λAP",//C_LANG_INDEX_CAN_NOT_LOCATION_AP
	"ͼƬ��������Ϊdefault!�����µ���...",
	"��APû��5GHz��Ƶ��Ԫ",//C_LANG_INDEX_AP_WIRELESS_NOT_SUPPORT_5G
	"�ӿ����ư��ظ�", //C_LANG_INDEX_DDNS_INTERFACE_REPEAT
	"�󶨽ӿڴ���",    //C_LANG_INDEX_DDNS_INTERFACE_ERROR
        "��ȡ����MAC��ַʧ�ܣ�����������·����״̬�Ƿ�Ϊ�����ӻ��߾�̬����ip�Ƿ���д��ȷ��", //C_LANG_INDEX_GET_GW_MAC_FAIL
	"�������ڹ������֧�����õ�2038��1��18��",//C_LANG_INDEX_TIMESET_TOO_LARGE
	""
};
