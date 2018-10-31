//-------------------------------------
// clish.cpp
//
// A console client for libclish
//-------------------------------------

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

extern "C"
{
#include <linux/autoconf.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "ifmisc.h"
#include "comExeFun.h"
#include <stdarg.h>
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif
#include <signal.h>
#if HAVE_LOCALE_H
#include <locale.h>
#endif
#if HAVE_LANGINFO_CODESET
#include <langinfo.h>
#endif

#include "lub/list.h"
#include "lub/system.h"
#include "clish/shell.h"
#include "clish/internal.h"

#ifndef VERSION
#define VERSION 1.2.2
#endif
#define QUOTE(t) #t
/* #define version(v) printf("%s\n", QUOTE(v)) */
#define version(v) printf("%s\n", v)

/* Hooks */
static clish_shell_hooks_t my_hooks = {
    NULL, /* don't worry about init callback */
    clish_access_callback,
    NULL, /* don't worry about cmd_line callback */
    clish_script_callback,
    NULL, /* don't worry about fini callback */
    clish_config_callback,
    clish_log_callback,
    NULL  /* don't register any builtin functions */
};

static void help(int status, const char *argv0);

/*--------------------------------------------------------- */
int main(int argc, char **argv)
{
	int running;
	int result = -1;
	clish_shell_t *shell = NULL;

	/* Command line options */
	const char *socket_path = KONFD_SOCKET_PATH;
	bool_t lockless = BOOL_FALSE;
	bool_t stop_on_error = BOOL_FALSE;
	bool_t interactive = BOOL_TRUE;
	bool_t quiet = BOOL_FALSE;
	bool_t utf8 = BOOL_FALSE;
	bool_t bit8 = BOOL_FALSE;
	bool_t log = BOOL_FALSE;
	const char *xml_path = getenv("CLISH_PATH");
	const char *view = getenv("CLISH_VIEW");
	const char *viewid = getenv("CLISH_VIEWID");
	FILE *outfd = stdout;
	bool_t istimeout = BOOL_FALSE;
	int timeout = 0;
	bool_t cmd = BOOL_FALSE; /* -c option */
	lub_list_t *cmds; /* Commands defined by -c */
	lub_list_node_t *iter;
	const char *histfile = "~/.clish_history";
	char *histfile_expanded = NULL;
	unsigned int histsize = 50;

	/* Signal vars */
	struct sigaction sigpipe_act;
	sigset_t sigpipe_set;

	static const char *shortopts = "hvs:ledx:w:i:bqu8okt:c:f:z:";
#ifdef HAVE_GETOPT_H
	static const struct option longopts[] = {
		{"help",	0, NULL, 'h'},
		{"version",	0, NULL, 'v'},
		{"socket",	1, NULL, 's'},
		{"lockless",	0, NULL, 'l'},
		{"stop-on-error", 0, NULL, 'e'},
		{"dry-run",	0, NULL, 'd'},
		{"xml-path",	1, NULL, 'x'},
		{"view",	1, NULL, 'w'},
		{"viewid",	1, NULL, 'i'},
		{"background",	0, NULL, 'b'},
		{"quiet",	0, NULL, 'q'},
		{"utf8",	0, NULL, 'u'},
		{"8bit",	0, NULL, '8'},
		{"log",		0, NULL, 'o'},
		{"check",	0, NULL, 'k'},
		{"timeout",	1, NULL, 't'},
		{"command",	1, NULL, 'c'},
		{"histfile",	1, NULL, 'f'},
		{"histsize",	1, NULL, 'z'},
		{NULL,		0, NULL, 0}
	};
#endif

	ProfInit();
	/* Ignore SIGPIPE */
	sigemptyset(&sigpipe_set);
	sigaddset(&sigpipe_set, SIGPIPE);
	sigpipe_act.sa_flags = 0;
	sigpipe_act.sa_mask = sigpipe_set;
	sigpipe_act.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sigpipe_act, NULL);

#if HAVE_LOCALE_H
	/* Set current locale */
	setlocale(LC_ALL, "");
#endif

	/* Var initialization */
	cmds = lub_list_new(NULL);

	/* Parse command line options */
	while(1) {
		int opt;
#ifdef HAVE_GETOPT_H
		opt = getopt_long(argc, argv, shortopts, longopts, NULL);
#else
		opt = getopt(argc, argv, shortopts);
#endif
		if (-1 == opt)
			break;
		switch (opt) {
		case 's':
			socket_path = optarg;
			break;
		case 'l':
			lockless = BOOL_TRUE;
			break;
		case 'e':
			stop_on_error = BOOL_TRUE;
			break;
		case 'b':
			interactive = BOOL_FALSE;
			break;
		case 'q':
			quiet = BOOL_TRUE;
			break;
		case 'u':
			utf8 = BOOL_TRUE;
			break;
		case '8':
			bit8 = BOOL_TRUE;
			break;
		case 'o':
			log = BOOL_TRUE;
			break;
		case 'd':
			my_hooks.script_fn = clish_dryrun_callback;
			break;
		case 'x':
			xml_path = optarg;
			break;
		case 'w':
			view = optarg;
			break;
		case 'i':
			viewid = optarg;
			break;
		case 'k':
			lockless = BOOL_TRUE;
			my_hooks.script_fn = clish_dryrun_callback;
			my_hooks.config_fn = NULL;
			break;
		case 't':
			istimeout = BOOL_TRUE;
			timeout = atoi(optarg);
			break;
		case 'c':
			char *str;
			cmd = BOOL_TRUE;
			quiet = BOOL_TRUE;
			str = strdup(optarg);
			lub_list_add(cmds, str);
			break;
		case 'f':
			if (!strcmp(optarg, "/dev/null"))
				histfile = NULL;
			else
				histfile = optarg;
			break;
		case 'z': {
				int itmp = 0;
				itmp = atoi(optarg);
				if (itmp <= 0) {
					fprintf(stderr, "Error: Illegal histsize option value.\n");
					help(-1, argv[0]);
					goto end;
				}
				histsize = itmp;
			}
			break;
		case 'h':
			help(0, argv[0]);
			exit(0);
			break;
		case 'v':
			version(VERSION);
			exit(0);
			break;
		default:
			help(-1, argv[0]);
			goto end;
			break;
		}
	}

	/* Validate command line options */
	if (utf8 && bit8) {
		fprintf(stderr, "The -u and -8 options can't be used together.\n");
		goto end;
	}

	/* Create shell instance */
	if (quiet)
		outfd = fopen("/dev/null", "w");
	shell = clish_shell_new(&my_hooks, NULL, NULL, outfd, stop_on_error);
	if (!shell) {
		fprintf(stderr, "Cannot run clish.\n");
		goto end;
	}
	/* Load the XML files */
	clish_shell_load_scheme(shell, xml_path);
	/* Set communication to the konfd */
	clish_shell__set_socket(shell, socket_path);
	/* Set lockless mode */
	if (lockless)
		clish_shell__set_lockfile(shell, NULL);
	/* Set interactive mode */
	if (!interactive)
		clish_shell__set_interactive(shell, interactive);
	/* Set startup view */
	if (view)
		clish_shell__set_startup_view(shell, view);
	/* Set startup viewid */
	if (viewid)
		clish_shell__set_startup_viewid(shell, viewid);
	/* Set UTF-8 or 8-bit mode */
	if (utf8 || bit8)
		clish_shell__set_utf8(shell, utf8);
	else {
#if HAVE_LANGINFO_CODESET
		/* Autodetect encoding */
		if (!strcmp(nl_langinfo(CODESET), "UTF-8"))
			clish_shell__set_utf8(shell, BOOL_TRUE);
#else
		/* The default is 8-bit if locale is not supported */
		clish_shell__set_utf8(shell, BOOL_FALSE);
#endif
	}
	/* Set logging */
	if (log)
		clish_shell__set_log(shell, log);
	/* Set idle timeout */
	if (istimeout)
		clish_shell__set_timeout(shell, timeout);
	/* Set history settings */
	clish_shell__stifle_history(shell, histsize);
	if (histfile)
		histfile_expanded = lub_system_tilde_expand(histfile);
	if (histfile_expanded)
		clish_shell__restore_history(shell, histfile_expanded);

	/* Set source of command stream: files or interactive tty */
	if(optind < argc) {
		int i;
		/* Run the commands from the files */
		for (i = argc - 1; i >= optind; i--)
			clish_shell_push_file(shell, argv[i], stop_on_error);
	} else {
		/* The interactive shell */
		clish_shell_push_fd(shell, fdopen(dup(fileno(stdin)), "r"),
			stop_on_error);
	}

	/* Execute startup */
	running = clish_shell_startup(shell);
	if (running) {
		fprintf(stderr, "Cannot startup clish.\n");
		goto end;
	}

	if (cmd) {
		/* Iterate cmds */
		for(iter = lub_list__get_head(cmds);
			iter; iter = lub_list_node__get_next(iter)) {
			char *str = (char *)lub_list_node__get_data(iter);
			clish_shell_forceline(shell, str, NULL);
		}
	} else {
		/* Main loop */
		result = clish_shell_loop(shell);
	}

end:
	/* Cleanup */
	if (shell) {
		if (histfile_expanded) {
			clish_shell__save_history(shell, histfile_expanded);
			free(histfile_expanded);
		}
		clish_shell_delete(shell);
	}
	if (quiet)
		fclose(outfd);

	/* Delete each cmds element */
	while ((iter = lub_list__get_head(cmds))) {
		lub_list_del(cmds, iter);
		free(lub_list_node__get_data(iter));
		lub_list_node_free(iter);
	}
	lub_list_free(cmds);
	ProfDetach();

	return result;
}

/*--------------------------------------------------------- */
/* Print help message */
static void help(int status, const char *argv0)
{
	const char *name = NULL;

	if (!argv0)
		return;

	/* Find the basename */
	name = strrchr(argv0, '/');
	if (name)
		name++;
	else
		name = argv0;

	if (status != 0) {
		fprintf(stderr, "Try `%s -h' for more information.\n",
			name);
	} else {
		printf("Usage: %s [options] [script_file] [script_file] ...\n", name);
		printf("CLI utility. Command line shell."
			"The part of the klish project.\n");
		printf("Options:\n");
		printf("\t-v, --version\tPrint version.\n");
		printf("\t-h, --help\tPrint this help.\n");
		printf("\t-s <path>, --socket=<path>\tSpecify listen socket "
			"\n\t\tof the konfd daemon.\n");
		printf("\t-l, --lockless\tDon't use locking mechanism.\n");
		printf("\t-e, --stop-on-error\tStop script execution on error.\n");
		printf("\t-b, --background\tStart shell using non-interactive mode.\n");
		printf("\t-q, --quiet\tDisable echo while executing commands\n\t\tfrom the file stream.\n");
		printf("\t-d, --dry-run\tDon't actually execute ACTION scripts.\n");
		printf("\t-x <path>, --xml-path=<path>\tPath to XML scheme files.\n");
		printf("\t-w <view_name>, --view=<view_name>\tSet the startup view.\n");
		printf("\t-i <vars>, --viewid=<vars>\tSet the startup viewid variables.\n");
		printf("\t-u, --utf8\tForce UTF-8 encoding.\n");
		printf("\t-8, --8bit\tForce 8-bit encoding.\n");
		printf("\t-o, --log\tEnable command logging to syslog's local0.\n");
		printf("\t-k, --check\tCheck input files for syntax errors only.\n");
		printf("\t-t <timeout>, --timeout=<timeout>\tIdle timeout in seconds.\n");
		printf("\t-c <command>, --command=<command>\tExecute specified command(s).\n\t\tMultiple options are possible.\n");
		printf("\t-f <path>, --histfile=<path>\tFile to save command history.\n");
		printf("\t-z <num>, --histsize=<num>\tCommand history size in lines.\n");
	}
}
