/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002      Open Source Development Labs, Inc.
 *               2002-2022 Mark Wong
 *
 * 13 May 2003
 */

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "logging.h"
#include "libpq_delivery.h"

#define DELIVERY_1 \
		"SELECT no_o_id\n" \
		"FROM new_order\n" \
		"WHERE no_w_id = $1\n" \
		"  AND no_d_id = $2\n" \
		"ORDER BY no_o_id ASC\n" \
		"LIMIT 1"

#define DELIVERY_2 \
		"DELETE FROM new_order\n" \
		"WHERE no_o_id = $1\n" \
		"  AND no_w_id = $2\n" \
		"  AND no_d_id = $3"

#define DELIVERY_3 \
		"UPDATE orders\n" \
		"SET o_carrier_id = $1\n" \
		"WHERE o_id = $2\n" \
		"  AND o_w_id = $3\n" \
		"  AND o_d_id = $4\n" \
		"RETURNING o_c_id"

#define DELIVERY_4 \
		"UPDATE order_line\n" \
		"SET ol_delivery_d = current_timestamp\n" \
		"WHERE ol_o_id = $1\n" \
		"  AND ol_w_id = $2\n" \
		"  AND ol_d_id = $3"

#define DELIVERY_5 \
		"SELECT SUM(ol_amount * ol_quantity)\n" \
		"FROM order_line\n" \
		"WHERE ol_o_id = $1\n" \
		"  AND ol_w_id = $2\n" \
		"  AND ol_d_id = $3"

#define DELIVERY_6 \
	"UPDATE customer\n" \
	"SET c_delivery_cnt = c_delivery_cnt + 1,\n" \
	"    c_balance = c_balance + $1\n" \
	"WHERE c_id = $2\n" \
	"  AND c_w_id = $3\n" \
	"  AND c_d_id = $4"

int execute_delivery(struct db_context_t *dbc, struct delivery_t *data)
{
	PGresult *res;
	const char *paramValues[4];

	char d_id[D_ID_LEN + 1];
	char o_carrier_id[O_CARRIER_ID_LEN + 1];
	char o_id[O_ID_LEN + 1];
	char ol_amount[OL_AMOUNT_LEN + 1];
	char w_id[W_ID_LEN + 1];

	int i;

#ifdef DEBUG
	int j;
#endif /* DEBUG */

	snprintf(o_carrier_id, O_CARRIER_ID_LEN, "%d", data->o_carrier_id);
	snprintf(w_id, W_ID_LEN, "%d", data->w_id);

	res = PQexec(dbc->conn, "BEGIN;");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->conn));
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	for (i = 0; i < D_ID_MAX; i++) {
		snprintf(d_id, D_ID_LEN, "%d", i + 1);

		paramValues[0] = w_id;
		paramValues[1] = d_id;
		res = PQexecParams(dbc->conn, DELIVERY_1, 2, NULL, paramValues, NULL,
				NULL, 0);
		if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
			LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->conn));
			PQclear(res);
			return ERROR;
		}
		strncpy(o_id, PQgetvalue(res, 0, 0), O_ID_LEN);
#ifdef DEBUG
		for (j = 0; j < PQntuples(res); j++) {
			LOG_ERROR_MESSAGE("D1[%d][%d] no_o_id %s",
					i, j, PQgetvalue(res, j, 0));
		}
#endif /* DEBUG */
		PQclear(res);

		paramValues[0] = o_id;
		paramValues[1] = w_id;
		paramValues[2] = d_id;
		res = PQexecParams(dbc->conn, DELIVERY_2, 3, NULL, paramValues, NULL,
				NULL, 0);
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
			LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->conn));
			PQclear(res);
			return ERROR;
		}
		PQclear(res);

		paramValues[0] = o_carrier_id;
		paramValues[1] = o_id;
		paramValues[2] = w_id;
		paramValues[3] = d_id;
		res = PQexecParams(dbc->conn, DELIVERY_3, 4, NULL, paramValues, NULL,
				NULL, 0);
		if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
			LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->conn));
			PQclear(res);
			return ERROR;
		}
#ifdef DEBUG
		for (j = 0; j < PQntuples(res); j++) {
			LOG_ERROR_MESSAGE("D3[%d][%d] o_c_id %s",
					i, j, PQgetvalue(res, j, 0));
		}
#endif /* DEBUG */
		PQclear(res);

		paramValues[0] = o_id;
		paramValues[1] = w_id;
		paramValues[2] = d_id;
		res = PQexecParams(dbc->conn, DELIVERY_4, 3, NULL, paramValues, NULL,
				NULL, 0);
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
			LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->conn));
			PQclear(res);
			return ERROR;
		}
		PQclear(res);

		res = PQexecParams(dbc->conn, DELIVERY_5, 3, NULL, paramValues, NULL,
				NULL, 0);
		if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
			LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->conn));
			PQclear(res);
			return ERROR;
		}
		strncpy(ol_amount, PQgetvalue(res, 0, 0), OL_AMOUNT_LEN);
#ifdef DEBUG
		for (j = 0; j < PQntuples(res); j++) {
			LOG_ERROR_MESSAGE("D5[%d][%d] sum %s",
					i, j, PQgetvalue(res, j, 0));
		}
#endif /* DEBUG */
		PQclear(res);

		paramValues[0] = ol_amount;
		paramValues[1] = w_id;
		paramValues[2] = w_id;
		paramValues[3] = d_id;
		res = PQexecParams(dbc->conn, DELIVERY_6, 4, NULL, paramValues, NULL,
				NULL, 0);
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
			LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->conn));
			PQclear(res);
			return ERROR;
		}
		PQclear(res);
	}

	return OK;
}
