/*
 * client_interface.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
 *
 * 5 august 2002
 */

#ifndef _CLIENT_INTERFACE_H_
#define _CLIENT_INTERFACE_H_

#include <transaction_data.h>

union transaction_data_t
{
	struct delivery_t delivery;
	struct new_order_t new_order;
	struct order_status_t order_status;
	struct payment_t payment;
	struct stock_level_t stock_level;
};

struct client_transaction_t
{
	int transaction;
	union transaction_data_t transaction_data;
};

int connect_to_client(char *addr, int port);
int receive_transaction_data(int s, struct client_transaction_t *client_data);
int send_transaction_data(int s, struct client_transaction_t *client_data);

#endif /* _CLIENT_INTERFACE_H_ */
