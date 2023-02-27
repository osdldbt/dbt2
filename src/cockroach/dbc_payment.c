/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 */

#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "common.h"
#include "logging.h"
#include "libpq_payment.h"

#define PAYMENT_1 \
		"UPDATE warehouse\n" \
		"SET w_ytd = w_ytd + $1\n" \
		"WHERE w_id = $2\n" \
		"RETURNING w_name, w_street_1, w_street_2, w_city, w_state, w_zip"

#define PAYMENT_2 \
		"UPDATE district\n" \
		"SET d_ytd = d_ytd + $1\n" \
		"WHERE d_id = $2\n" \
		"  AND d_w_id = $3\n" \
		"RETURNING d_name, d_street_1, d_street_2, d_city, d_state, d_zip"

#define PAYMENT_3 \
		"SELECT c_id\n" \
		"FROM customer\n" \
		"WHERE c_w_id = $1\n" \
		"  AND c_d_id = $2\n" \
		"  AND c_last = $3\n" \
		"ORDER BY c_first ASC"

#define PAYMENT_4 \
		"SELECT c_first, c_middle, c_last, c_street_1, c_street_2, c_city,\n" \
		"       c_state, c_zip, c_phone, c_since, c_credit, c_credit_lim,\n" \
		"       c_discount, c_balance\n" \
		"FROM customer\n" \
		"WHERE c_w_id = $1\n" \
		"  AND c_d_id = $2\n" \
		"  AND c_id = $3"

#define PAYMENT_5_GC \
		"UPDATE customer\n" \
		"SET c_balance = c_balance - $1,\n" \
		"    c_ytd_payment = c_ytd_payment + 1\n" \
		"WHERE c_id = $2\n" \
		"  AND c_w_id = $3\n" \
		"  AND c_d_id = $4"

#define PAYMENT_5_BC \
		"UPDATE customer\n" \
		"SET c_balance = c_balance - $1,\n" \
		"    c_ytd_payment = c_ytd_payment + 1,\n" \
		"    c_data = substring($2 || c_data, 1, 500)\n" \
		"WHERE c_id = $3\n" \
		"  AND c_w_id = $4\n" \
		"  AND c_d_id = $5\n" \
		"RETURNING substring(c_data, 1, 200)"

#define PAYMENT_6 \
		"INSERT INTO history (h_c_id, h_c_d_id, h_c_w_id, h_d_id, h_w_id,\n" \
		"                     h_date, h_amount, h_data)\n" \
		"VALUES ($1, $2, $3, $4, $5, CURRENT_TIMESTAMP, $6,\n" \
		"        $7 || '    ' || $8)\n" \
		"RETURNING h_date"

int execute_payment_cockroach(struct db_context_t *dbc, struct payment_t *data)
{
	PGresult *res;
	const char *paramValues[8];

#ifdef DEBUG
	int i;
#endif /* DEBUG */

	int gc = 0;
	char c_id[C_ID_LEN + 1];
	char c_data[C_DATA_LEN + 1];
	char c_last[4 * (C_LAST_LEN +1)];
	char c_d_id[D_ID_LEN + 1];
	char c_w_id[W_ID_LEN + 1];
	char d_id[D_ID_LEN + 1];
	char d_name[D_NAME_LEN + 1];
	char h_amount[H_AMOUNT_LEN + 1];
	char w_id[W_ID_LEN + 1];
	char w_name[W_NAME_LEN + 1];

	wcstombs(c_last, data->c_last, 4 * (C_LAST_LEN +1));
	snprintf(c_d_id, D_ID_LEN, "%d", data->c_d_id);
	snprintf(c_w_id, W_ID_LEN, "%d", data->c_w_id);
	snprintf(d_id, D_ID_LEN, "%d", data->d_id);
	snprintf(w_id, W_ID_LEN, "%d", data->w_id);
	snprintf(h_amount, H_AMOUNT_LEN, "%f", data->h_amount);

	res = PQexec(dbc->library.libpq.conn, "BEGIN;");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	paramValues[0] = h_amount;
	paramValues[1] = w_id;
	res = PQexecParams(dbc->library.libpq.conn, PAYMENT_1, 2, NULL, paramValues,
			NULL, NULL, 0);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
	if (PQntuples(res) == 0) {
		LOG_ERROR_MESSAGE("P1 %s\n"
				"P1 h_amount = %s\n"
				"P1 w_id = %s",
				PAYMENT_1, h_amount, w_id);
		PQclear(res);
		return ERROR;
	}
	strncpy(w_name, PQgetvalue(res, 0, 0), W_NAME_LEN);
#ifdef DEBUG
	for (i = 0; i < PQntuples(res); i++) {
		LOG_ERROR_MESSAGE("P1[%d] w_name %s\n"
				"P1[%d] w_street_1 %s\n"
				"P1[%d] w_street_2 %s\n"
				"P1[%d] w_city %s\n"
				"P1[%d] w_state %s\n"
				"P1[%d] w_zip %s",
				i, PQgetvalue(res, i, 0),
				i, PQgetvalue(res, i, 1),
				i, PQgetvalue(res, i, 2),
				i, PQgetvalue(res, i, 3),
				i, PQgetvalue(res, i, 4),
				i, PQgetvalue(res, i, 5));
	}
#endif /* DEBUG */
	PQclear(res);

	paramValues[0] = h_amount;
	paramValues[1] = d_id;
	paramValues[2] = w_id;
	res = PQexecParams(dbc->library.libpq.conn, PAYMENT_2, 3, NULL, paramValues,
			NULL, NULL, 0);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
	if (PQntuples(res) == 0) {
		LOG_ERROR_MESSAGE("P2 %s\n"
				"P2 h_amount = %s\n"
				"P2 d_id = %s\n"
				"P2 w_id = %s",
				PAYMENT_2, h_amount, d_id, w_id);
		PQclear(res);
		return ERROR;
	}
	strncpy(d_name, PQgetvalue(res, 0, 0), D_NAME_LEN);
#ifdef DEBUG
	for (i = 0; i < PQntuples(res); i++) {
		LOG_ERROR_MESSAGE("P2[%d] d_name %s\n"
				"P2[%d] d_street_1 %s\n"
				"P2[%d] d_street_2 %s\n"
				"P2[%d] d_city %s\n"
				"P2[%d] d_state %s\n"
				"P2[%d] d_zip %s",
				i, PQgetvalue(res, i, 0),
				i, PQgetvalue(res, i, 1),
				i, PQgetvalue(res, i, 2),
				i, PQgetvalue(res, i, 3),
				i, PQgetvalue(res, i, 4),
				i, PQgetvalue(res, i, 5));
	}
#endif /* DEBUG */
	PQclear(res);

	if (data->c_id == 0) {
		paramValues[0] = c_w_id;
		paramValues[1] = c_d_id;
		paramValues[2] = c_last;
		res = PQexecParams(dbc->library.libpq.conn, PAYMENT_3, 3, NULL,
				paramValues, NULL, NULL, 0);
		if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
			LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
			PQclear(res);
			return ERROR;
		}
		if (PQntuples(res) == 0) {
			LOG_ERROR_MESSAGE("P3 %s\n"
					"P3 c_w_id = %s\n"
					"P3 c_d_id = %s\n"
					"P3 c_last = %s",
					PAYMENT_3, c_w_id, c_d_id, c_last);
			PQclear(res);
			return ERROR;
		}
		strncpy(c_id, PQgetvalue(res, 0, 0), C_ID_LEN);
#ifdef DEBUG
		for (i = 0; i < PQntuples(res); i++) {
			LOG_ERROR_MESSAGE("P3[%d] c_id %s", i, PQgetvalue(res, i, 0));
		}
#endif /* DEBUG */
		PQclear(res);
	} else {
		snprintf(c_id, C_ID_LEN, "%d", data->c_id);
	}

	paramValues[0] = c_w_id;
	paramValues[1] = c_d_id;
	paramValues[2] = c_id;
	res = PQexecParams(dbc->library.libpq.conn, PAYMENT_4, 3, NULL, paramValues,
			NULL, NULL, 0);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
	if (PQntuples(res) == 0) {
		LOG_ERROR_MESSAGE("P4 %s\n"
				"P4 c_w_id = %s\n"
				"P4 c_d_id = %s\n"
				"P4 c_id = %s",
				PAYMENT_4, c_w_id, c_d_id, c_id);
		PQclear(res);
		return ERROR;
	}
	if (PQgetvalue(res, 0, 10)[0] == 'G')
		gc = 1;
	else if (PQgetvalue(res, 0, 10)[0] == 'B')
		gc = 0;
	else
		LOG_ERROR_MESSAGE("P4 unrecognized credit %s", PQgetvalue(res, 0, 10));
#ifdef DEBUG
	for (i = 0; i < PQntuples(res); i++) {
		LOG_ERROR_MESSAGE("P4[%d] c_first %s\n"
				"P4[%d] c_middle %s\n"
				"P4[%d] c_last %s\n"
				"P4[%d] c_street1 %s\n"
				"P4[%d] c_street2 %s\n"
				"P4[%d] c_city %s\n"
				"P4[%d] c_state %s\n"
				"P4[%d] c_zip %s\n"
				"P4[%d] c_phone %s\n"
				"P4[%d] c_since %s\n"
				"P4[%d] c_credit %s\n"
				"P4[%d] c_credit_lim %s\n"
				"P4[%d] c_discount %s\n"
				"P4[%d] c_balance %s",
				i, PQgetvalue(res, i, 0),
				i, PQgetvalue(res, i, 1),
				i, PQgetvalue(res, i, 2),
				i, PQgetvalue(res, i, 3),
				i, PQgetvalue(res, i, 4),
				i, PQgetvalue(res, i, 5),
				i, PQgetvalue(res, i, 6),
				i, PQgetvalue(res, i, 7),
				i, PQgetvalue(res, i, 8),
				i, PQgetvalue(res, i, 9),
				i, PQgetvalue(res, i, 10),
				i, PQgetvalue(res, i, 11),
				i, PQgetvalue(res, i, 12),
				i, PQgetvalue(res, i, 13));
	}
#endif /* DEBUG */
	PQclear(res);

	if (gc) {
		paramValues[0] = h_amount;
		paramValues[1] = c_id;
		paramValues[2] = c_w_id;
		paramValues[3] = c_d_id;
		res = PQexecParams(dbc->library.libpq.conn, PAYMENT_5_GC, 4, NULL,
				paramValues, NULL, NULL, 0);
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
			LOG_ERROR_MESSAGE("P5GC Error: %s\n"
					"P5GC %s\n"
					"P5GC h_amount = %s\n"
					"P5GC c_id = %s\n"
					"P5GC c_w_id = %s\n"
					"P5GC c_d_id = %s",
					PQerrorMessage(dbc->library.libpq.conn),
					PAYMENT_5_GC, h_amount, c_id, c_w_id, c_d_id);
			PQclear(res);
			return ERROR;
		}
		c_data[0] = '\0';
	} else {
		snprintf(c_data, C_DATA_LEN, "%s %d %d %d %d %f ",
				c_id, data->c_d_id, data->c_w_id, data->d_id, data->w_id,
				data->h_amount);

		paramValues[0] = h_amount;
		paramValues[1] = c_data;
		paramValues[2] = c_id;
		paramValues[3] = c_w_id;
		paramValues[4] = c_d_id;
		res = PQexecParams(dbc->library.libpq.conn, PAYMENT_5_BC, 5, NULL,
				paramValues, NULL, NULL, 0);
		if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
			LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
			PQclear(res);
			return ERROR;
		}
		if (PQntuples(res) == 0) {
			LOG_ERROR_MESSAGE("P5BC %s\n"
					"P5BC h_amount = %s\n"
					"P5BC c_data = %s\n"
					"P5BC c_id = %s\n"
					"P5BC c_w_id = %s\n"
					"P5BC c_d_id = %s",
					PAYMENT_5_BC, h_amount, c_id, c_w_id, c_d_id);
			PQclear(res);
			return ERROR;
		}
		strncpy(c_data, PQgetvalue(res, 0, 0), C_DATA_LEN);
#ifdef DEBUG
		for (i = 0; i < PQntuples(res); i++) {
			LOG_ERROR_MESSAGE("P5_BC[%d] c_data %s", i, PQgetvalue(res, i, 0));
		}
#endif /* DEBUG */
	}
	PQclear(res);

	paramValues[0] = c_id;
	paramValues[1] = c_d_id;
	paramValues[2] = c_w_id;
	paramValues[3] = d_id;
	paramValues[4] = w_id;
	paramValues[5] = h_amount;
	paramValues[6] = w_name;
	paramValues[7] = d_name;
	res = PQexecParams(dbc->library.libpq.conn, PAYMENT_6, 8, NULL, paramValues,
			NULL, NULL, 0);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
	if (PQntuples(res) == 0) {
		LOG_ERROR_MESSAGE("P6 %s\n"
				"P6 c_id = %s\n"
				"P6 c_d_id = %s\n"
				"P6 c_w_id = %s\n"
				"P6 d_id = %s\n"
				"P6 w_id = %s\n"
				"P6 h_amount = %s\n"
				"P6 w_name = %s\n"
				"P6 d_name = %s",
				PAYMENT_6, c_id, c_d_id, c_w_id, d_id, w_id, h_amount, w_name,
				d_name);
		PQclear(res);
		return ERROR;
	}
#ifdef DEBUG
	for (i = 0; i < PQntuples(res); i++) {
		LOG_ERROR_MESSAGE("P8[%d] h_date %s", i, PQgetvalue(res, i, 0));
	}
#endif /* DEBUG */
	PQclear(res);

	return OK;
}
