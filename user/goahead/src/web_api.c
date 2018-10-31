
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <uttMachine.h>
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "uttfunction.h"
#include "mib.h"
#include "profacce.h"

#if (FEATURE_AC == FYES)
#include    <urcp_ipc.h>
#include    <switch.h>
#include    <sw_cfg.h>
#include    "mibUrcp.h"
#include    "webmsg.h"


static int get_ap_wlhost(webs_t wp,  char_t *path, char_t *query);
static int get_ap_historyhost(webs_t wp, char_t *path, char_t *query);
typedef void (*func) (webs_t wp,  char_t *path, char_t *query);

typedef struct _api_func{
 int id;
 char magic_name[20];
 func api_func;
}api_func_t;

typedef enum {
	WLHOST = 0,
	HISTORYHOST,
	TB_MAX,
}api_id_t;

api_func_t def_api_tbl[]={
	{
		WLHOST,
		"wlhost",
		get_ap_wlhost
	},
	{	
	        HISTORYHOST,
		"historyhost",
		get_ap_historyhost
		
	}
};

static int get_ap_historyhost(webs_t wp, char_t *path, char_t *query)
{   
    char buf[560],tmp_str[64],tmp_date[64];
    urcp_shm_t *mem_shadow = NULL;
    uint32 i,j=0u;
    memset(buf,'\0',sizeof(buf));
    mem_shadow = get_shm_ptr();
    if(mem_shadow!=NULL)
    {
                for(i = 0; i < 7; i++){
		    memset(tmp_date, '\0',64);
		    sprintf(tmp_date, "%u-%u-%u: ", mem_shadow->wl_statis.wl_histroy[i].year, mem_shadow->wl_statis.wl_histroy[i].mon, mem_shadow->wl_statis.wl_histroy[i].day);
		  //  printf("%d,year:%u\n",__LINE__,mem_shadow->wl_statis.wl_histroy[i].year);
		  //  printf("%d,month:%u\n",__LINE__,mem_shadow->wl_statis.wl_histroy[i].mon);
		  //  printf("%d,day:%u\n",__LINE__,mem_shadow->wl_statis.wl_histroy[i].day);
		  //  printf("%d,tmp_date:%s\n",__LINE__,tmp_date);
		    strcat(buf,tmp_date);
		    for(j = 0; j < 24 ; j++){
			memset(tmp_str, '\0', 64);
		//	printf("%d,%lu\n",__LINE__,mem_shadow->wl_statis.wl_histroy[i].wl_data[j].client_num);
			sprintf(tmp_str,"%lu",mem_shadow->wl_statis.wl_histroy[i].wl_data[j].client_num);
			if(j < 23)
			strcat(tmp_str,",");
			else strcat(tmp_str," ");
		//	printf("%d,tmp_str:%s\n",__LINE__,tmp_str);
			strcat(buf,tmp_str);
		
		    }
	}
//	printf("%d,%s\n",__LINE__,buf);

	websWrite(wp, "%s\n", buf);
	websDone(wp, 200);
    }
}


/*static int get_ap_wlhost(webs_t wp, char_t *path, char_t *query)
{
	char ip[16],buf[560],tmp_str[64],url[128];
	urcp_shm_t *mem_shadow = NULL;
	urcp_dev_t *tmp = NULL;
	uint32 k, i = 0u,tb_index,val;
	char_t *ipaddr = NULL;

	ipaddr = websGetVar(wp, T("AP"), T(""));
	printf("%d,ipaddr=%s,\n",__LINE__,ipaddr);

	memset(buf,'\0',sizeof(buf));

	mem_shadow = get_shm_ptr();
	if(mem_shadow!=NULL)
    {
	sprintf(buf,"AP=%s\r\nWlhost=",ipaddr);
	for(i = 0u; i < DEV_HASH_SIZE; i++)
	{
	    tb_index = mem_shadow->dev_tb_hash[i];

	    while(tb_index!=NONE_NODE)
	    {
		tmp = &(mem_shadow->dev_table[tb_index-1]);
		printf("%d,ip=%s\n",__LINE__,int2ip(tmp->ip,ip));

		val = tmp->dev_type;
		printf("%d,%d\n",__LINE__,tmp->dev_type);
		printf("%d,%x\n",__LINE__,tmp->dev_type);
		if(((val&DEV_TYPE_FIT_AP) || (val&DEV_TYPE_FAT_AP) || (val&DEV_TYPE_WIRELESS))&&(tmp->aged != 2)&&(strcmp(ipaddr,int2ip(tmp->ip,ip)) == 0))
		{
		printf("%d,ip=%s\n",__LINE__,ip);
		printf("%d,%d\n",__LINE__,tmp->wl_info.clients);
			for(k = 0; k < tmp->wl_info.clients; k++) {
				memset(tmp_str, '\0', 64);
				sprintf(tmp_str, "%02X%02X%02X%02X%02X%02X", tmp->wl_info.sta_info[k].sta_mac[0],tmp->wl_info.sta_info[k].sta_mac[1],
						tmp->wl_info.sta_info[k].sta_mac[2],tmp->wl_info.sta_info[k].sta_mac[3],
						tmp->wl_info.sta_info[k].sta_mac[4],tmp->wl_info.sta_info[k].sta_mac[5]);
				printf("%d,%s\n",__LINE__,tmp->wl_info.sta_info[k].sta_mac[0]);
				printf("%d,%x\n",__LINE__,tmp->wl_info.sta_info[k].sta_mac[1]);
				printf("%d,%s\n",__LINE__,tmp_str);
				if(k < tmp->wl_info.clients - 1)
				{
					strcat(tmp_str, ",");
				}
				strcat(buf, tmp_str);
		}
			break;
		}
		tb_index = tmp->tb_next;
		}
	}
	}

	websWrite(wp, "%s\n", buf);
//	websRedirect(wp,"api/wlhost");
//		sprintf(url,"http://%s/%s",websGetVar(wp, T("HTTP_HOST"), 	websHostUrl), "api/wlhost");
//	websResponse(wp, 302, buf, url);
	websDone(wp, 200);

}*/

static int get_ap_wlhost(webs_t wp, char_t *path, char_t *query)
{
	char ip[16],buf[560],tmp_str[64],url[128];
	urcp_shm_t *mem_shadow = NULL;
	urcp_dev_t *tmp = NULL;
	uint32 k,t,count = 0u, i = 0u,tb_index,val;
	char_t *ipaddr = NULL;

	ipaddr = websGetVar(wp, T("AP"), T(""));
//	printf("%d,ipaddr=%s,\n",__LINE__,ipaddr);

	memset(buf,'\0',sizeof(buf));

	mem_shadow = get_shm_ptr();
	if(mem_shadow!=NULL)
    {
	sprintf(buf,"AP=%s\r\nWlhost=",ipaddr);
	for(i = 0u; i < DEV_HASH_SIZE; i++)
	{
	    tb_index = mem_shadow->dev_tb_hash[i];

	    while(tb_index!=NONE_NODE)
	    {
		tmp = &(mem_shadow->dev_table[tb_index-1]);
//		printf("%d,ip=%s\n",__LINE__,int2ip(tmp->ip,ip));

		val = tmp->dev_type;
		if(((val&DEV_TYPE_FIT_AP) || (val&DEV_TYPE_FAT_AP) || (val&DEV_TYPE_WIRELESS))&&(tmp->aged != 2)&&(strcmp(ipaddr,int2ip(tmp->ip,ip)) == 0))
		{
//		printf("%d\n",__LINE__);
//		printf("%d,ip=%s\n",__LINE__,ip);
//		printf("%d,%d\n",__LINE__,tmp->wl_info.joined_sz_num);
			for(k = 0; k < tmp->wl_info.joined_sz_num; k++) {
			    count = count + tmp->wl_info.ssid_info[k].sta_num; 
//			    printf("%d,%d\n",__LINE__,tmp->wl_info.ssid_info[k].sta_num);
			    for(t = 0 ; t < tmp->wl_info.ssid_info[k].sta_num;t++){
				memset(tmp_str, '\0', 64);
				sprintf(tmp_str, "%02X%02X%02X%02X%02X%02X", tmp->wl_info.ssid_info[k].sta_info[t].sta_mac[0],tmp->wl_info.ssid_info[k].sta_info[t].sta_mac[1],
						tmp->wl_info.ssid_info[k].sta_info[t].sta_mac[2],tmp->wl_info.ssid_info[k].sta_info[t].sta_mac[3],
						tmp->wl_info.ssid_info[k].sta_info[t].sta_mac[4],tmp->wl_info.ssid_info[k].sta_info[t].sta_mac[5]);
//				printf("%d,%s\n",__LINE__,tmp_str);
				if(t < tmp->wl_info.ssid_info[k].sta_num - 1)
				{
					strcat(tmp_str, ",");
//					printf("%d\n",__LINE__);
				}
				strcat(buf,tmp_str);
		    }
			    if((count!=0)&&(k < tmp->wl_info.joined_sz_num - 1)&&(tmp->wl_info.ssid_info[k+1].sta_num!=0))
			    {	
				    strcat(buf,",");
//				    printf("%d\n",__LINE__);
			    }

		}
			break;
		}
		tb_index = tmp->tb_next;
		}
	}
	}
//	printf("%s\n",buf);
	websWrite(wp, "%s\n", buf);
//	websRedirect(wp,"api/wlhost");
//		sprintf(url,"http://%s/%s",websGetVar(wp, T("HTTP_HOST"), 	websHostUrl), "api/wlhost");
//	websResponse(wp, 302, buf, url);
	websDone(wp, 200);

}


int websApiHandler(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg, 
	char_t *url, char_t *path, char_t *query)
{
	api_func_t		*sp = def_api_tbl;
	char_t		formBuf[FNAMESIZE];
	char_t		*cp, *formName,*pathName;
	int i;

	a_assert(websValid(wp));
	a_assert(url && *url);
	a_assert(path && *path == '/');

//printf("line=%d,url=%spath=%s,query=%s,\n",__LINE__,url,path,query);
	websStats.apiHits++;

/*
 *	Extract the form name
 */
	gstrncpy(formBuf, path, TSZ(formBuf));
	if ((formName = gstrchr(&formBuf[1], '/')) == NULL) {
		websError(wp, 200, T("Missing form name"));
		return 1;
	}
	formName++;
	if ((cp = gstrchr(formName, '/')) != NULL) {
		*cp = '\0';
	}
/*
 *	Lookup the C form function first and then try tcl (no javascript support 
 *	yet).
 */
	for (i = WLHOST; i < TB_MAX; i++)
	{
		sp = &def_api_tbl[i];
//	printf("line=%d,magicName=%s,formName=%s\n",__LINE__,sp->magic_name,formName);
		if(strcmp(formName,sp->magic_name) == 0)
		{
			break;
		}
	}
	if (i == TB_MAX) {
		websError(wp, 200, T("Form %s is not defined"), formName);
	} else {
		(*sp->api_func)(wp,path,query);
	}
	return 1;
}

#endif
