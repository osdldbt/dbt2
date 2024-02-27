/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 */

#include "common.h"
#include "logging.h"
#include "mysql_integrity.h"
#include "string.h"

#include <stdio.h>

int execute_integrity_mysql(
		struct db_context_t *dbc, struct integrity_t *data) {
	char stmt[512];
	int rc;

	MYSQL_RES *result;
	MYSQL_ROW row;

	/* Create the query and execute it. */
	sprintf(stmt, "select count(w_id) as w from warehouse");

#ifdef DEBUG_QUERY
	LOG_ERROR_MESSAGE("execute_delivery stmt: %s\n", stmt);
#endif

	rc = OK;

	if (mysql_query(dbc->library.mysql.mysql, stmt)) {
		LOG_ERROR_MESSAGE(
				"mysql reports: %d %s", mysql_errno(dbc->library.mysql.mysql),
				mysql_error(dbc->library.mysql.mysql));
		rc = ERROR;
	} else {
		if ((result = mysql_store_result(dbc->library.mysql.mysql))) {
			if ((row = mysql_fetch_row(result)) && (row[0])) {
				if (atoi(row[0]) != data->w_id) {
					LOG_ERROR_MESSAGE(
							"Wrong number of warehouses. Should be "
							"%d Database reports %d",
							data->w_id, atoi(row[0]));
					rc = ERROR;
				}
			} else {
				fprintf(stderr, "Error: %s\n",
						mysql_error(dbc->library.mysql.mysql));
				rc = ERROR;
			}
			mysql_free_result(result);
		} else {
			rc = ERROR;
		}
	}

	return rc;
}
