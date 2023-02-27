/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 */

#include <stdio.h>

#include "common.h"
#include "logging.h"
#include "libpq_delivery.h"

#define UDF_DELIVERY "SELECT * FROM delivery($1, $2)"

int execute_delivery_libpq(struct db_context_t *dbc, struct delivery_t *data)
{
	PGresult *res;
	const char *paramValues[2];
	const int paramLengths[2] = {sizeof(uint32_t), sizeof(uint32_t)};
	const int paramFormats[2] = {1, 1};

	uint32_t w_id;
	uint32_t o_carrier_id;

#ifdef DEBUG
	int i;

	LOG_ERROR_MESSAGE("D d_w_id %d o_carrier_id %d",
			data->w_id, data->o_carrier_id);
#endif /* DEBUG */

	w_id = htonl((uint32_t) data->w_id);
	o_carrier_id = htonl((uint32_t) data->o_carrier_id);

	paramValues[0] = (char *) &w_id;
	paramValues[1] = (char *) &o_carrier_id;

	/* Start a transaction block. */
	res = PQexec(dbc->library.libpq.conn, "BEGIN");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	res = PQexecParams(dbc->library.libpq.conn, UDF_DELIVERY, 2, NULL, paramValues,
			paramLengths, paramFormats, 1);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		LOG_ERROR_MESSAGE("D %s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
#ifdef DEBUG
	for (i = 0; i < PQntuples(res); i++) {
		LOG_ERROR_MESSAGE("SL[%d] %s=%d %s=%d", i,
				PQfname(res, 0), ntohl(*((uint32_t *) PQgetvalue(res, i, 0))),
				PQfname(res, 1), ntohl(*((uint32_t *) PQgetvalue(res, i, 1))));
	}
#endif /* DEBUG */
	PQclear(res);

	return OK;
}
