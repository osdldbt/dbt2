/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Labs, Inc.
 *
 * 13 May 2003
 */

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "logging.h"
#include "libpq_new_order.h"

int execute_new_order(struct db_context_t *dbc, struct new_order_t *data)
{
	PGresult *res;
	char stmt[128];
	char tmp[64];
	int i;

	/* Start a transaction block. */
	res = PQexec(dbc->conn, "BEGIN");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->conn));
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	/* Create the query and execute it. */
	sprintf(stmt,
		"SELECT new_order('%9d%2d%5d%1d%2d'",
		data->w_id, data->d_id, data->c_id, data->o_all_local,
		data->o_ol_cnt);
	for (i = 0; i < data->o_ol_cnt; i++) {
		sprintf(tmp, ", '%6d%9d%2d'",
			data->order_line[i].ol_i_id,
			data->order_line[i].ol_supply_w_id,
			data->order_line[i].ol_quantity);
		strcat(stmt, tmp);
	}
	for (i = data->o_ol_cnt; i < 15; i++) {
		strcat(stmt, ", ''");
	}
	strcat(stmt, ")");
	res = PQexec(dbc->conn, stmt);
	if (!res || (PQresultStatus(res) != PGRES_COMMAND_OK &&
		PQresultStatus(res) != PGRES_TUPLES_OK)) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->conn));
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	/* Commit the transaction. */
	res = PQexec(dbc->conn, "COMMIT");
	PQclear(res);

	return OK;
}
