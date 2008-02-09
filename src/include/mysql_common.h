/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Labs, Inc.
 *
 * May 13 2003
 */

#ifndef _MYSQL_COMMON_H_
#define _MYSQL_COMMON_H_

#include <mysql.h>
#include <string.h>
#include "transaction_data.h"

struct db_context_t {
	MYSQL * mysql;
};

struct sql_result_t
{
  MYSQL_RES * result_set;
  MYSQL_ROW current_row;
  unsigned int num_fields;
#define HAVE_SQL_RESULT_NUM_ROWS
  unsigned int num_rows;
  unsigned long * lengths;
  char * query;
};

extern char mysql_dbname[32];
extern char mysql_host[32];
extern char mysql_port_t[32];
extern char mysql_user[32];
extern char mysql_pass[32];
extern char mysql_socket_t[256];

int commit_transaction(struct db_context_t *dbc);
int _connect_to_db(struct db_context_t *dbc);
int _disconnect_from_db(struct db_context_t *dbc);
int _db_init(char *_mysql_dbname, char *_mysql_host, char * _mysql_user, char * _mysql_pass, 
             char *_mysql_port, char * _mysql_socket);
int rollback_transaction(struct db_context_t *dbc);

int dbt2_sql_execute(struct db_context_t *dbc, char * query,
                     struct sql_result_t * sql_result, char * query_name);
int dbt2_sql_close_cursor(struct db_context_t *dbc, struct sql_result_t * sql_result);
int dbt2_sql_fetchrow(struct db_context_t *dbc, struct sql_result_t * sql_result);
char * dbt2_sql_getvalue(struct db_context_t *dbc, struct sql_result_t * sql_result,
                         int field);

#endif /* _MYSQL_COMMON_H_ */

