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
	char stmt[512];
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
		"SELECT new_order(%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
		data->w_id, data->d_id, data->c_id, data->o_all_local,
		data->o_ol_cnt,
		data->order_line[0].ol_i_id,
		data->order_line[0].ol_supply_w_id,
		data->order_line[0].ol_quantity,
		data->order_line[1].ol_i_id,
		data->order_line[1].ol_supply_w_id,
		data->order_line[1].ol_quantity,
		data->order_line[2].ol_i_id,
		data->order_line[2].ol_supply_w_id,
		data->order_line[2].ol_quantity,
		data->order_line[3].ol_i_id,
		data->order_line[3].ol_supply_w_id,
		data->order_line[3].ol_quantity,
		data->order_line[4].ol_i_id,
		data->order_line[4].ol_supply_w_id,
		data->order_line[4].ol_quantity,
		data->order_line[5].ol_i_id,
		data->order_line[5].ol_supply_w_id,
		data->order_line[5].ol_quantity,
		data->order_line[6].ol_i_id,
		data->order_line[6].ol_supply_w_id,
		data->order_line[6].ol_quantity,
		data->order_line[7].ol_i_id,
		data->order_line[7].ol_supply_w_id,
		data->order_line[7].ol_quantity,
		data->order_line[8].ol_i_id,
		data->order_line[8].ol_supply_w_id,
		data->order_line[8].ol_quantity,
		data->order_line[9].ol_i_id,
		data->order_line[9].ol_supply_w_id,
		data->order_line[9].ol_quantity,
		data->order_line[10].ol_i_id,
		data->order_line[10].ol_supply_w_id,
		data->order_line[10].ol_quantity,
		data->order_line[11].ol_i_id,
		data->order_line[11].ol_supply_w_id,
		data->order_line[11].ol_quantity,
		data->order_line[12].ol_i_id,
		data->order_line[12].ol_supply_w_id,
		data->order_line[12].ol_quantity,
		data->order_line[13].ol_i_id,
		data->order_line[13].ol_supply_w_id,
		data->order_line[13].ol_quantity,
		data->order_line[14].ol_i_id,
		data->order_line[14].ol_supply_w_id,
		data->order_line[14].ol_quantity);
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
