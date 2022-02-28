/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002      Open Source Development Labs, Inc.
 *               2004      Alexey Stroganov & MySQL AB.
 *               2008      Steve VanDeBogart & UC Regents.
 *               2002-2022 Mark Wong
 *
 */
       
#include "common.h"
#include "logging.h"
#include "db.h"
#include <stdio.h>
#define _GNU_SOURCE
#include <string.h>

#include "nonsp_delivery.h"
#include "nonsp_integrity.h"
#include "nonsp_new_order.h"
#include "nonsp_order_status.h"
#include "nonsp_payment.h"
#include "nonsp_stock_level.h"

int commit_transaction_sqlite(struct db_context_t *dbc)
{
  if (dbc->library.sqlite.inTransaction)
  {
    if (sqlite3_exec(dbc->library.sqlite.db, "COMMIT TRANSACTION;", NULL, NULL, NULL) != SQLITE_OK)
    {
      LOG_ERROR_MESSAGE("COMMIT TRANSACTION;\nsqlite reports: %d %s", sqlite3_errcode(dbc->library.sqlite.db), sqlite3_errmsg(dbc->library.sqlite.db));
      return ERROR;
    }
	dbc->library.sqlite.inTransaction = 0;
#ifdef DEBUG_QUERY
    LOG_ERROR_MESSAGE("COMMIT TRANSACTION;\n");
#endif
  }

  return OK;
}

/* Open a connection to the database. */
int connect_to_db_sqlite(struct db_context_t *dbc)
{
	int rc;

	rc = sqlite3_open(dbc->library.sqlite.dbname, &dbc->library.sqlite.db);
	dbc->library.sqlite.inTransaction = 0;
	if (rc) {
		LOG_ERROR_MESSAGE("Connection to database '%s' failed (error code %d).",
				dbc->library.sqlite.dbname, rc);
		sqlite3_close(dbc->library.sqlite.db);
		return ERROR;
	}

    return OK;
}

/* Disconnect from the database and free the connection handle. */
int disconnect_from_db_sqlite(struct db_context_t *dbc)
{
	sqlite3_close(dbc->library.sqlite.db);
	return OK;
}

int db_init_sqlite(struct db_context_t *dbc, char *dbname)
{
	dbc->connect = connect_to_db_sqlite;
	dbc->commit_transaction = commit_transaction_sqlite;
	dbc->disconnect = disconnect_from_db_sqlite;
	dbc->rollback_transaction = rollback_transaction_sqlite;
	dbc->execute_delivery = execute_delivery_nonsp;
	dbc->execute_integrity = execute_integrity_nonsp;
	dbc->execute_new_order = execute_new_order_nonsp;
	dbc->execute_order_status = execute_order_status_nonsp;
	dbc->execute_payment = execute_payment_nonsp;
	dbc->execute_stock_level = execute_stock_level_nonsp;
	dbc->sql_execute = sql_execute_sqlite;
	dbc->sql_fetchrow = sql_fetchrow_sqlite;
	dbc->sql_close_cursor = sql_close_cursor_sqlite;
	dbc->sql_getvalue = sql_getvalue_sqlite;

	if (dbname != NULL) {
		strncpy(dbc->library.sqlite.dbname, dbname, SQLITE3_DBNAME_LEN);
	} else {
		strncpy(dbc->library.sqlite.dbname, "./dbt2.sqlite",
				SQLITE3_DBNAME_LEN);
	}
	return OK;
}

int rollback_transaction_sqlite(struct db_context_t *dbc)
{
  LOG_ERROR_MESSAGE("ROLLBACK INITIATED\n");

  if (dbc->library.sqlite.inTransaction)
  {
    if (sqlite3_exec(dbc->library.sqlite.db, "ROLLBACK TRANSACTION;", NULL, NULL, NULL) != SQLITE_OK)
    {
      LOG_ERROR_MESSAGE("ROLLBACK TRANSACTION;\nsqlite reports: %d %s", sqlite3_errcode(dbc->library.sqlite.db), sqlite3_errmsg(dbc->library.sqlite.db));
      return ERROR;
    }
	dbc->library.sqlite.inTransaction = 0;
  }

  return STATUS_ROLLBACK;
}

int sql_execute_sqlite(struct db_context_t *dbc, char * query, struct sql_result_t * sql_result,
                       char * query_name)
{
  sql_result->library.sqlite.error = 0;
  sql_result->library.sqlite.query_running = 0;
  sql_result->library.sqlite.prefetched = 0;

  if (!dbc->library.sqlite.inTransaction)
  {
    if (sqlite3_exec(dbc->library.sqlite.db, "BEGIN TRANSACTION;", NULL, NULL, NULL) != SQLITE_OK)
    {
      LOG_ERROR_MESSAGE("%s: BEGIN TRANSACTION;\nsqlite reports: %d %s", query_name, sqlite3_errcode(dbc->library.sqlite.db), sqlite3_errmsg(dbc->library.sqlite.db));
      return 0;
    }
	dbc->library.sqlite.inTransaction = 1;
  }

  if (sqlite3_prepare_v2(dbc->library.sqlite.db, query, strlen(query)+1, &sql_result->library.sqlite.pStmt, NULL) != SQLITE_OK)
  {
    LOG_ERROR_MESSAGE("%s: %s\nsqlite reports: %d %s",query_name, query,
							sqlite3_errcode(dbc->library.sqlite.db), sqlite3_errmsg(dbc->library.sqlite.db));
    sqlite3_finalize(sql_result->library.sqlite.pStmt);
    return 0;
  }
  sql_result->library.sqlite.num_fields = sqlite3_column_count(sql_result->library.sqlite.pStmt);
  sql_result->library.sqlite.query_running = 1;
  (*dbc->sql_fetchrow)(dbc, sql_result);
  if (sql_result->library.sqlite.error)
  {
    return 0;
  }
  sql_result->library.sqlite.prefetched = 1;
  return 1;
}

int sql_fetchrow_sqlite(struct db_context_t *dbc, struct sql_result_t * sql_result)
{
  int rc;

  if (!sql_result->library.sqlite.query_running)
  {
    return 0;
  }

  if (sql_result->library.sqlite.prefetched)
  {
    sql_result->library.sqlite.prefetched = 0;
    return 1;
  }

  rc = sqlite3_step(sql_result->library.sqlite.pStmt);

  if (rc == SQLITE_ROW)
  {
    return 1;
  }
  else if (rc == SQLITE_DONE)
  {
    rc = sqlite3_finalize(sql_result->library.sqlite.pStmt);
	if (rc != SQLITE_OK)
	{
      sql_result->library.sqlite.error = 1;
	}
    sql_result->library.sqlite.query_running = 0;
    return 0;
  }
  else
  {
    LOG_ERROR_MESSAGE("SQLITE error %d: %s\n", rc, sqlite3_errmsg(dbc->library.sqlite.db));
    sql_result->library.sqlite.error = 1;
    sqlite3_finalize(sql_result->library.sqlite.pStmt);
    sql_result->library.sqlite.query_running = 0;
	return 0;
  }
}

int sql_close_cursor_sqlite(struct db_context_t *dbc,
		struct sql_result_t * sql_result)
{
  int rc;

  if (sql_result->library.sqlite.query_running)
  {
    sql_result->library.sqlite.query_running = 0;
    rc = sqlite3_finalize(sql_result->library.sqlite.pStmt);
	if (rc != SQLITE_OK || sql_result->library.sqlite.error)
	{
      return 0;
    }
  }

  return 1;
}


char *sql_getvalue_sqlite(struct db_context_t *dbc,
		struct sql_result_t * sql_result, int field)
{
  char * tmp;
  
  tmp= NULL;
  if (sql_result->library.sqlite.query_running && field < sql_result->library.sqlite.num_fields)
  {
    tmp = strndup((char *) sqlite3_column_text(sql_result->library.sqlite.pStmt, field), sqlite3_column_bytes(sql_result->library.sqlite.pStmt, field));
    if (!tmp)
    {
#ifdef DEBUG_QUERY
      LOG_ERROR_MESSAGE("dbt2_sql_getvalue: var[%d]=NULL\n", field);
#endif
    }
  }
  else
  {
#ifdef DEBUG_QUERY
    LOG_ERROR_MESSAGE("dbt2_sql_getvalue: POSSIBLE NULL VALUE or ERROR\nQuery: %s\nField: %d from %d",
                       sqlite3_sql(sql_result->library.sqlite.pStmt), field, sql_result->library.sqlite.num_fields);
#endif
  }

  return tmp;
}

