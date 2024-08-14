/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "db.h"
#include "logging.h"

int connections_per_process = 1;
int db_conn_sleep = 1000; /* milliseconds */
int db_connections = 0;
int dbms = -1;
char dname[32] = "";
int exiting = 0;
int force_sleep = 0;
int port = CLIENT_PORT;
int max_driver_connections = 1024;
int sockfd;
char sname[SNAMELEN + 1] = "";

#if defined(HAVE_MYSQL) || defined(HAVE_ODBC)
char dbt2_user[128] = DB_USER;
char dbt2_pass[128] = DB_PASS;
#endif

#ifdef HAVE_LIBPQ
char postmaster_port[32] = "5432";
#endif /* HAVE_LIBPQ */

#ifdef HAVE_MYSQL
char dbt2_mysql_host[HOSTNAMELEN + 1];
char dbt2_mysql_port[32];
char dbt2_mysql_socket[256];
#endif /* HAVE_MYSQL */

int init_dbc(struct db_context_t *dbc) {
	memset(dbc, 0, sizeof(struct db_context_t));
	switch (dbms) {
#ifdef HAVE_LIBPQ
	case DBMSCOCKROACH:
		db_init_cockroach(dbc, dname, sname, postmaster_port);
		break;
	case DBMSLIBPQ:
		db_init_libpq(dbc, dname, sname, postmaster_port);
		break;
#endif /* HAVE_LIBPQ */

#ifdef HAVE_MYSQL
	case DBMSMYSQL:
		db_init_mysql(
				dbc, dname, dbt2_mysql_host, dbt2_user, dbt2_pass,
				dbt2_mysql_port, dbt2_mysql_socket);
		break;
#endif /* HAVE_MYSQL */

#ifdef HAVE_SQLITE3
	case DBMSSQLITE:
		db_init_sqlite(dbc, sname);
		break;
#endif /* HAVE_SQLITE3 */

#ifdef HAVE_ODBC
	case DBMSODBC:
		db_init_odbc(sname, "", "");
		break;
#endif /* HAVE_ODBC */

	default:
		printf("unrecognized dbms code: %d\n", dbms);
		LOG_ERROR_MESSAGE("unrecognized dbms code: %d", dbms);
		return 1;
	}

	return 0;
}
