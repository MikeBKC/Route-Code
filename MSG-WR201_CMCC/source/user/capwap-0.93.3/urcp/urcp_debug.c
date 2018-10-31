#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "urcp_debug.h"

#ifdef FEATURE_AP
#include "ap_msg.h"
#endif
#ifndef URCP_REL
#include "debug_module.c"

#ifdef FEATURE_AC
extern urcp_shm_t* shm_ptr;
#endif

static char fname[32];

/*AP_AC的debug通用定时器*/
#define AP_AC_DEBUG_DELAY_MS 1000 
static mtimer_t debug_check_timer;

void ap_ac_debug_init()
{
    MTIMER_START(&debug_check_timer, AP_AC_DEBUG_DELAY_MS);
}
void ap_ac_debug_check()
{
    if (MTIMER_TIMEOUT(&debug_check_timer))
    {
	if ((ttyname(2) == NULL) || (ttyname(1) == NULL))
	{
	    /*重定向到串口*/
	    chang_printf_io(NULL);
	}
	MTIMER_START(&debug_check_timer, AP_AC_DEBUG_DELAY_MS);
    }
}

int chang_printf_io(char *io_file)
{
    int fd = 0;
    char *tty = NULL;

    tty == ttyname(2);
    if ((tty != NULL) && (io_file != NULL) &&(strncmp(tty, io_file, 32) == 0 ))
    {
	return 0;
    }

    if (io_file == NULL)
    {
	fd = open(URCP_CONSOLE, O_WRONLY);
    }
    else
    {
	fd = open(io_file, O_WRONLY);
    }
    if (fd != -1)
    {
	dup2(fd, 2);
	dup2(fd, 1);
	close(fd);
    }

    return 0;
}
int urcp_debug(int id, int level, char *format, ...)
{
	int dlevel	= 0;
	int llevel	= 0;
	char buf[256], *ps;
	va_list valst;
	urcp_db_unit *pm = NULL;

	pm = &urcp_debug_module[0];
	if ( (pm->dlevel != URCP_DB_MIN) && (id >= 0) && (id < URCP_MODULE_ID_MAX) ) 
	{
		while((pm->id != id) && (pm->id < URCP_MODULE_ID_MAX))
		{
		    pm++;
		}
		if(pm->id == id)
		{
		    dlevel	= pm->dlevel;
		    llevel	= pm->llevel;

		    va_start(valst, format);

		    // Conosle
		    if(level <= dlevel) {
    //			fprintf(stderr, "%d in %s of %s", __LINE__, __func__, __FILE__);
#if 1
			    if (vfprintf(stderr, format, valst) == -1)
			    { 
				chang_printf_io(NULL);
			    }
#else
			    vfprintf(stderr, format, valst);
			    if (ttyname(2) == NULL)
			    {
				freopen("/dev/console","w",stderr);
			    }
#endif
		    }

		    // Logfile
		    if(level <= llevel) {
			    ps = buf;
			    if(level) { ps += sprintf(ps, "[%s|%d] ", pm->magic, __LINE__); }
			    vsprintf(ps, format, valst);

			    FILE *fp = fopen(fname, "at");
			    if(fp) {
				    int len = strlen(ps);
				    fwrite(ps, 1, len, fp);
				    fclose(fp);
			    }
		    }

		    va_end(valst);
		}
		else
		{
		    printf(stderr, "%s: Invalid ID: %d\n", __func__, id);
		}
	}

	return 0;
}



int urcp_set_debug(struct msgbuf_t *msgbuf)
{
	urcp_db_t urcp_db;
	urcp_db_unit *pm = NULL;

	memset(&urcp_db, 0, sizeof(urcp_db_t));

	memcpy(&urcp_db, msgbuf->sw_msg.buffer, sizeof(urcp_db_t));
//	printf("id = %d, dlevel = %d, llevel = %d\n", urcp_db.id, urcp_db.dlevel, urcp_db.llevel);
	if ((urcp_db.id == 0) && (urcp_db.dlevel >= 0))
	{
//	    printf("%s , start chang:%d\n", __func__, urcp_db.io_id);
	    if (urcp_db.io_file[0] == '\0')
	    {
		chang_printf_io(NULL);
	    }
	    else
	    {
		chang_printf_io(urcp_db.io_file);
	    }
	}
	if (urcp_db.id >= 0) {
		pm = &urcp_debug_module[0];
		if (urcp_db.id < URCP_MODULE_ID_MAX) {
		    while(pm->id < URCP_MODULE_ID_MAX)
		    {
			if(urcp_db.id == pm->id)
			{
			    pm->dlevel = urcp_db.dlevel;
			    pm->llevel = urcp_db.llevel;
			    if(urcp_db.logfile[0] != '\0')
			    {
				    memset(fname,'\0',sizeof(fname));
				    strcpy(fname,urcp_db.logfile);
			    }
			}
			pm++;
		    }
		}else if(urcp_db.id == URCP_MODULE_ID_ALL){
		    while(pm->id < URCP_MODULE_ID_MAX)
		    {
			pm->dlevel = urcp_db.dlevel;
			pm->llevel = urcp_db.llevel;
			if(urcp_db.logfile[0] != '\0')
			{
				memset(fname,'\0',sizeof(fname));
				strcpy(fname,urcp_db.logfile);
			}
			pm++;
		    }
		}
	}

	return 0;
}
int  urcp_get_debug(struct msgbuf_t *msgbuf)
{
	urcp_db_t urcp_db;
	urcp_db_unit *pm = &urcp_debug_module[0];
#ifdef FEATURE_AC
	int i, j;
    urcp_dev_t *tmp=NULL;
    int32 tb_index;
#endif
	memset(&urcp_db, 0, sizeof(urcp_db_t));
	memcpy(&urcp_db, msgbuf->sw_msg.buffer, sizeof(urcp_db_t));
	if (urcp_db.id >= 0) {
		if (urcp_db.id < URCP_MODULE_ID_MAX) {
		    printf("\n");
		    while(pm->id < URCP_MODULE_ID_MAX)
		    {
			if(pm->id == urcp_db.id)
			{
			    printf("%-8s%-12s%-12s%-12s%-12s\n","ID", "Module", "Debug", "Log", "Logfile Path");
			    printf("========================================================\n");
			    printf("%-8d%-12s%-12d%-12d%-12s\n",pm->id,pm->magic,pm->dlevel,pm->llevel,fname);
			    break;
			}
			pm++;
		    }
		}else if(urcp_db.id == URCP_MODULE_ID_ALL){
			printf("\n");
			printf("%-8s%-12s%-12s%-12s%-12s\n","ID", "Module", "Debug", "Log", "Logfile Path");
			printf("========================================================\n");
			while(pm->id < URCP_MODULE_ID_MAX)
			{
				printf("%-8d%-12s%-12d%-12d%-12s\n",pm->id,pm->magic,pm->dlevel,pm->llevel,fname);
				pm++;
			}
		}
#ifdef FEATURE_AC
		else if(urcp_db.id == URCP_MSG_CLIENT){
			printf("\n");
			printf("node  SN        aged  vid  MAC                BSSID              SSID              clients\n");
			printf("==========================================================================================\n");
			for(i = 0u; i < DEV_HASH_SIZE; i++)
			{
				tb_index = shm_ptr->dev_tb_hash[i];
				while(tb_index!=NONE_NODE)
				{
					tmp = &shm_ptr->dev_table[tb_index-1];
					printf("%-4d  %-8d  %-4d  %-3d  %-17s  ",tb_index,tmp->serial_no,tmp->aged,tmp->vid,str_macaddr(tmp->mac));

                    for(j=0;j<tmp->wl_info.joined_sz_num;j++){
                        if(j != 0){
                            printf("                                              ");
                        }
                        printf("%-17s  %-16s  %-7d\n",
                                str_macaddr(tmp->wl_info.ssid_info[j].mac),
                                tmp->wl_info.ssid_info[j].ssid,tmp->wl_info.ssid_info[j].sta_num);
                    }

					tb_index = tmp->tb_next;
				}
			}
		}
#endif
	}

	return 0;
}

#endif
