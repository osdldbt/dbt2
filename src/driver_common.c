/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "driver.h"
#include "logging.h"

int client_port = CLIENT_PORT;

int terminals_per_warehouse = 0;

struct transaction_mix_t transaction_mix;
struct key_time_t key_time;
struct think_time_t think_time;

int init_driver() {
	terminals_per_warehouse = table_cardinality.districts;

	transaction_mix.delivery_actual = MIX_DELIVERY;
	transaction_mix.order_status_actual = MIX_ORDER_STATUS;
	transaction_mix.payment_actual = MIX_PAYMENT;
	transaction_mix.stock_level_actual = MIX_STOCK_LEVEL;

	key_time.delivery = KEY_TIME_DELIVERY;
	key_time.new_order = KEY_TIME_NEW_ORDER;
	key_time.order_status = KEY_TIME_ORDER_STATUS;
	key_time.payment = KEY_TIME_PAYMENT;
	key_time.stock_level = KEY_TIME_STOCK_LEVEL;

	think_time.delivery = THINK_TIME_DELIVERY;
	think_time.new_order = THINK_TIME_NEW_ORDER;
	think_time.order_status = THINK_TIME_ORDER_STATUS;
	think_time.payment = THINK_TIME_PAYMENT;
	think_time.stock_level = THINK_TIME_STOCK_LEVEL;

	return OK;
}

int recalculate_mix() {
	/*
	 * Calculate the actual percentage that the New-Order transaction will
	 * be execute.
	 */
	transaction_mix.new_order_actual =
			1.0 - (transaction_mix.delivery_actual +
				   transaction_mix.order_status_actual +
				   transaction_mix.payment_actual +
				   transaction_mix.stock_level_actual);

	if (transaction_mix.new_order_actual < 0.0) {
		LOG_ERROR_MESSAGE(
				"invalid transaction mix. d %0.1f. o %0.1f. p %0.1f. s %0.1f. "
				"n %0.1f.\n",
				transaction_mix.delivery_actual,
				transaction_mix.order_status_actual,
				transaction_mix.payment_actual,
				transaction_mix.stock_level_actual,
				transaction_mix.new_order_actual);
		return ERROR;
	}

	/* Calculate the thresholds of each transaction. */
	transaction_mix.new_order_threshold = transaction_mix.new_order_actual;
	transaction_mix.payment_threshold = transaction_mix.new_order_threshold +
										transaction_mix.payment_actual;
	transaction_mix.order_status_threshold =
			transaction_mix.payment_threshold +
			transaction_mix.order_status_actual;
	transaction_mix.delivery_threshold =
			transaction_mix.order_status_threshold +
			transaction_mix.delivery_actual;
	transaction_mix.stock_level_threshold = transaction_mix.delivery_threshold +
											transaction_mix.stock_level_actual;

	return OK;
}

int set_client_hostname(char *addr) {
	strncpy(hostname, addr, HOSTNAMELEN);
	printf("connecting to client at '%s'\n", hostname);
	fflush(stdout);
	return OK;
}

int set_client_port(int port) {
	client_port = port;
	printf("connecting to client port at '%d'\n", client_port);
	fflush(stdout);
	return OK;
}

int set_duration(int seconds) {
	duration = seconds;
	return OK;
}

int set_table_cardinality(int table, int cardinality) {
	switch (table) {
	case TABLE_WAREHOUSE:
		table_cardinality.warehouses = cardinality;
		break;
	case TABLE_CUSTOMER:
		table_cardinality.customers = cardinality;
		break;
	case TABLE_ITEM:
		table_cardinality.items = cardinality;
		break;
	case TABLE_ORDER:
		table_cardinality.orders = cardinality;
		break;
	case TABLE_NEW_ORDER:
		table_cardinality.new_orders = cardinality;
		break;
	default:
		return ERROR;
	}

	return OK;
}

int set_transaction_mix(int transaction, double mix) {
	switch (transaction) {
	case DELIVERY:
		transaction_mix.delivery_actual = mix;
		break;
	case NEW_ORDER:
		transaction_mix.new_order_actual = mix;
		break;
	case ORDER_STATUS:
		transaction_mix.order_status_actual = mix;
		break;
	case PAYMENT:
		transaction_mix.payment_actual = mix;
		break;
	case STOCK_LEVEL:
		transaction_mix.stock_level_actual = mix;
		break;
	default:
		return ERROR;
	}
	return OK;
}
