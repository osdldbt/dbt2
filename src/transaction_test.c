/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002      Open Source Development Labs, Inc.
 *               2002-2022 Mark Wong
 *
 * 24 June 2002
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "common.h"
#include "client_interface.h"
#include "db.h"
#include "input_data_generator.h"
#include "logging.h"
#include "transaction_data.h"

#include "entropy.h"

char connect_str[32] = "";
int mode_altered = 0;

int main(int argc, char *argv[])
{
	int i;
	int dbms;
	int transaction = -1;
	struct db_context_t dbc;
	union transaction_data_t transaction_data;

	int port = 0;
	int sockfd;
	struct client_transaction_t client_txn;

	unsigned long long seed = 1;
	pcg64f_random_t rng;

	init_common();
	init_logging();

	if (argc < 3) {
		printf("usage: %s -a <dbms> -d <connect string> -t d/n/o/p/s [-w #] [-c #] [-i #] [-o #] [-n #] [-p #]",
			argv[0]);
		printf("\n\n");
		printf("-a <dbms>\n");
		printf("\tcockroach|mysql|pgsql|yugabyte\n");
		printf("-t (d|n|o|p|s)\n");
		printf("\td = Delivery, n = New-Order, o = Order-Status,\n");
		printf("\tp = Payment, s = Stock-Level\n");
		printf("-w #\n");
		printf("\tnumber of warehouses, default 1\n");
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
		printf("-p #\n");
		printf("\tport of client program, if -d is used, -d takes the address\n");
		printf("\tof the client program host system\n");
#ifdef ODBC
		printf("\nODBC:\n");
		printf("-d <connect string>\n");
		printf("\tdatabase connect string\n");
#endif /* ODBC */
#ifdef HAVE_LIBPQ
		printf("\nPostgreSQL (pgsql: Use env vars to set PGDATABASE etc.):\n");
		printf("-d <connect string>\n");
		printf("\tdatabase hostname\n");
#endif /* HAVE_LIBPQ */
		return 1;
	}

	for (i = 1; i < argc; i += 2) {
		if (strlen(argv[i]) != 2) {
			printf("invalid flag: %s\n", argv[i]);
			return 2;
		}
		if (argv[i][1] == 'a') {
			if (strcmp(argv[i + 1], "cockroach") == 0)
				dbms = DBMSCOCKROACH;
			else if (strcmp(argv[i + 1], "pgsql") == 0)
				dbms = DBMSLIBPQ;
			else {
				printf("unrecognized dbms option: %s", argv[i + 1]);
				exit(1);
			}
		} else if (argv[i][1] == 'd') {
			strcpy(connect_str, argv[i + 1]);
		} else if (argv[i][1] == 't') {
			if (argv[i + 1][0] == 'd') {
				transaction = DELIVERY;
			} else if (argv[i + 1][0] == 'n') {
				transaction = NEW_ORDER;
			} else if (argv[i + 1][0] == 'o') {
				transaction = ORDER_STATUS;
			} else if (argv[i + 1][0] == 'p') {
				transaction = PAYMENT;
			} else if (argv[i + 1][0] == 's') {
				transaction = STOCK_LEVEL;
			} else {
				printf("unknown transaction: %s\n",
					argv[i + 1]);
				return 3;
			}
		} else if (argv[i][1] == 'w') {
			table_cardinality.warehouses = atoi(argv[i + 1]);
		} else if (argv[i][1] == 'c') {
			table_cardinality.customers = atoi(argv[i + 1]);
		} else if (argv[i][1] == 'i') {
			table_cardinality.items = atoi(argv[i + 1]);
		} else if (argv[i][1] == 'o') {
			table_cardinality.orders = atoi(argv[i + 1]);
		} else if (argv[i][1] == 'n') {
			table_cardinality.new_orders = atoi(argv[i + 1]);
		} else if (argv[i][1] == 'p') {
			port = atoi(argv[i + 1]);
		} else {
			printf("invalid flag: %s\n", argv[i]);
			return 2;
		}
	}

	if (strlen(connect_str) == 0) {
		printf("-d flag was not used.\n");
		return 4;
	}

	if (transaction == -1) {
		printf("-t flag was not used.\n");
		return 5;
	}

	entropy_getbytes((void *) &seed, sizeof(seed));
	printf("seed = %llu\n", seed);

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

	pcg64f_srandom_r(&rng, seed);

	/* Generate input data. */
	memset(&transaction_data, 0, sizeof(union transaction_data_t));
	switch (transaction) {
	case DELIVERY:
		generate_input_data(&rng, DELIVERY,
				(void *) &transaction_data.delivery,
				(int) get_random(&rng, table_cardinality.warehouses) + 1);
		break;
	case NEW_ORDER:
		generate_input_data(&rng, NEW_ORDER,
				(void *) &transaction_data.new_order,
				(int) get_random(&rng, table_cardinality.warehouses) + 1);
		break;
	case ORDER_STATUS:
		generate_input_data(&rng, ORDER_STATUS,
				(void *) &transaction_data.order_status,
				(int) get_random(&rng, table_cardinality.warehouses) + 1);
		break;
	case PAYMENT:
		generate_input_data(&rng, PAYMENT, (void *) &transaction_data.payment,
				(int) get_random(&rng, table_cardinality.warehouses) + 1);
		break;
	case STOCK_LEVEL:
		generate_input_data2(&rng, STOCK_LEVEL,
				(void *) &transaction_data.stock_level,
				(int) get_random(&rng, table_cardinality.warehouses) + 1,
				(int) get_random(&rng, table_cardinality.districts) + 1);
		break;
	}

	if (port == 0) {
		/*
		 * Process transaction by connecting directly to the database.
		 */
		printf("connecting directly to the database...\n");
		switch(dbms) {
#ifdef HAVE_LIBPQ
		case DBMSCOCKROACH:
				db_init_cockroach(&dbc, "", connect_str, "");
				break;
		case DBMSLIBPQ:
				db_init_libpq(&dbc, "", connect_str, "");
				break;
#endif /* HAVE_LIBPQ */

#ifdef HAVE_MYSQL
		case DBMSMYSQL:
				/* TODO: Implement this for MySQL */
				printf("error: not implemented for mysql");
				exit(1);
				break;
#endif /* HAVE_MYSQL */

#ifdef HAVE_SQLITE3
		case DBMSSQLITE:
				db_init_sqlite(&dbc, connect_str);
				break;
#endif /* HAVE_SQLITE3 */

#ifdef HAVE_ODBC
		case DBMSODBC:
				rc = _db_init(sname, uname, auth);
				break;
#endif /* HAVE_ODBC */

		default:
				LOG_ERROR_MESSAGE("unrecognized dbms code: %d", dbms);
				return ERROR;
		}
		if ((*dbc.connect)(&dbc) != OK) {
			printf("cannot establish a database connection\n");
			return 6;
		}
		if (process_transaction(transaction, &dbc,
			(void *) &transaction_data) != OK) {
			disconnect_from_db(&dbc);
			printf("transaction failed\n");
			return 11;
		}
		disconnect_from_db(&dbc);
	} else {
		/* Process transaction by connecting to the client program. */
		printf("connecting to client program on port %d...\n", port);

		sockfd = connect_to_client(connect_str, port);
		if (sockfd > 0) {
			printf("connected to client\n");
		}

		client_txn.transaction = transaction;
		memcpy(&client_txn.transaction_data, &transaction_data,
			sizeof(union transaction_data_t));
		printf("sending transaction data...\n");
		if (send_transaction_data(sockfd, &client_txn) != OK) {
			printf("send_transaction_data() error\n");
			return 7;
		}

		printf("receiving transaction data...\n");
		if (receive_transaction_data(sockfd, &client_txn) != OK) {
			printf("receive_transaction_data() error\n");
			return 8;
		}

		memcpy(&transaction_data, &client_txn.transaction_data,
			sizeof(union transaction_data_t));
	}

	dump(stdout, transaction, (void *) &transaction_data);
	printf("\ndone.\n");

	return 0;
}
