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
#include <wchar.h>

#include "common.h"
#include "logging.h"
#include "libpq_payment.h"

#define UDF_PAYMENT "SELECT * FROM payment($1, $2, $3, $4, $5, $6, $7)"

int execute_payment_libpq(struct db_context_t *dbc, struct payment_t *data)
{
	PGresult *res;
	const char *paramValues[7];
	const int paramFormats[7] = {1, 1, 1, 1, 1, 1, 1};
	int paramLengths[7] = {
		sizeof(uint32_t), sizeof(uint32_t), sizeof(uint32_t), sizeof(uint32_t),
		sizeof(uint32_t), 0, sizeof(uint32_t)
	};
	char c_last[4 * (C_LAST_LEN + 1)];

	uint32_t w_id;
	uint32_t d_id;
	uint32_t c_id;
	uint32_t c_w_id;
	uint32_t c_d_id;
	uint32_t lh_amount;
	union
	{
		float f;
		uint32_t i;
	}	h_amount;

#ifdef DEBUG
	int i, j;
#endif /* DEBUG */

	w_id = htonl((uint32_t) data->w_id);
	d_id = htonl((uint32_t) data->d_id);
	c_id = htonl((uint32_t) data->c_id);
	c_w_id = htonl((uint32_t) data->c_w_id);
	c_d_id = htonl((uint32_t) data->c_d_id);
	h_amount.f = data->h_amount;
	lh_amount = htonl((uint32_t) h_amount.i);
	wcstombs(c_last, data->c_last, 4 * (C_LAST_LEN + 1));

#ifdef DEBUG
		LOG_ERROR_MESSAGE("P w_id %d d_id %d c_id %d c_w_id %d c_d_id %d "
				"c_last %s h_amount %f",
				data->w_id, data->d_id, data->c_id, data->c_w_id, data->c_d_id,
				c_last, data->h_amount);
#endif /* DEBUG */

	paramValues[0] = (char *) &w_id;
	paramValues[1] = (char *) &d_id;
	paramValues[2] = (char *) &c_id;
	paramValues[3] = (char *) &c_w_id;
	paramValues[4] = (char *) &c_d_id;
	paramValues[5] = c_last;
	paramValues[6] = (char *) &lh_amount;

	paramLengths[5] = strlen(c_last);

	/* Start a transaction block. */
	res = PQexec(dbc->library.libpq.conn, "BEGIN");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	res = PQexecParams(dbc->library.libpq.conn, UDF_PAYMENT, 7, NULL,
			paramValues, paramLengths, paramFormats, 1);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		LOG_ERROR_MESSAGE("P %s", PQerrorMessage(dbc->library.libpq.conn));
		PQclear(res);
		return ERROR;
	}
#ifdef DEBUG
	for (i = 0; i < PQntuples(res); i++) {
		union
		{
			float f;
			uint32_t i;
		}	v22;
		uint64_t v19, v25;
		time_t time19, time25;
		struct tm *tm19, *tm25;
		uint32_t v19mantissa, v25mantissa;
		uint16_t *num;

		v22.i = ntohl(*((uint32_t *) PQgetvalue(res, i, 22)));
		v19 = ntohll(*((uint64_t *) PQgetvalue(res, i, 19)));
		v25 = ntohll(*((uint64_t *) PQgetvalue(res, i, 25)));

		/* For ease of coding, assume and print timestamps in GMT. */

		time19 = v19 / (uint64_t) 1000000 +
				(uint64_t) (POSTGRES_EPOCH_JDATE - UNIX_EPOCH_JDATE) *
				(uint64_t) SECS_PER_DAY;
		tm19 = gmtime(&time19);
		v19mantissa = v19 - (uint64_t) (time19 -
				(POSTGRES_EPOCH_JDATE - UNIX_EPOCH_JDATE) * SECS_PER_DAY) *
				(uint64_t) 1000000;

		time25 = v25 / (uint64_t) 1000000 +
				(uint64_t) (POSTGRES_EPOCH_JDATE - UNIX_EPOCH_JDATE) *
				(uint64_t) SECS_PER_DAY;
		tm25 = gmtime(&time25);
		v25mantissa = v25 - (uint64_t) (time25 -
				(POSTGRES_EPOCH_JDATE - UNIX_EPOCH_JDATE) * SECS_PER_DAY) *
				(uint64_t) 1000000;

		LOG_ERROR_MESSAGE("P[%d] %s=%s %s=%s %s=%s %s=%s %s=%s %s=%s %s=%s "
				"%s=%s %s=%s %s=%s %s=%s %s=%s %s=%s %s=%s %s=%s %s=%s %s=%s "
				"%s=%s %s=%s %s=%04d-%02d-%02d %02d:%02d:%02d.%06d %s=%s %s=%f "
				"%s=%s %s=%04d-%02d-%02d %02d:%02d:%2d.%06d", i,
				PQfname(res, 0), PQgetvalue(res, i, 0),
				PQfname(res, 1), PQgetvalue(res, i, 1),
				PQfname(res, 2), PQgetvalue(res, i, 2),
				PQfname(res, 3), PQgetvalue(res, i, 3),
				PQfname(res, 4), PQgetvalue(res, i, 4),
				PQfname(res, 5), PQgetvalue(res, i, 5),
				PQfname(res, 6), PQgetvalue(res, i, 6),
				PQfname(res, 7), PQgetvalue(res, i, 7),
				PQfname(res, 8), PQgetvalue(res, i, 8),
				PQfname(res, 9), PQgetvalue(res, i, 9),
				PQfname(res, 10), PQgetvalue(res, i, 10),
				PQfname(res, 11), PQgetvalue(res, i, 11),
				PQfname(res, 12), PQgetvalue(res, i, 12),
				PQfname(res, 13), PQgetvalue(res, i, 13),
				PQfname(res, 14), PQgetvalue(res, i, 14),
				PQfname(res, 15), PQgetvalue(res, i, 15),
				PQfname(res, 16), PQgetvalue(res, i, 16),
				PQfname(res, 17), PQgetvalue(res, i, 17),
				PQfname(res, 18), PQgetvalue(res, i, 18),
				PQfname(res, 19),
				tm19->tm_year + 1900, tm19->tm_mon + 1, tm19->tm_mday,
				tm19->tm_hour, tm19->tm_min, tm19->tm_sec, v19mantissa,
				PQfname(res, 20), PQgetvalue(res, i, 20),
				PQfname(res, 22), v22.f,
				PQfname(res, 24), PQgetvalue(res, i, 24),
				PQfname(res, 25),
				tm25->tm_year + 1900, tm25->tm_mon + 1, tm25->tm_mday,
				tm25->tm_hour, tm25->tm_min, tm25->tm_sec, v25mantissa);
		/*
		 * For ease of coding, don't reconstruct the numerics, just dump out
		 * each segment.
		 */
		num = (uint16_t *) PQgetvalue(res, i, 21);
		if (num) {
			LOG_ERROR_MESSAGE("%s %u %u %u %u", PQfname(res, 21), ntohs(num[0]),
					ntohs(num[1]), ntohs(num[2]), ntohs(num[3]));
			for (j = 4; j < PQgetlength(res, i, 21) / sizeof(uint16_t); j++)
				LOG_ERROR_MESSAGE("%d %s %u", j, PQfname(res, 21),
				ntohs(num[j]));
		} else
				LOG_ERROR_MESSAGE("%d %s is NULL", i, PQfname(res, 21));

		num = (uint16_t *) PQgetvalue(res, i, 23);
		if (num) {
			LOG_ERROR_MESSAGE("%s %u %u %u %u", PQfname(res, 23), ntohs(num[0]),
					ntohs(num[1]), ntohs(num[2]), ntohs(num[3]));
			for (j = 4; j < PQgetlength(res, i, 23) / sizeof(uint16_t); j++)
				LOG_ERROR_MESSAGE("%d %s %u", j, PQfname(res, 23),
				ntohs(num[j]));
		} else
				LOG_ERROR_MESSAGE("%d %s is NULL", i, PQfname(res, 23));
	}
#endif /* DEBUG */
	PQclear(res);

	return OK;
}
