/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>

#include "common.h"
#include "libpq_stock_level.h"
#include "logging.h"

#define STOCK_LEVEL_1                                                          \
	"SELECT d_next_o_id\n"                                                     \
	"FROM district\n"                                                          \
	"WHERE d_w_id = $1\n"                                                      \
	"AND d_id = $2"

#define STOCK_LEVEL_2                                                          \
	"WITH\n"                                                                   \
	"ol AS (\n"                                                                \
	"    SELECT DISTINCT ol_i_id\n"                                            \
	"    FROM order_line\n"                                                    \
	"    WHERE ol_w_id = $2\n"                                                 \
	"      AND ol_d_id = $1\n"                                                 \
	"      AND ol_o_id BETWEEN ($4)\n"                                         \
	"                      AND ($5)\n"                                         \
	")\n"                                                                      \
	"SELECT count(*)\n"                                                        \
	"FROM ol, stock\n"                                                         \
	"WHERE s_w_id = $2\n"                                                      \
	"  AND s_i_id = ol_i_id\n"                                                 \
	"  AND s_quantity < $3"

int execute_stock_level_cockroach(
		struct db_context_t *dbc, struct stock_level_t *data) {
	PGresult *res;
	const char *paramValues[5];

	char d_w_id[W_ID_LEN + 1];
	char d_id[D_ID_LEN + 1];
	char threshold[THRESHOLD_LEN + 1];
	char ol_o_id1[OL_O_ID_LEN + 1];
	char ol_o_id2[OL_O_ID_LEN + 1];

#ifdef DEBUG
	int i;
#endif /* DEBUG */

	snprintf(d_w_id, W_ID_LEN, "%d", data->w_id);
	snprintf(d_id, D_ID_LEN, "%d", data->d_id);
	snprintf(threshold, THRESHOLD_LEN, "%d", data->threshold);

	res = PQexec(dbc->library.libpq.conn, "BEGIN;");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	paramValues[0] = d_w_id;
	paramValues[1] = d_id;
	res = PQexecParams(
			dbc->library.libpq.conn, STOCK_LEVEL_1, 2, NULL, paramValues, NULL,
			NULL, 0);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
	if (PQntuples(res) == 0) {
		LOG_ERROR_MESSAGE(
				"SL1 %s\n"
				"SL1 d_w_id = %s\n"
				"SL1 d_id = %s",
				STOCK_LEVEL_1, d_w_id, d_id);
		PQclear(res);
		return ERROR;
	}
#ifdef DEBUG
	for (i = 0; i < PQntuples(res); i++) {
		LOG_ERROR_MESSAGE("SL1[%d] ol_i_id %s", i, PQgetvalue(res, i, 0));
	}
#endif /* DEBUG */
	snprintf(ol_o_id1, OL_O_ID_LEN, "%d", atoi(PQgetvalue(res, 0, 0) - 20));
	snprintf(ol_o_id2, OL_O_ID_LEN, "%d", atoi(PQgetvalue(res, 0, 0) - 1));
	PQclear(res);

	paramValues[2] = threshold;
	paramValues[3] = ol_o_id1;
	paramValues[4] = ol_o_id2;
	res = PQexecParams(
			dbc->library.libpq.conn, STOCK_LEVEL_2, 5, NULL, paramValues, NULL,
			NULL, 0);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
	if (PQntuples(res) == 0) {
		LOG_ERROR_MESSAGE(
				"SL2 %s\n"
				"SL2 d_w_id = %s\n"
				"SL2 d_id = %s"
				"SL2 threshold = %s"
				"SL2 ol_o_id1 = %s"
				"SL2 ol_o_id2 = %s",
				STOCK_LEVEL_1, d_w_id, d_id, threshold, ol_o_id1, ol_o_id2);
		PQclear(res);
		return ERROR;
	}
#ifdef DEBUG
	for (i = 0; i < PQntuples(res); i++) {
		LOG_ERROR_MESSAGE("SL2[%d] count(*) %s", i, PQgetvalue(res, i, 0));
	}
#endif /* DEBUG */
	PQclear(res);

	return OK;
}
