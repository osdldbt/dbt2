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

#include <stdio.h>
#include <common.h>
#include <db_threadpool.h>
#include <listener.h>
#include <_socket.h>

/* Function Prototypes */
int parse_arguments(int argc, char *argv[]);
int parse_command(char *command);

/* Global Variables */
char sname[32] = "";
int db_connections = 0;
int port = 30000;
int sockfd;
int exiting = 0;

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
		LOG_ERROR_MESSAGE("startup() failed\n");
		printf("startup() failed\n");
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
	else if (strcmp(command, "help") == 0 || strcmp(command, "?") == 0)
	{
		printf("help or ?\n");
		printf("status\n");
		printf("exit or quit\n");
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
	pthread_t tid;

	init_common();

	sockfd = _listen(port);
	if (sockfd < 1)
	{
		printf("_listen() failed on port %d\n", port);
		return ERROR;
	}
	if (pthread_create(&tid, NULL, &init_listener, &sockfd) != 0)
	{
		LOG_ERROR_MESSAGE("pthread_create() error with init_listener()\n");
		return ERROR;
	}
	printf("listening to port %d\n", port);

	if (db_threadpool_init() != OK)
	{
		LOG_ERROR_MESSAGE("db_thread_pool_init() failed");
		return ERROR;
	}

	return OK;
}
