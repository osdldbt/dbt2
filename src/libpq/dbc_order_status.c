/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Labs, Inc.
 *
 * 13 May 2003
 */

#include <stdio.h>

#include "common.h"
#include "logging.h"
#include "libpq_order_status.h"

int execute_order_status(struct db_context_t *dbc, struct order_status_t *data)
{
	PGresult *res;
	char stmt[512];

	/* Start a transaction block. */
	res = PQexec(dbc->conn, "BEGIN");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->conn));
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	/* Create the query and execute it. */
/*
	sprintf(stmt, "SELECT * FROM order_status(%d, %d, %d, '%s') l(c_id INTEGER, c_first VARCHAR, c_middle VARCHAR, c_late VARCHAR, c_balance NUMERIC, o_id INTEGER, o_carrier_id INTEGER, o_entry_d VARCHAR, o_ol_cnt INTEGER, ol_i_id NUMERIC, ol_supply_w_id NUMERIC, ol_quantity NUMERIC, ol_amount NUMERIC, ol_delivery_d TIMESTAMP)",
		data->c_id, data->c_w_id, data->c_d_id, data->c_last);
*/
	sprintf(stmt, "SELECT * FROM order_status(%d, %d, %d, '%s')",
		data->c_id, data->c_w_id, data->c_d_id, data->c_last);
	res = PQexec(dbc->conn, stmt);
	if (!res || (PQresultStatus(res) != PGRES_COMMAND_OK &&
		PQresultStatus(res) != PGRES_TUPLES_OK)) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->conn));
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	return OK;
}
