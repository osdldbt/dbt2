/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002      Open Source Development Labs, Inc.
 *	             2002-2022 Mark Wong
 *
 * 13 May 2003
 */

#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "common.h"
#include "logging.h"
#include "libpq_order_status.h"

#define ORDER_STATUS_1 \
		"SELECT c_id\n" \
		"FROM customer\n" \
		"WHERE c_w_id = $1\n" \
		"  AND c_d_id = $2\n" \
		"  AND c_last = $3\n" \
		"ORDER BY c_first ASC"

#define ORDER_STATUS_2 \
		"SELECT c_first, c_middle, c_last, c_balance\n" \
		"FROM customer\n" \
		"WHERE c_w_id = $1\n" \
		"  AND c_d_id = $2\n" \
		"  AND c_id = $3"

#define ORDER_STATUS_3 \
		"SELECT o_id, o_carrier_id, o_entry_d, o_ol_cnt\n" \
		"FROM orders\n" \
		"WHERE o_w_id = $1\n" \
		"  AND o_d_id = $2\n" \
		"  AND o_c_id = $3\n" \
		"ORDER BY o_id DESC\n" \
		"LIMIT 1"

#define ORDER_STATUS_4 \
		"SELECT ol_i_id, ol_supply_w_id, ol_quantity, ol_amount,\n" \
		"	ol_delivery_d\n" \
		"FROM order_line\n" \
		"WHERE ol_w_id = $1\n" \
		"  AND ol_d_id = $2\n" \
		"  AND ol_o_id = $3"

int execute_order_status_cockroach(struct db_context_t *dbc,
		struct order_status_t *data)
{
	PGresult *res;
	const char *paramValues[4];

	char c_id[C_ID_LEN + 1];
	char c_d_id[D_ID_LEN + 1];
	char c_w_id[W_ID_LEN + 1];
	char c_last[4 * (C_LAST_LEN +1)];
	char o_id[O_ID_LEN + 1];

#ifdef DEBUG
	int i;
#endif /* DEBUG */

	snprintf(c_d_id, D_ID_LEN, "%d", data->c_d_id);
	wcstombs(c_last, data->c_last, 4 * (C_LAST_LEN +1));
	snprintf(c_w_id, W_ID_LEN, "%d", data->c_w_id);

	res = PQexec(dbc->library.libpq.conn, "BEGIN;");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	paramValues[0] = c_w_id;
	paramValues[1] = c_d_id;

	if (data->c_id == 0) {
		paramValues[2] = c_last;
		res = PQexecParams(dbc->library.libpq.conn, ORDER_STATUS_1, 3, NULL,
				paramValues, NULL, NULL, 0);
		if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
			LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
			PQclear(res);
			return ERROR;
		}
		if (PQntuples(res) == 0) {
			LOG_ERROR_MESSAGE("OS1 %s\n"
					"OS1 c_w_id = %s\n"
					"OS1 c_d_id = %s\n"
					"OS1 c_last = %s",
					ORDER_STATUS_1, c_w_id, c_d_id, c_last);
			PQclear(res);
			return ERROR;
		}
		strncpy(c_id, PQgetvalue(res, 0, 0), C_ID_LEN);
#ifdef DEBUG
		for (i = 0; i < PQntuples(res); i++) {
			LOG_ERROR_MESSAGE("OS1[%d] c_id %s", i, PQgetvalue(res, i, 0));
		}
#endif /* DEBUG */
		PQclear(res);
	} else {
		snprintf(c_id, C_ID_LEN, "%d", data->c_id);
	}

	paramValues[2] = c_id;
	res = PQexecParams(dbc->library.libpq.conn, ORDER_STATUS_2, 3, NULL,
			paramValues, NULL, NULL, 0);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
	if (PQntuples(res) == 0) {
		LOG_ERROR_MESSAGE("OS2 %s\n"
				"OS2 c_w_id = %s\n"
				"OS2 c_d_id = %s\n"
				"OS2 c_id = %s",
				ORDER_STATUS_2, c_w_id, c_d_id, c_id);
		PQclear(res);
		return ERROR;
	}
#ifdef DEBUG
	for (i = 0; i < PQntuples(res); i++) {
		LOG_ERROR_MESSAGE("OS2[%d] c_first %s\n"
				"OS2[%d] c_middle %s\n"
				"OS2[%d] c_last %s\n"
				"OS2[%d] c_balance %s",
				i, PQgetvalue(res, i, 0),
				i, PQgetvalue(res, i, 1),
				i, PQgetvalue(res, i, 2),
				i, PQgetvalue(res, i, 3));
	}
#endif /* DEBUG */
	PQclear(res);

	res = PQexecParams(dbc->library.libpq.conn, ORDER_STATUS_3, 3, NULL,
			paramValues, NULL, NULL, 0);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
	if (PQntuples(res) == 0) {
		LOG_ERROR_MESSAGE("OS3 %s\n"
				"OS3 o_w_id = %s\n"
				"OS3 o_d_id= %s\n"
				"OS3 o_c_id = %s",
				ORDER_STATUS_3, c_w_id, c_d_id, c_id);
		PQclear(res);
		return ERROR;
	}
	strncpy(o_id, PQgetvalue(res, 0, 0), C_ID_LEN);
#ifdef DEBUG
	for (i = 0; i < PQntuples(res); i++) {
		LOG_ERROR_MESSAGE("OS3[%d] o_id %s\n"
				"OS3[%d] o_carrier_id %s\n"
				"OS3[%d] o_entry_d %s\n"
				"OS3[%d] o_ol_cnt %s",
				i, PQgetvalue(res, i, 0),
				i, PQgetvalue(res, i, 1),
				i, PQgetvalue(res, i, 2),
				i, PQgetvalue(res, i, 3));
	}
#endif /* DEBUG */
	PQclear(res);

	res = PQexecParams(dbc->library.libpq.conn, ORDER_STATUS_4, 3, NULL,
			paramValues, NULL, NULL, 0);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
	if (PQntuples(res) == 0) {
		LOG_ERROR_MESSAGE("OS4 %s\n"
				"OS4 o_w_id = %s\n"
				"OS4 o_d_id= %s\n"
				"OS4 o_id = %s",
				ORDER_STATUS_4, c_w_id, c_d_id, o_id);
		PQclear(res);
		return ERROR;
	}
#ifdef DEBUG
	for (i = 0; i < PQntuples(res); i++) {
		LOG_ERROR_MESSAGE("OS4[%d] ol_i_id %s\n"
				"OS4[%d] ol_supply_w_id %s\n"
				"OS4[%d] ol_quantity %s\n"
				"OS4[%d] ol_amount %s",
				i, PQgetvalue(res, i, 0),
				i, PQgetvalue(res, i, 1),
				i, PQgetvalue(res, i, 2),
				i, PQgetvalue(res, i, 3));
	}
#endif /* DEBUG */
	PQclear(res);

	return OK;
}
