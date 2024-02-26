/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 */

#ifndef _DB_H_
#define _DB_H_

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif /* _POSIX_C_SOURCE */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "transaction_data.h"

#ifdef HAVE_LIBPQ
#include <libpq-fe.h>

#define LIBPQ_DBNAME_LEN 32
#define LIBPQ_PGHOST_LEN HOSTNAMELEN
#define LIBPQ_PGPORT_LEN 32

struct db_context_libpq {
	PGconn *conn;
	char dbname[LIBPQ_DBNAME_LEN + 1];
	char pghost[LIBPQ_PGHOST_LEN + 1];
	char pgport[LIBPQ_PGPORT_LEN + 1];
};
#endif /* HAVE_LIBPQ */

#ifdef HAVE_MYSQL
#include <mysql.h>

#define MYSQL_DBNAME_LEN 31
#define MYSQL_HOST_LEN HOSTNAMELEN
#define MYSQL_PASS_LEN 31
#define MYSQL_PORT_T_LEN 31
#define MYSQL_SOCKET_T_LEN 255
#define MYSQL_USER_LEN 31

struct db_context_mysql {
	MYSQL *mysql;
	char dbname[MYSQL_DBNAME_LEN + 1];
	char host[MYSQL_HOST_LEN + 1];
	char port_t[MYSQL_PORT_T_LEN + 1];
	char user[MYSQL_USER_LEN + 1];
	char pass[MYSQL_PASS_LEN + 1];
	char socket_t[MYSQL_SOCKET_T_LEN + 1];
};

struct sql_result_mysql
{
	MYSQL_RES * result_set;
	MYSQL_ROW current_row;
	unsigned int num_fields;
	unsigned int num_rows;
	unsigned long * lengths;
	char * query;
};
#endif /* HAVE_MYSQL */

#ifdef HAVE_ODBC
#ifdef IODBC
#include <isql.h>
#include <isqlext.h>
#include <isqltypes.h>
#else /* IODBC */
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#endif /* IODBC */

#ifndef SQLLEN
#define SQLLEN SQLINTEGER
#endif

#define LOG_ODBC_ERROR(type, handle) \
		log_odbc_error(__FILE__, __LINE__, type, handle)

struct db_context_odbc {
	SQLHDBC hdbc;
	SQLHSTMT hstmt;
};

struct sql_result_odbc
{
	unsigned int current_row;
	unsigned int result_set;
	SQLSMALLINT num_fields;
	SQLLEN num_rows;
	SQLUINTEGER *lengths;
	char *query;
};
#endif /* HAVE_ODBC */

#ifdef HAVE_SQLITE3
#include <sqlite3.h>

#define SQLITE3_DBNAME_LEN 255

struct db_context_sqlite {
	char dbname[SQLITE3_DBNAME_LEN + 1];
	unsigned char inTransaction;
	sqlite3 *db;
};

struct sql_result_sqlite
{
	unsigned char query_running;
	unsigned char error;
	unsigned char prefetched;
	unsigned int num_fields;
	sqlite3_stmt * pStmt;
};
#endif /* HAVE_SQLITE3 */

#define DBMSCOCKROACH 1
#define DBMSLIBPQ 2
#define DBMSMYSQL 3
#define DBMSODBC 4
#define DBMSSQLITE 5

struct sql_result_t
{
	int type;
	union {
#ifdef HAVE_MYSQL
		struct sql_result_mysql mysql;
#endif /* HAVE_MYSQL */
#ifdef HAVE_ODBC
		struct sql_result_odbc odbc;
#endif /* HAVE_ODBC */
#ifdef HAVE_SQLITE3
		struct sql_result_sqlite sqlite;
#endif /* HAVE_SQLITE3 */
	} library;
};

struct db_context_t {
	int type;
	int stop_time;
	struct timespec ts_retry;
	int (*connect)(struct db_context_t *);
	int (*commit_transaction)(struct db_context_t *);
	int (*disconnect)(struct db_context_t *);
	int (*rollback_transaction)(struct db_context_t *);
	int (*execute_delivery)(struct db_context_t *, struct delivery_t *);
	int (*execute_integrity)(struct db_context_t *, struct integrity_t *);
	int (*execute_new_order)(struct db_context_t *, struct new_order_t *);
	int (*execute_order_status)(struct db_context_t *, struct order_status_t *);
	int (*execute_payment)(struct db_context_t *, struct payment_t *);
	int (*execute_stock_level)(struct db_context_t *, struct stock_level_t *);
	int (*sql_execute)(struct db_context_t *, char *, struct sql_result_t *,
			char *);
	int (*sql_fetchrow)(struct db_context_t *, struct sql_result_t *);
	int (*sql_close_cursor)(struct db_context_t *, struct sql_result_t *);
	char *(*sql_getvalue)(struct db_context_t *, struct sql_result_t *, int);
	union {
#ifdef HAVE_LIBPQ
		struct db_context_libpq libpq;
#endif /* HAVE_LIBPQ */
#ifdef HAVE_MYSQL
		struct db_context_mysql mysql;
#endif /* HAVE_MYSQL */
#ifdef HAVE_ODBC
		struct db_context_odbc odbc;
#endif /* HAVE_ODBC */
#ifdef HAVE_SQLITE3
		struct db_context_sqlite sqlite;
#endif /* HAVE_SQLITE3 */
	} library;
};

extern const char s_dist[10][11];

int connect_to_db(struct db_context_t *);
#ifdef HAVE_ODBC
int db_init(char *sname, char *uname, char *auth);
#endif /* HAVE_ODBC */
#ifdef HAVE_LIBPQ
int db_init(char *_dbname, char *_pghost, char *_pgport);
#endif /* HAVE_LIBPQ */

#ifdef HAVE_MYSQL
int commit_transaction_mysql(struct db_context_t *);
int connect_to_db_mysql(struct db_context_t *);
int db_init_mysql(struct db_context_t *, char *, char *, char *, char *, char *,
		char *);
int disconnect_from_db_mysql(struct db_context_t *);
int rollback_transaction_mysql(struct db_context_t *);

int sql_execute_mysql(struct db_context_t *, char *, struct sql_result_t *,
		char *);
int sql_close_cursor_mysql(struct db_context_t *, struct sql_result_t *);
int sql_fetchrow_mysql(struct db_context_t *, struct sql_result_t *);
char *sql_getvalue_mysql(struct db_context_t *, struct sql_result_t *, int);
#endif /* HAVE_MYSQL */

int disconnect_from_db(struct db_context_t *);
int process_transaction(int, struct db_context_t *, union transaction_data_t *);

#ifdef HAVE_LIBPQ
int commit_transaction_cockroach(struct db_context_t *);
int connect_to_db_cockroach(struct db_context_t *);
int db_init_cockroach(struct db_context_t *, char *, char *, char *);
int disconnect_from_db_cockroach(struct db_context_t *);
int rollback_transaction_cockroach(struct db_context_t *);

int commit_transaction_libpq(struct db_context_t *);
int connect_to_db_libpq(struct db_context_t *);
int db_init_libpq(struct db_context_t *, char *, char *, char *);
int disconnect_from_db_libpq(struct db_context_t *);
int rollback_transaction_libpq(struct db_context_t *);
#endif /* HAVE_LIBPQ */

#ifdef HAVE_ODBC
int check_odbc_rc(SQLSMALLINT, SQLHANDLE, SQLRETURN);
int log_odbc_error(char *, int, SQLSMALLINT, SQLHANDLE);
int commit_transaction(struct db_context_t *);
int connect_to_db_odbc(struct db_context_t *);
int disconnect_from_db_odbc(struct db_context_t *);
int db_init_odbc(char *, char *, char *);
int rollback_transaction_odbc(struct db_context_t *);

int sql_execute_odbc(struct db_context_t *, char *, struct sql_result_t *,
		char *);
int sql_close_cursor_odbc(struct db_context_t *, struct sql_result_t *);
int sql_fetchrow_odbc(struct db_context_t *, struct sql_result_t *);
char *sql_getvalue_odbc(struct db_context_t *, struct sql_result_t *, int);
#endif /* HAVE_ODBC */

#ifdef HAVE_SQLITE3
int commit_transaction_sqlite(struct db_context_t *);
int connect_to_db_sqlite(struct db_context_t *);
int db_init_sqlite(struct db_context_t *, char *);
int disconnect_from_db_sqlite(struct db_context_t *);
int rollback_transaction_sqlite(struct db_context_t *);

int sql_execute_sqlite(struct db_context_t *, char *, struct sql_result_t *,
		char *);
int sql_close_cursor_sqlite(struct db_context_t *, struct sql_result_t *);
int sql_fetchrow_sqlite(struct db_context_t *, struct sql_result_t *);
char *sql_getvalue_sqlite(struct db_context_t *, struct sql_result_t *, int);
#endif /* HAVE_SQLITE3*/

#endif /* _DB_H_ */
