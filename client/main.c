/*
 * main.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
 *
 * 25 june 2002
 */

#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <common.h>
#include <_socket.h>
#ifdef ODBC
#include <odbc_common.h>
#endif /* ODBC */

/* Function Prototypes */
void *db_worker(void *no_data);
int parse_arguments(int argc, char *argv[]);
int parse_command(char *command);
int startup();

/* Global Variables */
char sname[32] = "";
int db_connections = 0;
int port = 30000;
int sockfd;
int exiting = 0;

sem_t db_worker_count;

int main(int argc, char *argv[])
{
	int count;
	char command[128];

	if (parse_arguments(argc, argv) != OK)
	{
		printf("usage: %s -d <db_name> -c # [-p #]\n", argv[0]);
		printf("\n");
		printf("-d <db_name>\n");
		printf("\tdatabase connect string\n");
		printf("-c #\n");
		printf("\tnumber of database connections\n");
		printf("-p #\n");
		printf("\tport to listen for incoming connections\n");
		return 1;
	}

	/* Check to see if the required flags were used. */
	if (strlen(sname) == 0)
	{
		printf("-d not used\n");
		return 2;
	}
	if (db_connections == 0)
	{
		printf("-c not used\n");
		return 3;
	}

	/* Ok, let's get started! */
	printf("opening %d conenction(s) to %s...\n", db_connections, sname);
	if (startup() != OK)
	{
		return 4;
	}

	/* Wait for command line input. */
	do
	{
		scanf("%s", command);
		if (parse_command(command) == EXIT_CODE)
		{
			break;
		}
	}
	while(1);

	/* Let everyone know we exited ok. */
	close(sockfd);
	do
	{
		/* Loop until all the DB worker threads have exited. */
		sem_getvalue(&db_worker_count, &count);
		sleep(1);
	}
	while (count > 0);
	printf("exiting...\n");

	return 0;
}

void *db_worker(void *no_data)
{
#ifdef ODBC
	struct odbc_context_t odbcc;
#endif /* ODBC */

	/* Open a connection to the database. */
#ifdef ODBC
	odbc_init(sname, DB_USER, DB_PASS);
	if (!exiting && odbc_connect(&odbcc) != OK)
	{
		LOG_ERROR_MESSAGE("odbc_connect() error, terminating program");
		printf("cannot connect to database, exiting...\n");
		exit(1);
	}
#endif /* ODBC */
	while (1 && !exiting);
	{
		/* Remove the sleep when we actually are doing something. */
		sleep(1);
	}

	/* Disconnect from the database. */
#ifdef ODBC
	/* The program is halting on the odbc_disconnect(). */
/*
	odbc_disconnect(&odbcc);
*/
#endif /* ODBC */

	sem_wait(&db_worker_count);
}

int parse_arguments(int argc, char *argv[])
{
	int i;

	if (argc < 3)
	{
		return ERROR;
	}

	for (i = 1; i < argc; i += 2)
	{

		/* Check for exact length of a flag: i.e. -c */
		if (strlen(argv[i]) != 2)
		{
			printf("invalid flag: %s\n", argv[i]);
			return ERROR;
		}

		/* Handle the recognized flags. */
		if (argv[i][1] == 'd')
		{
			strcpy(sname, argv[i + 1]);
		}
		else if (argv[i][1] == 'c')
		{
			db_connections = atoi(argv[i + 1]);
		}
		else if (argv[i][1] == 'p')
		{
			port = atoi(argv[i + 1]);
		}
		else
		{
			printf("invalid flag: %s\n", argv[i]);
			return ERROR;
		}
	}

	return OK;
}

int parse_command(char *command)
{
	int count;

	if (strcmp(command, "status") == 0)
	{
		sem_getvalue(&db_worker_count, &count);
		printf("db worker = %d\n", count);
	}
	else if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0)
	{
		exiting = 1;
		return EXIT_CODE;
	}
	else
	{
		printf("unknown command: %s\n", command);
	}
	return OK;
}

int startup()
{
	int i;

	init_common();

	if (sem_init(&db_worker_count, 0, 0) != 0)
	{
		LOG_ERROR_MESSAGE("cannot init running_eu_count\n");
		return ERROR;
	}

	sockfd = _listen(port);
	if (sockfd < 1)
	{
		printf("_listen() failed on port %d\n", port);
		return ERROR;
	}
	printf("listening to port %d\n", port);

	for (i = 0; i < db_connections; i++)
	{
		pthread_t tid;

		if (pthread_create(&tid, NULL, &db_worker, NULL) != 0)
		{
			LOG_ERROR_MESSAGE("error creating thread\n");
			return ERROR;
		}

		/* Keep a count of how many DB worker threads have started. */
		sem_post(&db_worker_count);
	}

	return OK;
}
