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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "transaction_data.h"

#ifdef ODBC
#include "odbc_common.h"
#endif /* ODBC */

#ifdef LIBPQ
#include "libpq_common.h"
#endif /* LIBPQ */

#ifdef LIBMYSQL
#include "mysql_common.h"
#endif /* LIBMYSQL */


int connect_to_db(struct db_context_t *dbc);
#ifdef ODBC
int db_init(char *sname, char *uname, char *auth);
#endif /* ODBC */
#ifdef LIBPQ
int db_init(char *_dbname, char *_pghost, char *_pgport);
#endif /* LIBPQ */

#ifdef LIBMYSQL
int db_init(char * _mysql_dbname, char *_mysql_host, char * _mysql_user,
            char * _mysql_pass, char * _mysql_port, char * _mysql_socket);
#endif /* LIBMYSQL */

int disconnect_from_db(struct db_context_t *dbc);
int process_transaction(int transaction, struct db_context_t *dbc,
	union transaction_data_t *odbct);

#endif /* _DB_H_ */
