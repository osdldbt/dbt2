/*
 * results.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
 *
 * 19 august 2002
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <common.h>

int main(int argc, char *argv[])
{
	FILE *log_mix;
	FILE *log_tps;
	int sample_length;
	int total_transaction_count;
	double response_time, total_response_time;
	time_t start_time = -1;
	time_t current_time;
	char transaction;
	char marker[64];
	int tid;

	if (argc < 3)
	{
		printf("Usage: %s <filename> <sample>\n", argv[0]);
		return 1;
	}

	/* Attempt to open the file. */
	log_mix = fopen(argv[1], "r");
	if (log_mix == NULL)
	{
		printf("cannot open %s\n", argv[1]);
		return 2;
	}

	/* Open file to output data. */
	log_tps = fopen("tps.csv", "w");
	if (log_tps == NULL)
	{
		printf("cannot open tps.csv\n");
		return 3;
	}

	while (fscanf(log_mix, "%s", marker))
	{
		if (strcmp(marker, "START") == 0)
		{
			break;
		}
	}

	while (fscanf(log_mix, "%d,%c,%f,%d", &current_time, &transaction,
		&response_time, &tid) == 4)
	{
		/* Note when the rampup has ended. */
		if (start_time == -1)
		{
			start_time = current_time;
		}

		total_response_time += response_time;
		++total_transaction_count;
		
		if (transaction == 'n')
		{
		}
		else if (transaction == 'p')
		{
		}
		else if (transaction == 'o')
		{
		}
		else if (transaction == 'd')
		{
		}
		else if (transaction == 's')
		{
		}
		else
		{
			printf("unknown transaction\n");
			return 4;
		}
	}

	return 0;
}
