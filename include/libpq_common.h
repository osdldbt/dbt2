/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Labs, Inc.
 *
 * May 13 2003
 */

#ifndef _LIGPQ_COMMON_H_
#define _LIGPQ_COMMON_H_

#include "transaction_data.h"

struct ligpq_context_t {
	char dbname[32];
	char pghost[32];
	char pgport[32];
	char pgoptions[32];
	char pgtty[32];
};

int libpq_connect(struct ligpq_context_t *pqc, PGconn *conn);
int libpq_disconnect(PGconn *conn);
int lipq_init(struct ligpq_context_t *pqc, char *dbname, char *pghost,
	char *pgport, char *pgoptions, char *pgtty);

#endif /* _LIGPQ_COMMON_H_ */
