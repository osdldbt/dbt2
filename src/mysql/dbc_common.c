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
#include "mysql_common.h"
#include <stdio.h>

char mysql_dbname[32] = "dbt2";
char mysql_host[32] = "localhost";
char mysql_user[32] = "root";
char mysql_pass[32] = "";
char mysql_port_t[32] = "0";
char mysql_socket_t[256] = "/tmp/mysql.sock";


int commit_transaction(struct db_context_t *dbc)
{

      if (mysql_real_query(dbc->mysql, "COMMIT", 6)) 
      {
        LOG_ERROR_MESSAGE("COMMIT failed. mysql reports: %d %s", 
                           mysql_errno(dbc->mysql), mysql_error(dbc->mysql));
        return ERROR;
      }
      return OK;
}

/* Open a connection to the database. */
int _connect_to_db(struct db_context_t *dbc)
{
    dbc->mysql=mysql_init(NULL);

    //FIXME: change atoi() to strtol() and check for errors
    if (!mysql_real_connect(dbc->mysql, mysql_host, mysql_user, mysql_pass, mysql_dbname, atoi(mysql_port_t), mysql_socket_t, 0))
    {
      if (mysql_errno(dbc->mysql))
      {
        LOG_ERROR_MESSAGE("Connection to database '%s' failed.", mysql_dbname);
	LOG_ERROR_MESSAGE("mysql reports: %d %s", 
                           mysql_errno(dbc->mysql), mysql_error(dbc->mysql));
      }
      return ERROR;
    }

    /* Disable AUTOCOMMIT mode for connection */
    if (mysql_real_query(dbc->mysql, "SET AUTOCOMMIT=0", 16))
    {
      LOG_ERROR_MESSAGE("mysql reports: %d %s", mysql_errno(dbc->mysql) ,
                         mysql_error(dbc->mysql));
      return ERROR;
    }

    return OK;
}

/* Disconnect from the database and free the connection handle. */
int _disconnect_from_db(struct db_context_t *dbc)
{
        mysql_close(dbc->mysql);
	return OK;
}

int _db_init(char * _mysql_dbname, char *_mysql_host, char * _mysql_user, 
             char * _mysql_pass, char * _mysql_port, char * _mysql_socket)
{
	/* Copy values only if it's not NULL. */
	if (_mysql_dbname != NULL) {
		strcpy(mysql_dbname, _mysql_dbname);
	}
	if (_mysql_host != NULL) {
		strcpy(mysql_host, _mysql_host);
        }
	if (_mysql_user != NULL) {
		strcpy(mysql_user, _mysql_user);
        }
	if (_mysql_pass != NULL) {
		strcpy(mysql_pass, _mysql_pass);
	}
	if (_mysql_port != NULL) {
		strcpy(mysql_port_t, _mysql_port);
	}
	if (_mysql_socket != NULL) {
		strcpy(mysql_socket_t, _mysql_socket);
	}
	return OK;
}

int rollback_transaction(struct db_context_t *dbc)
{
      LOG_ERROR_MESSAGE("ROLLBACK INITIATED\n");

      if (mysql_real_query(dbc->mysql, "ROLLBACK", 8)) 
      {
        LOG_ERROR_MESSAGE("ROLLBACK failed. mysql reports: %d %s", 
                           mysql_errno(dbc->mysql), mysql_error(dbc->mysql));
        return ERROR;
      }
      return STATUS_ROLLBACK;
}

int dbt2_sql_execute(struct db_context_t *dbc, char * query, struct sql_result_t * sql_result, 
                       char * query_name)
{

  sql_result->result_set= NULL;
  sql_result->num_fields= 0;
  sql_result->num_rows= 0;
  sql_result->query=query;

  if (mysql_query(dbc->mysql, query))
  {
    LOG_ERROR_MESSAGE("%s: %s\nmysql reports: %d %s",query_name, query,
                            mysql_errno(dbc->mysql), mysql_error(dbc->mysql));
    return 0;
  }
  else 
  {
    sql_result->result_set = mysql_store_result(dbc->mysql);

    if (sql_result->result_set)  
    {
      sql_result->num_fields= mysql_num_fields(sql_result->result_set);
      sql_result->num_rows= mysql_num_rows(sql_result->result_set);
    }
    else  
    {
      if (mysql_field_count(dbc->mysql) == 0)
      {
        sql_result->num_rows = mysql_affected_rows(dbc->mysql);
      }
      else 
      {
         LOG_ERROR_MESSAGE("%s: %s\nmysql reports: %d %s",query_name, query,
                            mysql_errno(dbc->mysql), mysql_error(dbc->mysql));
         return 0;
      }
    }
  }

  return 1;
}

int dbt2_sql_fetchrow(struct db_context_t *dbc, struct sql_result_t * sql_result)
{
  sql_result->current_row= mysql_fetch_row(sql_result->result_set);
  if (sql_result->current_row)
  {
    sql_result->lengths= mysql_fetch_lengths(sql_result->result_set);
    return 1;
  }
  return 0;
}

int dbt2_sql_close_cursor(struct db_context_t *dbc, struct sql_result_t * sql_result)
{

  if (sql_result->result_set)
  {
    mysql_free_result(sql_result->result_set);
  }

  return 1;
}


char * dbt2_sql_getvalue(struct db_context_t *dbc, struct sql_result_t * sql_result, int field)
{
  char * tmp;
  
  tmp= NULL;

  if (sql_result->current_row && field < sql_result->num_fields)
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
    LOG_ERROR_MESSAGE("dbt2_sql_getvalue: POSSIBLE NULL VALUE or ERROR\n\Query: %s\nField: %d from %d", 
                       sql_result->query, field, sql_result->num_fields);
#endif
  }
  return tmp;
}

