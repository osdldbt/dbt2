/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002      Open Source Development Labs, Inc.
 *               2002-2022 Mark Wong
 *
 * 7 August 2002
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "common.h"
#include "logging.h"
#include "driver.h"
#include "client_interface.h"
#include "input_data_generator.h"
#ifdef STANDALONE
#include "db.h"
#include "transaction_queue.h"
#include "db_threadpool.h"
#endif /* STANDALONE */

#include "entropy.h"

#define MIX_LOG_NAME "mix.log"

void *terminal_worker(void *data);

/* Global Variables */
pthread_t** g_tid = NULL;
extern int client_port;
char hostname[32];
int duration = 0;
int stop_time = 0;
int w_id_min = 0, w_id_max = 0;
int terminals_limit = 0;
int mode_altered = 0;
int client_conn_sleep = 1000; /* milliseconds */
int spread = 1;
int threads_start_time= 0;
int fork_per_processor = 1; /* Not used by this driver. */

FILE *log_mix = NULL;
pthread_mutex_t mutex_mix_log = PTHREAD_MUTEX_INITIALIZER;

int terminal_state[3][TRANSACTION_MAX] = {
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 }
};

pthread_mutex_t mutex_terminal_state[3][TRANSACTION_MAX] = {
	{ PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_MUTEX_INITIALIZER },
	{ PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_MUTEX_INITIALIZER },
	{ PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_MUTEX_INITIALIZER }
};

int init_driver_logging()
{
	char log_filename[512];

	log_filename[511] = '\0';
	snprintf(log_filename, 511, "%s/%s", output_path, MIX_LOG_NAME);
	log_mix = fopen(log_filename, "w");
	if (log_mix == NULL) {
		fprintf(stderr, "cannot open %s\n", log_filename);
		return ERROR;
	}

	return OK;
}

int integrity_terminal_worker()
{
	int sockfd;

	struct client_transaction_t client_data;
	extern int errno;

	unsigned long long local_seed = 0;
	pcg64f_random_t rng;

	entropy_getbytes((void *) &local_seed, sizeof(local_seed));
	pcg64f_srandom_r(&rng, local_seed);

	/* Connect to the client program. */
	sockfd = connect_to_client(hostname, client_port);
	if (sockfd < 1) {
		LOG_ERROR_MESSAGE("connect_to_client() failed, thread exiting...");
		printf("connect_to_client() failed, thread exiting...\n");
		pthread_exit(NULL);
	}

	client_data.transaction = INTEGRITY;
	generate_input_data(&rng, client_data.transaction,
			&client_data.transaction_data, table_cardinality.warehouses);

#ifdef DEBUG
	printf("executing transaction %c\n"
			 transaction_short_name[client_data.transaction]);
	fflush(stdout);

	LOG_ERROR_MESSAGE("executing transaction %c",
			transaction_short_name[client_data.transaction]);
#endif /* DEBUG */

	send_transaction_data(sockfd, &client_data);
	receive_transaction_data(sockfd, &client_data);
	close(sockfd);

	return client_data.status;
}

int start_driver()
{
	int i, j;
	struct timespec ts, rem;

	/* Just used to count the number of threads created. */
	int count;

	ts.tv_sec = (time_t) (client_conn_sleep / 1000);
	ts.tv_nsec = (long) (client_conn_sleep % 1000) * 1000000;
#ifdef STANDALONE
	/* Open database connectiosn. */
/*
	if (db_threadpool_init() != OK) {
		LOG_ERROR_MESSAGE("cannot open database connections");
		return ERROR;
	}
*/
#endif /* STANDALONE */

	/* Caulculate when the test should stop. */
	if (terminals_limit)
		threads_start_time =
				(int) ((double) client_conn_sleep / 1000.0 * terminals_limit);
	else
		threads_start_time = (int) ((double) client_conn_sleep / 1000.0 *
				(double) terminals_per_warehouse *
				(double) (w_id_max - w_id_min));

	stop_time = time(NULL) + duration + threads_start_time;
	printf("driver is starting to ramp up at time %d\n", (int) time(NULL));
	printf("driver will ramp up in  %d seconds\n", threads_start_time);
	printf("will stop test at time %d\n", stop_time);

	/* allocate g_tid */
	g_tid = (pthread_t**) malloc(sizeof(pthread_t*) * (w_id_max+1)/spread);
	memset(g_tid, 0, sizeof(pthread_t *) * (w_id_max + 1) / spread);
	count = 1;
	for (i = w_id_min; i < w_id_max + 1; i += spread) {
		g_tid[i] = (pthread_t*)
				malloc(sizeof(pthread_t) * terminals_per_warehouse);
		if (terminals_limit && count > terminals_limit) {
			break;
		}
		++count;
	}

	count = 1;
	for (j = 0; j < terminals_per_warehouse; j++) {
		for (i = w_id_min; i < w_id_max + 1; i += spread) {
			int ret;
			pthread_attr_t attr;
			size_t stacksize = 131072; /* 128 kilobytes. */
			struct terminal_context_t *tc;

			tc = (struct terminal_context_t *)
					malloc(sizeof(struct terminal_context_t));
			tc->w_id = i;
			tc->d_id = j + 1;
			if (pthread_attr_init(&attr) != 0) {
				LOG_ERROR_MESSAGE("could not init pthread attr: %d",
						(i + j + 1) * terminals_per_warehouse);
				return ERROR;
			}
			if (pthread_attr_setstacksize(&attr, stacksize) != 0) {
				LOG_ERROR_MESSAGE("could not set pthread stack size: %d",
						(i + j + 1) * terminals_per_warehouse);
				return ERROR;
			}
			ret = pthread_create(&g_tid[i][j], &attr, &terminal_worker,
					(void *) tc);
			if (ret != 0) {
				perror("pthread_create");
				LOG_ERROR_MESSAGE( "error creating terminal thread: %d",
						(i + j + 1) * terminals_per_warehouse);
				if (ret == EAGAIN) {
					LOG_ERROR_MESSAGE( "not enough system resources: %d",
							(i + j + 1) * terminals_per_warehouse);
				}
				return ERROR;
			}

			if ((count % 100) == 0) {
				printf("%d / %d threads started...\n", count,
						terminals_per_warehouse *
								(w_id_max - w_id_min + 1) / spread);
				fflush(stdout);
			}

			/* Sleep for between starting terminals. */
			while (nanosleep(&ts, &rem) == -1) {
				if (errno == EINTR) {
					memcpy(&ts, &rem, sizeof(struct timespec));
				} else {
					LOG_ERROR_MESSAGE(
							"sleep time invalid %d s %ls ns",
							ts.tv_sec, ts.tv_nsec);
					break;
				}
			}
			pthread_attr_destroy(&attr);

			/*
			 * Need to break out of the inner loop then break out of the other
			 * loop.
			 */
			if (terminals_limit && count > terminals_limit) {
				break;
			}
			++count;
		}
		/* Breaking out of the outer loop. */
		if (terminals_limit && count > terminals_limit) {
			break;
		}
	}
	printf("terminals started...\n");

	/* Note that the driver has started up all threads in the log. */
	pthread_mutex_lock(&mutex_mix_log);
	fprintf(log_mix, "%d,START,,,%d,,\n", (int) time(NULL), getpid());
	fflush(log_mix);
	pthread_mutex_unlock(&mutex_mix_log);

	/* wait until all threads quit */
	count = 1;
	for (j = 0; j < terminals_per_warehouse; j++) {
		for (i = w_id_min; i < w_id_max + 1; i += spread) {
			/*
			 * Need to break out of the inner loop then break out of the other
			 * loop.
			 */
			if (terminals_limit && count > terminals_limit) {
				break;
			}
			if (pthread_join(g_tid[i][j], NULL) != 0) {
				LOG_ERROR_MESSAGE("error join terminal thread");
				return ERROR;
			}
			++count;
		}
		/* Breaking out of the outer loop. */
		if (terminals_limit && count > terminals_limit) {
			break;
		}
	}
	printf("driver is exiting normally\n");
	return OK;
}

void *terminal_worker(void *data)
{
#ifndef STANDALONE
	int sockfd;
#endif /* NOT STANDALONE */

	struct terminal_context_t *tc;
	struct client_transaction_t client_data;
	double threshold;
	int keying_time;
	struct timespec thinking_time, rem;
	int mean_think_time; /* In milliseconds. */
	struct timeval rt0, rt1;
	double response_time;
	extern int errno;
	int rc;
	unsigned long long local_seed = 0;
	pid_t pid;
	pthread_t tid;
	char code;
	pcg64f_random_t rng;

#ifdef STANDALONE
	struct db_context_t dbc;
	struct transaction_queue_node_t *node =
			(struct transaction_queue_node_t *)
			malloc(sizeof(struct transaction_queue_node_t));
	extern char sname[32];
	extern int exiting;
#ifdef HAVE_LIBPQ
	extern char postmaster_port[32];
#endif /* HAVE_LIBPQ */

#ifdef HAVE_MYSQL
	extern char dbt2_mysql_port[32];
#endif /* HAVE_MYSQL */

#endif /* STANDALONE */

	tc = (struct terminal_context_t *) data;
	/* Each thread needs to seed in Linux. */
    tid = pthread_self();
    pid = getpid();
	entropy_getbytes((void *) &local_seed, sizeof(local_seed));
	printf("seed for %d:%lx : %llu\n", pid, tid, local_seed);
	fflush(stdout);
	pcg64f_srandom_r(&rng, local_seed);

#ifdef STANDALONE
#ifdef HAVE_ODBC
	db_init(sname, DB_USER, DB_PASS);
#endif /* HAVE_ODBC */
#ifdef HAVE_LIBPQ
	db_init(DB_NAME, sname, postmaster_port);
#endif /* HAVE_LIBPQ */
#ifdef HAVE_MYSQL
	printf("CONNECTED TO DB |%s| |%s| |%s|\n", DB_NAME, sname, dbt2_mysql_port);
	db_init(sname, "", dbt2_mysql_port);
#endif /* HAVE_MYSQL */
#ifdef HAVE_SQLITE3
	db_init(sname);
#endif /* HAVE_SQLITE3 */

	if (!exiting && connect_to_db(&dbc) != OK) {
		LOG_ERROR_MESSAGE("db_connect() error, terminating program");
		printf("cannot connect to database, exiting...\n");
		exit(1);
	}
#else
	/* Connect to the client program. */
	sockfd = connect_to_client(hostname, client_port);
	if (sockfd < 1) {
		LOG_ERROR_MESSAGE( "connect_to_client() failed, thread exiting...");
		printf("connect_to_client() failed, thread exiting...\n");
		pthread_exit(NULL);
	}
#endif /* STANDALONE */

	do {
		if (mode_altered == 1) {
			/*
			 * Determine w_id and d_id for the client per transaction.
			 * TODO: Find an efficient way to make sure more than one thread
			 * isn't execute with the same warehouse and district as asnother
			 * thread.
			 */
			tc->w_id = w_id_min + (int) get_random(&rng, w_id_max - w_id_min + 1);
			tc->d_id = (int) get_random(&rng, table_cardinality.districts) + 1;
		}

		/*
		 * Determine which transaction to execute, minimum keying time,
		 * and mean think time.
		 */
		threshold = get_percentage(&rng);
		if (threshold < transaction_mix.new_order_threshold) {
			client_data.transaction = NEW_ORDER;
			keying_time = key_time.new_order;
			mean_think_time = think_time.new_order;
		} else if (transaction_mix.payment_actual != 0 &&
			threshold < transaction_mix.payment_threshold) {
			client_data.transaction = PAYMENT;
			keying_time = key_time.payment;
			mean_think_time = think_time.payment;
		} else if (transaction_mix.order_status_actual != 0 &&
			threshold < transaction_mix.order_status_threshold) {
			client_data.transaction = ORDER_STATUS;
			keying_time = key_time.order_status;
			mean_think_time = think_time.order_status;
		} else if (transaction_mix.delivery_actual != 0 &&
			threshold < transaction_mix.delivery_threshold) {
			client_data.transaction = DELIVERY;
			keying_time = key_time.delivery;
			mean_think_time = think_time.delivery;
		} else {
			client_data.transaction = STOCK_LEVEL;
			keying_time = key_time.stock_level;
			mean_think_time = think_time.stock_level;
		}

#ifdef DEBUG
		printf("executing transaction %c\n",
			transaction_short_name[client_data.transaction]);
		fflush(stdout);
		LOG_ERROR_MESSAGE("executing transaction %c",
			transaction_short_name[client_data.transaction]);
#endif /* DEBUG */

		/* Generate the input data for the transaction. */
		if (client_data.transaction != STOCK_LEVEL) {
			generate_input_data(&rng, client_data.transaction,
					&client_data.transaction_data, tc->w_id);
		} else {
			generate_input_data2(&rng, client_data.transaction,
					&client_data.transaction_data, tc->w_id, tc->d_id);
		}

		/* Keying time... */
		pthread_mutex_lock(
				&mutex_terminal_state[KEYING][client_data.transaction]);
		++terminal_state[KEYING][client_data.transaction];
		pthread_mutex_unlock(
				&mutex_terminal_state[KEYING][client_data.transaction]);
		if (time(NULL) < stop_time) {
			sleep(keying_time);
		} else {
			break;
		}
		pthread_mutex_lock(
				&mutex_terminal_state[KEYING][client_data.transaction]);
		--terminal_state[KEYING][client_data.transaction];
		pthread_mutex_unlock(
				&mutex_terminal_state[KEYING][client_data.transaction]);

		/* Note this thread is executing a transation. */
		pthread_mutex_lock(
				&mutex_terminal_state[EXECUTING][client_data.transaction]);
		++terminal_state[EXECUTING][client_data.transaction];
		pthread_mutex_unlock(
				&mutex_terminal_state[EXECUTING][client_data.transaction]);
		/* Execute transaction and record the response time. */
		if (gettimeofday(&rt0, NULL) == -1) {
			perror("gettimeofday");
		}
#ifdef STANDALONE
		memcpy(&node->client_data, &client_data, sizeof(client_data));
/*
		enqueue_transaction(node);
		node = get_node();
		if (node == NULL) {
			LOG_ERROR_MESSAGE("Cannot get a transaction node.\n");
		}
*/
		rc = process_transaction(node->client_data.transaction, &dbc,
				&node->client_data.transaction_data);
		if (rc == ERROR) {
			LOG_ERROR_MESSAGE("process_transaction() error on %s",
					transaction_name[node->client_data.transaction]);
		}
#else /* STANDALONE */
		send_transaction_data(sockfd, &client_data);
		receive_transaction_data(sockfd, &client_data);
		rc = client_data.status;
#endif /* STANDALONE */
		if (gettimeofday(&rt1, NULL) == -1) {
			perror("gettimeofday");
		}
		response_time = difftimeval(rt1, rt0);
		pthread_mutex_lock(&mutex_mix_log);
		if (rc == OK) {
			code = 'C';
		} else if (rc == STATUS_ROLLBACK) {
			code = 'R';
		} else if (rc == ERROR) {
			code = 'E';
		}
		fprintf(log_mix, "%d,%c,%c,%f,%lx,%d,%d\n", (int) time(NULL),
				transaction_short_name[client_data.transaction], code,
				response_time, pthread_self(), tc->w_id, tc->d_id);
		fflush(log_mix);
		pthread_mutex_unlock(&mutex_mix_log);
		pthread_mutex_lock(&mutex_terminal_state[EXECUTING][client_data.transaction]);
		--terminal_state[EXECUTING][client_data.transaction];
		pthread_mutex_unlock(&mutex_terminal_state[EXECUTING][client_data.transaction]);

		/* Thinking time... */
		pthread_mutex_lock(&mutex_terminal_state[THINKING][client_data.transaction]);
		++terminal_state[THINKING][client_data.transaction];
		pthread_mutex_unlock(&mutex_terminal_state[THINKING][client_data.transaction]);
		if (time(NULL) < stop_time) {
			thinking_time.tv_nsec =
					(long) get_think_time(&rng, mean_think_time);
			thinking_time.tv_sec = (time_t) (thinking_time.tv_nsec / 1000);
			thinking_time.tv_nsec = (thinking_time.tv_nsec % 1000) * 1000000;
			while (nanosleep(&thinking_time, &rem) == -1) {
				if (errno == EINTR) {
					memcpy(&thinking_time, &rem, sizeof(struct timespec));
				} else {
					LOG_ERROR_MESSAGE(
							"sleep time invalid %d s %ls ns",
							thinking_time.tv_sec,
							thinking_time.tv_nsec);
					break;
				}
			}
		}
		pthread_mutex_lock(&mutex_terminal_state[THINKING][client_data.transaction]);
		--terminal_state[THINKING][client_data.transaction];
		pthread_mutex_unlock(&mutex_terminal_state[THINKING][client_data.transaction]);
	} while (time(NULL) < stop_time);

#ifdef STANDALONE
	/*recycle_node(node);*/
#endif /* STANDALONE */
	/* Note when each thread has exited. */
	pthread_mutex_lock(&mutex_mix_log);
	fprintf(log_mix, "%d,TERMINATED,,,%d,,\n", (int) time(NULL),
			(int) pthread_self());
	fflush(log_mix);
	pthread_mutex_unlock(&mutex_mix_log);
	return NULL; /* keep the compiler quiet */
}
