/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 */

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "db.h"

extern int connections_per_process;
extern int db_conn_sleep;
extern int db_connections;
extern int dbms;
extern char dname[32];
extern int exiting;
extern int force_sleep;
extern int max_driver_connections;
extern int port;
extern char sname[SNAMELEN + 1];
extern int sockfd;

#if defined(HAVE_MYSQL) || defined(HAVE_ODBC)
extern char dbt2_user[128];
extern char dbt2_pass[128];
#endif

#ifdef HAVE_LIBPQ
extern char postmaster_port[32];
#endif /* HAVE_LIBPQ */

#ifdef HAVE_MYSQL
extern char dbt2_mysql_host[HOSTNAMELEN + 1];
extern char dbt2_mysql_port[32];
extern char dbt2_mysql_socket[256];
#endif /* HAVE_MYSQL */

int init_dbc(struct db_context_t *);
int startup();
void status();

#endif /* _CLIENT_H_ */
