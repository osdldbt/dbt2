/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 */

#ifndef _LISTENER_H_
#define _LISTENER_H_

#include <_semaphore.h>

#include <transaction_queue.h>

void *init_listener(void *data);

extern sem_t listener_worker_count;

int recycle_node(struct transaction_queue_node_t *node);

#endif /* _LISTENER_H_ */
