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

	if (argc < 5)
	{
		printf("usage: %s -d <connect string> -t <transaction> [-w w_id_max]\n",
			argv[0]);
		printf("  <transaction>:  d - Delivery\n");
		printf("                  n - New-Order\n");
		printf("                  o - Order-Status\n");
		printf("                  p - Payment\n");
		printf("                  s - Stock-Level\n");
		return 1;
	}

	w_id_max = 1;
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
			w_id_max = atoi(argv[i + 1]);
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

	init_common();
	bzero(&odbc_data, sizeof(union odbc_transaction_t));
	srand(time(NULL));
#ifdef ODBC
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
				get_random(w_id_max) + 1);
			break;
		case NEW_ORDER:
			generate_input_data(NEW_ORDER, (void *) &odbc_data.new_order,
				get_random(w_id_max) + 1);
			break;
		case ORDER_STATUS:
			generate_input_data(ORDER_STATUS,
				(void *) &odbc_data.order_status,
				get_random(w_id_max) + 1);
			break;
		case PAYMENT:
			generate_input_data(PAYMENT, (void *) &odbc_data.payment,
				get_random(w_id_max) + 1);
			break;
		case STOCK_LEVEL:
			generate_input_data2(STOCK_LEVEL, (void *) &odbc_data.stock_level,
				get_random(w_id_max) + 1, get_random(D_ID_MAX) + 1);
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
