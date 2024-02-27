/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 */

#include <libpq-fe.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "db.h"
#include "libpq_delivery.h"
#include "libpq_integrity.h"
#include "libpq_new_order.h"
#include "libpq_order_status.h"
#include "libpq_payment.h"
#include "libpq_stock_level.h"
#include "logging.h"

int commit_transaction_libpq(struct db_context_t *dbc) {
	PGresult *res;

	res = PQexec(dbc->library.libpq.conn, "COMMIT");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		if (PQresultStatus(res) == PGRES_FATAL_ERROR &&
			strcmp("no connection to the server\n",
				   PQerrorMessage(dbc->library.libpq.conn)) == 0) {
			PQclear(res);
			return RECONNECT;
		}
	}
	PQclear(res);

	return OK;
}

/* Open a connection to the database. */
int connect_to_db_libpq(struct db_context_t *dbc) {
	const char *application_name = "dbt2-client";
	const char *keywords[5] = {
			"application_name", "host", "port", "dbname", NULL};
	const char *values[5];

	values[0] = application_name;
	values[1] = dbc->library.libpq.pghost;
	values[2] = dbc->library.libpq.pgport;
	values[3] = dbc->library.libpq.dbname;
	values[4] = NULL;

	dbc->library.libpq.conn = PQconnectdbParams(keywords, values, 1);
	if (PQstatus(dbc->library.libpq.conn) != CONNECTION_OK) {
		LOG_ERROR_MESSAGE(
				"Connection to database '%s' failed.",
				dbc->library.libpq.dbname);
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		PQfinish(dbc->library.libpq.conn);
		return ERROR;
	}
	return OK;
}

/* Disconnect from the database and free the connection handle. */
int disconnect_from_db_libpq(struct db_context_t *dbc) {
	PQfinish(dbc->library.libpq.conn);
	return OK;
}

int db_init_libpq(
		struct db_context_t *dbc, char *dbname, char *pghost, char *pgport) {
	dbc->connect = connect_to_db_libpq;
	dbc->commit_transaction = commit_transaction_libpq;
	dbc->disconnect = disconnect_from_db_libpq;
	dbc->rollback_transaction = rollback_transaction_libpq;
	dbc->execute_delivery = execute_delivery_libpq;
	dbc->execute_integrity = execute_integrity_libpq;
	dbc->execute_new_order = execute_new_order_libpq;
	dbc->execute_order_status = execute_order_status_libpq;
	dbc->execute_payment = execute_payment_libpq;
	dbc->execute_stock_level = execute_stock_level_libpq;

	if (dbname != NULL) {
		strncpy(dbc->library.libpq.dbname, dbname,
				sizeof(dbc->library.libpq.dbname));
	}
	if (pghost != NULL) {
		strncpy(dbc->library.libpq.pghost, pghost,
				sizeof(dbc->library.libpq.pghost));
	}
	if (pgport != NULL) {
		strncpy(dbc->library.libpq.pgport, pgport,
				sizeof(dbc->library.libpq.pgport));
	}

	return OK;
}

int rollback_transaction_libpq(struct db_context_t *dbc) {
	PGresult *res;

	res = PQexec(dbc->library.libpq.conn, "ROLLBACK");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
		if (PQresultStatus(res) == PGRES_FATAL_ERROR &&
			strcmp("no connection to the server\n",
				   PQerrorMessage(dbc->library.libpq.conn)) == 0) {
			PQclear(res);
			return RECONNECT;
		}
	}
	PQclear(res);

	return STATUS_ROLLBACK;
}
