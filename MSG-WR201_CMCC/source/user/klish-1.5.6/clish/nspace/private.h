/*
 * nspace.h
 */
#include <sys/types.h>
#include <regex.h>

#include "clish/nspace.h"

/*---------------------------------------------------------
 * PRIVATE TYPES
 *--------------------------------------------------------- */
struct clish_nspace_s {
	lub_bintree_t tree;	/* Tree of command links */
	clish_view_t *view;	/* The view to import commands from */
	char *prefix;		/* if non NULL the prefix for imported commands */
	regex_t prefix_regex;
	bool_t help;
	bool_t completion;
	bool_t context_help;
	bool_t inherit;
	clish_command_t * prefix_cmd;
};
