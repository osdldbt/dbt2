/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Labs, Inc.
 *
 * 13 May 2003
 */

#include <pthread.h>
#include <stdio.h>
#include <postgresql/libpq-fe.h>

#include "common.h"
#include "logging.h"
#include "libpq_delivery.h"

int execute_delivery(PGconn *conn, struct delivery_t *data)
{
	PGresult *res;
	char stmt[128];

	/* Start a transaction block. */
	res = PQexec(conn, "BEGIN");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOG_ERROR_MESSAGE("BEGIN command failed.\n");
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	/* Create the query and execute it. */
	sprintf(stmt, "SELECT delivery(%d, %d)",
		data->w_id, data->o_carrier_id);
	res = PQexec(conn, stmt);
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOG_ERROR_MESSAGE("SELECT failed\n");
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	/* Commit the transaction. */
	res = PQexec(conn, "COMMIT");
	PQclear(res);

	return OK;
}
