/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002      Open Source Development Lab, Inc.
 *               2002-2022 Mark Wong
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include "common.h"
#include "logging.h"
#include "client.h"
#include "_socket.h"
#include "db.h"

int parse_arguments(int, char **);
int parse_command(char *);
void usage(char *);

int main(int argc, char *argv[])
{
	if (parse_arguments(argc, argv) != OK) {
		usage(argv[0]);
		return 1;
	}

	init_common();

	/* Check to see if the required flags were used. */
	if (dbms == -1) {
		printf("-a not used to specify dbms\n");
		return 2;
	}
	if (strlen(sname) == 0) {
		printf("-d not used\n");
		return 2;
	}
#ifdef CLIENT1
	if (db_connections == 0) {
		printf("-c not used\n");
		return 3;
	}
#endif /* CLIENT1 */

	/* Ok, let's get started! */
	create_pid_file();

	sockfd = _listen(port);
	if (sockfd < 1) {
		printf("_listen() failed on port %d\n", port);
		return ERROR;
	}
	printf("listening to port %d\n", port);

	if (startup() != OK) {
		LOG_ERROR_MESSAGE("startup() failed\n");
		printf("startup() failed\n");
		return 4;
	}

	return 0;
}

int parse_arguments(int argc, char *argv[])
{
	int c;
	int length;

	if (argc < 3) {
		return ERROR;
	}

	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{ 0, 0, 0, 0 }
		};

		c = getopt_long(argc, argv, "a:b:c:C:d:fl:o:p:s:t:h:u:x:z:",
			long_options, &option_index);
		if (c == -1) {
			break;
		}

		switch (c) {
		case 0:
			break;
		case 'a':
			if (strcmp(optarg, "cockroach") == 0)
				dbms = DBMSCOCKROACH;
			else if (strcmp(optarg, "odbc") == 0)
				dbms = DBMSODBC;
			else if (strcmp(optarg, "pgsql") == 0)
				dbms = DBMSLIBPQ;
			else if (strcmp(optarg, "sqlite") == 0)
				dbms = DBMSSQLITE;
			else {
				printf("unrecognized dbms option: %s\n", optarg);
				exit(1);
			}
			break;
		case 'b':
			strcpy(dname, optarg);
			break;
		case 'c':
			db_connections = atoi(optarg);
			break;
		case 'C':
			max_driver_connections = atoi(optarg);
			break;
		case 'd':
			strncpy(sname, optarg, sizeof(sname));
			break;
		case 'f':
			force_sleep=1;
			break;
		case 'l':
#if defined(HAVE_LIBPQ)
			strcpy(postmaster_port, optarg);
#endif
#if defined(HAVE_MYSQL)
			strcpy(dbt2_mysql_port, optarg);
#endif
			break;
		case 'h':
#if defined(HAVE_MYSQL)
			strcpy(dbt2_mysql_host, optarg);
#endif
			break;
		case 'o':
			length = strlen(optarg);
			output_path = malloc(sizeof(char) * (length + 1));
			if (output_path == NULL) {
				printf("error allocating output_path\n");
				exit(1);
			}
			strcpy(output_path, optarg);
			if (output_path[length] == '/')
				output_path[length] = '\0';
			break;
		case 'p':
			port = atoi(optarg);
			break;
		case 's':
			db_conn_sleep = atoi(optarg);
			break;
		case 't':
#if defined(HAVE_MYSQL)
			strcpy(dbt2_mysql_socket, optarg);
#endif
			break;
#if defined(HAVE_MYSQL) || defined(HAVE_ODBC)
		case 'u':
			strncpy(dbt2_user, optarg, 127);
			break;
#endif
		case 'x':
			connections_per_process = atoi(optarg);
			break;
#if defined(HAVE_MYSQL) || defined(HAVE_ODBC)
		case 'z':
			strncpy(dbt2_pass, optarg, 127);
			break;
#endif
		default:
			printf("?? getopt returned character code 0%o ??\n", c);
		}
	}

	if (output_path == NULL) {
		output_path = malloc(sizeof(char) * 2);
		if (output_path == NULL) {
			printf("error allocating output_path\n");
			exit(1);
		}
		output_path[0] = '.';
		output_path[1] = '\0';
	}

	return OK;
}

int parse_command(char *command)
{
	if (strcmp(command, "status") == 0) {
		status();
	} else if (strcmp(command, "exit") == 0 ||
		strcmp(command, "quit") == 0) {
		exiting = 1;
		return EXIT_CODE;
	} else if (strcmp(command, "help") == 0 || strcmp(command, "?") == 0) {
		printf("help or ?\n");
		printf("status\n");
		printf("exit or quit\n");
	} else {
		printf("unknown command: %s\n", command);
	}
	return OK;
}

void usage(char *name)
{
	printf("%s is the DBT-2 Client\n\n", name);
	printf("Usage:\n");
	printf("  %s [OPTION]\n\n", name);
	printf("General options:\n");
	printf("  -a <dbms>      cockroach|mysql|pgsql|yugabyte\n");
#ifdef CLIENT1
	printf("  -f             set forced sleep\n");
	printf("  -c #           number of database connections\n");
#endif /* CLIENT1 */
#ifdef CLIENT2
	printf("  -C #           roughly maximum number of driver connections, "
			"default %d\n", max_driver_connections);
#endif /* CLIENT2 */
	printf("  -o <path>      output directory, default '.'\n");
	printf("  -p #           port to listen for incoming driver connections, "
			"default %d\n", CLIENT_PORT);
	printf("  -s #           seconds to sleep between openning db connections, "
			"default 1 s\n");
#ifdef CLIENT2
	printf("  -x #           number of database connections per process\n");
#endif /* CLIENT2 */
#ifdef HAVE_ODBC
	printf("\nunixODBC options:\n");
	printf("  -d <db_name>   database connect string\n");
	printf("  -u <db user>\n");
	printf("  -z <db password>\n");
#endif /* HAVE_ODBC */
#ifdef HAVE_LIBPQ
	printf("\nlibpq (CockroachDB, PostgreSQL, YugabyteDB) options:\n");
	printf("  -b <dbname>    database name\n");
	printf("  -d <hostname>  database hostname\n");
	printf("  -l #           postmaster port\n");
#endif /* HAVE_LIBPQ */
#ifdef HAVE_MYSQL
	printf("\nMySQL options:\n");
	printf("  -d <db_name>   database name\n");
	printf("  -l #           MySQL port number to\n");
	printf("  -h <hostname>  MySQL hostname\n");
	printf("  -t <socket>    MySQL socket\n");
	printf("  -u <db user>\n");
	printf("  -z <db password>\n");
#endif /* HAVE_MYSQL */
#ifdef HAVE_SQLITE3
	printf("\nSQLite options:\n");
	printf("  -d <db_file>   path to database file\n");
#endif
}
