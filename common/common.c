/*
 * common.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
 *
 * 16 may 2002
 * Based on TPC-C Standard Specification Revision 5.0.
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <common.h>

char a_string_char[A_STRING_CHAR_LEN];
const char *n_string_char = "0123456789";
const char *l_string_char =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

const char *c_last_syl[C_LAST_SYL_MAX] =
{
	"BAR", "OUGHT", "ABLE", "PRI", "PRES", "ESE", "ANTI", "CALLY", "ATION",
	"EING"
};

FILE *log_error;
pthread_mutex_t mutex_error_log = PTHREAD_MUTEX_INITIALIZER;

int w_id_max;

/* Clause 4.3.2.2.  */
void get_a_string(char *a_string, int x, int y)
{
	int length;
	int i;

	length = x + get_random((y - x)) + 1;
	a_string[length - 1] = '\0';

	for (i = 0; i < length - 1; i++)
	{
		a_string[i] = a_string_char[get_random(A_STRING_CHAR_LEN)];
	}

	return;
}

/* Clause 4.3.2.3 */
int get_c_last(char *c_last, int i)
{
	char tmp[4];

	c_last[0] = '\0';

	if (i < 0 || i > 999)
	{
		return ERROR;
	}

	/* Ensure the number is padded with leading 0's if it's less than 100. */
	sprintf(tmp, "%03d", i);

	strcat(c_last, c_last_syl[tmp[0] - '0']);
	strcat(c_last, c_last_syl[tmp[1] - '0']);
	strcat(c_last, c_last_syl[tmp[2] - '0']);
	return OK;
}

void get_l_string(char *a_string, int x, int y)
{
	int length;
	int i;

	length = x + get_random((y - x)) + 1;
	a_string[length - 1] = '\0';

	for (i = 0; i < length - 1; i++)
	{
		a_string[i] = l_string_char[get_random(L_STRING_CHAR_LEN)];
	}

	return;
}

/* Clause 4.3.2.2.  */
void get_n_string(char *n_string, int x, int y)
{
	int length;
	int i;

	length = x + get_random((long long) (y - x)) + 1;
	n_string[length - 1] = '\0';

	for (i = 0; i < length - 1; i++)
	{
		n_string[i] = n_string_char[get_random(N_STRING_CHAR_LEN)];
	}

	return;
}

/* Clause 2.1.6 */
int get_nurand(int a, int x, int y)
{
	return ((get_random(a + 1) | (x + get_random(y + 1))) % (y - x + 1)) + x;
}

/* Return a number from 0 to max - 1. */
double get_percentage()
{
	return (double) rand() / (double) RAND_MAX;
}

int get_random(int max)
{
	return (int) (get_percentage() * (double) max);
}

int init_common()
{
	int i, j;

	srand(1);

	/*
	 * Initialize a-string character set to 128 ascii characters.
	 * Clause 4.3.2.2.
	 */
	j = 0;
	a_string_char[j++] = (char) 33;
	for (i = 35; i <= 43; i++)
	{
		a_string_char[j++] = (char) i;
	}
	for (i = 45; i <= 126; i++)
	{
		a_string_char[j++] = (char) i;
	}
	for (i = 220; i <= 255; i++)
	{
		a_string_char[j++] = (char) i;
	}
	
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
