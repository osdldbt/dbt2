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

#include <libpq-fe.h>

#include "transaction_data.h"

struct db_context_t {
	PGconn *conn;
};

int commit_transaction(struct db_context_t *dbc);
int _connect_to_db(struct db_context_t *dbc);
int _disconnect_from_db(struct db_context_t *dbc);
int _db_init(char *_dbname, char *_pghost, char *_pgport);
int rollback_transaction(struct db_context_t *dbc);

extern char dbname[32];
extern char pghost[32];
extern char pgport[32];

#endif /* _LIBPQ_COMMON_H_ */
