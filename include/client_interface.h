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

#ifdef ODBC
#include <odbc_common.h>
#endif /* ODBC */

struct client_transaction_t
{
	int transaction;
#ifdef ODBC
	union odbc_transaction_t transaction_data;
#endif /* ODBC */
};

int connect_to_client(char *addr, int port);
int receive_transaction_data(int s, struct client_transaction_t *client_data);
int send_transaction_data(int s, struct client_transaction_t *client_data);

#endif /* _CLIENT_INTERFACE_H_ */