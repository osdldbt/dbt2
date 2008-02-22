/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Labs, Inc.
 * Copyright (C) 2008 Steve VanDeBogart & UC Regents.
 *
 */

#ifndef _SQLITE_COMMON_H_
#define _SQLITE_COMMON_H_

#include <sqlite3.h>
#include <string.h>
#include "transaction_data.h"

struct db_context_t {
	unsigned char inTransaction;
	sqlite3 * db;
};

struct sql_result_t
{
#define result_set query_running
  unsigned char query_running;
  unsigned char error;
  unsigned char prefetched;
  unsigned int num_fields;
  sqlite3_stmt * pStmt;
};

extern char sqlite_dbname[256];

int commit_transaction(struct db_context_t *dbc);
int _connect_to_db(struct db_context_t *dbc);
int _disconnect_from_db(struct db_context_t *dbc);
int _db_init(char *_dbname);
int rollback_transaction(struct db_context_t *dbc);

int dbt2_sql_execute(struct db_context_t *dbc, char * query,
                     struct sql_result_t * sql_result, char * query_name);
int dbt2_sql_close_cursor(struct db_context_t *dbc, struct sql_result_t * sql_result);
int dbt2_sql_fetchrow(struct db_context_t *dbc, struct sql_result_t * sql_result);
char * dbt2_sql_getvalue(struct db_context_t *dbc, struct sql_result_t * sql_result,
                         int field);

#endif /* _SQLITE_COMMON_H_ */

