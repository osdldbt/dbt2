/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 *
 * 19 August 2002
 */

#include <common.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "logging.h"
#include "transaction_data.h"

FILE *log_error;
pthread_mutex_t mutex_error_log = PTHREAD_MUTEX_INITIALIZER;

int edump(int type, void *data) {
	pthread_mutex_lock(&mutex_error_log);
	fprintf(log_error, "[%lx]\n", pthread_self());
	dump(log_error, type, data);
	pthread_mutex_unlock(&mutex_error_log);

	return OK;
}

/* Open a file to log errors to. */
int init_logging() {
	char log_filename[512];
	log_filename[511] = '\0';
	snprintf(log_filename, 511, "%s/%s", output_path, ERROR_LOG_NAME);
	log_error = fopen(log_filename, "w");
	if (log_error == NULL) {
		fprintf(stderr, "cannot open %s\n", log_filename);
		return ERROR;
	}

	return OK;
}

/* Open a file to log errors to, for multi process use. */
int init_logging_f() {
	char log_filename[512];
	log_filename[511] = '\0';
	snprintf(log_filename, 511, "%s/error-%d.log", output_path, getpid());
	log_error = fopen(log_filename, "w");
	if (log_error == NULL) {
		fprintf(stderr, "cannot open %s\n", log_filename);
		return ERROR;
	}

	return OK;
}

int log_error_message(char *filename, int line, const char *fmt, ...) {
	va_list fmtargs;
	time_t t;
	struct tm *tmp;
	FILE *of = (log_error) ? log_error : stderr;
	char outstr[32];

	t = time(NULL);
	tmp = localtime(&t);
	strftime(outstr, sizeof(outstr), "%Y-%m-%d %T %Z", tmp);

	pthread_mutex_lock(&mutex_error_log);
	fprintf(of, "%s tid:%lx %s:%d\n", outstr, pthread_self(), filename, line);
	va_start(fmtargs, fmt);
	vfprintf(of, fmt, fmtargs);
	va_end(fmtargs);
	fprintf(of, "\n");
	fflush(log_error);
	pthread_mutex_unlock(&mutex_error_log);

	return OK;
}
