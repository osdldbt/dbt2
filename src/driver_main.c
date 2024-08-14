/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 *
 * 5 august 2002
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "client_interface.h"
#include "common.h"
#include "db.h"
#include "driver.h"
#include "logging.h"
#ifdef DRIVER3
#include "client.h"
#endif /* DRIVER3 */

int perform_integrity_check = 0;

#ifdef DRIVER3
extern int dbms;
extern char dname[32];
extern int retry_delay;
extern char sname[SNAMELEN + 1];
#endif /* DRIVER3 */

int parse_arguments(int, char **);
void usage(char *);

int main(int argc, char *argv[]) {
	/* Initialize various components. */
	init_common();
	init_driver();

	if (parse_arguments(argc, argv) != OK) {
		usage(argv[0]);
		return 1;
	}

	create_pid_file(DRIVER_PID_FILENAME);

#ifdef DRIVER1
	if (init_logging() != OK || init_driver_logging() != OK) {
		printf("cannot initialize driver\n");
		return 1;
	};
	free(output_path);
#endif /* DRIVER1 */

	if (w_id_min == 0) {
		w_id_min = 1;
	}
	if (w_id_max == 0) {
		w_id_max = table_cardinality.warehouses;
	}

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
		printf("invalid transaction mix: -e %0.2f. -r %0.2f. -q %0.2f. -t "
			   "%0.2f. causes new-order mix of %0.2f.\n",
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
	printf("districts = %d\n",
		   table_cardinality.districts * table_cardinality.warehouses);
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

	printf("%12s %4s %9s %6s %8s\n", "transaction", "mix", "threshold",
		   "keying", "thinking");
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
		   transaction_mix.delivery_actual, transaction_mix.delivery_threshold,
		   key_time.delivery, think_time.delivery);
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
		printf("You used wrong parameters or something wrong with "
			   "database.\n");
		return 1;
	}

	start_driver();

	return 0;
}

int parse_arguments(int argc, char *argv[]) {
	int i;
	char *flag;

	if (argc < 9) {
		return ERROR;
	}

	for (i = 1; i < argc; i += 2) {
		if (strlen(argv[i]) < 2) {
			printf("invalid flag: %s\n", argv[i]);
			return ERROR;
		}
		flag = argv[i] + 1;
		if (strcmp(flag, "a") == 0) {
#ifdef DRIVER3
			if (strcmp(argv[i + 1], "cockroach") == 0) {
				dbms = DBMSCOCKROACH;
			} else if (strcmp(argv[i + 1], "odbc") == 0) {
				dbms = DBMSODBC;
			} else if (strcmp(argv[i + 1], "mysql") == 0) {
				dbms = DBMSMYSQL;
			} else if (strcmp(argv[i + 1], "pgsql") == 0) {
				dbms = DBMSLIBPQ;
			} else if (strcmp(argv[i + 1], "sqlite") == 0) {
				dbms = DBMSSQLITE;
			} else {
				printf("unrecognized dbms option: %s\n", argv[i + 1]);
				return ERROR;
			}
#else
#endif /* DRIVER3 */
		} else if (strcmp(flag, "b") == 0) {
#ifdef DRIVER3
			strcpy(dname, argv[i + 1]);
#else
#endif /* DRIVER3 */
		} else if (strcmp(flag, "d") == 0) {
#ifdef DRIVER3
			strncpy(sname, argv[i + 1], sizeof(sname));
#else
			set_client_hostname(argv[i + 1]);
#endif /* DRIVER3 */
#ifdef DRIVER3
#ifdef HAVE_LIBPQ
		} else if (strcmp(flag, "P") == 0) {
			extern char postmaster_port[32];
			strcpy(postmaster_port, argv[i + 1]);
#endif /* HAVE_LIBPQ */
#ifdef HAVE_MYSQL
			extern char dbt2_mysql_port[32];
			strcpy(dbt2_mysql_port, argv[i + 1]);
		} else if (strcmp(flag, "S") == 0) {
			extern char dbt2_mysql_socket[256];
			strncpy(dbt2_mysql_socket, argv[i + 1], sizeof(dbt2_mysql_socket));
#endif /* HAVE_MYSQL */
#if defined(HAVE_MYSQL) || defined(HAVE_ODBC)
		} else if (strcmp(flag, "u") == 0) {
			extern char dbt2_user[128];
			strncpy(dbt2_user, argv[i + 1], sizeof(dbt2_user));
#endif /* defined(HAVE_MYSQL) || defined(HAVE_ODBC) */
#endif /* DRIVER3 */
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
#ifdef DRIVER3
		} else if (strcmp(flag, "retry-delay") == 0) {
			retry_delay = atoi(argv[i + 1]);
#endif /* DRIVER3 */
		} else if (strcmp(flag, "sleep") == 0) {
			client_conn_sleep = atoi(argv[i + 1]);
		} else if (strcmp(flag, "spread") == 0) {
			spread = atoi(argv[i + 1]);
		} else if (strcmp(flag, "altered") == 0) {
			mode_altered = atoi(argv[i + 1]);
			if (mode_altered) {
				printf("altered mode detected\n");
			}
		} else if (strcmp(flag, "outdir") == 0) {
			int length = strlen(argv[i + 1]);
			output_path = malloc(sizeof(char) * (length + 1));
			if (output_path == NULL) {
				printf("error allocating output_path\n");
				return ERROR;
			}
			strcpy(output_path, argv[i + 1]);
			if (output_path[length] == '/') {
				output_path[length] = '\0';
			}
		} else if (strcmp(flag, "fpp") == 0) {
			fork_per_processor = atoi(argv[i + 1]);
		} else {
			printf("invalid flag: %s\n", argv[i]);
			return ERROR;
		}
	}

	if (output_path == NULL) {
		output_path = malloc(sizeof(char) * 2);
		if (output_path == NULL) {
			printf("error allocating output_path\n");
			return ERROR;
		}
		output_path[0] = '.';
		output_path[1] = '\0';
	}
	return OK;
}

void usage(char *name) {
	printf("%s is the DBT-2 remote terminal emulator\n\n", name);
	printf("Usage:\n");
	printf("  %s [OPTION]\n", name);
	printf("\nGeneral options:\n");
#ifdef DRIVER3
	printf("  -a <dbms>      cockroach|mysql|pgsql|yugabyte\n");
#endif /* DRIVER3 */
#if defined(DRIVER1) || defined(DRIVER2)
	printf("  -d <address>   client network address\n");
#endif /* defined(DRIVER1) || defined(DRIVER2) */
#if defined(DRIVER2) || defined(DRIVER3)
	printf("  -fpp #         the number of processes started per proccessor, "
		   "default 1\n");
#endif /* defined(DRIVER2) || defined(DRIVER3) */
	printf("  -l #           the duration of the run in seconds\n");
	printf("  -outdir <path> location of log files, default ./\n");
#if defined(DRIVER1) || defined(DRIVER2)
	printf("  -p #           client port, default %d\n", CLIENT_PORT);
	printf("  -sleep #       number of milliseconds to sleep between "
		   "opening client connections, default %d\n",
		   client_conn_sleep);
#endif /* defined(DRIVER1) || defined(DRIVER2) */
#ifdef DRIVER3
	printf("  -retry-delay # delay in milliseconds between database "
		   "connections, default %d\n",
		   retry_delay);
	printf("  -sleep #       number of milliseconds to sleep between opening "
		   "database connections, default %d\n",
		   client_conn_sleep);
#endif /* DRIVER3 */
	printf("  -z #           perform database integrity check\n");
	printf("\nPartitioning options:\n");
	printf("  -wmin #        lower warehouse id\n");
	printf("  -wmax #        upper warehouse id\n");
	printf("\nDatabase cardinality options:\n");
	printf("  -w #           warehouse cardinality, default 1\n");
	printf("  -c #           customer cardinality, default %d\n",
		   CUSTOMER_CARDINALITY);
	printf("  -i #           item cardinality, default %d\n", ITEM_CARDINALITY);
	printf("  -o #           order cardinality, default %d\n",
		   ORDER_CARDINALITY);
	printf("  -n #           new-order cardinality, default %d\n",
		   NEW_ORDER_CARDINALITY);
	printf("\nTransaction mix options:\n");
	printf("  -q %%           mix percentage of Payment transaction, default "
		   "%0.2f\n",
		   MIX_PAYMENT);
	printf("  -r %%           mix percentage of Order-Status transaction, "
		   "default %0.2f\n",
		   MIX_ORDER_STATUS);
	printf("  -e %%           mix percentage of Delivery transaction, default "
		   "%0.2f\n",
		   MIX_DELIVERY);
	printf("  -t %%           mix percentage of Stock-Level transaction, "
		   "default %0.2f\n",
		   MIX_STOCK_LEVEL);
	printf("\nKeying time options:\n");
	printf("  -ktd #         delivery keying time, default %d s\n",
		   KEY_TIME_DELIVERY);
	printf("  -ktn #         new-order keying time, default %d s\n",
		   KEY_TIME_NEW_ORDER);
	printf("  -kto #         order-status keying time, default %d s\n",
		   KEY_TIME_ORDER_STATUS);
	printf("  -ktp #         payment keying time, default %d s\n",
		   KEY_TIME_PAYMENT);
	printf("  -kts #         stock-level keying time, default %d s\n",
		   KEY_TIME_STOCK_LEVEL);
	printf("\nThinking time options:\n");
	printf("  -ttd #         delivery thinking time, default %d ms\n",
		   THINK_TIME_DELIVERY);
	printf("  -ttn #         new-order thinking time, default %d ms\n",
		   THINK_TIME_NEW_ORDER);
	printf("  -tto #         order-status thinking time, default %d ms\n",
		   THINK_TIME_ORDER_STATUS);
	printf("  -ttp #         payment thinking time, default %d ms\n",
		   THINK_TIME_PAYMENT);
	printf("  -tts #         stock-level thinking time, default %d ms\n",
		   THINK_TIME_STOCK_LEVEL);
	printf("\nAltered specification options:\n");
	printf("  -altered [0/1] random warehouse and district per transaction, "
		   "default: 0\n");
#ifdef DRIVER2
	printf("  -L #           limit the total number of terminals emulated\n");
#elif DRIVER3
	printf("  -L #           limit the total number of processes started\n");
#endif /* DRIVER2 */
	printf("  -spread #      fancy warehouse skipping trick for low i/o "
		   "runs\n");
	printf("  -tpw #         terminals started per warehouse, default 10\n");

#ifdef DRIVER3
#ifdef HAVE_ODBC
	printf("\nunixODBC options:\n");
	printf("  -d <db_name>   database connect string\n");
	printf("  -u <db user>\n");
	printf("  -z <db password>\n");
#endif /* HAVE_ODBC */
#ifdef HAVE_LIBPQ
	printf("\nlibpq (CockroachDB, PostgreSQL, YugabyteDB) options:\n");
	printf("  -b <dbname>    database name\n");
	printf("  -d <hostname>  database hostname\n");
	printf("  -P #           postmaster port\n");
#endif /* HAVE_LIBPQ */
#ifdef HAVE_MYSQL
	printf("\nMySQL options:\n");
	printf("  -d <db_name>   database name\n");
	printf("  -P #           MySQL port number to\n");
	printf("  -h <hostname>  MySQL hostname\n");
	printf("  -t <socket>    MySQL socket\n");
	printf("  -u <db user>\n");
	printf("  -z <db password>\n");
#endif /* HAVE_MYSQL */
#ifdef HAVE_SQLITE3
	printf("\nSQLite options:\n");
	printf("  -d <db_file>   path to database file\n");
#endif
#endif /* DRIVER3 */
}
