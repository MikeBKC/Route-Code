/*
 * shell_execute.c
 */
#include "private.h"
#include "lub/string.h"
#include "lub/argv.h"

#include <linux/autoconf.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "cliConvert.h"
#include "ifmisc.h"
#include "comExeFun.h"
#include "translate.h"
#include "wuScript.h"
#include "wusL2tpServer.h"
#include "wusPptpServer.h"
#include <stdarg.h>

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/utsname.h>
#include <signal.h>
#include <fcntl.h>
#include "clish/command.h"
#include "clish/command/private.h"
#include "clish/param/private.h"
#include "clish/var/private.h"
#include "clish/action/private.h"
#include "clish/ptype/private.h"
#include "lub/argv/private.h"
#include "clish/pargv/private.h"

#define SWORD_DEBUG 0
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

#if (UTT_LICENSE == FYES)
#define SWVER_1 4
#define SWVER_2 3
#define SWVER_3 4
#define HWVER_1 3
#define HWVER_2 2
#endif
typedef struct image_header {
    uint32_t imageLen;
    uint32_t imageCrc;
    uint32_t arch;
    uint32_t fsLen;
    uint32_t os;
	uint16_t systemVer;
    uint16_t imageType;
    uint16_t unuse;
    uint16_t funcVer;
    uint32_t loadAddr;
    uint32_t entryAddr;
    uint8_t  name[20];
    uint32_t kernelLen;
    uint32_t kernelCrc;
} image_header_t;

static int fd_global;

static int getCurInterface(clish_parg_t *ag);
static unsigned int getPositionByName(clish_parg_t* ag[], char *name);
static unsigned int strToDay(clish_parg_t **ag);
static int getValueByName(clish_parg_t **ag, char *name, char *buffer);
static int getTimeRange(clish_parg_t **ag, char *timeStart, char *timeStop);
static int getIPRange(clish_parg_t **ag, struct in_addr *pfromIP, struct in_addr *pendIP, char *ipStartParamName, char *ipStopParamName);
extern int moveLastToFirst(char* name);

static void doSystem(const char* _Format, ...)
{
    char cmdstr[128];/*Ӧ��������Ҫ���ipset�ʲ���̫��*/
    va_list   args; 

    memset(cmdstr, 0, sizeof(cmdstr));

    /*���������ַ���*/
    va_start(args,_Format); 
    vsnprintf(cmdstr, sizeof(cmdstr)-1, _Format,   args   ); 
    va_end(args);

    /*��������*/
    system(cmdstr);
    return;
}
/*
 * These are the internal commands for this framework.
 */
static clish_shell_builtin_fn_t
    clish_close,
    clish_overview,
    clish_source,
    clish_source_nostop,
    clish_history,
    clish_nested_up,
    clish_nop,
    clish_chpasswd,
#if 0
    clish_ethernet,
#endif
    clish_FastEthernet,
    clish_included_address,
    clish_dns_server,
    clish_name_server,
    clish_default_router,
    clish_ip_route,
    clish_no_ip_route,
    clish_nat_static,
    clish_no_nat_static,
    clish_write,
    clish_wdog,
    clish_ip_mtu,
    clish_ppp_auth,
    clish_ppp_pap_sentusername,
    clish_copy_to_remote,
    clish_copy_from_remote,
    clish_showRunningConfig,
    clish_show_flash,
    clish_dial,
#if (FIREWALL == FYES)
    clish_access_list,
    clish_no_access_list,
#endif
#if (GROUP_BASE == FYES)
#ifdef CONFIG_UTT_NETFILTER
    clish_rate_limit,
    clish_no_rate_limit;
#endif
#endif

static clish_shell_builtin_t clish_cmd_list[] = {
	{"clish_close", clish_close},
	{"clish_overview", clish_overview},
	{"clish_source", clish_source},
	{"clish_source_nostop", clish_source_nostop},
	{"clish_history", clish_history},
	{"clish_nested_up", clish_nested_up},
	{"clish_nop", clish_nop},
	{"clish_chpasswd", clish_chpasswd},
#if 0
	{"clish_ethernet", clish_ethernet},
#endif
	{"clish_FastEthernet", clish_FastEthernet},
	{"clish_included_address", clish_included_address},
	{"clish_dns_server", clish_dns_server},
	{"clish_name_server", clish_name_server},
	{"clish_default_router", clish_default_router},
	{"clish_ip_route", clish_ip_route},
	{"clish_no_ip_route", clish_no_ip_route},
	{"clish_nat_static", clish_nat_static},
	{"clish_no_nat_static", clish_no_nat_static},
	{"clish_write", clish_write},
	{"clish_wdog", clish_wdog},
	{"clish_ip_mtu", clish_ip_mtu},
	{"clish_ppp_auth", clish_ppp_auth},
	{"clish_ppp_pap_sentusername", clish_ppp_pap_sentusername},
	{"clish_copy_to_remote", clish_copy_to_remote},
	{"clish_copy_from_remote", clish_copy_from_remote},
	{"clish_showRunningConfig", clish_showRunningConfig},
	{"clish_show_flash",clish_show_flash},
	{"clish_dial",clish_dial},
#if (FIREWALL == FYES)
	{"clish_access_list", clish_access_list},
	{"clish_no_access_list", clish_no_access_list},
#endif
#if (GROUP_BASE == FYES)
#ifdef CONFIG_UTT_NETFILTER
	{"clish_rate_limit", clish_rate_limit},
	{"clish_no_rate_limit", clish_no_rate_limit},
#endif
#endif
	{NULL, NULL}
};

/*----------------------------------------------------------- */
/* Terminate the current shell session */
static int clish_close(clish_context_t *context, const lub_argv_t * argv)
{
	/* the exception proves the rule... */
	clish_shell_t *this = (clish_shell_t *)context->shell;

	argv = argv; /* not used */
	this->state = SHELL_STATE_CLOSING;

	return 0;
}

/*----------------------------------------------------------- */
/*
 Open a file and interpret it as a script in the context of a new
 thread. Whether the script continues after command, but not script, 
 errors depends on the value of the stop_on_error flag.
*/
static int clish_source_internal(clish_context_t *context,
	const lub_argv_t * argv, bool_t stop_on_error)
{
	int result = -1;
	const char *filename = lub_argv__get_arg(argv, 0);
	struct stat fileStat;

	/* the exception proves the rule... */
	clish_shell_t *this = (clish_shell_t *)context->shell;

	/*
	 * Check file specified is not a directory 
	 */
	if ((0 == stat((char *)filename, &fileStat)) &&
		(!S_ISDIR(fileStat.st_mode))) {
		/*
		 * push this file onto the file stack associated with this
		 * session. This will be closed by clish_shell_pop_file() 
		 * when it is finished with.
		 */
		result = clish_shell_push_file(this, filename,
			stop_on_error);
	}

	return result ? -1 : 0;
}

/*----------------------------------------------------------- */
/*
 Open a file and interpret it as a script in the context of a new
 thread. Invoking a script in this way will cause the script to
 stop on the first error
*/
static int clish_source(clish_context_t *context, const lub_argv_t * argv)
{
	return (clish_source_internal(context, argv, BOOL_TRUE));
}

/*----------------------------------------------------------- */
/*
 Open a file and interpret it as a script in the context of a new
 thread. Invoking a script in this way will cause the script to
 continue after command, but not script, errors.
*/
static int clish_source_nostop(clish_context_t *context, const lub_argv_t * argv)
{
	return (clish_source_internal(context, argv, BOOL_FALSE));
}

/*----------------------------------------------------------- */
/*
 Show the shell overview
*/
static int clish_overview(clish_context_t *context, const lub_argv_t * argv)
{
	clish_shell_t *this = context->shell;
	argv = argv; /* not used */

	tinyrl_printf(this->tinyrl, "%s\n", context->shell->overview);

	return 0;
}

/*----------------------------------------------------------- */
static int clish_history(clish_context_t *context, const lub_argv_t * argv)
{
	clish_shell_t *this = context->shell;
	tinyrl_history_t *history = tinyrl__get_history(this->tinyrl);
	tinyrl_history_iterator_t iter;
	const tinyrl_history_entry_t *entry;
	unsigned limit = 0;
	const char *arg = lub_argv__get_arg(argv, 0);

	if (arg && ('\0' != *arg)) {
		limit = (unsigned)atoi(arg);
		if (0 == limit) {
			/* unlimit the history list */
			(void)tinyrl_history_unstifle(history);
		} else {
			/* limit the scope of the history list */
			tinyrl_history_stifle(history, limit);
		}
	}
	for (entry = tinyrl_history_getfirst(history, &iter);
		entry; entry = tinyrl_history_getnext(&iter)) {
		/* dump the details of this entry */
		tinyrl_printf(this->tinyrl,
			"%5d  %s\n",
			tinyrl_history_entry__get_index(entry),
			tinyrl_history_entry__get_line(entry));
	}
	return 0;
}

/*----------------------------------------------------------- */
/*
 * Searches for a builtin command to execute
 */
static clish_shell_builtin_fn_t *find_builtin_callback(const
	clish_shell_builtin_t * cmd_list, const char *name)
{
	const clish_shell_builtin_t *result;

	/* search a list of commands */
	for (result = cmd_list; result && result->name; result++) {
		if (0 == strcmp(name, result->name))
			break;
	}
	return (result && result->name) ? result->callback : NULL;
}

/*----------------------------------------------------------- */
void clish_shell_cleanup_script(void *script)
{
	/* simply release the memory */
	lub_string_free(script);
}

/*-------------------------------------------------------- */
static int clish_shell_lock(const char *lock_path)
{
	int i;
	int res;
	int lock_fd = -1;
	struct flock lock;

	if (!lock_path)
		return -1;
	lock_fd = open(lock_path, O_WRONLY | O_CREAT, 00644);
	if (-1 == lock_fd) {
		fprintf(stderr, "Can't open lockfile %s.\n", lock_path);
		return -1;
	}
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	for (i = 0; i < CLISH_LOCK_WAIT; i++) {
		res = fcntl(lock_fd, F_SETLK, &lock);
		if (res != -1)
			break;
		if (EINTR == errno)
			continue;
		if ((EAGAIN == errno) || (EACCES == errno)) {
			if (0 == i)
				fprintf(stderr,
					"Try to get lock. Please wait...\n");
			sleep(1);
			continue;
		}
		break;
	}
	if (res == -1) {
		fprintf(stderr, "Can't get lock.\n");
		close(lock_fd);
		return -1;
	}
	fd_global = lock_fd;
	return lock_fd;
}

/*-------------------------------------------------------- */
static void clish_shell_unlock(int lock_fd)
{
	struct flock lock;

	if (lock_fd == -1)
		return;
	lock.l_type = F_UNLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	fcntl(lock_fd, F_SETLK, &lock);
	//close(lock_fd);
}

/*----------------------------------------------------------- */
int clish_shell_execute(clish_context_t *context, char **out)
{
	clish_shell_t *this = context->shell;
	const clish_command_t *cmd = context->cmd;
	clish_action_t *action;
	int result = 0;
	char *lock_path = clish_shell__get_lockfile(this);
	int lock_fd = -1;
	sigset_t old_sigs;
	struct sigaction old_sigint, old_sigquit, old_sighup;
	clish_view_t *cur_view = clish_shell__get_view(this);
	unsigned int saved_wdog_timeout = this->wdog_timeout;

	assert(cmd);
	action = clish_command__get_action(cmd);

	/* Pre-change view if the command is from another depth/view */
	{
		clish_view_restore_t restore = clish_command__get_restore(cmd);
		if ((CLISH_RESTORE_VIEW == restore) &&
			(clish_command__get_pview(cmd) != cur_view)) {
			clish_view_t *view = clish_command__get_pview(cmd);
			clish_shell__set_pwd(this, NULL, view, NULL, context);
		} else if ((CLISH_RESTORE_DEPTH == restore) &&
			(clish_command__get_depth(cmd) < this->depth)) {
			this->depth = clish_command__get_depth(cmd);
		}
	}

	/* Lock the lockfile */
	if (lock_path && clish_command__get_lock(cmd)) {
		lock_fd = clish_shell_lock(lock_path);
		if (-1 == lock_fd) {
			result = -1;
			goto error; /* Can't set lock */
		}
	}

	/* Ignore and block SIGINT, SIGQUIT, SIGHUP */
	if (!clish_command__get_interrupt(cmd)) {
		struct sigaction sa;
		sigset_t sigs;
		sa.sa_flags = 0;
		sigemptyset(&sa.sa_mask);
		sa.sa_handler = SIG_IGN;
		sigaction(SIGINT, &sa, &old_sigint);
		sigaction(SIGQUIT, &sa, &old_sigquit);
		sigaction(SIGHUP, &sa, &old_sighup);
		sigemptyset(&sigs);
		sigaddset(&sigs, SIGINT);
		sigaddset(&sigs, SIGQUIT);
		sigaddset(&sigs, SIGHUP);
		sigprocmask(SIG_BLOCK, &sigs, &old_sigs);
	}

	/* Execute ACTION */
	result = clish_shell_exec_action(action, context, out);

	/* Restore SIGINT, SIGQUIT, SIGHUP */
	if (!clish_command__get_interrupt(cmd)) {
		sigprocmask(SIG_SETMASK, &old_sigs, NULL);
		/* Is the signals delivery guaranteed here (before
		   sigaction restore) for previously blocked and
		   pending signals? The simple test is working well.
		   I don't want to use sigtimedwait() function bacause
		   it needs a realtime extensions. The sigpending() with
		   the sleep() is not nice too. Report bug if clish will
		   get the SIGINT after non-interruptable action.
		*/
		sigaction(SIGINT, &old_sigint, NULL);
		sigaction(SIGQUIT, &old_sigquit, NULL);
		sigaction(SIGHUP, &old_sighup, NULL);
	}

	/* Call config callback */
	if (!result && this->client_hooks->config_fn)
		this->client_hooks->config_fn(context);

	/* Call logging callback */
	if (clish_shell__get_log(this) && this->client_hooks->log_fn) {
		char *full_line = clish_shell__get_full_line(context);
		this->client_hooks->log_fn(context, full_line, result);
		lub_string_free(full_line);
	}

	/* Unlock the lockfile */
	if (lock_fd != -1){
		clish_shell_unlock(lock_fd);
		close(lock_fd);
	}
	/* Move into the new view */
	if (!result) {
		clish_view_t *view = clish_command__get_view(cmd);
		/* Save the PWD */
		if (view) {
			char *line = clish_shell__get_line(context);
			clish_shell__set_pwd(this, line, view,
				clish_command__get_viewid(cmd), context);
			lub_string_free(line);
		}
	}

	/* Set appropriate timeout. Workaround: Don't turn on  watchdog
	on the "set watchdog <timeout>" command itself. */
	if (this->wdog_timeout && saved_wdog_timeout) {
		tinyrl__set_timeout(this->tinyrl, this->wdog_timeout);
		this->wdog_active = BOOL_TRUE;
		fprintf(stderr, "Warning: The watchdog is active. Timeout is %u "
			"seconds.\nWarning: Press any key to stop watchdog.\n",
			this->wdog_timeout);
	} else
		tinyrl__set_timeout(this->tinyrl, this->idle_timeout);

error:
	return result;
}

/*----------------------------------------------------------- */
int clish_shell_exec_action(clish_action_t *action,
	clish_context_t *context, char **out)
{
	clish_shell_t *this = context->shell;
	int result = 0;
	const char *builtin;
	char *script;

	builtin = clish_action__get_builtin(action);
	script = clish_shell_expand(clish_action__get_script(action), SHELL_VAR_ACTION, context);
	if (builtin) {
		clish_shell_builtin_fn_t *callback;
		lub_argv_t *argv = script ? lub_argv_new(script, 0) : NULL;
		result = -1;
		/* search for an internal command */
		callback = find_builtin_callback(clish_cmd_list, builtin);
		if (!callback) {
			/* search for a client command */
			callback = find_builtin_callback(
				this->client_hooks->cmd_list, builtin);
		}
		/* invoke the builtin callback */
		if (callback)
			result = callback(context, argv);
		if (argv)
			lub_argv_delete(argv);
	} else if (script) {
		/* now get the client to interpret the resulting script */
		result = this->client_hooks->script_fn(context, action, script, out);
	}
	lub_string_free(script);

	return result;
}

/*----------------------------------------------------------- */
/*
 * Find out the previous view in the stack and go to it
 */
static int clish_nested_up(clish_context_t *context, const lub_argv_t *argv)
{
	clish_shell_t *this = context->shell;

	if (!this)
		return -1;

	argv = argv; /* not used */

	/* If depth=0 than exit */
	if (0 == this->depth) {
		this->state = SHELL_STATE_CLOSING;
		return 0;
	}
	this->depth--;

	return 0;
}

/*----------------------------------------------------------- */
/*
 * Builtin: NOP function
 */
static int clish_nop(clish_context_t *context, const lub_argv_t *argv)
{
	return 0;
}

/*----------------------------------------------------------- */
/*
 * Builtin: Set watchdog timeout. The "0" to turn watchdog off.
 */
static int clish_wdog(clish_context_t *context, const lub_argv_t *argv)
{
	const char *arg = lub_argv__get_arg(argv, 0);
	clish_shell_t *this = context->shell;

	/* Turn off watchdog if no args */
	if (!arg || ('\0' == *arg)) {
		this->wdog_timeout = 0;
		return 0;
	}

	this->wdog_timeout = (unsigned int)atoi(arg);

	return 0;
}

/*----------------------------------------------------------- */
const char *clish_shell__get_fifo(clish_shell_t * this)
{
	char *name;
	int res;

	if (this->fifo_name) {
		if (0 == access(this->fifo_name, R_OK | W_OK))
			return this->fifo_name;
		unlink(this->fifo_name);
		lub_string_free(this->fifo_name);
		this->fifo_name = NULL;
	}

	do {
		char template[] = "/tmp/klish.fifo.XXXXXX";
		name = mktemp(template);
		if (name[0] == '\0')
			return NULL;
		res = mkfifo(name, 0600);
		if (res == 0)
			this->fifo_name = lub_string_dup(name);
	} while ((res < 0) && (EEXIST == errno));

	return this->fifo_name;
}

/*--------------------------------------------------------- */
void *clish_shell__get_client_cookie(const clish_shell_t * this)
{
	return this->client_cookie;
}

/*-------------------------------------------------------- */
void clish_shell__set_log(clish_shell_t *this, bool_t log)
{
	assert(this);
	this->log = log;
}

/*-------------------------------------------------------- */
bool_t clish_shell__get_log(const clish_shell_t *this)
{
	assert(this);
	return this->log;
}

/********************************************************************
 * ������ clish_chpasswd
 * ���ܣ� �ı�Ĭ���û�����
 * ������ 2012-11-20
 * ������ klishĬ�ϴ������
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int clish_chpasswd(clish_context_t *context, const lub_argv_t *argv)
{
    MibProfileType mibType = MIB_PROF_ADMIN_CONF;
    AdminConfProfile *prof = NULL;
    unsigned index = 0U;
    unsigned agc = 0U;

    clish_parg_t* ag[10];

    clish_pargv_t* args= (clish_pargv_t*) context->pargv;

    agc = clish_pargv__get_count(args);
    for (index = 0U; index < agc; index++){
	ag[index] = clish_pargv__get_parg(args,index);
	SWORD_PRINTF("ag[%d]->param->name=%s\n",index,clish_parg__get_name((const clish_parg_t *)ag[index]));
	SWORD_PRINTF("ag[%d]->value=%s\n",index,clish_parg__get_value((const clish_parg_t *)ag[index]));
	SWORD_PRINTF("\n");
    }
    if (strcmp(ag[0]->value, "admin") == 0) {
	prof = (AdminConfProfile *)ProfGetInstPointByName(mibType, "admin");
	if (strcmp(ag[2]->value, prof->password) != 0) {
	    strcpy(prof->password, ag[2]->value);
	    doSystem("chpasswd.sh admin %s;killall goahead;goahead &", ag[2]->value);
	    SWORD_PRINTF("chpasswd.sh admin %s\n", ag[2]->value);
	}     
    } else {
	printf("User Name must be admin\n");
    }
    nvramWriteCommit();

    return 0;
}


#if (STATIC_ROUTE == FYES)
static int isStaticRouteIncludeLanIp(Uint32 lanIp, Uint32 lanMask) {
    MibProfileType mibType = MIB_PROF_STATIC_ROUTE;
    StaticRouteProfile *profRoute = NULL;
    int min = 0, max = 0, i = 0;
    int rst = 0;

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) {
	profRoute = (StaticRouteProfile *)ProfGetInstPointByIndex(mibType, i);
	if (ProfInstIsFree(profRoute) == 0) {
	    if ((lanIp & lanMask) == (ntohl(profRoute->dstAddr) & ntohl(profRoute->netmask))) {
		rst = 1;
		break;
	    }
	}
    }
    return rst;
}
#endif


/*
 * �ж�����ip��ַ���Ƿ��ص�
 * ���������
 *  ipS1,ipE1: ��һ��ip��ַ�ε���ʼip�ͽ���ip, �����ֽ���
 *  ipS2,ipE2: �ڶ���ip��ַ�ε���ʼip�ͽ���ip, �����ֽ���
 * ����ֵ:
 *  TRUE��     ����ip��ַ���ص�
 *  FALSE:     ����ip��ַ�β��ص�
 */
static int ipSegmentOverlap(Uint32 ipS1, Uint32 ipE1, Uint32 ipS2, Uint32 ipE2) {

    if (ipS1 >= ipS2 && ipS1 <= ipE2) {
	return TRUE;
    }
    if (ipE1 >= ipS2 && ipE1 <= ipE2) {
	return TRUE;
    }
    if (ipS2 >= ipS1 && ipS2 <= ipE1) {
	return TRUE;
    }
    if (ipE2 >= ipS1 && ipE2 <= ipE1) {
	return TRUE;
    }
    return FALSE;
}


/*
 * �����ַ���Ƿ��ͻ
 * �������:
 *  poolType:	Ҫ���ĵ�ַ�����ͣ��������ipS��ipE������pool����dhcp,pppoe,l2tp or pptp
 *  ipS:	��ʼIp��ַ, �����ֽ���
 *  ipE:	����ip��ַ, �����ֽ���
 * ���������	��
 * ����ֵ��
 *  �������ַ�س�ͻ�ĵ�ַ������
 */
static IpPoolEnum ipPoolConflict(IpPoolEnum poolType, Uint32 ipS, Uint32 ipE) {
    Uint32 ipStart = ntohl(ipS);
    Uint32 ipEnd = ntohl(ipE);

#if (DHCP_SERVER == FYES)
    /*
     * �Ƿ���dhcp server��ַ�س�ͻ
     */
    if (poolType != IP_POOL_DHCP) {
	MibProfileType mibTypeDhcp = MIB_PROF_DHCP_SERVER;
	DhcpServerProfile *profDhcp = NULL;
	profDhcp = (DhcpServerProfile *)ProfGetInstPointByIndex(mibTypeDhcp, 0);
	if ((profDhcp != NULL) && (ProfInstIsFree(profDhcp) == 0)) {
	    if (ipSegmentOverlap(ipStart, ipEnd, ntohl(profDhcp->ipStart), ntohl(profDhcp->ipEnd)) == TRUE) {
		return IP_POOL_DHCP;
	    }
	}
    }
#endif
#if (PPPOE_SERVER == FYES)
    /*
     * �Ƿ���pppoe server��ַ�س�ͻ
     */
    if (poolType != IP_POOL_PPPOE) {
	MibProfileType mibTypePppoe = MIB_PROF_PPPOE_SRV_GLO;
	PPPoESrvGloProfile *profPppoe = NULL;
	Uint32 pppoeIpE;
	profPppoe = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypePppoe, 0);
	if ((profPppoe != NULL) && (ProfInstIsFree(profPppoe) == 0)) {
	    if (profPppoe->maxSessCnt == 0) {
		pppoeIpE = ntohl(profPppoe->ipStart);
	    } else {
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
		pppoeIpE = ntohl(profPppoe->ipStart) + profPppoe->ipcount - 1;
#else
		pppoeIpE = ntohl(profPppoe->ipStart) + profPppoe->maxSessCnt - 1;
#endif
	    }
	    if (ipSegmentOverlap(ipStart, ipEnd, ntohl(profPppoe->ipStart), pppoeIpE) == TRUE) {
		return IP_POOL_PPPOE;
	    }
	}
    }
#endif
#if (L2TP_SERVER == FYES)
    /*
     * �Ƿ���l2tp server��ַ�س�ͻ
     */
    if (poolType != IP_POOL_L2TP) {
	MibProfileType mibTypeL2tp = MIB_PROF_L2TP_SRV_GLOBAL;
	L2TPSrvGlobalProfile *profL2tp = NULL;
	Uint32 l2tpIpE;
	profL2tp = (L2TPSrvGlobalProfile *)ProfGetInstPointByIndex(mibTypeL2tp, 0);
	if ((profL2tp != NULL) && (ProfInstIsFree(profL2tp) == 0)) {
	    if (profL2tp->poolIpCnt == 0) {
		l2tpIpE = ntohl(profL2tp->poolIpStart);
	    } else {
		l2tpIpE = ntohl(profL2tp->poolIpStart) + profL2tp->poolIpCnt - 1;
	    }

	    if (ipSegmentOverlap(ipStart, ipEnd, ntohl(profL2tp->poolIpStart), l2tpIpE) == TRUE) {
		return IP_POOL_L2TP;
	    }
	}

    }
#endif
#if (PPTP_SERVER == FYES)
    /*
     * �Ƿ���pptp server��ַ�س�ͻ
     */
    if (poolType != IP_POOL_PPTP) {
	MibProfileType mibTypePptp = MIB_PROF_PPTP_SRV_GLOBAL;
	PPTPSrvGlobalProfile *profPptp = NULL;
	Uint32 pptpIpE;
	profPptp = (PPTPSrvGlobalProfile *)ProfGetInstPointByIndex(mibTypePptp, 0);
	if ((profPptp != NULL) && (ProfInstIsFree(profPptp) == 0)) {
	    if (profPptp->poolIpCnt == 0) {
		pptpIpE = ntohl(profPptp->poolIpStart);
	    } else {
		pptpIpE = ntohl(profPptp->poolIpStart) + profPptp->poolIpCnt - 1;
	    }

	    if (ipSegmentOverlap(ipStart, ipEnd, ntohl(profPptp->poolIpStart), pptpIpE) == TRUE) {
		return IP_POOL_PPTP;
	    }
	}

    }
#endif
    return IP_POOL_NONE;
}



/*
 * ���õ�ַ�س�ͻ������Ϣ
 */
static void setPoolConflictMsg(IpPoolEnum poolType) {
    switch(poolType)
    {
	case IP_POOL_DHCP:
	    printf("LanIP in conflict with DHCP Server address pool\n");
	    break;
	case IP_POOL_PPPOE:
	    printf("LanIP in conflict with PPPOE Server address pool\n");
	    break;
	case IP_POOL_L2TP:
	    printf("LanIP in conflict with L2TP Server address pool\n");
	    break;
	case IP_POOL_PPTP:
	    printf("LanIP in conflict with PPTP Server address pool\n");
	    break;
	default:
	    break;
    }
    return;
}

/********************************************************************
 * ������ formLanConfig
 * ���ܣ� ����LAN������
 * ������ 2012-11-20
 * ������ LAN���������
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static void formLanConfig(Uint32 lanIp, Uint32 lanMask)
{
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;

    ProfChangeType profAction;
    struProfAlloc *profhead = NULL;

    IpPoolEnum cfPoolType = IP_POOL_NONE;
    Uint32 wanIp, wanMask;
    int i = 0;
    char if_addr[16] = {0};
    char if_nm[16] = {0};
    const char *errMsg=NULL;
    Uint32 wanCount = 0;
#if ((DHCP_SERVER == FYES) || (NET_SECURITY == FYES))
    unsigned int nIp, eIp, uNet, nIp1, eIp1;
#endif

#if (DHCP_SERVER == FYES)
    MibProfileType mibTypeDhcpS = MIB_PROF_DHCP_SERVER;
    DhcpServerProfile *profDhcpS = NULL;
#endif
#if (STATIC_NAT == FYES)
    MibProfileType mibTypeNat = MIB_PROF_STATIC_NAT;
    StaticNatProfile *profNat = NULL;
#endif
#if (L2TP_SERVER == FYES)
    MibProfileType mibTypeL2tp = MIB_PROF_L2TP_SRV_GLOBAL;
    L2TPSrvGlobalProfile *profL2tp = NULL;
#endif
#if (PPTP_SERVER == FYES)
    MibProfileType mibTypePptp = MIB_PROF_PPTP_SRV_GLOBAL;
    PPTPSrvGlobalProfile *profPptp = NULL;
#endif
#if (NET_SECURITY == FYES)
    SystemProfile *profSys = NULL;
#endif

#if (STATIC_ROUTE == FYES)
    /*
     * �жϾ�̬·��Ŀ�������Ƿ����lan ip
     */
    if (isStaticRouteIncludeLanIp(lanIp, lanMask))
    {
	printf("Ths lanIP is used by static route, can't modify\n");
	return;
    }
#endif

    wanCount = getWanSlotCount();

    /*
     * �ж�lan��ip��wan��ip�����Ƿ��ص�,wan�ڹ̶�IP���
     */
    for (i = 1; i <= wanCount; i++) 
    {
	if ((getIfIp(getWanIfNamePPP(i), if_addr) == 0) && 
		(getIfNetmask(getWanIfNamePPP(i), if_nm) == 0)) 
	{
	    wanIp = ntohl(inet_addr(if_addr));
	    wanMask = ntohl(inet_addr(if_nm));
	    /*
	     * ����Ƿ��wan����ͬһ����
	     */
	    if (((wanIp & wanMask) == (lanIp & wanMask)) || ((wanIp & lanMask) == (lanIp & lanMask)))
	    {
		printf("The LAN and WAN interface IP addresses must be on different subnets.\n");
		return;
	    }
	}
    }



    /*
     * ���lan��profile
     */
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
    if ((profIF == NULL) && (profIF->head.active == 0)) {
	printf("Failed to configure LAN settings\n");
	return;
    }

    if ((profIF->ether.sta.ip == htonl(lanIp)) && (profIF->ether.sta.nm == htonl(lanMask)))
    {
	return;
    }

    /* lan ip��ַ�Ƿ�͵�ַ�س�ͻ */
#if ((L2TP_SERVER == FYES) || (PPTP_SERVER == FYES) || (PPPOE_SERVER == FYES))
    cfPoolType = ipPoolConflict(IP_POOL_NONE, htonl(lanIp), htonl(lanIp));
    if (cfPoolType != IP_POOL_NONE)
    {
	setPoolConflictMsg(cfPoolType);
	return;
    }
#endif

#if (DHCP_SERVER == FYES)
    /*
     * ���dhcp��ʼ����ip
     */
    profDhcpS = (DhcpServerProfile*)ProfGetInstPointByIndex(mibTypeDhcpS, 0);
    if (profDhcpS == NULL) {
	printf("DHCP server error\n");
	return;
    }

    nIp = ntohl(profDhcpS->ipStart);
    eIp = ntohl(profDhcpS->ipEnd);
    /* ������lan������, dhcp��ַ������ */
    uNet = lanIp & 0xffffff00;
    /* dhcp��ַ����lanIp�仯��ĵ�ַ,�����ֽ��� */
    nIp1 = uNet + (nIp & 0x000000ff);
    eIp1 = uNet + (eIp & 0x000000ff);
#if 0
    printf("nIp=0x%x,eIp=0x%x,uNet=0x%x,nIp1=0x%x,eIp1=0x%x\n", nIp, eIp, uNet, nIp1, eIp1);
#endif
    if((lanIp <= eIp1) && (lanIp >= nIp1)){
	/* mantis 8712 */
	if (nIp1 != nIp) {
	    printf("Operation failed: DHCP address pool cannot include LAN interface IP address\n");
	} else {
	    printf("DHCP address pool cannot include LAN interface IP address\n");
	}
	return;
    }

    /*
     * �жϵ�ַ��
     */
    cfPoolType = ipPoolConflict(IP_POOL_DHCP, htonl(nIp1), htonl(eIp1));
    if (cfPoolType != IP_POOL_NONE) {
	setPoolConflictMsg(cfPoolType);
	return;
    }

    profAction = PROF_CHANGE_EDIT;
    ProfBackupByIndex(mibTypeDhcpS, profAction, 0, &profhead);

    if(profDhcpS->gateway != htonl(lanIp))
    {
	/*
	 * IP��ַ���޸�
	 */
	profDhcpS->ipStart = htonl(nIp1);
	profDhcpS->ipEnd = htonl(eIp1);
	profDhcpS->gateway = htonl(lanIp);
    }
    /*
     * dns����������dns�޸�Ϊlan��ip��ַ
     */
    if (profDhcpS->DnsProxyEn == FUN_ENABLE) {
	profDhcpS->pDns = htonl(lanIp);
    }
#endif

#if (NET_SECURITY == FYES)
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    nIp = ntohl(profSys->sysNetSe.accessCtrlFromIP);
    eIp = ntohl(profSys->sysNetSe.accessCtrlEndIP);
    uNet = lanIp & lanMask;
    /*���α仯�������豸���ʿ���*/
    if(((nIp&lanMask)!=uNet) || ((eIp&lanMask)!=uNet))
    {
	/*���ﲻ�豸��ʵ��������speedweb���豸������ز���
	 *���豸���ʿ�����ر仯�������speedweb�е�lan�ڳ�ʼ�������仯*/
	profSys->sysNetSe.accessCtrlFromIP = 0u;
	profSys->sysNetSe.accessCtrlEndIP = 0u;
    }
#endif

#if (STATIC_NAT == FYES)
    /*
     * Զ�̹���̬nat ����ip����lan�仯
     */
    profNat = (StaticNatProfile *)ProfGetInstPointByName(mibTypeNat, remCtrlName);
    if (profNat != NULL) {
	profNat->IP = htonl(lanIp);
    }
#endif



    /*
     * ����lan������
     */
    profAction = PROF_CHANGE_EDIT;
    ProfBackupByIndex(mibTypeIF, profAction, 0, &profhead);

    profIF->ether.sta.ip = htonl(lanIp);
    profIF->ether.sta.nm = htonl(lanMask);
    profIF->ether.speedMode = PORT_MODE_AUTO;

#if (L2TP_SERVER == FYES)
    /* ����ip�仯 */
    profL2tp = (L2TPSrvGlobalProfile *)ProfGetInstPointByIndex(mibTypeL2tp, 0);
    if (profL2tp != NULL) {
	profAction = PROF_CHANGE_EDIT;
	ProfBackupByIndex(mibTypeL2tp, profAction, 0, &profhead);
	profL2tp->listenIp = htonl(lanIp);
#if (STATIC_NAT == FYES)
	/* l2tp��̬ӳ��仯 */
	if (profL2tp->head.active == 1) {
	    profNat = (StaticNatProfile *)ProfGetInstPointByName(mibTypeNat, L2TP_NAT_NAME);
	    if (profNat != NULL) {
		profNat->IP = htonl(lanIp);
	    }
	}
#endif
    }
#endif

#if (PPTP_SERVER == FYES)
    /* ����ip�仯 */
    profPptp = (PPTPSrvGlobalProfile *)ProfGetInstPointByIndex(mibTypePptp, 0);
    if (profPptp != NULL) {
	profAction = PROF_CHANGE_EDIT;
	ProfBackupByIndex(mibTypePptp, profAction, 0, &profhead);
	profPptp->listenIp = htonl(lanIp);
#if (STATIC_NAT == FYES)
	/* pptp��̬ӳ��仯 */
	if (profPptp->head.active == 1) {
	    profNat = (StaticNatProfile *)ProfGetInstPointByName(mibTypeNat, PPTP_NAT_NAME);
	    if (profNat != NULL) {
		profNat->IP = htonl(lanIp);
	    }
	}
#endif
    }
#endif


    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();

    return;
}
/********************************************************************
 * ������ formWanIfConfig
 * ���ܣ� ����WAN������
 * ������ 2012-11-20
 * ������ WAN���������
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static void formWanIfConfig(int i, Uint32 lanIp, Uint32 lanMask)
{
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;

    ProfChangeType profAction;
    struProfAlloc *profhead = NULL;

    /*
     * ���Wan��profile
     */
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, i);
    if (profIF != NULL) {

	if(profIF->head.active == 0) {
	    profIF->head.active = 1;
	}
	/*
	 * ����Wan������
	 */
	profAction = PROF_CHANGE_EDIT;
	ProfBackupByIndex(mibTypeIF, profAction, 0, &profhead);

	profIF->ether.connMode = ETH_CONN_STATIC;
	profIF->ether.sta.ip = lanIp;
	profIF->ether.sta.nm = lanMask;

	doSystem("wan.sh %d\n", i);
    } else {
	printf("Configuration error");
    }
    nvramWriteCommit();

    return;
}
#if 0
static int clish_ethernet(clish_context_t *context, const lub_argv_t *argv)
{
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;

    ProfChangeType profAction;
    struProfAlloc *profhead = NULL;
    unsigned index = 0U;
    unsigned agc = 0U;
    Uint32 lanIp = 0U, lanMask = 0U;
    char interface[3];
    int i = 0;

    clish_parg_t* ag[10];

    clish_pargv_t* args= (clish_pargv_t*) context->pargv;

    memset(interface, 0, sizeof(interface));
    agc = clish_pargv__get_count(args);
    for (index = 0U; index < agc; index++){
	ag[index] = clish_pargv__get_parg(args,index);
	SWORD_PRINTF("ag[%d]->param->name=%s\n",index,clish_parg__get_name((const clish_parg_t *)ag[index]));
	SWORD_PRINTF("ag[%d]->value=%s\n",index,clish_parg__get_value((const clish_parg_t *)ag[index]));
	SWORD_PRINTF("\n");
    }

    sscanf(ag[4]->value, "%*s%*s%s", interface);/*ȡ���ǵڼ����ӿ�*/
    SWORD_PRINTF("%s-%d:interface[0] = %c\n", __func__, __LINE__, __FILE__, interface[0]);
    i = interface[0] - 48;/*ֱ�ӽ��ַ�������ת��Ϊ����*/
    SWORD_PRINTF("%s-%d:interface = %s, i = %d\n", __func__, __LINE__, __FILE__, interface, i);
    lanIp = inet_addr(ag[1]->value);
    lanMask = inet_addr(ag[2]->value);
    SWORD_PRINTF("%s-%d:lanIp = %x, lanMask = %x\n", __func__, __LINE__, __FILE__, lanIp, lanMask);
    if (i == 0) {/*LAN��*/
	formLanConfig(lanIp, lanMask);
    } else {/*WAN��*/
#if 0
	SWORD_PRINTF("WAN is disable\n");
#else
	formWanIfConfig(i, lanIp, lanMask);
#endif
    }

    return 0;
}
#endif

/********************************************************************
 * ������ clish_FastEthernet
 * ���ܣ� ���ýӿ�
 * ������ 2012-11-15
 * ������ klish�������
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int clish_FastEthernet(clish_context_t *context, const lub_argv_t *argv)
{
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;

    ProfChangeType profAction;
    struProfAlloc *profhead = NULL;
    unsigned index = 0U;
    unsigned agc = 0U;
    Uint32 lanIp = 0U, lanMask = 0U;
    char interface[3];
    int i = 0;

    clish_parg_t* ag[10];

    clish_pargv_t* args= (clish_pargv_t*) context->pargv;

    memset(interface, 0, sizeof(interface));
    agc = clish_pargv__get_count(args);
    for (index = 0U; index < agc; index++){
	ag[index] = clish_pargv__get_parg(args,index);
	SWORD_PRINTF("ag[%d]->param->name=%s\n",index,clish_parg__get_name((const clish_parg_t *)ag[index]));
	SWORD_PRINTF("ag[%d]->value=%s\n",index,clish_parg__get_value((const clish_parg_t *)ag[index]));
	SWORD_PRINTF("\n");
    }

    sscanf(ag[4]->value, "%*s%*s%s", interface);/*ȡ���ǵڼ����ӿ�*/
    SWORD_PRINTF("%s-%d:interface[0] = %c\n", __func__, __LINE__, __FILE__, interface[0]);
    i = (int)interface[0] - 48;/*ֱ�ӽ��ַ�������ת��Ϊ����*/
    SWORD_PRINTF("%s-%d:interface = %s, i = %d\n", __func__, __LINE__, __FILE__, interface, i);
    SWORD_PRINTF("%s-%d:lanIp = %x, lanMask = %x\n", __func__, __LINE__, __FILE__, lanIp, lanMask);
    if (i == 0) {/*LAN��*/
	lanIp = ip2int(ag[1]->value);
	lanMask = ip2int(ag[2]->value);
	formLanConfig(lanIp, lanMask);
    } else {/*WAN��*/
#if 0
	SWORD_PRINTF("WAN is disable\n");
#else
	lanIp = inet_addr(ag[1]->value);
	lanMask = inet_addr(ag[2]->value);
	formWanIfConfig(i, lanIp, lanMask);
#endif
    }

    return 0;
}


/********************************************************************
 * ������ clish_included_address
 * ���ܣ� ����DHCP��ʼ��ַ��������ַ������
 * ������ 2012-11-20
 * ������ klish�������
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int clish_included_address(clish_context_t *context, const lub_argv_t *argv)
{
    MibProfileType mibType = MIB_PROF_DHCP_SERVER;
    DhcpServerProfile *profDhcpS = NULL;
    struProfAlloc *profhead = NULL;
    unsigned index = 0U;
    unsigned agc = 0U;

    clish_parg_t* ag[10];

    clish_pargv_t* args= (clish_pargv_t*) context->pargv;

    profDhcpS = (DhcpServerProfile*)ProfGetInstPointByIndex(mibType, 0);
    if((profDhcpS != NULL) && (ProfInstIsFree(profDhcpS) == 0)) {
	agc = clish_pargv__get_count(args);
	for (index = 0U; index < agc; index++){
	    ag[index] = clish_pargv__get_parg(args, index);
	    SWORD_PRINTF("ag[%d]->param->name=%s\n",index,clish_parg__get_name((const clish_parg_t *)ag[index]));
	    SWORD_PRINTF("ag[%d]->value=%s\n",index,clish_parg__get_value((const clish_parg_t *)ag[index]));
	    SWORD_PRINTF("\n");
	}
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
	profDhcpS->ipStart = inet_addr(ag[0]->value);
	profDhcpS->ipEnd = inet_addr(ag[1]->value);
	profDhcpS->netmask = inet_addr(ag[2]->value);
	SWORD_PRINTF("%s-%d:profDhcpS->ipStart = %x, profDhcpS->ipEnd = %x, profDhcpS->netmask = %x\n", __func__, __LINE__, __FILE__, profDhcpS->ipStart, profDhcpS->ipEnd, profDhcpS->netmask);
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();

    }
    return 0;
}

/********************************************************************
 * ������ clish_dns_server
 * ���ܣ� ����DHCP����DNS��������ַ
 * ������ 2012-11-20
 * ������ klish�������
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int clish_dns_server(clish_context_t *context, const lub_argv_t *argv)
{
    MibProfileType mibType = MIB_PROF_DHCP_SERVER;
    DhcpServerProfile *profDhcpS = NULL;
    struProfAlloc *profhead = NULL;
    unsigned index = 0U;
    unsigned agc = 0U;
    struct in_addr addr;

    clish_parg_t* ag[10];

    clish_pargv_t* args= (clish_pargv_t*) context->pargv;

    profDhcpS = (DhcpServerProfile*)ProfGetInstPointByIndex(mibType, 0);
    if((profDhcpS != NULL) && (ProfInstIsFree(profDhcpS) == 0)) {
	agc = clish_pargv__get_count(args);
	for (index = 0U; index < agc; index++){
	    ag[index] = clish_pargv__get_parg(args, index);
	    SWORD_PRINTF("ag[%d]->param->name=%s\n",index,clish_parg__get_name((const clish_parg_t *)ag[index]));
	    SWORD_PRINTF("ag[%d]->value=%s\n",index,clish_parg__get_value((const clish_parg_t *)ag[index]));
	    SWORD_PRINTF("\n");
	}
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
	profDhcpS->pDns = inet_addr(ag[0]->value);
	if (strcmp(ag[1]->param->name, "secondserver") == 0) {
	    profDhcpS->sDns = inet_addr(ag[1]->value);
	}
	SWORD_PRINTF("%s-%d:profDhcpS->pDns = %x, profDhcpS->sDns = %x\n", __func__, __LINE__, __FILE__, profDhcpS->pDns, profDhcpS->sDns);
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();

    }
    return 0;
}

/********************************************************************
 * ������ clish_name_server
 * ���ܣ� ����DHCP����DNS��������ַ
 * ������ 2012-11-20
 * ������ klish�������
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int clish_name_server(clish_context_t *context, const lub_argv_t *argv)
{
    MibProfileType mibType = MIB_PROF_DHCP_SERVER;
    DhcpServerProfile *profDhcpS = NULL;
    struProfAlloc *profhead = NULL;
    unsigned index = 0U;
    unsigned agc = 0U;
    struct in_addr addr;

    clish_parg_t* ag[10];

    clish_pargv_t* args= (clish_pargv_t*) context->pargv;

    profDhcpS = (DhcpServerProfile*)ProfGetInstPointByIndex(mibType, 0);
    if((profDhcpS != NULL) && (ProfInstIsFree(profDhcpS) == 0)) {
	agc = clish_pargv__get_count(args);
	for (index = 0U; index < agc; index++) {
	    ag[index] = clish_pargv__get_parg(args, index);
	    SWORD_PRINTF("ag[%d]->param->name=%s\n",index,clish_parg__get_name((const clish_parg_t *)ag[index]));
	    SWORD_PRINTF("ag[%d]->value=%s\n",index,clish_parg__get_value((const clish_parg_t *)ag[index]));
	    SWORD_PRINTF("\n");
	}
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
	profDhcpS->pPDns = inet_addr(ag[0]->value);
	if (strcmp(ag[1]->param->name, "secondip") == 0) {
	    profDhcpS->sPDns = inet_addr(ag[1]->value);
	}
	SWORD_PRINTF("%s-%d:profDhcpS->pPDns = %x, profDhcpS->sPDns = %x\n", __func__, __LINE__, __FILE__, profDhcpS->pPDns, profDhcpS->sPDns);
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();

    }
    return 0;
}

/********************************************************************
 * ������ clish_default_router
 * ���ܣ� ����DHCP�����ص�ַ
 * ������ 2012-11-20
 * ������ klish�������
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int clish_default_router(clish_context_t *context, const lub_argv_t *argv)
{
    MibProfileType mibType = MIB_PROF_DHCP_SERVER;
    DhcpServerProfile *profDhcpS = NULL;
    struProfAlloc *profhead = NULL;
    unsigned index = 0U;
    unsigned agc = 0U;
    struct in_addr addr;

    clish_parg_t* ag[10];

    clish_pargv_t* args= (clish_pargv_t*) context->pargv;

    profDhcpS = (DhcpServerProfile*)ProfGetInstPointByIndex(mibType, 0);
    if((profDhcpS != NULL) && (ProfInstIsFree(profDhcpS) == 0)) {
	agc = clish_pargv__get_count(args);
	for (index = 0U; index < agc; index++){
	    ag[index] = clish_pargv__get_parg(args, index);
	    SWORD_PRINTF("ag[%d]->param->name=%s\n",index,clish_parg__get_name((const clish_parg_t *)ag[index]));
	    SWORD_PRINTF("ag[%d]->value=%s\n",index,clish_parg__get_value((const clish_parg_t *)ag[index]));
	    SWORD_PRINTF("\n");
	}
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
	profDhcpS->gateway = inet_addr(ag[0]->value);
	SWORD_PRINTF("%s-%d:profDhcpS->gateway = %x\n", __func__, __LINE__, __FILE__, profDhcpS->gateway);
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();

    }
    return 0;
}

/********************************************************************
 * ������ getInterfaceName
 * ���ܣ� ȡ�ýӿ�����
 * ������ 2012-11-20
 * ������ klish�������
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static char* getInterfaceName(IPADDR gateway)
{
    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;
    int i = 0;
    static char if_name[5];

    memset(if_name, 0, sizeof(if_name));

    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, i);
    if ((profIf != NULL) && (ProfInstIsFree(profIf) == 0)) { 
	if ((gateway & profIf->ether.sta.nm) == (profIf->ether.sta.ip & profIf->ether.sta.nm)) {
	    strcpy(if_name, "LAN0");
	}
    }
    if(*if_name == '\0') {
	for (i = 1; i <= getWanSlotCount(); i++) {
	    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, i);
	    if ((profIf != NULL) && (ProfInstIsFree(profIf) == 0)) { 
		if ((profIf->ether.connMode == ETH_CONN_STATIC) && ((gateway & profIf->ether.sta.nm) == (profIf->ether.sta.ip & profIf->ether.sta.nm))) {
		    break;
		}
	    }
	}
	if (i != (getWanSlotCount() + 1)) {
	    /*��WAN�ڵ�ַ�����ص�ַͬ����*/
	    sprintf(if_name, "WAN%d", i);
	} else {
	    strcpy(if_name, "WAN1");
	}
    }
    SWORD_PRINTF("%s-%d:%s, if_name = %s\n", __func__, __LINE__, __FILE__, if_name);
    return if_name;
}

/********************************************************************
 * ������ clish_ip_route
 * ���ܣ� ����һ����̬·��
 * ������ 2012-11-20
 * ������ klish�������
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int clish_ip_route(clish_context_t *context, const lub_argv_t *argv)
{
    MibProfileType mibType = MIB_PROF_STATIC_ROUTE;
    struProfAlloc *profhead = NULL;
    StaticRouteProfile *profRoute = NULL;
    char StaticRouteNameH[7] = "route_"; /*dhcp�Զ���Ĭ��·����ǰ׺*/
    int instIndex = 0, maxDeNameNo = 0;
    unsigned index = 0U;
    unsigned agc = 0U;
    Uint32 i = 0U;
    clish_parg_t* ag[10];
    char *routeName = NULL;
    char *interfacename = NULL;

    clish_pargv_t* args= (clish_pargv_t*) context->pargv;

    agc = clish_pargv__get_count(args);
    for (index = 0U; index < agc; index++){
	ag[index] = clish_pargv__get_parg(args, index);
	SWORD_PRINTF("ag[%d]->param->name=%s\n",index,clish_parg__get_name((const clish_parg_t *)ag[index]));
	SWORD_PRINTF("ag[%d]->value=%s\n",index,clish_parg__get_value((const clish_parg_t *)ag[index]));
	SWORD_PRINTF("\n");
    }
    interfacename = getInterfaceName(inet_addr(ag[2]->value));
    if(*interfacename != '\0') {/*���صĽӿ�����Ϊ���򴥷�profile Change*/
	maxDeNameNo = ProfGetMaxDefaultNameNo(mibType, StaticRouteNameH);/*��ȡ���Ĭ��·�������*/
	SWORD_PRINTF("%s-%d:%s, maxDeNameNo = %d\n", __func__, __LINE__, __FILE__, maxDeNameNo);
	routeName = ProfBuildDefaultName(StaticRouteNameH, maxDeNameNo);/*����Ĭ��·����*/
	SWORD_PRINTF("%s-%d:%s, routeName = %s\n", __func__, __LINE__, __FILE__, routeName);
	if(ProfGetInstIndexByName(mibType, routeName) == PROFFAIL) {
	    instIndex = ProfNewInst(mibType, routeName, FALSE);/*�½�ʵ��*/
	    if(instIndex != PROFFAIL) {
		/*�������ظ����жϡ�������ֻ����ʱ�ﵽ�����Ŀ*/
		ProfBackupByIndex(mibType, PROF_CHANGE_ADD, instIndex, &profhead);
		profRoute = (StaticRouteProfile *)ProfGetInstPointByIndex(mibType, instIndex);
		profRoute->netmask = inet_addr(ag[1]->value);
		profRoute->dstAddr = (inet_addr(ag[0]->value) & profRoute->netmask);
		profRoute->gwAddr = inet_addr(ag[2]->value);
		strcpy(profRoute->interfaceName, interfacename);
		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
		nvramWriteCommit();
	    }
	}
    }
    return 0;
}



/********************************************************************
 * ������ clish_no_ip_route
 * ���ܣ� ɾ��һ����̬·��
 * ������ 2012-11-20
 * ������ klish�������
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int clish_no_ip_route(clish_context_t *context, const lub_argv_t *argv)
{
    MibProfileType mibType = MIB_PROF_STATIC_ROUTE;
    StaticRouteProfile *profRoute = NULL;
    struProfAlloc *profhead = NULL;
    unsigned index = 0U;
    unsigned agc = 0U;
    int i = 0, min = 0, max = 0;
    clish_parg_t* ag[10];

    clish_pargv_t* args= (clish_pargv_t*) context->pargv;

    agc = clish_pargv__get_count(args);
    for (index = 0U; index < agc; index++) {
	ag[index] = clish_pargv__get_parg(args, index);
	SWORD_PRINTF("ag[%d]->param->name=%s\n",index,clish_parg__get_name((const clish_parg_t *)ag[index]));
	SWORD_PRINTF("ag[%d]->value=%s\n",index,clish_parg__get_value((const clish_parg_t *)ag[index]));
	SWORD_PRINTF("\n");
    }
    ProfInstLowHigh(mibType, &max, &min);
    for (i = 0; i < max; i++) {
	profRoute = (StaticRouteProfile *)ProfGetInstPointByIndex(mibType, i);
	if ((profRoute != NULL) && (ProfInstIsFree(profRoute) == 0)) {
	    if ((profRoute->netmask == inet_addr(ag[1]->value)) 
		    && (profRoute->dstAddr == (inet_addr(ag[0]->value) 
			    & profRoute->netmask)) 
		    && (profRoute->gwAddr == inet_addr(ag[2]->value))) {
		SWORD_PRINTF("%s-%d:%s, Find\n", __func__, __LINE__, __FILE__);
		ProfBackupByIndex(mibType, PROF_CHANGE_DEL, i, &profhead);
		ProfDelInstByIndex(mibType, i);
		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
		nvramWriteCommit();
	    }
	}
    }
    return 0;
}

/********************************************************************
 * ������ clish_nat_static
 * ���ܣ� ����һ����̬ӳ��
 * ������ 2012-11-22
 * ������ klish�������
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int clish_nat_static(clish_context_t *context, const lub_argv_t *argv)
{
    MibProfileType mibType = MIB_PROF_STATIC_NAT;
    struProfAlloc *profhead = NULL;
    StaticNatProfile *staticNatProfile = NULL;
    char NatStaticNameH[8] = "natsta_"; /*dhcp�Զ���Ĭ��·����ǰ׺*/
    int instIndex = 0, maxDeNameNo = 0;
    unsigned index = 0U;
    unsigned agc = 0U;
    clish_parg_t* ag[10];
    char *natName = NULL;
    char interfacename[5];
    int change = 0;
    int protocol = 0;
    IPADDR IP = 0U;
    UINT32 InnerPort = 0U;
    UINT32 OutPort = 0U;

    clish_pargv_t* args= (clish_pargv_t*) context->pargv;

    memset(interfacename, 0, sizeof(interfacename));
    agc = clish_pargv__get_count(args);
    for (index = 0U; index < agc; index++){
	ag[index] = clish_pargv__get_parg(args, index);
	SWORD_PRINTF("ag[%d]->param->name=%s\n",index,clish_parg__get_name((const clish_parg_t *)ag[index]));
	SWORD_PRINTF("ag[%d]->value=%s\n",index,clish_parg__get_value((const clish_parg_t *)ag[index]));
	SWORD_PRINTF("\n");
    }
    if (strcmp(ag[1]->value, "tcp") == 0) {
	protocol = 1;
    } else if ((strcmp(ag[1]->value, "udp") == 0)) {
	protocol = 2;
    } else {
	protocol = 3;
    }
    change = (protocol - 1) / 2;
    SWORD_PRINTF("%s-%d:%s, change = %d\n", __func__, __LINE__, __FILE__, change);
    change *= 2;
    if(strcmp(ag[4 - change]->value, "interface") == 0) {/*���ڽӿ���*/
	    sprintf(interfacename, "WAN%s", ag[6 - change]->value);
	    IP = inet_addr(ag[2 - change]->value);
	    InnerPort = strtoul(ag[7 - change]->value, 0, 10);
	    OutPort = strtoul(ag[3 - change]->value, 0, 10);
    }
    SWORD_PRINTF("%s-%d:%s, interfacename = %s, IP = %x, InnerPort = %u, OutPort = %u\n", 
	    __func__, __LINE__, __FILE__, interfacename, IP, InnerPort, OutPort);

    maxDeNameNo = ProfGetMaxDefaultNameNo(mibType, NatStaticNameH);/*��ȡ���Ĭ��·�������*/
    SWORD_PRINTF("%s-%d:%s, maxDeNameNo = %d\n", __func__, __LINE__, __FILE__, maxDeNameNo);
    natName = ProfBuildDefaultName(NatStaticNameH, maxDeNameNo);/*����Ĭ��·����*/
    SWORD_PRINTF("%s-%d:%s, natName = %s\n", __func__, __LINE__, __FILE__, natName);
    if(ProfGetInstIndexByName(mibType, natName) == PROFFAIL) { /*���û���δ��ʹ��*/
	instIndex = ProfNewInst(mibType, natName, FALSE);/*�½�ʵ��*/
	if(instIndex != PROFFAIL) {
	    /*�������ظ����жϡ�������ֻ����ʱ�ﵽ�����Ŀ*/
	    ProfBackupByIndex(mibType, PROF_CHANGE_ADD, instIndex, &profhead);
	    staticNatProfile = (StaticNatProfile *)ProfGetInstPointByIndex(mibType, instIndex);
	    staticNatProfile->protocol = protocol;
	    staticNatProfile->IP = IP;
	    staticNatProfile->InnerPort = InnerPort;
	    staticNatProfile->OutPort = OutPort;
	    strcpy(staticNatProfile->NatBind, interfacename);
	    staticNatProfile->PortNum = 1;
	    ProfUpdate(profhead);
	    ProfFreeAllocList(profhead);
	    nvramWriteCommit();
	}
    }
    return 0;
}

/********************************************************************
 * ������ clish_no_nat_static
 * ���ܣ� ɾ��һ����̬·��
 * ������ 2012-11-20
 * ������ klish�������
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int clish_no_nat_static(clish_context_t *context, const lub_argv_t *argv)
{
    MibProfileType mibType = MIB_PROF_STATIC_NAT;
    StaticNatProfile *staticNatProfile = NULL;
    struProfAlloc *profhead = NULL;
    unsigned index = 0U;
    unsigned agc = 0U;
    int change = 0, i = 0, min = 0, max = 0;
    int protocol = 0;
    clish_parg_t* ag[10];
    char interfacename[5];

    clish_pargv_t* args= (clish_pargv_t*) context->pargv;

    memset(interfacename, 0, sizeof(interfacename));
    agc = clish_pargv__get_count(args);
    for (index = 0U; index < agc; index++) {
	ag[index] = clish_pargv__get_parg(args, index);
	SWORD_PRINTF("ag[%d]->param->name=%s\n",index,clish_parg__get_name((const clish_parg_t *)ag[index]));
	SWORD_PRINTF("ag[%d]->value=%s\n",index,clish_parg__get_value((const clish_parg_t *)ag[index]));
	SWORD_PRINTF("\n");
    }
    if (strcmp(ag[1]->value, "tcp") == 0) {
	protocol = 1;
    } else if ((strcmp(ag[1]->value, "udp") == 0)) {
	protocol = 2;
    } else {
	protocol = 3;
    }
    change = (protocol - 1) / 2;
    SWORD_PRINTF("%s-%d:%s, change = %d\n", __func__, __LINE__, __FILE__, change);
    change *= 2;
    if(strcmp(ag[4 -  change]->value, "interface") == 0) {/*���ڽӿ���*/
	/*�ӿں�Ϊ1-9*/
	sprintf(interfacename, "WAN%s", ag[6 - change]->value);
	ProfInstLowHigh(mibType, &max, &min);
	for (i = 0; i < max; i++) {
	    staticNatProfile = (StaticNatProfile *)ProfGetInstPointByIndex(mibType, i);
	    if ((staticNatProfile != NULL) && (ProfInstIsFree(staticNatProfile) == 0)) {
		if ((staticNatProfile->protocol == protocol)
			&& (staticNatProfile->IP == inet_addr(ag[2 - change]->value)) 
			&& (staticNatProfile->InnerPort == strtoul(ag[7 - change]->value, 0, 10)) 
			&& (staticNatProfile->OutPort == strtoul(ag[3 - change]->value, 0, 10))
			&& (strcmp(staticNatProfile->NatBind, interfacename) == 0)) {
		    SWORD_PRINTF("%s-%d:%s, Find\n", __func__, __LINE__, __FILE__);
		    ProfBackupByIndex(mibType, PROF_CHANGE_DEL, i, &profhead);
		    ProfDelInstByIndex(mibType, i);
		    ProfUpdate(profhead);
		    ProfFreeAllocList(profhead);
		    nvramWriteCommit();
		}
	    }
	}
    }
    return 0;
}

/********************************************************************
 * ������ clish_write
 * ���ܣ� ������д��flash
 * ������ 2012-11-21
 * ������ klish�������
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int clish_write(clish_context_t *context, const lub_argv_t *argv)
{
    nvramWriteCommit();
    return 0;
}

/********************************************************************
 * �������ܣ��޸�InterfaceProfile�е�mtu
 * ����ʱ�䣺2012-11-30
 * ����ֵ��1--ʧ�ܣ�0--�ɹ�
 ******************************************************************/
static int clish_ip_mtu(clish_context_t *context, const lub_argv_t *argv)
{
    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL; 
    struProfAlloc *profhead = NULL;
    int i = 0;/*�˿ں�*/
    int ret = 0;/*����ֵ*/
    unsigned index = 0U;
    unsigned agc = 0U;
    clish_parg_t* ag[10];
    clish_pargv_t *args = (clish_pargv_t *)context->pargv;
    
    agc = clish_pargv__get_count(args);
    
    /*�õ������������*/
    for (index = 0U; index < agc; index++){
	ag[index] = clish_pargv__get_parg(args,index);
    }
    
    /*�ж϶˿ں�*/
    i = getCurInterface(ag[2]);
    if (i == -1)/*-1��ʾû�ҵ�*/
    {
	ret = 1;
    }
    else
    { 
	/*���ն˿ں�ȡ�ö�Ӧ�ṹ�壬��ֵ��ʧ���˳�*/
    	profIF = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, i);
    	if (profIF == NULL)
    	{
	    ret = 1;
    	}
	else
	{
	    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, i, &profhead);/*���ݽṹ��*/
	    profIF->ether.pppoe.mtu = strtol(ag[0]->value, NULL, 10);/*����mtu*/
    
	    /*����ṹ��*/
	    ProfUpdate(profhead);
	    ProfFreeAllocList(profhead);
	    nvramWriteCommit();
	}
    }

    return ret;
}

/*********************************************************************
 * �������ܣ��жϵ�ǰ���ĸ��ӿ���
 * ����ʱ�䣺2012-11-30
 *********************************************************************/
static int32_t getCurInterface(clish_parg_t *ag)
{
    char array[4];
    int32_t i;
    memset(array, 0, sizeof(array));
    sscanf(ag->value, "%*s%*s%s", array);
    i = (int)strtol(array, NULL, 10);
    /*LAN����������Ч����ʾ������Ϣ*/
    if (i == 0)
    {
	printf("0 is LAN interface, the command is useless\n");
	i = -1;
    } 
    /*����wan�������������ʾ������Ϣ*/
    if (i > MAX_WAN_IF_COUNT)
    {
	printf("The router only have %d WAN interface\n", MAX_WAN_IF_COUNT);
	i = -1;
    }
    return i;
}

/**********************************************************************
 * �������ܣ��޸�pppoe��������֤��ʽ
 * ����ʱ�䣺2012-11-30
 * ����ֵ:1--ʧ�ܣ�0--�ɹ�
 ********************************************************************/
static int clish_ppp_auth(clish_context_t *context, const lub_argv_t *argv)
{
    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
    struProfAlloc *profhead = NULL;
    int authType = 0;/*��֤����*/
    int i = 0;/*�˿ں�*/
    int ret = 0; /*����ֵ*/
    unsigned index = 0U;
    unsigned agc = 0U;
    clish_parg_t* ag[10];
    clish_pargv_t *args = (clish_pargv_t *)context->pargv;

    agc = clish_pargv__get_count(args);
    /*ȡ�ò�������*/
    for (index = 0U; index < agc; index++){
        ag[index] = clish_pargv__get_parg(args,index);
#if 0
	printf("ag[%d]->param->name=%s\n",index,clish_parg__get_name((const clish_parg_t *)ag[index]));
        printf("ag[%d]->value=%s\n",index,clish_parg__get_value((const clish_parg_t *)ag[index]));
        printf("\n");
#endif
    }
    
    /*�жϵ�ǰ���ĸ��˿���*/
    i = getCurInterface(ag[3]);
    if (i == -1)
    {
	ret = 1;
    }
    else
    {	
	/*���յõ��Ķ˿ں�ȡ�ö�Ӧ�ṹ��*/
	profIF = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, i);
	if (profIF == NULL)/*ʧ�����˳�*/
	{
	    ret = 1;
	}
	else
	{
	    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, i, &profhead);
	    
	    if ((strcmp(ag[0]->value, "pap")== 0) && (strcmp(ag[1]->value, "callout")==0))
	    {
		authType = PPP_AUTH_PAP;
	    }
	    
	    /*������֤����*/
	    profIF->ether.pppoe.authType = authType;
	    
	    /*����ṹ��*/
	    ProfUpdate(profhead);
	    ProfFreeAllocList(profhead);
	    nvramWriteCommit();
	}
    }
    return ret;    
}

/*********************************************************************
 * �������ܣ��޸�pppoe������û���������
 * ����ʱ�䣺2012-12-01
 * ��ע��������ִ�к��ֱ���л���pppoeģʽ
 ******************************************************************/
static int clish_ppp_pap_sentusername(clish_context_t *context, const lub_argv_t *argv)
{
    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
    struProfAlloc *profhead = NULL;
    int i = 0;
    int ret = 0;/*����ֵ*/
    unsigned index = 0U;
    unsigned agc = 0U;
    clish_parg_t* ag[10];
    clish_pargv_t *args = (clish_pargv_t *)context->pargv;

    agc = clish_pargv__get_count(args);
    
    /*ȡ�������������*/
    for (index = 0U; index < agc; index++){
        ag[index] = clish_pargv__get_parg(args,index);
#if 0
        printf("ag[%d]->param->name=%s\n",index,clish_parg__get_name((const clish_parg_t *)ag[index])
);
        printf("ag[%d]->value=%s\n",index,clish_parg__get_value((const clish_parg_t *)ag[index]));
        printf("\n");
#endif
    }
    
    /*�жϵ�ǰ���Ǹ��ӿ��£�����-1��ʾ����Ҫ���ã�ֱ���˳�*/
    i = getCurInterface(ag[4]);
    if (i == -1)
    {
	ret = 1;
    }
    else
    {
	/*ȡ�ö�Ӧ�˿ڵĽṹ��*/
	profIF = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, i);
	if (profIF == NULL)
	{
	    ret = 1;
	}
	else
	{
	    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, i, &profhead);

	    /*�����˺������ֱ���л���pppoeģʽ*/
	    profIF->ether.connMode = ETH_CONN_PPPOE;
	    strncpy((char*)profIF->ether.pppoe.user, ag[0]->value, UTT_PPPOE_CLI_NAME_LEN);/*�û���*/
	    strncpy((char*)profIF->ether.pppoe.passwd, ag[2]->value, UTT_PPPOE_CLI_PASS_LEN);/*����*/
	    
	    /*����ṹ��*/
	    ProfUpdate(profhead);
	    ProfFreeAllocList(profhead);
	    nvramWriteCommit();
	}
    }
    return ret; 
}

/*********************************************************************
 * �������ܣ����������ҵ���ag�����ж�Ӧ���±�
 * ����ʱ�䣺2012-12-04
 * ����ֵ  ������--�����±꣬999--û�ҵ�
 *********************************************************************/
static unsigned int getPositionByName(clish_parg_t **ag, char *name)
{
    unsigned int i = 0U;
    unsigned int ret = 999U;/*����ֵ*/
    while (ag[i] != NULL)
    {
	/*��˳��Ա�ÿ��������name���ҵ��򷵻ص�ǰ�±�*/
	if (0 == strcmp(ag[i]->param->name, name))
	{
	    ret = i;
	}
	if (strcmp(ag[i]->param->name, "_cur_depth") == 0)
	{
	    break;
	}
	i++;
    }
    if ((strcmp(name, "_cur_pwd") == 0) && (strcmp(ag[i+1]->param->name, name) == 0))
    {
	ret = i;
    }
    return ret;
}


#if (FIREWALL == FYES)
/**************************************************************
 * �������ܣ��õ�����ip����
 * ����ʱ�䣺2012-12-17
 ************************************************************/
static int getProtocolType(clish_parg_t **ag, int *protocolP)
{
    unsigned int i = 0U;
    /*����Э������*/
    i = getPositionByName(ag, "proto");
    /*�жϷ���ֵ��999U��ʾû�ҵ�*/
    if (i < 20U)
    {
	if ((strcmp(ag[i]->value, "ip")) == 0)
	{	
	    *protocolP = Filter_ALL;
	}
	else if ((strcmp(ag[i]->value, "ahp")) == 0)
	{	
	    *protocolP = Filter_AH;
	}
	else if ((strcmp(ag[i]->value, "icmp")) == 0)
	{	
	    *protocolP = Filter_ICMP;
	}
	else if ((strcmp(ag[i]->value, "tcp")) == 0)
	{	
	    *protocolP = Filter_TCP;		
	} 
	else/*��������ΪUDP*/
	{
	    *protocolP = Filter_UDP;           
	}  
    }
    else
    {
	return 1;
    }
    return 0;/*Э��Ϊ��ѡ��Ĭ�Ϸ���0*/
}


/********************************************************************
 * �������ܣ�ת��ip
 * ����ʱ�䣺2012-12-17
 * ���������ag--����������������飬flag--�ж�Դ��ַ��Ŀ�ĵ�ַ��
 *	    addr_FromIp_P,addr_EndIp_P--����ת�����ip
 * ����ֵ��0--�ɹ���1--ʧ��
 *********************************************************************/
static int IPTransducer(clish_parg_t **ag, int flag, struct in_addr *addr_FromIp_P, struct in_addr *addr_EndIp_P)
{
    unsigned int i = 0U;
    int ret = 0;/*����ֵ*/
    if (flag == 2)/*2ΪԴ��ַ*/
    {
	/*Դ��ַ*/
	i = getPositionByName(ag, "source");
    }
    else /*1ΪĿ�ĵ�ַ*/
    {
	/*Ŀ�ĵ�ַ*/
	i = getPositionByName(ag, "dest");
    }
    
    if (i > 30U)/*û�ж�Ӧ�������˳�*/
    {
	return 1;
    }
    if((strcmp(ag[i]->value, "any")) == 0)
    {
	/*any����£����ǲ����ã�������0.0.0.0--0.0.0.0*/
	if ((inet_aton("0.0.0.0", addr_FromIp_P) == 0) || (inet_aton("0.0.0.0", addr_EndIp_P) == 0))
	{
	    ret = 1;
	}
    } 
    /*host����£���ʼ�ͽ�������ͬһ��ַ*/
    else if(0 == strcmp(ag[i]->value, "host"))
    {
	if ((inet_aton(clish_parg__get_value((const clish_parg_t *)ag[i+flag]), addr_FromIp_P) == 0) || (inet_aton(clish_parg__get_value((const clish_parg_t *)ag[i+flag]), addr_EndIp_P) == 0))
	{
	    ret = 1;
	}	 
    }
    else
    {
	if (flag == 2)
	{
	    /*�ֶ�����ip����������ipת��*/
	    if (getIPRange(ag, addr_FromIp_P, addr_EndIp_P, "sourceaddressstart", "sourceaddressend") == 1)
	    {
		ret = 1;
	    }
	}
	else
	{
	    if (getIPRange(ag, addr_FromIp_P, addr_EndIp_P, "destaddressstart", "destaddressend") == 1)
	    {
		ret = 1;
	    }
	}
    }
    return ret;
}


/********************************************************************
 * �������ܣ��õ�Ŀ�Ķ˿�
 * ����ʱ�䣺2012-12-17
 ********************************************************************/
static int getDestPortValue(clish_parg_t **ag, int *dstFromPort_P, int *dstEndPort_P)
{
    unsigned int i = 0U ;
    i = getPositionByName(ag, "destport");
    /*������ֵ�ж���û�ҵ�*/
    if (i < 30U)
    {	
	if (strcmp(ag[i]->value, "destportstart") == 0)
	{
	    *dstFromPort_P = strtol(ag[i+1]->value, NULL, 0);
            *dstEndPort_P = strtol(ag[i+2]->value, NULL, 0);
	}
	else if (0 == strcmp(ag[i]->value, "any"))
	{
	    *dstFromPort_P = 1;
	    *dstEndPort_P = 65535;
	}
	else if (0 == strcmp(ag[i]->value, "epmap")) 
	{
	    *dstFromPort_P = 135;
	    *dstEndPort_P = 135;
	}
	else if (0 == strcmp(ag[i]->value, "finger")) 
	{
	    *dstFromPort_P = 79;
	    *dstEndPort_P = 79;
	}
	else if (0 == strcmp(ag[i]->value, "ftp"))
	{
	    *dstFromPort_P = 21;
	    *dstEndPort_P = 21;
	}
	else if (0 == strcmp(ag[i]->value, "h.323"))
	{
	    *dstFromPort_P = 1720;
	    *dstEndPort_P = 1720;
	}
	else if (0 == strcmp(ag[i]->value, "https"))
	{
	    *dstFromPort_P = 443;
	    *dstEndPort_P = 443;
	}
	else if (0 == strcmp(ag[i]->value, "ms-ds"))
	{
	    *dstFromPort_P = 445;
	    *dstEndPort_P = 445;
	}
	else if (0 == strcmp(ag[i]->value, "ms-sql-m"))
	{
	    *dstFromPort_P = 1434;
	    *dstEndPort_P = 1434;
	}
	else if (0 == strcmp(ag[i]->value, "ms-sql-s"))
	{
	    *dstFromPort_P = 1433;
	    *dstEndPort_P = 1433;
	}
	else if (0 == strcmp(ag[i]->value, "ms-ts"))
	{
	    *dstFromPort_P = 3389;
	    *dstEndPort_P = 3389;
	}
	else if (0 == strcmp(ag[i]->value, "msnp"))
	{
	    *dstFromPort_P = 1863;
	    *dstEndPort_P = 1863;
	}
	else if (0 == strcmp(ag[i]->value, "netbios-ssn"))
	{
	    *dstFromPort_P = 139;
	    *dstEndPort_P = 139;
	}
	else if (0 == strcmp(ag[i]->value, "pop3"))
	{
	    *dstFromPort_P = 110;
	    *dstEndPort_P = 110;
	}
	else if (0 == strcmp(ag[i]->value, "pptp"))
	{
	    *dstFromPort_P = 1723;
	    *dstEndPort_P = 1723;
	}
	else if (0 == strcmp(ag[i]->value, "smtp"))
	{
	    *dstFromPort_P = 25;
	    *dstEndPort_P = 25;
	}
	else if (0 == strcmp(ag[i]->value, "sql*net"))
	{
	    *dstFromPort_P = 66;
	    *dstEndPort_P = 66;
	}
	else if (0 == strcmp(ag[i]->value, "ssh"))
	{
	    *dstFromPort_P = 22;
	    *dstEndPort_P = 22;
	}
	else if (0 == strcmp(ag[i]->value, "telnet"))
	{
	    *dstFromPort_P = 23;
	    *dstEndPort_P = 23;
	}
	else if (0 == strcmp(ag[i]->value, "web"))
	{
	    *dstFromPort_P = 80;
	    *dstEndPort_P = 80;
	}
	else if (0 == strcmp(ag[i]->value, "bootpc"))
	{
	    *dstFromPort_P = 68;
	    *dstEndPort_P = 68;
	}
	else if (0 == strcmp(ag[i]->value, "bootps"))
	{
	    *dstFromPort_P = 67;
	    *dstEndPort_P = 67;
	}
	else if (0 == strcmp(ag[i]->value, "dns"))
	{
	    *dstFromPort_P = 53;
	    *dstEndPort_P = 53;
	}
	else if (0 == strcmp(ag[i]->value, "isakmp"))
	{
	    *dstFromPort_P = 500;
	    *dstEndPort_P = 500;
	}
	else if (0 == strcmp(ag[i]->value, "l2tp"))
	{
	    *dstFromPort_P = 1701;
	    *dstEndPort_P = 1701;
	}
	else if (0 == strcmp(ag[i]->value, "netbios-ns"))
	{
	    *dstFromPort_P = 137;
	    *dstEndPort_P = 137;
	}
	else if (0 == strcmp(ag[i]->value, "netbios-dgm"))
	{
	    *dstFromPort_P = 138;
	    *dstEndPort_P = 138;
	}
	else if (0 == strcmp(ag[i]->value, "ntp"))
	{
	    *dstFromPort_P = 123;
	    *dstEndPort_P = 123;
	}
	else if (0 == strcmp(ag[i]->value, "snmp"))
	{
	    *dstFromPort_P = 161;
	    *dstEndPort_P = 161;
	}
	else if (0 == strcmp(ag[i]->value, "snmptrap"))
	{
	    *dstFromPort_P = 162;
	    *dstEndPort_P = 162;
	}
	else if (0 == strcmp(ag[i]->value, "tftp"))
	{
	    *dstFromPort_P = 69;
	    *dstEndPort_P = 69;
	}
    }
    return 0;/*û��ʧ�������Ĭ�Ϸ���0*/
}


/******************************************
 * �������ܣ�ɾ���ѱ���ķ��ʿ��Ʋ���
 * ����ʱ�䣺2013-01-04
 **********************************************/
static int clish_no_access_list(clish_context_t *context, const lub_argv_t *argv)
{
    MibProfileType mibType = MIB_PROF_FIRE_WALL;
    FireWallProfile *prof= NULL;
    struProfAlloc *profList  = NULL;

    int instIndex = 0;
    unsigned int i = 0U;
    char policyName[60];/*ʵ����*/
    char enable[8];/*������ֹ*/
    char action[8];/*����*/
    int protocol = 0;/*Э��*/
    int dstFromPort = 0;/*Ŀ���ַ��ʼ�˿�*/
    int dstEndPort = 0;/*Ŀ���ַ�����˿�*/
    unsigned int groupDay = 0U;
    char timeStart[12];
    char timeStop[12];
    struct in_addr addr_SrcFromIp, addr_SrcEndIp, addr_DstFromIp, addr_DstEndIp;

    unsigned index = 0U;
    unsigned agc = 0U;
    clish_parg_t* ag[30];
    clish_pargv_t *args = (clish_pargv_t *)context->pargv;
    agc = clish_pargv__get_count(args);
    for (index = 0U; index < agc; index++){
        ag[index] = clish_pargv__get_parg(args,index);
#if 0
        printf("ag[%d]->param->name=%s\n",index,clish_parg__get_name((const clish_parg_t *)ag[index]
));
        printf("ag[%d]->value=%s\n",index,clish_parg__get_value((const clish_parg_t *)ag[index]));
        printf("\n");
#endif
    }

    /*������*/
    getValueByName(ag, "policyname", policyName);
    instIndex = ProfGetInstIndexByName(mibType, policyName);
    /*û�ж�Ӧ��ʵ��,��ӡ������Ϣ�˳�*/
    if (instIndex == PROFFAIL)
    {
        printf("The policy name %s is nonexistent\n", policyName);
        return 1;
    }
    
    /*�Ƿ�����*/
    memset(enable, 0, sizeof(enable));
    getValueByName(ag, "switch", enable);
    
    /*���� ��ֹ������*/
    memset(action, 0, sizeof(action));
    getValueByName(ag, "action", action);

    /*Э������*/
    getProtocolType(ag, &protocol);

    if (IPTransducer(ag, 2, &addr_SrcFromIp, &addr_SrcEndIp) == 1)
    {
        printf("Fail,please try again!\n");
        return 1;
    }

    if (IPTransducer(ag, 1, &addr_DstFromIp, &addr_DstEndIp) == 1)
    {
        printf("Fail,please try again!\n");
        return 1;
    }

    getDestPortValue(ag, &dstFromPort, &dstEndPort);

    prof = (FireWallProfile*)ProfGetInstPointByIndex(mibType, instIndex);

    if ((strcmp(enable, "open") == 0) && (prof->head.active == FALSE))
    {
	printf("parameter error\n");
	return 1;
    }
    else if ((strcmp(enable, "close") == 0) && (prof->head.active == TRUE))
    {
	printf("parameter error\n");
	return 1;
    }
    
    if ((strcmp(action, "permit") == 0) && (prof->Status == 0))
    {
	printf("parameter error\n");
	return 1;
    }
    else if ((strcmp(action, "deny") == 0) && (prof->Status == 1))
    {
	printf("parameter error\n");
	return 1;
    }

    if ((prof->FilterType != TYPE_FILTER_IP) || (prof->IpFilter.Protocol != protocol))
    {
	printf("parameter error\n");
        return 1;
    }

    if ((prof->addrTypeEn != ADDR_IPRANGE) || (prof->srcFromIp != addr_SrcFromIp.s_addr) || (prof->srcEndIp != addr_SrcEndIp.s_addr))
    {
        printf("parameter error\n");
	return 1;
    }

    i = getPositionByName(ag, "srcportstart");
    if (i < 30U)
    {
	if ((prof->IpFilter.srcFromPort != strtol(ag[i]->value, NULL, 0)) || (prof->IpFilter.srcEndPort != strtol(ag[i+1]->value, NULL, 0)))
	{
	    printf("parameter error\n");
	    return 1;
	}
    }

    if ((prof->IpFilter.dstFromIp != addr_DstFromIp.s_addr) || (prof->IpFilter.dstEndIp != addr_DstEndIp.s_addr) || (prof->IpFilter.dstFromPort != dstFromPort) || (prof->IpFilter.dstEndPort != dstEndPort))
    {
	printf("parameter error\n");
	return 1;
    }

    /*��Чʱ��*/
    if (getTimeRange(ag, timeStart, timeStop) == 1)
    {
	printf("Fail, please try again\n");
	return 1;
    }

    /*���ڣ�������7λ�������ַ���ת��Ϊunsigned int*/
    groupDay = strToDay(ag);
    if (groupDay == 999U)/*û�ҵ�*/
    {
	printf("Fail, please try again\n");
	return 1;
    }

    if ((prof->fw_day != groupDay) || (strcmp(prof->fw_timestart, timeStart) != 0) || (strcmp(prof->fw_timestop, timeStop) != 0))
    {
	printf("parameter error\n");
	return 1;
    }


    ProfBackupByName(mibType, PROF_CHANGE_DEL, policyName, &profList);
    ProfDelInstByName(mibType, policyName);

    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();

    return 0;
}


/**********************************************************************
 * �������ܣ�access-list�����ӷ��ʿ��Ʋ���
 * ����ʱ�䣺2012-12-04
 ********************************************************************/
static int clish_access_list(clish_context_t *context, const lub_argv_t *argv)
{
    MibProfileType mibType = MIB_PROF_FIRE_WALL;
    FireWallProfile *prof= NULL;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction = PROF_CHANGE_EDIT;
    unsigned int i = 0U;
    char policyName[60];/*ʵ����*/
    char enable[8];/*������ֹ*/
    char action[8];/*����*/
    int protocol = 0;/*Э��*/
    int dstFromPort = 0;/*Ŀ���ַ��ʼ�˿�*/
    int dstEndPort = 0;/*Ŀ���ַ�����˿�*/
    unsigned int groupDay = 0U;
    int ret = 0;/*����ֵ*/
    char timeStart[12];
    char timeStop[12];
    struct in_addr addr_SrcFromIp, addr_SrcEndIp, addr_DstFromIp, addr_DstEndIp;
    int instIndex = 0;
    unsigned index = 0U;
    unsigned agc = 0U;
    clish_parg_t* ag[30];
    clish_pargv_t *args = (clish_pargv_t *)context->pargv;

    agc = clish_pargv__get_count(args);

    /*�õ��������*/
    for (index = 0U; index < agc; index++){
	ag[index] = clish_pargv__get_parg(args,index);
#if 0
	printf("ag[%d]->param->name=%s\n",index,clish_parg__get_name((const clish_parg_t *)ag[index]));
	printf("ag[%d]->value=%s\n",index,clish_parg__get_value((const clish_parg_t *)ag[index]));
	printf("\n");
#endif
    }

    getValueByName(ag, "policyname", policyName);
    /*���ղ���������ʵ��*/
    instIndex = ProfGetInstIndexByName(mibType, policyName);
    /*���ʵ������������ӣ����ھ��޸�*/
    if(instIndex == PROFFAIL)
    {
	profAction = PROF_CHANGE_ADD;
    }
 
   /*�Ƿ�����*/
    memset(enable, 0, sizeof(enable));
    getValueByName(ag, "switch", enable);
    
    /*���� ��ֹ������*/
    memset(action, 0, sizeof(action));
    getValueByName(ag, "action", action);

    /*Э������*/
    getProtocolType(ag, &protocol);
        
    /*ת��Դip��ַ��ʧ���˳�*/
    if (IPTransducer(ag, 2, &addr_SrcFromIp, &addr_SrcEndIp) == 1)
    {
	printf("Source IP address transform fail, please try again!\n");
	ret = 1;
    }
    else
    {
	/*ת��Ŀ��IP��ַ��ʧ���˳�*/
	if (IPTransducer(ag, 1, &addr_DstFromIp, &addr_DstEndIp) == 1)
	{
	    printf("Destination IP address transform fail, please try again!\n");
	    ret = 1;
	}
	else
	{    
	    /*Ŀ�Ķ˿�*/
	    getDestPortValue(ag, &dstFromPort, &dstEndPort);
	    if (profAction == PROF_CHANGE_ADD)
	    {
		/*��������������ʵ��*/
		instIndex = ProfNewInst(mibType, policyName, FALSE);
	    }
	    /*���ʧ�ܣ���ӡ������Ϣ"�Ѵ������Ŀ"���Ƴ�*/
	    if(instIndex == PROFFAIL) 
	    {
		printf("Maximum number of entries has been reached.\n");
		ret = 1;
	    }
	    else
	    {
		ProfBackupByIndex(mibType, profAction, instIndex, &profhead);
		prof = (FireWallProfile*)ProfGetInstPointByIndex(mibType, instIndex);
		ProfSetNameByPoint((Pointer *)prof, policyName);

		if (strcmp(enable, "open") == 0)
		{
		    prof->head.active = TRUE;
		}
		else
		{
		    prof->head.active = FALSE;
		}


		if (strcmp(action, "permit") == 0)
		{
		    prof->Status = 1;/*����*/  
		}
		else
		{
		    prof->Status = 0;
		}

		/*��������ֻ��IPһ��*/
		prof->FilterType = TYPE_FILTER_IP; 
		prof->IpFilter.Protocol = protocol;
		prof->addrTypeEn = ADDR_IPRANGE;/*����Դ��ַ��ʾ��ʽΪ����*/
		prof->srcFromIp = addr_SrcFromIp.s_addr;
		prof->srcEndIp = addr_SrcEndIp.s_addr;

		i = getPositionByName(ag, "srcportstart");
		if (i < 30U)
		{
		    prof->IpFilter.srcFromPort = strtol(ag[i]->value, NULL, 0);
		    prof->IpFilter.srcEndPort = strtol(ag[i+1]->value, NULL, 0);
		}

		prof->IpFilter.dstFromIp = addr_DstFromIp.s_addr;
		prof->IpFilter.dstEndIp = addr_DstEndIp.s_addr;
		prof->IpFilter.dstFromPort = dstFromPort;
		prof->IpFilter.dstEndPort = dstEndPort;

		/*��Чʱ��*/
		if (getTimeRange(ag, timeStart, timeStop) == 1)
		{
		    printf("Fail, please try again\n");
		    return 1;
		}

		/*���ڣ�������7λ�������ַ���ת��Ϊunsigned int*/
		groupDay = strToDay(ag);
		if (groupDay == 999U)/*û�ҵ�*/
		{
		    printf("Fail, please try again\n");
		    return 1;
		}

		prof->fw_day = groupDay;
		strcpy((char *)prof->fw_timestart, timeStart); /*����ʱ��Ϊȫ��*/
		strcpy((char *)prof->fw_timestop, timeStop); 

		/*����ǰ�����Ƶ���󲢱���*/    
		moveToLast((char *)(prof->head.name));

		ProfFreeAllocList(profhead);
		nvramWriteCommit();
	    }
	}
    }
    return ret;
}


#endif


/****************************************************************
 * ��������:˼��������������ַ���ipת���ɽṹ���õ�unsigned int
 * ����ʱ�䣺2012-12-27
 *************************************************************/
static int getIPRange(clish_parg_t **ag, struct in_addr *pfromIP, struct in_addr *pendIP, char *ipStartParamName, char *ipStopParamName)
{
    unsigned int ipStartParamPosition = 0U;/*IP��Χ��ʼ�������±�*/
    unsigned int ipStopParamPosition = 0U;/*IP��Χ�����������±�*/
    ipStartParamPosition = getPositionByName(ag, ipStartParamName);
    ipStopParamPosition = getPositionByName(ag, ipStopParamName);
    if ((ipStartParamPosition > 30U) || (ipStopParamPosition > 30U))/*û�ҵ���Ӧ�Ĳ���������1�˳�*/
    {
	return 1;
    }
    /*ת��IP*/
    if ((inet_aton(ag[ipStartParamPosition]->value, pfromIP) == 0) || (inet_aton(ag[ipStopParamPosition]->value, pendIP) == 0))
    {
        printf("Fail,please try again\n");/*ת��ʧ�ܣ���ӡ������Ϣ*/
        return 1;
    }
    return 0;
}


/****************************************************************
 * �������ܣ��������7λ�����ַ���ת����ʵ����Ҫ���޷���intֵ(������)
 * ����ʱ�䣺2012-12-27
 *************************************************************/
static unsigned int strToDay(clish_parg_t **ag)
{
    unsigned int i = 0U;
    unsigned int dateDecimal = 0U;/*ʮ����1111111*/
    unsigned int dateBinary = 0U;/*������1111111����127U*/
    unsigned int multiple = 1U;
    
    i = getPositionByName(ag, "datemode");
    /*�жϷ���ֵ��999U��ʾû�ҵ�*/
    if (i < 30U)
    {
	if (strcmp(ag[i]->value, "any") == 0)
	{
	    return 127U;
	}
	else
	{
	    dateDecimal = strtoul(ag[i+1]->value, NULL, 0);
	    for (;dateDecimal != 0U; dateDecimal /= 10U, multiple *= 2U)
	    {
		dateBinary += (dateDecimal % 10U) * multiple;
	    }
	    return dateBinary;
	}
    }
    return 999U;   
}

/******************************************************************
 * �������ܣ�ȡ�������ʱ�䷶Χ
 * ����ʱ�䣺2012-12-27
 * ����ֵ��1--ʧ�� 0--�ɹ�
 ***************************************************************/
static int getTimeRange(clish_parg_t **ag, char *timeStart, char *timeStop)
{
    unsigned int i = 0U;
    i = getPositionByName(ag, "timemode");
    if (i > 30U)
    {
	return 1;
    }
    if (strcmp(ag[i]->value, "any") == 0)
    {
	strcpy(timeStart, "All");
	strcpy(timeStop, "0");
    }
    else
    {
	strcpy(timeStart, ag[i+1]->value);
	strcpy(timeStop, ag[i+2]->value);
    }
    return 0;
}


/***************************************************************
 * �������ܣ�����nameֵ��ag�����в��Ҷ�Ӧ�±꣬ȡ�����е�ֵ����buffer��
 * ����ʱ�䣺2012-12-17
 * ������ag--����������飬name--�����в��������֣�buffer--��Ҫд������ֵ���ַ���
 * ����ֵ��0--�ɹ� 1--ʧ��
 * ��ע��ֻ������һ���еĲ������������ڿ���û�еĲ���
 ***************************************************************/
static int getValueByName(clish_parg_t **ag, char *name, char *buffer)
{
    unsigned int i = 0U;
    i = getPositionByName(ag, name);
    /*�жϷ���ֵ��999U��ʾû�ҵ�*/
    if (i < 30U)
    {
	strcpy(buffer, ag[i]->value);
    }
    return 0;/*����ʧ�ܣ�ֱ�ӷ���0*/
}


#if (GROUP_BASE == FYES)
#ifdef CONFIG_UTT_NETFILTER
/******************************************************************
 * �������ܣ��õ���������ٲ���
 * ����ʱ�䣺2012-12-27
 ****************************************************************/
static int getlimitPolicy(clish_parg_t **ag)
{
    unsigned int i = 0U;
    i = getPositionByName(ag, "ratelimitmode");
    if (strcmp(ag[i]->value, "Each") == 0)
    {
	return EXCLUSIVE_RATE;/*����*/
    }
    else
    {
	return SHARE_RATE;/*����*/
    }
}

/*******************************************************************
 * �������ܣ��жϵ�ǰ�Ƿ���LAN����ִ������
 * ����ʱ�䣺2012-12-27
 * ����ֵ��1--����LAN���� 0--��LAN����
 ***************************************************************/
static int isLANInterface(clish_parg_t **ag)
{
    unsigned int ifnum = 0U;
    char array[4];
    /*ȡ�ö˿ں��ֶ��±�*/
    ifnum = getPositionByName(ag, "_cur_pwd");
    if (ifnum == 999U)/*û�ҵ�*/
    {
	return 1;
    }
    /*�ж��Ƿ���LAN�£�ֻ��LAN����Ч*/
    memset(array, 0, sizeof(array));
    sscanf(ag[ifnum]->value, "%*s%*s%s", array);
    if (strtol(array, NULL, 10) != 0)/*����lan�ڴ�ӡ������Ϣ�˳�*/
    {
	printf("Command only effective in LAN port\n");
	return 1;
    }
    
    return 0;
}
    

/****************************************************************
 * �������ܣ�rate-limit����,��ϸ������ 
 * ����ʱ�䣺2012-12-11
 *****************************************************************/
static int clish_rate_limit(clish_context_t *context, const lub_argv_t *argv)
{
    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    struProfAlloc *profhead = NULL;
    GroupBaseType webGroupType = GBT_FINE_LIMIT;
    GroupBaseProfile *prof = NULL;
    ProfChangeType profAction = PROF_CHANGE_SPECIAL;    

    int instIndex = 0;
    unsigned int i = 0U;
    int max = 0, min = 0;/*��ϸ������ʵ�������С*/
    char policyName[60];/*ʵ����*/
    unsigned int groupDay = 0U;
    int ret = 0;/*����ֵ*/
    char timeStart[12];
    char timeStop[12];
    struct in_addr fromIP, endIP;
    unsigned int limitPolicy = 0U;
    
    unsigned index = 0U;
    unsigned agc = 0U;
    clish_parg_t* ag[30];
    clish_pargv_t *args = (clish_pargv_t *)context->pargv;
    agc = clish_pargv__get_count(args);
    for (index = 0U; index < agc; index++){
	ag[index] = clish_pargv__get_parg(args,index);
#if 0
	printf("ag[%d]->param->name=%s\n",index,clish_parg__get_name((const clish_parg_t *)ag[index]));
	printf("ag[%d]->value=%s\n",index,clish_parg__get_value((const clish_parg_t *)ag[index]));
	printf("\n");
#endif
    }    

    /*�жϵ�ǰ�Ƿ���LAN����*/
    if (isLANInterface(ag) == 1)
    {
	return 1;
    }

    getValueByName(ag, "policyname", policyName);

    /*��Чʱ��*/
    if (getTimeRange(ag, timeStart, timeStop) == 1)
    {
	printf("Fail, please try again\n");
	return 1;
    }

    /*���ڣ�������7λ�������ַ���ת��Ϊunsigned int*/
    groupDay = strToDay(ag);
    if (groupDay == 999U)/*û�ҵ�*/
    {
	printf("Fail, please try again\n");
	return 1;
    }
    
    /*ת��IP��ʧ���˳�*/
    if (getIPRange(ag, &fromIP, &endIP, "IPstartaddress", "IPendaddress") == 1)
    {
	return 1;
    }

    /*���ٲ���*/
    limitPolicy = getlimitPolicy(ag);

    ProfInstLowHigh(mibType, &max, &min);

    uttSemP(SEM_UTTNF_NO, 0);/*������ֹ��ͻ*/

    instIndex = ProfGetInstIndexByName(mibType, policyName);
    if(instIndex == PROFFAIL)
    {
	if(ProfCountInstByValue(mibType, OFFANDSIZE(GroupBaseProfile, groupType), 0u, (Pointer)&webGroupType) >= MAX_FINE_LIMIT_PROFILES)
	{	    
	    printf("Maximum number of entries has been reached\n");
	    return 1;
	}
	instIndex = ProfNewInst(mibType, policyName, FALSE);
	profAction = PROF_CHANGE_ADD;
    }
    ProfBackupByIndex(mibType, profAction, instIndex, &profhead);
    prof = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, instIndex);
    
    prof->groupType = webGroupType;/*���;�ϸ������*/
    ProfSetNameByPoint(prof, policyName);
    prof->day = groupDay;/*����*/ 
    strcpy(prof->timeStart, timeStart);
    strcpy(prof->timeStop, timeStop);
    prof->addrTypeEn = ADDR_IPRANGE;    
    prof->ipStart = fromIP.s_addr; 
    prof->ipEnd = endIP.s_addr;
    prof->limitPolicy = limitPolicy;
    prof->uRate = strtoul(ag[getPositionByName(ag, "uprate")]->value, NULL, 0);/*�ϴ�����*/
    prof->dRate = strtoul(ag[getPositionByName(ag, "downrate")]->value, NULL, 0);/*��������*/

    uttSemV(SEM_UTTNF_NO, 0);/*�ͷ���*/
#if 0    
    if (moveLastToFirst(prof->head.name) != 1)
    {
#endif
    ProfUpdate(profhead);
#if 0   
    }
#endif
    ProfFreeAllocList(profhead);
    nvramWriteCommit();

    return ret;    
}

/****************************************************************
 * �������ܣ�rate-limitɾ������,��ϸ������ 
 * ����ʱ�䣺2012-12-27
 *****************************************************************/
static int clish_no_rate_limit(clish_context_t *context, const lub_argv_t *argv)
{
    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    GroupBaseProfile *prof = NULL;
    struProfAlloc *profList  = NULL;

    int instIndex = 0;
    char policyName[60];/*ʵ����*/
    unsigned int groupDay = 0U;
    char timeStart[12];
    char timeStop[12];
    struct in_addr fromIP, endIP;
    unsigned int limitPolicy = 0U;
    
    unsigned index = 0U;
    unsigned agc = 0U;
    clish_parg_t* ag[30];
    clish_pargv_t *args = (clish_pargv_t *)context->pargv;
    agc = clish_pargv__get_count(args);
    for (index = 0U; index < agc; index++){
	ag[index] = clish_pargv__get_parg(args,index);
#if 0
	printf("ag[%d]->param->name=%s\n",index,clish_parg__get_name((const clish_parg_t *)ag[index]));
	printf("ag[%d]->value=%s\n",index,clish_parg__get_value((const clish_parg_t *)ag[index]));
	printf("\n");
#endif
    }

    /*�ж��Ƿ���LAN����*/    
    if (isLANInterface(ag) == 1)
    {
	return 1;
    }

    /*������*/
    getValueByName(ag, "policyname", policyName);
    instIndex = ProfGetInstIndexByName(mibType, policyName);
    /*���û�ж�Ӧ��ʵ�����ӡ������Ϣ�˳�*/
    if (instIndex == PROFFAIL)
    {
        printf("The policy name %s is nonexistent\n", policyName);
	return 1;
    }
   
    /*�õ�����ip��ַ��Χ*/ 
    if (getIPRange(ag, &fromIP, &endIP, "IPstartaddress", "IPendaddress") == 1)
    {
	printf("Fail,please try again!\n");
	return 1;
    }
    prof = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, instIndex);
    /*�뱣���ip��һ�£��˳�*/
    if ((prof->ipStart != fromIP.s_addr) || (prof->ipEnd != endIP.s_addr))
    {
	printf("parameter error\n");
	return 1;
    }

    /*���ٲ���*/
    limitPolicy = getlimitPolicy(ag);
    if (prof->limitPolicy != limitPolicy)/*���ٲ��Բ�һ�£��˳�*/
    {
	printf("parameter error\n");
	return 1;
    }
    
    /*�ж��������ѱ����ٶ��Ƿ�һ��*/
    if ((prof->uRate != strtoul(ag[getPositionByName(ag, "uprate")]->value, NULL, 0)) || (prof->dRate != strtoul(ag[getPositionByName(ag, "downrate")]->value, NULL, 0)))
    {
	printf("parameter error\n");
	return 1;
    }

    /*��Чʱ��*/
    if (getTimeRange(ag, timeStart, timeStop) == 1)
    {
	printf("parameter error\n");
	return 1;
    }
    /*������7λ�������ַ���ת��Ϊunsigned int*/
    groupDay = strToDay(ag);/*����*/

    if ((strcmp(prof->timeStart, timeStart) != 0) || (strcmp(prof->timeStop, timeStop) != 0) || (prof->day != groupDay)) 
    {
	printf("parameter error\n");
	return 1;
    }

    if (prof->addrTypeEn == ADDR_IPRANGE)
    {
	uttSemP(SEM_UTTNF_NO,0);/*������ֹ��ͻ*/

	ProfBackupByName(mibType, PROF_CHANGE_DEL, policyName, &profList);
	ProfDelInstByName(mibType, policyName);
	
	uttSemV(SEM_UTTNF_NO,0);/*�ͷ���*/
	ProfUpdate(profList);
	ProfFreeAllocList(profList);
	nvramWriteCommit();
    }

    return 0;    
}
#endif
#endif

/********************************************************************
 * ������ clish_showRunningConfig
 * ���ܣ� ��ʾ����
 * ������ 2012-12-21
 * ������ klish�������
 * ���أ� ��
 * ����� ��
 * ���ߣ�  
 ********************************************************************/
static int clish_showRunningConfig(clish_context_t *context, const lub_argv_t *argv)
{
    ProfInit();
    char* buf = NULL;
    buf = cliConfCreat();
    if(buf != NULL)
    {
	printf(buf);
	cliConfFree(buf);
    }
    return 0;
}
/********************************************************************
 * ������ clish_show_flash
 * ���ܣ� ��ʾ����
 * ������ 2012-12-21
 * ������ klish�������
 * ���أ� ��
 * ����� ��
 * ���ߣ�  
 ********************************************************************/
static int clish_show_flash(clish_context_t *context, const lub_argv_t *argv)
{
    return 0;
}
#define CONFIG_FILE "Configure"
/********************************************************************
 * ������ clish_copy_to_remote
 * ���ܣ� �����õ���
 * ������ 2012-12-20
 * ������ klish�������
 * ���أ� ��
 * ����� ��
 * ���ߣ�  
 ********************************************************************/
static int clish_copy_to_remote(clish_context_t *context, const lub_argv_t *argv)
{
    char hostName[256];
    char *ret = NULL;
    char get=0x0;
    
    memset(hostName, 0, sizeof(hostName));
    SWORD_PRINTF("%s-%d: \n",__func__,__LINE__);
    
    doSystem("cliStConf > %s &",CONFIG_FILE);
    printf("Address or name of remote host []?");
    ret = gets(hostName);
    while(ret == NULL)
    {
	printf("Address or name of remote host []?");
	ret = gets(hostName);
    }
    printf("Source filename [Configure]\n");
    printf("Translate config to %s.[Y/N]?",hostName);
    get = getchar();
    if((get == 'Y')||(get == 'y')){
    if(strlen(hostName)!=0) 
    {
	doSystem("tftp -p -r %s %s:69",CONFIG_FILE,hostName);
    }
    }
#if 0
    converConfigToXml();
    nvramWriteCommit();
#endif
    return 0;
}
#if 1
#if (UTT_LICENSE == FYES)
/********************************************************************
 * ������ get_version_from_local_license(int *swVer, int *hwVer)
 * ���ܣ� ��ȡ�豸����Ӳ���汾��
 * ������ 2012-12-21
 * ������ 
 * ���أ� ��
 * ����� ��
 * ���ߣ�  
 ********************************************************************/
int get_version_from_local_license(int *swVer, int *hwVer)
{
    int i=0;
    unsigned int ver[3];
    int ver_max=0;
    char *pv=NULL;
    char buf[11];

    memset(buf, 0, 11);
    memset(ver, 0, sizeof(ver));
    
    strncpy(buf, UTT_SWVERSION,10);
    if(isdigit(buf[0])){
	pv = buf;
    }else{
	pv = &buf[1];
    }
    SWORD_PRINTF("%s-%d: %s\n",__func__,__LINE__,pv);
    if( pv = strtok(pv,".") ){
	if(!isdigit(pv[0])){
	    SWORD_PRINTF("Soft: format of Version is wrong\n");
	    exit(1);
	}
	ver[i]=strtol(pv,NULL,10);
	ver_max = (int) pow(2,SWVER_1);
	if(ver[i]>ver_max){
	    SWORD_PRINTF("Soft: Version number is too big\n");
	    exit(1);
	}
	while( pv = strtok(NULL,".") ){
	    i++;
	    if(i>2) {
		SWORD_PRINTF("Soft: Version too many items\n");
		exit(1);
	    }
	    ver[i]=strtol(pv,NULL,10);
	    switch(i){
	    case 0:
		ver_max = (int) pow(2,SWVER_1);
		break;
	    case 1:
		ver_max = (int) pow(2,SWVER_2);
		break;
	    case 2:
		ver_max = (int) pow(2,SWVER_3);
		break;
	    default:
		break;
	    }
	    if(ver[i]>ver_max){
		SWORD_PRINTF("Soft: Version number is too big\n");
		exit(1);
	    }
	}
    }
    *swVer = (unsigned int)( ( ver[0]<<(SWVER_3+SWVER_2) ) + ( ver[1]<<(SWVER_3) ) + ( ver[2]) );

    i = 0;
    pv=NULL;
    ver_max = 0;
    memset(buf, 0, 5);
    memset(ver, 0, sizeof(ver));
    strncpy(buf, UTT_HWMODEL,10);
    if(isdigit(buf[0])){
	pv = buf;
    }else{
	pv = &buf[1];
    }
    SWORD_PRINTF("%s-%d: buf=%s,pv=%s\n",__func__,__LINE__,buf,pv);
    if( pv = strtok(pv,".") ){
	if(!isdigit(pv[0])){
	    SWORD_PRINTF("Hard: format of Version is wrong\n");
	    exit(1);
	}
	ver[i]=strtol(pv,NULL,10);
	ver_max = (int) pow(2,HWVER_1);
	if(ver[i]>ver_max){
	    SWORD_PRINTF("Hard: Version number is too big\n");
	    exit(1);
	}

	while( pv =strtok(NULL,".") ){
	    i++;
	    if(i>1) {
		SWORD_PRINTF("Hard: Version too many items\n");
		exit(1);
	    }
	    ver[i]=strtol(pv,NULL,10);
	    switch(i){
	    case 0:
		ver_max = (int) pow(2,HWVER_1);
		break;
	    case 1:
		ver_max = (int) pow(2,HWVER_2);
		break;
	    default:
		break;
	    }
	    if(ver[i]>ver_max){
		SWORD_PRINTF("Hard: Version number is too big\n");
		exit(1);
	    }
	}
    }
    *hwVer = (unsigned int) ( ( ver[0] << (HWVER_2) ) + ver[1] );
    return 0;
}
#endif
/********************************************************************
 * ������ loadSoftWare
 * ���ܣ� �������
 * ������ 2012-12-21
 * ������ ���������
 * ���أ� ��
 * ����� ��
 * ���ߣ�  
 ********************************************************************/
static int loadSoftWare(char *fileName)
{
    FILE *fd = NULL;
    char *content = NULL;
    long int length = 0;
    int retNum = -1,rel_read = 0;

    image_header_t *head;
    image_header_t localHead;
    int headLen;
    struct utsname utsbuf;
    char *tmp;
#if (UTT_LICENSE == FYES)
    int swVer = 0;
    int hwVer = 0;
    int tmp_ver = 0;
    int swVer_local = 0;
    int hwVer_local = 0;
    char *pv = NULL;
    char buf[11];
    unsigned short localfunc = 0U;
    unsigned short loadfunc = 0U;
#endif

    if((fileName!=NULL)&&(strlen(fileName)!=0)){ 
    
    SWORD_PRINTF("%s-%d: fileName=%s\n",__func__,__LINE__,fileName);
    fd=fopen(fileName,"r");
    if(fd != NULL)
    {
	fseek(fd, 0, SEEK_END);
	length = ftell(fd);
	SWORD_PRINTF("%s-%d: length=%d\n",__func__,__LINE__,length);
	if( length <= sizeof(image_header_t) ){
	    fprintf(stderr,"updating file is invalid\n");
	    return;
	}
#if 1
	content = (char *)malloc(length+1);
	if(content == NULL){
	    fprintf(stderr,"malloc error\n");
	    return;
	}
	memset(content, 0, sizeof(content));
#endif
	fseek(fd, 0, SEEK_SET);
	SWORD_PRINTF("%s-%d: curlength=%d\n",__func__,__LINE__,ftell(fd));
	rel_read = fread(content, 1, length, fd);
	SWORD_PRINTF("%s-%d:length=%d, content[0 1 2]=%x %x %x,end 3 =%x %x %x\n",__func__,__LINE__,length,content[0],content[1],content[2],content[length-3],content[length-2],content[length-1]);
	SWORD_PRINTF("%s-%d: rel_read=%d\n",__func__,__LINE__,rel_read);

	head = (image_header_t*)content;

	flash_read_mtdname("Kernel", (char*)&localHead, 0, sizeof(image_header_t) );
	uname(&utsbuf);
	tmp = strtok(utsbuf.version, " ");

#if (CONFIG_X86_CPU == 1)
	if(strcmp(utsbuf.version, (char *)head->name) != 0) {
	    fprintf(stderr,"����ͺŲ�һ�£�������ѡ���������\n");
	    if(content != NULL)
		free(content);
	    return;
	}
#else
	if(strcmp(localHead.name, head->name) != 0) {
	    SWORD_PRINTF("%s-%d: VER ERROR:local[%s],web[%s]\n", __func__,__LINE__,localHead.name, head->name);
	    fprintf(stderr,"����ͺŲ�һ�£�������ѡ���������\n");
	    if(content != NULL)
		free(content);
	    return;
	}
#endif
#if (UTT_LICENSE == FYES)
    if(_judgeUttSN() == 1){
    memset(buf, 0, 11);
    tmp_ver = (int)ntohs( head->systemVer );
    swVer = (tmp_ver>>(HWVER_1+HWVER_2));
    hwVer = (tmp_ver & (0xFFFF>>(SWVER_1+SWVER_2+SWVER_3)));
    get_version_from_local_license(&swVer_local, &hwVer_local);
    strcpy(buf, UTT_FUNCVER);
    if(isdigit(buf[0])){
	pv = buf;
    }else{
	pv = &buf[1];
    }
    localfunc = (unsigned short)strtol(pv, NULL, 10);
    loadfunc = head->funcVer;
    if(loadfunc < localfunc) {
	printf("%s-%d: tmp_ver=%d, swVer=%d, hwVer=%d,sec_ver=%d,third_ver=%d,load_funcVer=%d\n",__func__,__LINE__,
	    tmp_ver,swVer,hwVer,swVer&0x70,swVer&0xF,head->funcVer);
	printf("%s-%d: swver_local=%d, hw_local=%d,localfuncVer=%d\n",__func__,__LINE__,swVer_local,hwVer_local,localfunc);
	fprintf(stderr,"�������ʧЧ��\n");
	if(content != NULL)
	    free(content);
	return;
    }
    }
#endif
	if (crc32 (0, (const unsigned char *)content+8, length - 8) != ntohl(head->imageCrc)) {
	    printf("%s-%d:creat_crc=%ld,load_crc=%ld\n",__func__,__LINE__,crc32(0, (const unsigned char *)content+8, length - 8),ntohl(head->imageCrc));
	    fprintf(stderr,"���У��ʹ���\n");
	    if(content != NULL)
		free(content);
	    return;
	}

	headLen = sizeof(image_header_t);
	SWORD_PRINTF("%s-%d: headLen=%d\n",__func__,__LINE__,headLen);

#if 0
	while(feof(fd)!=0){
	    rel_read = 0;
	    memset(content, 0, sizeof(content));
	    rel_read = fread(content, 1, 1024, fd);
	    printf("%s-%d: \n",__func__,__LINE__);
	    length += rel_read;
	}
	length = lseek(fd, 0, SEEK_END);
	if(length == 0){
	    return;
	}
	lseek(fd, 0, SEEK_SET);
#endif
	    fprintf(stderr,"updating software, please waiting ...\n");
#if (CONFIG_X86_CPU == 1)
	retNum = flash_write_mtdname("/dev/sda", content + headLen, 0, length - headLen);
#else
	    mtd_unlock("Kernel");
	    retNum = flash_write_mtdname("Kernel", content, 0, length);
#endif
	if(retNum == -1){
	    fprintf(stderr,"updating software error\n");
	}else{
	    fprintf(stderr,"write done\n");
	    SWORD_PRINTF("%s-%d: \n",__func__,__LINE__);
	    fprintf(stderr,"rebooting device ...\n");
	    sleep(3);
	    doSystem("reboot&");
	    SWORD_PRINTF("%s-%d: \n",__func__,__LINE__);
	}
	if(content != NULL)
	    free(content);
	return;
    }
    }
}

#endif
/********************************************************************
 * ������ loadDeviceConfigure
 * ���ܣ� �����õ���
 * ������ 2012-12-21
 * ������ �����ļ���
 * ���أ� ��
 * ����� ��
 * ���ߣ�  
 ********************************************************************/
static int loadDeviceConfigure(char *fileName)
{
	int i;
	int res;
	struct flock lock;
	FILE *fd = NULL;
	
    if((fileName != NULL)&&(strlen(fileName) != 0U)) {
	fd=fopen(fileName,"r");
	if(fd != NULL){
	clish_shell_unlock(fd_global);  /*frisr unock fd_global*/
	SWORD_PRINTF("%s-%d: fd_global=%d\n",__func__,__LINE__,fd_global);
	fprintf(stderr, "update Device Configure ...\n");
	doSystem("clish -x /etc_ro/clish-enable -w configure-view ./%s",fileName);  /*config the configure file*/

	/*then locak fd_global again*/
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	for (i = 0; i < CLISH_LOCK_WAIT; i++) {
		res = fcntl(fd_global, F_SETLK, &lock);	    /*set the file lock*/
		if (res != -1)
			break;
		if (EINTR == errno)
			continue;
		if ((EAGAIN == errno) || (EACCES == errno)) {
			if (0 == i)
				fprintf(stderr,
					"Try to get lock. Please wait...\n");
			sleep(1);
			continue;
		}
		break;
	}

	SWORD_PRINTF("%s-%d: fileName=%s\n",__func__,__LINE__,fileName);
	}
    }
    SWORD_PRINTF("%s-%d: \n",__func__,__LINE__);
    return 0;
}

/********************************************************************
 * ������ clish_copy_from_remote
 * ���ܣ� �����õ���
 * ������ 2012-12-20
 * ������ klish�������
 * ���أ� ��
 * ����� ��
 * ���ߣ�  
 ********************************************************************/
static int clish_copy_from_remote(clish_context_t *context, const lub_argv_t *argv)
{
    char hostName[256];
    char fileName_s[256];
    char fileName_d[256];
    char *ret = NULL;

    memset(hostName, 0, sizeof(hostName));
    memset(fileName_s, 0, sizeof(fileName_s));
    memset(fileName_d, 0, sizeof(fileName_d));
    
    SWORD_PRINTF("%s-%d: hostName=%s, fileName_s=%s, fileName_d=%s\n",__func__,__LINE__,hostName,fileName_s,fileName_d);   
    
    printf("Address or name of remote host []?");
    ret = gets(hostName);
    while(ret == NULL)
    {
	printf("Address or name of remote host []?");
	ret = gets(hostName);
    }
#if 0
    ret = NULL;
    printf("Source filename []?");
    ret = gets(fileName_s);
    while(ret == NULL)
    {
	printf("Source filename []?");
	ret = gets(fileName_s);
    }
#endif 
    ret = NULL;
    printf("Destination filename []?");
    ret = gets(fileName_d);
    while(ret == NULL)
    {
	printf("Destination filename []?");
	ret = gets(fileName_d);
    }
    if((strlen(hostName)!=0)&&(strlen(fileName_d)!=0)) 
    {
#if 0
	printf("----------\n");
	/* the exception proves the rule... */
	clish_shell_t *this = (clish_shell_t *)context->shell;

	argv = argv; /* not used */
	this->state = SHELL_STATE_CLOSING;
#endif
	SWORD_PRINTF("------\n");
	doSystem("tftp -g -r %s %s:69",fileName_d,hostName);

	if((strstr(fileName_d, UTTARCH)!=NULL)&&(strstr(fileName_d, ".bin")!=NULL))
	{
	    loadSoftWare(fileName_d);
	} else {
	    loadDeviceConfigure(fileName_d);
	}
    }
    
    doSystem("rm -f %s &", fileName_d);
    SWORD_PRINTF("%s-%d: hostName=%s, fileName_s=%s, fileName_d=%s\n",__func__,__LINE__,hostName,fileName_s,fileName_d);   
    return 0;
}

/**********************************************************************
 * �������ܣ�wan������pppoe����
 * ����ʱ�䣺2013-7-1
 * ����ֵ:1--ʧ�ܣ�0--�ɹ�
 ********************************************************************/
static int clish_dial(clish_context_t *context, const lub_argv_t *argv)
{
    int ret = 0;/*����ֵ*/
#if 1
    int wanIndex;
    unsigned index = 0U;
    unsigned agc = 0U;
    clish_parg_t* ag[10];
    clish_pargv_t *args = (clish_pargv_t *)context->pargv;
    
    agc = clish_pargv__get_count(args);
    
    /*�õ������������*/
    for (index = 0U; index < agc; index++){
	ag[index] = clish_pargv__get_parg(args, index);
	SWORD_PRINTF("ag[%d]->param->name=%s\n",index,clish_parg__get_name((const clish_parg_t *)ag[index]));
	SWORD_PRINTF("ag[%d]->value=%s\n",index,clish_parg__get_value((const clish_parg_t *)ag[index]));
	SWORD_PRINTF("\n");
    }
    /*�ж϶˿ں�*/
    index--;
    wanIndex = getCurInterface(ag[index]);
    if (wanIndex == -1)/*-1��ʾû�ҵ�*/
    {
	ret = 1;
    }

    doSystem("wan.sh %d",wanIndex);
#endif

    return ret;    
}

/*----------------------------------------------------------- */
