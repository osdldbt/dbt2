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
#include <semaphore.h>
#include <common.h>

void *listener_worker(void *data);

sem_t listener_worker_count;

/* Can I come up with a neater way to share the global variables? */
extern int exiting;

void *init_listener(void *data)
{
	int *s = (int *) data; /* Listener socket. */
	int sockfd;

	if (sem_init(&listener_worker_count, 0, 0) != 0)
	{
		LOG_ERROR_MESSAGE("cannot init listener_worker_count\n");
		printf("cannot init listener_worker_count, exiting...\n");
		exit(11);
	}

	while (!exiting)
	{
		pthread_t tid;

		sockfd = _accept(s);
		if (sockfd == -1)
		{
			LOG_ERROR_MESSAGE("_accept() failed, trying again...\n");
			continue;
		}
		if (pthread_create(&tid, NULL, &listener_worker, &sockfd) != 0)
		{
			LOG_ERROR_MESSAGE("pthread_create() failed, closing socket and waiting for a new request\n");
			close(sockfd);
			continue;
		}
		sem_post(&listener_worker_count);
	}
}

void *listener_worker(void *data)
{
	int *s = (int *) data;

	while (!exiting)
	{
	}
}
