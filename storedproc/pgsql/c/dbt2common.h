/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2003-2008 Mark Wong & Open Source Development Labs, Inc.
 *
 * Based on TPC-C Standard Specification Revision 5.0 Clause 2.8.2.
 */

#ifndef _DBT2COMMON_H_
#define _DBT2COMMON_H_

const char s_dist[10][11] = {
	"s_dist_01", "s_dist_02", "s_dist_03", "s_dist_04", "s_dist_05",
	"s_dist_06", "s_dist_07", "s_dist_08", "s_dist_09", "s_dist_10"
};

void escape_str(char *, char *);

void escape_str(char *orig_str, char *esc_str)
{
	int i, j;
	for (i = 0, j = 0; i < strlen(orig_str); i++) {
		if (orig_str[i] == '\'') {
			esc_str[j++] = '\\';
		} else if (orig_str[i] == '\\') {
			esc_str[j++] = '\\';
		} else if (orig_str[i] == ')') {
			esc_str[j++] = '\\';
		}
		esc_str[j++] = orig_str[i];
	}
	esc_str[j] = '\0';
}

#endif /* _DBT2COMMON_H_ */
