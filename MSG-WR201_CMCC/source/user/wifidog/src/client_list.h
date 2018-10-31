/********************************************************************\
 * This program is free software; you can redistribute it and/or    *
 * modify it under the terms of the GNU General Public License as   *
 * published by the Free Software Foundation; either version 2 of   *
 * the License, or (at your option) any later version.              *
 *                                                                  *
 * This program is distributed in the hope that it will be useful,  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
 * GNU General Public License for more details.                     *
 *                                                                  *
 * You should have received a copy of the GNU General Public License*
 * along with this program; if not, contact:                        *
 *                                                                  *
 * Free Software Foundation           Voice:  +1-617-542-5942       *
 * 59 Temple Place - Suite 330        Fax:    +1-617-542-2652       *
 * Boston, MA  02111-1307,  USA       gnu@gnu.org                   *
 *                                                                  *
\********************************************************************/

/* $Id: client_list.h,v 1.6.4.3 2016/08/05 05:13:13 cao.yongxiang Exp $ */
/** @file client_list.h
    @brief Client List functions
    @author Copyright (C) 2004 Alexandre Carmel-Veilleux <acv@acv.ca>
*/

#ifndef _CLIENT_LIST_H_
#define _CLIENT_LIST_H_


/** Counters struct for a client's bandwidth usage (in bytes)
 */
typedef struct _t_counters {
    unsigned long long	incoming;	/**< @brief Incoming data total*/
    unsigned long long	outgoing;	/**< @brief Outgoing data total*/
    unsigned long long	incoming_history;	/**< @brief Incoming data before wifidog restarted*/
    unsigned long long	outgoing_history;	/**< @brief Outgoing data before wifidog restarted*/
    time_t	last_updated;	/**< @brief Last update of the counters */
} t_counters;

/*
 * �ͻ��˵Ŀ��Ʊ�־
 * */
typedef enum {
    /*ÿ������λ����һ�����ͣ����¶���������λ*/
    WIFIDOG_CLIENT_CTRL_TYPE_RATE_LIMIT=0,	    /*�������*/
    WIFIDOG_CLIENT_CTRL_TYPE_EXPIRING_NOTICE,	    /*����ͨ��*/
} ControlType;
/*
 * �ͻ�����֤���Զ���
 * */
typedef enum {
    WIFIDOG_CLIENT_AUTH_TYPE_UNKOWN=0,		    /*δ֪֤����*/
    WIFIDOG_CLIENT_AUTH_TYPE_FREE=1,		    /*����֤����*/
    WIFIDOG_CLIENT_AUTH_TYPE_MAC_BLACK,		    /*MAC������*/
    WIFIDOG_CLIENT_AUTH_TYPE_MAC_GREY,		    /*MAC������*/
    WIFIDOG_CLIENT_AUTH_TYPE_MAC_WHITE,		    /*MAC������*/
    WIFIDOG_CLIENT_AUTH_TYPE_PHONE,		    /*�ֻ���֤*/
    WIFIDOG_CLIENT_AUTH_TYPE_WECHAT_URL,	    /*΢��URL��֤*/
    WIFIDOG_CLIENT_AUTH_TYPE_ACCOUNT,		    /*�˺���֤*/
#if (SWITCH_AUTH_MODE == FYES)
    WIFIDOG_CLIENT_AUTH_TYPE_SWITCH_NO_MAC_AUTH,	    /*���㽻��������֤(·��ģʽ),�޷���ȡmac��ͨ��ip��ͨ*/
#endif
} AuthType;
/** Client node for the connected client linked list.
 */
typedef struct	_t_client {
  struct	_t_client *next;        /**< @brief Pointer to the next client */
	char	*ip;			/**< @brief Client Ip address */
	char	*mac;			/**< @brief Client Mac address */
	char	*token;			/**< @brief Client token */
	int	expire;			/**��֤��Ч��*/
	int	runtime;
	int	free_auth_client;	/** ����֤�û�*/
	int	need_notify;		/*�����ǿ�����ߴ�֪ͨ�û�*/
	unsigned int fw_connection_state; /**< @brief Connection state in the
						     firewall */
	int	fd;			/**< @brief Client HTTP socket (valid only
					     during login before one of the
					     _http_* function is called */
	t_counters	counters;	/**< @brief Counters for input/output of
					     the client. */
	AuthType authType;		/*�ͻ��˵���֤����*/
	int ctrlType;			/*�ͻ��˵Ŀ��Ʊ�־*/
	union {
#if (WIFI_RATE_LIMIT == FYES) 
	    /*�������*/
	    int index;			/* ctrlType Ϊ�������ʱ�����ÿͻ����Ǵ�����Ƶģ�
					 * ��ʱindex�����������ȫ�ֱ��������������б���˿ͻ��������������飬��ʱʱ���õ�*/
#endif
	    void *reserve;
	}data;
} t_client;

/** @brief Get the first element of the list of connected clients
 */
t_client *client_get_first_client(void);

/** @brief Initializes the client list */
void client_list_init(void);

/** @brief Adds a new client to the connections list */
t_client *client_list_append(const char *ip, const char *mac, const char *token);

/** @brief Finds a client by its IP and MAC */
t_client *client_list_find(const char *ip, const char *mac);

/** @brief Finds a client only by its IP */
t_client *client_list_find_by_ip(const char *ip); /* needed by fw_iptables.c, auth.c 
					     * and wdctl_thread.c */

/** @brief Finds a client only by its Mac */
t_client *client_list_find_by_mac(const char *mac); /* needed by wdctl_thread.c */

/** @brief Finds a client by its token */
t_client *client_list_find_by_token(const char *token);

/** @brief Deletes a client from the connections list */
void client_list_delete(t_client *client);

/*delete aclient by mac returned by auth server*/
void client_delete_by_mac(char* macStr);
/*mac������ʾ���Һ���*/
int client_find_need_notify(const char *ip, const char *mac);
void clist_list_write_file(void);
#if (WIFIDOG_REMOTE_LOGOUT == FYES) /*Զ�̹Ҷ�*/
int cli_del_by_mac(char* macStr);
#endif
#define LOCK_CLIENT_LIST() do { \
	debug(LOG_DEBUG, "Locking client list"); \
	pthread_mutex_lock(&client_list_mutex); \
	debug(LOG_DEBUG, "Client list locked"); \
} while (0)

#define UNLOCK_CLIENT_LIST() do { \
	debug(LOG_DEBUG, "Unlocking client list"); \
	pthread_mutex_unlock(&client_list_mutex); \
	debug(LOG_DEBUG, "Client list unlocked"); \
} while (0)

#if (UTT_KNOWIFI == FYES)
typedef struct st_freeDomain{
    char domain[128];
    unsigned long freetime;
    unsigned long runtime;
    struct st_freeDomain *next;
}freeDomain;
extern freeDomain *freeDomainListHead;
freeDomain *freeDomainFind(char *domain);
int freeDomainEdit(char *domain, unsigned long freetime);
int freeDomainDel(char *domain);
int freeDomainCheckTimeOut(unsigned long checkTime);
#endif

#endif /* _CLIENT_LIST_H_ */
