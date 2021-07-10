/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002      Open Source Development Labs, Inc.
 *               2002-2021 Mark Wong
 *
 * 13 May 2003
 */

#include <stdio.h>
#include <wchar.h>

#include "common.h"
#include "logging.h"
#include "libpq_payment.h"

int execute_payment(struct db_context_t *dbc, struct payment_t *data)
{
	PGresult *res;
	wchar_t wcstmt[128];
	char stmt[512];


	/* Start a transaction block. */
	res = PQexec(dbc->conn, "BEGIN");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->conn));
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	/* Create the query and execute it. */
	swprintf(wcstmt, 128, L"SELECT payment(%d, %d, %d, %d, %d, '%ls', %f)",
		data->w_id, data->d_id, data->c_id, data->c_w_id, data->c_d_id,
		data->c_last, data->h_amount);
	wcstombs(stmt, wcstmt, 512);
	res = PQexec(dbc->conn, stmt);
	if (!res || (PQresultStatus(res) != PGRES_COMMAND_OK &&
		PQresultStatus(res) != PGRES_TUPLES_OK)) {
		LOG_ERROR_MESSAGE("%s\n%s", stmt, PQerrorMessage(dbc->conn));
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	return OK;
}
