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
#include <logging.h>
#include <client_interface.h>
#include <transaction_queue.h>
#include <db_threadpool.h>
#include <db.h>
#ifdef ODBC
#include <odbc_common.h>
#endif /* ODBC */

/* Function Prototypes */
void *db_worker(void *no_data);
int startup();

/* Global Variables */
int db_connections = 0;

/* These should probably be handled differently. */
extern char sname[32];
extern int exiting;
sem_t db_worker_count;
#ifdef STANDALONE
extern FILE *log_mix;
extern pthread_mutex_t mutex_mix_log;
#endif /* STANDALONE */

void *db_worker(void *no_data)
{
	int length;
	struct transaction_queue_node_t *node;
#ifdef ODBC
	struct odbc_context_t odbcc;
#endif /* ODBC */
#ifdef STANDALONE
	struct timeval rt0, rt1;
	double response_time;
#endif /* STANDALONE */

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
	while (!exiting)
	{
		/*
		 * I know this loop will prevent the program from exiting because
		 * of the dequeue...
		 */
		node = dequeue_transaction();
		if (node == NULL)
		{
			LOG_ERROR_MESSAGE("dequeue was null");
			pthread_mutex_lock(&mutex_transaction_counter[EXECUTING][node->client_data.transaction]);
			--transaction_counter[EXECUTING][node->client_data.transaction];
			pthread_mutex_unlock(&mutex_transaction_counter[EXECUTING][node->client_data.transaction]);
			continue;
		}
#ifdef STANDALONE
		if (gettimeofday(&rt0, NULL) == -1)
		{
			perror("gettimeofday");
		}
#endif /* STANDALONE */
#ifdef ODBC
		if (process_transaction(node->client_data.transaction, &odbcc,
			&node->client_data.transaction_data) != OK)
		{
			LOG_ERROR_MESSAGE("process_transaction() error on %s",
				transaction_name[node->client_data.transaction]);
			/*
			 * Assume this isn't a fatal error, send the results back,
			 * and try processing the next
			 * transaction.
			 */
		}
#endif /* ODBC */
#ifdef STANDALONE
		if (gettimeofday(&rt1, NULL) == -1)
		{
			perror("gettimeofday");
		}
		response_time = difftimeval(rt1, rt0);
		pthread_mutex_lock(&mutex_mix_log);
		fprintf(log_mix, "%d,%c,%f,%d\n", time(NULL),
		transaction_short_name[node->client_data.transaction], response_time,
			pthread_self());
			fflush(log_mix);
		pthread_mutex_unlock(&mutex_mix_log);
#endif /* STANDALONE */
#ifndef STANDALONE
		length = send_transaction_data(node->s, &node->client_data);
		if (length == ERROR)
		{
			LOG_ERROR_MESSAGE("send_transaction_data() error");
			/*
			 * Assume this isn't a fatal error and try processing the next
			 * transaction.
			 */
		}
#endif /* STANDALONE */
		pthread_mutex_lock(&mutex_transaction_counter[EXECUTING][node->client_data.transaction]);
		--transaction_counter[EXECUTING][node->client_data.transaction];
		pthread_mutex_unlock(&mutex_transaction_counter[EXECUTING][node->client_data.transaction]);
		recycle_node(node);
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
			LOG_ERROR_MESSAGE("error creating db thread");
			return ERROR;
		}

		/* Keep a count of how many DB worker threads have started. */
		sem_post(&db_worker_count);
	}

	return OK;
}
