/*
 * db_threadpool.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
 *
 * 31 june 2002
 */

#include <stdio.h>
#include <common.h>
#include <db_threadpool.h>
#ifdef ODBC
#include <odbc_common.h>
#endif /* ODBC */

/* Function Prototypes */
void *db_worker(void *no_data);
int startup();

/* Global Variables */
extern int db_connections;

/* These should probably be handled differently. */
extern char sname[32];
extern int exiting;
sem_t db_worker_count;

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

int db_threadpool_init()
{
	int i;

	if (sem_init(&db_worker_count, 0, 0) != 0)
	{
		LOG_ERROR_MESSAGE("cannot init db_worker_count\n");
		return ERROR;
	}

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
