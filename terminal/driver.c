/*
 * driver.h
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
#include <time.h>
#include <errno.h>
#include <common.h>
#include <driver.h>

#define MIX_LOG_NAME "mix.log"

void *terminal_worker(void *data);

/* Global Variables */
struct transaction_mix_t transaction_mix;
char hostname[32];
int port = CLIENT_PORT;
int terminals = 0; /* The number of terminals to emulate. */
int duration = 0;
int stop_time;

FILE *log_mix;
pthread_mutex_t mutex_mix_log = PTHREAD_MUTEX_INITIALIZER;

int init_driver()
{
	transaction_mix.delivery_actual = MIX_DELIVERY;
	transaction_mix.order_status_actual = MIX_ORDER_STATUS;
	transaction_mix.payment_actual = MIX_PAYMENT;
	transaction_mix.stock_level_actual = MIX_STOCK_LEVEL;

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
		case TABLE_STOCK:
			table_cardinality.stock = cardinality;
			break;
		case TABLE_NEW_ORDER:
			table_cardinality.new_orders = cardinality;
			break;
		default:
			return ERROR;
	}

	return OK;
}

int set_terminals(int number)
{
	terminals = number;
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
	int i;
	pthread_t last_tid;

	printf("starting %d terminal(s)...\n", terminals);
	for (i = 0; i < terminals; i++)
	{
		pthread_t tid;

		if (pthread_create(&tid, NULL, &terminal_worker, NULL) != 0)
		{
			LOG_ERROR_MESSAGE("error creating terminal thread");
			return ERROR;
		}
		last_tid = tid;
	}
	printf("terminals started...\n", terminals);
	pthread_join (last_tid, NULL);

	return OK;
}

void *terminal_worker(void *data)
{
	int transaction;
	double threshold;
	int keying_time;
	struct timespec think_time, rem;
	int mean_think_time; /* In milliseconds. */
	struct timeval rt0, rt1;
	double response_time;
	extern int errno;

	while (time(NULL) < stop_time)
	{
		/* Determine which transaction to execute. */
		threshold = get_percentage();
		if (threshold < transaction_mix.new_order_threshold)
		{
			transaction = NEW_ORDER;
		}
		else if (threshold < transaction_mix.payment_threshold)
		{
			transaction = PAYMENT;
		}
		else if (threshold < transaction_mix.order_status_threshold)
		{
			transaction = ORDER_STATUS;
		}
		else if (threshold < transaction_mix.delivery_threshold)
		{
			transaction = DELIVERY;
		}
		else
		{
			transaction = STOCK_LEVEL;
		}

		/*
		 * Determine minimum keying time and mean think time based on the
		 * transaction.
		 */
		switch (transaction)
		{
			case DELIVERY:
				keying_time = 2;
				mean_think_time = 5000;
				break;
			case NEW_ORDER:
				keying_time = 18;
				mean_think_time = 12000;
				break;
			case ORDER_STATUS:
				keying_time = 2;
				mean_think_time = 10000;
				break;
			case PAYMENT:
				keying_time = 3;
				mean_think_time = 12000;
				break;
			case STOCK_LEVEL:
				keying_time = 2;
				mean_think_time = 5000;
				break;
		}

		/* Keying time... */
		sleep(keying_time);

		/* Execute transaction. */
		if (gettimeofday(&rt0, NULL) == -1)
		{
			perror("gettimeofday");
		}
		if (gettimeofday(&rt1, NULL) == -1)
		{
			perror("gettimeofday");
		}
		response_time = difftimeval(rt1, rt0);
		pthread_mutex_lock(&mutex_mix_log);
		fprintf(log_mix, "%d,%c,%f,%d\n", time(NULL),
			transaction_short_name[transaction], response_time,
			pthread_self());
		fflush(log_mix);
		pthread_mutex_unlock(&mutex_mix_log);

		/* Thinking time... */
		think_time.tv_nsec = (long) get_think_time(mean_think_time);
		think_time.tv_sec = (time_t) (think_time.tv_nsec / 1000);
		think_time.tv_nsec = (think_time.tv_nsec % 1000) * 1000000;
		while (nanosleep(&think_time, &rem) == -1)
		{
			if (errno == EINTR)
			{
				memcpy(&think_time, &rem, sizeof (struct timespec));
			}
			else
			{
				LOG_ERROR_MESSAGE("sleep time invalid %d s %ls ns",
					think_time.tv_sec, think_time.tv_nsec);
				break;
			}
		}
	}
}
