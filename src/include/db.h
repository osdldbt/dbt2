/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002      Open Source Development Labs, Inc.
 *               2002-2022 Mark Wong
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

#ifdef HAVE_LIBPQ
#include <libpq-fe.h>

#define LIBPQ_DBNAME_LEN 32
#define LIBPQ_PGHOST_LEN 64
#define LIBPQ_PGPORT_LEN 32

struct db_context_libpq {
	PGconn *conn;
	char dbname[LIBPQ_DBNAME_LEN + 1];
	char pghost[LIBPQ_PGHOST_LEN + 1];
	char pgport[LIBPQ_PGPORT_LEN + 1];
};
#endif /* HAVE_LIBPQ */

#ifdef LIBMYSQL
#include "mysql_common.h"
#endif /* LIBMYSQL */

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
#define DBMSSQLITE 3

struct sql_result_t
{
	int type;
	union {
#ifdef HAVE_SQLITE3
		struct sql_result_sqlite sqlite;
#endif /* HAVE_SQLITE3 */
	} library;
};

struct db_context_t {
	int type;
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
#ifdef HAVE_SQLITE3
		struct db_context_sqlite sqlite;
#endif /* HAVE_SQLITE3 */
	} library;
};

extern const char s_dist[10][11];

int connect_to_db(struct db_context_t *);
#ifdef ODBC
int db_init(char *sname, char *uname, char *auth);
#endif /* ODBC */
#ifdef HAVE_LIBPQ
int db_init(char *_dbname, char *_pghost, char *_pgport);
#endif /* HAVE_LIBPQ */

#ifdef LIBMYSQL
int db_init(char * _mysql_dbname, char *_mysql_host, char * _mysql_user,
            char * _mysql_pass, char * _mysql_port, char * _mysql_socket);
#endif /* LIBMYSQL */

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
