/*
 * This file is released under the terms of the Artistic License.
 * Please see the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Labs, Inc.
 * Copyright (C) 2004 Alexey Stroganov & MySQL AB.
 *
 */

#include <string.h>

#include <nonsp_common.h>

void dbt2_escape_str(char *orig_str, char *esc_str)
{
	int i, j;

	if (orig_str && esc_str) {
		int len= strlen(orig_str);
		for (i = 0, j = 0; i < len; i++) {
			if (orig_str[i] == '\'') {
	      		esc_str[j++] = '\'';
	    	} else if (orig_str[i] == '\\') {
	      		esc_str[j++] = '\\';
	    	} else if (orig_str[i] == ')') {
	      		esc_str[j++] = '\\';
	    	}
	    	esc_str[j++] = orig_str[i];
	  }
	  esc_str[j] = '\0';
	}
}

int dbt2_init_values(char ** values, int max_values)
{
	int i;

	for (i=0; i<max_values; i++) {
		values[i]= NULL;
	}
	return 1;
}

int dbt2_free_values(char ** values, int max_values)
{
	int i;

	for (i=0; i<max_values; i++) {
		if (values[i]) {
      	free(values[i]);
      	values[i]= NULL;
		}
	}
	return 1;
}
