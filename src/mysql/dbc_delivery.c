/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 */

#include "common.h"
#include "logging.h"
#include "mysql_delivery.h"
#include "string.h"

#include <stdio.h>

int execute_delivery_mysql(struct db_context_t *dbc, struct delivery_t *data) {
	char stmt[512];

	/* Create the query and execute it. */
	sprintf(stmt, "call delivery(%d, %d)", data->w_id, data->o_carrier_id);

#ifdef DEBUG_QUERY
	LOG_ERROR_MESSAGE("execute_delivery stmt: %s\n", stmt);
#endif
	if (mysql_query(dbc->library.mysql.mysql, stmt)) {
		LOG_ERROR_MESSAGE(
				"mysql reports: %d %s", mysql_errno(dbc->library.mysql.mysql),
				mysql_error(dbc->library.mysql.mysql));
		return ERROR;
	}
	return OK;
}
