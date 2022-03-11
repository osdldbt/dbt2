/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002      Open Source Development Lab, Inc.
 *               2002-2022 Mark Wong
 *
 * 25 june 2002
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

#include <pthread.h>

#include "common.h"
#include "logging.h"
#include "db_threadpool.h"
#include "listener.h"
#include "_socket.h"
#include "transaction_queue.h"
#include "db.h"

/* Function Prototypes */
int parse_arguments(int argc, char *argv[]);
int parse_command(char *command);

/* Global Variables */
char sname[64] = "";
char dname[32] = "";
int port = CLIENT_PORT;
int sockfd;
int exiting = 0;
int force_sleep = 0;

#if defined(HAVE_MYSQL) || defined(HAVE_ODBC)
char dbt2_user[128] = DB_USER;
char dbt2_pass[128] = DB_PASS;
#endif

#ifdef HAVE_LIBPQ
char postmaster_port[32] = "5432";
#endif /* HAVE_LIBPQ */

#ifdef HAVE_MYSQL
char dbt2_mysql_host[128];
char dbt2_mysql_port[32];
char dbt2_mysql_socket[256];
#endif /* HAVE_MYSQL */

int startup();
void usage(char *);

int main(int argc, char *argv[])
{
	unsigned int count;
	char command[128];

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
	if (db_connections == 0) {
		printf("-c not used\n");
		return 3;
	}

	/* Ok, let's get started! */
	init_logging();

	printf("opening %d connection(s) to %s...\n", db_connections, sname);
	if (startup() != OK) {
		LOG_ERROR_MESSAGE("startup() failed\n");
		printf("startup() failed\n");
		return 4;
	}
	printf("client has started\n");

	printf("%d DB worker threads have started\n", db_connections);
	fflush(stdout);
	create_pid_file();

	/* Wait for command line input. */
	do {
		if (force_sleep == 1) {
			sleep(600);
			continue;
		}
		scanf("%s", command);
		if (parse_command(command) == EXIT_CODE) {
			break;
		}
	} while(1);

	printf("closing socket...\n");
	close(sockfd);
	printf("waiting for threads to exit... [NOT!]\n");

	/*
	 * There are threads waiting on a semaphore that won't exit and I
	 * haven't looked into how to get around that so I'm forcing an exit.
	 */
exit(0);
	do {
		/* Loop until all the DB worker threads have exited. */
		sem_getvalue(&db_worker_count, &count);
		sleep(1);
	} while (count > 0);

	/* Let everyone know we exited ok. */
	printf("exiting...\n");

	return 0;
}

int parse_arguments(int argc, char *argv[])
{
	int c;

	if (argc < 3) {
		return ERROR;
	}

	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{ 0, 0, 0, 0 }
		};

		c = getopt_long(argc, argv, "a:b:c:d:fl:o:p:s:t:h:u:z:",
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
			strcpy(output_path, optarg);
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
		case 'z':
			strncpy(dbt2_pass, optarg, 127);
			break;
#endif
		default:
			printf("?? getopt returned character code 0%o ??\n", c);
		}
	}

	return OK;
}

int parse_command(char *command)
{
	int i, j;
	unsigned int count;
	int stats[2][TRANSACTION_MAX];

	if (strcmp(command, "status") == 0) {
		time_t current_time;
		printf("------\n");
		sem_getvalue(&queue_length, &count);
		printf("transactions waiting = %d\n", count);
		sem_getvalue(&db_worker_count, &count);
		printf("db connections = %d\n", count);
		sem_getvalue(&listener_worker_count, &count);
		printf("terminal connections = %d\n", count);
		for (i = 0; i < 2; i++) {
			for (j = 0; j < TRANSACTION_MAX; j++) {
				pthread_mutex_lock(
					&mutex_transaction_counter[i][j]);
				stats[i][j] = transaction_counter[i][j];
				pthread_mutex_unlock(
					&mutex_transaction_counter[i][j]);
			}
		}
		printf("transaction   queued  executing\n");
		printf("------------  ------  ---------\n");
		for (i = 0; i < TRANSACTION_MAX; i++) {
			printf("%12s  %6d  %9d\n", transaction_name[i],
				stats[REQ_QUEUED][i], stats[REQ_EXECUTING][i]);
		}
		printf("------------  ------  ---------\n");
		printf("------  ------------  --------\n");
		printf("Thread  Transactions  Last (s)\n");
		printf("------  ------------  --------\n");
		time(&current_time);
		for (i = 0; i < db_connections; i++) {
			printf("%6d  %12d  %8d\n", i, worker_count[i],
				(int) (current_time - last_txn[i]));
		}
		printf("------  ------------  --------\n");
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

int startup()
{
	pthread_t tid;
	int ret;

	fflush(stdout);
	sockfd = _listen(port);
	if (sockfd < 1) {
		printf("_listen() failed on port %d\n", port);
		return ERROR;
	}
	if (init_transaction_queue() != OK) {
		LOG_ERROR_MESSAGE("init_transaction_queue() failed");
		return ERROR;
	}
	ret = pthread_create(&tid, NULL, &init_listener, &sockfd);
	if (ret != 0) {
		LOG_ERROR_MESSAGE(
			"pthread_create() error with init_listener()");
		if (ret == EAGAIN) {
			LOG_ERROR_MESSAGE("not enough system resources");
		}
		return ERROR;
	}
	printf("listening to port %d\n", port);

	if (db_threadpool_init() != OK) {
		LOG_ERROR_MESSAGE("db_thread_pool_init() failed");
		return ERROR;
	}

	return OK;
}

int create_pid_file()
{
  FILE * fpid;
  char pid_filename[1024];

  sprintf(pid_filename, "%s%s", output_path, CLIENT_PID_FILENAME);

  fpid = fopen(pid_filename,"w");
  if (!fpid)
  {
    printf("cann't create pid file: %s\n", pid_filename);
    return ERROR;
  }

  fprintf(fpid,"%d", (unsigned int) getpid());
  fclose(fpid);

  return OK;
}

void usage(char *name)
{
	printf("%s is the DBT-2 Client\n\n", name);
	printf("Usage:\n");
	printf("  %s [OPTION]\n\n", name);
	printf("General options:\n");
	printf("  -a <dbms>      cockroach|mysql|pgsql|yugabyte\n");
	printf("  -c #           number of database connections\n");
	printf("  -f             set forced sleep\n");
	printf("  -p #           port to listen for incoming driver connections, "
			"default %d\n", CLIENT_PORT);
	printf("  -s #           seconds to sleep between openning db connections, "
			"default 1 s\n");
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
