/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Labs, Inc.
 *
 * 16 June 2002
 */

#ifndef _DB_H_
#define _DB_H_

#ifdef ODBC
#include <odbc_common.h>
#include <client_interface.h>

int connect_to_db(struct db_context_t *dbc);
int process_transaction(int transaction, struct db_context_t *dbc,
	union transaction_data_t *odbct);
#endif /* ODBC */

#endif /* _DB_H_ */
