/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <wchar.h>

#include "common.h"
#include "logging.h"
#include "libpq_order_status.h"

#define UDF_ORDER_STATUS "SELECT * FROM order_status($1, $2, $3, $4)"

int execute_order_status_libpq(struct db_context_t *dbc, struct order_status_t *data)
{
	PGresult *res;
	const char *paramValues[4];
	int paramLengths[4] = {
			sizeof(uint32_t), sizeof(uint32_t), sizeof(uint32_t), 0
	};
	const int paramFormats[4] = {1, 1, 1, 1};
	char c_last[4 * (C_LAST_LEN + 1)];

	uint32_t c_id;
	uint32_t c_w_id;
	uint32_t c_d_id;

#ifdef DEBUG
	int i;
#endif /* DEBUG */

	c_id = htonl((uint32_t) data->c_id);
	c_w_id = htonl((uint32_t) data->c_w_id);
	c_d_id = htonl((uint32_t) data->c_d_id);
	wcstombs(c_last, data->c_last, 4 * (C_LAST_LEN + 1));

#ifdef DEBUG
	LOG_ERROR_MESSAGE("OS c_id %d c_w_id %d c_d_id %d c_last %s",
			data->c_id, data->c_w_id, data->c_d_id, c_last);
#endif /* DEBUG */

	paramValues[0] = (char *) &c_id;
	paramValues[1] = (char *) &c_w_id;
	paramValues[2] = (char *) &c_d_id;
	paramValues[3] = c_last;

	paramLengths[3] = strlen(c_last);

	/* Start a transaction block. */
	res = PQexec(dbc->library.libpq.conn, "BEGIN");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	res = PQexecParams(dbc->library.libpq.conn, UDF_ORDER_STATUS, 4, NULL,
			paramValues, paramLengths, paramFormats, 1);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		LOG_ERROR_MESSAGE("OS %s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
#ifdef DEBUG
	for (i = 0; i < PQntuples(res); i++) {
		union
		{
			float f;
			uint32_t i;
		} v2, v3;
		uint64_t v4;
		time_t time4;
		uint32_t v4mantissa;
		struct tm *tm4;

		v2.i = ntohl(*((uint32_t *) PQgetvalue(res, i, 2)));
		v3.i = ntohl(*((uint32_t *) PQgetvalue(res, i, 3)));
		v4 = ntohll(*((uint64_t *) PQgetvalue(res, i, 4)));
		
		time4 = v4 / (uint64_t) 1000000 +
				(uint64_t) (POSTGRES_EPOCH_JDATE - UNIX_EPOCH_JDATE) *
						(uint64_t) SECS_PER_DAY;
		v4mantissa = v4 - (uint64_t) (time4 -
				(POSTGRES_EPOCH_JDATE - UNIX_EPOCH_JDATE) * SECS_PER_DAY) *
					(uint64_t) 1000000 ;

		/* For ease of coding, assume and print timestamps in GMT. */
		tm4 = gmtime(&time4);

		LOG_ERROR_MESSAGE("OS[%d] %s=%d %s=%d %s=%f %s=%f "
				"%s=%04d-%02d-%02d %02d:%02d:%02d.%6d", i,
				PQfname(res, 0), ntohl(*((uint32_t *) PQgetvalue(res, i, 0))),
				PQfname(res, 1), ntohl(*((uint32_t *) PQgetvalue(res, i, 1))),
				PQfname(res, 2), v2.f,
				PQfname(res, 3), v3.f,
				PQfname(res, 4),
				tm4->tm_year + 1900, tm4->tm_mon + 1, tm4->tm_mday,
				tm4->tm_hour, tm4->tm_min, tm4->tm_sec, v4mantissa);
	}
#endif /* DEBUG */
	PQclear(res);

	return OK;
}
