/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 */

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "db.h"
#include "logging.h"

int execute_delivery_cockroach(struct db_context_t *, struct delivery_t *);
int execute_integrity_cockroach(struct db_context_t *, struct integrity_t *);
int execute_new_order_cockroach(struct db_context_t *, struct new_order_t *);
int execute_order_status_cockroach(
		struct db_context_t *, struct order_status_t *);
int execute_payment_cockroach(struct db_context_t *, struct payment_t *);
int execute_stock_level_cockroach(
		struct db_context_t *, struct stock_level_t *);

int commit_transaction_cockroach(struct db_context_t *dbc) {
	PGresult *res;

	res = PQexec(dbc->library.libpq.conn, "COMMIT");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
	}
	PQclear(res);

	return OK;
}

/* Open a connection to the database. */
int connect_to_db_cockroach(struct db_context_t *dbc) {
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
int disconnect_from_db_cockroach(struct db_context_t *dbc) {
	PQfinish(dbc->library.libpq.conn);
	return OK;
}

int db_init_cockroach(
		struct db_context_t *dbc, char *dbname, char *pghost, char *pgport) {
	dbc->connect = connect_to_db_cockroach;
	dbc->commit_transaction = commit_transaction_cockroach;
	dbc->disconnect = disconnect_from_db_cockroach;
	dbc->rollback_transaction = rollback_transaction_cockroach;
	dbc->execute_delivery = execute_delivery_cockroach;
	dbc->execute_integrity = execute_integrity_cockroach;
	dbc->execute_new_order = execute_new_order_cockroach;
	dbc->execute_order_status = execute_order_status_cockroach;
	dbc->execute_payment = execute_payment_cockroach;
	dbc->execute_stock_level = execute_stock_level_cockroach;

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

int rollback_transaction_cockroach(struct db_context_t *dbc) {
	PGresult *res;

	res = PQexec(dbc->library.libpq.conn, "ROLLBACK");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->library.libpq.conn));
	}
	PQclear(res);

	return STATUS_ROLLBACK;
}
