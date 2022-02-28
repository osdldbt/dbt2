/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Jenny Zhang & Open Source Development Lab, Inc.
 *               2002-2022 Mark Wong
 *
 * 5 august 2002
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <common.h>
#include <logging.h>
#include <client_interface.h>
#include <driver.h>
#include <unistd.h>
#ifdef STANDALONE
#include <db_threadpool.h>

char sname[32] = "";
int exiting = 0;
#endif /* STANDALONE */

#ifdef HAVE_LIBPQ
char postmaster_port[32];
#endif /* HAVE_LIBPQ */

#ifdef HAVE_MYSQL
char dbt2_mysql_port[32];
#endif /* HAVE_MYSQL */

int perform_integrity_check = 0;

int parse_arguments(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	/* Initialize various components. */
	init_common();
	init_driver();

	if (parse_arguments(argc, argv) != OK) {
		printf("usage: %s -d <address> -wmin # -wmax # -l # [-w #] [-p #] [-c #] [-i #] [-o #] [-n #] [-q %%] [-r %%] [-e %%] [-t %%] [-altered 0] [-spread #] [-z]",
			argv[0]);
#ifdef STANDALONE
#ifdef HAVE_LIBPQ
		printf(" -z #");
#endif /* HAVE_LIBPQ */
#endif /* STANDALONE */
		printf("\n\n");
#ifdef STANDALONE
		printf("-dbname <connect_string>\n");
		printf("\tdatabase connect string\n");
#ifdef HAVE_LIBPQ
		printf("-z #\n");
		printf("\tpostmaster listener port\n");
#endif /* HAVE_LIBPQ */
#ifdef HAVE_MYSQL
		printf("-z #\n");
		printf("\tmysql server listener port\n");
#endif /* HAVE_MYSQL */
#else /* STANDALONE */
		printf("-d <address>\n");
		printf("\tnetwork address where client program is running\n");
		printf("-p #\n");
		printf("\tclient port, default %d\n", CLIENT_PORT);
#endif /* STANDALONE */
		printf("\n");
		printf("-L #\n");
		printf("\tlimit the total number of terminals emulated\n");
		printf("-l #\n");
		printf("\tthe duration of the run in seconds\n");
		printf("\n");
		printf("-wmin #\n");
		printf("\tlower warehouse id\n");
		printf("-wmax #\n");
		printf("\tupper warehouse id\n");
		printf("-w #\n");
		printf("\twarehouse cardinality, default 1\n");
		printf("-c #\n");
		printf("\tcustomer cardinality, default %d\n", CUSTOMER_CARDINALITY);
		printf("-i #\n");
		printf("\titem cardinality, default %d\n", ITEM_CARDINALITY);
		printf("-o #\n");
		printf("\torder cardinality, default %d\n", ORDER_CARDINALITY);
		printf("-n #\n");
		printf("\tnew-order cardinality, default %d\n", NEW_ORDER_CARDINALITY);
		printf("\n");
		printf("-q %%\n");
		printf("\tmix percentage of Payment transaction, default %0.2f\n",
				MIX_PAYMENT);
		printf("-r %%\n");
		printf("\tmix percentage of Order-Status transaction, default %0.2f\n",
				MIX_ORDER_STATUS);
		printf("-e %%\n");
		printf("\tmix percentage of Delivery transaction, default %0.2f\n",
				MIX_DELIVERY);
		printf("-t %%\n");
		printf("\tmix percentage of Stock-Level transaction, default %0.2f\n",
				MIX_STOCK_LEVEL);
		printf("\n");
		printf("-ktd #\n");
		printf("\tdelivery keying time, default %d s\n", KEY_TIME_DELIVERY);
		printf("-ktn #\n");
		printf("\tnew-order keying time, default %d s\n", KEY_TIME_NEW_ORDER);
		printf("-kto #\n");
		printf("\torder-status keying time, default %d s\n",
				KEY_TIME_ORDER_STATUS);
		printf("-ktp #\n");
		printf("\tpayment keying time, default %d s\n", KEY_TIME_PAYMENT);
		printf("-kts #\n");
		printf("\tstock-level keying time, default %d s\n",
				KEY_TIME_STOCK_LEVEL);
		printf("-ttd #\n");
		printf("\tdelivery thinking time, default %d ms\n",
				THINK_TIME_DELIVERY);
		printf("-ttn #\n");
		printf("\tnew-order thinking time, default %d ms\n",
				THINK_TIME_NEW_ORDER);
		printf("-tto #\n");
		printf("\torder-status thinking time, default %d ms\n",
				THINK_TIME_ORDER_STATUS);
		printf("-ttp #\n");
		printf("\tpayment thinking time, default %d ms\n", THINK_TIME_PAYMENT);
		printf("-tts #\n");
		printf("\tstock-level thinking time, default %d ms\n",
				THINK_TIME_STOCK_LEVEL);
		printf("\n");
		printf("-tpw #\n");
		printf("\tterminals started per warehouse, default 10\n");

		printf("\n");
		printf("-altered [0/1]\n");
		printf("\trandom warehouse and district per transaction, default: 0\n");
		printf("-sleep #\n");
		printf("\tnumber of milliseconds to sleep between terminal creation\n");
		printf("-spread #\n");
		printf("\tfancy warehouse skipping trick for low i/o runs\n");

		printf("-z #\n");
		printf("\tperform database integrity check\n");
#ifdef STANDALONE
		printf("\nDriver is in STANDALONE mode.\n");
#endif /* STANDALONE */

		return 1;
	}
	create_pid_file();

	if(init_logging() != OK || init_driver_logging() != OK) {
		printf("cannot init driver\n");
		return 1;
	};

	/* Sanity check on the parameters. */
	if (w_id_min > w_id_max) {
		printf("wmin cannot be larger than wmax\n");
		return 1;
	}
	if (w_id_max > table_cardinality.warehouses) {
		printf("wmax cannot be larger than w\n");
		return 1;
	}

	if (recalculate_mix() != OK) {
		printf("invalid transaction mix: -e %0.2f. -r %0.2f. -q %0.2f. -t %0.2f. causes new-order mix of %0.2f.\n",
				transaction_mix.delivery_actual,
				transaction_mix.order_status_actual,
				transaction_mix.payment_actual,
				transaction_mix.stock_level_actual,
				transaction_mix.new_order_actual);
		return 1;
	}

	/* Double check database table cardinality. */
	printf("\n");
	printf("database table cardinalities:\n");
	printf("warehouses = %d\n", table_cardinality.warehouses);
	printf("districts = %d\n", table_cardinality.districts);
	printf("customers = %d\n", table_cardinality.customers);
	printf("items = %d\n", table_cardinality.items);
	printf("orders = %d\n", table_cardinality.orders);
	printf("stock = %d\n", table_cardinality.items);
	printf("new-orders = %d\n", table_cardinality.new_orders);
	printf("\n");

	/*
	 * Double check the transaction mix, threshold, keying time, and thinking
	 * time.
	 */

	printf("%12s %4s %9s %6s %8s\n",
			"transaction", "mix", "threshold", "keying", "thinking");
	printf("%-12s %4.2f %9.2f %6d %8d\n", "new order",
			transaction_mix.new_order_actual,
			transaction_mix.new_order_threshold, key_time.new_order,
			think_time.new_order);
	printf("%-12s %4.2f %9.2f %6d %8d\n", "payment",
			transaction_mix.payment_actual, transaction_mix.payment_threshold,
			key_time.payment, think_time.payment);
	printf("%-12s %4.2f %9.2f %6d %8d\n", "order status",
			transaction_mix.order_status_actual,
			transaction_mix.order_status_threshold, key_time.order_status,
			think_time.order_status);
	printf("%-12s %4.2f %9.2f %6d %8d\n", "delivery",
			transaction_mix.delivery_actual,
			transaction_mix.delivery_threshold, key_time.delivery,
			think_time.delivery);
	printf("%-12s %4.2f %9.2f %6d %8d\n", "stock level",
			transaction_mix.stock_level_actual,
			transaction_mix.stock_level_threshold, key_time.stock_level,
			think_time.stock_level);
	printf("\n");

	printf("w_id range %d to %d\n", w_id_min, w_id_max);
	printf("%d terminals per warehouse\n", terminals_per_warehouse);
	printf("%d second steady state duration\n", duration);
	printf("\n");

	if (perform_integrity_check && integrity_terminal_worker() != OK) {
	   printf("You used wrong parameters or something wrong with database.\n");
	   return 1;
	}

	start_driver();

	return 0;
}

int parse_arguments(int argc, char *argv[])
{
	int i;
	char *flag;

	if (argc < 9) {
		return ERROR;
	}

	for (i = 1; i < argc; i += 2) {
		if (strlen(argv[i]) < 2) {
			printf("invalid flag: %s\n", argv[i]);
			exit(1);
		}
		flag = argv[i] + 1;
		if (strcmp(flag, "d") == 0) {
			set_client_hostname(argv[i + 1]);
#ifdef HAVE_LIBPQ
		} else if (strcmp(flag, "z") == 0) {
			strcpy(postmaster_port, argv[i + 1]);
#endif /* HAVE_LIBPQ */
#ifdef HAVE_MYSQL
		} else if (strcmp(flag, "z") == 0) {
			strcpy(dbt2_mysql_port, argv[i + 1]);
#endif /* HAVE_MYSQL */
		} else if (strcmp(flag, "p") == 0) {
			set_client_port(atoi(argv[i + 1]));
		} else if (strcmp(flag, "L") == 0) {
			terminals_limit = atoi(argv[i + 1]);
			printf("emulated terminals limited to %d\n", terminals_limit);
		} else if (strcmp(flag, "l") == 0) {
			set_duration(atoi(argv[i + 1]));
		} else if (strcmp(flag, "w") == 0) {
			set_table_cardinality(TABLE_WAREHOUSE, atoi(argv[i + 1]));
		} else if (strcmp(flag, "c") == 0) {
			set_table_cardinality(TABLE_CUSTOMER, atoi(argv[i + 1]));
		} else if (strcmp(flag, "i") == 0) {
			set_table_cardinality(TABLE_ITEM, atoi(argv[i + 1]));
		} else if (strcmp(flag, "o") == 0) {
			set_table_cardinality(TABLE_ORDER, atoi(argv[i + 1]));
		} else if (strcmp(flag, "s") == 0) {
			set_table_cardinality(TABLE_STOCK, atoi(argv[i + 1]));
		} else if (strcmp(flag, "n") == 0) {
			set_table_cardinality(TABLE_NEW_ORDER, atoi(argv[i + 1]));
		} else if (strcmp(flag, "q") == 0) {
			set_transaction_mix(PAYMENT, atof(argv[i + 1]));
		} else if (strcmp(flag, "r") == 0) {
			set_transaction_mix(ORDER_STATUS, atof(argv[i + 1]));
		} else if (strcmp(flag, "e") == 0) {
			set_transaction_mix(DELIVERY, atof(argv[i + 1]));
		} else if (strcmp(flag, "t") == 0) {
			set_transaction_mix(STOCK_LEVEL, atof(argv[i + 1]));
		} else if (strcmp(flag, "z") == 0) {
			perform_integrity_check = 1;
		} else if (strcmp(flag, "wmin") == 0) {
			w_id_min = atoi(argv[i + 1]);
		} else if (strcmp(flag, "wmax") == 0) {
			w_id_max = atoi(argv[i + 1]);
		} else if (strcmp(flag, "ktd") == 0) {
			key_time.delivery = atoi(argv[i + 1]);
		} else if (strcmp(flag, "ktn") == 0) {
			key_time.new_order = atoi(argv[i + 1]);
		} else if (strcmp(flag, "kto") == 0) {
			key_time.order_status = atoi(argv[i + 1]);
		} else if (strcmp(flag, "ktp") == 0) {
			key_time.payment = atoi(argv[i + 1]);
		} else if (strcmp(flag, "kts") == 0) {
			key_time.stock_level = atoi(argv[i + 1]);
		} else if (strcmp(flag, "ttd") == 0) {
			think_time.delivery = atoi(argv[i + 1]);
		} else if (strcmp(flag, "ttn") == 0) {
			think_time.new_order = atoi(argv[i + 1]);
		} else if (strcmp(flag, "tto") == 0) {
			think_time.order_status = atoi(argv[i + 1]);
		} else if (strcmp(flag, "ttp") == 0) {
			think_time.payment = atoi(argv[i + 1]);
		} else if (strcmp(flag, "tts") == 0) {
			think_time.stock_level = atoi(argv[i + 1]);
		} else if (strcmp(flag, "tpw") == 0) {
			terminals_per_warehouse = atoi(argv[i + 1]);
		} else if (strcmp(flag, "sleep") == 0) {
			client_conn_sleep = atoi(argv[i + 1]);
		} else if (strcmp(flag, "spread") == 0) {
			spread = atoi(argv[i + 1]);
		} else if (strcmp(flag, "altered") == 0) {
			mode_altered = atoi(argv[i + 1]);
			if (mode_altered)
				printf("altered mode detected\n");
#ifdef STANDALONE
		} else if (strcmp(flag, "dbc") == 0) {
			db_connections = atoi(argv[i + 1]);
		} else if (strcmp(flag, "dbname") == 0) {
			strcpy(sname, argv[i + 1]);
#endif /* STANDALONE */
		} else if (strcmp(flag, "outdir") == 0) {
			strcpy(output_path, argv[i + 1]);
		} else {
			printf("invalid flag: %s\n", argv[i]);
			exit(1);
		}
	}

	return OK;
}
