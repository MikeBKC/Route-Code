
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "ifmisc.h"

#if (DHCP_POOL== FYES)
#if (FEATURE_AC == FYES)
/*******************
 *
 *1.6版本软件升级到1.9.2版本时，DHCP地址池配置转换为vlan子接口和DHCP地址池
 *
 ******************/
void updateInitDhcp()
{
	int n = 0;
	int min, max, i = 0;
	int min1, max1, i1 = 0;
	int min2, max2, i2 = 0;
    DhcpPoolProfile *profDhcp = NULL;
    VlanConfigProfile *profVlan = NULL;
	ServiceZoneProfile *profSz = NULL;
    MibProfileType mibTypeDhcp = MIB_PROF_DHCP_POOL;
    MibProfileType mibTypeVlan = MIB_PROF_VLAN_CONFIG;
	MibProfileType mibTypeSz = MIB_PROF_SERVICE_ZONE;
//	struProfAlloc *profhead = NULL;
	int flag = 0;
	char interface[10];

	n = strlen(getLanIfName()) + 1;

	ProfInstLowHigh(mibTypeDhcp, &max, &min);
	ProfInstLowHigh(mibTypeVlan, &max1, &min1);
	ProfInstLowHigh(mibTypeSz, &max2, &min2);

	profDhcp = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibTypeDhcp, 0);
	if ((NULL != profDhcp) && (0 == ProfInstIsFree((Pointer)profDhcp)) && (TRUE == profDhcp->head.active))
	{
	    strcpy(profDhcp->interface,"LAN");
		flag = 1;
	}
	for (i = min + 1; i < max; i++) {
		profDhcp = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibTypeDhcp, i);
			if ((NULL != profDhcp) && (0 == ProfInstIsFree((Pointer)profDhcp)) && (TRUE == profDhcp->head.active)) {
				if (0 == profDhcp->vid) {
					/* 被服务区引用了 */
					if (0 == strncmp(getLanIfName(), profDhcp->interface, n - 1)) {
						profDhcp->vid = (Uint16)strtoul(profDhcp->interface + n, NULL, 10);
						memset(interface,'\0',sizeof(interface));
						sprintf(interface,"VIF%d",profDhcp->vid);
                        strcpy(profDhcp->interface,interface);
						flag = 1;
						for (i1 = min1; i1 < max1; i1++) {
							profSz = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibTypeSz, i1);
							if ((NULL != profSz) && (0 == ProfInstIsFree((Pointer)profSz)) && (TRUE == profSz->head.active)) {
								if (profDhcp->vid == profSz->vid) {
									i2 = ProfNewInst(mibTypeVlan, interface, FALSE);
									profVlan = (VlanConfigProfile *)ProfGetInstPointByIndex(mibTypeVlan, i2);
									profVlan->vlanEn = PROF_ENABLE;
									profVlan->vid = profSz->vid;
									profVlan->ip = profSz->virif;
									profVlan->netmask = profSz->netmask;

									break;
								}
							}
						}
					} else {
//						ProfBackupByIndex(mibTypeDhcp, PROF_CHANGE_DEL, i, &profhead);
						ProfDelInstByIndex(mibTypeDhcp, i);
						flag = 1;
					}
				}
		}
	}

	if (flag == 1) {
//		ProfUpdate(profhead);
//		ProfFreeAllocList(profhead);
		nvramWriteCommit();
	}

	return;
}
#endif
void DhcpPoolInit()
{
    FILE *fp = NULL;
	char tmpFile[UTT_INTERFACE_LEN + 20];
    int min = 0, max = 0, i;
    int dhcp_has_initd = 0;
    UserProfile *prof;
    DhcpPoolProfile *profDhcp = NULL;
    struct in_addr tmpAd;
    MibProfileType mibType = MIB_PROF_USER;
    MibProfileType mibTypeDhcp = MIB_PROF_DHCP_POOL;

	doSystem("rm /etc/udhcpd.conf;killall udhcpd &");
	ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++) {
		prof = (UserProfile *)ProfGetInstPointByIndex(mibType, i);
	    if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
			if (NULL == fp) {
			    sprintf(tmpFile, "/etc/udhcpd.%s.tmp", prof->dhcpPoolName);
			    fp = fopen(tmpFile, "w");
			}

			if (NULL != fp) {
				fseek(fp, 0, SEEK_END);

				tmpAd.s_addr = prof->ip;
				fprintf(fp, "static_lease %s %s\n", converMac6To17Byte(prof->mac), inet_ntoa(tmpAd));
			}
		}
	}
	if (NULL != fp) {
		fclose(fp);
	}

	ProfInstLowHigh(mibTypeDhcp, &max, &min);
	for (i = min; i < max; i++) {
		profDhcp = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibTypeDhcp, i);
		if (NULL != profDhcp) {
			if (PROF_ENABLE == profDhcp->dhcpEn) {
//				doSystem("mdhcp-init.sh add %s %s", profDhcp->interface, profDhcp->head.name);
				/*1.6版本profDhcp->interface成员可能带虚接口，1.9版本不需要虚接口，故改为geyLanIfName()*/
                dhcp_has_initd = 1;
				doSystem("mdhcp-init.sh add %s %s", getLanIfName(), profDhcp->head.name);
			}
		}
	}
	
    if(!dhcp_has_initd){
        doSystem("config-mdhcp-init.sh -r %s;config-mdhcp-init.sh -i %s %s;config-mdhcp-init.sh -p;udhcpd /etc/udhcpd.conf &",
                getLanIfName(),getLanIfName(),getLanIfName());
    }
    else {
	doSystem("udhcpd /etc/udhcpd.conf&");
    }

	return;
}

#if (DHCP_SERVER_OPTION_43 == FYES)
#define OPTIONP43_FILE_PATH_LENTH 30
void dhcpPoolOption43Init(const DhcpPoolProfile *prof)
{
    if(prof->option43Type == DHCP_OPTION_43_TYPE_DISABLE)
        return 0;

    int i;
    FILE *fp;
    struct in_addr addr;
    char file_path[OPTIONP43_FILE_PATH_LENTH];
    char l_in_tlv[3];
    
    memset(file_path, 0, OPTIONP43_FILE_PATH_LENTH); 
    sprintf(file_path, "/tmp/udhcpdoption43_vid%d", prof->vid);
    
    if((fp = fopen(file_path, "w")) == NULL)
    {
        printf("cannot open file %s\n", file_path);
        return ;
    }

    if (prof->option43Type == DHCP_OPTION_43_TYPE_HEX)
    {
        inet_aton(prof->option43Addr, &addr);
#if defined(CONFIG_BOARD_P1010_C)
        fprintf(fp,"%02x %02x %02x %02x %02x %02x %02x %02x", 0x2b, 0x06, 0x01, 0x04, (addr.s_addr>>24)&0xff, (addr.s_addr>>16)&0xff, (addr.s_addr>>8)&0xff, (addr.s_addr)&0xff);
#else
        fprintf(fp,"%02x%02x%02x%02x%02x%02x%02x%02x", 0x2b, 0x06, 0x01, 0x04, (addr.s_addr)&0xff, (addr.s_addr>>8)&0xff, (addr.s_addr>>16)&0xff, (addr.s_addr>>24)&0xff);
#endif
    }
    else if (prof->option43Type == DHCP_OPTION_43_TYPE_ASCII)
    {
#if defined(CONFIG_BOARD_P1010_C)
        fprintf(fp, "%02x %02x %02x %02x", 0x2b, strlen(prof->option43Addr)+2, 0x03, strlen(prof->option43Addr)); 
#else
        fprintf(fp, "%02x%02x%02x%02x", 0x2b, strlen(prof->option43Addr)+2, 0x03, strlen(prof->option43Addr)); 
#endif
        for (i=0; i<strlen(prof->option43Addr); i++)
#if defined(CONFIG_BOARD_P1010_C)
            fprintf(fp, " %02x", prof->option43Addr[i]);
#else
            fprintf(fp, "%02x", prof->option43Addr[i]);
#endif
    }
    else if (prof->option43Type == DHCP_OPTION_43_TYPE_COSTUM)
    {
#if 0
        if ((prof->option43Addr[2]-'0')*16+(prof->option43Addr[3]-'0') == (strlen(prof->option43Addr)+strlen(prof->option43Addr2))/2-2)
            fprintf(fp, "%s%s", prof->option43Addr, prof->option43Addr2);
        else
            printf("%s %d: costum option 43 error, length %d not compare %d\n", __func__, __LINE__, 
                    (prof->option43Addr[2]-'0')*16+(prof->option43Addr[3]-'0'), (strlen(prof->option43Addr)+strlen(prof->option43Addr2))/2-2);
#endif
#if defined(CONFIG_BOARD_P1010_C)
        if (strlen(prof->option43Addr) % 2 == 0 && strlen(prof->option43Addr) >= 6) {
            fprintf(fp, "%c%c", prof->option43Addr[0], prof->option43Addr[1]);
            for (i=2; i<strlen(prof->option43Addr); i+=2)
            {
                fprintf(fp, " %c%c", prof->option43Addr[i], prof->option43Addr[i+1]);
            }
            for (i=0; i<strlen(prof->option43Addr2); i+=2)
            {
                fprintf(fp, " %c%c", prof->option43Addr2[i], prof->option43Addr2[i+1]);
            }
        }
#else
        //if ((prof->option43Addr[2]-'0')*16+(prof->option43Addr[3]-'0') == (strlen(prof->option43Addr)+strlen(prof->option43Addr2))/2-2)
        memset(l_in_tlv, 0, 3);
        l_in_tlv[0] = prof->option43Addr[2];
        l_in_tlv[1] = prof->option43Addr[3];
        if((strtol(l_in_tlv, NULL, 16) == strlen(prof->option43Addr)+strlen(prof->option43Addr2))/2-2)
            fprintf(fp, "%s%s", prof->option43Addr, prof->option43Addr2);
        else
            printf("%s %d: costum option 43 error, length %d not compare %d\n", __func__, __LINE__, 
                    (prof->option43Addr[2]-'0')*16+(prof->option43Addr[3]-'0'), (strlen(prof->option43Addr)+strlen(prof->option43Addr2))/2-2);
#endif
    }
    fclose(fp);
    return;
}

void dhcpServerOption43Del(const DhcpPoolProfile *prof)
{
    doSystem("rm /tmp/udhcpdoption43_vid%d", prof->vid);
}

void dhcpServerOption43DelAll()
{
    int min, max, index;
    DhcpPoolProfile *prof = NULL;
    ProfInstLowHigh(MIB_PROF_DHCP_POOL, &max, &min);
    for(index = min + 1; index < max; index++)//跳过第一个
    {
        prof = (DhcpPoolProfile *)ProfGetInstPointByIndex(MIB_PROF_DHCP_POOL, index);
        if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)) && (TRUE == prof->head.active)) {
            dhcpServerOption43Del(prof);
        }
    }
}
#endif


void DhcpPoolChange(const DhcpPoolProfile* oldProf, const DhcpPoolProfile* newProf)
{
    FILE *fp = NULL;
	char tmpFile[UTT_INTERFACE_LEN + 20];
    int min = 0, max = 0, i;
    UserProfile *prof;
    struct in_addr tmpAd;
    MibProfileType mibType = MIB_PROF_USER;

	ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++) {
		prof = (UserProfile *)ProfGetInstPointByIndex(mibType, i);
	    if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
			sprintf(tmpFile, "/etc/udhcpd.%s.tmp", prof->dhcpPoolName);
			fp = fopen(tmpFile, "a+");
			if (NULL != fp) {
				fseek(fp, 0, SEEK_END);

				tmpAd.s_addr = prof->ip;
				fprintf(fp, "static_lease %s %s\n", converMac6To17Byte(prof->mac), inet_ntoa(tmpAd));
			}
			fclose(fp);
		}
	}

	if ((oldProf->dhcpEn == PROF_ENABLE) && (newProf->dhcpEn == PROF_ENABLE)){
//		doSystem("mdhcp.sh del %s", oldProf->interface);
		doSystem("echo -n >/etc/udhcpd_vlan.conf");
		doSystem("echo 'del_vlan %d'>>/etc/udhcpd_vlan.conf", oldProf->vid);
		doSystem("mdhcp.sh add %s %s", newProf->interface, newProf->head.name);
		doSystem("killall -q -USR2 udhcpd");
	} else if ((oldProf->dhcpEn == PROF_ENABLE) && (newProf->dhcpEn == PROF_DISABLE)) {
		doSystem("echo 'del_vlan %d'>/etc/udhcpd_vlan.conf", newProf->vid);
//		doSystem("mdhcp.sh del %s", oldProf->interface);
		doSystem("killall -q -USR2 udhcpd");
	} else if ((oldProf->dhcpEn == PROF_DISABLE) && (newProf->dhcpEn == PROF_ENABLE)) {
		doSystem("echo -n >/etc/udhcpd_vlan.conf");
		doSystem("mdhcp.sh add %s %s", newProf->interface, newProf->head.name);
		doSystem("killall -q -USR2 udhcpd");
	} else if ((oldProf->dhcpEn == PROF_DISABLE) && (newProf->dhcpEn == PROF_DISABLE)) {
	}

	doSystem("rm -rf /etc/udhcpd.*.tmp");

	return;
}

void DhcpPoolDel(const DhcpPoolProfile *oldProf)
{
	if (oldProf->dhcpEn == PROF_ENABLE) {
		doSystem("echo 'del_vlan %d'>/etc/udhcpd_vlan.conf", oldProf->vid);
//		doSystem("mdhcp.sh del %s", oldProf->interface);
		doSystem("killall -q -USR2 udhcpd");
	}
}

void DhcpPoolProfileChange(Pointer tag, MibProfileType profileType,
        ProfChangeType changeType, Uint32 index,
        const DhcpPoolProfile* oldProfileDataPtr, const DhcpPoolProfile* newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
#if (DHCP_SERVER_OPTION_43 == FYES)
            dhcpPoolOption43Init(newProfileDataPtr);
#endif
            break;
        case PROF_CHANGE_EDIT:
#if (DHCP_SERVER_OPTION_43 == FYES)
            dhcpServerOption43Del(oldProfileDataPtr);
            dhcpPoolOption43Init(newProfileDataPtr);
#endif
            DhcpPoolChange(oldProfileDataPtr, newProfileDataPtr);
            break;
        case PROF_CHANGE_DEL:
#if (DHCP_SERVER_OPTION_43 == FYES)
            dhcpServerOption43Del(oldProfileDataPtr);
#endif
            DhcpPoolDel(oldProfileDataPtr);
            break;
        case PROF_CHANGE_DELALL:
            DhcpPoolDel(oldProfileDataPtr);
#if (DHCP_SERVER_OPTION_43 == FYES)
            dhcpServerOption43DelAll(oldProfileDataPtr);
#endif
            break;
        default:
            break;
    };
}

void dhcpBindAdd(const UserProfile *newProf)
{
//	printf("debug: %d in %s()\n", __LINE__, __func__);
    FILE *fp = NULL;
	char tmpFile[UTT_INTERFACE_LEN + 20];
    int min = 0, max = 0, i;
    UserProfile *prof;
    DhcpPoolProfile *profDhcp = NULL;
    struct in_addr tmpAd;
    MibProfileType mibType = MIB_PROF_USER;
    MibProfileType mibTypeDhcp = MIB_PROF_DHCP_POOL;

	profDhcp = (DhcpPoolProfile *)ProfGetInstPointByName(mibTypeDhcp, newProf->dhcpPoolName);
	if (NULL != profDhcp) {
		if (PROF_ENABLE == profDhcp->dhcpEn) {
			ProfInstLowHigh(mibType, &max, &min);
			for (i = min; i < max; i++) {
				prof = (UserProfile *)ProfGetInstPointByIndex(mibType, i);
			    if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
//					printf("User->head.name = %s, dhcpPoolName = %s\n", prof->head.name, prof->dhcpPoolName);
					sprintf(tmpFile, "/etc/udhcpd.%s.tmp", prof->dhcpPoolName);
					fp = fopen(tmpFile, "a+");
					if (NULL != fp) {
						fseek(fp, 0, SEEK_END);

						tmpAd.s_addr = prof->ip;
						fprintf(fp, "static_lease %s %s\n", converMac6To17Byte(prof->mac), inet_ntoa(tmpAd));
					}
					fclose(fp);
				}
			}

//			doSystem("echo 'del_vlan %d'>/etc/udhcpd_vlan.conf", profDhcp->vid);
//			doSystem("mdhcp.sh del %s", oldProf->interface);
//			doSystem("killall -q -USR2 udhcpd");
			doSystem("echo -n >/etc/udhcpd_vlan.conf");
			doSystem("mdhcp.sh add %s %s", profDhcp->interface, profDhcp->head.name);
		    doSystem("killall -q -USR2 udhcpd");
			doSystem("rm -rf /etc/udhcpd.*.tmp");
		}
	}

	return;
}

void dhcpBindEdit(const UserProfile *oldProf, const UserProfile *newProf)
{
    FILE *fp = NULL;
	char tmpFile[UTT_INTERFACE_LEN + 20];
    int min = 0, max = 0, i;
    UserProfile *prof;
    DhcpPoolProfile *profDhcp = NULL;
    struct in_addr tmpAd;
    MibProfileType mibType = MIB_PROF_USER;
    MibProfileType mibTypeDhcp = MIB_PROF_DHCP_POOL;

	ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++) {
		prof = (UserProfile *)ProfGetInstPointByIndex(mibType, i);
	    if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
//			printf("User->head.name = %s, dhcpPoolName = %s\n", prof->head.name, prof->dhcpPoolName);
			sprintf(tmpFile, "/etc/udhcpd.%s.tmp", prof->dhcpPoolName);
			fp = fopen(tmpFile, "a+");
			if (NULL != fp) {
				fseek(fp, 0, SEEK_END);

				tmpAd.s_addr = prof->ip;
				fprintf(fp, "static_lease %s %s\n", converMac6To17Byte(prof->mac), inet_ntoa(tmpAd));
			}
			fclose(fp);
		}
	}
	
	doSystem("echo -n >/etc/udhcpd_vlan.conf");
//	printf("oldProf->dhcpPoolName = %s, newProf->dhcpPoolName = %s\n", oldProf->dhcpPoolName, newProf->dhcpPoolName);
	if (strcmp(oldProf->dhcpPoolName, newProf->dhcpPoolName) != 0) {
		profDhcp = (DhcpPoolProfile *)ProfGetInstPointByName(mibTypeDhcp, oldProf->dhcpPoolName);
		if (PROF_ENABLE == profDhcp->dhcpEn) {
	//		doSystem("mdhcp.sh del %s", profDhcp->interface);
	//		doSystem("mdhcp.sh add %s %s", profDhcp->interface, profDhcp->head.name);
	//		doSystem("echo 'del_vlan %d'>/etc/udhcpd_vlan.conf", profDhcp->vid);
	//		doSystem("killall -q -USR2 udhcpd");
			doSystem("mdhcp.sh add %s %s", profDhcp->interface, profDhcp->head.name);
	//	    doSystem("killall -q -USR2 udhcpd");
		}
	}
	profDhcp = (DhcpPoolProfile *)ProfGetInstPointByName(mibTypeDhcp, newProf->dhcpPoolName);
	if (PROF_ENABLE == profDhcp->dhcpEn) {
//		doSystem("mdhcp.sh del %s", profDhcp->interface);
//		doSystem("mdhcp.sh add %s %s", profDhcp->interface, profDhcp->head.name);
//		doSystem("echo 'del_vlan %d'>/etc/udhcpd_vlan.conf", profDhcp->vid);
//		doSystem("killall -q -USR2 udhcpd");
		doSystem("mdhcp.sh add %s %s", profDhcp->interface, profDhcp->head.name);
	}
	    doSystem("killall -q -USR2 udhcpd");

	doSystem("rm -rf /etc/udhcpd.*.tmp");

	return;
}

void dhcpBindDel(const UserProfile *oldProf)
{
    FILE *fp = NULL;
	char tmpFile[UTT_INTERFACE_LEN + 20];
    int min = 0, max = 0, i;
    UserProfile *prof;
    DhcpPoolProfile *profDhcp = NULL;
    struct in_addr tmpAd;
    MibProfileType mibType = MIB_PROF_USER;
    MibProfileType mibTypeDhcp = MIB_PROF_DHCP_POOL;

	profDhcp = (DhcpPoolProfile *)ProfGetInstPointByName(mibTypeDhcp, oldProf->dhcpPoolName);
	if (NULL != profDhcp) {
		if (PROF_ENABLE == profDhcp->dhcpEn) {
			ProfInstLowHigh(mibType, &max, &min);
			for (i = min; i < max; i++) {
				prof = (UserProfile *)ProfGetInstPointByIndex(mibType, i);
			    if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
//					printf("User->head.name = %s, dhcpPoolName = %s\n", prof->head.name, prof->dhcpPoolName);
					sprintf(tmpFile, "/etc/udhcpd.%s.tmp", prof->dhcpPoolName);
					fp = fopen(tmpFile, "w+");
					if (NULL != fp) {
						fseek(fp, 0, SEEK_END);

						tmpAd.s_addr = prof->ip;
						fprintf(fp, "static_lease %s %s\n", converMac6To17Byte(prof->mac), inet_ntoa(tmpAd));
					}
					fclose(fp);
				}
			}

//			doSystem("mdhcp.sh del %s", profDhcp->interface);
//			doSystem("mdhcp.sh add %s %s", profDhcp->interface, profDhcp->head.name);
//		    doSystem("echo 'del_vlan %d'>/etc/udhcpd_vlan.conf", profDhcp->vid);
//		    doSystem("killall -q -USR2 udhcpd");
			doSystem("echo -n >/etc/udhcpd_vlan.conf");
		    doSystem("mdhcp.sh add %s %s", profDhcp->interface, profDhcp->head.name);
		    doSystem("killall -q -USR2 udhcpd");
			doSystem("rm -rf /etc/udhcpd.*.tmp");
		}
	}

	return;
}

void DhcpBindProfileChange(Pointer tag, MibProfileType profileType,
        ProfChangeType changeType, Uint32 index,
        const UserProfile* oldProfileDataPtr, const UserProfile* newProfileDataPtr )
{
    switch(changeType) {
		case PROF_CHANGE_ADD:
			dhcpBindAdd(newProfileDataPtr);
			break;
        case PROF_CHANGE_EDIT:
			dhcpBindEdit(oldProfileDataPtr, newProfileDataPtr);
			break;
		case PROF_CHANGE_DEL:
		case PROF_CHANGE_DELALL:
			dhcpBindDel(oldProfileDataPtr);
			break;
        default:
            break;
    };
}

void funInitDhcpPool()
{
	registerForProfileUpdates(MIB_PROF_USER, NULL, DhcpBindProfileChange);
	registerForProfileUpdates(MIB_PROF_DHCP_POOL, NULL, DhcpPoolProfileChange);
#if (FEATURE_AC == FYES)
	updateInitDhcp();
#endif

#if (DHCP_SERVER_OPTION_43 == FYES)
    DhcpPoolProfile *prof = NULL;
    int max, min, index;
    ProfInstLowHigh(MIB_PROF_DHCP_POOL, &max, &min);
    for(index = min; index < max; index++)
    {
        prof = (DhcpPoolProfile *)ProfGetInstPointByIndex(MIB_PROF_DHCP_POOL, index);
        if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)) && (TRUE == prof->head.active)) {
            dhcpPoolOption43Init(prof);
        }
    }	
#endif

	DhcpPoolInit();
}

#endif
