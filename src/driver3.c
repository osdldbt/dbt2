/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002      Open Source Development Labs, Inc.
 *               2002-2022 Mark Wong
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <stddef.h>
#include <sched.h>
#include <sys/time.h>
#include <sys/sysinfo.h>
#include <sys/wait.h>

#include <ev.h>

#include "common.h"
#include "logging.h"
#include "driver.h"
#include "client_interface.h"
#include "input_data_generator.h"
#include "db.h"
#include "client.h"

#include "entropy.h"

#define MIX_LOG_NAME "mix.log"

/* Global Variables */
static pcg64f_random_t rng;

extern int client_port;
char hostname[32];

int client_conn_sleep = 100; /* milliseconds */
int duration = 0;
int fork_per_processor = 1;
int mode_altered = 0;
int spread = 1;
int stop_time = 0;
int terminals_limit = 0; /* Not used by this driver. */
int w_id_min = 0, w_id_max = 0;

FILE *log_mix = NULL;

struct rte_events
{
	/*
	 * Set up 3 callbacks, in order of execution:
	 * 1: Start the transaction after thinking, or immediately for the 1st time.
	 *    (tt)
	 * 2: Initiate the transaction with the client, after keying time. (kt)
	 * 3: Receive data from the client, and "think" before starting next
	 *    transaction. (recvtn)
	 */
	ev_timer kt;
	ev_timer tt;

	int w_id;
	int d_id;
	struct timeval rt0, rt1;
	int mean_think_time; /* In milliseconds. */
	struct client_transaction_t client_data;
};

static struct rte_events *rte;
struct db_context_t dbc;

int init_driver_logging()
{
	char log_filename[512];

	log_filename[511] = '\0';
	snprintf(log_filename, 511, "%s/mix-%d.log", output_path, getpid());
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

	entropy_getbytes((void *) &local_seed, sizeof(local_seed));
	pcg64f_srandom_r(&rng, local_seed);

	/* Connect to the client program. */
	sockfd = connect_to_client(hostname, client_port);
	if (sockfd < 1) {
		printf("connect_to_client() failed, exiting...\n");
		fflush(stdout);
		LOG_ERROR_MESSAGE("connect_to_client() failed, thread exiting...");
	}

	client_data.transaction = INTEGRITY;
	generate_input_data(&rng, client_data.transaction,
			&client_data.transaction_data, table_cardinality.warehouses);

	send_transaction_data(sockfd, &client_data);
	receive_transaction_data(sockfd, &client_data);
	close(sockfd);

	return client_data.status;
}

/* Callback once keying time has passed, send transction request to client. */
static void keying_time_cb(struct ev_loop *loop, ev_timer *w, int revents)
{
	struct rte_events *w0 = (struct rte_events *)
			((char *) w - offsetof (struct rte_events, kt));
	char code;
	time_t tt;
	double response_time;

	ev_timer_stop(loop, w);
	if (gettimeofday(&w0->rt0, NULL) == -1)
		perror("gettimeofday");

	w0->client_data.status = process_transaction(w0->client_data.transaction,
			&dbc, &w0->client_data.transaction_data);

	if (gettimeofday(&w0->rt1, NULL) == -1)
		perror("gettimeofday");
	response_time = difftimeval(w0->rt1, w0->rt0);

	switch (w0->client_data.status) {
	case OK:
		code = 'C';
		break;
	case STATUS_ROLLBACK:
		code = 'R';
		break;
	case ERROR:
		code = 'E';
		break;
	default:
		code = 'X';
		break;
	}
	tt = time(NULL);
	fprintf(log_mix, "%ld,%c,%c,%f,%d,%d,%d\n",
			tt, transaction_short_name[w0->client_data.transaction], code,
			response_time, getpid(), w0->w_id, w0->d_id);

	if (stop_time == 0 || tt < stop_time) {
		ev_timer_set(&w0->tt, w0->mean_think_time / 1000, 0);
		ev_timer_start(loop, &w0->tt);
	}
}

/*
 * Callback once thinking time has passed, or to start the application
 * transaction for the first time.
 */
static void thinking_time_cb (struct ev_loop *loop, ev_timer *w, int revents)
{
	struct rte_events *w0 = (struct rte_events *)
			((char *) w - offsetof (struct rte_events, tt));
	double threshold;
	int keying_time;

	ev_timer_stop(loop, w);

	if (mode_altered == 1) {
		/*
		 * Determine w_id and d_id for the client per transaction.
		 * TODO: Find an efficient way to make sure more than one thread isn't
		 * executing with the same warehouse and district as another connection.
		 */
		w0->w_id = w_id_min + (int) get_random(&rng, w_id_max - w_id_min + 1);
		w0->d_id = (int) get_random(&rng, table_cardinality.districts) + 1;
	}

	threshold = get_percentage(&rng);
	if (threshold < transaction_mix.new_order_threshold) {
		w0->client_data.transaction = NEW_ORDER;
		keying_time = key_time.new_order;
		w0->mean_think_time = think_time.new_order;
	} else if (transaction_mix.payment_actual != 0 &&
			threshold < transaction_mix.payment_threshold) {
		w0->client_data.transaction = PAYMENT;
		keying_time = key_time.payment;
		w0->mean_think_time = think_time.payment;
	} else if (transaction_mix.order_status_actual != 0 &&
			threshold < transaction_mix.order_status_threshold) {
		w0->client_data.transaction = ORDER_STATUS;
		keying_time = key_time.order_status;
		w0->mean_think_time = think_time.order_status;
	} else if (transaction_mix.delivery_actual != 0 &&
			threshold < transaction_mix.delivery_threshold) {
		w0->client_data.transaction = DELIVERY;
		keying_time = key_time.delivery;
		w0->mean_think_time = think_time.delivery;
	} else {
		w0->client_data.transaction = STOCK_LEVEL;
		keying_time = key_time.stock_level;
		w0->mean_think_time = think_time.stock_level;
	}

	/* Generate the input data for the transaction. */
	if (w0->client_data.transaction != STOCK_LEVEL) {
		generate_input_data(&rng, w0->client_data.transaction,
				&w0->client_data.transaction_data, w0->w_id);
	} else {
		generate_input_data2(&rng, w0->client_data.transaction,
				&w0->client_data.transaction_data, w0->w_id, w0->d_id);
	}

	ev_timer_set(&w0->kt, keying_time, 0);
	ev_timer_start(loop, &w0->kt);
}

int start_driver()
{
	int rc;
	struct ev_loop* loop;

	int i, j, k, l;
	unsigned long long local_seed = 0;
	struct timespec ts, rem;

	int nprocs = get_nprocs();
	cpu_set_t set;

	int number_of_warehouses = w_id_max - w_id_min + 1;
	int max_fork = (nprocs * fork_per_processor) > number_of_warehouses ?
			number_of_warehouses : nprocs * fork_per_processor;
	double partition_size = (double) number_of_warehouses / (double) max_fork;
	int mymin, mymax;

	int start_time;

	/* The time to sleep between opening client connections. */
	ts.tv_sec = (time_t) (client_conn_sleep / 1000);
	ts.tv_nsec = (long) (client_conn_sleep - (ts.tv_sec * 1000)) * 1000;

	CPU_ZERO(&set);

	CPU_SET((max_fork - 1) % nprocs, &set);
	if (sched_setaffinity(0, sizeof(set), &set) == -1) {
		perror("sched_setaffinity");
		return 1;
	}
	printf("[%d] pinned to processor %d.\n", getpid(), (max_fork - 1) % nprocs);
	fflush(stdout);

	/* Caulculate when the test should stop. */
	start_time = (int) ((double) client_conn_sleep / 1000.0 *
			(double) (max_fork - 1));
	stop_time = time(NULL) + duration + start_time;
	printf("driver is starting to ramp up at time %d\n", (int) time(NULL));
	printf("driver will ramp up in %d seconds\n", start_time);
	printf("will stop test at time %d\n\n", stop_time);

	printf("%d out of %d processors available\n", nprocs, get_nprocs_conf());
	printf("starting %d driver process(es) "
			"(1 database connection per process)\n", max_fork);
	printf("each fork will cover %f warehouse(s)\n\n", partition_size);
	fflush(stdout);

	for (l = 0; l < max_fork; l++) {
		/*
		 * Calculate the number of warehouses in the current partition so that
		 * parent knows how long to sleep before the next fork, and so the child
		 * knows its own warehouse coverage.
		 */
		mymin = w_id_min + (int) ((double) l * partition_size);
		if (l != (max_fork - 1))
			mymax = w_id_min + (int) ((double) (l + 1) * partition_size) - 1;
		else
			mymax = w_id_max;

		/* Fall back to the parent process on the last partition. */
		if (l == (max_fork - 1))
			break;

		rc = fork();
		if (rc == 0) {
			int cpu = l % nprocs;
			CPU_SET(cpu, &set);
			if (sched_setaffinity(0, sizeof(set), &set) == -1) {
				perror("sched_setaffinity");
				return 1;
			}
			printf("[%d] pinned to processor %d.\n", getpid(), cpu);
			fflush(stdout);
			break;
		} else if (rc == -1) {
			perror("fork");
			return 1;
		} else {
			/* The time to sleep between forking next process. */
			struct timespec ts0, rem0;
			ts0.tv_sec = ts.tv_sec;
			ts0.tv_nsec = ts.tv_nsec;

			while (nanosleep(&ts0, &rem0) == -1) {
				if (errno == EINTR) {
					memcpy(&ts, &rem, sizeof(struct timespec));
				} else {
					LOG_ERROR_MESSAGE(
							"sleep time invalid %ld s %ld ns",
							ts.tv_sec, ts.tv_nsec);
				}
			}
		}
	}

	if (init_logging_f() != OK) {
		printf("cannot open error log\n");
		return 1;
	};

	if (init_driver_logging() != OK) {
		printf("cannot open mix log\n");
		return 1;
	};

	entropy_getbytes((void *) &local_seed, sizeof(local_seed));
	printf("[%d] seed %llu\n", getpid(), local_seed);
	fflush(stdout);
	pcg64f_srandom_r(&rng, local_seed);

	loop = EV_DEFAULT;

	printf("[%d] assigned part %d: warehouses %d to %d.\n", getpid(), l, mymin,
			mymax);
	fflush(stdout);

	if (init_dbc(&dbc) != 0) {
		printf("[%d] cannot initialize database settings, exiting...\n",
				getpid());
		return 99;
	}
	if (connect_to_db(&dbc) != OK) {
		printf("[%d] cannot connect to database, exiting...\n", getpid());
		return 99;
	}

	rte = malloc(sizeof(struct rte_events) * (mymax - mymin + 1) *
			terminals_per_warehouse);
	if (rte == NULL) {
		printf("error allocating space for rte\n");
		return ERROR;
	}
	memset(rte, 0, sizeof(struct rte_events) * (mymax - mymin + 1) *
			terminals_per_warehouse);

	k = 0;
	for (j = 0; j < terminals_per_warehouse; j++) {
		for (i = mymin; i < mymax + 1; i += spread) {
			/* Set this terminal's home warehouse and district .*/
			rte[k].w_id = i;
			rte[k].d_id = j + 1;

			/* Initialize all the events for this terminal. */
			ev_timer_init((struct ev_timer *) &rte[k].tt,
					thinking_time_cb, 0, 0);
			ev_timer_init((struct ev_timer *) &rte[k].kt, keying_time_cb, 0, 0);

			ev_timer_start(loop, (struct ev_timer *) &rte[k].tt);

			++k;
		}
	}

	fprintf(log_mix, "%ld,START,,,%d,,\n", time(NULL), getpid());

	printf("[%d] terminals started\n", getpid());
	fflush(stdout);

	ev_run(loop, 0);
	fprintf(log_mix, "%ld,TERMINATED,,,%d,,\n", time(NULL), getpid());

	free(rte);

	disconnect_from_db(&dbc);

	if (rc == 0)
		wait(NULL);
	printf("[%d] driver is exiting normally\n", getpid());
	return OK;
}
