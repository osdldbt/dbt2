/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>

#include "client.h"
#include "common.h"
#include "db_threadpool.h"
#include "listener.h"
#include "logging.h"
#include "transaction_queue.h"

/* Function Prototypes */
int parse_command(char *command);

int startup() {
	pthread_t tid;
	int ret;
	char command[128];
	extern char sname[SNAMELEN + 1];

	init_logging();
	free(output_path);

	printf("opening %d connection(s) to %s...\n", db_connections, sname);
	fflush(stdout);

	if (init_transaction_queue() != OK) {
		LOG_ERROR_MESSAGE("init_transaction_queue() failed");
		return ERROR;
	}

	ret = pthread_create(&tid, NULL, &init_listener, &sockfd);
	if (ret != 0) {
		LOG_ERROR_MESSAGE("pthread_create() error with init_listener()");
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

	printf("%d DB worker threads have started\n", db_connections);
	fflush(stdout);

	printf("client has started\n");

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
	} while (1);

	printf("closing socket...\n");
	close(sockfd);
	return OK;
}

void status() {
	unsigned int count;
	time_t current_time;
	int i, j;
	int stats[2][TRANSACTION_MAX];

	printf("------\n");
	sem_getvalue(&queue_length, &count);
	printf("transactions waiting = %d\n", count);
	sem_getvalue(&db_worker_count, &count);
	printf("db connections = %d\n", count);
	sem_getvalue(&listener_worker_count, &count);
	printf("terminal connections = %d\n", count);
	for (i = 0; i < 2; i++) {
		for (j = 0; j < TRANSACTION_MAX; j++) {
			pthread_mutex_lock(&mutex_transaction_counter[i][j]);
			stats[i][j] = transaction_counter[i][j];
			pthread_mutex_unlock(&mutex_transaction_counter[i][j]);
		}
	}
	printf("transaction   queued  executing\n");
	printf("------------  ------  ---------\n");
	for (i = 0; i < TRANSACTION_MAX; i++) {
		printf("%12s  %6d  %9d\n", transaction_name[i], stats[REQ_QUEUED][i],
			   stats[REQ_EXECUTING][i]);
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
}
