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

char *transaction_name[TRANSACTION_MAX] =
	{ "delivery    ",
	  "new-order   ",
	  "order-status",
	  "payment     ",
	  "stock-level "
	};

int main(int argc, char *argv[])
{
	int i;
	FILE *log_mix;
	FILE *log_tps;
	int sample_length = 30;
	int total_transaction_count = 0;
	int total_new_order_count = 0;
	int current_transaction_count = 0;
	double response_time, total_response_time = 0;
	time_t start_time = -1;
	time_t previous_time, current_time;
	char transaction;
	char marker[64];
	int tid;
	int elapsed_time;
	double tps;

	int transaction_count[TRANSACTION_MAX] = { 0, 0, 0, 0, 0 };
	double transaction_response_time[TRANSACTION_MAX] = { 0, 0, 0, 0, 0 };

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

	while (fscanf(log_mix, "%d,%c,%lf,%d", &current_time, &transaction,
		&response_time, &tid) == 4)
	{
		/* Note when the rampup has ended. */
		if (start_time == -1)
		{
			start_time = current_time;
			previous_time = current_time;
		}

		total_response_time += response_time;
		++total_transaction_count;
		
		if (transaction == 'n')
		{
			++transaction_count[NEW_ORDER];
			transaction_response_time[NEW_ORDER] += response_time;
			++total_new_order_count;
		}
		else if (transaction == 'p')
		{
			++transaction_count[PAYMENT];
			transaction_response_time[PAYMENT] += response_time;
		}
		else if (transaction == 'o')
		{
			++transaction_count[ORDER_STATUS];
			transaction_response_time[ORDER_STATUS] += response_time;
		}
		else if (transaction == 'd')
		{
			++transaction_count[DELIVERY];
			transaction_response_time[DELIVERY] += response_time;
		}
		else if (transaction == 's')
		{
			++transaction_count[STOCK_LEVEL];
			transaction_response_time[STOCK_LEVEL] += response_time;
		}
		else
		{
			printf("unknown transaction, continuing\n");
			continue;
		}

		/* Output data to csv file for charting transaction per second. */
		if (current_time <= previous_time + sample_length)
		{
			++current_transaction_count;
		}
		else
		{
			fprintf(log_tps, "%d,%f\n", elapsed_time,
				(double) current_transaction_count / 30.0);
				elapsed_time += sample_length;

			previous_time = current_time;
			current_transaction_count = 1;
		}
	}
	fclose(log_mix);
	fclose(log_tps);

	/* Calculate the actual mix of transactions. */
	printf("transaction\t%\tavg response time (s)\n");
	for (i = 0; i < TRANSACTION_MAX; i++)
	{
		printf("%s\t%2.2f\t%0.3f\n", transaction_name[i],
			(double) transaction_count[i] / (double) total_transaction_count * 100.0,
			transaction_response_time[i] / (double) transaction_count[i]);
	}

	/* Calculated the number of transactions per second. */
	tps = (double) total_new_order_count / difftime(current_time, start_time);
	printf("\n%0.2f bogotransactions (type 2) per second\n", tps);
	printf("%0.1f minute duration\n",
		difftime(current_time, start_time) / 60.0);
	printf("%d total bogotransactions (type 2)\n", total_new_order_count);
	printf("\n");

	return 0;
}
