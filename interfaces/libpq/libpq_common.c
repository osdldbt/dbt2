/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Labs, Inc.
 *
 * 13 May 2003
 */

#include <stdio.h>

#include "common.h"
#include "logging.h"
#include "libpq_common.h"

char dbname[32] = "";
char pghost[32] = "";
char pgport[32] = "";
char pgoptions[32] = "";
char pgtty[32] = "";

/* Open a connection to the database. */
int libpq_connect(struct db_context_t *dbc)
{
	dbc->conn = PQsetdb(pghost, pgport, pgoptions, pgtty, dbname);

	if (PQstatus(dbc->conn) == CONNECTION_BAD) {
		LOG_ERROR_MESSAGE("Connection to database '%s' failed.",
			dbname);
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->conn));
		PQfinish(dbc->conn);
		return ERROR;
	}
	return OK;
}

/* Disconnect from the database and free the connection handle. */
int libpq_disconnect(struct db_context_t *dbc)
{
	PQfinish(dbc->conn);
	return OK;
}

int lipq_init(char *_dbname, char *_pghost, char *_pgport, char *_pgoptions,
	char *_pgtty)
{
	/* Copy values only if it's not NULL. */
	if (_pghost != NULL) {
		strcpy(pghost, _pghost);
	}
	if (_pgport != NULL) {
		strcpy(pgport, _pgport);
	}
	if (_pgoptions != NULL) {
		strcpy(pgoptions, _pgoptions);
	}
	if (_pgtty != NULL) {
		strcpy(pgtty, _pgtty);
	}
	return OK;
}
