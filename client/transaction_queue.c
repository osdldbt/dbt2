/*
 * transaction_queue.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
 *
 * 5 august 2002
 */

#include <pthread.h>
#include <common.h>
#include <transaction_queue.h>

sem_t queue_length;
/* Add to the tail, take off the head. */
struct transaction_queue_node_t *transaction_head, *transaction_tail;
pthread_mutex_t mutex_queue = PTHREAD_MUTEX_INITIALIZER;

struct transaction_queue_node_t *dequeue_transaction()
{
	struct transaction_queue_node_t *node;

	sem_wait(&queue_length);
	pthread_mutex_lock(&mutex_queue);
	node = transaction_head;
	if (transaction_head == NULL)
	{
		return NULL;
	}
	else if (transaction_head->next == NULL)
	{
		transaction_head = transaction_tail = NULL;
	}
	pthread_mutex_unlock(&mutex_queue);

	return node;
}

int enqueue_transaction(struct transaction_queue_node_t *node)
{
	pthread_mutex_lock(&mutex_queue);
	node->next = NULL;
	if (transaction_tail != NULL)
	{
		transaction_tail->next = node;
		transaction_tail = node;
	}
	else
	{
		transaction_head = transaction_tail = node;
	}
	sem_post(&queue_length);
	pthread_mutex_unlock(&mutex_queue);

	return OK;
}

int init_transaction_queue()
{
	transaction_head = transaction_tail = NULL;
	if (sem_init(&queue_length, 0, 0) != 0)
	{
		LOG_ERROR_MESSAGE("cannot init queue_length\n");
		return ERROR;
	}

	return OK;
}
