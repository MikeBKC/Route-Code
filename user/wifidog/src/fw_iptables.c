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

/* $Id: fw_iptables.c,v 1.29.4.14 2016/08/29 08:55:00 cao.yongxiang Exp $ */
/** @internal
  @file fw_iptables.c
  @brief Firewall iptables functions
  @author Copyright (C) 2004 Philippe April <papril777@yahoo.com>
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "netdb.h"
#include "common.h"

#include "safe.h"
#include "conf.h"
#include "fw_iptables.h"
#include "firewall.h"
#include "debug.h"
#include "util.h"
#include "client_list.h"
#include "uttMachine.h"
#if (WIFIDOG_AUTH == FYES) || (WIFIDOG_MAKE_MONEY == FYES)
#include "typedef.h"
#include "mib.h"
#include "profacce.h"
#include "comExeFun.h"
#include "flash_api.h"
#include "signature.h"
#include "linux/netfilter_ipv4/utt_netfilter.h"
#if (WHITELIST == 1 )
#ifndef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
/*移到内核实现，通过iptables的WiFiMatch匹配模块实现*/
static void whiteList();
#endif
#endif
#endif

static int iptables_do_command(const char *format, ...);
static char *iptables_compile(const char *, const char *, const t_firewall_rule *);
static void iptables_load_ruleset(const char *, const char *, const char *);

extern pthread_mutex_t	client_list_mutex;
extern pthread_mutex_t	config_mutex;

/**
Used to supress the error output of the firewall during destruction */ 
static int fw_quiet = 0;
static int free_auth_add = 0;
#ifdef WIFI_IPSET_LAN
#if (VLAN_CONFIG == FYES)
int lan_flag[4+MAX_VLAN_CONFIG_PROFILES];
static void wifidogAddOrDelVlanRoute(int vid ,int vlan_net,int cidr,int action);
#else
int lan_flag[4] = {0,0,0,0};
#endif
#endif
static int del_cnt = 3;
#if (WIFI_RATE_LIMIT == FYES)
RateLimitInfo rateLimitGroup[MAX_WIFIDOG_RATE_LIMIT_GROUP];
#endif

/** @internal
 * @brief Insert $ID$ with the gateway's id in a string.
 *
 * This function can replace the input string with a new one. It assumes
 * the input string is dynamically allocted and can be free()ed safely.
 *
 * This function must be called with the CONFIG_LOCK held.
 */
static void
iptables_insert_gateway_id(char **input)
{
	char *token;
	const s_config *config;
	char *buffer;

	if (strstr(*input, "$ID$")==NULL)
		return;


	while ((token=strstr(*input, "$ID$"))!=NULL)
		/* This string may look odd but it's standard POSIX and ISO C */
		memcpy(token, "%1$s", 4);

	config = config_get_config();
	safe_asprintf(&buffer, *input, config->gw_interface);

	free(*input);
	*input=buffer;
}

/** @internal 
 * */
static int
iptables_do_command(const char *format, ...)
{
	va_list vlist;
	char *fmt_cmd;
	char *cmd;
	int rc;

	va_start(vlist, format);
	safe_vasprintf(&fmt_cmd, format, vlist);
	va_end(vlist);

	safe_asprintf(&cmd, "iptables %s", fmt_cmd);
	free(fmt_cmd);

	iptables_insert_gateway_id(&cmd);

	//debug(LOG_DEBUG, "Executing command: %s", cmd);

	rc = execute(cmd, fw_quiet);

	if (rc!=0)
		debug(LOG_ERR, "iptables command failed(%d): %s", rc, cmd);

	free(cmd);

	return rc;
}

/**
 * @internal
 * Compiles a struct definition of a firewall rule into a valid iptables
 * command.
 * @arg table Table containing the chain.
 * @arg chain Chain that the command will be (-A)ppended to.
 * @arg rule Definition of a rule into a struct, from conf.c.
 */
	static char *
iptables_compile(const char * table, const char *chain, const t_firewall_rule *rule)
{
	char	command[MAX_BUF],
		mode[7];

	memset(command, 0, MAX_BUF);
	memset(mode, 0, 7);

	if (rule->block_allow == 1) {
#if 0
		mode = safe_strdup("RETURN");
#endif
		strcpy(mode,"RETURN");
	} else {
#if 0
		mode = safe_strdup("REJECT");
#endif
		strcpy(mode,"REJECT");
	}

	snprintf(command, sizeof(command),  "-t %s -A %s ",table, chain);
	if (rule->mask != NULL) {
		snprintf((command + strlen(command)), (sizeof(command) - 
					strlen(command)), "-d %s ", rule->mask);
	}
	if (rule->protocol != NULL) {
		snprintf((command + strlen(command)), (sizeof(command) -
					strlen(command)), "-p %s ", rule->protocol);
	}
	if (rule->port != NULL) {
		snprintf((command + strlen(command)), (sizeof(command) -
					strlen(command)), "--dport %s ", rule->port);
	}
	snprintf((command + strlen(command)), (sizeof(command) - 
				strlen(command)), "-j %s", mode);

#if 0
	free(mode);
#endif

	/* XXX The buffer command, an automatic variable, will get cleaned
	 * off of the stack when we return, so we strdup() it. */
	return(safe_strdup(command));
}

/**
 * @internal
 * Load all the rules in a rule set.
 * @arg ruleset Name of the ruleset
 * @arg table Table containing the chain.
 * @arg chain IPTables chain the rules go into
 */
	static void
iptables_load_ruleset(const char * table, const char *ruleset, const char *chain)
{
	t_firewall_rule		*rule;
	char			*cmd=NULL;

	debug(LOG_DEBUG, "Load ruleset %s into table %s, chain %s", ruleset, table, chain);

	for (rule = get_ruleset(ruleset); rule != NULL; rule = rule->next) {
		cmd = iptables_compile(table, chain, rule);
		if(cmd != NULL) {
		debug(LOG_DEBUG, "Loading rule \"%s\" into table %s, chain %s", cmd, table, chain);
		iptables_do_command(cmd);
		free(cmd);
		cmd = NULL;
		}
	}

	debug(LOG_DEBUG, "Ruleset %s loaded into table %s, chain %s", ruleset, table, chain);
}

#ifdef WIFI_PRE_REDIRECT
	void
iptables_fw_clear_authservers(char *ip)
#else
	void
iptables_fw_clear_authservers(void)
#endif
{
#ifdef WIFI_PRE_REDIRECT
    if(ip != NULL)
	iptables_do_command("-t filter -D " TABLE_WIFI_PRE_REDIRECT " -d %s -j ACCEPT", ip);
#else
	iptables_do_command("-t filter -F " TABLE_WIFIDOG_AUTHSERVERS);
	iptables_do_command("-t nat -F " TABLE_WIFIDOG_AUTHSERVERS);
#endif
}

	void
iptables_fw_set_authservers(void)
{
	const s_config *config;
	t_auth_serv *auth_server;

	config = config_get_config();

	for (auth_server = config->auth_servers; auth_server != NULL; auth_server = auth_server->next) {
		if (auth_server->last_ip && strcmp(auth_server->last_ip, "0.0.0.0") != 0) {
#ifdef WIFI_PRE_REDIRECT
			iptables_do_command("-t filter -I " TABLE_WIFI_PRE_REDIRECT " -d %s -j ACCEPT", auth_server->last_ip);
#else
			iptables_do_command("-t filter -A " TABLE_WIFIDOG_AUTHSERVERS " -d %s -j ACCEPT", auth_server->last_ip);
			iptables_do_command("-t nat -A " TABLE_WIFIDOG_AUTHSERVERS " -d %s -j ACCEPT", auth_server->last_ip);
#endif
		}
	}

	t_secondary_auth_server *sec_server;
	for (sec_server = config->secondary_auth_server; sec_server != NULL;sec_server = sec_server->next){
	    if (sec_server->ip && strcmp(sec_server->ip, "0.0.0.0") != 0) {
		iptables_do_command("-t filter -A " TABLE_WIFIDOG_AUTHSERVERS " -d %s -j ACCEPT", sec_server->ip);
		iptables_do_command("-t nat -A " TABLE_WIFIDOG_AUTHSERVERS " -d %s -j ACCEPT", sec_server->ip);
	    }
	}
}

#if (WIFIDOG_MAKE_MONEY != FYES)
#if (WX_DOMAIN_WHITE_LIST_ON == FYES)
/*
 * WecharPolicyRuleAdd
 * 微信策略放通规则添加
 * 通过配置查找l7规则文件名
 *
 * */
extern void WechatPolicyRuleAdd(void){
    MibProfileType mibType = MIB_PROF_PDB_GLOBAL;
    PdbProfile *prof= NULL;
    char rule[256];
    int i,j;
#if 0
    int max, min;
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) 
#endif
    {
	i = 168;
	prof = (PdbProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) {
	    if((prof->type == wifidogUp) && (i == 168)) {
		for(j=0;j<MAX_PDB_LINE_COUNT;j++)
		{
		    if(strlen(prof->line[j])!=0)
		    {
			memset(rule,0,256);
			buildRule(prof->line[j],rule);
			if(strlen(rule) > 0U) {
#ifdef WIFI_PRE_REDIRECT
			    iptables_do_command("-t filter -A %s %s -j RETURN",TABLE_WIFI_PRE_REDIRECT,rule);
#else
			    iptables_do_command("-t filter -A " TABLE_WIFIDOG_UNKNOWN " %s -j RETURN",rule);
#endif
			}
		    }
		}
	    } else {
		debug(LOG_ERR, "[ERROR] please check wechat policy config instindex.");
	    }
	}
    }
}
/*
 * WechatDomainAdd
 * 微信域名放通
 * 通过配置查找放通域名
 *
 * */
extern void WechatDomainAdd(void){
    MibProfileType mibType = MIB_PROF_PDB_GLOBAL;
    PdbProfile *prof= NULL;
    char *ptr = NULL, *start = NULL;
    int i,j;
#if 0
    int max, min;
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) 
#endif
    {
	i = 168;
	prof = (PdbProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) {
	    if((prof->type == wifidogUp) && (i == 168)) {
		for(j=0;j<MAX_PDB_LINE_COUNT;j++)
		{
		    if((strlen(prof->line[j])!=0) && (strncmp(prof->line[j], "DNS|",4) == 0))
		    {
			start = (prof->line[j])+4;
			debug(LOG_DEBUG, "Dns string: %s",start);
			ptr = strtok(start, ";");
			while(ptr != NULL) {
			    getDomainNameIP(ptr,WEIXIN_IPSET_GRP);
			    ptr = strtok(NULL, ";");
			}
		    }
		}
	    } else {
		debug(LOG_ERR, "[ERROR] please check wechat policy config instindex.");
	    }
	}
    }
}
#endif
#endif

#if (WIFIDOG_MAKE_MONEY == FYES)
/*单价单位为分*/
static int getUnitPrice(void)
{
    MakeMoneyProfile *prof = NULL;
    float charge;
    int price = 1;
    uttSemP(SEM_SALE_WIFI,0);
    prof = (MakeMoneyProfile *)ProfGetInstPointByIndex(MIB_PROF_MAKE_MONEY,0);
    if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)))
    {
	sscanf(prof->charge,"%f",&charge);
	price = (int)(charge*100);
    }
    uttSemV(SEM_SALE_WIFI,0);
    return price;
}
#endif

#if (UTT_KNOWIFI == FYES)
/*家路由 放通目的地址*/
char *knowifi_white_list[] = {
    //*.qingxueba.com在内核中实现
    "*.greenwifi.com",
    "*.qingxueba.com",
    "knowifi.me",
    "null"
};
static int initKnowifiIptables()
{
    char cmd[255];
    int idx = 0;

    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "iptables -t filter -N knowifi_dns_free");
    execute(cmd, 0);
    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "iptables -t filter -I FORWARD -j knowifi_dns_free");
    execute(cmd, 0);
    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "iptables -t filter -I INPUT -j knowifi_dns_free");
    execute(cmd, 0);

    for (idx = 0; strcmp(knowifi_white_list[idx], "null"); idx++)
    {
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "iptables -t filter -A knowifi_dns_free -m domain --name \"%s\" -j ACCEPT", knowifi_white_list[idx]);
	execute(cmd, 0);
    }

    return 1;
}
static int destoryKnowifiIptables()
{
    char cmd[255];

    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "iptables -t filter -F knowifi_dns_free");
    execute(cmd, 0);
    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "iptables -t filter -D FORWARD -j knowifi_dns_free");
    execute(cmd, 0);
    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "iptables -t filter -D INPUT -j knowifi_dns_free");
    execute(cmd, 0);
    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "iptables -t filter -X knowifi_dns_free");
    execute(cmd, 0);
    return 1;
}
#endif


/** Initialize the firewall rules
*/
	int
iptables_fw_init(void)
{
	const s_config *config;
	char * ext_interface = NULL;
	int gw_port = 0;
	t_trusted_mac *p;

	fw_quiet = 0;
#ifdef WIFI_IPSET_LAN
	int i = 0;
#endif

	LOCK_CONFIG();
	config = config_get_config();
	gw_port = config->gw_port;
	if (config->external_interface) {
		ext_interface = safe_strdup(config->external_interface);
	} else {
		ext_interface = get_ext_iface();
	}

	if (ext_interface == NULL) {
		UNLOCK_CONFIG();
		debug(LOG_ERR, "FATAL: no external interface");
		return 0;
	}
	/*
	 *
	 * Everything in the MANGLE table
	 *
	 */

#if (WIFIDOG_MAKE_MONEY != FYES)
	execute(("ipset -q -N " IPSET_WIFIDOG_FREE_AUTH " iphash"), 0);
#if (WX_DOMAIN_WHITE_LIST_ON == FYES)
	execute(("ipset -q -N " WEIXIN_IPSET_GRP " iphash"), 0);
#endif
#endif
#if (SWITCH_AUTH_MODE == FYES)
	execute(("ipset -q -N " IPSET_GROUP_SWITCH_CLIENT_LIST " iphash"), 0);
#endif
#if ((WIFIDOG_AUTH == FYES) || (WIFIDOG_MAKE_MONEY == FYES))
#if ((WHITELIST == 1) || (WIFIDOG_MAKE_MONEY == FYES))
	execute(("ipset -q -N " WHITE_LIST " iphash"), 0);
#endif
#endif
#ifdef WIFI_IPSET_LAN
	initLanIpset();
#endif


#if 0
#ifndef WIFI_IPSET_LAN
/*del needless mangle table, because ipset group*/
	/* Create new chains */
	iptables_do_command("-t mangle -N " TABLE_WIFIDOG_TRUSTED);
	iptables_do_command("-t mangle -N " TABLE_WIFIDOG_OUTGOING);
	iptables_do_command("-t mangle -N " TABLE_WIFIDOG_INCOMING);
#if (WIFIDOG_AUTH == FYES)
	iptables_do_command("-t mangle -N " TABLE_WIFIDOG_PREROUTING);
	/* Assign links and rules to these new chains */
	iptables_do_command("-t mangle -I PREROUTING -i %s -j " TABLE_WIFIDOG_PREROUTING, config->gw_interface);
#ifdef WIFI_IPSET_LAN
#if (VLAN_CONFIG == FYES)
	for (i =0;i<4+MAX_VLAN_CONFIG_PROFILES;i++)
#else
	for (i =0;i<4;i++)
#endif
	{
	if(lan_flag[i] > 0) {
	iptables_do_command("-t mangle -I %s 1 -m set --set %s%d src -j RETURN",TABLE_WIFIDOG_PREROUTING,WIFI_GROUP_PREFIX,lan_flag[i]);
	}
	}
#else
	iptables_do_command("-t mangle -I %s 1 -i %s -j " TABLE_WIFIDOG_OUTGOING, TABLE_WIFIDOG_PREROUTING, config->gw_interface);
#endif
	iptables_do_command("-t mangle -I %s 1 -i %s -j " TABLE_WIFIDOG_TRUSTED, TABLE_WIFIDOG_PREROUTING, config->gw_interface);//this rule will be inserted before the prior one
	iptables_do_command("-t mangle -I %s 1 -m set --set %s src -j RETURN",TABLE_WIFIDOG_PREROUTING,IPSET_WIFIDOG_FREE_AUTH);
#else

	/* Assign links and rules to these new chains */
	iptables_do_command("-t mangle -I PREROUTING 1 -i %s -j " TABLE_WIFIDOG_OUTGOING, config->gw_interface);
	iptables_do_command("-t mangle -I PREROUTING 1 -i %s -j " TABLE_WIFIDOG_TRUSTED, config->gw_interface);//this rule will be inserted before the prior one
#endif
	iptables_do_command("-t mangle -I POSTROUTING 1 -o %s -j " TABLE_WIFIDOG_INCOMING, config->gw_interface);
	//iptables_do_command("-t mangle -I %s -m set --set %s dst -j RETURN",TABLE_WIFIDOG_INCOMING,IPSET_WIFIDOG_FREE_AUTH);

#if 0
	/*del needless trust mac list rule*/
	for (p = config->trustedmaclist; p != NULL; p = p->next)
		iptables_do_command("-t mangle -A " TABLE_WIFIDOG_TRUSTED " -m mac --mac-source %s -j MARK --set-mark %d", p->mac, FW_MARK_KNOWN);
#endif

/*end del needless mangle table, because ipset group*/
#endif
#endif
#ifdef WIFI_IPSET_LAN
#ifdef WIFI_PRE_REDIRECT
    /* 解决认证跳转慢的问题
     * 修改方法：
     *	在PREROUTING中,构造http的响应包做302的处理，
     *	通过内核HTTPREDIRECT来处理
     * 注意：
     *	filter nat表中的规则都不需要
     *	*/
	iptables_do_command("-t filter  -N " TABLE_WIFI_PRE_REDIRECT);
	iptables_load_ruleset("filter", "unknown-users", TABLE_WIFI_PRE_REDIRECT);
#if (VLAN_CONFIG == FYES)
	for (i =0;i<4+MAX_VLAN_CONFIG_PROFILES;i++)
#else
	for (i =0;i<4;i++)
#endif
	{
	if(lan_flag[i] > 0) {
	iptables_do_command("-t filter -A %s -m set --set %s%d src -j RETURN",TABLE_WIFI_PRE_REDIRECT,WIFI_GROUP_PREFIX,lan_flag[i]);
#if (WIFIDOG_MAC_AUTH == FYES)
	iptables_do_command("-t filter -A %s -m set --set %s%d src -j DROP",TABLE_WIFI_PRE_REDIRECT,WIFI_GROUP_BLACK_MAC,lan_flag[i]);
#endif
	}
	}
#if (WIFIDOG_MAKE_MONEY != FYES)
	iptables_do_command("-t filter  -A %s -m set --set %s src -j RETURN",TABLE_WIFI_PRE_REDIRECT,IPSET_WIFIDOG_FREE_AUTH);
#endif
#if (SWITCH_AUTH_MODE == FYES)
	iptables_do_command("-t filter  -A %s -m set --set %s src -j RETURN",TABLE_WIFI_PRE_REDIRECT,IPSET_GROUP_SWITCH_CLIENT_LIST);
#endif
	//iptables_do_command("-t mangle -A %s -j " TABLE_WIFIDOG_AUTHSERVERS, TABLE_WIFI_PRE_REDIRECT);
#if ((WIFIDOG_AUTH == FYES) || (WIFIDOG_MAKE_MONEY == FYES))
#if ((WHITELIST == 1) || (WIFIDOG_MAKE_MONEY == FYES))
	iptables_do_command("-t filter -A %s -m set --set %s dst -j RETURN",TABLE_WIFI_PRE_REDIRECT,WHITE_LIST);
#endif
#endif
#if 1
#if (WIFIDOG_MAKE_MONEY != FYES)
#if (WX_DOMAIN_WHITE_LIST_ON == FYES)
	/*微信策略添加*/
	WechatPolicyRuleAdd();
#endif
#endif
#else
	iptables_do_command("-t filter -A %s -p tcp -m multiport --dport 443,8080 -m layer7 --l7proto Wechat -j RETURN",TABLE_WIFI_PRE_REDIRECT);
#endif
#if (WIFIDOG_MAKE_MONEY != FYES) 
#if (WX_DOMAIN_WHITE_LIST_ON == FYES)
	iptables_do_command("-t filter -A %s -m set --set %s dst -j RETURN",TABLE_WIFI_PRE_REDIRECT,WEIXIN_IPSET_GRP);
#endif
#endif
	iptables_do_command("-t filter -A %s -m set --set %s dst -j RETURN ",TABLE_WIFI_PRE_REDIRECT,SYS_LAN_RANGE_GROUP);
#if (WIFIDOG_MAKE_MONEY == FYES)
	    iptables_do_command("-t filter -A %s -p tcp --dport 80 -j WIFIREJECT --redirect-val %u --auth-domain %s:%d --login-url %s --server-version %s --unit-price %d",TABLE_WIFI_PRE_REDIRECT,config->tmpSN,config->auth_servers->authserv_hostname,config->auth_servers->authserv_http_port,config->auth_servers->authserv_login_script_path_fragment,SERV_VERSION, getUnitPrice());
#else
#if (WIFIDOG_USER_SECRET_KEY == FYES)
	if(strcmp(config->secret_key,SECRET_KEY_VALUE) != 0){
	    iptables_do_command("-t filter -A %s -p tcp --dport 80 -j WIFIREJECT --redirect-val %u --auth-domain %s:%d --login-url %s --secret-key %s  --server-version %s" ,TABLE_WIFI_PRE_REDIRECT,config->tmpSN,config->auth_servers->authserv_hostname,config->auth_servers->authserv_http_port,config->auth_servers->authserv_login_script_path_fragment,config->secret_key,SERV_VERSION);
	}else
#endif
	{
	    iptables_do_command("-t filter -A %s -p tcp --dport 80 -j WIFIREJECT --redirect-val %u --auth-domain %s:%d --login-url %s --server-version %s",TABLE_WIFI_PRE_REDIRECT,config->tmpSN,config->auth_servers->authserv_hostname,config->auth_servers->authserv_http_port,config->auth_servers->authserv_login_script_path_fragment,SERV_VERSION);
	}
#endif /*WIFIDOG_MAKE_MONEY*/
	iptables_do_command("-t filter -A %s -p tcp --dport 80 -j RETURN",TABLE_WIFI_PRE_REDIRECT); /*TCP三次握手的包放过*/
	iptables_do_command("-t filter -A %s -j DROP",TABLE_WIFI_PRE_REDIRECT);
#ifdef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
	/*wifidog 域名白名单生效规则，匹配域名并把域名ip添加到ipset组中*/
	iptables_do_command("-t filter -A INPUT -p udp --sport 53 -m wifiMatch --add-dns-ip -j RETURN ");
	iptables_do_command("-t filter -A FORWARD -p udp --sport 53 -m wifiMatch --add-dns-ip -j RETURN ");
#endif
#if(WIFIDOG_MAKE_MONEY == FYES)
	iptables_do_command("-t filter -A FORWARD -m uttdev --is-salewifi-in -j "TABLE_WIFI_PRE_REDIRECT);
#elif (UTT_KNOWIFI == FYES)
	iptables_do_command("-t filter -I FORWARD -m uttdev --is-lan-in -m set --set %s src -j %s",KNOWIFI_REDIRECT_CLIENT ,TABLE_WIFI_PRE_REDIRECT);
	initKnowifiIptables();

#else
	iptables_do_command("-t filter -A FORWARD -m uttdev --is-lan-in -j "TABLE_WIFI_PRE_REDIRECT);
#endif
#endif
#endif
	/*
	 *
	 * Everything in the NAT table
	 *
	 */

#if !defined(WIFI_PRE_REDIRECT)
	/* Create new chains */
	iptables_do_command("-t nat -N " TABLE_WIFIDOG_OUTGOING);
#if 0
/*del needless rules*/
	iptables_do_command("-t nat -N " TABLE_WIFIDOG_WIFI_TO_ROUTER);
#endif
	iptables_do_command("-t nat -N " TABLE_WIFIDOG_WIFI_TO_INTERNET);
#if 0
/*del needless rules*/
	iptables_do_command("-t nat -N " TABLE_WIFIDOG_GLOBAL);
	iptables_do_command("-t nat -N " TABLE_WIFIDOG_UNKNOWN);
/*end del needless rules*/
#endif
	iptables_do_command("-t nat -N " TABLE_WIFIDOG_AUTHSERVERS);

	/* Assign links and rules to these new chains */
#if (MULTI_LAN == FYES)
#if (IP_GRP == FYES)
	iptables_do_command("-t nat -A PREROUTING -m set --set %s src -j " TABLE_WIFIDOG_OUTGOING,SYS_LAN_RANGE_GROUP);
#else
#error ipset has been removed
#endif
#else
	iptables_do_command("-t nat -A PREROUTING -i %s+ -j " TABLE_WIFIDOG_OUTGOING, config->gw_interface);
#endif

#if((MULTI_LAN == FYES))
#if(IP_GRP == FYES)
	iptables_do_command("-t nat -A " TABLE_WIFIDOG_OUTGOING " -m set --set %s dst -j RETURN ", SYS_LAN_RANGE_GROUP);
#else
	iptables_do_command("-t nat -A " TABLE_WIFIDOG_OUTGOING " -d %s -j RETURN ", config->gw_address);
#endif
#else
	iptables_do_command("-t nat -A " TABLE_WIFIDOG_OUTGOING " -d %s -j RETURN ", config->gw_address);
#endif
#if 0
/*del needless rules*/
	iptables_do_command("-t nat -A " TABLE_WIFIDOG_WIFI_TO_ROUTER " -j ACCEPT");
#endif

#if (WIFIDOG_MAKE_MONEY != FYES)
	iptables_do_command("-t nat -A " TABLE_WIFIDOG_OUTGOING " -m set --set %s src -j RETURN",IPSET_WIFIDOG_FREE_AUTH);
#endif
#if (SWITCH_AUTH_MODE == FYES)
	iptables_do_command("-t nat -A " TABLE_WIFIDOG_OUTGOING " -m set --set %s src -j RETURN",IPSET_GROUP_SWITCH_CLIENT_LIST);
#endif
	iptables_do_command("-t nat -A " TABLE_WIFIDOG_OUTGOING " -j " TABLE_WIFIDOG_WIFI_TO_INTERNET);
#ifdef WIFI_IPSET_LAN
#if (VLAN_CONFIG == FYES)
	for (i =0;i<4+MAX_VLAN_CONFIG_PROFILES;i++)
#else
	for (i =0;i<4;i++)
#endif
	{
	if(lan_flag[i] > 0) {
	iptables_do_command("-t nat -A " TABLE_WIFIDOG_WIFI_TO_INTERNET " -m set --set %s%d src -j RETURN",WIFI_GROUP_PREFIX,lan_flag[i]);
#if (WIFIDOG_MAC_AUTH == FYES)
	iptables_do_command("-t nat -A " TABLE_WIFIDOG_WIFI_TO_INTERNET " -m set --set %s%d src -j DROP",WIFI_GROUP_BLACK_MAC,lan_flag[i]);
#endif
	}
	}
#else
	iptables_do_command("-t nat -A " TABLE_WIFIDOG_WIFI_TO_INTERNET " -m mark --mark 0x%u -j ACCEPT", FW_MARK_KNOWN);
	iptables_do_command("-t nat -A " TABLE_WIFIDOG_WIFI_TO_INTERNET " -m mark --mark 0x%u -j ACCEPT", FW_MARK_PROBATION);
#endif
#if (WIFIDOG_MAKE_MONEY != FYES)
#if (WX_DOMAIN_WHITE_LIST_ON == FYES)
	iptables_do_command("-t nat -A " TABLE_WIFIDOG_WIFI_TO_INTERNET "  -p tcp --dport 80 -m set --set " WEIXIN_IPSET_GRP " dst -j RETURN "); /*微信白名单*/
#endif
#endif
	/* 开机默认数据包不会重定向到认证服务器
	 * 检测外网连接后，删除此规则
	 * */
#if ((WIFIDOG_AUTH == FYES) || (WIFIDOG_MAKE_MONEY == FYES))
#if ((WHITELIST == 1) || (WIFIDOG_MAKE_MONEY == FYES))
	iptables_do_command("-t nat -A " TABLE_WIFIDOG_WIFI_TO_INTERNET " -m set --set " WHITE_LIST " dst -j RETURN "); /*微信白名单*/
#endif
#endif
	iptables_do_command("-t nat -A " TABLE_WIFIDOG_WIFI_TO_INTERNET " -j RETURN ");
	iptables_do_command("-t nat -A " TABLE_WIFIDOG_WIFI_TO_INTERNET " -j " TABLE_WIFIDOG_AUTHSERVERS);

#if 0
/*del needless rules*/
	iptables_do_command("-t nat -A " TABLE_WIFIDOG_UNKNOWN " -j " TABLE_WIFIDOG_AUTHSERVERS);
	iptables_do_command("-t nat -A " TABLE_WIFIDOG_UNKNOWN " -j " TABLE_WIFIDOG_GLOBAL);
/*end del needless rules*/
#endif
#if 1 //(CONFIG_X86_CPU == 1)
#if (CONFIG_X86_CPU == 1)
	iptables_do_command("-t nat -A " TABLE_WIFIDOG_WIFI_TO_INTERNET " -p tcp --dport 80 -j DNAT --to-destination %s:%d", config->gw_address, gw_port+1);
#endif
	iptables_do_command("-t nat -A " TABLE_WIFIDOG_WIFI_TO_INTERNET " -p tcp --dport 80 -j DNAT --to-destination %s:%d", config->gw_address, gw_port);
#else
	iptables_do_command("-t nat -A " TABLE_WIFIDOG_WIFI_TO_INTERNET " -p tcp --dport 80 -j REDIRECT --to-ports %d", gw_port);
#endif

#endif

#if !defined(WIFI_PRE_REDIRECT)
	/*
	 *
	 * Everything in the FILTER table
	 *
	 */

	/* Create new chains */
	iptables_do_command("-t filter -N " TABLE_WIFIDOG_WIFI_TO_INTERNET);
	iptables_do_command("-t filter -N " TABLE_WIFIDOG_AUTHSERVERS);
#if 0
/*del needless rules*/
	iptables_do_command("-t filter -N " TABLE_WIFIDOG_LOCKED);
	iptables_do_command("-t filter -N " TABLE_WIFIDOG_GLOBAL);
	iptables_do_command("-t filter -N " TABLE_WIFIDOG_VALIDATE);
/*end del needless rules*/
#endif
#ifndef WIFI_IPSET_LAN
	iptables_do_command("-t filter -N " TABLE_WIFIDOG_KNOWN);
#endif
	iptables_do_command("-t filter -N " TABLE_WIFIDOG_UNKNOWN);

	/* Assign links and rules to these new chains */

	/* Insert at the beginning */
#if (MULTI_LAN == FYES)
#if (IP_GRP == FYES)
	iptables_do_command("-t filter -I FORWARD -m set --set %s src -j " TABLE_WIFIDOG_WIFI_TO_INTERNET, SYS_LAN_RANGE_GROUP);
#else
#error ipset has been removed
#endif
#else
	iptables_do_command("-t filter -I FORWARD -i %s+ -j " TABLE_WIFIDOG_WIFI_TO_INTERNET, config->gw_interface);
#endif
#ifdef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
	/*wifidog 域名白名单生效规则，匹配域名并把域名ip添加到ipset组中*/
	iptables_do_command("-t filter -I INPUT -p udp --sport 53 -m wifiMatch --add-dns-ip -j RETURN ");
	iptables_do_command("-t filter -I FORWARD -p udp --sport 53 -m wifiMatch --add-dns-ip -j RETURN ");
#endif


#if (SWITCH_AUTH_MODE == FYES)
	iptables_do_command("-t filter -I " TABLE_WIFIDOG_WIFI_TO_INTERNET " -m set --set %s src -j RETURN",IPSET_GROUP_SWITCH_CLIENT_LIST);
#endif

#if (WIFIDOG_MAKE_MONEY != FYES)
	iptables_do_command("-t filter -I " TABLE_WIFIDOG_WIFI_TO_INTERNET " -m set --set %s src -j RETURN",IPSET_WIFIDOG_FREE_AUTH);
#endif
#if 0
/*del needless rules*/
	iptables_do_command("-t filter -A " TABLE_WIFIDOG_WIFI_TO_INTERNET " -m state --state INVALID -j DROP");

	/* XXX: Why this? it means that connections setup after authentication
	   stay open even after the connection is done... 
	   iptables_do_command("-t filter -A " TABLE_WIFIDOG_WIFI_TO_INTERNET " -m state --state RELATED,ESTABLISHED -j ACCEPT");*/

	//Won't this rule NEVER match anyway?!?!? benoitg, 2007-06-23
	//iptables_do_command("-t filter -A " TABLE_WIFIDOG_WIFI_TO_INTERNET " -i %s -m state --state NEW -j DROP", ext_interface);

	/* TCPMSS rule for PPPoE */
	iptables_do_command("-t filter -A " TABLE_WIFIDOG_WIFI_TO_INTERNET " -o %s -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu", ext_interface);
/*end del needless rules*/
#endif


#if 0
/*del needless rules*/
	iptables_do_command("-t filter -A " TABLE_WIFIDOG_WIFI_TO_INTERNET " -m mark --mark 0x%u -j " TABLE_WIFIDOG_LOCKED, FW_MARK_LOCKED);
	iptables_load_ruleset("filter", "locked-users", TABLE_WIFIDOG_LOCKED);

	iptables_do_command("-t filter -A " TABLE_WIFIDOG_WIFI_TO_INTERNET " -j " TABLE_WIFIDOG_GLOBAL);
	iptables_load_ruleset("filter", "global", TABLE_WIFIDOG_GLOBAL);
	iptables_load_ruleset("nat", "global", TABLE_WIFIDOG_GLOBAL);

	iptables_do_command("-t filter -A " TABLE_WIFIDOG_WIFI_TO_INTERNET " -m mark --mark 0x%u -j " TABLE_WIFIDOG_VALIDATE, FW_MARK_PROBATION);
	iptables_load_ruleset("filter", "validating-users", TABLE_WIFIDOG_VALIDATE);
/*end del needless rules*/
#endif

#ifdef WIFI_IPSET_LAN
#if (VLAN_CONFIG == FYES)
	for (i =0;i<4+MAX_VLAN_CONFIG_PROFILES;i++)
#else
	for (i =0;i<4;i++)
#endif
	{
	if(lan_flag[i] > 0) {
	iptables_do_command("-t filter -A " TABLE_WIFIDOG_WIFI_TO_INTERNET " -m set --set %s%d src -j RETURN ", WIFI_GROUP_PREFIX,lan_flag[i]);

#if (WIFIDOG_MAC_AUTH == FYES)
	iptables_do_command("-t filter -A " TABLE_WIFIDOG_WIFI_TO_INTERNET " -m set --set %s%d src -j DROP",WIFI_GROUP_BLACK_MAC,lan_flag[i]);
#endif
	}
	}
#else
	iptables_do_command("-t filter -A " TABLE_WIFIDOG_WIFI_TO_INTERNET " -m mark --mark 0x%u -j " TABLE_WIFIDOG_KNOWN, FW_MARK_KNOWN);
	iptables_load_ruleset("filter", "known-users", TABLE_WIFIDOG_KNOWN);
#endif
	iptables_load_ruleset("filter", "unknown-users", TABLE_WIFIDOG_WIFI_TO_INTERNET);
	iptables_do_command("-t filter -A " TABLE_WIFIDOG_WIFI_TO_INTERNET " -j " TABLE_WIFIDOG_AUTHSERVERS);
	iptables_fw_set_authservers();

	iptables_do_command("-t filter -A " TABLE_WIFIDOG_WIFI_TO_INTERNET " -j " TABLE_WIFIDOG_UNKNOWN);
	
#if 1
#if (WIFIDOG_MAKE_MONEY != FYES)
	/*微信策略添加*/
	WechatPolicyRuleAdd();
#endif
#else
	iptables_do_command("-t filter -A " TABLE_WIFIDOG_UNKNOWN " -p tcp -m multiport --dport 443,8080 -m layer7 --l7proto Wechat -j RETURN");
#endif
#if(WIFIDOG_MAKE_MONEY != FYES)
#if (WX_DOMAIN_WHITE_LIST_ON == FYES)
	iptables_do_command("-t filter -A " TABLE_WIFIDOG_UNKNOWN " -p tcp --dport 80 -m set --set " WEIXIN_IPSET_GRP " dst -j RETURN");
#endif
#endif
#if ((WIFIDOG_AUTH == FYES) || (WIFIDOG_MAKE_MONEY == FYES))
#if ((WHITELIST == 1) || (WIFIDOG_MAKE_MONEY == FYES))
	iptables_do_command("-t filter -A " TABLE_WIFIDOG_UNKNOWN " -m set --set " WHITE_LIST " dst -j RETURN");
#endif
#endif
	/* TCPMSS rule for PPPoE */
	iptables_do_command("-t filter -A " TABLE_WIFIDOG_UNKNOWN " -o %s -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu", ext_interface);
	
	iptables_do_command("-t filter -A " TABLE_WIFIDOG_UNKNOWN " -j REJECT --reject-with icmp-port-unreachable");
#endif

#if (WIFIDOG_MAKE_MONEY != FYES)
	MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
	WebAuthGlobalProfile *profG = NULL;
	//ProfInit();
	profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
	/*web认证不开启不会用到计费模式返回0*/
	if((profG != NULL) && (ProfInstIsFree(profG) == 0))
	{
	    if(profG->exceptIpGroup[0] != '\0') {
		iptables_fw_add_del_exceptIp(IPTABLE_ADD, profG->exceptIpGroup);
	    }
	}
#endif
#if (WIFIDOG_NOTICE == FYES)
	//wifi_Notice_Handle(IPTABLE_ADD, "http://www.utt.com.cn");
	execute(("ipset -q -N " WIFI_IPSET_NOTICE_GROUP " iphash"), 0);
	execute(("ipset -q -N " WIFI_IPSET_NOTICE_RECORD_GROUP " iphash"), 0);
	wifi_Notice_Handle(IPTABLE_ADD);
#endif
#if (WIFI_RATE_LIMIT == FYES)
	initRateLimit();
#endif
	//ProfDetach();

	free(ext_interface);
#if !defined(WIFI_PRE_REDIRECT)
#if (CONFIG_X86_CPU == 1)
	{
	char uttwdCmd[100];
	memset(uttwdCmd, 0, 100);
	char option[50+1];
	char version[10];
	char key[20];
	memset(option, 0, sizeof(option));
	memset(version, 0, sizeof(version));
	memset(key, 0, sizeof(key));
#if (SERVER_VERSION == FYES)
	strncpy(version, SERV_VERSION, 10);
#else
#endif
#if (WIFIDOG_USER_SECRET_KEY == FYES)
	if((config->secret_key !=NULL) && (strcmp(config->secret_key, SECRET_KEY_VALUE) !=0)) {
	    strncpy(key, config->secret_key, 20);
	}
#else
#endif
	/* 1、uttwd中的版本号默认是没有的， 如果需要版本号则需要配置参数
	 * 2、uttwd中的加密密钥默认是SECRET_KEY_VALUE，是不需要配置参数的；
	 *    如果是用户自己定义的话，则需要配置参数
	 *
	 * */
	if(strlen(version) > 0U) {
	    snprintf(option, sizeof(option), " -ver=%s", version);
	}
	if(strlen(key) > 0U) {
	    snprintf(option, (sizeof(option)-strlen(option)), " -key=%s", key);
	}
	snprintf(uttwdCmd, 100,"uttwd -sn=%u -lan=%s -ios=20 %s &",config->tmpSN,config->gw_address,option);
	debug(LOG_ERR, "option: %s, uttwd cmd: %s", option,uttwdCmd);
	execute(uttwdCmd, 0);
	}
#endif
#endif
	UNLOCK_CONFIG();
	return 1;
}

/** Remove the firewall rules
 * This is used when we do a clean shutdown of WiFiDog and when it starts to make
 * sure there are no rules left over
 */
	int
iptables_fw_destroy(void)
{
	fw_quiet = 1;
#ifdef WIFI_IPSET_LAN
	int i = 0;
#endif

	debug(LOG_DEBUG, "Destroying our iptables entries");

#if (UTT_KNOWIFI == FYES)
	destoryKnowifiIptables();
#endif

	/*
	 *
	 * Everything in the MANGLE table
	 *
	 */
#ifndef WIFI_IPSET_LAN
/*del needless mangle table, because ipset group*/
	debug(LOG_DEBUG, "Destroying chains in the MANGLE table");
#if (WIFIDOG_AUTH == FYES) || (WIFIDOG_MAKE_MONEY != FYES)
	iptables_fw_destroy_mention("mangle", "PREROUTING", TABLE_WIFIDOG_PREROUTING);
	iptables_fw_destroy_mention("mangle", "POSTROUTING", TABLE_WIFIDOG_INCOMING);
	iptables_do_command("-t mangle -F " TABLE_WIFIDOG_TRUSTED);
	iptables_do_command("-t mangle -F " TABLE_WIFIDOG_OUTGOING);
	iptables_do_command("-t mangle -F " TABLE_WIFIDOG_INCOMING);
	iptables_do_command("-t mangle -F " TABLE_WIFIDOG_PREROUTING);
	iptables_do_command("-t mangle -X " TABLE_WIFIDOG_TRUSTED);
	iptables_do_command("-t mangle -X " TABLE_WIFIDOG_OUTGOING);
	iptables_do_command("-t mangle -X " TABLE_WIFIDOG_INCOMING);
	iptables_do_command("-t mangle -X " TABLE_WIFIDOG_PREROUTING);
#else
	iptables_fw_destroy_mention("mangle", "PREROUTING", TABLE_WIFIDOG_TRUSTED);
	iptables_fw_destroy_mention("mangle", "PREROUTING", TABLE_WIFIDOG_OUTGOING);
	iptables_fw_destroy_mention("mangle", "POSTROUTING", TABLE_WIFIDOG_INCOMING);
	iptables_do_command("-t mangle -F " TABLE_WIFIDOG_TRUSTED);
	iptables_do_command("-t mangle -F " TABLE_WIFIDOG_OUTGOING);
	iptables_do_command("-t mangle -F " TABLE_WIFIDOG_INCOMING);
	iptables_do_command("-t mangle -X " TABLE_WIFIDOG_TRUSTED);
	iptables_do_command("-t mangle -X " TABLE_WIFIDOG_OUTGOING);
	iptables_do_command("-t mangle -X " TABLE_WIFIDOG_INCOMING);
#endif

/*end del needless mangle table, because ipset group*/
#endif
#ifdef WIFI_PRE_REDIRECT
	iptables_do_command("-t filter  -F " TABLE_WIFI_PRE_REDIRECT);
	iptables_fw_destroy_mention("filter", "FORWARD", TABLE_WIFI_PRE_REDIRECT);
	iptables_do_command("-t filter  -X " TABLE_WIFI_PRE_REDIRECT);
#endif
#if !defined(WIFI_PRE_REDIRECT)
	/*
	 *
	 * Everything in the NAT table
	 *
	 */
	debug(LOG_DEBUG, "Destroying chains in the NAT table");
	iptables_fw_destroy_mention("nat", "PREROUTING", TABLE_WIFIDOG_OUTGOING);
	iptables_do_command("-t nat -F " TABLE_WIFIDOG_AUTHSERVERS);
	iptables_do_command("-t nat -F " TABLE_WIFIDOG_OUTGOING);
#if 0
/*del needless rules*/
	iptables_do_command("-t nat -F " TABLE_WIFIDOG_WIFI_TO_ROUTER);
#endif
	iptables_do_command("-t nat -F " TABLE_WIFIDOG_WIFI_TO_INTERNET);
#if 0
/*del needless rules*/
	iptables_do_command("-t nat -F " TABLE_WIFIDOG_GLOBAL);
	iptables_do_command("-t nat -F " TABLE_WIFIDOG_UNKNOWN);
/*del needless rules*/
#endif
	iptables_do_command("-t nat -X " TABLE_WIFIDOG_AUTHSERVERS);
	iptables_do_command("-t nat -X " TABLE_WIFIDOG_OUTGOING);
#if 0
/*del needless rules*/
	iptables_do_command("-t nat -X " TABLE_WIFIDOG_WIFI_TO_ROUTER);
#endif
	iptables_do_command("-t nat -X " TABLE_WIFIDOG_WIFI_TO_INTERNET);
#if 0
/*del needless rules*/
	iptables_do_command("-t nat -X " TABLE_WIFIDOG_GLOBAL);
	iptables_do_command("-t nat -X " TABLE_WIFIDOG_UNKNOWN);
/*del needless rules*/
#endif
#endif

#if !defined(WIFI_PRE_REDIRECT)
	/*
	 *
	 * Everything in the FILTER table
	 *
	 */
	debug(LOG_DEBUG, "Destroying chains in the FILTER table");
	iptables_fw_destroy_mention("filter", "FORWARD", TABLE_WIFIDOG_WIFI_TO_INTERNET);
	iptables_do_command("-t filter -F " TABLE_WIFIDOG_WIFI_TO_INTERNET);
	iptables_do_command("-t filter -F " TABLE_WIFIDOG_AUTHSERVERS);
#if 0
/*del needless rules*/
	iptables_do_command("-t filter -F " TABLE_WIFIDOG_LOCKED);
	iptables_do_command("-t filter -F " TABLE_WIFIDOG_GLOBAL);
	iptables_do_command("-t filter -F " TABLE_WIFIDOG_VALIDATE);
/*end del needless rules*/
#endif
#ifndef WIFI_IPSET_LAN
	iptables_do_command("-t filter -F " TABLE_WIFIDOG_KNOWN);
#endif
	iptables_do_command("-t filter -F " TABLE_WIFIDOG_UNKNOWN);
	iptables_do_command("-t filter -X " TABLE_WIFIDOG_WIFI_TO_INTERNET);
	iptables_do_command("-t filter -X " TABLE_WIFIDOG_AUTHSERVERS);
#if 0
/*del needless rules*/
	iptables_do_command("-t filter -X " TABLE_WIFIDOG_LOCKED);
	iptables_do_command("-t filter -X " TABLE_WIFIDOG_GLOBAL);
	iptables_do_command("-t filter -X " TABLE_WIFIDOG_VALIDATE);
/*end del needless rules*/
#endif
#ifndef WIFI_IPSET_LAN
	iptables_do_command("-t filter -X " TABLE_WIFIDOG_KNOWN);
#endif
	iptables_do_command("-t filter -X " TABLE_WIFIDOG_UNKNOWN);
#endif
#ifdef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
	/*wifidog 域名白名单生效规则，匹配域名并把域名ip添加到ipset组中*/
	iptables_do_command("-t filter -D INPUT -p udp --sport 53 -m wifiMatch --add-dns-ip -j RETURN ");
	iptables_do_command("-t filter -D FORWARD -p udp --sport 53 -m wifiMatch --add-dns-ip -j RETURN ");
#endif

#if (SWITCH_AUTH_MODE == FYES)
	execute(("ipset -q -F " IPSET_GROUP_SWITCH_CLIENT_LIST " "), 0);
	execute(("ipset -q -X " IPSET_GROUP_SWITCH_CLIENT_LIST " "), 0);
#endif

#if (WIFIDOG_MAKE_MONEY != FYES)
	execute(("ipset -q -F " IPSET_WIFIDOG_FREE_AUTH " "), 0);
	execute(("ipset -q -X " IPSET_WIFIDOG_FREE_AUTH " "), 0);
#endif
#if (WIFIDOG_MAKE_MONEY != FYES)
#if (WX_DOMAIN_WHITE_LIST_ON == FYES)
	execute(("ipset -q -F " WEIXIN_IPSET_GRP " "), 0);
	execute(("ipset -q -X " WEIXIN_IPSET_GRP " "), 0);
#endif
#endif
#if ((WIFIDOG_AUTH == FYES) || (WIFIDOG_MAKE_MONEY == FYES))
#if ((WHITELIST == 1) || (WIFIDOG_MAKE_MONEY == FYES))
	execute(("ipset -q -F " WHITE_LIST " "), 0);
	execute(("ipset -q -X " WHITE_LIST " "), 0);
#endif
#endif
#ifdef WIFI_IPSET_LAN
	for(i=1;i<5;i++) {
	    wifidogDelgrp(i);
	}

#if (VLAN_CONFIG == FYES)
	wifidogDelVlanGrp();
#endif
#endif
#if (WIFIDOG_NOTICE == FYES)
	wifi_Notice_Handle(IPTABLE_DEL);
	execute(("ipset -q -F " WIFI_IPSET_NOTICE_GROUP " iphash"), 0);
	execute(("ipset -q -X " WIFI_IPSET_NOTICE_RECORD_GROUP " iphash"), 0);
#endif
#if (WIFI_RATE_LIMIT == FYES)
	destoryRateLimit();
#endif
#if !defined(WIFI_PRE_REDIRECT)
#if (CONFIG_X86_CPU == 1)
	execute(("killall uttwd "), 0);
#endif
#endif
	return 1;
}

/*
 * Helper for iptables_fw_destroy
 * @param table The table to search
 * @param chain The chain in that table to search
 * @param mention A word to find and delete in rules in the given table+chain
 */
int
iptables_fw_destroy_mention(
		const char * table,
		const char * chain,
		const char * mention
		) {
	FILE *p = NULL;
	char *command = NULL;
	char *command2 = NULL;
	char line[MAX_BUF];
	char rulenum[10];
	char *victim = safe_strdup(mention);
	int deleted = 0;

	iptables_insert_gateway_id(&victim);

	debug(LOG_DEBUG, "Attempting to destroy all mention of %s from %s.%s", victim, table, chain);

	safe_asprintf(&command, "iptables -t %s -L %s -n --line-numbers -v", table, chain);
	iptables_insert_gateway_id(&command);

	if ((p = popen(command, "r"))) {
		/* Skip first 2 lines */
		while (!feof(p) && fgetc(p) != '\n');
		while (!feof(p) && fgetc(p) != '\n');
		/* Loop over entries */
		while (fgets(line, sizeof(line), p)) {
			/* Look for victim */
			if (strstr(line, victim)) {
				/* Found victim - Get the rule number into rulenum*/
				if (sscanf(line, "%9[0-9]", rulenum) == 1) {
					/* Delete the rule: */
					debug(LOG_DEBUG, "Deleting rule %s from %s.%s because it mentions %s", rulenum, table, chain, victim);
					safe_asprintf(&command2, "-t %s -D %s %s", table, chain, rulenum);
					iptables_do_command(command2);
					free(command2);
					deleted = 1;
					/* Do not keep looping - the captured rulenums will no longer be accurate */
					break;
				}
			}
		}
		pclose(p);
	}

	free(command);
	free(victim);

	if (deleted) {
		/* Recurse just in case there are more in the same table+chain */
		iptables_fw_destroy_mention(table, chain, mention);
	}

	return (deleted);
}

/** Set if a specific client has access through the firewall */
	int
iptables_fw_access(fw_access_t type, const char *ip, const char *mac, int tag)
{
	int rc = 1;

	fw_quiet = 0;

	switch(type) {
		case FW_ACCESS_ALLOW:
#ifdef WIFI_IPSET_LAN
			wifidogAorDIpMac(ip, mac, IPSET_MEM_DEL);
			wifidogAorDIpMac(ip, mac, IPSET_MEM_ADD);
#else
			iptables_do_command("-t mangle -A " TABLE_WIFIDOG_OUTGOING " -s %s -m mac --mac-source %s -j MARK --set-mark %d", ip, mac, tag);
			rc = iptables_do_command("-t mangle -A " TABLE_WIFIDOG_INCOMING " -d %s -j ACCEPT", ip);
#endif
			break;
		case FW_ACCESS_DENY:
#ifdef WIFI_IPSET_LAN
			wifidogAorDIpMac(ip, mac, IPSET_MEM_DEL);
#else
			iptables_do_command("-t mangle -D " TABLE_WIFIDOG_OUTGOING " -s %s -m mac --mac-source %s -j MARK --set-mark %d", ip, mac, tag);
			rc = iptables_do_command("-t mangle -D " TABLE_WIFIDOG_INCOMING " -d %s -j ACCEPT", ip);
#endif
			break;
		default:
			rc = -1;
			break;
	}

	return rc;
}

/** Update the counters of all the clients in the client list */
	int
iptables_fw_counters_update(void)
{
	FILE *output;
	char *script,
	     ip[16],
	     rc;
	unsigned long long int counter;
	t_client *p1;
	struct in_addr tempaddr;

	/* Look for outgoing traffic */
	safe_asprintf(&script, "%s %s", "iptables", "-v -n -x -t mangle -L " TABLE_WIFIDOG_OUTGOING);
	iptables_insert_gateway_id(&script);
	output = popen(script, "r");
	free(script);
	if (!output) {
		debug(LOG_ERR, "popen(): %s", strerror(errno));
		return -1;
	}

	/* skip the first two lines */
	while (('\n' != fgetc(output)) && !feof(output))
		;
	while (('\n' != fgetc(output)) && !feof(output))
		;
	while (output && !(feof(output))) {
		rc = fscanf(output, "%*s %llu %*s %*s %*s %*s %*s %15[0-9.] %*s %*s %*s %*s %*s %*s", &counter, ip);
		//rc = fscanf(output, "%*s %llu %*s %*s %*s %*s %*s %15[0-9.] %*s %*s %*s %*s %*s 0x%*u", &counter, ip);
		if (2 == rc && EOF != rc) {
			/* Sanity*/
			if (!inet_aton(ip, &tempaddr)) {
				debug(LOG_WARNING, "I was supposed to read an IP address but instead got [%s] - ignoring it", ip);
				continue;
			}
			debug(LOG_DEBUG, "Read outgoing traffic for %s: Bytes=%llu", ip, counter);
			LOCK_CLIENT_LIST();
			if ((p1 = client_list_find_by_ip(ip))) {
				if ((p1->counters.outgoing - p1->counters.outgoing_history) < counter) {
					p1->counters.outgoing = p1->counters.outgoing_history + counter;
					p1->counters.last_updated = time(NULL);
					debug(LOG_DEBUG, "%s - Updated counter.outgoing to %llu bytes.  Updated last_updated to %d", ip, counter, p1->counters.last_updated);
				}
			} else {
				debug(LOG_ERR, "Could not find %s in client list", ip);
			}
			UNLOCK_CLIENT_LIST();
		}
	}
	pclose(output);

	/* Look for incoming traffic */
	safe_asprintf(&script, "%s %s", "iptables", "-v -n -x -t mangle -L " TABLE_WIFIDOG_INCOMING);
	iptables_insert_gateway_id(&script);
	output = popen(script, "r");
	free(script);
	if (!output) {
		debug(LOG_ERR, "popen(): %s", strerror(errno));
		return -1;
	}

	/* skip the first two lines */
	while (('\n' != fgetc(output)) && !feof(output))
		;
	while (('\n' != fgetc(output)) && !feof(output))
		;
	while (output && !(feof(output))) {
		rc = fscanf(output, "%*s %llu %*s %*s %*s %*s %*s %*s %15[0-9.]", &counter, ip);
		if (2 == rc && EOF != rc) {
			/* Sanity*/
			if (!inet_aton(ip, &tempaddr)) {
				debug(LOG_WARNING, "I was supposed to read an IP address but instead got [%s] - ignoring it", ip);
				continue;
			}
			debug(LOG_DEBUG, "Read incoming traffic for %s: Bytes=%llu", ip, counter);
			LOCK_CLIENT_LIST();
			if ((p1 = client_list_find_by_ip(ip))) {
				if ((p1->counters.incoming - p1->counters.incoming_history) < counter) {
					p1->counters.incoming = p1->counters.incoming_history + counter;
					debug(LOG_DEBUG, "%s - Updated counter.incoming to %llu bytes", ip, counter);
				}
			} else {
				debug(LOG_ERR, "Could not find %s in client list", ip);
			}
			UNLOCK_CLIENT_LIST();
		}
	}
	pclose(output);

	return 1;
}

#if 0
int iptables_fw_add_del(fw_op_type_t opt)
{
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;
    int add = 1;

    if(opt == IPTABLE_ADD) {
	//ProfInit();
	if(free_auth_add == 0) {
	/*PREROUTING*/
	debug(LOG_DEBUG, "================================================ ");
	debug(LOG_DEBUG, "add firewall iptables rule for wifidog free auth ");
	syslog(LOG_WARNING, "Auth Server reachable: No; open free utt wifi auth.");
	profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
	/*web认证不开启不会用到计费模式返回0*/
	if((profG != NULL) && (ProfInstIsFree(profG) == 0) && (profG->enabled == 1))
	{
	    if(profG->exceptIpGroup[0] != '\0') {
		add = 2;
	    }
	}
	iptables_do_command("-t mangle -I " TABLE_WIFIDOG_PREROUTING " %d -j RETURN",add);
	iptables_do_command("-t nat -I " TABLE_WIFIDOG_OUTGOING " %d -j RETURN",add);
	/*POSTROUTING*/
	iptables_do_command("-t mangle -I " TABLE_WIFIDOG_INCOMING " %d -j RETURN",add);
	/*FORWARD*/
	iptables_do_command("-t filter -I " TABLE_WIFIDOG_WIFI_TO_INTERNET " %d -j RETURN",add);
	free_auth_add = 1;
	}
	//ProfDetach();
    } else if(opt == IPTABLE_DEL) {
	if(free_auth_add == 1) {
	debug(LOG_DEBUG, "================================================ ");
	debug(LOG_DEBUG, "del firewall iptables rule for wifidog free auth ");
	syslog(LOG_WARNING, "Auth Server reachable: Yes; close free utt wifi auth.");
	/*PREROUTING*/
	iptables_do_command("-t mangle -D " TABLE_WIFIDOG_PREROUTING " -j RETURN");
	iptables_do_command("-t nat -D " TABLE_WIFIDOG_OUTGOING " -j RETURN");
	/*POSTROUTING*/
	iptables_do_command("-t mangle -D " TABLE_WIFIDOG_INCOMING " -j RETURN");
	/*FORWARD*/
	iptables_do_command("-t filter -D " TABLE_WIFIDOG_WIFI_TO_INTERNET " -j RETURN");
	free_auth_add = 0;
	}
    } else {
	fprintf(stderr,"operate type error.\n");   
    }
    return 1;
}
#endif

int iptables_fw_add_del_exceptIp(fw_op_type_t opt, char *exceptIpGrp)
{

    if((opt == IPTABLE_ADD) && (exceptIpGrp != NULL)) {
	/*PREROUTING*/
	debug(LOG_DEBUG, "================================================ ");
	debug(LOG_DEBUG, "add firewall iptables rule for wifidog except ip ");
#ifndef WIFI_IPSET_LAN
/*del needless mangle table, because ipset group*/
	iptables_do_command("-t mangle -I " TABLE_WIFIDOG_PREROUTING " 1 -m set --set %s src -j RETURN", exceptIpGrp);
	/*POSTROUTING*/
	iptables_do_command("-t mangle -I " TABLE_WIFIDOG_INCOMING " 1 -m set --set %s dst -j RETURN", exceptIpGrp);
/*end del needless mangle table, because ipset group*/
#endif
#ifdef WIFI_PRE_REDIRECT
	iptables_do_command("-t filter -I " TABLE_WIFI_PRE_REDIRECT " 1 -m set --set %s src -j RETURN", exceptIpGrp);
#else
	iptables_do_command("-t nat -I " TABLE_WIFIDOG_OUTGOING " 1 -m set --set %s src -j RETURN", exceptIpGrp);
	/*FORWARD*/
	iptables_do_command("-t filter -I " TABLE_WIFIDOG_WIFI_TO_INTERNET " 1 -m set --set %s src -j RETURN", exceptIpGrp);
#endif
    } else if((opt == IPTABLE_DEL) && (exceptIpGrp != NULL)) {
	debug(LOG_DEBUG, "================================================ ");
	debug(LOG_DEBUG, "del firewall iptables rule for wifidog free except ip ");
	/*PREROUTING*/
#ifndef WIFI_IPSET_LAN
/*del needless mangle table, because ipset group*/
	iptables_do_command("-t mangle -D " TABLE_WIFIDOG_PREROUTING " -m set --set %s src -j RETURN", exceptIpGrp);
	/*POSTROUTING*/
	iptables_do_command("-t mangle -D " TABLE_WIFIDOG_INCOMING " -m set --set %s dst -j RETURN", exceptIpGrp);
/*end del needless mangle table, because ipset group*/
#endif
#ifdef WIFI_PRE_REDIRECT
	iptables_do_command("-t filter -D " TABLE_WIFI_PRE_REDIRECT " -m set --set %s src -j RETURN", exceptIpGrp);
#else
	iptables_do_command("-t nat -D " TABLE_WIFIDOG_OUTGOING " -m set --set %s src -j RETURN", exceptIpGrp);
	/*FORWARD*/
	iptables_do_command("-t filter -D " TABLE_WIFIDOG_WIFI_TO_INTERNET " -m set --set %s src -j RETURN", exceptIpGrp);
#endif
    } else {
	fprintf(stderr,"operate type error.\n");   
    }
    return 1;
}

static int add_flag = 0;
static void getDomainNameIP(char *domain,char *ipset_grp)
{
    struct hostent *host;
    struct in_addr inaddr;
    char **ptr = NULL;
    char ip[20];

    if((domain != NULL) && (strlen(domain) > 0)) {
    host = gethostbyname(domain);
    //inaddr.s_addr=*(unsigned long *)host->h_addr;
    if(host!=NULL)
    {
	for(ptr=host->h_addr_list;*ptr != NULL;ptr++) {
	memcpy((char *) &inaddr, *ptr,(size_t) host->h_length);
	memset(ip, 0, sizeof(ip));
	strcpy(ip,inet_ntoa(inaddr));
	ipsetAOrDIpStr(ipset_grp, ip, IPSET_MEM_ADD);                                   
	debug(LOG_DEBUG,"%s-%d: hostname=%s,ip[0]=%s,ip=%s\n",__func__,__LINE__,host->h_name,inet_ntoa(inaddr),ip);
	}
    }
    }
    return;
}
#if (WIFIDOG_MAKE_MONEY != FYES)
void iptables_add_weixin_ipset()
{
    if(add_flag == 0) 
    {
	/*
	 * 还需要定时更新域名ip
	 * 在uttTool/uttUpDomain.c中实现
	 * 若果修改此处，则也需要修改uttTool/uttUpDomain.c
	 * */
#if (WX_DOMAIN_WHITE_LIST_ON == FYES)
	ipsetFlushGrp(WEIXIN_IPSET_GRP);
	getDomainNameIP(WEIXIN_DOMAIN_SHORT,WEIXIN_IPSET_GRP);
	getDomainNameIP(WEIXIN_DOMAIN_LONG,WEIXIN_IPSET_GRP);
	getDomainNameIP(WEIXIN_DOMAIN_SZLONG,WEIXIN_IPSET_GRP);
	getDomainNameIP(WEIXIN_DOMAIN_SZSHORT,WEIXIN_IPSET_GRP);
	WechatDomainAdd();
#endif
#ifndef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
	/*移到内核实现，通过iptables的WiFiMatch匹配模块实现*/
	#if(WIFIDOG_AUTH == FYES)
	#if(WHITELIST ==1)
	ipsetFlushGrp(WHITE_LIST);
	whiteList();
	#endif
	#endif
#endif
	add_flag = 1;
    }
    
}
#endif
void
iptables_del_ifOnline()
{
	/* 开机默认数据包不会重定向到认证服务器
	 * 检测外网连接后，删除此规则
	 * */
    if(del_cnt > 0) {
	iptables_do_command("-t nat -D " TABLE_WIFIDOG_WIFI_TO_INTERNET " -j RETURN ");
	del_cnt--;
    }
	return;
}

#ifdef WIFI_IPSET_LAN
void
wifidogAddgrp(int index, unsigned int ip, unsigned int cidr)
{
    struct in_addr addrS;
    char *ipStr = NULL;
    char cmd[100];

    memset(cmd, 0, 100);
    memset(&addrS, 0, sizeof(addrS));
    addrS.s_addr = ip;
    ipStr = inet_ntoa(addrS);
    if(ipStr != NULL) {
#if (WIFIDOG_MAC_AUTH == FYES)
	snprintf(cmd, 100,"ipset -q -N %s%d macipmap --network %s/%d ",WIFI_GROUP_BLACK_MAC,index,ipStr,cidr);
	execute(cmd, 0);
	memset(cmd, 0, 100);
#endif
	snprintf(cmd, 100,"ipset -q -N %s%d macipmap --network %s/%d",WIFI_GROUP_PREFIX,index,ipStr,cidr);
	debug(LOG_DEBUG, "cmd: %s =====ipset add group",cmd);
	execute(cmd, 0);
#if (UTT_KNOWIFI == FYES)
	memset(cmd, 0, 100);
	snprintf(cmd, 100,"ipset -q -N %s macipmap --network %s/%d",KNOWIFI_REDIRECT_CLIENT,ipStr,cidr);
	execute(cmd, 0);
#endif
    }
}
#if (VLAN_CONFIG == FYES)
void wifidogDelVlanGrp(){
	int min=0, max=0, index = 0;
	MibProfileType mibType = MIB_PROF_VLAN_CONFIG;
	VlanConfigProfile *prof = NULL;
	unsigned int vlan_ip = 0U, vlan_net = 0U;
	
	ProfInstLowHigh(mibType, &max, &min);

	for (index = min; index < max; index++) {
		prof = (VlanConfigProfile *)ProfGetInstPointByIndex(mibType, index);
		if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)) && (TRUE == prof->head.active)) {
			wifidogDelgrp(((prof->vid)+4));
			wifidogAddOrDelVlanRoute(prof->vid, ((prof->ip) & (prof->netmask)),headbits(htonl(prof->netmask))%100,0);
		}
	}
}
static void wifidogAddOrDelVlanRoute(int vid ,int vlan_net,int cidr,int action){
    struct in_addr addrS;
    char *ipStr = NULL;
    char cmd[100];

    memset(cmd, 0, 100);
    memset(&addrS, 0, sizeof(addrS));
    addrS.s_addr = vlan_net;
    ipStr = inet_ntoa(addrS);

    if(action == 1){
	snprintf(cmd,100,"ip route add table 1 %s/%d dev %s.%d proto kernel  scope link",ipStr,cidr,getLanIfName(),vid);
    }else{
	snprintf(cmd,100,"ip route del table 1 %s/%d dev %s.%d proto kernel  scope link",ipStr,cidr,getLanIfName(),vid);
    }
    debug(LOG_DEBUG, "cmd: %s =====ipset add group",cmd);
    execute(cmd, 0);
}
#endif
void
wifidogDelgrp(int index)
{
    char cmd[100];
#if (WIFIDOG_MAC_AUTH == FYES)
    memset(cmd, 0, 100);
    snprintf(cmd, 100, "ipset -q -F %s%d ",WIFI_GROUP_BLACK_MAC,index);
    execute(cmd, 0);
#endif
    snprintf(cmd, 100, "ipset -q -F %s%d ",WIFI_GROUP_PREFIX,index);
    execute(cmd, 0);
#if (WIFIDOG_MAC_AUTH == FYES)
    memset(cmd, 0, 100);
    snprintf(cmd, 100,"ipset -q -X %s%d ",WIFI_GROUP_BLACK_MAC,index);
    execute(cmd, 0);
#endif
    memset(cmd, 0, 100);
    snprintf(cmd, 100,"ipset -q -X %s%d ",WIFI_GROUP_PREFIX,index);
    execute(cmd, 0);
#if (UTT_KNOWIFI == FYES)
    memset(cmd, 0, 100);
    snprintf(cmd, 100,"ipset -q -F %s ",KNOWIFI_REDIRECT_CLIENT);
    execute(cmd, 0);
    memset(cmd, 0, 100);
    snprintf(cmd, 100,"ipset -q -X %s ",KNOWIFI_REDIRECT_CLIENT);
    execute(cmd, 0);
#endif
}

/*
 * 匹配vlan子接口网段
 * 匹配则返回ipset组的index索引值
 * */
int matchVlanNet(unsigned int client)
{
    int ret = 0;
#if (VLAN_CONFIG == FYES)
	int min=0, max=0, index = 0;
	MibProfileType mibType = MIB_PROF_VLAN_CONFIG;
	VlanConfigProfile *prof = NULL;
	unsigned int vlan_ip = 0U, vlan_net = 0U;
	
	ProfInstLowHigh(mibType, &max, &min);

	for (index = min; index < max; index++) {
		prof = (VlanConfigProfile *)ProfGetInstPointByIndex(mibType, index);
		if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)) && (TRUE == prof->head.active)) {
		    vlan_ip = htonl(prof->ip);
		    vlan_net = (prof->ip) & (prof->netmask);

		    if((vlan_ip != 0U)&&((htonl(client) & htonl(prof->netmask)) == htonl(vlan_net))) {
			ret = (prof->vid)+4;
			break;
		    }
		    vlan_ip = 0U;
		    vlan_net = 0U;
		}
	}
#endif
    return ret;
}

void
wifidogAorDIpMac(const char *ipStr, const char *mac, char addorDel)
{
    struct in_addr addrS;
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;

    unsigned int iIp = 0U, iIp2 = 0U, iIp3 = 0U, iIp4 = 0U;
    unsigned int iIpStart = 0U, iIpStart2 = 0U, iIpStart3 = 0U, iIpStart4 = 0U;
    unsigned int clientip = 0U;
    int index = 0;
    char cmd[100];

#if(WIFIDOG_MAC_AUTH == FYES)
    t_client    *client =  client_list_find(ipStr, mac);
#endif
    memset(&addrS, 0, sizeof(addrS));
    memset(cmd, 0, 100);
    //ProfInit();
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
    if ((profIF != NULL) && (profIF->head.active == 1))
    {/*判断lan口profile是否正常*/
	iIp = htonl(profIF->ether.sta.ip);
	iIpStart = profIF->ether.sta.ip & profIF->ether.sta.nm;
#if (MULTI_LAN == FYES)
	iIp2 = htonl(profIF->ether.sta.ip2);
	iIp3 = htonl(profIF->ether.sta.ip3);
	iIp4 = htonl(profIF->ether.sta.ip4);
	iIpStart2 = profIF->ether.sta.ip2 & profIF->ether.sta.nm2;
	iIpStart3 = profIF->ether.sta.ip3 & profIF->ether.sta.nm3;
	iIpStart4 = profIF->ether.sta.ip4 & profIF->ether.sta.nm4;
#endif

    }
    debug(LOG_DEBUG,"iIp: 0x%x,iIpStart: 0x%x,mask;0x%x,htonl(mask): 0x%x",iIp,iIpStart,profIF->ether.sta.nm,htonl(profIF->ether.sta.nm));
    if(inet_aton(ipStr, &addrS)!=0) {
	clientip = addrS.s_addr;
	debug(LOG_DEBUG, "ipStr: %s, htonl(clientip): 0x%x,clientip&mask: 0x%x,lanip: 0x%x =====ipset ",ipStr,htonl(clientip),htonl(clientip) & htonl(profIF->ether.sta.nm),htonl(iIpStart));
	if((iIp != 0U)&&(htonl(clientip) & htonl(profIF->ether.sta.nm)) == htonl(iIpStart)) {
	    index = 1;
#if (MULTI_LAN == FYES)
	} else if((iIp2 != 0U)&&(htonl(clientip) & htonl(profIF->ether.sta.nm2)) == htonl(iIpStart2)) {
	    index = 2;
	} else if((iIp3 != 0U)&&(htonl(clientip) & htonl(profIF->ether.sta.nm3)) == htonl(iIpStart3)) {
	    index = 3;
	} else if((iIp4 != 0U)&&(htonl(clientip) & htonl(profIF->ether.sta.nm4)) == htonl(iIpStart4)) {
	    index = 4;
#endif
	}
	else { /*匹配VLAN子接口网段*/
	    index = matchVlanNet(clientip);
	}
    
	if(index > 0) {
	    if(addorDel == IPSET_MEM_ADD) {
		if(mac != NULL) {
#if (WIFIDOG_MAC_AUTH == FYES)
		    if(client != NULL && client->authType == WIFIDOG_CLIENT_AUTH_TYPE_MAC_BLACK){
			snprintf(cmd, 100,"ipset -%c %s%d %s,%s ",addorDel,WIFI_GROUP_BLACK_MAC,index,ipStr,mac);
			debug(LOG_DEBUG, "cmd: %s =====ipset add ip mac",cmd);
			execute(cmd, 0);
		    }else
#endif
		    {
			snprintf(cmd, 100,"ipset -%c %s%d %s,%s ",addorDel,WIFI_GROUP_PREFIX,index,ipStr,mac);
			debug(LOG_DEBUG, "cmd: %s =====ipset add ip mac",cmd);
			execute(cmd, 0);
		    }
		}
	    } else if(addorDel == IPSET_MEM_DEL) {
#if (WIFIDOG_MAC_AUTH == FYES)
		if(client != NULL && client->authType == WIFIDOG_CLIENT_AUTH_TYPE_MAC_BLACK){
		    snprintf(cmd, 100,"ipset -%c %s%d %s ",addorDel,WIFI_GROUP_BLACK_MAC,index,ipStr);
		    debug(LOG_DEBUG, "cmd: %s =====ipset del ip mac",cmd);
		    execute(cmd, 0);
		}else 
#endif
		{
		    snprintf(cmd, 100,"ipset -%c %s%d %s ",addorDel,WIFI_GROUP_PREFIX,index,ipStr);
		    debug(LOG_DEBUG, "cmd: %s =====ipset del ip mac",cmd);
		    execute(cmd, 0);
		}
	    }
	}
    }
    //ProfDetach();
}
void 
initLanIpset()
{
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;

    unsigned int iIp = 0U, iIp2 = 0U, iIp3 = 0U, iIp4 = 0U;
    unsigned int iIpStart = 0U, iIpStart2 = 0U, iIpStart3 = 0U, iIpStart4 = 0U;

    memset(lan_flag, 0, sizeof(lan_flag));
    //ProfInit();
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
    if ((profIF != NULL) && (profIF->head.active == 1))
    {/*判断lan口profile是否正常*/
	iIp = htonl(profIF->ether.sta.ip);
	iIpStart = profIF->ether.sta.ip & profIF->ether.sta.nm;
#if (MULTI_LAN == FYES)
	iIp2 = htonl(profIF->ether.sta.ip2);
	iIp3 = htonl(profIF->ether.sta.ip3);
	iIp4 = htonl(profIF->ether.sta.ip4);
	iIpStart2 = profIF->ether.sta.ip2 & profIF->ether.sta.nm2;
	iIpStart3 = profIF->ether.sta.ip3 & profIF->ether.sta.nm3;
	iIpStart4 = profIF->ether.sta.ip4 & profIF->ether.sta.nm4;
#endif
	
	if(iIp != 0U)
	{/*接口启用了*/
	    wifidogAddgrp(1, iIpStart, headbits(htonl(profIF->ether.sta.nm))%100);
	    lan_flag[0] = 1;
	}
#if (MULTI_LAN == FYES)
	if(iIp2 != 0U)
	{
	    wifidogAddgrp(2, iIpStart2, headbits(htonl(profIF->ether.sta.nm2))%100);
	    lan_flag[1] = 2;
	}
	if(iIp3 != 0U)
	{
	    wifidogAddgrp(3, iIpStart3, headbits(htonl(profIF->ether.sta.nm3))%100);
	    lan_flag[2] = 3;
	}
	if(iIp4 != 0U)
	{
	    wifidogAddgrp(4, iIpStart4, headbits(htonl(profIF->ether.sta.nm4))%100);
	    lan_flag[3] = 4;
	}
#endif
    }
#if (VLAN_CONFIG == FYES)
	int min=0, max=0, index = 0;
	MibProfileType mibType = MIB_PROF_VLAN_CONFIG;
	VlanConfigProfile *prof = NULL;
	unsigned int vlan_ip = 0U, vlan_net = 0U;
	
	ProfInstLowHigh(mibType, &max, &min);

	for (index = min; index < max; index++) {
		prof = (VlanConfigProfile *)ProfGetInstPointByIndex(mibType, index);
		if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)) && (TRUE == prof->head.active)) {
		    vlan_ip = htonl(prof->ip);
		    vlan_net = (prof->ip) & (prof->netmask);
		    if(vlan_ip != 0U) {
			wifidogAddgrp(((prof->vid)+4), vlan_net, headbits(htonl(prof->netmask))%100);
			wifidogAddOrDelVlanRoute(prof->vid, vlan_net,headbits(htonl(prof->netmask))%100,1);
			lan_flag[4+index] =(prof->vid)+4;
		    }
		    vlan_ip = 0U;
		    vlan_net = 0U;
		}
	}
#endif
    //ProfDetach();
}
#ifndef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
/*移到内核实现，通过iptables的WiFiMatch匹配模块实现*/
#if(WIFIDOG_AUTH == FYES)
#if (WHITELIST == 1)
static void whiteList()
{   
    int min, max, index;
    MibProfileType mibType = MIB_PROF_WHITE_LIST;
    WhiteListProfile *prof = NULL;

    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
	prof = (WhiteListProfile *)ProfGetInstPointByIndex(mibType, index);
	debug(LOG_DEBUG,"%s-%d: hostname=%s\n",__func__,__LINE__,(prof->domain));
	if((prof->domain != NULL) && (strlen(prof->domain) > 0U)) 
	{
	    getDomainNameIP(prof->domain,WHITE_LIST);			/*加入 WHITE_LIST*/ 
	}
	debug(LOG_DEBUG,"%s-%d: hostname=%s\n",__func__,__LINE__,(prof->domain));
    }
}
#endif
#endif
#endif
#if (WIFIDOG_NOTICE == FYES)
/*
 * wifi_Notice_Rule_Add 
 * wifidog 到期通告推送规则添加
 * guo.deyuan
 *
 * */
static void wifi_Notice_Rule_Add(const char *url_r) {
    //char url_r[50];
    if((url_r != NULL)&&(strlen(url_r) > 0U)) {
	char lanip[16] = {0};
	/* 获得lan口IP */
	getIfIp((char *)getLanIfName(), lanip);

	/*
	 * 新建连个通告链
	 * 分别在PREROUTING和POSTROUTING中引用
	 * */
#if 0
	if(webauthWebGrpAdd(WIFI_IPSET_NOTICE_GROUP) != 0) {
	    debug(LOG_ERR,"[NOTICE] create notice group %s error.",WIFI_IPSET_NOTICE_GROUP);
	}
	if(webauthWebGrpAdd(WIFI_IPSET_NOTICE_RECORD_GROUP) != 0) {
	    debug(LOG_ERR,"[NOTICE] create notice record group %s error.",WIFI_IPSET_NOTICE_RECORD_GROUP);
	}
#endif
	iptables_do_command("-t mangle -N %s ", WIFI_NOTICE_PRE_CHAIN);
	iptables_do_command("-t mangle -N %s ", WIFI_NOTICR_REDIRECT_CHAIN);
	iptables_do_command("-t mangle -A "WIFI_NOTICE_PRE_CHAIN" -m set --set "WIFI_IPSET_NOTICE_GROUP" src -m layer7 --l7proto httpGet -j SET --add-set "WIFI_IPSET_NOTICE_RECORD_GROUP" src,src,dst ");
	if(strncmp(url_r, "http://", 7) == 0) {
	    iptables_do_command("-t mangle -A "WIFI_NOTICR_REDIRECT_CHAIN" -m set --set "WIFI_IPSET_NOTICE_RECORD_GROUP" dst,dst,src -j HTTPREDIRECT --redirect-dir %s?url=%s --return c ", WIFI_NOTICE_HTML,url_r);
	} else {
	    iptables_do_command("-t mangle -A "WIFI_NOTICR_REDIRECT_CHAIN" -m set --set "WIFI_IPSET_NOTICE_RECORD_GROUP" dst,dst,src -j HTTPREDIRECT --redirect-dir %s?url=http://%s --return c ", WIFI_NOTICE_HTML,url_r);
	}
	/*
	 * PREROUTING 链中添加通告推送规则
	 * 记录需通告的客户端ip
	 * */
#if (MULTI_LAN == FYES)
	/*通告初始化*/
#if (IP_GRP == FYES)
	iptables_do_command("-t mangle -A PREROUTING -p tcp --dport 80 -m set ! --set %s dst -j %s ", SYS_LAN_GROUP, WIFI_NOTICE_PRE_CHAIN);
#else
#error ipset has been removed
#endif
#else
	iptables_do_command("-t mangle -A PREROUTING -p tcp --dport 80 ! -d %s -j %s ", lanip, WIFI_NOTICE_PRE_CHAIN);
#endif
	/*
	 * POSTROUTING 链中添加通告推送规则
	 * 推送通告url
	 * */
	iptables_do_command("-t mangle -A POSTROUTING -p tcp --sport 80 -m set --set %s dst -j %s ", WIFI_IPSET_NOTICE_RECORD_GROUP, WIFI_NOTICR_REDIRECT_CHAIN);
    }
}
/*
 * wifi_Notice_Rule_Del 
 * wifidog 到期通告推送规则删除
 * guo.deyuan
 *
 * */
static void wifi_Notice_Rule_Del() {
    /*
     * 删除链中规则
     * */
#if (MULTI_LAN == FYES)
    /*通告规则删除*/
#if (IP_GRP == FYES)
    iptables_do_command("-t mangle -D PREROUTING -p tcp --dport 80 -m set ! --set %s dst -j %s 1>/dev/null 2>&1", SYS_LAN_GROUP, WIFI_NOTICE_PRE_CHAIN);
#else
#error ipset has been removed
#endif
#else
    /*通告规则删除*/
    iptables_do_command("-t mangle -D PREROUTING -p tcp --dport 80 ! -d %s -j %s 1>/dev/null 2>&1", lanip, WIFI_NOTICE_PRE_CHAIN);
#endif
    iptables_do_command("-t mangle -D POSTROUTING -p tcp --sport 80 -m set --set %s dst -j %s 1>/dev/null 2>&1", WIFI_IPSET_NOTICE_RECORD_GROUP, WIFI_NOTICR_REDIRECT_CHAIN);
    /*
     * 规则清空删除
     * */
    iptables_do_command("-t mangle -F %s ", WIFI_NOTICE_PRE_CHAIN);
    iptables_do_command("-t mangle -F %s ", WIFI_NOTICR_REDIRECT_CHAIN);
    iptables_do_command("-t mangle -X %s ", WIFI_NOTICE_PRE_CHAIN);
    iptables_do_command("-t mangle -X %s ", WIFI_NOTICR_REDIRECT_CHAIN);
}

int checkUrlValid(char *url) {
    int ret = 1;
    if((url!=NULL) && (strlen(url)>0)) {
	if(strncmp(url, "http://", 7) != 0) {
	    
	}
    } else {
	ret = 0;
    }
    return ret;
}
/*
 * getNoticeTime
 * 获取到期推送时间
 * */
int getNoticeTime(unsigned int *time)
{
	const s_config *config;
	config = config_get_config();
    
	/*返回时间单位为: 分钟*/
	*time = config->noticeTime;
	return (config->noticeTime);
}

/*
 * getNoticeUrl
 * 获取到期推送URL
 * */
char * getNoticeUrl(char *url)
{
	const s_config *config;
	config = config_get_config();
    
	/*返回时间单位为: 分钟*/
	strncpy(url, config->noticeUrl,MAX_URL_LEN);
	return (config->noticeUrl);
}
/*
 * wifi_Notice_Handle()
 * 到期通告规则操作函数
 * 参数；
 * operate
 *	    IPTABLE_ADD - 添加规则
 *	    IPTABLE_DEL - 删除规则
 * url
 *	    通告URL,如果是删除规则，url可以为NULL.
 * guo.deyuan
 * */
extern void wifi_Notice_Handle(fw_op_type_t operate) {

    unsigned int noticeTime = 0U;	/*到期推送时间*/
    char noticeurl[MAX_URL_LEN+1];

    if(operate == IPTABLE_ADD) {
	getNoticeTime(&noticeTime);
	memset(noticeurl, 0, sizeof(noticeurl));
	getNoticeUrl(noticeurl);
	/*
	 * 如果开启了到期推送，
	 * 且推送url是有效值，才添加通告规则
	 * */
	if((noticeTime > 0U) && (strlen(noticeurl) > 0)) {
	    wifi_Notice_Rule_Add(noticeurl);
	} else {
	    debug(LOG_ERR,"[NOTICE] wifi auth notice rules error, expireSec: %d, notice url: %s.",noticeTime,noticeurl);
	}
    } else if (operate == IPTABLE_DEL) {
	wifi_Notice_Rule_Del();
    }

}
#endif /*WIFIDOG_NOTICE*/
#if (WIFI_RATE_LIMIT == FYES)
/**/
void Add_ip_ipset(int index, char *ip)
{
    /*ip添加到ipset组index中*/
}
/*
 * isGroupIndexValid()
 * 检测数组下标是否越界
 * */
int isGroupIndexValid(index) {
    int ret = 0;
    if((index >=0) && (index < MAX_WIFIDOG_RATE_LIMIT_GROUP)) {
	ret = 1;
    }
    return ret;
}
/*
 * RateLimitRuleAddOrDel()
 * 添加或者删除限速规则
 * 参数：
 *  index   - 限速组下标
 *  up	    - 上行限速速率 单位kbit/s
 *  down    - 下行限速速率 单位kbit/s
 *  opt	    - 操作类型
 *	    IPTABLE_ADD
 *	    IPTABLE_DEL
 *
 * */
int RateLimitRuleAddOrDel(int index, int up, int down, fw_op_type_t opt) {
    int ret = 1;
    char action='\0';
    char upstr[32];
    char downstr[32];

    if(opt == IPTABLE_ADD) {
	action = 'A';
#ifdef WIFI_PRE_REDIRECT
	iptables_do_command("-t filter -I  %s 6  -m set --set %s src -j RETURN ",TABLE_WIFI_PRE_REDIRECT,rateLimitGroup[index].name);
#else
	iptables_do_command("-t nat -I " TABLE_WIFIDOG_WIFI_TO_INTERNET " -m set --set %s src -j RETURN",rateLimitGroup[index].name);
	iptables_do_command("-t filter -I  %s -m set --set %s src -j RETURN ",TABLE_WIFIDOG_WIFI_TO_INTERNET,rateLimitGroup[index].name);
#endif
    } else if(opt == IPTABLE_DEL) {
	action = 'D';
#ifdef WIFI_PRE_REDIRECT
	iptables_do_command(" -t filter -D %s  -m set --set %s src  -j RETURN ",TABLE_WIFI_PRE_REDIRECT,rateLimitGroup[index].name);
#else
	iptables_do_command("-t nat -D " TABLE_WIFIDOG_WIFI_TO_INTERNET " -m set --set %s src -j RETURN",rateLimitGroup[index].name);
	iptables_do_command(" -t filter -D %s  -m set --set %s src  -j RETURN ",TABLE_WIFIDOG_WIFI_TO_INTERNET,rateLimitGroup[index].name);
#endif
    }
    memset(upstr, 0, sizeof(upstr));
    memset(downstr, 0, sizeof(downstr));
    if(up > 0) {
	sprintf(upstr , "%.0u", up*1024U/8U);
	iptables_do_command("-t mangle -%c "WIFI_RATE_LIMIT_CHAIN" -m uttdev --lan-to-wan -m set --set %s src -m hashlimit --hashlimit-name wifi%dsrc --hashlimit %s/sec --hashlimit-burst %s --hashlimit-mode srcip -j ACCEPT", action, rateLimitGroup[index].name, index, upstr, upstr);
	iptables_do_command("-t mangle -%c "WIFI_RATE_LIMIT_CHAIN" -m uttdev --lan-to-wan -m set --set %s src -j DROP",action,rateLimitGroup[index].name);
    }
    if(down > 0) {
	sprintf(downstr , "%.0u", down*1024U/8U);
	iptables_do_command("-t mangle -%c "WIFI_RATE_LIMIT_CHAIN" -m uttdev --wan-to-lan -m set --set %s dst -m hashlimit --hashlimit-name wifi%ddst --hashlimit %s/sec --hashlimit-burst %s --hashlimit-mode dstip -j ACCEPT",action,rateLimitGroup[index].name, index, downstr, downstr);
	iptables_do_command("-t mangle -%c "WIFI_RATE_LIMIT_CHAIN" -m uttdev --wan-to-lan -m set --set %s dst -j DROP",action,rateLimitGroup[index].name);
    }

    return ret;
}

/*
 * RateLimitGroupSortAndInsert
 * 对所有的限速组根据下行速率按照从小到大排序
 * 排序结果保存到限速结构体的sort变量中
 *
 * guo.deyuan
 * */
int RateLimitGroupSortAndInsert(int up, int down)
{
    int i = 0;
    int little = -1;
    int sort = 0;
    int index = -1;

    /*每次添加时都重新排序*/
    for(i=0;i<MAX_WIFIDOG_RATE_LIMIT_GROUP;i++) {
	rateLimitGroup[i].sort = -1;
    }

    /* 对所有的限速组根据下行限速速率，按照从小到大排序，及sort值从0到(MAX_WIFIDOG_RATE_LIMIT_GROUP-1)
     * 将需要添加的限速值按就近原则添加到限速组中
     * */
    while(sort < MAX_WIFIDOG_RATE_LIMIT_GROUP) {
	little = -1;
	for(i=0;i<MAX_WIFIDOG_RATE_LIMIT_GROUP;i++) {
	    if((little == -1) && (rateLimitGroup[i].mark == 1) &&
		    ((rateLimitGroup[i].sort < 0) || (rateLimitGroup[i].sort >= MAX_WIFIDOG_RATE_LIMIT_GROUP))) {
		little = i;
		continue;
	    }
	    if((rateLimitGroup[i].mark == 1) && (little != i) && (isGroupIndexValid(rateLimitGroup[i].sort) != 1)) {
		if(rateLimitGroup[little].downRate > rateLimitGroup[i].downRate) {
		    little = i;
		}
	    }
	}
	debug(LOG_DEBUG,"%s-%d: little %d, sort %d",__func__,__LINE__,little,sort);
	rateLimitGroup[little].sort = sort;
	if(down < rateLimitGroup[little].downRate) {
	    index = little;
	    break;
	}
	sort++;

    }
    /*以上没有匹配，说明需要添加的下行限速是最大的
     * 添加到最大下行限速组中*/
    if(index == -1) {
	index = little;
    }
    return index;
}
int adjustRatemitGroup(int up, int down) {
    int i=0;
    int index = -1;
    int cycle = 0;

    while(isGroupIndexValid(index) != 1) {
	cycle++;
	for(i=0;i<MAX_WIFIDOG_RATE_LIMIT_GROUP;i++) {
	    if(rateLimitGroup[i].mark == 1) {
		if(cycle == 1) {
		/*首先比较下行速率是否有相同的组*/
		if((down > 0) && (rateLimitGroup[i].downRate == down)) {
		    index = i;
		    break;
		}
		} else if(cycle == 2) {
		/*然后比较上行速率是否有相同的组*/
		if((up > 0) && (rateLimitGroup[i].upRate == up)) {
		    index = i;
		    break;
		}
		} else if(cycle == 3) {
		    /* 选择速率执行金的组
		     * 按下行速率选择
		     * */
		    index = RateLimitGroupSortAndInsert(up, down);
		}
	    }
	}
	if(cycle > 3) {
	    break;
	}
    }
    return index;
}

void rateLimitGroupShow()
{
    int i = 0;

    for(i=0;i<MAX_WIFIDOG_RATE_LIMIT_GROUP;i++) {
	debug(LOG_DEBUG, "rate limit group index %d, ipset name %22s, upRate %6d, downRate %6d, mark %d, sort %d",
		i,rateLimitGroup[i].name,rateLimitGroup[i].upRate,rateLimitGroup[i].downRate,rateLimitGroup[i].mark,rateLimitGroup[i].sort);
    }

}
/*
 * isRateLimitGroupExist
 * 参数：
 * up	    - 上行限速速率
 * down	    - 下行限速速率
 * opt	    - 限速组操作，分以下情况
 *	    0 - 仅查找是否存在限速组
 *	    1 - 查找限速组，如果不存在添加
 *	    2 - 查找限速组，如果存在则删除
 *
 * */
int isRateLimitGroupExist(int up, int down, int opt)
{
    int i = 0;
    int index = -1;
    int spareIndex = -1;
    int noLimit = 1;
    /* 先设定五个限速组
     * 根据上下行速率查找组索引
     * 1 不存在则新建限速组且新增限速规则，返回索引
     * 2 存在，则返回索引
     * 3 不存在且没有限速组可分配，则返回-1
     **/
    for(i=0;i<MAX_WIFIDOG_RATE_LIMIT_GROUP;i++) {
	if(rateLimitGroup[i].mark == 1) {
	    noLimit = 0;
	    if((rateLimitGroup[i].upRate == up) && (rateLimitGroup[i].downRate == down) && ((up > 0) || (down > 0))) {
		index = i;
		break;
	    }
	} else {
	    if((spareIndex == -1)) {
		spareIndex = i;
	    }
	}
    }

    if(isGroupIndexValid(index) == 1) {
	debug(LOG_DEBUG, "find group index %d",index);
	if(opt == 2) {
	    /*删除限速规则*/
	    if(RateLimitRuleAddOrDel(index, rateLimitGroup[index].upRate, rateLimitGroup[index].downRate, IPTABLE_DEL) == 1) {
		rateLimitGroup[index].upRate = 0;
		rateLimitGroup[index].downRate = 0;
		rateLimitGroup[index].mark = 0;
		rateLimitGroup[index].sort = -1;
	    }
	}
    } else if(isGroupIndexValid(spareIndex) == 1) {
	if(opt == 1) { 
	    if(noLimit == 1) {
		/*添加限速入口链*/
		iptables_do_command("-t mangle -D FORWARD  -j "WIFI_RATE_LIMIT_CHAIN"" );
		iptables_do_command("-t mangle -I FORWARD  1 -j "WIFI_RATE_LIMIT_CHAIN"" );
	    }
	    /*添加限速规则*/
	    if(RateLimitRuleAddOrDel(spareIndex, up, down, IPTABLE_ADD) == 1) {
		rateLimitGroup[spareIndex].upRate = up;
		rateLimitGroup[spareIndex].downRate = down;
		rateLimitGroup[spareIndex].mark = 1;
		rateLimitGroup[spareIndex].sort = -1;
		index = spareIndex;
		debug(LOG_DEBUG, "not find group index,but find spare group index %d",index);
	    }
	} else {
	    index = -1;
	}
    } else {
	index = adjustRatemitGroup(up, down);
	debug(LOG_DEBUG, "adjust rate limit group, find group index %d",index);
    }
    return index;
}
/* 
 * createRateLimit()
 * 参数：
 * ip		- 客户端ip
 * upRate	- 上行速率限制
 * downRate	- 下行速率限制
 * version	- greenWiFi版本号
 * authType	- 客户端认证类型
 **/
int createRateLimit(char *ip, char *mac, int upRate, int downRate, int version, int authType)
{
    t_client *client = NULL;
    int index = -1;
    index = isRateLimitGroupExist(upRate, downRate, 1);

    if(index == -1) {
	/*限速组不存在且没有可分配的限速组*/
	debug(LOG_ERR, "[RATE_LIMIT] rate limit rule error, rate limit group is not find.");
    } else if(isGroupIndexValid(index) == 1) {
	 /* 限速组存在，将ip添加到限速组index中
	  * authType记录 client中
	  * 设置客户端为带宽控制类型
	  * version 是否记录待定
	  **/
	client = client_list_find(ip, mac);
	if(client != NULL) {
	    //ipsetAOrDIpStr(rateLimitGroup[index].name, client->ip, IPSET_MEM_DEL);
	    rateRuleIPAddOrDel(index, client->ip, IPSET_MEM_ADD);
	    client->ctrlType |= 1<<WIFIDOG_CLIENT_CTRL_TYPE_RATE_LIMIT;
	    client->data.index = index;
	} else {
	    debug(LOG_ERR, "[RATE_LIMIT] wifi add rate limit error, client ip %s mac %s is not find.",ip,mac);
	}	
    } else {
	/*未知错误*/
	debug(LOG_ERR, "[RATE_LIMIT] index error, Invalid index.");
    }
    rateLimitGroupShow();
    return index;
}
void rateRuleIPAddOrDel(int limitGroupIndex,char *ip ,char opt)
{
    int i = 0;
    int flag = 0;
    if(opt == IPSET_MEM_DEL){
	ipsetAOrDIpStr(rateLimitGroup[limitGroupIndex].name, ip, IPSET_MEM_DEL);
	rateLimitGroup[limitGroupIndex].count -= 1;
	for(i=0;i<MAX_WIFIDOG_RATE_LIMIT_GROUP;i++) {
	    if(rateLimitGroup[i].count <= 0) {
		RateLimitRuleAddOrDel(i, rateLimitGroup[i].upRate, rateLimitGroup[i].downRate, IPTABLE_DEL);
		rateLimitGroup[i].upRate = 0;
		rateLimitGroup[i].downRate = 0;
		rateLimitGroup[i].mark = 0;
		rateLimitGroup[i].sort = -1;
	    }else{
		flag = 1;
	    }
	}
	if(!flag){
	    iptables_do_command("-t mangle -d FORWARD  -j "WIFI_RATE_LIMIT_CHAIN"" );
	}
    }else if(opt == IPSET_MEM_ADD){
	ipsetAOrDIpStr(rateLimitGroup[limitGroupIndex].name, ip, IPSET_MEM_DEL);
	ipsetAOrDIpStr(rateLimitGroup[limitGroupIndex].name, ip, IPSET_MEM_ADD);
	rateLimitGroup[limitGroupIndex].count += 1;
    }
}
void destoryRateLimit()
{
    int i = 0;

    iptables_do_command("-t mangle -D FORWARD  -j "WIFI_RATE_LIMIT_CHAIN"" );
    iptables_do_command("-t mangle -F "WIFI_RATE_LIMIT_CHAIN" ");
    iptables_do_command("-t mangle -X "WIFI_RATE_LIMIT_CHAIN" ");
    for(i=0;i<MAX_WIFIDOG_RATE_LIMIT_GROUP;i++) {
	if(rateLimitGroup[i].mark == 1) {
	    ipsetFlushGrp(rateLimitGroup[i].name);
	}
	ipsetDelGrp(rateLimitGroup[i].name, FALSE);
    }
    memset(rateLimitGroup, 0, sizeof(rateLimitGroup));
}
void initRateLimit()
{
    int i = 0;

    memset(rateLimitGroup, 0, sizeof(rateLimitGroup));
    for(i=0;i<MAX_WIFIDOG_RATE_LIMIT_GROUP;i++) {
	snprintf(rateLimitGroup[i].name,MAX_IPSET_NAME_LENGTH,"%s%d",WIFI_IPSET_RATELIMIT_GROUP_PREFIX,i);
	if(webauthWebGrpAdd(rateLimitGroup[i].name) != 0) {
	    debug(LOG_ERR, "[RATE_LIMIT] create rate limit group erroe, index %d, name %s",i,rateLimitGroup[i].name);
	}
	rateLimitGroup[i].sort = -1;
    }
    iptables_do_command("-t mangle -N "WIFI_RATE_LIMIT_CHAIN" ");
}
#endif /*WIFI_RATE_LIMIT*/
#endif

