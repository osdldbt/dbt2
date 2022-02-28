/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002      Open Source Development Labs, Inc.
 *               2004      Alexey Stroganov & MySQL AB.
 *               2002-2022 Mark Wong
 *
 */
       
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "logging.h"
#include "db.h"
#include "mysql_delivery.h"
#include "mysql_integrity.h"
#include "mysql_new_order.h"
#include "mysql_order_status.h"
#include "mysql_payment.h"
#include "mysql_stock_level.h"

int commit_transaction_mysql(struct db_context_t *dbc)
{

      if (mysql_real_query(dbc->library.mysql.mysql, "COMMIT", 6))
      {
        LOG_ERROR_MESSAGE("COMMIT failed. mysql reports: %d %s", 
                           mysql_errno(dbc->library.mysql.mysql), mysql_error(dbc->library.mysql.mysql));
        return ERROR;
      }
      return OK;
}

/* Open a connection to the database. */
int connect_to_db_mysql(struct db_context_t *dbc)
{
    dbc->library.mysql.mysql=mysql_init(NULL);

    //FIXME: change atoi() to strtol() and check for errors
    if (!mysql_real_connect(dbc->library.mysql.mysql, dbc->library.mysql.host, dbc->library.mysql.user, dbc->library.mysql.pass, dbc->library.mysql.dbname, atoi(dbc->library.mysql.port_t), dbc->library.mysql.socket_t, 0))
    {
      if (mysql_errno(dbc->library.mysql.mysql))
      {
        LOG_ERROR_MESSAGE("Connection to database '%s' failed.", dbc->library.mysql.dbname);
	LOG_ERROR_MESSAGE("mysql reports: %d %s", 
                           mysql_errno(dbc->library.mysql.mysql), mysql_error(dbc->library.mysql.mysql));
      }
      return ERROR;
    }

    /* Disable AUTOCOMMIT mode for connection */
    if (mysql_real_query(dbc->library.mysql.mysql, "SET AUTOCOMMIT=0", 16))
    {
      LOG_ERROR_MESSAGE("mysql reports: %d %s", mysql_errno(dbc->library.mysql.mysql) ,
                         mysql_error(dbc->library.mysql.mysql));
      return ERROR;
    }

    return OK;
}

/* Disconnect from the database and free the connection handle. */
int disconnect_from_db_mysql(struct db_context_t *dbc)
{
        mysql_close(dbc->library.mysql.mysql);
	return OK;
}

int db_init_mysql(struct db_context_t *dbc, char *dbname, char *host,
		char *user, char *pass, char *port, char *socket)
{
	dbc->commit_transaction = commit_transaction_mysql;
	dbc->disconnect = disconnect_from_db_mysql;
	dbc->rollback_transaction = rollback_transaction_mysql;
	dbc->execute_delivery = execute_delivery_mysql;
	dbc->execute_integrity = execute_integrity_mysql;
	dbc->execute_new_order = execute_new_order_mysql;
	dbc->execute_order_status = execute_order_status_mysql;
	dbc->execute_payment = execute_payment_mysql;
	dbc->execute_stock_level = execute_stock_level_mysql;
	dbc->sql_execute = sql_execute_mysql;
	dbc->sql_fetchrow = sql_fetchrow_mysql;
	dbc->sql_close_cursor = sql_close_cursor_mysql;
	dbc->sql_getvalue = sql_getvalue_mysql;

	/* Copy values only if it's not NULL. */
	if (dbname != NULL) {
		strncpy(dbc->library.mysql.dbname, dbname, MYSQL_DBNAME_LEN);
	} else {
		strncpy(dbc->library.mysql.dbname, "dbt2", MYSQL_DBNAME_LEN);
	}
	if (host != NULL) {
		strncpy(dbc->library.mysql.host, host, MYSQL_HOST_LEN);
	} else {
		strncpy(dbc->library.mysql.host, "localhost", MYSQL_HOST_LEN);
        }
	if (user != NULL) {
		strncpy(dbc->library.mysql.user, user, MYSQL_USER_LEN);
	} else {
		strncpy(dbc->library.mysql.user, "root", MYSQL_USER_LEN);
        }
	if (pass != NULL) {
		strncpy(dbc->library.mysql.pass, pass, MYSQL_PASS_LEN);
	} else {
		strncpy(dbc->library.mysql.pass, "", MYSQL_PASS_LEN);
	}
	if (port != NULL) {
		strncpy(dbc->library.mysql.port_t, port, MYSQL_PORT_T_LEN);
	} else {
		strncpy(dbc->library.mysql.port_t, "0", MYSQL_PORT_T_LEN);
	}
	if (socket != NULL) {
		strncpy(dbc->library.mysql.socket_t, socket, MYSQL_SOCKET_T_LEN);
	} else {
		strncpy(dbc->library.mysql.socket_t, "/tmp/mysql.sock",
				MYSQL_SOCKET_T_LEN);
	}
	return OK;
}

int rollback_transaction_mysql(struct db_context_t *dbc)
{
      LOG_ERROR_MESSAGE("ROLLBACK INITIATED\n");

      if (mysql_real_query(dbc->library.mysql.mysql, "ROLLBACK", 8))
      {
        LOG_ERROR_MESSAGE("ROLLBACK failed. mysql reports: %d %s", 
                           mysql_errno(dbc->library.mysql.mysql), mysql_error(dbc->library.mysql.mysql));
        return ERROR;
      }
      return STATUS_ROLLBACK;
}

int sql_execute_mysql(struct db_context_t *dbc, char * query, struct sql_result_t * sql_result,
                       char * query_name)
{

  sql_result->library.mysql.result_set= NULL;
  sql_result->library.mysql.num_fields= 0;
  sql_result->library.mysql.num_rows= 0;
  sql_result->library.mysql.query=query;

  if (mysql_query(dbc->library.mysql.mysql, query))
  {
    LOG_ERROR_MESSAGE("%s: %s\nmysql reports: %d %s",query_name, query,
                            mysql_errno(dbc->library.mysql.mysql), mysql_error(dbc->library.mysql.mysql));
    return 0;
  }
  else 
  {
    sql_result->library.mysql.result_set = mysql_store_result(dbc->library.mysql.mysql);

    if (sql_result->library.mysql.result_set)
    {
      sql_result->library.mysql.num_fields= mysql_num_fields(sql_result->library.mysql.result_set);
      sql_result->library.mysql.num_rows= mysql_num_rows(sql_result->library.mysql.result_set);
    }
    else  
    {
      if (mysql_field_count(dbc->library.mysql.mysql) == 0)
      {
        sql_result->library.mysql.num_rows = mysql_affected_rows(dbc->library.mysql.mysql);
      }
      else 
      {
         LOG_ERROR_MESSAGE("%s: %s\nmysql reports: %d %s",query_name, query,
                            mysql_errno(dbc->library.mysql.mysql), mysql_error(dbc->library.mysql.mysql));
         return 0;
      }
    }
  }

  return 1;
}

int sql_fetchrow_mysql(struct db_context_t *dbc, struct sql_result_t * sql_result)
{
  sql_result->library.mysql.current_row= mysql_fetch_row(sql_result->library.mysql.result_set);
  if (sql_result->library.mysql.current_row)
  {
    sql_result->library.mysql.lengths= mysql_fetch_lengths(sql_result->library.mysql.result_set);
    return 1;
  }
  return 0;
}

int sql_close_cursor_mysql(struct db_context_t *dbc, struct sql_result_t * sql_result)
{

  if (sql_result->library.mysql.result_set)
  {
    mysql_free_result(sql_result->library.mysql.result_set);
  }

  return 1;
}


char *sql_getvalue_mysql(struct db_context_t *dbc, struct sql_result_t * sql_result, int field)
{
  char * tmp;
  
  tmp= NULL;

  if (sql_result->library.mysql.current_row && field < sql_result->library.mysql.num_fields)
  {
    if (sql_result->library.mysql.current_row[field])
    {
      if ((tmp = calloc(sizeof(char), sql_result->library.mysql.lengths[field]+1)))
      {
        memcpy(tmp, (sql_result->library.mysql.current_row)[field], sql_result->library.mysql.lengths[field]);
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
    LOG_ERROR_MESSAGE("dbt2_sql_getvalue: POSSIBLE NULL VALUE or ERROR\n\Query: %s\nField: %d from %d", 
                       sql_result->library.mysql.query, field, sql_result->library.mysql.num_fields);
#endif
  }
  return tmp;
}

