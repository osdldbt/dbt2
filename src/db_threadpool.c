/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Labs, Inc.
 *
 * 31 June 2002
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "common.h"
#include "listener.h"
#include "logging.h"
#include "client_interface.h"
#include "transaction_queue.h"
#include "db_threadpool.h"
#include "db.h"

/* Function Prototypes */
void *db_worker(void *data);
int startup();

/* Global Variables */
int db_connections = 0;
int db_conn_sleep = 1;
int *worker_count;
time_t *last_txn;

/* These should probably be handled differently. */
extern char sname[32];
extern int exiting;
sem_t db_worker_count;
#ifdef STANDALONE
extern FILE *log_mix;
extern pthread_mutex_t mutex_mix_log;
#endif /* STANDALONE */

#if defined(LIBMYSQL) || defined(ODBC)
extern char dbt2_user[128];
extern char dbt2_pass[128];
#endif

#ifdef LIBPQ
extern char postmaster_port[32];
#endif /* LIBPQ */

#ifdef LIBMYSQL
extern char dbt2_mysql_port[32];
extern char dbt2_mysql_host[128];
extern char dbt2_mysql_socket[256];
#endif /* LIBMYSQL */

void *db_worker(void *data)
{
        int id = *((int *) data); /* Whoa... */
#ifndef STANDALONE
        int length;
#endif /* NOT STANDALONE */
        struct transaction_queue_node_t *node;
        struct db_context_t dbc;
#ifdef STANDALONE
        struct timeval rt0, rt1;
        double response_time;
#endif /* STANDALONE */

        /* Open a connection to the database. */
#ifdef ODBC
        printf("connect to ODBC server with parameters: DSN: |%s| user: |%s| pass: |%s|\n", sname, dbt2_user, dbt2_pass);
        db_init(sname, dbt2_user, dbt2_pass);
#endif /* ODBC */
#ifdef LIBPQ
        db_init(DB_NAME, sname, postmaster_port);
#endif /* LIBPQ */

#ifdef LIBMYSQL
       printf("connect to mysql server with parameters: db_name: |%s| host: |%s| user: |%s| pass: |%s| port: |%s| socket: |%s|\n", sname, dbt2_mysql_host, dbt2_user, dbt2_pass, dbt2_mysql_port, dbt2_mysql_socket);
       db_init(sname, dbt2_mysql_host , dbt2_user, dbt2_pass, dbt2_mysql_port, dbt2_mysql_socket);
#endif /* LIBMYSQL */


        if (!exiting && connect_to_db(&dbc) != OK) {
                LOG_ERROR_MESSAGE("connect_to_db() error, terminating program");
		printf("cannot connect to database(see details in error.log file, exiting...\n");
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
#ifdef STANDALONE
                if (gettimeofday(&rt0, NULL) == -1) {
                        perror("gettimeofday");
                }
#endif /* STANDALONE */
                node->client_data.status =
                        process_transaction(node->client_data.transaction,
                        &dbc, &node->client_data.transaction_data);
                if (node->client_data.status == ERROR) {
                        LOG_ERROR_MESSAGE("process_transaction() error on %s",
                                transaction_name[
                                node->client_data.transaction]);
                        /*
                         * Assume this isn't a fatal error, send the results
                         * back, and try processing the next transaction.
                         */
                }
#ifdef STANDALONE
                if (gettimeofday(&rt1, NULL) == -1) {
                        perror("gettimeofday");
                }
                response_time = difftimeval(rt1, rt0);
                pthread_mutex_lock(&mutex_mix_log);
                fprintf(log_mix, "%d,%c,%f,%d\n", (int) time(NULL),
                transaction_short_name[node->client_data.transaction],
                        response_time, (int) pthread_self());
                        fflush(log_mix);
                pthread_mutex_unlock(&mutex_mix_log);
#endif /* STANDALONE */

#ifndef STANDALONE
                length = send_transaction_data(node->s, &node->client_data);
                if (length == ERROR) {
                        LOG_ERROR_MESSAGE("send_transaction_data() error");
                        /*
                         * Assume this isn't a fatal error and try processing
                         * the next transaction.
                         */
                }
#endif /* NOT STANDALONE */
                pthread_mutex_lock(&mutex_transaction_counter[REQ_EXECUTING][
                        node->client_data.transaction]);
                --transaction_counter[REQ_EXECUTING][
                        node->client_data.transaction];
                pthread_mutex_unlock(&mutex_transaction_counter[REQ_EXECUTING][
                        node->client_data.transaction]);
                recycle_node(node);

                /* Keep track of how many transactions this thread has done. */
                ++worker_count[id];

                /* Keep track of then the last transaction was execute. */
                time(&last_txn[id]);
        }

        /* Disconnect from the database. */
        disconnect_from_db(&dbc);

        sem_wait(&db_worker_count);

        return NULL;        /* keep compiler quiet */
}

int db_threadpool_init()
{
        int i;
        if (sem_init(&db_worker_count, 0, 0) != 0) {
                LOG_ERROR_MESSAGE("cannot init db_worker_count\n");
                return ERROR;
        }

        worker_count = (int *) malloc(sizeof(int) * db_connections);
        bzero(worker_count, sizeof(int) * db_connections);

        last_txn = (time_t *) malloc(sizeof(time_t) * db_connections);

        for (i = 0; i < db_connections; i++) {
                pthread_t tid;

                /*
                 * Initialize the last_txn array with the time right before
                 * the worker thread is started.  This looks better than
                 * initializing the array with zeros.
                 */
                time(&last_txn[i]);

                /*
                 * Is it possible for i to change before db_worker can copy it?
                 */
                if (pthread_create(&tid, NULL, &db_worker, &i) != 0) {
                        LOG_ERROR_MESSAGE("error creating db thread");
                        return ERROR;
                }

                /* Keep a count of how many DB worker threads have started. */
                sem_post(&db_worker_count);

                /* Don't let the database connection attempts occur too fast. */
#ifdef DELAY_IN_MILISECONDS
          	usleep(db_conn_sleep*1000);
#else
                sleep(db_conn_sleep);
#endif
        }
        return OK;
}
