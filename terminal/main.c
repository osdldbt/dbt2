/*
 * main.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 *
 * 5 august 2002
 */

#include <stdlib.h>
#include <common.h>
#include <client_interface.h>
#include <driver.h>

int parse_arguments(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	init_common();
	init_driver();

	if (parse_arguments(argc, argv) != OK)
	{
		printf("usage: %s -d <address> -w # -u # -l # [-p #] [-c #] [-i #] [-o #] [-s #] [-n #] [-q %] [-r %] [-e %] [-t %]\n",
			argv[0]);
		printf("\n");
		printf("-d <address>\n");
		printf("\tnetwork address where client program is running\n");
		printf("-p #\n");
		printf("\tclient port, default %d\n", CLIENT_PORT);
		printf("\n");
		printf("-u #\n");
		printf("\tthe number of terminals to emulate\n");
		printf("-l #\n");
		printf("\tthe duration of the run in seconds\n");
		printf("\n");
		printf("-w #\n");
		printf("\tnumber of warehouses\n");
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
		printf("\n");
		printf("-q %\n");
		printf("\tmix percentage of Payment transaction, default %0.2f\n",
			MIX_PAYMENT);
		printf("-r %\n");
		printf("\tmix percentage of Order-Status transaction, default %0.2f\n",
			MIX_ORDER_STATUS);
		printf("-e %\n");
		printf("\tmix percentage of Delivery transaction, default %0.2f\n",
			MIX_DELIVERY);
		printf("-t %\n");
		printf("\tmix percentage of Stock-Level transaction, default %0.2f\n",
			MIX_STOCK_LEVEL);
		return 1;
	}

	if (recalculate_mix() != OK)
	{
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
	printf("stock = %d\n", table_cardinality.stock);
	printf("new-orders = %d\n", table_cardinality.new_orders);
	printf("\n");

	/* Double check the transaction mix. */
	printf("transaction mix:\n");
	printf("new-order mix %0.2f\n", transaction_mix.new_order_actual);
	printf("payment mix %0.2f\n", transaction_mix.payment_actual);
	printf("order-status mix %0.2f\n", transaction_mix.order_status_actual);
	printf("delivery mix %0.2f\n", transaction_mix.delivery_actual);
	printf("stock-level mix %0.2f\n", transaction_mix.stock_level_actual);
	printf("\n");

	/* Double check the transaction threshold. */
	printf("transaction thresholds:\n");
	printf("new-order threshold %0.2f\n", transaction_mix.new_order_threshold);
	printf("payment threshold %0.2f\n", transaction_mix.payment_threshold);
	printf("order-status threshold %0.2f\n",
		transaction_mix.order_status_threshold);
	printf("delivery threshold %0.2f\n", transaction_mix.delivery_threshold);
	printf("stock-level threshold %0.2f\n",
		transaction_mix.stock_level_threshold);
	printf("\n");

	start_driver(terminals);

	return 0;
}

int parse_arguments(int argc, char *argv[])
{
	int i;

	if (argc < 6)
	{
		return ERROR;
	}

	for (i = 1; i < argc; i += 2)
	{
		if (strlen(argv[i]) != 2)
		{
			printf("invalid flag: %s\n", argv[i]);
			return ERROR;
		}
		if (argv[i][1] == 'd')
		{
			set_client_hostname(argv[i + 1]);
		}
		else if (argv[i][1] == 'p')
		{
			set_client_port(atoi(argv[i + 1]));
		}
		else if (argv[i][1] == 'u')
		{
			set_terminals(atoi(argv[i + 1]));
		}
		else if (argv[i][1] == 'l')
		{
			set_duration(atoi(argv[i + 1]));
		}
		else if (argv[i][1] == 'w')
		{
			set_table_cardinality(TABLE_WAREHOUSE, atoi(argv[i + 1]));
		}
		else if (argv[i][1] == 'c')
		{
			set_table_cardinality(TABLE_CUSTOMER, atoi(argv[i + 1]));
		}
		else if (argv[i][1] == 'i')
		{
			set_table_cardinality(TABLE_ITEM, atoi(argv[i + 1]));
		}
		else if (argv[i][1] == 'o')
		{
			set_table_cardinality(TABLE_ORDER, atoi(argv[i + 1]));
		}
		else if (argv[i][1] == 's')
		{
			set_table_cardinality(TABLE_STOCK, atoi(argv[i + 1]));
		}
		else if (argv[i][1] == 'n')
		{
			set_table_cardinality(TABLE_NEW_ORDER, atoi(argv[i + 1]));
		}
		else if (argv[i][1] == 'q')
		{
			set_transaction_mix(PAYMENT, atof(argv[i + 1]));
		}
		else if (argv[i][1] == 'r')
		{
			set_transaction_mix(ORDER_STATUS, atof(argv[i + 1]));
		}
		else if (argv[i][1] == 'e')
		{
			set_transaction_mix(DELIVERY, atof(argv[i + 1]));
		}
		else if (argv[i][1] == 't')
		{
			set_transaction_mix(STOCK_LEVEL, atof(argv[i + 1]));
		}
		else
		{
			printf("invalid flag: %s\n", argv[i]);
			return ERROR;
		}
	}

	return OK;
}
