/*
 * transaction_test.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
 *
 * 24 june 2002
 */

#include <stdio.h>
#include <string.h>
#include <common.h>
#include <logging.h>
#include <transaction_data.h>
#include <input_data_generator.h>
#include <db.h>
#ifdef ODBC
#include <odbc_common.h>
#endif /* ODBC */
#include <client_interface.h>

union txn_data_t
{
	struct delivery_t delivery;
	struct new_order_t new_order;
	struct order_status_t order_status;
	struct payment_t payment;
	struct stock_level_t stock_level;
};

int main(int argc, char *argv[])
{
	int i;
	char sname[32] = "";
	int transaction = -1;
	struct odbc_context_t odbcc;
	union txn_data_t txn_data;
	union odbc_transaction_t odbc_data;

	int port = 0;
	int sockfd;
	struct client_transaction_t client_txn;

	init_common();
	init_logging();

	if (argc < 3)
	{
		printf("usage: %s -d <connect string> -t d/n/o/p/s [-w #] [-c #] [-i #] [-o #] [-n #] [-p #]\n",
			argv[0]);
		printf("\n");
		printf("-d <connect string>\n");
		printf("\tdatabase connect string\n");
		printf("-t d/n/o/p/s\n");
		printf("\td = Delivery. n = New-Order. o = Order-Status\n");
		printf("\tp = Payment. s = Stock-Level\n");
		printf("-w #\n");
		printf("\tnumber of warehouses, default 1\n");
		printf("-c #\n");
		printf("\tcustomer cardinality, default %d\n", CUSTOMER_CARDINALITY);
		printf("-i #\n");
		printf("\titem cardinality, default %d\n", ITEM_CARDINALITY);
		printf("-o #\n");
		printf("\torder cardinality, default %d\n", ORDER_CARDINALITY);
		printf("-n #\n");
		printf("\tnew-order cardinality, default %d\n", NEW_ORDER_CARDINALITY);
		printf("-p #\n");
		printf("\tport of client program, if used -d takes the address\n");
		printf("\tof the client program\n");
		return 1;
	}

	for (i = 1; i < argc; i += 2)
	{
		if (strlen(argv[i]) != 2)
		{
			printf("invalid flag: %s\n", argv[i]);
			return 2;
		}
		if (argv[i][1] == 'd')
		{
			strcpy(sname, argv[i + 1]);
		}
		else if (argv[i][1] == 't')
		{
			if (argv[i + 1][0] == 'd')
			{
				transaction = DELIVERY;
			}
			else if (argv[i + 1][0] == 'n')
			{
				transaction = NEW_ORDER;
			}
			else if (argv[i + 1][0] == 'o')
			{
				transaction = ORDER_STATUS;
			}
			else if (argv[i + 1][0] == 'p')
			{
				transaction = PAYMENT;
			}
			else if (argv[i + 1][0] == 's')
			{
				transaction = STOCK_LEVEL;
			}
			else
			{
				printf("unknown transaction: %s\n", argv[i + 1]);
				return 3;
			}
		}
		else if (argv[i][1] == 'w')
		{
			table_cardinality.warehouses = atoi(argv[i + 1]);
		}
		else if (argv[i][1] == 'c')
		{
			table_cardinality.customers = atoi(argv[i + 1]);
		}
		else if (argv[i][1] == 'i')
		{
			table_cardinality.items = atoi(argv[i + 1]);
		}
		else if (argv[i][1] == 'o')
		{
			table_cardinality.orders = atoi(argv[i + 1]);
		}
		else if (argv[i][1] == 'n')
		{
			table_cardinality.new_orders = atoi(argv[i + 1]);
		}
		else if (argv[i][1] == 'p')
		{
			port = atoi(argv[i + 1]);
		}
		else
		{
			printf("invalid flag: %s\n", argv[i]);
			return 2;
		}
	}

	if (strlen(sname) == 0)
	{
		printf("-d flag was not used.\n");
		return 4;
	}

	if (transaction == -1)
	{
		printf("-t flag was not used.\n");
		return 5;
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

	srand(time(NULL));

	/* Generate input data. */
	bzero(&txn_data, sizeof(union txn_data_t));
	switch (transaction)
	{
		case DELIVERY:
			generate_input_data(DELIVERY, (void *) &txn_data.delivery,
				get_random(table_cardinality.warehouses) + 1);
			break;
		case NEW_ORDER:
			generate_input_data(NEW_ORDER, (void *) &txn_data.new_order,
				get_random(table_cardinality.warehouses) + 1);
			break;
		case ORDER_STATUS:
			generate_input_data(ORDER_STATUS, (void *) &txn_data.order_status,
				get_random(table_cardinality.warehouses) + 1);
			break;
		case PAYMENT:
			generate_input_data(PAYMENT, (void *) &txn_data.payment,
				get_random(table_cardinality.warehouses) + 1);
			break;
		case STOCK_LEVEL:
			generate_input_data2(STOCK_LEVEL, (void *) &txn_data.stock_level,
				get_random(table_cardinality.warehouses) + 1,
				get_random(table_cardinality.districts) + 1);
			break;
	}

	if (port == 0)
	{
		/* Process transaction by connecting directly to the database. */
		printf("connecting directly to the database...\n");
#ifdef ODBC
		bzero(&odbc_data, sizeof(union odbc_transaction_t));
		odbc_init(sname, DB_USER, DB_PASS);
		if (odbc_connect(&odbcc) != OK)
		{
			return 6;
		}
		memcpy(&odbc_data, &txn_data, sizeof(union txn_data_t));
		process_transaction(transaction, &odbcc, (void *) &odbc_data);
		memcpy(&txn_data, &odbc_data, sizeof(union txn_data_t));
		odbc_disconnect(&odbcc);
#endif /* ODBC */
	}
	else
	{
		/* Process transaction by connecting to the client program. */
		printf("connecting to client program on port %d...\n", port);

		sockfd = connect_to_client(sname, port);
		if (sockfd > 0)
		{
			printf("connected to client\n");
		}

		client_txn.transaction = transaction;
		memcpy(&client_txn.transaction_data, &txn_data,
			sizeof(union txn_data_t));
		printf("sending transaction data...\n");
		if (send_transaction_data(sockfd, &client_txn) != OK)
		{
			printf("send_transaction_data() error\n");
			return 7;
		}

		printf("receiving transaction data...\n");
		if (receive_transaction_data(sockfd, &client_txn) != OK)
		{
			printf("receive_transaction_data() error\n");
			return 8;
		}

		memcpy(&txn_data, &client_txn.transaction_data,
			sizeof(union txn_data_t));
	}

	dump(stdout, transaction, (void *) &txn_data);
	printf("\ndone.\n");

	return 0;
}
