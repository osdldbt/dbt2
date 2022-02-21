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
#include <catalog/pg_type_d.h>

#include "common.h"
#include "logging.h"
#include "libpq_new_order.h"

#define NEW_ORDER_1 \
		"SELECT w_tax\n" \
		"FROM warehouse\n" \
		"WHERE w_id = $1"

#define NEW_ORDER_2 \
		"UPDATE district\n" \
		"SET d_next_o_id = d_next_o_id + 1\n" \
		"WHERE d_w_id = $1\n" \
		"  AND d_id = $2\n" \
		"RETURNING d_tax, d_next_o_id"

#define NEW_ORDER_3 \
		"SELECT c_discount, c_last, c_credit\n" \
		"FROM customer\n" \
		"WHERE c_w_id = $1\n" \
		"  AND c_d_id = $2\n" \
		"  AND c_id = $3"

#define NEW_ORDER_4 \
		"INSERT INTO new_order (no_o_id, no_w_id, no_d_id)\n" \
		"VALUES ($1, $2, $3)"

#define NEW_ORDER_5 \
		"INSERT INTO orders (o_id, o_d_id, o_w_id, o_c_id, o_entry_d,\n" \
		"		    o_carrier_id, o_ol_cnt, o_all_local)\n" \
		"VALUES ($1, $2, $3, $4, current_timestamp, NULL, $5, $6)"

#define NEW_ORDER_6 \
		"SELECT i_price, i_name, i_data\n" \
		"FROM item\n" \
		"WHERE i_id = $1"

#define NEW_ORDER_7 \
		"SELECT s_quantity, $1, s_data\n" \
		"FROM stock\n" \
		"WHERE s_i_id = $2\n" \
		"  AND s_w_id = $3"

#define NEW_ORDER_8 \
		"UPDATE stock\n" \
		"SET s_quantity = s_quantity - $1\n" \
		"WHERE s_i_id = $2\n" \
		"  AND s_w_id = $3"

#define NEW_ORDER_9 \
		"INSERT INTO order_line (ol_o_id, ol_d_id, ol_w_id, ol_number,\n" \
		"			ol_i_id, ol_supply_w_id, ol_delivery_d,\n" \
		"			ol_quantity, ol_amount, ol_dist_info)\n" \
		"VALUES ($1, $2, $3, $4, $5, $6, NULL, $7, $8, $9)"

const char s_dist[10][11] = {
	"s_dist_01", "s_dist_02", "s_dist_03", "s_dist_04", "s_dist_05",
	"s_dist_06", "s_dist_07", "s_dist_08", "s_dist_09", "s_dist_10"
};

int execute_new_order_cockroach(struct db_context_t *dbc,
		struct new_order_t *data)
{
	PGresult *res;
	const char *paramValues[9];
	const Oid paramTypes[3] = {TEXTOID, INT4OID, INT4OID};

	char c_id[C_ID_LEN + 1];
	char d_id[D_ID_LEN + 1];
	char d_next_o_id[O_ID_LEN + 1];
	char o_all_local[O_ALL_LOCAL_LEN + 1];
	char o_ol_cnt[O_OL_CNT_LEN + 1];
	char ol_amount[I_PRICE_LEN + 1];
	char w_id[W_ID_LEN + 1];

	int decr_quantity;
	float fol_amount;
	char ol_i_id[I_ID_LEN + 1];
	char ol_number[O_OL_CNT_LEN + 1];
	char ol_supply_w_id[W_ID_LEN + 1];
	char qty[OL_QUANTITY_LEN + 1];
	char my_s_dist[S_DIST_LEN + 1];

	int i;
#ifdef DEBUG
	int j;
#endif /* DEBUG */

	snprintf(c_id, C_ID_LEN, "%d", data->c_id);
	snprintf(d_id, D_ID_LEN, "%d", data->d_id);
	snprintf(o_ol_cnt, O_OL_CNT_LEN, "%d", data->o_ol_cnt);
	snprintf(o_all_local, O_ALL_LOCAL_LEN, "%d", data->o_all_local);
	snprintf(w_id, W_ID_LEN, "%d", data->w_id);

	res = PQexec(dbc->library.libpq.conn, "BEGIN;");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	paramValues[0] = w_id;
	res = PQexecParams(dbc->library.libpq.conn, NEW_ORDER_1, 1, NULL,
			paramValues, NULL, NULL, 0);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
	if (PQntuples(res) == 0) {
		LOG_ERROR_MESSAGE("NO1 %s\n"
				"NO1 w_id = %s",
				NEW_ORDER_1, w_id);
		PQclear(res);
		return ERROR;
	}
#ifdef DEBUG
	for (j = 0; j < PQntuples(res); j++) {
		LOG_ERROR_MESSAGE("NO1[%d] w_tax %s", j, PQgetvalue(res, j, 0));
	}
#endif /* DEBUG */
	PQclear(res);

	paramValues[1] = d_id;
	res = PQexecParams(dbc->library.libpq.conn, NEW_ORDER_2, 2, NULL,
			paramValues, NULL, NULL, 0);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
	if (PQntuples(res) == 0) {
		LOG_ERROR_MESSAGE("NO2 %s\n"
				"NO2 d_w_id = %s\n"
				"NO2 d_id = %s\n"
				NEW_ORDER_2, w_id, d_id);
		PQclear(res);
		return ERROR;
	}
	strncpy(d_next_o_id, PQgetvalue(res, 0, 1), O_ID_LEN);
#ifdef DEBUG
	for (j = 0; j < PQntuples(res); j++) {
		LOG_ERROR_MESSAGE("NO2[%d] d_tax %s\n"
				"NO2[%d] d_next_o_id %s",
				j, PQgetvalue(res, j, 0),
				j, PQgetvalue(res, j, 1));
	}
#endif /* DEBUG */
	PQclear(res);

	paramValues[2] = c_id;
	res = PQexecParams(dbc->library.libpq.conn, NEW_ORDER_3, 3, NULL,
			paramValues, NULL, NULL, 0);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
	if (PQntuples(res) == 0) {
		LOG_ERROR_MESSAGE("NO3 %s\n"
				"NO3 c_w_id = %s\n"
				"NO3 c_d_id = %s\n"
				"NO3 c_id = %s",
				NEW_ORDER_3, w_id, d_id, c_id);
		PQclear(res);
		return ERROR;
	}
#ifdef DEBUG
	for (j = 0; j < PQntuples(res); j++) {
		LOG_ERROR_MESSAGE("NO3[%d] c_discount %s\n"
				"NO3[%d] c_last %s\n"
				"NO3[%d] c_credit %s",
				j, PQgetvalue(res, j, 0),
				j, PQgetvalue(res, j, 1),
				j, PQgetvalue(res, j, 2));
	}
#endif /* DEBUG */
	PQclear(res);

	paramValues[0] = d_next_o_id;
	paramValues[1] = w_id;
	paramValues[2] = d_id;
	res = PQexecParams(dbc->library.libpq.conn, NEW_ORDER_4, 3, NULL,
			paramValues, NULL, NULL, 0);
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}

	paramValues[0] = d_next_o_id;
	paramValues[1] = d_id;
	paramValues[2] = w_id;
	paramValues[3] = c_id;
	paramValues[4] = o_ol_cnt;
	paramValues[5] = o_all_local;
	res = PQexecParams(dbc->library.libpq.conn, NEW_ORDER_5, 6, NULL,
			paramValues, NULL, NULL, 0);
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}

	for (i = 0; i < data->o_ol_cnt; i++) {
		snprintf(ol_i_id, I_ID_LEN, "%d", data->order_line[i].ol_i_id);

		paramValues[0] = ol_i_id;
		res = PQexecParams(dbc->library.libpq.conn, NEW_ORDER_6, 1, NULL,
				paramValues, NULL, NULL, 0);
		if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
			LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
			PQclear(res);
			return ERROR;
		}
		if (PQntuples(res) == 0) {
			LOG_ERROR_MESSAGE("NO6 %s\n"
					"NO6 [%d] ol_i_id = %s",
					NEW_ORDER_6, i, ol_i_id);
			PQclear(res);
			return ERROR;
		}
		fol_amount += atof(PQgetvalue(res, 0, 0)) *
				(float) data->order_line[i].ol_quantity;
		snprintf(ol_amount, I_PRICE_LEN, "%f", fol_amount);
#ifdef DEBUG
		for (j = 0; j < PQntuples(res); j++) {
			LOG_ERROR_MESSAGE("NO6[%d][%d] i_price %s\n"
					"NO6[%d][%d] i_name %s\n"
					"NO6[%d][%d] i_data %s",
					i, j, PQgetvalue(res, j, 0),
					i, j, PQgetvalue(res, j, 1),
					i, j, PQgetvalue(res, j, 2));
		}
#endif /* DEBUG */
		PQclear(res);

		paramValues[0] = s_dist[data->d_id - 1];
		paramValues[1] = ol_i_id;
		paramValues[2] = w_id;
		res = PQexecParams(dbc->library.libpq.conn, NEW_ORDER_7, 3, paramTypes,
				paramValues, NULL, NULL, 0);
		if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
			LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
			PQclear(res);
			return ERROR;
		}
		if (PQntuples(res) == 0) {
			LOG_ERROR_MESSAGE("NO7 %s\n"
					"NO7 [%d] s_dist = %s"
					"NO7 [%d] ol_i_id = %s"
					"NO7 [%d] w_id = %s",
					NEW_ORDER_7, i, s_dist[data->d_id - 1], i, ol_i_id, i,
					w_id);
			PQclear(res);
			return ERROR;
		}
		if (atoi(PQgetvalue(res, 0, 0)) >
				(data->order_line[i].ol_quantity + 10)) {
			decr_quantity = data->order_line[i].ol_quantity;
		} else {
			decr_quantity = data->order_line[i].ol_quantity - 91;
		}
		strncpy(my_s_dist, PQgetvalue(res, 0, 1), S_DIST_LEN);
#ifdef DEBUG
		for (j = 0; j < PQntuples(res); j++) {
			LOG_ERROR_MESSAGE("NO7[%d][%d] s_quantity %s\n"
					"NO7[%d][%d] s_dist_%02d %s\n"
					"NO7[%d][%d] s_data %s",
					i, j, PQgetvalue(res, j, 0),
					i, j, data->d_id, PQgetvalue(res, j, 1),
					i, j, PQgetvalue(res, j, 2));
		}
#endif /* DEBUG */
		PQclear(res);

		snprintf(qty, OL_QUANTITY_LEN, "%d", decr_quantity);
		paramValues[0] = qty;
		paramValues[1] = ol_i_id;
		paramValues[2] = w_id;
		res = PQexecParams(dbc->library.libpq.conn, NEW_ORDER_8, 3, NULL,
				paramValues, NULL, NULL, 0);
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
			LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
			LOG_ERROR_MESSAGE("NO8 %s\n"
					"NO8 [%d] decr_quantity = %s"
					"NO8 [%d] ol_i_id = %s"
					"NO8 [%d] w_id = %s",
					NEW_ORDER_8, i, qty, i, ol_i_id, i, w_id);
			PQclear(res);
			return ERROR;
		}
#ifdef DEBUG
		for (j = 0; j < PQntuples(res); j++) {
			LOG_ERROR_MESSAGE("NO8[%d][%d] s_quantity %s\n"
					"NO8[%d][%d] s_dist_%02d %s\n"
					"NO8[%d][%d] s_data %s",
					i, j, PQgetvalue(res, j, 0),
					i, j, data->d_id, PQgetvalue(res, j, 1),
					i, j, PQgetvalue(res, j, 2));
		}
#endif /* DEBUG */
		PQclear(res);

		snprintf(ol_number, O_OL_CNT_LEN, "%d", i + 1);
		snprintf(qty, OL_QUANTITY_LEN, "%d", data->order_line[i].ol_quantity);
		snprintf(ol_supply_w_id, W_ID_LEN, "%d",
				data->order_line[i].ol_supply_w_id);
		paramValues[0] = d_next_o_id;
		paramValues[1] = d_id;
		paramValues[2] = w_id;
		paramValues[3] = ol_number;
		paramValues[4] = ol_i_id;
		paramValues[5] = ol_supply_w_id;
		paramValues[6] = qty;
		paramValues[7] = ol_amount;
		paramValues[8] = my_s_dist;
		res = PQexecParams(dbc->library.libpq.conn, NEW_ORDER_9, 9, NULL,
				paramValues, NULL, NULL, 0);
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
			LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
			PQclear(res);
			return ERROR;
		}
		PQclear(res);
	}

	return OK;
}
