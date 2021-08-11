/*
 * This file is released under the terms of the Artistic License.
 * Please see the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Open Source Development Labs, Inc.
 *               2002-2021 Mark Wong
 *
 * Based on TPC-C Standard Specification Revision 5.0.
 */

#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <wchar.h>
#include <math.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "entropy.h"

#define CUSTOMER_DATA "customer.data"
#define DISTRICT_DATA "district.data"
#define HISTORY_DATA "history.data"
#define ITEM_DATA "item.data"
#define NEW_ORDER_DATA "new_order.data"
#define ORDER_DATA "order.data"
#define ORDER_LINE_DATA "order_line.data"
#define STOCK_DATA "stock.data"
#define WAREHOUSE_DATA "warehouse.data"

#define MODE_SAPDB 0
#define MODE_PGSQL 1
#define MODE_MYSQL 2
#define MODE_DRIZZLE 3

#define MODE_FLAT 0
#define MODE_DIRECT 1

void gen_customers();
void gen_districts();
void gen_history();
void gen_items();
void gen_new_order();
void gen_orders();
void gen_stock();
void gen_warehouses();

int warehouses = 0;
int customers = CUSTOMER_CARDINALITY;
int items = ITEM_CARDINALITY;
int orders = ORDER_CARDINALITY;
int new_orders = NEW_ORDER_CARDINALITY;

int mode_load = MODE_FLAT;

int mode_string = MODE_PGSQL;
char delimiter = ',';
char null_str[16] = "\"NULL\"";

unsigned long long seed = 0;
int table = TABLE_ALL;

int w_id = -1;

int part = 1; /* Which partition to generator. */
int partitions = 1; /* How many partitions of data. */

#define ERR_MSG( fn ) { (void)fflush(stderr); \
		(void)fprintf(stderr, __FILE__ ":%d:" #fn ": %s\n", \
		__LINE__, strerror(errno)); }
#define METAPRINTF( args ) if( fprintf args < 0  ) ERR_MSG( fn )

/* Oh my gosh, is there a better way to do this? */
#define FPRINTF(a, b, c) \
	if (mode_string == MODE_SAPDB) { \
		METAPRINTF((a, "\""b"\"", c)); \
	} else if (mode_string == MODE_PGSQL || mode_string == MODE_MYSQL || \
			mode_string == MODE_DRIZZLE) { \
		METAPRINTF((a, b, c)); \
	}
#define FPRINTF2(a, b) \
	if (mode_string == MODE_SAPDB) { \
		METAPRINTF((a, "\""b"\"")); \
	} else if (mode_string == MODE_PGSQL || mode_string == MODE_MYSQL || \
			mode_string == MODE_DRIZZLE) { \
		METAPRINTF((a, b)); \
	}

void print_timestamp(FILE *ofile, struct tm *date)
{
	if (mode_string == MODE_SAPDB) {
		METAPRINTF((ofile, "\"%04d%02d%02d%02d%02d%02d000000\"",
				date->tm_year + 1900, date->tm_mon + 1, date->tm_mday,
				date->tm_hour, date->tm_min, date->tm_sec));
	} else if (mode_string == MODE_PGSQL || mode_string == MODE_MYSQL || \
			mode_string == MODE_DRIZZLE) {
		METAPRINTF((ofile, "%04d-%02d-%02d %02d:%02d:%02d",
				date->tm_year + 1900, date->tm_mon + 1, date->tm_mday,
				date->tm_hour, date->tm_min, date->tm_sec));
	} else {
		printf("unknown string mode: %d\n", mode_string);
		exit(1);
	}
}

/* Clause 4.3.3.1 */
void gen_customers()
{
	FILE *output;
	int i, j, k;
	wchar_t a_string[1024];
	char sa_string[4096];
	struct tm *tm1;
	time_t t1;
	char filename[1024] = "\0";
	pcg64f_random_t rng;

	double partition_size = (double) warehouses / (double) partitions;
	int start = (int) round(partition_size * (double) (part - 1));
	int end = (int) round(partition_size * (double) part);

	end = end > warehouses ? warehouses : end;

	pcg64f_srandom_r(&rng, seed);
	printf("Generating customer table data for warehouse %d to %d...\n",
			start + 1, end);

	if (mode_load == MODE_FLAT) {
		if (strlen(output_path) > 0) {
			strcpy(filename, output_path);
			strcat(filename, "/");
		}
		strcat(filename, CUSTOMER_DATA);
		if (partitions > 1) {
			char temp[4];
			strcat(filename, ".");
			snprintf(temp, 3, "%d", part);
			strcat(filename, temp);
		}
		output = fopen(filename, "w");
		if (output == NULL) {
			printf("cannot open %s\n", CUSTOMER_DATA);
			return;
		}
	} else if (mode_load == MODE_DIRECT) {
		switch (mode_string) {
		case MODE_PGSQL:
			output = popen("psql", "w");
			if (output == NULL) {
				printf("error cannot open pipe for direct load\n");
				return;
			}
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			fprintf(output, "BEGIN;\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			fprintf(output,
					"COPY customer FROM STDIN DELIMITER '%c' NULL '%s';\n",
					delimiter, null_str);
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;
			break;
		}
	} else {
		printf("error unknown load mode: %d\n", mode_load);
	}

	if (part > 1)
		pcg64f_advance_r(&rng,
				(part - 1) * (DISTRICT_CARDINALITY *
						(customers * 10 + 2 * (customers - 1000))));

	for (i = start; i < end; i++) {
		for (j = 0; j < DISTRICT_CARDINALITY; j++) {
			for (k = 0; k < customers; k++) {
				/* c_id */
				FPRINTF(output, "%d", k + 1);
				METAPRINTF((output, "%c", delimiter));

				/* c_d_id */
				FPRINTF(output, "%d", j + 1);
				METAPRINTF((output, "%c", delimiter));

				/* c_w_id */
				FPRINTF(output, "%d", i + 1);
				METAPRINTF((output, "%c", delimiter));

				/* c_first */
				get_a_string(&rng, a_string, 8, 16);
				wcstombs(sa_string, a_string, 4096);
				FPRINTF(output, "%s", sa_string);
				METAPRINTF((output, "%c", delimiter));

				/* c_middle */
				FPRINTF2(output, "OE");
				METAPRINTF((output, "%c", delimiter));

				/* c_last Clause 4.3.2.7 */
				if (k < 1000) {
					get_c_last(a_string, k);
				} else {
					get_c_last(a_string, get_nurand(&rng, 255, 0, 999));
				}
				wcstombs(sa_string, a_string, 4096);
				FPRINTF(output, "%s", sa_string);
				METAPRINTF((output, "%c", delimiter));

				/* c_street_1 */
				get_a_string(&rng, a_string, 10, 20);
				wcstombs(sa_string, a_string, 4096);
				FPRINTF(output, "%s", sa_string);
				METAPRINTF((output, "%c", delimiter));

				/* c_street_2 */
				get_a_string(&rng, a_string, 10, 20);
				wcstombs(sa_string, a_string, 4096);
				FPRINTF(output, "%s", sa_string);
				METAPRINTF((output, "%c", delimiter));

				/* c_city */
				get_a_string(&rng, a_string, 10, 20);
				wcstombs(sa_string, a_string, 4096);
				FPRINTF(output, "%s", sa_string);
				METAPRINTF((output, "%c", delimiter));

				/* c_state */
				get_a_string(&rng, a_string, 2, 2);
				wcstombs(sa_string, a_string, 4096);
				FPRINTF(output, "%s", sa_string);
				METAPRINTF((output, "%c", delimiter));

				/* c_zip */
				get_n_string(&rng, a_string, 4, 4);
				wcstombs(sa_string, a_string, 4096);
				FPRINTF(output, "%s11111", sa_string);
				METAPRINTF((output, "%c", delimiter));

				/* c_phone */
				get_n_string(&rng, a_string, 16, 16);
				wcstombs(sa_string, a_string, 4096);
				FPRINTF(output, "%s", sa_string);
				METAPRINTF((output, "%c", delimiter));

				/* c_since */
				/*
				 * Milliseconds are not calculated.  This
				 * should also be the time when the data is
				 * loaded, I think.
				 */
				time(&t1);
				tm1 = localtime(&t1);
				print_timestamp(output, tm1);
				METAPRINTF((output, "%c", delimiter));

				/* c_credit */
				if (get_percentage(&rng) < .10) {
					FPRINTF2(output, "BC");
				} else {
					FPRINTF2(output, "GC");
				}
				METAPRINTF((output, "%c", delimiter));

				/* c_credit_lim */
				FPRINTF2(output, "50000.00");
				METAPRINTF((output, "%c", delimiter));

				/* c_discount */
				FPRINTF(output, "0.%04d", (int) get_random(&rng, 5000));
				METAPRINTF((output, "%c", delimiter));

				/* c_balance */
				FPRINTF2(output, "-10.00");
				METAPRINTF((output, "%c", delimiter));

				/* c_ytd_payment */
				FPRINTF2(output, "10.00");
				METAPRINTF((output, "%c", delimiter));

				/* c_payment_cnt */
				FPRINTF2(output, "1");
				METAPRINTF((output, "%c", delimiter));

				/* c_delivery_cnt */
				FPRINTF2(output, "0");
				METAPRINTF((output, "%c", delimiter));

				/* c_data */
				get_a_string(&rng, a_string, 300, 500);
				wcstombs(sa_string, a_string, 4096);
				FPRINTF(output, "%s", sa_string);
				METAPRINTF((output, "\n"));
			}
		}
	}

	if (mode_load == MODE_FLAT) {
		fclose(output);
	} else {
		switch (mode_string) {
		case MODE_PGSQL:
			fprintf(output, "\\.\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			fprintf(output, "COMMIT;\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			pclose(output);
			break;
		}
	}

	printf("Finished customer table data for warehouse %d to %d...\n",
			start + 1, end);
	return;
}

/* Clause 4.3.3.1 */
void gen_districts()
{
	FILE *output;
	int i, j;
	wchar_t a_string[48];
	char sa_string[192];
	char filename[1024] = "\0";
	pcg64f_random_t rng;

	double partition_size = (double) warehouses / (double) partitions;
	int start = (int) round(partition_size * (double) (part - 1));
	int end = (int) round(partition_size * (double) part);

	end = end > warehouses ? warehouses : end;

	pcg64f_srandom_r(&rng, seed);
	printf("Generating district table data for warehouse %d to %d...\n",
			start + 1, end);

	if (mode_load == MODE_FLAT) {
		if (strlen(output_path) > 0) {
			strcpy(filename, output_path);
			strcat(filename, "/");
		}
		strcat(filename, DISTRICT_DATA);
		if (partitions > 1) {
			char temp[4];
			strcat(filename, ".");
			snprintf(temp, 3, "%d", part);
			strcat(filename, temp);
		}
		output = fopen(filename, "w");
		if (output == NULL) {
			printf("cannot open %s\n", DISTRICT_DATA);
			return;
		}
	} else if (mode_load == MODE_DIRECT) {
		switch (mode_string) {
		case MODE_PGSQL:
			output = popen("psql", "w");
			if (output == NULL) {
				printf("error cannot open pipe for direct load\n");
				return;
			}
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			fprintf(output, "BEGIN;\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			fprintf(output,
					"COPY district FROM STDIN DELIMITER '%c' NULL '%s';\n",
					delimiter, null_str);
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;
			break;
		}
	} else {
		printf("error unknown load mode: %d\n", mode_load);
	}

	if (part > 1)
		pcg64f_advance_r(&rng, (part - 1) * DISTRICT_CARDINALITY * 7);

	for (i = start; i < end; i++) {
		for (j = 0; j < DISTRICT_CARDINALITY; j++) {
			/* d_id */
			FPRINTF(output, "%d", j + 1);
			METAPRINTF((output, "%c", delimiter));

			/* d_w_id */
			FPRINTF(output, "%d", i + 1);
			METAPRINTF((output, "%c", delimiter));

			/* d_name */
			get_a_string(&rng, a_string, 6, 10);
			wcstombs(sa_string, a_string, 192);
			FPRINTF(output, "%s", sa_string);
			METAPRINTF((output, "%c", delimiter));

			/* d_street_1 */
			get_a_string(&rng, a_string, 10, 20);
			wcstombs(sa_string, a_string, 192);
			FPRINTF(output, "%s", sa_string);
			METAPRINTF((output, "%c", delimiter));

			/* d_street_2 */
			get_a_string(&rng, a_string, 10, 20);
			wcstombs(sa_string, a_string, 192);
			FPRINTF(output, "%s", sa_string);
			METAPRINTF((output, "%c", delimiter));

			/* d_city */
			get_a_string(&rng, a_string, 10, 20);
			wcstombs(sa_string, a_string, 192);
			FPRINTF(output, "%s", sa_string);
			METAPRINTF((output, "%c", delimiter));

			/* d_state */
			get_a_string(&rng, a_string, 2, 2);
			wcstombs(sa_string, a_string, 192);
			FPRINTF(output, "%s", sa_string);
			METAPRINTF((output, "%c", delimiter));

			/* d_zip */
			get_n_string(&rng, a_string, 4, 4);
			wcstombs(sa_string, a_string, 192);
			FPRINTF(output, "%s11111", sa_string);
			METAPRINTF((output, "%c", delimiter));

			/* d_tax */
			FPRINTF(output, "0.%04d", (int) get_random(&rng, 2000));
			METAPRINTF((output, "%c", delimiter));

			/* d_ytd */
			FPRINTF2(output, "30000.00");
			METAPRINTF((output, "%c", delimiter));

			/* d_next_o_id */
			FPRINTF2(output, "3001");

			METAPRINTF((output, "\n"));
		}
	}

	if (mode_load == MODE_FLAT) {
		fclose(output);
	} else {
		switch (mode_string) {
		case MODE_PGSQL:
			fprintf(output, "\\.\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			fprintf(output, "COMMIT;\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			pclose(output);
			break;
		}
	}

	printf("Finished district table data for warehouse %d to %d...\n",
			start + 1, end);
	return;
}

/* Clause 4.3.3.1 */
void gen_history()
{
	FILE *output;
	int i, j, k;
	wchar_t a_string[64];
	char sa_string[256];
	struct tm *tm1;
	time_t t1;
	char filename[1024] = "\0";
	pcg64f_random_t rng;

	double partition_size = (double) warehouses / (double) partitions;
	int start = (int) round(partition_size * (double) (part - 1));
	int end = (int) round(partition_size * (double) part);

	end = end > warehouses ? warehouses : end;

	pcg64f_srandom_r(&rng, seed);
	printf("Generating history table data from warehouse %d to %d...\n",
			start + 1, end);

	if (mode_load == MODE_FLAT) {
		if (strlen(output_path) > 0) {
			strcpy(filename, output_path);
			strcat(filename, "/");
		}
		strcat(filename, HISTORY_DATA);
		if (partitions > 1) {
			char temp[4];
			strcat(filename, ".");
			snprintf(temp, 3, "%d", part);
			strcat(filename, temp);
		}
		output = fopen(filename, "w");
		if (output == NULL) {
			printf("cannot open %s\n", HISTORY_DATA);
			return;
		}
	} else if (mode_load == MODE_DIRECT) {
		switch (mode_string) {
		case MODE_PGSQL:
			output = popen("psql", "w");
			if (output == NULL) {
				printf("error cannot open pipe for direct load\n");
				return;
			}
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			fprintf(output, "BEGIN;\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			fprintf(output,
					"COPY history FROM STDIN DELIMITER '%c' NULL '%s';\n",
					delimiter, null_str);
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;
			break;
		}
	} else {
		printf("error unknown load mode: %d\n", mode_load);
	}

	if (part > 1)
		pcg64f_advance_r(&rng, (part - 1) * DISTRICT_CARDINALITY * customers);

	for (i = start; i < end; i++) {
		for (j = 0; j < DISTRICT_CARDINALITY; j++) {
			for (k = 0; k < customers; k++) {
				/* h_c_id */
				FPRINTF(output, "%d", k + 1);
				METAPRINTF((output, "%c", delimiter));

				/* h_c_d_id */
				FPRINTF(output, "%d", j + 1);
				METAPRINTF((output, "%c", delimiter));

				/* h_c_w_id */
				FPRINTF(output, "%d", i + 1);
				METAPRINTF((output, "%c", delimiter));

				/* h_d_id */
				FPRINTF(output, "%d", j + 1);
				METAPRINTF((output, "%c", delimiter));

				/* h_w_id */
				FPRINTF(output, "%d", i + 1);
				METAPRINTF((output, "%c", delimiter));

				/* h_date */
				/*
				 * Milliseconds are not calculated.  This
				 * should also be the time when the data is
				 * loaded, I think.
				 */
				time(&t1);
				tm1 = localtime(&t1);
				print_timestamp(output, tm1);
				METAPRINTF((output, "%c", delimiter));

				/* h_amount */
				FPRINTF2(output, "10.00");
				METAPRINTF((output, "%c", delimiter));

				/* h_data */
				get_a_string(&rng, a_string, 12, 24);
				wcstombs(sa_string, a_string, 256);
				FPRINTF(output, "%s", sa_string);

				METAPRINTF((output, "\n"));
			}
		}
	}

	if (mode_load == MODE_FLAT) {
		fclose(output);
	} else {
		switch (mode_string) {
		case MODE_PGSQL:
			fprintf(output, "\\.\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			fprintf(output, "COMMIT;\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			pclose(output);
			break;
		}
	}

	printf("Finished history table data for warehouse %d to %d...\n",
			start + 1, end);
	return;
}

/* Clause 4.3.3.1 */
void gen_items()
{
	FILE *output;
	int i;
	wchar_t a_string[128];
	char sa_string[512];
	int j;
	char filename[1024] = "\0";
	pcg64f_random_t rng;
	pcg64f_random_t temp_rng;

	double partition_size = (double) items / (double) partitions;
	int start = (int) round(partition_size * (double) (part - 1));
	int end = (int) round(partition_size * (double) part);

	end = end > items ? items : end;

	pcg64f_srandom_r(&rng, seed);
	printf("Generating item table data from %d to %d...\n", start + 1, end);

	if (mode_load == MODE_FLAT) {
		if (strlen(output_path) > 0) {
			strcpy(filename, output_path);
			strcat(filename, "/");
		}
		strcat(filename, ITEM_DATA);
		if (partitions > 1) {
			char temp[4];
			strcat(filename, ".");
			snprintf(temp, 3, "%d", part);
			strcat(filename, temp);
		}
		output = fopen(filename, "w");
		if (output == NULL) {
			printf("cannot open %s\n", ITEM_DATA);
			return;
		}
	} else if (mode_load == MODE_DIRECT) {
		switch (mode_string) {
		case MODE_PGSQL:
			output = popen("psql", "w");
			if (output == NULL) {
				printf("error cannot open pipe for direct load\n");
				return;
			}
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			fprintf(output, "BEGIN;\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			fprintf(output,
					"COPY item FROM STDIN DELIMITER '%c' NULL '%s';\n",
					delimiter, null_str);
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;
			break;
		}
	} else {
		printf("error unknown load mode: %d\n", mode_load);
	}

	if (part > 1)
		pcg64f_advance_r(&rng,
				round((double) (part - 1) * partition_size * 5.0));

	for (i = start; i < end; i++) {
		/* i_id */
		FPRINTF(output, "%d", i + 1);
		METAPRINTF((output, "%c", delimiter));

		/* i_im_id */
		FPRINTF(output, "%d", (int) get_random(&rng, 9999) + 1);
		METAPRINTF((output, "%c", delimiter));

		/* i_name */
		get_a_string(&rng, a_string, 14, 24);
		wcstombs(sa_string, a_string, 512);
		FPRINTF(output, "%s", sa_string);
		METAPRINTF((output, "%c", delimiter));

		/* i_price */
		FPRINTF(output, "%0.2f", ((double) get_random(&rng, 9900) + 100.0) / 100.0);
		METAPRINTF((output, "%c", delimiter));

		/* i_data */
		get_a_string(&rng, a_string, 26, 50);
		pcg64f_srandom_r(&temp_rng, pcg64f_random_r(&rng));
		if (get_percentage(&temp_rng) < .10) {
			j = (int) get_random(&temp_rng, wcslen(a_string) - 8);
			a_string[j++] = L'O';
			a_string[j++] = L'R';
			a_string[j++] = L'I';
			a_string[j++] = L'G';
			a_string[j++] = L'I';
			a_string[j++] = L'N';
			a_string[j++] = L'A';
			a_string[j] = L'L';
		}
		wcstombs(sa_string, a_string, 512);
		FPRINTF(output, "%s", sa_string);

		METAPRINTF((output, "\n"));
	}

	if (mode_load == MODE_FLAT) {
		fclose(output);
	} else {
		switch (mode_string) {
		case MODE_PGSQL:
			fprintf(output, "\\.\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			fprintf(output, "COMMIT;\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			pclose(output);
			break;
		}
	}

	printf("Finished item table data from %d to %d...\n", start + 1, end);
	return;
}

/* Clause 4.3.3.1 */
void gen_new_orders()
{
	FILE *output;
	int i, j, k;
	char filename[1024] = "\0";
	pcg64f_random_t rng;

	double partition_size = (double) warehouses / (double) partitions;
	int start = (int) round(partition_size * (double) (part - 1));
	int end = (int) round(partition_size * (double) part);

	end = end > warehouses ? warehouses : end;

	pcg64f_srandom_r(&rng, seed);
	printf("Generating new-order table data for warehouse %d to %d...\n",
			start + 1, end);

	if (mode_load == MODE_FLAT) {
		if (strlen(output_path) > 0) {
			strcpy(filename, output_path);
			strcat(filename, "/");
		}
		strcat(filename, NEW_ORDER_DATA);
		if (partitions > 1) {
			char temp[4];
			strcat(filename, ".");
			snprintf(temp, 3, "%d", part);
			strcat(filename, temp);
		}
		output = fopen(filename, "w");
		if (output == NULL) {
			printf("cannot open %s\n", NEW_ORDER_DATA);
			return;
		}
	} else if (mode_load == MODE_DIRECT) {
		switch (mode_string) {
		case MODE_PGSQL:
			output = popen("psql", "w");
			if (output == NULL) {
				printf("error cannot open pipe for direct load\n");
				return;
			}
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			fprintf(output, "BEGIN;\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			fprintf(output,
					"COPY new_order FROM STDIN DELIMITER '%c' NULL '%s';\n",
					delimiter, null_str);
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;
			break;
		}
	} else {
		printf("error unknown load mode: %d\n", mode_load);
	}

	for (i = start; i < end; i++) {
		for (j = 0; j < DISTRICT_CARDINALITY; j++) {
			for (k = orders - new_orders; k < orders; k++) {
				/* no_o_id */
				FPRINTF(output, "%d", k + 1);
				METAPRINTF((output, "%c", delimiter));

				/* no_d_id */
				FPRINTF(output, "%d", j + 1);
				METAPRINTF((output, "%c", delimiter));

				/* no_w_id */
				FPRINTF(output, "%d", i + 1);

				METAPRINTF((output, "\n"));
			}
		}
	}

	if (mode_load == MODE_FLAT) {
		fclose(output);
	} else {
		switch (mode_string) {
		case MODE_PGSQL:
			fprintf(output, "\\.\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			fprintf(output, "COMMIT;\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			pclose(output);
			break;
		}
	}

	printf("Finished new-order table data for warehouse %d to %d...\n",
			start + 1, end);
	return;
}

/* Clause 4.3.3.1 */
void gen_orders()
{
	FILE *order, *order_line;
	int i, j, k, l;
	wchar_t a_string[64];
	char sa_string[256];
	struct tm *tm1;
	time_t t1;
	char filename[1024] = "\0";

	struct node_t {
		int value;
		struct node_t *next;
	};
	struct node_t *head;
	struct node_t *current;
	struct node_t *prev;
	struct node_t *new_node;
	int iter;

	int o_ol_cnt;

	pcg64f_random_t rng;
	pcg64f_random_t ol_rng;

	double partition_size = (double) warehouses / (double) partitions;
	int start = (int) round(partition_size * (double) (part - 1));
	int end = (int) round(partition_size * (double) part);

	end = end > warehouses ? warehouses : end;

	pcg64f_srandom_r(&rng, seed);
	printf("Generating order and order-line table data for warehouse %d to %d"
			"...\n", start + 1, end);

	if (mode_load == MODE_FLAT) {
		if (strlen(output_path) > 0) {
			strcpy(filename, output_path);
			strcat(filename, "/");
		}
		strcat(filename, ORDER_DATA);
		if (partitions > 1) {
			char temp[4];
			strcat(filename, ".");
			snprintf(temp, 3, "%d", part);
			strcat(filename, temp);
		}
		order = fopen(filename, "w");
		if (order == NULL) {
			printf("cannot open %s\n", ORDER_DATA);
			return;
		}

		filename[0] = '\0';
		if (strlen(output_path) > 0) {
			strcpy(filename, output_path);
			strcat(filename, "/");
		}
		strcat(filename, ORDER_LINE_DATA);
		if (partitions > 1) {
			char temp[4];
			strcat(filename, ".");
			snprintf(temp, 3, "%d", part);
			strcat(filename, temp);
		}
		order_line = fopen(filename, "w");
		if (order_line == NULL) {
			printf("cannot open %s\n", ORDER_LINE_DATA);
			return;
		}
	} else if (mode_load == MODE_DIRECT) {
		switch (mode_string) {
		case MODE_PGSQL:
			order = popen("psql", "w");
			if (order == NULL) {
				printf("error cannot open pipe for direct load\n");
				return;
			}
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(order) != EOF) ;

			fprintf(order, "BEGIN;\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(order) != EOF) ;

			fprintf(order,
					"COPY orders FROM STDIN DELIMITER '%c' NULL '%s';\n",
					delimiter, null_str);
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(order) != EOF) ;

			order_line = popen("psql", "w");
			if (order_line == NULL) {
				printf("error cannot open pipe for direct load\n");
				return;
			}
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(order_line) != EOF) ;

			fprintf(order_line, "BEGIN;\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(order_line) != EOF) ;

			fprintf(order_line,
					"COPY order_line FROM STDIN DELIMITER '%c' NULL '%s';\n",
					delimiter, null_str);
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(order_line) != EOF) ;
			break;
		}
	} else {
		printf("error unknown load mode: %d\n", mode_load);
	}

	if (part > 1)
		pcg64f_advance_r(&rng, (part - 1) * DISTRICT_CARDINALITY * ((customers - 1) + 2101 + orders));
		/*
		pcg64f_advance_r(&rng, (part - 1) * DISTRICT_CARDINALITY * ((customers - 1) + orders + 2100));
		*/

	for (i = start; i < end; i++) {
		for (j = 0; j < DISTRICT_CARDINALITY; j++) {
			/*
			 * Create a random list of numbers from 1 to customers for o_c_id.
			 */
			head = (struct node_t *) malloc(sizeof(struct node_t));
			head->value = 1;
			head->next = NULL;
			for (k = 2; k <= customers; k++) {
				current = prev = head;

				/* Find a random place in the list to insert a number. */
				iter = (int) get_random(&rng, k - 1);
				while (iter > 0) {
					prev = current;
					current = current->next;
					--iter;
				}

				/* Insert the number. */
				new_node = (struct node_t *) malloc(sizeof(struct node_t));
				if (current == prev) {
					/* Insert at the head of the list. */
					new_node->next = head;
					head = new_node;
				} else if (current == NULL) {
					/* Insert at the tail of the list. */
					prev->next = new_node;
					new_node->next = NULL;
				} else {
					/* Insert somewhere in the middle of the list. */
					prev->next = new_node;
					new_node->next = current;
				}
				new_node->value = k;
			}

			current = head;
			for (k = 0; k < orders; k++) {
				/* o_id */
				FPRINTF(order, "%d", k + 1);
				METAPRINTF((order, "%c", delimiter));

				/* o_d_id */
				FPRINTF(order, "%d", j + 1);
				METAPRINTF((order, "%c", delimiter));

				/* o_w_id */
				FPRINTF(order, "%d", i + 1);
				METAPRINTF((order, "%c", delimiter));

				/* o_c_id */
				FPRINTF(order, "%d", current->value);
				METAPRINTF((order, "%c", delimiter));
				current = current->next;

				/* o_entry_d */
				/*
				 * Milliseconds are not calculated.  This
				 * should also be the time when the data is
				 * loaded, I think.
				 */
				time(&t1);
				tm1 = localtime(&t1);
				print_timestamp(order, tm1);
				METAPRINTF((order, "%c", delimiter));

				/* o_carrier_id */
				if (mode_string == MODE_DRIZZLE) {
					/*
					 * FIXME: Not to spec but Drizzle doesn't handle null
					 * value for when reading from a file for now
					 */
					FPRINTF(order, "%d", (int) get_random(&rng, 9) + 1);
				}
				else {
					if (k < 2101) {
						FPRINTF(order, "%d", (int) get_random(&rng, 9) + 1);
					} else {
						METAPRINTF((order, "%s", null_str));
					}
				}
				METAPRINTF((order, "%c", delimiter));

				/* o_ol_cnt */
				pcg64f_srandom_r(&ol_rng, pcg64f_random_r(&rng));
				o_ol_cnt = (int) get_random(&ol_rng, 10) + 5;
				FPRINTF(order, "%d", o_ol_cnt);
				METAPRINTF((order, "%c", delimiter));

				/* o_all_local */
				FPRINTF2(order, "1");

				METAPRINTF((order, "\n"));

				/*
				 * Generate data in the order-line table for
				 * this order.
				 */
				for (l = 0; l < o_ol_cnt; l++) {
					/* ol_o_id */
					FPRINTF(order_line, "%d", k + 1);
					METAPRINTF((order_line, "%c", delimiter));

					/* ol_d_id */
					FPRINTF(order_line, "%d", j + 1);
					METAPRINTF((order_line, "%c", delimiter));

					/* ol_w_id */
					FPRINTF(order_line, "%d", i + 1);
					METAPRINTF((order_line, "%c", delimiter));

					/* ol_number */
					FPRINTF(order_line, "%d", l + 1);
					METAPRINTF((order_line, "%c", delimiter));

					/* ol_i_id */
					FPRINTF(order_line, "%d", (int) get_random(&ol_rng,
							ITEM_CARDINALITY - 1) + 1);
					METAPRINTF((order_line, "%c", delimiter));

					/* ol_supply_w_id */
					FPRINTF(order_line, "%d", i + 1);
					METAPRINTF((order_line, "%c", delimiter));

					/* ol_delivery_d */
					if (mode_string == MODE_DRIZZLE) {
						/*
						 * FIXME: Not to spec but Drizzle doesn't handle null
						 * value for when reading from a file for now
						 */
						time(&t1);
						tm1 = localtime(&t1);
						print_timestamp(order_line, tm1);
					}
					else  {
						if (k < 2101) {
						/*
						 * Milliseconds are not
						 * calculated.  This should
						 * also be the time when the
						 * data is loaded, I think.
						 */
							time(&t1);
							tm1 = localtime(&t1);
							print_timestamp(order_line, tm1);
						} else {
							METAPRINTF((order_line, "%s", null_str));
						}
					}
					METAPRINTF((order_line, "%c", delimiter));

					/* ol_quantity */
					FPRINTF2(order_line, "5");
					METAPRINTF((order_line, "%c", delimiter));

					/* ol_amount */
					if (k < 2101) {
						FPRINTF2(order_line, "0.00");
					} else {
						FPRINTF(order_line, "%f", (double) (get_random(&ol_rng,
								999998) + 1) / 100.0);
					}
					METAPRINTF((order_line, "%c", delimiter));

					/* ol_dist_info */
					get_l_string(&ol_rng, a_string, 24, 24);
					wcstombs(sa_string, a_string, 256);
					FPRINTF(order_line, "%s", sa_string);

					METAPRINTF((order_line, "\n"));
				}
			}
			while (head != NULL) {
				current = head;
				head = head->next;
				free(current);
			}
		}
	}

	if (mode_load == MODE_FLAT) {
		fclose(order);
		fclose(order_line);
	} else {
		switch (mode_string) {
		case MODE_PGSQL:
			fprintf(order, "\\.\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(order) != EOF) ;
			fprintf(order_line, "\\.\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(order_line) != EOF) ;

			fprintf(order, "COMMIT;\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(order) != EOF) ;
			fprintf(order_line, "COMMIT;\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(order_line) != EOF) ;

			pclose(order);
			pclose(order_line);
			break;
		}
	}
	printf("Finished order and order-line table data for warehouse %d to %d"
			"...\n", start + 1, end);
	return;
}

/* Clause 4.3.3.1 */
void gen_stock()
{
	FILE *output;
	int i, j, k;
	wchar_t a_string[128];
	char sa_string[512];
	char filename[1024] = "\0";
	pcg64f_random_t rng;
	pcg64f_random_t temp_rng;

	double partition_size = (double) warehouses / (double) partitions;
	int start = (int) round(partition_size * (double) (part - 1));
	int end = (int) round(partition_size * (double) part);

	end = end > warehouses ? warehouses : end;

	pcg64f_srandom_r(&rng, seed);
	printf("Generating stock table data for warehouse %d to %d...\n",
			start + 1, end);

	if (mode_load == MODE_FLAT) {
		if (strlen(output_path) > 0) {
			strcpy(filename, output_path);
			strcat(filename, "/");
		}
		strcat(filename, STOCK_DATA);
		if (partitions > 1) {
			char temp[4];
			strcat(filename, ".");
			snprintf(temp, 3, "%d", part);
			strcat(filename, temp);
		}
		output = fopen(filename, "w");
		if (output == NULL) {
			printf("cannot open %s\n", STOCK_DATA);
			return;
		}
	} else if (mode_load == MODE_DIRECT) {
		switch (mode_string) {
		case MODE_PGSQL:
			output = popen("psql", "w");
			if (output == NULL) {
				printf("error cannot open pipe for direct load\n");
				return;
			}
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			fprintf(output, "BEGIN;\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			fprintf(output,
					"COPY stock FROM STDIN DELIMITER '%c' NULL '%s';\n",
					delimiter, null_str);
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;
			break;
		}
	} else {
		printf("error unknown load mode: %d\n", mode_load);
	}

	if (part > 1)
		pcg64f_advance_r(&rng, (part - 1) * items * 13);

	for (i = start; i < end; i++) {
		for (j = 0; j < items; j++) {
			/* s_i_id */
			FPRINTF(output, "%d", j + 1);
			METAPRINTF((output, "%c", delimiter));

			/* s_w_id */
			FPRINTF(output, "%d", i + 1);
			METAPRINTF((output, "%c", delimiter));

			/* s_quantity */
			FPRINTF(output, "%d", (int) get_random(&rng, 90) + 10);
			METAPRINTF((output, "%c", delimiter));

			/* s_dist_01 */
			get_l_string(&rng, a_string, 24, 24);
			wcstombs(sa_string, a_string, 512);
			FPRINTF(output, "%s", sa_string);
			METAPRINTF((output, "%c", delimiter));

			/* s_dist_02 */
			get_l_string(&rng, a_string, 24, 24);
			wcstombs(sa_string, a_string, 512);
			FPRINTF(output, "%s", sa_string);
			METAPRINTF((output, "%c", delimiter));

			/* s_dist_03 */
			get_l_string(&rng, a_string, 24, 24);
			wcstombs(sa_string, a_string, 512);
			FPRINTF(output, "%s", sa_string);
			METAPRINTF((output, "%c", delimiter));

			/* s_dist_04 */
			get_l_string(&rng, a_string, 24, 24);
			wcstombs(sa_string, a_string, 512);
			FPRINTF(output, "%s", sa_string);
			METAPRINTF((output, "%c", delimiter));

			/* s_dist_05 */
			get_l_string(&rng, a_string, 24, 24);
			wcstombs(sa_string, a_string, 512);
			FPRINTF(output, "%s", sa_string);
			METAPRINTF((output, "%c", delimiter));

			/* s_dist_06 */
			get_l_string(&rng, a_string, 24, 24);
			wcstombs(sa_string, a_string, 512);
			FPRINTF(output, "%s", sa_string);
			METAPRINTF((output, "%c", delimiter));

			/* s_dist_07 */
			get_l_string(&rng, a_string, 24, 24);
			wcstombs(sa_string, a_string, 512);
			FPRINTF(output, "%s", sa_string);
			METAPRINTF((output, "%c", delimiter));

			/* s_dist_08 */
			get_l_string(&rng, a_string, 24, 24);
			wcstombs(sa_string, a_string, 512);
			FPRINTF(output, "%s", sa_string);
			METAPRINTF((output, "%c", delimiter));

			/* s_dist_09 */
			get_l_string(&rng, a_string, 24, 24);
			wcstombs(sa_string, a_string, 512);
			FPRINTF(output, "%s", sa_string);
			METAPRINTF((output, "%c", delimiter));

			/* s_dist_10 */
			get_l_string(&rng, a_string, 24, 24);
			wcstombs(sa_string, a_string, 512);
			FPRINTF(output, "%s", sa_string);
			METAPRINTF((output, "%c", delimiter));

			/* s_ytd */
			FPRINTF2(output, "0");
			METAPRINTF((output, "%c", delimiter));

			/* s_order_cnt */
			FPRINTF2(output, "0");
			METAPRINTF((output, "%c", delimiter));

			/* s_remote_cnt */
			FPRINTF2(output, "0");
			METAPRINTF((output, "%c", delimiter));

			/* s_data */
			get_a_string(&rng, a_string, 26, 50);
			pcg64f_srandom_r(&temp_rng, pcg64f_random_r(&rng));
			if (get_percentage(&temp_rng) < .10) {
				k = (int) get_random(&temp_rng, wcslen(a_string) - 8);
				a_string[k++] = L'O';
				a_string[k++] = L'R';
				a_string[k++] = L'I';
				a_string[k++] = L'G';
				a_string[k++] = L'I';
				a_string[k++] = L'N';
				a_string[k++] = L'A';
				a_string[k] = L'L';
			}
			wcstombs(sa_string, a_string, 512);
			FPRINTF(output, "%s", sa_string);

			METAPRINTF((output, "\n"));
		}
	}

	if (mode_load == MODE_FLAT) {
		fclose(output);
	} else {
		switch (mode_string) {
		case MODE_PGSQL:
			fprintf(output, "\\.\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			fprintf(output, "COMMIT;\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			pclose(output);
			break;
		}
	}

	printf("Finished stock table data for warehouse %d to %d...\n", start + 1,
			end);
	return;
}

/* Clause 4.3.3.1 */
void gen_warehouses()
{
	FILE *output;
	int i;
	wchar_t a_string[48];
	char sa_string[192];
	char filename[1024] = "\0";
	pcg64f_random_t rng;

	double partition_size = (double) warehouses / (double) partitions;
	int start = (int) round(partition_size * (double) (part - 1));
	int end = (int) round(partition_size * (double) part);

	end = end > warehouses ? warehouses : end;

	pcg64f_srandom_r(&rng, seed);
	printf("Generating warehouse table data %d to %d...\n", start + 1, end);

	if (mode_load == MODE_FLAT) {
		if (strlen(output_path) > 0) {
			strcpy(filename, output_path);
			strcat(filename, "/");
		}
		strcat(filename, WAREHOUSE_DATA);
		if (partitions > 1) {
			char temp[4];
			strcat(filename, ".");
			snprintf(temp, 3, "%d", part);
			strcat(filename, temp);
		}
		output = fopen(filename, "w");
		if (output == NULL) {
			printf("cannot open %s\n", WAREHOUSE_DATA);
			return;
		}
	} else if (mode_load == MODE_DIRECT) {
		switch (mode_string) {
		case MODE_PGSQL:
			output = popen("psql", "w");
			if (output == NULL) {
				printf("error cannot open pipe for direct load\n");
				return;
			}
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			fprintf(output, "BEGIN;\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			fprintf(output,
					"COPY warehouse FROM STDIN DELIMITER '%c' NULL '%s';\n",
					delimiter, null_str);
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;
			break;
		}
	} else {
		printf("error unknown load mode: %d\n", mode_load);
	}

	if (part > 1)
		pcg64f_advance_r(&rng, (part - 1) * 7);

	for (i = start; i < end; i++) {
		/* w_id */
		FPRINTF(output, "%d", i + 1);
		METAPRINTF((output, "%c", delimiter));

		/* w_name */
		get_a_string(&rng, a_string, 6, 10);
		wcstombs(sa_string, a_string, 192);
		FPRINTF(output, "%s", sa_string);
		METAPRINTF((output, "%c", delimiter));

		/* w_street_1 */
		get_a_string(&rng, a_string, 10, 20);
		wcstombs(sa_string, a_string, 192);
		FPRINTF(output, "%s", sa_string);
		METAPRINTF((output, "%c", delimiter));

		/* w_street_2 */
		get_a_string(&rng, a_string, 10, 20);
		wcstombs(sa_string, a_string, 192);
		FPRINTF(output, "%s", sa_string);
		METAPRINTF((output, "%c", delimiter));

		/* w_city */
		get_a_string(&rng, a_string, 10, 20);
		wcstombs(sa_string, a_string, 192);
		FPRINTF(output, "%s", sa_string);
		METAPRINTF((output, "%c", delimiter));

		/* w_state */
		get_a_string(&rng, a_string, 2, 2);
		wcstombs(sa_string, a_string, 192);
		FPRINTF(output, "%s", sa_string);
		METAPRINTF((output, "%c", delimiter));

		/* w_zip */
		get_n_string(&rng, a_string, 4, 4);
		wcstombs(sa_string, a_string, 192);
		FPRINTF(output, "%s11111", sa_string);
		METAPRINTF((output, "%c", delimiter));

		/* w_tax */
		FPRINTF(output, "0.%04d", (int) get_random(&rng, 2000));
		METAPRINTF((output, "%c", delimiter));

		/* w_ytd */
		FPRINTF2(output, "300000.00");

		METAPRINTF((output, "\n"));
	}

	if (mode_load == MODE_FLAT) {
		fclose(output);
	} else {
		switch (mode_string) {
		case MODE_PGSQL:
			fprintf(output, "\\.\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			fprintf(output, "COMMIT;\n");
			/* FIXME: Handle properly instead of blindly reading the output. */
			while (fgetc(output) != EOF) ;

			pclose(output);
			break;
		}
	}

	printf("Finished warehouse table data %d to %d...\n", start + 1, end);
	return;
}

int main(int argc, char *argv[])
{
	struct stat st;

	/* For getoptlong(). */
	int c;

	init_common();

	if (argc < 2) {
		printf("usage: %s [options]\n", argv[0]);
		printf("  options:\n");
		printf("    -w <int> - warehouse cardinality\n");
		printf("    -c <int> - customer cardinality, default %d\n",
				CUSTOMER_CARDINALITY);
		printf("    -i <int> - item cardinality, default %d\n",
				ITEM_CARDINALITY);
		printf("    -o <int> - order cardinality, default %d\n",
				ORDER_CARDINALITY);
		printf("    -n <int> - new-order cardinality, default %d\n",
				NEW_ORDER_CARDINALITY);
		printf("    -d <path> - output path of data files\n");
		printf("    --seed <int> - set random number generation seed\n");
		printf("    --table <table> - set random number generation seed\n");
		printf("    --drizzle - format data for Drizzle\n");
		printf("    --mysql - format data for MySQL\n");
		printf("    --pgsql - format data for PostgreSQL\n");
		printf("    --sapdb - format data for SAP DB\n");
		printf("    --direct - don't generate flat files, load directly into "
				"database\n");
		printf("    -W <int> - warehouse id start\n");
		printf("    -P | --partitions <int> - how many partitions of data, "
				"default 1\n");
		printf("    -p | --part <int> - wich partition of data to generate\n");
		return 1;
	}

	/* Parse command line arguments. */
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{ "direct", no_argument, &mode_load, MODE_DIRECT },
			{ "seed", required_argument, 0, 0 },
			{ "table", required_argument, 0, 0 },
			{ "part", required_argument, 0, 0 },
			{ "partitions", required_argument, 0, 0 },
			{ "pgsql", no_argument, &mode_string, MODE_PGSQL },
			{ "sapdb", no_argument, &mode_string, MODE_SAPDB },
			{ "mysql", no_argument, &mode_string, MODE_MYSQL },
			{ "drizzle", no_argument, &mode_string, MODE_DRIZZLE },
			{ 0, 0, 0, 0 }
		};

		c = getopt_long(argc, argv, "c:d:i:n:o:P:p:w:W:",
				long_options, &option_index);
		if (c == -1) {
			break;
		}

		switch (c) {
		case 0:
			if (strcmp(long_options[option_index].name, "table") == 0) {
				if (strcmp(optarg, "warehouse") == 0) {
					table = TABLE_WAREHOUSE;
				} else if (strcmp(optarg, "district") == 0) {
					table = TABLE_DISTRICT;
				} else if (strcmp(optarg, "customer") == 0) {
					table = TABLE_CUSTOMER;
				} else if (strcmp(optarg, "item") == 0) {
					table = TABLE_ITEM;
				} else if (strcmp(optarg, "orders") == 0) {
					table = TABLE_ORDER;
				} else if (strcmp(optarg, "stock") == 0) {
					table = TABLE_STOCK;
				} else if (strcmp(optarg, "new_order") == 0) {
					table = TABLE_NEW_ORDER;
				} else if (strcmp(optarg, "history") == 0) {
					table = TABLE_HISTORY;
				} else if (strcmp(optarg, "order_line") == 0) {
					table = TABLE_ORDER_LINE;
				} else {
					printf("unknown table: %s\n", optarg);
					return 2;
				}
			} else if (strcmp(long_options[option_index].name, "seed") == 0) {
				seed = strtoull(optarg, NULL, 10);
			}
			break;
		case 'c':
			customers = atoi(optarg);
			break;
		case 'd':
			strcpy(output_path, optarg);
			break;
		case 'i':
			items = atoi(optarg);
			break;
		case 'n':
			new_orders = atoi(optarg);
			break;
		case 'o':
			orders = atoi(optarg);
			break;
		case 'P':
			partitions = atoi(optarg);
			if (partitions < 1) {
				printf("number of partitions must be at least 1\n");
				return 2;
			}
			break;
		case 'p':
			part = atoi(optarg);
			if (part < 1) {
				printf("the part to generate must start at 1\n");
				return 2;
			}
			break;
		case 'W':
			w_id = atoi(optarg);
			/* shift w_id because counts start at 0 */
			--w_id;
			break;
		case 'w':
			warehouses = atoi(optarg);
			break;
		default:
			printf("?? getopt returned character code 0%o ??\n", c);
			return 2;
		}
	}

	if (partitions > 1) {
		if (part > partitions) {
			printf("the part to generate must be less that the number of "
					"partitions: %d\n", partitions);
			return 3;
		}
	}

	if (warehouses == 0) {
		printf("-w must be used\n");
		return 3;
	}

	if (strlen(output_path) > 0 && ((stat(output_path, &st) < 0) ||
			(st.st_mode & S_IFMT) != S_IFDIR)) {
		printf("Output directory of data files '%s' not exists\n", output_path);
		return 3;
	}

	/* Verify that datagen supports a direct load for the selected database. */
	if (mode_load == MODE_DIRECT) {
		switch (mode_string) {
		case MODE_SAPDB:
		case MODE_MYSQL:
		case MODE_DRIZZLE:
			printf("the rdbms select does not support direct loading\n");
			return 4;
		}
	}

	/* Set the correct delimiter. */
	if (mode_string == MODE_SAPDB) {
		delimiter = ',';
		strcpy(null_str, "\"NULL\"");
	} else if (mode_string == MODE_PGSQL || mode_string == MODE_MYSQL ||
			mode_string == MODE_DRIZZLE) {
		delimiter = '\t';
		strcpy(null_str, "");
	}

	if (seed == 0) {
		entropy_getbytes((void *) &seed, sizeof(seed));
	}
	printf("seed = %llu\n", seed);

	printf("warehouses = %d\n", warehouses);
	printf("districts = %d\n", DISTRICT_CARDINALITY);
	printf("customers = %d\n", customers);
	printf("items = %d\n", items);
	printf("orders = %d\n", orders);
	printf("stock = %d\n", items);
	printf("new_orders = %d\n", new_orders);
	printf("\n");

	if (mode_load != MODE_DIRECT) {
		if (strlen(output_path) > 0) {
			printf("Output directory of data files: %s\n",output_path);
		} else {
			printf("Output directory of data files: current directory\n");
		}
		printf("\n");
	}

	printf("Generating data files for %d warehouse(s)...\n", warehouses);

	if (table == TABLE_ALL || table == TABLE_ITEM)
		gen_items();
	if (table == TABLE_ALL || table == TABLE_WAREHOUSE)
		gen_warehouses();
	if (table == TABLE_ALL || table == TABLE_STOCK)
		gen_stock();
	if (table == TABLE_ALL || table == TABLE_DISTRICT)
		gen_districts();
	if (table == TABLE_ALL || table == TABLE_CUSTOMER)
		gen_customers();
	if (table == TABLE_ALL || table == TABLE_HISTORY)
		gen_history();
	if (table == TABLE_ALL || table == TABLE_ORDER)
		gen_orders();
	if (table == TABLE_ALL || table == TABLE_NEW_ORDER)
		gen_new_orders();

	return 0;
}
