/*
 * db_threadpool.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
 *
 * 25 june 2002
 */

#include <pthread.h>
#include <semaphore.h>

extern sem_t db_worker_count;
