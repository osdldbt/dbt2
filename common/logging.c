/*
 * logging.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
 *
 * 19 august 2002
 */

#include <pthread.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <common.h>
#include <logging.h>
#include <transaction_data.h>

FILE *log_error;
pthread_mutex_t mutex_error_log = PTHREAD_MUTEX_INITIALIZER;

int edump(int type, void *data)
{
	pthread_mutex_lock(&mutex_error_log);
	fprintf(log_error, "[%d]\n", pthread_self());
	dump(log_error, type, data);
	pthread_mutex_unlock(&mutex_error_log);

	return OK;
}

int init_logging()
{
	/* Open a file to log errors to. */
	log_error = fopen(ERROR_LOG_NAME, "w");
	if (log_error == NULL)
	{
		fprintf(stderr, "cannot open %s\n", ERROR_LOG_NAME);
		return ERROR;
	}

	return OK;
}

int log_error_message(char *filename, int line, const char *fmt, ...)
{
	va_list fmtargs;
	time_t t;
	FILE *of = (log_error) ? log_error: stderr;

	/* Print the error message(s) */
	t = time(NULL);
	va_start(fmtargs, fmt);

	pthread_mutex_lock(&mutex_error_log);
	fprintf(of, "%stid:%d %s:%d\n", ctime(&t), pthread_self(), filename, line);
	va_start(fmtargs, fmt);
	vfprintf(of, fmt, fmtargs);
	va_end(fmtargs);
	fprintf(of, "\n");
	fflush(log_error);
	pthread_mutex_unlock(&mutex_error_log);

	return OK;
}
