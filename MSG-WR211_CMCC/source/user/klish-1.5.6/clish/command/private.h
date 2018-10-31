/*
 * command.h
 */

#include "clish/command.h"

/*---------------------------------------------------------
 * PRIVATE TYPES
 *--------------------------------------------------------- */
struct clish_command_s {
	lub_bintree_node_t bt_node;
	char *name;
	char *text;
	clish_paramv_t *paramv;
	clish_action_t *action;
	clish_config_t *config;
	clish_view_t *view;
	char *viewid;
	char *detail;
	char *escape_chars;
	char *regex_chars;
	clish_param_t *args;
	const struct clish_command_s *link;
	clish_view_t *alias_view;
	char *alias;
	clish_view_t *pview;
	bool_t lock;
	bool_t interrupt;
	bool_t dynamic; /* Is command dynamically created */
};
