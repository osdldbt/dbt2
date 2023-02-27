/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 */

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "logging.h"
#include "libpq_new_order.h"

#define UDF_NEW_ORDER \
		"SELECT * FROM new_order(" \
		"$1, $2, $3, $4, $5, " \
		"($6, $7, $8), " \
		"($9, $10, $11), " \
		"($12, $13, $14), " \
		"($15, $16, $17), " \
		"($18, $19, $20), " \
		"($21, $22, $23), " \
		"($24, $25, $26), " \
		"($27, $28, $29), " \
		"($30, $31, $32), " \
		"($33, $34, $35), " \
		"($36, $37, $38), " \
		"($39, $40, $41), " \
		"($42, $43, $44), " \
		"($45, $46, $47), " \
		"($48, $49, $50))"

int execute_new_order_libpq(struct db_context_t *dbc, struct new_order_t *data)
{
	int i;
	PGresult *res;
	const char *paramValues[50];
	const int paramFormats[50] = {
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1
	};
	int paramLengths[50] = {
		sizeof(uint32_t), sizeof(uint32_t), sizeof(uint32_t), sizeof(uint32_t),
		sizeof(uint32_t),
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0
	};


	uint32_t w_id;
	uint32_t d_id;
	uint32_t c_id;
	uint32_t o_all_local;
	uint32_t o_ol_cnt;
	uint32_t ol_i_id[15];
	uint32_t ol_supply_w_id[15];
	uint32_t ol_quantity[15];

	/* Start a transaction block. */
	res = PQexec(dbc->library.libpq.conn, "BEGIN");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	w_id = htonl((uint32_t) data->w_id);
	d_id = htonl((uint32_t) data->d_id);
	c_id = htonl((uint32_t) data->c_id);
	o_all_local = htonl((uint32_t) data->o_all_local);
	o_ol_cnt = htonl((uint32_t) data->o_ol_cnt);

	paramValues[0] = (char *) &w_id;
	paramValues[1] = (char *) &d_id;
	paramValues[2] = (char *) &c_id;
	paramValues[3] = (char *) &o_all_local;
	paramValues[4] = (char *) &o_ol_cnt;

#ifdef DEBUG
	LOG_ERROR_MESSAGE("NO w_id %d d_id %d c_id %d o_all_local %d o_ol_cnt %d",
			data->w_id, data->d_id, data->c_id, data->o_all_local,
			data->o_ol_cnt);
#endif /* DEBUG */
	for (i = 0; i < data->o_ol_cnt; i++) {
		ol_i_id[i] = htonl((uint32_t) data->order_line[i].ol_i_id);
		ol_supply_w_id[i] =
				htonl((uint32_t) data->order_line[i].ol_supply_w_id);
		ol_quantity[i] = htonl((uint32_t) data->order_line[i].ol_quantity);

		paramValues[(i * 3) + 5] = (char *) &ol_i_id[i];
		paramValues[(i * 3) + 6] = (char *) &ol_supply_w_id[i];
		paramValues[(i * 3) + 7] = (char *) &ol_quantity[i];

		paramLengths[(i * 3) + 5] = sizeof(ol_i_id[i]);
		paramLengths[(i * 3) + 6] = sizeof(ol_supply_w_id[i]);
		paramLengths[(i * 3) + 7] = sizeof(ol_quantity[i]);
#ifdef DEBUG
		LOG_ERROR_MESSAGE("NO[%d] ol_i_id %d ol_supply_w_id %d ol_quantity %d",
				i + 1, data->order_line[i].ol_i_id,
				data->order_line[i].ol_supply_w_id,
				data->order_line[i].ol_quantity);
#endif /* DEBUG */
	}
	for (i = data->o_ol_cnt; i < 15; i++) {
		paramValues[(i * 3) + 5] = NULL;
		paramValues[(i * 3) + 6] = NULL;
		paramValues[(i * 3) + 7] = NULL;

		paramLengths[(i * 3) + 5] = 0;
		paramLengths[(i * 3) + 6] = 0;
		paramLengths[(i * 3) + 7] = 0;
	}

	res = PQexecParams(dbc->library.libpq.conn, UDF_NEW_ORDER, 50, NULL,
			paramValues, paramLengths, paramFormats, 1);
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		data->rollback = 0;
		LOG_ERROR_MESSAGE("NO %s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
	data->rollback = atoi(PQgetvalue(res, 0, 0));
#ifdef DEBUG
	for (i = 0; i < PQntuples(res); i++) {
		union
		{
			float f;
			uint32_t i;
		}	v5, v6;

		v5.i = ntohl(*((uint32_t *) PQgetvalue(res, i, 5)));
		v6.i = ntohl(*((uint32_t *) PQgetvalue(res, i, 6)));
		LOG_ERROR_MESSAGE("NO[%d] %s=%d %s=%d %s=%s %s=%d %s=%d %s=%f %s=%f "
				"%s=%s", i,
				PQfname(res, 0), ntohl(*((uint32_t *) PQgetvalue(res, i, 0))),
				PQfname(res, 1), ntohl(*((uint32_t *) PQgetvalue(res, i, 1))),
				PQfname(res, 2), PQgetvalue(res, i, 2),
				PQfname(res, 3), ntohl(*((uint32_t *) PQgetvalue(res, i, 3))),
				PQfname(res, 4), ntohl(*((uint32_t *) PQgetvalue(res, i, 4))),
				PQfname(res, 5), v5.f,
				PQfname(res, 6), v6.f,
				PQfname(res, 7), PQgetvalue(res, i, 7));
	}
#endif /* DEBUG */
	PQclear(res);

	return OK;
}
