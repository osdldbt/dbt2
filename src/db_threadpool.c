/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 *
 * 31 June 2002
 */

#define _POSIX_C_SOURCE 199309L

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include <strings.h>

#include "client.h"
#include "client_interface.h"
#include "common.h"
#include "db.h"
#include "db_threadpool.h"
#include "listener.h"
#include "logging.h"
#include "transaction_queue.h"

/* Function Prototypes */
void *db_worker(void *data);
int startup();

/* Global Variables */
int *worker_count;
time_t *last_txn;

/* These should probably be handled differently. */
sem_t db_worker_count;

#if defined(HAVE_MYSQL) || defined(HAVE_ODBC)
extern char dbt2_user[128];
extern char dbt2_pass[128];
#endif

#ifdef HAVE_LIBPQ
extern char postmaster_port[32];
#endif /* HAVE_LIBPQ */

#ifdef HAVE_MYSQL
extern char dbt2_mysql_port[32];
extern char dbt2_mysql_host[HOSTNAMELEN + 1];
extern char dbt2_mysql_socket[256];
#endif /* HAVE_MYSQL */

void *db_worker(void *data) {
	int id = *((int *) data); /* Whoa... */

	int length;
	struct transaction_queue_node_t *node;
	struct db_context_t dbc;

	/* Open a connection to the database. */

	if (init_dbc(&dbc) != 0) {
		return NULL;
	}

	if (!exiting && connect_to_db(&dbc) != OK) {
		LOG_ERROR_MESSAGE("connect_to_db() error, terminating program");
		printf("cannot connect to database(see details in error.log "
			   "file, exiting...\n");
		exit(1);
	}

	while (!exiting) {
		/*
		 * I know this loop will prevent the program from exiting
		 * because of the dequeue...
		 */
		node = dequeue_transaction();
		if (node == NULL) {
			LOG_ERROR_MESSAGE("dequeue was null");
			continue;
		}
		node->client_data.status = process_transaction(
				node->client_data.transaction, &dbc,
				&node->client_data.transaction_data);
		if (node->client_data.status == ERROR) {
			LOG_ERROR_MESSAGE(
					"process_transaction() error on %s",
					transaction_name[node->client_data.transaction]);
			/*
			 * Assume this isn't a fatal error, send the results
			 * back, and try processing the next transaction.
			 */
		}

		length = send_transaction_data(node->s, &node->client_data);
		if (length == ERROR) {
			LOG_ERROR_MESSAGE("send_transaction_data() error");
			/*
			 * Assume this isn't a fatal error and try processing
			 * the next transaction.
			 */
		}
		pthread_mutex_lock(
				&mutex_transaction_counter[REQ_EXECUTING]
										  [node->client_data.transaction]);
		--transaction_counter[REQ_EXECUTING][node->client_data.transaction];
		pthread_mutex_unlock(
				&mutex_transaction_counter[REQ_EXECUTING]
										  [node->client_data.transaction]);
		recycle_node(node);

		/* Keep track of how many transactions this thread has done. */
		++worker_count[id];

		/* Keep track of then the last transaction was execute. */
		time(&last_txn[id]);
	}

	/* Disconnect from the database. */
	disconnect_from_db(&dbc);

	sem_wait(&db_worker_count);

	return NULL; /* keep compiler quiet */
}

int db_threadpool_init() {
	struct timespec ts, rem;
	int i;
	extern int errno;

	if (sem_init(&db_worker_count, 0, 0) != 0) {
		LOG_ERROR_MESSAGE("cannot init db_worker_count\n");
		return ERROR;
	}

	worker_count = (int *) malloc(sizeof(int) * db_connections);
	memset(worker_count, 0, sizeof(int) * db_connections);

	ts.tv_sec = (time_t) db_conn_sleep / 1000;
	ts.tv_nsec = (long) (db_conn_sleep % 1000) * 1000000;

	last_txn = (time_t *) malloc(sizeof(time_t) * db_connections);

	for (i = 0; i < db_connections; i++) {
		int ret;
		pthread_t tid;
		pthread_attr_t attr;
		size_t stacksize = 262144; /* 256 kilobytes. */

		/*
		 * Initialize the last_txn array with the time right before
		 * the worker thread is started.  This looks better than
		 * initializing the array with zeros.
		 */
		time(&last_txn[i]);

		/*
		 * Is it possible for i to change before db_worker can copy it?
		 */
		if (pthread_attr_init(&attr) != 0) {
			LOG_ERROR_MESSAGE("could not init pthread attr");
			return ERROR;
		}
		if (pthread_attr_setstacksize(&attr, stacksize) != 0) {
			LOG_ERROR_MESSAGE("could not set pthread stack size");
			return ERROR;
		}
		ret = pthread_create(&tid, &attr, &db_worker, &i);
		if (ret != 0) {
			LOG_ERROR_MESSAGE("error creating db thread");
			if (ret == EAGAIN) {
				LOG_ERROR_MESSAGE("not enough system resources");
			}
			return ERROR;
		}

		/* Keep a count of how many DB worker threads have started. */
		sem_post(&db_worker_count);

		/*
		 * Don't let the database connection attempts occur too fast.
		 */
		while (nanosleep(&ts, &rem) == -1) {
			if (errno == EINTR) {
				memcpy(&ts, &rem, sizeof(struct timespec));
			} else {
				LOG_ERROR_MESSAGE(
						"sleep time invalid %d s %ls ns", ts.tv_sec,
						ts.tv_nsec);
				break;
			}
		}
		pthread_attr_destroy(&attr);
	}
	return OK;
}
