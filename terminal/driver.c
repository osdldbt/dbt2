/*
 * driver.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
 *
 * 7 august 2002
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <semaphore.h>
#include <ctype.h>
#include <common.h>
#include <string.h>
#include <logging.h>
#include <driver.h>
#include <client_interface.h>
#include <input_data_generator.h>
#ifdef STANDALONE
#include <db.h>
#include <transaction_queue.h>
#include <odbc_common.h>
#include <db_threadpool.h>
#endif /* STANDALONE */

#define MIX_LOG_NAME "mix.log"

void *terminal_worker(void *data);

/* Global Variables */
struct transaction_mix_t transaction_mix;
struct key_time_t key_time;
struct think_time_t think_time;
char hostname[32];
int client_port = CLIENT_PORT;
int duration = 0;
int stop_time;
sem_t terminal_count;
int w_id_min, w_id_max;
int terminals_per_warehouse;
int mode_altered = 1;
unsigned int seed = -1;

FILE *log_mix;
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

int init_driver()
{
	terminals_per_warehouse = table_cardinality.districts;

	transaction_mix.delivery_actual = MIX_DELIVERY;
	transaction_mix.order_status_actual = MIX_ORDER_STATUS;
	transaction_mix.payment_actual = MIX_PAYMENT;
	transaction_mix.stock_level_actual = MIX_STOCK_LEVEL;

	key_time.delivery = KEY_TIME_DELIVERY;
	key_time.new_order = KEY_TIME_NEW_ORDER;
	key_time.order_status = KEY_TIME_ORDER_STATUS;
	key_time.payment = KEY_TIME_PAYMENT;
	key_time.stock_level = KEY_TIME_STOCK_LEVEL;

	think_time.delivery = THINK_TIME_DELIVERY;
	think_time.new_order = THINK_TIME_NEW_ORDER;
	think_time.order_status = THINK_TIME_ORDER_STATUS;
	think_time.payment = THINK_TIME_PAYMENT;
	think_time.stock_level = THINK_TIME_STOCK_LEVEL;

	if (sem_init(&terminal_count, 0, 0) != 0)
	{
		LOG_ERROR_MESSAGE("cannot sem_init() terminal_count");
		return ERROR;
	}

	log_mix = fopen(MIX_LOG_NAME, "w");
	if (log_mix == NULL)
	{
		fprintf(stderr, "cannot open %s\n", MIX_LOG_NAME);
		return ERROR;
	}

	return OK;
}

int recalculate_mix()
{
	/*
	 * Calculate the actual percentage that the New-Order transaction will
	 * be execute.
	 */
	transaction_mix.new_order_actual = 1.0 -
		(transaction_mix.delivery_actual +
		transaction_mix.order_status_actual +
		transaction_mix.payment_actual +
		transaction_mix.stock_level_actual);

	if (transaction_mix.new_order_actual < 0.0)
	{
		LOG_ERROR_MESSAGE(
			"invalid transaction mix. d %0.1f. o %0.1f. p %0.1f. s %0.1f. n %0.1f.\n",
			transaction_mix.delivery_actual,
			transaction_mix.order_status_actual,
			transaction_mix.payment_actual,
			transaction_mix.stock_level_actual,
			transaction_mix.new_order_actual);
		return ERROR;
	}

	/* Calculate the thresholds of each transaction. */
	transaction_mix.new_order_threshold = transaction_mix.new_order_actual;
	transaction_mix.payment_threshold = transaction_mix.new_order_threshold +
		transaction_mix.payment_actual;
	transaction_mix.order_status_threshold = transaction_mix.payment_threshold
		+ transaction_mix.order_status_actual;
	transaction_mix.delivery_threshold = transaction_mix.order_status_threshold
		+ transaction_mix.delivery_actual;
	transaction_mix.stock_level_threshold =
		transaction_mix.delivery_threshold +
		transaction_mix.stock_level_actual;

	return OK;
}

int set_client_hostname(char *addr)
{
	strcpy(hostname, addr);
	return OK;
}

int set_client_port(int port)
{
	client_port = port;
	return OK;
}

int set_duration(int seconds)
{
	duration = seconds;
	stop_time = time(NULL) + duration;
	return OK;
}

int set_table_cardinality(int table, int cardinality)
{
	switch (table)
	{
		case TABLE_WAREHOUSE:
			table_cardinality.warehouses = cardinality;
			break;
		case TABLE_CUSTOMER:
			table_cardinality.customers = cardinality;
			break;
		case TABLE_ITEM:
			table_cardinality.items = cardinality;
			break;
		case TABLE_ORDER:
			table_cardinality.orders = cardinality;
			break;
		case TABLE_NEW_ORDER:
			table_cardinality.new_orders = cardinality;
			break;
		default:
			return ERROR;
	}

	return OK;
}

int set_transaction_mix(int transaction, double mix)
{
	switch (transaction)
	{
		case DELIVERY:
			transaction_mix.delivery_actual = mix;
			break;
		case NEW_ORDER:
			transaction_mix.new_order_actual = mix;
			break;
		case ORDER_STATUS:
			transaction_mix.order_status_actual = mix;
			break;
		case PAYMENT:
			transaction_mix.payment_actual = mix;
			break;
		case STOCK_LEVEL:
			transaction_mix.stock_level_actual = mix;
			break;
		default:
			return ERROR;
	}
	return OK;
}

int start_driver()
{
	int i, j;
	int count;

#ifdef STANDALONE
	/* Open database connectiosn. */
/*
	if (db_threadpool_init() != OK)
	{
		LOG_ERROR_MESSAGE("cannot open database connections");
		return ERROR;
	}
*/
#endif /* STANDALONE */

	for (i = w_id_min; i < w_id_max + 1; i++)
	{
		for (j = 0; j < terminals_per_warehouse; j++)
		{
			pthread_t tid;
			struct terminal_context_t *tc;

			tc = (struct terminal_context_t *)
				malloc(sizeof(struct terminal_context_t));
			tc->w_id = i + 1;
			tc->d_id = j + 1;
			if (pthread_create(&tid, NULL, &terminal_worker, (void *) tc) != 0)
			{
				LOG_ERROR_MESSAGE("error creating terminal thread");
				return ERROR;
			}
			sleep(1); /* Sleep for 1 second between starting terminals. */
		}
		if (mode_altered == 1)
		{
			/*
			 * This effectively allows one client to touch the entire
			 * warehouse range.  The setting of w_id and d_id is moot here.
			 */
			break;
		}
	}
	printf("terminals started...\n");

	/* Note that the driver has started up all threads in the log. */
	pthread_mutex_lock(&mutex_mix_log);
	fprintf(log_mix, "START\n");
	fflush(log_mix);
	pthread_mutex_unlock(&mutex_mix_log);

	do
	{
		sem_getvalue(&terminal_count, &count);
/*
		printf("transaction\texec\tkeying\tthinking [%d]\n", count);
		for (i = 0; i < TRANSACTION_MAX; i++)
		{
			printf("%s", transaction_name[i]);
			for (j = 0; j < 3; j++)
			{
				pthread_mutex_lock(&mutex_terminal_state[j][i]);
				printf("\t%d", terminal_state[j][i]);
				pthread_mutex_unlock(&mutex_terminal_state[j][i]);
			}
			printf("\n");
		}
		printf("\n");
		sleep(1);
*/
		sleep(30);
	}
	while (count > 0);

	return OK;
}

void *terminal_worker(void *data)
{
#ifndef STANDALONE
	int length;
#endif /* STANDALONE */
	struct terminal_context_t *tc;
	struct client_transaction_t client_data;
	double threshold;
	int keying_time;
	struct timespec thinking_time, rem;
	int mean_think_time; /* In milliseconds. */
	struct timeval rt0, rt1;
	double response_time;
	extern int errno;
#ifndef STANDALONE
	int sockfd;
#endif /* STANDALONE */
	int rc;

#ifdef STANDALONE
	struct odbc_context_t odbcc;
	struct transaction_queue_node_t *node = (struct transaction_queue_node_t *)
		malloc(sizeof(struct transaction_queue_node_t));
	extern char sname[32];
	extern int exiting;
#endif /* STANDALONE */

	tc = (struct terminal_context_t *) data;
	/* Each thread needs to seed in Linux. */
	if (seed == -1)
	{
		seed = time(NULL) * pthread_self() * getpid();
	}
	printf("seed: %u\n", seed);
	fflush(stdout);
	srand(seed);

	/* Keep a count of how many terminals are being emulated. */
	sem_post(&terminal_count);

#ifdef STANDALONE
	odbc_init(sname, DB_USER, DB_PASS);
	if (!exiting && odbc_connect(&odbcc) != OK)
	{
		LOG_ERROR_MESSAGE("odbc_connect() error, terminating program");
		printf("cannot connect to database, exiting...\n");
		exit(1);
	}
#else
	/* Connect to the client program. */
	sockfd = connect_to_client(hostname, client_port);
	if (sockfd < 1)
	{
		LOG_ERROR_MESSAGE("connect_to_client() failed, thread exiting...\n");
		pthread_exit(NULL);
	}
#endif /* STANDALONE */

	do
	{
		if (mode_altered == 1)
		{
			/* Determine w_id and d_id for the client per transaction. */

			tc->w_id = w_id_min + get_random(w_id_max - w_id_min + 1);
			tc->d_id = get_random(table_cardinality.districts);
		}

		/*
		 * Determine which transaction to execute, minimum keying time, and
         * mean think time.
		 */
		threshold = get_percentage();
		if (threshold < transaction_mix.new_order_threshold)
		{
			client_data.transaction = NEW_ORDER;
			keying_time = key_time.new_order;
			mean_think_time = think_time.new_order;
		}
		else if (transaction_mix.payment_actual != 0 &&
			threshold < transaction_mix.payment_threshold)
		{
			client_data.transaction = PAYMENT;
			keying_time = key_time.payment;
			mean_think_time = think_time.payment;
		}
		else if (transaction_mix.order_status_actual != 0 &&
			threshold < transaction_mix.order_status_threshold)
		{
			client_data.transaction = ORDER_STATUS;
			keying_time = key_time.order_status;
			mean_think_time = think_time.order_status;
		}
		else if (transaction_mix.delivery_actual != 0 &&
			threshold < transaction_mix.delivery_threshold)
		{
			client_data.transaction = DELIVERY;
			keying_time = key_time.delivery;
			mean_think_time = think_time.delivery;
		}
		else
		{
			client_data.transaction = STOCK_LEVEL;
			keying_time = key_time.stock_level;
			mean_think_time = think_time.stock_level;
		}

#ifdef DEBUG
		LOG_ERROR_MESSAGE("executing transaction %c", 
			transaction_short_name[client_data.transaction]);
#endif /* DEBUG */

		/* Generate the input data for the transaction. */
		if (client_data.transaction != STOCK_LEVEL)
		{
			generate_input_data(client_data.transaction,
				&client_data.transaction_data, tc->w_id);
		}
		else
		{
			generate_input_data2(client_data.transaction,
				&client_data.transaction_data, tc->w_id, tc->d_id);
		}

		/* Keying time... */
		pthread_mutex_lock(&mutex_terminal_state[KEYING][client_data.transaction]);
		++terminal_state[KEYING][client_data.transaction];
		pthread_mutex_unlock(&mutex_terminal_state[KEYING][client_data.transaction]);
		if (time(NULL) < stop_time)
		{
			sleep(keying_time);
		}
		else
		{
			break;
		}
		pthread_mutex_lock(&mutex_terminal_state[KEYING][client_data.transaction]);
		--terminal_state[KEYING][client_data.transaction];
		pthread_mutex_unlock(&mutex_terminal_state[KEYING][client_data.transaction]);

		/* Note this thread is executing a transation. */
		pthread_mutex_lock(&mutex_terminal_state[EXECUTING][client_data.transaction]);
		++terminal_state[EXECUTING][client_data.transaction];
		pthread_mutex_unlock(&mutex_terminal_state[EXECUTING][client_data.transaction]);
/* #ifndef STANDALONE */
		/* Execute transaction and record the response time. */
		if (gettimeofday(&rt0, NULL) == -1)
		{
			perror("gettimeofday");
		}
/*#endif*/ /* STANDALONE */
#ifdef STANDALONE
		memcpy(&node->client_data, &client_data, sizeof(client_data));
/*
		enqueue_transaction(node);
		node = get_node();
		if (node == NULL)
		{
			LOG_ERROR_MESSAGE("Cannot get a transaction node.\n");
		}
*/
		rc = process_transaction(node->client_data.transaction, &odbcc,
			&node->client_data.transaction_data);
		if (rc == ERROR)
		{
			LOG_ERROR_MESSAGE("process_transaction() error on %s",
			transaction_name[node->client_data.transaction]);
		}
#else /* STANDALONE */
		length = send_transaction_data(sockfd, &client_data);
		length = receive_transaction_data(sockfd, &client_data);
#endif /* STANDALONE */
		if (gettimeofday(&rt1, NULL) == -1)
		{
			perror("gettimeofday");
		}
		response_time = difftimeval(rt1, rt0);
		pthread_mutex_lock(&mutex_mix_log);
		if (rc == OK)
		{
			fprintf(log_mix, "%d,%c,%f,%d\n", time(NULL),
				transaction_short_name[client_data.transaction],
				response_time, pthread_self());
		}
		else if (rc == STATUS_ROLLBACK)
		{
			fprintf(log_mix, "%d,%c,%f,%d\n", time(NULL),
				toupper(transaction_short_name[client_data.transaction]),
				response_time, pthread_self());
		}
		else if (rc == ERROR)
		{
			fprintf(log_mix, "%d,%c,%f,%d\n", time(NULL),
				'E', response_time, pthread_self());
		}
		fflush(log_mix);
		pthread_mutex_unlock(&mutex_mix_log);
		pthread_mutex_lock(&mutex_terminal_state[EXECUTING][client_data.transaction]);
		--terminal_state[EXECUTING][client_data.transaction];
		pthread_mutex_unlock(&mutex_terminal_state[EXECUTING][client_data.transaction]);

		/* Thinking time... */
		pthread_mutex_lock(&mutex_terminal_state[THINKING][client_data.transaction]);
		++terminal_state[THINKING][client_data.transaction];
		pthread_mutex_unlock(&mutex_terminal_state[THINKING][client_data.transaction]);
		if (time(NULL) < stop_time)
		{
			thinking_time.tv_nsec = (long) get_think_time(mean_think_time);
			thinking_time.tv_sec = (time_t) (thinking_time.tv_nsec / 1000);
			thinking_time.tv_nsec = (thinking_time.tv_nsec % 1000) * 1000000;
			while (nanosleep(&thinking_time, &rem) == -1)
			{
				if (errno == EINTR)
				{
					memcpy(&thinking_time, &rem, sizeof (struct timespec));
				}
				else
				{
					LOG_ERROR_MESSAGE("sleep time invalid %d s %ls ns",
						thinking_time.tv_sec, thinking_time.tv_nsec);
					break;
				}
			}
		}
		pthread_mutex_lock(&mutex_terminal_state[THINKING][client_data.transaction]);
		--terminal_state[THINKING][client_data.transaction];
		pthread_mutex_unlock(&mutex_terminal_state[THINKING][client_data.transaction]);
	}
	while (time(NULL) < stop_time);

#ifdef STANDALONE
	/*recycle_node(node);*/
#endif /* STANDALONE */

	LOG_ERROR_MESSAGE("exiting normally...");

	/* Note when each thread has exited. */
	pthread_mutex_lock(&mutex_mix_log);
	fprintf(log_mix, "TERMINATED %d\n", pthread_self());
	fflush(log_mix);
	pthread_mutex_unlock(&mutex_mix_log);

	/* Update the terminal count. */
	sem_wait(&terminal_count);

	return NULL;	/* keep the compiler quiet */
}
