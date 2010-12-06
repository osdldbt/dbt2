/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Labs, Inc.
 * Copyright (C) 2004 Alexey Stroganov & MySQL AB.
 *
 */

#include "common.h"
#include "logging.h"
#include "drizzle_common.h"
#include <stdio.h>

char drizzle_dbname[32] = "dbt2";
char drizzle_host[32] = "localhost";
char drizzle_user[32] = "root";
char drizzle_pass[32] = "";
char drizzle_port[32] = "4427";
char drizzle_socket_t[256] = "/tmp/drizzle.sock";

int commit_transaction(struct db_context_t *dbc)
{

      (void)drizzle_query_str(dbc->drizzle_con, dbc->drizzle_result_set, "COMMIT", &drizzle_return);
      if (drizzle_return != DRIZZLE_RETURN_OK)
      {
        printf("drizzle_query COMMIT failed: %s\n", drizzle_con_error(dbc->drizzle_con));
        return ERROR;
      }
      return OK;
}

/* Open a connection to the database. */
int _connect_to_db(struct db_context_t *dbc)
{
    dbc->drizzle= drizzle_create(NULL);
    if (dbc->drizzle == NULL)
    {
      printf("drizzle_create failed\n");
      return ERROR;
    }

    dbc->drizzle_con= drizzle_con_create(dbc->drizzle, NULL);
    if (dbc->drizzle_con == NULL)
    {
      printf("drizzle_con_create failed\n");
      return ERROR;
    }

    drizzle_con_set_tcp(dbc->drizzle_con, drizzle_host, atoi(drizzle_port));
    drizzle_con_set_db(dbc->drizzle_con, drizzle_dbname);
    drizzle_con_set_options(dbc->drizzle_con, DRIZZLE_CON_MYSQL);

    /* Disable AUTOCOMMIT mode for connection */
    (void)drizzle_query_str(dbc->drizzle_con, dbc->drizzle_result_set, "SET AUTOCOMMIT=0", &drizzle_return);
    if (drizzle_return != DRIZZLE_RETURN_OK)
    {
      printf("drizzle_query AUTOCOMMIT=0: %s\n", drizzle_con_error(dbc->drizzle_con));
      return ERROR;
    }


    return OK;
}

/* Disconnect from the database and free the connection handle. */
int _disconnect_from_db(struct db_context_t *dbc)
{
	drizzle_con_free(dbc->drizzle_con);
	drizzle_free(dbc->drizzle);
	return OK;
}

int _db_init(char * _drizzle_dbname, char *_drizzle_host, char * _drizzle_user,
             char * _drizzle_pass, char * _drizzle_port, char * _drizzle_socket)
{
	/* Copy values only if it's not NULL. */
	if (_drizzle_dbname != NULL) {
		strcpy(drizzle_dbname, _drizzle_dbname);
	}
	if (_drizzle_host != NULL) {
		strcpy(drizzle_host, _drizzle_host);
        }
	if (_drizzle_user != NULL) {
		strcpy(drizzle_user, _drizzle_user);
        }
	if (_drizzle_pass != NULL) {
		strcpy(drizzle_pass, _drizzle_pass);
	}
	if (_drizzle_port != NULL) {
		strcpy(drizzle_port, _drizzle_port);
	}
	if (_drizzle_socket != NULL) {
		strcpy(drizzle_socket_t, _drizzle_socket);
	}
	return OK;
}

int rollback_transaction(struct db_context_t *dbc)
{
      LOG_ERROR_MESSAGE("ROLLBACK INITIATED\n");

      (void)drizzle_query_str(dbc->drizzle_con, dbc->drizzle_result_set, "ROLLBACK", &drizzle_return);
      if (drizzle_return != DRIZZLE_RETURN_OK)
      {
        printf("drizzle_query ROLLBACK failed: %s\n", drizzle_con_error(dbc->drizzle_con));
        return ERROR;
      }

      return STATUS_ROLLBACK;
}

/* NOTE that both db_context_t and sql_result_t have a
   drizzle_result_t, dbc->drizzle_result_set and sql_result->result_set
   When calling drizzle_query_str, second argment is a drizzle_result_t type.
   so in dbt2_sql_execute, we will use sql_result->result_set. Other routines
   will use dbc->drizzle_result_set but do not do anything with the results
*/
int dbt2_sql_execute(struct db_context_t *dbc, char * query, struct sql_result_t * sql_result, char * query_name)
{

      sql_result->result_set= NULL;
      sql_result->num_columns= 0;
      sql_result->num_rows= 0;
      sql_result->query=query;

      sql_result->result_set= drizzle_query_str(dbc->drizzle_con, NULL, query, &drizzle_return);
      if (drizzle_return != DRIZZLE_RETURN_OK)
      {
        LOG_ERROR_MESSAGE("drizzle_query %s failed: %s\n", query, drizzle_con_error(dbc->drizzle_con));
        return 0;
      }
      else
      {
        drizzle_return = drizzle_result_buffer(sql_result->result_set);
        if (drizzle_return != DRIZZLE_RETURN_OK)
        {
           printf("drizzle_result_buffer:%s\n", drizzle_con_error(dbc->drizzle_con));
           return 0;

 	/* if we have rows return then we have a SELECT statement */
        }
        if (sql_result->result_set->row_count)
        {
           sql_result->num_columns= drizzle_result_column_count(sql_result->result_set);
           sql_result->num_rows= sql_result->result_set->row_count;
        }
        else   /* else we have INSERT, UPDATE, DELETE statements */
        {
             sql_result->num_rows = drizzle_result_affected_rows(sql_result->result_set);
        }
      }

      return 1;
}

int dbt2_sql_fetchrow(struct db_context_t *dbc, struct sql_result_t * sql_result)
{
  sql_result->current_row= drizzle_row_next(sql_result->result_set);
  if (sql_result->current_row)
  {
    sql_result->lengths= drizzle_row_field_sizes(sql_result->result_set);
    return 1;
  }
  return 0;
}

int dbt2_sql_close_cursor(struct db_context_t *dbc, struct sql_result_t * sql_result)
{

  if (sql_result->result_set)
  {
    drizzle_result_free(sql_result->result_set);
  }

  return 1;
}


char * dbt2_sql_getvalue(struct db_context_t *dbc, struct sql_result_t * sql_result, int field)
{
  char * tmp;

  tmp= NULL;

  if (sql_result->current_row && field < sql_result->num_columns)
  {
    if (sql_result->current_row[field])
    {
      if ((tmp = calloc(sizeof(char), sql_result->lengths[field]+1)))
      {
        memcpy(tmp, (sql_result->current_row)[field], sql_result->lengths[field]);
      }
      else
      {
        LOG_ERROR_MESSAGE("dbt2_sql_getvalue: CALLOC FAILED for value from field=%d\n", field);
      }
    }
    else
    {
#ifdef DEBUG_QUERY
      LOG_ERROR_MESSAGE("dbt2_sql_getvalue: var[%d]=NULL\n", field);
#endif
    }
  }
  else
  {
#ifdef DEBUG_QUERY
    LOG_ERROR_MESSAGE("dbt2_sql_getvalue: POSSIBLE NULL VALUE or ERROR\nQuery: %s\nField: %d from %d", sql_result->query, field, sql_result->num_columns);
#endif
  }
  return tmp;
}

