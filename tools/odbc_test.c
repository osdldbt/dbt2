/*
 * odbc_test.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
 *
 * 24 june 2002
 */

#include <stdio.h>
#include <common.h>
#include <transaction_data.h>
#include <input_data_generator.h>
#include <db.h>
#include <odbc_common.h>

int main(int argc, char *argv[])
{
	int rc;
	int i;
	char sname[32] = "";
	int transaction = -1;
	struct odbc_context_t odbcc;
	union odbc_transaction_t odbc_data;

	init_common();

	if (argc < 5)
	{
		printf("usage: %s -d <connect string> -t d/n/o/p/s [-w #] [-c #] [-i #] [-o #] [-s #] [-n #]\n",
			argv[0]);
		printf("\n");
		printf("-d <connect string>\n");
		printf("\tdatabase connect string\n");
		printf("-t d/n/o/p/s\n");
		printf("\td = Delivery. n = New-Order. o = Order-Status\n");
		printf("\tp = Payment. s = Stock-Level\n");
		printf("-w #\n");
		printf("\tNumber of warehouses,  default 1\n");
		printf("-c #\n");
		printf("\tcustomer cardinality, default %d\n", CUSTOMER_CARDINALITY);
		printf("-i #\n");
		printf("\titem cardinality, default %d\n", ITEM_CARDINALITY);
		printf("-o #\n");
		printf("\torder cardinality, default %d\n", ORDER_CARDINALITY);
		printf("-s #\n");
		printf("\tstock cardinality, default %d\n", STOCK_CARDINALITY);
		printf("-n #\n");
		printf("\tnew-order cardinality, default %d\n", NEW_ORDER_CARDINALITY);
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
		else if (argv[i][1] == 's')
		{
			table_cardinality.stock = atoi(argv[i + 1]);
		}
		else if (argv[i][1] == 'n')
		{
			table_cardinality.new_orders = atoi(argv[i + 1]);
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

	if (transaction == 0)
	{
		printf("-t flag was not used.\n");
		return 5;
	}

	/* Double check database table cardinality. */
	printf("warehouses = %d\n", table_cardinality.warehouses);
	printf("districts = %d\n", table_cardinality.districts);
	printf("customers = %d\n", table_cardinality.customers);
	printf("items = %d\n", table_cardinality.items);
	printf("orders = %d\n", table_cardinality.orders);
	printf("stock = %d\n", table_cardinality.stock);
	printf("new-orders = %d\n", table_cardinality.new_orders);

	srand(time(NULL));
#ifdef ODBC
	bzero(&odbc_data, sizeof(union odbc_transaction_t));
	odbc_init(sname, DB_USER, DB_PASS);
	if (odbc_connect(&odbcc) != OK)
	{
		return 6;
	}
#endif /* ODBC */
	switch (transaction)
	{
		case DELIVERY:
			generate_input_data(DELIVERY, (void *) &odbc_data.delivery,
				get_random(table_cardinality.warehouses) + 1);
			break;
		case NEW_ORDER:
			generate_input_data(NEW_ORDER, (void *) &odbc_data.new_order,
				get_random(table_cardinality.warehouses) + 1);
			break;
		case ORDER_STATUS:
			generate_input_data(ORDER_STATUS,
				(void *) &odbc_data.order_status,
				get_random(table_cardinality.warehouses) + 1);
			break;
		case PAYMENT:
			generate_input_data(PAYMENT, (void *) &odbc_data.payment,
				get_random(table_cardinality.warehouses) + 1);
			break;
		case STOCK_LEVEL:
			generate_input_data2(STOCK_LEVEL, (void *) &odbc_data.stock_level,
				get_random(table_cardinality.warehouses) + 1,
				get_random(table_cardinality.districts) + 1);
			break;
	}
	process_transaction(transaction, &odbcc, &odbc_data);
#ifdef ODBC
	odbc_disconnect(&odbcc);
#endif /* ODBC */
	dump(stdout, transaction, (void *) &odbc_data);

	printf("\ndone.\n");

	return 0;
}
