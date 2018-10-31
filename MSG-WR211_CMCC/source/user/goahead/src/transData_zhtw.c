
#include "translate.h"
const char* transData_zhtw[] = {
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
	"�O���ȡ���ƣ�ipλַ�Σ�ݔ���e�`",//C_LANG_INDEX_DECIVE_ACCESS_CTL_IP_ERR
	"IP λַ�e�`!",//C_LANG_INDEX_IP_PLAN_ADDR_ERR
	"���������}��",//C_LANG_INDEX_STRATEGY_NAME_REPEATED
	"���_���Ŀ��",//C_LANG_INDEX_MAXIMUM_NUM_HAS_REACHED
	"�Ҳ���ԓ��������",//C_LANG_INDEX_CANT_FIND_STRA_NAME
	"�������ѽ����ڣ�",//C_LANG_INDEX_STRATEGY_NAME_EXISTS
	"�xȡedit_Name׃��ʧ����",//C_LANG_INDEX_READ_EDIT_NAME_VAL_ERR
	"���������ܞ��",//C_LANG_INDEX_STRATEGY_NAME_CANT_NULL
	"�Ҳ���ԓ�M����",//C_LANG_INDEX_CANT_FIND_THE_GROP_CONF
	"�����Ñ�",//C_LANG_INDEX_ALL_USERS
	"�Ҳ���ԓ��������\n",//C_LANG_INDEX_CANT_FIND_STRA_NAME_GN
	"��",
	"�r",
	"��",
	"��",
	"��",
	"�r",
	"��",
	"��",
	"���Å����e�`",//C_LANG_INDEX_CINFIG_PARAMETER_WRONG
	"�oЧλַ���^��ʼλַ",
	"�oЧ���ն�IPλַ",
	"�oЧλַ���^��ַ��",
	"�ŷ���λַ���ܰ�����λַ���^��",
	"λַ���^���ܰ�������IP",//C_LANG_INDEX_ADDR_POOL_CANT_CONTAIN_INF_IP
	"�o�Bӳ���ѽ��_�����l�����o������pptp��Ҫ���o�Bӳ��",
	"�����e�`��",
	"IPλַݔ���e�`",
	"�h�˃ȾW�W�β����c���؃ȾW�W���دB",
	"������Qݔ�����}��",
	"���_���lĿ��",
	"�Ҳ���ԓ������Q��",
	"�xȡ����ʧ��",//C_LANG_INDEX_PPTP_SERVER_READ_DATA_ERR
	"�h�˃ȾW�W·λַݔ���e�`��",
	"�h�˃ȾW�ӾW·����ݔ���e�`��",
	"�h�˃ȾW�W�β����c���؃ȾW�W���دB",
	"�oЧ�ܴa��C��ʽ",
	"������Q�Ѵ��ڣ�",//C_LANG_INDEX_PPTP_CLIENT_TUNNEL_EXITS_ERR
	"�xȡ����ʧ��",
	"�Ñ������ܳ��^11��λԪ�M��",
	"�ܴa���ܳ��^31��λԪ�M��",
	"����ʧ����ϵ�y�д���ͬ����̖��",
	"��ֹ�޸��A�Oʹ������",
	"�Ҳ���ԓ�Ñ�",
	"���܄h��Ĭ�J����T",//C_LANG_INDEX_CANT_DEL_DEFAULT_USER
	"�΄������ܞ��",
	"�΄���ݔ�����}��",
	"Ӌ���΄ռ����_����󂀔�����������µ�Ӌ���΄գ�",
	"�Ҳ���ԓ�΄�����",
	"�΄����L�Ȳ��ܳ��^11λ��",//C_LANG_INDEX_TASK_NAME_SHOULD_LESS_11
	"�oЧIP��ַ",	    //C_LANG_INDEX_INVALID_IP
	"WAN��IPλַ�������óɏV��λַ",
	"LAN��IPλַ�����cWAN��IP��ͬһ�W��",
	"�oЧ���뷽ʽ",
	"���²���ʧ�����Ԓ����f����!",
	"���²��Գɹ�!",
	"�oЧ����",
	"Ո������AP����ģʽ APClient Mode",
	"�xȡedit_Name׃��ʧ��!",	//C_LANG_INDEX_READ_EDIT_NAME_ERR
	"Դλַ·�ɵ���ʼλַ��춽Y��λַ",
	"�z�yIP������LAN�ھW��IP",
	"�z�yIP������WAN��IP",
	"LAN����ʧ��",
	"DHCP�ŷ����e�`",
	"׃�����DHCPλַ���^������LAN��λַ������ʧ��",
	"DHCPλַ���^������LAN��λַ",
#if (MULTI_SSID_DHCP_POOL == FYES)
	"׃����ğo��SSID1 DHCPλַ���^������LAN��λַ������ʧ��",
	"�o��SSID1 DHCPλַ���^������LAN��λַ",
	"׃����ğo��SSID2 DHCPλַ���^������LAN��λַ������ʧ��",
	"�o��SSID2 DHCPλַ���^������LAN��λַ",
#if (WIRELESS_5G == FYES)
	"׃����ğo��5G SSID1 DHCPλַ���^������LAN��λַ������ʧ��",
	"�o��5G SSID1 DHCPλַ���^������LAN��λַ",
	"׃����ğo��5G SSID2 DHCPλַ���^������LAN��λַ������ʧ��",
	"�o��5G SSID2 DHCPλַ���^������LAN��λַ",
#endif
#endif
	"��ӿڰ�����LAN�ڵ�ַ",
	"�Ҳ���ԓ�Ñ���",
	"���܄h���h��ϵ�y����ӳ��",
	"�Ҳ���ԓ�o�Bӳ����",
	"���܄h��ϵ�y�A�O�o�Bӳ��",
	"�Ñ������ܞ��!",
	"�o�Bӳ�������ܞ��!",
	"���Q���}",
	"�Ҳ���ԓ�lӳ��!",
	"�����½������޸�ϵ�y�A�O�o�Bӳ��",
	"�ⲿ���دB!",
	"��ͬIP�Ȳ����دB!",
	"�o�Bӳ�����ѽ�����!",
	"���܄h���A�OҎ�t",
	"�Ҳ���ԓҎ�t",
	"Ҏ�t�����}",
	"�����ⲿip���^��!",
	"�ⲿIPλַ���دB",
	"�Ȳ���ʼIPλַ����LAN����ͬһ�W��!",
	"�Ȳ��Y��IPλַ����LAN����ͬһ�W��!",
	"�ⲿIPλַ�β��ܰ���������IPλַ!",
	"�ⲿIPλַ�β��ܵ�춸�����IPλַ!",
	"�Ȳ�IPλַ�β��ܰ���������IPλַ!",
	"�Ȳ�IPλַ�β��ܵ�춸�����IPλַ!",
	"�Ҳ���·�����Q!",
	"Ŀ�ĵ�ַ�e�`",
	"λַ/�����e�`",
	"�l��λַ�e�`",
	"Ŀ�ľW·���ܰ����ȾWIPλַ",
	"��������e�`",
	"·�����Qݔ�����}!",
	"�M�����ܞ��!",
	"�M���ѽ�����!",
	"�Mip�������ܴ��",
	"ԓ���ò�����!",
	"�o���ύ��ԓ������ʹ���o������O��!",
	"�Ҳ���ԓ����",
	"�o���ύ��ֻ���SIP/MAC�����Ñ�����ԓ������ʹ�����ڽ����Ñ�����e���o������O��",
	"�o���ύ��ԓ������ʹ����䛵�MAC�c�����Ñ��б��e��MAC��һ�£��o������O��",
	"�o���ύ��ԓ������ʹ����䛵�IP�c�����Ñ��б��IP��һ�£��o������O��",
	"������",
	"�l������ʧ��",
	"�l",
	"�xȡ���Ì�����ʧ��!",
	"���_������×lĿ���o����������ã�",
	"�Ñ���������sys.�_�^",
	"ݔ�����}!",
	"�Ñ���ݔ�����}!",
	"ԓIP��ַ�����������o�����}����!",
	"ԓMAC��ַ�����������o�����}����!",
	"ԓIP�cMAC��ַ�����������o�����}����!",
	"�oЧ��ʼIPλַ",//C_LANG_INDEX_INVALID_START_IP_ADDR
	"�oЧ��DNS�ŷ���",
	"�oЧ��DNS�ŷ���",//C_LANG_INDEX_INVALID_SEC_DNS_SERVER
	"�oЧϵ�y����Ԓ��",
	"���ڷ�λַ���^�ȵĹ̶�IP����",//C_LANG_INDEX_EXIT_FIX_IP_IN_POOL
	"LAN��IPλַ���ܰ�����PPPoEλַ���^��",//C_LANG_INDEX_LAN_IP_CANT_IN_PPPoE_POOL
	"�̶�IPλַݔ���e�`",
	"�����@ȡ�e�`",
	"�̶�IP��ַ����PPPoEλַ���^�ȣ�",//C_LANG_INDEX_STATIC_IP_ADDR_CANT_IN_PPPoE_POOL
	"��ֹ�����ѷ���IP",
	"�Ñ����Ѵ��ڣ�",
	"�̶�IP��ַ�Ѵ���",//C_LANG_INDEX_STATIC_IP_ADDR_ALREADY_EXITS
	"�����Ѵ���",//C_LANG_INDEX_CASE_ALREADY_EXITS
	"���_���lĿ��",//C_LANG_INDEX_ALREADY_THE_MAX_TITLE
	"�޸ĵČ��������ڣ�",//C_LANG_INDEX_CHANGED_CASE_NOT_EXITS
	"���Ñ��M",//C_LANG_INDEX_BE_USER_GROUP
	"���ã��o���޸ģ�",//C_LANG_INDEX_USE_CANT_BE_CHANGE
	"�Ҳ����Ñ�����",
	"���ã��o���h����",
	"������",
	"����",
	"���^��",
	"�^��",//C_LANG_INDEX_EXPIRE
	"Ո�x���Ñ�",
	"δ�ҵ�������",//C_LANG_INDEX_DID_NOT_FIND_CASE
	"���Å����e�`",
	"��]���S�ə��M�д˲�����",
	"�ύ�������`��Ո����ݔ�룡",
	"�ύ�������`�����ܴa�c�_�J�ܴa��ͬ��",//C_LANG_INDEX_SUBMIT_WRONG_PARMETERS_PASSWD_DIFF
	"�����ɹ�",//C_LANG_INDEX_SUCCESSFUL_OPERATION
	"�f�ܴa��e�`��",//C_LANG_INDEX_OLD_PASSWD_CIN_ERR
	"�޸��ѳ��^5�Σ����ղ����޸ģ�",//C_LANG_INDEX_HAVE_CHANGED_5_TIMES_CANT_TODAY

	"�Ҳ����M��!",
	"�r�g�M",
	"�������ã��o���h����",
	"�����Ѵ���!",
	"�޸ĵČ��������ڣ�",
	"�r�g���������޸ġ�",
	"Ո�x���Ñ�",
	"δ�ҵ�������",
	"ԓ�M���Ѵ������О�����YӍ��λ򾫼��������YӍ�����!",
	"�Ҳ���ID",
	"QQ̖�a�Ѵ���",
	"MSN��̖�Ѵ���",
	"MSN��ա�",
	"ipλַ�������ܴ��65535!",
	"MD5�Ƿ������²���ʧ�����Ԓ����f����!",
	"δʹ��",//C_LANG_INDEX_NOT_IN_USE
	"ʹ����",//C_LANG_INDEX_IN_USE
	"�J�C�ɹ�",
	"�J�Cʧ��",
	"���ύ�ą������`��",
	"���ܴa�c�_�J�ܴa��ͬ",//C_LANG_INDEX_PASSWD_NEW_PASSWD_DIFF
	"���]���S�ə࣡",//C_LANG_INDEX_YOU_HAVE_NO_LEGAL_POWER
	"���޸ĳ��^5��",
	"�ˎ�̖δ�B��",
	"���]���S�ə��M�д˲�����",//C_LANG_INDEX_NO_LEGAL_POWER_FOR_OPERATION
	"�M",
	"��ip�������ܴ��",
	"��Ҏ�tʹ�ã��o���h����",
	"���M",
	"ʹ�ã��o���h����",//C_LANG_INDEX_USING_CANT_DEL
	"�ͽM",
	"��Ͳ���ͬ",//C_LANG_INDEX_TYPE_DIFF
	"�M�����໥������",
	"����",
	"�M����Ȳ��ܴ��",
	"���o����ӽM",
	"������",//C_LANG_INDEX_NEGATION
	"�cϵ�y�M���nͻ",//C_LANG_INDEX_SYSTEM_GROUP_CONFLICT
	/*wusUserStats.c*/
	"����ܛ�w",//C_LANG_INDEX_SW_TALK
	"P2Pܛ�w",//C_LANG_INDEX_SW_P2P
	"��Ʊܛ�w",//C_LANG_INDEX_SW_STOCK
	"�W·ҕ�l",//C_LANG_INDEX_SW_NET_VIDEO
	"�W·�[��",//C_LANG_INDEX_SW_NET_GAME
	"ُ��Wվ",//C_LANG_INDEX_SW_SHOPPING_SITE
	"�罻�Wվ",//C_LANG_INDEX_SW_SOCIAL_NET_SITE
	"�W��[��",//C_LANG_INDEX_SW_WEB_GAME
	"�]��",//C_LANG_INDEX_SW_MAIL
	"Փ��",//C_LANG_INDEX_SW_FORUM
	"����",//C_LANG_INDEX_SW_OTHERS
	/*wusDhcpServer.c*/
#if (MULTI_SSID_DHCP_POOL == FYES)
	"�oЧ�о���ʼλַ",
	"�oЧ�о��Y����ַ",
	"�oЧ�o��SSID1��ʼλַ",
	"�oЧ�o��SSID1�Y����ַ",
	"�oЧ�o��SSID2��ʼλַ",
	"�oЧ�o��SSID2�Y����ַ",
#if (WIRELESS_5G == FYES)
	"�oЧ5G�o��SSID1��ʼ��ַ",
	"�oЧ5G�o��SSID1������ַ",
	"�oЧ5G�o��SSID2��ʼ��ַ",
	"�oЧ5G�o��SSID2������ַ",
#endif
#else
	"�oЧ��ʼλַ",
	"�oЧ�Y����ַ",
#endif
	"�oЧ�ӾW·����",
	"�oЧ�l��",
	"��ʼλַ��LANλַ����ͬһ�W·��",
	"��ʼλַ���l��λַ����ͬһ�W·��",
	"��ʼλַ�ͽY��λַ����ͬһ�W·��",
	"�Y����ַ�^��",
	"IP��ַ����������",
	"SSID���ܞ��",//C_LANG_INDEX_SSID_CANT_NULL
	"ͨ�����ܞ��",//C_LANG_INDEX_CHANNEL_CANT_NULL
	"Radius�ŷ���IP�oЧ",//C_LANG_INDEX_RADIUS_INVAL_SER_IP
	/* wireless Wlan* */
	"MAC��ַ��ΗlĿ�ѽ��_�����ֵ��",
	"MAC��ַ��������д˵�ַ",
	"�o��MAC��ַ�^�V�б��ѽ��_�����lĿ��",//C_LANG_INDEX_MAC_FILTER_LIST_MAX
	"�����û����Ѿ��ﵽ���ֵ",
	"���",
	"���ڱ���̬·������",
	"�M���Ѵ��ڴ��Ñ���",
	"�޷�����ʵ��",

/**************** REOS SE V1.2 *******************/

	"������ʧ��",
	"VLAN ID����ȡֵ����",
	"���Յ^���Q���ܳ��^11���ֹ�",
	"�oЧ�ӾW�ڴa",
	"�ܴa�L�Ȳ��ܳ��^11���ֹ�",
	"�˿ڑ���0~65536֮�g",
	"�oЧRadius������IP",
	"Radius�ܴa�L�Ȳ��ܳ��^31���ֹ�",
	"�ܴa�����L�ڳ���ȡֵ����",
	"����ʧ�������Յ^���Q�nͻ",
	"����ʧ��������ģ�����Q�nͻ",
	"����ʧ����VLAN ID�nͻ",
	"����ʧ������ַ���ѱ�ʹ��",
	"���܄h��Ĭ�J�x�",
	"ܛ���ς��У� Ո�����}������",
	"�oЧ���ļ�",
	"ܛ����̖�e�`��Ո�����x��ܛ��",
	"ܛ��У���e�`",
	"�oЧ���O�ã���ͬ��̖��ͬӲ���汾��ܛ��ֻ���O��һ����Ĭ�J",
	"�oЧ��ʼ��ַ",
	"�oЧ�Y����ַ",
	"��ʼ��ַ�;W�v��ַ����ͬһ�W�j��",
	"��ʼ��ַ�ͽY����ַ����ͬһ�W�j��",
	"�Y����ַ�^��",
	"����ʧ����ϵ�y�д���ͬ����̖",
	"���܄h��ʹ���е��x�",
	"�Ҳ����h���ĽM",
	"�Ҳ�����Ҫ�޸ę��޵�AP",
	"�Y������̖�������ʼ����̖",
	"����̖�دB",
	"DHCP��ַ�ص�ַ��LAN��IP��ַ����ͬһ�W�j!",

	"���_��������S�@ȡ�ļ���",
	"ʣ�N�惦���g����",
	"ʣ�N�ȴ治��",
	"����ʧ��",
	"����ͼƬ̫���ļ���С����Ϊ60K��",	//C_LANG_INDEX_AP_DRAWING_FILE_LIMIT_D908W
	"�_��",
	"�P�]",
	"��̖�^��",
	"�]��ʣ�N�r�g",
	"ɾ����WAN�����ã����ܻᵼ�²���·��ʧЧ",
	"�������ʧ��",
	"ͼƬ������������С��32���ַ���",
	"ͼƬ��С���󣨱���С��32K��",
	"�ѳɹ�����ͼƬ",
	"ͼƬ��ʽ����",
	"����ͼƬ�����ڣ��뵼��",
	"д�ļ�ʧ��",
	"�����ļ���ʽ����",
	"�����ļ���ȡ����",

    "��ַ�ط�Χ��Ҫ�����Ѱ󶨵ľ�̬IP��ַ",       //C_LANG_INDEX_STATIC_IP_NOT_IN_DHCP_POOL
	"PPPOE�˺�������������10���������"	//C_LANG_INDEX_PPPOE_LIMIT_CANT_MORE
	"�ҳ�����MAC��С������MAC������ͬ",
	"��ȡ��֤ҳ��ʧ�ܣ������³��Ի������豸����",
	"�ϴ��ļ���ʽ����",
	"�ϴ��ļ���С����",
	"�ѳɹ��ϴ���֤ҳ��",
	"�����",
	"tw_vlan���Ʋ��ܳ���11���ַ�",//C_LANG_INDEX_TV_NAME_OUT_OF_RANGE
	"tw_����ʧ����vlan���Q�nͻ",//C_LANG_INDEX_TV_NAME_CONFLICT
    "tw_�̶�IP��ַ�����ظ���",
    "tw_�����޸�����.",
    "tw_������IP��ַ�κ���Чʱ������Ѿ����ڣ�",
    "tw_�����ȶ�ȡ����",//C_LANG_INDEX_RATE_UNIT_ERR
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
    "�r",//C_LANG_INDEX_SHORT_HOUR
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


