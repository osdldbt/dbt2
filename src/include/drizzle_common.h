/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Labs, Inc.
 *
 * May 13 2003
 */

#ifndef _DRIZZLE_COMMON_H_
#define _DRIZZLE_COMMON_H_

#include <libdrizzle/drizzle_client.h>
#include <string.h>
#include "transaction_data.h"

drizzle_return_t drizzle_return;

struct db_context_t {
  drizzle_st * drizzle;
  drizzle_con_st * drizzle_con;
  drizzle_result_st * drizzle_result_set;
};

struct sql_result_t
{
  drizzle_result_st * result_set;
  drizzle_row_t current_row;
  /*char **current_row;*/
  unsigned int num_columns;
  unsigned int num_rows;
  size_t * lengths;
  char * query;
};

extern char drizzle_dbname[32];
extern char drizzle_host[32];
extern char drizzle_port[32];
extern char drizzle_user[32];
extern char drizzle_pass[32];
extern char drizzle_socket_t[256];

int commit_transaction(struct db_context_t *dbc);
int _connect_to_db(struct db_context_t *dbc);
int _disconnect_from_db(struct db_context_t *dbc);
int _db_init(char *_drizzle_dbname, char *_drizzle_host, char * _drizzle_user, char * _drizzle_pass,
             char *_drizzle_port, char * _drizzle_socket);
int rollback_transaction(struct db_context_t *dbc);

int dbt2_sql_execute(struct db_context_t *dbc, char * query,
                     struct sql_result_t * sql_result, char * query_name);
int dbt2_sql_close_cursor(struct db_context_t *dbc, struct sql_result_t * sql_result);
int dbt2_sql_fetchrow(struct db_context_t *dbc, struct sql_result_t * sql_result);
char * dbt2_sql_getvalue(struct db_context_t *dbc, struct sql_result_t * sql_result,
                         int field);

#endif /* _DRIZZLE_COMMON_H_ */

