/*
 * listener.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
 *
 * 31 june 2002
 */

#include <pthread.h>
#include <common.h>
#include <logging.h>
#include <listener.h>
#include <client_interface.h>

void *listener_worker(void *data);

sem_t listener_worker_count;
sem_t free_count;
struct transaction_queue_node_t *node_head, *node_tail;
pthread_mutex_t mutex_node = PTHREAD_MUTEX_INITIALIZER;

/* Can I come up with a neater way to share the global variables? */
extern int exiting;

struct transaction_queue_node_t *get_node()
{
	struct transaction_queue_node_t *node;

	sem_wait(&free_count);
	pthread_mutex_lock(&mutex_node);
	node = node_head;
	if (node_head == NULL)
	{
		return NULL;
	}
	else if (node_head->next == NULL)
	{
		node_head = node_tail = NULL;
	}
	pthread_mutex_unlock(&mutex_node);

	return node;
}

void *init_listener(void *data)
{
	int *s = (int *) data; /* Listener socket. */
	node_head = node_tail = NULL;

	if (sem_init(&listener_worker_count, 0, 0) != 0)
	{
		LOG_ERROR_MESSAGE("cannot init listener_worker_count\n");
		printf("cannot init listener_worker_count, exiting...\n");
		exit(11);
	}
	if (sem_init(&free_count, 0, 0) != 0)
	{
		LOG_ERROR_MESSAGE("cannot init free_count");
		exit(12);
	}

	while (!exiting)
	{
		pthread_t tid;
		struct transaction_queue_node_t *node;

		/* Allocate some memory to pass to the new thread. */
		node = (struct transaction_queue_node_t *)
			malloc(sizeof(struct transaction_queue_node_t));

		node->s = _accept(s);
		if (node->s == -1)
		{
			LOG_ERROR_MESSAGE("_accept() failed, trying again...\n");
			continue;
		}

		if (pthread_create(&tid, NULL, &listener_worker, (void *) node) != 0)
		{
			LOG_ERROR_MESSAGE("pthread_create() failed, closing socket and waiting for a new request\n");
			close(node->s);
			continue;
		}
		sem_post(&listener_worker_count);
	}
}

void *listener_worker(void *data)
{
	int rc;
	struct transaction_queue_node_t *node =
		(struct transaction_queue_node_t *) data;

	while (!exiting)
	{
		rc = receive_transaction_data(node->s, &node->client_data);
		if (rc == ERROR_SOCKET_CLOSED)
		{
			LOG_ERROR_MESSAGE("exiting...");
			/* Exit the thread when the socket has closed. */
			sem_wait(&listener_worker_count);
			pthread_exit(0);
		}
		else if (rc != OK)
		{
			LOG_ERROR_MESSAGE("receive_transaction_data() error");
			continue;
		}

		/* Queue up the transaction data to be processed. */
		enqueue_transaction(node);
		node = get_node();
		if (node == NULL)
		{
			LOG_ERROR_MESSAGE("Cannot get a transaction node.\n");
		}
	}
}

int recycle_node(struct transaction_queue_node_t *node)
{
	pthread_mutex_lock(&mutex_node);
	node->next = NULL;
	if (node_tail != NULL)
	{
		node_tail->next = node;
		node_tail = node;
	}
	else
	{
		node_head = node_tail = node;
	}
	sem_post(&free_count);
	pthread_mutex_unlock(&mutex_node);

	return OK;
}
