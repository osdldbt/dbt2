/*
 * listener.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
 *
 * 31 june 2002
 */

#ifndef _LISTENER_H_
#define _LISTERNE_H_

#include <semaphore.h>

void *init_listener(void *data);

extern sem_t listener_worker_count;

#endif /* _LISTENER_H_ */
