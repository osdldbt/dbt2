/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *  
 * Copyright (C) 2002 Mark Wong & Open Source Development Labs, Inc.
 *
 * 11 june 2002
 */

#ifndef _ODBC_COMMON_H_
#define _ODBC_COMMON_H_

#include "common.h"
#include "logging.h"
#include "transaction_data.h"

#ifdef IODBC
#include <isql.h>
#include <isqlext.h>
#include <isqltypes.h>
#else /* UNIXODBC */
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h> 
#endif

#ifndef SQLLEN
#define SQLLEN SQLINTEGER
#endif


#define LOG_ODBC_ERROR(type, handle) log_odbc_error(__FILE__, __LINE__, type, handle)

#define COMMIT "COMMIT"
#define ROLLBACK "ROLLBACK"

struct db_context_t {
	SQLHDBC hdbc;
	SQLHSTMT hstmt;
};

struct sql_result_t
{
  unsigned int current_row;
  unsigned int result_set;
  SQLSMALLINT num_fields;
  SQLLEN num_rows;
  SQLUINTEGER * lengths;
  char * query;
};


int check_odbc_rc(SQLSMALLINT handle_type, SQLHANDLE handle, SQLRETURN rc);
int log_odbc_error(char *filename, int line, SQLSMALLINT handle_type,
	SQLHANDLE handle);
int commit_transaction(struct db_context_t *dbc);
int _connect_to_db(struct db_context_t *odbcc);
int _disconnect_from_db(struct db_context_t *odbcc);
int _db_init(char *sname, char *uname, char *auth);
int rollback_transaction(struct db_context_t *dbc);


int dbt2_sql_execute(struct db_context_t *dbc, char * query,
                     struct sql_result_t * sql_result, char * query_name);
int dbt2_sql_close_cursor(struct db_context_t *dbc, struct sql_result_t * sql_result);
int dbt2_sql_fetchrow(struct db_context_t *dbc, struct sql_result_t * sql_result);
char * dbt2_sql_getvalue(struct db_context_t *dbc, struct sql_result_t * sql_result,
                         int field);


#endif /* _ODBC_COMMON_H_ */
