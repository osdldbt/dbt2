/*
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
#include <string.h>

int main(int argc, char *argv[])
{
	int i;
	FILE *log_mix;
	FILE *log_notpm;
	FILE *log_dtpm;
	FILE *log_ostpm;
	FILE *log_ptpm;
	FILE *log_sltpm;

	int sample_length;
	int total_transaction_count = 0;
	int current_transaction_count[TRANSACTION_MAX] = { 0, 0, 0, 0, 0 };
	int rollback_count[TRANSACTION_MAX] = { 0, 0, 0, 0, 0 };
	double response_time, total_response_time = 0;
	time_t start_time = -1;
	time_t previous_time = 0; /* Initialized to remove compiler warning. */
	time_t current_time;
	char transaction;
	char marker[64];
	int tid;
	int elapsed_time = 0;
	double tps;
	char no_filename[256];
	char d_filename[256];
	char os_filename[256];
	char p_filename[256];
	char sl_filename[256];

	int errors = 0; /* Unknown error counter. */

	int transaction_count[TRANSACTION_MAX] = { 0, 0, 0, 0, 0 };
	double transaction_response_time[TRANSACTION_MAX] = { 0, 0, 0, 0, 0 };

	if (argc < 3) {
		printf("Usage: %s <filename> <sample> [output_dir]\n", argv[0]);
		return 1;
	}

	sample_length = atoi(argv[2]);

	/* Attempt to open the file. */
	log_mix = fopen(argv[1], "r");
	if (log_mix == NULL) {
		printf("cannot open %s\n", argv[1]);
		return 2;
	}

	/* Open file to output data. */
	if (argc == 4) {
		sprintf(no_filename, "%s/notpm.csv", argv[3]);
		sprintf(d_filename, "%s/dtpm.csv", argv[3]);
		sprintf(os_filename, "%s/ostpm.csv", argv[3]);
		sprintf(p_filename, "%s/ptpm.csv", argv[3]);
		sprintf(sl_filename, "%s/sltpm.csv", argv[3]);
	} else {
		strcpy(no_filename, "notpm.csv");
		strcpy(d_filename, "dtpm.csv");
		strcpy(os_filename, "ostpm.csv");
		strcpy(p_filename, "ptpm.csv");
		strcpy(sl_filename, "sltpm.csv");
	}
	log_notpm = fopen(no_filename, "w");
	if (log_notpm == NULL) {
		printf("cannot open notpm.csv\n");
		return 3;
	}
	log_dtpm = fopen(d_filename, "w");
	if (log_dtpm == NULL) {
		printf("cannot open dtpm.csv\n");
		return 3;
	}
	log_ostpm = fopen(os_filename, "w");
	if (log_ostpm == NULL) {
		printf("cannot open ostpm.csv\n");
		return 3;
	}
	log_ptpm = fopen(p_filename, "w");
	if (log_ptpm == NULL) {
		printf("cannot open ptpm.csv\n");
		return 3;
	}
	log_sltpm = fopen(sl_filename, "w");
	if (log_sltpm == NULL) {
		printf("cannot open sltpm.csv\n");
		return 3;
	}

	while (fscanf(log_mix, "%s", marker)) {
		if (strcmp(marker, "START") == 0) {
			break;
		}
	}

	while (fscanf(log_mix, "%d,%c,%lf,%d", (int *) &current_time,
		&transaction, &response_time, &tid) == 4) {
		int i;

		/* Note when the rampup has ended. */
		if (start_time == -1) {
			start_time = current_time;
			previous_time = current_time;
		}

		/*
		 * Output data to csv file for charting transaction per second.
		 */
		if (current_time > previous_time + sample_length) {
			fprintf(log_notpm, "%d,%f\n", elapsed_time / 60,
				((double) current_transaction_count[NEW_ORDER] /
				sample_length) * 60.0);
			fprintf(log_dtpm, "%d,%f\n", elapsed_time / 60,
				(double) current_transaction_count[DELIVERY] /
				sample_length * 60.6);
			fprintf(log_ostpm, "%d,%f\n", elapsed_time / 60,
				(double) current_transaction_count[
				ORDER_STATUS] / sample_length * 60.6);
			fprintf(log_ptpm, "%d,%f\n", elapsed_time / 60,
				(double) current_transaction_count[PAYMENT] /
				sample_length * 60.6);
			fprintf(log_sltpm, "%d,%f\n", elapsed_time / 60,
				(double) current_transaction_count[
				STOCK_LEVEL] / sample_length * 60.6);

			elapsed_time += sample_length;
			previous_time = current_time;

			for (i = 0; i < TRANSACTION_MAX; i++) {
				current_transaction_count[i] = 0;
			}
		}

		total_response_time += response_time;
		++total_transaction_count;
		
		if (transaction == 'n') {
			++transaction_count[NEW_ORDER];
			transaction_response_time[NEW_ORDER] += response_time;
			++current_transaction_count[NEW_ORDER];
		} else if (transaction == 'p') {
			++transaction_count[PAYMENT];
			transaction_response_time[PAYMENT] += response_time;
			++current_transaction_count[PAYMENT];
		} else if (transaction == 'o') {
			++transaction_count[ORDER_STATUS];
			transaction_response_time[ORDER_STATUS] +=
				response_time;
			++current_transaction_count[ORDER_STATUS];
		} else if (transaction == 'd') {
			++transaction_count[DELIVERY];
			transaction_response_time[DELIVERY] += response_time;
			++current_transaction_count[DELIVERY];
		} else if (transaction == 's') {
			++transaction_count[STOCK_LEVEL];
			transaction_response_time[STOCK_LEVEL] += response_time;
			++current_transaction_count[STOCK_LEVEL];
		} else if (transaction == 'N') {
			++rollback_count[NEW_ORDER];
			++transaction_count[NEW_ORDER];
			transaction_response_time[NEW_ORDER] += response_time;
			++current_transaction_count[NEW_ORDER];
		} else if (transaction == 'P') {
			++rollback_count[PAYMENT];
			++transaction_count[PAYMENT];
			transaction_response_time[PAYMENT] += response_time;
			++current_transaction_count[PAYMENT];
		} else if (transaction == 'O') {
			++rollback_count[ORDER_STATUS];
			++transaction_count[ORDER_STATUS];
			transaction_response_time[ORDER_STATUS] +=
				response_time;
			++current_transaction_count[ORDER_STATUS];
		} else if (transaction == 'D') {
			++rollback_count[DELIVERY];
			++transaction_count[DELIVERY];
			transaction_response_time[DELIVERY] += response_time;
			++current_transaction_count[DELIVERY];
		} else if (transaction == 'S') {
			++rollback_count[STOCK_LEVEL];
			++transaction_count[STOCK_LEVEL];
			transaction_response_time[STOCK_LEVEL] += response_time;
			++current_transaction_count[STOCK_LEVEL];
		} else if (transaction == 'E') {
			++errors;
		} else {
			printf("unknown transaction, continuing\n");
			continue;
		}
	}
	fclose(log_mix);
	fclose(log_notpm);
	fclose(log_dtpm);
	fclose(log_ostpm);
	fclose(log_ptpm);
	fclose(log_sltpm);

	/* Calculate the actual mix of transactions. */
	printf("Transaction       %%  Avg Response Time (s)        Total  Rollbacks      %%\n");
	for (i = 0; i < TRANSACTION_MAX; i++) {
		printf("%12s  %5.2f  %21.3f  %11d  %9d  %5.2f\n",
			transaction_name[i],
			(double) transaction_count[i] /
			(double) total_transaction_count * 100.0,
			transaction_response_time[i] /
			(double) transaction_count[i],
			transaction_count[i], rollback_count[i],
			(double) rollback_count[i] /
			(double) transaction_count[i] * 100.0);
	}

	/* Calculated the number of transactions per second. */
	tps = (double) transaction_count[NEW_ORDER] /
		difftime(current_time, start_time);
	printf("\n");
	printf("%0.2f new-order transactions per minute (NOTPM)\n", tps * 60);
	printf("%0.1f minute duration\n",
		difftime(current_time, start_time) / 60.0);
	printf("%d total unknown errors\n", errors);
	printf("\n");

	return 0;
}
