/*
 * clish_access_callback.c
 *
 *
 * callback hook to check whether the current user is a 
 * member of the specified group (access string)
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#ifdef HAVE_GRP_H
#include <grp.h>
#endif

#include "lub/string.h"
#include "lub/db.h"
#include "internal.h"

/*--------------------------------------------------------- */
bool_t clish_access_callback(const clish_shell_t * shell, const char *access)
{
	bool_t allowed = BOOL_FALSE; /* assume the user is not allowed */
#ifdef HAVE_GRP_H
	int num_groups;
	long ngroups_max;
	gid_t *group_list;
	int i;
	char *tmp_access, *full_access;
	char *saveptr;

	assert(access);
	full_access = lub_string_dup(access);
	ngroups_max = sysconf(_SC_NGROUPS_MAX) + 1;
	group_list = (gid_t *)malloc(ngroups_max * sizeof(gid_t));

	/* Get the groups for the current user */
	num_groups = getgroups(ngroups_max, group_list);
	assert(num_groups != -1);

	/* Now check these against the access provided */
	/* The external loop goes trough the list of valid groups */
	/* The allowed groups are indicated by a colon-separated (:) list. */
	for (tmp_access = strtok_r(full_access, ":", &saveptr);
		tmp_access; tmp_access = strtok_r(NULL, ":", &saveptr)) {
		/* The internal loop goes trough the system group list */
		for (i = 0; i < num_groups; i++) {
			struct group *ptr = lub_db_getgrgid(group_list[i]);
			if (!ptr)
				continue;
			if (0 == strcmp(ptr->gr_name, tmp_access)) {
				/* The current user is permitted to use this command */
				allowed = BOOL_TRUE;
				free(ptr);
				break;
			}
			free(ptr);
		}
	}

	lub_string_free(full_access);
	free(group_list);
#endif
	return allowed;
}

/*--------------------------------------------------------- */
