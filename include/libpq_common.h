/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Labs, Inc.
 *
 * May 13 2003
 */

#ifndef _LIBPQ_COMMON_H_
#define _LIBPQ_COMMON_H_

#include <postgresql/libpq-fe.h>

#include "transaction_data.h"

struct db_context_t {
	PGconn *conn;
};

int libpq_connect(struct db_context_t *dbc);
int libpq_disconnect(struct db_context_t *dbc);
int lipq_init(char *_dbname, char *_pghost, char *_pgport, char *_pgoptions,
	char *_pgtty);

extern char dbname[32];
extern char pghost[32];
extern char pgport[32];
extern char pgoptions[32];
extern char pgtty[32];

#endif /* _LIBPQ_COMMON_H_ */
