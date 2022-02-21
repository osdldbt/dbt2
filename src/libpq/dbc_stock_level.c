/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002      Open Source Development Labs, Inc.
 * Copyright (C) 2002-2022 Mark Wong
 *
 * 13 May 2003
 */

#include <stdio.h>

#include "common.h"
#include "logging.h"
#include "libpq_stock_level.h"

#define UDF_STOCK_LEVEL "SELECT * FROM stock_level($1, $2, $3)"

int execute_stock_level_libpq(struct db_context_t *dbc, struct stock_level_t *data)
{
	PGresult *res;
	const char *paramValues[3];
	const int paramLengths[3] = {
			sizeof(uint32_t), sizeof(uint32_t), sizeof(uint32_t)
	};
	const int paramFormats[3] = {1, 1, 1};

	uint32_t d_w_id;
	uint32_t d_id;
	uint32_t threshold;

#ifdef DEBUG
	int i;

	LOG_ERROR_MESSAGE("SL d_w_id %d d_id %d threshold %d",
			data->w_id, data->d_id, data->threshold);
#endif /* DEBUG */

	d_w_id = htonl((uint32_t) data->w_id);
	d_id = htonl((uint32_t) data->d_id);
	threshold = htonl((uint32_t) data->threshold);

	paramValues[0] = (char *) &d_w_id;
	paramValues[1] = (char *) &d_id;
	paramValues[2] = (char *) &threshold;

	/* Start a transaction block. */
	res = PQexec(dbc->library.libpq.conn, "BEGIN");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	res = PQexecParams(dbc->library.libpq.conn, UDF_STOCK_LEVEL, 3, NULL,
			paramValues, paramLengths, paramFormats, 1);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		LOG_ERROR_MESSAGE("SL %s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
#ifdef DEBUG
	for (i = 0; i < PQntuples(res); i++) {
		LOG_ERROR_MESSAGE("SL[%d] %s %d", i, PQfname(res, 0),
				ntohl(*((uint32_t *) PQgetvalue(res, i, 0))));
	}
#endif /* DEBUG */
	PQclear(res);

	return OK;
}
