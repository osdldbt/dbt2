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
	int i;
	FILE *log_mix;
	FILE *log_tps;
	/*
	FILE *log_dtps;
	FILE *log_otps;
	FILE *log_ptps;
	FILE *log_stps;
	*/
	int sample_length;
	int total_transaction_count = 0;
	int current_transaction_count[TRANSACTION_MAX] = { 0, 0, 0, 0, 0 };
	double response_time, total_response_time = 0;
	time_t start_time = -1;
	time_t previous_time = 0; /* Initialized to remove compiler warning. */
	time_t current_time;
	char transaction;
	char marker[64];
	int tid;
	int elapsed_time = 0;
	double tps;
	char filename[256];
	/*
	char filename_d[256];
	char filename_o[256];
	char filename_p[256];
	char filename_s[256];
	*/

	int transaction_count[TRANSACTION_MAX] = { 0, 0, 0, 0, 0 };
	double transaction_response_time[TRANSACTION_MAX] = { 0, 0, 0, 0, 0 };

	if (argc < 3)
	{
		printf("Usage: %s <filename> <sample>\n", argv[0]);
		return 1;
	}

	sample_length = atoi(argv[2]);

	/* Attempt to open the file. */
	log_mix = fopen(argv[1], "r");
	if (log_mix == NULL)
	{
		printf("cannot open %s\n", argv[1]);
		return 2;
	}

	/* Open file to output data. */
	if (argc == 4)
	{
		sprintf(filename, "%s/tps.csv", argv[3]);
		/*
		sprintf(filename_d, "%s/tps_d.csv", argv[3]);
		sprintf(filename_o, "%s/tps_o.csv", argv[3]);
		sprintf(filename_p, "%s/tps_p.csv", argv[3]);
		sprintf(filename_s, "%s/tps_s.csv", argv[3]);
		*/
	}
	else
	{
		strcpy(filename, "tps.csv");
		/*
		strcpy(filename, "tps_d.csv");
		strcpy(filename, "tps_o.csv");
		strcpy(filename, "tps_p.csv");
		strcpy(filename, "tps_s.csv");
		*/
	}
	log_tps = fopen(filename, "w");
	if (log_tps == NULL)
	{
		printf("cannot open tps.csv\n");
		return 3;
	}
/*
	log_dtps = fopen(filename, "w");
	if (log_dtps == NULL)
	{
		printf("cannot open tps.csv\n");
		return 3;
	}
	log_otps = fopen(filename_o, "w");
	if (log_otps == NULL)
	{
		printf("cannot open tps_o.csv\n");
		return 3;
	}
	log_ptps = fopen(filename_p, "w");
	if (log_ptps == NULL)
	{
		printf("cannot open tps_p.csv\n");
		return 3;
	}
	log_stps = fopen(filename_s, "w");
	if (log_stps == NULL)
	{
		printf("cannot open tps_s.csv\n");
		return 3;
	}
*/

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
		int i;

		/* Note when the rampup has ended. */
		if (start_time == -1)
		{
			start_time = current_time;
			previous_time = current_time;
		}

		/* Output data to csv file for charting transaction per second. */
		if (current_time > previous_time + sample_length)
		{
			fprintf(log_tps, "%d\t%f\t%f\n", elapsed_time,
				(double) current_transaction_count[NEW_ORDER] / sample_length,
				((double) current_transaction_count[NEW_ORDER] / sample_length) * 60.0);
/*
			fprintf(log_dtps, "%d,%f\n", elapsed_time,
				(double) current_transaction_count[DELIVERY] / sample_length);
			fprintf(log_otps, "%d,%f\n", elapsed_time,
				(double) current_transaction_count[ORDER_STATUS] / sample_length);
			fprintf(log_ptps, "%d,%f\n", elapsed_time,
				(double) current_transaction_count[PAYMENT] / sample_length);
			fprintf(log_stps, "%d,%f\n", elapsed_time,
				(double) current_transaction_count[STOCK_LEVEL] / sample_length);
*/

			elapsed_time += sample_length;
			previous_time = current_time;

			for (i = 0; i < TRANSACTION_MAX; i++)
			{
				current_transaction_count[i] = 0;
			}
		}

		total_response_time += response_time;
		++total_transaction_count;
		
		if (transaction == 'n')
		{
			++transaction_count[NEW_ORDER];
			transaction_response_time[NEW_ORDER] += response_time;
			++current_transaction_count[NEW_ORDER];
		}
		else if (transaction == 'p')
		{
			++transaction_count[PAYMENT];
			transaction_response_time[PAYMENT] += response_time;
			++current_transaction_count[PAYMENT];
		}
		else if (transaction == 'o')
		{
			++transaction_count[ORDER_STATUS];
			transaction_response_time[ORDER_STATUS] += response_time;
			++current_transaction_count[ORDER_STATUS];
		}
		else if (transaction == 'd')
		{
			++transaction_count[DELIVERY];
			transaction_response_time[DELIVERY] += response_time;
			++current_transaction_count[DELIVERY];
		}
		else if (transaction == 's')
		{
			++transaction_count[STOCK_LEVEL];
			transaction_response_time[STOCK_LEVEL] += response_time;
			++current_transaction_count[STOCK_LEVEL];
		}
		else
		{
			printf("unknown transaction, continuing\n");
			continue;
		}
	}
	fclose(log_mix);
	fclose(log_tps);
/*
	fclose(log_dtps);
	fclose(log_otps);
	fclose(log_ptps);
	fclose(log_stps);
*/

	/* Calculate the actual mix of transactions. */
	printf("transaction\t%%\tavg response time (s)\n");
	for (i = 0; i < TRANSACTION_MAX; i++)
	{
		printf("%s\t%2.2f\t%0.3f\n", transaction_name[i],
			(double) transaction_count[i] / (double) total_transaction_count * 100.0,
			transaction_response_time[i] / (double) transaction_count[i]);
	}

	/* Calculated the number of transactions per second. */
	tps = (double) transaction_count[NEW_ORDER] / difftime(current_time, start_time);
	printf("\n%0.2f bogotransactions (type 2) per second\n", tps);
	printf("%0.1f minute duration\n",
		difftime(current_time, start_time) / 60.0);
	printf("%d total bogotransactions (type 2)\n",
		transaction_count[NEW_ORDER]);
	printf("\n");

	return 0;
}
