/*
 * driver.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
 *
 * 7 august 2002
 */

#ifndef _DRIVER_H_
#define _DRIVER_H_

#define MIX_DELIVERY 0.04
#define MIX_ORDER_STATUS 0.04
#define MIX_PAYMENT 0.43
#define MIX_STOCK_LEVEL 0.04

struct transaction_mix_t
{
	/*
	 * These are the numbers are the actual percentage a transaction is
	 * executed.
	 */
	double delivery_actual;
	double new_order_actual;
	double payment_actual;
	double order_status_actual;
	double stock_level_actual;

	/*
	 * These are the numbers checked against to determine the next
	 * transaction.
	 */
	double delivery_threshold;
	double new_order_threshold;
	double payment_threshold;
	double order_status_threshold;
	double stock_level_threshold;
};

int init_driver();
int recalculate_mix();
int set_client_hostname(char *addr);
int set_client_port(int port);
int set_duration(int seconds);
int set_table_cardinality(int table, int cardinality);
int set_terminals(int number);
int set_transaction_mix(int transaction, double mix);
int start_driver();

extern struct transaction_mix_t transaction_mix;
extern char hostname[32];
extern int port;
extern int terminals;
extern int duration;

#endif /* _DRIVER_H_ */
