/*
 * main.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
 *
 * 16 may 2002
 * Based on TPC-C Standard Specification Revision 5.0.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>

#include "common.h"

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

void gen_customers();
void gen_districts();
void gen_history();
void gen_items();
void gen_new_order();
void gen_orders();
void gen_stock();
void gen_warehouses();

char output_path[512] = "";

int warehouses = 0;
int customers = CUSTOMER_CARDINALITY;
int items = ITEM_CARDINALITY;
int orders = ORDER_CARDINALITY;
int new_orders = NEW_ORDER_CARDINALITY;

int mode_string = MODE_SAPDB;
char delimiter = ',';
char null_str[16] = "\"NULL\"";

/* Oh my gosh, is there a better way to do this? */
#define FPRINTF(a, b, c) \
	if (mode_string == MODE_SAPDB) { \
		fprintf(a, "\""b"\"", c); \
	} else if (mode_string == MODE_PGSQL) { \
		fprintf(a, b, c); \
	}
#define FPRINTF2(a, b) \
	if (mode_string == MODE_SAPDB) { \
		fprintf(a, "\""b"\""); \
	} else if (mode_string == MODE_PGSQL) { \
		fprintf(a, b); \
	}

void escape_me(char *str)
{
	/* Shouldn't need a buffer bigger than this. */
	char buffer[4096] = "";
	int i = 0;
	int j = 0;
	int k = 0;

	/* Don't need to do anything for SAP DB. */
	if (mode_string == MODE_PGSQL) {
		strcpy(buffer, str);
		i = strlen(buffer);
		for (k = 0; k <= i; k++) {
			if (buffer[k] == '\\') {
				str[j++] = '\\';
			}
			str[j++] = buffer[k];
		}
	}
}

void print_timestamp(FILE *ofile, struct tm *date)
{
	if (mode_string == MODE_SAPDB) {
		fprintf(ofile, "\"%04d%02d%02d%02d%02d%02d000000\"",
			date->tm_year + 1900, date->tm_mon + 1, date->tm_mday,
			date->tm_hour, date->tm_min, date->tm_sec);
	} else if (mode_string == MODE_PGSQL) {
		fprintf(ofile, "%04d-%02d-%02d %02d:%02d:%02d",
			date->tm_year + 1900, date->tm_mon + 1, date->tm_mday,
			date->tm_hour, date->tm_min, date->tm_sec);
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
	char a_string[512];
	struct tm *tm1;
	time_t t1;
	char filename[1024] = "\0";

	srand(0);
	printf("Generating customer table data...\n");

	if (strlen(output_path) > 0) {
		strcpy(filename, output_path);
		strcat(filename, "/");
	}
	strcat(filename, CUSTOMER_DATA);
	output = fopen(filename, "w");
	if (output == NULL) {
		printf("cannot open %s\n", CUSTOMER_DATA);
		return;
	}

	for (i = 0; i < warehouses; i++) {
		for (j = 0; j < DISTRICT_CARDINALITY; j++) {
			for (k = 0; k < customers; k++) {
				/* c_id */
				FPRINTF(output, "%d", k + 1);
				fprintf(output, "%c", delimiter);

				/* c_d_id */
				FPRINTF(output, "%d", j + 1);
				fprintf(output, "%c", delimiter);

				/* c_w_id */
				FPRINTF(output, "%d", i + 1);
				fprintf(output, "%c", delimiter);

				/* c_first */
				get_a_string(a_string, 8, 16);
				escape_me(a_string);
				FPRINTF(output, "%s", a_string);
				fprintf(output, "%c", delimiter);

				/* c_middle */
				FPRINTF2(output, "OE");
				fprintf(output, "%c", delimiter);

				/* c_last Clause 4.3.2.7 */
				if (k < 1000) {
					get_c_last(a_string, k);
				} else {
					get_c_last(a_string,
						get_nurand(255, 0, 999));
				}
				escape_me(a_string);
				FPRINTF(output, "%s", a_string);
				fprintf(output, "%c", delimiter);

				/* c_street_1 */
				get_a_string(a_string, 10, 20);
				escape_me(a_string);
				FPRINTF(output, "%s", a_string);
				fprintf(output, "%c", delimiter);

				/* c_street_2 */
				get_a_string(a_string, 10, 20);
				escape_me(a_string);
				FPRINTF(output, "%s", a_string);
				fprintf(output, "%c", delimiter);

				/* c_city */
				get_a_string(a_string, 10, 20);
				escape_me(a_string);
				FPRINTF(output, "%s", a_string);
				fprintf(output, "%c", delimiter);

				/* c_state */
				get_l_string(a_string, 2, 2);
				FPRINTF(output, "%s", a_string);
				fprintf(output, "%c", delimiter);

				/* c_zip */
				get_n_string(a_string, 4, 4);
				FPRINTF(output, "%s11111", a_string);
				fprintf(output, "%c", delimiter);

				/* c_phone */
				get_n_string(a_string, 16, 16);
				FPRINTF(output, "%s", a_string);
				fprintf(output, "%c", delimiter);

				/* c_since */
				/*
				 * Milliseconds are not calculated.  This
				 * should also be the time when the data is
				 * loaded, I think.
				 */
				time(&t1);
				tm1 = localtime(&t1);
				print_timestamp(output, tm1);
				fprintf(output, "%c", delimiter);

				/* c_credit */
				if (get_percentage() < .10) {
					FPRINTF2(output, "BC");
				} else {
					FPRINTF2(output, "GC");
				}
				fprintf(output, "%c", delimiter);

				/* c_credit_lim */
				FPRINTF2(output, "50000.00");
				fprintf(output, "%c", delimiter);

				/* c_discount */
				FPRINTF(output, "0.%04d", get_random(5000));
				fprintf(output, "%c", delimiter);

				/* c_balance */
				FPRINTF2(output, "-10.00");
				fprintf(output, "%c", delimiter);

				/* c_ytd_payment */
				FPRINTF2(output, "10.00");
				fprintf(output, "%c", delimiter);

				/* c_payment_cnt */
				FPRINTF2(output, "1");
				fprintf(output, "%c", delimiter);

				/* c_delivery_cnt */
				FPRINTF2(output, "0");
				fprintf(output, "%c", delimiter);

				/* c_data */
				get_a_string(a_string, 300, 500);
				escape_me(a_string);
				FPRINTF(output, "%s", a_string);

				fprintf(output, "\n");
			}
		}
	}
	fclose(output);
	printf("Finished customer table data...\n");
	return;
}

/* Clause 4.3.3.1 */
void gen_districts()
{
	FILE *output;
	int i, j;
	char a_string[32];
	char filename[1024] = "\0";

	srand(0);
	printf("Generating district table data...\n");

	if (strlen(output_path) > 0) {
		strcpy(filename, output_path);
		strcat(filename, "/");
	}
	strcat(filename, DISTRICT_DATA);
	output = fopen(filename, "w");
	if (output == NULL) {
		printf("cannot open %s\n", DISTRICT_DATA);
		return;
	}

	for (i = 0; i < warehouses; i++) {
		for (j = 0; j < DISTRICT_CARDINALITY; j++) {
			/* d_id */
			FPRINTF(output, "%d", j + 1);
			fprintf(output, "%c", delimiter);

			/* d_w_id */
			FPRINTF(output, "%d", i + 1);
			fprintf(output, "%c", delimiter);

			/* d_name */
			get_a_string(a_string, 6, 10);
			escape_me(a_string);
			FPRINTF(output, "%s", a_string);
			fprintf(output, "%c", delimiter);

			/* d_street_1 */
			get_a_string(a_string, 10, 20);
			escape_me(a_string);
			FPRINTF(output, "%s", a_string);
			fprintf(output, "%c", delimiter);

			/* d_street_2 */
			get_a_string(a_string, 10, 20);
			escape_me(a_string);
			FPRINTF(output, "%s", a_string);
			fprintf(output, "%c", delimiter);

			/* d_city */
			get_a_string(a_string, 10, 20);
			escape_me(a_string);
			FPRINTF(output, "%s", a_string);
			fprintf(output, "%c", delimiter);

			/* d_state */
			get_l_string(a_string, 2, 2);
			FPRINTF(output, "%s", a_string);
			fprintf(output, "%c", delimiter);

			/* d_zip */
			get_n_string(a_string, 4, 4);
			FPRINTF(output, "%s11111", a_string);
			fprintf(output, "%c", delimiter);

			/* d_tax */
			FPRINTF(output, "0.%04d", get_random(2000));
			fprintf(output, "%c", delimiter);

			/* d_ytd */
			FPRINTF2(output, "30000.00");
			fprintf(output, "%c", delimiter);

			/* d_next_o_id */
			FPRINTF2(output, "3001");

			fprintf(output, "\n");
		}
	}
	fclose(output);
	printf("Finished district table data...\n");
	return;
}

/* Clause 4.3.3.1 */
void gen_history()
{
	FILE *output;
	int i, j, k;
	char a_string[32];
	struct tm *tm1;
	time_t t1;
	char filename[1024] = "\0";

	srand(0);
	printf("Generating history table data...\n");

	if (strlen(output_path) > 0) {
		strcpy(filename, output_path);
		strcat(filename, "/");
	}
	strcat(filename, HISTORY_DATA);
	output = fopen(filename, "w");
	if (output == NULL) {
		printf("cannot open %s\n", HISTORY_DATA);
		return;
	}

	for (i = 0; i < warehouses; i++) {
		for (j = 0; j < DISTRICT_CARDINALITY; j++) {
			for (k = 0; k < customers; k++) {
				/* h_c_id */
				FPRINTF(output, "%d", k + 1);
				fprintf(output, "%c", delimiter);

				/* h_c_d_id */
				FPRINTF(output, "%d", j + 1);
				fprintf(output, "%c", delimiter);

				/* h_c_w_id */
				FPRINTF(output, "%d", i + 1);
				fprintf(output, "%c", delimiter);

				/* h_d_id */
				FPRINTF(output, "%d", j + 1);
				fprintf(output, "%c", delimiter);

				/* h_w_id */
				FPRINTF(output, "%d", i + 1);
				fprintf(output, "%c", delimiter);

				/* h_date */
				/*
				 * Milliseconds are not calculated.  This
				 * should also be the time when the data is
				 * loaded, I think.
				 */
				time(&t1);
				tm1 = localtime(&t1);
				print_timestamp(output, tm1);
				fprintf(output, "%c", delimiter);

				/* h_amount */
				FPRINTF2(output, "10.00");
				fprintf(output, "%c", delimiter);

				/* h_data */
				get_a_string(a_string, 12, 24);
				escape_me(a_string);
				FPRINTF(output, "%s", a_string);

				fprintf(output, "\n");
			}
		}
	}
	fclose(output);
	printf("Finished history table data...\n");
	return;
}

/* Clause 4.3.3.1 */
void gen_items()
{
	FILE *output;
	int i;
	char a_string[64];
	int j;
	char filename[1024] = "\0";

	srand(0);
	printf("Generating item table data...\n");

	if (strlen(output_path) > 0) {
		strcpy(filename, output_path);
		strcat(filename, "/");
	}
	strcat(filename, ITEM_DATA);
	output = fopen(filename, "w");
	if (output == NULL) {
		printf("cannot open %s\n", ITEM_DATA);
		return;
	}

	for (i = 0; i < items; i++) {
		/* i_id */
		FPRINTF(output, "%d", i + 1);
		fprintf(output, "%c", delimiter);

		/* i_im_id */
		FPRINTF(output, "%d", get_random(9999) + 1);
		fprintf(output, "%c", delimiter);

		/* i_name */
		get_a_string(a_string, 14, 24);
		escape_me(a_string);
		FPRINTF(output, "%s", a_string);
		fprintf(output, "%c", delimiter);

		/* i_price */
		FPRINTF(output, "%0.2f",
			((double) get_random(9900) + 100.0) / 100.0);
		fprintf(output, "%c", delimiter);

		/* i_data */
		get_a_string(a_string, 26, 50);
		if (get_percentage() < .10)
		{
			j = get_random(strlen(a_string) - 8);
			strncpy(a_string + j, "ORIGINAL", 8);
		}
		escape_me(a_string);
		FPRINTF(output, "%s", a_string);

		fprintf(output, "\n");
	}
	fclose(output);
	printf("Finished item table data...\n");
	return;
}

/* Clause 4.3.3.1 */
void gen_new_orders()
{
	FILE *output;
	int i, j, k;
	char filename[1024] = "\0";

	srand(0);
	printf("Generating new-order table data...\n");

	if (strlen(output_path) > 0) {
		strcpy(filename, output_path);
		strcat(filename, "/");
	}
	strcat(filename, NEW_ORDER_DATA);
	output = fopen(filename, "w");
	if (output == NULL) {
		printf("cannot open %s\n", NEW_ORDER_DATA);
		return;
	}

	for (i = 0; i < warehouses; i++) {
		for (j = 0; j < DISTRICT_CARDINALITY; j++) {
			for (k = orders - new_orders; k < orders; k++) {
				/* no_o_id */
				FPRINTF(output, "%d", k + 1);
				fprintf(output, "%c", delimiter);

				/* no_d_id */
				FPRINTF(output, "%d", j + 1);
				fprintf(output, "%c", delimiter);

				/* no_w_id */
				FPRINTF(output, "%d", i + 1);

				fprintf(output, "\n");
			}
		}
	}
	fclose(output);
	printf("Finished new-order table data...\n");
	return;
}

/* Clause 4.3.3.1 */
void gen_orders()
{
	FILE *order, *order_line;
	int i, j, k, l;
	char a_string[32];
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

	srand(0);
	printf("Generating order and order-line table data...\n");

	if (strlen(output_path) > 0) {
		strcpy(filename, output_path);
		strcat(filename, "/");
	}
	strcat(filename, ORDER_DATA);
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
	order_line = fopen(filename, "w");
	if (order_line == NULL) {
		printf("cannot open %s\n", ORDER_LINE_DATA);
		return;
	}

	for (i = 0; i < warehouses; i++) {
		for (j = 0; j < DISTRICT_CARDINALITY; j++) {
			/*
			 * Create a random list of numbers from 1 to customers
			 * for o_c_id.
			 */
			head = (struct node_t *) malloc(sizeof(struct node_t));
			head->value = 1;
			head->next = NULL;
			for (k = 2; k <= customers; k++) {
				current = prev = head;

				/*
				 * Find a random place in the list to insert a
				 * number.
				 */
				iter = get_random(k - 1);
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
					/*
					 * Insert somewhere in the middle of
					 * the list.
					 */
					prev->next = new_node;
					new_node->next = current;
				}
				new_node->value = k;
			}

			for (k = 0; k < orders; k++) {
				/* o_id */
				FPRINTF(order, "%d", k + 1);
				fprintf(order, "%c", delimiter);

				/* o_d_id */
				FPRINTF(order, "%d", j + 1);
				fprintf(order, "%c", delimiter);

				/* o_w_id */
				FPRINTF(order, "%d", i + 1);
				fprintf(order, "%c", delimiter);

				/* o_c_id */
				current = head;
				head = head->next;
				FPRINTF(order, "%d", current->value);
				fprintf(order, "%c", delimiter);
				free(current);

				/* o_entry_d */
				/*
				 * Milliseconds are not calculated.  This
				 * should also be the time when the data is
				 * loaded, I think.
				 */
				time(&t1);
				tm1 = localtime(&t1);
				print_timestamp(order, tm1);
				fprintf(order, "%c", delimiter);

				/* o_carrier_id */
				if (k < 2101) {
					FPRINTF(order, "%d", get_random(9) + 1);
				} else {
					fprintf(order, "%s", null_str);
				}
				fprintf(order, "%c", delimiter);

				/* o_ol_cnt */
				o_ol_cnt = get_random(10) + 5;
				FPRINTF(order, "%d", o_ol_cnt);
				fprintf(order, "%c", delimiter);

				/* o_all_local */
				FPRINTF2(order, "1");

				fprintf(order, "\n");

				/*
				 * Generate data in the order-line table for
				 * this order.
				 */
				for (l = 0; l < o_ol_cnt; l++) {
					/* ol_o_id */
					FPRINTF(order_line, "%d", k + 1);
					fprintf(order_line, "%c", delimiter);

					/* ol_d_id */
					FPRINTF(order_line, "%d", j + 1);
					fprintf(order_line, "%c", delimiter);

					/* ol_w_id */
					FPRINTF(order_line, "%d", i + 1);
					fprintf(order_line, "%c", delimiter);

					/* ol_number */
					FPRINTF(order_line, "%d", l + 1);
					fprintf(order_line, "%c", delimiter);

					/* ol_i_id */
					FPRINTF(order_line, "%d",
						get_random(ITEM_CARDINALITY - 1) + 1);
					fprintf(order_line, "%c", delimiter);

					/* ol_supply_w_id */
					FPRINTF(order_line, "%d", i + 1);
					fprintf(order_line, "%c", delimiter);

					/* ol_delivery_d */
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
						fprintf(order_line, "%s",
							null_str);
					}
					fprintf(order_line, "%c", delimiter);

					/* ol_quantity */
					FPRINTF2(order_line, "5");
					fprintf(order_line, "%c", delimiter);

					/* ol_amount */
					if (k < 2101) {
						FPRINTF2(order_line, "0.00");
					} else {
						FPRINTF(order_line, "%f",
							(double) (get_random(999998) + 1) / 100.0);
					}
					fprintf(order_line, "%c", delimiter);

					/* ol_dist_info */
					get_l_string(a_string, 24, 24);
					FPRINTF(order_line, "%s", a_string);

					fprintf(order_line, "\n");
				}
			}
		}
	}
	fclose(order);
	fclose(order_line);
	printf("Finished order and order-line table data...\n");
	return;
}

/* Clause 4.3.3.1 */
void gen_stock()
{
	FILE *output;
	int i, j, k;
	char a_string[64];
	char filename[1024] = "\0";

	srand(0);
	printf("Generating stock table data...\n");

	if (strlen(output_path) > 0) {
		strcpy(filename, output_path);
		strcat(filename, "/");
	}
	strcat(filename, STOCK_DATA);
	output = fopen(filename, "w");
	if (output == NULL) {
		printf("cannot open %s\n", STOCK_DATA);
		return;
	}

	for (i = 0; i < warehouses; i++) {
		for (j = 0; j < items; j++) {
			/* s_i_id */
			FPRINTF(output, "%d", j + 1);
			fprintf(output, "%c", delimiter);

			/* s_w_id */
			FPRINTF(output, "%d", i + 1);
			fprintf(output, "%c", delimiter);

			/* s_quantity */
			FPRINTF(output, "%d", get_random(90) + 10);
			fprintf(output, "%c", delimiter);

			/* s_dist_01 */
			get_l_string(a_string, 24, 24);
			FPRINTF(output, "%s", a_string);
			fprintf(output, "%c", delimiter);

			/* s_dist_02 */
			get_l_string(a_string, 24, 24);
			FPRINTF(output, "%s", a_string);
			fprintf(output, "%c", delimiter);

			/* s_dist_03 */
			get_l_string(a_string, 24, 24);
			FPRINTF(output, "%s", a_string);
			fprintf(output, "%c", delimiter);

			/* s_dist_04 */
			get_l_string(a_string, 24, 24);
			FPRINTF(output, "%s", a_string);
			fprintf(output, "%c", delimiter);

			/* s_dist_05 */
			get_l_string(a_string, 24, 24);
			FPRINTF(output, "%s", a_string);
			fprintf(output, "%c", delimiter);

			/* s_dist_06 */
			get_l_string(a_string, 24, 24);
			FPRINTF(output, "%s", a_string);
			fprintf(output, "%c", delimiter);

			/* s_dist_07 */
			get_l_string(a_string, 24, 24);
			FPRINTF(output, "%s", a_string);
			fprintf(output, "%c", delimiter);

			/* s_dist_08 */
			get_l_string(a_string, 24, 24);
			FPRINTF(output, "%s", a_string);
			fprintf(output, "%c", delimiter);

			/* s_dist_09 */
			get_l_string(a_string, 24, 24);
			FPRINTF(output, "%s", a_string);
			fprintf(output, "%c", delimiter);

			/* s_dist_10 */
			get_l_string(a_string, 24, 24);
			FPRINTF(output, "%s", a_string);
			fprintf(output, "%c", delimiter);

			/* s_ytd */
			FPRINTF2(output, "0");
			fprintf(output, "%c", delimiter);

			/* s_order_cnt */
			FPRINTF2(output, "0");
			fprintf(output, "%c", delimiter);

			/* s_remote_cnt */
			FPRINTF2(output, "0");
			fprintf(output, "%c", delimiter);

			/* s_data */
			get_a_string(a_string, 26, 50);
			if (get_percentage() < .10) {
				k = get_random(strlen(a_string) - 8);
				strncpy(a_string + k, "ORIGINAL", 8);
			}
			escape_me(a_string);
			FPRINTF(output, "%s", a_string);

			fprintf(output, "\n");
		}
	}
	fclose(output);
	printf("Finished stock table data...\n");
	return;
}

/* Clause 4.3.3.1 */
void gen_warehouses()
{
	FILE *output;
	int i;
	char a_string[32];
	char filename[1024] = "\0";

	srand(0);
	printf("Generating warehouse table data...\n");

	if (strlen(output_path) > 0) {
		strcpy(filename, output_path);
		strcat(filename, "/");
	}
	strcat(filename, WAREHOUSE_DATA);

	output = fopen(filename, "w");
	if (output == NULL) {
		printf("cannot open %s\n", WAREHOUSE_DATA);
		return;
	}

	for (i = 0; i < warehouses; i++) {
		/* w_id */
		FPRINTF(output, "%d", i + 1);
		fprintf(output, "%c", delimiter);

		/* w_name */
		get_a_string(a_string, 6, 10);
		escape_me(a_string);
		FPRINTF(output, "%s", a_string);
		fprintf(output, "%c", delimiter);

		/* w_street_1 */
		get_a_string(a_string, 10, 20);
		escape_me(a_string);
		FPRINTF(output, "%s", a_string);
		fprintf(output, "%c", delimiter);

		/* w_street_2 */
		get_a_string(a_string, 10, 20);
		escape_me(a_string);
		FPRINTF(output, "%s", a_string);
		fprintf(output, "%c", delimiter);

		/* w_city */
		get_a_string(a_string, 10, 20);
		escape_me(a_string);
		FPRINTF(output, "%s", a_string);
		fprintf(output, "%c", delimiter);

		/* w_state */
		get_l_string(a_string, 2, 2);
		FPRINTF(output, "%s", a_string);
		fprintf(output, "%c", delimiter);

		/* w_zip */
		get_n_string(a_string, 4, 4);
		FPRINTF(output, "%s11111", a_string);
		fprintf(output, "%c", delimiter);

		/* w_tax */
		FPRINTF(output, "0.%04d", get_random(2000));
		fprintf(output, "%c", delimiter);

		/* w_ytd */
		FPRINTF2(output, "300000.00");

		fprintf(output, "\n");
	}
	fclose(output);
	printf("Finished warehouse table data...\n");
	return;
}

int main(int argc, char *argv[])
{
	FILE *p;
	char pwd[256];
	char cmd[256];

	/* For getoptlong(). */
	int c;

	init_common();

	if (argc < 2)
	{
		printf("Usage: %s -w # [-c #] [-i #] [-o #] [-s #] [-n #] [-d <str>]\n", argv[0]);
		printf("\n");
		printf("-w #\n");
		printf("\twarehouse cardinality\n");
		printf("-c #\n");
		printf("\tcustomer cardinality, default %d\n",
			CUSTOMER_CARDINALITY);
		printf("-i #\n");
		printf("\titem cardinality, default %d\n", ITEM_CARDINALITY);
		printf("-o #\n");
		printf("\torder cardinality, default %d\n", ORDER_CARDINALITY);
		printf("-n #\n");
		printf("\tnew-order cardinality, default %d\n",
			NEW_ORDER_CARDINALITY);
		printf("-d <path>\n");
		printf("\toutput path of data files\n");
		return 1;
	}

	/* Parse command line arguments. */
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{ "pgsql", no_argument, &mode_string, MODE_PGSQL },
			{ "sapdb", no_argument, &mode_string, MODE_SAPDB },
			{ 0, 0, 0, 0 }
		};

		c = getopt_long(argc, argv, "c:d:i:n:o:w:",
			long_options, &option_index); 
		if (c == -1) {
			break;
		}

		switch (c) {
		case 0:
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
		case 'w':
			warehouses = atoi(optarg);
			break;
		default:
			printf("?? getopt returned character code 0%o ??\n", c);
			return 2;
		}
	}

	if (warehouses == 0)
	{
		printf("-w must be used\n");
		return 3;
	}

	/* Set the correct delimiter. */
	if (mode_string == MODE_SAPDB) {
		delimiter = ',';
		strcpy(null_str, "\"NULL\"");
	} else if (mode_string == MODE_PGSQL) {
		delimiter = '\t';
		strcpy(null_str, "");
	}

	printf("warehouses = %d\n", warehouses);
	printf("districts = %d\n", DISTRICT_CARDINALITY);
	printf("customers = %d\n", customers);
	printf("items = %d\n", items);
	printf("orders = %d\n", orders);
	printf("stock = %d\n", items);
	printf("new_orders = %d\n", new_orders);
	printf("\n");

	printf("Generating data files for %d warehouse(s)...\n", warehouses);

	gen_items();
	gen_warehouses();
	gen_stock();
	gen_districts();
	gen_customers();
	gen_history();
	gen_orders();
	gen_new_orders();

	/*
	 * In my environment, I don't have enough /tmp space to put the data
	 * files in /tmp.
	 *
	 * Ensure the user hasn't supplied /tmp, otherwise we would be making
	 * cyclical symlinks.  If the user has supplied /tmp, it's safe to 
	 * fallback to our default location.
	 */
	if (strlen(output_path) > 0
		&& strcmp(output_path, "/tmp") != 0
		&& strcmp(output_path, "/tmp/") != 0)
	{
		strcpy(pwd, output_path);
	}
	else
	{
		p = popen("pwd", "r");
		fscanf(p, "%s", pwd);
	}

	printf("creating links in /tmp to data files...\n");

	sprintf(cmd, "ln -fs %s/%s /tmp/%s", pwd, ITEM_DATA, ITEM_DATA);
	popen(cmd, "r");

	sprintf(cmd, "ln -fs %s/%s /tmp/%s", pwd, WAREHOUSE_DATA,
		WAREHOUSE_DATA);
	popen(cmd, "r");

	sprintf(cmd, "ln -fs %s/%s /tmp/%s", pwd, STOCK_DATA, STOCK_DATA);
	popen(cmd, "r");

	sprintf(cmd, "ln -fs %s/%s /tmp/%s", pwd, DISTRICT_DATA, DISTRICT_DATA);
	popen(cmd, "r");

	sprintf(cmd, "ln -fs %s/%s /tmp/%s", pwd, CUSTOMER_DATA, CUSTOMER_DATA);
	popen(cmd, "r");

	sprintf(cmd, "ln -fs %s/%s /tmp/%s", pwd, HISTORY_DATA, HISTORY_DATA);
	popen(cmd, "r");

	sprintf(cmd, "ln -fs %s/%s /tmp/%s", pwd, ORDER_DATA, ORDER_DATA);
	popen(cmd, "r");

	sprintf(cmd, "ln -fs %s/%s /tmp/%s", pwd, ORDER_LINE_DATA,
		ORDER_LINE_DATA);
	popen(cmd, "r");

	sprintf(cmd, "ln -fs %s/%s /tmp/%s", pwd, NEW_ORDER_DATA,
		NEW_ORDER_DATA);
	popen(cmd, "r");

	return 0;
}
