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
#include "libpq_common.h"

/* Open a connection to the database. */
int libpq_connect(struct ligpq_context_t *pqc, PGconn *conn)
{
	conn = PQsetdb(pqc->pghost, pqc->pgport, pqc->pgoptions, pqc->pgtty,
		pqc->dbname);

	if (PQstatus(conn) == CONNECTION_BAD) {
		LOG_ERROR_MESSAGE("Connection to database '%s' failed.",
			pqc->dbname);
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(conn));
		PQfinish(conn);
		return ERROR;
	}
	return OK;
}

/* Disconnect from the database and free the connection handle. */
int libpq_disconnect(PGconn *conn)
{
	PQfinish(conn);
	return OK;
}

int lipq_init(struct ligpq_context_t *pqc, char *dbname, char *pghost,
	char *pgport, char *pgoptions, char *pgtty)
{
	/* Initialize the data structure. */
	pqc->dbname[0] = '\0';
	pqc->pghost[0] = '\0';
	pqc->pgport[0] = '\0';
	pqc->pgoptions[0] = '\0';
	pqc->pgtty[0] = '\0';

	/* Copy values only if it's not NULL. */
	if (pghost != NULL) {
		strcpy(pqc->pghost, pghost);
	}
	if (pgport != NULL) {
		strcpy(pqc->pgport, pgport);
	}
	if (pgoptions != NULL) {
		strcpy(pqc->pgoptions, pgoptions);
	}
	if (pgtty != NULL) {
		strcpy(pqc->pgtty, pgtty);
	}
	return OK;
}
